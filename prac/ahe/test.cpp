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

// got grey of pixel(x, y)
// when x, y out of range, using nearest value in edge.
inline u8 grey_edgecopy(const image_t& im, int x, int y)
{
    if(x < 0) x = 0;
    if(y < 0) y = 0;
    if(x >= im.width()) x = im.width() - 1;
    if(y >= im.height()) y = im.height() - 1;
    return im.get_pixel(x, y).grey();
}

u8 ahe(const image_t& im, int x, int y, u32 sz, int num, float weight)
{
    float grey_dist[256] = {.0f};
    for(int h = y - sz; h <= y + sz; ++h)
    {
        for(int w = x - sz; w <= x + sz; ++w)
        {
            grey_dist[grey_edgecopy(im, w, h)] += weight;
        }
    }
    u8 tar_grey = im.get_pixel(x, y).grey();
    float per = .0f;
    float ng = .0f;
    for(int i = 0; i <= tar_grey; ++i)
    {
        per += grey_dist[i];
        ng = per * 255;
    }
    return ng;
}

image_t ahe(const image_t& im, int sz = 1)
{
    image_t img(im);
    auto buff = img.buffer();
    auto W = img.width();

    // these variables would not change, it's unnecessary to compute for each sub ahe.
    int edge = sz * 2 + 1;
    int num = edge * edge;
    float weight = 1.f / num;

    for(int h = 0; h < img.height(); ++h)
    {
        for(int w = 0; w < img.width(); ++w)
        {
            auto& c = buff[h * W + w];
            c.grey(ahe(im, w, h, sz, num, weight));
        }
    }
    return img;
}

image_t rgb_he(const image_t& im, int off)
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
void he_map(shared_ptr<image_t> tile, /*color_t* map*/vector<color_t>& map)
{
    auto buff = tile->buffer();
    auto sz = tile->width() * tile->height();
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

    float rp = .0f, gp = .0f, bp = .0f;
    for(int i = 0; i < 256; ++i)
    {
        rp += r_dist[i];
        map[i].r = rp * 255;
        gp += g_dist[i];
        map[i].g = gp * 255;
        bp += b_dist[i];
        map[i].b = bp * 255;
    }
}
void regrey(shared_ptr<image_t> tile, color_t* m)
{
    auto buff = tile->buffer();
    auto sz = tile->width() * tile->height();
    for(int i = 0; i < sz; ++i)
    {
        buff[i].r = m[buff[i].r].r;
        buff[i].g = m[buff[i].g].g;
        buff[i].b = m[buff[i].b].b;
    }
}
inline s32 linear_interp(s32 a, s32 b, f32 t = .5f)
{
    return a * (1 - t) + b * t;
}
inline s32 bilinear_interp(s32 a, s32 b, s32 c, s32 d, f32 t1, f32 t2)
{
    f32 r1 = 1 - t1, r2 = 1 - t2;
    return a * r1 * r2 + b * t1 * r2 + c * r1 * t2 + d * t1 * t2;
}
void regrey_linear(shared_ptr<image_t> tile, color_t* m, color_t* m2)
{
    auto buff = tile->buffer();
    auto sz = tile->width() * tile->height();
    for(int i = 0; i < sz; ++i)
    {
        buff[i].r = linear_interp(m[buff[i].r].r, m2[buff[i].r].r);
        buff[i].g = linear_interp(m[buff[i].g].g, m2[buff[i].g].g);
        buff[i].b = linear_interp(m[buff[i].b].b, m2[buff[i].b].b);
    }
}

shared_ptr<image_t> the(shared_ptr<image_t> im, int slices = 8)
{
    int W = im->width(), H = im->height();
    auto img = make_shared<image_t>();
    img->init(W, H);
    int tw = W / slices, th = H / slices;
    int tile_num = slices * slices; 
    vector<vector<color_t>> tilemap;
    vector<shared_ptr<image_t>> tiles;
    //color_t tilemap[tile_num][256];
    //shared_ptr<image_t> tiles[tile_num];

    for(int y = 0; y < slices; ++y)
    {
        for(int x = 0; x < slices; ++x)
        {
            int px = x * tw;
            int py = y * th;
            int pw = tw;
            int ph = th;

            int idx = y * slices + x;
            tiles[idx] = im->get_pixels(px, py, pw, ph);
            he_map(tiles[idx], tilemap[idx]);

//            auto tilehe = rgb_he(*tiles[idx], 0);
//            img.set_pixels(tilehe, px, py);
        }
    }

    int hfw = tw / 2, hfh = th / 2;
    for(int py = 0; py < H; ++py)
    {
        for(int px = 0; px < W; ++px)
        {
            // 4 corner, just he as usual
            if(((px < hfw) || (px + hfw > W)) && ((py < hfh) || (py + hfh > H)))
            {
                int x = px / tw, y = py / th;
                if(x >= slices || y >= slices) continue;
                int idx = y * slices + x;
                int tx = px % tw, ty = py % th;
                //printf("pix (%d, %d), tile (%d, %d) tile pix(%d, %d)\n", px, py, x, y, tx, ty);
                //regrey(tiles[idx], tilemap[idx]);
                auto c = tiles[idx]->get_pixel(tx, ty);
                c.r = tilemap[idx][c.r].r;
                c.g = tilemap[idx][c.g].g;
                c.b = tilemap[idx][c.b].b;
                tiles[idx]->set_pixel(tx, ty, c);
            }
            // 2 vertical edge, use linear interpolation.
            else if((px < hfw) || (px + hfw > W))
            {
                int x = px / tw, y = py / th;
                int idxx = (px - hfw) / tw;
                idxx = idxx < 0 ? 0 : idxx;
                int idxy = (py - hfh) / th;
                idxy = idxy < 0 ? 0 : idxy;
                if(x >= slices || y >= slices) continue;
                int idx = y * slices + x;
                int idx1 = idxy * slices + idxx;
                int idx2 = idx1 + slices;
                int tx = px % tw, ty = py % th;
                auto c = tiles[idx]->get_pixel(tx, ty);
                f32 t = (py - (idxy * th + hfh) + .0f) / th;
                c.r = linear_interp(tilemap[idx1][c.r].r, tilemap[idx2][c.r].r, t);
                c.g = linear_interp(tilemap[idx1][c.g].g, tilemap[idx2][c.g].g, t);
                c.b = linear_interp(tilemap[idx1][c.b].b, tilemap[idx2][c.b].b, t);
                tiles[idx]->set_pixel(tx, ty, c);
            }
            // 2 horizontal edge, also linear interp
            else if((py < hfh) || (py + hfh > H))
            {
                int x = px / tw, y = py / th;
                int idxx = (px - hfw) / tw;
                idxx = idxx < 0 ? 0 : idxx;
                int idxy = (py - hfh) / th;
                idxy = idxy < 0 ? 0 : idxy;
                if(x >= slices || y >= slices) continue;
                int idx = y * slices + x;
                int idx1 = idxy * slices + idxx;
                int idx2 = idx1 + 1;
                int idx3 = idx1 + slices;
                int idx4 = idx3 + 1;
                int tx = px % tw, ty = py % th;
                auto c = tiles[idx]->get_pixel(tx, ty);
                f32 t = (px - (idxx * tw + hfw) + .0f) / tw;
                c.r = linear_interp(tilemap[idx1][c.r].r, tilemap[idx2][c.r].r, t);
                c.g = linear_interp(tilemap[idx1][c.g].g, tilemap[idx2][c.g].g, t);
                c.b = linear_interp(tilemap[idx1][c.b].b, tilemap[idx2][c.b].b, t);
                tiles[idx]->set_pixel(tx, ty, c);
            }
            // other inner pixes, use bilinear interp
            else
            {
                int x = px / tw, y = py / th;
                int idxx = (px - hfw) / tw;
                idxx = idxx < 0 ? 0 : idxx;
                int idxy = (py - hfh) / th;
                idxy = idxy < 0 ? 0 : idxy;
                if(x >= slices || y >= slices) continue;
                int idx = y * slices + x;
                int idx1 = idxy * slices + idxx;
                int idx2 = idx1 + 1;
                int idx3 = idx1 + slices;
                int idx4 = idx3 + 1;
                int tx = px % tw, ty = py % th;
                auto c = tiles[idx]->get_pixel(tx, ty);
                f32 t = (px - (idxx * tw + hfw) + .0f) / tw;
                f32 t2 = (py - (idxy * th + hfh) + .0f) / th;
                c.r = bilinear_interp(tilemap[idx1][c.r].r, tilemap[idx2][c.r].r,
                                      tilemap[idx3][c.r].r, tilemap[idx4][c.r].r, t, t2);
                c.g = bilinear_interp(tilemap[idx1][c.g].g, tilemap[idx2][c.g].g,
                                      tilemap[idx3][c.g].g, tilemap[idx4][c.g].g, t, t2);
                c.b = bilinear_interp(tilemap[idx1][c.b].b, tilemap[idx2][c.b].b,
                                      tilemap[idx3][c.b].b, tilemap[idx4][c.b].b, t, t2);
                tiles[idx]->set_pixel(tx, ty, c);
            }
        }
    }

    for(int y = 0; y < slices; ++y)
    {
        for(int x = 0; x < slices; ++x)
        {
            int px = x * tw;
            int py = y * th;
            int idx = y * slices + x;
            //regrey(tiles[idx], tilemap[idx]);

            img->set_pixels(tiles[idx], px, py);
        }
    }

    return img;
}
void pro(const char* src, const vector<s32>& params)
{
    auto img = image_io::read(src);
    image_io::show_image(img, "origin");
    for(auto p : params)
    {
        char buff[256];
        sprintf(buff,  "ahe-%d", p);
        //image_io::show_image(ahe(img, p), buff);
        image_io::show_image(the(img, p), buff);
    }
}
int main(int argc, char** argv)
{
    if(argc < 2)
    {
        cerr << "Usage: ahe file [offsets...]" << endl;
        return 0;
    }
    vector<s32> params;
    for(int i = 2; i < argc; ++i)
    {
        int s = atoi(argv[i]);
        if(s > 0)
        params.emplace_back(s);
    }

    if(params.size() == 0) params.emplace_back(4);
    pro(argv[1], params);

    getch();
    return 0;
}