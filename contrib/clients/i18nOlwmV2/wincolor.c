/*
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) wincolor.c 50.4 90/12/12 Crucible";
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

/***************************************************************************
* global data
***************************************************************************/

extern Atom AtomColorMapWindows;

/*
 * ColorFocusLocked indicates the color focus mode.  If it is false, we are in
 * "color-follows-mouse" mode.  In this mode, colormaps are installed based on
 * the location of the mouse.  The WM_COLORMAP_WINDOWS property determines the
 * list of windows that are eligible to have their colormaps installed, but
 * changes to this property do not necessarily cause colormap installation.
 * If ColorFocusLocked is true, we are in "color-locked" mode.  In this mode,
 * a particular colormap is locked into the hardware, and colormaps do not
 * track the pointer location.  If a particular client has the focus, this
 * client will be named by ColorFocusClient, and changes to its
 * WM_COLORMAP_WINDOWS property will cause colormap changes.  If no client has
 * the focus (e.g. the colormap window is the root) then only user action can
 * cause the colormap to change.  In this case the ColorFocusClient will be
 * NULL.
 */
Bool ColorFocusLocked;
Client *ColorFocusClient = NULL;

/*
 * ColorFocusWindow indicates the window that currently has the color focus.  
 * This is updated regardless of the color focus mode.
 */
WinGeneric *ColorFocusWindow;

extern void WinAddColorClient();
extern void WinRemoveColorClient();

/***************************************************************************
* private data
***************************************************************************/

static Colormap currColormap = None;
static Window currColormapWindow = NULL;
static ClassColormap classColormap;

/***************************************************************************
* private functions
***************************************************************************/

/* 
 * eventDestroy - handle destroy events on the colormap window 
 */
static int
eventDestroy(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinColormap *winInfo;
{
    Client *cli;
    List *cli_list = winInfo->core.colormapClients;
    List **win_list;
    WinGeneric *newfocuswin;

    /*
     * For every client in this window's client list, search that client's 
     * window list and remove this window from it.
     */
    for (cli = ListEnum(&cli_list); cli != NULL; cli = ListEnum(&cli_list)) {
	win_list = &(cli->colormapWins);
	while (*win_list != NULL) {
	    if ((*win_list)->value == winInfo) {
		ListDestroyCell(win_list);
		break;
	    }
	    win_list = &((*win_list)->next);
	}
	if (ColorFocusLocked && ColorFocusWindow == winInfo &&
	    ColorFocusClient == cli) {
	    if (cli->colormapWins)
		newfocuswin = cli->colormapWins->value;
	    else
		newfocuswin = (WinGeneric *) PANEOFCLIENT(cli);
	    InstallColormap(dpy, newfocuswin);
	}
    }

    ListDestroy(winInfo->core.colormapClients);
    winInfo->core.colormapClients = NULL_LIST;
    (WinFunc(winInfo,core.destroyfunc))(dpy, winInfo);
}


/* 
 * eventEnterLeaveNotify - handle enter/leave notify events on the colormap window 
 */
static int
eventEnterLeaveNotify(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinColormap *winInfo;
{
    if (event->xany.type == EnterNotify)
	ColorWindowCrossing(dpy, event, winInfo);
}


/*
 * eventUnmapNotify - handle the unmapping of a colormap window
 */
static int
eventUnmapNotify(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinColormap *winInfo;
{
    if (ColorFocusWindow == (WinGeneric *)winInfo && !ColorFocusLocked) {
	InstallPointerColormap(dpy, None, 0, 0, False);
    }
}


/*
 * eventColormapNotify
 *
 * Handle changes to this window's colormap attribute.  If this window had the 
 * colormap focus, install the new colormap.
 */
static int
eventColormapNotify(dpy, event, winInfo)
    Display *dpy;
    XEvent *event;
    WinColormap *winInfo;
{
    void ColormapChange();

    ColormapChange(dpy, event, (WinGeneric *)winInfo);
}
	    

/*
 * DestroyColormap -- destroy the colormap window resources and free any allocated
 *	data.
 */
static int
destroyColormap(dpy, winInfo)
Display	*dpy;
WinGeneric *winInfo;
{
#ifdef DEBUG
	if (winInfo->core.colormapClients != NULL_LIST)
	    puts("warning: destroying cmap window with non-null client list");
#endif /* DEBUG */

	/* free our data and throw away window */
	/* REMIND this test is here to avoid problems with changing
	 * a colourmap window into a pane
	 */
	if (WIGetInfo(winInfo->core.self) == winInfo)
	    WIUninstallInfo(winInfo->core.self);
	MemFree(winInfo);
}

/***************************************************************************
* global functions
***************************************************************************/

/*
 * InstallColormap
 *
 * Install the colormap for the given window.  If the window's colormap 
 * attribute is None, install the default colormap instead.  This can occur if
 * a client creates a window, sets its colormap attribute to a particular 
 * colormap, and then destroys that colormap.
 */
void
InstallColormap(dpy, winInfo)
    Display         *dpy;
    WinGeneric	    *winInfo;
{
    Colormap cmap = winInfo->core.colormap;

    if (cmap == None) {
	Client *cli = winInfo->core.client;
	if (cli == NULL)
	    cmap = DefaultColormap(dpy, DefaultScreen(dpy));
	else {
	    WinRoot *root = WIGetInfo(RootWindow(cli->dpy, cli->screen));
	    cmap = root->core.colormap;
	}
    }

    XInstallColormap(dpy, cmap);
    ColorFocusWindow = winInfo;
}


/*
 * InstallPointerColormap
 *
 * Install the colormap for the leafmost window that we know about that 
 * encloses the pointer.  The boolean setfocusclient indicates whether to set 
 * the colormap focus client to this window's client.  If root is None, we do 
 * a QueryPointer to find out where the pointer is instead of using the rootx 
 * and rooty values.
 */
void
InstallPointerColormap(dpy, root, rootx, rooty, setfocusclient)
    Display *dpy;
    Window root;
    int rootx, rooty;
    Bool setfocusclient;
{
    Window src;
    Window dest;
    Window child;
    int srcx;
    int srcy;
    int destx, desty;
    WinGeneric *wi = NULL;
    WinGeneric *t;

    if (root == None) {
	Window child;
	unsigned int state;
	int junkx, junky;

	/* We want only the root, rootx, and rooty; we throw the rest away */
	XQueryPointer(dpy, DefaultRootWindow(dpy), &root, &child,
		      &rootx, &rooty, &junkx, &junky, &state);
    }

    src  = root;
    dest = root;
    srcx = rootx;
    srcy = rooty;

    while (1) {
	(void) XTranslateCoordinates(dpy, src, dest, srcx, srcy,
				     &destx, &desty, &child);
	t = WIGetInfo(dest);
	if (t != NULL)
	    wi = t;
	if (child == None)
	    break;
	src = dest;
	dest = child;
	srcx = destx;
	srcy = desty;
    }
    /*
     * At this point, dest contains the leafmost window that encloses the 
     * pointer, and wi points to the window structure of the leafmost known 
     * window that encloses the pointer.
     */

    /* if we didn't find a window we know about, use the root instead */

    if (wi == NULL) {
	wi = WIGetInfo(root);
	if (wi == NULL)
	    return;
    }

    /*
     * If we are over a frame, its window button, or its resize corners,
     * use the head of the pane's colormap window list, or the pane itself if 
     * it has no list.
     */
    switch (wi->core.kind) {
    case WIN_FRAME:
    case WIN_RESIZE:
    case WIN_PUSHPIN:
    case WIN_WINBUTTON:
	if (wi->core.client->colormapWins)
	    wi = wi->core.client->colormapWins->value;
	else
	    wi = (WinGeneric *) PANEOFCLIENT(wi->core.client);
	break;
    default:
	break;
    }
    InstallColormap(dpy, wi);

    if (setfocusclient) {
	if (wi->core.colormapClients) {
	    /* REMIND if multiple clients, use just the first one */
	    ColorFocusClient = (Client *) wi->core.colormapClients->value;
	} else if (wi->core.client) {
	    ColorFocusClient = wi->core.client;
	} else {
	    ColorFocusClient = NULL;
	}
	ColorFocusLocked = True;
    }
}


/*
 * UnlockColormap
 *
 * Turn off colormap-locked mode.
 */
void
UnlockColormap(dpy)
    Display *dpy;
{
    ColorFocusClient = NULL;
    ColorFocusLocked = False;
    InstallPointerColormap(dpy, None, 0, 0, False);
}


/*
 * ColormapChange
 *
 * Handle a change to a window's colormap attribute.
 */
void
ColormapChange(dpy, event, winInfo)
    Display *dpy;
    XEvent *event;
    WinGeneric *winInfo;
{
    if (event->xcolormap.new) {
	winInfo->core.colormap = event->xcolormap.colormap;
	if (winInfo == ColorFocusWindow)
	    InstallColormap(dpy, winInfo);
    }
}


/*
 * ColorWindowCrossing
 *
 * Handle colormap installation on crossing events.  If we are not in
 * colormap-locked mode, install the window's colormap.
 */
void
ColorWindowCrossing(dpy, event, winInfo)
    Display *dpy;
    XEvent  *event;
    WinGeneric *winInfo;
{
    if (!ColorFocusLocked)
	InstallColormap(dpy, winInfo);
}


/*
 * TrackSubwindows      -- check for the WM_COLORMAP_WINDOWS prop
 *                      on a pane, if it exists, track the subwindows.
 */
void
TrackSubwindows(cli)
    Client *cli;
{
    Display	    *dpy = cli->dpy;
    int		    screen = cli->screen;
    Window	    pane = PANEWINOFCLIENT(cli);
    Atom            actType;
    int             actFormat;
    unsigned long   nItems, remain;
    Window	    *cmapwindata;
    int		    status;
    List	    **last;
    List	    *oldlist;
    List	    *l;
    WinGeneric	    *cmwi;
    int		    i;
    Bool	    paneseen = False;

    oldlist = cli->colormapWins;
    cli->colormapWins = NULL_LIST;

    cmapwindata = GetWindowProperty(dpy, pane, AtomColorMapWindows, 0L, 
		1000000L, XA_WINDOW, 32, &nItems, &remain);
    if (cmapwindata == NULL)
    {
	return;
    }

    /*
     * Register all the windows on the new list, taking care to not touch any 
     * window that was on the old list, while getting rid of windows not on 
     * the new list.  This is a three-step process:
     *
     * 1.   Mark all old windows using the "tag" field.
     * 
     * 2.   Run through the new list.  If we encounter a marked window,
     *	    we know it was on the old list; unmark it.  For other windows,
     *	    we add the client to window's client list.  If we didn't see the 
     *	    pane window on the new list, add it to the front, and deal with it 
     *	    as for any other window.
     *
     * 3.   Any marked window on the old list is not on the new list.  Run
     *	    through the old list and, for each unmarked window, remove this
     *	    client from that window's client list.  Further, if this leaves no 
     *	    clients on the window's client list, and this window is a colormap 
     *	    window, destroy it.
     */

    l = oldlist;
    for (cmwi = ListEnum(&l); cmwi != NULL; cmwi = ListEnum(&l))
	cmwi->core.tag = True;
    
    last = &cli->colormapWins;
    for (i=0; i<nItems; ++i) {
	paneseen = (paneseen || (cmapwindata[i] == pane));
	cmwi = WIGetInfo(cmapwindata[i]);
	if (cmwi == NULL) {
	    cmwi = MakeColormap(cli, cmapwindata[i]);
	} else {
	    if (cmwi->core.tag) {
		cmwi->core.tag = False;
	    } else
		WinAddColorClient(cmwi, cli);
	}
	(*last) = ListCons(cmwi, NULL_LIST);
	last = &((*last)->next);
    }
    XFree(cmapwindata);

    if (!paneseen) {
	WinGenericPane *paneinfo = PANEOFCLIENT(cli);

	cli->colormapWins = ListCons(paneinfo, cli->colormapWins);
	if (paneinfo->core.tag) {
	    /* pane was on the old list, but not on the new */
	    paneinfo->core.tag = False;
	} else {
	    WinAddColorClient(paneinfo, cli);
	}
    }

    l = oldlist;
    for (cmwi = ListEnum(&l); cmwi != NULL; cmwi = ListEnum(&l)) {
	if (cmwi->core.tag)
	    WinRemoveColorClient(dpy, cmwi, cli);
    }
    ListDestroy(oldlist);

    /* install colormaps as necessary */

    if (!ColorFocusLocked) {
	InstallPointerColormap(dpy, None, 0, 0, False);
    } else if (ColorFocusClient == cli) {
	if (cli->colormapWins)
	    InstallColormap(dpy, (WinGeneric *)cli->colormapWins->value);
	else
	    InstallColormap(dpy, PANEOFCLIENT(cli));
    }
}


/*
 * UnTrackSubwindows -- stop tracking all subwindows.  The Bool destroyed 
 * indicates that this client is being destroyed.  If so, this client loses 
 * the colormap focus.  If not, the color focus window is transferred to this
 * client's pane.
 */
void
UnTrackSubwindows(cli, destroyed)
    Client *cli;
    Bool destroyed;
{
    WinGeneric	*wi;
    List	*l;

    l = cli->colormapWins;
    for (wi = ListEnum(&l); wi != NULL; wi = ListEnum(&l))
	WinRemoveColorClient(cli->dpy, wi, cli);
    ListDestroy(cli->colormapWins);
    cli->colormapWins = NULL_LIST;

    if (ColorFocusClient == cli) {
	if (destroyed) {
	    ColorFocusClient = NULL;
	    if (GRV.ColorFocusLocked) {
		/* lock in the root's colormap */
		InstallColormap(cli->dpy,
				WIGetInfo(DefaultRootWindow(cli->dpy)));
	    } else {
		/* revert to follow-mouse */
		ColorFocusLocked = False;
		InstallPointerColormap(cli->dpy, None, 0, 0, False);
	    }
	} else {
	    InstallColormap(cli->dpy, PANEOFCLIENT(cli));
	}
    }
}


/*
 * MakeColormap  -- create the colormap window. Return a WinGeneric structure.
 */
WinColormap *
MakeColormap(cli,win)
Client *cli;
Window win;
{
	WinColormap *w;
        XWindowAttributes winAttr;
	int status;

        if (XGetWindowAttributes(cli->dpy, win, &winAttr) == 0)
	    return NULL;

	/* create the associated structure */
	w = MemNew(WinColormap);
	w->class = &classColormap;
	w->core.self = win;
	w->core.kind = WIN_COLORMAP;
	w->core.client = cli;
        w->core.colormap = winAttr.colormap;
	w->core.colormapClients = ListCons(cli, NULL_LIST);

        /* start paying attention to enter/exits on the win */
        XSelectInput(cli->dpy, win,
		     EnterWindowMask | ColormapChangeMask |
		     StructureNotifyMask);

	/* register the window */
	WIInstallInfo(w);

	return w;
}


void
ColormapInit(dpy)
Display *dpy;
{
        classColormap.core.kind = WIN_COLORMAP;
        classColormap.core.xevents[DestroyNotify] = eventDestroy;
        classColormap.core.xevents[EnterNotify] = eventEnterLeaveNotify;
        classColormap.core.xevents[LeaveNotify] = eventEnterLeaveNotify;
	classColormap.core.xevents[UnmapNotify] = eventUnmapNotify;
	classColormap.core.xevents[ColormapNotify] = eventColormapNotify;
        classColormap.core.focusfunc = NULL;
        classColormap.core.drawfunc = NULL;
        classColormap.core.destroyfunc = destroyColormap;
        classColormap.core.selectfunc = NULL;
        classColormap.core.newconfigfunc = NULL;
        classColormap.core.newposfunc = NULL;
        classColormap.core.setconfigfunc = NULL;
        classColormap.core.createcallback = NULL;
        classColormap.core.heightfunc = NULL;
        classColormap.core.widthfunc = NULL;
}


void
ColorFocusInit(dpy, root)
    Display *dpy;
    WinGeneric *root;
{
    InstallColormap(dpy, root);
    ColorFocusClient = NULL;
    ColorFocusLocked = GRV.ColorFocusLocked;
}

/* The following two functions are used when a pane is being mapped, to
 * handle the possibility that a pane has already been named as a
 * colourmap window before it was mapped.
 */

/* ColormapUnhook -- Given a window, if exists as a colourmap 
 *	window, remove it from the event dispatching lookup table
 * 	and return a pointer to the window structure.
 */
WinColormap *
ColormapUnhook(w)
Window w;
{
	WinColormap *win;

	win = WIGetInfo(w);
	if (win != NULL)
	{
	    if (win->core.kind == WIN_COLORMAP)
	    {
	        WIUninstallInfo(w);
	    }
	    else
	    {
		win = NULL;
	    }
	}
	return win;
}

/* ColormapTransmogrify -- Take a previously-saved colourmap window
 *	structure, which has been superceded by a pane window structure,
 *	and patch up client-to-window references to point to the
 *	pane window structure.  When done, destroy the colourmap
 *	window structure.
 */
void
ColormapTransmogrify(winc, winp)
WinColormap *winc;
WinPane *winp;
{
    Client *cli;
    List *cli_list;
    List *win_list;
    WinGeneric *newfocuswin;

    if (winc == NULL)
	return;

    /*
     * For every client in the colourmap window's client list, search that 
     * client's window list and change the reference.
     */
    cli_list = winc->core.colormapClients;
    for (cli = ListEnum(&cli_list); cli != NULL; cli = ListEnum(&cli_list)) {
	win_list = cli->colormapWins;
	while (win_list != NULL) {
	    if (win_list->value == winc) {
		win_list->value = winp;
		break;
	    }
	    win_list = win_list->next;
	}
    }

    /* patch up other pointers */
    if (ColorFocusWindow == winc)
	ColorFocusWindow = (WinGeneric *)winp;
#ifdef NOTDEF
    /* REMIND check that this next statement is correct */
    if (ColorFocusClient == winc->core.client)
	ColorFocusClient = winp->core.client;
#endif

    winp->core.colormapClients = winc->core.colormapClients;
    winc->core.colormapClients = NULL_LIST;

    /* the colourmap window can now be destroyed since all references
     * to it have been removed.
     */
    (WinFunc(winc,core.destroyfunc))(winc->core.client->dpy, winc);
}
