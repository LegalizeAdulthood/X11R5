/* -*-C-*-
********************************************************************************
*
* File:         wc_MainW.c
* RCS:          $Header: wc_MainW.c,v 1.3 91/03/14 03:14:51 mayer Exp $
* Description:  XM_MAIN_WINDOW_WIDGET_CLASS
* Author:       Niels Mayer, HPLabs
* Created:      Sat Oct 28 03:35:05 1989
* Modified:     Thu Oct  3 23:56:06 1991 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: wc_MainW.c,v 1.3 91/03/14 03:14:51 mayer Exp $";

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"


extern Widget Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(); /* w_classes.c */


/******************************************************************************
 * (send <mainwinwidget> :SET_AREAS <menu> <command> <hscroll> <vscroll>
 *                                  <workregion>)
 *      ==> returns <mainwinwidget>.
 * This method is used to attach children to be managed by an instance of
 * XM_MAIN_WINDOW_WIDGET_CLASS. <menu> <command> <hscroll> <vscroll>
 * and <workregion> are all WIDGETOBJs. If you don't want to set
 * the particular region, pass NIL as the argument for the particular widget.
 * 
 * void XmMainWindowSetAreas(mw, menu, command, hscroll, vscroll, wregion)
 *     XmMainWindowWidget mw;
 *     Widget menu;
 *     Widget command;
 *     Widget hscroll;
 *     Widget vscroll;
 *     Widget wregion;
 ******************************************************************************/
LVAL Xm_Main_Window_Widget_Class_SET_AREAS()
{
  LVAL self, widgetobj;
  Widget widget_id, menu, command, hscroll, vscroll, wregion;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /*  get <self> */

  if (moreargs() && (*xlargv == NIL)) { /* get <menu> */
    menu = NULL;
    nextarg();
  }
  else
    menu = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&widgetobj);

  if (moreargs() && (*xlargv == NIL)) { /* get <command> */
    command = NULL;
    nextarg();
  }
  else
    command = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&widgetobj);

  if (moreargs() && (*xlargv == NIL)) { /* get <hscroll> */
    hscroll = NULL;
    nextarg();
  }
  else
    hscroll = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&widgetobj);

  if (moreargs() && (*xlargv == NIL)) { /* get <vscroll> */
    vscroll = NULL;
    nextarg();
  }
  else
    vscroll = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&widgetobj);

  if (moreargs() && (*xlargv == NIL)) { /* get <workregion> */
    wregion = NULL;
    nextarg();
  }
  else
    wregion = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&widgetobj);

  xllastarg();

  XmMainWindowSetAreas(widget_id, menu, command, hscroll, vscroll, wregion);

  return (self);
}


/******************************************************************************
 * (send <mainwinwidget> :SEP1)
 *       ==> returns the WIDGETOBJ corresponding to the first separator widget.
 *
 * Widget XmMainWindowSep1(w)
 *  Widget w;
 ******************************************************************************/
LVAL Xm_Main_Window_Widget_Class_SEP1()
{
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ();
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();
  
  return (Wcls_WidgetID_To_WIDGETOBJ(XmMainWindowSep1(widget_id)));
}


/******************************************************************************
 * (send <mainwinwidget> :SEP2)
 *       ==> returns the WIDGETOBJ corresponding to the second separator widget.
 *
 * Widget XmMainWindowSep2(w)
 *  Widget w;
 ******************************************************************************/
LVAL Xm_Main_Window_Widget_Class_SEP2()
{
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ();
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();
  
  return (Wcls_WidgetID_To_WIDGETOBJ(XmMainWindowSep2(widget_id)));
}


#ifdef WINTERP_MOTIF_11
/******************************************************************************
 * (send <mainwinwidget> :SEP3)
 *       ==> returns the WIDGETOBJ corresponding to the third separator widget.
 *
 * Widget XmMainWindowSep3(w)
 *  Widget w;
 ******************************************************************************/
LVAL Xm_Main_Window_Widget_Class_SEP3()
{
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ();
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();
  
  return (Wcls_WidgetID_To_WIDGETOBJ(XmMainWindowSep3(widget_id)));
}
#endif				/* WINTERP_MOTIF_11 */

/******************************************************************************
 *
 ******************************************************************************/
Wc_MainW_Init()
{
  LVAL o_XM_MAIN_WINDOW_WIDGET_CLASS;
  extern LVAL Wcls_Create_Subclass_Of_WIDGET_CLASS(); /* w_classes.c */
  extern      xladdmsg();	/* from xlobj.c */

  o_XM_MAIN_WINDOW_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_MAIN_WINDOW_WIDGET_CLASS",
					 xmMainWindowWidgetClass);

  xladdmsg(o_XM_MAIN_WINDOW_WIDGET_CLASS, ":SET_AREAS",
	   FTAB_Xm_Main_Window_Widget_Class_SET_AREAS);

  xladdmsg(o_XM_MAIN_WINDOW_WIDGET_CLASS, ":SEP1",
	   FTAB_Xm_Main_Window_Widget_Class_SEP1);

  xladdmsg(o_XM_MAIN_WINDOW_WIDGET_CLASS, ":SEP2",
	   FTAB_Xm_Main_Window_Widget_Class_SEP2);

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_MAIN_WINDOW_WIDGET_CLASS, ":SEP3",
	   FTAB_Xm_Main_Window_Widget_Class_SEP3);
#endif				/* WINTERP_MOTIF_11 */

}
