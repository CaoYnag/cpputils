#include <spes/io_png.h>
#include <cstdio>
#include <png.h>
using namespace std;

namespace spes::image::io
{
	bool png_sig_check(s8* buff, u32 len)
	{
		return !png_sig_cmp((png_const_bytep)buff, 0, len);
	}
	shared_ptr<image_t> read_png(FILE* fp)
	{
		auto im = make_shared<image_t>();
		if (!fp)
		{
			perror("invalid fp in read_png");
			return im;
		}
		auto png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

		if (!png)
		{
			perror("init png read failed.");
			return im;
		}

		auto info = png_create_info_struct(png);
		if (!info)
		{
			perror("init png info failed.");
			png_destroy_read_struct(&png, nullptr, nullptr);
			return im;
		}

		if (setjmp(png_jmpbuf(png)))
		{
			png_destroy_read_struct(&png, &info, nullptr);
			return im;
		}

		//int sig = 0;
		png_init_io(png, fp);
		//png_set_sig_bytes(png, sig);
		png_read_png(png, info, PNG_TRANSFORM_EXPAND, nullptr);

		png_uint_32 w, h;
		int dep, color_type, interlace_type;
		png_get_IHDR(png, info, &w, &h, &dep, &color_type, &interlace_type, nullptr, nullptr);
		im->init(w, h);
		auto buff = im->buffer();

		png_bytep* rps = png_get_rows(png, info);

		u32 idx = 0;
		for (u32 y = 0; y < h; ++y)
		{
			for (u32 x = 0; x < w; ++x)
			{
				color_t c;
				if(color_type & PNG_COLOR_MASK_ALPHA)
					c = color_t(rps[y][4 * x], rps[y][4 * x + 1], rps[y][4 * x + 2], rps[y][4 * x + 3]);
				else
					c = color_t(rps[y][3 * x], rps[y][3 * x + 1], rps[y][3 * x + 2], 255);
				buff[idx++] = c;
			}
		}

		png_destroy_read_struct(&png, &info, nullptr);
		return im;
	}
	void write_png(shared_ptr<image_t> im, FILE* fp)
	{
		png_structp png;
		png_infop info;

		int sig = 0;
		if (!fp)
		{
			perror("invalid fp in write_png");
			return;
		}

		png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

		if (!png)
		{
			perror("init png write failed.");
			return;
		}

		info = png_create_info_struct(png);
		if (info == nullptr)
		{
			perror("init png info failed.");
			png_destroy_write_struct(&png, nullptr);
			return;
		}

		if (setjmp(png_jmpbuf(png)))
		{
			png_destroy_write_struct(&png, &info);
			return;
		}
		png_init_io(png, fp);
		png_set_sig_bytes(png, sig);
		png_set_IHDR(png, info, im->width(), im->height(), 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		png_bytepp rps = new png_bytep[im->height()];
		for (u32 y = 0; y < im->height(); ++y)
			rps[y] = (png_bytep)(im->buffer() + y * im->width());

		png_set_rows(png, info, rps);
		png_write_info(png, info);
		png_write_png(png, info, PNG_TRANSFORM_IDENTITY, nullptr);
		png_write_end(png, info);

		delete[] rps;
		png_destroy_write_struct(&png, &info);
	}
}
