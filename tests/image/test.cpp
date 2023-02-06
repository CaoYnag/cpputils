#include <iostream>
#include <spes/image.h>
#include <spes/imageio.h>
#include <vector>
#include <stdexcept>
#include <conio.h>
using namespace std;
using namespace spes;
using namespace spes::color;
using namespace spes::image;
using namespace spes::image::io;

void test1() {
#ifdef _WIN32
    const char* png_path = "D:\\temp\\img_pro\\interlude_01.png";
    const char* png_write_path = "D:\\temp\\img_pro\\png_write_test.png";
    const char* jpeg_path = "D:\\temp\\img_pro\\ac_morty.jpg";
    const char* jpeg_write_path = "D:\\temp\\img_pro\\jpeg_write_test.jpg";
#endif
#ifdef __unix__
    const char* png_path = "/home/spes/Pictures/wp_sel/13517568-2edda26680447bb2.png";
    const char* png_write_path = "/home/spes/temp/png_write_test.png";
    const char* jpeg_path = "/home/spes/Pictures/wp_sel/sakura.jpg";
    const char* jpeg_write_path = "/home/spes/temp/jpeg_write_test.jpg";
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


    cout << "Reading png file " << png_path << endl;
    auto im = image_io::read(png_path);
    if (!im)
    {
        cout << "err read im " << png_path << endl;
    }
    else
    {
        cout << "img size (" << im->width() << " * " << im->height() << ")" << endl;
        //image_io::show_image(im, "PNG Image");
        image_io::write(im, png_write_path);
    }
    /*
    cout << "Reading jpeg file " << jpeg_path << endl;
    auto jim = image_io::read(jpeg_path);
    if (!jim) {
        cout << "err read im " << jpeg_path << endl;
    } else {
        cout << "img size (" << jim->width() << " * " << jim->height() << ")" << endl;
        image_io::show_image(jim, "Jpeg Image");
        image_io::write(jim, jpeg_write_path, IMAGE_FMT_JPEG);
    }
    */
    /*buff = im.buffer();
    for (int i = 0; i < im.width() * im.height(); ++i)
    {
        buff[i] = buff[i].grey(255 - buff[i].grey());
    }*/
    // image_io::show_image(im);
    getch();
}

void test2() {
    auto shit = image_io::read("/home/spes/Resource/res/yuutsu.jpg"); // 165x177
    auto im = make_shared<image_t>();
    im->init(600, 600);
    im->set_pixels(shit, -100, -100);
    im->set_pixels(shit, 100, 100);
    im->set_pixels(shit, 500, 500);
    image_io::write(im, "/home/spes/temp/image_render.jpg", IMAGE_FMT_JPEG);

    auto sub1 = shit->get_pixels(0, 0, 100, 100);
    image_io::write(sub1, "/home/spes/temp/image_sub1.jpg", IMAGE_FMT_JPEG);
    auto sub2 = shit->get_pixels(-50, -50, 100, 100);
    image_io::write(sub2, "/home/spes/temp/image_sub2.jpg", IMAGE_FMT_JPEG);
    auto sub3 = shit->get_pixels(0, 0, 300, 300);
    image_io::write(sub3, "/home/spes/temp/image_sub3.jpg", IMAGE_FMT_JPEG);
}

void test_hsv() {
    printf("hsv size: %ld\n", sizeof(hsv_t));
    auto    shit = image_io::read("/home/spes/Resource/res/yuutsu.jpg"); // 165x177
    color_t first = shit->buffer()[0];
    auto    hsv = to_hsv(first);
    auto    buff = image_transform(shit, IMP_FMT_HSV);
    hsv_t*  hsvbuff = (hsv_t*)buff->buff;
    auto    hsvfirst = hsvbuff[0];
    auto    c = from_hsv(hsvfirst);
    printf("equals: %d, hsv first: (%.2f, %.2f, %.2f), to hsv(%.2f, %.2f, %.2f)\n",
           (c == first), hsvfirst.h, hsvfirst.s, hsvfirst.v,
           hsv.h, hsv.s, hsv.v);
    cout << "origin c: " << first << " reved: " << c << endl;
    auto rev = image_transform(buff);
    image_io::write(rev, "/home/spes/temp/rev_hsv.jpg", IMAGE_FMT_JPEG);
}

/* render transparent img */
void test3() {
    auto sakura = image_io::read("/home/spes/temp/sakura512.png");
    auto base = image_io::read("/home/spes/temp/avatar.png");
    base->draw_img(sakura, 0, 0);
    image_io::write(base, "/home/spes/temp/alpha_test.png", IMAGE_FMT_PNG);
}

int main(int argc, char* argv[]) {
    test1();
    // test2();
    // test_hsv();
    // test3();
    return 0;
}
