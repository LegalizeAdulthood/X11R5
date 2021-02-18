/*
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) winicon.c 50.5 91/08/26 Crucible";
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
#include "slots.h"
#include "group.h"

/***************************************************************************
* global data
***************************************************************************/

extern GC IconNormalGC;
extern GC IconSelectedGC;
extern GC IconUnselectedGC;
extern GC IconBorderGC;
extern GC DrawBackgroundGC;
extern GC DrawSelectedGC;
extern GC RootGC;

extern Graphics_info	*olgx_gisnormal;
extern Menu 		IconFullMenu;


/***************************************************************************
* private data
***************************************************************************/

/* events in the icon window that are interesting */
#define ICON_EVENT_MASK        (ButtonPressMask | ButtonReleaseMask | \
				ExposureMask | Button1MotionMask)

/* border sizes, in pixels */
#define ICON_HORZBORDER 3
#define ICON_VERTBORDER 3

/* Class function vector */
static ClassIconFrame classIconFrame;

/***************************************************************************
* forward-declared functions
***************************************************************************/

/***************************************************************************
* private event functions
***************************************************************************/

static int
menuPressIcon(dpy,event,iconInfo)
Display *dpy;
XEvent *event;
WinIconFrame *iconInfo;
{
        if (iconInfo->fcore.menu)
            MenuShow(dpy,iconInfo,iconInfo->fcore.menu,event);
}

static int
selectDoubleClickIcon(dpy,event,iconInfo)
Display *dpy;
XEvent *event;
WinIconFrame *iconInfo;
{
	StateIconNorm(iconInfo->core.client);
}

static int
adjustClickIcon(dpy,event,iconInfo)
Display *dpy;
XEvent *event;
WinIconFrame *iconInfo;
{
        ToggleSelection(iconInfo->core.client, event->xbutton.time);
}

/***************************************************************************
* private functions
***************************************************************************/

/* selectDragIcon -- the user has held the select button down long enough
 * to initiate a drag.  Unpin the icon slot and start a window-move.
 */
static int
selectDragIcon(dpy, ev, iframe, lastpress)
Display *dpy;
XEvent *ev;
WinIconFrame *iframe;
XButtonEvent *lastpress;
{
	SlotFree(iframe);
	iframe->fManuallyPositioned = True;
	UserMoveWindows(dpy, lastpress, iframe);
}


/*
 * newconfigIcon -- compute a new configuration of icon window
 */
static int
newconfigIcon(winInfo, pxcre)
WinIconFrame *winInfo;
XConfigureRequestEvent *pxcre;
{
        Client *cli = winInfo->core.client;
        Display *dpy = cli->dpy;
        WinPane *winPane = (WinPane *)winInfo->fcore.panewin;
        int neww;
        int newh;
        WinGeneric *winDeco;

        neww = winInfo->fcore.panewin->core.width + 2*widthBothIcon(winInfo);
        newh = winInfo->fcore.panewin->core.height + heightTopIcon(winInfo) +
            heightBottomIcon(winInfo);

        if (neww != winInfo->core.width)
        {
                winInfo->core.width = neww;
                winInfo->core.dirtyconfig |= CWWidth;
        }

        if (newh != winInfo->core.height)
        {
                winInfo->core.height = newh;
                winInfo->core.dirtyconfig |= CWHeight;
        }

        if (winInfo->core.dirtyconfig)
        {
                (WinFunc(winPane,core.newposfunc))(winPane, 
			widthBothIcon(winInfo), heightTopIcon(winInfo));
        }

	if (winInfo->core.dirtyconfig & (CWWidth | CWHeight))
	{
                IconChangeName(winInfo,PANEWINOFCLIENT(cli));
	}

	return winInfo->core.dirtyconfig;
}

/*
 * newposIcon -- the icon is being moved to a new x,y location.  If the icon
 *	slot has not yet been allocated, do so; otherwise, blindly accept
 *	the x,y position.
 */
static int
newposIcon(winInfo,x,y)
WinIconFrame *winInfo;
int x,y;
{
	WinNewPosFunc(winInfo,x,y);
	if ((winInfo->iconslot == NULL) && 
	    (winInfo->core.client->groupmask != GROUP_DEPENDENT))
	{
		SlotAlloc(winInfo, winInfo->fManuallyPositioned, GRV.FSnapToGrid);
	}
	return winInfo->core.dirtyconfig;
}


static void
drawDashedRect(dpy, win, x, y, w, h)
    Display *dpy;
    Window win;
    int x, y, w, h;
{
    XPoint pts[5];

    pts[0].x = x;	pts[0].y = y;
    pts[1].x = x;	pts[1].y = y + h;
    pts[2].x = x + w;	pts[2].y = y + h;
    pts[3].x = x + w;	pts[3].y = y;
    pts[4].x = x;	pts[4].y = y;

    /*
     * The following is necessary because IconBorderGC uses the LineOnOffDash
     * line-style, which is faster than LineDoubleDash on some servers.
     */
    XDrawLines(dpy, win, IconUnselectedGC, pts, 5, CoordModeOrigin);
    XDrawLines(dpy, win, IconBorderGC, pts, 5, CoordModeOrigin);
}

/*
 * drawIconBorder -- based on the value of select, draw the border for an icon
 */
static void
drawIconBorder(dpy, winInfo, select)
Display *dpy;
WinIconFrame *winInfo;
Bool    select;
{
	GC highlightGC;
	int rectangle;    /* loop iterator for rectangles */
	Bool drawRectangle;
	int x, y;           /* values for use with */
	unsigned int width, height;  /* rectangle drawn for border */
	int olgxState;
	Window w = winInfo->core.self;
	
	x = y = 0;
	width = winInfo->core.width - 1;
	height = winInfo->core.height - 1;

	/*
	 * If 3D is used, give "borderless" icons.  Otherwise, give black and 
	 * white borders.
	 */
	if (select) {
	    XDrawRectangle(dpy, w, IconSelectedGC,
			   x, y, width, height );
	    XDrawRectangle(dpy, w, IconSelectedGC,
			   x+1, y+1, width-2, height-2 );
	    XDrawRectangle(dpy, w, IconSelectedGC,
			   x+2, y+2, width-4, height-4 );
	} else {
	    XDrawRectangle(dpy, w, IconUnselectedGC,
			   x, y, width, height);
	    if (GRV.F3dUsed) {
		XDrawRectangle(dpy, w, IconUnselectedGC, 
			       x+1, y+1, width-2, height-2);
	    } else {
#ifdef notdef
		XDrawRectangle(dpy, w, IconBorderGC, 
			       x+1, y+1, width-2, height-2);
#endif /* notdef */
		drawDashedRect(dpy, w, x+1, y+1, width-2, height-2);
	    }

	    XDrawRectangle(dpy, w, IconUnselectedGC,
			   x+2, y+2, width-4, height-4);
	}

#ifdef notdef
/*
 * This stuff was used for the attempt at 3D-look icons.
 * It has been abandoned in favor of the "borderless" icon look.
 */

        /* initial values for first rectangle */
        x = 0;
        y = 0;
        /* need to subtract one, based on how XDrawRectangle works */
        width = winInfo->core.width - 1;
        height = winInfo->core.height - 1;

        /* draw three rectangles for border */
	for ( rectangle = 0 ; rectangle < 3 ; rectangle++ )
	{
              switch( rectangle )
              {
              case 0:         /* outermost rectangle */
                      if ( GRV.F3dUsed )
                      {
                              if ( select )
                                      olgxState = OLGX_INVOKED;
                              else
                                      olgxState = OLGX_NORMAL;

                              olgx_draw_box( olgx_gisnormal,
                                             winInfo->core.self,
                                             x, y, width+1, height+1,
                                             olgxState, 0 );
                              drawRectangle = False;
                      }
                      else
                      {
                              highlightGC = select
                                              ? DrawSelectedGC
                                              : DrawBackgroundGC;
                              drawRectangle = True;
                      }
                      break;
              case 1:         /* middle rectangle */
                      if ( select )
                                      highlightGC = DrawSelectedGC;
                      else if ( GRV.F3dUsed )
                              highlightGC = DrawBackgroundGC;
                      else    /* REMIND eventually need to handle
                               * IconBorder resource when 2d & ColorDisplay
                               */
                              highlightGC = IconBorderGC;
                      drawRectangle = True;
                      break;
              case 2:         /* innermost rectangle */
              default:
                      highlightGC = select ? DrawSelectedGC
                                           : DrawBackgroundGC;
                      drawRectangle = True;
                      break;
              }

              if ( drawRectangle )
                      XDrawRectangle( dpy, winInfo->core.self, highlightGC,
                                      x, y, width, height );
              x++;
              y++;
              width -= 2;
              height -= 2;
      }
#endif /* notdef */
}

/*
 * drawIcon -- draw the icon window
 */
/*ARGSUSED*/  /* dpy arg will be used when multiple Displays supported */
static int
drawIcon(dpy, winInfo)
Display       *dpy;
WinIconFrame *winInfo;
{
	unsigned long	iconBgPixel;
	extern GC	IconSelectedGC, IconNormalGC;
	Window		frameWin = winInfo->core.self;

	XFillRectangle(dpy, frameWin, IconUnselectedGC, 0, 0,
		       winInfo->core.width, winInfo->core.height);

	/* draw icon name */
#ifdef OW_I18N
	XwcDrawString(dpy, frameWin, GRV.IconFontSetInfo.fs, IconNormalGC,
			winInfo->nameX,
#else
	XDrawString(dpy, frameWin, IconNormalGC, winInfo->nameX,
#endif
		    winInfo->nameY, winInfo->fcore.name, winInfo->nameLength);

	/* draw border */
	drawIconBorder(dpy, winInfo, winInfo->core.client->isSelected);
}


/*
 * DestroyIcon -- destroy the icon window resources and free any allocated
 *    data.
 */
static int
destroyIcon(dpy, winInfo)
Display       *dpy;
WinIconFrame *winInfo;
{
      /* free our data and throw away window */
      SlotFree(winInfo);
      ListDestroy(winInfo->core.children);
      XFree(winInfo->fcore.name);
      XUndefineCursor(dpy, winInfo->core.self);
      XDestroyWindow(dpy,winInfo->core.self);
      WIUninstallInfo(winInfo->core.self);
      MemFree(winInfo);
}


static int
heightTopIcon(win)
WinIconFrame	*win;
{
	WinIconPane *winPane = (WinIconPane *)(win->fcore.panewin);
	int basicbottom;

	if (winPane->iconClientWindow)
	{
	    return ICON_VERTBORDER;
	}
	else
	{
#ifdef OW_I18N
	    basicbottom = ICON_VERTBORDER
		+ GRV.IconFontSetInfo.fs_extents->max_ink_extent.height;
#else
	    basicbottom = ICON_VERTBORDER+GRV.IconFontInfo->max_bounds.ascent+
		GRV.IconFontInfo->max_bounds.descent;
#endif
	    if (winPane->core.height > ICON_WIN_HEIGHT - ICON_VERTBORDER - basicbottom)
		return ICON_VERTBORDER;
	    else
		return (ICON_WIN_HEIGHT-ICON_VERTBORDER-basicbottom-winPane->core.height)/2+ICON_VERTBORDER;
	}
}

static int
heightBottomIcon(win)
WinIconFrame	*win;
{
	WinIconPane *winPane = (WinIconPane *)(win->fcore.panewin);
	int basicbottom;

	if (winPane->iconClientWindow)
	{
	    return ICON_VERTBORDER;
	}
	else
	{
#ifdef OBSOLETE
	    basicbottom = ICON_VERTBORDER+GRV.IconFontInfo->max_bounds.ascent+
		GRV.IconFontInfo->max_bounds.descent;
#endif
	    basicbottom = winPane->core.height/2;
	    if (winPane->core.height > ICON_WIN_HEIGHT - ICON_VERTBORDER - basicbottom)
		return basicbottom;
	    else
		return (ICON_WIN_HEIGHT-ICON_VERTBORDER-basicbottom-winPane->core.height)/2+basicbottom;
	}
}

/* The icon pane has the same border width on either side, so this function
 * is used to calculate both border widths.
 */
static int
widthBothIcon(win)
WinIconFrame	*win;
{
	WinIconPane *winPane = (WinIconPane *)(win->fcore.panewin);

	if (winPane->iconClientWindow)
	{
	    return ICON_HORZBORDER;
	}
	else
	{
	    if (winPane->core.width < ICON_WIN_WIDTH - 2*ICON_HORZBORDER)
	    {
		return (ICON_WIN_WIDTH-winPane->core.width)/2;
	    }
	    else
		return ICON_HORZBORDER;
	}
}

/***************************************************************************
* global functions
***************************************************************************/

/*
 * MakeIcon  -- create the icon window. Return a WinIconFrame structure.
 *	Note that unlike most Make functions, icons are not mapped right
 *	away.
 */
WinIconFrame *
MakeIcon(cli,panewin,paneattrs)
Client *cli;
Window panewin;		
XWindowAttributes *paneattrs;
{
	Display		*dpy = cli->dpy;
	int		screen = cli->screen;
	WinIconFrame	*w;
        XSetWindowAttributes attributes;
        unsigned long   valuemask;
	XWMHints	*wmHints = cli->wmHints;

	/* create the window structure */
	w = MemNew(WinIconFrame);
	w->core.kind = WIN_ICON;
	w->class = &classIconFrame;
	w->core.parent = NULL;
	w->core.children = NULL;
	w->core.client = cli;
	w->core.width = ICON_WIN_WIDTH;
	w->core.height = ICON_WIN_HEIGHT;

	/* fill out  the associated structure */
	w->core.dirtyconfig = CWX|CWY|CWWidth|CWHeight;
	w->core.colormap = None;
	w->core.exposures = NULL;

	/* create the icon frame */
	valuemask = CWEventMask;
	attributes.event_mask = ICON_EVENT_MASK;
	w->core.self = XCreateWindow(dpy, RootWindow(dpy, screen),
			 w->core.x, w->core.y, 1, 1, 0,
			 DefaultDepth(dpy, screen), InputOutput,
			 DefaultVisual(dpy, screen), valuemask, &attributes);

	/* install icon frame in client */
	cli->iconwin = w;	/* REMIND: should be called cli->iconframe */
	
	/* set the position - either from position or icon slot */
	if (wmHints && (wmHints->flags & IconPositionHint)) 
	{
		w->core.x = wmHints->icon_x;
		w->core.y = wmHints->icon_y;
		w->fManuallyPositioned = True;
	}
	else
	{
		/* to be fixed up at config time */
		w->core.x = w->core.y = 0;
		w->fManuallyPositioned = False;
	}

	/* register the window */
	WIInstallInfo(w);

	/* set cursor for frame */
	XDefineCursor( dpy, w->core.self, GRV.IconPointer );

	IconChangeName(w,panewin);

        w->fcore.menu = &IconFullMenu;

	return w;
}

void
IconInit(dpy)
Display *dpy;
{
	classIconFrame.core.kind = WIN_ICON;
	classIconFrame.core.xevents[Expose] = WinEventExpose;
	classIconFrame.core.xevents[ButtonRelease] = GFrameEventButtonRelease;
	classIconFrame.core.xevents[MotionNotify] = GFrameEventMotionNotify;
	classIconFrame.core.xevents[ButtonPress] = GFrameEventButtonPress;
	classIconFrame.core.focusfunc = NULL;
	classIconFrame.core.drawfunc = drawIcon;
	classIconFrame.core.destroyfunc = destroyIcon;
	classIconFrame.core.selectfunc = GFrameSelect;
	classIconFrame.core.newconfigfunc = newconfigIcon;
	classIconFrame.core.newposfunc = newposIcon;
	classIconFrame.core.setconfigfunc = GFrameSetConfigFunc;
	classIconFrame.core.createcallback = NULL;
	classIconFrame.core.heightfunc = NULL;
	classIconFrame.core.widthfunc =  NULL;
	classIconFrame.fcore.heighttop = heightTopIcon;
	classIconFrame.fcore.heightbottom = heightBottomIcon;
	classIconFrame.fcore.widthleft = widthBothIcon;
	classIconFrame.fcore.widthright = widthBothIcon;
	classIconFrame.fcore.menuPress = menuPressIcon;
	classIconFrame.fcore.adjustPress = NULL;
	classIconFrame.fcore.adjustClick = adjustClickIcon;
	classIconFrame.fcore.selectPress = NULL;
	classIconFrame.fcore.selectClick = NULL;
	classIconFrame.fcore.selectDoubleClick = selectDoubleClickIcon;
	classIconFrame.fcore.selectDrag = selectDragIcon;
}

/*
 * DrawIconToWindowLines -- draw the open (close) lines when a window is
 *      becoming an icon or vice-versa
 */
void
DrawIconToWindowLines(dpy, iconInfo, winInfo)
Display *dpy;
WinPaneFrame *winInfo;
WinIconFrame *iconInfo;
{
        int     ii;

        XGrabServer(dpy);

        for(ii=0; ii < 3 ; ii++)
        {
                /* draw the close lines */
                XDrawLine(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootGC,
      	                        iconInfo->core.x,
                                iconInfo->core.y,
                                winInfo->core.x,
                                winInfo->core.y);
                XDrawLine(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootGC,
                                iconInfo->core.x,
                                (int)(iconInfo->core.y + iconInfo->core.height),
                                winInfo->core.x,
                                (int)(winInfo->core.y + winInfo->core.height));
                XDrawLine(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootGC,
                                (int)(iconInfo->core.x + iconInfo->core.width),
                                iconInfo->core.y,
                                (int)(winInfo->core.x + winInfo->core.width),
                                winInfo->core.y);
                XDrawLine(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootGC,
                                (int)(iconInfo->core.x + iconInfo->core.width),
                                (int)(iconInfo->core.y + iconInfo->core.height),
                                (int)(winInfo->core.x + winInfo->core.width),
                                (int)(winInfo->core.y + winInfo->core.height));

                XFlush(dpy);
                /* erase the close lines */
                XDrawLine(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootGC,
                                iconInfo->core.x,
                                iconInfo->core.y,
                                winInfo->core.x,
                                winInfo->core.y);
                XDrawLine(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootGC,
                                iconInfo->core.x,
                                (int)(iconInfo->core.y + iconInfo->core.height),
                                winInfo->core.x,
                                (int)(winInfo->core.y + winInfo->core.height));
                XDrawLine(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootGC,
                                (int)(iconInfo->core.x + iconInfo->core.width),
                                iconInfo->core.y,
                                (int)(winInfo->core.x + winInfo->core.width),
                                winInfo->core.y);
                XDrawLine(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootGC,
                                (int)(iconInfo->core.x + iconInfo->core.width),
                                (int)(iconInfo->core.y + iconInfo->core.height),
                                (int)(winInfo->core.x + winInfo->core.width),
                                (int)(winInfo->core.y + winInfo->core.height));
                XFlush(dpy);
        }

        XUngrabServer(dpy);
}

/* IconChangeName -- the client has decided to change the name of its
 * 	icon.  Handle this.
 */
void
IconChangeName(winIcon,pane)
WinIconFrame *winIcon;
Window pane;
{
	Client *cli = winIcon->core.client;
	Display *dpy = cli->dpy;

#ifdef OW_I18N
	if (!XwcGetIconName(dpy, pane, &(winIcon->fcore.name))
	 && !XwcFetchName(dpy, pane, &(winIcon->fcore.name)))
		winIcon->fcore.name = MemNewWString(GRV.DefaultWinName);

	winIcon->nameLength = wslen(winIcon->fcore.name);
	winIcon->nameWidth = XwcTextEscapement(GRV.IconFontSetInfo.fs,
					  winIcon->fcore.name, 
					  winIcon->nameLength);
#else
	if (!XGetIconName(dpy, pane, &(winIcon->fcore.name)))
		if (!XFetchName(dpy, pane, &(winIcon->fcore.name)))
			winIcon->fcore.name = MemNewString(GRV.DefaultWinName);

	winIcon->nameLength = strlen(winIcon->fcore.name);
	winIcon->nameWidth = XTextWidth(GRV.IconFontInfo, winIcon->fcore.name, 
		winIcon->nameLength);
#endif OW_I18N
	winIcon->nameX = (winIcon->core.width - winIcon->nameWidth)/2;
	/* put the title no lower than just above the selection hightlight area 
	 * (the selection box is three pixels wide: 3+1=4).  optimally, put
	 * it centered in the lower third of the icon window.  recall that
	 * descent goes below the Y value used for drawing.
	 */
#ifdef OW_I18N
	winIcon->nameY = MIN( (winIcon->core.height - 4
				- (GRV.IconFontSetInfo.fs_extents->max_ink_extent.height + 
				   GRV.IconFontSetInfo.fs_extents->max_ink_extent.y)),
			      5*winIcon->core.height/6 );
#else OW_I18N
	winIcon->nameY = MIN( (winIcon->core.height - 4
				- GRV.IconFontInfo->max_bounds.descent),
			      5*winIcon->core.height/6 );
#endif OW_I18N

	if (!winIcon->core.dirtyconfig)
		(WinFunc(winIcon,core.drawfunc))(dpy,winIcon);
}


/*
 * Set the icon's (x,y) location explicitly.  This information is typically
 * taken from the WM_HINTS structure.  Since the coordinates specify the 
 * absolute position of the icon pane, we must subtract the icon border to get 
 * the position if the icon frame.
 */
void
IconSetPos(win,x,y)
WinIconFrame *win;
int x,y;
{
    (WinFunc(win,core.newposfunc))(win,x-ICON_HORZBORDER,y-ICON_VERTBORDER);
}

/*
 * IconShow -- map an icon onto the screen, handling reparenting and
 * save-sets for icon panes.
 */
void
IconShow(cli, winIcon)
    Client *cli;
    WinIconFrame *winIcon;
{
    WinIconPane *pane = (WinIconPane *)winIcon->fcore.panewin;

    XReparentWindow(cli->dpy, pane->core.self, winIcon->core.self,
			pane->core.x, pane->core.y);
    XMapWindow(cli->dpy, pane->core.self);
    if (pane->iconClientWindow)
        XChangeSaveSet(cli->dpy, pane->core.self, SetModeInsert);
    XMapWindow(cli->dpy, winIcon->core.self);
}


/*
 * IconHide -- remove an icon from the screen, handling reparenting and
 * save-sets for icon panes.
 */
void
IconHide(cli, winIcon)
    Client *cli;
    WinIconFrame *winIcon;
{
    WinIconPane *pane = (WinIconPane *)winIcon->fcore.panewin;

    XUnmapWindow(cli->dpy, winIcon->core.self);
    XUnmapWindow(cli->dpy, pane->core.self);
    XReparentWindow(cli->dpy, pane->core.self, DefaultRootWindow(cli->dpy),
			winIcon->core.x + pane->core.x,
			winIcon->core.y + pane->core.y);
    if (pane->iconClientWindow)
        XChangeSaveSet(cli->dpy, pane->core.self, SetModeDelete);
}
