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
using namespace std;
using namespace spes;
using namespace spes::algo;
using namespace spes::image;
using namespace spes::math;
using namespace spes::image::io;
using namespace spes::canvas;

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 400
rect rc(SCREEN_WIDTH * 0.2, SCREEN_HEIGHT * 0.2, SCREEN_WIDTH * 0.8, SCREEN_HEIGHT * 0.8); // clip wnd

vector<point2d> pts;
vector<polygon2d> polys;
vector<polygon2d> clipped;

void init()
{
    printf("clip window: [%.2f, %.2f, %.2f, %.2f]\n", rc.left(), rc.top(), rc.right(), rc.bottom());
    glClearColor(.0, .0, .0, 0.0); // black background
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)SCREEN_WIDTH, (GLdouble)SCREEN_HEIGHT, 0.0);
    glViewport(0.0, SCREEN_WIDTH, 0.0, SCREEN_HEIGHT);
}
void kb_callback(unsigned char key, int xx, int yy)
{
    switch (key)
    {
        case 27: exit(0);
        case 'c':
        {
            pts.clear();
            polys.clear();
            clipped.clear();
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
            if(pts.size() < 1) return;
            polys.emplace_back(pts);
            int sz = clipped.size();
            sutherland_hodgman(polys[polys.size() - 1], rc, clipped);
            printf("end pick, split into %d polys.\n\n", (clipped.size() - sz));
            pts.clear();
        }
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // draw clip wnd
    glColor3f(1., 1., 1.);
    glPointSize(2.0);
    glBegin(GL_LINE_LOOP);
    glVertex2d(rc.left(), rc.top());
    glVertex2d(rc.right(), rc.top());
    glVertex2d(rc.right(), rc.bottom());
    glVertex2d(rc.left(), rc.bottom());
    glEnd();

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
        glBegin(GL_LINE_LOOP);
        for(auto& p : poly._pts)
            glVertex2d(p.x, p.y);
        glEnd();
    }

    // draw clipped polys
    glPointSize(2.0);
    glColor3f(1., 1., .0);
    for(auto& poly : clipped)
    {
        glBegin(GL_LINE_LOOP);
        for(auto& p : poly._pts)
            glVertex2d(p.x, p.y);
        glEnd();
    }

    glFlush();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Sutherland Hodgman GL Sample");
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutMouseFunc(mouse_callback);
    glutKeyboardFunc(kb_callback);

    init();
    glutMainLoop();

    return 0;
}