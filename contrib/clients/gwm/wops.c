/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/**************************************************************\
* 							       *
* 	X11 BULL WINDOW MANAGER.			       *
* 							       *
* 	MODULE FOR OPERATIONS THAT MAY BE CALLED FROM Lisp.    *
* 							       *
\**************************************************************/

/*  include  */

#include        "EXTERN.h"
#include "wool.h"
#include "wl_atom.h"
#include "gwm.h"
#include "wl_number.h"
#include "wl_string.h"
#include "wl_client.h"
#include "wl_menu.h"
#include "wl_name.h"
#include "wl_event.h"

/*  local constants  */

/*  external  */

extern XError(), NoXError();
extern WOB_METHOD ClientWindowClass[];
extern WOOL_OBJECT UserMoveWindow(), UserResizeWindow();
extern WOOL_OBJECT MwmLikeUserResizeWindow();
extern ClientWindow ClientWindowLeader();

/*  routines  */

char *NoRootError="Window function called on root window%s";
#define NoRoot(w) ((w)->status != RootStatus ? (w) : \
		   (wool_error(NoRootError, ""),w))

/*
 * GetTarget returns window having status in ancestors of current wob
 */

Wob
GetTarget(flag)
unsigned long   flag;
{
    Wob             wob = TargetWob;

    while ((wob -> parent != NULL) && (wob -> status & flag) == 0)
	wob = wob -> parent;
    return wob;
}

/*
 * SetTarget makes TargetWob, TargetWindow and Context
 */

SetTarget(wob)
Wob   wob;
{
    extern WOOL_Namespace ContextNamespace;

    TargetWob = wob;
    while (!(wob -> status & (TargetWindowStatus)))
	wob = wob -> parent;
    TargetWindow = (ClientWindow) wob;
    Context = TargetWindow -> screen;
    ContextNamespace -> current = Context -> screen;
}

WOOL_OBJECT
MapWindow(argc, argv)
int argc;
WOOL_Number argv[];
{
    ClientWindow    cw;

    if (argc)
	cw = (ClientWindow) argv[0] -> number;
    else
	cw = TargetWindow;
    ClientWindowMap(cw);
    return NIL;
}

WOOL_OBJECT
UnmapWindow(argc, argv)
int argc;
WOOL_Number argv[];
{
    ClientWindow    cw;

    if (argc)
	cw = (ClientWindow) argv[0] -> number;
    else
	cw = TargetWindow;
    ClientWindowUnmap(cw);
    return NIL;
}

WOOL_OBJECT
IconifyWindow()
{
    if (TargetWindow) {
	if (TargetWindow -> mapped) {
	    XUnmapWindow(dpy, TargetWindow -> hook);
	    if (TargetWindow -> client && !TargetWindow -> client_wob)
		XUnmapWindow(dpy, TargetWindow -> client);
	    TargetWindow -> mapped = 0;
	}
	if (TargetWindow -> status & IconStatus) {	/* de-iconify */
	    SetTarget(TargetWindow -> window);
	} else {			/* iconify */
	    if (!(TargetWindow -> icon)) {
		RealizeIconWindow(TargetWindow);
	    }
	    SetTarget(TargetWindow -> icon);
	}
	ClientWindowMap(TargetWindow);
    }
    return NIL;
}

/* move window returns () if Ok, else:
 *	0 if X problem with server/window
 * 	1 if pointer was not on screen
 * 	2 if user cancelled move
 *	3 if button was released before entering function
 */

WOOL_OBJECT
MoveWindow(argc, argv)
int argc;
WOOL_Number argv[];
{
    int i;
    ClientWindow    cw = TargetWindow;

    for(i=0; i< argc; i++)		/* MOVE HOOK */
	must_be_number(argv[i], i);
    switch (argc) {
    case 1:
        cw = (ClientWindow) argv[0] -> number;
    case 0:
	return UserMoveWindow(cw);
	break;
    case 2:
	XMoveWindow(dpy, TargetWindow -> hook,
		    TargetWindow -> box.x = argv[0] -> number,
		    TargetWindow -> box.y = argv[1] -> number);
	SendSyntheticMoveEvent(TargetWindow);
	break;
    default:
	XMoveWindow(dpy, ((Wob) (argv[0] -> number)) -> hook,
		    ((Wob) (argv[0] -> number)) -> box.x = argv[1] -> number,
		    ((Wob) (argv[0] -> number)) -> box.y = argv[2] -> number);
	SendSyntheticMoveEvent(argv[0] -> number);
	break;
    }
    return NIL;
}

/* resize window returns () if Ok, else:
 *	0 if X problem with server/window
 *      1 if pointer was not on screen
 *      2 if user cancelled resize
 *	3 if button was released before entering function
 */

WOOL_OBJECT
ResizeWindow(argc, argv)
int argc;
WOOL_Number argv[];
{
    int             width, height;
    int i;
    ClientWindow    cw = TargetWindow;

    for(i=0; i< argc; i++)
	must_be_number(argv[i], i);
    switch (argc) {
    case 1:
	cw = (ClientWindow) argv[0] -> number;
    case 0:
	if (cw -> client)
	    if (GWM_resize_style)
		return MwmLikeUserResizeWindow(cw);
	    else
	    	return UserResizeWindow(cw);
	break;
    case 2:
	if (TargetWindow -> client)
	    width = argv[0] -> number;
	height = argv[1] -> number;
	dims_outer_to_inner(TargetWindow, &width, &height);
	conform_to_hints(&(TargetWindow -> cached_props -> normal_hints),
			 &width, &height);
	XResizeWindow(dpy, TargetWindow -> client, width, height);
	ReconfigureClientWindow(TargetWindow, TargetWindow -> client);
	break;
    default:
	if (!(cw = ((ClientWindow) argv[0] -> number)) -> client)
	    return NIL;
	width = argv[1] -> number;
	height = argv[2] -> number;
	dims_outer_to_inner(cw, &width, &height);
	conform_to_hints(&(cw -> cached_props -> normal_hints), 
			 &width, &height);
	XResizeWindow(dpy, cw -> client, width, height);
        ReconfigureClientWindow(cw, cw -> client);
	break;
    }
    return NIL;
}

/*
 * Raise window (optionally % other top-level window)
 */

WOOL_OBJECT
RaiseWindow(argc, argv)
int argc;
WOOL_Number argv[];
{
    XWindowChanges values;

    switch (argc) {
    case 0:
	XRaiseWindow(dpy, TargetWindow -> hook);
	break;
    case 1:
	must_be_number(argv[0], 0);
	values.sibling = ((Wob) (argv[0] -> number)) -> hook;
	values.stack_mode = Above;
	XConfigureWindow(dpy, TargetWindow -> hook,
			 CWStackMode | CWSibling, &values);
	break;
    }
    return NIL;
}

/*
 * Lower window (optionally % other top-level window)
 */

WOOL_OBJECT
LowerWindow(argc, argv)
int argc;
WOOL_Number argv[];
{
    XWindowChanges values;

    switch (argc) {
    case 0:
	XLowerWindow(dpy, TargetWindow -> hook);
	break;
    case 1:
        must_be_number(argv[0], 0);
        values.sibling = ((Wob) (argv[0] -> number)) -> hook;
        values.stack_mode = Below;
        XConfigureWindow(dpy, TargetWindow -> hook,
                         CWStackMode | CWSibling, &values);
	break;
    }
    return NIL;
}

WOOL_OBJECT
KillWindow(argc, argv)
int argc;
WOOL_Number argv[];
{
    ClientWindow    cw = TargetWindow;
    Wob             wob;

    if (argc)
	cw = (ClientWindow) argv[0] -> number;
    /*  kill always main window, not icon! */
    cw = cw -> window;
    if (wob = (Wob) LookUpWob(cw -> client))
	WOOL_send(WOOL_close, wob, (wob));	/* don't kill gwm! */
    else if (cw -> client)
	XKillClient(dpy, cw -> client);
    return NIL;
}

WOOL_OBJECT
ReDecorateWindow(argc, argv)
int argc;
WOOL_Number argv[];
{
    ClientWindow    cw = TargetWindow;
    int             new_target = 0;
    int             was_mapped;
    Window	    window;
    WOOL_List       window_group;

    if (argc)
	cw = (ClientWindow) (argv[0] -> number);
    if (cw && cw -> type == ClientWindowClass) {
	GWM_ProcessingExistingWindows = 1;
	if (cw == TargetWindow)
	    new_target = 1;
	cw = cw -> window;
	window = cw -> client;
	if (was_mapped = cw -> mapped)
	    ClientWindowUnmap(cw);
	SetTarget(cw);
	if (cw == ClientWindowLeader(cw)) {
	    increase_reference(window_group =
			       (WOOL_List) wool_window_group_get());
	} else {
	    window_group = 0;
	}
	UnDecorateWindow(cw, 1, 1);
	if (cw = (ClientWindow) DecorateWindow(window,
					       Context->rootWob, 1, 1)) {
	    ClientWindowInitialMap(cw);
	    if (was_mapped) {
		ClientWindowMap(cw);
		wool_process_unmap_events(window);
	    }
	    if (window_group && window_group != (WOOL_List) NIL) {
		int i;
		for (i=1; i<window_group->size; i++) {
		    AddWindowToGroupLeader(
					  ((WOOL_Number)window_group->list[i])
					   ->number, cw);
		}
	    }
	    GWM_ProcessingExistingWindows = 0;
	    if (new_target)
		SetTarget(cw);
	    return (WOOL_OBJECT) WLNumber_make(cw);
	} else {
	    XMapWindow(dpy, window);
	}
	decrease_reference(window_group);
    }
    return NIL;
}

/* returns NIL if OK
 * (pop-up [menu [item]])
 * if menu = () takes default
 * item is the nth item in the menu (starting with 0).
 * If pointer is not on screen, do nothing and returns 0
 * if cannot grab, returns 1, Ok= ()
 * returns pointer (modifier + buttons status) mask
 */

WOOL_OBJECT
PopMenu(argc, argv)
int argc;
WOOL_Menu argv[];
{
    WOOL_Menu       wl_menu = (WOOL_Menu) TargetWob -> menu;
    int             x, y, rx, ry, item = 0, i;
    Window          root, child;
    Menu            menu;
    int             floating = 1;
    unsigned int    mask;


    for (i = 0; i < argc; i++) {
	if (argv[i] -> type == WLNumber)
	    item = ((WOOL_Number) argv[i]) -> number;
	else if (argv[i] -> type == WLMenu)
	    wl_menu = argv[i];
	else if (argv[i] == (WOOL_Menu) WA_here)
	    floating = 0;
    }
    menu = wl_menu -> wob_menu;
    set_grab(Context -> rootWob);
    XQueryPointer(dpy, Context -> root, &root, &child,
		  &x, &y, &rx, &ry, &mask);
    if (root == Context -> root) {
	if (floating) {
	    PlaceMenu(item, menu, x, y, TargetWob);
	    XMoveWindow(dpy, menu -> hook, menu -> box.x, menu -> box.y);
	}
	XMapRaised(dpy, menu -> hook);
	if (NIL == set_grab(menu)) {	/* ok */
	    XSync(dpy, 0);
	    wool_process_exposes();
	    menu -> parent = (TargetWob == (Wob) menu
			      ? (Wob) Context -> rootWob :TargetWob) ;
	    return NIL;
	} else {			/* grab failed */
	    remove_grab(0);
	    XUnmapWindow(dpy, menu -> hook);
	    return (WOOL_OBJECT) WLNumber_make(1);
	}
    } else {				/* not good screen */
	remove_grab(0);
	return (WOOL_OBJECT) WLNumber_make(0);
    }
}

/* de-pop a menu previously popped by PopMenu
 */

WOOL_OBJECT
UnpopMenu(argc, argv)
int argc;
WOOL_Menu argv[];
{
    Menu            menu;
    WOOL_Menu       wl_menu;

    menu = (Menu) GetTarget(MenuStatus);
    if (argc) {
	wl_menu = argv[0];
	if (wl_menu == (WOOL_Menu) NIL)
	    return NIL;
	if (wl_menu -> type == WLMenu)
	    menu = wl_menu -> wob_menu;
	else
	    menu = (Menu) ((WOOL_Number) wl_menu) -> number;
    }
    remove_grab(menu);
    XUnmapWindow(dpy, menu -> hook);
    menu -> parent = (Wob) Context -> rootWob;
    XSync(dpy, 0);
    return (WOOL_OBJECT) WLNumber_make(menu);
}

PlaceMenu(item, menu, x, y, wob)
int 	item;			/* item to place it */
Menu	menu;			/* the menu */
int	x,y;			/* mouse coordinates */
Wob	wob;			/* triggering wob */
{
    int             oldx, oldy;
    int             bw = 2 * menu -> box.borderwidth;

    if (item >= menu -> nbars)		/* if out put to last */
	item = menu -> nbars - 1;
    /* set pointer in item-th item */
    oldx = menu -> box.x = x - menu -> box.width / 2;
    oldy = menu -> box.y = y - (menu -> bars[item] -> box.y +
				menu -> bars[item] -> box.height / 2);
    /* place full pop on screen */
    if (menu -> box.x < 0)
	menu -> box.x = 0;
    if (menu -> box.y < 0)
	menu -> box.y = 0;
    if (menu -> box.x + menu -> box.width + bw > Context -> width)
	menu -> box.x = Context -> width - menu -> box.width - bw;
    if (menu -> box.y + menu -> box.height + bw > Context -> height)
	menu -> box.y = Context -> height - menu -> box.height - bw;
    if (menu -> box.x != oldx || menu -> box.y != oldy)
	XWarpPointer(dpy, None, None, 0, 0, 0, 0,
		     menu -> box.x - oldx, menu -> box.y - oldy);
}

WOOL_OBJECT
CirculateWindowsDown()
{
    XCirculateSubwindowsDown(dpy, Context->root);
    return NIL;
}

WOOL_OBJECT
CirculateWindowsUp()
{
    XCirculateSubwindowsUp(dpy, Context->root);
    return NIL;
}
	
/*
 * the "current-window" functions
 */

WOOL_OBJECT
wool_current_window()
{
    if (TargetWindow)
	return (WOOL_OBJECT) WLNumber_make(TargetWindow);
    else
	return NIL;
}

WOOL_OBJECT
wool_set_current_window(number)
WOOL_Number 	number;
{
    must_be_number(number, 0);
    if (WobIsValid(number->number)
	&& (
	    (((Wob)(number->number))->type == ClientWindowClass)
	    || (((Wob)(number->number))->type == ScreenClass))) {
	SetTarget(number -> number);
	return (WOOL_OBJECT) number;
    } else {
    	return wool_error(INTERNAL_ERROR, "Not a Window!");
    }
}

WOOL_OBJECT
wool_current_window_is_mapped()
{
    if (TargetWindow -> mapped)
	return TRU;
    else
	return NIL;
}

WOOL_OBJECT
wool_current_window_name()
{
    return (WOOL_OBJECT) NoRoot(TargetWindow) -> cached_props -> windowname;
}

WOOL_OBJECT
wool_current_window_name_set(name)
WOOL_String	name;
{
    XStoreName(dpy, NoRoot(TargetWindow) -> client, name -> string);
    return (WOOL_OBJECT) name;
}

WOOL_OBJECT
wool_current_window_client_name()
{
    return (WOOL_OBJECT) NoRoot(TargetWindow) -> cached_props -> clientname;
}

WOOL_OBJECT
wool_current_window_client_class()
{
    return (WOOL_OBJECT) NoRoot(TargetWindow) -> cached_props -> clientclass;
}

WOOL_OBJECT
wool_current_window_machine_name()
{
    return (WOOL_OBJECT) NoRoot(TargetWindow) -> cached_props -> machinename;
}

WOOL_OBJECT
wool_current_window_icon_name()
{
    return (WOOL_OBJECT) NoRoot(TargetWindow) -> cached_props -> iconname;
}

WOOL_OBJECT
wool_current_window_geometry(measure)
int measure;
{
    Window          root;
    unsigned int    measures[6];

    XGetGeometry(dpy, TargetWindow -> client,
		 &root, (int *)&measures[0], (int *)&measures[1], &measures[2],
		 &measures[3], &measures[4], &measures[5]);
    return (WOOL_OBJECT) WLNumber_make(measures[measure]);
}

/* sets Context to those of the root of a window */

int
ContextOfXWindow(window)
Window window;
{
    Window          root;
    unsigned int     measures[6];
    int result;

    XSetErrorHandler(NoXError);
    result = XGetGeometry(dpy, window,
                 &root, (int *)&measures[0], (int *)&measures[1], &measures[2],
                 &measures[3], &measures[4], &measures[5]);
    XSync(dpy, 0);
    XSetErrorHandler(XError);
    if (!result) 
	return result;
    result = ScreenOfRoot(root);
    if (result == -1)
	return result;
    SetTarget(GWMManagedScreens[result] -> rootWob);
    return 0;
}

WOOL_OBJECT
wool_current_client_x()
{
    return wool_current_window_geometry(0);
}

WOOL_OBJECT
wool_current_client_y()
{
    return wool_current_window_geometry(1);
}

WOOL_OBJECT
wool_current_client_width()
{
    return wool_current_window_geometry(2);
}

WOOL_OBJECT
wool_current_client_height()
{
    return wool_current_window_geometry(3);
}

WOOL_OBJECT
wool_current_window_x()
{
    return (WOOL_OBJECT) WLNumber_make(TargetWindow -> box.x);
}

WOOL_OBJECT
wool_current_window_y()
{
    return (WOOL_OBJECT) WLNumber_make(TargetWindow -> box.y);
}

WOOL_OBJECT
wool_current_window_width()
{
    return (WOOL_OBJECT) WLNumber_make(TargetWindow -> box.width);
}

WOOL_OBJECT
wool_current_window_height()
{
    return (WOOL_OBJECT) WLNumber_make(TargetWindow -> box.height);
}

/* returns client-managed icon window, if any, but first check that it exists
 */

WOOL_OBJECT
wool_current_window_icon_window()
{
    Window dummy_win;

    if (NoRoot(TargetWindow) -> cached_props -> wm_hints.flags
	& IconWindowHint
	&& !TrapXErrors(XGetTransientForHint(dpy,
		 TargetWindow -> cached_props -> wm_hints.icon_window,
					     &dummy_win))
	) {
	return (WOOL_OBJECT) WLNumber_make(
		      TargetWindow -> cached_props -> wm_hints.icon_window);
    }
    return NIL;
}

/* each time this function is called, construct the icon pixmap from the hints
 * (this to allow for applications changing their icons and user to change
 * color via the foreground and background variables)
 */

WOOL_OBJECT
wool_current_window_icon_bitmap()
{
    if (TargetWindow -> cached_props -> wm_hints.flags & IconPixmapHint) {
	return (WOOL_OBJECT)
	    WLPixmap_make(TargetWindow -> cached_props ->
			  wm_hints.icon_pixmap);
    } else {
	return NIL;
    }
}

WOOL_OBJECT
wool_current_window_icon_bitmap_id()
{
    if (TargetWindow -> cached_props -> wm_hints.flags & IconPixmapHint) {
	return (WOOL_OBJECT) WLNumber_make(TargetWindow -> cached_props ->
			  wm_hints.icon_pixmap);
    } else {
	return NIL;
    }
}

WOOL_OBJECT
wool_current_window_US_position()
{
    if (NoRoot(TargetWindow) -> status & IconStatus) {
	if (TargetWindow -> cached_props -> wm_hints.flags & IconPositionHint)
	    return TRU;
    } else {
	if (TargetWindow && TargetWindow -> cached_props -> normal_hints.flags
	    & USPosition)
	    return TRU;
    }
    return NIL;
}

WOOL_OBJECT
wool_current_window_US_size()
{
    if (TargetWindow && TargetWindow -> cached_props -> normal_hints.flags 
 	 & USSize)
	return TRU;
    else
	return NIL;
}

WOOL_OBJECT
wool_current_window_P_position()
{
   if (TargetWindow && TargetWindow -> cached_props -> normal_hints.flags
       & PPosition)
     return TRU;
   else
     return NIL;
}

WOOL_OBJECT
wool_current_window_P_size()
{
    if (TargetWindow && TargetWindow -> cached_props -> normal_hints.flags 
 	 & PSize)
	return TRU;
    else
	return NIL;
}

WOOL_OBJECT
wool_current_window_is_transient_for()
{
    if (NoRoot(TargetWindow) 
	&& TargetWindow -> cached_props -> transient_for) {
	return TargetWindow -> cached_props -> transient_for;
    } else {
	return NIL;
    }
}

WOOL_OBJECT
wool_window_map_as_icon_get()
{
    if ((TargetWindow && TargetWindow -> cached_props -> wm_hints.flags
	 & StateHint)
	&& (TargetWindow -> cached_props -> wm_hints.initial_state == 3))
	return TRU;
    else
	return NIL;
}

WOOL_OBJECT
wool_window_map_as_icon_set(flag)
WOOL_OBJECT	flag;
{
    if (TargetWindow) {
	TargetWindow -> cached_props -> wm_hints.flags |= StateHint;
	TargetWindow -> cached_props -> wm_hints.initial_state
	    = (flag == NIL ? 1 : 3);
    }
    return flag;
}

/*
 * set focus on
 * 	current client if no arg (do nothing if input hint is false)
 * 	other window's client if arg
 * 	if arg = NIL, rootwindow
 */

WOOL_OBJECT
wool_set_focus(argc, argv)
int	argc;
WOOL_Number	argv[];
{
    ClientWindow    cw = TargetWindow;

    if (!cw)
	return NIL;
    if (argc) {
	if (argv[0] == (WOOL_Number) NIL) {
	    XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot,
			   CurrentTime);
	    return TRU;
	} else {
	    cw = (ClientWindow) argv[0] -> number;
	}
    }
    if (GWM_No_set_focus)
	return NIL;
    if (cw -> type == ClientWindowClass) {
	if (cw -> cached_props -> wm_take_focus) {
	    send_protocol_message(cw -> client, XA_WM_TAKE_FOCUS, 0, 0, 0);
	} else if (GWM_check_input_focus_flag || ((cw -> cached_props ->
					      wm_hints.flags & InputHint) &&
		  (cw -> cached_props -> wm_hints.input) && cw -> client)) {
	    TrapXErrors(XSetInputFocus(dpy, cw -> client, RevertToPointerRoot,
				       CurrentTime));
	} else
	    return NIL;
    } else {
	TrapXErrors(XSetInputFocus(dpy, cw -> hook, RevertToPointerRoot,
				   CurrentTime));
    }
    return TRU;
}

/*
 * returns [x y modifiers screen]
 */

WOOL_OBJECT
wool_current_mouse_position()
{
    unsigned int    ptrmask;	/* state of ptr when queried */
    Window          root, sub_window;
    int             root_x, root_y, cur_x, cur_y;
    WOOL_List	    wl_list = wool_list_make(4);

    root=0;
    XQueryPointer(dpy,
		  Context->root, &root, &sub_window,
		  &root_x, &root_y, &cur_x, &cur_y, &ptrmask);
    increase_reference(wl_list -> list[0] = (WOOL_OBJECT)
	WLNumber_make(cur_x));
    increase_reference(wl_list -> list[1] = (WOOL_OBJECT)
	WLNumber_make(cur_y));
    increase_reference(wl_list -> list[2] = (WOOL_OBJECT)
	WLNumber_make(ptrmask));
    increase_reference(wl_list -> list[3] = (WOOL_OBJECT)
        (root ? WLNumber_make(ScreenOfRoot(root))
	      : WLNumber_make(0)));
    return (WOOL_OBJECT) wl_list;
}

/* warps pointer to pos
 * (warp-pointer x y [window-relative-to])
 */

WOOL_OBJECT
wool_warp_pointer(argc, argv)
int		argc;
WOOL_Number	argv[];
{
    Window          dest_w = None;

    if (argc < 2 || argc > 3)
	wool_error(BAD_NUMBER_OF_ARGS, argc);
    must_be_number(argv[0], 0);
    must_be_number(argv[1], 1);
    if (argc == 3) {
	must_be_number(argv[2], 2);
	dest_w = ((Wob) (argv[2] -> number)) -> hook;
    }
    XWarpPointer(dpy, None, dest_w,
		 0, 0, 0, 0, argv[0] -> number, argv[1] -> number);
    return NIL;
}

/*
 * logical size of a window is size divided by resize increments (+ base)
 */

WOOL_OBJECT
wool_window_logical_size_get()
{
    WOOL_List       wl_list = wool_list_make(2);
    unsigned int    w = 0, h = 0;

    if (TargetWindow -> client) {
	pixel_to_logical_size(&(TargetWindow -> cached_props -> normal_hints),
	 TargetWindow -> inner_width, TargetWindow -> inner_height, &w, &h);
    }
    increase_reference(wl_list -> list[0] = (WOOL_OBJECT) WLNumber_make(w));
    increase_reference(wl_list -> list[1] = (WOOL_OBJECT) WLNumber_make(h));
    return (WOOL_OBJECT) wl_list;
}

WOOL_OBJECT
wool_window_logical_size_set(list)
WOOL_List	list;
{
    unsigned int    w, h;

    if (list -> type != WLList || list == (WOOL_List) NIL || list -> size != 2
	|| list -> list[0] -> type != WLNumber
	|| list -> list[1] -> type != WLNumber)
	bad_argument(list, 0, "list of two numbers");
    logical_to_pixel_size(&(TargetWindow -> cached_props -> normal_hints),
			  ((WOOL_Number) list -> list[0]) -> number,
			  ((WOOL_Number) list -> list[1]) -> number,
			  &w, &h);
    XResizeWindow(dpy, TargetWindow -> client, w, h);
    ReconfigureClientWindow(TargetWindow, TargetWindow -> client);
    return NIL;
}

/* moves a window at x,y and gravity
 * WARNING: xpt & yptr are pointers on SHORTS
 */

PlaceWinWithGravity(w, gravity, anchor_x, anchor_y, xptr, yptr, width, height, 
		    borderwidth)
Window  w;
int     gravity, anchor_x, anchor_y;
short   *xptr, *yptr;
unsigned int width, height, borderwidth;
{
    int x = *xptr, y = *yptr;

    switch((gravity - 1) % 3) {
    case 0:
	x = anchor_x;
	break;
    case 1:
	x = anchor_x - width/2 - borderwidth;
	break;
    case 2:
	x = anchor_x - width - 2*borderwidth;
	break;
    }
    switch((gravity - 1) / 3) {
    case 0:
	y = anchor_y;
	break;
    case 1:
	y = anchor_y - height/2 - borderwidth;
	break;
    case 2:
	y = anchor_y - height - 2*borderwidth;
	break;
    }
    if (*xptr != x || *yptr != y) {
	*xptr = x;
	*yptr = y;
	XMoveWindow(dpy, w, x, y);
    }
}
