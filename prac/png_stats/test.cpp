#include <iostream>
#include <string>
#include <spes/image.h>
#include <spes/imageio.h>
#include <cmath>
#include <filesystem>
#include <vector>
using namespace std;
using namespace spes;
using namespace spes::image;
using namespace spes::image::io;
using namespace spes::color;
using namespace std::filesystem;

void pro(const char* src)
{
    auto img = image_io::read(src);
	if(!img->valid())
	{
		cerr << "err read " << src << endl;
		return;
	}
	int W = img->width();
	int H = img->height();
	int sz = W * H;
	auto raw = img->buffer();

	int max = -9999, min = 0xffffff;
	float s = 1.f / sz;
	float mean = .0f;
	for(int i = 0; i < sz; ++i)
	{
		int val = raw[i].r << 16;
		val |= raw[i].g << 8;
		val |= raw[i].b;
		val /= 10;
		val -= 10000;
		if(val > max) max = val;
		if(val < min) min = val;
		mean += val * s;
	}

	printf("max: %d, min: %d, mean: %.2f\n", max, min, mean);
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        cerr << "Usage: png_stats file" << endl;
        return 0;
    }
    pro(argv[1]);

    return 0;
}
