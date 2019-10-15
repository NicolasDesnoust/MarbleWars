#include <gtk/gtk.h>
#include <string.h>
#include "curve.h"
#include "util.h"
#include "mydata.h"
#include "drawings.h"
#include "menus.h"
#include "gui.h"


void on_edit_radio_toggled (GtkWidget *widget, gpointer data)
{
	Mydata *my = get_mydata(data);
	
	my->edit_mode = GPOINTER_TO_INT(g_object_get_data (G_OBJECT(widget), "mode"));
}

void on_button_change_background_clicked (GtkWidget *widget, gpointer data)
{
	Mydata *my = get_mydata(data);

	my->game.level_list.levels[my->game.current_level].background = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (my->game.change_background_spin));
	refresh_area(my->area1);
}

void on_button_load_level_clicked (GtkWidget *widget, gpointer data)
{
	Mydata *my = get_mydata(data);
	int level = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (my->game.load_level_spin));
	load_level (&my->game.level_list, level);

	my->game.current_level = level;
	my->game.canon.cx = my->game.level_list.levels[level].canon_x;
	my->game.canon.cy = my->game.level_list.levels[level].canon_y;
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (my->game.marble_intro_spin), my->game.level_list.levels[level].marbles_intro);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (my->game.marble_total_spin), my->game.level_list.levels[level].marbles_total);
	refresh_area(my->area1);
}

void on_button_save_level_clicked (GtkWidget *widget, gpointer data)
{
	Mydata *my = get_mydata(data);
	FILE* fichier = NULL;
	int current_level = my->game.current_level;
	int level = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (my->game.save_level_spin));
	char levels_path[30] = "./levels/";
	char str[12] = "";
	sprintf(str, "level%.2d.txt", level);
	strcat(levels_path, str);

	fichier = fopen (levels_path, "w");

	if (fichier != NULL)
	{
		if (level == my->game.level_list.level_count)
		{
			my->game.level_list.level_count++;
			gtk_spin_button_set_range (GTK_SPIN_BUTTON (my->game.load_level_spin), 0, my->game.level_list.level_count-1);
			gtk_spin_button_set_range (GTK_SPIN_BUTTON (my->game.save_level_spin), 0, my->game.level_list.level_count);
			gtk_spin_button_set_range (GTK_SPIN_BUTTON (my->game.delete_level_spin), 0, my->game.level_list.level_count-1);		
		}

		fprintf (fichier, "%d\n", my->game.level_list.levels[current_level].background);

		fprintf (fichier, "%lf %lf\n", my->game.level_list.levels[current_level].canon_x, my->game.level_list.levels[current_level].canon_y);

		fprintf (fichier, "%d %d\n", gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (my->game.marble_intro_spin)), gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (my->game.marble_total_spin)));

		fprintf (fichier, "%d\n", my->game.level_list.levels[current_level].curve_infos.curve_list.curve_count);

		my->game.level_list.levels[level].curve_infos.current_curve = -1;
		my->game.level_list.levels[level].curve_infos.current_control = -1;	

		for (int i = 0; i < my->game.level_list.levels[current_level].curve_infos.curve_list.curve_count; i++)
		{
			fprintf (fichier, "%lf %lf\n", my->game.level_list.levels[current_level].curve_infos.curve_list.curves[i].shift_x, my->game.level_list.levels[current_level].curve_infos.curve_list.curves[i].shift_y);

			fprintf (fichier, "%d\n", my->game.level_list.levels[current_level].curve_infos.curve_list.curves[i].control_count);

			for (int j = 0; j < my->game.level_list.levels[current_level].curve_infos.curve_list.curves[i].control_count; j++)
			{
				fprintf (fichier, "%lf %lf\n", my->game.level_list.levels[current_level].curve_infos.curve_list.curves[i].controls[j].x, my->game.level_list.levels[current_level].curve_infos.curve_list.curves[i].controls[j].y);	
			}
		}
		fclose (fichier);
		load_level (&my->game.level_list, level);
	}
	else
		printf ("impossible de sauvegarder le niveau\n");
}

void on_button_delete_level_clicked (GtkWidget *widget, gpointer data)
{
	Mydata *my = get_mydata(data);

	int level = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (my->game.delete_level_spin));

	char levels_path[30] = "./levels/";
	char old_name[30] = "", new_name[30] = "";
	char str[12] = "";
	sprintf(str, "level%.2d.txt", level);
	strcat(levels_path, str);
	remove (levels_path);

	for (int i = level+1; i < my->game.level_list.level_count; i++)
	{
		strcpy (old_name, "./levels/");
		strcpy (new_name, "./levels/");
		sprintf(str, "level%.2d.txt", i);
		strcat(old_name, str);
		sprintf(str, "level%.2d.txt", i-1);
		strcat(new_name, str);
		rename (old_name, new_name);
	}
	my->game.level_list.level_count--;
	gtk_spin_button_set_range (GTK_SPIN_BUTTON (my->game.load_level_spin), 0, my->game.level_list.level_count-1);
	gtk_spin_button_set_range (GTK_SPIN_BUTTON (my->game.save_level_spin), 0, my->game.level_list.level_count);
	gtk_spin_button_set_range (GTK_SPIN_BUTTON (my->game.delete_level_spin), 0, my->game.level_list.level_count-1);	
}

void on_button_new_level_clicked (GtkWidget *widget, gpointer data)
{
	Mydata *my = get_mydata(data);
	FILE* fichier = NULL;
	char levels_path[30] = "./levels/";
	char str[12] = "";
	sprintf(str, "level%.2d.txt", my->game.level_list.level_count);
	strcat(levels_path, str);

	fichier = fopen (levels_path, "w");

	if (fichier != NULL)
	{
		my->game.level_list.level_count++;
		gtk_spin_button_set_range (GTK_SPIN_BUTTON (my->game.load_level_spin), 0, my->game.level_list.level_count-1);
		gtk_spin_button_set_range (GTK_SPIN_BUTTON (my->game.save_level_spin), 0, my->game.level_list.level_count);
		gtk_spin_button_set_range (GTK_SPIN_BUTTON (my->game.delete_level_spin), 0, my->game.level_list.level_count-1);		
		
		fprintf (fichier, "%lf %lf\n", 450.0, 250.0);

		fprintf (fichier, "%d %d\n", 0, 0);

		fprintf (fichier, "%d\n", 0);

		my->game.level_list.levels[my->game.level_list.level_count-1].curve_infos.current_curve = -1;
		my->game.level_list.levels[my->game.level_list.level_count-1].curve_infos.current_control = -1;	

		fclose (fichier);
		int level = my->game.level_list.level_count-1;
		load_level (&my->game.level_list, level);

		my->game.current_level = level;
		my->game.canon.cx = my->game.level_list.levels[level].canon_x;
		my->game.canon.cy = my->game.level_list.levels[level].canon_y;
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (my->game.marble_intro_spin), my->game.level_list.levels[level].marbles_intro);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (my->game.marble_total_spin), my->game.level_list.levels[level].marbles_total);
		refresh_area(my->area1);
	}
	else
		printf ("impossible de creer le niveau\n");
}

void window_init (GtkApplication* app, gpointer data)
{
	Mydata *my = get_mydata(data);

	my->window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (my->window), my->title);
	gtk_widget_set_size_request (my->window, my->win_width, my->win_height);
	gtk_window_set_resizable (GTK_WINDOW (my->window), FALSE);
	gtk_window_set_position (GTK_WINDOW (my->window), GTK_WIN_POS_CENTER_ALWAYS);
}

void save_scores (Game *game)
{
	FILE* fichier = NULL;
	fichier = fopen("scores.txt", "w");

	if (fichier != NULL)
	{
		for (int i = 0; i < 10; i++)
		{	
			fprintf(fichier, "%d %s\n", game->high_scores[i].score_value, game->high_scores[i].player_name);
		}
		fclose (fichier);
	}
	else
		printf ("impossible d'ouvrir le fichier des scores\n");
}

void on_button1_clicked (GtkWidget *widget, gpointer data)
{
	Mydata *my = get_mydata(data);
	const gchar *name = gtk_entry_get_text (GTK_ENTRY (my->entry));
	if (strcmp(name, "") != 0)
	{
		for (int i = 0; i < 10; i++)
		{
			if (my->game.score >= my->game.high_scores[i].score_value)
			{
				for (int j = 9; j > i; j--)
				{
					my->game.high_scores[j].score_value = my->game.high_scores[j-1].score_value;
					strcpy(my->game.high_scores[j].player_name,  my->game.high_scores[j-1].player_name);
				}
				my->game.high_scores[i].score_value = my->game.score;
				strcpy(my->game.high_scores[i].player_name, name);
				save_scores (&my->game);
				break;
			}
		}

		gtk_widget_hide (my->sub_win);
		sub_win_init (my);
		init_mydata (my);
	}	
}

void sub_win_init (gpointer data)
{
	GtkWidget *vbox1, *button1;
	GtkWidget *hbox[11], *label[21];
	char str[30];
	Mydata *my = get_mydata(data);

	my->sub_win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (my->sub_win), "High scores");
	gtk_widget_set_size_request (my->sub_win, my->win_width/4, my->win_height/4);
	gtk_window_set_resizable (GTK_WINDOW (my->sub_win), FALSE);
	gtk_window_set_position (GTK_WINDOW (my->sub_win), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_transient_for (GTK_WINDOW (my->sub_win), GTK_WINDOW (my->window));
	g_signal_connect (my->sub_win, "delete-event", G_CALLBACK (gtk_widget_hide_on_delete), my);

	vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_add (GTK_CONTAINER (my->sub_win), vbox1);

	for (int i = 0; i < 10; i++)
	{
		hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 30);
		gtk_container_add (GTK_CONTAINER (vbox1), hbox[i]);
		label[i*2] = gtk_label_new (my->game.high_scores[i].player_name);
		gtk_box_pack_start (GTK_BOX (hbox[i]), label[i*2], FALSE, FALSE, 10);
		sprintf(str, "%d", my->game.high_scores[i].score_value);
		label[i*2+1] = gtk_label_new (str);
		gtk_box_pack_start (GTK_BOX (hbox[i]), label[i*2+1], TRUE, TRUE, 10);
	}

	label[20] = gtk_label_new ("Enter your name :");
	gtk_box_pack_start (GTK_BOX (vbox1), label[20], TRUE, TRUE, 0);
	my->entry = gtk_entry_new ();
	gtk_box_pack_start (GTK_BOX (vbox1), my->entry, TRUE, TRUE, 0);

	hbox[10] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 50);
	gtk_container_add (GTK_CONTAINER (vbox1), hbox[10]);
	button1 =  gtk_button_new_with_label ("save");
	gtk_box_pack_start (GTK_BOX (hbox[10]), button1, TRUE, FALSE, 0);
	g_signal_connect (button1, "clicked", G_CALLBACK (on_button1_clicked), my);

	gtk_widget_show_all (my->sub_win);
	gtk_widget_hide (my->sub_win);

}

void editing_init (Mydata *my)
{
	GtkWidget *vbox1, *hbox1, *hbox2, *hbox3, *hbox4, *hbox5, *hbox6, *sepHorizontal, *sepHorizontal2;
	GtkWidget *load_level, *save_level, *new_level, *delete_level, *change_background;
	GtkWidget *marble_intro_label, *marble_total_label;
	int i;

	my->frame1 = gtk_frame_new ("Editing");

	vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

	my->edit_radios[1] = gtk_radio_button_new_with_label (NULL, "Add curve");
	my->edit_radios[2] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (my->edit_radios[1]), "Move Curve");
	my->edit_radios[3] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (my->edit_radios[1]), "Remove Curve");
	my->edit_radios[4] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (my->edit_radios[1]), "Add control");
	my->edit_radios[5] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (my->edit_radios[1]), "Move control");
	my->edit_radios[6] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (my->edit_radios[1]), "Remove control  ");
	my->edit_radios[7] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (my->edit_radios[1]), "Move canon");

	for (i = EDIT_NONE+1; i < EDIT_LAST; i++)
	{
		g_object_set_data (G_OBJECT(my->edit_radios[i]), "mode", GINT_TO_POINTER(i));
		g_signal_connect (my->edit_radios[i], "toggled", G_CALLBACK(on_edit_radio_toggled), my);
		gtk_box_pack_start (GTK_BOX (vbox1), my->edit_radios[i], FALSE, TRUE, 2);
	}

	sepHorizontal = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start (GTK_BOX (vbox1), sepHorizontal, FALSE, FALSE, 0);

	hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, TRUE, 2);

	marble_intro_label = gtk_label_new ("Marbles at start :");
	gtk_box_pack_start (GTK_BOX (hbox1), marble_intro_label, TRUE, TRUE, 2);

	my->game.marble_intro_spin = gtk_spin_button_new_with_range (0, 50, 1);
	gtk_box_pack_start (GTK_BOX (hbox1), my->game.marble_intro_spin, FALSE, TRUE, 2);

	hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (vbox1), hbox2, FALSE, TRUE, 2);

	marble_total_label = gtk_label_new ("Total of marbles : ");
	gtk_box_pack_start (GTK_BOX (hbox2), marble_total_label, TRUE, TRUE, 2);

	my->game.marble_total_spin = gtk_spin_button_new_with_range (0, 200, 1);
	gtk_box_pack_start (GTK_BOX (hbox2), my->game.marble_total_spin, FALSE, TRUE, 2);

	sepHorizontal2 = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start (GTK_BOX (vbox1), sepHorizontal2, FALSE, FALSE, 0);

	hbox6 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (vbox1), hbox6, FALSE, TRUE, 2);

	change_background = gtk_button_new_with_label ("Change background");
	gtk_box_pack_start (GTK_BOX (hbox6), change_background, TRUE, TRUE, 2);

	my->game.change_background_spin = gtk_spin_button_new_with_range (0, 13, 1);
	gtk_box_pack_start (GTK_BOX (hbox6), my->game.change_background_spin, FALSE, TRUE, 2);

	hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (vbox1), hbox3, FALSE, TRUE, 2);

	load_level = gtk_button_new_with_label ("Load level");
	gtk_box_pack_start (GTK_BOX (hbox3), load_level, TRUE, TRUE, 2);

	my->game.load_level_spin = gtk_spin_button_new_with_range (0, my->game.level_list.level_count-1, 1);
	gtk_box_pack_start (GTK_BOX (hbox3), my->game.load_level_spin, FALSE, TRUE, 2);

	hbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (vbox1), hbox4, FALSE, TRUE, 2);

	save_level = gtk_button_new_with_label ("Save level");
	gtk_box_pack_start (GTK_BOX (hbox4), save_level, TRUE, TRUE, 2);

	my->game.save_level_spin = gtk_spin_button_new_with_range (0, my->game.level_list.level_count, 1);
	gtk_box_pack_start (GTK_BOX (hbox4), my->game.save_level_spin, FALSE, TRUE, 2);

	hbox5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (vbox1), hbox5, FALSE, TRUE, 2);

	delete_level = gtk_button_new_with_label ("Delete level");
	gtk_box_pack_start (GTK_BOX (hbox5), delete_level, TRUE, TRUE, 2);

	my->game.delete_level_spin = gtk_spin_button_new_with_range (0, my->game.level_list.level_count-1, 1);
	gtk_box_pack_start (GTK_BOX (hbox5), my->game.delete_level_spin, FALSE, TRUE, 2);

	new_level = gtk_button_new_with_label ("New level");
	gtk_box_pack_start (GTK_BOX (vbox1), new_level, FALSE, TRUE, 2);

	g_signal_connect (load_level, "clicked", G_CALLBACK(on_button_load_level_clicked), my);
	g_signal_connect (save_level, "clicked", G_CALLBACK(on_button_save_level_clicked), my);
	g_signal_connect (new_level, "clicked", G_CALLBACK(on_button_new_level_clicked), my);
	g_signal_connect (delete_level, "clicked", G_CALLBACK(on_button_delete_level_clicked), my);
	g_signal_connect (change_background, "clicked", G_CALLBACK(on_button_change_background_clicked), my);

	gtk_container_add (GTK_CONTAINER (my->frame1), vbox1);
}

void layout_init (gpointer data)
{
	Mydata *my = get_mydata(data);

	my->vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
	gtk_container_add (GTK_CONTAINER (my->window), my->vbox1);

	gtk_box_pack_start (GTK_BOX (my->vbox1), my->menu_bar, FALSE, FALSE, 0);

	my->hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (my->vbox1), my->hbox1, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (my->hbox1), my->frame1, FALSE, FALSE, 0);
	my->scroll = gtk_scrolled_window_new (NULL, NULL);
	gtk_box_pack_start (GTK_BOX (my->hbox1), my->area1, TRUE, TRUE, 0);
	//gtk_container_add (GTK_CONTAINER (my->scroll), my->area1);
}
