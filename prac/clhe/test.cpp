#include <iostream>
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

f32 find_limit(f32* array, f32 line)
{
    f32 dist[256];
    memcpy(dist, array, sizeof(f32) * 256);
    sort(dist, dist + 256);
    return dist[(int)(line * 255)];
}

image_t clhe_rgb(const image_t& im, f64 limit)
{
    image_t img(im);
    auto buff = img.buffer();
    auto sz = img.width() * img.height();
    f32 r_dist[256] = {.0f}, g_dist[256] = {.0f}, b_dist[256] = {.0f};
    float w = 1.f / sz;

    for(int i = 0; i < sz; ++i)
    {
        r_dist[buff[i].r] += w;
        g_dist[buff[i].g] += w;
        b_dist[buff[i].b] += w;
    }
    f32 r_limit = find_limit(r_dist, limit);
    f32 g_limit = find_limit(g_dist, limit);
    f32 b_limit = find_limit(b_dist, limit);
    printf("got limit: %.6f %.6f %.6f\n", r_limit, g_limit, b_limit);
    f32 r_overflow = .0f, g_overflow = .0f, b_overflow = .0f;
    for(int i = 0; i < 256; ++i)
    {
        if(r_dist[i] > r_limit)
        {
            r_overflow += r_dist[i] - r_limit;
            r_dist[i] = r_limit;
        }
        if(g_dist[i] > g_limit)
        {
            g_overflow += g_dist[i] - g_limit;
            g_dist[i] = g_limit;
        }
        if(b_dist[i] > b_limit)
        {
            b_overflow += b_dist[i] - b_limit;
            b_dist[i] = b_limit;
        }
    }
    r_overflow /= 256;
    g_overflow /= 256;
    b_overflow /= 256;
    for(int i = 0; i < 256; ++i)
    {
        r_dist[i] += r_overflow;
        g_dist[i] += g_overflow;
        b_dist[i] += b_overflow;
    }

    f32 r_map[256];
    f32 g_map[256];
    f32 b_map[256];
    f32 rp = .0f, gp = .0f, bp = .0f;
    for(int i = 0; i < 256; ++i)
    {
        rp += r_dist[i];
        r_map[i] = rp * 255;
        gp += g_dist[i];
        g_map[i] = gp * 255;
        bp += b_dist[i];
        b_map[i] = bp * 255;
    }
    for(int i = 0; i < sz; ++i)
    {
        buff[i].r = r_map[buff[i].r];
        buff[i].g = g_map[buff[i].g];
        buff[i].b = b_map[buff[i].b];
    }
    return img;
}

image_t clhe(const image_t& im, f64 limit)
{
    image_t img(im);
    auto buff = img.buffer();
    auto sz = img.width() * img.height();
    f32 grey_dist[256] = {.0f};
    float w = 1.f / sz;

    for(int i = 0; i < sz; ++i)
        grey_dist[buff[i].grey()] += w;
    f32 grey_overflow = .0f;
    for(int i = 0; i < 256; ++i)
        if(grey_dist[i] > limit)
        {
            grey_overflow += grey_dist[i] - limit;
            grey_dist[i] = grey_overflow;
        }
    grey_overflow /= 256;
    printf("dispatched overflow: %.6f\n", grey_overflow);
    for(int i = 0; i < 256; ++i)
        grey_dist[i] += grey_overflow;

    float grey_map[256];
    float per = .0f;
    for(int i = 0; i < 256; ++i)
    {
        per += grey_dist[i];
        grey_map[i] = per * 255;
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
        sprintf(buff,  "clhe-%.6f", p);
        /*image_io::show_image(clhe(img, p), buff);
        sprintf(buff,  "rgb-%.2f", p);*/
        image_io::show_image(clhe_rgb(img, p), buff);
    }
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        cerr << "Usage: clhe file [offsets...]" << endl;
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