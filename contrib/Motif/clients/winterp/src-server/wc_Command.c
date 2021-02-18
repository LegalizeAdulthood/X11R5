/* -*-C-*-
********************************************************************************
*
* File:         wc_Command.c
* RCS:          $Header: wc_Command.c,v 1.3 91/03/14 03:14:32 mayer Exp $
* Description:  XM_COMMAND_WIDGET_CLASS
* Author:       Niels Mayer, HPLabs
* Created:      Fri Oct 27 21:57:58 1989
* Modified:     Thu Oct  3 23:56:43 1991 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: wc_Command.c,v 1.3 91/03/14 03:14:32 mayer Exp $";

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/Command.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"
#include "w_XmString.h"

extern Widget Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(); /* w_classes.c */


/*****************************************************************************
 * (send XM_COMMAND_WIDGET_CLASS :new
 *			     [:managed/:unmanaged]
 *                           [<name>]
 *                           <parent> 
 *                           [:XMN_<arg1> <val1>]
 *                           [. . .             ]
 *                           [:XMN_<argN> <valN>])
 *
 * Create a new widget via XmCreateCommand().
 *
 * The optional keyword-argument :managed will cause a subsequent call to
 * XtManageChild(). If the submessage :unmanaged is present, or no submessage,
 * then XtManageChild() won't be called, and the resulting widget will be
 * returned unmanaged.
 ****************************************************************************/
LVAL Xm_Command_Widget_Class_Method_ISNEW()
{
  extern ArgList Wres_Get_LispArglist(); /* from w_resources.c */
  extern void    Wres_Free_C_Arglist_Data(); /* from w_resources.c */
  extern LVAL k_managed, k_unmanaged;
  LVAL self, o_parent;
  char* name;
  Boolean managed_p;
  Widget parent_widget_id, widget_id;

  self = xlgaobject();		/* NOTE: xlobj.c:clnew() returns an OBJECT; if this method
				   returns successfully, it will return a WIDGETOBJ */

  /* get optional managed/unmanaged arg */
  if (moreargs() && ((*xlargv == k_managed) || (*xlargv == k_unmanaged)))
    managed_p = (nextarg() == k_managed);
  else
    managed_p = FALSE;		/* by default don't call XtManageChild() */

  /* get optional <name> arg */
  if (moreargs() && (stringp(*xlargv)))
    name = (char*) getstring(nextarg());
  else
    name = "";			/* default name */

  /* get required <parent> widget-object arg */
  parent_widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&o_parent);

  /*
   * Store the widget object <self> in the XmNuserData resource on the
   * widget. This will allow us to retrieve the widget object from Xtoolkit
   * functions returning widget ID's without having to keep around a table
   * of widgetID-->widget-objects.
   */
   ARGLIST_RESET(); ARGLIST_ADD(XmNuserData, (XtArgVal) self); 

  if (moreargs()) {		/* if there are more arguments, */
    Cardinal xt_numargs;	/* then we have some extra widget resources to set */
    ArgList xt_arglist = Wres_Get_LispArglist(self, parent_widget_id, ARGLIST(), &xt_numargs);
    widget_id = XmCreateCommand(parent_widget_id, name, xt_arglist, xt_numargs);
    Wres_Free_C_Arglist_Data();
  }
  else 
    widget_id = XmCreateCommand(parent_widget_id, name, ARGLIST());
  
  Wcls_Initialize_WIDGETOBJ(self, widget_id);

  if (managed_p)
    XtManageChild(widget_id);

#ifdef DEBUG_WINTERP_1
  Wcls_Print_WidgetObj_Info(self);
#endif
  return (self);
}


/******************************************************************************
 * typedef struct
 * {
 *     int reason;
 *     XEvent	*event;
 *     XmString	value;
 *     int	length;
 * } XmCommandCallbackStruct;
 ******************************************************************************/
static void Lexical_Bindings_For_XmCommandCallbackStruct(bindings_list, lexical_env, cd, o_widget)
     LVAL bindings_list;	/* a list of symbols to which values from XmCommandCallbackStruct are bound */
     LVAL lexical_env;		
     XmCommandCallbackStruct* cd;
     LVAL o_widget;		/* XLTYPE_WIDGETOBJ */
{
  extern LVAL s_CALLBACK_WIDGET, s_CALLBACK_REASON, s_CALLBACK_XEVENT, s_CALLBACK_VALUE, s_CALLBACK_LENGTH; /* w_callbacks.c */
  extern LVAL Wcb_Get_Callback_Reason_Symbol();	/* w_callbacks.c */
  register LVAL s_bindname;

  for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {

    s_bindname = car(bindings_list);

    if (s_bindname == s_CALLBACK_WIDGET) {
      xlpbind(s_bindname, o_widget, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_REASON) {
      xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_XEVENT) {
      xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_VALUE) {
      xlpbind(s_bindname, (cd->value) ? cv_xmstring(XmStringCopy(cd->value)) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_LENGTH) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->length), lexical_env);
    }
    else {
      extern char temptext[];	/* from winterp.c */
      sprintf(temptext,
	      "Unknown binding name in XmCommandCallbackStruct callback evaluator. Valid symbols are [%s %s %s %s %s].",
	      (char*) getstring(getpname(s_CALLBACK_WIDGET)),
	      (char*) getstring(getpname(s_CALLBACK_REASON)),
	      (char*) getstring(getpname(s_CALLBACK_XEVENT)),
	      (char*) getstring(getpname(s_CALLBACK_VALUE)),
	      (char*) getstring(getpname(s_CALLBACK_LENGTH)));
      xlerror(temptext, s_bindname);
    }
  }
}


/******************************************************************************
 * This is called indirectly via XtAddCallback() for callbacks returning
 * an XmCommandCallbackStruct as call_data.
 ******************************************************************************/
static void XmCommandCallbackStruct_Callbackproc(widget, client_data, call_data)
     Widget    widget;
     XtPointer client_data;
     XtPointer call_data;
{
  extern void Wcb_Meta_Callbackproc(); /* w_callbacks.c */

  Wcb_Meta_Callbackproc(client_data, call_data,
			Lexical_Bindings_For_XmCommandCallbackStruct,
			NULL);
}


/******************************************************************************
 * Same as WIDGET_CLASS's :add_callback method except that this understands
 * how to get values from the XmCommandCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_VALUE
 * CALLBACK_LENGTH
 ******************************************************************************/
LVAL Xm_Command_Widget_Class_Method_ADD_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */

  return (Wcb_Meta_Method_Add_Callback(XmCommandCallbackStruct_Callbackproc, FALSE));
}


/******************************************************************************
 * Same as WIDGET_CLASS's :set_callback method except that this understands
 * how to get values from the XmCommandCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_VALUE
 * CALLBACK_LENGTH
 ******************************************************************************/
LVAL Xm_Command_Widget_Class_Method_SET_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */

  return (Wcb_Meta_Method_Add_Callback(XmCommandCallbackStruct_Callbackproc, TRUE));
}


/******************************************************************************
 * (send <commandwidget> :get_child <symbol>)
 * This method returns a WIDGETOBJ corresonding to <symbol>
 * <symbol> can be :DIALOG_COMMAND_TEXT,
 *                 :DIALOG_HISTORY_LIST, or
 *                 :DIALOG_PROMPT_LABEL
 *
 * Widget XmCommandGetChild (widget, child)
 * Widget   widget;
 * unsigned char   child;
 ******************************************************************************/
LVAL Xm_Command_Widget_Class_Method_GET_CHILD()
{
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ();
  extern LVAL s_XmDIALOG_COMMAND_TEXT, s_XmDIALOG_HISTORY_LIST, s_XmDIALOG_PROMPT_LABEL;
  LVAL self, lval_child;
  Widget widget_id;
  unsigned char child;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  lval_child = xlgasymbol();
  xllastarg();
  
  if (lval_child == s_XmDIALOG_COMMAND_TEXT)
    child = XmDIALOG_COMMAND_TEXT;
  else if (lval_child == s_XmDIALOG_HISTORY_LIST)
    child = XmDIALOG_HISTORY_LIST;
  else if (lval_child == s_XmDIALOG_PROMPT_LABEL)
    child = XmDIALOG_PROMPT_LABEL;
  else 
    xlerror("COMMAND_WIDGET_CLASS method :GET_CHILD -- unknown child type.", lval_child);

  return (Wcls_WidgetID_To_WIDGETOBJ(XmCommandGetChild(widget_id, child)));
}


/******************************************************************************
 * (send <commandwidget> :set_value <value>)
 * This sets the text in the widget's command area to the string or XmString
 * <value>. If a normal string is given, it will be converted to an XmString
 * and returned as the  method's result.
 *
 * void XmCommandSetValue (widget, value)
 * Widget   widget;
 * XmString value;
 ******************************************************************************/
LVAL Xm_Command_Widget_Class_Method_SET_VALUE()
{
  LVAL self, lval_value;
  Widget widget_id;
  XmString value;
  extern XmString Get_String_or_XmString_Arg_Returning_XmString(); /* w_XmString.c */

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  value = Get_String_or_XmString_Arg_Returning_XmString(&lval_value);
  xllastarg();
  
  /* the lame part about all this is that the XmString value that we may
     have converted above will just be cvted back to a string below. */
  XmCommandSetValue(widget_id, value);
  
  return (lval_value);
}


/******************************************************************************
 * (send <commandwidget> :append_value <value>)
 * This appends the string or XmString <value> to the string in the command
 * area widget. If a normal string is given, it will be converted to an
 * XmString and returned as the method's result.
 * 
 * void XmCommandAppendValue (widget, value)
 * Widget widget;
 * XmString value;
 ******************************************************************************/
LVAL Xm_Command_Widget_Class_Method_APPEND_VALUE()
{
  LVAL self, lval_value;
  Widget widget_id;
  XmString value;
  extern XmString Get_String_or_XmString_Arg_Returning_XmString(); /* w_XmString.c */

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  value = Get_String_or_XmString_Arg_Returning_XmString(&lval_value);
  xllastarg();
  
  /* the lame part about all this is that the XmString value that we may
     have converted above will just be cvted back to a string below */
  XmCommandAppendValue(widget_id, value);
  
  return (lval_value);
}


/******************************************************************************
 * (send <commandwidget> :error <error>)
 * This temporarily displays the string or XmString <error> in the history
 * area of the command widget, the display is cleared upon entry of the
 * next command. If a normal string is given, it will be converted to an
 * XmString and returned as the method's result.
 *
 * void XmCommandError (widget, error)
 * Widget widget;
 * XmString error;
 ******************************************************************************/
LVAL Xm_Command_Widget_Class_Method_ERROR()
{
  LVAL self, lval_value;
  Widget widget_id;
  XmString value;
  extern XmString Get_String_or_XmString_Arg_Returning_XmString(); /* w_XmString.c */

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  value = Get_String_or_XmString_Arg_Returning_XmString(&lval_value);
  xllastarg();
  
  /* the lame part about all this is that the XmString value that we may
     have converted above will just be cvted back to a string below */
  XmCommandError(widget_id, value);
  
  return (lval_value);
}


/******************************************************************************
 * (send <commandwidget> :GET_HISTORY_ITEMS)
 * 	==> returns an array of XmStrings.
 *
 * This retrieves the XmCommand widget resources XmNhistoryItems and
 * XmNhistoryItemCount from <commandwidget> and returns an array of XmStrings
 * representing the history items.
 *******************************************************************************/
LVAL Xm_Command_Widget_Class_Method_GET_HISTORY_ITEMS()
{
  LVAL self;
  Widget widget_id;
  XmStringTable xmstrtab;
  int		xmstrtab_size;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  ARGLIST_RESET();
  ARGLIST_ADD(XmNhistoryItems, &xmstrtab);
  ARGLIST_ADD(XmNhistoryItemCount, &xmstrtab_size);
  XtGetValues(widget_id, ARGLIST());

  return (Wxms_XmStringTable_To_Lisp_Vector(xmstrtab, xmstrtab_size));
}


/******************************************************************************
 *
 ******************************************************************************/
Wc_Command_Init()
{
  LVAL o_XM_COMMAND_WIDGET_CLASS;
  extern LVAL Wcls_Create_Subclass_Of_WIDGET_CLASS(); /* w_classes.c */
  extern      xladdmsg();	/* from xlobj.c */

  o_XM_COMMAND_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_COMMAND_WIDGET_CLASS",
					 xmCommandWidgetClass);

  /* a special :isnew method on this class allows for the creation of this
   * widget with Command.c:XmCreateCommand(), rather than using default
   * :ISNEW (which does XtCreateWidget()). The only special thing done by
   * XmCreateCommand() is that it prepends the following arg to the arglist:
   * XmNdialogType == XmDIALOG_COMMAND.
   */
  xladdmsg(o_XM_COMMAND_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Xm_Command_Widget_Class_Method_ISNEW);

  xladdmsg(o_XM_COMMAND_WIDGET_CLASS, ":ADD_CALLBACK",
           FTAB_Xm_Command_Widget_Class_Method_ADD_CALLBACK);

  xladdmsg(o_XM_COMMAND_WIDGET_CLASS, ":SET_CALLBACK",
           FTAB_Xm_Command_Widget_Class_Method_SET_CALLBACK);

  xladdmsg(o_XM_COMMAND_WIDGET_CLASS, ":GET_CHILD",
           FTAB_Xm_Command_Widget_Class_Method_GET_CHILD);

  xladdmsg(o_XM_COMMAND_WIDGET_CLASS, ":ERROR",
	   FTAB_Xm_Command_Widget_Class_Method_ERROR);

  xladdmsg(o_XM_COMMAND_WIDGET_CLASS, ":SET_VALUE",
	   FTAB_Xm_Command_Widget_Class_Method_SET_VALUE);

  xladdmsg(o_XM_COMMAND_WIDGET_CLASS, ":APPEND_VALUE",
	   FTAB_Xm_Command_Widget_Class_Method_APPEND_VALUE);

  xladdmsg(o_XM_COMMAND_WIDGET_CLASS, ":GET_HISTORY_ITEMS",
	   FTAB_Xm_Command_Widget_Class_Method_GET_HISTORY_ITEMS);
}
