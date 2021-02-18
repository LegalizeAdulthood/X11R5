/* $XConsortium: MultiText.c,v 1.44 91/07/12 11:27:23 converse Exp $ */

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

/*
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

*/

/***********************************************************************
 *
 * MultiText Widget
 *
 ***********************************************************************/

/*
 * MultiText.c - Source code for MultiText Widget.
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
 * The MultiText was changed from AsciiText.c.
 *
 * By Li Yuhong.
 */ 
#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/XawInit.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/MultiTextP.h>
#include <X11/Xaw/MultiSrc.h>
#include <X11/Xaw/MultiSinkP.h>
#include <X11/Xaw/MultiSink.h>

#define TAB_COUNT 32

static void Initialize(), Destroy();

MultiTextClassRec multiTextClassRec = {
  { /* core fields */
    /* superclass       */      (WidgetClass) &textClassRec,
    /* class_name       */      "Text",
    /* widget_size      */      sizeof(MultiRec),
    /* class_initialize */      XawInitializeWidgetSet,
    /* class_part_init  */	NULL,
    /* class_inited     */      FALSE,
    /* initialize       */      Initialize,
    /* initialize_hook  */	NULL,
    /* realize          */      XtInheritRealize,
    /* actions          */      NULL,
    /* num_actions      */      0,
    /* resources        */      NULL,
    /* num_resource     */      0,
    /* xrm_class        */      NULLQUARK,
    /* compress_motion  */      TRUE,
    /* compress_exposure*/      XtExposeGraphicsExpose | XtExposeNoExpose,
    /* compress_enterleave*/	TRUE,
    /* visible_interest */      FALSE,
    /* destroy          */      Destroy,
    /* resize           */      XtInheritResize,
    /* expose           */      XtInheritExpose,
    /* set_values       */      NULL,
    /* set_values_hook  */	NULL,
    /* set_values_almost*/	XtInheritSetValuesAlmost,
    /* get_values_hook  */	NULL,
    /* accept_focus     */      XtInheritAcceptFocus,
    /* version          */	XtVersion,
    /* callback_private */      NULL,
    /* tm_table         */      XtInheritTranslations,
    /* query_geometry	*/	XtInheritQueryGeometry
  },
  { /* Simple fields */
    /* change_sensitive	*/	XtInheritChangeSensitive
  },
  { /* text fields */
    /* empty            */      0
  },
  { /* multi fields */
    /* empty            */      0
  }
};

WidgetClass multiTextWidgetClass = (WidgetClass)&multiTextClassRec;


static void
Initialize(request, new, args, num_args)
Widget request, new;
ArgList args;
Cardinal *num_args;
{
  MultiWidget w = (MultiWidget) new;
  int i;
  int tabs[TAB_COUNT], tab;
#if defined(IM)
  MultiSinkObject sink;
#endif	/* defined(IM) */

  /* superclass Initialize can't set the following,
   * as it didn't know the source or sink when it was called */
  if (request->core.height == DEFAULT_TEXT_HEIGHT)
    new->core.height = DEFAULT_TEXT_HEIGHT;

  w->text.source = XtCreateWidget( "textSource", multiSrcObjectClass,
				  new, args, *num_args );
  w->text.sink = XtCreateWidget( "textSink", multiSinkObjectClass,
				new, args, *num_args );

  if (w->core.height == DEFAULT_TEXT_HEIGHT)
    w->core.height = VMargins(w) + XawTextSinkMaxHeight(w->text.sink, 1);

  for (i=0, tab=0 ; i < TAB_COUNT ; i++) 
    tabs[i] = (tab += 8);
  
  XawTextSinkSetTabs(w->text.sink, TAB_COUNT, tabs);

  XawTextDisableRedisplay(new);
  XawTextEnableRedisplay(new);
#if defined(IM)
    sink = (MultiSinkObject)w->text.sink;
    XawImRegister(new);
    XawImVASetValues(new, XtNfontSet, sink->multi_sink.fontset,
		    XtNinsertPosition, w->text.insertPos,
		    XtNforeground, sink->text_sink.foreground,
		    XtNbackground, sink->text_sink.background,
		    NULL);
#endif  /* defnied(IM) */

}

static void 
Destroy(w)
Widget w;
{
    if (w == XtParent(((MultiWidget)w)->text.source))
	XtDestroyWidget( ((MultiWidget)w)->text.source );

    if (w == XtParent(((MultiWidget)w)->text.sink))
	XtDestroyWidget( ((MultiWidget)w)->text.sink );
}
