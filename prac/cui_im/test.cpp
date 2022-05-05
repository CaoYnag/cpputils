#include <iostream>
#include <memory>
#include <string>
#include <spes/image.h>
#include <spes/imageio.h>
#include <vector>
using namespace std;
using namespace spes;
using namespace spes::image;
using namespace spes::image::io;
using namespace spes::color;

int SIZE = 64;

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
            ++idx;
        }
        printf("\n");
    }
}
shared_ptr<image_t> resize(shared_ptr<image_t> im, int w, int h)
{
    auto re = make_shared<image_t>();
    re->init(w, h);
    float sx, sy;
    sx = (im->width() + .0f) / w;
    sy = (im->height() + .0f) / h;
    float isx = 1 / sx;
    float isy = 1 / sy;
    float xs, xe, ys, ye;
    int ixs, ixe, iys, iye;
    printf("scale (%.2f, %.2f) is (%.2f, %.2f)\n", sx, sy, isx, isy);

    long cnt = 0;
    long total = w * h;
    for(int y = 0; y < h; ++y)
    {
        ys = y * sy;
        ye = ys + sy;
        iys = ys;
        iye = ye;
        if(iye != ye) ++iye;

        for(int x = 0; x < w; ++x)
        {
            xs = x * sx;
            xe = xs + sx;
            ixs = xs;
            ixe = xe;
            if(ixe != xe) ++ixe;
            float a = .0f, r = .0f, g = .0f, b = .0f;
            for(int yy = iys; yy < iye; ++yy)
            {
                float ssy = isy;
                if(yy == iys) ssy *= 1 + iys - ys;
                if(yy == iye) ssy *= 1 + ye - iye;
                for(int xx = ixs; xx < ixe; ++xx)
                {
                    color_t xc = im->get_pixel(xx, yy);
                    float ss = isx * ssy;
                    if(xx == ixs) ss *= 1 + ixs - xs;
                    if(xx == ixe) ss *= 1 + xe - ixe;
                    a += xc.a * ss;
                    r += xc.r * ss;
                    g += xc.g * ss;
                    b += xc.b * ss;
                }
            }
            re->set_pixel(x, y, color_t(r, g, b, a));
            ++cnt;
            if(cnt % 1000 == 0)
            {
                printf("complete %.2f\%\n", ((cnt * 100.0f) / total));
            }
        }
        
    }

    return re;
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
    // h /= 2;
    float s= (size + .0) / max(w, h);
    float r = 1 / s;
    w *= s;
    h *= s;
    printf("origin (%dx%d) resize (%dx%d)\n", img->width(), img->height(), w, h);
    auto re = resize(img, w, h);

    image_io::write(re, "trans.png"); // write converted image

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