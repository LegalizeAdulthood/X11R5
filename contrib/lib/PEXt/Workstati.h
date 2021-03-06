/*
			   W o r k s t a t i . h

    $Header: Workstati.h,v 1.0 91/10/04 17:01:07 rthomson Exp $

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

#ifndef _PEXtWorkstation_h
#define _PEXtWorkstation_h

#include "PEXt.h"

/*
  PEX Workstation Widget

  Resources (basically follows the Workstation State Table):

  Name: RepType	(Class)
	Default

  workstationId: Integer (WorkstationId)
	<Next available workstation Id>
  workstationType: WorkstationType (WorkstationType)
	PEXtDefaultWorkstationType
  hlhsrMode: HLHSRMode (HLHSRMode)
	PEXtDefaultHLHSRMode
  deferralMode: DeferralMode (DeferralMode)
	PEXtDefaultDeferralMode
  modificationMode: ModificationMode (ModificationMode)
	PEXtDefaultModificationMode
  resizePolicy: ResizePolicy (ResizePolicy)
	PEXtDefaultResizePolicy
  resizeGravity: Gravity (Gravity)
	Center
  resizeCallback: CallbackList (ResizeCallback)
	NULL
  traversalPolicy: TraversalPolicy (TraversalPolicy)
	PEXtDefaultTraversalPolicy
  traversalCallback: CallbackList (TraversalCallback)
	NULL
  controlFlag: ControlFlag (ControlFlag)
	PEXtDefaultControlFlag
  unrealizeCallback: CallbackList (UnrealizeCallback)
	{ unrealize, NULL }
*/

/* Class record constants */

extern WidgetClass pextWorkstationWidgetClass;

typedef struct _PEXtWorkstationClassRec *PEXtWorkstationWidgetClass;
typedef struct _PEXtWorkstationRec      *PEXtWorkstationWidget;

typedef struct _PEXtWorkstationCallbackStruct {
  int reason;
  int width, height;
  XEvent *event;
} PEXtWorkstationCallbackStruct;
    
extern Widget PEXtCreateWorkstation();

extern Pint PEXtWorkstation();

#endif /* _PEXtWorkstation_h */
/* DON'T ADD STUFF AFTER THIS #endif */
