/*
			   S t r i n g D e f s . h

    $Header: StringDefs.h,v 1.0 91/10/04 17:01:04 rthomson Exp $

    This file contains string definitions for resource names, class names,
    enumerated resource values, etc.

    Author: Rich Thomson
    Date: Wed May 23 21:17:48 MDT 1990

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

#ifndef PEXt_StringDefs_h
#define PEXt_StringDefs_h

					/* PHIGS Resource Classes */
#define PEXtCClipIndicator "ClipIndicator"
#define PEXtCColorBundle "ColorBundle"
#define PEXtCControlFlag "ControlFlag"
#define PEXtCCullMode "CullMode"
#define PEXtCDeferralMode "DeferralMode"
#define PEXtCDepthCueMode "DepthCueMode"
#define PEXtCDistinguishMode "DistinguishMode"
#define PEXtCEdgeFlag "EdgeFlag"
#define PEXtCGeneralColor "GeneralColor"
#define PEXtCHLHSRMode "HLHSRMode"
#define PEXtCInteriorStyle "InteriorStyle"
#define PEXtCIntList "IntList"
#define PEXtCLightSourceType "LightSourceType"
#define PEXtCLimit "Limit"
#define PEXtCLimit3 "Limit3"
#define PEXtCLineType "LineType"
#define PEXtCModificationMode "ModificationMode"
#define PEXtCPoint "Point"
#define PEXtCPoint3 "Point3"
#define PEXtCProjectionType "ProjectionType"
#define PEXtCReflectanceEquation "ReflectanceEquation"
#define PEXtCShadingMethod "ShadingMethod"
#define PEXtCVector "Vector"
#define PEXtCVector3 "Vector3"

#define PEXtCResizePolicy "ResizePolicy"
#define PEXtCResizeCallback "ResizeCallback"
#define PEXtCGravity "Gravity"
#define PEXtCTraversalPolicy "TraversalPolicy"

					/* X Extensions Resource Classes */
#define PEXtCPickMode "PickMode"
#define PEXtCPickHighlightMode "PickHighlightMode"

/*
  Representation Types
*/
					/* PEXt Resource Name */
#define PEXtRResizePolicy "PEXtResizePolicy"
#define PEXtRTraversalPolicy "PEXtTraversalPolicy"

					/* PHIGS Resource Names */
#define PEXtRClipIndicator "PEXtClipIndicator"
#define PEXtRColorBundle "PEXtColorBundle"
#define PEXtRControlFlag "PEXtControlFlag"
#define PEXtRCullMode "PEXtCullMode"
#define PEXtRDeferralMode "PEXtDeferralMode"
#define PEXtRDepthCueMode "PEXtDepthCueMode"
#define PEXtRDistinguishMode "PEXtDistinguishMode"
#define PEXtREdgeFlag "PEXtEdgeFlag"
#define PEXtRGeneralColor "PEXtGeneralColor"
#define PEXtRGravity "PEXtGravity"
#define PEXtRHLHSRMode "PEXtHLHSRMode"
#define PEXtRInteriorStyle "PEXtInteriorStyle"
#define PEXtRIntList "PEXtIntList"
#define PEXtRLightSourceType "PEXtLightSourceType"
#define PEXtRLimit "PEXtLimit"
#define PEXtRLimit3 "PEXtLimit3"
#define PEXtRLineType "PEXtLineType"
#define PEXtRModificationMode "PEXtModificationMode"
#define PEXtRPoint "PEXtPoint"
#define PEXtRPoint3 "PEXtPoint3"
#define PEXtRProjectionType "PEXtProjectionType"
#define PEXtRReflectanceEquation "PEXtReflectanceEquation"
#define PEXtRShadingMethod "PEXtShadingMethod"
#define PEXtRVector "PEXtVector"
#define PEXtRVector3 "PEXtVector3"
#define PEXtRWorkstationType "PEXtWorkstationType"

					/* X Extensions Resource Names */
#define PEXtRPickMode "PEXtPickMode"
#define PEXtRPickHighlightMode "PEXtPickHighlightMode"

/*
  Boolean enumerations
*/
#define PEXtE0 "0"
#define PEXtE1 "1"

/*
  Workstation resize policies
*/
#define PEXtEcallback "callback"
#define PEXtEporthole "porthole"
#define PEXtEcontainer "container"
#define PEXtDefaultResizePolicy "pextdefaultresizepolicy"

/*
  Workstation traversal policies
*/
#define PEXtEredraw "redraw"
#define PEXtDefaultTraversalPolicy "pextdefaulttraversalpolicy"

/*
  Workstation Type enumerated values
*/
#define PEXtEtool "tool"
#define PEXtEdrawable "drawable"
#define PEXtDefaultWorkstationType "pextdefaultworkstationtype"

/*
  General Color type enumerated values
*/
#define PEXtEindirect "indirect"
#define PEXtErgb "rgb"
#define PEXtEcie "cie"
#define PEXtEhls "hls"
#define PEXtEhsv "hsv"
#define PEXtDefaultGeneralColor "pextdefaultgeneralcolor"

/*
  Interior Style enumerated values
*/
#define PEXtEhollow "hollow"
#define PEXtEsolid "solid"
#define PEXtEpattern "pattern"
#define PEXtEhatch "hatch"
#define PEXtEempty "empty"

/*
  Face Culling Mode enumerated values
*/
#define PEXtEcullBackFace "backface"
#define PEXtEcullFrontFace "frontface"

/*
  Projection Type enumerated values
*/
#define PEXtEperspective "perspective"
#define PEXtEparallel "parallel"

/*
  Reflectance Equation enumerated values
*/
#define PEXtEnone "none"
#define PEXtEambient "ambient"
#define PEXtEdiffuse "diffuse"
#define PEXtEspecular "specular"

/*
  Shading Method enumerated values

  (Shading method none is already defined as PEXtEnone)
*/
#define PEXtEcolor "color"
#define PEXtEdotProduct "dotproduct"
#define PEXtEnormal "normal"

/*
  Line Type enumerated values

  (Line Type solid is already defined as PEXtEsolid)
*/
#define PEXtEdash "dash"
#define PEXtEdot "dot"
#define PEXtEdashDot "dashdot"

/*
  Light Source Type enumerated values
*/
					/* ambient already defined */
#define PEXtEdirectional "directional"
#define PEXtEpositional "positional"
#define PEXtEspot "spot"

/*
  Clip Indicator enumerated values
*/
#define PEXtEclip "clip"
#define PEXtEnoClip "noclip"
#define PEXtDefaultClipIndicator "pextdefaultclipindicator"

/*
  Hidden Line, Hidden Surface Removal Modes
*/
#define PEXtEzBuffer "zbuffer"

/*
  Workstation Control Flag
*/
#define PEXtDefaultControlFlag "pextdefaultcontrolflag"
#define PEXtEconditionally "conditionally"
#define PEXtEalways "always"

/*
  Workstation Deferral Mode
*/
#define PEXtDefaultDeferralMode "pextdefaultdeferralmode"
#define PEXtEasap "asap"
#define PEXtEasSoonAsPossible "assoonaspossible"
#define PEXtEbnig "bnig"
#define PEXtEbeforeNextInteractionGlobally "beforenextinteractionglobally"
#define PEXtEbnil "bnil"
#define PEXtEbeforeNextInteractionLocally "beforenextinteractionlocally"
#define PEXtEasti "asti"
#define PEXtEatSomeTime "atsometime"
#define PEXtEwait "wait"
#define PEXtEwhenApplicationRequestsIt "whenapplicationrequestsit"

/*
  Workstation modification mode
*/
#define PEXtDefaultModificationMode "pextdefaultmodificationmode"
#define PEXtEnive "nive"
#define PEXtEnoImmediateVisualEffect "noimmediatevisualeffect"
#define PEXtEuwor "uwor"
#define PEXtEupdateWithoutRegeneration "updatewithoutregeneration"
#define PEXtEuqum "uqum"
#define PEXtEuseQuickUpdateMethods "usequickupdatemethods"

/*
  Depth Cue Mode
*/
#define PEXtEallowed "allowed"
#define PEXtEsuppressed "suppressed"


/*
  X Picking Extension Enumerated values
*/
#define PEXtEfirst "first"		/* picking modes */
#define PEXtElast "last"
#define PEXtEnear "near"
#define PEXtEfar "far"
#define PEXtEpickFirst "pickfirst"
#define PEXtEpickLast "picklast"
#define PEXtEpickNear "picknear"
#define PEXtEpickFar "pickfar"
#define PEXtDefaultPickMode "pextdefaultpickmode"

#define PEXtEitem "item"
#define PEXtEcommand "command"
#define PEXtEpickHighlightItem "highlightitem"
#define PEXtEpickHighlightCommand "highlightcommand"
#define PEXtEpickHighlightOff "highlightoff"
#define PEXtDefaultPickHighlightMode "pextdefaultpickhighlightmode"

/*
  Default resource values
*/
#define PEXtDefaultClipLimits "pextdefaultcliplimits"
#define PEXtDefaultConcentrationExponent "pextdefaultconcentrationexponent"
#define PEXtDefaultCullMode "pextdefaultcullmode"
#define PEXtDefaultDepthCueMode "pextdefaultdepthcuemode"
#define PEXtDefaultDistinguishMode "pextdefaultdistinguishmode"
#define PEXtDefaultEdgeFlag "pextdefaultedgeflag"
#define PEXtDefaultEdgeType "pextdefaultedgetype"
#define PEXtDefaultEdgeWidthScaleFactor "pextdefaultedgewidthscalefactor"
#define PEXtDefaultEdgeColorIndex "pextdefaultedgecolorindex"
#define PEXtDefaultGeneralColor "pextdefaultgeneralcolor"
#define PEXtDefaultGravity "pextdefaultgravity"
#define PEXtDefaultHLHSRMode "pextdefaulthlhsrmode"
#define PEXtDefaultInteriorStyle "pextdefaultinteriorstyle"
#define PEXtDefaultLightSourceType "pextdefaultlightsourcetype"
#define PEXtDefaultLineType "pextdefaultlinetype"
#define PEXtDefaultPositionalAttenuation0 "pextdefaultpositionalattenuation0"
#define PEXtDefaultPositionalAttenuation1 "pextdefaultpositionalattenuation1"
#define PEXtDefaultProjectionType "pextdefaultprojectiontype"
#define PEXtDefaultReflectanceEquation "pextdefaultreflectanceequation"
#define PEXtDefaultShadingMethod "pextdefaultshadingmethod"
#define PEXtDefaultSpotAttenuation0 "pextdefaultspotattenuation0"
#define PEXtDefaultSpotAttenuation1 "pextdefaultspotattenuation1"
#define PEXtDefaultSpreadAngle "pextdefaultspreadangle"
#define PEXtDefaultPickMode "pextdefaultpickmode"
#define PEXtDefaultPickHighlightMode "pextdefaultpickhighlightmode"

/*
  Other resource values
*/
#define PEXtOrigin "pextorigin"
#define PEXtUnitVectorX "pextunitvectorx"
#define PEXtUnitVectorY "pextunitvectory"
#define PEXtUnitVectorZ "pextunitvectorz"

/*
  Include the gravity string definitions -- yanked from <X11/Xmu/Converters.h>
*/
#define PEXtEforget "forget"
#define PEXtEnorthWest "northwest"
#define PEXtEnorth "north"
#define PEXtEnorthEast "northeast"
#define PEXtEwest "west"
#define PEXtEcenter "center"
#define PEXtEeast "east"
#define PEXtEsouthWest "southwest"
#define PEXtEsouth "south"
#define PEXtEsouthEast "southeast"
#define PEXtEleft "left"
#define PEXtEcenter "center"
#define PEXtEright "right"
#define PEXtEtop "top"
#define PEXtEbottom "bottom"

#endif					/* _PEXt_StringDefs_h */
