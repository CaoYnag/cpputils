#pragma once
#include "image.h"
#include <vector>

namespace spes::image::io {
std::vector<std::shared_ptr<image_t>> read_gif(FILE*);
void                                  write_gif(std::vector<std::shared_ptr<image_t>>&, FILE*, u32 delay);
} // namespace spes::image::io