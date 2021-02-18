/*
			   W o r k s t a t i . c

    $Header: Workstati.c,v 1.0 91/10/04 17:01:07 rthomson Exp $

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

#include "StringDefs.h"
#include "Quarks.h"
#include "PEXt.h"
#include "Converters.h"

#include "WorkstatP.h"

#define WSID_WARNING \
  "Workstation: Can't change workstation identifier after realization."
#define WSTYPE_WARNING \
  "Workstation: Can't change workstation type after realization."

#define DEFAULTWSID 1			/* default workstation ID */

/*
  magic numbers used to detect condition where resource's value is taken
  from the list below and not set outside us.
*/
#define MAGIC_WSID -1
#define MAGIC_HLHSR -1
#define MAGIC_MOD -1
#define MAGIC_DEFER -1


#define wsoffset(field) XtOffsetOf(PEXtWorkstationRec, workstation.field)

static void Unrealize();
static XtCallbackRec UnrealizeCallbackList[] = {
  { Unrealize, (XtPointer) NULL },
  { (XtCallbackProc) NULL, (XtPointer) NULL }
};
					/* resources for widget instances */
static XtResource resources[] = {
  { PEXtNworkstationId, PEXtCWorkstationId, XtRInt, sizeof(int),
      wsoffset(id), XtRImmediate, (XtPointer) MAGIC_WSID },
  { PEXtNworkstationType, PEXtCWorkstationType, PEXtRWorkstationType,
      sizeof(Pint), wsoffset(type), XtRImmediate, (XtPointer) NULL },
  { PEXtNhlhsrMode, PEXtCHLHSRMode, PEXtRHLHSRMode, sizeof(int),
      wsoffset(hlhsr), XtRImmediate, (XtPointer) MAGIC_HLHSR },
  { PEXtNdeferralMode, PEXtCDeferralMode, PEXtRDeferralMode,
      sizeof(Pdefer_mode), wsoffset(deferral),
      XtRImmediate, (XtPointer) MAGIC_DEFER },
  { PEXtNmodificationMode, PEXtCModificationMode, PEXtRModificationMode,
      sizeof(Pmod_mode), wsoffset(modification),
      XtRImmediate, (XtPointer) MAGIC_MOD },
  { PEXtNresizePolicy, PEXtCResizePolicy, PEXtRResizePolicy,
      sizeof(PEXtResizePolicy), wsoffset(resize_policy),
      XtRString, PEXtDefaultResizePolicy },
  { PEXtNresizeGravity, PEXtCGravity, XtRGravity, sizeof(XtGravity),
      wsoffset(resize_gravity), XtRString, PEXtEcenter },
  { PEXtNresizeCallback, PEXtCResizeCallback, XtRCallback,
      sizeof(XtPointer), wsoffset(resize_callback), XtRCallback, NULL },
  { PEXtNtraversalPolicy, PEXtCTraversalPolicy, PEXtRTraversalPolicy,
      sizeof(PEXtTraversalPolicy), wsoffset(traversal_policy),
      XtRString, PEXtDefaultTraversalPolicy },
  { PEXtNtraversalCallback, PEXtCTraversalCallback, XtRCallback,
      sizeof(XtPointer), wsoffset(traversal_callback), XtRCallback, NULL },
  { PEXtNcontrolFlag, PEXtCControlFlag, PEXtRControlFlag, sizeof(Pctrl_flag),
      wsoffset(control), XtRString, PEXtDefaultControlFlag },
  { PEXtNunrealizeCallback, PEXtCUnrealizeCallback, XtRCallback,
      sizeof(XtPointer), wsoffset(unrealize_callback),
      XtRImmediate, (XtPointer) UnrealizeCallbackList }
};

static void Traverse();

static XtActionsRec actions[] = {
  { "Traverse", Traverse }
};
					/* widget method functions */
static void ClassInitialize();
static void Initialize();
static void Realize();
static void Destroy();
static void Resize();
static void Redisplay();
static Boolean SetValues();
static XtGeometryResult QueryGeometry();

PEXtWorkstationClassRec pextWorkstationClassRec = {
  {					/* core fields */
    (WidgetClass) &coreClassRec,	/* superclass */
    "PEXtWorkstation",			/* class_name */
    sizeof(PEXtWorkstationRec),		/* widget_size */
    (XtProc) ClassInitialize,		/* class_initialize */
    NULL,				/* class_part_init */
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
    XtExposeCompressMultiple | XtExposeNoExpose, /* compress_exposure */
    TRUE,				/* compress_enterleave */
    FALSE,				/* visible_interest */
    (XtWidgetProc) Destroy,		/* destroy */
    (XtWidgetProc) Resize,		/* resize */
    (XtExposeProc) Redisplay,	/* expose */
    (XtSetValuesFunc) SetValues,	/* set_values */
    NULL,				/* set_values_hook */
    XtInheritSetValuesAlmost,		/* set_values_almost */
    NULL,				/* get_values_hook */
    NULL,				/* accept_focus */
    XtVersion,				/* version */
    NULL,				/* callback_private */
    NULL,				/* tm_table */
    (XtGeometryHandler) QueryGeometry,	/* query_geometry */
    XtInheritDisplayAccelerator,	/* display_accelerator */
    NULL,
  },
  {					/* workstation fields */
    (XtRealizeProc) Realize,		/* realize method */
  },
};

					/* public class pointer */
WidgetClass pextWorkstationWidgetClass =
  (WidgetClass) &pextWorkstationClassRec;

/*
			Method Function Definitions
*/

/*
  ClassInitialize

  This method initializes the widget's class structure.  The realize method
  for the workstation class has already been assigned in the structure
  above. It will be called by sub-classes, if needed.
*/
static void ClassInitialize()
{
  PEXtRegisterConverters();		/* initialize conversion routines */
}

/*
  Initialize

  This method initializes the new widget's instance record.
*/
static void Initialize(treq, tnew, args, num_args)
     Widget treq, tnew;
     ArgList args;
     Cardinal num_args;
{
  PEXtWorkstationWidget new = (PEXtWorkstationWidget) tnew;

  if (!new->workstation.type)
    new->workstation.type = phigs_ws_type_x_drawable;

  if (new->workstation.id == MAGIC_WSID)
    new->workstation.id = DEFAULTWSID;

  new->workstation.state = PWS_ST_WSCL;

  if (new->workstation.hlhsr == MAGIC_HLHSR)
    new->workstation.hlhsr = PHIGS_HLHSR_MODE_NONE;

  if (new->workstation.deferral == (Pdefer_mode) MAGIC_DEFER ||
      new->workstation.modification == (Pmod_mode) MAGIC_MOD)
    {
      Pdefer_mode def;
      Pmod_mode mod;

      pinq_def_disp_upd_st(new->workstation.type, &PEXtError, &def, &mod);
      SAFE_PEX("Workstation.Initialize.inq_def_dsp_upd_st");

      if (new->workstation.deferral == (Pdefer_mode) MAGIC_DEFER)
	new->workstation.deferral = def;

      if (new->workstation.modification == (Pmod_mode) MAGIC_MOD)
	new->workstation.modification = mod;
    }      
}

/*
				 Realize

  This is the Realize method for the workstation.  It calls the superclass'
  realize method to get the X window created and then attempts to open a
  PHIGS workstation on that X window.

  Once the workstation is successfully opened, the widget must set all the
  workstation state in the server described by its instance variables.

  This consists of the workstation's hidden-line/surface removal mode and
  display update state.
*/
#define superclass (&coreClassRec)

static void Realize(w, valueMask, attributes)
     Widget w;
     XtValueMask valueMask;
     XSetWindowAttributes *attributes;
{
  PEXtWorkstationWidget ws = (PEXtWorkstationWidget) w;

  if (ws->workstation.type == phigs_ws_type_x_drawable)	/* WStype drawable */
    {					/* uses client-supplied window */
      Pconnid_x_drawable drawable;

                                        /* call Core's realization method */
      (*superclass->core_class.realize)(w, valueMask, attributes);

      drawable.display = XtDisplay(w);	/* attempt to open workstation */
      drawable.drawable_id = XtWindow(w);
      popen_ws(ws->workstation.id, (Pconnid *) &drawable,
	       ws->workstation.type);
    }
  else if (ws->workstation.type == phigs_ws_type_x_tool)
					/* WSType tool: API creates window */
    popen_ws(ws->workstation.id, (Pconnid *) XtDisplay(ws),
	     ws->workstation.type);
  else
    XtAppErrorMsg(XtWidgetToApplicationContext(w), "BadValue",
		  "PEXtWorkstation.Realize", PEXtToolkitError,
		  "Bad workstation type.", (String *) NULL, (Cardinal *) NULL);

  pinq_ws_st(&ws->workstation.state);	/* query WS state and return it */
  if (ws->workstation.state != PWS_ST_WSOP) /* nope, we must die! */
    XtAppErrorMsg(XtWidgetToApplicationContext(w), "BadAlloc",
		  "PEXtWorkstation.Realize", PEXtToolkitError,
		  "Cannot open workstation.",
		  (String *) NULL, (Cardinal *) NULL);

					/* set display update state */
  pset_disp_upd_st(ws->workstation.id,
		   ws->workstation.deferral, ws->workstation.modification);

					/* set HLHS removal mode */
  pset_hlhsr_mode(ws->workstation.id, ws->workstation.hlhsr);
}

/*
				Unrealize

  This callback routine is invoked when the widget is unrealized and all X
  windows associated with the widget are destroyed.  Since the window will
  be destroyed, the workstation must be closed as well.
*/
static void Unrealize(w, client_data, call_data)
     Widget w;
     XtPointer client_data, call_data;
{
  PEXtWorkstationWidget ws = (PEXtWorkstationWidget) w;

  if (ws->workstation.state == PWS_ST_WSOP) /* close workstation, if open */
    {
      punpost_all_structs(ws->workstation.id); /* unpost all structures */
      pclose_ws(ws->workstation.id);	/* close the workstation */
    }
}


/*
				  Destroy

  This is the destroy method for the workstation.  It cleans up any
  resources left in the server by the workstation.  Currently, it simply
  invokes the Unrealize callback.
*/
static void Destroy(w)
     Widget w;
{
  Unrealize(w, (XtPointer) NULL, (XtPointer) NULL);
}

/*
			      ComputePorthole

    ComputePorthole computes the workstation window and viewport needed to
    create a workstation transformation whose result is to make a
    'porthole' into NPC space through the entire DC space contained by the
    X window.
*/
static void ComputePorthole(width, height, gravity, window, viewport)
     int width, height;			/* IN: size of X window in DC */
     XtGravity gravity;			/* IN: fit surface with gravity */
     Plimit *window, *viewport;		/* OUT: workstation transformation */
{
  viewport->x_min = 0.0;
  viewport->y_min = 0.0;
  viewport->x_max = width - 1.0;
  viewport->y_max = height - 1.0;	/* use entirety of DC space */

  if (width > height)			/* window is wider than high */
    {
      window->x_min = 0.0;
      window->x_max = 1.0;		/* use entire width of NPC */

      switch (gravity)
	{
	case NorthGravity:		/* attach porthole to top */
	case NorthEastGravity:
	case NorthWestGravity:
	  window->y_max = 1.0;
	  window->y_min = (width - height)/((float) width);
	  break;

	case SouthGravity:		/* attach porthole to bottom */
	case SouthEastGravity:
	case SouthWestGravity:
	  window->y_min = 0.0;
	  window->y_max = height/((float) width);
	  break;

	default:			/* center porthole */
	  window->y_min = (width - height)/(2.0 * width);
	  window->y_max = 1.0 - window->y_min;
	}
    }
  else					/* window is taller than wide */
    {
      window->y_min = 0.0;
      window->y_max = 1.0;		/* use entire height of NPC */

      switch (gravity)
	{
	case EastGravity:		/* attach porthole to right */
	case NorthEastGravity:
	case SouthEastGravity:
	  window->x_max = 1.0;
	  window->x_min = (height - width)/((float) height);
	  break;

	case WestGravity:		/* attach porthole to left */
	case SouthWestGravity:
	case NorthWestGravity:
	  window->x_min = 0.0;
	  window->x_max = width/((float) height);
	  break;

	default:			/* center porthole */
	  window->x_min = (height - width)/(2.0 * height);
	  window->x_max = 1.0 - window->x_min;
	}
    }
}

/*
			     ComputeContainer

    ComputeContainer computes the workstation window and viewport needed to
    create a workstation transformation whose result is to 'contain' the
    entirety of NPC space in a portion of the X window.
*/
static void ComputeContainer(width, height, gravity, window, viewport)
     int width, height;			/* IN: size of X window in DC */
     XtGravity gravity;			/* IN: fit surface with gravity */
     Plimit *window, *viewport;		/* OUT: workstation transformation */
{
  window->x_min = window->y_min = 0.0;
  window->x_max = window->y_max = 1.0;	/* use entirety of NPC space */

  if (width > height)			/* window is wider than high */
    {
      viewport->y_min = 0.0;
      viewport->y_max = height - 1.0;	/* use entire height of DC */

      switch (gravity)
	{
	case EastGravity:		/* attach container to right */
	case NorthEastGravity:
	case SouthEastGravity:
	  viewport->x_max = width - 1.0;
	  viewport->x_min = width - height;
	  break;

	case WestGravity:		/* attach container to left */
	case SouthWestGravity:
	case NorthWestGravity:
	  viewport->x_min = 0.0;
	  viewport->x_max = height - 1.0;
	  break;

	default:			/* center container */
	  viewport->x_min = (width - height)/2.0;
	  viewport->x_max = width - viewport->x_min;
	}
    }
  else					/* window is taller than wide */
    {
      viewport->x_min = 0.0;
      viewport->x_max = width - 1.0;	/* use entire width of DC */

      switch (gravity)
	{
	case NorthGravity:		/* attach container to top */
	case NorthEastGravity:
	case NorthWestGravity:
	  viewport->y_min = height - width;
	  viewport->y_max = height - 1.0;
	  break;

	case SouthGravity:		/* attach container to bottom */
	case SouthEastGravity:
	case SouthWestGravity:
	  viewport->y_min = 0.0;
	  viewport->y_max = width - 1.0;
	  break;

	default:			/* center container */
	  viewport->y_min = (height - width)/2.0;
	  viewport->y_max = height - viewport->y_min;
	}
    }
}


/*
				 Resize

    This method resizes the workstation.  There are several types of resize
    behaviours, dependant upon the resize policy.

    Policy			Behaviour
    PEXtResizeNone		Nothing is done.
    PEXtResizeCallback		Client supplied callbacks are called
    PEXtResizePorthole		Given the new window geometry, construct
    				the largest porthole into NPC space that
				will fit into the X window.  The gravity is
				used to orient the porthole in NPC space.
    PEXtResizeContainer		Given the new window geometry, contruct the
    				largest portion of the window that will
				contain all of NPC space.  Orient NPC space
				relative to the specified gravity.
*/
static void Resize(w)
     Widget w;
{
  PEXtWorkstationWidget ws = (PEXtWorkstationWidget) w;
  Plimit wks_window;			/* NPC space */
  Plimit wks_viewport;			/* DC space */
  PEXtWorkstationCallbackStruct cs;
  
  if (!XtIsRealized(w))			/* do nothing if not realized */
    return;

  switch (ws->workstation.resize_policy)
    {
    case PEXtResizeNone:
      break;

    case PEXtResizeCallback:
      cs.reason = PEXtCR_RESIZE;
      cs.width = ws->core.width;
      cs.height = ws->core.height;
      cs.event = (XEvent *) NULL;
      XtCallCallbacks(w, PEXtNresizeCallback, (XtPointer) &cs);
      break;

    case PEXtResizePorthole:		/* largest porthole into NPC space */
      ComputePorthole(ws->core.width, ws->core.height,
		      ws->workstation.resize_gravity,
		      &wks_window, &wks_viewport);

					/* adjust workstation transformation */
      pset_ws_vp(ws->workstation.id, &wks_viewport);
      pset_ws_win(ws->workstation.id, &wks_window);

					/* perform appropriate update */
      Redisplay(w, (XExposeEvent *) NULL);
      break;

    case PEXtResizeContainer:		/* contain NPC space in largest */
      ComputeContainer(ws->core.width, ws->core.height,
		       ws->workstation.resize_gravity,
		       &wks_window, &wks_viewport);

					/* adjust workstation transformation */
      pset_ws_vp(ws->workstation.id, &wks_viewport);
      pset_ws_win(ws->workstation.id, &wks_window);

					/* perform appropriate update */
      Redisplay(w, (XExposeEvent *) NULL);
      break;

    default:
      XtAppErrorMsg(XtWidgetToApplicationContext(w), "BadValue",
		    "PEXtWorkstation.Realize", PEXtToolkitError,
		    "Bad resize policy.",
		    (String *) NULL, (Cardinal *) NULL);
    }      
}

/*
				Redisplay

  This method traverses the structures posted on a given workstation,
  depending on the traversal policy of the workstation.  The Redisplay
  method is invoked both by the Intrinsics as a result of expose events on
  the workstation's window and by the Resize method, which may require a
  traversal after changing the workstation transformation.

  Policy		Behaviour
  PEXtTraverseNone	Do nothing
  PEXtTraverseCallback	Invoke client's callback routines upon exposure
  PEXtTraverseRedraw	REDRAW ALL STRUCTURES
*/
static void Redisplay(w, event)
     Widget w;
     XExposeEvent *event;
{
  if (!XtIsRealized(w))
    return;

  Traverse(w, event, (String *) NULL, (Cardinal *) NULL);
}

/*
				SetValues

  This is the SetValues method for the workstation.  It must adjust PHIGS
  workstation state in the server based upon the changing of instance
  variables.
*/
static Boolean SetValues(current, request, new, args, num_args)
     Widget current, request, new;
     ArgList args;
     Cardinal *num_args;
{
  PEXtWorkstationWidget current_ws = (PEXtWorkstationWidget) current;
  PEXtWorkstationWidget new_ws = (PEXtWorkstationWidget) new;
  register Boolean do_redisplay = False;

  if (!XtIsRealized(new_ws))
    return do_redisplay;
					/* cannot change ID on open WS */
  if (current_ws->workstation.id != new_ws->workstation.id)
    {
      new_ws->workstation.id = current_ws->workstation.id;
      XtAppWarning(XtWidgetToApplicationContext(current),
		   WSID_WARNING);
    }
					/* cannot change type on open WS */
  if (current_ws->workstation.type != new_ws->workstation.type)
    {
      new_ws->workstation.type = current_ws->workstation.type;
      XtAppWarning(XtWidgetToApplicationContext(current),
		   WSTYPE_WARNING);
    }
					/* new HLHSR mode */
  if (current_ws->workstation.hlhsr != new_ws->workstation.hlhsr)
    pset_hlhsr_mode(new_ws->workstation.id, new_ws->workstation.hlhsr);
  
  if (current_ws->workstation.deferral != new_ws->workstation.deferral ||
      current_ws->workstation.modification != new_ws->workstation.modification)
    pset_disp_upd_st(new_ws->workstation.id,
		     new_ws->workstation.deferral,
		     new_ws->workstation.modification);

}

/*
			      QueryGeometry

  This is the QueryGeometry method for the workstation.  It decides whether
  or not the given geometry request can be satisfied.
*/
static XtGeometryResult QueryGeometry(w, request, answer)
     Widget w;
     XtWidgetGeometry *request, *answer;
{
  PEXtWorkstationWidget ws = (PEXtWorkstationWidget) w;

  answer->request_mode |= CWWidth | CWHeight;
  answer->width = 1;			/* we can be this small, sure... */
  answer->height = 1;

  if (((answer->request_mode & request->request_mode) != answer->request_mode)
      || (request->width < answer->width)
      || (request->height < answer->height))
    return XtGeometryAlmost;
  else if ((request->width == answer->width) &&
	   (request->height == answer->height))
    return XtGeometryNo;
  else
    return XtGeometryYes;
}

/*
  Traverse()

  This action procedure invokes traversal of the structure(s) on the
  workstation.  It is given as a separate action so it can be invoked by
  subclasses (perhaps it should be a method?).
*/
static void Traverse(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  PEXtWorkstationWidget ws = (PEXtWorkstationWidget) w;
  PEXtWorkstationCallbackStruct cs;

  switch (ws->workstation.traversal_policy)
    {
    case PEXtTraverseNone:
      break;

    case PEXtTraverseCallback:
      cs.reason = PEXtCR_TRAVERSE;
      cs.width = 0;
      cs.height = 0;
      cs.event = (XEvent *) event;
      XtCallCallbacks(w, PEXtNtraversalCallback, (XtPointer) &cs);
      break;

    case PEXtTraverseRedraw:		/* redraw all structures */
      predraw_all_structs(ws->workstation.id, ws->workstation.control);
      break;
 
    default:
      XtAppErrorMsg(XtWidgetToApplicationContext(w), "BadValue",
		    "PEXtWorkstation.Redisplay", PEXtToolkitError,
		    "Bad traversal policy.",
		    (String *) NULL, (Cardinal *) NULL);
    }      
}

/*
  PEXtCreateWorkstation

  Workstation widget convenience creation function.
*/
Widget PEXtCreateWorkstation(parent, name, args, n)
     Widget parent;
     String name;
     ArgList args;
     Cardinal n;
{
  return XtCreateWidget(name, pextWorkstationWidgetClass, parent, args, n);
}

/*
  PEXtWorkstation

  Convenience function to return the workstation ID of a workstation widget
*/
Pint PEXtWorkstation(w)
     Widget w;
{
  PEXtWorkstationWidget ws = (PEXtWorkstationWidget) w;

  return ws->workstation.id;
}
