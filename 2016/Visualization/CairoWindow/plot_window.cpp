#include <cmath>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <cairo.h>

#include <cairo-xlib.h>
#include <vector>
#include <sstream>
#include <algorithm>

struct rect
{
	double x0, y0, x1, y1;
};

auto rescale = [](auto old, auto old_min, auto old_max, auto new_min, auto new_max)
{
    double r = (double)(old - old_min) / (double)(old_max - old_min);
    return new_min + r * (new_max - new_min);
};

enum align{left_top=0, center=1, right_bottom=2};

auto plot_function = [](auto context, auto rect, auto x0, auto x1, auto f)
{
  std::vector<double> ys( (int)(rect.x1-rect.x0) );
  std::generate(ys.begin(), ys.end(),
  [=, i=0, d=(x1-x0)/(ys.size()-1)]()mutable
  {
    auto y = f(x0+i*d);
	++i;
    return y;
  });

  auto minmax = std::minmax_element(ys.begin(), ys.end());

  for(int i=0; i<(int)ys.size(); ++i)
  {
      auto x = rescale(i,     0, ys.size()-1, rect.x0, rect.x1);
      auto y = rescale(ys[i], *minmax.first, *minmax.second, rect.y0, rect.y1);
      if(i==0){ cairo_move_to(context, x, y); }
      else    { cairo_line_to(context, x, y); }
  }
  cairo_stroke (context);

  cairo_set_source_rgb(context, 0, 0, 0);
  cairo_select_font_face(context, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(context, 14.0);

  cairo_font_extents_t fe;
  cairo_font_extents(context, &fe);

  auto center_label = [&](auto str, auto x, auto y, align ax, align ay)
  {
    cairo_text_extents_t te;
    cairo_text_extents (context, str.c_str(), &te);

	auto xx = x - te.width * ((int)ax / 2.0);
	auto yy = y - te.height* ((int)ay / 2.0);
    
    cairo_move_to(context, xx, yy);
    cairo_show_text(context, str.c_str());
  };

  auto stringify = [](auto x)
  {
    std::stringstream ss;
    ss << x;
    return ss.str();
  };
  
  center_label(stringify(*minmax.first),                      rect.x0*0.95, rect.y1,               align::right_bottom, align::center);
  center_label(stringify(0.5*(*minmax.second+*minmax.first)), rect.x0*0.95, 0.5*(rect.y0+rect.y1), align::right_bottom, align::center);
  center_label(stringify(*minmax.second),                     rect.x0*0.95, rect.y0,               align::right_bottom, align::center);

  center_label(stringify(x0),          rect.x0,               rect.y1*1.05, align::center, align::left_top);
  center_label(stringify(0.5*(x0+x1)), 0.5*(rect.x0+rect.x1), rect.y1*1.05, align::center, align::left_top);
  center_label(stringify(x1),          rect.x1,               rect.y1*1.05, align::center, align::left_top);
  
};

//----------------------------

int cairo_check_event(Display* dsp, int block)
{
   char keybuf[8];
   KeySym key;
   XEvent e;

   for (;;)
   {
      if(block || XPending(dsp)){ XNextEvent(dsp, &e); }
      else{ return 0; }

      if(e.type == KeyPress)
      {
		XLookupString(&e.xkey, keybuf, sizeof(keybuf), &key, NULL);
        return key;
      }
   }
}

cairo_surface_t *cairo_create_x11_surface0(int x, int y)
{
   Display *dsp;
   Drawable da;
   int screen;
   cairo_surface_t *sfc;

   if ((dsp = XOpenDisplay(NULL)) == NULL)
      exit(1);
   screen = DefaultScreen(dsp);
   da = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp), 0, 0, x, y, 0, 0, 0);
   XSelectInput(dsp, da, ButtonPressMask | KeyPressMask);
   XMapWindow(dsp, da);

   sfc = cairo_xlib_surface_create(dsp, da, DefaultVisual(dsp, screen), x, y);
   cairo_xlib_surface_set_size(sfc, x, y);

   return sfc;
}

int main(void)
{
  cairo_surface_t* surface;
  cairo_t*         cr;

  int x = 640;
  int y = 480;

  Display *dsp;
  Drawable da;
  int screen;

  if ((dsp = XOpenDisplay(NULL)) == NULL)
     exit(1);
  screen = DefaultScreen(dsp);
  da = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp), 0, 0, x, y, 0, 0, 0);
  XSelectInput(dsp, da, ButtonPressMask | KeyPressMask);
  XMapWindow(dsp, da);

  surface = cairo_xlib_surface_create(dsp, da, DefaultVisual(dsp, screen), x, y);
  cairo_xlib_surface_set_size(surface, x, y);
  cr = cairo_create(surface);

  //Start rendering:
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint(cr);

  cairo_set_line_width (cr, 5.0);
  cairo_set_source_rgba (cr, 0, 1, 0, 1);

  auto plot_rect = rect{ 100, 100, 550, 350 };
  plot_function(cr, plot_rect, -5.0, 5.0, [](auto x){ return exp(-x*x/4.0)*cos(x*4.5); });

  cairo_set_line_width (cr, 2.0);
  cairo_set_source_rgba (cr, 0, 0, 0, 1);
  cairo_rectangle(cr, plot_rect.x0, plot_rect.y0, plot_rect.x1-plot_rect.x0, plot_rect.y1-plot_rect.y0);
  cairo_stroke(cr);

  cairo_destroy(cr);

  cairo_check_event(dsp, 1);

  cairo_surface_destroy(surface);
  XCloseDisplay(dsp);

  return 0;
}


//---------------------

