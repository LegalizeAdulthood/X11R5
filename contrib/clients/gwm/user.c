/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*************************************************\
* 						  *
* 	BULL WINDOW MANAGER for X11 .		  *
* 						  *
* 	MODULE handling interactions with user	  *
* 						  *
\*************************************************/

/*  include  */

#include        "EXTERN.h"
#include	"wool.h"
#include 	"wl_atom.h"
#include 	"gwm.h"
#include 	"wl_number.h"
#include 	"wl_string.h"
#include 	"wl_pixmap.h"
#include 	"wl_cursor.h"

/*  local  */
					     
static int	inner_ulx, inner_uly;/* ul corner of inner window */
static int	inner_lrbx, inner_lrby;/* lr corner of inner window, counted
				          from the lr corner of outer window
				          */
    /* inner dims are the inner of the client window */
#define MAKE_INNER_DIMS \
  { int tmp;\
    tmp = cw -> inner_x + cw -> box.borderwidth;\
    inner_ulx = Max(1, tmp);\
    tmp = cw -> inner_y + cw -> box.borderwidth;\
    inner_uly = Max(1, tmp);\
    tmp = lrx - ulx - cw -> inner_width - cw -> \
	box.borderwidth - cw -> inner_x - 2 * cw -> inner_borderwidth + 1;\
    inner_lrbx = Max(1, tmp);\
    tmp = lry - uly - cw -> inner_height - cw ->\
	box.borderwidth - cw -> inner_y - 2 * cw -> inner_borderwidth + 1;\
    inner_lrby = Max(1, tmp);\
  }

static    int             xdir, ydir;	/* resize dirs */
char * MakeUserDims(), *MakePairUserDims();

/*  global  */

extern Wob	GetTarget();
extern WOOL_OBJECT MeterClose();

/*  routines  */

/*
 * type of grid:
 * 	0	outer rectangle of the window (default)
 * 	1	outer rectangle divided in 9 (uwm-style)
 * 	2	outer rectangle with center cross (X) inside
 * 	3	outer rectangle + inner (client) outline
 * 	4	styles 1 and 3 combined
 *	5	2-pixel wide grid
 */

#define MAX_GRID_SEGMENTS	12

int 
StoreBox(type, box, ulx, uly, lrx, lry)
int type;			/* the type of grid */
XSegment box[];
int ulx;			/* Upper left X coordinate. */
int uly;			/* Upper left Y coordinate. */
int lrx;			/* Lower right X coordinate. */
int lry;			/* Lower right Y coordinate. */
{
    int             number_of_segments = 8;

    box[0].x1 = ulx;
    box[0].y1 = uly;
    box[0].x2 = lrx;
    box[0].y2 = uly;

    box[1].x1 = lrx;
    box[1].y1 = uly;
    box[1].x2 = lrx;
    box[1].y2 = lry;

    box[2].x1 = lrx;
    box[2].y1 = lry;
    box[2].x2 = ulx;
    box[2].y2 = lry;

    box[3].x1 = ulx;
    box[3].y1 = lry;
    box[3].x2 = ulx;
    box[3].y2 = uly;

    switch (type) {
    case 5:			/* MwmLike grid */
	box[4].x1 = ulx + 1;
	box[4].y1 = uly + 1;
	box[4].x2 = lrx - 1;
	box[4].y2 = uly + 1;

	box[5].x1 = ulx + 1;
	box[5].y1 = uly + 1;
	box[5].x2 = ulx + 1;
	box[5].y2 = lry - 1;

	box[6].x1 = ulx + 1;
	box[6].y1 = lry - 1;
	box[6].x2 = lrx - 1;
	box[6].y2 = lry - 1;

	box[7].x1 = lrx - 1;
	box[7].y1 = uly + 1;
	box[7].x2 = lrx - 1;
	box[7].y2 = lry - 1;

	return (8) ;

    case 4:			/* 9-squares grid + inner outline */
	box[8].x1 = ulx + (lrx - ulx) / 3;
	box[8].y1 = uly;
	box[8].x2 = box[8].x1;
	box[8].y2 = lry;

	box[9].x1 = lrx - (lrx - ulx) / 3;
	box[9].y1 = uly;
	box[9].x2 = box[9].x1;
	box[9].y2 = lry;

	box[10].x1 = ulx;
	box[10].y1 = uly + (lry - uly) / 3;
	box[10].x2 = lrx;
	box[10].y2 = box[10].y1;

	box[11].x1 = ulx;
	box[11].y1 = lry - (lry - uly) / 3;
	box[11].x2 = lrx;
	box[11].y2 = box[11].y1;

	number_of_segments = 12;

    case 3:			/* + inner outline */
	if (inner_uly) {
	    box[4].x1 = ulx + inner_ulx;
	    box[4].y1 = uly + inner_uly;
	    box[4].x2 = lrx - inner_lrbx;
	    box[4].y2 = box[4].y1;
	} else {
	    box[4].x1 = box[4].y1 = box[4].x2 = box[4].y2 = 0;
	}
	if (inner_lrbx) {
	    box[5].x1 = lrx - inner_lrbx;
	    box[5].y1 = uly + inner_uly;
	    box[5].x2 = box[5].x1;
	    box[5].y2 = lry - inner_lrby;
	} else {
	    box[5].x1 = box[5].y1 = box[5].x2 = box[5].y2 = 0;
	}
	if (inner_lrby) {
	    box[6].x1 = lrx - inner_lrbx;
	    box[6].y1 = lry - inner_lrby;
	    box[6].x2 = ulx + inner_ulx;
	    box[6].y2 = lry - inner_lrby;
	} else {
	    box[6].x1 = box[6].y1 = box[6].x2 = box[6].y2 = 0;
	}
	if (inner_ulx) {
	    box[7].x1 = ulx + inner_ulx;
	    box[7].y1 = lry - inner_lrby;
	    box[7].x2 = ulx + inner_ulx;
	    box[7].y2 = uly + inner_uly;
	} else {
	    box[7].x1 = box[7].y1 = box[7].x2 = box[7].y2 = 0;
	}

	return number_of_segments;

    case 2:			/* cross grid */
	box[4].x1 = ulx;
	box[4].y1 = uly;
	box[4].x2 = lrx;
	box[4].y2 = lry;

	box[5].x1 = ulx;
	box[5].y1 = lry;
	box[5].x2 = lrx;
	box[5].y2 = uly;

	return (6);

    case 1:			/* 9-squares grid */
	box[4].x1 = ulx + (lrx - ulx) / 3;
	box[4].y1 = uly;
	box[4].x2 = box[4].x1;
	box[4].y2 = lry;

	box[5].x1 = lrx - (lrx - ulx) / 3;
	box[5].y1 = uly;
	box[5].x2 = box[5].x1;
	box[5].y2 = lry;

	box[6].x1 = ulx;
	box[6].y1 = uly + (lry - uly) / 3;
	box[6].x2 = lrx;
	box[6].y2 = box[6].y1;

	box[7].x1 = ulx;
	box[7].y1 = lry - (lry - uly) / 3;
	box[7].x2 = lrx;
	box[7].y2 = box[7].y1;

	return (8);

    default:			/* simple outer square */
	return (4);
    }
}

WOOL_OBJECT
UserMoveWindow(cw)
ClientWindow 	cw;
{
    int    prev_x;		/* Previous evt window X location. */
    int    prev_y;		/* Previous evt window Y location. */
    int             cur_x;		/* Current event window X location. */
    int             cur_y;		/* Current event window Y location. */
    int             root_x;		/* Root window X location. */
    int             root_y;		/* Root window Y location. */
    int             ulx, uly;		/* Event window upper left X, Y. */
    int             lrx, lry;		/* Event window lower right X, Y. */
    int             num_vectors;	/* Number of vectors in box. */
    unsigned int    ptrmask;		/* state of ptr when queried */
    Window          sub_window;		/* Query mouse event sub-window. */
    Window          root;		/* Query mouse event root. */
    XEvent          button_event;	/* Button event packet. */
    XSegment        box[MAX_GRID_SEGMENTS];	/* Box vertex buffer. */
    unsigned long   mask = ButtonPressMask | ButtonReleaseMask;
    int             called_from_press;	/* aborts on press! */
    int             called_from_press_button;	/* aborts on press! */
    WOOL_Cursor     cursor;

    /* Get Position of pointer of the event */
    XQueryPointer(dpy, Context -> root,
		  &root, &sub_window,
		  &root_x, &root_y, &cur_x, &cur_y, &ptrmask);
    if (root != Context -> root) {	/* cursor is on another screen */
	return (WOOL_OBJECT) WLNumber_make(1);
    }
    if (TriggeringEvent && TriggeringEvent -> type == ButtonPress) {
	cur_x = TriggeringEvent -> xbutton.x_root;
	cur_y = TriggeringEvent -> xbutton.y_root;
	called_from_press = 1;
	called_from_press_button = TriggeringEvent -> xbutton.button;
	/* if button has been released, aborts! */
	if (GWM_UserSynchronous && !(ptrmask &
		     (Button1Mask << (called_from_press_button - Button1))))
	    return (WOOL_OBJECT) WLNumber_make(3);
    } else {
	/* put the pointer always inside the window */
	cur_x = Max(cur_x, cw -> box.x);
	cur_y = Max(cur_y, cw -> box.y);
	cur_x = Min(cur_x, cw -> box.x + cw -> box.width +
		    2 * cw -> box.borderwidth);
	cur_y = Min(cur_y, cw -> box.y + cw -> box.height +
		    2 * cw -> box.borderwidth);
	called_from_press = 0;
    }

    cursor = (WOOL_Cursor) WOOL_send(WOOL_eval, WA_cursor, (WA_cursor));
    if (XGrabPointer(dpy, Context -> root, FALSE, mask,
		     GrabModeAsync, GrabModeAsync, None,
		     (cursor == (WOOL_Cursor) NIL ? None : cursor -> cursor),
		     CurrentTime)
	!= GrabSuccess)
	return (WOOL_OBJECT) WLNumber_make(0);
    if (GWM_GrabServer)
	XGrabServer(dpy);
    XSetForeground(dpy, Context -> gc.Draw, Context -> pixel.GridColor);

    /* Initialize movement variables. */
    ulx = cw -> box.x;
    uly = cw -> box.y;
    lrx = cw -> box.x + cw -> box.width + (cw -> box.borderwidth * 2) - 1;
    lry = cw -> box.y + cw -> box.height + (cw -> box.borderwidth * 2) - 1;
    MAKE_INNER_DIMS

    /* Store the box. */
	num_vectors = StoreBox(GWM_Movegrid, box, ulx, uly, lrx, lry);

    /* Initialize the previous location variables. */
    prev_x = cur_x;
    prev_y = cur_y;

    /* Now draw the box. */
    XDrawSegments(dpy, Context -> root, Context -> gc.Draw, box, num_vectors);
    if (GWM_Move_meter) {
        MeterOpen("(100,100)");
        MeterUpdate(MakePairUserDims(ulx, uly));
    }

    /* Main loop. */
    while (TRUE) {
	if (XCheckMaskEvent(dpy, ButtonReleaseMask | ButtonPressMask,
			    &button_event)) {
	    /* The button was released, so move the window. */
	    XDrawSegments(dpy, Context -> root, Context -> gc.Draw,
			  box, num_vectors);
	    if (!called_from_press || button_event.type == ButtonRelease) {
		/* MOVE HOOK */
		XMoveWindow(dpy, cw -> hook,/* only move if released */
			    button_event.xbutton.x_root + ulx - prev_x,
			    button_event.xbutton.y_root + uly - prev_y);
		cw -> box.x = button_event.xbutton.x_root + ulx - prev_x;
		cw -> box.y = button_event.xbutton.y_root + uly - prev_y;
		SendSyntheticMoveEvent(cw);
		if (button_event.type == ButtonPress)
		    wait_for_button_release(button_event.xbutton.button);
		if (GWM_Move_meter)
		    MeterClose();
		XUngrabPointer(dpy, CurrentTime);
		if (GWM_GrabServer)
		    XUngrabServer(dpy);
		return NIL;
	    } else {			/* another buttonpress = abort! */
		wait_for_button_release(button_event.xbutton.button);
		wait_for_button_release(called_from_press_button);
                if (GWM_Move_meter)
                    MeterClose();
		XUngrabPointer(dpy, CurrentTime);
		if (GWM_GrabServer)
		    XUngrabServer(dpy);
		return (WOOL_OBJECT) WLNumber_make(2);
	    }
	}
	/* track ghost window */
	XQueryPointer(dpy,
		      RootWindow(dpy, Context -> screen), &root, &sub_window,
		      &root_x, &root_y, &cur_x, &cur_y, &ptrmask);
	if ((cur_x != prev_x) || (cur_y != prev_y)) {
	    /* erase the old box first! */
	    XDrawSegments(dpy, Context -> root, Context -> gc.Draw, box,
			  num_vectors);
	    ulx += cur_x - prev_x;	/* change Box position. */
	    uly += cur_y - prev_y;
	    lrx += cur_x - prev_x;
	    lry += cur_y - prev_y;
	    if (GWM_confine_windows) {
	      if (ulx < 0) {
		lrx -= ulx;
		ulx = 0;
	      }
	      if (uly < 0) {
		lry -= uly;
		uly = 0;
	      }
	      if (lrx >= Context -> width) {
		ulx -= (lrx - Context -> width) + 1;
		lrx = Context -> width - 1;
	      }
	      if (lry >=  Context -> height) {
		uly -= (lry - Context -> height) + 1;
		lry = Context -> height - 1;
	      }
	    }
	    num_vectors = StoreBox(GWM_Movegrid, box, ulx, uly, lrx, lry);
	    /* Draw the new box. */
	    XDrawSegments(dpy, Context -> root, Context -> gc.Draw, box,
			  num_vectors);
            if (GWM_Move_meter)
                MeterUpdate(MakePairUserDims(ulx, uly));
	}
	/* Save old box position. */
	prev_x = cur_x;
	prev_y = cur_y;
    }
}

WOOL_OBJECT
UserResizeWindow(cw)
ClientWindow	cw;
{
    int    prev_x2;			/* Previous evt window X location. */
    int    prev_y2;			/* Previous evt window Y location. */
    int             init_x, init_y;
    int             init_x2, init_y2;
    int             x1, y1;		/* fixed point */
    int             x2, y2;		/* varying point */
    int             cur_x;		/* Current event window X location. */
    int             cur_y;		/* Current event window Y location. */
    int             root_x;		/* Root window X location. */
    int             root_y;		/* Root window Y location. */
    int             ulx, uly;		/* Event window upper left X, Y. */
    int             lrx, lry;		/* Event window lower right X, Y. */
    int             init_ulx, init_uly;	/* Init window upper left X and Y. */
    int             init_lrx, init_lry;	/* Init window lower right X and Y. */
    int             num_vectors;	/* Number of vectors in box. */
    unsigned int    ptrmask;		/* state of ptr when queried */
    Window          sub_window;		/* Query mouse event sub-window. */
    Window          root;		/* Query mouse event root. */
    XEvent          button_event;	/* Button event packet. */
    XSegment        box[MAX_GRID_SEGMENTS]; /* Box vertex buffer. */
    unsigned long   mask = ButtonPressMask | ButtonReleaseMask;
    int             inner_width, inner_height;
    int             called_from_press;	/* aborts on press! */
    int             called_from_press_button; /* aborts on press! */
    WOOL_Cursor     cursor;

    /* Get Position of pointer of the event */
    XQueryPointer(dpy, Context -> root,
		  &root, &sub_window,
		  &root_x, &root_y, &cur_x, &cur_y, &ptrmask);
    if (root != Context -> root) {	/* cursor is on another screen */
	return (WOOL_OBJECT) WLNumber_make(1);
    }
    if (TriggeringEvent && TriggeringEvent -> type == ButtonPress) {
	cur_x = TriggeringEvent -> xbutton.x_root;
	cur_y = TriggeringEvent -> xbutton.y_root;
	called_from_press = 1;
	called_from_press_button = TriggeringEvent -> xbutton.button;
	/* if button has been released, aborts! */
	if (GWM_UserSynchronous && !(ptrmask &
				     (Button1Mask << (called_from_press_button - Button1))))
	    return (WOOL_OBJECT) WLNumber_make(3);
    } else {
	/* put the pointer always inside the window */
	cur_x = Max(cur_x, cw -> box.x);
	cur_y = Max(cur_y, cw -> box.y);
	cur_x = Min(cur_x, cw -> box.x + cw -> box.width +
		    2 * cw -> box.borderwidth);
	cur_y = Min(cur_y, cw -> box.y + cw -> box.height +
		    2 * cw -> box.borderwidth);
	called_from_press = 0;
    }

    init_x = cur_x;
    init_y = cur_y;

    cursor = (WOOL_Cursor) WOOL_send(WOOL_eval, WA_cursor, (WA_cursor));
    if (XGrabPointer(dpy, Context -> root, FALSE, mask,
		     GrabModeAsync, GrabModeAsync, None,
		     (cursor == (WOOL_Cursor) NIL ? None : cursor -> cursor),
		     CurrentTime)
	!= GrabSuccess)
	return (WOOL_OBJECT) WLNumber_make(0);
    if (GWM_GrabServer)
	XGrabServer(dpy);
    XSetForeground(dpy, Context -> gc.Draw, Context -> pixel.GridColor);

    /* Initialize movement variables. */
    init_ulx = ulx = cw -> box.x;
    init_uly = uly = cw -> box.y;
    init_lrx = lrx = cw -> box.x + cw -> box.width +
	(cw -> box.borderwidth * 2) - 1;
    init_lry = lry = cw -> box.y + cw -> box.height +
	(cw -> box.borderwidth * 2) - 1;
    MAKE_INNER_DIMS

	/* determine the direction of resize 0, 1 or 2 */
	xdir = ((cur_x - ulx) * 3) / (lrx - ulx);
    ydir = ((cur_y - uly) * 3) / (lry - uly);
    /* handle cur_x == lrx case */
    if (xdir == 3)
	xdir = 2;
    if (ydir == 3)
	ydir = 2;
    /* determine the fixed & varying points */
    if (xdir) {
	x1 = ulx;
	x2 = lrx;
    } else {
	x1 = lrx;
	x2 = ulx;
    }
    if (ydir) {
	y1 = uly;
	y2 = lry;
    } else {
	y1 = lry;
	y2 = uly;
    }

    /* Initialize the previous location variables. */
    init_x2 = prev_x2 = x2;
    init_y2 = prev_y2 = y2;

    /* Store the box. */
    num_vectors = StoreBox(GWM_Resizegrid, box, Min(x1, x2), Min(y1, y2),
			   Max(x1, x2), Max(y1, y2));
    /* Now draw the box. */
    XDrawSegments(dpy, Context -> root, Context -> gc.Draw, box, num_vectors);
    if (GWM_Resize_meter) {
	MeterOpen("80x24");
	MeterUpdate(MakeUserDims(&(cw -> cached_props -> normal_hints),
				 cw -> inner_width, cw -> inner_height));
    }
    /* Main loop. */
    while (TRUE) {
	if (XCheckMaskEvent(dpy, ButtonReleaseMask | ButtonPressMask,
			    &button_event)) {
	    /* The button was released, so move the window. */
	    XDrawSegments(dpy, Context -> root, Context -> gc.Draw,
			  box, num_vectors);
	    /* OK, let's resize! */
	    if (!called_from_press || button_event.type == ButtonRelease) {
		int             delta, borderwidth, depth;

		delta = Min(x1, x2) - init_ulx;
		cw -> box.x += delta;
		delta = Min(y1, y2) - init_uly;
		cw -> box.y += delta;
		delta = Max(x1, x2) - init_lrx - (Min(x1, x2) - init_ulx);
		cw -> inner_width += delta;
		cw -> box.width += delta;
		delta = Max(y1, y2) - init_lry - (Min(y1, y2) - init_uly);
		cw -> inner_height += delta;
		cw -> box.height += delta;
		XResizeWindow(dpy, cw -> client,
			      cw -> inner_width, cw -> inner_height);
		ReconfigureClientWindow(cw, cw -> client);
		if (button_event.type == ButtonPress)
		    wait_for_button_release(button_event.xbutton.button);
		if (GWM_Resize_meter)
		    MeterClose();
		XUngrabPointer(dpy, CurrentTime);
		if (GWM_GrabServer)
		    XUngrabServer(dpy);
		return NIL;
	    } else {			/* another buttonpress = abort! */
		wait_for_button_release(button_event.xbutton.button);
		wait_for_button_release(called_from_press_button);
		if (GWM_Resize_meter)
		    MeterClose();
		XUngrabPointer(dpy, CurrentTime);
		if (GWM_GrabServer)
		    XUngrabServer(dpy);
		return (WOOL_OBJECT) WLNumber_make(2);
	    }
	}
	/* track ghost window */
	XQueryPointer(dpy,
		      RootWindow(dpy, Context -> screen), &root, &sub_window,
		      &root_x, &root_y, &cur_x, &cur_y, &ptrmask);
	x2 = validate_dir(xdir, cur_x - init_x, cw -> inner_width);
	y2 = validate_dir(ydir, cur_y - init_y, cw -> inner_height);
	inner_width = cw -> inner_width + (xdir ? x2 : -x2);
	inner_height = cw -> inner_height + (ydir ? y2 : -y2);
	conform_to_hints(&(cw -> cached_props -> normal_hints),
			 &inner_width, &inner_height);
	x2 = init_x2 + (xdir ? inner_width - cw -> inner_width :
			cw -> inner_width - inner_width);
	y2 = init_y2 + (ydir ? inner_height - cw -> inner_height :
			cw -> inner_height - inner_height);
	if (GWM_confine_windows) {
	    int readjust = 0;
	    if ((int)cw -> box.borderwidth > x2) {
		x2 = (int)cw -> box.borderwidth;
		inner_width = cw -> box.x - x2 + cw -> inner_width;
		readjust = 1;
	    } else if ((int)((Context -> width - cw -> box.borderwidth) -1)
		       < x2) {
		x2 = (int)((Context -> width - cw -> box.borderwidth) -1);
		inner_width = x2 - cw -> box.x - cw -> box.borderwidth -1;
		readjust = 1;
	    }

	    if ((int)cw -> box.borderwidth > y2) {
		y2 = (int)cw -> box.borderwidth;
		inner_height = cw -> box.y - y2 + cw -> inner_height;
		readjust = 1;
	    } else if ((int)((Context -> height - cw -> box.borderwidth) -1)
		       < y2) {
		y2 = (int)((Context -> height - cw -> box.borderwidth) -1);
		inner_height = y2 - cw -> box.y - cw -> box.borderwidth -1;
		readjust = 1;
	    }
	    if (readjust) {
		conform_to_hints(&(cw -> cached_props -> normal_hints),
				 &inner_width, &inner_height);
		x2 = init_x2 + (xdir ? inner_width - cw -> inner_width :
				cw -> inner_width - inner_width);
		y2 = init_y2 + (ydir ? inner_height - cw -> inner_height :
				cw -> inner_height - inner_height);
	    }
	}
	if ((x2 != prev_x2) || (y2 != prev_y2)) {
	    /* erase the old box first! */
	    XDrawSegments(dpy, Context -> root, Context -> gc.Draw, box,
			  num_vectors);
	    num_vectors = StoreBox(GWM_Resizegrid, box,
				   Min(x1, x2), Min(y1, y2),
				   Max(x1, x2), Max(y1, y2));
	    /* Draw the new box. */
	    XDrawSegments(dpy, Context -> root, Context -> gc.Draw, box,
			  num_vectors);
	    if (GWM_Resize_meter)
		MeterUpdate(MakeUserDims(
					 &(cw -> cached_props -> normal_hints),
					 inner_width, inner_height));
	    /* Save old box position. */
	    prev_x2 = x2;
	    prev_y2 = y2;
	}
    }
}

int
validate_dir(dir, delta, margin)
int dir, delta, margin;
{
    switch (dir) {
    case 0:
	return Min(margin, delta);
    case 2:
	return Max(-margin, delta);
    }
    return 0;
}


int
conform_to_hints(hints, width, height)
XSizeHints	*hints;
int *width, *height;
{
    if (hints -> flags & PMinSize) {
	if (*width < hints -> min_width)
	    *width = hints -> min_width;
	if (*height < hints -> min_height)
	    *height = hints -> min_height;
    }
    if (hints -> flags & PMaxSize) {
	if (*width > hints -> max_width)
	    *width = hints -> max_width;
	if (*height > hints -> max_height)
	    *height = hints -> max_height;
    }
    if (hints -> flags & PAspect) {
	if (hints -> min_aspect.y * (*width)
	    < hints -> min_aspect.x * (*height)) {
	    if (xdir == 1) {
		*height = ((*width) * hints -> min_aspect.y)
		    / hints -> min_aspect.x;
	    } else {
		*width = ((*height) * hints -> min_aspect.x)
		    / hints -> min_aspect.y;
	    }
	}
	if (hints -> max_aspect.y * (*width)
	    > hints -> max_aspect.x * (*height)) {
	    if (ydir == 1) {
		*width = ((*height) * hints -> max_aspect.x)
		    / hints -> max_aspect.y;
	    } else {
		*height = ((*width) * hints -> max_aspect.y)
		    / hints -> max_aspect.x;
	    }
	}
    }
#ifdef NOBASEDIMS
    if (hints -> flags & PResizeInc) {
	*width = hints -> min_width + hints -> width_inc *
	    (((*width) - hints -> min_width) / hints -> width_inc);
	*height = hints -> min_height + hints -> height_inc *
	    (((*height) - hints -> min_height) / hints -> height_inc);
    }
#else /* NOBASEDIMS */
    if (hints -> flags & PResizeInc) {
	*width = hints -> base_width + hints -> width_inc *
	    (((*width) - hints -> base_width) / hints -> width_inc);
	*height = hints -> base_height + hints -> height_inc *
	    (((*height) - hints -> base_height) / hints -> height_inc);
    }
#endif /* NOBASEDIMS */
}

/*
 * Updates the sizehints for consitency with themselves and the screen dims.
 */

CheckConsistency(hints)
XSizeHints *hints;
{
#ifndef NOBASEDIMS
    if((hints -> flags & PMinSize) && !(hints -> flags & PBaseSize)) {
	hints -> flags |= PBaseSize;
	hints -> base_width = hints -> min_width;
	hints -> base_height = hints -> min_height;
    }
    if((hints -> flags & PResizeInc) && !(hints -> flags & PBaseSize)) {
	hints -> base_width = 0;
	hints -> base_height = 0;
    }
#else /* NOBASEDIMS */
    if((hints -> flags & PResizeInc) && !(hints -> flags & PMinSize)) {
	hints -> min_width = 0;
	hints -> min_height = 0;
    }
#endif /* NOBASEDIMS */
    if (hints -> flags & PResizeInc && 
	(hints -> width_inc <= 0 || hints -> height_inc <= 0)) {
	hints -> height_inc = hints -> width_inc = 1;
	trace(0, "Bogus increment hints\n");
    }
}

/*
 * user feedback
 */

static    char            meter_str[20];

char *
MakePairUserDims(x, y)
int	x, y;
{
    sprintf(meter_str, "(%d, %d)", x, y	);
    return meter_str;
}

char *
MakeUserDims(hints, width, height)
XSizeHints *hints;
int	width, height;
{
    unsigned int rw, rh;

    pixel_to_logical_size(hints, width, height, &rw, &rh);
    sprintf(meter_str, "%d x %d", rw, rh);
    return meter_str;
}

pixel_to_logical_size(hints, width, height, rw, rh)
XSizeHints *hints;
unsigned int width, height, *rw, *rh;
{
    *rw = width;
    *rh = height;
#ifdef NOBASEDIMS
    if (hints->flags & PResizeInc) {
	*rw = ((width) - hints->min_width) / hints->width_inc;
	*rh = ((height) - hints->min_height) / hints->height_inc;
    }
#else /* NOBASEDIMS */
    if (hints->flags & PResizeInc) {
	*rw = ((width) - hints->base_width) / hints->width_inc;
	*rh = ((height) - hints->base_height) / hints->height_inc;
    }
#endif /* NOBASEDIMS */
}

logical_to_pixel_size(hints, width, height, rw, rh)
XSizeHints *hints;
unsigned int width, height, *rw, *rh;
{
    *rw = width;
    *rh = height;
#ifdef NOBASEDIMS
    if (hints -> flags & PResizeInc) {
	*rw = hints -> min_width + width * hints -> width_inc;
	*rh = hints -> min_height + height * hints -> height_inc;
    }
#else /* NOBASEDIMS */
    if (hints -> flags & PResizeInc) {
	*rw = hints -> base_width + width * hints -> width_inc;
	*rh = hints -> base_height + height * hints -> height_inc;
    }
#endif /* NOBASEDIMS */
}

Cursor
get_zone_cursor(zone)
int zone;
{
WOOL_Cursor cursor ;

switch (zone)
 {
  case 0:
    cursor = (WOOL_Cursor) WOOL_send(WOOL_eval, WA_cursorNW, (WA_cursorNW));
    break;
  case 1:
    cursor = (WOOL_Cursor) WOOL_send(WOOL_eval, WA_cursorN, (WA_cursorN));
    break;
  case 2:
    cursor = (WOOL_Cursor) WOOL_send(WOOL_eval, WA_cursorNE, (WA_cursorNE));
    break;
  case 3:
    cursor = (WOOL_Cursor) WOOL_send(WOOL_eval, WA_cursorW, (WA_cursorW));
    break;
  case 5:
    cursor = (WOOL_Cursor) WOOL_send(WOOL_eval, WA_cursorE, (WA_cursorE));
    break;
  case 6:
    cursor = (WOOL_Cursor) WOOL_send(WOOL_eval, WA_cursorSW, (WA_cursorSW));
    break;
  case 7:
    cursor = (WOOL_Cursor) WOOL_send(WOOL_eval, WA_cursorS, (WA_cursorS));
    break;
  case 8:
    cursor = (WOOL_Cursor) WOOL_send(WOOL_eval, WA_cursorSE, (WA_cursorSE));
    break;
  default: cursor = (WOOL_Cursor) WOOL_send(WOOL_eval, WA_cursor, (WA_cursor));
 }
    return (cursor == (WOOL_Cursor) NIL ? None : cursor -> cursor);
}

/* MWM-like resize
 * code by Frederic Charton
 */

WOOL_OBJECT
MwmLikeUserResizeWindow(cw)
ClientWindow	cw;
{
    int    prev_x2;			/* Previous evt window X location. */
    int    prev_y2;			/* Previous evt window Y location. */
    int             init_x, init_y;
    int             init_x2, init_y2;
    int             x1, y1;		/* fixed point */
    int             x2, y2;		/* varying point */
    int             cur_x;		/* Current event window X location. */
    int             cur_y;		/* Current event window Y location. */
    int             root_x;		/* Root window X location. */
    int             root_y;		/* Root window Y location. */
    int             ulx, uly;		/* Event window upper left X, Y. */
    int             lrx, lry;		/* Event window lower right X, Y. */
    int             init_ulx, init_uly;	/* Init window upper left X and Y. */
    int             init_lrx, init_lry;	/* Init window lower right X and Y. */
    int             num_vectors;	/* Number of vectors in box. */
    unsigned int    ptrmask;		/* state of ptr when queried */
    Window          sub_window;		/* Query mouse event sub-window. */
    Window          root;		/* Query mouse event root. */
    XEvent          button_event;	/* Button event packet. */
    XSegment        box[MAX_GRID_SEGMENTS]; /* Box vertex buffer. */
    unsigned long   mask = ButtonPressMask | ButtonReleaseMask;
    int             inner_width, inner_height;
    int             called_from_press;	/* aborts on press! */
    int             called_from_press_button; /* aborts on press! */
    int             zone;
    int             new_xdir, new_ydir, new_zone;
    int             chgt_x, chgt_y;
    WOOL_Cursor     cursor;

    /* Get Position of pointer of the event */
    XQueryPointer(dpy, Context -> root,
		  &root, &sub_window,
		  &root_x, &root_y, &cur_x, &cur_y, &ptrmask);
    if (root != Context -> root) {	/* cursor is on another screen */
	return (WOOL_OBJECT) WLNumber_make(1);
    }
    if (TriggeringEvent && TriggeringEvent -> type == ButtonPress) {
	cur_x = TriggeringEvent -> xbutton.x_root;
	cur_y = TriggeringEvent -> xbutton.y_root;
	called_from_press = 1;
	called_from_press_button = TriggeringEvent -> xbutton.button;
	/* if button has been released, aborts! */
	if (GWM_UserSynchronous && !(ptrmask &
				     (Button1Mask << (called_from_press_button - Button1))))
	    return (WOOL_OBJECT) WLNumber_make(3);
    } else {
	/* put the pointer always inside the window */
	cur_x = Max(cur_x, cw -> box.x);
	cur_y = Max(cur_y, cw -> box.y);
	cur_x = Min(cur_x, cw -> box.x + cw -> box.width +
		    2 * cw -> box.borderwidth);
	cur_y = Min(cur_y, cw -> box.y + cw -> box.height +
		    2 * cw -> box.borderwidth);
	called_from_press = 0;
    }

    init_x = cur_x;
    init_y = cur_y;

    cursor = (WOOL_Cursor) WOOL_send(WOOL_eval, WA_cursor, (WA_cursor));
    if (XGrabPointer(dpy, Context -> root, FALSE, mask,
		     GrabModeAsync, GrabModeAsync, None,
		     (cursor == (WOOL_Cursor) NIL ? None : cursor -> cursor),
		     CurrentTime)
	!= GrabSuccess)
	return (WOOL_OBJECT) WLNumber_make(0);
    if (GWM_GrabServer)
	XGrabServer(dpy);
    XSetForeground(dpy, Context -> gc.Draw, Context -> pixel.GridColor);

    /* Initialize movement variables. */
    init_ulx = ulx = cw -> box.x;
    init_uly = uly = cw -> box.y;
    init_lrx = lrx = cw -> box.x + cw -> box.width +
	(cw -> box.borderwidth * 2) - 1;
    init_lry = lry = cw -> box.y + cw -> box.height +
	(cw -> box.borderwidth * 2) - 1;
    MAKE_INNER_DIMS

	/* determine the direction of resize 0, 1 or 2 */
	if (GWM_window_cutting_size > 0) {
	    if (cur_x < (ulx + GWM_window_cutting_size))
		xdir = 0;
	    else if (cur_x > (lrx - GWM_window_cutting_size))
		xdir = 2;
	    else
		xdir = 1;
	    if (cur_y < (uly + GWM_window_cutting_size))
		ydir = 0;
	    else if (cur_y > (lry - GWM_window_cutting_size))
		ydir = 2;
	    else
		ydir = 1;
	} else {
	    xdir = ((cur_x - ulx) * 3) / (lrx - ulx);
	    ydir = ((cur_y - uly) * 3) / (lry - uly);
	    /* handle cur_x == lrx case */
	    if (xdir == 3)
		xdir = 2;
	    if (ydir == 3)
		ydir = 2;
	}

    /* determine the fixed & varying points */
    if (xdir) {
	x1 = ulx;
	x2 = lrx;
    } else {
	x1 = lrx;
	x2 = ulx;
    }
    if (ydir) {
	y1 = uly;
	y2 = lry;
    } else {
	y1 = lry;
	y2 = uly;
    }

    chgt_x = chgt_y = 0;

    zone = 3 * ydir + xdir;
    XChangeActivePointerGrab(dpy, mask, get_zone_cursor(zone), CurrentTime);

    /* Initialize the previous location variables. */
    init_x2 = prev_x2 = x2;
    init_y2 = prev_y2 = y2;

    /* Store the box. */
    num_vectors = StoreBox(GWM_Resizegrid, box, Min(x1, x2), Min(y1, y2),
			   Max(x1, x2), Max(y1, y2));
    /* Now draw the box. */
    XDrawSegments(dpy, Context -> root, Context -> gc.Draw, box, num_vectors);
    if (GWM_Resize_meter) {
	MeterOpen("80x24");
	MeterUpdate(MakeUserDims(&(cw -> cached_props -> normal_hints),
				 cw -> inner_width, cw -> inner_height));
    }
    /* Main loop. */
    while (TRUE) {
	if (XCheckMaskEvent(dpy, ButtonReleaseMask | ButtonPressMask,
			    &button_event)) {
	    /* The button was released, so move the window. */
	    XDrawSegments(dpy, Context -> root, Context -> gc.Draw,
			  box, num_vectors);
	    /* OK, let's resize! */
	    if (!called_from_press || button_event.type == ButtonRelease) {
		int             delta, borderwidth, depth;

		delta = Min(x1, x2) - init_ulx;
		cw -> box.x += delta;
		delta = Min(y1, y2) - init_uly;
		cw -> box.y += delta;
		delta = Max(x1, x2) - init_lrx - (Min(x1, x2) - init_ulx);
		cw -> inner_width += delta;
		cw -> box.width += delta;
		delta = Max(y1, y2) - init_lry - (Min(y1, y2) - init_uly);
		cw -> inner_height += delta;
		cw -> box.height += delta;
		XResizeWindow(dpy, cw -> client,
			      cw -> inner_width, cw -> inner_height);
		ReconfigureClientWindow(cw, cw -> client);
		if (button_event.type == ButtonPress)
		    wait_for_button_release(button_event.xbutton.button);
		if (GWM_Resize_meter)
		    MeterClose();
		XUngrabPointer(dpy, CurrentTime);
		if (GWM_GrabServer)
		    XUngrabServer(dpy);
		return NIL;
	    } else {			/* another buttonpress = abort! */
		wait_for_button_release(button_event.xbutton.button);
		wait_for_button_release(called_from_press_button);
		if (GWM_Resize_meter)
		    MeterClose();
		XUngrabPointer(dpy, CurrentTime);
		if (GWM_GrabServer)
		    XUngrabServer(dpy);
		return (WOOL_OBJECT) WLNumber_make(2);
	    }
	}
	/* track ghost window */
	XQueryPointer(dpy,
		      RootWindow(dpy, Context -> screen), &root, &sub_window,
		      &root_x, &root_y, &cur_x, &cur_y, &ptrmask);
	x2 = validate_dir(xdir, cur_x - init_x, cw -> inner_width);
	y2 = validate_dir(ydir, cur_y - init_y, cw -> inner_height);
	inner_width = cw -> inner_width + (xdir ? x2 : -x2 - chgt_x);
	inner_height = cw -> inner_height + (ydir ? y2 : -y2 - chgt_y);
	if (inner_width < 2 * GWM_window_cutting_size)
	    inner_width = 2 * GWM_window_cutting_size;
	if (inner_height < 2 * GWM_window_cutting_size)
	    inner_height = 2 * GWM_window_cutting_size;
	conform_to_hints(&(cw -> cached_props -> normal_hints),
			 &inner_width, &inner_height);
	x2 = init_x2 + (xdir ? inner_width - cw -> inner_width :
			cw -> inner_width - inner_width - chgt_x);
	y2 = init_y2 + (ydir ? inner_height - cw -> inner_height :
			cw -> inner_height - inner_height - chgt_y);
	if (GWM_confine_windows) {
	    int readjust = 0;
	    if ((int)cw -> box.borderwidth > x2) {
		x2 = (int)cw -> box.borderwidth;
		inner_width = cw -> box.x - x2 + cw -> inner_width;
		readjust = 1;
	    } else if ((int)((Context -> width - cw -> box.borderwidth) -1)
		       < x2) {
		x2 = (int)((Context -> width - cw -> box.borderwidth) -1);
		inner_width = x2 - cw -> box.x - cw -> box.borderwidth;
		readjust = 1;
	    }

	    if ((int)cw -> box.borderwidth > y2) {
		y2 = (int)cw -> box.borderwidth;
		inner_height = cw -> box.y - y2 + cw -> inner_height;
		readjust = 1;
	    } else if ((int)((Context -> height - cw -> box.borderwidth) -1)
		       < y2) {
		y2 = (int)((Context -> height - cw -> box.borderwidth) -1);
		inner_height = y2 - cw -> box.y - cw -> box.borderwidth;
		readjust = 1;
	    }
	    if (readjust) {
		conform_to_hints(&(cw -> cached_props -> normal_hints),
				 &inner_width, &inner_height);
		x2 = init_x2 + (xdir ? inner_width - cw -> inner_width :
				cw -> inner_width - inner_width - chgt_x);
		y2 = init_y2 + (ydir ? inner_height - cw -> inner_height :
				cw -> inner_height - inner_height - chgt_y);
	    }
	}
	if (GWM_Mwm_catch_corners &&
	    (GWM_window_cutting_size > 0) && (zone % 2)) {
	    ulx = Min(x1, x2);
	    uly = Min(y1, y2);
	    lrx = Max(x1, x2);
	    lry = Max(y1, y2);
	    if (cur_x < (ulx + GWM_window_cutting_size))
		new_xdir = 0;
	    else if (cur_x > (lrx - GWM_window_cutting_size))
		new_xdir = 2;
	    else
		new_xdir = 1;
	    if (cur_y < (uly + GWM_window_cutting_size))
		new_ydir = 0;
	    else if (cur_y > (lry - GWM_window_cutting_size))
		new_ydir = 2;
	    else
		new_ydir = 1;
	    new_zone = 3 * new_ydir + new_xdir;
	    if (!(new_zone % 2) && (new_zone != 4)
	    	&& (Abs(new_zone - zone) <= 3)) {
		XChangeActivePointerGrab(dpy, mask,
		    get_zone_cursor(new_zone), CurrentTime);
		zone = new_zone;
		/* determine if the varying point is changing */
		if ((xdir == 1) && (new_xdir == 0))
		    chgt_x = cw -> inner_width;
		if ((ydir == 1) && (new_ydir == 0))
		    chgt_y = cw -> inner_height;
		xdir = new_xdir;
		ydir = new_ydir;
		init_x = init_x2;
		init_y = init_y2;
		/* determine the new points */
		if (xdir) {
		    x1 = ulx;
		    x2 = lrx;
		} else {
		    x1 = lrx;
		    x2 = ulx;
		}
		if (ydir) {
		    y1 = uly;
		    y2 = lry;
		} else {
		    y1 = lry;
		    y2 = uly;
		}
	    }
	}
	if ((x2 != prev_x2) || (y2 != prev_y2)) {
	    /* erase the old box first! */
	    XDrawSegments(dpy, Context -> root, Context -> gc.Draw, box,
			  num_vectors);
	    num_vectors = StoreBox(GWM_Resizegrid, box,
			Min(x1, x2), Min(y1, y2), Max(x1, x2), Max(y1, y2));
	    /* Draw the new box. */
	    XDrawSegments(dpy, Context -> root, Context -> gc.Draw, box,
			  num_vectors);
	    if (GWM_Resize_meter)
		MeterUpdate(MakeUserDims(
				      &(cw -> cached_props -> normal_hints),
					 inner_width, inner_height));
	    /* Save old box position. */
	    prev_x2 = x2;
	    prev_y2 = y2;
	}
    }
}
