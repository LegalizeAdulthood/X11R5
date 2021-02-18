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
 * XcuBmgr.c - XcuBmgr widget
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xcu/BmgrP.h>
#include <stdio.h>

/*
 * Full class record constant
 */

/* Private Data */

/*
 * XtResource :
 *	name, class, type, size,
 *	offset, default_type, default_address
 */

static Boolean	def_true = True ;
static XcuBmgrType def_type = XcuBMGR_ANY_OF_MANY ;

/*
 * resource declarations
 */

#define offset(field) XtOffset(XcuBmgrWidget, field)
#define Offset(field) XtOffset(XcuBmgrWidget, bmgr.field)

static XtResource resources[] =
{
     {XtNbmgrType,	  XtCXcuBmgrType,  XtRXcuBmgrType, sizeof(XcuBmgrType),
      Offset(bmgr_type),  XtRXcuBmgrType,  (XPointer) &def_type}

    ,{XtNdownAction,	  XtCBoolean,  XtRBoolean, sizeof(Boolean),
      Offset(down_action),XtRBoolean,  (XPointer) &def_true}

    ,{XtNsetCallback, XtCCallback, XtRCallback, sizeof(XPointer), 
      Offset(set_callbacks), XtRCallback, (XPointer)NULL}

    ,{XtNunsetCallback, XtCCallback, XtRCallback, sizeof(XPointer), 
      Offset(unset_callbacks), XtRCallback, (XPointer)NULL}

    ,{XtNillegalCallback, XtCCallback, XtRCallback, sizeof(XPointer), 
      Offset(illegal_callbacks), XtRCallback, (XPointer)NULL}

    /*** intentionally undocumented ! Probably used only by XcuWlm ***/
    ,{XtNprocedureList, XtCProcedureList,
      XtRProcedureList, sizeof(XtProcedureList *),
      Offset(procedure_list), XtRProcedureList, (XPointer)NULL}
} ;

#undef Offset
#undef offset

static void ClassInitialize() ;
static void Initialize() ;
static void Destroy() ;

static void Notification () ;
static void Notify () ;

#define superclass (&widgetClassRec)

XcuBmgrClassRec xcuBmgrClassRec =
{
  {
  /* core_class fields */	
    /* superclass	  	*/	(WidgetClass) superclass,
    /* class_name	  	*/	"Bmgr",
    /* widget_size	  	*/	sizeof(XcuBmgrRec),
    /* class_initialize   	*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
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
    /* resize		  	*/	NULL,
    /* expose		  	*/	NULL,
    /* set_values	  	*/	NULL,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus	 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* tm_table		   	*/	NULL,
    /* query_geometry		*/	NULL,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL,
  }
} ;

/* For use by clients... */
WidgetClass xcuBmgrWidgetClass = (WidgetClass) &xcuBmgrClassRec ;

/**
 ***
 **** Toolkit Procedures
 ***
 **/

/***** **** *** ** * ClassInitialize * ** *** **** *****/

static void
ClassInitialize()
{
XcuAddStringToBmgrTypeConverter () ;

return ;
}

/***** **** *** ** * Initialize * ** *** **** *****/

static void
Initialize (request, new)
    Widget request, new ;
{
XcuBmgrWidget bw = (XcuBmgrWidget) new ;
static String XcuBmgrSampleText = "Sample" ;
static String XcuBmgrManageText = "Manage" ;
static String XcuBmgrSetAllText = "SetAll" ;
static String XcuBmgrUnsetAllText = "UnsetAll" ;
static String XcuBmgrToggleAllText = "ToggleAll" ;
static String XcuBmgrSetChildText = "SetChild" ;
static String XcuBmgrUnsetChildText = "UnsetChild" ;

bw->bmgr.buttons = NULL ;
bw->bmgr.button_values = NULL ;
bw->bmgr.n_buttons = 0 ;
bw->core.width = 1 ;
bw->core.height = 1 ;
bw->core.x = -3 ;
bw->core.y = -3 ;
bw->bmgr.procedure_list = (XtProcedureList *)
			   XtMalloc (8 * sizeof (XtProcedureList)) ;
bw->bmgr.procedure_list[0].name = XcuBmgrManageText ;
bw->bmgr.procedure_list[0].procedure = XcuBmgrManage ;
bw->bmgr.procedure_list[1].name = XcuBmgrSampleText ;
bw->bmgr.procedure_list[1].procedure = XcuBmgrSample ;
bw->bmgr.procedure_list[2].name = XcuBmgrSetAllText ;
bw->bmgr.procedure_list[2].procedure = XcuBmgrSetAll ;
bw->bmgr.procedure_list[3].name = XcuBmgrUnsetAllText ;
bw->bmgr.procedure_list[3].procedure = XcuBmgrUnsetAll ;
bw->bmgr.procedure_list[4].name = XcuBmgrToggleAllText ;
bw->bmgr.procedure_list[4].procedure = XcuBmgrToggleAll ;
bw->bmgr.procedure_list[5].name = XcuBmgrSetChildText ;
bw->bmgr.procedure_list[5].procedure = XcuBmgrSetChild ;
bw->bmgr.procedure_list[6].name = XcuBmgrUnsetChildText ;
bw->bmgr.procedure_list[6].procedure = XcuBmgrUnsetChild ;
bw->bmgr.procedure_list[7].name = NULL ;
bw->bmgr.procedure_list[7].procedure = NULL ;

return ;
}

/***** **** *** ** * Destroy * ** *** **** *****/

static void
Destroy (widget)
    Widget widget ;
{
XcuBmgrWidget bw = (XcuBmgrWidget) widget ;

/*
 * free dynamically allocated data
 */

if (!XtIsRealized (widget))
    return ;

XtFree ((char *)bw->bmgr.buttons) ;
XtFree ((char *)bw->bmgr.button_values) ;
XtFree ((char *)bw->bmgr.procedure_list) ;

return ;
}

/**
 ***
 **** Private Procedures
 ***
 **/

static void
Notification (w, client_data, child_data)
    Widget w ;
    XPointer client_data ;
    XPointer child_data ;
{
XcuBmgrWidget bmgr = (XcuBmgrWidget) client_data ;
XcuButtonWidget button = (XcuButtonWidget) w ;
if (*child_data == 'u' && !bmgr->bmgr.down_action ||
    *child_data == 'd' &&  bmgr->bmgr.down_action)
    {
    Notify (bmgr, button) ;
    }
return ;
}

static void
Notify (bmgr, button)
    XcuBmgrWidget bmgr ;
    XcuButtonWidget button ;
{
Cardinal i_caller ;
static XPointer callback_data ;

/* just return (with a nasty beep) if this is an unselectable button */
/* TODO: Probably not possible any more */
/***
if (bmgr->bmgr.selectable == False)
    {
    XtWarning ("XcuBmgr is unselectable!!! (???)\n") ;
    XBell (XtDisplay(button), 100) ;
    return ;
    }
***/

/* Find the button # that is notifying */
for (i_caller=0;  i_caller < bmgr->bmgr.n_buttons;  i_caller++)
    if (bmgr->bmgr.buttons[i_caller] == button)
	break ;

/**
if (bmgr->bmgr.help_active)
    {
    }
**/

switch (bmgr->bmgr.bmgr_type)
    {
    case XcuBMGR_TRANSIENT :
	{
	Arg arg ;
	/* Blink the button */
	XtSetArg (arg, XtNset, (XtArgVal) True) ; 
	XtSetValues ((Widget)button, &arg, 1) ;
	XtSetArg (arg, XtNset, (XtArgVal) False) ; 
	XtSetValues ((Widget)button, &arg, 1) ;
	/* send selected event and value */
	callback_data = (XPointer) bmgr->bmgr.button_values[i_caller] ;
	XtCallCallbacks ((Widget)bmgr, XtNsetCallback, callback_data) ;
	break ;
	}
    case XcuBMGR_HELP :
	{
	/* return a special help string */
	/* Let client take care of set/unset instructions */
	break ;
	}
    case XcuBMGR_ANY_OF_MANY :
    case XcuBMGR_SINGLE_TOGGLE :
	{
	Arg arg ;
	if (button->button.set)
	    {
	    XtSetArg (arg, XtNset, (XtArgVal) False) ; 
	    XtSetValues ((Widget)button, &arg, 1) ;
	    /* send unselected event and value */
	    callback_data = (XPointer) bmgr->bmgr.button_values[i_caller] ;
	    XtCallCallbacks ((Widget)bmgr, XtNunsetCallback, callback_data) ;
	    }
	else
	    {
	    XtSetArg (arg, XtNset, (XtArgVal) True) ; 
	    XtSetValues ((Widget)button, &arg, 1) ;
	    /* send selected event and value */
	    callback_data = (XPointer) bmgr->bmgr.button_values[i_caller] ;
	    XtCallCallbacks ((Widget)bmgr, XtNsetCallback, callback_data) ;
	    }
	break ;
	}
    case XcuBMGR_DOUBLE_TOGGLE :
	{
	/* Although it might seem that this is just a degenerate case of
	 * ONE_OF_MANY, toggles allow either the set or unset buttons to
	 * be clicked, whereas ONE_OF_MANY insists on clicking an unset
	 * button
	 */
	XcuButtonWidget calling_button, sibling_button, set_button ;
	Arg calling_arg, sibling_arg  ;

	if (bmgr->bmgr.buttons[0] == button)
	    {
	    calling_button = bmgr->bmgr.buttons[0] ;
	    sibling_button = bmgr->bmgr.buttons[1] ;
	    }
	else
	    {
	    calling_button = bmgr->bmgr.buttons[1] ;
	    sibling_button = bmgr->bmgr.buttons[0] ;
	    }

	if (calling_button->button.set)
	    {
	    XtSetArg (calling_arg, XtNset, (XtArgVal) False) ; 
	    XtSetArg (sibling_arg, XtNset, (XtArgVal) True) ; 
	    set_button = sibling_button ;
	    }
	else
	    {
	    XtSetArg (calling_arg, XtNset, (XtArgVal) True) ; 
	    XtSetArg (sibling_arg, XtNset, (XtArgVal) False) ; 
	    set_button = calling_button ;
	    }

	XtSetValues ((Widget)calling_button, &calling_arg, 1) ;
	XtSetValues ((Widget)sibling_button, &sibling_arg, 1) ;
	i_caller = (set_button == bmgr->bmgr.buttons[1]) ? 1 : 0 ;
	callback_data = (XPointer) bmgr->bmgr.button_values[i_caller] ;
	XtCallCallbacks ((Widget)bmgr, XtNsetCallback, callback_data) ;

	callback_data = (XPointer) bmgr->bmgr.button_values[1 - i_caller] ;
	XtCallCallbacks ((Widget)bmgr, XtNunsetCallback, callback_data) ;
	break ;
	}
    case XcuBMGR_ONE_OF_MANY :
	{
	Cardinal i ;
	Arg arg ;

	/* search for the button that is currently set */
	for (i=0;  i < bmgr->bmgr.n_buttons;  i++)
	    {
	    if (bmgr->bmgr.buttons[i]->button.set)
		break ;
	    }

	if (i < bmgr->bmgr.n_buttons && bmgr->bmgr.buttons[i] == button)
	    {
	    /* Can't select the one button that is selected */
	    callback_data = (XPointer) bmgr->bmgr.button_values[i] ;
	    XtCallCallbacks ((Widget)bmgr, XtNillegalCallback, callback_data) ;
	    /* XBell (XtDisplay(bmgr->bmgr.buttons[i]), 100) ; */
	    return ;
	    }

	if (i < bmgr->bmgr.n_buttons)
	    {
	    XtSetArg (arg, XtNset, (XtArgVal) False) ; 
	    XtSetValues ((Widget)bmgr->bmgr.buttons[i], &arg, 1) ;
	    callback_data = (XPointer) bmgr->bmgr.button_values[i] ;
	    XtCallCallbacks ((Widget)bmgr, XtNunsetCallback, callback_data) ;
	    }

	XtSetArg (arg, XtNset, (XtArgVal) True) ; 
	XtSetValues ((Widget)button, &arg, 1) ;
	callback_data = (XPointer) bmgr->bmgr.button_values[i_caller] ;
	XtCallCallbacks ((Widget)bmgr, XtNsetCallback, callback_data) ;

	break ;
	}
    default :
	{
	break ;
	}
    }
return ;
}

/**
 ***
 **** Public Procedures
 ***
 **/

void
XcuBmgrManage (bmgr, buttons, values, n_buttons)
    XcuBmgrWidget bmgr ;
    XcuButtonWidget *buttons ;
    XPointer *values ;
    Cardinal n_buttons ;
{
Cardinal i ;
if (!XtIsSubclass ((Widget)bmgr, (WidgetClass)xcuBmgrWidgetClass)) {
    XtError
    ("XcuBmgrManage requires 1st arg to be subclass of xcuBmgrWidgetClass") ;
    }
bmgr->bmgr.n_buttons = n_buttons ;
if (bmgr->bmgr.buttons)
    {
    XtFree ((char *)bmgr->bmgr.buttons) ;
    XtFree ((char *)bmgr->bmgr.button_values) ;
    }
bmgr->bmgr.buttons = (XcuButtonWidget *)
		     XtMalloc (n_buttons * sizeof (XcuButtonWidget)) ;
bmgr->bmgr.button_values = (XPointer *)
		     XtMalloc (n_buttons * sizeof (XPointer)) ;
for (i=0;  i < n_buttons;  i++)
    {
    if (!XtIsSubclass ((Widget)buttons[i], (WidgetClass)xcuButtonWidgetClass)) {
	XtError ("XcuBmgrManage requires an array of xcuButtonWidgetClass") ;
	}
    bmgr->bmgr.buttons[i] = buttons[i] ;
    bmgr->bmgr.button_values[i] = values[i] ;
    }

switch (bmgr->bmgr.bmgr_type)
    {
    case (int) XcuBMGR_TRANSIENT :
	{
	Arg arg ;
	XtSetArg (arg, XtNset, False) ;
	for (i=0;  i < bmgr->bmgr.n_buttons;  i++)
	    if (bmgr->bmgr.buttons[i]->button.set)
		XtSetValues ((Widget)bmgr->bmgr.buttons[i], &arg, 1) ;
	break ;
	}
    case (int) XcuBMGR_DOUBLE_TOGGLE :
    case (int) XcuBMGR_ONE_OF_MANY :
	{
	Cardinal n_set = 0 ;
	Cardinal first_set = 0 ;
	char error_text[100] ;
	/* There must be more than one child and one and only one of them set */
	if (bmgr->bmgr.n_buttons < 2)
	    {
	    sprintf (error_text,
	     "XcuBmgrManage/OneOfMany (%s) : must be .gt. 1 button managed\n",
	     bmgr->core.name ? bmgr->core.name : "") ;
	    XtWarning (error_text) ;
	    return ;
	    }
	for (i=0;  i < bmgr->bmgr.n_buttons;  i++)
	    {
	    Boolean set ;
	    set = bmgr->bmgr.buttons[i]->button.set ;
	    if (set)
		{
		if (n_set == 0)
		    first_set = i ;
		n_set++ ;
		}
	    }
	if (n_set < 1)
	    {
	    Arg arg ;
	    XtSetArg (arg, XtNset, True) ;
	    /* We'll set the first one */
	    XtSetValues ((Widget)bmgr->bmgr.buttons[bmgr->bmgr.n_buttons-1],
			  &arg, 1) ;
	    sprintf (error_text,
	     "XcuBmgrManage/OneOfMany (%s) : No buttons set. Setting first.\n",
	     bmgr->core.name ? bmgr->core.name : "") ;
	    XtWarning (error_text) ;
	    }
	else
	if (n_set > 1)
	    {
	    Arg arg ;
	    XtSetArg (arg, XtNset, False) ;
	    /* We'll unset all but the first one set */
	    sprintf (error_text,
	     "XcuBmgrManage/OneOfMany (%s) : Too many set. Unsetting extras.\n",
	     bmgr->core.name ? bmgr->core.name : "") ;
	    XtWarning (error_text) ;
	    for (i=first_set+1;  i < bmgr->bmgr.n_buttons;  i++)
		XtSetValues ((Widget)bmgr->bmgr.buttons[i], &arg, 1) ;
	    }
	break ;
	}
    case (int) XcuBMGR_SINGLE_TOGGLE :
    case (int) XcuBMGR_ANY_OF_MANY :
    case (int) XcuBMGR_HELP :
	{
	break ;
	}
    }

for (i=0;  i < bmgr->bmgr.n_buttons;  i++)
    {
    /* add callbacks for button down and button up */
    XtAddCallback ((Widget)bmgr->bmgr.buttons[i], XtNcallback,
		   Notification, (XPointer) bmgr) ;
    }
return ;
}

/***** **** *** ** * XcuBmgrSample * ** *** **** *****/

void
XcuBmgrSample (bmgr)
    XcuBmgrWidget bmgr ;
{
static XPointer callback_data ;
Cardinal i ;

if (!XtIsSubclass ((Widget)bmgr, (WidgetClass)xcuBmgrWidgetClass)) {
    XtError
    ("XcuBmgrSample requires 1st arg to be subclass of xcuBmgrWidgetClass");
    }
switch (bmgr->bmgr.bmgr_type)
  {
  case XcuBMGR_TRANSIENT : { break ; } /* sampling not meaningful */
  case XcuBMGR_ONE_OF_MANY :
  case XcuBMGR_ANY_OF_MANY :
  case XcuBMGR_SINGLE_TOGGLE :
  case XcuBMGR_DOUBLE_TOGGLE :
    {
    /* for each button, call set or unsetCallback as appropriate */
    for (i=0;  i < bmgr->bmgr.n_buttons;  i++)
	{
	callback_data = (XPointer) bmgr->bmgr.button_values[i] ;
	if (bmgr->bmgr.buttons[i]->button.set)
	    {
	    /* send selected event and value */
	    XtCallCallbacks ((Widget)bmgr, XtNsetCallback, callback_data) ;
	    }
	else
	    {
	    /* send unselected event and value */
	    XtCallCallbacks ((Widget)bmgr, XtNunsetCallback, callback_data) ;
	    }
	}
    break ;
    }
  case (int) XcuBMGR_HELP :
    {
    break ;
    }
  }

return ;
}

/***** **** *** ** * XcuBmgrToggleAll * ** *** **** *****/

/* Toggle the states of DoubleToggle, SingleToggle, and AnyOfMany children */
/* Accomplished by faking a callback from all children */

void
XcuBmgrToggleAll (bmgr)
    XcuBmgrWidget bmgr ;
{
int i ;

if (!XtIsSubclass ((Widget)bmgr, (WidgetClass)xcuBmgrWidgetClass))
 XtError
 ("XcuBmgrToggleAll requires 1st arg to be subclass of xcuBmgrWidgetClass");

if (bmgr->bmgr.bmgr_type != XcuBMGR_DOUBLE_TOGGLE &&
    bmgr->bmgr.bmgr_type != XcuBMGR_SINGLE_TOGGLE &&
    bmgr->bmgr.bmgr_type != XcuBMGR_ANY_OF_MANY)
    {
    char text[200] ;
    sprintf (text, "XcuBmgr(%s)/XcuBmgrSetAll must have a bmgrType of\n%s\n",
	     bmgr->core.name,
	     "XcuBMGR_ANY_OF_MANY, XcuBMGR_SINGLE_TOGGLE, or XcuBMGR_ANY_OF_MANY") ;
    XtWarning (text) ;
    return ;
    }

for (i=0;  i < bmgr->bmgr.n_buttons;  i++)
    {
    Notify (bmgr, bmgr->bmgr.buttons[i]); 
    }
return ;
}

/***** **** *** ** * XcuBmgrSetAll * ** *** **** *****/

/* Change the states of AnyOfMany or SingleToggle children to Set */
/* Accomplished by faking a callback from all unset children */

void
XcuBmgrSetAll (bmgr)
    XcuBmgrWidget bmgr ;
{
int i ;

if (!XtIsSubclass ((Widget)bmgr, (WidgetClass)xcuBmgrWidgetClass))
 XtError
 ("XcuBmgrSetAll requires 1st arg to be subclass of xcuBmgrWidgetClass");

if (bmgr->bmgr.bmgr_type != XcuBMGR_ANY_OF_MANY &&
    bmgr->bmgr.bmgr_type != XcuBMGR_SINGLE_TOGGLE)
    {
    char text[200] ;
    sprintf (text, "XcuBmgr(%s)/XcuBmgrSetAll must have a bmgrType of\n%s\n",
	     bmgr->core.name,
	     "XcuBMGR_ANY_OF_MANY or XcuBMGR_SINGLE_TOGGLE") ;
    XtWarning (text) ;
    return ;
    }

for (i=0;  i < bmgr->bmgr.n_buttons;  i++)
    {
    if (!bmgr->bmgr.buttons[i]->button.set)
	Notify (bmgr, bmgr->bmgr.buttons[i]); 
    }
return ;
}

/***** **** *** ** * XcuBmgrUnsetAll * ** *** **** *****/

/* Change the states of AnyOfMany or SingleToggle children to Unset */
/* Accomplished by faking a callback from all set children */

void
XcuBmgrUnsetAll (bmgr)
    XcuBmgrWidget bmgr ;
{
int i ;

if (!XtIsSubclass ((Widget)bmgr, (WidgetClass)xcuBmgrWidgetClass))
 XtError
 ("XcuBmgrUnsetAll requires 1st arg to be subclass of xcuBmgrWidgetClass");

if (bmgr->bmgr.bmgr_type != XcuBMGR_ANY_OF_MANY &&
    bmgr->bmgr.bmgr_type != XcuBMGR_SINGLE_TOGGLE)
    {
    char text[200] ;
    sprintf (text, "XcuBmgr(%s)/XcuBmgrUnsetAll must have a bmgrType of\n%s\n",
	     bmgr->core.name,
	     "XcuBMGR_ANY_OF_MANY or XcuBMGR_SINGLE_TOGGLE") ;
    XtWarning (text) ;
    return ;
    }

for (i=0;  i < bmgr->bmgr.n_buttons;  i++)
    {
    if (bmgr->bmgr.buttons[i]->button.set)
	Notify (bmgr, bmgr->bmgr.buttons[i]); 
    }
return ;
}

/***** **** *** ** * XcuBmgrSetChild * ** *** **** *****/

/* Change the state of a child to set */

void
XcuBmgrSetChild (bmgr, child)
    XcuBmgrWidget bmgr ;
    XcuButtonWidget child ;
{
int i ;

if (!XtIsSubclass ((Widget)bmgr, (WidgetClass)xcuBmgrWidgetClass))
 XtError
 ("XcuBmgrSetChild requires 1st arg to be subclass of xcuBmgrWidgetClass");

if (!XtIsSubclass ((Widget)child, (WidgetClass)xcuButtonWidgetClass))
 XtError
 ("XcuBmgrSetChild requires 2nd arg to be subclass of xcuButtonWidgetClass");

for (i=0;  i < bmgr->bmgr.n_buttons;  i++)
    {
    if (bmgr->bmgr.buttons[i] == child)
	{
	if (!bmgr->bmgr.buttons[i]->button.set)
	    Notify (bmgr, child) ;
	break ;
	}
    }
return ;
}

/***** **** *** ** * XcuBmgrUnsetChild * ** *** **** *****/

/* Change the state of a child to unset */

void
XcuBmgrUnsetChild (bmgr, child)
    XcuBmgrWidget bmgr ;
    XcuButtonWidget child ;
{
int i ;

if (!XtIsSubclass ((Widget)bmgr, (WidgetClass)xcuBmgrWidgetClass))
 XtError
 ("XcuBmgrUnsetChild requires 1st arg to be subclass of xcuBmgrWidgetClass");

if (!XtIsSubclass ((Widget)child, (WidgetClass)xcuButtonWidgetClass))
 XtError
 ("XcuBmgrUnsetChild requires 2nd arg to be subclass of xcuButtonWidgetClass");

for (i=0;  i < bmgr->bmgr.n_buttons;  i++)
    {
    if (bmgr->bmgr.buttons[i] == child)
	{
	if (bmgr->bmgr.buttons[i]->button.set)
	    Notify (bmgr, child) ;
	break ;
	}
    }
return ;
}

