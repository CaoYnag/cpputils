#include <gtk/gtk.h>
#include <thread>
#include <spes/imageio.h>
using namespace std;
using namespace spes::image::io;

class GtkImageViewer
{
    GtkWidget* _wnd;
    GtkWidget* _img;
public:

    void init(const char* name, image_t& im)
    {
        _wnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_position(GTK_WINDOW(_wnd), GTK_WIN_POS_CENTER);
        gtk_window_set_default_size(GTK_WINDOW(_wnd), 230, 150);
        gtk_window_set_title(GTK_WINDOW(_wnd), name);
        gtk_window_set_resizable(GTK_WINDOW(_wnd), FALSE);

        gtk_container_set_border_width(GTK_CONTAINER(_wnd), 2);

        //_img = gtk_image_new_from_file(file);
        //gtk_image_new_from_image();
        auto pix = gdk_pixbuf_new_from_data((const guchar*)im.buffer(), GDK_COLORSPACE_RGB, true, 8, im.width(), im.height(), im.width() * 4, NULL, NULL);
        _img = gtk_image_new_from_pixbuf(pix);
        gtk_container_add(GTK_CONTAINER(_wnd), _img);


        g_signal_connect_swapped(G_OBJECT(_wnd), "destroy",
                                 G_CALLBACK(gtk_main_quit), G_OBJECT(_wnd));
    }

    void show()
    {
        gtk_widget_show_all(_wnd);
        gtk_main();
    }

    void mainloop()
    {
        thread([](){gtk_main();}).detach();
    }
};

int main( int argc, char *argv[])
{
    g_thread_init(0);
    gdk_threads_init();
    gtk_init(0, 0);


    auto yuutsu = image_io::read("/home/spes/Resource/res/yuutsu.jpg");
    auto shit = image_io::read("/home/spes/Resource/res/shit.jpeg");

    thread([&]()
    {
        gdk_threads_enter();
        GtkImageViewer viewer1;
        viewer1.init("wnd1", yuutsu);
        viewer1.show();
        gdk_threads_leave();
    }).detach();

    thread([&]()
           {
               gdk_threads_enter();
               GtkImageViewer viewer2;
               viewer2.init("wnd2", shit);
               viewer2.show();
               gdk_threads_leave();
           }).detach();

    while(true);
    return 0;
}
