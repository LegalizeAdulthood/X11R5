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


/* Definitions for Digital Pick/PEXEcho/Z PEX extension used by the server and
C bindings */

#ifndef MPEXPROTO_H
#define MPEXPROTO_H

/****************************************************************
 * 		REPLIES						*
 ****************************************************************/

typedef struct
{
    BYTE		type;	/* X_Reply */
    CARD8		what;
    CARD16		sequenceNumber B16;
    CARD32		length B32;	/* not 0 */
    BYTE		pad[24];
    /* SINGLE NewRendererAttributes(itemMask) */
} mpexGetNewRendererAttributesReply;

typedef struct
{
    BYTE		type;	/* X_Reply */
    CARD8		what;
    CARD16		sequenceNumber B16;
    CARD32		length B32;	/* not 0 */
    CARD32		numRefs B32;
    BYTE		pad[20];
    /* LISTof PickElementRef(numRefs) */
} mpexEndPickOneReply;

typedef mpexEndPickOneReply mpexPickOneReply;

typedef struct
{
    BYTE		type;	/* X_Reply */
    CARD8		what;
    CARD16		sequenceNumber B16;
    CARD32		length B32;	/* not 0 */
    CARD32		numPicks B32;
    CARD8		morePicks;
    BYTE		pad[19];
    /* LISTof CLISTof PickElementRef(numPicks) */
} mpexEndPickAllReply;

typedef mpexEndPickAllReply mpexPickAllReply;

typedef struct
{
    BYTE		type;	/* X_Reply */
    CARD8		what;
    CARD16		sequenceNumber B16;
    CARD32		length B32;	/* 0 */
    CARD32		pattern B32;
    BYTE		pad[20];
} mpexGetDashPatternReply;

typedef struct
{
    BYTE		type;	/* X_Reply */
    CARD8		what;
    CARD16		sequenceNumber B16;
    CARD32		length B32;	/* not 0 */
    INT16		xOffset B16;
    INT16		yOffset B16;
    BYTE		pad[20];
    CARD16		glyph[16];
} mpexGetMarkerGlyphReply;


typedef struct
{
    BYTE		type;	/* X_Reply */
    CARD8		what;
    CARD16		sequenceNumber B16;
    CARD32		length B32;	/* 0 */
    BYTE		requireAnotherPass;
    BYTE		pad[23];
} mpexEndTransparencyRenderingReply;


/****************************************************************
 *		EVENTS						*
 ****************************************************************/

typedef struct
{
    BYTE		type;
    BYTE		detail;
    CARD16		sequenceNumber B16;
    pexRenderer		rdr B32;
    BYTE		pad[24];
} mpexMaxHitsReachedEvent;

/****************************************************************
 *		REQUESTS					*
 ****************************************************************/

typedef struct
{
    CARD8		reqType;
    CARD8		opcode;
    CARD16		length B16;
    pexEnumTypeIndex	fpFormat B16;
    CARD16		pad B16;
    pexRenderer		rdr B32;
    pexBitmask		itemMask B32;
    /* SINGLE NewRendererAttributes(itemMask) */
} mpexChangeNewRendererReq;

typedef struct
{
    CARD8		reqType;
    CARD8		opcode;
    CARD16		length B16;
    pexEnumTypeIndex	fpFormat B16;
    CARD16		pad B16;
    pexRenderer		rdr B32;
    pexBitmask		itemMask B32;
} mpexGetNewRendererAttributesReq;

typedef struct
{
    CARD8		reqType;
    CARD8		opcode;
    CARD16		length B16;
    pexRenderer		rdr B32;
    pexStructure	sid B32;
    pexElementRange	range;
} mpexRenderElementsReq;

typedef struct
{
    CARD8		reqType;
    CARD8		opcode;
    CARD16		length B16;
    pexRenderer		rdr B32;
    CARD32		numRefs B32;
    /* LISTof ElementRef(numRefs) */
} mpexAccumulateStateReq;

typedef struct
{
    CARD8		reqType;
    CARD8		opcode;
    CARD16		length B16;
    pexRenderer		rdr B32;
    Drawable		drawable B32;
    CARD32		id B32;
    CARD8		which;
    BYTE		pad[3];
} mpexBeginPickOneReq;

typedef pexResourceReq mpexEndPickOneReq;

typedef struct
{
    CARD8		reqType;
    CARD8		opcode;
    CARD16		length B16;
    pexRenderer		rdr B32;
    Drawable		drawable B32;
    pexStructure	sid B32;
    CARD8		which;
    BYTE		pad[3];
} mpexPickOneReq;

typedef struct
{
    CARD8		reqType;
    CARD8		opcode;
    CARD16		length B16;
    pexRenderer		rdr B32;
    Drawable		drawable B32;
    unsigned long	id B32;
    CARD8		sendEvent;
    BYTE		pad[3];
} mpexBeginPickAllReq;

typedef pexResourceReq mpexEndPickAllReq;

typedef struct
{
    CARD8		reqType;
    CARD8		opcode;
    CARD16		length B16;
    pexRenderer		rdr B32;
    Drawable		drawable B32;
} mpexPickAllReq;

typedef struct
{
    CARD8		reqType;
    CARD8		opcode;
    CARD16		length B16;
    pexStructure	str B32;
    CARD32		pickid B32;
    INT32		offset B32;
} mpexSetElementPointerAtPickIdReq;

typedef struct
{
    CARD8		reqType;
    CARD8		opcode;
    CARD16		length B16;
    pexRenderer		rdr B32;
    INT16		patternNum B16;
    CARD16		pad B16;
    CARD32		pattern B32;
} mpexSetDashPatternReq;

typedef struct
{
    CARD8		reqType;
    CARD8		opcode;
    CARD16		length B16;
    pexRenderer		rdr B32;
    INT16		patternNum B16;
    CARD16		pad B16;
} mpexGetDashPatternReq;

typedef struct
{
    CARD8		reqType;
    CARD8		opcode;
    CARD16		length B16;
    pexRenderer		rdr B32;
    INT16		glyphNum B16;
    CARD16		pad B16;
    INT16		xOffset B16;
    INT16		yOffset B16;
    CARD16		glyph[16];
} mpexSetMarkerGlyphReq;

typedef struct
{
    CARD8		reqType;
    CARD8		opcode;
    CARD16		length B16;
    pexRenderer		rdr B32;
    INT16		glyphNum B16;
    CARD16		pad B16;
} mpexGetMarkerGlyphReq;

typedef struct
{
    CARD8		reqType;
    CARD8		opcode;
    CARD16		length B16;
    CARD16		sequenceNum B16;
    CARD16		totalNum B16;
    CARD32		totalLength B32;
} mpexLargeRequestReq;

typedef struct
{
    CARD8	reqType;
    CARD8	opcode;
    CARD16	length B16;
    pexRenderer	rdr B32;
    Drawable	drawable B32;
    CARD8	firstPass;
    BYTE	pad[3];
} mpexBeginTransparencyRenderingReq;

typedef struct
{
    CARD8	reqType;
    CARD8	opcode;
    CARD16	length B16;
    pexRenderer rdr B32;
    pexSwitch	flushFlag;
    BYTE	pad[3];
} mpexEndTransparencyRenderingReq;

/*****************************************************************
 * Output Commands
 *****************************************************************/

typedef struct
{
    pexElementInfo	head;
    pexCoord3D		origin;
    pexTableIndex	index B16;
    CARD16		pad B16;
} mpexAnnoPixmap;

typedef struct
{
    pexElementInfo	head;
    pexCoord3D		center;
    FLOAT		radius B32;
    FLOAT		startAngle B32;
    FLOAT		endAngle B32;
} mpexArc;

typedef struct
{
    pexElementInfo	head;
    pexCoord3D		center;
    FLOAT		radius B32;
} mpexCircle;


typedef struct
{
    pexElementInfo	head;
    pexCoord2D		center;
    pexCoord2D		majorAxis;
    pexCoord2D		minorAxis;
    FLOAT		startAngle B32;
    FLOAT		endAngle B32;
} mpexEllipticalArc2D;

typedef struct
{
    pexElementInfo	head;
    pexCoord3D		center;
    pexCoord3D		majorAxis;
    pexCoord3D		minorAxis;
    FLOAT		startAngle B32;
    FLOAT		endAngle B32;
} mpexEllipticalArc3D;

typedef struct
{
    pexElementInfo	head;
    pexCoord2D		center;
    pexCoord2D		majorAxis;
    pexCoord2D		minorAxis;
} mpexEllipse2D;

typedef struct
{
    pexElementInfo	head;
    pexCoord3D		center;
    pexCoord3D		majorAxis;
    pexCoord3D		minorAxis;
} mpexEllipse3D;

typedef struct
{
    pexElementInfo	head;
    CARD16		shape B16;
    pexColourType	colourType B16;
    pexBitmaskShort	facetAttribs B16;
    pexBitmaskShort	vertexAttribs B16;
    CARD16		edgeAttribs B16;
    CARD16		numFacets B16;
    CARD16		numEdges B16;
    CARD16		numVertices B16;
    /*  LISTof CARD16(numFacets) */
    /*  LISTof OptData(numFacets, facetAttribs, colourType) */
    /*  LISTof VertexIndex(numEdges, edgeAttribs) */
    /*  LISTof Vertex(numVertices, vertexAttribs, colourType) */
} mpexIndexedPolygon;

typedef struct 
{
    pexElementInfo       head;
    pexCoord3D           origin;
    FLOAT                xdist B32;
    FLOAT                ydist B32;
} mpexGridRectangular;

typedef struct
{
    pexElementInfo      head;
    pexCoord3D          center;
    FLOAT		startAngle B32;
    FLOAT		endAngle B32;
    FLOAT               angDist B32;
    FLOAT               radDist B32;
} mpexGridRadial;

typedef struct
{
    pexElementInfo	head;
    pexTableIndex	index B16;
    CARD16		pad B16;
} mpexEchoIndex;

typedef struct
{
    pexElementInfo	head;
    pexTableIndex	index B16;
    CARD16		pad B16;
} mpexHighlightIndex;

typedef struct
{
    pexElementInfo 	head;
} mpexNoop;

#endif /* MPEXPROTO_H */
