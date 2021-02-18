/* 
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

/* client.c - functions relating to clients as a whole 
 */

static	char	sccsid[] = "@(#) client.c 50.5 90/12/12 Crucible";
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
#include "events.h"
#include "mem.h"
#include "olwm.h"
#include "win.h"
#include "menu.h"
#include "group.h"
#include "globals.h"

/***************************************************************************
* global data
***************************************************************************/

List *ActiveClientList;

/*
 * FocusIndeterminate indicates that no window has the focus and that the 
 * focus should be transferred to the NoFocus window.  FocusIndeterminate is 
 * set when the window with the focus becomes Withdrawn.  Since the Unmap-
 * Notify event has no timestamp, setting the focus to the NoFocus window 
 * immediately is difficult to do properly.  Instead, this flag is set and a 
 * message is sent to the NoFocus window.  If this flag is still set when the 
 * message is received, the focus is set to the NoFocus window.  This flag is 
 * unset whenever the focus changes, so the transfer of focus to the NoFocus 
 * window is nullified if there has been an intervening focus transfer.
 */
Bool FocusIndeterminate = False;

extern Atom AtomColorMapWindows;
extern Atom AtomDeleteWindow;
extern Atom AtomLeftFooter;
extern Atom AtomProtocols;
extern Atom AtomRightFooter;
extern Atom AtomShowProperties;
extern Atom AtomTakeFocus;
extern Atom AtomWindowBusy;
extern Atom AtomWMState;
#ifdef OW_I18N
extern Atom AtomLeftIMStatus;
extern Atom AtomRightIMStatus;
#endif

extern Window NoFocusWin;

extern void RecursiveRefresh();

/***************************************************************************
* private data
***************************************************************************/

/***************************************************************************
* private functions
***************************************************************************/

static void clientSetBusy();

/***************************************************************************
* global functions
***************************************************************************/


/* ClientSendProtocol - send a protocol message to a client
 */
void *
ClientSendProtocol(cli,proto,evtime)
Client *cli;
Atom proto;
Time evtime;
{
	WinGeneric      *winInfo;
	XEvent          clientEvent;
	WinPaneFrame	*winFrame;

	clientEvent.xclient.type = ClientMessage;
	clientEvent.xclient.message_type = AtomProtocols;
	clientEvent.xclient.format = 32;
	clientEvent.xclient.display = cli->dpy;
	winFrame = cli->framewin;
	clientEvent.xclient.window = ClientPane(cli);
	clientEvent.xclient.data.l[0] = proto;
	clientEvent.xclient.data.l[1] = evtime;

	XSendEvent(cli->dpy, clientEvent.xclient.window, False,
		NoEventMask, &clientEvent);
	return NULL;
}

/* ClientShowProps - send a ClientMessage of type WM_SHOW_PROPERTIES
 */
void
ClientShowProps(cli)
Client *cli;
{
	XEvent clientEvent;
	
	clientEvent.xclient.type = ClientMessage;
	clientEvent.xclient.message_type = AtomShowProperties;
	clientEvent.xclient.format = 32;
	clientEvent.xclient.display = cli->dpy;
	clientEvent.xclient.window = PANEWINOFCLIENT(cli);

	XSendEvent(cli->dpy, clientEvent.xclient.window, False,
		NoEventMask, &clientEvent);
}

/* ClientKill - a client must be killed.  If it can handle the DELETE_WINDOW
 *	protocol, use it; otherwise, if we are forcing the client to go
 *	away, kill it.
 */
void *
ClientKill(cli,pforce)
Client *cli;
Bool pforce;
{
	if (cli->protocols & DELETE_WINDOW)
		ClientSendProtocol(cli,AtomDeleteWindow,LastEventTime);
	else
		if (pforce)
			XKillClient(cli->dpy,ClientPane(cli));
	return NULL;	
}


/* UnparentClient - while exiting OLWM: unmap all icons that are on the
 * screen.  Reparent all windows back to the root, suitably offset
 * according to their window-gravities.  Also remap all non-withdrawn
 * windows, and remove all Withdrawn windows from the save-set (so
 * they don't get remapped.  REMIND: We have to do this because
 * Withdrawn windows are still left reparented inside the frame; this
 * shouldn't be the case.
 */
void *
UnparentClient(cli,junk)
Client *cli;
void *junk;
{
	WinPaneFrame *frameInfo;
	WinPane *paneInfo;
	Window pane;
	int rx, ry;

#ifdef DEBUG
	printf("UnparentClient: %x\n",cli);
#endif

	if (cli->wmState == IconicState)
	{
		IconHide(cli, cli->iconwin);
	}

	frameInfo = cli->framewin;
	paneInfo = (WinPane*)(frameInfo->fcore.panewin);
	pane = paneInfo->core.self;
	FrameUnparentPane(cli, frameInfo, paneInfo);

	if (cli->wmState == DontCareState) 
	{ 
		XChangeSaveSet(cli->dpy, pane, SetModeDelete);
	} 
	else 
	{
		XMapWindow(cli->dpy,pane);
	}

	return NULL;
}

/*
 * addClient -- add this client structure to the list of active clients
 */
static void
addClient(cli)
Client *cli;
{
	List *l = ActiveClientList;
	Client *tc;

#ifdef DEBUG
	printf("addClient: %x\n",cli);
#endif
        /* First look to see if window is already listed.  */
        for(tc = ListEnum(&l); tc != NULL; tc = ListEnum(&l))
        {
                if (tc == cli)
                        return;
        }

        /* Wasn't present, add to list. */
	ActiveClientList = ListCons(cli, ActiveClientList);
}

/*
 * removeClient -- remove this client structure from the list of active
 *      clients.
 */
static void
removeClient(cli)
Client *cli;
{
	List **l;

	for (l = &ActiveClientList ; *l != NULL; l = &((*l)->next))
	{
		if ((*l)->value == cli)
		{
			ListDestroyCell(l);
			return;
		}
	}
}

/* 
 * DestroyClient -- destroy all resources associated with this client
 */
void 
DestroyClient(cli)
Client *cli;
{
	/* remove any possible external references to the client */

	UnTrackSubwindows(cli, True);
	if (IsSelected(cli))
		RemoveSelection(cli);
	removeClient(cli);

	/* destroy the window resources associated with the client */
	WinCallDestroy(cli);

	GroupRemove(cli->groupid, cli);

	/* free up the client structure resources */
	/* REMIND  there's lots of other stuff to free up here */
	/* REMIND  what's to be done with followers here? */
	MemFree(cli);
}

/*
 * ClientConfigure - a configure request event has been received on the
 * pane.  Configure the windows accordingly.
 */
void
ClientConfigure(cli,win,pxcre)
Client *cli;
WinGeneric *win;
XConfigureRequestEvent *pxcre;
{
        XWindowChanges          winChange;

	if ((cli == NULL) || (win == NULL))
	{
                /* We don't know about this window, or it's withdrawn
                 * convert the request into an XConfigureWindow
                 * call. We do not look at the hints to see if
                 * the resize is in the proper increments, but since
                 * the app is asking for the reconfigure this seems
                 * right.
                 */
                winChange.x = pxcre->x;
                winChange.y = pxcre->y;
                winChange.width = pxcre->width;
                winChange.height = pxcre->height;
                winChange.border_width = pxcre->border_width;
                winChange.stack_mode = pxcre->detail;
                winChange.sibling = pxcre->above;

                XConfigureWindow(pxcre->display,
                         pxcre->window,
                         /* lint will warn: this is a long, not int */
                         pxcre->value_mask,
                         &winChange );
	}
	else /* cli->wmState == NormalState or IconicState */
	{
		WinCallConfig(cli->dpy,win,pxcre);
	}
}

/*
 * ClientGetWMState      -- get the contents of the WM_STATE property.
 */
Bool
ClientGetWMState( dpy, win, state, iconwin )
    Window win;
    int *state;
    Window *iconwin;
{
    int status;
    Atom type;
    int format, nitems, remain;
    int *data;

    data = GetWindowProperty(dpy, win, AtomWMState, 0L, 
		LONG_LENGTH(int)+LONG_LENGTH(Window), AtomWMState, 32, &nitems,
                &remain);
    if (data == NULL)
    {
	return False;
    } 
    else if (nitems != 2)
    {
        XFree(data);
        return False;
    }
    *state = data[0];
    *iconwin = (Window) data[1];
    XFree(data);
    return True;
}

/*
 * ClientSetWMState     -- set the contents of the WM_STATE property,
 *                         given the information in the WinInfo struct.
 */
void
ClientSetWMState( cli )
    Client *cli;
{
    unsigned long data[2];
    WinPaneFrame *frameInfo = cli->framewin;
    WinIconFrame *iconWinInfo = cli->iconwin;
    Window pane = PANEWINOFCLIENT(cli);

    data[0] = cli->wmState;
    if ( iconWinInfo ) {
        if ( iconWinInfo->fcore.panewin )
            data[1] = iconWinInfo->fcore.panewin->core.self;
        else
            data[1] = iconWinInfo->core.self;
    } else {
        data[1] = None;
    }
    XChangeProperty(cli->dpy, pane, AtomWMState, AtomWMState,
                    32, PropModeReplace, (unsigned char *)data, 2);
}


/* ClientCreate -- allocate and initialise a client structure 
 */
Client *
ClientCreate(dpy,screen)
Display *dpy;
int screen;
{
	Client *cli = MemNew(Client);

	cli->wmState = DontCareState;
	cli->dpy = dpy;
	cli->screen = screen;
	/* all other fields set to zero by allocation function */
	addClient(cli);
	return cli;
}

/* ClientPane - return the pane window of a client
 */
Window
ClientPane(cli)
Client *cli;
{
	WinPaneFrame *wf;
	WinPane *wp;

	if ((wf = cli->framewin) == NULL)
		return NULL;
	if ((wp = (WinPane *)(wf->fcore.panewin)) == NULL)
		return NULL;
	return wp->core.self;
}

/* ClientDistributeProperty -- a property of the client has changed.
 *	Forward the change notification to the appropriate handler.
 */
 /* REMIND this is pretty sketchy at the moment */
void
ClientDistributeProperty(cli, event)
Client *cli;
XPropertyEvent *event;
{

	/* REMIND check that we're seeing a change, not a delete
	 * on some of these */

	if (event->atom == AtomColorMapWindows)
	{
		if (event->state == PropertyNewValue)
		{
			TrackSubwindows(cli);
		}
		else
		{
			UnTrackSubwindows(cli, False);
		}
	}
	else if (event->atom == XA_WM_ICON_NAME)
	{
		IconChangeName(cli->iconwin, PANEWINOFCLIENT(cli));
	}
	else if ((event->atom == AtomRightFooter) || (event->atom == AtomLeftFooter))
	{
		FrameNewFooter(cli);
	}
#ifdef OW_I18N
	else if ((event->atom == AtomRightIMStatus)
	      || (event->atom == AtomLeftIMStatus))
	{
		FrameNewIMStatus(cli);
	}
#endif OW_I18N
	else if (event->atom == XA_WM_NAME)
	{
		FrameNewHeader(cli);
	}
        else if (event->atom == AtomWindowBusy)
        {
		clientSetBusy(cli,event->time,event->state);
        }
}

/* refresh the window.  We do this by creating a window on top
 * of the window to refresh and then immediately destroy it
 */
void
ClientRefresh(cli)
Client *cli;
{
	WinPaneFrame *winFrame = cli->framewin;
	Window w;
	XSetWindowAttributes xswa;

    if (GRV.RefreshRecursively) {
	RecursiveRefresh(cli->dpy, winFrame->core.self);
    } else {
	w = XCreateWindow(cli->dpy, winFrame->core.self, 0, 0,
	    winFrame->core.width, winFrame->core.height, 0,
	    CopyFromParent, InputOutput, CopyFromParent,
	    0, &xswa);
	XMapRaised(cli->dpy,w);
	XDestroyWindow(cli->dpy,w);
    }
}


/* ClientInBox -- given a bounding box, apply a function to all clients
 *      which fall inside the rectangle
 */
void *
ClientInBox(cli, close)
Client *cli;
clientInBoxClosure *close;
{
	int x, y, w, h;

	if (cli->wmState == IconicState)
	{
		x = cli->iconwin->core.x;
		y = cli->iconwin->core.y;
		w = cli->iconwin->core.width;
		h = cli->iconwin->core.height;
	}
	else
	{
		x = cli->framewin->core.x;
		y = cli->framewin->core.y;
		w = cli->framewin->core.width;
		h = cli->framewin->core.height;
	}

	if ((x >= close->bx) &&
	    (y >= close->by) &&
	    ((x + w) <= (close->bx + close->bw)) &&
	    ((y + h) <= (close->by + close->bh)))
		(close->func)(cli, close->timestamp);

	return NULL;
}

/*
 * ClientSetBusy -- the busy state has (possibly) been changed for a client.
 *	if the client is going from normal to busy:
 *          mark the client as busy
 *          put up a busy window
 *      if the client is going from busy to normal:
 *          mark the client as unbusy
 *          take down a busy window (if it exists)
 */
static void
clientSetBusy(cli,evtime,state)
    Client *cli;
    Time evtime;
    int state;
{
        int *newBusyPtr;
	int newBusy;
        Atom actType;
        int actFormat;
        unsigned long nItems, remain;
	int status;

	if (state == PropertyNewValue) {
	    newBusyPtr = GetWindowProperty(cli->dpy, PANEWINOFCLIENT(cli),
		AtomWindowBusy, 0L, LONG_LENGTH(*newBusyPtr), 
		XA_INTEGER, 32, &nItems, &remain);

	    if (newBusyPtr == NULL) {
		/* property not found or has the wrong type */
		newBusy = 0;
	    } else {
		if (nItems != LONG_LENGTH(newBusy) || remain != 0) {
		    /* got a property, but it is invalid */
		    newBusy = 0;
		} else {
		    /* valid property */
		    newBusy = *newBusyPtr;
		}
		XFree(newBusyPtr);
	    }
	} else {
	    /* property was deleted */
	    newBusy = 0;
	}

        if (cli->isBusy && (newBusy == 0)) {
	    /* losing busy */
            cli->isBusy = False;
	    if (cli->isFocus)
		ClientSetFocus(cli,True,evtime);
	    FrameSetBusy(cli->framewin, False);
        }
        else if (!cli->isBusy && (newBusy == 1)) {
	    /* gaining busy */
            cli->isBusy = True;
	    FrameSetBusy(cli->framewin, True);
	    if (cli->isFocus)
	    	    ClientSetFocus(cli,True,evtime);
        }
}

/*
 * ClientSetFocus -- possibly set the focus to this client.  If the
 *	client is GloballyActive, we only send TakeFocus messages
 *	if sendTF is true.  If the client has a different focus
 *	mode sendTF is ignored.
 */
void
ClientSetFocus(cli,sendTF,evtime)
Client *cli;
Bool sendTF;
Time evtime;
{
	FocusIndeterminate = False;

	if (cli->isBusy)
	{
	    /* REMIND
	     * setting RevertToNone is problematic.  When the window goes 
	     * unbusy, the winBusy will be unmapped, which may leave the focus 
	     * at None until the client gets around to responding to our
	     * WM_TAKE_FOCUS message.
	     */
	    if (cli->framewin->winBusy != NULL)
		XSetInputFocus(cli->dpy, cli->framewin->winBusy->core.self,
			RevertToNone, evtime);
	    else
		XSetInputFocus(cli->dpy, NoFocusWin,
			RevertToPointerRoot, evtime);
	}
	else
	{
	    switch (cli->focusMode)
	    {
	    case NoInput:
	        /* do nothing */
	        break;

	    case Passive:
	    case LocallyActive:
	        XSetInputFocus(cli->dpy, PANEWINOFCLIENT(cli),
		    RevertToParent, evtime);
	        break;

	    case GloballyActive:
	        if (sendTF)
	        {
		    ClientSendProtocol(cli, AtomTakeFocus, evtime);
	        }
	        break;
	    }
	}
}
