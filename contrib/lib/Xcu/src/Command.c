
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
 * XcuCommand.c - XcuCommand button widget
 */

#define XtStrlen(s)	((s) ? strlen(s) : 0)

  /* The following are defined for the reader's convenience.  Any
     Xt..Field macro in this code just refers to some field in
     one of the substructures of the WidgetRec.  */

#include <stdio.h>
#include <X11/Xos.h>
#include <ctype.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include <X11/Xcu/CommandP.h>
#include <X11/Xcu/CommandI.h>
#include <X11/Xmu/Drawing.h>

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static char defaultTranslations[] =
    "<Btn1Down>:	set() \n\
     <Btn1Up>:		notify() unset() \n\
     <EnterWindow>:	highlight() \n\
     <LeaveWindow>:	unset(NoRedisplay) unhighlight()";

#define offset(field) XtOffset(XcuCommandWidget, field)
static XtResource resources[] = { 

   {XtNcallback, XtCCallback, XtRCallback, sizeof(XPointer), 
      offset(command.callbacks), XtRCallback, (XPointer)NULL},
   {XtNcursor, XtCCursor, XtRCursor, sizeof(Cursor),
      offset(simple.cursor), XtRString, "dot"},
};
#undef offset

static XtActionsRec actionsList[] =
{
  {"set",		Set},
  {"notify",		Notify},
  {"highlight",		Highlight},
  {"unset",		Unset},
  {"unhighlight",	Unhighlight},
};

  /* ...ClassData must be initialized at compile time.  Must
     initialize all substructures.  (Actually, last two here
     need not be initialized since not used.)
  */
#define superclass		(&xcuLabelClassRec)
XcuCommandClassRec xcuCommandClassRec = {
  {
    (WidgetClass) &xcuLabelClassRec,	/* superclass		  */	
    "Command",			/* class_name		  */
    sizeof(XcuCommandRec),		/* size			  */
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
};

static void GetnormalGC () ;
static void GetgrayGC () ;

  /* for public consumption */
WidgetClass xcuCommandWidgetClass = (WidgetClass) &xcuCommandClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void Get_inverseGC(cbw)
    XcuCommandWidget cbw;
{
    XGCValues	values;

    /* Set up a GC for inverse (set) state */

    values.foreground   = ComWforeground;
    values.font		= ComWfont->fid;
    values.fill_style   = FillSolid;

    ComWinverseGC = XtGetGC((Widget)cbw,
    	(unsigned) GCForeground | GCFont | GCFillStyle, &values);
}

static void Get_inverseTextGC(cbw)
    XcuCommandWidget cbw;
{
    XGCValues	values;

    /* Set up a GC for inverse (set) state */

    if (cbw->simple.shadow)
	values.foreground   = cbw->simple.background_pixel;
    else
	values.foreground   = ComWbackground; /* default is White */
    values.font		= ComWfont->fid;
    values.fill_style   = FillSolid;
    values.line_width   = ComWhighlightThickness > 1
			  ? ComWhighlightThickness : 0;

    ComWinverseTextGC = XtGetGC((Widget)cbw,
    	(unsigned) GCLineWidth | GCForeground | GCFont | GCFillStyle, &values);
}

static void Get_highlightGC(cbw)
    XcuCommandWidget cbw;
{
    XGCValues	values;
    
    /* Set up a GC for highlighted state.  It has a thicker
       line width for the highlight border */

    values.foreground   = ComWforeground;
    values.line_width   = ComWhighlightThickness > 1
			  ? ComWhighlightThickness : 0;

    ComWhighlightGC = XtGetGC((Widget)cbw,
    	(unsigned) GCForeground | GCLineWidth, &values);
}

/***** **** *** ** * GetgrayGC * ** *** **** *****/

static void
GetgrayGC(cbw)
    XcuCommandWidget cbw;
{
XGCValues	values;

values.foreground = cbw->label.foreground ;
values.font	  = cbw->label.font->fid ;
values.tile       = XmuCreateStippledPixmap (XtScreen((Widget)cbw),
					     cbw->label.foreground,
					     cbw->core.background_pixel,
					     cbw->core.depth) ;
values.fill_style = FillTiled ;

cbw->command.gray_GC = XtGetGC(
    (Widget)cbw, 
    (unsigned) GCForeground | GCFont | GCTile | GCFillStyle, 
    &values);
return ;
}

/***** **** *** ** * GetnormalGC * ** *** **** *****/

static void
GetnormalGC (cbw)
    XcuCommandWidget cbw ;
{
XGCValues	values ;

values.foreground	= cbw->label.foreground ;
values.font		= cbw->label.font->fid ;

cbw->command.normal_GC = XtGetGC(
    (Widget)cbw,
    (unsigned) GCForeground | GCFont,
    &values) ;
return ;
}


/* ARGSUSED */
static void Initialize(request, new)
 Widget request, new;
{
    XcuCommandWidget cbw = (XcuCommandWidget) new;

    Get_inverseGC(cbw);
    Get_inverseTextGC(cbw);
    Get_highlightGC(cbw);
    GetnormalGC (cbw) ;
    GetgrayGC (cbw) ;

      /* init flags for state */
    ComWset = FALSE;
    ComWhighlighted = FALSE;  
    ComWdisplayHighlighted = FALSE;
    ComWdisplaySet = FALSE;
    ComWnotExpose = False ;
    ComWhighlightChange = FALSE;

} 

/***************************
*
*  Action Procedures
*
***************************/

/* ARGSUSED */
static void Set(w,event,params,num_params)
     Widget w;
     XEvent *event;
     String *params;		/* unused */
     Cardinal *num_params;	/* unused */
{
  XcuCommandWidget cbw = (XcuCommandWidget)w;
  ComWset = TRUE;
  ComWnotExpose = TRUE ;
  Redisplay(w, event, (Region)NULL);
}

/* ARGSUSED */
static void Unset(w,event,params,num_params)
     Widget w;
     XEvent *event;
     String *params;		/* unused */
     Cardinal *num_params;
{
  XcuCommandWidget cbw = (XcuCommandWidget)w;
  ComWset = FALSE;
  if (*num_params == 0)
      {
      ComWnotExpose = TRUE ;
      Redisplay(w, event, (Region)NULL);
      }
}

/* ARGSUSED */
static void Highlight(w,event,params,num_params)
     Widget w;
     XEvent *event;
     String *params;		/* unused */
     Cardinal *num_params;	/* unused */
{
  XcuCommandWidget cbw = (XcuCommandWidget)w;
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
  XcuCommandWidget cbw = (XcuCommandWidget)w;
  ComWhighlighted = FALSE;
  ComWhighlightChange = TRUE;
  ComWnotExpose = TRUE ;
  Redisplay(w, event, (Region)NULL);
  ComWhighlightChange = FALSE;
}

/* ARGSUSED */
static void Notify(w,event,params,num_params)
     Widget w;
     XEvent *event;
     String *params;		/* unused */
     Cardinal *num_params;	/* unused */
{
  XcuCommandWidget cbw = (XcuCommandWidget)w;
  if (ComWset)
      XtCallCallbacks(w, XtNcallback, NULL);
}
/*
 * Repaint the widget window
 */

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
   XcuCommandWidget cbw = (XcuCommandWidget) w;

   /* Here's the scoop:  If the command button button is normal,
      you show the text.  If the command button is highlighted but 
      not set, you draw a thick border and show the text.
      If the command button is set, draw the button and text
      in inverse. */

   /* Note that Redisplay must remember the state of its last
      draw to determine whether to erase the window before
      redrawing to avoid flicker.  If the state is the same,
      the window just needs to redraw (even on an expose). */

   /* The shadow needs to be redrawn on expose and highlight changes,
      but not on set changes.
      The facets need to be redrawn only on expose. */
cbw->label.needs_fill = cbw->simple.shadow ? True : False ;
cbw->simple.exposing = ComWnotExpose ? False : True ;
ComWnotExpose = False ;

if (cbw->simple.exposing)
    {
    /* TODO: Need to check location of cursor to find out what highlight
     * status is...
     */
    }

if (
    (!ComWset && ComWdisplaySet)
   )
    {
    cbw->label.background_GC = ComWinverseTextGC ;
    cbw->label.pixmap_GC = cbw->label.pixmap_GC ;
    cbw->label.needs_fill = True ;
    }
else if (ComWset)
    {
    /*
     * Don't clear the window if the button's in a set condition;
     * instead, fill it with black to avoid flicker. (Must fill
     * with black in case it was an expose)
     */
    cbw->label.background_GC = ComWinverseGC ;
    cbw->label.pixmap_GC = cbw->label.pixmap_GC ;
    cbw->label.needs_fill = True ;
    }

/* draw the string:  there are three different "styles" for it,
    all in separate GCs */

if (ComWsensitive)
    {
    if (ComWset)
	{
	cbw->label.sensitive_text_GC = ComWinverseTextGC ;
	}
    else
	{
	cbw->label.sensitive_text_GC = ComWnormalGC ;
	}
    }
else
    {
    cbw->label.insensitive_text_GC = ComWgrayGC ;
    }

(*superclass->core_class.expose)( w, event, region ) ;

ComWdisplayHighlighted = ComWhighlighted;
ComWdisplaySet = ComWset;

}


/* ARGSUSED */
static void Destroy(w)
    Widget w;
{
  /* must free GCs and pixmaps */
}


/*
 * Set specified arguments into widget
 */
/* ARGSUSED */
static Boolean SetValues (current, request, new)
    Widget current, request, new;
{
    XcuCommandWidget cbw = (XcuCommandWidget) current;
    XcuCommandWidget newcbw = (XcuCommandWidget) new;

    if (XtCField(newcbw,sensitive) != ComWsensitive &&
	!XtCField(newcbw,sensitive)) {  /* about to become insensitive? */
	XtCBField(newcbw,set) = FALSE;	/* yes, then anticipate unset */
	newcbw->simple.highlighted = FALSE;
    }

     if (XtLField(newcbw,foreground) != ComWforeground)
       {
         XtReleaseGC(new, ComWinverseGC);
	 Get_inverseGC(newcbw);
         XtReleaseGC(new, ComWhighlightGC);
	 Get_highlightGC(newcbw);
         XtReleaseGC(new, ComWnormalGC);
	 GetnormalGC (newcbw) ;
       }
    else 
      {
	if (XtCField(newcbw,background_pixel) != ComWbackground ||
	     XtLField(newcbw,font) != ComWfont ||
	     newcbw->simple.background_pixel != cbw->simple.background_pixel) {
	     XtReleaseGC(new, ComWinverseTextGC);
	     Get_inverseTextGC(newcbw);
	     }
      }
     
    return (XtCField(newcbw, sensitive) != ComWsensitive ||
	    newcbw->simple.highlighted != ComWhighlighted);
}
