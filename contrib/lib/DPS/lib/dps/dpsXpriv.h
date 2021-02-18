/*
 * dpsXpriv.h -- client lib internal impl interface for the X version
 *
 * Copyright (C) 1989, 1991 by Adobe Systems Incorporated.
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

#ifndef DPSXPRIVATE_H
#define DPSXPRIVATE_H

#ifdef VMS
#include <decw$include/X.h>
#include <decw$include/Xlib.h>
#include <decw$include/Xutil.h>
#else /* VMS */
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif /* VMS */
#include "DPS/dpsclient.h"
#include "dpsprivate.h"
#include "DPS/dpsXclient.h"

/* Event types returned by the XDPSEvent routine. */
#define XDPSE_NonRegDPY		(-2)
#define XDPSE_NonDPS		(-1)
#define XDPSE_Output		0
#define XDPSE_Status		1

#define XDPSMaxEvents		2	/* Max no. of dps event types. */
/* typedefs */

typedef struct _t_XDPSPrivContextRec
  {
  Display		*dpy;
  Drawable		drawable;
  GC			gc;
  int			x;
  int			y;
  unsigned int		eventmask;
  XStandardColormap	*grayramp;
  XStandardColormap	*ccube;
  int			actual;
  DPSContext		ctxt;	/* Points back to its context */
  ContextXID		cxid;
  char			*newObjFormat;	/* This is the object format that a */
					/* new context must use for sending */
					/* BOS's to the client.  If the     */
					/* server and client have the same  */
					/* number formats then this will be */
					/* null.			    */
  } XDPSPrivContextRec, *XDPSPrivContext;


extern DPSProcs XDPSconvProcs;
extern DPSProcs XDPSrawProcs;
extern int	XDPSQuitBlocking;


extern XDPSPrivContext XDPSCreatePrivContextRec (/*
  Display	    *dpy,
  Drawable	    drawable,
  GC		    gc,
  int		    x,
  int		    y,
  unsigned int	    eventmask,
  XStandardColormap grayramp,
  XStandardColormap ccube,
  int		    actual */);
  
  /* See if dpy supports the DPS extension.  If not, return NULL.  If so,
     it sets up a private context object that is used for creating
     contexts and spaces. */


extern DPSNumFormat XDPSNumFormat (/* Display *dpy */);

  /* Determine the number format for server over the "dpy" connection. */

extern void XDPSSetProcs ();

  /* Set pointers to raw and conversion context procs. */

extern void XDPSSetContextEncoding ( /*
  DPSContext ctxt,
  DPSProgramEncoding progEncoding,
  DPSNameEncoding nameEncoding */);
  
  /* Sets context's program and name encodings to new values. */

extern void XDPSStatusEventHandler (/* XDPSLStatusEvent *event */);

  /* Is registered with Xlib and is called when a dps status event is
     received.  It determines what context the event belongs to and,
     if that context has a status event handler, calls its handler
     passing it the status type. */

extern void XDPSForceEvents (/* Display *dpy */);

  /* Forces processing of events that are pending over the 'dpy'
     connection.  This causes DPS events to be handled by their handlers. */

#endif /* DPSXPRIVATE_H */
