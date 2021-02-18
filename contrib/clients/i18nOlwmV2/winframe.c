/*
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) winframe.c 50.8 91/08/26 Crucible";
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

extern Menu FrameFullMenu;
extern Menu FrameLimMenu;
extern Menu FrameNormMenu;
extern Atom AtomLeftFooter;
extern Atom AtomRightFooter;
extern Atom AtomTakeFocus;
extern Atom AtomDfltBtn;
#ifdef OW_I18N
extern Atom AtomLeftIMStatus;
extern Atom AtomRightIMStatus;
#endif
extern Window NoFocusWin;
extern Graphics_info *olgx_gisnormal;
extern int Resize_height, Resize_width;

extern GC DrawBackgroundGC;
extern GC DrawNormalGC;
extern GC DrawReverseGC;
extern GC DrawBlackGC;
extern GC DrawBusyGC;

/***************************************************************************
* private data
***************************************************************************/

/* Events in the adornment window that are interesting. */
#define FRAME_EVENT_MASK        (ButtonPressMask | ButtonReleaseMask | \
                                 ExposureMask | Button1MotionMask | \
                                 EnterWindowMask | LeaveWindowMask | \
                                 SubstructureRedirectMask | \
                                 FocusChangeMask | PropertyChangeMask)

/* REMIND rework this stuff so it can handle different point sizes */
#define FRAME_OUTLINE_WIDTH     2
#define FRAME_SELECTED_WIDTH    3  

static ClassPaneFrame classPaneFrame;

/***************************************************************************
* forward-declared functions
***************************************************************************/

static void setTitleText();
static void setFooterText();
#ifdef OW_I18N
static void setIMStatusText();
#endif

/***************************************************************************
* sizing and decoration positioning functions
***************************************************************************/

/* decoration positioning */
/* ptSize - determine the point size we're working with.
 * REMIND this function is a hack which should be replaced with a
 * glyph font property.
 */
static int
ptSize(gis)
Graphics_info *gis;
{
#ifdef OBSOLETE
	switch (Resize_height)
	{
	case 10:
		return 10;
	case 11:
		return 12;
	case 12:
		return 14;
	case 14:
		return 19;
	default:
		return 10+((int)(0.5 * Resize_height));
	}
#endif
	return Pointsize_Glyph(gis);
}

static int
xposCloseButton(gis)
Graphics_info *gis;
{
	return ptSize(gis)+FRAME_OUTLINE_WIDTH;
}

static int
yposCloseButton(cli,gis)
Client *cli;
Graphics_info *gis;
{
    if (Abbrev_MenuButton_Height(gis) < headerHeight(cli,gis))
	return(ResizeArm_Height(gis)+(headerHeight(cli,gis)-Abbrev_MenuButton_Height(gis))/2);
    else
	return ResizeArm_Height(gis)+(ptSize(gis)>>4+2);
}

static int
xposPushPin(gis)
Graphics_info *gis;
{
	return xposCloseButton(gis);
}

static int
yposPushPin(cli,gis)
Client *cli;
Graphics_info *gis;
{
    if (PushPinOut_Height(gis) < headerHeight(cli,gis))
	return(ResizeArm_Height(gis)+(headerHeight(cli,gis)-PushPinOut_Height(gis))/2);
    else
	return ResizeArm_Height(gis)+(ptSize(gis)>>4);
}

static int
decoToTitle(gis)
Graphics_info *gis;
{
#ifdef NOTDEF
	return (2*xposCloseButton(gis))/3;
#endif
	return (ptSize(gis)>>2);
}

/* REMIND change this function to use olgx macros to extract
 * font size 
 */
static int
headerHeight(cli,gis)
Client *cli;
Graphics_info *gis;
{
#ifdef OW_I18N
	int fontht = GRV.TitleFontSetInfo.fs_extents->max_logical_extent.height;
#else
	int fontht = GRV.TitleFontInfo->ascent 
		     + GRV.TitleFontInfo->descent;
#endif
	return MAX(Abbrev_MenuButton_Height(gis),fontht+3);
}

/* REMIND this function should also be changed to use olgx macros */
static int
footerHeight(cli,gis)
Client *cli;
Graphics_info *gis;
{
#ifdef OW_I18N
	return GRV.TitleFontSetInfo.fs_extents->max_logical_extent.height
#else
	return GRV.TitleFontInfo->ascent + GRV.TitleFontInfo->descent
#endif
	    + ResizeArm_Height(gis);
}

#ifdef OW_I18N
static int
IMstatusHeight(cli,gis)
Client *cli;
Graphics_info *gis;
{
	return GRV.TitleFontSetInfo.fs_extents->max_logical_extent.height
		+ ResizeArm_Height(gis);
}
#endif OW_I18N

/* height/width functions */
static int
heightTopFrame(win)
WinPaneFrame *win;
{
	Client *cli = win->core.client;

	if (cli->wmDecors->flags & WMDecorationHeader)
		return headerHeight(cli,olgx_gisnormal)+2*ResizeArm_Height(olgx_gisnormal);
	else
		return ResizeArm_Height(olgx_gisnormal);
}

static int
heightBottomFrame(win)
WinPaneFrame *win;
{
#ifdef OW_I18N
	register int	result;
	Client *cli = win->core.client;

	result = 0;
	if (cli->wmDecors->flags & WMDecorationFooter)
		result += footerHeight(cli, olgx_gisnormal);
	if (cli->wmDecors->flags & WMDecorationIMStatus)
		result += IMstatusHeight(cli, olgx_gisnormal);
	result += ResizeArm_Height(olgx_gisnormal);

	return result;
#else OW_I18N
	Client *cli = win->core.client;

	if (cli->wmDecors->flags & WMDecorationFooter)
		return footerHeight(cli,olgx_gisnormal)+ResizeArm_Height(olgx_gisnormal);
	else
		return ResizeArm_Height(olgx_gisnormal);
#endif OW_I18N
}

static int 
widthRightFrame(win)
WinPaneFrame *win;
{
	return ResizeArm_Width(olgx_gisnormal);
}

static int 
widthLeftFrame(win)
WinPaneFrame *win;
{
	return ResizeArm_Width(olgx_gisnormal);
}

/***************************************************************************
* event-handling functions
***************************************************************************/

/*
 * handle events for the frame
 *
 * The reader should be aware of the fact that both the titlebar
 * window and the frame window are affected when the window's
 * focus and select state is changed.  The window manager was
 * being written before the OpenLook spec. was completed so it is
 * not the most efficient design and is rather awkward in places.
 *
 * The way focusing is handled deserves some attention.  For a
 * detailed description of how focusing should be handled see
 * the Inter-Client Communication Conventions Manual.  I'll give
 * a rough overview below.
 *
 * A client can use one of four input models:  No Input, Passive,
 * Locally Active, Globally Active.  When OLWM is in focus-follows-
 * mouse mode, focus is handled in a fairly straightforward manner.
 * When the cursor enters a window, signaled by the frame getting
 * an EnterNotify event, OLWM sets the focus like this:
 *
 *      No Input        - Do nothing.
 *      Passive         - Set the focus using XSetInputFocus.
 *      Locally Active  - Set the focus using XSetInputFocus.
 *      Globally Active - Set the focus by sending a message
 *                        to the client.
 *
 * When OLWM is in click-to-focus mode, focus is a bit more complicated.
 * This is due to the fact that the user can press down in the decoration
 * around the client window and drag the window, and NOT set the focus.
 * If the user just clicks, without moving, in the decoration then we
 * set the focus.  So, we don't know whether to set the focus until
 * the button release event.  But, if the user presses down in the
 * client window we must set the focus immediately.  This is so the user
 * can go to an xterm which does not have the focus, press down, (which
 * sets the focus), move the cursor, and release the button to select
 * some text, for example.  If the client is Passive or Locally Active,
 * we have a passive grab on the SELECT button.  This is how we handle
 * setting the focus upon button press and release for the four input
 * modes:
 *
 *      ButtonPress
 *              No Input        - Do Nothing.
 *              Passive         - If the press was in the client,
 *                                set the focus
 *              Locally Active  - If the press was in the client,
 *                                set the focus
 *              Globally Active - Do Nothing.
 *
 *      ButtonRelease
 *              No Input        - Do Nothing.
 *              Passive         - Set the focus.  We only get here
 *                                if the button press was NOT in
 *                                the client.
 *              Locally Active  - Set the focus.  We only get here
 *                                if the button press was NOT in
 *                                the client.
 *              Globally Active - Send a message to the client.
 */

/*
 * eventMapRequest -- the pane is go from iconic to normal states
 */
static int
eventMapRequest(dpy, event, frameInfo)
Display	*dpy;
XEvent	*event;
WinPaneFrame *frameInfo;
{
	Client *cli = frameInfo->core.client;

        /* transition from Iconic or Withdrawn */
        if (cli->wmState == IconicState ) {
            StateIconNorm(cli);
        } else {
#ifdef DEBUG
            ErrorWarning("withdrawn window giving map request?");
#endif /* DEBUG */
        }
}

/*
 * eventConfigureRequest -- the pane is trying to change configuration
 */
static int
eventConfigureRequest(dpy, event, frameInfo)
Display	*dpy;
XEvent	*event;
WinPaneFrame *frameInfo;
{
	Client *cli = frameInfo->core.client;
	WinPane		*winPane = (WinPane*)frameInfo->fcore.panewin;

	ClientConfigure(cli,winPane,event);
}

/*
 * selectClickFrame -- the select button has been clicked
 */
static int
selectClickFrame(dpy, event, frameInfo)
Display	*dpy;
XEvent	*event;
WinPaneFrame *frameInfo;
{
	WinPane		*winPane = (WinPane*)frameInfo->fcore.panewin;
	Window		panewindow = winPane->core.self;
	Client *cli = frameInfo->core.client;

        if (!GRV.FocusFollowsMouse)
        {
		ClientSetFocus(cli,True,event->xbutton.time);
        }  
}

/*
 * selectDoubleClickFrame -- the select button has been double-clicked
 */
static int
selectDoubleClickFrame(dpy, event, frameInfo)
Display	*dpy;
XEvent	*event;
WinPaneFrame *frameInfo;
{
    if (frameInfo->core.client->wmDecors->flags & WMDecorationResizeable) 
    {
        if (frameInfo->normfullsizefunc != NULL)
	    (frameInfo->normfullsizefunc)(frameInfo);
    }
}

/*
 * selectDragFrame -- the select button has been pressed and moved enough
 * 	to trigger a drag.
 */
static int
selectDragFrame(dpy, event, frameInfo, lastpress)
Display *dpy;
XEvent *event;
WinPaneFrame *frameInfo;
XButtonEvent *lastpress;
{
	UserMoveWindows(dpy, lastpress, frameInfo);
}

/*
 * menuPressFrame -- the menu button has been pressed
 */
static int
menuPressFrame(dpy,event,frameInfo)
Display *dpy;
XEvent *event;
WinPaneFrame *frameInfo;
{
    if (frameInfo->fcore.menu)
        MenuShow(dpy, frameInfo, frameInfo->fcore.menu, event );
}

/*
 * selectPressFrame -- the select or adjust button has been pressed
 */
static int
selectAdjustPressFrame(dpy, event, frameInfo)
Display	*dpy;
XEvent	*event;
WinPaneFrame *frameInfo;
{
	Client *cli = frameInfo->core.client;

        /* If the button press was in the
         * client, set the input focus.
         */
	ClientSetFocus(cli,False,event->xbutton.time);
}


/*
 * adjustClickFrame -- the adjust button has been pressed
 */
static int
adjustClickFrame(dpy, event, frameInfo)
Display	*dpy;
XEvent	*event;
WinPaneFrame *frameInfo;
{
	Client *cli = frameInfo->core.client;

        ToggleSelection(cli, event->xbutton.time);
        if (!GRV.FocusFollowsMouse)
        {
		ClientSetFocus(cli,True,event->xbutton.time);
        }  
}

/*
 * eventEnterNotify -- if in follow-mouse and pointer enters this tree of windows,
 *	set the focus 
 */
static int
eventEnterNotify(dpy, event, frameInfo)
Display	*dpy;
XEvent	*event;
WinPaneFrame *frameInfo;
{
	Client *cli = frameInfo->core.client;
	WinRoot *root;

        if (GRV.FocusFollowsMouse && (event->xcrossing.detail != NotifyInferior))
        {
		ClientSetFocus(cli,True,event->xcrossing.time);
        }  

	switch (event->xcrossing.detail) {
	case NotifyInferior:
	case NotifyNonlinear:
	    /*
	     * REMIND - it's sort of bogus that we must look up the client's 
	     * root window
	     */
	    root = WIGetInfo(RootWindow(cli->dpy, cli->screen));
	    ColorWindowCrossing(dpy, event, root);
	    break;
	}

	/* REMIND: does this imply and exit from the pane? If so, the
	 * pointer warping information is no longer valid and the 
	 * pointerIsWarped flag should be set to False.  On the other hand
	 * how will we distinguish the initial warp generated enterNotify
	 * from the enterFromPane enterNotify?
	 */
}

/*
 * eventLeaveNotify -- if in follow-mouse and pointer left this tree of windows,
 *	set the focus to the NoFocus window
 */
static int
eventLeaveNotify(dpy, event, frameInfo)
Display	*dpy;
XEvent	*event;
WinPaneFrame *frameInfo;
{
        if (GRV.FocusFollowsMouse && (event->xcrossing.detail != NotifyInferior))
        {
		    NoFocusTakeFocus(dpy,event->xcrossing.time);
        }  

	/* if pointer had been warped to pane, this leave notify
	 * indicates that the user has shifted their attention away
	 * from the "warp-to" location, and it is no longer necessary
	 * to worry about warping the pointer back.
	 */
	if (frameInfo->pointerIsWarped)
		frameInfo->pointerIsWarped = False;
}


static int
eventFocus(dpy, event, frameInfo)
    Display *dpy;
    XEvent *event;
    WinPaneFrame *frameInfo;
{
    if (event->xfocus.detail != NotifyInferior)
	WinCallFocus(frameInfo, (event->type == FocusIn));
}


/*
 * drawHeaderBusy3D
 * draw the header in the busy state in 3D look
 */
static void
drawHeaderBusy3D(dpy, win, cli, sel)
    Display *dpy;
    WinPaneFrame *win;
    Client *cli;
    Bool sel;
{
	Window self = win->core.self;
	int w = win->core.width;
	int armh = ResizeArm_Height(olgx_gisnormal);
	int armw = ResizeArm_Width(olgx_gisnormal);

	XFillRectangle(dpy, self, DrawBackgroundGC, armw, armh, 
		w-2*armw, heightTopFrame(win)-armh);
	XFillRectangle(dpy, self, DrawBusyGC, armw, armh, 
		w-2*armw, heightTopFrame(win)-armh);
	olgx_draw_text(olgx_gisnormal, self, win->fcore.name, win->titlex, 
		win->titley, 0, OLGX_NORMAL);
}


/*
 * drawHeaderBusy2D
 * draw the header in the busy state in 2D look
 */
static void
drawHeaderBusy2D(dpy, win, cli, sel)
    Display *dpy;
    WinPaneFrame *win;
    Client *cli;
    Bool sel;
{
	Window self = win->core.self;
	int w = win->core.width;
	int armh = ResizeArm_Height(olgx_gisnormal);
	int armw = ResizeArm_Width(olgx_gisnormal);

	/* fill in frame-colored area below titlebar */
	XFillRectangle(dpy, self, DrawBackgroundGC, armw, armh, 
		w-2*armw, heightTopFrame(win)-armh);
	XFillRectangle(dpy, self, DrawBusyGC, armw, armh, 
		w-2*armw, heightTopFrame(win)-armh);

	/* fill in window name in titlebar */
#ifdef OW_I18N
	XwcDrawString(dpy, self, GRV.TitleFontSetInfo.fs, DrawNormalGC,
		win->titlex, win->titley,
		win->fcore.name, win->nameLength);
	if (GRV.BoldFontEmulation == True)
		XwcDrawString(dpy, self, GRV.TitleFontSetInfo.fs, DrawNormalGC,
			win->titlex + 1, win->titley,
			win->fcore.name, win->nameLength);
#else
	XDrawString(dpy, self, DrawNormalGC, win->titlex, win->titley,
		win->fcore.name, win->nameLength);
#endif
}


/* drawHeaderFocusClick3D - draw the header, with focus, in 
 * 	click-to-focus mode (3D)
 */
static void
drawHeaderFocusClick3D(dpy, win, cli, sel)
Display *dpy;
WinPaneFrame *win;
Client *cli;
Bool sel;
{
	Window self = win->core.self;
	int w = win->core.width;
	int armh = ResizeArm_Height(olgx_gisnormal);

	XFillRectangle(dpy, self, DrawBackgroundGC, widthLeftFrame(win), 
		heightTopFrame(win)-armh, w-widthLeftFrame(win)-widthRightFrame(win), armh);

	olgx_draw_box(olgx_gisnormal, self, widthLeftFrame(win), armh, 
		w-widthLeftFrame(win)-widthRightFrame(win), heightTopFrame(win)-(2*armh),
		OLGX_INVOKED, True);

	olgx_draw_text(olgx_gisnormal, self, win->fcore.name, win->titlex, 
		win->titley, 0, OLGX_INVOKED);
#ifdef OW_I18N
	if (GRV.BoldFontEmulation == True)
		olgx_draw_text(olgx_gisnormal, self, win->fcore.name,
			win->titlex + 1, win->titley, 0, OLGX_INVOKED);
#endif
}

/* drawHeaderFocusFollow3D - draw the header, with focus, in 
 * focus-follow-mouse (3D mode)
 */
static void
drawHeaderFocusFollow3D(dpy, win, cli, sel)
Display *dpy;
WinPaneFrame *win;
Client *cli;
Bool sel;
{
	Window self = win->core.self;
	int outlinewidth = sel?FRAME_SELECTED_WIDTH:FRAME_OUTLINE_WIDTH;
	int w = win->core.width;
	int h = win->core.height;
	int armh = ResizeArm_Height(olgx_gisnormal);
	int armw = ResizeArm_Width(olgx_gisnormal);

	XFillRectangle(dpy, self, DrawBackgroundGC, armw, armh, 
		w-2*armw, heightTopFrame(win)-armh);

	olgx_draw_text(olgx_gisnormal, self, win->fcore.name, win->titlex, 
		win->titley, 0, OLGX_NORMAL);
#ifdef OW_I18N
        if (GRV.BoldFontEmulation == True)
		olgx_draw_text(olgx_gisnormal, self, win->fcore.name,
			win->titlex + 1, win->titley, 0, OLGX_NORMAL);
#endif

	if (cli->wmDecors->flags & WMDecorationResizeable)
		olgx_draw_text_ledge(olgx_gisnormal, self, Resize_width+1, 
			outlinewidth+1, w-(2*Resize_width)-2);
	else
		olgx_draw_text_ledge(olgx_gisnormal, self, widthLeftFrame(win), 
			outlinewidth+1, w-widthLeftFrame(win)-widthRightFrame(win));

	olgx_draw_text_ledge(olgx_gisnormal, self, widthLeftFrame(win),
		heightTopFrame(win)-3, w-widthLeftFrame(win)-widthRightFrame(win));
}

/* drawHeaderNoFocus3D - draw the header, without focus (3D mode)
 */
static void
drawHeaderNoFocus3D(dpy, win, cli, sel)
Display *dpy;
WinPaneFrame *win;
Client *cli;
Bool sel;
{
	Window self = win->core.self;
	int w = win->core.width;
	int armh = ResizeArm_Height(olgx_gisnormal);
	int armw = ResizeArm_Width(olgx_gisnormal);

	XFillRectangle(dpy, self, DrawBackgroundGC, armw, armh, 
		w-2*armw, heightTopFrame(win)-armh);

	olgx_draw_text(olgx_gisnormal, self, win->fcore.name, win->titlex, 
		win->titley, 0, OLGX_NORMAL);
#ifdef OW_I18N
        if (GRV.BoldFontEmulation == True)
		olgx_draw_text(olgx_gisnormal, self, win->fcore.name,
			win->titlex + 1, win->titley, 0, OLGX_NORMAL);
#endif
}

/* drawHeaderFocusClick2D - draw the header, with focus, in 
 * 	click-to-focus mode (2D)
 */
static void
drawHeaderFocusClick2D(dpy, win, cli, sel)
Display *dpy;
WinPaneFrame *win;
Client *cli;
Bool sel;
{
	Window self = win->core.self;
	int w = win->core.width;
	int armh = ResizeArm_Height(olgx_gisnormal);

	/* draw frame-colored rectangle below titlebar box */
	XFillRectangle(dpy, self, DrawBackgroundGC, widthLeftFrame(win), 
		heightTopFrame(win)-armh, w-widthLeftFrame(win)-widthRightFrame(win), armh);

	/* draw black titlebar to indicate 2d focus (XFillRectangle uses
	 * foreground color for fill)
	 */
	XFillRectangle(dpy, self, DrawBlackGC, widthLeftFrame(win), armh-1, 
		w-widthLeftFrame(win)-widthRightFrame(win), heightTopFrame(win)-(2*armh)+1);

	/* fill in window name */
#ifdef OW_I18N
	XwcDrawString(dpy, self, GRV.TitleFontSetInfo.fs, DrawReverseGC,
		win->titlex, win->titley, win->fcore.name, win->nameLength);
#ifdef OW_I18N
        if (GRV.BoldFontEmulation == True)
		XwcDrawString(dpy, self, GRV.TitleFontSetInfo.fs,
			DrawReverseGC, win->titlex + 1, win->titley,
			win->fcore.name, win->nameLength);
#endif
#else
	XDrawString(dpy, self, DrawReverseGC, win->titlex, win->titley,
		win->fcore.name, win->nameLength);
#endif
}

/* drawHeaderFocusFollow2D - draw the header, with focus, in 
 * focus-follow-mouse (2D mode)
 */
static void
drawHeaderFocusFollow2D(dpy, win, cli, sel)
Display *dpy;
WinPaneFrame *win;
Client *cli;
Bool sel;
{
	Window self = win->core.self;
	int outlinewidth = sel?FRAME_SELECTED_WIDTH:FRAME_OUTLINE_WIDTH;
	int w = win->core.width;
	int h = win->core.height;
	int armh = ResizeArm_Height(olgx_gisnormal);
	int armw = ResizeArm_Width(olgx_gisnormal);

	/* fill in frame-colored area below titlebar area */
	XFillRectangle(dpy, self, DrawBackgroundGC, armw, armh, 
		w-2*armw, heightTopFrame(win)-armh);

	/* fill in window name */
#ifdef OW_I18N
	XwcDrawString(dpy, self, GRV.TitleFontSetInfo.fs, DrawNormalGC,
		win->titlex, win->titley, win->fcore.name, win->nameLength);
#ifdef OW_I18N
        if (GRV.BoldFontEmulation == True)
		XwcDrawString(dpy, self, GRV.TitleFontSetInfo.fs, DrawNormalGC,
			win->titlex + 1, win->titley, win->fcore.name,
			win->nameLength);
#endif
#else
	XDrawString(dpy, self, DrawNormalGC, win->titlex, win->titley,
		win->fcore.name, win->nameLength);
#endif

	/* draw 2 pixel tall black focus indicator line above titlebar area 
	 * (without overwriting the resize corners)
	 */
	if (cli->wmDecors->flags & WMDecorationResizeable)
		XFillRectangle(dpy, self, DrawBlackGC, Resize_width+1, 
			outlinewidth+1, w-(2*Resize_width)-2, 2);
	else
		XFillRectangle(dpy, self, DrawBlackGC, widthLeftFrame(win), 
			outlinewidth+1, w-widthLeftFrame(win)-widthRightFrame(win), 2);

	/* draw 2 pixel tall black focus indicator line below titlebar area */
	XFillRectangle(dpy, self, DrawBlackGC, widthLeftFrame(win), heightTopFrame(win)-3,
		w-widthLeftFrame(win)-widthRightFrame(win), 2);
}

/* drawHeaderNoFocus2D - draw the header, without focus (2D mode)
 */
static void
drawHeaderNoFocus2D(dpy, win, cli, sel)
Display *dpy;
WinPaneFrame *win;
Client *cli;
Bool sel;
{
	Window self = win->core.self;
	int w = win->core.width;
	int armh = ResizeArm_Height(olgx_gisnormal);
	int armw = ResizeArm_Width(olgx_gisnormal);

	/* fill in frame-colored area below titlebar */
	XFillRectangle(dpy, self, DrawBackgroundGC, armw, armh, 
		w-2*armw, heightTopFrame(win)-armh);

	/* fill in window name in titlebar */
#ifdef OW_I18N
	XwcDrawString(dpy, self, GRV.TitleFontSetInfo.fs, DrawNormalGC,
		win->titlex, win->titley, win->fcore.name, win->nameLength);
        if (GRV.BoldFontEmulation == True)
		XwcDrawString(dpy, self, GRV.TitleFontSetInfo.fs, DrawNormalGC,
			win->titlex + 1, win->titley, win->fcore.name,
			win->nameLength);
#else
	XDrawString(dpy, self, DrawNormalGC, win->titlex, win->titley,
		win->fcore.name, win->nameLength);
#endif
}


/* drawFooter - draw the footer
 */
static void
drawFooter(dpy, win, cli)
Display *dpy;
WinPaneFrame *win;
Client *cli;
{
#ifdef OW_I18N
	Window self;
	int w;
	int h = win->core.height;
	int armh;
	int fy;
	int baseline;
	int margin;
	int footwidth;
	int qfootwidth;
	int gutter;
	int rstart, lmaxwidth, rmaxwidth;

	self = win->core.self;
	w = win->core.width;
	h = win->core.height;
	armh = ResizeArm_Height(olgx_gisnormal);
	fy = h - heightBottomFrame(win);
	if (cli->wmDecors->flags & WMDecorationIMStatus)
		fy += IMstatusHeight(cli, olgx_gisnormal);
	baseline = fy + ResizeArm_Height(olgx_gisnormal) - 
	           GRV.TitleFontSetInfo.fs_extents->max_logical_extent.y;
	margin = FRAME_OUTLINE_WIDTH + ptSize(olgx_gisnormal);
	footwidth = w - 2*margin;
	qfootwidth = footwidth / 4;
	gutter = ptSize(olgx_gisnormal);

#else OW_I18N
	Window self = win->core.self;
	int w = win->core.width;
	int h = win->core.height;
	int armh = ResizeArm_Height(olgx_gisnormal);
	int fy = h-heightBottomFrame(win);
	int baseline = fy + GRV.TitleFontInfo->ascent +
	    ResizeArm_Height(olgx_gisnormal);
	int margin = FRAME_OUTLINE_WIDTH + ptSize(olgx_gisnormal);
	int footwidth = w - 2*margin;
	int qfootwidth = footwidth / 4;
	int gutter = ptSize(olgx_gisnormal);
	int rstart, lmaxwidth, rmaxwidth;
#endif OW_I18N

	/* fill in frame-colored area above footer */
	XFillRectangle(dpy, self, DrawBackgroundGC, widthLeftFrame(win), fy,
		       w - widthLeftFrame(win) - widthRightFrame(win),
		       footerHeight(cli, olgx_gisnormal));

	/* REMIND we don't paint the "more arrow" if text is truncated */

	if ((win->leftfooterWidth + win->rightfooterWidth + gutter)
	    <= footwidth) {
	    /* room for both: no clipping */
	    lmaxwidth = win->leftfooterWidth;
	    rmaxwidth = win->rightfooterWidth;
	} else if (win->rightfooterWidth < qfootwidth) {
	    /* right footer takes less than 1/4 of the footer */
	    rmaxwidth = win->rightfooterWidth;
	    lmaxwidth = footwidth - rmaxwidth - gutter;
	} else if
	    ((win->leftfooterWidth) < (footwidth - qfootwidth - gutter)) {
	    /* left footer takes less than 3/4 of the footer */
	    lmaxwidth = win->leftfooterWidth;
	    rmaxwidth = footwidth - lmaxwidth - gutter;
	} else {
	    /* must truncate both */
	    rmaxwidth = qfootwidth;
	    lmaxwidth = footwidth - qfootwidth - gutter;
	}
	rstart = w - margin - rmaxwidth;

#ifdef OW_I18N
	if (win->leftfooter) {
	    olgx_draw_text(olgx_gisnormal, self, win->leftfooter,
			   margin, baseline, lmaxwidth, OLGX_NORMAL);
	    if (GRV.BoldFontEmulation == True)
		olgx_draw_text(olgx_gisnormal, self, win->leftfooter,
			   margin + 1, baseline, lmaxwidth, OLGX_NORMAL);
	}

	if (win->rightfooter) {
	    olgx_draw_text(olgx_gisnormal, self, win->rightfooter,
			   rstart, baseline, rmaxwidth, OLGX_NORMAL);
	    if (GRV.BoldFontEmulation == True)
		olgx_draw_text(olgx_gisnormal, self, win->rightfooter,
			   rstart + 1, baseline, rmaxwidth, OLGX_NORMAL);
	}
#else OW_I18N
	if (win->leftfooter)
	    olgx_draw_text(olgx_gisnormal, self, win->leftfooter,
			   margin, baseline, lmaxwidth, OLGX_NORMAL);

	if (win->rightfooter)
	    olgx_draw_text(olgx_gisnormal, self, win->rightfooter,
			   rstart, baseline, rmaxwidth, OLGX_NORMAL);
#endif OW_I18N
}

#ifdef OW_I18N
/*
 * drawIMStatus - draw the IMStatus, IMStatus window position is NOT
 * reflected by footer exsitence or not, but footer postion will
 * affect by IMStatus existance (this is bad assumption).
 */
static void
drawIMStatus(dpy, win, cli)
Display *dpy;
WinPaneFrame *win;
Client *cli;
{
	Window self;
	int w;
	int h;
	int armh;
	int fy;
	int baseline;
	int margin;
	int statuswidth;
	int qstatuswidth;
	int gutter;
	int rstart, lmaxwidth, rmaxwidth;

	self = win->core.self;
	w = win->core.width;
	h = win->core.height;
	armh = ResizeArm_Height(olgx_gisnormal);
	fy = h - heightBottomFrame(win);
	baseline = fy + ResizeArm_Height(olgx_gisnormal) - 
		GRV.TitleFontSetInfo.fs_extents->max_logical_extent.y;
	margin = FRAME_OUTLINE_WIDTH + ptSize(olgx_gisnormal);
	statuswidth = w - 2*margin;
	qstatuswidth = statuswidth / 4;
	gutter = ptSize(olgx_gisnormal);

	/* fill in frame-colored area above IMStatus */
	XFillRectangle(dpy, self, DrawBackgroundGC, widthLeftFrame(win), fy,
		       w - widthLeftFrame(win) - widthRightFrame(win),
		       IMstatusHeight(cli, olgx_gisnormal));

	/* REMIND we don't paint the "more arrow" if text is truncated */

	if ((win->leftIMstatusWidth + win->rightIMstatusWidth + gutter)
	    <= statuswidth) {
	    /* room for both: no clipping */
	    lmaxwidth = win->leftIMstatusWidth;
	    rmaxwidth = win->rightIMstatusWidth;
	} else if (win->leftIMstatusWidth < qstatuswidth) {
	    /* left IMStatus takes less than 1/4 of the status */
	    lmaxwidth = win->leftIMstatusWidth;
	    rmaxwidth = statuswidth - lmaxwidth - gutter;
	} else if
	    ((win->rightIMstatusWidth) < (statuswidth - qstatuswidth - gutter)) {
	    /* right IMStatus takes less than 3/4 of the IMStatus */
	    rmaxwidth = win->rightIMstatusWidth;
	    lmaxwidth = statuswidth - rmaxwidth - gutter;
	} else {
	    /* must truncate both */
	    lmaxwidth = qstatuswidth;
	    rmaxwidth = statuswidth - qstatuswidth - gutter;
	}
	rstart = w - margin - rmaxwidth;

	if (win->leftIMstatus) {
	    olgx_draw_text(olgx_gisnormal, self, win->leftIMstatus,
			   margin, baseline, lmaxwidth, OLGX_NORMAL);
	    if (GRV.BoldFontEmulation == True)
		olgx_draw_text(olgx_gisnormal, self, win->leftIMstatus,
			   margin + 1, baseline, lmaxwidth, OLGX_NORMAL);
	}

	if (win->rightIMstatus) {
	    olgx_draw_text(olgx_gisnormal, self, win->rightIMstatus,
			   rstart, baseline, rmaxwidth, OLGX_NORMAL);
	    if (GRV.BoldFontEmulation == True)
		olgx_draw_text(olgx_gisnormal, self, win->rightIMstatus,
			   rstart + 1, baseline, rmaxwidth, OLGX_NORMAL);
	}
}
#endif OW_I18N

/* drawBase2D - draw the outer border of the window (2D mode)
 */
static void
drawBase2D(dpy, win, cli, sel)
Display *dpy;
WinPaneFrame *win;
Client *cli;
Bool sel;
{
	Window self = win->core.self;
	int outlinewidth = sel?FRAME_SELECTED_WIDTH:FRAME_OUTLINE_WIDTH;
	int w = win->core.width;
	int h = win->core.height;
	int armh = ResizeArm_Height(olgx_gisnormal);
	int armw = ResizeArm_Width(olgx_gisnormal);

	/* top base area */
	XFillRectangle(dpy, self, DrawBackgroundGC, outlinewidth, 
		outlinewidth, 	w-(2*outlinewidth), armh-outlinewidth);
	XFillRectangle(dpy, self, DrawBlackGC, 0, 0, 
		w, outlinewidth);

	/* bottom base area */
	XFillRectangle(dpy, self, DrawBackgroundGC, outlinewidth, h-armh,
		w-(2*outlinewidth), armh-outlinewidth);
	XFillRectangle(dpy, self, DrawBlackGC, 0, h-outlinewidth, 
		w, outlinewidth);

	/* left base area */
	XFillRectangle(dpy, self, DrawBackgroundGC, outlinewidth, armh,
		armw-outlinewidth, h-(2*armh));
	XFillRectangle(dpy, self, DrawBlackGC, 0, outlinewidth,
		outlinewidth, h-(2*outlinewidth));

	/* right base area */
	XFillRectangle(dpy, self, DrawBackgroundGC, w-armw, armh,
		armw-outlinewidth, h-(2*armh));
	XFillRectangle(dpy, self, DrawBlackGC, w-outlinewidth, outlinewidth,
		outlinewidth, h-(2*outlinewidth));
}

/* drawBase3D - draw the outer border of the window (3D mode)
 */
static void
drawBase3D(dpy, win, cli, sel)
Display *dpy;
WinPaneFrame *win;
Client *cli;
Bool sel;
{
	olgx_draw_box(olgx_gisnormal, win->core.self, 0, 0, win->core.width, 
		win->core.height, OLGX_NORMAL, True);
	if (sel) {
	    olgx_draw_box(olgx_gisnormal, win->core.self, 1, 1,
			  win->core.width-2, win->core.height-2,
			  OLGX_NORMAL, False, 0);
	}
}

/*
 * drawFrame -- draw the frame window
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static int
drawFrame(dpy, winInfo)
Display	*dpy;
WinPaneFrame *winInfo;
{
    Client *cli = winInfo->core.client;

    if (GRV.F3dUsed && GRV.F3dFrames)
        drawBase3D(dpy, winInfo, cli, cli->isSelected);
    else
        drawBase2D(dpy, winInfo, cli, cli->isSelected);

    if (cli->wmDecors->flags & WMDecorationHeader)
    {
	if (cli->isBusy) {
	    if (GRV.F3dUsed)
		drawHeaderBusy3D(dpy, winInfo, cli, cli->isSelected);
	    else
		drawHeaderBusy2D(dpy, winInfo, cli, cli->isSelected);
	}
        else if (cli->isFocus)
            if (GRV.FocusFollowsMouse)
                if (GRV.F3dUsed)
                    drawHeaderFocusFollow3D(dpy, winInfo, cli, cli->isSelected);
		else
		    drawHeaderFocusFollow2D(dpy, winInfo, cli, cli->isSelected);
	    else
                if (GRV.F3dUsed)
		    drawHeaderFocusClick3D(dpy, winInfo, cli, cli->isSelected);
		else
		    drawHeaderFocusClick2D(dpy, winInfo, cli, cli->isSelected);
	else
	    if (GRV.F3dUsed)
		drawHeaderNoFocus3D(dpy, winInfo, cli);
	    else
		drawHeaderNoFocus2D(dpy, winInfo, cli);
    }

    if (cli->wmDecors->flags & WMDecorationFooter)
    {
        drawFooter(dpy, winInfo, cli); /* no difference between 2D and 3D */
    }
#ifdef OW_I18N
    if (cli->wmDecors->flags & WMDecorationIMStatus)
    {
        drawIMStatus(dpy, winInfo, cli); /* no difference between 2D and 3D */
    }
#endif
}


/*
 * DestroyFrame -- destroy the frame window resources and free any allocated
 *	data.
 */
static int
destroyFrame(dpy, winInfo)
Display	*dpy;
WinPaneFrame *winInfo;
{
	/* free our data and throw away window */
	ListDestroy(winInfo->core.children);
	XFree(winInfo->fcore.name);
	XFree(winInfo->leftfooter);
	XFree(winInfo->rightfooter);
#ifdef OW_I18N
	XFree(winInfo->leftIMstatus);
	XFree(winInfo->rightIMstatus);
#endif
	XUndefineCursor(dpy, winInfo->core.self);
	XDestroyWindow(dpy,winInfo->core.self);
	WIUninstallInfo(winInfo->core.self);
	MemFree(winInfo);
}

/*
 * newconfigFrame -- compute a new configuration of frame window
 */
static int
newconfigFrame(winInfo, pxcre)
WinPaneFrame *winInfo;
XConfigureRequestEvent *pxcre;
{
	Client *cli = winInfo->core.client;
	Display *dpy = cli->dpy;
	WinPane *winPane = (WinPane *)winInfo->fcore.panewin;
	int neww;
	int newh;
	WinGeneric *winDeco;

	neww = winInfo->fcore.panewin->core.width + widthLeftFrame(winInfo) + 
	    widthRightFrame(winInfo);
	newh = winInfo->fcore.panewin->core.height + heightTopFrame(winInfo) + 
	    heightBottomFrame(winInfo);

	if (neww != winInfo->core.width)
	{
		winInfo->core.width = neww;
		winInfo->core.dirtyconfig |= CWWidth;
		setTitleText(cli,dpy,winInfo,winPane->core.self);
		setFooterText(cli,dpy,winInfo,winPane->core.self);
#ifdef OW_I18N
		setIMStatusText(cli,dpy,winInfo,winPane->core.self);
#endif
	}

	if (newh != winInfo->core.height)
	{
		winInfo->core.height = newh;
		winInfo->core.dirtyconfig |= CWHeight;
	}
	
	if (winInfo->core.dirtyconfig)
	{
		(WinFunc(winPane,core.newposfunc))(winPane, widthLeftFrame(winInfo), 
			heightTopFrame(winInfo));
		winDeco = winInfo->winDeco;
	        if (cli->wmDecors->flags & WMDecorationPushPin)
       		{
		    (WinFunc(winDeco,core.newposfunc))(winDeco,
			xposPushPin(olgx_gisnormal),
			yposPushPin(cli,olgx_gisnormal));
        	}
        	if (cli->wmDecors->flags & WMDecorationCloseButton)
        	{
		    (WinFunc(winDeco,core.newposfunc))(winDeco,
			xposCloseButton(olgx_gisnormal),
			yposCloseButton(cli,olgx_gisnormal));
        	}
		if (cli->isBusy && winInfo->winBusy != NULL)
		    (WinFunc(winInfo->winBusy,core.newposfunc))(winInfo->winBusy, 
			widthLeftFrame(winInfo), heightTopFrame(winInfo));
	}

	return winInfo->core.dirtyconfig;
}

/* 
 * makeSpecials -- make any special mark windows (pushpin, close button)
 */
static void
makeSpecials(cli,dpy,wf,panewin,wid,high)
Client *cli;
Display *dpy;
WinPaneFrame *wf;
Window panewin;
int wid,high;
{
	NewXSizeHints *hints;
	int decorWidth;

	/* Make resize children */
	if (cli->wmDecors->flags & WMDecorationResizeable)
	{
		MakeResize(dpy, wf, upleft, 0, 0);
		MakeResize(dpy, wf, upright, wid-Resize_width, 0);
		MakeResize(dpy, wf, lowleft, 0, high-Resize_height);
		MakeResize(dpy, wf, lowright, wid-Resize_width, high-Resize_height);
	}

        /* Here we figure out, among other things, how much space
         * the decorations will take up in the title bar.  Also, we
         * set the leftmost point at which the title string can be
         * drawn without interfering with the decoration, if any,
         * on the left hand side of the title bar.  'frame->titleOff'
         *
	 * A window cannot have both a close button and a pushpin.  So, if
	 * they ask for both, they only get the pushpin.  This mutual
	 * exclusion was taken care of in GetOLWinDecors in states.c
         */
        if (cli->wmDecors->flags & WMDecorationPushPin)
        {
		(WinPushPin *)(wf->winDeco) = 
			MakePushPin(dpy,wf,panewin,xposPushPin(olgx_gisnormal),
				    yposPushPin(cli,olgx_gisnormal));
                decorWidth = xposPushPin(olgx_gisnormal) + 
			PushPinOut_Width(olgx_gisnormal);
        }

        if (cli->wmDecors->flags & WMDecorationCloseButton)
        {
		(WinButton *)(wf->winDeco) = 
			MakeButton(dpy,wf,xposCloseButton(olgx_gisnormal),
				   yposCloseButton(cli,olgx_gisnormal));
                decorWidth = xposCloseButton(olgx_gisnormal) +
			Abbrev_MenuButton_Width(olgx_gisnormal) ;
        }

	wf->titleOff = decorWidth + decoToTitle(olgx_gisnormal);

        /* Add decoration's size to minimum width of window. */
        hints = cli->normHints;
        hints->min_width = MAX(decorWidth, hints->min_width);
}

/* setTitleText - extract the name of the window and set up the titlebar
 * 	area
 */
static void
setTitleText(cli,dpy,w,panewin)
Client *cli;
Display *dpy;
WinPaneFrame *w;
Window panewin;
{
	int fontheight;
	int availwidth;
#ifdef OW_I18N
	char *winname;
	wchar_t *ptr, *tmp;
#else
	char *ptr, *tmp;
#endif

        /* Get window name */
	if (w->fcore.name)
		MemFree(w->fcore.name);

#ifdef OW_I18N
	if (XwcFetchName(dpy, panewin, &tmp) == 0 || tmp == NULL) {
	    w->fcore.name = MemNewWString(GRV.DefaultWinName);
	} else {
	    /*
	     * Somewhat nitpicky.  We copy the string with MemNewString to 
	     * ensure that we can free it with MemFree (above), because the 
	     * string returned by XFetchName must be freed with XFree.
	     */
	    w->fcore.name = MemNewWString(tmp);
	    XFree(tmp);
	}
#else OW_I18N
	if (XFetchName(dpy, panewin, &tmp) == 0 || tmp == NULL) {
	    w->fcore.name = MemNewString(GRV.DefaultWinName);
	} else {
	    /*
	     * Somewhat nitpicky.  We copy the string with MemNewString to 
	     * ensure that we can free it with MemFree (above), because the 
	     * string returned by XFetchName must be freed with XFree.
	     */
	    w->fcore.name = MemNewString(tmp);
	    XFree(tmp);
	}
#endif OW_I18N

#ifdef OW_I18N
	w->nameLength = wslen(w->fcore.name);
	w->nameWidth = XwcTextEscapement(GRV.TitleFontSetInfo.fs,
				    w->fcore.name, w->nameLength);
#else
	w->nameLength = strlen(w->fcore.name);
	w->nameWidth = XTextWidth(GRV.TitleFontInfo, w->fcore.name, w->nameLength);
#endif

#ifdef NOTDEF
	availwidth = w->core.width - widthLeftFrame(w) - widthRightFrame(w) - 
		w->titleOff;
#endif
	availwidth = w->core.width - widthRightFrame(w) - w->titleOff;
	availwidth = MAX(0,availwidth);

        if (availwidth < w->nameWidth)
        {
                /* Must truncate the title.
                 * First we see if there is a colon and truncate
                 * all the chars up to the colon.
                 */
#ifdef OW_I18N
                if (ptr = wschr(w->fcore.name, ':'))
#else
                if (ptr = strchr(w->fcore.name, ':'))
#endif
                {
                        ptr++; /* after ':' */
                        w->nameLength -= ptr - w->fcore.name;
			tmp = w->fcore.name;
#ifdef OW_I18N
                        w->fcore.name = MemNewWString(ptr);
#else
                        w->fcore.name = MemNewString(ptr);
#endif
			MemFree(tmp);
#ifdef OW_I18N
                        w->nameWidth = XwcTextEscapement(
				GRV.TitleFontSetInfo.fs,
				w->fcore.name, w->nameLength );
#else
                        w->nameWidth = XTextWidth( GRV.TitleFontInfo, w->fcore.name, 
				w->nameLength);
#endif
                }
        }

        while (availwidth < w->nameWidth)
        {
                /* Truncate the title from the right. */
#ifdef OW_I18N
                w->fcore.name[wslen(w->fcore.name) - 1] = '\0';
                w->nameLength--;
                w->nameWidth = XwcTextEscapement( GRV.TitleFontSetInfo.fs,
				w->fcore.name, w->nameLength);
#else
                w->fcore.name[strlen(w->fcore.name) - 1] = '\0';
                w->nameLength--;
                w->nameWidth = XTextWidth( GRV.TitleFontInfo, w->fcore.name, 
					   w->nameLength);
#endif
        }


        /* Center that title. */
        w->titlex = w->titleOff + (availwidth - w->nameWidth)/2;
#ifdef OW_I18N
        w->titley = 2 + ResizeArm_Height(olgx_gisnormal) - 
		    GRV.TitleFontSetInfo.fs_extents->max_ink_extent.y;
#else
        w->titley = GRV.TitleFontInfo->max_bounds.ascent + 2 + ResizeArm_Height(olgx_gisnormal);
#endif
}

/* setFooterText - extract the footer texts and determine where to draw them
 */
static void
setFooterText(cli,dpy,w,panewin)
Client *cli;
Display *dpy;
WinPaneFrame *w;
Window panewin;
{
        Atom actType;
        int actFormat;
#ifndef OW_I18N
        unsigned long nItems, remain;
#endif
	int status;
#ifdef OW_I18N
	wchar_t *p;
#endif

	/* REMIND add in truncation later */

        /* Read the left footer, if any. */
	if (w->leftfooter != NULL)
		XFree(w->leftfooter);
#ifdef OW_I18N
	if (XwcGetTextWindowProperty(AtomLeftFooter, dpy,
				     panewin, &(w->leftfooter)) == 0)
		w->leftfooter = NULL;
#else OW_I18N
	w->leftfooter = GetWindowProperty(dpy, panewin, AtomLeftFooter, 0L, 
		FOOTLEN, XA_STRING, 0, &nItems, &remain);
#endif OW_I18N
	if (w->leftfooter == NULL)
		w->leftfooterWidth = w->leftfooterLength = 0;
	else
	{
#ifdef OW_I18N
		/*
		 * Footer does not necessary to be i18n (wide char
		 * support), but new drawing function require to be in
		 * wide char form.  We will not deal with compound
		 * text from client.
		 */
		w->leftfooterLength = wslen(w->leftfooter);
		w->leftfooterWidth = XwcTextEscapement(GRV.TitleFontSetInfo.fs, 
			w->leftfooter, w->leftfooterLength);
#else OW_I18N
		w->leftfooterLength = strlen(w->leftfooter);
		w->leftfooterWidth = XTextWidth(GRV.TitleFontInfo, 
			w->leftfooter, w->leftfooterLength);
#endif OW_I18N
	}

        /* Read the right footer, if any. */
	if (w->rightfooter != NULL)
		XFree(w->rightfooter);
#ifdef OW_I18N
	if (XwcGetTextWindowProperty(AtomRightFooter, dpy,
				     panewin, &(w->rightfooter)) == 0)
		w->rightfooter = NULL;
#else OW_I18N
	w->rightfooter = GetWindowProperty(dpy, panewin, AtomRightFooter, 0L, 
		FOOTLEN, XA_STRING, 0, &nItems, &remain);
#endif OW_I18N
	if (w->rightfooter == NULL)
		w->rightfooterWidth = w->rightfooterLength = 0;
	else
	{
#ifdef OW_I18N
		w->rightfooterLength = wslen(w->rightfooter);
		w->rightfooterWidth = XwcTextEscapement(GRV.TitleFontSetInfo.fs, 
			w->rightfooter, w->rightfooterLength);
#else OW_I18N
		w->rightfooterLength = strlen(w->rightfooter);
		w->rightfooterWidth = XTextWidth(GRV.TitleFontInfo, 
			w->rightfooter, w->rightfooterLength);
#endif OW_I18N
	}

}

#ifdef OW_I18N
/* setIMStatusText - extract the IMStatus texts and determine where to draw them
 */
static void
setIMStatusText(cli,dpy,w,panewin)
Client *cli;
Display *dpy;
WinPaneFrame *w;
Window panewin;
{
        Atom actType;
        int actFormat;
        unsigned long nItems, remain;
	int status;
	wchar_t *p;

	/* REMIND add in truncation later */

        /* Read the left IMStatus, if any. */
	if (w->leftIMstatus != NULL)
		XFree(w->leftIMstatus);
	if (XwcGetTextWindowProperty(AtomLeftIMStatus, dpy,
				     panewin, &(w->leftIMstatus)) == 0)
	{
		w->leftIMstatus = NULL;
		w->leftIMstatusWidth = w->leftIMstatusLength = 0;
	}
	else
	{
		w->leftIMstatusLength = wslen(w->leftIMstatus);
		w->leftIMstatusWidth = XwcTextEscapement(GRV.TitleFontSetInfo.fs, 
			w->leftIMstatus, w->leftIMstatusLength);
	}

        /* Read the right IMStatus, if any. */
	if (w->rightIMstatus != NULL)
		XFree(w->rightIMstatus);
	if (XwcGetTextWindowProperty(AtomRightIMStatus, dpy,
				     panewin, &(w->rightIMstatus)) == 0)
	{
		w->rightIMstatus = NULL;
		w->rightIMstatusWidth = w->rightIMstatusLength = 0;
	}
	else
	{
		w->rightIMstatusLength = wslen(w->rightIMstatus);
		w->rightIMstatusWidth = XwcTextEscapement(
			GRV.TitleFontSetInfo.fs, 
			w->rightIMstatus, w->rightIMstatusLength);
	}
}
#endif OW_I18N

/*
 * setupGrabs
 *
 * Set up any pointer grabs for this window, as appropriate for the focus mode
 * (follow-mouse or click) and for the focus model (Passive, Globally Active,
 * etc.) of this window.  This is important for ClickFocus mode for Passive
 * and Locally Active clients.  If the user clicks over the pane window, we
 * get the event, set the focus, and replay the event, thus passing the event
 * through.
 * 
 * REMIND we need to remove explicit reference to Buttons 1 and 2.
 */
void
FrameSetupGrabs(cli, win, activate)
    Client *cli;
    Window win;
    Bool activate;
{
    if (!GRV.FocusFollowsMouse) {
	switch (cli->focusMode) {
	case Passive:
	case LocallyActive:
	    if (activate) {
		XGrabButton(cli->dpy, Button1, 0, win, True,
		    ButtonPressMask | ButtonReleaseMask | Button1MotionMask,
		    GrabModeSync, GrabModeSync, None, None);
		XGrabButton(cli->dpy, Button2, 0, win, True,
		    ButtonPressMask | ButtonReleaseMask | Button1MotionMask,
		    GrabModeSync, GrabModeSync, None, None);
	    } else {
		XUngrabButton(cli->dpy, Button1, 0, win);
		XUngrabButton(cli->dpy, Button2, 0, win);
	    }
	    break;
	}
    }
}


/***************************************************************************
* global functions
***************************************************************************/

/*
 * MakeFrame  -- create the frame window. Return a WinPaneFrame structure.
 *	Note that unlike most Make functions, frames are not mapped right
 *	away.
 */
WinPaneFrame *
MakeFrame(cli,panewin,paneattrs)
Client *cli;
Window panewin;		
XWindowAttributes *paneattrs;
{
	Display *dpy = cli->dpy;
	int screen = cli->screen;
	WinPaneFrame *w;
	Window win;
        XSetWindowAttributes attributes;
        unsigned long   valuemask;
	int wid, high;

	/* create the frame window */
        valuemask = CWEventMask | CWSaveUnder | CWBackPixmap | CWCursor;
        attributes.event_mask = FRAME_EVENT_MASK;
        attributes.save_under = paneattrs->save_under;
	attributes.background_pixmap = None;
	attributes.cursor = GRV.TargetPointer;

        win = XCreateWindow(dpy, RootWindow(dpy, screen),
                         0, 0, 1, 1,
                         0,
                         DefaultDepth(dpy, screen),
                         InputOutput,
                         DefaultVisual(dpy, screen),
                         valuemask,
                         &attributes);

	/* create the associated structure */
	w = MemNew(WinPaneFrame);
	w->class = &classPaneFrame;
	w->core.self = win;
	w->core.kind = WIN_FRAME;
	w->core.parent = NULL;
	w->core.children = NULL;
	w->core.client = cli;
	/* x and y set later */

	/* compute size of frame from pane */
	wid = paneattrs->width + widthLeftFrame(w) + widthRightFrame(w);
	high = paneattrs->height + heightTopFrame(w) + heightBottomFrame(w);

	w->core.width = wid;	/* these get fixed up at config time */
	w->core.height = high;
	w->core.stack_mode = Above;
	w->core.dirtyconfig = CWX | CWY | CWHeight | CWWidth | CWStackMode;
	w->core.colormap = None;
	w->core.exposures = NULL;

	/* REMIND this call appears to be redundant */
	FrameSetPosFromPane(w, paneattrs->x, paneattrs->y);

	cli->framewin = w;

	/* register the window */
	WIInstallInfo(w);

	/* if there's any special marks, make them */
	makeSpecials(cli,dpy,w,panewin,wid,high);

	/* set up the titlebar */
	if (cli->wmDecors->flags & WMDecorationHeader)
		setTitleText(cli,dpy,w,panewin);

	/* set up the footer */
	if (cli->wmDecors->flags & WMDecorationFooter)
		setFooterText(cli,dpy,w,panewin);
#ifdef OW_I18N
	/* set up the status */
	if (cli->wmDecors->flags & WMDecorationIMStatus)
		setIMStatusText(cli,dpy,w,panewin);
#endif OW_I18N

        /* Determine which menu should come up when menus are requested
         * for this frame. */
        switch(cli->wmDecors->menu_type)
        {
                case MENU_FULL:
                        w->fcore.menu = &FrameFullMenu;
                        break;

                case MENU_LIMITED:
                        w->fcore.menu = &FrameLimMenu;
                        break;

                case MENU_NONE:
                        w->fcore.menu = NULL;
                        break;
        }

	FrameSetupGrabs(cli, win, True);

	/* set the full/normal size to transition to full size 
	 * on first activation */
	w->normfullsizefunc = FrameFullSize;
	w->restoreSet = False;

	return w;
}

void
FrameInit(dpy)
Display *dpy;
{
	classPaneFrame.core.kind = WIN_FRAME;

	classPaneFrame.core.xevents[MapRequest] = eventMapRequest;
	classPaneFrame.core.xevents[ConfigureRequest] = eventConfigureRequest;
	classPaneFrame.core.xevents[Expose] = WinEventExpose;
	classPaneFrame.core.xevents[ButtonRelease] = GFrameEventButtonRelease;
	classPaneFrame.core.xevents[MotionNotify] = GFrameEventMotionNotify;
	classPaneFrame.core.xevents[ButtonPress] = GFrameEventButtonPress;
	classPaneFrame.core.xevents[EnterNotify] = eventEnterNotify;
	classPaneFrame.core.xevents[LeaveNotify] = eventLeaveNotify;
	classPaneFrame.core.xevents[FocusIn] = eventFocus;
	classPaneFrame.core.xevents[FocusOut] = eventFocus;

	classPaneFrame.core.focusfunc = GFrameFocus;
	classPaneFrame.core.drawfunc = drawFrame;
	classPaneFrame.core.destroyfunc = destroyFrame;
	classPaneFrame.core.selectfunc = GFrameSelect;
	classPaneFrame.core.newconfigfunc = newconfigFrame;
	classPaneFrame.core.newposfunc = WinNewPosFunc;
	classPaneFrame.core.setconfigfunc = GFrameSetConfigFunc;
	classPaneFrame.core.createcallback = NULL;
	classPaneFrame.core.heightfunc = NULL;
	classPaneFrame.core.widthfunc = NULL;
	classPaneFrame.fcore.heighttop = heightTopFrame;
	classPaneFrame.fcore.heightbottom = heightBottomFrame;
	classPaneFrame.fcore.widthleft = widthRightFrame;
	classPaneFrame.fcore.widthright = widthLeftFrame;
	classPaneFrame.fcore.menuPress = menuPressFrame;
	classPaneFrame.fcore.adjustPress = selectAdjustPressFrame;
	classPaneFrame.fcore.adjustClick = adjustClickFrame;
	classPaneFrame.fcore.selectPress = selectAdjustPressFrame;
	classPaneFrame.fcore.selectClick = selectClickFrame;
	classPaneFrame.fcore.selectDoubleClick = selectDoubleClickFrame;
	classPaneFrame.fcore.selectDrag = selectDragFrame;
}

#ifdef NOTDEF
/* FrameSetStack -- set the frame's stacking position.   Does not initiate
 *	a configuration change.
 */
void
FrameSetStack(win, mask, mode, sib)
WinPaneFrame *win;
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
#endif /* NOTDEF */


/* FrameMoveRelative
 * Moves a frame by a delta in x and y
 */
void 
FrameMoveRelative(win,dx,dy)
WinPaneFrame *win;
int dx,dy;
{
	(WinFunc(win,core.newposfunc))(win,win->core.x+dx,win->core.y+dy);
}


/*
 * FrameSetPosAbsolute
 * The client is moving the pane to an absolute location on the screen, so we 
 * must move the frame accordingly.
 */
void
FrameSetPosAbsolute(win, x, y)
    WinPaneFrame *win;
    int x, y;
{
        int fx, fy;
	WinGenericPane *pane = win->fcore.panewin;
	int panebord = (pane == NULL)?(0):(pane->pcore.oldBorderWidth);

        fx = x - widthLeftFrame(win)+panebord;
        fy = y - heightTopFrame(win)+panebord;
        (WinFunc(win,core.newposfunc))(win, fx, fy);
}


/* FrameSetPosFromPane -- the client has specified a position for the pane.  
 * 	Using the window gravity, the frame's position should be adjusted
 * 	so that the point on the frame named by the window gravity is at the
 *	corresponding point in the requested pane.
 *	REMIND we aren't accounting for the window's border width here 
 */
void
FrameSetPosFromPane(win,x,y)
WinPaneFrame *win;
int x,y;
{
	int bw = widthLeftFrame(win)+widthRightFrame(win);
	int bh = heightTopFrame(win)+heightBottomFrame(win);
	WinGenericPane *pane = win->fcore.panewin;
	int panebord = (pane == NULL)?(0):(pane->pcore.oldBorderWidth);

	switch (win->core.client->normHints->win_gravity)
	{
	case NorthWestGravity:
		break;

	case NorthGravity:
		x -= bw/2-panebord;
		break;

	case NorthEastGravity:
		x -= bw-2*panebord;
		break;

	case WestGravity:
		y -= bh/2-panebord;
		break;

	case CenterGravity:
		y -= bh/2-panebord;
		x -= bw/2-panebord;
		break;

	case EastGravity:
		y -= bh/2-panebord;
		x -= bw-2*panebord;
		break;

	case SouthWestGravity:
		y -= bh-2*panebord;
		break;

	case SouthGravity:
		y -= bh-2*panebord;
		x -= bw/2-panebord;
		break;

	case SouthEastGravity:
		y -= bh-2*panebord;
		x -= bw-2*panebord;
		break;

	}
	(WinFunc(win,core.newposfunc))(win, x, y);
}

/* FrameUnparentPane -- Reparent the pane back to the root, moving the pane's
 * 	position according to the window gravity
 *	REMIND we aren't accounting for the window's border width yet
 */
void
FrameUnparentPane(cli, winFrame, winPane)
Client *cli;
WinPaneFrame *winFrame;
WinPane *winPane;
{
	int x = winFrame->core.x;
	int y = winFrame->core.y;
	int bw = widthLeftFrame(winFrame)+widthRightFrame(winFrame);
	int bh = heightTopFrame(winFrame)+heightBottomFrame(winFrame);
	int panebord = winPane->pcore.oldBorderWidth;

	switch (winFrame->core.client->normHints->win_gravity)
	{
	case NorthWestGravity:
		break;

	case NorthGravity:
		x += bw/2-panebord;
		break;

	case NorthEastGravity:
		x += bw-2*panebord;
		break;

	case WestGravity:
		y += bh/2-panebord;
		break;

	case CenterGravity:
		y += bh/2-panebord;
		x += bw/2-panebord;
		break;

	case EastGravity:
		y += bh/2-panebord;
		x += bw-2*panebord;
		break;

	case SouthWestGravity:
		y += bh-2*panebord;
		break;

	case SouthGravity:
		y += bh-2*panebord;
		x += bw/2-panebord;
		break;

	case SouthEastGravity:
		y += bh-2*panebord;
		x += bw-2*panebord;
		break;
	}

	XSetWindowBorderWidth(cli->dpy,winPane->core.self,winPane->pcore.oldBorderWidth);
        if (winPane->pcore.oldSaveUnder)
        {
            XSetWindowAttributes xwa;
            xwa.save_under = True;
            XChangeWindowAttributes(cli->dpy,winPane->core.self,CWSaveUnder,&xwa);
        }

	if (winPane->core.kind != WIN_MENU) {
	    XReparentWindow(cli->dpy, winPane->core.self, 
		DefaultRootWindow(cli->dpy), x, y);
	    XChangeSaveSet(cli->dpy, winPane->core.self, SetModeDelete);
	}
}


/*
 * FrameFullSize -- make the frame full size
 */
void
FrameFullSize(frameInfo)
WinPaneFrame         *frameInfo;
{
	Client *cli = frameInfo->core.client;
	WinPane *paneInfo = (WinPane *)frameInfo->fcore.panewin;
	int maxheight;

        /* substite "normal size" menu for "full size" menu */
        if (cli->wmDecors->menu_type == MENU_FULL)
            frameInfo->fcore.menu = &FrameNormMenu;

        /* save restore size attributes */
	if (!frameInfo->restoreSet)
	{
            frameInfo->restoreWidth = paneInfo->core.width;
            frameInfo->restoreHeight = paneInfo->core.height;
            frameInfo->restoreY = frameInfo->core.y;
            frameInfo->restoreSet = True;
	}

        /* if there is a program specified max size */
        if ((cli->normHints) &&
            (cli->normHints->flags & PMaxSize))
        {
		(WinFunc(paneInfo,pcore.setsizefunc))(paneInfo, 
			cli->normHints->max_width,
			cli->normHints->max_height);
		WinCallConfig(cli->dpy, paneInfo, NULL);
        }
        else
        {
                /* move window to top of screen and change the
                 * height to Display Height */
                frameInfo->core.y = 0;
		frameInfo->core.dirtyconfig |= CWY;
		maxheight = DisplayHeight(cli->dpy, cli->screen) - 
			heightTopFrame(frameInfo) - heightBottomFrame(frameInfo);
		(WinFunc(paneInfo,pcore.setsizefunc))(paneInfo, paneInfo->core.width,
			maxheight);
		WinCallConfig(cli->dpy, paneInfo, NULL);
        }
	frameInfo->normfullsizefunc = FrameNormSize;
}

/*
 * FrameNormSize -- restore the frame to normal size
 */
void
FrameNormSize(winInfo)
WinPaneFrame *winInfo;
{
	Client *cli = winInfo->core.client;
	WinPane *paneInfo = (WinPane *)winInfo->fcore.panewin;

        /* substite "full size" menu for "normal size" menu */
        if (cli->wmDecors->menu_type == MENU_FULL)
            winInfo->fcore.menu = &FrameFullMenu;

        /* restore from saved values */
        winInfo->core.y = winInfo->restoreY;
        winInfo->core.dirtyconfig |= CWY;
	winInfo->restoreSet = False;

        /* call the pane's config func to register new size */
	(WinFunc(paneInfo,pcore.setsizefunc))(paneInfo, winInfo->restoreWidth,
		winInfo->restoreHeight);
	WinCallConfig(cli->dpy, paneInfo, NULL);
	winInfo->normfullsizefunc = FrameFullSize;
}

/* FrameNewFooter -- the footer text has changed; update as appropriate */
void
FrameNewFooter(cli)
Client *cli;
{
	setFooterText(cli,cli->dpy,cli->framewin,PANEWINOFCLIENT(cli));
	(WinFunc(cli->framewin,core.drawfunc))(cli->dpy, cli->framewin);
}

#ifdef OW_I18N
/* FrameNewIMStatus -- the IMStatus text has changed; update as appropriate */
void
FrameNewIMStatus(cli)
Client *cli;
{
	setIMStatusText(cli,cli->dpy,cli->framewin,PANEWINOFCLIENT(cli));
	(WinFunc(cli->framewin,core.drawfunc))(cli->dpy, cli->framewin);
}
#endif OW_I18N

/* FrameNewHeader -- the header text has changed; update as appropriate */
void
FrameNewHeader(cli)
Client *cli;
{
	setTitleText(cli,cli->dpy,cli->framewin,PANEWINOFCLIENT(cli));
	(WinFunc(cli->framewin,core.drawfunc))(cli->dpy, cli->framewin);
}

/* FrameFlashTitleBar -- flash the title bar 
 */
void
FrameFlashTitleBar(winInfo)
WinPaneFrame *winInfo;
{
	Client *cli = winInfo->core.client;
	Display *dpy = cli->dpy;
	int ii;
	void (*drawdiff)(), (*drawsame)();

	if (GRV.F3dUsed)
	{
		if (cli->isFocus)
		{
			drawsame = (GRV.FocusFollowsMouse) ?
				(drawHeaderFocusFollow3D):(drawHeaderFocusClick3D);
			drawdiff = drawHeaderNoFocus3D;
		}
		else
		{
			drawdiff = (GRV.FocusFollowsMouse) ?
				(drawHeaderFocusFollow3D):(drawHeaderFocusClick3D);
			drawsame = drawHeaderNoFocus3D;
		}
	}
	else
	{
		if (cli->isFocus)
		{
			drawsame = (GRV.FocusFollowsMouse) ?
				(drawHeaderFocusFollow2D):(drawHeaderFocusClick2D);
			drawdiff = drawHeaderNoFocus2D;
		}
		else
		{
			drawdiff = (GRV.FocusFollowsMouse) ?
				(drawHeaderFocusFollow2D):(drawHeaderFocusClick2D);
			drawsame = drawHeaderNoFocus2D;
		}
	}

	for (ii=0; ii<6; ii++)
	{
		drawdiff(dpy, winInfo, cli, cli->isSelected);
		XFlush(dpy);
		olwm_usleep((unsigned)(GRV.FlashTime));
		drawsame(dpy, winInfo, cli, cli->isSelected);
		XFlush(dpy);
		olwm_usleep((unsigned)(GRV.FlashTime));
	}
}

/* FrameSetBusy - change the frame's busy state.  The client's overall 
 *  	indication has already been set; create a busy window and manipulate
 *	the focus (if necessary).
 */
void
FrameSetBusy(win, newBusy)
WinPaneFrame *win;
Bool newBusy;
{
	if (newBusy)
	{
	    win->winBusy = MakeBusy(win->core.client->dpy, win);
	}
	else
	{
	    (WinFunc(win->winBusy,core.destroyfunc))(win->core.client->dpy, win->winBusy);
	}
	WinCallDraw(win);
}

/* FrameWarpPointer - warp to pane windows default button position (if necessary)
 *	This function can only be called AFTER the frame & pane are mapped.
 */
#define WARPINFO_LEN		6
void
FrameWarpPointer(cli)
Client		*cli;
{
	WinPaneFrame	*frameInfo;
	WinPane		*paneInfo;
	int		*warpParam;
	Bool		sameScreen;	/* pointer is on same screen as client */
	int	status;
        Atom	actType;
        int	actFormat;
        unsigned long nItems, remain;
	Window	root, child;
	int	root_x, root_y, win_x, win_y;
	unsigned int	keys_buttons;
	int	xoff, yoff;

	frameInfo = cli->framewin;
	paneInfo = (WinPane*)(frameInfo->fcore.panewin);

	/* if user has turned off pointer warping, just invalidate the warpinfo */
	if (!GRV.PopupJumpCursor)
	{
		frameInfo->pointerIsWarped = False;
		return;
	}

	/* see if window pane has any warp info */
	/* REMIND: could this property be tracked automatically? */
	warpParam = GetWindowProperty(cli->dpy, paneInfo->core.self, AtomDfltBtn,
		0L, WARPINFO_LEN, XA_INTEGER, 0, &nItems, &remain);
	
	if (warpParam == NULL)
	{
		frameInfo->pointerIsWarped = False;
	}
	else if (nItems != WARPINFO_LEN)
	{
		frameInfo->pointerIsWarped = False;
	        XFree(warpParam);
	}
	else
	{
		/* save warp destination information */
		cli->warpInfo.warpToX = warpParam[0];
		cli->warpInfo.warpToY = warpParam[1];
		cli->warpInfo.dflButtonX = warpParam[2];
		cli->warpInfo.dflButtonY = warpParam[3];
		cli->warpInfo.dflButtonW = warpParam[4];
		cli->warpInfo.dflButtonH = warpParam[5];

		/* save warp return information */
		sameScreen = XQueryPointer(cli->dpy, paneInfo->core.self,
					&root, &child, &root_x, &root_y,
					&win_x, &win_x, &keys_buttons);
		cli->warpInfo.warpBackWin = root;
		cli->warpInfo.warpBackX = root_x;
		cli->warpInfo.warpBackY = root_y;

		/* warp the pointer */
		XWarpPointer(cli->dpy,
			     None, paneInfo->core.self,
			     0, 0, 0, 0,
			     cli->warpInfo.warpToX,
			     cli->warpInfo.warpToY);

		frameInfo->pointerIsWarped = True;
	        XFree(warpParam);
	}

}

/* FrameUnwarpPointer - called when a pane is unmapping, and the pointer
 * needs to be restored to its original position (if it was warped when the
 * window was initially mapped).
 */
void
FrameUnwarpPointer(cli)
Client		*cli;
{
	WinPaneFrame	*frameInfo;
	WinPane		*paneInfo;
	int		warpx, warpy;

Window root, child;
int rootx, rooty, winx, winy, state;
Bool result;

	frameInfo = cli->framewin;
	paneInfo = (WinPane*)(frameInfo->fcore.panewin);

	result = XQueryPointer(cli->dpy, DefaultRootWindow(cli->dpy),
			       &root, &child, &rootx, &rooty, &winx, &winy,
			       &state);

	/*
	 * The pane window may already be unmapped at this point, so we must 
	 * translate the origin of the warp rectangle into root coordinates.
	 */
	WinRootPos(paneInfo, &warpx, &warpy);
	warpx += cli->warpInfo.dflButtonX;
	warpy += cli->warpInfo.dflButtonY;

	if (frameInfo->pointerIsWarped) {
	    XWarpPointer(cli->dpy, DefaultRootWindow(cli->dpy),
			 cli->warpInfo.warpBackWin,
			 warpx, warpy,
			 cli->warpInfo.dflButtonW, cli->warpInfo.dflButtonH,
			 cli->warpInfo.warpBackX, cli->warpInfo.warpBackY);
	    /* invalidate the pointer warp info */
	    frameInfo->pointerIsWarped = False;
	}
}
