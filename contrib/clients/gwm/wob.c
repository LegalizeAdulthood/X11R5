/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*******************************************************\
* 						        *
* 	BULL WINDOW MANAGER for X11		        *
* 						        *
* 		module managing Wobs.		        *
* Object Oriented Programming, but NO reference count!  *
* 						        *
\*******************************************************/

/*  include  */

#include        "EXTERN.h"
#include 	"gwm.h"
#include	"wl_atom.h"
#include	"wl_list.h"
#include	"wl_number.h"
#include	"wl_cursor.h"
#include	"wl_pixmap.h"


/*  external  */

/*  local  */

/*  global  */

extern WOB_METHOD       BarClass[], PlugClass[], ClientWindowClass[], 
			ScreenClass[];


/*
 * search all wobs for the one having the same X window as the evnt and
 * returns it.
 */

Wob 
LookUpWob(window)
Window           window;
{
    Wob             wob;

    if (!XFindContext(dpy, window, wob_context, (char **)&wob))
	return wob;
    else
	return 0;
}

/* same but callable from wool
 */

WOOL_OBJECT
LookUpWobW(wl_window)
WOOL_Number           wl_window;
{
    Wob             wob;

    if (!XFindContext(dpy, wl_window->number, wob_context, (char **)&wob))
	return (WOOL_OBJECT) WLNumber_make(wob);
    else if (!XFindContext(dpy,  wl_window->number, client_context, (char **)&wob))
	return (WOOL_OBJECT) WLNumber_make(wob);
    else
	return NIL;
}

WobRecordHook(wob)
Wob	wob;
{
    XSaveContext(dpy, wob -> hook, wob_context, (char *)wob);
}

ClientWindow
LookUpClient(window)
Window           window;
{
    Wob             wob;

    if (!XFindContext(dpy, window, client_context, (char **)&wob))
	return (ClientWindow)  wob;
    else
	return 0;
}

WOOL_OBJECT
Client2WindowW(clientid)
    WOOL_Number clientid;
{
    ClientWindow cw;

    must_be_number(clientid, 0);
    if (cw = LookUpClient(clientid->number)) {
	return (WOOL_OBJECT) WLNumber_make(cw);
    } else {
	return NIL;
    }
}

WOOL_OBJECT
Window2ClientW(w)
    WOOL_Number w;
{
    ClientWindow cw;

    must_be_number(w, 0);
    cw = (ClientWindow) w->number;
    if (WobIsValid(cw) && cw->type == ClientWindowClass && cw->client) {
	return (WOOL_OBJECT) WLNumber_make(cw->client);
    } else {
	return NIL;
    }
}

int
IsAnAncestor(parent, child)
Wob parent, child;
{
    Wob             wob = child;

    while (wob){
	if(wob == parent)
		return TRUE;
	wob = wob -> parent;
    }
    return FALSE;    
}

Wob 
NewWob(size)
int             size;
{
    Wob         new;

    new = (Wob) Malloc(size);
    bzero(new, size);
    new -> valid = WobMAGIC;
    return new;
}

WobRelease(wob)
Wob             wob;
{
    decrease_reference(wob -> bordertile);
    decrease_reference(wob -> menu);
    decrease_reference(wob -> fsm);
    decrease_reference(wob -> cursor);
    decrease_reference(wob -> tile);
    decrease_reference(wob -> property);
    XDeleteContext(dpy, wob -> hook, wob_context);
    XDestroyWindow(dpy, wob -> hook);
    wob -> valid = 0;
    DelayedFree(wob);
}

WobEventHandler(wob, evt)
Wob	wob;
Event	evt;
{
    WLFsm_action(wob->fsm, wob, evt);
}

/*
 * default routines to respond to methods
 */

WobEval(wob)
Wob wob;
{
    wool_error(INTERNAL_ERROR, "Trying to eval a Wob!");
}

WobPrint(wob)
Wob wob;
{
    wool_puts("{");
    wool_print_type(wob);
    wool_printf("[0x%x", wob);
    wool_printf("/0x%x]}", wob -> hook);
}

WobExecute(wob)
Wob wob;
{
    wool_error(INTERNAL_ERROR, "Trying to execute a Wob!");
}

WobSet(wob)
Wob wob;
{
    wool_error(INTERNAL_ERROR, "Trying to set a Wob!");
}

WobGetCValue(wob)
Wob wob;
{
    wool_error(INTERNAL_ERROR, "Trying to get the c_value of a Wob!");
}

WobActionRoutine(wob)
Wob wob;
{
    wool_error(INTERNAL_ERROR, "No Action defined for this wob!");
}

/*
 * functions to query about wobs
 */

WOOL_OBJECT
wool_current_wob()
{
    return (WOOL_OBJECT) WLNumber_make(TargetWob);
}

WOOL_OBJECT
wool_current_wob_set(wob)
WOOL_Number wob;
{
    must_be_number(wob, 0);
    if (!WobIsValid(wob->number))
	wool_error(INTERNAL_ERROR, "Not a Wob!");
    SetTarget(wob -> number);
    return (WOOL_OBJECT) wob;
}

WOOL_OBJECT
wool_wob_is_valid(wob)
WOOL_Number wob;
{
    must_be_number(wob, 0);
    if (WobIsValid(wob->number))
	return (WOOL_OBJECT) wob;
    else
	return NIL;
}

WOOL_OBJECT
wool_window_is_valid(wob)
WOOL_Number wob;
{
    must_be_number(wob, 0);
    if (WobIsValid(wob->number) &&
	((Wob)(wob->number))->type == ClientWindowClass)
	return (WOOL_OBJECT) wob;
    else
	return NIL;
}

WOOL_OBJECT
wool_current_wob_width()
{
    return (WOOL_OBJECT) WLNumber_make(TargetWob -> box.width);
}

WOOL_OBJECT
wool_current_wob_height()
{
    return (WOOL_OBJECT) WLNumber_make(TargetWob -> box.height);
}

WOOL_OBJECT
current_wob_get_bitmap()
{
    return TargetWob -> tile;
}

WOOL_OBJECT
current_wob_set_bitmap(bitmap)
WOOL_Pixmap bitmap;
{
    if (TargetWob -> type == BarClass)
	set_bar_bitmap(TargetWob, bitmap);
    else if (TargetWob -> type == PlugClass)
	set_plug_bitmap(TargetWob, bitmap);
    else if (TargetWob -> type == ScreenClass)
        wool_set_screen_background(bitmap);
    return (WOOL_OBJECT) bitmap;
}

WOOL_OBJECT
current_wob_get_borderpixel()
{
    return (WOOL_OBJECT) WLNumber_make(TargetWob -> box.borderpixel);
}

WOOL_OBJECT
current_wob_set_borderpixel(color)
WOOL_Number	color;
{
    if (color -> type == WLNumber) {
	TargetWob -> box.borderpixel = color -> number;
	XSetWindowBorder(dpy, TargetWob -> hook, color -> number);
    } else {
	decrease_reference(TargetWob -> bordertile);
	increase_reference(TargetWob -> bordertile = (WOOL_OBJECT) color);
	XSetWindowBorderPixmap(dpy, TargetWob -> hook,
			 ((WOOL_Pixmap) (TargetWob -> bordertile)) -> pixmap);
    }
    return (WOOL_OBJECT) color;
}

WOOL_OBJECT
current_wob_get_background()
{
    return (WOOL_OBJECT) WLNumber_make(TargetWob -> box.background);
}

WOOL_OBJECT
current_wob_set_background(color)
WOOL_Number	color;
{
    if (color -> type == WLNumber) {
	TargetWob -> box.background = color -> number;
	XSetWindowBackground(dpy, TargetWob -> hook, color -> number);
    }
    if (TargetWob -> type == PlugClass) {
	WOOL_send(WOOL_redraw, ((Plug) TargetWob) -> graphic,
		  (((Plug) TargetWob) -> graphic, TargetWob));
    } else {
	XClearWindow(dpy, TargetWob -> hook);
    }
    return (WOOL_OBJECT) color;
}


WOOL_OBJECT
current_wob_get_borderwidth()
{
    return (WOOL_OBJECT) WLNumber_make(TargetWob -> box.borderwidth);
}

/* change a wob's borderwidth
 * If it is not a window on the screen reconfigure the father
 */

WOOL_OBJECT
current_wob_set_borderwidth(width)
WOOL_Number width;
{
    extern WOB_METHOD ScreenClass[];

    must_be_number(width, 0);
    XSetWindowBorderWidth(dpy, TargetWob -> hook,
			  TargetWob -> box.borderwidth = width -> number);
    /* TO_DO: reconfigure inside of bars! or composite wobs hack follows*/
    if (TargetWob -> type == BarClass && TargetWob -> parent &&
	TargetWob -> parent -> type == ClientWindowClass)
	ReconfigureBar(TargetWob, TargetWob -> parent);

    if (TargetWob -> parent) {
	if (TargetWob -> parent -> type != ScreenClass) {
	    WOOL_send(WOOL_reconfigure, TargetWob -> parent,
		      (TargetWob -> parent, TargetWob));
	}
    }
    return (WOOL_OBJECT) width;
}

WOOL_OBJECT
current_wob_invert_aera()
{
    XSetForeground(dpy, Context->gc.Draw, Context -> pixel.InvertColor);
    XFillRectangle(dpy, TargetWob -> hook, Context->gc.Draw, 0, 0,
		   TargetWob -> box.width, TargetWob -> box.height);
    return NIL;
}
	
WOOL_OBJECT
Wob_set_property(property)
WOOL_OBJECT property;
{
    WOOL_OBJECT     object = TargetWob -> property;

    must_be_or_nil(WLList, property, 0);
    increase_reference(TargetWob -> property = property);
    decrease_reference(object);
    return (property);
}
	
WOOL_OBJECT
Wob_get_property()
{
    return (TargetWob -> property);
}
	
WOOL_OBJECT
Wob_set_menu(menu)
WOOL_OBJECT menu;
{
    WOOL_OBJECT     object = TargetWob -> menu;

    increase_reference(TargetWob -> menu = menu);
    decrease_reference(object);
    return (menu);
}
	
WOOL_OBJECT
Wob_get_menu()
{
    return (TargetWob -> menu);
}
	
WOOL_OBJECT
Wob_set_fsm(fsm)
WOOL_OBJECT fsm;
{
    WOOL_OBJECT     object = TargetWob -> fsm;

    increase_reference(TargetWob -> fsm = fsm);
    decrease_reference(object);
    TargetWob -> curstate = (int)
	WOOL_send(WOOL_open, TargetWob -> fsm, (TargetWob -> fsm));
    return (fsm);
}
	
WOOL_OBJECT
Wob_get_fsm()
{
    return (TargetWob -> fsm);
}

int
WobGetDimensions(wob, box)
Wob	wob;
Box	box;
{
    box -> x = wob -> box.x;
    box -> y = wob -> box.y;
    box -> width = wob -> box.width + 2 * wob -> box.borderwidth;
    box -> height = wob -> box.height + 2 * wob -> box.borderwidth;
    return NULL;
}

/* if given a number, returns the wobs at this adress, otherwise returns type
 */

WOOL_OBJECT
GetWobOfNumber(number)
WOOL_Number number;
{
    if (number -> type == WLNumber)
	return (WOOL_OBJECT) number -> number;
    else
	return (WOOL_OBJECT) number;
}

/* to set/get the cursor of a wob
 */

WOOL_OBJECT
wool_wob_cursor_get()
{
    return TargetWob -> cursor;
}

WOOL_OBJECT
wool_wob_cursor_set(cursor)
WOOL_Cursor	cursor;
{
    XSetWindowAttributes wa;

    must_be_or_nil(WLCursor, cursor, 0);
    decrease_reference(TargetWob -> cursor);
    increase_reference(TargetWob -> cursor = (WOOL_OBJECT) cursor);
    wa.cursor = cursor -> cursor;
    XChangeWindowAttributes(dpy, TargetWob -> hook, CWCursor, &wa);
    return (WOOL_OBJECT) cursor;
}

/* function called when a wob is tried to be open with a 0 dimension
 */

GWM_no_size_window_error(wob)
Wob	wob;
{
    char           *explanation;

    if (wob -> box.width) {
	if (wob -> box.height)
	    return;
	else
	    wob -> box.height = 1, explanation = "height";
    } else {
	if (wob -> box.height)
	    wob -> box.width = 1, explanation = "width";
	else
	    wob -> box.height = 1, wob -> box.width = 1, explanation = "size";
    }
    wool_puts("GWM error: attempted to create a ");
    WLAtom_print(wob -> type[0]);
    wool_printf(" with null %s, using 1 instead\n", explanation);
    stop_if_in_dbx();
}

/* absolute position (% screen) of current wob
 */

WOOL_OBJECT
wool_wob_x_get()
{
    Wob wob = TargetWob;
    int x = 0;

    while (wob -> type != ScreenClass) {
	x += wob -> box.x;
	if ((wob -> status & TopLevelXWindowStatus) || ! wob -> parent)
	    break;
	wob = wob -> parent;
	x += wob -> box.borderwidth;
    }
    return (WOOL_OBJECT) WLNumber_make(x);
}

WOOL_OBJECT
wool_wob_y_get()
{
    Wob wob = TargetWob;
    int y = 0;

    while (wob -> type != ScreenClass) {
	y += wob -> box.y;
	if ((wob -> status & TopLevelXWindowStatus) || ! wob -> parent)
	    break;
	wob = wob -> parent;
	y += wob -> box.borderwidth;
    }
    return (WOOL_OBJECT) WLNumber_make(y);
}
