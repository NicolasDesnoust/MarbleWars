#include "drawings.h"

void draw_control_labels (cairo_t *cr, PangoLayout *layout, Curve_infos *ci)
{
	int i, j, x, y;

	font_set_name (layout, "Sans, 8");
	cairo_set_source_rgb (cr, 0.5, 0.5, 0.5);

	for (i = 0; i < ci->curve_list.curve_count; i++)
	{
		for (j = 0; j < ci->curve_list.curves[i].control_count; j++)
		{
			x = ci->curve_list.curves[i].controls[j].x;
			y = ci->curve_list.curves[i].controls[j].y;
			font_draw_text (cr, layout, FONT_BC, x, y - 2, "%d", j);
		}
	}
}

void draw_bezier_polygons_open (cairo_t *cr, Curve_infos *ci)
{
	Control bez_points[4];
	int i, j;

	for (j = 0; j < ci->curve_list.curve_count; j++)
	{
		for (i = 0; i < ci->curve_list.curves[j].control_count - 3; i++)
		{
			compute_bezier_points_open (&ci->curve_list.curves[j], i, bez_points);
			cairo_set_source_rgb (cr, 0.0, 1.0, 0.0);
			cairo_set_line_width (cr, 1.5);

			cairo_move_to (cr, bez_points[0].x, bez_points[0].y);
			cairo_line_to (cr, bez_points[1].x, bez_points[1].y);
			cairo_stroke (cr);

			cairo_move_to (cr, bez_points[2].x, bez_points[2].y);
			cairo_line_to (cr, bez_points[3].x, bez_points[3].y);
			cairo_stroke (cr);
		}
	}
}

void draw_bezier_curve (cairo_t *cr, Control bez_points[4], double theta)
{
	double bx[4], by[4];
	double t = 0.0, i = 0.0, x, y;
	int j;

	for (j = 0; j < 4; j++)
	{
		bx[j] = bez_points[j].x;
		by[j] = bez_points[j].y;
	}

	x = compute_bezier_cubic (bx, t);
	y = compute_bezier_cubic (by, t);
	cairo_set_line_width (cr, 2.5);
	cairo_set_source_rgb (cr, 0.9, 0.5, 1.0);
	cairo_move_to (cr, x, y);
	i++;
	t = i * theta;

	while (t <= 1)
	{
		x = compute_bezier_cubic (bx, t);
		y = compute_bezier_cubic (by, t);

		cairo_line_to (cr, x, y);

		i++;
		t = i * theta;
	}
	cairo_stroke (cr);
}

void draw_bezier_curves_prolong (cairo_t *cr, Curve_infos *ci, double theta)
{
	Control bez_points[4];
	int j, i;	

	for (j = 0; j < ci->curve_list.curve_count; j++)
	{
		Curve *curve = &ci->curve_list.curves[j];

		if (curve->control_count <= 3) continue;

        	compute_bezier_points_prolong_first (curve, bez_points);      
		draw_bezier_curve (cr, bez_points, theta);

		for (i = 0; i < curve->control_count - 3; i++)
		{
			compute_bezier_points_open (curve, i, bez_points);
			draw_bezier_curve (cr, bez_points, theta);
		}

		compute_bezier_points_prolong_last (curve, bez_points);
		draw_bezier_curve (cr, bez_points, theta);
	}
}

void quick_message (GtkWidget *parent, gpointer data)
{
	Mydata *my = get_mydata(data);

	gtk_window_present(GTK_WINDOW (my->sub_win));
}

void draw_control_polygons (cairo_t *cr, Curve_infos *ci)
{
	int i, j, x, y;
	
	cairo_set_line_width (cr, 1.5);

	for (i = 0; i < ci->curve_list.curve_count; i++)
	{

		if (i == ci->current_curve)
			cairo_set_source_rgb (cr, 1.0, 1.0, 0.0);
		else
			cairo_set_source_rgb (cr, 0.5, 0.5, 0.5);

		cairo_move_to (cr, ci->curve_list.curves[i].controls[0].x, ci->curve_list.curves[i].controls[0].y);

		for (j = 1; j < ci->curve_list.curves[i].control_count; j++)
		{
			x = ci->curve_list.curves[i].controls[j].x;
			y = ci->curve_list.curves[i].controls[j].y;

			cairo_line_to (cr, x, y);
		}
		cairo_stroke (cr);
	}

	for (i = 0; i < ci->curve_list.curve_count; i++)
	{
		for (j = 0; j < ci->curve_list.curves[i].control_count; j++)
		{
			x = ci->curve_list.curves[i].controls[j].x;
			y = ci->curve_list.curves[i].controls[j].y;

			if (i == ci->current_curve && j == ci->current_control)
			{
				cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
				cairo_rectangle (cr, x - 3.0, y - 3.0, 6.0, 6.0);
				cairo_stroke (cr);
			}
			else
			{
				cairo_set_source_rgb (cr, 0.0, 0.0, 1.0);
				cairo_rectangle (cr, x - 3.0, y - 3.0, 6.0, 6.0);
				cairo_stroke (cr);
			}		
		}
	}
}

void draw_canon (cairo_t *cr, gpointer data)
{
	Mydata *my = get_mydata(data);
	int ICwidth, ICheight, IBwidth, IBheight;
	double dx, dy, cx, cy, x, y;

	dx = cos (my->game.canon.angle);
	dy = sin (my->game.canon.angle);
	cx = my->game.canon.cx;
	cy = my->game.canon.cy;

	if (my->image_billes[my->game.canon.ammo1] != NULL)
	{
		IBwidth = cairo_image_surface_get_width (my->image_billes[my->game.canon.ammo1]);
		IBheight = cairo_image_surface_get_height (my->image_billes[my->game.canon.ammo1]);

		x = cx + dx * my->game.canon.ammo1_dist - IBwidth/2 -22;
		y = cy + dy * my->game.canon.ammo1_dist - IBheight/2;

		cairo_save (cr);
		cairo_translate (cr, x + IBwidth/2, y + IBheight/2);
		cairo_rotate (cr, my->game.canon.angle);
		cairo_scale (cr, MARBLE_SIZE, MARBLE_SIZE);
		cairo_translate (cr, - x - IBwidth/2 , - y - IBheight/2);
		cairo_set_source_surface (cr, my->image_billes[my->game.canon.ammo1], x, y);
		cairo_rectangle (cr, x, y, IBwidth, IBheight);
		cairo_fill (cr);
		cairo_restore (cr);
	}

	if (my->image_billes[my->game.canon.ammo2] != NULL)
	{
		IBwidth = cairo_image_surface_get_width (my->image_billes[my->game.canon.ammo2]);
		IBheight = cairo_image_surface_get_height (my->image_billes[my->game.canon.ammo2]);
		x = cx - IBwidth/2 -22;
		y = cy - IBheight/2;

		cairo_save (cr);
		cairo_translate (cr, x + IBwidth/2, y + IBheight/2);
		cairo_rotate (cr, my->game.canon.angle);
		cairo_scale (cr, MARBLE_SIZE, MARBLE_SIZE);
		cairo_translate (cr, - x - IBwidth/2 , - y - IBheight/2);
		cairo_set_source_surface (cr, my->image_billes[my->game.canon.ammo2], x, y);
		cairo_rectangle (cr, x, y, IBwidth, IBheight);
		cairo_fill (cr);
		cairo_restore (cr);
	}	

	if (my->image_canon != NULL)
	{
		ICwidth = cairo_image_surface_get_width (my->image_canon);
		ICheight = cairo_image_surface_get_height (my->image_canon);

		double x = cx - ICwidth/2, y = cy - ICheight/2;		

		cairo_save (cr);
		cairo_translate (cr, x + ICwidth/2 -22, y + ICheight/2);
		cairo_rotate (cr, my->game.canon.angle);
		cairo_scale (cr, 0.25, 0.25);
		cairo_translate (cr, - x - ICwidth/2 +22, - y - ICheight/2);
		cairo_set_source_surface (cr, my->image_canon, x, y);
		cairo_rectangle (cr, x, y, ICwidth, ICheight);
		cairo_fill (cr);
		cairo_restore (cr);
	}
}

void draw_shots (cairo_t *cr, gpointer data)
{
	Mydata *my = get_mydata(data);
	int IBwidth, IBheight, i, color;

	for (i = 0; i < my->game.shot_list.shot_count; i++)
	{
		color = my->game.shot_list.shots[i].color;

		if (my->image_billes[color] != NULL)
		{
			IBwidth = cairo_image_surface_get_width (my->image_billes[color]);
			IBheight = cairo_image_surface_get_height (my->image_billes[color]);

			double x = my->game.shot_list.shots[i].x - IBwidth/2, y = my->game.shot_list.shots[i].y - IBheight/2;

			cairo_save (cr);
			cairo_translate (cr, x + IBwidth/2, y + IBheight/2);
			cairo_scale (cr, MARBLE_SIZE, MARBLE_SIZE);
			cairo_translate (cr, - x - IBwidth/2, - y - IBheight/2);
			cairo_set_source_surface (cr, my->image_billes[color], x, y);
			cairo_rectangle (cr, x, y, IBwidth, IBheight);
			cairo_fill (cr);
			cairo_restore (cr);
		}
	}	
}

void draw_marble (cairo_t *cr, gpointer data, Marble *marble)
{
	Mydata *my = get_mydata(data);
	int IBwidth, IBheight;

	if (my->image_billes[marble->color] != NULL)
	{
		IBwidth = cairo_image_surface_get_width (my->image_billes[marble->color]);
		IBheight = cairo_image_surface_get_height (my->image_billes[marble->color]);

		double x = marble->x - IBwidth/2, y = marble->y - IBheight/2;

		cairo_save (cr);
		cairo_translate (cr, x + IBwidth/2, y + IBheight/2);
		cairo_scale (cr, MARBLE_SIZE, MARBLE_SIZE);
		cairo_translate (cr, - x - IBwidth/2, - y - IBheight/2);
		cairo_set_source_surface (cr, my->image_billes[marble->color], x, y);
		cairo_rectangle (cr, x, y, IBwidth, IBheight);
		cairo_fill (cr);
		cairo_restore (cr);

		if (marble->step_explode > 0 && my->explode_animation[marble->step_explode/EXPLOSION_SPEED] != NULL)
		{
			IBwidth = cairo_image_surface_get_width (my->explode_animation[marble->step_explode/EXPLOSION_SPEED]);
			IBheight = cairo_image_surface_get_height (my->explode_animation[marble->step_explode/EXPLOSION_SPEED]);

			x = marble->x - IBwidth/2, y = marble->y - IBheight/2;

			cairo_save (cr);
			cairo_translate (cr, x + IBwidth/2, y + IBheight/2);
			cairo_scale (cr, 0.5, 0.5);
			cairo_translate (cr, - x - IBwidth/2, - y - IBheight/2);
			cairo_set_source_surface (cr, my->explode_animation[marble->step_explode/EXPLOSION_SPEED], x, y);
			cairo_rectangle (cr, x, y, IBwidth, IBheight);
			cairo_fill (cr);
			cairo_restore (cr);
		}
	}
}

void draw_path (cairo_t *cr, Track *track)
{
	cairo_set_source_rgb (cr, 0.85, 0.85, 0.85);
	cairo_set_line_width (cr, 5);

	int compt = 0;
	while (compt+2 <= track->sample_count - 2)
	{
		cairo_move_to (cr, track->sample_x[compt], track->sample_y[compt]);
		for (int i = compt+1; i < compt+2; i++)
			cairo_line_to (cr, track->sample_x[i], track->sample_y[i]);
		compt+=2;
	}
	cairo_stroke (cr);

	cairo_arc (cr, track->sample_x[track->sample_count-1], track->sample_y[track->sample_count-1], 17, 0, G_PI*2);
	cairo_new_sub_path (cr);
	cairo_arc (cr, track->sample_x[track->sample_count-1], track->sample_y[track->sample_count-1], 12, 0, G_PI*2);
	cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);

	cairo_arc (cr, track->sample_x[0], track->sample_y[0], 3, 0, G_PI*2);
	cairo_fill (cr);
}

void draw_tracks (cairo_t *cr, gpointer data)
{
	Mydata *my = get_mydata(data);

	if (my->game.track_list.track_count == 0)
		return;

	for (int j = 0; j < my->game.track_list.track_count; j++)
	{
		draw_path (cr, &my->game.track_list.tracks[j]);

		for (int i = my->game.track_list.tracks[j].first_visible; i < my->game.track_list.tracks[j].marble_count; i++)
			draw_marble (cr, my, &my->game.track_list.tracks[j].marbles[i]);
	}
}

void prepare_ammo (gpointer data)
{
	Mydata *my = get_mydata(data);

	my->game.canon.ammo1_dist = 0;
}

void swap_ammo (gpointer data)
{
	Mydata *my = get_mydata(data);
	
	int temp = my->game.canon.ammo1;
	my->game.canon.ammo1 = my->game.canon.ammo2;
	my->game.canon.ammo2 = temp;
	prepare_ammo (my);	
}

gboolean on_area1_draw (GtkWidget *area, cairo_t *cr, gpointer data)
{
	Mydata *my = get_mydata(data);
	int pbWidth, pbHeight;

	if (my->game.state == GS_HELLO && my->pixbuf1 != NULL)
	{
		pbWidth = gdk_pixbuf_get_width (my->pixbuf1);
		pbHeight = gdk_pixbuf_get_height (my->pixbuf1);
		gdk_cairo_set_source_pixbuf (cr, my->pixbuf1, 0, 0);
		cairo_rectangle (cr, 0, 0, pbWidth, pbHeight);
		cairo_fill (cr);
	}

	if (my->background[my->game.level_list.levels[my->game.current_level].background] != NULL && my->game.current_level >= 0)
	{
		pbWidth = gdk_pixbuf_get_width (my->background[my->game.level_list.levels[my->game.current_level].background]);
		pbHeight = gdk_pixbuf_get_height (my->background[my->game.level_list.levels[my->game.current_level].background]);
		gdk_cairo_set_source_pixbuf (cr, my->background[my->game.level_list.levels[my->game.current_level].background], 0, 0);
		cairo_rectangle (cr, 0, 0, pbWidth, pbHeight);
		cairo_fill (cr);

		cairo_set_source_rgb (cr, 0.92, 0.92, 0.92);

		PangoLayout *level_layout = pango_cairo_create_layout (cr);
		font_set_name (level_layout, "Verdana, bold 20");
		font_draw_text (cr, level_layout, FONT_MC, 700, 55, "%d", my->game.current_level+1 + (my->game.level_list.level_count+1)*my->game.difficulty);
		g_object_unref (level_layout);

		PangoLayout *score_layout = pango_cairo_create_layout (cr);
		font_set_name (score_layout, "Verdana, bold 20");
		font_draw_text (cr, score_layout, FONT_MC, 700, 205, "%.6d", my->game.score);
		g_object_unref (score_layout);

	}

	if (my->heart[0] != NULL && my->heart[1] != NULL && my->game.current_level >= 0)
	{
		int width = gdk_pixbuf_get_width (my->heart[0]);
		int height = gdk_pixbuf_get_height (my->heart[0]);

		gdk_cairo_set_source_pixbuf (cr, my->heart[0], 650, 118.5);
		cairo_rectangle (cr, 650, 118.5, width, height);
		cairo_fill (cr);
		if (my->game.lifes >= 2)
			gdk_cairo_set_source_pixbuf (cr, my->heart[0], 683, 118.5);
		else
			gdk_cairo_set_source_pixbuf (cr, my->heart[1], 683, 118.5);
		cairo_rectangle (cr, 683, 118.5, width, height);
		cairo_fill (cr);
		if (my->game.lifes == 3)
			gdk_cairo_set_source_pixbuf (cr, my->heart[0], 716, 118.5);
		else
			gdk_cairo_set_source_pixbuf (cr, my->heart[1], 716, 118.5);
		cairo_rectangle (cr, 716, 118.5, width, height);

		cairo_fill (cr);
	}

	if (my->show_edit)
	{
		draw_control_polygons (cr, &my->game.level_list.levels[my->game.current_level].curve_infos);
		PangoLayout *layout = pango_cairo_create_layout (cr);
		draw_control_labels (cr, layout, &my->game.level_list.levels[my->game.current_level].curve_infos);
		g_object_unref (layout);

		draw_bezier_polygons_open (cr, &my->game.level_list.levels[my->game.current_level].curve_infos);
		draw_bezier_curves_prolong (cr, &my->game.level_list.levels[my->game.current_level].curve_infos, 0.1);
	}
	else
	{
		draw_tracks (cr, data);
		draw_shots (cr, data);
	}

	if (my->game.state != GS_HELLO)
		draw_canon (cr, data);

	return TRUE;
}

gboolean on_area1_key_press (GtkWidget *area, GdkEvent *event, gpointer data)
{
	Mydata *my = get_mydata(data);
	GdkEventKey *evk = &event->key;

	printf ("%s: GDK_KEY_%s\n", __func__, gdk_keyval_name(evk->keyval));
	switch (my->game.state) {
		case GS_HELLO :
			reset_game (&my->game);
			break;
		case GS_WON :
			break;
		case GS_LOST :
			break;
		case GS_PLAYING :
			switch (evk->keyval) {
				case GDK_KEY_space :
					swap_ammo (my);
					break;
				case GDK_KEY_q :
					gtk_widget_destroy(my->window);
					break;
				case GDK_KEY_p :
					my->game.state = GS_PAUSE;
					break;
			}
			break;
		case GS_PAUSE :
			switch (evk->keyval) {
				case GDK_KEY_a :
					if (my->show_edit)
						set_edit_mode(my, 1);
					break;
				case GDK_KEY_b :
					if (my->show_edit)
						set_edit_mode(my, 2);
					break;
				case GDK_KEY_c :
					if (my->show_edit)
						set_edit_mode(my, 3);
					break;
				case GDK_KEY_d :
					if (my->show_edit)
						set_edit_mode(my, 4);
					break;
				case GDK_KEY_e :
					if (my->show_edit)
						set_edit_mode(my, 5);
					break;
				case GDK_KEY_f :
					if (my->show_edit)
						set_edit_mode(my, 6);
					break;
				case GDK_KEY_q :
					gtk_widget_destroy(my->window);
					break;
				case GDK_KEY_p :
					if (!my->show_edit && !gtk_widget_is_visible (my->sub_win))
						my->game.state = GS_PLAYING;
					break;
			}
			break;
	}
	return TRUE; 
}

gboolean on_area1_key_release (GtkWidget *area, GdkEvent *event, gpointer data)
{
	GdkEventKey *evk = &event->key;

	printf ("%s: GDK_KEY_%s\n", __func__, gdk_keyval_name(evk->keyval));
	return TRUE; 
}

gboolean on_timeout1 (gpointer data)
{
	Mydata *my = get_mydata(data);

	if (my->game.sub_win_print_id == 1)
		quick_message (my->window, data);

	if (my->game.state != GS_PLAYING)
		return TRUE;

	progress_game_next_step (&my->game, my->win_width, my->win_height);

	refresh_area (my->area1);
	return TRUE;	
}

void shoot_ammo (gpointer data, double sx, double sy)
{
	Mydata *my = get_mydata(data);

	if (my->game.shot_list.shot_count == SHOT_MAX)
		return;

	double dx, dy, cx, cy;
	dx = cos (my->game.canon.angle);
	dy = sin (my->game.canon.angle);
	cx = my->game.canon.cx;
	cy = my->game.canon.cy;

	my->game.shot_list.shot_count++;
	my->game.shot_list.shots[my->game.shot_list.shot_count - 1].x = cx - 22 + dx * 20;
	my->game.shot_list.shots[my->game.shot_list.shot_count - 1].y = cy + dy * 20;
	my->game.shot_list.shots[my->game.shot_list.shot_count - 1].dx = dx;
	my->game.shot_list.shots[my->game.shot_list.shot_count - 1].dy = dy;
	my->game.shot_list.shots[my->game.shot_list.shot_count - 1].color = my->game.canon.ammo1;

	if(is_in_track (&my->game, my->game.canon.ammo2))
		my->game.canon.ammo1 = my->game.canon.ammo2;
	else
		my->game.canon.ammo1 = random_color(&my->game);
	my->game.canon.ammo2 = random_color(&my->game);
}

gboolean on_area1_button_press (GtkWidget *area, GdkEvent *event, gpointer data)
{
	Mydata *my = get_mydata(data);
	GdkEventButton *evb = &event->button;

	my->click_n = evb->button;
	my->click_x = evb->x;
	my->click_y = evb->y;

	if (event->type != GDK_BUTTON_PRESS)
		return TRUE;

	if (my->click_n == 1)
	{
		switch(my->game.state)
		{
			case GS_HELLO:
				break;
			case GS_PLAYING:
				shoot_ammo (my, evb->x, evb->y);
				prepare_ammo (my);
				break;
			case GS_PAUSE:
				switch (my->edit_mode)
				{
					case EDIT_ADD_CURVE:
						if(add_curve(&my->game.level_list.levels[my->game.current_level].curve_infos) < 0)
							break;
						set_edit_mode (my, 4);
						add_control (&my->game.level_list.levels[my->game.current_level].curve_infos, my->click_x, my->click_y);
					break;
					case EDIT_MOVE_CURVE:
						find_control (&my->game.level_list.levels[my->game.current_level].curve_infos, my->click_x, my->click_y);
					break;
					case EDIT_REMOVE_CURVE:
						if (find_control (&my->game.level_list.levels[my->game.current_level].curve_infos, my->click_x, my->click_y) == 0)
							remove_curve (&my->game.level_list.levels[my->game.current_level].curve_infos);
					break;
					case EDIT_ADD_CONTROL:
						add_control (&my->game.level_list.levels[my->game.current_level].curve_infos, my->click_x, my->click_y);
					break;
					case EDIT_MOVE_CONTROL:
						find_control (&my->game.level_list.levels[my->game.current_level].curve_infos, my->click_x, my->click_y);
					break;
					case EDIT_REMOVE_CONTROL:
						if (find_control (&my->game.level_list.levels[my->game.current_level].curve_infos, my->click_x, my->click_y) == 0)
							remove_control (&my->game.level_list.levels[my->game.current_level].curve_infos);
					break;
					case EDIT_MOVE_CANON:
						my->game.canon.cx = my->click_x;
						my->game.canon.cy = my->click_y;
						my->game.level_list.levels[my->game.current_level].canon_x = my->click_x;
						my->game.level_list.levels[my->game.current_level].canon_y = my->click_y;
					break;
				}
				break;
			case GS_WON:
				break;
			case GS_LOST:
				break;
		}
	refresh_area (my->area1);
	}
	return TRUE; 	
}

gboolean on_area1_button_release (GtkWidget *area, GdkEvent *event, gpointer data)
{
	Mydata *my = get_mydata(data);
	GdkEventButton *evb = &event->button;

	my->click_n = 0;

	printf ("%s: %d %.1f %.1f\n", __func__, evb->button, evb->x, evb->y);
	refresh_area (my->area1);
	return TRUE; 
}

void update_canon_angle (gpointer data, double sx, double sy)
{
	Mydata *my = get_mydata(data);

	if (my->game.state != GS_PLAYING)
		return;

	double vx, vy, n;
	vx = sx - my->game.canon.cx;
	vy = sy - my->game.canon.cy;
	n = sqrt (vx*vx + vy*vy);

	my->game.canon.angle = acos (vx/n);
	if (vy < 0)
		my->game.canon.angle *= (-1);
}

gboolean on_area1_motion_notify (GtkWidget *area, GdkEvent *event, gpointer data)
{
	Mydata *my = get_mydata(data);
	GdkEventMotion *evm = &event->motion;

	my->last_x = my->click_x;
	my->last_y = my->click_y;

	my->click_x = evm->x;
	my->click_y = evm->y;

	if (my->click_n == 1 && my->show_edit)
	{
		switch (my->edit_mode)
		{
			case EDIT_MOVE_CURVE:
				move_curve (&my->game.level_list.levels[my->game.current_level].curve_infos, my->click_x - my->last_x, my->click_y - my->last_y);
			break;
			case EDIT_MOVE_CONTROL:
				move_control (&my->game.level_list.levels[my->game.current_level].curve_infos, my->click_x - my->last_x, my->click_y - my->last_y);
			break;
		}
		refresh_area (my->area1);
	}
        update_canon_angle (data, evm->x, evm->y);
        refresh_area (my->area1);

	return TRUE; 
}

gboolean on_area1_enter_notify (GtkWidget *area, GdkEvent *event, gpointer data)
{
	GdkEventCrossing *evc = &event->crossing;
	printf ("%s: %.1f %.1f\n", __func__, evc->x, evc->y);

	gtk_widget_grab_focus (area);
	return TRUE; 
}

gboolean on_area1_leave_notify (GtkWidget *area, GdkEvent *event, gpointer data)
{
	GdkEventCrossing *evc = &event->crossing;
	printf ("%s: %.1f %.1f\n", __func__, evc->x, evc->y);

	gtk_widget_grab_focus (area);
	return TRUE;
}

void area1_init (gpointer data)
{
	Mydata *my = get_mydata(data);

	my->area1 = gtk_drawing_area_new ();

	g_signal_connect (my->area1, "draw", G_CALLBACK (on_area1_draw), my);

	g_signal_connect (my->area1, "key-press-event", G_CALLBACK (on_area1_key_press), my);
	g_signal_connect (my->area1, "key-release-event", G_CALLBACK (on_area1_key_release), my);
	g_signal_connect (my->area1, "button-press-event", G_CALLBACK (on_area1_button_press), my);
	g_signal_connect (my->area1, "button-release-event", G_CALLBACK (on_area1_button_release), my);
	g_signal_connect (my->area1, "motion-notify-event", G_CALLBACK (on_area1_motion_notify), my);
	g_signal_connect (my->area1, "enter-notify-event", G_CALLBACK (on_area1_enter_notify), my);
	g_signal_connect (my->area1, "leave-notify-event", G_CALLBACK (on_area1_leave_notify), my);

	gtk_widget_add_events (my->area1, GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK |GDK_FOCUS_CHANGE_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK);

	gtk_widget_set_can_focus (my->area1, TRUE);

	if (my->pixbuf1 != NULL)
		gtk_widget_set_size_request (my->area1, gdk_pixbuf_get_width (my->pixbuf1), gdk_pixbuf_get_height (my->pixbuf1));

	refresh_area (my->area1);
}


