#include <cmath>
#include <cairo.h>
#include <cairo/cairo-pdf.h>
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

int main(void)
{
  cairo_surface_t *surface;
  cairo_t *cr;

  //surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 640, 480);
  surface = cairo_pdf_surface_create("plot_best.pdf", 640, 480);
  cr = cairo_create(surface);

  cairo_set_line_width (cr, 5.0);
  cairo_set_source_rgba (cr, 0, 1, 0, 1);

  auto plot_rect = rect{ 100, 100, 550, 350 };
  plot_function(cr, plot_rect, -5.0, 5.0, [](auto x){ return exp(-x*x/4.0)*cos(x*4.5); });

  cairo_set_line_width (cr, 2.0);
  cairo_set_source_rgba (cr, 0, 0, 0, 1);
  cairo_rectangle(cr, plot_rect.x0, plot_rect.y0, plot_rect.x1-plot_rect.x0, plot_rect.y1-plot_rect.y0);
  cairo_stroke(cr);

  cairo_destroy(cr);
  cairo_surface_destroy(surface);

  return 0;
}
