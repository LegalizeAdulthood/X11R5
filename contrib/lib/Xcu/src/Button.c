
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
 * XcuButton.c - XcuButton widget
 */

#define XtStrlen(s)	((s) ? strlen(s) : 0)

  /* The following are defined for the reader's convenience.  Any
     Xt..Field macro in this code just refers to some field in
     one of the substructures of the WidgetRec.  */

#include <X11/Xos.h>
#include <ctype.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include <X11/Xcu/ButtonP.h>
#include <X11/Xcu/ButtonI.h>
#include <X11/Xmu/Drawing.h>

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static char defaultTranslations[] =
    "<Btn1Down>:	notify(down) \n\
     <Btn1Up>:		notify(up)\n\
     <EnterWindow>:	highlight() \n\
     <LeaveWindow>:	unhighlight()";

static Boolean defFalse = False;

#define offset(field) XtOffset(XcuButtonWidget, field)
static XtResource resources[] = { 

   {XtNcallback, XtCCallback, XtRCallback, sizeof(XPointer), 
      offset(button.callbacks), XtRCallback, (XPointer)NULL},
   {XtNcursor, XtCCursor, XtRCursor, sizeof(Cursor),
      offset(simple.cursor), XtRString, "dot"},
   {XtNset, XtCBoolean, XtRBoolean, sizeof(Boolean),
      offset(button.set), XtRBoolean, (XPointer)&defFalse},
};
#undef offset

static XtActionsRec actionsList[] =
{
  {"notify",		Notify},
  {"highlight",		Highlight},
  {"unhighlight",	Unhighlight},
};

  /* ...ClassData must be initialized at compile time.  Must
     initialize all substructures.
  */
#define superclass		(&xcuLabelClassRec)
XcuButtonClassRec xcuButtonClassRec =
{
  {
    (WidgetClass) &xcuLabelClassRec,	/* superclass		  */	
    "Button",				/* class_name		  */
    sizeof(XcuButtonRec),		/* size			  */
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
    XtInheritQueryGeometry,		/* query_geometry	  */
    XtInheritDisplayAccelerator,	/* display accelerator	  */
    NULL,				/* extension		  */
  },  /* CoreClass fields initialization */
  {
    XtInheritChangeSensitive, /* Simple Class fields */
    XtInheritDrawFacets,
    XtInheritDrawHighlight
  },  /* SimpleClass fields initialization */
  {
    XtInheritRedisplayText,
  },  /* LabelClass fields initialization */
  {
    0
  },  /* XcuButtonClass fields initialization */
};

  /* for public consumption */
WidgetClass xcuButtonWidgetClass = (WidgetClass) &xcuButtonClassRec;

static void GetnormalGC () ;
static void GetgrayGC () ;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void Get_inverseGC(cbw)
    XcuButtonWidget cbw;
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
    XcuButtonWidget cbw;
{
    XGCValues	values;

    /* Set up a GC for inverse (set) state */

    if (cbw->simple.shadow)
	values.foreground   = cbw->simple.background_pixel;
    else
	values.foreground   = ComWbackground; /* default is White */
    values.font		= ComWfont->fid;
    values.fill_style   = FillSolid;

    ComWinverseTextGC = XtGetGC((Widget)cbw,
    	(unsigned) GCForeground | GCFont | GCFillStyle, &values);
}

static void Get_highlightGC(cbw)
    XcuButtonWidget cbw;
{
    XGCValues	values;
    
    /* Set up a GC for highlighted state.  It has a thicker
       line width for the highlight border */

    values.foreground   = ComWforeground;

    ComWhighlightGC = XtGetGC((Widget)cbw,
    	(unsigned) GCForeground, &values);
}

/***** **** *** ** * GetgrayGC * ** *** **** *****/

static void
GetgrayGC(cbw)
    XcuButtonWidget cbw;
{
XGCValues	values;

values.foreground = cbw->label.foreground ;
values.font	  = cbw->label.font->fid ;
values.tile       = XmuCreateStippledPixmap (XtScreen((Widget)cbw),
					     cbw->label.foreground,
					     cbw->core.background_pixel,
					     cbw->core.depth) ;
values.fill_style = FillTiled ;

cbw->button.gray_GC = XtGetGC(
    (Widget)cbw, 
    (unsigned) GCForeground | GCFont | GCTile | GCFillStyle, 
    &values);
return ;
}

/***** **** *** ** * GetnormalGC * ** *** **** *****/

static void
GetnormalGC (cbw)
    XcuButtonWidget cbw ;
{
XGCValues	values ;

values.foreground	= cbw->label.foreground ;
values.font		= cbw->label.font->fid ;

cbw->button.normal_GC = XtGetGC(
    (Widget)cbw,
    (unsigned) GCForeground | GCFont,
    &values) ;
return ;
}


/* ARGSUSED */
static void Initialize(request, new)
 Widget request, new;
{
    XcuButtonWidget cbw = (XcuButtonWidget) new;

    Get_inverseGC(cbw);
    Get_inverseTextGC(cbw);
    Get_highlightGC(cbw);
    GetnormalGC (cbw) ;
    GetgrayGC (cbw) ;

      /* init flags for state */
    ComWhighlighted = FALSE;  
    ComWdisplayHighlighted = FALSE;
    ComWdisplaySet = FALSE;
    ComWnotExpose = FALSE;

} 

/***************************
*
*  Action Procedures
*
***************************/

/* ARGSUSED */
static void Highlight(w,event,params,num_params)
     Widget w;
     XEvent *event;
     String *params;		/* unused */
     Cardinal *num_params;	/* unused */
{
  XcuButtonWidget cbw = (XcuButtonWidget)w;
  ComWhighlighted = TRUE;
  ComWhighlightChange = TRUE;
  ComWnotExpose = TRUE;
  Redisplay(w, event, (Region)NULL);
  ComWhighlightChange = False;
}

/* ARGSUSED */
static void Unhighlight(w,event,params,num_params)
     Widget w;
     XEvent *event;
     String *params;		/* unused */
     Cardinal *num_params;	/* unused */
{
  XcuButtonWidget cbw = (XcuButtonWidget)w;
  ComWhighlighted = FALSE;
  ComWhighlightChange = TRUE;
  ComWnotExpose = TRUE;
  Redisplay(w, event, (Region)NULL);
  ComWhighlightChange = False;
}

/* ARGSUSED */
static void Notify(w,event,params,num_params)
     Widget w;
     XEvent *event;
     String *params;		/* unused */
     Cardinal *num_params;	/* unused */
{
  /* if (ComWset) */
      XtCallCallbacks(w, XtNcallback, params[0]);
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
XcuButtonWidget cbw = (XcuButtonWidget) w;

/* Here's the scoop:  If the button is normal,
  you show the text.  If the button is highlighted but 
  not set, you draw a thick border and show the text.
  If the button is set, draw the button and text
  in inverse. */

/* Note that Redisplay must remember the state of its last
  draw to determine whether to erase the window before
  redrawing to avoid flicker.  If the state is the same,
  the window just needs to redraw (even on an expose). */


/*
 * if it is not highlighted and it used to be highlighted and it is not set, or
 * if it is not set and it used to be set
 *   --> Clear the window
 * else if it is set
 *   --> Fill the window with black
 */
cbw->label.needs_fill = cbw->simple.shadow ? True : False ;
cbw->simple.exposing = ComWnotExpose ? False : True ;
ComWnotExpose = False ;

if (
    (!ComWset && ComWdisplaySet)
   )
    {
    cbw->label.pixmap_GC = cbw->label.pixmap_GC ;
    cbw->label.background_GC = ComWinverseTextGC ;
    cbw->label.needs_fill = True ;
    }
else if (ComWset)
    {
    /*
     * Don't clear the window if the button's in a set condition;
     * instead, fill it with black to avoid flicker. (Must fill
     * with black in case it was an expose)
     */
    cbw->label.pixmap_GC = cbw->label.pixmap_GC ;
    cbw->label.background_GC = ComWinverseGC ;
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

return ;
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
    XcuButtonWidget cbw = (XcuButtonWidget) current;
    XcuButtonWidget newcbw = (XcuButtonWidget) new;

    if (XtCField(newcbw,sensitive) != ComWsensitive &&
	!XtCField(newcbw,sensitive)) {  /* about to become insensitive? */
	newcbw->button.set = FALSE;	/* yes, then anticipate unset */
	newcbw->simple.highlighted = FALSE;
    }

     if (XtLField(newcbw,foreground) != ComWforeground)
       {
         XtReleaseGC(new, ComWinverseGC);
	 Get_inverseGC(newcbw);
         XtReleaseGC(new, ComWhighlightGC);
	 Get_highlightGC(newcbw);
       }
    else 
      {
	if (XtCField(newcbw,background_pixel) != ComWbackground ||
	     XtLField(newcbw,font) != ComWfont) {
	     XtReleaseGC(new, ComWinverseTextGC);
	     Get_inverseTextGC(newcbw);
	     }
      }
     
    return (XtCField(newcbw, sensitive) != ComWsensitive ||
	    XtBField(newcbw, set) != ComWset ||
	    newcbw->simple.highlighted != ComWhighlighted);
}
