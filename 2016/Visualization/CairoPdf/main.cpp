#include <cmath>
#include <cairo.h>
#include <cairo/cairo-pdf.h>

int main(void)
{
  cairo_surface_t *surface;
  cairo_t *cr;

  surface = cairo_pdf_surface_create("image2.pdf", 640, 480);
  cr = cairo_create(surface);

  cairo_set_line_width (cr, 10.0);
  cairo_set_source_rgba (cr, 1, 0, 0, 1);

  cairo_move_to(cr, 100, 100);
  cairo_line_to (cr, 200, 100);
  cairo_line_to (cr, 200, 200);
  cairo_line_to (cr, 100, 200);
  cairo_line_to (cr, 100, 100);
  cairo_stroke (cr);

  cairo_set_source_rgba (cr, 0, 0, 1, 1);
  cairo_arc (cr, 300, 100, 90.0, 0, 2*M_PI);
  cairo_fill (cr);

  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 40.0);
  cairo_move_to(cr, 10.0, 300.0);
  cairo_show_text(cr, "Hello Cairo!");

  cairo_destroy(cr);
  cairo_surface_destroy(surface);

  return 0;
}