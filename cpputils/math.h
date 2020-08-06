#pragma once
#include <vector>
#include <cmath>
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

	 vector2d operator+(const vector2d& v1, const vector2d& v2);
	 vector2d operator-(const vector2d& v);
	 vector2d operator-(const vector2d& v1, const vector2d& v2);
	 s32 operator*(const vector2d& v1, const vector2d& v2);
	 vector2d operator*(const vector2d& v, f32 s);
	 bool operator==(const vector2d& v1, const vector2d& v2);
	 f32 cross(vector2d& v1, vector2d& v2);

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

		bool resolv_x(f32 y, f32& x);
		bool resolv_y(f32 x, f32& y);
	};

	bool operator==(const line2d& v1, const line2d& v2);

	class rect
	{
	public:
		vector2d _lt, _rb;
	public:
		rect(const vector2d& lt, const vector2d& rb);
		rect(f32 left, f32 top, f32 right, f32 bottom);
		virtual ~rect();

		inline u32 width() const { return _rb.x - _lt.x; }
		inline u32 height() const { return _rb.y - _lt.y; }
		inline f32 left() const { return _lt.x; }
		inline f32 right() const { return _rb.x; }
		inline f32 top() const { return _lt.y; }
		inline f32 bottom() const { return _rb.y; }
	};
	bool operator==(const rect& rc1, const rect& rc2);

	bool intersects(const rect& a, const rect& b, rect& ret);

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

	KEY GenKey(const vector2d& p, const rect& r);

	int cohen_sutherland(line2d& line, const rect& rect);


	class polygon2d
	{
	public:
		std::vector<vector2d> _pts;
		std::vector<vector2d> _nms;
		u32 _num;
		bool _convex;
	public:
		polygon2d(const std::vector<vector2d>& pts);
		virtual ~polygon2d();

		inline u32 num() const { return _num; }
		inline rect bbox() const {}
		inline bool is_convex() const { return _convex; }
		inline bool is_concave() const { return !_convex; }
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