#ifndef CURVE_H
#define CURVE_H

#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#define CONTROL_MAX 100
#define CURVE_MAX  200

typedef struct {
	double x, y;
} Control;

typedef struct {
	int control_count;
	Control controls[CONTROL_MAX];
	double shift_x, shift_y;
} Curve;

typedef struct {
	int curve_count;
	Curve curves[CURVE_MAX];
} Curve_list;

typedef struct {
	Curve_list curve_list;
	int current_curve, current_control;
} Curve_infos;

void init_curve_infos (Curve_infos *ci);

int remove_curve (Curve_infos *ci);

int remove_control (Curve_infos *ci);

int add_curve (Curve_infos *ci);

int add_control (Curve_infos *ci, double x, double y);

int find_control (Curve_infos *ci, double x, double y);

int move_control (Curve_infos *ci, double dx, double dy);

int move_curve (Curve_infos *ci, double dx, double dy);

void move_shift (Curve_infos *ci, double dx, double dy);

void reset_shift (Curve_infos *ci);

void compute_bezier_points_open (Curve *curve, int i, Control bez_points[4]);

void compute_bezier_points_close (Curve *curve, int i, Control bez_points[4]);

double compute_bezier_cubic (double b[4], double t);

void compute_bezier_points_prolong_first (Curve *curve, Control bez_points[4]);

void compute_bezier_points_prolong_last (Curve *curve, Control bez_points[4]);

void sample_bezier_curve (Control bez_points[4], double theta,
  double sx[], double sy[], int *ind, int ind_max, int is_first);

double compute_distant_point_forward (double sx[], double sy[], double tA, int tmax,
  double dist, double *xB, double *yB);

double compute_distant_point_backward (double sx[], double sy[], double tA, int tmax,
  double dist, double *xB, double *yB);

#endif /* CURVE_H */
