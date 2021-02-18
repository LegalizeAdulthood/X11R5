/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/************************************************\
* 						 *
* 	BULL WINDOW MANAGER for X11 .		 *
* 						 *
* 	MODULE defining the Screen Wob Class.	 *
* 						 *
\************************************************/

/*  include  */

#include	<stdio.h>
#include 	"EXTERN.h"
#include 	"wool.h"
#include	"wl_atom.h"	
#include	"wl_list.h"
#include	"wl_number.h"
#include	"wl_string.h"
#include 	"gwm.h"
#include	"wl_fsm.h"
#include	"wl_pixmap.h"
#include	"wl_cursor.h"
#include	"wl_client.h"

/*  local constants  */

/* external */

extern Wob      NewWob();
extern ClientWindow LookUpClient(), DecorateWindow();


extern ScreenOpen(), ScreenClose(), ScreenEventHandler(), ReconfigureScreen();

WOB_METHOD	ScreenClass[] = {
				 0,	/* METHODS_ARRAY */
				 WobEval,
				 WobPrint,
				 WobRelease,
				 WobExecute,
				 WobSet,
				 WobGetCValue,
				 ScreenOpen,
				 ScreenClose,
				 ScreenEventHandler,
                                 (WOB_METHOD) wool_undefined_method_1,
                                 WobGetDimensions,
                                 (WOB_METHOD) wool_undefined_method_2,
                                 (WOB_METHOD) wool_undefined_method_2,
				 (WOB_METHOD) ReconfigureScreen,
			(WOB_METHOD) wool_undefined_method_2,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1
};

/*
 * Set up Screen Wob. Usually called from SetUpWob.
 */

ScreenOpen(screen)
ClientWindow       screen;
{
    screen -> curstate = (int)
	WOOL_send(WOOL_open, screen -> fsm, (screen -> fsm));
    if (screen -> cursor != NIL)
	XDefineCursor(dpy, screen -> hook,
		      ((WOOL_Cursor) screen -> cursor) -> cursor);
    if (screen -> tile != NIL)
	wool_set_screen_background(screen -> tile);
    screen -> input_mask = RootMask | ((WOOL_Fsm) screen -> fsm) -> mask;
    XSelectInput(dpy, screen -> hook, screen -> input_mask);
    SetTarget(screen);
}

/* Initialize screen data in the list of managed screens
 * This function is called BEFORE user profile
 */

GWMScreenContext
RegisterScreen(screen_number)
int 	screen_number;
{
    XSetWindowAttributes wa;

    Context = (GWMScreenContext) Malloc(sizeof(struct _GWMScreenContext));
    bzero(Context, sizeof(struct _GWMScreenContext));

    Context -> screen = screen_number;
    Context -> x_screen = ScreenOfDisplay(dpy, screen_number);
    Context -> width = DisplayWidth(dpy, screen_number);
    Context -> height = DisplayHeight(dpy, screen_number);
    Context -> depth = DisplayPlanes(dpy, screen_number);
    Context -> widthMM = DisplayWidthMM(dpy, screen_number);
    Context -> heightMM = DisplayHeightMM(dpy, screen_number);

    Context -> root = RootWindow(dpy, screen_number);
    Context -> rootWob = (ClientWindow) NewWob(sizeof(struct _ClientWindow));
    Context -> rootWob -> parent = (Wob) NULL;	/* to stop recursion */
    Context -> rootWob -> type = ScreenClass;
    Context -> rootWob -> hook = Context -> root;
    Context -> rootWob -> client = Context -> root;
    Context -> rootWob -> status = RootStatus;
    Context -> rootWob -> window = Context -> rootWob;
    Context -> rootWob -> icon = Context -> rootWob;
    Context -> rootWob -> box.width = Context -> width;
    Context -> rootWob -> box.height = Context -> height;
    Context -> rootWob -> screen = Context;

    /* init root window colormap */
    wa.colormap = Context -> rootWob -> colormap =
	DefaultColormap(dpy, Context -> screen);
    XChangeWindowAttributes(dpy, Context -> root, CWColormap, &wa);
    XInstallColormap(dpy, wa.colormap);
    Context -> InstalledColormapCW = Context -> rootWob;

    WobRecordHook(Context -> rootWob);

    return	Context;
}

/* initialize screen AFTER user profile & root window
 */

SetUpScreen(screen, wl_client)
ClientWindow	screen;
WOOL_Client	wl_client;
{
    if (wl_client -> type != WLClient) {
	wool_puts("\007GWM: screen not described! -- check your .gwmrc!\n");
	exit(1);
    }
    screen -> box.x = 0;
    screen -> box.y = 0;
    screen -> box.borderwidth = 0;
    screen -> box.width = Context -> width;
    screen -> box.height = Context -> height;
    screen -> box.background = wl_client -> background;
    increase_reference(screen -> cursor = wl_client -> cursor);
    increase_reference(screen -> fsm = wl_client -> fsm);
    increase_reference(screen -> menu = wl_client -> menu);
    increase_reference(screen -> property =
		       (WOOL_OBJECT) wl_client -> property);
    /* bordertile is used to store the background */
    increase_reference(screen -> tile = wl_client -> tile);
    increase_reference(screen -> opening = wl_client -> opening);
    increase_reference(screen -> closing = wl_client -> closing);
    increase_reference(screen -> grabs = (WOOL_OBJECT) wl_client -> grabs);
    increase_reference(screen -> icon_plug =
		       (WOOL_OBJECT) wl_client -> icon_plug);
    fix_fsm(&(screen -> fsm));
    screen -> cached_props = (CWCachedProperties)
	Malloc(sizeof(struct _CWCachedProperties));
    bzero(screen -> cached_props, sizeof(struct _CWCachedProperties));
#define update_cached_field(name, val) \
	increase_reference(screen -> cached_props -> name = (WOOL_OBJECT) val)
    update_cached_field(clientclass, NIL_STRING);
    update_cached_field(clientname, NIL_STRING);
    update_cached_field(machinename, NIL_STRING);
    update_cached_field(windowname, NIL_STRING);
    update_cached_field(iconname, NIL_STRING);

    CreateGwmLabelWindow();
}

/* Called when an event is reported to rootWindow.
 * There we detect and decorate newly created windows on first map
 */

ScreenEventHandler(screen, evt)
    ClientWindow screen;
    Event evt;
{
    ClientWindow wob;

    switch (evt->type) {

    case MapRequest:			/* look if this window is not yet
					 * managed to decorate it */
	SetTarget(screen);
	if (evt->xmaprequest.parent == Context->root)
	    if (wob = (ClientWindow) LookUpWob(evt->xmaprequest.window)) {
		if (!(wob->opening))	/* has already been mapped */
		    XMapWindow(dpy, evt->xmaprequest.window);
	    } else {
		if (wob = LookUpClient(evt->xmaprequest.window)) {
		    if (!(wob->opening))
			ClientWindowInitialMap(wob);
		} else {
		    wob = DecorateWindow(evt->xmaprequest.window,
			screen, 1, 1);
		    ClientWindowInitialMap(wob);
		}
	    }
	break;

    case ColormapNotify:		/* the screen colormap has changed:
					 * reflect it if the current wob has
					 * no defined colormap */
	if (evt->xcolormap.new) {	/* colormap has changed */
	    SetTarget(screen);
	    screen->colormap = (evt->xcolormap.colormap == None ?
		DefaultColormap(dpy, Context->screen) :
		evt->xcolormap.colormap);
	    if (Context->InstalledColormapCW == screen ||
		Context->InstalledColormapCW->colormap == None)
		XInstallColormap(dpy, screen->colormap);
	}
	break;

    case UnmapNotify:{			/* iccc: to withdraw a window */
	    ClientWindow cw;

	    if (evt->xunmap.send_event
		&& evt->xunmap.from_configure == False
		&& (cw = LookUpClient(evt->xunmap.window))) {
		if (cw->icon)
		    ClientWindowUnmap(cw->icon);
		ClientWindowUnmap(cw);
		cw->cached_props->user_icon = 0;
		UnDecorateWindow(cw, 1, 0);
	    }
	}
	break;

    case ConfigureRequest:{		/* unmapped window */
	    ClientWindow cw;

	    /* warning: window may have been redecorated meanwhile */
	    if (cw = (ClientWindow)
		LookUpClient(evt->xconfigurerequest.window)) {
		ConfigureRequestEventHandler(cw, evt);	/* was remapped */
	    } else if (evt->xconfigurerequest.window != Context->root) {
		ConfigureUnmappedWindow(evt);	/* not yet known, obey */
	    }
	}
	break;

    case PropertyNotify:		/* GWM_EXECUTE */
	SetTarget(screen);
	if (evt->xproperty.atom == XA_GWM_EXECUTE) {
	    WlExecGWM_EXECUTE();
	} else {
	    WLFsm_action(screen->fsm, screen, evt);
	}
	break;

    default:
	WLFsm_action(screen->fsm, screen, evt);
    }
}

/* Here we clean up everything for the next WM
 */

int
ScreenClose(screen)
ClientWindow       screen;
{
    ClientWindow    cw;

    if (screen -> closing)
	WOOL_send(WOOL_eval, screen -> closing, (screen -> closing));
    if (Context -> WindowCount) {
	cw = Context -> rootWob -> next;
	while (cw) {
	    if (cw -> status & ClientWindowStatus) {
		SetTarget(cw);
		UnDecorateWindow(cw, 1, 1);
	    }
	    cw = cw -> next;
	}
    }
    decrease_reference(screen -> opening);
    decrease_reference(screen -> closing);
    decrease_reference(screen -> grabs);
    decrease_reference(screen -> icon_plug);
    WobRelease(screen);
    return OK;
}

/* the "current screen" functions 
 */

WOOL_OBJECT
wool_current_screen_get()
{
    return (WOOL_OBJECT) WLNumber_make(Context -> screen);
}

WOOL_OBJECT
wool_current_screen_set(number)
WOOL_Number	number;
{
    must_be_number(number, 0);
    if(GWMManagedScreens[number -> number]) {
	SetTarget(GWMManagedScreens[number -> number] -> rootWob);
	return (WOOL_OBJECT) number;
    }else{
	return NIL;
    }
}

/* same with "current root-window" */

WOOL_OBJECT
wool_get_root_window()
{
    return (WOOL_OBJECT) WLNumber_make(Context -> rootWob);
}

WOOL_OBJECT
wool_set_root_window(number)
WOOL_Number     number;
{
    must_be_number(number, 0);
    SetTarget(number -> number);
    return (WOOL_OBJECT) WLNumber_make(Context -> rootWob);
}

/* sets Context to DefaultScreen, or first one... */

SetDefaultScreen()
{
    if (GWMManagedScreens[DefaultScreen(dpy)]) {
	SetTarget(GWMManagedScreens[DefaultScreen(dpy)] -> rootWob);
	GWM_DefaultScreen = DefaultScreen(dpy);
    } else {
	for (ContextPtr = GWMManagedScreens;; ContextPtr++)
	    if (*ContextPtr) {
		SetTarget((*ContextPtr) -> rootWob);
		GWM_DefaultScreen = (*ContextPtr) -> screen;
		return;
	    }
    }
}

int
ScreenOfRoot(window)
Window	window;
{
    int             screen;

    for (screen = 0; screen < ScreenCount(dpy); screen++)
	if (RootWindow(dpy, screen) == window)
	    return screen;
    return -1;
}

/* does nothing
 */

ReconfigureScreen(screen, culprit)
Wob screen, culprit;
{}

/********************************************\
* 					     *
* "Meter" gestion (window to display infos)  *
* 					     *
\********************************************/

/* create the X window for the meter. called at screen initialization
 */

MeterCreate()
{
    XGCValues       gcv;
    XSetWindowAttributes wa;

    Context -> meter.x = 0;
    Context -> meter.y = 0;
    Context -> meter.width = 1;
    Context -> meter.height = 1;
    Context -> meter.anchor_x = Context -> meter.anchor_y = 0;
    Context -> meter.gravity = 1;
    Context -> meter.borderwidth = 0;
    Context -> meter.font = DefaultFont;
    Context -> meter.foreground = Context -> pixel.Back;
    Context -> meter.background = Context -> pixel.Fore;
    Context -> meter.borderpixel = Context -> pixel.Back;
    Context -> meter.horizontal_margin = 2;
    Context -> meter.vertical_margin = 2;
    Context -> meter.string_length = 60;
    Context -> meter.string = (char *) Malloc(60);
    Context -> meter.window = XCreateSimpleWindow(dpy, Context -> root,
				     Context -> meter.x, Context -> meter.y,
			    Context -> meter.width, Context -> meter.height,
					 0, 0, Context -> meter.background);

    gcv.foreground = Context -> meter.foreground;
    gcv.background = Context -> meter.background;
    gcv.function = GXcopy;
    gcv.graphics_exposures = False;
    Context -> meter.gc = XCreateGC(dpy, Context -> root,
      GCForeground | GCBackground | GCFunction | GCGraphicsExposures, &gcv);
    XSelectInput(dpy, Context -> meter.window, MeterMask);
    wa.override_redirect = 1;
    XChangeWindowAttributes(dpy, Context -> meter.window,
			    CWOverrideRedirect, &wa);
    XSetFont(dpy, Context -> meter.gc, Context -> meter.font);
    XSetForeground(dpy, Context -> meter.gc, Context -> meter.foreground);
    XSetBackground(dpy, Context -> meter.gc, Context -> meter.background);
}

/* pop up the meter with room for string
 */

MeterOpen(string)
char *string;
{
    MeterUpdate(string);
    XMapRaised(dpy, Context -> meter.window);
}

/* unmaps the meter
 */

WOOL_OBJECT
MeterClose()
{
    XClearWindow(dpy, Context -> meter.window);
    XUnmapWindow(dpy, Context -> meter.window);
    return NIL;
}

/* displays another string in the meter, updates size.
 */

MeterUpdate(string)
char	*string;
{
    int             up, down, dir, width, height, l;
    XCharStruct     extent;

    XQueryTextExtents(dpy, Context -> meter.font,
		      string, strlen(string),
		      &dir, &up, &down, &extent);
    Context->meter.string_x = Context -> meter.horizontal_margin 
	- extent.lbearing;
    Context->meter.string_y = Context -> meter.vertical_margin + up;
    width = extent.width + 2 * Context -> meter.horizontal_margin;
    height = up + down + 2 * Context -> meter.vertical_margin;
    if (width != Context -> meter.width || height != Context -> meter.height)
	XResizeWindow(dpy, Context -> meter.window,
		      Context -> meter.width = width,
		      Context -> meter.height = height);
    if ((l = strlen(string)) >= Context -> meter.string_length) {
	Context -> meter.string_length = Max(
			      2 * Context -> meter.string_length + 4, l + 1);
	Context -> meter.string = (char *) Realloc(Context -> meter.string,
					       Context -> meter.string_length);
    }
    strcpy(Context -> meter.string, string);
    PlaceWinWithGravity(Context -> meter.window,
			Context -> meter.gravity,
			Context -> meter.anchor_x,
			Context -> meter.anchor_y,
			&(Context -> meter.x),
			&(Context -> meter.y),
			Context -> meter.width,
			Context -> meter.height,
			Context -> meter.borderwidth);
    MeterRefresh();
}

MeterRefresh()
{
    XClearWindow(dpy, Context -> meter.window);
    XDrawString(dpy, Context -> meter.window, Context -> meter.gc,
		Context->meter.string_x, Context->meter.string_y, 
		Context->meter.string, strlen(Context->meter.string));
}   

/* pops menu from wool
 */

WOOL_OBJECT
wool_meter_open(argc, argv)
int argc;
WOOL_Number argv[];
{
    switch(argc){
    case 3:
	Context -> meter.anchor_x = argv[0] -> number;
	Context -> meter.anchor_y = argv[1] -> number;
	MeterOpen(((WOOL_String) argv[2]) -> string);
	break;
    case 1:
	MeterOpen(((WOOL_String) argv[0]) -> string);
	break;
    default:
	return wool_error(BAD_NUMBER_OF_ARGS, argc);
    }
    return NIL;
}

/* displays a string in the meter
 */

WOOL_OBJECT
wool_meter_update(string)
WOOL_String	string;
{
    MeterUpdate(string -> string);
    return (WOOL_OBJECT) string;
}

/* change graphics of meter
 * usage: (meter <key value>...)
 * where key can be:
 * 	font
 * 	background
 * 	foreground
 * 	horizontal-margin
 * 	vertical-margin
 * 	x
 * 	y
 * 	gravity
 * 	borderpixel
 * 	borderwidth
 */

WOOL_OBJECT
wool_meter_modify(argc, argv)
int		argc;
WOOL_Atom	argv[];
{
    WOOL_Atom      *key = argv, *last_key = argv + argc;
    WOOL_List      result;
    WOOL_OBJECT   *result_index;

    if (argc % 2)
	if (argc == 1) {
	    must_be_or_nil(WLList, argv[0], 0);
	    if (argv[0] == (WOOL_Atom) NIL) {
		return NIL;
	    } else {
		key = (WOOL_Atom *) ((WOOL_List) argv[0]) -> list;
		last_key = (WOOL_Atom *) key +
		    ((WOOL_List) argv[0]) -> size;
	    }
	} else
	    wool_error(BAD_NUMBER_OF_ARGS, argc);

    result = wool_list_make(last_key - key);
    result_index = &(result -> list[0]);
    for (; key < last_key; key += 2) {
	WOOL_Atom old_key = *key;
	int old_value;

	if (*key == WA_font) {
	    old_value = Context -> meter.font;
	    Context -> meter.font = get_C_from_object(key[1]);
	    XSetFont(dpy, Context -> meter.gc, Context -> meter.font);
	} else if (*key == WA_background) {
	    old_value = Context -> meter.background;
	    Context -> meter.background = get_C_from_object(key[1]);
	    XSetBackground(dpy, Context -> meter.gc,
			   Context -> meter.background);
	} else if (*key == WA_foreground) {
	    old_value = Context -> meter.foreground;
	    Context -> meter.foreground = get_C_from_object(key[1]);
	    XSetForeground(dpy, Context -> meter.gc,
			   Context -> meter.foreground);
	} else if (*key == WA_horizontal_margin) {
	    old_value = Context -> meter.horizontal_margin;
	    Context -> meter.horizontal_margin = get_C_from_object(key[1]);
	} else if (*key == WA_vertical_margin) {
	    old_value = Context -> meter.vertical_margin;
	    Context -> meter.vertical_margin = get_C_from_object(key[1]);
	} else if (*key == WA_x) {
	    old_value = Context -> meter.anchor_x;
	    Context -> meter.anchor_x = get_C_from_object(key[1]);
	} else if (*key == WA_y) {
	    old_value = Context -> meter.anchor_y;
	    Context -> meter.anchor_y = get_C_from_object(key[1]);
	} else if (*key == WA_gravity) {
	    old_value = Context -> meter.gravity;
	    Context -> meter.gravity = get_C_from_object(key[1]);
	} else if (*key == WA_borderwidth) {
	    old_value = Context -> meter.borderwidth;
	    XSetWindowBorderWidth(dpy, Context -> meter.window,
				  Context -> meter.borderwidth
				  = get_C_from_object(key[1]));
	} else if (*key == WA_borderpixel) {
	    old_value = Context -> meter.borderpixel;
	    XSetWindowBorder(dpy, Context -> meter.window,
				  Context -> meter.borderpixel
				  = get_C_from_object(key[1]));
	} else {
	    if (wool_warning("GWM WARNING: ")) {
		wool_print(*key);
		wool_puts(" is not a key\n");
	    }
	    old_key = NULL;
	}
	if (old_key) {
	    increase_reference(*result_index++ = (WOOL_OBJECT) old_key);
	    increase_reference(*result_index++ =
			       (WOOL_OBJECT) WLNumber_make(old_value));
	}
    }
    if (result -> size = result_index - &(result -> list[0]))
	return (WOOL_OBJECT) result;
    else
	return NIL;
}

/* handles event.
 * returns 1 if was for a meter, 0 otherwise
 */

int
MeterEventHandler(evt)
Event           evt;
{
    ClientWindow    wob;

    if(GWM_is_ending)
	return 1;
    FOR_ALL_SCREENS {
	if (evt -> xany.window == Context -> meter.window) {
	    switch (evt -> type) {
	    case Expose:
		MeterRefresh();
		break;
	    }
	    return 1;
	}
    }END_OF_ALL_SCREENS;
    return 0;
}
