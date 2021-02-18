/*
 * XDPSlib.h - client header file for DPS library.  This file describes the
 * Xlib interface to the Display Postscript extension.  It should be used only
 * by that interface and by code that actually uses this interface (e.g., the
 * client library interface).
 *
 * Copyright (C) 1988-1990 by Adobe Systems Incorporated and Digital Equipment
 * 			      Corporation
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notices appear in all copies and
 * that both those copyright notices and this permission notice appear
 * in supporting documentation, and that the names of Adobe Systems 
 * Incorporated and Digital Equipment Corporation not be used in 
 * advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission.  Adobe Systems Incorporated
 * and Digital Equipment Corporation make no representations about the 
 * suitability of this software for any purpose.  It is provided "as is" 
 * without express or implied warranty.
 * 
 * ADOBE SYSTEMS INCORPORATED AND DIGITAL EQUIPMENT CORPORATION DISCLAIM
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL ADOBE
 * SYSTEMS INCORPORATED AND DIGITAL EQUIPMENT CORPORATION BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN 
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT 
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * PostScript, Display PostScript, and Adobe are trademarks of Adobe Systems
 * Incorporated registered in the U.S.A. and other countries.
 *
 * Author: Adobe Systems Incorporated and Digital Equipment Corporation
 */
 
#ifndef _XDPSlib_h
#define _XDPSlib_h

#include "XDPS.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/*
 * Default standard colormap type.
 */

#define DefaultStdCMap	((XStandardColormap *) -1)


/*
 * event masks defined for convenience
 */

#define XDPSL_ALL_EVENTS \
 (PSRUNNINGMASK|PSNEEDSINPUTMASK|PSZOMBIEMASK|PSFROZENMASK)

#ifdef _NO_PROTO

typedef void (*XDPSLEventHandler)();

extern int XDPSLInit();
extern ContextXID XDPSLCreate2Context();
extern ContextXID XDPSLCreateContextAndSpace();
extern SpaceXID XDPSLCreateSpace();
extern void XDPSLGiveInput();
extern int XDPSLGetStatus();
extern void XDPSLDestroySpace();
extern void XDPSLReset();
extern void XDPSLNotifyContext();
extern ContextXID XDPSLCreateContextFromID();
extern Status XDPSLIDFromContext();
extern ContextPSID XDPSLContextFromXID();
extern void XDPSLSetStatusMask();
extern XExtCodes *XDPSLGetCodes();
extern void XDPSLSetTextEventHandler();
extern void XDPSLCallOutputEventHandler();
extern void XDPSLSetStatusEventHandler();
extern void XDPSLCallStatusEventHandler();
extern void XDPSLInitDisplayFlags();
extern Bool XDPSLGetPassEventsFlag();
extern void XDPSLSetPassEventsFlag();
extern Bool XDPSLGetWrapWaitingFlag();
extern void XDPSLSetWrapWaitingFlag();

#else /* _NO_PROTO */

typedef void (*XDPSLEventHandler)(XEvent *event);

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
 * XDPSLInit returns the first event number for the extension.
 */

extern int XDPSLInit(Display *dpy,
		     int *numberType_return,
		     char **floatingName_return);

extern ContextXID XDPSLCreateContext(Display *dpy,
				     SpaceXID sxid,
				     Drawable drawable,
				     GC gc,
				     int x, int y,
				     unsigned int eventMask,
				     XStandardColormap *grayRamp,
				     XStandardColormap *colorCube,
				     unsigned int actual,
				     ContextPSID *cpsid_return);

extern ContextXID XDPSLCreateContextAndSpace(Display *dpy,
					     Drawable drawable,
					     GC gc,
					     int x, int y,
					     unsigned int eventMask,
					     XStandardColormap *grayRamp,
					     XStandardColormap *colorCube,
					     unsigned int actual,
					     ContextPSID *cpsid_return,
					     SpaceXID *sxid_return);

extern SpaceXID XDPSLCreateSpace(Display *dpy);

extern void XDPSLGiveInput(Display *dpy, ContextXID cxid,
			   char *data, int length);

extern int XDPSLGetStatus(Display *dpy, ContextXID cxid);

extern void XDPSLDestroySpace(Display *dpy, SpaceXID sxid);

extern void XDPSLReset(Display *dpy, ContextXID cxid);

extern void XDPSLNotifyContext(Display *dpy, ContextXID cxid, int type);

extern ContextXID XDPSLCreateContextFromID(Display *dpy,
					   ContextPSID cpsid,
					   SpaceXID *sxid_return);

extern Status XDPSLIDFromContext(Display *dpy,
				 ContextPSID cpsid,
				 ContextXID *cxid_return,
				 SpaceXID *sxid_return);

/* Returns 1 on success, 0 on failure (cpsid not a valid context). */

extern ContextPSID XDPSLContextFromXID(Display *dpy, ContextXID cxid);
  
extern void XDPSLSetStatusMask(Display *dpy, ContextXID cxid,
			       unsigned long enableMask,
			       unsigned long disableMask,
			       unsigned long nextMask);
  

extern XExtCodes *XDPSLGetCodes(Display *dpy);

extern void XDPSLSetTextEventHandler(Display *dpy,
				     XDPSLEventHandler proc);

extern void XDPSLCallOutputEventHandler(Display *dpy, XEvent *event);

extern void XDPSLSetStatusEventHandler(Display *dpy,
				       XDPSLEventHandler proc);

extern void XDPSLCallStatusEventHandler(Display *dpy, XEvent *event);

extern void XDPSLInitDisplayFlags(Display *dpy);

extern Bool XDPSLGetPassEventsFlag(Display *dpy);

extern void XDPSLSetPassEventsFlag(Display *dpy, Bool flag);

extern Bool XDPSLGetWrapWaitingFlag(Display *dpy);

extern void XDPSLSetWrapWaitingFlag(Display *dpy, Bool flag);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _NO_PROTO */

typedef struct _XDPSLOutputEvent {
    int type;			/* of event. */
    unsigned long serial;	/* # of last request processed by server. */
    Bool send_event;		/* True if this came from a SendEvent req. */
    Display *display;		/* Display the event was read from. */
    ContextXID cxid;		/* Context from which data comes. */
    int length;			/* Number of characters of data. */
    char data[24];		/* Data received. */
} XDPSLOutputEvent;


typedef struct _XDPSLStatusEvent {
    int type;			/* of event. */
    unsigned long serial;	/* # of last request processed by server. */
    Bool send_event;		/* True if this came from a SendEvent req. */
    Display *display;		/* Display the event was read from. */
    ContextXID cxid;		/* Context whose status we're reporting. */
    int status;			/* Status of context. */
} XDPSLStatusEvent;
    
#endif /* _XDPSlib_h */
