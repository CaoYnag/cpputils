#pragma once
#include "image.h"
using namespace spes::image;

namespace spes::image::io
{
	bool jpeg_sig_check(s8* buff, u32 len);
	std::shared_ptr<image_t> read_jpeg(FILE*);
	void write_jpeg(std::shared_ptr<image_t>, FILE*, int quality = 100);
}