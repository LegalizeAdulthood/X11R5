/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) winnofoc.c 50.4 90/12/12 Crucible";

#ifdef OW_I18N
#include "i18n.h"
#endif
#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <olgx/olgx.h>
#ifdef OW_I18N
#include "ollocale.h"
#endif
#include "mem.h"
#include "olwm.h"
#include "win.h"
#include "globals.h"

extern unsigned int FindModifierMask();
extern Bool FocusIndeterminate;
extern Atom AtomProtocols;
extern Atom AtomTakeFocus;

/***************************************************************************
* global data
***************************************************************************/

Window NoFocusWin = NULL;

/***************************************************************************
* private data
***************************************************************************/

static ClassNoFocus classNoFocus;

/***************************************************************************
* private functions
***************************************************************************/

/*
 * DestroyNoFocus -- destroy the no-focus window and free any allocated
 *	data.
 */
static int
destroyNoFocus(dpy, winInfo)
Display	*dpy;
WinGeneric *winInfo;
{
	/* free our data and throw away window */
	XDestroyWindow(dpy, winInfo->core.self);
	WIUninstallInfo(winInfo->core.self);
	MemFree(winInfo);
}


/*
 * eventClientMessage - handle WM_TAKE_FOCUS messages.
 */
static int
eventClientMessage(dpy, event, winInfo)
    Display *dpy;
    XEvent *event;
    WinNoFocus *winInfo;
{
    if (FocusIndeterminate &&
	event->xclient.message_type == AtomProtocols &&
	event->xclient.data.l[0] == AtomTakeFocus) {
	    XSetInputFocus(dpy, NoFocusWin,
			   RevertToPointerRoot,
			   event->xclient.data.l[1]);
	    FocusIndeterminate = False;
    }
}


/***************************************************************************
* global functions
***************************************************************************/

/*
 * MakeNoFocus  -- create the no-focus windows to a window. Return a 
 * WinGeneric structure.
 */
WinGeneric *
MakeNoFocus(dpy, par)
Display	*dpy;
WinGeneric *par;
{
	XSetWindowAttributes attributes;
	WinNoFocus *w;

	/* create the window */
	attributes.event_mask = KeyPressMask;
	attributes.override_redirect = True;
	NoFocusWin = XCreateWindow(dpy, par->core.self,
			 -10, -10,
			 10, 10	,
			 0,
			 0,
			 InputOnly,
			 CopyFromParent,
			 CWEventMask | CWOverrideRedirect,
			 &attributes);
	XMapWindow(dpy, NoFocusWin);

	/* create the associated structure */
	w = MemNew(WinNoFocus);
	w->core.self = NoFocusWin;
	w->core.kind = WIN_NOFOCUS;
	w->class = &classNoFocus;
	w->core.parent = par;
	w->core.children = NULL;
	w->core.client = NULL;
	w->core.x = -10;
	w->core.y = -10;
	w->core.width = 10;
	w->core.height = 10;
	w->core.dirtyconfig = False;
	w->core.exposures = NULL;

	/* register the window */
	WIInstallInfo(w);

	return w;
}


void
NoFocusInit(dpy)
Display *dpy;
{
	classNoFocus.core.kind = WIN_NOFOCUS;
	classNoFocus.core.xevents[ButtonPress] = NoFocusEventBeep;
	classNoFocus.core.xevents[ButtonRelease] = NoFocusEventBeep;
	classNoFocus.core.xevents[KeyPress] = NoFocusEventBeep;
	classNoFocus.core.xevents[KeyRelease] = NoFocusEventBeep;
	classNoFocus.core.xevents[ClientMessage] = eventClientMessage;
	classNoFocus.core.focusfunc = NULL;
	classNoFocus.core.drawfunc = NULL;
	classNoFocus.core.destroyfunc = destroyNoFocus;
	classNoFocus.core.selectfunc = NULL;
	classNoFocus.core.newconfigfunc = NULL;
	classNoFocus.core.newposfunc = NULL;
	classNoFocus.core.setconfigfunc = NULL;
	classNoFocus.core.createcallback = NULL;
	classNoFocus.core.heightfunc = NULL;
	classNoFocus.core.widthfunc = NULL;
}

void
NoFocusTakeFocus(dpy,evtime)
Display *dpy;
Time evtime;
{
	XSetInputFocus(dpy, NoFocusWin, RevertToParent, evtime);
}

/*
 * NoFocusEventBeep -- beep on keyboard/mouse events for the no-focus window
 *	Also used by busy windows
 */
int
NoFocusEventBeep(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinGeneric *winInfo;
{
        XEvent dummy;

	if (FindModifierMask(event->xkey.keycode) != 0)
		return;

        XSync(dpy,0);
        /*
         * Discard pending keyboard and mouse events on this
         * window, and then beep once.
         */
        while (XCheckTypedWindowEvent(dpy,winInfo->core.self,
                                      KeyPress,&dummy))
            ;
        while (XCheckTypedWindowEvent(dpy,winInfo->core.self,
                                      KeyRelease,&dummy))
            ;
        while (XCheckTypedWindowEvent(dpy,winInfo->core.self,
                                      ButtonPress,&dummy))
            ;
        while (XCheckTypedWindowEvent(dpy,winInfo->core.self,
                                      ButtonRelease,&dummy))
            ;
	if (GRV.Beep == BeepAlways)
            XBell(dpy, 100);
}

