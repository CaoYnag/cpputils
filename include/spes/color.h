#pragma once
#include "type.h"
#include <initializer_list>
#include <vector>
#include <iostream>
#include <tuple>
using std::tuple;
using std::vector;

namespace spes::color {
extern const int BAND_NUM;
enum color_band {
    BAND_R = 0,
    BAND_G,
    BAND_B,
    BAND_A
};
union color_t {
    u32 c;
    struct
    {
        u8 r, g, b, a;
    };
    u8 v[4];

    color_t();
    color_t(u32);
    color_t(const std::initializer_list<u8>&);
    color_t(u8 r, u8 g, u8 b, u8 a = 0xff);
    u8      grey();
    color_t grey(u8);
};
/* simple add color, and use bigger alpha */
color_t operator+(const color_t& a, const color_t& b);
/* merge color with alpha */
color_t operator*(const color_t& a, const color_t& b);
/* clamp color */
color_t       operator*(const color_t& a, float s);
color_t       operator-(const color_t& a, const color_t& b);
bool          operator==(const color_t& a, const color_t& b);
bool          operator!=(const color_t& a, const color_t& b);
std::ostream& operator<<(std::ostream& os, const color_t& c);

class Colors {
public:
    static color_t BLACK;
    static color_t WHITE;
    static color_t RED;
    static color_t GREEN;
    static color_t BLUE;
    static color_t AQUA;
};
// color_t operator"" _c(const char*);
// string operator"" _s(const color_t&);

struct hsv_t {
    f32 h, s, v;
};

hsv_t   to_hsv(const color_t& c);
color_t from_hsv(const hsv_t& c);

/* Clamp Color */
class ColorGradient {
    vector<tuple<double, color_t>> _colors;

public:
    ColorGradient(const vector<color_t>& colors);
    ColorGradient(const vector<tuple<double, color_t>>& colors);

    color_t clamp(double s) const;
};
} // namespace spes::color
