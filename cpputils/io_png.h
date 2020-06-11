#pragma once
#include "image.h"
using namespace spes::image;

namespace spes::image::io
{
	image_t read_png(const char*);
	void write_png(const image_t&, const char*);
}