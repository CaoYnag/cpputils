#pragma once
#include "type.h"
#include "color.h"
#include "math.h"
#include "image.h"
#include <vector>

namespace spes::canvas {
/*
for paramter points: p1, p2, p3, p4 ... pn
*/
enum CANVAS_RENDER_MODE {
    CRM_None,
    /* just draw p1, p2 ... pn */
    CRM_Points,
    /* draw [p1, p2], [p3, p4] ... [pn-1, pn], and n must be an odd number */
    CRM_LineSegments,
    /* draw [p1, p2, p3 ... pn] */
    CRM_LineStrip,
    /* draw [p1, p2, p3 ... pn, p1] */
    CRM_LineRing
};
class Canvas {
protected:
    std::shared_ptr<spes::image::image_t> _im;
    u64                                   _size;
    spes::color::color_t*                 _buff;
    spes::math::rect                      _rc;
    spes::math::size2d                    _sz;

    inline u32 idx(u32 x, u32 y) const {
        return x + y * _im->width();
    }
    inline u32 idx(const spes::math::point2d& p) const {
        return p.x + p.y * _im->width();
    }

public:
    Canvas(u32 w, u32 h, spes::color::color_t bk = spes::color::Colors::WHITE);
    Canvas(std::shared_ptr<spes::image::image_t> im);
    virtual ~Canvas();

    inline std::shared_ptr<spes::image::image_t> image() {
        return _im;
    }

public:
    virtual void draw(u32 mode, const std::vector<spes::math::point2d>& pts, spes::color::color_t c = spes::color::Colors::BLACK);
    virtual void draw_point(const spes::math::point2d& p, spes::color::color_t c = spes::color::Colors::BLACK);
    virtual void draw_line(const spes::math::line2d& l, spes::color::color_t c = spes::color::Colors::BLACK);
    virtual void draw_rect(const spes::math::rect& rec, spes::color::color_t c = spes::color::Colors::BLACK);
    virtual void draw_triangle(const std::vector<spes::math::point2d>& tri, spes::color::color_t c = spes::color::Colors::BLACK);
    virtual void draw_polygon(const spes::math::polygon2d& poly, spes::color::color_t c = spes::color::Colors::BLACK);
};
} // namespace spes::canvas