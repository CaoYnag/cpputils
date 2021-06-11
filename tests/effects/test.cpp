#include <iostream>
#include <spes/image.h>
#include <spes/imageio.h>
#include <vector>
#include <stdexcept>
#include <conio.h>
using namespace std;
using namespace spes;
using namespace spes::image;
using namespace spes::image::io;

void histogram(image_t& img)
{
    auto buff = img.buffer();
    auto sz = img.width() * img.height();
    auto sz4 = sz * 4;
    float grey_dist[256] = {.0f};
    float w = 1.f / sz;

    for(int i = 0; i < sz; ++i)
        grey_dist[buff[i].grey()] += w;

    for(int i = 0; i < 256; ++i)
        printf("grey %d : %.2f\n", i, grey_dist[i]);

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
}

void gamma(image_t& img)
{
    const float GAMMA = 8;
    auto buff = img.buffer();
    auto sz = img.width() * img.height();

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
}
void test(const char* src)
{
    auto ori = image_io::read(src);
    auto hist = make_shared<image_t>(ori);
    histogram(*hist);
    auto gm = make_shared<image_t>(ori);
    gamma(*gm);

    image_io::show_image(ori);
    image_io::show_image(hist);
    image_io::show_image(gm);
}

int main(int argc, char* argv[])
{
    test("/home/spes/temp/img_pro/215609.jpeg");
	return 0;
}