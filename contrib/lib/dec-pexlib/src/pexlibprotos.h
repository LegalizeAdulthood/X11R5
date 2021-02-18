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


/*
 *
 *  File Name:
 *     pexlibprotos.h
 *
 *  Subsystem:
 *     pex
 *
 *  Version: 
 *     0.1
 *
 *  Abstract:
 *     Define prototypes for all external pexlib routines
 *
 *  Keywords:
 *     pex, pexlib
 *
 *  Environment:
 *     included by pexlib.h if PROTOS is defined
 *
 *
 *  Creation Date:
 *     13-Mar-1991
 *
 *  Modification History:
 */

#ifndef PEXLIBPROTOS_DEFINED
#define PEXLIBPROTOS_DEFINED

#include <stdio.h>

#ifndef PEXPROTO_H
#   include "PEXproto.h"
#endif /* PEXPROTO_H */

#ifdef MPEX
#   ifndef MPEXLIB_H
#	include "MPEXlib.h"
#   endif
#endif


/*
 *  Shared typedefs, macros and constants
 */

#ifndef ARGS
#   define ARGS(_args)	_args
#   define NO_ARGS	(void)
#   define INPUT	/* nothing */
#   define OUTPUT	/* nothing */
#   define INPUT_OUTPUT	/* nothing */
#   define MUST_CLEAN_UP    1	    /* must clean up after myself */
#endif

/* 
 * Routines defined in pl_oc_buff.c
 */

extern pxlOCBuf * PEXAllocateRetainedOCBuffer
    ARGS ((
	INPUT Display		* display,
	INPUT int		type,
	INPUT XID		target,
	INPUT void		(* errorFunction)(),
	INPUT unsigned int	initSize
	));

extern pxlOCBuf * PEXAllocateTransientOCBuffer
    ARGS ((
	INPUT Display		* display,
	INPUT int		type,
	INPUT XID		target,
	INPUT void		(* errorFunction)(),
	INPUT int		initSize
	));

extern void PEXGetDefaultOCBufferType
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	OUTPUT Display          ** displayReturn,
	OUTPUT int              * typeReturn,
	OUTPUT XID              * targetReturn
	));

extern unsigned int * PEXGetWordsError
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int               nWords
	));

extern void PEXCopyBytesError
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		nBytes,
	INPUT char		* buffer
	));
			
extern void PEXInitOC
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		ocType,
	INPUT int             	ocHeaderLength,
	INPUT int             	ocDataLength,
	INPUT char            	** pReq
	));

extern void PEXCopyBytesToOC
    ARGS ((
	INPUT_OUTPUT pxlOCBuf   * ocbuf,
	INPUT int		nBytes,
	INPUT char		* OCData
	));

extern void PEXCopyWordsToOC
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		nWords,
	INPUT char		* OCData
	));

#ifdef DEBUG

extern unsigned int * PEXGetOCWords
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		nWords
	));

extern void PEXAddOC
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT unsigned int	ocType,
	INPUT int		size,
	INPUT char		* OCData
	));

extern void PEXAddListOC
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT unsigned int	ocType,
	INPUT int		countNeeded,
	INPUT int		count,
	INPUT int		elementSize,
	INPUT char		* elementList
	));
#endif /* DEBUG */


extern void PEXDefaultStoreOCList
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		numElements,
	INPUT pxlElementInfo	* ocList
	));

extern int PEXDefaultTransientOCBuffer NO_ARGS;

extern int PEXDefaultRetainedOCBuffer NO_ARGS;

extern void PEXDefaultOCError 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		value
	));

extern void PEXSetOCOverflow 
    ARGS ((
	INPUT int		value
	));

extern int PEXGetOCOverflow NO_ARGS;

extern void PEXClearOCOverflow NO_ARGS;

/* 
 * Routines defined in pl_oc_attr.c
 */	

extern void PEXSetMarkerType 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		type
	));

extern void PEXSetMarkerScale 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT double		scale
	));

extern void PEXSetMarkerColourIndex 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		index
	));

extern void PEXSetMarkerColour 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlColour		* colour
	));
	
extern void PEXSetMarkerBundleIndex
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		index
	));

extern void PEXSetTextFontIndex
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		index
	));

extern void PEXSetTextPrecision 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		precision
	));
	
extern void PEXSetCharExpansion
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT double		expansion
	));

extern void PEXSetCharSpacing 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT double		spacing
	));

extern void PEXSetTextColourIndex 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		index
	));

extern void PEXSetTextColour 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlColour		* colour
	));

extern void PEXSetCharHeight 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT double		height
	));

extern void PEXSetCharUpVector 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT double		x,
	INPUT double		y
	));

extern void PEXSetTextPath 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		path
	));

extern void PEXSetTextAlignment 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		halignment,
	INPUT int		valignment
	));

extern void PEXSetATextHeight 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT double		height
	));

extern void PEXSetATextUpVector 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT double		x,
	INPUT double		y
	));

extern void PEXSetATextPath 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		path
	));

extern void PEXSetATextAlignment 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		halignment,
	INPUT int		valignment
	));

extern void PEXSetATextStyle 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		style
	));

extern void PEXSetTextBundleIndex 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		index
	));

extern void PEXSetLineType 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		type
	));

extern void PEXSetLineWidth 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT double		width
	));

extern void PEXSetLineColourIndex 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		index
	));

extern void PEXSetLineColour 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlColour		* colour
	));

extern void PEXSetCurveApproxMethod 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		approxMethod,
	INPUT double		tolerance
	));

extern void PEXSetPolylineInterpMethod 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		method
	));

extern void PEXSetLineBundleIndex 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		index
	));

extern void PEXSetInteriorStyle 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		style
	));

extern void PEXSetInteriorStyleIndex 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		index
	));

extern void PEXSetSurfaceColourIndex 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		index
	));

extern void PEXSetSurfaceColour 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlColour		* colour
	));

extern void PEXSetReflectionAttributes 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlReflectionAttr	* reflectionAttr
	));

extern void PEXSetReflectionModel 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		model
	));

extern void PEXSetSurfaceInterpMethod 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		method
	));

extern void PEXSetBFInteriorStyle 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		style
	));

extern void PEXSetBFInteriorStyleIndex 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		index
	));

extern void PEXSetBFSurfaceColourIndex 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		index
	));

extern void PEXSetBFSurfaceColour 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlColour		* colour
	));

extern void PEXSetBFReflectionAttributes 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlReflectionAttr	* reflectionAttr
	));

extern void PEXSetBFReflectionModel 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		model
	));

extern void PEXSetBFSurfaceInterpMethod 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		method
	));

extern void PEXSetSurfaceApproxMethod 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		approxMethod,
	INPUT double		sTolerance,
	INPUT double		tTolerance
	));

extern void PEXSetFacetCullingMode 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		mode
	));

extern void PEXSetFacetDistinguishFlag 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		flag
	));

extern void PEXSetPatternSize 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT double		width,
	INPUT double		height
	));

extern void PEXSetPatternRefPt 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT double		x,
	INPUT double		y
	));

extern void PEXSetPatternAttributes 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlCoord3D	* refPt,
	INPUT pxlVector3D	* vec1,
	INPUT pxlVector3D	* vec2
	));

extern void PEXSetInteriorBundleIndex 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		index
	));

extern void PEXSetSurfaceEdgeFlag 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		flag
	));

extern void PEXSetSurfaceEdgeType 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		type
	));

extern void PEXSetSurfaceEdgeWidth 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT double		width
	));

extern void PEXSetSurfaceEdgeColourIndex 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		index
	));

extern void PEXSetSurfaceEdgeColour 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlColour		* colour
	));

extern void PEXSetEdgeBundleIndex 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		index
	));

extern void PEXSetIndividualASF 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		attribute,
	INPUT int		value
	));

extern void PEXSetLocalTransform 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf		* ocbuf,
	INPUT pxlLocalTransform3DData	* transform
	));

extern void PEXSetLocalTransform2D 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf		* ocbuf,
	INPUT pxlLocalTransform2DData	* transform
	));

extern void PEXSetGlobalTransform 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlMatrix		transform
	));

extern void PEXSetGlobalTransform2D 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlMatrix3X3	transform
	));

extern void PEXSetModelClipFlag 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		flag
	));

extern void PEXSetModelClipVolume 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		operator,
	INPUT pxlHalfSpace	* halfSpaces,
	INPUT int		numHalfSpaces
	));

extern void PEXSetModelClipVolume2D 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		operator,
	INPUT pxlHalfSpace2D	* halfSpaces,
	INPUT int		numHalfSpaces
	));

extern void PEXRestoreModelClipVolume 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf
	));

extern void PEXSetViewIndex 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		index
	));

extern void PEXSetLightSourceState 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlTableIndex	* enable,
	INPUT int		numEnable,
	INPUT pxlTableIndex	* disable,
	INPUT int		numDisable
	));

extern void PEXSetDepthCueIndex 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		index
	));

extern void PEXSetRenderingColourModel
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		model
	));

extern void PEXSetParaSurfCharacteristics
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int	 	pscType,
	INPUT char		* pscData	
	));

extern void PEXSetPickID 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT unsigned long	id
	));

extern void PEXSetHlhsrID 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT unsigned long	id
	));

extern void PEXAddToNameSet 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlName		* names,
	INPUT int		numNames
	));

extern void PEXRemoveFromNameSet 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlName		* names,
	INPUT int		numNames
	));
/* 
 * Routines defined in pl_oc_prim.c
 */

extern void PEXMarkers 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlCoord3D	* points,
	INPUT int		numPoints
	));

extern void PEXMarkers2D 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlCoord2D	* pts,
	INPUT int		numPts
	));

extern void PEXText 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlCoord3D	* origin,
	INPUT pxlVector3D	* vec1,
	INPUT pxlVector3D	* vec2,
	INPUT char		* string,
	INPUT int		count
	));

extern void PEXText2D 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlCoord2D	* origin,
	INPUT char		* string,
	INPUT int		count
	));

extern void PEXAnnotationText 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlCoord3D	* origin,
	INPUT pxlCoord3D	* offset,
	INPUT char		* string,
	INPUT int		count
	));

extern void PEXAnnotationText2D 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlCoord2D	* origin,
	INPUT pxlCoord2D	* offset,
	INPUT char		* string,
	INPUT int		count
	));

extern void PEXMonoEncodedText
    ARGS ((
	INPUT pxlOCBuf                  * ocbuf,
	INPUT pxlCoord3D                * origin,
	INPUT pxlVector3D               * vec1,
	INPUT pxlVector3D               * vec2,
	INPUT int                       numEncodings,
	INPUT pxlMonoEncodedTextData    * encodedTextList
	));

extern void PEXMonoEncodedText2D
    ARGS ((
	INPUT pxlOCBuf                  * ocbuf,
	INPUT pxlCoord2D                * origin,
	INPUT int                       numEncodings,
	INPUT pxlMonoEncodedTextData    * encodedTextList
	));

extern void PEXMonoEncodedAnnoText
    ARGS ((
	INPUT pxlOCBuf                  * ocbuf,
	INPUT pxlCoord3D                * origin,
	INPUT pxlCoord3D                * offset,
	INPUT int                       numEncodings,
	INPUT pxlMonoEncodedTextData    * encodedTextList
	));

extern void PEXMonoEncodedAnnoText2D
    ARGS ((
	INPUT pxlOCBuf                  * ocbuf,
	INPUT pxlCoord2D                * origin,
	INPUT pxlCoord2D                * offset,
	INPUT int                       numEncodings,
	INPUT pxlMonoEncodedTextData    * encodedTextList
	));

extern void PEXPolyline 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlCoord3D	* vertices,
	INPUT int		numVertices
	));

extern void PEXPolyline2D 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlCoord2D	* vertices,
	INPUT int		numVertices
        ));

extern void PEXPolylineSet 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT unsigned long	vertexAttributes,
	INPUT pxlPolylineData	* polylines,
	INPUT int		numPolylines
	));

extern void PEXNurbCurve 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		order,
	INPUT int		type,
	INPUT double		tmin,
	INPUT double		tmax,
	INPUT float		* knots,
	INPUT char		* points,
	INPUT int		numPoints
	));

extern void PEXFillArea 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		shape,
	INPUT int		ignoreEdges,
	INPUT pxlCoord3D	* points,
	INPUT int		numPoints
	));

extern void PEXFillArea2D 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		shape,
	INPUT int		ignoreEdges,
	INPUT pxlCoord2D	* points,
	INPUT int		numPoints
	));

extern void PEXMultiFillAreaData 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		shape,
	INPUT int		ignoreEdges,
	INPUT unsigned long	facetAttributes,
	INPUT unsigned long	vertexAttributes,
	INPUT pxlPolygonData	* polygons,
	INPUT int		numPolygons
	));

extern void PEXFillAreaSet 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		shape,
	INPUT int		ignoreEdges,
	INPUT int		contourHint,
	INPUT pxlPolygonData	* polygons,
	INPUT int		numPolygons
	));

extern void PEXFillAreaSet2D 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		shape,
	INPUT int		ignoreEdges,
	INPUT int		contourHint,
	INPUT pxlPolygonData	* polygons,
	INPUT int		numPolygons
	));

extern void PEXMultiFillAreaSetData 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		shape,
	INPUT int		ignoreEdges,
	INPUT int		contourHint,
	INPUT unsigned long	facetAttributes,
	INPUT unsigned long	vertexAttributes,
	INPUT pxlComplexPolygonData	* complexPolygons,
	INPUT int		numComplexPolygons
	));

extern void PEXSetOfFillAreaSets
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		shape,
	INPUT int               contourHint,
	INPUT int               contoursAllOne,
	INPUT unsigned long     FASAttributes,
	INPUT unsigned long     vertexAttributes,
	INPUT unsigned long     edgeAttributes,
	INPUT int               numFAS,
	INPUT int               numVertices,
	INPUT int               numEdges,
	INPUT int               numContours,
	INPUT char              *FASOptDataList,
	INPUT char              *verticeList,
	INPUT char              *edgeList,
	INPUT char		*FASList
	));

extern void PEXTriangleStrip 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT unsigned long	facetAttributes,
	INPUT unsigned long	vertexAttributes,
	INPUT char		* facetData,
	INPUT char		* points,
	INPUT int		numPoints
	));

extern void PEXQuadMesh 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		shape,
	INPUT unsigned long	facetAttributes,
	INPUT unsigned long	vertexAttributes,
	INPUT char		* facetData,
	INPUT char		* points,
	INPUT int		numPointsM,
	INPUT int		numPointsN
	));

extern void MPEXIndexedPolygons 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		shape,
	INPUT unsigned long	facetAttributes,
	INPUT unsigned long	edgeAttributes,
	INPUT unsigned long	vertexAttributes,
	INPUT int		numFacets,
	INPUT int		numEdges,
	INPUT int		numVertices,
	INPUT unsigned short 	* facetCounts,
	INPUT unsigned char 	* facetOptData,
	INPUT unsigned char 	* edges,
	INPUT unsigned char 	* vertices
	));

extern void PEXNurbSurface 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlNurbSurface	* nurb
	));

extern void PEXCellArray 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlCoord3D	* pt1,
	INPUT pxlCoord3D	* pt2,
	INPUT pxlCoord3D	* pt3,
	INPUT int		dx,
	INPUT int		dy,
	INPUT pxlIndexedColour 	* icolours
	));

extern void PEXCellArray2D 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlCoord2D	* pt1,
	INPUT pxlCoord2D	* pt2,
	INPUT int		dx,
	INPUT int		dy,
	INPUT pxlIndexedColour	* icolours
	));

extern void PEXExtendedCellArray 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlCoord3D	* pt1,
	INPUT pxlCoord3D	* pt2,
	INPUT pxlCoord3D	* pt3,
	INPUT int		dx,
	INPUT int		dy,
	INPUT char	 	* colours
	));

extern void PEXGdp 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		id,
	INPUT pxlCoord3D	* points,
	INPUT int		numPoints,
	INPUT char		* data,
	INPUT int		numBytes
	));

extern void PEXGdp2D 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT int		id,
	INPUT pxlCoord2D	* points,
	INPUT int		numPoints,
	INPUT char		* data,
	INPUT int		numBytes
	));
/* 
 * Routines defined in pl_oc_phigs.c
 */

extern void PEXPolylineSetUnpacked 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf   *ocbuf,
	INPUT unsigned long     vertexAttributes,
	INPUT int               colourType,
	INPUT pxlCoord3D        *points,
	INPUT int               *colours,
	INPUT int               numPolylines,
	INPUT int               *sizes
	));

extern void PEXEncodedTextUnpacked
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlCoord3D	* origin,
	INPUT pxlVector3D	* vec1,
	INPUT pxlVector3D	* vec2,
	INPUT int		numCharStrings,
	INPUT int		* charSetList,
	INPUT int		* charWidthList,
	INPUT int		* encodingStateList,
	INPUT pxlStringData	* charStringsList,
	INPUT int		highByteFirst
	));

extern void PEXEncodedText2DUnpacked
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlCoord2D	* origin,
	INPUT int		numCharStrings,
	INPUT int		* charSetList,
	INPUT int		* charWidthList,
	INPUT int		* encodingStateList,
	INPUT pxlStringData	* charStringsList,
	INPUT int		highByteFirst
	));

extern void PEXEncodedAnnoTextUnpacked
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlCoord3D	* origin,
	INPUT pxlCoord3D	* offset,
	INPUT int		numCharStrings,
	INPUT int		* charSetList,
	INPUT int		* charWidthList,
	INPUT int		* encodingStateList,
	INPUT pxlStringData	* charStringsList,
	INPUT int		highByteFirst
	));

extern void PEXEncodedAnnoText2DUnpacked
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlCoord2D	* origin,
	INPUT pxlCoord2D	* offset,
	INPUT int		numCharStrings,
	INPUT int		* charSetList,
	INPUT int		* charWidthList,
	INPUT int		* encodingStateList,
	INPUT pxlStringData	* charStringsList,
	INPUT int		highByteFirst
	));

extern void PEXFillAreaDataUnpacked
    ARGS ((
	INPUT pxlOCBuf          *ocbuf,
	INPUT int               shape,
	INPUT int               ignoreEdges,
	INPUT unsigned long     facetAttributes,
	INPUT unsigned long     vertexAttributes,
	INPUT int               colourType,
	INPUT int               *facetColour,
	INPUT pxlVector3D       *facetNormal,
	INPUT pxlCoord3D        *points,
	INPUT int               *colours,
	INPUT pxlVector3D       *normals,
	INPUT int               numPoints
	));

extern void PEXFillAreaSetUnpacked
    ARGS ((
	INPUT pxlOCBuf          *ocbuf,
	INPUT int               shape,
	INPUT int               ignoreEdges,
	INPUT int               contourHint,
	INPUT pxlCoord3D        *points,
	INPUT int               numContours,
	INPUT int               *sizes
	));

extern void PEXFillAreaSet2DUnpacked
    ARGS ((
	INPUT pxlOCBuf          *ocbuf,
	INPUT int               shape,
	INPUT int               ignoreEdges,
	INPUT int               contourHint,
	INPUT pxlCoord2D        *points,
	INPUT int               numContours,
	INPUT int               *sizes
	));

extern void PEXFillAreaSetWithDataUnpacked
    ARGS ((
	INPUT pxlOCBuf          *ocbuf,
	INPUT int               shape,
	INPUT int               ignoreEdges,
	INPUT int               contourHint,
	INPUT unsigned long     facetAttributes,
	INPUT unsigned long     vertexAttributes,
	INPUT int               colourType,
	INPUT int               *facetColour,
	INPUT pxlVector3D       *facetNormal,
	INPUT pxlCoord3D        *points,
	INPUT int               *colours,
	INPUT pxlVector3D       *normals,
	INPUT int               *edges,
	INPUT int               numContours,
	INPUT int               *sizes
	));

extern void PEXTriangleStripUnpacked
    ARGS ((
	INPUT pxlOCBuf          *ocbuf,
	INPUT unsigned long     facetAttributes,
	INPUT unsigned long     vertexAttributes,
	INPUT int               colourType,
	INPUT int               *facetColours,
	INPUT pxlVector3D       *facetNormals,
	INPUT pxlCoord3D        *points,
	INPUT int               *colours,
	INPUT pxlVector3D       *normals,
	INPUT int               numPoints
	));

extern void PEXQuadMeshUnpacked
    ARGS ((
	INPUT pxlOCBuf          *ocbuf,
	INPUT int               shape,
	INPUT unsigned long     facetAttributes,
	INPUT unsigned long     vertexAttributes,
	INPUT int               colourType,
	INPUT int               *facetColours,
	INPUT pxlVector3D       *facetNormals,
	INPUT pxlCoord3D        *points,
	INPUT int               *colours,
	INPUT pxlVector3D       *normals,
	INPUT int               numPointsM,
	INPUT int               numPointsN
	));

extern void PEXIndexedPolygonsToSOFAS
    ARGS ((
	INPUT pxlOCBuf          *ocbuf,
	INPUT int               shape,
	INPUT unsigned long     facetAttributes,
	INPUT unsigned long     edgeAttributes,
	INPUT unsigned long     vertexAttributes,
	INPUT int               colourType,
	INPUT int               *facetColours,
	INPUT pxlVector3D       *facetNormals,
	INPUT int               numFacets,
	INPUT int               *edges,
	INPUT int               *edgeVisFlags,
	INPUT pxlCoord3D        *points,
	INPUT int               *colours,
	INPUT pxlVector3D       *normals,
	INPUT int               numVertices,
	INPUT int               *facetCounts
	));

extern void MPEXIndexedPolygonsUnpacked
    ARGS ((
	INPUT pxlOCBuf          *ocbuf,
	INPUT int               shape,
	INPUT unsigned long     facetAttributes,
	INPUT unsigned long     edgeAttributes,
	INPUT unsigned long     vertexAttributes,
	INPUT int               colourType,
	INPUT int               *facetColours,
	INPUT pxlVector3D       *facetNormals,
	INPUT int               numFacets,
	INPUT int               *edges,
	INPUT int               *edgeVisFlags,
	INPUT pxlCoord3D        *points,
	INPUT int               *colours,
	INPUT pxlVector3D       *normals,
	INPUT int               numVertices,
	INPUT int               *facetCounts
	));

extern void PEXNurbSurfaceUnpacked
    ARGS ((
	INPUT pxlOCBuf          *ocbuf,
	INPUT int               uOrder,
	INPUT int               vOrder,
	INPUT float             *uKnots,
	INPUT int               numUKnots,
	INPUT float             *vKnots,
	INPUT int               numVKnots,
	INPUT int               surfaceType,
	INPUT char              *points,
	INPUT int               numMPoints,
	INPUT int               numNPoints,
	INPUT pxlPhigsTrimList  *trimList,
	INPUT int               numTrimLoops
	));
/* 
 * Routines declared in pl_font.c
 */

extern pxlFont PEXOpenFont 
    ARGS ((
	INPUT Display		* display,
	INPUT char		* fontname
	));

extern void PEXCloseFont 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlFont		font
	));

extern Status PEXQueryFont 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlFont		font,
	OUTPUT pxlFontInfo	** fontInfoReturn
	));

extern Status PEXListFonts 
    ARGS ((
	INPUT Display		* display,
	INPUT char		* pattern,
	INPUT int		maxNames,
	OUTPUT int		* countReturn,
	OUTPUT pxlStringData	* * namesReturn
	));

extern Status PEXListFontsWithInfo 
    ARGS ((
	INPUT Display		* display,
	INPUT char		* pattern,
	INPUT int		maxNames,
	OUTPUT int		* numStringsReturn,
	OUTPUT int		* numFontInfoReturn,
	OUTPUT pxlStringData	** namesReturn,
	OUTPUT pxlFontInfo	** infoReturn
	));

extern Status PEXQueryTextExtents 
    ARGS ((
	INPUT Display		* display,
	INPUT XID		id,
	INPUT int		fontGroup,
	INPUT int		path,
	INPUT double		expansion,
	INPUT double		spacing,
	INPUT double		height,
	INPUT int		halign,
	INPUT int		valign,
	INPUT char		* string,
	INPUT int		count,
	OUTPUT pxlCoord2D	* lowerLeftReturn,
	OUTPUT pxlCoord2D	* upperRightReturn,
	OUTPUT pxlCoord2D	* concatPtReturn
	));

/* 
 * Routines defined in pl_free.c
 */

extern void PEXFree 
    ARGS ((
	INPUT char		* pch
	));

/* 
 * Routines defined in pl_lut.c
 */

extern pxlLookupTable PEXCreateLookupTable 
    ARGS ((
	INPUT Display		* display,
	INPUT Drawable		d,
	INPUT int		type
	));

extern void PEXFreeLookupTable 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlLookupTable	lut
	));

extern void PEXCopyLookupTable 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlLookupTable	srcLut,
	INPUT pxlLookupTable	destLut
	));

extern Status PEXGetTableInfo 
    ARGS ((
	INPUT Display		* display,
	INPUT Drawable		d,
	INPUT int		type,
	OUTPUT pxlTableInfo	* * tableInfoReturn
	));

extern Status PEXGetPredefinedEntries 
    ARGS ((
	INPUT Display		* display,
	INPUT Drawable		d,
	INPUT int		type,
	INPUT int		start,
	INPUT int		count,
	OUTPUT char		* * entriesReturn,
	OUTPUT int		* numEntriesReturn
	));

extern Status PEXGetDefinedIndices 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlLookupTable	lut,
	OUTPUT unsigned short	* * indicesReturn,
	OUTPUT int		* numIndicesReturn
	));

extern Status PEXGetTableEntry 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlLookupTable	lut,
	INPUT int		index,
	INPUT int		valueType,
	OUTPUT int		* statusReturn,
	OUTPUT char		* * entryReturn
	));

extern Status PEXGetTableEntries 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlLookupTable	lut,
	INPUT int		start,
	INPUT int		count,
	INPUT int		valueType,
	OUTPUT char		* * entriesReturn
	));

extern void PEXSetTableEntries 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlLookupTable	lut,
	INPUT int		type,
	INPUT int		start,
	INPUT int		count,
	INPUT char		* entries
	));

extern void PEXDeleteTableEntries 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlLookupTable	lut,
	INPUT int		start,
	INPUT int		count
	));

/* 
 * Routines defined in pl_nameset.c
 */

extern pxlNameSet PEXCreateNameSet 
    ARGS ((
	INPUT Display		* display
	));

extern void PEXFreeNameSet 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlNameSet	ns
	));

extern void PEXCopyNameSet 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlNameSet	srcNs,
	INPUT pxlNameSet	destNs
	));

extern Status PEXGetNameSet 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlNameSet	ns,
	OUTPUT pxlName 		* * namesReturn,
	OUTPUT int		* numNamesReturn
	));

extern void PEXChangeNameSet 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlNameSet	ns,
	INPUT int		action,
	INPUT pxlName		* values,
	INPUT int		numValues
	));

/* 
 * Routines defined in pl_oc_struct.c
 */

extern void PEXExecuteStructure 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT pxlStructure	structure
	));

extern void PEXLabel 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT long		label
	));

extern void PEXApplicationData 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT char		* data,
	INPUT int		numBytes
	));

extern void PEXGse 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* ocbuf,
	INPUT unsigned long	id,
	INPUT char		* data,
	INPUT int		numBytes
	));

/* 
 * Routines defined in pl_pc.c
 */

extern pxlPipelineContext PEXCreatePipelineContext 
    ARGS ((
	INPUT Display		* display,
	INPUT unsigned long	valueMask[2],
	INPUT pxlPCAttributes	* values
	));

extern void PEXFreePipelineContext 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPipelineContext  pc
	));

extern void PEXCopyPipelineContext 
    ARGS ((
	INPUT Display		* display,
	INPUT unsigned long	valueMask[2],
	INPUT pxlPipelineContext srcPc,
	INPUT pxlPipelineContext destPc
	));

extern Status PEXGetPipelineContext 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPipelineContext pc,
	INPUT unsigned long	valueMask[2],
	OUTPUT pxlPCAttributes	* * pcAttributesReturn
	));

extern void PEXChangePipelineContext 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPipelineContext pc,
	INPUT unsigned long	valueMask[2],
	INPUT pxlPCAttributes	* pcAttributes
	));

/* 
 * Routines defined in pl_rdr.c
 */

extern pxlRenderer PEXCreateRenderer 
    ARGS ((
	INPUT Display		* display,
	INPUT Drawable		d,
	INPUT unsigned long	valueMask,
	INPUT pxlRendererAttributes 	* values
	));

extern void PEXFreeRenderer 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlRenderer	renderer
	));

extern Status PEXGetRendererAttributes
    ARGS ((
	INPUT Display			* display,
	INPUT pxlRenderer		renderer,
	INPUT unsigned long		valueMask,
	OUTPUT pxlRendererAttributes 	* * valuesReturn
	));

extern Status PEXGetRendererDynamics
    ARGS ((
	INPUT Display		* display,
	INPUT pxlRenderer	renderer,
	OUTPUT unsigned long	* tablesReturn,
	OUTPUT unsigned long	* namesetsReturn,
	OUTPUT unsigned long	* attributesReturn
	));

extern void PEXChangeRenderer 
    ARGS ((
	INPUT Display			* display,
	INPUT pxlRenderer		renderer,
	INPUT unsigned long		valueMask,
	INPUT pxlRendererAttributes 	* values
	));

extern void PEXBeginRendering 
    ARGS ((
	INPUT Display		* display,
	INPUT Drawable		d,
	INPUT pxlRenderer	renderer
	));

extern void PEXEndRendering 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlRenderer	renderer,
	INPUT Bool		flush
	));

extern void PEXBeginStructure 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlRenderer	renderer,
	INPUT unsigned long	id
	));

extern void PEXEndStructure 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlRenderer	renderer
	));

extern void PEXRenderNetwork 
    ARGS ((
	INPUT Display		* display,
	INPUT Drawable		d,
	INPUT pxlRenderer	renderer,
	INPUT pxlStructure	structure
	));

#ifdef MPEX

extern void MPEXBeginTransparencyRendering
    ARGS ((
	INPUT Display		*display,
	INPUT Drawable		d,
	INPUT pxlRenderer	renderer,
	INPUT int		clear
	));

extern int MPEXEndTransparencyRendering 
    ARGS ((
	INPUT Display		*display,
	INPUT pxlRenderer	renderer,
	INPUT Bool		flush
	));

#endif

/* 
 * Routines defined in pl_startup.c
 */

extern Status PEXInitialize 
    ARGS ((
	INPUT Display		* display,
	OUTPUT pxlInfo		* * pexinfo
	));

extern Status PEXTerminate 
    ARGS ((
	INPUT Display		* display,
	INPUT XExtCodes		* codes
	));

extern Status PEXGetEnumTypeInfo 
    ARGS ((	
	INPUT Display			* display,
	INPUT Drawable			drawable,
	INPUT int			enumType,
	INPUT unsigned long		returnMask,
	OUTPUT pxlEnumTypeDescList	* * enumInfoReturn,
	OUTPUT int			* countReturn
	));

extern void PEXSetFloatingPointFormat 
    ARGS ((
	INPUT int		fpFormat
	));

extern int PEXGetFloatingPointFormat NO_ARGS;

extern void PEXSetDirectColourFormat 
    ARGS ((
	INPUT int		colourFormat
	));

extern int PEXGetDirectColourFormat NO_ARGS;

extern void PEXSetColourType 
    ARGS ((
	INPUT int		colourType
	));

extern int PEXGetColourType NO_ARGS;

extern Status PEXGetImpDepConstants 
    ARGS ((
	INPUT Display		* display,
	INPUT Drawable		drawable,
	INPUT short		names[],
	INPUT int		count,
	OUTPUT unsigned long	* valuesReturn
	));

/* 
 * Routines defined in pl_struct.c
 */

extern pxlStructure PEXCreateStructure 
    ARGS ((
	INPUT Display		* display
	));

extern void PEXDestroyStructures 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	* structures,
	INPUT int		numStructures
	));

extern void PEXCopyStructure 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	srcStructure,
	INPUT pxlStructure	destStructure
	));

extern Status PEXGetStructureInfo 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	structure,
	OUTPUT pxlStructureInformation	* * structureInfoReturn
	));

extern Status PEXGetElementInfo 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	structure,
	INPUT int		whence1,
	INPUT long		offset1,
	INPUT int		whence2,
	INPUT long		offset2,
	OUTPUT pxlElementInfo	* * elementInfoReturn,
	OUTPUT int		* numElementInfoReturn
	));

extern Status PEXGetStructuresInNetwork 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	structure,
	INPUT int		which,
	OUTPUT pxlStructure	* * structuresReturn,
	OUTPUT int		* numStructuresReturn
	));

extern Status PEXGetAncestors 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	structure,
	INPUT int		pathOrder,
	INPUT int		pathDepth,
	OUTPUT pxlStructurePath	* * pathsReturn,
	OUTPUT int		* numPathsReturn
	));

extern Status PEXGetDescendants 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	structure,
	INPUT int		pathOrder,
	INPUT int		pathDepth,
	OUTPUT pxlStructurePath	* * pathsReturn,
	OUTPUT int		* numPathsReturn
	));

extern Status PEXFetchElements 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	structure,
	INPUT int		whence1,
	INPUT long		offset1,
	INPUT int		whence2,
	INPUT long		offset2,
	OUTPUT pxlOCBuf		* * ocbufReturn
	));

extern void PEXSetEditingMode 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	structure,
	INPUT int		mode
	));

extern void PEXSetElementPtr 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	structure,
	INPUT int		whence,
	INPUT long		offset
	));

extern void PEXSetElementPtrAtLabel 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	structure,
	INPUT long		label,
	INPUT long		offset
	));

extern Status PEXElementSearch 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	structure,
	INPUT int		whence,
	INPUT long		offset,
	INPUT int		direction,
	INPUT short		* inclList,
	INPUT int		numIncl,
	INPUT short		* exclList,
	INPUT int		numExcl,
	OUTPUT int		* statusReturn,
	OUTPUT unsigned long	* offsetReturn
	));

extern void PEXDeleteElements 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	structure,
	INPUT int		whence1,
	INPUT long		offset1,
	INPUT int		whence2,
	INPUT long		offset2
	));

extern void PEXDeleteToLabel 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	structure,
	INPUT int		whence,
	INPUT long		offset,
	INPUT long		label
	));

extern void PEXDeleteBetweenLabels 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	structure,
	INPUT long		label1,
	INPUT long		label2
	));

extern void PEXCopyElements 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	srcStructure,
	INPUT int		srcWhence1,
	INPUT long		srcOffset1,
	INPUT int		srcWhence2,
	INPUT long		srcOffset2,
	INPUT pxlStructure	destStructure,
	INPUT int		destWhence,
	INPUT long		destOffset
	));

extern void PEXChangeStructureRefs 
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	oldStructure,
	INPUT pxlStructure	newStructure
	));

/* 
 * Routines defined in pl_utl.c
 */

extern Status PEXRotationMatrix 
    ARGS ((
	INPUT int		axis,
	INPUT double		angle,
	OUTPUT pxlMatrix	matrixReturn
	));

extern void PEXArbRotationMatrix 
    ARGS ((
	INPUT pxlCoord3D	* pt1,
	INPUT pxlCoord3D	* pt2,
	INPUT double		angle,
	OUTPUT pxlMatrix	matrixReturn
	));

extern void PEXScalingMatrix 
    ARGS ((
	INPUT double		sx,
	INPUT double		sy,
	INPUT double		sz,
	OUTPUT pxlMatrix	matrixReturn
	));

extern void PEXTranslationMatrix 
    ARGS ((
	INPUT double		tx,
	INPUT double		ty,
	INPUT double		tz,
	OUTPUT pxlMatrix	matrixReturn
	));

extern void PEXCopyMatrix 
    ARGS ((
	INPUT pxlMatrix		matrix,
	OUTPUT pxlMatrix	matrixReturn
	));

extern void PEXMultiplyMatrices 
    ARGS ((
	INPUT pxlMatrix		mat1,
	INPUT pxlMatrix		mat2,
	OUTPUT pxlMatrix	matrixReturn
	));

extern void PEXIdentityMatrix 
    ARGS ((
	OUTPUT pxlMatrix	matrixReturn
	));

extern Status PEXInvertMatrix 
    ARGS ((
	INPUT pxlMatrix		matrix,
	OUTPUT pxlMatrix	inverseReturn
	));

extern Status PEXTransform3dPoints 
    ARGS ((
	INPUT pxlMatrix		mat,
	INPUT pxlCoord3D	* pts,
	INPUT int		numPts,
	OUTPUT pxlCoord3D	* ptsReturn
	));

extern void PEXTransform4dPoints 
    ARGS ((
	INPUT pxlMatrix		mat,
	INPUT pxlCoord4D	* pts,
	INPUT int		numPts,
	OUTPUT pxlCoord4D	* ptsReturn
	));

extern Status PEXLookatViewMatrix 
    ARGS ((
	INPUT pxlCoord3D	* from,
	INPUT pxlCoord3D	* to,
	INPUT pxlVector3D	* up,
	OUTPUT pxlMatrix	matReturn
	));

extern Status PEXPolarViewMatrix 
    ARGS ((
	INPUT pxlCoord3D	* from,
	INPUT double		distance,
	INPUT double		azimuth,
	INPUT double		altitude,
	INPUT double		twist,
	OUTPUT pxlMatrix	matReturn
	));

extern Status PEXOrthoProjMatrix 
    ARGS ((
	INPUT double		height,
	INPUT double		aspect,
	INPUT double		near,
	INPUT double		far,
	OUTPUT pxlMatrix	matReturn
	));

extern Status PEXPerspProjMatrix 
    ARGS ((
	INPUT double		fovy,
	INPUT double		distance,
	INPUT double		aspect,
	INPUT double		near,
	INPUT double		far,
	OUTPUT pxlMatrix	matReturn
	));

extern void PEXComputeNormals 
    ARGS ((
	INPUT int		method,
	INPUT int		primType,
	INPUT char		* prim,
	OUTPUT char		* primReturn
	));

/*
 * Routines from pl_mpex.c
 */

#ifdef MPEX

extern void MPEXChangeNewRenderer 
    ARGS ((
	INPUT Display			*display,
	INPUT pxlRenderer		renderer,
	INPUT unsigned long		valueMask,
	INPUT mpxlRendererAttributes 	*values
	));

extern Status MPEXGetNewRendererAttributes
    ARGS ((
	INPUT Display			*display,
	INPUT pxlRenderer		renderer,
	INPUT unsigned long		valueMask,
	OUTPUT mpxlRendererAttributes **valuesReturn
	));

extern void MPEXRenderElements 
    ARGS ((
	INPUT Display		*display,
	INPUT pxlRenderer	renderer,
	INPUT pxlStructure	structure,
	INPUT int		whence1,
	INPUT long		offset1,
	INPUT int		whence2,
	INPUT long		offset2
	));

extern void MPEXAccumulateState 
    ARGS ((
	INPUT Display		*display,
	INPUT pxlRenderer	renderer,
	INPUT pxlElementRef	*elements,
	INPUT int		numElements
	));

extern void MPEXBeginPickOne
    ARGS ((
	INPUT Display		*display,
	INPUT pxlRenderer	renderer,
	INPUT Drawable		d,
	INPUT unsigned long	id,
	INPUT int		which
	));

extern Status MPEXEndPickOne 
    ARGS ((
	INPUT Display			*display,
	INPUT pxlRenderer		renderer,
	OUTPUT pxlPickPath	**elementsReturn,
	OUTPUT int			*numElementsReturn
	));

extern Status MPEXPickOne
    ARGS ((
	INPUT Display			*display,
	INPUT pxlRenderer		renderer,
	INPUT Drawable			d,
	INPUT pxlStructure		structure,
	INPUT int			which,
	OUTPUT pxlPickPath	**elementsReturn,
	OUTPUT int			*numElementsReturn
	));

extern void MPEXBeginPickAll 
    ARGS ((
	INPUT Display		*display,
	INPUT pxlRenderer	renderer,
	INPUT Drawable		d,
	INPUT unsigned long	id,
	INPUT Bool		sendEvent
	));

extern Status MPEXEndPickAll 
    ARGS ((
	INPUT Display		*display,
	INPUT pxlRenderer	renderer,
	OUTPUT int		*moreReturn,
	OUTPUT mpxlPickAllPath	**pathsReturn,
	OUTPUT int		*numPathsReturn
	));

extern Status MPEXPickAll 
    ARGS ((
	INPUT Display		*display,
	INPUT pxlRenderer	renderer,
	INPUT Drawable		d,
	OUTPUT int		*moreReturn,
	OUTPUT mpxlPickAllPath	**pathsReturn,
	OUTPUT int		*numPathsReturn
	));

extern void MPEXSetHighlightIndex 
    ARGS ((
	INPUT pxlOCBuf		*ocbuf,
	INPUT int		index
	));

extern void MPEXSetEchoIndex 
    ARGS ((
	INPUT pxlOCBuf		*ocbuf,
	INPUT int		index
	));

extern void MPEXSetElementPtrAtPickID 
    ARGS ((
	INPUT Display		*display,
	INPUT pxlStructure	structure,
	INPUT unsigned long	id,
	INPUT long		offset
	));

extern void MPEXNoop 
    ARGS ((
	INPUT pxlOCBuf  *ocbuf
	));

extern void MPEXCircle
    ARGS ((
	INPUT pxlOCBuf		*ocbuf,
	INPUT pxlCoord3D	*center,
	INPUT double		radius
	));

extern void MPEXArc
    ARGS ((
	INPUT pxlOCBuf		*ocbuf,
	INPUT pxlCoord3D	*center,
	INPUT double		radius,
	INPUT double		startAngle,
	INPUT double		endAngle
	));

extern void MPEXGridRectangular
    ARGS ((
	INPUT_OUTPUT pxlOCBuf   *ocbuf,
	INPUT pxlCoord3D        *origin,
	INPUT float             xdist,
	INPUT float             ydist
        ));

extern void MPEXGridRadial
    ARGS ((
	INPUT_OUTPUT pxlOCBuf   *ocbuf,
	INPUT pxlCoord3D        *center,
	INPUT float             startAngle,
	INPUT float             endAngle,
	INPUT float             angDist,
	INPUT float             radDist
        ));

extern void MPEXEllipticalArc2D
    ARGS ((
	INPUT pxlOCBuf		*ocbuf,
	INPUT pxlCoord2D	*center,
	INPUT pxlCoord2D	*majorAxis,
	INPUT pxlCoord2D	*minorAxis,
	INPUT double		startAngle,
	INPUT double		endAngle
	));


extern void MPEXEllipticalArc3D
    ARGS ((
	INPUT pxlOCBuf		*ocbuf,
	INPUT pxlCoord3D	*center,
	INPUT pxlCoord3D	*majorAxis,
	INPUT pxlCoord3D	*minorAxis,
	INPUT double		startAngle,
	INPUT double		endAngle
	));

extern void MPEXEllipse2D
    ARGS ((
	INPUT pxlOCBuf		*ocbuf,
	INPUT pxlCoord2D	*center,
	INPUT pxlCoord2D	*majorAxis,
	INPUT pxlCoord2D	*minorAxis
	));

extern void MPEXEllipse3D
    ARGS ((
	INPUT pxlOCBuf		*ocbuf,
	INPUT pxlCoord3D	*center,
	INPUT pxlCoord3D	*majorAxis,
	INPUT pxlCoord3D	*minorAxis
	));

extern void MPEXAnnotationPixmap 
    ARGS ((
	INPUT pxlOCBuf		*ocbuf,
	INPUT pxlCoord3D	*origin,
	INPUT int		index
	));

extern void MPEXSetDashPattern 
    ARGS ((
	INPUT Display		*display,
	INPUT pxlRenderer	renderer,
	INPUT int		patternNumber,
	INPUT unsigned long	pattern
	));

extern Status MPEXGetDashPattern
    ARGS ((
	INPUT Display		*display,
	INPUT pxlRenderer	renderer,
	INPUT int		patternNumber,
	OUTPUT unsigned long	*pattern
	));

extern void MPEXSetMarkerGlyph 
    ARGS ((
	INPUT Display		*display,
	INPUT pxlRenderer	renderer,
	INPUT int		glyphNumber,
	INPUT unsigned short	glyph[16],
	INPUT int		xOffset,
	INPUT int		yOffset
	));

extern Status MPEXGetMarkerGlyph
    ARGS ((
	INPUT Display		*display,
	INPUT pxlRenderer	renderer,
	INPUT int		glyphNumber,
	OUTPUT unsigned short	glyph[16],
	OUTPUT int		*xOffset,
	OUTPUT int		*yOffset
	));

#endif /* MPEX */

/* 
 * Routines from pl_oc_parse.c
 */

extern void PEXDumpOCBufHeader 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* oc,
	INPUT_OUTPUT FILE	* output_file
	));

extern void PEXGetOCListsFromOCBuf 
    ARGS ((
	INPUT_OUTPUT pxlOCBuf	* oc,
	INPUT void		(* routine) (),
	INPUT unsigned long	arg
	));

/* 
 * Routines defined in pl_sc.c
 */

extern pxlSearchContext PEXCreateSearchContext
    ARGS ((
	INPUT Display		* display,
	INPUT unsigned long	valueMask,
	INPUT pxlSCAttributes	* values
	));

extern void PEXFreeSearchContext
    ARGS ((
	INPUT Display		* display,
	INPUT pxlSearchContext	sc
	));

extern void PEXCopySearchContext
    ARGS ((
	INPUT Display		* display,
	INPUT unsigned long	valueMask,
	INPUT pxlSearchContext	srcSc,
	INPUT pxlSearchContext	destSc
	));

extern Status PEXGetSearchContext
    ARGS ((
	INPUT Display		* display,
	INPUT pxlSearchContext	sc,
	INPUT unsigned long	valueMask,
	OUTPUT pxlSCAttributes	* * scAttributesReturn
	));

extern void PEXChangeSearchContext
    ARGS ((
	INPUT Display		* display,
	INPUT pxlSearchContext	sc,
	INPUT unsigned long	valueMask,
	OUTPUT pxlSCAttributes	* values
	));

extern Status PEXSearchNetwork
    ARGS ((
	INPUT Display		* display,
	INPUT pxlSearchContext	sc,
	OUTPUT pxlStructurePath	* * pathReturn
	));

/* 
 * Routined defined in pl_wks.c
 */

extern pxlPhigsWks PEXCreatePhigsWks
    ARGS ((
	INPUT Display		* display,
	INPUT Drawable		d,
	INPUT pxlLookupTable	lineBundle,
	INPUT pxlLookupTable	markerBundle,
	INPUT pxlLookupTable	textBundle,
	INPUT pxlLookupTable	interiorBundle,
	INPUT pxlLookupTable	edgeBundle,
	INPUT pxlLookupTable	colourTable,
	INPUT pxlLookupTable	patternTable,
	INPUT pxlLookupTable	textFontTable,
	INPUT pxlLookupTable	depthCueTable,
	INPUT pxlLookupTable	lightTable,
	INPUT pxlLookupTable	colourApproxTable,
	INPUT pxlNameSet	highlightIncl,
	INPUT pxlNameSet	highlightExcl,
	INPUT pxlNameSet	invisibilityIncl,
	INPUT pxlNameSet	invisibilityExcl
	));

extern void PEXFreePhigsWks
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks
	));

extern Status PEXGetWksInfo
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks,
	INPUT unsigned long	valueMask,
	OUTPUT pxlPhigsWksInfo	* * wksAttributesReturn
	));

extern Status PEXGetWksDynamics
    ARGS ((
	INPUT Display			* display,
	INPUT Drawable			d,
	OUTPUT pxlPhigsWksDynamics	* * wksDynamicsReturn
	));

extern Status PEXGetWksViewRep
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks,
	INPUT unsigned int	index,
	OUTPUT int		* viewUpdateReturn,
	OUTPUT pxlViewEntry	* * reqViewReturn,
	OUTPUT pxlViewEntry	* * curViewReturn
	));

extern Status PEXGetWksPostings
    ARGS ((
	INPUT Display		* display,
	INPUT pxlStructure	structure,
	OUTPUT pxlPhigsWks	* * wksReturn
	));

extern void PEXSetWksViewPriority
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks,
	INPUT unsigned int	index1,
	INPUT unsigned int	index2,
	INPUT int		priority
	));

extern void PEXSetWksDrawableUpdate
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks,
	INPUT int		drawableUpdate
	));

extern void PEXSetWksViewRep
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks,
	INPUT unsigned int	viewIndex,
	INPUT pxlViewEntry	* viewRep
	));

extern void PEXSetWksNpcSubvolume
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks,
	INPUT pxlNpcSubvolume	* npcSubvolume
	));

extern void PEXSetWksViewport
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks,
	INPUT pxlViewport	* viewport
	));

extern void PEXSetWksHlhsrMode
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks,
	INPUT unsigned int	hlhsrMode
	));

extern void PEXRedrawStructures
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks
	));

extern void PEXUpdatePhigsWks
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks
	));

extern void PEXExecuteDeferredActions
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks
	));

extern void PEXMapDCtoWC
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks,
	INPUT pxlDeviceCoord	* dcPoints,
	INPUT int		numPoints,
	OUTPUT pxlCoord3D	* wcPointsReturn,
	OUTPUT unsigned int	* viewIndexReturn
	));

extern void PEXMapWCtoDC
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks,
	INPUT pxlCoord3D	* wcPoints,
	INPUT int		numPoints,
	INPUT unsigned int	viewIndex,
	OUTPUT pxlDeviceCoord	* dcPointsReturn
	));

extern void PEXPostStructure
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks,
	INPUT pxlStructure	structure,
	INPUT unsigned long	priority
	));

extern void PEXUnpostStructure
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks,
	INPUT pxlStructure	structure
	));

extern void PEXUnpostAllStructures
    ARGS ((
	INPUT Display		* display,
	INPUT pxlPhigsWks	wks
	));


#ifdef MUST_CLEAN_UP
#   undef ARGS
#   undef NO_ARGS
#   undef INPUT
#   undef OUTPUT
#   undef INPUT_OUTPUT
#   undef MUST_CLEAN_UP
#endif

#endif /* PEXLIBPROTOS_DEFINED */
