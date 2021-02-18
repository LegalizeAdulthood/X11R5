/*
		       C o n v e r t e r s . c

    $Header: Converters.c,v 1.0 91/10/04 17:00:55 rthomson Exp $

    This file contains resource converters for PEX resources.

    Author: Rich Thomson
    Date: Wed May 23 21:17:48 MDT 1990

    Editing History:
    10-Jul-90	RAT: Added resource converters for PHIGS types PControlFlag
		(workstation update control flag), PDefMode (workstation
		deferral mode), and PMod (workstation modification mode).

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
#include <ctype.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include <X11/Xos.h>

#ifdef ES_PICK_EXTENSION
#include <X11/extensions/XPick.h>
#endif

#include "PEXt.h"
#include "StringDefs.h"
#include "Converters.h"



/*
			     Private Variables
*/
static char buffer[1024];		/* for string conversions */


/*
  All these error strings should go into some kind of error database;
  perhaps appended to /usr/lib/X11/XErrorDB.  Until the toolkit allows one
  to merge an error database into the existing error database without
  modifying the file, these #define's will probably have to remain here.

  For now, I've #defined them all in one place, both to help check for
  compile errors and to isolate the changes to one region.
*/

#define wP_cvtStringToInteriorStyle \
  "String to Interior Style conversion needs no extra arguments"
#define wP_cvtInteriorStyleToString \
  "Interior Style to String conversion needs no extra arguments"
#define wP_cvtStringToCullMode \
  "String to Face Culling Mode conversion needs no extra arguments"
#define wP_cvtCullModeToString \
  "Face Culling Mode to String conversion needs no extra arguments"
#define wP_cvtStringToGeneralColor \
  "String to General Color conversion needs screen and colormap arguments"
#define wP_cvtGeneralColorToString \
  "General Color to String conversion needs no extra arguments"
#define wP_cvtStringToColorBundle \
  "String to Color Bundle conversion needs screen and colormap arguments"
#define wP_cvtColorBundleToString \
  "Color Bundle to String conversion needs no extra arguments"
#define wP_cvtStringToProjectionType \
  "String to Projection Type conversion needs no extra arguments"
#define wP_cvtProjectionTypeToString \
  "Projection Type to String conversion needs no extra arguments"
#define wP_cvtStringToReflectanceEquation \
  "String to Reflectance Equation conversion needs no extra arguments"
#define wP_cvtReflectanceEquationToString \
  "Reflectance Equation to String conversion needs no extra arguments"
#define wP_cvtStringToShadingMethod \
  "String to Shading Method conversion needs no extra arguments"
#define wP_cvtShadingMethodToString \
  "Shading Method to String conversion needs no extra arguments"
#define wP_cvtStringToEdgeFlag \
  "String to Edge Flag conversion needs no extra arguments"
#define wP_cvtEdgeFlagToString \
  "Edge Flag to String conversion needs no extra arguments"
#define wP_cvtStringToLineType \
  "String to Line Type conversion needs no extra arguments"
#define wP_cvtLineTypeToString \
  "Line Type to String conversion needs no extra arguments"
#define wP_cvtStringToDistinguishMode \
  "String to Distinguish Mode conversion needs no extra arguments"
#define wP_cvtDistinguishModeToString \
  "Distinguish Mode to String conversion needs no extra arguments"
#define wP_cvtStringToPoint3 \
  "String to Point3 conversion needs no extra arguments"
#define wP_cvtPoint3ToString \
  "Point3 to String conversion needs no extra arguments"
#define wP_cvtStringToPoint \
  "String to Point conversion needs no extra arguments"
#define wP_cvtPointToString \
  "Point to String conversion needs no extra arguments"
#define wP_cvtStringToVector3 \
  "String to Vector3 conversion needs no extra arguments"
#define wP_cvtVector3ToString \
  "Vector3 to String conversion needs no extra arguments"
#define wP_cvtStringToVector \
  "String to Vector conversion needs no extra arguments"
#define wP_cvtVectorToString \
  "Vector to String conversion needs no extra arguments"
#define wP_cvtStringToLimit \
  "String to Limit conversion needs no extra arguments"
#define wP_cvtLimitToString \
  "Limit to String conversion needs no extra arguments"
#define wP_cvtStringToLimit3 \
  "String to Limit3 conversion needs no extra arguments"
#define wP_cvtLimit3ToString \
  "Limit3 to String conversion needs no extra arguments"
#define wP_cvtStringToIntList \
  "String to Int List conversion needs no extra arguments"
#define wP_cvtIntListToString \
  "Int List to String conversion needs no extra arguments"
#define wP_cvtStringToLightSourceType \
  "String to Light Source Type conversion needs no extra arguments"
#define wP_cvtLightSourceTypeToString \
  "Light Source Type to String conversion needs no extra arguments"
#define wP_cvtStringToClipIndicator \
  "String to Clip Indicator conversion needs no extra arguments"
#define wP_cvtClipIndicatorToString \
  "Clip Indicator to String conversion needs no extra arguments"
#define wP_cvtStringToDepthCueMode \
  "String to Depth Cue Mode conversion needs no extra arguments"
#define wP_cvtDepthCueModeToString \
  "Depth Cue Mode to String conversion needs no extra arguments"
#define wP_cvtStringToControlFlag \
  "String to Control Flag conversion needs no extra arguments"
#define wP_cvtControlFlagToString \
  "Control Flag to String conversion needs no extra arguments"
#define wP_cvtStringToDeferralMode \
  "String to Deferral Mode conversion needs no extra arguments"
#define wP_cvtDeferralModeToString \
  "Deferral Mode to String conversion needs no extra arguments"
#define wP_cvtStringToModificationMode \
  "String to Modification Mode conversion needs no extra arguments"
#define wP_cvtModificationModeToString \
  "Modification Mode to String conversion needs no extra arguments"
#define wP_cvtStringToHLHSRMode \
  "String to HLHSR Mode conversion needs no extra arguments"
#define wP_cvtHLHSRModeToString \
  "HLHSR Mode to String conversion needs no extra arguments"
#define wP_cvtStringToWorkstationType \
  "String to Workstation Type conversion needs no extra arguments"
#define wP_cvtWorkstationTypeToString \
  "Workstation Type to String conversion needs no extra arguments"

#define wP_cvtStringToPickMode \
  "String to Pick Mode conversion needs no extra arguments"
#define wP_cvtPickModeToString \
  "Pick Mode to String conversion needs no extra arguments"
#define wP_cvtStringToPickHighlightMode \
  "String to Pick Highlighting Mode conversion needs no extra arguments"
#define wP_cvtPickHighlightModeToString \
  "Pick Highlighting Mode to String conversion needs no extra arguments"
#define wP_cvtStringToGravity \
  "String to Gravity conversion needs no extra arguments"
#define wP_cvtStringToResizePolicy \
  "String to Resize Policy conversion needs no extra arguments"
#define wP_cvtResizePolicyToString \
  "Resize Policy to String conversion needs no extra arguments"
#define wP_cvtStringToTraversalPolicy \
  "String to Traversal Policy conversion needs no extra arguments"
#define wP_cvtTraversalPolicyToString \
  "Traversal Policy to String conversion needs no extra arguments"


					/* conversion errors */
#define cE_cvtStringToInteriorStyle \
  "Cannot convert \"%s\" to Interior Style"
#define cE_cvtInteriorStyleToString \
  "Cannot convert interior style %d to String"
#define cE_cvtStringToCullMode \
  "Cannot convert \"%s\" to Cull Mode"
#define cE_cvtCullModeToString \
  "Cannot convert face culling mode %d to String"
#define cE_cvtStringToGeneralColor_tooFewComponents \
  "Too few components for color model %s"
#define cE_cvtStringToGeneralColor_componentRange \
  "Color component out of range [0,1]"
#define cE_cvtStringToGeneralColor_XColor \
  "Bad X color specification."
#define cE_cvtGeneralColorToString \
  "Bad color model in General Color to String conversion"
#define cE_cvtStringToColorBundle \
  "Bad color model for String to Color Bundle conversion"
#define cE_cvtStringToProjectionType \
  "Cannot convert \"%s\" to Projection Type"
#define cE_cvtProjectionTypeToString \
  "Cannot convert projection type %d to string"
#define cE_cvtStringToReflectanceEquation \
  "Cannot convert \"%s\" to Reflectance Equation"
#define cE_cvtReflectanceEquationToString \
  "Cannot convert reflectance equation %d to String"
#define cE_cvtStringToShadingMethod \
  "Cannot convert \"%s\" to Shading Method"
#define cE_cvtShadingMethodToString \
  "Cannot convert shading method %d to String"
#define cE_cvtStringToEdgeFlag \
  "Cannot convert \"%s\" to Edge Flag"
#define cE_cvtEdgeFlagToString \
  "Cannot convert edge flag %d to String"
#define cE_cvtStringToLineType \
  "Cannot covnert \"%s\" to Line Type"
#define cE_cvtLineTypeToString \
  "Cannot convert line type %d to String"
#define cE_cvtStringToDistinguishMode \
  "Cannot covnert \"%s\" to Disgintuish Mode"
#define cE_cvtDistinguishModeToString \
  "Cannot convert face distinguishing mode %d to String"
#define cE_cvtStringToPoint3 \
  "Cannot convert \"%s\" to Point3"
#define cE_cvtStringToPoint \
  "Cannot convert \"%s\" to Point"
#define cE_cvtStringToVector3 \
  "Cannot convert \"%s\" to Vector3"
#define cE_cvtStringToVector \
  "Cannot convert \"%s\" to Vector"
#define cE_cvtStringToLimit \
  "Cannot convert \"%s\" to Limit"
#define cE_cvtStringToLimit3 \
  "Cannot convert \"%s\" to Limit3"
#define cE_cvtStringToIntList \
  "Cannot convert \"%s\" to Int List"
#define cE_cvtStringToLightSourceType \
  "Cannot convert \"%s\" to Light Source Type"
#define cE_cvtLightSourceTypeToString \
  "Cannot convert light source type %d to String"
#define cE_cvtStringToClipIndicator \
  "Cannot convert clip flag %d to String"
#define cE_cvtStringToPickMode \
  "Cannot convert \"%s\" to Pick Mode"
#define cE_cvtPickModeToString \
  "Cannot convert Pick Mode %d to String"
#define cE_cvtStringToPickHighlightMode \
  "Cannot convert \"%s\" to Pick Highlighting Mode"
#define cE_cvtPickHighlightModeToString \
  "Cannot convert pick highlighting mode %d to String"
#define cE_cvtStringToDepthCueMode \
  "Cannot convert \"%s\" to Depth Cue Mode"
#define cE_cvtDepthCueModeToString \
  "Cannot convert depth cue mode %d to String"
#define cE_cvtStringToControlFlag \
  "Cannot convert \"%s\" to Control Flag"
#define cE_cvtStringToDeferralMode \
  "Cannot convert \"%s\" to Deferral Mode"
#define cE_cvtStringToModificationMode \
  "Cannot convert \"%s\" to Modification Mode"
#define cE_cvtStringToHLHSRMode \
  "Cannot convert \"%s\" to HLHSR Mode"
#define cE_cvtStringToWorkstationType \
  "Cannot convert \"%s\" to Workstation Type"
#define cE_cvtWorkstationTypeToString \
  "Cannot convert workstation type %d to String"
#define cE_cvtStringToGravity \
  "Cannot convert \"%s\" to Gravity"
#define cE_cvtStringToResizePolicy \
  "Cannot convert \"%s\" to Resize Policy"
#define cE_cvtResizePolicyToString \
  "Cannot convert Resize Policy %d to String"
#define cE_cvtStringToTraversalPolicy \
  "Cannot convert \"%s\" to Traversal Policy"
#define cE_cvtTraversalPolicyToString \
  "Cannot convert Traversal Policy %d to String"



/*
			 Globally Available Quarks
*/
/*
  Quarks corresponding to resource strings used in the converters
*/
XrmQuark PEXtQEtrue, PEXtQEfalse, PEXtQEon, PEXtQEoff, PEXtQEyes, PEXtQEno,
  PEXtQE1, PEXtQE0, PEXtQEhollow, PEXtQEsolid, PEXtQEpattern, PEXtQEhatch,
  PEXtQEempty, PEXtQpextDefaultInteriorStyle, PEXtQEnone, PEXtQEcullBackFace,
  PEXtQEcullFrontFace, PEXtQpextDefaultCullMode, PEXtQEindirect, PEXtQErgb,
  PEXtQEhls, PEXtQEcie, PEXtQEhsv, PEXtQpextDefaultGeneralColor,
  PEXtQEparallel, PEXtQEperspective, PEXtQpextDefaultProjectionType,
  PEXtQEambient, PEXtQEdiffuse, PEXtQEspecular,
  PEXtQpextDefaultReflectanceEquation, PEXtQEcolor, PEXtQEdotProduct,
  PEXtQEnormal, PEXtQpextDefaultShadingMethod, PEXtQEsolid, PEXtQEdash,
  PEXtQEdot, PEXtQEdashDot, PEXtQpextDefaultLineType,
  PEXtQpextDefaultEdgeFlag, PEXtQpextDefaultDistinguishMode, PEXtQpextOrigin,
  PEXtQEdirectional, PEXtQEpositional, PEXtQEspot,
  PEXtQpextDefaultLightSourceType, PEXtQpextDefaultClipLimits, PEXtQEclip,
  PEXtQEnoClip, PEXtQpextDefaultClipIndicator, PEXtQpextUnitVectorX,
  PEXtQpextUnitVectorY, PEXtQpextUnitVectorZ, PEXtQpextDefaultHLHSRMode,
  PEXtQEzBuffer, PEXtQpextDefaultControlFlag, PEXtQEconditionally,
  PEXtQEalways, PEXtQpextDefaultDeferralMode, PEXtQEasap,
  PEXtQEasSoonAsPossible, PEXtQEbnig, PEXtQEbeforeNextInteractionGlobally,
  PEXtQEbnil, PEXtQEbeforeNextInteractionLocally, PEXtQEasti,
  PEXtQEatSomeTime, PEXtQEwait, PEXtQEwhenApplicationRequestsIt,
  PEXtQpextDefaultModificationMode, PEXtQEnive,
  PEXtQEnoImmediateVisualEffect, PEXtQEuwor, PEXtQEtool, PEXtQEdrawable,
  PEXtQpextDefaultWorkstationType, PEXtQEupdateWithoutRegeneration,
  PEXtQEuqum, PEXtQEuseQuickUpdateMethods, PEXtQEallowed, PEXtQEsuppressed,
  PEXtQpextDefaultDepthCueMode, PEXtQEnorthWest, PEXtQEnorth, PEXtQEnorthEast,
  PEXtQEwest, PEXtQEcenter, PEXtQEeast, PEXtQEsouthWest, PEXtQEsouth,
  PEXtQEsouthEast, PEXtQEleft, PEXtQEtop, PEXtQEright, PEXtQEbottom,
  PEXtQpextDefaultGravity, PEXtQEcallback, PEXtQEporthole, PEXtQEcontainer,
  PEXtQEredraw, PEXtQpextDefaultResizePolicy, PEXtQpextDefaultTraversalPolicy,
  PEXtQExtDefaultForeground, PEXtQExtDefaultBackground;

#ifdef ES_PICK_EXTENSION
XrmQuark				/* Picking Extension Quarks */
  PEXtQEpickFirst, PEXtQEpickLast, PEXtQEfirst, PEXtQElast, PEXtQEnear,
  PEXtQEfar, PEXtQEpickFirst, PEXtQEpickLast, PEXtQEpickNear,
  PEXtQEpickFar, PEXtQpextDefaultPickMode, PEXtQEitem, PEXtQEcommand,
  PEXtQEpickHighlightItem, PEXtQEpickHighlightCommand,
  PEXtQEpickHighlightOff, PEXtQpextDefaultPickHighlightMode;
#endif



/*
  PEXtLowerCaseStrip

  Convert source string to lower case destination, removing white space.
*/
String PEXtLowerCaseStrip(source)
     String source;
{
  static char lowerName[1024];
  register char ch;
  register String dest = lowerName;

  while (ch = *source++)
    if (!isspace(ch))
      *dest++ = isupper(ch) ? (ch | 0x20) : ch;

  *dest = 0;

  return lowerName;
}

/*
  CvtStringToInteriorStyle -- convert string to interior style
*/
/*ARGSUSED*/
static Boolean CvtStringToInteriorStyle(dpy, args, num_args,
					fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pint_style is;
  register XrmQuark q;

  if (*num_args)
    WrongParameters("cvtStringToInteriorStyle", wP_cvtStringToInteriorStyle);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEsolid || q == PEXtQpextDefaultInteriorStyle)
    is = PSTYLE_SOLID;
  else if (q == PEXtQEhollow)
    is = PSTYLE_HOLLOW;
  else if (q == PEXtQEempty)
    is = PSTYLE_EMPTY;
  else
    {
      sprintf(buffer, cE_cvtStringToInteriorStyle, (String) fromVal->addr);
      ConversionError("cvtStringToInteriorStyle", buffer);
      return False;
    }
  
  DONE(is, Pint_style);
}

/*
  CvtInteriorStyleToString -- convert interior style to string
*/
static Boolean CvtInteriorStyleToString(dpy, args, num_args,
					fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args)
    WrongParameters("cvtInteriorStyleToString", wP_cvtInteriorStyleToString);

  switch (* ((Pint_style *) fromVal->addr))
    {
    case PSTYLE_SOLID: DONESTR("Solid"); break;
    case PSTYLE_HOLLOW: DONESTR("Hollow"); break;
    case PSTYLE_EMPTY: DONESTR("Empty"); break;

    default:
      sprintf(buffer, cE_cvtInteriorStyleToString, *((int *) fromVal->addr));
      ConversionError("cvtInteriorStyleToString", buffer);
      return False;
    }
  /* NOT REACHED */
}

/*
  CvtStringToCullMode -- convert string to face culling mode
*/
/*ARGSUSED*/
static Boolean CvtStringToCullMode(dpy, args, num_args,
				   fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pcull_mode cm;
  register XrmQuark q;

  if (*num_args)
    WrongParameters("cvtStringToCullMode", wP_cvtStringToCullMode);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEnone || q == PEXtQpextDefaultCullMode)
    cm = PCULL_NONE;
  else if (q == PEXtQEcullFrontFace)
    cm = PCULL_FRONTFACE;
  else if (q == PEXtQEcullBackFace)
    cm = PCULL_BACKFACE;
  else
    {
      sprintf(buffer, cE_cvtStringToCullMode, (String) fromVal->addr);
      ConversionError("cvtStringToCullMode", buffer);
      return False;
    }
  
  DONE(cm, Pcull_mode);
}

/*
  CvtCullModeToString -- convert face culling mode to string
*/
static Boolean CvtCullModeToString(dpy, args, num_args,
				   fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args)
    WrongParameters("cvtCullModeToString", wP_cvtCullModeToString);

  switch (* ((Pcull_mode *) fromVal->addr))
    {
    case PCULL_NONE: DONESTR("None"); break;
    case PCULL_FRONTFACE: DONESTR("Front Face"); break;
    case PCULL_BACKFACE: DONESTR("Back Face"); break;

    default:
      sprintf(buffer, cE_cvtCullModeToString, *((int *) fromVal->addr));
      ConversionError("cvtCullModeToString", buffer);
      return False;
    }
  /* NOT REACHED */
}

/*
  CvtStringToGeneralColor

  This routine convert a string resources to PHIGS generalized color resource.
  A generalized color can be one of the following:

        An X color specification
	An indirect color specification
	An RGB floating point color specification
	PEXtDefaultGeneralColor (which results in "indirect, 1")

  A PHIGS color specification consists of a model name followed by a comma
  separated list of data values to complete the specification.  Ex:

	indirect, 1
	rgb, 0.4, 0.5, 0.6		(r, g, b)
	hls, 0.2, 0.2, 0.2		(h, l, s)
	hlv, 0.2, 0.2, 0.2		(h, l, v)
	cie, 0.4, 0.5, 1.0		(c, i, e)

  TODO:
    Converter should examine RGB mask of pixel value before ripping it out of
    the pixel used for XtDefaultForeground, XtDefaultBackground.

    Converter should be able to sense missing arguments on PHIGS
    specifications.
*/
/*ARGSUSED*/
static Boolean CvtStringToGeneralColor(dpy, args, num_args,
				       fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pgcolr gc;
  register XrmQuark q;
  register String comma, lowerName;

  if (*num_args != 2)
    WrongParameters("cvtStringToGeneralColor", wP_cvtStringToGeneralColor);
  
  lowerName = PEXtLowerCaseStrip((String) fromVal->addr);

  if (comma = index(lowerName, ','))	/* PHIGS color specification */
    {
      *comma++ = '\0';
      q = XrmStringToQuark(lowerName);
      if (q == PEXtQEindirect)		/* indexed color specification */
	{
	  gc.type = PINDIRECT;
	  while (isspace(*comma))	/* skip leading white space */
	    comma++;
	  gc.val.ind = atoi(comma);
	}
      else
	{
	  if (q == PEXtQErgb)		/* RGB specification */
	    gc.type = PMODEL_RGB;
	  else if (q == PEXtQEhls)	/* HLS specification */
	    gc.type = PMODEL_HLS;
	  else if (q == PEXtQEcie)	/* CIE specification */
	    gc.type = PMODEL_CIELUV;
	  else if (q == PEXtQEhsv)	/* HSV specification */
	    gc.type = PMODEL_HSV;

	  if (sscanf(comma, "%f, %f, %f",
		     &gc.val.general.x, &gc.val.general.y, &gc.val.general.z)
	      != 3)
	    {
	      sprintf(buffer, cE_cvtStringToGeneralColor_tooFewComponents, 
		      XrmQuarkToString(q));
	      ConversionError("cvtStringToGeneralColor", buffer);
	      return False;
	    }

	  if (gc.val.general.x < 0.0 || gc.val.general.x > 1.0 ||
	      gc.val.general.y < 0.0 || gc.val.general.y > 1.0 ||
	      gc.val.general.z < 0.0 || gc.val.general.z > 1.0)
	    {
	      ConversionError("cvtStringToGeneralColor",
			      cE_cvtStringToGeneralColor_componentRange);
	      return False;
	    }
	}
    }
  else if ((q = XrmStringToQuark(lowerName)) == PEXtQpextDefaultGeneralColor)
    {					/* default general color */
      gc.type = PINDIRECT;
      gc.val.ind = 1;
    }
  else					/* use Xt style color spec */
    {
      XColor database, screen;

      gc.type = PMODEL_RGB;

      if (q == PEXtQExtDefaultBackground)
	gc.val.general.x = gc.val.general.y = gc.val.general.z = 1.;
      else if (q == PEXtQExtDefaultForeground)
	gc.val.general.x = gc.val.general.y = gc.val.general.z = 0.;
      else if (lowerName[0] == '#')	/* #rgb spec */
	{
	  String hex_digits = "0123456789abcdef";
	  String digits = lowerName+1;
	  int length = strlen(digits);
	  int i;
	  int channel;

	  if (length % 3)		/* need #rgb, #rrggbb, ... */
	    ConversionError("cvtStringToGeneralColor",
			    cE_cvtStringToGeneralColor_XColor);

	  channel = 0;
	  for (i = 0; i < length/3; i++)
	    channel = (channel << 4) +
	      (int) (index(hex_digits, *digits++) - hex_digits);

	  gc.val.general.x = ((float) channel)/((1 << (length/3 * 4))-1);

	  channel = 0;
	  for (i = 0; i < length/3; i++)
	    channel = (channel << 4) +
	      (int) (index(hex_digits, *digits++) - hex_digits);

	  gc.val.general.y = ((float) channel)/((1 << (length/3 * 4))-1);

	  channel = 0;
	  for (i = 0; i < length/3; i++)
	    channel = (channel << 4) +
	      (int) (index(hex_digits, *digits++) - hex_digits);

	  gc.val.general.z = ((float) channel)/((1 << (length/3 * 4))-1);
	}
      else if (XLookupColor(dpy, *((Colormap *) args[1].addr),
			    (String) fromVal->addr, &database, &screen))
	{
	  gc.val.general.x = database.red/65535.;
	  gc.val.general.y = database.green/65535.;
	  gc.val.general.z = database.blue/65535.;
	}
      else
	ConversionError("cvtStringToGeneralColor",
			cE_cvtStringToGeneralColor_XColor);
    }
  DONE(gc, Pgcolr);
}

/*
  CvtGeneralColorToString -- convert general color to string
*/
static Boolean CvtGeneralColorToString(dpy, args, num_args,
				       fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  register Pgcolr *color = (Pgcolr *) fromVal->addr;

  if (*num_args)
    WrongParameters("cvtGeneralColorToString", wP_cvtGeneralColorToString);

  switch (color->type)
    {
    case PINDIRECT:
      sprintf(buffer, "Indirect, %d", color->val.ind);
      break;

    case PMODEL_RGB:
      sprintf(buffer, "RGB, %f, %f, %f", color->val.general.x,
	      color->val.general.y, color->val.general.z);
      break;

    case PMODEL_CIELUV:
      sprintf(buffer, "CIE, %f, %f, %f", color->val.general.x,
	      color->val.general.y, color->val.general.z);
      break;

    case PMODEL_HSV:
      sprintf(buffer, "HSV, %f, %f, %f", color->val.general.x,
	      color->val.general.y, color->val.general.z);
      break;

    case PMODEL_HLS:
      sprintf(buffer, "HLS, %f, %f, %f", color->val.general.x,
	      color->val.general.y, color->val.general.z);
      break;

    default:
      ConversionError("cvtGeneralColorToString", cE_cvtGeneralColorToString);
      return False;
    }
  DONESTR(buffer);
}

/*
  CvtStringToColorBundle -- convert string to color bundle table entry

  Same as CvtStringToGeneralColor, but indexed colors are not allowed and the
  color model is ignored (must be set with PSetColourModel).

*/
/*ARGSUSED*/
static Boolean CvtStringToColorBundle(dpy, args, num_args,
				      fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pcolr_rep cb;
  Pgcolr gc;
  XrmValue interVal;
  
  if (*num_args != 2)
    WrongParameters("cvtStringToColorBundle", wP_cvtStringToColorBundle);

  interVal.size = sizeof(Pgcolr);
  interVal.addr = (XtPointer) &gc;
  if (CvtStringToGeneralColor(dpy, args, num_args, fromVal, &interVal))
    {
      switch (gc.type)
	{
	case PMODEL_RGB:
	  cb.rgb.red = gc.val.general.x;
	  cb.rgb.green = gc.val.general.y;
	  cb.rgb.blue = gc.val.general.z;
	  break;

	case PMODEL_CIELUV:
	  cb.cieluv.cieluv_x = gc.val.general.x;
	  cb.cieluv.cieluv_y = gc.val.general.y;
	  cb.cieluv.cieluv_y_lum = gc.val.general.z;
	  break;

	case PMODEL_HSV:
	  cb.hsv.hue = gc.val.general.x;
	  cb.hsv.satur = gc.val.general.y;
	  cb.hsv.value = gc.val.general.z;

	case PMODEL_HLS:
	  cb.hls.hue = gc.val.general.x;
	  cb.hls.lightness = gc.val.general.y;
	  cb.hls.satur = gc.val.general.y;
	  break;
	  
	case PINDIRECT:
	default:
	  ConversionError("cvtStringToColorBundle", cE_cvtStringToColorBundle);
	  return False;
	}
    }
  else
    return False;			/* CvtStringToGeneralColor failed */

  DONE(cb, Pcolr_rep);
}

/*
  CvtStringToProjectionType -- convert string to projection type
*/
/*ARGSUSED*/
static Boolean CvtStringToProjectionType(dpy, args, num_args,
					 fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pproj_type pt;
  register XrmQuark q;

  if (*num_args)
    WrongParameters("cvtStringToProjectionType", wP_cvtStringToProjectionType);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEparallel || q == PEXtQpextDefaultProjectionType)
    pt = PTYPE_PARAL;
  else if (q == PEXtQEperspective)
    pt = PTYPE_PERSPECT;
  else
    {
      sprintf(buffer, cE_cvtStringToProjectionType, (String) fromVal->addr);
      ConversionError("cvtStringToProjectionType", buffer);
      return False;
    }
  
  DONE(pt, Pproj_type);
}

/*
  CvtProjectionTypeToString -- convert projection type to string
*/
static Boolean CvtProjectionTypeToString(dpy, args, num_args,
					 fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args)
    WrongParameters("cvtProjectionTypeToString", wP_cvtProjectionTypeToString);

  switch (* ((Pproj_type *) fromVal->addr))
    {
    case PTYPE_PARAL:
      DONESTR("Parallel");
      break;

    case PTYPE_PERSPECT:
      DONESTR("Perspective");
      break;

    default:
      sprintf(buffer, cE_cvtProjectionTypeToString, *((int *) fromVal->addr));
      ConversionError("cvtProjectionTypeToString", buffer);
      return False;
    }
  /* NOT REACHED */
}

/*
  CvtStringToReflectanceEquation -- convert string to reflectance equation
*/
/*ARGSUSED*/
static Boolean CvtStringToReflectanceEquation(dpy, args, num_args,
					      fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pint re;
  register XrmQuark q;

  if (*num_args)
    WrongParameters("cvtStringToReflectanceEquation",
		    wP_cvtStringToReflectanceEquation);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQpextDefaultReflectanceEquation || q == PEXtQEnone)
    re = PREFL_NONE;
  else if (q == PEXtQEambient)
    re = PREFL_AMBIENT;
  else if (q == PEXtQEdiffuse)
    re = PREFL_AMB_DIFF;
  else if (q == PEXtQEspecular)
    re = PREFL_AMB_DIFF_SPEC;
  else
    {
      sprintf(buffer, cE_cvtStringToReflectanceEquation, (String)
	      fromVal->addr);
      ConversionError("cvtStringToReflectanceEquation", buffer);
      return False;
    }
  
  DONE(re, Pint);
}

/*
  CvtReflectanceEquationToString -- convert reflectance equation to string
*/
static Boolean CvtReflectanceEquationToString(dpy, args, num_args,
					      fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args)
    WrongParameters("cvtReflectanceEquationToString",
		    wP_cvtReflectanceEquationToString); 

  switch (* ((Pint *) fromVal->addr))
    {
    case PREFL_NONE: DONESTR("None"); break;
    case PREFL_AMBIENT: DONESTR("Ambient"); break;
    case PREFL_AMB_DIFF: DONESTR("Diffuse"); break;
    case PREFL_AMB_DIFF_SPEC: DONESTR("Specular"); break;

    default:
      sprintf(buffer, cE_cvtReflectanceEquationToString,
	      *((int *) fromVal->addr)); 
      ConversionError("cvtReflectanceEquationToString", buffer);
      return False;
    }
  /* NOT REACHED */
}

/*
  CvtStringToShadingMethod -- convert string to shading method
*/
/*ARGSUSED*/
static Boolean CvtStringToShadingMethod(dpy, args, num_args,
					fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pint sm;
  register XrmQuark q;

  if (*num_args)
    WrongParameters("cvtStringToShadingMethod", wP_cvtStringToShadingMethod);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQpextDefaultShadingMethod || q == PEXtQEnone)
    sm = PSD_NONE;
  else if (q == PEXtQEcolor)
    sm = PSD_COLOUR;
  else if (q == PEXtQEdotProduct)
    sm = PSD_DOT_PRODUCT;
  else if (q == PEXtQEnormal)
    sm = PSD_NORMAL;
  else
    {
      sprintf(buffer, cE_cvtStringToShadingMethod, (String) fromVal->addr);
      ConversionError("cvtStringToShadingMethod", buffer);
      return False;
    }

  DONE(sm, Pint);
}

/*
  CvtShadingMethodToString -- convert reflectance equation to string
*/
static Boolean CvtShadingMethodToString(dpy, args, num_args,
					fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args)
    WrongParameters("cvtShadingMethodToString", wP_cvtShadingMethodToString);

  switch (* ((Pint *) fromVal->addr))
    {
    case PSD_NONE: DONESTR("None"); break;
    case PSD_COLOUR: DONESTR("Color"); break;
    case PSD_DOT_PRODUCT: DONESTR("Dot Product"); break;
    case PSD_NORMAL: DONESTR("Normal"); break;

    default:
      sprintf(buffer, cE_cvtShadingMethodToString, *((int *) fromVal->addr));
      ConversionError("conversionError", buffer);
      return False;
    }
  /* NOT REACHED */
}

/*
  CvtStringToEdgeFlag -- convert string to edge flag
*/
static Boolean CvtStringToEdgeFlag(dpy, args, num_args,
				   fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pedge_flag ef;
  XrmQuark q;

  if (*num_args != 0)
    WrongParameters("cvtStringToEdgeFlag", wP_cvtStringToEdgeFlag);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEtrue || q == PEXtQEon || q == PEXtQEyes)
    ef = PEDGE_ON;
  else if (q == PEXtQEfalse || q == PEXtQEoff || q == PEXtQEno ||
	   q == PEXtQE0 || q == PEXtQpextDefaultEdgeFlag)
    ef = PEDGE_OFF;
  else
    {
      sprintf(buffer, cE_cvtStringToEdgeFlag, (String) fromVal->addr);
      ConversionError("cvtStringToEdgeFlag", buffer);
      return False;
    }

  DONE(ef, Pedge_flag);
}

/*
  CvtEdgeFlagToString -- convert edge flag to string argument
*/
/*ARGSUSED*/
static Boolean CvtEdgeFlagToString(dpy, args, num_args,
				   fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static char buff[10];
  
  if (*num_args)
    WrongParameters("cvtEdgeFlagToString", wP_cvtEdgeFlagToString);
  
  switch (* ((Pedge_flag *) fromVal->addr))
    {
    case PEDGE_ON: DONESTR("On"); break;
    case PEDGE_OFF: DONESTR("Off"); break;

    default:
      sprintf(buffer, cE_cvtEdgeFlagToString, *((int *) fromVal->addr));
      ConversionError("cvtEdgeFlagToString", buffer);
      return False;
    }
  /* NOTREACHED */
}

/*
  CvtStringToLineType -- convert string to line type
*/
static Boolean CvtStringToLineType(dpy, args, num_args,
				   fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pint lt;
  XrmQuark q;

  if (*num_args != 0)
    WrongParameters("cvtStringToLineType", wP_cvtStringToLineType);

  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEsolid || q == PEXtQpextDefaultLineType)
    lt = PLINE_SOLID;
  else if (q == PEXtQEdash)
    lt = PLINE_DASH;
  else if (q == PEXtQEdot)
    lt = PLINE_DOT;
  else if (q == PEXtQEdashDot)
    lt = PLINE_DASH_DOT;
  else
    {
      sprintf(buffer, cE_cvtStringToLineType, (String) fromVal->addr);
      ConversionError("cvtStringToLineType", buffer);
      return False;
    }

  DONE(lt, Pint);
}

/*
  CvtLineTypeToString -- convert line type to string
*/
static Boolean CvtLineTypeToString(dpy, args, num_args,
				   fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args != 0)
    WrongParameters("cvtStringToLineType", wP_cvtLineTypeToString);
  
  switch (* ((Pint *) fromVal->addr))
    {
    case PLINE_SOLID: DONESTR("Solid"); break;
    case PLINE_DASH: DONESTR("Dashed"); break;
    case PLINE_DOT: DONESTR("Dotted"); break;
    case PLINE_DASH_DOT: DONESTR("Dash Dotted"); break;

    default:
      sprintf(buffer, cE_cvtLineTypeToString, *((Pint *) fromVal->addr));
      ConversionError("cvtLineTypeToString", buffer);
      return False;
    }
  /* NOTREACHED */
}

/*
  CvtStringToDistinguishMode -- convert string to face distinguish mode
*/
static Boolean CvtStringToDistinguishMode(dpy, args, num_args,
					  fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pdisting_mode dm;
  XrmQuark q;

  if (*num_args != 0)
    WrongParameters("cvtStringToDistinguishMode",
		    wP_cvtStringToDistinguishMode);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEno || q == PEXtQEfalse || q == PEXtQEoff ||
      q == PEXtQE0 || q == PEXtQpextDefaultDistinguishMode)
    dm = PDISTING_NO;
  else if (q == PEXtQEyes || q == PEXtQEtrue || q == PEXtQEon)
    dm = PDISTING_YES;
  else
    {
      sprintf(buffer, cE_cvtStringToDistinguishMode, (String) fromVal->addr);
      ConversionError("cvtStringToDistinguishMode", buffer);
      return False;
    }

  DONE(dm, Pdisting_mode);
}

/*
  CvtDistinguishModeTo -- convert string to face distinguish mode
*/
static Boolean CvtDistinguishModeToString(dpy, args, num_args,
					  fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pdisting_mode dm;
  XrmQuark q;

  if (*num_args != 0)
    WrongParameters("cvtDistinguishModeToString",
		    wP_cvtDistinguishModeToString); 
  
  switch (* ((Pdisting_mode *) fromVal->addr))
    {
    case PDISTING_NO: DONESTR("No"); break;
    case PDISTING_YES: DONESTR("Yes"); break;

    default:
      sprintf(buffer, cE_cvtDistinguishModeToString, *((int *) fromVal->addr));
      ConversionError("cvtDistinguishModeToString", buffer);
      return False;
    }
  /* NOTREACHED */
}

/*
  CvtStringToPoint3 -- convert string to 3D point
*/
static Boolean CvtStringToPoint3(dpy, args, num_args, fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Ppoint3 p3;
  register int num_parsed;

  if (*num_args != 0)
    WrongParameters("cvtStringToPoint3", wP_cvtStringToPoint3);
  
  if (index((String) fromVal->addr, ','))
    {
      if ((num_parsed = sscanf((String) fromVal->addr, "%f,%f,%f",
			       &p3.x, &p3.y, &p3.z)) != 3)
	{
	  sprintf(buffer, cE_cvtStringToPoint3, (String) fromVal->addr);
	  ConversionError("cvtStringToPoint3", buffer);
	  return False;
	}
    }
  else
    {
      if (XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr))
	  != PEXtQpextOrigin)
	{	
	  sprintf(buffer, cE_cvtStringToPoint3, (String) fromVal->addr);
	  ConversionError("cvtStringToPoint3", buffer);
	  return False;
	}
      else
	p3.x = 0.0, p3.y = 0.0, p3.z = 0.0;
      
    }

  DONE(p3, Ppoint3);
}

/*
  CvtPoint3ToString -- convert 3D point to string argument
*/
/*ARGSUSED*/
static Boolean CvtPoint3ToString(dpy, args, num_args, fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static char buff[80];
  register Ppoint3 *point = (Ppoint3 *) fromVal->addr;
  
  if (*num_args)
    WrongParameters("cvtPoint3ToString", wP_cvtPoint3ToString);
  
  sprintf(buff, "%f, %f, %f", point->x, point->y, point->z);
  DONESTR(buff);
  /* NOTREACHED */
}

/*
  CvtStringToPoint -- convert string to 2D point
*/
static Boolean CvtStringToPoint(dpy, args, num_args, fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Ppoint p;
  register int num_parsed;

  if (*num_args != 0)
    WrongParameters("cvtStringToPoint", wP_cvtStringToPoint);
  
  if (index((String) fromVal->addr, ','))
    {
      if ((num_parsed = sscanf((String) fromVal->addr, "%f,%f",
			       &p.x, &p.y)) != 2)
	{
	  sprintf(buffer, cE_cvtStringToPoint, (String) fromVal->addr);
	  ConversionError("cvtStringToPoint", buffer);
	  return False;
	}
    }
  else
    {
      if (XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr))
	  != PEXtQpextOrigin)
	{	
	  sprintf(buffer, cE_cvtStringToPoint, (String) fromVal->addr);
	  ConversionError("cvtStringToPoint", buffer);
	  return False;
	}
      else
	p.x = 0.0, p.y = 0.0;
      
    }

  DONE(p, Ppoint);
}

/*
  CvtPointToString -- convert 2D point to string argument
*/
/*ARGSUSED*/
static Boolean CvtPointToString(dpy, args, num_args, fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static char buff[80];
  register Ppoint *point = (Ppoint *) fromVal->addr;
  
  if (*num_args)
    WrongParameters("cvtPointToString", wP_cvtPointToString);
  
  sprintf(buff, "%f, %f", point->x, point->y);
  DONESTR(buff);
  /* NOTREACHED */
}

/*
  CvtStringToVector3 -- convert string to 3D vector
*/
/*ARGSUSED*/
static Boolean CvtStringToVector3(dpy, args, num_args, fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pvec3 v3;
  register int num_parsed;
  XrmQuark q;

  if (*num_args != 0)
    WrongParameters("cvtStringToVector3", wP_cvtStringToVector3);
  
  if (index((String) fromVal->addr, ','))
    {
      if ((num_parsed = sscanf((String) fromVal->addr, "%f,%f,%f",
			       &v3.delta_x, &v3.delta_y, &v3.delta_z)) != 3)
	{
	  sprintf(buffer, cE_cvtStringToVector3, (String) fromVal->addr);
	  ConversionError("cvtStringToVector3", buffer);
	  return False;
	}
    }
  else
    {
      q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
      if (q == PEXtQpextOrigin)
	v3.delta_x = 0.0, v3.delta_y = 0.0, v3.delta_z = 0.0;
      else if (q == PEXtQpextUnitVectorX)
	v3.delta_x = 1.0, v3.delta_y = 0.0, v3.delta_z = 0.0;
      else if (q == PEXtQpextUnitVectorY)
	v3.delta_x = 0.0, v3.delta_y = 1.0, v3.delta_z = 0.0;
      else if (q == PEXtQpextUnitVectorZ)
	v3.delta_x = 0.0, v3.delta_y = 0.0, v3.delta_z = 1.0;
      else
	{	
	  sprintf(buffer, cE_cvtStringToVector3, (String) fromVal->addr);
	  ConversionError("cvtStringToVector3", buffer);
	  return False;
	}
      
    }

  DONE(v3, Pvec3);
}

/*
  CvtVector3ToString -- convert 3D vector to string argument
*/
/*ARGSUSED*/
static Boolean CvtVector3ToString(dpy, args, num_args, fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static char buff[80];
  register Pvec3 *vector = (Pvec3 *) fromVal->addr;
  
  if (*num_args)
    WrongParameters("cvtVector3ToString", wP_cvtVector3ToString);
  
  sprintf(buff, "%f, %f, %f",
	  vector->delta_x, vector->delta_y, vector->delta_z);
  DONESTR(buff);
  /* NOTREACHED */
}

/*
  CvtStringToVector -- convert string to 2D vector
*/
/*ARGSUSED*/
static Boolean CvtStringToVector(dpy, args, num_args, fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pvec v;
  register int num_parsed;
  XrmQuark q;

  if (*num_args != 0)
    WrongParameters("cvtStringToVector", wP_cvtStringToVector);
  
  if (index((String) fromVal->addr, ','))
    {
      if ((num_parsed = sscanf((String) fromVal->addr,
			      "%f,%f", &v.delta_x, &v.delta_y)) != 2)
	{
	  sprintf(buffer, cE_cvtStringToVector, (String) fromVal->addr);
	  ConversionError("cvtStringToVector", buffer);
	  return False;
	}
    }
  else
    {
      q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
      if (q == PEXtQpextOrigin)
	v.delta_x = 0.0, v.delta_y = 0.0;
      else if (q == PEXtQpextUnitVectorX)
	v.delta_x = 1.0, v.delta_y = 0.0;
      else if (q == PEXtQpextUnitVectorY)
	v.delta_x = 0.0, v.delta_y = 1.0;
      else
	{	
	  sprintf(buffer, cE_cvtStringToVector, (String) fromVal->addr);
	  ConversionError("cvtStringToVector", buffer);
	  return False;
	}
      
    }

  DONE(v, Pvec);
}

/*
  CvtVectorToString -- convert 2D vector to string argument
*/
/*ARGSUSED*/
static Boolean CvtVectorToString(dpy, args, num_args, fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static char buff[80];
  register Pvec *vector = (Pvec *) fromVal->addr;
  
  if (*num_args)
    WrongParameters("cvtVectorToString", wP_cvtVectorToString);
  
  sprintf(buff, "%f, %f", vector->delta_x, vector->delta_y);
  DONESTR(buff);
  /* NOTREACHED */
}

/*
  CvtStringToLimit -- convert string to 2D limit
*/
/*ARGSUSED*/
static Boolean CvtStringToLimit(dpy, args, num_args, fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Plimit l;

  if (*num_args != 0)
    WrongParameters("cvtStringToLimit", wP_cvtStringToLimit);
  
  if (index((String) fromVal->addr, ','))
    {
      if (sscanf((String) fromVal->addr, "%f,%f,%f,%f",
		 &l.x_min, &l.y_min, &l.x_max, &l.y_max) != 4)
	{
	  sprintf(buffer, cE_cvtStringToLimit, (String) fromVal->addr);
	  ConversionError("cvtStringToLimit", buffer);
	  return False;
	}
    }
  else
    {
      if (XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr))
	  != PEXtQpextDefaultClipLimits)
	{	
	  sprintf(buffer, cE_cvtStringToLimit, (String) fromVal->addr);
	  ConversionError("cvtStringToLimit", buffer);
	  return False;
	}
      else
	l.x_min = 0.0, l.y_min = 0.0;
	l.x_max = 1.0, l.y_max = 1.0;
    }

  DONE(l, Plimit);
}

/*
  CvtLimitToString -- convert 2D limit to string argument
*/
/*ARGSUSED*/
static Boolean CvtLimitToString(dpy, args, num_args, fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static char buff[80];
  register Plimit *limit = (Plimit *) fromVal->addr;
  
  if (*num_args)
    WrongParameters("cvtLimitToString", wP_cvtLimitToString);
  
  sprintf(buff, "%f, %f, %f, %f",
	  limit->x_min, limit->y_min, limit->x_max, limit->y_max);
  DONESTR(buff);
  /* NOTREACHED */
}

/*
  CvtStringToLimit3 -- convert string to 3D limit
*/
/*ARGSUSED*/
static Boolean CvtStringToLimit3(dpy, args, num_args, fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Plimit3 l3;

  if (*num_args != 0)
    WrongParameters("cvtStringToLimit3", wP_cvtStringToLimit3);
  
  if (index((String) fromVal->addr, ','))
    {
      if (sscanf((String) fromVal->addr, "%f,%f,%f,%f,%f,%f",
		 &l3.x_min, &l3.y_min, &l3.z_min,
		 &l3.x_max, &l3.y_max, &l3.z_max) != 6)
	{
	  sprintf(buffer, cE_cvtStringToLimit3, (String) fromVal->addr);
	  ConversionError("cvtStringToLimit3", buffer);
	  return False;
	}
    }
  else
    {
      if (XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr))
	  != PEXtQpextDefaultClipLimits)
	{	
	  sprintf(buffer, cE_cvtStringToLimit3, (String) fromVal->addr);
	  ConversionError("cvtStringToLimit3", buffer);
	  return False;
	}
      else
	l3.x_min = 0.0, l3.y_min = 0.0, l3.z_min = 0.0,
	l3.x_max = 1.0, l3.y_max = 1.0, l3.z_max = 1.0;
    }

  DONE(l3, Plimit3);
}

/*
  CvtLimit3ToString -- convert 3D limit to string argument
*/
/*ARGSUSED*/
static Boolean CvtLimit3ToString(dpy, args, num_args, fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static char buff[120];
  register Plimit3 *limit = (Plimit3 *) fromVal->addr;
  
  if (*num_args)
    WrongParameters("cvtLimit3ToString", wP_cvtLimit3ToString);
  
  sprintf(buff, "%f, %f, %f, %f, %f, %f",
	  limit->x_min, limit->y_min, limit->z_min,
	  limit->x_max, limit->y_max, limit->z_max);
  DONESTR(buff);
  /* NOTREACHED */
}

/*
  CvtStringToIntList -- convert string to integer list

  TODO:
    Provide a destructor under R4 for an IntList that frees calloc'ed memory.
*/
#define MAX_STATIC 20
static Boolean CvtStringToIntList(dpy, args, num_args, fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pint_list il;
  static Pint ints[MAX_STATIC];
  register int num_ints = 0;
  register String comma;
  register Pint *dest;

  if (*num_args)
    WrongParameters("cvtStringToIntList", wP_cvtStringToIntList);

  for (num_ints = 1,
       comma = (String) fromVal->addr; *comma; comma++)
    if (*comma == ',')
      num_ints++;
  
  il.num_ints = num_ints;
  il.ints = (Pint *) calloc(num_ints, sizeof(Pint));

  if (num_ints > 1)
    {
      dest = il.ints;
      comma = (String) fromVal->addr;
      
      while (num_ints--)
	if (sscanf(comma, "%d", dest++) == 1)
	  comma = index(comma, ',')+1;
	else
	  {
	    sprintf(buffer, cE_cvtStringToIntList, (String) fromVal->addr);
	    ConversionError("cvtStringToIntList", buffer);
	    return False;
	  }
    }
  else
    {
      if (XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr))
	  == PEXtQEnone)
	il.num_ints = 0, il.ints = (Pint *) NULL;
      else
	if (sscanf((String) fromVal->addr, "%d", &il.ints[0]) != 1)
	  {
	    sprintf(buffer, cE_cvtStringToIntList, (String) fromVal->addr);
	    ConversionError("cvtStringToIntList", buffer);
	    return False;
	  }
    }
  
  DONE(il, Pint_list);		/* remember to free list in client */
}

/*
  CvtIntListToString -- convert 3D limit to string argument
*/
/*ARGSUSED*/
static Boolean CvtIntListToString(dpy, args, num_args, fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static char buff[2048];
  register Pint_list *list = (Pint_list *) fromVal->addr;
  register Pint i;
  register String label;
  
  if (*num_args)
    WrongParameters("cvtIntListToString", wP_cvtIntListToString);
  
  label = buff;
  for (i = 0; i < list->num_ints-1; i++)
    label += strlen(sprintf(label, "%d, ", list->ints[i]))+1;
  sprintf(label, "%d", list->ints[list->num_ints-1]);
  
  DONESTR(buff);
  /* NOTREACHED */
}

/*
  CvtStringToLightSourceType -- convert string to light source type
*/
/*ARGSUSED*/
static Boolean CvtStringToLightSourceType(dpy, args, num_args,
					  fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pint lst;
  XrmQuark q;

  if (*num_args != 0)
    WrongParameters("cvtStringToLightSourceType",
		    wP_cvtStringToLightSourceType);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEambient || q == PEXtQpextDefaultLightSourceType)
    lst = PLIGHT_AMBIENT;
  else if (q == PEXtQEdirectional)
    lst = PLIGHT_DIRECTIONAL;
  else if (q == PEXtQEpositional)
    lst = PLIGHT_POSITIONAL;
  else if (q == PEXtQEspot)
    lst = PLIGHT_SPOT;
  else
    {
      sprintf(buffer, cE_cvtStringToLightSourceType, (String) fromVal->addr);
      ConversionError("cvtStringToLightSourceType", buffer);
      return False;
    }

  DONE(lst, Pint);
}

/*
  CvtLightSourceTypeToString -- convert light source type to string
*/
/*ARGSUSED*/
static Boolean CvtLightSourceTypeToString(dpy, args, num_args,
					  fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args != 0)
    WrongParameters("cvtLightSourceTypeToString",
		    wP_cvtLightSourceTypeToString); 
  
  switch (* ((Pint *) fromVal->addr))
    {
    case PLIGHT_AMBIENT: DONESTR("Ambient"); break;
    case PLIGHT_DIRECTIONAL: DONESTR("Directional"); break;
    case PLIGHT_POSITIONAL: DONESTR("Positional"); break;
    case PLIGHT_SPOT: DONESTR("Spot"); break;

    default:
      sprintf(buffer, cE_cvtLightSourceTypeToString,
	      *((Pint *) fromVal->addr)); 
      ConversionError("cvtLightSourceTypeToString", buffer);
      return False;
    }
  /* NOTREACHED */
}

/*
  CvtStringToClipIndicator -- convert string to clipping indicator
*/
/*ARGSUSED*/
static Boolean CvtStringToClipIndicator(dpy, args, num_args,
				   fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pclip_ind ci;
  XrmQuark q;

  if (*num_args != 0)
    WrongParameters("cvtStringToClipIndicator", wP_cvtStringToClipIndicator);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEclip || q == PEXtQEon || q == PEXtQEtrue ||
      q == PEXtQEyes || q == PEXtQpextDefaultClipIndicator)
    ci = PIND_CLIP;
  else if (q == PEXtQEnoClip || q == PEXtQEoff || q == PEXtQE0 ||
	   q == PEXtQEfalse || q == PEXtQEno)
    ci = PIND_NO_CLIP;
  else
    {
      sprintf(buffer, cE_cvtStringToClipIndicator, (String) fromVal->addr);
      ConversionError("cvtStringToClipIndicator", buffer);
      return False;
    }

  DONE(ci, Pclip_ind);
}

/*
  CvtClipIndicatorToString -- convert clipping indicator to string
*/
static Boolean CvtClipIndicatorToString(dpy, args, num_args,
				   fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args != 0)
    WrongParameters("cvtStringToClipIndicator", wP_cvtStringToClipIndicator);
  
  switch (* ((Pclip_ind *) fromVal->addr))
    {
    case PIND_CLIP: DONESTR("Clip"); break;
    case PIND_NO_CLIP: DONESTR("No Clip"); break;

    default:
      sprintf(buffer, cE_cvtStringToClipIndicator, *((int *) fromVal->addr));
      ConversionError("cvtClipIndicatorToString", buffer);
      return False;
    }
  /* NOTREACHED */
}

#ifdef ES_PICK_EXTENSION
/*
  CvtStringToPickMode -- convert string to pick mode
*/
/*ARGSUSED*/
static Boolean CvtStringToPickMode(dpy, args, num_args,
				      fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static int pm;
  XrmQuark q;

  if (*num_args != 0)
    WrongParameters("cvtStringToPickMode", wP_cvtStringToPickMode);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEfirst || q == PEXtQEpickFirst ||
      q == PEXtQpextDefaultPickMode)
    pm = pick_first;
  else if (q == PEXtQElast || q == PEXtQEpickLast)
    pm = pick_last;
  else if (q == PEXtQEnear || q == PEXtQEpickNear)
    pm = pick_near;
  else if (q == PEXtQEfar || q == PEXtQEpickFar)
    pm = pick_far;
  else
    {
      sprintf(buffer, cE_cvtStringToPickMode, (String) fromVal->addr);
      ConversionError("cvtStringToPickMode", buffer);
      return False;
    }

  DONE(pm, int);
}

/*
  CvtPickModeToString -- convert clipping indicator to string
*/
static Boolean CvtPickModeToString(dpy, args, num_args,
				      fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args != 0)
    WrongParameters("cvtPickModeToString", wP_cvtPickModeToString);
  
  switch (*fromVal->addr)
    {
    case pick_first: DONESTR("First"); break;
    case pick_last: DONESTR("Last"); break;
    case pick_near: DONESTR("Near"); break;
    case pick_far: DONESTR("Far"); break;

    default:
      sprintf(buffer, cE_cvtPickModeToString, *((int *) fromVal->addr));
      ConversionError("cvtPickModeToString", buffer);
      return False;
    }
  /* NOTREACHED */
}

/*
  CvtStringToPickHighlightMode -- convert string to pick highlighting mode
*/
/*ARGSUSED*/
static Boolean CvtStringToPickHighlightMode(dpy, args, num_args,
					       fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static int phm;
  XrmQuark q;

  if (*num_args != 0)
    WrongParameters("cvtStringToPickHighlightMode",
		    wP_cvtStringToPickHighlightMode);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEitem || q == PEXtQEpickHighlightItem ||
      q == PEXtQpextDefaultPickHighlightMode)
    phm = pick_highlighting_item;
  else if (q == PEXtQEcommand || q == PEXtQEpickHighlightCommand)
    phm = pick_highlighting_command;
  else if (q == PEXtQEpickHighlightOff || q == PEXtQE0 ||
	   q == PEXtQEoff || q == PEXtQEno || q == PEXtQEfalse)
    phm = pick_highlighting_off;
  else
    {
      sprintf(buffer, cE_cvtStringToPickHighlightMode,
	      (String) fromVal->addr);
      ConversionError("cvtStringToPickHighlightMode", buffer);
      return False;
    }

  DONE(phm, int);
}

/*
  CvtPickHighlightModeToString -- convert clipping indicator to string
*/
static Boolean CvtPickHighlightModeToString(dpy, args, num_args,
					       fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args != 0)
    WrongParameters("cvtPickHighlightModeToString",
		    wP_cvtPickHighlightModeToString);
  
  switch (*fromVal->addr)
    {
    case pick_highlighting_item: DONESTR("Item"); break;
    case pick_highlighting_command: DONESTR("Command"); break;
    case pick_highlighting_off: DONESTR("Off"); break;

    default:
      sprintf(buffer, cE_cvtPickHighlightModeToString,
	      *((int *) fromVal->addr));
      ConversionError("cvtPickHighlightModeToString", buffer);
      return False;
    }
  /* NOTREACHED */
}
#endif					/* ES_PICK_EXTENSION */

/*
  CvtStringToDepthCueMode -- convert string to depth cue mode
*/
/*ARGSUSED*/
static Boolean CvtStringToDepthCueMode(dpy, args, num_args,
				       fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pdcue_mode dcm;
  register XrmQuark q;

  if (*num_args)
    WrongParameters("cvtStringToDepthCueMode", wP_cvtStringToDepthCueMode);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEsuppressed || q == PEXtQpextDefaultDepthCueMode)
    dcm = PSUPPRESSED;
  else if (q == PEXtQEallowed)
    dcm = PALLOWED;
  else
    {
      sprintf(buffer, cE_cvtStringToDepthCueMode, (String) fromVal->addr);
      ConversionError("cvtStringToDepthCueMode", buffer);
      return False;
    }
  
  DONE(dcm, Pdcue_mode);
}

/*
  CvtDepthCueModeToString -- convert depth cue mode to string
*/
static Boolean CvtDepthCueModeToString(dpy, args, num_args,
				       fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args)
    WrongParameters("cvtDepthCueModeToString", wP_cvtDepthCueModeToString);

  switch (* ((Pdcue_mode *) fromVal->addr))
    {
    case PALLOWED: DONESTR("Allowed"); break;
    case PSUPPRESSED: DONESTR("Suppressed"); break;

    default:
      sprintf(buffer, cE_cvtDepthCueModeToString,  *((int *) fromVal->addr));
      ConversionError("cvtDepthCueModeToString", buffer);
      return False;
    }
  /* NOT REACHED */
}

/*
  CvtStringToWorkstationType -- convert string to PHIGS ws type
*/
/*ARGSUSED*/
static Boolean CvtStringToWorkstationType(dpy, args, num_args,
					  fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pint wst;
  register XrmQuark q;

  if (*num_args)
    WrongParameters("cvtStringToWorkstationType",
		    wP_cvtStringToWorkstationType);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEtool)
    wst = phigs_ws_type_x_tool;
  else if (q == PEXtQEdrawable || q == PEXtQpextDefaultWorkstationType)
    wst = phigs_ws_type_x_drawable;
  else
    {
      sprintf(buffer, cE_cvtStringToWorkstationType, (String) fromVal->addr);
      ConversionError("cvtStringToWorkstationType", buffer);
      return False;
    }
  
  DONE(wst, Pint);
}

/*
  CvtWorkstationTypeToString -- convert PHIGS ws type to string
*/
static Boolean CvtWorkstationTypeToString(dpy, args, num_args,
					  fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  Pint wst;
  
  if (*num_args)
    WrongParameters("cvtWorkstationTypeToString",
		    wP_cvtWorkstationTypeToString);

  wst = (* ((Pint *) fromVal->addr));
  if (wst == phigs_ws_type_x_tool)
    {
      DONESTR("Tool");
    }
  else if (wst == phigs_ws_type_x_drawable)
    {
      DONESTR("Drawable");
    }
  else
    {
      sprintf(buffer, cE_cvtWorkstationTypeToString, *((int *) fromVal->addr));
      ConversionError("cvtWorkstationTypeToString", buffer);
      return False;
    }
  /* NOT REACHED */
}

/*
  CvtStringToHLHSRMode -- convert string to hidden line, surface removal mode

  Quarks			PHIGS Value
  PEXtQEfalse
  PEXtQEoff
  PEXtQEno
  PEXtQEnone
  PEXtQpextDefaultHLHSRMode	PHIGS_HLHSR_MODE_NONE
  PEXtQEzBuffer			PHIGS_HLHSR_MODE_ZBUFF
*/
/*ARGSUSED*/
static Boolean CvtStringToHLHSRMode(dpy, args, num_args,
				    fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pint hlhsr_mode;
  register XrmQuark q;

  if (*num_args)
    WrongParameters("cvtStringToHLHSRMode", wP_cvtStringToHLHSRMode);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQpextDefaultHLHSRMode || q == PEXtQEnone || q == PEXtQE0 ||
      q == PEXtQEfalse || q == PEXtQEoff || q == PEXtQEno)
    hlhsr_mode = PHIGS_HLHSR_MODE_NONE;
  else if (q == PEXtQEzBuffer)
    hlhsr_mode = PHIGS_HLHSR_MODE_ZBUFF;
  else
    {
      sprintf(buffer, cE_cvtStringToHLHSRMode, (String) fromVal->addr);
      ConversionError("cvtStringToHLHSRMode", buffer);
      return False;
    }
  
  DONE(hlhsr_mode, Pint);
}

/*
  CvtHLHSRModeToString -- hidden line, surface removal  mode to string
*/
static Boolean CvtHLHSRModeToString(dpy, args, num_args,
				    fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args)
    WrongParameters("cvtModificationModeToString",
		    wP_cvtModificationModeToString);

  switch (* ((Pint *) fromVal->addr))
    {
    case PHIGS_HLHSR_MODE_NONE: DONESTR("None"); break;
    case PHIGS_HLHSR_MODE_ZBUFF: DONESTR("Z Buffer"); break;
    }
  /* NOTREACHED */
}

/*
  CvtStringToControlFlag -- convert string to workstation update control flag

  Quarks			PHIGS Value
  PEXtQpextDefaultControlFlag
  PEXtQEconditionally		PCONDITIONALLY
  PEXtQEalways			PALWAYS
*/
/*ARGSUSED*/
static Boolean CvtStringToControlFlag(dpy, args, num_args,
				      fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pctrl_flag cf;
  register XrmQuark q;

  if (*num_args)
    WrongParameters("cvtStringToControlFlag", wP_cvtStringToControlFlag);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEconditionally || q == PEXtQpextDefaultControlFlag)
    cf = PFLAG_COND;
  else if (q == PEXtQEalways)
    cf = PFLAG_ALWAYS;
  else
    {
      sprintf(buffer, cE_cvtStringToControlFlag, (String) fromVal->addr);
      ConversionError("cvtStringToControlFlag", buffer);
      return False;
    }
  
  DONE(cf, Pctrl_flag);
}

/*
  CvtControlFlagToString -- convert workstation control flag to string
*/
static Boolean CvtControlFlagToString(dpy, args, num_args,
				      fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args)
    WrongParameters("cvtControlFlagToString", wP_cvtControlFlagToString);

  switch (* ((Pctrl_flag *) fromVal->addr))
    {
    case PFLAG_COND: DONESTR("Conditionally"); break;
    case PFLAG_ALWAYS: DONESTR("Always"); break;
    }
  /* NOTREACHED */
}

/*
  CvtStringToDeferralMode -- convert string to workstation deferral mode
  
  Quarks				PHIGS Value
  PEXtQEasap
  PEXtQEasSoonAsPossible
  PEXtQpextDefaultDeferralMode		PASAP
  PEXtQEbnig
  PEXtQEbeforeNextInteractionGlobally	PBNIG
  PEXtQEbnil
  PEXtQEbeforeNextInteractionLocally	PBNIL
  PEXtQEasti
  PEXtQEatSomeTime			PASTI
  PEXtQEwait
  PEXtQEwhenApplicationRequestsIt		PWAIT
*/
/*ARGSUSED*/
static Boolean CvtStringToDeferralMode(dpy, args, num_args,
				       fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pdefer_mode dm;
  register XrmQuark q;

  if (*num_args)
    WrongParameters("cvtStringToDeferralMode", wP_cvtStringToDeferralMode);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEasap || q == PEXtQEasSoonAsPossible ||
      q == PEXtQpextDefaultDeferralMode)
    dm = PDEFER_ASAP;
  else if (q == PEXtQEbnig || q == PEXtQEbeforeNextInteractionGlobally)
    dm = PDEFER_BNIG;
  else if (q == PEXtQEbnil || q == PEXtQEbeforeNextInteractionLocally)
    dm = PDEFER_BNIL;
  else if (q == PEXtQEasti || q == PEXtQEatSomeTime)
    dm = PDEFER_ASTI;
  else if (q == PEXtQEwait || q == PEXtQEwhenApplicationRequestsIt)
    dm = PDEFER_WAIT;
  else
    {
      sprintf(buffer, cE_cvtStringToDeferralMode, (String) fromVal->addr);
      ConversionError("cvtStringToDeferralMode", buffer);
      return False;
    }
  
  DONE(dm, Pdefer_mode);
}

/*
  CvtDeferralModeToString -- convert workstation deferral mode to string
*/
static Boolean CvtDeferralModeToString(dpy, args, num_args,
				       fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args)
    WrongParameters("cvtDeferralModeToString", wP_cvtDeferralModeToString);

  switch (* ((Pdefer_mode *) fromVal->addr))
    {
    case PDEFER_ASAP: DONESTR("As Soon As Possible"); break;
    case PDEFER_BNIG: DONESTR("Before Next Interaction Globally"); break;
    case PDEFER_BNIL: DONESTR("Before Next Interaction Locally"); break;
    case PDEFER_ASTI: DONESTR("At Some Time"); break;
    case PDEFER_WAIT: DONESTR("When Application Requests It"); break;
    }
  /* NOTREACHED */
}

/*
  CvtStringToModificationMode -- convert string to workstation deferral mode

  Quarks				PHIGS Value
  PEXtQpextDefaultModificationMode
  PEXtQEnive
  PEXtQEnoImmediateVisualEffect		PNIVE
  PEXtQEuwor
  PEXtQEupdateWithoutRegeneration	PUWOR
  PEXtQEuqum
  PEXtQEuseQuickUpdateMethods		PUQUM
*/
/*ARGSUSED*/
static Boolean CvtStringToModificationMode(dpy, args, num_args,
					   fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static Pmod_mode m;
  register XrmQuark q;

  if (*num_args)
    WrongParameters("cvtStringToModificationMode",
		    wP_cvtStringToModificationMode);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEnive || q == PEXtQEnoImmediateVisualEffect ||
      q == PEXtQpextDefaultModificationMode)
    m = PMODE_NIVE;
  else if (q == PEXtQEuwor || q == PEXtQEupdateWithoutRegeneration)
    m = PMODE_UWOR;
  else if (q == PEXtQEuqum || q == PEXtQEuseQuickUpdateMethods)
    m = PMODE_UQUM;
  else
    {
      sprintf(buffer, cE_cvtStringToModificationMode, (String) fromVal->addr);
      ConversionError("cvtStringToModificationMode", buffer);
      return False;
    }
  
  DONE(m, Pdefer_mode);
}

/*
  CvtToModificationModeString -- Convert WS modification mode to string
*/
static Boolean CvtModificationModeToString(dpy, args, num_args,
					   fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args)
    WrongParameters("cvtModificationModeToString",
		    wP_cvtModificationModeToString);

  switch (* ((Pmod_mode *) fromVal->addr))
    {
    case PMODE_NIVE: DONESTR("No Immediate Visual Effect"); break;
    case PMODE_UWOR: DONESTR("Update Without Regeneration"); break;
    case PMODE_UQUM: DONESTR("Use Quick Update Methods"); break;
    }
  /* NOTREACHED */
}

/*
  CvtStringToResizePolicy -- convert string to PEXt workstation resize policy
*/
/*ARGSUSED*/
static Boolean CvtStringToResizePolicy(dpy, args, num_args,
				       fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static PEXtResizePolicy rp;
  register XrmQuark q;

  if (*num_args)
    WrongParameters("cvtStringToResizePolicy", wP_cvtStringToResizePolicy);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEnone || q == PEXtQpextDefaultResizePolicy)
    rp = PEXtResizeNone;
  else if (q == PEXtQEcallback)
    rp = PEXtResizeCallback;
  else if (q == PEXtQEporthole)
    rp = PEXtResizePorthole;
  else if (q == PEXtQEcontainer)
    rp = PEXtResizeContainer;
  else
    {
      sprintf(buffer, cE_cvtStringToResizePolicy, (String) fromVal->addr);
      ConversionError("cvtStringToResizePolicy", buffer);
      return False;
    }
  
  DONE(rp, PEXtResizePolicy);
}

/*
  CvtResizePolicyToString -- convert PEXt ws resize policy to string
*/
static Boolean CvtResizePolicyToString(dpy, args, num_args,
				       fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args)
    WrongParameters("cvtResizePolicyToString", wP_cvtResizePolicyToString);

  switch ((* ((PEXtResizePolicy *) fromVal->addr)))
    {
    case PEXtResizeNone: DONESTR("None"); break;
    case PEXtResizeCallback: DONESTR("Callback"); break;
    case PEXtResizePorthole: DONESTR("Porthole"); break;
    case PEXtResizeContainer: DONESTR("Container"); break;

    default:
      sprintf(buffer, cE_cvtResizePolicyToString, *((int *) fromVal->addr));
      ConversionError("cvtResizePolicyToString", buffer);
      return False;
    }
  /* NOTREACHED */
}

/*
  CvtStringToTraversalPolicy

  Convert string to PEXt workstation traversal policy
*/
/*ARGSUSED*/
static Boolean CvtStringToTraversalPolicy(dpy, args, num_args,
					  fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  static PEXtTraversalPolicy tp;
  register XrmQuark q;

  if (*num_args)
    WrongParameters("cvtStringToTraversalPolicy",
		    wP_cvtStringToTraversalPolicy);
  
  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEnone || q == PEXtQpextDefaultTraversalPolicy)
    tp = PEXtTraverseNone;
  else if (q == PEXtQEcallback)
    tp = PEXtTraverseCallback;
  else if (q == PEXtQEredraw)
    tp = PEXtTraverseRedraw;
  else
    {
      sprintf(buffer, cE_cvtStringToTraversalPolicy, (String) fromVal->addr);
      ConversionError("cvtStringToTraversalPolicy", buffer);
      return False;
    }
  
  DONE(tp, PEXtTraversalPolicy);
}

/*
  CvtTraversalPolicyToString -- convert PEXt ws Traversal policy to string
*/
static Boolean CvtTraversalPolicyToString(dpy, args, num_args,
				       fromVal, toVal, closure)
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  if (*num_args)
    WrongParameters("cvtTraversalPolicyToString",
		    wP_cvtTraversalPolicyToString);

  switch ((* ((PEXtTraversalPolicy *) fromVal->addr)))
    {
    case PEXtTraverseNone: DONESTR("None"); break;
    case PEXtTraverseCallback: DONESTR("Callback"); break;
    case PEXtTraverseRedraw: DONESTR("Redraw"); break;

    default:
      sprintf(buffer, cE_cvtTraversalPolicyToString, *((int *) fromVal->addr));
      ConversionError("cvtTraversalPolicyToString", buffer);
      return False;
    }
  /* NOTREACHED */
}

/*
  CvtStringToGravity -- string to gravity converter stolen from Xaw
*/
static Boolean CvtStringToGravity(dpy, args, num_args,
				  fromVal, toVal, closure) 
     Display *dpy;
     XrmValuePtr args;
     Cardinal *num_args;
     XrmValuePtr fromVal, toVal;
     XtPointer *closure;
{
  int g;
  XrmQuark q;

  if (*num_args != 0)
    WrongParameters("cvtStringToGravity", wP_cvtStringToGravity);

  q = XrmStringToQuark(PEXtLowerCaseStrip((String) fromVal->addr));
  if (q == PEXtQEnorthWest || q == PEXtQpextDefaultGravity)
    g = NorthWestGravity;
  else if (q == PEXtQEnorth || q == PEXtQEtop)
    g = NorthGravity;
  else if (q == PEXtQEnorthEast)
    g = NorthEastGravity;
  else if (q == PEXtQEwest || q == PEXtQEleft)
    g = WestGravity;
  else if (q == PEXtQEcenter)
    g = CenterGravity;
  else if (q == PEXtQEeast || q == PEXtQEright)
    g = EastGravity;
  else if (q == PEXtQEsouthWest)
    g = SouthWestGravity;
  else if (q == PEXtQEsouth || q == PEXtQEbottom)
    g = SouthGravity;
  else if (q == PEXtQEsouthEast)
    g = SouthEastGravity;
  else
    {
      sprintf(buffer, cE_cvtStringToGravity, (String) fromVal->addr);
      ConversionError("cvtStringToGravity", buffer);
      return False;
    }
  
  DONE(g, int);
}

typedef struct _NameValuePair {
  String Name;
  XrmQuark *Value;
} NameValuePair;

static void InitializeNameValuePairs(name_value_pairs, num_pairs)
     NameValuePair name_value_pairs[];
     Cardinal num_pairs;
{
  register int i;

  for (i = 0; i < num_pairs; i++)
    *name_value_pairs[i].Value = XrmStringToQuark(name_value_pairs[i].Name);
}

					/* the following macros may be */
					/* inherintly non-portable */
#define ENUM(name) { PEXtE/**/name, &PEXtQE/**/name }
#define DEFAULT(name) { PEXtDefault/**/name, &PEXtQpextDefault/**/name }

static NameValuePair quark_list[] = {
  { XtExtdefaultbackground, &PEXtQExtDefaultBackground },
  { XtExtdefaultforeground, &PEXtQExtDefaultForeground },

  ENUM(callback), ENUM(porthole), ENUM(container), DEFAULT(ResizePolicy),

  ENUM(redraw), DEFAULT(TraversalPolicy),

  { XtEtrue, &PEXtQEtrue }, { XtEfalse, &PEXtQEfalse },
  { XtEon, &PEXtQEon }, { XtEoff, &PEXtQEoff },
  { XtEyes, &PEXtQEyes }, { XtEno, &PEXtQEno },
  ENUM(1), ENUM(0),

  ENUM(hollow), ENUM(solid), ENUM(pattern), ENUM(hatch), ENUM(empty),
  DEFAULT(InteriorStyle),

  ENUM(indirect), ENUM(rgb), ENUM(hls), ENUM(cie), ENUM(hsv),
  DEFAULT(GeneralColor),

  ENUM(parallel), ENUM(perspective), DEFAULT(ProjectionType),

  ENUM(ambient), ENUM(diffuse), ENUM(specular), DEFAULT(ReflectanceEquation),

  ENUM(color), ENUM(dotProduct), ENUM(normal), DEFAULT(ShadingMethod),

  ENUM(cullBackFace), ENUM(cullFrontFace), DEFAULT(CullMode),

  DEFAULT(EdgeFlag),

  ENUM(dash), ENUM(dot), ENUM(dashDot), DEFAULT(LineType),

  DEFAULT(DistinguishMode),

  ENUM(none), ENUM(directional), ENUM(positional), ENUM(spot),
  DEFAULT(LightSourceType),

  DEFAULT(ClipLimits),

  ENUM(clip), ENUM(noClip), DEFAULT(ClipIndicator),

  { PEXtOrigin, &PEXtQpextOrigin },
  { PEXtUnitVectorX, &PEXtQpextUnitVectorX },
  { PEXtUnitVectorY, &PEXtQpextUnitVectorY },
  { PEXtUnitVectorZ, &PEXtQpextUnitVectorZ },
  
  ENUM(zBuffer), DEFAULT(HLHSRMode),

  ENUM(conditionally), ENUM(always), DEFAULT(ControlFlag),

  ENUM(asap), ENUM(asSoonAsPossible),
  ENUM(bnig), ENUM(beforeNextInteractionGlobally),
  ENUM(bnil), ENUM(beforeNextInteractionLocally),
  ENUM(asti), ENUM(atSomeTime),
  ENUM(wait), ENUM(whenApplicationRequestsIt), DEFAULT(DeferralMode),

  ENUM(nive), ENUM(noImmediateVisualEffect),
  ENUM(uwor), ENUM(updateWithoutRegeneration),
  ENUM(uqum), ENUM(useQuickUpdateMethods), DEFAULT(ModificationMode),
  
  ENUM(allowed), ENUM(suppressed), DEFAULT(DepthCueMode),

  ENUM(tool), ENUM(drawable), DEFAULT(WorkstationType),

  ENUM(northWest), ENUM(north), ENUM(northEast), ENUM(west), ENUM(center),
  ENUM(east), ENUM(southWest), ENUM(south), ENUM(southEast), ENUM(left),
  ENUM(top), ENUM(right), ENUM(bottom), DEFAULT(Gravity),

#ifdef ES_PICK_EXTENSION
  ENUM(first), ENUM(pickFirst), ENUM(last), ENUM(pickLast),
  ENUM(near), ENUM(pickNear), ENUM(far), ENUM(pickFar), DEFAULT(PickMode),

  ENUM(item), ENUM(pickHighlightItem), ENUM(command),
  ENUM(pickHighlightCommand), ENUM(pickHighlightOff),
  DEFAULT(PickHighlightMode)
#endif					/* ES_PICK_EXTENSION */
      
  };


typedef struct __PEXtConverterRec {
  String from_type, to_type;
  XtTypeConverter proc;
  XtConvertArgList args;
  Cardinal num_args;
  XtCacheType cache;
  XtDestructor destroy;
} _PEXtConverterRec;

#define StringTo(type) \
  { XtRString, PEXtR/**/type, CvtStringTo/**/type, NULL, 0, XtCacheAll, NULL }
#define ToString(type) \
  { PEXtR/**/type, XtRString, Cvt/**/type/**/ToString, NULL, 0, \
      XtCacheAll, NULL }
#define StringToString(type) StringTo(type), ToString(type)

/*
  PEXtRegisterConverters

  This routine adds all necessary PEX resource converters
*/
void PEXtRegisterConverters()
{
  static Boolean initialized = False;
  static XtConvertArgRec colorConvertArgs[] = {
    { XtBaseOffset, (XtPointer) XtOffset(Widget, core.screen),
	sizeof(Screen *) },
    { XtBaseOffset, (XtPointer) XtOffset(Widget, core.colormap),
	sizeof(Colormap) }
  };
  static _PEXtConverterRec converters[] = {
    StringToString(WorkstationType),	/* PHIGS workstation type */
    StringToString(HLHSRMode),		/* Hidden Line/Surface Removal Mode */
    StringToString(ControlFlag),	/* Update Control Flag */
    StringToString(DeferralMode),	/* Deferral Mode */
    StringToString(ModificationMode),	/* Modification Mode */
    StringToString(InteriorStyle),	/* PHIGS Interior Style */
    StringToString(ProjectionType),	/* PHIGS Projection Type */
    StringToString(CullMode),		/* PHIGS+ Face Culling Mode */
					/* PHIGS+ Generalized Color */
    { XtRString, PEXtRGeneralColor, CvtStringToGeneralColor,
	colorConvertArgs, XtNumber(colorConvertArgs), XtCacheAll, NULL },
    { PEXtRGeneralColor, XtRString, CvtGeneralColorToString,
	colorConvertArgs, XtNumber(colorConvertArgs), XtCacheAll, NULL },
    StringToString(ReflectanceEquation), /* PHIGS+ Reflectance Equation */
    StringToString(ShadingMethod),	/* PHIGS+ Shading Method */
    StringToString(EdgeFlag),		/* PHIGS Edge Flag */
    StringToString(LineType),		/* PHIGS Line Type */
    StringToString(DistinguishMode),	/* PHIGS+ Face Distinguish Mode */
    StringToString(Point),		/* PHIGS 2D Point, 3D Point, */
    StringToString(Point3),
    StringToString(Vector),		/* 2D Vector, 3D Vector */
    StringToString(Vector3),
					/* PHIGS Integer List */
    { XtRString, PEXtRIntList, CvtStringToIntList, NULL, 0,
	XtCacheNone, NULL },
    ToString(IntList),
    StringToString(LightSourceType),	/* PHIGS Light Source Type */
					/* PHIGS Color Bundle */
    { XtRString, PEXtRColorBundle, CvtStringToColorBundle,
	colorConvertArgs, XtNumber(colorConvertArgs), XtCacheAll, NULL },
    StringToString(ClipIndicator),	/* PHIGS Clipping Indicator */
    StringToString(Limit),		/* PHIGS 2D Limit, 3D Limit */
    StringToString(Limit3),
    StringToString(DepthCueMode),	/* PHIGS Depth Cue Mode */
    StringToString(ResizePolicy),	/* Workstation Widget resize policy */
    StringToString(TraversalPolicy),	/* WS Widget traversal policy */
    StringTo(Gravity),			/* String => Gravity from Xaw */

#ifdef ES_PICK_EXTENSION
					/* E&S X11 Picking Extension */
    StringToString(PickMode),		/* XPick Pick Mode */
    StringToString(PickHighlightMode),	/* XPick Pick Highlighting Mode */
#endif					/* ES_PICK_EXTENSION */
  };
  register int i;

  if (initialized) return;
					/* initialize quarks */
  InitializeNameValuePairs(quark_list, XtNumber(quark_list));

  for (i = 0; i < XtNumber(converters); i++)
    XtSetTypeConverter(converters[i].from_type, converters[i].to_type,
		       converters[i].proc,
		       converters[i].args, converters[i].num_args,
		       converters[i].cache, converters[i].destroy);
  
  initialized = True;
}
