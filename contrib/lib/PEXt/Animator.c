/*

			    A n i m a t o r . c

    $Header: Animator.c,v 1.0 91/10/04 17:00:53 rthomson Exp $

    Rich Thomson
    Design Systems Division
    Evans & Sutherland Computer Corporation

     Copyright (C) 1990, 1991, Evans & Sutherland Computer Corporation
*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

       Copyright 1990, 1991 by Evans & Sutherland Computer Corporation,
			     Salt Lake City, Utah
				       
			     All Rights Reserved
				       
    Permission to use, copy, modify, and distribute this software and its
       documentation for any purpose and without fee is hereby granted,
    provided that the above copyright notice appear in all copies and that
       both that copyright notice and this permission notice appear in
  supporting documentation, and that the names of Evans & Sutherland not be
      used in advertising or publicity pertaining to distribution of the
	     software without specific, written prior permission.
				       
  EVANS & SUTHERLAND  DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
    INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
    EVENT SHALL EVANS & SUTHERLAND BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
    DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
       TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
			PERFORMANCE OF THIS SOFTWARE.
				       
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#include <stdio.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>

#include "AnimatorP.h"

#define aoffset(field) XtOffsetOf(PEXtAnimatorRec, animator.field)

					/* resources for widget instances */
static XtResource resources[] = {
  { PEXtNupdateRate, PEXtCUpdateRate, XtRFloat, sizeof(float),
      aoffset(rate), XtRImmediate, (XtPointer) 0 },
  { PEXtNupdateCallback, PEXtCUpdateCallback, XtRCallback,
      sizeof(XtPointer), aoffset(update), XtRCallback, NULL }
};

static void Frame();

static XtActionsRec actions[] = {
  { "frame", Frame },
};
					/* widget method functions */
static void Initialize();
static void Realize();
static void Destroy();
static Boolean SetValues();

PEXtAnimatorClassRec pextAnimatorClassRec = {
  {					/* core fields */
    (WidgetClass) &pextWorkstationWidgetClass, /* superclass */
    "Animator",				/* class_name */
    sizeof(PEXtAnimatorRec),		/* widget_size */
    (XtProc) NULL,			/* class_initialize */
    (XtWidgetClassProc) NULL,		/* class_part_init */
    FALSE,				/* class_inited */
    (XtInitProc) Initialize,		/* initialize */
    NULL,				/* initialize_hook */
    (XtRealizeProc) Realize,		/* realize */
    actions,				/* actions */
    XtNumber(actions),			/* num_actions */
    resources,				/* resources */
    XtNumber(resources),		/* num_resources */
    NULLQUARK,				/* xrm_class */
    TRUE,				/* compress_motion */
    XtExposeCompressMultiple,		/* compress_exposure */
    TRUE,				/* compress_enterleave */
    FALSE,				/* visible_interest */
    (XtWidgetProc) Destroy,		/* destroy */
    (XtWidgetProc) XtInheritResize,	/* resize */
    (XtExposeProc) XtInheritExpose,	/* expose */
    (XtSetValuesFunc) SetValues,	/* set_values */
    NULL,				/* set_values_hook */
    XtInheritSetValuesAlmost,		/* set_values_almost */
    NULL,				/* get_values_hook */
    NULL,				/* accept_focus */
    XtVersion,				/* version */
    NULL,				/* callback_private */
    NULL,				/* tm_table */
    (XtGeometryHandler) XtInheritQueryGeometry,	/* query_geometry */
    XtInheritDisplayAccelerator,	/* display_accelerator */
    NULL,
  },
  {					/* workstation fields */
    0,					/* dummy_field */
  },
  {					/* animator fields */
    0,					/* dummy_field */
  },
};

					/* public class pointer */
WidgetClass pextAnimatorWidgetClass = (WidgetClass) &pextAnimatorClassRec;

/*
			Method Function Definitions
*/

/*
				Initialize
*/
#define DEFAULTWIDTH 300
#define DEFAULTHEIGHT 300
#define DEFAULTRATE (1.0/24.0)

static void Initialize(treq, tnew, args, num_args)
     Widget treq, tnew;
     ArgList args;
     Cardinal num_args;
{
  PEXtAnimatorWidget new = (PEXtAnimatorWidget) tnew;

  if (new->core.height == 0)
    new->core.height = DEFAULTHEIGHT;

  if (new->core.width == 0)
    new->core.width = DEFAULTWIDTH;

  if (new->animator.rate < 0)
    new->animator.rate = DEFAULTRATE;

  new->animator.timed = (XtIntervalId) NULL;
  new->animator.background = (XtWorkProcId) NULL;
}

static void do_frame_timer();
static Boolean do_frame_bg();

/*
				 Realize
*/
#define superclass (&pextWorkstationClassRec)

static void Realize(w, valueMask, attributes)
     Widget w;
     XtValueMask valueMask;
     XSetWindowAttributes *attributes;
{
  PEXtAnimatorWidget anim = (PEXtAnimatorWidget) w;

					/* realize workstation first */
  (*superclass->workstation_class.realize)(w, valueMask, attributes);

					/* do we need updates? */
  if (XtHasCallbacks(w, PEXtNupdateCallback) == XtCallbackHasSome)
    {
      if (anim->animator.rate == 0.0)	/* yes, continuously! */
	anim->animator.background =
	  XtAppAddWorkProc(XtWidgetToApplicationContext(w), do_frame_bg,
			   (XtPointer) w);
      else				/* yes, at certain rate */
	anim->animator.timed =
	  XtAppAddTimeOut(XtWidgetToApplicationContext(w),
			  anim->animator.rate * 1000, do_frame_timer,
			  (XtPointer) w);
    }
}

/*
				  Destroy
*/
static void Destroy(w)
     Widget w;
{
  PEXtAnimatorWidget anim = (PEXtAnimatorWidget) w;

  if (anim->animator.timed)
    XtRemoveTimeOut(XtWidgetToApplicationContext(w), anim->animator.timed);
  else if (anim->animator.background)
    XtRemoveWorkProc(anim->animator.background);
}

/*
				SetValues
*/
static Boolean SetValues(current, request, new, args, num_args)
     Widget current, request, new;
     ArgList args;
     Cardinal *num_args;
{
  printf("SetValues not implemented for Animator\n");
}

static void do_frame_timer(client_data, id)
     register XtPointer client_data;
     register XtIntervalId *id;
{
  register Widget w = (Widget) client_data;

  Frame(w, (XEvent *) NULL, (String *) NULL, (Cardinal *) NULL);
}

static Boolean do_frame_bg(client_data)
     register XtPointer client_data;
{
  register Widget w = (Widget) client_data;

  Frame(w, (XEvent *) NULL, (String *) NULL, (Cardinal *) NULL);

  return False;
}

/*
				  Frame()

  This action procedure calls the update callbacks of the animator part,
  and then the traverse callbacks of the workstation part.
*/
/*ARGSUSED*/
static void Frame(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  PEXtAnimatorWidget anim = (PEXtAnimatorWidget) w;

  XtCallCallbacks(w, PEXtNupdateCallback, (XtPointer) NULL);
  XtCallActionProc(w, "Traverse", event, params, num_params);

  if (anim->animator.timed)
    {
      XtRemoveTimeOut(anim->animator.timed);
      anim->animator.timed =
	  XtAppAddTimeOut(XtWidgetToApplicationContext(w),
			  anim->animator.rate * 1000, do_frame_timer,
			  (XtPointer) w);
    }
}

Widget PEXtCreateAnimator(parent, name, args, n)
     Widget parent;
     String name;
     ArgList args;
     Cardinal n;
{
  return XtCreateWidget(name, pextAnimatorWidgetClass, parent, args, n);
}
