/* font.c
 *
 * Edouard.Thiel@lif.univ-mrs.fr - 18/02/2016
 *
 * This program is free software under the terms of the
 * GNU Lesser General Public License (LGPL) version 2.1.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "font.h"


/*
 * Chargement d'une fonte.
 * Usage : font_set_name (layout, "Times, bold italic 20");
*/

void font_set_name (PangoLayout *layout, char const *desc)
{
  PangoFontDescription *d = pango_font_description_from_string (desc);
  pango_layout_set_font_description (layout, d);
  pango_font_description_free (d);
}


/*
 * Change la taille de la foncte actuelle ; taille en pixels.
*/

void font_set_size (PangoLayout *layout, int size)
{
  const PangoFontDescription *d1 = pango_layout_get_font_description (layout);
  PangoFontDescription *d2 = pango_font_description_copy (d1);
  pango_font_description_set_absolute_size (d2, size * PANGO_SCALE);
  pango_layout_set_font_description (layout, d2);
  pango_font_description_free (d2);
}


/*
 * Place le texte actuel contenu dans le layout selon align, x, y
*/

int font_set_position (cairo_t *cr, PangoLayout *layout, Font_align align, 
    double x, double y)
{
  int valign, halign, text_w, text_h;

  if (align <= FONT_BEGIN || align >= FONT_END)
    { fprintf (stderr, "%s: bad align\n", __func__); return -1; }

  /* Decode align */
  align -= FONT_BEGIN + 1;
  halign = align % 3;
  valign = align / 3;

  /* On recupere les dimensions */
  pango_layout_get_pixel_size (layout, &text_w, &text_h);

  /* Alignement horizontal dans le layout */
  pango_layout_set_alignment (layout, 
      halign == 0 ? PANGO_ALIGN_LEFT : 
      halign == 1 ? PANGO_ALIGN_CENTER : 
                    PANGO_ALIGN_RIGHT);

  /* Déplacement du point de référence */
  cairo_move_to (cr, 
      x - text_w * halign/2.,
      y - text_h * valign/2.);

  return 0;
}


/*
 * Ecrit du texte, s'utilise comme printf.
 *
 * L'affichage de texte necessite un PangoLayout, qu'il faut creer avant puis
 * detruire apres :
 *
 *    PangoLayout *layout = pango_cairo_create_layout (cr);
 *    font_set_name (layout, "sans 12")
 *    font_draw_text (cr, layout, FONT_TL, 10, 10, "Poids = %d kg", 35);
 *    font_draw_text (cr, layout, FONT_TL, 10, 30, "Texte sur\ndeux lignes");
 *    g_object_unref (layout);
*/

void font_draw_text (cairo_t *cr, PangoLayout *layout,
    Font_align align, double x, double y, const char *format, ...)
{
  va_list ap;
  char buf[20000];

  /* Ecrit la chaine formatee dans buf */
  va_start (ap, format);
  vsnprintf (buf, sizeof(buf)-1, format, ap);
  va_end (ap);
  buf[sizeof(buf)-1] = 0;
  if (buf[0] == 0) return;

  /* On donne la chaine a afficher */
  pango_layout_set_text (layout, buf, -1);

  if (font_set_position (cr, layout, align, x, y) < 0) return;

  /* Affichage du layout */
  pango_cairo_show_layout (cr, layout);
}


/*
 * Ecrit du texte balise', s'utilise comme printf.
 * Meme usage que font_draw_text ; remplacer '\n' par '&#10;'
 *
 *    font_draw_markup (cr, layout, FONT_TL, 10, 10, "<b>Poids</b> = %d kg", 35);
 *    font_draw_markup (cr, layout, FONT_TL, 10, 30, "Texte sur&#10;deux lignes");
*/

void font_draw_markup (cairo_t *cr, PangoLayout *layout,
    Font_align align, double x, double y, const char *format, ...)
{
  va_list ap;
  char buf[20000];

  /* Ecrit la chaine formatee dans buf */
  va_start (ap, format);
  vsnprintf (buf, sizeof(buf)-1, format, ap);
  va_end (ap);
  buf[sizeof(buf)-1] = 0;
  if (buf[0] == 0) return;

  /* On donne le texte balise' a analyser */
  pango_layout_set_markup (layout, buf, -1);

  if (font_set_position (cr, layout, align, x, y) < 0) return;

  /* Affichage du layout */
  pango_cairo_show_layout (cr, layout);
}

