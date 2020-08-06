#include "canvas.h"
using namespace std;
using namespace spes;
using namespace spes::math;
using namespace spes::color;
using namespace spes::image;

namespace spes::canvas
{
	Canvas::Canvas(u32 w, u32 h, color_t bk) : _im(), _sz({ w + .0f, h + .0f }), _size(w* h), _rc(0, 0, w, h)
	{
		_im.init(w, h);
		_buff = _im.buffer();
		for (u64 i = 0; i < _size; ++i)
			_buff[i] = bk;
	}
	Canvas::Canvas(image_t& im) : _im(im), _sz({ im.width() + .0f, im.height() + .0f }), 
		_size(im.width() * im.height()), _rc(0, 0, im.width(), im.height())
	{
		_buff = _im.buffer();
	}
	Canvas::~Canvas()
	{
		_buff = nullptr;
	}

	void Canvas::draw(u32 mode, const std::vector<point2d>& pts, color_t c)
	{
		switch (mode)
		{
		default:
		case CRM_None:break;
		case CRM_Points:
		{
			for (auto& p : pts) draw_point(p, c);
		}break;
		case CRM_LineSegments:
		{
			for (u32 i = 0; i < pts.size() - 1; i += 2)
			{
				draw_line(line2d(pts[i], pts[i + 1]), c);
			}
		}break;
		case CRM_LineStrip:
		{
			for (u32 i = 0; i < pts.size() - 1; i += 1)
			{
				draw_line(line2d(pts[i], pts[i + 1]), c);
			}
		}break;
		case CRM_LineRing:
		{
			for (u32 i = 0; i < pts.size() - 1; i += 1)
			{
				draw_line(line2d(pts[i], pts[i + 1]), c);
			}
			draw_line(line2d(pts[0], pts[pts.size() - 1]), c);
		}break;
		}
	}
	void Canvas::draw_point(const point2d& p, color_t c)
	{
		_buff[idx(p)] = c;
	}
	void Canvas::draw_line(const line2d& line, color_t c)
	{
		line2d l = line;
		auto ret = cohen_sutherland(l, _rc);
		if (ret == CR_REFUSED) return;

		if (l.vertical())
		{
			s32 x = l._a.x;
			s32 iy = l._a.y;
			s32 ay = l._b.y;
			if (iy > ay)
			{
				s32 tmp = ay;
				ay = iy;
				iy = tmp;
			}

			for (; iy <= ay; ++iy)
			{
				_buff[idx(x, iy)] = c;
			}
		}
		else if (l.horizontal())
		{
			s32 ix = l._a.x;
			s32 ax = l._b.x;
			s32 y = l._a.y;
			if (ix > ax)
			{
				s32 tmp = ax;
				ax = ix;
				ix = tmp;
			}

			for (; ix <= ax; ++ix)
			{
				_buff[idx(ix, y)] = c;
			}
		}
		else
		{
			f32 x = l._a.x;
			f32 y = l._a.y;
			f32 inc = l._a.x > l._b.x ? -1 : 1;
			f32 yinc = l._k * inc;
			f32 yinci = yinc > 0 ? 1 : -1;
			while ((x - l._b.x) * inc < 0)
			{
				f32 nx = x + inc;
				f32	ny = y + yinc;
				while ((y - ny) * yinc < 0)
				{
					_buff[idx(x, y)] = c;
					y += yinci;
				}

				x = nx;
				y = ny;
			}
			_buff[idx(l._b.x, l._b.y)] = c;
		}
	}


	void Canvas::draw_rect(const rect& rec, color_t c)
	{
		rect rc(0, 0, 0, 0);
		if (intersects(_rc, rec, rc))
		{
			color_t* l = _buff + (s32)(rc.top() * _sz.w);

			// may be memset would be better?
			for (u32 y = rc.top(); y < rc.bottom(); ++y)
			{
				for (u32 x = rc.left(); x < rc.right(); ++x)
					l[x] = c;
				l += (s32)_sz.w;
			}
		}
	}
	void Canvas::draw_triangle(const std::vector<point2d>& tri, color_t c)
	{
		if (tri.size() < 3) return; // not a triangle
		if (tri[0].y == tri[2].y && tri[1].y == tri[2].y) return; // a vertical line
		if (tri[0].x == tri[2].x && tri[1].x == tri[2].x) return; // a horizontal line
		// situation 1 or 2
		if (tri[0].y == tri[1].y || tri[0].y == tri[2].y || tri[1].y == tri[2].y)
		{
			point2d h1, h2, v;
			if (tri[0].y == tri[1].y)
			{
				h1 = tri[0];
				h2 = tri[1];
				v = tri[2];
			}
			else if (tri[0].y == tri[2].y)
			{
				h1 = tri[0];
				h2 = tri[2];
				v = tri[1];
			}
			else
			{
				h1 = tri[1];
				h2 = tri[2];
				v = tri[0];
			}

			s32 inc = v.y > h1.y ? 1 : -1;
			line2d l1(v, h1), l2(v, h2);
			for (s32 y = h1.y; (y - v.y) * inc <= 0; y += inc)
			{
				f32 x1, x2;
				l1.resolv_x(y, x1);
				l2.resolv_x(y, x2);

				draw_line(line2d({ x1, y + .0f }, { x2, y + .0f }), c);
			}
			return;
		}

		// situation 3
		point2d top, mid, bot;
		auto max_y = spes::math::fmax(spes::math::fmax(tri[0].y, tri[1].y), tri[2].y);
		auto min_y = spes::math::fmin(spes::math::fmin(tri[0].y, tri[1].y), tri[2].y);
		for (int i = 0; i < 3; ++i)
		{
			if (tri[i].y == max_y)
				bot = tri[i];
			else if (tri[i].y == min_y)
				top = tri[i];
			else mid = tri[i];
		}
		line2d s1(top, mid), s2(mid, bot), h(top, bot);
		for (s32 y = top.y; y <= bot.y; ++y)
		{
			f32 x1, x2;
			if (y >= mid.y)
				s2.resolv_x(y, x1);
			else s1.resolv_x(y, x1);
			h.resolv_x(y, x2);

			draw_line(line2d({ x1, y + .0f }, { x2, y + .0f }), c);
		}
	}
	void Canvas::draw_polygon(const polygon2d& poly, color_t c)
	{
		vector<point2d> tmp = poly._pts;
		while (tmp.size() > 2)
		{
			int idx = 0;
			for (; idx < tmp.size() - 2; ++idx)
			{
				auto pt1 = tmp[idx], pt2 = tmp[idx + 1], pt3 = tmp[idx + 2];
				auto v1 = pt2 - pt1, v2 = pt3 - pt2;
				auto r1 = cross(v1, v2);
				if (r1 > 0)
				{
					draw_triangle({ pt1, pt2, pt3 }, c);
					tmp.erase(tmp.begin() + idx + 1);

					if (idx == 0)
					{
						pt1 = tmp[tmp.size() - 1], pt2 = tmp[idx], pt3 = tmp[idx + 1];
						v1 = pt2 - pt1, v2 = pt3 - pt2;
						r1 = cross(v1, v2);

						if (r1 == 0)
							tmp.erase(tmp.begin() + idx);
					}
					break;
				}
				if (r1 == 0)
				{
					tmp.erase(tmp.begin() + idx + 1);
					break;
				}
			}
		}
	}
}