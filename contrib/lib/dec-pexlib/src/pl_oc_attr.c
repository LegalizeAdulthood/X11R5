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
**	pl_oc_attr.c
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
**	This file contains the routines associated with the PEX attribute output
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
 * E   PEXSetMarkerType
 * E   PEXSetMarkerScale
 * E   PEXSetMarkerColourIndex
 * E   PEXSetMarkerColour
 * E   PEXSetMarkerBundleIndex
 * E   PEXSetTextFontIndex
 * E   PEXSetTextPrecision
 * E   PEXSetCharExpansion
 * E   PEXSetCharSpacing
 * E   PEXSetTextColourIndex
 * E   PEXSetTextColour
 * E   PEXSetCharHeight
 * E   PEXSetCharUpVector
 * E   PEXSetTextPath
 * E   PEXSetTextAlignment
 * E   PEXSetATextHeight
 * E   PEXSetATextUpVector
 * E   PEXSetATextPath
 * E   PEXSetATextAlignment
 * E   PEXSetATextStyle
 * E   PEXSetTextBundleIndex
 * E   PEXSetLineType
 * E   PEXSetLineWidth
 * E   PEXSetLineColourIndex
 * E   PEXSetLineColour
 * E   PEXSetCurveApproxMethod
 * E   PEXSetPolylineInterpMethod
 * E   PEXSetLineBundleIndex
 * E   PEXSetInteriorStyle
 * E   PEXSetInteriorStyleIndex
 * E   PEXSetSurfaceColourIndex
 * E   PEXSetSurfaceColour
 * E   PEXSetReflectionAttributes
 * E   PEXSetReflectionModel
 * E   PEXSetSurfaceInterpMethod
 * E   PEXSetBFInteriorStyle
 * E   PEXSetBFInteriorStyleIndex
 * E   PEXSetBFSurfaceColourIndex
 * E   PEXSetBFSurfaceColour
 * E   PEXSetBFReflectionAttributes
 * E   PEXSetBFReflectionModel
 * E   PEXSetBFSurfaceInterpMethod
 * E   PEXSetSurfaceApproxMethod
 * E   PEXSetTrimCurveApproxMethod
 * E   PEXSetFacetCullingMode
 * E   PEXSetFacetDistinguishFlag
 * E   PEXSetPatternSize
 * E   PEXSetPatternRefPt
 * E   PEXSetPatternAttributes
 * E   PEXSetInteriorBundleIndex
 * E   PEXSetSurfaceEdgeFlag
 * E   PEXSetSurfaceEdgeType
 * E   PEXSetSurfaceEdgeWidth
 * E   PEXSetSurfaceEdgeColourIndex
 * E   PEXSetSurfaceEdgeColour
 * E   PEXSetEdgeBundleIndex
 * E   PEXSetIndividualASF
 * E   PEXSetLocalTransform
 * E   PEXSetLocalTransform2D
 * E   PEXSetGlobalTransform
 * E   PEXSetGlobalTransform2D
 * E   PEXSetModelClipFlag
 * E   PEXSetModelClipVolume
 * E   PEXSetModelClipVolume2D
 * E   PEXRestoreModelClipVolume
 * E   PEXSetViewIndex
 * E   PEXSetLightSourceState
 * E   PEXSetDepthCueIndex
 * E   PEXSetPickID
 * E   PEXSetHlhsrID
 * E   PEXSetColourApproxIndex
 * E   PEXSetRenderingColourModel
 * E   PEXSetParaSurfCharacteristics
 * E   PEXAddToNameSet
 * E   PEXRemoveFromNameSet
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
#include "pl_ref_data.h"

/*+
 * NAME:
 * 	PEXSetMarkerType
 *
 * FORMAT:
 * 	`PEXSetMarkerType`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	type		The marker type used when drawing
 *			marker primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the marker type used when drawing marker primitives.
 *	This marker type is used when the `PEXMarkerTypeAsf' is `PEXIndividual'.
 *	`Type' should be one of the marker types supported by the PEX
 *	server extension.  (See `PEXGetEnumTypeInfo".)  Registered values
 *	for `type' are `PEXMarkerDot', `PEXMarkerCross', `PEXMarkerAsterisk',
 *	`PEXMarkerCircle', and `PEXMarkerX' (all defined in the `PEX.h' include file).
 *	`PEXMarkerAsterisk' is the default type.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	the `PEXoc.h' include file:
 *
 *	==
 *	PEX_SetMarkerType (ocbuf, type)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `type' is
 *	a pointer to a value of type `pxlEnumTypeIndex' (defined in the
 *	`PEXprotost.h' include file) containing the marker type
 *	used when drawing marker primitives.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *                              point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXMarkers", `PEXMarkers2D",
 *	`PEXSetIndividualASF"
 *
 */

void
PEXSetMarkerType (ocbuf, type)

INPUT pxlOCBuf		*ocbuf;
INPUT int		type;

{
/* AUTHOR: */
    pexEnumTypeIndex	t;

    /* convert from a long to a short to avoid problems on non-VAX machines */
    t = type;
    PEXAddOC (ocbuf, PEXOCMarkerType, sizeof( pexEnumTypeIndex), &t);

    return;
}

/*+
 * NAME:
 * 	PEXSetMarkerScale
 *
 * FORMAT:
 * 	`PEXSetMarkerScale`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	scale		The marker scale used when drawing
 *			marker primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets the marker
 *	size scale factor used when drawing marker primitives.
 *	This scale factor is used when the `MarkerSizeAsf' is `PEXIndividual'.
 *	`Scale' is multiplied by the nominal marker size for the display
 *	device (see `PEXGetImpDepConstants") to produce a marker size
 *	mapped to the workstation's nearest available marker size.  The
 *  	default scale value is 1.0.  If the marker type `PEXMarkerDot' is
 *	used, the scale factor is ignored, and the marker is always displayed
 *	as the smallest displayable dot.
 *
 *  	C programmers can elect to use the equivalent C macro defined in
 *	the `PEXoc.h' include file:
 *
 *	==
 *	PEX_SetMarkerScale (ocbuf, scale)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `scale'
 *	is a pointer to a floating point value representing the scale
 *	factor used when drawing marker primitives.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXMarkers", `PEXMarkers2D",
 *	`PEXSetIndividualASF"
 *
 */

void
PEXSetMarkerScale (ocbuf, scale)

INPUT pxlOCBuf		*ocbuf;
INPUT float		scale;

{
/* AUTHOR: */
    float		s;

    /* convert from a double to a float to avoid problems on non-VAX machines */
    s = scale;
    PEXAddOC (ocbuf, PEXOCMarkerScale, sizeof( FLOAT), &s);

    return;
}

/*+
 * NAME:
 * 	PEXSetMarkerColourIndex
 *
 * FORMAT:
 * 	`PEXSetMarkerColourIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		The indexed colour used as the marker
 *			colour.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the colour used to draw marker primitives.
 *	This colour is used if the `PEXMarkerColourAsf' is `PEXIndividual'.
 *	This output command sets the marker colour attribute to an
 *	indexed colour with the value indicated by `index'.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	the `PEXoc.h' include file:
 *
 *	==
 *	PEX_SetMarkerColourIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlTableIndex' (defined in the
 *	`PEXprotost.h' include file) containing the indexed
 *	colour used as the marker colour.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXMarkers", `PEXMarkers2D",
 *	`PEXSetIndividualASF", `PEXCreateLookupTable"
 *
 */

void
PEXSetMarkerColourIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCMarkerColourIndex, sizeof( pexTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetMarkerColour
 *
 * FORMAT:
 * 	`PEXSetMarkerColour`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	colour		A pointer to the colour used when drawing
 *			marker primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the colour used to draw marker primitives.
 *	This colour is used when the `PEXMarkerColourAsf' is `PEXIndividual'.
 *	This output command sets the marker colour attribute to
 *	either an indexed colour or a direct colour value, depending
 *	on the current value of the PEXlib colour type attribute.  The
 *	`pxlColour' data type is defined as a union of `pxlIndexedColour'
 *	and the data structures defining the direct colour formats.
 *	The colour must be in the current colour type.  (See `PEXSetColourType".)
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	the `PEXoc.h' include file:
 *
 *	==
 *	PEX_SetMarkerColour (ocbuf, colour)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer.  `colour'
 *	is a pointer to a structure of type `pxlColour' 
 *	containing the colour used as the
 *	marker colour.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXMarkers", `PEXMarkers2D",
 *	`PEXSetIndividualASF", `PEXCreateLookupTable"
 *
 */

void
PEXSetMarkerColour (ocbuf, colour)

INPUT pxlOCBuf	*ocbuf;
INPUT pxlColour	*colour;

{
/* AUTHOR: */
    pxlColourSpecifier	pcs;

    InitializeColourSpecifier (pcs, colour);

    PEXAddOC (ocbuf, PEXOCMarkerColour,
	sizeof( pxlColourSpecifier) - AdjustSizeFromType( pexColourTypeHidden), 
	&pcs);

    return;
}

/*+
 * NAME:
 * 	PEXSetMarkerBundleIndex
 *
 * FORMAT:
 * 	`PEXSetMarkerBundleIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		An index indicating the entry in the marker bundle
 *			table used to obtain marker attributes.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the index value used to obtain marker attributes.
 *	Marker primitives rendered after this command
 *	use marker attributes obtained from the marker bundle lookup
 *	table entry referenced by `index' for marker attributes whose attribute
 *	source flags are `PEXBundled'.  The default value of `index'
 *	is one.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	the `PEXoc.h' include file:
 *
 *	==
 *	PEX_SetMarkerBundleIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlTableIndex' (defined in the
 *	'PEXprotostr' include file) indicating the
 *	marker bundle table entry from which marker bundle attributes can
 *	be obtained.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer",  `PEXMarkers", `PEXMarkers2D",
 *	`PEXSetIndividualASF", `PEXCreateLookupTable"
 *
 */

void
PEXSetMarkerBundleIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCMarkerBundleIndex, sizeof( pexTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetTextFontIndex
 *
 * FORMAT:
 * 	`PEXSetTextFontIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		The text font index used when drawing
 *			text and annotation text primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the index value used to obtain the resource identifier of a
 *	PEX font or an X font.
 *	This text font index is used when the current `PEXTextFontIndexAsf'
 *	is `PEXIndividual'.
 *	`Index' specifies the entry in the current
 *	text font lookup table from which the font resource identifier is
 *	obtained.  Text and annotation text primitives rendered after
 *	this command are drawn using this font.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in the `PEXoc.h' include file:
 *
 *	==
 *	PEX_SetTextFontIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlTableIndex' (defined in the
 *	`PEXprotost.h' include file) indicating the
 *	text font table entry from which the font resource identifier is to
 *	be obtained.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXText",
 * 	`PEXText2D", `PEXAnnotationText", `PEXAnnotationText2D",
 *	`PEXCreateLookupTable"
 *
 */

void
PEXSetTextFontIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCTextFontIndex, sizeof( pexTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetTextPrecision
 *
 * FORMAT:
 * 	`PEXSetTextPrecision`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	precision	The text precision used when drawing
 *			text and annotation text primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets the text
 *	precision used when drawing text and annotation text primitives.
 *	This text precision is used when the `PEXTextPrecAsf' is `PEXIndividual'.
 *	Possible values are `PEXStringPrecision', `PEXCharPrecision', and
 *	`PEXStrokePrecision'.  The `PEX.h' include file contains definitions for
 *	these constants.  The default is `PEXStringPrecision'.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in the `PEXoc.h' include file:
 *
 *	==
 *	PEX_SetTextPrecision (ocbuf, precision)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `precision'
 *	is a pointer to a value of type `CARD16' (defined in the
 *	`Xmd.h' include file) indicating the text precision used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXText",
 *	`PEXText2D", `PEXAnnotationText", `PEXAnnotationText2D"
 *
 */

void
PEXSetTextPrecision (ocbuf, precision)

INPUT pxlOCBuf		*ocbuf;
INPUT int		precision;

{
/* AUTHOR: */
    CARD16	p;

    p = precision;
    PEXAddOC (ocbuf, PEXOCTextPrecision, sizeof( CARD16), &p);

    return;
}

/*+
 * NAME:
 * 	PEXSetCharExpansion
 *
 * FORMAT:
 * 	`PEXSetCharExpansion`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	expansion	The character expansion factor used
 *			when drawing text and annotation text primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the character expansion factor used
 *	when drawing text and annotation text primitives.
 *	This expansion factor is used when the `PEXCharExpansionAsf'
 *	is `PEXIndividual'.
 *	The character expansion factor is the deviation of the width to
 *	height ratio of the character glyphs from the ratio indicated by the
 *	font designer.  The default value is 1.0, which causes no
 *	deviation from the font definition.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in the `PEXoc.h' include file:
 *
 *	==
 *	PEX_SetCharExpansion (ocbuf, expansion)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `expansion'
 *	is a pointer to a floating point value used as the
 *	character expansion attribute.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF",`PEXText",
 *	`PEXText2D", `PEXAnnotationText", `PEXAnnotationText2D"
 *
 */

void
PEXSetCharExpansion (ocbuf, expansion)

INPUT pxlOCBuf		*ocbuf;
INPUT float		expansion;

{
/* AUTHOR: */
    float		e;

    e = expansion;
    PEXAddOC (ocbuf, PEXOCCharExpansion, sizeof( FLOAT), &e);

    return;
}

/*+
 * NAME:
 * 	PEXSetCharSpacing
 *
 * FORMAT:
 * 	`PEXSetCharSpacing`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	spacing		A floating point value specifying the
 *			character spacing factor used when drawing
 *			text and annotation text primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the spacing factor `spacing' used when drawing text
 *	and annotation text primitives.
 *	This character spacing value is used when the `PEXCharSpacingAsf'
 *	is `PEXIndividual'.
 *	The character spacing attribute
 *	specifies how much additional space is inserted between two
 *	adjacent characters, and is expressed as a fraction of the
 *	font's nominal character height.  The default value is 0.0.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in the `PEXoc.h' include file:
 *
 *	==
 *	PEX_SetCharSpacing (ocbuf, spacing)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `spacing'
 *	is a pointer to a floating point value used as the
 *	character spacing attribute.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXText",
 *	`PEXText2D", `PEXAnnotationText", `PEXAnnotationText2D"
 *
 */

void
PEXSetCharSpacing (ocbuf, spacing)

INPUT pxlOCBuf		*ocbuf;
INPUT float		spacing;

{
/* AUTHOR: */
    float		s;

    s = spacing;
    PEXAddOC (ocbuf, PEXOCCharSpacing, sizeof( FLOAT), &s);

    return;
}

/*+
 * NAME:
 * 	PEXSetTextColourIndex
 *
 * FORMAT:
 * 	`PEXSetTextColourIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		The colour used when drawing
 *			text and annotation text primtives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the colour used to draw text and annotation text primitives.
 *	This colour is used when the `PEXTextColourAsf' is `PEXIndividual'.
 *	This output command sets the text colour attribute to an
 *	indexed colour with the value indicated by `index'.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in the `PEXoc.h' include file:
 *
 *	==
 *	PEX_SetTextColourIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlTableIndex' (defined in the
 *	`PEXprotost.h' include file) containing the indexed
 *	colour used as the text colour.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXText",
 *	`PEXText2D", `PEXAnnotationText", `PEXAnnotationText2D",
 *	`PEXCreateLookupTable"
 *
 */

void
PEXSetTextColourIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCTextColourIndex, sizeof( pexTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetTextColour
 *
 * FORMAT:
 * 	`PEXSetTextColour`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	colour		A pointer to the colour used when drawing
 *			text and annotation text primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the colour used to draw text and annotation text primitives.
 *	This colour is used when the `PEXTextColourAsf' is `PEXIndividual'.
 *	This output command sets the text colour attribute
 *	to either an indexed colour or a direct colour value, depending
 *	on the current value of the PEXlib colour type attribute.  The
 *	`pxlColour' data type is defined a union of `pxlIndexedColour'
 *	and the data structures defining the direct colour formats.
 *	The colour must be in the current colour type.  (See `PEXSetColourType".)
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in the `PEXoc.h' include file:
 *
 *	==
 *	PEX_SetTextColour (ocbuf, colour)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `colour'
 *	is a pointer to a structure of type `pxlColour' 
 *	containing the colour used as the
 *	text colour.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF",  `PEXText",
 *	`PEXText2D", `PEXAnnotationText", `PEXAnnotationText2D",
 *	`PEXCreateLookupTable"
 *
 */

void
PEXSetTextColour (ocbuf, colour)

INPUT pxlOCBuf	*ocbuf;
INPUT pxlColour	*colour;

{
/* AUTHOR: */
    pxlColourSpecifier	pcs;

    InitializeColourSpecifier (pcs, colour);

    PEXAddOC (ocbuf, PEXOCTextColour,
	sizeof( pxlColourSpecifier) - AdjustSizeFromType( pexColourTypeHidden), 
	&pcs);

    return;
}

/*+
 * NAME:
 * 	PEXSetCharHeight
 *
 * FORMAT:
 * 	`PEXSetCharHeight`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	height		The character height value used
 *			when drawing text primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the character height value used
 *	when drawing text primitives.
 *	`Height' is specified in modeling coordinates.  The
 *	default value is 0.01.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in the `PEXoc.h' include file:
 *
 *	==
 *	PEX_SetCharHeight (ocbuf, height)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `height'
 *	is a pointer to a floating point value used as the
 *	character height.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXText", `PEXText2D"
 *
 *
 */

void
PEXSetCharHeight (ocbuf, height)

INPUT pxlOCBuf		*ocbuf;
INPUT float		height;

{
/* AUTHOR: */
    float		h;

    h = height;
    PEXAddOC (ocbuf, PEXOCCharHeight, sizeof( FLOAT), &h);

    return;
}

/*+
 * NAME:
 * 	PEXSetCharUpVector
 *
 * FORMAT:
 * 	`PEXSetCharUpVector`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	x		X component of the text up vector.
 *
 *	y		Y component of the text up vector.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the x and y components of the vector defining the "up"
 *	direction for text primitives.  This direction is
 *	specified in the text local coordinate system defined by the
 *	direction vectors associated with the text primitive.
 *	The text "character base vector" is implicitly defined
 *	to lie at a right angle clockwise from the character up vector.
 *	For example, x = 0 and y = 1 (the default case) defines an up
 *	vector perpendicular to the viewer's line of sight.
 *	(It appears "straight up" according to the orientation of the
 *	viewer.)
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in the `PEXoc.h' include file:
 *
 *	==
 *	PEX_SetCharUpVector (ocbuf, vector)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and
 *	`vector' is a pointer to a structure of type `pxlVector2D' (defined in
 *	the `PEXprotost.h' include file) which contains the values used as the
 *	character up vector.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXText", `PEXText2D"
 *
 */

void
PEXSetCharUpVector (ocbuf, x, y)

INPUT pxlOCBuf		*ocbuf;
INPUT float		x;
INPUT float		y;

{
/* AUTHOR: */
    pexVector2D		v;

    v.x = x;
    v.y = y;

    PEXAddOC (ocbuf, PEXOCCharUpVector, sizeof( pexVector2D), &v);

    return;
}

/*+
 * NAME:
 * 	PEXSetTextPath
 *
 * FORMAT:
 * 	`PEXSetTextPath`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	path		The text (drawing) path used for
 *			text primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the text path attribute.  Possible values for `path'
 *	are `PEXPathRight', `PathLeft', `PathUp', and `PEXPathDown'.  The `PEX.h'
 *	include file contains definitions for these four constants.  The
 *	default is `PEXPathRight'.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in the `PEXoc.h' include file:
 *
 *	==
 *	PEX_SetTextPath (ocbuf, path)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `path'
 *	is a pointer to a value of type `CARD16' (defined in the
 *	`Xmd.h' include file) containing the text path attribute used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXText", `PEXText2D"
 *
 */

void
PEXSetTextPath (ocbuf, path)

INPUT pxlOCBuf		*ocbuf;
INPUT int		path;

{
/* AUTHOR: */
    CARD16	p;

    p = path;
    PEXAddOC (ocbuf, PEXOCTextPath, sizeof( CARD16), &p);

    return;
}

/*+
 * NAME:
 * 	PEXSetTextAlignment
 *
 * FORMAT:
 * 	`PEXSetTextAlignment`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	halignment	An integer specifying the value of the
 *			horizontal text alignment attribute.
 *
 *	valignment	An integer specifying the value of the
 *			vertical text alignment attribute.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the horizontal and vertical text alignment attributes.
 *	Possible values for `halignment' are `PEXHalignNormal', `PEXHalignLeft',
 *	`PEXHalignRight', and `PEXHalignCenter'.  Possible values for `valignment'
 *	are `PEXValignNormal', `PEXValignTop', `PEXValignCap', `PEXValignHalf',
 *	`PEXValignBase', and `PEXValignBottom'.  The `PEX.h' include
 *	file contains definitions for these constants.  The default is
 *	`PEXHalignNormal', `PEXValignNormal'.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in the `PEXoc.h' include file:
 *
 *	==
 *	PEX_SetTextAlignment (ocbuf, alignment)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `alignment' is a
 *	pointer to a data structure of type `pxlTextAlignmentData' (defined
 *	in the `PEXprotost.h' include file) containing
 *	the values used for the text alignment attribute.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXText", `PEXText2D"
 *
 */

void
PEXSetTextAlignment (ocbuf, halignment, valignment)

INPUT pxlOCBuf		*ocbuf;
INPUT int		halignment;
INPUT int		valignment;

{
/* AUTHOR: */
    pexTextAlignmentData	a;

    a.vertical = valignment;
    a.horizontal = halignment;

    PEXAddOC (ocbuf, PEXOCTextAlignment, sizeof( pexTextAlignmentData), &a);

    return;
}

/*+
 * NAME:
 * 	PEXSetATextHeight
 *
 * FORMAT:
 * 	`PEXSetATextHeight`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	height		The height used when drawing annotation text
 *			primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the text height for annotation text primitives.
 *	`Height' is expressed in modeling coordinates.  The default
 *	value is 0.01.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in the `PEXoc.h' include file:
 *
 *	==
 *	PEX_SetATextHeight (ocbuf, height)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and
 *	`height' is a pointer to a floating point value containing the
 *	annotation text height value.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXAnnotationText", `PEXAnnotationText2D"
 *
 */

void
PEXSetATextHeight (ocbuf, height)

INPUT pxlOCBuf		*ocbuf;
INPUT float		height;

{
/* AUTHOR: */
    float		h;

    h = height;
    PEXAddOC (ocbuf, PEXOCAtextHeight, sizeof( FLOAT), &h);

    return;
}

/*+
 * NAME:
 * 	PEXSetATextUpVector
 *
 * FORMAT:
 * 	`PEXSetATextUpVector`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	x		X component of the annotation text up vector.
 *
 *	y		Y component of the annotation text up vector.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the x and y components of the vector defining the "up"
 *	direction for annotation text primitives.  This direction is
 *	specified in the text local coordinate system defined by the
 *	direction vectors associated with the annotation text primitive.
 *	The annotation text "character base vector" is implicitly defined
 *	to lie at a right angle clockwise from the character up vector.
 *	For example, x = 0 and y = 1 (the default case) defines an up
 *	vector perpendicular to the viewer's line of sight.
 *	(It appears "straight up" according to the orientation of the
 *	viewer.)
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in the `PEXoc.h' include file.
 *
 *	==
 *	PEX_SetATextUpVector (ocbuf, vector)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and
 *	`vector' is a pointer to a structure of type `pxlVector2D' (defined in
 *	the `PEXprotost.h' include file) containing the values used as the
 *	annotation text up vector.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXAnnotationText", `PEXAnnotationText2D"
 *
 */

void
PEXSetATextUpVector (ocbuf, x, y)

INPUT pxlOCBuf		*ocbuf;
INPUT float		x;
INPUT float		y;

{
/* AUTHOR: */
    pexVector2D		v;

    v.x = x;
    v.y = y;

    PEXAddOC (ocbuf, PEXOCAtextUpVector, sizeof( pexVector2D), &v);

    return;
}

/*+
 * NAME:
 * 	PEXSetATextPath
 *
 * FORMAT:
 * 	`PEXSetATextPath`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	path		The text (drawing) path used for annotation
 *			text primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the annotation text path attribute.  Possible values for `path'
 *	are `PEXPathRight', `PathLeft', `PathUp', and `PEXPathDown'.  Include file
 *	`PEX.h' contains definitions for these four constants .  The default
 *	is `PEXPathRight'.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetATextPath (ocbuf, path)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `path'
 *	is a pointer to a value of type `CARD16' (defined in include file
 *	`Xmd.h')  containing the annotation text path value.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXAnnotationText", `PEXAnnotationText2D"
 *
 */

void
PEXSetATextPath (ocbuf, path)

INPUT pxlOCBuf		*ocbuf;
INPUT int		path;

{
/* AUTHOR: */
    CARD16	p;

    p = path;
    PEXAddOC (ocbuf, PEXOCAtextPath, sizeof( CARD16), &p);

    return;
}

/*+
 * NAME:
 * 	PEXSetATextAlignment
 *
 * FORMAT:
 * 	`PEXSetATextAlignment`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	halignment	An integer specifying the value of the
 *			horizontal annotation text alignment attribute.
 *
 *	valignment	An integer specifying the value of the
 *			vertical annotation text alignment attribute.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the horizontal and vertical annotation text alignment attributes.
 *	Possible values for `halignment' are `PEXHalignNormal', `PEXHalignLeft',
 *	`PEXHalignRight', and `PEXHalignCenter'.  Possible values for `valignment'
 *	are `PEXValignNormal', `PEXValignTop', `PEXValignCap', `PEXValignHalf',
 *	`PEXValignBase', and `PEXValignBottom'.  Include file `PEX.h'
 *	contains definitions of these constants.  The default is
 *	`PEXHalignNormal', `PEXValignNormal'.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetATextAlignment (ocbuf, alignment)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `alignment' is a
 *	pointer to a data structure of type `pxlTextAlignmentData'
 *	(defined in include file `PEXprotost.h') containing
 *	the text alignment values.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXAnnotationText", `PEXAnnotationText2D"
 *
 */

void
PEXSetATextAlignment (ocbuf, halignment, valignment)

INPUT pxlOCBuf		*ocbuf;
INPUT int		halignment;
INPUT int		valignment;

{
/* AUTHOR: */
    pexTextAlignmentData	a;

    a.vertical = valignment;
    a.horizontal = halignment;

    PEXAddOC (ocbuf, PEXOCAtextAlignment, sizeof( pexTextAlignmentData), &a);

    return;
}

/*+
 * NAME:
 * 	PEXSetATextStyle
 *
 * FORMAT:
 * 	`PEXSetATextStyle`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	style		The annotation text style used for annotation
 *			text primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the annotation text style used for annotation text
 *	primitives.  The annotation text style determines
 *	whether the annotation text is connected to
 *	the text reference point by a line.  If a connecting line is
 *	to be drawn, the current set of line attributes is used.
 *	The annotation text style should be set to a style
 *	supported by the PEX server extension. (See `PEXGetEnumTypeInfo".)
 *	Registered values for the annotation text style are
 *	`PEXATextNotConnected' and `PEXATextConnected'.
 *	Include file `PEX.h' contains definitions of these constants.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetATextStyle (ocbuf, style)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `style'
 *	is a pointer to a value of type `pxlTableIndex' (defined in include
 *	file `PEXprotost.h') containing the annotation text style.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXAnnotationText", `PEXAnnotationText2D"
 *
 */

void
PEXSetATextStyle (ocbuf, style)

INPUT pxlOCBuf		*ocbuf;
INPUT int		style;

{
/* AUTHOR: */
    pexTableIndex	s;

    s = style;
    PEXAddOC (ocbuf, PEXOCAtextStyle, sizeof( pexTableIndex), &s);

    return;
}

/*+
 * NAME:
 * 	PEXSetTextBundleIndex
 *
 * FORMAT:
 * 	`PEXSetTextBundleIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		An index indicating the entry in the text bundle
 *			table used to obtain text attributes.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the index value used to obtain text attributes.
 *	Text and annotation text primitives rendered after this
 *	command use text attributes obtained from the text bundle lookup
 *	table entry referenced by `index' for text attributes whose attribute
 *	source flags are `PEXBundled'.  The default value of `index'
 *	is one.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetTextBundleIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlTableIndex' (defined in include
 *	file `PEXprotost.h') indicating the
 *	text bundle table entry from which text attributes can
 *	be obtained.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXText",
 *	`PEXText2D", `PEXAnnotationText", `PEXAnnotationText2D",
 *	`PEXCreateLookupTable"
 *
 */

void
PEXSetTextBundleIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCTextBundleIndex, sizeof( pexTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetLineType
 *
 * FORMAT:
 * 	`PEXSetLineType`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	type		The line type used when drawing line and curve
 *			primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the line type used when drawing line and curve primitives.
 *	This line type is used when the `PEXLineTypeAsf' is `PEXIndividual'.
 *	`Type' should be a line type supported by the PEX server
 *	extension.  (See `PEXGetEnumTypeInfo".)  Registered values for
 *	`type' are `PEXLineTypeSolid', `PEXLineTypeDashed',
 *	`PEXLineTypeDotted', and `LineTypeDashDot'.
 *	Include file `PEX.h'contains definitions for these constants.
 *	The default is `PEXLineTypeSolid'.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetLineType (ocbuf, type)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `type'
 *	is a pointer to a value of type `pxlEnumTypeIndex' (defined in include
 *	file `PEXprotost.h') containing the line
 *	type used for line and curve primitives.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXPolyline",
 *	`PEXPolyline2D", `PEXPolylineSet", `PEXNurbCurve"
 *
 */

void
PEXSetLineType (ocbuf, type)

INPUT pxlOCBuf		*ocbuf;
INPUT int		type;

{
/* AUTHOR: */
    pexEnumTypeIndex	t;

    t = type;
    PEXAddOC (ocbuf, PEXOCLineType, sizeof( pexEnumTypeIndex), &t);

    return;
}

/*+
 * NAME:
 * 	PEXSetLineWidth
 *
 * FORMAT:
 * 	`PEXSetLineWidth`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	width		The line width scale factor used when
 *			drawing line and curve primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets the line
 *	width scale factor used when drawing line and curve primitives.
 *	This scale factor is used when the `PEXLineWidthAsf' is `PEXIndividual'.
 *	This scale factor is used to increase or decrease the width (in pixels)
 *	from the nominal line width for the display device.  (See
 *	`PEXGetImpDepConstants".)  The default line width is 1.0.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetLineWidth (ocbuf, width)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `width'
 *	is a pointer to a floating point value representing the line width
 *	used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXPolyline",
 *	`PEXPolyline2D", `PEXPolylineSet", `PEXNurbCurve"
 *
 */

void
PEXSetLineWidth (ocbuf, width)

INPUT pxlOCBuf		*ocbuf;
INPUT float		width;

{
/* AUTHOR: */
    float		w;

    w = width;
    PEXAddOC (ocbuf, PEXOCLineWidth, sizeof( FLOAT), &w);

    return;
}

/*+
 * NAME:
 * 	PEXSetLineColourIndex
 *
 * FORMAT:
 * 	`PEXSetLineColourIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		The indexed colour used as the
 *			colour for drawing line and curve primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the colour used to draw line and curve primitives.
 *	This colour is used when the `PEXLineColourAsf' is `PEXIndividual'.
 *	This output command sets the line colour attribute to an
 *	indexed colour with the value indicated by `index'.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetLineColourIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlTableIndex' containing the
 *	indexed colour used as the colour for drawing lines and
 *	curves.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXPolyline",
 *	`PEXPolyline2D", `PEXPolylineSet", `PEXNurbCurve",
 *	`PEXCreateLookupTable"
 *
 */

void
PEXSetLineColourIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCLineColourIndex, sizeof( pexTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetLineColour
 *
 * FORMAT:
 * 	`PEXSetLineColour`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	colour		A pointer to the colour used when drawing
 *			line and curve primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the colour used to draw line and curve primitives.
 *	This colour is used when the `PEXLineColourAsf' is `PEXIndividual'.
 *	This output command sets the line colour attribute
 *	to either an indexed colour or a direct colour value, depending
 *	on the current value of the PEXlib colour type attribute.  The
 *	`pxlColour' data type is defined as a union of `pxlIndexedColour'
 *	and the data structures defining the direct colour formats.
 *	The colour must be in the current colour type.  (See `PEXSetColourType".)
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetLineColour (ocbuf, colour)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `colour'
 *	is a pointer to a structure of type `pxlColour' 
 *	containing the colour used for drawing
 *	lines and curves.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXPolyline",
 *	`PEXPolyline2D", `PEXPolylineSet", `PEXNurbCurve"
 *
 */

void
PEXSetLineColour (ocbuf, colour)

INPUT pxlOCBuf	*ocbuf;
INPUT pxlColour	*colour;

{
/* AUTHOR: */
    pxlColourSpecifier	pcs;

    InitializeColourSpecifier (pcs, colour);

    PEXAddOC (ocbuf, PEXOCLineColour,
	sizeof( pxlColourSpecifier) - AdjustSizeFromType( pexColourTypeHidden), 
	&pcs);

    return;
}

/*+
 * NAME:
 * 	PEXSetCurveApproxMethod
 *
 * FORMAT:
 * 	`PEXSetCurveApproxMethod`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	approxMethod
 *			The curve approximation method used
 *			to draw non-uniform B-spline curve
 *			primitives.
 *
 *	tolerance	A value used with the different approximation
 *			methods to indicate the desired accuracy.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the curve approximation method used when rendering
 *	non-uniform B-spline curve primitives.
 *	This curve approximation method is used when the `PEXCurveApproxAsf'
 *	is `PEXIndividual'.  Possible values for `approxMethod' are
 *	`PEXApproxImpDep' (to select the implementation-dependent method
 *	provided) which is the default, `PEXApproxConstantBetweenKnots',
 *	`PEXApproxConstantBetweenKnots', `ApproxWcsMetric',
 *	`ApproxNpcMetric', `PEXCurveApproxWcsChordalDev', and
 *	`PEXCurveApproxNpcChordalDev'.
 *	Include file `PEX.h' contains definitions of these constants.
 *	The `tolerance' value is
 *	provided to indicate the desired accuracy of the approximation, and is
 *	used in different ways for the different methods.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetCurveApproxMethod (ocbuf, method)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `method'
 *	is a pointer to a structure of type `pxlCurveApprox' (defined in
 *	include file `PEXprotost.h') defining the curve approximation method.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXNurbCurve"
 *
 */

void
PEXSetCurveApproxMethod (ocbuf, approxMethod, tolerance)

INPUT pxlOCBuf		*ocbuf;
INPUT int		approxMethod;
INPUT float		tolerance;

{
/* AUTHOR: */
    pexCurveApprox	c;

    c.approxMethod = approxMethod;
    c.tolerance = tolerance;

    PEXAddOC (ocbuf, PEXOCCurveApproximation, sizeof( pexCurveApprox), &c);

    return;
}

/*+
 * NAME:
 * 	PEXSetPolylineInterpMethod
 *
 * FORMAT:
 * 	`PEXSetPolylineInterpMethod`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	method		The polyline interpolation method used
 *			to draw polylines that have per-vertex colours.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	specifies the interpolation method used when
 *	rendering polyline primitives containing colours at each vertex.
 *	This method is used when the `PEXPolylineInterpAsf' is `PEXIndividual'.
 *	`Method' should be one of the polyline interpolation methods supported
 *	by the PEX server extension.  (See `PEXGetEnumTypeInfo".)
 *	Registered values are `PEXPolylineInterpNone' and
 *	`PEXPolylineInterpColour' (defined in include file `PEX.h').  If
 *	`PEXPolylineInterpNone'
 *	is specified, no interpolation is performed between polyline
 *	vertices.  If the colour values differ at the
 *	endpoints of a given polyline segment, it is implementation dependent
 *	whether the colour at vertex `i'
 *	is used to draw the segment between vertex `i' and vertex `i'+1 or
 *	whether the average of the two colours is used.  If the first way is
 *	implemented, the colour at the last vertex is not used.
 *	If `PEXPolylineInterpColour' is specified, the colour values
 *	along each polyline segment are computed by linearly interpolating
 *	between the colour values at the vertices.
 *	The default is `PEXPolylineInterpNone'.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetPolylineInterpMethod (ocbuf, method)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `method'
 *	is a pointer to a value of type `CARD16' (defined in include file
 *	`Xmd.h') containing the polyline
 *	interpolation method used to draw polyline primitives.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXPolylineSet"
 *
 */

void
PEXSetPolylineInterpMethod (ocbuf, method)

INPUT pxlOCBuf	*ocbuf;
INPUT int	method;

{
/* AUTHOR: */
    CARD16	m;

    m = method;
    PEXAddOC (ocbuf, PEXOCPolylineInterp, sizeof( CARD16), &m);

    return;
}

/*+
 * NAME:
 * 	PEXSetLineBundleIndex
 *
 * FORMAT:
 * 	`PEXSetLineBundleIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		An index indicating the entry in the line bundle
 *			table used to obtain line and curve
 *			attributes.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the index value used to obtain line and curve attributes.
 *	Line and curve primitives rendered after this command
 *	use line and curve attributes obtained from the line bundle lookup
 *	table entry referenced by `index' for those line attributes whose
 *	attribute source flags are `PEXBundled'.  The default value of `index'
 *	is one.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetLineBundleIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlTableIndex' (defined in include
 *	file `PEXprotost.h') indicating the
 *	line bundle table entry from which line and curve attributes can
 *	be obtained.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXPolyline",
 *	`PEXPolyline2D", `PEXPolylineSet", `PEXNurbCurve",
 *	`PEXCreateLookupTable"
 *
 */

void
PEXSetLineBundleIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCLineBundleIndex, sizeof( pexTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetInteriorStyle
 *
 * FORMAT:
 * 	`PEXSetInteriorStyle`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	style		The interior style used to render
 *			surface primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the interior style used to render surface
 *	primitives.  `Style' should be one of the interior styles
 *	supported by the PEX server extension. (See `PEXGetEnumTypeInfo".)
 *	Possible values for `style' are `PEXInteriorStyleHollow',
 *	`PEXInteriorStyleSolid', `PEXInteriorStylePattern', `PEXInteriorStyleHatch',
 *	and `PEXInteriorStyleEmpty'.
 *	Include file `PEX.h' contains definitions of these constants.
 *	The default is `PEXInteriorStyleHollow'.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetInteriorStyle (ocbuf, style)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `style'
 *	is a pointer to a value of type `pxlEnumTypeIndex' (defined in include
 *	file `PEXprotost.h') indicating the interior style used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetInteriorStyle (ocbuf, style)

INPUT pxlOCBuf		*ocbuf;
INPUT int		style;

{
/* AUTHOR: */
    pexEnumTypeIndex	s;

    s = style;
    PEXAddOC (ocbuf, PEXOCInteriorStyle, sizeof( pexEnumTypeIndex), &s);

    return;
}

/*+
 * NAME:
 * 	PEXSetInteriorStyleIndex
 *
 * FORMAT:
 * 	`PEXSetInteriorStyleIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		The interior style index value used.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the interior style index used to draw hatched or patterned
 *	surface primitives.
 *	This index is used when the `PEXInteriorStyleIndexAsf' is
 *	`PEXIndividual'.
 *	This attribute has no effect on surface rendering for Digital
 *	implementations of PEX, since they do not currently support the
 *	`PEXInteriorStyleHatch' and `PEXInteriorStylePattern' interior styles.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetInteriorStyleIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlEnumTypeIndex' (defined in include
 *	file `PEXprotost.h') used as the interior style index.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetInteriorStyleIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexEnumTypeIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCInteriorStyleIndex, sizeof( pexEnumTypeIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetSurfaceColourIndex
 *
 * FORMAT:
 * 	`PEXSetSurfaceColourIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		The indexed colour used as the surface
 *			colour when drawing surface primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the colour used to draw surface primitives.
 *	This colour is used when the `PEXSurfaceColourAsf' is `PEXIndividual'.
 *	This output
 *	command sets the surface colour attribute to an indexed colour
 *	with the value indicated by `index'.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetSurfaceColourIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlTableIndex' (defined in include
 *	file `PEXprotost.h') containing the indexed
 *	colour used as the surface colour.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface",
 *	`PEXCreateLookupTable"
 *
 */

void
PEXSetSurfaceColourIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCSurfaceColourIndex, sizeof( pexTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetSurfaceColour
 *
 * FORMAT:
 * 	`PEXSetSurfaceColour`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	colour		A pointer to the colour used when drawing
 *			surface primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the colour used to draw surface primitives.
 *	This colour is used when the `PEXSurfaceColourAsf' is `PEXIndividual'.
 *	This output command sets the surface
 *	colour attribute to either an indexed colour or a direct colour value,
 *	depending on the current value of the PEXlib colour type attribute.
 *	The `pxlColour' data type is defined as a union of `pxlIndexedColour'
 *	and the data structures defining the direct colour formats.
 *	The colour must be in the current colour type.  (See `PEXSetColourType".)
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetSurfaceColour (ocbuf, colour)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `colour'
 *	is a pointer to a structure of type `pxlColour' 
 *	containing the surface colour used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface",
 *	`PEXCreateLookupTable"
 *
 */

void
PEXSetSurfaceColour (ocbuf, colour)

INPUT pxlOCBuf	*ocbuf;
INPUT pxlColour	*colour;

{
/* AUTHOR: */
    pxlColourSpecifier	pcs;

    InitializeColourSpecifier (pcs, colour);

    PEXAddOC (ocbuf, PEXOCSurfaceColour,
	sizeof( pxlColourSpecifier) - AdjustSizeFromType( pexColourTypeHidden), 
	&pcs);

    return;
}

/*+
 * NAME:
 * 	PEXSetReflectionAttributes
 *
 * FORMAT:
 * 	`PEXSetReflectionAttributes`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	reflectionAttr	A structure containing the reflection attributes
 *			to be used when drawing surface primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the reflection attributes used to draw surface
 *	primitives.
 *	These reflection attributes is used when the `PEXReflectionAttrAsf'
 *	is `PEXIndividual'.
 *	See the `Pipeline Contexts' chapter for a
 *	description of how the fields in the
 *	`reflectionAttr' parameter are used.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetReflectionAttributes (ocbuf, attr)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer, and
 *	`attr' is a pointer to a structure of type
 *	`pxlReflectionAttr' containing the reflection attribute values.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetReflectionAttributes (ocbuf, reflectionAttr)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlReflectionAttr	*reflectionAttr;

{
/* AUTHOR: */
    PEXAddOC (ocbuf, PEXOCSurfaceReflAttr,
	sizeof( pxlReflectionAttr) - 
		AdjustSizeFromType( reflectionAttr->specularColour.colourType),
    	reflectionAttr);

    return;
}

/*+
 * NAME:
 * 	PEXSetReflectionModel
 *
 * FORMAT:
 * 	`PEXSetReflectionModel`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	model		The reflection model used when
 *			drawing surface primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the reflection model used to draw surface
 *	primitives.
 *	This model is used when the `PEXReflectionModelAsf' is `PEXIndividual'.
 *	`Model' should be a reflection model
 *	supported by the PEX server extension.  (See
 *	`PEXGetEnumTypeInfo".)  Registered values for `method' are
 *	`PEXReflectionNoShading', `PEXReflectionAmbient',
 *	`PEXReflectionDiffuse', and `PEXReflectionSpecular'.
 *	Include file `PEX.h' contains definitions of these constants.
 *	The default is `PEXReflectionNoShading'.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetReflectionModel (ocbuf, model)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `model'
 *	is a pointer to a value of type `pxlEnumTypeIndex' (defined in include
 *	file `PEXprotost.h') containing the reflection model used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetReflectionModel (ocbuf, model)

INPUT pxlOCBuf		*ocbuf;
INPUT int		model;

{
/* AUTHOR: */
    pexEnumTypeIndex	m;

    m = model;
    PEXAddOC (ocbuf, PEXOCSurfaceReflModel, sizeof( pexEnumTypeIndex), &m);

    return;
}

/*+
 * NAME:
 * 	PEXSetSurfaceInterpMethod
 *
 * FORMAT:
 * 	`PEXSetSurfaceInterpMethod`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	method		The surface interpolation method used
 *			when drawing surface primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the surface interpolation method used to draw surface
 *	primitives.
 *	This method is used when the `PEXSurfaceInterpAsf' is `PEXIndividual'.
 *	`Method' should be a surface interpolation method
 *	supported by the PEX server extension.  (See
 *	`PEXGetEnumTypeInfo".)  Registered values for `method' are
 *	`PEXSurfaceInterpNone', `PEXSurfaceInterpColour',
 *	`PEXSurfaceInterpDotProduct', and `PEXSurfaceInterpNormal'.
 *	Include file `PEX.h' contains definitions of these constants.
 *	The default is `PEXSurfaceInterpNone'.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetSurfaceInterpMethod (ocbuf, method)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `method'
 *	is a pointer to a value of type `pxlEnumTypeIndex' (defined in
 *	`PEXprotost.h') containing the
 *	surface interpolation method used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetSurfaceInterpMethod (ocbuf, method)

INPUT pxlOCBuf		*ocbuf;
INPUT int		method;

{
/* AUTHOR: */
    pexEnumTypeIndex	m;

    m = method;
    PEXAddOC (ocbuf, PEXOCSurfaceInterp, sizeof( pexEnumTypeIndex), &m);

    return;
}

/*+
 * NAME:
 * 	PEXSetBFInteriorStyle
 *
 * FORMAT:
 * 	`PEXSetBFInteriorStyle`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	style		The style used when drawing back-facing
 *			surface primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the interior style used to draw back-facing surface
 *	primitives when the current "distinguish" attribute is set
 *	to `True' and the culling mode attribute allows back-faces
 *	to be drawn.
 *	This style is used when the `PEXBfInteriorStyleAsf' is `PEXIndividual'.
 *	`Style' should be an interior style
 *	supported by the PEX server extension.  (See
 *	`PEXGetEnumTypeInfo".)  Registered values for `style' are
 *	`PEXInteriorStyleHollow', `PEXInteriorStyleSolid', and
 *	`PEXInteriorStyleEmpty'.
 *	Include file `PEX.h' contains definitions of these constants.
 *	(Digital implementations of PEX do not
 *	currently support `PEXInteriorStyleHatch', and `PEXInteriorStylePattern'.)
 *	The default is `PEXInteriorStyleHollow'.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetBFInteriorStyle (ocbuf, style)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `style'
 *	is a pointer to a value of type `pxlEnumTypeIndex' (defined in include
 *	file `PEXprotost.h') containing the interior
 *	style used for back-facing surface primitives.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetBFInteriorStyle (ocbuf, style)

INPUT pxlOCBuf		*ocbuf;
INPUT int		style;

{
/* AUTHOR: */
    pexEnumTypeIndex	s;

    s = style;
    PEXAddOC (ocbuf, PEXOCBfInteriorStyle, sizeof( pexEnumTypeIndex), &s);

    return;
}

/*+
 * NAME:
 * 	PEXSetBFInteriorStyleIndex
 *
 * FORMAT:
 * 	`PEXSetBFInteriorStyleIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		The interior style index used when
 *			drawing back-facing surface primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the interior style index used to draw back-facing hatched
 *	or patterned surface
 *	primitives when the current "distinguish" attribute is set
 *	to `True' and the culling mode attribute allows back-faces
 *	to be drawn.  This attribute has no effect on surface rendering
 *	in Digital implementations of PEX, since they do not currently support
 *	the `PEXInteriorStyleHatch' and `PEXInteriorStylePattern' interior styles.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetBFInteriorStyleIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlTableIndex' (defined in include
 *	file `PEXprotost.h') used as
 *	the interior style index for back-facing surface primitives.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetBFInteriorStyleIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCBfInteriorStyleIndex, sizeof( pexTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetBFSurfaceColourIndex
 *
 * FORMAT:
 * 	`PEXSetBFSurfaceColourIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		The indexed colour used as the surface
 *			colour when drawing back-facing surface primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the colour used to draw back-facing surface
 *	primitives when the current "distinguish" attribute is set
 *	to `True' and the culling mode attribute allows back-faces
 *	to be drawn.  This output command sets the back-facing
 *	surface colour attribute to an indexed colour with the value
 *	indicated by `index'.  This colour is used when the
 *	`PEXBfSurfaceColourAsf' is `PEXIndividual'.
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetBFSurfaceColourIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlTableIndex' (defined in include
 *	file `PEXprotost.h') containing the indexed
 *	colour used as the back-facing surface colour.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface",
 *	`PEXCreateLookupTable"
 *
 */

void
PEXSetBFSurfaceColourIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCBfSurfaceColourIndex, sizeof( pexTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetBFSurfaceColour
 *
 * FORMAT:
 * 	`PEXSetBFSurfaceColour`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	colour		A pointer to the colour used when drawing
 *			back-facing surface primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the colour used to draw back-facing surface
 *	primitives when the current "distinguish" attribute is set
 *	to `True' and the culling mode attribute allows back-faces
 *	to be drawn.  This output command sets the back-facing surface
 *	colour attribute to either an indexed colour or a direct colour value,
 *	depending on the current value of the PEXlib colour type attribute.
 *	This colour is used when the `PEXBfSurfaceColourAsf' is `PEXIndividual'.
 *	The `pxlColour' data type is defined as a union of `pxlIndexedColour'
 *	and the data structures defining the direct colour formats.
 *	The colour must be in the current colour type.  (See `PEXSetColourType".)
 *
 *	C programmers can elect to
 *	use the equivalent C macro defined in include file `PEXoc.h':
 *
 *	==
 *	PEX_SetBFSurfaceColour (ocbuf, colour)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `colour'
 *	is a pointer to a structure of type `pxlColour' 
 *	containing the colour used for back-facing
 *	surface primitives.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface",
 *	`PEXCreateLookupTable"
 *
 */

void
PEXSetBFSurfaceColour (ocbuf, colour)

INPUT pxlOCBuf	*ocbuf;
INPUT pxlColour	*colour;

{
/* AUTHOR: */
    pxlColourSpecifier	pcs;

    InitializeColourSpecifier (pcs, colour);

    PEXAddOC (ocbuf, PEXOCBfSurfaceColour,
	sizeof( pxlColourSpecifier) - AdjustSizeFromType( pexColourTypeHidden), 
	&pcs);

    return;
}

/*+
 * NAME:
 *	 PEXSetBFReflectionAttributes
 *
 * FORMAT:
 *	`PEXSetBFReflectionAttributes`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	reflectionAttr	A structure containing the reflection attributes
 *			to be used when drawing back-facing surface primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the reflection attributes used to draw back-facing surface
 *	primitives when the current "distinguish" attribute is set
 *	to `True' and the culling mode attribute allows back-faces
 *	to be drawn.
 *	These reflection attributes are used when the `PEXBfReflectionAttrAsf'
 *	is `PEXIndividual'.
 *	See the `Pipeline Contexts' chapter for a
 *	description of how the fields in the `reflectionAttr' parameter
 *	are used.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetBFReflectionAttributes (ocbuf, attr)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and
 *	`attr' is a pointer to a structure of type
 *	`pxlReflectionAttr' containing the
 *	reflection attributes used for back-facing surface primitives.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetBFReflectionAttributes (ocbuf, reflectionAttr)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlReflectionAttr	*reflectionAttr;

{
/* AUTHOR: */
    PEXAddOC (ocbuf, PEXOCBfSurfaceReflAttr,
	sizeof( pxlReflectionAttr) -
	 	AdjustSizeFromType( reflectionAttr->specularColour.colourType),
     	reflectionAttr);

    return;
}

/*+
 * NAME:
 * 	PEXSetBFReflectionModel
 *
 * FORMAT:
 * 	`PEXSetBFReflectionModel`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	model		The reflection model used when
 *			drawing back-facing surface primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the reflection model used to draw back-facing surface
 *	primitives when the current "distinguish" attribute is set
 *	to `True' and the culling mode attribute allows back-faces to be drawn.
 *	This reflection model is used when the `PEXBfReflectionModelAsf'
 *	is `PEXIndividual'.
 *	`Model' should be a reflection model
 *	supported by the PEX server extension.  (See
 *	`PEXGetEnumTypeInfo".)  Registered values for `method' are
 *	`PEXReflectionNoShading', `PEXReflectionAmbient',
 *	`PEXReflectionDiffuse', and `PEXReflectionSpecular'.
 *	Include file `PEX.h' contains definitions of these constants.
 *	The default is `PEXReflectionNoShading'.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetBFReflectionModel (ocbuf, model)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `model'
 *	is a pointer to a value of type `pxlEnumTypeIndex' (defined in
 *	include file `PEXprotost.h') containing the
 *	reflection model used for back-facing surface primitives.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetBFReflectionModel (ocbuf, model)

INPUT pxlOCBuf		*ocbuf;
INPUT int		model;

{
/* AUTHOR: */
    pexEnumTypeIndex	m;

    m = model;
    PEXAddOC (ocbuf, PEXOCBfSurfaceReflModel, sizeof( pexEnumTypeIndex), &m);

    return;
}

/*+
 * NAME:
 * 	PEXSetBFSurfaceInterpMethod
 *
 * FORMAT:
 * 	`PEXSetBFSurfaceInterpMethod`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	method		The surface interpolation method used
 *			when drawing back-facing surface primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the surface interpolation method used to draw back-facing surface
 *	primitives when the current "distinguish" attribute is set
 *	to `True' and the culling mode attribute allows back-faces
 *	to be drawn.
 *	This method is used if the `PEXBfSurfaceInterpAsf' is `PEXIndividual'.
 *	`Method' should be a surface interpolation method
 *	supported by the PEX server extension.  (See
 *	`PEXGetEnumTypeInfo".)  Registered values for `method' are
 *	`PEXSurfaceInterpNone', `PEXSurfaceInterpColour',
 *	`PEXSurfaceInterpDotProduct', and `PEXSurfaceInterpNormal'.
 *	Include file `PEX.h' contains definitions of these constants.
 *	The default is `PEXSurfaceInterpNone'.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetBFSurfaceInterpMethod (ocbuf, method)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `method'
 *	is a pointer to a value of type `pxlEnumTypeIndex' (defined in include
 *	file `PEXprotost.h') containing the
 *	surface interpolation method used for back-facing
 *	surface primitives.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetBFSurfaceInterpMethod (ocbuf, method)

INPUT pxlOCBuf		*ocbuf;
INPUT int		method;

{
/* AUTHOR: */
    pexEnumTypeIndex	m;

    m = method;
    PEXAddOC (ocbuf, PEXOCBfSurfaceInterp, sizeof( pexEnumTypeIndex), &m);

    return;
}

/*+
 * NAME:
 * 	PEXSetSurfaceApproxMethod
 *
 * FORMAT:
 * 	`PEXSetSurfaceApproxMethod`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	approxMethod 	The surface approximation method used to
 *			render non-uniform B-spline surface
 *			primitives.
 *
 *	uTolerance	A value used with the different approximation
 *			methods to indicate the desired accuracy in the `u'
 *			direction.
 *
 *	vTolerance	A value used with the different approximation
 *			methods to indicate the desired accuracy in the `v'
 *			direction.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets the
 *	surface approximation method used when rendering
 *	non-uniform B-spline surface primitives.
 *	This method is used if the `PEXSurfaceApproxAsf' is `PEXIndividual'.
 *	Possible values for `approxMethod' are `PEXApproxImpDep'
 *	(to select the implementation-dependent method
 *	provided) which is the default, `PEXApproxConstantBetweenKnots',
 *	`PEXApproxConstantBetweenKnots', `ApproxWcsMetric',
 *	`ApproxNpcMetric', `PEXSurfaceApproxWcsPlanarDev', and
 *	`PEXSurfaceApproxNpcPlanarDev'.
 *	Include file `PEX.h' contains definitions of these constants.
 *	The `uTolerance' and `vTolerance' values
 *	are provided to indicate the desired accuracy of the approximation,
 *	and are used in different ways for the different methods.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetSurfaceApproxMethod (ocbuf, method)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `method' is
 *	a pointer to a structure of type `pxlSurfaceApprox' (defined in include
 *	file `PEXprotost.h') which contains the surface approximation method to
 *	be used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXNurbSurface"
 *
 */

void
PEXSetSurfaceApproxMethod (ocbuf, approxMethod, uTolerance, vTolerance)

INPUT pxlOCBuf		*ocbuf;
INPUT int		approxMethod;
INPUT float		uTolerance;
INPUT float		vTolerance;

{
/* AUTHOR: */
    pexSurfaceApprox	a;

    a.approxMethod = approxMethod;
    a.uTolerance = uTolerance;
    a.vTolerance = vTolerance;

    PEXAddOC (ocbuf, PEXOCSurfaceApproximation, sizeof( pexSurfaceApprox), &a);

    return;
}

/*+
 * NAME:
 * 	PEXSetFacetCullingMode
 *
 * FORMAT:
 * 	`PEXSetFacetCullingMode`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	mode		The mode used to cull surface facets.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the facet culling mode attribute to `mode'.  If the value of
 *	`mode' is `PEXBackFaces', all back-facing surfaces
 *	are culled and only front-facing surfaces are rendered.
 *	If the value of `mode' is `PEXFrontFaces', all front-facing surfaces
 *	are culled and only back-facing surfaces are rendered.
 *	Include file `PEX.h' contains definitions of these constants.
 *	If the value of `mode' is `None', both front- and back-facing
 *	surfaces are rendered.
 *	The default is `PEXBackFaces'.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetFacetCullingMode (ocbuf, mode)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `mode'
 *	is a pointer to a value of type `pxlCullMode' (defined in include file
 *	`PEXprotost.h') containing the culling
 *	mode used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer" `PEXFillArea", `PEXFillArea2D",
 *	`PEXMultiFillAreaData", `PEXFillAreaSet", `PEXFillAreaSet2D",
 *	`PEXMultiFillAreaSetData", `PEXTriangleStrip", `PEXQuadMesh",
 *	`PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetFacetCullingMode (ocbuf, mode)

INPUT pxlOCBuf		*ocbuf;
INPUT int		mode;

{
/* AUTHOR: */
    pexCullMode		m;

    m = mode;
    PEXAddOC (ocbuf, PEXOCCullingMode, sizeof( pexCullMode), &m);

    return;
}

/*+
 * NAME:
 * 	PEXSetFacetDistinguishFlag
 *
 * FORMAT:
 * 	`PEXSetFacetDistinguishFlag`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	flag		A flag indicating whether to distinguish
 *			back-facing surfaces from front-facing surfaces.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the facet distinguish flag attribute to the value passed
 *	in `flag'.  This flag value determines
 *	whether back-facing surfaces are rendered with the
 *	back-face surface attributes or with the front-face surface
 *	attributes.  If the value of `flag' is `False',
 *	front-face attributes are used to render the back-facing surfaces.
 *	If the value of `flag' is `True', back-face attributes are used
 *	to render back-facing surfaces.
 *	The default is `False'.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetFacetDistinguishFlag (ocbuf, flag)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `flag'
 *	is a pointer to a value of type `pxlSwitch' (defined in
 *	include file `PEXprotost.h') containing the facet distinguish flag
 *	value used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXFillArea", `PEXFillArea2D",
 *	`PEXMultiFillAreaData", `PEXFillAreaSet", `PEXFillAreaSet2D",
 *	`PEXMultiFillAreaSetData", `PEXTriangleStrip", `PEXQuadMesh",
 *	`PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetFacetDistinguishFlag (ocbuf, flag)

INPUT pxlOCBuf		*ocbuf;
INPUT int		flag;

{
/* AUTHOR: */
    pexSwitch	f;

    f = flag;
    PEXAddOC (ocbuf, PEXOCDistinguishFlag, sizeof( pexSwitch), &f);

    return;
}

/*+
 * NAME:
 * 	PEXSetPatternSize
 *
 * FORMAT:
 * 	`PEXSetPatternSize`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	width		The width value of the pattern.
 *
 *	height		The height value of the pattern.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the pattern size used when processing patterned surface
 *	primitives.  The value (x,0) is
 *	used as the pattern width vector, and the value (0,y) specifies
 *	the pattern height vector.  If the interior style is
 *	`PEXInteriorStylePattern',
 *	the renderer attempts to use these values, plus the pattern
 *	reference point and the pattern reference vector, to position,
 *	scale, and rotate the pattern on the surface.
 *
 *	Note: patterned polygons are not supported by Digital
 *	implementations of PEX.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetPatternSize (ocbuf, size)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `size' is
 *	a pointer to a structure of type `pxlCoord2D' (defined in include file
 *	`PEXprotost.h') containing the pattern size for patterned surface
 *	primitives.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXFillArea", `PEXFillArea2D",
 *	`PEXMultiFillAreaData", `PEXFillAreaSet", `PEXFillAreaSet2D",
 *	`PEXMultiFillAreaSetData", `PEXTriangleStrip", `PEXQuadMesh",
 *	`PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetPatternSize (ocbuf, width, height)

INPUT pxlOCBuf		*ocbuf;
INPUT float		width;
INPUT float		height;

{
/* AUTHOR: */
    pexCoord2D		c;

    c.x = width;
    c.y = height;

    PEXAddOC (ocbuf, PEXOCPatternSize, sizeof( pexCoord2D), &c);

    return;
}

/*+
 * NAME:
 * 	PEXSetPatternRefPt
 *
 * FORMAT:
 * 	`PEXSetPatternRefPt`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	x		The x-coordinate of the pattern reference point.
 *
 *	y		The y-coordinate of the pattern reference point.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the pattern reference point and pattern reference vectors used when
 *	processing patterned surface primitives.  The pattern reference
 *	point is set to (x,y,0), and the two pattern reference vectors are set
 *	to (1,0,0) and (0,1,0).
 *	If the surface interior style is `PEXInteriorStylePattern', the
 *	renderer attempts to use the pattern reference point, reference
 *	vectors, and the pattern size to position and scale the
 *	pattern on the surface.
 *
 *	Note:  patterned polygons are not supported by Digital
 *	implementations of PEX.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetPatternRefPt (ocbuf, refPt)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `refPt' is
 *	a pointer to a structure of type `pxlCoord2D' (defined in include file
 *	`PEXprotost.h') containing the pattern reference point used
 *	for patterned surface primitives.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXFillArea", `PEXFillArea2D",
 *	`PEXMultiFillAreaData", `PEXFillAreaSet", `PEXFillAreaSet2D",
 *	`PEXMultiFillAreaSetData", `PEXTriangleStrip", `PEXQuadMesh",
 *	`PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetPatternRefPt (ocbuf, x, y)

INPUT pxlOCBuf		*ocbuf;
INPUT float		x;
INPUT float		y;

{
/* AUTHOR: */
    pexCoord2D		c;

    c.x = x;
    c.y = y;

    PEXAddOC (ocbuf, PEXOCPatternRefPt, sizeof( pexCoord2D), &c);

    return;
}

/*+
 * NAME:
 * 	PEXSetPatternAttributes
 *
 * FORMAT:
 * 	`PEXSetPatternAttributes`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	refPt		The coordinates of the pattern reference point.
 *
 *	vec1		The coordinates of the first pattern reference vector.
 *
 *	vec2		The coordinates of the second pattern reference vector.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the pattern reference point `refPt' and the pattern
 *	reference vectors `vec1' and `vec2' used when
 *	processing patterned surface primitives.  If the
 *	surface interior type is `PEXInteriorStylePattern', the renderer attempts
 *	to use the pattern reference point, the pattern reference
 *	vectors, and the pattern size to position and scale the
 *	pattern on the surface.
 *
 *	Note:  patterned polygons are not supported by Digital
 *	implementations of PEX.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetPatternAttributes (ocbuf, attr)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `attr'
 *	is a pointer to a structure of type `pxlPatternAttr' (defined in
 *	include file `PEXprotost.h') containing the pattern attributes to
 *	be used for patterned surface primitives.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXFillArea", `PEXFillArea2D",
 *	`PEXMultiFillAreaData", `PEXFillAreaSet", `PEXFillAreaSet2D",
 *	`PEXMultiFillAreaSetData", `PEXTriangleStrip", `PEXQuadMesh",
 *	`PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetPatternAttributes (ocbuf, refPt, vec1, vec2)

INPUT pxlOCBuf			*ocbuf;
INPUT pxlCoord3D		*refPt;
INPUT pxlVector3D		*vec1;
INPUT pxlVector3D		*vec2;

{
/* AUTHOR: */
    pexCoord3D		attr[3];

    attr[0].x = refPt->x;
    attr[0].y = refPt->y;
    attr[0].z = refPt->z;
    attr[1].x = vec1->x;
    attr[1].y = vec1->y;
    attr[1].z = vec1->z;
    attr[2].x = vec2->x;
    attr[2].y = vec2->y;
    attr[2].z = vec2->z;

    PEXAddOC (ocbuf, PEXOCPatternAttr, sizeof(attr), attr);

    return;
}

/*+
 * NAME:
 * 	PEXSetInteriorBundleIndex
 *
 * FORMAT:
 * 	`PEXSetInteriorBundleIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		An index indicating the entry in the interior
 *			bundle 	table used to obtain surface
 *			attributes.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the index value used to obtain surface attributes.
 *  	Primitives rendered after this command
 *	use surface attributes obtained from the line bundle lookup
 *	table entry referenced by `index' for those surface interior
 *	attributes whose attribute source flags are `PEXBundled'.
 *	The default value of `index' is one.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetInteriorBundleIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlTableIndex' (defined in
 *	include file `PEXprotost.h') indicating the
 *	interior bundle table entry from which surface attributes can
 *	be obtained.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface",
 *	`PEXCreateLookupTable"
 *
 */

void
PEXSetInteriorBundleIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCInteriorBundleIndex, sizeof( pexTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetSurfaceEdgeFlag
 *
 * FORMAT:
 * 	`PEXSetSurfaceEdgeFlag`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	flag		A flag specifying whether surface
 *			edge drawing is enabled or disabled.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the surface edges attribute.
 *	This flag is used when the `PEXSurfaceEdgesAsf' is `PEXIndividual'.
 *	Possible values for `flag' are `PEXOn' and `PEXOff'.
 *	Include file `PEX.h' contains definitions of these constants.
 *	If `flag' is `PEXOn', surface edge drawing is enabled.  If `flag'
 *	is `PEXOff', surface edge drawing is disabled.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetSurfaceEdgeFlag (ocbuf, flag)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `flag'
 *	is a pointer to a value of type `pxlSwitch' (defined in include file
 *	`PEXprotost.h') containing the surface
 *	edges value used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetSurfaceEdgeFlag (ocbuf, flag)

INPUT pxlOCBuf		*ocbuf;
INPUT int		flag;

{
/* AUTHOR: */
    pexSwitch	f;

    f = flag;
    PEXAddOC (ocbuf, PEXOCSurfaceEdgeFlag, sizeof( pexSwitch), &f);

    return;
}

/*+
 * NAME:
 * 	PEXSetSurfaceEdgeType
 *
 * FORMAT:
 * 	`PEXSetSurfaceEdgeType`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	type		The surface edge type used when drawing edges
 *			of surface primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets the surface
 *	edge type used when drawing edges of surface primitives.
 *	This type is used when the `PEXSurfaceEdgeTypeAsf' is `PEXIndividual'.
 *	`Type' should be a surface edge type supported by the PEX server
 *	extension.  (See `PEXGetEnumTypeInfo".)  Registered values for
 *	`type' are `PEXSurfaceEdgeSolid', `PEXSurfaceEdgeDashed',
 *	`PEXSurfaceEdgeDotted', and `PEXSurfaceEdgeDashDot'.
 *	Include file `PEX.h' contains definitions of these constants.
 *	The default is `PEXSurfaceEdgeSolid'.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetSurfaceEdgeType (ocbuf, type)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `type'
 *	is a pointer to a value of type `pxlEnumTypeIndex' (defined in include
 *	file `PEXprotost.h') containing the
 *	type used for drawing surface edges.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetSurfaceEdgeType (ocbuf, type)

INPUT pxlOCBuf		*ocbuf;
INPUT int		type;

{
/* AUTHOR: */
    pexEnumTypeIndex	t;

    t = type;
    PEXAddOC (ocbuf, PEXOCSurfaceEdgeType, sizeof( pexEnumTypeIndex), &t);

    return;
}

/*+
 * NAME:
 * 	PEXSetSurfaceEdgeWidth
 *
 * FORMAT:
 * 	`PEXSetSurfaceEdgeWidth`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	width		The surface edge width scale factor used
 *			when drawing edges of surface primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets the surface edge
 *	width scale factor used when drawing edges of surface primitives.
 *	This scale factor is used when the `SurfaceEdgeWidthAsf' is
 *	`PEXIndividual'.
 *	The scale factor increases or decreases the width (in pixels)
 *	from the nominal edge width for the display device.  (See
 *	`PEXGetImpDepConstants".)  The default edge width is 1.0.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetSurfaceEdgeWidth (ocbuf, width)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `width'
 *	is a pointer to a floating point value representing the edge width
 *	used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface"
 *
 */

void
PEXSetSurfaceEdgeWidth (ocbuf, width)

INPUT pxlOCBuf		*ocbuf;
INPUT float		width;

{
/* AUTHOR: */
    float		w;

    w = width;
    PEXAddOC (ocbuf, PEXOCSurfaceEdgeWidth, sizeof( FLOAT), &w);

    return;
}

/*+
 * NAME:
 * 	PEXSetSurfaceEdgeColourIndex
 *
 * FORMAT:
 * 	`PEXSetSurfaceEdgeColourIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		The indexed colour used as the surface
 *			edge colour.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets
 *	the colour used to draw surface edges when surface edge
 *	drawing is enabled.
 *	This edge colour is used when the `PEXSurfaceEdgeColourAsf' is
 *	`PEXIndividual'.
 *	This output command sets the surface edge
 *	colour attribute to an indexed colour with the value indicated by
 *	`index'.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetEdgeColourIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlTableIndex' (defined in include
 *	file `PEXprotost.h') containing the indexed
 *	colour used as the surface edge colour.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface",
 *	`PEXCreateLookupTable"
 *
 */

void
PEXSetSurfaceEdgeColourIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCSurfaceEdgeColourIndex, sizeof( pexTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetSurfaceEdgeColour
 *
 * FORMAT:
 * 	`PEXSetSurfaceEdgeColour`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	colour		A pointer to the colour used when drawing
 *			edges of surface primitives.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the colour used to draw edges of surface primitives.
 *	This edge colour is used when the `PEXSurfaceEdgeColourAsf' is
 *	`PEXIndividual'.
 *	This output command sets the surface edge colour attribute
 *	to either an indexed colour or a direct colour value, depending
 *	on the current value of the PEXlib colour type attribute.  The
 *	`pxlColour' data type is defined as a union of `pxlIndexedColour'
 *	and the data structures defining the direct colour formats.
 *	The colour must be in the current colour type.  (See `PEXSetColourType".)
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetEdgeColour (ocbuf, colour)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `colour'
 *	is a pointer to a structure of type `pxlColour' 
 *	containing the colour used for
 *	drawing surface edges.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface",
 *	`PEXCreateLookupTable"
 *
 */

void
PEXSetSurfaceEdgeColour (ocbuf, colour)

INPUT pxlOCBuf	*ocbuf;
INPUT pxlColour	*colour;

{
/* AUTHOR: */
    pxlColourSpecifier	pcs;

    InitializeColourSpecifier (pcs, colour);

    PEXAddOC (ocbuf, PEXOCSurfaceEdgeColour,
	sizeof( pxlColourSpecifier) - AdjustSizeFromType( pexColourTypeHidden), 
	&pcs);

    return;
}

/*+
 * NAME:
 * 	PEXSetEdgeBundleIndex
 *
 * FORMAT:
 * 	`PEXSetEdgeBundleIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		An index indicating the entry in the edge bundle
 *			table used to obtain surface edge
 *			attributes.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the index value used to obtain surface edge attributes.
 *  	Surface primitives rendered after this command
 *	use surface edge attributes obtained from the edge bundle lookup
 * 	table entry referenced by `index' for
 *	those edge attributes whose attribute source flags are `PEXBundled'.
 *	The default value of `index' is one.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetEdgeBundleIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlTableIndex' 
 *	indicating the edge bundle table entry from
 *	which surface edge attributes can be obtained.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetIndividualASF", `PEXFillArea",
 *	`PEXFillArea2D", `PEXMultiFillAreaData", `PEXFillAreaSet",
 *	`PEXFillAreaSet2D", `PEXMultiFillAreaSetData", `PEXTriangleStrip",
 *	`PEXQuadMesh", `PEXIndexedPolygon", `PEXNurbSurface",
 *	`PEXCreateLookupTable"
 *
 */

void
PEXSetEdgeBundleIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCEdgeBundleIndex, sizeof( pexTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetIndividualASF
 *
 * FORMAT:
 * 	`PEXSetIndividualASF`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	attribute	An integer specifying the PEX attribute
 *			whose attribute source flag value is set.
 *
 *	value		An integer value specifying the value
 *			of the attribute source flag setting.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the specified attribute source flag to `value'.
 *	Possible values for `value' are `PEXIndividual'
 *	and `PEXBundled' (both defined in include file `PEX.h'.)  When rendering,
 *	if `value' is `PEXIndividual', the value for the corresponding attribute is
 *	obtained directly from the current rendering pipeline state.
 *	If `value' is `PEXBundled', the value for the attribute is
 *	obtained from the appropriate lookup table instead.  Possible values
 *	for `attribute' are:
 *
 *	>
 *	`PEXMarkerTypeAsf'
 *	`PEXMarkerScaleAsf'
 *	`PEXMarkerColourAsf'
 *	`PEXTextFontIndexAsf'
 *	`PEXTextPrecAsf'
 *	`PEXCharExpansionAsf'
 *	`PEXCharSpacingAsf'
 *	`PEXTextColourAsf'
 *	`PEXLineTypeAsf'
 *	`PEXLineWidthAsf'
 *	`PEXLineColourAsf'
 *	`PEXCurveApproxAsf'
 *	`PEXPolylineInterpAsf'
 *	`PEXInteriorStyleAsf'
 *	`PEXInteriorStyleIndexAsf'
 *	`PEXSurfaceColourAsf'
 *	`PEXSurfaceInterpAsf'
 *	`PEXReflectionModelAsf'
 *	`PEXReflectionAttrAsf'
 *	`BFInteriorStyleAsf'
 *	`BFInteriorStyleIndexAsf'
 *	`BFSurfaceColourAsf'
 *	`BFSurfaceInterpAsf'
 *	`BFReflectionModelAsf'
 *	`BFReflectionAttrAsf'
 *	`PEXSurfaceApproxAsf'
 *	`PEXSurfaceEdgesAsf'
 *	`PEXSurfaceEdgeTypeAsf'
 *	`SurfaceEdgeWidthAsf'
 *	`PEXSurfaceEdgeColourAsf'
 *	<
 *
 *	Include file `PEX.h' contains definitions of these constants.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetIndividualASF (ocbuf, asf)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `asf'
 *	is a pointer to a structure of type `pxlAsfData' (defined in include file
 *	`PEXlib.h') containing the ASF values to be set.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer"
 *
 */

void
PEXSetIndividualASF (ocbuf, attribute, value)

INPUT pxlOCBuf		*ocbuf;
INPUT int		attribute;
INPUT int		value;

{
/* AUTHOR: */
    pxlAsfData	asf;

    asf.attribute = attribute;
    asf.value = value;

    PEXAddOC (ocbuf, PEXOCSetAsfValues, sizeof( pxlAsfData), &asf);

    return;
}

/*+
 * NAME:
 * 	PEXSetLocalTransform
 *
 * FORMAT:
 * 	`PEXSetLocalTransform`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	transform	The matrix and composition rule used to compute
 *			the new local transformation matrix.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this command modifies the renderer's
 *	local transformation matrix.  `Transform' is a 4x4 array of floating
 *	point values and a `composition' field describing how the specified
 *	matrix is combined with the existing local transformation matrix.
 *	If `composition' is `PEXPreConcatenate', the specified matrix is
 *	pre-concatenated to the current local transformation matrix.
 *	If `composition' is `PEXPostConcatenate', the specified
 *	matrix is post-concatenated to the current local model
 *	transformation matrix.  If `composition' is `{PEXReplace OR PEXNSReplace}',
 *	the specified matrix replaces the current local model
 *	transformation matrix.
 *	Include file `PEX.h' contains definitions of these constants.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetLocalTransform (ocbuf, transform)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `transform'
 *	is a pointer to a data structure of type `pxlLocalTransform3DData'
 *	(defined in include file `PEXprotost.h') containing the local
 *	transformation to be used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetLocalTransform2D"
 *
 */

void
PEXSetLocalTransform (ocbuf, transform)

INPUT pxlOCBuf			*ocbuf;
INPUT pxlLocalTransform3DData	*transform;

{
/* AUTHOR: */
    PEXAddOC (ocbuf, PEXOCLocalTransform, sizeof( pxlLocalTransform3DData), 
	transform);

    return;
}

/*+
 * NAME:
 * 	PEXSetLocalTransform2D
 *
 * FORMAT:
 * 	`PEXSetLocalTransform2D`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	transform	The matrix and composition rule used to compute
 *			the new local transformation matrix.
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *	This output command is similar to `PEXSetLocalTransform" except
 *	that the local transformation matrix is specified as a 3x3 matrix.
 *	When processed by a renderer,
 *	it is expanded to a 4x4 matrix by inserting a column of zeros between
 *	the second and third columns of the matrix and inserting a
 *	row of zeros between the second and third rows of the matrix.
 *	The entry in the third row, third column is then set to 1.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetLocalTransform2D (ocbuf, transform)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `transform'
 *	is a pointer to a data structure of type `pxlLocalTransform2DData'
 *	(defined in include file `PEXprotost.h') containing the local
 *	transformation to be used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetLocalTransform"
 *
 */

void
PEXSetLocalTransform2D (ocbuf, transform)

INPUT pxlOCBuf			*ocbuf;
INPUT pxlLocalTransform2DData	*transform;

{
/* AUTHOR: */
    PEXAddOC (ocbuf, PEXOCLocalTransform2D, sizeof( pxlLocalTransform2DData), 
	transform);

    return;
}

/*+
 * NAME:
 * 	PEXSetGlobalTransform
 *
 * FORMAT:
 * 	`PEXSetGlobalTransform`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	transform	The matrix used as the new global transformation
 *			matrix.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this command replaces the current
 *	global transformation matrix with the matrix specified by `transform'.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetGlobalTransform (ocbuf, transform)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `transform'
 *	is a pointer to a data structure of type `pxlMatrix' (defined in
 *	include file `PEXprotost.h') containing the global transformation used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetGlobalTransform2D"
 *
 */

void
PEXSetGlobalTransform (ocbuf, transform)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlMatrix		transform;

{
/* AUTHOR: */
    PEXAddOC (ocbuf, PEXOCGlobalTransform, sizeof( pxlMatrix), transform);

    return;
}

/*+
 * NAME:
 * 	PEXSetGlobalTransform2D
 *
 * FORMAT:
 * 	`PEXSetGlobalTransform2D`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	transform	The matrix used as the new global transformation
 *			matrix.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This output command is similar to `PEXSetGlobalTransform" except
 *	that the global transformation matrix is specified as a 3x3 matrix.
 *	When processed by a renderer,
 *	it is expanded to a 4x4 matrix by inserting a column of zeros between
 *  	the second and third columns of the matrix and then inserting
 *	a row of zeros between the second and third rows of the matrix
 *	provided.  The entry in the third row, third column is then set to 1.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetGlobalTransform2D (ocbuf, transform)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `transform'
 *	is a pointer to a data structure of type `pxlMatrix3X3' (defined in
 *	include file `PEXprotost.h') containing the global transformation used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetGlobalTransform"
 *
 */

void
PEXSetGlobalTransform2D (ocbuf, transform)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlMatrix3X3	transform;

{
/* AUTHOR: */
    PEXAddOC (ocbuf, PEXOCGlobalTransform2D, sizeof( pxlMatrix3X3), transform);

    return;
}

/*+
 * NAME:
 * 	PEXSetModelClipFlag
 *
 * FORMAT:
 * 	`PEXSetModelClipFlag`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	flag		A flag indicating whether modeling clipping is
 *			enabled or disabled.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	Digital implementations of PEX do not currently support this output
 *	command.
 *
 *	When processed by a renderer, this output command enables or
 *	disables modeling clipping in the rendering pipeline, depending
 *	on the value of `flag'.  Possible values for `flag' are `PEXClip' (enable
 *	modeling clipping) and `PEXNoClip' (disable modeling clipping).
 *	Include file `PEX.h' contains definitions of these constants.
 *	The default is modeling clipping disabled.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetModelClipFlag (ocbuf, flag)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `flag'
 *	is a pointer to a value of type `pxlSwitch' (defined in include file
 *	`PEXprotost.h') containing the modeling
 *	clipping flag used.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetModelClipVolume", `PEXSetModelClipVolume2D",
 *	`PEXRestoreModelClip"
 *
 */

void
PEXSetModelClipFlag (ocbuf, flag)

INPUT pxlOCBuf		*ocbuf;
INPUT int		flag;

{
/* AUTHOR: */
    pexSwitch	f;

    f = flag;
    PEXAddOC (ocbuf, PEXOCModelClip, sizeof( pexSwitch), &f);

    return;
}

/*+
 * NAME:
 * 	PEXSetModelClipVolume
 *
 * FORMAT:
 * 	`PEXSetModelClipVolume`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	operator	The model clip volume operator set.
 *
 *	halfSpaces	An array of structures defining the
 *			coordinates and the normal vectors for the
 *			model clipping volume.
 *
 *	numHalfSpaces	The number of clipping volume halfspaces.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the model clip volume for rendering output primitives
 *	when modelling clipping is enabled.  Possible values
 *	for `operator' are `PEXModelClipReplace' and `PEXModelClipIntersection'.
 *	Include file `PEX.h' contains definitions of these constants.
 *	If `operator' is `PEXModelClipReplace',
 *	the resultant clipping volume is set to the
 *	newly-defined values.  If `operator' is `PEXModelClipIntersection',
 *	the resultant clipping volume is defined as the intersection
 *	of the current modeling clipping volume (defined by the
 *	default value in the current pipeline context) and the
 *	newly-defined clipping volume.
 *
 *	`halfSpaces' is an array of `pxlHalfSpace' structures (defined in
 *	include file `PEXprotost.h') defining the three-dimentional coordinates
 *	and the direction of the normal vector for each half-space
 *	to be defined.  The normal vector is normal to the plane
 *	defining the bound of the half-space, and points away from the
 *	primitives to be clipped.  `numHalfSpaces' specifies the
 *	number of half spaces being defined.
 *
 *	No equivalent C-language macro is provided for this routine.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetModelClipFlag", `PEXSetModelClipVolume2D",
 *	`PEXRestoreModelClip"
 *
 */

void
PEXSetModelClipVolume (ocbuf, operator, halfSpaces, numHalfSpaces)

INPUT pxlOCBuf		*ocbuf;
INPUT int		operator;
INPUT pxlHalfSpace	*halfSpaces;
INPUT int		numHalfSpaces;

{
/* AUTHOR: */
    pexModelClipVolume 	*pReq;
    int			lenofData;

    lenofData = NUMWORDS( numHalfSpaces * sizeof(pxlHalfSpace) );
    PEXInitOC( ocbuf, PEXOCModelClipVolume, 
		NUMWORDS(sizeof(pexModelClipVolume)),
		lenofData, &pReq);
    if (pReq == NULL) return;

    pReq->modelClipOperator = operator; 
    pReq->numHalfSpaces = numHalfSpaces;

    PEXCopyWordsToOC( ocbuf, lenofData, halfSpaces);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXSetModelClipVolume2D
 *
 * FORMAT:
 * 	`PEXSetModelClipVolume2D`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	operator	The model clip volume operator.
 *
 *	halfSpaces	An array of structures defining the
 *			coordinates and the normal vectors for the
 *			the clipping volume.
 *
 *	numHalfSpaces	The number of clipping volume halfSpaces.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the model clip volume for output primitives when modelling
 *	clipping is enabled.  Possible values for `operator' are
 *	`PEXModelClipReplace' and `ModelClipIntersect'.
 *	Include file `PEX.h' contains definitions of these constants.  If
 *	`operator' is `PEXModelClipReplace', the resultant clipping volume is set
 *	to the newly-defined values.  If `operator' is `ModelClipIntersect',
 *	the resultant clipping volume is defined as the intersection
 *	of the current modeling clipping volume (defined by the
 *	default value in the current pipeline context) and the
 *	newly-defined clipping volume.
 *
 *	`halfSpaces' is an array of `pxlHalfSpace2D' structures (defined in
 * 	include file `PEXprotost.h') defining the two-dimensional coordinates
 *	and the direction of the normal vector for each half-space
 *	to be defined.  The z-component of each point and vector normal
 *	are assumed zero.  The vector is normal to the plane
 *	defining the bound of the half-space, and points away from the
 *	primitives clipped.  `numHalfSpaces' specifies the
 *	number of half spaces defined.
 *
 *	No equivalent C-language macro is provided for this routine.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetModelClipFlag", `PEXSetModelClipVolume",
 *	`PEXRestoreModelClipVolume"
 *
 */

void
PEXSetModelClipVolume2D (ocbuf, operator, halfSpaces, numHalfSpaces)

INPUT pxlOCBuf		*ocbuf;
INPUT int		operator;
INPUT pxlHalfSpace2D	*halfSpaces;
INPUT int		numHalfSpaces;

{
/* AUTHOR: */
    pexModelClipVolume2D 	*pReq;
    int				lenofData;

    lenofData = NUMWORDS( numHalfSpaces * sizeof(pxlHalfSpace2D));
    PEXInitOC( ocbuf, PEXOCModelClipVolume2D, 
		NUMWORDS(sizeof(pexModelClipVolume2D)),
		lenofData, &pReq);
    if (pReq == NULL) return;

    pReq->modelClipOperator = operator; 
    pReq->numHalfSpaces = numHalfSpaces;

    PEXCopyWordsToOC( ocbuf, lenofData, halfSpaces);

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXRestoreModelClipVolume
 *
 * FORMAT:
 * 	`PEXRestoreModelClipVolume`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	restores the last-saved modeling clipping volume.
 *	If there is no last-saved clipping volume
 *	that can be restored, the modeling clipping volume is restored
 *	to its default state.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_RestoreModelClipVolume (ocbuf)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXSetModelClipFlag", `PEXModelClipVolume",
 *	`PEXModelClipVolume2D"
 *
 */

void
PEXRestoreModelClipVolume (ocbuf)

INPUT pxlOCBuf		*ocbuf;

{
/* AUTHOR: */
    PEXAddOC (ocbuf, PEXOCRestoreModelClip, 0, (char *)NULL);

    return;
}

/*+
 * NAME:
 * 	PEXSetViewIndex
 *
 * FORMAT:
 * 	`PEXSetViewIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		An index indicating the entry in the view
 *			table used to obtain viewing attributes.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the index value used to obtain view mapping, view
 *	orientation, and clipping attributes.
 *	Primitives rendered after this command
 *	use viewing attributes obtained from the view lookup
 *	table entry referenced by `index'.  The default view table entry
 *	is zero.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetViewIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlTableIndex' (defined in include
 *	file `PEXprotost.h') indicating the
 *	view table entry from which viewing attributes are to
 *	be obtained.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXCreateLookupTable"
 *
 */

void
PEXSetViewIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCViewIndex, sizeof( pexTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetLightSourceState
 *
 * FORMAT:
 * 	`PEXSetLightSourceState`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	enable		A list specifying the lights enabled.
 *
 *	numEnable	The number of lights in the enable list.
 *
 *	disable		A list specifying the lights disabled.
 *
 *	numDisable	The number of lights in the disable list.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the light source state in the rendering pipeline.
 *	Each element in the `enable' list activates the light represented by
 *	the corresponding light table entry and each element in the `disable'
 *	list deactivates the light represented by the corresponding light
 *	table entry.  Lights may not be in both the `enable' list and the
 *	`disable' list.
 *	Attempts to reference lights with indices greater than the number of
 *	lights supported in the PEX server extension's light
 *	lookup tables are ignored.
 *
 *	No equivalent C-language macro is provided for this routine.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer"
 *
 */

void
PEXSetLightSourceState (ocbuf, enable, numEnable, disable, numDisable)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlTableIndex	*enable;
INPUT int		numEnable;
INPUT pxlTableIndex	*disable;
INPUT int		numDisable;

{
/* AUTHOR: */
    pexLightState	*pReq;
    int			sizofEnableList = numEnable * sizeof( CARD16);
    int			sizofDisableList = numDisable * sizeof( CARD16);


    PEXInitOC( ocbuf, PEXOCLightState, NUMWORDS(sizeof(pexLightState)),
		NUMWORDS(sizofEnableList) + NUMWORDS(sizofDisableList), &pReq);
    if (pReq == NULL) return;

    pReq->numEnable = numEnable;
    pReq->numDisable = numDisable;

    PEXCopyBytesToOC( ocbuf, sizofEnableList, enable); 
    PEXCopyBytesToOC( ocbuf, sizofDisableList, disable); 

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXSetDepthCueIndex
 *
 * FORMAT:
 * 	`PEXSetDepthCueIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		An index indicating the entry in the depth cue
 *			table used to obtain depth-cueing
 *			attributes.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the index value used to obtain depth-cueing attributes.
 *	Primitives rendered after this command
 *	use depth-cueing attributes obtained from the depth cue lookup
 *	table entry referenced by `index'.  The default value of `index'
 *	is zero.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetDepthCueIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlTableIndex' (defined in include
 *	file `PEXprotost.h') indicating the
 *	depth cue table entry from which depth-cueing attributes are to
 *	be obtained.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXCreateLookupTable"
 *
 */

void
PEXSetDepthCueIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCDepthCueIndex, sizeof( pexTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetPickID
 *
 * FORMAT:
 * 	`PEXSetPickID`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	id		The value used as the pick identifier.
 *
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets the
 *	current pick identifier attribute to `id'.  The pick identifier is
 *	returned to the application through the use of the picking routines.
 *
 *	If the PEX server supports the Digital extensions, a structure's
 *	element pointer may be positioned relative to a pick identifier.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetPickID (ocbuf, id)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `id'
 *	is a pointer to a value of type `CARD32' (defined in include file
 *	`Xmd.h') indicating the value used as the pick identifier.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `MPEXSetElementPtrAtPickID"
 *
 */

void
PEXSetPickID (ocbuf, id)

INPUT pxlOCBuf		*ocbuf;
INPUT unsigned long	id;

{
/* AUTHOR: */
    PEXAddOC (ocbuf, PEXOCPickId, sizeof( CARD32), &id);

    return;
}

/*+
 * NAME:
 * 	PEXSetHlhsrID
 *
 * FORMAT:
 * 	`PEXSetHlhsrID`
 *
 * ARGUMENTS:
 *	ocbuf	The buffer to which the output command is added.
 *
 *	id	The value used as the hidden line / hidden
 *		surface identifier.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets the current
 *	HLHSR identifier attribute to `id'.  The HLHSR identifier has an
 *	implementation-dependent meaning.  Conceptually, this attribute is
 *	bound to all output primitives as they enter the rendering pipeline.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetHlhsrID (ocbuf, id)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `id' is
 *	a pointer to a value of type `CARD32' (defined in include file
 *	`Xmd.h') indicating the HLHSR identifier.
 *
 * ERRORS:
 *	Renderer 		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer"
 *
 */

void
PEXSetHlhsrID (ocbuf, id)

INPUT pxlOCBuf		*ocbuf;
INPUT unsigned long	id;

{
/* AUTHOR: */
    PEXAddOC (ocbuf, PEXOCHlhsrIdentifier, sizeof( CARD32), &id);

    return;
}

/*+
 * NAME:
 * 	PEXSetColourApproxIndex
 *
 * FORMAT:
 * 	`PEXSetColourApproxIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		An index indicating the entry in the colour approximation
 *			table used to obtain the colour approximation parameters.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the index value used to obtain the parameters used during the
 *	colour approximation stage of the rendering pipeline.
 *	Primitives rendered after this command use colour approximation
 *	parameters obtained from the colour approximation lookup
 *	table entry referenced by `index'.  The default colour approximation
 *	table entry is zero.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetColourApproxIndex (ocbuf, index)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `index'
 *	is a pointer to a value of type `pxlTableIndex' (defined in include
 *	file `PEXprotost.h') indicating the colour approximation
 *	table entry from which colour approximation parameters are to
 *	be obtained.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXCreateLookupTable"
 *
 */

void
PEXSetColourApproxIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR: */
    pexTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, PEXOCColourApproxIndex, sizeof( pexTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	PEXSetParaSurfCharacteristics
 *
 * FORMAT:
 * 	`PEXSetParaSurfCharacteristics`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *      pscType	 	The type of parametric surface characteristics to use 
 *
 *      pscData	        The data for the specified parametric surface 
 *			characteristics
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets the parametric
 *	surface characteristics to be used when rendering surfaces. 'pscType' 
 *	should be a parametric surface characteristic supported by the PEX 
 *	server extension.  (See `PEXGetEnumTypeInfo".)  
 *	Registered values are `PEXPSCNone', `PEXPSCImpDep', `PEXPSCIsoCurves',
 *	`PEXPSCMcLevelCurves', `PEXPSCWcLevelCurves'
 *	Include file `PEX.h' contains definitions of these constants.
 *	The default is `PEXPSCNone'.
 *
 *	The contents of 'pscData' depends on 'pscType'.  If 'pscType' is set 
 *	to `PEXPSCNone' or  `PEXPSCImpDep' there is no data so 'pscData' 
 * 	should be NULL.  If 'pscType' is set to `PEXPSCIsoCurves' then 
 *	'pscData' should point to a value of type `pxlPSC_IsoparametricCurves'.  *	If 'pscType' is set to `PEXPSCMcLevelCurves' or `PEXPSCWcLevelCurves' 
 *	then 'pscData' should point to a value of type `pxlPSC_LevelCurves'.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetParaSurfCharacteristics(ocbuf, pstType, pscData)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer, `model'
 *	is a pointer to a value of type `pxlEnumTypeIndex' (defined in
 *	include file `PEXprotost.h') containing the
 *	rendering colour model to use 
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXGetEnumTypeInfo" 
 *
 */

void
PEXSetParaSurfCharacteristics (ocbuf, pscType, pscData)

INPUT pxlOCBuf		*ocbuf;
INPUT int		pscType;
INPUT char		*pscData;

{
/* AUTHOR: */
    pexParaSurfCharacteristics 	*pReq;
    int                 	lenofData;

    lenofData = 0; 
    if (pscType == PEXPSCIsoCurves)
    {
	lenofData = NUMWORDS(sizeof(pxlPSC_IsoparametricCurves));
    }
    else if (pscType == PEXPSCMcLevelCurves || pscType == PEXPSCWcLevelCurves)
    {
        int nInt = (int) ((pxlPSC_LevelCurves *)pscData)->numberIntersections;
	lenofData = 
	    NUMWORDS( sizeof(pxlPSC_LevelCurves) + (nInt*sizeof(pxlCoord3D)) );
    }

    PEXInitOC( ocbuf, PEXOCParaSurfCharacteristics,
                NUMWORDS(sizeof(pexParaSurfCharacteristics)),
                lenofData, &pReq);
    if (pReq == NULL) return;

    pReq->characteristics = pscType;
    pReq->length = NUMBYTES(lenofData);

    if (lenofData > 0) PEXCopyWordsToOC( ocbuf, lenofData, pscData );

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	PEXSetRenderingColourModel
 *
 * FORMAT:
 * 	`PEXSetRenderingColourModel`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	model		The rendering colour model.  
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command sets the rendering 
 *	colour model.  The rendering colour model defines the colour model 
 * 	to be used for colour interpolation within the rendering pipeline.  
 *	Reflectance equations should have the appearance of being performed 
 * 	in the colour space specified by the rendering colour model.
 *	`model' should be a rendering colour model supported by the PEX 
 *	server extension.  (See `PEXGetEnumTypeInfo".)  
 *	Registered values are `PEXRdrColourModelImpDep', `PEXRdrColourModelRGB',
 *	`PEXRdrColourModelCIE', `PEXRdrColourModelHSV', `PEXRdrColourModelHLS'
 *	Include file `PEX.h' contains definitions of these constants.
 *	The default is `PEXRdrColourModelImpDep'.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `PEXoc.h':
 *
 *	==
 *	PEX_SetRenderingColourModel(ocbuf, model)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer and `model'
 *	is a pointer to a value of type `pxlEnumTypeIndex' (defined in
 *	include file `PEXprotost.h') containing the
 *	rendering colour model to use 
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXGetEnumTypeInfo" 
 *
 */

void
PEXSetRenderingColourModel (ocbuf, model)

INPUT pxlOCBuf		*ocbuf;
INPUT int		model;

{
/* AUTHOR: */
    pexEnumTypeIndex	m;

    m = model;
    PEXAddOC (ocbuf, PEXOCRenderingColourModel, sizeof( pexEnumTypeIndex), &m);

    return;
}

/*+
 * NAME:
 * 	PEXAddToNameSet
 *
 * FORMAT:
 * 	`PEXAddToNameSet`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	names		The list of names added to the current name set.
 *
 *	numNames	The number of names in `names'
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command adds the specified
 *	list of names to the current name set.  When output primitives are
 *	rendered, the values in the current name set are compared to the
 *	values in the current highlighting and invisibility name set resources
 *	to determine whether the primitive should be highlit
 *	or treated as invisible.
 *
 *	No equivalent C-language macro is provided for this routine.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXCreateNameSet", `PEXRemoveFromNameSet"
 *
 */

void
PEXAddToNameSet (ocbuf, names, numNames)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlName		*names;
INPUT int		numNames;

{
/* AUTHOR: */
    PEXAddListOC( ocbuf, PEXOCAddToNameSet, False/* count needed*/, numNames,
	    sizeof( pxlName), names);

    return;
}

/*+
 * NAME:
 * 	PEXRemoveFromNameSet
 *
 * FORMAT:
 * 	`PEXRemoveFromNameSet`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	names		The list of names removed from the current
 *			name set.
 *
 *	numNames	The number of names in `names'
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command removes the specified
 *	list of names from the current name set.  When output primitives are
 *	rendered, the values in the current name set are compared to the
 *	values in the current highlighting and invisibility name set resources
 *	to determine whether the primitive should be highlit
 *	or treated as invisible.
 *
 *	No equivalent C-language macro is provided for this routine.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateOCBuffer", `PEXCreateNameSet", `PEXRemoveFromNameSet"
 *
 */

void
PEXRemoveFromNameSet (ocbuf, names, numNames)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlName		*names;
INPUT int		numNames;

{
/* AUTHOR: */
    PEXAddListOC( ocbuf, PEXOCRemoveFromNameSet, False/* count needed*/, 
	numNames, sizeof( pxlName), names);

    return;
}
