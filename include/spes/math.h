#pragma once
#include <vector>
#include <cmath>
#include <memory>
#include <ostream>
#include "type.h"
using namespace spes;

namespace spes::math
{
	inline f32 fmax(f32 a, f32 b)
	{
		return a > b ? a : b;
	}
	inline f32 fmin(f32 a, f32 b)
	{
		return a < b ? a : b;
	}

	inline bool in_range(f32 v, f32 s, f32 e, bool contains_border = true)
	{
		return (v > s&& v < e) || (contains_border && (v == s || v == e));
	}

	template<typename T>
	double avg(const std::vector<T>& nums)
	{
		if (nums.size() <= 0) return .0;
		T sum = nums[0];
		for (int i = 1; i < nums.size(); ++i)
			sum += nums[i];
		return (sum + .0) / nums.size();
	}

	template<typename T>
	double variance(const std::vector<T>& nums)
	{
		if (nums.size() <= 1) return .0;
		return variance(nums, avg(nums));
	}

	template<typename T>
	double variance(const std::vector<T>& nums, double avg)
	{
		if (nums.size() <= 1) return .0;
		double sum = .0;

		for (T i : nums)
		{
			double d = i - avg;
			sum += d * d;
		}
		return sum / nums.size();
	}

	template<typename T>
	double standard_deviation(const std::vector<T>& nums)
	{
		return sqrt(variance(nums));
	}
	template<typename T>
	double standard_deviation(const std::vector<T>& nums, double avg)
	{
		return sqrt(variance(nums, avg));
	}

	 typedef union vector2d
	{
		struct
		{
			f32 x, y;
		};
		struct
		{
			f32 w, h;
		};
		struct
		{
			f32 s, e;
		};
	} size2d, point2d, range2d;

	vector2d normalize(const vector2d& v);
	 vector2d operator+(const vector2d& v1, const vector2d& v2);
	 vector2d operator-(const vector2d& v);
	 vector2d operator-(const vector2d& v1, const vector2d& v2);
	 f32 operator*(const vector2d& v1, const vector2d& v2);
	 vector2d operator*(const vector2d& v, f32 s);
	 bool operator==(const vector2d& v1, const vector2d& v2);
	 f32 cross(vector2d& v1, vector2d& v2);
	 std::ostream& operator<<(std::ostream&, const vector2d&);

    enum RANGE_COVERAGE_STATE
    {
        RCS_CROSS = 0,
        RCS_INDEPENDENT,
        RCS_A_COVER_B,
        RCS_B_COVER_A
    };
    int range_coverage(range2d& a, range2d& b);
	/*
	scale rect, makes it can be included in border
	*/
	size2d rect_adjust(size2d border, size2d rect);

	class line2d
	{
	private:
		void gen_k();
	public:
		vector2d _a, _b;
		f32 _k;
	public:
		line2d(const vector2d& a, const vector2d& b);
		virtual ~line2d();

		inline bool vertical() const { return _a.x == _b.x; }
		inline bool horizontal() const { return _a.y == _b.y; }
		void a(const vector2d& p);
		void b(const vector2d& p);

		/* point on line */
		bool contains(const point2d&) const;
		/* 0 for intersects, 1 for not intersect, -1 for covered */
		int intersect(line2d& line, point2d& rslt);

		bool resolv_x(f32 y, f32& x) const;
		bool resolv_y(f32 x, f32& y) const;

		bool seg_contains(const point2d&) const;
		/* 0 for intersects, 1 for not intersect, -1 for covered */
		int seg_intersect(line2d& seg, point2d& rslt);
	};

	bool operator==(const line2d& v1, const line2d& v2);

	class rect
	{
	public:
		vector2d _lt, _rb;
	public:
		rect(const vector2d& lt, const vector2d& rb);
		rect(const line2d& line);
		rect(f32 left, f32 top, f32 right, f32 bottom);
		virtual ~rect();

		inline u32 width() const { return _rb.x - _lt.x; }
		inline u32 height() const { return _rb.y - _lt.y; }
		inline f32 left() const { return _lt.x; }
		inline f32 right() const { return _rb.x; }
		inline f32 top() const { return _lt.y; }
		inline f32 bottom() const { return _rb.y; }

		inline bool contains(const point2d& pts){ return in_range(pts.x, left(), right()) && in_range(pts.y, top(), bottom());}
	};
	bool operator==(const rect& rc1, const rect& rc2);

	bool intersects(const rect& a, const rect& b, rect& ret);

	class polygon2d
	{
	public:
		std::vector<vector2d> _pts;
		std::vector<vector2d> _nms;
		u32 _num;
		bool _convex;
	public:
		polygon2d(const std::vector<vector2d>& pts);
		polygon2d(const polygon2d&);
		virtual ~polygon2d();

        void init(const std::vector<vector2d>& pts);
        void init(const polygon2d&);

		inline rect bbox() const { return rect(0, 0, 0, 0);} // TODO: complete this
		inline bool is_convex() const { return _convex; }
		inline bool is_concave() const { return !_convex; }

		// points or edge nums
		inline u32 num() const { return _num; }
		inline std::vector<vector2d> points() const { return _pts; }
		inline std::vector<vector2d> normals() const { return _nms; }
		std::shared_ptr<line2d> edge(int i) const;
	public:
		static bool judge_convex(const std::vector<vector2d>& pts);
		static void compute_nms(const std::vector<vector2d>& pts, std::vector<vector2d>& nms);
	};

	class polygons
	{
	public:
		static polygon2d Hexagon(f32 sz);
		static polygon2d Hexagram(f32 sz);
	};
}