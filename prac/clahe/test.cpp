#include <iostream>
#include <string>
#include <spes/image.h>
#include <spes/imageio.h>
#include <cmath>
#include <boost/filesystem.hpp>
#include <conio.h>
using namespace std;
using namespace spes;
using namespace spes::image;
using namespace spes::image::io;
using namespace spes::color;
using namespace boost::filesystem;

f32 find_limit(f32* array, f32 line)
{
    f32 dist[256];
    memcpy(dist, array, sizeof(f32) * 256);
    sort(dist, dist + 256);
    return dist[(int)(line * 255)];
}
void he_map(shared_ptr<image_t> tile, color_t* map, f64 limit)
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

    f32 r_limit = find_limit(r_dist, limit);
    f32 g_limit = find_limit(g_dist, limit);
    f32 b_limit = find_limit(b_dist, limit);
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
    printf("dispatched overflow, (%.6f, %.6f, %.6f)\n", r_overflow, g_overflow, b_overflow);
    for(int i = 0; i < 256; ++i)
    {
        r_dist[i] += r_overflow;
        g_dist[i] += g_overflow;
        b_dist[i] += b_overflow;
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

image_t clahe(image_t& im, int slices, f64 line)
{
    int W = im.width(), H = im.height();
    image_t img(im);
    //img.init(W, H);
    int tw = W / slices, th = H / slices;
    int tile_num = slices * slices;
    color_t tilemap[tile_num][256];
    shared_ptr<image_t> tiles[tile_num];

    for(int y = 0; y < slices; ++y)
    {
        for(int x = 0; x < slices; ++x)
        {
            int px = x * tw;
            int py = y * th;
            int pw = tw;
            int ph = th;

            int idx = y * slices + x;
            tiles[idx] = make_shared<image_t>();
            tiles[idx]->init(im.get_pixels(px, py, pw, ph));
            he_map(tiles[idx], tilemap[idx], line);

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

            img.set_pixels(*tiles[idx], px, py);
        }
    }

    return img;
}
void pro(const char* src, s32 slices, const vector<f64>& limit)
{
    auto img = image_io::read(src);
    image_io::show_image(img, "origin");
    for(auto p : limit)
    {
        char buff[256];
        sprintf(buff,  "clahe-%d-%f", slices, p);
        //image_io::show_image(ahe(img, p), buff);
        image_io::show_image(clahe(img, slices, p), buff);
    }
}
int main(int argc, char** argv)
{
    if(argc < 2)
    {
        cerr << "Usage: ahe file [offsets...]" << endl;
        return 0;
    }
    int sl = atoi(argv[2]);
    vector<f64> limit;
    for(int i = 3; i < argc; ++i)
    {
        limit.emplace_back(atof(argv[i]));
    }

    if(limit.size() == 0) limit.emplace_back(.9);
    pro(argv[1], sl, limit);

    getch();
    return 0;
}