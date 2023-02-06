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
#ifdef __unix__
#include <sys/time.h>
#endif
#ifdef _WIN32
#include <Windows.h>
#endif
using namespace std;
using namespace spes;
using namespace spes::color;
using namespace spes::image;
using namespace spes::math;
using namespace spes::image::io;
using namespace spes::canvas;

#undef GREEN
#undef RED
#undef BLACK
#undef BLUE
#undef YELLOW
#undef AQUA

#ifdef DEFAULT
#undef DEFAULT
#endif
#ifdef IN
#undef IN
#endif
#ifdef OUT
#undef OUT
#endif
constexpr const int DEFAULT = 0;
constexpr const int IN = 1;
constexpr const int OUT = 2;
constexpr const int AXIS_X = 0;
constexpr const int AXIS_Y = 1;

enum TEST_SH_MASKS {
    TEST_SH_OUT_MASK = 1 << 31,
    TEST_SH_IN_MASK = 1 << 30,
    TEST_SH_VALUE_MASK = ~(TEST_SH_IN_MASK | TEST_SH_OUT_MASK)
};

enum RANGE_STATE {
    RS_CROSS = 0,
    RS_INDEPENDENT,
    RS_A_COVER_B,
    RS_B_COVER_A
};

// 0 cross 1 independent 2 a cover b 3 b cover a
int range_state(range2d& a, range2d& b) {
    if (a.e < b.s || a.s > b.e) return RS_INDEPENDENT;

    if (in_range(a.s, b.s, b.e) && in_range(a.e, b.s, b.e)) return RS_B_COVER_A;
    if (in_range(b.s, a.s, a.e) && in_range(b.e, a.s, a.e)) return RS_A_COVER_B;
    return RS_CROSS;
}

void resolv_independent_ee(vector<point2d>& pts, vector<int>& dirs, int axis) {
    if (pts.size() != dirs.size()) return; // TODO exception or asssertion needed here.

    int first_idx = -1;
    for (int i = 0; i < pts.size(); ++i) {
        // just show info here
        if (dirs[i] == DEFAULT) cout << "-";
        if (dirs[i] == IN) cout << ">";
        if (dirs[i] == OUT) cout << "<";

        if (first_idx < 0 && dirs[i] == IN) {
            first_idx = i;
            // break;
        }
    }
    cout << endl;
    vector<range2d> in, out; // in or out pt idx and value
    for (int i = 0; i < pts.size(); ++i) {
        int ri = (i + first_idx) % pts.size();
        if (dirs[ri] == IN) in.push_back({ ri + .0f, axis ? pts[ri].y : pts[ri].x });
        if (dirs[ri] == OUT) out.push_back({ ri + .0f, axis ? pts[ri].y : pts[ri].x });
    }
    if (in.size() != out.size()) return;
    for (int i = 0; i < in.size(); ++i)
        cout << "[" << in[i].e << ", " << out[i].e << "] ";
    cout << endl;
    for (int i = 0; i < in.size();) {
        if (in.size() < 2) break;
        int     n = (i + 1) % in.size();
        range2d cur{ fminf(in[i].e, out[i].e), fmaxf(in[i].e, out[i].e) };
        range2d next{ fminf(in[n].e, out[n].e), fmaxf(in[n].e, out[n].e) };

        int state = range_state(cur, next);
        if (state == RS_CROSS) return; //
        if (state == RS_A_COVER_B) {
            // remove next
            in.erase(in.begin() + n);
            out.erase(out.begin() + n);
        } else if (state == RS_B_COVER_A) {
            // remove cur
            in.erase(in.begin() + i);
            out.erase(out.begin() + i);
        } else
            ++i;
    }
    for (int i = 0; i < in.size(); ++i)
        cout << "[" << in[i].e << ", " << out[i].e << "] ";
    cout << endl;
    for (int i = 0; i < dirs.size(); ++i)
        dirs[i] = 0;
    for (int i = 0; i < in.size(); ++i) {
        dirs[in[i].s] = IN;
        dirs[out[i].s] = OUT;
    }

    for (int i = 0; i < pts.size(); ++i) {
        // just show info here
        if (dirs[i] == DEFAULT) cout << "-";
        if (dirs[i] == IN) cout << ">";
        if (dirs[i] == OUT) cout << "<";
    }
}


void resolv_independent_ee2(vector<point2d>& pts, vector<int>& dirs, int axis, vector<vector<int>>& idxs) {
    if (pts.size() != dirs.size()) return; // TODO exception or asssertion needed here.


    int first_idx = -1;
    for (int i = 0; i < pts.size(); ++i) {
        // just show info here
        cout << dirs[i];
        int p = i - 1;
        if (p < 0) p = pts.size() - 1;

        if (first_idx < 0 && dirs[i] == 0 && dirs[p] != 0) {
            first_idx = i;
            // break;
        }
    }
    cout << endl;
    vector<range2d> in, out; // in or out pt idx and value
    for (int i = 0; i < pts.size(); ++i) {
        int ri = (i + first_idx) % pts.size();
        int p = ri - 1;
        p = p < 0 ? pts.size() - 1 : p;
        int n = (ri + 1) % pts.size();
        if (dirs[p] != dirs[ri]) in.push_back({ ri + .0f, axis ? pts[ri].y : pts[ri].x });
        if (dirs[n] != dirs[ri]) out.push_back({ ri + .0f, axis ? pts[ri].y : pts[ri].x });
    }
    if (in.size() != out.size()) return;

    if (in.size() < 2) {
        // just add all
        idxs.emplace_back();
        for (int i = 0; i < pts.size(); ++i) idxs[0].emplace_back(i);
        return;
    }

    for (int i = 0; i < in.size(); ++i)
        cout << "[" << in[i].e << ", " << out[i].e << "] ";
    cout << endl;
    for (int i = 0; i < in.size();) {
        if (in.size() < 2) break;
        int     n = (i + 1) % in.size();
        range2d cur{ fminf(in[i].e, out[i].e), fmaxf(in[i].e, out[i].e) };
        range2d next{ fminf(in[n].e, out[n].e), fmaxf(in[n].e, out[n].e) };

        int state = range_state(cur, next);
        if (state == RS_CROSS) return; //
        if (state == RS_A_COVER_B) {
            // remove next
            int start = in[n].s, end = out[n].s;
            end = end < start ? end + pts.size() : end;
            for (int s = start; s <= end; ++s) {
                int rs = s % pts.size();
                dirs[rs] = dirs[in[i].s];
            }
            in.erase(in.begin() + n);
            out.erase(out.begin() + n);
        } else if (state == RS_B_COVER_A) {
            // remove cur
            int start = in[i].s, end = out[i].s;
            end = end < start ? end + pts.size() : end;
            for (int s = start; s <= end; ++s) {
                int rs = s % pts.size();
                dirs[rs] = dirs[in[n].s];
            }
            in.erase(in.begin() + i);
            out.erase(out.begin() + i);
        } else
            ++i;
    }
    for (int i = 0; i < in.size(); ++i)
        cout << "[" << in[i].e << ", " << out[i].e << "] ";
    cout << endl;

    for (int i = 0; i < pts.size(); ++i)
        cout << dirs[i];
    cout << endl;

    if (in.size() < 2) {
        // just add all
        idxs.emplace_back();
        for (int i = 0; i < pts.size(); ++i) idxs[0].emplace_back(i);
    } else {
        int start = -1;
        for (int i = 0; i < pts.size(); ++i) {
            int p = (i - 1 + pts.size()) % pts.size();
            if (dirs[p] != dirs[i]) {
                start = i;
                break;
            }
        }
        int cur = -1;
        for (int i = 0; i < pts.size(); ++i) {
            int ri = (i + start) % pts.size();
            if (dirs[ri] != cur) {
                cur = dirs[ri];
                idxs.emplace_back();
            }
            idxs[idxs.size() - 1].emplace_back(ri);
        }
    }

    for (auto& idx : idxs) {
        cout << "\nnew poly[" << dirs[idx[0]] << "]: ";
        for (auto i : idx)
            cout << i << " ";
        cout << endl;
    }
}

void split_polygon(const vector<vector2d>& pts, const vector<vector<int>>& idxs, vector<polygon2d>& polys) {
    for (auto& idx : idxs) {
        vector<point2d> tmp;
        for (auto i : idx)
            tmp.push_back(pts[i]);
        polys.emplace_back(tmp);
    }
}

int sutherland_hodgman_custom(const polygon2d& src, const rect& bnds, vector<polygon2d>& polys) {
    vector<vector<vector2d>> srcs;
    srcs.emplace_back(src._pts);
    vector<vector<vector2d>> dsts;
    vector<vector2d>         pts;
    vector<vector2d>         tmp;

    vector<int> dirs;

    bool flag = false;
    int  s = 0, e = 0;

    // clip with top edge
    flag = false;
    int first_in = -1;
    for (auto& src_pts : srcs) {
        tmp.clear();
        pts.clear();
        dirs.clear();
        for (int i = 0; i < src_pts.size(); ++i) {
            auto& c = src_pts[i];
            auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
            s = c.y > bnds.top();
            e = n.y > bnds.top();

            int r = s + e;
            if (r == 0)
                ;            // both outside
            else if (r == 2) // both inside
            {
                pts.push_back(n);
                dirs.push_back(DEFAULT);
            } else if (s == 1) // goes out clip window
            {
                f32 x;
                line2d(c, n).resolv_x(bnds.top(), x);
                pts.push_back({ x, bnds.top() });
                dirs.push_back(OUT);
            } else if (e == 1) // goes in clip window
            {
                f32 x;
                line2d(c, n).resolv_x(bnds.top(), x);
                pts.push_back({ x, bnds.top() });
                if (first_in == -1) first_in = dirs.size();
                dirs.push_back(IN);
                pts.push_back(n);
                dirs.push_back(DEFAULT);
            }
        }
        // split poly by dirs
        resolv_independent_ee(pts, dirs, AXIS_X);
    }
    for (auto& dst_pts : dsts)
        polys.emplace_back(dst_pts);
    return algo::CR_ACCEPTED;
}
int sutherland_hodgman_custom2(const polygon2d& src, const rect& bnds, vector<polygon2d>& polys) {
    vector<vector<vector2d>> srcs;
    srcs.emplace_back(src._pts);
    vector<vector<vector2d>> dsts;
    vector<vector2d>         pts;
    vector<vector2d>         tmp;

    vector<int> dirs;

    bool flag = false;
    int  s = 0, e = 0;

    // clip with top edge
    flag = false;
    for (auto& src_pts : srcs) {
        int ins = 0;
        tmp.clear();
        pts.clear();
        dirs.clear();
        for (int i = 0; i < src_pts.size(); ++i) {
            auto& c = src_pts[i];
            auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
            s = c.y > bnds.top();
            e = n.y > bnds.top();

            int r = s + e;
            if (r == 0)
                ;            // both outside
            else if (r == 2) // both inside
            {
                pts.push_back(n);
                dirs.push_back(ins);
            } else if (s == 1) // goes out clip window
            {
                f32 x;
                line2d(c, n).resolv_x(bnds.top(), x);
                pts.push_back({ x, bnds.top() });
                dirs.push_back(ins);
                ins++;
            } else if (e == 1) // goes in clip window
            {
                f32 x;
                line2d(c, n).resolv_x(bnds.top(), x);
                pts.push_back({ x, bnds.top() });
                dirs.push_back(ins);
                pts.push_back(n);
                dirs.push_back(ins);
            }
        }
        // split poly by dirs
        vector<vector<int>> idxs;
        resolv_independent_ee2(pts, dirs, AXIS_X, idxs);
        split_polygon(pts, idxs, polys);
    }
    for (auto& dst_pts : dsts)
        polys.emplace_back(dst_pts);
    return algo::CR_ACCEPTED;
}

void split_polygon_final(const vector<vector2d>& pts, const vector<vector<int>>& idxs, vector<vector<vector2d>>& splited) {
    for (auto& idx : idxs) {
        vector<point2d> tmp;
        for (auto i : idx)
            tmp.push_back(pts[i]);
        splited.emplace_back(tmp);
    }
}
void resolv_independent_ee_final(vector<point2d>& pts, vector<int>& recs, int axis, vector<vector<int>>& idxs) {
    if (pts.size() != recs.size()) return; // TODO exception or asssertion needed here.


    int first_idx = -1;
    for (int i = 0; i < pts.size(); ++i) {
        int p = i - 1;
        if (p < 0) p = pts.size() - 1;

        if (first_idx < 0 && recs[i] == 0 && recs[p] != 0) {
            first_idx = i;
            break;
        }
    }
    vector<range2d> in, out; // in or out pt idx and value
    for (int i = 0; i < pts.size(); ++i) {
        int ri = (i + first_idx) % pts.size();
        int p = ri - 1;
        p = p < 0 ? pts.size() - 1 : p;
        int n = (ri + 1) % pts.size();
        if (recs[p] != recs[ri]) in.push_back({ ri + .0f, axis ? pts[ri].y : pts[ri].x });
        if (recs[n] != recs[ri]) out.push_back({ ri + .0f, axis ? pts[ri].y : pts[ri].x });
    }
    if (in.size() != out.size()) return;

    if (in.size() < 2) {
        // just add all
        idxs.emplace_back();
        for (int i = 0; i < pts.size(); ++i) idxs[0].emplace_back(i);
        return;
    }

    for (int i = 0; i < in.size();) {
        if (in.size() < 2) break;
        int     n = (i + 1) % in.size();
        range2d cur{ fminf(in[i].e, out[i].e), fmaxf(in[i].e, out[i].e) };
        range2d next{ fminf(in[n].e, out[n].e), fmaxf(in[n].e, out[n].e) };

        int state = range_state(cur, next);
        if (state == RS_CROSS) return; //
        if (state == RS_A_COVER_B) {
            // remove next
            int start = in[n].s, end = out[n].s;
            end = end < start ? end + pts.size() : end;
            for (int s = start; s <= end; ++s) {
                int rs = s % pts.size();
                recs[rs] = recs[in[i].s];
            }
            in.erase(in.begin() + n);
            out.erase(out.begin() + n);
        } else if (state == RS_B_COVER_A) {
            // remove cur
            int start = in[i].s, end = out[i].s;
            end = end < start ? end + pts.size() : end;
            for (int s = start; s <= end; ++s) {
                int rs = s % pts.size();
                recs[rs] = recs[in[n].s];
            }
            in.erase(in.begin() + i);
            out.erase(out.begin() + i);
        } else
            ++i;
    }

    if (in.size() < 2) {
        // just add all
        idxs.emplace_back();
        for (int i = 0; i < pts.size(); ++i) idxs[0].emplace_back(i);
    } else {
        int start = -1;
        for (int i = 0; i < pts.size(); ++i) {
            int p = (i - 1 + pts.size()) % pts.size();
            if (recs[p] != recs[i]) {
                start = i;
                break;
            }
        }
        int cur = -1;
        for (int i = 0; i < pts.size(); ++i) {
            int ri = (i + start) % pts.size();
            if (recs[ri] != cur) {
                cur = recs[ri];
                idxs.emplace_back();
            }
            idxs[idxs.size() - 1].emplace_back(ri);
        }
    }
}
int sutherland_hodgman_final(const polygon2d& src, const rect& bnds, vector<polygon2d>& polys) {
    vector<vector<vector2d>> srcs;
    srcs.emplace_back(src._pts);
    vector<vector<vector2d>> dsts;
    vector<vector2d>         pts;
    vector<vector2d>         tmp;
    vector<int>              recs;
    vector<vector<int>>      idxs;

    bool flag = false;
    int  s = 0, e = 0;

    // clip with left edge
    flag = false;
    for (auto& src_pts : srcs) {
        int ins = 0;
        tmp.clear();
        pts.clear();
        recs.clear();
        for (int i = 0; i < src_pts.size(); ++i) {
            auto& c = src_pts[i];
            auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
            s = c.x > bnds.left();
            e = n.x > bnds.left();

            int r = s + e;
            if (r == 0)
                ;            // both outside
            else if (r == 2) // both inside
            {
                pts.push_back(n);
                recs.push_back(ins);
            } else if (s == 1) // goes out clip window
            {
                f32 v;
                line2d(c, n).resolv_y(bnds.left(), v);
                pts.push_back({ bnds.left(), v });
                recs.push_back(ins);
                ins++;
            } else if (e == 1) // goes in clip window
            {
                f32 v;
                line2d(c, n).resolv_y(bnds.left(), v);
                pts.push_back({ bnds.left(), v });
                recs.push_back(ins);
                pts.push_back(n);
                recs.push_back(ins);
            }
        }
        resolv_independent_ee_final(pts, recs, AXIS_X, idxs);
        split_polygon_final(pts, idxs, dsts);
    }

    // clip with right edge
    flag = false;
    idxs.clear();
    recs.clear();
    srcs.clear();
    srcs.swap(dsts);
    for (auto& src_pts : srcs) {
        int ins = 0;
        tmp.clear();
        pts.clear();
        recs.clear();
        for (int i = 0; i < src_pts.size(); ++i) {
            auto& c = src_pts[i];
            auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
            s = c.x < bnds.right();
            e = n.x < bnds.right();

            int r = s + e;
            if (r == 0)
                ;            // both outside
            else if (r == 2) // both inside
            {
                pts.push_back(n);
                recs.push_back(ins);
            } else if (s == 1) // goes out clip window
            {
                f32 v;
                line2d(c, n).resolv_y(bnds.right(), v);
                pts.push_back({ bnds.right(), v });
                recs.push_back(ins);
                ins++;
            } else if (e == 1) // goes in clip window
            {
                f32 v;
                line2d(c, n).resolv_y(bnds.right(), v);
                pts.push_back({ bnds.right(), v });
                recs.push_back(ins);
                pts.push_back(n);
                recs.push_back(ins);
            }
        }
        vector<vector<int>> idxs;
        resolv_independent_ee_final(pts, recs, AXIS_X, idxs);
        split_polygon_final(pts, idxs, dsts);
    }

    // clip with top edge
    flag = false;
    idxs.clear();
    recs.clear();
    srcs.clear();
    srcs.swap(dsts);
    for (auto& src_pts : srcs) {
        int ins = 0;
        tmp.clear();
        pts.clear();
        recs.clear();
        for (int i = 0; i < src_pts.size(); ++i) {
            auto& c = src_pts[i];
            auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
            s = c.y > bnds.top();
            e = n.y > bnds.top();

            int r = s + e;
            if (r == 0)
                ;            // both outside
            else if (r == 2) // both inside
            {
                pts.push_back(n);
                recs.push_back(ins);
            } else if (s == 1) // goes out clip window
            {
                f32 x;
                line2d(c, n).resolv_x(bnds.top(), x);
                pts.push_back({ x, bnds.top() });
                recs.push_back(ins);
                ins++;
            } else if (e == 1) // goes in clip window
            {
                f32 x;
                line2d(c, n).resolv_x(bnds.top(), x);
                pts.push_back({ x, bnds.top() });
                recs.push_back(ins);
                pts.push_back(n);
                recs.push_back(ins);
            }
        }
        vector<vector<int>> idxs;
        resolv_independent_ee_final(pts, recs, AXIS_X, idxs);
        split_polygon_final(pts, idxs, dsts);
    }

    // clip with bottom edge
    flag = false;
    idxs.clear();
    recs.clear();
    srcs.clear();
    srcs.swap(dsts);
    for (auto& src_pts : srcs) {
        int ins = 0;
        tmp.clear();
        pts.clear();
        recs.clear();
        for (int i = 0; i < src_pts.size(); ++i) {
            auto& c = src_pts[i];
            auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
            s = c.y < bnds.bottom();
            e = n.y < bnds.bottom();

            int r = s + e;
            if (r == 0)
                ;            // both outside
            else if (r == 2) // both inside
            {
                pts.push_back(n);
                recs.push_back(ins);
            } else if (s == 1) // goes out clip window
            {
                f32 v;
                line2d(c, n).resolv_x(bnds.bottom(), v);
                pts.push_back({ v, bnds.bottom() });
                recs.push_back(ins);
                ins++;
            } else if (e == 1) // goes in clip window
            {
                f32 v;
                line2d(c, n).resolv_x(bnds.bottom(), v);
                pts.push_back({ v, bnds.bottom() });
                recs.push_back(ins);
                pts.push_back(n);
                recs.push_back(ins);
            }
        }
        vector<vector<int>> idxs;
        resolv_independent_ee_final(pts, recs, AXIS_X, idxs);
        split_polygon_final(pts, idxs, dsts);
    }

    for (auto& dst_pts : dsts)
        polys.emplace_back(dst_pts);
    return algo::CR_ACCEPTED;
}


void split_polygon(const vector<vector2d>& pts, const vector<vector<int>>& idxs, vector<vector<vector2d>>& splited) {
    for (auto& idx : idxs) {
        vector<point2d> tmp;
        for (auto i : idx)
            tmp.emplace_back(pts[i]);
        splited.emplace_back(tmp);
    }
}
void resolv_independent_ee_bak(vector<point2d>& pts, vector<int>& recs, int axis, vector<vector<int>>& idxs) {
    if (pts.size() != recs.size()) return; // TODO exception or asssertion needed here.

    // find an entry pt
    int first_idx = -1;
    for (int i = 0; i < pts.size(); ++i) {
        int p = (i - 1 + pts.size()) % pts.size();

        if (recs[i] == 0 && recs[p] != 0) {
            first_idx = i;
            break;
        }
    }

    // check point 1: no entry and exists, so all pts IN clip wnd, just add them
    if (first_idx == -1) {
        idxs.emplace_back();
        for (int i = 0; i < pts.size(); ++i) idxs[0].emplace_back(i);
        return;
    }

    vector<range2d> in, out; // store in or out pt's {idx, value}
    // mark in and out pts.
    for (int i = 0; i < recs.size() - 1; ++i) {
        int n = i + 1;
        if (recs[n] == recs[i] + 2) {
            in.push_back({ n + .0f, axis ? pts[n].y : pts[n].x });
            out.push_back({ i + .0f, axis ? pts[i].y : pts[i].x });
        }
    }
    //    for(int i = 0; i < pts.size(); ++i)
    //    {
    //        int ri = (i + first_idx) % pts.size();
    //        int p = (i - 1 + pts.size()) % pts.size();
    //        int n = (ri + 1) % pts.size();
    //        if(recs[p] != recs[ri]) in.push_back({ri + .0f, axis ? pts[ri].y : pts[ri].x});
    //        if(recs[n] != recs[ri]) out.push_back({ri + .0f, axis ? pts[ri].y : pts[ri].x});
    //    }

    if (in.size() != out.size()) return;

    // check point 2: after recog entries and exits,
    //  if only 1 [entry, exit] pair exits, which also means all pt IN clip wnd,
    //  just add all pt
    if (in.size() < 2) {
        idxs.emplace_back();
        for (int i = 0; i < pts.size(); ++i) idxs[0].emplace_back(i);
        return;
    }

    // determine [entry, exit] pair coverage here.
    // erase pairs which was covered.
    for (int i = 0; i < in.size();) {
        if (in.size() < 2) break; // only 1 pair remain, complete

        int     n = (i + 1) % in.size();
        range2d cur{ fminf(in[i].e, out[i].e), fmaxf(in[i].e, out[i].e) };
        range2d next{ fminf(in[n].e, out[n].e), fmaxf(in[n].e, out[n].e) };

        // determine coverage of current pair and next pair
        int state = range_state(cur, next);
        if (state == RS_CROSS) return;
        if (state == RS_A_COVER_B) {
            // erase next pair
            int start = in[n].s, end = out[n].s;
            end = end < start ? end + pts.size() : end;
            for (int s = start; s <= end; ++s) {
                int rs = s % pts.size();
                recs[rs] = recs[in[i].s];
            }
            in.erase(in.begin() + n);
            out.erase(out.begin() + n);
        } else if (state == RS_B_COVER_A) {
            // erase current pair
            int start = in[i].s, end = out[i].s;
            end = end < start ? end + pts.size() : end;
            for (int s = start; s <= end; ++s) {
                int rs = s % pts.size();
                recs[rs] = recs[in[n].s];
            }
            in.erase(in.begin() + i);
            out.erase(out.begin() + i);
        }
        // independent, check next
        else
            ++i;
    }

    // check point 3: after coverage determine,
    //      if there is only 1 pair, just add them all
    //      or split them
    if (in.size() < 2) {
        idxs.emplace_back();
        for (int i = 0; i < pts.size(); ++i) idxs[0].emplace_back(i);
    } else {
        // first, find first pt of any pt seq.
        int start = -1;
        for (int i = 0; i < pts.size(); ++i) {
            int p = (i - 1 + pts.size()) % pts.size();
            if (recs[p] != recs[i]) {
                start = i;
                break;
            }
        }

        // then, from start, add all pt to result by its seq id
        int cur = -1;
        for (int i = 0; i < pts.size(); ++i) {
            int ri = (i + start) % pts.size();
            if (recs[ri] != cur) {
                cur = recs[ri];
                idxs.emplace_back();
            }
            idxs[idxs.size() - 1].emplace_back(ri);
        }
    }
}

void resolv_independent_ee(vector<point2d>& pts, vector<int>& recs, int axis, vector<vector<int>>& idxs) {
    if (pts.size() != recs.size()) return; // TODO exception or asssertion needed here.

    // find an entry pt
    int first_idx = -1;
    for (int i = 0; i < pts.size(); ++i) {
        if (recs[i] & TEST_SH_IN_MASK) {
            first_idx = i;
            break;
        }
    }

    // check point 1: no entry and exists, so all pts IN clip wnd, just add them
    if (first_idx == -1) {
        idxs.emplace_back();
        for (int i = 0; i < pts.size(); ++i) idxs[0].emplace_back(i);
        return;
    }

    vector<range2d> in, out; // store in or out pt's {idx, value}
    // mark in and out pts.
    for (int i = 0; i < pts.size(); ++i) {
        int ri = (i + first_idx) % pts.size();
        if (recs[ri] & TEST_SH_IN_MASK) in.push_back({ ri + .0f, axis ? pts[ri].y : pts[ri].x });
        if (recs[ri] & TEST_SH_OUT_MASK) out.push_back({ ri + .0f, axis ? pts[ri].y : pts[ri].x });
    }

    if (in.size() != out.size()) return;

    // check point 2: after recog entries and exits,
    //  if only 1 [entry, exit] pair exits, which also means all pt IN clip wnd,
    //  just add all pt
    if (in.size() < 2) {
        idxs.emplace_back();
        for (int i = 0; i < pts.size(); ++i) idxs[0].emplace_back(i);
        return;
    }

    // last pair may has defferent value in head(0) and tail.
    // so rewrite these value
    if (!(recs[0] & TEST_SH_IN_MASK)) {
        int v = recs[recs.size() - 1] & TEST_SH_VALUE_MASK;
        int i = 0;
        while ((recs[i] & TEST_SH_VALUE_MASK) == 0) {
            if (recs[i] & TEST_SH_OUT_MASK) {
                recs[i] = v | TEST_SH_OUT_MASK;
                break;
            } else
                recs[i] = v;
            ++i;
        }
    }

    // determine [entry, exit] pair coverage here.
    // erase pairs which was covered.
    for (int i = 0; i < in.size();) {
        if (in.size() < 2) break; // only 1 pair remain, complete

        int     n = (i + 1) % in.size();
        range2d cur{ fminf(in[i].e, out[i].e), fmaxf(in[i].e, out[i].e) };
        range2d next{ fminf(in[n].e, out[n].e), fmaxf(in[n].e, out[n].e) };

        // determine coverage of current pair and next pair
        int state = range_state(cur, next);
        if (state == RS_CROSS) return;
        if (state == RS_A_COVER_B) {
            // erase next pair
            int start = in[n].s, end = out[n].s;
            end = end < start ? end + pts.size() : end;
            int v = recs[in[i].s] & TEST_SH_VALUE_MASK;
            for (int s = start; s <= end; ++s) {
                int rs = s % pts.size();
                recs[rs] = v;
            }
            in.erase(in.begin() + n);
            out.erase(out.begin() + n);
        } else if (state == RS_B_COVER_A) {
            // erase current pair
            int start = in[i].s, end = out[i].s;
            end = end < start ? end + pts.size() : end;
            int v = recs[in[n].s] & TEST_SH_VALUE_MASK;
            for (int s = start; s <= end; ++s) {
                int rs = s % pts.size();
                recs[rs] = v;
            }
            in.erase(in.begin() + i);
            out.erase(out.begin() + i);
        }
        // independent, check next
        else
            ++i;
    }

    // check point 3: after coverage determine,
    //      if there is only 1 pair, just add them all
    //      or split them
    if (in.size() < 2) {
        idxs.emplace_back();
        for (int i = 0; i < pts.size(); ++i) idxs[0].emplace_back(i);
    } else {
        // first, find first pt of any pt seq.
        int start = -1;
        for (int i = 0; i < pts.size(); ++i) {
            int p = (i - 1 + pts.size()) % pts.size();
            if ((recs[p] & TEST_SH_VALUE_MASK) != (recs[i] & TEST_SH_VALUE_MASK)) {
                start = i;
                break;
            }
        }

        // then, from start, add all pt to result by its seq id
        int cur = -1;
        for (int i = 0; i < pts.size(); ++i) {
            int ri = (i + start) % pts.size();
            int rec = recs[ri] & TEST_SH_VALUE_MASK;
            if (rec != cur) {
                cur = rec;
                idxs.emplace_back();
            }
            idxs[idxs.size() - 1].emplace_back(ri);
        }
    }
}
int sutherland_hodgman(const polygon2d& src, const rect& bnds, vector<polygon2d>& polys) {
    vector<vector<vector2d>> srcs;
    srcs.emplace_back(src._pts);
    vector<vector<vector2d>> dsts;
    vector<vector2d>         pts;
    vector<vector2d>         tmp;
    vector<int>              recs;
    vector<vector<int>>      idxs;

    bool flag = false;
    int  s = 0, e = 0;

    // clip with left edge
    flag = false;
    for (auto& src_pts : srcs) {
        int ins = 0;
        tmp.clear();
        pts.clear();
        recs.clear();
        idxs.clear();
        for (int i = 0; i < src_pts.size(); ++i) {
            auto& c = src_pts[i];
            auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
            s = c.x > bnds.left();
            e = n.x > bnds.left();

            int r = s + e;
            if (r == 0)
                ;            // both outside
            else if (r == 2) // both inside
            {
                pts.push_back(n);
                recs.push_back(ins);
            } else if (s == 1) // goes out clip window
            {
                f32 v;
                line2d(c, n).resolv_y(bnds.left(), v);
                pts.push_back({ bnds.left(), v });
                recs.push_back(ins | TEST_SH_OUT_MASK);
                ++ins;
            } else if (e == 1) // goes in clip window
            {
                f32 v;
                line2d(c, n).resolv_y(bnds.left(), v);
                pts.push_back({ bnds.left(), v });
                recs.push_back(ins | TEST_SH_IN_MASK);
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
    for (auto& src_pts : srcs) {
        int ins = 0;
        tmp.clear();
        pts.clear();
        idxs.clear();
        recs.clear();
        for (int i = 0; i < src_pts.size(); ++i) {
            auto& c = src_pts[i];
            auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
            s = c.x < bnds.right();
            e = n.x < bnds.right();

            int r = s + e;
            if (r == 0)
                ;            // both outside
            else if (r == 2) // both inside
            {
                pts.push_back(n);
                recs.push_back(ins);
            } else if (s == 1) // goes out clip window
            {
                f32 v;
                line2d(c, n).resolv_y(bnds.right(), v);
                pts.push_back({ bnds.right(), v });
                recs.push_back(ins | TEST_SH_OUT_MASK);
                ++ins;
            } else if (e == 1) // goes in clip window
            {
                f32 v;
                line2d(c, n).resolv_y(bnds.right(), v);
                pts.push_back({ bnds.right(), v });
                recs.push_back(ins | TEST_SH_IN_MASK);
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
    for (auto& src_pts : srcs) {
        int ins = 0;
        tmp.clear();
        pts.clear();
        recs.clear();
        idxs.clear();
        for (int i = 0; i < src_pts.size(); ++i) {
            auto& c = src_pts[i];
            auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
            s = c.y > bnds.top();
            e = n.y > bnds.top();

            int r = s + e;
            if (r == 0)
                ;            // both outside
            else if (r == 2) // both inside
            {
                pts.push_back(n);
                recs.push_back(ins);
            } else if (s == 1) // goes out clip window
            {
                f32 x;
                line2d(c, n).resolv_x(bnds.top(), x);
                pts.push_back({ x, bnds.top() });
                recs.push_back(ins | TEST_SH_OUT_MASK);
                ++ins;
            } else if (e == 1) // goes in clip window
            {
                f32 x;
                line2d(c, n).resolv_x(bnds.top(), x);
                pts.push_back({ x, bnds.top() });
                recs.push_back(ins | TEST_SH_IN_MASK);
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
    for (auto& src_pts : srcs) {
        int ins = 0;
        tmp.clear();
        pts.clear();
        recs.clear();
        idxs.clear();
        for (int i = 0; i < src_pts.size(); ++i) {
            auto& c = src_pts[i];
            auto& n = (i == (src_pts.size() - 1)) ? src_pts[0] : src_pts[i + 1];
            s = c.y < bnds.bottom();
            e = n.y < bnds.bottom();

            int r = s + e;
            if (r == 0)
                ;            // both outside
            else if (r == 2) // both inside
            {
                pts.push_back(n);
                recs.push_back(ins);
            } else if (s == 1) // goes out clip window
            {
                f32 v;
                line2d(c, n).resolv_x(bnds.bottom(), v);
                pts.push_back({ v, bnds.bottom() });
                recs.push_back(ins | TEST_SH_OUT_MASK);
                ++ins;
            } else if (e == 1) // goes in clip window
            {
                f32 v;
                line2d(c, n).resolv_x(bnds.bottom(), v);
                pts.push_back({ v, bnds.bottom() });
                recs.push_back(ins | TEST_SH_IN_MASK);
                pts.push_back(n);
                recs.push_back(ins);
            }
        }
        resolv_independent_ee(pts, recs, AXIS_X, idxs);
        split_polygon(pts, idxs, dsts);
    }

    for (auto& dst_pts : dsts)
        polys.emplace_back(dst_pts);
    return algo::CR_ACCEPTED;
}

void test_draw() {
    const u32 SIZE = 700;
    Canvas    canvas(SIZE, SIZE);
    Canvas    canvas2(SIZE, SIZE);
    Canvas    canvas3(SIZE, SIZE);
    Canvas    canvas4(SIZE, SIZE);
    Canvas    canvas5(SIZE, SIZE);

    //    {
    //        rect rc(50, 50, 650, 650);
    //        vector<point2d> bnds = {{50, 50}, {50, 650}, {650, 650}, {650, 50}};
    //        vector<point2d> pts = {{200, 200}, {200, 400}, {400, 400}, {400, 200}};
    //        polygon2d src(pts);
    //        vector<polygon2d> out;
    //        sutherland_hodgman_custom(src, rc, out);
    //
    //        canvas.draw(CRM_LineRing, bnds, Colors::AQUA);
    //        canvas.draw(CRM_LineRing, pts);
    //        for(auto& poly : out)
    //            canvas.draw(CRM_LineRing, poly._pts, Colors::GREEN);
    //    }
    //
    //    {
    //        rect rc(50, 400, 650, 650);
    //        vector<point2d> bnds = {{50, 400}, {50, 650}, {650, 650}, {650, 400}};
    //        //vector<point2d> pts = {{100, 100}, {100, 600}, {200, 600}, {200, 300}, {300, 300}, {300, 600}, {400, 600}, {400, 100}};
    //        vector<point2d> pts = {{100, 100}, {100, 600}, {600, 600}, {600, 100}};
    //        polygon2d src(pts);
    //        vector<polygon2d> out;
    //        sutherland_hodgman_custom(src, rc, out);
    //
    //        canvas2.draw(CRM_LineRing, bnds, Colors::AQUA);
    //        canvas2.draw(CRM_LineRing, pts);
    //        for(auto& poly : out)
    //            canvas2.draw(CRM_LineRing, poly._pts, Colors::GREEN);
    //    }
    //
    //    {
    //        rect rc(50, 400, 650, 650);
    //        vector<point2d> bnds = {{50, 400}, {50, 650}, {650, 650}, {650, 400}};
    //        vector<point2d> pts = {{100, 100}, {100, 600}, {200, 600}, {200, 500}, {300, 500}, {300, 600}, {400, 600}, {400, 100}};
    //        polygon2d src(pts);
    //        vector<polygon2d> out;
    //        sutherland_hodgman_custom(src, rc, out);
    //
    //        canvas3.draw(CRM_LineRing, bnds, Colors::AQUA);
    //        canvas3.draw(CRM_LineRing, pts);
    //        for(auto& poly : out)
    //            canvas3.draw(CRM_LineRing, poly._pts, Colors::GREEN);
    //    }
    //    {
    //        rect rc(50, 400, 650, 650);
    //        vector<point2d> bnds = {{50, 400}, {50, 650}, {650, 650}, {650, 400}};
    //        vector<point2d> pts = {{100, 100}, {100, 600}, {350, 300}, {600, 600}, {600, 100}};
    //        polygon2d src(pts);
    //        vector<polygon2d> out;
    //        sutherland_hodgman_custom(src, rc, out);
    //
    //        canvas4.draw(CRM_LineRing, bnds, Colors::AQUA);
    //        canvas4.draw(CRM_LineRing, pts);
    //        for(auto& poly : out)
    //        {
    //            canvas4.draw(CRM_LineRing, poly._pts, Colors::GREEN);
    //            cout << "splited: ";
    //            for (auto &v : poly._pts)
    //                cout << v << " ";
    //            cout << endl;
    //        }
    //    }
    {
        rect            rc(50, 400, 650, 650);
        vector<point2d> bnds = { { 50, 400 }, { 50, 650 }, { 650, 650 }, { 650, 400 } };
        // pts.assign({{10, -10}, {20, 10}, {30, -20}, {40,  10}, {50, -10}, {40, 20}, {30, -10}, {20, 20}});
        vector<point2d>   pts = { { 100, 100 }, { 200, 500 }, { 300, 100 }, { 400, 500 }, { 500, 100 }, { 400, 600 }, { 300, 300 }, { 200, 600 } };
        polygon2d         src(pts);
        vector<polygon2d> out;
        sutherland_hodgman(src, rc, out);

        canvas5.draw(CRM_LineRing, bnds, Colors::RED);
        canvas5.draw(CRM_LineRing, pts, Colors::GREEN);
        for (auto& poly : out) {
            canvas5.draw(CRM_LineRing, poly._pts);
            cout << "splited: ";
            for (auto& v : poly._pts)
                cout << v << " ";
            cout << endl;
        }
    }
    //    image_io::show_image(canvas.image(), "c1");
    //    image_io::show_image(canvas2.image(), "c2");
    //    image_io::show_image(canvas3.image(), "c3");
    //    image_io::show_image(canvas4.image(), "c4");
    image_io::show_image(canvas5.image(), "c5");
    getch();
}
void test_clip() {
    rect              rc(0, 0, 100, 100);
    vector<point2d>   pts = { { 10, -10 }, { 50, 10 }, { 90, -10 }, { 90, 90 }, { 10, 90 } };
    polygon2d         src(pts);
    vector<polygon2d> out;
    sutherland_hodgman(src, rc, out); // expect 1 : 7
    cout << "out " << out.size() << " polys: ";
    for (auto& p : out) cout << p._num << " ";
    cout << endl;


    cout << "==========================================================" << endl;
    out.clear();
    pts.assign({ { 10, -10 }, { 20, 10 }, { 30, -20 }, { 40, 10 }, { 50, -10 }, { 40, 20 }, { 30, -10 }, { 20, 20 } }); // 'M'
    src.init(pts);
    sutherland_hodgman(src, rc, out); // expect 2 : 6 6
    cout << "out " << out.size() << " polys: ";
    for (auto& p : out) cout << p._num << " ";
    cout << endl;

    cout << "==========================================================" << endl;
    out.clear();
    pts.assign({ { 10, 120 }, { 20, 80 }, { 30, 110 }, { 40, 80 }, { 50, 120 }, { 40, 90 }, { 30, 120 }, { 20, 90 } }); // inv 'M'
    src.init(pts);
    sutherland_hodgman(src, rc, out); // expect 2 : 6 6
    cout << "out " << out.size() << " polys: ";
    for (auto& p : out) cout << p._num << " ";
    cout << endl;


    cout << "==========================================================" << endl;
    out.clear();
    pts.assign({ { 10, -10 }, { 10, 200 }, { 20, 200 }, { 20, -10 } });
    src.init(pts);
    sutherland_hodgman(src, rc, out); // expect 1 : 4
    cout << "out " << out.size() << " polys: ";
    for (auto& p : out) cout << p._num << " ";
    cout << endl;


    cout << "==========================================================" << endl;
    out.clear();
    pts.assign({ { 50, -20 }, { -20, 50 }, { 50, 120 }, { 120, 50 } });
    src.init(pts);
    sutherland_hodgman(src, rc, out); // expect 1 : 8
    cout << "out " << out.size() << " polys: ";
    for (auto& p : out) cout << p._num << " ";
    cout << endl;
}
long cur_ms() {
#ifdef __unix__
    timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
#ifdef _WIN32
    return GetTickCount();
#endif
}
void test_perform() {
    rect      rc(0, 0, 100, 100);
    polygon2d src({ { 10, -10 }, { 20, 10 }, { 30, -20 }, { 40, 10 }, { 50, -10 }, { 40, 20 }, { 30, -10 }, { 20, 20 } });

    const int CNTS = 100000;
    cout << "test " << CNTS << " times." << endl;

    long start = cur_ms();
    for (int i = 0; i < CNTS; ++i) {
        vector<polygon2d> out;
        sutherland_hodgman(src, rc, out);
    }
    long end = cur_ms();
    cout << "cost " << (end - start) << "ms." << endl;


    start = cur_ms();
    for (int i = 0; i < CNTS; ++i) {
        vector<polygon2d> out;
        algo::sutherland_hodgman(src, rc, out);
    }
    end = cur_ms();
    cout << "opted cost " << (end - start) << "ms." << endl;
}
void test_mask() {
    printf("%x %x %x\n", TEST_SH_IN_MASK, TEST_SH_OUT_MASK, TEST_SH_VALUE_MASK);
}

int main(int argc, char** argv) {
    // test_draw();
    // test_clip();
    test_perform();
    // test_mask();
    return 0;
}