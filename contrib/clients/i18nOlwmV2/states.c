/*      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

/* states.c - functions relating to changes in client state 
 *	(Normal, Iconic, Withdrawn)
 */

static	char	sccsid[] = "@(#) states.c 50.5 90/12/12 Crucible";
#ifdef OW_I18N
#include "i18n.h"
#endif
#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#ifdef OW_I18N
#include "ollocale.h"
#endif
#include "mem.h"
#include "olwm.h"
#include "win.h"
#include "group.h"
#include "globals.h"

/***************************************************************************
* global data
***************************************************************************/

extern int	DefScreen;
extern Atom AtomClass;
extern Atom AtomDecorAdd;
extern Atom AtomDecorClose;
extern Atom AtomDecorDelete;
extern Atom AtomDecorFooter;
extern Atom AtomDecorHeader;
extern Atom AtomDecorPin;
extern Atom AtomDecorResize;
extern Atom AtomDeleteWindow;
extern Atom AtomMenuFull;
extern Atom AtomMenuLimited;
extern Atom AtomNone;
extern Atom AtomOlwmTimestamp;
extern Atom AtomPinIn;
extern Atom AtomPinOut;
extern Atom AtomProtocols;
extern Atom AtomSaveYourself;
extern Atom AtomTakeFocus;
extern Atom AtomWinAttr;
extern Atom AtomWTBase;
extern Atom AtomWTCmd;
extern Atom AtomWTHelp;
extern Atom AtomWTNotice;
extern Atom AtomWTOther;
extern Atom AtomWTProp;
extern Window NoFocusWin;
extern Bool FocusIndeterminate;
#ifdef OW_I18N
extern Atom AtomDecorIMStatus;
#endif

/***************************************************************************
* private data
***************************************************************************/

/* sanity checks for getting stuff out of hints */
#define IsCard16(x)	((x) == ((unsigned short)(x)) && (x) > 0 )
#define IsInt16(x)	((x) == ((short) (x)))

static WMDecorations BaseWindow =
{
        WMDecorationCloseButton | WMDecorationResizeable | WMDecorationHeader,
        MENU_FULL,
        PIN_IN
};

static WMDecorations CmdWindow =
{
      WMDecorationCloseButton | WMDecorationResizeable | WMDecorationHeader |
      WMDecorationPushPin,
      MENU_LIMITED,
      PIN_IN
};

static WMDecorations PropWindow =
{
      WMDecorationCloseButton | WMDecorationResizeable |
      WMDecorationHeader | WMDecorationPushPin,
      MENU_LIMITED,
      PIN_IN
};

static WMDecorations NoticeWindow =
{
      WMDecorationHeader,
      MENU_NONE,
      PIN_IN
};

static WMDecorations HelpWindow =
{
      WMDecorationCloseButton | WMDecorationPushPin | WMDecorationHeader,
      MENU_LIMITED,
      PIN_IN
};

static WMDecorations OtherWindow =
{
      0L,
      MENU_NONE,
      PIN_IN
};

static WMDecorations TransientWindow =
{
      0L,
      MENU_NONE,
      PIN_IN
};

static WMDecorations MinimalWindow =
{
      WMDecorationResizeable,
      MENU_FULL,
      PIN_IN
};

typedef struct {
	char *class, *instance;
} minimalclosure;

/***************************************************************************
* forward private declarations
***************************************************************************/


/***************************************************************************
* private functions
*********t******************************************************************/

/* matchInstClass -- run through the list of names to be minimally
 * decorated, and see if this window's class or instance match
 * any.
 */
static Bool
matchInstClass(str,mc)
char *str;
minimalclosure *mc;
{
    if ((mc->class == NULL) || (strcmp(str,mc->class)))
	return ((mc->instance != NULL) && (!strcmp(str,mc->instance)));
    else
	return True;
}


/*
 * getOlWinDecors - given the window attributes and decoration add/delete
 *	requests, determine what kind of window (according to the OpenLook
 *	kinds of windows) the client represents, and determine what sort of
 *	decorations are appropriate.
 */
WMDecorations *
getOLWinDecors(dpy, win, pwinType, transient, wmInstance, wmClass)
Display *dpy;
Window  win;
int     *pwinType;
Bool    transient;
char *wmInstance, *wmClass;
{
        WMDecorations   *decors;
        Atom    actType;
        int     actFormat;
        unsigned        long    nItems, remain;
        long            offset;
        Atom    *decorChange;
        int     status;
        struct
        {
              Atom    win_type;
              Atom    menu_type;
              int     pin_initial_state;
        } *winAttrs;
	minimalclosure mc;

        decors = MemNew(WMDecorations);

	/* if the instance or class strings match any of the names
	 * listed for minimal decoration, only provide resize corners
	 * and a menu.
	 */
	mc.class = wmClass;
	mc.instance = wmInstance;
	if (ListApply(GRV.Minimals,matchInstClass,&mc) != NULL)
	{
            *decors = MinimalWindow;
            *pwinType = OLMINIMALDECOR;
	    return decors;
	}

	winAttrs = GetWindowProperty(dpy, win, AtomWinAttr, 0L,
                         3*LONG_LENGTH(Atom),   /* one winAttrs struct */
			 AtomWinAttr, 0, &nItems, &remain);

        /* if the property does not exist, then we assume that the
        * window is a base window
        */
        if ((winAttrs == NULL) || (nItems != 3))
        {
                if (transient)
                {
                        *decors = TransientWindow;
                        *pwinType = OLTRANSIENTWINDOW;
                }
                else
                {
                        *decors = BaseWindow;
                        *pwinType = OLBASEWINDOW;
                }
        }
        else
        {
                if (winAttrs->win_type == AtomWTBase)
                {
                        *decors = BaseWindow;
                        *pwinType = OLBASEWINDOW;
                }
                else if (winAttrs->win_type == AtomWTCmd)
                {
                        *decors = CmdWindow;
                        *pwinType = OLCMDWINDOW;
                }
                else if (winAttrs->win_type == AtomWTProp)
                {
                        *decors = PropWindow;
                        *pwinType = OLPROPWINDOW;
                }
                else if (winAttrs->win_type == AtomWTHelp)
                {
                        *decors = HelpWindow;
                        *pwinType = OLHELPWINDOW;
                }
                else if (winAttrs->win_type == AtomWTNotice)
                {
                        *decors = NoticeWindow;
                        *pwinType = OLNOTICEWINDOW;
                }
                else if (winAttrs->win_type == AtomWTOther)
                {
                        *decors = OtherWindow;
                        *pwinType = OLOTHERWINDOW;

                        /* this is the only case where we look at menu type */
                        if (winAttrs->menu_type == AtomMenuFull)
                                decors->menu_type = MENU_FULL;
                        else if (winAttrs->menu_type == AtomMenuLimited)
                                decors->menu_type = MENU_LIMITED;
                        else if (winAttrs->menu_type == AtomNone)
                                decors->menu_type = MENU_NONE;
                }

                /* 
		 * Check the pushpin's initial state
		 * REMIND  There's some backwards compatibility code here.  Older
		 * clients use the _OL_PIN_IN and _OL_PIN_OUT atoms here,
		 * whereas the OLXCI specifies zero as out and one as in.
		 * Handle both cases.
		 */
                if ((winAttrs->pin_initial_state == AtomPinIn) ||
		    (winAttrs->pin_initial_state == PIN_IN))
                        decors->pushpin_initial_state = PIN_IN;
                else if ((winAttrs->pin_initial_state == AtomPinOut) ||
			 (winAttrs->pin_initial_state == PIN_OUT))
                        decors->pushpin_initial_state = PIN_OUT;
                else 
                        decors->pushpin_initial_state = PIN_OUT;

                XFree(winAttrs);
        }



        /* are there any non-standard decorations ? */
        /* first check for decorations added */
        offset = 0L;
        for(;;)
        {
		decorChange = GetWindowProperty(dpy, win, AtomDecorAdd,
                                  offset, LONG_LENGTH(Atom), XA_ATOM, 0,
                                  &nItems, &remain);

                if ((nItems == 0) || (decorChange == NULL))
                        break;

                offset++;

                if (*decorChange == AtomDecorClose)
                        decors->flags |= WMDecorationCloseButton;
                else if (*decorChange == AtomDecorFooter)
                        decors->flags |= WMDecorationFooter;
                else if (*decorChange == AtomDecorResize)
                        decors->flags |= WMDecorationResizeable;
                else if (*decorChange == AtomDecorHeader)
                        decors->flags |= WMDecorationHeader;
                else if (*decorChange == AtomDecorPin)
                        decors->flags |= WMDecorationPushPin;
#ifdef OW_I18N
                else if (*decorChange == AtomDecorIMStatus)
                        decors->flags |= WMDecorationIMStatus;
#endif OW_I18N

                XFree(decorChange);
        }

        /* now check for decorations deleted */
        offset = 0L;
        for(;;)
        {
		decorChange = GetWindowProperty(dpy, win, AtomDecorDelete,
                                  offset, LONG_LENGTH(Atom), XA_ATOM, 0,
                                  &nItems, &remain);

                if ((nItems == 0) || (decorChange == NULL))
                        break;

                offset++;

                if (*decorChange == AtomDecorClose)
                        decors->flags &= ~(WMDecorationCloseButton);
                else if (*decorChange == AtomDecorFooter)
                        decors->flags &= ~(WMDecorationFooter);
                else if (*decorChange == AtomDecorResize)
                        decors->flags &= ~(WMDecorationResizeable);
                else if (*decorChange == AtomDecorHeader)
                        decors->flags &= ~(WMDecorationHeader);
                else if (*decorChange == AtomDecorPin)
                        decors->flags &= ~(WMDecorationPushPin);
#ifdef OW_I18N
                else if (*decorChange == AtomDecorIMStatus)
                        decors->flags &= ~(WMDecorationIMStatus);
#endif

                XFree(decorChange);
        }

        /* If the window wants both a pushpin and a close button it
        * only can have a pushpin.
        */
        if ((decors->flags & WMDecorationCloseButton) &&
            (decors->flags & WMDecorationPushPin))
                decors->flags &= ~(WMDecorationCloseButton);

        return  decors;
}


/*
 * calcPosition -- calculate the next position to place a new window.  This
 *	function places all new windows on the diagonal and makes sure that
 *	there is enough room on the screen for the new window's size passed
 *	in w and h.
 */
static void
calcPosition(dpy, screen, attrs)
Display		*dpy;
XWindowAttributes *attrs;
{
	static 	int	nextPosition = 0;
	int		stepValue;
	int		retVal;

	/* REMIND redo this to allow for window decorations when 
	 * calculating max size 
	 */
	/* if the height of the current window is too large ... */
	while (nextPosition + attrs->height > DisplayHeight(dpy, screen))
	{
		/* if we are already at the upper left corner ... */
		if (nextPosition == 0)
		{
			/* .. then set the height to just fit */
			attrs->height = DisplayHeight(dpy, screen);
			break;
		}
		else
			/* .. try to fit window in upper-left */
			nextPosition = 0;
	}

	/* if the width of the current window is too large to fit... */
	while (nextPosition + attrs->width > DisplayWidth(dpy, screen))
	{
		/* if we are already at the upper-left corner... */
		if (nextPosition == 0)
		{
			/* ... then set the width to just fit */
			attrs->width = DisplayWidth(dpy, screen);
			break;
		}
		else
			/* ... try to fit window in the upper-left */
			nextPosition = 0;
	}

	stepValue = 30;  /* REMIND This should really be based on HEADER_HEIGHT. */

	/* we will return the current position */
	attrs->x = attrs->y = nextPosition;

	/* calculate the next return value */
	nextPosition = (nextPosition + stepValue ) 
			% DisplayWidth(dpy, screen);
#ifdef DEBUG
	printf("calcPosition: returning x %d y %d\n",attrs->x,attrs->y);
#endif
}

/* iconifyOne -- iconify one client
 */
static void *
iconifyOne(cli, winIcon)
Client *cli;
WinGeneric *winIcon;
{
	DrawIconToWindowLines(cli->dpy, winIcon, cli->framewin);
	if (cli->groupmask == GROUP_DEPENDENT)
    	    RemoveSelection(cli);	/* warp if necessary */
	XUnmapWindow(cli->dpy, cli->framewin->core.self);
	XUnmapWindow(cli->dpy, PANEWINOFCLIENT(cli));
	cli->framewin->fcore.panewin->pcore.pendingUnmaps++;
	cli->wmState = IconicState;
	ClientSetWMState(cli);
	return NULL;
}

/* deiconifyOne -- deiconify one client
 */
static void *
deiconifyOne(cli, winIcon)
Client *cli;
WinGeneric *winIcon;
{
	DrawIconToWindowLines(cli->dpy, winIcon, cli->framewin);
	XMapRaised(cli->dpy, cli->framewin->core.self);
	XMapRaised(cli->dpy, PANEWINOFCLIENT(cli));
	cli->wmState = NormalState;
	ClientSetWMState(cli);
	return NULL;
}

/***************************************************************************
* global functions
***************************************************************************/


/*
 * StateNew -- A client is mapping a top-level window (either a new window
 *	or a Withdrawn window).  The window may become Iconic or Normal 
 *	depending on the hints.  Check to see if this window needs to be mapped
 *	and if so add the required adornments.
 *		dpy 		-- display pointer
 *		window 		-- client's window
 *		fexisting	-- the window already exists and we
 *				   are starting olwm, so positioning should
 *				   be special-cased
 *		ourWinInfo	-- if is this one of our menu windows, this
 *			will be its WinMenu structure; this window must
 *			be a subclass of Pane
 */
Client *
StateNew(dpy, window, fexisting, ourWinInfo)
Display *dpy;
Window window;
Bool fexisting;
WinPane *ourWinInfo;
{
	Client 		*cli;
	WinPane		*winPane;
	WinIconFrame	*winIcon;
	WinPaneFrame	*winFrame;
	WinIconPane	*winIconPane;
	Window          transient_for;
	NewXSizeHints	*normalHints = NULL;
	long		mask;
	Bool		transient = False;
	Atom		actType;
	int		actFormat;
	unsigned long	nItems;
	unsigned long	remain;
	int		status;
	int		initstate;
	int		count;
	Atom		*protocols;
	XWindowAttributes paneAttr;
	int 		screen = DefaultScreen(dpy);
	int		tmpx, tmpy;
	char		*classInstance;

	/* if it's a transient window, do not adorn it */
	transient = XGetTransientForHint(dpy, window, &transient_for);

	/* 
	 * if this window is not a child of root, then it will be ignored
	 */
	if (ourWinInfo == NULL)
	{
		Window	root, parent, *children, iconwin;
		int	nChild;
		Status	result;
		
		result = XQueryTree(dpy, window,
		    &root, &parent, &children, &nChild);

		if ((result == 0) || (parent != RootWindow(dpy, screen)))
		    return NULL;
	}

	/* by selecting for DestroyNotifys here, we are guaranteed that
	 * if the window goes away we will eventually hear about it.
	 * The GetWindowAttributes following this select ensures that
	 * we don't have a race condition in finding out.
	 */
	if (!ourWinInfo)
	    XSelectInput(dpy, window, StructureNotifyMask);

        /* get all the info about the new pane */
        status = XGetWindowAttributes(dpy, window, &paneAttr);
        if ( status == 0 ) {
#ifdef DEBUG
            ErrorWarning("StateNew: GetWindowAttributes failed!");
#endif /* DEBUG */
            return NULL;
        }

	if (paneAttr.override_redirect)
	{
	    return NULL;
	}

	/*
	 * If it's an already-existing window, but it's unmapped, just
	 * ignore it.
	 */
	if (fexisting && paneAttr.map_state != IsViewable) {
	    return NULL;
	}

	/* Create the client structure so we can start hooking things to it */
	if ((cli = ClientCreate(dpy,screen)) == NULL)
	{
	    return NULL;
	}

	/*
	 * Get the WM_NORMAL_HINTS property.  If it's short, then we have a
	 * pre-ICCCM client on our hands, so we'll have to fill in some of the
	 * missing values.
	 */
	normalHints = GetWindowProperty(dpy, window, XA_WM_NORMAL_HINTS,
		0L, LONG_LENGTH(NewXSizeHints), XA_WM_SIZE_HINTS, 0,
		&nItems, &remain);

	/* clean up the normal hints we may have gotten back */
	if (normalHints == NULL)
	{
		/*
		 * We didn't get the property; punt.
		 */
		normalHints = MemNew(NewXSizeHints);
	} 
	else if ( nItems == LONG_LENGTH(NewXSizeHints)-3 ) 
	{
		/* REMIND check: does this section work properly on an X11R4
		 * property? 
		 */
		/*
		 * We got a short property.  Assume that this is a pre-X11R4
		 * client who's using the short version of the property.  Copy
		 * the data into a correctly-sized structure.  Then, depending
		 * on the flags set, ignore the window's real geometry and use
		 * the data in the hint (but only if it passes some sanity 
		 * checking).  The sanity checking is necessary because early 
		 * versions of XView write a short property, but rely on the 
		 * window manager to look at the window's geometry instead of 
		 * at the values in the hint.
		 */
		NewXSizeHints *temp;

		temp = MemNew(NewXSizeHints);
		bcopy(normalHints,temp,
		      sizeof(NewXSizeHints)-3*sizeof(long));
		XFree(normalHints);
		normalHints = temp;

		if ((!fexisting) && ((normalHints->flags & (USPosition|PPosition))
		    && IsInt16(normalHints->pad.x)
		    && IsInt16(normalHints->pad.y))) {
		    paneAttr.x = normalHints->pad.x;
		    paneAttr.y = normalHints->pad.y;
		}
		if ((!fexisting) && ((normalHints->flags & (USSize|PSize))
		    && IsCard16(normalHints->pad.width)
		    && IsCard16(normalHints->pad.height)
		    && normalHints->pad.width >= MINSIZE
		    && normalHints->pad.height >= MINSIZE
		    && normalHints->pad.width < 2*DisplayWidth(dpy,
						    screen)
		    && normalHints->pad.height < 2*DisplayHeight(dpy,
						    screen)))
		{
		    paneAttr.width = normalHints->pad.width;
		    paneAttr.height = normalHints->pad.height;
		}
	}
	else if (nItems != LONG_LENGTH(NewXSizeHints))
	{
		/*
		 * We got a property of the wrong length; punt.
		 */
		XFree(normalHints);
		normalHints = MemNew(NewXSizeHints);
	}

        /* If the hints structure did not have a min size set in
         * it, zero the fields out so the MAX function below will
         * work.  We can't be sure that the min width and height
         * fields have zero in them just because they haven't been
         * set.
         */
        if (!(normalHints->flags & PMinSize))
        {
                normalHints->min_width = 0;
                normalHints->min_height = 0;
        }

        /* We set up the min size to be at least MINSIZE (for pane). */
        normalHints->flags |= PMinSize;
        normalHints->min_width = MAX(MINSIZE, normalHints->min_width);
        normalHints->min_height = MAX(MINSIZE, normalHints->min_height);

	cli->normHints = normalHints;

	if ( !fexisting &&
	     !(normalHints->flags & USPosition) &&
	    (!(normalHints->flags & PPosition) ||
	     (GRV.PPositionCompat && paneAttr.x <= 1 && paneAttr.y <= 1)))
	{
	    /*
	     * The hints don't specify a position, so we choose a suitable
	     * one, taking into account the window size (to prevent the window
	     * from lapping off the screen).  
	     */
	    calcPosition(dpy, screen, &paneAttr);
	}

	if (!(normalHints->flags & PWinGravity))
	{
		normalHints->win_gravity = NorthWestGravity;
		normalHints->flags |= PWinGravity;
	}
	else if (normalHints->win_gravity == 0)
	{
		normalHints->win_gravity = NorthWestGravity;
	}

	/* Get the WM_HINTS; if the property does not exist then we will use
	 * NULL throughout.  All references to wmHints must be guarded for a
	 * NULL value.  */
	cli->wmHints = GetWindowProperty(dpy, window, XA_WM_HINTS, 0L,
		LONG_LENGTH(XWMHints), XA_WM_HINTS, 0, &nItems, &remain);

	if (cli->wmHints == NULL)
		cli->wmHints = MemNew(XWMHints);
	else if (nItems == sizeof(XWMHints)/sizeof(int))
                cli->hasMessages = True;
        else
                cli->hasMessages = False;

	/* Get the window class and instance strings */
	classInstance = GetWindowProperty(dpy, window, AtomClass, 0L,
		100000L, XA_STRING, 0, &nItems, &remain);
	if (classInstance != NULL)
	{
	    cli->wmInstance = MemNewString(classInstance);
	    cli->wmClass = MemNewString(classInstance + strlen(classInstance) + 1);
	    XFree(classInstance);
	}

	/* Get the OpenLook window type and associated decorations */
	cli->wmDecors = getOLWinDecors(dpy, window, &(cli->olType),transient,cli->wmInstance, cli->wmClass);

	/* Get the protocols in which the client will participate */
	protocols = GetWindowProperty(dpy, window, AtomProtocols, 0L,
		3*LONG_LENGTH(Atom), XA_ATOM, 0, &nItems, &remain);

        cli->protocols = 0;
	if (protocols != NULL)
	{
            for (count = 0; count < nItems; count++)
            {
                if (protocols[count] == AtomTakeFocus)
                        cli->protocols |= TAKE_FOCUS;
                else if (protocols[count] == AtomSaveYourself)
                        cli->protocols |= SAVE_YOURSELF;
                else if (protocols[count] == AtomDeleteWindow)
                        cli->protocols |= DELETE_WINDOW;
            }
            XFree(protocols);
	}

        /* We need to check to see if this window is a group leader
         * or not.  If it is a group leader then we do nothing with
         * it.  If it is not a group leader, we need to find the group
         * leader's frame and add ourselves as a follower.  Note that
	 * on startup windows get mapped with leaders first.
         */
        if ((cli->wmHints) && (cli->wmHints->flags & WindowGroupHint))
	{
	    cli->groupid = cli->wmHints->window_group;
	}
	else
	{
	    cli->groupid = window;
	}
	if (cli->groupid == window)
	    cli->groupmask = GROUP_LEADER;
	else
	{
	    if ((cli->protocols & DELETE_WINDOW) &&
		(cli->wmDecors->flags & WMDecorationPushPin) &&
		(cli->wmDecors->menu_type == MENU_LIMITED))
	    {
		cli->groupmask = GROUP_DEPENDENT;
	    }
	    else
	    {
		cli->groupmask = GROUP_INDEPENDENT;
	    }
	}
	GroupAdd(cli->groupid,cli,cli->groupmask);

        /* figure out what focus mode this window intends */
        if ((cli->wmHints) && (cli->wmHints->input))
        {
                if (cli->protocols & TAKE_FOCUS)
                        cli->focusMode = LocallyActive;
                else
                        cli->focusMode = Passive;
        }
        else /* wmHints->input == False */
        {
                if (cli->protocols & TAKE_FOCUS)
                        cli->focusMode = GloballyActive;
                else
                        cli->focusMode = NoInput;
        }


	/* officially set up the frame */
	winFrame = MakeFrame(cli,window,&paneAttr);

	if (ourWinInfo == NULL)
	{
		/* officially set up the pane */
		winPane = MakePane(cli,winFrame,window,&paneAttr);
	}
	else
	{
		winPane = ourWinInfo;
		(WinClass(winPane)->core.createcallback)(ourWinInfo,cli,winFrame);
	}

	/* officially set up the icon */
	winIcon = MakeIcon(cli,window,&paneAttr);
	winIconPane = MakeIconPane(cli,winIcon,cli->wmHints,fexisting);

	/* size and generally configure the frame window tree */
	FrameSetPosFromPane(winFrame,paneAttr.x,paneAttr.y);
	WinCallConfig(dpy, winPane, NULL);

	/* size and generally configure the icon window tree */
	WinCallConfig(dpy, winIconPane, NULL);
	if (cli->wmHints != NULL)
	    IconSetPos(winIcon,cli->wmHints->icon_x,cli->wmHints->icon_y);
	else
	    IconSetPos(winIcon,0,0);
	WinCallConfig(dpy, winIcon, NULL);
        /* we manually move the icon pane window, since all the configuration has
         * been done with the icon pane parented to root */
        WinRootPos(winIconPane, &tmpx, &tmpy);
        XMoveWindow(dpy, winIconPane->core.self, tmpx, tmpy);

	/* Determine the proper initial state of the window. */
	if (fexisting) {
	    int state;
	    Window *iw; /* REMIND: ignored for now */
	    /* For existing windows, look for a WM_STATE property. */
	    if (ClientGetWMState(dpy, winPane->core.self, &state, &iw)
		&& state == IconicState)
		initstate = IconicState;
	    else
		initstate = NormalState;
	} else {
	    /* For new windows, check the initial_state field of WM_HINTS. */
	    if (cli->wmHints && (cli->wmHints->flags & StateHint)
		&& (cli->wmHints->initial_state == IconicState))
		    initstate = IconicState;
	    else
		    initstate = NormalState;
	}

	if (cli->groupmask == GROUP_DEPENDENT)
	{
	    Client *leader = GroupLeader(cli->groupid);
	    if (leader != NULL)
		initstate = leader->wmState;
	}

	switch ( initstate ) {
	case NormalState:
	    cli->wmState = NormalState;
	    XMapRaised(dpy, winFrame->core.self);
	    XMapRaised(dpy, winPane->core.self);
	    if (!fexisting)
		FrameWarpPointer(cli);
	    break;
	case IconicState:
	    cli->wmState = IconicState;
	    /* dependent group followers don't get their own icons */
	    if (cli->groupmask != GROUP_DEPENDENT)
		IconShow(cli, winIcon);
	    break;
	}
	ClientSetWMState(cli);

	return cli;
}

/*
 * ReparentTree -- called at start up, this routine queries the window
 *	tree and reparents all the windows 
 */
void
ReparentTree(dpy)
Display	*dpy;
{
	int numChildren;
	Window *children, root, parent, w;
	XWindowAttributes winAttr;
	int ii;
	Client *cli;

	children = NULL;

	if (XQueryTree(dpy, RootWindow(dpy, DefScreen), &root, &parent,
				      &children, &numChildren)) 
	{
	    for (ii=0; ii<numChildren; ii++)
	    {
		w = children[ii];
		if (WIGetInfo(w) == NULL)
		{
#ifdef DEBUG
	            printf("setting: %d\n",w);
#endif
	            cli = StateNew(dpy, w, True, NULL);
		    if (cli != NULL)
		    {
			cli->framewin->fcore.panewin->pcore.pendingUnmaps++;	
					/* unmap because of reparent */
		    }
		}
	    }
	}

	if (children != NULL)
		XFree(children);
}


/* 
 * StateNormIcon - transition a window to IconicState from NormalState
 */
void
StateNormIcon(cli)
Client *cli;
{
	WinIconFrame *winIcon = cli->iconwin;

	/* don't do some other transition */
	if (cli->wmState != NormalState)
		return;

	/* we can't iconify if we are a dependent */
	if (cli->groupmask == GROUP_DEPENDENT)
		return;

	/* Map the icon window */
	IconShow(cli, winIcon);

	/* iconify self and dependents */
	if (cli->groupmask == GROUP_LEADER)
	    GroupApply(cli->groupid,iconifyOne,winIcon,GROUP_LEADER|GROUP_DEPENDENT);
	else
	    iconifyOne(cli,winIcon);
}

/* 
 * StateIconNorm - transition a window to NormalState from IconicState 
 */
void
StateIconNorm(cli)
Client *cli;
{
	WinIconFrame *winIcon = cli->iconwin;

	/* don't do some other transition */
	if (cli->wmState != IconicState)
		return;

	if (cli->groupmask == GROUP_LEADER)
	    GroupApply(cli->groupid,deiconifyOne,winIcon,
		GROUP_LEADER|GROUP_DEPENDENT);
	else
	    deiconifyOne(cli,winIcon);

	/* Unmap icons */
	IconHide(cli, winIcon);
}

/*
 * StateWithdrawn - a window is being withdrawn; tear down all related
 *	structures; clear the client out of all lists it may be
 * 	on; reparent the pane window
 */
void
StateWithdrawn(cli)
Client *cli;
{
        WinIconPane *iconPaneInfo;
	WinIconFrame *iconInfo = cli->iconwin;
	WinPaneFrame *frameInfo;
	WinPane *paneInfo;
	Window pane;
	Display *dpy = cli->dpy;
	int rx, ry;
	XClientMessageEvent cm;

#ifdef DEBUG
	printf("StateWithdrawn: cli %x\n", cli);
#endif
        frameInfo = cli->framewin;
	paneInfo = (WinPane*)(frameInfo->fcore.panewin);
	pane = paneInfo->core.self;
	iconInfo = cli->iconwin;

	if (cli->isFocus) {
	    cm.type = ClientMessage;
	    cm.message_type = AtomProtocols;
	    cm.format = 32;
	    cm.window = NoFocusWin;
	    cm.data.l[0] = AtomTakeFocus;
	    cm.data.l[1] = TimeFresh(frameInfo);
	    XSendEvent(dpy, NoFocusWin, False, 0, &cm);
	    FocusIndeterminate = True;
	}

        /* Return the pointer if necessary */
	FrameUnwarpPointer(cli);

        /* Unmap the frame. */
        XUnmapWindow(dpy, frameInfo->core.self);

        /* Unmap the pane so we get a map
         * request when the client wants a transition
         * from Withdrawn to Normal or Iconic.
         */
        XUnmapWindow(dpy, pane);

	/* Unmap the icon */
	if (iconInfo != NULL)
	    IconHide(cli, iconInfo);

	/* move the pane and unparent it */
	FrameUnparentPane(cli, frameInfo, paneInfo);
	DestroyClient(cli);
}
