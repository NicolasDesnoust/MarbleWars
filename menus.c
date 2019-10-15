#include <gtk/gtk.h>
#include <string.h>
#include "curve.h"
#include "util.h"
#include "mydata.h"
#include "drawings.h"
#include "menus.h"


void on_item_quit_activate (GtkMenuItem *menuitem, gpointer data)
{
	Mydata *my = get_mydata(data);
	printf ("Closing window and app\n");
	gtk_widget_destroy(my->window);
}

void on_item_edit_activate (GtkCheckMenuItem *check_menu_item, gpointer data)
{
	Mydata *my = get_mydata(data);
	my->show_edit = gtk_check_menu_item_get_active (check_menu_item);

	if (my->show_edit)
	{
		gtk_widget_set_size_request (my->window, my->win_width + 235, my->win_height);
		gtk_widget_show (my->frame1);
		my->game.state = GS_PAUSE;
		gtk_spin_button_set_range (GTK_SPIN_BUTTON (my->game.load_level_spin), 0, my->game.level_list.level_count-1);
		gtk_spin_button_set_range (GTK_SPIN_BUTTON (my->game.save_level_spin), 0, my->game.level_list.level_count);
		gtk_spin_button_set_range (GTK_SPIN_BUTTON (my->game.delete_level_spin), 0, my->game.level_list.level_count-1);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (my->game.change_background_spin), my->game.level_list.levels[my->game.current_level].background);
		refresh_area (my->area1);
	}	
	else
	{
		gtk_widget_set_size_request (my->window, my->win_width, my->win_height);
		gtk_widget_hide (my->frame1);
		reset_game (&my->game);
	}
}

void on_item_about_activate (GtkMenuItem *menuitem, gpointer data)
{
	Mydata *my = get_mydata(data);
	char *auteurs[] = {"Nicolas DESNOUST <desnoust.nicolas451@gmail.com>",NULL};

	gtk_show_about_dialog (NULL, "program-name", my->title, "version", "2.4", "website", "http://j.mp/optigra", "authors", auteurs, "logo-icon-name", "face-laugh", NULL);
}

void menus_init (gpointer data)
{
	GtkWidget *item_game, *sub_game, *item_quit;
	GtkWidget *item_level, *sub_level, *item_edit;
	GtkWidget *item_help, *sub_help, *item_about;

	Mydata *my = get_mydata(data);

	g_object_set (gtk_settings_get_default(), "gtk-shell-shows-menubar", FALSE, NULL);	
	my->menu_bar = gtk_menu_bar_new ();

	item_game = gtk_menu_item_new_with_label ("Game");
	gtk_menu_shell_append (GTK_MENU_SHELL(my->menu_bar), item_game);

	item_level = gtk_menu_item_new_with_label ("Level");
	gtk_menu_shell_append (GTK_MENU_SHELL(my->menu_bar), item_level);

	item_help = gtk_menu_item_new_with_label ("Help");
	gtk_menu_shell_append (GTK_MENU_SHELL(my->menu_bar), item_help);

	sub_game = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(item_game), sub_game);

	item_quit = gtk_menu_item_new_with_label ("Quit");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_game), item_quit);

	sub_level = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(item_level), sub_level);

	item_edit = gtk_check_menu_item_new_with_label ("Edit");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_level), item_edit);

	sub_help = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(item_help), sub_help);

	item_about = gtk_menu_item_new_with_label ("About");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_help), item_about);

	g_signal_connect (item_quit, "activate", G_CALLBACK(on_item_quit_activate), my);
	g_signal_connect (item_edit, "activate", G_CALLBACK(on_item_edit_activate), my);
	g_signal_connect (item_about, "activate", G_CALLBACK(on_item_about_activate), my);
}
