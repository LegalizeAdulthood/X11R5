/* $Id: TalkSrc.h,v 1.0 1991/10/03 18:18:11 lindholm Exp $ */
/* TalkSrc.h - Public header file for Talk Ascii Text Source.
   
   This is the public header file for the Talk Ascii Text Source.
   This gadget is intended to be used with the Text widget.
   It is a subclass of the Ascii Text Source gadget. It differs by
   having a slightly different restriction on editting from
   append mode, and it has a callback list for text modification.
   
   */

/*
  
  Copyright 1991 by the University of British Columbia.
  
  All rights reserved.
  
  Permission to use, copy, modify, and distribute this software and its
  documentation for any purpose and without fee is hereby granted,
  provided that the above copyright notice appear in all copies and that
  both that copyright notice and this permission notice appear in
  supporting documentation, and that the name of the University of
  British Columbia (U.B.C.) not be used in advertising or publicity
  pertaining to distribution of the software without specific, written
  prior permission.
  
  U.B.C. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
  U.B.C. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
  ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
  SOFTWARE.
  
  */


#ifndef _TalkSrc_h
#define _TalkSrc_h

#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiSrc.h>

/* Resources:
   
   replaceCallback Callback NULL
   
   */
#define XtNreplaceCallback "replaceCallback"

/* Class record constants */

extern WidgetClass talkSrcObjectClass;

typedef struct _TalkSrcClassRec *TalkSrcObjectClass;
typedef struct _TalkSrcRec      *TalkSrcObject;

/* Resource Definitions. We only have the generically-named "callback". */

/* A pointer to this structure is passed to the callback routines. */
typedef struct {
    int reason;
    XEvent *event;
    Boolean doit;		/* whether the replace should be done */
    int rc;			/* if not doit, return this code */
    XawTextPosition start, end;	/* start and end of chars to be removed */
    XawTextBlock *text;		/* text to be added to replace those removed */
} TalkSrcCallbackStruct;

#endif /* ndef _TalkSrc_h */   
