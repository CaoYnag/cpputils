#pragma once
#include "image.h"
using namespace spes::image;

namespace spes::image::io
{
	bool png_sig_check(s8* buff, u32 len);
	image_t read_png(FILE*);
	void write_png(image_t&, FILE*);
}