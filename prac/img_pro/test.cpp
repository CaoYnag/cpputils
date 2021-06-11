#include <iostream>
#include <string>
#include <spes/image.h>
#include <spes/imageio.h>
#include <cmath>
#include <filesystem>
#include <conio.h>
using namespace std;
using namespace spes;
using namespace spes::image;
using namespace spes::image::io;
using namespace spes::color;
using namespace std::filesystem;

shared_ptr<image_t> histogram(shared_ptr<image_t> im)
{
	auto img = make_shared<image_t>(im);
    auto buff = img->buffer();
    auto sz = img->width() * img->height();
    auto sz4 = sz * 4;
    float grey_dist[256] = {.0f};
    float w = 1.f / sz;

    for(int i = 0; i < sz; ++i)
        grey_dist[buff[i].grey()] += w;

    float grey_map[256];
    float per = .0f;
    for(int i = 0; i < 256; ++i)
    {
        per += grey_dist[i];
        grey_map[i] = per * 215 + 40;
    }
    for(int i = 0; i < sz; ++i)
    {
        auto g = buff[i].grey();
        buff[i] = buff[i].grey(grey_map[g]);
    }
    return img;
}

shared_ptr<image_t> the(shared_ptr<image_t> im)
{
    constexpr const int PIXELS_THRESHOLD = 100;
    int W = im->width(), H = im->height();
    auto img = make_shared<image_t>();
    img->init(W, H);
    for(int y = 0; y < H; y += PIXELS_THRESHOLD)
    {
        for(int x = 0; x < W; x += PIXELS_THRESHOLD)
        {
            int w = PIXELS_THRESHOLD;
            int h = PIXELS_THRESHOLD;
            if((x + w) > W) w = W - x;
            if((y + h) > H) h = H - y;

            auto tile = im->get_pixels(x, y, w, h);
            auto tilehe = histogram(tile);
            img->set_pixels(tilehe, x, y);
        }
    }

    return img;
}

shared_ptr<image_t> gamma(shared_ptr<image_t> im, float GAMMA)
{
	auto img = make_shared<image_t>(im);
    auto buff = img->buffer();
    auto sz = img->width() * img->height();

    int grey_map[256];
    for(int i = 0; i < 256; ++i)
    {
        grey_map[i] = pow(i / 255.f, GAMMA) * 255;
    }

    for(int i = 0; i < sz; ++i)
    {
        auto g = buff[i].grey();
        buff[i] = buff[i].grey(grey_map[g]);
    }
    return img;
}

shared_ptr<image_t> lighter(shared_ptr<image_t> im, float scale)
{
    auto buf = image_transform(im, IMP_FMT_HSV);
    auto sz = im->width() * im->height();
    hsv_t* buff = (hsv_t*)buf->buff;
    for(int i = 0; i < sz; ++i)
    {
        buff[i].v *= scale;
        if(buff[i].v >= 1.f) buff[i].v = .0f;
    }
    return image_transform(buf);
}

shared_ptr<image_t> sat(shared_ptr<image_t> im, float scale)
{
    auto buf = image_transform(im, IMP_FMT_HSV);
    auto sz = im->width() * im->height();
    hsv_t* buff = (hsv_t*)buf->buff;
    for(int i = 0; i < sz; ++i)
    {
        buff[i].s *= scale;
        if(buff[i].s >= 1.f) buff[i].v = .0f;
    }
    return image_transform(buf);
}

void pro(const char* src)
{
    auto img = image_io::read(src);
    image_io::show_image(img, "origin");
    image_io::show_image(histogram(img), "histo");
    //image_io::show_image(the(img), "the");
    image_io::show_image(gamma(img, .5f), "gamma-0.5");
    image_io::show_image(lighter(img, 4.f), "light-4.0");
    image_io::show_image(sat(img, 2.f), "sat-2.0");

}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        cerr << "Usage: imgpro [file]" << endl;
        return 0;
    }
    pro(argv[1]);
    // /home/spes/temp/raster_tile/18
    //pro("/home/spes/temp/raster_tile/18/162880/215617.jpeg");
    // "/home/spes/temp/image_test/grey.png"
    //pro("/home/spes/temp/raster_pro/215609.jpeg");
    getch();
    return 0;
}