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
**	pl_mpex.c
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
**	This file contains the Digital Pick/PEXEcho/Z extension routines.
**
**  Keywords:
**
**	< to be supplied >
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
**	30-Sep-89 
**
**--
*/

/*
 *   Table of Contents (routine names in order of appearance)
 */

/*
 * E   MPEXChangeNewRenderer
 * E   MPEXGetNewRendererAttributes
 * E   MPEXRenderElements
 * E   MPEXAccumulateState
 * E   MPEXBeginPickOne
 * E   MPEXEndPickOne
 * E   MPEXPickOne
 * E   MPEXBeginPickAll
 * E   MPEXEndPickAll
 * E   MPEXPickAll
 * E   MPEXSetHighlightIndex
 * E   MPEXSetEchoIndex
 * E   MPEXSetElementPtrAtPickID
 * I   _MPEXConvertMaxHitsEvent
 * E   MPEXNoop
 * E   MPEXCircle
 * E   MPEXArc
 * E   MPEXEllipticalArc2D
 * E   MPEXEllipticalArc3D
 * E   MPEXEllipse2D
 * E   MPEXEllipse3D
 * E   MPEXGridRectangular
 * E   MPEXGridRadial
 * E   MPEXAnnotationPixmap
 * E   MPEXSetDashPattern
 * E   MPEXGetDashPattern
 * E   MPEXSetMarkerGlyph
 * E   MPEXGetMarkerGlyph
 */

/*
 *   Include Files
 */

/*
 *  NEED EVENTS needs to be defined when including Xproto.h so xEvent can be
 *  sucked in.  Turn it on here in case Xproto.h is grabbed earlier
 */ 
#define NEED_EVENTS
#include "pex_features.h"
#include "pex_macros.h"

#include <X11/Xlib.h>      /* for XEvent */

#include <X11/Xproto.h>    /* for xEvent */
#undef NEED_EVENTS

#include "PEXlib.h"
#include "PEXlibint.h"
#include "PEXocbuf.h"
#include "MPEX.h"
#include "MPEXprotostr.h"
#include "MPEXproto.h"
#include "MPEXlib.h"

#include "pl_ref_data.h"

extern	char		*_XAllocScratch();

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
#define GetFacetDataLength(_fattribs_,_lenofColour_) \
    ( ((_fattribs_ & PEXGAColour) ? _lenofColour_ : 0) + \
      ((_fattribs_ & PEXGANormal) ? LENOF( pexVector3D) : 0) + \
      ((_fattribs_ & MPEXGATexture) ? LENOF( pexCoord2D) : 0))

/*
 * compute the number of words in a vertex with optional colours and normals
 */
#define GetVertexWithDataLength(_vattribs_,_lenofColour_)\
    ( LENOF( pexCoord3D) + \
      ((_vattribs_ & PEXGAColour) ? _lenofColour_ : 0) + \
      ((_vattribs_ & PEXGANormal) ? LENOF( pexVector3D) : 0) + \
      ((_vattribs_ & MPEXGATexture) ? LENOF( pexCoord2D) : 0))


/*+
 * NAME:
 * 	MPEXChangeNewRenderer
 *
 * FORMAT:
 * 	`MPEXChangeNewRenderer`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer to be changed.
 *
 *	valueMask	A mask indicating the new renderer attributes
 *			to be changed.
 *
 *	values		A pointer to a structure containing
 *			new values for the specified new renderer attributes.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine modifies Digital extended attributes of the specified
 *	`renderer' resource.
 *	Each bit in the bitmask array `valueMask' indicates whether
 *	the corresponding entry in `values' is to be used to modify
 * 	the renderer resource.  If a bit is set, the attribute in the
 *	renderer is set to the value specified in `values'.  Include file
 *	`MPEX.h' contains a list of constants used to create the bitmask,
 *	for example `MPEXNRALogicalOp' and `MPEXNRAPickPosition'.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Match			Specified lookup table resource was not
 *				created for drawables of the same root and
 *				depth as the specified renderer.
 *
 *	Value			An item in the item list is out of range.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	NameSet			A specified name set resource is invalid.
 *
 *	LookupTable		A specified lookup table resource is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateRenderer", `PEXFreeRenderer", `PEXGetRendererAttributes",
 *	`PEXGetRendererDynamics", `PEXBeginRendering", `PEXEndRendering",
 *	`PEXChangeRenderer", `MPEXGetNewRendererAttributes"
 *
 */

void
MPEXChangeNewRenderer (display, renderer, valueMask, values)

INPUT Display			*display;
INPUT pxlRenderer		renderer;
INPUT unsigned long		valueMask;
INPUT mpxlRendererAttributes 	*values;

{
/* AUTHOR:  Sally C. Barry */
    mpexChangeNewRendererReq	*req;
    CARD32			*pv;
    CARD32			*pvSend;
    unsigned long		f;
    int				i, j;
    int				n;
    int				length;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    MPEXGetReq (ChangeNewRenderer, req);
    req->fpFormat = pexFpFormat;
    req->rdr = renderer;
    req->itemMask = valueMask;

    CountOnes (valueMask, n);

    f =  n * sizeof(CARD32);

    if (valueMask & MPEXNRAPickStartPath)
    {
	f += values->pickStartPath.pathLength * sizeof(pexElementRef);
    }

    pv = pvSend = (CARD32 *)_XAllocScratch (display, (unsigned long)f);

    for (i=0; i<MaxNRAShift; i++)
    {
	f = (1L << i);
	if (valueMask & f)
	{
            switch (f)
	    {
	    case MPEXNRALogicalOp:
		*((INT16 *)pv) = values->logicalOp;
		pv++;
		break;
	    case MPEXNRAPlaneMask:
		*((CARD32 *)pv) = values->planeMask;
		pv++;
		break;
	    case MPEXNRABackgroundPixel:
		*((CARD32 *)pv) = values->backgroundPixel;
		pv++;
		break;
	    case MPEXNRAClearI:
		*((CARD8 *)pv) = values->clearI;
		pv++;
		break;
	    case MPEXNRAClearZ:
		*((CARD8 *)pv) = values->clearZ;
		pv++;
		break;
	    case MPEXNRAOverlapMode:
		*((INT16 *)pv) = values->overlapMode;
		pv++;
		break;
	    case MPEXNRAAaSwitch:
		*((CARD8 *)pv) = values->aaSwitch;
		pv++;
		break;
	    case MPEXNRAHighlightTable:
		*((pexLookupTable *)pv) = values->highlightTable;
		pv++;
		break;
	    case MPEXNRAEchoTable:
		*((pexLookupTable *)pv) = values->echoTable;
		pv++;
		break;
	    case MPEXNRAEchoSwitch:
		*((CARD8 *)pv) = values->echoSwitch;
		pv++;
		break;
	    case MPEXNRAPickPosition:
		*((pexDeviceCoord2D *)pv) = values->pickPosition;
		pv = (unsigned long *)((char *)pv + sizeof(pexDeviceCoord2D));
		break;
	    case MPEXNRAPickDistance:
		*((FLOAT *)pv) = values->pickDistance;
		pv++;
		break;
	    case MPEXNRAPickInclusion:
		*((pexNameSet *)pv) = values->pickInclusion;
		pv++;
		break;
	    case MPEXNRAPickExclusion:
		*((pexNameSet *)pv) = values->pickExclusion;
		pv++;
		break;
	    case MPEXNRAPickMaxHits:
		*((CARD32 *)pv) = values->pickMaxHits;
		pv++;
		break;
	    case MPEXNRAPickStartPath:
		length = values->pickStartPath.pathLength;
		*((long *)pv) = length;
		pv++;
		length *= sizeof(pexElementRef);
		COPY_AREA (values->pickStartPath.elementRefs, pv, length);
		pv = (CARD32 *)((char *)pv + length);
		break;
	    case MPEXNRACullSwitch:
		*((CARD8 *)pv) = values->cullSwitch;
		pv++;
		break;
	    case MPEXNRAPixmapTable:
		*((pexLookupTable *)pv) = values->pixmapTable;
		pv++;
		break;
	    }
	}
    }

    length = pv - pvSend;
    req->length += length;
    /* put the data in the X buffer */
    Data (display, (char *)pvSend, (unsigned)(length << 2));

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	MPEXGetNewRendererAttributes
 *
 * FORMAT:
 * 	`MPEXGetNewRendererAttributes`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer to be queried.
 *
 *	valueMask	A bitmask indicating new attributes to be
 *			returned from the renderer.
 *
 *	valuesReturn	Returns a pointer to a structure that contains the
 *			specified attributes from the renderer.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine returns Digital extended attributes from the specified
 *	`renderer' resource.
 *	Each bit in the bitmask array `valueMask' indicates whether
 *	the corresponding entry is to be returned in the structure pointed
 *	to by `valuesReturn'.  If a bit is set, the value in the renderer
 *	is returned in `valuesReturn'.  Include file `MPEX.h'
 *	contains a list of constants used to create the bitmask.
 *
 *	PEXlib allocates the storage for the returned new renderer attributes.
 *	`PEXFree"(`*valuesReturn') deallocates the memory.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 * 	FloatingPointFormat	Device does not support the specified floating point format.
 *
 * SEE ALSO:
 *	`PEXCreateRenderer", `PEXFreeRenderer", `PEXChangeRenderer",
 *	`PEXGetRendererDynamics", `PEXBeginRendering", `PEXEndRendering",
 *	`PEXGetRendererAttributes", `MPEXChangeNewRenderer"
 *
 */

Status
MPEXGetNewRendererAttributes (display, renderer, valueMask, valuesReturn)

INPUT Display			*display;
INPUT pxlRenderer		renderer;
INPUT unsigned long		valueMask;
OUTPUT mpxlRendererAttributes **valuesReturn;

{
/* AUTHOR:  Sally C. Barry */
    unsigned long			*pv;
    unsigned long			f;
    int					i;
    int					j;
    int					n;
    int					size;
    int					length;
    mpxlRendererAttributes		*prdra;
    pxlElementRef			*per;
    mpexGetNewRendererAttributesReq	*req;
    mpexGetNewRendererAttributesReply	rep;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    MPEXGetReq (GetNewRendererAttributes, req);
    req->fpFormat = (pexEnumTypeIndex)pexFpFormat;
    req->rdr = renderer;
    req->itemMask = valueMask;

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
        return (1);            /* return an error */
    }

    /* allocate a buffer for the replies to pass back to the client */
    pv = (unsigned long *)_XAllocScratch (display,
		(unsigned long)(rep.length << 2));

    _XRead (display, (char *)pv, (long)(rep.length << 2));
    prdra = (mpxlRendererAttributes *) PEXAllocBuf
        ((unsigned)(sizeof(PEXHeader) + sizeof(mpxlRendererAttributes)));
    ((PEXHeader *)prdra)->free = _PEXFreePtr; /* routine to free it */
    ((PEXHeader *)prdra)->type = 0;   /* set to point to data later, if nec */
    prdra = (mpxlRendererAttributes *)&(((PEXHeader *)prdra)[1]);

    *valuesReturn = prdra;

    /* copy the renderer attributes to the buffer */
    for (i=0; i<MaxNRAShift; i++)
    {
	f = (1L << i);
	if (valueMask & f)
	{
            switch (f)
	    {
	    case MPEXNRALogicalOp:
		prdra->logicalOp = *((INT16 *)pv);
		pv++;
		break;
	    case MPEXNRAPlaneMask:
		prdra->planeMask = *((CARD32 *)pv);
		pv++;
		break;
	    case MPEXNRABackgroundPixel:
		prdra->backgroundPixel = *((CARD32 *)pv);
		pv++;
		break;
	    case MPEXNRAClearI:
		prdra->clearI = *((CARD8 *)pv);
		pv++;
		break;
	    case MPEXNRAClearZ:
		prdra->clearZ = *((CARD8 *)pv);
		pv++;
		break;
	    case MPEXNRAOverlapMode:
		prdra->overlapMode = *((INT16 *)pv);
		pv++;
		break;
	    case MPEXNRAAaSwitch:
		prdra->aaSwitch = *((CARD8 *)pv);
		pv++;
		break;
	    case MPEXNRAHighlightTable:
		prdra->highlightTable = *((pxlLookupTable *)pv);
		pv++;
		break;
	    case MPEXNRAEchoTable:
		prdra->echoTable = *((pxlLookupTable *)pv);
		pv++;
		break;
	    case MPEXNRAEchoSwitch:
		prdra->echoSwitch = *((CARD8 *)pv);
		pv++;
		break;
	    case MPEXNRAPickPosition:
		prdra->pickPosition = *((pxlDeviceCoord2D *)pv);
		pv = (unsigned long *)((char *)pv + sizeof(pexDeviceCoord2D));
		break;
	    case MPEXNRAPickDistance:
		prdra->pickDistance = *((FLOAT *)pv);
		pv++;
		break;
	    case MPEXNRAPickInclusion:
		prdra->pickInclusion = *((pxlNameSet *)pv);
		pv++;
		break;
	    case MPEXNRAPickExclusion:
		prdra->pickExclusion = *((pxlNameSet *)pv);
		pv++;
		break;
	    case MPEXNRAPickMaxHits:
		prdra->pickMaxHits = *((CARD32 *)pv);
		pv++;
		break;
	    case MPEXNRAPickStartPath:
		n = *(int *)pv;
		pv++;
		prdra->pickStartPath.pathLength = n;
		size = n * sizeof(pxlElementRef);
		per = (pxlElementRef *)PEXAllocBuf ((unsigned)size);
		COPY_AREA (pv, (char *)per, size);
		prdra->pickStartPath.elementRefs = per;
		pv = (unsigned long *)((char *)pv + size);
		(((PEXHeader *)prdra)[-1]).type = (int)per; /* for _PEXFreePtr */
		break;
	    case MPEXNRACullSwitch:
		prdra->cullSwitch = *((CARD8 *)pv);
		pv++;
		break;
	    case MPEXNRAPixmapTable:
		prdra->pixmapTable = *((pxlLookupTable *)pv);
		pv++;
		break;
	    }
	}
    }

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	MPEXRenderElements
 *
 * FORMAT:
 * 	`MPEXRenderElements`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer by which the
 *			elements are to be processed.
 *
 *	structure	The resource ID of the structure which contains
 *			the elements.
 *
 *	whence1		An integer specifying, with `offset1', the
 *			beginning of the range of elements to be processed.
 *
 *	offset1		An offset from `whence1' denoting the beginning
 *			of the range of elements to be processed.
 *
 *	whence2		An integer specifying, with `offset2', the
 *			end of the range of elements to be processed.
 *
 *	offset2		An offset from `whence2' denoting the end
 *			of the range of elements to be processed.
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine causes the range of elements in `structure' to be
 *	processed by `renderer'.  This routine is a Digital extension to PEX
 *	and may not be available on other vendors' PEX server extensions.
 *	It facilitates highlighting of a primitive during server-side pick
 *	operations.
 *
 *	The beginning of the range is the sum of `whence1'
 *	plus `offset1'.   The end of the range is the sum
 *	of `whence2' and `offset2'.  Possible values for `whence1' and
 *	`whence2' are `PEXBeginning', `PEXCurrent', and `PEXEnd' (defined in the
 *	`PEX.h' include file).
 *	`Offset1' and `offset2' are integer values and can be negative.
 *
 *	For example, if `whence1' is `PEXBeginning', the beginning
 *	position is the value of `offset1'.  If
 *	`whence1' is `PEXCurrent', the beginning position is
 *	the value of the current element pointer position plus the value
 *	of `offset1'.
 *	If `whence1' is `PEXEnd', the end position is
 *	the offset of the last element in the structure plus the value
 *	of `offset2'.
 *
 *	If either computed offset is less than zero, it is set to
 *	zero before processinging the structure elements.  If either
 *	computed offset is greater than the number of elements in the
 *	structure, it is set to the offset of the last structure element
 *	in the structure.  The element pointer attribute of `structure'
 *	is not affected by this request.
 *
 *	This request operates similarly to a `PEXRenderOutputCommands"
 *	request, however, the output commands are processed from the
 *	structure stored in the PEX server extension, rather than from the
 *	client application program directly.
 *
 *	The application program must be careful if the structures are
 *	being shared by other applications or renderers.  Multiple
 *	access is possible, since the output commands are only being read,
 *	not modified.  Positions relative to the structure's element pointer
 *	may be used, but it is probably safer to use absolute element
 *	positions with this request.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateRenderer", `PEXCreateStructure", `MPEXAccumulateState"
 *
 */

void
MPEXRenderElements (display, renderer, structure, whence1, offset1, whence2,
	offset2)

INPUT Display		*display;
INPUT pxlRenderer	renderer;
INPUT pxlStructure	structure;
INPUT int		whence1;
INPUT long		offset1;
INPUT int		whence2;
INPUT long		offset2;

{
/* AUTHOR:  Sally C. Barry */
    mpexRenderElementsReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    MPEXGetReq (RenderElements, req);
    req->rdr = renderer;
    req->sid = structure;
    req->range.position1.whence = whence1;
    req->range.position1.offset = offset1;
    req->range.position2.whence = whence2;
    req->range.position2.offset = offset2;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	MPEXAccumulateState
 *
 * FORMAT:
 * 	`MPEXAccumulateState`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer which accumulates the
 *			hierarchical state.
 *
 *	elements	The element path which is to be traversed.
 *
 *	numElements	The number of elements in the element path.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine causes `renderer' to traverse the path specified by
 *	`elements'.  There is no visual output from the traversal, but the
 *	current hierarchical state is modified by output commands that
 *	change state.
 *	This routine is a Digital extension to PEX
 *	and may not be available on other vendors' PEX server extensions.
 *	It facilitates highlighting of a primitive during server-side pick
 *	operations.
 *
 *	Accumulation of state begins with the current pipeline values in
 *	`renderer'.   Output commands along the specified path are traversed
 *	in order.  Output commands that modify pipeline state, such as
 *	`PEXSetLineColour", affect the pipeline state.  Other output commands,
 *	such as output primitives and labels, are ignored.  Note that
 *	`PEXExecuteStructure" output commands have no affect on the
 *	pipeline values; they do not cause state values to be "pushed".
 *
 *	The renderer's `CurrentPath' attribute is not affected by this
 *	routine.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Path			Specified structure network path contains inappropriate or illegal values.
 *
 * SEE ALSO:
 *	`PEXCreateRenderer", `MPEXRenderElements"
 *
 */

void
MPEXAccumulateState (display, renderer, elements, numElements)

INPUT Display		*display;
INPUT pxlRenderer	renderer;
INPUT pxlElementRef	*elements;
INPUT int		numElements;

{
/* AUTHOR:  Sally C. Barry */
    mpexAccumulateStateReq	*req;
    int				size;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    size = numElements * sizeof(pxlElementRef);

    MPEXGetReqExtra (AccumulateState, size, req);
    req->rdr = renderer;
    req->numRefs = numElements;

    COPY_AREA (elements, ((char *)&(req[1])), size);

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	MPEXBeginPickOne
 *
 * FORMAT:
 * 	`MPEXBeginPickOne`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer on which the picking
 *			occurs.
 *
 *	d		The resource ID of an X window or pixmap.
 *
 *	id		The identifier of the root of the client structure
 *			network.
 *
 *	which		Indicates whether the closest or last result is to be
 *			returned.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine is a Digital extension to PEX
 *	and may not be available on other vendors' PEX server extensions.
 *
 *	[ TBD -- Note: this is for CLIENT-SIDE structures ]
 *
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Drawable		Specified drawable resource ID is invalid.
 *
 *	Value			`which' is invalid.
 *
 * SEE ALSO:
 *	`MPEXEndPickOne", `MPEXPickOne", `MPEXBeginPickAll",
 *	`MPEXEndPickAll", `MPEXPickAll"
 *
 */

void
MPEXBeginPickOne (display, renderer, d, id, which)

INPUT Display		*display;
INPUT pxlRenderer	renderer;
INPUT Drawable		d;
INPUT unsigned long	id;
INPUT int		which;

{
/* AUTHOR:  Sally C. Barry */
    mpexBeginPickOneReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    MPEXGetReq (BeginPickOne, req);
    req->rdr = renderer;
    req->drawable = d;
    req->id = id;
    req->which = which;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	MPEXEndPickOne
 *
 * FORMAT:
 * 	`MPEXEndPickOne`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer on which the picking
 *			occurred.
 *
 *	elementsReturn
 *
 *	numElementsReturn
 *
 * RETURNS:
 *	Zero if the pick was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine is a Digital extension to PEX
 *	and may not be available on other vendors' PEX server extensions.
 *
 *	[ TBD -- Note: this is for CLIENT-SIDE structures ]
 *
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 * SEE ALSO:
 *	`MPEXBeginPickOne", `MPEXPickOne", `MPEXBeginPickAll",
 *	`MPEXEndPickAll", `MPEXPickAll"
 *
 */

Status
MPEXEndPickOne (display, renderer, elementsReturn, numElementsReturn)

INPUT Display			*display;
INPUT pxlRenderer		renderer;
OUTPUT pxlPickPath		**elementsReturn;
OUTPUT int			*numElementsReturn;

{
/* AUTHOR:  Sally C. Barry */
    mpexEndPickOneReply		rep;
    mpexEndPickOneReq		*req;
    pxlPickPath		*info;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    MPEXGetReq (EndPickOne, req);
    req->id = renderer;

    if (_XReply (display, &rep, 0, xFalse) == 0)
    { /* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
        return (1);               /* return an error */
    }

    *numElementsReturn = rep.numRefs;

    /* allocate a buffer for the path to pass back to the client */
    info = (pxlPickPath *)PEXAllocBuf ((unsigned)(sizeof(PEXHeader) +
			(rep.numRefs * sizeof(pxlPickPath))));
    ((PEXHeader *)info)->free = _PEXFreeList; /* routine to free it */
    info = (pxlPickPath *)&((PEXHeader *)info)[1];
    *elementsReturn = info;

    _XRead (display, (char *)info, (long)(rep.length << 2));

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	MPEXPickOne
 *
 * FORMAT:
 * 	`MPEXPickOne`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer on which the picking
 *			occurred.
 *
 *	d		The resource ID of an X window or pixmap.
 *
 *	structure	The resource ID of the root of the structure
 *			network.
 *
 *	which		Indicates whether the closest or last result is to be
 *			returned.
 *	elementsReturn
 *
 *	numElementsReturn
 *
 * RETURNS:
 *	Zero if the pick was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine is a Digital extension to PEX
 *	and may not be available on other vendors' PEX server extensions.
 *
 *	[ TBD -- Note: this is for SERVER-SIDE structures ]
 *
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Drawable		Specified drawable resource ID is invalid.
 *
 *	Structure		Specified structure resource ID is invalid.
 *
 *	Value			`which' is invalid.
 *
 * SEE ALSO:
 *	`MPEXBeginPickOne", `MPEXEndPickOne", `MPEXBeginPickAll",
 *	`MPEXEndPickAll", `MPEXPickAll"
 *
 */

Status
MPEXPickOne (display, renderer, d, structure, which, elementsReturn,
	numElementsReturn)

INPUT Display			*display;
INPUT pxlRenderer		renderer;
INPUT Drawable			d;
INPUT pxlStructure		structure;
INPUT int			which;
OUTPUT pxlPickPath	**elementsReturn;
OUTPUT int			*numElementsReturn;

{
/* AUTHOR:  Sally C. Barry */
    mpexPickOneReply		rep;
    mpexPickOneReq		*req;
    pxlPickPath		*info;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    MPEXGetReq (PickOne, req);
    req->rdr = renderer;
    req->drawable = d;
    req->sid = structure;
    req->which = which;

    if (_XReply (display, &rep, 0, xFalse) == 0)
    { /* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
        return (1);               /* return an error */
    }

    *numElementsReturn = rep.numRefs;

    /* allocate a buffer for the path to pass back to the client */
    info = (pxlPickPath *)PEXAllocBuf ((unsigned)(sizeof(PEXHeader) +
			(rep.numRefs * sizeof(pxlPickPath))));
    ((PEXHeader *)info)->free = _PEXFreeList; /* routine to free it */
    info = (pxlPickPath *)&((PEXHeader *)info)[1];
    *elementsReturn = info;

    _XRead (display, (char *)info, (long)(rep.length << 2));

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	MPEXBeginPickAll
 *
 * FORMAT:
 * 	`MPEXBeginPickAll`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer on which the picking
 *			occurs.
 *
 *	d		The resource ID of an X window or pixmap.
 *
 *	id		The identifier of the root of the client structure
 *			network.
 *
 *	sendEvent	A Boolean operator specifying whether an event
 *			is received when the maximum number of hits is
 *			reached.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine is a Digital extension to PEX
 *	and may not be available on other vendors' PEX server extensions.
 *
 *	[ TBD -- Note: this is for CLIENT-SIDE structures ]
 *
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Drawable		Specified drawable resource ID is invalid.
 *
 *	Value			`sendEvent' is invalid.
 *
 * SEE ALSO:
 *	`MPEXBeginPickOne", `MPEXEndPickOne", `MPEXPickOne",
 *	`MPEXEndPickAll", `MPEXPickAll"
 *
 */

void
MPEXBeginPickAll (display, renderer, d, id, sendEvent)

INPUT Display		*display;
INPUT pxlRenderer	renderer;
INPUT Drawable		d;
INPUT unsigned long	id;
INPUT Bool		sendEvent;

{
/* AUTHOR:  Sally C. Barry */
    mpexBeginPickAllReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    MPEXGetReq (BeginPickAll, req);
    req->rdr = renderer;
    req->drawable = d;
    req->id = id;
    req->sendEvent = sendEvent;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	MPEXEndPickAll
 *
 * FORMAT:
 * 	`MPEXEndPickAll`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer on which the picking
 *			occurred.
 *
 *	moreReturn	`NoMorePicks', `MorePicks', `MaybeMorePicks'
 *
 *	pathsReturn
 *
 *	numPathsReturn
 *
 * RETURNS:
 *	Zero if the pick was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine is a Digital extension to PEX
 *	and may not be available on other vendors' PEX server extensions.
 *
 *	[ TBD -- Note: this is for CLIENT-SIDE structures ]
 *
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 * SEE ALSO:
 *	`MPEXBeginPickOne", `MPEXEndPickOne", `MPEXPickOne",
 *	`MPEXBeginPickAll", `MPEXPickAll"
 *
 */

Status
MPEXEndPickAll (display, renderer, moreReturn, pathsReturn, numPathsReturn)

INPUT Display		*display;
INPUT pxlRenderer	renderer;
OUTPUT int		*moreReturn;
OUTPUT mpxlPickAllPath	**pathsReturn;
OUTPUT int		*numPathsReturn;

{
/* AUTHOR:  Sally C. Barry */
    mpexEndPickAllReq		*req;
    mpexEndPickAllReply		rep;
    mpxlPickAllPath		*psp;
    char			*prep;
    pxlPickPath			*per;
    int				i;
    int				numElements;
    int				size;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    MPEXGetReq (EndPickAll, req);
    req->id = renderer;

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
	return (1);           /* return an error */
    }

    *moreReturn = rep.morePicks;

    /* allocate a buffer for the replies to pass back to the client */
    psp = (mpxlPickAllPath *)PEXAllocBuf ((unsigned)(sizeof(PEXHeader) +
    			rep.numPicks * sizeof(mpxlPickAllPath)));
    ((PEXHeader *)psp)->free = _PEXFreeStructurePath;/* routine to free it */
    ((PEXHeader *)psp)->size = rep.numPicks;         /* size of buffer */
    psp = (mpxlPickAllPath *)&(((PEXHeader *)psp)[1]);

    *pathsReturn = psp;
    *numPathsReturn = rep.numPicks;

    /* fill the buffer */
    prep = _XAllocScratch (display, (unsigned long)(rep.length << 2));
    _XRead (display, (char *)prep, (long)(rep.length << 2));

    for (i=0; i<rep.numPicks; i++)
    {
	numElements = *((CARD32 *)prep);
	prep += sizeof(CARD32);
	size = numElements * sizeof(pxlPickPath);
	per = (pxlPickPath *)PEXAllocBuf ((unsigned)size);
	COPY_AREA (prep, (char *)per, size);
	psp->pathLength = numElements;
	psp->elementRefs = per;
	psp++;
	prep += size;
    }

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	MPEXPickAll
 *
 * FORMAT:
 * 	`MPEXPickAll`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer on which the picking
 *			occurred.
 *
 *	d		The resource ID of an X window or pixmap.
 *
 *	moreReturn	`NoMorePicks', `MorePicks', `MaybeMorePicks'
 *
 *	pathsReturn
 *
 *	numPathsReturn
 *
 * RETURNS:
 *	Zero if the pick was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine is a Digital extension to PEX
 *	and may not be available on other vendors' PEX server extensions.
 *
 *	[ TBD -- Note: this is for SERVER-SIDE structures ]
 *
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Drawable		Specified drawable resource ID is invalid.
 *
 * SEE ALSO:
 *	`MPEXBeginPickOne", `MPEXEndPickOne", `MPEXPickOne",
 *	`MPEXBeginPickAll", `MPEXEndPickAll"
 *
 */

Status
MPEXPickAll (display, renderer, d, moreReturn, pathsReturn, numPathsReturn)

INPUT Display		*display;
INPUT pxlRenderer	renderer;
INPUT Drawable		d;
OUTPUT int		*moreReturn;
OUTPUT mpxlPickAllPath	**pathsReturn;
OUTPUT int		*numPathsReturn;

{
/* AUTHOR:  Sally C. Barry */
    mpexPickAllReq		*req;
    mpexPickAllReply		rep;
    mpxlPickAllPath		*psp;
    char			*prep;
    pxlPickPath			*per;
    int				i;
    int				numElements;
    int				size;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    MPEXGetReq (PickAll, req);
    req->rdr = renderer;
    req->drawable = d;

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
	return (1);           /* return an error */
    }

    *moreReturn = rep.morePicks;

    /* allocate a buffer for the replies to pass back to the client */
    psp = (mpxlPickAllPath *)PEXAllocBuf ((unsigned)(sizeof(PEXHeader) +
    			rep.numPicks * sizeof(mpxlPickAllPath)));
    ((PEXHeader *)psp)->free = _PEXFreeStructurePath;/* routine to free it */
    ((PEXHeader *)psp)->size = rep.numPicks;         /* size of buffer */
    psp = (mpxlPickAllPath *)&(((PEXHeader *)psp)[1]);

    *pathsReturn = psp;
    *numPathsReturn = rep.numPicks;

    /* fill the buffer */
    prep = _XAllocScratch (display, (unsigned long)(rep.length << 2));
    _XRead (display, (char *)prep, (long)(rep.length << 2));

    for (i=0; i<rep.numPicks; i++)
    {
	numElements = *((CARD32 *)prep);
	prep += sizeof(CARD32);
	size = numElements * sizeof(pxlPickPath);
	per = (pxlPickPath *)PEXAllocBuf ((unsigned)size);
	COPY_AREA (prep, (char *)per, size);
	psp->pathLength = numElements;
	psp->elementRefs = per;
	psp++;
	prep += size;
    }

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	MPEXSetHighlightIndex
 *
 * FORMAT:
 * 	`MPEXSetHighlightIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		An index indicating the entry in the highlight
 *			table used to obtain the highlighting parameters.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the index value used to obtain the highlighting parameters.
 *	Primitives rendered after this command which pass the highlight
 *	nameset test use the highlighting method and value
 *	parameters obtained from the highlight lookup
 *	table entry referenced by `index'.  The default
 *	table entry is zero. (?)
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `MPEXoc.h':
 *
 *	==
 *	MPEX_SetHighlightIndex (ocbuf, index)
 *	!=
 *
 *	`ocbuf' is a pointer to an OC buffer.  `index'
 *	is a pointer to a value of type `pxlTableIndex' (defined in include
 *	file `PEXprotost.h') indicating the highlight
 *	table entry from which highlighting parameters are to
 *	be obtained.
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
 *	`PEXAllocateOCBuffer", `PEXCreateLookupTable"
 *
 */

void
MPEXSetHighlightIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR:  Sally C. Barry */
    pxlTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, MPEXOCHighlightIndex, sizeof( pxlTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	MPEXSetEchoIndex
 *
 * FORMAT:
 * 	`MPEXSetEchoIndex`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	index		An index indicating the entry in the echo
 *			table used to obtain the preselect echoing parameters.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command
 *	sets the index value used to obtain the preselect echo parameters.
 *	Primitives which are preselected during a pick operation
 *	use the echoing method and value parameters obtained from the echo
 *	lookup table entry referenced by `index'.  The default
 *	table entry is zero. (?)
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `MPEXoc.h':
 *
 *	==
 *	MPEX_SetEchoIndex (ocbuf, index)
 *	!=
 *
 *	`ocbuf' is a pointer to an OC buffer.  `index'
 *	is a pointer to a value of type `pxlTableIndex' (defined in include
 *	file `PEXprotost.h') indicating the echo
 *	table entry from which preselect echo parameters are to
 *	be obtained.
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
 *	`PEXAllocateOCBuffer", `PEXCreateLookupTable"
 *
 */

void
MPEXSetEchoIndex (ocbuf, index)

INPUT pxlOCBuf		*ocbuf;
INPUT int		index;

{
/* AUTHOR:  Sally C. Barry */
    pxlTableIndex	i;

    i = index;
    PEXAddOC (ocbuf, MPEXOCEchoIndex, sizeof( pxlTableIndex), &i);

    return;
}

/*+
 * NAME:
 * 	MPEXSetElementPtrAtPickID
 *
 * FORMAT:
 * 	`MPEXSetElementPtrAtPickID`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	structure	An unsigned longword giving the resource ID
 *			of the structure whose element pointer position
 *			is to be set.
 *
 *	id		An unsigned integer specifying the pick identifier.
 *
 *	offset		A long integer specifying the offset from
 *			the pointer position of `id'.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *  	This routine sets the element pointer for the structure specified as
 *	`structure' at the position of the next `SetPickID" output
 *	command which has the specified pick identifier.
 *	If `id' is found, the element pointer for the structure is set to the
 *	offset of the output command that was found plus the value of the
 *	offset `offset'.  If the requested
 *	output command is not found, the structure's element pointer is
 *	left unchanged.  (For now, a Label error is returned.  May create an
 *	MPEX PickID error later.)
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateStructure",  `PEXSetPickID"
 *
 */

void
MPEXSetElementPtrAtPickID (display, structure, id, offset)

INPUT Display		*display;
INPUT pxlStructure	structure;
INPUT unsigned long	id;
INPUT long		offset;

{
/* AUTHOR:  Sally C. Barry */
    mpexSetElementPointerAtPickIdReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    MPEXGetReq (SetElementPointerAtPickId, req);
    req->str = structure;
    req->pickid = id;
    req->offset = offset;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/* Routine to convert a protocol-format event (wire) to a client event
structure (client) for a MPEXMaxHitsReachedEvent.  XESetWireToEvent is called
in PEXInitialize to set this up with Xlib. */

Status
_MPEXConvertMaxHitsEvent (display, client, wire)

INPUT	Display		*display;
INPUT	XEvent		*client;
INPUT	xEvent		*wire;

{
/* AUTHOR:  Sally C. Barry */
    mpexMaxHitsReachedEvent	*wireevent;
    mpxlMaxHitsReachedEvent	*clientevent;

    /* set up the pointers */
    wireevent = (mpexMaxHitsReachedEvent *)wire;
    clientevent = (mpxlMaxHitsReachedEvent *)client;

    /* now fill in the client structure */
    clientevent->type = wireevent->type & 0x7f;
    clientevent->serial = wireevent->sequenceNumber;
    clientevent->send_event = (wireevent->type & 0x80) != 0;
                              /* MSB set if event came from SendEvent request */
    clientevent->display = display;
    clientevent->rdr = wireevent->rdr;

    /* tell Xlib to add this to the event queue */
    return (True);
}

/*+
 * NAME:
 * 	MPEXNoop
 *
 * FORMAT:
 * 	`MPEXNoop`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	When processed by a renderer, this output command does nothing.  The
 *	current path is updated.
 *
 *	This output command is provided to facilitate restarting a client
 *	side `PickAll' traversal at an arbitrary start path.  The client
 *	side structure traverser can send `Noop" output commands in place of
 *	any primitives along the start path.  This preserves the proper offsets
 *	in the current path, while not redisplaying output primitives.
 *
 *	C programmers can elect to use the equivalent C macro defined in
 *	include file `MPEXoc.h':
 *
 *	==
 *	MPEX_Noop (ocbuf)
 *	!=
 *
 *	where `ocbuf' is a pointer to an OC buffer.
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
 *	`PEXAllocateOCBuffer", `MPEXBeginPickAll"
 *
 */

void
MPEXNoop (ocbuf)

INPUT pxlOCBuf		*ocbuf;

{
/* AUTHOR:  Sally C. Barry */
    PEXAddOC (ocbuf, MPEXOCNoop, 0, (char *)NULL);

    return;
}

/*+
 * NAME:
 * 	MPEXCircle
 *
 * FORMAT:
 * 	`MPEXCircle`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	center		The center of the circle primitive.
 *
 *	radius		The radius of the circle primitive.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine, when processed by a renderer, causes a "circle"
 *	primitive to be rendered.  The circle will be interpolated with
 *	polylines, using the current curve approximation method and the
 *	current polyline attributes.  The circle's
 *	center is at the modelling coordinate position `center'.  The radius
 *	of the circle is specified by the modelling coordinate distance
 *	`radius', which defines a distance along the X axis.
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
 *	`PEXSetLineColourIndex", `PEXSetLineColour", `PEXSetLineBundleIndex",
 *	`PEXSetCurveApproxMethod"
 *
 */

void
MPEXCircle (ocbuf, center, radius)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord3D	*center;
INPUT float		radius;

{
/* AUTHOR:  Sally C. Barry */
    mpexCircle		*pReq;

    PEXInitOC( ocbuf, MPEXOCCircle, LENOF(mpexCircle), 0, &pReq);
    if (pReq == NULL) return;

    pReq->center = *center;
    pReq->radius = radius;

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 * 	MPEXArc
 *
 * FORMAT:
 * 	`MPEXArc`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	center		The center of the circular arc primitive.
 *
 *	radius		The radius of the circular arc primitive.
 *
 *	startAngle	The starting angle of the arc, in radians.
 *
 *	endAngle	The ending angle of the arc, in radians.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine, when processed by a renderer, causes a "circular arc"
 *	primitive to be rendered.  The arc will be interpolated with
 *	polylines, using the current curve approximation method and the
 *	current polyline attributes.  The arc's
 *	center is at the modelling coordinate position `center'.  The radius
 *	of the arc is specified by the modelling coordinate distance
 *	`radius', which defines a distance along the X axis.
 *	`StartAngle' and `endAngle' are measured in radians, with positive
 *	angles measured counter-clockwise from the positive modelling
 *	space X axis.
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
 *	`PEXSetLineColourIndex", `PEXSetLineColour", `PEXSetLineBundleIndex",
 *	`PEXSetCurveApproxMethod"
 *
 */

void
MPEXArc (ocbuf, center, radius, startAngle, endAngle)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord3D	*center;
INPUT float		radius;
INPUT float		startAngle;
INPUT float		endAngle;

{
/* AUTHOR:  Sally C. Barry */
    mpexArc		*pReq;

    PEXInitOC( ocbuf, MPEXOCArc, LENOF(mpexArc), 0, &pReq);
    if (pReq == NULL) return;

    pReq->center = *center;
    pReq->radius = radius;
    pReq->startAngle = startAngle;
    pReq->endAngle = endAngle;

    PEXFinishOC( ocbuf);

    return;
}



/*+
 * NAME:
 * 	MPEXEllipticalArc2D
 *
 * FORMAT:
 * 	`MPEXEllipticalArc2D`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	center		The center of the circular arc primitive.
 *
 *	majorAxis	The major axis of the ellipse local coordinate system
 *
 *	minorAxis	The minor axis of the ellipse local coordinate system
 *
 *	startAngle	The starting angle of the arc, in radians.
 *
 *	endAngle	The ending angle of the arc, in radians.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine, when processed by a renderer, causes a "EllipticalArc2D"
 *	primitive to be rendered.  The EllipticalArc2D will be interpolated with
 *	polylines, using the current curve approximation method and the
 *	current polyline attributes.  The ellipticalarc's
 *	center is at the modelling coordinate position `center'.  
 *	The majorAxis defines the positive X-axis direction of the 2D
 *	elliptical arc local coordinate system.  The minorAxis is used in
 *	defining the Y-axis of the elliptical arc local coordinate system.
 *	The major and minor radius will be decided by the length of major
 *	and minor axes. The start_angle and the end_angle are measured in
 *	radians, with positive angles measured counter-clockwise from the
 *	positive X axis of elliptical arc local coordinate system.
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
 *	`PEXSetLineColourIndex", `PEXSetLineColour", `PEXSetLineBundleIndex",
 *	`PEXSetCurveApproxMethod"
 *
 */

void
MPEXEllipticalArc2D (ocbuf, center, majorAxis, minorAxis, startAngle, endAngle)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord2D	*center;
INPUT pxlCoord2D	*majorAxis;
INPUT pxlCoord2D	*minorAxis;
INPUT float		startAngle;
INPUT float		endAngle;

{
/* AUTHOR: */
    mpexEllipticalArc2D	*pReq;

    PEXInitOC( ocbuf, MPEXOCEllipticalArc2D, LENOF(mpexEllipticalArc2D), 0, &pReq);
    if (pReq == NULL) return;

    pReq->center = *center;
    pReq->majorAxis = *majorAxis;
    pReq->minorAxis = *minorAxis;
    pReq->startAngle = startAngle;
    pReq->endAngle = endAngle;

    PEXFinishOC( ocbuf);

    return;
}


/*+
 * NAME:
 * 	MPEXEllipticalArc3D
 *
 * FORMAT:
 * 	`MPEXEllipticalArc3D`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	center		The center of the circular arc primitive.
 *
 *	majorAxis	The major axis of the ellipse local coordinate system
 *
 *	minorAxis	The minor axis of the ellipse local coordinate system
 *
 *	startAngle	The starting angle of the arc, in radians.
 *
 *	endAngle	The ending angle of the arc, in radians.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine, when processed by a renderer, causes a "EllipticalArc3D"
 *	primitive to be rendered.  The EllipticalArc3D will be interpolated with
 *	polylines, using the current curve approximation method and the
 *	current polyline attributes.  The ellipticalarc's
 *	center is at the modelling coordinate position `center'.  
 *	The majorAxis defines the positive X-axis direction of the 3D
 *	elliptical arc local coordinate system.  The minorAxis is used in
 *	defining the Y-axis of the elliptical arc local coordinate system.
 *	The major and minor radius will be decided by the length of major
 *	and minor axes. The start_angle and the end_angle are measured in
 *	radians, with positive angles measured counter-clockwise from the
 *	positive X axis of elliptical arc local coordinate system.
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
 *	`PEXSetLineColourIndex", `PEXSetLineColour", `PEXSetLineBundleIndex",
 *	`PEXSetCurveApproxMethod"
 *
 */

void
MPEXEllipticalArc3D (ocbuf, center, majorAxis, minorAxis, startAngle, endAngle)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord3D	*center;
INPUT pxlCoord3D	*majorAxis;
INPUT pxlCoord3D	*minorAxis;
INPUT float		startAngle;
INPUT float		endAngle;

{
/* AUTHOR: */
    mpexEllipticalArc3D	*pReq;

    PEXInitOC( ocbuf, MPEXOCEllipticalArc3D, LENOF(mpexEllipticalArc3D), 0, &pReq);
    if (pReq == NULL) return;

    pReq->center = *center;
    pReq->majorAxis = *majorAxis;
    pReq->minorAxis = *minorAxis;
    pReq->startAngle = startAngle;
    pReq->endAngle = endAngle;

    PEXFinishOC( ocbuf);

    return;
}


/*+
 * NAME:
 * 	MPEXEllipse2D
 *
 * FORMAT:
 * 	`MPEXEllipse2D`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	center		The center of the circular arc primitive.
 *
 *	majorAxis	The major axis of the ellipse local coordinate system
 *
 *	minorAxis	The minor axis of the ellipse local coordinate system
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine, when processed by a renderer, causes a 2D filled ellipse
 *	primitive to be rendered.  The 2D ellipse will use the current curve
 *	approximation method and the current fill area attributes.  
 *	The ellipse's center is at the modelling coordinate position `center'.  
 *	The majorAxis defines the positive X-axis direction of the 2D
 *	elliptical arc local coordinate system.  The minorAxis is used in
 *	defining the Y-axis of the elliptical arc local coordinate system.
 *	The major and minor radius will be decided by the length of major
 *	and minor axes. 
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
 *	`PEXSetLineColourIndex", `PEXSetSurfaceColour", `PEXSetSurfaceBundleIndex",
 *	`PEXSetCurveApproxMethod"
 *
 */

void
MPEXEllipse2D (ocbuf, center, majorAxis, minorAxis)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord2D	*center;
INPUT pxlCoord2D	*majorAxis;
INPUT pxlCoord2D	*minorAxis;

{
/* AUTHOR: */
    mpexEllipse2D	*pReq;

    PEXInitOC( ocbuf, MPEXOCEllipse2D, LENOF(mpexEllipse2D), 0, &pReq);
    if (pReq == NULL) return;

    pReq->center = *center;
    pReq->majorAxis = *majorAxis;
    pReq->minorAxis = *minorAxis;

    PEXFinishOC( ocbuf);

    return;
}


/*+
 * NAME:
 * 	MPEXEllipse3D
 *
 * FORMAT:
 * 	`MPEXEllipse3D`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	center		The center of the circular arc primitive.
 *
 *	majorAxis	The major axis of the ellipse local coordinate system
 *
 *	minorAxis	The minor axis of the ellipse local coordinate system
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine, when processed by a renderer, causes a 3D filled ellipse
 *	primitive to be rendered.  The 3D ellipse will use the current curve
 *	approximation method and the current fill area attributes.  
 *	The ellipse's center is at the modelling coordinate position `center'.  
 *	The majorAxis defines the positive X-axis direction of the 3D
 *	elliptical arc local coordinate system.  The minorAxis is used in
 *	defining the Y-axis of the elliptical arc local coordinate system.
 *	The major and minor radius will be decided by the length of major
 *	and minor axes. 
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
 *	`PEXSetLineColourIndex", `PEXSetSurfaceColour", `PEXSetSurfaceBundleIndex",
 *	`PEXSetCurveApproxMethod"
 *
 */

void
MPEXEllipse3D (ocbuf, center, majorAxis, minorAxis)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord3D	*center;
INPUT pxlCoord3D	*majorAxis;
INPUT pxlCoord3D	*minorAxis;

{
/* AUTHOR: */
    mpexEllipse3D	*pReq;

    PEXInitOC( ocbuf, MPEXOCEllipse3D, LENOF(mpexEllipse3D), 0, &pReq);
    if (pReq == NULL) return;

    pReq->center = *center;
    pReq->majorAxis = *majorAxis;
    pReq->minorAxis = *minorAxis;

    PEXFinishOC( ocbuf);

    return;
}


/*+
 * NAME:
 * 	MPEXGridRectangular
 *
 * FORMAT:
 *	`MPEXGridRectangular`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *      origin          Specifies the Origin to be used for drawing the 
 *                      grid points.
 *
 *	xdist           The resolution of the grid points in the X direction
 *
 *      ydist           The resolution of the grid points in the Y direction
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine, when processed by a renderer, causes a rectangular grid to be
 *      drawn parallel to the view plane using the current marker attributes. The 
 *      marker type however is set to PEXMarkerDot and the resolution in the X and
 *      Y directions are specified by xdist and ydist.
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
 *	`PEXAllocateOCBuffer", `MPEXChangeNewRenderer", `PEXCreateLookupTable"
 *
 */

void
MPEXGridRectangular (ocbuf,origin,xdist,ydist)

INPUT  pxlOCBuf    *ocbuf;
INPUT  pxlCoord3D  *origin;
INPUT  float       xdist;
INPUT  float       ydist;
{

  mpexGridRectangular   *pReq;

  PEXInitOC(ocbuf,MPEXOCGridRectangular,LENOF(mpexGridRectangular),0,&pReq);
  if(pReq == NULL) return;

  pReq->origin = *origin;
  pReq->xdist  = xdist;
  pReq->ydist  = ydist;

  PEXFinishOC(ocbuf);

  return;
}
 
/*+
 * NAME:
 * 	MPEXGridRadial
 *
 * FORMAT:
 *	`MPEXGridRadial`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *      center          The origin for the radial grid
 *
 *      startAngle      Starting angle for the radial lines
 * 
 *      endAngle        Ending angle measured counter-clockwise for the radial lines
 * 
 *      angDist         Angular distance specifies the angular distance between the lines
 *
 *      radDist         Radial distance specifies the distance between the points in any
 *                      given radial line.
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine, when processed by a renderer, causes a radial grid to be
 *      drawn parallel to the view plane using the current marker attributes. The 
 *      marker type however is set to PEXMarkerDot. The Grid centered at "center" and
 *      spreads to inifinity with start angle and end angle specified and the lines are
 *      are drawn with a distance of angDist and the points in the lines are drawn radDist
 *      apart.
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
 *	`PEXAllocateOCBuffer", `MPEXChangeNewRenderer", `PEXCreateLookupTable"
 *
 */

void 
MPEXGridRadial (ocbuf,center,startAngle,endAngle,angDist,radDist)

INPUT pxlOCBuf     *ocbuf;
INPUT pxlCoord3D   *center;
INPUT float        startAngle;
INPUT float        endAngle;
INPUT float        angDist;
INPUT float        radDist;

{

  mpexGridRadial    *pReq;

  PEXInitOC(ocbuf,MPEXOCGridRadial,LENOF(mpexGridRadial),0,&pReq);
  if(pReq == NULL) return;

  pReq->center     = *center;
  pReq->startAngle = startAngle;
  pReq->endAngle   = endAngle;
  pReq->angDist    = angDist;
  pReq->radDist    = radDist;

  PEXFinishOC(ocbuf);

  return;

}

/*+
 * NAME:
 * 	MPEXAnnotationPixmap
 *
 * FORMAT:
 *	`MPEXAnnotationPixmap`
 *
 * ARGUMENTS:
 *	ocbuf		The buffer to which the output command is added.
 *
 *	origin		The origin of the annotation pixmap primitive.
 *
 *	index		An index indicating the entry in the pixmap
 *			table used to obtain the pixmap resource ID.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine, when processed by a renderer, causes an "annotation
 *	pixmap" primitive to be rendered.  The pixmap, specified by the
 *	pixmap table entry selected by `index', will be copied to the device
 *	coordinate position that corresponds to the projected modelling
 *	space position of `origin'.  If `origin' is clipped, the pixmap is
 *	not drawn.
 *	The copy operations is affected by the renderer's plane mask,
 *	logical operation, clip list, and HLHSR mode.  It is not affected by
 *	highlighting or echo methods.  Pixmaps may be picked.
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
 *	`PEXAllocateOCBuffer", `MPEXChangeNewRenderer", `PEXCreateLookupTable"
 *
 */

void
MPEXAnnotationPixmap (ocbuf, origin, index)

INPUT pxlOCBuf		*ocbuf;
INPUT pxlCoord3D	*origin;
INPUT int		index;

{
/* AUTHOR:  Sally C. Barry */
    mpexAnnoPixmap	*pReq;

    PEXInitOC( ocbuf, MPEXOCAnnoPixmap, LENOF( mpexAnnoPixmap), 0, &pReq);
    if (pReq == NULL) return;

    pReq->origin = *origin;
    pReq->index = index;

    PEXFinishOC( ocbuf);

    return;
}

/*+
 * NAME:
 *	MPEXSetDashPattern
 *
 * FORMAT:
 *	`MPEXSetDashPattern`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer whose dash pattern is
 *			being set.
 *
 *	patternNumber	The number of the pattern being set.
 *
 *	pattern		A bitmask specifying the pattern that may be used for
 *			drawing lines or surface edges.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine sets the dash pattern `patternNumber' associated with
 *	`renderer' to the specified `pattern'.  Valid values for `patternNumber'
 *	are `DashPattern1' through `DashPattern16'.
 *
 *	Each bit in `pattern', from least significant bit to most significant
 *	bit, is used to enable (if 1) or disable (if 0) writing of pixels when
 *	rendering lines or curves.  Along a line, the pattern wraps after 32
 *	pixels are processed.  (PEXOn some PEX server extensions, the pattern
 *	wraps after 16 pixels are processed.  For identical behavior on all
 *	Digital PEX server extensions,
 *	users should make the low and high 16-bits of `pattern' the same.)
 *	Patterns are pixel based and are not scaled.
 *
 *	The patterns set by this routine may be selected for drawing
 *	line primitives by calling `PEXSetLineType" if the
 *	`PEXLineTypeAsf' is `PEXIndividual' or by setting the selected `PEXLineBundleLUT'
 *	table entry if `PEXLineTypeAsf' is `PEXBundled'.  They may be selected for
 *	drawing surface edges by calling `PEXSetSurfaceEdgeType" if the
 *	`PEXSurfaceEdgeTypeAsf' is `PEXIndividual' or by setting the selected
 *	`PEXEdgeBundleLUT' table entry if `PEXSurfaceEdgeTypeAsf' is `PEXBundled'.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Value			`patternNumber' is invalid.
 *
 * SEE ALSO:
 *	`PEXSetLineType", `PEXSetSurfaceEdgeType", `PEXSetTableEntries"
 *
 */

void
MPEXSetDashPattern (display, renderer, patternNumber, pattern)

INPUT Display		*display;
INPUT pxlRenderer	renderer;
INPUT int		patternNumber;
INPUT unsigned long	pattern;

{
/* AUTHOR:  Sally C. Barry */
    mpexSetDashPatternReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    MPEXGetReq (SetDashPattern, req);
    req->rdr = renderer;
    req->patternNum = patternNumber;
    req->pattern = pattern;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	MPEXGetDashPattern
 *
 * FORMAT:
 * 	`MPEXGetDashPattern`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer whose dash pattern
 *			is being inquired.
 *
 *	patternNumber	The number of the pattern being inquired.
 *
 *	pattern		Returns a pointer to the requested dash pattern.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine returns the dash pattern associated with `patternNumber'
 *	for the specified renderer.   Valid values for `patternNumber' are
 *	`DashPattern1' through `DashPattern16'.  If the specified pattern
 *	number is not supported on
 *	the PEX server extension, a `Value' error is returned by the server.
 *	(See `PEXGetEnumTypeInfo".)
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Value			`patternNumber' is invalid.
 *
 * SEE ALSO:
 *	`MPEXSetDashPattern"
 *
 */

Status
MPEXGetDashPattern (display, renderer, patternNumber, pattern)

INPUT Display		*display;
INPUT pxlRenderer	renderer;
INPUT int		patternNumber;
OUTPUT unsigned long	*pattern;

{
/* AUTHOR:  Sally C. Barry */
    mpexGetDashPatternReply		rep;
    mpexGetDashPatternReq		*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    MPEXGetReq (GetDashPattern, req);
    req->rdr = renderer;
    req->patternNum = patternNumber;

    if (_XReply (display, &rep, 0, xTrue) == 0)
    { /* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
        return (1);               /* return an error */
    }

    *pattern = rep.pattern;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 *	MPEXSetMarkerGlyph
 *
 * FORMAT:
 *	`MPEXSetMarkerGlyph`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer whose marker glyph is
 *			being set.
 *
 *	glyphNumber	The number of the marker glyph being set.
 *
 *	glyph		A 16 x 16 bitmask specifying the glyph that may be
 *			used for drawing marker primitives.
 *
 *	xOffset		The X pixel position of the glyph's hot spot.
 *
 *	yOffset		The Y pixel position of the glyph's hot spot.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine sets the marker glyph `glyphNumber' associated with
 *	`renderer' to the specified `glyph'.  Valid values for `glyphNumber'
 *	are `MarkerGlyph1' through `MarkerGlyph16'.
 *
 *	The glyph is defined by a 16 x 16 bit mask, specified in row major
 *	order.  The "hot spot" of the glyph is specified by `xOffset' and
 *	`yOffset'.  These are measured in pixels from the upper left corner
 *	of the glyph, with positive Y increasing downward.  When displayed
 *	as a marker glyph, the hot spot will be positioned at the projected
 *	modelling space marker position.  Marker size scale factor is ignored
 *	when rendering marker glyphs.
 *
 *	The glyphs set by this routine may be selected for drawing
 *	polymarker primitives by calling `PEXSetMarkerType" if the
 *	`PEXMarkerTypeAsf' is `PEXIndividual' or by setting the selected
 *	`PEXMarkerBundleLUT' table entry if `PEXMarkerTypeAsf' is `PEXBundled'.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Value			`glyphNumber' is invalid.
 *
 * SEE ALSO:
 *	`PEXSetMarkerType", `PEXSetTableEntries"
 *
 */

void
MPEXSetMarkerGlyph (display, renderer, glyphNumber, glyph, xOffset, yOffset)

INPUT Display		*display;
INPUT pxlRenderer	renderer;
INPUT int		glyphNumber;
INPUT unsigned short	glyph[16];
INPUT int		xOffset;
INPUT int		yOffset;

{
/* AUTHOR:  Sally C. Barry */
    mpexSetMarkerGlyphReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    MPEXGetReq (SetMarkerGlyph, req);
    req->rdr = renderer;
    req->glyphNum = glyphNumber;
    req->xOffset = xOffset;
    req->yOffset = yOffset;
    COPY_AREA ((char *)glyph, (char *)(req->glyph), 32);

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	MPEXGetMarkerGlyph
 *
 * FORMAT:
 * 	`MPEXGetMarkerGlyph`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer whose marker glyph
 *			is being inquired.
 *
 *	glyphNumber	The number of the marker glyph being inquired.
 *
 *	glyph		Returns the requested marker glyph's
 *			16 x 16 bit mask.
 *
 *	xOffset		Returns a pointer to the X pixel position of the
 *			glyph's hot spot.
 *
 *	yOffset		Returns a pointer to the Y pixel position of the
 *			glyph's hot spot.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine returns the marker glyph associated with `glyphNumber'
 *	for the specified renderer.   Valid values for `glyphNumber' are
 *	`MarkerGlyph16' through `MarkerGlyph16'.  If the specified glyph
 *	number is not supported on
 *	the PEX server extension, a `Value' error is returned by the server.
 *	(See `PEXGetEnumTypeInfo".)
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 *	Value			`glyphNumber' is invalid.
 *
 * SEE ALSO:
 *	`MPEXSetMarkerGlyph"
 *
 */

Status
MPEXGetMarkerGlyph (display, renderer, glyphNumber, glyph, xOffset, yOffset)

INPUT Display		*display;
INPUT pxlRenderer	renderer;
INPUT int		glyphNumber;
OUTPUT unsigned short	glyph[16];
OUTPUT int		*xOffset;
OUTPUT int		*yOffset;

{
/* AUTHOR:  Sally C. Barry */
    mpexGetMarkerGlyphReply		rep;
    mpexGetMarkerGlyphReq		*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    MPEXGetReq (GetMarkerGlyph, req);
    req->rdr = renderer;
    req->glyphNum = glyphNumber;

    if (_XReply (display, &rep, 0, xFalse) == 0)
    { /* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
        return (1);               /* return an error */
    }

    *xOffset = rep.xOffset;
    *yOffset = rep.yOffset;

    _XRead (display, (char *)glyph, (long)(rep.length << 2));

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}
