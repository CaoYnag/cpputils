#pragma once
#include <vector>
#include <cmath>
#include "type.h"
using namespace spes;

namespace spes::math
{
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

	template<typename T>
	union vector2d
	{
		struct
		{
			T x, y;
		};
		struct
		{
			T w, h;
		};
		struct
		{
			T s, e;
		};
	};

	typedef vector2d<s32> vector2di, size2di, point2di, range2i;
	typedef vector2d<f32> vector2df, size2df, point2df, range2f;

	template<typename T>
	vector2d<T> operator+(const vector2d<T>& v1, const vector2d<T>& v2)
	{
		return { v1.x + v2.x, v1.y + v2.y };
	}
	template<typename T>
	vector2d<T> operator-(const vector2d<T>& v)
	{
		return { -v.x, -v.y };
	}
	template<typename T>
	vector2d<T> operator-(const vector2d<T>& v1, const vector2d<T>& v2)
	{
		return { v1.x - v2.x, v1.y - v2.y };
	}
	template<typename T>
	s32 operator*(const vector2d<T>& v1, const vector2d<T>& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}
	template<typename T>
	vector2d<T> operator*(const vector2d<T>& v, f32 s)
	{
		return { (T)(v.x * s), (T)(v.y * s) };
	}
	template<typename T>
	bool operator==(const vector2d<T>& v1, const vector2d<T>& v2)
	{
		return v1.x == v2.x && v1.y == v2.y;
	}

	/*
	scale rect, makes it can be included in border
	*/
	size2di rect_adjust(size2di border, size2di rect);

	template<typename T>
	class line2d
	{
	private:
		void gen_k()
		{
			if (vertical()) _k = 0;
			else _k = (_b.y - _a.y + .0f) / (_b.x - _a.x);
		}
	public:
		vector2d<T> _a, _b;
		f32 _k;
	public:
		line2d(const vector2d<T>& a, const vector2d<T>& b) : _a(a), _b(b)
		{
			gen_k();
		}
		virtual ~line2d()
		{}

		inline bool vertical() const { return _a.x == _b.x; }
		inline bool horizontal() const { return _a.y == _b.y; }
		void a(const vector2d<T>& p)
		{
			_a = p;
			gen_k();
		}
		void b(const vector2d<T>& p)
		{
			_b = p;
			gen_k();
		}
	};
	typedef line2d<s32> line2di;
	typedef line2d<f32> line2df;

	template<typename T>
	bool operator==(const line2d<T>& v1, const line2d<T>& v2)
	{
		return (v1._a == v2._a && v1._b == v2._b) || (v1._a == v2._b && v1._b == v2._a);
	}

	template<typename T>
	class rect
	{
	public:
		vector2d<T> _lt, _rb;
		u32 _w, _h;
	public:
		rect(const vector2d<T>& lt, const vector2d<T>& rb) : _lt(lt), _rb(rb), _w(rb.x - lt.x), _h(rb.y - lt.y)
		{}
		rect(T left, T top, T right, T bottom) : _lt({ left, top }), _rb({ right, bottom }), _w(right - left), _h(bottom - top)
		{}
		virtual ~rect() {}

		inline u32 width() const { return _w; }
		inline u32 height() const { return _h; }
		inline T left() const { return _lt.x; }
		inline T right() const { return _rb.x; }
		inline T top() const { return _lt.y; }
		inline T bottom() const { return _rb.y; }
	};
	typedef rect<s32> recti;
	typedef rect<f32> rectf;

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
	template<typename T>
	KEY GenKey(const vector2d<T>& p, const rect<T>& r)
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
	template<typename T>
	int cohen_sutherland(line2d<T>& line, const rect<T>& rect)
	{
		vector2d<T> start = line._a;
		vector2d<T> end = line._b;
		KEY s, e;

		for (unsigned int i = 0; i < 4; ++i)
		{

			s = GenKey(start, rect);
			e = GenKey(end, rect);

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
					float scale = (rect.bottom() - end.y + .0f) / (start.y - end.y);
					start.x = (start.x - end.x) * scale + end.x;
					start.y = rect.bottom();
				}
				if (e & _b)
				{
					float scale = (rect.bottom() - start.y + .0f) / (end.y - start.y);
					end.x = (end.x - start.x) * scale + start.x;
					end.y = rect.bottom();
				}
			}break;
			case 1:
			{
				//right
				if (s & _b)
				{
					float scale = (rect.right() - end.x + .0f) / (start.x - end.x);
					start.x = rect.right();
					start.y = (start.y - end.y) * scale + end.y;
				}
				if (e & _b)
				{
					float scale = (rect.right() - start.x + .0f) / (end.x - start.x);
					end.x = rect.right();
					end.y = (end.y - start.y) * scale + start.y;
				}
			}break;
			case 2:
			{
				//above
				if (s & _b)
				{
					float scale = (rect.top() - end.y) / (start.y - end.y);
					start.x = (start.x - end.x) * scale + end.x;
					start.y = rect.top();
				}
				if (e & _b)
				{
					float scale = (rect.top() - start.y) / (end.y - start.y);
					end.x = (end.x - start.x) * scale + start.x;
					end.y = rect.top();
				}
			}break;
			case 3:
			{
				//left
				if (s & _b)
				{
					float scale = (rect.left() - end.x) / (start.x - end.x);
					start.x = rect.left();
					start.y = (start.y - end.y) * scale + end.y;
				}
				if (e & _b)
				{
					float scale = (rect.left() - start.x) / (end.x - start.x);
					end.x = rect.left();
					end.y = (end.y - start.y) * scale + start.y;
				}
			}break;
			}
		}
		s = GenKey(start, rect);
		e = GenKey(end, rect);

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

	template<typename T>
	bool is_convex(const std::vector<vector2d<T>>& pts)
	{
		// TODO
		return true;
	}

	template<typename T>
	class polygon2d
	{
	public:
		std::vector<vector2d<T>> _pts;
		u32 _num;
		bool _convex;
	public:
		polygon2d(const std::vector<vector2d<T>>& pts) : _pts(pts), _num(pts.size())
		{
			_convex = is_convex(pts);
		}
		virtual ~polygon2d() {}

		inline u32 num() const { return _num; }
		inline rect<T> bbox() const {}
		inline bool is_convex() const { return _convex; }
		inline bool is_concave() const { return !_convex; }
	};
	typedef polygon2d<s32> polygon2di;
	typedef polygon2d<f32> polygon2df;
}