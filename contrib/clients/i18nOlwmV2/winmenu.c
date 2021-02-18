/*
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) winmenu.c 50.5 90/12/12 Crucible";

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

extern Atom AtomDecorAdd;
extern Atom AtomDecorHeader;
extern Atom AtomDecorPin;
extern Atom AtomDeleteWindow;
extern Atom AtomMenuLimited;
extern Atom AtomProtocols;
extern Atom AtomPushpinState;
extern Atom AtomWinAttr;
extern Atom AtomWTOther;
#ifdef OW_I18N
extern Atom AtomCompoundText;
#endif

extern Window NoFocusWin;

/***************************************************************************
* private data
***************************************************************************/

/* function vector for menu windows */
static ClassMenu classMenu;

static int bindex_last; 	/* set to last menu button pressed; cleared
				 * when button released or a submenu is
				 * brought up */

/*
 * after_info_t
 *
 * REMIND this is an abomination.  This structure simply holds the arguments
 * for the "afterProcessing" function, which should get called when a pop-up
 * menu is finished tracking.  Unfortunately, the architecture of this pinned
 * menu system assumes that popping up a menu is synchronous, when in fact it
 * just installs an event interposer and returns immediately.  The solution is
 * to pass along this information as a closure and have the information passed
 * back by the menu "sync" function.
 */

typedef struct {
    Display *dpy;
    Menu *menu;
    WinMenu *winInfo;
    int bindex;
} after_info_t;

static after_info_t afterInfo;

/***************************************************************************
* private functions
***************************************************************************/

static void
afterProcessing(dpy, menu, winInfo, bindex)
Display *dpy;
Menu *menu;
WinMenu *winInfo;
int bindex;
{
        Menu    *originalMenu = menu->originalMenu; /* menu this is derived from */
        Atom    actType;
        int     actFormat;
        unsigned long   nItems, remain;
        long    *pinState;

        /* unhighlight button */
        if ((bindex >= 0) && (bindex < menu->buttonCount))
                SetButton(dpy, menu, bindex, False);

        /* if our push-pin is out then we should go away */
	pinState = GetWindowProperty(dpy, winInfo->core.self, AtomPushpinState,
		0L, LONG_LENGTH(long), XA_INTEGER, 0, &nItems, &remain);

	if (pinState != NULL)
	{
            if (*pinState == PIN_OUT)     /* pin is out */
            {
                /* make our window die */
		ClientKill(winInfo->core.client,False);
            }
            XFree(pinState);
	}
}


static void
afterProcWrapper( i )
    after_info_t *i;
{
    afterProcessing( i->dpy, i->menu, i->winInfo, i->bindex );
}

/* pressButton -- a user has moved the pointer in such a way that a
 *      button should be pressed (that is, highlighted, ready for
 *      activation.  If any other button is pressed, unpress that button
 *      along the way.
 */
static int
pressButton(dpy, menu, winInfo, event, bindex, mbutton)
Display *dpy;
Menu *menu;
WinMenu *winInfo;
XEvent *event;
int bindex;
int mbutton;
{
        if ((mbutton == MB_MENU) || (mbutton = MB_SELECT))
        {
                SetButton(dpy, menu, bindex, True);

                if ((menu->buttons[bindex].stacked == True) &&
                    (menu->buttons[bindex].action.submenu != NULL) &&
                    (mbutton == MB_MENU))
                {
                        afterInfo.dpy = dpy;
                        afterInfo.menu = menu;
                        afterInfo.winInfo = winInfo;
                        afterInfo.bindex = bindex;
                        MenuShowSync(dpy, winInfo,
                            (Menu *)(menu->buttons[bindex].action.submenu),
                            event, afterProcWrapper, &afterInfo);
                }
        }
}



/* findButton - find the button corresponding to an x,y position
 *              modifies the x,y position to the correct popup location
 *              for cascaded menus.
 * returns: button index or -1 if button not found
 */
static int
findButton(menu, ex, ey, winInfo)
Menu *menu;
int *ex, *ey;
WinMenu *winInfo;
{
        int bindex;

        /* figure out if the button press is in a button */
        for (bindex=0; bindex < menu->buttonCount; bindex++)
        {
                if (PointInRect(*ex, *ey,
                                menu->buttons[bindex].activeX,
                                menu->buttons[bindex].activeY,
                                menu->buttons[bindex].activeW,
                                menu->buttons[bindex].activeH))
                {
			WinRootPos(winInfo, ex, ey);
                        *ex += menu->buttons[bindex].activeX +
                             menu->buttons[bindex].activeW;
                        *ey += menu->buttons[bindex].activeY;
                        return bindex;
                }
        }
        return -1;
}


/***************************************************************************
*  private event functions
***************************************************************************/

/* 
 * eventButtonPress  - a button has gone down.
 */
static int
eventButtonPress(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinMenu	*winInfo;
{
	Menu *menu = (Menu *)(winInfo->menu);
	int bindex;
	int ex, ey;

	ex = event->xbutton.x;
	ey = event->xbutton.y;
	bindex = findButton(menu, &ex, &ey, winInfo);
	bindex_last = bindex;
	if (bindex != -1)
	    pressButton(dpy, menu, winInfo, event, bindex, MouseButton(dpy,event));
}

/* 
 * eventButtonRelease  - a button has gone up
 */
static int
eventButtonRelease(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinMenu	*winInfo;
{
	Menu *menu = (Menu *)(winInfo->menu);
	int bindex;
	int ex, ey;

	ex = event->xbutton.x;
	ey = event->xbutton.y;

	if (MouseButton(dpy,event) == MB_ADJUST)
		return;

	bindex = findButton(menu, &ex, &ey, winInfo);

	/* Make sure that the button under the ButtonRelease is the
	 * same one that the ButtonPress went down on.
	 */
	if ((bindex == bindex_last) &&
	    (bindex_last >= 0) &&
	    (bindex_last < menu->buttonCount))
		ExecButtonAction(dpy,winInfo,menu,bindex,True);

	afterProcessing(dpy,menu,winInfo,bindex_last);
}

/* 
 * eventClientMessage  - handle a DELETE_WINDOW message.
 */
static int
eventClientMessage(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinMenu	*winInfo;
{
    if ((event->xclient.message_type == AtomProtocols) &&
        (event->xclient.data.l[0] == AtomDeleteWindow))
    {
	DestroyClient(winInfo->core.client);
    }
}

/* 
 * eventPropertyNotify  - the main window manager has changed a property on
 *	our pane; handle the property.
 */
static int
eventPropertyNotify(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinMenu	*winInfo;
{
	ClientDistributeProperty(winInfo->core.client, event);
}


/*
 * drawMenu -- draw the menu window
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static int
drawMenu(dpy, winInfo)
Display	*dpy;
WinMenu *winInfo;
{
	DrawMenu(dpy, winInfo->menu);
}


/*
 * DestroyMenu -- destroy the menu window resources and free any allocated
 *	data.
 */
static int
destroyMenu(dpy, winInfo)
Display	*dpy;
WinMenu *winInfo;
{
	/* free our data and throw away window */
	((Menu *)winInfo->menu)->originalMenu->currentlyDisplayed = False;
	MemFree(winInfo->menu);
	XUndefineCursor(dpy, winInfo->core.self);
	XDestroyWindow(dpy, winInfo->core.self);
	WIUninstallInfo(winInfo->core.self);
	MemFree(winInfo);
}

/* 
 * focusselectMenu - the focus or selection state has changed
 */
static int
focusselectMenu(dpy, winInfo, selected)
Display *dpy;
WinMenu *winInfo;
Bool selected;
{
        (WinFunc(winInfo,core.drawfunc))(dpy, winInfo);
}

/*
 * newconfigMenu - recomputes the size of the menu window
 *	Note that menus don't change size, so this is a no-op.
 */
static int 
newconfigMenu(win, pxcre)
WinMenu *win;
XConfigureRequestEvent *pxcre;
{
	return win->core.dirtyconfig;
}

/* menuSetParent -- callback during creation.  Since menus are internally-
 * 	created windows we must fix up certain fields that are only available
 *	after the window is mapped.
 */
static int
menuSetParent(w,cli,par)
WinMenu *w;
Client *cli;
WinGenericFrame *par;
{
    w->core.client = cli;
    WinAddChild(par,w);
    XReparentWindow(cli->dpy, w->core.self, par->core.self, w->core.x, w->core.y);
    par->fcore.panewin = (WinGenericPane *)w;
}


/***************************************************************************
* global functions
***************************************************************************/

/*
 * MakeMenu  -- create the pinned menu's menu window (around which we'll put
 *	a frame).    The window is mapped during the transition to normal
 *	state.
 */
WinMenu *
MakeMenu(dpy, junkwindow, menu, idx)
Display	*dpy;
WinGeneric *junkwindow;
Menu *menu;
int idx;
{
	WinMenu *w;
	Window win;
        unsigned long valuemask;
        XSetWindowAttributes attributes;
	Menu *originalMenu;
	Atom atomList[3];
	NewXSizeHints sizeHints;
	XWMHints wmHints;
#ifdef OW_I18N
	char *cts;
#endif

	/* save a pointer to the menu we're putting in the window.
	 * We will duplicate the structure later on and set the
	 * currentlyDisplayed flag in the original.
	 */
	originalMenu = menu;

	/* create the associated structure */
	w = MemNew(WinMenu);
	w->class = &classMenu;
	w->core.kind = WIN_MENU;
	w->core.children = NULL;
	w->core.x = menu->x;	
	w->core.y = menu->y;
	w->core.width = menu->width;
	w->core.height = menu->height - menu->titleHeight;
	w->core.dirtyconfig = CWX|CWY|CWWidth|CWHeight;
	w->core.exposures = NULL;

	/* create the actual window */
        attributes.event_mask = ButtonReleaseMask | ButtonPressMask | 
		ExposureMask | PropertyChangeMask;
        attributes.background_pixel = GRV.Bg1Color;
	attributes.cursor = GRV.MenuPointer;
        valuemask = CWEventMask | CWBackPixel | CWCursor;

        win = XCreateWindow(dpy, RootWindow(dpy, DefaultScreen(dpy)),
                        w->core.x, w->core.y,
			w->core.width, w->core.height,
                        0,
                        DefaultDepth(dpy, DefaultScreen(dpy)),
                        InputOutput,
                        DefaultVisual(dpy, DefaultScreen(dpy)),
                        valuemask,
                        &attributes);
	w->core.self = win;

	/* register the window */
	WIInstallInfo(w);

	/* first we set the properties defining what kind of OpenLook window it is */
	atomList[0] = AtomWTOther;
	atomList[1] = AtomMenuLimited;
	atomList[2] = (Atom) PIN_IN;
	XChangeProperty(dpy, win, AtomWinAttr, AtomWinAttr,
		32, PropModeReplace, (unsigned char *)atomList, 3);

	/* add a push-pin */
	atomList[0] = AtomDecorPin;
	atomList[1] = AtomDecorHeader;
	XChangeProperty(dpy, win, AtomDecorAdd, XA_ATOM,
		32, PropModeReplace, (unsigned char *)atomList, 2);

	/* set protocols */
	atomList[0] = AtomDeleteWindow;
	XChangeProperty(dpy, win, AtomProtocols, XA_ATOM,
		32, PropModeReplace, (unsigned char *)atomList, 1);

	/* now set the size hints */
	sizeHints.flags = USPosition | USSize;
	XChangeProperty(dpy, win, XA_WM_NORMAL_HINTS, XA_WM_SIZE_HINTS, 
		32, PropModeReplace, (unsigned char *)&sizeHints,
		sizeof(NewXSizeHints)/sizeof(long));

	/* and the wmHints */
	wmHints.flags = InputHint | StateHint | WindowGroupHint;
	wmHints.initial_state = NormalState;
	wmHints.input = False;
	   /*
	    * REMIND this is hack to make StateNew think we're a dependent 
	    * follower, and thus not iconify us.
	    */
	wmHints.window_group = NoFocusWin;

	XChangeProperty(dpy, win, XA_WM_HINTS, XA_WM_HINTS, 
		32, PropModeReplace, (unsigned char *)&wmHints,
		sizeof(XWMHints)/sizeof(long));

#ifdef OW_I18N
	cts = wcstoctsdup(menu->title);
	XChangeProperty(dpy, win, XA_WM_NAME, AtomCompoundText,
			8, PropModeReplace, (unsigned char *)cts,
			strlen(cts));
#else
	XStoreName(dpy, win, menu->title);
#endif

	/* create a private, duplicate menu structure */
	w->menu = (void *)MemNew(Menu);
	*(Menu *)(w->menu) = *menu;
	menu = (Menu *)(w->menu);
	menu->originalMenu = originalMenu;
	originalMenu->currentlyDisplayed = True;
#ifdef OW_I18N
	originalMenu->PinWin = w;
#endif
	menu->title = NULL;
	menu->x = menu->y = 0;
	menu->titleHeight = menu->titleWidth = 0;
	menu->height = w->core.height;
	menu->window = win;

	StateNew(dpy, win, False, w);

	return w;
}

void
MenuInit(dpy)
Display *dpy;
{
	classMenu.core.kind = WIN_MENU;
	classMenu.core.xevents[ButtonPress] = eventButtonPress;
	classMenu.core.xevents[ButtonRelease] = eventButtonRelease;
	classMenu.core.xevents[ClientMessage] = eventClientMessage;
	classMenu.core.xevents[PropertyNotify] = eventPropertyNotify;
	classMenu.core.xevents[Expose] = WinEventExpose;
	classMenu.core.focusfunc = focusselectMenu;
	classMenu.core.drawfunc = drawMenu;
	classMenu.core.destroyfunc = destroyMenu;
	classMenu.core.selectfunc = focusselectMenu;
	classMenu.core.newconfigfunc = newconfigMenu;
	classMenu.core.newposfunc = WinNewPosFunc;
	classMenu.core.setconfigfunc = WinSetConfigFunc;
	classMenu.core.createcallback = menuSetParent;
	classMenu.core.heightfunc = NULL;
	classMenu.core.widthfunc = NULL;
	classMenu.pcore.setsizefunc = NULL;

}
