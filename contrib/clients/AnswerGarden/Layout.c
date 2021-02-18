
/***********************************************************

Copyright 1989, 1990 by the Massachusetts Institute of Technology,
Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

   LAYOUT.C:  The Layout widget

   This is just like a bulletin board widget *except*
     - It does not resize itself to fit its children

   Mark Ackerman
   MIT/Center for Coordination Science
   July 7, 1990

   This was mostly done by Russ Sasnett, MIT/Project Athena and GTE
   Laboratories.  It is a stripped-down box widget.

   send bugs to:
    Mark Ackerman
    ackerman@athena.mit.edu
    ack@media-lab.media.mit.edu



******************************************************************/


/* 
 * Layout.c - Layout composite widget
 * 
 */

#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	"LayoutP.h"

/****************************************************************
 *
 * Layout Resources
 *
 ****************************************************************/

#define Offset(field) XtOffset(LayoutWidget, field)
static XtResource resources[] = {
    {XtNheight,XtCHeight,XtRDimension,sizeof(Dimension),
	 Offset(core.height), XtRImmediate, (XtPointer) 10},
    {XtNwidth,XtCWidth,XtRDimension,sizeof(Dimension),
	 Offset(core.width), XtRImmediate, (XtPointer) 10},
};

    
/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

static void Initialize();
static void Realize();
static void Resize();
static XtGeometryResult GeometryManager();
static void ChangeManaged();

LayoutClassRec layoutClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &compositeClassRec,
    /* class_name         */    "Layout",
    /* widget_size        */    sizeof(LayoutRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */	NULL,
    /* class_inited       */	FALSE,
    /* initialize         */    Initialize,
    /* initialize_hook    */	NULL,
    /* realize            */    Realize,
    /* actions            */    NULL,
    /* num_actions	  */	0,
    /* resources        */      resources,
    /* num_resources    */      XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/	TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    XtInheritResize,
    /* expose             */    NULL,
    /* set_values         */    NULL,
    /* set_values_hook    */	NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus       */    NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */	NULL,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },{
/* composite_class fields */
    /* geometry_manager   */    GeometryManager,
    /* change_managed     */    ChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
    /* extension          */	NULL
  },{
/* Layout class fields */
    /* empty		  */	0,
  }
};

WidgetClass layoutWidgetClass = (WidgetClass)&layoutClassRec;


/****************************************************************
 *
 * Private Routines
 *
 ****************************************************************/

/*
 *
 * Geometry Manager
 *
 * lets the child do whatever it wants to
 *
 */

/*ARGSUSED*/
static XtGeometryResult GeometryManager(w, request, reply)
    Widget		w;
    XtWidgetGeometry	*request;
    XtWidgetGeometry	*reply;	/* RETURN */

{
    if (request->request_mode & (CWX | CWY | CWWidth | CWHeight | CWBorderWidth)) 
	{
	if ((request->request_mode & CWWidth) == 0)
	    request->width = w->core.width;
	if ((request->request_mode & CWHeight) == 0)
	    request->height = w->core.height;
        if ((request->request_mode & CWBorderWidth) == 0)
	    request->border_width = w->core.border_width;
	if ((request->request_mode & CWX) == 0)
	    request->x = w->core.x;
	if ((request->request_mode & CWY) == 0)
	    request->y = w->core.y;

	w->core.x = request->x;
	w->core.y = request->y;
	w->core.width = request->width;
	w->core.height = request->height;
	w->core.border_width = request->border_width;
	}

    return (XtGeometryYes);
}


static void ChangeManaged(w)
    Widget w;
{
	/* don't care if anything goes away or not */
}

/* ARGSUSED */
static void Initialize(request, new)
    Widget request, new;
{
    LayoutWidget newlw = (LayoutWidget)new;

    newlw->layout.last_query_mode = CWWidth | CWHeight;
    newlw->layout.last_query_width = newlw->core.width;
    newlw->layout.last_query_height = newlw->core.height;

} /* Initialize */

static void Realize(w, valueMask, attributes)
    register Widget w;
    Mask *valueMask;
    XSetWindowAttributes *attributes;
{
    attributes->bit_gravity = NorthWestGravity;
    *valueMask |= CWBitGravity;

    XtCreateWindow( w, (unsigned)InputOutput, (Visual *)CopyFromParent,
		    *valueMask, attributes);
} /* Realize */
