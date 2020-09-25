#pragma once
#include "image.h"
#include <vector>
using namespace spes::image;

namespace spes::image::io
{
	std::vector<image_t> read_gif(const char*);
	void write_gif(std::vector<image_t>&, const char*, u32 delay);
}