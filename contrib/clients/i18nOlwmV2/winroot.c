/*
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) winroot.c 50.6 90/12/12 Crucible";

#ifdef OW_I18N
#include "i18n.h"
#endif
#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#include <olgx/olgx.h>
#ifdef OW_I18N
#include "ollocale.h"
#endif
#include "mem.h"
#include "olwm.h"
#include "win.h"
#include "menu.h"
#include "globals.h"
#include "group.h"

/***************************************************************************
* global data
***************************************************************************/

extern Menu *RootMenu;
extern Window	NoFocusWin;
extern Time	SelectionTime;

/***************************************************************************
* private data
***************************************************************************/

static int lastButton;
static XEvent buttonPressEvent;
static ClassRoot classRoot;

/***************************************************************************
* private functions
***************************************************************************/

static void
redistributeKeystroke( dpy, e )
Display *dpy;
XEvent *e;
{
    XKeyEvent *ke = (XKeyEvent *) e;
    Window childwin, dstwin, srcwin;
    int srcx, srcy, dstx, dsty;
    static Bool pressreceived = False;
    static XKeyEvent pressevent;

    if ( ke->type == KeyPress ) {
	if ( pressreceived == False ) {
	    srcwin = ke->window;
	    dstwin = ke->window;
	    srcx = ke->x;
	    srcy = ke->y;
	    while (1) {
		XTranslateCoordinates( dpy, srcwin, dstwin, srcx, srcy,
		    &dstx, &dsty, &childwin );
		if ( childwin == None )
		    break;
		srcx = dstx;
		srcy = dsty;
		srcwin = dstwin;
		dstwin = childwin;
	    }
	    pressevent = *ke;
	    pressevent.x = dstx;
	    pressevent.y = dsty;
	    pressevent.window = dstwin;
	    pressevent.subwindow = None;
	    pressreceived = True;
	}
    } else {
	if ( pressreceived == True ) {
#ifdef notdef
	    XUngrabKeyboard( dpy, pressevent.time );
#endif /* notdef */
	    XSendEvent( dpy, pressevent.window, True, 
		KeyPressMask, &pressevent );
	    XTranslateCoordinates( dpy, ke->window, pressevent.window,
		ke->x, ke->y, &dstx, &dsty, &childwin );
	    ke->x = dstx;
	    ke->y = dsty;
	    ke->window = pressevent.window;
	    ke->subwindow = None;
	    XSendEvent( dpy, pressevent.window, True, KeyPressMask, ke );
	}
	pressreceived = False;
    }
}


static Bool
matchKeystrokeToSpec(event,spec)
    XEvent *event;
    KeySpec *spec;
{
    return (spec->keycode == event->xkey.keycode &&
	    (spec->modmask == AnyModifier ||
	     spec->modmask == event->xkey.state));
}


static void
rootKeystroke(dpy,pEvent)
Display	*dpy;
XEvent *pEvent;
{
    Window		child;
    WinGeneric		*childInfo;
    Client		*childClient;
    Bool		samescreen;
    int			destX, destY;

    samescreen = XTranslateCoordinates(dpy,
	DefaultRootWindow(dpy), DefaultRootWindow(dpy),
	pEvent->xkey.x, pEvent->xkey.y, &destX, &destY, &child );

    if (!samescreen)
	return;

    if (pEvent->type == KeyPress && child != None &&
	matchKeystrokeToSpec(pEvent, &(GRV.FrontKey))) {
	XWindowChanges xwc;
	xwc.stack_mode = Opposite;
	XConfigureWindow( dpy, child, CWStackMode, &xwc );
    } else if (matchKeystrokeToSpec(pEvent, &(GRV.HelpKey))) {
	redistributeKeystroke( dpy, pEvent );
    } else if (pEvent->type == KeyPress && child != None &&
	       matchKeystrokeToSpec(pEvent, &(GRV.OpenKey))) {
	childInfo = WIGetInfo(child);
	if ( childInfo ) {
	    childClient = childInfo->core.client;
	    if (childClient != NULL)
	    {
	        if ( childClient->wmState == IconicState ) {
		    StateIconNorm(childClient);
	        } else {
		    /* check for base vs. popup window */
	    	    if (childClient->groupmask == GROUP_DEPENDENT) {
		        /*
		         * REMIND: passing (Menu *)NULL is bogus.
		         * Also, should pass DA_THIS instead of 0.
		         */
		        DismissAction(dpy,childInfo,(Menu *)NULL,0,False);
		    } else {
		        StateNormIcon(childClient);
		    }
    	        }
	    }
	}
    } else if (pEvent->type == KeyPress &&
	       matchKeystrokeToSpec(pEvent, &(GRV.ColorLockKey))) {
	InstallPointerColormap(dpy, pEvent->xkey.root, pEvent->xkey.x_root,
			       pEvent->xkey.y_root, True);
    } else if (pEvent->type == KeyPress &&
	       matchKeystrokeToSpec(pEvent, &(GRV.ColorUnlockKey))) {
	UnlockColormap(dpy);
    }
}


static void
rootProperty( dpy, pEvent )
Display	*dpy;
XEvent	*pEvent;
{
	Atom		actualType;
	int		actualFormat;
	unsigned long	nitems, remain;
	char 		*resourceString;
	XrmDatabase	serverDB;
#ifdef OW_I18N
	extern XrmDatabase	applicationLocaleDB;
#endif

	/* make sure that the property was the one we care about and
	 * changed (as opposed to deleted)
	 */
	if ( (pEvent->xproperty.atom != XA_RESOURCE_MANAGER) 
	     || (pEvent->xproperty.state != PropertyNewValue) )
		return;

	resourceString = GetWindowProperty(dpy, RootWindow(dpy, DefaultScreen(dpy)),
		XA_RESOURCE_MANAGER, 0L, 100000000L, 
		/* REMIND what should the length really be ? */
		XA_STRING, 0, &nitems, &remain);
	if (resourceString != NULL)
        {
		serverDB = XrmGetStringDatabase( resourceString );
#ifdef OW_I18N
		/*
		 * We need the two step, see GetDefaults() for more
		 * detail.
		 */
		UpdateDBValues( dpy, serverDB, True );
		/*
		 * In case of we pickup the new locale and that is
		 * different from initial locale (other than the when
		 * pickup the ApplicationLocaleDefaults in
		 * GetDefaults().  So, ApplicationLocaleDefaults is
		 * not in the olwmDB.
		 */
		parseApplicationLocaleDefaults();
		(void) XrmMergeDatabases( serverDB, &applicationLocaleDB );
		UpdateDBValues( dpy, applicationLocaleDB, False );
#else
		UpdateDBValues( dpy, serverDB );
#endif
		XFree( resourceString );
        }
}


/* 
 * eventEnterNotify - the pointer has entered the root window
 */
static int
eventEnterNotify(dpy, pEvent, winInfo)
Display	*dpy;
XEvent	*pEvent;
WinRoot	*winInfo;
{
    ColorWindowCrossing(dpy, pEvent, winInfo);
}

/* 
 * eventConfigureRequest - a client wants to change configuration
 */
static int
eventConfigureRequest(dpy, pEvent, winInfo)
Display	*dpy;
XEvent	*pEvent;
WinRoot	*winInfo;
{
#define ConfEvent	(pEvent->xconfigurerequest)
	if ((winInfo = WIGetInfo(ConfEvent.window)) == NULL)
	{
		/* we don't know about this window, so let it go */
		ClientConfigure(NULL,NULL,pEvent);
	}
	else /* OBSOLETE: if (ConfEvent.value_mask & (CWX | CWY | CWWidth | CWHeight)) */
	{
		/* configure the window and its frame */
		ClientConfigure(winInfo->core.client,winInfo,pEvent);
	}
	/* REMIND doesn't handle stacking or border width yet */
}

/* 
 * eventMapRequest - a new client is mapping
 */
static int
eventMapRequest(dpy, pEvent, winInfo)
Display	*dpy;
XEvent	*pEvent;
WinRoot	*winInfo;
{
#define MapEvent	(pEvent->xmap)
	StateNew(dpy, MapEvent.window, False, NULL);
}

/* 
 * eventMotionNotify - the pointer is moving
 */
static int
eventMotionNotify(dpy, pEvent, winInfo)
Display	*dpy;
XEvent	*pEvent;
WinRoot	*winInfo;
{
	short			boxX, boxY, boxW, boxH;
	clientInBoxClosure	cibclosure;
	Time			timestamp;

	if (!pEvent->xmotion.same_screen)
	    return;

	/* If the user hasn't moved more than the threshold
	 * amount, break out of here.  REMIND  Also, if we get a 
	 * MotionNotify event with no buttons down, we ignore it.
	 * Ideally this shouldn't happen, but some areas of the code
	 * still leave the pointer grabbed even after all the buttons
	 * have gone up.
	 */
	if ((ABS(pEvent->xmotion.x - buttonPressEvent.xbutton.x) < 
	     GRV.MoveThreshold) &&
	    (ABS(pEvent->xmotion.y - buttonPressEvent.xbutton.y) < 
	     GRV.MoveThreshold))
	    return;
	if (pEvent->xmotion.state == 0)
	   return;

	switch(lastButton)
	{
	case MB_SELECT:	
		ClearSelections(dpy);

		/* need to trace a bounding box */
		if (TraceBoundingBox(dpy, &buttonPressEvent, 
			&boxX, &boxY, &boxW, &boxH, &timestamp))
		{
		    /* select all frames or icons in the box */
		    cibclosure.dpy = dpy;
		    cibclosure.func = AddSelection;
		    cibclosure.bx = boxX;
		    cibclosure.by = boxY;
		    cibclosure.bw = boxW;
		    cibclosure.bh = boxH;
		    cibclosure.timestamp = timestamp;
		    ListApply(ActiveClientList, ClientInBox, &cibclosure);
		}
		break;

	case MB_ADJUST:	
		/* bounding box with toggles */
		if (TraceBoundingBox(dpy, &buttonPressEvent, 
			&boxX, &boxY, &boxW, &boxH, &timestamp))
		{
		    cibclosure.dpy = dpy;
		    cibclosure.func = ToggleSelection;
		    cibclosure.bx = boxX;
		    cibclosure.by = boxY;
		    cibclosure.bw = boxW;
		    cibclosure.bh = boxH;
		    cibclosure.timestamp = timestamp;
		    ListApply(ActiveClientList, ClientInBox, &cibclosure);
		}
		break;
	}
}

/* 
 * eventButtonRelease - handle a click
 */
static int
eventButtonRelease(dpy, pEvent, winInfo)
Display	*dpy;
XEvent	*pEvent;
WinRoot	*winInfo;
{
	/* This only happens if we did NOT get a motion notify
	 * after the last button press. 
	 */
	if (MouseButton(dpy, pEvent) == MB_SELECT)
	{
		/* Clear the selection list. */
		ClearSelections(dpy);

		/* Set the input focus to the root window. */
		XSetInputFocus(dpy, NoFocusWin, RevertToPointerRoot,
			       pEvent->xbutton.time);

		/* Tell the server that we are now the current
		 * selection owner.  This might be necessary if
		 * the user had selected some text in an xterm
		 * and wants to unselect it.  S/he does this by
		 * clicking in the root window.
		 */
		 XSetSelectionOwner(dpy, XA_PRIMARY,
				    RootWindow(dpy, DefaultScreen(dpy)),
			            pEvent->xbutton.time);
		 SelectionTime = pEvent->xbutton.time;
	}
}
		
/* 
 * eventButtonPress - record a button press
 */
static int
eventButtonPress(dpy, pEvent, winInfo)
Display	*dpy;
XEvent	*pEvent;
WinRoot	*winInfo;
{
	buttonPressEvent = *pEvent;

	/* set lastButton */
	if ((lastButton = MouseButton(dpy, pEvent)) == MB_MENU)
	{
		/* bring up menu for root */
		MenuShow(dpy, winInfo, RootMenu, pEvent);
	}
}

/* 
 * eventSelectionNotify - someone else has asked for the selection value
 */
static int
eventSelectionNotify(dpy, pEvent, winInfo)
Display	*dpy;
XEvent	*pEvent;
WinRoot	*winInfo;
{
	/* We get this event from the owner of the selection
	 * if we had made a XConvertSelection call.  At the
	 * moment we never make a XConvertSelction call
	 * so we don't have to worry about a SelectionNotify
	 * event.  An XCovertSelection call is used to get
	 * the current selection.
	 */
}

/* 
 * eventSelectionRequest - someone else has asked for the selection value
 */
static int
eventSelectionRequest(dpy, pEvent, winInfo)
Display	*dpy;
XEvent	*pEvent;
WinRoot	*winInfo;
{
		SelectionResponse((XSelectionRequestEvent *)pEvent);
}

/* 
 * eventSelectionClear - someone else has asked to be the selection owner
 */
static int
eventSelectionClear(dpy, pEvent, winInfo)
Display	*dpy;
XEvent	*pEvent;
WinRoot	*winInfo;
{
	/*
	 * Someone else has asked to be the selection owner.
	 * Clear our list of selected windows.
	 */
	ClearSelections(dpy);
}

/* 
 * eventKeyPressRelease - a keystroke has happened in the root window
 */
static int
eventKeyPressRelease(dpy, pEvent, winInfo)
Display	*dpy;
XEvent	*pEvent;
WinRoot	*winInfo;
{
   rootKeystroke( dpy, pEvent );
}

/* 
 * eventPropertyNotify - a root property has changed
 */
static int
eventPropertyNotify(dpy, pEvent, winInfo)
Display	*dpy;
XEvent	*pEvent;
WinRoot	*winInfo;
{
    rootProperty( dpy, pEvent );
}

/* 
 * eventUnmapNotify - an unreparented pane is going away
 */
static int
eventUnmapNotify(dpy, pEvent, winInfo)
Display	*dpy;
XEvent	*pEvent;
WinRoot	*winInfo;
{
    WinGeneric *wg;
    Client *cli;

    wg = WIGetInfo(pEvent->xunmap.window);
    if (wg != NULL)
    {
	StateWithdrawn(wg->core.client);
    }
}


/*
 * drawRoot -- draw the root window
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static int
drawRoot(dpy, winInfo)
Display	*dpy;
WinGeneric *winInfo;
{
}

/*
 * DestroyRoot -- destroy the root window resources and free any allocated
 *	data.
 */
static int
destroyRoot(dpy, winInfo)
Display	*dpy;
WinGeneric *winInfo;
{
	/* free our data and throw away window */
	WIUninstallInfo(winInfo->core.self);
	MemFree(winInfo);
}

/***************************************************************************
* global functions
***************************************************************************/

/*
 * MakeRoot  -- create the root window. Return a WinGeneric structure.
 */
WinGeneric *
MakeRoot(dpy, par)
Display	*dpy;
WinGeneric *par;	/* ignored */
{
	XWindowAttributes attr;
	WinRoot *w;
	Window win;

	win = RootWindow(dpy, DefaultScreen(dpy));

	if (XGetWindowAttributes(dpy, win, &attr) == 0) {
#ifdef OW_I18N
	    ErrorGeneral(gettext("could not get attributes of root window"));
#else
	    ErrorGeneral("could not get attributes of root window");
#endif
	    /*NOTREACHED*/
	}

	/* create the associated structure */
	w = MemNew(WinRoot);
	w->core.self = win;
	w->class = &classRoot;
	w->core.kind = WIN_ROOT;
	w->core.parent = NULL;
	w->core.children = NULL;
	w->core.client = NULL;
	w->core.x = 0;
	w->core.y = 0;
	w->core.width = attr.width;
	w->core.height = attr.height;
	w->core.dirtyconfig = False;
	w->core.colormap = attr.colormap;
	w->core.exposures = NULL;

	/* register the window */
	WIInstallInfo(w);

	return w;
}

void
RootInit(dpy)
Display *dpy;
{
	classRoot.core.kind = WIN_ROOT;
	classRoot.core.xevents[ConfigureRequest] = eventConfigureRequest;
	classRoot.core.xevents[EnterNotify] = eventEnterNotify;
	classRoot.core.xevents[MapRequest] = eventMapRequest;
	classRoot.core.xevents[MotionNotify] = eventMotionNotify;
	classRoot.core.xevents[ButtonRelease] = eventButtonRelease;
	classRoot.core.xevents[ButtonPress] = eventButtonPress;
	classRoot.core.xevents[SelectionNotify] = eventSelectionNotify;
	classRoot.core.xevents[SelectionRequest] = eventSelectionRequest;
	classRoot.core.xevents[SelectionClear] = eventSelectionClear;
	classRoot.core.xevents[KeyPress] = eventKeyPressRelease;
	classRoot.core.xevents[KeyRelease] = eventKeyPressRelease;
	classRoot.core.xevents[PropertyNotify] = eventPropertyNotify;
	classRoot.core.xevents[UnmapNotify] = eventUnmapNotify;
	classRoot.core.focusfunc = NULL;
	classRoot.core.drawfunc = NULL;
	classRoot.core.destroyfunc = NULL;
	classRoot.core.selectfunc = NULL;
	classRoot.core.newconfigfunc = NULL;
	classRoot.core.newposfunc = NULL;
	classRoot.core.setconfigfunc = NULL;
	classRoot.core.createcallback = NULL;
	classRoot.core.heightfunc = NULL;
	classRoot.core.widthfunc = NULL;
}
