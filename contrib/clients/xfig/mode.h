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

/******************  modes  ******************/

#define		F_NULL		 	0
#define	    FIRST_DRAW_MODE         F_CIRCLE_BY_RAD
#define		F_CIRCLE_BY_RAD 	1
#define		F_CIRCLE_BY_DIA 	2
#define		F_ELLIPSE_BY_RAD 	3
#define		F_ELLIPSE_BY_DIA 	4
#define		F_CIRCULAR_ARC		5
#define		F_POLYLINE		6
#define		F_BOX			7
#define		F_POLYGON		8
#define		F_TEXT			9
#define		F_SPLINE		10
#define		F_CLOSED_SPLINE		11
#define		F_INTSPLINE		12
#define		F_CLOSED_INTSPLINE	13
#define		F_ARC_BOX		14
#define		F_REGPOLY		15
#define		F_EPSBITMAP		16
#define	    FIRST_EDIT_MODE         F_GLUE
#define		F_GLUE			30
#define		F_BREAK			31
#define		F_SCALE			32
#define		F_ADD			33
#define		F_COPY			34
#define		F_MOVE			35
#define		F_DELETE		36
#define		F_MOVE_POINT		37
#define		F_DELETE_POINT		38
#define		F_ADD_POINT		39
#define		F_DELETE_ARROW_HEAD	40
#define		F_ADD_ARROW_HEAD	41
#define		F_FLIP			42
#define		F_ROTATE		43
#define		F_AUTOARROW		44
#define		F_CONVERT		45
#define         F_CHANGE		46
#define		F_UPDATE		47
#define		F_ALIGN			48
#define		F_ZOOM			49
#define		F_LOAD			50
#define		F_EDIT			50

extern int      cur_mode;

/* alignment mode */
#define         NONE                    0
#define         LEFT                    1
#define         TOP                     1
#define         CENTER                  2
#define         RIGHT                   3
#define         BOTTOM                  3

extern int      cur_halign;
extern int      cur_valign;

/* angle geometry */
#define         L_UNCONSTRAINED         0
#define         L_LATEXLINE             1
#define         L_LATEXARROW            2
#define         L_MOUNTHATTAN           3
#define         L_MANHATTAN             4
#define         L_MOUNTAIN              5

extern int      manhattan_mode;
extern int      mountain_mode;
extern int      latexline_mode;
extern int      latexarrow_mode;

/* arrow mode */
#define         L_NOARROWS              0
#define         L_FARROWS               1
#define         L_FBARROWS              2
#define         L_BARROWS               3

extern int      autoforwardarrow_mode;
extern int      autobackwardarrow_mode;

/* grid mode */
#define         GRID_0                  0
#define         GRID_1                  1
#define         GRID_2                  2

extern int      cur_gridmode;

/* point position */
#define         P_ANY                   0
#define         P_MAGNET                1
#define         P_GRID1                 2
#define         P_GRID2                 3

extern int      cur_pointposn;
extern int      posn_rnd[P_GRID2 + 1];
extern int      posn_hlf[P_GRID2 + 1];

/* rotn axis */
#define		UD_FLIP			1
#define		LR_FLIP			2

extern int      cur_rotnangle;

/* misc */
extern int      action_on;
extern int      highlighting;
extern int      aborting;
extern int      anypointposn;
extern int      figure_modified;
extern int      cur_numsides;

extern void     reset_modifiedflag();
extern void     set_modifiedflag();
extern void     reset_action_on();
extern void     set_action_on();

/**********************  global mode variables  ************************/

extern int      num_point;
extern int      min_num_points;

/***************************  Print Settings  ****************************/

extern int      print_landscape;/* def. orientation for printer */

/***************************  Mode Settings  ****************************/

extern int      cur_objmask;

/***************************  Text Settings  ****************************/

extern int      cur_textstep;
extern int      hidden_text_length;

/***************************  File Settings  ****************************/

extern char     cur_dir[];
extern char     cur_filename[];
extern char     save_filename[];/* to undo load */
extern char     file_header[];
extern char     cut_buf_name[];
