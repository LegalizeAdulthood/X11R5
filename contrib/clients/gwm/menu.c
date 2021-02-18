/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*****************************************************\
* 						      *
* 	BULL WINDOW MANAGER for X11 .		      *
* 						      *
* 		MODULE defining the Menu Wob Class    *
* 						      *
\*****************************************************/

/*  include  */

#include	"EXTERN.h"
#include "wool.h"
#include "wl_atom.h"
#include "wl_number.h"
#include "wl_string.h"
#include "wl_list.h"
#include "gwm.h"
#include "wl_fsm.h"
#include "wl_pixmap.h"
#include "wl_cursor.h"
#include "wl_bar.h"
#include "wl_menu.h"

/*  local constants  */

/*  external  */

extern Wob      NewWob();

extern Menu MenuOpen();
extern MenuClose(), MenuEventHandler(), ReconfigureMenu();

WOB_METHOD       MenuClass[] = {
			       0,	/* METHODS_ARRAY */
			       WobEval,
			       WobPrint,
			       WobRelease,
			       WobExecute,
			       WobSet,
			       WobGetCValue,
			       (WOB_METHOD) MenuOpen,
			       MenuClose,
			       MenuEventHandler,
                               (WOB_METHOD) wool_undefined_method_1,
			       WobGetDimensions,
                               (WOB_METHOD) wool_undefined_method_2,
                               (WOB_METHOD) wool_undefined_method_2,
                               ReconfigureMenu,
			(WOB_METHOD) wool_undefined_method_2,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1
};

unsigned int	MenuMask = 0;

/*  routines  */

Menu	NewMenu(), MenuOpen();

/*
 * Set up a menu
 */

Menu
SetUpMenu(wl_menu)
WOOL_Menu	wl_menu;
{
    Menu menu = NewMenu(Context->rootWob, wl_menu);

    UpdateMenuGeometry(menu);
    MenuOpen(menu);
    return menu;
}

Menu
MenuOpen(menu)
Menu             menu;
{
    int             i;

    check_window_size(menu);
    menu -> hook =
	XCreateSimpleWindow(dpy, Context->root,
			    menu -> box.x, menu -> box.y,
			    menu -> box.width, menu -> box.height,
			    menu -> box.borderwidth,
			    menu -> box.borderpixel, menu -> box.background);
    menu -> status = MenuStatus | TopLevelXWindowStatus;
    WobRecordHook(menu);
    for (i = 0; i < menu -> nbars; i++)
	WOOL_send(WOOL_open, menu -> bars[i], (menu -> bars[i]));
    menu -> curstate = (int) WOOL_send(WOOL_open, menu -> fsm, (menu -> fsm));
    menu -> input_mask = MenuMask | ((WOOL_Fsm) menu -> fsm) -> mask;
    XSelectInput(dpy, menu -> hook, menu -> input_mask); 
    if (menu -> cursor != NIL)
	XDefineCursor(dpy, menu -> hook,
		      ((WOOL_Cursor) menu -> cursor) -> cursor);
    if (menu -> bordertile != NIL)
	XSetWindowBorderPixmap(dpy, menu -> hook,
			       ((WOOL_Pixmap) menu -> bordertile) -> pixmap);
    {
	XSetWindowAttributes wa;	/* menus are OverrideRedirect */

	wa.override_redirect = 1;
	XChangeWindowAttributes(dpy, menu -> hook, CWOverrideRedirect, &wa);
    }
    XMapSubwindows(dpy, menu -> hook);
    return menu;
}

Menu 
NewMenu(parent, wl_menu)
Wob             parent;
WOOL_Menu	wl_menu;
{
    int             i;
    Menu            menu;
    int             dir =
    (wl_menu -> direction == HORIZONTAL ? VERTICAL : HORIZONTAL);
    WOOL_OBJECT     object;

    menu = (Menu) NewWob(sizeof(struct _Menu)
			 + sizeof(Bar) * Max(0, (wl_menu -> bars_size - 1)));
    wl_menu = (WOOL_Menu) wool_type_or_evaluate(wl_menu, WLMenu);
    menu -> type = MenuClass;
    menu -> parent = parent;
    menu -> screen = ((ClientWindow) parent) -> screen;
    menu -> box.borderwidth = wl_menu -> borderwidth;
    menu -> box.borderpixel = wl_menu -> borderpixel;
    menu -> box.background = wl_menu -> background;
    menu -> direction = wl_menu -> direction;
    menu -> bar_separator = wl_menu -> bar_separator;
    menu -> min_width = DefaultMenuMinWidth;
    menu -> max_width = DefaultMenuMaxWidth;
    increase_reference(menu  -> menu = 
                       wool_type_or_evaluate(wl_menu -> menu, WLMenu));
    increase_reference(menu -> property = (WOOL_OBJECT) wl_menu -> property);
    increase_reference(menu -> bordertile =
		    wool_type_or_evaluate(wl_menu -> bordertile, WLPixmap));
    increase_reference(menu -> fsm =
		       wool_type_or_evaluate(wl_menu -> fsm, WLFsm));
    increase_reference(menu -> cursor =
		       wool_type_or_evaluate(wl_menu -> cursor, WLCursor));
    menu -> nbars = wl_menu -> bars_size;
    for (i = 0; i < wl_menu -> bars_size; i++) {
	object = wool_type_or_evaluate(wl_menu -> bars[i], WLBar);
	UpdateClientWindowBar(menu, &(menu -> bars[i]), object, dir);
    }
    return menu;
}

MenuClose(menu)
Menu             menu;
{
    int i;

    for (i = 0; i < menu -> nbars; i++) {
	BarClose(menu -> bars[i]);
    }
    WobRelease(menu);
}

/*
 * Set here the dimensions of a menu
 */

UpdateMenuGeometry(menu)
Menu	menu;
{
    int             i, width = 0, length = 0;
    int             dir = menu -> direction;

    for (i = 0; i < menu -> nbars; i++) {
	if (dir == HORIZONTAL) {
	    menu -> bars[i] -> box.x = length;
	    menu -> bars[i] -> box.y = 0;
	} else {
	    menu -> bars[i] -> box.x = 0;
	    menu -> bars[i] -> box.y = length;
	}
	length += UpdateBarWidth(menu -> bars[i]) +
	    menu -> bar_separator;
	width = Max(NaturalBarLength(menu -> bars[i]), width);
    }
    width = Min(Max(width, menu -> min_width), menu -> max_width);
    length = Max(length - menu -> bar_separator, 1);
    if (dir == HORIZONTAL) {
	menu -> box.width = length;
	menu -> box.height = width;
    } else {
	menu -> box.width = width;
	menu -> box.height = length;
    }
    for (i = 0; i < menu -> nbars; i++) {
	if (dir == HORIZONTAL)
	    menu -> bars[i] -> box.height = width
		- 2 * menu -> bars[i] -> box.borderwidth;
	else
	    menu -> bars[i] -> box.width = width
		- 2 * menu -> bars[i] -> box.borderwidth;
	UpdateBarLength(menu -> bars[i]);
    }

}

/*
 * Since a menu will receive events for its sons, we must transmit them!
 */

MenuEventHandler(menu, evt)
Menu	menu;
XEvent	*evt;
{
    int             i;

    switch (evt -> type) {
    case GWMUserEvent:
	WLFsm_action(menu -> fsm, menu, evt);
	if (GWM_Propagate_user_events)
	    for (i = 0; i < menu -> nbars; i++)
		if (menu -> bars[i])
		    WOOL_send(WOOL_process_event, menu -> bars[i],
			      (menu -> bars[i], evt));
	break;
    case ClientMessage:
	if (evt -> xclient.message_type == XA_WM_PROTOCOLS
	    && evt -> xclient.data.l[0] == (long) XA_WM_DELETE_WINDOW) {
				/* to not be sent messages later */
	    if (((ClientWindow) menu -> parent) -> client_wob == (Wob) menu)
		((ClientWindow) menu -> parent) -> client_wob = 0;
	    MenuClose(menu);
	} else {
	    WLFsm_action(menu -> fsm, menu, evt);
	}
	break;
    case PropertyNotify:	/* HACK for placed menus */
	WOOL_send(WOOL_process_event, menu -> parent, (menu -> parent, evt));
	break;
    default:
	WLFsm_action(menu -> fsm, menu, evt);
    }
}
	
ReconfigureMenu(menu, culprit)
Menu     menu;
Bar      culprit;		/* only bar */
{
    int             i, width = menu -> box.width, height = menu ->box.height;

    UpdateMenuGeometry(menu);
    for (i = 0; i < menu -> nbars; i++)
	if (menu -> bars[i])
	    WOOL_send(WOOL_reconfigure, menu -> bars[i],
		      (menu -> bars[i], menu));
    if ((width != menu -> box.width) || (height != menu -> box.height)) {
	XResizeWindow(dpy, menu -> hook,
		      menu -> box.width, menu -> box.height);
	if (menu -> parent && menu -> parent -> type == ClientWindowClass &&
	    ((ClientWindow) menu -> parent) -> client_wob == (Wob) menu) {
	    WOOL_send(WOOL_reconfigure, menu -> parent,
		      (menu -> parent, menu));
	    UpdateMenuNormalHints(menu, 0, 0, 0,
				  menu -> box.width, menu -> box.height);
	}
    }
}

/*
 * Place a fixed menu on the screen
 * usage (place-menu "name-of-this-menu" menu [x y])
 */

WOOL_OBJECT
PlaceFixedMenu(argc, argv)
int argc;
WOOL_String	*argv;
{
    int             x = 0, y = 0;
    WOOL_Menu       wl_menu;
    Window          w;
    XClassHint      classhints;
    XWMHints        wm_hints;
    ClientWindow    wob;
    XSetWindowAttributes wa;
    WOOL_String     icon_name;
    WOOL_OBJECT     starts_iconic;
    WOOL_String     wl_string;

    if (argc < 2 || argc == 3 || argc > 4)
	return wool_error(BAD_NUMBER_OF_ARGS, argc);
    must_be_string(argv[0], 0);
    if (argc == 4) {
	must_be_number(argv[2], 2);
	must_be_number(argv[3], 3);
	x = ((WOOL_Number) argv[2]) -> number;
	y = ((WOOL_Number) argv[3]) -> number;
    }
    wl_menu = (WOOL_Menu) wool_type_or_evaluate(argv[1], WLMenu);
    w = wl_menu -> wob_menu -> hook;

    /* Now, set the names */
    XStoreName(dpy, w, argv[0] -> string);
    get_val_from_context(icon_name, WA_icon_name);
    get_val_from_context(starts_iconic, WA_starts_iconic);
    if(icon_name != (WOOL_String) NIL)
	XSetIconName(dpy, w, icon_name -> string);
    else
	XSetIconName(dpy, w, argv[0] -> string);
    wl_string = (WOOL_String) WOOL_send(WOOL_eval, WA_class_name,
					(WA_class_name));
    if (wl_string->type == WLString)
	classhints.res_class = wl_string->string;
    else
	classhints.res_class = "Gwm";
    wl_string = (WOOL_String) WOOL_send(WOOL_eval, WA_client_name,
					(WA_client_name));
    if (wl_string->type == WLString)
	classhints.res_name = wl_string->string;
    else
	classhints.res_name = "menu";
    XSetClassHint(dpy, w, &classhints);
    /* machine_name */
    XChangeProperty(dpy, w, XA_WM_CLIENT_MACHINE, XA_STRING, 8, PropModeReplace,
		    ((WOOL_String) wool_host_name)->string,
		    strlen(((WOOL_String) wool_host_name)->string));

    /* normal_hints */
    UpdateMenuNormalHints(wl_menu -> wob_menu, (argc == 4 ? 1 : 0), x, y,
			  wl_menu -> wob_menu -> box.width,
			  wl_menu -> wob_menu -> box.height);

    /* WM_hints */
    if (starts_iconic == NIL) {
	wm_hints.flags = 0;
    } else {
	wm_hints.flags = StateHint;
	wm_hints.initial_state = WM_STATE_Iconified;
    }
    XSetWMHints(dpy, w, &wm_hints);

    /* participate in the delete_window protocol */
    {
#define GWM_menus_number_of_protocols 1
	Window          protocols[GWM_menus_number_of_protocols];

	protocols[0] = XA_WM_DELETE_WINDOW;

	XChangeProperty(dpy, w, XA_WM_PROTOCOLS, XA_ATOM, 32, PropModeReplace,
			(unsigned char *)protocols, GWM_menus_number_of_protocols);
    }

    /* a placed menu is no more OverrideRedirect */
    wa.override_redirect = 0;
    XChangeWindowAttributes(dpy, w, CWOverrideRedirect, &wa);

    /* now decorate it and map it */
    wob = (ClientWindow) DecorateWindow(w, Context -> rootWob, 0, 1);

    ClientWindowInitialMap(wob);

    return (WOOL_OBJECT) WLNumber_make(wob);
}

UpdateMenuNormalHints(menu, pos, x, y, width, height)
Menu menu;
int pos, x, y, width, height;
{
    XSizeHints      normal_hints;

    normal_hints.flags = (pos ? USPosition : 0) | PMinSize | PMaxSize;
    normal_hints.min_width = normal_hints.max_width = width;
    normal_hints.min_height = normal_hints.max_height = height;
    if (pos) {
	normal_hints.x = x;
	normal_hints.y = y;
	XMoveWindow(dpy, menu -> hook, x, y);
    }
    XSetNormalHints(dpy, menu -> hook, &normal_hints);
}
