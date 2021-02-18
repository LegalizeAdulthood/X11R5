/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) Select.c 50.4 90/12/12 Crucible";
#ifdef OW_I18N
#include "i18n.h"
#endif
#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "olwm.h"
#include "win.h"
#include "list.h"

static List *selectList = NULL_LIST;

Time SelectionTime;

/* Externals */
extern Atom	AtomLength;
extern Atom	AtomListLength;
extern Atom	AtomTargets;
extern Atom	AtomTimestamp;
extern Atom AtomOlwmTimestamp;

/*
 * IsSelected -- returns boolean telling whether or not client is selected.
 *
 * Input:
 *
 *	cli	- client which might be on selection list.
 *
 * Output:
 *
 *	Returns True if the client cli is on the selection list.
 *	Returns False if it is not.
 *
 * Globals:
 *
 *	selectList	- The list of selected windows.
 */
Bool
IsSelected(cli)
Client *cli;
{
	return cli->isSelected;
}


/*
 * AddSelection -- add this client to the selection list if it is not 
 *	already present.  This routine also tells the server it is the current 
 *	XA_PRIMARY selection owner.
 *
 * Input:
 *
 *	cli	- The client we are adding to the selectList.
 *
 * Output:
 *
 *	None.
 *
 * Globals:
 *
 *	selectList	- The list of selected windows.
 *	The current selection owner in the server is also adjusted.
 */
int
AddSelection(cli, timestamp)
Client *cli;
Time timestamp;
{
	List *l = selectList;
	Client *tc;

	if (selectList == NULL_LIST)
	{
		/* Tell the server that we are now the 
		 * current selection owner.
		 * We really only need to call this 
		 * if this is the first window
		 * in the SelectList.  Calling this sends the
		 * previous owner of the XA_PRIMARY selection 
		 * a SelectionClear event, unless the previous 
		 * owner was us already.
	 	 */
		XSetSelectionOwner(cli->dpy, XA_PRIMARY, 
				   RootWindow(cli->dpy, cli->screen),
				   timestamp);
	}
	else
	{
		/* First look to see if window is already listed. */
		for(tc = ListEnum(&l); tc != NULL; tc = ListEnum(&l))
		{
			if (tc == cli)
				return;
		}
	}

	/* If we get here the window wasn't already in the list. */
	selectList = ListCons(cli,selectList);
	cli->isSelected = True;

	/* Tell the window it is selected. */
	WinCallSelect(cli, True);
	SelectionTime = timestamp;
}

/*
 * RemoveSelection -- de-select the window
 *
 * Purpose:
 * 	This routine will remove an entry from the SelectList.
 * Input:
 *
 *	winInfo	- The window we are removing from the SelectList.
 *
 * Output:
 *	Returns True if client was deselected; false if the
 *	client was not already selected.
 *
 * Globals:
 *
 *	selectList	- The list of selected windows.
 */
Bool
RemoveSelection(cli)
Client *cli;
{
	List **l;

	for (l = &selectList ; *l != NULL; l = &((*l)->next))
	{
		if ((*l)->value == cli)
		{
			ListDestroyCell(l);
			cli->isSelected = False;
			WinCallSelect(cli,False);
			return True;
		}
	}
	return False;
}



/*
 * ToggleSelection -- if the winInfo is already selected, then de-select it.
 *	If not, then select it.
 *	Returns whether it is selected or not.
 */
Bool
ToggleSelection(cli, timestamp)
Client *cli;
Time timestamp;
{

	/* If already present, we want to deselect. */
	if (RemoveSelection(cli))
		return False;
	else
	{
		AddSelection(cli, timestamp);
		return True;
	}
}


/*
 * ClearSelections -- clear the selection list
 */
void
ClearSelections(dpy)
Display	*dpy;
{
	List *l;
	Client *cli;

	l = selectList;
	for(cli = ListEnum(&l); cli != NULL; cli = ListEnum(&l))
	{
		cli->isSelected = False;
		WinCallSelect(cli,False);
	}
	ListDestroy(selectList);
	selectList = NULL_LIST;
}

/*
 * EnumSelections --- enumerate the selected client structures.  
 *	Pass NULL to begin enumeration; any non-NULL value thereafter
 *	will continue enumeration where it left off.  This function
 *	uses static data, so only one enumeration can happen at any
 *	given time.
 *	Returns NULL when list is exhausted.
 */
Client *
EnumSelections(foo)
void *foo;
{
	static List *l;
	Client *ct;

	if (foo == NULL)
		l = selectList;

	if (l != NULL)
	{
		ct = l->value;
		l = l->next;
		return ct;
	}
	return NULL;
}

/*
 * SelectionResponse
 * Respond to a selection request in the manner specified by the ICCCM.
 * If we understand the target, write the appropriate property and send the 
 * event.  If we don't understand the target, just send the event.
 */
void
SelectionResponse(request)
    XSelectionRequestEvent *request;
{
    XSelectionEvent response;
    unsigned long data[10]; /* longer than anything actually passed */
    unsigned long *wdata;
    int format, nelements, i;
    Bool accepted = True;
    Atom type;
    Client *cli;
    List *l;

    response.type = SelectionNotify;
    response.serial = request->serial;
    response.requestor = request->requestor;
    response.selection = request->selection;
    response.time = request->time;

    if ( request->property == None ) {
	/*
	 * The requestor is obsolete, because the property field should always 
	 * contain the destination property.  Use the target atom to name this 
	 * property, per the suggestion in ICCCM section 2.2.
	 */
	request->property = request->target;
    }

    /*
     * REMIND: We don't support MULTIPLE yet.
     */
    if ( request->target == AtomTargets ) {
	data[0] = AtomTargets;
	data[1] = AtomTimestamp;
	data[2] = AtomListLength;
	data[3] = XA_DRAWABLE;
	data[4] = AtomLength;
	type = XA_ATOM;
	format = 32;
	nelements = 5;
	wdata = data;
    } else if ( request->target == AtomTimestamp ) {
	data[0] = SelectionTime;
	type = XA_INTEGER;
	format = 32;
	nelements = 1;
	wdata = data;
    } else if ( request->target == AtomListLength ) {
	data[0] = ListCount(selectList);
	type = XA_INTEGER;
	format = 32;
	nelements = 1;
	wdata = data;
    } else if ( request->target == XA_DRAWABLE ) {
	nelements = ListCount(selectList);
	wdata = (unsigned long *)MemCalloc(nelements,sizeof(unsigned long));
	cli = NULL;
	while (cli = EnumSelections(cli))
	    wdata[i] = PANEWINOFCLIENT(cli);
	type = XA_DRAWABLE;
	format = 32;
    } else if ( request->target == AtomLength ) {
	data[0] = ListCount(selectList)*sizeof(long);
	type = XA_INTEGER;
	format = 32;
	nelements = 1;
	wdata = data;
    } else {
	accepted = False;
    }

    if ( accepted ) {
	XChangeProperty(request->display,request->requestor,request->property,
			type, format, PropModeReplace, wdata, nelements);
	response.target = request->target;
	response.property = request->property;
    } else {
	response.target = None;
	response.property = None;
    }

    XSendEvent(request->display, request->requestor, False,
	       NoEventMask, &response);
}

/* TimeFresh -- get a fresh timestamp from the server
 */
Time
TimeFresh(win)
WinGeneric *win;
{
	XEvent e;
	Time timestamp;

	XChangeProperty(win->core.client->dpy, win->core.self,
		AtomOlwmTimestamp, XA_INTEGER,
		32, PropModeReplace, win, 0);
	XSync(win->core.client->dpy,0);
	if (XCheckTypedWindowEvent(win->core.client->dpy, win->core.self,
	    PropertyNotify, &e))
		timestamp = e.xproperty.time;
	else
		timestamp = CurrentTime;

	return timestamp;
}
