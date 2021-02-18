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

void            list_delete_arc();
void            list_delete_ellipse();
void            list_delete_line();
void            list_delete_spline();
void            list_delete_text();
void            list_delete_compound();

void            list_add_arc();
void            list_add_ellipse();
void            list_add_line();
void            list_add_spline();
void            list_add_text();
void            list_add_compound();

F_line          *last_line();
F_arc           *last_arc();
F_ellipse       *last_ellipse();
F_text          *last_text();
F_spline        *last_spline();
F_compound      *last_compound();

F_line          *prev_line();
F_arc           *prev_arc();
F_ellipse       *prev_ellipse();
F_text          *prev_text();
F_spline        *prev_spline();
F_compound      *prev_compound();

void            delete_line();
void            delete_arc();
void            delete_ellipse();
void            delete_text();
void            delete_spline();
void            delete_compound();

void            add_line();
void            add_arc();
void            add_ellipse();
void            add_text();
void            add_spline();
void            add_compound();

void            change_line();
void            change_arc();
void            change_ellipse();
void            change_text();
void            change_spline();
void            change_compound();
