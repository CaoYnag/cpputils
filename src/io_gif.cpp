#include <spes/io_gif.h>
#include <gif.h>
using namespace std;

namespace spes::image::io
{
	std::vector<image_t> read_gif(const char* path)
	{
		// TODO not implemented now
		return {};
	}

	void write_gif(std::vector<image_t>& ims, const char* path, u32 delay)
	{
		if (ims.size() <= 0) return;
		GifWriter g;
		GifBegin(&g, path, ims[0].width(), ims[0].height(), delay);
		for (auto& im : ims)
			GifWriteFrame(&g, (u8*)im.buffer(), im.width(), im.height(), delay);
		GifEnd(&g);
	}
}