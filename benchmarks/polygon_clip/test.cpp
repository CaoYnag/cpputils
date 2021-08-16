#include <iostream>
#include <spes/algo.h>
#include <spes/utils.h>
#include <functional>
using namespace std;
using namespace spes;
using namespace spes::algo;
using namespace spes::math;

long bench(int batch, function<void()> foo)
{
	long s = cur_ms();

	for(int i = 0; i < batch; ++i)
		foo();

	long e = cur_ms();
	return e - s;
}

void bench()
{
	rect rc (-10, -10, 10, 10);
	vector<vector2d> rct_pts = {{-10, -10}, {-10, 10}, {10, 10}, {10, -10}};
	polygon2d wnd = rct_pts;
	vector<vector2d> pts = {{-8, -20}, {-8, 14}, {-2, -14}, {2, 8}, {8, 8}, {8, -12}, {6, -12}, {6, 6}, {4, 6}, {4, -20}};
	polygon2d poly = pts;
	vector<polygon2d> rslt;

	printf("wa\n");
	for(int b = 10000; b <= 1000000; b *= 10)
		cout << b << "\t:" << bench(b, [&](){weiler_atherton(poly, wnd, rslt);rslt.clear();}) << endl;


	printf("sh rc\n");
	for(int b = 10000; b <= 1000000; b *= 10)
		cout << b << "\t:" << bench(b, [&](){ sutherland_hodgman(poly, rc); }) << endl;
	printf("sh nm\n");
	for(int b = 10000; b <= 1000000; b *= 10)
		cout << b << "\t:" << bench(b, [&](){ sutherland_hodgman(poly, wnd); }) << endl;
	printf("sh mdf\n");
	for(int b = 10000; b <= 1000000; b *= 10)
		cout << b << "\t:" << bench(b, [&](){ sutherland_hodgman(poly, rc, rslt);rslt.clear();}) << endl;

}

int main(int argc, char** argv)
{
	bench();
	return 0;
}
