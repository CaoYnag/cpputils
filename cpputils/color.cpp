#include "color.h"
#include "math.h"
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
	color_t operator==(const color_t& a, const color_t& b)
	{
		return a.c == b.c;
	}
	color_t operator!=(const color_t& a, const color_t& b)
	{
		return a.c != b.c;
	}


	color_t Colors::BLACK	= 0xff000000;
	color_t Colors::WHITE	= 0xffffffff;
	color_t Colors::RED		= 0xff0000ff;
	color_t Colors::GREEN	= 0xff00ff00;
	color_t Colors::BLUE	= 0xffff0000;
	color_t Colors::AQUA	= 0xffffff00;
}