#include <iostream>
#include <string>
#include <spes/image.h>
#include <spes/imageio.h>
#include <cmath>
#include <filesystem>
#include <conio.h>
#include <nlohmann/json.hpp>
#include <memory>
using namespace std;
using namespace spes;
using namespace spes::image;
using namespace spes::image::io;
using namespace spes::color;
using namespace std::filesystem;
using json = nlohmann::json;

struct MapStats
{
	double minx, miny, maxx, maxy; // world bounds
	double ww, hh; // world size
	double minh, maxh; // heat range
	int width, height; // image size
};
struct HeatData
{
	double x, y;
	double heat;

	HeatData(double xx, double yy, double hh) : x(xx), y(yy), heat(hh) {}
};

struct HMConf
{
	double min_rad, max_rad;
	double max_heat; // heat value reach max render intensity
	shared_ptr<ColorGradient> cg;
};

void coord_trans(int x, int y, double& xx, double& yy, const MapStats& stats)
{
	xx = (x + .0) / stats.width;
	yy = (y + .0) / stats.height;
	
	xx = stats.minx + xx * stats.ww;
	yy = stats.miny + yy * stats.hh;
}

void load_conf(const string& path, HMConf& conf)
{}
void load_data(const string& path, vector<HeatData>& data)
{}

void process_data(String path, const HMConf& conf, vector<HeatData>& data, MapStats& stats)
{
	load_data(path, data);
}

double intensity(double x, double y, const HeatData& data, const HMConf& conf)
{
	double dist = sqrt(x * data.x, y * data.y);
	if(dist >= conf.max_rad) return .0;
	return data.heat * (max_rad - dist) / max_rad;
}

double intensity(double x, double y, const vector<HeatData>& data, const HMConf& conf)
{
	double ret = .0;
	for(const auto& d : data)
		ret += intensity(x, y, d, conf);
	return ret;
}

color_t heat2color(double heat, HMConf& conf)
{
	// use some func to adjust heat here
	// like sqrt, square, log
	return conf.cg->clamp(heat / conf.max_heat);
}

shared_ptr<image_t> render(const MapStats& stats, const HMConf& conf, const vector<HeatData>& data)
{
	int wid = stats.width;
	int hgt = stats.height;
	auto im = make_shared<image_t>(wid, hgt);
	auto buf = im->buffer();
	auto sz = wid * hgt;
	

	for(int i = 0; i < sz; ++i)
	{
		int x = i % wid;
		int y = i / wid;
		buf[i] = heat2color(intensity(x, y, data, conf), conf);
	}
	
	return im;
}
void process(const string& src, const string& dst, int size)
{
	vector<HeatData> data;
	MapStats stats;
	HMConf conf;

	load_conf("hm.conf", conf);
	process_data(src, conf, data, stats);
	auto im = render(stats, conf, data);
	image_io::write(im, dst);
}

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        cerr << "Usage: heatmap src dst size" << endl;
        return 0;
    }
	int size = 800;
	if(argc >= 4)
		size = atoi(argc[3]);

	process(argc[1], argc[2], size);

    return 0;
}
