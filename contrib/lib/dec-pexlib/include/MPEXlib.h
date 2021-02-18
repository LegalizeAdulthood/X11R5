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


#ifndef MPEXLIB_H
#define MPEXLIB_H

#ifndef MPEX_H
#  include "MPEX.h"
#endif
#ifndef MPEXPROTO_H
#  include "MPEXproto.h"
#endif
#ifndef MPEXPROTOSTR_H
#  include "MPEXprotostr.h"
#endif

/*
 * Following are the typedefs used in the PEXlib interface. mpxlFooBar
 * definitions are always supersets of the mpexFooBar protocol definitions.
 * Note that most mpxlFooBar definitions are equivalent to the mpexFooBar
 * protocol definitions.  However, some of the definitions do have extra
 * fields. See documentation in PEXlib.h for more details. 
 */

/*
 *  Definitions for output commands
 */
typedef mpexAnnoPixmap                  mpxlAnnoPixmap;
typedef mpexArc                         mpxlArc;
typedef mpexCircle                      mpxlCircle;
typedef mpexIndexedPolygon              mpxlIndexedPolygon;
typedef mpexEllipticalArc2D		mpxlEllipticalArc2D;
typedef mpexEllipticalArc3D		mpxlEllipticalArc3D;
typedef mpexEllipse2D			mpxlEllipse2D;
typedef mpexEllipse3D			mpxlEllipse3D;
typedef mpexGridRectangular             mpxlGridRectangular;
typedef mpexGridRadial                  mpxlGridRadial;
typedef mpexEchoIndex                   mpxlEchoIndex;
typedef mpexHighlightIndex              mpxlHighlightIndex;
typedef mpexNoop                        mpxlNoop;

/*
 *  Definitions for lookup table entries
 */
typedef struct
{
    pxlEnumTypeIndex    method B16;
    CARD16              pad B16;
    pxlColourSpecifier  colour;
} mpxlHighlightEntry;

typedef struct
{
    pxlEnumTypeIndex    method B16;
    CARD16              pad B16;
    pxlColourSpecifier  colour;
} mpxlEchoEntry;

typedef mpexPixmapEntry mpxlPixmapEntry;

/* 
 * pick path returned by pick all 
 */
typedef struct mpxlPickAllPath
{
    int                 pathLength;
    pxlPickPath         *elementRefs;
} mpxlPickAllPath;

/* 
 * Definition for MPEX Renderer Attributes.  Defines are in MPEX.h. 
 */

typedef struct mpxlRendererAttributes
{
    short		logicalOp;
    short		overlapMode;
    unsigned long	planeMask;
    unsigned long	backgroundPixel;
    unsigned char	clearI;
    unsigned char	clearZ;
    unsigned char	aaSwitch;
    unsigned char	echoSwitch;
    unsigned char	cullSwitch;
    pxlLookupTable	highlightTable;
    pxlLookupTable	echoTable;
    pxlLookupTable	pixmapTable;
    pxlDeviceCoord2D	pickPosition;
    float		pickDistance;
    pxlNameSet		pickInclusion;
    pxlNameSet		pickExclusion;
    unsigned long	pickMaxHits;
    pxlStructurePath	pickStartPath;
} mpxlRendererAttributes;


/* Event Structures */

/* The first several items in each event structure must be as they are or
   toolkits will break */

typedef struct mpxlMaxHitsReachedEvent
{
    int 		type;		/* type of event, MPEXMaxHitsReached */
    unsigned long	serial;		/* # of last req processed by server */
    Bool		send_event;	/* True if came from SendEvent req */
    Display		*display;	/* Display the event was read from */
    pxlRenderer		rdr;		/* renderer doing the pick */
} mpxlMaxHitsReachedEvent;

#endif /* MPEXLIB_H */
