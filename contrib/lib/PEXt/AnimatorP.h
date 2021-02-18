/*
			    A n i m a t o r P . h

    $Header: AnimatorP.h,v 1.0 91/10/04 17:00:55 rthomson Exp $

    Rich Thomson
    Design Systems Division
    Evans & Sutherland Computer Corporation

       Copyright (C) 1990, 1991 Evans & Sutherland Computer Corporation
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

#ifndef _AnimatorP_h
#define _AnimatorP_h

#include "PEXt.h"
#include "WorkstatP.h"
#include "Animator.h"

typedef struct _PEXtAnimatorClassPart {
  int dummy;
} PEXtAnimatorClassPart;

typedef struct _PEXtAnimatorClassRec {
    CoreClassPart core_class;
    PEXtWorkstationClassPart workstation_class;
    PEXtAnimatorClassPart animator_class;
} PEXtAnimatorClassRec;

extern PEXtAnimatorClassRec pextAnimatorClassRec;

typedef struct _PEXtAnimatorPart {
  float rate;				/* frame rate in seconds/frame */
  XtCallbackList update;		/* frame update callback */
  XtIntervalId timed;			/* for timed refresh */
  XtWorkProcId background;		/* for refresh ASAP */
} PEXtAnimatorPart;

typedef struct _PEXtAnimatorRec {
  CorePart core;
  PEXtWorkstationPart workstation;
  PEXtAnimatorPart animator;
} PEXtAnimatorRec;

#endif /* _AnimatorP.h_h */
/* DON'T ADD STUFF AFTER THIS #endif */
