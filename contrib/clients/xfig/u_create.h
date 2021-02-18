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

extern F_arc   *create_arc();
extern F_ellipse *create_ellipse();
extern F_line  *create_line();
extern F_spline *create_spline();
extern F_text  *create_text();
extern F_compound *create_compound();
extern F_eps   *create_eps();
extern F_point *create_point();
extern F_control *create_cpoint();

extern F_arc   *copy_arc();
extern F_ellipse *copy_ellipse();
extern F_line  *copy_line();
extern F_spline *copy_spline();
extern F_text  *copy_text();
extern F_compound *copy_compound();

extern F_point *copy_points();
extern F_arrow *forward_arrow();
extern F_arrow *backward_arrow();
extern F_arrow *new_arrow();
extern char    *new_string();
