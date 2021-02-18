/***********************************************************
Copyright 1987,1991 by Digital Equipment Corporation, Maynard, Massachusetts,
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


/*
**++
**  File Name:
**
**	pl_pc.c
**
**  Subsystem:
**
**	PEXlib
**
**  Version:
**
**	V1.0
**
**  Abstract:
**
**	This file contains the routines associated with PEX pipeline contexts.
**
**  Keywords:
**
**  Environment:
**
**	VMS and Ultrix
**
**
**  Creation Date:
**
**  Modification History:
**
**	18-Aug-88  
**
**--
*/

/*
 *   Table of Contents (routine names in order of appearance)
 */

/*
 * I   _PEXGeneratePCList
 * I   _PEXGeneratePCAttr
 * E   PEXCreatePipelineContext
 * E   PEXFreePipelineContext
 * E   PEXCopyPipelineContext
 * E   PEXGetPipelineContext
 * E   PEXChangePipelineContext
 */

/*
 *   Include files
 */

#include "pex_features.h"
#include "pex_macros.h"

#include "PEXlib.h"
#include "PEXlibint.h"

#include "pl_ref_data.h"

extern	char		*_XAllocScratch();


/*
 * Copy Colour Specifier Data. Note that pxlColour is a union.  This macro
 * only copies the valid colour data (ie: it uses the true colour length)
 * 30-Apr-91 AFV - Will also copy the pad word after the indexed colour
 */
#define PackColourSpecifier(srcBuf, dstBuf, sizeColour) \
{ \
    ((pxlColourSpecifier *)(dstBuf))->colourType = \
        ((pxlColourSpecifier *)(srcBuf))->colourType; \
    sizeColour = \
        ColourSizeFromType ( ((pxlColourSpecifier *)(srcBuf))->colourType ); \
    COPY_SMALL_AREA( &(((pxlColourSpecifier *)(srcBuf))->colour), \
        &(((pxlColourSpecifier *)(dstBuf))->colour), \
        sizeColour); \
}

/* routine to write a packed list of pc attributes into the transport buf */ 

INTERNAL void
_PEXGeneratePCList (display, req, valueMask, values)

INPUT Display             *display;
INPUT pexReq              *req;
INPUT unsigned long       *valueMask;
INPUT pxlPCAttributes	  *values;

{
    long		*pv;
    long		*pvSend;
    unsigned long       f;
    int                 i;
    int			j;
    int			n;
    int			length;
    long		size;
    Bool		bitSet;
    int			sizeColour;

    /* f is the maximum size we might need to store the PC list.
     * just use 2*sizeof(pxlPCAttributes) to account for padding between
     * shorts */
    f = 2 * sizeof(pxlPCAttributes);

    if (valueMask[1] & 1L << (PEXPCModelClipVolume - PEXPCBfInteriorStyleIndex))
	f += values->modelClipVolume.numHalfSpaces * sizeof(pexHalfSpace);

    if (valueMask[1] & 1L << (PEXPCLightState - PEXPCBfInteriorStyleIndex))
	f += values->lightState.numLights * sizeof(pexTableIndex);

    pv = pvSend = (long *)_XAllocScratch (display, (unsigned long)f);

    for (n = 0; n < (PEXMaxPCIndex + 1); n++)
    {
	bitSet = valueMask[n >> 5] & ( 1L << (n & 0x1f) );
	if (bitSet != 0)
        {
            switch (n)
	    {
	    /* note that there are 2 bytes of pad between 2 byte items */
            case PEXPCMarkerType:
		*((pexEnumTypeIndex *)pv) = values->markerType;
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCMarkerScale:
		*((FLOAT *)pv) = values->markerScale;
		pv += NUMWORDS( sizeof(FLOAT));
		break;
            case PEXPCMarkerColour:
		PackColourSpecifier(&(values->markerColour), pv, sizeColour);
		pv += NUMWORDS( sizeof(pexColourSpecifier) + sizeColour); 
		break;
            case PEXPCMarkerBundleIndex:
		*((pexTableIndex *)pv) = values->markerBundleIndex;
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
            case PEXPCTextFont:
		*((pexTableIndex *)pv) = values->textFont;
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
            case PEXPCTextPrecision:
		*((CARD16 *)pv) = values->textPrecision;
		pv += NUMWORDS( sizeof(CARD16));
		break;
            case PEXPCCharExpansion:
		*((FLOAT *)pv) = values->charExpansion;
		pv += NUMWORDS( sizeof(FLOAT));
		break;
            case PEXPCCharSpacing:
		*((FLOAT *)pv) = values->charSpacing;
		pv += NUMWORDS( sizeof(FLOAT));
		break;
            case PEXPCTextColour:
		PackColourSpecifier(&(values->textColour), pv, sizeColour);
		pv += NUMWORDS( sizeof(pexColourSpecifier) + sizeColour); 
		break;
            case PEXPCCharHeight:
		*((FLOAT *)pv) = values->charHeight;
		pv += NUMWORDS( sizeof(FLOAT));
		break;
            case PEXPCCharUpVector:
		*((pexVector2D *)pv) = values->charUpVector;
		pv += NUMWORDS( sizeof(pexVector2D)); 
		break;
            case PEXPCTextPath:
		*((CARD16 *)pv) = values->textPath;
		pv += NUMWORDS( sizeof(CARD16));
		break;
            case PEXPCTextAlignment:
		*((pexTextAlignmentData *)pv) = values->textAlignment;
		pv += NUMWORDS( sizeof(pexTextAlignmentData)); 
		break;
            case PEXPCAtextHeight:
		*((FLOAT *)pv) = values->atextHeight;
		pv += NUMWORDS( sizeof(FLOAT));
		break;
            case PEXPCAtextUpVector:
		*((pexVector2D *)pv) = values->atextUpVector;
		pv += NUMWORDS( sizeof(pexVector2D)); 
		break;
            case PEXPCAtextPath:
		*((CARD16 *)pv) = values->atextPath;
		pv += NUMWORDS( sizeof(CARD16));
		break;
            case PEXPCAtextAlignment:
		*((pexTextAlignmentData *)pv) = values->atextAlignment;
		pv += NUMWORDS( sizeof(pexTextAlignmentData)); 
		break;
            case PEXPCAtextStyle:
		*((pexEnumTypeIndex *)pv) = values->atextStyle;
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCTextBundleIndex:
		*((pexTableIndex *)pv) = values->textBundleIndex;
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
            case PEXPCLineType:
		*((pexEnumTypeIndex *)pv) = values->lineType;
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCLineWidth:
		*((FLOAT *)pv) = values->lineWidth;
		pv += NUMWORDS( sizeof(FLOAT));
		break;
            case PEXPCLineColour:
		PackColourSpecifier(&(values->lineColour), pv, sizeColour);
		pv += NUMWORDS( sizeof(pexColourSpecifier) + sizeColour); 
		break;
            case PEXPCCurveApproximation:
		*((pexCurveApprox *)pv) = values->curveApprox;
		pv += NUMWORDS( sizeof(pexCurveApprox)); 
		break;
            case PEXPCPolylineInterp:
		*((pexEnumTypeIndex *)pv) = values->polylineInterp;
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCLineBundleIndex:
		*((pexTableIndex *)pv) = values->lineBundleIndex;
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
            case PEXPCInteriorStyle:
		*((pexEnumTypeIndex *)pv) = values->interiorStyle;
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCInteriorStyleIndex:
		*((pexTypeOrTableIndex *)pv) = values->interiorStyleIndex;
		pv += NUMWORDS( sizeof(pexTypeOrTableIndex));
		break;
            case PEXPCSurfaceColour:
		PackColourSpecifier(&(values->surfaceColour), pv, sizeColour);
		pv += NUMWORDS( sizeof(pexColourSpecifier) + sizeColour); 
		break;
            case PEXPCSurfaceReflAttr:
		((pexReflectionAttr *)pv)->ambient = 
			values->reflectionAttr.ambient;
		((pexReflectionAttr *)pv)->diffuse = 
			values->reflectionAttr.diffuse;
		((pexReflectionAttr *)pv)->specular = 
			values->reflectionAttr.specular;
		((pexReflectionAttr *)pv)->specularConc = 
			values->reflectionAttr.specularConc;
		((pexReflectionAttr *)pv)->transmission = 
			values->reflectionAttr.transmission;
		PackColourSpecifier(&(values->reflectionAttr.specularColour), 
			&(((pexReflectionAttr *)pv)->specularColour), 
			sizeColour);
		pv += NUMWORDS( sizeof(pexReflectionAttr) + sizeColour); 
		break;
            case PEXPCSurfaceReflModel:
		*((pexEnumTypeIndex *)pv) = values->reflectionModel;
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCSurfaceInterp:
		*((pexEnumTypeIndex *)pv) = values->surfaceInterp;
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCBfInteriorStyle:
		*((pexEnumTypeIndex *)pv) = values->bfInteriorStyle;
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCBfInteriorStyleIndex:
		*((pexTypeOrTableIndex *)pv) = values->bfInteriorStyleIndex;
		pv += NUMWORDS( sizeof(pexTypeOrTableIndex));
		break;
            case PEXPCBfSurfaceColour:
		PackColourSpecifier(&(values->bfSurfaceColour), pv, sizeColour);
		pv += NUMWORDS( sizeof(pexColourSpecifier) + sizeColour); 
		break;
            case PEXPCBfSurfaceReflAttr:
		((pexReflectionAttr *)pv)->ambient = 
			values->bfReflectionAttr.ambient;
		((pexReflectionAttr *)pv)->diffuse = 
			values->bfReflectionAttr.diffuse;
		((pexReflectionAttr *)pv)->specular = 
			values->bfReflectionAttr.specular;
		((pexReflectionAttr *)pv)->specularConc = 
			values->bfReflectionAttr.specularConc;
		((pexReflectionAttr *)pv)->transmission = 
			values->bfReflectionAttr.transmission;
		PackColourSpecifier(&(values->bfReflectionAttr.specularColour), 
			&(((pexReflectionAttr *)pv)->specularColour), 
			sizeColour);
		pv += NUMWORDS( sizeof(pexReflectionAttr) + sizeColour); 
		break;
            case PEXPCBfSurfaceReflModel:
		*((pexEnumTypeIndex *)pv) = values->bfReflectionModel;
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCBfSurfaceInterp:
		*((pexEnumTypeIndex *)pv) = values->bfSurfaceInterp;
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCSurfaceApproximation:
		*((pexSurfaceApprox *)pv) = values->surfaceApprox;
		pv += NUMWORDS( sizeof(pexSurfaceApprox));
		break;
            case PEXPCCullingMode:
		*((pexCullMode *)pv) = values->cullingMode;
		pv += NUMWORDS( sizeof(pexCullMode));
		break;
            case PEXPCDistinguishFlag:
		*((pexSwitch *)pv) = values->distinguish;
		pv += NUMWORDS( sizeof(pexSwitch));
		break;
            case PEXPCPatternSize:
		*((pexCoord2D *)pv) = values->patternSize;
		pv += NUMWORDS( sizeof(pexCoord2D));
		break;
            case PEXPCPatternRefPt:
		*((pexCoord3D *)pv) = values->patternRefPt;
		pv += NUMWORDS( sizeof(pexCoord3D));
		break;
            case PEXPCPatternRefVec1:
		*((pexVector3D *)pv) = values->patternRefVec1;
		pv += NUMWORDS( sizeof(pexVector3D));
		break;
            case PEXPCPatternRefVec2:
		*((pexVector3D *)pv) = values->patternRefVec2;
		pv += NUMWORDS( sizeof(pexVector3D));
		break;
            case PEXPCInteriorBundleIndex:
		*((pexTableIndex *)pv) = values->interiorBundleIndex;
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
            case PEXPCSurfaceEdgeFlag:
		*((pexEnumTypeIndex *)pv) = values->surfaceEdges;
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCSurfaceEdgeType:
		*((pexEnumTypeIndex *)pv) = values->surfaceEdgeType;
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCSurfaceEdgeWidth:
		*((FLOAT *)pv) = values->surfaceEdgeWidth;
		pv += NUMWORDS( sizeof(FLOAT));
		break;
            case PEXPCSurfaceEdgeColour:
		PackColourSpecifier(&(values->surfaceEdgeColour), pv, 
			sizeColour);
		pv += NUMWORDS( sizeof(pexColourSpecifier) + sizeColour); 
		break;
            case PEXPCEdgeBundleIndex:
		*((pexTableIndex *)pv) = values->edgeBundleIndex;
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
            case PEXPCLocalTransform:
		COPY_LARGE_AREA ((char *)values->localTransform, (char *)pv,
			sizeof(pexMatrix));
		pv += NUMWORDS( sizeof(pexMatrix));
		break;
            case PEXPCGlobalTransform:
		COPY_LARGE_AREA ((char *)values->globalTransform, (char *)pv,
			sizeof(pexMatrix));
		pv += NUMWORDS( sizeof(pexMatrix));
		break;
            case PEXPCModelClip:
		*((CARD16 *)pv) = values->modelClip;
		pv += NUMWORDS( sizeof(CARD16));
		break;
            case PEXPCModelClipVolume:
		size = values->modelClipVolume.numHalfSpaces;
		*((long *)pv) = size;
		pv++;
		size *= sizeof(pexHalfSpace);
		COPY_AREA ((char *)values->modelClipVolume.halfSpace,
			(char *)pv, size);
		pv += NUMWORDS( size);
		break;
            case PEXPCViewIndex:
		*((pexTableIndex *)pv) = values->viewIndex;
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
            case PEXPCLightState:
		size = values->lightState.numLights;
		*((long *)pv) = size;
		pv++;
		size *= sizeof(pexTableIndex);
		COPY_AREA ((char *)values->lightState.lightIndex,
			(char *)pv, size);
		pv += NUMWORDS( size);
		break;
            case PEXPCDepthCueIndex:
		*((pexTableIndex *)pv) = values->depthCueIndex;
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
            case PEXPCPickId:
		*((CARD32 *)pv) = values->pickId;
		pv++;
		break;
            case PEXPCHlhsrIdentifier:
		*((CARD32 *)pv) = values->HlhsrIdentifier;
		pv++;
		break;
            case PEXPCNameSet:
		*((pexNameSet *)pv) = values->nameSet;
		pv += NUMWORDS( sizeof(pexNameSet));
		break;
            case PEXPCSetAsfValues:
		*((unsigned long *)pv) = values->asfValues[0];
		pv++;
		*((unsigned long *)pv) = values->asfValues[1];
		pv++;
		break;
	    case PEXPCColourApproxIndex:
		*((pexTableIndex *)pv) = values->colourApproxIndex;
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
	    case PEXPCRenderingColourModel:
		*((pexEnumTypeIndex *)pv) = values->rdrColourModel;
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
	    case PEXPCParaSurfCharacteristics:
		((pxlPSCData *)pv)->characteristics = 
			values->psc.characteristics;
		((pxlPSCData *)pv)->length = values->psc.length;
		pv += NUMWORDS( sizeof(pxlPSCData) - sizeof(char *));
		COPY_AREA (values->psc.pscData, (char *)pv, values->psc.length);
		pv += NUMWORDS( values->psc.length);
		break;
	    }
	}
    }

    length = pv - pvSend;
    req->length += length;

    /* put the data in the X buffer */
    Data (display, (char *)pvSend, (length << 2));

    return;
}

/* Routine to fill in a pxlPCAttributes structure from the PC attributes
part of a Get PC reply */

INTERNAL void
_PEXGeneratePCAttr (display, pv, valueMask, ppca)

INPUT Display				*display;
INPUT unsigned long			*pv;
INPUT unsigned long			valueMask[3];
OUTPUT pxlPCAttributes			*ppca;

{
    int				i;
    int				n;
    Bool			bitSet;
    int				sizeColour;
    int				size;

    /* copy the pipeline context components to the buffer */
    for (n = 0; n < (PEXMaxPCIndex + 1); n++)
    {
	bitSet = valueMask[n >> 5] & ( 1L << (n & 0x1f) );

	if (bitSet != 0)
        {
            switch (n)
	    {
	    /* note:  2 bytes of pad between 2 byte items */
            case PEXPCMarkerType:
		ppca->markerType = *((pxlEnumTypeIndex *)pv);
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCMarkerScale:
		ppca->markerScale = *((FLOAT *)pv);
		pv += NUMWORDS( sizeof(FLOAT)); 
		break;
            case PEXPCMarkerColour:
		PackColourSpecifier(pv, &(ppca->markerColour), sizeColour);
		pv += NUMWORDS( sizeof(pexColourSpecifier) + sizeColour); 
		break;
            case PEXPCMarkerBundleIndex:
		ppca->markerBundleIndex = *((pxlTableIndex *)pv);
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
            case PEXPCTextFont:
		ppca->textFont = *((pxlTableIndex *)pv);
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
            case PEXPCTextPrecision:
		ppca->textPrecision = *((CARD16 *)pv);
		pv += NUMWORDS( sizeof(CARD16));
		break;
            case PEXPCCharExpansion:
	  	ppca->charExpansion = *((FLOAT *)pv);
		pv += NUMWORDS( sizeof(FLOAT)); 
		break;
            case PEXPCCharSpacing:
		ppca->charSpacing = *((FLOAT *)pv);
		pv += NUMWORDS( sizeof(FLOAT)); 
		break;
            case PEXPCTextColour:
		PackColourSpecifier(pv, &(ppca->textColour), sizeColour);
		pv += NUMWORDS( sizeof(pexColourSpecifier) + sizeColour); 
		break;
            case PEXPCCharHeight:
		ppca->charHeight = *((FLOAT *)pv);
		pv += NUMWORDS( sizeof(FLOAT)); 
		break;
            case PEXPCCharUpVector:
		ppca->charUpVector = *((pxlVector2D *)pv);
		pv += NUMWORDS( sizeof(pexVector2D)); 
		break;
            case PEXPCTextPath:
		ppca->textPath = *((CARD16 *)pv);
		pv += NUMWORDS( sizeof(CARD16));
		break;
            case PEXPCTextAlignment:
		ppca->textAlignment = *((pxlTextAlignmentData *)pv);
		pv += NUMWORDS( sizeof(pexTextAlignmentData));
		break;
            case PEXPCAtextHeight:
		ppca->atextHeight = *((FLOAT *)pv);
		pv += NUMWORDS( sizeof(FLOAT)); 
		break;
            case PEXPCAtextUpVector:
		ppca->atextUpVector = *((pxlVector2D *)pv);
		pv += NUMWORDS( sizeof(pexVector2D)); 
		break;
            case PEXPCAtextPath:
		ppca->atextPath = *((CARD16 *)pv);
		pv += NUMWORDS( sizeof(CARD16));
		break;
            case PEXPCAtextAlignment:
		ppca->atextAlignment = *((pxlTextAlignmentData *)pv);
		pv += NUMWORDS( sizeof(pexTextAlignmentData));
		break;
            case PEXPCAtextStyle:
		ppca->atextStyle = *((pxlEnumTypeIndex *)pv);
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCTextBundleIndex:
		ppca->textBundleIndex = *((pxlTableIndex *)pv);
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
            case PEXPCLineType:
		ppca->lineType = *((pxlEnumTypeIndex *)pv);
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCLineWidth:
		ppca->lineWidth = *((FLOAT *)pv);
		pv += NUMWORDS( sizeof(FLOAT)); 
		break;
            case PEXPCLineColour:
		PackColourSpecifier(pv, &(ppca->lineColour), sizeColour);
		pv += NUMWORDS( sizeof(pexColourSpecifier) + sizeColour); 
		break;
            case PEXPCCurveApproximation:
		ppca->curveApprox = *((pxlCurveApprox *)pv);
		pv += NUMWORDS( sizeof(pexCurveApprox)); 
		break;
            case PEXPCPolylineInterp:
		ppca->polylineInterp = *((pxlEnumTypeIndex *)pv);
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCLineBundleIndex:
		ppca->lineBundleIndex = *((pxlTableIndex *)pv);
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
            case PEXPCInteriorStyle:
		ppca->interiorStyle = *((pxlEnumTypeIndex *)pv);
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCInteriorStyleIndex:
		ppca->interiorStyleIndex = *((pxlTypeOrTableIndex *)pv);
		pv += NUMWORDS( sizeof(pexTypeOrTableIndex));
		break;
            case PEXPCSurfaceColour:
		PackColourSpecifier(pv, &(ppca->surfaceColour), sizeColour);
		pv += NUMWORDS( sizeof(pexColourSpecifier) + sizeColour); 
		break;
            case PEXPCSurfaceReflAttr:
		ppca->reflectionAttr.ambient = 
			((pexReflectionAttr *)pv)->ambient;
		ppca->reflectionAttr.diffuse = 
			((pexReflectionAttr *)pv)->diffuse;
		ppca->reflectionAttr.specular = 
			((pexReflectionAttr *)pv)->specular;
		ppca->reflectionAttr.specularConc = 
			((pexReflectionAttr *)pv)->specularConc;
		ppca->reflectionAttr.transmission = 
			((pexReflectionAttr *)pv)->transmission;
		PackColourSpecifier( 
			&(((pexReflectionAttr *)pv)->specularColour), 
			&(ppca->reflectionAttr.specularColour), 
			sizeColour);
		pv += NUMWORDS( sizeof(pexReflectionAttr) + sizeColour); 
		break;
            case PEXPCSurfaceReflModel:
		ppca->reflectionModel = *((pxlEnumTypeIndex *)pv);
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCSurfaceInterp:
		ppca->surfaceInterp = *((pxlEnumTypeIndex *)pv);
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCBfInteriorStyle:
		ppca->bfInteriorStyle = *((pxlEnumTypeIndex *)pv);
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCBfInteriorStyleIndex:
		ppca->bfInteriorStyleIndex = *((pxlTypeOrTableIndex *)pv);
		pv += NUMWORDS( sizeof(pexTypeOrTableIndex));
		break;
            case PEXPCBfSurfaceColour:
		PackColourSpecifier(pv, &(ppca->bfSurfaceColour), sizeColour);
		pv += NUMWORDS( sizeof(pexColourSpecifier) + sizeColour); 
		break;
            case PEXPCBfSurfaceReflAttr:
		ppca->bfReflectionAttr.ambient = 
			((pexReflectionAttr *)pv)->ambient;
		ppca->bfReflectionAttr.diffuse = 
			((pexReflectionAttr *)pv)->diffuse;
		ppca->bfReflectionAttr.specular = 
			((pexReflectionAttr *)pv)->specular;
		ppca->bfReflectionAttr.specularConc = 
			((pexReflectionAttr *)pv)->specularConc;
		ppca->bfReflectionAttr.transmission = 
			((pexReflectionAttr *)pv)->transmission;
		PackColourSpecifier( 
			&(((pexReflectionAttr *)pv)->specularColour), 
			&(ppca->bfReflectionAttr.specularColour), 
			sizeColour);
		pv += NUMWORDS( sizeof(pexReflectionAttr) + sizeColour); 
		break;
            case PEXPCBfSurfaceReflModel:
		ppca->bfReflectionModel = *((pxlEnumTypeIndex *)pv);
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCBfSurfaceInterp:
		ppca->bfSurfaceInterp = *((pxlEnumTypeIndex *)pv);
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCSurfaceApproximation:
		ppca->surfaceApprox = *((pxlSurfaceApprox *)pv);
		pv += NUMWORDS( sizeof(pexSurfaceApprox));
		break;
            case PEXPCCullingMode:
		ppca->cullingMode = *((pxlCullMode *)pv);
		pv += NUMWORDS( sizeof(pexCullMode));
		break;
            case PEXPCDistinguishFlag:
		ppca->distinguish = *((pexSwitch *)pv);
		pv += NUMWORDS( sizeof(pexSwitch));
		break;
            case PEXPCPatternSize:
		ppca->patternSize = *((pxlCoord2D *)pv);
		pv += NUMWORDS( sizeof(pexCoord2D));
		break;
            case PEXPCPatternRefPt:
		ppca->patternRefPt = *((pxlCoord3D *)pv);
		pv += NUMWORDS( sizeof(pexCoord3D));
		break;
            case PEXPCPatternRefVec1:
		ppca->patternRefVec1 = *((pxlVector3D *)pv);
		pv += NUMWORDS( sizeof(pexVector3D)); 
		break;
            case PEXPCPatternRefVec2:
		ppca->patternRefVec2 = *((pxlVector3D *)pv);
		pv += NUMWORDS( sizeof(pexVector3D)); 
		break;
            case PEXPCInteriorBundleIndex:
		ppca->interiorBundleIndex = *((pxlTableIndex *)pv);
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
            case PEXPCSurfaceEdgeFlag:
		ppca->surfaceEdges = *((CARD16 *)pv);
		pv += NUMWORDS( sizeof(CARD16));
		break;
            case PEXPCSurfaceEdgeType:
		ppca->surfaceEdgeType = *((pxlEnumTypeIndex *)pv);
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
            case PEXPCSurfaceEdgeWidth:
		ppca->surfaceEdgeWidth = *((FLOAT *)pv);
		pv += NUMWORDS( sizeof(FLOAT)); 
		break;
            case PEXPCSurfaceEdgeColour:
		PackColourSpecifier(pv, &(ppca->surfaceEdgeColour), sizeColour);
		pv += NUMWORDS( sizeof(pexColourSpecifier) + sizeColour); 
		break;
            case PEXPCEdgeBundleIndex:
		ppca->edgeBundleIndex = *((pxlTableIndex *)pv);
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
            case PEXPCLocalTransform:
		COPY_LARGE_AREA ((char *)pv, (char *)ppca->localTransform,
			sizeof(pxlMatrix));
		pv += NUMWORDS( sizeof(pexMatrix));
		break;
            case PEXPCGlobalTransform:
		COPY_LARGE_AREA ((char *)pv, (char *)ppca->globalTransform,
			sizeof(pxlMatrix));
		pv += NUMWORDS( sizeof(pexMatrix));
		break;
            case PEXPCModelClip:
		ppca->modelClip = *((CARD16 *)pv);
		pv += NUMWORDS( sizeof(CARD16));
		break;
            case PEXPCModelClipVolume:
		size = *((int *)pv);
		pv++;
		ppca->modelClipVolume.numHalfSpaces = size;
		size *= sizeof(pxlHalfSpace);
		ppca->modelClipVolume.halfSpace =
			(pxlHalfSpace *)PEXAllocBuf ((unsigned)size);
		COPY_AREA ((char *)pv,
			(char *)(ppca->modelClipVolume.halfSpace), size);
		pv += NUMWORDS( size);
		break;
            case PEXPCViewIndex:
		ppca->viewIndex = *((pxlTableIndex *)pv);
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
            case PEXPCLightState:
		size = *((int *)pv);
		pv++;
		ppca->lightState.numLights = size;
		size *= sizeof(pxlTableIndex);
		ppca->lightState.lightIndex =
			(pxlTableIndex *)PEXAllocBuf ((unsigned)size);
		COPY_AREA ((char *)pv, (char *)(ppca->lightState.lightIndex),
			size);
		pv += NUMWORDS( size);
		break;
            case PEXPCDepthCueIndex:
		ppca->depthCueIndex = *((pxlTableIndex *)pv);
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
            case PEXPCPickId:
		ppca->pickId = *((INT32 *)pv);
		pv++;
		break;
            case PEXPCHlhsrIdentifier:
		ppca->HlhsrIdentifier = *((CARD32 *)pv);
		pv++;
		break;
            case PEXPCNameSet:
		ppca->nameSet = *((pxlNameSet *)pv);
		pv += NUMWORDS( sizeof(pexNameSet));
		break;
            case PEXPCSetAsfValues:
		ppca->asfValues[0] = *((CARD32 *)pv);
		pv++;
		ppca->asfValues[1] = *((CARD32 *)pv);
		pv++;
		break;
	    case PEXPCColourApproxIndex:
		ppca->colourApproxIndex = *((pxlTableIndex *)pv);
		pv += NUMWORDS( sizeof(pexTableIndex));
		break;
	    case PEXPCRenderingColourModel:
		ppca->rdrColourModel = *((pxlEnumTypeIndex *)pv);
		pv += NUMWORDS( sizeof(pexEnumTypeIndex));
		break;
	    case PEXPCParaSurfCharacteristics:
		ppca->psc.characteristics = ((pxlPSCData *)pv)->characteristics;
		ppca->psc.length = size = ((pxlPSCData *)pv)->length;
		pv += NUMWORDS( sizeof(pxlPSCData) - sizeof(char *));;
		ppca->psc.pscData = (char *) PEXAllocBuf (size);
		COPY_AREA ((char *)pv, (char *)(ppca->psc.pscData), size);
		pv += NUMWORDS( size); 
		break;
	    }
	}
    }

    return;
}

/*+
 * NAME:
 * 	PEXCreatePipelineContext
 *
 * FORMAT:
 * 	`PEXCreatePipelineContext`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	valueMask	A pointer to an array of two unsigned longwords
 *			used as a bitmask array.
 *
 *	values		A pointer to a structure containing values used
 *			to override default values in the new pipeline
 *			context.
 *
 * RETURNS:
 *	The resource ID of the newly-created pipeline context.
 *
 * DESCRIPTION:
 *	This routine creates a pipeline context and returns its resource ID.
 *	Each bit in the bitmask array `valueMask'
 *	corresponds to a pipeline context attribute.  If the bit is set,
 *	the value in the new pipeline context is set to the value specified
 *	in `values'.  Attributes corresponding to clear bits in `valueMask'
 *	will be initialized to the default values.
 *	The macro `PEX_SetPCAttrMaskBit" can be used
 *	to set bits within the two-word bitmask using the constant values
 *	defined in the `PEX.h' include file.
 *
 * ERRORS:
 *	IDChoice		ID already in use or not in range assigned to
 *				client.
 *
 *	Value			An item in the item list is out of range.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	ColourType		Device does not support the specified colour type.
 *
 *	Alloc			Server failed to allocate the resource.
 *
 * SEE ALSO:
 *	`PEXFreePipelineContext",  `PEXCopyPipelineContext", `PEXGetPipelineContext",
 *	`PEXChangePipelineContext"
 *
 */

pxlPipelineContext
PEXCreatePipelineContext (display, valueMask, values)

INPUT Display		*display;
INPUT unsigned long	valueMask[3];
INPUT pxlPCAttributes	*values;

{
/* AUTHOR: */
    pexCreatePipelineContextReq		*req;
    pxlPipelineContext			pc;

    /* get a pipeline context resource id from X */
    pc = XAllocID (display);

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (CreatePipelineContext, req);
    req->fpFormat = (pexEnumTypeIndex)pexFpFormat;
    req->pc = pc;
    req->itemMask[0] = valueMask[0];
    req->itemMask[1] = valueMask[1];
    req->itemMask[2] = valueMask[2];

    if (valueMask[0] != 0 || valueMask[1] != 0 || valueMask[2] != 0)
    {
	_PEXGeneratePCList (display, (pexReq *)req, valueMask, values);
    }

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (pc);
}

/*+
 * NAME:
 * 	PEXFreePipelineContext
 *
 * FORMAT:
 * 	`PEXFreePipelineContext`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful call to `XOpenDisplay".
 *
 *	pc		The resource ID of the pipeline context to be freed.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine deletes the association between the pipeline context
 *	Resource ID `pc' and the pipeline context.  The pipeline context
 *	storage is freed when no other resource references it.
 *
 * ERRORS:
 *	PipelineContext		Pipeline context resource ID is invalid.
 *
 * SEE ALSO:
 *	`PEXCreatePipelineContext",  `PEXCopyPipelineContext", `PEXGetPipelineContext",
 *	`PEXChangePipelineContext"
 *
 */

void
PEXFreePipelineContext (display, pc)

INPUT Display			*display;
INPUT pxlPipelineContext	pc;

{
/* AUTHOR: */
    pexFreePipelineContextReq		*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (FreePipelineContext, req);
    req->id = pc;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXCopyPipelineContext
 *
 * FORMAT:
 * 	`PEXCopyPipelineContext`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	valueMask	A pointer to an array of two unsigned longwords
 *			to be used as a bitmask array.
 *
 *	srcPc		The resource ID of the pipeline context from which
 *			attributes are to be copied.
 *
 *	destPc		The resource ID of the pipeline context to which
 *			attributes are to be copied.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine copies attributes from the source pipeline context
 *	`srcPc' to the destination pipeline context `destPc'.  Both must
 *	already exist as valid pipeline context resources.  Attributes
 *	indicated by ones in the bitmask array `valueMask' are copied and
 *	the remainder of the attributes are left as they were.  The macro
 * 	`PEX_SetPCAttrMaskBit" can be used to set bits within the two-word
 *	bitmask using the constant values for the pipeline context attributes.
 *	Include file `PEX.h' contains definitions of these constant values.
 *
 * ERRORS:
 *	PipelineContext		Pipeline context resource ID is invalid.
 *
 * SEE ALSO:
 *	`PEXCreatePipelineContext", `PEXFreePipelineContext",  `PEXGetPipelineContext",
 *	`PEXChangePipelineContext"
 *
 */

void
PEXCopyPipelineContext (display, valueMask, srcPc, destPc)

INPUT Display			*display;
INPUT unsigned long		valueMask[3];
INPUT pxlPipelineContext	srcPc;
INPUT pxlPipelineContext	destPc;

{
/* AUTHOR: */
    pexCopyPipelineContextReq		*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (CopyPipelineContext, req);
    req->src = srcPc;
    req->dst = destPc;
    req->itemMask[0] = valueMask[0];
    req->itemMask[1] = valueMask[1];
    req->itemMask[2] = valueMask[2];

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXGetPipelineContext
 *
 * FORMAT:
 * 	`PEXGetPipelineContext`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	pc		The resource ID of the pipeline context whose
 *			attributes are to be queried.
 *
 *	valueMask	A pointer to an array of two unsigned longwords
 *			to be used as a bitmask array.
 *
 *	pcAttributesReturn
 *			Returns a pointer to a structure containing
 *			the pipeline context values returned
 *			from pipeline context `pc'.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *     	This routine returns the requested attributes of the pipeline context
 * 	`pc'.  Each bit in the bitmask array `valueMask'
 *	corresponds to a pipeline context attribute.  If the bit is set,
 *	the attribute value in the pipeline context is returned in the
 *	structure pointed at by `pcAttributesReturn'.
 *	The macro `PEX_SetPCAttrMaskBit" can be used
 *	to set bits within the two-word bitmask using the constant values
 *	defined in `PEX.h'.
 *
 *	Returned values are placed in a data structure whose storage has
 *	been allocated by PEXlib.  Deallocate this storage with the
 *	`PEXFree"(`*pcAttributesReturn') routine.
 *
 * ERRORS:
 *	PipelineContext		Pipeline context resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 * SEE ALSO:
 *	`PEXCreatePipelineContext", `PEXFreePipelineContext",  `PEXCopyPipelineContext",
 *	`PEXChangePipelineContext"
 *
 */

Status
PEXGetPipelineContext (display, pc, valueMask, pcAttributesReturn)

INPUT Display			*display;
INPUT pxlPipelineContext	pc;
INPUT unsigned long		valueMask[3];
OUTPUT pxlPCAttributes		**pcAttributesReturn;

{
/* AUTHOR: */
    pexGetPipelineContextReq	*req;
    pexGetPipelineContextReply	rep;
    unsigned long		*pv;
    pxlPCAttributes		*ppca;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (GetPipelineContext, req);
    req->fpFormat = (pexEnumTypeIndex)pexFpFormat;
    req->pc = pc;
    req->itemMask[0] = valueMask[0];
    req->itemMask[1] = valueMask[1];
    req->itemMask[2] = valueMask[2];

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
	UnlockDisplay (display);
	PEXSyncHandle ();
	return (1);         /* return an error */
    }

    /* allocate a buffer for the replies to pass back to the client */
    pv = (unsigned long *)_XAllocScratch (display,
	(unsigned long)(rep.length << 2));
    _XRead (display, (char *)pv, (long)(rep.length << 2));

    ppca = (pxlPCAttributes *)
	PEXAllocBuf ((unsigned)(sizeof(PEXHeader) + sizeof(pxlPCAttributes)));
    ((PEXHeader *)ppca)->free = _PEXFreePCAttr; /* routine to call to free it */
    ppca = (pxlPCAttributes *)&(((PEXHeader *)ppca)[1]);

    *pcAttributesReturn = ppca;

    _PEXGeneratePCAttr (display, pv, valueMask, ppca);

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	PEXChangePipelineContext
 *
 * FORMAT:
 * 	`PEXChangePipelineContext`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *	 		returned by a successful  `XOpenDisplay" call.
 *
 *	pc		The resource ID of the pipeline context to be changed.
 *
 *	valueMask	A pointer to an array of two unsigned longwords
 *			to be used as a bitmask array.
 *
 *	pcAttributes	A pointer to a structure containing the new
 *			values for attributes in the pipeline context `pc'.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine changes components of the pipeline context specifed
 *	by `pc' to the values stored in the structure `pcAttributes'.  Each
 *	bit in the bitmask array `valueMask' corresponds to a pipeline
 *	context attribute.  If the bit is set, the value in the pipeline
 *	context is changed to the value specified in `pcAttributes'.  The
 *	macro `PEX_SetPCAttrMaskBit" can be used to set bits within the
 *	two-word bitmask.  This macro uses the constant values for the
 *	pipeline context attributes defined in include file `PEX.h'. Note that
 *	this macro cannot be used with the constant `PEXPCAllValues'.  Include
 *	file `PEXlib.h' contains definitions of this macro.  The format for
 *	`PEX_SetPCAttrMaskBit" is as follows:
 *
 *	==
 *	PEX_SetPCAttrMaskBit (mask, attrNum)
 *	!=
 *
 *	where `mask' is an array of two unsigned longwords and `attrNum' is
 *	a pipeline context attribute value.
 *
 * ERRORS:
 *	PipelineContext		Pipeline context resource ID is invalid.
 *
 *	Value			An item in the item list is out of range.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	ColourType		Device does not support the specified colour type.
 *
 * SEE ALSO:
 *	`PEXCreatePipelineContext", `PEXFreePipelineContext",  `PEXCopyPipelineContext",
 *	`PEXGetPipelineContext"
 *
 */

void
PEXChangePipelineContext (display, pc, valueMask, pcAttributes)

INPUT Display			*display;
INPUT pxlPipelineContext	pc;
INPUT unsigned long		valueMask[3];
INPUT pxlPCAttributes		*pcAttributes;

{
/* AUTHOR: */
    pexChangePipelineContextReq		*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (ChangePipelineContext, req);
    req->fpFormat = (pexEnumTypeIndex)pexFpFormat;
    req->pc = pc;
    req->itemMask[0] = valueMask[0];
    req->itemMask[1] = valueMask[1];
    req->itemMask[2] = valueMask[2];

    _PEXGeneratePCList (display, (pexReq *)req, valueMask, pcAttributes);

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}
