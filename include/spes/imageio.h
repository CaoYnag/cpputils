#pragma once
#include "image.h"
#include "math.h"
#include <string>

namespace spes::image::io {
enum image_format {
    IMAGE_FMT_UNKNOWN,
    IMAGE_FMT_BMP,
    IMAGE_FMT_JPG,
    IMAGE_FMT_JPEG,
    IMAGE_FMT_PNG,
    IMAGE_FMT_GIF
};

u32 image_format(FILE* fp);
u32 image_format(const char* path);

class ImageViewer {
protected:
    image_t            _im;
    spes::math::size2d _sz;
    std::string        _tt;

public:
    ImageViewer(std::shared_ptr<image_t>, std::string tt);
    virtual ~ImageViewer();

    /* do some init stuff */
    virtual void init() = 0;
    /* open window and show image */
    virtual void show() = 0;
    /* refresh window */
    virtual void update() = 0;

    virtual void resize(u32 w, u32 h) = 0;
    virtual void best_size() = 0;
    virtual void origin_size() = 0;
};
class image_io {
public:
    static std::shared_ptr<image_t> read(const char* path);
    static void                     write(std::shared_ptr<image_t>, const char* path, u32 fmt = IMAGE_FMT_PNG);

    static std::shared_ptr<image_t> read(FILE* fp);
    static void                     write(std::shared_ptr<image_t>, FILE* fp, u32 fmt = IMAGE_FMT_PNG);

    static std::vector<std::shared_ptr<image_t>> read_anim(const char* path);
    static void                                  write_anim(std::vector<std::shared_ptr<image_t>>&, const char* path, u32 delay, u32 fmt = IMAGE_FMT_GIF);

    static std::vector<std::shared_ptr<image_t>> read_anim(FILE* fp);
    static void                                  write_anim(std::vector<std::shared_ptr<image_t>>&, FILE* fp, u32 delay, u32 fmt = IMAGE_FMT_GIF);
    static spes::math::size2d                    screen_size();
    static ImageViewer*                          show_image(const std::shared_ptr<image_t>, std::string title = "ImageView");
};
} // namespace spes::image::io
