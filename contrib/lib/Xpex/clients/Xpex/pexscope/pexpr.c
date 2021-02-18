/* $Header: pexpr.c,v 2.3 91/09/11 15:52:20 sinyaw Exp $ */
#include <stdio.h>
#include <X11/X.h>
#include "PEX.h"
#include "PEXprotost.h"
#include "PEXproto.h"
#include "Xpexlib.h"

#define PrintUval(_STR,_UVAL) \
(void) fprintf(stderr,"_STR: %u\n",_UVAL)

#define PrintVal(_STR,_VAL) \
(void) fprintf(stderr,"_STR: %d\n",_VAL)

#define PrintFval(_STR,_VAL) \
(void) fprintf(stderr,"_STR: %g\n",_VAL)

#define PrintString(_STR1,_STR2) \
(void) fprintf(stderr,"_STR1: %s\n",_STR2)

#define CR \
(void) fprintf(stderr,"\n")

void
pexpr_element_info( buf)
	register unsigned char *buf;
{
#define VALID_OC(_OC) ( _OC > 0) && ( _OC <= PEXMaxOC)

	static char *oc_name[PEXMaxOC+1] = {
		"All",
		"MarkerType",
		"MarkerScale",
		"MarkerColourIndex",
		"MarkerColour",
		"MarkerBundleIndex",
		"TextFontIndex",
		"TextPrecision",
		"CharExpansion",
		"CharSpacing",
		"TextColourIndex",
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
		"LineColourIndex",
		"LineColour",
		"CurveApproximation",
		"PolylineInterp",
		"LineBundleIndex",
		"InteriorStyle",
		"InteriorStyleIndex",
		"SurfaceColourIndex",
		"SurfaceColour",
		"SurfaceReflAttr",
		"SurfaceReflModel",
		"SurfaceInterp",
		"BfInteriorStyle",
		"BfInteriorStyleIndex",
		"BfSurfaceColourIndex",
		"BfSurfaceColour",
		"BfSurfaceReflAttr",
		"BfSurfaceReflModel",
		"BfSurfaceInterp",
		"SurfaceApproximation",
		"CullingMode",
		"DistinguishFlag",
		"PatternSize",
		"PatternRefPt",
		"PatternAttr",
		"InteriorBundleIndex",
		"SurfaceEdgeFlag",
		"SurfaceEdgeType",
		"SurfaceEdgeWidth",
		"SurfaceEdgeColourIndex",
		"SurfaceEdgeColour",
		"SurfaceEdgeIndex",
		"SetAsfValues",
		"LocalTransform",
		"LocalTransform2D",
		"GlobalTransform",
		"GlobalTransform2D",
		"ModelClip",
		"ModelClipVolume",
		"ModelClipVolume2D",
		"RestoreModelClip",
		"ViewIndex",
		"LightState",
		"DepthCueIndex",
		"PickId",
		"HlhsrIdentifier",
		"ColourApproxIndex",
		"RenderingColourModel",
		"ParaSurfCharacteristics",
		"AddToNameSet",
		"RemoveFromNameSet",
		"ExecuteStructure",
		"Label",
		"ApplicationData",
		"Gse",
		"Marker",
		"Marker2D",
		"Text",
		"Text2D",
		"AnnotationText",
		"AnnotationText2D",
		"Polyline",
		"Polyline2D",
		"PolylineSet",
		"NurbCurve",
		"FillArea",
		"FillArea2D",
		"ExtFillArea",
		"FillAreaSet",
		"FillAreaSet2D",
		"ExtFillAreaSet",
		"TriangleStrip",
		"QuadrilateralMesh",
		"SOFAS",
		"NurbSurface",
		"CellArray",
		"CellArray2D",
		"ExtCellArray",
		"Gdp",
		"Gdp2D"
};

	pexElementInfo *p = (pexElementInfo *) buf;

	if( VALID_OC( p->elementType)) {
		(void) fprintf( stderr, "elementType: %s\n",
			oc_name[p->elementType]);
	} else {
		(void) fprintf( stderr, "elementType: Invalid.\n");
	}
	(void) fprintf( stderr, "length: %u \n", p->length);
#undef VALID_OC
}

void
pexpr_colour_type( type)
	pexColourType type;
{
#define VALID_PEX_COLOUR_TYPE(_TYPE) \
(_TYPE >= PEXIndexedColour) && (_TYPE <= PEXMaxColour)

	static char *ct[PEXMaxColour+1] = {
		"Indexed",
		"RgbFloat",
		"CieFloat",
		"HsvFloat",
		"HlsFloat",
		"Rgb8",
		"Rgb16"
	};

	if( VALID_PEX_COLOUR_TYPE(type)) {
		PrintString(colourType,ct[type]);
	} else {
		PrintString(colourType,"Invalid");
	}

#undef VALID_PEX_COLOUR_TYPE
}

void
pexpr_float_colour( buf)
	register unsigned char *buf;
{
	pexFloatColour *p = (pexFloatColour *) buf;

	(void) fprintf( stderr, "values: ( %g, %g, %g)\n",
	p->first, p->second, p->third);
}

void
pexpr_rgb8_colour( buf)
	register unsigned char *buf;
{
	pexRgb8Colour *p = (pexRgb8Colour *) buf;

	(void) fprintf( stderr, "values: ( %u, %u, %u)\n",
	p->red, p->green, p->blue);
}

void
pexpr_rgb16_colour( buf)
	register unsigned char *buf;
{
	pexRgb16Colour *p = (pexRgb16Colour *) buf;

	(void) fprintf( stderr, "values: ( %u, %u, %u)\n",
	p->red, p->green, p->blue);
}

void
pexpr_colour_value( type, buf)
	pexColourType type;
	register unsigned char *buf;
{
	switch( type) {
	case PEXIndexedColour:
	{
		pexIndexedColour *ic = (pexIndexedColour *) buf;
		PrintUval(index,ic->index);
		break;
	}
	case PEXRgbFloatColour:
	case PEXCieFloatColour:
	case PEXHsvFloatColour:
	case PEXHlsFloatColour:
		pexpr_float_colour( buf);
		break;
	case PEXRgb8Colour:
		pexpr_rgb8_colour( buf);
		break;
	case PEXRgb16Colour:
		pexpr_rgb8_colour( buf);
		break;
	default:
		break;
	}
}

void
pexpr_colour_specifier( p)
	pexColourSpecifier *p;
{
	pexColourType type = p->colourType;
	unsigned char *pStream = (unsigned char *) p;

	pexpr_colour_type( p->colourType);

	pStream += sizeof(pexColourSpecifier);

	pexpr_colour_value( type, pStream);
}

void
pexpr_coord_3d( str, p)
	char *str;
	pexCoord3D *p;
{
	(void) fprintf( stderr, 
	"%s: ( %g, %g, %g)\n", str, p->x, p->y, p->z);
}

void
pexpr_vector_3d( str, p)
	char *str;
	pexVector3D *p;
{
	(void) fprintf( stderr, 
	"%s: ( %g, %g, %g)\n", str, p->x, p->y, p->z);
}

void
pexpr_coord_2d( str, p)
	char *str;
	pexCoord2D *p;
{
	(void) fprintf( stderr, 
	"%s: ( %g, %g)\n", str, p->x, p->y);
}

void
pexpr_vector_2d( buf)
	register unsigned char *buf;
{
	pexVector2D *p = (pexVector2D *) buf;
	PrintFval(x,p->x);
	PrintFval(y,p->y);
}

void
pexpr_text_alignment_data( buf)
	register unsigned char *buf;
{
#define VALID_V_ALIGN(_V) \
(_V >= PEXValignNormal) && (_V <= PEXValignBottom)

#define VALID_H_ALIGN(_H) \
(_H >= PEXHalignNormal) && (_H <= PEXHalignRight)

	static char *v_align[6] = {
		"Normal", "Top", "Cap", "Half",   "Base",   "Bottom" 
	};

	static char *h_align[4] = {
		"Normal", "Left", "Center", "Right"
	};
	pexTextAlignmentData *p = (pexTextAlignmentData *) buf;

	if( VALID_V_ALIGN(p->vertical)) {
		PrintString(alignment.vertical,v_align[p->vertical]);
	} else {
		PrintString(alignment.vertical,"Invalid");
	}

	if( VALID_H_ALIGN(p->horizontal)) {
		PrintString(alignment.horizontal,h_align[p->horizontal]);
	} else {
		PrintString(alignment.horizontal,"Invalid");
	}

#undef VALID_V_ALIGN
#undef VALID_H_ALIGN
}

void
pexpr_editing_mode( mode)
	CARD16 mode;
{
#define VALID_EDIT_MODE(_MODE) \
(_MODE == PEXStructureInsert) || (_MODE == PEXStructureReplace)

	static char *e_mode[2] = {
		"Insert",
		"Replace"
	};

	if( VALID_EDIT_MODE(mode)) {
		PrintString(editMode,e_mode[mode]);
	} else {
		PrintString(editMode,"Invalid");
	}
#undef VALID_EDIT_MODE
}

void
pexpr_buffer_mode( mode)
	CARD16 mode;
{
#define VALID_BUFFER_MODE(_MODE) \
(_MODE == PEXSingleBuffered) || (_MODE == PEXDoubleBuffered)

	static char *b_mode[2] = {
		"Single", "Double"
	};

	if( VALID_BUFFER_MODE(mode)) {
		PrintString(bufferMode,b_mode[mode]);
	} else {
		PrintString(bufferMode,"Invalid");
	}
#undef VALID_BUFFER_MODE
}

void 
pexpr_table_type( type)
	CARD16 type;
{
#define VALID_TABLE_TYPE(_TYPE) \
(_TYPE >= PEXLineBundleLUT) && (_TYPE <= PEXMaxTableType)

	static char *table_type[PEXMaxTableType+1] = {
		"",
		"LineBundle",
		"MarkerBundle",
		"TextBundle",
		"InteriorBundle",
		"EdgeBundle",
		"Pattern",
		"TextFont",
		"Colour",
		"View",
		"Light",
		"DepthCue",
		"ColourApprox"
	};
	if( VALID_TABLE_TYPE(type)) {
		PrintString(tableType,table_type[type]);
	} else {
		PrintString(tableType,"Invalid");
	}
#undef VALID_TABLE_TYPE
}

void
pexpr_whence( str, v)
	char *str;
	CARD16 v;
{
#define VALID_WHENCE(_WHENCE) \
(_WHENCE >= PEXBeginning) && (_WHENCE <= PEXEnd)

	static char *whence[3] = {
		"Beginning",
		"Current",
		"End"
	};

	if( VALID_WHENCE(v)) {
		(void) fprintf( stderr,"%s: %s\n", str, whence[v]);
	} else {
		(void) fprintf( stderr,"%s: Invalid\n", str);
	}
}

void
pexpr_matrix( m)
	pexMatrix m;
{
	register long i;

	for( i = 0; i <= 3; i++) {
		(void) fprintf( stderr,
		"[ %g %g %g %g ]\n",
		m[0][i], m[1][i], m[2][i], m[3][i]);
	}
}

void
pexpr_matrix_3x3( m)
	pexMatrix3X3 m;
{
	register long i;

	for( i = 0; i <= 2; i++) {
		(void) fprintf( stderr,
		"[ %g %g %g ]\n",
		m[0][i], m[1][i], m[2][i]);
	}
}

void
pexpr_comp_type( type)
	pexComposition type;	
{
#define VALID_COMPTYPE(_TYPE) \
(_TYPE >= PEXPreConcatenate) && (_TYPE <= PEXReplace)

	static char *comp_type[3] = {
		"PreConcatenate",
		"PostConcatenate",
		"Replace"
	};

	if( VALID_COMPTYPE(type)) {
		PrintString(compType,comp_type[type]);
	} else {
		PrintString(compType,"Invalid");
	}
#undef VALID_COMPTYPE
}

void
pexpr_geo_attrs( str, bitmask)
	char *str;
	pexBitmaskShort bitmask;
{
	static char *geo_attrs[] = {
		"None",					/* 0x0000 */
		"Color",				/* 0x0001 */
		"Normal",				/* 0x0002 */
		"Colour, Normal",		/* 0x0003 */
		"Edges", 				/* 0x0004 */
		"Colour, Edges",		/* 0x0005 */
		"Normal, Edges",		/* 0x0006 */
		"Colour, Normal, Edges" /* 0x0007 */
	};
	bitmask &= 0x0003;

	(void) fprintf( stderr, "%s: %s: 0x%x\n", 
	str, geo_attrs[(int) bitmask], bitmask);
}

void
pexpr_shape_hint( str, shape_hint)
	char *str;
	CARD16 shape_hint;
{
#define VALID_SHAPE_HINT(_SHAPE) \
(_SHAPE >= PEXComplex) && (_SHAPE <= PEXUnknownShape)

	static char *shape[4] = {
		"Complex",
		"NonConvex",
		"Convex",
		"UnknownShape"
	};

	if( VALID_SHAPE_HINT(shape_hint)) {
		(void) fprintf( stderr, "%s: %s\n", str, shape[shape_hint]);
	} else {
		(void) fprintf( stderr, "%s: Invalid\n", str);
	}
#undef VALID_SHAPE_HINT
}

void
pexpr_contour_hint( str, contour_hint)
	char *str;
	CARD8 contour_hint;
{
#define VALID_CONTOUR_HINT(_CONTOUR) \
(_CONTOUR >= PEXDisjoint) && (_CONTOUR <= PEXUnknownContour)

	static char *contour[4] = {
		"Disjoint",
		"Nested",
		"Intersecting",
		"UnknownContour"
	};

	if( VALID_CONTOUR_HINT(contour_hint)) {
		(void) fprintf( stderr, "%s: %s\n", 
			str, contour[contour_hint]);
	} else {
		(void) fprintf( stderr, "%s: Invalid\n", str);
	}
#undef VALID_CONTOUR_HINT
}

void
pexpr_curve_approx_method( str, method)
	char *str;
	pexEnumTypeIndex method;
{
#define VALID_APPROX_METHOD(_METHOD) \
(_METHOD >= PEXApproxImpDep) && (_METHOD <= PEXApproxDcRelative)
	static char *curve_approx_method[12] = {
		"Invalid",
		"ImpDep",
		"ConstantBetweenKnots",
		"WCS_ChordalSize",
		"NPC_ChordalSize",
		"DC_ChordalSize",
		"WCS_ChordalDev",
		"NPC_ChordalDev",
		"DC_ChordalDev",
		"WCS_Relative",
		"NPC_Relative",
		"DC_Relative"
	};
	if( VALID_APPROX_METHOD(method)) {
		(void) fprintf( stderr, "%s: %s\n", 
			str, curve_approx_method[method]);
	} else {
		(void) fprintf( stderr, "%s: Invalid\n", str);
	}
#undef VALID_APPROX_METHOD
}

void
pexpr_float( str, value)
	char *str;
	XpexFloat value;
{
	(void) fprintf( stderr, "%s: %g\n", str, value);
}

void
pexpr_card( str, value)
	char *str;
	unsigned value;
{
	(void) fprintf( stderr, "%s: %u\n", str, value);
}

void
pexpr_coord_type( str, type)
	char *str;
	pexCoordType type;
{
	(void) fprintf( stderr, "%s: %s\n", str,
		( type == PEXRational) ? "Rational" : 
		(( type == PEXNonRational) ? "NonRational" : "Invalid"));
}

void
pexpr_switch( str, on_off)
	char *str;
	pexSwitch on_off;
{
	(void) fprintf( stderr, "%s: %s\n", str,
		(on_off == 0) ? "False/Off" :
		( ( on_off == 1) ? "True/On" : "Invalid"));
}

void
pexpr_array_of_coord_3d( count, points)
	int count;
	pexCoord3D *points;
{
	while( count--) {
		(void) fprintf( stderr, "( %g %g %g)\n",
			points->x, points->y, points->z);
		points++;
	}
}

void
pexpr_array_of_coord_2d( count, points)
	int count;
	pexCoord2D *points;
{
	while( count--) {
		(void) fprintf( stderr, "( %g %g)\n", points->x, points->y);
		points++;
	}
}
