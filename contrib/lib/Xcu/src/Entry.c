
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
 * XcuEntry.c - XcuEntry button widget
 */

#define XtStrlen(s)	((s) ? strlen(s) : 0)

  /* The following are defined for the reader's convenience.  Any
     Xt..Field macro in this code just refers to some field in
     one of the substructures of the WidgetRec.  */

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <ctype.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include <X11/Xcu/EntryP.h>
#include <X11/Xcu/EntryI.h>

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static void Highlight() ;
static void Unhighlight() ;
static void Key_press() ;
static void extend_buffer () ;
static void set_background () ;

static char defaultTranslations[] =
    "<KeyPress>:	key_press() \n\
     <EnterWindow>:	highlight() \n\
     <LeaveWindow>:	unset(NoRedisplay) unhighlight()";

static char def_false = False ;

#define offset(field) XtOffset(XcuEntryWidget, field)
static XtResource resources[] = { 
    {XtNcallback, XtCCallback, XtRCallback, sizeof(XPointer), 
      offset(entry.callbacks), XtRCallback, (XPointer)NULL}
   ,{XtNcolorEntry, XtCColorEntry, XtRBoolean, sizeof(Boolean), 
      offset(entry.color_entry), XtRBoolean, (XPointer)&def_false}
   ,{XtNsignalChar, XtCSignalChar, XtRString, sizeof(String), 
      offset(entry.signal_char), XtRString, (XPointer)"\004"}
   ,{XtNkillChar, XtCKillChar, XtRString, sizeof(char), 
      offset(entry.kill_char), XtRString, (XPointer)"\025"}
   ,{XtNeraseChar, XtCEraseChar, XtRString, sizeof(char), 
      offset(entry.erase_char), XtRString, (XPointer)"\010"}
   ,{XtNcursor, XtCCursor, XtRCursor, sizeof(Cursor),
      offset(simple.cursor), XtRString, "hand2"}
   ,{XtNprefix,  XtCPrefix, XtRString, sizeof(String),
      offset(entry.prefix), XtRString, NULL}
    /*** intentionally undocumented ! Probably used only by XcuWlm ***/
    ,{XtNprocedureList, XtCProcedureList,
      XtRProcedureList, sizeof(XtProcedureList *),
      offset(entry.procedure_list), XtRProcedureList, (XPointer)NULL}
};
#undef offset

static XtActionsRec actionsList[] =
{
  {"key_press",		Key_press},
  {"highlight",		Highlight},
  {"unhighlight",	Unhighlight},
};

  /* ...ClassData must be initialized at compile time.  Must
     initialize all substructures.  (Actually, last two here
     need not be initialized since not used.)
  */
#define superclass		(&xcuCommandClassRec)
XcuEntryClassRec xcuEntryClassRec = {
  {
    (WidgetClass) &xcuCommandClassRec,	/* superclass		  */	
    "Entry",				/* class_name		  */
    sizeof(XcuEntryRec),		/* size			  */
    NULL,				/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    XtInheritRealize,			/* realize		  */
    actionsList,			/* actions		  */
    XtNumber(actionsList),		/* num_actions		  */
    resources,				/* resources		  */
    XtNumber(resources),		/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    FALSE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    Destroy,				/* destroy		  */
    XtInheritResize,			/* resize		  */
    Redisplay,				/* expose		  */
    SetValues,				/* set_values		  */
    NULL,				/* set_values_hook	  */
    XtInheritSetValuesAlmost,		/* set_values_almost	  */
    NULL,				/* get_values_hook	  */
    NULL,				/* accept_focus		  */
    XtVersion,				/* version		  */
    NULL,				/* callback_private	  */
    defaultTranslations,		/* tm_table		  */
    NULL,				/* query_geometry	  */
    XtInheritDisplayAccelerator,	/* display accelerator	  */
    NULL,				/* extension		  */
  },
  {
    XtInheritChangeSensitive, /* Simple Class fields */
    XtInheritDrawFacets,
    XtInheritDrawHighlight
  },
  {
    XtInheritRedisplayText,  /* Label Class field */
  },
  {
    0,                                     /* field not used    */
  },  /* XcuCommandClass fields initialization */
  {
    0,                                     /* field not used    */
  },  /* XcuEntryClass fields initialization */
};

  /* for public consumption */
WidgetClass xcuEntryWidgetClass = (WidgetClass) &xcuEntryClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

/* ARGSUSED */
static void Initialize(request, new)
 Widget request, new;
{
static String XcuEntrySampleText = "Sample" ;
XcuEntryWidget cbw = (XcuEntryWidget) new;

      /* init flags for state */
    ComWset = FALSE;
    ComWhighlighted = FALSE;  
    ComWdisplayHighlighted = FALSE;
    ComWdisplaySet = FALSE;
    ComWnotExpose = False ;
    ComWhighlightChange = FALSE;
    cbw->entry.total_text = XtMalloc (XcuEntryBufferBlock) ;

    if (cbw->entry.prefix == NULL) 
	{
	*cbw->entry.total_text = '\0' ;
	cbw->entry.prefix_length = 0 ;
	}
    else
	{
	/* TODO: need to check against BufferBlock */
	strcpy (cbw->entry.total_text, cbw->entry.prefix) ;
	}

    cbw->entry.prefix = cbw->entry.total_text ;
    cbw->entry.prefix_length = strlen(cbw->entry.prefix) ;

    cbw->entry.entered_text = cbw->entry.total_text + cbw->entry.prefix_length ;
    /* TODO: need to check against BufferBlock */
    if (cbw->label.label)
	strcpy (cbw->entry.entered_text, cbw->label.label) ;
    else if (cbw->core.name)
	strcpy (cbw->entry.entered_text, cbw->core.name) ;
    cbw->entry.buffer_length = XcuEntryBufferBlock ;
    cbw->entry.n_entered = 0 ;
    if (cbw->entry.color_entry)
	set_background (cbw) ;

cbw->entry.procedure_list = (XtProcedureList *)
			   XtMalloc (2 * sizeof (XtProcedureList)) ;
cbw->entry.procedure_list[0].name = XcuEntrySampleText ;
cbw->entry.procedure_list[0].procedure = XcuEntrySample ;
cbw->entry.procedure_list[1].name = NULL ;
cbw->entry.procedure_list[1].procedure = NULL ;

    if (cbw->entry.prefix)
	{
	Arg arg ;
	XtSetArg (arg, XtNlabel, cbw->entry.total_text) ;
	XtSetValues (cbw, &arg, 1) ;
	}

return ;
} 

/***************************
*
*  Action Procedures
*
***************************/

/* ARGSUSED */
static void Key_press(w,event,params,num_params)
     Widget w;
     XKeyEvent *event;
     String *params;		/* unused */
     Cardinal *num_params;	/* unused */
{
char buffer[2] ;
KeySym keysym ;
XComposeStatus status ;
Arg arg[2] ;

XcuEntryWidget cbw = (XcuEntryWidget)w;
XLookupString (event, buffer, 1, &keysym, &status) ;


if (IsModifierKey(keysym))
    return ;

if (buffer[0] == '\015')	/* Convert carriage return to newline */
    buffer[0] = '\012' ;

if (buffer[0] == *cbw->entry.signal_char)
    {
    if (*cbw->entry.signal_char == '\012')
	{
	if (cbw->entry.n_entered == 0)
	    {
	    *cbw->entry.entered_text = '\0' ;
	    XtSetArg (arg[0], XtNlabel, cbw->entry.total_text) ;
	    XtSetValues (w, arg, 1) ;
	    }
	else
	    cbw->entry.n_entered = 0 ;
	}
    XtCallCallbacks (w, XtNcallback, cbw->entry.entered_text);
    if (cbw->entry.color_entry)
	set_background (cbw) ;
    }
else
    {
    if (buffer[0] == *cbw->entry.kill_char)
	{
	while (cbw->entry.n_entered &&
	       cbw->entry.entered_text[cbw->entry.n_entered-1] != '\012')
	    {
	    cbw->entry.entered_text[cbw->entry.n_entered-1] = '\0' ;
	    cbw->entry.n_entered-- ;
	    }
	}
    else
    if (buffer[0] == *cbw->entry.erase_char)
	{
	if (cbw->entry.n_entered)
	    {
	    cbw->entry.entered_text[cbw->entry.n_entered-1] = '\0' ;
	    cbw->entry.n_entered-- ;
	    }
	}
    else
	{
	if (cbw->entry.n_entered == cbw->entry.buffer_length - 1)
	    extend_buffer (cbw) ;

	cbw->entry.entered_text[cbw->entry.n_entered] = buffer[0] ;
	cbw->entry.n_entered++ ;
	cbw->entry.entered_text[cbw->entry.n_entered] = '\0' ;
	}

    XtSetArg (arg[0], XtNlabel, cbw->entry.total_text) ;
    XtSetValues (w, arg, 1) ;
    }
}

static void
extend_buffer (cbw)
   XcuEntryWidget cbw ;
{
char *temp ;
cbw->entry.buffer_length += XcuEntryBufferBlock ;
temp = XtMalloc (cbw->entry.buffer_length) ;
strcpy (temp, cbw->entry.total_text) ;
XtFree (cbw->entry.total_text) ;
cbw->entry.total_text = temp ;
cbw->entry.prefix = temp ;
cbw->entry.entered_text = temp + cbw->entry.prefix_length ;
return ;
}

/************************
*
*  REDISPLAY (DRAW)
*
************************/

/* ARGSUSED */
static void Redisplay(w, event, region)
    Widget w;
    XEvent *event;		/* unused */
    Region region;		/* unused */
{
(*superclass->core_class.expose)( w, event, region ) ;
return ;
}


/* ARGSUSED */
static void Destroy(w)
    Widget w;
{
XcuEntryWidget cbw = (XcuEntryWidget) w;
XtFree (cbw->entry.total_text) ;
return ;
}

static void convert_null_to_nl () ;
static void convert_nl_to_null () ;

/*
 * Set specified arguments into widget
 */
/* ARGSUSED */
static Boolean SetValues (current, request, new)
    Widget current, request, new;
{
    XcuEntryWidget cbw = (XcuEntryWidget) current;
    XcuEntryWidget newcbw = (XcuEntryWidget) new;
    convert_null_to_nl (newcbw) ;
    if (cbw->label.label != newcbw->label.label)
	{
	char *label0 = &newcbw->label.label[cbw->entry.prefix_length] ;
	/* TODO: need to check against BufferBlock */
	if (newcbw->label.label)
	    {
	    strcpy (newcbw->entry.entered_text, label0) ;
	    }
	else if (newcbw->core.name)
	    strcpy (newcbw->entry.entered_text, newcbw->core.name) ;
	if (newcbw->entry.color_entry)
	    set_background (newcbw) ;
	}
    if (cbw->entry.prefix != newcbw->entry.prefix)
	{
	String temp ;
	/* TODO: need to check against BufferBlock */
	newcbw->entry.prefix_length = strlen (newcbw->entry.prefix) ;
	temp = XtMalloc (newcbw->entry.buffer_length) ;
	strcpy (temp, newcbw->entry.prefix) ;
	strcat (temp, newcbw->label.label) ;
	XtFree (cbw->entry.total_text) ;
	newcbw->entry.total_text = temp ;
	newcbw->entry.prefix = temp ;
	newcbw->entry.entered_text = temp + newcbw->entry.prefix_length ;
	}
    convert_nl_to_null (newcbw) ;

    return False ;
}

static void
convert_null_to_nl (cbw)
    XcuEntryWidget cbw ;
{
int i ;
for (i=1;  i < cbw->label.n_lines;  i++)
    {
    cbw->label.label[cbw->label.locations[i]-1] = '\n' ;
    }
return ;
}

static void
convert_nl_to_null (cbw)
    XcuEntryWidget cbw ;
{
int i ;
for (i=1;  i < cbw->label.n_lines;  i++)
    cbw->label.label[cbw->label.locations[i]-1] = '\0' ;
return ;
}

static void Highlight(w,event,params,num_params)
     Widget w;
     XEvent *event;
     String *params;		/* unused */
     Cardinal *num_params;	/* unused */
{
  XcuEntryWidget cbw = (XcuEntryWidget)w;
  ComWhighlighted = TRUE;
  ComWhighlightChange = TRUE;
  ComWnotExpose = TRUE ;
  Redisplay(w, event, (Region)NULL);
  ComWhighlightChange = FALSE;
}

/* ARGSUSED */
static void Unhighlight(w,event,params,num_params)
     Widget w;
     XEvent *event;
     String *params;		/* unused */
     Cardinal *num_params;	/* unused */
{
  XcuEntryWidget cbw = (XcuEntryWidget)w;
  ComWhighlighted = FALSE;
  ComWhighlightChange = TRUE;
  ComWnotExpose = TRUE ;
  Redisplay(w, event, (Region)NULL);
  ComWhighlightChange = FALSE;
}

/***** **** *** ** * XcuEntrySample * ** *** **** *****/

void
XcuEntrySample (entry)
    XcuEntryWidget entry ;
{

if (!XtIsSubclass ((Widget)entry, (WidgetClass)xcuEntryWidgetClass)) {
    XtError
    ("XcuEntrySample requires 1st arg to be subclass of xcuEntryWidgetClass");
    }
XtCallCallbacks ((Widget)entry, XtNcallback, entry->entry.entered_text) ;
return ;
}

static void
set_background (cbw)
    XcuEntryWidget cbw ;
{
Arg arg[2] ;
XColor visual_def, exact_def ;
Display *display = XtDisplay((Widget)cbw) ;

if (XAllocNamedColor(display,
		     DefaultColormap(display,DefaultScreen(display)),
		     cbw->entry.entered_text,
		     &visual_def, &exact_def)
    == True)
    {
    float red = ((int)exact_def.red)   / 65280.0 ;
    float grn = ((int)exact_def.green) / 65280.0 ;
    float blu = ((int)exact_def.blue)  / 65280.0 ;
    float sum = 0.30 * red * red +
		0.59 * grn * grn +
		0.11 * blu * blu ;
    if (sum >= 0.5)
	XtSetArg (arg[0], XtNforeground,
		  BlackPixel(display,DefaultScreen(display))) ;
    else
	XtSetArg (arg[0], XtNforeground,
		  WhitePixel(display,DefaultScreen(display))) ;
    XtSetArg (arg[1], XtNbackground, exact_def.pixel) ;
    XtSetValues ((Widget)cbw, arg, 2) ;
    }
return ;
}
