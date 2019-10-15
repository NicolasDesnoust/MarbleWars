#include <gtk/gtk.h>
#include "util.h"

void refresh_area (GtkWidget *area)
{
	GdkWindow *win = gtk_widget_get_window (area);
	if (win == NULL) return; 
	gdk_window_invalidate_rect (win, NULL, FALSE);
}
