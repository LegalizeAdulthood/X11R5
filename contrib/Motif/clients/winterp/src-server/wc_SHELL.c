/* -*-C-*-
********************************************************************************
*
* File:         wc_SHELL.c
* RCS:          $Header: wc_SHELL.c,v 1.6 91/04/08 14:06:52 mayer Exp $
* Description:  SHELL_WIDGET/POPUP_SHELL_WIDGET metaclass + class instances.
* Author:       Niels Mayer, HPLabs
* Created:      Fri Oct 27 22:03:22 1989
* Modified:     Thu Oct  3 23:40:48 1991 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: wc_SHELL.c,v 1.6 91/04/08 14:06:52 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/MenuShell.h>

#include "winterp.h"

/* this must come after winterp.h since WINTERP_MOTIF_11 may be def'd there */
#ifdef WINTERP_MOTIF_11
#include <Xm/Protocols.h>	/* <Xm/Protocols.h> location seems to have moved in 1.1 */
#else
#include <X11/Protocols.h>
#endif				/* WINTERP_MOTIF_11 */

#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"


extern Widget Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(); /* w_classes.c */


typedef struct _ShellWidgetAssoc_Node {
  LVAL				 widget_OBJ;
  Widget			 widget_ID;
  struct _ShellWidgetAssoc_Node* next;
} ShellWidgetAssoc_Node;

static ShellWidgetAssoc_Node* shell_widget_alist = NULL;


/*****************************************************************************
 * This removes the (widget_OBJ . widget_ID) association from shell_widget_alist
 * when the shell widget gets destroyed. It is called from
 * Wshl_Assoc_WidgetID_With_WIDGETOBJ() via XtAddCallback().
 ****************************************************************************/
static void Wshl_Widget_Destroy_CallbackProc(widgetID, client_data, call_data)
     Widget    widgetID;
     XtPointer client_data;	/* type is really the ShellWidgetAssoc_Node* to be destroyed */
     XtPointer call_data;
{
  register ShellWidgetAssoc_Node* cur;
  register ShellWidgetAssoc_Node* prev;
  register ShellWidgetAssoc_Node* elt_to_destroy = (ShellWidgetAssoc_Node*) client_data;

  if (!shell_widget_alist)	/* ASSERT(shell_widget_alist!=NULL): something has gone awry if this callbackproc got called but shell_widget_alist is empty. */
    xlfatal("Internal error 0 in Wshl_Widget_Destroy_CallbackProc().");

  prev = (ShellWidgetAssoc_Node*) NULL;
  cur = shell_widget_alist;
  while (cur && (cur != elt_to_destroy)) {
    prev = cur;
    cur = cur->next;
  }
  if (!prev) {			/* shell_widget_alist == cur == elt_to_destroy */
    shell_widget_alist = shell_widget_alist->next; /* pop the first elt */
    XtFree(cur);
  }
  else if (cur) {		/* cur == elt_to_destroy */
    prev->next = cur->next;	/* remove cur from alist */
    XtFree(cur);
  }
  else			/* ASSERT(cur!=NULL): something has gone awry if this callbackproc got called but can't find elt_to_destroy in shell_widget_alist. */
    xlfatal("Internal error 1 in Wshl_Widget_Destroy_CallbackProc().");
}


/*****************************************************************************
 * This associates a WIDGETOBJ with a WIDGETID and stores it on the assoc list
 * shell_widget_alist. This routine is needed to set up data for
 * Wshl_WidgetID_To_WIDGETOBJ().
 ****************************************************************************/
static void Wshl_Assoc_WidgetID_With_WIDGETOBJ(widget_id, o_widget)
     Widget widget_id;
     LVAL o_widget;
{
  ShellWidgetAssoc_Node* elt = (ShellWidgetAssoc_Node*) XtMalloc(sizeof(ShellWidgetAssoc_Node));
  elt->widget_OBJ = o_widget;
  elt->widget_ID  = widget_id;

  if (shell_widget_alist)
    elt->next = shell_widget_alist;
  else
    elt->next = NULL;		/* elt is the only one on the alist */

  shell_widget_alist = elt;	/* make elt the new head of the list */

  XtAddCallback(widget_id, XmNdestroyCallback, Wshl_Widget_Destroy_CallbackProc, (XtPointer) elt); /* this removes elt when widget is destoyed */
}


/*****************************************************************************
 * On all widgets other than shell widgets, we can retrieve the WIDGETOBJ
 * associated with a particular Xt widget_id by retrieving the XmNuserData
 * resource. Unfortunately, shell widgets do not have that resource, so we
 * have to set up a special lookup mechanism. 
 *
 * Note: this routine uses a linear search through the association list 
 * pointed at by shell_widget_alist. Since I don't expect applications to 
 * have zillions of shell widgets, I think a linear search will be adequate.
 ****************************************************************************/
LVAL Wshl_WidgetID_To_WIDGETOBJ(widget_id)
     Widget widget_id;
{
  extern void Wcls_Initialize_WIDGETOBJ(); /* w_classes.c */
  extern LVAL Wcls_WidgetClassID_To_WIDGETCLASSOBJ(); /* w_classes.c */
  extern LVAL o_WIDGET_CLASS;	/* from wc_WIDGET.c */
  register ShellWidgetAssoc_Node* cur;
  LVAL o_widgetclass, o_widget;

  /*
   * first, try to find widgetobj corresponding to widget_id by doing a linear
   * search on list created by Wshl_Assoc_WidgetID_With_WIDGETOBJ() returning
   * that WIDGETOBJ if successful
   */
  cur = shell_widget_alist;
  while (cur && (cur->widget_ID != widget_id))
    cur = cur->next;
  if (cur)			/* cur->widget_ID == widget_id */
    return (cur->widget_OBJ);

  /*
   * we couldn't find WIDGETOBJ associated with widget_id, so create a WIDGETOBJ
   * of the appropriate class.
   */
  if (o_widgetclass = Wcls_WidgetClassID_To_WIDGETCLASSOBJ(XtClass(widget_id)))	/* returns NIL on failure */
    o_widget = newobject(o_widgetclass, WIDGETOBJ_SIZE);
  else {			/* fail gracefully (i think). */
    errputstr("Warning -- In Wshl_WidgetID_To_WIDGETOBJ() couldn't find a valid\n");
    errputstr("           widgetclass object inside widget classrecord. Creating\n");
    errputstr("           a \"generic\" WIDGETOBJ of class WIDGET_CLASS.\n");
    o_widget = newobject(o_WIDGET_CLASS, WIDGETOBJ_SIZE);
  }
  
  Wcls_Initialize_WIDGETOBJ(o_widget, widget_id);
  Wshl_Assoc_WidgetID_With_WIDGETOBJ(widget_id, o_widget);
  return (o_widget);  
}



/*****************************************************************************
 * (send <Shell_Widget_Class> :new
 *                                 [<name>]
 *				   [<class-name>]
 *                                 [:XMN_<arg1> <val1>]
 *                                 [. . .             ]
 *                                 [:XMN_<argN> <valN>])
 *
 * For the <Shell_Widget_Class> metaclass,
 * create a new widget via XtAppCreateShell().
 ****************************************************************************/
LVAL Shell_Widget_Class_Method_ISNEW()
{
  extern ArgList Wres_Get_LispArglist(); /* from w_resources.c */
  extern void    Wres_Free_C_Arglist_Data(); /* from w_resources.c */
  extern WidgetClass Wcls_WIDGETCLASSOBJ_To_WidgetClassID(); /* w_classes.c */
  extern Widget   toplevel_Wgt;	/* from winterp.c -- see "NOTE POTENTIAL BUG" comment below */
  extern Display* display;	/* from winterp.c -- see "NOTE POTENTIAL BUG" comment below */
  extern char*    app_class;	/* from winterp.c -- the class name of the application (defaults to "Winterp" unless given command-line -class <classname>) */
  LVAL self;
  char* name;
  char* class_name;
  WidgetClass widgetclass_id;
  Widget widget_id;

  self = xlgaobject();		/* NOTE: xlobj.c:clnew() returns an OBJECT; if this method
				   returns successfully, it will return a WIDGETOBJ */

  /* get optional <name> arg */
  if (moreargs() && (stringp(*xlargv)))
    name = (char*) getstring(nextarg());
  else
    name = "";			/* default name */

  /* get optional <class_name> arg */
  if (moreargs() && (stringp(*xlargv)))
    class_name = (char*) getstring(nextarg());
  else
    class_name = app_class;	/* default name */

  if (!(widgetclass_id = Wcls_WIDGETCLASSOBJ_To_WidgetClassID(getclass(self))))
    xlerror("Expected a 'Class' object that is a subclass of 'WIDGET_CLASS'.", self);

  if (moreargs()) {		/* if there are more arguments, */
    Cardinal xt_numargs;	/* then we have some extra widget resources to set */
    ArgList xt_arglist = Wres_Get_LispArglist(self, toplevel_Wgt, NULL, 0, &xt_numargs);

    /* NOTE POTENTIAL BUG: above, we pass toplevel_Wgt as the widget for
       Wres_Get_LispArglist(). toplevel_Wgt is created by XtInitialize() in
       winterp.c:main(). Wres_Get_LispArglist() may need to use that widget
       for particular kinds of argument conversions done through XtConvert().
       These resource converters reference things like the display/screen/
       foreground/background. For those cases, do not attempt to invoke
       XtConvert() by passing string-valued arguments to this method. Instead,
       create the shell widget, and then use :set_values to set the arguments
       requiring such conversions. This will really break bigtime if I ever
       implement multiple AppContexts for multiscreen usage of WINTERP.
       
       Note also that below, we pass the global "display" -- this will have
       to change for multi-display use....*/

    widget_id = XtAppCreateShell(name, class_name, widgetclass_id, display, xt_arglist, xt_numargs);
    Wres_Free_C_Arglist_Data();
  }
  else
    widget_id = XtAppCreateShell(name, class_name, widgetclass_id, display, NULL, 0);

  Wcls_Initialize_WIDGETOBJ(self, widget_id);
  Wshl_Assoc_WidgetID_With_WIDGETOBJ(widget_id, self);
  
#ifdef DEBUG_WINTERP_1
  Wcls_Print_WidgetObj_Info(self);
#endif
  return (self);
}

/*****************************************************************************
 * (send APPLICATION_SHELL_WIDGET_CLASS :new
 *                                 [<name>]
 *				   [<class-name>]
 *                                 [:XMN_<arg1> <val1>]
 *                                 [. . .             ]
 *                                 [:XMN_<argN> <valN>])
 *
 * For the APPLICATION_SHELL_WIDGET_CLASScreate a new widget via
 * XtAppCreateShell().
 *
 * This is the same as the normal shell creation routine except that this
 * overrides the application shell's delete response so that WINTERP is
 * exited cleanly via wrapup().
  ****************************************************************************/
LVAL Application_Shell_Widget_Class_Method_ISNEW()
{
  extern ArgList Wres_Get_LispArglist(); /* from w_resources.c */
  extern void    Wres_Free_C_Arglist_Data(); /* from w_resources.c */
  extern WidgetClass Wcls_WIDGETCLASSOBJ_To_WidgetClassID(); /* w_classes.c */
  extern Widget   toplevel_Wgt;	/* from winterp.c -- see "NOTE POTENTIAL BUG" comment below */
  extern Display* display;	/* from winterp.c -- see "NOTE POTENTIAL BUG" comment below */
  extern char*    app_class;	/* from winterp.c -- the class name of the application (defaults to "Winterp" unless given command-line -class <classname>) */
  extern Atom     wm_delete_atom; /* from winterp.c */
  extern void     Winterp_Application_Shell_WMDelete_Callback(); /* from winterp.c */
  LVAL self;
  char* name;
  char* class_name;
  WidgetClass widgetclass_id;
  Widget widget_id;

  self = xlgaobject();		/* NOTE: xlobj.c:clnew() returns an OBJECT; if this method
				   returns successfully, it will return a WIDGETOBJ */

  /* get optional <name> arg */
  if (moreargs() && (stringp(*xlargv)))
    name = (char*) getstring(nextarg());
  else
    name = "";			/* default name */

  /* get optional <class_name> arg */
  if (moreargs() && (stringp(*xlargv)))
    class_name = (char*) getstring(nextarg());
  else
    class_name = app_class;	/* default name */

  if (!(widgetclass_id = Wcls_WIDGETCLASSOBJ_To_WidgetClassID(getclass(self))))
    xlerror("Expected a 'Class' object that is a subclass of 'WIDGET_CLASS'.", self);

  /*
   * We handle wm deletion (f.kill) w/ XmAddWMProtocolCallback() below, therefore
   * override the default delete-response of destroy...
   */
  ARGLIST_RESET(); ARGLIST_ADD(XmNdeleteResponse, XmDO_NOTHING);

  if (moreargs()) {		/* if there are more arguments, */
    Cardinal xt_numargs;	/* then we have some extra widget resources to set */
    ArgList xt_arglist = Wres_Get_LispArglist(self, toplevel_Wgt, ARGLIST(), &xt_numargs);

    /* NOTE POTENTIAL BUG: above, we pass toplevel_Wgt as the widget for
       Wres_Get_LispArglist(). toplevel_Wgt is created by XtInitialize() in
       winterp.c:main(). Wres_Get_LispArglist() may need to use that widget
       for particular kinds of argument conversions done through XtConvert().
       These resource converters reference things like the display/screen/
       foreground/background. For those cases, do not attempt to invoke
       XtConvert() by passing string-valued arguments to this method. Instead,
       create the shell widget, and then use :set_values to set the arguments
       requiring such conversions. This will really break bigtime if I ever
       implement multiple AppContexts for multiscreen usage of WINTERP.
       
       Note also that below, we pass the global "display" -- this will have
       to change for multi-display use....*/

    widget_id = XtAppCreateShell(name, class_name, widgetclass_id, display, xt_arglist, xt_numargs);
    Wres_Free_C_Arglist_Data();
  }
  else
    widget_id = XtAppCreateShell(name, class_name, widgetclass_id, display, ARGLIST());

  XmAddWMProtocolCallback(widget_id, wm_delete_atom, Winterp_Application_Shell_WMDelete_Callback, NULL);

  Wcls_Initialize_WIDGETOBJ(self, widget_id);
  Wshl_Assoc_WidgetID_With_WIDGETOBJ(widget_id, self);
  
#ifdef DEBUG_WINTERP_1
  Wcls_Print_WidgetObj_Info(self);
#endif
  return (self);
}


/*****************************************************************************
 * (send <Popup_Shell_Widget_Class> :new
 *                                       [<name>]
 *                                       <parent> 
 *                                       [:XMN_<arg1> <val1>]
 *                                       [. . .             ]
 *                                       [:XMN_<argN> <valN>])
 *
 * For the <Popup_Shell_Widget_Class> meta-class,
 * create a new widget via XtCreatePopupShell().
 ****************************************************************************/
LVAL Popup_Shell_Widget_Class_Method_ISNEW()
{
  extern ArgList Wres_Get_LispArglist(); /* from w_resources.c */
  extern void    Wres_Free_C_Arglist_Data(); /* from w_resources.c */
  extern WidgetClass Wcls_WIDGETCLASSOBJ_To_WidgetClassID(); /* w_classes.c */
  LVAL self;
  LVAL o_parent;
  char* name;
  WidgetClass widgetclass_id;
  Widget parent_widget_id, widget_id;

  self = xlgaobject();		/* NOTE: xlobj.c:clnew() returns an OBJECT; if this method
				   returns successfully, it will return a WIDGETOBJ */

  /* get optional <name> arg */
  if (moreargs() && (stringp(*xlargv)))
    name = (char*) getstring(nextarg());
  else
    name = "";			/* default name */

  /* get required <parent> widget-object arg */
  parent_widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&o_parent);

  if (!(widgetclass_id = Wcls_WIDGETCLASSOBJ_To_WidgetClassID(getclass(self))))
    xlerror("Expected a 'Class' object that is a subclass of 'WIDGET_CLASS'.", self);

  if (moreargs()) {		/* if there are more arguments, */
    Cardinal xt_numargs;	/* then we have some extra widget resources to set */
    ArgList xt_arglist = Wres_Get_LispArglist(self, parent_widget_id, NULL, 0, &xt_numargs);
    widget_id = XtCreatePopupShell(name, widgetclass_id, parent_widget_id, xt_arglist, xt_numargs);
    Wres_Free_C_Arglist_Data();
  }
  else
    widget_id = XtCreatePopupShell(name, widgetclass_id, parent_widget_id, NULL, 0);

  Wcls_Initialize_WIDGETOBJ(self, widget_id);
  Wshl_Assoc_WidgetID_With_WIDGETOBJ(widget_id, self);

#ifdef DEBUG_WINTERP_1
  Wcls_Print_WidgetObj_Info(self);
#endif
  return (self);
}


/*****************************************************************************
 * (send <Popup_Shell_Widget_Instance> :popup <grabkind>)
 * where <grabkind> is one of the following keyword symbols: 
 *                  :grab_none, :grab_nonexclusive, or  :grab_exclusive.
 ****************************************************************************/
static LVAL k_grab_none;	/* initialized in Wc_SHELL_Init() */
static LVAL k_grab_nonexclusive; /* initialized in Wc_SHELL_Init() */
static LVAL k_grab_exclusive;	/* initialized in Wc_SHELL_Init() */
LVAL Popup_Shell_Widget_Class_Method_POPUP()
{
  LVAL self, grabkind;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  grabkind = xlgasymbol();
  xllastarg();

  if (grabkind == k_grab_none)
    XtPopup(widget_id, XtGrabNone);
  else if (grabkind == k_grab_nonexclusive)
    XtPopup(widget_id, XtGrabNonexclusive);
  else if (grabkind == k_grab_exclusive)
    XtPopup(widget_id, XtGrabExclusive);
  else
    xlerror("Unknown <grabkind> symbol. (Expected :grab_none, :grab_nonexclusive, or  :grab_exclusive)", grabkind);

  return (self);
}


/*****************************************************************************
 * (send <Popup_Shell_Widget_Instance> :popdown)
 ****************************************************************************/
LVAL Popup_Shell_Widget_Class_Method_POPDOWN()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();
  
  XtPopdown(widget_id);
  return (self);
}


/*****************************************************************************
 * (send <Shell_Widget_Instance> :realize)
 ****************************************************************************/
LVAL Shell_Widget_Class_Method_REALIZE()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  XtRealizeWidget(widget_id);
  return (self);
}


/*****************************************************************************
 * (send <Shell_Widget_Instance> :unrealize)
 ****************************************************************************/
LVAL Shell_Widget_Class_Method_UNREALIZE()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  XtUnrealizeWidget(widget_id);
  return (self);
}


/******************************************************************************
 * (send <Shell_Widget_Instance> :IS_MOTIF_WM_RUNNING)
 * returns T if mwm is running, else NIL.
 *
 * extern Boolean XmIsMotifWMRunning(shell);
 *        Widget shell;
 *******************************************************************************/
LVAL Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  return (XmIsMotifWMRunning(widget_id) ? true : NIL);
}

/******************************************************************************
 * (send <Application_Shell_Widget_Instance> :GET_ARGV)
 * 	==> returns an array of strings.
 *
 * This retrieves the resources XmNargv and XmNargc from <Application_Shell_Widget_Instance>
 * and returns an array of strings representing the command line args with
 * which the program was called.
 *******************************************************************************/
LVAL Application_Shell_Widget_Class_Method_GET_ARGV()
{
  LVAL self, result;
  Widget widget_id;
  String* retrieved_argv;
  int     retrieved_argc, i;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <Application_Shell_Widget_Instance>  */
  xllastarg();

  ARGLIST_RESET();
  ARGLIST_ADD(XmNargc, &retrieved_argc);
  ARGLIST_ADD(XmNargv, &retrieved_argv);
  XtGetValues(widget_id, ARGLIST());

  if (retrieved_argc == 0)
    return (NIL);
  if (retrieved_argv == NULL)
    return (NIL);
  
  xlsave1(result);

  result = newvector(retrieved_argc);

  for (i = 0; i < retrieved_argc; i++)
    setelement(result, i, cvstring(retrieved_argv[i]));	/* cvstring() copies the string. This copy may be freed later via garbage collection. */

  xlpop();
  return (result);
}


/******************************************************************************
 * (send <Application_Shell_Widget_Instance> :SET_ARGV <args>)
 * returns <Application_Shell_Widget_Instance>.
 *
 * This sets the XmNargv and XmNargc resources based on the sequence of string
 * arguments <args>. The resources are set on <Application_Shell_Widget_Instance> 
 * and this supposedly will allow session managers that the application should
 * start up with the new <args> specified here.
 *******************************************************************************/
#define STRINGTABLE_SIZE_INCREMENT 20
LVAL Application_Shell_Widget_Class_Method_SET_ARGV()
{
  LVAL self, lval_args, elt;
  Widget widget_id;
  String string_from_lisp, string_to_c;
  int	 string_length, i;
  String* set_argv = NULL;
  int     set_argc = 0;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <Application_Shell_Widget_Instance>  */
  lval_args = xlgetarg();	/* get <args> */
  xllastarg();

  /*
   * if argument is a vector, then step through array of lisp <string>.
   */
  if (vectorp(lval_args)) {
    set_argc = getsize(lval_args);
    set_argv = (String*) XtMalloc((unsigned) ((set_argc + 1) * sizeof(String)));

    for (i = 0 ; i < set_argc ; i++) {
      elt = getelement(lval_args, i);
      if (stringp(elt)) {
	if (string_from_lisp = (String) getstring(elt)) {
	  string_length = getslength(elt);
	  string_to_c = (String) XtMalloc((unsigned) (string_length + 1) * sizeof(char)); /* unpluggable memory leak -- XmNargv doesn't get copied in widget... */
	  string_to_c = strncpy(string_to_c, string_from_lisp, string_length);
	  string_to_c[string_length] = '\000';
	  set_argv[i] = string_to_c;
	}
	else
	  set_argv[i] = "";
      }
      else {
	XtFree(set_argv);
	xlerror("Expected as argument a sequence (list or vector) of strings; encountered a non-string array element.", elt);
      }
    }
    set_argv[i] = NULL;		/* null terminate */
    /* implicit: set_argc = i; */
  }
  /*
   * if argument is a list, then cdr through list of lisp <string>
   */
  else if (consp(lval_args)) {
    int size = STRINGTABLE_SIZE_INCREMENT;
    set_argv = (String*) XtMalloc((unsigned) ((size + 1) * sizeof(String)));

    for (i = 0 ; (consp(lval_args)) ; lval_args = cdr(lval_args), i++) {
      if (i >= size) {		/* make sure it'll fit into allocated set_argv */
	size += STRINGTABLE_SIZE_INCREMENT;
	set_argv = (String*) XtRealloc(set_argv, (unsigned) ((size + 1) * sizeof(String)));
      }
      elt = car(lval_args);
      if (stringp(elt)) {
	if (string_from_lisp = (String) getstring(elt)) {
	  string_length = strlen(string_from_lisp);
	  string_to_c = (String) XtMalloc((unsigned) (string_length + 1) * sizeof(char)); /* unpluggable memory leak -- XmNargv doesn't get copied in widget... */
	  string_to_c = strncpy(string_to_c, string_from_lisp, string_length);
	  string_to_c[string_length] = '\000';
	  set_argv[i] = string_to_c;
	}
	else
	  set_argv[i] = "";
      }
      else {
	XtFree(set_argv);
	xlerror("Expected as argument a sequence (list or vector) of strings; encountered a non-string list element.", elt);
      }
    }

    if (lval_args) {		/* if loop terminated due to list pointer not being a CONS cell */
      XtFree(set_argv);
      xlerror("Expected as argument a sequence (list or vector) of strings; encountered a weird list tail.", lval_args);
    }

    set_argv[i] = NULL;		/* null terminate */
    set_argc = i;
  }
  /*
   * if argument wasn't list or vector, then error
   */
  else
    xlerror("Wrong argument type given; Expected as argument a sequence (list or vector) of strings." , lval_args);

  ARGLIST_RESET();
  ARGLIST_ADD(XmNargc, (XtArgVal) set_argc);
  ARGLIST_ADD(XmNargv, (XtArgVal) set_argv);
  XtSetValues(widget_id, ARGLIST());

  /*
   * Normally, we'd want to free set_argv after passing it to the widget via XtSetValues().
   * However, Motif/Xt has a bug in that if you free argv before XtRealize() gets called
   * you'll get a core dump. Therefore, I don't free argv. On the other hand, this
   * definitely causes a memory leak, but the fault lies in the Xtoolkit and Motif.
   */
#ifdef THE_FOLLOWING_CODE_IS_COMMENTED_OUT
  if (set_argv)
    XtFree(set_argv);
#endif				/* THE_FOLLOWING_CODE_IS_COMMENTED_OUT */


  return (self);
}



/******************************************************************************
 *
 ******************************************************************************/
Wc_SHELL_Init()
{
  LVAL o_TOP_LEVEL_SHELL_WIDGET_CLASS;
  LVAL o_TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS;
  LVAL o_APPLICATION_SHELL_WIDGET_CLASS;
  LVAL o_APPLICATION_POPUP_SHELL_WIDGET_CLASS;
  LVAL o_OVERRIDE_SHELL_WIDGET_CLASS;
  LVAL o_OVERRIDE_POPUP_SHELL_WIDGET_CLASS;
  LVAL o_TRANSIENT_SHELL_WIDGET_CLASS;
  LVAL o_TRANSIENT_POPUP_SHELL_WIDGET_CLASS;
  LVAL o_XM_DIALOG_POPUP_SHELL_WIDGET_CLASS;
  LVAL o_XM_MENU_POPUP_SHELL_WIDGET_CLASS;
  extern LVAL Wcls_Create_Subclass_Of_WIDGET_CLASS(); /* w_classes.c */
  extern      xladdmsg();	/* from xlobj.c */

  k_grab_none          = xlenter(":GRAB_NONE");
  k_grab_nonexclusive  = xlenter(":GRAB_NONEXCLUSIVE");
  k_grab_exclusive     = xlenter(":GRAB_EXCLUSIVE");

  /*------------------- TOP_LEVEL_SHELL_WIDGET_CLASS -----------------------*/
  o_TOP_LEVEL_SHELL_WIDGET_CLASS = 
    Wcls_Create_Subclass_Of_WIDGET_CLASS("TOP_LEVEL_SHELL_WIDGET_CLASS",
					 topLevelShellWidgetClass);
  xladdmsg(o_TOP_LEVEL_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_TOP_LEVEL_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_TOP_LEVEL_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_TOP_LEVEL_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*------------------- TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS ------------------*/
  o_TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS = 
    Wcls_Create_Subclass_Of_WIDGET_CLASS("TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS",
					 topLevelShellWidgetClass);

  xladdmsg(o_TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS, ":POPUP",
	   FTAB_Popup_Shell_Widget_Class_Method_POPUP);
  xladdmsg(o_TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS, ":POPDOWN",
	   FTAB_Popup_Shell_Widget_Class_Method_POPDOWN);
  xladdmsg(o_TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Popup_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*------------------- APPLICATION_SHELL_WIDGET_CLASS -----------------------*/
  o_APPLICATION_SHELL_WIDGET_CLASS = 
    Wcls_Create_Subclass_Of_WIDGET_CLASS("APPLICATION_SHELL_WIDGET_CLASS",
					 applicationShellWidgetClass);
  xladdmsg(o_APPLICATION_SHELL_WIDGET_CLASS, ":GET_ARGV", 
	   FTAB_Application_Shell_Widget_Class_Method_GET_ARGV);
  xladdmsg(o_APPLICATION_SHELL_WIDGET_CLASS, ":SET_ARGV", 
	   FTAB_Application_Shell_Widget_Class_Method_SET_ARGV);
  xladdmsg(o_APPLICATION_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_APPLICATION_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  /*
   * note special initializer method which sets up f.kill handler
   * so that WINTERP exits cleanly.
   */
  xladdmsg(o_APPLICATION_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Application_Shell_Widget_Class_Method_ISNEW); 
  xladdmsg(o_APPLICATION_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*------------------- APPLICATION_POPUP_SHELL_WIDGET_CLASS -----------------------*/
  o_APPLICATION_POPUP_SHELL_WIDGET_CLASS = 
    Wcls_Create_Subclass_Of_WIDGET_CLASS("APPLICATION_POPUP_SHELL_WIDGET_CLASS",
					 applicationShellWidgetClass);
  xladdmsg(o_APPLICATION_POPUP_SHELL_WIDGET_CLASS, ":GET_ARGV", 
	   FTAB_Application_Shell_Widget_Class_Method_GET_ARGV);
  xladdmsg(o_APPLICATION_POPUP_SHELL_WIDGET_CLASS, ":SET_ARGV", 
	   FTAB_Application_Shell_Widget_Class_Method_SET_ARGV);
  xladdmsg(o_APPLICATION_POPUP_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_APPLICATION_POPUP_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_APPLICATION_POPUP_SHELL_WIDGET_CLASS, ":POPUP",
	   FTAB_Popup_Shell_Widget_Class_Method_POPUP);
  xladdmsg(o_APPLICATION_POPUP_SHELL_WIDGET_CLASS, ":POPDOWN",
	   FTAB_Popup_Shell_Widget_Class_Method_POPDOWN);
  xladdmsg(o_APPLICATION_POPUP_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Popup_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_APPLICATION_POPUP_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*------------------- OVERRIDE_SHELL_WIDGET_CLASS -----------------------*/
  o_OVERRIDE_SHELL_WIDGET_CLASS = 
    Wcls_Create_Subclass_Of_WIDGET_CLASS("OVERRIDE_SHELL_WIDGET_CLASS",
					 overrideShellWidgetClass);
  xladdmsg(o_OVERRIDE_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_OVERRIDE_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_OVERRIDE_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_OVERRIDE_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*------------------- OVERRIDE_POPUP_SHELL_WIDGET_CLASS -----------------------*/
  o_OVERRIDE_POPUP_SHELL_WIDGET_CLASS = 
    Wcls_Create_Subclass_Of_WIDGET_CLASS("OVERRIDE_POPUP_SHELL_WIDGET_CLASS",
					 overrideShellWidgetClass);
  xladdmsg(o_OVERRIDE_POPUP_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_OVERRIDE_POPUP_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_OVERRIDE_POPUP_SHELL_WIDGET_CLASS, ":POPUP",
	   FTAB_Popup_Shell_Widget_Class_Method_POPUP);
  xladdmsg(o_OVERRIDE_POPUP_SHELL_WIDGET_CLASS, ":POPDOWN",
	   FTAB_Popup_Shell_Widget_Class_Method_POPDOWN);
  xladdmsg(o_OVERRIDE_POPUP_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Popup_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_OVERRIDE_POPUP_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*------------------- TRANSIENT_SHELL_WIDGET_CLASS -----------------------*/
  o_TRANSIENT_SHELL_WIDGET_CLASS = 
    Wcls_Create_Subclass_Of_WIDGET_CLASS("TRANSIENT_SHELL_WIDGET_CLASS",
					 transientShellWidgetClass);
  xladdmsg(o_TRANSIENT_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_TRANSIENT_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_TRANSIENT_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_TRANSIENT_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*------------------- TRANSIENT_POPUP_SHELL_WIDGET_CLASS -----------------------*/
  o_TRANSIENT_POPUP_SHELL_WIDGET_CLASS = 
    Wcls_Create_Subclass_Of_WIDGET_CLASS("TRANSIENT_POPUP_SHELL_WIDGET_CLASS",
					 transientShellWidgetClass);
  xladdmsg(o_TRANSIENT_POPUP_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_TRANSIENT_POPUP_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_TRANSIENT_POPUP_SHELL_WIDGET_CLASS, ":POPUP",
	   FTAB_Popup_Shell_Widget_Class_Method_POPUP);
  xladdmsg(o_TRANSIENT_POPUP_SHELL_WIDGET_CLASS, ":POPDOWN",
	   FTAB_Popup_Shell_Widget_Class_Method_POPDOWN);
  xladdmsg(o_TRANSIENT_POPUP_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Popup_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_TRANSIENT_POPUP_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*-------------------- XM_DIALOG_POPUP_SHELL_WIDGET_CLASS ------------------------*/
  o_XM_DIALOG_POPUP_SHELL_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_DIALOG_POPUP_SHELL_WIDGET_CLASS",
					 xmDialogShellWidgetClass);
  xladdmsg(o_XM_DIALOG_POPUP_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_XM_DIALOG_POPUP_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_XM_DIALOG_POPUP_SHELL_WIDGET_CLASS, ":POPUP",
	   FTAB_Popup_Shell_Widget_Class_Method_POPUP);
  xladdmsg(o_XM_DIALOG_POPUP_SHELL_WIDGET_CLASS, ":POPDOWN",
	   FTAB_Popup_Shell_Widget_Class_Method_POPDOWN);
  xladdmsg(o_XM_DIALOG_POPUP_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Popup_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_XM_DIALOG_POPUP_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*-------------------- XM_MENU_POPUP_SHELL_WIDGET_CLASS ------------------------*/
  o_XM_MENU_POPUP_SHELL_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_MENU_POPUP_SHELL_WIDGET_CLASS",
					 xmMenuShellWidgetClass);
  xladdmsg(o_XM_MENU_POPUP_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_XM_MENU_POPUP_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_XM_MENU_POPUP_SHELL_WIDGET_CLASS, ":POPUP",
	   FTAB_Popup_Shell_Widget_Class_Method_POPUP);
  xladdmsg(o_XM_MENU_POPUP_SHELL_WIDGET_CLASS, ":POPDOWN",
	   FTAB_Popup_Shell_Widget_Class_Method_POPDOWN);
  xladdmsg(o_XM_MENU_POPUP_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Popup_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_XM_MENU_POPUP_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);
}
