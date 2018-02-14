#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>

#include <X11/Xlib.h>

struct Rect
{
	int x, y;
	int w, h;
};

int main()
{
	Display* d;
	Window   w;
	XEvent   e;
	GC       ctx;
	int      screen;

	d = XOpenDisplay(0);
	if(!d)
	{
		printf("Cannot open display\n");
		return -1;
	}

	Rect wnd{50, 50, 640, 480};

	screen = DefaultScreen(d);
	auto visual = DefaultVisual(d, screen); 

	w = XCreateSimpleWindow(d, RootWindow(d, screen), wnd.x, wnd.y, wnd.w, wnd.h, 1, BlackPixel(d, screen), WhitePixel(d, screen));
	XSelectInput(d, w, ExposureMask | KeyPressMask | StructureNotifyMask | ResizeRedirectMask);
	XMapWindow(d, w);
	ctx = DefaultGC(d, screen);

	auto rgb = [](unsigned long r, unsigned long g, unsigned long b)
	{
		return ((r*256)+g)*256+b;
	};

	Font font = XLoadFont(d, "fixed");
	std::string msg = "Hello, World!";
	
	auto redraw = [&]()
	{
		XSetForeground(d, ctx, rgb(192, 192, 192));    //Set grey color
		XFillRectangle(d, w, ctx, 0, 0, wnd.w, wnd.h); //fill window area with grey rectangle
		XSetForeground(d, ctx, rgb(0, 128, 255));      //Set blue color
		XSetFont(d, ctx, font);                        //Set active font
		XDrawString(d, w, ctx, 20, 20, msg.data(), msg.size()); //Draw string "Hello, World!"

		XSetForeground(d, ctx, rgb(255, 0, 0)); //Set red color
		XDrawLine(d, w, ctx, 20, 40, 100, 40); //Draw line

		XFlush(d);//Send out all drawing commands
	};

	while(1)
	{
		XNextEvent(d, &e);
		if(e.type == Expose){ redraw(); }

		if(e.type == ConfigureNotify)
		{
			XConfigureEvent xce = e.xconfigure;
			if (xce.width != wnd.w || xce.height != wnd.h)//on resize adjust draw area and update 
			{
				wnd.w = xce.width;
				wnd.h = xce.height;
				redraw();
			}
		}
		
		if(e.type == KeyPress){ break; }//on key press, quit
	}

	XCloseDisplay(d);
	return 0;
}