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
**	pl_rdr.c
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
**	This file contains the routines associated with PEX renderers.
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
**	18-Aug-88 
**--
*/

/*
 *   Table of Contents (routine names in order of appearance)
 */

/*
 * E   PEXCreateRenderer
 * I   _PEXGenerateRendererList
 * E   PEXFreeRenderer
 * E   PEXGetRendererAttributes
 * E   PEXGetRendererDynamics
 * E   PEXChangeRenderer
 * E   PEXBeginRendering
 * E   PEXEndRendering
 * E   PEXBeginStructure
 * E   PEXEndStructure
 * E   PEXRenderNetwork
 */

/*
 *   Include Files
 */

#include "pex_features.h"
#include "pex_macros.h"

#include "PEXlib.h"
#include "PEXlibint.h"

#include "pl_ref_data.h"

#ifdef MPEX
#include "MPEX.h"
#include "MPEXproto.h"
#endif

extern	char		*_XAllocScratch();

/* routine to write a packed list of renderer attributes to the transport buf */
INTERNAL void
_PEXGenerateRendererList (display, req, valueMask, values)

INPUT Display             	*display;
INPUT pexReq		    	*req;
INPUT unsigned long       	valueMask;
INPUT pxlRendererAttributes 	*values;

{
    CARD32		*pv, *pvSend;
    unsigned long	f;
    int			i, j, n, length;

    CountOnes (valueMask, n);

    f =  n * sizeof(CARD32) +
	sizeof(pexNpcSubvolume) +
	sizeof(pexViewport) +
	2*sizeof(short);

    if (valueMask & PEXRDClipList)
    {
	f += values->clipList.numDeviceRects * sizeof(pexDeviceRect);
    }

    pv = pvSend = (CARD32 *)_XAllocScratch (display, (unsigned long)f);

    for (i=0; i<(PEXMaxRDShift + 1); i++)
    {
	f = (1L << i);
	if (valueMask & f)
	{
            switch (f)
	    {
            case PEXRDCurrentPath:
		/* current path doesn't make sense in a new or changed
		   renderer, so ignore it.  this is for client-side traversal
		   and is updated by begin-structure and processing of 
		   subsequent output commands and cleared by end-structure */
		break;
            case PEXRDPipelineContext:
		*((pexPC *)pv) = values->pipelineContext;
		pv++;
		break;
            case PEXRDMarkerBundle:
		*((pexLookupTable *)pv) = values->markerBundle;
		pv++;
		break;
            case PEXRDTextBundle:
		*((pexLookupTable *)pv) = values->textBundle;
		pv++;
		break;
            case PEXRDLineBundle:
		*((pexLookupTable *)pv) = values->lineBundle;
		pv++;
		break;
            case PEXRDInteriorBundle:
		*((pexLookupTable *)pv) = values->interiorBundle;
		pv++;
		break;
            case PEXRDEdgeBundle:
		*((pexLookupTable *)pv) = values->edgeBundle;
		pv++;
		break;
            case PEXRDViewTable:
		*((pexLookupTable *)pv) = values->viewTable;
		pv++;
		break;
            case PEXRDColourTable:
		*((pexLookupTable *)pv) = values->colourTable;
		pv++;
		break;
            case PEXRDDepthCueTable:
		*((pexLookupTable *)pv) = values->depthCueTable;
		pv++;
		break;
            case PEXRDLightTable:
		*((pexLookupTable *)pv) = values->lightTable;
		pv++;
		break;
            case PEXRDColourApproxTable:
		*((pexLookupTable *)pv) = values->colourApproxTable;
		pv++;
		break;
            case PEXRDPatternTable:
		*((pexLookupTable *)pv) = values->patternTable;
		pv++;
		break;
            case PEXRDTextFontTable:
		*((pexLookupTable *)pv) = values->textFontTable;
		pv++;
		break;
            case PEXRDHighlightIncl:
		*((pexNameSet *)pv) = values->highlightIncl;
		pv++;
		break;
            case PEXRDHighlightExcl:
		*((pexNameSet *)pv) = values->highlightExcl;
		pv++;
		break;
            case PEXRDInvisibilityIncl:
		*((pexNameSet *)pv) = values->invisibilityIncl;
		pv++;
		break;
            case PEXRDInvisibilityExcl:
		*((pexNameSet *)pv) = values->invisibilityExcl;
		pv++;
		break;
	    case PEXRDRendererState:
		/* renderer state doesn't make sense in a new or changed
		   renderer, so ignore it.  this is set to 'rendering' by
		   begin-rendering and set to idle by 'end-rendering' */
		break;
            case PEXRDHlhsrMode:
		*((INT16 *)pv) = values->hlhsrMode;
		pv++;
		break;
            case PEXRDNpcSubvolume:
		*((pexNpcSubvolume *)pv) = values->npcSubvolume;
		pv += NUMWORDS( sizeof(pexNpcSubvolume));
		break;
            case PEXRDViewport:
		*((pexViewport *)pv) = values->viewport;
		pv += NUMWORDS( sizeof(pexViewport));
		break;
	    case PEXRDClipList:
		length = values->clipList.numDeviceRects;
		*((long *)pv) = length;
		pv++;
		length *= sizeof(pexDeviceRect);
		COPY_AREA ((char *)(values->clipList.deviceRect),
			(char *)pv, length);
		pv += NUMWORDS( length);
		break;
	    }
	}
    }

    length = pv - pvSend;
    req->length += length;
    /* put the data in the X buffer */
    Data (display, (char *)pvSend, (length << 2));

    return;
}

/*+
 * NAME:
 * 	PEXCreateRenderer
 *
 * FORMAT:
 * 	`PEXCreateRenderer`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	d		The resource ID of an X window or pixmap.
 *
 *	valueMask	A mask indicating the fields to be used in the `values'
 *			structure.
 *
 *	values		A pointer to a structure containing values used
 *			to override the default values in the new renderer
 *			resource.
 *
 * RETURNS:
 *	The resource ID of the newly-created renderer resource.
 *
 * DESCRIPTION:
 *	This routine creates a renderer and returns the resource ID of the
 *	newly-created renderer resource.  Each bit in the
 *	`valueMask' bitmask corresponds to a field in the structure pointed to
 *	by `values'.  If a bit is set, the new renderer value
 *	is set to the value specified in `values'.  Include file `PEX.h'
 *	contains a list of constants used to create the bitmask.
 *	Fields not explicitly set have default values.
 *	The renderer resource may only be used in conjunction
 *	with drawables that have the same root and depth as `d'.
 *
 *	Note that the renderer's `CurrentPath' and `RendererState'
 *	attributes cannot be set explicitly using this routine.
 *
 * ERRORS:
 *	IDChoice		ID already in use or not in range assigned to
 *				client.
 *
 *	Drawable		Specified drawable resource ID is invalid.
 *
 *	PipelineContext		Pipeline context resource ID is invalid.
 *
 *	NameSet			Specified name set resource ID is invalid.
 *
 *	LookupTable		Specified lookup table resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified fp format.
 *
 *	Value			An item in the item list is out of range.
 *
 *	Alloc			Server failed to allocate the resource.
 *
 *	Match			Lookup table root or depth does not match
 *				example drawable's root or depth.
 *
 * SEE ALSO:
 *	`PEXFreeRenderer", `PEXGetRendererAttributes", `PEXChangeRenderer",
 *	`PEXGetRendererDynamics", `PEXBeginRendering", `PEXEndRendering"
 *
 */

pxlRenderer
PEXCreateRenderer (display, d, valueMask, values)

INPUT Display			*display;
INPUT Drawable			d;
INPUT unsigned long		valueMask;
INPUT pxlRendererAttributes 	*values;

{
/* AUTHOR: */
    pexCreateRendererReq	*req;
    pxlRenderer			rdr;

    /* get a renderer resource id from X */
    rdr = XAllocID (display);

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (CreateRenderer, req);
    req->fpFormat = (pexEnumTypeIndex)pexFpFormat;
    req->drawable = d;
    req->rdr = rdr;
    /* Turn off these bits.  They are not modifiable attributes and would mess
       up the count */
    valueMask &= ~(PEXRDCurrentPath | PEXRDRendererState);
    req->itemMask = valueMask;

    _PEXGenerateRendererList (display, (pexReq *)req, valueMask, values);

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (rdr);
}

/*+
 * NAME:
 * 	PEXFreeRenderer
 *
 * FORMAT:
 * 	`PEXFreeRenderer`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer to be freed.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine deletes the renderer resource specified by `renderer' and
 *	frees the storage associated with it.
 *
 * ERRORS:
 *	Renderer	Specified renderer resource ID is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateRenderer", `PEXGetRendererAttributes", `PEXChangeRenderer",
 *	`PEXGetRendererDynamics", `PEXBeginRendering", `PEXEndRendering"
 *
 */

void
PEXFreeRenderer (display, renderer)

INPUT Display		*display;
INPUT pxlRenderer	renderer;

{
/* AUTHOR: */
    pexFreeRendererReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (FreeRenderer, req);
    req->id = renderer;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXGetRendererAttributes
 *
 * FORMAT:
 * 	`PEXGetRendererAttributes`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer to be queried.
 *
 *	valueMask	A bitmask indicating attributes to be
 *			returned from the renderer.
 *
 *	valuesReturn	Returns a pointer to a structure that contains the
 *			specified attributes from the renderer.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine returns attributes from the specified `renderer' resource.
 *	Each bit in the bitmask array `valueMask' indicates whether
 *	the corresponding entry is to be returned in the structure pointed
 *	to by `valuesReturn'.  If a bit is set, the value in the renderer
 *	is returned in `valuesReturn'.  Include file `PEX.h'
 *	contains a list of constants used to create the bitmask.
 *
 *	PEXlib allocates the storage for the returned renderer attributes.
 *	`PEXFree"(`*valuesReturn') deallocates the memory.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 * 	FloatingPointFormat	Device does not support the specified fp format.
 *
 * SEE ALSO:
 *	`PEXCreateRenderer", `PEXFreeRenderer", `PEXChangeRenderer",
 *	`PEXGetRendererDynamics", `PEXBeginRendering", `PEXEndRendering"
 *
 */

Status
PEXGetRendererAttributes (display, renderer, valueMask, valuesReturn)

INPUT Display			*display;
INPUT pxlRenderer		renderer;
INPUT unsigned long		valueMask;
OUTPUT pxlRendererAttributes 	**valuesReturn;

{
/* AUTHOR: */
    unsigned long			*pv;
    unsigned long			f;
    int					i;
    int					j;
    int					n;
    int					size;
    int					length;
    pxlRendererAttributes		*prdra;
    pexGetRendererAttributesReq		*req;
    pexGetRendererAttributesReply	rep;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (GetRendererAttributes, req);
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
    prdra = (pxlRendererAttributes *) PEXAllocBuf
        ((unsigned)(sizeof(PEXHeader) + sizeof(pxlRendererAttributes)));
    ((PEXHeader *)prdra)->free = _PEXFreeRdrAttr; /* routine to free it */
    prdra = (pxlRendererAttributes *)&(((PEXHeader *)prdra)[1]);

    *valuesReturn = prdra;

    /* copy the renderer attributes to the buffer */
    for (i=0; i<(PEXMaxRDShift + 1); i++)
    {
	f = (1L << i);
	if (valueMask & f)
	{
            switch (f)
	    {
            case PEXRDCurrentPath:
		n = *(int *)pv;
		pv++;
		prdra->currentPath.pathLength = n;
		size = n * sizeof(pxlElementRef);
		prdra->currentPath.elementRefs =
			(pxlElementRef *)PEXAllocBuf ((unsigned)size);
		COPY_AREA ((char *)pv, (char *)prdra->currentPath.elementRefs, 
			size);
		pv += NUMWORDS( size);
		break;
            case PEXRDPipelineContext:
		prdra->pipelineContext = *((pxlPipelineContext *)pv);
		pv++;
		break;
            case PEXRDMarkerBundle:
		prdra->markerBundle = *((pxlLookupTable *)pv);
		pv++;
		break;
            case PEXRDTextBundle:
		prdra->textBundle = *((pxlLookupTable *)pv);
		pv++;
		break;
            case PEXRDLineBundle:
		prdra->lineBundle = *((pxlLookupTable *)pv);
 		pv++;
		break;
            case PEXRDInteriorBundle:
		prdra->interiorBundle = *((pxlLookupTable *)pv);
		pv++;
		break;
            case PEXRDEdgeBundle:
		prdra->edgeBundle = *((pxlLookupTable *)pv);
		pv++;
		break;
            case PEXRDViewTable:
		prdra->viewTable = *((pxlLookupTable *)pv);
		pv++;
		break;
            case PEXRDColourTable:
		prdra->colourTable = *((pxlLookupTable *)pv);
		pv++;
		break;
            case PEXRDDepthCueTable:
		prdra->depthCueTable = *((pxlLookupTable *)pv);
		pv++;
		break;
            case PEXRDLightTable:
		prdra->lightTable = *((pxlLookupTable *)pv);
		pv++;
		break;
            case PEXRDColourApproxTable:
		prdra->colourApproxTable = *((pxlLookupTable *)pv);
		pv++;
		break;
            case PEXRDPatternTable:
		prdra->patternTable = *((pxlLookupTable *)pv);
		pv++;
		break;
            case PEXRDTextFontTable:
		prdra->textFontTable = *((pxlLookupTable *)pv);
		pv++;
		break;
            case PEXRDHighlightIncl:
		prdra->highlightIncl = *((pxlNameSet *)pv);
		pv++;
		break;
            case PEXRDHighlightExcl:
		prdra->highlightExcl = *((pxlNameSet *)pv);
		pv++;
		break;
            case PEXRDInvisibilityIncl:
		prdra->invisibilityIncl = *((pxlNameSet *)pv);
		pv++;
		break;
            case PEXRDInvisibilityExcl:
		prdra->invisibilityExcl = *((pxlNameSet *)pv);
		pv++;
		break;
	    case PEXRDRendererState:
		prdra->rendererState = *((CARD16 *)pv);
		pv++;
		break;
            case PEXRDHlhsrMode:
		prdra->hlhsrMode = *((INT16 *)pv);
		pv++;
		break;
            case PEXRDNpcSubvolume:
		prdra->npcSubvolume = *((pxlNpcSubvolume *)pv);
		pv += NUMWORDS( sizeof(pexNpcSubvolume));
		break;
            case PEXRDViewport:
		prdra->viewport = *((pxlViewport *)pv);
		pv += NUMWORDS( sizeof(pexViewport));
		break;
	    case PEXRDClipList:
		n = *(int *)pv;
		pv++;
		prdra->clipList.numDeviceRects = n;
		size = n * sizeof(pxlDeviceRect);
		prdra->clipList.deviceRect =
			(pxlDeviceRect *)PEXAllocBuf ((unsigned)size);
		COPY_AREA ((char *)pv, (char *)(prdra->clipList.deviceRect),
			size);
		pv += NUMWORDS( size);
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
 * 	PEXGetRendererDynamics
 *
 * FORMAT:
 * 	`PEXGetRendererDynamics`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer to be queried.
 *
 *	tablesReturn	Returns a pointer to a bitmask that describes the
 *			dynamics of lookup tables associated with the renderer.
 *
 *	namesetsReturn	Returns a pointer to a bitmask that describes the
 *			dynamics of namesets associated with the renderer.
 *
 *	attributesReturn	Returns a pointer to a bitmask that describes
 *			the dynamics of other attributes associated with the
 *			renderer.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine returns the dynamics (binding times) for all of the
 *	attributes of the specified `renderer' resource.  Each bit in the
 *	returned bitmasks indicates the dynamics for a particular attribute
 *	of the renderer.  For each bit, a value of zero indicates that the
 *	specified attribute may be modified at any time, and the change will
 *	take place immediately.  A value of one indicates that the specified
 *	attribute may not be modified dynamically.  In this case, the change
 *	is "pending" and will take effect at the next explicit or implicit
 *	"begin rendering".
 *
 *	Include file `PEX.h' contains a list of constants for the bitmasks.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource ID is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateRenderer", `PEXFreeRenderer", `PEXChangeRenderer",
 *	`PEXGetRendererAttributes", `PEXBeginRendering", `PEXEndRendering"
 *
 */

Status
PEXGetRendererDynamics (display, renderer, tablesReturn, namesetsReturn,
			attributesReturn)

INPUT Display			*display;
INPUT pxlRenderer		renderer;
OUTPUT unsigned long		*tablesReturn;
OUTPUT unsigned long		*namesetsReturn;
OUTPUT unsigned long		*attributesReturn;

{
/* AUTHOR: */
    pexGetRendererDynamicsReq		*req;
    pexGetRendererDynamicsReply		rep;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (GetRendererDynamics, req);
    req->id = renderer;

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
        return (1);            /* return an error */
    }

    /* get the return values */
    *tablesReturn = rep.tables;
    *namesetsReturn = rep.namesets;
    *attributesReturn = rep.attributes;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	PEXChangeRenderer
 *
 * FORMAT:
 * 	`PEXChangeRenderer`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of the renderer to be changed.
 *
 *	valueMask	A mask indicating the renderer attributes
 *			to be changed.
 *
 *	values		A pointer to a structure containing
 *			new values for the specified renderer attributes.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine modifies attributes of the specified `renderer' resource.
 *	Each bit in the bitmask array `valueMask' indicates whether
 *	the corresponding entry in `values' is to be used to modify
 * 	the renderer resource.  If a bit is set, the attribute in the
 *	renderer is set to the value specified in `values'.  Include file
 *	`PEX.h' contains a list of constants used to create the bitmask,
 *	for example `PEXRDPipelineContext' and `PEXRDViewTable'.
 *
 *	Note that the renderer's `CurrentPath' and `RendererState'
 *	attributes cannot be set explictly using this routine.
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
 *	FloatingPointFormat	Device does not support the specified fp format.
 *
 *	NameSet			A specified name set resource is invalid.
 *
 *	LookupTable		A specified lookup table resource is invalid.
 *
 *	PipelineContext		Specified pipeline context resource is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateRenderer", `PEXFreeRenderer", `PEXGetRendererAttributes",
 *	`PEXGetRendererDynamics", `PEXBeginRendering", `PEXEndRendering"
 *
 */

void
PEXChangeRenderer (display, renderer, valueMask, values)

INPUT Display			*display;
INPUT pxlRenderer		renderer;
INPUT unsigned long		valueMask;
INPUT pxlRendererAttributes 	*values;

{
/* AUTHOR: */
    pexChangeRendererReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (ChangeRenderer, req);
    req->fpFormat = pexFpFormat;
    req->rdr = renderer;
    /* Turn off these bits.  They are not modifiable attributes and would mess
       up the count */
    valueMask &= ~(PEXRDCurrentPath | PEXRDRendererState);
    req->itemMask = valueMask;

    _PEXGenerateRendererList (display, (pexReq *)req, valueMask, values);

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXBeginRendering
 *
 * FORMAT:
 * 	`PEXBeginRendering`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	d		The resource ID of an X window or pixmap.
 *
 *	renderer	The resource ID of a renderer.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine initializes the specified `renderer' and binds the
 *	specified drawable to it.  Subsequent output primitive commands
 *	sent to `renderer' produce output on the drawable `d'.
 *	The renderer's pipeline state is initialized to the values
 *	in its `PipelineContext' attribute, or to their default values
 *	if `PipelineContext'=`NULL'.  This routine causes the renderer's
 *	`RendererState' attribute to be set to `PEXRendering' and its
 *	`CurrentPath' attribute to be set to `NULL'.  The renderer's
 *	`HlhsrMode' is used to initialize hidden surface computations.
 *	(For example, the Z-buffer is initialized if the `HlhsrMode' is
 *	set to `HlhsrZBuffer'.)
 *
 *	If the renderer's state is `PEXRendering' or 
 *	`MPEXPicking' when this request is received, an implicit "end rendering"
 * 	request is performed before the "begin rendering" request is executed.  
 *	Output commands received by a renderer are ignored if the state is 
 *	not `PEXRendering' or `MPEXPicking'.
 *
 * ERRORS:
 *	Renderer	Specified renderer resource ID is invalid.
 *
 *	Drawable	Specified drawable resource ID is invalid.
 *
 *	Match		Specified renderer resource was not created for
 *			drawables of the same root and depth as the specified
 *			drawable.
 *
 * SEE ALSO:
 *	`PEXCreateRenderer", `PEXEndRendering"
 *
 */

void
PEXBeginRendering (display, d, renderer)

INPUT Display		*display;
INPUT Drawable		d;
INPUT pxlRenderer	renderer;

{
/* AUTHOR: */
    pexBeginRenderingReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (BeginRendering, req);
    req->rdr = renderer;
    req->drawable = d;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXEndRendering
 *
 * FORMAT:
 * 	`PEXEndRendering`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure returned
 *			by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of a renderer.
 *
 *	flush		A Boolean operator specifying whether any pending
 *			output for `renderer' is to be rendered onto its
 *			associated drawable.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine terminates rendering on the specified `renderer' resource.
 *	If `flush' is `True', pending output for `renderer' is rendered onto its
 *	associated drawable.  If `flush is `False', pending output is discarded.
 *	In either case, the `RendererState' attribute of the
 *	renderer is set to `PEXIdle'.
 *
 * ERRORS:
 *	Renderer	Specified renderer resource ID is invalid.
 *
 *	RendererState	Renderer is already in the `PEXIdle' state and `flush' is `True'.
 *
 * SEE ALSO:
 *	`PEXCreateRenderer", `PEXBeginRendering"
 *
 */

void
PEXEndRendering (display, renderer, flush)

INPUT Display		*display;
INPUT pxlRenderer	renderer;
INPUT Bool		flush;

{
/* AUTHOR: */
    pexEndRenderingReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (EndRendering, req);
    req->rdr = renderer;
    req->flushFlag = flush;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXBeginStructure
 *
 * FORMAT:
 * 	`PEXBeginStructure`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	The resource ID of a renderer.
 *
 *	id		An application-specified structure ID.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	Application programs can use this routine to simulate the
 *	side effects of the "execute structure" output command.  This request
 *	saves the rendering pipeline attributes in the specified
 *	`renderer'.  This request also increments the element offset
 *	of the last entry in the renderer's `CurrentPath' attribute.
 *	The routine then adds the structure name `id' and an element offset
 *	of zero to the renderer's `CurrentPath' attribute.
 *
 *	The renderering pipeline's global transform attribute is set to the
 *	matrix computed by concatenating the current local transform
 *	and current global transform matrices.  The local transform
 *	matrix is then set to the identity matrix.
 *
 * ERRORS:
 *	Renderer	Specified renderer resource ID is invalid.
 *
 * SEE ALSO:
 *	`PEXEndStructure", `PEXExecuteStructure", `PEXCreateRenderer"
 *
 */

void
PEXBeginStructure (display, renderer, id)

INPUT Display		*display;
INPUT pxlRenderer	renderer;
INPUT unsigned long	id;

{
/* AUTHOR: */
    pexBeginStructureReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (BeginStructure, req);
    req->rdr = renderer;
    req->sid = id;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXEndStructure
 *
 * FORMAT:
 * 	`PEXEndStructure`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	renderer	An unsigned longword specifying the resource
 *			ID of the renderer whose last-saved attributes
 *			are to be restored.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	Application programs can use this routine to simulate the side effects
 *	of the return from an "execute structure" output command.  This request
 *	restores the last-saved rendering pipeline attributes in `renderer'.
 *	This request also removes the last element reference in the renderer's
 *	`CurrentPath' attribute.  Subsequent output commands cause the element
 *	offset of the element reference at the end of the list to be
 *	incremented.
 *
 * ERRORS:
 *	Renderer	Specified renderer resource ID is invalid.
 *
 * SEE ALSO:
 *	`PEXBeginStructure", `PEXExecuteStructure", `PEXCreateRenderer"
 *
 */

void
PEXEndStructure (display, renderer)

INPUT Display		*display;
INPUT pxlRenderer	renderer;

{
/* AUTHOR: */
    pexEndStructureReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (EndStructure, req);
    req->id = renderer;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXRenderNetwork
 *
 * FORMAT:
 * 	`PEXRenderNetwork`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	d		The resource ID of an X window or pixmap.
 *
 *	renderer	The resource ID of a renderer resource.
 *
 *	structure	The resource ID of a structure resource.
 *
 * RETURNS:
 * 	None
 *
 * DESCRIPTION:
 *  	This routine executes all output commands stored in `structure' using
 *	the specified `renderer'.  Output primitives in `structure' are
 *	rendered on the drawable `d'.  Structures referenced
 *	through "execute structure" output commands are also executed.
 *	This request effectively performs an implicit `PEXBeginRendering"
 *	before the traversal of the specified structure network. This request
 *	performs an implicit `PEXEndRendering" after the traversal.
 *
 * ERRORS:
 *	Renderer	Specified renderer resource ID is invalid.
 *
 *	Drawable	Specified drawable resource ID is invalid.
 *
 *	Structure	Specified structure resource ID is invalid.
 *
 * SEE ALSO:
 *	`PEXExecuteStructure", `PEXBeginRendering", `PEXEndRendering",
 *	`PEXCreateStructure", `PEXCreateRenderer"
 *
 */

void
PEXRenderNetwork (display, d, renderer, structure)

INPUT Display		*display;
INPUT Drawable		d;
INPUT pxlRenderer	renderer;
INPUT pxlStructure	structure;

{
/* AUTHOR: */
    pexRenderNetworkReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (RenderNetwork, req);
    req->rdr = renderer;
    req->drawable = d;
    req->sid = structure;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}
