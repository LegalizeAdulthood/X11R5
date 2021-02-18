/*
				P E X t . h

    $Header: PEXt.h,v 1.0 91/10/04 17:00:59 rthomson Exp $

    This file contains procedure declarations and useful constants and
    macros for the PEX Toolkit.

	Copyright (C) 1990, 1991, Evans & Sutherland Computer Corporation

			   Author: Rich Thomson
			    Date: May 4th, 1990

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

#ifndef _PEXt_h
#define _PEXt_h

#include <math.h>
#include <X11/Intrinsic.h>
#include <X11/phigs/phigs.h>

#include "Primitive.h"
#include "Structure.h"

extern int PEXtError;
extern Pmatrix3 PEXtIdentityMatrix;

#define SAFE_PEX(routine) PEXtSanityCheck(__FILE__, __LINE__, routine)

#ifndef M_PI
#define PEXtPI 3.14159265358979323846
#else
#define PEXtPI (M_PI)
#endif M_PI

#define PEXtNumCounters 4

typedef enum {
  PEXtWorkstationId,
  PEXtStructureId,
  } PEXtIdentifier;

typedef enum _PEXtResizePolicy {	/* resize policy of workstation */
  PEXtResizeNone,			/* do nothing on resize */
  PEXtResizeCallback,			/* callback client on resize */
  PEXtResizePorthole,			/* window is porthole into NPC space */
  PEXtResizeContainer			/* window is container of NPC space */
  } PEXtResizePolicy;

typedef enum _PEXtTraversalPolicy {	/* traversal policy of workstation */
  PEXtTraverseNone,			/* never issue traversal */
  PEXtTraverseCallback,			/* callback client for traversal */
  PEXtTraverseRedraw			/* redraw all structures */
  } PEXtTraversalPolicy;

/* Callback reasons */
#define PEXtCR_RESIZE 0			/* resize callback invoked */
#define PEXtCR_TRAVERSE 1		/* traversal callback invoked */

extern String PEXtLowerCaseStrip(
#if NeedFunctionPrototypes
  String
#endif
  );


extern Widget PEXtInitialize(
#if NeedFunctionPrototypes
  XtAppContext*				/* context_return */,
  CONST String				/* application_class */,
  XrmOptionDescList			/* options */,
  Cardinal				/* num_options */,
  Cardinal*				/* argc_in_out */,
  String*				/* argv_in_out */,
  CONST String*				/* fallback_reosurces */,
  ArgList				/* args */,
  Cardinal				/* num_args */
#endif
  );

/*
			  Resource Names/Classes
*/
#define PEXtNworkstationId "workstationId"
#define PEXtCWorkstationId "WorkstationId"

#define PEXtNworkstationType "workstationType"
#define PEXtCWorkstationType "WorkstationType"

#define PEXtNresizePolicy "resizePolicy"
#define PEXtCResizePolicy "ResizePolicy"

#define PEXtNhlhsrMode "hlhsrMode"

#define PEXtNdeferralMode "deferralMode"

#define PEXtNmodificationMode "modificationMode"

#define PEXtNresizeCallback "resizeCallback"

#define PEXtNresizePolicy "resizePolicy"

#define PEXtNresizeGravity "resizeGravity"

#define PEXtNtraversalPolicy "traversalPolicy"

#define PEXtNtraversalCallback "traversalCallback"
#define PEXtCTraversalCallback "TraversalCallback"

#define PEXtNcontrolFlag "controlFlag"

#define PEXtNunrealizeCallback "unrealizeCallback"
#define PEXtCUnrealizeCallback "UnrealizeCallback"

#define PEXtNupdateRate "updateRate"
#define PEXtCUpdateRate "UpdateRate"

#define PEXtNupdateCallback "updateCallback"
#define PEXtCUpdateCallback "UpdateCallback"

#endif					/* _PEXt_h */
