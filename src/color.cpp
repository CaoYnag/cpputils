#include <spes/color.h>
#include <spes/math.h>
using namespace std;

namespace spes::color
{
	const int BAND_NUM = 4;
	color_t::color_t() {}
	color_t::color_t(u32 cc) : c(cc)
	{}
	color_t::color_t(const std::initializer_list<u8>& l)
	{
		int idx = 0;
		for (u8 u : l)
		{
			v[idx++] = u;
		}
	}
	color_t::color_t(u8 rr, u8 gg, u8 bb, u8 aa) : r(rr), g(gg), b(bb), a(aa)
	{}

	u8 color_t::grey()
	{
		return (r + g + b) / 3;
	}

	color_t color_t::grey(u8 ng)
	{
		f32 factor = (ng + 0.f) / grey();
		r *= factor;
		g *= factor;
		b *= factor;

		return *this;
	}

	color_t operator+(const color_t& a, const color_t& b)
	{
		u8 aa = a.a > b.a ? a.a : b.a;
		return color_t(a.r + b.r, a.g + b.g, a.b + b.b, aa);
	}
	color_t operator-(const color_t& a, const color_t& b)
	{
		u8 aa = a.a < b.a ? a.a : b.a;
		u8 rr = a.r > b.r ? a.r - b.r : 0;
		u8 gg = a.g > b.g ? a.g - b.g : 0;
		u8 bb = a.b > b.b ? a.b - b.b : 0;
		return color_t(rr, gg, bb, aa);
	}
	bool operator==(const color_t& a, const color_t& b)
	{
		return a.c == b.c;
	}
    bool operator!=(const color_t& a, const color_t& b)
	{
		return a.c != b.c;
	}
    std::ostream& operator<<(std::ostream& os, const color_t& c)
    {
	    char buff[1024];
	    sprintf(buff, "%02x%02x%02x%02x", c.a, c.r, c.g, c.b);
	    os << buff;
	    return os;
    }


	color_t Colors::BLACK	= 0xff000000;
	color_t Colors::WHITE	= 0xffffffff;
	color_t Colors::RED		= 0xff0000ff;
	color_t Colors::GREEN	= 0xff00ff00;
	color_t Colors::BLUE	= 0xffff0000;
	color_t Colors::AQUA	= 0xffffff00;


	/* HSV */
    hsv_t to_hsv(const color_t& c)
    {
        f32 r = c.r / 255.f;
        f32 g = c.g / 255.f;
        f32 b = c.b / 255.f;
        f32 i = fmin(r, fmin(g, b));
        f32 a = fmax(r, fmax(g, b));
        f32 diff = a - i;

        f32 h, s, v;
        /* assign v */
        v = a;
        /* assign s */
        if(a == .0f) s = 0;
        else s = diff / a;
        /* assign h */
        constexpr const f32 PI = 3.141592654589793;
        constexpr const f32 deg60 = PI / 3;

        if(i == a) h = 0;
        else if(a == r) h = deg60 * ((g - b) / diff);
        else if(a == g) h = deg60 * ((b - r) / diff + 2);
        else h = deg60 * ((r - g) / diff + 4);
        return {h, s, v};
    }
    color_t from_hsv(const hsv_t& c)
    {
        constexpr const f32 PI = 3.141592654589793;
        constexpr const f32 deg60 = PI / 3;

        float r, g, b;
        if(c.s == .0f)
        {
            r = c.v;
            g = c.v;
            b = c.v;
        }
        else
        {
            f32 h = c.h / deg60;
            int ih = h;
            f32 f = h - ih;
            f32 pa = c.v * (1 - c.s);
            f32 pb = c.v * (1 - c.s * f);
            f32 pc = c.v * (1 - c.s * (1 - f));
            switch (ih)
            {
                case 0:
                {
                    r = c.v;
                    g = pc;
                    b = pa;
                }break;
                case 1:
                {
                    r = pb;
                    g = c.v;
                    b = pa;
                }break;
                case 2:
                {
                    r = pa;
                    g = c.v;
                    b = pc;
                }break;
                case 3:
                {
                    r = pa;
                    g = pb;
                    b = c.v;
                }break;
                case 4:
                {
                    r = pc;
                    g = pa;
                    b = c.v;
                }break;
                case 5:
                {
                    r = c.v;
                    g = pa;
                    b = pb;
                }break;
            }
        }
        return color_t(r * 0xff, g * 0xff, b * 0xff);
    }
}