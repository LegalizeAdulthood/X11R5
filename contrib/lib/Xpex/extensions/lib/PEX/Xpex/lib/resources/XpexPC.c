/* $Header: XpexPC.c,v 2.8 91/09/11 16:07:21 sinyaw Exp $ */

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

#include <stdio.h>
#include "Xpexlibint.h"
#include "Xpexlib.h"
#include <X11/extensions/Xext.h>
#include "extutil.h"
#include "Xpextutil.h"

#define XpexPACK_CARD8(_DST,_SRC) \
{ \
CARD8 *p = (CARD8 *) (_DST); \
*p = (CARD8) (_SRC); \
(_DST) = ((CARD8 *)(_DST)) + sizeof(CARD32); \
}

#define XpexUNPACK_CARD8(_DST,_SRC) \
{ \
CARD8 *p = (CARD8 *) (_SRC); \
(_DST) = *p; \
(_SRC) = ((CARD8 *)(_SRC)) + sizeof(CARD32); \
}

#define XpexPACK_CARD16(_DST,_SRC) \
{ \
CARD16 *p = (CARD16 *) (_DST); \
*p = (CARD16) (_SRC); \
(_DST) = ((CARD8 *)(_DST)) + sizeof(CARD32); \
}

#define XpexUNPACK_CARD16(_DST,_SRC) \
{ \
CARD16 *p = (CARD16 *) (_SRC); \
(_DST) = (CARD16) *p; \
(_SRC) = ((CARD8 *)(_SRC)) + sizeof(CARD32); \
}

#define XpexPACK_CARD32(_DST,_SRC) \
{ \
CARD32 *dst = (CARD32 *) (_DST); \
*dst = (CARD32) (_SRC); \
(_DST) = ((CARD8 *)(_DST)) + sizeof(CARD32); \
}

#define XpexUNPACK_CARD32(_DST,_SRC) \
{ \
CARD32 *p = (CARD32 *) (_SRC); \
(_DST) = *p; \
(_SRC) = ((CARD8 *)(_SRC)) + sizeof(CARD32); \
}

#define XpexPACK_INT16(_DST,_SRC) \
{ \
INT16 *p = (INT16 *) (_DST); \
*p = (INT16) (_SRC); \
(_DST) = ((CARD8 *)(_DST)) + sizeof(CARD32); \
}

#define XpexUNPACK_INT16(_DST,_SRC) \
{ \
INT16 *p = (INT16 *)(_SRC); \
(_DST) = *p; \
(_SRC) = ((CARD8 *)(_SRC)) + sizeof(CARD32); \
}

#define XpexPACK_FLOAT(_DST,_SRC) \
{ \
XpexFloat *dst = (XpexFloat *) (_DST); \
*dst =  (XpexFloat) (_SRC); \
(_DST) = ((CARD8 *)(_DST)) + sizeof(PEXFLOAT); \
}

#define XpexUNPACK_FLOAT(_DST,_SRC) \
{ \
XpexFloat *src = (XpexFloat *)(_SRC); \
(_DST) = *src; \
(_SRC) = ((CARD8 *)(_SRC)) + sizeof(PEXFLOAT); \
}

#define XpexPACK_COORD_2D(_DST,_SRC,) \
{ \
pexCoord2D *dst = (pexCoord2D *) (_DST); \
dst->x = (_SRC).x; \
dst->y = (_SRC).y; \
(_DST) = ((CARD8 *)(_DST)) + sizeof(pexCoord2D); \
}

#define XpexUNPACK_COORD_2D(_DST,_SRC,) \
{ \
pexCoord2D *src = (pexCoord2D *) (_SRC); \
(_DST).x = src->x; \
(_DST).y = src->y; \
(_SRC) = ((CARD8 *)(_SRC)) + sizeof(pexCoord2D); \
}

#define XpexPACK_COORD_3D(_DST,_SRC) \
{ \
pexCoord3D *dst = (pexCoord3D *) (_DST); \
dst->x = (_SRC).x; \
dst->y = (_SRC).y; \
dst->z = (_SRC).z; \
(_DST) = ((CARD8 *)(_DST)) + sizeof(pexCoord3D); \
}

#define XpexUNPACK_COORD_3D(_DST,_SRC,) \
{ \
pexCoord3D *src = (pexCoord3D *) (_SRC); \
(_DST).x = src->x; \
(_DST).y = src->y; \
(_DST).z = src->z; \
(_SRC) = ((CARD8 *)(_SRC)) + sizeof(pexCoord3D); \
}

#define XpexPACK_TEXT_ALIGNMENT(_DST,_SRC) \
{ \
pexTextAlignmentData *dst = (pexTextAlignmentData *) (_DST); \
dst->vertical = (CARD16) (_SRC).vertical; \
dst->horizontal = (CARD16) (_SRC).horizontal; \
(_DST) = ((CARD8 *)(_DST)) + sizeof(pexTextAlignmentData); \
}

#define XpexUNPACK_TEXT_ALIGNMENT(_DST,_SRC) \
{ \
pexTextAlignmentData *src = (pexTextAlignmentData *) (_SRC); \
(_DST).vertical = src->vertical; \
(_DST).horizontal = src->horizontal; \
(_SRC) = ((CARD8 *)(_SRC)) + sizeof(pexTextAlignmentData); \
}

#define XpexPACK_VECTOR_2D(_DST,_SRC) \
{ \
pexVector2D *dst = (pexVector2D *) (_DST); \
dst->x = (XpexFloat) (_SRC).x; \
dst->y = (XpexFloat) (_SRC).y; \
(_DST) = ((CARD8 *)(_DST)) + sizeof(pexVector2D); \
}

#define XpexUNPACK_VECTOR_2D(_DST,_SRC) \
{ \
pexVector2D *src = (pexVector2D *) (_SRC); \
(_DST).x = src->x; \
(_DST).y = src->y; \
(_SRC) = ((CARD8 *)(_SRC)) + sizeof(pexVector2D); \
}

#define XpexPACK_VECTOR_3D(_DST,_SRC) \
{ \
pexVector3D *dst = (pexVector3D *) (_DST); \
dst->x =  (_SRC).x; \
dst->y =  (_SRC).y; \
dst->z =  (_SRC).z; \
(_DST) = ((CARD8 *)(_DST)) + sizeof(pexVector3D); \
}

#define XpexUNPACK_VECTOR_3D(_DST,_SRC) \
{ \
pexVector3D *src = (pexVector3D *) (_SRC); \
(_DST).x = src->x; \
(_DST).y = src->y; \
(_DST).z = src->z; \
(_SRC) = ((CARD8 *)(_SRC)) + sizeof(pexVector3D); \
}

#define XpexPACK_COLOR_SPECIFIER(_DST,_SRC) \
{\
int bytes; \
pexColourSpecifier *p = (pexColourSpecifier *) (_DST); \
p->colourType = (pexColourType) ((_SRC).color_type); \
(_DST) += sizeof(pexColourSpecifier); \
Xpex_pack_color(p->colourType, &((_SRC).value), (_DST), &bytes); \
(_DST) += bytes; \
}

#define XpexUNPACK_COLOR_SPECIFIER(_DST,_SRC) \
{ \
int bytesUnpacked; \
pexColourType *ct = (pexColourType *)(_SRC); \
(_SRC) = ((CARD8 *)(_SRC)) + sizeof(pexColourSpecifier); \
Xpex_unpack_color(*ct, (_SRC), &((_DST).value), &bytesUnpacked); \
(_SRC) = ((CARD8 *)(_SRC)) + bytesUnpacked; \
}

#define XpexPACK_REFLECTION_ATTR(_DST,_SRC) \
{ \
int bytes; \
pexReflectionAttr *p = (pexReflectionAttr *) (_DST); \
p->ambient = (XpexFloat)((_SRC).ambient); \
p->diffuse = (XpexFloat)((_SRC).diffuse); \
p->specular = (XpexFloat)((_SRC).specular); \
p->specularConc = (XpexFloat)((_SRC).specular_conc); \
p->specularColour.colourType = \
(pexColourType) ((_SRC).specular_color.color_type); \
(_DST) += sizeof(pexReflectionAttr); \
Xpex_pack_color((_SRC).specular_color.color_type, \
&((_SRC).specular_color.value), (_DST), &bytes); \
(_DST) += bytes; \
}

#define XpexUNPACK_REFLECTION_ATTR(_DST,_SRC) \
{ \
pexReflectionAttr *src = (pexReflectionAttr *) (_SRC); \
(_DST).ambient = src->ambient; \
(_DST).diffuse = src->diffuse; \
(_DST).specular = src->specular; \
(_DST).specular_conc = src->specularConc; \
(_DST).transmission = src->transmission; \
(_DST).specular_color.color_type = \
color_type = src->specularColour.colourType; \
(_SRC) = (CARD8 *) (++src); \
Xpex_unpack_color(color_type, (_SRC), \
&((_DST).specular_color.value), &length); \
(_SRC) += length; \
}

#define XpexPACK_MATRIX(_DST,_SRC) \
bcopy((char *)(_SRC),(char *)(_DST),sizeof(pexMatrix)); \
(_DST) = ((CARD8 *)(_DST)) + sizeof(pexMatrix)

#define XpexPACK_MATRIX3X3(_DST,_SRC) \
bcopy((char *)(_SRC),(char *)(_DST),sizeof(pexMatrix3X3)); \
(_DST) = ((CARD8 *)(_DST)) + sizeof(pexMatrix3X3)

#define XpexUNPACK_MATRIX(_DST,_SRC) \
bcopy((char *)(_SRC),(char *)(_DST),sizeof(pexMatrix)); \
(_SRC) = ((CARD8 *)(_SRC)) + sizeof(pexMatrix)

#define XpexUNPACK_MATRIX3X3(_DST,_SRC) \
bcopy((char *)(_SRC),(char *)(_DST),sizeof(pexMatrix3X3)); \
(_SRC) = ((CARD8 *)(_SRC)) + sizeof(pexMatrix3X3)

#define XpexUNPACK_SURFACE_APPROX(_DST,_SRC) \
{ \
pexSurfaceApprox *src = (pexSurfaceApprox *) (_SRC); \
(_DST).approxMethod = src->approxMethod; \
(_DST).uTolerance = src->uTolerance; \
(_DST).vTolerance = src->vTolerance; \
_SRC = ((CARD8 *)(_SRC)) + sizeof(pexSurfaceApprox); \
}

#define XpexUNPACK_CURVE_APPROX(_DST,_SRC) \
{ \
pexCurveApprox *src = (pexCurveApprox *)(_SRC); \
(_DST).approxMethod = src->approxMethod; \
(_DST).tolerance = src->tolerance; \
(_SRC) = ((CARD8 *)(_SRC)) + sizeof(pexCurveApprox); \
}

#define XpexSIZE_OF_COLOR(_t) \
( (_t == PEXIndexedColour) || (_t == PEXRgb8Colour) ? 4 : \
(_t == PEXRgb16Colour) ? 8 : 3 * sizeof(XpexFloat))

#define Xpex_size_of_color_specifier(_t) \
sizeof(pexColourSpecifier) + XpexSIZE_OF_COLOR(_t)

extern void Xpex_pack_color();
extern void Xpex_unpack_color();

static int sizeof_pc_values();
static void pack_pc_values();
static void unpack_pc_values();

int size_of_psc_data();

XpexPC
XpexCreatePC(dpy, valuemask, values)
	Display *dpy;
	XpexPCBitmask valuemask;
	XpexPCValues *values;
{
	pexPC	pc_id;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	{
		register pexCreatePipelineContextReq *req;
		int extra = sizeof_pc_values(valuemask, values);

		XpexGetReqExtra(CreatePipelineContext, dpy, i, extra, req);
		req->fpFormat = XpexGetFloatingPointFormat(dpy);
		req->pc = pc_id = XAllocID(dpy);
		req->itemMask[0] = valuemask[0];
		req->itemMask[1] = valuemask[1];
		req->itemMask[2] = valuemask[2];
		if (extra) {
    		pack_pc_values(valuemask, values, (CARD8 *)(req + 1));
		}
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return pc_id;
}

void
XpexCopyPC(dpy, src, dst, valuemask)
	Display  *dpy;
	XpexPC  src, dst;
	XpexPCBitmask valuemask;
{
	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);

	LockDisplay(dpy);
	{
		register pexCopyPipelineContextReq *req;

		XpexGetReq(CopyPipelineContext, dpy, i, req);
		req->src = src;
		req->dst = dst;
		req->itemMask[0] = valuemask[0];
		req->itemMask[1] = valuemask[1];
		req->itemMask[2] = valuemask[2];
	}
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexFreePC(dpy, pc_id)
	Display	*dpy;
	XpexPC	pc_id;
{	
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);

	LockDisplay(dpy);
	{
		register pexFreePipelineContextReq *req;

		XpexGetResReq(FreePipelineContext, dpy, i, pc_id, req);
	}
	UnlockDisplay(dpy);
	SyncHandle();
}
 
Status 
XpexGetPC(dpy, pc, valueMask, values) 
	Display	*dpy; 
	XpexPC	pc; 
	XpexPCBitmask valueMask;
	XpexPCValues *values; /* RETURN */ 
{
	CARD8 *list;

	if (!_XpexGetPC(dpy, pc, valueMask, &list)) {
		return 0;
	}
	unpack_pc_values(list, valueMask, values);

	if (list) {
		XFree(list);
	}
	return 1;
}

static Status 
_XpexGetPC(dpy, pc, valueMask, list) 
	Display	*dpy; 
	XpexPC	pc; 
	XpexPCBitmask valueMask;
	CARD8 **list; /* RETURN */ 
{
	register	pexGetPipelineContextReq	*req;
				pexGetPipelineContextReply	reply;
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);

	Status 		status = 1;
	unsigned 	size;

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReq(GetPipelineContext, dpy, i, req);
	req->fpFormat = (pexEnumTypeIndex) XpexGetFloatingPointFormat(dpy);
	req->pc = pc;
	req->itemMask[0] = valueMask[0];
	req->itemMask[1] = valueMask[1];
	req->itemMask[2] = valueMask[2];
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*list = (CARD8 *)Xmalloc(size)) {
			_XRead(dpy, *list, (long)size);
		} else {
			status = 0;
			_XpexClearReply(dpy, reply.length);
		}
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return status;
}

void
XpexChangePC(dpy, pc, valueMask, values)
	Display  *dpy;
	XpexPC  pc;
	XpexPCBitmask valueMask;
	XpexPCValues  *values; 
{
	register	pexChangePipelineContextReq *req;
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	int extra = sizeof_pc_values(valueMask, values);

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReqExtra(ChangePipelineContext, dpy, i, extra, req);
	req->fpFormat = (pexEnumTypeIndex) XpexGetFloatingPointFormat(dpy);
	req->pc = pc;
	req->itemMask[0] = valueMask[0];
	req->itemMask[1] = valueMask[1];
	req->itemMask[2] = valueMask[2];
    pack_pc_values(valueMask, values, (CARD8 *)(req + 1));
	UnlockDisplay(dpy);
	SyncHandle();
}

int
size_of_psc_data(p)
	register XpexParaSurfChar *p;
{
	switch (p->characteristics) {
	case PEXPSCNone:
	case PEXPSCImpDep:
		return 0;
	case PEXPSCIsoCurves:
		return sizeof(pexPSC_IsoparametricCurves);
	case PEXPSCMcLevelCurves:
		return (sizeof(pexPSC_LevelCurves) +
		p->record.mc_curves.numberIntersections * 
		sizeof(pexCoord3D));
	case PEXPSCWcLevelCurves:
		return (sizeof(pexPSC_LevelCurves) +
		p->record.wc_curves.numberIntersections * 
		sizeof(pexCoord3D));
	}
	return 0;
}

static int
sizeof_pc_values(valueMask, values)
	XpexPCBitmask valueMask;
	XpexPCValues *values;
{
	int size = 0;

	if (valueMask[0] & XpexPC_MarkerType) {
		size += sizeof(CARD32);
	}
	if (valueMask[0] & XpexPC_MarkerScale) {
		size += sizeof(XpexFloat);
	}
	if (valueMask[0] & XpexPC_MarkerColor) {
		size += sizeof(pexColourSpecifier) +
		XpexSIZE_OF_COLOR(values->marker_color.color_type);
	}
	if (valueMask[0] & XpexPC_MarkerBundleIndex) {
		size += sizeof(CARD32);
	}
	if (valueMask[0] & XpexPC_TextFont) {
		size += sizeof(CARD32);
	}
	if (valueMask[0] & XpexPC_TextPrecision) {
		size += sizeof(CARD32);
	}
	if (valueMask[0] & XpexPC_CharExpansion) {
		size += sizeof(XpexFloat);
	}
	if (valueMask[0] & XpexPC_CharSpacing) {
		size += sizeof(XpexFloat);
	}
	if (valueMask[0] & XpexPC_TextColor) {
		size += sizeof(pexColourSpecifier) +
		XpexSIZE_OF_COLOR(values->text_color.color_type);
	}
	if (valueMask[0] & XpexPC_CharHeight) {
		size += sizeof(XpexFloat);
	}
	if (valueMask[0] & XpexPC_CharUpVector) {
		size += sizeof(pexVector2D);
	}
	if (valueMask[0] & XpexPC_TextPath) {
		size += sizeof(CARD32);
	}
	if (valueMask[0] & XpexPC_TextAlignment) {
		size += sizeof(pexTextAlignmentData);
	}
	if (valueMask[0] & XpexPC_AtextHeight) {
		size += sizeof(XpexFloat);
	}
	if (valueMask[0] & XpexPC_AtextUpVector) {
		size += sizeof(pexVector2D);
	}
	if (valueMask[0] & XpexPC_AtextPath) {
		size += sizeof(CARD32);
	}
	if (valueMask[0] & XpexPC_AtextAlignment) {
		size += sizeof(pexTextAlignmentData);
	}
	if (valueMask[0] & XpexPC_AtextStyle) {
		size += sizeof(CARD32);
	}
	if (valueMask[0] & XpexPC_TextBundleIndex) {
		size += sizeof(CARD32);
	}
	if (valueMask[0] & XpexPC_LineType) {
		size += sizeof(CARD32);
	}
	if (valueMask[0] & XpexPC_LineWidth) {
		size += sizeof(XpexFloat);
	}
	if (valueMask[0] & XpexPC_LineColor) {
		size += sizeof(pexColourSpecifier) +
		XpexSIZE_OF_COLOR(values->line_color.color_type);
	}
	if (valueMask[0] & XpexPC_CurveApproximation) {
		size += sizeof(pexCurveApprox);
	}
	if (valueMask[0] & XpexPC_PolylineInterp) {
		size += sizeof(CARD32);
	}
	if (valueMask[0] & XpexPC_LineBundleIndex) {
		size += sizeof(CARD32);
	}
	if (valueMask[0] & XpexPC_InteriorStyle) {
		size += sizeof(CARD32);
	}
	if (valueMask[0] & XpexPC_SurfaceColor) {
		size += sizeof(pexColourSpecifier) +
		XpexSIZE_OF_COLOR(values->surface_color.color_type);
	}
	if (valueMask[0] & XpexPC_SurfaceReflAttr) {
		size += sizeof(pexReflectionAttr) +
		XpexSIZE_OF_COLOR(values->reflection_attr.specular_color.color_type);
	}
	if (valueMask[0] & XpexPC_SurfaceReflModel) {
		size += sizeof(CARD32);
	}
	if (valueMask[0] & XpexPC_SurfaceInterp) {
		size += sizeof(CARD32);
	}
	if (valueMask[0] & XpexPC_BfInteriorStyle) {
		size += sizeof(CARD32);
	}
	/* check second bitmask */

	if (valueMask[1] & XpexPC_BfInteriorStyleIndex) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_BfSurfaceColor) {
		size += sizeof(pexColourSpecifier) +
		XpexSIZE_OF_COLOR(values->bf_surface_color.color_type);
	}
	if (valueMask[1] & XpexPC_BfSurfaceReflAttr) {
		size += sizeof(pexReflectionAttr) +
		XpexSIZE_OF_COLOR(values->bf_reflection_attr.specular_color.color_type);
	}
	if (valueMask[1] & XpexPC_BfSurfaceReflModel) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_BfSurfaceInterp) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_SurfaceApproximation) {
		size += sizeof(pexSurfaceApprox);
	}
	if (valueMask[1] & XpexPC_CullingMode) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_DistinguishFlag) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_PatternSize) {
		size += sizeof(pexCoord2D);
	}
	if (valueMask[1] & XpexPC_PatternRefPt) {
		size += sizeof(pexCoord3D);
	}
	if (valueMask[1] & XpexPC_PatternRefVec1) {
		size += sizeof(pexVector3D);
	}
	if (valueMask[1] & XpexPC_PatternRefVec2) {
		size += sizeof(pexVector3D);
	}
	if (valueMask[1] & XpexPC_InteriorBundleIndex) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_SurfaceEdgeFlag) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_SurfaceEdgeType) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_SurfaceEdgeWidth) {
		size += sizeof(XpexFloat);
	}
	if (valueMask[1] & XpexPC_SurfaceEdgeColor) {
		size += sizeof(pexColourSpecifier) +
		XpexSIZE_OF_COLOR(values->surface_edge_color.color_type);
	}
	if (valueMask[1] & XpexPC_EdgeBundleIndex) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_LocalTransform) {
		size += sizeof(pexMatrix);
	}
	if (valueMask[1] & XpexPC_GlobalTransform) {
		size += sizeof(pexMatrix);
	}
	if (valueMask[1] & XpexPC_ModelClip) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_ModelClipVolume) {
		size += sizeof(CARD32) + 
		values->model_clip_volume.num_half_spaces * sizeof(pexHalfSpace);
	}
	if (valueMask[1] & XpexPC_ViewIndex) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_LightState) {
		size += sizeof(CARD32) +
		values->light_state.num_indices * sizeof(pexTableIndex);
	}
	if (valueMask[1] & XpexPC_DepthCueIndex) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_SetAsfValues) {
		size += sizeof(CARD32) + sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_PickId) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_HlhsrIdentifier) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_NameSet) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_ColorApproxIndex) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_RenderingColorModel) {
		size += sizeof(CARD32);
	}
	if (valueMask[1] & XpexPC_ParaSurfCharacteristics) {
		/* type + length + data */
		size += sizeof(CARD32) + 
		size_of_psc_data(&(values->psurf_char));
	}
	return size;
}

static void
pack_pc_values(valueMask, values, list)
	XpexPCBitmask valueMask;
	XpexPCValues  *values;
	CARD8 *list; /* RETURN */
{
	if (valueMask[0] & XpexPC_MarkerType) {
		XpexPACK_INT16(list,values->marker_type);
	}
	if (valueMask[0] & XpexPC_MarkerScale) {
		XpexPACK_FLOAT(list,values->marker_scale);
	}
	if (valueMask[0] & XpexPC_MarkerColor) {
		XpexPACK_COLOR_SPECIFIER(list,values->marker_color);
	} 
	if (valueMask[0] & XpexPC_MarkerBundleIndex) {
		XpexPACK_CARD16(list,values->marker_bundle_index);
	}
	if (valueMask[0] & XpexPC_TextFont) {
		XpexPACK_CARD16(list,values->text_font_index);
	}
	if (valueMask[0] & XpexPC_TextPrecision) {
		XpexPACK_CARD16(list,values->text_precision);
	}
	if (valueMask[0] & XpexPC_CharExpansion) {
		XpexPACK_FLOAT(list,values->char_expansion);
	}
	if (valueMask[0] & XpexPC_CharSpacing) {
		XpexPACK_FLOAT(list,values->char_spacing);
	}
	if (valueMask[0] & XpexPC_TextColor) {
		XpexPACK_COLOR_SPECIFIER(list,values->text_color);
	}
	if (valueMask[0] & XpexPC_CharHeight) {
		XpexPACK_FLOAT(list,values->char_height);
	}
	if (valueMask[0] & XpexPC_CharUpVector) {
		XpexPACK_VECTOR_2D(list,values->char_up_vector);
	}
	if (valueMask[0] & XpexPC_TextPath) {
		XpexPACK_CARD16(list,values->text_path);
	}
	if (valueMask[0] & XpexPC_TextAlignment) {
		XpexPACK_TEXT_ALIGNMENT(list,values->text_alignment);
	}
	if (valueMask[0] & XpexPC_AtextHeight) {
		XpexPACK_FLOAT(list,values->atext_height);
	}
	if (valueMask[0] & XpexPC_AtextUpVector) {
		XpexPACK_VECTOR_2D(list,values->atext_up_vector);
	}
	if (valueMask[0] & XpexPC_AtextPath) {
		XpexPACK_CARD16(list,values->atext_path);
	}
	if (valueMask[0] & XpexPC_AtextAlignment) {
		XpexPACK_TEXT_ALIGNMENT(list,values->atext_alignment);
	}
	if (valueMask[0] & XpexPC_AtextStyle) {
		XpexPACK_INT16(list,values->atext_style);
	}
	if (valueMask[0] & XpexPC_TextBundleIndex) {
		XpexPACK_CARD16(list,values->text_bundle_index);
	}
	if (valueMask[0] & XpexPC_LineType) {
		XpexPACK_INT16(list,values->line_type);
	}
	if (valueMask[0] & XpexPC_LineWidth) {
		XpexPACK_FLOAT(list,values->line_width);
	}
	if (valueMask[0] & XpexPC_LineColor) {
		XpexPACK_COLOR_SPECIFIER(list,values->line_color);
	}
	if (valueMask[0] & XpexPC_CurveApproximation) {
		XpexPACK_INT16(list,values->curve_approx.approxMethod);
		XpexPACK_FLOAT(list,values->curve_approx.tolerance);
	}
	if (valueMask[0] & XpexPC_PolylineInterp) {
		XpexPACK_INT16(list,values->polyline_interp);
	}
	if (valueMask[0] & XpexPC_LineBundleIndex) {
		XpexPACK_CARD16(list,values->line_bundle_index);
	}
	if (valueMask[0] & XpexPC_InteriorStyle) {
		XpexPACK_INT16(list,values->interior_style);
	}
	if (valueMask[0] & XpexPC_InteriorStyleIndex) {
		XpexPACK_CARD16(list,values->interior_style_index);
	}
	if (valueMask[0] & XpexPC_SurfaceColor) {
		XpexPACK_COLOR_SPECIFIER(list,values->surface_color);
	}
	if (valueMask[0] & XpexPC_SurfaceReflAttr) {
		XpexPACK_REFLECTION_ATTR(list,values->reflection_attr);
	}
	if (valueMask[0] & XpexPC_SurfaceReflModel) {
		XpexPACK_INT16(list,values->reflection_model);
	}
	if (valueMask[0] & XpexPC_SurfaceInterp) {
		XpexPACK_INT16(list,values->surface_interp);
	}
	if (valueMask[0] & XpexPC_BfInteriorStyle) {
		XpexPACK_INT16(list,values->bf_interior_style);
	}
	if (valueMask[1] & XpexPC_BfInteriorStyleIndex) {
		XpexPACK_CARD16(list,values->bf_interior_style_index);
	}
	if (valueMask[1] & XpexPC_BfSurfaceColor) {
		XpexPACK_COLOR_SPECIFIER(list,values->bf_surface_color);
	}
	if (valueMask[1] & XpexPC_BfSurfaceReflAttr) {
		XpexPACK_REFLECTION_ATTR(list,values->bf_reflection_attr);
	}
	if (valueMask[1] & XpexPC_BfSurfaceReflModel) {
		XpexPACK_INT16(list,values->bf_reflection_model);
	}
	if (valueMask[1] & XpexPC_BfSurfaceInterp) {
		XpexPACK_INT16(list,values->bf_surface_interp);
	}
	if (valueMask[1] & XpexPC_SurfaceApproximation) {
		XpexPACK_INT16(list,values->surface_approx.approxMethod);
		XpexPACK_FLOAT(list,values->surface_approx.uTolerance);
		XpexPACK_FLOAT(list,values->surface_approx.vTolerance);
	}
	if (valueMask[1] & XpexPC_CullingMode) {
		XpexPACK_CARD16(list,values->culling_mode);
	}
	if (valueMask[1] & XpexPC_DistinguishFlag) {
		XpexPACK_CARD8(list,values->distinguish_flag);
	}
	if (valueMask[1] & XpexPC_PatternSize) {
		XpexPACK_COORD_2D(list,values->pattern_size);
	}
	if (valueMask[1] & XpexPC_PatternRefPt) {
		XpexPACK_COORD_3D(list,values->pattern_ref_pt);
	}
	if (valueMask[1] & XpexPC_PatternRefVec1) {
		XpexPACK_VECTOR_3D(list,values->pattern_ref_vec1);
	}
	if (valueMask[1] & XpexPC_PatternRefVec2) {
		XpexPACK_VECTOR_3D(list,values->pattern_ref_vec2);
	}
	if (valueMask[1] & XpexPC_InteriorBundleIndex) {
		XpexPACK_CARD16(list,values->interior_bundle_index);
	}
	if (valueMask[1] & XpexPC_SurfaceEdgeFlag) {
		XpexPACK_CARD16(list,values->surface_edge_flag);
	}
	if (valueMask[1] & XpexPC_SurfaceEdgeType) {
		XpexPACK_INT16(list,values->surface_edge_type);
	}
	if (valueMask[1] & XpexPC_SurfaceEdgeWidth) {
		XpexPACK_FLOAT(list,values->surface_edge_width);
	}
	if (valueMask[1] & XpexPC_SurfaceEdgeColor) {
		XpexPACK_COLOR_SPECIFIER(list,values->surface_edge_color);
	}
	if (valueMask[1] & XpexPC_EdgeBundleIndex) {
		XpexPACK_CARD16(list,values->edge_bundle_index);
	}
	if (valueMask[1] & XpexPC_LocalTransform) {
		XpexPACK_MATRIX(list,&(values->local_transform[0][0]));
	}
	if (valueMask[1] & XpexPC_GlobalTransform) {
		XpexPACK_MATRIX(list,&(values->global_transform[0][0]));
	}
	if (valueMask[1] & XpexPC_ModelClip) {
		XpexPACK_CARD16(list,values->model_clip);
	}
	if (valueMask[1] & XpexPC_ModelClipVolume) {
		int size = values->model_clip_volume.num_half_spaces *
			sizeof(pexHalfSpace);
		XpexPACK_CARD32(list,values->model_clip_volume.num_half_spaces);

		bcopy((char *)values->model_clip_volume.half_spaces,
		(char *)list, size);
		list = ((CARD8 *)(list)) + size;
	}
	if (valueMask[1] & XpexPC_ViewIndex) {
		XpexPACK_CARD16(list,values->view_index);
	}
	if (valueMask[1] & XpexPC_LightState) {
		int size = 
			sizeof(pexTableIndex) * 
			values->light_state.num_indices;

		XpexPACK_CARD16(list,values->light_state.num_indices);
		bcopy((char *)values->light_state.indices,(char *)list,
		size);

		list = ((CARD8 *)(list)) +  size + PADDING(size);
	}
	if (valueMask[1] & XpexPC_DepthCueIndex) {
		XpexPACK_CARD16(list,values->depth_cue_index);
	}
	if (valueMask[1] & XpexPC_SetAsfValues) {
		XpexPACK_CARD32(list,values->asf_enables);
		XpexPACK_CARD32(list,values->asfs);
	}
	if (valueMask[1] & XpexPC_PickId) {
		XpexPACK_CARD32(list,values->pick_id);
	}
	if (valueMask[1] & XpexPC_HlhsrIdentifier) {
		XpexPACK_CARD32(list,values->hlhsr_identifier);
	}
	if (valueMask[1] & XpexPC_NameSet) {
		XpexPACK_CARD32(list,values->name_set);
	}
	if (valueMask[1] & XpexPC_ColorApproxIndex) {
		XpexPACK_CARD16(list,values->color_approx_index);
	}
	if (valueMask[1] & XpexPC_RenderingColorModel) {
		XpexPACK_CARD16(list,values->rdr_color_model);
	}
	if (valueMask[1] & XpexPC_ParaSurfCharacteristics) {
		int bytes_packed;
		/* dst,src,length */
		pack_psc_data(list,&(values->psurf_char),&bytes_packed);
		list += bytes_packed;
	}
}

static void
unpack_pc_values(list, valueMask, values)
	CARD8 *list;
	XpexPCBitmask valueMask;
	XpexPCValues *values; /* RETURN */
{
	XpexColorType color_type;
	int length;

	if (valueMask[0] & XpexPC_MarkerType) {
		XpexUNPACK_INT16(values->marker_type,list);
	}
	if (valueMask[0] & XpexPC_MarkerScale) {
		XpexUNPACK_FLOAT(values->marker_scale,list);
	}
	if (valueMask[0] & XpexPC_MarkerColor){
		XpexUNPACK_COLOR_SPECIFIER(values->marker_color,list);
	} 
	if (valueMask[0] & XpexPC_MarkerBundleIndex) {
		XpexUNPACK_CARD16(values->marker_bundle_index,list);
	}
	if (valueMask[0] & XpexPC_TextFont) {
		XpexUNPACK_CARD16(values->text_font_index,list);
	}
	if (valueMask[0] & XpexPC_TextPrecision) {
		XpexUNPACK_CARD16(values->text_precision,list);
	}
	if (valueMask[0] & XpexPC_CharExpansion) {
		XpexUNPACK_FLOAT(values->char_expansion,list);
	}
	if (valueMask[0] & XpexPC_CharSpacing) {
		XpexUNPACK_FLOAT(values->char_spacing,list);
	}
	if (valueMask[0] & XpexPC_TextColor) {
		XpexUNPACK_COLOR_SPECIFIER(values->text_color,list);
	}
	if (valueMask[0] & XpexPC_CharHeight) {
		XpexUNPACK_FLOAT(values->char_height,list);
	}
	if (valueMask[0] & XpexPC_CharUpVector) {
		XpexUNPACK_VECTOR_2D(values->char_up_vector,list);
	}
	if (valueMask[0] & XpexPC_TextPath) {
		XpexUNPACK_CARD16(values->text_path,list);
	}
	if (valueMask[0] & XpexPC_TextAlignment) {
		XpexUNPACK_TEXT_ALIGNMENT(values->text_alignment,list);
	}
	if (valueMask[0] & XpexPC_AtextHeight) {
		XpexUNPACK_FLOAT(values->atext_height,list);
	}
	if (valueMask[0] & XpexPC_AtextUpVector) {
		XpexUNPACK_VECTOR_2D(values->atext_up_vector,list);
	}
	if (valueMask[0] & XpexPC_AtextPath) {
		XpexUNPACK_CARD16(values->atext_path,list);
	}
	if (valueMask[0] & XpexPC_AtextAlignment) {
		XpexUNPACK_TEXT_ALIGNMENT(values->atext_alignment,list);
	}
	if (valueMask[0] & XpexPC_AtextStyle) {
		XpexUNPACK_INT16(values->atext_style,list);
	}
	if (valueMask[0] & XpexPC_TextBundleIndex) {
		XpexUNPACK_CARD16(values->text_bundle_index,list);
	}
	if (valueMask[0] & XpexPC_LineType) {
		XpexUNPACK_INT16(values->line_type,list);
	}
	if (valueMask[0] & XpexPC_LineWidth) {
		XpexUNPACK_FLOAT(values->line_width,list);
	}
	if (valueMask[0] & XpexPC_LineColor) {
		XpexUNPACK_COLOR_SPECIFIER(values->line_color,list);
	}
	if (valueMask[0] & XpexPC_CurveApproximation) {
		XpexUNPACK_CURVE_APPROX(values->curve_approx,list);
	}
	if (valueMask[0] & XpexPC_PolylineInterp) {
		XpexUNPACK_INT16(values->polyline_interp,list);
	}
	if (valueMask[0] & XpexPC_LineBundleIndex) {
		XpexUNPACK_CARD16(values->line_bundle_index,list);
	}
	if (valueMask[0] & XpexPC_InteriorStyle) {
		XpexUNPACK_INT16(values->interior_style,list);
	}
	if (valueMask[0] & XpexPC_InteriorStyleIndex) {
		XpexUNPACK_CARD16(values->interior_style_index,list);
	}
	if (valueMask[0] & XpexPC_SurfaceColor) {
		XpexUNPACK_COLOR_SPECIFIER(values->surface_color,list);
	}
	if (valueMask[0] & XpexPC_SurfaceReflAttr){
		XpexUNPACK_REFLECTION_ATTR(values->reflection_attr,list);
	} 
	if (valueMask[0] & XpexPC_SurfaceReflModel) {
		XpexUNPACK_INT16(values->reflection_model,list);
	}
	if (valueMask[0] & XpexPC_SurfaceInterp) {
		XpexUNPACK_INT16(values->surface_interp,list);
	}
	if (valueMask[0] & XpexPC_BfInteriorStyle) {
		XpexUNPACK_INT16(values->bf_interior_style,list); 
	}
	if (valueMask[1] & XpexPC_BfInteriorStyleIndex) {
		XpexUNPACK_CARD16(values->bf_interior_style_index,list);
	}
	if (valueMask[1] & XpexPC_BfSurfaceColor) {
		XpexUNPACK_COLOR_SPECIFIER(values->line_color,list);
	}
	if (valueMask[1] & XpexPC_BfSurfaceReflAttr) {
		XpexUNPACK_REFLECTION_ATTR(values->bf_reflection_attr,list);
	}
	if (valueMask[1] & XpexPC_BfSurfaceReflModel) {
		XpexUNPACK_INT16(values->bf_reflection_model,list); 
	}
	if (valueMask[1] & XpexPC_BfSurfaceInterp) {
		XpexUNPACK_INT16(values->bf_surface_interp,list); 
	}
	if (valueMask[1] & XpexPC_SurfaceApproximation) {
		XpexUNPACK_SURFACE_APPROX(values->surface_approx,list);
	}
	if (valueMask[1] & XpexPC_CullingMode) {
		XpexUNPACK_CARD16(values->culling_mode,list); 
	}
	if (valueMask[1] & XpexPC_DistinguishFlag) {
		XpexUNPACK_CARD8(values->distinguish_flag,list);
	}
	if (valueMask[1] & XpexPC_PatternSize) {
		XpexUNPACK_COORD_2D(values->pattern_size,list);
	}
	if (valueMask[1] & XpexPC_PatternRefPt) {
		XpexUNPACK_COORD_3D(values->pattern_ref_pt,list);
	}
	if (valueMask[1] & XpexPC_PatternRefVec1) {
		XpexUNPACK_VECTOR_3D(values->pattern_ref_vec1,list);
	}
	if (valueMask[1] & XpexPC_PatternRefVec2) {
		XpexUNPACK_VECTOR_3D(values->pattern_ref_vec2,list);
	}
	if (valueMask[1] & XpexPC_InteriorBundleIndex) {
		XpexUNPACK_CARD16(values->interior_bundle_index,list);
	}
	if (valueMask[1] & XpexPC_SurfaceEdgeFlag) {
		XpexUNPACK_CARD16(values->surface_edge_flag,list);
	}
	if (valueMask[1] & XpexPC_SurfaceEdgeType) {
		XpexUNPACK_INT16(values->surface_edge_type,list);
	}
	if (valueMask[1] & XpexPC_SurfaceEdgeWidth) {
		XpexUNPACK_FLOAT(values->surface_edge_width,list);
	}
	if (valueMask[1] & XpexPC_SurfaceEdgeColor) {
		XpexUNPACK_COLOR_SPECIFIER(values->surface_edge_color,list);
	}
	if (valueMask[1] & XpexPC_EdgeBundleIndex) {
		XpexUNPACK_CARD16(values->edge_bundle_index,list);
	}
	if (valueMask[1] & XpexPC_LocalTransform) {
		XpexUNPACK_MATRIX(&(values->local_transform[0][0]),list);
	}
	if (valueMask[1] & XpexPC_GlobalTransform) {
		XpexUNPACK_MATRIX(&(values->global_transform[0][0]),list);
	}
	if (valueMask[1] & XpexPC_ModelClip) {
		XpexUNPACK_CARD16(values->model_clip,list);
	}
	if (valueMask[1] & XpexPC_ModelClipVolume) {
		CARD32 *src1 = (CARD32 *) list++;
		int size = *src1 * sizeof(pexHalfSpace);
		values->model_clip_volume.num_half_spaces = *src1;
		if (*src1) {
			if (values->model_clip_volume.half_spaces = 
			(pexHalfSpace *) Xmalloc(size)) {
				bcopy((char *)list,
				(char *)(values->model_clip_volume.half_spaces),
				size);
			}
			list += size;
		}
	}
	if (valueMask[1] & XpexPC_ViewIndex) {
		XpexUNPACK_CARD16(values->view_index,list);
	}
	if (valueMask[1] & XpexPC_LightState) {
		CARD32 *src1 = (CARD32 *) list++;
		pexTableIndex *src2;
		int size = *src1 * sizeof(pexTableIndex);
		values->light_state.num_indices = *src1;
		if (*src1) {
			if(values->light_state.indices = 
			(pexTableIndex *) Xmalloc(size)) {
				bcopy((char *)list, 
				(char *)(values->light_state.indices),
				size);
			}
			list += size + PADDING(size);
		}
	}
	if (valueMask[1] & XpexPC_DepthCueIndex) {
		XpexUNPACK_CARD16(values->depth_cue_index,list);
	}
	if (valueMask[1] & XpexPC_SetAsfValues) {
		CARD32 *p = (CARD32 *) list;
		values->asf_enables = *p++;
		values->asfs = *p++;
		list = (CARD8 *) ++p;
	}
	if (valueMask[1] & XpexPC_PickId) {
		XpexUNPACK_CARD32(values->pick_id,list);
	}
	if (valueMask[1] & XpexPC_HlhsrIdentifier) {
		XpexUNPACK_CARD32(values->hlhsr_identifier,list);
	}
	if (valueMask[1] & XpexPC_NameSet) {
		XpexUNPACK_CARD32(values->name_set,list);
	}
	if (valueMask[1] & XpexPC_ColorApproxIndex) {
		XpexUNPACK_CARD16(values->color_approx_index,list);
	}
	if (valueMask[1] & XpexPC_RenderingColorModel) {
		XpexUNPACK_CARD16(values->rdr_color_model,list);
	}
	if (valueMask[1] & XpexPC_ParaSurfCharacteristics) {
		extern void unpacked_psc_data();
		int bytes_unpacked;
		/* dst,src,length */
		unpack_psc_data(values->psurf_char,list,&bytes_unpacked); 
		list += bytes_unpacked;
	}
}
