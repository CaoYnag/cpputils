\\\\\\\\\\\\\\#include <iostream>
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
using namespace nlohmann;

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
	double min_rad, max_rad; // conf in pix. trans to coord
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
{
	auto sz = file_size(path);
	char buff[sz];
	FILE* fp = fopen(path.c_str(), "r");
	if(!fp)
		throw runtime_error("cannot open con file.");
	fread(buff, 1, sz, fp);
	fclose(fp);

	json j = json::parse(string(buff, sz));
	conf.min_rad = j["min_rad"];
	conf.max_rad = j["max_rad"];
	conf.max_heat = j["max_heat"];

	vector<color_t> colors;
	json cgj = j["color_scheme"];
	for(json& c : cgj)
		colors.emplace_back(c["r"], c["g"], c["b"], c["a"]);
	conf.cg = make_shared<ColorGradient>(colors);
}
void load_data(const string& path, vector<HeatData>& data)
{
	auto sz = file_size(path);
	char buff[sz];
	FILE* fp = fopen(path.c_str(), "r");
	if(!fp) throw runtime_error("cannot open data file.");
	fread(buff, 1, sz, fp);
	fclose(fp);

	json d = json::parse(string(buff, sz));
	for(json& sd : d)
	{
		data.emplace_back(sd["lng"], sd["lat"], sd["count"]);
	}
}

void process_data(const string& path, int size, HMConf& conf, vector<HeatData>& data, MapStats& stats)
{
	load_data(path, data);
	stats.minx = 99999;
	stats.maxx = -999999;
	stats.miny = 999999;
	stats.maxy = -99999;
	stats.minh = 99999;
	stats.maxh = -99999;
	
	for(auto& d : data)
	{
		if(d.x < stats.minx) stats.minx = d.x;
		if(d.x > stats.maxx) stats.maxx = d.x;
		if(d.y < stats.miny) stats.miny = d.y;
		if(d.y > stats.maxy) stats.maxy = d.y;

		if(d.heat < stats.minh) stats.minh = d.heat;
		if(d.heat > stats.maxh) stats.maxh = d.heat;
	}



	int rs = size - conf.max_rad * 2;
	stats.ww = stats.maxx - stats.minx;
	stats.hh = stats.maxy - stats.miny;
	double pre = .0;
	
	if(stats.ww > stats.hh)
	{
		stats.width = size;
		stats.height = size * stats.hh / stats.ww;
		pre = stats.ww / rs;
	}
	else
	{
		stats.height = size;
		stats.width = size * stats.ww / stats.hh;
		pre = stats.hh / rs;
	}

	conf.max_rad *= pre;
	conf.min_rad *= pre;
	stats.minx -= conf.max_rad;
	stats.maxx += conf.max_rad;
	stats.miny -= conf.max_rad;
	stats.maxy += conf.max_rad;
	
	stats.ww = stats.maxx - stats.minx;
	stats.hh = stats.maxy - stats.miny;
}

double intensity(double x, double y, const HeatData& data, const HMConf& conf)
{
	x -= data.x;
	y -= data.y;
	double dist = sqrt(x * x + y * y);
	if(dist >= conf.max_rad) return .0;
	return data.heat * (conf.max_rad - dist) / conf.max_rad;
}

double intensity(double x, double y, const vector<HeatData>& data, const HMConf& conf)
{
	double ret = .0;
	for(const auto& d : data)
		ret += intensity(x, y, d, conf);
	return ret;
}

color_t heat2color(double heat, const HMConf& conf)
{
	// use some func to adjust heat here
	// like sqrt, square, log
	return conf.cg->clamp(heat / conf.max_heat);
}

shared_ptr<image_t> render(const MapStats& stats, const HMConf& conf, const vector<HeatData>& data)
{
	int wid = stats.width;
	int hgt = stats.height;
	auto im = make_shared<image_t>();
	im->init(wid, hgt);
	auto buf = im->buffer();
	auto sz = wid * hgt;
	

	for(int i = 0; i < sz; ++i)
	{
		int x = i % wid;
		int y = i / wid;
		double xx, yy;
		coord_trans(x, y, xx, yy, stats);
		buf[i] = heat2color(intensity(xx, yy, data, conf), conf);
	}
	
	return im;
}

void draw_cg_strip(shared_ptr<ColorGradient> cg)
{
	auto im = make_shared<image_t>();

	int wid = 512;
	int hgt = 64;
	im->init(wid, hgt);
	auto buf = im->buffer();
	for(int x = 0; x < wid; ++x)
	{
		auto c = cg->clamp((x + .0) / wid).c;
		for(int y = 0; y < 64; ++y)
			buf[x + y * wid] = c;
	}
	image_io::write(im, "strip.png", IMAGE_FMT_PNG);
	
}
void process(const string& src, const string& conf_path, const string& dst, int size)
{
	vector<HeatData> data;
	MapStats stats;
	HMConf conf;

	load_conf(conf_path, conf);
	process_data(src, size, conf, data, stats);
	{
		// do some log here
		printf("world bnds: (%lf %lf %lf %lf)\n", stats.minx, stats.miny, stats.maxx, stats.maxy);
		printf("world size: (%lf %lf)\n", stats.ww, stats.hh);
		printf("heat range: (%lf %lf)\n", stats.minh, stats.maxh);
		printf("image size: (%d %d)\n", stats.width, stats.height);
		printf("conf: rad (%lf %lf) heat: %f\n", conf.min_rad, conf.max_rad, conf.max_heat);
	}
	draw_cg_strip(conf.cg);
	auto im = render(stats, conf, data);
	image_io::write(im, dst.c_str(), IMAGE_FMT_PNG);
}

int main(int argc, char** argv)
{
    if(argc < 4)
    {
        cerr << "Usage: heatmap src conf dst size" << endl;
        return 0;
    }
	int size = 800;
	if(argc >= 5)
		size = atoi(argv[4]);

	process(argv[1], argv[2], argv[3], size);

    return 0;
}
