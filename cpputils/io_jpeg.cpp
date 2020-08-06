#include "io_jpeg.h"
#include <cstdio>
#include <png.h>
#include "jpeglib.h"
#include <cstring>
using namespace std;
using namespace spes::image;

namespace spes::image::io
{
	bool jpeg_sig_check(s8* buff, u32 l)
	{
		constexpr u32 JPEG_SIG_LEN = 4;
		// TODO: any other choice to init a char array?
		constexpr s8 JPEG_SIG[] = {(s8)0xff, (s8)0xd8, (s8)0xff, (s8)0xe0};
		u32 len = l < JPEG_SIG_LEN ? l : JPEG_SIG_LEN;
		for (int i = 0; i < 4; ++i)
			if (buff[i] != JPEG_SIG[i]) return false;
		return true;
	}
	struct my_error_mgr {
		jpeg_error_mgr pub;
		jmp_buf env;
	};

	typedef struct my_error_mgr* my_error_ptr;
	void my_error_exit(j_common_ptr cinfo)
	{
		my_error_ptr myerr = (my_error_ptr)cinfo->err;
		(*cinfo->err->output_message) (cinfo);
		longjmp(myerr->env, 1);
	}
	
	image_t read_jpeg(const char* path)
	{
		jpeg_decompress_struct cinfo;
		my_error_mgr jerr;

		FILE* infile;
		JSAMPARRAY buffer;
		int row_stride;
		jmp_buf env;

		image_t im;

		if ((infile = fopen(path, "rb")) == NULL) {
			fprintf(stderr, "can't open %s\n", path);
			return im;
		}
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = my_error_exit;

		if (setjmp(jerr.env)) {
			jpeg_destroy_decompress(&cinfo);
			fclose(infile);
			return im;
		}
		jpeg_create_decompress(&cinfo);

		jpeg_stdio_src(&cinfo, infile);

		(void)jpeg_read_header(&cinfo, TRUE);
		im_buff buff(cinfo.image_width, cinfo.image_height, IMP_FMT_RGB);

		(void)jpeg_start_decompress(&cinfo);
		row_stride = cinfo.output_width * cinfo.output_components;

		buffer = (*cinfo.mem->alloc_sarray)
			((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

		while (cinfo.output_scanline < cinfo.output_height) {
			u8* dst = buff.buff + cinfo.output_scanline * row_stride;
			(void)jpeg_read_scanlines(&cinfo, buffer, 1);
			memcpy(dst, buffer[0], row_stride);
		}

		(void)jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);

		return image_transform(buff);
	}

	void write_jpeg(image_t& im, const char* path, int quality)
	{
		im_buff buff = image_transform(im, IMP_FMT_RGB);

		jpeg_compress_struct cinfo;
		struct jpeg_error_mgr jerr;

		FILE* outfile;
		JSAMPROW row_pointer[1];
		int row_stride;

		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_compress(&cinfo);

		if ((outfile = fopen(path, "wb")) == NULL) {
			fprintf(stderr, "can't open %s\n", path);
			return;
		}
		jpeg_stdio_dest(&cinfo, outfile);

		cinfo.image_width = buff.w;
		cinfo.image_height = buff.h;
		cinfo.input_components = 3;
		cinfo.in_color_space = JCS_RGB;


		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, quality, TRUE);


		jpeg_start_compress(&cinfo, TRUE);


		row_stride = buff.w * 3;

		while (cinfo.next_scanline < cinfo.image_height) 
		{
			row_pointer[0] = &buff.buff[cinfo.next_scanline * row_stride];
			(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
		}
		jpeg_finish_compress(&cinfo);
		fclose(outfile);
		jpeg_destroy_compress(&cinfo);
	}
}