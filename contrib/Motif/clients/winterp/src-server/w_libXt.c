/* -*-C-*-
********************************************************************************
*
* File:         w_libXt.c
* RCS:          $Header: w_libXt.c,v 1.3 91/03/14 03:13:47 mayer Exp $
* Description:  Random XLISP Primitives and Methods for the Xt Intrinsics
* Author:       Niels Mayer, HPLabs
* Created:      Fri Nov 24 00:36:13 1989
* Modified:     Thu Oct  3 20:51:42 1991 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: w_libXt.c,v 1.3 91/03/14 03:13:47 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>		/* Xm/Xm.h only needed for "winterp.h"*/
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"

extern Widget Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(); /* w_classes.c */

/*****************************************************************************
 * (send <widget> :SET_VALUES 
 *                <resource_1> <value_1>
 *                . . .
 *                <resource_n> <value_n>)
 * ==> returns <widget>
 *
 * void XtSetValues(widget, args, num_args);
 *     Widget           widget;
 *     ArgList          args;
 *     Cardinal         num_args;
 ****************************************************************************/
LVAL Widget_Class_Method_SET_VALUES()
{
  extern ArgList Wres_Get_LispArglist(); /* from w_resources.c */
  extern void    Wres_Free_C_Arglist_Data(); /* from w_resources.c */
  LVAL    self;
  Cardinal xt_numargs;
  ArgList xt_arglist;
  Widget  widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  
  if (moreargs()) {
    xt_arglist = Wres_Get_LispArglist(self, widget_id, NULL, 0, &xt_numargs);
    XtSetValues(widget_id, xt_arglist, xt_numargs);
    Wres_Free_C_Arglist_Data();
  }
  else
    xlfail("In widget :set_values method, no arguments were given.");
  
  return (self);
}


/*****************************************************************************
 * (send <widget> :GET_VALUES 
 *                <resource_1> <place_1>
 *                . . .
 *                <resource_n> <place_n>)
 * where <place_i> is a the place to put <resource_i>. If <place_i> is NIL,
 * then <resource_i>'s value is returned in a list. If <place_i> is a quoted
 * symbol, then that symbol gets <resource_i>'s value. If <place_i> is a 
 * place-form (see 'setf') then <resource_i>'s value will be put in <place_i>:
 *         <place>     the field specifier (quoted):
 *                        <sym>                   set value of a symbol
 *                        (car <expr>)            set car of a cons node
 *                        (cdr <expr>)            set cdr of a cons node
 *                        (nth <n> <expr>)        set nth car of a list
 *                        (aref <expr> <n>)       set nth element of an array
 *                        (get <sym> <prop>)      set value of a property
 *                        (symbol-value <sym>)    set value of a symbol
 *                        (symbol-function <sym>) set functional value of a symbol
 *                        (symbol-plist <sym>)    set property list of a symbol
 *
 * void XtGetValues(widget, args, num_args);
 *     Widget           widget;
 *     ArgList          args;
 *     Cardinal         num_args;
 ****************************************************************************/
LVAL Widget_Class_Method_GET_VALUES()
{
  extern ArgList Wres_Get_GetValues_ArgList();	/* from w_resources.c */
  extern LVAL    Wres_GetValues_ArgList_To_Lisp(); /* from w_resources.c */
  LVAL    self;
  Cardinal xt_numargs;
  ArgList xt_arglist;
  Widget  widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);

  if (moreargs()) {
    xt_arglist = Wres_Get_GetValues_ArgList(&xt_numargs);
    XtGetValues(widget_id, xt_arglist, xt_numargs);
    return (Wres_GetValues_ArgList_To_Lisp(xt_arglist, xt_numargs)); /* sets the variables assoc'd w/ resource and/or return list of vars that weren't set */
  }
  else
    xlfail("In widget :get_values method, no arguments were given.");
}


/*****************************************************************************
 * (send <widget> :destroy) -- destroy a widget. 
 * ==> returns NIL.
 *
 * After calling this routine, the WIDGETOBJ may be garbage collected, along
 * with all other resources the widget references, such as PIXMAPs CALLBACKOBJs,
 * and EVHANDLEROBJs. Furthermore, this will destroy all children of the
 * destoyed widget and allow their storage to be garbage collected as well.
 *
 * void XtDestroyWidget (widget);
 *     Widget widget;
 ****************************************************************************/
LVAL Widget_Class_Method_DESTROY()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  XtDestroyWidget(widget_id);
  return (NIL);
}


/*****************************************************************************
 * (send <widget> :MANAGE)
 * ==> returns <widget>
 *
 * This method will add <widget> to it's parent-widget's managed
 * list, which means it will become visible (if mapped) and will take up space
 * within the managing widget.
 *
 * void XtManageChild (child);
 *      Widget    child;
 ****************************************************************************/
LVAL Widget_Class_Method_MANAGE()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  XtManageChild(widget_id);
  return (self);
}


/*****************************************************************************
 * (send <widget> :UNMANAGE)
 * ==> returns <widget>
 *
 * This method will remove <widget> from its parent's
 * managed list, which means it will no longer be visible or take up space.
 *
 * void XtUnmanageChild (child);
 *     Widget child;
 ****************************************************************************/
LVAL Widget_Class_Method_UNMANAGE()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  XtUnmanageChild(widget_id);
  return (self);
}

/******************************************************************************/
#define WIDGETLIST_SIZE_INCREMENT 20
static Cardinal widgetlist_size = 0;
static WidgetList widgetlist = NULL;
WidgetList Get_Widget_List_or_Vector_Argument_Returning_WidgetList(num_children)
     Cardinal *num_children;
{
  register Cardinal size, i;
  LVAL elt, lval_widgets;
  
  lval_widgets = xlgetarg();

  if (vectorp(lval_widgets)) {
    *num_children = size = (Cardinal) getsize(lval_widgets); /* get number of widgets */
    if (size > widgetlist_size) { /* make sure it'll fit in current widgetlist array */
      if (widgetlist)
	widgetlist = (Widget*) XtRealloc(widgetlist, (unsigned) (size * sizeof(Widget)));
      else
	widgetlist = (Widget*) XtMalloc((unsigned) (size * sizeof(Widget)));
      widgetlist_size = size;
    }
    for (i = 0 ; (i < size) ; i++) {
      elt = getelement(lval_widgets, i);
      if (widgetobj_p(elt)) {
	if (!(widgetlist[i] = get_widgetobj_widgetID(elt)))
	  xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", elt);
      }
      else
	xlerror("Bad widget-vector element -- expected a vector of WIDGETOBJs.", elt);
    }
  }
  else if (consp(lval_widgets)) {
    for (i = 0 ; (consp(lval_widgets)) ; lval_widgets = cdr(lval_widgets), i++) {
      if (i >= widgetlist_size)	{ /* make sure it'll fit in current widgetlist array */
	if (widgetlist)
	  widgetlist = (Widget*) XtRealloc(widgetlist, (unsigned) ((widgetlist_size + WIDGETLIST_SIZE_INCREMENT) * sizeof(Widget)));
	else
	  widgetlist = (Widget*) XtMalloc((unsigned) (WIDGETLIST_SIZE_INCREMENT * sizeof(Widget)));
	widgetlist_size += WIDGETLIST_SIZE_INCREMENT;
      }
      elt = car(lval_widgets);
      if (widgetobj_p(elt)) {
	if (!(widgetlist[i] = get_widgetobj_widgetID(elt)))
	  xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", elt);
      }
      else
	xlerror("Bad widget-list element -- expected a list of WIDGETOBJs.", elt);
    }
    if (lval_widgets)		/* if loop terminated due to list pointer not being a CONS cell */
      xlerror("Bad widget-list element -- expected a list of WIDGETOBJs.", lval_widgets);
    *num_children = i;
  }
  else
    xlerror("Bad argument type -- expected a list or vector of WIDGETOBJs.", lval_widgets);

  return (widgetlist);		/* DO NOT FREE THIS RESULT */
}

/******************************************************************************
 * (XT_MANAGE_CHILDREN <widgets...>)
 * ==> returns T.
 * 
 * Given a vector or list of widgets, all of which must be children of the
 * same parent, this will add those widgets to their parent-widget's managed
 * list, which means they will become visible (if mapped) and will take up space
 * within the managing widget.
 *
 * An error will be signalled if the widgets passed to this routine  do not
 * all have the same parent.
 *
 * void XtManageChildren(children, num_children)
 *     WidgetList children;
 *     Cardinal   num_children;
 ******************************************************************************/
LVAL Wxt_Prim_XT_MANAGE_CHILDREN()
{
  WidgetList children;
  Cardinal num_children;
  extern LVAL true;

  children = Get_Widget_List_or_Vector_Argument_Returning_WidgetList(&num_children);
  xllastarg();

  XtManageChildren(children, num_children);

  return (true);
}

/******************************************************************************
 * (XT_UNMANAGE_CHILDREN <widgets...>)
 * ==> returns T.
 * 
 * Given a vector or list of widgets, all of which must be children of the
 * same parent, this will remove those child-widgets from their parent's
 * managed list, which means they will no longer be visible or take up space.
 *
 * An error will be signalled if the widgets passed to this routine  do not
 * all have the same parent.
 *
 * void XtUnmanageChildren (children, num_children)
 *     WidgetList children;
 *     Cardinal   num_children;
 ******************************************************************************/
LVAL Wxt_Prim_XT_UNMANAGE_CHILDREN()
{
  WidgetList children;
  Cardinal num_children;
  extern LVAL true;

  children = Get_Widget_List_or_Vector_Argument_Returning_WidgetList(&num_children);
  xllastarg();

  XtUnmanageChildren(children, num_children);

  return (true);
}


/******************************************************************************
 * (send <widget> :ADD_GRAB <exclusive_p> <spring_loaded_p>)
 * ==>  returns <widget>
 *
 * Appends <widget> to the modal cascade -- redirects user input to this widget
 * <exclusive_p> and <spring_loaded_p> are booleans.
 *
 * void XtAddGrab(widget, exclusive, spring_loaded)
 *      Widget  widget;
 *      Boolean exclusive;
 *      Boolean spring_loaded;
 ******************************************************************************/
LVAL Widget_Class_Method_ADD_GRAB()
{
  LVAL self, exclusive_p, spring_loaded_p;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  exclusive_p = xlgetarg();
  spring_loaded_p = xlgetarg();
  xllastarg();

  XtAddGrab(widget_id,
	    (exclusive_p) ? TRUE : FALSE,
	    (spring_loaded_p) ? TRUE : FALSE);
  return (self);
}


/******************************************************************************
 * (send <widget> :REMOVE_GRAB)
 * ==>  returns <widget>
 *
 * Removes redirection of user input to <widget>.
 * 
 * void XtRemoveGrab(widget)
 *     Widget  widget;
 ******************************************************************************/
LVAL Widget_Class_Method_REMOVE_GRAB()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  XtRemoveGrab(widget_id);
  return (self);
}


/******************************************************************************
 * (send <widget> :IS_COMPOSITE)
 * ==> returns T if <widget> is a composite widget,  else NIL.
 *
 * #define XtIsCompositeObject(widget) XtIsSubclass(widget, (WidgetClass) compositeObjectClass)
 ******************************************************************************/
LVAL Widget_Class_Method_IS_COMPOSITE()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();


#ifndef WINTERP_MOTIF_11
  return (XtIsCompositeObject(widget_id) ? true : NIL);
#else
  return (XtIsComposite(widget_id) ? true : NIL);
#endif /* WINTERP_MOTIF_11 */


}


/******************************************************************************
 * (send <widget> :IS_CONSTRAINT)
 * ==> returns T if <widget> is a constraint widget, else NIL.
 *
 * #define XtIsConstraint(widget)      XtIsSubclass(widget, (WidgetClass) constraintWidgetClass)
 ******************************************************************************/
LVAL Widget_Class_Method_IS_CONSTRAINT()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (XtIsConstraint(widget_id) ? true : NIL);
}


/******************************************************************************
 * (send <widget> :IS_SHELL)
 * ==> returns T if <widget> is a shell widget, else NIL.
 *
 * #define XtIsShell(widget)	    XtIsSubclass(widget, (WidgetClass) shellWidgetClass)
 ******************************************************************************/
LVAL Widget_Class_Method_IS_SHELL()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (XtIsShell(widget_id) ? true : NIL);
}


/******************************************************************************
 * (send <widget> :SET_SENSITIVE <sensitive_p>)
 * ==> returns <widget>.
 *
 * If <sensitive_p> is NIL, then the widget will not respond to user input.
 *
 * void XtSetSensitive (widget, sensitive)
 *      Widget    widget;
 *      Boolean   sensitive;
 ******************************************************************************/
LVAL Widget_Class_Method_SET_SENSITIVE()
{
  LVAL self, sensitive_p;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  sensitive_p = xlgetarg();
  xllastarg();

  XtSetSensitive(widget_id, 
		 (sensitive_p) ? TRUE : FALSE);
  return (self);
}


/******************************************************************************
 * (send <widget> :SET_MAPPED_WHEN_MANAGED <mapped_p>)
 * ==> returns <widget>.
 *
 * If <mapped_p> is non-NIL, then the widget will be mapped (displayed).
 *
 * void XtSetMappedWhenManaged()
 *      Widget    widget;
 *      Boolean   mappedWhenManaged;
 ******************************************************************************/
LVAL Widget_Class_Method_SET_MAPPED_WHEN_MANAGED()
{
  LVAL self, mapped_p;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  mapped_p = xlgetarg();
  xllastarg();

  XtSetMappedWhenManaged(widget_id, 
			 (mapped_p) ? TRUE : FALSE);
  return (self);
}

/******************************************************************************
 * (send <widget> :IS_MANAGED)
 * ==> returns T if the widget is managed, else NIL. See method :MANAGE.
 *
 * Boolean XtIsManaged(widget)
 *        Widget widget;
 ******************************************************************************/
LVAL Widget_Class_Method_IS_MANAGED()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (XtIsManaged(widget_id) ? true : NIL);
}


/******************************************************************************
 * (send <widget> :IS_REALIZED)
 * ==> returns T if the widget is realized, else NIL. See method :REALIZE.
 *
 * Boolean XtIsRealized (widget);
 *         Widget widget;
 ******************************************************************************/
LVAL Widget_Class_Method_IS_REALIZED()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (XtIsRealized(widget_id) ? true : NIL);
}


/******************************************************************************
 * (send <widget> :IS_SENSITIVE)
 * ==> returns T if the widget will accept user input, else NIL. See also
 * method :SET_SENSITIVE.
 *
 * Boolean XtIsSensitive(widget);
 *         Widget widget;
 ******************************************************************************/
LVAL Widget_Class_Method_IS_SENSITIVE()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (XtIsSensitive(widget_id) ? true : NIL);
}

/*****************************************************************************
 * (send <widget> :PARENT)
 * ==> returns the given <widget>'s parent widget or NIL if no parent.
 *
 * Widget XtParent(widget)
 *        Widget widget;
 *
 * Note: for Motif 1.1 bug workaround for calling :PARENT on
 * XmList/:scrolled and XmText/:scrolled widgets, see methods
 * Xm_List_Widget_Class_Method_PARENT() and Xm_Text_Widget_Class_Method_PARENT()
 ****************************************************************************/
LVAL Widget_Class_Method_PARENT()
{
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ();
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (Wcls_WidgetID_To_WIDGETOBJ(XtParent(widget_id)));
}


/*****************************************************************************
 * (send <widget> :WINDOW)
 * ==> returns the given <widget>'s window.
 *
 * Window XtWindow(Widget);
 * Window XtWindowOfObject(Widget);
 ****************************************************************************/
LVAL Widget_Class_Method_WINDOW()
{
  LVAL self;
  Widget widget_id;
  Window window_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

#ifdef WINTERP_MOTIF_11
  if (window_id = XtWindowOfObject(widget_id))
#else
  if (window_id = XtWindow(widget_id))
#endif /* WINTERP_MOTIF_11 */
    return (cv_window(window_id));
  else
    return (NIL);
}


/*****************************************************************************
 * (send <widget> :MAP)
 * ==> returns <widget>.
 *
 * If the widget is realized and managed, this method will make the window
 * appear on the display. Make it disappear with :UNMAP.
 *
 * #define XtMapWidget(widget)	XMapWindow(XtDisplay(widget), XtWindow(widget))
 ****************************************************************************/
LVAL Widget_Class_Method_MAP()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  if (
#ifdef WINTERP_MOTIF_11
      XtIsWidget(widget_id)
#else
      XtIsWindowObject(widget_id)
#endif				/* WINTERP_MOTIF_11 */
      ) {
    XtMapWidget(widget_id);
    return (self);
  }
  else 
    xlerror("Method :MAP only applies to widgets, not gadgets.", self);
}


/*****************************************************************************
 * (send <widget> :UNMAP)
 * ==> returns <widget>.
 *
 * If the widget is realized and managed, this method will make the window
 * disappear from the display. Make it reappear with :MAP.
 *
 * #define XtUnmapWidget(widget) XUnmapWindow(XtDisplay(widget), XtWindow(widget))
 ****************************************************************************/
LVAL Widget_Class_Method_UNMAP()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  if (
#ifdef WINTERP_MOTIF_11
      XtIsWidget(widget_id)
#else
      XtIsWindowObject(widget_id)
#endif				/* WINTERP_MOTIF_11 */
      ) {
    XtUnmapWidget(widget_id);
    return (self);
  }
  else 
    xlerror("Method :UNMAP only applies to widgets, not gadgets.", self);
}


#ifdef WINTERP_MOTIF_11		/* actually, just X11r4, but Motif1.1-->X11r4 */
/*****************************************************************************
 * (send <widget> :NAME)
 * ==> returns string of widget's name.
 *
 * extern String XtName(Widget object);
 ****************************************************************************/
LVAL Widget_Class_Method_NAME()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (cvstring((char*) XtName(widget_id)));	/* XtName String return is copied by cvstring(), copy is freed by XLISP-GC */
}
#endif /* WINTERP_MOTIF_11 */


#ifdef WINTERP_MOTIF_11
/******************************************************************************
 * (XT_RESOLVE_PATHNAME <type> <filename> <suffix> <path>)
 *	==> returns a string representing the pathname of an existing file
 *	    created from the parameters  <type> <filename> <suffix> <path>.
 *	    will return NIL if no such file exists, is readable, and
 *	    isn't a directory.
 *
 * where
 *
 * <path> is a string of paths separated by colons (':'), in which the 
 * 	following substituions are done:
 *	%N -- gets substituted by the parameter <filename>, a name without the extension.
 *	%T -- gets substituted by the parameter <type>, a directory name. 
 *	%S -- gets substituted by the parameter <suffix>.
 *	
 *	X11r4 Language-dependent substitutions for %L, %l %t, %c may occur as well.
 *	See the documentation for XtResolvePathname() for further details.
 *
 * <type> is a STRING, or NIL. This substitutes for occurrences of %T in <path>.
 *
 * <filename> is a STRING. If this parameter is NIL, then note that XtResolvePathname()
 *	will substitute the application class name.
 *
 * <suffix> is a STRING, or NIL. This substitutes for occurrences of %S in <path>.
 * 
 * String XtResolvePathname(
 *     Display*	dpy,
 *     CONST String type,
 *     CONST String filename,
 *     CONST String suffix,
 *     CONST String path,
 *     Substitution substitution,
 *     Cardinal num_substitutions,
 *     XtFilePredicate predicate)
 ******************************************************************************/
LVAL Wxt_Prim_XT_RESOLVE_PATHNAME()
{
  extern Display* display;	/* global in winterp.c */
  String type, filename, suffix, path;
  String result;
  /*  LVAL lval_result; */

  if (moreargs() && (*xlargv == NIL)) {	/* get <type> */
    type = NULL;
    nextarg();
  }
  else
    type = (String) getstring(xlgastring());

  if (moreargs() && (*xlargv == NIL)) {	/* get <filename> */
    filename = NULL;
    nextarg();
  }
  else
    filename = (String) getstring(xlgastring());

  if (moreargs() && (*xlargv == NIL)) {	/* get <suffix> */
    suffix = NULL;
    nextarg();
  }
  else
    suffix = (String) getstring(xlgastring());

  if (moreargs() && (*xlargv == NIL)) {	/* get <path> */
    path = NULL;
    nextarg();
  }
  else
    path = (String) getstring(xlgastring());

  xllastarg();

  result = XtResolvePathname(display, type, filename, suffix, path,
			     (Substitution) NULL, 0,
			     (XtFilePredicate) NULL);

  if (result) {
/*
    lval_result = cvstring(result);
    XtFree(result);
    return (lval_result);
*/
    return (cv_string(result));	/* XtResolvePathname() result is not copied, will be freed by XLISP-GC */
  }
  else
    return (NIL);
}
#endif
