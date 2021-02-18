/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*************************************************************\
* 							      *
* GWM: Wool-gwm.c					      *
* 							      *
* WOOL/GWM interface.					      *
* 							      *
* Here are defined Wool objects referring to X objects	      *
* To add a function:					      *
*        - declare it (coded in or extern)		      *
*        - add it to the declaration of predefined functions  *
*          in the wool_init function			      *
* 							      *
\*************************************************************/

/*  include  */

#include <stdio.h>
#include "EXTERN.h"
#include "wool.h"
#include "wl_number.h"
#include "wl_atom.h"
#include "wl_string.h"
#include "wl_list.h"
#include "wl_func.h"
#include "wl_active.h"
#include "wl_pointer.h"
#include "gwm.h"
#include "wl_event.h"
#include "wl_fsm.h"
#include "wl_label.h"
#include "wl_pixmap.h"
#include "wl_cursor.h"
#include "wl_plug.h"
#include "wl_bar.h"
#include "wl_menu.h"
#include "wl_client.h"
#include "wl_coll.h"
#include "wl_name.h"
#include <X11/Xresource.h>
#include "INTERN.h"

/*
 * external:
 * declare here all functions defined in GWM_init_wool_before_profile
 */

extern WOOL_OBJECT
	MapWindow(),
	UnmapWindow(),
	MoveWindow(),
	ResizeWindow(),
	LowerWindow(),
	RaiseWindow(),
	IconifyWindow(),
	KillWindow(),
	ReDecorateWindow(),
	CirculateWindowsDown(),
	CirculateWindowsUp(),
	PopMenu(),
	PlaceFixedMenu(),
	UnpopMenu(),
	wool_set_focus(),
	wool_current_wob(),
	wool_current_wob_set(),
	wool_current_wob_width(),
	wool_current_wob_height(),
	current_wob_set_bitmap(),
	current_wob_set_borderpixel(),
	current_wob_set_borderwidth(),
	current_wob_set_background(),
	current_wob_get_bitmap(),
	current_wob_get_borderpixel(),
	current_wob_get_borderwidth(),
	current_wob_get_background(),
	current_wob_invert_aera(),
	wool_current_window(),
	wool_set_current_window(),
	wool_current_window_is_mapped(),
	wool_current_window_name(),
	wool_current_window_name_set(),
	wool_current_window_client_name(),
	wool_current_window_client_class(),
	wool_current_window_machine_name(),
	wool_current_window_icon_name(),
	wool_current_window_x(),
	wool_current_window_y(),
	wool_current_window_width(),
	wool_current_window_height(),
	wool_current_client_x(),
	wool_current_client_y(),
	wool_current_client_width(),
	wool_current_client_height(),
	wool_current_window_icon_window(),
	wool_current_window_icon_bitmap(),
	wool_current_window_icon_bitmap_id(),
	wool_current_window_US_position(),
	wool_current_window_US_size(),
	wool_current_window_P_position(),
	wool_current_window_P_size(),
	wool_current_window_is_transient_for(),
	Wob_set_property(),
	Wob_get_property(),
	wool_last_key_struck(),
	wool_send_key_to_client(),
	wool_send_keycode_to_client(),
	wool_logical_coords(),
	wool_process_exposes(),
	wool_current_mouse_position(),
	wool_meter_open(),
	MeterClose(),
	wool_meter_update(),
	wool_window_logical_size_get(),
	wool_window_logical_size_set(),
	wool_window_map_as_icon_get(),
	wool_window_map_as_icon_set(),
	Wob_get_menu(),
	Wob_set_menu(),
	Wob_get_fsm(),
	Wob_set_fsm(),
	wool_last_user_event(),
	wool_resend_event(),
	wool_window_group_get(),
	wool_window_group_set(),
	wool_xpm_pixmap_make(),
	wool_draw_line(),
	wool_wob_cursor_get(),
	wool_wob_cursor_set(),
	wool_wm_state_user_icon_set(),
	wool_wm_state_user_icon_get(),
	wool_wm_state_update(),
	wool_set_colormap_focus(),
	wool_set_wm_icon_sizes(),
	wool_set_subwindow_colormap_focus(),
	wool_set_grabs(), wool_unset_grabs(),
        wool_set_grab(), wool_remove_grab(),
	wool_delete_window(),
	wool_save_yourself(),
	wool_meter_modify(),
	wool_current_screen_get(),
	wool_current_screen_set(),
	wool_warp_pointer(),
	wool_get_root_window(), wool_set_root_window(),
	WLMenu_wob(),
	wool_wm_state_get(),
	wool_ungrab_and_replay_event(),
	wool_wob_x_get(), wool_wob_y_get(),
	wool_makes_replayable(),
	wool_get_triggering_event_state(),
	wool_get_triggering_event_code(),
	wool_get_triggering_event_x(),
	wool_get_triggering_event_y(),
	wool_get_triggering_event_x_relative(),
	wool_get_triggering_event_y_relative(),
	wool_get_triggering_event_time(),
	wool_event_was_due_to_a_grab(),
        wool_send_button_to_client(),
        MakeResourceIdentifier(),
        wool_allow_events(),
    	LookUpWobW(),
        wool_draw_rectangle(),
        wool_draw_text(),
        wool_wob_is_valid(), wool_window_is_valid(),
        Window2ClientW(), Client2WindowW(),
	wool_get_wm_command();
/*---*/

#ifdef SHAPE
extern WOOL_OBJECT wool_window_has_shaped_client();
#endif /* SHAPE */

extern XError(), NoXError();
extern char * getenv();
extern ClientWindow init_managed_windows(), get_next_managed_window();
extern WOOL_OBJECT GetWobOfNumber();

/*  local defs  */

XrmDatabase WLRSC_database = NULL;

/*  global variables */

/*  routines  */

void            GWM_init_wool_before_profile();

/*
 * Initialize Wool. Define subrs and global known from WOOL.
 */

int
InitWool()
{
    init_new_standard_wool_methods();
    if (wool_init(GWM_init_wool_before_profile))
	return FatalError;
    /* put here the initialisations to be done AFTER user profile */
    return OK;
}

/* Sets new methods to standard Wool types
 */

EXT WOOL_OBJECT WLString_get_dimensions();	/* see wl_label.c */

init_new_standard_wool_methods()
{
    WLString[WOOL_get_dimensions] = WLString_get_dimensions;
}

/* refreshes the screen or window if specified
 * very simple: creates/destroy a window just over the managed one
 */

WOOL_OBJECT
wool_refresh(argc, argv)
int argc;
WOOL_Number     argv[];
{
    XSetWindowAttributes wa;
    Window          wd;
    ClientWindow    cw;

    if (argc) {
	must_be_number(argv[0], 0);
	cw = (ClientWindow) argv[0] -> number;
    }
    wd = XCreateSimpleWindow(dpy, (argc ? cw -> hook : Context->root), 0, 0,
			     (argc ? cw -> box.width : Context->width),
			     (argc ? cw -> box.height : Context->height),
			     0, Context -> pixel.Back, Context -> pixel.Back);
    wa.override_redirect = 1;
    XChangeWindowAttributes(dpy, wd, CWOverrideRedirect, &wa);
    XMapWindow(dpy, wd);
    XDestroyWindow(dpy, wd);
    XFlush(dpy);
    return NIL;
}

/* gets the (default) visual type of screen
 * returns an atom that can be of:
 * mono, color or gray
 */

WOOL_OBJECT
wool_get_visual_type()
{
    switch (DefaultVisual(dpy, Context -> screen) -> class) {
    case StaticGray:
    case GrayScale:
	if (Context -> depth > 1)
	    return (WOOL_OBJECT) WA_gray;
	else
	    return (WOOL_OBJECT) WA_mono;
    default:
	return (WOOL_OBJECT) WA_color;
    }
}

/* sets the backgound of the screen in the current context.
 * can be given a WOOL_Number (pixel) or WOOL_Pixmap (pixmap)
 */

WOOL_OBJECT
wool_set_screen_background(background)
WOOL_Number     background;
{
    if (background -> type == WLNumber) {
	XSetWindowBackground(dpy, Context->root,
			     background -> number);
    } else if (background -> type == WLPixmap) {
	decrease_reference(((ClientWindow) Context->rootWob) -> tile);
	increase_reference(((ClientWindow) Context->rootWob) -> tile =
			   (WOOL_OBJECT) background);
	XSetWindowBackgroundPixmap(dpy, Context->root,
				   ((WOOL_Pixmap) background) -> pixmap);
    } else {
	wool_puts("Not a background: ");
	wool_print(background);
	wool_newline();
    }
    XClearWindow(dpy, Context->root);
    return NIL;
}

WOOL_OBJECT
wool_set_screen_saver(argc, argv)
int argc;
WOOL_Number	argv[];
{
    if (argc != 4)
	wool_error(BAD_NUMBER_OF_ARGS, argc);
    XSetScreenSaver(dpy, argv[0] -> number, argv[1] -> number,
		    argv[2] -> number, argv[3] -> number);
    return NIL;
}

WOOL_OBJECT
wool_set_acceleration(num, den)
WOOL_Number     num, den;
{
    XChangePointerControl(dpy, 1, 0, num -> number, den -> number, 0);
    return NIL;
}

WOOL_OBJECT
wool_set_threshold(threshold)
WOOL_Number     threshold;
{
    XChangePointerControl(dpy, 0, 1, 1, 1, threshold -> number);
    return NIL;
}

/* Keyboard bell ringer: (bell [volume percentage])
 */

WOOL_OBJECT
wool_keyboard_bell_ring(argc, argv)
int argc;
WOOL_Number argv[];
{
    int             percent = 0;

    if (argc) {
	must_be_number(argv[0], 0);
	percent = argv[0] -> number;
    }
    XBell(dpy, percent);
    return NIL;
}

/*
 * change mapping by specifiyng
 * keycode keysym keysym-with-mod1 keysym-with-mod2 ...
 * WARNING:  XChangeKeyboardMapping MALLOCS! (do not re-change same keys!)
 */

WOOL_OBJECT
wool_change_keyboard_mapping(argc, argv)
int argc;
WOOL_Number argv[];
{
    KeySym         *keysyms = (KeySym *) Malloc((argc - 1) * sizeof(KeySym));
    int             i;

    for (i = 0; i < argc - 1; i++) {
	keysyms[i] = argv[i + 1] -> number;
    }
    XChangeKeyboardMapping(dpy, argv[0] -> number, argc - 1, keysyms, 1);
    Free(keysyms);
    return NIL;
}

/*
 * converts a key name to a keysym
 */

WOOL_OBJECT
wool_string_to_keysym(string)
WOOL_String string;
{
    KeySym          keysym;

    must_be_string(string, 0);
    return (WOOL_OBJECT)
    	WLNumber_make(XStringToKeysym(string -> string));
}
    
/* translation beetween keysyms and keycodes
 */

WOOL_OBJECT
wool_keysym_to_keycode(key)
WOOL_Number key;
{
    must_be_number(key, 0);
    return (WOOL_OBJECT) WLNumber_make(XKeysymToKeycode(dpy, key->number));
}

WOOL_OBJECT
wool_keycode_to_keysym(key, index)
WOOL_Number key, index;
{
    must_be_number(key, 0);
    must_be_number(index, 1);
    return (WOOL_OBJECT) WLNumber_make(XKeycodeToKeysym(
				      dpy, key -> number, index -> number));
}

/*
 * Selection interface:
 * Active values:
 * 	cut-buffer: access to cut-buffer 0 (for xterm use)
 */

WOOL_OBJECT
wool_cut_buffer_0_get()
{
    int             nbytes;
    char           *buffer = XFetchBytes(dpy, &nbytes);
    WOOL_String     string_of_buffer;

    if (buffer) {
	string_of_buffer = WLString_make(buffer);
	XFree(buffer);
	return (WOOL_OBJECT) string_of_buffer;
    } else {
	return (WOOL_OBJECT) NIL_STRING;
    }
}

WOOL_OBJECT
wool_cut_buffer_0_set(string)
WOOL_String	string;
{
    XStoreBytes(dpy, string -> string, strlen(string -> string));
    return (WOOL_OBJECT) string;
}

/* buffer i becomes buffer i+incr %8 */

WOOL_OBJECT
wool_rotate_cut_buffers(incr)
WOOL_Number incr;
{
    must_be_number(incr, 0);
    XRotateBuffers(dpy, incr -> number);
    return NIL;
}

/*
 * RESOURCE MANAGER INTERFACE
 * (WLRSC)
 * 
 * (WLRSC_put description value)
 * (WLRSC_get name class) --> value
 * 
 * types:
 * 	0	WOOL_OBJECT (auto-typed!)
 * 
 * KNOWN_BUG: if putting twice the same description,
 * 	previous value is not freed!
 */

WOOL_OBJECT
WLRSC_put(resource, value)
WOOL_String 	resource;
WOOL_OBJECT	value;
{
    XrmValue xrm_value;

    must_be_string(resource, 0);
    xrm_value.size = sizeof(WOOL_OBJECT);
    increase_reference(value);
    xrm_value.addr = (caddr_t) &value;
    XrmPutResource(&WLRSC_database, resource -> string, 0, &xrm_value);
    return value;
}

WOOL_OBJECT
WLRSC_get(name, class)
WOOL_String 	name, class;
{
    XrmValue        xrm_value;
    char *	    type;

    must_be_string(name, 0);
    must_be_string(class, 1);
#ifdef DEBUG
    {
	int numdn = 0, numdc = 0;
	char *p;
	for (p = name -> string; *p; p++)
	    if (*p == '.')
		numdn++;
	for (p = class -> string; *p; p++)
	    if (*p == '.')
		numdc++;
	if (numdn != numdc) {
	    char tmp[2000];
	    strcpy(tmp, name->string);
	    strcat(tmp, ", ");
	    strcat(tmp, class->string);
	    return wool_error("Bad resource fetch %s", tmp);
	}
    }
#endif

    if (False ==
	XrmGetResource(WLRSC_database, name -> string, class -> string,
		       &type, &xrm_value))
	return NIL;
    else
	return (WOOL_OBJECT) *((WOOL_OBJECT *) (xrm_value.addr));
}

/*
 * get any property of current window
 */

WOOL_OBJECT
wool_get_window_property(name)
WOOL_String name;
{
    Atom            actual_type;
    Atom            property_name;
    int             actual_format;
    unsigned long   nitems;
    unsigned long   bytes_after;
    unsigned char  *buffer = 0;
    WOOL_OBJECT     result = NIL;
    WOOL_Pointer    x_atom;

    must_be_string(name, 0);
    if (wool_self_pointer_make(name -> string, '\030', &x_atom)) {
	property_name = *(x_atom -> ptr);
    } else {
	property_name = XInternAtom(dpy, name -> string, False);
	*(x_atom -> ptr) = property_name;
    }
    if (Success == XGetWindowProperty(dpy, TargetWindow -> client,
				   property_name, 0, 8000,
				      GWM_delete_read_properties,
       AnyPropertyType, &actual_type, &actual_format, &nitems, &bytes_after,
				   &buffer)
	&& buffer && nitems) {
	switch (actual_type) {
	case XA_STRING:
	    result = (WOOL_OBJECT) WLString_make(buffer);
	    break;
	case XA_INTEGER:
	    result = (WOOL_OBJECT) WLNumber_make(*((int *) buffer));
	    break;
	}
	XFree(buffer);
	return result;
    } else {
	return NIL;
    }
}

WOOL_OBJECT
wool_set_window_property(name, value)
WOOL_String name;
WOOL_String value;
{
    Atom            actual_type;
    Atom            property_name;
    int             actual_format;
    unsigned long   nitems;
    unsigned char  *buffer;
    WOOL_Pointer    x_atom;
    WOOL_TYPE       type;

    must_be_string(name, 0);
    if (wool_self_pointer_make(name -> string, '\030', &x_atom)) {
	property_name = *(x_atom -> ptr);
    } else {
	property_name = XInternAtom(dpy, name -> string, False);
	*(x_atom -> ptr) = property_name;
    }

    type = value -> type;
    if (type == WLString || type == WLAtom || type == WLActive ||
	type == WLPointer || type == WLName) {
	actual_type = XA_STRING;
	actual_format = 8;
	if (type == WLString)
	    buffer = (unsigned char *) value -> string;
	else
	    buffer = (unsigned char *) ((WOOL_Atom) value) -> p_name;
	nitems = strlen(buffer) + 1;
    } else if (type == WLNumber) {
	actual_type = XA_INTEGER;
	actual_format = 32;
	buffer = (unsigned char *) &(((WOOL_Number) value) -> number);
	nitems = 1;
    } else {
	bad_argument(value, 1, "string or number");
    }

    if (Success != XChangeProperty(dpy, TargetWindow -> client,
				   property_name, actual_type, actual_format,
				   PropModeReplace, buffer, nitems))
	return NIL;
    else
	return (WOOL_OBJECT) value;
}

/* executes GWM_EXECUTE string in current window
 */

WlExecGWM_EXECUTE()
{
    WOOL_OBJECT wool_get_window_property();
    static WOOL_String gwm_execute_string;
    WOOL_OBJECT to_exec;
    int OldGWM_delete_read_properties = GWM_delete_read_properties;
    
    if(!gwm_execute_string)
	gwm_execute_string = WLString_make("GWM_EXECUTE");
    
    GWM_delete_read_properties = True;
    to_exec = wool_get_window_property(gwm_execute_string);
    GWM_delete_read_properties = OldGWM_delete_read_properties;
    wool_execute_wool_string(to_exec);
}

/*
 * interface to XGetDefault
 */

WOOL_OBJECT
wool_x_get_default(program, option)
WOOL_String program;
WOOL_String option;
{
    char           *result;

    must_be_string(program, 0);
    must_be_string(option, 1);
    result = XGetDefault(dpy, program -> string, option -> string);
    if (result)
	return (WOOL_OBJECT) WLString_make(result);
    else
	return NIL;
}

/*
 * Returns a color (pixel value) from an English name, or a #RGB description
 */

WOOL_OBJECT
wool_color_make(color)
WOOL_String color;
{
    XColor          x_color;

    must_be_string(color, 0);
    XSetErrorHandler(NoXError);
    if (!XParseColor(dpy, DefaultColormap(dpy, Context -> screen),
		     color -> string, &x_color)) {
	wool_printf("GWM: Color not found: %s\n", color -> string);
	x_color.pixel = Context -> pixel.Back;
    } else if (!XAllocColor(dpy, DefaultColormap(dpy, Context -> screen),
		     &x_color)) {
	wool_printf("GWM: Colormap full, couldn't allocate Color: %s\n",
		    color -> string);
	x_color.pixel = Context -> pixel.Back;
    }
    XSync(dpy, 0);
    XSetErrorHandler(XError);
    return (WOOL_OBJECT) WLNumber_make(x_color.pixel);
}

WOOL_OBJECT
wool_rgb_color_make(argc, argv)
int argc;
WOOL_Number argv[];
{
    XColor          x_color;

    if (argc != 3)
	wool_error(BAD_NUMBER_OF_ARGS, argc);
    must_be_number(argv[0], 0);
    must_be_number(argv[1], 1);
    must_be_number(argv[2], 2);

    x_color.red = argv[0] -> number;
    x_color.green = argv[1] -> number;
    x_color.blue = argv[2] -> number;

    XSetErrorHandler(NoXError);
    if (!XAllocColor(dpy, DefaultColormap(dpy, Context -> screen), &x_color)) {
	wool_printf("GWM: Color not found: #%04x%04x%04x\n",
		argv[0] -> number, argv[1] -> number, argv[2] -> number);
	x_color.pixel = Context -> pixel.Back;
    }
    XSync(dpy, 0);
    XSetErrorHandler(XError);
    return (WOOL_OBJECT) WLNumber_make(x_color.pixel);
}

WOOL_OBJECT
wool_color_free(argc, argv)
    int argc;
    WOOL_Number argv[];
{
    unsigned long *pixels = (unsigned long *) Malloc(argc * sizeof(int));
    int i;

    for (i = 0; i < argc; i++) {
	pixels[i] = argv[i] -> number;
    }
    XSetErrorHandler(NoXError);
    XFreeColors(dpy, DefaultColormap(dpy, Context -> screen),
		pixels, argc, 0);
    XSync(dpy, 0);
    XSetErrorHandler(XError);
    return NIL;
}

/* returns the RGB values of a color as a list of 3 integers between
 * 0 and 65535
 */

WOOL_OBJECT
wool_color_components(color)
WOOL_Number color;
{
    XColor          x_color;
    WOOL_List       result;

    must_be_number(color, 0);
    x_color.pixel = color->number;
    XQueryColor(dpy, DefaultColormap(dpy, Context -> screen), &x_color);
    result = wool_list_make(3);
    increase_reference(result->list[0] =
		       (WOOL_OBJECT) WLNumber_make(x_color.red));
    increase_reference(result->list[1] =
		       (WOOL_OBJECT) WLNumber_make(x_color.green));
    increase_reference(result->list[2] =
		       (WOOL_OBJECT) WLNumber_make(x_color.blue));
    return (WOOL_OBJECT) result;
}


/*
 * loading a font (prefixed by ^F in hash table)
 */

WOOL_OBJECT
wool_font_make(string)
WOOL_String     string;
{
    long            fontid;
    WOOL_Pointer    font_pointer;

    if (wool_self_pointer_make(string -> string, '\006', &font_pointer)) {
	fontid = *(font_pointer -> ptr);
    } else {
	if (TrapXErrors(fontid = (long) XLoadFont(dpy, string -> string))
	    || (!fontid)) {

/*
 *     /*This should have used the server's default but seems broken... 
 * 	    wool_printf("GWM: Cannot find Font %s , using server default\n",
 * 			string -> string);
 * 	    fontid = XQueryFont(dpy,
 * 			      DefaultGC(dpy, Context->screen) -> gid) -> fid;
 * 	}
 */

	    wool_printf("GWM: Cannot find Font %s , using \"font\" default\n",
			string -> string);
	    *(font_pointer -> ptr) = fontid = DefaultFont;
	} else {
	    *(font_pointer -> ptr) = fontid;
	}
    }
    return (WOOL_OBJECT) WLNumber_make(fontid);
}


/*
 * send an user event to all sub-wobs of the window
 */

WOOL_OBJECT
wool_send_user_event(argc, argv)
int argc;
WOOL_Number argv[];
{
    ClientWindow cw, sender;
    XAnyEvent evt;
    int old_GWM_Propagate_user_events = GWM_Propagate_user_events;

    sender = TargetWindow;
    if(argc >= 2){
	cw = (ClientWindow) argv[1] -> number;
    }else{
	cw = sender;
    }
    if (argc == 3 && ((WOOL_OBJECT) argv[2]) != NIL)
	GWM_Propagate_user_events = 0;
    if(!cw || argc == 0)
    return NIL;
    must_be_or_nil(WLAtom, argv[0], 0);
    evt.type = GWMUserEvent;			/* our event */
    evt.display = (Display *) argv[0]; 		/* the message */
    evt.window = (Window) sender;		/* the sender */
    {
	SAVE_EVENT_CONTEXT;
	WOOL_send(WOOL_process_event, cw, (cw, &evt)); 
	RESTORE_EVENT_CONTEXT;
    }
    GWM_Propagate_user_events = old_GWM_Propagate_user_events;
    return NIL;
}

/* get the value of GWM_ProcessingExistingWindows */

WOOL_OBJECT
GWM_ProcessingExistingWindows_get()
{
    if (GWM_ProcessingExistingWindows)
	return TRU;
    else
	return NIL;
}

/* returns the list of managed screens
 */

WOOL_OBJECT
wool_get_list_of_screens()
{
    WOOL_List       wl_list = wool_list_make(ScreenCount(dpy));
    int             i = 0;

    FOR_ALL_SCREENS{
	increase_reference(wl_list -> list[i++] =
			   (WOOL_OBJECT) WLNumber_make(Context -> screen));
    }END_OF_ALL_SCREENS;
    wl_list -> size = i;
    return (WOOL_OBJECT) wl_list;
}

/* returns the list of clientwindows or realized icons on screen,
 * mapped or not
 */

WOOL_OBJECT
wool_get_list_of_windows(argc, argv)
int	argc;
WOOL_Atom argv[];
{
    int             i;
    WOOL_List       wl_list;
    ClientWindow    cw;
    int             status = 0;		/* 'window or 'icon */
    int             mapped = 0;		/* 'mapped for only visible ones */
    int             stacking_order = 0;	/* do a XQueryTree to get the list */
    unsigned int    nb_windows = Context -> WindowCount;
    Window         *windows, dummywin, parent;

    for (i = 0; i < argc; i++)		/* options parsing */
	if (argv[i] == WA_window)
	    status = ClientWindowStatus;
	else if (argv[i] == WA_icon)
	    status = IconStatus;
	else if (argv[i] == WA_mapped)
	    mapped = 1;
	else if (argv[i] == WA_stacking_order)
	    stacking_order = 1;

    if (stacking_order) {
	XQueryTree(dpy, Context->root, &dummywin, &parent,
		   &windows, &nb_windows);
    }
    if (nb_windows) {
	wl_list = wool_list_make(nb_windows);
	if (stacking_order)
	    cw = init_managed_windows(windows, nb_windows);
	else
	    cw = Context->rootWob -> next;
	i = 0;
	while (cw) {
	    if (((!status) || cw -> status & status)
		&& ((!mapped) || cw -> mapped))
		increase_reference(wl_list -> list[i++] =
				   (WOOL_OBJECT) WLNumber_make(cw));
	    if (stacking_order)
		cw = get_next_managed_window();
	    else
		cw = cw -> next;
	}
	wl_list -> size = i;
	if (i) {
	    return (WOOL_OBJECT) wl_list;
	}
    }
    return NIL;
}

static Window *managed_window_list;
static int     managed_window_size;

ClientWindow
init_managed_windows(window_list, window_size)
Window *window_list;
int     window_size;
{
    ClientWindow    cw = 0;

    while (window_size > 0
	   && (!(cw = (ClientWindow) LookUpWob(*window_list))
	       || cw -> type != ClientWindowClass)) {
	window_list++;
	window_size--;
    }
    managed_window_list = window_list;
    managed_window_size = window_size;
    return (window_size ? cw : 0);
}

ClientWindow
get_next_managed_window()
{
    ClientWindow    cw = 0;

    do {
	managed_window_list++;
	managed_window_size--;
    } while (managed_window_size > 0 &&
	     (!(cw = (ClientWindow) LookUpWob(*managed_window_list))
	      || cw -> type != ClientWindowClass));
    return (managed_window_size ? cw : 0);
}

/*
 * get the type of the wob (status)
 */

WOOL_OBJECT
wool_wob_status(wob)
Wob	wob;
{
    if (wob -> type[0] == (WOB_METHOD) WA_window
	&& (wob -> status & IconStatus))
	return (WOOL_OBJECT) WA_icon;
    else
	return (WOOL_OBJECT) wob -> type[0];
}

WOOL_OBJECT
wool_get_window_status()
{
    return wool_wob_status(TargetWindow);
}

WOOL_OBJECT
wool_get_wob_status()
{
    return wool_wob_status(TargetWob);
}

WOOL_OBJECT
wool_get_wob_of_position(x, y)
WOOL_Number x,y;
{
    int             xr, yr;
    Window          window;
    Wob             wob;

    if (XTranslateCoordinates(dpy, Context->root, Context->root,
			      x -> number, y -> number, &xr, &yr, &window)) {
	if ((wob = (Wob) LookUpWob(window))
	    || (wob = (Wob) LookUpClient(window)))
	    return (WOOL_OBJECT) WLNumber_make(wob);
    }
    return NIL;
}

/*
 * get the associated window:
 * 	clientwindow => icon
 * 	icon => clientwindow
 * 	menu => parent
 * 	wob => parent
 */

WOOL_OBJECT
wool_get_wob_parent()
{
    if (TargetWob -> parent)
	return (WOOL_OBJECT) WLNumber_make(TargetWob -> parent);
    else
	return NIL;
}

WOOL_OBJECT
wool_get_icon()
{
    Wob             target = TargetWob;

    if (!(TargetWindow -> icon)) {
	RealizeIconWindow(TargetWindow);
	SetTarget(target);
    }
    return (WOOL_OBJECT) WLNumber_make(TargetWindow -> icon);
}

WOOL_OBJECT
wool_icon_exists(argc, argv)
int argc;
WOOL_Number argv[];
{
    ClientWindow    cw;

    if (argc) {
	must_be_number(argv[0], 0);
	cw = (ClientWindow) argv[0] -> number;
    } else {
	cw = TargetWindow;
    }
    if (cw->icon) {
	return TRU;
    } else {
	return NIL;
    }
}

WOOL_OBJECT
wool_get_window()
{
    return (WOOL_OBJECT) WLNumber_make(TargetWindow -> window);
}

/*
 * width & height of any object
 */

WOOL_OBJECT
wool_width(object)
WOOL_OBJECT	object;
{
    struct _Box     box;

    object = GetWobOfNumber(object);
    WOOL_send(WOOL_get_dimensions, object, (object, &box));
    return (WOOL_OBJECT) WLNumber_make(box.width);
}

WOOL_OBJECT
wool_height(object)
WOOL_OBJECT	object;
{
    struct _Box     box;

    object = GetWobOfNumber(object);
    WOOL_send(WOOL_get_dimensions, object, (object, &box));
    return (WOOL_OBJECT) WLNumber_make(box.height);
}

WOOL_OBJECT
wool_dimensions(object)
WOOL_OBJECT     object;
{
    struct _Box     box;
    WOOL_Number	    tab[4];

    object = GetWobOfNumber(object);
    WOOL_send(WOOL_get_dimensions, object, (object, &box));
    tab[0] = WLNumber_make(box.x);
    tab[1] = WLNumber_make(box.y);
    tab[2] = WLNumber_make(box.width);
    tab[3] = WLNumber_make(box.height);
    return (WOOL_OBJECT) wool_list_make_from_evaluated_array(4, tab);    
}

#ifdef DEBUG
/* debugging-only functions:
 */

char *
atom_name(atom)				/* get name (string) of X atom */
Atom	atom;
{
    if (atom)
	return XGetAtomName(dpy, atom);
    else
	return NULL;
}
#endif /* DEBUG */

/* handler called by wool before execing a son after a fork
 */

wool_clean_before_exec()
{
    close(XConnectionNumber(dpy));
}

/*
 * restarts GWM the hard way
 */

WOOL_OBJECT
GWM_restart(argc, argv)
int     argc;
WOOL_String argv[];
{
    extern int      GWM_argc;
    extern char   **GWM_argv;

    if (argc) {				/* pass args */
	char          **new_argv = (char **)
	Malloc(sizeof(char *) * (argc + 1));
	int             i;

	for (i = 0; i < argc; i++) {
	    must_be_string(argv[i], i);
	    new_argv[i] = argv[i] -> string;
	}
	new_argv[argc] = 0;
	GWM_is_restarting = new_argv;
    } else {				/* same args */
	GWM_is_restarting = GWM_argv;
    }
    XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
    XSync(dpy, True);
    FOR_ALL_SCREENS {
	if (Context -> rootWob) {
	    SetTarget(Context -> rootWob);
	    ScreenClose(Context -> rootWob);
	}
    }END_OF_ALL_SCREENS;
    XCloseDisplay(dpy);
    execvp(GWM_is_restarting[0], GWM_is_restarting);
    return wool_error("could not restart %s", GWM_is_restarting[0]);
}

/* this function allows a kind of multitasking in wool:
 * It recusrivly processes pending events in the queue and returns as soon as
 * the queue is empty
 * Can be used during long tedious tasks so as not to "freeze" gwm
 * with arg <> (), do a Sync before
 */

WOOL_OBJECT
wool_process_events(argc, argv)
int         argc;
WOOL_OBJECT argv[];
{
    SAVE_EVENT_CONTEXT;
    if (argc && argv[0] != NIL)
	XSync(dpy, 0);
    GWM_ProcessEvents(0);
    RESTORE_EVENT_CONTEXT;
    return NIL;
}

/* to make a function call of an atom */

wool_function_call_make(list, atom)
WOOL_List *list;
WOOL_Atom atom;
{
    *list = wool_list_make(1);
    increase_reference((*list) -> list[0] = (WOOL_OBJECT) atom);
    increase_reference(*list);
}

/* Here goes the initialisations to be done AFTER wool is brought up, but
 * BEFORE the user's profile is read
 * 
 * You will bind all your C functions to WOOL atoms here
 */

void
GWM_init_wool_before_profile()
{
    /* Types of Wool Objects, This the ONLY list of all objects 
     * see also declarations of wobs at the end of the function
     */

    WLActive[0] = (WOOL_METHOD) wool_atom("active");
    WLAtom[0] = (WOOL_METHOD) wool_atom("atom");
    WLBar[0] = (WOOL_METHOD) wool_atom("bar");
    WLClient[0] = (WOOL_METHOD) wool_atom("client");
    WLCollection[0] = (WOOL_METHOD) wool_atom("collection");
    WLQuotedExpr[0] = (WOOL_METHOD) wool_atom("quoted-expr");
    WLCursor[0] = (WOOL_METHOD) wool_atom("cursor");
    WLEvent[0] = (WOOL_METHOD) wool_atom("event");
    WLFsm[0] = (WOOL_METHOD) wool_atom("fsm");
    WLFsmState[0] = (WOOL_METHOD) wool_atom("fsm-state");
    WLStateArc[0] = (WOOL_METHOD) wool_atom("state-arc");
    WLSubr[0] = (WOOL_METHOD) wool_atom("subr");
    WLFSubr[0] = (WOOL_METHOD) wool_atom("fsubr");
    WLExpr[0] = (WOOL_METHOD) wool_atom("expr");
    WLFExpr[0] = (WOOL_METHOD) wool_atom("fexpr");
    WLLabel[0] = (WOOL_METHOD) wool_atom("label");
    WLList[0] = (WOOL_METHOD) wool_atom("list");
    WLMenu[0] = (WOOL_METHOD) wool_atom("menu");
    WLName[0] = (WOOL_METHOD) wool_atom("name");
    WLNamespace[0] = (WOOL_METHOD) wool_atom("namespace");
    WLNumber[0] = (WOOL_METHOD) wool_atom("number");
    WLPixmap[0] = (WOOL_METHOD) wool_atom("pixmap");
    WLPlug[0] = (WOOL_METHOD) wool_atom("plug");
    WLPointer[0] = (WOOL_METHOD) wool_atom("pointer");
    WLString[0] = (WOOL_METHOD) wool_atom("string");

    					/*  wool functions: declare them with
					   type, C name, WOOL name, number of
					   arguments (0,1,2,NARY) */

    wool_subr_make(WLSubr, wool_client_make, "window-make", NARY);
    wool_subr_make(WLSubr, wool_plug_make, "plug-make", 1);
    wool_subr_make(WLSubr, wool_bar_make, "bar-make", NARY);
    wool_subr_make(WLSubr, wool_fsm_make, "fsm-make", NARY);
    wool_subr_make(WLSubr, wool_fsm_state_make, "state-make", NARY);
    wool_subr_make(WLFSubr, wool_state_arc_make, "on", NARY);
    wool_subr_make(WLSubr, wool_state_arc_make, "on-eval", NARY);
    increase_reference(wool_atom("pixmap-make") -> c_val =
     wool_subr_make(WLSubr, wool_layered_bitmap_make, "layered-bitmap-make",
		    NARY));
    wool_subr_make(WLSubr, wool_label_make, "active-label-make", NARY);
    wool_subr_make(WLSubr, wool_cursor_make, "cursor-make", NARY);
    wool_subr_make(WLSubr, wool_menu_make, "menu-make", NARY);
    wool_subr_make(WLSubr, wool_font_make, "font-make", 1);
    wool_subr_make(WLSubr, wool_color_make, "color-make", 1);
    wool_subr_make(WLSubr, wool_rgb_color_make, "color-make-rgb", NARY);
    wool_subr_make(WLSubr, wool_color_free, "color-free", NARY);
    wool_subr_make(WLSubr, wool_color_components, "color-components", 1);
    wool_subr_make(WLSubr, wool_stamp_make, "label-make", NARY);
    wool_subr_make(WLSubr, wool_string_to_keysym, "key-make", 1);
    wool_subr_make(WLSubr, wool_set_acceleration, "set-acceleration", 2);
    wool_subr_make(WLSubr, wool_set_threshold, "set-threshold", 1);
    wool_subr_make(WLSubr,
		   wool_change_keyboard_mapping, "set-key-binding", NARY);
    wool_subr_make(WLSubr, wool_refresh, "refresh", NARY);
    wool_subr_make(WLSubr, wool_set_screen_saver, "set-screen-saver", NARY);
    wool_subr_make(WLSubr, PlaceFixedMenu, "place-menu", NARY);
    wool_subr_make(WLSubr, wool_meter_open, "meter-open", NARY);
    wool_subr_make(WLSubr, wool_meter_update, "meter-update", 1);
    wool_subr_make(WLSubr, MeterClose, "meter-close", 0);
    wool_subr_make(WLSubr, WLRSC_put, "resource-put", 2);
    wool_subr_make(WLSubr, WLRSC_get, "resource-get", 2);
    wool_subr_make(WLSubr, wool_x_get_default, "get-x-default", 2);
    wool_subr_make(WLFSubr, CirculateWindowsDown,
		   "circulate-windows-down", 0);
    wool_subr_make(WLFSubr, CirculateWindowsUp, "circulate-windows-up", 0);
    wool_subr_make(WLSubr, ReDecorateWindow, "re-decorate-window", NARY);
    wool_subr_make(WLSubr, KillWindow, "kill-window", NARY);
    wool_subr_make(WLSubr, MoveWindow, "move-window", NARY);
    wool_subr_make(WLSubr, ResizeWindow, "resize-window", NARY);
    wool_subr_make(WLSubr, RaiseWindow, "raise-window", NARY);
    wool_subr_make(WLSubr, LowerWindow, "lower-window", NARY);
    wool_subr_make(WLSubr, IconifyWindow, "iconify-window", 0);
    wool_subr_make(WLSubr, wool_set_focus, "set-focus", NARY);
    wool_subr_make(WLSubr, wool_send_user_event, "send-user-event", NARY);
    wool_subr_make(WLSubr, wool_get_triggering_event_state,
		   "current-event-modifier", 0);
    wool_subr_make(WLSubr, wool_get_triggering_event_code,
		   "current-event-code", 0);
    wool_subr_make(WLSubr, wool_get_triggering_event_x,
		   "current-event-x", 0);
    wool_subr_make(WLSubr, wool_get_triggering_event_y,
		   "current-event-y", 0);
    wool_subr_make(WLSubr, wool_get_triggering_event_time,
		   "current-event-time", 0);
    wool_subr_make(WLSubr, PopMenu, "pop-menu", NARY);
    wool_subr_make(WLSubr, UnpopMenu, "unpop-menu", NARY);
    wool_subr_make(WLSubr, wool_set_grab, "grab-server", NARY);
    wool_subr_make(WLSubr, wool_remove_grab, "ungrab-server", NARY);
    wool_subr_make(WLSubr, MapWindow, "map-window", NARY);
    wool_subr_make(WLSubr, UnmapWindow, "unmap-window", NARY);
    wool_subr_make(WLFSubr, wool_last_key_struck, "last-key", 0);
    wool_subr_make(WLSubr, wool_get_list_of_windows, "list-of-windows", NARY);
    wool_subr_make(WLSubr, wool_get_list_of_screens, "list-of-screens", NARY);
    wool_subr_make(WLFSubr, wool_logical_coords,
		   "current-event-window-coords", 0);
    wool_subr_make(WLSubr, wool_send_key_to_client, "send-key-to-window", 2);
    wool_subr_make(WLSubr, wool_send_keycode_to_client,
		   "send-keycode-to-window", 2);
    wool_subr_make(WLSubr, wool_send_button_to_client,
		   "send-button-to-window", NARY);
    wool_subr_make(WLFSubr, wool_process_exposes, "process-exposes", 0);
    wool_subr_make(WLFSubr, wool_current_mouse_position,
		   "current-mouse-position", 0);
    wool_subr_make(WLFSubr, current_wob_invert_aera, "wob-invert", 0);
    wool_subr_make(WLSubr, wool_get_window_property, "get-x-property", 1);
    wool_subr_make(WLSubr, wool_set_window_property, "set-x-property", 2);
    wool_subr_make(WLSubr, wool_get_wob_of_position, "wob-at-coords", 2);
    wool_subr_make(WLFSubr, wool_last_user_event, "current-user-event", 0);
    wool_subr_make(WLSubr, wool_resend_event, "send-current-event", 1);
    wool_subr_make(WLSubr, wool_xpm_pixmap_make, "paxmap-make", NARY);
    wool_subr_make(WLSubr, wool_xpm_pixmap_make, "pixmap-load", NARY);
    wool_subr_make(WLSubr, wool_rotate_cut_buffers,
    		   "rotate-cut-buffers", NARY);
    wool_subr_make(WLSubr, wool_draw_line, "draw-line", NARY);
    wool_subr_make(WLSubr, wool_draw_rectangle, "draw-rectangle", NARY);
    wool_subr_make(WLSubr, wool_draw_text, "draw-text", NARY);
    wool_subr_make(WLSubr, wool_keyboard_bell_ring, "bell", NARY);
    wool_subr_make(WLSubr, wool_wm_state_update, "window-wm-state-update", 0);
    wool_subr_make(WLSubr, wool_set_colormap_focus,
    		   "set-colormap-focus", NARY);
    wool_subr_make(WLSubr, wool_set_wm_icon_sizes, "set-icon-sizes", NARY);
    wool_subr_make(WLSubr, wool_set_subwindow_colormap_focus,
    		   "set-subwindow-colormap-focus", NARY);
    wool_subr_make(WLSubr, wool_set_grabs, "set-grabs", NARY);
    wool_subr_make(WLSubr, wool_unset_grabs, "unset-grabs", NARY);
    wool_subr_make(WLSubr, wool_width, "width", 1);
    wool_subr_make(WLSubr, wool_height, "height", 1);
    wool_subr_make(WLSubr, wool_dimensions, "dimensions", 1);
    wool_subr_make(WLSubr, GWM_restart, "restart", NARY);
    wool_subr_make(WLSubr, wool_process_events, "process-events", NARY);
    wool_subr_make(WLSubr, wool_delete_window, "delete-window", NARY);
    wool_subr_make(WLSubr, wool_save_yourself, "save-yourself", NARY);
    wool_subr_make(WLSubr, wool_keycode_to_keysym, "keycode-to-keysym", 2);
    wool_subr_make(WLSubr, wool_keysym_to_keycode, "keysym-to-keycode", 1);
    wool_subr_make(WLSubr, wool_list_make_from_wool, "list-make", NARY);
    wool_subr_make(WLSubr, wool_meter_modify, "meter", NARY);
    wool_subr_make(WLSubr, wool_warp_pointer, "warp-pointer", NARY);
    wool_subr_make(WLSubr, WLMenu_wob, "menu-wob", 1);
    wool_subr_make(WLSubr, wool_wm_state_get, "window-wm-state", NARY);
    wool_subr_make(WLSubr, wool_get_triggering_event_x_relative,
    	"current-event-relative-x", 0);
    wool_subr_make(WLSubr, wool_get_triggering_event_y_relative,
    	"current-event-relative-y", 0);
    wool_subr_make(WLSubr, wool_ungrab_and_replay_event,
	"ungrab-server-and-replay-event", 1);
    wool_subr_make(WLSubr, wool_makes_replayable, "replayable-event", 1);
    wool_subr_make(WLSubr, wool_event_was_due_to_a_grab,
    	"current-event-from-grab", 0);
    wool_subr_make(WLSubr, wool_get_wm_command, "get-wm-command", 0);
    wool_subr_make(WLSubr, MakeResourceIdentifier, "make-string-usable-for-resource-key", 1);
    wool_subr_make(WLSubr, wool_allow_events, "allow-event-processing", 0);
    wool_subr_make(WLSubr, LookUpWobW, "xid-to-wob", 1);
    wool_subr_make(WLSubr, Window2ClientW, "window-to-client", 1);
    wool_subr_make(WLSubr, Client2WindowW, "client-to-window", 1);
    wool_subr_make(WLSubr, wool_window_is_valid, "window-is-valid", 1);
    wool_subr_make(WLSubr, wool_wob_is_valid, "wob-is-valid", 1);
    wool_subr_make(WLSubr, wool_icon_exists, "window-icon?", NARY);

					/* --- */
#ifdef SHAPE
    wool_subr_make(WLSubr, wool_window_has_shaped_client,
    	"window-is-shaped", 0);
#else /* SHAPE */
    wool_subr_make(WLSubr, NIL_FUNC, "window-is-shaped", 0);
#endif /* SHAPE */

 					/* pointers to ints: declare them
					   with WOOL name, pointer to memory
					   location */

    wool_pointer_make("borderwidth", &DefaultBorderWidth);
    wool_pointer_make("inner-borderwidth", &DefaultInnerBorderwidth);
    wool_pointer_make("move-grid-style", &GWM_Movegrid);
    wool_pointer_make("resize-grid-style", &GWM_Resizegrid);
    wool_pointer_make("resize-style", &GWM_resize_style);
    wool_pointer_make("mwm-resize-style-corner-size", &GWM_window_cutting_size);
    wool_pointer_make("mwm-resize-style-catch-corners", &GWM_Mwm_catch_corners);
    wool_pointer_make("move-meter", &GWM_Move_meter);
    wool_pointer_make("resize-meter", &GWM_Resize_meter);
    wool_pointer_make("label-horizontal-margin",
		      &DefaultLabelHorizontalMargin);
    wool_pointer_make("label-vertical-margin",
		      &DefaultLabelVerticalMargin);
    wool_pointer_make("plug-separator", &DefaultPlugSeparator);
    wool_pointer_make("bar-separator", &DefaultBarSeparator);
    wool_pointer_make("invert-cursors", &GWM_InvertCursors);
    WA_font = (WOOL_Atom) wool_pointer_make("font", &DefaultFont);
    wool_pointer_make("freeze-server", &GWM_GrabServer);
    wool_pointer_make("double-click-delay", &GWM_DoubleClickDelay);
    wool_pointer_make("direction", &DefaultMenuDirection);
    wool_pointer_make("bar-max-width", &DefaultBarMaxWidth);
    wool_pointer_make("bar-min-width", &DefaultBarMinWidth);
    wool_pointer_make("menu-max-width", &DefaultMenuMaxWidth);
    wool_pointer_make("menu-min-width", &DefaultMenuMinWidth);
    wool_pointer_make("check-input-focus-flag", &GWM_check_input_focus_flag);
    wool_pointer_make("print-errors-flag", &wool_do_print_errors);
    wool_pointer_make("screen-count", &GWM_ScreenCount);
    wool_pointer_make("grab-keyboard-also", &GWM_grab_keyboard_also);
    wool_pointer_make("confine-grabs", &GWM_confine_grabs);
    wool_pointer_make("confine-windows", &GWM_confine_windows);
    wool_pointer_make("gwm-quiet", &GWM_quiet);
    wool_pointer_make("reenter-on-opening", &GWM_reenter_on_opens);
    wool_pointer_make("delete-read-properties", &GWM_delete_read_properties);

					/* pointers to Context fields */

#define pointer_to_Context(name, field) \
    (WOOL_Atom) wool_base_pointer_make(name, &Context, &(Context -> field))

    WA_background = pointer_to_Context("background", pixel.Back);
    WA_foreground = pointer_to_Context("foreground", pixel.Fore);
    WA_borderpixel = pointer_to_Context("borderpixel", pixel.Border);
    pointer_to_Context("grid-color", pixel.GridColor);
    pointer_to_Context("invert-color", pixel.InvertColor);
    pointer_to_Context("screen-width", width);
    pointer_to_Context("screen-height", height);
    pointer_to_Context("screen-depth", depth);
    pointer_to_Context("screen-widthMM", widthMM);
    pointer_to_Context("screen-heightMM", heightMM);

 					/* context atoms initialisation. Give
					   type, C name, WOOL name, initial
					   value */

    wool_atom_make(WOOL_Atom, WA_bordertile, "bordertile", NIL);
    wool_atom_make(WOOL_Atom, WA_property, "property", NIL);
    wool_atom_make(WOOL_Atom, WA_fsm, "fsm", NIL);
    wool_atom_make(WOOL_Atom, WA_menu, "menu", NIL);
    wool_atom_make(WOOL_Atom, WA_tile, "tile", NIL);
    wool_atom_make(WOOL_Atom, WA_cursor, "cursor", NIL);
    wool_atom_make(WOOL_Atom, WA_property, "property", NIL);
    wool_atom_make(WOOL_Atom, WA_describe_window, "describe-window", NIL);
    wool_atom_make(WOOL_Atom, WA_describe_screen, "describe-screen", NIL);
    wool_atom_make(WOOL_Atom, WA_window_description, "window-description",
		   NIL);
    wool_atom_make(WOOL_Atom, WA_icon_description, "icon-description", NIL);
    wool_atom_make(WOOL_Atom, WA_grabs, "grabs", NIL);
    wool_atom_make(WOOL_Atom, WA_opening, "opening", NIL);
    wool_atom_make(WOOL_Atom, WA_closing, "closing", NIL);
    wool_atom_make(WOOL_Atom, WA_horizontal_margin, "horizontal-margin", NIL);
    wool_atom_make(WOOL_Atom, WA_vertical_margin, "vertical-margin", NIL);
    wool_atom_make(WOOL_Atom, WA_cursorNW, "cursor-NW", NIL);
    wool_atom_make(WOOL_Atom, WA_cursorNE, "cursor-NE", NIL);
    wool_atom_make(WOOL_Atom, WA_cursorN, "cursor-N", NIL);
    wool_atom_make(WOOL_Atom, WA_cursorSW, "cursor-SW", NIL);
    wool_atom_make(WOOL_Atom, WA_cursorSE, "cursor-SE", NIL);
    wool_atom_make(WOOL_Atom, WA_cursorS, "cursor-S", NIL);
    wool_atom_make(WOOL_Atom, WA_cursorE, "cursor-E", NIL);
    wool_atom_make(WOOL_Atom, WA_cursorW, "cursor-W", NIL);
    wool_atom_make(WOOL_Atom, WA_cursorW, "cursor-W", NIL);
    wool_atom_make(WOOL_Atom, WA_class_name, "class-name",
		   WLString_make("Gwm"));
    wool_atom_make(WOOL_Atom, WA_client_name, "client-name", 
		   WLString_make("menu"));
    wool_atom_make(WOOL_Atom, WA_icon_name, "icon-name", NIL);
    wool_atom_make(WOOL_Atom, WA_starts_iconic, "starts-iconic", NIL);

    wool_atom_with_numeric_value_make("horizontal", HORIZONTAL);
    wool_atom_with_numeric_value_make("vertical", VERTICAL);

    WA_iconify_window = wool_atom("iconify-window");
    WA_here = wool_atom("here");
    WA_bar = wool_atom("bar");
    WA_plug = wool_atom("plug");
    WA_root = wool_atom("root");
    WA_icon = wool_atom("icon");
    WA_mapped = wool_atom("mapped");
    WA_stacking_order = wool_atom("stacking-order");
    WA_mono = wool_atom("mono");
    WA_gray = wool_atom("gray");
    WA_color = wool_atom("color");
    WA_grab = wool_atom("grab");
    WA_ungrab = wool_atom("ungrab");
    WA_x = wool_atom("x");
    WA_y = wool_atom("y");
    WA_gravity = wool_atom("gravity");
    WA_borderwidth = wool_atom("borderwidth");

 					/* functional calls are lists of one
					   atom */

    wool_function_call_make(&WL_describe_window_call, WA_describe_window);
    wool_function_call_make(&WL_describe_screen_call, WA_describe_screen);
    wool_function_call_make(&WL_iconify_window_call, WA_iconify_window);

 					/* active values. Give WOOL name, get
					   C function, set C function (might
					   be NULL) */

    WA_window = (WOOL_Atom)
	wool_active_make("window",
			 wool_current_window, wool_set_current_window);
    wool_active_make("window-name", wool_current_window_name,
		     wool_current_window_name_set);
    wool_active_make("window-client-name",
		     wool_current_window_client_name, NULL);
    wool_active_make("window-client-class",
		     wool_current_window_client_class, NULL);
    wool_active_make("window-machine-name",
		     wool_current_window_machine_name, NULL);
    wool_active_make("window-icon-name",
		     wool_current_window_icon_name, NULL);
    wool_active_make("window-is-mapped",
		     wool_current_window_is_mapped, NULL);
    wool_active_make("window-x", wool_current_window_x, NULL);
    wool_active_make("window-y", wool_current_window_y, NULL);
    wool_active_make("wob-x", wool_wob_x_get, NULL);
    wool_active_make("wob-y", wool_wob_y_get, NULL);
    wool_active_make("window-width", wool_current_window_width, NULL);
    wool_active_make("window-height", wool_current_window_height, NULL);
    wool_active_make("window-client-x", wool_current_client_x, NULL);
    wool_active_make("window-client-y", wool_current_client_y, NULL);
    wool_active_make("window-client-width", wool_current_client_width, NULL);
    wool_active_make("window-client-height", wool_current_client_height,
		     NULL);
    wool_active_make("window-icon-window", wool_current_window_icon_window,
		     NULL);
    wool_active_make("window-icon-pixmap", wool_current_window_icon_bitmap,
		     NULL);
    wool_active_make("window-icon-pixmap-id",
		     wool_current_window_icon_bitmap_id, NULL);
    wool_active_make("window-user-set-position",
		     wool_current_window_US_position, NULL);
    wool_active_make("window-user-set-size", wool_current_window_US_size,
		     NULL);
    wool_active_make("window-program-set-position",
		     wool_current_window_P_position, NULL);
    wool_active_make("window-program-set-size", wool_current_window_P_size,
		     NULL);
    wool_active_make("window-is-transient-for",
		     wool_current_window_is_transient_for, NULL);
    wool_active_make("wob", wool_current_wob, wool_current_wob_set);
    wool_active_make("wob-width", wool_current_wob_width, NULL);
    wool_active_make("wob-height", wool_current_wob_height, NULL);
    wool_active_make("wob-property", Wob_get_property, Wob_set_property);
    wool_active_make("wob-menu", Wob_get_menu, Wob_set_menu);
    wool_active_make("wob-fsm", Wob_get_fsm, Wob_set_fsm);
    wool_active_make("window-property", WLClient_get_property,
		     WLClient_set_property);
    wool_active_make("wob-status", wool_get_wob_status, NULL);
    wool_active_make("window-status", wool_get_window_status, NULL);
    wool_active_make("wob-parent", wool_get_wob_parent, NULL);
    wool_active_make("window-size", wool_window_logical_size_get,
		     wool_window_logical_size_set);
    wool_active_make("wob-tile", current_wob_get_bitmap,
		     current_wob_set_bitmap);
    wool_active_make("wob-pixmap", current_wob_get_bitmap,
		     current_wob_set_bitmap);
    wool_active_make("wob-borderpixel", current_wob_get_borderpixel,
		     current_wob_set_borderpixel);
    wool_active_make("wob-borderwidth", current_wob_get_borderwidth,
		     current_wob_set_borderwidth);
    wool_active_make("wob-background", current_wob_get_background,
		     current_wob_set_background);
    wool_active_make("window-was-on-screen",
		     GWM_ProcessingExistingWindows_get, NULL);
    wool_active_make("window-icon", wool_get_icon, NULL);
    wool_active_make("window-window", wool_get_window, NULL);
    wool_active_make("window-starts-iconic",
		     wool_window_map_as_icon_get,
		     wool_window_map_as_icon_set);
    wool_active_make("window-group", wool_window_group_get,
		     wool_window_group_set);
    wool_active_make("cut-buffer", wool_cut_buffer_0_get,
		     wool_cut_buffer_0_set);
    wool_active_make("wob-cursor", wool_wob_cursor_get, wool_wob_cursor_set);
    wool_active_make("window-wm-state-icon", wool_wm_state_user_icon_get,
		     wool_wm_state_user_icon_set);
    wool_active_make("screen", wool_current_screen_get,
    		     wool_current_screen_set);
    wool_active_make("screen-type", wool_get_visual_type, NULL);
    wool_active_make("root-window", wool_get_root_window,
    		     wool_set_root_window);

    /* Xevents */
    wool_all_events_make();

    /* Strings */
    increase_reference(DefaultClientClass = WLString_make("client"));
    increase_reference(DefaultClientName = WLString_make("name"));
    increase_reference(DefaultWindowName = WLString_make("window"));
    increase_reference(DefaultMachineName = WLString_make("machine"));
    increase_reference(DefaultIconName = WLString_make("icon"));
    increase_reference(wool_atom("display-name") -> c_val =
		       (WOOL_OBJECT) WLString_make(GWM_Display));

    /* fsms */
    NIL_FSM_make();

    /* type names of the wobs for consistency purposes */

    ClientWindowClass[0] = (WOB_METHOD) WA_window;
    BarClass[0] = (WOB_METHOD) WA_bar;
    MenuClass[0] = (WOB_METHOD) WA_menu;
    PlugClass[0] = (WOB_METHOD) WA_plug;
    ScreenClass[0] = (WOB_METHOD) WA_root;

    /* screen namespace */
    ContextNamespace_init();
    SetDefaultScreen();
}

/*
 * Keep in mind that SetUpdefault is called BEFORE wool is initialized
 * InitWool in wool_C.c initialize after wool...
 * argc, argv points AFTER the host:display arg
 */

int
SetUpDefaults()
{
    /* initialize wool_host_name */
    wool_hostname_get();

    /* fonts */
    if (TrapXErrors(DefaultFont = (long) XLoadFont(dpy, DEFAULT_FONT))
	|| (!DefaultFont)) {
	wool_printf("GWM: Cannot find default font %s, aborting\n",
		    DEFAULT_FONT);
	GWM_end(1);
    }

    /* set up per-screen defaults */
    FOR_ALL_SCREENS {
	Context -> pixel.Fore = BlackPixel(dpy, Context -> screen);
	Context -> pixel.Back = WhitePixel(dpy, Context -> screen);
	Context -> pixel.GridColor =
	    (((unsigned long) 1) << Context -> depth) - 1;
	Context -> pixel.InvertColor = Context -> pixel.GridColor;
	Context -> pixel.Border = Context -> pixel.Fore;
	MeterCreate();
    }END_OF_ALL_SCREENS;

    /* X Atoms */
    XA_WM_STATE = XInternAtom(dpy, "WM_STATE", False);
    XA_WM_COLORMAP_WINDOWS = XInternAtom(dpy, "WM_COLORMAP_WINDOWS", False);
    XA_WM_CHANGE_STATE = XInternAtom(dpy, "WM_CHANGE_STATE", False);
    XA_WM_PROTOCOLS = XInternAtom(dpy, "WM_PROTOCOLS", False);
    XA_WM_TAKE_FOCUS = XInternAtom(dpy, "WM_TAKE_FOCUS", False);
    XA_WM_SAVE_YOURSELF = XInternAtom(dpy, "WM_SAVE_YOURSELF", False);
    XA_WM_DELETE_WINDOW = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    XA_GWM_EXECUTE = XInternAtom(dpy, "GWM_EXECUTE", False);
    XA_GWM_RUNNING = XInternAtom(dpy, "GWM_RUNNING", False);
}

/* initialize the switching name space of the screen */

WOOL_Namespace ContextNamespace;

void
ContextNamespace_trigger(i)
int i;
{
    wool_current_screen_set(WLNumber_make(i));
}

static Wob ContextNamespace_oldwob;

void
ContextNamespace_save_state()
{
    ContextNamespace_oldwob = TargetWob;
}

void
ContextNamespace_restore_state()
{
    SetTarget(ContextNamespace_oldwob);
}

ContextNamespace_init()
{
    int i;

    ContextNamespace = WLNamespace_make();
    ContextNamespace -> trigger = ContextNamespace_trigger;
    ContextNamespace -> save_state = ContextNamespace_save_state;
    ContextNamespace -> restore_state = ContextNamespace_restore_state;
    for (i=0; i < ScreenCount(dpy); i++)
	WLNamespace_add(ContextNamespace);
    wool_atom_make(WOOL_Atom, WA_ContextNamespace, "screen.",
		   ContextNamespace);
}
