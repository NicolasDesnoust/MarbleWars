/* font.h
 *
 * Edouard.Thiel@lif.univ-mrs.fr - 18/02/2016
 *
 * This program is free software under the terms of the
 * GNU Lesser General Public License (LGPL) version 2.1.
*/

#ifndef FONT_H
#define FONT_H

#include <gtk/gtk.h>

typedef enum { 
  FONT_BEGIN, 
  FONT_TL, FONT_TC, FONT_TR,     /* Top    Left, Top    Center, Top    Right */
  FONT_ML, FONT_MC, FONT_MR,     /* Middle Left, Middle Center, Middle Right */
  FONT_BL, FONT_BC, FONT_BR,     /* Bottom Left, Bottom Center, Bottom Right */
  FONT_END
} Font_align;


void font_set_name (PangoLayout *layout, char const *desc);
void font_set_size (PangoLayout *layout, int size);
int font_set_position (cairo_t *cr, PangoLayout *layout, Font_align align, 
    double x, double y);
void font_draw_text (cairo_t *cr, PangoLayout *layout,
    Font_align align, double x, double y, const char *format, ...);
void font_draw_markup (cairo_t *cr, PangoLayout *layout,
    Font_align align, double x, double y, const char *format, ...);

#endif /* FONT_H */

