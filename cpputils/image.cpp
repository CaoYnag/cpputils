#include "image.h"
#include <cstring>

namespace spes::image
{

	s32 format_depth(u32 fmt)
	{
		switch (fmt)
		{
		case IMP_FMT_ARGB:
		case IMP_FMT_XRGB:
			return 4;
		case IMP_FMT_RGB:
			return 3;
		case IMP_FMT_GREY:
			return 1;
		default:
			return 0;
		}
	}
	im_buff::im_buff(u32 ww, u32 hh, u32 ffmt) : w(ww), h(hh), fmt(ffmt)
	{
		u32 sz = w * h;
		s32 dep = format_depth(fmt);
		buff = new u8[sz * dep];
		memset(buff, 0, sz * dep);
	}
	im_buff::im_buff(u32 ww, u32 hh, u32 ffmt, u8* data) : w(ww), h(hh), fmt(ffmt)
	{
		u32 sz = w * h;
		s32 dep = format_depth(fmt);
		buff = new u8[sz * dep];
		if (buff)
			memcpy(buff, data, sz * dep);
		else
			memset(buff, 0, sz * dep);
	}
	im_buff::~im_buff()
	{}

	image_t::image_t() : _init(false), _buff(nullptr)
	{
	}
	image_t::image_t(const image_t& im) : _init(false), _buff(nullptr)
	{
		init(im);
	}
	void image_t::init(u32 w, u32 h)
	{
		init(w, h, nullptr);
	}
	void image_t::init(u32 w, u32 h, color_t* buff)
	{
		_w = w;
		_h = h;
		_size = w * h;
		if (_buff) delete[] _buff;
		_buff = new color_t[_size];
		if (buff)
			memcpy(_buff, buff, _size * BAND_NUM);
		else
			memset(_buff, 0, _size * BAND_NUM);
		_init = true;
	}
	void image_t::init(const image_t& image)
	{
		init(image._w, image._h, image._buff);
	}
	image_t::~image_t()
	{
		if (_buff) delete[] _buff;
	}

	color_t image_t::get_pixel(int x, int y) const
	{
		if (_init)
			return _buff[idx(x, y)];
		else
			return 0;
	}
	void image_t::set_pixel(int x, int y, color_t c)
	{
		if (_init)
		_buff[idx(x, y)] = c;
	}



	image_t image_transform(const im_buff& buff)
	{
		image_t im;

		if (buff.fmt == IMP_FMT_ARGB || buff.fmt == IMP_FMT_XRGB)
		{
			im.init(buff.w, buff.h, (color_t*)buff.buff);
			return im;
		}
		else im.init(buff.w, buff.h);

		auto dst = im.buffer();
		auto src = buff.buff;

		auto sz = buff.w * buff.h;
		for (u32 idx = 0; idx < sz; ++idx)
		{
			color_t c = 0x0;
			switch (buff.fmt)
			{
			case IMP_FMT_RGB:
				c = color_t(src[idx * 3], src[idx * 3 + 1], src[idx * 3 + 2], 0xff);
				break;
			case IMP_FMT_GREY:
				c = color_t(src[idx], src[idx], src[idx], 0xff);
				break;
			default:break;
			}
			dst[idx] = c;
		}

		return im;
	}

	im_buff image_transform(image_t& im, u32 fmt)
	{
		if (fmt == IMP_FMT_ARGB || fmt == IMP_FMT_XRGB)
			return im_buff(im.width(), im.height(), fmt, (u8*)im.buffer());
		im_buff buff(im.width(), im.height(), fmt);

		auto dst = buff.buff;
		auto src = im.buffer();
		u32 sz = im.width() * im.height();
		
		for (u32 idx = 0; idx < sz; ++idx)
		{
			color_t c = src[idx];
			switch (fmt)
			{
			case IMP_FMT_RGB:
			{
				dst[idx * 3] = c.r;
				dst[idx * 3 + 1] = c.g;
				dst[idx * 3 + 2] = c.b;
			}break;
			case IMP_FMT_GREY:
			{
				// assume src image is grey, so  r == g == b
				dst[idx] = c.r;
			}break;
			default: break;
			}
		}

		return buff;
	}


	bool operator!(const image_t& im)
	{
		return !im.valid();
	}
}