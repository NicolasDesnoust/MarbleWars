#ifndef GAME_H
#define GAME_H

#include <gtk/gtk.h>
#include <string.h>
#include <math.h>
#include "curve.h"
#include <stdbool.h>

typedef enum { GS_HELLO, GS_PLAYING, GS_PAUSE, GS_WON, GS_LOST } Game_state;
typedef enum { TS_INTRO, TS_NORMAL, TS_COMBO2, TS_COMBO3} Track_state;

#define SHOT_MAX          10
#define SHOT_SPEED         5
#define TRACK_MAX         10
#define MARBLE_COLORS      5
#define MARBLE_SPEED     0.5
#define MARBLE_SPEED_IO    5
#define MARBLE_SIZE     0.16
#define EXPLOSION_SPEED    2
#define AMMO1_DIST_MAX    30
#define MARBLE_MAX       200
#define SAMPLE_MAX      1000 
#define LEVEL_MAX         20
#define SAMPLE_THETA    0.05

typedef struct {
  double cx, cy;    // centre canon
  double angle;     // en radians
  int ammo1, ammo2, ammo1_dist;
} Canon;

typedef struct {
  double x, y;      // coordonnées centre
  double dx, dy;    // vecteur déplacement
  int color;
} Shot;

typedef struct {
  int shot_count;
  Shot shots[SHOT_MAX];
} Shot_list;

typedef struct {
  double x, y;      // coordonnées centre
  double t;         // paramètre dans l'échantillonnage
  int color;
  int is_combo_end; // ou encore, facteur vitesse et direction ?
  int step_explode;
} Marble;

typedef struct {
  int    sample_count;          // échantillonnage courbe
  double sample_x[SAMPLE_MAX], 
         sample_y[SAMPLE_MAX];
  int marble_count;
  int first_visible;
  Marble marbles[MARBLE_MAX];
  Track_state state;
} Track;

typedef struct {
  int track_count;
  Track tracks[TRACK_MAX];
} Track_list;

typedef struct {
  Curve_infos curve_infos;
  double canon_x, canon_y;
  int marbles_intro, marbles_total;
  int background;
} Level;

typedef struct {
  int level_count;
  Level levels[LEVEL_MAX];
} Level_list;

typedef struct {
  int score_value;
  char player_name[50];
} Score;

typedef struct {
  Game_state state;
  int current_level;
  int difficulty;
  int lifes;
  int score;
  int sub_win_print_id;
  Score high_scores[10];
  int marble_diameter;
  Canon canon;
  Shot_list shot_list;
  Track_list track_list;
  Level_list level_list;
  GRand *g_rand;
  GtkWidget *load_level_spin, *save_level_spin, *delete_level_spin, *marble_intro_spin, *marble_total_spin, *change_background_spin;
} Game;

void init_game (Game *game);

int random_color (Game *game);

int load_level (Level_list *level_list, int level);

void reset_game (Game *game);

bool is_in_track (Game *game, int color);

void progress_game_next_step (Game *game, int win_width, int win_height);

void sample_curve_to_track (Curve *curve, Track *track, double theta);

void create_marbles (Game *game);


#endif /* GAME_H */
