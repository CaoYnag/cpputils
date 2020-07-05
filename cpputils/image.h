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
		IMP_FMT_GREY
	};
	s32 format_depth(u32 fmt);
	struct im_buff
	{
		u8* buff;
		u32 w, h, fmt;
		im_buff(u32 w, u32 h, u32 fmt);
		im_buff(u32 w, u32 h, u32 fmt, u8* data);
		virtual ~im_buff();
	};

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
		image_t(const image_t&);
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
		inline color_t* buffer() { return _buff; }

		color_t get_pixel(int x, int y) const;
		void set_pixel(int x, int y, color_t  c);

		inline bool valid() const
		{
			return _init;
		}
	};
	bool operator!(const image_t&);

	image_t image_transform(const im_buff&);
	im_buff image_transform(image_t&, u32 fmt);
}