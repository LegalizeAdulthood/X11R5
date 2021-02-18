/* -*-C-*-
********************************************************************************
*
* File:         w_evnthndlr.c
* RCS:          $Header: w_evnthndlr.c,v 1.6 91/03/14 03:13:36 mayer Exp $
* Description:  Interfaces to Xtoolkit Event Handler Routines
* Author:       Niels Mayer, HPLabs
* Created:      Thu Nov 23 06:11:39 1989
* Modified:     Thu Oct  3 20:40:45 1991 (Niels Mayer) mayer@hplnpm
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
**
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: w_evnthndlr.c,v 1.6 91/03/14 03:13:36 mayer Exp $";

#include <stdio.h>
#include <Xm/Xm.h>		/* Xm/Xm.h only needed for "winterp.h"*/
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"

static LVAL k_RAW, k_NONMASKABLE,
  s_EVHANDLER_WIDGET, s_EVHANDLER_XEVENT, s_EVHANDLER_BUTTON, s_EVHANDLER_TIME,
  s_EVHANDLER;

#define RAW_OPTION         (1L<<0)
#define NONMASKABLE_OPTION (1L<<1)

extern Widget Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(); /* w_classes.c */


/******************************************************************************
 * This is an XtEventHandler procedure that is called by the various add-
 * event-handler methods in this file. It serves to call the lisp evaluator
 * on the lexical closure that was created by the add-event-handler-methods.
 * It will bind the symbols EVHANDLER_WIDGET EVHANDLER_XEVENT to
 * the widget and xevent that caused the event handler to fire.
******************************************************************************/
static void Winterp_Event_Handler_Proc(widget_id, client_data, event)
     Widget    widget_id;
     XtPointer client_data;
     XEvent*   event;
{
  extern LVAL xlenv, xlfenv;
  LVAL        oldenv, oldfenv, l_evalforms, s_name;
  CONTEXT     cntxt;
  LVAL        bindings_list, s_bindname;
  LVAL        c_evhandler = get_evhandler_closure((LVAL) client_data);

  /*
   * Most of this procedure looks alot like xleval.c:evfun(), which is what
   * the evaluator calls when a functional form is to be evaluated. The
   * main difference is that instead of calling xlabind() to bind the
   * formal parameter symbols of a function to their values in the new
   * lexical environment frame returned by xlframe(getenv(fun)), we 
   * bind symbols on bindings_list...
   */
  
  /* protect some pointers */
  xlstkcheck(3);
  xlsave(oldenv);
  xlsave(oldfenv);
  xlsave(l_evalforms);
  
  /* create a new environment frame */
  oldenv = xlenv;
  oldfenv = xlfenv;
  xlenv = xlframe(getenvt(c_evhandler)); /* note: changed getenv()-->getenvt() due to name conflict with stdlib.h:getenv() */
  xlfenv = getfenv(c_evhandler);

  /* bind  local variables specified durng call to :SET/:ADD_EVENT_HANDLER */
  for (bindings_list = getargs(c_evhandler);
       consp(bindings_list); bindings_list = cdr(bindings_list)) {
    s_bindname = car(bindings_list);
    if (s_bindname == s_EVHANDLER_WIDGET) {
      xlpbind(s_bindname, get_evhandler_widget((LVAL) client_data), xlenv);
    }
    else if (s_bindname == s_EVHANDLER_BUTTON) {
      xlpbind(s_bindname, (event) ? cvfixnum((FIXTYPE) ((XButtonEvent*) event)->button) : NIL, xlenv);
    }
    else if (s_bindname == s_EVHANDLER_TIME) {
      xlpbind(s_bindname, (event) ? cvfixnum((FIXTYPE) ((XButtonEvent*) event)->time) : NIL, xlenv);
    }
    else if (s_bindname == s_EVHANDLER_XEVENT) {
      xlpbind(s_bindname, (event) ? cv_xevent(event) : NIL, xlenv);
    }
    else {
      extern char temptext[]; /* from winterp.c */
      sprintf(temptext,
	      "Unknown binding name in Event Handler. Valid symbols are [%s %s %s %s].",
	      (char*) getstring(getpname(s_EVHANDLER_WIDGET)),
	      (char*) getstring(getpname(s_EVHANDLER_BUTTON)),
	      (char*) getstring(getpname(s_EVHANDLER_TIME)),
	      (char*) getstring(getpname(s_EVHANDLER_XEVENT)));
      xlerror(temptext, s_bindname);
    }
  }

  /* setup the implicit block */
  if (s_name = getname(c_evhandler))
    xlbegin(&cntxt, CF_RETURN, s_name);
  
  /* execute the block */
  if (s_name && setjmp(cntxt.c_jmpbuf))
    { }
  else
    for (l_evalforms = getbody(c_evhandler); consp(l_evalforms); 
	 l_evalforms = cdr(l_evalforms))
      xleval(car(l_evalforms));
  
  /* finish the block context */
  if (s_name)
    xlend(&cntxt);
  
  /* restore the environment */
  xlenv = oldenv;
  xlfenv = oldfenv;
  
  /* restore the stack */
  xlpopn(3);
}


/******************************************************************************
 * (send <widget> :ADD_EVENT_HANDLER [:RAW] [:NONMASKABLE] <event_mask>
 *                                   <event_data_bindings_list> <code>)
 * (send <widget> :SET_EVENT_HANDLER [:RAW] [:NONMASKABLE] <event_mask>
 *                                   <event_data_bindings_list> <code>)
 * ==> returns an <EVHANDLEROBJ> which identifies the handler.
 *
 * Optional keyword :RAW indicates that XtAddRawEventHandler() will
 * be invoked so as to not affect the widget's input mask nor for
 * it to select for events. Otherwise XtAddEventHandler() will get 
 * called.
 *
 * Optional keyword :NONMASKABLE indicates that the handler should
 * be called for nonmaskable events.
 * 
 * <event_mask> is a FIXNUM which is the value of the an event mask.
 * event masks may be combined by doing a bitwise or via the xlisp
 * 'logior' function. The following event mask constants have been
 * defined: NO_EVENT_MASK, KEY_PRESS_MASK, KEY_RELEASE_MASK,
 * BUTTON_PRESS_MASK, BUTTON_RELEASE_MASK, ENTER_WINDOW_MASK,
 * LEAVE_WINDOW_MASK, POINTER_MOTION_MASK, POINTER_MOTIONHINT_MASK,
 * BUTTON1_MOTION_MASK, BUTTON2_MOTION_MASK, BUTTON3_MOTION_MASK,
 * BUTTON4_MOTION_MASK, BUTTON5_MOTION_MASK, BUTTON_MOTION_MASK,
 * KEYMAP_STATE_MASK, EXPOSURE_MASK,  VISIBILITY_CHANGE_MASK
 * STRUCTURE_NOTIFY_MASK, RESIZE_REDIRECT_MASK, SUBSTRUCTURE_NOTIFY_MASK
 * SUBSTRUCTURE_REDIRECT_MASK, FOCUS_CHANGE_MASK, PROPERTY_CHANGE_MASK
 * COLORMAP_CHANGE_MASK, OWNER_GRAB_BUTTON_MASK.
 *
 * <event_data_bindings_list> is a list of symbols that get
 * bound to data specific to the action that caused the event
 * handler to fire. These symbols get lexically bound during
 * the execution of the event-handler <code>.
 * Valid symbols are:
 * EVHANDLER_WIDGET -- bound to the widgetobj of the event
 * EVHANDLER_XEVENT -- bound to the XEvent that fired handler.
 * EVHANDLER_BUTTON -- fixnum -- the button or keycode
 * EVHANDLER_TIME   -- fixnum -- the timestamp.
 *
 * <code> is a list of lisp-forms to be evaluated when the eventhandler
 * fires.
 *
 * The eventhandler may be removed by giving the <EVHANDLEROBJ> to
 * procedure REMOVE_EVENT_HANDLER.
 *
 * Note that the :SET_EVENT_HANDLER variant does exactly the same
 * thing as :ADD_EVENT_HANDLER except that it ensure that only one
 * event handler with the given <event_mask>, :RAW and :NONMASKABLE
 * specifications exist on <widget>. It will remove all other matching
 * event handlers in order to set the current event handler. THis
 * function is useful for making interactive changes to an event
 * handler without having to remember to remove the previous
 * handler. Note that :SET_EVENT_HANDLER is slower, so it should 
 * not be used in cases where speed is important.
 *
 * NOTE: the EVHANDLER_TIME and EVHANDLER_BUTTON
 * binding values may return gibberish if the eventhandler returned
 * a event->type that doesn't define those fields. THis is
 * currently just a kludge for motif functions that require
 * the event->button info (popup menus) or event->time info
 * (XmClipboard*). Hopefully I'll come up with something better.
 *
 * void XtAddEventHandler(widget, eventMask, other, proc, closure)
 *     Widget	       widget;
 *     EventMask       eventMask;
 *     Boolean         other;
 *     XtEventHandler  proc;
 *     XtPointer       closure;
 * 
 * void XtAddRawEventHandler(widget, eventMask, other, proc, closure)
 *     Widget	       widget;
 *     EventMask       eventMask;
 *     Boolean         other;
 *     XtEventHandler  proc;
 *     XtPointer       closure;
 ******************************************************************************/
LVAL Widget_Class_Meta_Method_Add_Event_Handler();
LVAL Widget_Class_Method_ADD_EVENT_HANDLER()
{
  return (Widget_Class_Meta_Method_Add_Event_Handler(FALSE));
}
LVAL Widget_Class_Method_SET_EVENT_HANDLER()
{
  return (Widget_Class_Meta_Method_Add_Event_Handler(TRUE));
}

LVAL Widget_Class_Meta_Method_Add_Event_Handler(one_evhandler_per_mask_p)
     Boolean one_evhandler_per_mask_p;
{
  extern LVAL s_lambda, xlenv, xlfenv;
  Boolean raw_p, nonmaskable_p;
  EventMask event_mask;
  Widget widget_id;
  LVAL o_self, l_fargs, l_code, evhandler_obj;

  /* get <widget_instance> */
  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&o_self);

  /* get optional :RAW or :NONMASKABLE keyword args */
  raw_p = nonmaskable_p = FALSE;
  if (moreargs() && ((*xlargv == k_RAW))) {
    nextarg();
    raw_p = TRUE;
  }
  if (moreargs() && ((*xlargv == k_NONMASKABLE))) {
    nextarg();
    nonmaskable_p = TRUE;
  }
  
  /* get required <event_mask> arg, a fixnum */
  event_mask = (EventMask) getfixnum(xlgafixnum());

  /* get <event_data_bindings_list> -- args to be bound at call time.
     NOTE: may want to check that these args are valid... No biggie
     though invalid names will get get caught at runtime,
     when the eventhandler fires. */
  l_fargs = xlgalist();		
  
  /* get <code> */
  l_code = xlgalist();
  xllastarg();

  /* 
   * if this procedure is being called from the :set_event_handler method 
   * (indicated by one_evhandler_per_mask_p == TRUE), 
   * then remove all eventhandlers on <widget> matching <event_mask>
   * <raw_p> and <nonmaskable_p>.
   */
  if (one_evhandler_per_mask_p) {
    extern LVAL v_savedobjs;	
    int  i = Wso_Hash(o_self);
    LVAL l_hbucket = getelement(v_savedobjs, i); /* a list of objects, including all evhandler-objs on this widget */
    LVAL obj;
    LVAL l_prev = NIL;
    while (l_hbucket) {		/* while there are elements in the hashbucket */
      obj = car(l_hbucket);	/* obj points to cur elt */
      if (evhandlerobj_p(obj)
	  && (get_evhandler_mask(obj) == event_mask)
	  && (((get_evhandler_options(obj) & RAW_OPTION) ? TRUE : FALSE) == raw_p)
	  && (((get_evhandler_options(obj) & NONMASKABLE_OPTION) ? TRUE : FALSE) == nonmaskable_p)
	  && (get_evhandler_widget(obj) == o_self)) {

	if (get_evhandler_options(obj) & RAW_OPTION)
	  XtRemoveRawEventHandler(widget_id, event_mask,
				  ((get_evhandler_options(obj) & NONMASKABLE_OPTION) ? TRUE : FALSE),
				  Winterp_Event_Handler_Proc, (XtPointer) obj);
	else
	  XtRemoveEventHandler(widget_id, event_mask,
			       ((get_evhandler_options(obj) & NONMASKABLE_OPTION) ? TRUE : FALSE),
			       Winterp_Event_Handler_Proc, (XtPointer) obj);

	l_hbucket = cdr(l_hbucket); /* l_hbucket now points to next elt or NIL */
	if (!l_prev)
	  setelement(v_savedobjs, i, l_hbucket); /* remove first, head is now next elt */
	else
	  rplacd(l_prev, l_hbucket); /* remove cur, point previous to next */
      }
      else {
	l_prev = l_hbucket;
	l_hbucket = cdr(l_hbucket);
      }
    }
  }

  /* 
   * create the client_data to be sent to (*XtEventHandler)()
   * That procedure takes the client_data and extracts the widget-object,
   * and the closure, and uses these to execute the event handler's code.
   */
  xlsave1(evhandler_obj);	/* protect some pointers */
  evhandler_obj = new_evhandlerobj();
  set_evhandler_widget(evhandler_obj, o_self);
  set_evhandler_mask(evhandler_obj, event_mask);
  {
    long options = 0L;
    if (raw_p)
      options |= RAW_OPTION;	/* this bit tells REMOVE_EVENT_HANDLER which removeproc to call */
    if (nonmaskable_p)
      options |= NONMASKABLE_OPTION; /* we need this bit for REMOVE_EVENT_HANDLER */
    set_evhandler_options(evhandler_obj, options);
  }
  set_evhandler_closure(evhandler_obj, 
			xlclose(s_EVHANDLER, s_lambda, l_fargs, l_code, xlenv, xlfenv));
  
  if (raw_p)
    XtAddRawEventHandler(widget_id, event_mask, nonmaskable_p, 
			 Winterp_Event_Handler_Proc, (XtPointer) evhandler_obj);
  else
    XtAddEventHandler(widget_id, event_mask, nonmaskable_p, 
		      Winterp_Event_Handler_Proc, (XtPointer) evhandler_obj);

  /*
   * Enter the evhandler_obj in v_savedobjs, so that it gets marked.
   * This way, it won't be garbage collected while the eventhandler is
   * active. REMOVE_EVENT_HANDLER, and :destroy on the widget will
   * end up removing the evhandlerobj from v_savedobjs,  which will
   * permit it to be garbage collected.
   */
  { 
    int  i = Wso_Hash(o_self);
    LVAL l_hbucket;
    extern LVAL v_savedobjs;
    
    xlsave1(l_hbucket);
    l_hbucket = cons(evhandler_obj, getelement(v_savedobjs, i));
    setelement(v_savedobjs, i, l_hbucket);
    xlpop(/*l_hbucket*/);
  }

  xlpop(/*evhandler_obj*/);
  return (evhandler_obj);
}

/******************************************************************************
 * (REMOVE_EVENT_HANDLER <EVHANDEROBJ>)
 * returns true.
 *
 * This procedure removes the eventhander corresponding to the 
 * <EVHANDLEROBJ> returned by method :ADD_EVENT_HANDLER
 *
 * void XtRemoveEventHandler(widget, eventMask, other, proc, closure)
 *     Widget	      widget;
 *     EventMask      eventMask;
 *     Boolean	      other;
 *     XtEventHandler proc;
 *     XtPointer      closure;
 *
 * void XtRemoveRawEventHandler(widget, eventMask, other, proc, closure)
 *     Widget	        widget;
 *     EventMask        eventMask;
 *     Boolean	        other;
 *     XtEventHandler   proc;
 *     XtPointer	closure;
 ******************************************************************************/
LVAL Weh_Prim_REMOVE_EVENT_HANDLER()
{
  LVAL evhandler_obj;
  LVAL o_widget;
  Widget widget_id;
  Boolean raw_p, nonmaskable_p;
  extern LVAL true;

  evhandler_obj = xlga_evhandlerobj();
  xllastarg();

  /* check if this evhandler hasn't already been removed */
  if ((o_widget = get_evhandler_widget(evhandler_obj)) == NIL)
    xlerror("EventHandler associated with <evhandlerobj> has already been removed.", evhandler_obj);
  
  /* mark the evhandler_obj as being removed */
  set_evhandler_widget(evhandler_obj, NIL);
  
  {
    long options;
    options = get_evhandler_options(evhandler_obj);
    raw_p = (options & RAW_OPTION) ? TRUE : FALSE;
    nonmaskable_p = (options & NONMASKABLE_OPTION) ? TRUE : FALSE;
  }

  if (!(widget_id = get_widgetobj_widgetID(o_widget)))
    xlerror("widget object not properly initialized by :isnew.", o_widget);
  
  if (raw_p)
    XtRemoveRawEventHandler(widget_id, get_evhandler_mask(evhandler_obj),
			    nonmaskable_p, Winterp_Event_Handler_Proc,
			    (XtPointer) evhandler_obj);
  else
    XtRemoveEventHandler(widget_id, get_evhandler_mask(evhandler_obj),
			 nonmaskable_p, Winterp_Event_Handler_Proc,
			 (XtPointer) evhandler_obj);
			    
  /* remove <evhandler_obj> from v_savedobjs allowing it to be garbage collected */
  {
    extern LVAL v_savedobjs;
    int i = Wso_Hash(o_widget); /* note that we hash all evhandlers on the same widget to the same hashbucket */
    LVAL l_hbucket = getelement(v_savedobjs, i);
    LVAL l_prev = NIL;

    while (l_hbucket && (car(l_hbucket) != evhandler_obj)) {
      l_prev = l_hbucket;
      l_hbucket = cdr(l_hbucket);
    }
    if (!l_hbucket)
      xlerror("Internal error in REMOVE_EVENT_HANDLER -- couldn't remove <evhandlerobj> from v_savedobjs. Hash error?",
	      evhandler_obj);
    if (!l_prev)		/* first elt matched */
      setelement(v_savedobjs, i, cdr(l_hbucket));
    else
      rplacd(l_prev, cdr(l_hbucket));
  }
  
  return (true);
}


/******************************************************************************
 * (send <widget> :BUILD_EVENT_MASK)
 * ==> this returns as a FIXNUM the event mask representing the logical OR of
 * all event masks for event handlers registered on <widget>. This includes
 * masks set by XtAddEventHandler(), all event translations & accelerators.
 * 
 * EventMask XtBuildEventMask(widget)
 *     Widget widget;
 ******************************************************************************/
LVAL Widget_Class_Method_BUILD_EVENT_MASK()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();
  
  return (cvfixnum((FIXTYPE) XtBuildEventMask(widget_id)));
}


/******************************************************************************
 *
 ******************************************************************************/
LVAL Weh_Init()
{
  LVAL s_NO_EVENT_MASK = xlenter("NO_EVENT_MASK");
  LVAL s_KEY_PRESS_MASK = xlenter("KEY_PRESS_MASK");
  LVAL s_KEY_RELEASE_MASK = xlenter("KEY_RELEASE_MASK");
  LVAL s_BUTTON_PRESS_MASK = xlenter("BUTTON_PRESS_MASK");
  LVAL s_BUTTON_RELEASE_MASK = xlenter("BUTTON_RELEASE_MASK");
  LVAL s_ENTER_WINDOW_MASK = xlenter("ENTER_WINDOW_MASK");
  LVAL s_LEAVE_WINDOW_MASK = xlenter("LEAVE_WINDOW_MASK");
  LVAL s_POINTER_MOTION_MASK = xlenter("POINTER_MOTION_MASK");
  LVAL s_POINTER_MOTIONHINT_MASK = xlenter("POINTER_MOTIONHINT_MASK");
  LVAL s_BUTTON1_MOTION_MASK = xlenter("BUTTON1_MOTION_MASK");
  LVAL s_BUTTON2_MOTION_MASK = xlenter("BUTTON2_MOTION_MASK");
  LVAL s_BUTTON3_MOTION_MASK = xlenter("BUTTON3_MOTION_MASK");
  LVAL s_BUTTON4_MOTION_MASK = xlenter("BUTTON4_MOTION_MASK");
  LVAL s_BUTTON5_MOTION_MASK = xlenter("BUTTON5_MOTION_MASK");
  LVAL s_BUTTON_MOTION_MASK = xlenter("BUTTON_MOTION_MASK");
  LVAL s_KEYMAP_STATE_MASK = xlenter("KEYMAP_STATE_MASK");
  LVAL s_EXPOSURE_MASK = xlenter("EXPOSURE_MASK");
  LVAL s_VISIBILITY_CHANGE_MASK = xlenter("VISIBILITY_CHANGE_MASK");
  LVAL s_STRUCTURE_NOTIFY_MASK = xlenter("STRUCTURE_NOTIFY_MASK");
  LVAL s_RESIZE_REDIRECT_MASK = xlenter("RESIZE_REDIRECT_MASK");
  LVAL s_SUBSTRUCTURE_NOTIFY_MASK = xlenter("SUBSTRUCTURE_NOTIFY_MASK");
  LVAL s_SUBSTRUCTURE_REDIRECT_MASK = xlenter("SUBSTRUCTURE_REDIRECT_MASK");
  LVAL s_FOCUS_CHANGE_MASK = xlenter("FOCUS_CHANGE_MASK");
  LVAL s_PROPERTY_CHANGE_MASK = xlenter("PROPERTY_CHANGE_MASK");
  LVAL s_COLORMAP_CHANGE_MASK = xlenter("COLORMAP_CHANGE_MASK");
  LVAL s_OWNER_GRAB_BUTTON_MASK = xlenter("OWNER_GRAB_BUTTON_MASK");

  setvalue(s_NO_EVENT_MASK, cvfixnum((FIXTYPE) NoEventMask));
  setvalue(s_KEY_PRESS_MASK, cvfixnum((FIXTYPE) KeyPressMask));
  setvalue(s_KEY_RELEASE_MASK, cvfixnum((FIXTYPE) KeyReleaseMask));
  setvalue(s_BUTTON_PRESS_MASK, cvfixnum((FIXTYPE) ButtonPressMask));
  setvalue(s_BUTTON_RELEASE_MASK, cvfixnum((FIXTYPE) ButtonReleaseMask));
  setvalue(s_ENTER_WINDOW_MASK, cvfixnum((FIXTYPE) EnterWindowMask));
  setvalue(s_LEAVE_WINDOW_MASK, cvfixnum((FIXTYPE) LeaveWindowMask));
  setvalue(s_POINTER_MOTION_MASK, cvfixnum((FIXTYPE) PointerMotionMask));
  setvalue(s_POINTER_MOTIONHINT_MASK, cvfixnum((FIXTYPE) PointerMotionHintMask));
  setvalue(s_BUTTON1_MOTION_MASK, cvfixnum((FIXTYPE) Button1MotionMask));
  setvalue(s_BUTTON2_MOTION_MASK, cvfixnum((FIXTYPE) Button2MotionMask));
  setvalue(s_BUTTON3_MOTION_MASK, cvfixnum((FIXTYPE) Button3MotionMask));
  setvalue(s_BUTTON4_MOTION_MASK, cvfixnum((FIXTYPE) Button4MotionMask));
  setvalue(s_BUTTON5_MOTION_MASK, cvfixnum((FIXTYPE) Button5MotionMask));
  setvalue(s_BUTTON_MOTION_MASK, cvfixnum((FIXTYPE) ButtonMotionMask));
  setvalue(s_KEYMAP_STATE_MASK, cvfixnum((FIXTYPE) KeymapStateMask));
  setvalue(s_EXPOSURE_MASK, cvfixnum((FIXTYPE) ExposureMask));
  setvalue(s_VISIBILITY_CHANGE_MASK, cvfixnum((FIXTYPE) VisibilityChangeMask));
  setvalue(s_STRUCTURE_NOTIFY_MASK, cvfixnum((FIXTYPE) StructureNotifyMask));
  setvalue(s_RESIZE_REDIRECT_MASK, cvfixnum((FIXTYPE) ResizeRedirectMask));
  setvalue(s_SUBSTRUCTURE_NOTIFY_MASK, cvfixnum((FIXTYPE) SubstructureNotifyMask));
  setvalue(s_SUBSTRUCTURE_REDIRECT_MASK, cvfixnum((FIXTYPE) SubstructureRedirectMask));
  setvalue(s_FOCUS_CHANGE_MASK, cvfixnum((FIXTYPE) FocusChangeMask));
  setvalue(s_PROPERTY_CHANGE_MASK, cvfixnum((FIXTYPE) PropertyChangeMask));
  setvalue(s_COLORMAP_CHANGE_MASK, cvfixnum((FIXTYPE) ColormapChangeMask));
  setvalue(s_OWNER_GRAB_BUTTON_MASK, cvfixnum((FIXTYPE) OwnerGrabButtonMask));

  k_RAW = xlenter(":RAW");
  k_NONMASKABLE = xlenter(":NONMASKABLE");
  s_EVHANDLER_WIDGET = xlenter("EVHANDLER_WIDGET");
  s_EVHANDLER_XEVENT = xlenter("EVHANDLER_XEVENT");
  s_EVHANDLER_BUTTON = xlenter("EVHANDLER_BUTTON");
  s_EVHANDLER_TIME   = xlenter("EVHANDLER_TIME");
  s_EVHANDLER = xlenter("EVHANDLER");
}  
