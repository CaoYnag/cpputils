#pragma once
#include "image.h"
#include <vector>
using namespace spes::image;

namespace spes::image::io
{
	std::vector<image_t> read_gif(FILE*);
	void write_gif(std::vector<image_t>&, FILE*, u32 delay);
}