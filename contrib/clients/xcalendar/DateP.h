
/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#ifndef _XawDateP_h
#define _XawDateP_h

#include <X11/Xos.h>		/* Needed for struct tm. */
#include "Date.h"
#include <X11/CoreP.h>

#define SEG_BUFF_SIZE		128
#define ASCII_TIME_BUFLEN	32	/* big enough for 26 plus slop */

/* New fields for the date widget instance record */
typedef struct {
	 Pixel	fgpixel;	/* color index for text */
	 XFontStruct	*font;	/* font for text */
	 GC	myGC;		/* pointer to GraphicsContext */
	 GC	EraseGC;	/* eraser GC */
/* start of graph stuff */
	 int	update;		/* update frequence */
	 int	backing_store;	/* backing store type */
	 Boolean reverse_video;
	 Boolean chime;
	 Boolean beeped;
	 int	numseg;
	 int	padding;
         int    date;
	 XPoint	segbuff[SEG_BUFF_SIZE];
	 XPoint	*segbuffptr;
	 XPoint	*hour, *sec;
	 struct tm  otm ;
	 XtIntervalId interval_id;
	 char prev_time_string[ASCII_TIME_BUFLEN];
   } DatePart;

/* Full instance record declaration */
typedef struct _DateRec {
   CorePart core;
   DatePart date;
   } DateRec;

/* New fields for the Date widget class record */
typedef struct {int dummy;} DateClassPart;

/* Full class record declaration. */
typedef struct _DateClassRec {
   CoreClassPart core_class;
   DateClassPart date_class;
   } DateClassRec;

/* Class pointer. */
extern DateClassRec dateClassRec;

#endif /* _XawDateP_h */
