#include <spes/algo.h>
#include <vector>
#include <cassert>
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


    constexpr const int AXIS_X = 0;
    constexpr const int AXIS_Y = 1;
    enum SH_MASKS
    {
        SH_OUT_MASK = 1 << 31,
        SH_IN_MASK = 1 << 30,
        SH_VALUE_MASK = ~(SH_IN_MASK | SH_OUT_MASK)
    };
    // split
    void split_polygon(const vector<vector2d>& pts, const vector<vector<int>>& idxs, vector<vector<vector2d>>& splited)
    {
        for(auto& idx : idxs)
        {
            vector<point2d> tmp;
            for(auto i : idx)
                tmp.emplace_back(pts[i]);
            splited.emplace_back(tmp);
        }
    }
    void resolv_independent_ee(vector<point2d>& pts, vector<int>& recs, int axis, vector<vector<int>>& idxs)
    {
        if(pts.size() != recs.size()) return; // TODO exception or asssertion needed here.

        // find an entry pt
        int first_idx = -1;
        for(int i = 0; i < pts.size(); ++i)
        {
            if(recs[i] & SH_IN_MASK)
            {
                first_idx = i;
                break;
            }
        }

        // check point 1: no entry and exists, so all pts IN clip wnd, just add them
        if(first_idx == -1)
        {
            idxs.emplace_back();
            for(int i = 0; i < pts.size(); ++i) idxs[0].emplace_back(i);
            return;
        }

        vector<range2d> in, out; // store in or out pt's {idx, value}
        // mark in and out pts.
        for(int i = 0; i < pts.size(); ++i)
        {
            int ri = (i + first_idx) % pts.size();
            if(recs[ri] & SH_IN_MASK) in.push_back({ri + .0f, axis ? pts[ri].y : pts[ri].x});
            if(recs[ri] & SH_OUT_MASK) out.push_back({ri + .0f, axis ? pts[ri].y : pts[ri].x});
        }

        if(in.size() != out.size()) return;

        // check point 2: after recog entries and exits,
        //  if only 1 [entry, exit] pair exits, which also means all pt IN clip wnd,
        //  just add all pt
        if(in.size() < 2)
        {
            idxs.emplace_back();
            for(int i = 0; i < pts.size(); ++i) idxs[0].emplace_back(i);
            return;
        }

        // last pair may has defferent value in head(0) and tail.
        // so rewrite these value
        if(!(recs[0] & SH_IN_MASK))
        {
            int v = recs[recs.size() - 1] & SH_VALUE_MASK;
            int i = 0;
            while((recs[i] & SH_VALUE_MASK) == 0)
            {
                if(recs[i] & SH_OUT_MASK)
                {
                    recs[i] = v | SH_OUT_MASK;
                    break;
                }
                else recs[i] = v;
                ++i;
            }
        }
        // determine [entry, exit] pair coverage here.
        // erase pairs which was covered.
        for(int i = 0; i < in.size(); )
        {
            if(in.size() < 2) break; // only 1 pair remain, complete

            int n = (i + 1) % in.size();
            range2d cur {fminf(in[i].e, out[i].e), fmaxf(in[i].e, out[i].e)};
            range2d next {fminf(in[n].e, out[n].e), fmaxf(in[n].e, out[n].e)};

            // determine coverage of current pair and next pair
            int state = range_coverage(cur, next);
            if(state == RCS_CROSS) return;
            if(state == RCS_A_COVER_B)
            {
                // erase next pair
                int start = in[n].s, end = out[n].s;
                end = end < start ? end + pts.size() : end;
                for(int s = start; s <= end; ++s)
                {
                    int rs = s % pts.size();
                    recs[rs] = recs[in[i].s] & SH_VALUE_MASK;
                }
                in.erase(in.begin() + n);
                out.erase(out.begin() + n);
            }
            else if(state == RCS_B_COVER_A)
            {
                // erase current pair
                int start = in[i].s, end = out[i].s;
                end = end < start ? end + pts.size() : end;
                for(int s = start; s <= end; ++s)
                {
                    int rs = s % pts.size();
                    recs[rs] = recs[in[n].s] & SH_VALUE_MASK;
                }
                in.erase(in.begin() + i);
                out.erase(out.begin() + i);
            }
                // independent, check next
            else ++i;
        }

        // check point 3: after coverage determine,
        //      if there is only 1 pair, just add them all
        //      or split them
        if(in.size() < 2)
        {
            idxs.emplace_back();
            for(int i = 0; i < pts.size(); ++i) idxs[0].emplace_back(i);
        }
        else
        {
            // first, find first pt of any pt seq.
            int start = -1;
            for(int i = 0; i < pts.size(); ++i)
            {
                int p = (i - 1 + pts.size()) % pts.size();
                if((recs[p] & SH_VALUE_MASK) != (recs[i] & SH_VALUE_MASK))
                {
                    start = i;
                    break;
                }
            }

            // then, from start, add all pt to result by its seq id
            int cur = -1;
            for(int i = 0; i < pts.size(); ++i)
            {
                int ri = (i + start) % pts.size();
                int rec = recs[ri] & SH_VALUE_MASK;
                if(rec != cur)
                {
                    cur = rec;
                    idxs.emplace_back();
                }
                idxs[idxs.size() - 1].emplace_back(ri);
            }
        }
    }
    int sutherland_hodgman(const polygon2d& src, const rect& bnds, vector<polygon2d>& polys)
    {
        vector<vector<vector2d>> srcs;
        srcs.emplace_back(src._pts);
        vector<vector<vector2d>> dsts;
        vector<vector2d> pts;
        vector<vector2d> tmp;
        vector<int> recs;
        vector<vector<int>> idxs;

        bool flag = false;
        int s = 0, e = 0;

        // clip with left edge
        flag = false;
        for(auto& src_pts : srcs)
        {
            int ins = 0;
            tmp.clear();
            pts.clear();
            recs.clear();
            idxs.clear();
            for(int i = 0; i < src_pts.size(); ++i)
            {
                auto& c = src_pts[i];
                auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
                s = c.x > bnds.left();
                e = n.x > bnds.left();

                int r = s + e;
                if(r == 0); // both outside
                else if(r == 2) // both inside
                {
                    pts.push_back(n);
                    recs.push_back(ins);
                }
                else if(s == 1) // goes out clip window
                {
                    f32 v;
                    line2d(c, n).resolv_y(bnds.left(), v);
                    pts.push_back({bnds.left(), v});
                    recs.push_back(ins | SH_OUT_MASK);
                    ++ins;
                }
                else if(e == 1) // goes in clip window
                {
                    f32 v;
                    line2d(c, n).resolv_y(bnds.left(), v);
                    pts.push_back({bnds.left(), v});
                    recs.push_back(ins | SH_IN_MASK);
                    pts.push_back(n);
                    recs.push_back(ins);
                }
            }
            resolv_independent_ee(pts, recs, AXIS_Y, idxs);
            split_polygon(pts, idxs, dsts);
        }

        // clip with right edge
        flag = false;
        srcs.clear();
        srcs.swap(dsts);
        for(auto& src_pts : srcs)
        {
            int ins = 0;
            tmp.clear();
            pts.clear();
            idxs.clear();
            recs.clear();
            for(int i = 0; i < src_pts.size(); ++i)
            {
                auto& c = src_pts[i];
                auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
                s = c.x < bnds.right();
                e = n.x < bnds.right();

                int r = s + e;
                if(r == 0); // both outside
                else if(r == 2) // both inside
                {
                    pts.push_back(n);
                    recs.push_back(ins);
                }
                else if(s == 1) // goes out clip window
                {
                    f32 v;
                    line2d(c, n).resolv_y(bnds.right(), v);
                    pts.push_back({bnds.right(), v});
                    recs.push_back(ins | SH_OUT_MASK);
                    ++ins;
                }
                else if(e == 1) // goes in clip window
                {
                    f32 v;
                    line2d(c, n).resolv_y(bnds.right(), v);
                    pts.push_back({bnds.right(), v});
                    recs.push_back(ins | SH_IN_MASK);
                    pts.push_back(n);
                    recs.push_back(ins);
                }
            }
            resolv_independent_ee(pts, recs, AXIS_Y, idxs);
            split_polygon(pts, idxs, dsts);
        }

        // clip with top edge
        flag = false;
        srcs.clear();
        srcs.swap(dsts);
        for(auto& src_pts : srcs)
        {
            int ins = 0;
            tmp.clear();
            pts.clear();
            recs.clear();
            idxs.clear();
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
                    recs.push_back(ins);
                }
                else if(s == 1) // goes out clip window
                {
                    f32 x;
                    line2d(c, n).resolv_x(bnds.top(), x);
                    pts.push_back({x, bnds.top()});
                    recs.push_back(ins | SH_OUT_MASK);
                    ++ins;
                }
                else if(e == 1) // goes in clip window
                {
                    f32 x;
                    line2d(c, n).resolv_x(bnds.top(), x);
                    pts.push_back({x, bnds.top()});
                    recs.push_back(ins | SH_IN_MASK);
                    pts.push_back(n);
                    recs.push_back(ins);
                }
            }
            resolv_independent_ee(pts, recs, AXIS_X, idxs);
            split_polygon(pts, idxs, dsts);
        }

        // clip with bottom edge
        flag = false;
        srcs.clear();
        srcs.swap(dsts);
        for(auto& src_pts : srcs)
        {
            int ins = 0;
            tmp.clear();
            pts.clear();
            recs.clear();
            idxs.clear();
            for(int i = 0; i < src_pts.size(); ++i)
            {
                auto& c = src_pts[i];
                auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
                s = c.y < bnds.bottom();
                e = n.y < bnds.bottom();

                int r = s + e;
                if(r == 0); // both outside
                else if(r == 2) // both inside
                {
                    pts.push_back(n);
                    recs.push_back(ins);
                }
                else if(s == 1) // goes out clip window
                {
                    f32 v;
                    line2d(c, n).resolv_x(bnds.bottom(), v);
                    pts.push_back({v, bnds.bottom()});
                    recs.push_back(ins | SH_OUT_MASK);
                    ++ins;
                }
                else if(e == 1) // goes in clip window
                {
                    f32 v;
                    line2d(c, n).resolv_x(bnds.bottom(), v);
                    pts.push_back({v, bnds.bottom()});
                    recs.push_back(ins | SH_IN_MASK);
                    pts.push_back(n);
                    recs.push_back(ins);
                }
            }
            resolv_independent_ee(pts, recs, AXIS_X, idxs);
            split_polygon(pts, idxs, dsts);
        }

        for(auto& dst_pts : dsts)
            polys.emplace_back(dst_pts);
        return algo::CR_ACCEPTED;
    }

	int sutherland_hodgman(const polygon2d& src, const polygon2d& bnds, vector<polygon2d>& polys)
	{
		// TODO to be finished
		return CR_REFUSED;
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

	void mark_points(vector<marked_point>& pts, int stat, const vector<point2d>& src)
	{
		int t = -1, w = -1;
		for(int i = 0; i < src.size(); ++i)
		{
			if(stat == WA_TAR) t = i;
			if(stat == WA_WND) w = i;
			pts.emplace_back(src[i], stat, t, w);
		}
	}

	int weiler_atherton(const polygon2d& target, const polygon2d& window, vector<polygon2d>& rslt)
	{
		vector<marked_point> tar;
		vector<marked_point> wnd;
		vector<marked_point> ints;
		// 1. mark all points.
		mark_points(tar, WA_TAR, target.points());
		mark_points(wnd, WA_WND, window.points());

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
				c = 1 - c;
				pt.t += c;
			}
			tar.insert(++it, pt);
			it = find_if(wnd.begin(), wnd.end(), [&](const marked_point& p){return p.w == pt.w;});
			{
				auto v = pt.pt - it->pt;
				f32 c = 1 / (v * v + 1);
				c = 1 - c;
				pt.w += c;
			}
			wnd.insert(++it, pt);
		}
		sort(tar.begin(), tar.end(), [](const marked_point& p1, const marked_point& p2){return p2.t > p1.t;});
		sort(wnd.begin(), wnd.end(), [](const marked_point& p1, const marked_point& p2){return p2.w > p1.w;});

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
			S = it;

			poly_in:
			if(it == tar.end()) // no more `in` pts, done
				break;

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
			it = find_if(wnd.begin(), wnd.end(), [&](const marked_point& pt){ return pt.pt == it->pt; });

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
					else
					{
						// need unmark `in` in wnd pt?
						it = find_if(tar.begin(), tar.end(), [&](const marked_point& pt){ return pt.pt == it->pt; });
						goto poly_in;
					}
				}
				else poly.push_back(it->pt);
			}
		}

		return rslt.size() == 0;
	}

}