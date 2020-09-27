#include <iostream>
#include <spes/canvas.h>
#include <spes/imageio.h>
#include <spes/math.h>
#include <spes/algo.h>
#include <vector>
#include <stdexcept>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;
using namespace spes;
using namespace spes::algo;
using namespace spes::image;
using namespace spes::math;
using namespace spes::image::io;
using namespace spes::canvas;
#undef GREEN
#undef RED
#undef BLACK
#undef BLUE
#undef AQUA

int sutherland_hodgman_custom(const polygon2d& src, const rect& bnds, vector<polygon2d>& polys)
{
    vector<vector<vector2d>> srcs;
    srcs.emplace_back(src._pts);
    vector<vector<vector2d>> dsts;
    vector<vector2d> pts;
    vector<vector2d> tmp;

    const int DEFAULT = 0;
    const int IN = 1;
    const int OUT = 2;
    vector<int> dirs;

    bool flag = false;
    int s = 0, e = 0;

    // clip with top edge
    flag = false;
    int first_in = -1;
    for(auto& src_pts : srcs)
    {
        tmp.clear();
        pts.clear();
        dirs.clear();
        for(int i = 0; i < src_pts.size(); ++i)
        {
            auto& c = src_pts[i];
            auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
            s = c.y > bnds.top();
            e = n.y > bnds.top();

            int r = s + e;
            if(r == 0); // both outside
            else if(r == 2) // both inside
            {
                pts.push_back(n);
                dirs.push_back(DEFAULT);
            }
            else if(s == 1) // goes out clip window
            {
                f32 x;
                line2d(c, n).resolv_x(bnds.top(), x);
                pts.push_back({x, bnds.top()});
                dirs.push_back(OUT);
            }
            else if(e == 1) // goes in clip window
            {
                f32 x;
                line2d(c, n).resolv_x(bnds.top(), x);
                pts.push_back({x, bnds.top()});
                if(first_in == -1) first_in = dirs.size();
                dirs.push_back(IN);
                pts.push_back(n);
                dirs.push_back(DEFAULT);
            }
        }
        // split poly by dirs
        for(int i = 0; i < pts.size(); ++i)
        {
            int idx = i + first_in;
            idx = idx >= pts.size() ? idx - pts.size() : idx;
            if(dirs[idx] == IN)
                cout << ">" << pts[idx];
            else if(dirs[idx] == OUT)
                cout << pts[idx] << "<" << endl;
            else cout << "-";
        }
    }
    for(auto& dst_pts : dsts)
        polys.emplace_back(dst_pts);
    return CR_ACCEPTED;
}

void test_draw()
{
    const u32 SIZE = 700;
    Canvas canvas(SIZE, SIZE);
    Canvas canvas2(SIZE, SIZE);
    Canvas canvas3(SIZE, SIZE);
    Canvas canvas4(SIZE, SIZE);

    {
        rect rc(50, 50, 650, 650);
        vector<point2d> bnds = {{50, 50}, {50, 650}, {650, 650}, {650, 50}};
        vector<point2d> pts = {{200, 200}, {200, 400}, {400, 400}, {400, 200}};
        polygon2d src(pts);
        vector<polygon2d> out;
        sutherland_hodgman_custom(src, rc, out);

        canvas.draw(CRM_LineRing, bnds, Colors::AQUA);
        canvas.draw(CRM_LineRing, pts);
        for(auto& poly : out)
            canvas.draw(CRM_LineRing, poly._pts, Colors::GREEN);
    }

    {
        rect rc(50, 400, 650, 650);
        vector<point2d> bnds = {{50, 400}, {50, 650}, {650, 650}, {650, 400}};
        //vector<point2d> pts = {{100, 100}, {100, 600}, {200, 600}, {200, 300}, {300, 300}, {300, 600}, {400, 600}, {400, 100}};
        vector<point2d> pts = {{100, 100}, {100, 600}, {600, 600}, {600, 100}};
        polygon2d src(pts);
        vector<polygon2d> out;
        sutherland_hodgman_custom(src, rc, out);

        canvas2.draw(CRM_LineRing, bnds, Colors::AQUA);
        canvas2.draw(CRM_LineRing, pts);
        for(auto& poly : out)
            canvas2.draw(CRM_LineRing, poly._pts, Colors::GREEN);
    }

    {
        rect rc(50, 400, 650, 650);
        vector<point2d> bnds = {{50, 400}, {50, 650}, {650, 650}, {650, 400}};
        vector<point2d> pts = {{100, 100}, {100, 600}, {200, 600}, {200, 500}, {300, 500}, {300, 600}, {400, 600}, {400, 100}};
        polygon2d src(pts);
        vector<polygon2d> out;
        sutherland_hodgman_custom(src, rc, out);

        canvas3.draw(CRM_LineRing, bnds, Colors::AQUA);
        canvas3.draw(CRM_LineRing, pts);
        for(auto& poly : out)
            canvas3.draw(CRM_LineRing, poly._pts, Colors::GREEN);
    }
    {
        rect rc(50, 400, 650, 650);
        vector<point2d> bnds = {{50, 400}, {50, 650}, {650, 650}, {650, 400}};
        vector<point2d> pts = {{100, 100}, {100, 600}, {350, 300}, {600, 600}, {600, 100}};
        polygon2d src(pts);
        vector<polygon2d> out;
        sutherland_hodgman_custom(src, rc, out);

        canvas4.draw(CRM_LineRing, bnds, Colors::AQUA);
        canvas4.draw(CRM_LineRing, pts);
        for(auto& poly : out)
        {
            canvas4.draw(CRM_LineRing, poly._pts, Colors::GREEN);
            cout << "splited: ";
            for (auto &v : poly._pts)
                cout << v << " ";
            cout << endl;
        }
    }
    image_io::show_image(canvas.image(), "c1");
    image_io::show_image(canvas2.image(), "c2");
    image_io::show_image(canvas3.image(), "c3");
    image_io::show_image(canvas4.image(), "c4");
    getch();
}
void test_clip()
{
    rect rc(0, 0, 100, 100);
    vector<point2d> pts = {{10, -10}, {50, 10}, {90, -10}, {90, 90}, {10, 90}};
    polygon2d src(pts);
    vector<polygon2d> out;
    sutherland_hodgman_custom(src, rc, out);
}
int main(int argc, char** argv)
{
    // test_draw();
    test_clip();
	return 0;
}