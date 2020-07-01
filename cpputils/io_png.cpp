#include "io_png.h"
#include <cstdio>
#include <png.h>
using namespace std;

namespace spes::image::io
{
	image_t read_png(const char* file)
	{
		image_t im;
		FILE* fp;
		if ((fp = fopen(file, "rb")) == nullptr)
			return im;

		auto png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

		if (png == nullptr)
		{
			fclose(fp);
			return im;
		}

		auto info = png_create_info_struct(png);
		if (info == nullptr)
		{
			fclose(fp);
			png_destroy_read_struct(&png, nullptr, nullptr);
			return im;
		}

		if (setjmp(png_jmpbuf(png)))
		{
			png_destroy_read_struct(&png, &info, nullptr);
			fclose(fp);
			return im;
		}

		int sig = 0;
		png_init_io(png, fp);
		png_set_sig_bytes(png, sig);
		png_read_png(png, info, PNG_TRANSFORM_EXPAND, nullptr);
		PNG_COLOR_TYPE_RGBA;

		png_bytep* rps = png_get_rows(png, info);

		png_uint_32 w, h;
		int dep, color_type, interlace_type;
		png_get_IHDR(png, info, &w, &h, &dep, &color_type, &interlace_type, nullptr, nullptr);
		im.init(w, h);
		auto buff = im.buffer();

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
		fclose(fp);
		return im;
	}
	void write_png(image_t& im, const char* file)
	{
		png_structp png;
		png_infop info;

		int sig = 0;
		FILE* fp;
		if ((fp = fopen(file, "wb")) == nullptr)
			return;

		png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

		if (png == nullptr)
		{
			fclose(fp);
			return;
		}

		info = png_create_info_struct(png);
		if (info == nullptr)
		{
			fclose(fp);
			png_destroy_write_struct(&png, nullptr);
			return;
		}

		if (setjmp(png_jmpbuf(png)))
		{
			png_destroy_write_struct(&png, &info);
			fclose(fp);
			return;
		}

		png_init_io(png, fp);
		png_set_sig_bytes(png, sig);
		png_set_IHDR(png, info, im.width(), im.height(), 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		png_bytepp rps = new png_bytep[im.height()];
		for (u32 y = 0; y < im.height(); ++y)
			rps[y] = (png_bytep)(im.buffer() + y * im.width());

		png_set_rows(png, info, rps);
		png_write_info(png, info);
		png_write_png(png, info, PNG_TRANSFORM_IDENTITY, nullptr);
		png_write_end(png, info);

		delete[] rps;
		png_destroy_write_struct(&png, &info);
		fclose(fp);
	}
}