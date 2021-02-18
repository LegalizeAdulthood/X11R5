/**********************************************************

  The ANSWER GARDEN PROJECT

Copyright 1989, 1990, 1991 by the Massachusetts Institute of Technology,
Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of MIT not be used in
advertising or publicity pertaining to distribution of the software
without specific, written prior permission.

Answer Garden is a trademark of the Massachusetts Institute of
Technology.  All rights reserved.


  Mark Ackerman
  MIT/Center for Coordination Science
  MIT/Project Athena

  ackerman@athena.mit.edu
  ackerman@xv.mit.edu

***********************************************************/


/* 
 * StackObj.c - StackObj object
 * 
 */

#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	"StackObjP.h"
#include        "AGmacros.h"


/****************************************************************
 *
 * StackObj Resources
 *
 ****************************************************************/

#define offset(field) XtOffset(StackObj, field)
static XtResource resources[] = {
  {XtNx, XtCX, XtRPosition, sizeof(Position),
     offset(stackObj.x),XtRImmediate,(XtPointer)0},
  {XtNy, XtCY, XtRPosition, sizeof(Position),
     offset(stackObj.y),XtRImmediate,(XtPointer)0},  
  {XtNhorizDistance, XtCThickness, XtRDimension, sizeof(Dimension),
     offset(stackObj.dx), XtRImmediate, (XtPointer) 10},
  {XtNvertDistance, XtCThickness, XtRDimension, sizeof(Dimension),
        offset(stackObj.dy), XtRImmediate, (XtPointer) 20},
};


    
/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

static void Initialize();


#define SuperClass ((WidgetClass)&objectClassRec)

StackObjClassRec stackObjClassRec = {
{
  
  /* superclass	        */	SuperClass,
  /* class_name	        */	"StackObj",
  /* widget_size	*/	sizeof(StackObjRec),
  /* class_initialize   */    NULL,
  /* class_part_initialize*/	NULL,
  /* class_inited       */	FALSE,
  /* initialize	        */	Initialize,
  /* initialize_hook    */	NULL,		
  /* pad                */    NULL,
  /* pad		  */	NULL,
  /* pad       	        */	0,
  /* resources	       */	resources,
  /* num_resources	*/      XtNumber(resources),
  /* xrm_class	        */	NULLQUARK,
  /* pad                */      FALSE,
  /* pad                */      FALSE,
  /* pad                */      FALSE,
  /* pad                */      FALSE,
  /* destroy		*/	NULL,
  /* pad		*/	NULL,
  /* pad		*/	NULL,
  /* set_values	        */      NULL,
  /* set_values_hook    */	NULL,			
  /* pad                */      NULL,
  /* get_values_hook    */	NULL,			
  /* pad                */      NULL,
  /* version		*/	XtVersion,
  /* callback_offsets   */      NULL,
  /* pad                */      NULL,
  /* pad                */      NULL,
  /* pad                */      NULL,
  /* extension	    */  NULL
},
{
  NULL, /* makes_compiler_happy */
}
};

WidgetClass stackObjClass = (WidgetClass)&stackObjClassRec;



/****************************************************************
 *
 * Private Routines
 *
 ****************************************************************/


static void Initialize(request, new)
    Widget request, new;
{
  StackObj so = (StackObj)new;

  so->stackObj.initial_x = so->stackObj.x;
  so->stackObj.initial_y = so->stackObj.y;
  so->stackObj.current_x = so->stackObj.x;
  so->stackObj.current_y = so->stackObj.y;

  so->stackObj.nchildren = -1;
}

AG_Stack_Add_Shell(so,shell,x,y)
     StackObj so;
     Widget shell;
     Position *x;
     Position *y;
{
  if (so->stackObj.nchildren < MAXCHILDREN)
    (so->stackObj.nchildren)++;
  so->stackObj.children[so->stackObj.nchildren] = shell;
  *x =  so->stackObj.current_x = so->stackObj.current_x + so->stackObj.dx;
  *y =  so->stackObj.current_y = so->stackObj.current_y + so->stackObj.dy;
  
  
}

AG_Stack_Delete_Shell(so,shell)
     StackObj so;
     Widget shell;
{
  int i;
  int j;
  for (i=0;i<=so->stackObj.nchildren;i++)
    if (so->stackObj.children[i] == shell)
      {
	for (j=i;j<so->stackObj.nchildren;j++)
	  so->stackObj.children[j] = so->stackObj.children[j+1];
	so->stackObj.nchildren--;
	return;
      }
  return;
}

AG_Stack_Tidy(so)
     StackObj so;
{
  int i;
  Position curx;
  Position cury;
  Window window;
  Window parent;
  Window root;
  Window *children;
  int nchildren;
  Widget shell;

  curx = so->stackObj.initial_x;
  cury = so->stackObj.initial_y;

  for (i=0;i<so->stackObj.nchildren;i++)
    {
      shell = so->stackObj.children[i];
      if (!XQueryTree(XtDisplay(shell), XtWindow(shell),&root,
		      &parent,&children,&nchildren))
	return;  /* something is wrong */
      while (parent != RootWindowOfScreen(XtScreen(shell)))
	{
	  window = parent;
	  XFree((char *)children);
	  if (!XQueryTree(XtDisplay(shell), XtWindow(shell),&root,
			  &parent,&children, &nchildren))
	    {
	      Util_Debug("unclear state in AGRaiseShell");
	      return; /* something is wrong */
	    }
	}
      XFree((char *)children);
        /* This is a brute force way of doing it.  I cannot for hours
	   of trying move my popups by doing a SetValues */
      XMoveWindow(window,curx,cury);

      so->stackObj.current_x = curx;
      so->stackObj.current_y = cury;
      curx = curx + so->stackObj.dx;
      cury = cury + so->stackObj.dy;
    }

}


