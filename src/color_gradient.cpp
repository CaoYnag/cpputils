#include <spes/color.h>
#include <spes/math.h>
#include <stdexcept>
using namespace std;

namespace spes::color
{
	ColorGradient::ColorGradient(const vector<color_t>& colors)
	{
		if(colors.empty())
			throw runtime_error("bad color gradient param, 1 or more color is needed!");
		double s = 1.0;
		if(colors.size() > 1)
			s = 1.0 / (colors.size() - 1);
		double ss = 0;
		for(auto& c : colors)
		{
			_colors.emplace_back(ss, c);
			ss += s;
		}
	}

	ColorGradient::ColorGradient(const vector<tuple<double, color_t>>& colors) : _colors(colors)
	{
		if(colors.empty())
			throw runtime_error("bad color gradient param, 1 or more color is needed!");
	}

	color_t ColorGradient::clamp(double s) const
	{
		if(_colors.size() == 1) return get<1>(_colors[0]);
		if(s < 0) s = 0;
		if(s > 1) s = 1;
		int e = 1;
		while(s > get<0>(_colors[e])) ++e;
		int sidx = e - 1;
		auto ss = get<0>(_colors[sidx]);
		auto es = get<0>(_colors[e]);
		auto sc = get<1>(_colors[sidx]);
		auto ec = get<1>(_colors[e]);
		s -= ss;
		s /= (es - ss);
		
		return color_t(
			sc.r + (ec.r - sc.r) * s,
			sc.g + (ec.g - sc.g) * s,
			sc.b + (ec.b - sc.b) * s,
			sc.a + (ec.a - sc.a) * s
			);
	}
}
