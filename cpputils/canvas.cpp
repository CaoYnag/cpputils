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
	Canvas::Canvas(image_t& im) : _im(im), _sz({ im.width(), im.height() }), 
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
			s32 x = l._a.x;
			f32 y = l._a.y;
			s32 inc = l._a.x > l._b.x ? -1 : 1;
			s32 yinc = l._k * inc;
			s32 yinci = yinc > 0 ? 1 : -1;
			while(x < l._b.x)
			{
				s32 nx = x + inc;
				f32	ny = y + yinc;
				while (y != ny)
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
	{}
	void Canvas::draw_polygon(const polygon2d& poly, color_t c)
	{}
}