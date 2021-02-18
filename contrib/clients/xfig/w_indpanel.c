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
#include "object.h"
#include "mode.h"
#include "paintop.h"
#include "u_fonts.h"
#include "w_drawprim.h"
#include "w_icons.h"
#include "w_indpanel.h"
#include "w_mousefun.h"
#include "w_setup.h"
#include "w_util.h"
#include "w_zoom.h"

extern int     *font_ps_sel, *font_latex_sel;	/* ptrs to return font nos */
extern int      (*font_setimage) ();	/* call this function */
extern Widget   font_widget;	/* on this widget */
extern char    *psfont_menu_bits[], *latexfont_menu_bits[];
extern Pixmap   psfont_menu_bitmaps[], latexfont_menu_bitmaps[];
extern struct _fstruct ps_fontinfo[], latex_fontinfo[];
extern char    *panel_get_value();

/**************     local variables and routines   **************/

static int      cur_anglegeom = L_UNCONSTRAINED;
static int      cur_arrowmode = L_NOARROWS;
static int      cur_indmask = I_MIN1;

DeclareStaticArgs(15);

/* declarations for choice buttons */
static int      inc_choice(), dec_choice();
static int      show_valign(), show_halign(), show_textjust();
static int      show_arrowmode(), show_linestyle(), show_anglegeom();
static int      show_pointposn(), show_gridmode();

/* declarations for value buttons */
static int      show_linewidth(), inc_linewidth(), dec_linewidth();
static int      show_boxradius(), inc_boxradius(), dec_boxradius();
static int      show_areafill(), darken_fill(), lighten_fill();
static int      show_font(), inc_font(), dec_font();
static int      show_fontsize(), inc_fontsize(), dec_fontsize();
static int      show_textstep(), inc_textstep(), dec_textstep();
static int      show_zoom(), inc_zoom(), dec_zoom();
static int      show_rotnangle(), inc_rotnangle(), dec_rotnangle();
static int      show_numsides(), inc_numsides(), dec_numsides();

static int      popup_fonts();

static char     indbuf[5];
static int      old_zoomscale = -1;
static int      old_rotnangle = -1;

#define		DEF_IND_SW_HT		32
#define		DEF_IND_SW_WD		64
#define		FONT_IND_SW_WD		(40+PS_FONTPANE_WD)
#define		NARROW_IND_SW_WD	56

/* indicator switch definitions */

static choice_info anglegeom_choices[] = {
    {L_UNCONSTRAINED, &unconstrained_ic,},
    {L_LATEXLINE, &latexline_ic,},
    {L_LATEXARROW, &latexarrow_ic,},
    {L_MOUNTHATTAN, &mounthattan_ic,},
    {L_MANHATTAN, &manhattan_ic,},
    {L_MOUNTAIN, &mountain_ic,},
};

#define	NUM_ANGLEGEOM_CHOICES (sizeof(anglegeom_choices)/sizeof(choice_info))

static choice_info valign_choices[] = {
    {NONE, &none_ic,},
    {TOP, &valignt_ic,},
    {CENTER, &valignc_ic,},
    {BOTTOM, &valignb_ic,},
};

#define	NUM_VALIGN_CHOICES (sizeof(valign_choices)/sizeof(choice_info))

static choice_info halign_choices[] = {
    {NONE, &none_ic,},
    {LEFT, &halignl_ic,},
    {CENTER, &halignc_ic,},
    {RIGHT, &halignr_ic,},
};

#define	NUM_HALIGN_CHOICES (sizeof(halign_choices)/sizeof(choice_info))

static choice_info gridmode_choices[] = {
    {GRID_0, &none_ic,},
    {GRID_1, &grid1_ic,},
    {GRID_2, &grid2_ic,},
};

#define	NUM_GRIDMODE_CHOICES (sizeof(gridmode_choices)/sizeof(choice_info))

static choice_info pointposn_choices[] = {
    {P_ANY, &any_ic,},
    {P_MAGNET, &magnet_ic,},
    {P_GRID1, &grid1_ic,},
    {P_GRID2, &grid2_ic,},
};

#define	NUM_POINTPOSN_CHOICES (sizeof(pointposn_choices)/sizeof(choice_info))

static choice_info arrowmode_choices[] = {
    {L_NOARROWS, &noarrows_ic,},
    {L_FARROWS, &farrows_ic,},
    {L_FBARROWS, &fbarrows_ic,},
    {L_BARROWS, &barrows_ic,},
};

#define	NUM_ARROWMODE_CHOICES (sizeof(arrowmode_choices)/sizeof(choice_info))

static choice_info textjust_choices[] = {
    {T_LEFT_JUSTIFIED, &textL_ic,},
    {T_CENTER_JUSTIFIED, &textC_ic,},
    {T_RIGHT_JUSTIFIED, &textR_ic,},
};

#define	NUM_TEXTJUST_CHOICES (sizeof(textjust_choices)/sizeof(choice_info))

static choice_info linestyle_choices[] = {
    {SOLID_LINE, &solidline_ic,},
    {DASH_LINE, &dashline_ic,},
    {DOTTED_LINE, &dottedline_ic,},
};

#define	NUM_LINESTYLE_CHOICES (sizeof(linestyle_choices)/sizeof(choice_info))

choice_info     areafill_choices[NUMFILLPATS + 1];

#define	I_CHOICE	0
#define	I_IVAL		1
#define	I_FVAL		2	/* textstep is an integer - kludge */

typedef struct ind_sw_struct {
    int             type;	/* one of I_CHOICE .. I_FVAL */
    int             func;
    char            line1[6], line2[6];
    int             sw_width;
    int            *varadr;
    int             (*inc_func) ();
    int             (*dec_func) ();
    int             (*show_func) ();
    choice_info    *choices;	/* specific to I_CHOICE */
    int             numchoices;	/* specific to I_CHOICE */
    int             sw_per_row;	/* specific to I_CHOICE */
    TOOL            widget;
    Pixmap          normalPM;
}               ind_sw_info;

#define         inc_action(z)   (z->inc_func)(z)
#define         dec_action(z)   (z->dec_func)(z)
#define         show_action(z)  (z->show_func)(z)

static ind_sw_info ind_switches[] = {
    {I_IVAL, I_ZOOM, "Zoom", "Scale", NARROW_IND_SW_WD,
    &zoomscale, inc_zoom, dec_zoom, show_zoom,},
    {I_IVAL, I_ROTNANGLE, "Rotn", "Angle", NARROW_IND_SW_WD,
    &cur_rotnangle, inc_rotnangle, dec_rotnangle, show_rotnangle,},
    {I_IVAL, I_NUMSIDES, "Num", "Sides", NARROW_IND_SW_WD,
    &cur_numsides, inc_numsides, dec_numsides, show_numsides,},
    {I_CHOICE, I_GRIDMODE, "Grid", "Mode", DEF_IND_SW_WD,
	&cur_gridmode, inc_choice, dec_choice, show_gridmode,
    gridmode_choices, NUM_GRIDMODE_CHOICES, NUM_GRIDMODE_CHOICES,},
    {I_CHOICE, I_POINTPOSN, "Point", "Posn", DEF_IND_SW_WD,
	&cur_pointposn, inc_choice, dec_choice, show_pointposn,
    pointposn_choices, NUM_POINTPOSN_CHOICES, NUM_POINTPOSN_CHOICES,},
    {I_CHOICE, I_VALIGN, "Vert", "Align", DEF_IND_SW_WD,
	&cur_valign, inc_choice, dec_choice, show_valign,
    valign_choices, NUM_VALIGN_CHOICES, NUM_VALIGN_CHOICES,},
    {I_CHOICE, I_HALIGN, "Horiz", "Align", DEF_IND_SW_WD,
	&cur_halign, inc_choice, dec_choice, show_halign,
    halign_choices, NUM_HALIGN_CHOICES, NUM_HALIGN_CHOICES,},
    {I_CHOICE, I_ANGLEGEOM, "Angle", "Geom", DEF_IND_SW_WD,
	&cur_anglegeom, inc_choice, dec_choice, show_anglegeom,
    anglegeom_choices, NUM_ANGLEGEOM_CHOICES, NUM_ANGLEGEOM_CHOICES / 2,},
    {I_CHOICE, I_FILLAREA, "Area", "Fill", DEF_IND_SW_WD,
	&cur_areafill, darken_fill, lighten_fill, show_areafill,
    areafill_choices, NUMFILLPATS + 1, (NUMFILLPATS + 1) / 2},
    {I_IVAL, I_LINEWIDTH, "Line", "Width", DEF_IND_SW_WD,
    &cur_linewidth, inc_linewidth, dec_linewidth, show_linewidth,},
    {I_CHOICE, I_LINESTYLE, "Line", "Style", DEF_IND_SW_WD,
	&cur_linestyle, inc_choice, dec_choice, show_linestyle,
    linestyle_choices, NUM_LINESTYLE_CHOICES, NUM_LINESTYLE_CHOICES,},
    {I_IVAL, I_BOXRADIUS, "Box", "Curve", DEF_IND_SW_WD,
    &cur_boxradius, inc_boxradius, dec_boxradius, show_boxradius,},
    {I_CHOICE, I_ARROWMODE, "Arrow", "Mode", DEF_IND_SW_WD,
	&cur_arrowmode, inc_choice, dec_choice, show_arrowmode,
    arrowmode_choices, NUM_ARROWMODE_CHOICES, NUM_ARROWMODE_CHOICES,},
    {I_CHOICE, I_TEXTJUST, "Text", "Just", DEF_IND_SW_WD,
	&cur_textjust, inc_choice, dec_choice, show_textjust,
    textjust_choices, NUM_TEXTJUST_CHOICES, NUM_TEXTJUST_CHOICES,},
    {I_IVAL, I_FONTSIZE, "Text", "Size", NARROW_IND_SW_WD,
    &cur_fontsize, inc_fontsize, dec_fontsize, show_fontsize,},
    {I_IVAL, I_TEXTSTEP, "Text", "Step", NARROW_IND_SW_WD,
    &cur_textstep, inc_textstep, dec_textstep, show_textstep,},
    {I_IVAL, I_FONT, "Text", "Font", FONT_IND_SW_WD,
    &cur_ps_font, inc_font, dec_font, show_font,},
};

#define		NUM_IND_SW	(sizeof(ind_switches) / sizeof(ind_sw_info))

static Arg      button_args[] =
{
     /* 0 */ {XtNlabel, (XtArgVal) "        "},
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

/* button selection event handler */
static void     sel_ind_but();

static XtActionsRec ind_actions[] =
{
    {"EnterIndSw", (XtActionProc) draw_mousefun_ind},
    {"LeaveIndSw", (XtActionProc) clear_mousefun},
};

static String   ind_translations =
"<EnterWindow>:EnterIndSw()highlight()\n\
    <LeaveWindow>:LeaveIndSw()unhighlight()\n";

init_ind_panel(tool)
    TOOL            tool;
{
    register int    i;
    register ind_sw_info *sw;

    /* does he want to always see ALL of the indicator buttons? */
    if (appres.ShowAllButtons) {
	cur_indmask = I_ALL;	/* yes */
	i = DEF_IND_SW_HT * 2;	/* two rows high when showing all buttons */
    } else
	i = DEF_IND_SW_HT;

    FirstArg(XtNwidth, INDPANEL_WD);
    NextArg(XtNheight, i);
    NextArg(XtNhSpace, INTERNAL_BW);
    NextArg(XtNvSpace, INTERNAL_BW);
    NextArg(XtNresizable, False);
    NextArg(XtNfromVert, canvas_sw);
    NextArg(XtNvertDistance, -INTERNAL_BW);
    NextArg(XtNtop, XtChainBottom);
    NextArg(XtNbottom, XtChainBottom);
    NextArg(XtNleft, XtChainLeft);
    NextArg(XtNright, XtChainLeft);
    NextArg(XtNborderWidth, 0);
    if (appres.ShowAllButtons) {
	NextArg(XtNorientation, XtorientVertical);	/* use two rows */
    } else {
	NextArg(XtNorientation, XtorientHorizontal);	/* expand horizontally */
    }
    NextArg(XtNmappedWhenManaged, False);

    ind_panel = XtCreateWidget("ind_panel", boxWidgetClass, tool,
			       Args, ArgCount);

    XtAppAddActions(tool_app, ind_actions, XtNumber(ind_actions));

    for (i = 0; i < NUM_IND_SW; ++i) {
	sw = &ind_switches[i];

	button_args[1].value = sw->sw_width;
	button_args[2].value = DEF_IND_SW_HT;
	sw->widget = XtCreateWidget("button", commandWidgetClass,
			     ind_panel, button_args, XtNumber(button_args));
	if (sw->func & cur_indmask)
	    XtManageChild(sw->widget);

	/* allow left & right buttons */
	/* (callbacks pass same data for ANY button) */
	XtAddEventHandler(sw->widget, ButtonReleaseMask, (Boolean) 0,
			  sel_ind_but, (caddr_t) sw);
	XtOverrideTranslations(sw->widget,
			       XtParseTranslationTable(ind_translations));
    }
}

setup_ind_panel()
{
    register int    i;
    register ind_sw_info *isw;
    register Display *d = tool_d;
    register Screen *s = tool_s;
    register Pixmap p;

    /* get the foreground and background from the indicator widget */
    /* and create a gc with those values */
    ind_button_gc = XCreateGC(tool_d, XtWindow(ind_panel), (unsigned long) 0, NULL);
    FirstArg(XtNforeground, &ind_but_fg);
    NextArg(XtNbackground, &ind_but_bg);
    GetValues(ind_switches[0].widget);
    XSetBackground(tool_d, ind_button_gc, ind_but_bg);
    XSetForeground(tool_d, ind_button_gc, ind_but_fg);
    XSetFont(tool_d, ind_button_gc, button_font->fid);

    /* also create gc with fore=background for blanking areas */
    ind_blank_gc = XCreateGC(tool_d, XtWindow(ind_panel), (unsigned long) 0, NULL);
    XSetBackground(tool_d, ind_blank_gc, ind_but_bg);
    XSetForeground(tool_d, ind_blank_gc, ind_but_bg);

    /* initialize the area-fill gc and pixmaps */
    init_fill_pm();
    init_fill_gc();

    FirstArg(XtNbackgroundPixmap, areafill_choices[NUMFILLPATS].normalPM);
    SetValues(ind_panel);

    for (i = 0; i < NUM_IND_SW; ++i) {
	isw = &ind_switches[i];

	p = XCreatePixmap(d, XtWindow(isw->widget), isw->sw_width,
			  DEF_IND_SW_HT, DefaultDepthOfScreen(s));
	XFillRectangle(d, p, ind_blank_gc, 0, 0,
		       isw->sw_width, DEF_IND_SW_HT);
	XDrawImageString(d, p, ind_button_gc, 3, 12, isw->line1, strlen(isw->line1));
	XDrawImageString(d, p, ind_button_gc, 3, 25, isw->line2, strlen(isw->line2));

	isw->normalPM = button_args[6].value = (XtArgVal) p;
	XtSetValues(isw->widget, &button_args[6], 1);
    }

    XDefineCursor(d, XtWindow(ind_panel), (Cursor) arrow_cursor.bitmap);
    update_current_settings();

    FirstArg(XtNmappedWhenManaged, True);
    SetValues(ind_panel);
}

update_indpanel(mask)
    int             mask;
{
    register int    i;
    register ind_sw_info *isw;

    /* only update current mask if user wants to see relevant ind buttons */
    if (appres.ShowAllButtons)
	return;

    cur_indmask = mask;
    XtUnmanageChild(ind_panel);
    for (isw = ind_switches, i = 0; i < NUM_IND_SW; isw++, i++) {
	if (isw->func & cur_indmask) {
	    XtManageChild(isw->widget);
	} else {
	    XtUnmanageChild(isw->widget);
	}
    }
    XtManageChild(ind_panel);
}

/* come here when a button is pressed in the indicator panel */

static void
sel_ind_but(widget, isw, event)
    Widget          widget;
    ind_sw_info    *isw;
    XButtonEvent   *event;
{
    if (event->button == Button3) {	/* right button */
	inc_action(isw);
    } else if (event->button == Button2) {	/* middle button */
	dec_action(isw);
    } else {			/* left button */
	if (isw->func == I_FONT)
	    popup_fonts(isw);
	else if (isw->type == I_IVAL)
	    popup_ival_panel(isw);
	else if (isw->type == I_CHOICE)
	    popup_choice_panel(isw);
    }
}

static
update_string_pixmap(isw, buf, xpos)
    ind_sw_info    *isw;
    char           *buf;
    int             xpos;
{
    XDrawImageString(tool_d, isw->normalPM, ind_button_gc,
		     xpos, 18, buf, strlen(buf));
    /*
     * Fool the toolkit by changing the background pixmap to 0 then giving it
     * the modified one again.  Otherwise, it sees that the pixmap ID is not
     * changed and doesn't actually draw it into the widget window
     */
    button_args[6].value = 0;
    XtSetValues(isw->widget, &button_args[6], 1);

    /* put the pixmap in the widget background */
    button_args[6].value = isw->normalPM;
    XtSetValues(isw->widget, &button_args[6], 1);
}

static
update_choice_pixmap(isw, mode)
    ind_sw_info    *isw;
    int             mode;
{
    choice_info    *tmp_choice;
    int             i;
    register Pixmap p;

    /* put the pixmap in the widget background */
    p = isw->normalPM;
    tmp_choice = isw->choices;
    for (i = mode; i > 0; i--, tmp_choice++);
    XPutImage(tool_d, p, ind_button_gc, tmp_choice->icon, 0, 0, 32, 0, 32, 32);
    /*
     * Fool the toolkit by changing the background pixmap to 0 then giving it
     * the modified one again.  Otherwise, it sees that the pixmap ID is not
     * changed and doesn't actually draw it into the widget window
     */
    button_args[6].value = 0;
    XtSetValues(isw->widget, &button_args[6], 1);
    button_args[6].value = p;
    XtSetValues(isw->widget, &button_args[6], 1);
}

/********************************************************

	auxiliary functions

********************************************************/

static Widget   choice_popup;
static ind_sw_info *choice_i;
static Widget   ival_popup, form, cancel, set, beside, below, newvalue, label;
static Widget   dash_length, dot_gap;
static ind_sw_info *ival_i;


static void
choice_panel_dismiss()
{
    XtDestroyWidget(choice_popup);
    XtSetSensitive(choice_i->widget, True);
}

static void
choice_panel_cancel(w, ev)
    Widget          w;
    XButtonEvent   *ev;
{
    choice_panel_dismiss();
}

static void
choice_panel_set(w, sel_choice, ev)
    Widget          w;
    choice_info    *sel_choice;
    XButtonEvent   *ev;
{
    (*choice_i->varadr) = sel_choice->value;
    show_action(choice_i);

    /* auxiliary info */
    switch (choice_i->func) {
    case I_LINESTYLE:
	/* dash length */
	cur_dashlength = (float) atof(panel_get_value(dash_length));
	if (cur_dashlength <= 0.0)
	    cur_dashlength = DEF_DASHLENGTH;
	/* dot gap */
	cur_dotgap = (float) atof(panel_get_value(dot_gap));
	if (cur_dotgap <= 0.0)
	    cur_dotgap = DEF_DOTGAP;
	break;
    }

    choice_panel_dismiss();
}

popup_choice_panel(isw)
    ind_sw_info    *isw;
{
    Position        x_val, y_val;
    Dimension       width, height;
    char            buf[32];
    choice_info    *tmp_choice;
    Pixmap          p;
    register int    i;

    choice_i = isw;
    XtSetSensitive(choice_i->widget, False);

    FirstArg(XtNwidth, &width);
    NextArg(XtNheight, &height);
    GetValues(tool);
    /* position the popup 1/3 in from left and 2/3 down from top */
    XtTranslateCoords(tool, (Position) (width / 3), (Position) (2 * height / 3),
		      &x_val, &y_val);

    FirstArg(XtNx, x_val);
    NextArg(XtNy, y_val);
    NextArg(XtNresize, False);
    NextArg(XtNresizable, False);

    choice_popup = XtCreatePopupShell("xfig: set indicator panel",
				      transientShellWidgetClass, tool,
				      Args, ArgCount);

    form = XtCreateManagedWidget("form", formWidgetClass, choice_popup, NULL, 0);

    FirstArg(XtNborderWidth, 0);
    sprintf(buf, "%s %s", isw->line1, isw->line2);
    label = XtCreateManagedWidget(buf, labelWidgetClass, form, Args, ArgCount);

    FirstArg(XtNlabel, "cancel");
    NextArg(XtNfromVert, label);
    NextArg(XtNresize, False);
    NextArg(XtNresizable, False);
    NextArg(XtNheight, 32);
    NextArg(XtNborderWidth, INTERNAL_BW);
    cancel = XtCreateManagedWidget("cancel", commandWidgetClass,
				   form, Args, ArgCount);
    XtAddEventHandler(cancel, ButtonReleaseMask, (Boolean) 0,
		      choice_panel_cancel, (XtPointer) NULL);

    tmp_choice = isw->choices;
    for (i = 0; i < isw->numchoices; tmp_choice++, i++) {
	if (isw->func == I_FILLAREA)
	    p = areafill_choices[i].normalPM;
	else
	    p = XCreatePixmapFromBitmapData(tool_d, XtWindow(ind_panel),
			    tmp_choice->icon->data, tmp_choice->icon->width,
			   tmp_choice->icon->height, ind_but_fg, ind_but_bg,
					    DefaultDepthOfScreen(tool_s));
	if (i % isw->sw_per_row == 0) {
	    if (i == 0)
		below = label;
	    else
		below = beside;
	    beside = cancel;
	}
	FirstArg(XtNfromVert, below);
	NextArg(XtNfromHoriz, beside);
	NextArg(XtNbackgroundPixmap, p);
	NextArg(XtNwidth, tmp_choice->icon->width);
	NextArg(XtNresize, False);
	NextArg(XtNresizable, False);
	NextArg(XtNheight, tmp_choice->icon->height);
	NextArg(XtNborderWidth, INTERNAL_BW);
	beside = XtCreateManagedWidget(" ", commandWidgetClass,
				       form, Args, ArgCount);
	XtAddEventHandler(beside, ButtonReleaseMask, (Boolean) 0,
			  choice_panel_set, (caddr_t) tmp_choice);
    }

    /* auxiliary info */
    switch (isw->func) {
    case I_LINESTYLE:
	/* dash length */
	FirstArg(XtNfromVert, beside);
	NextArg(XtNborderWidth, 0);
	label = XtCreateManagedWidget("Default dash length =",
				    labelWidgetClass, form, Args, ArgCount);
	sprintf(buf, "%1.1f", cur_dashlength);
	FirstArg(XtNfromVert, beside);
	NextArg(XtNborderWidth, INTERNAL_BW);
	NextArg(XtNfromHoriz, label);
	NextArg(XtNstring, buf);
	NextArg(XtNinsertPosition, strlen(buf));
	NextArg(XtNeditType, "append");
	NextArg(XtNwidth, 40);
	dash_length = XtCreateManagedWidget(buf, asciiTextWidgetClass,
					    form, Args, ArgCount);
	/* dot gap */
	FirstArg(XtNfromVert, dash_length);
	NextArg(XtNborderWidth, 0);
	label = XtCreateManagedWidget("    Default dot gap =",
				    labelWidgetClass, form, Args, ArgCount);
	sprintf(buf, "%1.1f", cur_dotgap);
	FirstArg(XtNfromVert, dash_length);
	NextArg(XtNborderWidth, INTERNAL_BW);
	NextArg(XtNfromHoriz, label);
	NextArg(XtNstring, buf);
	NextArg(XtNinsertPosition, strlen(buf));
	NextArg(XtNeditType, "append");
	NextArg(XtNwidth, 40);
	dot_gap = XtCreateManagedWidget(buf, asciiTextWidgetClass,
					form, Args, ArgCount);
	break;
    }

    XtPopup(choice_popup, XtGrabExclusive);
}

static void
ival_panel_dismiss()
{
    XtDestroyWidget(ival_popup);
    XtSetSensitive(ival_i->widget, True);
}

static void
ival_panel_cancel(w, ev)
    Widget          w;
    XButtonEvent   *ev;
{
    ival_panel_dismiss();
}

static void
ival_panel_set(w, ev)
    Widget          w;
    XButtonEvent   *ev;
{
    int             new_value;

    new_value = atoi(panel_get_value(newvalue));
    ival_panel_dismiss();

    (*ival_i->varadr) = new_value;
    show_action(ival_i);
}

popup_ival_panel(isw)
    ind_sw_info    *isw;
{
    Position        x_val, y_val;
    Dimension       width, height;
    char            buf[32];

    ival_i = isw;
    XtSetSensitive(ival_i->widget, False);

    FirstArg(XtNwidth, &width);
    NextArg(XtNheight, &height);
    GetValues(tool);
    /* position the popup 1/3 in from left and 2/3 down from top */
    XtTranslateCoords(tool, (Position) (width / 3), (Position) (2 * height / 3),
		      &x_val, &y_val);

    FirstArg(XtNx, x_val);
    NextArg(XtNy, y_val);
    NextArg(XtNwidth, 240);

    ival_popup = XtCreatePopupShell("xfig: set indicator panel",
				    transientShellWidgetClass, tool,
				    Args, ArgCount);

    form = XtCreateManagedWidget("form", formWidgetClass, ival_popup, NULL, 0);

    FirstArg(XtNborderWidth, 0);
    sprintf(buf, "%s %s", isw->line1, isw->line2);
    label = XtCreateManagedWidget(buf, labelWidgetClass, form, Args, ArgCount);

    FirstArg(XtNfromVert, label);
    NextArg(XtNborderWidth, 0);
    newvalue = XtCreateManagedWidget("Value =", labelWidgetClass,
				     form, Args, ArgCount);
    sprintf(buf, "%d", (*isw->varadr));
    FirstArg(XtNfromVert, label);
    NextArg(XtNborderWidth, INTERNAL_BW);
    NextArg(XtNfromHoriz, newvalue);
    NextArg(XtNstring, buf);
    NextArg(XtNinsertPosition, strlen(buf));
    NextArg(XtNeditType, "append");
    NextArg(XtNwidth, 40);
    newvalue = XtCreateManagedWidget(buf, asciiTextWidgetClass,
				     form, Args, ArgCount);

    FirstArg(XtNlabel, "cancel");
    NextArg(XtNfromVert, newvalue);
    NextArg(XtNborderWidth, INTERNAL_BW);
    cancel = XtCreateManagedWidget("cancel", commandWidgetClass,
				   form, Args, ArgCount);
    XtAddEventHandler(cancel, ButtonReleaseMask, (Boolean) 0,
		      ival_panel_cancel, (XtPointer) NULL);

    FirstArg(XtNlabel, "set");
    NextArg(XtNfromVert, newvalue);
    NextArg(XtNfromHoriz, cancel);
    NextArg(XtNborderWidth, INTERNAL_BW);
    set = XtCreateManagedWidget("set", commandWidgetClass,
				form, Args, ArgCount);
    XtAddEventHandler(set, ButtonReleaseMask, (Boolean) 0,
		      ival_panel_set, (XtPointer) NULL);

    XtPopup(ival_popup, XtGrabExclusive);
}

/********************************************************

	commands to change indicator settings

********************************************************/

update_current_settings()
{
    int             i;
    ind_sw_info    *isw;

    for (i = 0; i < NUM_IND_SW; ++i) {
	isw = &ind_switches[i];
	show_action(isw);
    }
}

static
dec_choice(sw)
    ind_sw_info    *sw;
{
    if (--(*sw->varadr) < 0)
	(*sw->varadr) = sw->numchoices - 1;
    show_action(sw);
}

static
inc_choice(sw)
    ind_sw_info    *sw;
{
    if (++(*sw->varadr) > sw->numchoices - 1)
	(*sw->varadr) = 0;
    show_action(sw);
}

/* ARROW MODE		 */

static
show_arrowmode(sw)
    ind_sw_info    *sw;
{
    update_choice_pixmap(sw, cur_arrowmode);
    switch (cur_arrowmode) {
    case L_NOARROWS:
	autobackwardarrow_mode = 0;
	autoforwardarrow_mode = 0;
	put_msg("NO ARROWS");
	break;
    case L_FARROWS:
	autobackwardarrow_mode = 0;
	autoforwardarrow_mode = 1;
	put_msg("Auto FORWARD ARROWS (for ARC, POLYLINE and SPLINE)");
	break;
    case L_FBARROWS:
	autobackwardarrow_mode = 1;
	autoforwardarrow_mode = 1;
	put_msg("Auto FORWARD and BACKWARD ARROWS (for ARC, POLYLINE and SPLINE)");
	break;
    case L_BARROWS:
	autobackwardarrow_mode = 1;
	autoforwardarrow_mode = 0;
	put_msg("Auto BACKWARD ARROWS (for ARC, POLYLINE and SPLINE)");
	break;
    }
}

/* LINE WIDTH		 */

#define MAXLINEWIDTH 200

static
dec_linewidth(sw)
    ind_sw_info    *sw;
{
    --cur_linewidth;
    show_linewidth(sw);
}

static
inc_linewidth(sw)
    ind_sw_info    *sw;
{
    ++cur_linewidth;
    show_linewidth(sw);
}

static
show_linewidth(sw)
    ind_sw_info    *sw;
{
    if (cur_linewidth > MAXLINEWIDTH)
	cur_linewidth = MAXLINEWIDTH;
    else if (cur_linewidth < 0)
	cur_linewidth = 0;

    /* erase by drawing wide, inverted (white) line */
    pw_vector(sw->normalPM, DEF_IND_SW_WD / 2, DEF_IND_SW_HT / 2,
	      DEF_IND_SW_WD, DEF_IND_SW_HT / 2, ERASE,
	      DEF_IND_SW_HT, PANEL_LINE, 0.0);
    /* draw current line thickness into pixmap */
    if (cur_linewidth > 0)	/* don't draw line for zero-thickness */
	pw_vector(sw->normalPM, DEF_IND_SW_WD / 2, DEF_IND_SW_HT / 2,
		  DEF_IND_SW_WD, DEF_IND_SW_HT / 2, PAINT,
		  cur_linewidth, PANEL_LINE, 0.0);

    /*
     * Fool the toolkit by changing the background pixmap to 0 then giving it
     * the modified one again.  Otherwise, it sees that the pixmap ID is not
     * changed and doesn't actually draw it into the widget window
     */
    button_args[6].value = 0;
    XtSetValues(sw->widget, &button_args[6], 1);
    /* put the pixmap in the widget background */
    button_args[6].value = (XtArgVal) sw->normalPM;
    XtSetValues(sw->widget, &button_args[6], 1);
    put_msg("LINE Thickness = %d", cur_linewidth);
}

/* ANGLE GEOMETRY		 */

static
show_anglegeom(sw)
    ind_sw_info    *sw;
{
    update_choice_pixmap(sw, cur_anglegeom);
    switch (cur_anglegeom) {
    case L_UNCONSTRAINED:
	manhattan_mode = 0;
	mountain_mode = 0;
	latexline_mode = 0;
	latexarrow_mode = 0;
	put_msg("UNCONSTRAINED geometry (for POLYLINE and SPLINE)");
	break;
    case L_MOUNTHATTAN:
	mountain_mode = 1;
	manhattan_mode = 1;
	latexline_mode = 0;
	latexarrow_mode = 0;
	put_msg("MOUNT-HATTAN geometry (for POLYLINE and SPLINE)");
	break;
    case L_MANHATTAN:
	manhattan_mode = 1;
	mountain_mode = 0;
	latexline_mode = 0;
	latexarrow_mode = 0;
	put_msg("MANHATTAN geometry (for POLYLINE and SPLINE)");
	break;
    case L_MOUNTAIN:
	mountain_mode = 1;
	manhattan_mode = 0;
	latexline_mode = 0;
	latexarrow_mode = 0;
	put_msg("MOUNTAIN geometry (for POLYLINE and SPLINE)");
	break;
    case L_LATEXLINE:
	latexline_mode = 1;
	manhattan_mode = 0;
	mountain_mode = 0;
	latexarrow_mode = 0;
	put_msg("LATEX LINE geometry: allow only LaTeX line slopes");
	break;
    case L_LATEXARROW:
	latexarrow_mode = 1;
	manhattan_mode = 0;
	mountain_mode = 0;
	latexline_mode = 0;
	put_msg("LATEX ARROW geometry: allow only LaTeX arrow slopes");
	break;
    }
}

/* LINE STYLE		 */

static
show_linestyle(sw)
    ind_sw_info    *sw;
{
    update_choice_pixmap(sw, cur_linestyle);
    switch (cur_linestyle) {
    case SOLID_LINE:
	cur_styleval = 0.0;
	put_msg("SOLID LINE STYLE (for BOX, POLYGON and POLYLINE)");
	break;
    case DASH_LINE:
	cur_styleval = cur_dashlength;
	put_msg("DASH LINE STYLE (for BOX, POLYGON and POLYLINE)");
	break;
    case DOTTED_LINE:
	cur_styleval = cur_dotgap;
	put_msg("DOTTED LINE STYLE (for BOX, POLYGON and POLYLINE)");
	break;
    }
}

/* VERTICAL ALIGNMENT	 */

static
show_valign(sw)
    ind_sw_info    *sw;
{
    update_choice_pixmap(sw, cur_valign);
    switch (cur_valign) {
    case NONE:
	put_msg("No vertical alignment");
	break;
    case TOP:
	put_msg("Vertically align to TOP");
	break;
    case CENTER:
	put_msg("Center vertically when aligning");
	break;
    case BOTTOM:
	put_msg("Vertically align to BOTTOM");
	break;
    }
}

/* HORIZ ALIGNMENT	 */

static
show_halign(sw)
    ind_sw_info    *sw;
{
    update_choice_pixmap(sw, cur_halign);
    switch (cur_halign) {
    case NONE:
	put_msg("No horizontal alignment");
	break;
    case LEFT:
	put_msg("Horizontally align to LEFT");
	break;
    case CENTER:
	put_msg("Center horizontally when aligning");
	break;
    case RIGHT:
	put_msg("Horizontally align to RIGHT");
	break;
    }
}

/* GRID MODE	 */

static
show_gridmode(sw)
    ind_sw_info    *sw;
{
    static int      prev_gridmode = -1;

    update_choice_pixmap(sw, cur_gridmode);
    switch (cur_gridmode) {
    case GRID_0:
	put_msg("No grid");
	break;
    case GRID_1:
	put_msg("Small grid");
	break;
    case GRID_2:
	put_msg("Large grid");
	break;
    }
    if (cur_gridmode != prev_gridmode)
	setup_grid(cur_gridmode);
    prev_gridmode = cur_gridmode;
}

/* POINT POSITION	 */

static
show_pointposn(sw)
    ind_sw_info    *sw;
{
    update_choice_pixmap(sw, cur_pointposn);
    switch (cur_pointposn) {
    case P_ANY:
	put_msg("Arbitrary Positioning of Points");
	break;
    case P_MAGNET:
	put_msg("MAGNET MODE: entered points rounded to the nearest %s increment",
		(appres.INCHES ? "1/16\"" : "2 mm"));
	break;
    case P_GRID1:
	put_msg("Points rounded to small grid positions");
	break;
    case P_GRID2:
	put_msg("Points rounded to large grid positions");
	break;
    }
}

/* TEXT JUSTIFICATION	 */

static
show_textjust(sw)
    ind_sw_info    *sw;
{
    update_choice_pixmap(sw, cur_textjust);
    switch (cur_textjust) {
    case T_LEFT_JUSTIFIED:
	put_msg("Left justify text");
	break;
    case T_CENTER_JUSTIFIED:
	put_msg("Center text");
	break;
    case T_RIGHT_JUSTIFIED:
	put_msg("Right justify text");
	break;
    }
}

/* BOX RADIUS	 */

static
dec_boxradius(sw)
    ind_sw_info    *sw;
{
    --cur_boxradius;
    show_boxradius(sw);
}

static
inc_boxradius(sw)
    ind_sw_info    *sw;
{
    ++cur_boxradius;
    show_boxradius(sw);
}

#define MAXRADIUS 30
static
show_boxradius(sw)
    ind_sw_info    *sw;
{
    if (cur_boxradius > MAXRADIUS)
	cur_boxradius = MAXRADIUS;
    else if (cur_boxradius < 3)
	cur_boxradius = 3;
    /* erase by drawing wide, inverted (white) line */
    pw_vector(sw->normalPM, DEF_IND_SW_WD / 2, DEF_IND_SW_HT / 2,
	      DEF_IND_SW_WD, DEF_IND_SW_HT / 2, ERASE,
	      DEF_IND_SW_HT, PANEL_LINE, 0.0);
    /* draw current radius into pixmap */
    curve(sw->normalPM, 0, cur_boxradius, -cur_boxradius, 0, 1,
	  cur_boxradius, cur_boxradius,
	  DEF_IND_SW_WD - 2, DEF_IND_SW_HT - 2, PAINT, 1, PANEL_LINE, 0.0, 0);

    /*
     * Fool the toolkit by changing the background pixmap to 0 then giving it
     * the modified one again.  Otherwise, it sees that the pixmap ID is not
     * changed and doesn't actually draw it into the widget window
     */
    button_args[6].value = 0;
    XtSetValues(sw->widget, &button_args[6], 1);
    /* put the pixmap in the widget background */
    button_args[6].value = (XtArgVal) sw->normalPM;
    XtSetValues(sw->widget, &button_args[6], 1);
    put_msg("ROUNDED-CORNER BOX Radius = %d", cur_boxradius);
}

/* AREA FILL */

static
darken_fill(sw)
    ind_sw_info    *sw;
{
    if (++cur_areafill > NUMFILLPATS)
	cur_areafill = 0;
    show_areafill(sw);
}

static
lighten_fill(sw)
    ind_sw_info    *sw;
{
    if (--cur_areafill < 0)
	cur_areafill = NUMFILLPATS;
    show_areafill(sw);
}

static
show_areafill(sw)
    ind_sw_info    *sw;
{
    if (cur_areafill == 0) {
	XCopyArea(tool_d, areafill_choices[0].normalPM, sw->normalPM,
		  ind_button_gc, 0, 0, 32, 32, 32, 0);
	put_msg("NO-FILL MODE");
    } else {
	/* put the pixmap in the widget background */
	XCopyArea(tool_d, areafill_choices[cur_areafill].normalPM, sw->normalPM,
		  ind_button_gc, 0, 0, 26, 24, 35, 4);
	put_msg("FILL MODE (density = %d%%)",
		((cur_areafill - 1) * 100) / (NUMFILLPATS - 1));
    }
    button_args[6].value = 0;
    XtSetValues(sw->widget, &button_args[6], 1);
    button_args[6].value = (XtArgVal) sw->normalPM;
    XtSetValues(sw->widget, &button_args[6], 1);
}

/* FONT */

static
inc_font(sw)
    ind_sw_info    *sw;
{
    if (using_ps)
	cur_ps_font++;
    else
	cur_latex_font++;
    show_font(sw);
}

static
dec_font(sw)
    ind_sw_info    *sw;
{
    if (using_ps)
	cur_ps_font--;
    else
	cur_latex_font--;
    show_font(sw);
}

static
show_font(sw)
    ind_sw_info    *sw;
{
    if (using_ps) {
	if (cur_ps_font >= NUM_PS_FONTS)
	    cur_ps_font = 0;
	else if (cur_ps_font < 0)
	    cur_ps_font = NUM_PS_FONTS - 1;
    } else {
	if (cur_latex_font >= NUM_LATEX_FONTS)
	    cur_latex_font = 0;
	else if (cur_latex_font < 0)
	    cur_latex_font = NUM_LATEX_FONTS - 1;
    }

    /* erase larger fontpane bits if we switched to smaller (Latex) */
    XFillRectangle(tool_d, sw->normalPM, ind_blank_gc, 0, 0,
		   32 + max2(PS_FONTPANE_WD, LATEX_FONTPANE_WD), DEF_IND_SW_HT);
    /* and redraw info */
    XDrawImageString(tool_d, sw->normalPM, ind_button_gc, 3, 12, sw->line1,
		     strlen(sw->line1));
    XDrawImageString(tool_d, sw->normalPM, ind_button_gc, 3, 25, sw->line2,
		     strlen(sw->line2));

    XCopyArea(tool_d, using_ps ? psfont_menu_bitmaps[cur_ps_font] :
	      latexfont_menu_bitmaps[cur_latex_font],
	      sw->normalPM, ind_button_gc, 0, 0,
	      using_ps ? PS_FONTPANE_WD : LATEX_FONTPANE_WD,
	      using_ps ? PS_FONTPANE_HT : LATEX_FONTPANE_HT,
	  using_ps ? 32 : 32 + (PS_FONTPANE_WD - LATEX_FONTPANE_WD) / 2, 6);

    button_args[6].value = 0;
    XtSetValues(sw->widget, &button_args[6], 1);
    /* put the pixmap in the widget background */
    button_args[6].value = (XtArgVal) sw->normalPM;
    XtSetValues(sw->widget, &button_args[6], 1);
    put_msg("Font: %s", using_ps ? ps_fontinfo[cur_ps_font].name :
	    latex_fontinfo[cur_latex_font].name);
}

/* popup menu of printer fonts */

static int      psflag;
static ind_sw_info *return_sw;

int    show_font_return();

static
popup_fonts(sw)
    ind_sw_info    *sw;
{
    return_sw = sw;
    psflag = using_ps ? 1 : 0;
    fontpane_popup(&cur_ps_font, &cur_latex_font, &psflag,
		   show_font_return, sw->widget);
}

show_font_return(w)
    Widget          w;
{
    if (psflag)
	cur_textflags = cur_textflags | PSFONT_TEXT;
    else
	cur_textflags = cur_textflags & (~PSFONT_TEXT);
    show_font(return_sw);
}

/* increase font size */

static
inc_fontsize(sw)
    ind_sw_info    *sw;
{
    if (cur_fontsize >= 100) {
	cur_fontsize = (cur_fontsize / 10) * 10;	/* round first */
	cur_fontsize += 10;
    } else if (cur_fontsize >= 50) {
	cur_fontsize = (cur_fontsize / 5) * 5;
	cur_fontsize += 5;
    } else if (cur_fontsize >= 20) {
	cur_fontsize = (cur_fontsize / 2) * 2;
	cur_fontsize += 2;
    } else
	cur_fontsize++;
    show_fontsize(sw);
}


/* decrease font size */

static
dec_fontsize(sw)
    ind_sw_info    *sw;
{
    if (cur_fontsize > 100) {
	cur_fontsize = (cur_fontsize / 10) * 10;	/* round first */
	cur_fontsize -= 10;
    } else if (cur_fontsize > 50) {
	cur_fontsize = (cur_fontsize / 5) * 5;
	cur_fontsize -= 5;
    } else if (cur_fontsize > 20) {
	cur_fontsize = (cur_fontsize / 2) * 2;
	cur_fontsize -= 2;
    } else if (cur_fontsize > 4)
	cur_fontsize--;
    show_fontsize(sw);
}

static
show_fontsize(sw)
    ind_sw_info    *sw;
{
    if (cur_fontsize < 0)
	cur_fontsize = 0;
    else if (cur_fontsize > 1000)
	cur_fontsize = 1000;

    put_msg("Font size %d", cur_fontsize);
    /* write the font size in the background pixmap */
    indbuf[0] = indbuf[1] = indbuf[2] = indbuf[3] = indbuf[4] = '\0';
    sprintf(indbuf, "%4d", cur_fontsize);
    update_string_pixmap(sw, indbuf, 28);
}

static
inc_rotnangle(sw)
    ind_sw_info    *sw;
{
    if (cur_rotnangle < 30 || cur_rotnangle >= 120)
	cur_rotnangle = 30;
    else if (cur_rotnangle < 45)
	cur_rotnangle = 45;
    else if (cur_rotnangle < 60)
	cur_rotnangle = 60;
    else if (cur_rotnangle < 90)
	cur_rotnangle = 90;
    else if (cur_rotnangle < 120)
	cur_rotnangle = 120;
    show_rotnangle(sw);
}

static
dec_rotnangle(sw)
    ind_sw_info    *sw;
{
    if (cur_rotnangle > 120 || cur_rotnangle <= 30)
	cur_rotnangle = 120;
    else if (cur_rotnangle > 90)
	cur_rotnangle = 90;
    else if (cur_rotnangle > 60)
	cur_rotnangle = 60;
    else if (cur_rotnangle > 45)
	cur_rotnangle = 45;
    else if (cur_rotnangle > 30)
	cur_rotnangle = 30;
    show_rotnangle(sw);
}

static
show_rotnangle(sw)
    ind_sw_info    *sw;
{
    if (cur_rotnangle < 1)
	cur_rotnangle = 1;
    else if (cur_rotnangle > 180)
	cur_rotnangle = 180;

    put_msg("Angle of rotation %d", cur_rotnangle);
    if (cur_rotnangle == old_rotnangle)
	return;

    /* write the font size in the background pixmap */
    indbuf[0] = indbuf[1] = indbuf[2] = indbuf[3] = indbuf[4] = '\0';
    sprintf(indbuf, "%3d", cur_rotnangle);
    update_string_pixmap(sw, indbuf, 34);

    /* change markers if we changed to or from 90 degrees (except at start) */
    if (old_rotnangle != -1) {
	if (cur_rotnangle == 90)
	    update_markers(M_NO_TEXT);
	else if (old_rotnangle == 90)
	    update_markers(M_ROTATE_ANGLE);
    }
    old_rotnangle = cur_rotnangle;
}

/* NUMSIDES */

static
inc_numsides(sw)
    ind_sw_info    *sw;
{
    cur_numsides++;
    show_numsides(sw);
}

static
dec_numsides(sw)
    ind_sw_info    *sw;
{
    cur_numsides--;
    show_numsides(sw);
}

static
show_numsides(sw)
    ind_sw_info    *sw;
{
    if (cur_numsides < 3)
	cur_numsides = 3;
    else if (cur_numsides > 99)
	cur_numsides = 99;

    put_msg("Number of sides %2d", cur_numsides);
    /* write the font size in the background pixmap */
    indbuf[0] = indbuf[1] = indbuf[2] = indbuf[3] = indbuf[4] = '\0';
    sprintf(indbuf, "%2d", cur_numsides);
    update_string_pixmap(sw, indbuf, 38);
}

/* ZOOM */

static
inc_zoom(sw)
    ind_sw_info    *sw;
{
    zoomscale++;
    show_zoom(sw);
}

static
dec_zoom(sw)
    ind_sw_info    *sw;
{
    zoomscale--;
    show_zoom(sw);
}

static
show_zoom(sw)
    ind_sw_info    *sw;
{
    if (zoomscale < 1)
	zoomscale = 1;
    else if (zoomscale > 10)
	zoomscale = 10;

    put_msg("Zoom scale %2d", zoomscale);
    if (zoomscale == old_zoomscale)
	return;

    /* write the font size in the background pixmap */
    indbuf[0] = indbuf[1] = indbuf[2] = indbuf[3] = indbuf[4] = '\0';
    sprintf(indbuf, "%2d", zoomscale);
    update_string_pixmap(sw, indbuf, 38);

    /* fix up the rulers and grid */
    reset_rulers();
    redisplay_rulers();
    setup_grid(cur_gridmode);
    old_zoomscale = zoomscale;
}

/* TEXTSTEP */

static
inc_textstep(sw)
    ind_sw_info    *sw;
{
    if (cur_textstep >= 100) {
	cur_textstep = (cur_textstep / 10) * 10;	/* round first */
	cur_textstep += 10;
    } else if (cur_textstep >= 50) {
	cur_textstep = (cur_textstep / 5) * 5;
	cur_textstep += 5;
    } else if (cur_textstep >= 20) {
	cur_textstep = (cur_textstep / 2) * 2;
	cur_textstep += 2;
    } else
	cur_textstep++;
    show_textstep(sw);
}

static
dec_textstep(sw)
    ind_sw_info    *sw;
{
    if (cur_textstep > 100) {
	cur_textstep = (cur_textstep / 10) * 10;	/* round first */
	cur_textstep -= 10;
    } else if (cur_textstep > 50) {
	cur_textstep = (cur_textstep / 5) * 5;
	cur_textstep -= 5;
    } else if (cur_textstep > 20) {
	cur_textstep = (cur_textstep / 2) * 2;
	cur_textstep -= 2;
    } else if (cur_textstep > 4)
	cur_textstep--;
    show_textstep(sw);
}

/* could make this more generic - but a copy will do for font set JNT */
static
show_textstep(sw)
    ind_sw_info    *sw;
{
    if (cur_textstep < 0)
	cur_textstep = 0;
    else if (cur_textstep > 990)
	cur_textstep = 990;

    put_fmsg("Font step %.1f", (double) cur_textstep / 10.0);
    /* write the font size in the background pixmap */
    indbuf[0] = indbuf[1] = indbuf[2] = indbuf[3] = indbuf[4] = '\0';
    sprintf(indbuf, "%4.1f", cur_textstep / 10.0);
    update_string_pixmap(sw, indbuf, 28);
}
