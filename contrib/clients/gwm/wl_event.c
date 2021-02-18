/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/***********************\
* 		        *
*  WOOL_OBJECT: Event  *
*  BODY		        *
* 		        *
\***********************/

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_atom.h"
#include "wl_pointer.h"
#include "wl_string.h"
#include "wl_number.h"
#include "wl_list.h"
#include "wl_func.h"
#include "gwm.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "INTERN.h"
#include "wl_event.h"
#include "EXTERN.h"
#include "wl_cursor.h"
#include "wl_fsm.h"


extern WOB_METHOD       MenuClass[], PlugClass[];

/* table of events getting redirected to grabbing wob during a GWM grab
 */

Card32 EventProperties[] = {
/* zero */             0,
/* one */              0,
/* KeyPress */         EPGrabRedirected|EPKeyOrButton|EPXY|EPXYRoot|EPTime,
/* KeyRelease */       EPGrabRedirected|EPKeyOrButton|EPXY|EPXYRoot|EPTime,
/* ButtonPress */      EPGrabRedirected|EPKeyOrButton|EPXY|EPXYRoot|EPTime,
/* ButtonRelease */    EPGrabRedirected|EPKeyOrButton|EPXY|EPXYRoot|EPTime,
/* MotionNotify */     EPGrabRedirected|EPKeyOrButton|EPXY|EPXYRoot|EPTime,
/* EnterNotify */      EPRedirectable|EPCrossing|EPXY|EPXYRoot|EPTime,
/* LeaveNotify */      EPRedirectable|EPCrossing|EPXY|EPXYRoot|EPTime,
/* FocusIn */          EPRedirectable|EPFocus|EPXY|EPXYRoot|EPTime,
/* FocusOut */         EPRedirectable|EPFocus|EPXY|EPXYRoot|EPTime,
/* KeymapNotify */     0,
/* Expose */           0,
/* GraphicsExpose */   0,
/* NoExpose */         0,
/* VisibilityNotify */ 0,
/* CreateNotify */     0,
/* DestroyNotify */    0,
/* UnmapNotify */      0,
/* MapNotify */        0,
/* MapRequest */       0,
/* ReparentNotify */   0,
/* ConfigureNotify */  0,
/* ConfigureRequest */ 0,
/* GravityNotify */    0,
/* ResizeRequest */    0,
/* CirculateNotify */  0,
/* CirculateRequest */ 0,
/* PropertyNotify */   0,
/* SelectionClear */   0,
/* SelectionRequest */ 0,
/* SelectionNotify */  0,
/* ColormapNotify */   0,
/* ClientMessage */    0,
/* MappingNotify */    0,
/* LASTEvent */        0,
/* GWMUserEvent */     0
};

/* to filter button releases events from the fact that they get the same button
 * pressed as a modifier...
 */
unsigned int ButtonMasks[] = {
    ~0,
    ~Button1Mask,
    ~Button2Mask,
    ~Button3Mask,
    ~Button4Mask,
    ~Button5Mask
    };

/*
 * to built a Event from C
 */

WOOL_Event
wool_event_make(name, x_type, match, release, mask, state, detail)
char *		name;
int		x_type;
int		(*match)();
int		(*release)();
unsigned int	mask, state, detail;
{
    WOOL_Event     object = (WOOL_Event) WLEvent_make(state, detail);

    object -> x_type = x_type;
    object -> match = match;
    object -> release = release;
    object -> mask = mask;
    return (WOOL_Event) WLAtom_set(wool_atom(name), object);
}

WOOL_Event
WLEvent_make(state, detail)
unsigned int	state, detail;
{
    WOOL_Event     object = (WOOL_Event) Malloc(
					       sizeof(struct _WOOL_Event));

    zrt_put(object);
    object -> type = WLEvent;
    object -> state = state;
    object -> detail = detail;
    object -> user_mask = 0;
    object -> flags = 0;
    return object;
}

WOOL_OBJECT
WLEvent_print(obj)
WOOL_Event obj;
{
    wool_printf("{EVENT %s", eventtype[obj->x_type]);
    wool_printf(" d=%d", obj->detail);
    wool_printf(" s=%d", obj->state);
    /*print_atom_pointing_to(obj); */
    wool_puts("}");
    return (WOOL_OBJECT) obj;
}

/*
 * List of matching procedures for each event
 */

int 
WLEvent_user(wl_event, evt)
WOOL_Event	wl_event;
Event	evt;
{
    if ((wl_event -> detail == (int) evt -> xany.display)
	|| (wl_event -> detail == (unsigned int) ANY))
	return TRUE;
    else
	return FALSE;
}

int 
WLEvent_propertychange(wl_event, evt)
WOOL_Event	wl_event;
Event	evt;
{
    if (evt -> xproperty.atom == (Atom) wl_event -> detail
	|| (wl_event -> detail == (unsigned int) ANY)) {
	return TRUE;
    } else
	return FALSE;
}

int 
WLEvent_property_icon_pixmap_change(wl_event, evt)
WOOL_Event	wl_event;
Event	evt;
{
    if ((evt -> xproperty.atom == (Atom) XA_WM_HINTS) 
	&&
	(SetTarget(TargetWob),
	 TargetWindow -> cached_props -> wm_hints.flags & IconPixmapHint)
	&&
	(!(GWM_backup_of_old_property.wm_hints.flags & IconPixmapHint)
	 ||
	 TargetWindow -> cached_props -> wm_hints.icon_pixmap !=
	 GWM_backup_of_old_property.wm_hints.icon_pixmap))
	return TRUE;
    else
	return FALSE;
}

int 
WLEvent_configurenotify(wl_event, evt)
WOOL_Event	wl_event;
Event	evt;
{
    return TRUE;
}

int 
WLEvent_buttonpress(wl_event, evt)
WOOL_Event	wl_event;
Event	evt;
{
    if (((wl_event -> detail == (unsigned int) ANY) ||
	 (wl_event -> detail == evt -> xbutton.button))
	&&
	((wl_event -> state == (unsigned int) ANY) ||
	 (wl_event -> state == evt -> xbutton.state)))
	return TRUE;
    else
	return FALSE;
}

int 
WLEvent_double_buttonpress(wl_event, evt)
WOOL_Event	wl_event;
Event	evt;
{
    if (((wl_event -> detail == (unsigned int) ANY) ||
	 (wl_event -> detail == evt -> xbutton.button))
	&&
	((wl_event -> state == (unsigned int) ANY) ||
	 (wl_event -> state == evt -> xbutton.state))
	&&
	evt -> xbutton.button == GWM_LastEvent.xbutton.button
	&&
	evt -> xbutton.window == GWM_LastEvent.xbutton.window
	&&
	evt -> xbutton.time <
	(GWM_LastEvent.xbutton.time + GWM_DoubleClickDelay))
	return TRUE;
    else
	return FALSE;
}

int 
WLEvent_buttonrelease(wl_event, evt)
WOOL_Event	wl_event;
Event	evt;
{
    unsigned int butmask = ButtonMasks[evt -> xbutton.button];

    if (((wl_event -> detail == (unsigned int) ANY) ||
	 (wl_event -> detail == evt -> xbutton.button))
	&&
	((wl_event -> state == (unsigned int) ANY) ||
	 ((wl_event -> state & butmask) == (evt -> xbutton.state & butmask)
	  )))
	return TRUE;
    else
	return FALSE;
}

int 
WLEvent_keypress(wl_event, evt)
WOOL_Event	wl_event;
Event	evt;
{
    if (((wl_event -> detail == (unsigned int) ANY) ||
	 (wl_event -> detail == XKeycodeToKeysym(dpy,
				    evt -> xkey.keycode, 0)))
	&&
	((wl_event -> state == (unsigned int) ANY) ||
	 (wl_event -> state == evt -> xkey.state))) {
	return TRUE;
    } else
	return FALSE;
}

int 
WLEvent_keyrelease(wl_event, evt)
WOOL_Event	wl_event;
Event	evt;
{
    if (((wl_event -> detail == (unsigned int) ANY) ||
	 (wl_event -> detail == XKeycodeToKeysym(dpy,
				    evt -> xkey.keycode, 0)))
	&&
	((wl_event -> state == (unsigned int) ANY) ||
	 (wl_event -> state == evt -> xkey.state))) {
	return TRUE;
    } else
	return FALSE;
}

/*
 * enter & leave in the subtree of the window
 */

int
WLEvent_enterwindow(wl_event, evt)
WOOL_Event	wl_event;
Event	evt;
{
    XEvent          report;

    if (evt -> xcrossing.detail != NotifyInferior) {
	/* XSync(dpy, 0); */
	if (!XCheckTypedWindowEvent(dpy, evt -> xcrossing.window,
				    LeaveNotify, &report)
	    || report.xcrossing.detail == NotifyInferior)
	    return TRUE;
	else
	    return FALSE;
    } else
	return FALSE;
}

int
WLEvent_leavewindow(wl_event, evt)
WOOL_Event	wl_event;
Event	evt;
{
    XEvent          report;

    if (evt -> xcrossing.detail != NotifyInferior) {
	/* XSync(dpy, 0); */
	if (!XCheckTypedWindowEvent(dpy, evt -> xcrossing.window,
				    EnterNotify, &report)
	    || report.xcrossing.detail == NotifyInferior)
	    return TRUE;
	else
	    return FALSE;
    } else
	return FALSE;
}

int
WLEvent_enterwindow_nograb(wl_event, evt)
WOOL_Event	wl_event;
Event	evt;
{
    XEvent          report;

    if (evt -> xcrossing.detail != NotifyInferior) {
	/* XSync(dpy, 0); */
	if ((!XCheckTypedWindowEvent(dpy, evt -> xcrossing.window,
				    LeaveNotify, &report)
	    || report.xcrossing.detail == NotifyInferior)
	    && evt -> xcrossing.mode != NotifyGrab
	    && evt -> xcrossing.mode != NotifyUngrab)
	    return TRUE;
	else
	    return FALSE;
    } else
	return FALSE;
}

int
WLEvent_leavewindow_nograb(wl_event, evt)
WOOL_Event	wl_event;
Event	evt;
{
    XEvent          report;

    if (evt -> xcrossing.detail != NotifyInferior) {
	/* XSync(dpy, 0); */
	if ((!XCheckTypedWindowEvent(dpy, evt -> xcrossing.window,
				    EnterNotify, &report)
	    || report.xcrossing.detail == NotifyInferior)
	    && evt -> xcrossing.mode != NotifyGrab
	    && evt -> xcrossing.mode != NotifyUngrab)
	    return TRUE;
	else
	    return FALSE;
    } else
	return FALSE;
}

/* Focus management
 */

int
WLEvent_focusin(wl_event, evt)
WOOL_Event	wl_event;
Event	evt;
{
    if (evt -> xfocus.detail == NotifyNonlinearVirtual
	 || TargetWob -> type == PlugClass) {
	return TRUE;
    } else
	return FALSE;
}

int
WLEvent_focusout(wl_event, evt)
WOOL_Event	wl_event;
Event	evt;
{
    if (evt -> xfocus.detail == NotifyNonlinearVirtual
	 || TargetWob -> type == PlugClass) {
	return TRUE;
    } else
	return FALSE;
}


/* visibility management
 */

int
WLEvent_visibility_partially_obscured(wl_event, evt)
WOOL_Event      wl_event;
Event   evt;
{
    return (evt -> xvisibility.state == VisibilityPartiallyObscured);
}

int
WLEvent_visibility_fully_obscured(wl_event, evt)
WOOL_Event      wl_event;
Event   evt;
{
    return (evt -> xvisibility.state == VisibilityFullyObscured);
}

int
WLEvent_visibility_unobscured(wl_event, evt)
WOOL_Event      wl_event;
Event   evt;
{
    return (evt -> xvisibility.state == VisibilityUnobscured);
}

/*
 * list of release procedures
 */

int
WLEvent_button_release(wl_event, evt)
WOOL_Event      wl_event;
Event   evt;
{
    wait_for_button_release(evt -> xbutton.button);
    return 0;
}

int
WLEvent_key_release(wl_event, evt)
WOOL_Event      wl_event;
Event   evt;
{
    wait_for_key_release(evt -> xkey.keycode);
    return 0;
}

/* mapping of window
 */

int
WLEvent_map_notify(wl_event, evt)
WOOL_Event      wl_event;
Event   evt;
{
    return TRUE;
}

/*
 * functions to make the different events
 */

WOOL_Event
WLEvent_user_make(data)
WOOL_Atom	data;
{
    WOOL_Event      event;

    if (data -> type == WLNumber && (((WOOL_Number) data) -> number == ANY))
	event = WLEvent_make(0, ANY);
    else
	event = WLEvent_make(0, (unsigned int)data);
    event -> x_type = GWMUserEvent;
    event -> match = WLEvent_user;
    event -> release = NULL;
    event -> mask = 0;
    event -> user_mask = 1;
    return event;
}

WOOL_Event
WLEvent_propertychange_make(name)
WOOL_String	name;
{
    WOOL_Event      event = WLEvent_make(0, 0);
    WOOL_Pointer    x_atom;
    Atom            property_name;

    event -> x_type = PropertyNotify;
    event -> match = WLEvent_propertychange;
    event -> release =  NULL;
    event -> mask = PropertyChangeMask;
    must_be_string(name, 0);
    if (wool_self_pointer_make(name -> string, '\030', &x_atom)) {
	property_name = *(x_atom -> ptr);
    } else {
	property_name = XInternAtom(dpy, name -> string, False);
	*(x_atom -> ptr) = property_name;
    }
    event -> detail = (unsigned int) property_name;    
    return event;
}

WOOL_Event
WLEvent_button_make(button, modifier)
WOOL_Number	modifier;
WOOL_Number	button;
{
    WOOL_Event      event = WLEvent_make(modifier -> number,
		  (button -> number == AnyButton ? ANY : button -> number));

    event -> x_type = ButtonPress;
    event -> match = WLEvent_buttonpress;
    event -> release =  WLEvent_button_release;
    event -> mask = ButtonPressMask | ButtonReleaseMask;
    return event;
}

WOOL_Event
WLEvent_buttonpress_make(button, modifier)
WOOL_Number	modifier;
WOOL_Number	button;
{
    WOOL_Event      event = WLEvent_button_make(button, modifier);

    event -> release = NULL;
    return event;
}

WOOL_Event
WLEvent_double_button_make(button, modifier)
WOOL_Number	modifier;
WOOL_Number	button;
{
    WOOL_Event      event = WLEvent_make(modifier -> number,
		  (button -> number == AnyButton ? ANY : button -> number));

    event -> x_type = ButtonPress;
    event -> match = WLEvent_double_buttonpress;
    event -> release =  WLEvent_button_release;
    event -> mask = ButtonPressMask | ButtonReleaseMask;
    return event;
}

WOOL_Event
WLEvent_double_buttonpress_make(button, modifier)
WOOL_Number	modifier;
WOOL_Number	button;
{
    WOOL_Event      event = WLEvent_double_button_make(button, modifier);

    event -> release = NULL;
    return event;
}

WOOL_Event
WLEvent_buttonrelease_make(button, modifier)
WOOL_Number	modifier;
WOOL_Number	button;
{
    WOOL_Event      event =
    WLEvent_button_make(button,
			WLNumber_make(((unsigned long) modifier -> number) |
	  (((unsigned long) Button1Mask) << (button -> number - Button1))));

    event -> x_type = ButtonRelease;
    event -> match = WLEvent_buttonrelease;
    event -> release = NULL;
    return event;
}

WOOL_Event
WLEvent_key_make(key, modifier)
WOOL_Number	modifier;
WOOL_Number	key;
{
    WOOL_Event      event;
    KeySym          keysym;

    must_be_number(modifier, 1);
    if (key -> type == WLNumber)
	keysym = key -> number;
    else {
	if ((keysym = XStringToKeysym(((WOOL_String) key) -> string))
	    == NoSymbol) {
	    wool_printf("unknown key name: \"%s\"\n", 
			((WOOL_String) key) -> string);
	    keysym = (KeySym) UNDEFINED_KEYCODE;
	    }
    }
    event = WLEvent_make(modifier -> number, keysym);
    event -> x_type = KeyPress;
    event -> match = WLEvent_keypress;
    event -> release = WLEvent_key_release;
    event -> mask = KeyPressMask | KeyReleaseMask;
    return event;
}

WOOL_Event
WLEvent_keypress_make(key, modifier)
WOOL_Number	modifier;
WOOL_Number	key;
{
    WOOL_Event      event = WLEvent_key_make(key, modifier);

    event -> release = NULL;
    return event;
}

WOOL_Event
WLEvent_keyrelease_make(key, modifier)
WOOL_Number	modifier;
WOOL_Number	key;
{
    WOOL_Event      event = WLEvent_key_make(key, modifier);

    event -> x_type = KeyRelease;
    event -> match = WLEvent_keyrelease;
    event -> release = NULL;
    return event;
}

/*
 * Initialize all the events
 */

void
wool_all_events_make()
{
    grab_queue_init();

    wool_atom_with_numeric_value_make("any", ANY);
    wool_atom_with_numeric_value_make("alone", 0);
    wool_atom_with_numeric_value_make("with-button-1", Button1Mask);
    wool_atom_with_numeric_value_make("with-button-2", Button2Mask);
    wool_atom_with_numeric_value_make("with-button-3", Button3Mask);
    wool_atom_with_numeric_value_make("with-button-4", Button4Mask);
    wool_atom_with_numeric_value_make("with-button-5", Button5Mask);
    wool_atom_with_numeric_value_make("with-shift", ShiftMask);
    wool_atom_with_numeric_value_make("with-lock", LockMask);
    wool_atom_with_numeric_value_make("with-control", ControlMask);
    wool_atom_with_numeric_value_make("with-alt", Mod1Mask);
    wool_atom_with_numeric_value_make("with-modifier-1", Mod1Mask);
    wool_atom_with_numeric_value_make("with-modifier-2", Mod2Mask);
    wool_atom_with_numeric_value_make("with-modifier-3", Mod3Mask);
    wool_atom_with_numeric_value_make("with-modifier-4", Mod4Mask);
    wool_atom_with_numeric_value_make("with-modifier-5", Mod5Mask);

    wool_subr_make(WLSubr, WLEvent_buttonpress_make, "buttonpress", 2);
    wool_subr_make(WLSubr, WLEvent_keypress_make, "keypress", 2);
    wool_subr_make(WLSubr, WLEvent_button_make, "button", 2);
    wool_subr_make(WLSubr, WLEvent_key_make, "key", 2);
    wool_subr_make(WLSubr, WLEvent_buttonrelease_make, "buttonrelease", 2);
    wool_subr_make(WLSubr, WLEvent_keyrelease_make, "keyrelease", 2);
    wool_subr_make(WLSubr, WLEvent_user_make, "user-event", 1);
    wool_subr_make(WLSubr, WLEvent_propertychange_make, "property-change", 1);
    wool_subr_make(WLSubr, WLEvent_double_buttonpress_make,
		   "double-buttonpress", 2);
    wool_subr_make(WLSubr, WLEvent_double_button_make, "double-button", 2);
    wool_event_make("name-change", PropertyNotify, WLEvent_propertychange,
    NULL, PropertyChangeMask, 0, XA_WM_NAME);
    wool_event_make("window-icon-pixmap-change", PropertyNotify,
		    WLEvent_property_icon_pixmap_change, NULL,
		    PropertyChangeMask, 0, XA_WM_HINTS);
    wool_event_make("geometry-change", ConfigureNotify,
    WLEvent_configurenotify, NULL, SubstructureNotifyMask, 0, 0);
    wool_event_make("enter-window", EnterNotify, WLEvent_enterwindow, NULL,
		    EnterWindowMask, 0, 0);
    wool_event_make("leave-window", LeaveNotify, WLEvent_leavewindow, NULL,
		    LeaveWindowMask, 0, 0);
    wool_event_make("enter-window-not-from-grab", EnterNotify,
		    WLEvent_enterwindow_nograb, NULL,
		    EnterWindowMask, 0, 0);
    wool_event_make("leave-window-not-from-grab", LeaveNotify,
		    WLEvent_leavewindow_nograb, NULL,
		    LeaveWindowMask, 0, 0);
    wool_event_make("focus-in", FocusIn, WLEvent_focusin, NULL,
		    FocusChangeMask, 0, 0);
    wool_event_make("focus-out", FocusOut, WLEvent_focusout, NULL,
		    FocusChangeMask, 0, 0);
    wool_event_make("map-notify", MapNotify, WLEvent_map_notify, NULL,
		    StructureNotifyMask, 0, 0);
    wool_event_make("visibility-unobscured", VisibilityNotify,
		    WLEvent_visibility_unobscured,
		    NULL, VisibilityChangeMask, 0, 0);
    wool_event_make("visibility-partially-obscured", VisibilityNotify,
		    WLEvent_visibility_partially_obscured,
		    NULL, VisibilityChangeMask, 0, 0);
    wool_event_make("visibility-fully-obscured", VisibilityNotify,
		    WLEvent_visibility_fully_obscured,
		    NULL, VisibilityChangeMask, 0, 0);
}

/*
 * wait for release of button n
 */

int
LookForButtonRelease(display, evt, arg)
Display *display;
XEvent *evt;
char *arg;
{
    int n = (int)arg;
    if ((evt -> type == ButtonRelease) && (evt -> xbutton.button == n))
	return True;
    else
	return False;
}

wait_for_button_release(n)
int n;
{
    XEvent	    button_event;

    XIfEvent(dpy, &button_event, LookForButtonRelease, (char *)n);
}

/*
 * wait for release of key of code n
 */

int
LookForKeyRelease(display, evt, arg)
Display *display;
XEvent *evt;
char *arg;
{
    if ((evt -> type == KeyRelease) && (evt -> xkey.keycode == (int)arg))
	return True;
    else
	return False;
}

wait_for_key_release(n)
int n;
{
    XEvent	    button_event;

    XIfEvent(dpy, &button_event, LookForKeyRelease, (char *)n);
}

/* grab management
 */

WOOL_OBJECT
set_grab(wob)
Wob	wob;
{
    WOOL_Cursor     cursor;
    int             status;

    if (wob -> type == MenuClass)
	cursor = (WOOL_Cursor) wob -> cursor;
    else
	cursor =
	    (WOOL_Cursor) WOOL_send(WOOL_eval, WA_cursor, (WA_cursor));
    if ((status =
	 XGrabPointer(dpy, wob -> hook, True,
		      LegalPointerEventMask(((WOOL_Fsm) wob -> fsm) -> mask),
		      GrabModeAsync, GrabModeAsync,
		      (GWM_confine_grabs ? wob -> hook : None),
		      (cursor == (WOOL_Cursor) NIL ? None : cursor -> cursor),
		      CurrentTime))
	!= GrabSuccess)
	return (WOOL_OBJECT) WLNumber_make(status);
    if (GWM_grab_keyboard_also)
	if ((status = XGrabKeyboard(dpy, wob -> hook, True,
				 GrabModeAsync, GrabModeAsync, CurrentTime))
	    != GrabSuccess) {
	    XUngrabPointer(dpy, CurrentTime);
	    return (WOOL_OBJECT) WLNumber_make(status);
	} else {
	    GWM_KeyboardGrabbed = wob;
	}
    if (GWM_GrabServer)
	XGrabServer(dpy);
    GWM_ServerGrabbed = wob;
    return NIL;
}

WOOL_OBJECT
remove_grab(wob)
Wob	wob;
{
    if (!wob || wob == GWM_ServerGrabbed) {
	if (GWM_ServerGrabbed)
	  XUngrabPointer(dpy, CurrentTime);
	if (GWM_KeyboardGrabbed)
	  XUngrabKeyboard(dpy, CurrentTime);
	if (GWM_GrabServer)
	  XUngrabServer(dpy);
	XSync(dpy, 0);
	GWM_ServerGrabbed = GWM_KeyboardGrabbed = NULL;
	FlushQueuedGrabEvents();
    }
    return NIL;
}

WOOL_OBJECT
wool_set_grab(argc, argv)
int	argc;
WOOL_Number	argv[];
{
    GWM_GrabChildEvents = 0;
    switch (argc) {
    case 0:
	return set_grab(TargetWob);
    case 2:
	GWM_GrabChildEvents = 1;
    }
    must_be_number(argv[0], 0);
    return set_grab(argv[0] -> number);
}

WOOL_OBJECT
wool_remove_grab(argc, argv)
int	argc;
WOOL_Number	argv[];
{
    if (!argc)
	return remove_grab(0);
    must_be_number(argv[0], 0);
    return remove_grab(argv[0] -> number);
}

/* replay back event in a grab
 * if arg != NIL, replay key event
 */

WOOL_OBJECT
wool_ungrab_and_replay_event(key)
WOOL_OBJECT	key;
{
    if (key == NIL)
	XAllowEvents(dpy, ReplayPointer, CurrentTime);
    else
	XAllowEvents(dpy, ReplayKeyboard, CurrentTime);
    return NIL;
}

/* makes a button event replayable */

WOOL_OBJECT
wool_makes_replayable(event)
WOOL_Event 	event;
{
    must_be_event(event, 0);
    event -> flags |= FREEZE_ON_GRABS;
    return (WOOL_OBJECT) event;
}

/* allows event processing after a freeze due to a replayable event
 */

WOOL_OBJECT
wool_allow_events()
{
    XAllowEvents(dpy, AsyncPointer, CurrentTime);
    XAllowEvents(dpy, AsyncKeyboard, CurrentTime);
    return NIL;
}

/* last events of some type
 */

WOOL_OBJECT
wool_last_key_struck()
{
    char            s[81];
    int             l;

    if ((TriggeringEvent -> type == KeyPress ||
	 TriggeringEvent -> type == KeyRelease)
	&& (l = XLookupString(&TriggeringEvent->xkey, s, 80, 0, 0))) {
	s[l] = '\0';
	return (WOOL_OBJECT) WLString_make(s);
    } else
	return NIL;
}

WOOL_OBJECT
wool_last_user_event()
{
    if (TriggeringEvent -> type == GWMUserEvent)
	return (WOOL_OBJECT) TriggeringEvent -> xany.display;
    else
	return wool_error("last event%s was not an user event", "");
}

Event
last_key_or_button_event(evt)
Event	evt;
{
    if (event_is_key_or_button(*TriggeringEvent))
	return TriggeringEvent;
    else
	return &GWM_LastEvent;
}

/*
 * returns the coords of the click relative to the client as a list
 * (logical-x logical-y x y) for the current window's client
 */

#ifdef NOBASEDIMS
#ifdef PBaseSize
#undef PBaseSize
#endif
#define PBaseSize PMinSize
#define base_width min_width
#define base_height min_height
#endif /* NOBASEDIMS */


WOOL_OBJECT
wool_logical_coords()
{
    int             x=0 , y=0;
    Window          child;
    WOOL_List       wl_list = wool_list_make(6);
    Event           evt = last_key_or_button_event(TriggeringEvent);
    XSizeHints     *normal_hints =
	&(TargetWindow -> cached_props -> normal_hints);

    if (TargetWindow -> client)
	XTranslateCoordinates(dpy, evt -> xbutton.root,
			      TargetWindow -> client, evt -> xbutton.x_root,
			      evt -> xbutton.y_root, &x, &y, &child);
    increase_reference(wl_list -> list[0] = (WOOL_OBJECT) WLNumber_make(
        (x - (normal_hints -> flags & PBaseSize ?
	      normal_hints -> base_width : 0))
	 / (normal_hints -> flags & PResizeInc ?
	    normal_hints -> width_inc : 1)));
    increase_reference(wl_list -> list[1] = (WOOL_OBJECT) WLNumber_make(
        (y - (normal_hints -> flags & PBaseSize ?
	      normal_hints -> base_height : 0))
	 / (normal_hints -> flags & PResizeInc ?
	    normal_hints -> height_inc : 1)));
    increase_reference(wl_list -> list[2] = (WOOL_OBJECT) WLNumber_make(x));
    increase_reference(wl_list -> list[3] = (WOOL_OBJECT) WLNumber_make(y));

    XTranslateCoordinates(dpy, evt -> xbutton.root,
                              TargetWindow -> hook, evt -> xbutton.x_root,
                              evt -> xbutton.y_root, &x, &y, &child);
    increase_reference(wl_list -> list[4] = (WOOL_OBJECT) WLNumber_make(x));
    increase_reference(wl_list -> list[5] = (WOOL_OBJECT) WLNumber_make(y));

    return (WOOL_OBJECT) wl_list;
}

int EventMasks[]={0,0,
	KeyPressMask,
	KeyReleaseMask,
	ButtonPressMask,
	ButtonReleaseMask};

WOOL_OBJECT
wool_resend_event(wob)
WOOL_Number	wob;
{
    Window          window;
    Event	    evt = last_key_or_button_event(TriggeringEvent);

    if (wob = (WOOL_Number) NIL)
	window = TargetWindow -> client;
    else
	window = ((Wob) wob) -> hook;
    if (window) {
	evt -> xany.window = window;
	XSendEvent(dpy, TargetWindow -> client, False,
		   EventMasks[evt->type], evt);
	XFlush(dpy);
    }
    return (WOOL_OBJECT) NIL;
}

WOOL_OBJECT
wool_send_keycode_to_client(keycode, mod)
WOOL_Number	keycode, mod;
{
    XEvent event;

    fill_x_key_event(&event, keycode -> number, mod -> number);
    XSendEvent(dpy, TargetWindow -> client, False, KeyPressMask, &event);
    fill_x_key_event(&event, keycode -> number, mod -> number);
    event.type = KeyRelease;
    XSendEvent(dpy, TargetWindow -> client, False, KeyReleaseMask, &event);
    return (WOOL_OBJECT) keycode;
}

WOOL_OBJECT
wool_send_key_to_client(key, mod)
WOOL_Number	key, mod;
{
    XEvent event;
    char           *p = 0;
    int             keysym, keycode;
    int modifier;

    must_be_number(mod, 1);
    if (key -> type != WLNumber) {
	must_be_string(key, 0);
	p = ((WOOL_String) key) -> string;
	if (!(*p))
	    return NIL;
    }
    do {				/* we MUST re-init event each time */
	modifier = mod -> number;
	if (key -> type == WLNumber)
	    keysym = key -> number;
	else {
	    keysym = *p++;
	}
	if (!(keycode = XKeysymToKeycode(dpy, keysym)))
	    return wool_error("unknown keysym for this server: 0x%x", keysym);

	/* add shift automatically for shifted chars */
	/* We should do same thing for control chars & meta... */
	if (keysym != XKeycodeToKeysym(dpy, keycode, 0)) {
	    if (keysym == XKeycodeToKeysym(dpy, keycode, 1)) {
		modifier |= ShiftMask;
	    }
	}

	fill_x_key_event(&event, keycode, modifier);
	XSendEvent(dpy, TargetWindow -> client, False, KeyPressMask, &event);
	fill_x_key_event(&event, keycode, modifier);
	event.type = KeyRelease;
	XSendEvent(dpy, TargetWindow -> client, False, KeyReleaseMask,
		   &event);
    } while (p && *p);
    return (WOOL_OBJECT) key;
}

fill_x_key_event(evt, keycode, modifier)
XKeyPressedEvent *evt;
int keycode, modifier;
{
    evt -> type = KeyPress;
    evt -> display = dpy;
    evt -> window =  evt -> subwindow = TargetWindow -> client;
    evt -> root = Context->root;
    evt -> time = CurrentTime;
    evt -> x = evt -> y = evt -> x_root = evt -> y_root = 0;
    evt -> same_screen = 1;
    evt -> keycode = keycode;
    evt -> state = modifier;
}

/* same with button: button modifier x y
 */

fill_x_button_event(evt, button, modifier, x, y, x_root, y_root, child)
XButtonEvent *evt;
int button, modifier, x, y, x_root, y_root;
Window child;
{
    evt -> type = ButtonPress;
    evt -> display = dpy;
    evt -> window = child;
    evt -> subwindow = 0;
    evt -> root = Context->root;
    evt -> time = CurrentTime;
    evt -> x = x;
    evt -> y = y;
    evt -> x_root = x_root;
    evt -> y_root = y_root;
    evt -> same_screen = 1;
    evt -> button = button;
    evt -> state = modifier;
}

/* look which window would receive button event
 */

Window
WindowGettingButtonEvent(w, x, y)
Window w;
int x,y;
{
    int x2, y2;
    Window child, w2 = w;
    XWindowAttributes wa;

 find_window:
    XTranslateCoordinates(dpy, w, w2, x, y, &x2, &y2, &child);
    if (child) {
	x = x2;
	y = y2;
	w = w2;
	w2 = child;
	goto find_window;
    }
    x = x2;
    y = y2;
    w = w2;

 find_listener:
    XGetWindowAttributes(dpy, w, &wa);
    if (!(wa.all_event_masks & (ButtonPressMask | ButtonReleaseMask))) {
	Window d1, d2, *d3, parent;
	unsigned int d4;
	
	XQueryTree(dpy, w, &d1, &parent, &d3, &d4);
	if (parent) {
	    w = parent;
	    goto find_listener;
	}
    }
    return w;
}

WOOL_OBJECT
wool_send_button_to_client(argc, argv)
int	argc;
WOOL_Number	argv[];
{
    XEvent event;
    int button, modifier, x, y, x2, y2, x_root, y_root, i;
    Window child, dummy;

    if (argc != 4)
	return wool_error(BAD_NUMBER_OF_ARGS, argc);
    for (i = 0; i < argc; i++)
	must_be_number(argv[i], i);
    button = argv[0] -> number;
    modifier = argv[1] -> number;
    x = argv[2] -> number;
    y = argv[3] -> number;

    XTranslateCoordinates(dpy, TargetWindow -> client, Context -> root, x, y,
			  &x_root, &y_root, &child);

    child = WindowGettingButtonEvent(TargetWindow -> client, x, y);
    x2 =x, y2 =y;
    XTranslateCoordinates(dpy, TargetWindow -> client, child, x2, y2,
			  &x, &y, &dummy);

    fill_x_button_event(&event, button, modifier, x, y, x_root, y_root, child);
    XSendEvent(dpy, child, False, ButtonPressMask, &event);
    fill_x_button_event(&event, button, modifier, x, y, x_root, y_root, child);
    event.type = ButtonRelease;
    XSendEvent(dpy, child, False, ButtonReleaseMask, &event);
    return (WOOL_OBJECT) argv[0];
}

/* management of the enter/leave on grabs for menus
 * these functions are used
 * - just before sending event in GWM_ProcessEvents (gwm.c)
 * 	    if (IsNotGrabDiscarded(&evt)) {
 * 		send-event...
 * 	    }
 * - in remove_grab (at the end) for FlushQueuedGrabEvents
 * 
 * - in wool_all_events_make for grab_queue_init
 */

static XEvent *grab_queue;
static int     grab_queue_start;
static int     grab_queue_size;
static int     grab_queue_limit;

grab_queue_init()
{
    grab_queue_size = 0;
    grab_queue_limit = 1;
    grab_queue = (XEvent *) Malloc(grab_queue_limit * sizeof(XEvent));
}

int
IsNotGrabDiscarded(evt)
Event	evt;
{
    if (((evt -> type == LeaveNotify)
	 && (evt -> xcrossing.mode == NotifyGrab))
	|| ((evt -> type == FocusOut)
	    && (evt -> xfocus.mode == NotifyGrab))) {
	if (grab_queue_size == grab_queue_limit) {
	    grab_queue_limit *= 2;
	    grab_queue = (XEvent *) Realloc(grab_queue, grab_queue_limit
					    * sizeof(XEvent));
	}
	bcopy(evt, &grab_queue[grab_queue_size++], sizeof(XEvent));
	return FALSE;
    } else {
	if (((evt -> type == EnterNotify)
	 && (evt -> xcrossing.mode == NotifyUngrab))
	|| ((evt -> type == FocusIn)
	    && (evt -> xfocus.mode == NotifyUngrab))) {
	    return TRUE;
	} else {
	    return FALSE;
	}
    }
}

FlushQueuedGrabEvents()
{
    Wob             wob;

    SAVE_EVENT_CONTEXT;
    while (grab_queue_start < grab_queue_size) {
	grab_queue_start++;
	if (wob = (Wob) LookUpWob(grab_queue[grab_queue_start].xany.window)) {
	    WOOL_send(WOOL_process_event, wob,
		      (wob, &grab_queue[grab_queue_start]));
	}
    }
    RESTORE_EVENT_CONTEXT;
    grab_queue_size = 0;
    grab_queue_start = 0;
}


/*
 * recuperates parameters of the triggering event
 */

#define field_of_event(evt, event_property, etype, field) \
    if(TriggeringEvent->type > 1 && TriggeringEvent->type < LASTEvent && \
       EventProperties[TriggeringEvent->type] & event_property) \
	return (WOOL_OBJECT) WLNumber_make(((etype *) evt) -> field); \
    else return (WOOL_OBJECT) WLNumber_make(0);

WOOL_OBJECT
wool_get_triggering_event_state()
{
    field_of_event(TriggeringEvent, EPKeyOrButton, XButtonEvent, state);
}

WOOL_OBJECT
wool_get_triggering_event_code()
{
    field_of_event(TriggeringEvent, EPKeyOrButton, XButtonEvent, button);
}

WOOL_OBJECT
wool_get_triggering_event_x()
{
    field_of_event(TriggeringEvent, EPXYRoot, XButtonEvent, x_root);
}

WOOL_OBJECT
wool_get_triggering_event_y()
{
    field_of_event(TriggeringEvent, EPXYRoot, XButtonEvent, y_root);
}

WOOL_OBJECT
wool_get_triggering_event_x_relative()
{
    field_of_event(TriggeringEvent, EPXY, XButtonEvent, x);
}

WOOL_OBJECT
wool_get_triggering_event_y_relative()
{
    field_of_event(TriggeringEvent, EPXY, XButtonEvent, y);
}

WOOL_OBJECT
wool_get_triggering_event_time()
{
    field_of_event(TriggeringEvent, EPXY, XButtonEvent, time);
}

/* used to know if current event was due to a grab */

WOOL_OBJECT
wool_event_was_due_to_a_grab()
{
    WOOL_Atom             result = (WOOL_Atom) NIL;

    if (event_is_crossing(*TriggeringEvent)) {
	if (TriggeringEvent -> xcrossing.mode == NotifyGrab)
	    result = WA_grab;
	else if (TriggeringEvent -> xcrossing.mode == NotifyUngrab)
	    result = WA_ungrab;
    } else if (event_is_focus(*TriggeringEvent)) {
	if (TriggeringEvent -> xfocus.mode == NotifyGrab)
	    result = WA_grab;
	else if (TriggeringEvent -> xfocus.mode == NotifyUngrab)
	    result = WA_ungrab;
    }
    return (WOOL_OBJECT) result;
}

/********************\
* 		     *
* Tracing of events  *
* 		     *
\********************/

#ifdef DEBUG

char *FocusDetailText[] = {
    "NotifyAncestor",		/* 0 */
    "NotifyVirtual",		/* 1 */
    "NotifyInferior",		/* 2 */
    "NotifyNonlinear",		/* 3 */
    "NotifyNonlinearVirtual",	/* 4 */
    "NotifyPointer",		/* 5 */
    "NotifyPointerRoot",	/* 6 */
    "NotifyDetailNone"		/* 7 */
};

char *FocusModeText[] = {
    "NotifyNormal",            /* 0 */
    "NotifyGrab",              /* 1 */
    "NotifyUngrab",            /* 2 */
    "NotifyWhileGrabbed"       /* 3 */
};

GWM_prettyprint_fsm_event(wob, evt, text, state, newstate)
Wob	wob;
Event           evt;
char	*text;
int state, newstate;
{
    fprintf(stderr, "%s %s (0x%lx/0x%lx): ",
	    text, ((WOOL_Atom) wob -> type[0])-> p_name,
	    wob, wob -> hook);
    if (state || newstate) {
	fprintf(stderr, " (s%d", state);
	if (state != newstate) {
	    fprintf(stderr, "->%d", newstate);
	}
	fprintf(stderr, ")");
    }
    GWM_prettyprint_event(" ", evt, "\n");
}

GWM_prettyprint_event(tag, evt, end)
    char *tag;
    Event evt;
    char *end;
{
    fputs(tag, stderr);
    fprintf(stderr, "%s ", eventtype[evt -> type]); 
    switch (evt->type) {
    case GWMUserEvent:
	fprintf(stderr, "%s",
	    ((WOOL_String) (evt->xany.display))->string);
	break;
    case FocusIn:
    case FocusOut:
	fprintf(stderr, "w 0x%x, detail %s, mode %s",
	    evt->xfocus.window,
	    FocusDetailText[evt->xfocus.detail],
	    FocusModeText[evt->xfocus.mode]);
	break;
    case EnterNotify:
    case LeaveNotify:
	fprintf(stderr, "w 0x%x, detail %s, mode %s",
	    evt->xcrossing.window,
	    FocusDetailText[evt->xcrossing.detail],
	    FocusModeText[evt->xcrossing.mode]);
	break;
    case PropertyNotify:
	fprintf(stderr, "w 0x%x, %s",
	    evt->xproperty.window,
	    XGetAtomName(dpy, evt->xproperty.atom));
	break;
    case ReparentNotify:
	fprintf(stderr, "w 0x%x to 0x%x %s",
	    evt->xreparent.window, evt->xreparent.parent,
		(evt->xreparent.override_redirect ? "(Override)":""));
	break;

    default:
	fprintf(stderr, "w 0x%x", evt->xany.window);
    }
    fputs(end, stderr);    
}

#endif /* DEBUG */
