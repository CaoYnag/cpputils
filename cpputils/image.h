#pragma once
#include "color.h"
using namespace spes::color;

namespace spes::image
{
	class image_t
	{
	protected:
		u32 _w, _h;
		u64 _size;
		color_t* _buff;
		bool _init;

		inline u32 idx(u32 x, u32 y) const
		{
			return x + y * _w;
		}
	public:
		image_t();
		virtual ~image_t();

		void init(u32, u32);
		void init(u32, u32, color_t*);
		void init(const image_t&);

		inline u32 width() const
		{
			return _w;
		}
		inline u32 height() const
		{
			return _h;
		}
		inline color_t* buffer();

		color_t get_pixel(int x, int y) const;
		void set_pixel(int x, int y, color_t  c);

		inline bool valid() const
		{
			return _init;
		}
	};
}