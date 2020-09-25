#include <iostream>
#include <spes/image.h>
#include <spes/imageio.h>
#include <vector>
#include <stdexcept>
#include <conio.h>
using namespace std;
using namespace spes;
using namespace spes::image;
using namespace spes::image::io;

void test_screen()
{
    auto sz = image_io::screen_size();
    printf("screen size: %.0fx%.0f\n", sz.w, sz.h);
}

void test_show()
{
    auto yuutsu = image_io::read("/home/spes/Resource/res/yuutsu.jpg");
    printf("loaded img %dx%d\n", yuutsu.width(), yuutsu.height());
    image_io::show_image(yuutsu, "cat");

    auto shit = image_io::read("/home/spes/Resource/res/shit.jpeg");
    printf("loaded img %dx%d\n", shit.width(), shit.height());
    image_io::show_image(shit, "shit");
}

int main(int argc, char* argv[])
{
    test_screen();
    test_show();
    getch();
	return 0;
}