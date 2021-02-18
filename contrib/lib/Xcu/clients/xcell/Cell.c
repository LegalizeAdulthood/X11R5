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
 * XcuCell.c - XcuCell widget
 */

#define XtStrlen(s)		((s) ? strlen(s) : 0)
#define IsSensitive(w) ((w)->core.sensitive && (w)->core.ancestor_sensitive)

#define	rand2()		(rand() & 1)
#define	rand3()		(rand() % 3)
#define	rand4()		(rand() & 3)
#define	rand5()		(rand() % 5)
#define	rand6()		(rand() % 6)
#define	rand7()		(rand() & 7)

#define MAX_COLORS_ALLOWED 8

#include <ctype.h>
#include <X11/IntrinsicP.h>
#include <X11/Xos.h>
#include <X11/StringDefs.h>
#include "CellP.h"
#include "CellI.h"
#include <X11/Xcu/SimpleP.h>

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

#define offset(field) XtOffset(XcuCellWidget, field)

static Dimension def_zero = 0 ;
static Boolean def_false = False ;
static Boolean def_true = True ;
static int def_one = 1 ;
static int def_minus1 = -1 ;
static int def_seed = 317317317 ;

#ifdef DEFFILE
#undef DEFFILE
#endif
#define DEFFILE "."

static XtResource resources[] =
    {
     {XtNcolors, XtCCellColors, XtRString, sizeof(String),
	offset(cell.color_string), XtRString,
	"Black,White,Blue,Green,Red,Yellow,Cyan,Magenta"}
    ,{XtNcolor0, XtCCellColor, XtRString, sizeof(String),
	offset(cell.color0), XtRString, (XPointer) NULL}
    ,{XtNcolor1, XtCCellColor, XtRString, sizeof(String),
	offset(cell.color1), XtRString, (XPointer) NULL}
    ,{XtNcolor2, XtCCellColor, XtRString, sizeof(String),
	offset(cell.color2), XtRString, (XPointer) NULL}
    ,{XtNcolor3, XtCCellColor, XtRString, sizeof(String),
	offset(cell.color3), XtRString, (XPointer) NULL}
    ,{XtNcolor4, XtCCellColor, XtRString, sizeof(String),
	offset(cell.color4), XtRString, (XPointer) NULL}
    ,{XtNcolor5, XtCCellColor, XtRString, sizeof(String),
	offset(cell.color5), XtRString, (XPointer) NULL}
    ,{XtNcolor6, XtCCellColor, XtRString, sizeof(String),
	offset(cell.color6), XtRString, (XPointer) NULL}
    ,{XtNcolor7, XtCCellColor, XtRString, sizeof(String),
	offset(cell.color7), XtRString, (XPointer) NULL}
    ,{XtNdirectory, XtCCellDirectory, XtRString, sizeof(String),
	offset(cell.directory), XtRString, (XPointer) DEFFILE}
    ,{XtNignoreColors, XtCCellIgnoreColors, XtRString, sizeof(String),
	offset(cell.ignore_colors), XtRString, (XPointer)NULL}
    /***
    ,{XtNcolorsFile, XtCCellColorsFile, XtRString, sizeof(String),
	offset(cell.colors_file), XtRString, (XPointer)NULL}
    ***/
    ,{XtNrulesFile, XtCCellRulesFile, XtRString, sizeof(String),
	offset(cell.rules_file), XtRString, (XPointer)""}
    ,{XtNalternating, XtCCellAlternating, XtRBoolean, sizeof(Boolean),
	offset(cell.alternating), XtRBoolean, (XPointer)&def_false}
    ,{XtNperturb, XtCCellPerturb, XtRBoolean, sizeof(Boolean),
	offset(cell.masks_active), XtRBoolean, (XPointer)&def_false}
    ,{XtNpickFile, XtCCellPickFile, XtRString, sizeof(String),
	offset(cell.pick_file), XtRString, (XPointer)""}
    ,{XtNruleIndex, XtCCellRuleIndex, XtRInt, sizeof(int),
	offset(cell.rule_index_), XtRInt, (XPointer)&def_minus1}
    ,{XtNnColors, XtCCellNColors, XtRInt, sizeof(int),
	offset(cell.n_colors), XtRString, (XPointer)"6"}
    ,{XtNseed, XtCCellSeed, XtRInt, sizeof(int),
	offset(cell.seed_), XtRInt, (XPointer)&def_seed}
    ,{XtNcellType, XtCCellType, XtRString, sizeof(String),
	offset(cell.type_), XtRString, (XPointer)"lcr4"}
    ,{XtNperRow, XtCCellPerRow, XtRInt, sizeof(int),
	offset(cell.per_row_), XtRInt, (XPointer)&def_one}
    ,{XtNperCol, XtCCellPerCol, XtRInt, sizeof(int),
	offset(cell.per_col_), XtRInt, (XPointer)&def_one}
    ,{XtNsequence, XtCCellSequence, XtRXcuCellSequence,
	sizeof(XcuCellSequenceType),
	offset(cell.sequence), XtRString, (XPointer)"OrderedPermute"}
    ,{XtNfrequency, XtCCellFrequency, XtRXcuCellFrequency,
	sizeof(XcuCellFrequencyType),
	offset(cell.frequency), XtRString, (XPointer)"Fixed_frequency"}
    ,{XtNorderedRules, XtCCellOrderedRules, XtRBoolean, sizeof(Boolean),
	offset(cell.order_), XtRBoolean, (XPointer)&def_false}
    ,{XtNverticalMerge, XtCCellVerticalMerge, XtRBoolean, sizeof(Boolean),
	offset(cell.vertical_merge_), XtRBoolean, (XPointer)&def_false}
    ,{XtNhorizontalMerge, XtCCellHorizontalMerge, XtRBoolean, sizeof(Boolean),
	offset(cell.horizontal_merge_), XtRBoolean, (XPointer)&def_false}
    ,{XtNinternalWidth, XtCWidth, XtRDimension,  sizeof(Dimension),
	offset(cell.internal_width), XtRDimension, (XPointer)&def_zero}
    ,{XtNinternalHeight, XtCHeight, XtRDimension, sizeof(Dimension),
	offset(cell.internal_height), XtRDimension, (XPointer)&def_zero}
    ,{XtNfacetWidth, XtCFacetWidth, XtRDimension, sizeof(Dimension),
	offset(simple.facet_width), XtRDimension, (XPointer)&def_zero}
    ,{XtNmutate, XtCMutate, XtRBoolean, sizeof(Boolean),
	offset(cell.mutate), XtRBoolean, (XPointer)&def_false}
    ,{XtNbeep, XtCBeep, XtRBoolean, sizeof(Boolean),
	offset(cell.beep), XtRBoolean, (XPointer)&def_true}
    /*** intentionally undocumented ! Probably used only by XcuWlm ***/
    ,{XtNprocedureList, XtCProcedureList,
      XtRProcedureList, sizeof(XtProcedureList *),
      offset(cell.procedure_list), XtRProcedureList, (XPointer)NULL}
    } ;

static void Pick () ;

static char defaultTranslations[] =
    "<Btn1Down>:	pick()" ;

static XtActionsRec actionsList[] =
{
  {"pick",		Pick},
};

static void ClassInitialize() ;
static void ClassPartInitialize();
static void Initialize() ;
static void Resize() ;
static void Redisplay() ;
static void Destroy() ;
static Boolean SetValues() ;
static Dimension PreferredWidth() ;
static Dimension PreferredHeight() ;
static XtGeometryResult QueryGeometry() ;
static void GetCellGC () ;
static void GetBackgroundGC () ;
static void XcuCvtStringToColors();
static void XcuCvtWordToColor();
static void XcuConvertStringToWords ();
static void CvtStringToFrequency() ;
static void CvtStringToSequence() ;

static void get_rule_type () ;
static void get_save_rule () ;
static void get_order_number () ;
static void get_pick_file () ;
static void get_mod_file () ;
static void get_size_arrays () ;
static void get_seed () ;
static void get_rules () ;
static void get_wraps () ;
static void get_counts () ;
static void get_initial_row () ;
static void get_rule () ;
static void get_ignores () ;
static void get_colors () ;
static void get_permutations () ;
static void get_rcells () ;
static void p_rule () ;
static void put_row () ;
static void do_gets () ;
static void set_changed_false () ;
static void set_changed_true () ;
static void permute_colormap () ;
static long rndm() ;

static void permute (/* int, int, int, int*, int**, int *, int * */) ;
static int  **permutations (/* int, int, int *, int * */) ;
static int  n_by_m_perm (/* int, int */) ;
static int  factorial (/* int */) ;

extern XrmQuark XcuSimplifyArg () ;

#define superclass (&xcuSimpleClassRec)

XcuCellClassRec xcuCellClassRec =
{
  {
  /* core_class fields */	
    /* superclass	  	*/	(WidgetClass) &xcuSimpleClassRec,
    /* class_name	  	*/	"Cell",
    /* widget_size	  	*/	sizeof(XcuCellRec),
    /* class_initialize   	*/	ClassInitialize,
    /* class_part_initialize	*/	ClassPartInitialize,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize		  	*/	XtInheritRealize,
    /* actions		  	*/	actionsList,
    /* num_actions	  	*/	XtNumber(actionsList),
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
    /* tm_table		   	*/	defaultTranslations,
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
    NULL,                        /* XcuCell Class fields  */
  },
} ;

/* For use by clients... */
WidgetClass xcuCellWidgetClass = (WidgetClass) &xcuCellClassRec ;

/**
 ***
 **** Toolkit Procedures
 ***
 **/

/***** **** *** ** * ClassInitialize * ** *** **** *****/

static XrmQuark	XrmQEfixed, XrmQEpage, XrmQErule, XrmQEline ;
static XrmQuark	XrmQEordered, XrmQErandomp, XrmQErandomc ;

static void
ClassInitialize()
{
XrmQEfixed   = XrmStringToQuark("fixedfrequency") ;
XrmQEpage = XrmStringToQuark("pagefrequency") ;
XrmQErule  = XrmStringToQuark("rulefrequency") ;
XrmQEline  = XrmStringToQuark("linefrequency") ;
XtAddConverter( XtRString, XtRXcuCellFrequency, CvtStringToFrequency,
		NULL, 0 ) ;
XrmQEordered   = XrmStringToQuark("orderedpermute") ;
XrmQErandomp = XrmStringToQuark("randompermute") ;
XrmQErandomc  = XrmStringToQuark("randomcolor") ;
XtAddConverter( XtRString, XtRXcuCellSequence, CvtStringToSequence,
		NULL, 0 ) ;
return ;
}

/***** **** *** ** * ClassPartInitialize * ** *** **** *****/

static void
ClassPartInitialize(class)
    WidgetClass class;
{
#ifdef XXX
register XcuCellWidgetClass c = (XcuCellWidgetClass)class;

/*****
if (c->cell_class.redisplay_text == XtInheritRedisplayText)
    c->cell_class.redisplay_text = RedisplayText;
*****/

#endif
return ;
}

/***** **** *** ** * Initialize * ** *** **** *****/

static void
Initialize (request, new)
    Widget request, new ;
{
XcuCellWidget lw = (XcuCellWidget) new ;
Dimension facet_add = (lw->simple.shadow ? 1 : 2) * lw->simple.facet_width ;

static String XcuCellNextText = "XcuCellNextMap" ;
static String XcuCellFirstText = "XcuCellFirstMap" ;

lw->cell.procedure_list = (XtProcedureList *)
			   XtMalloc (3 * sizeof (XtProcedureList)) ;
lw->cell.procedure_list[0].name = XcuCellNextText ;
lw->cell.procedure_list[0].procedure = XcuCellNextMap ;
lw->cell.procedure_list[1].name = XcuCellFirstText ;
lw->cell.procedure_list[1].procedure = XcuCellFirstMap ;
lw->cell.procedure_list[2].name = NULL ;
lw->cell.procedure_list[2].procedure = NULL ;


if (lw->cell.rules_file) Xcu_copy_ds(&lw->cell.rules_file, lw->cell.rules_file);
if (lw->cell.pick_file) Xcu_copy_ds(&lw->cell.pick_file, lw->cell.pick_file);
if (lw->cell.directory) Xcu_copy_ds(&lw->cell.directory, lw->cell.directory);

XcuCvtStringToColors (lw) ;

GetCellGC (lw) ;

if (lw->simple.shadow)
    GetBackgroundGC (lw, lw->simple.background_pixel) ;
else
    GetBackgroundGC (lw, lw->core.background_pixel) ;

lw->simple.exposing = True ;

if (lw->core.width == 0)
    {
    if (lw->core.height == 0)
	lw->core.width = lw->core.height = 256 ;
    else
	lw->core.width = lw->core.height ;
    }

if (lw->core.height == 0)
    lw->core.height = lw->core.width ;

lw->cell.width = lw->core.width - 2*lw->cell.internal_width - facet_add ;
lw->cell.height = lw->core.height - 2*lw->cell.internal_height - facet_add ;

lw->cell.image = XCreateImage
	(
	XtDisplay(new),
	XDefaultVisualOfScreen(XtScreen(new)),
	XDefaultDepthOfScreen(XtScreen(new)),
	ZPixmap,
	0,
	(char *) NULL,
	lw->cell.width,
	1,
	8,
	0
	);


save_rule = (int ***) NULL ;
asave_rule = (int ***) NULL ;
irule = (int *) NULL ;
arule = (int *) NULL ;
fpick = (FILE *) NULL ;
mask = (int **) NULL ;
rule = (int **) NULL ;
lw->cell.image->data = NULL ;
row0 = NULL ;
row1 = NULL ;
lw->cell.first_initial_row = True ;
mask_iter = 0 ;
row = 0 ;
lw->cell.ignore_count = 0 ;
lw->cell.ignore_array = NULL ;
lw->cell.is_exposed = False ;
lw->cell.cells = NULL ;
lw->cell.rcells = NULL ;
lw->cell.permute_array = NULL ;
lw->cell.permutation_index = -1 ;

set_changed_true (lw) ;
lw->cell.mod_changed = lw->cell.masks_active ;
lw->cell.alt_changed = lw->cell.alternating ;
do_gets (lw) ;

(*XtClass(new)->core_class.resize) (new) ;

return ;
}

/***** **** *** ** * Resize * ** *** **** *****/

static void
Resize (w)
    Widget w ;
{
XcuCellWidget lw = (XcuCellWidget) w ;
Dimension facet_add = (lw->simple.shadow ? 1 : 2) * lw->simple.facet_width ;

lw->cell.width = lw->core.width - facet_add - 2 * lw->cell.internal_width ;
lw->cell.height = lw->core.height - facet_add - 2 * lw->cell.internal_height ;
lw->cell.width &= (~3) ;
lw->cell.internal_width = (lw->core.width - lw->cell.width - facet_add) / 2 ;
lw->cell.internal_height = (lw->core.height - lw->cell.height - facet_add)/2 ;

lw->cell.width_changed = True ;
lw->cell.height_changed = True ;
do_gets (lw) ;

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
XcuCellWidget curlw = (XcuCellWidget) current ;
XcuCellWidget newlw = (XcuCellWidget) new ;
Boolean need_to_redisplay = False ;
Boolean was_resized = False ;

if (curlw->cell.color0 != newlw->cell.color0)
    {
    Xcu_copy_ds (&newlw->cell.color0, newlw->cell.color0) ;
    XcuCvtWordToColor (newlw, newlw->cell.color0, newlw->cell.colors, 0) ;
    newlw->cell.colors_changed = True ;
    }

if (curlw->cell.color1 != newlw->cell.color1)
    {
    Xcu_copy_ds (&newlw->cell.color1, newlw->cell.color1) ;
    XcuCvtWordToColor (newlw, newlw->cell.color1, newlw->cell.colors, 1) ;
    newlw->cell.colors_changed = True ;
    }

if (curlw->cell.color2 != newlw->cell.color2)
    {
    Xcu_copy_ds (&newlw->cell.color2, newlw->cell.color2) ;
    XcuCvtWordToColor (newlw, newlw->cell.color2, newlw->cell.colors, 2) ;
    newlw->cell.colors_changed = True ;
    }

if (curlw->cell.color3 != newlw->cell.color3)
    {
    Xcu_copy_ds (&newlw->cell.color3, newlw->cell.color3) ;
    XcuCvtWordToColor (newlw, newlw->cell.color3, newlw->cell.colors, 3) ;
    newlw->cell.colors_changed = True ;
    }

if (curlw->cell.color4 != newlw->cell.color4)
    {
    Xcu_copy_ds (&newlw->cell.color4, newlw->cell.color4) ;
    XcuCvtWordToColor (newlw, newlw->cell.color4, newlw->cell.colors, 4) ;
    newlw->cell.colors_changed = True ;
    }

if (curlw->cell.color5 != newlw->cell.color5)
    {
    Xcu_copy_ds (&newlw->cell.color5, newlw->cell.color5) ;
    XcuCvtWordToColor (newlw, newlw->cell.color5, newlw->cell.colors, 5) ;
    newlw->cell.colors_changed = True ;
    }

if (curlw->cell.color6 != newlw->cell.color6)
    {
    Xcu_copy_ds (&newlw->cell.color6, newlw->cell.color6) ;
    XcuCvtWordToColor (newlw, newlw->cell.color6, newlw->cell.colors, 6) ;
    newlw->cell.colors_changed = True ;
    }

if (curlw->cell.color7 != newlw->cell.color7)
    {
    Xcu_copy_ds (&newlw->cell.color7, newlw->cell.color7) ;
    XcuCvtWordToColor (newlw, newlw->cell.color7, newlw->cell.colors, 7) ;
    newlw->cell.colors_changed = True ;
    }

if (curlw->cell.rules_file != newlw->cell.rules_file)
    {
    Xcu_copy_ds (&newlw->cell.rules_file, newlw->cell.rules_file) ;
    newlw->cell.rules_changed = True ;
    }

if (curlw->cell.ignore_colors != newlw->cell.ignore_colors)
    {
    Xcu_copy_ds (&newlw->cell.ignore_colors, newlw->cell.ignore_colors) ;
    newlw->cell.ignore_colors_changed = True ;
    }

if (curlw->cell.directory != newlw->cell.directory)
    {
    Xcu_copy_ds (&newlw->cell.directory, newlw->cell.directory) ;
    }

if (curlw->cell.masks_active != newlw->cell.masks_active)
    {
    if (newlw->cell.masks_active && newlw->cell.all_random_)
	{
	fprintf (stderr,
		 "Warning. Perturbation of random automatoms not allowed.\n") ;
	newlw->cell.masks_active = False ;
	}
    else
	{
	newlw->cell.mod_changed = True ;
	}
    }

if (curlw->cell.pick_file != newlw->cell.pick_file)
    {
    Xcu_copy_ds (&newlw->cell.pick_file, newlw->cell.pick_file) ;
    newlw->cell.pick_changed = True ;
    }

if (curlw->cell.rule_index_ != newlw->cell.rule_index_)
    {
    newlw->cell.rrule_changed = True ;
    }

if (curlw->cell.seed_ != newlw->cell.seed_)
    {
    newlw->cell.seed_changed = True ;
    }

if (newlw->cell.type_ &&
    (!curlw->cell.type_ || strcmp(newlw->cell.type_, curlw->cell.type_) != 0))
    {
    newlw->cell.type_changed = True ;
    }

if (curlw->cell.per_row_ != newlw->cell.per_row_)
    {
    newlw->cell.hmany_changed = True ;
    }

if (curlw->cell.per_col_ != newlw->cell.per_col_)
    {
    newlw->cell.many_changed = True ;
    }

if (curlw->cell.horizontal_merge_ != newlw->cell.horizontal_merge_)
    {
    newlw->cell.hmerge_changed = True ;
    }

if (curlw->cell.colors != newlw->cell.colors)
    {
    newlw->cell.colors_changed = True ;
    }

if (curlw->cell.n_colors != newlw->cell.n_colors)
    {
    newlw->cell.n_colors_changed = True ;
    }

if (curlw->cell.alternating != newlw->cell.alternating)
    {
    newlw->cell.alt_changed = True ;
    }

if (
   was_resized
   || curlw->simple.facet_width != newlw->simple.facet_width
   || curlw->cell.internal_width != newlw->cell.internal_width
   || curlw->cell.internal_height != newlw->cell.internal_height
   )
    {
    (*XtClass(newlw)->core_class.resize) (new) ;
    need_to_redisplay = True ;
    }
else
    {
    do_gets (newlw) ;
    }

if (curlw->core.background_pixel != newlw->core.background_pixel)
    {
    if (newlw->simple.shadow)
	GetBackgroundGC (newlw, newlw->simple.background_pixel) ;
    else
	GetBackgroundGC (newlw, newlw->core.background_pixel) ;
    need_to_redisplay = True ;
    }

if (newlw->cell.frequency != curlw->cell.frequency)
    {
    if (newlw->cell.frequency == XcuPAGE_FREQUENCY)
	{
	newlw->cell.permutation_index++ ;
	if (newlw->cell.permutation_index == newlw->cell.n_permutations)
	    newlw->cell.permutation_index = 0 ;
	}
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
XcuCellWidget lw = (XcuCellWidget) widget ;

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
/*
 * free dynamically allocated data
 */

if (!XtIsRealized (widget))
    return ;

return ;
}

/***** **** *** ** * Redisplay * ** *** **** *****/

static void Redisplay(w, event, region)
    Widget w;
    XEvent *event;		/* unused */
    Region region;		/* unused */
{
XcuCellWidget lw = (XcuCellWidget) w;

/*
 * Repaint the background if need be
 * Redisplay the text
 * Allow superclass to handle facets and highlights
 */

lw->cell.is_exposed = True ;
(*superclass->core_class.expose)( w, event, region ) ;

	if (lw->simple.shadow)
	    {
	    XFillRectangle(XtDisplay(w),XtWindow(w), lw->cell.background_GC,
		    0, 0,
		    lw->core.width - lw->simple.facet_width,
		    lw->core.height - lw->simple.facet_width);
	    }
	else
	    {
	    XFillRectangle(XtDisplay(w),XtWindow(w), lw->cell.background_GC,
		    lw->simple.facet_width, lw->simple.facet_width,
		    lw->core.width - 2*lw->simple.facet_width,
		    lw->core.height - 2*lw->simple.facet_width);
	    }

return ;
}

/**
 ***
 **** Private Procedures
 ***
 **/


/***** **** *** ** * GetBackgroundGC * ** *** **** *****/

static void
GetBackgroundGC (lw, background_color)
    XcuCellWidget lw ;
    Pixel background_color ;
{
XGCValues	values ;

values.foreground	= background_color ;

lw->cell.background_GC = XtGetGC(
    (Widget)lw,
    (unsigned) GCForeground,
    &values) ;
return ;
}

/***** **** *** ** * GetCellGC * ** *** **** *****/

static void
GetCellGC (lw)
    XcuCellWidget lw ;
{
XGCValues	values ;


values.function	= GXcopy ;
lw->cell.cell_GC = XtGetGC(
    (Widget)lw,
    (unsigned) GCFunction,
    &values) ;
return ;
}


static Dimension
PreferredHeight (lw)
    XcuCellWidget lw ;
{
Dimension facet_add = (lw->simple.shadow ? 1 : 2) * lw->simple.facet_width ;
Dimension smallest_height = lw->cell.height +
		     2*lw->cell.internal_height +
		     facet_add ;

return smallest_height ;
}

static Dimension
PreferredWidth (lw)
    XcuCellWidget lw ;
{
Dimension facet_add = (lw->simple.shadow ? 1 : 2) * lw->simple.facet_width ;
Dimension smallest_width = lw->cell.width +
		     2*lw->cell.internal_width +
		     facet_add ;
return smallest_width ;
}

/*
 * Convert String To Colors
 */

static void
XcuCvtStringToColors(lw)
    XcuCellWidget lw ;
{
Cardinal	i ;
String		*words ;
Display		*display = XtDisplay(lw) ;
XColor		visual_def_return ;

if (lw->cell.color_string == NULL) return ;

XcuConvertStringToWords (lw->cell.color_string, &words, &lw->cell.n_colors) ;

lw->cell.colors = (XColor *) XtMalloc (lw->cell.n_colors * sizeof(XColor)) ;

for (i=0;  i < lw->cell.n_colors;  i++)
    {
    if (XLookupColor(display,
		     DefaultColormap(display,DefaultScreen(display)),
		     words[i], &visual_def_return, &lw->cell.colors[i])
	== True)
	{
	}
    else
	{
	fprintf (stderr, "Error converting (%s) to Pixel\n", words[i]) ;
	fprintf (stderr, "Setting it to Black\n") ;
	lw->cell.colors[i].pixel = BlackPixel(display,DefaultScreen(display)) ;
	lw->cell.colors[i].red = 0 ;
	lw->cell.colors[i].green = 0 ;
	lw->cell.colors[i].blue = 0 ;
	}
    }

return ;
}

static void
XcuCvtWordToColor(cw, word, colors, index)
    XcuCellWidget cw ;
    String word ;
    XColor *colors ;
    int index ;
{
Display *display = XtDisplay(cw) ;
XColor visual_def_return ;

if (word == NULL) return ;
if (index >= cw->cell.max_colors) return ;

    if (XLookupColor(display,
			 DefaultColormap(display,DefaultScreen(display)),
			 word, &visual_def_return, &colors[index])
	== True)
	{
	cw->cell.cells[index] = colors[index].pixel ;
	if (index < cw->cell.max_rw_cells)
	    {
	    XStoreColor  (XtDisplay(cw),
		  DefaultColormap(XtDisplay(cw),DefaultScreen(XtDisplay(cw))),
		  &colors[index]) ;
	    }
	else
	    {
	    int h ;
	    h=XFreeColors (XtDisplay(cw),
	      DefaultColormap(XtDisplay(cw),DefaultScreen(XtDisplay(cw))),
	      &cw->cell.cells[index], 1, NULL) ;
	    h=XAllocColor (XtDisplay(cw),
	      DefaultColormap(XtDisplay(cw),DefaultScreen(XtDisplay(cw))),
	      &colors[index]) ;
	    cw->cell.cells[index] = colors[index].pixel ;
	    for (h=0;  h < cw->cell.per_row_;  h++)
		{
		if (index < cw->cell.reduct_)
		    cw->cell.rcells[h][index] = cw->cell.cells[index] ;
		}
	    }
	}
    else
	{
	fprintf (stderr, "Error converting (%s) to Pixel\n", word) ;
	fprintf (stderr, "Leaving it unchanged\n") ;
	}

return ;
}

static void
XcuConvertStringToWords (s, words, n_words)
    String s ;
    String **words ;
    int *n_words ;
{
int n_commas = 0 ;
int i ;
String p ;

/*
 * First, find the number of words in the string.
 * It is the number of commas, plus one.
 */
for (p=s;  *p;  p++)
    {
    if (*p == ',')
	n_commas++ ;
    }

*n_words = n_commas + 1 ;
*words = (String *) XtMalloc (*n_words * sizeof (String)) ;

/*
 * Copy each word into the output array
 */

for (i=0;  i < *n_words;  i++)
    {
    char *comma = strchr (s, ',') ;
    if (comma) *comma = '\0' ;
    (*words)[i] = (String) XtMalloc (strlen(s) + 1) ;
    strcpy ((*words)[i], s) ;
    if (!comma)
	break ;
    s = comma + 1 ;
    }
return ;
}

static void
get_rule_type (lw)
    XcuCellWidget lw ;
{
     if (strcmp(lw->cell.type_, "klcrt3") == 0)
	{ mix = KLCRT3 ; spread = 11 ; reduct = 3 ; windo = 5 ; }
else if (strcmp(lw->cell.type_, "klcrt4") == 0)
	{ mix = KLCRT4 ; spread = 16 ; reduct = 4 ; windo = 5 ; }
else if (strcmp(lw->cell.type_, "klcrt5") == 0)
	{ mix = KLCRT5 ; spread = 21 ; reduct = 5 ; windo = 5 ; }
else if (strcmp(lw->cell.type_, "lccrrr4") == 0)
	{ mix = LCCRRR4 ; spread = 19 ; reduct = 4 ; windo = 3 ; }
else if (strcmp(lw->cell.type_, "lccr4") == 0)
	{ mix = LCCR4 ; spread = 16 ; reduct = 4 ; windo = 3 ; }
else if (strcmp(lw->cell.type_, "lcr3") == 0)
	{ mix = LCR3 ; spread = 7 ; reduct = 3 ; windo = 3 ; }
else if (strcmp(lw->cell.type_, "lcr4") == 0)
	{ mix = LCR4 ; spread = 10 ; reduct = 4 ; windo = 3 ; }
else if (strcmp(lw->cell.type_, "lcr5") == 0)
	{ mix = LCR5 ; spread = 13 ; reduct = 5 ; windo = 3 ; }
else if (strcmp(lw->cell.type_, "lcr6") == 0)
	{ mix = LCR6 ; spread = 16 ; reduct = 6 ; windo = 3 ; }
else
	{
	fprintf (stderr, "Unknown rule: (%s)\n", lw->cell.type_) ;
	exit (1) ;
	}
get_mod_file (lw) ;
return ;
}

static void
get_save_rule (lw)
    XcuCellWidget lw ;
{
int is, it ;

if (save_rule)
    {
    for (is=0;  is < lw->cell.old_many_per_page;  is++)
	{
	for (it=0;  it < lw->cell.old_hmany_per_page;  it++)
	    {
	    XtFree (save_rule[is][it]) ;
	    XtFree (asave_rule[is][it]) ;
	    }
	XtFree (save_rule[is]) ;
	XtFree (asave_rule[is]) ;
	}
    XtFree (save_rule) ;
    XtFree (asave_rule) ;
    XtFree (irule) ;
    XtFree (arule) ;
    XtFree (lw->cell.mutation) ;
    }

save_rule = (int ***) XtMalloc (many_per_page * sizeof (int **)) ;
asave_rule = (int ***) XtMalloc (many_per_page * sizeof (int **)) ;
for (is=0;  is < many_per_page;  is++)
    {
    save_rule[is] = (int **) XtMalloc (hmany_per_page * sizeof (int *)) ;
    asave_rule[is] = (int **) XtMalloc (hmany_per_page * sizeof (int *)) ;
    for (it=0;  it < hmany_per_page;  it++)
	{
	save_rule[is][it] = (int *) XtMalloc (spread * sizeof (int)) ;
	asave_rule[is][it] = (int *) XtMalloc (spread * sizeof (int)) ;
	}
    }

irule = (int *) XtMalloc (hmany_per_page * sizeof (int)) ;
arule = (int *) XtMalloc (hmany_per_page * sizeof (int)) ;
for (it=0;  it < hmany_per_page;  it++)
	{
	irule[it] = -1 ;
	arule[it] = -1 ;
	}

lw->cell.mutation = (int *) XtCalloc (spread, sizeof (int)) ;
lw->cell.old_many_per_page = many_per_page ;
lw->cell.old_hmany_per_page = hmany_per_page ;

return ;
}

static void
get_order_number (lw)
    XcuCellWidget lw ;
{
if (rrule == -1)
    order_number = -1 ; /* NOP ? */
else
if (rrule == 0)
    order_number = -1 ;
else
    order_number = rrule - 2 ;

return ;
}

static void
get_pick_file (lw)
    XcuCellWidget lw ;
{
if (fpick)
    fclose (fpick) ;

if (*lw->cell.pick_file == '/' || *lw->cell.pick_file == '.')
    fpick = fopen (lw->cell.pick_file, "a") ;
else
    {
    char temp[200] ;
    sprintf (temp, "%s/%s", lw->cell.directory, lw->cell.pick_file) ;
    fpick = fopen (temp, "a") ;
    }

return ;
}

static void
get_mod_file (lw)
    XcuCellWidget lw ;
{
int i, sp ;
FILE *fd ;
char temp[100] ;

sprintf (temp, "%s/Mods/%d_%d", lw->cell.directory, reduct, spread) ;
fd = fopen (temp, "r") ;
if (fd == 0)
    {
    fprintf (stderr, "Trouble Opening Mod File (%s)\n", temp) ;
    exit (1) ;
    }

if (mask)
    {
    for (i=0;  i < masks;  i++)
	XtFree (mask[i]) ;
    XtFree (mask) ;
    }

fscanf (fd, "%d%d%d", &masks, &lw->cell.mod_hmany_per_page,
			      &lw->cell.mod_many_per_page) ;
mask = (int **) XtMalloc (masks * sizeof (int *)) ;
for (i=0;  i < masks;  i++)
	{
	mask[i] = (int *) XtMalloc (spread * sizeof (int)) ;
	for (sp=0;  sp < spread;  sp++)
		{
		int num = fscanf (fd, "%d", &mask[i][sp]) ;
		if (num < 1)
			break ;
		}
	if (sp < spread)
		break ;
	}
if (i < masks)
	{
	fprintf (stderr, "Insufficient mask data found\n") ;
	exit (1) ;
	}
fclose (fd) ;
return ;
}

static void
get_seed (lw)
    XcuCellWidget lw ;
{
if (lw->cell.seed_ != 317317317)
    srand (lw->cell.seed_) ;
else
    {
    struct timeval tp_, *tp = &tp_ ;
    struct timezone tzp_, *tzp = &tzp_ ;
    gettimeofday (tp, tzp) ;
    srand (tp->tv_sec) ;
    }
return ;
}

static void
get_rules (lw)
    XcuCellWidget lw ;
{
FILE *fd ;
int i, sp ;

if (rule && lw->cell.rules_file)
    {
    for (i=0;  i < lw->cell.rule_count;  i++)
	XtFree (rule[i]) ;
    XtFree (rule) ;
    }

if (lw->cell.rules_file &&
    *lw->cell.rules_file &&
    strcmp(lw->cell.rules_file, "random"))
    {
    char temp[200] ;
    fd = fopen (lw->cell.rules_file, "r") ;
    if (!fd)
	{
	if (*lw->cell.rules_file != '.' &&
	    *lw->cell.rules_file != '/')
	    {
	    sprintf (temp, "%s/%s", lw->cell.directory, lw->cell.rules_file) ;
	    }

	fd = fopen (temp, "r") ;
	if (!fd)
	    {
	    fprintf (stderr, "Trouble opening rule file: (%s)\n",
		     lw->cell.rules_file) ;
	    exit (1) ;
	    }
	}
    /*
     * Once through the file to find the number of rules...
     */

    for (nrule=0; ; )
	{
	for (sp=0;  sp < spread;  sp++)
	    {
	    int dummy ;
	    int num = fscanf (fd, "%d", &dummy) ;
	    if (num < 1)
		    break ;
	    }
	if (sp < spread)
	    break ;
	nrule++ ;
	}

    lw->cell.rule_count = nrule + (lw->cell.masks_active ? hmany_per_page : 0);
    rule = (int **) XtMalloc (lw->cell.rule_count * sizeof (int *)) ;
    fseek (fd, 0l, 0) ;

    for (i=0; i < nrule;  i++)
	{
	rule[i] = (int *) XtMalloc (spread * sizeof (int)) ;
	for (sp=0;  sp < spread;  sp++)
	    {
	    fscanf (fd, "%d", &rule[i][sp]) ;
	    }
	}
    if (lw->cell.masks_active)
	{
	int h ;
	for (h=0;  h < hmany_per_page;  h++)
	    {
	    rule[nrule+h] = (int *) XtMalloc (spread * sizeof (int)) ;
	    }
	}

    allrandom = False ;
    }
else
if (lw->cell.rules_file)
    {
    /* Null String implies random rules (also "random") */
    rule = (int **) XtMalloc (hmany_per_page * sizeof (int *)) ;
    for (i=0;  i < hmany_per_page;  i++)
	rule[i] = (int *) XtMalloc (spread * sizeof (int)) ;
    allrandom = True ;
    lw->cell.rule_count = hmany_per_page ;
    lw->cell.masks_active = False ;
    }
return ;
}

static void
get_counts (lw)
    XcuCellWidget lw ;
{
many_count = lw->cell.height / many_per_page ;
hmany_count = lw->cell.width / hmany_per_page ;

return ;
}

static void
get_wraps (lw)
    XcuCellWidget lw ;
{
int i, h, w ;
wrap_left = (unsigned char **)
	    XtMalloc (hmany_per_page * sizeof (unsigned char *)) ;
for (i=0;  i < hmany_per_page;  i++)
    wrap_left[i] = (unsigned char *)
		   XtMalloc (windo/2 * sizeof (unsigned char)) ;

wrap_right = (unsigned char **)
	    XtMalloc (hmany_per_page * sizeof (unsigned char *)) ;
for (i=0;  i < hmany_per_page;  i++)
    wrap_right[i] = (unsigned char *)
		   XtMalloc (windo/2 * sizeof (unsigned char)) ;

for (h=0;  h < hmany_per_page;  h++)
    {
    for (w=0; w < windo/2;  w++)
	{
	if (!hmerge)
	    {
	    wrap_left[h][w] = (h+1) * hmany_count - w - 1 ;
	    wrap_right[h][w] = h * hmany_count + w ;
	    }
	else
	    {
	    if (h == 0)
		wrap_left[h][w] = h * hmany_count - w - 1 ;
	    else
		wrap_left[h][w] = hmany_per_page * hmany_count - w - 1 ;
	    if (h == hmany_per_page-1)
		wrap_right[h][w] = w ;
	    else
		wrap_right[h][w] = (h+1) * hmany_count + w ;
	    }
	}
    }
return ;
}

static void
get_ignores (lw)
    XcuCellWidget lw ;
{
/*
 * Convert String To Ignores
 */
Cardinal	i ;
String	*words ;

if (lw->cell.ignore_array)
    {
    }

if (lw->cell.ignore_colors)
    {
    if (*lw->cell.ignore_colors)
	{
	XcuConvertStringToWords (lw->cell.ignore_colors,
				 &words, &lw->cell.ignore_count) ;
	lw->cell.ignore_array = (int *) XtMalloc (lw->cell.ignore_count *
						    sizeof(int)) ;
	for (i=0;  i < lw->cell.ignore_count;  i++)
	    {
	    lw->cell.ignore_array[i] = atoi(words[i]) ;
	    }
	lw->cell.ignore_colors = (String) NULL ;
	}
    else
	lw->cell.ignore_count = 0 ;
    }

return ;
}

static void
get_size_arrays (lw)
    XcuCellWidget lw ;
{
if (lw->cell.image->data)
    {
    XtFree(lw->cell.image->data) ;
    }
if (row0)
    {
    XtFree(row0) ;
    }

if (row1)
    {
    XtFree(row1) ;
    }

lw->cell.image->data = XtMalloc((unsigned) lw->cell.width * sizeof(char)) ;
row0 = (unsigned char *) XtMalloc((unsigned) lw->cell.width * sizeof(char)) ;
row1 = (unsigned char *) XtMalloc((unsigned) lw->cell.width * sizeof(char)) ;

lw->cell.image->width = lw->cell.width ;
lw->cell.image->bytes_per_line = lw->cell.width ;

return ;
}

static
void get_rcells (lw)
    XcuCellWidget lw ;
{
int i, h ;
if (lw->cell.rcells)
    {
    for (i=0;  i < lw->cell.old_rcell_hmany;  i++)
	XtFree (lw->cell.rcells[i]) ;
    XtFree (lw->cell.rcells) ;
    }
lw->cell.old_rcell_hmany = hmany_per_page ;
lw->cell.rcells = (Pixel **) XtMalloc (hmany_per_page * sizeof (Pixel *)) ;
for (h=0;  h < hmany_per_page;  h++)
    {
    lw->cell.rcells[h] = (Pixel *) XtMalloc (reduct * sizeof (Pixel)) ;
    for (i=0;  i < reduct;  i++)
	{
	lw->cell.rcells[h][i] = lw->cell.cells[i] ;
	}
    }

return ;
}

static
void get_colors (lw)
    XcuCellWidget lw ;
{
int i, h ;

    if (lw->cell.cells)
	{
	XtFree (lw->cell.cells) ;
	XFreeColors (XtDisplay(lw),
		    DefaultColormap(XtDisplay(lw),DefaultScreen(XtDisplay(lw))),
		    lw->cell.cells,
		    lw->cell.old_n_colors,
		    NULL) ;
	}

    lw->cell.old_n_colors = lw->cell.n_colors ;
    lw->cell.cells = (Pixel *) XtMalloc (lw->cell.n_colors * sizeof (Pixel)) ;

    /*
     * First allocate as many writable colormap entries as we can get.
     */

    for (lw->cell.max_rw_cells=0;
	 lw->cell.max_rw_cells < lw->cell.n_colors;
	 lw->cell.max_rw_cells++)
	{
	if (!XAllocColorCells (XtDisplay(lw),
	       DefaultColormap(XtDisplay(lw),DefaultScreen(XtDisplay(lw))),
	       True,
	       NULL, 0,
	       &lw->cell.cells[lw->cell.max_rw_cells], 1))
	    break ;
	}

for (i=0;  i < lw->cell.max_rw_cells;  i++)
    {
    lw->cell.colors[i].pixel = lw->cell.cells[i] ;
    lw->cell.colors[i].flags = DoRed | DoGreen | DoBlue ;
    }

if (lw->cell.max_rw_cells)
XStoreColors  (XtDisplay(lw),
	      DefaultColormap(XtDisplay(lw),DefaultScreen(XtDisplay(lw))),
	      lw->cell.colors, lw->cell.max_rw_cells) ;
/*
 * Now get the remainder as read-only colormap entries
 */

for (i=lw->cell.max_rw_cells;  i < lw->cell.n_colors;  i++)
    {
    if (!XAllocColor (XtDisplay(lw),
		  DefaultColormap(XtDisplay(lw),DefaultScreen(XtDisplay(lw))),
		  &lw->cell.colors[i]))
	break ;
    lw->cell.cells[i] = lw->cell.colors[i].pixel ;
    }


if (i < lw->cell.n_colors)
    {
    fprintf (stderr, "Warning: Unable to allocate enough colormap cells!\n") ;
    }

lw->cell.max_colors = i ;

return ;
}

static
void get_permutations (lw)
    XcuCellWidget lw ;
{
int i ;
int m_taken = reduct ;
Pixel arr[MAX_COLORS_ALLOWED] ;

lw->cell.permutation_index = 0 ;

if (lw->cell.permute_array)
    {
    for (i=0;  i < lw->cell.n_permutations;  i++)
	XtFree (lw->cell.permute_array[i]) ;
    XtFree (lw->cell.permute_array) ;
    }

for (i=0;  i < lw->cell.n_colors;  i++)
    arr[i] = i ;

lw->cell.permute_array = permutations (lw->cell.n_colors,
				       m_taken,
				       arr,
				       &lw->cell.n_permutations) ;
return ;
}

void
XcuCellStep (lw)
    XcuCellWidget lw ;
{
int h ;
unsigned char *temp ;
int *iarule ;

if (!lw->cell.is_exposed)
    return ;

if (row == lw->cell.height)
    {
    row = 0 ;
    mask_iter = 0 ;
    }

if (row == 0)
    {
    if (lw->cell.frequency == XcuPAGE_FREQUENCY)
	XcuCellNextMap (lw, 1) ;
    rule_number = 0 ;
    get_rule (lw, irule, save_rule, NULL) ;
    if (lw->cell.alternating)
	get_rule (lw, arule, asave_rule, irule) ;
    rule_number++ ;
    get_initial_row (lw) ;
    put_row (lw, lw->cell.image) ;
    row_count = 1 ;
    row++ ;
    return ;
    }

row_count++ ;
iarule = (!lw->cell.alternating || !(row_count & 1)) ? irule : arule ;

for (h=0;  h < hmany_per_page;  h++)
    {
    if (lw->cell.frequency == XcuLINE_FREQUENCY)
	{
	lw->cell.current_h = h ;
	XcuCellNextMap (lw, 1) ;
	}
    switch (mix)
      {
      case LCR3 :
      case LCR4 :
      case LCR5 :
      case LCR6 :
	{
	lcr (h, hmany_count, lw->cell.masks_active, row0, row1,
	      rule, nrule, wrap_left, wrap_right,
	      iarule, lw->cell.rcells[h], lw->cell.image->data) ;
	break ;
	}
      case LCCR4 :
	{
	lccr (h, hmany_count, lw->cell.masks_active, row0, row1, rule, nrule,
		  wrap_left, wrap_right, iarule, lw->cell.rcells[h],
		  lw->cell.image->data) ;
	break ;
	}
      case LCCRRR4 :
	{
	lccrrr (h, hmany_count, lw->cell.masks_active, row0, row1, rule, nrule,
		  wrap_left, wrap_right, iarule, lw->cell.rcells[h],
		  lw->cell.image->data) ;
	break ;
	}
      case KLCRT3 :
      case KLCRT4 :
      case KLCRT5 :
	{
	klcrt (h, hmany_count, lw->cell.masks_active, row0, row1, rule, nrule,
		  wrap_left, wrap_right, iarule, lw->cell.rcells[h],
		  lw->cell.image->data) ;
	break ;
	}
      }
    }
put_row (lw, lw->cell.image) ;

temp = row0 ;
row0 = row1 ;
row1 = temp ;

if (row > 1 && many_per_page > 1)
    {
    if (row_count > many_count && row < lw->cell.height - many_count/2)
	{
	row_count = 1 ;
	get_initial_row (lw) ;
	get_rule (lw, irule, save_rule, 0) ;
	if (lw->cell.alternating)
	    get_rule (lw, arule, asave_rule, irule) ;
	rule_number++ ;
	}
    }

row++; 
/*** FUTURE RESOURCE
if (row == lw->cell.height)
    {
    if (sleep_seconds)
	sleep (sleep_seconds) ;
    }
***/
return ;
}

static void
Pick (w,event,params,num_params)
     Widget w;
     XButtonEvent *event;
     String *params;		/* unused */
     Cardinal *num_params;	/* unused */
{
XcuCellWidget lw = (XcuCellWidget) w ;
Dimension facet_sub = (lw->simple.shadow ? 0 : 1) * lw->simple.facet_width ;
int dcx, dcy ;
int picked_rule ;
int hpicked_rule ;

dcx = event->x - lw->cell.internal_width - facet_sub ;
dcy = event->y - lw->cell.internal_height - facet_sub ;
picked_rule = dcy / many_count ;
hpicked_rule = dcx / hmany_count ;

/* TODO: Some sort of acknowledging flash? */
if (lw->cell.beep)
    fprintf (stderr, "\007") ;

if (!fpick)
    {
    char tmp[20] ;
    sprintf (tmp, "/tmp/cell%d", getpid()) ;
    fprintf (stderr, "No pick file specified! Opening %s for output.\n", tmp) ;
    fpick = fopen (tmp, "a") ;
    }
p_rule (fpick, "", save_rule[picked_rule][hpicked_rule], spread) ;
if (lw->cell.alternating)
    p_rule (fpick, " ", asave_rule[picked_rule][hpicked_rule], spread) ;
fflush (fpick) ;
return ;
}

static void
get_initial_row (lw)
    XcuCellWidget lw ;
{
int i, h ;
/* for now only random or merge initialization */
/* (We may want to add initialization by a defined pattern) */

if (
    lw->cell.first_initial_row || !vmerge
   )
    {
    for (i=0;  i < lw->cell.width;  i++)
	{
	switch (reduct)
	    {
	    case 3 : { row0[i] = rand3() ; break ; }
	    case 4 : { row0[i] = rand4() ; break ; }
	    case 5 : { row0[i] = rand5() ; break ; }
	    case 6 : { row0[i] = rand6() ; break ; }
	    }
	}
    lw->cell.first_initial_row = False ;
    }
else
    {
    /* we may reinitialize if the row has come out all equal */
    for (h=0;  h < hmany_per_page;  h++)
	{
	int c = h*hmany_count ;
	for (i=1;  i < hmany_count;  i++)
	    {
	    if (row0[c+i] != row0[c])
		{
		break ;
		}
	    }
	if (i == hmany_count)
	    {
	    for (i=0;  i < hmany_count;  i++)
		{
		switch (reduct)
		    {
		    case 3 : { row0[c+i] = rand3() ; break ; }
		    case 4 : { row0[c+i] = rand4() ; break ; }
		    case 5 : { row0[c+i] = rand5() ; break ; }
		    case 6 : { row0[c+i] = rand6() ; break ; }
		    }
		}
	    }
	}
    }
return ;
}

static void
get_rule (lw, iarule, iasave_rule, real_irule)
    XcuCellWidget lw ;
    int *iarule ;
    int ***iasave_rule ;
    int *real_irule ;
{
int i, h, rule_cnt ;

for (h=0;  h < hmany_per_page;  h++)
  {
  do
    {
    if (mask_iter >= masks)
	mask_iter -= masks ;
    if (lw->cell.mutate)
	{
	int mutate_location = rand() % spread ;
	int mutate_value = rand3() + 1 ;
	lw->cell.mutation[mutate_location] += mutate_value ;
	lw->cell.mutation[mutate_location] %= 4 ;
	iarule[h] = h ;
	for (i=0;  i < spread;  i++)
	    rule[h][i] = lw->cell.mutation[i] ;
	}
    else
    if (allrandom)
	{
	iarule[h] = h ;
	for (i=0;  i < spread;  i++)
	    {
	    int val ;
	    for (;;)
		{
		switch (reduct)
		    {
		    case 3 : { val = rand3() ; break ; }
		    case 4 : { val = rand4() ; break ; }
		    case 5 : { val = rand5() ; break ; }
		    case 6 : { val = rand6() ; break ; }
		    }
		if (lw->cell.ignore_count)
		    {
		    int j ;
		    for (j=0;  j < lw->cell.ignore_count;  j++)
			if (lw->cell.ignore_array[j] == val)
			    break ;
		    if (j == lw->cell.ignore_count)
			break ;
		    }
		else
		    break ;
		}
	    rule[h][i] = val ;
	    }
	}
    else
    if (order || lw->cell.masks_active)
	{
	if (!lw->cell.masks_active || mask_iter == 0)
	    {
	    order_number++ ;
	    if (order_number == nrule)
		order_number = 0 ;
	    iarule[h] = order_number ;
	    }
	if (lw->cell.masks_active)
	    {
	    int r ;
	    for (r=0;  r < spread;  r++)
		rule[nrule+h][r] = rule[iarule[0]][r] ;
	    }
	}
    else
    if (rrule == -1) /* random rule number */
	{
	iarule[h] = (rand() % nrule) ; 
	if (iarule[h] == nrule) iarule[h] = 0 ;
	}
    else  /* use the specified rule */
	{
	iarule[h] = rrule - 1 ;
	}

    if (lw->cell.masks_active)
	{
	for (i=0;  i < spread;  i++)
	    {
	    rule[nrule+h][i] += mask[mask_iter][i] ;
	    if (rule[nrule+h][i] >= reduct)
		rule[nrule+h][i] -= reduct ;
	    }

	mask_iter++ ;
	if (mask_iter > masks)
	    mask_iter = 0 ;
	}

    for (rule_cnt=0;  rule_cnt < spread;  rule_cnt++)
	{
	if (lw->cell.masks_active)
	    iasave_rule[rule_number][h][rule_cnt] = rule[nrule+h][rule_cnt] ;
	else
	    {
	    iasave_rule[rule_number][h][rule_cnt] = rule[iarule[h]][rule_cnt] ;
	    }
	}
    if (lw->cell.frequency == XcuRULE_FREQUENCY)
	{
	lw->cell.current_h = h ;
	XcuCellNextMap (lw, 1) ;
	}
    } while (hmany_per_page > 1 && real_irule && iarule[h] == real_irule[h]) ;
      /* This avoids getting the same rule for both rules when alternating */
  }

return ;
}

static void
p_rule (fd, pre_text, _rule, _spread)
    FILE *fd ;
    char *pre_text ;
    int *_rule ;
    int _spread ;
{
int i, j ;
int m3 = _spread/3*3 ;
int orphans = _spread - m3 ;
int groups, ptr ;

fprintf (fd, "%s", pre_text) ;
for (i=0;  i < orphans;  i++)
	fprintf (fd, "%d ", _rule[i]) ;

groups = m3 / 3 ;
ptr = orphans ;
for (i=0;  i < groups;  i++)
	{
	fprintf (fd, " ") ;
	for (j=0;  j < 3;  j++)
		{
		fprintf (fd, "%d ", _rule[ptr]) ;
		ptr++ ;
		}
	}
fprintf (fd, "\n") ;
return ;
}

static void
put_row (lw, image)
    XcuCellWidget lw ;
    XImage *image ;
{
Dimension facet_left = (lw->simple.shadow) ?
			0 : lw->simple.facet_width ;
XPutImage (XtDisplay(lw), XtWindow(lw), lw->cell.cell_GC,
	   image, 0, 0,
	   facet_left + lw->cell.internal_width,
	   facet_left + lw->cell.internal_height + row,
	   lw->cell.width, 1) ;
return ;
}

static void
do_gets (lw)
    XcuCellWidget lw ;
{
if (lw->cell.alt_changed)
    {
    lw->cell.first_initial_row = True ;
    row = 0 ;
    }

if (lw->cell.type_changed)
    {
    int old_spread = spread ;
    int old_windo = windo ;
    int old_reduct = reduct ;
    get_rule_type	(lw) ;
    if (spread != old_spread)
	lw->cell.spread_changed = True ;
    if (windo != old_windo)
	lw->cell.windo_changed = True ;
    if (reduct != old_reduct)
	lw->cell.reduct_changed = True ;
    row = 0 ;
    lw->cell.first_initial_row = True ;
    mask_iter = 0 ;
    }

if (lw->cell.mod_changed)
    {
    int temp ;

    temp = hmany_per_page ;
    hmany_per_page = lw->cell.mod_hmany_per_page ;
    lw->cell.mod_hmany_per_page = temp ;

    temp = many_per_page ;
    many_per_page = lw->cell.mod_many_per_page ;
    lw->cell.mod_many_per_page = temp ;

    lw->cell.many_changed = True ;
    lw->cell.hmany_changed = True ;

    row = 0 ;
    lw->cell.first_initial_row = True ;
    mask_iter = 0 ;
    }

if (lw->cell.hmany_changed || lw->cell.many_changed || lw->cell.spread_changed)
    {
    get_save_rule	(lw) ;
    }

if (lw->cell.rrule_changed)
    {
    get_order_number(lw) ;
    row = 0 ;
    lw->cell.first_initial_row = True ;
    mask_iter = 0 ;
    }

if (lw->cell.pick_changed)
    {
    get_pick_file	(lw) ;
    }

if (lw->cell.seed_changed)
    {
    get_seed	(lw) ;
    row = 0 ;
    lw->cell.first_initial_row = True ;
    mask_iter = 0 ;
    }

if (lw->cell.rules_changed || lw->cell.mod_changed ||
    lw->cell.spread_changed || lw->cell.hmany_changed)
    {
    get_rules	(lw) ;
    row = 0 ;
    lw->cell.first_initial_row = True ;
    mask_iter = 0 ;
    }

if (lw->cell.width_changed || lw->cell.height_changed ||
    lw->cell.many_changed  || lw->cell.hmany_changed)
    {
    get_counts	(lw) ;
    lw->cell.hmany_changed = True ;
    row = 0 ;
    lw->cell.first_initial_row = True ;
    mask_iter = 0 ;
    }

if (lw->cell.windo_changed || lw->cell.hmerge_changed ||
    lw->cell.hmany_changed)
    {
    get_wraps	(lw) ;
    row = 0 ;
    lw->cell.first_initial_row = True ;
    mask_iter = 0 ;
    }

if (lw->cell.ignore_colors_changed)
    {
    get_ignores	(lw) ;
    }

if (lw->cell.width_changed || lw->cell.height_changed)
    {
    get_size_arrays	(lw) ;
    row = 0 ;
    lw->cell.first_initial_row = True ;
    mask_iter = 0 ;
    }

if (lw->cell.colors_changed || lw->cell.n_colors_changed)
    {
    get_colors	(lw) ;
    lw->cell.hmany_changed = True ; /* Hack to get get_rcells() called */
    }

if (lw->cell.reduct_changed || lw->cell.hmany_changed)
    {
    get_rcells	(lw) ;
    }

if (lw->cell.reduct_changed || lw->cell.n_colors_changed)
    {
    get_permutations (lw) ;
    }

set_changed_false (lw) ;
return ;
}

static void
set_changed_false (lw)
    XcuCellWidget lw ;
{
lw->cell.height_changed = False ;
lw->cell.hmerge_changed = False ;
lw->cell.hmany_changed = False ;
lw->cell.ignore_colors_changed = False ;
lw->cell.many_changed = False ;
lw->cell.mod_changed = False ;
lw->cell.pick_changed = False ;
lw->cell.rrule_changed = False ;
lw->cell.rules_changed = False ;
lw->cell.seed_changed = False ;
lw->cell.spread_changed = False ;
lw->cell.type_changed = False ;
lw->cell.width_changed = False ;
lw->cell.windo_changed = False ;
lw->cell.colors_changed = False ;
lw->cell.n_colors_changed = False ;
lw->cell.reduct_changed = False ;
lw->cell.alt_changed = False ;
return ;
}

static void
set_changed_true (lw)
    XcuCellWidget lw ;
{
lw->cell.height_changed = True ;
lw->cell.hmerge_changed = True ;
lw->cell.hmany_changed = True ;
lw->cell.ignore_colors_changed = True ;
lw->cell.many_changed = True ;
lw->cell.pick_changed = True ;
lw->cell.rrule_changed = True ;
lw->cell.rules_changed = True ;
lw->cell.seed_changed = True ;
lw->cell.spread_changed = True ;
lw->cell.type_changed = True ;
lw->cell.width_changed = True ;
lw->cell.windo_changed = True ;
lw->cell.colors_changed = True ;
lw->cell.n_colors_changed = True ;
lw->cell.reduct_changed = True ;
lw->cell.alt_changed = True ;
return ;
}

void
XcuCellFirstMap (lw)
    XcuCellWidget lw ;
{
lw->cell.permutation_index = 0 ;
permute_colormap (lw) ;
return ;
}

void
XcuCellNextMap (lw, direction)
    XcuCellWidget lw ;
    int direction ;
{
switch (lw->cell.sequence)
  {
  case XcuORDERED_PERMUTE :
    {
    if (direction >= 0)
	{
	lw->cell.permutation_index++ ;
	if (lw->cell.permutation_index == lw->cell.n_permutations)
	    lw->cell.permutation_index = 0 ;
	}
    else
	{
	lw->cell.permutation_index-- ;
	if (lw->cell.permutation_index < 0)
	    lw->cell.permutation_index = lw->cell.n_permutations - 1 ;
	}
    permute_colormap (lw) ;
    break ;
    }
  case XcuRANDOM_PERMUTE :
    {
    lw->cell.permutation_index = (rand() % lw->cell.n_permutations) ;
    permute_colormap (lw) ;
    break ;
    }
  case XcuRANDOM_COLOR :
    {
    int i ;
    XColor color[MAX_COLORS_ALLOWED] ;
    if (lw->cell.max_rw_cells >= reduct &&
	lw->cell.frequency != XcuLINE_FREQUENCY &&
	(lw->cell.frequency != XcuRULE_FREQUENCY || hmany_per_page == 1))
	{
	/*
	 * Alter the colors by modifying the colormap
	 */
	for (i=0;  i < reduct;  i++)
	    {
	    color[i].flags = DoRed|DoGreen|DoBlue ;
	    color[i].pixel = lw->cell.colors[i].pixel ;
	    color[i].red = rndm(65535L,-1) ;
	    color[i].green = rndm(65535L,-1) ;
	    color[i].blue = rndm(65535L,-1) ;
	    }

	XStoreColors  (XtDisplay(lw),
		  DefaultColormap(XtDisplay(lw),DefaultScreen(XtDisplay(lw))),
		  color, reduct) ;
	}
    else
	{
	/*
	 * Can't do it!
	 */
	}
    break ;
    }
  }
return ;
}

static void
permute_colormap (lw)
    XcuCellWidget lw ;
{
static XColor *temp = NULL ;
static int max_temp = 0 ;
int i ;
if (reduct > max_temp)
    {
    if (temp)
	XtFree (temp) ;
    temp = (XColor *) XtMalloc (reduct * sizeof (XColor)) ;
    }
/*
 * We'll change the colormap if we have enough writable cells,
 * o.w., we can only change future colors by altering the pixels written
 * But we have to alter pixels written if the coloration is by rule or line
 * (unless there is only one rule per row and then we can change the colormap)
 */
if (lw->cell.max_rw_cells >= reduct &&
    lw->cell.frequency != XcuLINE_FREQUENCY &&
    (lw->cell.frequency != XcuRULE_FREQUENCY || hmany_per_page == 1))
    {
    /*
     * Alter the colors by modifying the colormap
     */
    for (i=0;  i < reduct;  i++)
	{
	int index = lw->cell.permute_array[lw->cell.permutation_index][i] ;
	temp[i].flags = lw->cell.colors[i].flags ;
	temp[i].pixel = lw->cell.colors[i].pixel ;
	temp[i].red = lw->cell.colors[index].red ;
	temp[i].green = lw->cell.colors[index].green ;
	temp[i].blue = lw->cell.colors[index].blue ;
	}

    XStoreColors  (XtDisplay(lw),
		  DefaultColormap(XtDisplay(lw),DefaultScreen(XtDisplay(lw))),
		  temp, reduct) ;
    }
else
    {
    /*
     * Alter the colors by modifying the pixels written instead of modifying
     * the colormap
     */
    for (i=0;  i < reduct;  i++)
	{
	int index = lw->cell.permute_array[lw->cell.permutation_index][i] ;
	lw->cell.rcells[lw->cell.current_h][i] = lw->cell.cells[index] ;
	}
    }
return ;
}

static int **
permutations (n_items, m_taken, items, n_permutations)
    int n_items ;
    int m_taken ;
    Pixel *items ;
    int *n_permutations ;
{
int i ;
int n_found ;
static Pixel **array ;
Pixel *temp ;
/*
 * given n items, taken m at a time, this function...
 * computes the number of permutations,
 * allocates an array of pointers to the permutations,
 * and computes the permutations.
 */

if (n_items < m_taken)
    {
    int n=1 ;

    for (i=0;  i < m_taken;  i++)
	n *= n_items ;
    n -= n_items ; /* skip cases where all the same color */
    *n_permutations = n ;

    array = (Pixel **) XtMalloc (*n_permutations * sizeof (Pixel *)) ;
    for (i=0;  i < n;  i++)
	array[i] = (Pixel *) XtMalloc (m_taken * sizeof(Pixel)) ;
    for (i=0,n=0;  n < *n_permutations;  i++)
	{
	int index = i ;
	int k ;
	for (k=0;  k < m_taken;  k++)
	    {
	    array[n][k] = index%n_items ;
	    index /= n_items ;
	    }
	for (k=1;  k < m_taken;  k++)
	    if (array[n][k] != array[n][0])
		break ;
	if (k != m_taken)
	    {
	    n++ ;
	    }
	}

    return array ;
    }
*n_permutations = n_by_m_perm (n_items, m_taken) ;

array = (Pixel **) XtMalloc (*n_permutations * sizeof (Pixel *)) ;
for (i=0;  i < *n_permutations;  i++)
	{
	array[i] = (Pixel *) XtMalloc (m_taken * sizeof (Pixel)) ;
	}

temp = (Pixel *) XtMalloc (m_taken * sizeof(Pixel)) ;
n_found = 0 ;
permute (m_taken, 0, n_items, items, array, temp, &n_found) ;
XtFree (temp) ;

return array ;
}

static void
permute (n_levels, i_level, n_elements, elements,
	 perm_array, temp_array, n_found)
    int n_levels ;
    int i_level ;
    int n_elements ;
    Pixel *elements ;
    Pixel **perm_array ;
    int *temp_array ;
    int *n_found ;
{
int	i, j ;

for (i=0;  i < n_elements;  i++)
    {
    temp_array[i_level] = elements[i] ;
    if (i_level == n_levels - 1)
	{
	for (j=0;  j < n_levels;  j++)
	    {
	    perm_array[*n_found][j] = temp_array[j] ;
	    }
	*n_found += 1 ;
	}
    else
	{
	Pixel *pass_array = (Pixel *) XtMalloc((n_elements-1) * sizeof(Pixel));
	for (j=0;  j < i;  j++)
	    {
	    pass_array[j] = elements[j] ;
	    }
	for (j=i+1;  j < n_elements;  j++)
	    {
	    pass_array[j-1] = elements[j] ;
	    }
	permute (n_levels, i_level+1, n_elements-1,
		 pass_array, perm_array, temp_array, n_found) ;
	XtFree ((char *)pass_array) ;
	}
    }
return ;
}

static int
n_by_m_perm (n, m)
    int n ;
    int m ;
{
return factorial (n) / factorial (n-m) ;
}

static int
factorial (n)
    int n ;
{
if (n == 0)
	return 1 ;
else
	return n * factorial (n-1) ;
}

/**
 *** Resource Converter Routine
 **/

/*
 * Convert String To Frequency
 */

static void
CvtStringToFrequency(args, num_args, fromVal, toVal)
    XrmValuePtr *args ;		/* unused */
    Cardinal	*num_args ;	/* unused */
    XrmValuePtr fromVal ;
    XrmValuePtr toVal ;
{
static XcuCellFrequencyType	e ; /* must be static! */
XrmQuark	q ;
char	*s = (char *) fromVal->addr ;

if (s == NULL) return ;

q = XcuSimplifyArg (s, "xcu") ;

toVal->size = sizeof(XcuCellFrequencyType) ;
toVal->addr = (XPointer) &e ;

if (q == XrmQEfixed)   { e = XcuFIXED_FREQUENCY;   return; }
if (q == XrmQEpage) { e = XcuPAGE_FREQUENCY; return; }
if (q == XrmQErule)  { e = XcuRULE_FREQUENCY;  return; }
if (q == XrmQEline)  { e = XcuLINE_FREQUENCY;  return; }

toVal->size = 0 ;
toVal->addr = NULL ;
return ;
}

/*
 * Convert String To Sequence
 */

static void
CvtStringToSequence(args, num_args, fromVal, toVal)
    XrmValuePtr *args ;		/* unused */
    Cardinal	*num_args ;	/* unused */
    XrmValuePtr fromVal ;
    XrmValuePtr toVal ;
{
static XcuCellSequenceType	e ; /* must be static! */
XrmQuark	q ;
char	*s = (char *) fromVal->addr ;

if (s == NULL) return ;

q = XcuSimplifyArg (s, "xcu") ;

toVal->size = sizeof(XcuCellSequenceType) ;
toVal->addr = (XPointer) &e ;

if (q == XrmQEordered)   { e = XcuORDERED_PERMUTE;   return; }
if (q == XrmQErandomp) { e = XcuRANDOM_PERMUTE; return; }
if (q == XrmQErandomc)  { e = XcuRANDOM_COLOR;  return; }

toVal->size = 0 ;
toVal->addr = NULL ;
return ;
}

static long
rndm(maxval)
    long maxval;
{
  long lrand48();
  int sr1=15, sr2=16;
  long mv=maxval;
  while (mv > 0x8000L) {
    sr1++;
    sr2--;
    mv >>= 1;
  }
return ((lrand48() >> sr1) * maxval) >> sr2;
}

