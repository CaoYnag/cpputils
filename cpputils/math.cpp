#include "math.h"
using namespace spes;

namespace spes::math
{
	vector2d operator+(const vector2d& v1, const vector2d& v2)
	{
		return { v1.x + v2.x, v1.y + v2.y };
	}

	vector2d operator-(const vector2d& v)
	{
		return { -v.x, -v.y };
	}

	vector2d operator-(const vector2d& v1, const vector2d& v2)
	{
		return { v1.x - v2.x, v1.y - v2.y };
	}

	s32 operator*(const vector2d& v1, const vector2d& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	vector2d operator*(const vector2d& v, f32 s)
	{
		return { (v.x * s), (v.y * s) };
	}

	bool operator==(const vector2d& v1, const vector2d& v2)
	{
		return v1.x == v2.x && v1.y == v2.y;
	}


	f32 cross(vector2d& v1, vector2d& v2)
	{
		return v1.x * v2.y - v1.y * v2.x;
	}

	size2d rect_adjust(size2d border, size2d rect)
	{
		if (rect.w > border.w)
		{
			f32 factor = (border.w + .0f) / rect.w;

			if (rect.h > border.h)
				factor = fminf(factor, (border.h + .0f) / rect.h);

			rect = rect * factor;
		}
		return rect;
	}


	void line2d::gen_k()
	{
		if (vertical()) _k = 0;
		else _k = (_b.y - _a.y + .0f) / (_b.x - _a.x);
	}
	line2d::line2d(const vector2d& a, const vector2d& b) : _a(a), _b(b)
	{
		gen_k();
	}
	line2d::~line2d()
	{}

	void line2d::a(const vector2d& p)
	{
		_a = p;
		gen_k();
	}
	void line2d::b(const vector2d& p)
	{
		_b = p;
		gen_k();
	}

	bool line2d::resolv_x(f32 y, f32& x)
	{
		if (vertical())
			x = _a.x;
		else
			x = _a.x + (y - _a.y) / _k;
		return in_range(y, _a.y, _b.y) || in_range(y, _b.y, _a.y);
	}

	bool line2d::resolv_y(f32 x, f32& y)
	{
		if (horizontal())
			y = _a.y;
		else
			y = (x - _a.x) * _k + _a.y;
		return in_range(x, _a.x, _b.x) || in_range(x, _b.x, _a.x);
	}

	bool operator==(const line2d& v1, const line2d& v2)
	{
		return (v1._a == v2._a && v1._b == v2._b) || (v1._a == v2._b && v1._b == v2._a);
	}



	rect::rect(const vector2d& lt, const vector2d& rb) : _lt(lt), _rb(rb)
	{}
	rect::rect(f32 left, f32 top, f32 right, f32 bottom) : _lt({ left, top }), _rb({ right, bottom })
	{}
	rect::~rect() {}

	bool operator==(const rect& rc1, const rect& rc2)
	{
		return rc1._lt == rc2._lt && rc1._rb == rc2._rb;
	}

	bool intersects(const rect& a, const rect& b, rect& ret)
	{
		if (a.left() > b.right() || a.right() < b.left() || a.top() > b.bottom() || a.bottom() < b.top()) return false;
		f32 left = fmax(a.left(), b.left());
		f32 right = fmin(a.right(), b.right());
		f32 top = fmax(a.top(), b.top());
		f32 bot = fmin(a.right(), b.right());
		ret._lt.x = left;
		ret._lt.y = top;
		ret._rb.x = right;
		ret._rb.y = bot;
		return true;
	}

	KEY GenKey(const vector2d& p, const rect& r)
	{
		KEY key = 0;

		if (p.y > r.bottom())
		{
			key |= CB_BELOW;
		}
		if (p.y < r.top())
		{
			key |= CB_ABOVE;
		}
		if (p.x < r.left())
		{
			key |= CB_LEFT;
		}
		if (p.x > r.right())
		{
			key |= CB_RIGHT;
		}

		return key;
	}

	int cohen_sutherland(line2d& line, const rect& rect)
	{
		vector2d start = line._a;
		vector2d end = line._b;
		KEY s, e;

		for (unsigned int i = 0; i < 4; ++i)
		{

			s = GenKey(start, rect);
			e = GenKey(end, rect);

			if ((s == e) && (s == 0))
			{
				//accept, all point inside the rect
				line.a(start);
				line.b(end);
				return CR_ACCEPTED;
			}
			int _b = 1 << i;
			if ((s & _b) && (e & _b))
			{
				//all point at same side
				return CR_REFUSED;
			}

			switch (i)
			{
			case 0:
			{
				//below
				if (s & _b)
				{
					float scale = (rect.bottom() - end.y + .0f) / (start.y - end.y);
					start.x = (start.x - end.x) * scale + end.x;
					start.y = rect.bottom();
				}
				if (e & _b)
				{
					float scale = (rect.bottom() - start.y + .0f) / (end.y - start.y);
					end.x = (end.x - start.x) * scale + start.x;
					end.y = rect.bottom();
				}
			}break;
			case 1:
			{
				//right
				if (s & _b)
				{
					float scale = (rect.right() - end.x + .0f) / (start.x - end.x);
					start.x = rect.right();
					start.y = (start.y - end.y) * scale + end.y;
				}
				if (e & _b)
				{
					float scale = (rect.right() - start.x + .0f) / (end.x - start.x);
					end.x = rect.right();
					end.y = (end.y - start.y) * scale + start.y;
				}
			}break;
			case 2:
			{
				//above
				if (s & _b)
				{
					float scale = (rect.top() - end.y) / (start.y - end.y);
					start.x = (start.x - end.x) * scale + end.x;
					start.y = rect.top();
				}
				if (e & _b)
				{
					float scale = (rect.top() - start.y) / (end.y - start.y);
					end.x = (end.x - start.x) * scale + start.x;
					end.y = rect.top();
				}
			}break;
			case 3:
			{
				//left
				if (s & _b)
				{
					float scale = (rect.left() - end.x) / (start.x - end.x);
					start.x = rect.left();
					start.y = (start.y - end.y) * scale + end.y;
				}
				if (e & _b)
				{
					float scale = (rect.left() - start.x) / (end.x - start.x);
					end.x = rect.left();
					end.y = (end.y - start.y) * scale + start.y;
				}
			}break;
			}
		}
		s = GenKey(start, rect);
		e = GenKey(end, rect);

		if ((s == e) && (s == 0))
		{
			//accept, all point inside the rect
			line.a(start);
			line.b(end);
			return CR_ACCEPTED;
		}
		else
		{
			return CR_REFUSED;
		}
	}
	

	polygon2d::polygon2d(const std::vector<vector2d>& pts) : _pts(pts), _num(pts.size()), _nms()
	{
		_convex = judge_convex(pts);
		compute_nms(pts, _nms);
	}
	polygon2d::~polygon2d() {}

	bool polygon2d::judge_convex(const std::vector<vector2d>& pts)
	{
		return true;
	}
	void polygon2d::compute_nms(const std::vector<vector2d>& pts, std::vector<vector2d>& nms)
	{}


	polygon2d polygons::Hexagon(f32 sz)
	{
		return polygon2d({});
	}
	polygon2d polygons::Hexagram(f32 sz)
	{
		f32 sz_2 = sz / 2.f,
			sz_3 = sz / 3.f,
			sz_4 = sz / 4.f,
			sz_6 = sz / 6.f;
		return polygon2d(
			{
				{sz_2, .0f}, {sz_3 * 2, sz_4}, {sz, sz_4},
				{sz - sz_6, sz_2}, {sz, sz_4 * 3}, {sz_3 * 2, sz_4 * 3},
				{sz_2, sz}, {sz_3, sz_4 * 3}, {0.f, sz_4 * 3},
				{sz_6, sz_2}, {0, sz_4}, {sz_3, sz_4}
			});
	}
}