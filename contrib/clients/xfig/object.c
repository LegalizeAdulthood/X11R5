/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
 *
 * "Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty."
 *
 */

#include "fig.h"
#include "object.h"

/************************  Objects  **********************/

F_compound      objects = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/*F_compound      select_objects = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};*/

/************  global object pointers ************/

F_line         *cur_l, *new_l, *old_l;
F_arc          *cur_a, *new_a, *old_a;
F_ellipse      *cur_e, *new_e, *old_e;
F_text         *cur_t, *new_t, *old_t;
F_spline       *cur_s, *new_s, *old_s;
F_compound     *cur_c, *new_c, *old_c;
F_point        *first_point, *cur_point;

/*************** object attribute settings ***********/

/*  Lines  */
int             cur_linewidth = 1;
int             cur_linestyle = SOLID_LINE;
float           cur_dashlength = DEF_DASHLENGTH;
float           cur_dotgap = DEF_DOTGAP;
float           cur_styleval = 0.0;
int             pen_size = 0;
int             pen_type = 0;
int             cur_color = -1;
int             cur_boxradius = DEF_BOXRADIUS;
int             cur_areafill = 0;
char            EMPTY_EPS[8] = "<empty>";

/* Text */
int             cur_fontsize;	/* font size */
int             cur_latex_font = 0;
int             cur_ps_font = 0;
int             cur_textjust = T_LEFT_JUSTIFIED;
int             cur_textflags = PSFONT_TEXT;

/* Misc */
float           cur_angle = 0.0;
