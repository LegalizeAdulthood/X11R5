/*
 * $XConsortium: MultiText.h,v 1.16 91/01/21 12:39:04 swick Exp $ 
 */

/*
 * Copyright 1991 by OMRON Corporation
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON and MIT not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON and MIT make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * OMRON AND MIT DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL OMRON OR MIT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 *      Author: Li Yuhong	 OMRON Corporation
 */

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

/***********************************************************************
 *
 * MultiText Widget
 *
 ***********************************************************************/

/*
 * MultiText.c - Public header file for MultiText Widget.
 *
 * This Widget is intended to be used as a simple front end to the 
 * text widget with an multi source and multi sink attached to it.
 *
 * Date:    June 29, 1989
 *
 * By:      Chris D. Peterson
 *          MIT X Consortium 
 *          kit@expo.lcs.mit.edu
 */

/*
 * This file was changed from AsciiText.h.
 *
 * By Li Yuhong, Sept. 18, 1990
 */

#ifndef _MultiText_h
#define _MultiText_h

/****************************************************************
 *
 * MultiText widgets
 *
 ****************************************************************/

#include <X11/Xaw/Text.h>		/* MultiText is a subclass of Text */
#include <X11/Xaw/MultiSrc.h>

/* Resources:

 Name		     Class		RepType		  Default Value
 ----		     -----		-------		  -------------
 autoFill	     AutoFill		Boolean		  False
 background	     Background		Pixel		  XtDefaultBackground
 border		     BorderColor	Pixel		  XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	  1
 bottomMargin	     Margin		Position	  2
 cursor		     Cursor		Cursor		  xterm
 destroyCallback     Callback		Pointer		  NULL
 displayCaret	     Output		Boolean		  True
 displayPosition     TextPosition	int		  0
 editType	     EditType		XawTextEditType	  XawtextRead
 fontSet             FontSet            XFontSet          XtDefaultFontSet
 foreground	     Foreground		Pixel		  Black
 height		     Height		Dimension	  font height
 insertPosition	     TextPosition	int		  0
 leftMargin	     Margin		Position	  2
 mappedWhenManaged   MappedWhenManaged	Boolean		  True
 resize		     Resize		XawTextResizeMode XawtextResizeNever
 rightMargin	     Margin		Position	  4
 scrollHorizontal    Scroll		XawTextScrollMode XawtextScrollNever
 scrollVertical	     Scroll		XawTextScrollMode XawtextScrollNever
 selectTypes	     SelectTypes	Pointer		  pos/word/line/par/all
 selection	     Selection		Pointer		  (empty selection)
 sensitive	     Sensitive		Boolean		  True
 sink		     TextSink		Widget		  (none)
 source		     TextSource		Widget		  (none)
 string		     String		String		  NULL
 topMargin	     Margin		Position	  2
 width		     Width		Dimension	  100
 wrap		     Wrap		XawTextWrapMode	  XawtextWrapNever
 x		     Position		Position	  0
 y		     Position		Position	  0

 (see also *Src.h and *Sink.h)
*/

/*
 * Everything else we need is in StringDefs.h or Text.h
 */

typedef struct _MultiTextClassRec	*MultiTextWidgetClass;
typedef struct _MultiRec	        *MultiWidget;

extern WidgetClass multiTextWidgetClass;

#endif /* _MultiText_h */
