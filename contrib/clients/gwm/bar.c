/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*********************************************\
* 					      *
* 	BULL WINDOW MANAGER for X11 .	      *
* 					      *
* 	MODULE defining the Bar Wob class.    *
* 					      *
\*********************************************/

 /* include */
#include	"EXTERN.h"
#include	"wool.h"
#include	"wl_atom.h"
#include	"gwm.h"
#include	"wl_fsm.h"
#include	"wl_pixmap.h"
#include	"wl_cursor.h"
#include	"wl_bar.h"

 /* local constants */

 /* external */
extern Wob      NewWob();
extern Plug     NewPlug();
extern WOOL_METHOD WLMenu[];
extern Bar BarOpen();
extern BarEventHandler(), BarClose(), UpdateBarGeometry(), ReconfigureBar();

WOB_METHOD       BarClass[] = {
			      0,	/* METHODS_ARRAY */
			      WobEval,
			      WobPrint,
			      WobRelease,
			      WobExecute,
			      WobSet,
			      WobGetCValue,
			      (WOB_METHOD) BarOpen,
			      BarClose,
			      BarEventHandler,
                              (WOB_METHOD) wool_undefined_method_1,
			      WobGetDimensions,
                              (WOB_METHOD) wool_undefined_method_2,
                              (WOB_METHOD) wool_undefined_method_2,
                              ReconfigureBar,
			(WOB_METHOD) wool_undefined_method_2,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1
};

 /* routines */

/*
 * NewBar
 * Creates a new bar object from a WOOL_Bar description
 * Warning: a plug may be NULL to indicate an extensible space
 */

Bar 
NewBar(client, wl_bar)
ClientWindow             client;
WOOL_Bar		wl_bar;
{
    Bar             bar = (Bar) NewWob(sizeof(struct _Bar)
		       + sizeof(Plug) * Max(0, (wl_bar -> plugs_size - 1)));
    int             i;
    WOOL_OBJECT     object;

    wl_bar = (WOOL_Bar) wool_type_or_evaluate(wl_bar, WLBar);
    bar -> type = BarClass;
    bar -> parent = (Wob) client;

    /* set up the box info */
    bar -> box.width = bar -> box.height = wl_bar -> min_width;
    bar -> min_width = wl_bar -> min_width;
    bar -> max_width = wl_bar -> max_width;
    bar -> box.borderwidth = wl_bar -> borderwidth;
    bar -> box.borderpixel = wl_bar -> borderpixel;
    bar -> box.background = wl_bar -> background;
    bar -> plug_separator = wl_bar -> plug_separator;
    increase_reference(bar -> menu = 
                       wool_type_or_evaluate(wl_bar -> menu, WLMenu));
    increase_reference(bar -> property = (WOOL_OBJECT) wl_bar -> property);
    increase_reference(bar -> bordertile =
		     wool_type_or_evaluate(wl_bar -> bordertile, WLPixmap));
    increase_reference(bar -> fsm =
		       wool_type_or_evaluate(wl_bar -> fsm, WLFsm));
    increase_reference(bar -> cursor =
		       wool_type_or_evaluate(wl_bar -> cursor, WLCursor));
    increase_reference(bar -> tile =
		       wool_type_or_evaluate(wl_bar -> tile, WLPixmap));

    /* then recursively sets plug infos */
    bar -> nplugs = wl_bar -> plugs_size;
    for (i = 0; i < wl_bar -> plugs_size; i++) {
	object = wool_type_or_evaluate(wl_bar -> plugs[i], WLPlug);
	bar -> plugs[i] = (Wob) NewPlug(bar, object);
    }
    return bar;
}

BarClose(bar)
Bar             bar;
{
    int             i;

    if(!bar) return;
    for (i = 0; i < bar -> nplugs; i++) {
	if (bar -> plugs[i])
	    PlugClose(bar -> plugs[i]);
    }
    WobRelease(bar);
}

/*
 * Open a Bar
 */

Bar
BarOpen(bar)
Bar             bar;
{
    int             i;

    if(!bar)
    	return bar;
    check_window_size(bar);
    bar -> hook = XCreateSimpleWindow(dpy, bar -> parent -> hook,
				      bar -> box.x, bar -> box.y,
				      bar -> box.width, bar -> box.height,
				      bar -> box.borderwidth,
				      bar -> box.borderpixel,
				      bar -> box.background);
    if (bar -> parent -> type == ScreenClass)
	bar -> status |= TopLevelXWindowStatus;
    WobRecordHook(bar);
    if (bar -> cursor != NIL)
	XDefineCursor(dpy, bar -> hook,
		      ((WOOL_Cursor) bar -> cursor) -> cursor);
    if (bar -> tile != NIL)
	XSetWindowBackgroundPixmap(dpy, bar -> hook,
				   ((WOOL_Pixmap) bar -> tile) -> pixmap);
    if (bar -> bordertile != NIL)
	XSetWindowBorderPixmap(dpy, bar -> hook,
		      ((WOOL_Pixmap) bar -> bordertile) -> pixmap);

    bar -> curstate = (int) WOOL_send(WOOL_open, bar -> fsm, (bar -> fsm));
    for (i = 0; i < bar -> nplugs; i++) {
	if (bar -> plugs[i])
	    WOOL_send(WOOL_open, bar -> plugs[i], (bar -> plugs[i]));
    }
    bar -> input_mask = WobMask | ((WOOL_Fsm) bar -> fsm) -> mask;
    XSelectInput(dpy, bar -> hook, bar -> input_mask);
    XMapWindow(dpy, bar -> hook);
    XMapSubwindows(dpy, bar -> hook);
    return bar;
}

/*
 * Called BEFORE adjusting the client
 * Here we take a bar setup, and suppose that its box data is updated.
 * Then we ask for the dimension of plugs and proceed to position them.
 * Adjust the width of the bars
 */

int
UpdateBarWidth(bar)
Bar		bar;
{
    Plug  *plugs;
    int             i;

    if (!bar)
	return 0;
    plugs = (Plug *) & ((bar -> plugs)[0]);
    bar -> box.height = bar -> box.width = 0;
    /* bar with no plugs have size of tile */
    if (!bar -> nplugs && bar -> tile != NIL) {	
	bar -> box.height = ((WOOL_Pixmap) bar -> tile) -> height;
	bar -> box.width = ((WOOL_Pixmap) bar -> tile) -> width;
    }
	
    for (i = 0; i < bar -> nplugs; i++) {
	if (plugs[i]) {
	    UpdatePlugGeometry(plugs[i]);
	    if (bar -> direction == HORIZONTAL) {
		bar -> box.height = Max(bar -> box.height,
		  plugs[i] -> box.height + 2 * plugs[i] -> box.borderwidth);
	    } else {
		bar -> box.width = Max(bar -> box.width,
		  plugs[i] -> box.width + 2 * plugs[i] -> box.borderwidth);
	    }
	}
    }
    if (bar -> direction == HORIZONTAL) {
	bar -> box.height = Min(bar -> max_width, Max(bar -> min_width,
						      bar -> box.height));
    } else {
	bar -> box.width = Min(bar -> max_width, Max(bar -> min_width,
						     bar -> box.width));
    }
   
    return 2 * bar -> box.borderwidth + (bar -> direction == HORIZONTAL ?
				      bar -> box.height : bar -> box.width);
}

/*
 * Called AFTER adjusting the client
 * Adjust space in the length of the bar.
 * If we encounter a NULL plug, treat it as an extensible space
 */

UpdateBarLength(bar)
Bar		bar;
{
    Plug  *plugs;
    int             i, n = 0, current_pos = 0, n_spaces = 0;
    int             total_space, free_space, shift = 0;

    if (!bar)
	return;
    plugs = (Plug *) & ((bar -> plugs)[0]);
    for (i = 0; i < bar -> nplugs; i++)
	if (bar -> plugs[i]) {
	    if (bar -> direction == HORIZONTAL) {
		plugs[i] -> box.x = current_pos;
		plugs[i] -> box.y = (bar -> box.height
		- plugs[i]->box.height - 2 * plugs[i] -> box.borderwidth) / 2;
	    } else {
		plugs[i] -> box.y = current_pos;
		plugs[i] -> box.x = (bar -> box.width
		- plugs[i]->box.width - 2 * plugs[i] -> box.borderwidth) / 2;
	    }
	    current_pos += (bar -> direction == HORIZONTAL ?
			    plugs[i] -> box.width : plugs[i] -> box.height)
		+ bar -> plug_separator + 2 * plugs[i] -> box.borderwidth;
	} else {
	    n_spaces++;
	}
    total_space = (bar -> direction == HORIZONTAL ? bar -> box.width
		 : bar -> box.height) - current_pos + bar -> plug_separator;
    if (n_spaces) {
	free_space = total_space / n_spaces;
	if (total_space > 0) {
	    for (i = 0; i < bar -> nplugs; i++) {
		if (bar -> plugs[i]) {
		    if (bar -> direction == HORIZONTAL) {
			plugs[i] -> box.x += shift;
		    } else {
			plugs[i] -> box.y += shift;
		    }
		} else {
		    if (++n == n_spaces) {
			shift = total_space;
		    } else {
			shift += free_space;
		    }
		}
	    }
	}
    }
}

int
NaturalBarLength(bar)
Bar		bar;
{
    Plug  *plugs;
    int             i, current_pos = 0;

    if (!bar)
	return 0;
    plugs = (Plug *) & ((bar -> plugs)[0]);
    for (i = 0; i < bar -> nplugs; i++)
	if (bar -> plugs[i]) {
	    if (bar -> direction == HORIZONTAL) {
		plugs[i] -> box.x = current_pos;
		plugs[i] -> box.y = (bar -> box.height
		- plugs[i]->box.height - 2 * plugs[i] -> box.borderwidth) / 2;
	    } else {
		plugs[i] -> box.y = current_pos;
                plugs[i] -> box.x = (bar -> box.width
                - plugs[i]->box.width - 2 * plugs[i] -> box.borderwidth) / 2;
	    }
	    current_pos += (bar -> direction == HORIZONTAL ?
			    plugs[i] -> box.width : plugs[i] -> box.height)
		+ bar -> plug_separator + 2 * plugs[i] -> box.borderwidth;
	}
    if (current_pos) {
	current_pos -= bar -> plug_separator;
	if (bar -> direction == HORIZONTAL) {
	    bar -> box.width = current_pos;
	} else {
	    bar -> box.height = current_pos;
	}
    }
    return 2 * bar -> box.borderwidth + (bar -> direction == HORIZONTAL ?
				      bar -> box.width : bar -> box.height);
}

BarEventHandler(bar, evt)
Bar	bar;
XEvent	*evt;
{
    int             i;

    switch (evt -> type) {
    case Expose:
	XClearWindow(dpy, bar -> hook);
	break;
    case GWMUserEvent:			/* TODO: no more test on plugs masks*/
	WLFsm_action(bar -> fsm, bar, evt);
	if (GWM_Propagate_user_events)
	    for (i = 0; i < bar -> nplugs; i++)
		if ((bar -> plugs[i]) &&
		    ((WOOL_Fsm) ((bar -> plugs[i]) -> fsm)) -> user_mask) {
		    WOOL_send(WOOL_process_event, bar -> plugs[i],
			      (bar -> plugs[i], evt));
		}
	break;
    default:
	WLFsm_action(bar -> fsm, bar, evt);
    }
}

ReconfigureBar(bar, culprit)
Bar	bar;
Wob	culprit;		/* parent or plug */
{
    int             i, width, height, must_resize = 1;

    if (!bar)
	return;
    width = bar -> box.width;
    height = bar -> box.height;
    if (culprit != (Wob) bar -> parent) {	/* plug */
	UpdateBarWidth(bar);
	if ((width != bar -> box.width) || (height != bar -> box.height)
	    || bar -> parent -> type == MenuClass) { /* HACK */
	    WOOL_send(WOOL_reconfigure, bar -> parent, (bar -> parent, bar));
	}
    }
    UpdateBarLength(bar);
    XMoveResizeWindow(dpy, bar -> hook, bar -> box.x, bar -> box.y,
			  bar -> box.width, bar -> box.height);
    for (i = 0; i < bar -> nplugs; i++)
	if (bar -> plugs[i])
	    ReconfigurePlug(bar -> plugs[i], bar);
    XClearWindow(dpy, bar -> hook);
}

set_bar_bitmap(bar, wl_pixmap)
Bar     bar;
WOOL_Pixmap     wl_pixmap;
{
    XSetWindowBackgroundPixmap(dpy, bar->hook, wl_pixmap -> pixmap);
    if (!bar->nplugs)
	ReconfigureBar(bar, 0);
    XClearWindow(dpy, bar -> hook);
}

