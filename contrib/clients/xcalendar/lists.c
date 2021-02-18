/*
 *	$Source: /p/p4/X11R5/contrib/purdue/xcalendar/RCS/lists.c,v $
 *	$Header: /p/p4/X11R5/contrib/purdue/xcalendar/RCS/lists.c,v 1.1 1991/10/04 22:36:33 bingle Exp $
 *      $Author: bingle $
 *      $Locker:  $
 *
 * 	Copyright (C) 1988 Massachusetts Institute of Technology	
 *	
 */


/*

   Permission to use, copy, modify, and distribute this
   software and its documentation for any purpose and without
   fee is hereby granted, provided that the above copyright
   notice appear in all copies and that both that copyright
   notice and this permission notice appear in supporting
   documentation, and that the name of M.I.T. not be used in
   advertising or publicity pertaining to distribution of the
   software without specific, written prior permission.
   M.I.T. makes no representations about the suitability of
   this software for any purpose.  It is provided "as is"
   without express or implied warranty.

*/

/** Written by Roman J. Budzianowski - Project Athena, MIT **/
/** part of xcalendar **/

#ifndef lint
static char *rcsid_lists_c = "$Header: /p/p4/X11R5/contrib/purdue/xcalendar/RCS/lists.c,v 1.1 1991/10/04 22:36:33 bingle Exp $";
#endif lint

/* list manipulation package*/

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#include <X11/Xaw/Text.h>
#include <X11/Xutil.h>

#include "xcalendar.h"


List CreateList(length,elementSize)
     Cardinal length;		/* can be zero */
     Cardinal elementSize;
{
   char *elements;
   List list;

   list = (List)XtMalloc(sizeof(ListRec));

   if(!length) length = 1;

   elements = XtCalloc(length,elementSize);

   list->list = elements;
   list->len = length;
   list->pos = 0;
   list->size = elementSize;

   return(list);
}

void FreeList(list)
     List list;
{
   XtFree(list->list);
   XtFree(list);
}

Cardinal PushOnList(list,addr)
       register List   list;
       caddr_t         addr;
{
   if(list->pos  >= list->len)
     list->list = XtRealloc(list->list,(++list->len)*list->size);

   bcopy(addr,list->list + list->size * list->pos, list->size);

   return(++list->pos);
}

/* double linked circular list  */

LList *pput(element,aKey,list)		
     caddr_t element;
     Window  aKey;
     LList   *list; 
{
   LList *new;

   new = (LList *)XtMalloc(sizeof(struct _llist));
   new->element = element;
   
   if(XSaveContext(dsp,aKey,(XContext)element,(caddr_t)new) == XCNOMEM)
     XtError("No memory");

   if(list){
      new->next = list->next;
      list->next = new;
      new->prev = list;
      new->next->prev = new;
   }
   else{
      new->next = new;
      new->prev = new;
   }
   return new;
}
 
LList *premove(element,aKey,list)
     caddr_t element;
     Window  aKey;
     LList   *list;
{
   LList *ptr, *lookup();

   if(!list) return NULL;
   
   ptr = lookup(element,aKey,list);

   if(is_last(list)){
      XDeleteContext(dsp,aKey,(XContext)ptr->element);
      free(list);
      return NULL;
   }

   if(ptr){
      XDeleteContext(dsp,aKey,(XContext)ptr->element);
      ptr->next->prev = ptr->prev;
      ptr->prev->next = ptr->next;
      list = ptr->next;
      free(ptr);
   }

   return list;

}

LList *lookup(element,aKey,list)
     caddr_t element;
     Window  aKey;
     LList   *list;
{
   LList *ptr = NULL;
   int status;

   if(!list) return NULL;
   
   status = XFindContext(dsp,aKey,(XContext)element,(caddr_t *)&ptr);

   if(status == 0){
      return ptr;
   }
   else if(status == XCNOENT)
     return NULL;
   else
     XtError("No Memory");
}

is_last(list)
     LList *list;
{
   return( (list->next == list)? 1 : 0);
}
      
