/*
 * dpsXclient.h -- XDPS extension interface to the Display Postscript Library.
 *
 * Copyright (C) 1989-1991 by Adobe Systems Incorporated.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notices appear in all copies and that
 * both those copyright notices and this permission notice appear in
 * supporting documentation and that the name of Adobe Systems
 * Incorporated not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  If any portion of this software is changed, it cannot be
 * marketed under Adobe's trademarks and/or copyrights unless Adobe, in
 * its sole discretion, approves by a prior writing the quality of the
 * resulting implementation.
 * 
 * ADOBE MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THE SOFTWARE FOR
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
 * ADOBE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO EVENT SHALL ADOBE BE LIABLE
 * TO YOU OR ANY OTHER PARTY FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE, STRICT LIABILITY OR ANY OTHER ACTION ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  ADOBE WILL NOT
 * PROVIDE ANY TRAINING OR OTHER SUPPORT FOR THE SOFTWARE.
 * 
 * PostScript, Display PostScript, and Adobe are trademarks of Adobe Systems
 * Incorporated registered in the U.S.A. and other countries.
 *
 * Author: Adobe Systems Incorporated
 */

#ifndef DPSXCLIENT_H
#define DPSXCLIENT_H

#ifdef VMS
#include <decw$include/X.h>
#include <decw$include/Xlib.h>
#include <decw$include/Xutil.h>
#else /* VMS */
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif /* VMS */
#include <DPS/XDPS.h>
#include <DPS/dpsclient.h>

/* Codes 2000 thru 2099 reserved for XDPS extension error codes */

/* dps_err_invalidAccess was promoted to a general error code; maintain its
   old value for compatibility */

#undef dps_err_invalidAccess
#define dps_err_invalidAccess	2000
#define dps_err_encodingCheck	2001
#define dps_err_closedDisplay	2002
#define dps_err_deadContext	2003
#define dps_err_warning		2004
#define dps_err_fatal		2005
#define dps_err_recursiveWait	2006

  /* The above definitions specify additional error conditions:
  
      dps_err_invalidAccess: signaled when expecting output from a context
		created by another client.  Such contexts send their
		output only to the original creator and attempting to
		receive output from them would cause indefinite blocking.
		
      dps_err_encodingCheck: signaled when changing name encoding for a
		context created by another client or a context created
		in a space which was created by another client.  Such
		contexts can only accept string name encoding.  This
		error is also generated when an attempt is made to change
		name or program encoding to other than acceptable values.

      dps_err_closedDisplay: when sending PostScript code to a context
		that has had its 'display' closed.

      dps_err_deadContext: signaled when expecting output from a context
		which has died in the server but still has its X resources
		active (a zombie context).  Attempting to receive output
		from such a context would cause indefinite blocking.

      dps_err_warning: a recoverable error has been found by the Client
      		Library.  The error proc should display the message
      		(text string passed through arg1) and then simply
      		return.

      dps_err_fatal: a non-recoverable error has been found by the Client
      		Library.  The error proc should display the message
      		(text string passed through arg1), and then call
      		abort() or should otherwise exit the application.
      		The error proc should not return.

      dps_err_recursiveWait: a client has called a wrap that returns
		a value from a status or output handler, and this handler
		was invoked while waiting for the results of another
		value-returning wrap.  Such a recursive wait would
		cause indefinite blocking.  This error will usually
		cause both wraps to return incorrect values and will
		often trigger further errors.

  */
  
extern long int DPSLastUserObjectIndex;

typedef enum {dps_event_pass_through, dps_event_internal_dispatch,
	      dps_event_query} DPSEventDelivery;

/* Event delivery types for XDPSSetEventDelivery */

#ifdef _NO_PROTO

extern DPSContext XDPSCreateContext();
extern DPSContext XDPSCreateSimpleContext();
extern DPSContext DPSCreateTextContext();
extern DPSContext DPSContextFromContextID();
extern DPSContext XDPSFindContext ();
extern void DPSDefaultTextBackstop ();
extern void DPSChangeEncoding ();
typedef void (*XDPSStatusProc)();
extern XDPSStatusProc XDPSRegisterStatusProc();
extern int XDPSGetContextStatus();
extern long int DPSNewUserObjectIndex();
extern DPSContext XDPSContextFromSharedID();
extern DPSSpace XDPSSpaceFromSharedID();
extern XID XDPSXIDFromContext();
extern XID XDPSXIDFromSpace();
extern DPSContext XDPSContextFromXID();
extern DPSSpace XDPSSpaceFromXID();
extern void XDPSUnfreezeContext();
extern void XDPSSetStatusMask();
extern void XDPSGetDefaultColorMaps();
extern DPSEventDelivery XDPSSetEventDelivery();
extern Bool XDPSIsStatusEvent();
extern Bool XDPSIsOutputEvent();
extern Bool XDPSIsDPSEvent();
extern Bool XDPSDispatchEvent();

#else /* _NO_PROTO */

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern DPSContext XDPSCreateContext(Display *dpy,
				    Drawable drawable,
				    GC gc,
				    int x,
				    int y,
				    unsigned int eventmask,
				    XStandardColormap *grayramp,
				    XStandardColormap *ccube,
				    int actual,
				    DPSTextProc textProc,
				    DPSErrorProc errorProc,
				    DPSSpace space);
  
  /* Checks to see if 'dpy' supports the DPS extension.  If not, NULL is
     returned.  If so, a context associated with 'dpy', 'drawable' and
     'gc' is created.  'x' and 'y' are offsets from the drawable's origin
     to the PostScript device's origin in X coordinates.  'eventmask' is
     currently ignored.  'grayramp', 'ccube', and 'actual' collectively
     define the colormap that will be associated with the new context.
     'textProc' is called to dispose of text sent by the context.
     'errorProc' is called to handle errors reported by the context.
     'space' determines the execution space of the new context.  A null
     space causes a new one to be created.  */

extern DPSContext XDPSCreateSimpleContext(Display *dpy,
					  Drawable drawable,
					  GC gc,
					  int x,
					  int y,
					  DPSTextProc textProc,
					  DPSErrorProc errorProc,
					  DPSSpace space);

  /* Does the same thing as XDPSCreateContext (above) but uses the
     standard colormap information for the new context. */

extern DPSContext DPSCreateTextContext(DPSTextProc textProc,
				       DPSErrorProc errorProc);

  /* Create a new DPSContext whose PostScript is converted to text and sent
     to 'textProc' (rather than the interpreter).  'errorProc' can
     be used by the implementation of 'textProc' to report any i/o
     errors. */


extern DPSContext DPSContextFromContextID(DPSContext ctxt,
					  ContextPSID cid,
					  DPSTextProc textProc,
					  DPSErrorProc errorProc);

  /* Returns a DPSContext that is bound to the PostScript context
     created when the "fork" operator was executed by 'ctxt', resulting
     in the integer value 'cid'.

     If a DPSContext has already been created for cid, that DPSContext
     is returned.  Otherwise, a new one is created, as follows:

     If 'textProc' or 'errorProc' are NULL, the new DPSContext will
     inherit the respective procs from 'ctxt', otherwise the the new
     context gets the 'textProc' and/or 'errorProc' passed in. The new
     DPSContext inherits everything else from 'ctxt', except its
     chaining properties, which are set to NULL.

     DPSContextFromContextID will return NULL if it is unable to create
     the context. */

extern DPSContext XDPSFindContext(Display *dpy, ContextPSID cid);

  /* Find a known context based on its PostScript id, return its DPSContext. */


extern void DPSDefaultTextBackstop(DPSContext ctxt, char *buf,
				   long unsigned int count);

  /* This is the text backstop proc automatically installed by
     the client library.  Since it is a DPSTextProc, you may
     use it as your context textProc.  Writes text to stdout
     and flushes stdout. */

extern void DPSChangeEncoding(DPSContext ctxt,
			      DPSProgramEncoding newProgEncoding,
			      DPSNameEncoding newNameEncoding);

  /* Change one or more of the context's 2 encoding parameters.
     If the formats and encodings for the system on which the client
     library is built are different than the formats and encodings
     specified by 'ctxt', DPSWritePostScript (as well as the
     operations BinObjSeqWrite and WriteTypedObjectArray defined
     above, but not WriteData) will convert the bytes of
     code from the former to the latter before sending
     them on to 'ctxt'. */

typedef void (*XDPSStatusProc)(DPSContext ctxt, int code);

extern XDPSStatusProc XDPSRegisterStatusProc(DPSContext ctxt,
					     XDPSStatusProc proc);

  /* Register a procedure to be called when a XDPSLStatusEvent
     is received by the client.  The procedure will be called with two
     parameters: the context it was registered with, and the
     status code derived from the event.  The proc may be
     called at any time that the client processes events.  Overwrites
     any previously registered routine and returns it.  See XDPS.h */

extern int XDPSGetContextStatus(DPSContext ctxt);

  /* Returns the status of 'ctxt' as defined in XDPS.h. */

extern long int DPSNewUserObjectIndex();

  /* Returns a new user object index.  The Client Library is
     the sole allocator of new user object indices.  User object
     indices are dynamic, and should not be used for computation
     or stored in long term storage, such as a file. */

/* ============================= */
/* The following items are provided for lower level access to the DPS
    X library extension.
*/

extern DPSContext XDPSContextFromSharedID(Display *dpy,
					  ContextPSID cid,
					  DPSTextProc textProc,
					  DPSErrorProc errorProc);

  /* Create a context record for PostScript "cid" shared by another client. */

extern DPSSpace XDPSSpaceFromSharedID(Display *dpy, SpaceXID sxid);
  
  /* Create a space record for a "sxid" shared by another client. */

extern XID XDPSXIDFromContext(Display **Pdpy, DPSContext ctxt);

  /* Get the XID for the given context record.  Return its 'dpy' */

extern XID XDPSXIDFromSpace(Display **Pdpy, DPSSpace spc);

  /* Get the XID for the given space record.  Retunr its 'dpy' */

extern DPSContext XDPSContextFromXID (Display *dpy, XID xid);

  /* Get the context record for the given XID on 'dpy'*/

extern DPSSpace XDPSSpaceFromXID (Display *dpy, XID xid);

  /* Get the space record for the given XID on 'dpy'*/

extern void XDPSUnfreezeContext(DPSContext ctxt);

  /* Notify a context that is in "frozen" mode to continue
     with execution.  */

extern void XDPSSetStatusMask(DPSContext ctxt,
			      unsigned long enableMask,
			      unsigned long disableMask,
			      unsigned long nextMask);

  /* Set the status reporting mask for the client.  The enableMask
     specifies the events the client wants to receive, the
     disableMask specifies the events the client no longer wants
     to receive, and the nextMask specifies the events that
     the client wishes to receive only the next instance of. 
     The mask values are specified in XDPS.h */

extern void XDPSGetDefaultColorMaps(Display *dpy,
				    Screen *screen,
				    Drawable drawable,
				    XStandardColormap *colorcube,
				    XStandardColormap *grayramp);

  /* Get the default color maps for the screen, such as they are.  If screen
     is NULL, use the drawable to find out which screen.  Either colorcube or
     grayramp may be NULL to indicate that you don't want that colormap.
     Both may be NULL, but there's not much point in calling the routine
     then, is there? */

/* ============================= */
/* New event dispatching procedures */

extern DPSEventDelivery XDPSSetEventDelivery(Display *dpy,
					     DPSEventDelivery newMode);

  /* Set or query the event delivery mode */

extern Bool XDPSIsStatusEvent(XEvent *event, DPSContext *ctxt, int *status);

  /* Check if an event is a DPS status event; return the context and
     status if so. */

extern Bool XDPSIsOutputEvent(XEvent *event);

  /* Check if an event is a DPS output event. */

extern Bool XDPSIsDPSEvent(XEvent *event);

  /* Check if an event is a DPS event. */

extern Bool XDPSDispatchEvent(XEvent *event);

  /* Check if an event is a DPS event; call the status or output
     handler if so. */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _NO_PROTO */

#endif /* DPSXCLIENT_H */
