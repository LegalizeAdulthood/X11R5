/*
			    W o r k s t a t P . h

    $Header: WorkstatP.h,v 1.0 91/10/04 17:01:06 rthomson Exp $

    Rich Thomson
    Design Systems Division
    Evans & Sutherland Computer Corporation

	Copyright (C) 1990, 1991, Evans & Sutherland Computer Corporation
*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	Copyright 1990, 1991 by Evans & Sutherland Computer Corporation,
			   Salt Lake City, Utah

			    All Rights Reserved

    Permission to use, copy, modify, and distribute this software and its 
    documentation for any purpose and without fee is hereby granted, 
    provided that the above copyright notice appear in all copies and that
    both that copyright notice and this permission notice appear in 
    supporting documentation, and that the names of Evans & Sutherland not be
    used in advertising or publicity pertaining to distribution of the
    software without specific, written prior permission.  

    EVANS & SUTHERLAND  DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
    INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
    EVENT SHALL EVANS & SUTHERLAND BE LIABLE FOR ANY SPECIAL, INDIRECT OR
    CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
    DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#ifndef _PEXtWorkstatP_h
#define _PEXtWorkstatP_h

#include <X11/CoreP.h>
#include <X11/Xmu/Converters.h>
#include "Workstati.h"

typedef struct _PEXtWorkstationClassPart {
  XtRealizeProc realize;
} PEXtWorkstationClassPart;

typedef struct _PEXtWorkstationClassRec {
    CoreClassPart core_class;
    PEXtWorkstationClassPart workstation_class;
} PEXtWorkstationClassRec;

extern PEXtWorkstationClassRec pextWorkstationClassRec;

typedef struct _PEXtWorkstationPart {
  int id;				/* workstation identifier */
  Pint type;				/* workstation type */
  Pws_st state;				/* workstation state: OPEN/CLOSED */
  int hlhsr;				/* hidden-line/surface removal mode */
  Pdefer_mode deferral;			/* workstation deferral mode */
  Pmod_mode modification;		/* workstation modification mode */
  PEXtResizePolicy resize_policy;	/* workstation resize policy */
  XtGravity resize_gravity;		/* container/porthole gravity  */
  XtCallbackList resize_callback;	/* resize callback procedures */
  PEXtTraversalPolicy traversal_policy;	/* workstation traversal policy */
  XtCallbackList traversal_callback;	/* traversal callback */
  Pctrl_flag control;			/* redraw control flag */
  XtCallbackList unrealize_callback;	/* unrealize callback */
} PEXtWorkstationPart;

typedef struct _PEXtWorkstationRec {
  CorePart core;
  PEXtWorkstationPart workstation;
} PEXtWorkstationRec;

#endif /* _PEXtWorkstatP.h_h */
/* DON'T ADD STUFF AFTER THIS #endif */
