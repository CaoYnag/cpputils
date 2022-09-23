#pragma once
#include "math.h"
#include "image.h"
#include "canvas.h"
#include <vector>
#include <functional>

namespace spes::canvas {
typedef std::function<f32(f32)> func;
class FuncCanvas : public Canvas {
public:
    FuncCanvas(spes::math::size2d&, spes::math::range2d&);
    FuncCanvas(spes::math::size2d&, spes::math::size2d&, spes::math::range2d&);
    virtual ~FuncCanvas();

    void background(spes::color::color_t);
    void background(std::shared_ptr<spes::image::image_t>);
    void axis(bool draw, spes::color::color_t c = spes::color::Colors::BLACK);
    void color(spes::color::color_t);
    void x_range(spes::math::range2d&);
    void y_range(spes::math::range2d&);
    void func(func);
};
} // namespace spes::canvas