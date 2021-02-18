/* -*-C-*-
********************************************************************************
*
* File:         wc_ScrolledW.c
* RCS:          $Header: wc_ScrolledW.c,v 1.3 91/03/14 03:15:11 mayer Exp $
* Description:  XM_SCROLLED_WINDOW_WIDGET_CLASS
* Author:       Niels Mayer, HPLabs
* Created:      Sat Oct 28 04:38:40 1989
* Modified:     Thu Oct  3 22:21:26 1991 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r5 contrib tape release
*
* WINTERP Copyright 1989, 1990, 1991 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
*
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: wc_ScrolledW.c,v 1.3 91/03/14 03:15:11 mayer Exp $";

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/ScrolledW.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"

extern Widget Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(); /* w_classes.c */


/******************************************************************************
 * (send <ScrolledWin_Widget> :SET_AREAS <hscroll> <vscroll> <wregion>)
 * ==> returns <ScrolledWin_Widget>....
 * This method allows you to add or change the work region controlled by
 * a scrolled window, and set the scrollbars for scrolled windows in which
 * resource :XMN_SCROLLING_POLICY is set to :APPLICATION_DEFINED.
 * <wregion> <hscroll> and  <vscroll> are all WIDGETOBJS. Use NIL
 * if you don't want to set a particular parameter.
 *
 * void XmScrolledWindowSetAreas(sw, hscroll, vscroll, wregion)
 *     XmScrolledWindowWidget sw;
 *     Widget hscroll;
 *     Widget vscroll;
 *     Widget wregion;
 ******************************************************************************/
LVAL Xm_Scrolled_Window_Widget_Class_Method_SET_AREAS()
{
  LVAL self, widgetobj;
  Widget widget_id, hscroll_id, vscroll_id, wregion_id;
  int value;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <self> */

  if (moreargs() && (*xlargv == NIL)) {	/* get <hscroll> */
    nextarg();
    hscroll_id = NULL;
  }
  else
    hscroll_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&widgetobj);

  if (moreargs() && (*xlargv == NIL)) {	/* get <vscroll> */
    nextarg();
    vscroll_id = NULL;
  }
  else
    vscroll_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&widgetobj);

  if (moreargs() && (*xlargv == NIL)) {	/* get <wregion> */
    nextarg();
    wregion_id = NULL;
  }
  else
    wregion_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&widgetobj);

  xllastarg();

  XmScrolledWindowSetAreas(widget_id, hscroll_id, vscroll_id, wregion_id);
  
  return (self);
}

/******************************************************************************
 *
 ******************************************************************************/
Wc_ScrolledW_Init()
{
  LVAL o_XM_SCROLLED_WINDOW_WIDGET_CLASS;
  extern LVAL Wcls_Create_Subclass_Of_WIDGET_CLASS(); /* w_classes.c */
  extern      xladdmsg();	/* from xlobj.c */

  o_XM_SCROLLED_WINDOW_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_SCROLLED_WINDOW_WIDGET_CLASS",
					 xmScrolledWindowWidgetClass);

  xladdmsg(o_XM_SCROLLED_WINDOW_WIDGET_CLASS, ":SET_AREAS",
	   FTAB_Xm_Scrolled_Window_Widget_Class_Method_SET_AREAS);
}
