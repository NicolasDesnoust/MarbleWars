#include <gtk/gtk.h>
#include <locale.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "curve.h"
#include "util.h"
#include "mydata.h"
#include "drawings.h"
#include "menus.h"
#include "gui.h"


void on_app_activate (GtkApplication* app, gpointer user_data)
{
	Mydata *my = get_mydata(user_data);

	window_init (app, my);
	sub_win_init (my);
	menus_init (my);
	area1_init (my);
	editing_init (my);
	layout_init (my);

	g_timeout_add (20, on_timeout1, my);
	gtk_widget_show_all (my->window);
	gtk_widget_hide (my->frame1);
}

int main (int argc, char *argv[])
{
	setlocale(LC_ALL, getenv ("LANG"));
	Mydata my;
	init_mydata (&my);

	GtkApplication *app;
	int status;

	app = gtk_application_new (NULL, G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK(on_app_activate), &my);
	status = g_application_run (G_APPLICATION(app), argc, argv);
	g_object_unref (app);

	return status;
}
