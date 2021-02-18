#ifndef lint
     static char *rcsid = "$Header: /usr3/Src/emu/client/RCS/aux_widgets.c,v 1.6 91/10/01 01:12:25 jkh Exp $";
#endif

/*
 * This file is part of the PCS emu system.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 *
 * Author: Jordan K. Hubbard
 * Date: August 31th, 1990.
 * Description: This handles all the scrolling schrott for the emu
 *		client.
 *
 * Revision History:
 *
 * $Log:	aux_widgets.c,v $
 * Revision 1.6  91/10/01  01:12:25  jkh
 * Removed useless local vars.
 * 
 * Revision 1.1  90/09/10  16:22:21  jkh
 * Initial revision
 *
 */

#include "client.h"
#include <ctype.h>

/*
 * Convenience functions.
 */
Export Inline Dimension
HeightOf(w)
Widget w;
{
     Dimension height;
     Arg args[2];
     int i;

     i = 0;
     XtSetArg(args[i], XtNheight, &height);	i++;
     XtGetValues(w, args, i);
     return height;
}

Export Inline Dimension
WidthOf(w)
Widget w;
{
     Dimension width;
     Arg args[2];
     int i;

     i = 0;
     XtSetArg(args[i], XtNwidth, &width);	i++;
     XtGetValues(w, args, i);
     return width;
}

Export Inline Dimension
BorderOf(w)
Widget w;
{
     Dimension bw;
     Arg args[2];
     int i;

     i = 0;
     XtSetArg(args[i], XtNborderWidth, &bw);	i++;
     XtGetValues(w, args, i);
     return bw;
}

#ifdef MOTIF	/* Motif scrollbar routines follow */

Export void
ScrollProc(w, client_data, call_data)
register Widget w;
XtPointer client_data;
register XmScrollBarCallbackStruct *call_data;
{
     float percent;
     int code;
     int pos = call_data->value;

     if ((int)client_data != 1 && (int)client_data != 2)
	  fatal("Bad client_data passed to ScrollProc.");
     else {
	  if ((int)client_data == 1) {		/* scrollProc */
	       percent = ((float)((int)pos)) / ((float)HeightOf(w));
	       XpTermSetRegister(Term, 'b', CB_INT_TYPE, TRUE);
	       code = OP_SCROLL_SCREEN_RELATIVE;
	  }
	  else {				/* jumpProc */
	       percent = ((float)pos) / 100.0;
	       XpTermSetRegister(Term, 'b', CB_INT_TYPE, FALSE);
	       code = OP_SCROLL_SCREEN_ABSOLUTE;
	  }
	  XpTermSetRegister(Term, 'a', CB_INT_TYPE, (int)(percent * 1000.0));
	  XpTermDispatchRequest(Term, code);
     }
}

Export void
ScrollbarAdjust(w, pos, size)
Widget w;
float pos, size;
{
     if (SBar)
	  XmScrollBarSetValues(SBar, (int)(pos * 100),
			       (int)(size * 100), 0, 0, 0);
}

Export Widget
ScrollbarCreate(parent, sizep)
Widget parent;
Dimension *sizep;
{
     Arg args[10];
     int i;
     static XtCallbackRec cb[2] = {
	  { (XtCallbackProc)ScrollProc,	(XtPointer)1 },
	  { (XtCallbackProc)NULL,	(XtPointer)NULL },
     },
     cb2[2] = {
	  { (XtCallbackProc)ScrollProc,	(XtPointer)2 },
	  { (XtCallbackProc)NULL,	(XtPointer)NULL },
     };
     Widget scb;

     i = 0;
/*
     XtSetArg(args[i], XmNdragCallback, cb);		i++;
*/
     XtSetArg(args[i], XmNvalueChangedCallback, cb2);	i++;
     XtSetArg(args[i], XmNorientation, XmVERTICAL);	i++;
     XtSetArg(args[i], XmNminimum, 0);			i++;
     XtSetArg(args[i], XmNmaximum, 100);		i++;
     XtSetArg(args[i], XmNsliderSize, 100);		i++;
/*
     XtSetArg(args[i], XmNincrement, 1);		i++;
     XtSetArg(args[i], XmNpageIncrement, 10);		i++;
*/
     XtSetArg(args[i], XmNborderWidth, 1);		i++;
     scb = XtCreateManagedWidget("vScrollBar", xmScrollBarWidgetClass,
				 parent, args, i);
     if (sizep)
          *sizep = WidthOf(scb) + BorderOf(scb);
     return scb;
}

#else	/* ATHENA scrollbar routines */

Export void
ScrollProc(w, client_data, pos)
register Widget w;
XtPointer client_data;
register XtPointer pos;
{
     float percent;
     int i, code;
     Arg args[5];

     if ((int)client_data != 1 && (int)client_data != 2)
	  fatal("Bad client_data passed to ScrollProc.");
     else {
	  if ((int)client_data == 1) {		/* scrollProc */
	       percent = ((float)((int)pos)) / ((float)HeightOf(w));
	       XpTermSetRegister(Term, 'b', CB_INT_TYPE, TRUE);
	       code = OP_SCROLL_SCREEN_RELATIVE;
	  }
	  else {				/* jumpProc */
	       percent = *((float *)pos);
	       XpTermSetRegister(Term, 'b', CB_INT_TYPE, FALSE);
	       code = OP_SCROLL_SCREEN_ABSOLUTE;
	  }
	  XpTermSetRegister(Term, 'a', CB_INT_TYPE, (int)(percent * 1000.0));
	  XpTermDispatchRequest(Term, code);
     }
}

Export void
ScrollbarAdjust(w, pos, size)
Widget w;
float pos, size;
{
     if (SBar)
	  XawScrollbarSetThumb(SBar, pos, size);
}

Export Widget
ScrollbarCreate(parent, sizep)
Widget parent;
Dimension *sizep;
{
     Arg args[5];
     int i;
     static XtCallbackRec cb[2] = {
	  { (XtCallbackProc)ScrollProc,	(XtPointer)1 },
	  { (XtCallbackProc)NULL,	(XtPointer)NULL },
     },
     cb2[2] = {
	  { (XtCallbackProc)ScrollProc,	(XtPointer)2 },
	  { (XtCallbackProc)NULL,	(XtPointer)NULL },
     };
     Widget scb;

     i = 0;
     XtSetArg(args[i], XtNscrollProc, cb);		i++;
     XtSetArg(args[i], XtNjumpProc, cb2);		i++;
     XtSetArg(args[i], XtNborderWidth, 1);		i++;
     scb = XtCreateManagedWidget("vScrollBar", scrollbarWidgetClass,
				 parent, args, i);
     XawScrollbarSetThumb(scb, 0.0, 1.0);
     if (sizep)
          *sizep = WidthOf(scb) + 1;
     return scb;
}

#endif /* MOTIF || ATHENA */

/*
 * Handle the positioning of children in the term widget. The term
 * widget just acts like a container and passes control to us whenever
 * something needs to be done regarding the layout of its children.
 *
 * "self" is always the term widget ID. "w" is the child causing
 * the layout request or NULL if the layout request is an externally
 * requested resize of the term widget. If w is the term widget itself,
 * (w == self) then this is the initial Realize request.
 * 
 * The "width" and "height" members are the width and height request
 * members, but TAKE HEED. These "height" and "width" values aren't always
 * relative to the same widget! If "w" is NULL or equal to "self", then the
 * width and height are for the term widget. If w is something else (hopefully
 * the canvas), then they're relative to that widget.
 *
 * IMPORTANT NOTE: Though this routine is in the client, it's LOGICALLY
 * part of the widget since it is doing the widget's internal geometry
 * management. For that reason, we MUST use things like XtResizeWidget()
 * to change height and width (even though the Xt Intrinsics guide says
 * clients should _not_ do this) since using XtSetValues() will put us into
 * an infinite loop! This layout cruft should be considered an "extension"
 * to the term widget, not a part of the true client code.
 *
 */

Export XtGeometryResult
do_layout(self, child, width, height)
TermWidget self;
Widget child;
Dimension width, height;
{
     Dimension m_h = 0, m_w = width, s_h = height, s_w = 0;
     Dimension yCanvas = 0, xCanvas = 0;
     Dimension wCanvas = width, hCanvas = height;
     Dimension m_bdr = 0, s_bdr = 0;
     Dimension m_all, s_all;

     if (MBar) {
	  m_h = HeightOf(MBar);
	  m_bdr = BorderOf(MBar);
     }

     if (SBar) {
	  s_w = WidthOf(SBar);
	  s_bdr = BorderOf(SBar);
     }

     /* Precalculate for convenience */
     m_all = m_h + m_bdr;
     s_all = s_w + s_bdr;

     if (child) {
	  int i;
	  Arg args[10];
	  Dimension o_width = width, o_height = height;

	  /*
	   * The WM will have generally already added the base_width and
	   * base_height in, in which case we don't need to compensate for
	   * the scrollbar width and height.
	   */
	  if ((WidthOf(Canvas) == o_width && HeightOf(Canvas) == o_height) ||
	      (Widget)self != child) {
	       o_width += (SBar ? s_all : 0);
	       o_height += (MBar ? m_all : 0);
	  }

	  if (child == (Widget)self) {	/* We're being realized */
	       /* Position everyone properly the first time around */
	       if (MBar) {
		    /* Move menu bar to hide top and left border */
		    XtMoveWidget(MBar, -1, -1);
		    yCanvas = m_all;
		    hCanvas -= m_all;
	       }
	       /* Move scrollbar down and over */
	       if (SBar)  {
		    XtMoveWidget(SBar, -1, yCanvas - 1);
		    xCanvas = s_all;
		    wCanvas -= s_all;
	       }
	       XtMoveWidget(Canvas, xCanvas, yCanvas);
	  }
	  /*
	   * Size is canvas size, not term size, thus we need to make
	   * the menu bar wider by the width of the scrollbar, if present.
	   * Since the scrollbar is always as high as the canvas, its
	   * height is fine.
	   */
	  else if (SBar)
	       m_w += s_all;
	  /* Change our own size */
	  i = 0;
	  XtSetArg(args[i], XtNwidth, o_width);		i++;
	  XtSetArg(args[i], XtNheight, o_height);	i++;
	  XtSetValues((Widget)self, args, i);
     }
     else {
	  /*
	   * Adjust the height and width properly for the canvas
	   * size this was a resize request, not a geometry request.
	   * Also, since this is now the term size, we need to adjust the
	   * scrollbar height so that it's shorter if the menu bar is present.
	   */
	  if (MBar) {
	       hCanvas -= m_all;
	       s_h -= m_h;
	  }
	  wCanvas -= (SBar ? s_all : 0);
     }
     /* Give the canvas the size it wants, modulo frobs */
     XtResizeWidget(Canvas, wCanvas, hCanvas, 0);

     if (MBar)
	  XtResizeWidget(MBar, m_w, m_h, 1);

     if (SBar)
	  XtResizeWidget(SBar, s_w, s_h, 1);

     /*
      * This is naive, yes, but we're not smart enough to handle the
      * failure case anyway, so why sweat it?
      */
     return XtGeometryYes;
}
     
/*
 * Dispatch a request for menu item 'w'. Right now, we don't use 'w'
 * for anything, but might in the future if we have to deal with multiple
 * term widgets with multiple menus, for example.
 */
Export void
DoMenuDispatch(w, str)
Widget w;
String str;
{
     Import int atoi();

     if (!str)
	  fatal("MenuDispatch: NULL string passed");

     while (isspace(*str))
	  ++str;
     if (isdigit(*str))
	  XpTermDoRop(Term, atoi(str));
     else
	  XpTermDoEmulation(Term, str);
}
