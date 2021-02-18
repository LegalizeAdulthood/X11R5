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
#include "object.h"
#include "paintop.h"
#include "w_drawprim.h"
#include "w_icons.h"
#include "w_indpanel.h"
#include "w_util.h"
#include "w_mousefun.h"
#include "w_setup.h"

extern          finish_text_input();
extern          erase_objecthighlight();

extern          circlebyradius_drawing_selected();
extern          circlebydiameter_drawing_selected();
extern          ellipsebyradius_drawing_selected();
extern          ellipsebydiameter_drawing_selected();
extern          box_drawing_selected();
extern          arcbox_drawing_selected();
extern          line_drawing_selected();
extern          regpoly_drawing_selected();
extern          epsbitmap_drawing_selected();
extern          text_drawing_selected();
extern          arc_drawing_selected();
extern          spline_drawing_selected();
extern          intspline_drawing_selected();
extern          align_selected();
extern          compound_selected();
extern          break_selected();
extern          scale_selected();
extern          point_adding_selected();
extern          delete_point_selected();
extern          move_selected();
extern          move_point_selected();
extern          delete_selected();
extern          copy_selected();
extern          rotate_cw_selected();
extern          rotate_ccw_selected();
extern          flip_ud_selected();
extern          flip_lr_selected();
extern          convert_selected();
extern          arrow_head_selected();
extern          edit_item_selected();
extern          update_selected();

/**************     local variables and routines   **************/

#define MAX_MODEMSG_LEN	80
typedef struct mode_switch_struct {
    PIXRECT         icon;
    int             mode;
    int             (*setmode_func) ();
    int             objmask;
    int             indmask;
    char            modemsg[MAX_MODEMSG_LEN];
    TOOL            widget;
    Pixmap          normalPM, reversePM;
}               mode_sw_info;

#define         setmode_action(z)    (z->setmode_func)(z)

DeclareStaticArgs(13);
/* pointer to current mode switch */
static mode_sw_info *current = NULL;

/* button selection event handler */
static void     sel_mode_but();
static void     turn_on();

static mode_sw_info mode_switches[] = {
    {&cirrad_ic, F_CIRCLE_BY_RAD, circlebyradius_drawing_selected, M_NONE,
    I_BOX, "CIRCLE drawing: specify RADIUS",},
    {&cirdia_ic, F_CIRCLE_BY_DIA, circlebydiameter_drawing_selected, M_NONE,
    I_BOX, "CIRCLE drawing: specify DIAMETER",},
    {&ellrad_ic, F_ELLIPSE_BY_RAD, ellipsebyradius_drawing_selected, M_NONE,
    I_BOX, "ELLIPSE drawing: specify RADIUSES",},
    {&elldia_ic, F_ELLIPSE_BY_DIA, ellipsebydiameter_drawing_selected, M_NONE,
    I_BOX, "ELLIPSE drawing: specify DIAMETERS",},
    {&c_spl_ic, F_CLOSED_SPLINE, spline_drawing_selected, M_NONE,
    I_CLOSED, "CLOSED SPLINE drawing: specify control points",},
    {&spl_ic, F_SPLINE, spline_drawing_selected, M_NONE,
    I_OPEN, "SPLINE drawing: specify control points",},
    {&c_intspl_ic, F_CLOSED_INTSPLINE, intspline_drawing_selected, M_NONE,
    I_CLOSED, "CLOSED INTERPOLATED SPLINE drawing",},
    {&intspl_ic, F_INTSPLINE, intspline_drawing_selected, M_NONE,
    I_OPEN, "INTERPOLATED SPLINE drawing",},
    {&polygon_ic, F_POLYGON, line_drawing_selected, M_NONE,
    I_CLOSED, "POLYGON drawing",},
    {&line_ic, F_POLYLINE, line_drawing_selected, M_NONE,
    I_OPEN, "POLYLINE drawing",},
    {&box_ic, F_BOX, box_drawing_selected, M_NONE,
    I_BOX, "Rectangular BOX drawing",},
    {&arc_box_ic, F_ARC_BOX, arcbox_drawing_selected, M_NONE,
    I_ARCBOX, "Rectangular BOX drawing with ROUNDED CORNERS",},
    {&regpoly_ic, F_REGPOLY, regpoly_drawing_selected, M_NONE,
    I_REGPOLY, "Regular Polygon",},
    {&arc_ic, F_CIRCULAR_ARC, arc_drawing_selected, M_NONE,
    I_ARC, "ARC drawing: specify three points on the arc",},
    {&epsbitmap_ic, F_EPSBITMAP, epsbitmap_drawing_selected, M_NONE,
    I_MIN2, "Encapsulated Postscript Bitmap",},
    {&text_ic, F_TEXT, text_drawing_selected, M_TEXT_NORMAL,
    I_TEXT, "TEXT input (from keyboard)",},
    {&glue_ic, F_GLUE, compound_selected, M_COMPOUND,
    I_MIN2, "GLUE objects into COMPOUND object",},
    {&break_ic, F_BREAK, break_selected, M_COMPOUND,
    I_MIN1, "BREAK COMPOUND object",},
    {&scale_ic, F_SCALE, scale_selected, M_NO_TEXT,
    I_MIN2, "SCALE objects",},
    {&align_ic, F_ALIGN, align_selected, M_COMPOUND,
    I_ALIGN, "ALIGN objects within a COMPOUND",},
    {&movept_ic, F_MOVE_POINT, move_point_selected, M_NO_TEXT,
    I_MIN2, "MOVE POINTs",},
    {&move_ic, F_MOVE, move_selected, M_ALL,
    I_MIN2, "MOVE objects",},
    {&addpt_ic, F_ADD_POINT, point_adding_selected, M_VARPTS_OBJECT,
    I_MIN1, "ADD POINTs (to lines, polygons and splines)",},
    {&copy_ic, F_COPY, copy_selected, M_ALL,
    I_MIN2, "COPY objects",},
    {&deletept_ic, F_DELETE_POINT, delete_point_selected, M_VARPTS_OBJECT,
    I_MIN1, "DELETE POINTs (from lines, polygons and splines)",},
    {&delete_ic, F_DELETE, delete_selected, M_ALL,
    I_MIN1, "DELETE objects",},
    {&update_ic, F_UPDATE, update_selected, M_ALL,
    I_OBJECT, "UPDATE object <-> current settings",},
    {&change_ic, F_EDIT, edit_item_selected, M_OBJECT,
    I_MIN1, "CHANGE OBJECT via EDIT pane",},
    {&flip_x_ic, F_FLIP, flip_ud_selected, M_NO_TEXT,
    I_MIN1, "FLIP objects up or down",},
    {&flip_y_ic, F_FLIP, flip_lr_selected, M_NO_TEXT,
    I_MIN1, "FLIP objects left or right",},
    {&rotCW_ic, F_ROTATE, rotate_cw_selected, M_NO_TEXT,
    I_ROTATE, "ROTATE objects clockwise",},
    {&rotCCW_ic, F_ROTATE, rotate_ccw_selected, M_NO_TEXT,
    I_ROTATE, "ROTATE objects counter-clockwise",},
    {&convert_ic, F_CONVERT, convert_selected,
	(M_POLYLINE_LINE | M_POLYLINE_POLYGON | M_SPLINE_INTERP), I_MIN1,
    "CONVERT lines (polygons) into splines (closed-splines) or vice versa",},
    {&autoarrow_ic, F_AUTOARROW, arrow_head_selected, M_OPEN_OBJECT,
    I_MIN1 | I_LINEWIDTH, "ADD/DELETE ARROWs",},
};

#define		NUM_MODE_SW	(sizeof(mode_switches) / sizeof(mode_sw_info))

static Arg      button_args[] =
{
     /* 0 */ {XtNlabel, (XtArgVal) "    "},
     /* 1 */ {XtNwidth, (XtArgVal) 0},
     /* 2 */ {XtNheight, (XtArgVal) 0},
     /* 3 */ {XtNresizable, (XtArgVal) False},
     /* 4 */ {XtNborderWidth, (XtArgVal) 0},
     /* 5 */ {XtNresize, (XtArgVal) False},	/* keeps buttons from being
						 * resized when there are not
						 * a multiple of three of
						 * them */
     /* 6 */ {XtNbackgroundPixmap, (XtArgVal) NULL},
};

static XtActionsRec mode_actions[] =
{
    {"EnterModeSw", (XtActionProc) draw_mousefun_mode},
    {"LeaveModeSw", (XtActionProc) clear_mousefun},
};

static String   mode_translations =
"<EnterWindow>:EnterModeSw()highlight()\n\
    <LeaveWindow>:LeaveModeSw()unhighlight()\n";

int
init_mode_panel(tool)
    TOOL            tool;
{
    register int    i;
    register mode_sw_info *sw;

    FirstArg(XtNwidth, MODEPANEL_WD);
    NextArg(XtNhSpace, INTERNAL_BW);
    NextArg(XtNvSpace, INTERNAL_BW);
    NextArg(XtNtop, XtChainTop);
    NextArg(XtNbottom, XtChainTop);
    NextArg(XtNfromVert, msg_panel);
    NextArg(XtNvertDistance, -INTERNAL_BW);
    NextArg(XtNleft, XtChainLeft);
    NextArg(XtNright, XtChainLeft);
    NextArg(XtNresizable, False);
    NextArg(XtNborderWidth, 0);
    NextArg(XtNmappedWhenManaged, False);

    mode_panel = XtCreateWidget("mode_panel", boxWidgetClass, tool,
				Args, ArgCount);

    XtAppAddActions(tool_app, mode_actions, XtNumber(mode_actions));

    for (i = 0; i < NUM_MODE_SW; ++i) {
	sw = &mode_switches[i];
	if (sw->mode == FIRST_DRAW_MODE) {
	    FirstArg(XtNwidth, MODE_SW_WD * SW_PER_ROW +
		     INTERNAL_BW * (SW_PER_ROW - 1));
	    NextArg(XtNborderWidth, 0);
	    NextArg(XtNresize, False);
	    NextArg(XtNheight, (MODEPANEL_SPACE + 1) / 2);
	    NextArg(XtNlabel, "Drawing\n modes");
	    d_label = XtCreateManagedWidget("label", labelWidgetClass,
					    mode_panel, Args, ArgCount);
	} else if (sw->mode == FIRST_EDIT_MODE) {
	    /* assume Args still set up from d_label */
	    ArgCount -= 2;
	    NextArg(XtNheight, (MODEPANEL_SPACE) / 2);
	    NextArg(XtNlabel, "Editing\n modes");
	    e_label = XtCreateManagedWidget("label", labelWidgetClass,
					    mode_panel, Args, ArgCount);
	}
	button_args[1].value = sw->icon->width;
	button_args[2].value = sw->icon->height;
	sw->widget = XtCreateManagedWidget("button", commandWidgetClass,
			    mode_panel, button_args, XtNumber(button_args));

	/* left button changes mode */
	XtAddEventHandler(sw->widget, ButtonReleaseMask, (Boolean) 0,
			  sel_mode_but, (caddr_t) sw);
	XtOverrideTranslations(sw->widget,
			       XtParseTranslationTable(mode_translations));
    }
    return;
}

/*
 * after panel widget is realized (in main) put some bitmaps etc. in it
 */

setup_mode_panel()
{
    register int    i;
    register mode_sw_info *msw;
    register Display *d = tool_d;
    register Screen *s = tool_s;

    blank_gc = XCreateGC(tool_d, XtWindow(mode_panel), (unsigned long) 0, NULL);
    button_gc = XCreateGC(tool_d, XtWindow(mode_panel), (unsigned long) 0, NULL);
    FirstArg(XtNforeground, &but_fg);
    NextArg(XtNbackground, &but_bg);
    GetValues(mode_switches[0].widget);

    XSetBackground(tool_d, blank_gc, but_bg);
    XSetForeground(tool_d, blank_gc, but_bg);

    FirstArg(XtNfont, button_font);
    SetValues(d_label);
    SetValues(e_label);

    if (appres.INVERSE) {
	FirstArg(XtNbackground, WhitePixelOfScreen(tool_s));
    } else {
    	FirstArg(XtNbackground, BlackPixelOfScreen(tool_s));
    }
    SetValues(mode_panel);

    for (i = 0; i < NUM_MODE_SW; ++i) {
	msw = &mode_switches[i];
	/* create normal bitmaps */
	msw->normalPM = XCreatePixmapFromBitmapData(d, XtWindow(msw->widget),
		       msw->icon->data, msw->icon->width, msw->icon->height,
				   but_fg, but_bg, DefaultDepthOfScreen(s));

	FirstArg(XtNbackgroundPixmap, msw->normalPM);
	SetValues(msw->widget);

	/* create reverse bitmaps */
	msw->reversePM = XCreatePixmapFromBitmapData(d, XtWindow(msw->widget),
		      msw->icon->data, msw->icon->width, msw->icon->height,
				   but_bg, but_fg, DefaultDepthOfScreen(s));
    }

    XDefineCursor(d, XtWindow(mode_panel), (Cursor) arrow_cursor.bitmap);
    FirstArg(XtNmappedWhenManaged, True);
    SetValues(mode_panel);
}

/* come here when a button is pressed in the mode panel */

static void
sel_mode_but(widget, msw, event)
    Widget          widget;
    mode_sw_info   *msw;
    XButtonEvent   *event;
{
    int             new_objmask;

    if (action_on) {
	if (cur_mode == F_TEXT)
	    finish_text_input();/* finish up any text input */
	else {
	    put_msg("FINISH drawing current object first");
	    return;
	}
    } else if (highlighting)
	erase_objecthighlight();
    if (event->button == Button1) {	/* left button */
	turn_off_current();
	turn_on(msw);
	update_indpanel(msw->indmask);
	put_msg(msw->modemsg);
	cur_mode = msw->mode;
	anypointposn = !(msw->indmask & I_POINTPOSN);
	new_objmask = msw->objmask;
	if (cur_mode == F_ROTATE && cur_rotnangle != 90)
	    new_objmask = M_ROTATE_ANGLE;
	update_markers(new_objmask);
	current = msw;
	setmode_action(msw);
    }
}

void
force_positioning()
{
    update_indpanel(current->indmask | I_POINTPOSN);
    anypointposn = 0;
}

void
force_nopositioning()
{
    update_indpanel(current->indmask & ~I_POINTPOSN);
    anypointposn = 1;
}

static void
turn_on(msw)
    mode_sw_info   *msw;
{
    FirstArg(XtNbackgroundPixmap, msw->reversePM);
    SetValues(msw->widget);
}

turn_on_current()
{
    if (current)
	turn_on(current);
}

turn_off_current()
{
    if (current) {
	FirstArg(XtNbackgroundPixmap, current->normalPM);
	SetValues(current->widget);
    }
}
