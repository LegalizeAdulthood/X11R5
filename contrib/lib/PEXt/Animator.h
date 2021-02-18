/*

			     A n i m a t o r . h

    $Header: Animator.h,v 1.0 91/10/04 17:00:54 rthomson Exp $

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

#ifndef _PEXt_Animate_h
#define _PEXt_Animate_h

#include "PEXt.h"

/*
  PEX Animator Widget:		Core
  				 |
			  PEXtWorkstation
			         |
			   PEXtAnimator

  New Resources:

  Name			Representation	Class		Default

  updateRate		Float		UpdateRate	24 frames/second
  updateCallback	CallbackList	UpdateCallback	NULL

*/

/* Class record constants */

extern WidgetClass pextAnimatorWidgetClass;

typedef struct _PEXtAnimatorClassRec *PEXtAnimatorWidgetClass;
typedef struct _PEXtAnimatorRec      *PEXtAnimatorWidget;

typedef struct _PEXtAnimatorCallbackStruct {
  int reason;
  int width, height;
  XEvent *event;
} PEXtAnimatorCallbackStruct;
    
extern Widget PEXtCreateAnimator();

extern float PEXtFPSToUpdateRate();	/* frames/second => rate */
extern float PEXtSPFToUpdateRate();	/* seconds/frame => rate */

#endif /* _PEXt_Animator_h */
/* DON'T ADD STUFF AFTER THIS #endif */

