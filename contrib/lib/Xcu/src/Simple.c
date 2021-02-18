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

/* XcuSimple */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xcu/SimpleP.h>
/* Facet */
#include <X11/Xcu/SimpleI.h>

/* XcuSimple */
#define UnspecifiedPixmap 2 /* ??? NULL */
#define IsSensitive(w) ((w)->core.sensitive && (w)->core.ancestor_sensitive)

/* XcuSimple */
static Cursor defaultCursor = None;
static Pixmap defaultPixmap = NULL;
/* Facet */
static Dimension   defFacetWidth    = 5 ;
static Dimension   defHighlightWidth  = 2 ;
static Boolean   def_false   	    = False ;
static Pixel   def_bp   	    = None ;
static Pixel   def_wp   	    = None ;
static XcuSunType def_sun_direction = XcuSUN_NW ;

static XtResource resources[] = {
#define offset(field) XtOffset(XcuSimpleWidget, simple.field)
#include <X11/Xcu/SimpleR.h>
#undef offset
};

#ifdef CompositeSimple
static XtResource compResources[] = {
#define offset(field) XtOffset(XcuSimpleCompWidget, simple.field)
#include <X11/Xcu/SimpleR.h>
#undef offset
};                

static XtResource compConstResources[] = {
#define offset(field) XtOffset(XcuSimpleCompConstWidget, simple.field)
#include <X11/Xcu/SimpleR.h>
#undef offset
};                

#endif
/*  Forward Definition of private routines  */

static void ChangeSensitive();
static void ClassInitialize() ;
static void ClassPartInitialize();
static void Realize();
static Boolean SetValues();
static void Initialize();
static void Redisplay();
static void Resize();
static void Destroy();
/* (private) */
static void GetCursorColors () ;
static void Draw_Facets();
static void Draw_Highlight();
static void Erase_Highlight();
static Boolean SetValuesCorePart();
static void SetValuesFacetPart();
static void DestroyFacet();
static void GetDimFacetGC();
static void GetBrightFacetGC();
static void GetHighlightGC();
static void GetUnhighlightGC();
static void GetBackgroundGC();
static void GetCoreBkgGC();
#ifdef CompositeSimple
static XtGeometryResult GeometryManager() ;
static void CompInitialize();
static void CompClassPartInitialize();
static void CompRedisplay();
static Boolean CompSetValues();
static void CompDestroy();
static void CompRealize();
static void CompConstInitialize();
static void CompConstClassPartInitialize();
static void CompConstRedisplay();
static Boolean CompConstSetValues();
static void CompConstDestroy();
static void CompConstRealize();
#endif

/*  Static Class Definition  */

XcuSimpleClassRec xcuSimpleClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"Simple",
    /* widget_size		*/	sizeof(XcuSimpleRec),
    /* class_initialize   	*/	ClassInitialize,
    /* class_part_initialize	*/	ClassPartInitialize,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	NULL,
    /* query_geometry		*/	NULL,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL,
  },
  { /* simple fields */
  /* XcuSimple */
    /* change_sensitive		*/	ChangeSensitive,
  /* Facet */
    /* draw facet		*/	Draw_Facets,
    /* draw highlight		*/      Draw_Highlight
  }
};

WidgetClass xcuSimpleWidgetClass = (WidgetClass)&xcuSimpleClassRec;

#ifdef CompositeSimple
XcuSimpleCompClassRec xcuSimpleCompClassRec = {
  { /* core_class fields */	
    /* superclass	  	*/	(WidgetClass) &compositeClassRec,
    /* class_name	  	*/	"Simple",
    /* widget_size	  	*/	sizeof(XcuSimpleRec),
    /* class_initialize   	*/	ClassInitialize,
    /* class_part_initialize	*/	CompClassPartInitialize,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	CompInitialize,
    /* initialize_hook		*/	NULL,
    /* realize		  	*/	CompRealize,
    /* actions		  	*/	NULL,
    /* num_actions	  	*/	0,
    /* resources	  	*/	compResources,
    /* num_resources	  	*/  	XtNumber(compResources),
    /* xrm_class	  	*/	NULLQUARK,
    /* compress_motion	  	*/	TRUE,
    /* compress_exposure  	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest	  	*/	FALSE,
    /* destroy		  	*/	CompDestroy,
    /* resize		  	*/	Resize,
    /* expose		  	*/	CompRedisplay,
    /* set_values	  	*/	CompSetValues,
    /* set_values_hook	      	*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,  
    /* accept_focus	 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* tm_table		   	*/	NULL,
    /* query_geometry		*/	NULL,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL,
  },
  { /* composite_class fields */
    /* geometry_manager		*/	GeometryManager,
    /* change_managed		*/	NULL,
    /* insert_child		*/	XtInheritInsertChild,
    /* delete_child		*/	XtInheritDeleteChild,
    /* extension		*/	NULL,
  },
  {
  /* XcuSimple */
    /* change_sensitive		*/	ChangeSensitive,
  /* Facet */
    /* draw facet		*/	Draw_Facets,
    /* draw highlight		*/      Draw_Highlight
  }
};                                                               

WidgetClass xcuSimpleCompWidgetClass = (WidgetClass)&xcuSimpleCompClassRec;

XcuSimpleCompConstClassRec xcuSimpleCompConstClassRec = {
  { /* core_class fields */	
    /* superclass	  	*/	(WidgetClass) &constraintClassRec,
    /* class_name	  	*/	"Simple",
    /* widget_size	  	*/	sizeof(XcuSimpleRec),
    /* class_initialize   	*/	ClassInitialize,
    /* class_part_initialize	*/	CompConstClassPartInitialize,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	CompConstInitialize,
    /* initialize_hook		*/	NULL,
    /* realize		  	*/	CompConstRealize,
    /* actions		  	*/	NULL,
    /* num_actions	  	*/	0,
    /* resources	  	*/	compConstResources,
    /* num_resources	  	*/  	XtNumber(compConstResources),
    /* xrm_class	  	*/	NULLQUARK,
    /* compress_motion	  	*/	TRUE,
    /* compress_exposure  	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest	  	*/	FALSE,
    /* destroy		  	*/	CompConstDestroy,
    /* resize		  	*/	Resize,
    /* expose		  	*/	CompConstRedisplay,
    /* set_values	  	*/	CompConstSetValues,
    /* set_values_hook	      	*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,  
    /* accept_focus	 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* tm_table		   	*/	NULL,
    /* query_geometry		*/	NULL,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL,
  },
  { /* composite_class fields */
    /* geometry_manager		*/	GeometryManager,
    /* change_managed		*/	NULL,
    /* insert_child		*/	XtInheritInsertChild,
    /* delete_child		*/	XtInheritDeleteChild,
    /* extension		*/	NULL,
  },
  { /* constraint_class fields */
    /* subresourses       */   NULL,
    /* subresource_count  */   0,
    /* initialize         */   NULL,
    /* destroy            */   NULL,
    /* set_values         */   NULL,
    /* extension         */    NULL
  },
  {
  /* XcuSimple */
    /* change_sensitive		*/	ChangeSensitive,
  /* Facet */
    /* draw facet		*/	Draw_Facets,
    /* draw highlight		*/      Draw_Highlight
  }
};                                                               

WidgetClass xcuSimpleCompConstWidgetClass =
	    (WidgetClass)&xcuSimpleCompConstClassRec;

#endif
/**
 *** Toolkit Callable Routines
 **/

/***** **** *** ** * ClassInitialize * ** *** **** *****/

static void
ClassInitialize()
{
XcuAddStringToSunConverter () ;
return ;
}

/***** **** *** ** * ClassPartInitialize * ** *** **** *****/

static void ClassPartInitialize(class)
    WidgetClass class;
{
    register XcuSimpleWidgetClass c = (XcuSimpleWidgetClass)class;

    /* XcuSimple */
    if (c->simple_class.change_sensitive == XtInheritChangeSensitive)
	c->simple_class.change_sensitive = ChangeSensitive;
    /* Facet */
    if ( c->simple_class.draw_facets == XtInheritDrawFacets )
         c->simple_class.draw_facets = Draw_Facets ;

    if ( c->simple_class.draw_highlight == XtInheritDrawHighlight )
         c->simple_class.draw_highlight = Draw_Highlight ;
}

static void Initialize(request, new)
 Widget request, new;
{
    XcuSimpleWidget fw = (XcuSimpleWidget) new;

/*  screen depth hack */
    /***
    fw->core.depth = 24 ;
    ***/

    GetDimFacetGC( new, &fw->simple );
    GetBrightFacetGC( new, &fw->simple );
    GetHighlightGC( new, &fw->simple );
    GetUnhighlightGC( new, &fw->simple );
    GetCursorColors (fw) ;
    fw->simple.highlighted = FALSE;  
    fw->simple.first_expose = True ;
    if (fw->simple.shadow)
	{
	/* For shadows to look right, parts of it have to look like parent */
	fw->simple.background_pixel = fw->core.background_pixel ;
#ifdef CompositeSimple
	if (XtIsSubclass (fw->core.parent, xcuSimpleCompWidgetClass))
	    {
	    XcuSimpleCompWidget parent = (XcuSimpleCompWidget)fw->core.parent ;
	    if (parent->simple.shadow)
		fw->core.background_pixel = parent->simple.background_pixel;
	    else
		fw->core.background_pixel = parent->core.background_pixel ;
	    }
	else
	if (XtIsSubclass (fw->core.parent, xcuSimpleCompConstWidgetClass))
	    {
	    XcuSimpleCompConstWidget parent =
		(XcuSimpleCompConstWidget)fw->core.parent ;
	    if (parent->simple.shadow)
		fw->core.background_pixel = parent->simple.background_pixel;
	    else
		fw->core.background_pixel = parent->core.background_pixel ;
	    }
	else
#endif
	fw->core.background_pixel = fw->core.parent->core.background_pixel ;
	fw->simple.save_border_width = fw->core.border_width ;
	fw->core.border_width = 0 ;
	GetBackgroundGC (fw, &fw->simple) ;
	GetCoreBkgGC (fw, &fw->simple) ;
	fw->simple.total_facet_width = fw->simple.facet_width ;
	if (fw->simple.sun_direction == XcuSUN_NW ||
	    fw->simple.sun_direction == XcuSUN_SW)
	    fw->simple.left_facet_width = 0 ;
	else
	    fw->simple.left_facet_width = fw->simple.facet_width ;

	if (fw->simple.sun_direction == XcuSUN_NW ||
	    fw->simple.sun_direction == XcuSUN_NE)
	    fw->simple.top_facet_width = 0 ;
	else
	    fw->simple.top_facet_width = fw->simple.facet_width ;
	fw->simple.right_facet_width =
	    fw->simple.facet_width - fw->simple.left_facet_width ;
	fw->simple.bot_facet_width =
	    fw->simple.facet_width - fw->simple.top_facet_width ;
	}
    else
	{
	fw->simple.total_facet_width = 2 * fw->simple.facet_width ;
	fw->simple.left_facet_width = fw->simple.facet_width ;
	fw->simple.right_facet_width = fw->simple.facet_width ;
	fw->simple.top_facet_width = fw->simple.facet_width ;
	fw->simple.bot_facet_width = fw->simple.facet_width ;
	}
return ;
}

static void Realize(w, valueMask, attributes)
    register Widget w;
    Mask *valueMask;
    XSetWindowAttributes *attributes;
{
    Pixmap border_pixmap;
    XcuSimpleWidget sw = (XcuSimpleWidget) w ;

    if (!IsSensitive(w)) {
	/* change border to gray; have to remember the old one,
	 * so XtDestroyWidget deletes the proper one */
	if (!((XcuSimpleWidget)w)->simple.insensitive_border)
	    ((XcuSimpleWidget)w)->simple.insensitive_border =
		XmuCreateStippledPixmap (XtScreen(w),
					 w->core.border_pixel,
					 w->core.background_pixel,
					 w->core.depth) ;
        border_pixmap = w->core.border_pixmap;
	attributes->border_pixmap =
	  w->core.border_pixmap = ((XcuSimpleWidget)w)->simple.insensitive_border;

	*valueMask |= CWBorderPixmap;
	*valueMask &= ~CWBorderPixel;
    }

    if ((attributes->cursor = ((XcuSimpleWidget)w)->simple.cursor) != None)
	*valueMask |= CWCursor;

    XtCreateWindow( w, (unsigned int)InputOutput, (Visual *)CopyFromParent,
		    *valueMask, attributes );

    if (attributes->cursor != None)
	{
	XRecolorCursor (XtDisplay(w), attributes->cursor,
			&sw->simple.cursor_fg, &sw->simple.cursor_bg) ;
	}
    if (!IsSensitive(w))
	{
	/* Compiler may give a warning here, but it's really okay! */
	w->core.border_pixmap = border_pixmap;
	}
}


static Boolean SetValues(current, request, new)
    Widget current, request, new;
{
    XcuSimpleWidget curfw = (XcuSimpleWidget) current;
    XcuSimpleWidget reqfw = (XcuSimpleWidget) request;
    XcuSimpleWidget newfw = (XcuSimpleWidget) new;
    Boolean was_resized ;
    Boolean needs_redisplay = False ;

    if (curfw->simple.cursor_foreground != newfw->simple.cursor_foreground ||
        curfw->simple.cursor_background != newfw->simple.cursor_background)
	{
	GetCursorColors (newfw) ;
	}

    if ((current->core.sensitive != new->core.sensitive ||
	 current->core.ancestor_sensitive != new->core.ancestor_sensitive))
	ChangeSensitive( new );

    was_resized = SetValuesCorePart( current, request, new ) ;

    SetValuesFacetPart( &curfw->simple, &reqfw->simple, &newfw->simple, new ) ;

    if (curfw->simple.shadow != newfw->simple.shadow)
	{
	if (newfw->simple.shadow)
	    {
	    /* For shadows to look right, parts have to look like parent */
	    newfw->simple.background_pixel = newfw->core.background_pixel ;
	    newfw->core.background_pixel =
		newfw->core.parent->core.background_pixel;
	    newfw->simple.save_border_width = newfw->core.border_width ;
	    newfw->core.border_width = 0 ;
	    GetBackgroundGC (newfw, &newfw->simple) ;
	    GetCoreBkgGC (newfw, &newfw->simple) ;
	    }
	else
	    {
	    /* revert to normal pixmaps */
	    newfw->core.background_pixel = newfw->simple.background_pixel;
	    newfw->core.border_width = newfw->simple.save_border_width ;
	    XtReleaseGC( newfw, newfw->simple.background_GC );
	    }
	}
    else
    if (curfw->core.background_pixel != newfw->core.background_pixel)
	{
	if (newfw->simple.shadow)
	    {
	    /* For shadows to look right, parts have to look like parent */
	    newfw->simple.background_pixel = newfw->core.background_pixel ;
	    newfw->core.background_pixel =
		newfw->core.parent->core.background_pixel;
	    XtReleaseGC( newfw, newfw->simple.background_GC );
	    GetBackgroundGC (newfw, &newfw->simple) ;
	    GetCoreBkgGC (newfw, &newfw->simple) ;
	    needs_redisplay = True ;
	    if (XtIsRealized(newfw))
		{
		XSetWindowBackgroundPixmap (XtDisplay(newfw), XtWindow(newfw),
				      None) ;
		/**
		XSetWindowBackground (XtDisplay(newfw), XtWindow(newfw),
				      newfw->core.background_pixel) ;
		**/
		}
	    }
	}

    if (was_resized) {
	(*XtClass(new)->core_class.resize) ((Widget)newfw);
    }

    return( was_resized | needs_redisplay );
}

static void Destroy( w )
    Widget  w ;
{
    XcuSimpleWidget fw = (XcuSimpleWidget)w ;
    DestroyFacet( fw, &fw->simple ) ;
    if (fw->simple.shadow)
	XtReleaseGC( fw, fw->simple.background_GC );
	
}

static void Resize(w)
    Widget w;
{
XtCallCallbacks (w, XtNresize, NULL) ;
return ;
}

static void Redisplay(w, event, region)
    Widget w;
    XEvent *event;                                 
    Region region;
{
    XcuSimpleWidget fw = (XcuSimpleWidget) w;

    /* Draw these parts of the button: facets/shadows and highlight */

    if  (
	 fw->simple.exposing ||
	 (!fw->simple.shadow && fw->simple.highlight_change &&
	   !fw->simple.highlighted) ||
	 (fw->simple.shadow && fw->simple.highlight_change)
	)
	{
	/*
	 * Draw facets if exposing or highlight is changing to False
	 * Draw shadow if exposing or highlight is changing.
	 * if (not shadow, highlights are changing to off, 
	       but facets smaller than highlights)
	 *     erase highlights first
	 */
	if (
	    !fw->simple.shadow &&
	    fw->simple.highlight_change &&
	    !fw->simple.highlighted &&
	    fw->simple.highlight_width > fw->simple.facet_width
	   )
	    {
	    Erase_Highlight ( w, &fw->simple ) ;
	    }

	(*((XcuSimpleClassRec *)XtClass(fw))->simple_class.draw_facets)
				      (w, &fw->simple);

	}
    if  (
	!fw->simple.shadow &&
	((fw->simple.highlight_change && fw->simple.highlighted == TRUE) ||
	fw->simple.exposing)
	)
	{
	/*
	 * Draw highlight if
	 *  not a shadow AND
	 *  highlight is changing to true OR exposing
	 */
        (*((XcuSimpleClassRec *) XtClass(fw))->
               simple_class.draw_highlight)(w,&fw->simple) ;
	}

    if (fw->simple.first_expose)
	{
	fw->simple.first_expose = False ;
	XtCallCallbacks(w, XtNfirstExpose, region);
	}
}

/**
 *** Private Routines
 **/

static void
GetCursorColors (sw)
    XcuSimpleWidget sw ;
{
XrmValue pixel_value ;
XrmValue color_value ;

if (sw->simple.cursor_foreground == None)
    sw->simple.cursor_foreground = BlackPixelOfScreen(XtScreen((Widget)sw)) ;

pixel_value.size = sizeof (sw->simple.cursor_foreground) ;
pixel_value.addr = (XPointer) &sw->simple.cursor_foreground ;
XtConvert ((Widget) sw, XtRPixel, &pixel_value, XtRColor, &color_value) ;
sw->simple.cursor_fg = *((XColor *)color_value.addr) ;

if (sw->simple.cursor_background == None)
    sw->simple.cursor_background = WhitePixelOfScreen(XtScreen((Widget)sw)) ;

pixel_value.size = sizeof (sw->simple.cursor_background) ;
pixel_value.addr = (XPointer) &sw->simple.cursor_background ;
XtConvert ((Widget) sw, XtRPixel, &pixel_value, XtRColor, &color_value) ;
sw->simple.cursor_bg = *((XColor *)color_value.addr) ;

return ;
}

static Boolean SetValuesCorePart( cur, req, new )
    Widget cur, req, new ;
{
    Boolean was_resized = False;

    if ((cur->core.width  != new->core.width) ||
        (cur->core.height != new->core.height)) {
        
    /* we have to know if the size change is going to take
       before calling Resize() */

        if ((XtMakeResizeRequest(cur, 
                    new->core.width, new->core.height,
		    &new->core.width, &new->core.height)
	                 == XtGeometryNo)) {
	      new->core.width = cur->core.width;
	      new->core.height = cur->core.height;
        }
        else was_resized = True ;
    }

    return( was_resized );
}
 
static void SetValuesFacetPart( cur, req, new, w )
    XcuSimplePart *cur, *req, *new ;
    Widget    w ;
{
    if ( cur->dim_facet_color != new->dim_facet_color ) {
	XtReleaseGC( cur, cur->dim_facet_GC );
	GetDimFacetGC( w, new );
    }

    if ( cur->highlight_color != new->highlight_color ) {
	XtReleaseGC( cur, cur->highlight_GC );
	GetBrightFacetGC( w, new );
    }

    if ( cur->bright_facet_color != new->bright_facet_color ) {
	XtReleaseGC( cur, cur->bright_facet_GC );
	GetBrightFacetGC( w, new );
    }
}

static void DestroyFacet( w, simple )
    XcuSimpleWidget w ;
    XcuSimplePart *simple ;
{ 
    XtReleaseGC(w, simple->dim_facet_GC);
    XtReleaseGC(w, simple->bright_facet_GC);
    XtReleaseGC(w, simple->highlight_GC);
    XtReleaseGC(w, simple->unhighlight_GC);
}

/* routines to get all the graphics contexts 	*/

static void GetCoreBkgGC( fw, simple )
    XcuSimpleWidget    fw ;
    XcuSimplePart *simple ;
{
    XGCValues	values;

    values.foreground = fw->core.background_pixel;
    simple->bkg_GC = XtGetGC(
	(Widget)fw, (unsigned) GCForeground, &values);
}

static void GetBackgroundGC( fw, simple )
    XcuSimpleWidget    fw ;
    XcuSimplePart *simple ;
{
    XGCValues	values;

    values.foreground = simple->background_pixel;
    simple->background_GC = XtGetGC(
	(Widget)fw, (unsigned) GCForeground, &values);
}

static void GetDimFacetGC( w, simple )
    Widget    w ;
    XcuSimplePart *simple ;
{
    XGCValues	values;

    values.foreground = simple->dim_facet_color;
    simple->dim_facet_GC = XtGetGC(
	w, (unsigned) GCForeground, &values);
}

static void GetBrightFacetGC( w, simple )
    Widget    w ;
    XcuSimplePart *simple ;
{
    XGCValues	values;

    values.foreground = simple->bright_facet_color;
    simple->bright_facet_GC = XtGetGC(
	w, (unsigned) GCForeground, &values);
}                                                 

static void GetHighlightGC( w, simple )
    Widget    w ;
    XcuSimplePart *simple ;
{
    XGCValues  	values;

    values.foreground = simple->highlight_color;
    values.line_width = simple->highlight_width;
    simple->highlight_GC = XtGetGC(
	w, (unsigned) (GCForeground | GCLineWidth), &values);
}

static void GetUnhighlightGC( w, simple )
    Widget    w ;
    XcuSimplePart *simple ;
{
    XcuSimpleWidget fw = (XcuSimpleWidget) w ;
    XGCValues  	values;

    values.foreground = fw->core.background_pixel;
    values.line_width = simple->highlight_width;
    simple->unhighlight_GC = XtGetGC(
	w, (unsigned) (GCForeground | GCLineWidth), &values);
}

/*
 * Repaint the widget window
 */

static void Draw_Facets( w, simple )
    Widget w ;
    XcuSimplePart *simple ;
{
   XPoint	points[4] ;

    /* note that for FillPolygon a pixel on a horizontal boundary
       is only filled if the rest of the polygon is below it.
       This means we have to fudge some of the boundaries by one */

if (!FacetWidth)
    return ;

if (simple->shadow)
    {
    GC shadowGC = simple->highlighted ? HighlightGC : DimGC ;

    if (simple->sun_direction == XcuSUN_NW)
	{
	XFillRectangle(  XtDisplay(w), XtWindow(w), shadowGC,
			FacetWidth, Height - FacetWidth,
			Width - FacetWidth, FacetWidth ) ;
	XFillRectangle(  XtDisplay(w), XtWindow(w), shadowGC,
			Width - FacetWidth, FacetWidth,
			FacetWidth, Height - 2*FacetWidth ) ;
	XFillRectangle(  XtDisplay(w), XtWindow(w), simple->bkg_GC,
			0, Height - FacetWidth,
			FacetWidth, FacetWidth ) ;
	XFillRectangle(  XtDisplay(w), XtWindow(w), simple->bkg_GC,
			Width - FacetWidth, 0,
			FacetWidth, FacetWidth ) ;
	}
    else
    if (simple->sun_direction == XcuSUN_SE)
	{
	XFillRectangle(  XtDisplay(w), XtWindow(w), shadowGC,
			0, 0, Width - FacetWidth, FacetWidth ) ;
	XFillRectangle(  XtDisplay(w), XtWindow(w), shadowGC,
			0, FacetWidth, FacetWidth, Height - 2*FacetWidth ) ;
	XFillRectangle(  XtDisplay(w), XtWindow(w), simple->bkg_GC,
			Width - FacetWidth, 0, FacetWidth, FacetWidth ) ;
	XFillRectangle(  XtDisplay(w), XtWindow(w), simple->bkg_GC,
			0, Height - FacetWidth, FacetWidth, FacetWidth ) ;
	}
    }
else
    {
/* top */

    points[0].x = 0 ;      points[0].y = 0 ;
    points[1].x = Width ;  points[1].y = points[0].y ;
    points[2].x = points[1].x - FacetWidth + 1 ;
    points[2].y = points[1].y + FacetWidth     ;
    points[3].x = points[0].x + FacetWidth     ; 
    points[3].y = points[2].y ;

    XFillPolygon( XtDisplay(w), XtWindow(w), BrightGC, points, 4,
                  Convex, CoordModeOrigin );

/* left side */

    points[0].x = points[0].x ;  points[0].y = points[0].y ;
    points[1].x = points[3].x ;  points[1].y = points[3].y ;
    points[2].x = points[1].x ;
    points[2].y = Height - FacetWidth ;
    points[3].x = points[0].x ;  
    points[3].y = points[2].y + FacetWidth ;

    XFillPolygon( XtDisplay(w), XtWindow(w), BrightGC, points, 4,
                  Convex, CoordModeOrigin );
                                                   
/* bottom */

    points[0].x = points[3].x ;  points[0].y = points[3].y ;
    points[1].x = points[2].x ;  points[1].y = points[2].y ;
    points[2].x = Width - FacetWidth /* + 1 */ ; 
    points[2].y = points[1].y ;
    points[3].x = Width ; 
    points[3].y = points[0].y ;

    XFillPolygon( XtDisplay(w), XtWindow(w), DimGC, points, 4,
                  Convex, CoordModeOrigin );

/* right side */

    points[0].x = points[3].x ;  points[0].y = points[3].y ;
    points[1].x = points[2].x /* - 1 */ ;  points[1].y = points[2].y ;
    points[2].x = points[1].x ;
    points[2].y = FacetWidth ;
    points[3].x = points[0].x ;
    points[3].y = points[2].y - FacetWidth ;
                                             
    XFillPolygon( XtDisplay(w), XtWindow(w), DimGC, points, 4,
                  Convex, CoordModeOrigin );
    }

}

static void Draw_Highlight( w, simple )
    Widget w ;
    XcuSimplePart *simple ;
{
if (Highlighted && HighlightWidth != 0)
    {
    /* wide lines are centered on the path, so indent it */
    int offset = HighlightWidth/2;
    XDrawRectangle (XtDisplay(w), XtWindow(w),
		    HighlightGC,
		    offset, offset,
		    Width - HighlightWidth,
		    Height - HighlightWidth);
    }
}
                       
static void Erase_Highlight( w, simple )
    Widget w ;
    XcuSimplePart *simple ;
{
if (!Highlighted && HighlightWidth != 0)
    {
    /* wide lines are centered on the path, so indent it */
    int offset = HighlightWidth/2;
    XDrawRectangle (XtDisplay(w), XtWindow(w),
		    UnhighlightGC,
		    offset, offset,
		    Width - HighlightWidth,
		    Height - HighlightWidth);
    }
}
                       
                       
/* XcuSimple Routines */

static void ChangeSensitive(w)
    register Widget w;
{
    if (XtIsRealized(w)) {
	if (IsSensitive(w))
	    if (w->core.border_pixmap != UnspecifiedPixmap)
		XSetWindowBorderPixmap( XtDisplay(w), XtWindow(w),
				        w->core.border_pixmap );
	    else
		XSetWindowBorder( XtDisplay(w), XtWindow(w), 
				  w->core.border_pixel );
	else {
	    if (!((XcuSimpleWidget)w)->simple.insensitive_border)
		((XcuSimpleWidget)w)->simple.insensitive_border =
		    XmuCreateStippledPixmap (XtScreen(w),
					     w->core.border_pixel,
					     w->core.background_pixel,
					     w->core.depth) ;
	    XSetWindowBorderPixmap( XtDisplay(w), XtWindow(w),
				    ((XcuSimpleWidget)w)->
				        simple.insensitive_border );
	}
    }
    return ;
}

#ifdef CompositeSimple

/*
 ** Routines for Composite superclasses.
 */

static XtGeometryResult GeometryManager(w)
Widget w ;
{
    return( XtGeometryYes ) ;
}

static void CompClassPartInitialize( widgetClass )
    WidgetClass  widgetClass ;
{
    /*
    register XcuSimpleCompWidgetClass c = (XcuSimpleCompWidgetClass)widgetClass;
    */
    XcuSimpleCompWidgetClass c = (XcuSimpleCompWidgetClass)widgetClass;
    /* XcuSimple */
    if (c->simple_class.change_sensitive == XtInheritChangeSensitive)
	c->simple_class.change_sensitive = ChangeSensitive;
    /* Facet */
    if ( c->simple_class.draw_facets == XtInheritDrawFacets )
         c->simple_class.draw_facets = Draw_Facets ;

    if ( c->simple_class.draw_highlight == XtInheritDrawHighlight )
         c->simple_class.draw_highlight = Draw_Highlight ;
}

static void CompInitialize(request, new)
 Widget request, new;
{
    XcuSimpleCompWidget fcw = (XcuSimpleCompWidget) new;

/*  screen depth hack */
    /***
    fcw->core.depth = 24 ;
    ***/

    GetDimFacetGC( new, &fcw->simple );
    GetBrightFacetGC( new, &fcw->simple );
    GetHighlightGC( new, &fcw->simple );
    GetUnhighlightGC( new, &fcw->simple );
    /***
    GetCursorColors (fcw) ;
    ***/
    fcw->simple.highlighted = FALSE;  
    fcw->simple.first_expose = True ;

    if (fcw->simple.shadow)
	{
	/* For shadows to look right, parts of it have to look like parent */
	fcw->simple.background_pixel = fcw->core.background_pixel ;
#ifdef CompositeSimple
	if (XtIsSubclass (fcw->core.parent, xcuSimpleCompWidgetClass))
	    {
	    XcuSimpleCompWidget parent = (XcuSimpleCompWidget)fcw->core.parent;
	    if (parent->simple.shadow)
		fcw->core.background_pixel = parent->simple.background_pixel;
	    else
		fcw->core.background_pixel = parent->core.background_pixel ;
	    }
	else
#endif
	fcw->core.background_pixel = fcw->core.parent->core.background_pixel ;
	fcw->simple.save_border_width = fcw->core.border_width ;
	fcw->core.border_width = 0 ;
	GetBackgroundGC (fcw, &fcw->simple) ;
	GetCoreBkgGC (fcw, &fcw->simple) ;
	fcw->simple.total_facet_width = fcw->simple.facet_width ;
	if (fcw->simple.sun_direction == XcuSUN_NW ||
	    fcw->simple.sun_direction == XcuSUN_SW)
	    fcw->simple.left_facet_width = 0 ;
	else
	    fcw->simple.left_facet_width = fcw->simple.facet_width ;

	if (fcw->simple.sun_direction == XcuSUN_NW ||
	    fcw->simple.sun_direction == XcuSUN_NE)
	    fcw->simple.top_facet_width = 0 ;
	else
	    fcw->simple.top_facet_width = fcw->simple.facet_width ;
	fcw->simple.right_facet_width =
	    fcw->simple.facet_width - fcw->simple.left_facet_width ;
	fcw->simple.bot_facet_width =
	    fcw->simple.facet_width - fcw->simple.top_facet_width ;
	}
    else
	{
	fcw->simple.total_facet_width = 2 * fcw->simple.facet_width ;
	fcw->simple.left_facet_width = fcw->simple.facet_width ;
	fcw->simple.right_facet_width = fcw->simple.facet_width ;
	fcw->simple.top_facet_width = fcw->simple.facet_width ;
	fcw->simple.bot_facet_width = fcw->simple.facet_width ;
	}
}

static Boolean CompSetValues(current, request, new)
    Widget current, request, new;
{
    XcuSimpleCompWidget curfcw = (XcuSimpleCompWidget) current;
    XcuSimpleCompWidget reqfcw = (XcuSimpleCompWidget) request;
    XcuSimpleCompWidget newfcw = (XcuSimpleCompWidget) new;
    Boolean was_resized ;

    was_resized = SetValuesCorePart( current, request, new ) ;

    SetValuesFacetPart( &curfcw->simple, &reqfcw->simple, &newfcw->simple, new ) ;

    if (was_resized) {
	(*XtClass(new)->core_class.resize) ((Widget)newfcw);
    }

    return( was_resized );
}

static void CompDestroy( w )
    Widget  w ;
{
    XcuSimpleCompWidget fcw = (XcuSimpleCompWidget)w ;
    DestroyFacet( fcw, &fcw->simple ) ;
}

static void CompRedisplay(w, event, region)
    Widget w;
    XEvent *event;                                 
    Region region;
{
    XcuSimpleCompWidget fcw = (XcuSimpleCompWidget) w;

    /* Now draw the button: facets, button face (with text), and border */

    (*((XcuSimpleCompClassRec *) XtClass(fcw))->
                 simple_class.draw_facets)( w, &fcw->simple ) ;

    /***
    if ( fcw->simple.highlighted == TRUE )
        (*((XcuSimpleCompClassRec *) XtClass(fcw))->
                     simple_class.draw_highlight)( w, &fcw->simple ) ;
		     ***/
}

static void CompRealize(w, valueMask, attributes)
    register Widget w;
    Mask *valueMask;
    XSetWindowAttributes *attributes;
{
    Pixmap border_pixmap;
    XcuSimpleCompWidget sw = (XcuSimpleCompWidget) w ;

    if (!IsSensitive(w)) {
	/* change border to gray; have to remember the old one,
	 * so XtDestroyWidget deletes the proper one */
	if (!((XcuSimpleCompWidget)w)->simple.insensitive_border)
	    ((XcuSimpleCompWidget)w)->simple.insensitive_border =
		XmuCreateStippledPixmap (XtScreen(w),
					 w->core.border_pixel,
					 w->core.background_pixel,
					 w->core.depth) ;
        border_pixmap = w->core.border_pixmap;
	attributes->border_pixmap =
	  w->core.border_pixmap = ((XcuSimpleCompWidget)w)->simple.insensitive_border;

	*valueMask |= CWBorderPixmap;
	*valueMask &= ~CWBorderPixel;
    }

    if ((attributes->cursor = ((XcuSimpleCompWidget)w)->simple.cursor) != None)
	*valueMask |= CWCursor;

    XtCreateWindow( w, (unsigned int)InputOutput, (Visual *)CopyFromParent,
		    *valueMask, attributes );

    if (attributes->cursor != None)
	{
	XRecolorCursor (XtDisplay(w), attributes->cursor,
			&sw->simple.cursor_fg, &sw->simple.cursor_bg) ;
	}
    if (!IsSensitive(w))
	{
	/* Compiler may give a warning here, but it's really okay! */
	w->core.border_pixmap = border_pixmap;
	}
}
#endif

#ifdef CompositeSimple

/*
 ** Routines for Composite superclasses.
 */

static void CompConstClassPartInitialize( widgetClass )
    WidgetClass  widgetClass ;
{
    /*
    register XcuSimpleCompConstWidgetClass c = (XcuSimpleCompConstWidgetClass)widgetClass;
    */
    XcuSimpleCompConstWidgetClass c = (XcuSimpleCompConstWidgetClass)widgetClass;
    /* XcuSimple */
    if (c->simple_class.change_sensitive == XtInheritChangeSensitive)
	c->simple_class.change_sensitive = ChangeSensitive;
    /* Facet */
    if ( c->simple_class.draw_facets == XtInheritDrawFacets )
         c->simple_class.draw_facets = Draw_Facets ;

    if ( c->simple_class.draw_highlight == XtInheritDrawHighlight )
         c->simple_class.draw_highlight = Draw_Highlight ;
}

static void CompConstInitialize(request, new)
 Widget request, new;
{
    XcuSimpleCompConstWidget fcw = (XcuSimpleCompConstWidget) new;

/*  screen depth hack */
    /***
    fcw->core.depth = 24 ;
    ***/

    GetDimFacetGC( new, &fcw->simple );
    GetBrightFacetGC( new, &fcw->simple );
    GetHighlightGC( new, &fcw->simple );
    GetUnhighlightGC( new, &fcw->simple );
    /***
    GetCursorColors (fcw) ;
    ***/
    fcw->simple.highlighted = FALSE;  
    fcw->simple.first_expose = True ;

    if (fcw->simple.shadow)
	{
	/* For shadows to look right, parts of it have to look like parent */
	fcw->simple.background_pixel = fcw->core.background_pixel ;
#ifdef CompositeSimple
	if (XtIsSubclass (fcw->core.parent, xcuSimpleCompConstWidgetClass))
	    {
	    XcuSimpleCompConstWidget parent = (XcuSimpleCompConstWidget)fcw->core.parent;
	    if (parent->simple.shadow)
		fcw->core.background_pixel = parent->simple.background_pixel;
	    else
		fcw->core.background_pixel = parent->core.background_pixel ;
	    }
	else
#endif
	fcw->core.background_pixel = fcw->core.parent->core.background_pixel ;
	fcw->simple.save_border_width = fcw->core.border_width ;
	fcw->core.border_width = 0 ;
	GetBackgroundGC (fcw, &fcw->simple) ;
	GetCoreBkgGC (fcw, &fcw->simple) ;
	fcw->simple.total_facet_width = fcw->simple.facet_width ;
	if (fcw->simple.sun_direction == XcuSUN_NW ||
	    fcw->simple.sun_direction == XcuSUN_SW)
	    fcw->simple.left_facet_width = 0 ;
	else
	    fcw->simple.left_facet_width = fcw->simple.facet_width ;

	if (fcw->simple.sun_direction == XcuSUN_NW ||
	    fcw->simple.sun_direction == XcuSUN_NE)
	    fcw->simple.top_facet_width = 0 ;
	else
	    fcw->simple.top_facet_width = fcw->simple.facet_width ;
	fcw->simple.right_facet_width =
	    fcw->simple.facet_width - fcw->simple.left_facet_width ;
	fcw->simple.bot_facet_width =
	    fcw->simple.facet_width - fcw->simple.top_facet_width ;
	}
    else
	{
	fcw->simple.total_facet_width = 2 * fcw->simple.facet_width ;
	fcw->simple.left_facet_width = fcw->simple.facet_width ;
	fcw->simple.right_facet_width = fcw->simple.facet_width ;
	fcw->simple.top_facet_width = fcw->simple.facet_width ;
	fcw->simple.bot_facet_width = fcw->simple.facet_width ;
	}
}

static Boolean CompConstSetValues(current, request, new)
    Widget current, request, new;
{
    XcuSimpleCompConstWidget curfcw = (XcuSimpleCompConstWidget) current;
    XcuSimpleCompConstWidget reqfcw = (XcuSimpleCompConstWidget) request;
    XcuSimpleCompConstWidget newfcw = (XcuSimpleCompConstWidget) new;
    Boolean was_resized ;

    was_resized = SetValuesCorePart( current, request, new ) ;

    SetValuesFacetPart( &curfcw->simple, &reqfcw->simple, &newfcw->simple, new ) ;

    if (was_resized) {
	(*XtClass(new)->core_class.resize) ((Widget)newfcw);
    }

    return( was_resized );
}

static void CompConstDestroy( w )
    Widget  w ;
{
    XcuSimpleCompConstWidget fcw = (XcuSimpleCompConstWidget)w ;
    DestroyFacet( fcw, &fcw->simple ) ;
}

static void CompConstRedisplay(w, event, region)
    Widget w;
    XEvent *event;                                 
    Region region;
{
    XcuSimpleCompConstWidget fcw = (XcuSimpleCompConstWidget) w;

    /* Now draw the button: facets, button face (with text), and border */

    (*((XcuSimpleCompConstClassRec *) XtClass(fcw))->
                 simple_class.draw_facets)( w, &fcw->simple ) ;

    /***
    if ( fcw->simple.highlighted == TRUE )
        (*((XcuSimpleCompConstClassRec *) XtClass(fcw))->
                     simple_class.draw_highlight)( w, &fcw->simple ) ;
		     ***/
}

static void CompConstRealize(w, valueMask, attributes)
    register Widget w;
    Mask *valueMask;
    XSetWindowAttributes *attributes;
{
    Pixmap border_pixmap;
    XcuSimpleCompConstWidget sw = (XcuSimpleCompConstWidget) w ;

    if (!IsSensitive(w)) {
	/* change border to gray; have to remember the old one,
	 * so XtDestroyWidget deletes the proper one */
	if (!((XcuSimpleCompConstWidget)w)->simple.insensitive_border)
	    ((XcuSimpleCompConstWidget)w)->simple.insensitive_border =
		XmuCreateStippledPixmap (XtScreen(w),
					 w->core.border_pixel,
					 w->core.background_pixel,
					 w->core.depth) ;
        border_pixmap = w->core.border_pixmap;
	attributes->border_pixmap =
	  w->core.border_pixmap = ((XcuSimpleCompConstWidget)w)->simple.insensitive_border;

	*valueMask |= CWBorderPixmap;
	*valueMask &= ~CWBorderPixel;
    }

    if ((attributes->cursor = ((XcuSimpleCompConstWidget)w)->simple.cursor) != None)
	*valueMask |= CWCursor;

    XtCreateWindow( w, (unsigned int)InputOutput, (Visual *)CopyFromParent,
		    *valueMask, attributes );

    if (attributes->cursor != None)
	{
	XRecolorCursor (XtDisplay(w), attributes->cursor,
			&sw->simple.cursor_fg, &sw->simple.cursor_bg) ;
	}
    if (!IsSensitive(w))
	{
	/* Compiler may give a warning here, but it's really okay! */
	w->core.border_pixmap = border_pixmap;
	}
}
#endif

