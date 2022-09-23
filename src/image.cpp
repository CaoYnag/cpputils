#include <spes/image.h>
#include <cstring>
using namespace std;
using namespace spes::color;

namespace spes::image {

s32 format_depth(u32 fmt) {
    switch (fmt) {
    case IMP_FMT_HSV:
        return 12; // sizeof(f32) * 3
    case IMP_FMT_ARGB:
    case IMP_FMT_XRGB:
        return 4;
    case IMP_FMT_RGB:
        return 3;
    case IMP_FMT_GREY:
        return 1;
    case IMP_FMT_GREY16:
        return 2;
    default:
        return 0;
    }
}
im_buff::im_buff(s32 ww, s32 hh, u32 ffmt) :
    w(ww), h(hh), fmt(ffmt) {
    u32 sz = w * h;
    s32 dep = format_depth(fmt);
    buff = new u8[sz * dep];
    memset(buff, 0, sz * dep);
}
im_buff::im_buff(s32 ww, s32 hh, u32 ffmt, u8* data) :
    w(ww), h(hh), fmt(ffmt) {
    u32 sz = w * h;
    s32 dep = format_depth(fmt);
    buff = new u8[sz * dep];
    if (data)
        memcpy(buff, data, sz * dep);
    else
        memset(buff, 0, sz * dep);
}
im_buff::im_buff(const im_buff& o) :
    w(o.w), h(o.h), fmt(o.fmt) {
    u32 sz = w * h;
    s32 dep = format_depth(fmt);
    buff = new u8[sz * dep];
    if (o.buff)
        memcpy(buff, o.buff, sz * dep);
    else
        memset(buff, 0, sz * dep);
}
im_buff::im_buff(shared_ptr<im_buff> o) :
    w(o->w), h(o->h), fmt(o->fmt) {
    u32 sz = w * h;
    s32 dep = format_depth(fmt);
    buff = new u8[sz * dep];
    if (o->buff)
        memcpy(buff, o->buff, sz * dep);
    else
        memset(buff, 0, sz * dep);
}
im_buff::~im_buff() {
}

image_t::image_t() :
    _init(false), _buff(nullptr) {
}
image_t::image_t(const image_t& im) :
    _init(false), _buff(nullptr) {
    init(im);
}
image_t::image_t(shared_ptr<image_t> im) :
    _init(false), _buff(nullptr) {
    init(im);
}
void image_t::init(s32 w, s32 h) {
    init(w, h, nullptr);
}
void image_t::init(s32 w, s32 h, color_t c) {
    _w = w;
    _h = h;
    _size = w * h;
    if (_buff) delete[] _buff;
    _buff = new color_t[_size];
    for (int i = 0; i < _size; ++i)
        _buff[i] = c;
    _init = true;
}
void image_t::init(s32 w, s32 h, color_t* buff) {
    _w = w;
    _h = h;
    _size = w * h;
    if (_buff) delete[] _buff;
    _buff = new color_t[_size];
    if (buff)
        memcpy(_buff, buff, _size * BAND_NUM);
    else
        memset(_buff, 0, _size * BAND_NUM);
    _init = true;
}
void image_t::init(const image_t& image) {
    init(image._w, image._h, image._buff);
}
void image_t::init(shared_ptr<image_t> image) {
    init(image->_w, image->_h, image->_buff);
}
image_t::~image_t() {
    if (_buff) delete[] _buff;
}

color_t image_t::get_pixel(int x, int y) const {
    if (!_init) return 0;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= _w) x = _w - 1;
    if (y >= _h) y = _h - 1;
    return _buff[idx(x, y)];
}
void image_t::set_pixel(s32 x, s32 y, color_t c) {
    if (!_init) return;
    if (x < 0 || y < 0 || x >= _w || y >= _h) return;
    _buff[idx(x, y)] = c;
}

void image_t::set_pixels(shared_ptr<image_t> im, s32 px, s32 py) {
    if (px >= _w || py >= _h || px <= -im->_w || py <= -im->_h) return;
    int dy = py + im->height();

    int y = py;
    if (y < 0) y = 0;
    for (; y < dy; ++y) {
        if (y >= _h) break;
        int x = px;
        if (x < 0) x = 0;

        int src_idx = (y - py) * im->_w + (x - px);
        int dst_idx = y * _w + x;
        int cpy_w = im->_w;
        if (px < 0)
            cpy_w += px;
        if (px + im->_w > _w)
            cpy_w -= (px + im->_w) - _w;
        memcpy(_buff + dst_idx, im->_buff + src_idx, cpy_w * 4);
    }
}

void image_t::draw_img(shared_ptr<image_t> im, s32 px, s32 py) {
    if (px >= _w || py >= _h || px <= -im->_w || py <= -im->_h) return;
    int dy = py + im->height();

    int y = py;
    if (y < 0) y = 0;
    for (; y < dy; ++y) {
        if (y >= _h) break;
        int x = px;
        if (x < 0) x = 0;

        int src_idx = (y - py) * im->_w + (x - px);
        int dst_idx = y * _w + x;
        int cpy_w = im->_w;
        if (px < 0)
            cpy_w += px;
        if (px + im->_w > _w)
            cpy_w -= (px + im->_w) - _w;
        color_t* dst = _buff + dst_idx;
        color_t* src = im->_buff + src_idx;
        for (int idx = 0; idx < cpy_w; ++idx)
            dst[idx] = dst[idx] * src[idx];
    }
}
shared_ptr<image_t> image_t::get_pixels(s32 x, s32 y, s32 w, s32 h, color_t c) {
    auto dst = make_shared<image_t>();
    dst->init(w, h, c);
    s32 dx = 0, dy = 0;
    s32 sx = x, sy = y, sw = w, sh = h;
    if (sx < 0) {
        sw += sx;
        dx -= sx;
        sx = 0;
    }
    if (sy < 0) {
        sh += sy;
        dy -= sy;
        sy = 0;
    }
    if (sw > _w) sw = _w;
    if (sh > _h) sh = _h;

    for (int yi = 0; yi < sh; ++yi) {
        auto src_buf = _buff + _w * (yi + sy) + sx;
        auto dst_buf = dst->_buff + dst->_w * (yi + dy) + dx;
        memcpy(dst_buf, src_buf, sw * BAND_NUM);
    }
    return dst;
}

shared_ptr<image_t> image_transform(shared_ptr<im_buff> buff) {
    auto im = make_shared<image_t>();

    if (buff->fmt == IMP_FMT_ARGB || buff->fmt == IMP_FMT_XRGB) {
        im->init(buff->w, buff->h, (color_t*)buff->buff);
        return im;
    } else
        im->init(buff->w, buff->h);

    auto dst = im->buffer();
    auto src = buff->buff;

    auto sz = buff->w * buff->h;
    for (u32 idx = 0; idx < sz; ++idx) {
        color_t c = 0x0;
        switch (buff->fmt) {
        case IMP_FMT_RGB: {
            c = color_t(src[idx * 3], src[idx * 3 + 1], src[idx * 3 + 2], 0xff);
        } break;
        case IMP_FMT_GREY: {
            c = color_t(src[idx], src[idx], src[idx], 0xff);
        } break;
        case IMP_FMT_HSV: {
            hsv_t* fsrc = (hsv_t*)src;
            c = from_hsv(fsrc[idx]);
        } break;
        default: break;
        }
        dst[idx] = c;
    }

    return im;
}

shared_ptr<im_buff> image_transform(shared_ptr<image_t> im, u32 fmt) {
    if (fmt == IMP_FMT_ARGB || fmt == IMP_FMT_XRGB)
        return make_shared<im_buff>(im->width(), im->height(), fmt, (u8*)im->buffer());
    auto buff = make_shared<im_buff>(im->width(), im->height(), fmt);

    auto dst = buff->buff;
    auto src = im->buffer();
    u32  sz = im->width() * im->height();

    for (u32 idx = 0; idx < sz; ++idx) {
        color_t c = src[idx];
        switch (fmt) {
        case IMP_FMT_RGB: {
            dst[idx * 3] = c.r;
            dst[idx * 3 + 1] = c.g;
            dst[idx * 3 + 2] = c.b;
        } break;
        case IMP_FMT_GREY: {
            // assume src image is grey, so  r == g == b
            dst[idx] = c.r;
        } break;
        case IMP_FMT_HSV: {
            hsv_t* ndst = (hsv_t*)dst;
            ndst[idx] = to_hsv(c);
        } break;
        default: break;
        }
    }

    return buff;
}

bool operator!(const image_t& im) {
    return !im.valid();
}

bool operator!(shared_ptr<image_t> im) {
    return !(im || im->valid());
}
} // namespace spes::image