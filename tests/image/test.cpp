#include <iostream>
#include "../../cpputils/image.h"
#include "../../cpputils/imageio.h"
#include <vector>
#include <stdexcept>
#include <conio.h>
using namespace std;
using namespace spes;
using namespace spes::image;
using namespace spes::image::io;

int main(int argc, char* argv)
{
#ifdef WIN32
	const char* path = "D:\\temp\\img_pro\\interlude_01.png";
	const char* write_path = "D:\\temp\\img_pro\\write_test.png";
#endif
#ifdef UNIX
	const char* path = "";
#endif

	/*image_t mem;
	const int size = 600;
	const int num = size * size;
	mem.init(size, size);
	auto buff = mem.buffer();
	for (int i = 0; i < num; ++i)
	{
		buff[i] = color_t(0, 255, 0, 255);
	}
	image_io::show_image(mem);
	image_io::write(mem, write_path);*/


	cout << "Reading file " << path << endl;
	auto im = image_io::read(path);
	cout << "img size (" << im.width() << " * " << im.height() << ")" << endl;
	image_io::show_image(im);
	image_io::write(im, write_path);

	/*buff = im.buffer();
	for (int i = 0; i < im.width() * im.height(); ++i)
	{
		buff[i] = buff[i].grey(255 - buff[i].grey());
	}*/
	//image_io::show_image(im);

	getch();
	return 0;
}