#include <spes/algo.h>
#include <vector>
using namespace std;
using namespace spes::math;

namespace spes::algo
{
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

    int cohen_sutherland(line2d& line, const rect& rc)
    {
        vector2d start = line._a;
        vector2d end = line._b;
        KEY s, e;

        for (unsigned int i = 0; i < 4; ++i)
        {

            s = GenKey(start, rc);
            e = GenKey(end, rc);

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
                        float scale = (rc.bottom() - end.y + .0f) / (start.y - end.y);
                        start.x = (start.x - end.x) * scale + end.x;
                        start.y = rc.bottom();
                    }
                    if (e & _b)
                    {
                        float scale = (rc.bottom() - start.y + .0f) / (end.y - start.y);
                        end.x = (end.x - start.x) * scale + start.x;
                        end.y = rc.bottom();
                    }
                }break;
                case 1:
                {
                    //right
                    if (s & _b)
                    {
                        float scale = (rc.right() - end.x + .0f) / (start.x - end.x);
                        start.x = rc.right();
                        start.y = (start.y - end.y) * scale + end.y;
                    }
                    if (e & _b)
                    {
                        float scale = (rc.right() - start.x + .0f) / (end.x - start.x);
                        end.x = rc.right();
                        end.y = (end.y - start.y) * scale + start.y;
                    }
                }break;
                case 2:
                {
                    //above
                    if (s & _b)
                    {
                        float scale = (rc.top() - end.y) / (start.y - end.y);
                        start.x = (start.x - end.x) * scale + end.x;
                        start.y = rc.top();
                    }
                    if (e & _b)
                    {
                        float scale = (rc.top() - start.y) / (end.y - start.y);
                        end.x = (end.x - start.x) * scale + start.x;
                        end.y = rc.top();
                    }
                }break;
                case 3:
                {
                    //left
                    if (s & _b)
                    {
                        float scale = (rc.left() - end.x) / (start.x - end.x);
                        start.x = rc.left();
                        start.y = (start.y - end.y) * scale + end.y;
                    }
                    if (e & _b)
                    {
                        float scale = (rc.left() - start.x) / (end.x - start.x);
                        end.x = rc.left();
                        end.y = (end.y - start.y) * scale + start.y;
                    }
                }break;
            }
        }
        s = GenKey(start, rc);
        e = GenKey(end, rc);

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

    void split_polygon(const vector<point2d>& pts, vector<polygon2d>& polys)
    {
        if(pts.size() < 3) return;
        vector<vector2d> tmp;
        for(auto& p : pts)
        {
            //if(tmp.size() > 0 && (tmp[tmp.size() - 1] == p)) continue;
            if(tmp.size() >= 2)
            {
                line2d l(p, tmp[tmp.size() - 1]);
                bool find = false;
                for(int k = 0; k < tmp.size() - 1; ++k)
                {
                    if(l.contains(tmp[k]))
                    {
                        find = true;
                        break;
                    }
                }

                if(find)
                {
                    if(tmp.size() > 2) polys.emplace_back(tmp);
                    tmp.clear();
                }
            }
            tmp.push_back(p);
        }
        if(tmp.size() > 2) polys.emplace_back(tmp);
    }

    int sutherland_hodgman(const polygon2d& src, const rect& bnds, vector<polygon2d>& polys)
    {
        vector<vector<vector2d>> srcs;
        srcs.emplace_back(src._pts);
        vector<vector<vector2d>> dsts;
        vector<vector2d> pts;
        vector<vector2d> tmp;

        bool flag = false;
        int s = 0, e = 0;

        // clip with left edge
        for(auto& src_pts : srcs)
        {
            for(int i = 0; i < src_pts.size(); ++i)
            {
                auto& c = src_pts[i];
                auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
                s = c.x > bnds.left();
                e = n.x > bnds.left();

                int r = s + e;
                if(r == 0); // both outside
                else if(r == 2) // both inside
                    pts.push_back(n);
                else if(s == 1) // goes out clip window
                {
                    f32 y;
                    line2d(c, n).resolv_y(bnds.left(), y);
                    pts.push_back({bnds.left(), y});
                    if(flag)
                    {
                        dsts.emplace_back(pts);
                        pts.clear();
                        flag = false;
                    }
                    else if(pts.size() > 0) // bak pts to tmp
                    {
                        tmp.assign(pts.begin(), pts.end());
                        pts.clear();
                    }
                }
                else if(e == 1) // goes in clip window
                {
                    flag = true;
                    f32 y;
                    line2d(c, n).resolv_y(bnds.left(), y);
                    pts.push_back({bnds.left(), y});
                    pts.push_back(n);
                }
            }
            if(pts.size() > 0) tmp.insert(tmp.end(), pts.begin(), pts.end());
            if(tmp.size() > 0) dsts.emplace_back(tmp);
        }



        // clip with right edge
        srcs.clear();
        srcs.swap(dsts);
        flag = false;
        for(auto& src_pts : srcs)
        {
            tmp.clear();
            pts.clear();
            for(int i = 0; i < src_pts.size(); ++i)
            {
                auto& c = src_pts[i];
                auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
                s = c.x < bnds.right();
                e = n.x < bnds.right();

                int r = s + e;
                if(r == 0); // both outside
                else if(r == 2) // both inside
                    pts.push_back(n);
                else if(s == 1) // goes out clip window
                {
                    f32 y;
                    line2d(c, n).resolv_y(bnds.right(), y);
                    pts.push_back({bnds.right(), y});
                    if(flag)
                    {
                        dsts.emplace_back(pts);
                        pts.clear();
                        flag = false;
                    }
                    else if(pts.size() > 0) // bak pts to tmp
                    {
                        tmp.assign(pts.begin(), pts.end());
                        pts.clear();
                    }
                }
                else if(e == 1) // goes in clip window
                {
                    flag = true;
                    f32 y;
                    line2d(c, n).resolv_y(bnds.right(), y);
                    pts.push_back({bnds.right(), y});
                    pts.push_back(n);
                }
            }
            if(pts.size() > 0) tmp.insert(tmp.end(), pts.begin(), pts.end());
            if(tmp.size() > 0) dsts.emplace_back(tmp);
        }

        // clip with top edge
        srcs.clear();
        srcs.swap(dsts);
        flag = false;
        for(auto& src_pts : srcs)
        {
            tmp.clear();
            pts.clear();
            for(int i = 0; i < src_pts.size(); ++i)
            {
                auto& c = src_pts[i];
                auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
                s = c.y > bnds.top();
                e = n.y > bnds.top();

                int r = s + e;
                if(r == 0); // both outside
                else if(r == 2) // both inside
                    pts.push_back(n);
                else if(s == 1) // goes out clip window
                {
                    f32 x;
                    line2d(c, n).resolv_x(bnds.top(), x);
                    pts.push_back({x, bnds.top()});
                    if(flag)
                    {
                        dsts.emplace_back(pts);
                        pts.clear();
                        flag = false;
                    }
                    else if(pts.size() > 0) // bak pts to tmp
                    {
                        tmp.assign(pts.begin(), pts.end());
                        pts.clear();
                    }
                }
                else if(e == 1) // goes in clip window
                {
                    flag = true;
                    f32 x;
                    line2d(c, n).resolv_x(bnds.top(), x);
                    pts.push_back({x, bnds.top()});
                    pts.push_back(n);
                }
            }
            if(pts.size() > 0) tmp.insert(tmp.end(), pts.begin(), pts.end());
            if(tmp.size() > 0) dsts.emplace_back(tmp);
        }
        // clip with bottom edge
        srcs.clear();
        srcs.swap(dsts);
        flag = false;
        for(auto& src_pts : srcs)
        {
            tmp.clear();
            pts.clear();
            for(int i = 0; i < src_pts.size(); ++i)
            {
                auto& c = src_pts[i];
                auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
                s = c.y < bnds.bottom();
                e = n.y < bnds.bottom();

                int r = s + e;
                if(r == 0); // both outside
                else if(r == 2) // both inside
                    pts.push_back(n);
                else if(s == 1) // goes out clip window
                {
                    f32 x;
                    line2d(c, n).resolv_x(bnds.bottom(), x);
                    pts.push_back({x, bnds.bottom()});
                    if(flag)
                    {
                        dsts.emplace_back(pts);
                        pts.clear();
                        flag = false;
                    }
                    else if(pts.size() > 0) // bak pts to tmp
                    {
                        tmp.assign(pts.begin(), pts.end());
                        pts.clear();
                    }
                }
                else if(e == 1) // goes in clip window
                {
                    flag = true;
                    f32 x;
                    line2d(c, n).resolv_x(bnds.bottom(), x);
                    pts.push_back({x, bnds.bottom()});
                    pts.push_back(n);
                }
            }
            if(pts.size() > 0) tmp.insert(tmp.end(), pts.begin(), pts.end());
            if(tmp.size() > 0) dsts.emplace_back(tmp);
        }

        for(auto& dst_pts : dsts)
            polys.emplace_back(dst_pts);
        return CR_ACCEPTED;
    }

    int sutherland_hodgman(const polygon2d& src, const polygon2d& bnds, vector<polygon2d>& polys)
    {
        // TODO to be finished
        return CR_REFUSED;
    }
    int weiler_atherton(const polygon2d& src, const polygon2d& bnds, vector<polygon2d>& polys)
    {
        // TODO to be finished
        return CR_REFUSED;
    }
}