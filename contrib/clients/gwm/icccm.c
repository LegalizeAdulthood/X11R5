/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/***********************************************\
* 					        *
* GWM: icccm.c				        *
* Miscellaneous utilities to enforce the ICCCM  *
* 					        *
\***********************************************/

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_atom.h"
#include "wl_list.h"
#include "wl_string.h"
#include "gwm.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "wl_number.h"

extern XError(), NoXError();
DECLARE_strchr;

/* Used to tell a client that it has been moved
 */

SendSyntheticMoveEvent(cw)
ClientWindow	cw;
{
    XConfigureEvent	event;

    if(cw -> client_wob)		/* do not send to ourselves */
	return;
    event.type = ConfigureNotify;
    event.display = dpy;
    event.event = cw -> client;
    event.window = cw -> client;
    event.x = cw -> inner_x + cw ->box.x + cw -> box.borderwidth;
    event.y = cw -> inner_y + cw ->box.y + cw -> box.borderwidth;
    event.width = cw -> inner_width;
    event.height = cw -> inner_height;
    event.border_width = cw -> inner_borderwidth;
    event.above = cw -> hook;
    event.override_redirect = False;
    XSendEvent(dpy, cw -> client, False, StructureNotifyMask, &event);
}

/* for defining another icon for WM_STATE property
 */

WOOL_OBJECT
wool_wm_state_user_icon_get()
{
    if (TargetWindow -> cached_props -> user_icon)
	return (WOOL_OBJECT)
	    WLNumber_make(TargetWindow -> cached_props -> user_icon);
    else
	return NIL;
}

WOOL_OBJECT
wool_wm_state_user_icon_set(number)
WOOL_Number	number;
{
    must_be_number(number, 0);
    TargetWindow -> cached_props -> user_icon = (ClientWindow)
	number -> number;
    Update_XA_WM_STATE(TargetWindow);
    return (WOOL_OBJECT) number;
}

/* updating this window's WM_STATE once another icon is defined
 */

WOOL_OBJECT
wool_wm_state_update()
{
    Update_XA_WM_STATE(TargetWindow);
    return NIL;
}

/*&WN window-wm-state
 * getting this window's WM_STATE once another icon is defined
 */

WOOL_OBJECT
wool_wm_state_get(argc, argv)
int     argc;
WOOL_Number argv[];
{
    ClientWindow    cw = TargetWindow;
    Atom            actualtype;
    int             actualformat;
    unsigned long   nitems;
    unsigned long   bytesafter;
    WM_STATE_PROP   wm_state;

    if (argc) {
	must_be_number(argv[0], 0);
	cw = (ClientWindow) argv[0] -> number;
    }
    cw = cw -> window;
    if ((Success == XGetWindowProperty(dpy, cw -> client, XA_WM_STATE, 0,
		      WM_STATE_PROP_Length, False, XA_WM_STATE, &actualtype,
			    &actualformat, &nitems, &bytesafter, &wm_state))
	&& nitems && wm_state) {
	switch (wm_state -> state) {
	case WM_STATE_Iconified:
	    return (WOOL_OBJECT) WA_icon;
	case WM_STATE_Normal:
	    return (WOOL_OBJECT) WA_window;
	}
    }
    return NIL;
}

/* Colormaps
 * colormap focus management:
 */

WOOL_OBJECT
wool_set_colormap_focus(argc, argv)
int	argc;
WOOL_Number argv[];
{
    ClientWindow    cw = TargetWindow;

    if (argc) {
	if (argv[0] == (WOOL_Number) NIL) {
	    GWM_set_default_colormap();
	    Context -> InstalledColormapCW = Context -> rootWob;
	    return NIL;
	}
	must_be_number(argv[0], 0);
	cw = (ClientWindow) argv[0] -> number;
    }
    if (cw -> colormap) {
	if (cw -> colormap != Context -> InstalledColormapCW -> colormap)
	    XInstallColormap(dpy, cw -> colormap);
	if (cw -> cached_props -> colormap_windows)
	    cw -> cached_props -> colormap_windows_index = 0;
    } else {
	GWM_set_default_colormap();
    }
    Context -> InstalledColormapCW = cw;
    return NIL;
}

GWM_set_default_colormap()
{
    if (Context -> InstalledColormapCW -> colormap &&
	Context -> InstalledColormapCW -> colormap
	!= Context -> rootWob -> colormap)
	XInstallColormap(dpy, Context -> rootWob -> colormap);
}

/* Sub windows colormaps
 * colormap focus management:
 */

WOOL_OBJECT
wool_set_subwindow_colormap_focus(argc, argv)
int	argc;
WOOL_Number argv[];
{
    ClientWindow    cw = TargetWindow;
    int             index;
    XWindowAttributes wa;
    Colormap        current_colormap;

    if (argc) {
	must_be_number(argv[0], 0);
	index = argv[0] -> number;
    }
    if (cw == Context -> InstalledColormapCW &&
	cw -> cached_props -> colormap_windows) {
	if (argc) {			/* absolute index */
	    cw -> cached_props -> colormap_windows_index =
		index % cw -> cached_props -> colormap_windows_size;
	    XGetWindowAttributes(dpy,
				 cw -> cached_props -> colormap_windows[
			cw -> cached_props -> colormap_windows_index], &wa);
	    if (wa.colormap)
		XInstallColormap(dpy, wa.colormap);
	} else {			/* increment index */
	    XGetWindowAttributes(dpy,
				 cw -> cached_props -> colormap_windows[
			cw -> cached_props -> colormap_windows_index], &wa);
	    current_colormap = wa.colormap;
	    for (index = cw -> cached_props -> colormap_windows_index + 1;
		 index % cw -> cached_props -> colormap_windows_size !=
		 cw -> cached_props -> colormap_windows_index;
		 index++) {
		index = index % cw -> cached_props -> colormap_windows_size;
		XGetWindowAttributes(dpy,
			cw -> cached_props -> colormap_windows[index], &wa);
		if (wa.colormap != current_colormap) {
		    cw -> cached_props -> colormap_windows_index = index;
		    XInstallColormap(dpy, wa.colormap);
		    break;
		}
	    }
	}
    }
    return NIL;
}


/* updating a colormap for a watched window in a WM_COLORMAP_WINDOWS list
 */

Update_colormap_in_colormap_windows_list(evt)
XColormapEvent	*evt;
{
    ClientWindow    cw;

    if (ContextOfXWindow(evt -> window))
	return;				/* if no more window, returns */
    cw = Context -> InstalledColormapCW;
    if (cw -> cached_props -> colormap_windows_index &&
	evt -> window == cw -> cached_props -> colormap_windows[
			       cw -> cached_props -> colormap_windows_index]
	&& evt -> new)
	if (evt -> colormap)
	    XInstallColormap(dpy, evt -> colormap);
	else
	    XInstallColormap(dpy, Context->rootWob -> colormap);
}

/* declare accepted icons sizes for future clients
 */

WOOL_OBJECT
wool_set_wm_icon_sizes(argc, argv)
int     argc;
WOOL_Number argv[];
{
    int i;
    XIconSize	icon_sizes;

    if(argc != 6)
    return wool_error(BAD_NUMBER_OF_ARGS, argc);
    for(i=0;i<6;i++){
	must_be_number(argv[i], i);
    }
    icon_sizes.min_width = argv[0] -> number;
    icon_sizes.min_height = argv[1] -> number;
    icon_sizes.max_width = argv[2] -> number;
    icon_sizes.max_height = argv[3] -> number;
    icon_sizes.width_inc = argv[4] -> number;
    icon_sizes.height_inc = argv[5] -> number;
    XSetIconSizes(dpy, Context->root, &icon_sizes, 6);
    return NIL;
}

/* get WM_COMMAND
 */

WOOL_OBJECT
wool_get_wm_command()
{
    Atom actual_type;
    int actual_format = 0;
    unsigned long nitems = 0L, leftover = 0L;
    unsigned char *prop = NULL;
    WOOL_List list;
    int i, nstrings = 0, n;
    unsigned char *p;

    if (XGetWindowProperty (dpy, TargetWindow -> client, XA_WM_COMMAND,
			    0L, 1000000L, False,
			    AnyPropertyType, &actual_type, &actual_format,
			    &nitems, &leftover, &prop) == Success &&
	actual_type != None && prop && nitems) {
	for (i = 0; i < nitems; i++)
	    if (prop[i] == '\0')
		nstrings ++;
	list = wool_list_make(nstrings);
	p = prop;
	for (n = 0; n < nstrings; n++) {
	    increase_reference(list -> list[n] =
			       (WOOL_OBJECT) WLString_make(p));
	    while(*p++);
	}
	XFree(prop);
	return (WOOL_OBJECT) list;
    }

    return NIL;

}

/* When not-yet mapped configures its window, we must execute the 
 * configure request!
 */

ConfigureUnmappedWindow(evt)
XConfigureRequestEvent	*evt;
{
    XWindowChanges	window_changes;

    window_changes.x = evt -> x;
    window_changes.y = evt -> y;
    window_changes.width = evt -> width;
    window_changes.height = evt -> height;
    window_changes.border_width = evt -> border_width;
    window_changes.sibling = evt -> above;
    window_changes.stack_mode = evt -> detail;

    TrapXErrors(XConfigureWindow(dpy,
				 evt -> window, evt -> value_mask,
				 &window_changes));
}

/* PROTOCOLS communication via user events
 */

send_protocol_message(window, protocol, timestamp, data_size, data)
Window	window;
Atom   	protocol;
Time 	timestamp;			/* 0 for CurrentTime */
int 	data_size;			/* 0 to 3 32-bit quantities */
Card32 *data;
{
    XClientMessageEvent event;

    event.type = ClientMessage;
    event.window = window;
    event.message_type = XA_WM_PROTOCOLS;
    event.format = 32;
    event.data.l[0] = (long) protocol;
    if (timestamp)
	event.data.l[1] = (long) timestamp;
    else
	event.data.l[1] = (long) CurrentTime;
    data_size = Min(3, data_size);
    if (data_size)
	bcopy(data, &(event.data.l[2]), data_size * 4);
    if (data_size < 3)
	bzero(&(event.data.l[2 + data_size]), (3 - data_size) * 4);

    TrapXErrors(XSendEvent(dpy, window, False, 0, &event));
}

/* WM_DELETE_WINDOW
 * if window participate in protocol, sends message and returns TRUE
 * else unmaps it and its associated icon (go to withdrawn) and returns NIL
 */

WOOL_OBJECT
wool_delete_window(argc, argv)
int argc;
WOOL_Number *argv;
{
    ClientWindow    cw = TargetWindow -> window;

    if (argc) {
	must_be_number(argv[0], 0);
	cw = ((ClientWindow) argv[0] -> number) -> window;
    }
    if (cw -> client
	&& cw -> cached_props -> wm_delete_window) {
	send_protocol_message(cw -> client, XA_WM_DELETE_WINDOW, 0, 0, 0);
	return TRU;
    } else {
	return NIL;
    }
}

/* WM_SAVE_YOURSELF
 * if window participate in protocol, sends message and returns TRUE
 * else returns NIL
 */

WOOL_OBJECT
wool_save_yourself(argc, argv)
int argc;
WOOL_Number *argv;
{
    ClientWindow    cw = TargetWindow -> window;

    if (argc) {
	must_be_number(argv[0], 0);
	cw = ((ClientWindow) argv[0] -> number) -> window;
    }
    if (cw -> client && !cw -> client_wob
	&& cw -> cached_props -> wm_save_yourself) {
	send_protocol_message(cw -> client, XA_WM_SAVE_YOURSELF, 0, 0, 0);
	return TRU;
    } else {
	return NIL;
    }
}

/* Gravity: 123
 *          456
 *          789
 * (Gravity - 1) % 3: x: 0 1 2
 * (Gravity - 1) / 3: y: 0 1 2
 * 1 is center, -1 is just client mustn't move
 */


/* obeys the Window Gravity hints
 * direction is 1 on decoration, 0 on un-decoration
 * adjusts in place the cw->box.x/y dims
 */

ObeyWinGravityHint(cw, direction)
ClientWindow    cw;
int             direction;
{
    int x_gravity, y_gravity;
    int x_offset, y_offset;

#ifdef NOBASEDIMS		/* old pre -R4 code */
    x_gravity = y_gravity = -1;
    if (!GWM_ProcessingExistingWindows
	&& !cw -> cached_props -> new_normal_hints) {
	if (cw -> cached_props -> normal_hints.flags & USPosition) {
	    cw -> box.x = cw -> cached_props -> normal_hints.x;
	    cw -> box.y = cw -> cached_props -> normal_hints.y;
	} else if (cw -> cached_props -> normal_hints.flags & PPosition) {
	    cw -> box.x = cw -> cached_props -> normal_hints.x;
	    cw -> box.y = cw -> cached_props -> normal_hints.y;
	} 
    }
#else /* NOBASEDIMS */
    if (cw -> cached_props -> normal_hints.flags & PWinGravity) {
	x_gravity = (cw -> cached_props -> normal_hints.win_gravity - 1) % 3;
	y_gravity = (cw -> cached_props -> normal_hints.win_gravity - 1) / 3;
    } else {
	x_gravity = y_gravity = -1;
    }
#endif /* NOBASEDIMS */

    switch(x_gravity) {
    case 0:			/* west */
	x_offset = 0;
	break;
    case 1:			/* center */
	x_offset =  cw -> old_inner_borderwidth - cw -> box.borderwidth
	    + (cw -> inner_width - cw -> box.width)/2;
	break;
    case 2:			/* east */
	x_offset = cw -> inner_width - cw -> box.width
	    + 2 * (cw -> old_inner_borderwidth - cw -> box.borderwidth);
	break;
    default:			/* client don't move */
	x_offset = cw -> old_inner_borderwidth - cw -> inner_borderwidth
	    - cw -> inner_x - cw -> box.borderwidth;
    }

    switch(y_gravity) {
    case 0:			/* north */
	y_offset = 0;
	break;
    case 1:			/* center */
	y_offset = cw -> old_inner_borderwidth - cw -> box.borderwidth
	    + (cw -> inner_height - cw -> box.height)/2;
	break;
    case 2:			/* south */
	y_offset =  cw -> inner_height - cw -> box.height
	    + 2 * (cw -> old_inner_borderwidth - cw -> box.borderwidth);
	break;
    default:			/* client don't move */
	y_offset = cw -> old_inner_borderwidth - cw -> inner_borderwidth
	    - cw -> inner_y - cw -> box.borderwidth;
    }
    trace1(6, "window 0x%x:" , cw->client);
    trace2(6, "at %d, %d, offseted by:" , cw->box.x, cw->box.y);
    trace2(6, " %d, %d\n" , x_offset, y_offset);
    cw->box.x += (direction ?  x_offset : - x_offset);
    cw->box.y += (direction ?  y_offset : - y_offset);
}


    
/* misc utilities */

/* replaces ., blanks,  and * by _ for using as class in Xrm
 * returns arg if not changed, new string if changed so as not to modify
 * the original string
 */

WOOL_OBJECT
MakeResourceIdentifier(string)
WOOL_String string;
{
    char *p;
    WOOL_String new_string = 0;

    must_be_string(string, 1);
    for (p = string->string; *p; p++)
	if (*p == '.' || *p == '*' || *p == ' ' || *p == '\t') {
	    if (!new_string)
		new_string = WLString_make(string->string);
	    new_string->string[p - string->string] = '_';
	}

    if (new_string)
	return (WOOL_OBJECT) new_string;
    else
	return (WOOL_OBJECT) string;
}

/* create an unmapped window whose ID is stored on the root window of each
 * managed screen in the GWM_RUNNING property
 * current screen is given in the context
 */

CreateGwmLabelWindow()
{
    XSetWindowAttributes wa;
    

    wa.override_redirect = True;
    Context->GwmWindow = XCreateWindow (dpy, Context->root, 
					-100, -100, 10, 10, 0, 0,
					InputOnly, CopyFromParent,
					CWOverrideRedirect,
					&wa);
    XChangeProperty(dpy, Context->root, XA_GWM_RUNNING, XA_GWM_RUNNING,
	32, PropModeReplace, &(Context->GwmWindow), 1);
    XChangeProperty(dpy, Context->GwmWindow, XA_GWM_RUNNING, XA_GWM_RUNNING,
	32, PropModeReplace, &(Context->GwmWindow), 1);
}

/* FindToplevelWindow
 * given a window, move back in hierarchy to return the toplevel one
 * If given root, returns 0
 */

Window
FindToplevelWindow(w)
    Window w;
{
    Window parent = 0;
    Window         *children;		/* list of root sons */
    unsigned int    nchildren;		/* number of children */
    Window root;

    if (w == Context->root) {
	return 0;
    }
    
    for (;;) {
	if(!XQueryTree(dpy, w, &root, &parent, &children, &nchildren)) {
	    return 0;
	}
	if (parent == Context->root) {
	    return w;
	}
	w = parent;
    }
}

/* DecoratedWindow
 * given a X window, returns the GWM window associated to, decorating it in
 * the process if needed.
 * used to look for window group leaders and transient_for masters
 * returns 0 if failed
 */

ClientWindow
DecoratedWindow(w)
    Window w;
{
    ClientWindow cw = (ClientWindow) LookUpClient(w);
    if (cw) {
	return cw;
    } else {
	cw = (ClientWindow) LookUpWob(w);
	if (cw) {
	    if (cw->status | ClientWindowStatus) {
		return 0;
	    } else {
		Window win = FindToplevelWindow(w);

		if (win) {
		    return (ClientWindow)
			DecorateWindow(win, Context->rootWob, 1, 0);
		} else {
		    return 0;
		}		    
	    }
	} else {
	    cw = (ClientWindow) DecorateWindow(w, Context->rootWob, 1, 0);
	    return cw;
	}
    }
}
