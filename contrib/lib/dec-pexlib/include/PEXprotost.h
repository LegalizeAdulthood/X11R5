/*	@(#)PEXprotost.h 5.1 91/02/12	*/


/***********************************************************
Copyright 1989, 1990, 1991 by Sun Microsystems, Inc. and the X Consortium.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Sun Microsystems,
the X Consortium, and MIT not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.  

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#ifndef PEXPROTOSTR_H
#define PEXPROTOSTR_H

/* Matches revision 5.0P */

#include <X11/Xmd.h>			/* defines things like CARD32 */

#include "floatdef.h"			/* defines type "FLOAT" */

typedef CARD32  pexAsfAttribute;
typedef CARD8	pexAsfValue;
typedef CARD32	pexBitmask;
typedef CARD16	pexBitmaskShort;
typedef CARD16  pexCoordType; 	/* rational, nonrational */
typedef CARD16	pexComposition;
typedef CARD16	pexCullMode;
typedef BYTE 	pexDynamicType;
typedef INT16	pexEnumTypeIndex;
typedef XID 	pexLookupTable;
typedef CARD32 	pexName;
typedef XID 	pexNameSet;
typedef XID	pexPC;
typedef XID	pexFont;
typedef FLOAT	pexMatrix[4][4];
typedef FLOAT 	pexMatrix3X3[3][3];
typedef XID	pexPhigsWks;
typedef XID	pexPickMeasure;
typedef XID	pexRenderer;
typedef XID	pexSC;
typedef XID	pexStructure;
typedef CARD8	pexSwitch;
typedef CARD16	pexTableIndex;
typedef CARD16	pexTableType;	/* could be smaller if it ever helps */
typedef CARD16	pexTextHAlignment;
typedef CARD16	pexTextVAlignment;
typedef CARD16	pexTypeOrTableIndex;
typedef pexEnumTypeIndex	pexColourType; 	/* ColourType */

/* included in others */
typedef struct {
    CARD16	length B16;
    /* list of CARD8 -- don't swap */
} pexString;

typedef struct {
    pexStructure	sid B32;
    FLOAT		priority;
} pexStructureInfo;

typedef struct {
    FLOAT	x;
    FLOAT	y;
} pexVector2D;

typedef struct {
    FLOAT	x;
    FLOAT	y;
    FLOAT	z;
} pexVector3D;

/* Coord structures */

typedef struct {
    FLOAT	x;
    FLOAT	y;
} pexCoord2D;

typedef struct {
    FLOAT	x;
    FLOAT	y;
    FLOAT	z;
} pexCoord3D;

typedef struct {
    FLOAT	x;
    FLOAT	y;
    FLOAT	z;
    FLOAT	w;
} pexCoord4D;


/* Colour structures */
typedef struct {
    FLOAT	red;
    FLOAT	green;
    FLOAT	blue;
} pexRgbFloatColour;

typedef struct {
    FLOAT	hue;
    FLOAT	saturation;
    FLOAT	value;
} pexHsvColour;

typedef struct {
    FLOAT	hue;
    FLOAT	lightness;
    FLOAT	saturation;
} pexHlsColour;

typedef struct {
    FLOAT	x;
    FLOAT	y;
    FLOAT	z;
} pexCieColour;

typedef struct {
    CARD8	red;
    CARD8	green;
    CARD8	blue;
    CARD8	pad;
} pexRgb8Colour;

typedef struct {
    CARD16	red B16;
    CARD16	green B16;
    CARD16	blue B16;
    CARD16	pad B16;
} pexRgb16Colour;

typedef struct {
    pexTableIndex	index B16;
    CARD16		pad B16;
} pexIndexedColour;

typedef struct {
    union {
	pexIndexedColour	indexed;
	pexRgb8Colour		rgb8;
	pexRgb16Colour		rgb16;
	pexRgbFloatColour	rgbFloat;
	pexHsvColour		hsvFloat;
	pexHlsColour		hlsFloat;
	pexCieColour		cieFloat;
    } format;
} pexColour;

typedef struct {
    FLOAT   first;
    FLOAT   second;
    FLOAT   third;
} pexFloatColour;

typedef struct {
    pexColourType	colourType B16;	/* ColourType enumerated type */
    CARD16		unused B16;
    /* SINGLE COLOUR(colourType) */
} pexColourSpecifier;


typedef struct {
    pexEnumTypeIndex	approxMethod B16;
    CARD16		unused B16;
    FLOAT		tolerance;
} pexCurveApprox;

typedef struct {
    INT16	x B16;
    INT16 	y B16;
    FLOAT 	z;
} pexDeviceCoord;

typedef struct {
    INT16	x B16;
    INT16 	y B16;
} pexDeviceCoord2D;

typedef struct {
    INT16	xmin B16;
    INT16	ymin B16;
    INT16	xmax B16;
    INT16	ymax B16;
} pexDeviceRect;

typedef struct {
    CARD16	elementType B16;
    CARD16	length B16;
} pexElementInfo;

typedef struct {
    CARD16	whence B16;
    CARD16	unused B16;
    INT32	offset B32;
} pexElementPos;

typedef struct {
    pexElementPos	position1;
    pexElementPos	position2;
} pexElementRange;

typedef struct {
    pexStructure	structure B32;
    CARD32		offset B32;
} pexElementRef;

typedef struct {
	pexCoord2D lowerLeft;
	pexCoord2D upperRight;
	pexCoord2D concatpoint;
} pexExtentInfo;

typedef struct {
    pexEnumTypeIndex	index B16;
    pexString		descriptor;
} pexEnumTypeDesc;

typedef struct {
    pexCoord3D	point;
    pexVector3D	vector;
} pexHalfSpace;

typedef struct {
    pexNameSet	incl;
    pexNameSet	excl;
} pexNameSetPair;

typedef struct {
    pexCoord2D	point;
    pexVector2D	vector;
} pexHalfSpace2D;

typedef struct {
    CARD16	composition B16;
    CARD16	unused B16;
    pexMatrix	matrix;
} pexLocalTransform3DData;

typedef struct {
    CARD16		composition B16;
    CARD16		unused B16;
    pexMatrix3X3	matrix;
} pexLocalTransform2DData;

typedef struct {
    pexCoord3D	minval;
    pexCoord3D	maxval;
} pexNpcSubvolume;

/*  an OPT_DATA  structure cannot be defined because it has variable content
 *  and size.  An union structure could be used to define a template for
 *  the data. However, since unions pad to a fixed amount of space and the
 *  protocol uses variable lengths, this is not appropriate for protocol
 *  data types.  The most correct way of defining this data is to define
 *  one data structure for every possible combination of color, normal and
 *  edge data that could be given with a vertex or facet.
 */

typedef struct {
    pexStructure	sid B32;
    CARD32		offset B32;
    CARD32		pickid B32;
} pexPickPath;

typedef struct {
    pexTextVAlignment		vertical B16;
    pexTextHAlignment		horizontal B16;
} pexTextAlignmentData;

typedef struct {
    pexSwitch		visibility;
    CARD8		unused;
    CARD16		order B16;
    pexCoordType	type B16;
    INT16		approxMethod B16;
    FLOAT		tolerance;
    FLOAT		tMin;
    FLOAT		tMax;
    CARD32		numKnots B32;
    CARD32		numCoord B32;
    /* LISTof FLOAT(numKnots) -- length = order + number of coords */
    /* LISTof {pexCoord3D|pexCoord4D}(numCoord) */
} pexTrimCurve;

typedef struct {
    FLOAT		ambient;
    FLOAT		diffuse;
    FLOAT		specular;
    FLOAT		specularConc;
    FLOAT		transmission;  /* 0.0 = opaque, 1.0 = transparent */
    pexColourSpecifier  specularColour;
    /* SINGLE COLOUR() */
} pexReflectionAttr;

typedef struct {
    pexEnumTypeIndex	approxMethod B16;
    CARD16		unused B16;
    FLOAT		uTolerance;
    FLOAT		vTolerance;
} pexSurfaceApprox;


typedef struct {
    pexCoord3D	point;
    /* SINGLE OPT_DATA() */
} pexVertex;


typedef struct {
    pexDeviceCoord	minval;
    pexDeviceCoord	maxval;
    pexSwitch		useDrawable;
    BYTE		pad[3];
} pexViewport;

typedef struct {
    CARD16		clipFlags B16;
    CARD16		unused B16;
    pexNpcSubvolume	clipLimits;
    pexMatrix		orientation;
    pexMatrix		mapping;
} pexViewEntry;

typedef struct {
    pexTableIndex	index B16;
    CARD16		unused B16;
    pexViewEntry	view;
} pexViewRep;

/*
 * typedefs for lookup tables
 */

typedef struct {
    CARD16	definableEntries B16;
    CARD16	numPredefined B16;
    CARD16	predefinedMin B16;
    CARD16	predefinedMax B16;
} pexTableInfo;

typedef struct {
    pexEnumTypeIndex	lineType B16;
    pexEnumTypeIndex	polylineInterp B16;
    pexCurveApprox	curveApprox;
    FLOAT		lineWidth;
    pexColourSpecifier	lineColour;
    /* SINGLE COLOUR() */
} pexLineBundleEntry;

typedef struct {
    pexEnumTypeIndex	markerType B16;
    INT16		unused B16;
    FLOAT		markerScale;
    pexColourSpecifier	markerColour;
    /* SINGLE COLOUR() */
} pexMarkerBundleEntry;

typedef struct {
    CARD16		textFontIndex B16;
    CARD16		textPrecision B16;
    FLOAT		charExpansion;
    FLOAT		charSpacing;
    pexColourSpecifier	textColour;
    /* SINGLE COLOUR() */
} pexTextBundleEntry;


/*
    Note that since an InteriorBundleEntry contains 4 embedded instances of 
    pexColourSpecifier, a variable-sized item, a data structure cannot be
    defined for it.
*/
typedef struct {
    pexEnumTypeIndex    interiorStyle B16;
    INT16		interiorStyleIndex B16;
    pexEnumTypeIndex    reflectionModel B16;
    pexEnumTypeIndex    surfaceInterp B16;
    pexEnumTypeIndex    bfInteriorStyle B16;
    INT16		bfInteriorStyleIndex B16;
    pexEnumTypeIndex    bfReflectionModel B16;
    pexEnumTypeIndex    bfSurfaceInterp B16;
    pexSurfaceApprox    surfaceApprox;
    /* SINGLE pexColourSpecifier		surfaceColour    */
    /* SINGLE pexReflectionAttr			reflectionAttr   */
    /* SINGLE pexColourSpecifier		bfSurfaceColour  */
    /* SINGLE pexReflectionAttr			bfReflectionAttr */
} pexInteriorBundleEntry;

typedef struct {
    pexSwitch		edges;
    CARD8		unused;
    pexEnumTypeIndex	edgeType B16;
    FLOAT		edgeWidth;
    pexColourSpecifier	edgeColour;
    /* SINGLE COLOUR() */
} pexEdgeBundleEntry;

typedef struct {
    pexColourType	colourType B16; 
    CARD16		numx B16;
    CARD16		numy B16;
    CARD16		unused B16;
    /* LISTof Colour(numx, numy) 2D array of colours */
} pexPatternEntry;

/* a pexColourEntry is just a pexColourSpecifier
*/

typedef struct {
    CARD32	numFonts B32;
    /* LISTof pexFont( numFonts ) */
} pexTextFontEntry;

/* a pexViewEntry is defined above */

typedef struct {
    pexEnumTypeIndex	lightType B16;
    INT16		unused B16;
    pexVector3D		direction;
    pexCoord3D		point;
    FLOAT		concentration;
    FLOAT		spreadAngle;
    FLOAT		attenuation1;
    FLOAT		attenuation2;
    pexColourSpecifier	lightColour;
    /* SINGLE COLOUR() */
} pexLightEntry;

typedef struct {
    pexSwitch		mode;
    CARD8		unused;
    CARD16		unused2 B16;
    FLOAT		frontPlane;
    FLOAT		backPlane;
    FLOAT		frontScaling;
    FLOAT		backScaling;
    pexColourSpecifier	depthCueColour;
    /* SINGLE COLOUR() */
} pexDepthCueEntry;

typedef struct {
    INT16	approxType;
    INT16	approxModel;
    CARD16	max1 B16;
    CARD16	max2 B16;
    CARD16	max3 B16;
    CARD8	dither;
    CARD8	unused;
    CARD32	mult1 B32;
    CARD32	mult2 B32;
    CARD32	mult3 B32;
    FLOAT	weight1;
    FLOAT	weight2;
    FLOAT	weight3;
    CARD32	basePixel B32;
} pexColourApproxEntry;


/*  Font structures */

typedef struct {
    Atom	name B32;
    CARD32	value B32;
} pexFontProp;

typedef struct {
    CARD32	firstGlyph B32;
    CARD32	lastGlyph B32;
    CARD32	defaultGlyph B32;
    pexSwitch	allExist;
    pexSwitch	strokeFont;
    CARD16	unused B16;
    CARD32	numProps B32;
    /* LISTof pexFontProp(numProps) */
} pexFontInfo;


/* Text Structures */

typedef struct {
    INT16	characterSet B16;
    CARD8	characterSetWidth;
    CARD8	encodingState;
    CARD16	unused B16;
    CARD16	numChars;
    /* LISTof CHARACTER( numChars ) */
    /* pad */
} pexMonoEncoding;

/* CHARACTER is either a CARD8, a CARD16, or a CARD32 */


/* Parametric Surface Characteristics types */

/* type 1 None */

/* type 2 Implementation Dependent */

typedef struct {
    CARD16	placementType B16;
    CARD16	unused B16;
    CARD16	numUcurves B16;
    CARD16	numVcurves B16;
} pexPSC_IsoparametricCurves;		/* type 3 */

typedef struct {
    pexCoord3D	    origin;
    pexVector3D	    direction;
    CARD16	    numberIntersections B16;
    CARD16	    pad B16;
    /* LISTof pexCoord3D( numIntersections ) */
} pexPSC_LevelCurves;			/*  type 4: MC
					    type 5: WC */

/* Pick Device data records */

typedef struct {
    pexDeviceCoord2D	position;
    FLOAT		distance;
} pexPD_DC_HitBox;				/* pick device 1 */

typedef pexNpcSubvolume pexPD_NPC_HitVolume;	/* pick device 2 */


/* Output Command errors */

typedef struct {
    CARD8	type;		    /*  0 */
    CARD8	errorCode;	    /* 14 */
    CARD16	sequenceNumber B16;
    CARD32	resourceId B32;	    /* renderer or structure */
    CARD16	minorCode B16;
    CARD8	majorCode;
    CARD8	unused;
    CARD16	opcode B16;	    /* opcode of failed output command */
    CARD16	numCommands B16;    /* number successfully done before error */
    BYTE	pad[16];
} pexOutputCommandError;



#endif /* PEXPROTOSTR_H */
