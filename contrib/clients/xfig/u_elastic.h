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

#define		MOVE_ARB	0
#define		MOVE_HORIZ_VERT	1
#define		BOX_SCALE	2
#define		BOX_HSTRETCH	3
#define		BOX_VSTRETCH	4

extern int      constrained;
extern int      fix_x, fix_y, work_numsides;
extern int      x1off, x2off, y1off, y2off;
extern CURSOR   cur_latexcursor;
extern int      from_x, from_y;
extern double   cosa, sina;
extern int      movedpoint_num;
extern int      latex_fix_x, latex_fix_y;
extern F_point *left_point, *right_point;

extern          elastic_box();
extern          elastic_movebox();
extern          resizing_box();
extern          elastic_box_constrained();
extern          constrained_resizing_box();
extern          moving_box();
extern          elastic_poly();
extern          resizing_poly();
extern		scaling_compound();
extern		elastic_scalecompound();

extern          resizing_ebr(), resizing_ebd();
extern          constrained_resizing_ebr(), constrained_resizing_ebd();
extern          constrained_resizing_cbd(), resizing_cbr(), resizing_cbd();
extern          elastic_moveellipse();
extern          moving_ellipse();
extern          elastic_scaleellipse();
extern          scaling_ellipse();

extern          freehand_line();
extern          latex_line();
extern          constrainedangle_line();
extern          elastic_moveline();
extern          elastic_line();
extern          moving_line();
extern          reshaping_line();
extern          reshaping_latexline();
extern          extending_line();
extern          elastic_linelink();
extern          elastic_scalepts();
extern          scaling_line();

extern          moving_arc();
extern          elastic_movearc();
extern          reshaping_arc();
extern          elastic_arclink();
extern          scaling_arc();
extern          elastic_scalearc();

extern          moving_text();
extern          draw_movingtext();

extern          moving_spline();
extern          scaling_spline();

extern          adjust_box_pos();
extern          adjust_pos();
