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
	const char* png_path = "D:\\temp\\img_pro\\interlude_01.png";
	const char* png_write_path = "D:\\temp\\img_pro\\png_write_test.png";
	const char* jpeg_path = "D:\\temp\\img_pro\\ac_morty.jpg";
	const char* jpeg_write_path = "D:\\temp\\img_pro\\jpeg_write_test.png";
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


	/*cout << "Reading png file " << png_path << endl;
	auto im = image_io::read(png_path);
	if (!im)
	{
		cout << "err read im " << png_path << endl;
	}
	else
	{
		cout << "img size (" << im.width() << " * " << im.height() << ")" << endl;
		image_io::show_image(im, "PNG Image");
		image_io::write(im, png_write_path);
	}*/

	cout << "Reading jpeg file " << jpeg_path << endl;
	auto jim = image_io::read(jpeg_path);
	if (!jim)
	{
		cout << "err read im " << jpeg_path << endl;
	}
	else
	{
		cout << "img size (" << jim.width() << " * " << jim.height() << ")" << endl;
		image_io::show_image(jim, "Jpeg Image");
		image_io::write(jim, jpeg_write_path, IMAGE_FMT_JPEG);
	}
	
	/*buff = im.buffer();
	for (int i = 0; i < im.width() * im.height(); ++i)
	{
		buff[i] = buff[i].grey(255 - buff[i].grey());
	}*/
	//image_io::show_image(im);

	getch();
	return 0;
}