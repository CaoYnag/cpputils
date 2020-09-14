#include "imageio.h"
#include "io_png.h"
#include "io_jpeg.h"
#include "io_gif.h"
#include "math.h"
#include <functional>
#include <thread>
#include <map>
#ifdef WIN32
#include <Windows.h>
#include <gl/glew.h>
#endif
#ifdef __unix__
#include <xcb/xcb.h>	// screen size
#include <gtk/gtk.h>
#endif
using namespace std;
using namespace spes;
using namespace spes::math;
using namespace spes::image;


namespace spes::image::io
{
	ImageViewer::ImageViewer(image_t im, std::string tt) : _im(move(im)), _tt(tt)
	{
		_sz = rect_adjust(image_io::screen_size(), { (f32)im.width(), (f32)im.height() });
	}
	ImageViewer::~ImageViewer()
	{}

#ifdef WIN32
	LRESULT CALLBACK IV_Wnd_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	class WindowsImageViewer : public ImageViewer
	{
	public:
		static map<HWND, WindowsImageViewer*> INSTS;
	protected:
		HWND _hwnd;
		HDC _hdc;
		HGLRC _rc;
		u32 _tex;
	public:
		WindowsImageViewer(image_t& im, string tt) : ImageViewer(im, tt)
		{
		}
		virtual ~WindowsImageViewer()
		{
			ReleaseDC(_hwnd, _hdc);
			INSTS[_hwnd] = nullptr;
		}

		void init_wnd()
		{
			WNDCLASS wc = { 0 };
			const char* CLS_NAME = "ImageViewer";;
			wc.style = NULL;
			wc.hIcon = NULL;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = NULL;
			wc.lpfnWndProc = IV_Wnd_Proc;
			wc.hbrBackground = NULL;
			wc.lpszMenuName = NULL;
			wc.lpszClassName = CLS_NAME;
			wc.hCursor = NULL;

			RegisterClass(&wc);

			_hwnd = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW, CLS_NAME, _tt.c_str(), WS_CAPTION | WS_SYSMENU | WS_SIZEBOX, CW_USEDEFAULT,
				CW_USEDEFAULT, (u32)_sz.w, (u32)_sz.h, NULL, NULL, NULL, NULL);
			//_hwnd = CreateWindowExA(WS_EX_TOOLWINDOW, CLS_NAME, _tt.c_str(), WS_POPUP | WS_CAPTION, CW_USEDEFAULT,
			//	CW_USEDEFAULT, _sz.w, _sz.h, NULL, NULL, NULL, NULL);
			if (_hwnd == NULL)return;
			INSTS[_hwnd] = this;

			_hdc = GetDC(_hwnd);
			ShowWindow(_hwnd, SW_SHOW);
		}

		void init_gl()
		{
			int pixelFormat;

			PIXELFORMATDESCRIPTOR pfd =
			{
				sizeof(PIXELFORMATDESCRIPTOR),  // size
				1,                          // version
				PFD_SUPPORT_OPENGL |        // OpenGL window
				PFD_DRAW_TO_WINDOW |        // render to window
				PFD_DOUBLEBUFFER,           // support double-buffering
				PFD_TYPE_RGBA,              // color type
				32,                         // prefered color depth
				0, 0, 0, 0, 0, 0,           // color bits (ignored)
				0,                          // no alpha buffer
				0,                          // alpha bits (ignored)
				0,                          // no accumulation buffer
				0, 0, 0, 0,                 // accum bits (ignored)
				16,                         // depth buffer
				0,                          // no stencil buffer
				0,                          // no auxiliary buffers
				PFD_MAIN_PLANE,             // main layer
				0,                          // reserved
				0, 0, 0,                    // no layer, visible, damage masks
			};
			pixelFormat = ChoosePixelFormat(_hdc, &pfd);
			SetPixelFormat(_hdc, pixelFormat, &pfd);

			glewExperimental = GL_TRUE;
			glewInit();

			_rc = wglCreateContext(_hdc);
			wglMakeCurrent(_hdc, _rc);

			glClearColor(1.0, 1.0, 1.0, .0);
			glShadeModel(GL_SMOOTH);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
			glEnable(GL_TEXTURE_2D);

			glGenTextures(1, &_tex);

			gluOrtho2D(-1, 1, -1, 1);
			//glOrtho(0, 1, 0, 1, 0, 1);
			glViewport(0, 0, _sz.w, _sz.h);
		}

		virtual void show()
		{
			MSG msg;
			//while (GetMessageA(&msg, NULL, 0, 0))
			while(true)
			{
				PeekMessageA(&msg, _hwnd, 0, 0, PM_REMOVE);
				display();
				SwapBuffers(_hdc);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		virtual void update()
		{
			UpdateWindow(_hwnd);
		}

		virtual void init()
		{
			init_wnd();
			init_gl();
		}

		void display()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glBindTexture(GL_TEXTURE_2D, _tex);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _im.width(), _im.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, _im.buffer());

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			glEnable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
			glTexCoord2f(.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, .0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, .0f);
			glTexCoord2f(1.0f, .0f);
			glVertex3f(1.0f, 1.0f, .0f);
			glTexCoord2f(.0f, .0f);
			glVertex3f(-1.0f, 1.0f, .0f);
			glEnd();
			glDisable(GL_TEXTURE_2D);
		}


		void on_resize(u32 w, u32 h)
		{
			glViewport(0, 0, w, h);
		}
		void resize(u32 w, u32 h)
		{
			RECT rect;
			GetWindowRect(_hwnd, &rect);
			MoveWindow(_hwnd, rect.left, rect.top, w, h, true);
		}

		virtual void best_size()
		{
			resize(_sz.w, _sz.h);
		}
		virtual void origin_size()
		{
			resize(_im.width(), _im.height());
		}
	};
	map<HWND, WindowsImageViewer*> WindowsImageViewer::INSTS;

	LRESULT CALLBACK IV_Wnd_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_SIZE:
		{
			u32 h = HIWORD(lParam);
			u32 w = LOWORD(lParam);
			WindowsImageViewer::INSTS[hWnd]->on_resize(w, h);
		}break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_KEYUP:
		{
			int key = wParam;
			switch (key)
			{
			case 'O':
				WindowsImageViewer::INSTS[hWnd]->origin_size();
				break;
			case 'B':
				WindowsImageViewer::INSTS[hWnd]->best_size();
				break;
			default:
				break;
			}
		}break;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
#endif
#ifdef __unix__
	class UnixImageViewer : public ImageViewer
	{
	public:
		static bool INITED;
	protected:
        GtkWidget* _wnd;
        GtkWidget* _img;
	public:
		UnixImageViewer(image_t& im, string tt) : ImageViewer(im, tt)
		{
		}
		virtual ~UnixImageViewer()
		{
		}

		/* do some init stuff */
		virtual void init()
		{
            if(INITED) return;
            g_thread_init(0);
            gdk_threads_init();
            gtk_init(0, 0);
            INITED = true;
		}
		/* open window and show image */
		virtual void show()
		{
            gdk_threads_enter();
            _wnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
            gtk_window_set_position(GTK_WINDOW(_wnd), GTK_WIN_POS_CENTER);
            gtk_window_set_default_size(GTK_WINDOW(_wnd), 100, 100);
            gtk_window_set_title(GTK_WINDOW(_wnd), _tt.c_str());
            gtk_window_set_resizable(GTK_WINDOW(_wnd), FALSE);

            gtk_container_set_border_width(GTK_CONTAINER(_wnd), 2);

            auto pix = gdk_pixbuf_new_from_data((const guchar*)_im.buffer(), GDK_COLORSPACE_RGB, true, 8, _im.width(), _im.height(), _im.width() * 4, NULL, NULL);
            _img = gtk_image_new_from_pixbuf(pix);
            gtk_container_add(GTK_CONTAINER(_wnd), _img);


            g_signal_connect_swapped(G_OBJECT(_wnd), "destroy",
                                     G_CALLBACK(gtk_main_quit), G_OBJECT(_wnd));
            gtk_widget_show_all(_wnd);
            gtk_main();
            gdk_threads_leave();
		}
		/* refresh window */
		virtual void update(){}

		virtual void resize(u32 w, u32 h){}
		virtual void best_size(){}
		virtual void origin_size(){}
	};
	bool UnixImageViewer::INITED = false;
#endif

	size2d image_io::screen_size()
	{
#ifdef WIN32
		RECT rect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, (void*)&rect, 0);
		return { rect.right - rect.left + .0f, rect.bottom - rect.top - GetSystemMetrics(SM_CYCAPTION) + .0f };
#endif
#ifdef __unix__
		/* Open the connection to the X server. Use the DISPLAY environment variable */
		int i, screenNum;
		xcb_connection_t *connection = xcb_connect (NULL, &screenNum);
		/* Get the screen whose number is screenNum */ 
		const xcb_setup_t *setup = xcb_get_setup (connection);
		xcb_screen_iterator_t iter = xcb_setup_roots_iterator (setup);  
		// we want the screen at index screenNum of the iterator
		for (i = 0; i < screenNum; ++i) 
			xcb_screen_next (&iter);

		xcb_screen_t *screen = iter.data;
		return {screen->width_in_pixels, screen->height_in_pixels};
#endif
	}

	ImageViewer* image_io::show_image(image_t& im, string title)
	{
		ImageViewer* viewer = nullptr;
#ifdef WIN32
		viewer = new WindowsImageViewer(im, title);
#endif
#ifdef __unix__
		viewer = new UnixImageViewer(im, title);
#endif
		thread th([=]()->void
		{
			viewer->init();
			viewer->show();
		});
		th.detach();
		return viewer;
	}

	u32 image_format(const char* path)
	{
		constexpr u32 BYTES_TO_CHECK = 4;
		constexpr u32 BYTES_TO_READ = BYTES_TO_CHECK + 1;
		char buff[BYTES_TO_READ];
		FILE* fp = fopen(path, "r");
		if (!fp) return IMAGE_FMT_UNKNOWN;
		fgets(buff, BYTES_TO_READ, fp);
		fclose(fp);
		if (png_sig_check(buff, BYTES_TO_CHECK))
			return IMAGE_FMT_PNG;
		if (jpeg_sig_check(buff, BYTES_TO_CHECK))
			return IMAGE_FMT_JPEG;

		return IMAGE_FMT_UNKNOWN;
	}

	image_t image_io::read(const char* path)
	{
		switch (image_format(path))
		{
		case IMAGE_FMT_JPG:
		case IMAGE_FMT_JPEG:
			return read_jpeg(path);
		case IMAGE_FMT_PNG:
			return read_png(path);
		case IMAGE_FMT_BMP:
		default: break;
		}
		return image_t();
	}

	void image_io::write(image_t& im, const char* path, u32 fmt)
	{
		switch (fmt)
		{
		case IMAGE_FMT_PNG:
			write_png(im, path); break;
		case IMAGE_FMT_JPG:
		case IMAGE_FMT_JPEG:
			write_jpeg(im, path); break;
		default:
			break;
		}
	}

	std::vector<image_t> read_anim(const char* path)
	{
		return {};
	}
	void write_anim(std::vector<image_t>& ims, const char* path, u32 delay, u32 fmt)
	{
		write_gif(ims, path, delay);
	}
}