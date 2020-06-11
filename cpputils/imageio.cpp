#include "imageio.h"
#include "io_png.h"
using namespace spes::image;


namespace spes::image::io
{
	image_t image_io::read(const char* path)
	{
		return read_png(path);
	}

	void image_io::write_image(const image_t& im, const char* path, u32 fmt)
	{
		write_png(im, path);
	}
}