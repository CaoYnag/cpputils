#include <iostream>
#include <string>
#include <spes/image.h>
#include <spes/imageio.h>
#include <cmath>
#include <boost/filesystem.hpp>
#include <conio.h>
#include <vector>
using namespace std;
using namespace spes;
using namespace spes::image;
using namespace spes::image::io;
using namespace spes::color;
using namespace boost::filesystem;

image_t rgb_he(const image_t& im, int off = 0)
{
    image_t img(im);
    auto buff = img.buffer();
    auto sz = img.width() * img.height();
    float r_dist[256] = {.0f};
    float g_dist[256] = {.0f};
    float b_dist[256] = {.0f};
    float w = 1.f / sz;

    for(int i = 0; i < sz; ++i)
    {
        r_dist[buff[i].r] += w;
        g_dist[buff[i].g] += w;
        b_dist[buff[i].b] += w;
    }

    float r_map[256];
    float g_map[256];
    float b_map[256];
    float rp = .0f, gp = .0f, bp = .0f;
    for(int i = 0; i < 256; ++i)
    {
        rp += r_dist[i];
        r_map[i] = rp * (255 - off) + off;
        gp += g_dist[i];
        g_map[i] = gp * (255 - off) + off;
        bp += b_dist[i];
        b_map[i] = bp * (255 - off) + off;
    }
    for(int i = 0; i < sz; ++i)
    {
        buff[i].r = r_map[buff[i].r];
        buff[i].g = g_map[buff[i].g];
        buff[i].b = b_map[buff[i].b];
    }
    return img;
}

image_t histogram(const image_t& im, int off = 0)
{
    image_t img(im);
    auto buff = img.buffer();
    auto sz = img.width() * img.height();
    float grey_dist[256] = {.0f};
    float w = 1.f / sz;

    for(int i = 0; i < sz; ++i)
        grey_dist[buff[i].grey()] += w;

    float grey_map[256];
    float per = .0f;
    for(int i = 0; i < 256; ++i)
    {
        per += grey_dist[i];
        grey_map[i] = per * (255 - off) + off;
    }
    for(int i = 0; i < sz; ++i)
    {
        auto g = buff[i].grey();
        buff[i] = buff[i].grey(grey_map[g]);
    }
    return img;
}

void pro(const char* src, const vector<f64>& params)
{
    auto img = image_io::read(src);
    image_io::show_image(img, "origin");
    for(auto p : params)
    {
        char buff[256];
        sprintf(buff,  "he-%.2f", p);
        image_io::show_image(histogram(img, p), buff);
        sprintf(buff,  "rgb-%.2f", p);
        image_io::show_image(rgb_he(img, p), buff);
    }
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        cerr << "Usage: he file [offsets...]" << endl;
        return 0;
    }
    vector<f64> params;
    for(int i = 2; i < argc; ++i)
        params.emplace_back(atof(argv[i]));

    if(params.size() == 0) params.emplace_back(0);
    pro(argv[1], params);

    getch();
    return 0;
}