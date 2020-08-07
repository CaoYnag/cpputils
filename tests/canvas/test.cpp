#include <iostream>
#include "../../cpputils/canvas.h"
#include "../../cpputils/imageio.h"
#include "../../cpputils/math.h"
#include <vector>
#include <stdexcept>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;
using namespace spes;
using namespace spes::image;
using namespace spes::math;
using namespace spes::image::io;
using namespace spes::canvas;

f32 cross(vector2d& v1, vector2d& v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

int main(int argc, char** argv)
{
	const u32 SIZE = 700;
	Canvas canvas(SIZE, SIZE);
	image_io::show_image(canvas.image());


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


	/* DRAW SIN FUNCTION */
	/*canvas.draw_line(line2d({ 0, 350 }, { 700, 350 })); // x axis
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
	getch();*/

	/* DRAW TRIANGLES */
	/*canvas.draw_rect(rect(-10, -10, 300, 300), Colors::AQUA);
	canvas.draw_triangle({ {400.f, .0f}, {400.0f, 400.f}, {600.f, 400.f} }, Colors::RED);  // sit1
	canvas.draw_triangle({ {400.f, 400.0f}, {600.0f, 400.f}, {600.f, 700.f} }, Colors::GREEN);  // sit2
	canvas.draw_triangle({ {100.f, 400.0f}, {0.0f, 600.f}, {300.f, 700.f} }, Colors::BLUE);  // sit3
	getch();*/

	/* DRAW CONVEX POLYGON */
	/*vector<point2d> poly =
	{
		{350.f, 50.f}, {650.f, 200.f}, {650.f, 500.f},
		{350.f, 650.f}, {50.f, 500.f}, {50.f, 200.f}
	}; // Hexagon
	for (int i = 1; i < poly.size() - 1; ++i)
	{
		canvas.draw_triangle({ poly[0], poly[i], poly[i + 1] }, Colors::random());
	}
	canvas.draw(CRM_LineRing, poly);
	getch();*/

	vector<point2d> concave =
	{
		{350.f, 50.f}, {450.f, 200.f}, {650.f, 200.f}, {550.f, 350.f}, {650.f, 500.f},
		{450.f, 500.f}, {350.f, 650.f}, {250.f, 500.f}, {50.f, 500.f}, {150.f, 350.f},
		{50.f, 200.f}, {250.f, 200.f}
	};// Hexagram
	vector<point2d> inter =
	{
		//{350.f, 50.f}, 
		{450.f, 200.f},
		//{650.f, 200.f}, 
		{550.f, 350.f},
		//{650.f, 500.f},
		{450.f, 500.f},
		//{350.f, 650.f}, 
		{250.f, 500.f},
		//{50.f, 500.f}, 
		{150.f, 350.f},
		//{50.f, 200.f}, 
		{250.f, 200.f}
	};
	/*vector<point2d> inter =
	{
		{350.f, 100.f}, {500.f, 350.f},
		{350.f, 600.f}, {200.f, 350.f}
	};*/

	//canvas.draw_line(line2d({ 300.f, 300.f }, { 400.f, 300.f }), Colors::RED);
	//canvas.draw_line(line2d({ 400.f, 300.f }, { 400.f, 400.f }), Colors::GREEN);
	//canvas.draw_line(line2d({ 400.f, 400.f }, { 300.f, 400.f }), Colors::BLUE);
	//canvas.draw_line(line2d({ 300.f, 400.f }, { 300.f, 300.f }), Colors::PURPLE);

	//canvas.draw_line(line2d({ 300.f, 300.f }, { 400.f, 400.f }), Colors::BLACK);
	//canvas.draw_line(line2d({ 400.f, 300.f }, { 300.f, 400.f }), Colors::AQUA);
	//getch();

	/*for (int i = 1; i < inter.size() - 1; ++i)
	{
		canvas.draw_triangle({ inter[0], inter[i], inter[i + 1] }, Colors::PURPLE);
	}
	canvas.draw(CRM_LineRing, inter, Colors::BLACK);*/

	//auto poly = polygons::Hexagram(700);
	vector<point2d> pts = inter;
	//pts = { {100.f, 100.f}, {400.f, 200.f}, {600.f, 100.f}, {400.f, 400.f}, {600.f, 600.f}, {100.f, 600.f} };
	/*pts =
	{
		{350.f, 100.f}, {400.f, 300.f}, {600.f, 350.f}, {400.f, 400.f},
		{350.f, 600.f}, {300.f, 400.f}, {100.f, 350.f}, {300.f, 300.f}
	};*/

	/*pts =
	{
		{100.f, 100.f}, {350.f, 600.f}, {600.f, 100.f}, {350.f, 300.f}
	};*/
	canvas.draw(CRM_LineRing, { {.0f, .0f}, {699.f, 0.f}, {699.f, 699.f}, {0.f, 699.f} }, Colors::AQUA);
	canvas.draw(CRM_LineRing, pts);
	getch();

	/*color_t colors[] = { Colors::RED, Colors::GREEN, Colors::BLUE, Colors::AQUA };
	u32 cidx = 0;
	vector<point2d> tmp = pts;
	while (tmp.size() > 2)
	{
		cout << "remain points num: " << tmp.size() << endl;

		int idx = 0;
		for (; idx < tmp.size() - 2; ++idx)
		{
			auto pt1 = tmp[idx], pt2 = tmp[idx + 1], pt3 = tmp[idx + 2];
			auto v1 = pt2 - pt1, v2 = pt3 - pt2;
			auto r1 = cross(v1, v2);
			if (r1 > 0)
			{
				cout << "draw tri at " << idx << endl;
				canvas.draw_triangle({ pt1, pt2, pt3 }, colors[cidx++ % 4]);
				tmp.erase(tmp.begin() + idx + 1);

				if (idx == 0)
				{
					pt1 = tmp[tmp.size() - 1], pt2 = tmp[idx], pt3 = tmp[idx + 1];
					v1 = pt2 - pt1, v2 = pt3 - pt2;
					r1 = cross(v1, v2);

					if (r1 == 0)
					{
						tmp.erase(tmp.begin() + idx);
						cout << "straight before." << endl;
					}
				}
				break;
			}
			if (r1 == 0)
			{
				cout << "straight at " << idx << endl;
				tmp.erase(tmp.begin() + idx + 1);
				break;
			}
		}
		canvas.draw(CRM_LineRing, pts);
		getch();
	}
	*/
	canvas.draw_polygon(pts);
	getch();
	image_io::write(canvas.image(), "D:\\temp\\img_pro\\concave.jpg", IMAGE_FMT_JPG);
	cout << "write done." << endl;
	getch();
	return 0;
}