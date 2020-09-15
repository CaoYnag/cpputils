#pragma once
#include "color.h"
using namespace spes::color;

namespace spes::image
{
	enum IMAGE_PIXEL_FORMAT
	{
		IMP_FMT_ARGB,
		IMP_FMT_XRGB,
		IMP_FMT_RGB,
		IMP_FMT_GREY,
		IMP_FMT_HSV
	};
	s32 format_depth(u32 fmt);
	struct im_buff
	{
		u8* buff;
        s32 w, h, fmt;
		im_buff(s32 w, s32 h, u32 fmt);
		im_buff(s32 w, s32 h, u32 fmt, u8* data);
		im_buff(const im_buff& o);
		virtual ~im_buff();
	};

	class image_t
	{
	protected:
		s32 _w, _h;
		s64 _size;
		color_t* _buff;
		bool _init;

		inline u32 idx(u32 x, u32 y) const
		{
			return x + y * _w;
		}
	public:
		image_t();
		image_t(const image_t&);
		virtual ~image_t();

		void init(s32, s32);
		void init(s32, s32, color_t*);
		void init(const image_t&);

		inline s32 width() const
		{
			return _w;
		}
		inline s32 height() const
		{
			return _h;
		}
		inline color_t* buffer() { return _buff; }

		color_t get_pixel(s32 x, s32 y) const;
		void set_pixel(s32 x, s32 y, color_t  c);
		/* draw img at (x, y) */
		void set_pixels(const image_t&, s32 x, s32 y);

		inline bool valid() const
		{
			return _init;
		}
	};
	bool operator!(const image_t&);

	image_t image_transform(const im_buff&);
	im_buff image_transform(image_t&, u32 fmt);
}