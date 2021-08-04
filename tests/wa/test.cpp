#include <iostream>
#include <spes/algo.h>
#include <list>
#include <cassert>
using namespace std;
using namespace spes::math;

enum CutRet
{
	CR_ACCEPTED = 0,
	CR_REFUSED = 1,
};
enum CSBIT
{
	CB_BELOW = 0x01,//0001
	CB_RIGHT = 0x02,//0010
	CB_ABOVE = 0x04,//0100
	CB_LEFT = 0x08,//1000

	CB_BELOW_INV = 0xfe,//1110
	CB_RIGHT_INV = 0xfd,//1101
	CB_ABOVE_INV = 0xfb,//1011
	CB_LEFT_INV = 0xf7,//
};
typedef u8 KEY;

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
struct marked_point
{
	point2d pt;
	u32 stat;
	f32 t;
	f32 w;

	marked_point(const point2d& p, u32 s) : pt(p), stat(s), t(-1), w(-1) {}
	marked_point(const point2d& p, u32 s, f32 tt, f32 ww) : pt(p), stat(s), t(tt), w(ww) {}
};
enum WA_MARKS
{
	WA_EMPTY    = 0,
	WA_IN       = 1,
	WA_OUT      = 2,

	WA_TAR      = 0x10,
	WA_WND      = 0x20,
	WA_INT      = 0x30
};
const char* marks_ascii[] =
{
	"emp",
	"in ",
	"out",

	"-----splitter------",

	"tar",
	"wnd",
	"int"
};
const int MASK = 0xf;
char* str_mark(int mark)
{
	static char buff[2048];
	sprintf(buff, "%s | %s", marks_ascii[mark & MASK], marks_ascii[3 + (mark >> 4)]);
	return buff;
}

template<template<typename> class Container = std::vector>
void display_marked_pts(Container<marked_point>& pts, const string& text = "marked poly:")
{
	cout << text << ":\n";
	for(int i = 0; i < pts.size(); ++i)
	{
		auto& pt = pts.at(i);
		printf("(%.2f, %.2f) %s %.2f %.2f\n", pt.pt.x, pt.pt.y, str_mark(pt.stat), pt.t, pt.w);
	}
	printf("\n");
}
void wa_contruct_polys(vector<marked_point>& pts, vector<polygon2d>& rslt)
{
	display_marked_pts(pts);
}

template<template<typename> class Container = std::vector>
void mark_points(Container<marked_point>& pts, int stat, const vector<point2d>& src)
{
	int t = -1, w = -1;
	for(int i = 0; i < src.size(); ++i)
	{
		if(stat == WA_TAR) t = i;
		if(stat == WA_WND) w = i;
		pts.emplace_back(src[i], stat, t, w);
	}
}

void show_poly(const polygon2d& poly)
{
	for(auto& pt : poly.points())
		printf("(%.2f, %.2f) ", pt.x, pt.y);
	printf("\n");
}

void show_polys(const vector<polygon2d>& polys)
{
	int cnt = 0;
	for(auto& poly : polys)
	{
		cout << "polygon " << (++cnt) << ":\n";
		show_poly(poly);
	}
}

template<template<typename> class Container = std::vector>
int weiler_atherton(const polygon2d& target, const polygon2d& window, vector<polygon2d>& rslt)
{
	Container<marked_point> tar;
	Container<marked_point> wnd;
	Container<marked_point> ints;
	// 1. mark all points.
	 mark_points<Container>(tar, WA_TAR, target.points());
	 mark_points<Container>(wnd, WA_WND, window.points());
	 display_marked_pts<Container>(tar, "target");
	 display_marked_pts<Container>(wnd, "window");

	// 2. find out all intersections, and insert into the 2 array.
	point2d tmp;
	for(int we = 0; we < window.num(); ++we)
	{
		auto wnd_edge = window.edge(we);
//		wnd.emplace_back(wnd_edge->_a, WA_WND);
		for(int te = 0; te < target.num(); ++te)
		{
			auto tar_edge = target.edge(te);
//			tar.empplace_back(tar_edge->_a, WA_TAR);
			if(wnd_edge->seg_intersect(*tar_edge, tmp))
			{
				// printf("(%.2f, %.2f) not on edge w %d and t %d.\n", tmp.x, tmp.y, we, te);
				continue;
			}

			// find out in or out
			int stat = WA_INT;
			int v = (tmp - tar_edge->_a) * window.normals().at(we);
			if(v > 0) stat |= WA_OUT; // all normals goes out, so if the dot result is bigger than 0, so this line goes out.
			else stat |= WA_IN;
			ints.emplace_back(tmp, stat, te, we);
		}
	}
	 display_marked_pts<Container>(ints, "intersects");

	// 3. insert and rearrange intersected points
	for(auto& pt : ints)
	{
		auto it = find_if(tar.begin(), tar.end(), [&](const marked_point& p){return p.t == pt.t;});
		{
			// 'it' must be edge of tar
			// so compute dist(pt, it), set pt.t += 1 / (1 + dist)
			// do same tricks in wnd insert.
			auto v = pt.pt - it->pt;
			f32 c = 1 / (v * v + 1); // this always less than 1
			pt.t += c;
		}
		tar.insert(it + 1, pt);
		it = find_if(wnd.begin(), wnd.end(), [&](const marked_point& p){return p.w == pt.w;});
		{
			auto v = pt.pt - it->pt;
			f32 c = 1 / (v * v + 1);
			pt.w += c;
		}
		wnd.insert(it + 1, pt);
	}
	cout << "inserted:\n";
	display_marked_pts<Container>(tar, "target");
	display_marked_pts<Container>(wnd, "window");

	sort(tar.begin(), tar.end(), [](const marked_point& p1, const marked_point& p2){return p2.t > p1.t;});
	sort(wnd.begin(), wnd.end(), [](const marked_point& p1, const marked_point& p2){return p2.w > p1.w;});


	cout << "rearraged:\n";
	display_marked_pts<Container>(tar, "target");
	display_marked_pts<Container>(wnd, "window");

	// 4. construct polygons.
	// 1) find an `in` pt from tar and record it `S`
	// 2) unmark `in` pt, add pts one by one to output from tar
	// 3) if meet an `out` pt, turn to wnd, found out position of this `out` pt.
	// 4) add pts to output from wnd
	// 5) then must meet an `in` pt, if this pt is `S`, finish a polygon, and back to step 1).
	//      otherwise, just back to step 2.
	// TODO how do we identify points has same position?
	vector<vector2d> poly;
	decltype(tar.begin()) S;
	while(true)
	{
		auto it = find_if(tar.begin(), tar.end(), [](const marked_point& pt){return pt.stat & WA_IN;});
		if(it == tar.end()) // no more `in` pts, done
			break;
		S = it;
		poly_in:
		it->stat &= ~WA_IN; // unmark
		poly.push_back(it->pt);
		while(true)
		{
			++it;
			if(it == tar.end()) it = tar.begin();

			assert(it != S && "fatal error, no match out point for a in point.");

			poly.push_back(it->pt);
			if(it->stat & WA_OUT) // meet `out` point.
				break;
		}
		// find the `out` pt in wnd
		it = find_if(wnd.begin(), wnd.end(), [&](const marked_point& pt){return pt.pt == it->pt;});

		assert(it != wnd.end() && "no match out point in wnd.");

		while(true)
		{
			++it;
			if(it == wnd.end()) it = wnd.begin();
			if(it->stat & WA_IN)
			{
				if(it->pt == S->pt) // meet start.
				{
					rslt.emplace_back(poly);
					poly.clear();
					break; // a new round.
				}
				else goto poly_in;
			}
			else poly.push_back(it->pt);
		}
	}

	return rslt.size() == 0;
}

void test()
{
	rect rc (-10, -10, 10, 10);
	vector<vector2d> wnd_pts = {{-10, -10}, {-10, 10}, {10, 10}, {10, -10}};
	polygon2d wnd = wnd_pts;
	vector<vector2d> pts = {{-13, 0}, {0, -13}, {13, 0}, {0, 13}}; // diamond
	vector<vector2d> rct_ver = {{0, 0}, {0, 20}, {20, 20}, {20, 0}};
	polygon2d poly = pts;
	vector<polygon2d> rslt;
	weiler_atherton<vector>(poly, wnd, rslt);

	show_polys(rslt);
}

int main(int argc, char* argv[])
{
	test();
    return 0;
}