#include "mydata.h"

void init_mydata (Mydata *my)
{
	my->magic = MYDATA_MAGIC;
	my->title = "MARBLE WARS";
	my->win_height = 528;
	my->win_width = 800;

	GdkPixbuf *tmp1 = gdk_pixbuf_new_from_file ("./resources/menu.png", NULL);
	int width = gdk_pixbuf_get_width(tmp1),
	    height = gdk_pixbuf_get_height(tmp1);
	my->pixbuf1 = gdk_pixbuf_scale_simple (tmp1, 0.3125 * width, 0.3125 * height, GDK_INTERP_BILINEAR);
	tmp1 = gdk_pixbuf_new_from_file ("./resources/heart.png", NULL);
	width = gdk_pixbuf_get_width(tmp1),
	height = gdk_pixbuf_get_height(tmp1);
	my->heart[0] = gdk_pixbuf_scale_simple (tmp1, 0.36 * width, 0.36 * height, GDK_INTERP_BILINEAR);
	tmp1 = gdk_pixbuf_new_from_file ("./resources/heart2.png", NULL);
	my->heart[1] = gdk_pixbuf_scale_simple (tmp1, 0.36 * width, 0.36 * height, GDK_INTERP_BILINEAR);

	char str[30] = "";
	for (int k = 0; k < 14; k++)
	{
		sprintf(str, "./backgrounds/%.2d.png", k);
		tmp1 = gdk_pixbuf_new_from_file (str, NULL);
		width = gdk_pixbuf_get_width(tmp1),
	  	height = gdk_pixbuf_get_height(tmp1);
		my->background[k] = gdk_pixbuf_scale_simple (tmp1, 0.3125 * width, 0.3125 * height, GDK_INTERP_BILINEAR);
	}
	my->image_canon = cairo_image_surface_create_from_png ("./resources/canon.png");
	g_object_unref (tmp1);
	init_game (&my->game);

	for (int i = 0; i < MARBLE_COLORS; i++)
	{
		sprintf(str, "./resources/%.2d.png", i);
		my->image_billes[i] = cairo_image_surface_create_from_png (str);
	}
	for (int j = 0; j < 9; j++)
	{
		sprintf(str, "./explode/ex%.2d.png", j);
		my->explode_animation[j] = cairo_image_surface_create_from_png (str);
	}
	if (my->image_billes[0] != NULL)
		my->game.marble_diameter = cairo_image_surface_get_width (my->image_billes[0]) * MARBLE_SIZE;

	my->click_n = 0; my->click_x = 0; my->click_y = 0;
	my->show_edit = FALSE;
	my->edit_mode = EDIT_ADD_CURVE;
}

Mydata *get_mydata (gpointer data)
{
	if (data == NULL) {
		fprintf (stderr, "get_mydata: NULL data\n"); return NULL;
	}
	if (((Mydata *)data)->magic != MYDATA_MAGIC) {
		fprintf (stderr, "get_mydata: bad magic number\n"); return NULL;
	}
	return data;
}

void set_edit_mode (Mydata *my, int mode)
{
	if (mode <= EDIT_NONE || mode >= EDIT_LAST)
		return;

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(my->edit_radios[mode]), TRUE);
}
