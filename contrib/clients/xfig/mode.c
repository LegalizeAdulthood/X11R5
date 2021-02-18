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
#include "mode.h"
#include "object.h"

int             cur_mode = F_NULL;
int             cur_halign = NONE;
int             cur_valign = NONE;
int             manhattan_mode = 0;
int             mountain_mode = 0;
int             latexline_mode = 0;
int             latexarrow_mode = 0;
int             autoforwardarrow_mode = 0;
int             autobackwardarrow_mode = 0;
int             cur_gridmode = GRID_0;
int             cur_pointposn = P_MAGNET;
int             posn_rnd[P_GRID2 + 1];
int             posn_hlf[P_GRID2 + 1];
int             cur_rotnangle = 90;
int             cur_numsides = 6;
int             action_on = 0;
int             highlighting = 0;
int             aborting = 0;
int             anypointposn = 0;
int             figure_modified = 0;

/**********************  global mode variables  ************************/

int             num_point;
int             min_num_points;

/***************************  Print Settings  ****************************/

int             print_landscape = 0;	/* def. orientation for printer */

/***************************  Mode Settings  ****************************/

int             cur_objmask = M_NONE;

/***************************  Text Settings  ****************************/

int             cur_textstep = 12;
int             hidden_text_length;

/***************************  File Settings  ****************************/

char            cur_dir[1024];
char            cur_filename[200] = "";
char            save_filename[200] = "";	/* to undo load */
char            cut_buf_name[100];
char            file_header[32] = "#FIG ";

/*************************** routines ***********************/

void
reset_modifiedflag()
{
    figure_modified = 0;
}

void
set_modifiedflag()
{
    figure_modified = 1;
}

void
set_action_on()
{
    action_on = 1;
}

void
reset_action_on()
{
    action_on = 0;
}
