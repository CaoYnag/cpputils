#pragma once
#include "image.h"
using namespace spes::image;

namespace spes::image::io
{
	enum image_format
	{
		IMAGE_FMT_BMP,
		IMAGE_FMT_JPG,
		IMAGE_FMT_PNG
	};
	class image_io
	{
	public:
		static image_t read(const char* path);
		static void write_image(const image_t&, const char* path, u32 fmt = IMAGE_FMT_PNG);
	};
}