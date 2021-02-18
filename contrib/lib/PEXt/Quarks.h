/*
			      Q u a r k s . h

    $Header: Quarks.h,v 1.0 91/10/04 17:01:01 rthomson Exp $

    This file contains definitions for all quarks defined by the resource
    converters in ``Converters.c''.

    Author: Rich Thomson
    Date: Wed May 23 21:17:48 MDT 1990

    Editing History:
    10-Jul-90	RAT: Added quarks needed for CvtStringToControlFlag,
		CvtStringToDeferralMode, CvtStringToModificationMode.

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

#ifndef PEXt_Quarks_h
#define PEXt_Quarks_h

extern XrmQuark
  PEXtQEtool,				/* workstation types */
  PEXtQEdrawable,
  PEXtQpextDefaultWorkstationType,
  PEXtQEhollow,				/* interior styles */
  PEXtQEsolid,
  PEXtQEpattern,
  PEXtQEhatch,
  PEXtQEempty,
  PEXtQpextDefaultInteriorStyle,
  PEXtQEnone,
  PEXtQEcullBackFace,			/* face culling modes */
  PEXtQEcullFrontFace,
  PEXtQpextDefaultCullMode,
  PEXtQpextDefaultDistinguishMode,	/* face disgintuish mode */
  PEXtQEindirect,			/* generalized color */
  PEXtQErgb,
  PEXtQEhls,
  PEXtQEcie,
  PEXtQEhsv,
  PEXtQpextDefaultGeneralColor,
  PEXtQEparallel,			/* projection types */
  PEXtQEperspective,
  PEXtQpextDefaultProjectionType,
  PEXtQEambient,			/* reflectance equations */
  PEXtQEdiffuse,
  PEXtQEspecular,
  PEXtQpextDefaultReflectanceEquation,
  PEXtQEcolor,				/* shading methods */
  PEXtQEdotProduct,
  PEXtQEnormal,
  PEXtQpextDefaultShadingMethod,
  PEXtQEsolid,				/* line types */
  PEXtQEdash,
  PEXtQEdot,
  PEXtQEdotDash,
  PEXtQpextDefaultLineType,
  PEXtQEdirectional,			/* light source types */
  PEXtQEpositional,
  PEXtQEspot,
  PEXtQpextDefaultLightSourceType,
  PEXtQpextDefaultEdgeFlag,		/* edge flag */
  PEXtQptOrigin,			/* origin */
  PEXtQpextDefaultClipLimits,		/* [0..1] in each direction */
  PEXtQEclip,				/* clip flags */
  PEXtQEnoClip,
  PEXtQpextDefaultClipFlag,
  PEXtQptUnitVectorX,			/* unit vectors */
  PEXtQptUnitVectorY,
  PEXtQptUnitVectorZ,
  PEXtQpextDefaultHLHSRMode,		/* hidden line, surf. removal modes */
  PEXtQEzBuffer,
  PEXtQpextDefaultControlFlag,		/* workstation control flag */
  PEXtQEconditionally,
  PEXtQEalways,
  PEXtQpextDefaultDeferralMode,		/* workstation deferral mode */
  PEXtQEasap, PEXtQEasSoonAsPossible,
  PEXtQEbnig, PEXtQEbeforeNextInteractionGlobally,
  PEXtQEbnil, PEXtQEbeforeNextInteractionLocally,
  PEXtQEasti, PEXtQEatSomeTime,
  PEXtQEwait, PEXtQEwhenApplicationRequestsIt,
  PEXtQpextDefaultModificationMode,	/* workstation modification mode */
  PEXtQEnive, PEXtQEnoImmediateVisualEffect,
  PEXtQEuwor, PEXtQEupdateWithoutRegeneration,
  PEXtQEuqum, PEXtQEuseQuickUpdateMethods,
  PEXtQEallowed,			/* depth cue modes */
  PEXtQEsuppressed,
  PEXtQpextDefaultDepthCueMode,

  /* Note: the following Quarks will only be available if the library is */
  /* compiled with -DES_PICK_EXTENSION; ultimately they should be migrated */
  /* into the extension itself (i.e. the extension library should become Xt */
  /* literate) */

					/* E&S pick extension: pick modes */
  PEXtQEfirst, PEXtQElast, PEXtQEnear, PEXtQEfar, PEXtQEpickFirst,
  PEXtQEpickLast, PEXtQEpickNear, PEXtQEpickFar, PEXtQpextDefaultPickMode,
					/* pick highlighting modes */
  PEXtQEitem, PEXtQEcommand, PEXtQEpickHighlightItem,
  PEXtQEpickHighlightCommand, PEXtQEpickHighlightOff,
  PEXtQpextDefaultPickHighlightMode;

#endif
