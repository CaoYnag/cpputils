#include <iostream>
#include <spes/algo.h>
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

	marked_point(point2d& p, u32 s) : pt(p), stat(s) {}
};
enum WA_MARKS
{
	WA_EMPTY = 0,
	WA_IN = 1,
	WA_OUT = 2
};
const char* marks_ascii[] =
{
	"emp",
	"in ",
	"out"
};
void display_marked_pts(vector<marked_point>& pts, const string& text = "marked poly:")
{
	cout << text << ":";
	for(int i = 0; i < pts.size(); ++i)
	{
		if(i % 3 == 0) printf("\n");
		auto& pt = pts.at(i);
		printf("(%.2f, %.2f %s) ", pt.pt.x, pt.pt.y, marks_ascii[pt.stat]);
	}
	printf("\n");
}
void wa_contruct_polys(vector<marked_point>& pts, vector<polygon2d>& rslt)
{
		display_marked_pts(pts);
}
int weiler_atherton(const polygon2d& src, const rect& rc, vector<polygon2d>& rslt)
{
	auto&& src_poly = src.points();
	vector<marked_point> pts;
	int idx = 0;
	int next = 0;
	KEY s, e;

	while(idx < src.num())
	{
		next = (idx == (src.num() - 1)) ? 0 : idx + 1;
		point2d sp = src_poly[idx];
		point2d ep = src_poly[next];
		point2d rslt;
		++idx;
		s = GenKey(sp, rc);
		e = GenKey(ep, rc);

		// do cohen_sutherland and mark stat
		if(s == e && !s) // both inside
		{
			pts.emplace_back(sp, 0);
			pts.emplace_back(ep, 0);
			continue;
		}
		// c-s clip
		pts.emplace_back(sp, WA_EMPTY);
		line2d line(sp, ep);

		for(int i = 0; i < 4; ++i)
		{
			u32 mask = i << i;
			switch (i)
			{
				case 0:
				{
					//below
					rslt.y = rc.bottom();
					line.resolv_x(rslt.y, rslt.x);
				}break;
				case 1:
				{
					//right
					rslt.x = rc.right();
					line.resolv_y(rslt.x, rslt.y);
				}break;
				case 2:
				{
					//above
					rslt.y = rc.top();
					line.resolv_x(rslt.y, rslt.x);
				}break;
				case 3:
				{
					//left
					rslt.x = rc.right();
					line.resolv_y(rslt.x, rslt.y);
				}break;
			}

			if (s & mask) // s out of rc, so must be a in line
				pts.emplace_back(rslt, WA_IN);
			if (e & mask) // e out of rc, so must be a out line
				pts.emplace_back(rslt, WA_OUT);
			// pts.emplace_back(ep, WA_EMPTY); // avoid repeat pts.
		}
	}
	wa_contruct_polys(pts, rslt);
	return rslt.size() == 0;
}

void test()
{
	rect rc (-10, -10, 10, 10);
	vector<vector2d> pts = {{-13, 0}, {0, -13}, {13, 0}, {0, 13}}; // diamond
	vector<vector2d> rct_ver = {{0, 0}, {0, 20}, {20, 20}, {20, 0}};
	polygon2d poly = rct_ver;
	vector<polygon2d> rslt;
	weiler_atherton(poly, rc, rslt);
}

int main(int argc, char* argv[])
{
	test();
    return 0;
}