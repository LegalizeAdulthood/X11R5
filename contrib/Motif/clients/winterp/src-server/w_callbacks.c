/* -*-C-*-
********************************************************************************
*
* File:         w_callbacks.c
* RCS:          $Header: w_callbacks.c,v 1.9 91/03/14 03:13:25 mayer Exp $
* Description:  
* Author:       Niels Mayer, HPLabs
* Created:      Sat Aug 26 07:44:17 1989
* Modified:     Thu Oct  3 21:32:41 1991 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: w_callbacks.c,v 1.9 91/03/14 03:13:25 mayer Exp $";

#include <stdio.h>
#include <Xm/Xm.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"

#ifdef HP_GRAPH_WIDGET
#include <Xm/Graph.h>		/* needed for graph widget's added callback reasins XmCR_* */
#endif				/* HP_GRAPH_WIDGET */

/* Symbols init'd in Wcb_Init() */
LVAL s_CALLBACK_WIDGET, s_CALLBACK_REASON, s_CALLBACK_XEVENT, s_CALLBACK_WINDOW, s_CALLBACK_VALUE, s_CALLBACK_LENGTH;
#ifdef WINTERP_MOTIF_11
LVAL s_CALLBACK_CLICK_COUNT;
#endif				/* WINTERP_MOTIF_11 */


/* 
 * To prevent garbage collection of callback-objects, we store the objects in
 * the hashtable w_savedobjs.c:v_savedobjs after hashing on the address of the
 * widget-object LVAL. Since callbacks occur on a per-widget basis, we use the
 * widget as the hash-key and then search through the hashbucket looking for
 * LVALs that are callback-objects, have the appropriate widget-object, and
 * have the right callback name.
 * 
 * Garbage collection of lisp-objects will occur to any object not referenced
 * as symbol values, inside lexical environments saved as functional closures,
 * as elements of the evaluation or argument stack. In the case of callbacks,
 * the callback closure, the callback's lexical environment, and the callback
 * widget are implicitly referenced inside Motif/Xtoolkit code. As long as the
 * widget on which these callbacks are placed still exists, we must prevent
 * callback-objects from being garbage collected. 
 * 
 * This is done by:
 *    1) storing them in v_savedobjs when the callback is added
 *       (note that v_savedobjs is a hashtable hashed by the LVAL's pointer,
 *        for callbacks, we store all callbacks on a particular widget in the
 *        hashbucket associated with hashing on that widget-object. this makes
 *        it easier to implement :set_callback. and :remove_all_callbacks.)
 *    2) removing them when the callback is removed
 *       (as in methods :set_callback :remove_all_callbacks and function remove_callback)
 *    3) removing them when the widget is destroyed.
 *       (requires a destroy callback to be added onto each widget)
 */


/******************************************************************************
 * 
 ******************************************************************************/
void Wcb_Meta_Callbackproc(client_data, call_data, bind_call_data_values_proc, set_call_data_values_proc)
     XtPointer client_data;	/* XLTYPE_CALLBACKOBJ */
     XtPointer call_data;	/* a pointer to a structure that is accessed via (*bind_call_data_values_proc)() */
     void      (*bind_call_data_values_proc)(/* LVAL bindings_list; LVAL lexical_env; XtPointer call_data; LVAL o_widget */);
     void      (*set_call_data_values_proc)(/* LVAL bindings_list; LVAL lexical_env; XtPointer call_data */);
{
  extern LVAL          xlenv, xlfenv;
  LVAL                 oldenv, oldfenv, l_evalforms, s_name;
  CONTEXT              cntxt;
  LVAL                 c_callback = get_callback_closure((LVAL) client_data);

  /*
   * Most of this procedure looks alot like xleval.c:evfun(), which is what
   * the evaluator calls when a functional form is to be evaluated. The
   * main difference is that instead of calling xlabind() to bind the
   * formal parameter symbols of a function to their values in the new
   * lexical environment frame returned by xlframe(getenv(fun)), we look
   * at the formal args from the callback's closure and bind these to the
   * appropriate values.
   */
  
  /* protect some pointers */
  xlstkcheck(3);
  xlsave(oldenv);
  xlsave(oldfenv);
  xlsave(l_evalforms);
  
  /* create a new environment frame */
  oldenv = xlenv;
  oldfenv = xlfenv;
  xlenv = xlframe(getenvt(c_callback));	/* note: changed getenv()-->getenvt() due to name conflict with stdlib.h:getenv() */
  xlfenv = getfenv(c_callback);
  
  /* get the list of bindings and bind (locally) the symbols to values retrieved from call_data -- see also xlabind() */
  (*bind_call_data_values_proc)(getargs(c_callback), /* a list of symbols to which values from call_data struct are locally bound */
				xlenv, /* the lexical environment in which the bindings are made  */
				call_data, /* a pointer to a Xm*CallbackStruct (the structure used depends on the widget-class of the widget containing the callback */
				get_callback_widget((LVAL) client_data)); /* the WIDGETOBJ on which the callback occured. */
  
  /* setup the implicit block */
  if (s_name = getname(c_callback))
    xlbegin(&cntxt, CF_RETURN, s_name);
  
  /* execute the block */
  if (s_name && setjmp(cntxt.c_jmpbuf))
    { }
  else
    for (l_evalforms = getbody(c_callback); consp(l_evalforms); 
	 l_evalforms = cdr(l_evalforms))
      xleval(car(l_evalforms));

  /* if this is a callback in which elements of the call_data structure must
     be set before the callback returns (as in the XmText or the XmGraph widgets),
     then set_call_data_values_proc is a pointer to the function that does this
     dirty deed. */
  if (set_call_data_values_proc)
    (*set_call_data_values_proc)(getargs(c_callback), /* a list of symbols to which values from call_data struct are locally bound */
				 xlenv, /* the lexical environment in which the bindings are made  */
				 call_data); /* a pointer to a Xm*CallbackStruct (the structure used depends on the widget-class of the widget containing the callback */
  
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
 * Are we having fun yet?
 ******************************************************************************/
static LVAL s_CR_NONE, s_CR_HELP, s_CR_VALUE_CHANGED, s_CR_INCREMENT, /* Symbols init'd in Wcb_Init() */
  s_CR_DECREMENT, s_CR_PAGE_INCREMENT, s_CR_PAGE_DECREMENT, s_CR_TO_TOP,
  s_CR_TO_BOTTOM, s_CR_DRAG, s_CR_ACTIVATE, s_CR_ARM, s_CR_DISARM, s_CR_MAP,
  s_CR_UNMAP, s_CR_FOCUS, s_CR_LOSING_FOCUS, s_CR_MODIFYING_TEXT_VALUE,
  s_CR_MOVING_INSERT_CURSOR, s_CR_EXECUTE, s_CR_SINGLE_SELECT,
  s_CR_MULTIPLE_SELECT, s_CR_EXTENDED_SELECT, s_CR_BROWSE_SELECT,
  s_CR_DEFAULT_ACTION, s_CR_CLIPBOARD_DATA_REQUEST, s_CR_CLIPBOARD_DATA_DELETE,
  s_CR_CASCADING, s_CR_OK, s_CR_CANCEL, s_CR_APPLY, s_CR_NO_MATCH,
  s_CR_COMMAND_ENTERED, s_CR_COMMAND_CHANGED, s_CR_EXPOSE, s_CR_RESIZE, s_CR_INPUT;
#ifdef WINTERP_MOTIF_11
static LVAL s_CR_GAIN_PRIMARY, s_CR_LOSE_PRIMARY;
#ifdef XmCR_CREATE		/* added in Motif 1.1.3 */
static LVAL s_XmCR_CREATE;
#endif /* XmCR_CREATE */
#ifdef XmCR_PROTOCOLS		/* added in Motif 1.1.3 */
static LVAL s_XmCR_PROTOCOLS;
#endif /* XmCR_PROTOCOLS */
#endif				/* WINTERP_MOTIF_11 */

#ifdef HP_GRAPH_WIDGET
static LVAL s_CR_NEW_ARC, s_CR_NEW_NODE, s_CR_NODE_MOVED, s_CR_ARC_MOVED, s_CR_SUBGRAPH_MOVED, /* Symbols init'd in Wcb_Init() */
  s_CR_ARC_EDITED, s_CR_SELECT_NODE, s_CR_SELECT_ARC, s_CR_SELECT_SUBGRAPH, s_CR_DELETE_NODE,
  s_CR_DELETE_ARC, s_CR_SELECT, s_CR_RELEASE, s_CR_NODE_DOUBLE_CLICK, s_CR_ARC_DOUBLE_CLICK,
  s_CR_DOUBLE_CLICK, s_CR_DESELECT_ARC, s_CR_DESELECT_NODE, s_CR_DESELECT;
#endif				/* HP_GRAPH_WIDGET */

LVAL Wcb_Get_Callback_Reason_Symbol(cb_reason)
     int cb_reason;
{
  switch (cb_reason) {
  case XmCR_NONE:
    return (s_CR_NONE);
  case XmCR_HELP:
    return (s_CR_HELP);
  case XmCR_VALUE_CHANGED:
    return (s_CR_VALUE_CHANGED);
  case XmCR_INCREMENT:
    return (s_CR_INCREMENT);
  case XmCR_DECREMENT:
    return (s_CR_DECREMENT);
  case XmCR_PAGE_INCREMENT:
    return (s_CR_PAGE_INCREMENT);
  case XmCR_PAGE_DECREMENT:
    return (s_CR_PAGE_DECREMENT);
  case XmCR_TO_TOP:
    return (s_CR_TO_TOP);
  case XmCR_TO_BOTTOM:
    return (s_CR_TO_BOTTOM);
  case XmCR_DRAG:
    return (s_CR_DRAG);
  case XmCR_ACTIVATE:
    return (s_CR_ACTIVATE);
  case XmCR_ARM:
    return (s_CR_ARM);
  case XmCR_DISARM:
    return (s_CR_DISARM);
  case XmCR_MAP:
    return (s_CR_MAP);
  case XmCR_UNMAP:
    return (s_CR_UNMAP);
  case XmCR_FOCUS:
    return (s_CR_FOCUS);
  case XmCR_LOSING_FOCUS:
    return (s_CR_LOSING_FOCUS);
  case XmCR_MODIFYING_TEXT_VALUE:
    return (s_CR_MODIFYING_TEXT_VALUE);
  case XmCR_MOVING_INSERT_CURSOR:
    return (s_CR_MOVING_INSERT_CURSOR);
  case XmCR_EXECUTE:
    return (s_CR_EXECUTE);
  case XmCR_SINGLE_SELECT:
    return (s_CR_SINGLE_SELECT);
  case XmCR_MULTIPLE_SELECT:
    return (s_CR_MULTIPLE_SELECT);
  case XmCR_EXTENDED_SELECT:
    return (s_CR_EXTENDED_SELECT);
  case XmCR_BROWSE_SELECT:
    return (s_CR_BROWSE_SELECT);
  case XmCR_DEFAULT_ACTION:
    return (s_CR_DEFAULT_ACTION);
  case XmCR_CLIPBOARD_DATA_REQUEST:
    return (s_CR_CLIPBOARD_DATA_REQUEST);
  case XmCR_CLIPBOARD_DATA_DELETE:
    return (s_CR_CLIPBOARD_DATA_DELETE);
  case XmCR_CASCADING:
    return (s_CR_CASCADING);
  case XmCR_OK:
    return (s_CR_OK);
  case XmCR_CANCEL:
    return (s_CR_CANCEL);
  case XmCR_APPLY:
    return (s_CR_APPLY);
  case XmCR_NO_MATCH:
    return (s_CR_NO_MATCH);
  case XmCR_COMMAND_ENTERED:
    return (s_CR_COMMAND_ENTERED);
  case XmCR_COMMAND_CHANGED:
    return (s_CR_COMMAND_CHANGED);
  case XmCR_EXPOSE:
    return (s_CR_EXPOSE);
  case XmCR_RESIZE:
    return (s_CR_RESIZE);
  case XmCR_INPUT:
    return (s_CR_INPUT);
#ifdef WINTERP_MOTIF_11
  case XmCR_GAIN_PRIMARY:
    return (s_CR_GAIN_PRIMARY);
  case XmCR_LOSE_PRIMARY:
    return (s_CR_LOSE_PRIMARY);
#ifdef XmCR_CREATE		/* added in Motif 1.1.3 */
  case XmCR_CREATE:
    return (s_XmCR_CREATE);
#endif /* XmCR_CREATE */
#ifdef XmCR_PROTOCOLS		/* added in Motif 1.1.3 */
  case XmCR_PROTOCOLS:
    return (s_XmCR_PROTOCOLS);
#endif /* XmCR_PROTOCOLS */
#endif /* WINTERP_MOTIF_11 */

#ifdef HP_GRAPH_WIDGET
  case XmCR_NEW_ARC:
    return (s_CR_NEW_ARC);
  case XmCR_NEW_NODE:
    return (s_CR_NEW_NODE);
  case XmCR_NODE_MOVED:
    return (s_CR_NODE_MOVED);
  case XmCR_ARC_MOVED:
    return (s_CR_ARC_MOVED);
  case XmCR_SUBGRAPH_MOVED:
    return (s_CR_SUBGRAPH_MOVED);
  case XmCR_ARC_EDITED:
    return (s_CR_ARC_EDITED);
  case XmCR_SELECT_NODE:
    return (s_CR_SELECT_NODE);
  case XmCR_SELECT_ARC:
    return (s_CR_SELECT_ARC);
  case XmCR_SELECT_SUBGRAPH:
    return (s_CR_SELECT_SUBGRAPH);
  case XmCR_DELETE_NODE:
    return (s_CR_DELETE_NODE);
  case XmCR_DELETE_ARC:
    return (s_CR_DELETE_ARC);
  case XmCR_SELECT:
    return (s_CR_SELECT);
  case XmCR_RELEASE:
    return (s_CR_RELEASE);
  case XmCR_NODE_DOUBLE_CLICK:
    return (s_CR_NODE_DOUBLE_CLICK);
  case XmCR_ARC_DOUBLE_CLICK:
    return (s_CR_ARC_DOUBLE_CLICK);
  case XmCR_DOUBLE_CLICK:
    return (s_CR_DOUBLE_CLICK);
  case XmCR_DESELECT_ARC:
    return (s_CR_DESELECT_ARC);
  case XmCR_DESELECT_NODE:
    return (s_CR_DESELECT_NODE);
  case XmCR_DESELECT:
    return (s_CR_DESELECT);
#endif /* HP_GRAPH_WIDGET */

  default:
    xlfail("Internal error in Wcb_Get_Callback_Reason_Symbol() -- unknown callback reason.");
  }
}


/*******************************************************************************
 *
 ******************************************************************************/
LVAL Wcb_Meta_Method_Add_Callback(callback_proc, one_callback_per_name_p)
     XtCallbackProc callback_proc;
     Boolean        one_callback_per_name_p;
{
  extern Widget Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(); /* w_classes.c */
  extern char* Wres_Get_Name();	/* w_resources.c */
  extern LVAL  Wres_Get_Symbol(); /* w_resources.c */
  extern Boolean Wres_Is_Callback_P(); /* w_resources.c */
  extern LVAL s_lambda, xlenv, xlfenv;
  LVAL   o_self, l_fargs, l_code;
  LVAL   xtr_name, s_callback, callback_obj;
  Widget widget_id;
  char*  name;
  
  /* get <widget_instance> */
  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&o_self);
				    
  /* get <name> */
  xtr_name = xlgetarg();
  if (xtresource_p(xtr_name) && Wres_Is_Callback_P(xtr_name)) {
    name = Wres_Get_Name(xtr_name);
    s_callback = Wres_Get_Symbol(xtr_name);
  }
  else if (symbolp(xtr_name)) {	
    xtr_name = getvalue(xtr_name);
    if (xtresource_p(xtr_name) && Wres_Is_Callback_P(xtr_name)) {
      name = Wres_Get_Name(xtr_name);
      s_callback = Wres_Get_Symbol(xtr_name);
    }
    else
      xlerror("Invalid callback name symbol", xtr_name);
  }
  else
    xlerror("Invalid callback name argument", xtr_name);
      
  /* get <call_data_binding_names_list> -- args to be bound at call time.
     NOTE: may want to check that these args are valid for the particular widget
     class that this "meta-method" is used in. No biggie though -- they'll get
     caught at runtime, when the callback fires. */
  l_fargs = xlgalist();		

  /* get <code> */
  l_code = xlgalist();
  xllastarg();

  /* 
   * if this procedure is being called from the :set_callback method 
   * (indicated by one_callback_per_name_p = TRUE), 
   * then remove all callbacks on <widget_inst> matching <name>
   */
  if (one_callback_per_name_p) { 
    extern LVAL v_savedobjs;	
    int  i = Wso_Hash(o_self);
    LVAL l_hbucket = getelement(v_savedobjs, i); /* a list of objects, including all callback-objs on this widget */
    LVAL obj;
    LVAL l_prev = NIL;
    while (l_hbucket) {		/* while there are elements in the hashbucket */
      obj = car(l_hbucket);	/* obj points to cur elt */
      if (callbackobj_p(obj) && (get_callback_name(obj) == name) && (get_callback_widget(obj) == o_self)) {
	XtRemoveCallback(widget_id, name, get_callback_proc(obj), (XtPointer) obj);
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
   * create the client_data to be sent to (*callback_proc)()
   * That procedure takes the client_data and extracts the widget-object,
   * and the closure, and uses these to execute the callback.
   */
  xlsave1(callback_obj);	/* protect some pointers */
  callback_obj = new_callbackobj();
  set_callback_widget(callback_obj, o_self);
  set_callback_name(callback_obj, name);
  set_callback_proc(callback_obj, callback_proc);
  set_callback_closure(callback_obj,
		       xlclose(s_callback, s_lambda, l_fargs, l_code, xlenv, xlfenv));
  
  XtAddCallback(widget_id, name, callback_proc, (XtPointer) callback_obj);

  /*
   * Enter the callback_obj in v_savedobjs, so that it gets marked.
   * This way, it won't be garbage collected while the callback is
   * active. :set_callback, xt_remove_callback, and :remove_all_callbacks
   * may remove and destroy the callback_obj created here. Destroying the
   * widget will result in the callbackobj getting garbage collected 
   * -- see Wcls_Widget_Destroy_Callback()
   */
  { 
    int  i = Wso_Hash(o_self);
    LVAL l_hbucket;
    extern LVAL v_savedobjs;
    
    xlsave1(l_hbucket);
    l_hbucket = cons(callback_obj, getelement(v_savedobjs, i));
    setelement(v_savedobjs, i, l_hbucket);
    xlpop();
  }

  /* resore the stack */
  xlpop(/*callback_obj*/);

  return (callback_obj);
}


/******************************************************************************
 * lisp: (SEND <widget> :REMOVE_ALL_CALLBACKS <name>)
 * returns T.
 *
 * <name> is a resource keyword of type XmRCallback, eg,
 * :XMN_ACTIVATE_CALLBACK, :XMN_ARM_CALLBACK, :XMN_DISARM_CALLBACK.
 *
 * This procedure removes all callbacks matching <name> for <widget>.
 ******************************************************************************/
LVAL Widget_Class_Method_REMOVE_ALL_CALLBACKS()
{
  extern Widget Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(); /* w_classes.c */
  extern char* Wres_Get_Name();	/* w_resources.c */
  extern Boolean Wres_Is_Callback_P(); /* w_resources.c */
  LVAL   o_self;
  LVAL   xtr_name;
  Widget widget_id;
  char*  name;
  
  /* get <widget_instance> */
  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&o_self);
				    
  /* get <name> */
  xtr_name = xlgetarg();
  if (xtresource_p(xtr_name) && Wres_Is_Callback_P(xtr_name)) {
    name = Wres_Get_Name(xtr_name);
  }
  else if (symbolp(xtr_name)) {	
    xtr_name = getvalue(xtr_name);
    if (xtresource_p(xtr_name) && Wres_Is_Callback_P(xtr_name)) {
      name = Wres_Get_Name(xtr_name);
    }
    else
      xlerror("Invalid callback name symbol", xtr_name);
  }
  else
    xlerror("Invalid callback name argument", xtr_name);
      
  xllastarg();

  /* remove from v_savedobjs all callback-objs on <widget_inst> matching <name> */
  {
    extern LVAL v_savedobjs;
    int  i = Wso_Hash(o_self);
    LVAL l_hbucket = getelement(v_savedobjs, i); /* a list of objects, including all callback-objs on this widget  */
    LVAL obj;
    LVAL l_prev = NIL;
    while (l_hbucket) {		/* while there are elements in the hashbucket */
      obj = car(l_hbucket);	/* obj points to cur elt */
      if (callbackobj_p(obj) && (get_callback_name(obj) == name) && (get_callback_widget(obj) == o_self)) {
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

  XtRemoveAllCallbacks(widget_id, name);

  return (o_self);
}


/******************************************************************************
 * lisp: (XT_REMOVE_CALLBACK <callback-obj>)
 * 
 * where <callback-obj> is the value returned by methods :set_callback or
 * :add_callback.
 ******************************************************************************/
LVAL Wcb_Prim_XtRemoveCallback()
{
  LVAL callback_obj;
  LVAL o_widget;
  Widget widget_id;
  extern LVAL true;

  callback_obj = xlga_callbackobj();
  xllastarg();

  /* check if this callback hasn't already been removed */
  if ((o_widget = get_callback_widget(callback_obj)) == NIL)
    xlerror("Callback associated with <callback-obj> has already been removed.", callback_obj);
  
  /* mark the callback_obj as being removed */
  set_callback_widget(callback_obj, NIL);

  if (!(widget_id = get_widgetobj_widgetID(o_widget)))
    xlerror("widget object not properly initialized by :isnew.", o_widget);
  
  XtRemoveCallback(widget_id,
		   get_callback_name(callback_obj),
		   get_callback_proc(callback_obj), /* note that there are difft callbackproc's for difft widgetclasses */
		   (XtPointer) callback_obj);

  /* remove <callback_obj> from v_savedobjs allowing it to be garbage collected */
  {
    extern LVAL v_savedobjs;
    int i = Wso_Hash(o_widget); /* note that we hash all callbacks on the same widget to the same hashbucket */
    LVAL l_hbucket = getelement(v_savedobjs, i);
    LVAL l_prev = NIL;

    while (l_hbucket && (car(l_hbucket) != callback_obj)) {
      l_prev = l_hbucket;
      l_hbucket = cdr(l_hbucket);
    }
    if (!l_hbucket)
      xlerror("Internal error in XtRemoveCallback -- couldn't remove <callback-obj> from v_savedobjs. Hash error?",
	      callback_obj);
    if (!l_prev)		/* first elt matched */
      setelement(v_savedobjs, i, cdr(l_hbucket));
    else
      rplacd(l_prev, cdr(l_hbucket));
  }
  
  return (true);
}


/******************************************************************************
 * lisp: (SEND <widget> :HAS_CALLBACKS <name>)
 * 		returns CALLBACK_NO_LIST -- no such callback list
 *		returns CALLBACK_HAS_NONE -- has no callbacks on list
 *		returns CALLBACK_HAS_SOME -- has some callbacks on list
 *
 * <name> is a resource keyword of type XmRCallback, eg,
 * :XMN_ACTIVATE_CALLBACK, :XMN_ARM_CALLBACK, :XMN_DISARM_CALLBACK.
 *
 *
 * XtCallbackStatus XtHasCallbacks(
 *     Widget  widgetm
 *     CONST String callback_name);
 ******************************************************************************/
static LVAL s_CALLBACK_NO_LIST, s_CALLBACK_HAS_NONE, s_CALLBACK_HAS_SOME; /* Symbols init'd in Wcb_Init() */
LVAL Widget_Class_Method_HAS_CALLBACKS()
{
  extern Widget Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(); /* w_classes.c */
  extern char* Wres_Get_Name();	/* w_resources.c */
  extern Boolean Wres_Is_Callback_P(); /* w_resources.c */
  LVAL   o_self;
  LVAL   xtr_name;
  Widget widget_id;
  char*  name;
  
  /* get <widget_instance> */
  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&o_self);
				    
  /* get <name> */
  xtr_name = xlgetarg();
  if (xtresource_p(xtr_name) && Wres_Is_Callback_P(xtr_name)) {
    name = Wres_Get_Name(xtr_name);
  }
  else if (symbolp(xtr_name)) {	
    xtr_name = getvalue(xtr_name);
    if (xtresource_p(xtr_name) && Wres_Is_Callback_P(xtr_name)) {
      name = Wres_Get_Name(xtr_name);
    }
    else
      xlerror("Invalid callback name symbol", xtr_name);
  }
  else
    xlerror("Invalid callback name argument", xtr_name);
      
  xllastarg();

  switch (XtHasCallbacks(widget_id, name)) {
  case XtCallbackNoList:
    return (s_CALLBACK_NO_LIST);
  case XtCallbackHasNone:
    return (s_CALLBACK_HAS_NONE);
  case XtCallbackHasSome:
    return (s_CALLBACK_HAS_SOME);
  }
}


/******************************************************************************
 *
 ******************************************************************************/
Wcb_Init()
{
  /*
   * Define shared callback call-data symbols used by various
   * Lexical_Bindings_For_Xm*CallbackStruct() procs in noted files.
   */
  s_CALLBACK_WIDGET = xlenter("CALLBACK_WIDGET"); /* wc_*.c */
  s_CALLBACK_REASON = xlenter("CALLBACK_REASON"); /* wc_*.c */
  s_CALLBACK_XEVENT = xlenter("CALLBACK_XEVENT"); /* wc_*.c */
#ifdef WINTERP_MOTIF_11
  s_CALLBACK_CLICK_COUNT = xlenter("CALLBACK_CLICK_COUNT"); /* wc_ArrowB.c wc_DrawnB.c wc_PushB.c */
#endif				/* WINTERP_MOTIF_11 */
  s_CALLBACK_WINDOW = xlenter("CALLBACK_WINDOW"); /* wc_DrawingA.c wc_DrawnB.c */
  s_CALLBACK_VALUE = xlenter("CALLBACK_VALUE");	/* wc_Command.c wc_FileSB.c wc_Scale.c wc_ScrollBar.c wc_SelectioB.c */
  s_CALLBACK_LENGTH = xlenter("CALLBACK_LENGTH"); /* wc_Command.c wc_FileSB.c wc_SelectioB.c */

  s_CR_NONE = xlenter("CR_NONE");
  s_CR_HELP = xlenter("CR_HELP");
  s_CR_VALUE_CHANGED = xlenter("CR_VALUE_CHANGED");
  s_CR_INCREMENT = xlenter("CR_INCREMENT");
  s_CR_DECREMENT = xlenter("CR_DECREMENT");
  s_CR_PAGE_INCREMENT = xlenter("CR_PAGE_INCREMENT");
  s_CR_PAGE_DECREMENT = xlenter("CR_PAGE_DECREMENT");
  s_CR_TO_TOP = xlenter("CR_TO_TOP");
  s_CR_TO_BOTTOM = xlenter("CR_TO_BOTTOM");
  s_CR_DRAG = xlenter("CR_DRAG");
  s_CR_ACTIVATE = xlenter("CR_ACTIVATE");
  s_CR_ARM = xlenter("CR_ARM");
  s_CR_DISARM = xlenter("CR_DISARM");
  s_CR_MAP = xlenter("CR_MAP");
  s_CR_UNMAP = xlenter("CR_UNMAP");
  s_CR_FOCUS = xlenter("CR_FOCUS");
  s_CR_LOSING_FOCUS = xlenter("CR_LOSING_FOCUS");
  s_CR_MODIFYING_TEXT_VALUE = xlenter("CR_MODIFYING_TEXT_VALUE");
  s_CR_MOVING_INSERT_CURSOR = xlenter("CR_MOVING_INSERT_CURSOR");
  s_CR_EXECUTE = xlenter("CR_EXECUTE");
  s_CR_SINGLE_SELECT = xlenter("CR_SINGLE_SELECT");
  s_CR_MULTIPLE_SELECT = xlenter("CR_MULTIPLE_SELECT");
  s_CR_EXTENDED_SELECT = xlenter("CR_EXTENDED_SELECT");
  s_CR_BROWSE_SELECT = xlenter("CR_BROWSE_SELECT");
  s_CR_DEFAULT_ACTION = xlenter("CR_DEFAULT_ACTION");
  s_CR_CLIPBOARD_DATA_REQUEST = xlenter("CR_CLIPBOARD_DATA_REQUEST");
  s_CR_CLIPBOARD_DATA_DELETE = xlenter("CR_CLIPBOARD_DATA_DELETE");
  s_CR_CASCADING = xlenter("CR_CASCADING");
  s_CR_OK = xlenter("CR_OK");
  s_CR_CANCEL = xlenter("CR_CANCEL");
  s_CR_APPLY = xlenter("CR_APPLY");
  s_CR_NO_MATCH = xlenter("CR_NO_MATCH");
  s_CR_COMMAND_ENTERED = xlenter("CR_COMMAND_ENTERED");
  s_CR_COMMAND_CHANGED = xlenter("CR_COMMAND_CHANGED");
  s_CR_EXPOSE = xlenter("CR_EXPOSE");
  s_CR_RESIZE = xlenter("CR_RESIZE");
  s_CR_INPUT = xlenter("CR_INPUT");
#ifdef WINTERP_MOTIF_11
  s_CR_GAIN_PRIMARY = xlenter("CR_GAIN_PRIMARY");
  s_CR_LOSE_PRIMARY = xlenter("CR_LOSE_PRIMARY");
#ifdef XmCR_CREATE		/* added in Motif 1.1.3 */
  s_XmCR_CREATE = xlenter("CR_CREATE");
#endif /* XmCR_CREATE */
#ifdef XmCR_PROTOCOLS		/* added in Motif 1.1.3 */
  s_XmCR_PROTOCOLS = xlenter("CR_PROTOCOLS");
#endif /* XmCR_PROTOCOLS */
#endif				/* WINTERP_MOTIF_11 */

#ifdef HP_GRAPH_WIDGET
  s_CR_NEW_ARC = xlenter("CR_NEW_ARC");
  s_CR_NEW_NODE = xlenter("CR_NEW_NODE");
  s_CR_NODE_MOVED = xlenter("CR_NODE_MOVED");
  s_CR_ARC_MOVED = xlenter("CR_ARC_MOVED");
  s_CR_SUBGRAPH_MOVED = xlenter("CR_SUBGRAPH_MOVED");
  s_CR_ARC_EDITED = xlenter("CR_ARC_EDITED");
  s_CR_SELECT_NODE = xlenter("CR_SELECT_NODE");
  s_CR_SELECT_ARC = xlenter("CR_SELECT_ARC");
  s_CR_SELECT_SUBGRAPH = xlenter("CR_SELECT_SUBGRAPH");
  s_CR_DELETE_NODE = xlenter("CR_DELETE_NODE");
  s_CR_DELETE_ARC = xlenter("CR_DELETE_ARC");
  s_CR_SELECT= xlenter("CR_SELECT");
  s_CR_RELEASE= xlenter("CR_RELEASE");
  s_CR_NODE_DOUBLE_CLICK= xlenter("CR_NODE_DOUBLE_CLICK");
  s_CR_ARC_DOUBLE_CLICK= xlenter("CR_ARC_DOUBLE_CLICK");
  s_CR_DOUBLE_CLICK= xlenter("CR_DOUBLE_CLICK");
  s_CR_DESELECT_NODE = xlenter("CR_DESELECT_NODE");
  s_CR_DESELECT_ARC = xlenter("CR_DESELECT_ARC");
  s_CR_DESELECT = xlenter("CR_DESELECT");
#endif				/* HP_GRAPH_WIDGET */

  s_CALLBACK_NO_LIST = xlenter("CALLBACK_NO_LIST");
  s_CALLBACK_HAS_NONE = xlenter("CALLBACK_HAS_NONE");
  s_CALLBACK_HAS_SOME = xlenter("CALLBACK_HAS_SOME");
}
