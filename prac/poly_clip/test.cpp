#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <spes/canvas.h>
#include <spes/imageio.h>
#include <spes/math.h>
#include <spes/algo.h>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cassert>
using namespace std;
using namespace spes;
using namespace spes::image;
using namespace spes::math;
using namespace spes::image::io;
using namespace spes::canvas;
using namespace spes::algo;

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800

constexpr const int CA_SH = 0; // sutherland hodgman
constexpr const int CA_WA = 1; // weiler atherton

int clip_algo = 0;
int wa_state = 0;
vector<point2d> pts;
shared_ptr<polygon2d> clip_wnd;
vector<polygon2d> polys;
vector<polygon2d> clipped;

// clip last poly in polys into clipped
void do_clip()
{
	switch(clip_algo)
	{
	case CA_SH:
	{
		clipped.emplace_back(*sutherland_hodgman(polys.back(), *clip_wnd));
	}break;
	case CA_WA:
	{
		weiler_atherton(polys.back(), *clip_wnd, clipped);
	}break;
	}
}

void init()
{
    glClearColor(.0, .0, .0, 0.0); // black background
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)SCREEN_WIDTH, (GLdouble)SCREEN_HEIGHT, 0.0);
    glViewport(0.0, SCREEN_WIDTH, 0.0, SCREEN_HEIGHT);


	//1.开启混合功能
	glEnable(GL_BLEND);

	//2.指定混合因子
	//注意:如果你修改了混合方程式,当你使用混合抗锯齿功能时,请一定要改为默认混合方程式
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//3.开启对点\线\多边形的抗锯齿功能
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);

}
void kb_callback(unsigned char key, int xx, int yy)
{
    switch (key)
    {
	case 27: exit(0);
	case 'c':
	{
		// clear all.
		clip_wnd = nullptr;
		wa_state = 0;
	}
	case 'q':
	{
		// do not clear clip wnd
		pts.clear();
		polys.clear();
		clipped.clear();
	}break;
	case 's':
	{
		clip_algo = CA_SH;
		printf("changed clip algo to sutherland hodgman.\n");
	}break;
	case 'w':
	{
		clip_algo = CA_WA;
		printf("changed clip algo to weiler atherton.\n");
	}break;
    }
}
void mouse_callback(int button, int state, int x, int y)
{
    if(state == GLUT_DOWN)
    {
        // pick pts of a poly
        if(button == GLUT_LEFT_BUTTON)
        {
            printf("pick (%d, %d)\n", x, y);
            pts.push_back({x, y});
        }
        // end pick pts, construct new poly and clip it
        if(button == GLUT_RIGHT_BUTTON)
        {
            if(pts.size() < 3) return;
            if(wa_state == 0) // building a clip window
            {
	            clip_wnd = make_shared<polygon2d>(pts);
	            printf("clip window settled with %d pts\n", clip_wnd->num());
	            wa_state = 1; // change state.
            }
            else
            {
	            polys.emplace_back(pts);
	            do_clip();
            }
	        pts.clear();
        }
    }
}

void draw_polygon(const polygon2d& poly, bool draw_norm = true)
{
	glBegin(GL_LINE_LOOP);
	for(auto& pt : poly.points())
			glVertex2d(pt.x, pt.y);
	glEnd();
	if(draw_norm)
	{
		glBegin(GL_LINES);
		for(int i = 0; i < poly.num(); ++i)
		{
			auto edge = poly.edge(i);
			auto nm = poly.normals().at(i);
			point2d center = edge->clamp(0.5f);
			auto temp = normalize(nm) * 20;
			glVertex2d(center.x, center.y);
			glVertex2d(center.x + temp.x, center.y + temp.y);
		}
		glEnd();
	}
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GLUT_MULTISAMPLE);

    // draw clip wnd
    glColor3f(1., 1., 1.);
    glPointSize(2.0);
    if(clip_wnd) draw_polygon(*clip_wnd);

    glPointSize(1.0);
    // draw picking lines
    glColor3f(.0, 1., .0);
    glBegin(GL_LINE_STRIP);
    for(auto& p : pts)
        glVertex2d(p.x, p.y);
    glEnd();

    // draw origin polys
    glColor3f(1., .0, .0);
    for(auto& poly : polys)
    {
	    draw_polygon(poly);
    }

    // draw clipped polys
    glPointSize(2.0);
    glColor3f(1., 1., .0);
    for(auto& poly : clipped) draw_polygon(poly, false); // do not draw norms for clipped poly.

    glFlush();

	glDisable(GLUT_MULTISAMPLE);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_MULTISAMPLE);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Polygon Clipping GL Sample");
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutMouseFunc(mouse_callback);
    glutKeyboardFunc(kb_callback);

    init();
    glutMainLoop();

    return 0;
}
