#ifndef MYDATA_H
#define MYDATA_H

#include <gtk/gtk.h>
#include "curve.h"
#include "game.h"


#define MYDATA_MAGIC 0x46EA7E05

typedef enum {EDIT_NONE, EDIT_ADD_CURVE, EDIT_MOVE_CURVE, EDIT_REMOVE_CURVE, EDIT_ADD_CONTROL,
	EDIT_MOVE_CONTROL, EDIT_REMOVE_CONTROL, EDIT_MOVE_CANON, EDIT_LAST} Edit;

typedef struct {
	unsigned int magic;
	Game game;
	GtkWidget *window, *sub_win, *entry, *vbox1, *hbox1, *scale1, *area1, *menu_bar, *scroll, *frame1, *edit_radios[EDIT_LAST];
	GdkPixbuf *pixbuf1;
	GdkPixbuf *background[14], *heart[2];
	cairo_surface_t *image_canon;
	cairo_surface_t *image_billes[MARBLE_COLORS];
	cairo_surface_t *explode_animation[9];
	char *title;
	int win_width, win_height, click_n, show_edit, edit_mode;
	double click_x, click_y, last_x, last_y;	
} Mydata;

void init_mydata (Mydata *my);

Mydata *get_mydata (gpointer data);

void set_edit_mode (Mydata *my, int mode);

#endif /* MYDATA_H */
