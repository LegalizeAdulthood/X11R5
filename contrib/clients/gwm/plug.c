/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*****************************************************\
* 						      *
* 	BULL WINDOW MANAGER for X11 .		      *
* 						      *
* 		MODULE defining the Plug Wob Class    *
* 						      *
\*****************************************************/

/*  include  */

#include        "EXTERN.h"
#include 	"gwm.h"
#include	"wl_atom.h"
#include	"wl_list.h"
#include	"wl_fsm.h"
#include	"wl_pixmap.h"
#include	"wl_cursor.h"
#include	"wl_plug.h"

/*  local constants  */

/*  external  */

extern Wob      NewWob();
extern WOOL_METHOD WLMenu[];
extern Plug PlugOpen();
extern PlugClose(), PlugEventHandler(), ReconfigurePlug();

WOB_METHOD       PlugClass[] = {
			       0,	/* METHODS_ARRAY */
			       WobEval,
			       WobPrint,
			       WobRelease,
			       WobExecute,
			       WobSet,
			       WobGetCValue,
			       (WOB_METHOD) PlugOpen,
			       PlugClose,
			       PlugEventHandler,
                               (WOB_METHOD) wool_undefined_method_1,
                               WobGetDimensions,
                               (WOB_METHOD) wool_undefined_method_2,
                               (WOB_METHOD) wool_undefined_method_2,
                               ReconfigurePlug,
			(WOB_METHOD) wool_undefined_method_2,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1
};

/*  routines  */

/*
 * Set up an icon that must be mapped onto a bar.
 */

Plug
PlugOpen(plug)
Plug             plug;
{
    check_window_size(plug);
    plug -> hook =
	XCreateSimpleWindow(dpy, plug -> parent -> hook,
			    plug -> box.x, plug -> box.y,
			    plug -> box.width, plug -> box.height,
			    plug -> box.borderwidth,
			    plug -> box.borderpixel, plug -> box.background);
    if (plug -> parent -> type == ScreenClass)
        plug -> status |= TopLevelXWindowStatus;
    WobRecordHook(plug);
    plug -> curstate = (int) WOOL_send(WOOL_open, plug -> fsm, (plug -> fsm));
    plug -> input_mask = WobMask 
	| (plug -> graphic_mask = 
	   ((unsigned int) WOOL_send(WOOL_open, 
				     plug -> graphic, 
				     (plug -> graphic, plug))))
	    | ((WOOL_Fsm) plug -> fsm) -> mask;
    XSelectInput(dpy, plug -> hook, plug -> input_mask);
    if (plug -> cursor != NIL)
	XDefineCursor(dpy, plug -> hook,
		      ((WOOL_Cursor) plug -> cursor) -> cursor);
    if (plug -> bordertile != NIL)
	XSetWindowBorderPixmap(dpy, plug -> hook,
			       ((WOOL_Pixmap) plug -> bordertile) -> pixmap);
    return plug;
}

Plug 
NewPlug(bar, wl_plug)
Bar             bar;
WOOL_Plug	wl_plug;
{
    Plug            plug;
    WOOL_OBJECT     graphic;

    /* if plug = (), then it is a space and we return NULL */
    if (wl_plug == (WOOL_Plug) NIL)
	return NULL;
    plug = (Plug) NewWob(sizeof(struct _Plug));
    wl_plug = (WOOL_Plug) wool_type_or_evaluate(wl_plug, WLPlug);
    plug -> type = PlugClass;
    plug -> parent = (Wob) bar;
    graphic = wl_plug -> graphic;
    if (graphic -> type == WLAtom || graphic -> type == WLList)
	graphic = WOOL_send(WOOL_eval, graphic, (graphic));
    /* first, set the box info */
    plug -> box.borderwidth = wl_plug -> borderwidth;
    plug -> box.borderpixel = wl_plug -> borderpixel;
    plug -> box.background = wl_plug -> background;
    increase_reference(plug -> property = (WOOL_OBJECT) wl_plug -> property);
    increase_reference(plug -> bordertile =
		    wool_type_or_evaluate(wl_plug -> bordertile, WLPixmap));
    increase_reference(plug -> fsm =
		       wool_type_or_evaluate(wl_plug -> fsm, WLFsm));
    increase_reference(plug -> menu =
		       wool_type_or_evaluate(wl_plug -> menu, WLMenu));
    increase_reference(plug -> cursor =
		       wool_type_or_evaluate(wl_plug -> cursor, WLCursor));
    increase_reference(plug -> graphic = graphic);
    increase_reference(plug -> tile = (graphic -> type == WLPixmap
				       ? graphic : NIL));
    return plug;
}

PlugClose(wob)
Plug             wob;
{
    decrease_reference(wob -> graphic);
    WobRelease(wob);
}

/*
 * Set here the dimensions of a plug
 */

UpdatePlugGeometry(plug)
Plug	plug;
{
    WOOL_send(WOOL_get_dimensions, plug -> graphic,
	      (plug -> graphic, &(plug -> box)));
}

PlugEventHandler(plug, evt)
Plug	plug;
XEvent	*evt;
{
    switch (evt -> type) {
    case Expose:
	SetTarget(plug);
	WOOL_send(WOOL_redraw, plug -> graphic,
		  (plug -> graphic, plug));
	break;
    default:
	WLFsm_action(plug -> fsm, plug, evt);
    }
}
	
ReconfigurePlug(plug, culprit)
Plug     plug;
Wob	culprit;
{
    int             width, height;

    if (culprit == (Wob) plug -> parent) {
	XMoveWindow(dpy, plug -> hook, plug -> box.x, plug -> box.y);
    } else if (culprit == (Wob) plug -> graphic) {
	width = plug -> box.width;
	height = plug -> box.height;
	UpdatePlugGeometry(plug);
	if ((plug -> box.width != width) || (plug -> box.height != height)) {
	    XResizeWindow(dpy, plug -> hook,
			  plug -> box.width, plug -> box.height);
	    WOOL_send(WOOL_reconfigure, plug -> parent,
		      (plug -> parent, plug));
	}
    }
    WOOL_send(WOOL_redraw, plug -> graphic, (plug -> graphic, plug));
}

set_plug_bitmap(plug, wl_pixmap)
Plug	plug;
WOOL_OBJECT	wl_pixmap;
{
    unsigned int    graphic_mask;

    if (wl_pixmap -> type == WLAtom || wl_pixmap -> type == WLList)
	return;
    decrease_reference(plug -> graphic);
    increase_reference(plug -> graphic = wl_pixmap);
    graphic_mask = ((unsigned int) WOOL_send(WOOL_open, plug -> graphic,
					  (plug -> graphic, plug)));
    if (graphic_mask != plug -> graphic_mask) {
	plug -> input_mask &= ~plug -> graphic_mask;
	plug -> input_mask |= graphic_mask;
	XSelectInput(dpy, plug -> hook, plug -> input_mask);
    }
    ReconfigurePlug(plug, plug -> graphic);
    decrease_reference(plug -> tile);
    increase_reference(plug -> tile = (wl_pixmap -> type == WLPixmap
                                       ? wl_pixmap : NIL));
}

