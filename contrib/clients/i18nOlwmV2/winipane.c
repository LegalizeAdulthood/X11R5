/*
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) winipane.c 50.5 90/12/12 Crucible";

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
#include "menu.h"
#include "globals.h"
#include "events.h"

#include "iconbitmap.h"

/***************************************************************************
* global data
***************************************************************************/

extern Atom AtomChangeState;
extern Atom AtomColorMapWindows;
extern Atom AtomOlwmTimestamp;
extern Window NoFocusWin;
extern GC IconSelectedGC, IconNormalGC;

/***************************************************************************
* private data
***************************************************************************/

/* border width for reparented windows */
#define NORMAL_BORDERWIDTH      0

/* This event maks if for wm-created icon panes. */
#define ICON_EVENT_MASK		(ButtonPressMask | ButtonReleaseMask | \
				 Button1MotionMask | ExposureMask )

/* This event mask is for clients who handle their own icons. */
#define ICON_EVENT_MASK_2        (ButtonPressMask | ButtonReleaseMask | \
				  Button1MotionMask)

static ClassPane classIconPane;

/* default icon bitmap (single-plane pixmap) */
static Pixmap   pixIcon;

/***************************************************************************
* private functions
***************************************************************************/

/*
 * drawIPane -- draw the pane window
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static int
drawIPane(dpy, winInfo)
Display	*dpy;
WinIconPane *winInfo;
{
	Window		pane;
	Client		*cli;
	WinIconFrame	*iconFrame;
	GC		iconGC;
	unsigned long	iconBgPixel;
	extern GC	IconSelectedGC, IconNormalGC, IconUnselectedGC;

	cli = winInfo->core.client;
	iconFrame = cli->iconwin;
	pane = winInfo->core.self;

	if (!winInfo->iconClientWindow) {
	    XFillRectangle(dpy, pane, IconUnselectedGC, 0, 0,
			   winInfo->core.width, winInfo->core.height);
	    XCopyPlane(dpy, winInfo->iconPixmap, pane, IconNormalGC,
		       0, 0, winInfo->core.width, winInfo->core.height,
		       0, 0, (unsigned long)1L);
	}
}


/*
 * focusIPane -- handle focus change
 */
static int
focusIPane(dpy, winInfo, focus)
Display	*dpy;
WinGeneric *winInfo;
Bool focus;
{
	/* REMIND: change background pixel of pane window */
}

/*
 * destroyIPane -- destroy the pane window resources and free any allocated
 *	data.
 */
static int
destroyIPane(dpy, winInfo)
Display	*dpy;
WinIconPane *winInfo;
{
	/* free our data and throw away window */
	WIUninstallInfo(winInfo->core.self);
        if (!winInfo->iconClientWindow)
	{
	      /* REMIND there may be other resources to be freed */
              XDestroyWindow(dpy,winInfo->core.self);
	}
	MemFree(winInfo);
}

/*
 * setconfigIPane -- change configuration of pane window
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static int
setconfigIPane(dpy, winInfo)
Display	*dpy;
WinIconPane *winInfo;
{
        XWindowChanges xwc;

        if (winInfo->core.dirtyconfig)
        {
                xwc.x = winInfo->core.x;
                xwc.y = winInfo->core.y;
                xwc.width = winInfo->core.width;
                xwc.height = winInfo->core.height;
                XConfigureWindow(dpy, winInfo->core.self,
                        winInfo->core.dirtyconfig&(CWX|CWY|CWWidth|CWHeight), &xwc);
                winInfo->core.dirtyconfig &= ~(CWX|CWY|CWWidth|CWHeight);
        }
}


/* 
 * newconfigIPane - compute a new configuration given an event
 * Note:  this function must *always* be called with a configure request
 * event.
 */
static int
newconfigIPane(win, pxcre)
WinIconPane *win;
XConfigureRequestEvent *pxcre;
{
    int oldWidth, oldHeight;
    Client *cli = win->core.client;
    int oldX, oldY;
    WinIconFrame *winFrame = (WinIconFrame *)(win->core.parent);

    if (pxcre == NULL)
	return win->core.dirtyconfig;

    oldX = win->core.x;
    oldY = win->core.y;
    oldWidth = win->core.width;
    oldHeight = win->core.height;

    if ((pxcre->value_mask & CWHeight) && (pxcre->height != oldHeight))
    {
	win->core.height = pxcre->height;
	win->core.dirtyconfig |= CWHeight;
    }

    if ((pxcre->value_mask & CWWidth) && (pxcre->width != oldWidth))
    {
	win->core.width = pxcre->width;
	win->core.dirtyconfig |= CWWidth;
    }

    if (pxcre->value_mask & CWBorderWidth)
    {
	win->pcore.oldBorderWidth = pxcre->border_width;
    }

    if (pxcre->value_mask & (CWX | CWY)) 
    {
	FrameSetPosFromPane(winFrame, (pxcre->value_mask & CWX)?(pxcre->x):oldX,
		(pxcre->value_mask & CWY)?(pxcre->y):oldY);
    }

    if (pxcre->value_mask & (CWStackMode | CWSibling))
    {
	GFrameSetStack(winFrame, pxcre->value_mask, pxcre->detail, pxcre->above);
    }

    return win->core.dirtyconfig;
}

/* 
 * newposIPane - move to a given position (relative to parent)
 */
static int
newposIPane(win,x,y)
WinIconPane *win;
int x, y;
{
	if (win->core.x != x)
	{
		win->core.x = x;
		win->core.dirtyconfig |= CWX;
	}

	if (win->core.y != y)
	{
		win->core.y = y;
		win->core.dirtyconfig |= CWY;
	}

	return win->core.dirtyconfig;
}

/* 
 * setsizeIPane - set the pane to a particular size, and initiate a reconfigure
 */
static int
setsizeIPane(win,w,h)
WinIconPane *win;
int w, h;
{
	if (win->core.width != w)
	{
		win->core.width = w;
		win->core.dirtyconfig |= CWWidth;
	}

	if (win->core.height != h)
	{
		win->core.height = h;
		win->core.dirtyconfig |= CWHeight;
	}
}

/***************************************************************************
* global functions
***************************************************************************/

/*
 * MakeIconPane  -- create the pane window. Return a WinGeneric structure.
 */
WinIconPane *
MakeIconPane(cli,par,wmHints,fexisting)
Client *cli;
WinGeneric *par;
XWMHints *wmHints;
Bool fexisting;
{
	WinIconPane *w;
	WinIconFrame	*frame = (WinIconFrame *)par;
	XSetWindowAttributes attributes;
	long valuemask;
	XWindowAttributes winAttr;
	Window iconPane;
	Window winRoot;
	int borderWidth, depthReturn;
	Display *dpy = cli->dpy;
	int screen = cli->screen;
	Status status;

	/* create the associated structure */
	w = MemNew(WinIconPane);
	w->class = &classIconPane;
	w->core.kind = WIN_ICONPANE;
	WinAddChild(par,w);
	w->core.children = NULL;
	w->core.client = cli;
	w->core.x = 0;
	w->core.y = 0;
	w->core.colormap = None;
	w->core.dirtyconfig = CWX|CWY|CWWidth|CWHeight;
	w->core.exposures = NULL;

	/* install the pane structure.
	 */
	frame->fcore.panewin = (WinGenericPane *)w;

	/* create the pane window and size it */
	if (wmHints && (wmHints->flags & IconWindowHint))
	{
	    WinGeneric *wrongPane;

            /* if we are reparenting existing windows, make sure that
	     * we haven't already accidentally processed the icon window
	     * as a pane.  If so, tear down the client created for that
	     * "pane".
	     */
	    if (((wrongPane = WIGetInfo(wmHints->icon_window)) != NULL) && 
		(wrongPane->core.kind != WIN_PANE))
	    {
		wmHints->flags &= ~IconWindowHint;
#ifdef OW_I18N
		ErrorWarning(
		   gettext("An existing window was named as an icon window."));
#else
		ErrorWarning("An existing window was named as an icon window.");
#endif
	    }
	    else
	    {
		if (wrongPane != NULL)
		{
		    StateWithdrawn(wrongPane->core.client);
		}

	        /* get the current width and height of the icon window */
	        status = XGetGeometry(dpy, wmHints->icon_window, &winRoot,
		    &(w->core.x), &(w->core.y),
		    &(w->core.width), &(w->core.height),
		    &borderWidth, &depthReturn);

		if (status)
		{
	            w->iconPixmap = None;
	            w->iconClientWindow = True;
	            iconPane = wmHints->icon_window;
	            XSelectInput(dpy, iconPane, ICON_EVENT_MASK_2);
	            XSetWindowBorderWidth(dpy, iconPane, NORMAL_BORDERWIDTH);
		}
		else
		{
		    wmHints->flags &= ~IconWindowHint;
#ifdef OW_I18N
		    ErrorWarning(gettext("An invalid window is was named as an icon window."));
#else
		    ErrorWarning("An invalid window is was named as an icon window.");
#endif
		}
	    }
	}
	if (wmHints && !(wmHints->flags & IconWindowHint) &&
	    (wmHints->flags & IconPixmapHint))
	{
	    w->iconClientWindow = False;
	    w->iconPixmap = wmHints->icon_pixmap;

	    /* get pixmap dimensions */
	    status = XGetGeometry(dpy, w->iconPixmap, &winRoot,
		&(w->core.x), &(w->core.y),
		&(w->core.width), &(w->core.height),
		&borderWidth, &depthReturn);

	    if (status)
	    {
	         /* build icon pixmap window */
	         valuemask = CWEventMask;
	         attributes.event_mask = ICON_EVENT_MASK;

	         iconPane = XCreateWindow(dpy, RootWindow(cli->dpy, cli->screen),
		     0, 0, w->core.width, w->core.height,
		     0, DefaultDepth(dpy, screen), InputOutput,
		     DefaultVisual(dpy, screen), valuemask, &attributes);
	    }
	    else
	    {
#ifdef OW_I18N
		ErrorWarning(
		     gettext("An invalid pixmap was named as an icon pixmap"));
#else
		ErrorWarning("An invalid pixmap was named as an icon pixmap");
#endif
		wmHints->flags &= ~IconPixmapHint;
	    }
	}
	if (!wmHints || (wmHints && !(wmHints->flags & (IconPixmapHint|IconWindowHint))))
	{
	    w->iconClientWindow = False;
	    w->iconPixmap = pixIcon;

	    /* get pixmap dimensions */
	    XGetGeometry(dpy, w->iconPixmap, &winRoot,
		&(w->core.x), &(w->core.y),
		&(w->core.width), &(w->core.height),
		&borderWidth, &depthReturn);

	    /* build icon pixmap window */
	    valuemask = CWEventMask;
	    attributes.event_mask = ICON_EVENT_MASK;

	    iconPane = XCreateWindow(dpy, RootWindow(cli->dpy, cli->screen),
		0, 0, w->core.width, w->core.height,
		0, DefaultDepth(dpy, screen), InputOutput,
		DefaultVisual(dpy, screen), valuemask, &attributes);
	}
	w->core.self = iconPane;

	/* set up icon cursor */
	XDefineCursor(dpy, w->core.self, GRV.IconPointer);

	/* register the window */
	WIInstallInfo(w);

	return w;
}

/*
 * IconPaneInit -- initialise the IconPane class function vector
 */
void
IconPaneInit(dpy)
Display *dpy;
{
        /* create default icon bitmap (single-plane pixmap) which has color
         * applied to it when copied into icon window
         */
        pixIcon = XCreateBitmapFromData( dpy,
                                         RootWindow(dpy, DefaultScreen(dpy)),
                                         icon_bits, icon_width, icon_height );

	classIconPane.core.kind = WIN_ICONPANE;
	classIconPane.core.xevents[Expose] = WinEventExpose;
	classIconPane.core.xevents[ButtonRelease] = PropagateEventToParent;
	classIconPane.core.xevents[MotionNotify] = PropagateEventToParent;
	classIconPane.core.xevents[ButtonPress] = PropagateEventToParent;
	classIconPane.core.focusfunc = focusIPane;
	classIconPane.core.drawfunc = drawIPane;	/* NULL */
	classIconPane.core.destroyfunc = destroyIPane;
	classIconPane.core.selectfunc = drawIPane;	/* NULL */
	classIconPane.core.newconfigfunc = newconfigIPane;
	classIconPane.core.newposfunc = newposIPane;
	classIconPane.core.setconfigfunc = setconfigIPane;
	classIconPane.core.createcallback = NULL;
	classIconPane.core.heightfunc = NULL;
	classIconPane.core.widthfunc = NULL;
	classIconPane.pcore.setsizefunc = setsizeIPane;
}

