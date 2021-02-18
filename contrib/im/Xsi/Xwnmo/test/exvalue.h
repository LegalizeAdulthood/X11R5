/*
 * $Id: exvalue.h,v 1.1 1991/09/11 06:48:27 ohm Exp $
 */
/*
 * X11R5 Input Method Test Program
 * exvalue.h v 1.0   Tue Mar  5 12:10:42 JST 1991
 */

/*
 * Copyright 1991 by OMRON Corporation
 * Copyright 1991 by the Massachusetts Institute of Technology
 *
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include <curses.h>

#include "define.h"
#include "typedef.h"

/* valuable of main window */
extern WINDOW		*prdisp, *modisp, *stdisp;

/* valuable of X resource */
extern Display          *dpy;
extern int              scr;
extern Window           rwin;
extern TESTWIN		window1, window2, window3;
extern Window		mroot, mwin[];
extern XFontSet         fontset[];
extern char      	bit_bits[ARRAY_MAX][BITS_MAX];
extern Pixmap           pix[];
extern u_long		pixel[ARRAY_MAX][PIX_MAX];
extern Colormap		cmap[];
extern Colormap		dcmap;
extern GC		gc;
extern CALLBACK		upec, ustc;
extern Cursor		cursor[];

/* valuable of pointer to logfile */
extern FILE	*imfp;		/* log file for im test */
extern FILE	*icfp;		/* log file for ic test */

/* valuable to use input context */
extern XIMStyle		uistyle;	/* Using Input STYLE */
extern Window		uclient;	/* Using CLIENT window */
extern Window		ufocus;		/* Using FOCUS window */
extern RESOURCE		ures;		/* Using RESource name/class */
extern PREEDIT		upea;		/* Using PreEdit Area */
extern STATUS		usta;		/* Using STatus Area */
extern FLAG		cmflag; 	/* ColorMap FLAG */
extern FLAG		focuson; 	/* flag for FOCUS ON/off */
extern Window		ufname;		/* Using Focus window NAME */

extern SETICMF		ftbl[];

                             






