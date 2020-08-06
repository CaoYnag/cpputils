#include <iostream>
#include "../../cpputils/canvas.h"
#include "../../cpputils/imageio.h"
#include <vector>
#include <stdexcept>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;
using namespace spes;
using namespace spes::image;
using namespace spes::image::io;
using namespace spes::canvas;

int main(int argc, char* argv)
{
	const char* write_path = nullptr;
	const char* write_path2 = nullptr;
#ifdef __unix__
	write_path = "/home/spes/temp/canvas1.png";
	write_path2 = "/home/spes/temp/canvas2.png";
#endif
	const u32 SIZE = 700;
	Canvas canvas(SIZE, SIZE);
	Canvas canvas2(SIZE,  SIZE);
	image_io::show_image(canvas.image());
	image_io::show_image(canvas2.image(), "Shapes");


	//canvas.draw_line(line2di({ 50, 50 }, { 650, 50 }));
	//canvas.draw_line(line2di({ 650, 50 }, { 650, 650 }));
	//canvas.draw_line(line2di({ 650, 650 }, { 50, 650 }));
	//canvas.draw_line(line2di({ 50, 650 }, { 50, 50 }));

	//getch();
	/*canvas.draw_line(line2di({ 50, 550 }, { 550, 50 }));
	canvas.draw_line(line2di({ 50, 50 }, { 550, 550 }));*/

	//canvas.draw_line(line2di({ 50, 50 }, { 550, 50 })); // hor
	//canvas.draw_line(line2di({ 50, 50 }, { 50, 550 })); // ver
	//canvas.draw_line(line2di({ 50, 50 }, { 550, 550 })); // +k
	//canvas.draw_line(line2di({ 550, 50 }, { 50, 550 })); // -k

	//getch();

	//cout << "Test LineSegs" << endl;
	//canvas.draw(CRM_LineSegments, { {0, 0}, {50, 50}, {550, 550}, {599, 599} });

	////getch();
	//cout << "Test LineStrip" << endl;
	//canvas.draw(CRM_LineStrip, { {50, 50}, {200, 650}, {350, 50}, {500, 650}, {650, 50} });

	//getch();
	//cout << "Test LineRing" << endl;
	//canvas.draw(CRM_LineRing, { {200, 200}, {400, 200}, {400, 400}, {200, 400} });

	//cout << "press any key to start random draw." << endl;
	//getch();

	//srand(time(nullptr));
	//const u32 offset = CRM_LineRing - CRM_Points + 1;
	//while (true)
	//{
	//	u32 mode = CRM_Points + rand() % offset;
	//	vector<point2di> pts;
	//	for (int i = 0; i < 100; ++i)
	//	{
	//		s32 x = rand() % SIZE,
	//			y = rand() % SIZE;
	//		pts.push_back({x, y});
	//	}
	//	canvas.draw(CRM_LineRing, pts);
	//	cout << "mode " << mode << endl;
	//	getch();
	//}

	canvas.draw_line(line2d({ 0, 350 }, { 700, 350 })); // x axis
	canvas.draw_line(line2d({ 350, 0 }, { 350, 700 })); // y axis


	// x [-pi, pi] [50, 650]
	// y [-1, 1] [50, 650]
	const float PI = 3.1415926535897932384;

	vector<point2d> pts(700);
	for (int x = 50; x < 650; ++x)
	{
		f32 rx = -PI + (x - 50) * 2 * PI / 600.0f;
		f32 ry = sinf(rx);
		f32 y = 50 + (ry + 1) * 600 / 2;
		pts[x - 50] = {x + .0f, y};
	}

	vector<point2d> pts2;
	const f32 PI2 = PI * 2;
	const f32 inc = 0.01;
	for (float theta = .0f; theta <  5 * PI2; theta += inc)
	{
		f32 l = 300 * cosf(theta * 4);// 3 leaves
		//float l = 3 * theta;
		f32 x = l * cosf(theta);
		f32 y = l * sinf(theta);
		pts2.push_back({x + 350, y + 350});
	}
	getch();
	canvas.draw(CRM_LineStrip, pts);
	//canvas2.draw(CRM_LineStrip, pts2, Colors::RED);
	getch();

	canvas2.draw_rect(rect(-10, -10, 300, 300), Colors::AQUA);
	getch();
	image_io::write(canvas.image(), write_path);
	image_io::write(canvas2.image(), write_path2);
	return 0;
}