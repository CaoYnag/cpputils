#pragma once
#include "image.h"
using namespace spes::image;

namespace spes::image::io
{
	bool jpeg_sig_check(s8* buff, u32 len);
	image_t read_jpeg(const char*);
	void write_jpeg(image_t&, const char*, int quality = 100);
}