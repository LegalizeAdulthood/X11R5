/* -*-C-*-
********************************************************************************
*
* File:         wc_List.c
* RCS:          $Header: wc_List.c,v 1.4 91/03/14 03:14:46 mayer Exp $
* Description:  XM_LIST_WIDGET_CLASS
* Author:       Niels Mayer, HPLabs
* Created:      Sat Oct 28 03:28:28 1989
* Modified:     Thu Oct  3 23:40:23 1991 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: wc_List.c,v 1.4 91/03/14 03:14:46 mayer Exp $";

/*
 * <limits.h> defines machine dependent limits on sizes of numbers, if your
 * machine doesn't have this, then your compiler doesn't conform to standards
 * XPG2, XPG3, POSIX.1, FIPS 151-1 and you should complain to the manufacturer.
 * 
 * If for some reason your system isn't standards-conforming, you may work
 * around this problem by using the following definitions (assuming 32 bit machine):
 * 
 * #define INT_MAX 2147483647
 */
#include <limits.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/List.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"
#include "w_XmString.h"


extern Widget Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(); /* w_classes.c */


/******************************************************************************/
static int Get_ListPosition_Argument()
{
  LVAL lval_position;
  long position;

  lval_position = xlgafixnum();
  position = (long) getfixnum(lval_position);
  if (position < 0L)
    xlerror("List Position/Count argument must be a fixnum >= 0.", lval_position);
  if (position > (long) INT_MAX)
    xlerror("List Position/Count argument too large.", lval_position);
  
  return ((int) position);
}

/******************************************************************************/
static SuperXmStringTable Get_StringTable_or_XmStringTable_Arg_Returning_SuperXmStringTable()
{
  LVAL lisp_val = xlgetarg();
  return (Wxms_Cvt_LispStringSequence_to_SuperXmStringTable(lisp_val));	/* must call Wxms_Free_SuperXmStringTable() when done w/ result */
}


/******************************************************************************/
static LVAL IntList_To_Lisp_Vector(intlist, size)
     int* intlist;
     int  size;
{
  LVAL result;
  int i;
  
  if (!intlist || !size)
    return (NIL);

  xlsave1(result);
  result = newvector(size);
  for (i = 0; i < size; i++)
    setelement(result, i, cvfixnum((FIXTYPE) intlist[i]));
  xlpop();
  return (result);
}


/*****************************************************************************
 * (send XM_LIST_WIDGET_CLASS :new
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
 *     (send XM_LIST_WIDGET_CLASS :new ...)
 *     --> XmCreateList();
 *     (send XM_LIST_WIDGET_CLASS :new :scrolled ...)
 *     --> XmCreateScrolledList();
 *          Note: extra convenience fn XmCreateScrolledList() puts the
 *	  list widget inside a scrolled window but returns the list widget.
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_ISNEW()
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
    scrolled_p = FALSE;		/* by default, we don't want a scroled list */

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
      widget_id = XmCreateScrolledList(parent_widget_id, name, xt_arglist, xt_numargs);
    else
      widget_id = XmCreateList(parent_widget_id, name, xt_arglist, xt_numargs);
    Wres_Free_C_Arglist_Data();
  }
  else 
    if (scrolled_p)
      widget_id = XmCreateScrolledList(parent_widget_id, name, ARGLIST());
    else
      widget_id = XmCreateList(parent_widget_id, name, ARGLIST());

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
 * (send <listwidget> :PARENT)
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
 * wc_Text.c:Xm_Text_Widget_Class_Method_PARENT()
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_PARENT()
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


/*****************************************************************************
 * (send <listwidget> :ADD_ITEM <item> <position>)
 *	==> returns the new item, as an XmString.
 * This method adds <item> to <listwidget> at the given position.
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString and that XmString is returned.
 * <position> is an positive fixnum. Specifying 0 will add the item to the end
 *	of the list, 1 makes it the first item, 2 the second, etc.
 *
 * void XmListAddItem(w, item, pos)
 * Widget w;
 * XmString item;
 * int pos;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_ADD_ITEM()
{
  LVAL self, lval_item;
  int position;
  Widget widget_id;
  XmString str;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);	/* get <listwidget> */
  str = Get_String_or_XmString_Arg_Returning_XmString(&lval_item); /* get <item> */
  position = Get_ListPosition_Argument(); /* get <position> */
  xllastarg();

  XmListAddItem(widget_id, str, position);

  /* note: XmListAddItem() creates a copy of the XmString <item>, allowing us to
     free str whenever we want. If <item> was passed in as an XmString, the
     XmString will be freed via garbage collection whenever <item> is no longer
     referenced. If <item> was passed in as a normal string, then a new
     XmString object is created in this method -- it will be freed when it
     gets garbage collected.
     */
  return (lval_item);		/* return the XmString item. */
}


#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <listwidget> :ADD_ITEMS <item-seq> <position>)
 *	==> returns <listwidget>
 *
 * <item-seq> is a list or array of strings or XmStrings. Note that if 
 * strings are given, they will automatically be converted to XmStrings
 * before being passed to <listwidget>.
 *
 * <position> is an positive fixnum. Specifying 0 will add the item to the end
 *	of the list, 1 makes it the first item, 2 the second, etc.
 * -------------------------------------------------------------------------
 * void XmListAddItems (Widget w, XmString *items, int item_count, int pos);
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_ADD_ITEMS()
{
  LVAL self;
  int position;
  Widget widget_id;
  SuperXmStringTable superstrtab;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  superstrtab = Get_StringTable_or_XmStringTable_Arg_Returning_SuperXmStringTable(); /* get <item-seq> */
  position = Get_ListPosition_Argument(); /* get <position> */
  xllastarg();

  XmListAddItems(widget_id, superstrtab->xmstrtab, superstrtab->xmstrtab_end_idx, position);

  /* note: XmListAddItems() creates a copy of the XmStrings in <item-seq>, allowing us to
     free all XmStrings created. */
  Wxms_Free_SuperXmStringTable(superstrtab); /* dealloc any XmStrings created by autoconversion in Get_StringTable_or_XmStringTable_Arg_Returning_SuperXmStringTable() */

  return (self);		/* <listwidget> */
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <listwidget> :ADD_ITEM_UNSELECTED <item> <position>)
 *	==> returns <item> as an XmString.
 * This method adds <item> to <listwidget> at the given position and ensures
 * that <item> is not selected.
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString and that XmString is returned by the method.
 * <position> is an positive fixnum. Specifying 0 will add the item to the end
 *	of the list, 1 makes it the first item, 2 the second, etc.
 *
 * void XmListAddItemUnselected(w, item, pos)
 *     Widget w;
 *     XmString item;
 *     int	     pos;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_ADD_ITEM_UNSELECTED()
{
  LVAL self, lval_item;
  int position;
  Widget widget_id;
  XmString str;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  str = Get_String_or_XmString_Arg_Returning_XmString(&lval_item); /* get <item> */
  position = Get_ListPosition_Argument(); /* get <position> */
  xllastarg();

  XmListAddItemUnselected(widget_id, str, position);

  /* note: XmListAddItemUnselected() creates a copy of the XmString <item>,
     allowing us to free str whenever we want. If <item> was passed in as an
     XmString, the XmString will be freed via garbage collection whenever <item>
     is no longer referenced. If <item> was passed in as a normal string, then a
     new XmString object is created in this method -- it will be freed when it
     gets garbage collected.  */
  return (lval_item);		/* return the XmString item. */
}


/*****************************************************************************
 * (send <listwidget> :DELETE_ITEM <item>)
 *	==> returns <item> as an XmString
 * This method deletes the specified <item> from <listwidget>, giving
 * an XtWarning if <item> doesn't exist.
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString and that XmString is returned by the method.
 *
 * void XmListDeleteItem(w, item)
 *     Widget w;
 *     XmString item;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_DELETE_ITEM()
{
  LVAL self, lval_item;
  Widget widget_id;
  XmString str;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  str = Get_String_or_XmString_Arg_Returning_XmString(&lval_item); /* get <item> */
  xllastarg();

  XmListDeleteItem(widget_id, str);

  /* note: XmListDeleteItem() creates a copy of the XmString <item>,
     allowing us to free str whenever we want. If <item> was passed in as an
     XmString, the XmString will be freed via garbage collection whenever <item>
     is no longer referenced. If <item> was passed in as a normal string, then a
     new XmString object is created in this method -- it will be freed when it
     gets garbage collected.  */
  return (lval_item);		/* return the XmString item. */
}


#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <listwidget> :DELETE_ITEMS <item-seq>)
 *	==> returns <listwidget>
 *
 * <item-seq> is a list or array of strings or XmStrings. Note that if 
 * strings are given, they will automatically be converted to XmStrings
 * before being passed to <listwidget>.
 *
 * This method deletes the specified <item-seq> from <listwidget>, giving
 * an XtWarning if <item> doesn't exist.
 * --------------------------------------------------------------------------
 * void XmListDeleteItems (Widget w, XmString *items, int item_count);
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_DELETE_ITEMS()
{
  LVAL self;
  Widget widget_id;
  SuperXmStringTable superstrtab;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  superstrtab = Get_StringTable_or_XmStringTable_Arg_Returning_SuperXmStringTable(); /* get <item-seq> */
  xllastarg();

  XmListDeleteItems(widget_id, superstrtab->xmstrtab, superstrtab->xmstrtab_end_idx);

  /* note: XmListDeleteItems() creates a copy of the XmStrings in <item-seq>, allowing us to
     free all XmStrings created. */
  Wxms_Free_SuperXmStringTable(superstrtab); /* dealloc any XmStrings created by autoconversion in Get_StringTable_or_XmStringTable_Arg_Returning_SuperXmStringTable() */

  return (self);
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <listwidget> :DELETE_POS <position>)
 *	==> returns <listwidget>.
 * This method deletes the item at <position> from <listwidget>. A position
 * of zero deletes the last item on the list. Gives an XtWarning if the 
 * item doesn't exist.
 * <position> is an positive fixnum. 
 *
 * void XmListDeletePos(w, pos)
 *     Widget w;
 *     int	pos;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_DELETE_POS()
{
  LVAL self;
  int position;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  position = Get_ListPosition_Argument(); /* get <position> */
  xllastarg();

  XmListDeletePos(widget_id, position);

  return (self);
}


#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <listwidget> :DELETE_ITEMS_POS <position> <item-count>)
 *	==> returns <listwidget>.
 * This method deletes the items at <position> from <listwidget>. The number of
 * items to delete at <position> is specified by <item-count>. A position
 * of zero deletes the last item on the list. Gives an XtWarning if the 
 * item doesn't exist.
 * <position> is an positive fixnum. 
 * <item-count> is an positive fixnum. 
 *
 * --------------------------------------------------------------------------
 * void XmListDeleteItemsPos (Widget w, int item_count, int pos);
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_DELETE_ITEMS_POS()
{
  LVAL self;
  int position, item_count;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  position = Get_ListPosition_Argument(); /* get <position> */
  item_count = Get_ListPosition_Argument(); /* get <item-count> */
  xllastarg();

  XmListDeleteItemsPos(widget_id, item_count, position);

  return (self);
}


/*****************************************************************************
 * (send <listwidget> :DELETE_ALL_ITEMS)
 *	==> returns <listwidget>.
 * --------------------------------------------------------------------------
 * void XmListDeleteAllItems (Widget w);
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_DELETE_ALL_ITEMS()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  xllastarg();

  XmListDeleteAllItems(widget_id);

  return (self);
}


/*****************************************************************************
 * (send <listwidget> :REPLACE_ITEMS <old-item-seq> <new-item-seq>)
 *	==> returns <listwidget>
 *
 * <old-item-seq> and <new-item-seq> is a list or array of strings or XmStrings.
 * Note that if  strings are given, they will automatically be converted to
 * XmStrings before being passed to <listwidget>.
 *
 * -------------------------------------------------------------------------
 * void XmListReplaceItems (Widget w, XmString *old_items, int item_count, XmString *new_items);
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_REPLACE_ITEMS()
{
  LVAL self;
  Widget widget_id;
  SuperXmStringTable old_superstrtab;
  SuperXmStringTable new_superstrtab;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  old_superstrtab = Get_StringTable_or_XmStringTable_Arg_Returning_SuperXmStringTable(); /* get <old-item-seq> */
  new_superstrtab = Get_StringTable_or_XmStringTable_Arg_Returning_SuperXmStringTable(); /* get <new-item-seq> */
  xllastarg();
  
  XmListReplaceItems(widget_id, old_superstrtab->xmstrtab, old_superstrtab->xmstrtab_end_idx, new_superstrtab->xmstrtab);

  /* note: XmListReplaceItems() creates a copy of the XmStrings in <item-seq>,
     allowing us to free all XmStrings created. */
  Wxms_Free_SuperXmStringTable(old_superstrtab); /* this deallocs any XmStrings created by autoconversion in Get_StringTable_or_XmStringTable_Arg_Returning_SuperXmStringTable() */
  Wxms_Free_SuperXmStringTable(new_superstrtab); /* this deallocs any XmStrings created by autoconversion in Get_StringTable_or_XmStringTable_Arg_Returning_SuperXmStringTable() */

  return (self);		/* <listwidget> */
}


/*****************************************************************************
 * (send <listwidget> :REPLACE_ITEMS_POS <item-seq> <position>)
 *	==> returns <listwidget>
 *
 * This method replaces the items beginning at <position> with the items
 * specified in <item-seq>. <item-seq> is a list or array of strings or
 * XmStrings. Note that if  strings are given, they will automatically be
 * converted to XmStrings before being passed to <listwidget>.
 *
 * -------------------------------------------------------------------------
 * void XmListReplaceItemsPos (Widget w, XmString *new_items, int item_count, int position);
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_REPLACE_ITEMS_POS()
{
  LVAL self;
  int position;
  Widget widget_id;
  SuperXmStringTable superstrtab;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  superstrtab = Get_StringTable_or_XmStringTable_Arg_Returning_SuperXmStringTable(); /* get <item-seq> */
  position = Get_ListPosition_Argument(); /* get <position> */
  xllastarg();

  XmListReplaceItemsPos(widget_id, superstrtab->xmstrtab, superstrtab->xmstrtab_end_idx, position);

  /* note: XmListReplaceItemsPos() creates a copy of the XmStrings in <item-seq>,
     allowing us to free all XmStrings created. */
  Wxms_Free_SuperXmStringTable(superstrtab); /* this deallocs any XmStrings created by autoconversion in Get_StringTable_or_XmStringTable_Arg_Returning_SuperXmStringTable() */

  return (self);		/* <listwidget> */
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <listwidget> :SELECT_ITEM <item> [<notify_p>])
 *	==> returns <item> as an XmString
 * This method adds <item> to the selected items list and highlights it.
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString and that XmString is returned by the method.
 * <notify_p> is an optional boolean parameter. If non-NIL, it will issue a
 *	callback indicating an item has been selected.
 *
 * void XmListSelectItem(w, item, notify)
 *     Widget w;
 *     XmString item;
 *     Boolean notify;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_SELECT_ITEM()
{
  LVAL self, lval_item;
  Widget widget_id;
  XmString str;
  Boolean notify_p;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  str = Get_String_or_XmString_Arg_Returning_XmString(&lval_item); /* get <item> */
  if (moreargs())		/* get optional <notify_p> */
    notify_p = nextarg() ? TRUE : FALSE;
  else
    notify_p = FALSE;
  xllastarg();

  XmListSelectItem(widget_id, str, notify_p);

  /* note: XmListSelectItem() creates a copy of the XmString <item>,
     allowing us to free str whenever we want. If <item> was passed in as an
     XmString, the XmString will be freed via garbage collection whenever <item>
     is no longer referenced. If <item> was passed in as a normal string, then a
     new XmString object is created in this method -- it will be freed when it
     gets garbage collected.  */
  return (lval_item);		/* return the XmString item. */
}


/*****************************************************************************
 * (send <listwidget> :SELECT_POS <position> [<notify_p>])
 *	==> returns <listwidget>.
 * This method adds the item corresponding to <position> to the selected
 * items list and highlights it.
 * <position> is an positive fixnum. 
 * <notify_p> is an optional boolean parameter. If non-NIL, it will issue a
 *	callback indicating an item has been selected.
 *
 * void XmListSelectPos(w, pos, notify)
 *     Widget w;
 *     int	pos;
 *     Boolean notify;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_SELECT_POS()
{
  LVAL self;
  int position;
  Widget widget_id;
  Boolean notify_p;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  position = Get_ListPosition_Argument(); /* get <position> */
  if (moreargs())		/* get optional <notify_p> */
    notify_p = nextarg() ? TRUE : FALSE;
  else
    notify_p = FALSE;
  xllastarg();

  XmListSelectPos(widget_id, position, notify_p);

  return (self);
}


/*****************************************************************************
 * (send <listwidget> :DESELECT_ITEM <item>)
 *	==> returns <item> as an XmString
 * This method removes the specified <item> from the selected list and
 * unhighlights it.
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString and that XmString is returned by the method.
 *
 * void XmListDeselectItem(w, item)
 *     Widget w;
 *     XmString item;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_DESELECT_ITEM()
{
  LVAL self, lval_item;
  Widget widget_id;
  XmString str;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  str = Get_String_or_XmString_Arg_Returning_XmString(&lval_item); /* get <item> */
  xllastarg();

  XmListDeselectItem(widget_id, str);

  /* note: XmListDeselectItem() creates a copy of the XmString <item>,
     allowing us to free str whenever we want. If <item> was passed in as an
     XmString, the XmString will be freed via garbage collection whenever <item>
     is no longer referenced. If <item> was passed in as a normal string, then a
     new XmString object is created in this method -- it will be freed when it
     gets garbage collected.  */
  return (lval_item);		/* return the XmString item. */
}


/*****************************************************************************
 * (send <listwidget> :DESELECT_POS <position>)
 *	==> returns <listwidget>.
 * This method removes the item associated with <position> from the
 * selected list and unhighlights it.
 * <position> is an positive fixnum. 
 *
 * void XmListDeselectPos(w, pos)
 *     Widget w;
 *     int	pos;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_DESELECT_POS()
{
  LVAL self;
  int position;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  position = Get_ListPosition_Argument(); /* get <position> */
  xllastarg();

  XmListDeselectPos(widget_id, position);

  return (self);
}


/*****************************************************************************
 * (send <listwidget> :DESELECT_ALL_ITEMS)
 *	==> returns <listwidget>.
 * This method unhighlighs all selected items and removes all items from the
 * selected items list.
 *
 * void XmListDeselectAllItems(w)
 *     Widget w;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_DESELECT_ALL_ITEMS()
{
  LVAL self;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  xllastarg();

  XmListDeselectAllItems(widget_id);
  return (self);
}


/*****************************************************************************
 * (send <listwidget> :SET_POS <position>)
 *	==> returns <listwidget>.
 * This method makes the item associated with <position> be the first
 * visible element of the list.
 * <position> is an positive fixnum. 
 *
 * void XmListSetPos(w, pos)
 *     Widget w;
 *     int    pos;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_SET_POS()
{
  LVAL self;
  int position;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  position = Get_ListPosition_Argument(); /* get <position> */
  xllastarg();

  XmListSetPos(widget_id, position);

  return (self);
}


/*****************************************************************************
 * (send <listwidget> :SET_BOTTOM_POS <position>)
 *	==> returns <listwidget>.
 * This method makes the item associated with <position> be the last visible
 * position in the list.
 * <position> is an positive fixnum. 
 *
 * void XmListSetBottomPos(w, pos)
 *     Widget w;
 *     int    pos;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_SET_BOTTOM_POS()
{
  LVAL self;
  int position;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  position = Get_ListPosition_Argument(); /* get <position> */
  xllastarg();

  XmListSetBottomPos(widget_id, position);

  return (self);
}


/*****************************************************************************
 * (send <listwidget> :SET_ITEM <item>)
 *	==> returns <item> as an XmString
 * This method makes <item> be the first visible item in the list.
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString and that XmString is returned by the method.
 *
 * void XmListSetItem(w, item)
 *     Widget w;
 *     XmString item;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_SET_ITEM()
{
  LVAL self, lval_item;
  Widget widget_id;
  XmString str;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  str = Get_String_or_XmString_Arg_Returning_XmString(&lval_item); /* get <item> */
  xllastarg();

  XmListSetItem(widget_id, str);

  /* note: XmListSetItem() creates a copy of the XmString <item>,
     allowing us to free str whenever we want. If <item> was passed in as an
     XmString, the XmString will be freed via garbage collection whenever <item>
     is no longer referenced. If <item> was passed in as a normal string, then a
     new XmString object is created in this method -- it will be freed when it
     gets garbage collected.  */
  return (lval_item);		/* return the XmString item. */
}


/*****************************************************************************
 * (send <listwidget> :SET_BOTTOM_ITEM <item>)
 *	==> returns <item> as an XmString
 * This method makes <item> be the last visible position in the list.
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString and that XmString is returned by the method.
 *
 * void XmListSetBottomItem(w, item)
 *     Widget w;
 *     XmString 	  item;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_SET_BOTTOM_ITEM()
{
  LVAL self, lval_item;
  Widget widget_id;
  XmString str;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  str = Get_String_or_XmString_Arg_Returning_XmString(&lval_item); /* get <item> */
  xllastarg();

  XmListSetBottomItem(widget_id, str);

  /* note: XmListSetBottomItem() creates a copy of the XmString <item>,
     allowing us to free str whenever we want. If <item> was passed in as an
     XmString, the XmString will be freed via garbage collection whenever <item>
     is no longer referenced. If <item> was passed in as a normal string, then a
     new XmString object is created in this method -- it will be freed when it
     gets garbage collected.  */
  return (lval_item);		/* return the XmString item. */
}


#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <listwidget> :SET_ADD_MODE <add-mode-p>)
 *	==> returns <listwidget>
 *
 * <add-mode-p> is a boolean. If <add-mode-p> non-NIL, then <listwidget>
 * will be set into add mode.
 *
 * --------------------------------------------------------------------------
 * void XmListSetAddMode (Widget w, Boolean add_mode);
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_SET_ADD_MODE()
{
  LVAL self, lval_add_mode_p;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  lval_add_mode_p = xlgetarg(); /* get <add-mode-p> */
  xllastarg();

  XmListSetAddMode(widget_id, (lval_add_mode_p) ? TRUE : FALSE);

  return (self);
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <listwidget> :ITEM_EXISTS <item>)
 *	==> returns T if the item is in the specified list widget, else NIL.
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString.
 *
 * Boolean XmListItemExists(w, item)
 *     Widget w;
 *     XmString item;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_ITEM_EXISTS()
{
  extern LVAL true;
  Boolean result;
  LVAL self, lval_item;
  Widget widget_id;
  XmString str;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  if (moreargs() && stringp(*xlargv)) {	/* get <item> */
    str = XmStringCreate((char*) getstring(nextarg()), XmSTRING_DEFAULT_CHARSET);
    lval_item = NIL;
  }
  else if (moreargs() && xmstring_p(*xlargv)) {
    lval_item = nextarg();
    str = get_xmstring(lval_item);
  }
  else
    xlerror("Bad List-Item type: expected an XmString or a string.", *xlargv);
  xllastarg();

  result = XmListItemExists(widget_id, str);

  if (lval_item == NIL)		/* free the XmString only if we created it here ... note that we can't rely on gc to free the XmStrings since we can't return the XmString from this method */
    XmStringFree(str);

  return (result ? true : NIL);
}


#ifdef WINTERP_MOTIF_11
/*****************************************************************************
 * (send <listwidget> :ITEM_POS <item>)
 *	==> returns FIXNUM position of <item> in <listwidget>. If <item>
 *	doesn't exist, returns NIL.
 *
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString.
 * --------------------------------------------------------------------------
 * int XmListItemPos (Widget w, XmString item);
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_ITEM_POS()
{
  LVAL self, lval_item;
  Widget widget_id;
  XmString str;
  int result;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  if (moreargs() && stringp(*xlargv)) {	/* get <item> */
    str = XmStringCreate((char*) getstring(nextarg()), XmSTRING_DEFAULT_CHARSET);
    lval_item = NIL;
  }
  else if (moreargs() && xmstring_p(*xlargv)) {
    lval_item = nextarg();
    str = get_xmstring(lval_item);
  }
  else
    xlerror("Bad List-Item type: expected an XmString or a string.", *xlargv);
  xllastarg();

  result = XmListItemPos(widget_id, str);

  if (lval_item == NIL)		/* free the XmString only if we created it here ... note that we can't rely on gc to free the XmStrings since we can't return the XmString from this method */
    XmStringFree(str);

  return (result ? cvfixnum((FIXTYPE) result) : NIL);
}


/*****************************************************************************
 * (send <listwidget> :GET_MATCH_POS <item>)
 *	==> returns an array of FIXNUMS, each representing the position of <item>
 *          in <listwidget>. returns NIL if no matches found for <item>.
 *
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString.
 * --------------------------------------------------------------------------
 * Boolean XmListGetMatchPos (Widget w, XmString item, int **pos_list, int *pos_count);
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_GET_MATCH_POS()
{
  LVAL self, lval_item, lval_result;
  Widget widget_id;
  XmString str;
  Boolean result;
  int* position_list;
  int position_count;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  if (moreargs() && stringp(*xlargv)) {	/* get <item> */
    str = XmStringCreate((char*) getstring(nextarg()), XmSTRING_DEFAULT_CHARSET);
    lval_item = NIL;
  }
  else if (moreargs() && xmstring_p(*xlargv)) {
    lval_item = nextarg();
    str = get_xmstring(lval_item);
  }
  else
    xlerror("Bad List-Item type: expected an XmString or a string.", *xlargv);
  xllastarg();

  result = XmListGetMatchPos(widget_id, str, &position_list, &position_count);

  if (lval_item == NIL)		/* free the XmString only if we created it here ... note that we can't rely on gc to free the XmStrings since we can't return the XmString from this method */
    XmStringFree(str);

  if (!result)
    return (NIL);
  else {
    lval_result = IntList_To_Lisp_Vector(position_list, position_count);
    XtFree(position_list);
    return (lval_result);
  }
}


/*****************************************************************************
 * (send <listwidget> :GET_SELECTED_POS)
 *	==> returns an array of FIXNUMS, each representing the position of the
 *          selected items in <listwidget>. returns NIL if there are no selected
 *          items.
 *
 * --------------------------------------------------------------------------
 * Boolean XmListGetSelectedPos (Widget w, int **pos_list, int *pos_count);
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_GET_SELECTED_POS()
{
  LVAL self, lval_result;
  Widget widget_id;
  Boolean result;
  int* position_list;
  int position_count;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  xllastarg();

  result = XmListGetSelectedPos(widget_id, &position_list, &position_count);

  if (!result)
    return (NIL);
  else {
    lval_result = IntList_To_Lisp_Vector(position_list, position_count);
    XtFree(position_list);
    return (lval_result);
  }
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * (send <listwidget> :SET_HORIZ_POS <position>)
 *	==> returns <listwidget>.
 * This method scrolls the list horizontally such that the scrollbar reflects
 * <position> in the list.
 * <position> is an positive fixnum. 
 *
 * void XmListSetHorizPos(w, position)
 * Widget w;
 * int    position;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_SET_HORIZ_POS()
{
  LVAL self;
  int position;
  Widget widget_id;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self); /* get <listwidget> */
  position = Get_ListPosition_Argument(); /* get <position> */
  xllastarg();

  XmListSetHorizPos(widget_id, position);

  return (self);
}


/******************************************************************************
 * typedef struct
 * {
 *    int 	reason;
 *    XEvent    *event;
 *    XmString  item;
 *    int       item_length;
 *    int       item_position;
 *    XmString  *selected_items;
 *    int       selected_item_count;
 *    int       *selected_item_positions;  -- THIS FIELD EXISTS ONLY IN MOTIF 1.1
 *    int       selection_type;
 * } XmListCallbackStruct;
 ******************************************************************************/
static LVAL s_INITIAL, s_ADDITION, s_MODIFICATION;
static LVAL s_CALLBACK_ITEM, s_CALLBACK_ITEM_LENGTH,
  s_CALLBACK_ITEM_POSITION, s_CALLBACK_SELECTED_ITEMS,
  s_CALLBACK_SELECTED_ITEM_COUNT, s_CALLBACK_SELECTION_TYPE;
#ifdef WINTERP_MOTIF_11
static LVAL s_CALLBACK_SELECTED_ITEM_POSITIONS;
#endif				/* WINTERP_MOTIF_11 */

static void Lexical_Bindings_For_XmListCallbackStruct(bindings_list, lexical_env, cd, o_widget)
     LVAL      bindings_list;	/* a list of symbols to which values from XmListCallbackStruct are bound */
     LVAL      lexical_env;		
     XmListCallbackStruct* cd;
     LVAL      o_widget;	/* XLTYPE_WIDGETOBJ */
{
  extern LVAL s_CALLBACK_WIDGET, s_CALLBACK_REASON, s_CALLBACK_XEVENT; /* w_callbacks.c */
  extern LVAL Wcb_Get_Callback_Reason_Symbol();	/* w_callbacks.c */
  register LVAL s_bindname;

  switch (cd->reason) {

  case XmCR_SINGLE_SELECT:	/* valid XmListCallbackStruct fields: reason, event, item, item_length, item_position */
  case XmCR_DEFAULT_ACTION:	/* valid XmListCallbackStruct fields: reason, event, item, item_length, item_position */
  case XmCR_BROWSE_SELECT:	/* valid XmListCallbackStruct fields: reason, event, item, item_length, item_position */

    for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {
      s_bindname = car(bindings_list);

      if (s_bindname == s_CALLBACK_WIDGET) {
	xlpbind(s_bindname, o_widget, lexical_env); /* add binding to lexical_env */
      }
      else if (s_bindname == s_CALLBACK_REASON) {
	xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_XEVENT) {
	xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
      }
      else if (s_bindname == s_CALLBACK_ITEM) {
	xlpbind(s_bindname, (cd->item) ? cv_xmstring(XmStringCopy(cd->item)) : NIL, lexical_env); /* the copied XmString may be dealloc'd later by gc */
      }
      else if (s_bindname == s_CALLBACK_ITEM_LENGTH) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->item_length), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_ITEM_POSITION) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->item_position), lexical_env);
      }
      else {
	extern char temptext[];	/* from winterp.c */
	sprintf(temptext,
		"Unknown binding name in XmListCallbackStruct callback evaluator for\n\tCALLBACK_REASON == CR_SINGLE_SELECT, CR_DEFAULT_ACTION, CR_BROWSE_SELECT.\n\tValid symbols are [%s %s %s %s %s %s].",
		(char*) getstring(getpname(s_CALLBACK_WIDGET)),
		(char*) getstring(getpname(s_CALLBACK_REASON)),
		(char*) getstring(getpname(s_CALLBACK_XEVENT)),
		(char*) getstring(getpname(s_CALLBACK_ITEM)),
		(char*) getstring(getpname(s_CALLBACK_ITEM_LENGTH)),
		(char*) getstring(getpname(s_CALLBACK_ITEM_POSITION)));
	xlerror(temptext, s_bindname);
      }
    }
    break;

  case XmCR_MULTIPLE_SELECT:	/* valid XmListCallbackStruct fields: reason, event, item, item_length, item_position, selected_items, selected_item_count, selected_item_positions */

    for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {
      s_bindname = car(bindings_list);

      if (s_bindname == s_CALLBACK_WIDGET) {
	xlpbind(s_bindname, o_widget, lexical_env); /* add binding to lexical_env */
      }
      else if (s_bindname == s_CALLBACK_REASON) {
	xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_XEVENT) {
	xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
      }
      else if (s_bindname == s_CALLBACK_ITEM) {
	xlpbind(s_bindname, (cd->item) ? cv_xmstring(XmStringCopy(cd->item)) : NIL, lexical_env); /* the copied XmString may be dealloc'd later by gc */
      }
      else if (s_bindname == s_CALLBACK_ITEM_LENGTH) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->item_length), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_ITEM_POSITION) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->item_position), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_SELECTED_ITEMS) {
	xlpbind(s_bindname, Wxms_XmStringTable_To_Lisp_Vector(cd->selected_items, cd->selected_item_count), lexical_env); /* the vector and it's constituent XmStrings may be freed later via gc */
      }
      else if (s_bindname == s_CALLBACK_SELECTED_ITEM_COUNT) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->selected_item_count), lexical_env);
      }
#ifdef WINTERP_MOTIF_11
      else if (s_bindname == s_CALLBACK_SELECTED_ITEM_POSITIONS) {
	xlpbind(s_bindname, IntList_To_Lisp_Vector(cd->selected_item_positions, cd->selected_item_count), lexical_env); /* the vector and it's constituent FIXNUMs may be freed later via gc */
      }
#endif				/* WINTERP_MOTIF_11 */
      else {
	extern char temptext[];	/* from winterp.c */
	sprintf(temptext,
#ifdef WINTERP_MOTIF_11
		"Unknown binding name in XmListCallbackStruct callback evaluator for\n\tCALLBACK_REASON == CR_MULTIPLE_SELECT.\n\tValid symbols are [%s %s %s %s %s %s %s %s %s].",
#else				/* MOTIF 1.0 */
		"Unknown binding name in XmListCallbackStruct callback evaluator for\n\tCALLBACK_REASON == CR_MULTIPLE_SELECT.\n\tValid symbols are [%s %s %s %s %s %s %s %s].",
#endif				/* WINTERP_MOTIF_11 */
		(char*) getstring(getpname(s_CALLBACK_WIDGET)),
		(char*) getstring(getpname(s_CALLBACK_REASON)),
		(char*) getstring(getpname(s_CALLBACK_XEVENT)),
		(char*) getstring(getpname(s_CALLBACK_ITEM)),
		(char*) getstring(getpname(s_CALLBACK_ITEM_LENGTH)),
		(char*) getstring(getpname(s_CALLBACK_ITEM_POSITION)),
		(char*) getstring(getpname(s_CALLBACK_SELECTED_ITEMS)),
#ifdef WINTERP_MOTIF_11
		(char*) getstring(getpname(s_CALLBACK_SELECTED_ITEM_POSITIONS)),
#endif				/* WINTERP_MOTIF_11 */
		(char*) getstring(getpname(s_CALLBACK_SELECTED_ITEM_COUNT)));
	xlerror(temptext, s_bindname);
      }
    }
    break;

  case XmCR_EXTENDED_SELECT:	/* valid XmListCallbackStruct fields: reason, event, item, item_length, item_position, selected_items, selected_item_count, selected_item_positions, selection_type */

    for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {
      s_bindname = car(bindings_list);

      if (s_bindname == s_CALLBACK_WIDGET) {
	xlpbind(s_bindname, o_widget, lexical_env); /* add binding to lexical_env */
      }
      else if (s_bindname == s_CALLBACK_REASON) {
	xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_XEVENT) {
	xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
      }
      else if (s_bindname == s_CALLBACK_ITEM) {
	xlpbind(s_bindname, (cd->item) ? cv_xmstring(XmStringCopy(cd->item)) : NIL, lexical_env); /* the copied XmString may be dealloc'd later by gc */
      }
      else if (s_bindname == s_CALLBACK_ITEM_LENGTH) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->item_length), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_ITEM_POSITION) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->item_position), lexical_env);
      }
      else if (s_bindname == s_CALLBACK_SELECTED_ITEMS) {
	xlpbind(s_bindname, Wxms_XmStringTable_To_Lisp_Vector(cd->selected_items, cd->selected_item_count), lexical_env); /* the vector and it's constituent XmStrings may be freed later via gc */
      }
      else if (s_bindname == s_CALLBACK_SELECTED_ITEM_COUNT) {
	xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->selected_item_count), lexical_env);
      }
#ifdef WINTERP_MOTIF_11
      else if (s_bindname == s_CALLBACK_SELECTED_ITEM_POSITIONS) {
	xlpbind(s_bindname, IntList_To_Lisp_Vector(cd->selected_item_positions, cd->selected_item_count), lexical_env); /* the vector and it's constituent FIXNUMs may be freed later via gc */
      }
#endif				/* WINTERP_MOTIF_11 */
      else if (s_bindname == s_CALLBACK_SELECTION_TYPE)
	switch (cd->selection_type) {
	case XmINITIAL:
	  xlpbind(s_bindname, s_INITIAL, lexical_env);
	  break;
	case XmADDITION:
	  xlpbind(s_bindname, s_ADDITION, lexical_env);
	  break;
	case XmMODIFICATION:
	  xlpbind(s_bindname, s_MODIFICATION, lexical_env);
	  break;
	default:
	  xlfail("Internal error in Lexical_Bindings_For_XmListCallbackStruct() -- unknown cb->selection_type.");
	  break;
	}
      else {
	extern char temptext[];	/* from winterp.c */
	sprintf(temptext,
#ifdef WINTERP_MOTIF_11
		"Unknown binding name in XmListCallbackStruct callback evaluator for\n\tCALLBACK_REASON == CR_EXTENDED_SELECT.\n\tValid symbols are [%s %s %s %s %s %s %s %s %s %s].",
#else				/* MOTIF 1.0 */
		"Unknown binding name in XmListCallbackStruct callback evaluator for\n\tCALLBACK_REASON == CR_EXTENDED_SELECT.\n\tValid symbols are [%s %s %s %s %s %s %s %s %s].",
#endif				/* WINTERP_MOTIF_11 */
		(char*) getstring(getpname(s_CALLBACK_WIDGET)),
		(char*) getstring(getpname(s_CALLBACK_REASON)),
		(char*) getstring(getpname(s_CALLBACK_XEVENT)),
		(char*) getstring(getpname(s_CALLBACK_ITEM)),
		(char*) getstring(getpname(s_CALLBACK_ITEM_LENGTH)),
		(char*) getstring(getpname(s_CALLBACK_ITEM_POSITION)),
		(char*) getstring(getpname(s_CALLBACK_SELECTED_ITEMS)),
		(char*) getstring(getpname(s_CALLBACK_SELECTED_ITEM_COUNT)),
#ifdef WINTERP_MOTIF_11
		(char*) getstring(getpname(s_CALLBACK_SELECTED_ITEM_POSITIONS)),
#endif				/* WINTERP_MOTIF_11 */
		(char*) getstring(getpname(s_CALLBACK_SELECTION_TYPE)));
	xlerror(temptext, s_bindname);
      }
    }
    break;
  }
}


/*******************************************************************************
 * This is called indirectly via XtAddCallback() for callbacks returning
 * an XmListCallbackStruct as call_data.
 ******************************************************************************/
static void XmListCallbackStruct_Callbackproc(widget, client_data, call_data)
     Widget    widget;
     XtPointer client_data;
     XtPointer call_data;
{
  extern void Wcb_Meta_Callbackproc();	/* w_callbacks.c */

  Wcb_Meta_Callbackproc(client_data, call_data,
			Lexical_Bindings_For_XmListCallbackStruct,
			NULL);
}


/******************************************************************************
 * Same as WIDGET_CLASS's :add_callback method except that this understands
 * how to get values from the XmListCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_ITEM
 * CALLBACK_ITEM_LENGTH
 * CALLBACK_ITEM_POSITION
 * CALLBACK_SELECTED_ITEMS
 * CALLBACK_SELECTED_ITEM_COUNT
 * CALLBACK_SELECTION_TYPE
 * CALLBACK_SELECTED_ITEM_POSITION -- new for motif 1.1
 ******************************************************************************/
LVAL Xm_List_Widget_Class_Method_ADD_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */

  return (Wcb_Meta_Method_Add_Callback(XmListCallbackStruct_Callbackproc, FALSE));
}


/******************************************************************************
 * Same as WIDGET_CLASS's :set_callback method except that this understands
 * how to get values from the XmListCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_ITEM
 * CALLBACK_ITEM_LENGTH
 * CALLBACK_ITEM_POSITION
 * CALLBACK_SELECTED_ITEMS
 * CALLBACK_SELECTED_ITEM_COUNT
 * CALLBACK_SELECTION_TYPE
 * CALLBACK_SELECTED_ITEM_POSITION -- new for motif 1.1
 ******************************************************************************/
LVAL Xm_List_Widget_Class_Method_SET_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */

  return (Wcb_Meta_Method_Add_Callback(XmListCallbackStruct_Callbackproc, TRUE));
}


/******************************************************************************
 * (send <listwidget> :GET_ITEMS)
 * 	==> returns an array of XmStrings.
 *
 * This retrieves the XmList widget resources XmNitems and XmNitemCount
 * from <listwidget> and returns an array of XmStrings
 * representing the items in the list.
 *******************************************************************************/
LVAL Xm_List_Widget_Class_Method_GET_ITEMS()
{
  LVAL self;
  Widget widget_id;
  XmStringTable xmstrtab;
  int		xmstrtab_size;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  ARGLIST_RESET();
  ARGLIST_ADD(XmNitems, &xmstrtab);
  ARGLIST_ADD(XmNitemCount, &xmstrtab_size);
  XtGetValues(widget_id, ARGLIST());

  return (Wxms_XmStringTable_To_Lisp_Vector(xmstrtab, xmstrtab_size));
}


/******************************************************************************
 * (send <listwidget> :GET_SELECTED_ITEMS)
 * 	==> returns an array of XmStrings.
 *
 * This retrieves the XmList widget resources XmNselectedItems and
 * XmNselectedItemCount from <listwidget> and returns an array of XmStrings
 * representing the selected items in the list widget.
 *******************************************************************************/
LVAL Xm_List_Widget_Class_Method_GET_SELECTED_ITEMS()
{
  LVAL self;
  Widget widget_id;
  XmStringTable xmstrtab;
  int		xmstrtab_size;

  widget_id = Wcls_Get_WIDGETOBJ_Argument_Returning_Validated_WidgetID(&self);
  xllastarg();

  ARGLIST_RESET();
  ARGLIST_ADD(XmNselectedItems, &xmstrtab);
  ARGLIST_ADD(XmNselectedItemCount, &xmstrtab_size);
  XtGetValues(widget_id, ARGLIST());

  return (Wxms_XmStringTable_To_Lisp_Vector(xmstrtab, xmstrtab_size));
}


/******************************************************************************
 *
 ******************************************************************************/
Wc_List_Init()
{
  LVAL o_XM_LIST_WIDGET_CLASS;
  extern LVAL Wcls_Create_Subclass_Of_WIDGET_CLASS(); /* w_classes.c */
  extern      xladdmsg();	/* from xlobj.c */

  o_XM_LIST_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_LIST_WIDGET_CLASS",
					 xmListWidgetClass);

  /* a special :isnew method on this class allows for the creation of this
     widget inside a scrolled window if the submessage keyword :scrolled 
     is given ... */
  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Xm_List_Widget_Class_Method_ISNEW);

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":PARENT", 
	   FTAB_Xm_List_Widget_Class_Method_PARENT);
#endif				/* WINTERP_MOTIF_11 */
  
  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":ADD_ITEM", 
	   FTAB_Xm_List_Widget_Class_Method_ADD_ITEM);

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":ADD_ITEMS", 
	   FTAB_Xm_List_Widget_Class_Method_ADD_ITEMS);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":ADD_ITEM_UNSELECTED", 
	   FTAB_Xm_List_Widget_Class_Method_ADD_ITEM_UNSELECTED);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":DELETE_ITEM", 
	   FTAB_Xm_List_Widget_Class_Method_DELETE_ITEM);

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":DELETE_ITEMS", 
	   FTAB_Xm_List_Widget_Class_Method_DELETE_ITEMS);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":DELETE_POS", 
	   FTAB_Xm_List_Widget_Class_Method_DELETE_POS);

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":DELETE_ITEMS_POS", 
	   FTAB_Xm_List_Widget_Class_Method_DELETE_ITEMS_POS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":DELETE_ALL_ITEMS", 
	   FTAB_Xm_List_Widget_Class_Method_DELETE_ALL_ITEMS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":REPLACE_ITEMS", 
	   FTAB_Xm_List_Widget_Class_Method_REPLACE_ITEMS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":REPLACE_ITEMS_POS", 
	   FTAB_Xm_List_Widget_Class_Method_REPLACE_ITEMS_POS);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SELECT_ITEM", 
	   FTAB_Xm_List_Widget_Class_Method_SELECT_ITEM);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SELECT_POS", 
	   FTAB_Xm_List_Widget_Class_Method_SELECT_POS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":DESELECT_ITEM", 
	   FTAB_Xm_List_Widget_Class_Method_DESELECT_ITEM);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":DESELECT_POS", 
	   FTAB_Xm_List_Widget_Class_Method_DESELECT_POS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":DESELECT_ALL_ITEMS", 
	   FTAB_Xm_List_Widget_Class_Method_DESELECT_ALL_ITEMS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SET_POS", 
	   FTAB_Xm_List_Widget_Class_Method_SET_POS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SET_BOTTOM_POS", 
	   FTAB_Xm_List_Widget_Class_Method_SET_BOTTOM_POS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SET_ITEM", 
	   FTAB_Xm_List_Widget_Class_Method_SET_ITEM);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SET_BOTTOM_ITEM", 
	   FTAB_Xm_List_Widget_Class_Method_SET_BOTTOM_ITEM);

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SET_ADD_MODE", 
	   FTAB_Xm_List_Widget_Class_Method_SET_ADD_MODE);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":ITEM_EXISTS", 
	   FTAB_Xm_List_Widget_Class_Method_ITEM_EXISTS);

#ifdef WINTERP_MOTIF_11
  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":ITEM_POS", 
	   FTAB_Xm_List_Widget_Class_Method_ITEM_POS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":GET_MATCH_POS", 
	   FTAB_Xm_List_Widget_Class_Method_GET_MATCH_POS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":GET_SELECTED_POS", 
	   FTAB_Xm_List_Widget_Class_Method_GET_SELECTED_POS);
#endif				/* WINTERP_MOTIF_11 */

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SET_HORIZ_POS", 
	   FTAB_Xm_List_Widget_Class_Method_SET_HORIZ_POS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":ADD_CALLBACK", 
	   FTAB_Xm_List_Widget_Class_Method_ADD_CALLBACK);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SET_CALLBACK", 
	   FTAB_Xm_List_Widget_Class_Method_SET_CALLBACK);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":GET_ITEMS", 
	   FTAB_Xm_List_Widget_Class_Method_GET_ITEMS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":GET_SELECTED_ITEMS", 
	   FTAB_Xm_List_Widget_Class_Method_GET_SELECTED_ITEMS);

  
  s_INITIAL                          = xlenter("INITIAL");
  s_ADDITION                         = xlenter("ADDITION");
  s_MODIFICATION                     = xlenter("MODIFICATION");
  s_CALLBACK_ITEM                    = xlenter("CALLBACK_ITEM");
  s_CALLBACK_ITEM_LENGTH             = xlenter("CALLBACK_ITEM_LENGTH");
  s_CALLBACK_ITEM_POSITION           = xlenter("CALLBACK_ITEM_POSITION");
  s_CALLBACK_SELECTED_ITEMS          = xlenter("CALLBACK_SELECTED_ITEMS");
  s_CALLBACK_SELECTED_ITEM_COUNT     = xlenter("CALLBACK_SELECTED_ITEM_COUNT");
  s_CALLBACK_SELECTION_TYPE          = xlenter("CALLBACK_SELECTION_TYPE");
#ifdef WINTERP_MOTIF_11
  s_CALLBACK_SELECTED_ITEM_POSITIONS = xlenter("CALLBACK_SELECTED_ITEM_POSITIONS");
#endif				/* WINTERP_MOTIF_11 */
}
