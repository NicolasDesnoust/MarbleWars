#include "game.h"

int load_level (Level_list *level_list, int level)
{
	FILE* fichier = NULL;
	char levels_path[30] = "./levels/";
	char str[12] = "";
	sprintf(str, "level%.2d.txt", level);
	strcat(levels_path, str);
	fichier = fopen(levels_path, "r");

	if (fichier != NULL)
	{	
		if (fscanf(fichier, "%d", &level_list->levels[level].background) == 0)
			printf ("le fichier des niveau ne peut etre lu\n");

		if (fscanf(fichier, "%lf %lf", &level_list->levels[level].canon_x, &level_list->levels[level].canon_y) == 0)
			printf ("le fichier des niveau ne peut etre lu\n");

		if (fscanf(fichier, "%d %d", &level_list->levels[level].marbles_intro, &level_list->levels[level].marbles_total) == 0)
			printf ("le fichier des niveau ne peut etre lu\n");

		if (fscanf(fichier, "%d", &level_list->levels[level].curve_infos.curve_list.curve_count) == 0)
			printf ("le fichier des niveau ne peut etre lu\n");

		level_list->levels[level].curve_infos.current_curve = -1;
		level_list->levels[level].curve_infos.current_control = -1;
	
		for (int i = 0; i < level_list->levels[level].curve_infos.curve_list.curve_count; i++)
		{
			if (fscanf(fichier, "%lf %lf", &level_list->levels[level].curve_infos.curve_list.curves[i].shift_x, &level_list->levels[level].curve_infos.curve_list.curves[i].shift_y) == 0)
				printf ("le fichier des niveau ne peut etre lu\n");

			if (fscanf(fichier, "%d", &level_list->levels[level].curve_infos.curve_list.curves[i].control_count) == 0)
				printf ("le fichier des niveau ne peut etre lu\n");

			for (int j = 0; j < level_list->levels[level].curve_infos.curve_list.curves[i].control_count; j++)
			{
				if (fscanf(fichier, "%lf %lf", &level_list->levels[level].curve_infos.curve_list.curves[i].controls[j].x, &level_list->levels[level].curve_infos.curve_list.curves[i].controls[j].y) == 0)
				printf ("le fichier des niveau ne peut etre lu\n");	
			}
		}
		fclose (fichier);
		return 1;
	}
	else
		return 0;
}

void init_levels (Game *game)
{
	int i = 0;

	while (load_level (&game->level_list, i))
	{
		game->level_list.level_count++;
		i++;
	}
}

void init_scores (Game *game)
{
	FILE* fichier = NULL;
	fichier = fopen("scores.txt", "r");

	if (fichier != NULL)
	{
		for (int i = 0; i < 10; i++)
		{	
			if (fscanf(fichier, "%d %s", &game->high_scores[i].score_value, game->high_scores[i].player_name) == 0)
			printf ("le fichier des scores ne peut etre lu\n");
		}
		fclose (fichier);
	}
	else
		printf ("impossible d'ouvrir le fichier des scores\n");
}

bool is_in_track (Game *game, int color)
{
	for (int i = 0; i < game->track_list.track_count; i++)
		for (int k = 0; k < game->track_list.tracks[i].marble_count; k++)
			if (color == game->track_list.tracks[i].marbles[k].color)
				return true;
	return false;
}

int random_color (Game *game)
{
	bool colors_still_in_track[MARBLE_COLORS];
	for (int j = 0; j < MARBLE_COLORS; j++)
		colors_still_in_track[j] = false;

	for (int j = 0; j < MARBLE_COLORS; j++)
		if (is_in_track (game, j))
			colors_still_in_track[j] = true;

	int compt = 0;
	for (int j = 0; j < MARBLE_COLORS; j++)
		if (colors_still_in_track[j] == true)
			compt++;

	int colors[MARBLE_COLORS];
	int a = 0;
	for (int j = 0; j < compt; j++)
	{
		while (colors_still_in_track[a] == false)
		{
			a++;		
		}
		colors[j] = a;
		a++;
	}

	if (compt > 0)
		return colors[g_rand_int_range (game->g_rand, 0, compt)];
	else
		return 0;
}

void init_game (Game *game)
{
	game->g_rand = g_rand_new();
	game->state = GS_HELLO;
	game->canon.cx = 400.0;
	game->canon.cy = 250.0;
	game->difficulty = 0;
	game->canon.ammo1_dist = AMMO1_DIST_MAX;
	game->current_level = -1;
	game->lifes = 3;
	game->score = 0;
	game->marble_diameter = 0;
	game->shot_list.shot_count = 0;
	game->track_list.track_count = 0;
	game->level_list.level_count = 0;
	game->sub_win_print_id = 0;
	init_levels (game);
	init_scores (game);
	game->canon.ammo1 = g_rand_int_range(game->g_rand, 0, MARBLE_COLORS);
	game->canon.ammo2 = g_rand_int_range(game->g_rand, 0, MARBLE_COLORS);
}

void do_vector_product (double xu, double yu, double zu, double xv, double yv, double zv, double *X, double *Y, double *Z)
{
	*X = yu*zv - yv*zu;
	*Y = xv*zu - xu*zv;
	*Z = xu*yv - xv*yu;
}

int detect_collides_with_marble (Marble marble, Shot shot, Game *game)
{
	double xA, yA, xB, yB;
	xA = marble.x;
	yA = marble.y;
	xB = shot.x;
	yB = shot.y;

	double dist = sqrt (pow ((xB - xA), 2) + pow ((yB - yA), 2));

	if (dist <= game->marble_diameter)
		return 1;
	return 0;
}

void start_step_explode (Game *game, int track, int first_marble, int compt)
{
	for (int i = first_marble; i < first_marble + compt; i++)
	{
		game->track_list.tracks[track].marbles[i].step_explode = 1;
	}
}

void detect_combos (Game *game, int track, int marble)
{
	int compt = 0, first_marble = -1; // first_marble : première bille du combo s'il existe
	for (int i = marble+1; i < game->track_list.tracks[track].marble_count; i++)
	{
		if (game->track_list.tracks[track].marbles[marble].color ==  game->track_list.tracks[track].marbles[i].color)
			compt++;
		else
			break;
	}

	for (int i = marble-1; i >= 0; i--)
	{
		if (game->track_list.tracks[track].marbles[marble].color ==  game->track_list.tracks[track].marbles[i].color && i >= game->track_list.tracks[track].first_visible)
		{
			compt++;
			first_marble = i;
		}		
		else
			break;
	}
	if (compt != 0)
		compt++;

	if (compt != 0 && first_marble == -1) // si il n'y a pas de bille derrière "marble" c'est la premiere du combo
	{
		first_marble = marble;
	}

	if (compt >= 3)
	{
		start_step_explode (game, track, first_marble, compt);
	}
}

int detect_gap_after_marble (Game *game, int track, int marble)
{
	double xA, yA, xB, yB;
	double dist;
	int end_gap = -1;
	Track *tk = &game->track_list.tracks[track];
	for (int k = marble; k < tk->marble_count-1; k++)
	{
		xA = tk->marbles[k].x;
		yA = tk->marbles[k].y;
		xB = tk->marbles[k+1].x;
		yB = tk->marbles[k+1].y;
		dist = sqrt (pow ((xB - xA), 2) + pow ((yB - yA), 2));

		if (dist >= game->marble_diameter*2)
			end_gap = k+1;
	}
	return end_gap;
}

bool detect_gap_before_marble (Game *game, int track, int marble)
{
	double xA, yA, xB, yB;
	double dist;
	Track *tk = &game->track_list.tracks[track];

	xA = tk->marbles[marble].x;
	yA = tk->marbles[marble].y;
	xB = tk->marbles[marble+1].x;
	yB = tk->marbles[marble+1].y;
	dist = sqrt (pow ((xB - xA), 2) + pow ((yB - yA), 2));

	if (dist >= game->marble_diameter*2)
		return true;
	return false; 
}


bool check_gap_between_marbles (Game *game, int marble1, int marble2, int track)
{
	double xA, yA, xB, yB;
	double dist;

	Track *tk = &game->track_list.tracks[track];

	if (marble2 > tk->marble_count-1)
		return false;

	xA = tk->marbles[marble1].x;
	yA = tk->marbles[marble1].y;
	xB = tk->marbles[marble2].x;
	yB = tk->marbles[marble2].y;
	dist = sqrt (pow ((xB - xA), 2) + pow ((yB - yA), 2));

	if (dist > game->marble_diameter*2)
		return true;
	return false;
}

void sort_table (Game *game, int marble, int track, bool devant)
{
	Track *tk = &game->track_list.tracks[track];
	Marble tmp_marble;
	tmp_marble.x = tk->marbles[tk->marble_count-1].x;
	tmp_marble.y = tk->marbles[tk->marble_count-1].y;
	tmp_marble.t = tk->marbles[tk->marble_count-1].t;
	tmp_marble.color = tk->marbles[tk->marble_count-1].color;
	tmp_marble.step_explode = tk->marbles[tk->marble_count-1].step_explode;
	if (devant == true)
		marble++;

	for (int i = tk->marble_count - 1; i > marble; i--)
	{
		tk->marbles[i].x = tk->marbles[i-1].x;
		tk->marbles[i].y = tk->marbles[i-1].y;
		tk->marbles[i].t = tk->marbles[i-1].t;
		tk->marbles[i].color = tk->marbles[i-1].color;
		tk->marbles[i].step_explode = tk->marbles[i-1].step_explode;
	}

	tk->marbles[marble].x = tmp_marble.x;
	tk->marbles[marble].y = tmp_marble.y;
	tk->marbles[marble].t = tmp_marble.t;
	tk->marbles[marble].color = tmp_marble.color;
	tk->marbles[marble].step_explode = tmp_marble.step_explode;
}

void insert_forward (Game *game, int marble, int track, int shot)
{
	Track *tk = &game->track_list.tracks[track];
	if (check_gap_between_marbles (game, marble, marble+1, track))
	{
		tk->marble_count++;
		tk->marbles[tk->marble_count-1].t = compute_distant_point_forward (tk->sample_x, tk->sample_y, tk->marbles[marble].t, tk->sample_count, game->marble_diameter, &tk->marbles[tk->marble_count-1].x, &tk->marbles[tk->marble_count-1].y);
		tk->marbles[tk->marble_count-1].color = game->shot_list.shots[shot].color;
		tk->marbles[tk->marble_count-1].step_explode = 0;
		sort_table (game, marble, track, true);
	}
	else
	{
		int i = marble;
		while (i != tk->marble_count-1 && !check_gap_between_marbles (game, i, i+1, track))
		{
				if (i == marble)
				{
					tk->marbles[i+1].t = compute_distant_point_forward (tk->sample_x, tk->sample_y, tk->marbles[i].t, tk->sample_count, game->marble_diameter*2, &tk->marbles[i+1].x, &tk->marbles[i+1].y);
				}
				else
					tk->marbles[i+1].t = compute_distant_point_forward (tk->sample_x, tk->sample_y, tk->marbles[i].t, tk->sample_count, game->marble_diameter, &tk->marbles[i+1].x, &tk->marbles[i+1].y);
				i++;		
		}

		tk->marble_count++;
		tk->marbles[tk->marble_count-1].t = compute_distant_point_forward (tk->sample_x, tk->sample_y, tk->marbles[marble].t, tk->sample_count, game->marble_diameter, &tk->marbles[tk->marble_count-1].x, &tk->marbles[tk->marble_count-1].y);
		tk->marbles[tk->marble_count-1].color = game->shot_list.shots[shot].color;
		tk->marbles[tk->marble_count-1].step_explode = 0;

		sort_table (game, marble, track, true);
	}
}

bool fv_near_origin (Game *game, Track *tk)
{
	double xA, yA, xB, yB;
	double dist;

	xA = tk->marbles[tk->first_visible].x;
	yA = tk->marbles[tk->first_visible].y;
	xB = tk->sample_x[0];
	yB = tk->sample_y[0];
	dist = sqrt (pow ((xB - xA), 2) + pow ((yB - yA), 2));

	if (dist < game->marble_diameter)
		return true;
	return false;	
}

void insert_backward (Game *game, int marble, int track, int shot)
{
	Track *tk = &game->track_list.tracks[track];

	if (marble == tk->first_visible)
	{
		if (fv_near_origin (game, tk))
		{
			int i = marble;
			while (i != tk->marble_count-1 && !check_gap_between_marbles (game, i, i+1, track))
			{
				if (i == marble)
				{
					tk->marbles[i].t = compute_distant_point_forward (tk->sample_x, tk->sample_y, tk->marbles[i].t, tk->sample_count, game->marble_diameter, &tk->marbles[i].x, &tk->marbles[i].y);
				}
				else
					tk->marbles[i].t = compute_distant_point_forward (tk->sample_x, tk->sample_y, tk->marbles[i-1].t, tk->sample_count, game->marble_diameter, &tk->marbles[i].x, &tk->marbles[i].y);
				i++;		
			}
			tk->marble_count++;
			tk->marbles[tk->marble_count-1].t = compute_distant_point_backward (tk->sample_x, tk->sample_y, tk->marbles[marble].t, tk->sample_count, game->marble_diameter, &tk->marbles[tk->marble_count-1].x, &tk->marbles[tk->marble_count-1].y);
			tk->marbles[tk->marble_count-1].color = game->shot_list.shots[shot].color;
			tk->marbles[tk->marble_count-1].step_explode = 0;
			sort_table (game, marble, track, false);
		}
		else
		{
			tk->marble_count++;
			tk->marbles[tk->marble_count-1].t = compute_distant_point_backward (tk->sample_x, tk->sample_y, tk->marbles[marble].t, tk->sample_count, game->marble_diameter, &tk->marbles[tk->marble_count-1].x, &tk->marbles[tk->marble_count-1].y);
			tk->marbles[tk->marble_count-1].color = game->shot_list.shots[shot].color;
			tk->marbles[tk->marble_count-1].step_explode = 0;
			sort_table (game, marble, track, false);
		}
		
	}
	else if (check_gap_between_marbles (game, marble, marble-1, track))
	{
		tk->marble_count++;
		tk->marbles[tk->marble_count-1].t = compute_distant_point_backward (tk->sample_x, tk->sample_y, tk->marbles[marble].t, tk->sample_count, game->marble_diameter, &tk->marbles[tk->marble_count-1].x, &tk->marbles[tk->marble_count-1].y);
		tk->marbles[tk->marble_count-1].color = game->shot_list.shots[shot].color;
		tk->marbles[tk->marble_count-1].step_explode = 0;
		sort_table (game, marble, track, false);
	}
	else
	{
		insert_forward (game, marble-1, track, shot);
	}
}

void add_shot_to_track (Game *game, int marble, int track, int shot)
{
	double xP, yP, xQ, yQ, xS, yS, xR, yR;	
	Track *tk = &game->track_list.tracks[track];

	xP = game->shot_list.shots[shot].x;
	yP = game->shot_list.shots[shot].y;
	xQ = xP + game->shot_list.shots[shot].dx;
	yQ = yP + game->shot_list.shots[shot].dy;
	xR = tk->marbles[marble].x;
	yR = tk->marbles[marble].y;
	compute_distant_point_forward (tk->sample_x, tk->sample_y, tk->marbles[marble].t, tk->sample_count, 0.1, &xS, &yS);

	double xU, yU, zU, xV, yV, zV, xT, yT, zT;

	do_vector_product (xP, yP, 1.0, xQ, yQ, 1.0, &xU, &yU, &zU);
	do_vector_product (xR, yR, 1.0, xS, yS, 1.0, &xV, &yV, &zV);
	do_vector_product (xU, yU, zU, xV, yV, zV, &xT, &yT, &zT);

	if (zT == 0)
		return;

	double sens;
	
	if (xR != xS)
		sens = (xP - xR)/(xS - xR);
	else
		sens = (yP - yR)/(yS - yR);

	if (sens < 0) // insérer derrière
	{
		insert_backward (game, marble, track, shot);
		detect_combos (game, track, marble);
	}
	else
	{
		insert_forward (game, marble, track, shot);
		detect_combos (game, track, marble+1);
	}

	for (int j = shot; j < game->shot_list.shot_count; j++)
	{
		game->shot_list.shots[j].x = game->shot_list.shots[j+1].x;
		game->shot_list.shots[j].y = game->shot_list.shots[j+1].y;
		game->shot_list.shots[j].dx = game->shot_list.shots[j+1].dx;
		game->shot_list.shots[j].dy = game->shot_list.shots[j+1].dy;
		game->shot_list.shots[j].color = game->shot_list.shots[j+1].color;
	}
	game->shot_list.shot_count--;
}

void create_marbles (Game *game)
{
	if (game->track_list.track_count == 0)
		return;

	for (int j = 0; j < game->track_list.track_count; j++)
	{
		for (int i = 0; i < game->level_list.levels[game->current_level].marbles_total + 10 * game->difficulty; i++)
		{
			game->track_list.tracks[j].marble_count++;
			game->track_list.tracks[j].marbles[i].t = 0;
			game->track_list.tracks[j].marbles[i].x = game->track_list.tracks[j].sample_x[0];
			game->track_list.tracks[j].marbles[i].y = game->track_list.tracks[j].sample_y[0];
			game->track_list.tracks[j].marbles[i].color = g_rand_int_range(game->g_rand, 0, MARBLE_COLORS);
			game->track_list.tracks[j].marbles[i].step_explode = 0; 
		}

		game->track_list.tracks[j].first_visible = game->track_list.tracks[j].marble_count - 1;
	}
	game->state = GS_PLAYING;
	game->canon.ammo1 = random_color (game);
	game->canon.ammo2 = random_color (game);
}

void reset_game (Game *game)
{
	game->score = 0;
	game->lifes = 3;
	game->shot_list.shot_count = 0;
	game->current_level = 0;
	load_level (&game->level_list, game->current_level);
	game->canon.cx = game->level_list.levels[game->current_level].canon_x;
	game->canon.cy = game->level_list.levels[game->current_level].canon_y;
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (game->marble_intro_spin), game->level_list.levels[game->current_level].marbles_intro);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (game->marble_total_spin), game->level_list.levels[game->current_level].marbles_total);

	game->track_list.track_count = game->level_list.levels[game->current_level].curve_infos.curve_list.curve_count;

	for (int i = 0; i < game->level_list.levels[game->current_level].curve_infos.curve_list.curve_count; i++)
	{
		sample_curve_to_track (&game->level_list.levels[game->current_level].curve_infos.curve_list.curves[i], &game->track_list.tracks[i], 0.1);
		game->track_list.tracks[i].marble_count = 0;
		game->track_list.tracks[i].first_visible = -1;
		game->track_list.tracks[i].state = TS_INTRO;
	}
	create_marbles (game);
}

void start_next_level (Game *game)
{
		int level = game->current_level+1;
		if (level == game->level_list.level_count)
		{
			level = 0;
			game->difficulty++;
		}		
		game->current_level = level;
		game->canon.cx = game->level_list.levels[level].canon_x;
		game->canon.cy = game->level_list.levels[level].canon_y;
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (game->marble_intro_spin), game->level_list.levels[level].marbles_intro);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (game->marble_total_spin), game->level_list.levels[level].marbles_total);

		game->shot_list.shot_count = 0;

		game->state = GS_PLAYING;

		game->track_list.track_count = game->level_list.levels[game->current_level].curve_infos.curve_list.curve_count;

		for (int i = 0; i < game->level_list.levels[game->current_level].curve_infos.curve_list.curve_count; i++)
		{
			sample_curve_to_track (&game->level_list.levels[game->current_level].curve_infos.curve_list.curves[i], &game->track_list.tracks[i], 0.1);
			game->track_list.tracks[i].marble_count = 0;
			game->track_list.tracks[i].first_visible = -1;
			game->track_list.tracks[i].state = TS_INTRO;
		}
		create_marbles (game);
}

void restart_level (Game *game)
{
	game->shot_list.shot_count = 0;
	game->difficulty = 0;
	game->state = GS_PLAYING;

	game->track_list.track_count = game->level_list.levels[game->current_level].curve_infos.curve_list.curve_count;

	for (int i = 0; i < game->level_list.levels[game->current_level].curve_infos.curve_list.curve_count; i++)
	{
		sample_curve_to_track (&game->level_list.levels[game->current_level].curve_infos.curve_list.curves[i], &game->track_list.tracks[i], 0.1);
		game->track_list.tracks[i].marble_count = 0;
		game->track_list.tracks[i].first_visible = -1;
		game->track_list.tracks[i].state = TS_INTRO;
	}
	create_marbles (game);
}

void sample_curve_to_track (Curve *curve, Track *track, double theta)
{
  	Control bez_points[4];
	int ind = 0;

	if (curve->control_count < 3) return;

	compute_bezier_points_prolong_first (curve, bez_points);
	sample_bezier_curve (bez_points, theta, 
	track->sample_x, track->sample_y, &ind, SAMPLE_MAX, 1);

	for (int k = 0; k < curve->control_count-3; k++) {
		compute_bezier_points_open (curve, k, bez_points);
		sample_bezier_curve (bez_points, theta, 
		track->sample_x, track->sample_y, &ind, SAMPLE_MAX, 0);
	}

	compute_bezier_points_prolong_last (curve, bez_points);
	sample_bezier_curve (bez_points, theta, 
	track->sample_x, track->sample_y, &ind, SAMPLE_MAX, 0);
	track->sample_count = ind;
}

void move_ammo1_one_step (int *ammo1_dist)
{
	if (*ammo1_dist < AMMO1_DIST_MAX)
		*ammo1_dist += 2;
}

void move_shot_one_step (Shot_list *shot_list)
{
	int i;

	for (i = 0; i < shot_list->shot_count; i++)
	{
		shot_list->shots[i].x += shot_list->shots[i].dx * SHOT_SPEED;
		shot_list->shots[i].y += shot_list->shots[i].dy * SHOT_SPEED;
	}
}

void suppress_far_shots (Shot_list *shot_list, int win_width, int win_height)
{
	int i, j;

	for (i = 0; i < shot_list->shot_count; i++)
		if (shot_list->shots[i].x > win_width - 220 || shot_list->shots[i].y > win_height || shot_list->shots[i].x < 0 || shot_list->shots[i].y < 0)
		{
			for (j = i; j < shot_list->shot_count; j++)
			{
				shot_list->shots[j].x = shot_list->shots[j+1].x;	
				shot_list->shots[j].y = shot_list->shots[j+1].y;	
				shot_list->shots[j].dx = shot_list->shots[j+1].dx;	
				shot_list->shots[j].dy = shot_list->shots[j+1].dy;	
				shot_list->shots[j].color = shot_list->shots[j+1].color;	
			}
			shot_list->shot_count--;
			if (i < shot_list->shot_count - 1)
				i--;
		}
}

void move_first_visible_one_step (Game *game, Track *track)
{
	int fv = track->first_visible;
	double tA = track->marbles[fv].t;
	double xB, yB, speed;

	if (track->state == TS_INTRO)
	{
		speed = MARBLE_SPEED_IO + 0.5 * MARBLE_SPEED_IO * game->difficulty;
	}
	else
		speed = MARBLE_SPEED + 0.5 * MARBLE_SPEED * game->difficulty;

	track->marbles[fv].t = compute_distant_point_forward (track->sample_x, track->sample_y, tA, track->sample_count, speed, &xB, &yB);
	track->marbles[fv].x = xB;	
	track->marbles[fv].y = yB;
}

void push_marbles_one_step (Game *game, Track *track)
{
	double tA;
	double xA, yA, xB, yB;
	double dist;

	for (int i = track->first_visible + 1; i < track->marble_count; i++)
	{
		if (i == game->level_list.levels[game->current_level].marbles_total - game->level_list.levels[game->current_level].marbles_intro +1)
			track->state = TS_NORMAL;


		xA = track->marbles[i].x;
		yA = track->marbles[i].y;
		xB = track->marbles[i-1].x;
		yB = track->marbles[i-1].y;

		dist = sqrt (pow ((xB - xA), 2) + pow ((yB - yA), 2));


		if (dist < game->marble_diameter)
		{
			tA = track->marbles[i-1].t;
			track->marbles[i].t = compute_distant_point_forward (track->sample_x, track->sample_y, tA, track->sample_count, game->marble_diameter, &xA, &yA);
			track->marbles[i].x = xA;	
			track->marbles[i].y = yA;
		}
		else
			break;
	}
}

void show_new_marbles (Game *game, Track *track)
{
	double xA, yA, xB, yB;
	double dist;

	xA = track->marbles[track->first_visible].x;
	yA = track->marbles[track->first_visible].y;
	xB = track->sample_x[0];
	yB = track->sample_y[0];
	dist = sqrt (pow ((xB - xA), 2) + pow ((yB - yA), 2));
	
	if (dist >= game->marble_diameter && track->first_visible > 0)
	{
		track->first_visible--;
		track->marbles[track->first_visible].t = 0;
		track->marbles[track->first_visible].x = track->sample_x[0];	
		track->marbles[track->first_visible].y = track->sample_y[0];
	}
}

bool game_lost (Game *game)
{
	double xM = 0, yM = 0, xT = 0, yT = 0, dist = 0;

	for (int i = 0; i < game->track_list.track_count; i++)
	{
		xM = game->track_list.tracks[i].marbles[game->track_list.tracks[i].marble_count-1].x;
		yM = game->track_list.tracks[i].marbles[game->track_list.tracks[i].marble_count-1].y;
		xT = game->track_list.tracks[i].sample_x[game->track_list.tracks[i].sample_count-1];
		yT = game->track_list.tracks[i].sample_y[game->track_list.tracks[i].sample_count-1];

		dist = sqrt (pow ((xT - xM), 2) + pow ((yT - yM), 2));

		if (dist <= 2.0)	
			return true;
	}
	return false;	
}

bool game_won (Game *game)
{
	if (game->state != GS_PLAYING)
		return false;

	for (int i = 0; i < game->track_list.track_count; i++)
	{
		if (game->track_list.tracks[i].marble_count > 0)
			return false;
	}
	return true;
}

void animate_explosions (Track_list *track_list)
{
	for (int j = 0; j < track_list->track_count; j++)
	{
		for (int i = 0; i < track_list->tracks[j].marble_count; i++)
		{
			if (track_list->tracks[j].marbles[i].step_explode > 0)
			{
				if (track_list->tracks[j].marbles[i].step_explode < 8*EXPLOSION_SPEED)
				{
					track_list->tracks[j].marbles[i].step_explode++;
				}			
			}
		}
	}
}

int check_step_explode (Track *track)
{
	for (int j = 0; j < track->marble_count; j++)
	{
		if (track->marbles[j].step_explode == 8*EXPLOSION_SPEED)
			return j;
	}
	return (-1);
}

void remove_marbles (Game *game, int track, int first_marble, int compt)
{
	for (int i = first_marble; i < game->track_list.tracks[track].marble_count - compt; i++)
	{
		game->track_list.tracks[track].marbles[i].step_explode = game->track_list.tracks[track].marbles[i+compt].step_explode;
		game->track_list.tracks[track].marbles[i].color = game->track_list.tracks[track].marbles[i+compt].color;
		game->track_list.tracks[track].marbles[i].t = game->track_list.tracks[track].marbles[i+compt].t;
		game->track_list.tracks[track].marbles[i].x = game->track_list.tracks[track].marbles[i+compt].x;
		game->track_list.tracks[track].marbles[i].y = game->track_list.tracks[track].marbles[i+compt].y;
	}
	game->track_list.tracks[track].marble_count -= compt;
	game->score+= 5*compt;
}

void delete_marbles (Game *game, Track_list *track_list)
{
	Track *t = NULL;
	int compt;
	int first_marble;

	for (int j = 0; j < track_list->track_count; j++)
	{
		t = &track_list->tracks[j];
		while (check_step_explode (t) != -1)
		{
			first_marble = check_step_explode (t);
			compt = 0;

			for (int i = first_marble; i < t->marble_count; i++)
			{
				if (t->marbles[i].step_explode == 8*EXPLOSION_SPEED)
					compt++;
				else
					break;
			}
			remove_marbles (game, j, first_marble, compt);
			if (first_marble-1 >= t->first_visible && first_marble-1 < t->marble_count)
				detect_combos (game, j, first_marble-1);
			else if (first_marble < t->marble_count && first_marble >= t->first_visible)
				detect_combos (game, j, first_marble);

			if(!is_in_track (game, game->canon.ammo1))
				game->canon.ammo1 = random_color(game);
			if(!is_in_track (game, game->canon.ammo2))
				game->canon.ammo2 = random_color(game);
		}
	}
}

void progress_game_next_step (Game *game, int win_width, int win_height)
{
	move_ammo1_one_step (&game->canon.ammo1_dist);
	move_shot_one_step (&game->shot_list);
	suppress_far_shots (&game->shot_list, win_width, win_height);

	for (int i = 0; i < game->track_list.track_count; i++)
	{
		move_first_visible_one_step (game, &game->track_list.tracks[i]);
		push_marbles_one_step (game, &game->track_list.tracks[i]);
		show_new_marbles (game, &game->track_list.tracks[i]);
	}

	for (int i = 0; i < game->shot_list.shot_count; i++)
	{
		for (int j = 0; j < game->track_list.track_count; j++)
		{
			for (int k = 0; k < game->track_list.tracks[j].marble_count; k++)
			{
				if (k >= game->track_list.tracks[j].first_visible)
					if(detect_collides_with_marble (game->track_list.tracks[j].marbles[k], game->shot_list.shots[i], game))
						add_shot_to_track (game, k, j, i);
			}
		}
	}

	for (int j = 0; j < game->track_list.track_count; j++)
	{
		animate_explosions (&game->track_list);
		delete_marbles (game, &game->track_list);
	}

	if (game_won (game))
		start_next_level (game);

	if (game_lost (game))
	{
		if (game->lifes == 1)
		{
			game->sub_win_print_id = 1;
			game->state = GS_PAUSE;
		}
		else
		{
			restart_level (game);
			game->lifes--;
		}
	}
}

