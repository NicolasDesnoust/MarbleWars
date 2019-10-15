#ifndef DRAWINGS_H
#define DRAWINGS_H

#include <gtk/gtk.h>
#include <stdio.h>
#include "util.h"
#include "curve.h"
#include "mydata.h"
#include "font.h"
#include "math.h"

void draw_control_polygons (cairo_t *cr, Curve_infos *ci);

void area1_init (gpointer data);

void draw_bezier_polygons_open (cairo_t *cr, Curve_infos *ci);

void draw_bezier_curves_open (cairo_t *cr, Curve_infos *ci, double theta);

gboolean on_timeout1 (gpointer data);

void update_area1_with_transforms (Mydata *my);

#endif /* DRAWINGS_H */
