#include <spes/math.h>
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

    std::ostream& operator<<(std::ostream& os, const vector2d& v)
    {
	    os << "(" << v.x << ", " << v.y << ")";
	    return os;
    }

	f32 cross(vector2d& v1, vector2d& v2)
	{
		return v1.x * v2.y - v1.y * v2.x;
	}

    int range_coverage(range2d& a, range2d& b)
    {
        if(a.e < b.s || a.s > b.e) return RCS_INDEPENDENT;

        if(in_range(a.s, b.s, b.e) && in_range(a.e, b.s, b.e)) return RCS_B_COVER_A;
        if(in_range(b.s, a.s, a.e) && in_range(b.e, a.s, a.e)) return RCS_A_COVER_B;
        return RCS_CROSS;
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

	bool line2d::resolv_x(f32 y, f32& x) const
	{
		if (vertical())
			x = _a.x;
		else
			x = _a.x + (y - _a.y) / _k;
		return in_range(y, _a.y, _b.y) || in_range(y, _b.y, _a.y);
	}

	bool line2d::resolv_y(f32 x, f32& y) const
	{
		if (horizontal())
			y = _a.y;
		else
			y = (x - _a.x) * _k + _a.y;
		return in_range(x, _a.x, _b.x) || in_range(x, _b.x, _a.x);
	}

	bool line2d::contains(const point2d& pt) const
    {
	    if(horizontal())
	        return pt.y == _a.y && (in_range(pt.x, _a.x, _b.x) || in_range(pt.x, _b.x, _a.x));
	    if(vertical())
	        return pt.x == _a.x && (in_range(pt.y, _a.y, _b.y) || in_range(pt.y, _b.y, _a.y));
	    f32 k = (pt.y - _a.y) / (pt.x - _a.x);
	    return k == _k && (in_range(pt.x, _a.x, _b.x) || in_range(pt.x, _b.x, _a.x)) && (in_range(pt.y, _a.y, _b.y) || in_range(pt.y, _b.y, _a.y));
    }
    bool line2d::on(const point2d& pt) const
    {
        if(horizontal())
            return pt.y == _a.y;
        if(vertical())
            return pt.x == _a.x;
        f32 k = (pt.y - _a.y) / (pt.x - _a.x);
        return k == _k;
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


	polygon2d::polygon2d(const std::vector<vector2d>& pts)
	{
		init(pts);
	}
	polygon2d::polygon2d(const polygon2d& poly)
	{
	    init(poly);
	}
	polygon2d::~polygon2d() {}

	bool polygon2d::judge_convex(const std::vector<vector2d>& pts)
	{
		// TODO complete this later
		return true;
	}
	void polygon2d::compute_nms(const std::vector<vector2d>& pts, std::vector<vector2d>& nms)
	{
		// TODO complete this later
	}

    void polygon2d::init(const std::vector<vector2d>& pts)
    {
	    _pts.assign(pts.begin(), pts.end());
	    _num = pts.size();
	    _convex = judge_convex(pts);
        compute_nms(pts, _nms);
    }
    void polygon2d::init(const polygon2d& o)
    {
	    _pts.assign(o._pts.begin(), o._pts.end());
	    _nms.assign(o._nms.begin(), o._nms.end());
	    _convex = o._convex;
	    _num = o._num;
    }

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