/* $Header: dpy_lut_info.c,v 1.1 91/09/18 13:50:05 sinyaw Exp $ */

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

/*
 *
 */
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "Xpexlib.h"
#include "Xpexutil.h"


static XpexLookupTable lineBundleLUT;
static XpexLookupTable markerBundleLUT;
static XpexLookupTable textBundleLUT;
static XpexLookupTable interiorBundleLUT;
static XpexLookupTable edgeBundleLUT;
static XpexLookupTable patternLUT;
static XpexLookupTable textFontLUT;
static XpexLookupTable colorLUT;
static XpexLookupTable lightLUT;
static XpexLookupTable depthCueLUT;
static XpexLookupTable colorApproxLUT;

#define XpexGetLineBundleTableInfo(_dpy,_drawable,_info) \
	XpexGetTableInfo((_dpy),(_drawable),Xpex_LineBundleLUT,(_info))

#define XpexGetMarkerBundleTableInfo(_dpy,_drawable,_info) \
	XpexGetTableInfo((_dpy),(_drawable),Xpex_MarkerBundleLUT,(_info))

#define XpexGetTextBundleTableInfo(_dpy,_drawable,_info) \
	XpexGetTableInfo((_dpy),(_drawable),Xpex_TextBundleLUT,(_info))

#define XpexGetInteriorBundleTableInfo(_dpy,_drawable,_info) \
	XpexGetTableInfo((_dpy),(_drawable),Xpex_InteriorBundleLUT,(_info))

#define XpexGetEdgeBundleTableInfo(_dpy,_drawable,_info) \
	XpexGetTableInfo((_dpy),(_drawable),Xpex_EdgeBundleLUT,(_info))

#define XpexGetPatternTableInfo(_dpy,_drawable,_info) \
	XpexGetTableInfo((_dpy),(_drawable),Xpex_PatternLUT,(_info))

#define XpexGetTextFontTableInfo(_dpy,_drawable,_info) \
	XpexGetTableInfo((_dpy),(_drawable),Xpex_TextFontLUT,(_info))

#define XpexGetColorTableInfo(_dpy,_drawable,_info) \
	XpexGetTableInfo((_dpy),(_drawable),Xpex_ColorLUT,(_info))

#define XpexGetViewTableInfo(_dpy,_drawable,_info) \
	XpexGetTableInfo((_dpy),(_drawable),Xpex_ViewLUT,(_info))

#define XpexGetLightTableInfo(_dpy,_drawable,_info) \
	XpexGetTableInfo((_dpy),(_drawable),Xpex_LightLUT,(_info))

#define XpexGetDepthCueTableInfo(_dpy,_drawable,_info) \
	XpexGetTableInfo((_dpy),(_drawable),Xpex_DepthCueLUT,(_info))

#define XpexGetColorApproxTableInfo(_dpy,_drawable,_info) \
	XpexGetTableInfo((_dpy),(_drawable),Xpex_ColorApproxLUT,(_info))

dpy_lut_info(dpy)
	Display *dpy;
{
	int screen = DefaultScreen(dpy);
	Drawable drawable = RootWindow(dpy,screen);

	XpexTableInfo table_info;
	
	int actual_count;
	register long i;
	char *predef_entries;

	markerBundleLUT = XpexCreateMarkerBundleLUT(dpy, drawable);
	lineBundleLUT = XpexCreateLineBundleLUT(dpy, drawable);
	textBundleLUT = XpexCreateTextBundleLUT(dpy, drawable);
	interiorBundleLUT = XpexCreateInteriorBundleLUT(dpy, drawable);
	edgeBundleLUT = XpexCreateEdgeBundleLUT(dpy, drawable);
	patternLUT = XpexCreatePatternLUT(dpy, drawable);
	textFontLUT = XpexCreateTextFontLUT(dpy, drawable);
	colorLUT = XpexCreateColorLUT(dpy, drawable);
	lightLUT = XpexCreateLightLUT(dpy, drawable);
	depthCueLUT = XpexCreateDepthCueLUT(dpy, drawable);
	colorApproxLUT = XpexCreateColorApproxLUT(dpy, drawable);

	XpexGetLineBundleTableInfo(dpy, drawable, &table_info);

	(void) fprintf(stderr,"\nLookup Table Information\n\n");

	(void) fprintf(stderr, "\tLineBundle.definableEntries: %d\n", 
	table_info.definableEntries);

	(void) fprintf(stderr, "\tLineBundle.numPredefined: %d\n",
	table_info.numPredefined);

	(void) fprintf(stderr, "\tLineBundle.predefinedMin: %d\n",
	table_info.predefinedMin);

	(void) fprintf(stderr, "\tLineBundle.predefinedMax: %d\n\n",
	table_info.predefinedMax);

	XpexGetMarkerBundleTableInfo(dpy, drawable, &table_info);

	(void) fprintf(stderr, "\tMarkerBundle.definableEntries: %d\n", 
	table_info.definableEntries);

	(void) fprintf(stderr, "\tMarkerBundle.numPredefined: %d\n",
	table_info.numPredefined);

	(void) fprintf(stderr, "\tMarkerBundle.predefinedMin: %d\n",
	table_info.predefinedMin);

	(void) fprintf(stderr, "\tMarkerBundle.predefinedMax: %d\n\n",
	table_info.predefinedMax);

	XpexGetTextBundleTableInfo(dpy, drawable, &table_info);

	(void) fprintf(stderr, "\tTextBundle.definableEntries: %d\n", 
	table_info.definableEntries);

	(void) fprintf(stderr, "\tTextBundle.numPredefined: %d\n",
	table_info.numPredefined);

	(void) fprintf(stderr, "\tTextBundle.predefinedMin: %d\n",
	table_info.predefinedMin);

	(void) fprintf(stderr, "\tTextBundle.predefinedMax: %d\n\n",
	table_info.predefinedMax);

	XpexGetInteriorBundleTableInfo(dpy, drawable, &table_info);

	(void) fprintf(stderr, "\tInteriorBundle.definableEntries: %d\n", 
	table_info.definableEntries);

	(void) fprintf(stderr, "\tInteriorBundle.numPredefined: %d\n",
	table_info.numPredefined);

	(void) fprintf(stderr, "\tInteriorBundle.predefinedMin: %d\n",
	table_info.predefinedMin);

	(void) fprintf(stderr, "\tInteriorBundle.predefinedMax: %d\n\n",
	table_info.predefinedMax);

	XpexGetEdgeBundleTableInfo(dpy, drawable, &table_info);

	(void) fprintf(stderr, "\tEdgeBundle.definableEntries: %d\n", 
	table_info.definableEntries);

	(void) fprintf(stderr, "\tEdgeBundle.numPredefined: %d\n",
	table_info.numPredefined);

	(void) fprintf(stderr, "\tEdgeBundle.predefinedMin: %d\n",
	table_info.predefinedMin);

	(void) fprintf(stderr, "\tEdgeBundle.predefinedMax: %d\n\n",
	table_info.predefinedMax);

	XpexGetPatternTableInfo(dpy, drawable, &table_info);

	(void) fprintf(stderr, "\tPattern.definableEntries: %d\n", 
	table_info.definableEntries);

	(void) fprintf(stderr, "\tPattern.numPredefined: %d\n",
	table_info.numPredefined);

	(void) fprintf(stderr, "\tPattern.predefinedMin: %d\n",
	table_info.predefinedMin);

	(void) fprintf(stderr, "\tPattern.predefinedMax: %d\n\n",
	table_info.predefinedMax);

	XpexGetTextFontTableInfo(dpy, drawable, &table_info);

	(void) fprintf(stderr, "\tTextFont.definableEntries: %d\n", 
	table_info.definableEntries);

	(void) fprintf(stderr, "\tTextFont.numPredefined: %d\n",
	table_info.numPredefined);

	(void) fprintf(stderr, "\tTextFont.predefinedMin: %d\n",
	table_info.predefinedMin);

	(void) fprintf(stderr, "\tTextFont.predefinedMax: %d\n\n",
	table_info.predefinedMax);

	XpexGetColorTableInfo(dpy, drawable, &table_info);

	(void) fprintf(stderr, "\tColor.definableEntries: %d\n", 
	table_info.definableEntries);

	(void) fprintf(stderr, "\tColor.numPredefined: %d\n",
	table_info.numPredefined);

	(void) fprintf(stderr, "\tColor.predefinedMin: %d\n",
	table_info.predefinedMin);

	(void) fprintf(stderr, "\tColor.predefinedMax: %d\n\n",
	table_info.predefinedMax);

	XpexGetViewTableInfo(dpy, drawable, &table_info);

	(void) fprintf(stderr, "\tView.definableEntries: %d\n", 
	table_info.definableEntries);

	(void) fprintf(stderr, "\tView.numPredefined: %d\n",
	table_info.numPredefined);

	(void) fprintf(stderr, "\tView.predefinedMin: %d\n",
	table_info.predefinedMin);

	(void) fprintf(stderr, "\tView.predefinedMax: %d\n\n",
	table_info.predefinedMax);

	XpexGetLightTableInfo(dpy, drawable, &table_info);

	(void) fprintf(stderr, "\tLight.definableEntries: %d\n", 
	table_info.definableEntries);

	(void) fprintf(stderr, "\tLight.numPredefined: %d\n",
	table_info.numPredefined);

	(void) fprintf(stderr, "\tLight.predefinedMin: %d\n",
	table_info.predefinedMin);

	(void) fprintf(stderr, "\tLight.predefinedMax: %d\n\n",
	table_info.predefinedMax);

	XpexGetDepthCueTableInfo(dpy, drawable, &table_info);

	(void) fprintf(stderr, "\tDepthCue.definableEntries: %d\n", 
	table_info.definableEntries);

	(void) fprintf(stderr, "\tDepthCue.numPredefined: %d\n",
	table_info.numPredefined);

	(void) fprintf(stderr, "\tDepthCue.predefinedMin: %d\n",
	table_info.predefinedMin);

	(void) fprintf(stderr, "\tDepthCue.predefinedMax: %d\n\n",
	table_info.predefinedMax);

	XpexGetColorApproxTableInfo(dpy, drawable, &table_info);

	(void) fprintf(stderr, "\tColorApprox.definableEntries: %d\n", 
	table_info.definableEntries);

	(void) fprintf(stderr, "\tColorApprox.numPredefined: %d\n",
	table_info.numPredefined);

	(void) fprintf(stderr, "\tColorApprox.predefinedMin: %d\n",
	table_info.predefinedMin);

	(void) fprintf(stderr, "\tColorApprox.predefinedMax: %d\n\n",
	table_info.predefinedMax);

	(void) fprintf(stderr, "Predefined Table Entries\n\n");

	XpexGetPredefinedEntries(dpy, drawable, Xpex_LineBundleLUT,
	0, 1, &predef_entries, &actual_count);
	{
		pexLineBundleDataIndexed *lbe = 
		(pexLineBundleDataIndexed *) predef_entries;

		(void) fprintf(stderr, "\tLineBundle.line_type: %d\n", 
		lbe->lineType);

		(void) fprintf(stderr, "\tLineBundle.polyline_interp: %d\n",
		lbe->polylineInterp);

		(void) fprintf(stderr, 
		"\tLineBundle.curve_approx: {%d, %.1f}\n",
		lbe->curveApprox.approxMethod,
		lbe->curveApprox.tolerance);

		(void) fprintf(stderr, 
		"\tLineBundle.line_width: %.1f\n", lbe->lineWidth);

		(void) fprintf(stderr, 
		"\tLineBundle.line_color: {%s, %d}\n\n",
		(lbe->colourType) ? "NotIndexed" : "Indexed",
		lbe->colourIndex);
	}

	XpexGetPredefinedEntries(dpy, drawable, Xpex_MarkerBundleLUT,
	0, 1, &predef_entries, &actual_count);
	{
		pexMarkerBundleDataIndexed *mbe = (pexMarkerBundleDataIndexed *)
		predef_entries;

		(void) fprintf(stderr, 
		"\tMarkerBundle.marker_type: %d\n", mbe->markerType);

		(void) fprintf(stderr, 
		"\tMarkerBundle.marker_scale: %.1f\n", mbe->markerScale);

		(void) fprintf(stderr, 
		"\tMarkerBundle.marker_color: {%s, %d}\n\n",
		(mbe->colourType) ? "NotIndexed" : "Indexed",
		mbe->colourIndex);
	}

	XpexGetPredefinedEntries(dpy, drawable, Xpex_TextBundleLUT,
	0, 1, &predef_entries, &actual_count);
	{
		pexTextBundleDataIndexed *tbe = 
		(pexTextBundleDataIndexed *) predef_entries;

		(void) fprintf(stderr, "\tTextBundle.text_font_index: %d\n",
		tbe->textFontIndex);

		(void) fprintf(stderr, "\tTextBundle.text_precision: %d\n",
		tbe->textPrecision);

		(void) fprintf(stderr, "\tTextBundle.char_expansion: %.1f\n",
		tbe->charExpansion);

		(void) fprintf(stderr, "\tTextBundle.char_spacing: %.1f\n",
		tbe->charSpacing);

		(void) fprintf(stderr, 
		"\tTextBundle.text_color: {%s, %d}\n\n",
		(tbe->colourType) ? "NotIndexed" : "Indexed",
		tbe->colourIndex);
	}

	XpexGetPredefinedEntries(dpy, drawable, Xpex_InteriorBundleLUT,
	0, 1, &predef_entries, &actual_count);

	dpy_interior_bundle(predef_entries);

	XpexGetPredefinedEntries(dpy, drawable, Xpex_EdgeBundleLUT,
	0, 1, &predef_entries, &actual_count);

	dpy_edge_bundle(predef_entries);

	XpexGetPredefinedEntries(dpy, drawable, Xpex_PatternLUT,
	0, 1, &predef_entries, &actual_count);

	if (!actual_count) {
		fprintf(stderr,"PatternEntry NotSupported\n\n");
	} else {
		dpy_pattern_entry(predef_entries);
	}

	XpexGetPredefinedEntries(dpy, drawable, Xpex_ColorLUT,
	0, 8, &predef_entries, &actual_count);

	{
		XpexColorEntryRgb *ce = (XpexColorEntryRgb *) predef_entries;

		for (i = 0; i < actual_count; i++) {
			(void) fprintf(stderr, 
			"\tColorTableIndex(%d)\tRed: %.1f Green: %.1f Blue: %.1f\n",
			i, ce[i].red, ce[i].green, ce[i].blue);
		}
		(void) fprintf(stderr, "\n");
	}

	XpexGetPredefinedEntries(dpy, drawable, Xpex_TextFontLUT,
	0, 1, &predef_entries, &actual_count);

	dpy_text_font_entry(predef_entries);

	XpexGetPredefinedEntries(dpy, drawable, Xpex_ViewLUT,
	0, 1, &predef_entries, &actual_count);

	dpy_view_entry(predef_entries);

	XpexGetPredefinedEntries(dpy, drawable, Xpex_LightLUT,
	0, 1, &predef_entries, &actual_count);

	dpy_light_entry(predef_entries);

	XpexGetPredefinedEntries(dpy, drawable, Xpex_DepthCueLUT,
	0, 1, &predef_entries, &actual_count);

	dpy_depth_cue_entry(predef_entries);

	XpexGetPredefinedEntries(dpy, drawable, Xpex_ColorApproxLUT,
	0, 1, &predef_entries, &actual_count);
	
	dpy_color_approx_entry(predef_entries);

	clean_up(dpy);
}

dpy_interior_bundle(entry)
	char *entry;
{
	register pexInteriorBundleDataIndexed *ibe = 
	(pexInteriorBundleDataIndexed *) entry;

	(void) fprintf(stderr, 
	"\tInteriorBundle.interior_style: %d\n",
	ibe->interiorStyle);

	(void) fprintf(stderr, 
	"\tInteriorBundle.interior_style_index: %d\n",
	ibe->interiorStyleIndex);

	(void) fprintf(stderr, 
	"\tInteriorBundle.surface_color: {%s, %d}\n",
	(ibe->surfaceColourType) ? "NotIndexed" : "Indexed",
	ibe->surfaceColourIndex);

	(void) fprintf(stderr,
	"\tInteriorBundle.reflection_attr: { %1.f, %1.f, %1.f, %1.f, %1.f, (%s, %d)}\n", 
	ibe->reflectionAttr.ambient,
	ibe->reflectionAttr.diffuse,
	ibe->reflectionAttr.specular,
	ibe->reflectionAttr.specularConc,
	ibe->reflectionAttr.transmission,
	(ibe->reflectionAttr.colourType) ? "NotIndexed" : "Indexed",
	ibe->reflectionAttr.colourIndex);

	(void) fprintf(stderr, 
	"\tInteriorBundle.reflection_model: %d\n",
	ibe->reflectionModel);

	(void) fprintf(stderr, 
	"\tInteriorBundle.surface_interp: %d\n",
	ibe->surfaceInterp);

	(void) fprintf(stderr, 
	"\tInteriorBundle.bf_interior_style: %d\n",
	ibe->bfInteriorStyle);

	(void) fprintf(stderr, 
	"\tInteriorBundle.bf_interior_style_index: %d\n",
	ibe->bfInteriorStyleIndex);

	(void) fprintf(stderr, 
	"\tInteriorBundle.bf_surface_color: {%s, %d}\n",
	(ibe->bfSurfaceColourType) ? "NotIndexed" : "Indexed",
	ibe->bfSurfaceColourIndex);

	(void) fprintf(stderr,
	"\tInteriorBundle.bf_reflection_attr: { %1.f, %1.f, %1.f, %1.f, %1.f, (%s, %d)}\n", 
	ibe->bfReflectionAttr.ambient,
	ibe->bfReflectionAttr.diffuse,
	ibe->bfReflectionAttr.specular,
	ibe->bfReflectionAttr.specularConc,
	ibe->bfReflectionAttr.transmission,
	(ibe->bfReflectionAttr.colourType) ? "NotIndexed" : "Indexed",
	ibe->bfReflectionAttr.colourIndex);

	(void) fprintf(stderr, 
	"\tInteriorBundle.bf_reflection_model: %d\n",
	ibe->bfReflectionModel);

	(void) fprintf(stderr, 
	"\tInteriorBundle.bf_surface_interp: %d\n",
	ibe->bfSurfaceInterp);

	(void) fprintf(stderr,
	"\tInteriorBundle.surface_approx: {%d, %1.f, %1.f}\n\n",
	ibe->surfaceApprox.approxMethod,
	ibe->surfaceApprox.uTolerance, ibe->surfaceApprox.vTolerance);
}

dpy_edge_bundle(entry)
	char *entry;
{
	register pexEdgeBundleDataIndexed *eb = 
	(pexEdgeBundleDataIndexed *) entry;

	(void) fprintf(stderr, 
	"\tEdgeBundle.surface_edges: %s(%d)\n", 
	(eb->edges) ? "On" : "Off", eb->edges);

	(void) fprintf(stderr, 
	"\tEdgeBundle.surface_edge_type: %s(%d)\n", 
	(eb->edgeType == 1) ? "SurfaceEdgeSolid" : "NotSurfaceEdgeSolid",
	eb->edgeType);

	(void) fprintf(stderr, 
	"\tEdgeBundle.surface_edge_width: %.1f\n", eb->edgeWidth);
	(void) fprintf(stderr, 
	"\tEdgeBundle.surface_edge_color: {%s(%d), %d}\n\n", 
	(eb->colourType) ? "NotIndexed" : "Indexed",
	eb->colourType, eb->colourIndex);
}

dpy_pattern_entry(entry)
	char *entry;
{
	pexPatternEntry *pe = (pexPatternEntry *) entry;

	(void) fprintf(stderr, "\tPattern.colourType: %s(%d)\n",
	(pe->colourType) ? "NotIndexed" : "Indexed",
	pe->colourType); 

	(void) fprintf(stderr, "\tPattern.numx: %d\n",
	pe->numx); 

	(void) fprintf(stderr, "\tPattern.numy: %d\n\n",
	pe->numy); 
}

dpy_text_font_entry(entry)
	char *entry;
{
	register int i;
	pexTextFontEntry *tfe = (pexTextFontEntry *) entry;
	pexFont *fonts;

	(void) fprintf(stderr, "\tTextFont.numFonts: %u\n\n",
	tfe->numFonts);
}

dpy_view_entry(entry)
	char *entry;
{
	pexViewEntry *ve = (pexViewEntry *) entry;

	(void) fprintf(stderr, "\tView.clip_flags: 0x%x\n",
	ve->clipFlags);

	(void) fprintf(stderr, "\tView.clip_limits: (%g,%g,%g),(%g,%g,%g)\n",
	ve->clipLimits.minval.x, ve->clipLimits.minval.y, ve->clipLimits.minval.z,
	ve->clipLimits.maxval.x, ve->clipLimits.maxval.y, ve->clipLimits.maxval.z);
/*
	(void) fprintf(stderr, "\tView.orientation: 0x%x\n",
	ve->orientation);

	(void) fprintf(stderr, "\tView.mapping: 0x%x\n",
	ve->mapping);
*/
	(void) fprintf(stderr, "\n");
}

dpy_light_entry(entry)
	char *entry;
{
	pexLightEntry *le = (pexLightEntry *) entry;
	
	(void) fprintf(stderr, "\tLight.lightType: %d\n",
	le->lightType);

	(void) fprintf(stderr, "\tLight.direction: (%g, %g, %g)\n",
	le->direction.x, le->direction.y, le->direction.z);

	(void) fprintf(stderr, "\tLight.point: (%g, %g, %g)\n",
	le->point.x, le->point.y, le->point.z);

	(void) fprintf(stderr, "\tLight.concentration: %g\n",
	le->concentration);

	(void) fprintf(stderr, "\tLight.spread_angle: %g\n",
	le->spreadAngle);

	(void) fprintf(stderr, "\tLight.attenuation: (%g,%g)\n",
	le->attenuation1, le->attenuation2);

	(void) fprintf(stderr, "\tLight.colorType: %s(%d)\n",
	(le->lightColour.colourType) ? "NotIndexed" : "Indexed",
	le->lightColour.colourType);

	(void) fprintf(stderr, "\n");
}

dpy_depth_cue_entry(entry)
	char *entry;
{
	pexDepthCueEntry *dce = (pexDepthCueEntry *) entry;

	(void) fprintf(stderr, "\tDepthCue.mode: %s(%d)\n",
	(dce->mode) ? "On" : "Off", dce->mode);

	(void) fprintf(stderr, "\tDepthCue.front_plane: %g\n",
	dce->frontPlane);

	(void) fprintf(stderr, "\tDepthCue.back_plane: %g\n",
	dce->backPlane);

	(void) fprintf(stderr, "\tDepthCue.front_scaling: %g\n",
	dce->frontScaling);

	(void) fprintf(stderr, "\tDepthCue.back_scaling: %g\n",
	dce->backScaling);

	(void) fprintf(stderr, "\tDepthCue.color: %s(%d)\n",
	(dce->depthCueColour.colourType) ? "NotIndexed" : "Indexed",
	dce->depthCueColour.colourType);

	(void) fprintf(stderr, "\n");
}

dpy_color_approx_entry(entry)
	char *entry;
{
	pexColourApproxEntry *dce = (pexColourApproxEntry *) entry;

	(void) fprintf(stderr, "\tColorApprox.type: %d\n",
	dce->approxType);

	(void) fprintf(stderr, "\tColorApprox.color_model: %d\n",
	dce->approxModel);

	(void) fprintf(stderr, "\tColorApprox.max1: %u\n",
	dce->max1);

	(void) fprintf(stderr, "\tColorApprox.max2: %u\n",
	dce->max2);

	(void) fprintf(stderr, "\tColorApprox.max3: %u\n",
	dce->max3);

	(void) fprintf(stderr, "\tColorApprox.dither: %u\n",
	dce->dither);

	(void) fprintf(stderr, "\tColorApprox.mult1: %u\n",
	dce->mult1);

	(void) fprintf(stderr, "\tColorApprox.mult2: %u\n",
	dce->mult2);

	(void) fprintf(stderr, "\tColorApprox.mult3: %u\n",
	dce->mult3);

	(void) fprintf(stderr, "\tColorApprox.weight1: %f\n",
	dce->weight1);

	(void) fprintf(stderr, "\tColorApprox.weight2: %f\n",
	dce->weight2);

	(void) fprintf(stderr, "\tColorApprox.weight3: %f\n",
	dce->weight3);

	(void) fprintf(stderr, "\tColorApprox.base_pixel: %u\n",
	dce->basePixel);

	(void) fprintf(stderr, "\n");
}

clean_up(dpy)
	Display *dpy;
{
	XpexFreeLookupTable(dpy, lineBundleLUT);
	XpexFreeLookupTable(dpy, markerBundleLUT);
	XpexFreeLookupTable(dpy, textBundleLUT);
	XpexFreeLookupTable(dpy, interiorBundleLUT);
	XpexFreeLookupTable(dpy, edgeBundleLUT);
	XpexFreeLookupTable(dpy, patternLUT);
	XpexFreeLookupTable(dpy, colorLUT);
	XpexFreeLookupTable(dpy, textFontLUT);
	XpexFreeLookupTable(dpy, lightLUT);
	XpexFreeLookupTable(dpy, depthCueLUT);
	XpexFreeLookupTable(dpy, colorApproxLUT);
}
