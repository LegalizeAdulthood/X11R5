/* $Id: TalkSrcP.h,v 1.0 1991/10/03 18:18:11 lindholm Exp $ */
/* TalkSrcP.h - Private header file for Talk Ascii Text Source.
   
   This is the private header file for the Talk Ascii Text Source.
   This gadget is intended to be used with the Text widget.
   It is a subclass of the Ascii Text Source gadget. It differs from
   that in it has a slightly different restriction on editting from
   append mode, and a callback for text modification.   
   
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


#ifndef _TalkSrcP_h
#define _TalkSrcP_h

#include "TalkSrc.h"
#include <X11/Xaw/AsciiSrcP.h>

/* Private declarations - none. */

/* New fields for the TalkSrc object class record - no real ones.*/
typedef struct _TalkSrcClassPart { char foo; } TalkSrcClassPart;

/* Full class record type */
typedef struct _TalkSrcClassRec {
    ObjectClassPart	object_class;
    TextSrcClassPart	text_src_class;
    AsciiSrcClassPart	ascii_src_class;
    TalkSrcClassPart	talk_src_class;
} TalkSrcClassRec;

extern TalkSrcClassRec talkSrcClassRec;

/* New fields for the Talk Src object record. */

typedef struct _TalkSrcPart {
    /* Resources. */
    XtCallbackList replace_callback;
} TalkSrcPart;

/* Full instance record type. */
typedef struct _TalkSrcRec {
    ObjectPart	object;
    TextSrcPart	text_src;
    AsciiSrcPart	ascii_src;
    TalkSrcPart		talk_src;
} TalkSrcRec;

#endif /* ndef _TalkSrcP_h */   
