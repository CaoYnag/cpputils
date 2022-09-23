#pragma once
#include "image.h"

namespace spes::image::io {
bool                     png_sig_check(s8* buff, u32 len);
std::shared_ptr<image_t> read_png(FILE*);
void                     write_png(std::shared_ptr<image_t>, FILE*);
} // namespace spes::image::io