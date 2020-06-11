#include "color.h"
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
}