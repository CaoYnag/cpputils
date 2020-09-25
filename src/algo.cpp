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

    int sutherland_hodgman(const polygon2d& src, const rect& bnds, polygon2d& rslt)
    {
        auto sz = src._pts.size();
        if(sz < 3) return CR_REFUSED;
        vector<KEY> keys;
        for(int i = 0; i < sz; ++i)
            keys[i] = GenKey(src._pts[i], bnds);

        vector<vector2d> pts;
        for(int c = 0; c < sz; ++c)
        {
            int n = c + 1;
            n = n == sz ? 0 : n;
            auto s = keys[c];
            auto e = keys[n];

            if((s == e) && (s == 0)) // both inside, add second pt
                pts.emplace_back(src._pts[n]);
            else if(s & e); // same side, skip 2 pt.
            else if(s == 0) // s inside, add intersects pt
            {

            }
            else if(e == 0) // e inside, add intersect pt and e
            {}
            else
                cerr << "sth err." << endl;

        }

        if(pts.size() < 3) return CR_REFUSED;
        rslt.init(pts);
        return CR_ACCEPTED;
    }

    int sutherland_hodgman(const polygon2d& src, const polygon2d& bnds, polygon2d& rslt)
    {}
    int weiler_atherton(const polygon2d& src, const polygon2d& bnds, polygon2d& rslt)
    {}
}