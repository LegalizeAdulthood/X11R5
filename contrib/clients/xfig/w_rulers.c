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
#include "resources.h"
#include "mode.h"
#include "paintop.h"
#include "w_drawprim.h"
#include "w_mousefun.h"
#include "w_setup.h"
#include "w_util.h"
#include "w_zoom.h"

/*
 * The following will create rulers the same size as the initial screen size.
 * if the user resizes the xfig window, the rulers won't have numbers there.
 * Should really reset the sizes if the screen resizes.
 */

/*
 * set maximum ruler size:
 * color servers for Vaxstations screw up the pixmaps if the rulers are
 * made too big (they can't handle pixmaps much larger than the screen)
 */

#define			INCH_MARK		8
#define			HALF_MARK		8
#define			QUARTER_MARK		6
#define			SIXTEENTH_MARK		4

#define			TRM_WID			16
#define			TRM_HT			8
#define			SRM_WID			8
#define			SRM_HT			16

extern          pan_origin();

static GC       tr_gc, tr_xor_gc, tr_erase_gc;
static GC       sr_gc, sr_xor_gc, sr_erase_gc;
static int      lasty = -100, lastx = -100;
static int      troffx = -8, troffy = -10;
static int      orig_zoomoff;
static int      last_drag_x, last_drag_y;
static char     tr_marker_image[16] = {
    0xFE, 0xFF,			/* *************** */
    0xFC, 0x7F,			/* *************  */
    0xF8, 0x3F,			/* ***********   */
    0xF0, 0x1F,			/* *********    */
    0xE0, 0x0F,			/* *******     */
    0xC0, 0x07,			/* *****      */
    0x80, 0x03,			/* ***       */
    0x00, 0x01,			/* *        */
};
static          mpr_static(trm_pr, TRM_WID, TRM_HT, 1, tr_marker_image);
static int      srroffx = 2, srroffy = -7;
static char     srr_marker_image[16] = {
    0x80,			/* *  */
    0xC0,			/* **  */
    0xE0,			/* ***  */
    0xF0,			/* ****  */
    0xF8,			/* *****  */
    0xFC,			/* ******  */
    0xFE,			/* *******  */
    0xFF,			/* ********  */
    0xFE,			/* *******  */
    0xFC,			/* ******  */
    0xF8,			/* *****  */
    0xF0,			/* ****  */
    0xE0,			/* ***  */
    0xC0,			/* **  */
    0x80,			/* *  */
    0x00
};
static          mpr_static(srrm_pr, SRM_WID, SRM_HT, 1, srr_marker_image);

static int      srloffx = -10, srloffy = -7;
static char     srl_marker_image[16] = {
    0x01,			/* *          */
    0x03,			/* **         */
    0x07,			/* ***        */
    0x0F,			/* ****       */
    0x1F,			/* *****      */
    0x3F,			/* ******     */
    0x7F,			/* *******    */
    0xFF,			/* ********   */
    0x7F,			/* *******    */
    0x3F,			/* ******     */
    0x1F,			/* *****      */
    0x0F,			/* ****       */
    0x07,			/* ***        */
    0x03,			/* **         */
    0x01,			/* *          */
    0x00
};
static          mpr_static(srlm_pr, SRM_WID, SRM_HT, 1, srl_marker_image);

static Pixmap   toparrow_pm = 0, sidearrow_pm = 0;
static Pixmap   topruler_pm = 0, sideruler_pm = 0;

DeclareStaticArgs(14);

static          topruler_selected();
static          topruler_exposed();
static          sideruler_selected();
static          sideruler_exposed();

redisplay_rulers()
{
    redisplay_topruler();
    redisplay_sideruler();
}

setup_rulers()
{
    setup_topruler();
    setup_sideruler();
}

reset_rulers()
{
    reset_topruler();
    reset_sideruler();
}

set_rulermark(x, y)
    int             x, y;
{
    if (appres.TRACKING) {
	set_siderulermark(y);
	set_toprulermark(x);
    }
}

erase_rulermark()
{
    if (appres.TRACKING) {
	erase_siderulermark();
	erase_toprulermark();
    }
}

#define	HINCH	(PIX_PER_INCH / 2)
#define	QINCH	(PIX_PER_INCH / 4)
#define	SINCH	(PIX_PER_INCH / 16)
#define TWOMM	(PIX_PER_CM / 5)

/************************* UNITBOX ************************/

XtActionsRec    unitbox_actions[] =
{
    {"EnterUnitBox", (XtActionProc) draw_mousefun_unitbox},
    {"LeaveUnitBox", (XtActionProc) clear_mousefun},
    {"HomeRulers", (XtActionProc) pan_origin},
};

static String   unitbox_translations =
"<EnterWindow>:EnterUnitBox()\n\
    <LeaveWindow>:LeaveUnitBox()\n\
    <Btn1Down>:HomeRulers()\n";

int
init_unitbox(tool)
    TOOL            tool;
{
    FirstArg(XtNwidth, RULER_WD);
    NextArg(XtNheight, RULER_WD);
    NextArg(XtNlabel, appres.INCHES ? "in" : "cm");
    NextArg(XtNfont, button_font);
    NextArg(XtNfromHoriz, canvas_sw);
    NextArg(XtNhorizDistance, -INTERNAL_BW);
    NextArg(XtNfromVert, mousefun);
    NextArg(XtNvertDistance, -INTERNAL_BW);
    NextArg(XtNresizable, False);
    NextArg(XtNtop, XtChainTop);
    NextArg(XtNbottom, XtChainTop);
    NextArg(XtNleft, XtChainLeft);
    NextArg(XtNright, XtChainLeft);
    NextArg(XtNborderWidth, INTERNAL_BW);

    unitbox_sw = XtCreateWidget("unitbox", labelWidgetClass, tool,
				Args, ArgCount);
    XtAppAddActions(tool_app, unitbox_actions, XtNumber(unitbox_actions));
    XtOverrideTranslations(unitbox_sw,
			   XtParseTranslationTable(unitbox_translations));
    return (1);
}

/************************* TOPRULER ************************/

XtActionsRec    topruler_actions[] =
{
    {"EventTopRuler", (XtActionProc) topruler_selected},
    {"ExposeTopRuler", (XtActionProc) topruler_exposed},
    {"EnterTopRuler", (XtActionProc) draw_mousefun_topruler},
    {"LeaveTopRuler", (XtActionProc) clear_mousefun},
};

static String   topruler_translations =
"Any<BtnDown>:EventTopRuler()\n\
    Any<BtnUp>:EventTopRuler()\n\
    <Btn2Motion>:EventTopRuler()\n\
    <EnterWindow>:EnterTopRuler()\n\
    <LeaveWindow>:LeaveTopRuler()\n\
    <Expose>:ExposeTopRuler()\n";

static
topruler_selected(tool, event, params, nparams)
    TOOL tool;
    INPUTEVENT     *event;
    String         *params;
    Cardinal       *nparams;
{
    XButtonEvent   *be = (XButtonEvent *) event;

    switch (event->type) {
    case ButtonPress:
	switch (be->button) {
	case Button1:
	    XDefineCursor(tool_d, topruler_win, (Cursor) l_arrow_cursor.bitmap);
	    break;
	case Button2:
	    XDefineCursor(tool_d, topruler_win, (Cursor) bull_cursor.bitmap);
	    orig_zoomoff = zoomxoff;
	    last_drag_x = event->x;
	    break;
	case Button3:
	    XDefineCursor(tool_d, topruler_win, (Cursor) r_arrow_cursor.bitmap);
	    break;
	}
	break;
    case ButtonRelease:
	switch (be->button) {
	case Button1:
	    pan_left();
	    break;
	case Button2:
	    if (orig_zoomoff != zoomxoff)
		setup_grid(cur_gridmode);
	    break;
	case Button3:
	    pan_right();
	    break;
	}
	XDefineCursor(tool_d, topruler_win, (Cursor) lr_arrow_cursor.bitmap);
	break;
    case MotionNotify:
	if (event->x != last_drag_x)
	    if ((zoomxoff != 0) || (event->x < last_drag_x)) {
		zoomxoff -= (event->x - last_drag_x);
		if (zoomxoff < 0)
		    zoomxoff = 0;
		reset_topruler();
		redisplay_topruler();
	    }
	last_drag_x = event->x;
	break;
    }
}

erase_toprulermark()
{
    XClearArea(tool_d, topruler_win, ZOOMX(lastx) + troffx,
	       TOPRULER_HT + troffy, trm_pr.width,
	       trm_pr.height, False);
}

set_toprulermark(x)
    int             x;
{
    XClearArea(tool_d, topruler_win, ZOOMX(lastx) + troffx,
	       TOPRULER_HT + troffy, trm_pr.width,
	       trm_pr.height, False);
    XCopyArea(tool_d, toparrow_pm, topruler_win, tr_xor_gc,
	      0, 0, trm_pr.width, trm_pr.height,
	      ZOOMX(x) + troffx, TOPRULER_HT + troffy);
    lastx = x;
}

static
topruler_exposed(tool, event, params, nparams)
    TOOL            tool;
    INPUTEVENT     *event;
    String         *params;
    Cardinal       *nparams;
{
    if (((XExposeEvent *) event)->count > 0)
	return;
    redisplay_topruler();
}

redisplay_topruler()
{
    XClearWindow(tool_d, topruler_win);
}

int
init_topruler(tool)
    TOOL            tool;
{
    TOPRULER_HT = RULER_WD;
    FirstArg(XtNwidth, TOPRULER_WD);
    NextArg(XtNheight, TOPRULER_HT);
    NextArg(XtNlabel, "");
    NextArg(XtNfromHoriz, mode_panel);
    NextArg(XtNhorizDistance, -INTERNAL_BW);
    NextArg(XtNfromVert, msg_panel);
    NextArg(XtNvertDistance, -INTERNAL_BW);
    NextArg(XtNresizable, False);
    NextArg(XtNtop, XtChainTop);
    NextArg(XtNbottom, XtChainTop);
    NextArg(XtNleft, XtChainLeft);
    NextArg(XtNright, XtChainLeft);
    NextArg(XtNborderWidth, INTERNAL_BW);

    topruler_sw = XtCreateWidget("topruler", labelWidgetClass, tool,
				 Args, ArgCount);

    XtAppAddActions(tool_app, topruler_actions, XtNumber(topruler_actions));
    XtOverrideTranslations(topruler_sw,
			   XtParseTranslationTable(topruler_translations));
    return (1);
}

setup_topruler()
{
    unsigned long   bg, fg;
    XGCValues       gcv;
    unsigned long   gcmask;

    topruler_win = XtWindow(topruler_sw);
    gcv.font = roman_font->fid;
    gcmask = GCFunction | GCForeground | GCBackground | GCFont;

    /* set up the GCs */
    FirstArg(XtNbackground, &bg);
    NextArg(XtNforeground, &fg);
    GetValues(topruler_sw);

    gcv.foreground = bg;
    gcv.background = bg;
    gcv.function = GXcopy;
    tr_erase_gc = XCreateGC(tool_d, topruler_win, gcmask, &gcv);

    gcv.foreground = fg;
    tr_gc = XCreateGC(tool_d, topruler_win, gcmask, &gcv);
    /*
     * The arrows will be XORed into the rulers. We want the foreground color
     * in the arrow to result in the foreground or background color in the
     * display. so if the source pixel is fg^bg, it produces fg when XOR'ed
     * with bg, and bg when XOR'ed with bg. If the source pixel is zero, it
     * produces fg when XOR'ed with fg, and bg when XOR'ed with bg.
     */
    /* first make a temporary xor gc */
    gcv.foreground = fg ^ bg;
    gcv.background = (unsigned long) 0;
    gcv.function = GXcopy;
    tr_xor_gc = XCreateGC(tool_d, topruler_win, gcmask, &gcv);

    /* make pixmaps for top ruler arrow */
    toparrow_pm = XCreatePixmap(tool_d, topruler_win, trm_pr.width,
				trm_pr.height, DefaultDepthOfScreen(tool_s));
    XPutImage(tool_d, toparrow_pm, tr_xor_gc, &trm_pr, 0, 0, 0, 0,
	      trm_pr.width, trm_pr.height);

    /* now make the real xor gc */
    gcv.background = bg;
    gcv.function = GXxor;
    tr_xor_gc = XCreateGC(tool_d, topruler_win, gcmask, &gcv);

    XDefineCursor(tool_d, topruler_win, (Cursor) lr_arrow_cursor.bitmap);

    topruler_pm = XCreatePixmap(tool_d, topruler_win,
				TOPRULER_WD, TOPRULER_HT,
				DefaultDepthOfScreen(tool_s));

    reset_topruler();
}

resize_topruler()
{
    XFreePixmap(tool_d, topruler_pm);
    topruler_pm = XCreatePixmap(tool_d, topruler_win,
				TOPRULER_WD, TOPRULER_HT,
				DefaultDepthOfScreen(tool_s));

    reset_topruler();
}

reset_topruler()
{
    register int    i, j;
    register Pixmap p = topruler_pm;
    char            number[3];
    int             X0;

    /* top ruler, adjustments for digits are kludges based on 6x13 char */
    XFillRectangle(tool_d, p, tr_erase_gc, 0, 0, TOPRULER_WD, TOPRULER_HT);

    X0 = BACKX(0);
    if (appres.INCHES) {
	X0 -= (X0 % SINCH);
	for (i = X0 + SINCH - 1; i <= X0 + TOPRULER_WD; i += SINCH) {
	    j = i + 1;
	    if (j % PIX_PER_INCH == 0) {
		XDrawLine(tool_d, p, tr_gc, ZOOMX(i), TOPRULER_HT - 1, ZOOMX(i),
			  TOPRULER_HT - INCH_MARK - 1);
		sprintf(number, "%d", j / PIX_PER_INCH);
		XDrawString(tool_d, p, tr_gc, ZOOMX(i) - 3,
			    TOPRULER_HT - INCH_MARK - 5, number,
			    j < PIX_PER_INCH * 10 ? 1 : 2);
	    } else if (j % HINCH == 0)
		XDrawLine(tool_d, p, tr_gc, ZOOMX(i), TOPRULER_HT - 1, ZOOMX(i),
			  TOPRULER_HT - HALF_MARK - 1);
	    else if (j % QINCH == 0)
		XDrawLine(tool_d, p, tr_gc, ZOOMX(i), TOPRULER_HT - 1, ZOOMX(i),
			  TOPRULER_HT - QUARTER_MARK - 1);
	    else if (j % SINCH == 0)
		XDrawLine(tool_d, p, tr_gc, ZOOMX(i), TOPRULER_HT - 1, ZOOMX(i),
			  TOPRULER_HT - SIXTEENTH_MARK - 1);
	}
    } else {
	X0 -= (X0 % TWOMM);
	for (i = X0 + TWOMM - 1; i <= X0 + TOPRULER_WD; i++) {
	    j = i + 1;
	    if (j % PIX_PER_CM == 0) {
		XDrawLine(tool_d, p, tr_gc, ZOOMX(i), TOPRULER_HT - 1, ZOOMX(i),
			  TOPRULER_HT - INCH_MARK - 1);
		sprintf(number, "%d", j / PIX_PER_CM);
		XDrawString(tool_d, p, tr_gc, ZOOMX(i) - 3,
			    TOPRULER_HT - INCH_MARK - 5, number,
			    j < PIX_PER_CM * 10 ? 1 : 2);
	    } else if (j % TWOMM == 0)
		XDrawLine(tool_d, p, tr_gc, ZOOMX(i), TOPRULER_HT - 1, ZOOMX(i),
			  TOPRULER_HT - QUARTER_MARK - 1);
	}
    }
    FirstArg(XtNbackgroundPixmap, p);
    SetValues(topruler_sw);
}

/************************* SIDERULER ************************/

XtActionsRec    sideruler_actions[] =
{
    {"EventSideRuler", (XtActionProc) sideruler_selected},
    {"ExposeSideRuler", (XtActionProc) sideruler_exposed},
    {"EnterSideRuler", (XtActionProc) draw_mousefun_sideruler},
    {"LeaveSideRuler", (XtActionProc) clear_mousefun},
};

static String   sideruler_translations =
"Any<BtnDown>:EventSideRuler()\n\
    Any<BtnUp>:EventSideRuler()\n\
    <Btn2Motion>:EventSideRuler()\n\
    <EnterWindow>:EnterSideRuler()\n\
    <LeaveWindow>:LeaveSideRuler()\n\
    <Expose>:ExposeSideRuler()\n";

    static
    sideruler_selected(tool, event, params, nparams)
    TOOL tool;
    INPUTEVENT     *event;
    String         *params;
    Cardinal       *nparams;
{
    XButtonEvent   *be = (XButtonEvent *) event;

    switch (event->type) {
    case ButtonPress:
	switch (be->button) {
	case Button1:
	    XDefineCursor(tool_d, sideruler_win, (Cursor) u_arrow_cursor.bitmap);
	    break;
	case Button2:
	    XDefineCursor(tool_d, sideruler_win, (Cursor) bull_cursor.bitmap);
	    orig_zoomoff = zoomyoff;
	    last_drag_y = event->y;
	    break;
	case Button3:
	    XDefineCursor(tool_d, sideruler_win, (Cursor) d_arrow_cursor.bitmap);
	    break;
	}
	break;
    case ButtonRelease:
	switch (be->button) {
	case Button1:
	    pan_up();
	    break;
	case Button2:
	    if (orig_zoomoff != zoomyoff)
		setup_grid(cur_gridmode);
	    break;
	case Button3:
	    pan_down();
	    break;
	}
	XDefineCursor(tool_d, sideruler_win, (Cursor) ud_arrow_cursor.bitmap);
	break;
    case MotionNotify:
	if (event->y != last_drag_y)
	    if ((zoomyoff != 0) || (event->y < last_drag_y)) {
		zoomyoff -= (event->y - last_drag_y);
		if (zoomyoff < 0)
		    zoomyoff = 0;
		reset_sideruler();
		redisplay_sideruler();
	    }
	last_drag_y = event->y;
	break;
    }
}

static
sideruler_exposed(tool, event, params, nparams)
    TOOL            tool;
    INPUTEVENT     *event;
    String         *params;
    Cardinal       *nparams;
{
    if (((XExposeEvent *) event)->count > 0)
	return;
    redisplay_sideruler();
}

int
init_sideruler(tool)
    TOOL            tool;
{
    SIDERULER_WD = RULER_WD;
    FirstArg(XtNwidth, SIDERULER_WD);
    NextArg(XtNheight, SIDERULER_HT);
    NextArg(XtNlabel, "");
    NextArg(XtNfromHoriz, canvas_sw);
    NextArg(XtNhorizDistance, -INTERNAL_BW);
    NextArg(XtNfromVert, topruler_sw);
    NextArg(XtNvertDistance, -INTERNAL_BW);
    NextArg(XtNresizable, False);
    NextArg(XtNtop, XtChainTop);
    NextArg(XtNbottom, XtChainTop);
    NextArg(XtNleft, XtChainLeft);
    NextArg(XtNright, XtChainLeft);
    NextArg(XtNborderWidth, INTERNAL_BW);

    sideruler_sw = XtCreateWidget("sideruler", labelWidgetClass, tool,
				  Args, ArgCount);

    XtAppAddActions(tool_app, sideruler_actions, XtNumber(sideruler_actions));
    XtOverrideTranslations(sideruler_sw,
			   XtParseTranslationTable(sideruler_translations));
    return (1);
}

redisplay_sideruler()
{
    XClearWindow(tool_d, sideruler_win);
}

setup_sideruler()
{
    unsigned long   bg, fg;
    XGCValues       gcv;
    unsigned long   gcmask;

    sideruler_win = XtWindow(sideruler_sw);
    gcv.font = roman_font->fid;
    gcmask = GCFunction | GCForeground | GCBackground | GCFont;

    /* set up the GCs */
    FirstArg(XtNbackground, &bg);
    NextArg(XtNforeground, &fg);
    GetValues(sideruler_sw);

    gcv.foreground = bg;
    gcv.background = bg;
    gcv.function = GXcopy;
    sr_erase_gc = XCreateGC(tool_d, sideruler_win, gcmask, &gcv);

    gcv.foreground = fg;
    sr_gc = XCreateGC(tool_d, sideruler_win, gcmask, &gcv);
    /*
     * The arrows will be XORed into the rulers. We want the foreground color
     * in the arrow to result in the foreground or background color in the
     * display. so if the source pixel is fg^bg, it produces fg when XOR'ed
     * with bg, and bg when XOR'ed with bg. If the source pixel is zero, it
     * produces fg when XOR'ed with fg, and bg when XOR'ed with bg.
     */
    /* first make a temporary xor gc */
    gcv.foreground = fg ^ bg;
    gcv.background = (unsigned long) 0;
    gcv.function = GXcopy;
    sr_xor_gc = XCreateGC(tool_d, topruler_win, gcmask, &gcv);

    /* make pixmaps for side ruler arrow */
    if (appres.RHS_PANEL) {
	sidearrow_pm = XCreatePixmap(tool_d, sideruler_win,
				     srlm_pr.width, srlm_pr.height,
				     DefaultDepthOfScreen(tool_s));
	XPutImage(tool_d, sidearrow_pm, sr_xor_gc, &srlm_pr, 0, 0, 0, 0,
		  srlm_pr.width, srlm_pr.height);
    } else {
	sidearrow_pm = XCreatePixmap(tool_d, sideruler_win,
				     srrm_pr.width, srrm_pr.height,
				     DefaultDepthOfScreen(tool_s));
	XPutImage(tool_d, sidearrow_pm, sr_xor_gc, &srrm_pr, 0, 0, 0, 0,
		  srrm_pr.width, srrm_pr.height);
    }

    /* now make the real xor gc */
    gcv.background = bg;
    gcv.function = GXxor;
    sr_xor_gc = XCreateGC(tool_d, sideruler_win, gcmask, &gcv);

    XDefineCursor(tool_d, sideruler_win, (Cursor) ud_arrow_cursor.bitmap);

    sideruler_pm = XCreatePixmap(tool_d, sideruler_win,
				 SIDERULER_WD, SIDERULER_HT,
				 DefaultDepthOfScreen(tool_s));

    reset_sideruler();
}

resize_sideruler()
{
    XFreePixmap(tool_d, sideruler_pm);
    sideruler_pm = XCreatePixmap(tool_d, sideruler_win,
				 SIDERULER_WD, SIDERULER_HT,
				 DefaultDepthOfScreen(tool_s));
    reset_sideruler();
}

reset_sideruler()
{
    register int    i, j;
    register Pixmap p = sideruler_pm;
    char            number[3];
    int             Y0;

    /* side ruler, adjustments for digits are kludges based on 6x13 char */
    XFillRectangle(tool_d, p, sr_erase_gc, 0, 0, SIDERULER_WD,
		   (int) (SIDERULER_HT));

    Y0 = BACKY(0);
    if (appres.INCHES) {
	Y0 -= (Y0 % SINCH);
	if (appres.RHS_PANEL) {
	    for (i = Y0 + SINCH - 1; i <= Y0 + SIDERULER_HT; i += SINCH) {
		j = i + 1;
		if (j % PIX_PER_INCH == 0) {
		    XDrawLine(tool_d, p, sr_gc, RULER_WD - INCH_MARK,
			      ZOOMY(i), RULER_WD, ZOOMY(i));
		    sprintf(number, "%2d", j / PIX_PER_INCH);
		    XDrawString(tool_d, p, sr_gc,
				RULER_WD - INCH_MARK - 14, ZOOMY(i) + 3,
				number, 2);
		} else if (j % HINCH == 0)
		    XDrawLine(tool_d, p, sr_gc,
			      RULER_WD - HALF_MARK, ZOOMY(i),
			      RULER_WD, ZOOMY(i));
		else if (j % QINCH == 0)
		    XDrawLine(tool_d, p, sr_gc,
			      RULER_WD - QUARTER_MARK, ZOOMY(i),
			      RULER_WD, ZOOMY(i));
		else if (j % SINCH == 0)
		    XDrawLine(tool_d, p, sr_gc,
			      RULER_WD - SIXTEENTH_MARK, ZOOMY(i),
			      RULER_WD, ZOOMY(i));
	    }
	} else {
	    for (i = Y0 + SINCH - 1; i <= Y0 + SIDERULER_HT; i += SINCH) {
		j = i + 1;
		if (j % PIX_PER_INCH == 0) {
		    XDrawLine(tool_d, p, sr_gc, 0, ZOOMY(i),
			      INCH_MARK - 1, ZOOMY(i));
		    sprintf(number, "%2d", j / PIX_PER_INCH);
		    XDrawString(tool_d, p, sr_gc, INCH_MARK + 3,
				ZOOMY(i) + 3, number, 2);
		} else if (j % HINCH == 0)
		    XDrawLine(tool_d, p, sr_gc, 0, ZOOMY(i),
			      HALF_MARK - 1, ZOOMY(i));
		else if (j % QINCH == 0)
		    XDrawLine(tool_d, p, sr_gc, 0, ZOOMY(i),
			      QUARTER_MARK - 1, ZOOMY(i));
		else if (j % SINCH == 0)
		    XDrawLine(tool_d, p, sr_gc, 0, ZOOMY(i),
			      SIXTEENTH_MARK - 1, ZOOMY(i));
	    }
	}
    } else {
	Y0 -= (Y0 % TWOMM);
	if (appres.RHS_PANEL) {
	    for (i = Y0 + TWOMM - 1; i <= Y0 + SIDERULER_HT; i++) {
		j = i + 1;
		if (j % PIX_PER_CM == 0) {
		    XDrawLine(tool_d, p, sr_gc, RULER_WD - INCH_MARK,
			      ZOOMY(i), RULER_WD, ZOOMY(i));
		    sprintf(number, "%2d", j / PIX_PER_CM);
		    XDrawString(tool_d, p, sr_gc,
				RULER_WD - INCH_MARK - 14, ZOOMY(i) + 3,
				number, 2);
		} else if (j % TWOMM == 0)
		    XDrawLine(tool_d, p, sr_gc,
			      RULER_WD - QUARTER_MARK, ZOOMY(i),
			      RULER_WD, ZOOMY(i));
	    }
	} else {
	    for (i = Y0 + TWOMM - 1; i <= Y0 + SIDERULER_HT; i++) {
		j = i + 1;
		if (j % PIX_PER_CM == 0) {
		    XDrawLine(tool_d, p, sr_gc, 0, ZOOMY(i),
			      INCH_MARK - 1, ZOOMY(i));
		    sprintf(number, "%2d", j / PIX_PER_CM);
		    XDrawString(tool_d, p, sr_gc, INCH_MARK + 3,
				ZOOMY(i) + 3, number, 2);
		} else if (j % TWOMM == 0)
		    XDrawLine(tool_d, p, sr_gc, 0, ZOOMY(i),
			      QUARTER_MARK - 1, ZOOMY(i));
	    }
	}
    }
    FirstArg(XtNbackgroundPixmap, p);
    SetValues(sideruler_sw);
}

erase_siderulermark()
{
    if (appres.RHS_PANEL)
	XClearArea(tool_d, sideruler_win,
		   RULER_WD + srloffx, ZOOMY(lasty) + srloffy,
		   srlm_pr.width, srlm_pr.height, False);
    else
	XClearArea(tool_d, sideruler_win,
		   srroffx, ZOOMY(lasty) + srroffy,
		   srlm_pr.width, srlm_pr.height, False);
}

set_siderulermark(y)
    int             y;
{
    if (appres.RHS_PANEL) {
	/*
	 * Because the ruler uses a background pixmap, we can win here by
	 * using XClearArea to erase the old thing.
	 */
	XClearArea(tool_d, sideruler_win,
		   RULER_WD + srloffx, ZOOMY(lasty) + srloffy,
		   srlm_pr.width, srlm_pr.height, False);
	XCopyArea(tool_d, sidearrow_pm, sideruler_win,
		  sr_xor_gc, 0, 0, srlm_pr.width,
		  srlm_pr.height, RULER_WD + srloffx, ZOOMY(y) + srloffy);
    } else {
	/*
	 * Because the ruler uses a background pixmap, we can win here by
	 * using XClearArea to erase the old thing.
	 */
	XClearArea(tool_d, sideruler_win,
		   srroffx, ZOOMY(lasty) + srroffy,
		   srlm_pr.width, srlm_pr.height, False);
	XCopyArea(tool_d, sidearrow_pm, sideruler_win,
		  sr_xor_gc, 0, 0, srrm_pr.width,
		  srrm_pr.height, srroffx, ZOOMY(y) + srroffy);
    }
    lasty = y;
}
