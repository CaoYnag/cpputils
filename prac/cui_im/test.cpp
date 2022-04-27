#include <iostream>
#include <memory>
#include <string>
#include <spes/image.h>
#include <spes/imageio.h>
#include <cmath>
#include <filesystem>
#include <conio.h>
#include <vector>
using namespace std;
using namespace spes;
using namespace spes::image;
using namespace spes::image::io;
using namespace spes::color;
using namespace std::filesystem;

int SIZE = 200;

void draw_im(shared_ptr<image_t> im)
{
    int w = im->width();
    int h = im->height();
    auto buf = im->buffer();
    int idx = 0;
    for(int i = 0; i < h; ++i)
    {
        for(int j = 0; j < w; ++j)
        {
            auto c = buf[idx];
            printf("\e[38;2;%d;%d;%dm#\e[0m", c.r, c.g, c.b);
            ++i;
        }
        printf("\n");
    }
}

void pro(const char* src, int size)
{
    auto img = image_io::read(src);
    if(!img->valid())
    {
        cerr << "failed open " << src << endl;
        return;
    }
    int w = img->width();
    int h = img->height();
    float s= (size + .0) / max(w, h);
    float r = 1 / s;
    w *= s;
    h *= s;
    printf("origin (%dx%d) resize (%dx%d)\n", img->width(), img->height(), w, h);
    auto re = make_shared<image_t>();
    re->init(w, h);

    //draw_im(re);
}


int main(int argc, char** argv)
{
    if(argc < 2)
    {
        cerr << "Usage: cui_im file [size]" << endl;
        return 0;
    }
    if(argc > 2) SIZE = atoi(argv[2]);
    
    pro(argv[1], SIZE);
    return 0;
}