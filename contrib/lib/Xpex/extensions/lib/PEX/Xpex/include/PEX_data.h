/* $Header: PEX_data.h,v 2.3 91/09/11 16:06:00 sinyaw Exp $ */

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

#ifndef PEX_DATA_H_INCLUDED
#define PEX_DATA_H_INCLUDED

typedef CARD16 pexImpDepConstantName;

typedef struct {
	CARD32 nominalLineWidth B32;
	CARD32 numSupportedLineWidths B32;
	CARD32 minLineWidth B32;
	CARD32 maxLineWidth B32;
	CARD32 nominalEdgeWidth B32;
	CARD32 numSupportedEdgeWidths B32;
	CARD32 minEdgeWidth B32;
	CARD32 maxEdgeWidth B32;
	CARD32 nominalMarkerSize B32;
	CARD32 numSupportedMarkerSizes B32;
	CARD32 minMarkerSize B32;
	CARD32 maxMarkerSize B32;
	PEXFLOAT chromaticityRedU B32;
	PEXFLOAT chromaticityRedV B32;
	PEXFLOAT luminanceRed B32;
	PEXFLOAT chromaticityGreenU B32;
	PEXFLOAT chromaticityGreenV B32;
	PEXFLOAT luminanceGreen B32;
	PEXFLOAT chromaticityBlueU B32;
	PEXFLOAT chromaticityBlueV B32;
	PEXFLOAT luminanceBlue B32;
	PEXFLOAT chromaticityWhiteU B32;
	PEXFLOAT chromaticityWhiteV B32;
	PEXFLOAT luminanceWhite B32;
	CARD32 maxNamesetNames B32;
	CARD32 maxModelClipPlanes B32;
	CARD32 transparencySupported B32;
	CARD32 ditheringSupported B32;
	CARD32 maxNonAmbientLights B32;
	CARD32 maxNurbOrder B32;
	CARD32 maxTrimCurveOrder B32;
	CARD32 bestColorApproxValues B32;
	pexSwitch doubleBufferingSupported B32;
} pexAllImpDepConstants;

typedef struct {
	PEXFLOAT  ambient;
	PEXFLOAT  diffuse;
	PEXFLOAT  specular;
	PEXFLOAT  specularConc;
	PEXFLOAT  transmission;  /* 0.0 = opaque, 1.0 = transparent */
	pexColourType colourType B16;
	CARD16 pad1 B16;
	pexTableIndex colourIndex B16;
	CARD16 pad2 B16;
} pexReflAttrDataIndexed;

typedef struct {
	PEXFLOAT  ambient;
	PEXFLOAT  diffuse;
	PEXFLOAT  specular;
	PEXFLOAT  specularConc;
	PEXFLOAT  transmission;  /* 0.0 = opaque, 1.0 = transparent */
	pexColourType  colourType B16;
	CARD16  pad B16;
	pexRgb8Colour  rgb8;
} pexReflAttrDataRgb8;

typedef struct {
	PEXFLOAT  ambient;
	PEXFLOAT  diffuse;
	PEXFLOAT  specular;
	PEXFLOAT  specularConc;
	PEXFLOAT  transmission;  /* 0.0 = opaque, 1.0 = transparent */
	pexColourType  colourType B16;
	CARD16  pad B16;
	pexRgb16Colour  rgb16;
} pexReflAttrDataRgb16;

typedef struct {
	PEXFLOAT  ambient;
	PEXFLOAT  diffuse;
	PEXFLOAT  specular;
	PEXFLOAT  specularConc;
	PEXFLOAT  transmission;  /* 0.0 = opaque, 1.0 = transparent */
	pexColourType  colourType B16;
	CARD16  pad B16;
	pexFloatColour  colourFloat;
} pexReflAttrDataFloat;

typedef struct {
	pexEnumTypeIndex  lineType B16;
	pexEnumTypeIndex  polylineInterp B16;
	pexCurveApprox  curveApprox;
	PEXFLOAT  lineWidth;
	pexColourType  colourType B16;
	CARD16 unused B16;
	pexTableIndex colourIndex;
	CARD16 pad B16;
} pexLineBundleDataIndexed;

typedef struct {
	pexEnumTypeIndex lineType B16;
	pexEnumTypeIndex polylineInterp B16;
	pexCurveApprox  curveApprox;
	PEXFLOAT  lineWidth;
	pexColourType colourType B16;
	CARD16 unused B16;
	pexRgb8Colour rgb8;
} pexLineBundleDataRgb8;

typedef struct {
	pexEnumTypeIndex lineType B16;
	pexEnumTypeIndex polylineInterp B16;
	pexColourType colourType B16;
	CARD16 unused B16;
	pexCurveApprox  curveApprox;
	PEXFLOAT  lineWidth;
	pexRgb16Colour rgb16;
} pexLineBundleDataRgb16;

typedef struct {
	pexEnumTypeIndex lineType B16;
	pexEnumTypeIndex polylineInterp B16;
	pexCurveApprox  curveApprox;
	PEXFLOAT  lineWidth;
	pexColourType colourType B16;
	CARD16 pad B16;
	pexFloatColour colourFloat;
} pexLineBundleDataFloat;

typedef struct {
	pexEnumTypeIndex markerType B16;
	INT16 unused B16;
	PEXFLOAT markerScale; 
	pexColourType colourType B16;
	CARD16 pad1 B16;
	pexTableIndex colourIndex;
	CARD16 pad2 B16;
} pexMarkerBundleDataIndexed;

typedef struct {
	pexEnumTypeIndex markerType B16;
	INT16 unused B16;
	PEXFLOAT markerScale; 
	pexColourType colourType B16;
	CARD16 pad B16;
	pexRgb8Colour rgb8;
} pexMarkerBundleDataRgb8;

typedef struct {
	pexEnumTypeIndex markerType B16;
	INT16 unused B16;
	PEXFLOAT markerScale; 
	pexColourType colourType B16;
	CARD16 pad B16;
	pexRgb16Colour rgb16;
} pexMarkerBundleDataRgb16;

typedef struct {
	pexEnumTypeIndex markerType B16;
	INT16 unused B16;
	PEXFLOAT markerScale; 
	pexColourType colourType B16;
	CARD16 pad B16;
	pexFloatColour colorFloat;
} pexMarkerBundleDataFloat;

typedef struct {
	CARD16 textFontIndex B16;
	CARD16 textPrecision B16;
	PEXFLOAT charExpansion;
	PEXFLOAT charSpacing;
	pexColourType colourType B16;
	CARD16 unused B16;
	pexTableIndex colourIndex B16;
	CARD16 pad B16;
} pexTextBundleDataIndexed;

typedef struct {
	CARD16 textFontIndex B16;
	CARD16 textPrecision B16;
	PEXFLOAT charExpansion;
	PEXFLOAT charSpacing;
	pexColourType colourType B16;
	CARD16 unused B16;
	pexRgb8Colour rgb8;
} pexTextBundleDataRgb8;

typedef struct {
	CARD16 textFontIndex B16;
	CARD16 textPrecision B16;
	PEXFLOAT charExpansion;
	PEXFLOAT charSpacing;
	pexColourType colourType B16;
	CARD16 unused B16;
	pexRgb16Colour rgb16;
} pexTextBundleDataRgb16;

typedef struct {
	CARD16 textFontIndex B16;
	CARD16 textPrecision B16;
	PEXFLOAT charExpansion;
	PEXFLOAT charSpacing;
	pexColourType colourType B16;
	CARD16 unused B16;
	pexFloatColour colorFloat;
} pexTextBundleDataFloat;

typedef struct {
	pexEnumTypeIndex    interiorStyle B16;
	INT16       interiorStyleIndex B16;
	pexEnumTypeIndex    reflectionModel B16;
	pexEnumTypeIndex    surfaceInterp B16;
	pexEnumTypeIndex    bfInteriorStyle B16;
	INT16       bfInteriorStyleIndex B16;
	pexEnumTypeIndex    bfReflectionModel B16;
	pexEnumTypeIndex    bfSurfaceInterp B16;
	pexSurfaceApprox    surfaceApprox;

	pexColourType	surfaceColourType B16;
	CARD16 			pad1 B16;
	pexTableIndex	surfaceColourIndex B16;
	CARD16			pad2 B16;

	pexReflAttrDataIndexed reflectionAttr;

	pexColourType	bfSurfaceColourType B16;
	CARD16 			pad3 B16;
	pexTableIndex	bfSurfaceColourIndex B16;
	CARD16			pad4 B16;

	pexReflAttrDataIndexed bfReflectionAttr;
} pexInteriorBundleDataIndexed;

typedef struct {
	pexEnumTypeIndex    interiorStyle B16;
	INT16       interiorStyleIndex B16;
	pexEnumTypeIndex    reflectionModel B16;
	pexEnumTypeIndex    surfaceInterp B16;
	pexEnumTypeIndex    bfInteriorStyle B16;
	INT16       bfInteriorStyleIndex B16;
	pexEnumTypeIndex    bfReflectionModel B16;
	pexEnumTypeIndex    bfSurfaceInterp B16;
	pexSurfaceApprox    surfaceApprox;

	pexColourType	surfaceColorType B16;
	CARD16 			pad1 B16;
	pexRgb8Colour		surfaceColor;

	pexReflAttrDataRgb8 reflectionAttr;

	pexColourType	bfSurfaceColorType B16;
	CARD16 			pad2 B16;
	pexRgb8Colour	bfSurfaceColor;

	pexReflAttrDataRgb8 bfReflectionAttr;
} pexInteriorBundleDataRgb8;

typedef struct {
	pexEnumTypeIndex    interiorStyle B16;
	INT16       interiorStyleIndex B16;
	pexEnumTypeIndex    reflectionModel B16;
	pexEnumTypeIndex    surfaceInterp B16;
	pexEnumTypeIndex    bfInteriorStyle B16;
	INT16       bfInteriorStyleIndex B16;
	pexEnumTypeIndex    bfReflectionModel B16;
	pexEnumTypeIndex    bfSurfaceInterp B16;
	pexSurfaceApprox    surfaceApprox;

	pexColourType	surfaceColorType B16;
	CARD16 			pad1 B16;
	pexRgb16Colour	surfaceColor;

	pexReflAttrDataRgb16 reflectionAttr;

	pexColourType	bfSurfaceColorType B16;
	CARD16 			pad2 B16;
	pexRgb16Colour	bfSurfaceColor;

	pexReflAttrDataRgb16 bfReflectionAttr;
} pexInteriorBundleDataRgb16;

typedef struct {
	pexEnumTypeIndex    interiorStyle B16;
	INT16       interiorStyleIndex B16;
	pexEnumTypeIndex    reflectionModel B16;
	pexEnumTypeIndex    surfaceInterp B16;
	pexEnumTypeIndex    bfInteriorStyle B16;
	INT16       bfInteriorStyleIndex B16;
	pexEnumTypeIndex    bfReflectionModel B16;
	pexEnumTypeIndex    bfSurfaceInterp B16;
	pexSurfaceApprox    surfaceApprox;

	pexColourType	surfaceColorType B16;
	CARD16 			pad1 B16;
	pexFloatColour		surfaceColor;

	pexReflAttrDataFloat reflectionAttr;

	pexColourType	bfSurfaceColorType B16;
	CARD16 			pad2 B16;
	pexFloatColour		bfSurfaceColor; 

	pexReflAttrDataFloat bfReflectionAttr;
} pexInteriorBundleDataFloat;

typedef struct {
	pexSwitch edges;
	CARD8 pad1;
	pexEnumTypeIndex edgeType B16;
	PEXFLOAT edgeWidth;
	pexColourType colourType B16;
	CARD16 pad2 B16;
	pexTableIndex colourIndex B16;
	CARD16 pad3 B16;
} pexEdgeBundleDataIndexed;

typedef struct {
	pexSwitch edges;
	CARD8 unused;
	pexEnumTypeIndex edgeType B16;
	PEXFLOAT edgeWidth;
	pexColourType colourType B16;
	CARD16 pad B16;
	pexRgb8Colour rgb8;
} pexEdgeBundleDataRgb8;

typedef struct {
	pexSwitch edges;
	CARD8 unused;
	pexEnumTypeIndex edgeType B16;
	PEXFLOAT edgeWidth;
	pexColourType colourType B16;
	CARD16 pad B16;
	pexRgb16Colour rgb16;
} pexEdgeBundleDataRgb16;

typedef struct {
	pexSwitch edges;
	CARD8 unused;
	pexEnumTypeIndex edgeType B16;
	PEXFLOAT edgeWidth;
	pexColourType colourType B16;
	CARD16 pad B16;
	pexFloatColour colorFloat;
} pexEdgeBundleDataFloat;

/* data formats of color entries */

typedef struct {
	pexColourType colourType B16;
	CARD16 pad1 B16;
	PEXFLOAT red;
	PEXFLOAT green;
	PEXFLOAT blue;
} pexColourEntryRgb;

typedef struct {
	pexColourType colourType B16;
	CARD16 pad1 B16;
	PEXFLOAT hue;
	PEXFLOAT saturation;
	PEXFLOAT value;
} pexColourEntryHsv;

typedef struct {
	pexColourType colourType B16;
	CARD16 pad1 B16;
	PEXFLOAT hue;
	PEXFLOAT lightness;
	PEXFLOAT saturation;
} pexColourEntryHls;

typedef struct {
	pexColourType colourType B16;
	CARD16 pad1 B16;
	PEXFLOAT x;
	PEXFLOAT y;
	PEXFLOAT z;
} pexColourEntryCie;

typedef struct {
	pexColourType colourType B16;
	CARD16 pad1 B16;
	CARD8 red; 
	CARD8 green;
	CARD8 blue;
} pexColourEntryRgb8;

typedef struct {
	pexColourType colourType B16;
	CARD16 pad1 B16;
	CARD16 red; 
	CARD16 green;
	CARD16 blue;
} pexColourEntryRgb16;

typedef struct {
	pexColourType colourType B16;
	CARD16 pad1 B16;
	PEXFLOAT first;
	PEXFLOAT second;
	PEXFLOAT third;
} pexColourEntryFloat;

typedef struct {
	pexEnumTypeIndex    lightType B16;
	INT16       unused B16;
	pexVector3D     direction;
	pexCoord3D      point;
	PEXFLOAT       concentration;
	PEXFLOAT       spreadAngle;
	PEXFLOAT       attenuation1;
	PEXFLOAT       attenuation2;
	pexColourType colourType B16;
	CARD16 pad1 B16;
	pexTableIndex colourIndex B16;
	CARD16 pad2 B16;
} pexLightDataIndexed;

typedef struct {
	pexEnumTypeIndex    lightType B16;
	INT16       unused B16;
	pexVector3D     direction;
	pexCoord3D      point;
	PEXFLOAT       concentration;
	PEXFLOAT       spreadAngle;
	PEXFLOAT       attenuation1;
	PEXFLOAT       attenuation2;
	pexColourType colourType B16;
	CARD16 pad B16;
	pexRgb8Colour rgb8;
} pexLightDataRgb8;

typedef struct {
	pexEnumTypeIndex    lightType B16;
	INT16       unused B16;
	pexVector3D     direction;
	pexCoord3D      point;
	PEXFLOAT       concentration;
	PEXFLOAT       spreadAngle;
	PEXFLOAT       attenuation1;
	PEXFLOAT       attenuation2;
	pexColourType colourType B16;
	CARD16 pad B16;
	pexRgb16Colour rgb16;
} pexLightDataRgb16;

typedef struct {
	pexEnumTypeIndex    lightType B16;
	INT16       unused B16;
	pexVector3D     direction;
	pexCoord3D      point;
	PEXFLOAT       concentration;
	PEXFLOAT       spreadAngle;
	PEXFLOAT       attenuation1;
	PEXFLOAT       attenuation2;
	pexColourType colourType B16;
	CARD16 pad B16;
	pexFloatColour colorFloat;
} pexLightDataFloat;

typedef struct {
	pexSwitch       mode;
	CARD8       unused;
	CARD16      unused2 B16;
	PEXFLOAT       frontPlane;
	PEXFLOAT       backPlane;
	PEXFLOAT       frontScaling;
	PEXFLOAT       backScaling;
	pexColourType colourType B16;
	CARD16 pad1 B16;
	pexTableIndex colourIndex B16;
	CARD16 pad2 B16;
} pexDepthCueDataIndexed;

typedef struct {
	pexSwitch       mode;
	CARD8       unused;
	CARD16      unused2 B16;
	PEXFLOAT       frontPlane;
	PEXFLOAT       backPlane;
	PEXFLOAT       frontScaling;
	PEXFLOAT       backScaling;
	pexColourType colourType B16;
	CARD16 pad B16;
	pexRgb8Colour rgb8;
} pexDepthCueDataRgb8;

typedef struct {
	pexSwitch       mode;
	CARD8       unused;
	CARD16      unused2 B16;
	PEXFLOAT       frontPlane;
	PEXFLOAT       backPlane;
	PEXFLOAT       frontScaling;
	PEXFLOAT       backScaling;
	pexColourType colourType B16;
	CARD16 pad B16;
	pexRgb16Colour rgb16;
} pexDepthCueDataRgb16;

typedef struct {
	pexSwitch       mode;
	CARD8       unused;
	CARD16      unused2 B16;
	PEXFLOAT       frontPlane;
	PEXFLOAT       backPlane;
	PEXFLOAT       frontScaling;
	PEXFLOAT       backScaling;
	pexColourType colourType B16;
	CARD16 pad B16;
	pexFloatColour colorFloat;
} pexDepthCueDataFloat;

/* Protocol Data Records for pexOptData and pexVertex */

typedef struct {
	pexIndexedColour indexed;
} pexOptIndexed; 

typedef struct {
	pexRgbFloatColour rgb;
} pexOptRgb; 

typedef struct {
	pexCieColour cie;
} pexOptCie; 

typedef struct {
	pexHsvColour hsv;
} pexOptHsv; 

typedef struct {
	pexHlsColour hls;
} pexOptHls; 

typedef struct {
	pexRgb8Colour rgb8;
} pexOptRgb8; 

typedef struct {
	pexRgb16Colour rgb16;
} pexOptRgb16; 

typedef struct {
	pexVector3D normal;
} pexOptNormal; 

typedef struct {
	pexIndexedColour indexed;
	pexVector3D normal;
} pexOptIndexedNormal; 

typedef struct {
	pexRgbFloatColour rgb;
	pexVector3D normal;
} pexOptRgbNormal; 

typedef struct {
	pexCieColour cie;
	pexVector3D normal;
} pexOptCieNormal; 

typedef struct {
	pexHsvColour hsv;
	pexVector3D normal;
} pexOptHsvNormal; 

typedef struct {
	pexHlsColour hls;
	pexVector3D normal;
} pexOptHlsNormal; 

typedef struct {
	pexRgb8Colour rgb8;
	pexVector3D normal;
} pexOptRgb8Normal; 

typedef struct {
	pexRgb16Colour rgb16;
	pexVector3D normal;
} pexOptRgb16Normal; 

/* --- */

typedef struct {
	pexCoord3D point;
} pexVertexNoOptData; 

typedef struct {
	pexCoord3D point;
	pexIndexedColour indexed;
} pexVertexIndexed; 

typedef struct {
	pexCoord3D point;
	pexRgbFloatColour rgb;
} pexVertexRgb; 

typedef struct {
	pexCoord3D point;
	pexCieColour cie;
} pexVertexCie; 

typedef struct {
	pexCoord3D point;
	pexHsvColour hsv;
} pexVertexHsv; 

typedef struct {
	pexCoord3D point;
	pexHlsColour hls;
} pexVertexHls; 

typedef struct {
	pexCoord3D point;
	pexRgb8Colour rgb8;
} pexVertexRgb8; 

typedef struct {
	pexCoord3D point;
	pexRgb16Colour rgb16;
} pexVertexRgb16; 

typedef struct {
	pexCoord3D point;
	pexVector3D normal;
} pexVertexNormal; 

typedef struct {
	pexCoord3D point;
	CARD32 edges;
} pexVertexEdges; 

typedef struct {
	pexCoord3D point;
	pexIndexedColour indexed;
	pexVector3D normal;
} pexVertexIndexedNormal; 

typedef struct {
	pexCoord3D point;
	pexRgbFloatColour rgb;
	pexVector3D normal;
} pexVertexRgbNormal; 

typedef struct {
	pexCoord3D point;
	pexCieColour cie;
	pexVector3D normal;
} pexVertexCieNormal; 

typedef struct {
	pexCoord3D point;
	pexHsvColour hsv;
	pexVector3D normal;
} pexVertexHsvNormal; 

typedef struct {
	pexCoord3D point;
	pexHlsColour hls;
	pexVector3D normal;
} pexVertexHlsNormal; 

typedef struct {
	pexCoord3D point;
	pexRgb8Colour rgb8;
	pexVector3D normal;
} pexVertexRgb8Normal; 

typedef struct {
	pexCoord3D point;
	pexRgb16Colour rgb16;
	pexVector3D normal;
} pexVertexRgb16Normal; 

typedef struct {
	pexCoord3D point;
	pexIndexedColour indexed;
	CARD32 edges;
} pexVertexIndexedEdges; 

typedef struct {
	pexCoord3D point;
	pexRgbFloatColour rgb;
	CARD32 edges;
} pexVertexRgbEdges; 

typedef struct {
	pexCoord3D point;
	pexCieColour cie;
	CARD32 edges;
} pexVertexCieEdges; 

typedef struct {
	pexCoord3D point;
	pexHsvColour hsv;
	CARD32 edges;
} pexVertexHsvEdges; 

typedef struct {
	pexCoord3D point;
	pexHlsColour hls;
	CARD32 edges;
} pexVertexHlsEdges; 

typedef struct {
	pexCoord3D point;
	pexRgb8Colour rgb8;
	CARD32 edges;
} pexVertexRgb8Edges; 

typedef struct {
	pexCoord3D point;
	pexRgb16Colour rgb16;
	CARD32 edges;
} pexVertexRgb16Edges; 

typedef struct {
	pexCoord3D point;
	pexVector3D normal;
	CARD32 edges;
} pexVertexNormalEdges; 

typedef struct {
	pexCoord3D point;
	pexIndexedColour indexed;
	pexVector3D normal;
	CARD32 edges;
} pexVertexIndexedNormalEdges; 

typedef struct {
	pexCoord3D point;
	pexRgbFloatColour rgb;
	pexVector3D normal;
	CARD32 edges;
} pexVertexRgbNormalEdges; 

typedef struct {
	pexCoord3D point;
	pexCieColour cie;
	pexVector3D normal;
	CARD32 edges;
} pexVertexCieNormalEdges; 

typedef struct {
	pexCoord3D point;
	pexHsvColour hsv;
	pexVector3D normal;
	CARD32 edges;
} pexVertexHsvNormalEdges; 

typedef struct {
	pexCoord3D point;
	pexHlsColour hls;
	pexVector3D normal;
	CARD32 edges;
} pexVertexHlsNormalEdges; 

typedef struct {
	pexCoord3D point;
	pexRgb8Colour rgb8;
	pexVector3D normal;
	CARD32 edges;
} pexVertexRgb8NormalEdges;

typedef struct {
	pexCoord3D point;
	pexRgb16Colour rgb16;
	pexVector3D normal;
	CARD32 edges;
} pexVertexRgb16NormalEdges; 

typedef struct {
	CARD8       reqType;
	CARD8       opcode;
	CARD16      length B16;
	pexEnumTypeIndex    fpFormat B16;
	CARD16      unused B32;
	XID         id B32;
	CARD32      numCommands B32;
	/* LISTof OutputCommand( numCommands ) */
} pexOutputCommandsReq;

/**
 ** The following structure type definitions are useful
 ** in the unpackaging of PEX error data.
 **/

typedef struct {
    CARD8  error;
    CARD8  code;
    CARD16 sequenceNumber;
    CARD32 badColourType;
    CARD16 minorOpcode;
    CARD8  majorOpcode;
    CARD8  unused[21];
} pexColourTypeError;

typedef struct {
    CARD8  error;
    CARD8  code;
    CARD16 sequenceNumber;
    CARD32 badRendererId;
    CARD16 minorOpcode;
    CARD8  majorOpcode;
    CARD8  unused[21];
} pexRendererStateError;

typedef struct {
    CARD8  error;
    CARD8  code;
    CARD16 sequenceNumber;
    CARD32 badFormat;
    CARD16 minorOpcode;
    CARD8  majorOpcode;
    CARD8  unused[21];
} pexFloatingPointFormatError;

typedef struct {
    CARD8  error;
    CARD8  code;
    CARD16 sequenceNumber;
    CARD32 badLabel;
    CARD16 minorOpcode;
    CARD8  majorOpcode;
    CARD8  unused[21];
} pexLabelError;

typedef struct {
    CARD8  error;
    CARD8  code;
    CARD16 sequenceNumber;
    CARD32 badId;
    CARD16 minorOpcode;
    CARD8  majorOpcode;
    CARD8  unused[21];
} pexLookupTableError;

typedef pexLookupTableError pexNamesetError;
typedef pexLookupTableError pexPathError;
typedef pexLookupTableError pexPexFontError;
typedef pexLookupTableError pexPhigsWksError;
typedef pexLookupTableError pexPickMeasureError;
typedef pexLookupTableError pexPipelineContextError;
typedef pexLookupTableError pexRendererError;
typedef pexLookupTableError pexSearchContextError;
typedef pexLookupTableError pexStructureError;

typedef struct {
    CARD8  error;
    CARD8  code;
    CARD16 sequenceNumber;
    CARD32 badId; /* bad renderer ID or structure ID */
    CARD16 minorOpcode;
    CARD8  majorOpcode;
    CARD8  pad;
    CARD16 opcodeOutputCmd; 
    CARD16 numProcessed;
    CARD8  unused[16];
} pexOutputCmdError;

#endif /* PEX_DATA_H_INCLUDED */
