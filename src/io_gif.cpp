#include <spes/io_gif.h>
#include <gif.h>
using namespace std;

namespace spes::image::io
{
	std::vector<shared_ptr<image_t>> read_gif(FILE*)
	{
		// TODO not implemented now
		return {};
	}

	void write_gif(std::vector<shared_ptr<image_t>>& ims, FILE* fp, u32 delay)
	{
		if (ims.size() <= 0) return;
		GifWriter g;
		const char* tmpfs_path = "/dev/shm/gifw_cache";
		GifBegin(&g, tmpfs_path, ims[0]->width(), ims[0]->height(), delay);
		for (auto& im : ims)
			GifWriteFrame(&g, (u8*)im->buffer(), im->width(), im->height(), delay);
		GifEnd(&g);
		FILE* cache = fopen(tmpfs_path, "rb");
		if(!cache) return; // open failed.
		constexpr const int batch_size = 1024 * 1024; // 1m
		char data[batch_size];
		size_t rd;
		while((rd = fread(data, 1, batch_size, cache)) > 0)
			fwrite(data, 1, rd, fp);
		fclose(cache);
	}
}