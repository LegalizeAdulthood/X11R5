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
#define		PIX_PER_INCH		80
#define		PIX_PER_CM		30	/* ((int)(PIX_PER_INCH / 2.54
						 * + (5.0/2))) */

/* Portrait dimensions */
#define		DEF_CANVAS_HT_PORT	9.5*PIX_PER_INCH
#define		DEF_CANVAS_WD_PORT	8*PIX_PER_INCH

/* Landscape dimensions */
#define		DEF_CANVAS_HT_LAND	8*PIX_PER_INCH
#define		DEF_CANVAS_WD_LAND	10*PIX_PER_INCH

#define		RULER_WD		24
#define		MAX_TOPRULER_WD		1.1*DEF_CANVAS_WD_LAND
#define		MAX_SIDERULER_HT	1.1*DEF_CANVAS_HT_PORT
#define		MIN_MOUSEFUN_WD		240

#define		SW_PER_ROW_PORT	2	/* switches/row in mode panel */
#define		SW_PER_ROW_LAND	2	/* same for landscape mode */
#define		SW_PER_COL_PORT	17
#define		SW_PER_COL_LAND	17

#define		MODE_SW_HT	32	/* height of a mode switch icon */
#define		MODE_SW_WD	36	/* width of a mode switch icon */

#define		DEF_INTERNAL_BW		1
#define		POPUP_BW		2

extern int      TOOL_WD, TOOL_HT;
extern int      CMDPANEL_WD, CMDPANEL_HT;
extern int      MODEPANEL_WD, MODEPANEL_HT;
extern int      MODEPANEL_SPACE;
extern int      MSGPANEL_WD, MSGPANEL_HT;
extern int      MOUSEFUN_WD, MOUSEFUN_HT;
extern int      INDPANEL_WD;
extern int      CANVAS_WD, CANVAS_HT;
extern int      INTERNAL_BW;
extern int      TOPRULER_WD, TOPRULER_HT;
extern int      SIDERULER_WD, SIDERULER_HT;
extern int      SW_PER_ROW, SW_PER_COL;
