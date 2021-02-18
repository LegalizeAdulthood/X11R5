#define CompositeSimple
/*
 * Copyright 1991 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          580 Theory Center, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@graphics.cornell.edu
 */

/*
 * XcuDeck widget
 */

#include <X11/Xos.h>
#include <stdio.h>
#include <sys/stat.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xcu/DeckP.h>

/* Private Definitions */

/*
 * defaults for resources
 */

static int     def_zero =  0 ;
#ifdef CompositeSimple
static Dimension   defFacetWidth    = 0 ;
#endif

/*
 * resource declarations
 */

#define offset(field) XtOffset(XcuDeckWidget, field)
#define Offset(field) XtOffset(XcuDeckWidget, deck.field)

static XcuResizeParticipants def_participants =
       XcuRESIZE_INTERS | XcuRESIZE_CHILDREN ;

static XtResource resources[] =
{
     {XtNinternalWidth,      XtCWidth,   XtRInt,  sizeof(int),
      Offset(internal_width),XtRInt,	(XPointer) &def_zero}

    ,{XtNinternalHeight,     XtCHeight,  XtRInt, sizeof(int),
      Offset(internal_height),XtRInt,	(XPointer) &def_zero}

    ,{XtNresizeParticipants, XtCHeight,  XtRResizeParticipants,
      sizeof(XcuResizeParticipants),
      Offset(resize_participants), XtRResizeParticipants,
      (XPointer) &def_participants}

    ,{XtNcallback, XtCCallback, XtRCallback, sizeof(XPointer), 
      Offset(callbacks), XtRCallback, (XPointer)NULL}
#ifdef CompositeSimple

    ,{XtNfacetWidth, XtCFacetWidth, XtRDimension, sizeof(Dimension),
	offset(simple.facet_width), XtRDimension, (XPointer)&defFacetWidth}
#endif

    /* UNDOCUMENTED FEATURE
       procedureList is not documented in the manual because it is probably
       only useful to the XcuWlm widget! */

    ,{XtNprocedureList, XtCProcedureList,
      XtRProcedureList, sizeof(XtProcedureList *),
      Offset(procedure_list), XtRProcedureList, (XPointer)NULL}
} ;

#undef Offset
#undef offset

/* Declarations of "toolkit" calls */

static void		ChangeManaged () ;
static void		ClassInitialize () ;
static void		Initialize () ;
static void		Resize () ;
static Boolean		SetValues () ;
static XtGeometryResult	GeometryManager () ;
static XtGeometryResult QueryGeometry () ;
static void		Destroy () ;
#ifdef CompositeSimple
static void		Redisplay() ;
#define superclass (&xcuSimpleCompClassRec)
#endif

/* Declarations of "private" calls */

static void		Trial_STS() ;
static void		Trial_ETS () ;
static void		DoLayout () ;
static void		alloc_wh () ;
static void		free_wh () ;
static void		init_order () ;
static int		child_to_i () ;

/* Declarations of "public" calls */

void XcuDeckRaiseLowest () ;
void XcuDeckLowerHighest () ;
void XcuDeckRaiseWidget () ;

/*
 * Class Record Constant
 */

XcuDeckClassRec xcuDeckClassRec =
{
  { /* core_class fields */
#ifdef CompositeSimple
    /* superclass         */    (WidgetClass) &xcuSimpleCompClassRec,
#else
    /* superclass         */    (WidgetClass) &compositeClassRec,
#endif
    /* class_name         */    "Deck",
    /* widget_size        */    sizeof(XcuDeckRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_init    */    NULL,
    /* class_inited       */    False,
    /* initialize         */    Initialize,
    /* initialize_hook    */    NULL,
    /* realize            */    XtInheritRealize,
    /* actions            */    NULL,
    /* num_actions        */    0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    True,
    /* compress_exposure  */    True,
    /* compress_enterleave*/    True,
    /* visible_interest   */    False,
    /* destroy            */    Destroy,
    /* resize             */    Resize,
#ifdef CompositeSimple
    /* expose		  */	Redisplay,
#else
    /* expose             */    NULL,
#endif
    /* set_values         */    SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */    NULL,
    /* accept_focus       */    NULL,
    /* version            */    XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry	  */	QueryGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension	  */	NULL,
  },
  { /* composite_class fields */
    /* geometry_manager   */   GeometryManager,
    /* change_managed     */   ChangeManaged,
    /* insert_child       */   XtInheritInsertChild,
    /* delete_child       */   XtInheritDeleteChild,
    /* extension 	  */   NULL,
  },
#ifdef CompositeSimple
  {
    XtInheritChangeSensitive, /* Simple Class fields */
    XtInheritDrawFacets,
    XtInheritDrawHighlight
  },
#endif
  { /* deck_class fields   */
    /* no entries   */   NULL
  }
} ;

/*
 * Class Record Pointer
 */

WidgetClass xcuDeckWidgetClass = (WidgetClass) &xcuDeckClassRec ;

/**
 ***
 **** Toolkit Callable Procedures
 ***
 **/

/***** **** *** ** * ClassInitialize * ** *** **** *****/

static void
ClassInitialize ()
{
/*
 * Register converters that might be needed
 */

XcuAddStringToResizeParticipantsConverter () ;

return ;
}

/***** **** *** ** * Initialize * ** *** **** *****/

static void
Initialize (request, new)
    Widget request, new ;
{
XcuDeckWidget dw = (XcuDeckWidget) new ;
static String XcuDeckRaiseWidgetText = "RaiseWidget" ;
static String XcuDeckRaiseLowestText = "RaiseLowest" ;
static String XcuDeckLowerHighestText = "LowerHighest" ;

/*
 * allocate space for and copy resources specified by address
 * (none for deck)
 */


/*
 * Initialize some non-resource fields
 */

dw->deck.order = NULL ;
dw->deck.width = NULL ;
dw->deck.height = NULL ;
dw->deck.procedure_list = (XtProcedureList *)
			   XtMalloc (4 * sizeof (XtProcedureList)) ;
dw->deck.procedure_list[0].name = XcuDeckRaiseLowestText ;
dw->deck.procedure_list[0].procedure = XcuDeckRaiseLowest ;
dw->deck.procedure_list[1].name = XcuDeckLowerHighestText ;
dw->deck.procedure_list[1].procedure = XcuDeckLowerHighest ;
dw->deck.procedure_list[2].name = XcuDeckRaiseWidgetText ;
dw->deck.procedure_list[2].procedure = XcuDeckRaiseWidget ;
dw->deck.procedure_list[3].name = NULL ;
dw->deck.procedure_list[3].procedure = NULL ;
#ifdef CompositeSimple
if (dw->simple.shadow)
    {
    XGCValues values ;
    values.foreground = dw->simple.background_pixel ;
    dw->deck.background_GC = XtGetGC (new, GCForeground, &values) ;
    }
#endif

return ;
}

/***** **** *** ** * Destroy * ** *** **** *****/

static void
Destroy (widget)
    Widget widget ;
{
XcuDeckWidget dw = (XcuDeckWidget) widget ;

/*
 * free dynamically allocated data
 */

free_wh (dw) ;

return ;
}

/***** **** *** ** * Resize * ** *** **** *****/

static void
Resize (w)
    Widget w ;
{
/*
 * Determine the new sizes and positions imposed by the resize
 */

Trial_STS ((XcuDeckWidget) w, (Widget) NULL) ;
Trial_ETS ((XcuDeckWidget) w) ;
DoLayout ((XcuDeckWidget) w, (Widget) NULL) ;

return ;
}

/***** **** *** ** * GeometryManager * ** *** **** *****/

static
XtGeometryResult
GeometryManager (w, request, reply)
    Widget w ;
    XtWidgetGeometry *request ;
    XtWidgetGeometry *reply;	/* RETURN */
{
Cardinal i ;
XtGeometryResult parent_result;
XtWidgetGeometry allowed ;
XcuDeckWidget dw = (XcuDeckWidget) w->core.parent ;
Dimension save_child_width = w->core.width ;
Dimension save_child_height = w->core.height ;
Dimension compromise_width, compromise_height ;

/*
 * Look for identical request to a prior XtGeometryAlmost -- we have to grant
 */

if (
   dw->deck.almost_status == True

   &&

   dw->deck.almost_widget == w
   
    &&

   (((dw->deck.almost_mode & CWWidth) && 
    (request->request_mode & CWWidth) && 
    (dw->deck.almost_width == request->width))
    ||
    ((dw->deck.almost_mode & CWWidth) ==
     (request->request_mode & CWWidth)))

    &&

   (((dw->deck.almost_mode & CWHeight) && 
    (request->request_mode & CWHeight) && 
    (dw->deck.almost_height == request->height))
    ||
    ((dw->deck.almost_mode & CWHeight) ==
     (request->request_mode & CWHeight)))
   )
    {
    /*
     * Granting a request identical to my last Almost reply
     */
    w->core.x = dw->deck.almost_x ;
    w->core.y = dw->deck.almost_y ;
    w->core.width = request->width ;
    w->core.height = request->height ;
    w->core.border_width = request->border_width ;

    Trial_STS ( dw, w ) ;
    Trial_ETS ( dw ) ;
    DoLayout  ( dw, w ) ;

    dw->deck.almost_status = False ;
    return XtGeometryYes ;
    }

dw->deck.almost_status = False ;

/*
 * Disallow only location requests from the children
 */

if ((request->request_mode & (CWWidth | CWHeight | CWStackMode | CWBorderWidth))
    == 0)
    {
    return XtGeometryNo ;
    }

/*
 * See if anything is different
 */

if (request->request_mode & CWWidth)
    allowed.width = request->width ;
else
    allowed.width = w->core.width ;

if (request->request_mode & CWHeight)
    allowed.height = request->height ;
else
    allowed.height = w->core.height ;

if (request->request_mode & CWBorderWidth)
    allowed.border_width = request->border_width ;
else
    allowed.border_width = w->core.border_width ;

/*
 * If there really isn't anything to change, just say yes
 */

if (allowed.width == w->core.width &&
    allowed.height == w->core.height &&
    allowed.border_width == w->core.border_width &&
    (request->request_mode & CWStackMode) == 0)
    {
    return XtGeometryYes ;
    }

if (request->request_mode & CWStackMode)
    {
    /* allow any stacking order request */
    reply->request_mode = (CWStackMode | CWSibling) ;
    reply->stack_mode = request->stack_mode ;
    reply->sibling = request->sibling ;
    }

if ((request->request_mode & (CWWidth | CWHeight | CWBorderWidth)) == 0)
    {
    return XtGeometryYes ;
    }

/*
 * We'll have to do a Trial_STS to see what the effects of the change are
 */

if (request->request_mode & CWWidth)
    w->core.width = request->width ;
if (request->request_mode & CWHeight)
    w->core.height = request->height ;
if (request->request_mode & CWBorderWidth)
    w->core.border_width = request->border_width ;
Trial_STS ( dw, w ) ;

/*
 * Run this layout by my parent
 */

parent_result = XtMakeResizeRequest ((Widget)dw,
			  (Dimension)dw->deck.standalone_width,
			  (Dimension)dw->deck.standalone_height,
			  &compromise_width,
			  &compromise_height) ;

if (parent_result == XtGeometryAlmost)
    {
    /* We'll accept whatever compromise is offered */
    XtMakeResizeRequest ((Widget)dw,
		         compromise_width, compromise_height, NULL, NULL) ;
    }
else
if (parent_result == XtGeometryYes)
    {
    }
else
    {
    w->core.width = save_child_width ;
    w->core.height = save_child_height ;
    return XtGeometryNo ;
    }

/*
 * Now we have to do a trial enclosed layout on the approved size
 */

Trial_ETS ( dw ) ;

i = child_to_i (dw, w) ;
if ( /* checking for nothing changed */
     (
     save_child_width == dw->deck.width[i] &&
     (request->request_mode & CWWidth)
     )
     &&
     (
     save_child_height == dw->deck.height[i] &&
     (request->request_mode & CWHeight)
     )
   )
    {
    w->core.width = save_child_width ;
    w->core.height = save_child_height ;
    return XtGeometryYes ;
    }

reply->request_mode = 0 ;

if (w->core.width != dw->deck.width[i] &&
    !(request->request_mode & CWWidth))
    {
    /* Need to change width even though it wasn't requested */
    reply->request_mode |= CWWidth ;
    }

if (w->core.height != dw->deck.height[i] &&
    !(request->request_mode & CWWidth))
    {
    /* Need to change height even though it wasn't requested */
    reply->request_mode |= CWHeight ;
    }

if (
   /* checking for changing width not as much as requested */
     (request->request_mode & CWWidth)
		   &&
     (
     request->width < dw->deck.width[i] &&
		      dw->deck.width[i] < save_child_width ||
     request->width > dw->deck.width[i] &&
		      dw->deck.width[i] > save_child_width
     )
   )
    {
    /* Need to change width less than requested */
    reply->request_mode |= CWWidth ;
    }

if (
   /* checking for changing height not as much as requested */
     (request->request_mode & CWHeight)
		   &&
     (
     request->height < dw->deck.height[i] &&
		      dw->deck.height[i] < save_child_height ||
     request->height > dw->deck.height[i] &&
		      dw->deck.height[i] > save_child_height
     )
   )
    {
    /* Need to change height less than requested */
    reply->request_mode |= CWHeight ;
    }

if (reply->request_mode)
    {
    /*
     * we'll return Almost.
     */
    w->core.width = save_child_width ;
    w->core.height = save_child_height ;
    dw->deck.almost_status = True ;
    dw->deck.almost_width = reply->width = dw->deck.width[i] ;
    dw->deck.almost_height = reply->height = dw->deck.height[i] ;
    dw->deck.almost_x = reply->x = dw->deck.x[i] ;
    dw->deck.almost_y = reply->y = dw->deck.y[i] ;
    reply->border_width = dw->deck.border_width[i] ;

    if (reply->x != w->core.x)
	reply->request_mode |= CWX ;
    if (reply->y != w->core.y)
	reply->request_mode |= CWY ;
    if (reply->border_width != w->core.border_width)
	reply->request_mode |= CWBorderWidth ;

    dw->deck.almost_widget = w ;
    dw->deck.almost_mode = reply->request_mode ;
    return XtGeometryAlmost ;
    }

/*
 * Actually do the layout
 */

if (!(request->request_mode & XtCWQueryOnly))
    DoLayout ( dw, w ) ;

return XtGeometryYes ;
}

/***** **** *** ** * SetValues * ** *** **** *****/

static Boolean
SetValues (current, request, new)
    Widget current, request, new ;
{
XcuDeckWidget cw = (XcuDeckWidget) current ;
XcuDeckWidget nw = (XcuDeckWidget) new ;

/* Check for new border color */

if (cw->core.border_pixel != nw->core.border_pixel)
    {
    Mask changes_mask ;
    XSetWindowAttributes changes ;

    changes_mask = CWBorderPixel ;
    changes.border_pixel = nw->core.border_pixel ;
    if (XtIsRealized(current))
	{
	XChangeWindowAttributes (XtDisplay(current), XtWindow(current),
				 changes_mask, &changes) ;
	}
    }

if (
      cw->deck.internal_width != nw->deck.internal_width
   || cw->deck.internal_height != nw->deck.internal_height
   || cw->deck.resize_participants != nw->deck.resize_participants
   )	
	{
	/*
	 * If any of these has changed, need to relayout (if realized)
	 */
	if (XtIsRealized (current))
	    {
	    nw->core.width = 0 ;
	    nw->core.height = 0 ;
	    Trial_STS (nw, (Widget) NULL) ;
	    Trial_ETS (nw) ;
	    DoLayout (nw, (Widget) NULL) ;
	    }
	}

return False ;
}

/***** **** *** ** * ChangeManaged * ** *** **** *****/

/*
 * ChangeManaged is called when there is a change in the set of children
 */

static void
ChangeManaged (w)
    Widget w ;
{
XtGeometryResult parent_result;
Dimension compromise_width, compromise_height ;
/*
 *
 */
XcuDeckWidget dw = (XcuDeckWidget) w ;

/*
 * Need to maintain an ordered list of the children.
 * We'll have to do it ourselves.
 * X Doesn't seem to want to let us peek at the stacking order.
 */

init_order (dw) ;
free_wh (dw) ;
alloc_wh (dw) ;
Trial_STS (dw, (Widget) NULL) ;

parent_result = XtMakeResizeRequest ((Widget)dw,
			  (Dimension)dw->deck.standalone_width,
			  (Dimension)dw->deck.standalone_height,
			  &compromise_width,
			  &compromise_height) ;

if (parent_result == XtGeometryAlmost)
    {
    /* We'll accept whatever compromise is offered */
    XtMakeResizeRequest ((Widget)dw,
		         compromise_width, compromise_height, NULL, NULL) ;
    }
else
if (parent_result == XtGeometryYes)
    {
    }
else
    {
    /***
    w->core.width = save_child_width ;
    w->core.height = save_child_height ;
    return XtGeometryNo ;
    ***/
    }
Trial_ETS (dw) ;
DoLayout (dw, (Widget) NULL) ;

return ;
}

/***** **** *** ** * QueryGeometry * ** *** **** *****/

static
XtGeometryResult
QueryGeometry (widget, requested, preferred)
    Widget widget;
    XtWidgetGeometry *requested ;
    XtWidgetGeometry *preferred ;
{
/*
 * Examine bits in requested->request_mode
 * Evaluate preferred geometry of the widget
 * Store the result in preferred, setting bits cared about in request_mode
 *  (CWX, CWY,  CWWidth, CWHeight,  CWBorderWidth,  CWSibling,  CWStackMode)
 *
 * acceptable without modification				XtGeometryYes
 *
 * one field in requested != one field in preferred ||
 * one bit set in preferred that is not set in requested	XtGeometryAlmost
 *
 * if preferred == current					XtGeometryNo
 */

XtGeometryResult return_mode ;
XcuDeckWidget w = (XcuDeckWidget) widget ;
preferred->width = w->deck.standalone_width ;
preferred->height = w->deck.standalone_height ;
preferred->request_mode = (CWWidth | CWHeight) ;

if ((requested->request_mode & (CWWidth | CWHeight)) == 0)
    {
    /* parent isn't interested in anything we're interested in */
    return XtGeometryYes;
    }

if (
    (
     ((requested->request_mode & CWWidth) != 0 &&
      preferred->width == requested->width)
		    ||
     ((requested->request_mode & CWWidth) == 0)
    )
		    &&
    (
     ((requested->request_mode & CWHeight) != 0 &&
      preferred->height == requested->height)
		    ||
     ((requested->request_mode & CWHeight) == 0)
    )
   )
    {
    /* Our values already identical to those the parent is interested in */
    return XtGeometryNo ;
    }

/*
 * That takes care of the simple cases, now we have to take a closer look...
 * I don't mind getting bigger than the smallest possible size.
 */

return_mode = XtGeometryYes ;
if (
    (requested->request_mode & CWHeight) &&
    (requested->height < preferred->height)
   )
    {
    return_mode = XtGeometryAlmost ;
    }

if (
    (requested->request_mode & CWWidth) &&
    (requested->width < preferred->width)
   )
    {
    return_mode = XtGeometryAlmost ;
    }

return return_mode ;
}

/**
 ***
 **** Private Procedures
 ***
 **/

/***** **** *** ** * Trial_STS * ** *** **** *****/

/*
 * The purpose of this function is to calculate the mimimum size of the deck
 */

static void
Trial_STS (dw, w_requester)
    XcuDeckWidget dw ;
    Widget w_requester ;
{
Dimension deck_width ;
Dimension deck_height ;
int num_children = dw->composite.num_children;
WidgetList children = dw->composite.children;
Cardinal i ;

/*
 * Step 0 : Collect the widths and heights of each item
 */

dw->deck.standalone_width = 0 ;
dw->deck.standalone_height = 0 ;

for (i=0;  i < num_children; i++)
    {
    Widget child = children[i] ;
    if (!XtIsManaged(child))
	continue ;
    /*
     * This is a bit tricky...
     * (Supposedly a lot easier in R3)
     * If the child has a new width and height because of a change in
     * text, its parent widget's GeometryManager is called, and
     * that's why we're here, now.  But, a call to QueryGeometry
     * at this point will result in the child returning its OLD preferred
     * geometry, since this Geometry manager was called with the old
     * widget, with only the new x,y,width,height copied in.  So,
     * I have to check for this situation and just grab the current width
     * and height in this situation.
     */
    if (child == w_requester)
	{
	dw->deck.width[i] = child->core.width ;
	dw->deck.height[i] = child->core.height ;
	dw->deck.border_width[i] = child->core.border_width ;
	}
    else
	{
	XtWidgetGeometry preferred ;
	XtQueryGeometry (child, NULL, &preferred) ;
	dw->deck.width[i] = preferred.width ;
	dw->deck.height[i] = preferred.height ;
	dw->deck.border_width[i] = preferred.border_width ;
	}

    dw->deck.x[i] = dw->deck.internal_width ;
    dw->deck.y[i] = dw->deck.internal_height ;
    deck_width = dw->deck.width[i] + 2*dw->deck.border_width[i] +
				     2*dw->deck.internal_width ;
    deck_height = dw->deck.height[i] + 2*dw->deck.border_width[i] +
				       2*dw->deck.internal_height ;

    if (deck_width > dw->deck.standalone_width)
	dw->deck.standalone_width = deck_width ;
    if (deck_height > dw->deck.standalone_height)
	dw->deck.standalone_height = deck_height ;
    }
#ifdef CompositeSimple
dw->deck.standalone_width += dw->simple.total_facet_width ;
dw->deck.standalone_height += dw->simple.total_facet_width ;
#endif

return ;
}

/***** **** *** ** * Trial_ETS * ** *** **** *****/

static void
Trial_ETS (dw)
    XcuDeckWidget dw ;
{
int c_slack ;  /* Can't be "Dimension" */
int r_slack ;
float c_expansion ;
float r_expansion ;
int internal_width = dw->deck.internal_width ;
int internal_height = dw->deck.internal_height ;

/*
 * Given an enclosing box, figure out how to parcel the additional
 * padding among the children.   If there is instead a loss of padding,
 * things would look awful, so things are laid out in the smallest way that
 * I would like, and the parent will have to settle for us being clipped.
 *
 * Theoretically, I could reduce the padding to zero, and that might be
 * a good idea.
 * If more space is needed, make the children's fonts smaller, but that is
 *  carrying things just a bit too far.
 */

/*
 * What are the possibilities for the proper effect of fitting within
 * a window that is larger than needed?
 *  1) Expand the children to take up the excess
 *  2) Expand the internalWidth,Height (centers the deck)
 *
 *  This code allows any combination of the above possibilities.
 *  (If no bits are set, the default is to expand the widgets)
 */

/*
 * Compute how much bigger (the slack) than the minimum we are.
 */

/* height */

if (dw->core.height == 0)
    dw->core.height = dw->deck.standalone_height ;

dw->deck.new_height = (dw->core.height < dw->deck.standalone_height) ?
	dw->deck.standalone_height : dw->core.height ;

r_expansion = (float) dw->deck.new_height / dw->deck.standalone_height ;
r_slack = dw->deck.new_height - dw->deck.standalone_height ;

/* width */

if (dw->core.width == 0)
    dw->core.width = dw->deck.standalone_width ;

dw->deck.new_width = (dw->core.width < dw->deck.standalone_width) ?
	dw->deck.standalone_width : dw->core.width ;

c_expansion = (float) dw->deck.new_width / dw->deck.standalone_width ;
c_slack = dw->deck.new_width - dw->deck.standalone_width ;

/*
 * Depending on what is allowed to change,
 * compute new padding parameters
 */

if ((int)dw->deck.resize_participants & (int)XcuRESIZE_INTERNALS)
    {
    if (((int)dw->deck.resize_participants & (int)XcuRESIZE_CHILDREN))
	{
	/* internal padding and children just expand proportionately */
	internal_height = dw->deck.internal_height * r_expansion ;
	r_slack -= 2 * (internal_height - dw->deck.internal_height) ;

	internal_width = dw->deck.internal_width * c_expansion ;
	c_slack -= 2 * (internal_width - dw->deck.internal_width) ;
	}
    else
	{
	/* internal padding takes up all the slack */
	internal_height = dw->deck.internal_height + r_slack/2 ;
	r_slack -= 2 * (internal_height - dw->deck.internal_height) ;

	internal_width = dw->deck.internal_width + c_slack/2 ;
	c_slack -= 2 * (internal_width - dw->deck.internal_width) ;
	}
    dw->deck.internal_width = internal_width ;
    dw->deck.internal_height = internal_height ;
    }

dw->deck.r_slack = r_slack ;
dw->deck.c_slack = c_slack ;

/* all have x,y at internal_width,height... */

return ;
}

/***** **** *** ** * DoLayout * ** *** **** *****/

static void
DoLayout (dw, requesting_widget)
    XcuDeckWidget dw ;
    Widget requesting_widget ;
{
Cardinal i ;
int num_children = dw->composite.num_children;
WidgetList children = dw->composite.children;

/* Use ConfigureWidget on siblings, modify requester directly */

for (i=0; i < num_children; i++)
    {
    Widget child = children[i] ;

    if (!XtIsManaged(child))
	continue ;

    if (child != requesting_widget)
	{
	XtConfigureWidget (child,
#ifdef CompositeSimple
                dw->simple.left_facet_width +
#endif
		dw->deck.internal_width,
#ifdef CompositeSimple
                dw->simple.top_facet_width +
#endif
		dw->deck.internal_height,
		(Dimension)
		    (dw->deck.new_width -
		     2*dw->deck.internal_width -
#ifdef CompositeSimple
		     dw->simple.total_facet_width -
#endif
		     2*dw->deck.border_width[i]),
		(Dimension)
		    (dw->deck.new_height -
		     2*dw->deck.internal_height -
#ifdef CompositeSimple
		     dw->simple.total_facet_width -
#endif
		     2*dw->deck.border_width[i]),
		(Dimension) dw->deck.border_width[i]) ;
	}
    else
	{
	child->core.x = dw->deck.internal_width ;
	child->core.y = dw->deck.internal_height ;
	child->core.width = dw->deck.new_width - 2*dw->deck.internal_width -
			    2*dw->deck.border_width[i] ;
	child->core.height = dw->deck.new_height - 2*dw->deck.internal_height -
			     2*dw->deck.border_width[i] ;
#ifdef CompositeSimple
child->core.width -= dw->simple.total_facet_width ;
child->core.height -= dw->simple.total_facet_width ;
#endif
	/*
	 * Well, this (XtConfigureWidget) isn't supposed to be here, but
	 * without it, the child's x & y never seem to be paid attention
	 * to.  Width and height change, but the widget stays right where
	 * it was...
	 */
	XtConfigureWidget (child, child->core.x, child->core.y,
				    (Dimension) child->core.width,
				    (Dimension) child->core.height,
				    (Dimension) child->core.border_width) ;
	}
    }

dw->core.width = dw->deck.new_width ;
dw->core.height = dw->deck.new_height ;

return ;
}

/***** **** *** ** * alloc_wh * ** *** **** *****/

static void
alloc_wh (dw)
    XcuDeckWidget dw ;
{
int num_children = dw->composite.num_children ;
dw->deck.x = (Position *) XtMalloc (num_children * sizeof (Position)) ;
dw->deck.y = (Position *) XtMalloc (num_children * sizeof (Position)) ;
dw->deck.width = (Dimension *) XtMalloc (num_children * sizeof (Dimension)) ;
dw->deck.height = (Dimension *) XtMalloc (num_children * sizeof (Dimension)) ;
dw->deck.border_width = (Dimension *) XtMalloc (num_children * sizeof (Dimension)) ;

return ;
}

/***** **** *** ** * free_wh * ** *** **** *****/

static void
free_wh (dw)
    XcuDeckWidget dw ;
{
if (dw->deck.width) XtFree ((char *)dw->deck.width) ;
if (dw->deck.height) XtFree ((char *)dw->deck.height) ;

return ;
}

/***** **** *** ** * child_to_i * ** *** **** *****/

/*
 * Well, I guess this should have been a constrained widget !
 */

static int
child_to_i (dw, w)
    XcuDeckWidget dw ;
    Widget w ;
{
Cardinal i ;
int num_children = dw->composite.num_children;
WidgetList children = dw->composite.children;

for (i=0;  i < num_children; i++)
    {
    Widget child = children[i] ;
    if (child == w)
	return i ;
    }
return 0 ; /* should never reach here... */
}

/***** **** *** ** * init_order * ** *** **** *****/

static void
init_order (dw)
    XcuDeckWidget dw ;
{
Cardinal i ;
if (dw->deck.order)
    XtFree ((char *)dw->deck.order) ;
dw->deck.order = (Widget *)
		 XtMalloc (dw->composite.num_children * sizeof (Widget)) ;
for (i=0;  i < dw->composite.num_children;  i++)
    {
    dw->deck.order[i] = dw->composite.children[i] ;
    }
return ;
}

/**
 ***
 **** Public Procedures
 ***
 **/

/***** **** *** ** * XcuDeckRaiseLowest * ** *** **** *****/

void
XcuDeckRaiseLowest (dw)
    XcuDeckWidget dw ;
{
int i ; /* Must not be Cardinal */
Cardinal n = dw->composite.num_children ;
Widget save ;

if (!XtIsSubclass ((Widget)dw, (WidgetClass)xcuDeckWidgetClass)) {
    XtError("XcuDeckRaiseLowest requires a subclass of xcuDeckWidgetClass");
    }
XCirculateSubwindows (XtDisplay((Widget)dw), XtWindow((Widget)dw),
			RaiseLowest) ;

save = dw->deck.order[n-1] ;
for (i=n-1;  i >= 1;  i--)
    dw->deck.order[i] = dw->deck.order[i-1] ;
dw->deck.order[0] = save ;

XtCallCallbacks((Widget)dw, XtNcallback, dw->deck.order);

return ;
}

/***** **** *** ** * XcuDeckLowerHighest * ** *** **** *****/

void
XcuDeckLowerHighest (dw)
    XcuDeckWidget dw ;
{
Cardinal i ;
Cardinal n = dw->composite.num_children ;
Widget save ;

if (!XtIsSubclass ((Widget)dw, (WidgetClass)xcuDeckWidgetClass)) {
    XtError("XcuDeckLowerHighest requires a subclass of xcuDeckWidgetClass");
    }
XCirculateSubwindows (XtDisplay((Widget)dw), XtWindow((Widget)dw),
			LowerHighest) ;

save = dw->deck.order[0] ;
for (i=0;  i < n-1;  i++)
    dw->deck.order[i] = dw->deck.order[i+1] ;
dw->deck.order[n-1] = save ;

XtCallCallbacks((Widget)dw, XtNcallback, dw->deck.order);

return ;
}

/***** **** *** ** * XcuDeckRaiseWidget * ** *** **** *****/

void
XcuDeckRaiseWidget (dw, to_be_raised)
    XcuDeckWidget dw ;
    Widget to_be_raised ;
{
int i ; /* Must not be Cardinal */
Cardinal n = dw->composite.num_children ;
Widget save ;

if (!XtIsSubclass ((Widget)dw, (WidgetClass)xcuDeckWidgetClass)) {
    XtError("XcuDeckRaiseWidget requires arg0 to be subclass of xcuDeckWidgetClass");
    }
XRaiseWindow (XtDisplay((Widget)dw), XtWindow(to_be_raised)) ;

for (i=0;  i < n;  i++)
    {
    if (dw->deck.order[i] == to_be_raised)
	break ;
    }
if (i == n)
    {
    char text[200] ;
    WidgetClass class = XtClass(to_be_raised) ;
    sprintf (text, "XcuDeckRaiseWidget: Widget (%s/%s) not found in XcuDeck\n",
	     class->core_class.class_name, to_be_raised->core.name) ;
    XtWarning (text) ;
    }
else
    {
    /* pull out the window, collapse part of the deck, store at top */
    save = dw->deck.order[i] ;
    for (  ;  i >= 1;  i--)
	dw->deck.order[i] = dw->deck.order[i-1] ;
    dw->deck.order[0] = save ;
    }

XtCallCallbacks((Widget)dw, XtNcallback, dw->deck.order);

return ;
}

#ifdef CompositeSimple

/***** **** *** ** * Redisplay * ** *** **** *****/

static void Redisplay(w, event, region)
    Widget w;
    XEvent *event;		/* unused */
    Region region;		/* unused */
{
XcuDeckWidget dw = (XcuDeckWidget) w ;
(*superclass->core_class.expose)( w, event, region ) ;
if (dw->simple.shadow)
    {
    XFillRectangle (XtDisplay(w), XtWindow(w), dw->deck.background_GC,
		    dw->simple.left_facet_width, dw->simple.top_facet_width,
		    dw->core.width - dw->simple.total_facet_width,
		    dw->core.height - dw->simple.total_facet_width) ;
    }
return ;
}

#endif
