/*
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) menu.c 50.8 91/08/26 Crucible";
/*
 * This file contains all of the functions for creating and displaying menus.
 *
 * Global Functions:
 * InitMenus	-- initialize menu stuff
 * MenuCreate	-- create a new menu
 * MenuShow	-- display a menu
 * SetButton	-- draw a button with a particular setting
 */


#ifdef OW_I18N
#include "i18n.h"
#endif
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <memory.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <olgx/olgx.h>

#ifdef OW_I18N
#include "ollocale.h"
#endif
#include "events.h"
#include "olwm.h"
#include "win.h"
#include "menu.h"
#include "globals.h"

/* Externals */
extern Graphics_info *olgx_gisnormal;
extern Graphics_info *olgx_gisbutton;
extern GC DrawBackgroundGC;
extern Bool ColorFocusLocked;
extern WinGeneric *ColorFocusWindow;

/* Locals */
static XEvent		lastPress;
static int		leftmostPosition;
static int		lastX, lastY, minX;
static WinGeneric	*prevColorFocusWindow;


/*
 * Table of currently active menus.
 * REMIND: perhaps this should be dynamically allocated.
 */
#define MAX_ACTIVE_MENUS	20	/* We hope, more than enough. */
static MenuInfo	menuTable[MAX_ACTIVE_MENUS];
static int topMenu = 0;			/* Next free menuTable slot. */

/* Menu callback support. */
static int	(*callback)() = NULL;	/* Button action. */

/*
 * These menu modes correspond to the two possible styles of menu user 
 * interface.  MODE_DRAG corresponds to the "Press-Drag-Release" style, and
 * MODE_CLICK corresponds to the "Click-Move-Click" style.
 */
enum MenuTrackMode { MODE_DRAG, MODE_CLICK } menuTrackMode;

typedef enum {
    L_ONBUTTON,		/* on a button */
    L_ONPIN,		/* on the pin */
    L_ONMENU,		/* elsewhere on the menu (or on an inactive button) */
    L_OFFMENU,		/* off the menu entirely */
} location_t;

#define NOBUTTON	-1	/* no button is active */

/* Calculate fontheight from font info structure. */
#define FONT_HEIGHT(f)		((f.fs_extents)->max_logical_extent.height)
#ifdef OW_I18N
#define BUTT_FONTHEIGHT		FONT_HEIGHT(GRV.ButtonFontSetInfo)
#define BUTT_FONTASCENT		(-GRV.ButtonFontSetInfo.fs_extents->max_logical_extent.y)
#define TITLE_FONTASCENT	(-GRV.TitleFontSetInfo.fs_extents->max_logical_extent.y)
#else OW_I18N
#define BUTT_FONTHEIGHT		FONT_HEIGHT(GRV.ButtonFontInfo)
#define BUTT_FONTASCENT		(GRV.ButtonFontInfo->ascent)
#define TITLE_FONTASCENT	(GRV.TitleFontInfo->ascent)
#endif OW_I18N

/* Label positioning. */
#ifdef OW_I18N
#define TEXT_HEIGHT		FONT_HEIGHT(GRV.ButtonFontSetInfo)
#else
#define TEXT_HEIGHT		FONT_HEIGHT(GRV.ButtonFontInfo)
#endif

/* Height and curve radius of button. */
#define BUTT_HEIGHT		Button_Height(olgx_gisbutton)
#define BUTT_RADIUS		ButtonEndcap_Width(olgx_gisbutton)

/* Space between buttons (these should be adjusted for resolution). */
#define BUTT_VSPACE	0  /* There used to be space between buttons. */
#define BUTT_HSPACE	5  /* Space betw button/menumark & pushpin/title/box */

/* Space above and below the titlebar text, and the space below the
 * last button in the menu.
 */
#define HEAD_VSPACE	4

/* The size of the menu mark is dependant on the font height. */
#define MENUMARK_SIZE 6

/* Width of menu border. */
#define MENUBW		0

/* Distance to the left and down of drop shadow (altitude of menu). */
#define MENUSHADOW_OFF	(10)

/* Events in the menu window that are interesting. */
#define MENU_EVENT_MASK	(PropertyChangeMask | SubstructureNotifyMask)

#define MENU_ATTR_EVENT_MASK	(ButtonPressMask | ExposureMask)

#define MENU_HORIZ_OFFSET 3

/* Static Functions */

static void		(*syncFunc)();
static void		*syncInfo;
static MenuInfo *	allocMenuInfo();
static void		showMenu();
static Bool		menuTrack();
static void		menuHandlePress();
static void		menuHandleMotion();
static Bool		menuHandleRelease();
static MenuInfo *	menuSearch();
static location_t	checkMenuEvent();
static int		menuHide();
static void		unmapChildren();
static void		activateButton();
static void		setMenuPin();
static void		activateSubMenu();
static void		drawButton();
static void		drawRevButton();
static Bool		isClick();

/*
 * ===========================================================================
 */

/*
 * Global routines
 */

/*	
 * InitMenus	-- get the font and related menu initialization
 *	dpy	- display
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
void
InitMenus(dpy)
Display		*dpy;
{
	/* Most of the stuff that should be in here 
	 * is actually in InitGraphics.c
	 */
}

/*
 * MenuCreate	-- fill out a new menu from an array of buttons
 *	dpy	- display to create menu on
 *	menu	- pointer to menu structure to be filled out
 */
void
MenuCreate(dpy, menu)
Display		*dpy;
Menu		*menu;
{
	Button	*buttons;		/* Copy of menu->buttons. */
	Button	*bp;			/* Temp button pointer. */
	int	count;			/* Copy of menu->buttonCount. */
	int	bindex;			/* Current button. */
	int	maxLabWidth;		/* Width of longest menu label. */
	int	menWidth, menHeight;	/* Width and height of menu. */
	int	nextY;			/* Button pos in menu. */
	int	hasStacked = False;	/* True if there are submenus. */ 
	XSetWindowAttributes attributes;

	buttons = menu->buttons;
	count = menu->buttonCount;

	/* Find longest menu entry. */
	for (maxLabWidth = 0, bindex = 0; bindex < count; bindex++)
	{
#ifdef OW_I18N
		maxLabWidth = MAX(maxLabWidth,
				  XwcTextEscapement(GRV.ButtonFontSetInfo.fs,
					     buttons[bindex].label,
					     wslen(buttons[bindex].label)));
#else
		maxLabWidth = MAX(maxLabWidth,
				  XTextWidth(GRV.ButtonFontInfo,
					     buttons[bindex].label,
					     strlen(buttons[bindex].label)));
#endif
		if (buttons[bindex].stacked)
			hasStacked = True;
	}

	maxLabWidth += 2 * BUTT_RADIUS;

	/* If any of the buttons have submenus, 
	 * make space for the menu mark. 
	 */
	if (hasStacked)
		maxLabWidth += BUTT_HSPACE + MENUMARK_SIZE;

	/* Calculate title parameters. */
	if (menu->title != NULL)
	{
#ifdef OW_I18N
		menu->titleWidth = XwcTextEscapement(GRV.TitleFontSetInfo.fs,
					      menu->title,
					      wslen(menu->title));
#else
		menu->titleWidth = XTextWidth(GRV.TitleFontInfo,
					      menu->title,
					      strlen(menu->title));
#endif
		menu->titleHeight = HEAD_VSPACE + 
#ifdef OW_I18N
				    MAX(FONT_HEIGHT(GRV.TitleFontSetInfo),
					PushPinOut_Height(olgx_gisnormal)) +
#else
				    MAX(FONT_HEIGHT(GRV.TitleFontInfo),
					PushPinOut_Height(olgx_gisnormal)) +
#endif
				    HEAD_VSPACE;

		if (menu->hasPushPin)
		{
			menu->titleX = BUTT_HSPACE + 
					PushPinOut_Width(olgx_gisnormal) +
					BUTT_HSPACE;
			menu->titleY = HEAD_VSPACE + TITLE_FONTASCENT;

			menWidth = MAX(BUTT_HSPACE + maxLabWidth + BUTT_HSPACE,
				       (BUTT_HSPACE + 
					PushPinOut_Width(olgx_gisnormal) +
					BUTT_HSPACE + menu->titleWidth +
					BUTT_HSPACE));
		}
		else
		{
			menWidth = MAX(BUTT_HSPACE + maxLabWidth + BUTT_HSPACE,
				       BUTT_HSPACE + menu->titleWidth +
				       BUTT_HSPACE);

			menu->titleX = (menWidth / 2) - (menu->titleWidth / 2);
			menu->titleY = HEAD_VSPACE + TITLE_FONTASCENT;
		}
	}
	else
	{
		menWidth = BUTT_HSPACE + maxLabWidth + BUTT_HSPACE;

		menu->titleX = 0;
		menu->titleY = 0;
		menu->titleWidth = 0;
		menu->titleHeight = 0;
	}
	
	/* Menu height is the sum of the buttons, the title height if any,
	 * the space above the first button, and the space below the last 
	 * button.
	 */
	menHeight = menu->titleHeight + HEAD_VSPACE +
		    ((BUTT_HEIGHT + BUTT_VSPACE) * count) +
		    HEAD_VSPACE;

	menu->width = menWidth;
	menu->height = menHeight;
	
	/* Place the pushpin.
	 * Pushpin is centered vertically in case the font height
	 * is smaller than the pushpin height.
	 */
	menu->pushPinX = BUTT_HSPACE;
	menu->pushPinY = (menu->titleHeight -
			  PushPinOut_Height(olgx_gisnormal)) / 2;

	/* Menu window. */
	attributes.event_mask = MENU_ATTR_EVENT_MASK;
	attributes.save_under = DoesSaveUnders(DefaultScreenOfDisplay(dpy));

	menu->window = XCreateWindow(dpy, DefaultRootWindow(dpy),
					0, 0,
					menWidth,
					menHeight,
					MENUBW,
					DefaultDepth(dpy, DefaultScreen(dpy)),
					InputOutput,
					DefaultVisual(dpy, DefaultScreen(dpy)),
					CWEventMask | CWSaveUnder /*| CWBackPixel */,
					&attributes);

#ifdef SHADOW
	attributes.background_pixmap = pixmapGray;
	attributes.save_under = DoesSaveUnders(DefaultScreenOfDisplay(dpy));
	menu->shadow = XCreateWindow(dpy, DefaultRootWindow(dpy),
					0, 0,
					menWidth,
					menHeight,
					0,
					DefaultDepth(dpy, DefaultScreen(dpy)),
					InputOutput,
					DefaultVisual(dpy, DefaultScreen(dpy)),
					CWBackPixmap |  CWSaveUnder,
					&attributes);
#endif /* SHADOW */

	XDefineCursor( dpy, menu->window, GRV.MenuPointer );

	/* Precalculate the button postions for faster 
	 * display/drawing and button-press checking.
	 * Because pinned menus don't have title windows,
	 * ( we just draw in the title ),
	 * these positions are calculated without the titleHeight.
	 */
	for(nextY = BUTT_VSPACE, bindex = 0; bindex < count; bindex++)
	{
		bp = &buttons[bindex];
		/* These describe the area of the button that will
		 * be hightlighted when the button is activated. Or
		 * one could say that these describe the area that
		 * will activate the button, since the button press code
		 * uses these to determine if a button press happens.
		 */
		bp->activeX = BUTT_HSPACE;
		bp->activeY = nextY;
		bp->activeW = menWidth - (2 * BUTT_HSPACE);
		bp->activeH = BUTT_HEIGHT;

		/* Move down to next button postion. */
		nextY += BUTT_HEIGHT + BUTT_VSPACE;
	}
}

#ifdef OW_I18N
MenuDestroy( dpy, menu )
Display		*dpy;
Menu		*menu;
{
	register Button	*bt;
	register int	i;

	for (i = 0, bt = menu->buttons; i < menu->buttonCount; i++, bt++)
		if (bt->stacked == True && bt->action.submenu != NULL)
			MenuDestroy( dpy, bt->action.submenu );

	if (menu->currentlyDisplayed == True)
		DestroyClient(menu->PinWin->core.client);
	XDestroyWindow(dpy, menu->window);
}
#endif OW_I18N

/*
 * ExecButtonAction
 *
 * Given a menu and a button, find the button's action (by searching down the 
 * menu tree following defaults, if necessary) and execute it.
 */
void
ExecButtonAction( dpy, winInfo, menu, btn, pinned )
Display		*dpy;
WinGeneric	*winInfo;
Menu		*menu;
int		 btn;
Bool		 pinned;
{
    /* search down the menu tree for defaults */
    while ( btn >= 0 && menu->buttons[btn].stacked ) 
    {
	menu = menu->buttons[btn].action.submenu;
	btn = menu->buttonDefault;
    }
    if ( btn >= 0 )
	(*menu->buttons[btn].action.callback)(dpy, winInfo, menu, btn, pinned);
}

/*
 * ExecDefaultAction(dpy, winInfo, menu, fPinned)
 *
 * Given a menu, execute the associated default action (if any)
 */  
void
ExecDefaultAction(dpy, winInfo, menu, fPinned)
Display		*dpy;
WinGeneric	*winInfo;
Menu		*menu;
Bool		 fPinned;
{
	FuncPtr defaultCallback;

	if (menu->buttonDefault < 0)
		return;

	defaultCallback = menu->buttons[menu->buttonDefault].action.callback;
	if (defaultCallback != NULL)
	{
		(*defaultCallback)(dpy, winInfo, menu, 
				   menu->buttonDefault, fPinned);
	}
}

/*
 * Draw menu contents into menu->window.
 */
void
DrawMenu(dpy, menu)
Display	*dpy;
Menu	*menu;
{
	int bindex;
	int byOff = 0;
	Window	win = menu->window;

	/* Draw the basic menu background if this menu isn't pinned */
	if ((menu->originalMenu != NULL) || (!GRV.F3dUsed))
	{
		XFillRectangle(dpy, win, DrawBackgroundGC, 0, 0, 
			menu->width, menu->height);
	}
	if (menu->originalMenu == NULL)
	{
		olgx_draw_box(olgx_gisnormal, win, 0, 0, 
			menu->width, menu->height,  OLGX_NORMAL, True);
	}

	/* Draw the menu title. */
	if (menu->title != NULL)
	{
		if (menu->hasPushPin)
		{
			/* If the menu is already displayed, draw the
			 * pushpin grayed out to indicate that it can't
			 * be pinned again.
			 */
			if (menu->currentlyDisplayed)
			{
				/* REMIND we have to manually erase the
				 * pushpin because OLGX is broken when
				 * it comes to erasing pushpins.
				 */
				XFillRectangle(dpy, win, DrawBackgroundGC,
					menu->pushPinX, menu->pushPinY,
					PushPinOut_Width(olgx_gisnormal),
					PushPinOut_Height(olgx_gisnormal));
				olgx_draw_pushpin(olgx_gisnormal, win, 
						  menu->pushPinX,
						  menu->pushPinY,
						  OLGX_PUSHPIN_OUT |
						  OLGX_INACTIVE);
			}
			else
			{
				XFillRectangle(dpy, win, DrawBackgroundGC,
					menu->pushPinX, menu->pushPinY,
					PushPinOut_Width(olgx_gisnormal),
					PushPinOut_Height(olgx_gisnormal));
				olgx_draw_pushpin(olgx_gisnormal, win, 
						  menu->pushPinX,
						  menu->pushPinY,
						  OLGX_PUSHPIN_OUT);
			}
		}

		olgx_draw_text(olgx_gisnormal, win, menu->title, menu->titleX,
			menu->titleY, 0, False, OLGX_NORMAL);
#ifdef OW_I18N
		if (GRV.BoldFontEmulation == True)
			olgx_draw_text(olgx_gisnormal, win, menu->title,
			       menu->titleX + 1, menu->titleY, 0,
			       False, OLGX_NORMAL);
#endif

	        olgx_draw_text_ledge(olgx_gisnormal, win, 
		        BUTT_HSPACE, menu->titleHeight-2, 
		        menu->width-(BUTT_HSPACE*2));

		/* byOff is the gap between the top of the menu and the
		 * top of the first button.
		 */
		byOff = menu->titleHeight;
	}
	else  /* No title on this menu. */
	{
		byOff = HEAD_VSPACE;
	}

	/* Draw the buttons. */
	for (bindex=0; bindex < menu->buttonCount; bindex++)
		drawButton(dpy, win, &menu->buttons[bindex], byOff, 
			   (bindex==menu->buttonDefault), 
			   (menu->originalMenu != NULL));
}


void
SetButton( dpy, menu, idx, highlight )
Display	*dpy;
Menu	*menu;
int	idx;
Bool	highlight;
{
    int		yoff;

    if ( menu->title == NULL )
	yoff = HEAD_VSPACE;
    else
	yoff = menu->titleHeight;

    if ( highlight )
	drawRevButton(dpy, menu->window, &menu->buttons[idx], yoff);
    else
	drawButton(dpy, menu->window, &menu->buttons[idx], yoff,
		   (idx == menu->buttonDefault),
		   (menu->originalMenu != NULL));
}



/*
 * MenuShow
 * MenuShowSync
 *
 * These functions are the main entry points into the menu tracking system.  
 * MenuShow() grabs everything, sets up the event interposer, and returns.
 * 
 * REMIND
 * MenuShowSync() sets up an additional callback that is called after the menu 
 * action is completed.  This is necessary for the present implementation of 
 * pinned menus, which need to have work done after the menu goes down, in 
 * addition to the menu button action.  This interface should probably go away 
 * once pinned menus are rearchitected.
 */
void
MenuShowSync(dpy, winInfo, menu, pevent, sfunc, sinfo)
Display		*dpy;
WinGeneric	*winInfo;
Menu		*menu;
XEvent		*pevent;
void		(*sfunc)();
void		*sinfo;
{
	MenuInfo *mInfo;

	/* Initialize the menu info alloc stuff. */
	/* memset takes int 2nd arg (uses as char) */
	/* lint will complain about this cast */
	memset((char *)menuTable, 0, sizeof(MenuInfo) * MAX_ACTIVE_MENUS);
	topMenu = 0;

	/* Grab the server to prevent anybody from
	 * sullying the underlying windows when the 
	 * menu window is mapped.
	 */
	XGrabServer(dpy);
	
	XGrabPointer(dpy, DefaultRootWindow(dpy),
		     False,
		     ButtonReleaseMask | ButtonMotionMask |
		     ButtonPressMask,
		     GrabModeAsync, GrabModeAsync,
		     None,
		     GRV.MenuPointer,
		     CurrentTime);
        
	if (ColorFocusLocked)
	    prevColorFocusWindow = ColorFocusWindow;
	InstallColormap(dpy, WIGetInfo(DefaultRootWindow(dpy)));

	InstallInterposer( menuTrack, (void *)winInfo );

	syncFunc = sfunc;
	syncInfo = sinfo;

	/* Install the first menu */
	menuTrackMode = MODE_DRAG;
	lastPress = *pevent;
	mInfo = allocMenuInfo( menu );
	showMenu(dpy, mInfo,
		 pevent->xbutton.x_root - MENU_HORIZ_OFFSET,
		 pevent->xbutton.y_root - (BUTT_HEIGHT + BUTT_VSPACE)/2);
}


void
MenuShow(dpy, winInfo, menu, pevent)
Display		*dpy;
WinGeneric	*winInfo;
Menu		*menu;
XEvent		*pevent;
{
    MenuShowSync(dpy, winInfo, menu, pevent, NULL, NULL);
}


/*
 * PointInRect	-- check if a point is inside a rectangle
 */
int
PointInRect(x, y, rx, ry, rw, rh)
int x, y, rx, ry, rw, rh;
{
	if (((x)>(rx)) && ((x)<(rx)+(rw)) && ((y)>(ry)) && ((y)<(ry)+(rh)))
		return 1;
	else
		return 0;
}


/*
 * ===========================================================================
 */

/*
 * Local routines
 */

static MenuInfo *
allocMenuInfo(menu)
Menu	*menu;
{
	MenuInfo	*new;
	int		i;

	if ( topMenu > MAX_ACTIVE_MENUS ) 
	{
#ifdef OW_I18N
	    fputs( gettext("olwm: no more active menus!\n"), stderr );
#else
	    fputs( "olwm: no more active menus!\n", stderr );
#endif
	    exit( 1 );
	}

	new = &menuTable[topMenu];
	++topMenu;

	new->menu = menu;
	new->childActive = False;
	new->childMenu = (Menu *)NULL;
	new->pinIn = False;
	new->litButton = NOBUTTON;

	return new;
}


/* not to be confused with MenuShow() */
static void
showMenu(dpy, mInfo, x, y)
Display		*dpy;
MenuInfo	*mInfo;
int		x, y;
{
	int		dpyWidth, dpyHeight;
	Menu		*menu = mInfo->menu;
#ifdef SHADOW
	XWindowChanges	values;
#endif /* SHADOW */

	/* if menu has a default, line default button with current y;
	 * otherwise line first button of menu up with current y. 
	 */
	if (menu->buttonDefault > 0)
		y -= menu->buttonDefault * (BUTT_HEIGHT + BUTT_VSPACE);
	if (menu->title != NULL)
		y -= menu->titleHeight;

	/* Make sure the menu is going to fit on the screen. */
	dpyWidth = DisplayWidth(dpy, DefaultScreen(dpy));
	dpyHeight = DisplayHeight(dpy, DefaultScreen(dpy));
	if ((x + menu->width) > dpyWidth)
		x = dpyWidth - menu->width;

	if ((y + menu->height) > dpyHeight)
		y = dpyHeight - menu->height;

	if (y < 0)
		y = 0;

	/* Move the menu window to position. */
	XMoveWindow(dpy, menu->window, x, y);
#ifdef SHADOW
	XMoveWindow(dpy, menu->shadow, x + MENUSHADOW_OFF, y + MENUSHADOW_OFF);
#endif /* SHADOW */
	menu->x = x;
	menu->y = y;

	/* Map the menu window and its shadow.
	 * The OLWM designers want to see the menu appear first and
	 * then the shadow, NOT the shadow and then the menu.
	 * So, we have to mess around a bit to do this.
	 * To make the menu appear first, and then the shadow
	 * under it, we have to map the menu first and then
	 * change the stacking order before mapping the shadow.
	 */
	XMapRaised(dpy, menu->window);

#ifdef SHADOW
	values.sibling = menu->window;
	values.stack_mode = Below;
	XConfigureWindow(dpy, menu->shadow, CWStackMode|CWSibling, &values);
	XMapWindow(dpy, menu->shadow);
#endif /* SHADOW */

	mInfo->ignoreNextExpose = True;
	DrawMenu(dpy, menu);
}


/*
 * eventX, eventY, eventTime
 *
 * Extract the xroot, yroot, or timestamp fields from an event, assuming it's
 * a MotionNotify, ButtonPress, or ButtonRelease.
 */

#define eventX(e)	((e)->type == MotionNotify ? (e)->xmotion.x_root \
						   : (e)->xbutton.x_root )

#define eventY(e)	((e)->type == MotionNotify ? (e)->xmotion.y_root \
						   : (e)->xbutton.y_root )

#define eventTime(e)	((e)->type == MotionNotify ? (e)->xmotion.time \
						   : (e)->xbutton.time )


/*
 * menuTracker
 * Event interposer for menu tracking.
 */
static int
menuTrack( dpy, pevent, win, closure )
Display		*dpy;
XEvent		*pevent;
WinGeneric	*win;
void		*closure;
{
    MenuInfo *mInfo;

    switch ( pevent->type ) 
    {
    case ButtonPress:
	lastPress = *pevent;
	menuHandlePress( dpy, pevent );
	break;
    case MotionNotify:
	if (!pevent->xmotion.same_screen)
		break;
	switch ( menuTrackMode ) 
	{
	case MODE_DRAG:
	    if (!isClick(&lastPress,pevent))
		menuHandleMotion( dpy, pevent );
	    break;
	case MODE_CLICK:
	    if (!isClick(&lastPress,pevent))
		menuTrackMode = MODE_DRAG;
	    break;
	}
	break;
    case ButtonRelease:
	switch ( menuTrackMode ) 
	{
	case MODE_DRAG:
	    if (isClick(&lastPress,pevent)) 
	    {
		menuTrackMode = MODE_CLICK;
	    } 
	    else 
	    {
		menuHide( dpy, (WinGeneric *)closure );
	    }
	    break;
	case MODE_CLICK:
	    if (menuHandleRelease(dpy,pevent))
		menuHide( dpy, (WinGeneric *)closure );
	    break;
	}
	break;
    case KeyPress:
	break;
    case KeyRelease:
	break;
    case Expose:
	mInfo = menuSearch( pevent );
	if ( mInfo == NULL )
	    return DISPOSE_DISPATCH;
	if ( mInfo->ignoreNextExpose )
	    mInfo->ignoreNextExpose = False;
	else 
	{
	    DrawMenu( dpy, mInfo->menu );
	    if ( mInfo->litButton != NOBUTTON )
		SetButton( dpy, mInfo->menu, mInfo->litButton, True );
	    if ( mInfo->pinIn ) 
	    {
		/*
		 * REMIND
		 * This is a trifle odd.  We have to set pinIn to False 
		 * because setMenuPin does nothing if pinIn already equals the 
		 * value we're setting it to.  The alternative is to code a 
		 * call to olgx_draw_pushpin here, which is worse.
		 */
		mInfo->pinIn = False;
		setMenuPin( dpy, mInfo, True );
	    }
	}
	break;
    default:
	return DISPOSE_DISPATCH;
    }

    /* for pointer events, save the event location */
    switch ( pevent->type ) 
    {
    case ButtonPress:
    case ButtonRelease:
    case MotionNotify:
	if (pevent->xmotion.same_screen)
	{
	    lastX = eventX(pevent);
	    lastY = eventY(pevent);
	}
	break;
    default:
	break;
    }
    return DISPOSE_USED;
}


static void
menuHandlePress( dpy, pevent )
Display	*dpy;
XEvent	*pevent;
{
    int bindex;
    int status;
    MenuInfo *mInfo;
    
    mInfo = menuSearch( pevent );
    status = checkMenuEvent(mInfo->menu, pevent, &bindex);

    switch ( status ) 
    {
    case L_ONBUTTON:
	/* need to unmap children of menu choice which was
	 * previously invoked using click menus
	 * REMIND doesn't seem to be working
	 */
	unmapChildren(dpy, mInfo);
	activateButton( dpy, mInfo, bindex );
	minX = eventX(pevent);
	break;
    case L_ONPIN:
	/* need to unmap children of menu choice which was
	 * previously invoked using click menus
	 * REMIND doesn't seem to be working
	 */
	unmapChildren(dpy, mInfo);
	setMenuPin( dpy, mInfo, True );
	break;
    default:
	break;
    }
}    


static void
menuHandleMotion( dpy, pevent )
Display *dpy;
XEvent	*pevent;
{
    int status;
    int bindex;
    MenuInfo *mInfo;
    int curX;
    int deltaX;
    Bool samebutton;
    int i;

    mInfo = menuSearch( pevent );
    status = checkMenuEvent( mInfo->menu, pevent, &bindex );

    /*
     * If the push pin was in before and this event is not a L_ONPIN event,
     * put the pin back out because we are no longer in the pin area.
     */
    if ((mInfo->pinIn) && (status != L_ONPIN))
	setMenuPin( dpy, mInfo, False );

    switch ( status ) 
    {
    case L_ONBUTTON:
	samebutton = ( bindex == mInfo->litButton );
	if (mInfo->childActive && !samebutton)
	    unmapChildren(dpy, mInfo);
	curX = pevent->xmotion.x_root;
	activateButton( dpy, mInfo, bindex );

	if (mInfo->menu->buttons[bindex].stacked) 
	{
	    if ( samebutton ) 
	    {
		deltaX = curX - minX;
		minX = MIN(curX,minX);
	    }
	    else 
	    {
		deltaX = curX - MAX(lastX,mInfo->menu->x);
		minX = MIN(curX,lastX);
	    }

	    if ((deltaX > GRV.DragRightDistance) ||
		(curX > (mInfo->menu->x +
			 mInfo->menu->buttons[bindex].activeX + 
			 mInfo->menu->buttons[bindex].activeW -
			 ButtonEndcap_Width(olgx_gisbutton) -
			 MenuMark_Width(olgx_gisnormal)))) 
	    {
		activateSubMenu(dpy, mInfo, bindex, pevent->xmotion.x_root);
		minX = curX;
	    }
	}
	break;

    case L_ONPIN:
	    setMenuPin( dpy, mInfo, True );
	    if (mInfo->childActive)
		    unmapChildren(dpy, mInfo);
	    activateButton( dpy, mInfo, NOBUTTON );
	    break;

    case L_OFFMENU:
	    activateButton( dpy, mInfo, NOBUTTON );
	    break;
	    
    case L_ONMENU:
	    if (!mInfo->childActive)
		activateButton( dpy, mInfo, NOBUTTON );
	    break;

    }  /* End switch */

    /*
     * Pull down all menus to the right of the current mouse position, except 
     * for the initial menu.
     */
    i = topMenu-1;
    while (i > 0 &&
	   menuTable[i].menu->x > pevent->xmotion.x_root)
	--i;
    if ( i < topMenu-1 ) 
    {
	unmapChildren( dpy, &menuTable[i] );
	topMenu = i+1;
	if ( status != L_ONBUTTON )
	    activateButton( dpy, &menuTable[i], NOBUTTON );
    }
}    


static Bool
menuHandleRelease( dpy, pevent )
Display *dpy;
XEvent	*pevent;
{
    int bindex;
    int status;
    MenuInfo *mInfo;
    MenuInfo *newmenu;
    
    mInfo = menuSearch( pevent );
    status = checkMenuEvent(mInfo->menu, pevent, &bindex);

    if (status == L_ONBUTTON &&
	mInfo->menu->buttons[bindex].stacked &&
	menuTrackMode == MODE_CLICK &&
	MouseButton(dpy,pevent) == MB_MENU)
    {
	unmapChildren(dpy, mInfo);
	activateButton( dpy, mInfo, bindex );
	activateSubMenu( dpy, mInfo, bindex, pevent->xbutton.x_root );
	return False;
    }
    return True;
}    


/*
 * menuSearch
 *
 * Given an event, search the stack of active menus for the menu on which this
 * event occurred.  The event must be a ButtonPress, ButtonRelease,
 * MotionNotify, or Expose event.  If the event didn't occur on any of the
 * menus, for the pointer events, the topmost menu in the stack is returned.  
 * Otherwise, zero is returned.
 */
static MenuInfo *
menuSearch( event )
XEvent *event;
{
    Window w;
    int i;

    switch ( event->type ) 
    {
    case ButtonPress:
    case ButtonRelease:
	w = event->xbutton.subwindow;
	break;
    case MotionNotify:
	if (event->xmotion.same_screen)
	{
	    w = event->xmotion.subwindow;
	}
	break;
    case Expose:
	w = event->xexpose.window;
	break;
    default:
#ifdef OW_I18N
	fputs( gettext("olwm: wrong event type passed to menuSearch\n"),
	       stderr );
#else
	fputs( "olwm: wrong event type passed to menuSearch\n", stderr );
#endif
	return (MenuInfo *) 0;
    }

    for (i=topMenu-1; i>=0; --i) 
    {
	if ( w == menuTable[i].menu->window )
	    return &menuTable[i];
    }
    return (event->type == Expose) ? (MenuInfo *) 0
				   : &menuTable[topMenu-1];
}


/*
 * checkMenuEvent
 *
 * Check a button or motion event against a menu.  Sets the index of the 
 * active button (or to NOBUTTON) and returns the pointer location:
 *	L_ONBUTTON, L_ONPIN, L_ONMENU, or L_OFFMENU.
 */
static location_t
checkMenuEvent( menu, pevent, bindex )
Menu	*menu;
XEvent	*pevent;
int	*bindex;
{
	int	i;
	int	yoff = 0;
	Window	subwindow;
	int	ex, ey;

	/* menu->title == NULL for pinned menus, as well as title-less ones */
	if (menu->title != NULL)
		yoff = menu->titleHeight;
	else
		yoff = HEAD_VSPACE;

	switch ( pevent->type ) 
	{
	case MotionNotify:
	    if (pevent->xmotion.same_screen)
	    {
	        subwindow = pevent->xmotion.subwindow;
	        ex = pevent->xmotion.x_root;
	        ey = pevent->xmotion.y_root;
	    }
	    break;
	case ButtonPress:
	case ButtonRelease:
	    subwindow = pevent->xbutton.subwindow;
	    ex = pevent->xbutton.x_root;
	    ey = pevent->xbutton.y_root;
	    break;
	}

	/* If the event window is not the menu window. */
	if (subwindow != menu->window) 
	{
		*bindex = NOBUTTON;
		return L_OFFMENU;
	}
	
	/*
	 * Check the event coordinates against each of the buttons.
	 * Since the button event is reported relative to root window
	 * it must be adjusted for the check.
	 */
	for (i=0; i < menu->buttonCount; i++)
	{
		if (PointInRect(ex - menu->x, 
				ey - menu->y,
				menu->buttons[i].activeX,
				menu->buttons[i].activeY + yoff,
				menu->buttons[i].activeW,
				menu->buttons[i].activeH))
		{
			/* Event is in a button.
			 * Is it a button stack and if so,
			 * is it in the right half of the button?
			 */
			*bindex = i;
			if (menu->buttons[i].state == Disabled)
			    return L_ONMENU;
			else
			    return L_ONBUTTON;
#ifdef notdef
			if ((menu->buttons[i].stacked) &&
			    ((ex - menu->x) > (menu->width/2)))
				return S_ACTIVATE;
			else
				return S_ONBUTTON;
#endif /* notdef */
		}
	}

	/* Check the pushpin area. */
	*bindex = -1;
	if (menu->hasPushPin &&
	    PointInRect(ex - menu->x, 
			ey - menu->y,
			menu->pushPinX, menu->pushPinY,
			PushPinOut_Width(olgx_gisnormal), 
			PushPinOut_Height(olgx_gisnormal)) && 
	    !menu->currentlyDisplayed)		/* true if menu is pinned */
		return L_ONPIN;
	else
		return L_ONMENU;
}


/*
 * menuHide
 *
 * Remove any active menus from the screen, and call the menu callback
 * function as necessary.
 */
static int
menuHide( dpy, winInfo )
Display		*dpy;
WinGeneric	*winInfo;
{
    int i;
    MenuInfo *mInfo = &menuTable[topMenu-1];
    int btn;
    Menu *menu;
    int (*callback)() = (int (*)()) 0;

    if (ColorFocusLocked)
	InstallColormap(dpy, prevColorFocusWindow);

    XUngrabServer(dpy);
    XUngrabPointer(dpy, CurrentTime);
    XFlush(dpy);
    UninstallInterposer();

    /* Unmap any active menus. */
    for ( i=topMenu-1; i>=0; --i )
    {
#ifdef SHADOW
	XUnmapWindow(dpy, menuTable[i].menu->shadow);
#endif /* SHADOW */
	XUnmapWindow(dpy, menuTable[i].menu->window);
    }

    if ( mInfo->pinIn ) 
    {
	(*mInfo->menu->pinAction)(dpy, winInfo, mInfo->menu, NOBUTTON, False);
    }
    else 
    {
	if ( mInfo->litButton != NOBUTTON )
	    ExecButtonAction(dpy, winInfo, mInfo->menu,
			     mInfo->litButton, False );
    }
	
    if ( syncFunc != NULL )
	(*syncFunc)( syncInfo );
}


static void
unmapChildren(dpy, mInfo)
Display		*dpy;
MenuInfo	*mInfo;
{
	int		i;

	i = topMenu-1;
	while ( i >= 0 && menuTable[i].menu != mInfo->menu ) 
	{
#ifdef SHADOW
	    XUnmapWindow(dpy, menuTable[i].menu->shadow);
#endif /* SHADOW */
	    XUnmapWindow(dpy, menuTable[i].menu->window);
	    --i;
	}
	topMenu = i+1;
#ifdef DEBUG
	if ( i < 0 )
	    fputs( "olwm: warning, internal error in unmapChildren!\n",
		  stderr );
#endif /* DEBUG */

	mInfo->childActive = False;
}


static void
activateButton( dpy, mInfo, idx )
Display		*dpy;
MenuInfo	*mInfo;
int		idx;
{
    if ( mInfo->litButton == idx )
	return;

    /* Unhighlight any highlit button. */
    if ( mInfo->litButton != NOBUTTON ) 
	SetButton( dpy, mInfo->menu, mInfo->litButton, False );

    /* Highlight the new button */
    if ( idx != NOBUTTON )
	SetButton( dpy, mInfo->menu, idx, True );
    
    mInfo->litButton = idx;
}


static void
setMenuPin( dpy, mInfo, state )
Display		*dpy;
MenuInfo	*mInfo;
Bool		state;
{
    if ( mInfo->pinIn != state ) 
    {
	mInfo->pinIn = state;
	XFillRectangle(dpy, mInfo->menu->window, DrawBackgroundGC,
		mInfo->menu->pushPinX, mInfo->menu->pushPinY,
		PushPinOut_Width(olgx_gisnormal),
		PushPinOut_Height(olgx_gisnormal));
	olgx_draw_pushpin(olgx_gisnormal, mInfo->menu->window,
	    mInfo->menu->pushPinX, mInfo->menu->pushPinY,
	    state ? OLGX_PUSHPIN_IN : OLGX_PUSHPIN_OUT);
    }
}


/*
 * activateSubMenu
 *
 * Given a MenuInfo struct and a button, activate that button's submenu.
 * It's assumed that the button actually has a submenu.  Note that only the
 * x-location is passed in, while the y-location is calculated.  The reason is 
 * that the x-location is determined by the mouse event, while the y-location 
 * is always based the location of the parent menu.  If a submenu is already 
 * active, do nothing.  This is primarily to prevent the same submenu from 
 * being activated again.  This occurs if a submenu is much narrower than its 
 * parent, and you pull off the right of the submenu back into the parent.
 */
static void
activateSubMenu( dpy, mInfo, bindex, x )
Display		*dpy;
MenuInfo	*mInfo;
int		bindex;
int		x;
{
    MenuInfo *newmenu;

    if ( !mInfo->childActive ) 
    {
	mInfo->childActive = True;
	mInfo->childMenu = 
	    (Menu *)(mInfo->menu->buttons[bindex].
		 action.submenu);
	newmenu = allocMenuInfo(mInfo->childMenu);
	showMenu(dpy, newmenu, x-MENU_HORIZ_OFFSET,
		 mInfo->menu->y +
		 mInfo->menu->buttons[bindex].activeY +
		 ((mInfo->menu->title != NULL) ? 
			mInfo->menu->titleHeight : 0));
    }
}


/* Draw a normal button. 
 * if fDefault is true, a default ring will be drawn.
 * fIsPinned indicates whether this button is on a pinned menu.
 */
/*ARGSUSED*/	/* dpy arg is not yet used */
static void
drawButton(dpy, win, button, yOffset, fDefault, fIsPinned)
Display	*dpy;
Window	win;
Button	*button;
int	yOffset;
Bool 	fDefault;
Bool 	fIsPinned;
{
	int state;

	state = OLGX_NORMAL | 
		OLGX_ERASE |
		((button->state == Enabled)? 0 : OLGX_INACTIVE) |
		((button->stacked) ? OLGX_HORIZ_MENU_MARK : 0) |
		(fDefault? OLGX_DEFAULT : 0);
	if (!(fIsPinned && GRV.FShowPinnedMenuButtons || GRV.FShowMenuButtons))
	{
		/* setting this flag turns OFF painting of the menu buttons */
		state |= OLGX_MENU_ITEM;
	}

	olgx_draw_button(olgx_gisbutton, win, button->activeX, 
			button->activeY+yOffset, button->activeW, 0,
			button->label, state);
			
}


/* Draw a reverse video button. */
/*ARGSUSED*/	/* dpy arg is not yet used */
static void
drawRevButton(dpy, win, button, yOffset)
Display	*dpy;
Window	win;
Button	*button;
int	yOffset;
{
	int state;

	/* if the button is disabled, do nothing */
	if (button->state == Disabled)
		return;

	state = OLGX_INVOKED | OLGX_ERASE | 
		((button->stacked) ? OLGX_HORIZ_MENU_MARK : 0);
	olgx_draw_button(olgx_gisbutton, win, 
			 button->activeX, button->activeY + yOffset,
			 button->activeW, 0, 
			 button->label, state | OLGX_MENU_ITEM);
}


/*
 * isClick
 * 
 * Takes two button events and returns a boolean indicating whether they are 
 * close enough (spacially and temporally) to be considered a click.
 */

#define THRESH_DIST   5

static Bool
isClick( e1, e2 )
XEvent *e1;
XEvent *e2;
{
    return (
	ABS(eventX(e1)-eventX(e2)) <= GRV.ClickMoveThreshold &&
	ABS(eventY(e1)-eventY(e2)) <= GRV.ClickMoveThreshold &&
	eventTime(e2)-eventTime(e1) <= GRV.DoubleClickTime
    );
}
