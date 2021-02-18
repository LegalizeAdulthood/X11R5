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


/* Definitions for Digital Pick/Echo/Z PEX extension likely to be used by
applications */

#ifndef MPEX_H
#define MPEX_H

#define MPEX_REQ	0x00000080 /* reqType is CARD8 */
#define MPEX_EVENT	0x00000000 /* offset from base event num from XInitExt */
#define MPEX_OC		0x00008000 /* elementType is CARD16 */
#define MPEX_ENUM	0x00008000 /* enum types are CARD16 */
#define MPEX_TABLE	0x00008000 /* pexTableType is CARD16 */

/*** Enumerated Types ***/
#define MPEXETHighlightMethod	(MPEX_ENUM+0)
#define MPEXETEchoMethod		(MPEX_ENUM+1)
#define MPEXETOverlapMode		(MPEX_ENUM+2)
#define MPEXETLogicalOp		(MPEX_ENUM+3)

#define FirstMPEXEnumType	(MPEX_ENUM+0)
#define LastMPEXEnumType	(MPEX_ENUM+3)

/*** MPEXHighlightLUT and Echo Methods ***/
#define MPEXChangeAllColors		0
#define MPEXChangeEdgeColor		1
#define MPEXChangeInteriorColor	2

/*** additional Hlhsr mode ***/
#define MPEXHlhsrZBufferId		-2

/*** Overlap Mode Values ***/
#define MPEXAnyPrim		0
#define MPEXFirstPrim	1
#define MPEXLastPrim	2

/*** Logical Operations ***/
#define MPEXOpClear		0x0
#define MPEXOpAnd		0x1
#define MPEXOpAndReverse	0x2
#define MPEXOpCopy		0x3
#define MPEXOpAndInverted	0x4
#define MPEXOpNoop		0x5
#define MPEXOpXor		0x6
#define MPEXOpOr		0x7
#define MPEXOpNor		0x8
#define MPEXOpEquiv		0x9
#define MPEXOpInvert	0xA
#define MPEXOpOrReverse	0xB
#define MPEXOpCopyInverted	0xC
#define MPEXOpOrInverted	0xD
#define MPEXOpNand		0xE
#define MPEXOpSet		0xF

/*** Lookup Table Types ***/
#define MPEXHighlightLUT	(MPEX_TABLE+0)
#define MPEXEchoLUT	(MPEX_TABLE+1)
#define MPEXPixmapLUT	(MPEX_TABLE+2)

#define maxMPEXTableType	(MPEX_TABLE+3)
#define FirstMPEXTableType	(MPEX_TABLE+0)
#define LastMPEXTableType	(MPEX_TABLE+2)

/*** Renderer State ***/
#define MPEXPicking			2
#define	MPEXTransparencyOpaque	3
#define	MPEXTransparency		4

/*** Which Element to Pick ***/
#define MPEXPickClosest	0
#define MPEXPickLast	1

/*** Values for "more picks" ***/
#define MPEXNoMorePicks		0
#define MPEXMorePicks		1
#define MPEXMaybeMorePicks		2

/*** user-settable Dash Patterns ***/
#define MPEXDashPattern1	-1
#define MPEXDashPattern2	-2
#define MPEXDashPattern3	-3
#define MPEXDashPattern4	-4
#define MPEXDashPattern5	-5
#define MPEXDashPattern6	-6
#define MPEXDashPattern7	-7
#define MPEXDashPattern8	-8
#define MPEXDashPattern9	-9
#define MPEXDashPattern10	-10
#define MPEXDashPattern11	-11
#define MPEXDashPattern12	-12
#define MPEXDashPattern13	-13
#define MPEXDashPattern14	-14
#define MPEXDashPattern15	-15
#define MPEXDashPattern16	-16

/*** user-settable Marker Glyphs ***/
#define MPEXMarkerGlyph1	-1
#define MPEXMarkerGlyph2	-2
#define MPEXMarkerGlyph3	-3
#define MPEXMarkerGlyph4	-4
#define MPEXMarkerGlyph5	-5
#define MPEXMarkerGlyph6	-6
#define MPEXMarkerGlyph7	-7
#define MPEXMarkerGlyph8	-8
#define MPEXMarkerGlyph9	-9
#define MPEXMarkerGlyph10	-10
#define MPEXMarkerGlyph11	-11
#define MPEXMarkerGlyph12	-12
#define MPEXMarkerGlyph13	-13
#define MPEXMarkerGlyph14	-14
#define MPEXMarkerGlyph15	-15
#define MPEXMarkerGlyph16	-16

/*** Bit Numbers for Change/GetNewRendererAttributes ***/
#define MPEXNRALogicalOpBit		0
#define MPEXNRAPlaneMaskBit		1
#define MPEXNRABackgroundPixelBit	2
#define MPEXNRAClearIBit		3
#define MPEXNRAClearZBit		4
#define MPEXNRAOverlapModeBit	5
#define MPEXNRAAaSwitchBit		6
#define MPEXNRAHighlightTableBit	7
#define MPEXNRAEchoTableBit		8
#define MPEXNRAEchoSwitchBit	9
#define MPEXNRAPickPositionBit	10
#define MPEXNRAPickDistanceBit	11
#define MPEXNRAPickInclusionBit	12
#define MPEXNRAPickExclusionBit	13
#define MPEXNRAPickMaxHitsBit	14
#define MPEXNRAPickStartPathBit	15
#define MPEXNRACullSwitchBit	16
#define MPEXNRAPixmapTableBit	17

#define MaxNRAShift	18
#define FirstNRABit	0
#define LastNRABit	17

/*** Bit masks for Change/GetNewRendererAttributes ***/
#define MPEXNRALogicalOp		(1L<<MPEXNRALogicalOpBit)
#define MPEXNRAPlaneMask		(1L<<MPEXNRAPlaneMaskBit)
#define MPEXNRABackgroundPixel	(1L<<MPEXNRABackgroundPixelBit)
#define MPEXNRAClearI		(1L<<MPEXNRAClearIBit)
#define MPEXNRAClearZ		(1L<<MPEXNRAClearZBit)
#define MPEXNRAOverlapMode		(1L<<MPEXNRAOverlapModeBit)
#define MPEXNRAAaSwitch		(1L<<MPEXNRAAaSwitchBit)
#define MPEXNRAHighlightTable	(1L<<MPEXNRAHighlightTableBit)
#define MPEXNRAEchoTable		(1L<<MPEXNRAEchoTableBit)
#define MPEXNRAEchoSwitch		(1L<<MPEXNRAEchoSwitchBit)
#define MPEXNRAPickPosition		(1L<<MPEXNRAPickPositionBit)
#define MPEXNRAPickDistance		(1L<<MPEXNRAPickDistanceBit)
#define MPEXNRAPickInclusion	(1L<<MPEXNRAPickInclusionBit)
#define MPEXNRAPickExclusion	(1L<<MPEXNRAPickExclusionBit)
#define MPEXNRAPickMaxHits		(1L<<MPEXNRAPickMaxHitsBit)
#define MPEXNRAPickStartPath	(1L<<MPEXNRAPickStartPathBit)
#define MPEXNRACullSwitch		(1L<<MPEXNRACullSwitchBit)
#define MPEXNRAPixmapTable		(1L<<MPEXNRAPixmapTableBit)

/*** Output Command Opcodes ***/
#define MPEXOCHighlightIndex		(MPEX_OC+0)
#define MPEXOCEchoIndex			(MPEX_OC+1)
#define MPEXOCNoop			(MPEX_OC+2)
#define MPEXOCAnnoPixmap		(MPEX_OC+3)
#define MPEXOCCircle			(MPEX_OC+4)
#define MPEXOCArc			(MPEX_OC+5)
#define MPEXOCIndexedPolygons		(MPEX_OC+6)
#define MPEXOCEllipticalArc2D		(MPEX_OC+7)
#define MPEXOCEllipticalArc3D		(MPEX_OC+8)
#define MPEXOCEllipse2D			(MPEX_OC+9)
#define MPEXOCEllipse3D			(MPEX_OC+10)
#define MPEXOCGridRectangular           (MPEX_OC+11)
#define MPEXOCGridRadial                (MPEX_OC+12)

#define MaxMPEXOC			(MPEX_OC+13)
#define FirstMPEXOC			(MPEX_OC+0)
#define LastMPEXOC			(MPEX_OC+12)

/*** Events ***/
#define MPEXMaxHitsReached	        (MPEX_EVENT+0)

/*** Requests ***/
#define MPEX_ChangeNewRenderer		(MPEX_REQ+0)
#define MPEX_GetNewRendererAttributes	(MPEX_REQ+1)
#define MPEX_RenderElements		(MPEX_REQ+2)
#define MPEX_AccumulateState		(MPEX_REQ+3)
#define MPEX_BeginPickOne		(MPEX_REQ+4)
#define MPEX_EndPickOne			(MPEX_REQ+5)
#define MPEX_PickOne			(MPEX_REQ+6)
#define MPEX_BeginPickAll		(MPEX_REQ+7)
#define MPEX_EndPickAll			(MPEX_REQ+8)
#define MPEX_PickAll			(MPEX_REQ+9)
#define MPEX_SetElementPointerAtPickId	(MPEX_REQ+10)
#define MPEX_SetDashPattern		(MPEX_REQ+11)
#define MPEX_GetDashPattern		(MPEX_REQ+12)
#define MPEX_SetMarkerGlyph		(MPEX_REQ+13)
#define MPEX_GetMarkerGlyph		(MPEX_REQ+14)
#define MPEX_LargeRequest		(MPEX_REQ+15)
#define MPEX_BeginTransparencyRendering	(MPEX_REQ+16)
#define MPEX_EndTransparencyRendering	(MPEX_REQ+17)

#define MaxMPEXCommand		(MPEX_REQ+18)
#define FirstMPEXCommand	(MPEX_REQ+0)
#define LastMPEXCommand		(MPEX_REQ+17)

/* dummied in because Pete doesn't know any better (used in pl_startup.c) */

#define	PEXMinError	0

#define Texture Information */

#define MPEXGATexture    0x0008
#define MPEXInteriorStyleTexture 6
 
#endif /* MPEX_H */
