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

/************** DECLARE EXPORTS ***************/

extern int      (*canvas_kbd_proc) ();
extern int      (*canvas_locmove_proc) ();
extern int      (*canvas_leftbut_proc) ();
extern int      (*canvas_middlebut_proc) ();
extern int      (*canvas_middlebut_save) ();
extern int      (*canvas_rightbut_proc) ();
extern int      (*return_proc) ();
extern int      null_proc();
extern int      clip_xmin, clip_ymin, clip_xmax, clip_ymax;
extern int      clip_width, clip_height;
extern int      cur_x, cur_y;
