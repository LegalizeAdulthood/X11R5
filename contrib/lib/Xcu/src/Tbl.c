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

#include <X11/Xos.h>
#include <sys/stat.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xcu/TblP.h>

void Xcu_copy_ds () ;

/* Private Definitions */

/*
 * defaults for resources
 */

static Boolean def_false = False ;
static int     def_zero =  0 ;
#ifdef CompositeSimple
static Dimension   defFacetWidth    = 0 ;
#endif

static XcuResizeParticipants def_participants =
       XcuRESIZE_INTERS | XcuRESIZE_INTERNALS | XcuRESIZE_CHILDREN ;

/*
 * resource declarations
 */

#define offset(field) XtOffset(XcuTblWidget, field)
#define Offset(field) XtOffset(XcuTblWidget, tbl.field)

static XtResource resources[] =
{
     {XtNalignedColumns,     XtCBoolean, XtRBoolean, sizeof(Boolean),
      Offset(aligned_columns), XtRBoolean, (XPointer) &def_false}

    ,{XtNclipOnShrink,       XtCBoolean, XtRBoolean, sizeof(Boolean),
      Offset(clip_on_shrink), XtRBoolean, (XPointer) &def_false}

    ,{XtNequalColumns,       XtCBoolean, XtRBoolean, sizeof(Boolean),
      Offset(equal_columns), XtRBoolean, (XPointer) &def_false}

    ,{XtNequalRows,          XtCBoolean, XtRBoolean, sizeof(Boolean),
      Offset(equal_rows),    XtRBoolean, (XPointer) &def_false}

    ,{XtNformatFile,         XtCString,  XtRString,  sizeof(String),
      Offset(format_file),   XtRString, (XPointer) NULL}

    ,{XtNformatString,	     XtCString,  XtRString,  sizeof(String),
      Offset(format_string), XtRString, (XPointer) NULL}

    ,{XtNinterWidth,	     XtCWidth,   XtRInt,  sizeof(int),
      Offset(inter_width),   XtRInt,	(XPointer) &def_zero}

    ,{XtNinterHeight,  	     XtCHeight,  XtRInt,  sizeof(int),
      Offset(inter_height),  XtRInt,	(XPointer) &def_zero}

    ,{XtNinternalWidth,      XtCWidth,   XtRInt,  sizeof(int),
      Offset(internal_width),XtRInt,	(XPointer) &def_zero}

    ,{XtNinternalHeight,     XtCHeight,  XtRInt, sizeof(int),
      Offset(internal_height),XtRInt,	(XPointer) &def_zero}
#ifdef CompositeSimple

    ,{XtNfacetWidth, XtCFacetWidth, XtRDimension, sizeof(Dimension),
	offset(simple.facet_width), XtRDimension, (XPointer)&defFacetWidth}
#endif

    ,{XtNresizeParticipants, XtCResizeParticipants,
      XtRResizeParticipants, sizeof(XcuResizeParticipants),
      Offset(resize_participants), XtRResizeParticipants,
      (XPointer) &def_participants}

} ;

#undef Offset
#undef offset

#define Offset(field) XtOffset(XcuTblConstraints, tbl.field)

static XtResource tblConstraintResources[] =
{
     {XtNfillColumn,         XtCBoolean, XtRBoolean, sizeof(Boolean),
      Offset(fill_column),   XtRBoolean, (XPointer) &def_false}
} ;

#undef Offset

/* Declarations of "toolkit" calls */

static void		ClassPartInitialize () ;
static void		ClassInitialize () ;
static void		ChangeManaged () ;
static void		Initialize () ;
static void		Resize () ;
static Boolean		ConstraintSetValues () ;
static Boolean		SetValues () ;
static XtGeometryResult	GeometryManager () ;
static XtGeometryResult QueryGeometry () ;
static void		Destroy () ;
#ifdef CompositeSimple
static void		Redisplay() ;
#define superclass (&xcuSimpleCompConstClassRec)
#endif

/* Declarations of "XcuTbl Class" calls */

static String		InterpretString () ;

/* Declarations of "private" calls */

static void		unput () ;
static void		DoLayout () ;
static void		Trial_STS() ;
static void		Trial_ETS () ;
static void		ytblerror () ;
static void		convert_nl () ;
static void		InitLayout () ;
static void		SetupLayout () ;
static void		set_equal_cols () ;
static void		total_mismatch () ;
static void		e_apportion_slack () ;
static void		v_apportion_slack () ;
static Boolean		adjust_aligned_columns () ;
static Boolean		adjust_equal_columns () ;
static Boolean		adjust_spanned_columns () ;

/* It's not possible to get the following thing static since it's a macro */

	int		ParseTblLayout () ;
/*
 * Class Record Constant
 */

XcuTblClassRec xcuTblClassRec =
{
  { /* core_class fields */
#ifdef CompositeSimple
    /* superclass         */    (WidgetClass) &xcuSimpleCompConstClassRec,
#else
    /* superclass         */    (WidgetClass) &constraintClassRec,
#endif
    /* class_name         */    "Tbl",
    /* widget_size        */    sizeof(XcuTblRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_init    */    ClassPartInitialize,
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
    /* extension	  */   NULL,
  },
  { /* constraint_class fields */
    /* subresourses       */   tblConstraintResources,
    /* subresource_count  */   XtNumber(tblConstraintResources),
    /* constraint_size    */   sizeof(XcuTblConstraintsRec),
    /* initialize         */   NULL,
    /* destroy            */   NULL,
    /* set_values         */   ConstraintSetValues,
    /* extension         */    NULL
  },
#ifdef CompositeSimple
  {
    XtInheritChangeSensitive, /* Simple Class fields */
    XtInheritDrawFacets,
    XtInheritDrawHighlight
  },
#endif
  { /* tbl_class fields   */
    /* interpret_string   */   InterpretString
  }
} ;

/*
 * Class Record Pointer
 */

WidgetClass xcuTblWidgetClass = (WidgetClass) &xcuTblClassRec ;

/*
 * This static record doesn't seem to be a good idea, but I can't figure out
 * another way of getting the necessary information into the lexical
 * input routine.  (It seems to me that lex and yacc have a serious design flaw:
 * it wasn't foreseen that one might want several parsers in the same module.)
 *
 * Although several instances must share these static variables, they are
 * only used during one call from the toolkit, so if it executes atomically,
 * this should be okay.
 */

#define TBL_MAX_UNPUT 200
#define TBL_MAX_CONTEXT 200

typedef struct
    {
    String input_buffer ;
    Cardinal input_pointer ;
    char unput_buffer[TBL_MAX_UNPUT] ;
    Cardinal unput_pointer ;
    char context_buffer[TBL_MAX_CONTEXT] ;
    int context_ptr ; /* must not be Cardinal */
    Cardinal line_number ;
    String cur_file ;
    int snl_equivalent ;
    int lnl_equivalent ;
    int string_file ;
    } TblInputRecord ;

static TblInputRecord _layout_input, *layout_input = &_layout_input ;
static int change_managed_in_progress = 0 ; /* hack */

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

XcuAddStringToJustifyConverter () ;
XcuAddStringToGravityConverter () ;
XcuAddStringToResizeParticipantsConverter () ;

return ;
}

/***** **** *** ** * Initialize * ** *** **** *****/

static void
Initialize (request, new)
    Widget request, new ;
{
XcuTblWidget tw = (XcuTblWidget) new ;

/*
 * allocate space for and copy resources specified by address
 */

if (tw->tbl.format_file != NULL) 
    Xcu_copy_ds (&tw->tbl.format_file, tw->tbl.format_file) ;

if (tw->tbl.format_string != NULL) 
    Xcu_copy_ds (&tw->tbl.format_string, tw->tbl.format_string) ;

/*
 * Initialize some non-resource fields
 */

tw->tbl.child_border_width = -1 ;
tw->tbl.child_border_color = -1 ;
tw->tbl.child_background_color = -1 ;
tw->tbl.child_foreground_color = -1 ;
tw->tbl.string_breaker = '\t' ;
tw->tbl.format_mode = TBL_FIRST_PASS ;
tw->tbl.initting = False ;
tw->tbl.force_width = False ;
tw->tbl.width0 = tw->core.width ;
tw->tbl.height0 = tw->core.height ;

#ifdef CompositeSimple
if (tw->simple.shadow)
    {
    XGCValues values ;
    values.foreground = tw->simple.background_pixel ;
    tw->tbl.background_GC = XtGetGC (new, GCForeground, &values) ;
    }
#endif

return ;
}

/***** **** *** ** * ClassPartInitialize * ** *** **** *****/

static void
ClassPartInitialize (wClass)
    WidgetClass wClass ;
{
XcuTblWidgetClass tw = (XcuTblWidgetClass) wClass ;

/*
 * provide for inheriting tbl_class procedures
 */

if (tw->tbl_class.interpret_string ==
    (XtTblStringProc) XtInheritInterpretString)
    {
    tw->tbl_class.interpret_string = InterpretString ;
    }

return ;
}

/***** **** *** ** * Destroy * ** *** **** *****/

static void
Destroy (widget)
    Widget widget ;
{
XcuTblWidget tw = (XcuTblWidget) widget ;
Cardinal  i ;

/*
 * free dynamically allocated data
 */

if (!XtIsRealized (widget))
    return ;

XtFree ((char *) tw->tbl.r_item) ;
XtFree ((char *) tw->tbl.equal_cols) ;
for (i=0;  i < tw->tbl.rows;  i++)
    XtFree ((char *) tw->tbl.item[i]) ;

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

Trial_STS ((XcuTblWidget) w, (Widget) NULL) ;
Trial_ETS ((XcuTblWidget) w) ;
DoLayout ((XcuTblWidget) w, (Widget) NULL) ;

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
XtGeometryResult parent_result;
XtWidgetGeometry allowed ;
XcuTblWidget thisInstance = (XcuTblWidget) w->core.parent ;
XcuTblConstraints constraint = (XcuTblConstraints) w->core.constraints ;
Dimension save_child_width = w->core.width ;
Dimension save_child_height = w->core.height ;
Dimension compromise_width, compromise_height ;

/*
 * We can't allow any requests while in init phase
 */

if (thisInstance->tbl.initting)
    return XtGeometryNo ;

/*
 * Look for identical request to a prior XtGeometryAlmost -- we have to grant
 */

if (
   constraint->tbl.almost_status == True

   &&

   constraint->tbl.almost_widget == w
   
    &&

   (((constraint->tbl.almost_mode & CWWidth) && 
    (request->request_mode & CWWidth) && 
    (constraint->tbl.almost_width == request->width))
    ||
    ((constraint->tbl.almost_mode & CWWidth) ==
     (request->request_mode & CWWidth)))

    &&

   (((constraint->tbl.almost_mode & CWHeight) && 
    (request->request_mode & CWHeight) && 
    (constraint->tbl.almost_height == request->height))
    ||
    ((constraint->tbl.almost_mode & CWHeight) ==
     (request->request_mode & CWHeight)))
   )
    {
    /*
     * Granting a request identical to my last Almost reply
     */
    w->core.x = constraint->tbl.almost_x ;
    w->core.y = constraint->tbl.almost_y ;
    w->core.width = request->width ;
    w->core.height = request->height ;
    w->core.border_width = request->border_width ;

    Trial_STS ( thisInstance, w ) ;
    Trial_ETS ( thisInstance ) ;
    if (!(request->request_mode & XtCWQueryOnly))
	DoLayout  ( thisInstance, w ) ;

    constraint->tbl.almost_status = False ;
    return XtGeometryYes ;
    }

constraint->tbl.almost_status = False ;

/*
 * Unfudge the width if necessary
 */

if (thisInstance->tbl.force_width)
    request->width-- ;

/*
 * Disallow anything but size requests from the children
 */

if ((request->request_mode & (CWWidth | CWHeight)) == 0)
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

/*
 * If there really isn't anything to change, just say yes
 */

if (allowed.width == w->core.width &&
    allowed.height == w->core.height &&
    !thisInstance->tbl.force_width)
    {
    return XtGeometryYes ;
    }

thisInstance->tbl.force_width = False ;

/*
 * We'll have to do a Trial_STS to see what the effects of the change are
 */

thisInstance->tbl.pre_almost_width = w->core.width = allowed.width ;
thisInstance->tbl.pre_almost_height = w->core.height = allowed.height ;

Trial_STS ( thisInstance, w ) ;

/*
 * Run this layout by my parent
 */

parent_result = XtMakeResizeRequest ((Widget)thisInstance,
			  (Dimension)thisInstance->tbl.standalone_width,
			  (Dimension)thisInstance->tbl.standalone_height,
			  &compromise_width,
			  &compromise_height) ;

if (parent_result == XtGeometryAlmost)
    {
    /* We'll accept whatever compromise is offered */
    XtMakeResizeRequest ((Widget)thisInstance,
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

Trial_ETS ( thisInstance ) ;

if ( /* checking for nothing changed */
     (
     save_child_width == constraint->tbl.ets_width &&
     (request->request_mode & CWWidth)
     )
     &&
     (
     save_child_height == constraint->tbl.ets_height &&
     (request->request_mode & CWHeight)
     )
   )
    {
    w->core.width = save_child_width ;
    w->core.height = save_child_height ;
    return XtGeometryYes ;
    }

reply->request_mode = 0 ;

if (w->core.width != constraint->tbl.ets_width &&
    !(request->request_mode & CWWidth))
    {
    /* Need to change width even though it wasn't requested */
    reply->request_mode |= CWWidth ;
    }

if (w->core.height != constraint->tbl.ets_height &&
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
     request->width < constraint->tbl.ets_width &&
		      constraint->tbl.ets_width < save_child_width ||
     request->width > constraint->tbl.ets_width &&
		      constraint->tbl.ets_width > save_child_width
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
     request->height < constraint->tbl.ets_height &&
		      constraint->tbl.ets_height < save_child_height ||
     request->height > constraint->tbl.ets_height &&
		      constraint->tbl.ets_height > save_child_height
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
    constraint->tbl.almost_status = True ;
    constraint->tbl.almost_width = reply->width = constraint->tbl.ets_width ;
    constraint->tbl.almost_height = reply->height = constraint->tbl.ets_height ;
    constraint->tbl.almost_x = reply->x = constraint->tbl.ets_x ;
    constraint->tbl.almost_y = reply->y = constraint->tbl.ets_y ;
    reply->border_width = thisInstance->tbl.typical_border ;

    if (reply->x != w->core.x)
	reply->request_mode |= CWX ;
    if (reply->y != w->core.y)
	reply->request_mode |= CWY ;
    if (reply->border_width != w->core.border_width)
	reply->request_mode |= CWBorderWidth ;

    constraint->tbl.almost_widget = w ;
    constraint->tbl.almost_mode = reply->request_mode ;
    return XtGeometryAlmost ;
    }

/*
 * Actually do the layout
 */

if (!(request->request_mode & XtCWQueryOnly))
    DoLayout  ( thisInstance, w ) ;

return XtGeometryYes ;
}

/***** **** *** ** * SetValues * ** *** **** *****/

static Boolean
SetValues (current, request, new)
    Widget current, request, new ;
{
XcuTblWidget cw = (XcuTblWidget) current ;
XcuTblWidget nw = (XcuTblWidget) new ;
Boolean   string_changed = False ;

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

/* Check for new layout (or modification of old layout) */

if (
    (
    nw->tbl.format_file &&
     (
      !cw->tbl.format_file ||
      strcmp (nw->tbl.format_file, cw->tbl.format_file) != 0
     )
     ||
    nw->tbl.format_string &&
     (
      !cw->tbl.format_string ||
      strcmp (nw->tbl.format_string, cw->tbl.format_string) != 0
     )
    )
   )
    {
    if (XtIsRealized (current))
	{
	SetupLayout (nw) ;
	string_changed = True ;
	}
    }
else
if (cw->tbl.equal_columns != nw->tbl.equal_columns)
    {
    set_equal_cols (nw) ;
    }

if (cw->core.width != nw->core.width)
    nw->tbl.width0 = cw->core.width ;
if (cw->core.height != nw->core.height)
    nw->tbl.height0 = cw->core.height ;

if (
      string_changed
   || cw->tbl.aligned_columns != nw->tbl.aligned_columns
   || cw->tbl.clip_on_shrink != nw->tbl.clip_on_shrink
   || cw->tbl.equal_columns != nw->tbl.equal_columns
   || cw->tbl.equal_rows != nw->tbl.equal_rows
   || cw->tbl.internal_width != nw->tbl.internal_width
   || cw->tbl.internal_height != nw->tbl.internal_height
   || cw->tbl.inter_width != nw->tbl.inter_width
   || cw->tbl.inter_height != nw->tbl.inter_height
   || cw->tbl.resize_participants != nw->tbl.resize_participants
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

/***** **** *** ** * ConstraintSetValues * ** *** **** *****/

static Boolean
ConstraintSetValues (current, request, new)
    Widget current, request, new ;
{
XcuTblWidget thisInstance = (XcuTblWidget) current->core.parent ;
XcuTblConstraints constraintc = (XcuTblConstraints) current->core.constraints ;
XcuTblConstraints constraintn = (XcuTblConstraints) new->core.constraints ;

/*
 * XtSetValues looks at x,y,width,height of new widget and compares it to
 * current widget.  If different, calls geometry manager of the widget's parent.
 * So, changing fill_column here won't invoke the geometry manager, and
 * even if it did, it would get called with fill_column as it used to be.
 * I suppose I could do all the Trial_STS,ETS myself, but it would be so much
 * easier if Xt would do it for me.  Is it a bug in Xt?  Besides, it would
 * be inefficient to do all that stuff twice.  For now, I'll
 * kludge so geometry manager will get called if necessary...
 */

if (new->core.x ==	      current->core.x &&
    new->core.y ==	      current->core.y &&
    new->core.width ==	      current->core.width &&
    new->core.height ==	      current->core.height &&
    new->core.border_width == current->core.border_width)
    {
    if (constraintn->tbl.fill_column !=   constraintc->tbl.fill_column)
	{
	XtWidgetGeometry preferred ;
	/*
	XtWidgetGeometry intended ;
	intended.request_mode = (CWWidth | CWHeight) ;
	*/
	/* put new fill_column in current widget */
	constraintc->tbl.fill_column = constraintn->tbl.fill_column ;
	/* force the geometry manager to get called by fudging width */
	XtQueryGeometry (current, NULL, &preferred) ;
	new->core.width = preferred.width + 1 ;
	new->core.height = preferred.height ;
	/* set a flag so geomgr will unadjust this value */
	thisInstance->tbl.force_width = True ;
	}
    constraintc->tbl.differ_geometry = False ;
    }
else
    {
    constraintc->tbl.differ_geometry = True ;
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
XcuTblWidget tw = (XcuTblWidget) w ;
/*
 * Normally this will only be called as a result of an XtRealizeWidget or
 * an XtManageWidget.
 *
 * This widget is intended to manage a fixed number of widgets in a fixed
 * layout array.  In order to change the set of managed children significantly,
 * one should probably unmanage this widget, change the children, change
 * the layout format, and then remanage the widget.
 */

SetupLayout ((XcuTblWidget) w) ;
Trial_STS ((XcuTblWidget) w, (Widget) NULL) ;
change_managed_in_progress = 1 ;
Trial_ETS ((XcuTblWidget) w) ;
change_managed_in_progress = 0 ;
DoLayout ((XcuTblWidget) w, (Widget) NULL) ;
if (XtIsRealized (w))
    {
    XtGeometryResult  parent_result ;
    Dimension compromise_width ;
    Dimension compromise_height ;
    tw->core.height-=2 ; /* Kludge */
    parent_result = XtMakeResizeRequest (w,
			      tw->tbl.standalone_width,
			      tw->tbl.standalone_height,
			      &compromise_width,
			      &compromise_height) ;

    if (parent_result == XtGeometryAlmost)
	{
	/* We'll accept whatever compromise is offered */
	XtMakeResizeRequest (w,
			     compromise_width, compromise_height, NULL, NULL) ;
	}
    }

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
XcuTblWidget w = (XcuTblWidget) widget ;
preferred->width = w->tbl.standalone_width ;
preferred->height = w->tbl.standalone_height ;
if (w->tbl.width0 > preferred->width) preferred->width = w->tbl.width0 ;
if (w->tbl.height0 > preferred->height) preferred->height = w->tbl.height0 ;
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

#ifdef CompositeSimple

/***** **** *** ** * Redisplay * ** *** **** *****/

static void Redisplay(w, event, region)
    Widget w;
    XEvent *event;		/* unused */
    Region region;		/* unused */
{
XcuTblWidget dw = (XcuTblWidget) w ;
(*superclass->core_class.expose)( w, event, region ) ;
if (dw->simple.shadow)
    {
    XFillRectangle (XtDisplay(w), XtWindow(w), dw->tbl.background_GC,
		    dw->simple.left_facet_width, dw->simple.top_facet_width,
		    dw->core.width - dw->simple.total_facet_width,
		    dw->core.height - dw->simple.total_facet_width) ;
    }
return ;
}

#endif

/**
 ***
 **** Private Procedures
 ***
 **/

/***** **** *** ** * Trial_STS * ** *** **** *****/

/*
 * The purpose of this function is to calculate the mimimum size of the tbl
 */

static void
Trial_STS (tw, w_requester)
    XcuTblWidget tw ;
    Widget w_requester ;
{
struct s_item **item = tw->tbl.item ;
Widget child ;
int *cols = tw->tbl.cols ;
Cardinal i ;
int needed_width = 0 ;
int max_r_height = 0 ;
int *r_width = (int *) XtMalloc (tw->tbl.rows * sizeof(int)) ;
int *r_height = tw->tbl.r_height ;

/*
 * The following calculations assume that all children have the same
 * border_width.  Is it okay for this not to be true?  Probably yes,
 * but probably also a pain in the neck.
 */

/*
 * Start the adjustment process
 */

/*
 * Step 0 : Collect the widths and heights of each item
 */

for (i=0;  i < tw->tbl.rows;  i++)
    {
    Cardinal j ;

    for (j=0;  j < cols[i];  j++)
	{
	Cardinal k ;
	XcuTblConstraints constraint = (XcuTblConstraints)
				    item[i][j].pw->core.constraints ;
	/*
	 * This is a bit tricky...
	 * If the child has a new width and height because of a change in
	 * text, its parent widget's GeometryManager is called, and
	 * that's why we're here, now.  But, a call to QueryGeometry
	 * at this point will result in the child returning its OLD preferred
	 * geometry, since this Geometry manager was called with the old
	 * widget, with only the new x,y,width,height copied in.  So,
	 * I have to check for this situation and just grab the current width
	 * and height in this situation.
	 * But, if the child has only a new fill mode, we're here, too, but
	 * in this case we DO want to do a QueryGeometry. So, have to look
	 * for a special flag set during ConstraintSetValues.
	 */
	if (item[i][j].primary != TBL_ITEM)
	    continue ;
	if (item[i][j].pw == w_requester && constraint->tbl.differ_geometry)
	    {
	    item[i][j].width = w_requester->core.width ;
	    item[i][j].height = w_requester->core.height ;
	    }
	else
	    {
	    XtWidgetGeometry preferred ;
	    /*
	    XtWidgetGeometry intended ;
	    intended.request_mode = (CWWidth | CWHeight) ;
	    */
	    XtQueryGeometry (item[i][j].pw, NULL, &preferred) ;
	    item[i][j].width = preferred.width ;
	    item[i][j].height = preferred.height ;
	    }
	item[i][j].adj_width = (item[i][j].width + constraint->tbl.span_width
				- 1) / constraint->tbl.span_width ;
	for (k=1;  k < constraint->tbl.span_height;  k++)
	    item[i+k][j].adj_width = item[i][j].adj_width ;
	}
    }

/*
 * Step 1a : Get all equal length columns and all spanned row columns equal
 */

for (;;)
    {
    Boolean something_changed = False ;
    something_changed = adjust_aligned_columns (tw) ;
    something_changed |= adjust_equal_columns (tw) ;
    something_changed |= adjust_spanned_columns (tw) ;
    if (!something_changed)
	break ;
    }

/*
 * Step 1b : find the min length needed for each row 
 *  Each row needs space for:
 *    TBL_ITEMS
 *    VSPAN_ITEMS (only once per item block)
 *    borders and inter-spaces
 */

for (i=0;  i < tw->tbl.rows;  i++)
    {
    Cardinal j ;
    int num_item ;

    r_width[i] = 0 ;
    for (j=0;  j < cols[i];  j++)
	{
	if (item[i][j].primary == TBL_ITEM)
	    {
	    child = item[i][j].pw ;

	    if (tw->tbl.equal_cols[i])
		{
		/* min length is multiple of largest child */
		if (item[i][j].adj_width > r_width[i])
		    {
		    r_width[i] = item[i][j].adj_width ;
		    }
		}
	    else
		{
		/* min length is sum of children */
		r_width[i] += item[i][j].width ;
		}
	    }
	else
	if (tw->tbl.aligned_columns && item[i][j].primary == TBL_HSPAN)
	    {
	    r_width[i] += item[i][j].adj_width ;
	    }
	else
	    {
	    if (!tw->tbl.equal_cols[i])
		{
		/* Need to include vertically spanned items */
		if (item[i][j].primary == TBL_VSPAN)
		    {
		    r_width[i] += item[i][j].adj_width ;
		    }
		}
	    }
	}

    if (tw->tbl.equal_cols[i])
	{
	r_width[i] = cols[i] * r_width[i] ;
	}

    num_item = tw->tbl.equal_cols[i] ? cols[i] : tw->tbl.r_item[i] ;
    r_width[i] += (num_item - 1) *
		  (tw->tbl.inter_width + 2 * tw->tbl.typical_border) ;
    if (r_width[i] > needed_width)
	{
	needed_width = r_width[i] ;
	}
    }

/* Now adjust each row to fit within largest width */
for (i=0;  i < tw->tbl.rows;  i++)
    {
    Cardinal last_item = 0 ;
    int n_arbs = 0 ;
    int *arbs ;
    Cardinal j ;
    int slack, *portions ;

    /* First see if there are any arbs that take up all the slack */

    arbs = (int *) XtMalloc (cols[i] * sizeof(int)) ;
    for (j=0;  j < cols[i];  j++)
	{
	XcuTblConstraints constraint =
	    (XcuTblConstraints) (item[i][j].pw->core.constraints) ;
	if (constraint->tbl.fill_column)
		{
		arbs[n_arbs++] = j ;
		}
	}

    if (n_arbs && !tw->tbl.equal_cols[i])
	{
	Cardinal i_arb ;
	int arb_adjustment ;

	arb_adjustment = (int) ((float) (needed_width - r_width[i]) /
					n_arbs + 0.5) ;
	/*
	 * adjust each one by its proportion of the slack, except for the
	 * last, which must take up enough to make up for round-off errors
	 */
	for (i_arb=0;  i_arb < n_arbs-1;  i_arb++)
	    {
	    item[i][arbs[i_arb]].width += arb_adjustment ;
	    }
	item[i][arbs[n_arbs-1]].width += (needed_width - r_width[i] -
						  (n_arbs-1)*arb_adjustment) ;
	XtFree ((char *)arbs) ;
	continue ;
	}

    XtFree ((char *)arbs) ;

    /* else everybody has to adjust */

    if (tw->tbl.equal_cols[i])
	{
	/* Doesn't include the two outermost children borders */
	slack = needed_width - (cols[i] - 1) * (tw->tbl.inter_width +
		2 * tw->tbl.typical_border) ;
	portions = (int *) XtMalloc (cols[i] * sizeof(int)) ;

	e_apportion_slack (slack, cols[i], portions) ;

	for (j=0;  j < cols[i];  j++)
	    {
	    if (item[i][j].primary == TBL_ITEM ||
		item[i][j].primary == TBL_HSPAN)
		{
		if (item[i][j].primary == TBL_ITEM)
		    {
		    last_item = j ;
		    item[i][last_item].width = 0 ;
		    }
		else
		    {
		    item[i][last_item].width +=
			(tw->tbl.inter_width + 2 * tw->tbl.typical_border) ;
		    }
		item[i][last_item].width += portions[j] ;
		}
	    }
	XtFree ((char *) portions) ;
	}
    else
	{
	int last_child = -1 ;
	int padding = (tw->tbl.r_item[i] - 1) *
	    (tw->tbl.inter_width + 2 * tw->tbl.typical_border) ;

	for (j=0;  j < cols[i];  j++)
	    {
	    /* each item is expanded by its proportion of width */
	    /* unless aligned columns is in effect, in which case */
	    /* every row after the first is merely set equal to the */
	    /* previous row */
	    if (tw->tbl.aligned_columns && i > 0)
		{
		if (item[i][j].primary == TBL_ITEM)
		    {
		    item[i][j].width = item[0][j].aligned_width ;
		    last_child = j ;
		    }
		else
		if (item[i][j].primary == TBL_HSPAN)
		    {
		    item[i][last_child].width += item[0][j].aligned_width ;
		    item[i][j].width = 0 ;
		    }
		}
	    else
		{
		item[i][j].width = (int)((float) (needed_width - padding) /
		      (r_width[i] - padding) * item[i][j].width + 0.5) ;
		if (tw->tbl.aligned_columns)
		    {
		    item[i][j].aligned_width = item[i][j].width ;
		    if (item[i][j].primary == TBL_ITEM)
			{
			last_child = j; 
			}
		    else
			{
			item[i][last_child].width += item[i][j].width ;
			}
		    }
		else
		    last_child = j ;
		}
	    }

	if (!tw->tbl.equal_cols[i] && last_child >= 0)
	    {
	    /* need to adjust the last item in this row to take up any slack */
	    int adj ;
	    int wide = 0 ;

	    for (j=0;  j < cols[i];  j++)
		{
		if (j == 0 || item[i][j].pw != item[i][j-1].pw)
		    {
		    wide += item[item[i][j].pi][item[i][j].pj].width ;
		    }
		}
	    adj = needed_width - wide -
		(tw->tbl.r_item[i] - 1) *
		(tw->tbl.inter_width + 2 * tw->tbl.typical_border) ;
	    item[i][last_child].width += adj ;
	    }
	}
    }

/*
 * Now, do the same thing for row equalization 
 * The process for the rows is somewhat simpler since there can not be a
 * variable number of rows per column (except that caused by spanning).
 */

/* Step 1 : find the min height needed for each row */

for (i=0;  i < tw->tbl.rows;  i++)
    {
    Cardinal j ;
    r_height[i] = 0 ;
    for (j=0;  j < cols[i];  j++)
	{
	if (item[i][j].primary == TBL_ITEM)
	    {
	    int child_height ;
	    XcuTblConstraints constraint ;
	    child = item[i][j].pw ;
	    constraint = (XcuTblConstraints)child->core.constraints;

	    /* height of a row is its largest child */
	    child_height = item[i][j].height / constraint->tbl.span_height ;
	    if (child_height > r_height[i])
		r_height[i] = child_height ;
	    }
	else
	if (item[i][j].primary == TBL_VSPAN)
	    {
	    /*
	     * Need to include proper fraction of vertically spanned items
	     * Simple because it is illegal to have a variable number of columns
	     * during vertical spanning
	     */
	    int child_height ;
	    XcuTblConstraints constraint =
		(XcuTblConstraints)item[i][j].pw->core.constraints;

	    child_height = item[i][j].height / constraint->tbl.span_height ;
	    if (tw->tbl.equal_rows == True)
		child_height = item[i][j].height ;
	    if (child_height > r_height[i])
		r_height[i] = child_height ;
	    }
	}
    if (r_height[i] > max_r_height)
	max_r_height = r_height[i] ;
    }

/* if keyword "equal_row_key", all rows changed to maximum row height */
if (tw->tbl.equal_rows == True)
    {
    for (i=0;  i < tw->tbl.rows;  i++)
	{
	r_height[i] = max_r_height ;
	}
    }

/* Now adjust each row to fit within its specified height */

for (i=0;  i < tw->tbl.rows;  i++)
    {
    Cardinal j ;
    for (j=0;  j < cols[i];  j++)
	{
	if (item[i][j].primary == TBL_ITEM)
	    {
	    int ii ;
	    int sum_height = 0 ;
	    XcuTblConstraints constraint ;
	    child = item[i][j].pw ;
	    constraint = (XcuTblConstraints)child->core.constraints;

	    for (ii=0;  ii < constraint->tbl.span_height;  ii++)
		{
		sum_height += r_height[i+ii] ;
		}
	    sum_height += 
		(constraint->tbl.span_height - 1) *
		(tw->tbl.inter_height + 2 * tw->tbl.typical_border) ;
	    if (XtIsManaged(child))
		{
		item[i][j].height = sum_height ;
		}
	    }
	}
    }

tw->tbl.standalone_width = needed_width +
	       2 * tw->tbl.typical_border +
	       2 * tw->tbl.internal_width  ;

/* Now one final pass to add in allowances for borders and padding */

tw->tbl.standalone_height = 0 ;
for (i=0;  i < tw->tbl.rows;  i++)
    {
    Widget last_pw = NULL ;
    Cardinal j ;

    tw->tbl.standalone_height += r_height[i] ;
    for (j=0;  j < cols[i];  j++)
	{
	if (item[i][j].primary == TBL_ITEM)
	    {
	    if (XtIsManaged(item[i][j].pw))
		{
		XcuTblConstraints constraint =
			(XcuTblConstraints) item[i][j].pw->core.constraints ;
		constraint->tbl.sts_width = item[i][j].width ;
		constraint->tbl.sts_height = item[i][j].height ;
		}
	    }
	if (item[i][j].pw != last_pw)
	    {
	    last_pw = item[i][j].pw ;
	    }
	}
    }

tw->tbl.standalone_height += (tw->tbl.rows-1) * tw->tbl.inter_height +
		 2 * tw->tbl.internal_height +
		 2 * tw->tbl.rows * tw->tbl.typical_border ;

#ifdef CompositeSimple
tw->tbl.standalone_width += tw->simple.total_facet_width ;
tw->tbl.standalone_height += tw->simple.total_facet_width ;
#endif
/*
 * cleanup
 */

XtFree ((char *)r_width) ;

return ;
}

/***** **** *** ** * Trial_ETS * ** *** **** *****/

static void
Trial_ETS (tw)
    XcuTblWidget tw ;
{
Cardinal i ;
int c_slack ;  /* Can't be "Dimension" */
int r_slack ;
int *p_r_slack = (int *) XtMalloc (tw->tbl.rows * sizeof(int)) ;
int *r_height = tw->tbl.r_height ;
float c_expansion ;
float r_expansion ;
int internal_width = tw->tbl.internal_width ;
int internal_height = tw->tbl.internal_height ;
int inter_width = tw->tbl.inter_width ;
int inter_height = tw->tbl.inter_height ;
int inter_width_delta ;
int inter_height_delta ;

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
 *  2) Expand the internalWidth,Height (centers the tbl)
 *  3) Expand the interWidgetWidth,Height (spreads them out)
 *
 *  This code allows any combination of the above possibilities.
 *  (If no bits are set, the default is to expand the widgets)
 */

/*
 * Compute how much bigger/smaller (the slack) than the minimum we are.
 */

/* height */

if (tw->core.height == 0 ||
    (XtIsRealized((Widget)tw) && change_managed_in_progress)) /* Hack */
    tw->core.height = tw->tbl.standalone_height ;

if (tw->core.height < tw->tbl.standalone_height && tw->tbl.clip_on_shrink)
    tw->tbl.new_height = tw->tbl.standalone_height ;
else
    tw->tbl.new_height = tw->core.height ;

r_expansion = (float) tw->tbl.new_height / tw->tbl.standalone_height ;
r_slack = tw->tbl.new_height - tw->tbl.standalone_height ;

/* width */

if (tw->core.width == 0)
    tw->core.width = tw->tbl.standalone_width ;

if (tw->core.width < tw->tbl.standalone_width && tw->tbl.clip_on_shrink)
    tw->tbl.new_width = tw->tbl.standalone_width ;
else
    tw->tbl.new_width = tw->core.width ;

c_expansion = (float) tw->tbl.new_width / tw->tbl.standalone_width ;
c_slack = tw->tbl.new_width - tw->tbl.standalone_width ;

/*
 * Depending on what is allowed to change,
 * compute new padding parameters
 */

if ((int)tw->tbl.resize_participants & (int)XcuRESIZE_INTERNALS)
     {
     if (((int)tw->tbl.resize_participants & (int)XcuRESIZE_INTERS) &&
         ((int)tw->tbl.resize_participants & (int)XcuRESIZE_CHILDREN))
	 {
	 /* internal padding and inter-widget just expand proportionately */
	 internal_height = tw->tbl.internal_height * r_expansion ;
	 inter_height = tw->tbl.inter_height * r_expansion ;
	 r_slack -= 2 * (internal_height - tw->tbl.internal_height) ;
	 r_slack -= (tw->tbl.rows - 1) *
		    (inter_height - tw->tbl.inter_height) ;

	 internal_width = tw->tbl.internal_width * c_expansion ;
	 inter_width = tw->tbl.inter_width * c_expansion ;
	 c_slack -= 2 * (internal_width - tw->tbl.internal_width) ;
	 c_slack -= (tw->tbl.max_cols - 1) *
		    (inter_width - tw->tbl.inter_width) ;
	 }
     else
     if (!((int)tw->tbl.resize_participants & (int)XcuRESIZE_INTERS) &&
          ((int)tw->tbl.resize_participants & (int)XcuRESIZE_CHILDREN))
	 {
	 /*
	  * For now, internal padding just expands proportionately.
	  * That means widgets will have to expand just a bit more than
	  * their fair share (also expanding for all of inters' share)
	  * This should be quite an adequate approximation
	  */
	 internal_height = tw->tbl.internal_height * r_expansion ;
	 r_slack -= 2 * (internal_height - tw->tbl.internal_height) ;

	 internal_width = tw->tbl.internal_width * c_expansion ;
	 c_slack -= 2 * (internal_width - tw->tbl.internal_width) ;
	 }
     else
     if ( ((int)tw->tbl.resize_participants & (int)XcuRESIZE_INTERS) &&
         !((int)tw->tbl.resize_participants & (int)XcuRESIZE_CHILDREN))
	 {
	 /* split up the slack between inter widget and internals */
	 int row_num_participants = (tw->tbl.rows - 1) + 2 ;
	 int col_num_participants = (tw->tbl.max_cols - 1) + 2 ;
	 inter_height = tw->tbl.inter_height +
				   r_slack / row_num_participants ;
	 internal_height =
		tw->tbl.internal_height +
		(r_slack -
		(inter_height - tw->tbl.inter_height) *
		(row_num_participants - 2)) / 2 ;
	 r_slack -= (2 * (internal_height - tw->tbl.internal_height) +
		     (row_num_participants - 2) *
		     (inter_height - tw->tbl.inter_height)) ;

	 inter_width = tw->tbl.inter_width +
				   c_slack / col_num_participants ;
	 internal_width =
		tw->tbl.internal_width +
		(c_slack -
		(inter_width - tw->tbl.inter_width) *
		(col_num_participants - 2)) / 2 ;
	 c_slack -= (2 * (internal_width - tw->tbl.internal_width) +
		     (col_num_participants - 2) *
		     (inter_width - tw->tbl.inter_width)) ;
	 }
     else
	 {
	 /* internal padding takes up all the slack */
	 internal_height = tw->tbl.internal_height + r_slack/2 ;
	 r_slack -= 2 * (internal_height - tw->tbl.internal_height) ;

	 internal_width = tw->tbl.internal_width + c_slack/2 ;
	 c_slack -= 2 * (internal_width - tw->tbl.internal_width) ;
	 }
     }
else
if ((int)tw->tbl.resize_participants & (int)XcuRESIZE_INTERS)
    {
    if ((int)tw->tbl.resize_participants & (int)XcuRESIZE_CHILDREN)
	{
	 /*
	  * For now, inter widget padding just expands proportionately.
	  * That means widgets will have to expand just a bit more than
	  * their fair share (also expanding for all of internals' share)
	  * This should be quite an adequate approximation
	  */
	 if (tw->tbl.inter_height > 0)
	     inter_height = tw->tbl.inter_height * r_expansion ;
	 r_slack -= (tw->tbl.max_cols - 1) *
		    (inter_height - tw->tbl.inter_height) ;

	 if (tw->tbl.inter_width > 0)
	     inter_width = tw->tbl.inter_width * c_expansion ;
	 c_slack -= (tw->tbl.max_cols - 1) *
		    (inter_width - tw->tbl.inter_width) ;
	}
    else
	 {
	 /* inter widget padding takes up all the slack */
	 if (tw->tbl.rows > 1)
	     inter_height = tw->tbl.inter_height + r_slack /
				    (int) (tw->tbl.rows - 1) ;
	 r_slack -= (tw->tbl.rows - 1) *
		    (inter_height - tw->tbl.inter_height) ;

	 if (tw->tbl.max_cols > 1)
	     {
	     inter_width = tw->tbl.inter_width + c_slack /
				    (int) (tw->tbl.max_cols - 1) ;
	     }
	 c_slack -= (tw->tbl.max_cols - 1) *
		    (inter_width - tw->tbl.inter_width) ;
	 }
    }

inter_width_delta = inter_width - tw->tbl.inter_width ;
inter_height_delta = inter_height - tw->tbl.inter_height ;

/*
 * else widgets take up all the slack
 */

v_apportion_slack (r_slack, tw->tbl.rows, p_r_slack, r_height) ;

for (i=0;  i < tw->tbl.rows;  i++)
    {
    int *p_c_slack = (int *)XtMalloc(tw->tbl.cols[i] * sizeof (int)) ;
    Cardinal j ;
    Cardinal slack_ptr = 0 ;
    int this_c_slack ;

    /*
     * we need to apportion the slack evenly over the widgets in the row
     * ( if the columns were supposed to be even, I suppose a one pixel
     *   difference won't be too bad. E.g., adding a slack of two pixels
     *   to 3 even columns will result in the last column being one short.
     *   Insisting on rounding up the slack is conceivable but would probably
     *   result in more extensive negotiations in the hierarchy.  I'll yield
     *   unilaterally.)
     */

    this_c_slack = c_slack +
		   inter_width_delta * (tw->tbl.max_cols - tw->tbl.cols[i]) ;
    /*
     * If the items in the row are of variable lengths we want to apportion
     * the slack in proportion to the width of each item, but
     * if the row has "equal_cols" then we want to allocate the space equally
     * for each column.
     */
    if (tw->tbl.equal_cols[i])
	{
	e_apportion_slack (this_c_slack, tw->tbl.cols[i], p_c_slack) ;
	for (j=0;  j < tw->tbl.cols[i];  j++)
	    {
	    if (tw->tbl.item[i][j].primary == TBL_ITEM)
		{
		if (XtIsManaged(tw->tbl.item[i][j].pw))
		    {
		    Cardinal k ;
		    XcuTblConstraints constraint =
			(XcuTblConstraints) tw->tbl.item[i][j].pw->core.constraints ;
		    constraint->tbl.ets_width = constraint->tbl.sts_width ;
		    for (k=0;  k < constraint->tbl.span_width;  k++)
			{
			/*
			 * Add in the appropriate slack for each column entry
			 * that it spans
			 */
			constraint->tbl.ets_width += p_c_slack[slack_ptr++] ;
			if (k != 0)
			    {
			    /*
			     * Also add in the amount of space between entries
			     */
			    constraint->tbl.ets_width += inter_width_delta ;
			    }
			}

		    /* similar computation for rows */

		    constraint->tbl.ets_height = constraint->tbl.sts_height ;
		    for (k=0;  k < constraint->tbl.span_height;  k++)
			{
			constraint->tbl.ets_height += p_r_slack[i+k] ;
			if (k != 0)
			    {
			    constraint->tbl.ets_height += inter_height_delta ;
			    }
			}
		    }
		}
	    }
	}
    else
	{
	int n_widths = 0 ;
	int *r_widths = (int *) XtMalloc (tw->tbl.cols[i] * sizeof(int)) ;

	/* Find the widths of the true items in this row */
	for (j=0;  j < tw->tbl.cols[i];  j++)
	    {
	    if (tw->tbl.item[i][j].primary == TBL_ITEM ||
	        tw->tbl.item[i][j].primary == TBL_VSPAN)
		{
		if (XtIsManaged(tw->tbl.item[i][j].pw))
		    {
		    XcuTblConstraints constraint =
			(XcuTblConstraints)tw->tbl.item[i][j].pw->core.constraints;
		    r_widths[n_widths++] = constraint->tbl.sts_width ;
		    }
		}
	    }

	v_apportion_slack (this_c_slack, n_widths, p_c_slack, r_widths) ;

	for (j=0;  j < tw->tbl.cols[i];  j++)
	    {
	    if (tw->tbl.item[i][j].primary == TBL_ITEM)
		{
		if (XtIsManaged(tw->tbl.item[i][j].pw))
		    {
		    Cardinal k ;
		    XcuTblConstraints constraint =
			(XcuTblConstraints) tw->tbl.item[i][j].pw->core.constraints ;
		    constraint->tbl.ets_width = constraint->tbl.sts_width ;
		    constraint->tbl.ets_width += p_c_slack[slack_ptr++] ;

		    /* similar computation for rows */

		    constraint->tbl.ets_height = constraint->tbl.sts_height ;
		    for (k=0;  k < constraint->tbl.span_height;  k++)
			{
			constraint->tbl.ets_height += p_r_slack[i+k] ;
			if (k != 0)
			    {
			    constraint->tbl.ets_height += inter_height_delta ;
			    }
			}
		    }
		}
	    }
	}

    XtFree ((char *) p_c_slack) ;
    }
XtFree ((char *) p_r_slack) ;

/*
 * Now to actually configure the children...
 */

 {
 Position current_y = internal_height ;
 Position *e_current_y = NULL ;
 Boolean vspan_current = False ;

#ifdef CompositeSimple
 current_y += tw->simple.top_facet_width ;
#endif
 for (i=0;  i < tw->tbl.rows;  i++)
    {
    Widget last_pw = NULL ;
    Position current_x = internal_width ;
    Cardinal j ;
    XcuTblConstraints constraint =
	    (XcuTblConstraints) tw->tbl.item[i][0].pw->core.constraints ;

#ifdef CompositeSimple
    current_x += tw->simple.left_facet_width ;
#endif
    /*
     * If this is the first row of a series of rows with equal columns...
     */

    if (!vspan_current &&
       (tw->tbl.vspan_status[i] && (i == 0 || !tw->tbl.vspan_status[i-1])))
	{
	vspan_current = True ;
	e_current_y = (Position *)
	  	      XtMalloc (tw->tbl.cols[i] * sizeof (Position)) ;
	for (j=0;  j < tw->tbl.cols[i];  j++)
		e_current_y[j] = current_y ;
	}

    /*
     * else if this is the first row of a series of rows with unequal columns...
     */

    else
    if (vspan_current &&
        (!tw->tbl.vspan_status[i] && (i == 0 || tw->tbl.vspan_status[i-1])))
	{
	vspan_current = False ;
	if (e_current_y)
	    {
	    current_y = e_current_y[0] ; /* all should be the same */
	    XtFree ((char *) e_current_y) ;
	    e_current_y = NULL ;
	    }
	}

    /*
     * if vspan_current and a new number of columns...
     */

    if (vspan_current &&
	i != 0 &&
	tw->tbl.cols[i] != tw->tbl.cols[i-1] &&
	tw->tbl.vspan_status[i-1])
	{
	current_y = e_current_y[0] ;
	XtFree ((char *) e_current_y) ;
	e_current_y = (Position *)
	  	      XtMalloc (tw->tbl.cols[i] * sizeof (Position)) ;
	for (j=0;  j < tw->tbl.cols[i];  j++)
		e_current_y[j] = current_y ;
	}

    for (j=0;  j < tw->tbl.cols[i];  j++)
	{
	XcuTblConstraints constraint =
		(XcuTblConstraints) tw->tbl.item[i][j].pw->core.constraints ;
	if (tw->tbl.item[i][j].primary == TBL_ITEM)
	    {
	    if (XtIsManaged(tw->tbl.item[i][j].pw))
		{
		Position this_y ;
		this_y = vspan_current ? e_current_y[j] : current_y ;
		constraint->tbl.ets_x = current_x ;
		constraint->tbl.ets_y = this_y ;
		}
	    }

	if (vspan_current && (tw->tbl.item[i][j].primary == TBL_ITEM ||
			      tw->tbl.item[i][j].primary == TBL_HSPAN))
	    {
	    e_current_y[j] += constraint->tbl.ets_height +
			      inter_height + 2*tw->tbl.typical_border ;
	    }
	if (tw->tbl.item[i][j].pw != last_pw)
	    {
	    current_x += constraint->tbl.ets_width + inter_width +
			 2*tw->tbl.typical_border ;
	    last_pw = tw->tbl.item[i][j].pw ;
	    }
	}

    if (!vspan_current)
	{
	current_y += constraint->tbl.ets_height +
		     inter_height + 2*tw->tbl.typical_border ;
	}
    }
 if (e_current_y)
    {
    XtFree ((char *) e_current_y) ;
    }
 }

return ;
}

/***** **** *** ** * DoLayout * ** *** **** *****/

static void
DoLayout (tw, requesting_widget)
    XcuTblWidget tw ;
    Widget requesting_widget ;
{
int num_children = tw->composite.num_children;
WidgetList children = tw->composite.children;
Widget *childP;

/* Use ConfigureWidget on siblings, modify requester directly */

for (childP = children; childP - children < num_children; childP++)
    {
    XcuTblConstraints twc = (XcuTblConstraints)(*childP)->core.constraints;

    if (!XtIsManaged(*childP))
	continue ;

    if (*childP != requesting_widget)
	{
	XtConfigureWidget (*childP,
			twc->tbl.ets_x,
			twc->tbl.ets_y,
			(Dimension) twc->tbl.ets_width,
			(Dimension) twc->tbl.ets_height,
			(Dimension) tw->tbl.typical_border) ;
	}
    else
	{
	requesting_widget->core.x = twc->tbl.ets_x ;
	requesting_widget->core.y = twc->tbl.ets_y ;
	requesting_widget->core.width = twc->tbl.ets_width ;
	requesting_widget->core.height = twc->tbl.ets_height ;
	requesting_widget->core.border_width = tw->tbl.typical_border ;
	/*
	 * Well, this (XtConfigureWidget) isn't supposed to be here, but
	 * without it, the child's x & y never seem to be paid attention
	 * to.  Width and height change, but the widget stays right where
	 * it was...
	 */
	/***
	XtConfigureWidget (*childP, twc->tbl.ets_x, twc->tbl.ets_y,
				    (Dimension) twc->tbl.ets_width,
				    (Dimension) twc->tbl.ets_height,
				    (Dimension) tw->tbl.typical_border) ;
	***/
	}
    }

tw->core.width = tw->tbl.new_width ;
tw->core.height = tw->tbl.new_height ;

return ;
}

/***** **** *** ** * adjust_aligned_columns * ** *** **** *****/

static Boolean
adjust_aligned_columns (tw)
    XcuTblWidget tw ;
{
/*
 * All items in a column need to be the same width if aligned_columns is True
 * Therefore, find biggest member in each column,
 *  and set all items of the column to that width.
 */
Cardinal i, j ;
Boolean change_value = False ;

if (!tw->tbl.aligned_columns)
    return False ;

for (j=0;  j < tw->tbl.cols[0];  j++)
    {
    int max_width = 0 ;
    int max_adj_width = 0 ;
    for (i=0;  i < tw->tbl.rows;  i++)
	{
	if (tw->tbl.item[i][j].adj_width > max_adj_width)
	    {
	    max_width = tw->tbl.item[i][j].width ;
	    max_adj_width = tw->tbl.item[i][j].adj_width ;
	    }
	}

    for (i=0;  i < tw->tbl.rows;  i++)
	{
	if (tw->tbl.item[i][j].adj_width < max_adj_width)
	    {
	    change_value = True ;
	    tw->tbl.item[i][j].adj_width = max_adj_width ;
	    tw->tbl.item[i][j].width = max_width ;
	    }
	}
    }
return change_value ;
}

/***** **** *** ** * adjust_equal_columns * ** *** **** *****/

static Boolean
adjust_equal_columns (tw)
    XcuTblWidget tw ;
{
/*
 * All columns in a row marked with an 'e', need to be same width.
 * Therefore, find biggest member, and set all members of the set to that width.
 */
Cardinal i, j ;
Boolean change_value = False ;

for (i=0;  i < tw->tbl.rows;  i++)
    {
    int max_width = 0 ;
    int max_adj_width = 0 ;
    for (j=0;  j < tw->tbl.cols[i];  j++)
	{
	if (tw->tbl.item[i][j].e && tw->tbl.item[i][j].adj_width >max_adj_width)
	    {
	    max_width = tw->tbl.item[i][j].width ;
	    max_adj_width = tw->tbl.item[i][j].adj_width ;
	    }
	}

    /* Only need to adjust if some columns were marked */
    if (max_width > 0)
	{
	for (j=0;  j < tw->tbl.cols[i];  j++)
	    {
	    if (tw->tbl.item[i][j].e && tw->tbl.item[i][j].adj_width <
					max_adj_width)
		{
		change_value = True ;
		tw->tbl.item[i][j].adj_width = max_adj_width ;
		tw->tbl.item[i][j].width = max_width ;
		}
	    }
	}
    }
return change_value ;
}

/***** **** *** ** * adjust_spanned_columns * ** *** **** *****/

static Boolean
adjust_spanned_columns (tw)
    XcuTblWidget tw ;
{
/*
 * All corresponding columns in spanned rows need to be same width.
 * Therefore, find biggest member, and set all members of the set to that width.
 */
Cardinal i, j ;
Boolean change_value = False ;

for (i=0;  i < tw->tbl.rows;  i++)
    {
    Cardinal ii = i ;
    /*
     * Look for the start of a spanning run
     */
    if (tw->tbl.vspan_status[i] && (i == 0 || !tw->tbl.vspan_status[i-1]))
	{
	/*
	 * For each column, search down through the spans
	 */
	for (j=0;  j < tw->tbl.cols[i];  j++)
	    {
	    int max_width = 0 ;
	    int max_adj_width = 0 ;
	    int filled_columns = 0 ;
	    for (ii=i;  ii < tw->tbl.rows && tw->tbl.vspan_status[ii];  ii++)
		{
		XcuTblConstraints constraint =
		   (XcuTblConstraints)tw->tbl.item[ii][j].pw->core.constraints ;
		if (constraint->tbl.fill_column)
		    filled_columns++ ;
		if (tw->tbl.item[ii][j].primary == TBL_ITEM &&
		    tw->tbl.item[ii][j].adj_width > max_adj_width)
		    {
		    max_adj_width = tw->tbl.item[ii][j].adj_width ;
		    max_width = tw->tbl.item[ii][j].width ;
		    }
		}
	    /*
	     * If some, but not all, of the entries in this column's spanning
	     * run were set to take up slack, then they must all be so set...
	     */
	    if (filled_columns > 0 && filled_columns < ii - i)
		{
		for (ii=i; ii < tw->tbl.rows && tw->tbl.vspan_status[ii];  ii++)
		    {
		    XcuTblConstraints constraint =
		       (XcuTblConstraints)tw->tbl.item[ii][j].pw->core.constraints;
		    constraint->tbl.fill_column = True ;
		    }
		}
	    /* Only need to adjust if some columns were marked */
	    if (max_adj_width > 0)
		{
		for (ii=i; ii < tw->tbl.rows && tw->tbl.vspan_status[ii]; ii++)
		    {
		    if (tw->tbl.item[ii][j].primary != TBL_HSPAN &&
			tw->tbl.item[ii][j].adj_width < max_adj_width)
			{
			change_value = True ;
			tw->tbl.item[ii][j].adj_width = max_adj_width ;
			tw->tbl.item[ii][j].width = max_width ;
			}
		    }
		}
	    }
	}
    i = ii ; /* bump past the spanning run */
    }
return change_value ;
}

/***** **** *** ** * v_apportion_slack * ** *** **** *****/

static void
v_apportion_slack (slack, n, portions, widths)
    int slack ;
    int n ;
    int *portions ;
    int *widths ;
{
Cardinal i ;
float exact_portion ;
float running_exact_portion = 0.0 ;
int apportioned = 0 ;
int total_width = 0 ;

if (n == 0) return ;

for (i=0;  i < n;  i++)
    {
    total_width += widths[i] ;
    }

for (i=0;  i < n-1;  i++)
    {
    exact_portion = (float) widths[i] / total_width * slack ;
    running_exact_portion += exact_portion ;
    portions[i] = running_exact_portion - apportioned ;
    apportioned += portions[i] ;
    }

portions[n-1] = slack - apportioned ;
return ;
}

/***** **** *** ** * e_apportion_slack * ** *** **** *****/

static void
e_apportion_slack (slack, n, portions)
    int slack ;
    int n ;
    int *portions ;
{
Cardinal i ;
float exact_portion = ((float) slack + 0.999) / n ;
float running_exact_portion = 0.0 ;
int apportioned = 0 ;

for (i=0;  i < n;  i++)
    {
    running_exact_portion += exact_portion ;
    portions[i] = running_exact_portion - apportioned ;
    apportioned += portions[i] ;
    }

return ;
}

/***** **** *** ** * SetupLayout * ** *** **** *****/

static void
SetupLayout (tw)
    XcuTblWidget tw ;
{
/*
 * Don't want to count children that have:
 *	core/mappedWhenManaged "False"
 *                  and
 *	constraint/layoutWhenUnmapped "False"
 */

if (tw->tbl.format_file != NULL) 
    {
    /*
     * Layout is contained in a file.
     * We'll read the file as one big string.
     */
    FILE *fd ;
    struct stat buf ;

    tw->tbl.format_mode = TBL_STRING_FORMAT ;
    fd = fopen (tw->tbl.format_file, "r") ;
    if ((int)fd <= 0)
	{
	char *my_text = "XcuTbl: Couldn't open (%s) for reading\n" ;
	char *etext = XtMalloc (strlen(my_text) + strlen(tw->tbl.format_file)) ;
	sprintf (etext, my_text, tw->tbl.format_file) ;
	XtError (etext) ;
	}
    fstat (fileno(fd), &buf) ;
    layout_input->input_buffer = XtMalloc (buf.st_size + 1) ;
    fread (layout_input->input_buffer, (int)buf.st_size, 1, fd) ;
    layout_input->input_buffer[buf.st_size] = '\0' ;
    layout_input->string_file = (int) TBL_FILE_FORMAT ;

    Xcu_copy_ds (&layout_input->cur_file, tw->tbl.format_file) ;
    XtFree (tw->tbl.format_file) ;
    tw->tbl.format_file = NULL ;

    fclose (fd) ;
    }
else
if (tw->tbl.format_string != NULL)
    {
    /*
     * Layout is contained in a string.
     */
    tw->tbl.format_mode = TBL_STRING_FORMAT ;
    Xcu_copy_ds (&layout_input->cur_file, "(String Resource)") ;
    layout_input->input_buffer = tw->tbl.format_string ;
    layout_input->string_file = (int) TBL_STRING_FORMAT ;
    }
else
if (tw->tbl.format_mode == TBL_FIRST_PASS)
    {
    /* default to a row of widgets */
    Cardinal i ;
    tw->tbl.format_mode = TBL_STRING_FORMAT ;
    Xcu_copy_ds (&layout_input->cur_file, "(Default Resource)") ;
    tw->tbl.format_string = XtCalloc (tw->composite.num_children * 2 + 2,
				  sizeof(char)) ;
    layout_input->input_buffer = tw->tbl.format_string ;
    layout_input->string_file = (int) TBL_STRING_FORMAT ;
    for (i=0;  i < tw->composite.num_children;  i++)
	strcat (tw->tbl.format_string, " c") ;
    strcat (tw->tbl.format_string, ".") ;
    }

if (tw->tbl.format_mode != TBL_NO_FORMAT)
    {
    layout_input->input_buffer = 
	(*((XcuTblWidgetClass)XtClass((Widget)tw))->tbl_class.interpret_string)
	(layout_input->input_buffer) ;
    layout_input->input_pointer = 0 ;
    layout_input->unput_pointer = 0 ;
    layout_input->context_ptr = 0 ;
    layout_input->line_number = 1 ;
    layout_input->lnl_equivalent = -1 ;
    layout_input->snl_equivalent = -1 ;
    if (XtIsRealized ((Widget)tw))
	{
	Cardinal i ;
	for (i=0;  i < tw->tbl.rows;  i++)
	    {
	    XtFree ((char *) tw->tbl.item[i]) ;
	    tw->tbl.cols[i] = 0 ;
	    }
	}
    tw->tbl.rows = 0 ;
    tw->tbl.data_row = 0 ;
    tw->tbl.n_cols = 0 ;
    ParseTblLayout (tw) ;
    tw->tbl.initting = True ;
    InitLayout (tw) ;
    tw->tbl.initting = False ;
    }

/*
 * We don't want to parse again unless we have to.
 */

/***
XtFree (tw->tbl.format_string) ;
tw->tbl.format_string = NULL ;
tw->tbl.format_mode = TBL_NO_FORMAT ;
***/

return ;
}

/***** **** *** ** * InterpretString * ** *** **** *****/

static String
InterpretString (string)
    char *string ;
{
return string ;
}

/***** **** *** ** * InitLayout * ** *** **** *****/

static void
InitLayout (tw)
    XcuTblWidget tw ;
{
struct s_item **item = tw->tbl.item ;
int *cols = tw->tbl.cols ;
WidgetList children = tw->composite.children ;
Widget child ;

/* this is for pointers from spanning entries back to the originating item */

int n_item = 0 ;
Cardinal i, j ;
Cardinal nth_child ;

/*
 * Find the number of true items.
 * Also find the number of "items" in each row, where an "item" is a sequence
 * of vertical spans.
 */

if (XtIsRealized ((Widget)tw))
    {
    XtFree ((char *) tw->tbl.r_height) ;
    XtFree ((char *) tw->tbl.r_item) ;
    }
tw->tbl.r_height = (int *) XtMalloc (tw->tbl.rows * sizeof (int)) ;
tw->tbl.r_item = (int *) XtMalloc (tw->tbl.rows * sizeof (int)) ;
tw->tbl.max_cols = 0 ;

for (i=0;  i < tw->tbl.rows;  i++)
    {
    tw->tbl.r_item[i] = 0 ;
    if (cols[i] > tw->tbl.max_cols)
	tw->tbl.max_cols = cols[i] ;
    for (j=0;  j < cols[i];  j++)
	{
	if (item[i][j].primary == TBL_ITEM)
	    {
	    /*
	     * Check for impossible situations
	     */
	    if (
	       i != 0 && j != 0 &&
	       item[i-1][j].primary == TBL_HSPAN &&
	       item[i][j-1].primary == TBL_VSPAN
	       )
		{
		char *text1 = "XcuTbl: Illegal placement of tbl item:\n" ;
		char *text2 = "     An 's' above with '^' to the left\n" ;
		char *etext = XtMalloc (strlen(text1) + strlen(text2) + 1) ;
		sprintf (etext, "%s%s", text1, text2) ;
		XtError (etext) ;
		}
	    /*
	     * Note this item
	     */
	    n_item++ ;
	    tw->tbl.r_item[i]++ ;
	    }
	else
	if (item[i][j].primary == TBL_HSPAN)
	    {
	    /*
	     * Check for impossible situations
	     */
	    if (j == 0)
		{
		char *text1 = "XcuTbl: Illegal placement of 's' span item:\n" ;
		char *text2 = "     First entry in a row\n" ;
		char *etext = XtMalloc (strlen(text1) + strlen(text2) + 1) ;
		sprintf (etext, "%s%s", text1, text2) ;
		XtError (etext) ;
		}

	    if (j != 0 && item[i][j-1].primary == TBL_VSPAN)
		{
		/*
		 * I assume he meant this...
		 */
		item[i][j].primary = TBL_VSPAN ;
		if (i != 0 && item[i-1][j].primary == TBL_ITEM)
		    {
		    /*
		     * Nope, he was confused
		     */
		    char *text1 = "XcuTbl: Illegal placement of 's' span item:\n" ;
		    char *text2 = "     An entry above with '^' to the left\n" ;
		    char *etext = XtMalloc (strlen(text1) + strlen(text2) + 1) ;
		    sprintf (etext, "%s%s", text1, text2) ;
		    XtError (etext) ;
		    }
		}
	    }
	else
	if (item[i][j].primary == TBL_VSPAN)
	    {
	    if (i == 0)
		{
		char *text1 = "XcuTbl: Illegal placement of '^' span item:\n" ;
		char *text2 = "     Entry in first row\n" ;
		char *etext = XtMalloc (strlen(text1) + strlen(text2) + 1) ;
		sprintf (etext, "%s%s", text1, text2) ;
		XtError (etext) ;
		}
	    else
	    if (j != 0 && item[i][j-1].primary == TBL_HSPAN &&
			 item[i-1][j].primary == TBL_HSPAN)
		{
		/* Must mean this... */
		item[i][j].primary = TBL_HSPAN ;
		j-- ;  /* recheck this position */
		}
	    else
	    if (j == 0)
		{
		tw->tbl.r_item[i]++ ;
		}
	    else
	    if (item[i][j-1].primary != TBL_VSPAN)
		{
		tw->tbl.r_item[i]++ ;
		}
	    else
		{
		int k ;
		/* Need to see in this case (both this and left neighbor
		 * are vspans) whether they are different items 
		 * Unfortunately, .pw has not yet been computed :-(
	         * I'll have to do it the hard way
		 * I'll have to look up from here and if the first non-vspan
		 * is an item, then different.  if hspan, then same. 
		 */
		for (k=i-1;  k >= 0;  k--)
		    if (item[k][j].primary != TBL_VSPAN)
			break ;
		if (item[k][j].primary == TBL_ITEM)
		    {
		    tw->tbl.r_item[i]++ ;
		    }
		}
	    }
	}
    }


if (n_item > tw->composite.num_children)
    {
    total_mismatch (n_item, tw) ;
    }

if (n_item < tw->composite.num_children)
    {
    /*
     * Check to see if it can be handled by repetitions of last line
     */
    int diff = tw->composite.num_children - n_item ;
    int last_row_items = tw->tbl.r_item[tw->tbl.rows-1] ;

    if ((diff % last_row_items) != 0)
	{
	total_mismatch (n_item, tw) ;
	}

    for (i=0;  i < diff / last_row_items;  i++)
	{
	int *save_r_item ;
	int *save_r_height ;

	tw->tbl.item[tw->tbl.rows] =
	    (ElementRecord *) XtMalloc (tw->tbl.cols[tw->tbl.rows-1] *
					sizeof (ElementRecord)) ;
	tw->tbl.cols[tw->tbl.rows] = tw->tbl.cols[tw->tbl.rows-1] ;

	/*
	 * need to reallocate the r_item array
	 */
	save_r_item = tw->tbl.r_item ;
	save_r_height = tw->tbl.r_height ;
	tw->tbl.r_item = (int *) XtMalloc ((tw->tbl.rows+1) * sizeof (int)) ;
	tw->tbl.r_height = (int *) XtMalloc ((tw->tbl.rows+1) * sizeof (int)) ;
	for (j=0;  j < tw->tbl.rows;  j++)
	    {
	    tw->tbl.r_item[j] = save_r_item[j] ;
	    tw->tbl.r_height[j] = save_r_height[j] ;
	    }
	XtFree ((char *) save_r_item) ;
	XtFree ((char *) save_r_height) ;
	tw->tbl.r_item[tw->tbl.rows] = tw->tbl.r_item[tw->tbl.rows-1] ;
	tw->tbl.r_height[tw->tbl.rows] = tw->tbl.r_height[tw->tbl.rows-1] ;

	for (j=0;  j < tw->tbl.cols[tw->tbl.rows];  j++)
	    {
	    tw->tbl.item[tw->tbl.rows][j] = tw->tbl.item[tw->tbl.rows-1][j] ;
	    tw->tbl.item[tw->tbl.rows][j].text = NULL ;
	    }
	tw->tbl.rows++ ;
	}
    }

/*
 * Find the span of each item
 */

nth_child = 0 ;

for (i=0;  i < tw->tbl.rows;  i++)
    {
    for (j=0;  j < cols[i];  j++)
	{
	XcuTblConstraints constraint ;
	if (item[i][j].primary == TBL_ITEM)
	    {
	    int ii ;
	    int jj ;
	    Arg args[10] ;
	    Cardinal n_args = 0 ;

	    child = children[nth_child++] ;
	    constraint = (XcuTblConstraints)child->core.constraints;

	    /*
	     * Inform the child widget of tbl specifiable resources
	     */

	    XtSetArg (args[n_args], XtNjustify, (XtArgVal)item[i][j].justify) ;
	    n_args++ ;

	    if (tw->tbl.child_border_width != -1)
		{
		XtSetArg (args[n_args], XtNborderWidth,
			  (XtArgVal)tw->tbl.child_border_width) ;
		n_args++ ;
		}

	    if (tw->tbl.child_border_color != -1)
		{
		XtSetArg (args[n_args], XtNborderColor,
			  (XtArgVal)tw->tbl.child_border_color) ;
		n_args++ ;
		}

	    if (tw->tbl.child_background_color != -1)
		{
		XtSetArg (args[n_args], XtNbackground,
			  (XtArgVal)tw->tbl.child_background_color) ;
		n_args++ ;
		}

	    if (tw->tbl.child_foreground_color != -1)
		{
		XtSetArg (args[n_args], XtNforeground,
			  (XtArgVal)tw->tbl.child_foreground_color) ;
		n_args++ ;
		}

	    if (item[i][j].g != XcuNO_GRAVITY)
		{
		XtSetArg (args[n_args], XtNgravity,
			  (XtArgVal)item[i][j].g) ;
		n_args++ ;
		}

	    if (item[i][j].text)
		{
		XtSetArg (args[n_args], XtNlabel, (XtArgVal)item[i][j].text) ;
		n_args++ ;
		}

	    XtSetValues (child, args, n_args) ;

	    constraint->tbl.fill_column = item[i][j].f ;
	    constraint->tbl.span_width = 1 ;
	    for (jj=j+1;  jj < cols[i];  jj++)
		{
		if (item[i][jj].primary != TBL_HSPAN)
		    break ;
		constraint->tbl.span_width++ ;
		}

	    constraint->tbl.span_height = 1 ;
	    for (ii=i+1;  ii < tw->tbl.rows;  ii++)
		{
		if (item[ii][j].primary != TBL_VSPAN)
		    break ;
		constraint->tbl.span_height++ ;
		}

	    /* now fill the pointer array with pointers back to this item */
	    for (ii=0;  ii < constraint->tbl.span_height;  ii++)
		{
		for (jj=0;  jj < constraint->tbl.span_width;  jj++)
		    {
		    tw->tbl.item[i+ii][j+jj].pw = child ;
		    tw->tbl.item[i+ii][j+jj].pi = i ;
		    tw->tbl.item[i+ii][j+jj].pj = j ;
		    }
		}
	    }
	}
    }

tw->tbl.typical_border = item[0][0].pw->core.border_width ;

set_equal_cols (tw) ;

return ;
}

/***** **** *** ** * set_equal_cols * ** *** **** *****/

static void
set_equal_cols (tw)
XcuTblWidget tw ;
{
Cardinal  i ;

if (XtIsRealized ((Widget)tw))
	{
	XtFree ((char *) tw->tbl.equal_cols) ;
	XtFree ((char *) tw->tbl.vspan_status) ;
	}
tw->tbl.equal_cols = (Boolean *) XtMalloc (tw->tbl.rows * sizeof(Boolean)) ;
tw->tbl.vspan_status = (Boolean *) XtMalloc (tw->tbl.rows * sizeof(Boolean)) ;

/*
 * If keyword "XtNequalColumns" is True, all elements set to True
 */

if (tw->tbl.equal_columns == True)
    {
    for (i=0;  i < tw->tbl.rows;  i++)
	tw->tbl.equal_cols[i] = True ;
    }
else
    {
    for (i=0;  i < tw->tbl.rows;  i++)
	{
	Cardinal j ;
	Cardinal e_count = 0 ;
	tw->tbl.equal_cols[i] = False ;
	for (j=0;  j < tw->tbl.cols[i];  j++)
	    {
	    if (tw->tbl.item[i][j].e)
		{
		e_count++ ;
		}
	    if (tw->tbl.item[i][j].primary == TBL_VSPAN)
		{
		/*
		 * If the previous row had equal_columns and this one
		 * is spanned to it, then this must be set, too.
		 */
		if (tw->tbl.equal_cols[i-1] == True)
		    {
		    tw->tbl.equal_cols[i] = True ;
		    tw->tbl.equal_cols[tw->tbl.item[i][j].pi] = True ;
		    }
		break ;
		}
	    else
	    if (tw->tbl.item[i][j].primary == TBL_HSPAN)
		{
		/* Any hz span in a row implies equal columns */
		/* (Unless alignedColumns is True) */
		if (!tw->tbl.aligned_columns)
		    {
		    tw->tbl.equal_cols[i] = True ;
		    }
		}
	    }
	/* if all columns were marked as equal, we'll treat it this way */
	if (e_count == tw->tbl.cols[i])
	    tw->tbl.equal_cols[i] = True ;
	}
    }

/*
 * mark all rows that are spanned
 */

for (i=0;  i < tw->tbl.rows;  i++)
    {
    Cardinal j ;

    tw->tbl.vspan_status[i] = False ;
    for (j=0;  j < tw->tbl.cols[i];  j++)
	{
	if (tw->tbl.item[i][j].primary == TBL_ITEM)
	    {
	    XcuTblConstraints constraint =
		    (XcuTblConstraints) tw->tbl.item[i][j].pw->core.constraints ;
	    if (constraint->tbl.span_height > 1)
		{
		tw->tbl.vspan_status[i] = True ;
		}
	    }
	else
	if (tw->tbl.item[i][j].primary == TBL_VSPAN)
	    {
	    tw->tbl.vspan_status[i] = True ;
	    }
	}
    /*
     * It is an error to have differing number of columns in spanned rows
     * or in layouts with XtNalignedColumns set to True.
     * (The true tbl doesn't complain, it just produces garbage)
     */

    if (i != 0 && tw->tbl.vspan_status[i] && tw->tbl.vspan_status[i-1] &&
	tw->tbl.cols[i] != tw->tbl.cols[i-1])
	{
	char *my_text1 = "XcuTbl: Variable number of columns (%d,%d) in rows\n";
	char *my_text2 = "     that have vertical spanning elements\n" ;
	char *my_text3 = "     (detected in rows %d and %d)\n" ;
	char *etext1 = XtMalloc (strlen(my_text1) + strlen(my_text2) +
				strlen(my_text3) + 1) ;
	char *etext2 ;
	sprintf (etext1, "%s%s%s", my_text1, my_text2, my_text3) ;
	etext2 = XtMalloc (strlen(etext1) + 5) ;
	sprintf (etext2, etext1, tw->tbl.cols[i-1], tw->tbl.cols[i], i-1, i) ;
	XtError (etext2) ;
	}

    if (i != 0 && tw->tbl.aligned_columns &&
	tw->tbl.cols[i] != tw->tbl.cols[i-1])
	{
	char *my_text1 = "XcuTbl: Variable number of columns in layout\n" ;
	char *my_text2 = "     that has XtNalignedColumns set to True\n" ;
	char *etext1 = XtMalloc (strlen(my_text1) + strlen(my_text2) + 1) ;
	sprintf (etext1, "%s%s", my_text1, my_text2) ;
	XtError (etext1) ;
	}
    }

return ;
}

/***** **** *** ** * total_mismatch * ** *** **** *****/

static void
total_mismatch (n_item, tw)
    int n_item ;
    XcuTblWidget tw ;
{
char *my_text1 = "XcuTbl (%s) : %d items in the format\n" ;
char *my_text2 = "     %d children in the widget\n" ;
char *etext1 = XtMalloc (strlen(my_text1) + 30) ;
char *etext2 = XtMalloc (strlen(my_text2) + 3) ;
char *etext3 ;
sprintf (etext1, my_text1, tw->core.name, n_item) ;
sprintf (etext2, my_text2, tw->composite.num_children) ;
etext3 = XtMalloc (strlen(etext1) + strlen(etext2) + 1) ;
sprintf (etext3, "%s%s", etext1, etext2) ;
XtError (etext3) ;
}

/*
 * And, finally, the routines used to parse a tbl description
 */

#define ytblparse() ParseTblLayout (tw) XcuTblWidget tw ;

#include "Tbl.y.c"

/***** **** *** ** * "yyerror" * ** *** **** *****/

static void
ytblerror (s)
    char *s ;
{
Cardinal length ;
String my_text = "XcuTbl: Parsing error (%s)\nLine # %d in file '%s':\n" ;
String etext ;
length = strlen(my_text) + strlen(s) + TBL_MAX_CONTEXT + 20 ;
etext = XtMalloc (length) ;
sprintf (etext, my_text, s, layout_input->line_number, layout_input->cur_file) ;
strcat (etext, layout_input->context_buffer) ;
XtError (etext) ;
}

/***** **** *** ** * "yywrap" * ** *** **** *****/

static int
ytblwrap ()
{
return 1 ;
}

/***** **** *** ** * the lex input routine * ** *** **** *****/

#define TBL_UNPUT_SOURCE 0
#define TBL_INPUT_SOURCE 1

static int
input ()
{
int t ;
int t_source ;

if (layout_input->unput_pointer)
    {
    t = layout_input->unput_buffer[layout_input->unput_pointer-1] ;
    layout_input->unput_pointer-- ;
    t_source = TBL_UNPUT_SOURCE ;
    }
else
    {
    t = layout_input->input_buffer[layout_input->input_pointer++] ;
    t_source = TBL_INPUT_SOURCE ;
    }

if (layout_input->string_file == (int) TBL_STRING_FORMAT &&
    layout_input->lnl_equivalent == -1 &&
    t == '\\')
    {
    int next = input () ;
    if (next == 'n')
	t = '\n' ;
    else
	unput (next) ;
    }

if (t == '\0' && t_source == TBL_INPUT_SOURCE)
    {
    unput (t) ;
    t = '\n' ;
    }

if (t == layout_input->lnl_equivalent)
    {
    t = '\n' ;
    }

if (t == '\n')
    {
    layout_input->line_number++ ;
    layout_input->context_ptr = 0 ;
    }
else
if (layout_input->context_ptr < TBL_MAX_CONTEXT)
    {
    layout_input->context_buffer[layout_input->context_ptr++] = t ;
    layout_input->context_buffer[layout_input->context_ptr] = '\0' ;
    }
else
    {
    char error_text[100] ;
    sprintf (error_text,
	    "Insufficient context buffer space for input character (%c)\n", t) ;
    XtWarning (error_text) ;
    }

return t ;
}

/***** **** *** ** * the lex unput routine * ** *** **** *****/

static void
unput (c)
    int c ;
{
layout_input->unput_buffer[layout_input->unput_pointer++] = c ;

/*
 * Just wipe out currrent line context characters.  Don't try to retrieve those
 * already cycled out. (It seems highly unlikely that anything significant
 * will ever be lost by this.)
 */

if (layout_input->context_ptr > 0)
    layout_input->context_ptr-- ;

if (c == '\n')
    if (layout_input->line_number > 1)
	layout_input->line_number-- ;

return ;
}

/**
 ***
 **** Public Procedures
 ***
 **/

/*
 * These routines are called instead of SetValues when it is necessary
 * to change the format in preparation for adding new children.
 * (SetValues would go ahead and reformat which could cause errors
 *  because of a mismatch between the new format and the old set
 *  of children.)
 */

void
XcuTblFormatString (tw, new_format)
    XcuTblWidget tw ;
    String new_format ;
{
Xcu_copy_ds (&tw->tbl.format_string, new_format) ;
return ;
}

void
XcuTblFormatFile (tw, new_format)
    XcuTblWidget tw ;
    String new_format ;
{
Xcu_copy_ds (&tw->tbl.format_file, new_format) ;
return ;
}

