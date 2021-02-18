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


#ifndef PEXLIB_H
#define PEXLIB_H

#include <X11/Xlib.h>
#ifndef PEX_H
#  include "PEX.h"
#endif
#ifndef PEXPROTO_H
#  include "PEXproto.h"
#endif
#ifndef PEXPROTOSTR_H
#  include "PEXprotost.h"
#endif

/* 
 * When the DECwindows Xlib.h file defines ConnectionNumber to be dpy->fd
 * again, rather than dpy->efn, this section can be removed. 
 */
#ifdef VMS
#undef ConnectionNumber
#define ConnectionNumber(dpy) ((dpy)->fd)
#endif

/*
 * PEXlib constants  
 */

/* for PEXRotationMatrix */

#define pxlXAxis   1
#define pxlYAxis   2
#define pxlZAxis   3

/* output command buffer types */

#define pxlRenderImmediate PEX_RenderOutputCommands
#define pxlAddToStructure  PEX_StoreElements

/*
 * Following are the typedefs used in the PEXlib interface. pxlFooBar
 * definitions are always supersets of the pexFooBar protocol definitions. 
 * Note that most pxlFooBar definitions are equivalent to the pexFooBar
 * protocol definitions.  However some of the definitions do have extra
 * fields. The extra fields are used to access data that would normally 
 * follow a pexFooBar in the protocol.  pxlFooBar definitions should
 * *never* contain fields which are not required in the protocol.  This
 * means pexlib can copy pxlFooBar data into the protocol stream.  Sometimes 
 * the data which follows a pexFooBar is variable length.  For example 
 *
 *   	typedef struct pxlColourSpecifier
 *	{
 *    	    pxlColourType       colourType;
 *  	    CARD16              unused;
 *    	    pxlColour           colour;
 *	} pxlColourSpecifier;
 *
 *	typedef struct pexColourSpecifier
 *	{
 *    	    pexColourType       colourType;
 *    	    CARD16              unused;
 *    	    ( SINGLE COLOUR(colourType) ) 
 *	} pexColourSpecifier;
 *
 * The pxlColorSpecifier has an extra field for accessing the colour data.
 * Note that the size of 'colour' depends on the colour type.  (ie: pxlColour 
 * and pexColour are defined as a union of all possible colour types)
 * Using a pxlColourSpecifier provides the application easy access to the 
 * colour data.  However PEXlib must pack the colour data before sending it 
 * down to the pex server.
 *
 */

/*
 * PEXlib definitions used globally 
 */

typedef pexAsfAttribute 	pxlAsfAttribute;
typedef pexAsfValue 		pxlAsfValue;
typedef pexBitmask 		pxlBitmask;
typedef pexBitmaskShort 	pxlBitmaskShort;
typedef pexCoordType 		pxlCoordType;
typedef pexComposition 		pxlComposition;
typedef pexCullMode 		pxlCullMode;
typedef pexDynamicType 		pxlDynamicType;
typedef pexEnumTypeIndex 	pxlEnumTypeIndex;
typedef pexLookupTable 		pxlLookupTable;
typedef pexName 		pxlName;
typedef pexNameSet 		pxlNameSet;
typedef pexPC 			pxlPipelineContext;
typedef pexFont 		pxlFont;
typedef pexMatrix 		pxlMatrix;
typedef pexMatrix3X3 		pxlMatrix3X3;
typedef pexRenderer 		pxlRenderer;
typedef pexStructure 		pxlStructure;
typedef pexSwitch 		pxlSwitch;
typedef pexTableIndex 		pxlTableIndex;
typedef pexTableType 		pxlTableType;
typedef pexTextHAlignment 	pxlTextHAlignment;
typedef pexTextVAlignment 	pxlTextVAlignment;
typedef pexTypeOrTableIndex 	pxlTypeOrTableIndex;
typedef pexColourType		pxlColourType;

typedef pexString               pxlString;
typedef pexStructureInfo        pxlStructureInfo;
typedef pexVector2D             pxlVector2D;
typedef pexVector3D             pxlVector3D;

typedef pexCurveApprox 		pxlCurveApprox;
typedef pexDeviceRect 		pxlDeviceRect;
typedef pexElementInfo 		pxlElementInfo;
typedef pexElementPos 		pxlElementPos;
typedef pexElementRange 	pxlElementRange;
typedef pexElementRef 		pxlElementRef;
typedef pexExtentInfo 		pxlExtentInfo;
typedef pexEnumTypeDesc 	pxlEnumTypeDesc;
typedef pexHalfSpace 		pxlHalfSpace;
typedef pexNameSetPair 		pxlNameSetPair;
typedef pexHalfSpace2D 		pxlHalfSpace2D;
typedef pexLocalTransform3DData pxlLocalTransform3DData;
typedef pexLocalTransform2DData pxlLocalTransform2DData;
typedef pexNpcSubvolume 	pxlNpcSubvolume;
typedef pexPickPath 		pxlPickPath;
typedef pexTextAlignmentData 	pxlTextAlignmentData;
typedef pexTrimCurve 		pxlTrimCurve;
typedef pexSurfaceApprox 	pxlSurfaceApprox;
typedef pexVertex 		pxlVertex;
typedef pexViewport 		pxlViewport;
typedef pexViewRep 		pxlViewRep;
typedef pexMonoEncoding         pxlMonoEncoding;
typedef pexPSC_IsoparametricCurves pxlPSC_IsoparametricCurves;
typedef pexPSC_LevelCurves      pxlPSC_LevelCurves;

typedef XID 			pxlSearchContext;
typedef pexPhigsWks		pxlPhigsWks;

/* 
 * Coordinate definitions 
 */

typedef pexCoord2D              pxlCoord2D;
typedef pexCoord3D              pxlCoord3D;
typedef pexCoord4D              pxlCoord4D;
typedef pexDeviceCoord          pxlDeviceCoord;
typedef pexDeviceCoord2D        pxlDeviceCoord2D;

/*  
 * Colour definitions 
 */

typedef pexRgbFloatColour       pxlRgbFloatColour;
typedef pexHsvColour            pxlHsvColour;
typedef pexHlsColour            pxlHlsColour;
typedef pexCieColour            pxlCieColour;
typedef pexRgb8Colour           pxlRgb8Colour;
typedef pexRgb16Colour          pxlRgb16Colour;
typedef pexIndexedColour        pxlIndexedColour;
typedef pexColour               pxlColour;
typedef pexFloatColour          pxlFloatColour;

typedef struct pxlColourSpecifier
{
    pxlColourType       colourType;
    CARD16              unused;
    pxlColour           colour;
} pxlColourSpecifier;


/* 
 * reflection attributes 
 */

typedef struct pxlReflectionAttr
{
    FLOAT               ambient;
    FLOAT               diffuse;
    FLOAT               specular;
    FLOAT               specularConc;
    FLOAT               transmission;
    pxlColourSpecifier  specularColour;
} pxlReflectionAttr;


/*
 * Definitions used for output commands
 */

typedef pexMarkerType			pxlMarkerType;
typedef pexMarkerScale			pxlMarkerScale;
typedef pexMarkerColourIndex		pxlMarkerColourIndex;
typedef pexMarkerColour			pxlMarkerColour;
typedef pexMarkerBundleIndex		pxlMarkerBundleIndex;
typedef pexTextFontIndex		pxlTextFontIndex;
typedef pexTextPrecision		pxlTextPrecision;
typedef pexCharExpansion		pxlCharExpansion;
typedef pexCharSpacing			pxlCharSpacing;
typedef pexTextColourIndex		pxlTextColourIndex;
typedef pexTextColour			pxlTextColour;
typedef pexCharHeight			pxlCharHeight;
typedef pexCharUpVector			pxlCharUpVector;
typedef pexTextPath			pxlTextPath;
typedef pexTextAlignment		pxlTextAlignment;
typedef pexAtextHeight			pxlAtextHeight;
typedef pexAtextUpVector		pxlAtextUpVector;
typedef pexAtextPath			pxlAtextPath;
typedef pexAtextAlignment		pxlAtextAlignment;
typedef pexAtextStyle			pxlAtextStyle;
typedef pexTextBundleIndex		pxlTextBundleIndex;
typedef pexLineType			pxlLineType;
typedef pexLineWidth			pxlLineWidth;
typedef pexLineColourIndex            	pxlLineColourIndex;
typedef pexLineColour         		pxlLineColour;
typedef pexCurveApproximation		pxlCurveApproximation;
typedef pexPolylineInterp		pxlPolylineInterp;
typedef pexLineBundleIndex              pxlLineBundleIndex;
typedef pexInteriorStyle		pxlInteriorStyle;
typedef pexInteriorStyleIndex		pxlInteriorStyleIndex;
typedef pexSurfaceColourIndex           pxlSurfaceColourIndex;
typedef pexSurfaceColour         	pxlSurfaceColour;
typedef pexSurfaceReflAttr		pxlSurfaceReflAttr;
typedef pexSurfaceReflModel		pxlSurfaceReflModel;
typedef pexSurfaceInterp		pxlSurfaceInterp;
typedef pexBfInteriorStyle		pxlBfInteriorStyle;
typedef pexBfInteriorStyleIndex		pxlBfInteriorStyleIndex;
typedef pexBfSurfaceColourIndex         pxlBfSurfaceColourIndex;
typedef pexBfSurfaceColour         	pxlBfSurfaceColour;
typedef pexBfSurfaceReflAttr		pxlBfSurfaceReflAttr;
typedef pexBfSurfaceReflModel		pxlBfSurfaceReflModel;
typedef pexBfSurfaceInterp		pxlBfSurfaceInterp;
typedef pexSurfaceApproximation		pxlSurfaceApproximation;
typedef pexCullingMode			pxlCullingMode;
typedef pexDistinguishFlag		pxlDistinguishFlag;
typedef pexPatternSize			pxlPatternSize;
typedef pexPatternRefPt			pxlPatternRefPt;
typedef pexPatternAttr			pxlPatternAttr;
typedef pexInteriorBundleIndex		pxlInteriorBundleIndex;
typedef pexSurfaceEdgeFlag		pxlSurfaceEdgeFlag;
typedef pexSurfaceEdgeType		pxlSurfaceEdgeType;
typedef pexSurfaceEdgeWidth 		pxlSurfaceEdgeWidth;
typedef pexSurfaceEdgeColourIndex       pxlSurfaceEdgeColourIndex;
typedef pexSurfaceEdgeColour         	pxlSurfaceEdgeColour;
typedef pexEdgeBundleIndex              pxlEdgeBundleIndex;
typedef pexSetAsfValues			pxlSetAsfValues;
typedef pexLocalTransform		pxlLocalTransform;
typedef pexLocalTransform2D		pxlLocalTransform2D;
typedef pexGlobalTransform		pxlGlobalTransform;
typedef pexGlobalTransform2D		pxlGlobalTransform2D;
typedef pexModelClip			pxlModelClip;
typedef pexModelClipVolume		pxlModelClipVolume;
typedef pexModelClipVolume2D		pxlModelClipVolume2D;
typedef pexRestoreModelClip		pxlRestoreModelClip;
typedef pexViewIndex			pxlViewIndex;
typedef pexLightState			pxlLightState;
typedef pexDepthCueIndex		pxlDepthCueIndex;
typedef pexPickId			pxlPickId;
typedef pexHlhsrIdentifier		pxlHlhsrIdentifier;
typedef pexColourApproxIndex		pxlColourApproxIndex;
typedef pexRenderingColourModel		pxlRenderingColourModel;
typedef pexParaSurfCharacteristics	pxlParaSurfCharacteristics;
typedef pexAddToNameSet			pxlAddToNameSet;
typedef pexRemoveFromNameSet		pxlRemoveFromNameSet;
typedef pexExecuteStructure		pxlExecuteStructure;
typedef pexLabel			pxlLabel;
typedef pexApplicationData		pxlApplicationData;
typedef pexGse				pxlGse;
typedef pexMarker			pxlMarker;
typedef pexMarker2D			pxlMarker2D;
typedef pexText				pxlText;
typedef pexText2D			pxlText2D;
typedef pexAnnotationText		pxlAnnotationText;
typedef pexAnnotationText2D		pxlAnnotationText2D;
typedef pexPolyline			pxlPolyline;
typedef pexPolyline2D			pxlPolyline2D;
typedef pexPolylineSet			pxlPolylineSet;
typedef pexNurbCurve			pxlNurbCurve;
typedef pexFillArea			pxlFillArea;
typedef pexFillArea2D			pxlFillArea2D;
typedef pexExtFillArea			pxlExtFillArea;
typedef pexFillAreaSet			pxlFillAreaSet;
typedef pexFillAreaSet2D		pxlFillAreaSet2D;
typedef pexExtFillAreaSet		pxlExtFillAreaSet;
typedef pexTriangleStrip		pxlTriangleStrip;
typedef pexQuadrilateralMesh		pxlQuadrilateralMesh;
typedef pexSOFAS			pxlSOFAS;
typedef pexNurbSurface			pxlNurbSurface;
typedef pexCellArray			pxlCellArray;
typedef pexCellArray2D			pxlCellArray2D;
typedef pexExtCellArray			pxlExtCellArray;
typedef pexGdp				pxlGdp;
typedef pexGdp2D			pxlGdp2D;

typedef struct pxlAsfData
{
    unsigned long       attribute;
    unsigned char	value;
    unsigned char	pad[3];
} pxlAsfData;

typedef struct pxlStringData
{
    int                 numChars;
    char                *chars;
} pxlStringData;

typedef struct pxlMonoEncodedTextData
{
    INT16       characterSet;
    CARD8       characterSetWidth;
    CARD8       encodingState;
    CARD16      unused;
    CARD16      numChars;
    char	*chars;
} pxlMonoEncodedTextData;

typedef struct pxlPolylineData
{
    int                 numPoints;
    char                *points;
} pxlPolylineData;

typedef struct pxlPolygonData
{
    char                *facetData;
    int                 numPoints;
    char                *points;
} pxlPolygonData;

typedef struct pxlContourData
{
   int			numPoints;
   char			*points;
} pxlContourData;

typedef struct pxlComplexPolygonData
{
    char		*facetData;
    int                 numPolygons;
    pxlContourData      *polygons;
} pxlComplexPolygonData;

typedef struct pxlTrimCurveData
{
    int                 visibility;
    int                 order;
    int                 type;
    int                 approxMethod;
    float               tolerance;
    float               tmin;
    float               tmax;
    int                 numPoints;
    float               *knots;
    char                *points;
} pxlTrimCurveData;

typedef struct pxlTrimLoopData
{
    int			numTrimCurves;
    pxlTrimCurveData	*trimCurves;
} pxlTrimLoopData;

typedef struct pxlNurbSurfaceData
{
    unsigned short      type;
    unsigned short      uorder;
    unsigned short      vorder;
    unsigned short      numTrimLoops;
    unsigned short	numMPoints;
    unsigned short	numNPoints;
    float               *uknots;
    float               *vknots;
    char                *points;
    pxlTrimLoopData     *trimLoop;
} pxlNurbSurfaceData;

typedef pexOutputCommandError   pxlOutputCommandError;


/*
 * Definitions used for output commands
 * Note that these MUST match the equivalent DEC PHIGS structures 
 */

typedef struct pxlPhigsPointList23
{
    int         number;
    int         type;
    char        *pts23;
} pxlPhigsPointList23;

typedef struct pxlPhigsFloatList
{
    int         number;
    float       *floats;
} pxlPhigsFloatList;

typedef struct pxlPhigsTrimCurve
{
    int                 visflag;
    int                 order;
    pxlPhigsFloatList   trimKnot;
    pxlPhigsPointList23 ctlpts;
    float               tmin;
    float               tmax;
} pxlPhigsTrimCurve;

typedef struct pxlPhigsTrimList
{
    int                 number;
    pxlPhigsTrimCurve   *trim;
} pxlPhigsTrimList;


/*
 * Definitions for lookup tables 
 */

typedef pexTableInfo            pxlTableInfo;

typedef struct pxlLineBundleEntry
{
    pxlEnumTypeIndex	lineType;
    pxlEnumTypeIndex	polylineInterp;
    pxlCurveApprox	curveApprox;
    FLOAT		lineWidth;
    pxlColourSpecifier	lineColour;
} pxlLineBundleEntry;

typedef struct pxlMarkerBundleEntry
{
    pxlEnumTypeIndex	markerType;
    INT16		unused;
    FLOAT		markerScale;
    pxlColourSpecifier	markerColour;
} pxlMarkerBundleEntry;

typedef struct pxlTextBundleEntry
{
    CARD16		textFontIndex;
    CARD16		textPrecision;
    FLOAT		charExpansion;
    FLOAT		charSpacing;
    pxlColourSpecifier	textColour;
} pxlTextBundleEntry;

typedef struct pxlInteriorBundleEntry
{
    pxlEnumTypeIndex    interiorStyle;
    INT16		interiorStyleIndex;
    pxlEnumTypeIndex    reflectionModel;
    pxlEnumTypeIndex    surfaceInterp;
    pxlEnumTypeIndex    bfInteriorStyle;
    INT16		bfInteriorStyleIndex;
    pxlEnumTypeIndex    bfReflectionModel;
    pxlEnumTypeIndex    bfSurfaceInterp;
    pxlSurfaceApprox    surfaceApprox;
    pxlColourSpecifier	surfaceColour;
    pxlReflectionAttr	reflectionAttr;
    pxlColourSpecifier	bfSurfaceColour;
    pxlReflectionAttr	bfReflectionAttr;
} pxlInteriorBundleEntry;

typedef struct pxlEdgeBundleEntry
{
    pxlSwitch		edges;
    CARD8		unused;
    pxlEnumTypeIndex	edgeType;
    FLOAT		edgeWidth;
    pxlColourSpecifier	edgeColour;
} pxlEdgeBundleEntry;

typedef struct pxlPatternEntry
{
    pxlColourType	colourType;
    unsigned short      numx;
    unsigned short      numy;
    CARD16              unused;
    char                *colours; /* ptr to 2D array of colours of type: */
				 /* pxlRgbFloatColour,
				    pxlHsvColour,
				    pxlHlsColour,
				    pxlCieColour,
				    pxlRgb8Colour,
				    pxlRgb16Colour,
				    or short (for PEXIndexedColour colours) */
} pxlPatternEntry;

typedef struct pxlColourEntry
{
    pxlColourSpecifier	colour;
} pxlColourEntry;

typedef struct pxlTextFontEntry
{
    CARD32      numFonts;
    pexFont	*fonts;
} pxlTextFontEntry;

typedef pexViewEntry pxlViewEntry;

typedef struct pxlLightEntry
{
    pxlEnumTypeIndex	lightType;
    INT16		unused;
    pxlVector3D		direction;
    pxlCoord3D		point;
    FLOAT		concentration;
    FLOAT		spreadAngle;
    FLOAT		attenuation1;
    FLOAT		attenuation2;
    pxlColourSpecifier	lightColour;
} pxlLightEntry;

typedef struct pxlDepthCueEntry
{
    pxlSwitch		mode;
    CARD8		unused;
    CARD16		unused2;
    FLOAT		frontPlane;
    FLOAT		backPlane;
    FLOAT		frontScaling;
    FLOAT		backScaling;
    pxlColourSpecifier	depthCueColour;
} pxlDepthCueEntry;

typedef pexColourApproxEntry pxlColourApproxEntry;


/*  
 * Definitions for fonts
 */

typedef pexFontProp             pxlFontProp;
typedef pexFontInfo             pxlFontInfo;
 

/* 
 * Defintions for PEX extension info 
 */

typedef struct pxlInfo
{
    unsigned short      majorVersion;
    unsigned short      minorVersion;
    unsigned long       release;
    unsigned long       subsetInfo;
    char                *vendorName;
} pxlInfo;


/* 
 * Definitions for enumerated type descriptions 
 */

typedef struct pxlEnumTypeDescList
{
    short               enumVal;
    char                *string;
} pxlEnumTypeDescList;


/* 
 * Definitions for pipeline context.
 */

typedef struct pxlLightList
{
    unsigned long	numLights;
    pxlTableIndex	*lightIndex;
} pxlLightList;

typedef struct pxlModelClipVolumeList
{
    int			numHalfSpaces;
    pxlHalfSpace	*halfSpace;
} pxlModelClipVolumeList;

typedef struct pxlPSCData
{
    short		characteristics;
    short               length;
    char		*pscData;
}pxlPSCData;

typedef struct pxlPCAttributes
{
    short               markerType;
    float               markerScale;
    pxlColourSpecifier	markerColour;
    unsigned short      markerBundleIndex;
    unsigned short      textFont;
    unsigned short      textPrecision;
    float               charExpansion;
    float               charSpacing;
    pxlColourSpecifier	textColour;
    float               charHeight;
    pxlVector2D         charUpVector;
    unsigned short      textPath;
    pxlTextAlignmentData textAlignment;
    float               atextHeight;
    pxlVector2D         atextUpVector;
    unsigned short      atextPath;
    pxlTextAlignmentData atextAlignment;
    short               atextStyle;
    unsigned short      textBundleIndex;
    short               lineType;
    float               lineWidth;
    pxlColourSpecifier	lineColour;
    pxlCurveApprox      curveApprox;
    short               polylineInterp;
    unsigned short      lineBundleIndex;
    short               interiorStyle;
    unsigned short      interiorStyleIndex;
    pxlColourSpecifier	surfaceColour;
    pxlReflectionAttr	reflectionAttr;
    short               reflectionModel;
    short               surfaceInterp;
    short               bfInteriorStyle;
    unsigned short      bfInteriorStyleIndex;
    pxlColourSpecifier	bfSurfaceColour;
    pxlReflectionAttr	bfReflectionAttr;
    short               bfReflectionModel;
    short               bfSurfaceInterp;
    pxlSurfaceApprox    surfaceApprox;
    unsigned short      cullingMode;
    unsigned char       distinguish;
    pxlCoord2D          patternSize;
    pxlCoord3D          patternRefPt;
    pxlVector3D         patternRefVec1;
    pxlVector3D         patternRefVec2;
    unsigned short      interiorBundleIndex;
    unsigned short      surfaceEdges;
    short               surfaceEdgeType;
    float               surfaceEdgeWidth;
    pxlColourSpecifier	surfaceEdgeColour;
    unsigned short      edgeBundleIndex;
    pxlMatrix           localTransform;
    pxlMatrix           globalTransform;
    unsigned short      modelClip;
    pxlModelClipVolumeList  modelClipVolume;
    unsigned short      viewIndex;
    pxlLightList	lightState;
    unsigned short      depthCueIndex;
    unsigned long       asfValues[2];
                        /* first word is mask, second is values */
    long		pickId;
    unsigned long       HlhsrIdentifier;
    pxlNameSet          nameSet;
    unsigned short	colourApproxIndex;
    short		rdrColourModel;
    pxlPSCData 		psc;
} pxlPCAttributes;


/* macro for setting bits in a PC value mask */

#define PEX_SetPCAttrMaskBit(mask, attrNum) \
    mask[((attrNum)) >> 5] |= 1L << ( ((attrNum)) & 0x1F)


/* 
 * Definitions for structures 
 */

typedef struct pxlStructureInformation
{
    unsigned long       numElements;
    unsigned long       length;
    unsigned long       numRefs;
    unsigned short      editMode;
    unsigned long       elementPtr;
} pxlStructureInformation;

typedef struct pxlStructurePath
{
    int                 pathLength;
    pxlElementRef       *elementRefs;
} pxlStructurePath;

typedef struct pxlNameSetPairList
{
    int                 numPairs;
    pxlNameSetPair      *pairs;
} pxlNameSetPairList;

/* typedefs for search context attributes. defines in PEX.h */

#define pxlMaxSCShift	6

typedef struct pxlSCAttributes
{
    pxlCoord3D          position;
    float               distance;
    unsigned short      ceiling;
    pxlStructurePath    startPath;
    pxlNameSetPairList  normalList;
    pxlNameSetPairList  invertedList;
} pxlSCAttributes;

/* typedefs for PHIGS workstation info attributes. defines in PEX.h */

typedef struct pxlViewList
{
    unsigned long	numViews;
    unsigned short	*views;
} pxlViewList;

typedef struct pxlStructureInfoList
{
    unsigned long	numStructureInfo;
    pxlStructureInfo	*info;
} pxlStructureInfoList;

typedef struct pxlPhigsWksInfo
{
    short		drawableUpdate;
    unsigned short      visualState;
    unsigned short      drawableSurface;
    unsigned short      viewUpdate;
    pxlViewList		definedViews;
    unsigned short      wksUpdate;
    pxlNpcSubvolume     reqNpcSubvolume;
    pxlNpcSubvolume     curNpcSubvolume;
    pxlViewport         reqWksViewport;
    pxlViewport         curWksViewport;
    unsigned short      hlhsrUpdate;
    unsigned short      reqHlhsrMode;
    unsigned short      curHlhsrMode;
    Drawable            drawable;
    pxlLookupTable      markerBundle;
    pxlLookupTable      textBundle;
    pxlLookupTable      lineBundle;
    pxlLookupTable      interiorBundle;
    pxlLookupTable      edgeBundle;
    pxlLookupTable      colorTable;
    pxlLookupTable      depthCueTable;
    pxlLookupTable	lightTable;
    pxlLookupTable      colorApproxTable;
    pxlLookupTable      patternTable;
    pxlLookupTable      textFontTable;
    pxlNameSet          highlightIncl;
    pxlNameSet          highlightExcl;
    pxlNameSet          invisibilityIncl;
    pxlNameSet          invisibilityExcl;
    pxlStructureInfoList postedStructures;
    unsigned long	numPriorities;
} pxlPhigsWksInfo;


/* typedefs for PHIGS workstation dynamics. defines in PEX.h */

typedef struct pxlPhigsWksDynamics
{
    unsigned char	viewRep;
    unsigned char	markerBundle;
    unsigned char	textBundle;
    unsigned char	lineBundle;
    unsigned char	interiorBundle;
    unsigned char	edgeBundle;
    unsigned char	colorTable;
    unsigned char	patternTable;
    unsigned char	wksTransform;
    unsigned char	highlightFilter;
    unsigned char	invisibilityFilter;
    unsigned char	hlhsrMode;
    unsigned char	structureModify;
    unsigned char	postStructure;
    unsigned char	unpostStructure;
    unsigned char	deleteStructure;
    unsigned char	referenceModify;
} pxlPhigsWksDynamics;

/* 
 * Definitions for renderer 
 */

typedef struct pxlClipList
{
    unsigned long	numDeviceRects;
    pxlDeviceRect	*deviceRect;
} pxlClipList;

typedef struct pxlRendererAttributes
{
    pxlPipelineContext  pipelineContext;
    pxlStructurePath    currentPath;
    pxlLookupTable      markerBundle;
    pxlLookupTable      textBundle;
    pxlLookupTable      lineBundle;
    pxlLookupTable      interiorBundle;
    pxlLookupTable      edgeBundle;
    pxlLookupTable      viewTable;
    pxlLookupTable      colourTable;
    pxlLookupTable      depthCueTable;
    pxlLookupTable      lightTable;
    pxlLookupTable      colourApproxTable;
    pxlLookupTable      patternTable;
    pxlLookupTable      textFontTable;
    pxlNameSet          highlightIncl;
    pxlNameSet          highlightExcl;
    pxlNameSet          invisibilityIncl;
    pxlNameSet          invisibilityExcl;
    unsigned short      rendererState;
    unsigned short      hlhsrMode;
    pxlNpcSubvolume     npcSubvolume;
    pxlViewport         viewport;
    pxlClipList		clipList;
} pxlRendererAttributes;


/*
 * Generic definition of pex OC buffers
 */

typedef struct pxlOCBuf
{
    int                 initLengthRequested; /* hint for size of oc request */
    int                 initLength;
    Display             *display;
    char                extOpcode;   /* opcode for pex extension */
    char                pexOpcode;   /* opcode for ROC or StoreElements */
    short		retainedOCBuffer;   /* 0 for transient, 1 for	     */
					    /* retained			     */
    unsigned long       target;      /* renderer id or structure id */
    unsigned int        *basePtr;    /* points to start of current oc request */
    unsigned int        *curPtr;     /* where to store next oc */
    unsigned int        *bufMax;     /* end of buffer containing oc request */
    int 		ocWordsLeft; /* words left to copy to oc buffer, words
				        allocated via PEXInitOC */
    int 		lrTotalNum;  /* number of packets for large request */
    int 		lrTotalLength; /* size of data for large request */
    int 		lrSequenceNum; /* # of current large request packet */
    int 		lrBufferLength; /* # words avail in a lr packet */
    int 		lrBufferLeft; /* # of words left in current lr packet */
    void		(* OCError)();
    void		(* FreeOCBuf)();
    void		(* ClearOCBuf)();
    void		(* SetOCBufType)();
    void		(* SetOCBufTarget)();
    void		(* GetOCBufType)();
    int			(* GetOCBufFreeSpace)();
    int 		(* GetMaxOCChunkSize)();
    void		(* FlushOCBuf)();
    void		(* SendOCBuf)();
    int			(* StartOC)();
    int			(* StartLargeRequest)();
    void		(* FinishOC)();
    unsigned int	*(* GetOCWords)();
    void		(* CopyOCBytes)();
    void		(* StoreOCList)();
} pxlOCBuf;


#ifndef PEXOCBUF_H
#  include "PEXocbuf.h"
#endif

#ifdef PROTOS
#   include "PEXlibprotos.h"
#else
extern void PEXDefaultOCError();
extern pxlOCBuf *PEXAllocateRetainedOCBuffer();
extern pxlOCBuf *PEXAllocateTransientOCBuffer();
extern void PEXGetDefaultOCBufferType ();
extern void PEXDefaultStoreOCList ();
extern unsigned int * PEXGetWordsError ();
extern void PEXCopyBytesError ();
#endif

#endif /* PEXLIB_H */
