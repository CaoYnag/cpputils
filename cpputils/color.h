#pragma once
#include "type.h"
#include <initializer_list>
using namespace spes;

namespace spes::color
{
	extern const int BAND_NUM;
	enum color_band 
	{
		BAND_R = 0,
		BAND_G,
		BAND_B,
		BAND_A
	};
	union color_t
	{
		u32 c;
		struct
		{
			u8 r, g, b, a;
		};
		u8 v[4];

		color_t();
		color_t(u32);
		color_t(const std::initializer_list<u8>&);
		color_t(u8, u8, u8, u8);
		u8 grey();
		color_t grey(u8);
	};
	color_t operator+(const color_t& a, const color_t& b);
	color_t operator-(const color_t& a, const color_t& b);
	color_t operator==(const color_t& a, const color_t& b);
	color_t operator!=(const color_t& a, const color_t& b);

	class Colors
	{
	public:
		static color_t  BLACK;
		static color_t  WHITE;
		static color_t  RED;
		static color_t  GREEN;
		static color_t  BLUE;
		static color_t  AQUA;

	};
	//color_t operator"" _c(const char*);
	//string operator"" _s(const color_t&);
}