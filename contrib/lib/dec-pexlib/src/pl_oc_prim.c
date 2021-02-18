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
**	pl_oc_prim.c
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
**	This file contains the routines associated with the PEX output primitive
**	commands.
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
**	19-Aug-88  
**
**--
*/

/*
 * Table of Contents (Routine names in order of appearance)
 */

/*
 * E   PEXMarkers
 * E   PEXMarkers2D
 * E   PEXText
 * E   PEXText2D
 * E   PEXAnnotationText
 * E   PEXAnnotationText2D
 * E   PEXMonoEncodedText
 * E   PEXMonoEncodedText2D
 * E   PEXMonoEncodedAnnoText
 * E   PEXMonoEncodedAnnoText2D
 * E   PEXPolyline
 * E   PEXPolyline2D
 * E   PEXPolylineSet
 * E   PEXNurbCurve
 * E   PEXFillArea
 * E   PEXFillArea2D
 * E   PEXMultiFillAreaData
 * E   PEXFillAreaSet
 * E   PEXFillAreaSet2D
 * E   PEXMultiFillAreaSetData
 * E   PEXSetOfFillAreaSets
 * E   PEXTriangleStrip
 * E   PEXQuadMesh
 * E   PEXNurbSurface
 * E   PEXCellArray
 * E   PEXCellArray2D
 * E   PEXExtendedCellArray
 * E   PEXGdp
 * E   PEXGdp2D
 */

/*
 *   Include Files
 */

#include "pex_features.h"
#include "pex_macros.h"

#include "PEX.h"
#include "PEXlib.h"
#include "PEXlibint.h"
#include "PEXocbuf.h"


#ifdef MPEX
#include "MPEX.h"
#include "MPEXprotostr.h"
#include "MPEXlib.h"
#endif

#include "pl_ref_data.h"

/*
 * compute the number of words in a colour .. note that all the
 * pex colour types are padded to end on a word boundary
 */
#define GetColourLength(_type_)\
    ((_type_) == PEXIndexedColour ?  LENOF( pexIndexedColour) :\
    ((_type_) == PEXRgb8Colour ?  LENOF( pexRgb8Colour) :\
    ((_type_) == PEXRgb16Colour ? LENOF( pexRgb16Colour) : \
                            LENOF( pexRgbFloatColour) )))

/*
 * compute the number of words in the facet data
 */
#ifdef MPEX
#define GetFacetDataLength(_fattribs_,_lenofColour_) \
    ( ((_fattribs_ & PEXGAColour) ? _lenofColour_ : 0) + \
      ((_fattribs_ & MPEXGATexture) ? LENOF( pexCoord2D) : 0) + \
      ((_fattribs_ & PEXGANormal) ? LENOF( pexVector3D) : 0) )
#else
#define GetFacetDataLength(_fattribs_,_lenofColour_) \
    ( ((_fattribs_ & PEXGAColour) ? _lenofColour_ : 0) + \
      ((_fattribs_ & PEXGANormal) ? LENOF( pexVector3D) : 0) )
#endif

/*
 * compute the number of words in a vertex with optional colours and normals
 */
#ifdef MPEX 
#define GetVertexWithDataLength(_vattribs_,_lenofColour_)\
    ( LENOF( pexCoord3D) + \
      ((_vattribs_ & PEXGAColour) ? _lenofColour_ : 0) + \
      ((_vattribs_ & MPEXGATexture) ? LENOF( pexCoord2D) : 0) + \
      ((_vattribs_ & PEXGANormal) ? LENOF( pexVector3D) : 0) )
#else
#define GetVertexWithDataLength(_vattribs_,_lenofColour_)\
    ( LENOF( pexCoord3D) + \
      ((_vattribs_ & PEXGAColour) ? _lenofColour_ : 0) + \
      ((_vattribs_ & PEXGANormal) ? LENOF( pexVector3D) : 0) )
#endif

/*
 * Store a text string as a mono encoded string which uses character set 1
 * and  byte character widths
 */
#define StoreDefaultMonoString(_ocbuf,_count,_string) \
{ \
  pexMonoEncoding *_pMonoEncoding; \
 \
  if ( (_pMonoEncoding = (pexMonoEncoding *)  \
		    PEXGetOCWords(_ocbuf, LENOF(pexMonoEncoding))) != NULL ) \
  { \
    _pMonoEncoding->characterSet = (INT16) 1; \
    _pMonoEncoding->characterSetWidth = (CARD8) PEXCSByte; \
    _pMonoEncoding->encodingState = 0;   \
    _pMonoEncoding->numChars = (CARD16) (_count); \
  } \
 \
  PEXCopyBytesToOC( _ocbuf, _count, _string); \
}

/*
 * Compute length of mono-encoded strings.  For efficiency this macro uses
 * the following globals:
 *    int 			i;
 *    pxlMonoEncodedTextData    *nextString;
 */
#define GET_LENGTH_STRINGS(_numStrings,_stringList,_lenofStrings) \
{ \
    (_lenofStrings) = 0; \
    for (i=0, nextString=(_stringList); i < (_numStrings); i++, nextString++) \
    { \
	lenofStrings += LENOF(pexMonoEncoding); \
        if ( nextString->characterSetWidth == PEXCSLong ) \
            (_lenofStrings) += NUMWORDS( nextString->numChars * sizeof(long));\
        else if ( nextString->characterSetWidth == PEXCSShort ) \
            (_lenofStrings) += NUMWORDS( nextString->numChars * sizeof(short));\
        else /* ( nextString->characterSetWidth == PEXCSByte) */ \
            (_lenofStrings) += NUMWORDS( nextString->numChars); \
    } \
}

/*
 * Macro to store a mono encoded string.  For efficiency this macro uses
 * the following globals:
 *    int               	i;
 *    pxlMonoEncodedTextData	*nextString;
 */
#define STORE_ENCODED_STRINGS(_ocbuf,_numStrings,_stringList) \
{ \
    for (i=0, nextString=(_stringList); i < (_numStrings); i++, nextString++) \
    { \
	PEXCopyWordsToOC(_ocbuf, LENOF(pexMonoEncoding), nextString); \
 \
        if ( nextString->characterSetWidth == PEXCSLong) \
            PEXCopyBytesToOC(_ocbuf, nextString->numChars * sizeof(long), \
		nextString->chars); \
        else if ( nextString->characterSetWidth == PEXCSShort) \
            PEXCopyBytesToOC(_ocbuf, nextString->numChars * sizeof(short), \
		nextString->chars); \
        else /* nextString->characterSetWidth == PEXCSByte) */ \
            PEXCopyBytesToOC(_ocbuf, nextString->numChars, \
		nextString->chars); \
    } \
}

/*+
 * NAME:
 * 	PEXMarkers
 *
 * FORMAT:
 * 	`PEXMarkers`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	points		A list of points specifying locations of markers.
 *
 *	numPoints	The number of points in the `points' array.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine, when processed by a renderer, draws a set of marker
 *	primitives.
 *	`Points' is an array containing the position (in modeling coordinates)
 *	of each marker to be drawn.  `NumPoints' indicates the
 *	number of points in the array.
 *
 *	Marker primitives are drawn using the marker colour, marker type,
 *	and marker scale attributes.  Each of these attributes has a
 *	corresponding ASF (attribute source flag).
 *	If a marker attribute's ASF is `PEXIndividual',
 *	the marker attribute is obtained directly from
 *	the current value in the rendering pipeline.
 *	If a marker attribute's ASF are `PEXBundled',
 *	the value is obtained from the marker bundle lookup
 *	table entry indicated by the current marker bundle index attribute.
 *
 *	Note that a marker has no geometric size, so geometric transformations
 *	do not affect the displayed size of the marker.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetMarkerType", `PEXSetMarkerScale",
 *	 `PEXSetMarkerColourIndex", `PEXSetMarkerColour",
 *	
 *
 *
 */

void
PEXMarkers (ocbuf, points, numPoints)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord3D	*points;
INPUT int		numPoints;

{
/* AUTHOR: */
    PEXAddListOC( ocbuf, PEXOCMarker, False/*not a counted list*/, numPoints, 
			sizeof( pxlCoord3D), points);

    return;
}

/*+
 * NAME:
 * 	PEXMarkers2D
 *
 * FORMAT:
 * 	`PEXMarkers2D`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *  			primitive command is added.
 *
 *	points		A list of points specifying locations of markers.
 *
 *	numPoints	The number of points in the `points' array.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine is the same as `PEXMarkers", except
 *	that the vertices consist of only (X,Y) values.  The
 *	Z-value is assumed to be zero.  This primitive is two-dimensional
 *	only in that the Z-components are implied.  Geometry and
 *	colour transformation operations are still carried out in three
 *	dimensions.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetMarkerType", `PEXSetMarkerScale",
 *	 `PEXSetMarkerColourIndex", `PEXSetMarkerColour", `PEXSetMarkerBundleIndex"
 *
 */

void
PEXMarkers2D (ocbuf, points, numPoints)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord2D	*points;
INPUT int		numPoints;

{
/* AUTHOR: */
    PEXAddListOC( ocbuf, PEXOCMarker2D, False/*not a counted list*/, numPoints, 
			sizeof( pxlCoord2D), points);

    return;
}

/*+
 * NAME:
 * 	PEXText
 *
 * FORMAT:
 * 	`PEXText`
 *
 * ARGUMENTS:
 *	ocbuf		The output buffer to which the output primitive
 *			command is added.
 *
 *	origin		The modeling coordinate position at which to draw
 *			the text string.
 *
 *	vec1		A vector defining the positive x-direction of
 *			the text local coordinate system.
 *
 *	vec2		A vector defining the positive y-direction of
 *			the text local coordinate system.
 *
 *	string		The character string to be drawn.
 *
 *	count		The number of bytes in `string'.
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *      This routine, when processed by a renderer, draws the text string
 *	specified
 *	by `string'.  The text string is located on a plane defined by
 *	its position and direction vectors.  `Origin' defines the position
 *	at which to draw the text string.  `Vec1' and `vec2' define the
 *	positive x- and y-directions of the text local coordinate system.
 *	If the two vectors are parallel or if one of the vectors has zero
 *	length, the vector values <1,0,0> and <0,1,0> are used.
 *
 *	During the rendering process, the string's position is
 * 	transformed to a position in device coordinates.  The string's
 *	colour is only transformed by the depth-cueing transformation
 *	(the light-source shading stage of the rendering pipeline
 *	affects only surfaces) and is mapped to a device colour.
 *
 *	Depending on the text attribute
 *	source flag values, the text colour, text precision,
 *	character expansion, character spacing, and text font attributes are
 *	obtained from one of two sources. These attributes are obtained from
 *	either the current text attribute values or
 *	from the entry in the text bundle lookup
 *	table specified by the current text bundle index attribute.
 *      The first character set in the text font will be used.
 *	The current character height, text path, text alignment attributes 
 *	and character up vector are also used to render the text string.
 *	The directions specified by the character up vector and text path
 *	are relative to the text local coordinate system defined by `vec1'
 *	and `vec2'.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 * 	`PEXAllocateOCBuffer", `PEXSetTextFontIndex", `PEXSetTextPrecision", 
 *	`PEXSetCharExpansion",  `PEXSetCharSpacing", `PEXSetTextColourIndex", 
 *	`PEXSetTextColour",  `PEXSetCharHeight", `PEXSetCharUpVector", 
 *	`PEXSetTextPath",  `PEXSetTextAlignment", `PEXSetTextBundleIndex"
 *
 */

void
PEXText (ocbuf, origin, vec1, vec2, string, count)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord3D	*origin;
INPUT pxlVector3D	*vec1;
INPUT pxlVector3D	*vec2;
INPUT char		*string;
INPUT int		count;

{
/* AUTHOR: */
    pexText	*pReq;

    PEXInitOC( ocbuf, PEXOCText, LENOF( pexText), 
	    LENOF(pexMonoEncoding) + NUMWORDS( count), &pReq);
    if (pReq == NULL) return;

    /* 
     * Store the text request header data in the oc buffer 
     */
    pReq->origin = *origin;
    pReq->vector1 = *vec1;
    pReq->vector2 = *vec2;
    pReq->numEncodings = (CARD16) 1;

    /*
     * Store the mono-encoded string
     */
    StoreDefaultMonoString(ocbuf, count, string);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXText2D
 *
 * FORMAT:
 * 	`PEXText2D`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	origin		The modeling coordinate position at which to draw
 *			the text string.
 *
 *	string		The character string to be drawn.
 *
 *	count		The number of bytes in `string'.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine is the same as `PEXText", except
 *	that the `origin' position consists of only an (X,Y) pair.  The
 *	Z-value is assumed to be zero.  This primitive is two-dimensional
 *	only in that the Z-component is implied.  Geometry and colour
 *	transformation operations are still carried out in three dimensions.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer",  `PEXSetTextFontIndex", `PEXSetTextPrecision", 
 *	`PEXSetCharExpansion",  `PEXSetCharSpacing", `PEXSetTextColourIndex", 
 *	`PEXSetTextColour",  `PEXSetCharHeight", `PEXSetCharUpVector", 
 *	`PEXSetTextPath",  `PEXSetTextAlignment", `PEXSetTextBundleIndex"
 *
 */

void
PEXText2D (ocbuf, origin, string, count)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord2D	*origin;
INPUT char		*string;
INPUT int		count;

{
/* AUTHOR: */
    pexText2D	*pReq;

    PEXInitOC( ocbuf, PEXOCText2D, LENOF( pexText2D), 
		LENOF(pexMonoEncoding) + NUMWORDS( count), &pReq);
    if (pReq == NULL) return;

    /* 
     * Store the text header request data in the oc buffer 
     */
    pReq->origin = *origin;
    pReq->numEncodings = (CARD16) 1;

    /*
     * Store the mono-encoded string
     */
    StoreDefaultMonoString(ocbuf, count, string);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXAnnotationText
 *
 * FORMAT:
 * 	`PEXAnnotationText`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	origin		The origin of the text string.
 *
 *	offset		The relative position of the text string from the
 *			origin.
 *
 *	string		A pointer to the character string to be drawn.
 *
 *	count		The number of bytes in `string'.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine, when processed by a renderer, draws an annotation text
 *	string.
 *	`String' is a pointer to the text string to be rendered.  `Origin'
 *	defines, in modelling coordinates, the origin of the lower
 *	left-hand corner of the first character in the text string.
 *	`Offset' specifies an offset from the origin in normalized
 *	projection coordinates.	 The point where the text string is
 *	placed is called the `annotation point'.  The `annotation point' is
 *	computed by adding the value of `offset' to the transformed
 *	`origin' point.  The z-component of the annotation point
 *	specifies the x-y plane in normalized projection coordinate
 *	space on which the annotation text string is placed.
 *
 *	Depending on the text attribute
 *	source flag values, the text colour, text precision,
 *	character expansion, character spacing, and text font attributes are
 *	obtained from one of two sources. These attributes are obtained from
 *	either the current text attribute values or
 *	from the entry in the text bundle lookup
 *	table specified by the current text bundle index attribute.
 *      The first character set in the text font will be used.
 *	The annotation text height, annotation text path,
 *	annotation text alignment, annotation text up vector,
 *	and annotation text style are also used to render the text string.
 *
 *	The annotation text string's colour is transformed only by the
 *	depth-cueing computation (the light-source shading stage of the
 *	rendering pipeline affects only surfaces) and is mapped to a
 *	device colour.  The entire annotation text primitive is clipped
 *	if `origin' is clipped.  If `origin' is not clipped, the
 *	annotation text is clipped according to text clipping rules.
 *	The connection line, if any, is clipped according to
 *	polyline clipping rules, except that modeling clipping is
 *	ignored.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXSetTextFontIndex", `PEXSetTextPrecision", `PEXSetCharExpansion",
 *	`PEXSetCharSpacing", `PEXSetTextColourIndex", `PEXSetTextColour",
 *	`PEXSetATextHeight", `PEXSetATextUpVector", `PEXSetATextPath",
 *	`PEXSetATextAlignment", `PEXSetATextStyle", `PEXSetTextBundleIndex", 
 *	`PEXAllocateOCBuffer"
 *
 */

void
PEXAnnotationText (ocbuf, origin, offset, string, count)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord3D	*origin;
INPUT pxlCoord3D	*offset;
INPUT char		*string;
INPUT int		count;

{
/* AUTHOR: */
    pexAnnotationText	*pReq;

    PEXInitOC( ocbuf, PEXOCAnnotationText, LENOF( pexAnnotationText),
		LENOF(pexMonoEncoding) + NUMWORDS( count), &pReq);
    if (pReq == NULL) return;

    /* 
     * Store the atext request header data in the oc buffer 
     */
    pReq->origin = *origin;
    pReq->offset = *offset;
    pReq->numEncodings = (CARD16) 1;

    /*
     * Store the mono-encoded string
     */
    StoreDefaultMonoString(ocbuf, count, string);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXAnnotationText2D
 *
 * FORMAT:
 * 	`PEXAnnotationText2D`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	origin		The origin of the text string.
 *
 *	offset		The relative position of the text string from the
 *			origin.
 *
 *	string		A pointer to the character string to be drawn.
 *
 *	count		The number of bytes in `string'.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine, when processed by a renderer, draws an annotation text
 *	string.  This primitive functions exactly as `PEXAnnotationText" 
 *	except that origin and offset positions are specified 
 *	using only x- and y-coordinates, and the z-coordinate is always 
 *	assumed to be zero.  This primitive is two-dimensional only in
 *	that the z-component is implied, since geometry and
 *	colour transformation operations are still carried out in three
 *	dimensions.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXSetTextFontIndex", `PEXSetTextPrecision", `PEXSetCharExpansion",
 *	`PEXSetCharSpacing", `PEXSetTextColourIndex", `PEXSetTextColour",
 *	`PEXSetATextHeight", `PEXSetATextUpVector", `PEXSetATextPath",
 *	`PEXSetATextAlignment", `PEXSetATextStyle", `PEXSetTextBundleIndex",
 *    	`PEXAllocateOCBuffer"
 *
 */

void
PEXAnnotationText2D (ocbuf, origin, offset, string, count)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord2D	*origin;
INPUT pxlCoord2D	*offset;
INPUT char		*string;
INPUT int		count;

{
/* AUTHOR: */
    pexAnnotationText2D	*pReq;

    PEXInitOC( ocbuf, PEXOCAnnotationText2D, LENOF( pexAnnotationText2D), 
		LENOF(pexMonoEncoding) + NUMWORDS(count), &pReq);
    if (pReq == NULL) return;

    /* 
     * Store the atext request header data in the oc buffer 
     */
    pReq->origin = *origin;
    pReq->offset = *offset;
    pReq->numEncodings = (CARD16) 1;

    /*
     * Store the mono-encoded string
     */
    StoreDefaultMonoString(ocbuf, count, string);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXMonoEncodedText
 *
 * FORMAT:
 * 	`PEXMonoEncodedText`
 *
 * ARGUMENTS:
 *	ocbuf		The output buffer to which the output primitive
 *			command is added.
 *
 *	origin		The modeling coordinate position at which to draw
 *			the text string.
 *
 *	vec1		A vector defining the positive x-direction of
 *			the text local coordinate system.
 *
 *	vec2		A vector defining the positive y-direction of
 *			the text local coordinate system.
 *
 * 	numEncodings	The number of mono encoded text strings in `meTextList'
 *
 * 	encodedTextList	List of mono encoded text strings
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine is the same as `PEXText", except
 *	that multiple encoded text strings can be specified.  Each text 
 *	string in `encodedTextList' has a character set, a character
 *	set width, an encoding state, and a list of characters. (see
 *      `pxlMonoEncodedTextData' defined in `PEXlib.h').  The character
 *	set is an index into the current font group.  Font groups have 
 *	individual fonts which are numbered starting at one; a value of 
 *	three would select the third font in the font group currently being 
 *	used.  If a character set is not available in the current font 
 *	group then the entire string will be rendered using the default 
 *	font group.  If a character set value is not available in the 
 *	default font group then that portion of the string will be rendered 
 *	in an implementation dependent manner.  The character width indicates 
 *	whether characters in the strings are 8-, 16-, or 32-bit values.  
 *	The possible values for character width are `PEXCSByte', 
 *	`PEXCSShort' and `PEXCSLong' (defined in `PEX.h'). Characters 
 *	in the string are byte swapped by the PEX server if necessary.
 *	The encoding state is provided for use by clients and is not 
 *	interpreted by the pex server.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 * 	`PEXAllocateOCBuffer", `PEXSetTextFontIndex", `PEXSetTextPrecision", 
 *	`PEXSetCharExpansion",  `PEXSetCharSpacing", `PEXSetTextColourIndex", 
 *	`PEXSetTextColour",  `PEXSetCharHeight", `PEXSetCharUpVector", 
 *	`PEXSetTextPath",  `PEXSetTextAlignment", `PEXSetTextBundleIndex"
 *
 */
void
PEXMonoEncodedText (ocbuf, origin, vec1, vec2, numEncodings, encodedTextList)

INPUT pxlOCBuf			*ocbuf;
INPUT pxlCoord3D		*origin;
INPUT pxlVector3D		*vec1;
INPUT pxlVector3D		*vec2;
INPUT int			numEncodings;
INPUT pxlMonoEncodedTextData	*encodedTextList;

{
/* AUTHOR: */
    pexText			*pReq;
    int 			i, lenofStrings;
    pxlMonoEncodedTextData	*nextString;

    /*
     * Get length of mono-encoded strings 
     */
    GET_LENGTH_STRINGS(numEncodings, encodedTextList, lenofStrings);

    PEXInitOC( ocbuf, PEXOCText, LENOF( pexText), lenofStrings, &pReq);
    if (pReq == NULL) return;

    /* 
     * Store the text request header data in the oc buffer 
     */
    pReq->origin = *origin;
    pReq->vector1 = *vec1;
    pReq->vector2 = *vec2;
    pReq->numEncodings = (CARD16) numEncodings;

    /*
     * Store the mono-encoded string
     */
    STORE_ENCODED_STRINGS(ocbuf, numEncodings, encodedTextList);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXMonoEncodedText2D
 *
 * FORMAT:
 * 	`PEXMonoEncodedText2D`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	origin		The modeling coordinate position at which to draw
 *			the text string.
 *
 * 	numEncodings	The number of mono encoded text strings in `meTextList'
 *
 * 	encodedTextList	List of mono encoded text strings
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine is the same as `PEXText2D", except
 *	that multiple encoded text strings can be specified.  Each text 
 *	string in `encodedTextList' has a character set, a character
 *	set width, an encoding state, and a list of characters. (see
 *      `pxlMonoEncodedTextData' defined in `PEXlib.h').  The character
 *	set is an index into the current font group.  Font groups have 
 *	individual fonts which are numbered starting at one; a value of 
 *	three would select the third font in the font group currently being 
 *	used.  If a character set is not available in the current font 
 *	group then the entire string will be rendered using the default 
 *	font group.  If a character set value is not available in the 
 *	default font group then that portion of the string will be rendered 
 *	in an implementation dependent manner.  The character width indicates 
 *	whether characters in the strings are 8-, 16-, or 32-bit values.  
 *	The possible values for character width are `PEXCSByte', 
 *	`PEXCSShort' and `PEXCSLong' (defined in `PEX.h'). Characters 
 *	in the string are byte swapped by the PEX server if necessary.
 *	The encoding state is provided for use by clients and is not 
 *	interpreted by the pex server.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer",  `PEXSetTextFontIndex", `PEXSetTextPrecision", 
 *	`PEXSetCharExpansion",  `PEXSetCharSpacing", `PEXSetTextColourIndex", 
 *	`PEXSetTextColour",  `PEXSetCharHeight", `PEXSetCharUpVector", 
 *	`PEXSetTextPath",  `PEXSetTextAlignment", `PEXSetTextBundleIndex"
 *
 */
void
PEXMonoEncodedText2D (ocbuf, origin, numEncodings, encodedTextList)

INPUT pxlOCBuf			*ocbuf;
INPUT pxlCoord2D		*origin;
INPUT int			numEncodings;
INPUT pxlMonoEncodedTextData	*encodedTextList;

{
/* AUTHOR: */
    pexText2D			*pReq;
    int 			i, lenofStrings;
    pxlMonoEncodedTextData	*nextString;

    /*
     * Get length of mono-encoded strings 
     */
    GET_LENGTH_STRINGS(numEncodings, encodedTextList, lenofStrings);

    PEXInitOC( ocbuf, PEXOCText2D, LENOF( pexText2D), lenofStrings, &pReq);
    if (pReq == NULL) return;

    /* 
     * Store the text header request data in the oc buffer 
     */
    pReq->origin = *origin;
    pReq->numEncodings = (CARD16) numEncodings;

    /*
     * Store the mono-encoded string
     */
    STORE_ENCODED_STRINGS(ocbuf, numEncodings, encodedTextList);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXMonoEncodedAnnoText
 *
 * FORMAT:
 * 	`PEXMonoEncodedAnnoText`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	origin		The origin of the text string.
 *
 *	offset		The relative position of the text string from the
 *			origin.
 *
 * 	numEncodings	The number of mono encoded text strings in `meTextList'
 *
 * 	encodedTextList	List of mono encoded text strings
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine is the same as `PEXAnnotationText", except
 *	that multiple encoded text strings can be specified.  Each text 
 *	string in `encodedTextList' has a character set, a character
 *	set width, an encoding state, and a list of characters. (see
 *      `pxlMonoEncodedTextData' defined in `PEXlib.h').  The character
 *	set is an index into the current font group.  Font groups have 
 *	individual fonts which are numbered starting at one; a value of 
 *	three would select the third font in the font group currently being 
 *	used.  If a character set is not available in the current font 
 *	group then the entire string will be rendered using the default 
 *	font group.  If a character set value is not available in the 
 *	default font group then that portion of the string will be rendered 
 *	in an implementation dependent manner.  The character width indicates 
 *	whether characters in the strings are 8-, 16-, or 32-bit values.  
 *	The possible values for character width are `PEXCSByte', 
 *	`PEXCSShort' and `PEXCSLong' (defined in `PEX.h'). Characters 
 *	in the string are byte swapped by the PEX server if necessary.
 *	The encoding state is provided for use by clients and is not 
 *	interpreted by the pex server.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXSetTextFontIndex", `PEXSetTextPrecision", `PEXSetCharExpansion",
 *	`PEXSetCharSpacing", `PEXSetTextColourIndex", `PEXSetTextColour",
 *	`PEXSetATextHeight", `PEXSetATextUpVector", `PEXSetATextPath",
 *	`PEXSetATextAlignment", `PEXSetATextStyle", `PEXSetTextBundleIndex", 
 *	`PEXAllocateOCBuffer"
 *
 */
void
PEXMonoEncodedAnnoText (ocbuf, origin, offset, numEncodings, encodedTextList)

INPUT pxlOCBuf			*ocbuf;
INPUT pxlCoord3D		*origin;
INPUT pxlCoord3D		*offset;
INPUT int			numEncodings;
INPUT pxlMonoEncodedTextData	*encodedTextList;

{
/* AUTHOR: */
    pexAnnotationText		*pReq;
    int 			i, lenofStrings;
    pxlMonoEncodedTextData	*nextString;

    /*
     * Get length of mono-encoded strings 
     */
    GET_LENGTH_STRINGS(numEncodings, encodedTextList, lenofStrings);

    PEXInitOC( ocbuf, PEXOCAnnotationText, LENOF( pexAnnotationText),
		lenofStrings, &pReq);
    if (pReq == NULL) return;

    /* 
     * Store the atext request header data in the oc buffer 
     */
    pReq->origin = *origin;
    pReq->offset = *offset;
    pReq->numEncodings = (CARD16) numEncodings;

    /*
     * Store the mono-encoded string
     */
    STORE_ENCODED_STRINGS(ocbuf, numEncodings, encodedTextList);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXMonoEncodedAnnoText2D
 *
 * FORMAT:
 * 	`PEXMonoEncodedAnnoText2D`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	origin		The origin of the text string.
 *
 *	offset		The relative position of the text string from the
 *			origin.
 *
 * 	numEncodings	The number of mono encoded text strings in `meTextList'
 *
 * 	encodedTextList	List of mono encoded text strings
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine is the same as `PEXAnnotationText2D", except
 *	that multiple encoded text strings can be specified.  Each text 
 *	string in `encodedTextList' has a character set, a character
 *	set width, an encoding state, and a list of characters. (see
 *      `pxlMonoEncodedTextData' defined in `PEXlib.h').  The character
 *	set is an index into the current font group.  Font groups have 
 *	individual fonts which are numbered starting at one; a value of 
 *	three would select the third font in the font group currently being 
 *	used.  If a character set is not available in the current font 
 *	group then the entire string will be rendered using the default 
 *	font group.  If a character set value is not available in the 
 *	default font group then that portion of the string will be rendered 
 *	in an implementation dependent manner.  The character width indicates 
 *	whether characters in the strings are 8-, 16-, or 32-bit values.  
 *	The possible values for character width are `PEXCSByte', 
 *	`PEXCSShort' and `PEXCSLong' (defined in `PEX.h'). Characters 
 *	in the string are byte swapped by the PEX server if necessary.
 *	The encoding state is provided for use by clients and is not 
 *	interpreted by the pex server.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXSetTextFontIndex", `PEXSetTextPrecision", `PEXSetCharExpansion",
 *	`PEXSetCharSpacing", `PEXSetTextColourIndex", `PEXSetTextColour",
 *	`PEXSetATextHeight", `PEXSetATextUpVector", `PEXSetATextPath",
 *	`PEXSetATextAlignment", `PEXSetATextStyle", `PEXSetTextBundleIndex",
 *    	`PEXAllocateOCBuffer"
 *
 */
void
PEXMonoEncodedAnnoText2D (ocbuf, origin, offset, numEncodings, encodedTextList)

INPUT pxlOCBuf			*ocbuf;
INPUT pxlCoord2D		*origin;
INPUT pxlCoord2D		*offset;
INPUT int			numEncodings;
INPUT pxlMonoEncodedTextData	*encodedTextList;

{
/* AUTHOR: */
    pexAnnotationText2D		*pReq;
    int 			i, lenofStrings;
    pxlMonoEncodedTextData	*nextString;

    /*
     * Get length of mono-encoded strings 
     */
    GET_LENGTH_STRINGS(numEncodings, encodedTextList, lenofStrings);

    PEXInitOC( ocbuf, PEXOCAnnotationText2D, LENOF( pexAnnotationText2D), 
		lenofStrings, &pReq);
    if (pReq == NULL) return;

    /* 
     * Store the atext request header data in the oc buffer 
     */
    pReq->origin = *origin;
    pReq->offset = *offset;
    pReq->numEncodings = (CARD16) numEncodings;

    /*
     * Store the mono-encoded string
     */
    STORE_ENCODED_STRINGS(ocbuf, numEncodings, encodedTextList);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXPolyline
 *
 * FORMAT:
 * 	`PEXPolyline`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	vertices	The positions of the polyline's vertices.
 *
 *	numVertices	The number of vertices in the polyline.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *  	This routine, when processed by a renderer, draws a polyline primitive.
 *	`Vertices'
 *	is a list of points in modelling coordinates defining the
 *	polyline.  Vertices are joined together by line segments.
 *	The first vertex is joined to the second, the second to the third,
 *	and so on.  The last vertex is not joined to the first.
 *	A polyline with fewer than two vertices is considered degenerate
 *	and is treated in an implementation-dependent manner.
 *
 *	Polylines have no geometric width, only length, so transformations
 *	affect only the displayed length of a polyline.  The polyline
 *	colours are transformed only by the depth-cueing computation
 *	(the light-source shading stage of the rendering pipeline affects
 *	only surfaces) and are mapped to device colours.  Polylines
 *	are not displayed if they are outside the currently defined
 *	clipping volume.  Polylines crossing the clipping volume are
 *	clipped, and only the portions inside the clipping volume
 *	are displayed.
 *
 *	Polyline primitives are drawn using the line colour, line type,
 *	line width, and polyline interpolation method attributes.
 *	Each of these attributes has a corresponding ASF (attribute source
 *	flag).  If a polyline attribute's ASF is `PEXIndividual',
 *	the polyline attribute is obtained directly from
 *	the current value in the rendering pipeline.
 *	If a polyline attribute's ASF is `PEXBundled',
 *	the value is obtained from the line bundle lookup
 *	table entry indicated by the current line bundle index attribute.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXLineType", `PEXSetLineWidth", 
 *	`PEXSetLineColourIndex", `PEXSetLineColour", `PEXSetLineBundleIndex"
 *
 */

void
PEXPolyline (ocbuf, vertices, numVertices)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord3D	*vertices;
INPUT int		numVertices;

{
/* AUTHOR: */
    PEXAddListOC( ocbuf, PEXOCPolyline, False/*not a counted list */, 
			numVertices, sizeof( pxlCoord3D), vertices);

    return;
}

/*+
 * NAME:
 * 	PEXPolyline2D
 *
 * FORMAT:
 * 	`PEXPolyline2D`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	vertices	The positions of the polyline's vertices
 *
 *	numVertices 	The number of vertices in the polyline.
 *
 * RETURNS:
 * 	None
 *
 * DESCRIPTION:
 *	This routine is the same as `PEXPolyline", except
 *	that the vertices consist of only (X,Y) values.  The
 *	Z-value is assumed to be zero.  This primitive is two-dimensional
 *	only in that the Z-components are implied.  Geometry and colour
 *	transformation operations are still carried out in three dimensions.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXLineType", `PEXSetLineWidth", 
 *	`PEXSetLineColourIndex", `PEXSetLineColour", `PEXSetLineBundleIndex"
 *
 */

void
PEXPolyline2D (ocbuf, vertices, numVertices)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord2D	*vertices;
INPUT int		numVertices;

{
/* AUTHOR: */
    PEXAddListOC( ocbuf, PEXOCPolyline2D, False/*not a counted list */, 
			numVertices, sizeof( pxlCoord2D), vertices);

    return;
}

/*+
 * NAME:
 * 	PEXPolylineSet
 *
 * FORMAT:
 * 	`PEXPolylineSet`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	vertexAttributes
 *			A mask indicating the attributes provided
 *			for each polyline vertex.
 *
 *	polylines	A list of polylines to be
 *			inserted into the current output command
 *			buffer and subsequently rendered.
 *
 *	numPolylines	The number of
 *			polylines to be inserted into the current
 *			output command buffer.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine is similar to `PEXPolyline" except that it allows
 *	applications to create an output primitive command containing
 *	a number of polylines.  These polylines can contain a
 *	colour value at each vertex.  `VertexAttributes' is a mask.  The
 *	`PEXGAColour' bit in this mask can be set to indicate that colour values
 *	accompany the coordinate information for each vertex.
 *	This constant is defined in `PEX.h'.
 *
 *	If colours are passed for each vertex, they are used instead of
 *	the line colour attribute.  Per-vertex colours are also affected by
 *	the polyline iterpolation method.  This method is obtained
 *	directly from the polyline interpolation attribute if the
 *	`PEXPolylineInterpAsf' is `PEXIndividual' or,  if the `PEXPolylineInterpAsf'
 *	is `PEXBundled',  from the line bundle lookup table entry indicated by
 *	the current line bundle index attribute.
 *
 *	For more information on vertex data format, see the introduction
 *	to this chapter.
 *
 *	Colours in the vertex list must be in the current colour
 *	type.  (See `PEXSetColourType".)
 *
 *	This output primitive command corresponds to the PHIGS/+ concept of
 *	a "polyline set 3 with data" and causes a PEX to generate a
 *	"polyline set 3D with data" output command.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXLineType", `PEXSetLineWidth", 
 *	`PEXSetLineColourIndex", `PEXSetLineColour", `PEXSetPolylineInterpMethod",
 *	`PEXSetLineBundleIndex"                    
 *
 */

void
PEXPolylineSet (ocbuf, vertexAttributes, polylines, numPolylines)

INPUT pxlOCBuf		*ocbuf;
INPUT unsigned long	vertexAttributes;
INPUT pxlPolylineData	*polylines;
INPUT int		numPolylines;

{
/* AUTHOR: */
    int			i;
    int			numPoints;
    int			lenofVertex;
    pexPolylineSet	*pReq;
    CARD32 		*pData;

    /* 
     * Calculate the total number of vertices 
     */
    for (i=0, numPoints=0; i < numPolylines; i++)
	numPoints += polylines[i].numPoints;

    /* 
     * Calculate how big each vertex is 
     */
    lenofVertex = LENOF( pxlCoord3D) +
    		((vertexAttributes & PEXGAColour) ? 
			GetColourLength( pexColourTypeHidden) : 0); 

    PEXInitOC( ocbuf, PEXOCPolylineSet, LENOF( pexPolylineSet), 
		numPolylines/*counts*/ + (numPoints * lenofVertex),
		&pReq);
    if (pReq == NULL) return;

    /* 
     * Store the polyline request header data in the oc buffer 
     */
    pReq->colourType = pexColourTypeHidden;
    pReq->vertexAttribs = vertexAttributes;
    pReq->numLists = numPolylines;

    /* 
     * For each polyline store a count and then the list of vertices. 
     * Note that the vertices are padded to end on a word boundary
     */
    for (i=0; i<numPolylines; i++)
    {
        if ( (pData = (CARD32 *) PEXGetOCWords(ocbuf, 1)) != NULL )
	    *pData = polylines[i].numPoints;  

	PEXCopyWordsToOC( ocbuf, polylines[i].numPoints * lenofVertex, 
			polylines[i].points);
    }

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXNurbCurve
 *
 * FORMAT:
 * 	`PEXNurbCurve`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	order		The order of the polynomial expression.
 *
 *	type		The type of B-spline
 *			curve, whether rational or non-rational.
 *
 *	tmin		The minimum value of the curve parameter "t".
 *
 *	tmax		The maximum value of the curve parameter "t".
 *
 *	knots		An array of coordinates specifying the the B-spline
 *			curve knots.
 *
 *	points		An array of coordinates specifying the points
 *			defining the B-spline curve.
 *
 *	numPoints	The number of points that define the curve.
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine, when processed by a renderer, causes a non-uniform
 *	B-spline curve to be rendered.  The spline shape is specified as a
 *	series of knots (defined by argument `knots') in the parametric
 *	coordinate space and a list of points (defined by argument
 *	`points') in the modelling coordinate space.  Argument `order' is a
 *	positive integer equal to the number of knots minus the
 *	number of points.  The sequence of knots, t(0) <= t(1) <=
 *	<= t(k-1) must be a non-decreasing sequence.
 *
 *	Possible values for argument `type' are `PEXRational' or `PEXNonRational'.
 *	Include file `PEX.h' contains definitions of these constants.
 *	If `type' is `PEXRational' (the usual case), the point
 *	list must be specified in homogeneous (4D) modeling coordinates.
 *	If `type' is `PEXNonRational', the point list must be
 *	specified in non-homogeneous (3D) modeling coordinates.
 *
 *	The spline can be restricted to a specific region in the parametric
 *	coordinate space such as `tmin' < t < `tmax'.  The
 *	parametric bounds, `tmin' < `tmax', specify the region in the
 *	parametric coordinate space over which the spline is to
 *	be evaluated.  The parametric bounds must satisfy the
 *	restriction `tmin' >= t(0), `tmax' <= t(k+1).
 *
 *	Non-uniform B-spline curve primitives are drawn using the line colour,
 *	line type, line width and curve approximation attributes.  Each
 *	of these attributes has a corresponding ASF (attribute source
 *	flag).  If an attribute's ASF is `PEXIndividual', the attribute
 *	is obtained directly from the current value in the rendering
 *	pipeline.  If an attributes's ASF is `PEXBundled', the value
 *	is obtained from the line bundle lookup table entry indicated by
 *	the current line bundle index attribute.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXLineType", `PEXSetLineWidth", 
 *	`PEXSetLineColourIndex", `PEXSetLineColour", `PEXSetCurveApproxMethod", 
 *	`PEXSetLineBundleIndex"
 *
 */

void
PEXNurbCurve (ocbuf, order, type, tmin, tmax, knots, points,
	numPoints)

INPUT pxlOCBuf		*ocbuf;
INPUT int		order;
INPUT int		type;
INPUT float		tmin;
INPUT float		tmax;
INPUT float		*knots;
INPUT char		*points;
INPUT int		numPoints;

{
/* AUTHOR: */
    pexNurbCurve 	*pReq;
    int			lenofVertexList;
    int			lenofKnotList;

    /* 
     * Calculate the number of words in the vertex list and knot list.  The 
     * number of knots = order + number of points.
     */
    lenofVertexList = numPoints *
	((type == PEXRational) ? LENOF( pxlCoord4D) : LENOF( pxlCoord3D));
    lenofKnotList = order + numPoints; /* knots are FLOATs */

    PEXInitOC( ocbuf, PEXOCNurbCurve, LENOF( pexNurbCurve),
		lenofKnotList + lenofVertexList, &pReq);
    if (pReq == NULL) return;

    /* 
     * Store the nurb request header data in the oc buffer 
     */
    pReq->curveOrder = order;
    pReq->coordType = type;
    pReq->tmin = tmin;
    pReq->tmax = tmax;
    pReq->numKnots = order + numPoints;
    pReq->numPoints = numPoints;

    /*
     * Store the knot list and the vertex list
     */
    PEXCopyWordsToOC( ocbuf, lenofKnotList, knots);
    PEXCopyWordsToOC( ocbuf, lenofVertexList, points);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXFillArea
 *
 * FORMAT:
 * 	`PEXFillArea`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	shape		The shape of the polygonal area to be rendered.
 *
 *	ignoreEdges	A flag that determines if
 *			surface edge attributes are to be applied.
 *
 *	points		An array of floating point
 *			values that define the vertices of the polygon.
 *
 *	numPoints	The number of vertices in the `points' array.
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine, when processed by a renderer, causes a fill area primitive
 *	to be rendered.  The polygonal area is defined by a list of vertices,
 *	(specified by argument `points') joined together to form a planar
 *	surface.  Argument `numPoints' passes the number of vertices in the
 *	`points' array.  Polygonal areas are not strictly required to be planar,
 *	but shading artifacts can occur if a polygonal area is not planar or
 *	nearly so.  The first vertex of the polygon is connected to the
 *	second, the second to the third, and so on.  The last vertex is
 *	implicitly connected to the first.
 *
 *	Argument `Shape' indicates the type of polygon defined by `vertices',
 *	and can have the values `PEXConvex', `NonConvex', `PEXComplex', (defined
 *	in include file `X.h') or `PEXUnknownShape' (defined in include file
 *	`PEX.h').  The PEX server extension can use the `shape' parameter to
 *	provide performance improvements for simpler types of polygons.  If PEX
 *	is provided an incorrect shape specification (for example, the `shape'
 *	parameter is `PEXConvex' but the application provides a self-intersecting
 *	polygon) the result is implementation-dependent.  Consequently,
 *	applications should pass `PEXUnknownShape' as the shape unless they are
 *	certain the polygon's shape is one of the other three.
 *	The shape parameter can be ignored by a PEX server extension: for
 *	example, it may treat all polygons as type `PEXUnknownShape'.
 *
 *	`IgnoreEdges' is a boolean value specifying whether surface edge
 *	attributes are applied to the polygon.  If
 *	the value of `ignoreEdges' is `True', no surface edges are
 *	drawn for the polygon.  If `ignoreEdges' is `False', and
 *	the surface edge flag is `PEXOn', surface edges are drawn using the
 *	current surface edge attributes.
 *
 *	During the rendering process, the polygonal vertices are
 *	transformed to positions in device coordinates.  The surface
 *	colours are transformed by the light source shading
 *	computation which uses the light state, interior style,
 *	and reflection model attributes.  Surface colours are further modified
 *	by the depth-cueing computation and then mapped to device colours.
 *	Polygonal areas are outside the currently-defined clipping volume are
 *	not displayed.  Polygonal areas crossing the clipping volume are
 *	clipped, and only the portions inside the clipping volume are displayed.
 *
 *	A polygonal area with fewer than three vertices is considered
 *	degenerate and is handled in an implementation-dependent manner.
 *	A polygonal area can cross over itself to form a complex shape.
 *	The odd-even rule is used to determine points lying in
 *	the interior of the polygonal area.
 *
 *	Depending on the setting of the surface attribute ASF values, the
 *	surface colour, interior style, interior style index,
 *	surface interpolation method,  and reflection model attributes are
 *	obtained from one of two sources.  These attributes are obtained
 *	directly from the current surface attributes values or
 *	from the interior bundle lookup table entry specified by the current
 *	interior bundle index attribute.
 *
 *	When a surface is rendered, the surface colour, ambient coefficient,
 *	diffuse coefficient, specular coefficient, specular colour, specular
 *	concentration, and transmission coefficient attributes are used to
 *	compute the colours of the surface if it is front-facing with respect to
 *	the point of view.  If the surface is back-facing, the current
 *	distinguish mode is `True', and the current culling mode allows
 *	back-faces to be drawn, the corresponding back-facing attributes
 *	are used instead.
 *
 *	Digital implementations of PEX do not use the pattern size, pattern
 *	reference point, pattern reference vector 1, and pattern reference
 *	vector 2 attributes.
 *
 *	Depending on the setting of the surface edge ASF values,
 *	the surface edges, surface edge colour, surface
 *	edge type, and surface edge width attributes are obtained from one
 *	of two sources.  These attributes are obtained directly from
 *	the current surface edge attribute values or from the
 *	edge bundle lookup table entry specified by the current edge
 *	bundle index attribute.
 *
 *	This output primitive command corresponds to the PHIGS/+ concept of
 *	a "fill area 3" (without data) and causes PEX to generate a
 *	"fill area 3D" output command.
 *
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 * 	`PEXAllocateOCBuffer",  `PEXSetInteriorStyle", `PEXSetInteriorStyleIndex",
 *	`PEXSetSurfaceColourIndex", `PEXSetSurfaceColour", `PEXSetReflectionAttributes", 
 *	`PEXSetReflectionModel", `PEXSetSurfaceInterpMethod", `PEXSetBFInteriorStyle",
 *	`PEXSetBFInteriorStyleIndex", `PEXSetBFSurfaceColourIndex",  `PEXSetBFSurfaceColour", 
 *	`PEXSetBFReflectionAttributes",  `PEXSetBFReflectionModel", `PEXSetBFSurfaceInterpMethod",
 *	`PEXSetFacetCullingMode", `PEXSetFacetDistinguishFlag",  `PEXSetNormalFlipFlag", 
 *	`PEXSetPatternSize",  `PEXSetPatternRefPt", `PEXSetPatternAttributes",
 *	`PEXSetInteriorBundleIndex", `PEXSetSurfaceEdgeFlag",  `PEXSetSurfaceEdgeType", 
 *	`PEXSetSurfaceEdgeWidth",   `PEXSetSurfaceEdgeColour", `PEXSetSurfaceEdgeColourIndex",
 *	`PEXSetEdgeBundleIndex" 
 */                             

void
PEXFillArea (ocbuf, shape, ignoreEdges, points, numPoints)

INPUT pxlOCBuf		*ocbuf;
INPUT int		shape;
INPUT int		ignoreEdges;
INPUT pxlCoord3D	*points;
INPUT int		numPoints;

{
/* AUTHOR: */
    pexFillArea	*pReq;
    int		lenofVertexList;

    lenofVertexList = numPoints * LENOF( pxlCoord3D);

    PEXInitOC( ocbuf, PEXOCFillArea, LENOF( pexFillArea), lenofVertexList, 
		&pReq);
    if (pReq == NULL) return;

    /* 
     * Store the fill area request data in the oc buffer 
     */
    pReq->shape = shape;        	/* convex, non-, complex, or unknown */
    pReq->ignoreEdges = ignoreEdges;   	/* edge flag */

    PEXCopyWordsToOC( ocbuf, lenofVertexList, points);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXFillArea2D
 *
 * FORMAT:
 * 	`PEXFillArea2D`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	shape		The shape of the polygonal area to be rendered.
 *
 *	ignoreEdges	A flag that determines if
 *			surface edge attributes are applied.
 *
 *	points		An array of floating point
 *			values that define the vertices of the polygon.
 *
 *	numPoints	The number of vertices in the `points' array.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine is the same as `PEXFillArea", except
 *	that the vertices consist of only (X,Y) values.  The
 *	Z-value is assumed to be zero.  This primitive is two-dimensional
 *	only in that the Z-components are implied.  Geometry and
 *	colour transformation operations are still carried out in 3 dimensions.
 *
 *	This output primitive command corresponds to the PHIGS/+ concept of
 * 	a "fill area 2" (without data) and causes PEX to generate a
 *	"fill area 2D" output command.
 *
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer",  `PEXSetInteriorStyle", `PEXSetInteriorStyleIndex",
 *	`PEXSetSurfaceColourIndex", `PEXSetSurfaceColour",  `PEXSetReflectionAttributes", 
 *	`PEXSetReflectionModel",  `PEXSetSurfaceInterpMethod", `PEXSetBFInteriorStyle",
 *	`PEXSetBFInteriorStyleIndex", `PEXSetBFSurfaceColourIndex",  `PEXSetBFSurfaceColour", 
 *	`PEXSetBFReflectionAttributes",  `PEXSetBFReflectionModel", `PEXSetBFSurfaceInterpMethod",
 *	`PEXSetFacetCullingMode", `PEXSetFacetDistinguishFlag",  `PEXSetNormalFlipFlag", 
 *	`PEXSetPatternSize",  `PEXSetPatternRefPt", `PEXSetPatternAttributes",
 *	`PEXSetInteriorBundleIndex", `PEXSetSurfaceEdgeFlag",  `PEXSetSurfaceEdgeType", 
 *	`PEXSetSurfaceEdgeWidth",  `PEXSetSurfaceEdgeColour", `PEXSetSurfaceEdgeColourIndex",
 *	`PEXSetEdgeBundleIndex"
 *
 */

void
PEXFillArea2D (ocbuf, shape, ignoreEdges, points, numPoints)

INPUT pxlOCBuf		*ocbuf;
INPUT int		shape;
INPUT int		ignoreEdges;
INPUT pxlCoord2D	*points;
INPUT int		numPoints;

{
/* AUTHOR: */
    pexFillArea2D	*pReq;
    int			lenofVertexList;

    lenofVertexList = numPoints * LENOF( pxlCoord2D);

    PEXInitOC( ocbuf, PEXOCFillArea2D, LENOF( pexFillArea2D),
		lenofVertexList, &pReq);
    if (pReq == NULL) return;

    /* 
     * Store the fill area request data in the oc buffer 
     */
    pReq->shape = shape;        	/* convex, non-, complex, or unknown */
    pReq->ignoreEdges = ignoreEdges;   	/* edge flag */

    PEXCopyWordsToOC( ocbuf, lenofVertexList, points);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXMultiFillAreaData
 *
 * FORMAT:
 * 	`PEXMultiFillAreaData`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	shape		The shape of each of the polygons in the list.
 *
 *	ignoreEdges	A flag that determines if surface edge attributes
 *			are applied to each of the polygons in the list.
 *
 *	facetAttributes
 *			A mask indicating the attributes provided
 *			for each polygon.
 *
 *	vertexAttributes
 *			A mask indicating the attributes provided
 *			for each polygon vertex.
 *
 *	polygons	An array of data structures containing polygons.
 *
 *	numPolygons	Number of polygons in `polygons' array.
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine is similar to `PEXFillArea" except that it allows
 *	applications to create a number of polygon output primitive
 *	commands in an output command buffer with a single subroutine
 *  	call.  These polygons can contain information such
 *	as vertex or facet normals and vertex or facet colours.
 *
 *	The `shape' and `ignoreEdges' parameters are used to construct
 *	each of the output primitive commands.  These parameters apply to
 *	the entire list of polygons.
 *
 *	Each polygon is defined using a `pxlPolygonData'
 *	data structure.  Include file `PEXlib.h' contains definitions of
 *	these data structures.    Each data structure consists of a pointer to
 *	a list of vertices, the number of points in the polygon, and
 *	a pointer to data that is applied to the entire polygon (for example, a
 *	facet normal or colour).  The number of polygons in the `polygons'
 *	array must be supplied in `numPolygons'.
 *
 *	The `facetAttributes' mask indicates the contents of
 *	a polygon's "facet data" field.  For polygon primitives, this data
 *	can be a colour, a normal, or a colour followed by a normal.
 *	Applications can use the constants `PEXGAColour' and `PEXGANormal' to
 *	construct a mask indicating the data provided for
 *	each polygon.  Include file `PEX.h' contains definitions of these
 *	constants.  If supplied, the facet normal is used to
 *	determine whether the polygon is back-facing.  Depending
 *	on the current reflection model and interior style attributes, the
 *	facet normal can also be used in shading computations.  The
 *	facet colour, if provided, is used instead of the current (front-face
 *	or back-face) surface colour to provide the intrinsic colour of the
 *	surface.
 *
 *	The `vertexAttributes' mask indicates the data present
 *	at each polygon vertex.  The (x,y,z) coordinate is assumed
 *	to be always provided.  In addition, applications can use
 *	the constants `PEXGAColour' and `PEXGANormal' to construct a mask
 *	indicating that a colour, a normal, or a colour followed by a normal
 *	is present as part of the vertex information.  If present, vertex
 *	normals and colours can be used in the shading computations.
 *
 *	All attributes affecting the representation of the `PEXFillArea"
 *	primitive affect the representation of this primitive.  If colour
 *	values are passed for each vertex, they are used instead of the surface
 *	colour attribute.  Per-vertex colours are also affected by the surface
 *	interpolation method.  This method is obtained directly from the
 *	surface interpolation attribute if the `PEXSurfaceInterpAsf' is
 *	`PEXIndividual'.  If the `PEXSurfaceInterpAsf' is `PEXBundled', the surface
 *	interpolation method is obtained from the interior bundle lookup table
 *	entry indicated by the current interior bundle index attribute.
 *
 *	For more information about the format of facet and vertex data, see
 *	the introduction to this chapter.
 *
 *	Colours in the facet and vertex lists must be in the current colour
 *	type.  (See `PEXSetColourType".)
 *
 *	This output primitive command corresponds to the PHIGS/+ concept of
 *	a "fill area 3 with data" and causes PEX to generate multiple
 *	"fill area 3D with data" output commands.
 *
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer",  `PEXSetInteriorStyle", `PEXSetInteriorStyleIndex",
 *	`PEXSetSurfaceColourIndex", `PEXSetSurfaceColour",  `PEXSetReflectionAttributes", 
 *	`PEXSetReflectionModel",  `PEXSetSurfaceInterpMethod", `PEXSetBFInteriorStyle",
 *	`PEXSetBFInteriorStyleIndex", `PEXSetBFSurfaceColourIndex",  `PEXSetBFSurfaceColour", 
 *	`PEXSetBFReflectionAttributes",  `PEXSetBFReflectionModel", `PEXSetBFSurfaceInterpMethod",
 *	`PEXSetFacetCullingMode", `PEXSetFacetDistinguishFlag",  `PEXSetNormalFlipFlag", 
 *	`PEXSetPatternSize",  `PEXSetPatternRefPt", `PEXSetPatternAttributes",
 *	`PEXSetInteriorBundleIndex", `PEXSetSurfaceEdgeFlag",  `PEXSetSurfaceEdgeType", 
 *	`PEXSetSurfaceEdgeWidth",  `PEXSetSurfaceEdgeColour", `PEXSetSurfaceEdgeColourIndex",
 *	`PEXSetEdgeBundleIndex"
 *
 */

void
PEXMultiFillAreaData (ocbuf, shape, ignoreEdges, facetAttributes, vertexAttributes,
	polygons, numPolygons)

INPUT pxlOCBuf		*ocbuf;
INPUT int		shape;
INPUT int		ignoreEdges;
INPUT unsigned long	facetAttributes;
INPUT unsigned long	vertexAttributes;
INPUT pxlPolygonData	*polygons;
INPUT int		numPolygons;

{
/* AUTHOR: */
    pexExtFillArea 	*pReq;
    CARD32		*pData;
    int			lenofFacetData;
    int			lenofVertex;
    int			lenofColour;
    int			i;

    /* 
     * Calculate the number of words in the optional facet data and the
     * number of words per vertex
     */
    lenofColour = GetColourLength( pexColourTypeHidden);
    lenofFacetData = GetFacetDataLength( facetAttributes, lenofColour); 
    lenofVertex = GetVertexWithDataLength( vertexAttributes, lenofColour);

    /* 
     * For each polygon, generate a separate output command 
     */
    for (i=0; i<numPolygons; i++)
    {
        PEXInitOC( ocbuf, PEXOCExtFillArea, LENOF( pexExtFillArea),
	    lenofFacetData + 1/*count*/ + 
		polygons[i].numPoints * lenofVertex,
	    &pReq);
        if (pReq == NULL) break;

        /* 
         * Store the fill area request header data in the oc buffer 
         */
        pReq->shape = shape;
        pReq->ignoreEdges = ignoreEdges;
        pReq->colourType = pexColourTypeHidden;
        pReq->facetAttribs = facetAttributes;
        pReq->vertexAttribs = vertexAttributes;

        PEXCopyWordsToOC( ocbuf, lenofFacetData, polygons[i].facetData);

        if ( (pData = (CARD32 *) PEXGetOCWords(ocbuf, 1)) != NULL )
	    *pData = polygons[i].numPoints;

        PEXCopyWordsToOC( ocbuf, lenofVertex * polygons[i].numPoints,
		polygons[i].points);

	PEXFinishOC( ocbuf);
    }

    return;
}

/*+
 * NAME:
 * 	PEXFillAreaSet
 *
 * FORMAT:
 * 	`PEXFillAreaSet`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	shape		The shape of each of the contours defining
 *			the complex polygon.
 *
 *	ignoreEdges	A flag that determines if
 *			surface edge attributes are applied.
 *
 *	contourHint 	Flag to indicate whether contours are disjoint or
 *			whether they overlap	
 *
 *	polygons	A pointer to an array of polygons defining the
 *			contours of the complex polygon.
 *
 *	numPolygons	The number of polygons in the `polygons' array.
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine is similar to `PEXFillArea", but allows for the creation
 *	of polygons with "islands" or "holes".  When processed by a renderer,
 *	this output command causes a complex polygon primitive to be rendered.
 *	A complex polygon consists of an array of polygons
 *	that define "contours" (disjoint pieces or holes) making up
 *	the primitive.  Each polygonal area, or contour, is defined by
 *	a list of vertices joined together to form a planar surface.
 *	The list of contours defining the complex polygon is specified
 *	by the `polygons' list, and `numPolygons' indicates the number of
 *	polygons in that list.
 *
 *	Argument `shape' indicates the type of each contour in the `polygons'
 *	array and can have the values `PEXConvex', `NonConvex', `PEXComplex', 
 *	or `PEXUnknownShape' (defined in include file
 *	`PEX.h').  The PEX server extension can use the `shape' parameter to
 *	provide performance  improvements for simpler types of polygons.  If
 *	PEX is provided an incorrect shape specification (for example, the
 *	`shape' parameter is `PEXConvex' but the application provides a
 *	self-intersecting polygon) the result is implementation-dependent.
 *	Consequently, applications should pass `PEXUnknownShape' as the shape unless
 *	they are certain the polygon's shape is one of the other three.
 *	The shape parameter can be ignored by PEX server extensions (that is,
 *	it can treat all polygons as type `PEXUnknownShape'.)
 *
 *	`IgnoreEdges' is a boolean value specifying whether surface edge
 *	attributes are applied to the polygon.  If
 *	the value of `ignoreEdges' is `True', no surface edges are
 *	drawn for the polygon.  If `ignoreEdges' is `False' and the surface
 *	edge flag is `PEXOn', surface edges are drawn using the current surface
 *	edge attributes.
 *
 *	The `contourHint' parameter provides further information as to the 
 *	relationships between contours in the fill area set.  The possible 
 *	values (defined in PEX.h) are `PEXDisjoint', `PEXNested', 
 *	`PEXIntersecting' and `PEXUnknownContour'.   If `contourHint' is 
 *	`PEXDisjoint', all contours will be spatially disjoint.  No overlapping 
 *	or intersection occurs between any contours in the fill area set.  If 
 *	`contourHint' is `PEXNested', contours will either be disjoint or 
 *	wholly contained within another contour.  No contour will have edges 
 *	that intersect or are coincident with edges of any other contour.  If 
 *	`contourHint' is `PEXIntersecting', separated contours may have edges 
 *	that are coincident or overlap. If `contourHint' is `PEXUnknownContour' 
 *	nothing is known about the interrelationships between contours.  Fill 
 *	area sets with contours that have higher complexity interrelationships 
 *	than that indicated by `contourHint' are drawn in an 
 *	implementation-dependent manner.  PEX server extensions may ignore 
 *	the contour hint and treat all fill area sets as `PEXUnknownContour'.
 *
 *	During the rendering process, the polygonal vertices are
 *	transformed to positions in device coordinates.  The surface
 *	colours are transformed by the light source shading
 *	computation (which uses the light state, interior style,
 *	reflection model attributes).  Surface colours are further modified by
 *	the depth-cueing computation and mapped to device colours.
 *	Polygonal areas outside the currently-defined clipping volume are not
 *	displayed.  Polygonal areas crossing the clipping volume are clipped,
 *	and only the portions inside the clipping volume are displayed.
 *
 *	A polygonal area with fewer than three vertices is considered
 *	degenerate and is handled in an implementation-dependent manner.
 *	A polygonal area can cross over itself to form a complex shape.
 *	The odd-even rule is used to determine the points lying in
 *	the interior of the polygonal area.
 *
 *	All attributes which affect the representation of the fill area 
 *	primitives also affect the representation of the fill area set 
 *	primitives.
 *
 *	This output primitive command corresponds to the PHIGS/+ concept of
 *	a "fill area set 3" (without data) and causes PEX to generate a
 *	"fill area set 3D" output command.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer",  `PEXSetInteriorStyle", `PEXSetInteriorStyleIndex",
 *	`PEXSetSurfaceColourIndex", `PEXSetSurfaceColour",  `PEXSetReflectionAttributes", 
 *	`PEXSetReflectionModel",  `PEXSetSurfaceInterpMethod", `PEXSetBFInteriorStyle",
 *	`PEXSetBFInteriorStyleIndex", `PEXSetBFSurfaceColourIndex",  `PEXSetBFSurfaceColour", 
 *	`PEXSetBFReflectionAttributes",  `PEXSetBFReflectionModel", `PEXSetBFSurfaceInterpMethod",
 * 	`PEXSetFacetCullingMode", `PEXSetFacetDistinguishFlag",  `PEXSetNormalFlipFlag", 
 *	`PEXSetPatternSize",  `PEXSetPatternRefPt", `PEXSetPatternAttributes",
 *	`PEXSetInteriorBundleIndex", `PEXSetSurfaceEdgeFlag",  `PEXSetSurfaceEdgeType", 
 *	`PEXSetSurfaceEdgeWidth",  `PEXSetSurfaceEdgeColour", `PEXSetSurfaceEdgeColourIndex",
 *	`PEXSetEdgeBundleIndex"
 *
 */

void
PEXFillAreaSet (ocbuf, shape, ignoreEdges, contourHint, polygons, numPolygons)

INPUT pxlOCBuf		*ocbuf;
INPUT int		shape;
INPUT int		ignoreEdges;
INPUT int		contourHint;
INPUT pxlPolygonData	*polygons;
INPUT int		numPolygons;

{
/* AUTHOR: */
    pexFillAreaSet	*pReq;
    CARD32		*pData;
    int			i;
    int			numPoints;

    /* 
     * Calculate the total number of vertices 
     */
    for (i=0, numPoints=0; i < numPolygons; i++)
	numPoints += polygons[i].numPoints;

    PEXInitOC( ocbuf, PEXOCFillAreaSet, LENOF( pexFillAreaSet),
	    numPolygons/*counts*/ + (numPoints * LENOF( pxlCoord3D)),
	    &pReq);
    if (pReq == NULL) return;

    /* 
     * Store the fill area set request header data in the oc buffer 
     */
    pReq->shape = shape; 
    pReq->ignoreEdges = ignoreEdges;
    pReq->contourHint = contourHint;
    pReq->numLists = numPolygons;

    /*
     * Now store the fill area set .. each fill area in the set consists of
     * a vertex count followed a polygon 
     */
    for (i=0; i < numPolygons; i++)
    {
        if ( (pData = (CARD32 *) PEXGetOCWords(ocbuf, 1)) != NULL )
	    *pData = polygons[i].numPoints;  

	PEXCopyWordsToOC( ocbuf, polygons[i].numPoints * LENOF( pxlCoord3D), 
		polygons[i].points);
    }

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXFillAreaSet2D
 *
 * FORMAT:
 * 	`PEXFillAreaSet2D`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	shape		The shape of the polygonal areas to be rendered.
 *
 *	ignoreEdges	A flag that determines if
 *			surface edge attributes are applied.
 *
 *	contourHint 	Flag to indicate whether contours are disjoint or
 *			whether they overlap	
 *
 *	polygons	A pointer to an array of polygons defining the
 *			contours of the complex polygon.
 *
 *	numPolygons	The number of polygons in the `polygons' array.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine is the same as `PEXFillAreaSet", except
 *	that the vertices consist of only (X,Y) values.  The
 *	Z-value is assumed to be zero.  This primitive is two-dimensional
 *	only in that the Z-components are implied.  Geometry and
 *  	colour transformation operations are still carried out in three
 *	dimensions.
 *
 *	This output primitive command corresponds to the PHIGS/+ concept of
 *	a "fill area set 2" (without data) and causes PEX to generate a
 *	"fill area set 2D" output command.
 *
 * ERRORS:
 * 	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer",  `PEXSetInteriorStyle", `PEXSetInteriorStyleIndex",
 *	`PEXSetSurfaceColourIndex", `PEXSetSurfaceColour",  `PEXSetReflectionAttributes", 
 *	`PEXSetReflectionModel",  `PEXSetSurfaceInterpMethod", `PEXSetBFInteriorStyle",
 *	`PEXSetBFInteriorStyleIndex", `PEXSetBFSurfaceColourIndex",  `PEXSetBFSurfaceColour", 
 *	`PEXSetBFReflectionAttributes",  `PEXSetBFReflectionModel", `PEXSetBFSurfaceInterpMethod",
 *	`PEXSetFacetCullingMode", `PEXSetFacetDistinguishFlag",  `PEXSetNormalFlipFlag", 
 *	`PEXSetPatternSize",  `PEXSetPatternRefPt", `PEXSetPatternAttributes",
 *	`PEXSetInteriorBundleIndex", `PEXSetSurfaceEdgeFlag",  `PEXSetSurfaceEdgeType", 
 *	`PEXSetSurfaceEdgeWidth",  `PEXSetSurfaceEdgeColour", `PEXSetSurfaceEdgeColourIndex",
 *	`PEXSetEdgeBundleIndex"
 *
 */

void
PEXFillAreaSet2D (ocbuf, shape, ignoreEdges, contourHint, polygons, numPolygons)

INPUT pxlOCBuf		*ocbuf;
INPUT int		shape;
INPUT int		ignoreEdges;
INPUT int		contourHint;
INPUT pxlPolygonData	*polygons;
INPUT int		numPolygons;

{
/* AUTHOR: */
    pexFillAreaSet2D	*pReq;
    CARD32		*pData;
    int			i;
    int			numPoints;

    /* 
     * Calculate the total number of vertices 
     */
    for (i=0, numPoints=0; i < numPolygons; i++)
	numPoints += polygons[i].numPoints;

    PEXInitOC( ocbuf, PEXOCFillAreaSet2D, LENOF( pexFillAreaSet2D),
	    numPolygons/*counts*/ + (numPoints * LENOF( pxlCoord2D)), 
	    &pReq);
    if (pReq == NULL) return;

    /* 
     * Store the fill area set request header data in the oc buffer 
     */
    pReq->shape = shape;
    pReq->ignoreEdges = ignoreEdges;
    pReq->contourHint = contourHint;
    pReq->numLists = numPolygons;

    /*
     * Now store the fill area set .. each fill area in the set consists of
     * a vertex count followed a polygon 
     */
    for (i=0; i < numPolygons; i++)
    {
        if ( (pData = (CARD32 *) PEXGetOCWords(ocbuf, 1)) != NULL )
	    *pData = polygons[i].numPoints;  

	PEXCopyWordsToOC( ocbuf, polygons[i].numPoints * LENOF( pxlCoord2D), 
		polygons[i].points);
    }

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXMultiFillAreaSetData
 *
 * FORMAT:
 * 	`PEXMultiFillAreaSetData`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	shape		The shape of the polygonal areas to be rendered.
 *
 *	ignoreEdges	A flag that determines if
 *			surface edge attributes are applied.
 *
 *	contourHint 	Flag to indicate whether contours are disjoint or
 *			whether they overlap	
 *
 *	facetAttributes
 *			A mask indicating the attributes provided
 *			for each fill area set .
 *
 *	vertexAttributes
 *			A mask indicating the attributes provided
 *			for each fill area set vertex.
 *
 *	FASList
 *			A pointer to an array of fill area sets
 *
 *	numFAS
 *			The number of polygons in the `polygons' array.
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine is similar to `PEXFillAreaSet" except that it allows
 *	applications to create a number of fill area set output primitive
 * 	commands in an output command buffer with a single subroutine
 * 	call.  These fill area sets can contain information such
 *	as vertex or facet normals, vertex or facet colours, and edge controls.
 *
 *	The `shape' and `contourHint' are used to 
 *	construct each of the output primitive commands and apply to the entire
 *	list of fill area sets.
 *
 *	Each fill area set is defined using
 *	a `pxlComplexPolygonData' data structure.  Include file `PEXlib.h'
 *	contains definitions for these data structures.  A
 *	`pxlComplexPolygonData' data structure consists of a pointer to data
 *	applied to the entire complex polygon primitive (for example, a
 *	facet normal or colour), the
 *	number of contours in the fill area set and a pointer to an
 *	array of `pxlContourData' structures which define each of these
 *	"contours".   Each `pxlContourData' structure
 *	contains a list of vertices and the number of points in the polygon.
 *	The `numFAS' argument supplies the
 *	number of fill area sets passed to this routine in the `FASList'
 *	array.
 *
 *	The `facetAttributes' mask is used to indicate the contents of
 *	a fill area set's "facet data" field.  For fill area set
 *	primitives, this data can be a colour, a normal, or a colour
 *	followed by a normal.  Since a fill area set is considered
 *	a single, planar, surface primitive, there is only one set of facet
 *	data.  Applications can use the constants `PEXGAColour' and
 *	`PEXGANormal' to construct a mask indicating the data provided
 *	for each fill area set.  Include file `PEX.h' contains
 *	definitions of these constants.   The facet normal, if supplied,
 *	determines whether the fill area set is back-facing.  Depending
 *	on the current reflection model and interior style attributes,
 *	the facet normal can also be used in shading computations.
 *	The facet colour, if provided, is used instead of the current
 *	(front-face or back-face) surface colour to provide the intrinsic
 *	colour of the surface.
 *
 *	The `vertexAttributes' mask indicates the data present
 *	at each polygon vertex.  The (x,y,z) coordinate is assumed
 *	to be always provided.  Application programs can use
 *	the constants `PEXGAColour', `PEXGANormal', and `PEXGAEdges' to construct a mask
 *	indicating whether colour, normal, and edge control information
 *	is present as part of the vertex information.
 *	If present, vertex normals and colours can be used in the
 *	shading computations.  Edge controls are used to indicate
 *	those edges drawn if the surface edge-drawing
 *	is enabled.  The edge control for vertex `i' indicates whether or
 *	not to draw the edge between vertex `i' and vertex `i+1'.
 *
 *	All attributes affecting the representation of fill areas also
 *	affect the representation of the fill area set primitive.
 *	If colours are passed as part of the facet data or the vertex data,
 *	they are used instead of the surface colour attribute.  Per-vertex
 *	colours are also affected by the surface interpolation method.
 *	This method is obtained directly from the surface interpolation
 *	attribute if the `PEXSurfaceInterpAsf' is `PEXIndividual'.  If the
 *	`PEXSurfaceInterpAsf' is `PEXBundled', the surface interpolation method is
 *	obtined from the interior bundle table entry indicated by the current
 *	interior bundle index attribute .
 *
 *	Colours in the vertex and facet lists must be in the current colour
 *	type  (See `PEXSetColourType".)
 *
 *	For more information about the format of facet and vertex data, see
 *	the introduction to this chapter.
 *
 *	This output primitive command corresponds to the PHIGS/+ concept of
 *	a "fill area set 3 with data" and causes PEX to generate multiple
 *	"fill area set 3D with data" output commands.
 *
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer",  `PEXSetInteriorStyle", `PEXSetInteriorStyleIndex",
 *	`PEXSetSurfaceColourIndex", `PEXSetSurfaceColour",  `PEXSetReflectionAttributes", 
 *	`PEXSetReflectionModel",  `PEXSetSurfaceInterpMethod", `PEXSetBFInteriorStyle",
 *	`PEXSetBFInteriorStyleIndex", `PEXSetBFSurfaceColourIndex",  `PEXSetBFSurfaceColour", 
 *	`PEXSetBFReflectionAttributes",  `PEXSetBFReflectionModel", `PEXSetBFSurfaceInterpMethod",
 *	`PEXSetFacetCullingMode", `PEXSetFacetDistinguishFlag",  `PEXSetNormalFlipFlag", 
 *	`PEXSetPatternSize",  `PEXSetPatternRefPt", `PEXSetPatternAttributes",
 *	`PEXSetInteriorBundleIndex", `PEXSetSurfaceEdgeFlag",  `PEXSetSurfaceEdgeType", 
 *	`PEXSetSurfaceEdgeWidth",  `PEXSetSurfaceEdgeColour", `PEXSetSurfaceEdgeColourIndex",
 *	`PEXSetEdgeBundleIndex"
 *
 */

void
PEXMultiFillAreaSetData (ocbuf, shape, ignoreEdges, contourHint, 
	facetAttributes, vertexAttributes, FASList, numFAS)

INPUT pxlOCBuf			*ocbuf;
INPUT int			shape;
INPUT int			ignoreEdges;
INPUT int			contourHint;
INPUT unsigned long		facetAttributes;
INPUT unsigned long		vertexAttributes;
INPUT pxlComplexPolygonData	*FASList;
INPUT int			numFAS;

{
/* AUTHOR: */
    pxlComplexPolygonData	*ppolyData;
    int				lenofColour;
    int				lenofFacetData;
    int				lenofVertex;
    int				i;
    int				j;
    int				numPoints;
    pexExtFillAreaSet		*pReq;
    CARD32			*pData;

    /* 
     * Calculate the size of the facet data and vertex data
     */
    lenofColour = GetColourLength( pexColourTypeHidden);
    lenofFacetData = GetFacetDataLength( facetAttributes, lenofColour); 
    lenofVertex = GetVertexWithDataLength( vertexAttributes, lenofColour);
    if (vertexAttributes & PEXGAEdges) lenofVertex++; /* edge switch is CARD32 */

    /* 
     * Generate a separate command for each fill area set 
     */
    for (i=0; i < numFAS; i++)
    {
	ppolyData = &FASList[i];

	for (j=0, numPoints = 0; j < ppolyData->numPolygons; j++)
	    numPoints += (ppolyData->polygons[j]).numPoints;

        PEXInitOC( ocbuf, PEXOCExtFillAreaSet, LENOF( pexExtFillAreaSet), 
		lenofFacetData + ppolyData->numPolygons/*counts*/ + 
			numPoints * lenofVertex,
		&pReq);
        if (pReq == NULL) break;

        /* 
         * Store the fill area set request header data in the oc buffer 
         */
        pReq->shape = shape;
        pReq->ignoreEdges = ignoreEdges;
        pReq->contourHint = contourHint;
        pReq->colourType = pexColourTypeHidden;
        pReq->facetAttribs = facetAttributes;
        pReq->vertexAttribs = vertexAttributes;
        pReq->numLists = ppolyData->numPolygons;

   	/*
   	 * Copy the facet data
	 */
	PEXCopyWordsToOC( ocbuf, lenofFacetData, ppolyData->facetData);

        /*
         * Copy the polygon vertices, preceded by a count
         * Note that the vertices are padded to end on a word boundary
 	 */
        for (j=0; j < ppolyData->numPolygons; j++)
	{
            if ( (pData = (CARD32 *)PEXGetOCWords(ocbuf, 1))!= NULL )
		*pData = ppolyData->polygons[j].numPoints; 

	    PEXCopyWordsToOC( ocbuf, 
			ppolyData->polygons[j].numPoints * lenofVertex,
			ppolyData->polygons[j].points);
	}

        PEXFinishOC( ocbuf);
    }

    return;
}

/*+
 * NAME:
 * 	PEXSetOfFillAreaSets
 *
 * FORMAT:
 * 	`PEXSetOfFillAreaSets
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	shape		The shape of the polygonal areas to be rendered.
 *
 *	contourHint 	Flag to indicate whether contours are disjoint or
 *			whether they overlap	
 *
 * 	contoursAllOne  True if each fill area set contains only one
 *			contour; False otherwise
 *
 *	FASAttributes
 *			A mask indicating the attributes provided
 *			for each fill area set 
 *
 *	vertexAttributes
 *			A mask indicating the attributes provided
 *			for each complex polygon vertex.
 *
 *      edgeAttributes  A mask indicating the attributes provided for each
 *			edge
 *
 * 	numFAS		Number of fill area sets in FASList
 *
 * 	numVertices 	Number of vertices in verticeList	
 *
 * 	numEdges	Number of edges in edgeList
 *
 * 	numContours 	Total number of contours in primitive
 *
 *	FASOptDataList	List of optional data for each fill area set
 *			(colors and/or normals)
 *
 *	verticeList     List of vertices
 *
 *	edgeList 	List of edge data (on/off switch)
 *
 *	FASList 	List of fill area sets
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this command will draw a set of fill 
 *	area sets that may be connected (ie: individual fill area sets may 
 *	share geometry and attribute information at vertices).  Shading 
 * 	calculations and transformations need only be performed once per 
 *	shared vertex instead of once for every fill area set that shares 
 *	the vertex.  Similarly, data can be transmitted across the network 
 *	once per unique vertex instead of once for every fill area set sharing 
 *	the vertex.
 *
 *	Argument `shape' indicates the type of contours that comprise each
 *	fill area set and can have the values `PEXConvex', `PEXNonConvex', 
 *	`PEXComplex', or `PEXUnknownShape' (defined in include file
 *	`PEX.h').  The PEX server extension can use the `shape' parameter to
 *	provide performance  improvements for simpler types of polygons.  If
 *	PEX is provided an incorrect shape specification (for example, the
 *	`shape' parameter is `PEXConvex' but the application provides a
 *	self-intersecting polygon) the result is implementation-dependent.
 *	Consequently, applications should pass `PEXUnknownShape' as the shape 
 *	unless they are certain the polygon's shape is one of the other three.
 *	The shape parameter can be ignored by PEX server extensions (that is,
 *	it can treat all polygons as type `PEXUnknownShape'.)
 *
 *	The `contourHint' parameter provides further information as to the 
 *	relationships between contours in the fill area set.  The possible 
 *	values (defined in PEX.h) are `PEXDisjoint', `PEXNested', 
 *	`PEXIntersecting' and `PEXUnknownContour'.   If `contourHint' is 
 *	`PEXDisjoint', all contours will be spatially disjoint.  No overlapping 
 *	or intersection occurs between any contours in the fill area set.  If 
 *	`contourHint' is `PEXNested', contours will either be disjoint or 
 *	wholly contained within another contour.  No contour will have edges 
 *	that intersect or are coincident with edges of any other contour.  If 
 *	`contourHint' is `PEXIntersecting', separated contours may have edges 
 *	that are coincident or overlap. If `contourHint' is `PEXUnknownContour' 
 *	nothing is known about the interrelationships between contours.  Fill 
 *	area sets with contours that have higher complexity interrelationships 
 *	than that indicated by `contourHint' are drawn in an 
 *	implementation-dependent manner.  PEX server extensions may ignore 
 *	the contour hint and treat all fill area sets as `PEXUnknownContour'.
 *
 *	The `FASAttributes' mask is used to indicate which attributes aree 
 *	specified for each fill area set.  This data which is contained in 
 *	`FASOptDataList' can be a colour, a normal, or a colour followed 
 *	by a normal.  Applications can use the constants `PEXGAColour' and
 *	`PEXGANormal' to construct a mask indicating the data provided
 *	for each fill area set.  Include file `PEX.h' contains
 *	definitions of these constants.   The facet normal, if supplied,
 *	determines whether the complex polygon is back-facing.  Depending
 *	on the current reflection model and interior style attributes,
 *	the facet normal can also be used in shading computations.
 *	The facet colour, if provided, is used instead of the current
 *	(front-face or back-face) surface colour to provide the intrinsic
 *	colour of the surface.
 *
 *	The `vertexAttributes' mask indicates the data present
 *	at each polygon vertex in the vertexList.  The (x,y,z) coordinate is 
 *	assumed to be always provided.  Application programs can use
 *	the constants `PEXGAColour'  and `PEXGANormal' to construct a mask
 *	indicating whether colour and normal information
 *	is present as part of the vertex information.
 *	If present, vertex normals and colours can be used in the
 *	shading computations.  
 *
 *	The `edgeAttributes' mask specifies the attributes that are specified 
 *	for each edge. The edge attributes are in `edgeList'.  The edge
 *	attributes, if present, are set to `PEXOn' or `PEXOff' and are
 *	used to indicate which edges should be drawn.
 *	The edge control for vertex `i' indicates whether or
 *	not to draw the edge between vertex `i' and vertex `i+1'.
 *
 *	The connectivity of the primitive is defined by the `FASList' array.
 * 	The number of contours in the first fill area is contained as the 
 *	first entry in the `FASList' array.  The number of contours is 
 *	followed by a list of data records, one for each of the contours in 
 *	the fill area set.  If this number is n, then the next n data records 
 *	in the `FASList' array are used to define the first fill area set.  
 *	The value following the contour count contains the number of vertices 
 *	in the first contour of the first fill area set.  If this number is m,
 *	then the next m values in the array comprise the data record for the 
 *	first contour.  This data record contains the indices for the vertices 
 *	of the first contour.  Depending on n, the next value in the list is 
 *	either the number of vertices in the second contour for the first fill 
 *	area set or it is the number of contours in the second fill area set.  
 *	As a special case, if `contoursAllOne' is True then the contour count 
 *	field in each fill area set is guaranteed to be one. `numContours'
 *	is the sum of all the contours in all of the fill area sets.
 *
 * 	Vertices are numbered with indices starting from zero (ie: the first 
 *	vertex is referenced as vertex 0)
 *
 *	All attributes affecting the representation of fill areas also
 *	affect the representation of this primitive primitive.
 *	If colours are passed as part of the facet data or the vertex data,
 *	they are used instead of the surface colour attribute.  Per-vertex
 *	colours are also affected by the surface interpolation method.
 *	This method is obtained directly from the surface interpolation
 *	attribute if the `PEXSurfaceInterpAsf' is `PEXIndividual'.  If the
 *	`PEXSurfaceInterpAsf' is `PEXBundled', the surface interpolation method is
 *	obtined from the interior bundle table entry indicated by the current
 *	interior bundle index attribute .
 *
 *	Colours in the vertex and facet lists must be in the current colour
 *	type  (See `PEXSetColourType".)
 *
 *	For more information about the format of facet and vertex data, see
 *	the introduction to this chapter.
 *
 *	This output primitive command corresponds to the PHIGS/+ concept of
 *	a "set of fill area set 3 with data".
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer",  `PEXSetInteriorStyle", `PEXSetInteriorStyleIndex",
 *	`PEXSetSurfaceColourIndex", `PEXSetSurfaceColour",  `PEXSetReflectionAttributes", 
 *	`PEXSetReflectionModel",  `PEXSetSurfaceInterpMethod", `PEXSetBFInteriorStyle",
 *	`PEXSetBFInteriorStyleIndex", `PEXSetBFSurfaceColourIndex",  `PEXSetBFSurfaceColour", 
 *	`PEXSetBFReflectionAttributes",  `PEXSetBFReflectionModel", `PEXSetBFSurfaceInterpMethod",
 *	`PEXSetFacetCullingMode", `PEXSetFacetDistinguishFlag",  `PEXSetNormalFlipFlag", 
 *	`PEXSetPatternSize",  `PEXSetPatternRefPt", `PEXSetPatternAttributes",
 *	`PEXSetInteriorBundleIndex", `PEXSetSurfaceEdgeFlag",  `PEXSetSurfaceEdgeType", 
 *	`PEXSetSurfaceEdgeWidth",  `PEXSetSurfaceEdgeColour", `PEXSetSurfaceEdgeColourIndex",
 *	`PEXSetEdgeBundleIndex"
 *
 */

void
PEXSetOfFillAreaSets (ocbuf, shape, contourHint, contoursAllOne, 
	FASAttributes, vertexAttributes, edgeAttributes, 
 	numFAS, numVertices, numEdges, numContours,
	FASOptDataList, verticeList, edgeList, FASList)

INPUT pxlOCBuf			*ocbuf;
INPUT int			shape;
INPUT int			contourHint;
INPUT int			contoursAllOne;
INPUT unsigned long		FASAttributes;
INPUT unsigned long		vertexAttributes;
INPUT unsigned long		edgeAttributes;
INPUT int 			numFAS;
INPUT int 			numVertices; 
INPUT int			numEdges; 
INPUT int			numContours; 
INPUT char			*FASOptDataList;
INPUT char			*verticeList;
INPUT char			*edgeList;
INPUT char			*FASList;

{
/* AUTHOR: */
    int				lenofColour;
    int				lenofFASOptData;
    int				lenofVertex;
    int				sizofEdge;
    int				i;
    int				j;
    pexSOFAS			*pReq;

    /* 
     * Calculate the size of the facet data and vertex data
     */
    lenofColour = GetColourLength( pexColourTypeHidden);
    lenofFASOptData = GetFacetDataLength( FASAttributes, lenofColour); 
    lenofVertex = GetVertexWithDataLength( vertexAttributes, lenofColour);
    sizofEdge = ((edgeAttributes == PEXOn) ? sizeof(CARD8) : 0);

    /* 
     * Generate a separate command for each fill area set 
     */
    PEXInitOC( ocbuf, PEXOCSOFAS, LENOF( pexSOFAS), 
	    (lenofFASOptData * numFAS) + (lenofVertex * numVertices) + 
	      NUMWORDS(sizofEdge * numEdges) +
	      NUMWORDS(sizeof(CARD16) * (numFAS + numContours + numEdges) ),
	    &pReq);
    if (pReq == NULL) return;

    /* 
     * Store the fill area set request header data in the oc buffer 
     */
    pReq->shape = shape;
    pReq->colourType = pexColourTypeHidden;
    pReq->FAS_Attributes = FASAttributes;
    pReq->vertexAttributes = vertexAttributes;
    pReq->edgeAttributes = edgeAttributes;
    pReq->contourHint = contourHint;
    pReq->contourCountsFlag = contoursAllOne;
    pReq->numFAS = numFAS;
    pReq->numVertices = numVertices;
    pReq->numEdges = numEdges;
    pReq->numContours = numContours;

    if (lenofFASOptData)
	PEXCopyWordsToOC( ocbuf, lenofFASOptData * numFAS, FASOptDataList);

    PEXCopyWordsToOC( ocbuf, lenofVertex * numVertices, verticeList);

    if (edgeAttributes)
        PEXCopyBytesToOC( ocbuf, sizeof(CARD8) * numEdges, edgeList);

    PEXCopyBytesToOC( ocbuf, sizeof(CARD16) * (numFAS + numContours + numEdges),
	    FASList );

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXTriangleStrip
 *
 * FORMAT:
 * 	`PEXTriangleStrip`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	facetAttributes
 *			A mask indicating the attributes provided
 *			for each triangle in the triangle strip.
 *
 *	vertexAttributes
 *			A mask indicating the attributes provided
 *			for each vertex in the triangle strip.
 *
 *	facetData	The list of facet data for the triangle strip.
 *
 *	points		The list of vertices defining the triangle strip.
 *
 *	numPoints	The number of vertices in the triangle strip.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *  	This routine, when processed by a renderer, causes a triangle strip
 *	primitive to be rendered.
 *
 *	The triangle strip is created from the vertex array.  There are
 *	`numPoints' - 2 facets in the strip.  The first triangle in the strip
 *	is formed from the first three vertices in the list, the second
 *	triangle is formed by the second through the fourth vertices in
 *	the list, etc., up to the last triangle, which is formed by the
 *	last three vertices in the list.  A strip with fewer than
 *	three vertices is considered degenerate and is handled in an
 *	implementation-dependent manner.
 *
 *	If facet normals are not provided,
 *	they are computed as for the polygon primitive for the odd-numbered
 *	triangles (that is, first, third, fifth, etc.).  To compute
 *	facet normals for the even-numbered triangles, the vertex order is
 *	reversed and the normal is computed as for the polygon primitive.
 *
 *	The `facetAttributes' mask is used to indicate the contents of
 *	the data in the `facetData' list.  There must be one data
 *	record in the `facetData' list for each triangle in the strip.
 *	For triangle strip primitives, this data can be a colour, a normal,
 *	or a colour followed by a normal.  Applications can use the constants
 *	`PEXGAColour' and `PEXGANormal' to construct a mask indicating the data
 *	provided for each triangle.
 *	Include file `PEX.h' contains definitions of these constants.
 *
 *	The facet normal, if supplied, determines whether the triangle is
 *	back-facing.  Depending on the current reflection model and
 *	interior style attributes, the facet normal can also be used in
 *	shading computations.  The facet colour, if provided,  is used instead
 *	of the current (front-face or back-face) surface colour to provide
 *	the intrinsic colour of the triangle.
 *
 *	The `vertexAttributes' mask is used to indicate the data present
 *	in the `points' array.  The number of data records in this list is
 *	indicated by `numPoints'.  The (x,y,z) coordinate is always assumed
 *	to be provided as part of each vertex data record.  In addition,
 *	applications can use the constants `PEXGAColour' and `PEXGANormal' to
 *	construct a mask indicating whether colour and normal information
 *	is present as part of the vertex information.
 *	If present, vertex normals and colours can be used in the
 *	shading computations.
 *
 *	Colours in the facet and vertex lists must be in the current colour
 *	type.  (See `PEXSetColourType".)
 *
 *	All attributes affecting the representation of fill areas also affect
 *	the representation of the triangle strip primitive.
 *
 *	For more information about the format of facet and vertex data, see
 *	the introduction to this chapter.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer",  `PEXSetInteriorStyle", `PEXSetInteriorStyleIndex",
 *	`PEXSetSurfaceColourIndex", `PEXSetSurfaceColour",  `PEXSetReflectionAttributes", 
 *	`PEXSetReflectionModel",  `PEXSetSurfaceInterpMethod", `PEXSetBFInteriorStyle",
 *	`PEXSetBFInteriorStyleIndex", `PEXSetBFSurfaceColourIndex",  `PEXSetBFSurfaceColour", 
 *	`PEXSetBFReflectionAttributes",  `PEXSetBFReflectionModel", `PEXSetBFSurfaceInterpMethod",
 *	`PEXSetFacetCullingMode", `PEXSetFacetDistinguishFlag",  `PEXSetNormalFlipFlag", 
 *	`PEXSetPatternSize",  `PEXSetPatternRefPt", `PEXSetPatternAttributes",
 *	`PEXSetInteriorBundleIndex", `PEXSetSurfaceEdgeFlag",  `PEXSetSurfaceEdgeType", 
 *	`PEXSetSurfaceEdgeWidth",  `PEXSetSurfaceEdgeColour", `PEXSetSurfaceEdgeColourIndex",
 *	`PEXSetEdgeBundleIndex"
 *
 */

void
PEXTriangleStrip (ocbuf, facetAttributes, vertexAttributes, facetData,
	points, numPoints)

INPUT pxlOCBuf		*ocbuf;
INPUT unsigned long	facetAttributes;
INPUT unsigned long	vertexAttributes;
INPUT char		*facetData;
INPUT char		*points;
INPUT int		numPoints;

{
/* AUTHOR: */
    int			lenofColour;
    int			lenofFacetDataList;
    int			lenofVertexList;
    pexTriangleStrip	*pReq;

    /* 
     * Calculate number of words in the list of facet data and the vertex list
     */
    lenofColour = GetColourLength( pexColourTypeHidden);
    lenofFacetDataList = (numPoints - 2) *
		GetFacetDataLength( facetAttributes, lenofColour); 
    lenofVertexList = numPoints *
		GetVertexWithDataLength( vertexAttributes, lenofColour);

    PEXInitOC( ocbuf, PEXOCTriangleStrip, LENOF( pexTriangleStrip),
	    	    lenofFacetDataList + lenofVertexList, &pReq);
    if (pReq == NULL) return;

    /* 
     * Store the triangle strip request header data in the oc buffer 
     */
    pReq->colourType = pexColourTypeHidden;
    pReq->facetAttribs = facetAttributes;
    pReq->vertexAttribs = vertexAttributes;
    pReq->numVertices = numPoints;

    /*
     * Now copy the facet data and vertices to the oc
     */
    PEXCopyWordsToOC( ocbuf, lenofFacetDataList, facetData);
    PEXCopyWordsToOC( ocbuf, lenofVertexList, points);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXQuadMesh
 *
 * FORMAT:
 * 	`PEXQuadMesh`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	shape		The shape of each quadrilateral in the mesh.
 *
 *	facetAttributes
 *			A mask indicating the attributes provided
 *			for each quadrilateral.
 *
 *	vertexAttributes
 *			A mask indicating the attributes provided
 *			for each vertex.
 *
 *	facetData	The list of facet data for the quadrilateral mesh.
 *
 *	points		A two-dimensional (`m' x `n') list of vertices that
 *			define the quadrilateral mesh.
 *
 *	numPointsM	The number of vertices in the quadrilateral mesh in the
 *			`m' direction.
 *
 *	numPointsN	The number of vertices in the quadrilateral mesh in the
 *			`n' direction.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *  	This routine, when processed by a renderer, causes a quadrilateral
 *	mesh primitive to be rendered.
 *	The quadrilateral mesh surface is created from the `points' array.
 *	The (ith,jth), (i+1th, jth), (i+1th, j+1th), and (ith, j+1th)
 *	vertices are connected to create a single facet.
 *	If facet normals are not provided, they are computed by taking
 *	the cross product of a vector computed by subtracting vertex(i,j)
 *	from vertex(i+1,j+1) with the vector computed by subtracting
 *	vertex(i+1,j) from vertex(i,j+1).
 *
 *	The `shape' parameter indicates the shape of each of the quadrilaterals
 *	in the mesh.  This parameter is used in the same way as for
 *	`PEXFillArea".
 *
 *	The `facetAttributes' mask is used to indicate the contents of
 *	the data in the `facetData' list.  There must be one data
 *	record in the `facetData' list for each quadrilateral in the mesh.
 *	(If there are `m' x `n' points, there are `m' - 1 by `n' - 1
 *	quadrilaterals.)
 *	For quadrilateral mesh primitives, this data can be a colour, a normal,
 *	or a colour followed by a normal.  Applications can use the constants
 *	`PEXGAColour' and `PEXGANormal' to construct a mask indicating the data
 *	provided for each quadrilateral.
 * 	(Include file `PEX.h' contains definitions of these constants.)
 *
 *	If supplied, the facet normal is used to
 *	determine whether the quadrilateral is back-facing.  Depending
 *	on the current reflection model and interior style attributes, the
 *	facet normal can also be used in shading computations.  The
 *	facet colour, if provided, is used instead of the current (front-face
 *	or back-face) surface colour to provide the intrinsic colour.
 *
 *	The `vertexAttributes' mask is used to indicate the data present
 *	in the `points' array.  The number of data records in this list is
 *	indicated by `numPointsM' x `numPointsN'.  The (x,y,z) coordinate
 *	is always assumed to be provided as part of each vertex data record.
 *	In addition, applications can use the constants `PEXGAColour' and
 *	`PEXGANormal' to construct a mask indicating whether colour and normal
 *	information is present as part of the vertex information.
 *	If present, vertex normals and colours can be used in the
 *	shading computations.
 *
 *	Colours in the facet and vertex lists must be in the current colour
 *	type.  (See `PEXSetColourType".)
 *
 *	All attributes affecting the representation of fill areas also affect
 *	the representation of the quadrilateral mesh primitive.
 *
 *	For more information about the format of facet and vertex data, see
 *	the introduction to this chapter.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer",  `PEXSetInteriorStyle", `PEXSetInteriorStyleIndex",
 *	`PEXSetSurfaceColourIndex", `PEXSetSurfaceColour",  `PEXSetReflectionAttributes", 
 *	`PEXSetReflectionModel",  `PEXSetSurfaceInterpMethod", `PEXSetBFInteriorStyle",
 *	`PEXSetBFInteriorStyleIndex", `PEXSetBFSurfaceColourIndex",  `PEXSetBFSurfaceColour", 
 *	`PEXSetBFReflectionAttributes",  `PEXSetBFReflectionModel", `PEXSetBFSurfaceInterpMethod",
 *	`PEXSetFacetCullingMode", `PEXSetFacetDistinguishFlag",  `PEXSetNormalFlipFlag", 
 *	`PEXSetPatternSize",  `PEXSetPatternRefPt", `PEXSetPatternAttributes",
 *	`PEXSetInteriorBundleIndex", `PEXSetSurfaceEdgeFlag",  `PEXSetSurfaceEdgeType", 
 *	`PEXSetSurfaceEdgeWidth",  `PEXSetSurfaceEdgeColour", `PEXSetSurfaceEdgeColourIndex",
 *	`PEXSetEdgeBundleIndex"
 *
 */

void
PEXQuadMesh (ocbuf, shape, facetAttributes, vertexAttributes,
	facetData, points, numPointsM, numPointsN)

INPUT pxlOCBuf		*ocbuf;
INPUT int		shape;
INPUT unsigned long	facetAttributes;
INPUT unsigned long	vertexAttributes;
INPUT char		*facetData;
INPUT char		*points;
INPUT int		numPointsM;
INPUT int		numPointsN;
{
/* AUTHOR: */
    int			lenofColour;
    int			lenofFacetDataList;
    int			lenofVertexList;
    pexQuadrilateralMesh *pReq;

    /* 
     * Calculate the number of words in the facet data list and the vertex list
     */
    lenofColour = GetColourLength( pexColourTypeHidden);
    lenofFacetDataList = ((numPointsM - 1) * (numPointsN - 1)) *
		GetFacetDataLength( facetAttributes, lenofColour); 
    lenofVertexList = numPointsM * numPointsN *
		GetVertexWithDataLength( vertexAttributes, lenofColour);

    PEXInitOC( ocbuf, PEXOCQuadrilateralMesh, LENOF( pexQuadrilateralMesh),
		lenofFacetDataList + lenofVertexList, &pReq);
    if (pReq == NULL) return;

    /* 
     * Store the quad mesh request header data in the oc buffer 
     */
    pReq->colourType = pexColourTypeHidden;
    pReq->mPts = numPointsM;
    pReq->nPts = numPointsN;
    pReq->facetAttribs = facetAttributes;
    pReq->vertexAttribs = vertexAttributes;
    pReq->shape = shape;

    /*
     * Now copy the facet data and vertices to the oc
     */
    PEXCopyWordsToOC( ocbuf, lenofFacetDataList, facetData);
    PEXCopyWordsToOC( ocbuf, lenofVertexList, points);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXNurbSurface
 *
 * FORMAT:
 * 	`PEXNurbSurface`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	nurb		A structure defining the non-uniform B-spline
 *			surface.
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine, when processed by a render, causes a non-uniform B-spline surface to
 *	be rendered.  The surface is generated as a function of the parametric
 *	variables `u' and `v'.
 *
 *	Argument `nurb' is a structure of type `pxlNurbSurfaceData'.  Include file
 *	`PEXlib.h' contains a description of this structure.
 *	The `nurb' structure contains `type', `uorder', `vorder',
 *	an array of `points' which is `numMPoints' \ `numNPoints' long, 
 *	an array of `uknots' and an array of `vknots'.
 *	The length of `uknots' is `uorder' + `numMPoints'.  The length of
 *	`vknots' is `vorder' + `numNPoints'.   Vertices are stored in the
 *	array of `points' in row major order.
 *
 *	The `nurb' structure also has a pointer to an array of `pxlTrimLoopData'
 *	structures.  Each element in this array defines a single trim loop
 *	and consists of the number of trim curves in the loop and
 *	a pointer to an array of `pxlTrimCurveData' structures.
 *	(The `pxlTrimLoopData' and `pxlTrimCurveData' structures are also defined
 *	in `PEXlib.h'.)  Each element in the trim curve array contains `type',
 *	`visibility', `order', `approxMethod', `tolerance',
 *	`tmin', and `tmax' information.  It also contains the number of points
 *	and arrays of `knots' and `points' for each trim curve defining the
 *	trimming loops associated with the surface.
 *
 *	The `uorder' and `vorder' of the surface must be positive integers and
 *	indicate the order of the parametric variables.   The spline
 *	shape is specified using two lists of knots in the parametric coordinate
 *	space, plus an array of control points specified in
 *	modelling coordinates.  The `uknots' and `vknots'
 *	sequences must each form a non-decreasing sequence of numbers.
 *
 *	The minimum and maximum knot values in `uknots' define the range over 
 *	which the B-spline surface is evaluated in the u parametric direction, 
 *	and the minimum and maximum knot values in `vknots' define the range 
 *	over which the B-spline surface is evaluated in the v parametric 
 *	direction.
 *
 *	The `type' can be either `PEXRational' or `PEXNonRational'.
 *	Include file `PEX.h' contains definitions of these constants.  If
 *	`type' is `PEXRational', the point list must be specified in
 *	homogeneous (4D) modelling coordinates.  If `type' is `PEXNonRational',
 *	the point list must be specified in non-homogeneous (3D) modelling
 *	coordinates.
 *
 *	If a list of trimming loops is specified, they serve to further
 *	restrict the region in parametric coordinate space over which the
 *	B-spline surface is evaluated.  When no trimming loops are
 *	specified, the rectangular parameter limits of the surface are
 *	rendered as the edges of the surface based on the edge flag attribute.
 *
 *	Each trimming loop is defined as a list of one or more B-spline
 *	trimming curve segments connected head-to-tail.  The list
 *	must be explicitly closed, so that the tail of the last B-spline
 *	curve joins the head of the first B-spline curve segment in each
 *	trimming loop.
 *
 *	Each B-spline curve segment is parameterized independently.  If there
 *	is floating point inaccuracy in closure or in head-to-tail connectivity
 *	between B-spline curve segments, closure or connectivity is assumed.
 *
 *	B-spline curve segments for trimming loops are defined in the parameter
 *	space of the surface and can not go outside of that space.  Each
 *	separate B-spline curve segment must not intersect itself.
 *	B-spline curve segments in a trimming loop must not intersect
 *	themselves, except for joining at the endpoints.
 *
 *	The `type' of each trim curve defining the trim loops can be either
 *	`PEXRational' or `PEXNonRational'.  If
 *	`type' is `PEXRational', the point list must be specified in
 *	3D modelling coordinates.  If `type' is `PEXNonRational',
 *	the point list must be specified in 2D modelling
 *	coordinates.
 *
 *	Each B-spline curve segment for trimming loops has a `visibility' flag
 *	controlling whether it is visible as a surface edge.
 *	Depending on the settings of a renderer's surface edge attributes,
 *	interior style, and the `visibility' flags associated with trim curves,
 *	the B-spline curve segments in trim curves may be drawn as surface
 *	edges.
 *
 *	All attributes affecting the representation of fill areas
 *	affect the representation of the non-unform B-spline surface
 *	primitive.  In addition, the surface approximation 
 *	is  used to determine how to
 *	approximate the B-spline surface.  Depending on the settings of the
 *	`PEXSurfaceApproxAsf', this is obtained either
 *	directly from the current attribute values or from the interior
 *	bundle lookup table specified by the current interior bundle index
 *	attribute.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer",  `PEXSetInteriorStyle", `PEXSetInteriorStyleIndex",
 *	`PEXSetSurfaceColourIndex", `PEXSetSurfaceColour",  `PEXSetReflectionAttributes", 
 *	`PEXSetReflectionModel",  `PEXSetSurfaceInterpMethod", `PEXSetBFInteriorStyle",
 *	`PEXSetBFInteriorStyleIndex", `PEXSetBFSurfaceColourIndex",  `PEXSetBFSurfaceColour", 
 *	`PEXSetBFReflectionAttributes",  `PEXSetBFReflectionModel", `PEXSetBFSurfaceInterpMethod",
 *	`PEXSetSurfaceApproxMethod", `PEXSetFacetCullingMode", 
 *	`PEXSetFacetDistinguishFlag",  `PEXSetNormalFlipFlag", `PEXSetPatternSize",
 *	`PEXSetPatternRefPt", `PEXSetPatternAttributes",  `PEXSetInteriorBundleIndex", 
 *	`PEXSetSurfaceEdgeFlag",  `PEXSetSurfaceEdgeType", `PEXSetSurfaceEdgeWidth",
 *	`PEXSetSurfaceEdgeColour", `PEXSetSurfaceEdgeColourIndex",  `PEXSetEdgeBundleIndex"
 *
 */

void
PEXNurbSurface (ocbuf, nurb)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlNurbSurfaceData	*nurb;

{
/* AUTHOR: */
    int			lenofVertexList;
    int			lenofUKnotList;
    int			lenofVKnotList;
    int			lenofTrimCurveData;
    int			i;
    int			j;
    int			thisLength;
    pxlTrimLoopData	*ptrimLoop;
    pxlTrimCurveData	*ptrimCurve;
    pexNurbSurface	*pReq;
    pxlTrimCurve	*pTCHead;
    CARD32		*pData;

    /* 
     * calculate the number of words in the vertex list and the knot lists 
     */
    lenofVertexList = nurb->numMPoints * nurb->numNPoints *
		((nurb->type == PEXRational) ? LENOF( pxlCoord4D) : 
				    	    LENOF( pxlCoord3D) );

    lenofUKnotList = nurb->uorder + nurb->numMPoints; /* knots are FLOATs */
    lenofVKnotList = nurb->vorder + nurb->numNPoints;

    /* 
     * Calculate the number of words in the trim curve data.  Note that the
     * vertices for the trim curve are in parametric space so they are either
     * 3D or 2D .. the pex 4.0C documentation is incorrect 
     */
    lenofTrimCurveData = 0;
    for (i=0; i<nurb->numTrimLoops; i++)
    {
	ptrimLoop = &nurb->trimLoop[i];

        /* 
	 * for each trim loop there is a count and a list of curves 
         */
	lenofTrimCurveData++;  /* count is CARD32 */

	for (j=0; j<ptrimLoop->numTrimCurves; j++)
	{
           /* 
            * for each trim curve there is a trim curve header, list of 
	    * knots and list of vertices 
            */
	    ptrimCurve = &ptrimLoop->trimCurves[j];
	    lenofTrimCurveData += LENOF( pxlTrimCurve) + 
		ptrimCurve->numPoints + ptrimCurve->order + /* knot list */
		ptrimCurve->numPoints * 			
		    ((ptrimCurve->type == PEXRational) ? LENOF( pxlCoord3D) : 
					              LENOF( pxlCoord2D) );
	}
    }

    PEXInitOC( ocbuf, PEXOCNurbSurface, LENOF( pexNurbSurface), 
		lenofUKnotList + lenofVKnotList + lenofVertexList + 
			lenofTrimCurveData, 
		&pReq);
    if (pReq == NULL) return;

    /*
     * Store the nurb request header data in the oc buffer
     */
    pReq->type = nurb->type;
    pReq->uOrder = nurb->uorder;
    pReq->vOrder = nurb->vorder;
    pReq->numUknots = nurb->uorder + nurb->numMPoints;
    pReq->numVknots = nurb->vorder + nurb->numNPoints;
    pReq->mPts = nurb->numMPoints;
    pReq->nPts = nurb->numNPoints;
    pReq->numLists = nurb->numTrimLoops;

    /*
     * Now copy the knot lists and the vertex list
     */
    PEXCopyWordsToOC( ocbuf, lenofUKnotList, nurb->uknots);
    PEXCopyWordsToOC( ocbuf, lenofVKnotList, nurb->vknots);
    PEXCopyWordsToOC( ocbuf, lenofVertexList, nurb->points);

    /* 
     * Now add the trim loop data (LISTofLISTofTrimCurve).  A trim loop
     * consists of a count followed by a list of trim curves. 
     */
    for (i=0; i<nurb->numTrimLoops; i++)
    {
	ptrimLoop = &nurb->trimLoop[i];

	if ( (pData = (CARD32 *) PEXGetOCWords(ocbuf, 1)) != NULL )
	    *pData = ptrimLoop->numTrimCurves;

	for (j=0; j<ptrimLoop->numTrimCurves; j++)
	{
	    ptrimCurve = &ptrimLoop->trimCurves[j];

	    /* add the trim curve header data */
	    if ( (pTCHead = (pxlTrimCurve *) 
			PEXGetOCWords(ocbuf, LENOF( pexTrimCurve))) != NULL) 
	    {
	        pTCHead->visibility = (pexSwitch) ptrimCurve->visibility;
	        pTCHead->order = (CARD16) ptrimCurve->order;
	        pTCHead->type = (pexCoordType)ptrimCurve->type;
	        pTCHead->approxMethod = (INT16)ptrimCurve->approxMethod;
	        pTCHead->tolerance = (FLOAT)ptrimCurve->tolerance;
	        pTCHead->tMin = (FLOAT)ptrimCurve->tmin;
	        pTCHead->tMax = (FLOAT)ptrimCurve->tmax;
	        pTCHead->numKnots = ptrimCurve->order + ptrimCurve->numPoints;
	        pTCHead->numCoord = ptrimCurve->numPoints;
	    }

	    /* add the trim curve knot list .. knots are FLOATs */
	    thisLength = ptrimCurve->order + ptrimCurve->numPoints;
    	    PEXCopyWordsToOC( ocbuf, thisLength, ptrimCurve->knots);

	    /* add the trim curve vertex list */
	    thisLength = ptrimCurve->numPoints *
		((ptrimCurve->type == PEXRational) ? LENOF( pxlCoord3D) : 
						  LENOF( pxlCoord2D));
    	    PEXCopyWordsToOC( ocbuf, thisLength, ptrimCurve->points);
	}
    }

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXCellArray
 *
 * FORMAT:
 * 	`PEXCellArray`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	pt1		The first cell array definition point.
 *
 *	pt2		The second cell array definition point.
 *
 *	pt3		The third cell array definition point.
 *
 *	dx		The number of cells in the X direction.
 *
 *	dy		The number of cells in the Y direction.
 *
 *	icolours		An array of colour index values which specify the colours
 *			of the cells in the cell array.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *  	This routine, when processed by a renderer, causes a 3D cell-array
 *	primitive to be rendered.
 *	A 3D cell-array primitive is a parallelogram of equally-sized
 *	cells, each of which is a parallelogram with a single colour.  Each
 *	cell has a width defined by the distance between `pt1' and `pt2'
 *	divided by `dx'.  Each cell has a height defined by the distance
 *	between `pt1' and `pt3' divided by `dy'.
 *
 *	Cell colours are specified in a one-dimensional array
 *	of size `dx' x `dy'.  The colour of each cell is specified by the
 *	colour index of the corresponding element in the `icolours' array.
 *	Colours are stored in the array by rows, that is, the column number
 *	of the array varies most rapidly.
 *	The first colour in the array is the colour at the cell at the
 *	corner of `pt1', and subsequent colours represent the colours
 *	of cells proceeding to `pt2'.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer"
 *
 */

void
PEXCellArray (ocbuf, pt1, pt2, pt3, dx, dy, icolours)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord3D	*pt1;
INPUT pxlCoord3D	*pt2;
INPUT pxlCoord3D	*pt3;
INPUT int		dx;
INPUT int		dy;
INPUT pxlIndexedColour 	*icolours;

{
/* AUTHOR: */
    pexCellArray	*pReq;
    pxlTableIndex	*pData;
    int			i, n;

    PEXInitOC( ocbuf, PEXOCCellArray, LENOF( pexCellArray), 
		NUMWORDS(dx * dy * sizeof(pxlTableIndex)), &pReq);
    if (pReq == NULL) return;

    /*
     * Store the cell array header data in the oc buffer
     */
    pReq->point1 = *pt1;
    pReq->point2 = *pt2;
    pReq->point3 = *pt3;
    pReq->dx = dx;
    pReq->dy = dy;

    /* repack colour indices */
    n = dx*dy;
    for (i = 0; i < n; )
    {
	if ( (pData = (pxlTableIndex *) PEXGetOCWords(ocbuf, 1)) != NULL)
	{
    	    pData[0] = icolours[i++].index;
	    if (i < n) pData[1] = icolours[i++].index;
	}
	else
	    break;
    }

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXCellArray2D
 *
 * FORMAT:
 * 	`PEXCellArray2D`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	pt1		The first cell array definition point.
 *
 *	pt2		The second cell array definition point.
 *
 *	dx		The number of cells in the X direction.
 *
 *	dy		The number of cells in the Y direction.
 *
 *	icolours		An array of colour index values which specify the colours
 *			of the cells in the cell array.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine, when processed by a renderer, causes a two-dimensional
 *	cell array primitive to be rendered.
 *	A two-dimensional cell-array primitive is a rectangle of equally-sized
 *	cells, each of which is a rectangle which has a single colour.
 *	The primitive is specified by two points which define a rectangle
 *	aligned with the modelling coordinate axes.
 *	The width of each cell is defined by the difference between the
 *	X coordinate of `pt1' and the X coordinate of `pt2' divided by `dx'.
 *	The height of each cell is defined by the difference between the
 *	Y coordinate of `pt1' and the Y coordinate of `pt2' divided by `dy'.
 *
 *	The colours for the cells are specified in a one-dimensional array
 *	of size `dx' x `dy'.  The colour of each cell is specified by the
 *	colour index of the corresponding element in the `icolours' array.
 *	Colours are stored in the array by rows, that is, the column number
 *	of the array varies most rapidly.
 *	The first colour in the array is the colour at the cell at the
 *	corner of `pt1', and subsequent colours represent the colours
 *	of cells proceeding in the X direction.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer"
 *
 */

void
PEXCellArray2D (ocbuf, pt1, pt2, dx, dy, icolours)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord2D	*pt1;
INPUT pxlCoord2D	*pt2;
INPUT int		dx;
INPUT int		dy;
INPUT pxlIndexedColour	*icolours;

{
/* AUTHOR: */
    pexCellArray2D	*pReq;
    pxlTableIndex	*pData;
    int	 		i, n;


    PEXInitOC( ocbuf, PEXOCCellArray2D, LENOF( pexCellArray2D), 
		NUMWORDS(dx * dy * sizeof(pxlTableIndex)), &pReq);
    if (pReq == NULL) return;

    /*
     * Store the cell array header data in the oc buffer
     */
    pReq->point1 = *pt1;
    pReq->point2 = *pt2;
    pReq->dx = dx;
    pReq->dy = dy;

    /* repack colour indices */
    n = dx*dy;
    for (i = 0; i < n; )
    {
	if ( (pData = (pxlTableIndex *) PEXGetOCWords(ocbuf, 1)) != NULL)
	{
    	    pData[0] = icolours[i++].index;
	    if (i < n) pData[1] = icolours[i++].index;
	}
	else
	    break;
    }

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXExtendedCellArray
 *
 * FORMAT:
 * 	`PEXExtendedCellArray`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	pt1		The first cell array definition point.
 *
 *	pt2		The second cell array definition point.
 *
 *	pt3		The third cell array definition point.
 *
 *	dx		The number of cells in the X direction.
 *
 *	dy		The number of cells in the Y direction.
 *
 *  	colours		An array of colours specifying the colours of the
 *			cells in the cell array.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine, when processed by a renderer, has the same effect as a
 *	3D cell array primitive, except the colours can be passed as either
 *	`Indirect' colour indices or direct colour values, depending on the
 *	current colour type.  (See `PEXSetColourType".)
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer"
 *
 */

void
PEXExtendedCellArray (ocbuf, pt1, pt2, pt3, dx, dy, colours)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord3D	*pt1;
INPUT pxlCoord3D	*pt2;
INPUT pxlCoord3D	*pt3;
INPUT int		dx;
INPUT int		dy;
INPUT char	 	*colours;

{
/* AUTHOR: */
    pexExtCellArray 	*pReq;
    int			lenofColourList;


    lenofColourList = dx * dy * GetColourLength( pexColourTypeHidden);
    PEXInitOC( ocbuf, PEXOCExtCellArray, LENOF( pexExtCellArray), 
		lenofColourList, &pReq);
    if (pReq == NULL) return;

    /*
     * Store the cell array header data in the oc buffer
     */
    pReq->colourType = pexColourTypeHidden;
    pReq->point1 = *pt1;
    pReq->point2 = *pt2;
    pReq->point3 = *pt3;
    pReq->dx = dx;
    pReq->dy = dy;

    PEXCopyWordsToOC( ocbuf, lenofColourList, colours);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXGdp
 *
 * FORMAT:
 * 	`PEXGdp`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	id		The identifier of the 3-Dimensional Generalized
 *			Drawing Primitive.
 *
 *	points		The coordinate points used by the GDP3.
 *
 *	numPoints	The number of coordinate points.
 *
 *	data		Other data used by the GDP3.
 *
 *	numBytes	The size, in bytes of the other data.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine, when processed by a renderer,  is
 *	implementation-dependent.  Because of floating point and colour format
 *	discrepancies across a network interface, GDPs are expected to
 *	be of limited usefulness, but they are provided for compatibility
 *	with PHIGS.  If the specified GDP identifier is not supported,
 *	then the output command is ignored.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer"
 *
 */

void
PEXGdp (ocbuf, id, points, numPoints, data, numBytes)

INPUT pxlOCBuf		*ocbuf;
INPUT int		id;
INPUT pxlCoord3D	*points;
INPUT int		numPoints;
INPUT char		*data;
INPUT int		numBytes;

{
/* AUTHOR: */
    pexGdp	*pReq;
    int		lenofVertexList;

    lenofVertexList = numPoints * LENOF( pxlCoord3D);
    PEXInitOC( ocbuf, PEXOCGdp, LENOF( pexGdp), 
		lenofVertexList + NUMWORDS( numBytes), &pReq);
    if (pReq == NULL) return;

    /*
     * Store the gdp header data in the oc buffer
     */
    pReq->gdpId = id; 
    pReq->numPoints = numPoints; 
    pReq->numBytes = numBytes;

    PEXCopyWordsToOC( ocbuf, lenofVertexList, points);
    PEXCopyBytesToOC( ocbuf, numBytes, data);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXGdp2D
 *
 * FORMAT:
 * 	`PEXGdp2D`
 *
 * ARGUMENTS:
 *	ocbuf		The output command buffer to which the output
 *			primitive command is added.
 *
 *	id		The identifier of the Two-Dimensional Generalized
 *			Drawing Primitive.
 *
 *	points		The coordinate points used by the GDP2.
 *
 *	numPoints	The number of coordinate points.
 *
 *	data		Other data used by the GDP2.
 *
 *	numBytes     	The size, in bytes, of the other data.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *  	This routine, when processed by a renderer, is
 *	implementation-dependent.  Because of floating point and colour
 *	format discrepancies across a network interface,
 *	GDPs are expected to be of limited usefulness, but they are provided
 *	for compatibility with PHIGS.  If the specified GDP identifier is 
 *	not supported, then the output command is ignored.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer"
 *
 */

void
PEXGdp2D (ocbuf, id, points, numPoints, data, numBytes)

INPUT pxlOCBuf		*ocbuf;
INPUT int		id;
INPUT pxlCoord2D	*points;
INPUT int		numPoints;
INPUT char		*data;
INPUT int		numBytes;

{
/* AUTHOR: */
    pexGdp2D	*pReq;
    int		lenofVertexList;

    lenofVertexList = numPoints * LENOF( pxlCoord2D);
    PEXInitOC( ocbuf, PEXOCGdp2D, LENOF( pexGdp2D), 
		lenofVertexList + NUMWORDS( numBytes), &pReq);
    if (pReq == NULL) return;

    /*
     * Store the gdp header data in the oc buffer
     */
    pReq->gdpId = id; 
    pReq->numPoints = numPoints; 
    pReq->numBytes = numBytes;

    PEXCopyWordsToOC( ocbuf, lenofVertexList, points);
    PEXCopyBytesToOC( ocbuf, numBytes, data);

    PEXFinishOC( ocbuf);

    return;
}
