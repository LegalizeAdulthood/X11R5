/* $Header: XpexFillArea.c,v 2.7 91/09/11 16:06:38 sinyaw Exp $ */
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

#include "Xpexlibint.h"
#include "Xpexlib.h"
#include <X11/extensions/Xext.h>
#include "extutil.h"
#include "Xpextutil.h"

static int sizeof_vertex();
static int sizeof_facet_opt_data();
static int sizeof_opt_data();
static void Xpex_pack_facet_opt_data();
static void Xpex_pack_array_of_facet_opt_data();
static void Xpex_pack_array_of_vertex();

void Xpex_pack_list_of_vertex();

void
XpexSetInteriorStyle(dpy, style)
	Display *dpy;
	XpexEnumTypeIndex style;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy, info);

	LockDisplay(dpy);
	{
		pexInteriorStyle *oc;
		XpexGetOutputCmd(InteriorStyle,dpy,info,oc);
		oc->interiorStyle = (pexEnumTypeIndex) style;
	}	
	UnlockDisplay(dpy);
}

void
XpexSetInteriorStyleIndex(dpy, index)
	Display *dpy;
	XpexTableIndex index;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexInteriorStyleIndex *oc;
		XpexGetOutputCmd(InteriorStyleIndex,dpy,info,oc);
		oc->index = index;
	}
	UnlockDisplay(dpy);
}

void
XpexSetSurfaceColorIndex(dpy, index)
	Display *dpy;
	XpexTableIndex index;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexSurfaceColourIndex *oc;
		XpexGetOutputCmd(SurfaceColourIndex,dpy,info,oc);
		oc->index = index;
	}
	UnlockDisplay(dpy);
}

void
XpexSetSurfaceColor(dpy, color)
	Display *dpy;
	XpexColor *color;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		int length;
		pexSurfaceColour *oc;
		XpexEnumTypeIndex type = XpexGetColorType(dpy);
		int extra = Xpex_sizeof_color(type);
		XpexGetOutputCmdExtra(SurfaceColour,dpy,info,extra,oc);
		oc->colourSpec.colourType = type;
		Xpex_pack_color(type,color,(char *)(oc+1),&length);
	}
	UnlockDisplay(dpy);
}

void
XpexSetReflectionAttributes(dpy, refl_attr)
	Display *dpy;
	XpexReflectionAttr *refl_attr;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexSurfaceReflAttr *oc;
		XpexColor *color = &(refl_attr->specular_color.value);
		XpexColorType type = refl_attr->specular_color.color_type;
		int length; /* bytes packed */
		int extra = Xpex_sizeof_color(type);
		XpexGetOutputCmdExtra(SurfaceReflAttr,dpy,info,extra,oc);
		oc->reflectionAttr.ambient = refl_attr->ambient;
		oc->reflectionAttr.diffuse = refl_attr->diffuse;
		oc->reflectionAttr.specular = refl_attr->specular;
		oc->reflectionAttr.specularConc = refl_attr->specular_conc;
		oc->reflectionAttr.transmission = refl_attr->transmission;
		oc->reflectionAttr.specularColour.colourType = type;
		Xpex_pack_color(type,color,(char *)(oc + 1),&length);
	}
	UnlockDisplay(dpy);
}

void
XpexSetReflectionModel(dpy, refl_model)
	Display *dpy;
	XpexEnumTypeIndex refl_model;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexSurfaceReflModel *oc;
		XpexGetOutputCmd(SurfaceReflModel,dpy,info,oc);
		oc->reflectionModel = (pexEnumTypeIndex) refl_model;
	}
	UnlockDisplay(dpy);
}

void
XpexSetSurfaceInterpMethod(dpy, method)
	Display *dpy;
	XpexEnumTypeIndex method;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexSurfaceInterp *oc;
		XpexGetOutputCmd(SurfaceInterp,dpy,info,oc);
		oc->surfaceInterp = (pexEnumTypeIndex) method;
	}
	UnlockDisplay(dpy);
}

void
XpexSetBfInteriorStyle(dpy, style)
	Display *dpy;
	XpexEnumTypeIndex style;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexBfInteriorStyle *oc;
		XpexGetOutputCmd(BfInteriorStyle,dpy,info,oc);
		oc->interiorStyle = (pexEnumTypeIndex) style;
	}
	UnlockDisplay(dpy);
}

void
XpexSetBfInteriorStyleIndex(dpy, index)
	Display *dpy;
	XpexTableIndex index;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexBfInteriorStyleIndex *oc;
		XpexGetOutputCmd(BfInteriorStyleIndex,dpy,info,oc);
		oc->index = (pexTableIndex) index;
	}
	UnlockDisplay(dpy);
}

void
XpexSetBfSurfaceColorIndex(dpy, index)
	Display *dpy;
	XpexTableIndex index;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexBfSurfaceColourIndex *oc;
		XpexGetOutputCmd(BfSurfaceColourIndex,dpy,info,oc);
		oc->index = (pexTableIndex) index;
	}
	UnlockDisplay(dpy);
}


void
XpexSetBfSurfaceColor(dpy, color)
	Display *dpy;
	XpexColor *color;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		int bytes_packed;
		pexBfSurfaceColour *oc;
		XpexEnumTypeIndex type = XpexGetColorType(dpy);
		int extra = Xpex_sizeof_color(type);
		XpexGetOutputCmdExtra(BfSurfaceColour,dpy,info,extra,oc);
		oc->colourSpec.colourType = type;
		Xpex_pack_color(type,color,(char *)(oc + 1), &bytes_packed);
	}
	UnlockDisplay(dpy);
}

void
XpexSetBfReflectionAttributes(dpy, refl_attr)
	Display *dpy;
	XpexReflectionAttr *refl_attr;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexBfSurfaceReflAttr *oc;
		XpexColor *color = &(refl_attr->specular_color.value);
		XpexColorType type = refl_attr->specular_color.color_type;
		int length; /* bytes packed */
		int extra = Xpex_sizeof_color(type);
		XpexGetOutputCmdExtra(BfSurfaceReflAttr,dpy,info,extra,oc);
		oc->reflectionAttr.ambient = refl_attr->ambient;
		oc->reflectionAttr.diffuse = refl_attr->diffuse;
		oc->reflectionAttr.specular = refl_attr->specular;
		oc->reflectionAttr.specularConc = refl_attr->specular_conc;
		oc->reflectionAttr.transmission = refl_attr->transmission;
		oc->reflectionAttr.specularColour.colourType = type;
		Xpex_pack_color(type,color,(char *)(oc + 1),&length);
	}
	UnlockDisplay(dpy);
}

void
XpexSetBfReflectionModel(dpy, refl_model)
	Display *dpy;
	XpexEnumTypeIndex refl_model;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexBfSurfaceReflModel *oc;
		XpexGetOutputCmd(BfSurfaceReflModel,dpy,info,oc);
		oc->reflectionModel = refl_model;
	}
	UnlockDisplay(dpy);
}

void
XpexSetBfSurfaceInterpMethod(dpy, method)
	Display *dpy;
	XpexEnumTypeIndex method;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexBfSurfaceInterp *oc;
		XpexGetOutputCmd(BfSurfaceInterp,dpy,info,oc);
		oc->surfaceInterp = (pexEnumTypeIndex) method;
	}
	UnlockDisplay(dpy);
}

void
XpexSetSurfaceApproximation(dpy, approxMethod, uTolerance, vTolerance)
	Display *dpy;
	XpexEnumTypeIndex approxMethod;
	XpexFloat uTolerance;
	XpexFloat vTolerance;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexSurfaceApproximation *oc;
		XpexGetOutputCmd(SurfaceApproximation,dpy,info,oc);
		oc->approx.approxMethod = (pexEnumTypeIndex) approxMethod;
		oc->approx.uTolerance = uTolerance;
		oc->approx.vTolerance = vTolerance;
	}
	UnlockDisplay(dpy);
}

void
XpexSetCullingMode(dpy, culling_mode)
	Display *dpy;
	XpexCullMode culling_mode;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexCullingMode *oc;
		XpexGetOutputCmd(CullingMode,dpy,info,oc);
		oc->cullMode = culling_mode;
	}
	UnlockDisplay(dpy);
}

void
XpexSetDistinguishFlag(dpy, distinguish_flag)
	Display *dpy;
	XpexSwitch distinguish_flag;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexDistinguishFlag *oc;
		XpexGetOutputCmd(DistinguishFlag,dpy,info,oc);
		oc->distinguish = (pexSwitch) distinguish_flag;
	}
	UnlockDisplay(dpy);
}

void
XpexSetPatternSize(dpy, width, height)
	Display *dpy;
	XpexFloat width;
	XpexFloat height;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexPatternSize *oc;
		XpexGetOutputCmd(PatternSize,dpy,info,oc);
		oc->size.x = width;
		oc->size.y = height;
	}
	UnlockDisplay(dpy);
}

void
XpexSetPatternRefPt(dpy, x, y)
	Display *dpy;
	XpexFloat x; 
	XpexFloat y;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexPatternRefPt *oc;
		XpexGetOutputCmd(PatternRefPt,dpy,info,oc);
		oc->point.x = x;
		oc->point.y = y;
	}
	UnlockDisplay(dpy);
}

void
XpexSetPatternAttr(dpy, ref_pt, vector1, vector2)
	Display *dpy;
	XpexCoord3D *ref_pt;
	XpexVector3D *vector1;
	XpexVector3D *vector2;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexPatternAttr *oc;
		XpexGetOutputCmd(PatternAttr,dpy,info,oc);
		oc->refPt.x   = ref_pt->x;
		oc->refPt.y   = ref_pt->y;
		oc->refPt.z   = ref_pt->z;
		oc->vector1.x = vector1->x;
		oc->vector1.y = vector1->y;
		oc->vector1.z = vector1->z;
		oc->vector2.x = vector2->x;
		oc->vector2.y = vector2->y;
		oc->vector2.z = vector2->z;
	}
	UnlockDisplay(dpy);
}

void
XpexSetInteriorBundleIndex(dpy, index)
	Display *dpy;
	XpexTableIndex index;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexInteriorBundleIndex *oc;
		XpexGetOutputCmd(InteriorBundleIndex,dpy,info,oc);
		oc->index = index;
	}
	UnlockDisplay(dpy);
}

void
XpexSetSurfaceEdgeFlag(dpy, on_off)
	Display *dpy;
	XpexSwitch on_off;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexSurfaceEdgeFlag *oc;
		XpexGetOutputCmd(SurfaceEdgeFlag,dpy,info,oc);
		oc->onoff = on_off;
	}
	UnlockDisplay(dpy);
}

void
XpexSetSurfaceEdgeType(dpy, edgeType)
	Display *dpy;
	XpexEnumTypeIndex edgeType;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexSurfaceEdgeType *oc;
		XpexGetOutputCmd(SurfaceEdgeType,dpy,info,oc);
		oc->edgeType = edgeType;
	}
	UnlockDisplay(dpy);
}

void
XpexSetSurfaceEdgeWidth(dpy, width)
	Display *dpy;
	XpexFloat width;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexSurfaceEdgeWidth *oc;
		XpexGetOutputCmd(SurfaceEdgeWidth,dpy,info,oc);
		oc->width = width;
	}
	UnlockDisplay(dpy);
}

void
XpexSetSurfaceEdgeColorIndex(dpy, index)
	Display *dpy;
	XpexTableIndex index;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexSurfaceEdgeColourIndex *oc;
		XpexGetOutputCmd(SurfaceEdgeColourIndex,dpy,info,oc);
		oc->index = index;
	}
	UnlockDisplay(dpy);
}

void
XpexSetSurfaceEdgeColor(dpy, color)
	Display *dpy;
	XpexColor *color;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		int length;
		pexSurfaceEdgeColour *oc;
		XpexEnumTypeIndex type = XpexGetColorType(dpy);
		int extra = Xpex_sizeof_color(type);
		XpexGetOutputCmdExtra(SurfaceEdgeColour,dpy,info,extra,oc);
		oc->colourSpec.colourType = type;
		Xpex_pack_color(type,color,(char *)(oc+1),&length);
	}
	UnlockDisplay(dpy);
}

void
XpexSetEdgeBundleIndex(dpy, index)
	Display *dpy;
	XpexTableIndex index;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexEdgeBundleIndex *oc;
		XpexGetOutputCmd(EdgeBundleIndex,dpy,info,oc);
		oc->index = index;
	}
	UnlockDisplay(dpy);
}

void
XpexFillArea(dpy, shape, ignore_edges, points, num_points)
	Display *dpy;
	XpexShape shape;
	XpexSwitch ignore_edges;
	XpexCoord3D *points;
	int num_points;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy, info);

	LockDisplay(dpy);
	{
		pexFillArea *oc;
		int extra = num_points * sizeof(pexCoord3D);

		XpexGetOutputCmdExtra(FillArea,dpy,info,extra,oc);
		oc->shape = shape;
		oc->ignoreEdges = ignore_edges;
		bcopy((char *)points,(char *)(oc + 1),extra);
	}
	UnlockDisplay(dpy);
}

void
XpexFillArea2D(dpy, shape, ignore_edges, points, num_points)
	Display *dpy;
	XpexShape shape;
	XpexSwitch ignore_edges;
	XpexCoord2D *points;
	int num_points;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy, info);

	LockDisplay(dpy);
	{
		pexFillArea2D *oc;
		int extra = num_points * sizeof(pexCoord3D);

		XpexGetOutputCmdExtra(FillArea2D,dpy,info,extra,oc);
		oc->shape = shape;
		oc->ignoreEdges = ignore_edges;
		bcopy((char *)points,(char *)(oc + 1),extra);
	}
	UnlockDisplay(dpy);
}

static int
sizeof_opt_data(attrMask,colorType)
	XpexBitmaskShort attrMask;
	XpexColorType colorType;
{
	int fp = sizeof(XpexFloat);
	int sc = Xpex_sizeof_color(colorType);
	XpexBitmaskShort vc = attrMask & XpexGA_Color;
	XpexBitmaskShort vn = attrMask & XpexGA_Normal;
	XpexBitmaskShort ve = attrMask & XpexGA_Edges;
	int vi = ((colorType == Xpex_IndexedColor) ? 1 : 0);
	int dv = (vc ? (vi ? 4 : sc) : 0) + 
		(vn ? 3 * fp : 0) + (ve ? 4 : 0);

	return dv;
}

static int
sizeof_facet_opt_data(attrMask,colorType)
	XpexBitmaskShort attrMask;
	XpexColorType colorType;
{
	attrMask = attrMask & XpexGA_Color & XpexGA_Normal;

	return sizeof_opt_data(attrMask,colorType);
}

static int
sizeof_vertex(attrMask,colorType)
	XpexBitmaskShort attrMask;
	XpexColorType colorType;
{
	int d = sizeof_opt_data(attrMask,colorType);

	return sizeof(pexCoord3D) + d;
}

static int
sizeof_facet(facetAttr, vertAttr, colorType, numVert)
	XpexBitmaskShort facetAttr;
	XpexBitmaskShort vertAttr;
	XpexColorType colorType;
	int numVert;
{
	int df = sizeof_facet_opt_data(facetAttr, colorType);
	int dv = sizeof_vertex(vertAttr,colorType);
	int f = df + (numVert * dv);

	return f;
}

void
XpexExtFillArea(dpy, shape, ignoreEdges, facetAttr, vertAttr,
	facetData, vertices, numVertices)
	Display *dpy;
	XpexShape shape;
	XpexSwitch ignoreEdges;
	XpexBitmaskShort facetAttr;
	XpexBitmaskShort vertAttr;
	XpexFacetOptData *facetData;
	XpexArrayOfVertex *vertices;
	int numVertices;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy, info);

	LockDisplay(dpy);
	{
		pexExtFillArea *oc;
		XpexColorType colorType = XpexGetColorType(dpy);

		int extra = sizeof_facet(facetAttr, vertAttr, 
		colorType, numVertices);

		XpexGetOutputCmdExtra(ExtFillArea,dpy,info,extra,oc);
		oc->shape = shape;
		oc->ignoreEdges = ignoreEdges;
		oc->colourType = colorType;
		oc->facetAttribs = facetAttr;
		oc->vertexAttribs = vertAttr;
		{
			int bytes_packed;
			char *stream = (char *)(oc + 1);
			CARD32 *c32;

			Xpex_pack_facet_opt_data(facetAttr, colorType, 
			stream, &bytes_packed);

			stream += bytes_packed;

			c32 = (CARD32 *) stream;

			*c32 = (CARD32) numVertices;

			stream += sizeof(CARD32);

			Xpex_pack_array_of_vertex(vertAttr, colorType, 
			vertices, numVertices, stream, &bytes_packed);
		}
	}
	UnlockDisplay(dpy);
}

static int
sizeof_fill_area_sets(numLists, lists)
	int numLists;
	XpexListOfCoord3D *lists;
{
	int size = 0;

	while (numLists--) {
		size += (sizeof(CARD32) + 
		lists->num_points * sizeof(pexCoord3D));
		lists++;
	}
	return size;
}

static void
pack_fill_area_sets(numLists, lists, pStream)
	int numLists;
	XpexListOfCoord3D *lists;
	CARD32 *pStream;
{
	register long i, j;
	int num_points;
	pexCoord3D *points;

	for (i = 0; i < numLists; i++) {
		*pStream++ = num_points = lists[i].num_points;
		for (j = 0; j < num_points; j++) {
			*pStream++ = (CARD32) lists[i].points[j].x;
			*pStream++ = (CARD32) lists[i].points[j].y;
			*pStream++ = (CARD32) lists[i].points[j].z;
			/* points = (pexCoord3D *) pStream;
			points->x = lists[i].points[j].x;
			points->y = lists[i].points[j].y;
			points->z = lists[i].points[j].z;
			pStream += sizeof(pexCoord3D); */
		}
	}
}

void
XpexFillAreaSet(dpy, shape, ignoreEdges, contourHint, 
	vertexLists, numLists)
	Display *dpy;
	XpexShape shape;
	XpexSwitch ignoreEdges;
	XpexContour contourHint;
	XpexListOfCoord3D *vertexLists;
	int numLists;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy, info);

	LockDisplay(dpy);
	{
		pexFillAreaSet *oc;
		int extra = sizeof_fill_area_sets(numLists, vertexLists);
		XpexGetOutputCmdExtra(FillAreaSet,dpy,info,extra,oc);
		oc->shape = shape;
		oc->ignoreEdges = ignoreEdges;
		oc->contourHint = contourHint;
		oc->numLists = (CARD32) numLists;
		pack_fill_area_sets(numLists, vertexLists, (CARD32 *)(oc + 1));
	}
	UnlockDisplay(dpy);
}

static int
sizeof_fill_area_sets_2d(numLists, lists)
	int numLists;
	XpexListOfCoord2D *lists;
{
	int size = 0;

	while (numLists--) {
		size += (sizeof(CARD32) + 
		lists->num_points * sizeof(pexCoord2D));
		lists++;
	}
	return size;
}

static void
pack_fill_area_sets_2d(numLists, lists, pStream)
	int numLists;
	XpexListOfCoord2D *lists;
	char *pStream;
{
	register long i, j;

	for (i = 0; i < numLists; i++) {
		CARD32 *count = (CARD32 *) pStream;
		int num_points = lists[i].num_points;
		*count = num_points;
		pStream += sizeof(CARD32);
		for (j = 0; j < num_points; j++) {
			pexCoord2D *points = (pexCoord2D *) pStream;
			points->x = lists[i].points[j].x;
			points->y = lists[i].points[j].x;
			pStream += sizeof(pexCoord2D);
		}
	}
}

void
XpexFillAreaSet2D(dpy, shape, ignoreEdges, contourHint, 
	vertexLists, numLists)
	Display *dpy;
	XpexShape shape;
	XpexSwitch ignoreEdges;
	XpexContour contourHint;
	XpexListOfCoord2D *vertexLists;
	int numLists;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexFillAreaSet2D *oc;
		int extra = sizeof_fill_area_sets_2d(numLists, vertexLists);
		XpexGetOutputCmdExtra(FillAreaSet2D,dpy,info,extra,oc);
		oc->shape = shape;
		oc->ignoreEdges = ignoreEdges;
		oc->contourHint = contourHint;
		oc->numLists = (CARD32) numLists;
		pack_fill_area_sets_2d(numLists, vertexLists, (char *)(oc + 1));
	}
	UnlockDisplay(dpy);
}

static int
sizeof_ext_fas_data(facetAttr, vertAttr, colorType, lists, numLists)
	XpexBitmaskShort facetAttr;
	XpexBitmaskShort vertAttr;
	XpexColorType colorType;
	XpexListOfVertex *lists;
	int numLists;
{
	int m = 0; /* let m = total number of vertices */
	int v; /* let v = size of vertex */
	int length = sizeof_facet_opt_data(facetAttr,colorType);
	register int i;

	for (i = 0; i < numLists; i++) {
		m += lists[i].numVertices;
	}
	v = sizeof_vertex(vertAttr, colorType);
	length += (sizeof(CARD32) * numLists) + (m * v);

	return length;
}

void
XpexExtFillAreaSet(dpy, shape, ignoreEdges, contourHint, 
	facetAttr, vertAttr, facetData, vertexLists, numLists)
	Display *dpy;
	XpexShape shape;
	XpexSwitch ignoreEdges;
	XpexContour contourHint;
	XpexBitmaskShort facetAttr;
	XpexBitmaskShort vertAttr;
	XpexFacetOptData *facetData;
	XpexListOfVertex *vertexLists;
	int numLists;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexExtFillAreaSet *oc;
		XpexEnumTypeIndex colorType = XpexGetColorType(dpy);
		int extra = sizeof_ext_fas_data(facetAttr, vertAttr,
		colorType, facetData, vertexLists, numLists);

		XpexGetOutputCmdExtra(ExtFillAreaSet,dpy,info,extra,oc);
		oc->shape = shape; 
		oc->ignoreEdges = ignoreEdges;
		oc->contourHint = contourHint;
		oc->colourType = colorType;
		oc->facetAttribs = facetAttr;
		oc->vertexAttribs = vertAttr;
		oc->numLists = (CARD32) numLists;
		{
			int bytes_packed;
			char *stream = (char *)(oc + 1);

			Xpex_pack_facet_opt_data(facetAttr, colorType, facetData, 
			stream, &bytes_packed);

			stream += bytes_packed;

			Xpex_pack_list_of_vertex(vertAttr, colorType, 
			vertexLists, numLists, stream, &bytes_packed);
		}
	}

	UnlockDisplay(dpy);
}

#ifdef IGNORE
static int
_XpexSizeOfOptData(mask, colorType)
	XpexBitmaskShort mask;
	XpexEnumTypeIndex colorType;
{
	int fp = sizeof(XpexFloat);
	int sc = (colorType == Xpex_Rgb8Color) ? 4 :
		((colorType == (Xpex_Rgb16Color) ? 8 : 3 * fp));
	XpexBitmaskShort vc = mask & XpexGA_Color; 
	XpexBitmaskShort vn = mask & XpexGA_Normal; 
	XpexBitmaskShort ve = mask & XpexGA_Edges; 

	int vi = ((colorType == Xpex_IndexedColor) ? 1 : 0);

	int dv = (vc ? (vi ? 4 : sc) : 0) + 
	(vn ? 3 * fp : 0) + (ve ? 4 : 0);

	return dv;
}

static int 
_XpexSizeOfFacetOptData(facetAttr, colorType)
	XpexBitmaskShort facetAttr;
	XpexEnumTypeIndex colorType;
{
	facetAttr = facetAttr & XpexGA_Color & XpexGA_Normal;

	return _XpexSizeOfOptData(facetAttr, colorType);
}

static int 
_XpexSizeOfVertex(mask, colorType)
	XpexBitmaskShort mask;
	XpexEnumTypeIndex colorType;
{
	int d = _XpexSizeOfOptData(mask, colorType);

	return (sizeof(pexCoord3D) + d);
}
#endif /* IGNORE */

static int
sizeof_triangle_strip_data(facetAttr, vertAttr, colorType, numVertices)
	XpexBitmaskShort facetAttr;
	XpexBitmaskShort vertAttr;
	XpexEnumTypeIndex colorType;
	int numVertices;
{
	int length	= (numVertices - 2) * 
	sizeof_facet_opt_data(facetAttr, colorType);

	length += numVertices * sizeof_vertex(vertAttr, colorType);

	return length;
}

void
XpexTriangleStrip(dpy, facetAttr, vertAttr, 
	facetData, vertices, numVertices)
	Display *dpy;
	XpexBitmaskShort facetAttr;
	XpexBitmaskShort vertAttr;
	XpexArrayOfFacetOptData	*facetData;
	XpexArrayOfVertex *vertices;
	int numVertices;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexTriangleStrip *oc;
		XpexEnumTypeIndex colorType = XpexGetColorType(dpy);
		int extra = sizeof_triangle_strip_data(facetAttr, 
		vertAttr, colorType, numVertices);

		XpexGetOutputCmdExtra(TriangleStrip,dpy,info,extra,oc);
		oc->colourType = (pexColourType) colorType;
		oc->facetAttribs = (pexBitmaskShort) facetAttr;
		oc->vertexAttribs = (pexBitmaskShort) vertAttr;
		oc->numVertices = (CARD32) numVertices;
		{
			char *stream = (char *)(oc + 1);
			int bytes_packed;

			Xpex_pack_array_of_facet_opt_data(facetAttr, colorType, 
			numVertices - 2, facetData, stream, &bytes_packed);

			stream += bytes_packed;

			Xpex_pack_array_of_vertex(vertAttr, colorType, 
			vertices, numVertices, stream, &bytes_packed);
		
		}
	}
	UnlockDisplay(dpy);
}

static int
sizeof_quad_mesh_data(facetAttr, vertAttr, 
	colorType, facetData, vertices, numPointsM, numPointsN)
	XpexBitmaskShort facetAttr;
	XpexBitmaskShort vertAttr;
	XpexEnumTypeIndex colorType;
	XpexArrayOfFacetOptData *facetData;
	XpexArrayOfVertex *vertices;
	int numPointsM;
	int numPointsN;
{
	int a = (numPointsM - 1) * (numPointsN - 1) *
	sizeof_facet_opt_data(facetAttr, colorType);

	int b = numPointsM * numPointsN *
	sizeof_vertex(facetAttr, colorType);

	return a + b;
}

void
XpexQuadMesh(dpy, shape, facetAttr, vertAttr, 
	facetData, vertices, numPointsM, numPointsN)
	Display *dpy;
	int shape;
	XpexBitmaskShort facetAttr;
	XpexBitmaskShort vertAttr;
	XpexArrayOfFacetOptData *facetData;
	XpexArrayOfVertex *vertices;
	int numPointsM;
	int numPointsN;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexQuadrilateralMesh *oc;
		XpexEnumTypeIndex colorType = XpexGetColorType(dpy);
		int extra  = sizeof_quad_mesh_data(facetAttr, vertAttr,
		colorType, facetData, vertices, numPointsM, numPointsN);

		XpexGetOutputCmdExtra(QuadrilateralMesh, dpy, info,extra, oc);
		oc->colourType = (pexColourType) colorType;
		oc->mPts = (CARD16) numPointsM;
		oc->nPts = (CARD16) numPointsN;
		oc->facetAttribs = facetAttr;
		oc->vertexAttribs = vertAttr;
		oc->shape = shape;
		{
			char *stream = (char *)(oc + 1);
			int bytes_packed;
			int numVertices = numPointsM * numPointsN;
			int num_facetData = (numPointsM - 1) * (numPointsN - 1);
			Xpex_pack_array_of_facet_opt_data(facetAttr, colorType,
			num_facetData, facetData, stream, &bytes_packed);

			stream += bytes_packed;
			Xpex_pack_array_of_vertex(vertAttr, colorType,
			vertices, numVertices, stream, &bytes_packed);
		}
	}

	UnlockDisplay(dpy);
}

void
XpexSetOfFillAreaSets(dpy, shape, 
	fas_attrs, vert_attrs, edge_attrs, 
	contour_hint, contours_all_1, 
	num_fas, num_verts, num_edges, num_contours, 
	per_fas_data, per_vert_data, per_edge_data, contours, length)
	Display *dpy;
	XpexShape shape;
	XpexBitmaskShort fas_attrs;
	XpexBitmaskShort vert_attrs;
	XpexBitmaskShort edge_attrs;
	XpexContour contour_hint;
	XpexSwitch contours_all_1;
	int num_fas;
	int num_verts;
	int num_edges;
	int num_contours;
	XpexArrayOfOptData *per_fas_data;
	XpexArrayOfVertex *per_vert_data;
	XpexSwitch *per_edge_data;
	char *contours; /* ARRAYOfLISTOfLISTOfCARD16 */
	int length;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		char *stream;
		pexSOFAS *oc;
		XpexColorType color_type = XpexGetColorType(dpy); 

		int bytesPacked;

		int extra = sizeof_var_sofas_data(
			fas_attrs, vert_attrs, edge_attrs,
			num_fas, num_verts, num_edges, num_contours,
			length);

		XpexGetOutputCmdExtra(SOFAS,dpy,info,extra,oc);
		oc->shape = shape;
		oc->colourType = color_type;
		oc->FAS_Attributes = fas_attrs;
		oc->vertexAttributes = vert_attrs;
		oc->edgeAttributes = edge_attrs;
		oc->contourHint = contour_hint;
		oc->contourCountsFlag = contours_all_1;
		oc->numFAS = num_fas;
		oc->numVertices = num_verts;
		oc->numEdges = num_edges;
		oc->numContours = num_contours;
		stream = (char *)(oc + 1);
		Xpex_pack_array_of_facet_opt_data(fas_attrs, color_type, 
		num_fas, per_fas_data, stream, &bytesPacked);
		stream += bytesPacked;
		Xpex_pack_array_of_vertex(vert_attrs, color_type,
		num_verts, per_vert_data, stream, &bytesPacked);
		stream += bytesPacked;
		bytesPacked = num_edges * sizeof(XpexSwitch);
		bcopy((char *)per_edge_data, stream, bytesPacked);
		stream += bytesPacked;
		bcopy((char *)contours, stream, length);
		stream += length + PADDING(length);
	}
	UnlockDisplay(dpy);
}

static int
sizeof_var_sofas_data(color_type,
	fas_attrs, vert_attrs, edge_attrs,
	num_fas, num_verts, num_edges, num_contours,
	per_fas_data, per_vert_data, per_edge_data, contours)
	XpexColorType color_type;
	XpexBitmaskShort fas_attrs;
	XpexBitmaskShort vert_attrs;
	XpexSwitch edge_attrs;
	int num_fas;
	int num_verts;
	int num_edges;
	int num_contours;
	CARD16 *contours;
{
	int size = 0;

	if (fas_attrs) {
		size += num_fas * sizeof_facet_opt_data(fas_attrs, color_type);
	}

	if (vert_attrs) {
		size += num_verts * sizeof_vertex(vert_attrs, color_type);
	}

	if (edge_attrs) {
		int temp = num_edges * sizeof(XpexSwitch);
		size += temp + PADDING(temp);
	}
#ifdef NOT_READY
	if (num_contours) {
		size +=
	}
#endif /* NOT_READY */

	return size;
}

static int 
sizeof_nurb_surf(type, num_u_knots, num_v_knots, 
	num_points, lists, num_lists)
	XpexCoordType type;
	int num_u_knots;
	int num_v_knots;
	int num_points;
	XpexListOfTrimCurve *lists;
	int num_lists;
{
	extern int Xpex_sizeof_array_of_list_of_trim_curve();

	int length = 
		((num_u_knots + num_v_knots) * sizeof(XpexFloat)) +
		(num_points * Xpex_sizeof_coord(type)) +
		Xpex_sizeof_array_of_list_of_trim_curve(lists,num_lists);

	return length;
}

void
XpexNurbSurface(dpy, type, uOrder, vOrder,
	uKnots, numUknots, vKnots, numVknots, 
	mPts, nPts, points, trimCurveLists, numLists)
	Display *dpy;
	XpexCoordType type;
	int uOrder;
	int vOrder;
	XpexFloat *uKnots;
	int numUknots;
	XpexFloat *vKnots;
	int numVknots;
	int mPts;
	int nPts;
	XpexArrayOfCoord *points;
	XpexListOfTrimCurve *trimCurveLists;
	int numLists;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		char *stream;
		pexNurbSurface *oc;
		int bytes_packed;
		int numPoints = mPts * nPts;
		int extra = sizeof_nurb_surf(type, numUknots, numVknots, 
		numPoints, numLists);

		XpexGetOutputCmdExtra(NurbSurface, dpy, info,extra, oc);
		oc->type = type;
		oc->uOrder = (CARD16) uOrder;
		oc->vOrder = (CARD16) vOrder;
		oc->numUknots = (CARD32) numUknots;
		oc->numVknots = (CARD32) numVknots;
		oc->mPts = (CARD16) mPts;
		oc->nPts = (CARD16) nPts;
		oc->numLists = (CARD32) numLists;

		stream = (char *)(oc + 1);
		bcopy((char *)uKnots,(char *)stream,numUknots*sizeof(XpexFloat));
		stream += numUknots * sizeof(XpexFloat);

		bcopy((char *)vKnots,(char *)stream,numVknots*sizeof(XpexFloat));
		stream += numVknots * sizeof(XpexFloat);

		if (type == Xpex_Rational) {
			bcopy((char *)points->points_4d, stream,
			numPoints * sizeof(pexCoord4D));
			stream += numPoints * sizeof(pexCoord4D);
		} else {
			bcopy((char *)points->points_3d, stream,
			numPoints * sizeof(pexCoord3D));
			stream += numPoints * sizeof(pexCoord3D);
		}

		if (numLists) {
			Xpex_pack_array_of_list_of_trim_curve(trimCurveLists,
			numLists, stream, &bytes_packed);
		}
	}
	UnlockDisplay(dpy);
}

static void
Xpex_pack_facet_opt_data(mask, colorType, facetData,
	pStream, length)
	XpexBitmaskShort mask;
	XpexEnumTypeIndex colorType;
	XpexFacetOptData *facetData;
	char *pStream; /* RETURN */
	int *length; /* RETURN */
{
	int fc = mask & XpexGA_Color; 
	int fn = mask & XpexGA_Normal; 

	if (fc && !fn) { 
		if (colorType == Xpex_IndexedColor) {
			char *src = (char *)&(facetData->indexed);
			*length = sizeof(pexOptIndexed);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_RgbFloatColor) {
			char *src = (char *)&(facetData->rgb);
			*length = sizeof(pexOptRgb);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_CieFloatColor) {
			char *src = (char *)&(facetData->cie);
			*length = sizeof(pexOptCie);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_HsvFloatColor) {
			char *src = (char *)&(facetData->hsv);
			*length = sizeof(pexOptHsv);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_HlsFloatColor) {
			char *src = (char *)&(facetData->hls);
			*length = sizeof(pexOptHls);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_Rgb8Color) {
			char *src = (char *)&(facetData->rgb8);
			*length = sizeof(pexOptRgb8);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_Rgb16Color) {
			char *src = (char *)&(facetData->rgb16);
			*length = sizeof(pexOptRgb16);
			bcopy(src, pStream, *length);
		}
	} else if (!fc && fn) {
		char *src = (char *)&(facetData->normal);
		*length = sizeof(pexOptNormal);
		bcopy(src, pStream, *length);
	} else if (fc && fn) {
		if (colorType == Xpex_IndexedColor) {
			char *src = (char *)&(facetData->indexed_normal);
			*length = sizeof(pexOptIndexedNormal);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_RgbFloatColor) {
			char *src = (char *)&(facetData->rgb_normal);
			*length = sizeof(pexOptRgbNormal);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_CieFloatColor) {
			char *src = (char *)&(facetData->cie_normal);
			*length = sizeof(pexOptCieNormal);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_HsvFloatColor) {
			char *src = (char *)&(facetData->hsv_normal);
			*length = sizeof(pexOptHsvNormal);
			bcopy(src, pStream, *length);
		} else if (fc && fn && (colorType == Xpex_HlsFloatColor)) {
			char *src = (char *)&(facetData->hls_normal);
			*length = sizeof(pexOptHlsNormal);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_Rgb8Color) {
			char *src = (char *)&(facetData->rgb8_normal);
			*length = sizeof(pexOptRgb8Normal);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_Rgb16Color) {
			char *src = (char *)&(facetData->rgb16_normal);
			*length = sizeof(pexOptRgb16Normal);
			bcopy(src, pStream, *length);
		}
	} else {
		*length = 0;
	}
}

static void
Xpex_pack_array_of_facet_opt_data(mask, colorType, count, facetData, 
	pStream, length)
	XpexBitmaskShort mask;
	XpexEnumTypeIndex colorType;
	int count;
	XpexArrayOfFacetOptData *facetData;
	char *pStream; /* RETURN */
	int *length; /* RETURN */
{
	int fc = mask & XpexGA_Color; 
	int fn = mask & XpexGA_Normal; 

	if (fc && !fn) { 
		if (colorType == Xpex_IndexedColor) {
			char *src = (char *)(facetData->indexed);
			*length = count * sizeof(pexOptIndexed);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_RgbFloatColor) {
			char *src = (char *)(facetData->rgb);
			*length = count * sizeof(pexOptRgb);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_CieFloatColor) {
			char *src = (char *)(facetData->cie);
			*length = count * sizeof(pexOptCie);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_HsvFloatColor) {
			char *src = (char *)(facetData->hsv);
			*length = count * sizeof(pexOptHsv);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_HlsFloatColor) {
			char *src = (char *)(facetData->hls);
			*length = count * sizeof(pexOptHls);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_Rgb8Color) {
			char *src = (char *)(facetData->rgb8);
			*length = count * sizeof(pexOptRgb8);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_Rgb16Color) {
			char *src = (char *)(facetData->rgb16);
			*length = count * sizeof(pexOptRgb16);
			bcopy(src, pStream, *length);
		}
	} else if (!fc && fn) {
		char *src = (char *)(facetData->normal);
		*length = count * sizeof(pexOptNormal);
		bcopy(src, pStream, *length);
	} else if (fc && fn) {
		if (colorType == Xpex_IndexedColor) {
			char *src = (char *)(facetData->indexed_normal);
			*length = count * sizeof(pexOptIndexedNormal);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_RgbFloatColor) {
			char *src = (char *)(facetData->rgb_normal);
			*length = count * sizeof(pexOptRgbNormal);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_CieFloatColor) {
			char *src = (char *)(facetData->cie_normal);
			*length = count * sizeof(pexOptCieNormal);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_HsvFloatColor) {
			char *src = (char *)(facetData->hsv_normal);
			*length = count * sizeof(pexOptHsvNormal);
			bcopy(src, pStream, *length);
		} else if (fc && fn && (colorType == Xpex_HlsFloatColor)) {
			char *src = (char *)(facetData->hls_normal);
			*length = count * sizeof(pexOptHlsNormal);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_Rgb8Color) {
			char *src = (char *)(facetData->rgb8_normal);
			*length = count * sizeof(pexOptRgb8Normal);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_Rgb16Color) {
			char *src = (char *)(facetData->rgb16_normal);
			*length = count * sizeof(pexOptRgb16Normal);
			bcopy(src, pStream, *length);
		}
	} else {
		*length = 0;
	}
}

static void
Xpex_pack_array_of_vertex(mask, colorType, 
	numVertices, vertices, pStream, length)
	XpexBitmaskShort mask;
	XpexEnumTypeIndex colorType;
	XpexArrayOfVertex *vertices;
	int numVertices;
	char *pStream; /* RETURN */
	int *length; /* RETURN */
{
	register int i;

	int vc = mask & XpexGA_Color;
	int vn = mask & XpexGA_Normal;
	int ve = mask & XpexGA_Edges;

	if (!vc && !vn && !ve) {
		char *src = (char *) (vertices->pts); 
		*length = numVertices * sizeof(pexVertexNoOptData);
		bcopy(src, pStream, *length);
	} else if (vc && !vn && !ve) {
		if (colorType == Xpex_IndexedColor) {
			char *src = (char *) (vertices->pts_indexed); 
			*length = numVertices * sizeof(pexVertexIndexed);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_RgbFloatColor) {
			char *src = (char *) (vertices->pts_rgb); 
			*length = numVertices * sizeof(pexVertexRgb);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_CieFloatColor) {
			char *src = (char *) (vertices->pts_cie); 
			*length = numVertices * sizeof(pexVertexCie);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_HsvFloatColor) {
			char *src = (char *) (vertices->pts_hsv); 
			*length = numVertices * sizeof(pexVertexHsv);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_HlsFloatColor) {
			char *src = (char *) (vertices->pts_hls); 
			*length = numVertices * sizeof(pexVertexHls);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_Rgb8Color) {
			char *src = (char *) (vertices->pts_rgb8); 
			*length = numVertices * sizeof(pexVertexRgb8);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_Rgb16Color) {
			char *src = (char *) (vertices->pts_rgb16); 
			*length = numVertices * sizeof(pexVertexRgb16);
			bcopy(src, pStream, *length);
		}
	} else if (!vc  && vn && !ve) {
		char *src = (char *) (vertices->pts_normal); 
		*length = numVertices * sizeof(pexVertexNormal);
		bcopy(src, pStream, *length);
	} else if (!vc && !vn && ve) {
		char *src = (char *) (vertices->pts_edges); 
		*length = numVertices * sizeof(pexVertexEdges);
		bcopy(src, pStream, *length);
	} else if (vc && vn && !ve) {
		if (colorType == Xpex_IndexedColor) {
			char *src = (char *) (vertices->pts_indexed_normal); 
			*length = numVertices * sizeof(pexVertexIndexedNormal);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_RgbFloatColor) {
			char *src = (char *) (vertices->pts_rgb_normal); 
			*length = numVertices * sizeof(pexVertexRgbNormal);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_CieFloatColor) {
			char *src = (char *) (vertices->pts_cie_normal); 
			*length = numVertices * sizeof(pexVertexCieNormal);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_HsvFloatColor) {
			char *src = (char *) (vertices->pts_hsv_normal); 
			*length = numVertices * sizeof(pexVertexHsvNormal);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_HlsFloatColor) {
			char *src = (char *) (vertices->pts_hls_normal); 
			*length = numVertices * sizeof(pexVertexHlsNormal);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_Rgb8Color) {
			char *src = (char *) (vertices->pts_rgb8_normal); 
			*length = numVertices * sizeof(pexVertexRgb8Normal);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_Rgb16Color) {
			char *src = (char *) (vertices->pts_rgb16_normal); 
			*length = numVertices * sizeof(pexVertexRgb16Normal);
			bcopy(src, pStream, *length);
		}
	} else if (vc && !vn && ve ) {
		if (colorType == Xpex_IndexedColor) {
			char *src = (char *) (vertices->pts_indexed_edges); 
			*length = numVertices * sizeof(pexVertexIndexedEdges);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_RgbFloatColor) {
			char *src = (char *) (vertices->pts_rgb_edges);
			*length = numVertices * sizeof(pexVertexRgbEdges);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_CieFloatColor) {
			char *src = (char *) (vertices->pts_rgb_edges);
			*length = numVertices * sizeof(pexVertexCieEdges);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_HsvFloatColor) {
			char *src = (char *) (vertices->pts_hsv_edges);
			*length = numVertices * sizeof(pexVertexHsvEdges);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_HlsFloatColor) {
			char *src = (char *) (vertices->pts_hls_edges);
			*length = numVertices * sizeof(pexVertexHlsEdges);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_Rgb8Color) {
			char *src = (char *) (vertices->pts_rgb8_edges);
			*length = numVertices * sizeof(pexVertexRgb8Edges);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_Rgb16Color) {
			char *src = (char *) (vertices->pts_rgb16_edges);
			*length = numVertices * sizeof(pexVertexRgb16Edges);
			bcopy(src, pStream, *length);
		}
	} else if (!vc && !vn && ve) {
		char *src = (char *) (vertices->pts_normal_edges);
		*length = numVertices * sizeof(pexVertexNormalEdges);
		bcopy(src, pStream, *length);
	} else if (vc && vn && ve) {
		if (colorType == Xpex_IndexedColor) {
			char *src = (char *) (vertices->pts_indexed_normal_edges);
			*length = numVertices * sizeof(pexVertexIndexedNormalEdges);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_RgbFloatColor) {
			char *src = (char *) (vertices->pts_rgb_normal_edges);
			*length = numVertices * sizeof(pexVertexRgbNormalEdges);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_CieFloatColor) {
			char *src = (char *) (vertices->pts_cie_normal_edges);
			*length = numVertices * sizeof(pexVertexCieNormalEdges);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_HsvFloatColor) {
			char *src = (char *) (vertices->pts_hsv_normal_edges);
			*length = numVertices * sizeof(pexVertexHsvNormalEdges);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_HlsFloatColor) {
			char *src = (char *) (vertices->pts_hls_normal_edges);
			*length = numVertices * sizeof(pexVertexHlsNormalEdges);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_Rgb8Color) {
			char *src = (char *) (vertices->pts_rgb8_normal_edges);
			*length = numVertices * sizeof(pexVertexRgb8NormalEdges);
			bcopy(src, pStream, *length);
		} else if (colorType == Xpex_Rgb16Color) {
			char *src = (char *) (vertices->pts_rgb16_normal_edges);
			*length = numVertices * sizeof(pexVertexRgb16NormalEdges);
			bcopy(src, pStream, *length);
		}
	} else {
		*length = 0;
	}
}

void
Xpex_pack_list_of_vertex(mask, colorType, 
	vertexLists, numLists, stream, total_length)
	XpexBitmaskShort mask;
	XpexEnumTypeIndex colorType;
	XpexListOfVertex *vertexLists;
	int numLists;
	char *stream; /* RETURN */
	int *total_length; /* RETURN */
{
	register int i;
	int length;

	*total_length = 0;

	for (i = 0; i < numLists; i++) {
		CARD32 *c32 = (CARD32 *) stream;
		*c32 = vertexLists[i].numVertices;
		stream += sizeof(CARD32);
		Xpex_pack_array_of_vertex(mask, colorType, 
		&(vertexLists[i].vertices), (int) *c32, 
		stream, &length);
		stream += length;
		*total_length += (sizeof(CARD32) + length);
	}
}


