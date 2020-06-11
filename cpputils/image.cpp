#include "image.h"
#include <cstring>

namespace spes::image
{
	image_t::image_t() : _init(false)
	{}
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

}