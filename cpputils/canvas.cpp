#include "canvas.h"
using namespace std;
using namespace spes;
using namespace spes::math;
using namespace spes::color;
using namespace spes::image;

namespace spes::canvas
{
	Canvas::Canvas(u32 w, u32 h, color_t bk) : _im(), _size(w * h)
	{
		_im.init(w, h);
		_buff = _im.buffer();
		for (u64 i = 0; i < _size; ++i)
			_buff[i] = bk;
	}
	Canvas::~Canvas()
	{
		_buff = nullptr;
	}

	void Canvas::draw(u32 mode, const std::vector<point2di>& pts, color_t c)
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
				draw_line(line2di(pts[i], pts[i + 1]), c);
			}
		}break;
		case CRM_LineStrip:
		{
			for (u32 i = 0; i < pts.size() - 1; i += 1)
			{
				draw_line(line2di(pts[i], pts[i + 1]), c);
			}
		}break;
		case CRM_LineRing:
		{
			for (u32 i = 0; i < pts.size() - 1; i += 1)
			{
				draw_line(line2di(pts[i], pts[i + 1]), c);
			}
			draw_line(line2di(pts[0], pts[pts.size() - 1]), c);
		}break;
		}
	}
	void  Canvas::draw_point(const point2di& p, color_t c)
	{
		_buff[idx(p)] = c;
	}
	void  Canvas::draw_line(const line2di& l, color_t c)
	{
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
}