#include <cstdio>
#include <iostream>
#include <spes/image.h>
#include <jpeglib.h>
#include <png.h>
#include <cstring>
#include <spes/imageio.h>
using namespace std;
using namespace spes;
using namespace spes::image;
using namespace spes::image::io;

bool jpeg_sig_check(s8* buff, u32 l)
{
    constexpr u32 JPEG_SIG_LEN = 4;
    // TODO: any other choice to init a char array?
    constexpr s8 JPEG_SIG[] = {(s8)0xff, (s8)0xd8, (s8)0xff, (s8)0xe0};
    u32 len = l < JPEG_SIG_LEN ? l : JPEG_SIG_LEN;
    for (int i = 0; i < 4; ++i)
        if (buff[i] != JPEG_SIG[i]) return false;
    return true;
}
struct my_error_mgr {
    jpeg_error_mgr pub;
    jmp_buf env;
};

typedef struct my_error_mgr* my_error_ptr;
void my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->env, 1);
}

shared_ptr<image_t> read_jpeg(FILE* fp)
{
    auto im = make_shared<image_t>();
    if(!fp)
    {
        fprintf(stderr, "invalid fp in %s", __func__);
        return im;
    }
    jpeg_decompress_struct cinfo;
    my_error_mgr jerr;

    JSAMPARRAY buffer;
    int row_stride;
    jmp_buf env;


    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.env)) {
        jpeg_destroy_decompress(&cinfo);
        return im;
    }
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, fp);

    (void)jpeg_read_header(&cinfo, TRUE);
    auto buff = make_shared<im_buff>(cinfo.image_width, cinfo.image_height, IMP_FMT_RGB);

    (void)jpeg_start_decompress(&cinfo);
    row_stride = cinfo.output_width * cinfo.output_components;

    buffer = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

    while (cinfo.output_scanline < cinfo.output_height) {
        u8* dst = buff->buff + cinfo.output_scanline * row_stride;
        (void)jpeg_read_scanlines(&cinfo, buffer, 1);
        memcpy(dst, buffer[0], row_stride);
    }

    (void)jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return image_transform(buff);
}

void write_jpeg(shared_ptr<image_t> im, FILE* fp, int quality)
{
    if (!fp) {
        fprintf(stderr, "error fp in %s", __func__);
        return;
    }
    auto buff = image_transform(im, IMP_FMT_RGB);

    jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    JSAMPROW row_pointer[1];
    int row_stride;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    jpeg_stdio_dest(&cinfo, fp);

    cinfo.image_width = buff->w;
    cinfo.image_height = buff->h;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;


    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);


    jpeg_start_compress(&cinfo, TRUE);


    row_stride = buff->w * 3;

    while (cinfo.next_scanline < cinfo.image_height) 
    {
        row_pointer[0] = &buff->buff[cinfo.next_scanline * row_stride];
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
}

int main(int argc, char** argv)
{
    FILE* fp = fopen("ac.jpg", "rb+");
    if(!fp)
    {
        cerr << "failed open ac.jpg" << endl;
        return 0;
    }
    auto type = image_format(fp);
    printf("after image_format, now fp pos: %ld\n", ftell(fp));
    if(type != IMAGE_FMT_JPEG)
    {
        printf("bad format: %d\n", type);
        return 0;
    }
    auto im = read_jpeg(fp);
    printf("image size: %dx%d\n", im->width(), im->height());
    fclose(fp);
    fp = fopen("ac2.jpg", "wb");
    if(!fp)
    {
        cerr << "failed open ac2.jpg" << endl;
        return 0;
    }
    write_jpeg(im, fp, 80);
    fclose(fp);
    return 0;
}