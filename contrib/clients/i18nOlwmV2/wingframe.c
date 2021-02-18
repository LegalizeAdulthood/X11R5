/*
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

/*
 * wingframe.c -- generic frame window routines
 */

static	char	sccsid[] = "@(#) wingframe.c 50.4 90/12/12  Crucible";
#ifdef OW_I18N
#include "i18n.h"
#endif
#include <errno.h>
#include <stdio.h>
#include <string.h>
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
#include "menu.h"
#include "globals.h"

/***************************************************************************
* global data
***************************************************************************/

extern Graphics_info	*olgx_gisnormal;
extern Atom		AtomOlwmAutoRaise;
extern Bool		FocusIndeterminate;

/***************************************************************************
* private data
***************************************************************************/

/* We sometimes get MotionNotify events when the button
 * is not pressed (even though we set Button1MotionMask).
 * Is this a bug in the X11R3 server?  To handle this
 * we need to record Button1 having been pressed.
 * When the MB_SELECT button, (Button1), has been pressed
 * hadSelect is set to true.
 * REMIND globalise this!
 */
static  Bool            hadSelect = False;
static  Time            timeOfLastButPress = 0;
static  int             buttonPressX;
static  int             buttonPressY;
static  XButtonEvent    lastSelectRelease;
static  XButtonEvent    lastSelectPress;
static  Time            lastDoubleClickTime;

/***************************************************************************
* forward-declared functions
***************************************************************************/

/***************************************************************************
* global functions
***************************************************************************/

/*
 * GFrameSelect -- handle selection state change
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
int
GFrameSelect(dpy, winInfo, selected)
Display	*dpy;
WinGeneric *winInfo;
Bool selected;
{
	(WinFunc(winInfo,core.drawfunc))(dpy, winInfo);
}

/*
 * GFrameFocus -- handle focus changes
 *
 * If we're in auto-raise mode, raise the window.  But if we're in focus-
 * follows-mouse, we query the pointer to make sure we're still in the same 
 * window before we do this raise.  This is to avoid restacking loops.
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
int
GFrameFocus(dpy, winInfo, focus)
Display	*dpy;
WinGeneric *winInfo;
{
	Bool samescreen;
	Window root, child;
	int rootx, rooty, winx, winy, state;

	if (focus)
	    FocusIndeterminate = False;

	if (focus && GRV.AutoRaise) {
	    if (GRV.FocusFollowsMouse) {
		samescreen = XQueryPointer(dpy, DefaultRootWindow(dpy), &root,
		    &child, &rootx, &rooty, &winx, &winy, &state);
		if (samescreen && child == winInfo->core.self)
		    XRaiseWindow(dpy, winInfo->core.self);
	    } else {
		XRaiseWindow(dpy, winInfo->core.self);
	    }
	}

	(WinFunc(winInfo,core.drawfunc))(dpy, winInfo);
}

/*
 * GFrameSetConfigFunc -- change configuration of frame window
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
int
GFrameSetConfigFunc(dpy, win)
Display	*dpy;
WinGenericFrame *win;
{
        XWindowChanges xwc;

        if (win->core.dirtyconfig)
        {
                xwc.x = win->core.x;
                xwc.y = win->core.y;
                xwc.width = win->core.width;
                xwc.height = win->core.height;
		xwc.sibling = win->core.stack_sib;
		xwc.stack_mode = win->core.stack_mode;
		if ((win->core.dirtyconfig & CWSibling) && 
		    !(win->core.dirtyconfig &CWStackMode))
			win->core.dirtyconfig &= ~CWSibling;
                XConfigureWindow(dpy,win->core.self,win->core.dirtyconfig,&xwc);
		win->core.dirtyconfig &= ~(CWX|CWY|CWWidth|CWHeight|CWSibling|CWStackMode);
        }
}

/* GFrameSetStack -- set the frame's stacking position.   Does not initiate
 *	a configuration change.
 */
void
GFrameSetStack(win, mask, mode, sib)
WinGenericFrame *win;
int mask;
int mode;
Window sib;
{
	WinGeneric *wsib;

	if ((mask & CWSibling) && (mask & CWStackMode))
	{
		wsib = WIGetInfo(sib);
		if (wsib != NULL)
		{
			win->core.stack_sib = wsib->core.client->framewin->core.self;
			win->core.dirtyconfig |= CWSibling;
		}
	}
	if (mask & CWStackMode)
	{
		win->core.stack_mode = mode;
		win->core.dirtyconfig |= CWStackMode;
	}
}

/* GFrameSetConfig - set the frame's size and position -- generally used in
 * resizing or moving the window.  We position the frame and resize the pane.
 * If the pane has refused resizing we skip that part.
 */
void
GFrameSetConfig(win,x,y,w,h)
WinGenericFrame *win;
int x,y,w,h;
{
        WinPane *pane = (WinPane *)win->fcore.panewin;

        (WinFunc(win,core.newposfunc))(win,x,y);
	if (pane != NULL)
	{
            if (WinFunc(pane,pcore.setsizefunc) != NULL)
                (WinFunc(pane,pcore.setsizefunc))(pane,
                    w-(WinFunc(win,fcore.widthleft))(win)-(WinFunc(win,fcore.widthright))(win),
                    h-(WinFunc(win,fcore.heighttop))(win)-(WinFunc(win,fcore.heightbottom))(win));
            WinCallConfig(win->core.client->dpy, pane, NULL);
	}
	else
	{
            WinCallConfig(win->core.client->dpy, win, NULL);
	}
}


/***************************************************************************
* global event functions
***************************************************************************/

/*
 * GFrameEventButtonRelease -- a button has been released
 *
 */
int
GFrameEventButtonRelease(dpy, event, frameInfo)
Display *dpy;
XEvent  *event;
WinGenericFrame *frameInfo;
{
        WinGenericPane	*winPane = (WinGenericPane*)frameInfo->fcore.panewin;
        Client *cli = frameInfo->core.client;

#define bevent  (event->xbutton)
        switch (MouseButton(dpy, event))
        {
	case MB_ADJUST:
                if (WinFunc(frameInfo,fcore.adjustClick) != NULL)
                {
		    (WinFunc(frameInfo,fcore.adjustClick))(dpy,event,frameInfo);
                }
		break;

        case MB_SELECT:
                ClearSelections(dpy);
                AddSelection(cli, event->xbutton.time);
                XRaiseWindow(dpy, frameInfo->core.self);
                hadSelect = False;

                if (WinFunc(frameInfo,fcore.selectClick) != NULL)
                {
		    (WinFunc(frameInfo,fcore.selectClick))(dpy,event,frameInfo);
                }

                /* If the click happened in the decoration windows,
                 * (i.e. not in the pane), check for a double click.
                 * Check last click time to see if we are in double
                 * click land.  Also check that the user hasn't just
                 * hit the button 3 times very rapidly.  This would
                 * cause a cycle of the window changing size.
		 * REMIND this is more nasty than it should be because
		 * not all frames are yet required to have panes.
		 * Once all panes have frames the subwindow test gets
		 * easier.
                 */
                if ((WinFunc(frameInfo,fcore.selectDoubleClick) != NULL) &&
		    ((winPane == NULL) || (bevent.subwindow != winPane->core.self)) &&
                   ((bevent.time-lastSelectRelease.time) <= GRV.DoubleClickTime) &&
                   ((bevent.time-lastDoubleClickTime) > GRV.DoubleClickTime))
                {
                    /* we have a double click */
		    if (WinFunc(frameInfo,fcore.selectDoubleClick) != NULL)
		    {
			(WinFunc(frameInfo,fcore.selectDoubleClick))(dpy,event,frameInfo);
		    }

                    lastDoubleClickTime = bevent.time;
                 }
                lastSelectRelease = event->xbutton;
                break;  /* out of MB_SELECT case */
        }
}

/*
 * GFrameEventMotionNotify -- a button is down and the pointer is moving
 */
int
GFrameEventMotionNotify(dpy, event, frameInfo)
Display *dpy;
XEvent  *event;
WinGenericFrame *frameInfo;
{
        /* We get this only after a Select press */
        if (hadSelect == False) /* watch for erroneous motions */
        {
                return;
        }

	if (!event->xmotion.same_screen)
		return;

        /* See if we have moved more than the threshold amount. */
        if ((ABS(event->xmotion.x - buttonPressX) < GRV.MoveThreshold) &&
            (ABS(event->xmotion.y - buttonPressY) < GRV.MoveThreshold))
                return;

	(WinFunc(frameInfo,fcore.selectDrag))(dpy, event, frameInfo, &lastSelectPress);

        /*
         * UserMoveWindows() will grab the pointer and handle events
         * using an interposer, so we can clear the hadSelect flag.
         */
        hadSelect = False;
}

/*
 * GFrameEventButtonPress -- a mouse button has gone down.
 */
/* Really we should not be doing anything on the
 * ButtonPress event.  We should wait for the
 * ButtonRelease before deciding what to do.
 */
int
GFrameEventButtonPress(dpy, event, frameInfo)
Display *dpy;
XEvent  *event;
WinGenericFrame *frameInfo;
{
        WinPane         *winPane = (WinPane*)frameInfo->fcore.panewin;
        Window          panewindow = winPane->core.self;
        Client *cli = frameInfo->core.client;

        switch (MouseButton(dpy, event))
        {
        case MB_MENU:
	    if (WinFunc(frameInfo,fcore.menuPress) != NULL)
		(WinFunc(frameInfo,fcore.menuPress))(dpy, event, frameInfo);
            break;

        case MB_SELECT:
                /* Save the location where the button went down so we
                 * can see if the user moves the mouse more than
                 * GRV.MoveThreshold, and wants to move the window.
                 */
                buttonPressX = event->xbutton.x;
                buttonPressY = event->xbutton.y;

                if (!GRV.FocusFollowsMouse && 
		    (WinFunc(frameInfo,fcore.selectPress) != NULL))
                {
                /* It is possible for us to replay the event and
                 * have the window, (decoration window, e.g. the
                 * resize corner, the titlebar), in which the
                 * button press happened to ignore it.
                 * In this case we would get the event
                 * again.  For example, the user could button press
                 * in the title bar, (which doesn't select this event),
                 * and have this EventFrame routine get the same event
                 * twice.  So, we check that the time stamp of this
                 * button press is different than the last.
                 */
                        if (event->xbutton.time == timeOfLastButPress)
                                /* We already dealt with this event. */
                                break;
                        timeOfLastButPress = event->xbutton.time;
                        if (event->xbutton.subwindow == panewindow)
                        {
			    (WinFunc(frameInfo,fcore.selectPress))(dpy,event,frameInfo);
                        }

                        /* Let the button press through
                         * if we had grabbed it.
                         */
                        XAllowEvents(dpy, ReplayPointer, CurrentTime);
                }  /* End if not GRV.FocusFollowsMouse */

                lastSelectPress = event->xbutton;
                hadSelect = True;
                break;  /* Break case MB_SELECT */

        case MB_ADJUST:
	    if (!GRV.FocusFollowsMouse) {
		if (event->xbutton.time == timeOfLastButPress)
		    break;
		timeOfLastButPress = event->xbutton.time;
		if (event->xbutton.subwindow == panewindow
		    && WinFunc(frameInfo,fcore.adjustPress) != NULL) {
		    (WinFunc(frameInfo,fcore.adjustPress))
			(dpy,event,frameInfo);
		}
		XAllowEvents(dpy, ReplayPointer, CurrentTime);
		break;
	    }
        }  /*  End switch on button pressed */
}


