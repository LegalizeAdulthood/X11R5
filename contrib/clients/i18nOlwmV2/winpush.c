/*
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) winpush.c 50.4 90/12/12 Crucible";

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

/***************************************************************************
* global data
***************************************************************************/

extern Atom AtomDeleteWindow;
extern Atom AtomPushpinState;
extern Graphics_info *olgx_gisnormal;
extern Graphics_info *olgx_gisrevpin;
extern GC DrawBusyGC, DrawBackgroundGC;

/***************************************************************************
* private data
***************************************************************************/

static ClassPushPin classPushPin;
static Bool     pushpinStateAfterPress;  /* State of the pushpin
					  * after the user pressed
				   	  * the mouse button. */

/***************************************************************************
* private functions
***************************************************************************/

/* locallyChangePushPinState -- temporarily change the pushpin state,
 * 	while tracking a button press over the pin.  The permanent change
 *	will be made later.
 */
static void
locallyChangePushPinState(dpy,winInfo,newState)
Display *dpy;
WinPushPin *winInfo;
Bool newState;
{
        if (winInfo->pushpinin != newState)
        {
                winInfo->pushpinin = newState;
                (WinFunc(winInfo,core.drawfunc))(dpy, winInfo);
        }
}

/* changePushPinState -- permanently change the push pin state */
static void
changePushPinState(dpy, event, winInfo)
Display *dpy;
XEvent  *event;
WinPushPin *winInfo;
{
	Client		*cli = winInfo->core.client;

        (WinFunc(winInfo,core.drawfunc))(dpy, winInfo);

        /* Tell the client that the state of its push-pin
         * has changed.
         */
        XChangeProperty(dpy, PANEWINOFCLIENT(cli),
                        AtomPushpinState,
                        XA_INTEGER, 32,
                        PropModeReplace,
                        (unsigned char *)&(winInfo->pushpinin), 1);

        /* If the client has WM_DELETE_WINDOW in
         * its WM_PROTOCOLS, then we should send
         * a WM_DELETE_WINDOW client message.
         */
        if (!winInfo->pushpinin)
        {
		ClientKill(winInfo->core.client,False);
        }  
#ifdef NOTDEF
        /* Else just unmap the window. */
        /* REMIND what should we do here? (if client isn't in delete protocol)
        else
                XUnmapWindow(dpy, clientEvent.xclient.window);
        */
#endif
}  

/* 
 * eventButtonPress - handle button press events on the pushpin window 
 */
static int
eventButtonPress(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinPushPin	*winInfo;
{
	if (MouseButton(dpy,event) == MB_SELECT)
	{
            locallyChangePushPinState(dpy, winInfo, !(winInfo->pushpinin));
            pushpinStateAfterPress = winInfo->pushpinin;
	}
}

/* 
 * eventButtonRelease - handle button release events on the pushpin window 
 */
static int
eventButtonRelease(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinPushPin	*winInfo;
{
	/*
	 * If the user pressed the SELECT button, and the pin state is the 
	 * same as it was just after he pressed the button over the pin, that 
	 * means that we really are changing the pin state.  Otherwise, the 
	 * user has previewed a pin state change but has backed off, so we do 
	 * nothing.
	 */
	if (MouseButton(dpy,event) == MB_SELECT &&
	    pushpinStateAfterPress == winInfo->pushpinin)
	{
		changePushPinState(dpy,event,winInfo);
	}
}

/* 
 * eventMotionNotify - handle pointer moves
 */
static int
eventMotionNotify(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinPushPin	*winInfo;
{
	Bool fInWindow;

	if (!event->xmotion.same_screen)
		return;

        /* When the user moves the cursor off the pushpin
         * while s/he has the button down we should pull
         * the button out.  If the user moves back onto the
         * pushpin put the pin back in.  So,
         *
         * if (cursor is off the pushpin) and (pin is in)
         *                  OR
         *    (cursor is on the pushpin) and (pin is out)
         * then
         *      change the state of the pushpin.
         */
        fInWindow = !((event->xmotion.x < 0) ||
             (event->xmotion.y < 0) ||
             (event->xmotion.x >= PushPinOut_Width(olgx_gisnormal)) ||
             (event->xmotion.y >= PushPinOut_Height(olgx_gisnormal)));
        locallyChangePushPinState(dpy, winInfo,
                fInWindow?pushpinStateAfterPress:!pushpinStateAfterPress);

        /* REMIND Maybe we should do this by getting LeaveNotify events
         * and seeing if MB_SELECT is pressed.  If it is, then we
         * know the user left the pushpin with the button pressed???
         */
}


/*
 * drawPushPin -- draw the pushpin window
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static int
drawPushPin(dpy, winInfo)
Display	*dpy;
WinPushPin *winInfo;
{
	Client *cli = winInfo->core.client;

        /* If the titlebar is in reverse video we need to
         * draw the pushpin in reverse video also.
         */
	if (GRV.F3dUsed) {
		/*
		 * REMIND
		 * We need to erase the background here to BG2.  We can't use
		 * OLGX_ERASE because olgx erases only in BG1.  So, we draw an 
		 * filled, invoked box that is just larger than the pin 
		 * window, so that the border doesn't show.
		 */
		olgx_draw_box(olgx_gisnormal, winInfo->core.self, -1, -1,
			      winInfo->core.width+2,
			      winInfo->core.height+2,
                              ((cli->isFocus) && (!GRV.FocusFollowsMouse))?
			          OLGX_INVOKED : OLGX_NORMAL, 
			      True);
        } else {
		XFillRectangle(dpy, winInfo->core.self, DrawBackgroundGC, 
			0, 0, winInfo->core.width, winInfo->core.height);
        }
	if (winInfo->core.client->isBusy)
	{
	    XFillRectangle(dpy, winInfo->core.self, DrawBusyGC,
		0, 0, winInfo->core.width, winInfo->core.height);
	}
	olgx_draw_pushpin(olgx_gisnormal, winInfo->core.self, 0, 0,
              	  ((winInfo->pushpinin) ? OLGX_PUSHPIN_IN : OLGX_PUSHPIN_OUT));
}


/*
 * DestroyPushPin -- destroy the pushpin window resources and free any allocated
 *	data.
 */
static int
destroyPushPin(dpy, winInfo)
Display	*dpy;
WinPushPin *winInfo;
{
	/* free our data and throw away window */
	XDestroyWindow(dpy, winInfo->core.self);
	WIUninstallInfo(winInfo->core.self);
	MemFree(winInfo);
}

/* 
 * focusselectPushPin - the focus or selection state has changed
 */
static int
focusselectPushPin(dpy, winInfo, selected)
Display *dpy;
WinPushPin *winInfo;
Bool selected;
{
        (WinFunc(winInfo,core.drawfunc))(dpy, winInfo);
}

/*
 * heightfuncPushPin - recomputes the correct height of the window
 */
static int 
heightfuncPushPin(win, pxcre)
WinPushPin *win;
XConfigureRequestEvent *pxcre;
{
	return PushPinOut_Width(olgx_gisnormal);
}

/*
 * widthfuncPushPin - recomputes the correct width of the window
 */
static int 
widthfuncPushPin(win, pxcre)
WinPushPin *win;
XConfigureRequestEvent *pxcre;
{
	return PushPinOut_Height(olgx_gisnormal);
}

/***************************************************************************
* global functions
***************************************************************************/

/*
 * MakePushPin  -- create the pushpin window. Return a WinGeneric structure.
 */
WinPushPin *
MakePushPin(dpy, par, pane, x, y)
Display	*dpy;
WinGeneric *par;
Window pane;
int x,y;
{
	WinPushPin *w;
	Window win;
        unsigned long valuemask;
        XSetWindowAttributes attributes;

        attributes.event_mask = Button1MotionMask | ButtonReleaseMask | 
		ButtonPressMask | ExposureMask;
        attributes.win_gravity = NorthWestGravity;
        valuemask = CWEventMask | CWWinGravity;

        win = XCreateWindow(dpy, par->core.self,
                        x, y,
			PushPinOut_Width(olgx_gisnormal), 
			PushPinOut_Height(olgx_gisnormal),
                        0,
                        DefaultDepth(dpy, DefaultScreen(dpy)),
                        CopyFromParent,
                        DefaultVisual(dpy, DefaultScreen(dpy)),
                        valuemask,
                        &attributes);

	/* create the associated structure */
	w = MemNew(WinPushPin);
	w->core.self = win;
	w->class = &classPushPin;
	w->core.kind = WIN_PUSHPIN;
	WinAddChild(par,w);
	w->core.children = NULL;
	w->core.client = par->core.client;
	w->core.x = x;	
	w->core.y = y;
	w->core.width = PushPinOut_Width(olgx_gisnormal);
	w->core.height = PushPinOut_Height(olgx_gisnormal);
	w->core.dirtyconfig = CWX | CWY | CWWidth | CWHeight;
	w->core.exposures = NULL;

        /* Determine initial state of push pin. */
        w->pushpinin =  (par->core.client->wmDecors->pushpin_initial_state == PIN_IN);

        /* Register the push-pin state. */
        XChangeProperty(dpy, pane,
                        AtomPushpinState,
                        XA_INTEGER, 32,
                        PropModeReplace,
                        (unsigned char *)&(w->pushpinin), 1);

	/* register the window */
	WIInstallInfo(w);

        XMapRaised(dpy, win);

	return w;
}

void
PushPinInit(dpy)
Display *dpy;
{
	classPushPin.core.kind = WIN_PUSHPIN;
	classPushPin.core.xevents[Expose] = WinEventExpose;
	classPushPin.core.xevents[ButtonPress] = eventButtonPress;
	classPushPin.core.xevents[ButtonRelease] = eventButtonRelease;
	classPushPin.core.xevents[MotionNotify] = eventMotionNotify;
	classPushPin.core.focusfunc = focusselectPushPin;
	classPushPin.core.drawfunc = drawPushPin;
	classPushPin.core.destroyfunc = destroyPushPin;
	classPushPin.core.selectfunc = focusselectPushPin;
	classPushPin.core.newconfigfunc = WinNewConfigFunc;
	classPushPin.core.newposfunc = WinNewPosFunc;
	classPushPin.core.setconfigfunc = WinSetConfigFunc;
	classPushPin.core.createcallback = NULL;
	classPushPin.core.heightfunc = heightfuncPushPin;
	classPushPin.core.widthfunc = widthfuncPushPin;
}
