/* -*-C-*-
********************************************************************************
*
* File:         wc_XmGraph.c
* RCS:          $Header: wc_XmGraph.c,v 1.4 91/03/15 01:13:03 mayer Exp $
* Description:  Interface to HP's "Graph Widget" -- this widget is currently
*               available only within HP, but may be offered in HP's MotifPlus
*               user-contributed package. Within HP, you can get XmGraph by doing
*               'ninstall -vhhplday XmGraph'...
* Author:       Niels Mayer & Audrey Ishizaki, HPLabs
* Created:      Sun Feb 18 19:32:38 1990
* Modified:     Thu Oct  3 22:56:04 1991 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: wc_XmGraph.c,v 1.4 91/03/15 01:13:03 mayer Exp $";

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/Graph.h>
#include <Xm/Arc.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"


extern Widget Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(); /* w_classes.c */
extern LVAL true;

/******************************************************************************
 ******************************************************************************/
static LVAL Wcxmg_WidgetList_To_Lisp_Vector(wlist, size)
     WidgetList wlist;
     int        size;
{
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ(); /* w_classes.c */
  LVAL result;
  int i;
  
  if (!wlist || !size)
    return (NIL);

  xlsave1(result);
  result = newvector(size);
  for (i = 0; i < size; i++)
    setelement(result, i, Wcls_WidgetID_To_WIDGETOBJ((wlist[i])));
  xlpop();
  return (result);
}


/*****************************************************************************
 * (send XM_GRAPH_WIDGET_CLASS :new
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
 *     (send XM_GRAPH_WIDGET_CLASS :new ...)
 *     --> XmCreateGraph();
 *     (send XM_GRAPH_WIDGET_CLASS :new :managed ...)
 *     --> XmCreateManagedGraph();
 *     (send XM_GRAPH_WIDGET_CLASS :new :scrolled ...)
 *     --> XmCreateScrolledGraph();
 *        Note: extra convenience fn XmCreateScrolledGraph() puts the
 *	  graph widget inside a scrolled window but returns the graph widget.
 ****************************************************************************/
LVAL Xm_Graph_Widget_Class_Method_ISNEW()
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
    scrolled_p = FALSE;		/* by default, we don't want a scroled graphwidget */

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
      widget_id = XmCreateScrolledGraph(parent_widget_id, name, xt_arglist, xt_numargs);
    else
      widget_id = XmCreateGraph(parent_widget_id, name, xt_arglist, xt_numargs);
    Wres_Free_C_Arglist_Data();
  }
  else 
    if (scrolled_p)
      widget_id = XmCreateScrolledGraph(parent_widget_id, name, ARGLIST());
    else
      widget_id = XmCreateGraph(parent_widget_id, name, ARGLIST());

  Wcls_Initialize_WIDGETOBJ(self, widget_id);

  if (managed_p)
    XtManageChild(widget_id);

#ifdef DEBUG_WINTERP_1
  Wcls_Print_WidgetObj_Info(self);
#endif
  return (self);
}


/*****************************************************************************
 * (send XM_ARC_WIDGET_CLASS :new
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
 *     (send XM_ARC_WIDGET_CLASS :new ...)
 *     --> XmCreateArc();
 *     (send XM_GRAPH_WIDGET_CLASS :new :managed ...)
 *     --> XmCreateArc();
 *
 * Note: the arc widget is kind of weird... it should only be created by the
 * "convenience function" XmCreateArc() which always creates a managed arc.
 * Thus, the argument :managed or :unmanaged is completely ignored here.
 * Not anymore, with doug's latest XmGraph...
 ****************************************************************************/
LVAL Xm_Arc_Widget_Class_Method_ISNEW()
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
    /* for this particular widget creation case we just swallow the arg and ignore it 
       we deal with this arg just for compatibility with other widget creators in WINTERP
       nextarg();
       This changed with the latest XmGraph (aud)
    */

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
    widget_id = XmCreateArc(parent_widget_id, name, xt_arglist, xt_numargs);
    Wres_Free_C_Arglist_Data();
  }
  else 
    widget_id = XmCreateArc(parent_widget_id, name, ARGLIST());

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
 *    int 	reason;
 *    XEvent    *event;
 *    WidgetList     selected_widgets;
 *    int            num_selected_widgets;
 *    WidgetList     selected_arcs;
 *    int            num_selected_arcs;
 *     Widget         widget;
 *    Widget         old_to;
 *    Widget         old_from;
 *    Widget         new_to;
 *    Widget         new_from;
 *    Boolean        doit;
 * } XmGraphCallbackStruct;
 *******************************************************************************/
static LVAL s_CALLBACK_SELECTED_WIDGETS, s_CALLBACK_NUM_SELECTED_WIDGETS,
  s_CALLBACK_SELECTED_ARCS, s_CALLBACK_NUM_SELECTED_ARCS, s_CALLBACK_OLD_TO,
  s_CALLBACK_OLD_FROM, s_CALLBACK_NEW_TO, s_CALLBACK_NEW_FROM;
static void Lexical_Bindings_For_XmGraphCallbackStruct(bindings_list, lexical_env, cd, o_widget)
     LVAL bindings_list;	/* a list of symbols to which values from XmGraphCallbackStruct are bound */
     LVAL lexical_env;		
     XmGraphCallbackStruct* cd;
     LVAL o_widget;		/* XLTYPE_WIDGETOBJ */
{
  extern LVAL s_CALLBACK_WIDGET, s_CALLBACK_REASON, s_CALLBACK_XEVENT; /* w_callbacks.c */
  extern LVAL Wcb_Get_Callback_Reason_Symbol();	/* w_callbacks.c */
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ(); /* w_classes.c */
  register LVAL s_bindname;

  for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {

    s_bindname = car(bindings_list);

#ifdef THE_FOLLOWING_CODE_IS_COMMENTED_OUT
fprintf(stderr, "callback_widget: %x\n", o_widget);
fprintf(stderr, "callback_reason: %x\n", cd->reason);
fprintf(stderr, "callback_event: %x\n", cd->event);
fprintf(stderr, "callback_selected_widgets[0]: %x\n", cd->selected_widgets[0]);
fprintf(stderr, "callback_num_selected_widgets: %d\n", cd->num_selected_widgets);
fprintf(stderr, "callback_selected_arcs[0]: %x\n", cd->selected_arcs[0]);
fprintf(stderr, "callback_num_selected_arcs: %d\n", cd->num_selected_arcs);
fprintf(stderr, "callback_old_to: %x\n", cd->old_to);
fprintf(stderr, "callback_old_from: %x\n", cd->old_from);
fprintf(stderr, "callback_new_to: %x\n", cd->new_to);
fprintf(stderr, "callback_new_from: %x\n", cd->new_from);
#endif				/* THE_FOLLOWING_CODE_IS_COMMENTED_OUT */

    if (s_bindname == s_CALLBACK_WIDGET) {
      xlpbind(s_bindname, o_widget, lexical_env); /* add binding to lexical_env */
    }
    else if (s_bindname == s_CALLBACK_REASON) {
      xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_XEVENT) {
      xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_SELECTED_WIDGETS) {
      xlpbind(s_bindname, Wcxmg_WidgetList_To_Lisp_Vector(cd->selected_widgets, cd->num_selected_widgets), lexical_env);	/* the vector and it's constituent XmStrings may be freed later via gc */
    }
    else if (s_bindname == s_CALLBACK_NUM_SELECTED_WIDGETS) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->num_selected_widgets), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_SELECTED_ARCS) {
      xlpbind(s_bindname, Wcxmg_WidgetList_To_Lisp_Vector(cd->selected_arcs, cd->num_selected_arcs), lexical_env);	/* the vector and it's constituent XmStrings may be freed later via gc */
    }
    else if (s_bindname == s_CALLBACK_NUM_SELECTED_ARCS) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->num_selected_arcs), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_OLD_TO) {
      xlpbind(s_bindname, Wcls_WidgetID_To_WIDGETOBJ(cd->old_to), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_OLD_FROM) {
      xlpbind(s_bindname, Wcls_WidgetID_To_WIDGETOBJ(cd->old_from), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_NEW_TO) {
      xlpbind(s_bindname, Wcls_WidgetID_To_WIDGETOBJ(cd->new_to), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_NEW_FROM) {
      xlpbind(s_bindname, Wcls_WidgetID_To_WIDGETOBJ(cd->new_from), lexical_env);
    }
    else {
      extern char temptext[];	/* from winterp.c */
      sprintf(temptext,
	      "Unknown binding name in XmGraphCallbackStruct callback evaluator. Valid symbols are [%s %s %s %s %s %s %s %s %s %s %s].",
	      (char*) getstring(getpname(s_CALLBACK_WIDGET)),
	      (char*) getstring(getpname(s_CALLBACK_REASON)),
	      (char*) getstring(getpname(s_CALLBACK_XEVENT)),
	      (char*) getstring(getpname(s_CALLBACK_SELECTED_WIDGETS)),
	      (char*) getstring(getpname(s_CALLBACK_NUM_SELECTED_WIDGETS)),
	      (char*) getstring(getpname(s_CALLBACK_SELECTED_ARCS)),
	      (char*) getstring(getpname(s_CALLBACK_NUM_SELECTED_ARCS)),
	      (char*) getstring(getpname(s_CALLBACK_OLD_TO)),
	      (char*) getstring(getpname(s_CALLBACK_OLD_FROM)),
	      (char*) getstring(getpname(s_CALLBACK_NEW_TO)),
	      (char*) getstring(getpname(s_CALLBACK_NEW_FROM)));
      xlerror(temptext, s_bindname);
    }
  }
}


/******************************************************************************
 * This is called indirectly via XtAddCallback() for callbacks returning an
 * XmGraphCallbackStruct as call_data.
 ******************************************************************************/
void XmGraphCallbackStruct_Callbackproc(widget, client_data, call_data)
     Widget    widget;
     XtPointer client_data;
     XtPointer call_data;
{
  extern void Wcb_Meta_Callbackproc(); /* w_callbacks.c */

  Wcb_Meta_Callbackproc(client_data, call_data,
			Lexical_Bindings_For_XmGraphCallbackStruct,
			NULL);	/* FIX: the graph callback allows setting of DOIT field */
}


/*
 * The Graph widget receives the values from XmGraphCallbackStruct:
 * CALLBACK_REASON
 * CALLBACK_EVENT
 * CALLBACK_INTERACTIVE (not available)
 * CALLBACK_SELECTED_WIDGETS (list)
 * CALLBACK_NUM_SELECTED_WIDGETS
 * CALLBACK_SELECTED_ARCS (list)
 * CALLBACK_NUM_SELECTED_ARCS
 * CALLBACK_WIDGET
 * CALLBACK_OLD_TO
 * CALLBACK_OLD_FROM
 * CALLBACK_NEW_TO
 * CALLBACK_NEW_FROM
 * CALLBACK_DOIT  (not available)
 */
LVAL Xm_Graph_Widget_Class_Method_ADD_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */

  return (Wcb_Meta_Method_Add_Callback(XmGraphCallbackStruct_Callbackproc, FALSE));
}

LVAL Xm_Graph_Widget_Class_Method_SET_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */

  return (Wcb_Meta_Method_Add_Callback(XmGraphCallbackStruct_Callbackproc, TRUE));
}


/*
 * (send graph_widget :center_around_widget widget)
 */
LVAL Xm_Graph_Widget_Class_Method_CENTER_AROUND_WIDGET ()
{
  LVAL self, w;
  Widget graph;
  Widget node;

  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  node = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&w);
  xllastarg();

  XmGraphCenterAroundWidget(graph, node);

  return (self);			/* NPM */
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_DESTROY_ALL_ARCS()
{
  LVAL self;
  Widget graph;

  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  XmGraphDestroyAllArcs(graph);

  return (self);			/* NPM */
}

/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_DESTROY_ALL_NODES()
{
  LVAL self;
  Widget graph;

  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  XmGraphDestroyAllNodes(graph);

  return (self);			/* NPM */
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_DESTROY_SELECTED_ARCS_OR_NODES()
{
  LVAL self;
  Widget graph;

  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  XmGraphDestroySelectedArcsOrNodes(graph);

  return (self);			/* NPM */
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_GET_ARCS()
{
  LVAL self;
  Widget graph;
  WidgetList alist;
  int num_arcs;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  alist = XmGraphGetArcs(graph, &num_arcs);
  return (Wcxmg_WidgetList_To_Lisp_Vector(alist, num_arcs));
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_GET_NODES()
{
  LVAL self;
  Widget graph;
  WidgetList nlist;
  int n_nodes;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  nlist = XmGraphGetNodes (graph, &n_nodes);
  return (Wcxmg_WidgetList_To_Lisp_Vector(nlist, n_nodes));
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_GET_ARCS_BETWEEN_NODES()
{
  LVAL self, f, t;
  Widget graph;
  Widget from, to;
  WidgetList alist;
  int num_arcs;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  from = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&f);
  to = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&t);
  xllastarg();

  alist = XmGraphGetArcsBetweenNodes (graph, from, to, &num_arcs);
  return (Wcxmg_WidgetList_To_Lisp_Vector(alist, num_arcs));
}
/*
 * returns a vector of values
 */
LVAL Xm_Graph_Widget_Class_Method_GET_NODE_ARCS()
{
  LVAL self, w, result, alistto, alistfrom;
  Widget graph;
  Widget widget;
  int i, n_from, n_to;
  WidgetList fromlist, tolist;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  widget = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&w);
  xllastarg();

  XmGraphGetNodeArcs (graph, widget, &fromlist, &tolist, &n_from, &n_to);

  xlsave1(result);
  result = newvector(4);
  setelement(result, 0, Wcxmg_WidgetList_To_Lisp_Vector(fromlist, n_from));
  setelement(result, 1, Wcxmg_WidgetList_To_Lisp_Vector(tolist, n_to));
  setelement(result, 2, cvfixnum(n_from));
  setelement(result, 3, cvfixnum(n_to));
  xlpop();			/* NPM */
  return (result);
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_GET_ROOTS()
{
  LVAL self;
  Widget graph;
  Widget node;
  WidgetList rlist;
  int num_nodes;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  rlist = XmGraphGetRoots (graph, &num_nodes);
  return (Wcxmg_WidgetList_To_Lisp_Vector(rlist, num_nodes));
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_GET_SELECTED_ARCS()
{
  LVAL self;
  Widget graph;
  WidgetList alist;
  int num_arcs;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  alist = XmGraphGetSelectedArcs (graph, &num_arcs);
  return (Wcxmg_WidgetList_To_Lisp_Vector(alist, num_arcs));
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_GET_SELECTED_NODES()
{
  LVAL self;
  Widget graph;
  WidgetList nlist;
  int num;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  nlist = XmGraphGetSelectedNodes (graph, &num);
  return (Wcxmg_WidgetList_To_Lisp_Vector(nlist, num));
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_INPUT_OVER_ARC()
{
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ(); /* w_classes.c */
  LVAL self;
  Widget graph;
  int x_pos, y_pos;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  x_pos = (int) getfixnum(xlgafixnum());
  y_pos = (int) getfixnum(xlgafixnum());
  xllastarg();

  return (Wcls_WidgetID_To_WIDGETOBJ(XmGraphInputOverArc(graph, x_pos, y_pos)));
}
/*
 *  FIX
 */
LVAL Xm_Graph_Widget_Class_Method_INSERT_ROOTS()
{
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_IS_POINT_IN_ARC()
{
  LVAL self;
  Widget arc;
  int x, y;

  arc = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  x = (int) getfixnum(xlgafixnum());
  y = (int) getfixnum(xlgafixnum());
  xllastarg();

  return (XmGraphIsPointInArc(arc, x, y) ? true : NIL);
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_IS_SELECTED_ARC()
{
  LVAL self, a;
  Widget graph;
  Widget arc;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  arc = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&a);
  xllastarg();

  return (XmGraphIsSelectedArc(graph, arc)? true : NIL);
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_IS_SELECTED_NODE()
{
  LVAL self, w;
  Widget graph;
  Widget node;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  node = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&w);
  xllastarg();

  return (XmGraphIsSelectedNode(graph, node) ? true : NIL);
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_MOVE_ARC()
{
  LVAL self, a, f, t;
  Widget graph;
  Widget arc;
  Widget from, to;
  Boolean rval;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  arc = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&a);
  from = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&f);
  to = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&t);
  xllastarg();

  return (XmGraphMoveArc(graph, arc, from, to) ? true : NIL);
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_MOVE_NODE()
{
  LVAL self, n, f, t;
  Widget graph;
  Widget node, from, to;

  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  node = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&n);
  from = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&f);
  to = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&t);
  xllastarg();

  return (XmGraphMoveNode(graph, node, from, to) ? true : NIL);
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_NUM_ARCS()
{
  LVAL self;
  Widget graph;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (cvfixnum((FIXTYPE) XmGraphNumArcs(graph)));
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_NUM_NODES()
{
  LVAL self;
  Widget graph;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (cvfixnum((FIXTYPE) XmGraphNumNodes(graph)));
}
/*
 *  returns a vector of two values
 */
LVAL Xm_Graph_Widget_Class_Method_NUM_NODE_ARCS()
{
  LVAL self, w, result;
  Widget graph;
  Widget node;
  int n_from, n_to;

  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  node = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&w);
  xllastarg();

  XmGraphNumNodeArcs (graph, w, &n_from, &n_to);

  xlsave1(result);
  result = newvector(2);
  setelement(result, 0, cvfixnum(n_from));
  setelement(result, 1, cvfixnum(n_to));
  xlpop();			/* NPM */
  return (result);
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_NUM_ROOTS()
{
  LVAL self;
  Widget graph;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (cvfixnum((FIXTYPE) XmGraphNumRoots(graph)));
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_NUM_SELECTED_ARCS()
{
  LVAL self;
  Widget graph;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (cvfixnum((FIXTYPE) XmGraphNumSelectedArcs(graph)));
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_NUM_SELECTED_NODES()
{
  LVAL self;
  Widget graph;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (cvfixnum((FIXTYPE) XmGraphNumSelectedNodes(graph)));
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_MOVE_ALL()
{
  LVAL self;
  Widget graph;
  int delta_x, delta_y;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  delta_x = (int) getfixnum(xlgafixnum());
  delta_y = (int) getfixnum(xlgafixnum());
  xllastarg();

  XmGraphMoveAll(graph, delta_x, delta_y);

  return (self);			/* NPM */
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_LAYOUT()
{
  LVAL self;
  Widget graph;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  XmGraphLayout(graph);

  return (self);			/* NPM */
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_RELAY_SUBGRAPH()
{
  LVAL self, w;
  Widget graph;
  Widget node;

  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  node = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&w);
  xllastarg();

  XmGraphRelaySubgraph(graph, node);

  return (self);			/* NPM */
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_REMOVE_ARC_BETWEEN_NODES()
{
  LVAL self, w1, w2;
  Widget graph;
  Widget widget1, widget2;
  
  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  widget1 = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&w1);
  widget2 = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&w2);
  xllastarg();

  XmGraphRemoveArcBetweenNodes(graph, widget1, widget2);

  return (self);			/* NPM */
}
/*
 * FIX: 
 */
LVAL Xm_Graph_Widget_Class_Method_REMOVE_ROOTS()
{
 /*
  XmGraphRemoveRoots (graph, nodes, n_nodes);
 */
}
/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_SELECT_ARC()
{
  LVAL self, a;
  Widget graph;
  Widget arc;

  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  arc = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&a);
  xllastarg();

  XmGraphSelectArc (graph, arc);

  return (self);			/* NPM */
}

/*
 *  FIX: 
 */
LVAL Xm_Graph_Widget_Class_Method_SELECT_ARCS()
{
/*
  XmGraphSelectArcs (graph, arcs, n_arcs);
 */
}

/*
 *  FIX
 */
LVAL Xm_Graph_Widget_Class_Method_SELECT_NODES()
{
}

/*
 *
 */
LVAL Xm_Graph_Widget_Class_Method_UNSELECT_ARC()
{
  LVAL self, a;
  Widget graph;
  Widget arc;

  graph = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  arc = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&a);
  xllastarg();

  XmGraphUnselectArc (graph, arc); 

  return (self);			/* NPM */
}

/*
 *  FIX:
 */
LVAL Xm_Graph_Widget_Class_Method_UNSELECT_ARCS()
{
/*
  XmGraphUnselectArcs (graph, arcs, n_arcs);
*/
}

/*
 *  FIX
 */
LVAL Xm_Graph_Widget_Class_Method_UNSELECT_NODES()
{
/*
  XmGraphUnselectNodes (graph, nodes, n_nodes);
*/
}

/*
 * (send <graph_widget> :select_node <node_widget>)
 */
LVAL Xm_Graph_Widget_Class_Method_SELECT_NODE ()
{
  LVAL self, node;
  Widget graph_widget;
  Widget node_widget;

  graph_widget = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  node_widget = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&node);
  xllastarg();

  XmGraphSelectNode(graph_widget, node_widget);

  return (self);			/* NPM */
}

/*
 * (send <graph_widget> :unselect_node <node_widget>)
 */
LVAL Xm_Graph_Widget_Class_Method_UNSELECT_NODE ()
{
  LVAL self, node;
  Widget graph_widget;
  Widget node_widget;
  
  graph_widget = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  node_widget = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&node);
  xllastarg();

  XmGraphUnselectNode(graph_widget, node_widget);

  return (self);			/* NPM */
}

/******************************************************************************
 *
 ******************************************************************************/
Wc_XmGraph_Init()
{
  LVAL o_XM_GRAPH_WIDGET_CLASS;
  LVAL o_XM_ARC_WIDGET_CLASS;
  extern LVAL Wcls_Create_Subclass_Of_WIDGET_CLASS(); /* w_classes.c */
  extern      xladdmsg();	/* from xlobj.c */
  
  /******************************* GRAPH WIDGET *********************************/
  o_XM_GRAPH_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_GRAPH_WIDGET_CLASS",
					 xmGraphWidgetClass);

  /* a special :isnew method on this class allows for the creation of this
     widget inside a scrolled window if the submessage keyword :scrolled 
     is given ... */
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Xm_Graph_Widget_Class_Method_ISNEW);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":ADD_CALLBACK", 
	   FTAB_Xm_Graph_Widget_Class_Method_ADD_CALLBACK);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":SET_CALLBACK", 
	   FTAB_Xm_Graph_Widget_Class_Method_SET_CALLBACK);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":CENTER_AROUND_WIDGET",
	   FTAB_Xm_Graph_Widget_Class_Method_CENTER_AROUND_WIDGET);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":DESTROY_ALL_ARCS",
	   FTAB_Xm_Graph_Widget_Class_Method_DESTROY_ALL_ARCS);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":DESTROY_ALL_NODES",
	   FTAB_Xm_Graph_Widget_Class_Method_DESTROY_ALL_NODES);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":DESTROY_SELECTED_ARCS_OR_NODES",
	   FTAB_Xm_Graph_Widget_Class_Method_DESTROY_SELECTED_ARCS_OR_NODES);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":GET_ARCS",
	   FTAB_Xm_Graph_Widget_Class_Method_GET_ARCS);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":GET_NODES",
	   FTAB_Xm_Graph_Widget_Class_Method_GET_NODES);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":GET_ARCS_BETWEEN_NODES",
	   FTAB_Xm_Graph_Widget_Class_Method_GET_ARCS_BETWEEN_NODES);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":GET_NODE_ARCS",
	   FTAB_Xm_Graph_Widget_Class_Method_GET_NODE_ARCS);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":GET_ROOTS",
	   FTAB_Xm_Graph_Widget_Class_Method_GET_ROOTS);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":GET_SELECTED_ARCS",
	   FTAB_Xm_Graph_Widget_Class_Method_GET_SELECTED_ARCS);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":GET_SELECTED_NODES",
	   FTAB_Xm_Graph_Widget_Class_Method_GET_SELECTED_NODES);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":INPUT_OVER_ARC",
	   FTAB_Xm_Graph_Widget_Class_Method_INPUT_OVER_ARC);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":INSERT_ROOTS",
	   FTAB_Xm_Graph_Widget_Class_Method_INSERT_ROOTS);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":IS_POINT_IN_ARC",
	   FTAB_Xm_Graph_Widget_Class_Method_IS_POINT_IN_ARC);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":IS_SELECTED_ARC",
	   FTAB_Xm_Graph_Widget_Class_Method_IS_SELECTED_ARC);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":IS_SELECTED_NODE",
	   FTAB_Xm_Graph_Widget_Class_Method_IS_SELECTED_NODE);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":MOVE_ARC",
	   FTAB_Xm_Graph_Widget_Class_Method_MOVE_ARC);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":MOVE_NODE",
	   FTAB_Xm_Graph_Widget_Class_Method_MOVE_NODE);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, "NUM_ARCS:",
	   FTAB_Xm_Graph_Widget_Class_Method_NUM_ARCS);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":NUM_NODES",
	   FTAB_Xm_Graph_Widget_Class_Method_NUM_NODES);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":NUM_NODE_ARCS",
	   FTAB_Xm_Graph_Widget_Class_Method_NUM_NODE_ARCS);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":NUM_ROOTS",
	   FTAB_Xm_Graph_Widget_Class_Method_NUM_ROOTS);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":NUM_SELECTED_ARCS",
	   FTAB_Xm_Graph_Widget_Class_Method_NUM_SELECTED_ARCS);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":NUM_SELECTED_NODES",
	   FTAB_Xm_Graph_Widget_Class_Method_NUM_SELECTED_NODES);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":MOVE_ALL",
	   FTAB_Xm_Graph_Widget_Class_Method_MOVE_ALL);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":LAYOUT",
	   FTAB_Xm_Graph_Widget_Class_Method_LAYOUT);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":RELAY_SUBGRAPH",
	   FTAB_Xm_Graph_Widget_Class_Method_RELAY_SUBGRAPH);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":REMOVE_ARC_BETWEEN_NODES",
	   FTAB_Xm_Graph_Widget_Class_Method_REMOVE_ARC_BETWEEN_NODES);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":REMOVE_ROOTS",
	   FTAB_Xm_Graph_Widget_Class_Method_REMOVE_ROOTS);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":SELECT_ARC",
	   FTAB_Xm_Graph_Widget_Class_Method_SELECT_ARC);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":SELECT_ARCS",
	   FTAB_Xm_Graph_Widget_Class_Method_SELECT_ARCS);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":SELECT_NODE", 
	   FTAB_Xm_Graph_Widget_Class_Method_SELECT_NODE);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":SELECT_NODES", 
	   FTAB_Xm_Graph_Widget_Class_Method_SELECT_NODES);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":UNSELECT_ARC",
	   FTAB_Xm_Graph_Widget_Class_Method_UNSELECT_ARC);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":UNSELECT_ARCS",
	   FTAB_Xm_Graph_Widget_Class_Method_UNSELECT_ARCS);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":UNSELECT_NODE", 
	   FTAB_Xm_Graph_Widget_Class_Method_UNSELECT_NODE);
  xladdmsg(o_XM_GRAPH_WIDGET_CLASS, ":UNSELECT_NODES", 
	   FTAB_Xm_Graph_Widget_Class_Method_UNSELECT_NODES);

  /******************************** ARC WIDGET **********************************/
  o_XM_ARC_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_ARC_WIDGET_CLASS",
					 xmArcWidgetClass);

  /* A special :isnew method on this class overrides default Xt widget creation  
     routine on meta-class WIDGET_CLASS. We need to call special creator XmCreateArc(). */
  xladdmsg(o_XM_ARC_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Xm_Arc_Widget_Class_Method_ISNEW);
  
  s_CALLBACK_SELECTED_WIDGETS = xlenter("CALLBACK_SELECTED_WIDGETS");
  s_CALLBACK_NUM_SELECTED_WIDGETS = xlenter ("CALLBACK_NUM_SELECTED_WIDGETS");
  s_CALLBACK_SELECTED_ARCS = xlenter("CALLBACK_SELECTED_ARCS");
  s_CALLBACK_NUM_SELECTED_ARCS = xlenter("CALLBACK_NUM_SELECTED_ARCS");
  s_CALLBACK_OLD_TO = xlenter("CALLBACK_OLD_TO");
  s_CALLBACK_OLD_FROM = xlenter("CALLBACK_OLD_FROM");
  s_CALLBACK_NEW_TO = xlenter("CALLBACK_NEW_TO");
  s_CALLBACK_NEW_FROM = xlenter("CALLBACK_NEW_FROM");
}
