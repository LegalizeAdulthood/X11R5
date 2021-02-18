/*
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) winbutton.c 50.4 90/12/12 Crucible";
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

extern Graphics_info *olgx_gisnormal;
extern GC DrawSelectedGC;
extern GC DrawBackgroundGC;

/***************************************************************************
* private data
***************************************************************************/

#define in_windowmark(x,y) \
	( (x) >= 0 && (y) >= 0 && \
	  (x) <= Abbrev_MenuButton_Width(olgx_gisnormal) && \
	  (y) <= Abbrev_MenuButton_Height(olgx_gisnormal) \
	)
static Bool buttonActive = False;
static ClassButton classButton;

/***************************************************************************
* private functions
***************************************************************************/

/* 
 * eventButtonPress - handle button press events on the close button window.  
 */
static int
eventButtonPress(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinButton	*winInfo;
{
	Client *cli = winInfo->core.client;
	WinPaneFrame *winFrame = cli->framewin;

        olgx_draw_abbrev_button(olgx_gisnormal, winInfo->core.self, 
		0, 0, OLGX_INVOKED);
        switch (MouseButton(dpy, event))
        {
        case MB_ADJUST:
                return;

        case MB_MENU:
                if (winFrame->fcore.menu)
                        MenuShow(dpy, winFrame, winFrame->fcore.menu, event);
                olgx_draw_abbrev_button(olgx_gisnormal, 
			winInfo->core.self,
                       	0, 0, OLGX_NORMAL | OLGX_ERASE);
                return;

        }
        XGrabPointer(dpy, winInfo->core.self,
             False,
             (ButtonReleaseMask | ButtonPressMask | PointerMotionMask),
             GrabModeAsync, GrabModeAsync,
             None,
             GRV.MovePointer,
             CurrentTime);
        buttonActive = True;
}

/* 
 * eventButtonRelease - handle button release events on the close button window.  
 */
static int
eventButtonRelease(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinButton	*winInfo;
{
	Client *cli = winInfo->core.client;
	WinPaneFrame *winFrame = cli->framewin;
	int x,y;

        /* REMIND: why is this necessary?  The pointer should have
         * been re-grabbed with GrabModeAsync, thus deactivating
         * the grab upon the release of the button. */
        XUngrabPointer(dpy,event->xbutton.time);

        x = event->xbutton.x;
        y = event->xbutton.y;

        buttonActive = False;
        olgx_draw_abbrev_button(olgx_gisnormal, winInfo->core.self, 
			0, 0, OLGX_NORMAL | OLGX_ERASE);

        if (!in_windowmark(x,y))
        {
                return;
        }

        /* we want to do the default action of the menu for this
         * window.  This will either be Iconify if the window is
         * a base window, or dismiss if not */
        switch(cli->wmDecors->menu_type)
        {
                case MENU_FULL:
                        StateNormIcon(cli);
                        break;

                case MENU_NONE:
                case MENU_LIMITED:
                        /*
                         * Send a WM_DELETE_WINDOW event.
			 * REMIND change to use event time
                         */
			ClientKill(cli,False);
                        break;
        }
}

/* 
 * eventMotionNotify - handle motion notify events on the close button window.  
 */
static int
eventMotionNotify(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinButton	*winInfo;
{
	Client *cli = winInfo->core.client;
	WinPaneFrame *winFrame = cli->framewin;
	int x,y;

	if (!event->xmotion.same_screen)
		return;

        x = event->xmotion.x;
        y = event->xmotion.y;
        if ( buttonActive && !in_windowmark(x,y) ) {
            olgx_draw_abbrev_button(olgx_gisnormal, winInfo->core.self,
		0, 0, OLGX_NORMAL | OLGX_ERASE);
            buttonActive = False;
        } else if ( !buttonActive && in_windowmark(x,y) ) {
            olgx_draw_abbrev_button(olgx_gisnormal, winInfo->core.self,
	    	0, 0, OLGX_INVOKED);
            buttonActive = True;
        }
}

/*
 * drawButton -- draw the pushpin window
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static int
drawButton(dpy, winInfo)
Display	*dpy;
WinButton *winInfo;
{
    GC erasegc;

    /*
     * REMIND
     * 
     * Erase the background first.  Unfortunately, we can't depend on
     * OLGX_ERASE to do the right thing, because it (a) erases only in BG1,
     * and (b) erases only in 2D mode.  We need to erase a background color
     * that depends on the state of the frame.  If we're in click-focus and we
     * have the focus, draw in BG2; otherwise, draw in BG1.
     */

    if (!GRV.FocusFollowsMouse && winInfo->core.client->isFocus)
	erasegc = DrawSelectedGC;
    else
	erasegc = DrawBackgroundGC;

    XFillRectangle(dpy, winInfo->core.self, erasegc, 0, 0,
		   Abbrev_MenuButton_Width(olgx_gisnormal),
		   Abbrev_MenuButton_Height(olgx_gisnormal));

    olgx_draw_abbrev_button(olgx_gisnormal, winInfo->core.self,
			    0, 0, OLGX_NORMAL | OLGX_ERASE);
}


/*
 * DestroyButton -- destroy the close button window resources and free any allocated
 *	data.
 */
static int
destroyButton(dpy, winInfo)
Display	*dpy;
WinButton *winInfo;
{
	/* free our data and throw away window */
	XDestroyWindow(dpy, winInfo->core.self);
	WIUninstallInfo(winInfo->core.self);
	MemFree(winInfo);
}

/* 
 * focusButton - the focus or selection state has changed
 */
static int
focusButton(dpy, winInfo, selected)
Display *dpy;
WinButton *winInfo;
Bool selected;
{
        (WinFunc(winInfo,core.drawfunc))(dpy, winInfo);
}

/*
 * heightfuncButton - recomputes the height of the close button window
 */
static int 
heightfuncButton(win, pxcre)
WinButton *win;
XConfigureRequestEvent *pxcre;
{
	return Abbrev_MenuButton_Width(olgx_gisnormal);
}

/*
 * widthfuncButton - recomputes the width of the close button window
 */
static int 
widthfuncButton(win, pxcre)
WinButton *win;
XConfigureRequestEvent *pxcre;
{
	return Abbrev_MenuButton_Height(olgx_gisnormal);
}


/***************************************************************************
* global functions
***************************************************************************/

/*
 * MakeButton  -- create the close button window. Return a WinGeneric structure.
 */
WinButton *
MakeButton(dpy, par, x, y)
Display	*dpy;
WinGeneric *par;
int x,y;
{
	WinButton *w;
	Window win;
        unsigned long valuemask;
        XSetWindowAttributes attributes;
	Client *cli = par->core.client;

        attributes.event_mask =
	    ButtonReleaseMask | ButtonPressMask | ExposureMask;
	attributes.cursor = GRV.IconPointer;
        valuemask = CWEventMask | CWCursor;

        win = XCreateWindow(dpy, par->core.self,
                        x, y,
			Abbrev_MenuButton_Width(olgx_gisnormal), 
			Abbrev_MenuButton_Height(olgx_gisnormal),
                        0,
                        DefaultDepth(dpy, DefaultScreen(dpy)),
                        CopyFromParent,
                        DefaultVisual(dpy, DefaultScreen(dpy)),
                        valuemask,
                        &attributes);

	/* create the associated structure */
	w = MemNew(WinButton);
	w->core.self = win;
	w->class = &classButton;
	w->core.kind = WIN_WINBUTTON;
	WinAddChild(par,w);
	w->core.children = NULL;
	w->core.client = par->core.client;
	w->core.x = x;	
	w->core.y = y;
	w->core.width = Abbrev_MenuButton_Width(olgx_gisnormal);
	w->core.height = Abbrev_MenuButton_Height(olgx_gisnormal);
	w->core.dirtyconfig = 0;
	w->core.exposures = NULL;

	/* register the window */
	WIInstallInfo(w);

        XMapWindow(dpy, win);

	return w;
}

void
ButtonInit(dpy)
Display *dpy;
{
        classButton.core.kind = WIN_WINBUTTON;
        classButton.core.xevents[ButtonPress] = eventButtonPress;
        classButton.core.xevents[ButtonRelease] = eventButtonRelease;
        classButton.core.xevents[MotionNotify] = eventMotionNotify;
        classButton.core.xevents[Expose] = WinEventExpose;
        classButton.core.focusfunc = focusButton;
        classButton.core.drawfunc = drawButton;
        classButton.core.destroyfunc = destroyButton;
        classButton.core.selectfunc = NULL;
        classButton.core.newconfigfunc = WinNewConfigFunc;
        classButton.core.newposfunc = WinNewPosFunc;
        classButton.core.setconfigfunc = WinSetConfigFunc;
        classButton.core.createcallback = NULL;
        classButton.core.heightfunc = heightfuncButton;
        classButton.core.widthfunc = widthfuncButton;
}

