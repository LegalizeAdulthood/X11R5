/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/


#ifndef PEXOC_H
#define PEXOC_H

#include <X11/Xlib.h>

#include "PEX.h"
#include "PEXprotost.h"


/* C programmers may choose to use these macros to create output commands
 * that set attributes instead of the subroutine calls in order to save the
 * overhead of a subroutine call for each output command. */

/* (There are no macros for routines which require a variable length list to
 * be passed.) */


#define AdjustColourSize (type) \
    ((type) == PEXIndexedColour ? sizeof(pxlColour)-sizeof(pexIndexedColour) : \
    ((type) == PEXRgb8Colour ? sizeof(pxlColour)-sizeof(pexRgb8Colour) : \
    ((type) == PEXRgb16Colour ? sizeof(pxlColour)-sizeof(pexRgb16Colour) : 0)))


/* PEX_SetMarkerType
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlEnumTypeIndex	*type;
 */
#define PEX_SetMarkerType(ocbuf, type)\
    PEXAddOC ((ocbuf), PEXOCMarkerType, sizeof(pxlEnumTypeIndex), (type))

/* PEX_SetMarkerScale
 *
 *  pxlOCBuf		*ocbuf;
 *  float		*scale;
 */
#define PEX_SetMarkerScale(ocbuf, scale) \
    PEXAddOC ((ocbuf), PEXOCMarkerScale, sizeof(float), (scale))

/* PEX_SetMarkerColourIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*index;
 */
#define PEX_SetMarkerColourIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), PEXOCMarkerColourIndex, sizeof(pxlTableIndex), (index))

/* PEX_SetMarkerColour
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlColourSpecifier  *colour;
 */
#define PEX_SetMarkerColour(ocbuf, colour) \
    PEXAddOC ((ocbuf), PEXOCMarkerColour, \ 
	sizeof(pxlColourSpecifier) - AdjustColourSize(colour->colourType), \
	(colour))

/* PEX_SetMarkerBundleIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*index;
 */
#define PEX_SetMarkerBundleIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), PEXOCMarkerBundleIndex, sizeof(pxlTableIndex), (index))

/* PEX_SetTextFontIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*index;
 */
#define PEX_SetTextFontIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), OCTextFontIndex, sizeof(pxlTableIndex), (index))

/* PEX_SetTextPrecision
 *
 *  pxlOCBuf		*ocbuf;
 *  CARD16		*precision;
 */
#define PEX_SetTextPrecision(ocbuf, precision) \
    PEXAddOC ((ocbuf), PEXOCTextPrecision, sizeof(CARD16), (precision))

/* PEX_SetCharExpansion
 *
 *  pxlOCBuf		*ocbuf;
 *  float		*expansion;
 */
#define PEX_SetCharExpansion(ocbuf, expansion) \
    PEXAddOC ((ocbuf), PEXOCCharExpansion, sizeof(float), (expansion))

/* PEX_SetCharSpacing
 *
 *  pxlOCBuf		*ocbuf;
 *  float		*spacing;
 */
#define PEX_SetCharSpacing(ocbuf, spacing) \
    PEXAddOC ((ocbuf), PEXOCCharSpacing, sizeof(float), (spacing))

/* PEX_SetTextColourIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*index;
 */
#define PEX_SetTextColourIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), PEXOCTextColourIndex, sizeof(pxlTableIndex), (index))

/* PEX_SetTextColour
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlColourSpecifier	*colour;
 */
#define PEX_SetTextColour(ocbuf, colour) \
    PEXAddOC ((ocbuf), PEXOCTextColour, \
	sizeof(pxlColourSpecifier) - AdjustColourSize(colour->colourType), \
	(colour))

/* PEX_SetCharHeight
 *
 *  pxlOCBuf		*ocbuf;
 *  float		*height;
 */
#define PEX_SetCharHeight(ocbuf, height) \
    PEXAddOC ((ocbuf), PEXOCCharHeight, sizeof(float), (height))

/* PEX_SetCharUpVector
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlVector2D		*vector;
 */
#define PEX_SetCharUpVector(ocbuf, vector) \
    PEXAddOC ((ocbuf), PEXOCCharUpVector, sizeof(pxlVector2D), (vector))

/* PEX_SetTextPath
 *
 *  pxlOCBuf		*ocbuf;
 *  CARD16		*path;
 */
#define PEX_SetTextPath(ocbuf, path) \
    PEXAddOC ((ocbuf), PEXOCTextPath, sizeof(CARD16), (path))

/* PEX_SetTextAlignment
 *
 *  pxlOCBuf			*ocbuf;
 *  pxlTextAlignmentData	*alignment;
 */
#define PEX_SetTextAlignment(ocbuf, alignment) \
    PEXAddOC ((ocbuf), PEXOCTextAlignment, sizeof(pxlTextAlignmentData), 
	(alignment))

/* PEX_SetATextHeight
 *
 *  pxlOCBuf		*ocbuf;
 *  float		*height;
 */
#define PEX_SetATextHeight(ocbuf, height) \
    PEXAddOC ((ocbuf), PEXOCAtextHeight, sizeof(float), (height))

/* PEX_SetATextUpVector
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlVector2D		*vector;
 */
#define PEX_SetATextUpVector(ocbuf, vector) \
    PEXAddOC ((ocbuf), PEXOCAtextUpVector, sizeof(pxlVector2D), (vector))

/* PEX_SetATextPath
 *
 *  pxlOCBuf		*ocbuf;
 *  CARD16		*path;
 */
#define PEX_SetATextPath(ocbuf, path) \
    PEXAddOC ((ocbuf), PEXOCAtextPath, sizeof(CARD16), (path))

/* PEX_SetATextAlignment
 *
 *  pxlOCBuf			*ocbuf;
 *  pxlTextAlignmentData	*alignment;
 */
#define PEX_SetATextAlignment(ocbuf, alignment) \
    PEXAddOC ((ocbuf), PEXOCAtextAlignment, \ 
	sizeof(pxlTextAlignmentData), (alignment))

/* PEX_SetATextStyle
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*style;
 */
#define PEX_SetATextStyle(ocbuf, style) \
    PEXAddOC ((ocbuf), PEXOCAtextStyle, sizeof(pxlTableIndex), (style))

/* PEX_SetTextBundleIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*index;
 */
#define PEX_SetTextBundleIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), PEXOCTextBundleIndex, sizeof(pxlTableIndex), (index))

/* PEX_SetLineType
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlEnumTypeIndex	*type;
 */
#define PEX_SetLineType(ocbuf, type) \
    PEXAddOC ((ocbuf), PEXOCLineType, sizeof(pxlEnumTypeIndex), (type))

/* PEX_SetLineWidth
 *
 *  pxlOCBuf		*ocbuf;
 *  float		*width;
 */
#define PEX_SetLineWidth(ocbuf, width) \
    PEXAddOC ((ocbuf), PEXOCLineWidth, sizeof(float), (width))

/* PEX_SetLineColourIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*index;
 */
#define PEX_SetLineColourIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), PEXOCLineColourIndex, sizeof(pxlTableIndex), (index))

/* PEX_SetLineColour
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlColourSpecifier	*colour;
 */
#define PEX_SetLineColour(ocbuf, colour) \
    PEXAddOC ((ocbuf), PEXOCLineColour, \
	sizeof(pxlColourSpecifier) - AdjustColourSize(colour->colourType), \
	(colour))

/* PEX_SetCurveApproxMethod
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlCurveApprox	*method;
 */
#define PEX_SetCurveApproxMethod(ocbuf, method) \
    PEXAddOC ((ocbuf), PEXOCCurveApproximation, sizeof(pxlCurveApprox), (method))

/* PEX_SetPolylineInterpMethod
 *
 *  pxlOCBuf		*ocbuf;
 *  CARD16		*method;
 */
#define PEX_SetPolylineInterpMethod(ocbuf, method) \
    PEXAddOC ((ocbuf), PEXOCPolylineInterp, sizeof(CARD16), (method))

/* PEX_SetLineBundleIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*index;
 */
#define PEX_SetLineBundleIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), PEXOCLineBundleIndex, sizeof(pxlTableIndex), (index))

/* PEX_SetInteriorStyle
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlEnumTypeIndex	*style;
 */
#define PEX_SetInteriorStyle(ocbuf, style) \
    PEXAddOC ((ocbuf), PEXOCInteriorStyle, sizeof(pxlEnumTypeIndex), (style))

/* PEX_SetInteriorStyleIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlEnumTypeIndex	*index;
 */
#define PEX_SetInteriorStyleIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), PEXOCInteriorStyleIndex, sizeof(pxlEnumTypeIndex), (index))

/* PEX_SetSurfaceColourIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*index;
 */
#define PEX_SetSurfaceColourIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), PEXOCSurfaceColourIndex, sizeof(pxlTableIndex), (index))

/* PEX_SetSurfaceColour
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlColourSpecifier	*colour;
 */
#define PEX_SetSurfaceColour(ocbuf, colour) \
    PEXAddOC ((ocbuf), PEXOCSurfaceColour, \
	sizeof(pxlColourSpecifier) - AdjustColourSize(colour->colourType), \
	(colour))

/* PEX_SetReflectionAttributes
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlReflectionAttr	*attr;
 */
#define PEX_SetReflectionAttributes(ocbuf, attr) \
    PEXAddOC ((ocbuf), PEXOCSurfaceReflAttr, 
	sizeof(pxlReflectionAttr) - \ 
	AdjustColourSize(attr->specularColour.colourType), (attr))

/* PEX_SetReflectionModel
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlEnumTypeIndex	*model;
 */
#define PEX_SetReflectionModel(ocbuf, model) \
    PEXAddOC ((ocbuf), PEXOCSurfaceReflModel, sizeof(pxlEnumTypeIndex), (model))

/* PEX_SetSurfaceInterpMethod
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlEnumTypeIndex	*method;
 */
#define PEX_SetSurfaceInterpMethod(ocbuf, method) \
    PEXAddOC ((ocbuf), OCSurfaceInterp, sizeof(pxlEnumTypeIndex), (method))

/* PEX_SetBFInteriorStyle
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlEnumTypeIndex	*style;
 */
#define PEX_SetBFInteriorStyle(ocbuf, style) \
    PEXAddOC ((ocbuf), PEXOCBfInteriorStyle, sizeof(pxlEnumTypeIndex), (style))

/* PEX_SetBFInteriorStyleIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*index;
 */
#define PEX_SetBFInteriorStyleIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), PEXOCBfInteriorStyleIndex, sizeof(pxlTableIndex), (index))

/* PEX_SetBFSurfaceColourIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*index;
 */
#define PEX_SetBFSurfaceColourIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), PEXOCBfSurfaceColourIndex, sizeof(pxlTableIndex), (index))

/* PEX_SetBFSurfaceColour
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlColourSpecifier	*colour;
 */
#define PEX_SetBFSurfaceColour(ocbuf, colour) \
    PEXAddOC ((ocbuf), PEXOCBfSurfaceColour, \
	sizeof(pxlColourSpecifier) - AdjustColourSize(colour->colourType), \
	(colour))

/* PEX_SetBFReflectionAttributes
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlReflectionAttr	*attr;
 */
#define PEX_SetBFReflectionAttributes(ocbuf, attr) \
    PEXAddOC ((ocbuf), PEXOCBfSurfaceReflAttr, \
	sizeof(pxlReflectionAttr) - \ 	
	AdjustColourSize(attr->specularColour.colourType), (attr))

/* PEX_SetBFReflectionModel
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlEnumTypeIndex	*model;
 */
#define PEX_SetBFReflectionModel(ocbuf, model) \
    PEXAddOC ((ocbuf), PEXOCBfSurfaceReflModel, sizeof(pxlEnumTypeIndex), (model))

/* PEX_SetBFSurfaceInterpMethod
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlEnumTypeIndex	*method;
 */
#define PEX_SetBFSurfaceInterpMethod(ocbuf, method) \
    PEXAddOC ((ocbuf), PEXOCBfSurfaceInterp, sizeof(pxlEnumTypeIndex), (method))

/* PEX_SetSurfaceApproxMethod
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlSurfaceApprox	*method;
 */
#define PEX_SetSurfaceApproxMethod(ocbuf, method) \
    PEXAddOC ((ocbuf), PEXOCSurfaceApproximation, sizeof(pxlSurfaceApprox), (method))

/* PEX_SetFacetCullingMode
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlCullMode		*mode;
 */
#define PEX_SetFacetCullingMode(ocbuf, mode) \
    PEXAddOC ((ocbuf), PEXOCCullingMode, sizeof(pxlCullMode), (mode))

/* PEX_SetFacetDistinguishFlag
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlSwitch		*flag;
 */
#define PEX_SetFacetDistinguishFlag(ocbuf, flag) \
    PEXAddOC ((ocbuf), OCDistinguish, sizeof(pxlSwitch), (flag))

/* PEX_SetPatternSize
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlCoord2D		*size;
 */
#define PEX_SetPatternSize(ocbuf, size) \
    PEXAddOC ((ocbuf), PEXOCPatternSize, sizeof(pxlCoord2D), (size))

/* PEX_SetPatternRefPt
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlCoord2D		*refPt;
 */
#define PEX_SetPatternRefPt(ocbuf, refPt) \
    PEXAddOC ((ocbuf), PEXOCPatternRefPt, sizeof(pxlCoord2D), (refPt))

/* PEX_SetPatternAttributes
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlPatternAttr	*attr;
 */
#define PEX_SetPatternAttributes(ocbuf, attr) \
    PEXAddOC ((ocbuf), PEXOCPatternAttr, sizeof(pxlPatternAttr), (attr))

/* PEX_SetInteriorBundleIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*index;
 */
#define PEX_SetInteriorBundleIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), PEXOCInteriorBundleIndex, sizeof(pxlTableIndex), (index))

/* PEX_SetSurfaceEdgeFlag
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlSwitch		*flag;
 */
#define PEX_SetSurfaceEdgeFlag(ocbuf, flag) \
    PEXAddOC ((ocbuf), PEXOCSurfaceEdgeFlag, sizeof(pxlSwitch), (flag))

/* PEX_SetSurfaceEdgeType
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlEnumTypeIndex	*type;
 */
#define PEX_SetSurfaceEdgeType(ocbuf, type) \
    PEXAddOC ((ocbuf), PEXOCSurfaceEdgeType, sizeof(pxlEnumTypeIndex), (type))

/* PEX_SetSurfaceEdgeWidth
 *
 *  pxlOCBuf		*ocbuf;
 *  float		*width;
 */
#define PEX_SetSurfaceEdgeWidth(ocbuf, width) \
    PEXAddOC ((ocbuf), PEXOCSurfaceEdgeWidth, sizeof(float), (width))

/* PEX_SetEdgeColourIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*index;
 */
#define PEX_SetEdgeColourIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), PEXOCSurfaceEdgeColourIndex, sizeof(pxlTableIndex), (index))

/* PEX_SetEdgeColour
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlColourSpecifier	*colour;
 */
#define PEX_SetEdgeColour(ocbuf, colour) \
    PEXAddOC ((ocbuf), PEXOCSurfaceEdgeColour, \
	sizeof(pxlColourSpecifier) - \
	AdjustColourSize(colour->colourType), (colour))

/* PEX_SetEdgeBundleIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*index;
 */
#define PEX_SetEdgeBundleIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), PEXOCEdgeBundleIndex, sizeof(pxlTableIndex), (index))

/* PEX_SetIndividualASF
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlAsfData		*asf;
 */
#define PEX_SetIndividualASF(ocbuf, asf) \
    PEXAddOC ((ocbuf), PEXOCSetAsfValues, sizeof(pxlAsfData), (asf))

/* PEX_SetLocalTransform
 *
 *  pxlOCBuf			*ocbuf;
 *  pxlLocalTransform3DData	*transform;
 */
#define PEX_SetLocalTransform(ocbuf, transform) \
    PEXAddOC ((ocbuf), PEXOCLocalTransform, \
	sizeof(pxlLocalTransform3DData), (transform))

/* PEX_SetLocalTransform2D
 *
 *  pxlOCBuf			*ocbuf;
 *  pxlLocalTransform2DData	*transform;
 */
#define PEX_SetLocalTransform2D(ocbuf, transform) \
    PEXAddOC ((ocbuf), PEXOCLocalTransform2D, \
	sizeof(pxlLocalTransform2DData), (transform))

/* PEX_SetGlobalTransform
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlMatrix		*transform;
 */
#define PEX_SetGlobalTransform(ocbuf, transform) \
    PEXAddOC ((ocbuf), PEXOCGlobalTransform, sizeof(pxlMatrix), (transform))

/* PEX_SetGlobalTransform2D
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlMatrix3X3	*transform;
 */
#define PEX_SetGlobalTransform2D(ocbuf, transform) \
    PEXAddOC ((ocbuf), PEXOCGlobalTransform2D, sizeof(pxlMatrix3X3), (transform))

/* PEX_SetModelClipFlag
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlSwitch		*flag;
 */
#define PEX_SetModelClipFlag(ocbuf, flag) \
    PEXAddOC ((ocbuf), PEXOCModelClip, sizeof(pxlSwitch), (flag))

/* No macro for PEX_SetModelClipVolume */
#define PEX_SetModelClipVolume(ocbuf, operator, halfSpaces, numHalfSpaces) \
	PEXSetModelClipVolume(ocbuf, operator, halfSpaces, numHalfSpaces)


/* No macro for PEX_SetModelClipVolume2D */
#define PEX_SetModelClipVolume2D (ocbuf, operator, halfSpaces, numHalfSpaces) \
	PEXSetModelClipVolume2D (ocbuf, operator, halfSpaces, numHalfSpaces)

/* PEX_RestoreModelClipVolume
 *
 *  pxlOCBuf		*ocbuf;
 */
#define PEX_RestoreModelClipVolume(ocbuf) \
    PEXAddOC ((ocbuf), PEXOCRestoreModelClip, 0, NULL)

/* PEX_SetViewIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*index;
 */
#define PEX_SetViewIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), PEXOCViewIndex, sizeof(pxlTableIndex), (index))

/* No macro for PEX_SetLightSourceState */
#define PEX_SetLightSourceState(ocbuf, enable, numEnable, disable, numDisable) \
	PEXSetLightSourceState (ocbuf, enable, numEnable, disable, numDisable)


/* PEX_SetDepthCueIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*index;
 */
#define PEX_SetDepthCueIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), PEXOCDepthCueIndex, sizeof(pxlTableIndex), (index))

/* PEX_SetRenderingColourModel
 *
 *  pxlOCBuf            *ocbuf;
 *  pxlEnumTypeIndex    *model;
 */
#define PEX_SetRenderingColourModel(ocbuf, model) \
    PEXAddOC ((ocbuf), PEXOCRenderingColourModel, sizeof(pxlEnumTypeIndex), (model))

/* No macro for PEX_SetParaSurfCharacteristics */
#define PEX_SetParaSurfCharacteristics (ocbuf, pscType, pscData) \
    PEXSetParaSurfCharacteristics (ocbuf, pscType, pscData) \


/* PEX_SetPickID
 *
 *  pxlOCBuf		*ocbuf;
 *  CARD32		*id;
 */
#define PEX_SetPickID(ocbuf, id) \
    PEXAddOC ((ocbuf), PEXOCPickId, sizeof(CARD32), (id))

/* PEX_SetHlhsrID
 *
 *  pxlOCBuf		*ocbuf;
 *  CARD32		*id;
 */
#define PEX_SetHlhsrID(ocbuf, id) \
    PEXAddOC ((ocbuf), PEXOCHlhsrIdentifier, sizeof(CARD32), (id))

/* PEX_SetColourApproxIndex
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlTableIndex	*index;
 */
#define PEX_SetColourApproxIndex (ocbuf, index) \
    PEXAddOC ((ocbuf), PEXOCColourApproxIndex, sizeof(pxlTableIndex), (index))

/* PEX_AddToNameSet
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlName		*names;
 *  int			numNames; 
 */
#define PEX_AddToNameSet(ocbuf, names, numNames) \
    PEXAddListOC( ocbuf, PEXOCAddToNameSet, False/* count needed*/, numNames, \
            sizeof( pexName), names);

/* PEX_RemoveFromNameSet
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlName		*names;
 *  int			numNames; 
 */
#define PEX_RemoveFromNameSet(ocbuf, names, numNames) \
    PEXAddListOC( ocbuf, PEXOCRemoveFromNameSet, False/* count needed*/, \
		numNames, sizeof( pexName), names);


/* PEX_ExecuteStructure
 *
 *  pxlOCBuf		*ocbuf;
 *  pxlStructure	*structure;
 */
#define PEX_ExecuteStructure(ocbuf, structure) \
    PEXAddOC ((ocbuf), PEXOCExecuteStructure, sizeof(pxlStructure), (structure))

/* PEX_Label
 *
 *  pxlOCBuf		*ocbuf;
 *  CARD32		*label;
 */
#define PEX_Label(ocbuf, label) \
    PEXAddOC ((ocbuf), PEXOCLabel, sizeof(CARD32), (label))

/* No macro for PEX_ApplicationData */


#endif /* PEXOC_H */
