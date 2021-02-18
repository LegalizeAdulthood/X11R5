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
 * XcuLabel.c - XcuLabel widget
 */

#define XtStrlen(s)		((s) ? strlen(s) : 0)
#define IsSensitive(w) ((w)->core.sensitive && (w)->core.ancestor_sensitive)

#include <ctype.h>
#include <X11/IntrinsicP.h>
#include <X11/Xos.h>
#include <X11/StringDefs.h>
#include <X11/Xcu/LabelP.h>
#include <X11/Xcu/SimpleP.h>
#include <X11/Xcu/JustifyR.h>
#include <X11/Xcu/GravityR.h>
#include <X11/Xcu/FloatR.h>
#include <X11/Xmu/Drawing.h>

void Xcu_copy_ds () ;

/*
 * Full class record constant
 */

/* Private Data */

/*
 * XtResource :
 *	name, class, type, size,
 *	offset, default_type, default_address
 *
 * These are the resources needed in addition to core resources
 */

static XcuGravity	def_gravity = XcuCENTER_GRAVITY ;
static XcuJustify	defJustify = XcuJUSTIFY_CENTER ;
static float		defLFactor = 1.0 ;
static Dimension	defIHeight = 2 ;
static Dimension	defIWidth = 2 ;
static Boolean		def_false = False ;

#define offset(field) XtOffset(XcuLabelWidget, field)

static XtResource resources[] =
    {
     {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(label.foreground), XtRString, "Black"}
    ,{XtNfont,  XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	offset(label.font),XtRString, "Fixed"}
    ,{XtNlabel,  XtCLabel, XtRString, sizeof(String),
	offset(label.label), XtRString, NULL}
    ,{XtNbiggestLabel,  XtCLabel, XtRString, sizeof(String),
	offset(label.big_label), XtRString, NULL}
    ,{XtNjustify, XtCJustify, XtRJustify, sizeof(XcuJustify),
	offset(label.justify), XtRJustify, (XPointer)&defJustify}
    ,{XtNinternalWidth, XtCWidth, XtRDimension,  sizeof(Dimension),
	offset(label.internal_width), XtRDimension, (XPointer)&defIWidth}
    ,{XtNinternalHeight, XtCHeight, XtRDimension, sizeof(Dimension),
	offset(label.internal_height), XtRDimension, (XPointer)&defIHeight}
    ,{XtNlineFactor, XtCLineFactor, XtRFloat, sizeof(float),
	offset(label.line_factor), XtRFloat, (XPointer)&defLFactor}
    ,{XtNgravity, XtCGravity, XtRGravity, sizeof(XcuGravity),
	offset(label.gravity), XtRGravity, (XPointer)&def_gravity}
    ,{XtNpreserveText, XtCPreserveText, XtRBoolean, sizeof(Boolean),
	offset(label.preserve_text), XtRBoolean, (XPointer)&def_false}
    ,{XtNpixmap, XtCPixmap, XtRPixmap, sizeof(Pixmap),
	offset(label.pixmap), XtRPixmap, (XPointer)None}
    ,{XtNbitmap, XtCBitmap, XtRBitmap, sizeof(Pixmap),
	offset(label.bitmap), XtRBitmap, (XPointer)None}
    ,{XtNtile, XtCTile, XtRBoolean, sizeof(Boolean),
	offset(label.tile), XtRBoolean, (XPointer)&def_false}
    } ;

static void ClassInitialize() ;
static void ClassPartInitialize();
static void Initialize() ;
static void Resize() ;
static void Redisplay() ;
static void RedisplayText() ;
static void Destroy() ;
static Boolean SetValues() ;
static XtGeometryResult QueryGeometry() ;
static void find_newlines () ;
static void SetTextWidthAndHeight () ;
static void GetSensitiveTextGC () ;
static void GetInsensitiveTextGC () ;
static void GetBackgroundGC () ;
static void GetPixmapGC () ;
static Dimension PreferredWidth () ;
static Dimension PreferredHeight () ;
static void MakePixmapFromBitmap () ;

#define superclass (&xcuSimpleClassRec)

XcuLabelClassRec xcuLabelClassRec =
{
  {
  /* core_class fields */	
    /* superclass	  	*/	(WidgetClass) &xcuSimpleClassRec,
    /* class_name	  	*/	"Label",
    /* widget_size	  	*/	sizeof(XcuLabelRec),
    /* class_initialize   	*/	ClassInitialize,
    /* class_part_initialize	*/	ClassPartInitialize,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize		  	*/	XtInheritRealize,
    /* actions		  	*/	NULL,
    /* num_actions	  	*/	0,
    /* resources	  	*/	resources,
    /* num_resources	  	*/	XtNumber(resources),
    /* xrm_class	  	*/	NULLQUARK,
    /* compress_motion	  	*/	TRUE,
    /* compress_exposure  	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest	  	*/	FALSE,
    /* destroy		  	*/	Destroy,
    /* resize		  	*/	Resize,
    /* expose		  	*/	Redisplay,
    /* set_values	  	*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus	 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* tm_table		   	*/	NULL,
    /* query_geometry		*/	QueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL,
  },
  {
    XtInheritChangeSensitive, /* Simple Class fields */
    XtInheritDrawFacets,
    XtInheritDrawHighlight
  },
  {
    RedisplayText,                        /* XcuLabel Class fields  */
  },
} ;

/* For use by clients... */
WidgetClass xcuLabelWidgetClass = (WidgetClass) &xcuLabelClassRec ;

/**
 ***
 **** Toolkit Procedures
 ***
 **/

/***** **** *** ** * ClassInitialize * ** *** **** *****/

static void
ClassInitialize()
{
extern void XmuCvtStringToBitmap();
static XtConvertArgRec screenConvertArg[] = {
    {XtBaseOffset, (XPointer) XtOffset(Widget, core.screen),
	 sizeof(Screen *)}
    };
XcuAddStringToJustifyConverter () ;
XcuAddStringToGravityConverter () ;
XcuAddStringToFloatConverter () ;
XtAddConverter ("String", "Bitmap", XmuCvtStringToBitmap, 
		   screenConvertArg, XtNumber(screenConvertArg));

return ;
}

/***** **** *** ** * ClassPartInitialize * ** *** **** *****/

static void
ClassPartInitialize(class)
    WidgetClass class;
{
register XcuLabelWidgetClass c = (XcuLabelWidgetClass)class;

if (c->label_class.redisplay_text == XtInheritRedisplayText)
    c->label_class.redisplay_text = RedisplayText;

return ;
}

/***** **** *** ** * Initialize * ** *** **** *****/

static void
Initialize (request, new)
    Widget request, new ;
{
XcuLabelWidget lw = (XcuLabelWidget) new ;
Dimension explicit_width = request->core.width ;
Dimension explicit_height = request->core.height ;
Dimension preferred_width, preferred_height ;

if (lw->label.label == NULL) 
    lw->label.label = lw->core.name ;
else
    Xcu_copy_ds (&lw->label.label, lw->label.label) ;

if (lw->label.big_label != NULL) 
    Xcu_copy_ds (&lw->label.big_label, lw->label.big_label) ;

if (lw->label.bitmap != None)
    MakePixmapFromBitmap (lw) ;

if (lw->label.pixmap != None)
    GetPixmapGC (lw) ;

GetSensitiveTextGC (lw, lw->label.foreground) ;
GetInsensitiveTextGC (lw, lw->label.foreground) ;
if (lw->simple.shadow)
    GetBackgroundGC (lw, lw->simple.background_pixel) ;
else
    GetBackgroundGC (lw, lw->core.background_pixel) ;

lw->label.needs_fill = True ;
lw->simple.exposing = True ;

lw->label.x = NULL ;
lw->label.y = NULL ;
lw->label.line_width = NULL ;
lw->label.len = NULL ;

if (lw->label.big_label)
    {
    find_newlines (lw->label.big_label, &lw->label.locations, &lw->label.n_lines) ;
    SetTextWidthAndHeight (lw, lw->label.big_label) ;

    if (lw->core.width == 0)
	{
        lw->core.width = lw->label.total_width + 2*lw->label.internal_width +
						 lw->simple.total_facet_width ;
	}
    if (lw->core.height == 0)
	{
        lw->core.height = lw->label.total_height +
			  2*lw->label.internal_height +
			  lw->simple.total_facet_width ;

	}
    lw->label.big_width = lw->core.width ;
    lw->label.big_height = lw->core.height ;
    }

find_newlines (lw->label.label, &lw->label.locations, &lw->label.n_lines) ;
SetTextWidthAndHeight (lw, lw->label.label) ;
if (!lw->label.big_label)
    {
    if (lw->core.width == 0)
	{
	lw->core.width = lw->label.total_width + 2*lw->label.internal_width +
						 lw->simple.total_facet_width ;
	}
    if (lw->core.height == 0)
	{
	lw->core.height = lw->label.total_height +
			  2*lw->label.internal_height +
			  lw->simple.total_facet_width ;
	}
    }

/*
 * if width or height was explicitly set for this label and it is greater
 * than the preferred dimensions, then set big_label to them. 
 */

preferred_width = PreferredWidth(lw) ;
preferred_height = PreferredHeight(lw) ;
if (!lw->label.big_label &&
    (explicit_width > preferred_width || (explicit_height > preferred_height))
   )
    {
    lw->label.big_label = (String) 1 ;
    lw->label.big_width = (explicit_width > preferred_width) ?
			    explicit_width : preferred_width ;
    lw->label.big_height = (explicit_height > preferred_height) ?
			    explicit_height : preferred_height ;
    }
/**
lw->label.old_width = lw->core.width; 
lw->label.old_height = lw->core.height; 
**/

lw->label.facet_width0 = lw->simple.facet_width ;
lw->label.internal_width0 = lw->label.internal_width ;
lw->label.internal_height0 = lw->label.internal_height ;

(*XtClass(new)->core_class.resize) (new) ;

return ;
}

/***** **** *** ** * Resize * ** *** **** *****/

static void
Resize (w)
    Widget w ;
{
int i ;
XcuLabelWidget lw = (XcuLabelWidget) w ;
Dimension preferred_width ;
Dimension preferred_height ;
Dimension facet_divisor = lw->simple.shadow ? 1 : 2 ;

lw->simple.facet_width = lw->label.facet_width0 ;
lw->label.internal_width = lw->label.internal_width0 ;
lw->label.internal_height = lw->label.internal_height0 ;
preferred_width = PreferredWidth(lw) ;
preferred_height = PreferredHeight(lw) ;

if (lw->core.width < preferred_width && lw->label.preserve_text)
    {
    /* Since we have to get smaller, we'll kill the frills... */
    /* First reduce the internal padding */
    /* If the squeeze is still on, kill the facet */
    /* If the squeeze is still on, there's nothing left but text, and
       I don't think I should mess with it */
    Dimension delta, original_delta ;

    delta = (preferred_width - lw->core.width + lw->simple.total_facet_width) /
	    facet_divisor ;
    original_delta = delta ;
    if (delta > lw->label.internal_width)
	delta = lw->label.internal_width; 
    lw->label.internal_width -= delta ;

    delta = original_delta - delta ;
    if (delta > lw->simple.facet_width)
	delta = lw->simple.facet_width; 
    lw->simple.facet_width -= delta ;
    }

if (lw->core.height < preferred_height && lw->label.preserve_text)
    {
    /* same as for width */
    Dimension delta, original_delta ;

    delta = (preferred_height - lw->core.height + lw->simple.total_facet_width)
	    / facet_divisor ;
    original_delta = delta ;
    if (delta > lw->label.internal_height)
	delta = lw->label.internal_height; 
    lw->label.internal_height -= delta ;

    delta = original_delta - delta ;
    if (delta > lw->simple.facet_width)
	delta = lw->simple.facet_width; 
    lw->simple.facet_width -= delta ;
    }

for (i=0;  i < lw->label.n_lines;  i++)
    {
    switch (lw->label.justify)
      {
	case XcuJUSTIFY_LEFT   :
	    lw->label.x[i] = 0 ;
	    break ;

	case XcuJUSTIFY_RIGHT  :
	    lw->label.x[i] = lw->label.total_width - lw->label.line_width[i] ;
	    break ;

	case XcuJUSTIFY_CENTER :
	    lw->label.x[i] = (lw->label.total_width -
			      lw->label.line_width[i]) / 2 ;
	    break ;
      }
    switch (lw->label.gravity)
	{
	case XcuWEST_GRAVITY :
	case XcuNORTH_WEST_GRAVITY :
	case XcuSOUTH_WEST_GRAVITY :
	    lw->label.x[i] += lw->label.internal_width +
			      lw->simple.left_facet_width ;
	    break ;
	case XcuEAST_GRAVITY :
	case XcuNORTH_EAST_GRAVITY :
	case XcuSOUTH_EAST_GRAVITY :
	    lw->label.x[i] += ((int)lw->core.width - lw->label.internal_width -
			     lw->label.total_width - lw->simple.right_facet_width) ;
	    break ;
	case XcuNORTH_GRAVITY :
	case XcuCENTER_GRAVITY :
	case XcuSOUTH_GRAVITY :
	    if (lw->simple.shadow)
		lw->label.x[i] += ((int)lw->core.width -
				    lw->label.total_width) / 2 +
				    lw->simple.left_facet_width ;
	    else
		lw->label.x[i] += ((int)lw->core.width -
				    lw->label.total_width) / 2 ;
	    break ;
	case XcuNO_GRAVITY :
	    break ;
	}

    /* to protect against the dreaded unsigned */
    if (lw->label.x[i] < 0)
	lw->label.x[i] = 0 ;

    lw->label.y[i] =
	lw->label.line_height * (i+1) +
	lw->label.line_height * (lw->label.line_factor - 1.0) * i -
	((lw->label.pixmap == None) ? lw->label.font->max_bounds.descent : 0) ;

    switch (lw->label.gravity)
	{
	case XcuNORTH_WEST_GRAVITY :
	case XcuNORTH_GRAVITY :
	case XcuNORTH_EAST_GRAVITY :
	    lw->label.y[i] += lw->label.internal_height +
			      lw->simple.top_facet_width ;
	    break ;

	case XcuWEST_GRAVITY :
	case XcuCENTER_GRAVITY :
	case XcuEAST_GRAVITY :
	    if (lw->simple.shadow)
		lw->label.y[i] += ((int)lw->core.height -
				    lw->label.total_height) / 2 +
				    lw->simple.left_facet_width ;
	    else
		lw->label.y[i] += ((int)lw->core.height -
				    lw->label.total_height) / 2 ;
	    break ;

	case XcuSOUTH_WEST_GRAVITY :
	case XcuSOUTH_GRAVITY :
	case XcuSOUTH_EAST_GRAVITY :
	    lw->label.y[i] += (int)lw->core.height - lw->label.total_height -
				lw->label.internal_height -
				lw->simple.bot_facet_width ;
	    break ;
	case XcuNO_GRAVITY :
	    break ;
	}
    }


/* for invocation of callbacks */
(*superclass->core_class.resize)( w ) ;

return ;
}

/***** **** *** ** * SetValues * ** *** **** *****/

/*
 * Set specified arguments into widget
 */

/* ARGSUSED */
static Boolean
SetValues (current, request, new)
    Widget current, request, new ;
{
XcuLabelWidget curlw = (XcuLabelWidget) current ;
XcuLabelWidget newlw = (XcuLabelWidget) new ;
Boolean need_to_redisplay = False ;
Boolean was_resized = False ;

if (newlw->label.label == NULL)
    Xcu_copy_ds (&newlw->label.label, newlw->core.name) ;

if (curlw->label.big_label != newlw->label.big_label)
    {
    was_resized = True ;
    }

newlw->label.newlines_found = False ;
if (curlw->label.label != newlw->label.label)
    {
    if (curlw->label.label != curlw->core.name)
	XtFree( (char *)curlw->label.label ) ;

    if (newlw->label.label != newlw->core.name)
	Xcu_copy_ds (&newlw->label.label, newlw->label.label) ;

    find_newlines (newlw->label.label, &newlw->label.locations, &newlw->label.n_lines) ;
    newlw->label.newlines_found = True ;

    was_resized = True ;
    }

if (curlw->label.line_factor != newlw->label.line_factor)
    {
    union {int iii ; float xxx ;} uni ;
    /*
     * This seems like a disgusting hack, but it's virtually impossible to
     * handle floats in resources...
     */
    uni.xxx = newlw->label.line_factor ;
    newlw->label.line_factor = *((float *) uni.iii) ;
    was_resized = True ;
    }

if (curlw->label.pixmap != newlw->label.pixmap)
    {
    XFreePixmap (XtDisplay(newlw), newlw->label.pixmap) ;
    XtReleaseGC ((Widget)newlw, newlw->label.pixmap_GC) ;
    GetPixmapGC (newlw) ;
    }
else
if (curlw->label.bitmap != newlw->label.bitmap)
    {
    XFreePixmap (XtDisplay(newlw), newlw->label.pixmap) ;
    MakePixmapFromBitmap (newlw) ;
    XtReleaseGC ((Widget)newlw, newlw->label.pixmap_GC) ;
    GetPixmapGC (newlw) ;
    }

if (curlw->label.tile != newlw->label.tile)
    {
    XtReleaseGC ((Widget)newlw, newlw->label.pixmap_GC) ;
    GetPixmapGC (newlw) ;
    }

if (
   was_resized
   || curlw->label.font != newlw->label.font
   || curlw->label.internal_height != newlw->label.internal_height
   || curlw->label.internal_width != newlw->label.internal_width
   || curlw->label.pixmap != newlw->label.pixmap
   || curlw->simple.facet_width != newlw->simple.facet_width
   )
    {
    if (newlw->label.big_label != curlw->label.big_label)
	{
	find_newlines (newlw->label.big_label, &newlw->label.locations, &newlw->label.n_lines) ;
	SetTextWidthAndHeight (newlw, newlw->label.big_label) ;

	newlw->core.width = newlw->label.total_width +
			    2 * newlw->label.internal_width +
			    newlw->simple.total_facet_width ;
	newlw->core.height = newlw->label.total_height +
			     2 * newlw->label.internal_height +
			     newlw->simple.total_facet_width ;
	newlw->label.big_width = newlw->core.width ;
	newlw->label.big_height = newlw->core.height ;
	}

    if (!newlw->label.newlines_found)
	find_newlines (newlw->label.label,
		       &newlw->label.locations, &newlw->label.n_lines) ;
    SetTextWidthAndHeight (newlw, newlw->label.label) ;
    (*XtClass(newlw)->core_class.resize) (new) ;
    need_to_redisplay = True ;
    if (newlw->label.big_label == curlw->label.big_label)
	{
	Dimension new_width, new_height ;
	new_width = newlw->label.total_width +
			    2 * newlw->label.internal_width +
			    newlw->simple.total_facet_width ;
	new_height = newlw->label.total_height +
			    2 * newlw->label.internal_height + newlw->simple.total_facet_width ;
	if (new_width > curlw->core.width || !newlw->label.big_label)
	    newlw->core.width = new_width ;
	if (new_height > curlw->core.height || !newlw->label.big_label)
	    newlw->core.height = new_height ;
	}
    if (curlw->label.internal_width != newlw->label.internal_width)
	newlw->label.internal_width0 = newlw->label.internal_width ;
    if (curlw->label.internal_height != newlw->label.internal_height)
	newlw->label.internal_height0 = newlw->label.internal_height ;
    if (curlw->simple.facet_width != newlw->simple.facet_width)
	newlw->label.facet_width0 = newlw->simple.facet_width ;
    if (curlw->simple.facet_width != newlw->simple.facet_width)
	newlw->label.facet_width0 = newlw->simple.facet_width ;
    }
else
if (
   curlw->label.justify != newlw->label.justify
   || curlw->label.gravity != newlw->label.gravity
   )
    {
    if ((int)newlw->label.gravity > 9)
	newlw->label.gravity = (XcuGravity) 9 ; /* KLUDGE! */
    /* These things don't cause width and height to change */
    /* But resize does what we need for justification/gravity */
    (*XtClass(newlw)->core_class.resize) (new) ;
    need_to_redisplay = True ;
    }

if (curlw->core.border_width != newlw->core.border_width)
    {
    Mask changes_mask ;
    XWindowChanges changes ;

    changes_mask = CWBorderWidth ;
    changes.border_width = newlw->core.border_width ;
    if (XtIsRealized(current))
	{
	XConfigureWindow (XtDisplay(current), XtWindow(current),
				 changes_mask, &changes) ;
	}
    }

if (curlw->label.foreground != newlw->label.foreground
    || curlw->label.font->fid != newlw->label.font->fid)
    {
    XtReleaseGC ((Widget)newlw, newlw->label.sensitive_text_GC) ;
    XtReleaseGC ((Widget)newlw, newlw->label.insensitive_text_GC) ;
    GetSensitiveTextGC (newlw, newlw->label.foreground) ;
    GetInsensitiveTextGC (newlw, newlw->label.foreground) ;
    need_to_redisplay = True ;
    }

if (curlw->core.background_pixel != newlw->core.background_pixel ||
    curlw->simple.background_pixel != newlw->simple.background_pixel)
    {
    /***
    XtReleaseGC ((Widget)curlw, curlw->label.background_GC) ;
    ***/
    if (newlw->simple.shadow)
	GetBackgroundGC (newlw, newlw->simple.background_pixel) ;
    else
	GetBackgroundGC (newlw, newlw->core.background_pixel) ;
    need_to_redisplay = True ;
    }

return need_to_redisplay ;
}

/***** **** *** ** * QueryGeometry * ** *** **** *****/

static XtGeometryResult
QueryGeometry (widget, requested, preferred)
    Widget widget ;
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
XcuLabelWidget lw = (XcuLabelWidget) widget ;

preferred->width = PreferredWidth (lw) ;
preferred->height = PreferredHeight (lw); 
preferred->request_mode = (CWWidth | CWHeight) ;

if ((requested->request_mode & (CWWidth | CWHeight)) == 0)
    {
    /* parent isn't interested in anything we're interested in */
    return XtGeometryNo ;
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

/***** **** *** ** * Destroy * ** *** **** *****/

static void
Destroy (widget)
    Widget widget ;
{
XcuLabelWidget lw = (XcuLabelWidget) widget ;

/*
 * free dynamically allocated data
 */

if (!XtIsRealized (widget))
    return ;

if (lw->label.x)	XtFree ((char *) lw->label.x) ;
if (lw->label.y) XtFree ((char *) lw->label.y) ;
if (lw->label.len) XtFree ((char *) lw->label.len) ;
if (lw->label.line_width) XtFree ((char *) lw->label.line_width) ;
if (lw->label.label) XtFree ((char *) lw->label.label) ;
if (lw->label.locations) XtFree ((char *) lw->label.locations) ;
XtReleaseGC ((Widget)lw, lw->label.sensitive_text_GC) ;
XtReleaseGC ((Widget)lw, lw->label.insensitive_text_GC) ;

return ;
}

/***** **** *** ** * Redisplay * ** *** **** *****/

static void Redisplay(w, event, region)
    Widget w;
    XEvent *event;		/* unused */
    Region region;		/* unused */
{
XcuLabelWidget lw = (XcuLabelWidget) w;
GC	    gc ;

/*
 * Repaint the background if need be
 * Redisplay the text
 * Allow superclass to handle facets and highlights
 */

(*superclass->core_class.expose)( w, event, region ) ;

if (lw->label.pixmap == None)
    {
    gc = IsSensitive(w) ?
	    lw->label.sensitive_text_GC : lw->label.insensitive_text_GC ;
    if (lw->label.needs_fill)
	{
	if (lw->simple.shadow)
	    {
	    XFillRectangle(XtDisplay(w),XtWindow(w), lw->label.background_GC,
		    lw->simple.left_facet_width, lw->simple.top_facet_width,
		    lw->core.width - lw->simple.facet_width,
		    lw->core.height - lw->simple.facet_width);
	    if (lw->simple.facet_width)
		XDrawRectangle(XtDisplay(w),XtWindow(w),
		    IsSensitive(w) ? lw->label.sensitive_text_GC :
				     lw->label.insensitive_text_GC,
		    lw->simple.left_facet_width, lw->simple.top_facet_width,
		    lw->core.width - lw->simple.facet_width - 1,
		    lw->core.height - lw->simple.facet_width - 1);
	    }
	else
	    {
	    XFillRectangle(XtDisplay(w),XtWindow(w), lw->label.background_GC,
		    lw->simple.facet_width, lw->simple.facet_width,
		    lw->core.width - 2*lw->simple.facet_width,
		    lw->core.height - 2*lw->simple.facet_width);
	    }
	}
    else
    if (lw->simple.shadow)
	{
	XDrawRectangle(XtDisplay(w),XtWindow(w),
		IsSensitive(w) ? lw->label.sensitive_text_GC :
				 lw->label.insensitive_text_GC,
		lw->simple.left_facet_width, lw->simple.top_facet_width,
		lw->core.width - lw->simple.facet_width - 1,
		lw->core.height - lw->simple.facet_width - 1);
	}
    (*((XcuLabelWidgetClass)XtClass(w))->label_class.redisplay_text)
	(w, event, region) ;
    }
else
   {
    if (lw->label.tile)
	{
        if (lw->simple.shadow)
	   XFillRectangle(XtDisplay(w),XtWindow(w), lw->label.pixmap_GC,
		    lw->simple.left_facet_width, lw->simple.top_facet_width,
		    lw->core.width - lw->simple.facet_width,
		    lw->core.height - lw->simple.facet_width);
        else
	   XFillRectangle(XtDisplay(w),XtWindow(w), lw->label.pixmap_GC,
		    lw->simple.facet_width, lw->simple.facet_width,
		    lw->core.width - 2*lw->simple.facet_width,
		    lw->core.height - 2*lw->simple.facet_width);
	}
    else
	{
        if (lw->simple.shadow)
	   XFillRectangle(XtDisplay(w),XtWindow(w), lw->label.background_GC,
		    lw->simple.left_facet_width, lw->simple.top_facet_width,
		    lw->core.width - lw->simple.facet_width,
		    lw->core.height - lw->simple.facet_width);
        else
	   XFillRectangle(XtDisplay(w),XtWindow(w), lw->label.background_GC,
		    lw->simple.facet_width, lw->simple.facet_width,
		    lw->core.width - 2*lw->simple.facet_width,
		    lw->core.height - 2*lw->simple.facet_width);
	XCopyArea(
	  XtDisplay(w), lw->label.pixmap, XtWindow(w), lw->label.pixmap_GC,
	  0,0,
	  lw->label.total_width, lw->label.total_height,
	  lw->label.x[0], lw->label.y[0]);
	}
    }

return ;
}

/**
 ***
 **** Private Procedures
 ***
 **/

/***** **** *** ** * RedisplayText * ** *** **** *****/

/*
 * Repaint the widget window
 */

static void
RedisplayText (w, event, region)
    Widget w ;
    XEvent *event ;
    Region region ;
{
int i ;
XcuLabelWidget lw = (XcuLabelWidget) w ;

for (i=0;  i < lw->label.n_lines;  i++)
    {
    XDrawString
	(
	XtDisplay(w), XtWindow(w),
	IsSensitive(w) ?
		lw->label.sensitive_text_GC : lw->label.insensitive_text_GC,
	lw->label.x[i], lw->label.y[i],
	&lw->label.label[lw->label.locations[i]], (int) lw->label.len[i]
	) ;
    }
return ;
}

/***** **** *** ** * SetTextWidthAndHeight * ** *** **** *****/

/*
 * Calculate width and height of displayed text in pixels
 */

static void
SetTextWidthAndHeight (lw, text)
    XcuLabelWidget lw ;
    String text ;
{
#define lwl	lw->label

int i ;
register XFontStruct	*fs = lwl.font ;

/*
 * allocate space for arrays of x, y, and len
 */

if (XtIsRealized ((Widget)lw))
    {
    if (lwl.x) XtFree ((char *) lwl.x) ;
    if (lwl.y) XtFree ((char *) lwl.y) ;
    if (lwl.len) XtFree ((char *) lwl.len) ;
    if (lwl.line_width) XtFree ((char *) lwl.line_width) ;
    }

lwl.x = (Position *) XtMalloc (lwl.n_lines * sizeof (Position)) ;
lwl.y = (Position *) XtMalloc (lwl.n_lines * sizeof (Position)) ;
lwl.len = (unsigned int *) XtMalloc (lwl.n_lines * sizeof (unsigned int)) ;
lwl.line_width = (int *) XtMalloc (lwl.n_lines * sizeof (int)) ;

if (lw->label.pixmap != None)
    {
    lw->label.total_height = lw->label.pixmap_height;
    lw->label.total_width = lw->label.pixmap_width;
    lwl.x[0] = 0 ;
    lwl.y[0] = 0 ;
    lwl.line_width[0] = lw->label.pixmap_width ;
    return;
    }

/*
 * Now to find the width and height of the entire label
 * The height is (sum of line heights + sum of interline spacings)
 * The width is max (line widths)
 */

lwl.line_height = fs->max_bounds.ascent + fs->max_bounds.descent ;
lwl.total_height = lwl.line_height * lwl.n_lines +
		   lwl.line_height * (lwl.line_factor - 1.0) *
		   (lwl.n_lines - 1) ;

lwl.total_width = 0 ;
for (i=0;  i < lwl.n_lines;  i++)
    {
    lwl.len[i] = XtStrlen(&text[lwl.locations[i]]) ;
    lwl.line_width[i] = XTextWidth(fs, &text[lwl.locations[i]], lwl.len[i]) ;
    if (lwl.total_width < lwl.line_width[i])
	lwl.total_width = lwl.line_width[i] ;
    }

return ;
}

/***** **** *** ** * GetInsensitiveTextGC * ** *** **** *****/

static void
GetInsensitiveTextGC(lw, foreground_color)
    XcuLabelWidget lw;
    Pixel foreground_color ;
{
XGCValues	values;

values.foreground = foreground_color ;
values.font	  = lw->label.font->fid ;
values.tile       = XmuCreateStippledPixmap (XtScreen((Widget)lw),
					     lw->label.foreground,
					     lw->core.background_pixel,
					     lw->core.depth) ;
values.fill_style = FillTiled ;

lw->label.insensitive_text_GC = XtGetGC(
    (Widget)lw, 
    (unsigned) GCForeground | GCFont | GCTile | GCFillStyle, 
    &values);
return ;
}

/***** **** *** ** * GetSensitiveTextGC * ** *** **** *****/

static void
GetSensitiveTextGC (lw, foreground_color)
    XcuLabelWidget lw ;
    Pixel foreground_color ;
{
XGCValues	values ;

values.foreground	= foreground_color ;
values.font		= lw->label.font->fid ;

lw->label.sensitive_text_GC = XtGetGC(
    (Widget)lw,
    (unsigned) GCForeground | GCFont,
    &values) ;
return ;
}

/***** **** *** ** * GetPixmapGC * ** *** **** *****/

static void
GetPixmapGC (lw)
    XcuLabelWidget lw ;
{
XGCValues	values ;
unsigned	flags = GCForeground | GCBackground ;

values.foreground	= lw->label.foreground ;
if (lw->simple.shadow)
    values.background	= lw->simple.background_pixel ;
else
    values.background	= lw->core.background_pixel ;

if (lw->label.tile)
    {
    values.fill_style = FillTiled ;
    values.tile = lw->label.pixmap ;
    flags |= (GCFillStyle | GCTile) ;
    }

lw->label.pixmap_GC = XtGetGC((Widget)lw, flags, &values) ;
return ;
}

/***** **** *** ** * GetBackgroundGC * ** *** **** *****/

static void
GetBackgroundGC (lw, background_color)
    XcuLabelWidget lw ;
    Pixel background_color ;
{
XGCValues	values ;

values.foreground	= background_color ;

lw->label.background_GC = XtGetGC(
    (Widget)lw,
    (unsigned) GCForeground,
    &values) ;
return ;
}

/***** **** *** ** * find_newlines * ** *** **** *****/

#define FIND_NEWLINES_MAX_LINES	100

static void
find_newlines (text, line_locations, n_lines)
    char *text ;
    unsigned int **line_locations ;
    unsigned int *n_lines ;
{
int string_length ;
int loc[FIND_NEWLINES_MAX_LINES] ;	/* local version of line_locations */
int n = 0 ;				/* local version of n_lines */
int i ;

loc[n++] = 0 ;				/* first line */

/*
 * check each character for a newline ('\n')
 */

string_length = strlen(text) ;
for (i=0;  i < string_length; i++)
    {
    if (text[i] == '\n')
	{
	if (n == FIND_NEWLINES_MAX_LINES)
	    {
	    XtWarning ("XcuLabel: Too many newlines!") ;
	    }
	else
	    {
	    loc[n++] = i+1 ;		/* subsequent lines */
	    text[i] = '\0' ;
	    }
	}
    }

/* allocate space for the calling array */
*line_locations = (unsigned int *) XtMalloc (n * sizeof (unsigned int)) ;

/* Now copy the line locations to the calling array */
for (i=0;  i < n;  i++)
    {
    (*line_locations)[i] = loc[i] ;
    }

*n_lines = n ;
return ;
}

static Dimension
PreferredWidth (lw)
    XcuLabelWidget lw ;
{
Dimension smallest_width = lw->label.total_width +
		     2*lw->label.internal_width +
		     lw->simple.total_facet_width ;
if (lw->label.big_label && lw->label.big_width > smallest_width)
	return lw->label.big_width ;
else
    {
    return smallest_width ;
   }
}

static Dimension
PreferredHeight (lw)
    XcuLabelWidget lw ;
{
Dimension smallest_height = lw->label.total_height +
		     2*lw->label.internal_height +
		     lw->simple.total_facet_width ;
if (lw->label.big_label && lw->label.big_height > smallest_height)
	return lw->label.big_height ;
else
    {
    return smallest_height ;
   }
}


static void
MakePixmapFromBitmap (lw)
    XcuLabelWidget lw ;
{
Window root;
Pixel bkg_pixel = (lw->simple.shadow) ? lw->simple.background_pixel :
					lw->core.background_pixel ;
int x, y ;
unsigned int bw, wr, hr, dr ;

XGetGeometry(XtDisplay(lw), (Drawable)lw->label.bitmap,
		&root, &x, &y, &wr, &hr, &bw, &dr) ;
lw->label.pixmap_width = wr ;
lw->label.pixmap_height = hr ;
lw->label.depth = DefaultDepthOfScreen(DefaultScreenOfDisplay(XtDisplay(lw))) ;
lw->label.pixmap = XmuCreatePixmapFromBitmap
	    (XtDisplay(lw), XDefaultRootWindow(XtDisplay(lw)),
	    lw->label.bitmap,
	    lw->label.pixmap_width, lw->label.pixmap_height,
	    lw->label.depth,
	    lw->label.foreground, bkg_pixel) ;
return ;
}
