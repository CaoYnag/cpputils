#pragma once
#include "type.h"
#include "color.h"
#include "math.h"
#include "image.h"
#include <vector>
using namespace spes;
using namespace spes::math;
using namespace spes::color;
using namespace spes::image;

namespace spes::canvas
{
	/*
	for paramter points: p1, p2, p3, p4 ... pn
	*/
	enum CANVAS_RENDER_MODE
	{
		CRM_None,
		/* just draw p1, p2 ... pn */
		CRM_Points,
		/* draw [p1, p2], [p3, p4] ... [pn-1, pn], and n must be an odd number */
		CRM_LineSegments,
		/* draw [p1, p2, p3 ... pn] */
		CRM_LineStrip,
		/* draw [p1, p2, p3 ... pn, p1] */
		CRM_LineRing
	};
	class Canvas
	{
	protected:
		image_t _im;
		u64 _size;
		color_t* _buff;

		inline u32 idx(u32 x, u32 y) const { return x + y * _im.width(); }
		inline u32 idx(const point2di& p) const { return p.x + p.y * _im.width(); }
	public:
		Canvas(u32 w, u32 h, color_t bk = Colors::WHITE);
		virtual ~Canvas();

		inline image_t& image() { return _im; }
	public:
		virtual void draw(u32 mode, const std::vector<point2di>& pts, color_t c = Colors::BLACK);
		virtual void draw_point(const point2di& p, color_t c = Colors::BLACK);
		virtual void draw_line(const line2di& l, color_t c = Colors::BLACK);
		virtual void draw_polygon(const polygon2di& poly);
	};
}