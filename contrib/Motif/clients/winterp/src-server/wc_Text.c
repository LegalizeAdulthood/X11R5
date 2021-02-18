/* -*-C-*-
********************************************************************************
*
* File:         wc_Text.c
* RCS:          $Header: wc_Text.c,v 1.6 91/03/14 03:15:19 mayer Exp $
* Description:  XM_TEXT_WIDGET_CLASS
* Author:       Niels Mayer, HPLabs
* Created:      Sat Oct 28 04:50:44 1989
* Modified:     Thu Oct  3 23:41:25 1991 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: wc_Text.c,v 1.6 91/03/14 03:15:19 mayer Exp $";

/*
 * <limits.h> defines machine dependent limits on sizes of numbers, if your
 * machine doesn't have this, then your compiler doesn't conform to standards
 * XPG2, XPG3, POSIX.1, FIPS 151-1 and you should complain to the manufacturer.
 * 
 * If for some reason your system isn't standards-conforming, you may work
 * around this problem by using the following definitions (assuming 32 bit machine):
 * 
 * #define INT_MAX 2147483647
 * #define INT_MIN (-2147483647 - 1)
 */
#include <limits.h>
#include <stdio.h>
#include <Xm/Xm.h>

#include "winterp.h"		/* this must occur before doing "#ifdef WINTERP_MOTIF_11" */

#include <Xm/Text.h>
#ifdef WINTERP_MOTIF_11
#include <Xm/TextF.h>
#endif				/* WINTERP_MOTIF_11 */

#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"


extern Widget Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(); /* w_classes.c */


/******************************************************************************/
static XmTextPosition Get_XmTextPosition_Argument()
{
  LVAL lval_position;
  long position;

  lval_position = xlgafixnum();
  position = (long) getfixnum(lval_position);
  if (position < 0L)
    xlerror("XmTextPosition must be a positive FIXNUM.", lval_position);
  return ((XmTextPosition) position);
}

/******************************************************************************/
static int Get_Int_Argument(min, max)
     long min;
     long max;
{
  LVAL lval_num;
  long num;

  lval_num = xlgafixnum();
  num = (long) getfixnum(lval_num);
  if (num < min)
    xlerror("FIXNUM value too small.", lval_num);
  if (num > max)
    xlerror("FIXNUM value too large.", lval_num);
  return ((int) num);
}

/******************************************************************************/
static Time Get_Optional_XEvent_Argument_Returning_EventTime_Else_CurrentTime()
{
  XEvent *event;

  if (moreargs()) {

    event = get_xevent(xlga_xevent());

    switch (event->type) {	/* better safe than sorry -- don't know what kind of XEvent will get passed... */
    case KeyPress:
    case KeyRelease:
      return (event->xkey.time);
      break;
    case ButtonPress:
    case ButtonRelease:
      return (event->xbutton.time);
      break;
    case MotionNotify:
      return (event->xmotion.time);
      break;
    case EnterNotify:
    case LeaveNotify:
      return (event->xcrossing.time);
      break;
    case PropertyNotify:
      return (event->xproperty.time);
      break;
    case SelectionClear:
      return (event->xselectionclear.time);
      break;
    case SelectionRequest:
      return (event->xselectionrequest.time);
      break;
    case SelectionNotify:
      return (event->xselection.time);
      break;
    default:
      xlfail("Invalid XEvent kind passed to XmText/XmTextField function -- missing <time> slot.");
      break;
    }
  }
  else
#ifdef WINTERP_MOTIF_11
    {
      extern Display* display;	/* global in winterp.c */
      return (XtLastTimestampProcessed(display));
    }
#else
    return (CurrentTime);
#endif				/* WINTERP_MOTIF_11 */
}

/*****************************************************************************
 * (send XM_TEXT_WIDGET_CLASS :new 
 *                           [:managed/:unmanaged]
 *                           [:scrolled]
 *                           [<name>]
 *                           <parent> 
 *                           [:XMN_<arg1> <val1>]
 *                           [. . .             ]
 *                           [:XMN_<argN> <valN>])
 *
 * The optional keyword submessage :managed will cause a subsequent call
 * to XtManageChild(). If the submessage :unmanaged is present, or no
 * submessage, then XtManageChild() won't be called, and the resulting
 * widget will be returned unmanaged.
 *
 *  (send XM_TEXT_WIDGET_CLASS :new ...)
 *  --> XmCreateText();
 *  (send XM_TEXT_WIDGET_CLASS :new :scrolled ...)
 *  --> XmCreateScrolledText();
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_ISNEW()
{
  extern ArgList Wres_Get_LispArglist(); /* from w_resources.c */
  extern void    Wres_Free_C_Arglist_Data(); /* from w_resources.c */
  extern LVAL k_managed, k_unmanaged, k_scrolled;
  LVAL self, o_parent;
  char* name;
  Boolean managed_p, scrolled_p;
  Widget parent_widget_id, widget_id;

  self = xlgaobject();		/* NOTE: xlobj.c:clnew() returns an OBJECT; if this method
				   returns successfully, it will return a WIDGETOBJ */

  /* get optional managed/unmanaged arg */
  if (moreargs() && ((*xlargv == k_managed) || (*xlargv == k_unmanaged)))
    managed_p = (nextarg() == k_managed);
  else
    managed_p = FALSE;		/* by default don't call XtManageChild() */

  /* get optional :scrolled arg */
  if (moreargs() && (*xlargv == k_scrolled)) {
    nextarg();
    scrolled_p = TRUE;
  }
  else
    scrolled_p = FALSE;		/* by default, we don't want a scrolled editor */

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
    if (scrolled_p)
      widget_id = XmCreateScrolledText(parent_widget_id, name, xt_arglist, xt_numargs);
    else
      widget_id = XmCreateText(parent_widget_id, name, xt_arglist, xt_numargs);
    Wres_Free_C_Arglist_Data();
  }
  else 
    if (scrolled_p)
      widget_id = XmCreateScrolledText(parent_widget_id, name, ARGLIST());
    else
      widget_id = XmCreateText(parent_widget_id, name, ARGLIST());

  Wcls_Initialize_WIDGETOBJ(self, widget_id);

  if (managed_p)
    XtManageChild(widget_id);

#ifdef DEBUG_WINTERP_1
  Wcls_Print_WidgetObj_Info(self);
#endif
  return (self);
}


#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <textdget> :PARENT)
 *	==> returns widgetobj
 *
 * This method is needed to work around a problem in Motif 1.1. It serves
 * to override incorrect behavior when you call (send <listwidget> :PARENT)
 * using the method defined on WIDGET_CLASS (List's super..class).
 * 
 * The problem was that :PARENT was returning the same widgetobj it was 
 * given, but only in the case of a list/text widget created with the
 * :scrolled option. Therefore, we check for this case, and if it occured
 * we create a new widgetobj for the scrolled-window parent...
 *
 * Detailed analysis of problem: For each widgetobj created by winterp,
 * we store the widgetobj on each  widgetID by doing
 * XtSetValues(widgetid, XmNuserData<-->widgetobj).
 * If the widget is a "hidden" child or parent of a widgetobj, then
 * XmNuserData isn't set, so it defaults to NULL. When we retrieve a
 * widgetID from the toolkit (as done when XtParent() is called) we then
 * look at XmNuserData to lookup the associated widgetobj. If XmNuserData
 * is NULL, then we create a new widgetobj of the appropriate class and
 * initialize the widgetobj's widgetID...
 * 
 * The problem here results from the fact that XtGetValues(XmNuserData)
 * on the parent widget returns the child's widgetobj. Since the
 * parent widget is "hidden", we'd expect XmNuserData to be NULL.
 * Instead, XmNuserData for the parent is set to the widgetobj of the
 * child! This proc works around that problem...
 *
 * Note: see also w_libXt.c:Widget_Class_Method_PARENT(),
 * wc_List.c:Xm_List_Widget_Class_Method_PARENT()
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_PARENT()
{
  extern LVAL Wcls_WidgetClassID_To_WIDGETCLASSOBJ(); /* w_classes.c */
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ();

  LVAL self, lval_parent;
  Widget widget_id, parent_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  parent_id = XtParent(widget_id);
  lval_parent = Wcls_WidgetID_To_WIDGETOBJ(parent_id);
  
  if (lval_parent != self)
    return (lval_parent);
  else {			/* handle motif 1.1 fuckup */
    LVAL o_widgetclass;
    if (o_widgetclass = Wcls_WidgetClassID_To_WIDGETCLASSOBJ(XtClass(parent_id))) /* returns NIL on failure */
      /* (2): Create a new WIDGETOBJ of the appropriate class. */
      lval_parent = newobject(o_widgetclass, WIDGETOBJ_SIZE);
    else {			/* fail gracefully (i think). */
      extern LVAL o_WIDGET_CLASS;	/* from wc_WIDGET.c */
      errputstr("Warning -- In Wcls_WidgetID_To_WIDGETOBJ() couldn't find a valid\n");
      errputstr("           widgetclass object inside widget classrecord. Creating\n");
      errputstr("           a \"generic\" WIDGETOBJ of class WIDGET_CLASS.\n");
      lval_parent = newobject(o_WIDGET_CLASS, WIDGETOBJ_SIZE);
    }
    Wcls_Initialize_WIDGETOBJ(lval_parent, parent_id);
    
    /* Store the pointer to the new widgetobj in the widget's XmNuserData resource. */
    ARGLIST_RESET(); ARGLIST_ADD(XmNuserData, (XtArgVal) lval_parent);
    XtSetValues(parent_id, ARGLIST());  
    return (lval_parent);
  }
}
#endif				/* WINTERP_MOTIF_11 */



/******************************************************************************
 * typedef struct {
 *     char *ptr;               -- Pointer to data.
 *     int length;              -- Number of bytes of data.
 *     XmTextFormat format;     -- Representation format.
 * } XmTextBlockRec, *XmTextBlock;
 * 
 * typedef struct
 * {
 *     int reason;
 *     XEvent  *event;
 *     Boolean doit;
 *     long currInsert, newInsert;
 *     long startPos, endPos;
 *     XmTextBlock text;
 * } XmTextVerifyCallbackStruct, *XmTextVerifyPtr;
 *
 * Note that the text callbacks are "weird" in that they expect values in
 * the callback structure to be set inside the callback proc to determine
 * what actions need to be taken after the callbackproc returns. In
 * particular, the XmTextVerifyCallbackStruct's 'doit' slot is always set to True,
 * and must be set to False if the callbackproc doesn't want the action to be
 * taken. To do this, Set_Call_Data_For_XmTextVerifyCallbackStruct() is
 * called by Wcb_Meta_Callbackproc() after the callback lisp code is evaluated,
 * and the values bound to these settable variables are set inside call_data....
 * 
 * Another inconsistency with the Text widget is that some callbacks on this widget
 * return XmAnyCallbackStruct's (XmNactivateCallback, XmNfocusCallback,
 * XmNvalueChangedCallback), whereas XmNlosingFocusCallback, XmNmodifyVerifyCallback,
 * and XmNmotionVerifyCallback return XmTextVerifyCallbackStruct. In the code below,
 * we look at the 'reason' slot of the call data, (which is present in both XmAnyCallbackStruct
 * and in XmTextVerifyCallbackStruct) to determine the kind of callback that occured
 * and we only bind the values that are appropriate for that kind  of callback.
 * Information about which slots are valid for particular callback was taken from
 * the documentation on the XmText(3X) widget, and verified against the Motif 1.1
 * source -- this is valid for both XmText and XmTextField widgets...
 ******************************************************************************/
static LVAL s_CALLBACK_DOIT, s_CALLBACK_CUR_INSERT, s_CALLBACK_NEW_INSERT, s_CALLBACK_START_POS, s_CALLBACK_END_POS, s_CALLBACK_TEXT;
static void Lexical_Bindings_For_XmTextVerifyCallbackStruct(bindings_list, lexical_env, cd, o_widget)
     LVAL bindings_list;	/* a list of symbols to which values from XmTextVerifyCallbackStruct are bound */
     LVAL lexical_env;		
     XmTextVerifyCallbackStruct* cd;
     LVAL o_widget;		/* XLTYPE_WIDGETOBJ */
{
  extern LVAL s_CALLBACK_WIDGET, s_CALLBACK_REASON, s_CALLBACK_XEVENT; /* w_callbacks.c */
  extern LVAL Wcb_Get_Callback_Reason_Symbol();	/* w_callbacks.c */
  extern LVAL true;
  register LVAL s_bindname;

  switch (cd->reason) {

  case XmCR_LOSING_FOCUS:	/* valid XmTextVerifyCallbackStruct fields: reason, event, doit, currInsert, newInsert, startPos, endPos */

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
      else if (s_bindname == s_CALLBACK_DOIT) {
	xlpbind(s_bindname, true, lexical_env);	/* this value is to be set within the callback code, we just make space for it, bind to T */
      }
      else if (s_bindname == s_CALLBACK_CUR_INSERT) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->currInsert), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_NEW_INSERT) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->newInsert), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_START_POS) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->startPos), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_END_POS) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->endPos), lexical_env);
      }
      else {
	extern char temptext[];	/* from winterp.c */
	sprintf(temptext,
		"Unknown binding name in XmTextVerifyCallbackStruct callback evaluator for\n\tCALLBACK_REASON == CR_LOSING_FOCUS.\n\tValid symbols are [%s %s %s %s %s %s %s %s].",
		(char*) getstring(getpname(s_CALLBACK_WIDGET)),
		(char*) getstring(getpname(s_CALLBACK_REASON)),
		(char*) getstring(getpname(s_CALLBACK_XEVENT)),
		(char*) getstring(getpname(s_CALLBACK_DOIT)),
		(char*) getstring(getpname(s_CALLBACK_CUR_INSERT)),
		(char*) getstring(getpname(s_CALLBACK_NEW_INSERT)),
		(char*) getstring(getpname(s_CALLBACK_START_POS)),
		(char*) getstring(getpname(s_CALLBACK_END_POS)));
	xlerror(temptext, s_bindname);
      }
    }
    break;

  case XmCR_MODIFYING_TEXT_VALUE: /* valid XmTextVerifyCallbackStruct fields: reason, event, doit, currInsert, newInsert, text, startPos, endPos */

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
      else if (s_bindname == s_CALLBACK_DOIT) {
	xlpbind(s_bindname, true, lexical_env);	/* this value is to be set within the callback code, we just make space for it, bind to T */
      }
      else if (s_bindname == s_CALLBACK_CUR_INSERT) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->currInsert), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_NEW_INSERT) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->newInsert), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_START_POS) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->startPos), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_END_POS) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->endPos), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_TEXT) {
	/* INTERNATIONALIZATION-LIMITATION: currently, we ignore the format field of XmTextBlockRec */
	XmTextBlock textblockptr = cd->text;
	if (textblockptr) {
	  LVAL lval_string;
	  char* string;
	  xlpbind(s_bindname, (lval_string = newstring(textblockptr->length + 1)), lexical_env);
	  string = (char*) getstring(lval_string);
	  strncpy(string, textblockptr->ptr, textblockptr->length);
	  string[textblockptr->length] = '\0';
	}
	else 
	  xlpbind(s_bindname, NIL, lexical_env);
      }
      else {
	extern char temptext[];	/* from winterp.c */
	sprintf(temptext,
		"Unknown binding name in XmTextVerifyCallbackStruct callback evaluator for\n\tCALLBACK_REASON == XmCR_MODIFYING_TEXT_VALUE.\n\tValid symbols are [%s %s %s %s %s %s %s %s %s].",
		(char*) getstring(getpname(s_CALLBACK_WIDGET)),
		(char*) getstring(getpname(s_CALLBACK_REASON)),
		(char*) getstring(getpname(s_CALLBACK_XEVENT)),
		(char*) getstring(getpname(s_CALLBACK_DOIT)),
		(char*) getstring(getpname(s_CALLBACK_CUR_INSERT)),
		(char*) getstring(getpname(s_CALLBACK_NEW_INSERT)),
		(char*) getstring(getpname(s_CALLBACK_START_POS)),
		(char*) getstring(getpname(s_CALLBACK_END_POS)),
		(char*) getstring(getpname(s_CALLBACK_TEXT)));
	xlerror(temptext, s_bindname);
      }
    }
    break;

  case XmCR_MOVING_INSERT_CURSOR: /* valid XmTextVerifyCallbackStruct fields: reason, event, currInsert, newInsert, doit */

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
      else if (s_bindname == s_CALLBACK_DOIT) {
	xlpbind(s_bindname, true, lexical_env);	/* this value is to be set within the callback code, we just make space for it, bind to T */
      }
      else if (s_bindname == s_CALLBACK_CUR_INSERT) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->currInsert), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_NEW_INSERT) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->newInsert), lexical_env);
      }
      else {
	extern char temptext[];	/* from winterp.c */
	sprintf(temptext,
		"Unknown binding name in XmTextVerifyCallbackStruct callback evaluator for\n\tCALLBACK_REASON == CR_MOVING_INSERT_CURSOR.\n\t Valid symbols are [%s %s %s %s %s %s].",
		(char*) getstring(getpname(s_CALLBACK_WIDGET)),
		(char*) getstring(getpname(s_CALLBACK_REASON)),
		(char*) getstring(getpname(s_CALLBACK_XEVENT)),
		(char*) getstring(getpname(s_CALLBACK_DOIT)),
		(char*) getstring(getpname(s_CALLBACK_CUR_INSERT)),
		(char*) getstring(getpname(s_CALLBACK_NEW_INSERT)));
	xlerror(temptext, s_bindname);
      }
    }
    break;

  default:			/* valid call_data fields are from XmAnyCallbackStruct */

    for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {
      s_bindname = car(bindings_list);

      if (s_bindname == s_CALLBACK_WIDGET) {
	xlpbind(s_bindname, o_widget, lexical_env);
      }
      else if (s_bindname == s_CALLBACK_REASON) {
	xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(((XmAnyCallbackStruct*) cd)->reason), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_XEVENT) {
	xlpbind(s_bindname, (cd->event) ? cv_xevent(((XmAnyCallbackStruct*) cd)->event) : NIL, lexical_env);
      }
      else {
	extern char temptext[];	/* from winterp.c */
	sprintf(temptext,
		"Unknown binding name in XmText's XmAnyCallbackStruct callback evaluator. Valid symbols are [%s %s %s].",
		(char*) getstring(getpname(s_CALLBACK_WIDGET)),
		(char*) getstring(getpname(s_CALLBACK_REASON)),
		(char*) getstring(getpname(s_CALLBACK_XEVENT)));
	xlerror(temptext, s_bindname);
      }
    }
    break;
  }
}


/******************************************************************************
 * As far as I can tell, the only settable field in the Text widget's call_data
 * is the doit field. Of course, the documentation and the source are quite
 * cryptic about this!
 ******************************************************************************/
static void Set_Call_Data_For_XmTextVerifyCallbackStruct(bindings_list, lexical_env, cd)
     LVAL bindings_list;	/* a list of symbols whose values are to be set in XmTextVerifyCallbackStruct */
     LVAL lexical_env;		
     XmTextVerifyCallbackStruct* cd;
{
  register LVAL ep = car(lexical_env); /* get current environment stack frame which was created in
					  Wcb_Meta_Callbackproc() by Lexical_Bindings_For_XmTextVerifyCallbackStruct(). */
  
  for (; ep; ep = cdr(ep))	/* while there are more bindings in current environment */
    if (s_CALLBACK_DOIT == car(car(ep))) { /* check to see if this symbol was bound in the envt */
      cd->doit = (cdr(car(ep))) ? TRUE : FALSE;	/* set doit field if value is non-NIL */
      return;
    }
}


/******************************************************************************
 * This is called indirectly via XtAddCallback() for callbacks returning
 * an XmTextVerifyCallbackStruct as call_data.
 ******************************************************************************/
static void XmTextVerifyCallbackStruct_Callbackproc(widget, client_data, call_data)
     Widget    widget;
     XtPointer client_data;
     XtPointer call_data;
{
  extern void Wcb_Meta_Callbackproc(); /* w_callbacks.c */

  Wcb_Meta_Callbackproc(client_data, call_data,
			Lexical_Bindings_For_XmTextVerifyCallbackStruct,
			Set_Call_Data_For_XmTextVerifyCallbackStruct);
}


/******************************************************************************
 * Same as WIDGET_CLASS's :add_callback method except that this understands
 * how to get values from the XmTextVerifyCallbackStruct.
 ******************************************************************************/
LVAL Xm_Text_Widget_Class_Method_ADD_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */

  return (Wcb_Meta_Method_Add_Callback(XmTextVerifyCallbackStruct_Callbackproc, FALSE));
}


/******************************************************************************
 * Same as WIDGET_CLASS's :set_callback method except that this understands
 * how to get values from the XmTextVerifyCallbackStruct.
 ******************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SET_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */

  return (Wcb_Meta_Method_Add_Callback(XmTextVerifyCallbackStruct_Callbackproc, TRUE));
}

#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <text_widget> :SET_HIGHLIGHT <leftpos> <rightpos> <highlightmode>)
 *   --> <text_widget>
 *
 * void XmTextSetHighlight (Widget w, XmTextPosition left, XmTextPosition right, XmHighlightMode mode);
 * void XmTextFieldSetHighlight (Widget widget, XmTextPosition left, XmTextPosition right, XmHighlightMode mode);
 ****************************************************************************/
static LVAL s_HIGHLIGHT_NORMAL, s_HIGHLIGHT_SELECTED, s_HIGHLIGHT_SECONDARY_SELECTED;
LVAL Xm_Text_Widget_Class_Method_SET_HIGHLIGHT()
{
  LVAL self, lval_mode;
  XmTextPosition left, right;
  XmHighlightMode mode;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  left = Get_XmTextPosition_Argument();
  right = Get_XmTextPosition_Argument();
  lval_mode = xlgetarg();
  xllastarg();

  if (lval_mode == s_HIGHLIGHT_NORMAL)
    mode = XmHIGHLIGHT_NORMAL;
  else if (lval_mode == s_HIGHLIGHT_SELECTED)
    mode = XmHIGHLIGHT_SELECTED;
  else if (lval_mode == s_HIGHLIGHT_SECONDARY_SELECTED)
    mode = XmHIGHLIGHT_SECONDARY_SELECTED;
  else {
    extern char temptext[];	/* from winterp.c */
    sprintf(temptext,
	    "Invalid XmHighlightMode argument. Expected one of the following [%s %s %s].",
	    (char*) getstring(getpname(s_HIGHLIGHT_NORMAL)),
	    (char*) getstring(getpname(s_HIGHLIGHT_SELECTED)),
	    (char*) getstring(getpname(s_HIGHLIGHT_SECONDARY_SELECTED)));
    xlerror(temptext, lval_mode);
  }
  
  XmTextSetHighlight(widget_id, left, right, mode);

  return (self);
}

LVAL Xm_Text_Field_Widget_Class_Method_SET_HIGHLIGHT()
{
  LVAL self, lval_mode;
  XmTextPosition left, right;
  XmHighlightMode mode;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  left = Get_XmTextPosition_Argument();
  right = Get_XmTextPosition_Argument();
  lval_mode = xlgetarg();
  xllastarg();

  if (lval_mode == s_HIGHLIGHT_NORMAL)
    mode = XmHIGHLIGHT_NORMAL;
  else if (lval_mode == s_HIGHLIGHT_SELECTED)
    mode = XmHIGHLIGHT_SELECTED;
  else if (lval_mode == s_HIGHLIGHT_SECONDARY_SELECTED)
    mode = XmHIGHLIGHT_SECONDARY_SELECTED;
  else {
    extern char temptext[];	/* from winterp.c */
    sprintf(temptext,
	    "Invalid XmHighlightMode argument. Expected one of the following [%s %s %s].",
	    (char*) getstring(getpname(s_HIGHLIGHT_NORMAL)),
	    (char*) getstring(getpname(s_HIGHLIGHT_SELECTED)),
	    (char*) getstring(getpname(s_HIGHLIGHT_SECONDARY_SELECTED)));
    xlerror(temptext, lval_mode);
  }
  
  XmTextFieldSetHighlight(widget_id, left, right, mode);

  return (self);
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :GET_STRING)
 *   --> string displayed by the text edit widget
 *
 * char *XmTextGetString (Widget widget);
 * char *XmTextFieldGetString (Widget widget);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_STRING()
{
  LVAL self, s_result;
  char* string;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  if (string = XmTextGetString(widget_id)) {
/*
    s_result = cvstring(string);
    XtFree(string);
*/
    s_result = cv_string(string); /* XmTextGetString() result is not copied, will be freed by XLISP-GC */
  }
  else 
    s_result = cvstring("");	/* don't call cv_string on this because GC would end up doing XtFree("")... */

  return (s_result);
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_GET_STRING()
{
  LVAL self, s_result;
  char* string;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  if (string = XmTextFieldGetString(widget_id)) {
/*
    s_result = cvstring(string);
    XtFree(string);
*/
    s_result = cv_string(string); /* XmTextFieldGetString() result is not copied, will be freed by XLISP-GC */
  }
  else 
    s_result = cvstring("");	/* don't call cv_string on this because GC would end up doing XtFree("")... */

  return (s_result);
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :GET_LAST_POSITION)
 * --> integer representing last position
 *
 * XmTextPosition XmTextGetLastPosition (Widget widget);
 * XmTextPosition XmTextFieldGetLastPosition (Widget widget);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_LAST_POSITION()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

#if (defined(WINTERP_MOTIF_11) || defined(hpux))
  return (cvfixnum((FIXTYPE) XmTextGetLastPosition(widget_id)));
#else
  /*
   * In HPUX 7.0's Motif 1.0, XmTextGetLastPosition() is defined... however
   * other manufacturers libXm (e.g. IBM AIX 3.1) don't export XmTextGetLastPosition().
   * In order to make WINTERP compile with the unknown Motif 1.0 libXm's "out there,
   * I've decided to only allow Motif 1.0 access to XmTextGetLastPosition() from HPUX.
   * If your Motif 1.0 dist supports this, then make sure the above #if expression becomes true.
   */
  xlfail("XmTextGetLastPosition() may not be defined on non-HPUX Motif 1.0.\nIf you know that it exists in your Motif installation, and you need to use\nthis method, you should recompile winterp after modifying src-server/wc_Text.c.");
  return (NIL);
#endif
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_GET_LAST_POSITION()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (cvfixnum((FIXTYPE) XmTextFieldGetLastPosition(widget_id)));
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :SET_STRING <string>)
 *
 * void XmTextSetString (Widget widget, char *value);
 * void XmTextFieldSetString (Widget widget, char *value);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SET_STRING()
{
  LVAL self;
  char* string;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  string = (char*) getstring(xlgastring());
  xllastarg();

  XmTextSetString(widget_id, string);
  return (self);
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_SET_STRING()
{
  LVAL self;
  char* string;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  string = (char*) getstring(xlgastring());
  xllastarg();

  XmTextFieldSetString(widget_id, string);
  return (self);
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :replace <from-pos> <to-pos> <string>)
 *
 * void XmTextReplace (Widget widget, XmTextPosition frompos, XmTextPosition topos, char *value);
 * void XmTextFieldReplace (Widget widget, XmTextPosition from_pos, XmTextPosition to_pos, char *value);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_REPLACE()
{
  LVAL self;
  XmTextPosition frompos, topos;
  char* string;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  frompos = Get_XmTextPosition_Argument();
  topos = Get_XmTextPosition_Argument();
  string = (char*) getstring(xlgastring());
  xllastarg();

  XmTextReplace(widget_id, frompos, topos, string);
  return (self);
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_REPLACE()
{
  LVAL self;
  XmTextPosition frompos, topos;
  char* string;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  frompos = Get_XmTextPosition_Argument();
  topos = Get_XmTextPosition_Argument();
  string = (char*) getstring(xlgastring());
  xllastarg();

  XmTextFieldReplace(widget_id, frompos, topos, string);
  return (self);
}
#endif				/* WINTERP_MOTIF_11 */


#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <text_widget> :INSERT <position> <value>)
 *   --> <text_widget>
 *
 * void XmTextInsert (Widget widget, XmTextPosition position, char *value);
 * void XmTextFieldInsert (Widget widget, XmTextPosition position, char *value);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_INSERT()
{
  LVAL self, lval_mode;
  XmTextPosition position;
  char* value;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  position = Get_XmTextPosition_Argument();
  value = (char*) getstring(xlgastring());
  xllastarg();

  XmTextInsert(widget_id, position, value);

  return (self);
}

LVAL Xm_Text_Field_Widget_Class_Method_INSERT()
{
  LVAL self, lval_mode;
  XmTextPosition position;
  char* value;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  position = Get_XmTextPosition_Argument();
  value = (char*) getstring(xlgastring());
  xllastarg();

  XmTextFieldInsert(widget_id, position, value);

  return (self);
}
#endif				/* WINTERP_MOTIF_11 */


#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <text_widget> :SET_ADD_MODE <add-modeP>)
 *
 * void XmTextSetAddMode (Widget widget, Boolean state);
 * void XmTextFieldSetAddMode (Widget widget, Boolean state);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SET_ADD_MODE()
{
  LVAL self, lval_addmodeP;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  lval_addmodeP = xlgetarg();
  xllastarg();

  XmTextSetAddMode(widget_id, (lval_addmodeP) ? TRUE : FALSE);

  return (self);
}

LVAL Xm_Text_Field_Widget_Class_Method_SET_ADD_MODE()
{
  LVAL self, lval_addmodeP;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  lval_addmodeP = xlgetarg();
  xllastarg();

  XmTextFieldSetAddMode(widget_id, (lval_addmodeP) ? TRUE : FALSE);

  return (self);
}
#endif				/* WINTERP_MOTIF_11 */


#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <text_widget> :GET_ADD_MODE)
 *      ====> returns T if in add mode, else NIL.
 *
 * Boolean XmTextGetAddMode (Widget widget);
 & Boolean XmTextFieldGetAddMode (Widget widget);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_ADD_MODE()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (XmTextGetAddMode(widget_id) ? true : NIL);
}

LVAL Xm_Text_Field_Widget_Class_Method_GET_ADD_MODE()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (XmTextFieldGetAddMode(widget_id) ? true : NIL);
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :get_editable)
 *      ====> returns T if editable, NIL otherwise.
 *
 * Boolean XmTextGetEditable (Widget widget);
 * Boolean XmTextFieldGetEditable (Widget widget);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_EDITABLE()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (XmTextGetEditable(widget_id) ? true : NIL);
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_GET_EDITABLE()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (XmTextFieldGetEditable(widget_id) ? true : NIL);
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :set_editable <editableP>)
 *
 * void XmTextSetEditable (Widget widget, Boolean editable);
 * void XmTextFieldSetEditable (Widget widget, Boolean editable);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SET_EDITABLE()
{
  LVAL self, lval_editablep;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  lval_editablep = xlgetarg();
  xllastarg();

  XmTextSetEditable(widget_id, (lval_editablep) ? TRUE : FALSE);
  return (self);
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_SET_EDITABLE()
{
  LVAL self, lval_editablep;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  lval_editablep = xlgetarg();
  xllastarg();

  XmTextFieldSetEditable(widget_id, (lval_editablep) ? TRUE : FALSE);
  return (self);
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :GET_MAX_LENGTH)
 *       =====> FIXNUM, the maximum length of the Text widget.
 *
 * int XmTextGetMaxLength (Widget widget);
 * int XmTextFieldGetMaxLength (Widget widget);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_MAX_LENGTH()
{
  LVAL self, s_result;
  char* string;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (cvfixnum((FIXTYPE) XmTextGetMaxLength(widget_id)));
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_GET_MAX_LENGTH()
{
  LVAL self, s_result;
  char* string;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (cvfixnum((FIXTYPE) XmTextFieldGetMaxLength(widget_id)));
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :SET_MAX_LENGTH <max_length>)
 *
 * void XmTextSetMaxLength (Widget widget, int max_length);
 * void XmTextFieldSetMaxLength (Widget widget, int max_length);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SET_MAX_LENGTH()
{
  LVAL self;
  int max_length;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  max_length = Get_Int_Argument(0L, (long) INT_MAX);
  xllastarg();

  XmTextSetMaxLength(widget_id, max_length);
  return (self);
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_SET_MAX_LENGTH()
{
  LVAL self;
  int max_length;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  max_length = Get_Int_Argument(0L, (long) INT_MAX);
  xllastarg();

  XmTextFieldSetMaxLength(widget_id, max_length);
  return (self);
}
#endif				/* WINTERP_MOTIF_11 */


#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <text_widget> :REMOVE) -- deletes the primary selected text and calls the widget's XmNmodifyVerifyCallback and XmNvalueChangedCallback callbacks if there is a selection.
 *	====> returns T normally; returns NIL if the primary selection is NULL
 *	      or if the widget doesn't own the primary selection.
 *
 * Boolean XmTextRemove (Widget widget);
 * Boolean XmTextFieldRemove (Widget widget);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_REMOVE()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (XmTextRemove(widget_id) ? true : NIL);
}

LVAL Xm_Text_Field_Widget_Class_Method_REMOVE()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (XmTextFieldRemove(widget_id) ? true : NIL);
}
#endif				/* WINTERP_MOTIF_11 */


#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <text_widget> :COPY [<xevent>]) -- copies the primary selection to the clipboard
 *	=====> Returns T normally; returns NIL if the primary selection is NULL,
 *      if the widget doesn't own the primary selection, or if the function
 *      is unable to gain ownership of the clipboard selection.
 *
 * Note that the optional <xevent> argument is used to set the time of the
 * selection. Since the time value comes from the time slot on the XEvent
 * bound to the callback symbol CALLBACK_XEVENT, we just pass the <xevent>
 * generated by the callback...
 *
 * Boolean XmTextCopy (Widget widget, Time copy_time);
 * Boolean XmTextFieldCopy (Widget widget, Time clip_time);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_COPY()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;
  Time time;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  time = Get_Optional_XEvent_Argument_Returning_EventTime_Else_CurrentTime();
  xllastarg();

  return (XmTextCopy(widget_id, time) ? true : NIL);
}

LVAL Xm_Text_Field_Widget_Class_Method_COPY()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;
  Time time;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  time = Get_Optional_XEvent_Argument_Returning_EventTime_Else_CurrentTime();
  xllastarg();

  return (XmTextFieldCopy(widget_id, time) ? true : NIL);
}
#endif				/* WINTERP_MOTIF_11 */


#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <text_widget> :CUT [<xevent>]) -- copies the primary selected text to the clipboard and then deletes the primary selected text; also calls the widget's XmNmodifyVerifyCallback and XmNvalueChangedCallback callbacks.
 *	=====> Returns T normally; returns NIL if the primary selection is NULL,
 *      if the widget doesn't own the primary selection, or if the function
 *      is unable to gain ownership of the clipboard selection.
 *
 * Note that the optional <xevent> argument is used to set the time of the
 * selection. Since the time value comes from the time slot on the XEvent
 * bound to the callback symbol CALLBACK_XEVENT, we just pass the <xevent>
 * generated by the callback...
 *
 * Boolean XmTextCut (Widget widget, Time cut_time);
 * Boolean XmTextFieldCut (Widget widget, Time clip_time);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_CUT()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;
  Time time;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  time = Get_Optional_XEvent_Argument_Returning_EventTime_Else_CurrentTime();
  xllastarg();

  return (XmTextCut(widget_id, time) ? true : NIL);
}

LVAL Xm_Text_Field_Widget_Class_Method_CUT()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;
  Time time;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  time = Get_Optional_XEvent_Argument_Returning_EventTime_Else_CurrentTime();
  xllastarg();

  return (XmTextFieldCut(widget_id, time) ? true : NIL);
}
#endif				/* WINTERP_MOTIF_11 */


#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <text_widget> :PASTE) -- inserts the clipboard selection at the destination cursor. If XmNpendingDelete is True and the destination cursor is inside the current selection, the clipboard selection replaces the selected text. Also calls the widget's XmNmodifyVerifyCallback and XmNvalueChangedCallback callbacks...
 *	====> returns T normally; returns NIL if the primary selection is NULL
 *	      or if the widget doesn't own the primary selection.
 *
 * Boolean XmTextPaste (Widget widget);
 * Boolean XmTextFieldPaste (Widget widget);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_PASTE()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (XmTextPaste(widget_id) ? true : NIL);
}

LVAL Xm_Text_Field_Widget_Class_Method_PASTE()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (XmTextFieldPaste(widget_id) ? true : NIL);
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :GET_SELECTION)
 *       ===> returns a string, the selected text.
 *
 * char *XmTextGetSelection (Widget widget);
 * char *XmTextFieldGetSelection (Widget widget);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_SELECTION()
{
  LVAL self, s_result;
  char* string;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  if (string = XmTextGetSelection(widget_id)) {
/*
    s_result = cvstring(string);
    XtFree(string);
*/
    s_result = cv_string(string); /* XmTextGetSelection() result is not copied, will be freed by XLISP-GC */
  }
  else
    s_result = cvstring("");	/* don't call cv_string on this because GC would end up doing XtFree("")... */

  return (s_result);
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_GET_SELECTION()
{
  LVAL self, s_result;
  char* string;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  if (string = XmTextFieldGetSelection(widget_id)) {
/*
    s_result = cvstring(string);
    XtFree(string);
*/
    s_result = cv_string(string); /* XmTextFieldGetSelection() result is not copied, will be freed by XLISP-GC */
  }
  else
    s_result = cvstring("");	/* don't call cv_string on this because GC would end up doing XtFree("")... */

  return (s_result);
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :SET_SELECTION <firstpos> <lastpos> [<xevent>])
 * Note that the optional <xevent> argument is used to set the time of the
 * selection. Since the time value comes from the time slot on the XEvent
 * bound to the callback symbol CALLBACK_XEVENT, we just pass the <xevent>
 * generated by the callback...
 *
 * void XmTextSetSelection (Widget widget, XmTextPosition first, XmTextPosition last, Time set_time);
 * void XmTextFieldSetSelection (Widget widget, XmTextPosition first, XmTextPosition last, Time sel_time);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SET_SELECTION()
{
  LVAL self;
  XmTextPosition firstpos, lastpos;
  Widget widget_id;
  Time time;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  firstpos = Get_XmTextPosition_Argument();
  lastpos = Get_XmTextPosition_Argument();
  time = Get_Optional_XEvent_Argument_Returning_EventTime_Else_CurrentTime();
  xllastarg();

  XmTextSetSelection(widget_id, firstpos, lastpos, time); 
  return (self);
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_SET_SELECTION()
{
  LVAL self;
  XmTextPosition firstpos, lastpos;
  Widget widget_id;
  Time time;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  firstpos = Get_XmTextPosition_Argument();
  lastpos = Get_XmTextPosition_Argument();
  time = Get_Optional_XEvent_Argument_Returning_EventTime_Else_CurrentTime();
  xllastarg();

  XmTextFieldSetSelection(widget_id, firstpos, lastpos, time);
  return (self);
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :CLEAR_SELECTION [<xevent>])
 * Note that the optional <xevent> argument is used to set the time of the
 * selection. Since the time value comes from the time slot on the XEvent
 * bound to the callback symbol CALLBACK_XEVENT, we just pass the <xevent>
 * generated by the callback...
 *
 * void XmTextClearSelection (Widget widget, Time clear_time);
 * void XmTextFieldClearSelection (Widget widget, Time sel_time);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_CLEAR_SELECTION()
{
  LVAL self;
  Widget widget_id;
  Time time;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  time = Get_Optional_XEvent_Argument_Returning_EventTime_Else_CurrentTime();
  xllastarg();

  XmTextClearSelection(widget_id, time);

  return (self);
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_CLEAR_SELECTION()
{
  LVAL self;
  Widget widget_id;
  Time time;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  time = Get_Optional_XEvent_Argument_Returning_EventTime_Else_CurrentTime();
  xllastarg();

  XmTextFieldClearSelection(widget_id, time);

  return (self);
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :GET_TOP_CHARACTER)
 *       ====> returns a FIXNUM representing top position of Text widget.
 *
 * Note that this method is on XmText only; missing from XmTextField
 * XmTextPosition XmTextGetTopCharacter (Widget widget);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_TOP_CHARACTER()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

#ifndef WINTERP_MOTIF_11
#ifdef hpux			/* HPUX 7.0 uses Motif 1.0 -- name change to XmTextSetTopCharacter() occured in Motif patch 1.01 */
  return (cvfixnum((FIXTYPE) XmTextGetTopPosition(widget_id)));
#else /* not hpux */
  return (cvfixnum((FIXTYPE) XmTextGetTopCharacter(widget_id)));
#endif /* hpux */
#else /* WINTERP_MOTIF_11 */
  return (cvfixnum((FIXTYPE) XmTextGetTopCharacter(widget_id)));
#endif /* WINTERP_MOTIF_11 */
}


/*****************************************************************************
 * (send <text_widget> :SET_TOP_CHARACTER <top_position>)
 *
 * Note that this method is on XmText only; missing from XmTextField
 * void XmTextSetTopCharacter (Widget widget, XmTextPosition top_character);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SET_TOP_CHARACTER()
{
  LVAL self;
  XmTextPosition top_position;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  top_position = Get_XmTextPosition_Argument();
  xllastarg();

#ifndef WINTERP_MOTIF_11
#ifdef hpux			/* HPUX 7.0 uses Motif 1.0 -- name change to XmTextSetTopCharacter() occured in Motif patch 1.01 */
  XmTextSetTopPosition(widget_id, top_position);
#else /* hpux */
  XmTextSetTopCharacter(widget_id, top_position);
#endif /* hpux */
#else /* WINTERP_MOTIF_11 */
  XmTextSetTopCharacter(widget_id, top_position);
#endif /* WINTERP_MOTIF_11 */

  return (self);
}


/*****************************************************************************
 * (send <text_widget> :GET_INSERTION_POSITION)
 *
 * XmTextPosition XmTextGetInsertionPosition (Widget widget);
 * XmTextPosition XmTextFieldGetInsertionPosition (Widget widget);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_INSERTION_POSITION()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (cvfixnum((FIXTYPE) XmTextGetInsertionPosition(widget_id)));
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_GET_INSERTION_POSITION()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (cvfixnum((FIXTYPE) XmTextFieldGetInsertionPosition(widget_id)));
}
#endif				/* WINTERP_MOTIF_11 */

#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <text_widget> :GET_CURSOR_POSITION)
 *
 * XmTextPosition XmTextGetCursorPosition (Widget widget);
 * XmTextPosition XmTextFieldGetCursorPosition (Widget widget);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_CURSOR_POSITION()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (cvfixnum((FIXTYPE) XmTextGetCursorPosition(widget_id)));
}

LVAL Xm_Text_Field_Widget_Class_Method_GET_CURSOR_POSITION()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (cvfixnum((FIXTYPE) XmTextFieldGetCursorPosition(widget_id)));
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :SET_INSERTION_POSITION <position>)
 *
 * void XmTextSetInsertionPosition (Widget widget, XmTextPosition position);
 * void XmTextFieldSetInsertionPosition (Widget widget, XmTextPosition position);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SET_INSERTION_POSITION()
{
  LVAL self;
  XmTextPosition insertion_position;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  insertion_position = Get_XmTextPosition_Argument();
  xllastarg();

  XmTextSetInsertionPosition(widget_id, insertion_position);
  return (self);
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_SET_INSERTION_POSITION()
{
  LVAL self;
  XmTextPosition insertion_position;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  insertion_position = Get_XmTextPosition_Argument();
  xllastarg();

  XmTextFieldSetInsertionPosition(widget_id, insertion_position);
  return (self);
}
#endif				/* WINTERP_MOTIF_11 */


#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <text_widget> :SET_CURSOR_POSITION <position>)
 *
 * void XmTextSetCursorPosition (Widget widget, XmTextPosition position);
 * void XmTextFieldSetCursorPosition (Widget widget, XmTextPosition position);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SET_CURSOR_POSITION()
{
  LVAL self;
  XmTextPosition insertion_position;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  insertion_position = Get_XmTextPosition_Argument();
  xllastarg();

  XmTextSetCursorPosition(widget_id, insertion_position);
  return (self);
}

LVAL Xm_Text_Field_Widget_Class_Method_SET_CURSOR_POSITION()
{
  LVAL self;
  XmTextPosition insertion_position;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  insertion_position = Get_XmTextPosition_Argument();
  xllastarg();

  XmTextFieldSetCursorPosition(widget_id, insertion_position);
  return (self);
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :GET_SELECTION_POSITION)
 *
 * Boolean XmTextGetSelectionPosition (Widget widget, XmTextPosition *left, XmTextPosition *right);
 * Boolean XmTextFieldGetSelectionPosition (Widget widget, XmTextPosition *left, XmTextPosition *right);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_SELECTION_POSITION()
{
  LVAL self, lval_result, lval_left_pos, lval_right_pos;
  Widget widget_id;
  long left_pos, right_pos;

  /* protect some pointers */
  xlstkcheck(3);
  xlsave(lval_left_pos);
  xlsave(lval_right_pos);
  xlsave(lval_result);

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

#if (defined(WINTERP_MOTIF_11) || defined(hpux))
  if (XmTextGetSelectionPosition(widget_id, &left_pos, &right_pos)) {
    lval_left_pos = cvfixnum((FIXTYPE) left_pos);
    lval_right_pos = cvfixnum((FIXTYPE) right_pos);
    lval_result = cons(lval_left_pos, lval_right_pos);
  }
  else 
    lval_result = NIL;
#else
  /*
   * In HPUX 7.0's Motif 1.0, XmTextGetSelectionPosition() is defined... however
   * other manufacturers libXm (e.g. IBM AIX 3.1) don't export XmTextGetSelectionPosition().
   * In order to make WINTERP compile with the unknown Motif 1.0 libXm's "out there,
   * I've decided to only allow Motif 1.0 access to XmTextGetSelectionPosition() from HPUX.
   * If your Motif 1.0 dist supports this, then make sure the above #if expression becomes true.
   */
  xlfail("XmTextGetSelectionPosition() may not be defined on non-HPUX Motif 1.0.\nIf you know that it exists in your Motif installation, and you need to use\nthis method, you should recompile winterp after modifying src-server/wc_Text.c.");
#endif

  /* restore the stack */
  xlpopn(3);

  return (lval_result);
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_GET_SELECTION_POSITION()
{
  LVAL self, lval_result, lval_left_pos, lval_right_pos;
  Widget widget_id;
  long left_pos, right_pos;

  /* protect some pointers */
  xlstkcheck(3);
  xlsave(lval_left_pos);
  xlsave(lval_right_pos);
  xlsave(lval_result);

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  if (XmTextFieldGetSelectionPosition(widget_id, &left_pos, &right_pos)) {
    lval_left_pos = cvfixnum((FIXTYPE) left_pos);
    lval_right_pos = cvfixnum((FIXTYPE) right_pos);
    lval_result = cons(lval_left_pos, lval_right_pos);
  }
  else 
    lval_result = NIL;

  /* restore the stack */
  xlpopn(3);

  return (lval_result);
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :XY_TO_POS <x> <y>)
 *
 * XmTextPosition XmTextXYToPos (Widget widget, Position x, Position y);
 * XmTextPosition XmTextFieldXYToPos (Widget widget, Position x, Position y);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_XY_TO_POS()
{
  LVAL self;
  Position x, y;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  x = (Position) Get_Int_Argument((long) SHRT_MIN, (long) SHRT_MAX);
  y = (Position) Get_Int_Argument((long) SHRT_MIN, (long) SHRT_MAX);
  xllastarg();

#if (defined(WINTERP_MOTIF_11) || defined(hpux))
  return (cvfixnum((FIXTYPE) XmTextXYToPos(widget_id, x, y)));
#else
  /*
   * In HPUX 7.0's Motif 1.0,  XmTextXYToPos() is defined... however
   * other manufacturers libXm (e.g. IBM AIX 3.1) don't export  XmTextXYToPos().
   * In order to make WINTERP compile with the unknown Motif 1.0 libXm's "out there,
   * I've decided to only allow Motif 1.0 access to  XmTextXYToPos() from HPUX.
   * If your Motif 1.0 dist supports this, then make sure the above #if expression becomes true.
   */
  xlfail("XmTextXYToPos() may not be defined on non-HPUX Motif 1.0.\nIf you know that it exists in your Motif installation, and you need to use\nthis method, you should recompile winterp after modifying src-server/wc_Text.c.");
  return (NIL);
#endif
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_XY_TO_POS()
{
  LVAL self;
  Position x, y;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  x = (Position) Get_Int_Argument((long) SHRT_MIN, (long) SHRT_MAX);
  y = (Position) Get_Int_Argument((long) SHRT_MIN, (long) SHRT_MAX);
  xllastarg();

  return (cvfixnum((FIXTYPE) XmTextFieldXYToPos(widget_id, x, y)));
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :POS_TO_XY <position>)
 *
 * Boolean XmTextPosToXY (Widget widget, XmTextPosition position, Position *x, Position *y);
 * Boolean XmTextFieldPosToXY (Widget widget, XmTextPosition position, Position *x, Position *y);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_POS_TO_XY()
{
  LVAL self, lval_result, lval_x, lval_y;
  Widget widget_id;
  XmTextPosition position;
  Position x, y;

  /* protect some pointers */
  xlstkcheck(3);
  xlsave(lval_x);
  xlsave(lval_y);
  xlsave(lval_result);

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  position =  Get_XmTextPosition_Argument();
  xllastarg();

#if (defined(WINTERP_MOTIF_11) || defined(hpux))
  if (XmTextPosToXY(widget_id, position, &x, &y)) {
    lval_x = cvfixnum((FIXTYPE) x);
    lval_y = cvfixnum((FIXTYPE) y);
    lval_result = cons(lval_x, lval_y);
  }
  else 
    lval_result = NIL;
#else
  /*
   * In HPUX 7.0's Motif 1.0, XmTextPosToXY() is defined... however
   * other manufacturers libXm (e.g. IBM AIX 3.1) don't export  XmTextPosToXY().
   * In order to make WINTERP compile with the unknown Motif 1.0 libXm's "out there,
   * I've decided to only allow Motif 1.0 access to XmTextPosToXY() from HPUX.
   * If your Motif 1.0 dist supports this, then make sure the above #if expression becomes true.
   */
  xlfail("XmTextPosToXY() may not be defined on non-HPUX Motif 1.0.\nIf you know that it exists in your Motif installation, and you need to use\nthis method, you should recompile winterp after modifying src-server/wc_Text.c.");
#endif

  /* restore the stack */
  xlpopn(3);

  return (lval_result);
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_POS_TO_XY()
{
  LVAL self, lval_result, lval_x, lval_y;
  Widget widget_id;
  XmTextPosition position;
  Position x, y;

  /* protect some pointers */
  xlstkcheck(3);
  xlsave(lval_x);
  xlsave(lval_y);
  xlsave(lval_result);

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  position =  Get_XmTextPosition_Argument();
  xllastarg();

  if (XmTextFieldPosToXY(widget_id, position, &x, &y)) {
    lval_x = cvfixnum((FIXTYPE) x);
    lval_y = cvfixnum((FIXTYPE) y);
    lval_result = cons(lval_x, lval_y);
  }
  else 
    lval_result = NIL;

  /* restore the stack */
  xlpopn(3);

  return (lval_result);
}
#endif				/* WINTERP_MOTIF_11 */


/******************************************************************************
 * Ignore this one for now, since text sources are best set and programmed in C
 * This method is for XmText only.
 *
 * XmTextSource XmTextGetSource (Widget widget);
 ******************************************************************************/


/******************************************************************************
 * Ignore this one for now, since text sources are best set and programmed in C
 * This method is for XmText only.
 *
 * void XmTextSetSource (Widget widget,
 *                       XmTextSource source,
 *                       XmTextPosition top_character,
 *                       XmTextPosition cursor_position);
 ******************************************************************************/


/*****************************************************************************
 * (send <text_widget> :SHOW_POSITION <position>)
 *
 * void XmTextShowPosition (Widget widget, XmTextPosition position);
 * void XmTextFieldShowPosition (Widget widget, XmTextPosition position);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SHOW_POSITION()
{
  LVAL self;
  XmTextPosition position;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  position = Get_XmTextPosition_Argument();
  xllastarg();

  XmTextShowPosition(widget_id, position);
  return (self);
}

#ifdef WINTERP_MOTIF_11
LVAL Xm_Text_Field_Widget_Class_Method_SHOW_POSITION()
{
  LVAL self;
  XmTextPosition position;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  position = Get_XmTextPosition_Argument();
  xllastarg();

  XmTextFieldShowPosition(widget_id, position);
  return (self);
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <text_widget> :SCROLL <n>)
 *
 * Note that this method is only defined on XmText widget, not def'd for
 * XmTextField widget.
 *
 * void XmTextScroll (Widget widget, int n);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SCROLL()
{
  LVAL self;
  Widget widget_id;
  int n;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  n = Get_Int_Argument((long) INT_MIN, (long) INT_MAX);
  xllastarg();

#if (defined(WINTERP_MOTIF_11) || defined(hpux))
  XmTextScroll(widget_id, n);
#else
  /*
   * In HPUX 7.0's Motif 1.0, XmTextScroll() is defined... however
   * other manufacturers libXm (e.g. IBM AIX 3.1) don't export XmTextScroll().
   * In order to make WINTERP compile with the unknown Motif 1.0 libXm's "out there,
   * I've decided to only allow Motif 1.0 access to XmTextScroll() from HPUX.
   * If your Motif 1.0 dist supports this, then make sure the above #if expression becomes true.
   */
  xlfail("XmTextScroll() may not be defined on non-HPUX Motif 1.0.\nIf you know that it exists in your Motif installation, and you need to use\nthis method, you should recompile winterp after modifying src-server/wc_Text.c.");
#endif

  return (self);
}


/*****************************************************************************
 * (send <text_widget> :DISABLE_REDISPLAY <loses-backingstore-p>)
 *
 * void XmTextDisableRedisplay(widget, losesbackingstore)
 * XmTextWidget widget;
 * Boolean losesbackingstore;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_DISABLE_REDISPLAY()
{
  LVAL self, lval_losesbackingstore_p;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  lval_losesbackingstore_p = xlgetarg();
  xllastarg();

#ifndef WINTERP_MOTIF_11
  XmTextDisableRedisplay(widget_id, (lval_losesbackingstore_p) ? TRUE : FALSE);
#else
  _XmTextDisableRedisplay(widget_id, (lval_losesbackingstore_p) ? TRUE : FALSE);
#endif /* WINTERP_MOTIF_11 */

  return (self);
}

/*****************************************************************************
 * (send <text_widget> :ENABLE_REDISPLAY)
 *
 * void XmTextEnableRedisplay(widget)
 * XmTextWidget widget;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_ENABLE_REDISPLAY()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

#ifndef WINTERP_MOTIF_11
  XmTextEnableRedisplay(widget_id);
#else
  _XmTextEnableRedisplay(widget_id);
#endif /* WINTERP_MOTIF_11 */

  return (self);
}

#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <text_widget> :GET_BASELINE)
 *      ====> Returns a FIXNUM value that indicates the x position of
 *            the first baseline in the Text widget.  The calculation
 *            takes into account the margin height, shadow thickness,
 *            highlight thickness, and font ascent of the first font in
 *            the fontlist.  In this calculation the x position of the top
 *            of the widget is 0.
 *
 * int XmTextGetBaseline (Widget widget);
 * int XmTextFieldGetBaseline (Widget widget);
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_BASELINE()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (cvfixnum((FIXTYPE) XmTextGetBaseline(widget_id)));
}

LVAL Xm_Text_Field_Widget_Class_Method_GET_BASELINE()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (cvfixnum((FIXTYPE) XmTextFieldGetBaseline(widget_id)));
}
#endif				/* WINTERP_MOTIF_11 */


/******************************************************************************
 *
 ******************************************************************************/
Wc_Text_Init()
{
  LVAL o_XM_TEXT_WIDGET_CLASS;
#ifdef WINTERP_MOTIF_11
  LVAL o_XM_TEXT_FIELD_WIDGET_CLASS;
#endif				/* WINTERP_MOTIF_11 */
  extern LVAL Wcls_Create_Subclass_Of_WIDGET_CLASS(); /* w_classes.c */
  extern      xladdmsg();	/* from xlobj.c */

  o_XM_TEXT_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_TEXT_WIDGET_CLASS",
					 xmTextWidgetClass);
#ifdef WINTERP_MOTIF_11
  o_XM_TEXT_FIELD_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_TEXT_FIELD_WIDGET_CLASS",
					 xmTextFieldWidgetClass);
#endif				/* WINTERP_MOTIF_11 */

  /*
   * a special :isnew method on XM_TEXT_WIDGET_CLASS  allows for the creation of
   * this widget inside a scrolled window if the submessage keyword :scrolled
   * is given ...
   */
  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Xm_Text_Widget_Class_Method_ISNEW);
#ifdef WINTERP_MOTIF_11
  /*
   * Note that XM_TEXT_FIELD_WIDGET_CLASS does not define its own :ISNEW method.
   * XM_TEXT_FIELD_WIDGET_CLASS uses the default instance initializer in
   * <WIDGET_CLASS>.
   */
#endif				/* WINTERP_MOTIF_11 */

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":PARENT",
           FTAB_Xm_Text_Widget_Class_Method_PARENT);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":ADD_CALLBACK",
           FTAB_Xm_Text_Widget_Class_Method_ADD_CALLBACK);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":ADD_CALLBACK",
           FTAB_Xm_Text_Widget_Class_Method_ADD_CALLBACK);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_CALLBACK",
           FTAB_Xm_Text_Widget_Class_Method_SET_CALLBACK);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":SET_CALLBACK",
           FTAB_Xm_Text_Widget_Class_Method_SET_CALLBACK);
#endif				/* WINTERP_MOTIF_11 */

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_HIGHLIGHT",
           FTAB_Xm_Text_Widget_Class_Method_SET_HIGHLIGHT);
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":SET_HIGHLIGHT",
           FTAB_Xm_Text_Field_Widget_Class_Method_SET_HIGHLIGHT);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_STRING", 
	   FTAB_Xm_Text_Widget_Class_Method_GET_STRING);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":GET_STRING", 
	   FTAB_Xm_Text_Field_Widget_Class_Method_GET_STRING);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_LAST_POSITION",
	   FTAB_Xm_Text_Widget_Class_Method_GET_LAST_POSITION);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":GET_LAST_POSITION",
	   FTAB_Xm_Text_Field_Widget_Class_Method_GET_LAST_POSITION);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_STRING", 
	   FTAB_Xm_Text_Widget_Class_Method_SET_STRING);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":SET_STRING", 
	   FTAB_Xm_Text_Field_Widget_Class_Method_SET_STRING);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":REPLACE",
	   FTAB_Xm_Text_Widget_Class_Method_REPLACE);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":REPLACE",
	   FTAB_Xm_Text_Field_Widget_Class_Method_REPLACE);
#endif				/* WINTERP_MOTIF_11 */

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":INSERT",
           FTAB_Xm_Text_Widget_Class_Method_INSERT);
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":INSERT",
           FTAB_Xm_Text_Field_Widget_Class_Method_INSERT);
#endif				/* WINTERP_MOTIF_11 */

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_ADD_MODE",
           FTAB_Xm_Text_Widget_Class_Method_SET_ADD_MODE);
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":SET_ADD_MODE",
           FTAB_Xm_Text_Field_Widget_Class_Method_SET_ADD_MODE);
#endif				/* WINTERP_MOTIF_11 */

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_ADD_MODE",
           FTAB_Xm_Text_Widget_Class_Method_GET_ADD_MODE);
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":GET_ADD_MODE",
           FTAB_Xm_Text_Field_Widget_Class_Method_GET_ADD_MODE);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_EDITABLE",
	   FTAB_Xm_Text_Widget_Class_Method_GET_EDITABLE);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":GET_EDITABLE",
	   FTAB_Xm_Text_Field_Widget_Class_Method_GET_EDITABLE);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_EDITABLE",
	   FTAB_Xm_Text_Widget_Class_Method_SET_EDITABLE);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":SET_EDITABLE",
	   FTAB_Xm_Text_Field_Widget_Class_Method_SET_EDITABLE);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_MAX_LENGTH",
	   FTAB_Xm_Text_Widget_Class_Method_GET_MAX_LENGTH);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":GET_MAX_LENGTH",
	   FTAB_Xm_Text_Field_Widget_Class_Method_GET_MAX_LENGTH);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_MAX_LENGTH",
	   FTAB_Xm_Text_Widget_Class_Method_SET_MAX_LENGTH);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":SET_MAX_LENGTH",
	   FTAB_Xm_Text_Field_Widget_Class_Method_SET_MAX_LENGTH);
#endif				/* WINTERP_MOTIF_11 */

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":REMOVE",
	   FTAB_Xm_Text_Widget_Class_Method_REMOVE);
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":REMOVE",
	   FTAB_Xm_Text_Field_Widget_Class_Method_REMOVE);
#endif				/* WINTERP_MOTIF_11 */

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":COPY",
	   FTAB_Xm_Text_Widget_Class_Method_COPY);
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":COPY",
	   FTAB_Xm_Text_Field_Widget_Class_Method_COPY);
#endif				/* WINTERP_MOTIF_11 */

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":CUT",
	   FTAB_Xm_Text_Widget_Class_Method_CUT);
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":CUT",
	   FTAB_Xm_Text_Field_Widget_Class_Method_CUT);
#endif				/* WINTERP_MOTIF_11 */

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":PASTE",
	   FTAB_Xm_Text_Widget_Class_Method_PASTE);
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":PASTE",
	   FTAB_Xm_Text_Field_Widget_Class_Method_PASTE);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_SELECTION",
	   FTAB_Xm_Text_Widget_Class_Method_GET_SELECTION);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":GET_SELECTION",
	   FTAB_Xm_Text_Field_Widget_Class_Method_GET_SELECTION);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_SELECTION",
	   FTAB_Xm_Text_Widget_Class_Method_SET_SELECTION);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":SET_SELECTION",
	   FTAB_Xm_Text_Field_Widget_Class_Method_SET_SELECTION);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":CLEAR_SELECTION",
	   FTAB_Xm_Text_Widget_Class_Method_CLEAR_SELECTION);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":CLEAR_SELECTION",
	   FTAB_Xm_Text_Field_Widget_Class_Method_CLEAR_SELECTION);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_TOP_CHARACTER",
	   FTAB_Xm_Text_Widget_Class_Method_GET_TOP_CHARACTER);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_TOP_CHARACTER",
	   FTAB_Xm_Text_Widget_Class_Method_SET_TOP_CHARACTER);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_INSERTION_POSITION",
	   FTAB_Xm_Text_Widget_Class_Method_GET_INSERTION_POSITION);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":GET_INSERTION_POSITION",
	   FTAB_Xm_Text_Field_Widget_Class_Method_GET_INSERTION_POSITION);
#endif				/* WINTERP_MOTIF_11 */

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_CURSOR_POSITION",
	   FTAB_Xm_Text_Widget_Class_Method_GET_CURSOR_POSITION);
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":GET_CURSOR_POSITION",
	   FTAB_Xm_Text_Field_Widget_Class_Method_GET_CURSOR_POSITION);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_INSERTION_POSITION",
	   FTAB_Xm_Text_Widget_Class_Method_SET_INSERTION_POSITION);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":SET_INSERTION_POSITION",
	   FTAB_Xm_Text_Field_Widget_Class_Method_SET_INSERTION_POSITION);
#endif				/* WINTERP_MOTIF_11 */

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_CURSOR_POSITION",
	   FTAB_Xm_Text_Widget_Class_Method_SET_CURSOR_POSITION);
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":SET_CURSOR_POSITION",
	   FTAB_Xm_Text_Field_Widget_Class_Method_SET_CURSOR_POSITION);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_SELECTION_POSITION",
	   FTAB_Xm_Text_Widget_Class_Method_GET_SELECTION_POSITION);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":GET_SELECTION_POSITION",
	   FTAB_Xm_Text_Field_Widget_Class_Method_GET_SELECTION_POSITION);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":XY_TO_POS",
	   FTAB_Xm_Text_Widget_Class_Method_XY_TO_POS);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":XY_TO_POS",
	   FTAB_Xm_Text_Field_Widget_Class_Method_XY_TO_POS);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":POS_TO_XY",
	   FTAB_Xm_Text_Widget_Class_Method_POS_TO_XY);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":POS_TO_XY",
	   FTAB_Xm_Text_Field_Widget_Class_Method_POS_TO_XY);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SHOW_POSITION",
	   FTAB_Xm_Text_Widget_Class_Method_SHOW_POSITION);
#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":SHOW_POSITION",
	   FTAB_Xm_Text_Field_Widget_Class_Method_SHOW_POSITION);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SCROLL",
	   FTAB_Xm_Text_Widget_Class_Method_SCROLL);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":DISABLE_REDISPLAY",
	   FTAB_Xm_Text_Widget_Class_Method_DISABLE_REDISPLAY);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":ENABLE_REDISPLAY",
	   FTAB_Xm_Text_Widget_Class_Method_ENABLE_REDISPLAY);

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_BASELINE",
	   FTAB_Xm_Text_Widget_Class_Method_GET_BASELINE);
  xladdmsg(o_XM_TEXT_FIELD_WIDGET_CLASS, ":GET_BASELINE",
	   FTAB_Xm_Text_Field_Widget_Class_Method_GET_BASELINE);
#endif				/* WINTERP_MOTIF_11 */


  /* 
   * Set up symbols used in :ADD_CALLBACK and :SET_CALLBACK 
   * Lexical_Bindings_For_XmTextVerifyCallbackStruct().
   */
  s_CALLBACK_DOIT = xlenter("CALLBACK_DOIT");
  s_CALLBACK_CUR_INSERT = xlenter("CALLBACK_CUR_INSERT");
  s_CALLBACK_NEW_INSERT = xlenter("CALLBACK_NEW_INSERT");
  s_CALLBACK_START_POS = xlenter("CALLBACK_START_POS");
  s_CALLBACK_END_POS = xlenter("CALLBACK_END_POS");
  s_CALLBACK_TEXT = xlenter("CALLBACK_TEXT");

#ifdef WINTERP_MOTIF_11
  /* 
   * Set up symbols used in Xm_Text_Widget_Class_Method_SET_HIGHLIGHT()
   * and Xm_Text_Field_Widget_Class_Method_SET_HIGHLIGHT()
   */
  s_HIGHLIGHT_NORMAL = xlenter(":HIGHLIGHT_NORMAL");
  s_HIGHLIGHT_SELECTED = xlenter(":HIGHLIGHT_SELECTED");
  s_HIGHLIGHT_SECONDARY_SELECTED = xlenter(":HIGHLIGHT_SECONDARY_SELECTED");
#endif				/* WINTERP_MOTIF_11 */
}
