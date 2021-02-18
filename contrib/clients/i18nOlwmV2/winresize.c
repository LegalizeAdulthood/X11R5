/*
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) winresize.c 50.4 90/12/12 Crucible";

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
extern int Resize_width, Resize_height;
extern GC DrawBusyGC, DrawSelectedGC, DrawBackgroundGC;

/***************************************************************************
* private data
***************************************************************************/

static int whicholgx[] = {OLGX_UPPER_LEFT, OLGX_UPPER_RIGHT, 
	OLGX_LOWER_LEFT, OLGX_LOWER_RIGHT};
static int whichgrav[] = {NorthWestGravity, NorthEastGravity, 
	SouthWestGravity, SouthEastGravity};

static ClassResize classResize;

/***************************************************************************
* private functions
***************************************************************************/

/* 
 * eventButtonPress - handle button press events on the resize window 
 */
static int
eventButtonPress(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinResize	*winInfo;
{
        if (MouseButton(dpy, event) != MB_SELECT)
                return;

	/* draw depressed corner */
	winInfo->depressed = True;
        (WinFunc(winInfo,core.drawfunc))(dpy,winInfo);

	/* resize function will eat button release */
        UserResizeWin(winInfo->core.client, winInfo->which, event);

        /* put corner back */
	winInfo->depressed = False;
        (WinFunc(winInfo,core.drawfunc))(dpy,winInfo);
}


/*
 * drawResize -- draw the resize window
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static int
drawResize(dpy, winInfo)
Display	*dpy;
WinResize *winInfo;
{
	WhichResize which = winInfo->which;

	if ((which == upright) || (which == upleft))
	{
	    if (winInfo->core.client->isBusy) {
		XFillRectangle(dpy,winInfo->core.self,DrawBackgroundGC,
		    (which==upleft)?(ResizeArm_Width(olgx_gisnormal)):(0),
		    ResizeArm_Height(olgx_gisnormal), 
		    winInfo->core.width-ResizeArm_Width(olgx_gisnormal),
		    winInfo->core.height-ResizeArm_Height(olgx_gisnormal));
		XFillRectangle(dpy,winInfo->core.self,DrawBusyGC,
		    (which==upleft)?(ResizeArm_Width(olgx_gisnormal)):(0),
		    ResizeArm_Height(olgx_gisnormal), 
		    winInfo->core.width-ResizeArm_Width(olgx_gisnormal),
		    winInfo->core.height-ResizeArm_Height(olgx_gisnormal));
	    } else if ((winInfo->core.client->isFocus) && (!GRV.FocusFollowsMouse))
	    {
		if (GRV.F3dUsed)
		{
		    olgx_draw_box(olgx_gisnormal, winInfo->core.self,
		        (which==upleft)?(ResizeArm_Width(olgx_gisnormal)):(-2),
		        ResizeArm_Height(olgx_gisnormal), 
		        winInfo->core.width-ResizeArm_Width(olgx_gisnormal)+2,
		        winInfo->core.height-ResizeArm_Height(olgx_gisnormal)+2,
			OLGX_INVOKED, True);
		}
		else
		{
		    XFillRectangle(dpy,winInfo->core.self,DrawSelectedGC,
		        (which==upleft)?(ResizeArm_Width(olgx_gisnormal)):(0),
		        ResizeArm_Height(olgx_gisnormal), 
		        winInfo->core.width-ResizeArm_Width(olgx_gisnormal),
		        winInfo->core.height-ResizeArm_Height(olgx_gisnormal));
		}
	    }
	    else
	    {
		XFillRectangle(dpy,winInfo->core.self,DrawBackgroundGC,
		    (which==upleft)?(ResizeArm_Width(olgx_gisnormal)):(0),
		    ResizeArm_Height(olgx_gisnormal), 
		    winInfo->core.width-ResizeArm_Width(olgx_gisnormal),
		    winInfo->core.height-ResizeArm_Height(olgx_gisnormal));
	    }
	} else {
	    XFillRectangle(dpy, winInfo->core.self, DrawBackgroundGC,
		(which==lowleft) ? (ResizeArm_Width(olgx_gisnormal)) : 0, 0,
		winInfo->core.width-ResizeArm_Width(olgx_gisnormal),
		winInfo->core.height-ResizeArm_Height(olgx_gisnormal));
	}
	olgx_draw_resize_corner(olgx_gisnormal, winInfo->core.self, 0, 0,
		whicholgx[winInfo->which],
		(winInfo->depressed)?(OLGX_INVOKED):(OLGX_NORMAL));
}


/*
 * DestroyResize -- destroy the resize window resources and free any allocated
 *	data.
 */
static int
destroyResize(dpy, winInfo)
Display	*dpy;
WinGeneric *winInfo;
{
	/* free our data and throw away window */
	XUndefineCursor(dpy, winInfo->core.self);
	XDestroyWindow(dpy, winInfo->core.self);
	WIUninstallInfo(winInfo->core.self);
	MemFree(winInfo);
}

/* 
 * focusselectResize - the focus or selection state has changed
 */
static int
focusResize(dpy, winInfo, selected)
Display *dpy;
WinResize *winInfo;
Bool selected;
{
	(WinFunc(winInfo,core.drawfunc))(dpy, winInfo);
}

/*
 * widthfuncResize - recomputes the width of the resize window
 */
static int 
widthfuncResize(win, pxcre)
WinResize *win;
XConfigureRequestEvent *pxcre;
{
	return Resize_width;
}

/*
 * heightfuncResize - recomputes the width of the resize window
 */
static int 
heightfuncResize(win, pxcre)
WinResize *win;
XConfigureRequestEvent *pxcre;
{
	return Resize_height;
}

/***************************************************************************
* global functions
***************************************************************************/

/*
 * MakeResize  -- create the resize window. Return a WinGeneric structure.
 */
WinResize *
MakeResize(dpy, par, which, x, y)
Display	*dpy;
WinGeneric *par;
WhichResize which;
int x,y;
{
	WinResize *w;
	Window win;
        unsigned long valuemask;
        XSetWindowAttributes attributes;

        attributes.event_mask = ButtonPressMask | ExposureMask;
        attributes.win_gravity = whichgrav[which];
	attributes.cursor = GRV.ResizePointer;
        valuemask = CWEventMask | CWWinGravity | CWCursor;

        win = XCreateWindow(dpy, par->core.self,
                         x, y,
                         Resize_width, Resize_height,
                         0,
                         DefaultDepth(dpy, DefaultScreen(dpy)),
                         CopyFromParent,
                         DefaultVisual(dpy, DefaultScreen(dpy)),
                         valuemask,
                         &attributes);

	/* create the associated structure */
	w = MemNew(WinResize);
	w->core.self = win;
	w->class = &classResize;
	w->core.kind = WIN_RESIZE;
	WinAddChild(par,w);
	w->core.children = NULL;
	w->core.client = par->core.client;
	w->core.x = x;	
	w->core.y = y;
	w->core.width = Resize_width;
	w->core.height = Resize_height;
	w->core.dirtyconfig = CWX | CWY | CWWidth | CWHeight;
	w->core.exposures = NULL;
	w->which = which;

	/* register the window */
	WIInstallInfo(w);

        XMapRaised(dpy, win);

	return w;
}


/* ResizeInit -- initialise values for the resize class 
 */
void
ResizeInit(dpy)
Display *dpy;
{
	classResize.core.kind = WIN_RESIZE;
	classResize.core.xevents[Expose] = WinEventExpose;
	classResize.core.xevents[ButtonPress] = eventButtonPress;
	classResize.core.focusfunc= focusResize;
	classResize.core.drawfunc= drawResize;
	classResize.core.destroyfunc = destroyResize;
	classResize.core.selectfunc = NULL;
	classResize.core.newconfigfunc = WinNewConfigFunc;
	classResize.core.newposfunc = WinNewPosFunc;
	classResize.core.setconfigfunc = WinSetConfigFunc;
	classResize.core.createcallback = NULL;
	classResize.core.heightfunc = heightfuncResize;
	classResize.core.widthfunc = widthfuncResize;
}
