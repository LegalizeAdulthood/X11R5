/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) atom.c 50.4 90/12/12 Crucible";

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>

Atom	AtomChangeState;
Atom	AtomColorMapWindows;
Atom	AtomDecorAdd;
Atom	AtomDecorClose;
Atom	AtomDecorDelete;
Atom	AtomDecorFooter;
Atom	AtomDecorHeader;
Atom	AtomDecorPin;
Atom	AtomDecorResize;
Atom	AtomDeleteWindow;
Atom	AtomLeftFooter;
Atom	AtomLength;
Atom	AtomListLength;
Atom	AtomMenuFull;
Atom	AtomMenuLimited;
Atom	AtomMultiple;
Atom	AtomNone;
Atom	AtomOlwmTimestamp;
Atom	AtomPinIn;
Atom	AtomPinOut;
Atom	AtomProtocols;
Atom	AtomPushpinState;
Atom	AtomRightFooter;
Atom	AtomSaveYourself;
Atom	AtomShowProperties;
Atom	AtomTakeFocus;
Atom	AtomTargets;
Atom	AtomTimestamp;
Atom	AtomWMApplyProperties;
Atom	AtomWMResetProperties;
Atom	AtomWMState;
Atom	AtomWTBase;
Atom	AtomWTCmd;
Atom	AtomWTHelp;
Atom	AtomWTNotice;
Atom	AtomWTOther;
Atom	AtomWTProp;
Atom	AtomWinAttr;
Atom	AtomWindowBusy;
Atom	AtomDfltBtn;
Atom	AtomClass;
#ifdef OW_I18N
Atom	AtomCompoundText;
Atom	AtomDecorIMStatus;
Atom	AtomLeftIMStatus;
Atom	AtomRightIMStatus;
#endif

/***************************************************************************
* Global functions
***************************************************************************/

void *
GetWindowProperty(dpy, w, property, long_offset, long_length, req_type, 
    req_fmt, nitems, bytes_after)
Display *dpy;
Window w;
Atom property;
long long_offset, long_length;
Atom req_type;
int req_fmt;
unsigned long *nitems;
unsigned long *bytes_after;
{
	int status;
	void *prop;
	Atom act_type;
	int act_format;

	status = XGetWindowProperty(dpy, w, property, long_offset, long_length,
		    False, req_type, &act_type, &act_format, nitems,
		    bytes_after, &prop);
	if ((status != Success) || (act_type != req_type))
	{
	    *nitems = 0;
	    return NULL;
	}
	if ((req_fmt != 0) && (act_format != req_fmt))
	{
	    XFree(prop);
	    *nitems = 0;
	    return NULL;
	}
	return prop;
}
