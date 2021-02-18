/* $Header: dpy_pc_info.c,v 1.1 91/09/18 13:50:05 sinyaw Exp $ */

/*****************************************************************
Copyright 1991 by Sony Microsystems Company, San Jose, California

					All Rights Reserved

Permission to use, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sony not be used
in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

SONY DISCLAIMS ANY AND ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL EXPRESS WARRANTIES AND ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, FOR A PARTICULAR PURPOSE. IN NO EVENT
SHALL SONY BE LIABLE FOR ANY DAMAGES OF ANY KIND, INCLUDING BUT NOT
LIMITED TO SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING FROM
LOSS OF USE, DATA OR LOSS OF ANY PAST, PRESENT, OR PROSPECTIVE PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.
*****************************************************************/

/* dpy_pc_info.c */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "Xpexlib.h"
#include "Xpexutil.h"

#define NameOfTextPath(_tp) \
(_tp == PEXPathRight) ? "PathRight" : \
(_tp == PEXPathLeft) ? "PathLeft" : \
(_tp == PEXPathUp) ? "PathUp" :  \
(_tp == PEXPathDown) ? "PathDown" : \
"BadTextPath"

#define NameOfValign(_v) \
(_v == PEXValignNormal) ? "ValignNormal" : \
(_v == PEXValignTop) ? "ValignTop" : \
(_v == PEXValignCap) ? " ValignCap" : \
(_v == PEXValignHalf) ? "ValignHalf" : \
(_v == PEXValignBase) ? " ValignBase": \
(_v == PEXValignBottom) ? "ValignBottom" : \
"BadValign"

#define NameOfHalign(_h) \
(_h == PEXHalignNormal) ? "HalignNormal" : \
(_h == PEXHalignLeft) ?  "HalignLeft" : \
(_h == PEXHalignCenter) ?  "HalignCenter" : \
(_h == PEXHalignRight) ?  "HalignRight" : \
"BadHalign"

#define NameOfTextPrecision(_tp) \
(_tp == PEXStringPrecision) ? "String" : \
(_tp == PEXCharPrecision) ? "Char" : \
(_tp == PEXStrokePrecision) ? "Stroke" : \
"BadTextPrecision"

#define NameOfMarkerType(_mt) \
(_mt == PEXMarkerDot) ? "MarkerDot" : \
(_mt == PEXMarkerCross) ? "MarkerCross" : \
(_mt == PEXMarkerAsterisk) ? "MarkerAsterisk" : \
(_mt == PEXMarkerCircle) ? "MarkerCircle" : \
(_mt == PEXMarkerX) ? "MarkerX" : \
"BadMarkerType"

#define NameOfAtextStyle(_ats) \
(_ats == PEXATextNotConnected) ? "ATextNotConnected" : \
(_ats == PEXATextConnected) ? "ATextConnected" : \
"BadAtextStyle"

#define NameOfInteriorStyle(_is) \
(_is == PEXInteriorStyleHollow) ? "InteriorStyleHollow" : \
(_is == PEXInteriorStyleSolid) ? "InteriorStyleSolid" : \
(_is == PEXInteriorStylePattern) ? "InteriorStylePattern" : \
(_is == PEXInteriorStyleHatch) ? "InteriorStyleHatch" : \
(_is == PEXInteriorStyleEmpty) ? "InteriorStyleEmpty" : \
"BadInteriorStyle"

#define NameOfLineType(_lt) \
(_lt == PEXLineTypeSolid) ? "LineTypeSolid" : \
(_lt == PEXLineTypeDashed) ? "LineTypeDashed" : \
(_lt == PEXLineTypeDotted) ? "LineTypeDotted" : \
(_lt == PEXLineTypeDashDot) ? "LineTypeDashDot" : \
"BadLineType"

#define NameOfPolylineInterp(_pi) \
(_pi == PEXPolylineInterpNone) ? "PolylineInterpNone" : \
(_pi == PEXPolylineInterpColour) ? "PolylineInterpColour" : \
"BadPolylineInterpMethod"

#define NameOfCurveApproxMethod(_cam) \
(_cam == PEXApproxImpDep) ? "ImpDep" : \
(_cam == PEXApproxConstantBetweenKnots) ? "ConstantBetweenKnots" : \
(_cam == PEXApproxWcsChordalSize) ? "WcsChordalSize" : \
(_cam == PEXApproxNpcChordalSize) ? "NpcChordalSize" : \
(_cam == PEXApproxDcChordalSize) ? "DcChordalSize" : \
(_cam == PEXCurveApproxWcsChordalDev) ? "WcsChordalDev" : \
(_cam == PEXCurveApproxNpcChordalDev) ? "NpcChordalDev" : \
(_cam == PEXCurveApproxDcChordalDev) ? "DcChordalDev" : \
(_cam == PEXApproxWcsRelative) ? "WcsRelative" : \
(_cam == PEXApproxNpcRelative) ? "NpcRelative" : \
(_cam == PEXApproxDcRelative) ? "DcRelative" : \
"BadCurveApproxMethod"

#define NameOfSurfaceApproxMethod(_sam) \
(_sam == PEXApproxImpDep) ? "ImpDep" : \
(_sam == PEXApproxConstantBetweenKnots) ? "ConstantBetweenKnots" : \
(_sam == PEXApproxWcsChordalSize) ? "WcsChordalSize" : \
(_sam == PEXApproxNpcChordalSize) ? "NpcChordalSize" : \
(_sam == PEXApproxDcChordalSize) ? "DcChordalSize" : \
(_sam == PEXSurfaceApproxWcsPlanarDev) ? "WcsPlanarDev" : \
(_sam == PEXSurfaceApproxNpcPlanarDev) ? "NpcPlanarDev" : \
(_sam == PEXSurfaceApproxDcPlanarDev) ? "DcPlanarDev" : \
(_sam == PEXApproxWcsRelative) ? "WcsRelative" : \
(_sam == PEXApproxNpcRelative) ? "NpcRelative" : \
(_sam == PEXApproxDcRelative) ? "DcRelative" : \
"BadSurfaceApproxMethod"

#define NameOfColorType(_t) \
(_t == PEXIndexedColour) ? "Indexed" : \
(_t == PEXRgbFloatColour) ? "RgbFloat" : \
(_t == PEXCieFloatColour) ? "CieFloat" : \
(_t == PEXHsvFloatColour) ? "HsvFloat" : \
(_t == PEXHlsFloatColour) ? "HlsFloat" : \
(_t == PEXRgb8Colour) ? "Rgb8" : \
(_t == PEXRgb16Colour) ? "Rgb16" : \
"BadColorType"

dpy_pc_info(dpy)
	Display *dpy;
{
	static char *pc_msg_str[] = {
		"Pipeline Context Attributes (Default Values)",
		"MarkerType",
		"MarkerScale",
		"MarkerColour",
		"MarkerBundleIndex",
		"TextFontIndex",
		"TextPrecision",
		"CharExpansion",
		"CharSpacing",
		"TextColour",
		"CharHeight",
		"CharUpVector",
		"TextPath",
		"TextAlignment",
		"AtextHeight",
		"AtextUpVector",
		"AtextPath",
		"AtextAlignment",
		"AtextStyle",
		"TextBundleIndex",
		"LineType",
		"LineWidth",
		"LineColour",
		"CurveApproximation",
		"PolylineInterp",
		"LineBundleIndex",
		"InteriorStyle",
		"InteriorStyleIndex",
		"SurfaceColour",
		"SurfaceReflAttr",
		"SurfaceReflModel",
		"SurfaceInterp",
		"BfInteriorStyle",
		"BfInteriorStyleIndex",
		"BfSurfaceColour",
		"BfSurfaceReflAttr",
		"BfSurfaceReflModel",
		"BfSurfaceInterp",
		"SurfaceApproximation",
		"CullingMode",
		"DistinguishFlag",
		"PatternSize",
		"PatternRefPt",
		"PatternRefVec1",
		"PatternRefVec2",
		"InteriorBundleIndex",
		"SurfaceEdgeFlag",
		"SurfaceEdgeType",
		"SurfaceEdgeWidth",
		"SurfaceEdgeColour",
		"EdgeBundleIndex",
		"LocalTransform",
		"GlobalTransform",
		"ModelClip",
		"ModelClipVolume",
		"ViewIndex",
		"LightState",
		"DepthCueIndex",
		"AsfValues",
		"PickId",
		"HlhsrIdentifier",
		"NameSet",
		"ColourApproxIndex",
		"RenderingColourModel",
		"ParaSurfCharacteristics"
	};
	XpexPCBitmask valueMask;
	XpexPC pc_id;
	XpexPCValues values;
	XpexPCValues valuesRet;
	CARD32 *data;

	(void) fprintf(stderr, "%s\n\n", pc_msg_str[0]);

	valueMask[0] = 0x0;
	valueMask[1] = 0x0;
	valueMask[2] = 0x0;

	pc_id = XpexCreatePC(dpy, valueMask, &values);

	valueMask[0] = XpexPC_MarkerType |
		XpexPC_MarkerScale |
		XpexPC_MarkerColor |
		XpexPC_MarkerBundleIndex |
		XpexPC_TextFont | 
		XpexPC_TextPrecision |
		XpexPC_CharExpansion |
		XpexPC_CharSpacing |
		XpexPC_TextColor | 
		XpexPC_CharHeight | 
		XpexPC_CharUpVector |
		XpexPC_TextPath |
		XpexPC_TextAlignment |
		XpexPC_AtextHeight |
		XpexPC_AtextUpVector |
		XpexPC_AtextPath |
		XpexPC_AtextAlignment |
		XpexPC_AtextStyle |
		XpexPC_TextBundleIndex |
		XpexPC_LineType |
		XpexPC_LineWidth |
		XpexPC_LineColor |
		XpexPC_CurveApproximation |
		XpexPC_PolylineInterp |
		XpexPC_LineBundleIndex |
		XpexPC_InteriorStyle |
		XpexPC_InteriorStyleIndex |
		XpexPC_SurfaceColor |
		XpexPC_SurfaceReflAttr |
		XpexPC_SurfaceReflModel |
		XpexPC_SurfaceInterp |
		XpexPC_BfInteriorStyle;
/*
	valueMask[0] = XpexPC_SurfaceColor;
	(void) _XpexGetPC(dpy, pc_id, valueMask, &data);
	fprintf(stderr,"SurfaceColorType: %d\n",*((pexEnumTypeIndex*)data));
	return;   */

	XpexGetPC(dpy, pc_id, valueMask, &valuesRet);

	(void) fprintf(stderr, "\t%s: %s(%d)\n",
		pc_msg_str[1], 
		NameOfMarkerType(valuesRet.marker_type),
		valuesRet.marker_type);

	(void) fprintf(stderr, "\t%s: %.1f\n",
		pc_msg_str[2], valuesRet.marker_scale);

	(void) fprintf(stderr, "\t%s: { %s(%d), %d }\n",
		pc_msg_str[3], 
		NameOfColorType(valuesRet.marker_color.color_type),
		valuesRet.marker_color.color_type, 
		valuesRet.marker_color.value.index);

	(void) fprintf(stderr, "\t%s: %d\n",
		pc_msg_str[4], 
		valuesRet.marker_bundle_index);

	(void) fprintf(stderr, "\t%s: %d\n",
		pc_msg_str[5], 
		valuesRet.text_font_index);

	(void) fprintf(stderr, "\t%s: %s(%d)\n", 
		pc_msg_str[6], 
		NameOfTextPrecision(valuesRet.text_precision),
		valuesRet.text_precision);

	(void) fprintf(stderr, "\t%s: %.1f\n",
		pc_msg_str[7], 
		valuesRet.char_expansion);

	(void) fprintf(stderr, "\t%s: %.1f\n",
		pc_msg_str[8], 
		valuesRet.char_spacing);

	(void) fprintf(stderr, "\t%s: {%s(%d), %d}\n",
		pc_msg_str[9], 
		NameOfColorType(valuesRet.text_color.color_type),
		valuesRet.text_color.color_type, 
		valuesRet.text_color.value.index);

	(void) fprintf(stderr, "\t%s: %.2f\n", 
		pc_msg_str[10], 
		valuesRet.char_height);

	(void) fprintf(stderr, "\t%s: <%.1f, %.1f>\n", 
		pc_msg_str[11], 
		valuesRet.char_up_vector.x, 
		valuesRet.char_up_vector.y);

	(void) fprintf(stderr, "\t%s: %s(%d)\n", 
		pc_msg_str[12], 
		NameOfTextPath(valuesRet.text_path), 
		valuesRet.text_path);

	(void) fprintf(stderr, "\t%s: { %s(%d), %s(%d) }\n", 
		pc_msg_str[13], 
		NameOfHalign(valuesRet.text_alignment.horizontal),
		valuesRet.text_alignment.horizontal, 
		NameOfValign(valuesRet.text_alignment.vertical),
		valuesRet.text_alignment.vertical);

	(void) fprintf(stderr, "\t%s: %.2f\n", 
		pc_msg_str[14], 
		valuesRet.atext_height);

	(void) fprintf(stderr, "\t%s: <%.1f, %.1f>\n", pc_msg_str[15], 
		valuesRet.atext_up_vector.x, 
		valuesRet.atext_up_vector.y);

	(void) fprintf(stderr, "\t%s: %s(%d)\n", 
		pc_msg_str[16], 
		NameOfTextPath(valuesRet.atext_path),
		valuesRet.atext_path);

	(void) fprintf(stderr, "\t%s: <%s(%d), %s(%d)>\n", 
		pc_msg_str[17], 
		NameOfHalign(valuesRet.atext_alignment.horizontal),
		valuesRet.atext_alignment.horizontal, 
		NameOfValign(valuesRet.atext_alignment.vertical),
		valuesRet.atext_alignment.vertical);

	(void) fprintf(stderr, "\t%s: %s(%d)\n", 
		pc_msg_str[18], 
		NameOfAtextStyle(valuesRet.atext_style),
		valuesRet.atext_style);

	(void) fprintf(stderr, "\t%s: %d\n", pc_msg_str[19], 
		valuesRet.text_bundle_index);

	(void) fprintf(stderr, "\t%s: %s(%d)\n", pc_msg_str[20], 
		NameOfLineType(valuesRet.line_type),
		valuesRet.line_type);

	(void) fprintf(stderr, "\t%s: %.1f\n", pc_msg_str[21], 
		valuesRet.line_width);

	(void) fprintf(stderr, "\t%s: {%s(%d), %d}\n", pc_msg_str[22], 
		NameOfColorType(valuesRet.line_color.color_type),
		valuesRet.line_color.color_type, 
		valuesRet.line_color.value.index);

	(void) fprintf(stderr, "\t%s: <%s(%d), %.1f>\n", pc_msg_str[23], 
		NameOfCurveApproxMethod(valuesRet.curve_approx.approxMethod),
		valuesRet.curve_approx.approxMethod, 
		valuesRet.curve_approx.tolerance);

	(void) fprintf(stderr, "\t%s: %s(%d)\n", pc_msg_str[24], 
		NameOfPolylineInterp(valuesRet.polyline_interp),
		valuesRet.polyline_interp);

	(void) fprintf(stderr, "\t%s: %d\n", pc_msg_str[25], 
		valuesRet.line_bundle_index);

	(void) fprintf(stderr, "\t%s: %s(%d)\n", pc_msg_str[26], 
		NameOfInteriorStyle(valuesRet.interior_style),
		valuesRet.interior_style);

	(void) fprintf(stderr, "\t%s: %d\n", pc_msg_str[27], 
		valuesRet.interior_style_index);

	XpexGetPC(dpy, pc_id, valueMask, &valuesRet);

	(void) fprintf(stderr, "\t%s: {%s(%d), %d}\n", 
		pc_msg_str[28], 
		NameOfColorType(valuesRet.surface_color.color_type),
		valuesRet.surface_color.color_type, 
		valuesRet.surface_color.value.index);

	(void) fprintf(stderr, 
		"\t%s: { %g, %g, %g, %g, %g, {%s(%d), %d}}\n", 
		pc_msg_str[29], 
		valuesRet.reflection_attr.ambient,
		valuesRet.reflection_attr.diffuse,
		valuesRet.reflection_attr.specular,
		valuesRet.reflection_attr.specular_conc,
		valuesRet.reflection_attr.transmission,
		NameOfColorType(valuesRet.reflection_attr.specular_color.color_type),
		valuesRet.reflection_attr.specular_color.color_type,
		valuesRet.reflection_attr.specular_color.value.index);

	(void) fprintf(stderr,
		"\t%s: %d\n", pc_msg_str[30],
		valuesRet.reflection_model);

	(void) fprintf(stderr,
		"\t%s: %d\n", pc_msg_str[31],
		valuesRet.surface_interp);

	(void) fprintf(stderr,
		"\t%s: %d\n", pc_msg_str[32],
		valuesRet.bf_interior_style);

	XpexFreePC(dpy, pc_id);
}
