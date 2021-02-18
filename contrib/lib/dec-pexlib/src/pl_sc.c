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
**	pl_sc.c
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
**	This file contains the routines associated with PEX search contexts.
**
**  Keywords:
**
**  Environment:
**
**	VMS and Ultrix
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
 *   Table of Contents (routine names in order of appearance)
 */

/*
 * I   _PEXGenerateSCList
 * E   PEXCreateSearchContext
 * E   PEXFreeSearchContext
 * E   PEXCopySearchContext
 * E   PEXGetSearchContext
 * E   PEXChangeSearchContext
 * E   PEXSearchNetwork
 */

/*
 *   Include Files
 */

#include "pex_features.h"
#include "pex_macros.h"

#include "PEX.h"
#include "PEXlib.h"
#include "PEXlibint.h"

#include "pl_ref_data.h"

extern	char		*_XAllocScratch();
extern	void		_PEXFreeSCAttr();
extern	void		_PEXFreeSCInfo();

void
_PEXGenerateSCList (display, req, valueMask, values)

INPUT Display		*display;
INPUT pexReq		*req;
INPUT unsigned long	valueMask;
INPUT pxlSCAttributes	*values;

{
    CARD32		*pv;
    CARD32		*pvSend;
    unsigned long       f;
    int			i;
    int			length;
    int			tmp;

    /* check the valueMask and allocate space for the structures in `values'
     * that are being used.
     */
    f = ((sizeof(pxlCoord3D) * ((valueMask & PEXSCPosition) ? 1 : 0)) +
	(sizeof(FLOAT) * ((valueMask & PEXSCDistance) ? 1 : 0)) +
	(sizeof(CARD32) * ((valueMask & PEXSCCeiling) ? 1 : 0)) +
        sizeof(CARD32) +
	(sizeof(pxlElementRef) * ((valueMask & PEXSCStartPath) ?
				values->startPath.pathLength : 0)) +
        sizeof(CARD32) +
	(sizeof(pxlNameSetPair) * ((valueMask & PEXSCNormalList) ?
				values->normalList.numPairs : 0)) +
        sizeof(CARD32) +
	(sizeof(pxlNameSetPair) * ((valueMask & PEXSCInvertedList) ?
				values->invertedList.numPairs : 0)));

    pv = pvSend = (CARD32 *)_XAllocScratch (display, (unsigned long)f);

    for (i=0; i<pxlMaxSCShift; i++)
    {
	f = (1L << i);
	if (valueMask & f)
	{
	    switch (f)
	    {
	    case PEXSCPosition:
		*((pxlCoord3D *)pv) = values->position;
		pv = (CARD32 *)((char *)pv + sizeof(pxlCoord3D));
		break;
	    case PEXSCDistance:
		*((FLOAT *)pv) = values->distance;
		pv = (CARD32 *)((char *)pv + sizeof(FLOAT));
		break;
	    case PEXSCCeiling:
		*((CARD16 *)pv) = values->ceiling;
		pv++;
		break;
	    case PEXSCStartPath:
		tmp = *((CARD32 *)pv) = values->startPath.pathLength;
		pv++;
		tmp *= sizeof(pxlElementRef);
		COPY_AREA ((char *)(values->startPath.elementRefs),
			(char *)pv, tmp);
		pv = (CARD32 *)((char *)pv + tmp);
		break;
	    case PEXSCNormalList:
		tmp = *((CARD32 *)pv) = values->normalList.numPairs;
		pv++;
		tmp *= sizeof(pxlNameSetPair);
		COPY_AREA ((char *)(values->normalList.pairs),
			(char *)pv, tmp);
		pv = (CARD32 *)((char *)pv + tmp);
		break;
	    case PEXSCInvertedList:
		tmp = *((CARD32 *)pv) = values->invertedList.numPairs;
		pv++;
		tmp *= sizeof(pxlNameSetPair);
		COPY_AREA ((char *)(values->invertedList.pairs),
			(char *)pv, tmp);
		pv = (CARD32 *)((char *)pv + tmp);
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
 * 	PEXCreateSearchContext
 *
 * FORMAT:
 * 	`PEXCreateSearchContext`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	valueMask	A mask indicating the fields to be used in the
 *			`values' structure.
 *
 *	values		A pointer to a structure containing values
 *			used to override the default values in the new
 *			search context resource.
 *
 * RETURNS:
 *	The resource ID of the newly-created search context resource.
 *
 * DESCRIPTION:
 *	This routine creates a search context and returns the resource
 *	identifier of the created search context resource.
 *	Each bit in the `valueMask' bitmask corresponds to a field in the
 *	structure pointed to by `values'.
 *	search context attribute.  If a bit is set, the new search context
 *	value is set to the value specified in `values'.
 *	The `PEX.h' include file contains a list of constants used to
 *	create the bitmask.
 *	Fields not explicitly set have default values.  The `PEXlib.h'
 *	include file defines the `pxlSCAttributes' structure.
 *
 * ERRORS:
 *	IDChoice		ID already in use or not in range assigned to
 *				client.
 *
 *	Value			An item in the item list is out of range.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	Alloc			Server failed to allocate the resource.
 *
 * SEE ALSO:
 *	`PEXCopySearchContext", `PEXChangeSearchContext", `PEXFreeSearchContext",
 *	`PEXGetSearchContext", `PEXSearchNetwork"
 *
 */

pxlSearchContext
PEXCreateSearchContext (display, valueMask, values)

INPUT Display		*display;
INPUT unsigned long	valueMask;
INPUT pxlSCAttributes	*values;

{
/* AUTHOR:  Sanjiv Maewall */
    pxlSearchContext		id;
    pexCreateSearchContextReq	*req;

    /* get a search context resource id from X */
    id = XAllocID (display);

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (CreateSearchContext, req);
    req->fpFormat = pexFpFormat;
    req->sc = id;
    req->itemMask = valueMask;

    _PEXGenerateSCList (display, (pexReq *)req, valueMask, values);

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (id);
}

/*+
 * NAME:
 * 	PEXFreeSearchContext
 *
 * FORMAT:
 * 	`PEXFreeSearchContext`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	sc		The resource ID of the search context to
 *			be freed.
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *  	This routine deletes the search context resource `sc' and frees the
 *	storage associated with it.
 *
 * ERRORS:
 *	SearchContext	Specified search context resource ID is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateSearchContext", `PEXCopySearchContext", `PEXChangeSearchContext",
 *	`PEXGetSearchContext", `PEXSearchNetwork"
 *
 */

void
PEXFreeSearchContext (display, sc)

INPUT Display		*display;
INPUT pxlSearchContext	sc;

{
/* AUTHOR:  Sanjiv Maewall */
    pexFreeSearchContextReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (FreeSearchContext, req);
    req->id = sc;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXCopySearchContext
 *
 * FORMAT:
 * 	`PEXCopySearchContext`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	valueMask	A bitmask specifying which attributes
 *			are to be copied.
 *
 *	srcSc		The resource ID of the search context to be copied.
 *
 *	destSc		The resource ID of the search context to which
 *			attributes are to be copied.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *  	This routine copies attributes from the source search context `srcSc'
 *	to the destination search context resource `destSc'.  Both must
 *	already exist as valid search context resources.
 *	Attributes indicated by ones in `valueMask' are copied and the
 *	remainder of the attributes are left as they were.
 *	The `PEX.h' include file contains constants that can be used to
 *	construct `valueMask'.
 *
 * ERRORS:
 *	SearchContext	Specified search context resource ID is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateSearchContext", `PEXChangeSearchContext", `PEXFreeSearchContext",
 *	`PEXGetSearchContext", `PEXSearchNetwork"
 *
 */

void
PEXCopySearchContext (display, valueMask, srcSc, destSc)

INPUT Display		*display;
INPUT unsigned long	valueMask;
INPUT pxlSearchContext	srcSc;
INPUT pxlSearchContext	destSc;

{
/* AUTHOR:  Sanjiv Maewall */
    pexCopySearchContextReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (CopySearchContext, req);
    req->src = srcSc;
    req->dst = destSc;
    req->itemMask = valueMask;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXGetSearchContext
 *
 * FORMAT:
 * 	`PEXGetSearchContext`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	sc		The resource ID of the search context to be queried.
 *
 *	valueMask	A mask indicating which attributes are to be returned.
 *
 *	scAttributesReturn Returns a pointer to a structure that contains
 *			the requested attributes of the search context.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *  	This routine returns the requested components of the search context
 *	resource specified by `sc'.  Each bit in the bitmask array
 *	`valueMask' corresponds to a search context attribute.  If
 *	the bit is set, the value is returned in the structure pointed
 *	at by `scAttributesReturn'.
 *
 *	PEXlib allocates storage for the returned search context attributes.
 *	`PEXFree"(`*scAttributesReturn') deallocates the storage.
 *
 * ERRORS:
 *	SearchContext		Specified search context resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 * SEE ALSO:
 *	`PEXCreateSearchContext", `PEXCopySearchContext",  `PEXChangeSearchContext",
 *	`PEXFreeSearchContext",  `PEXSearchNetwork"
 *
 */

Status
PEXGetSearchContext (display, sc, valueMask, scAttributesReturn)

INPUT Display		*display;
INPUT pxlSearchContext	sc;
INPUT unsigned long	valueMask;
OUTPUT pxlSCAttributes	**scAttributesReturn;

{
/* AUTHOR:  Sanjiv Maewall */
    pexGetSearchContextReply	rep;
    pexGetSearchContextReq	*req;
    pxlSCAttributes		*scattr;
    int				i;
    int				j;
    unsigned long		*pv;
    unsigned long		f;
    int				tmp;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (GetSearchContext, req);
    req->fpFormat = (pexEnumTypeIndex)pexFpFormat;
    req->sc = sc;
    req->itemMask = valueMask;

    if (_XReply (display, &rep, 0, xFalse) == 0)
    { /* there was an error */
	UnlockDisplay (display);
  	PEXSyncHandle ();
 	return (1);               /* return an error */
    }

    pv = (unsigned long *)_XAllocScratch (display,
	(unsigned long)(rep.length << 2));
    _XRead (display, (char *)pv, (long)(rep.length << 2));
    scattr = (pxlSCAttributes *)PEXAllocBuf
		((unsigned)(sizeof(pxlSCAttributes) + sizeof(PEXHeader)));
    ((PEXHeader *)scattr)->free = _PEXFreeSCAttr;
    scattr = (pxlSCAttributes *)&((PEXHeader *)scattr)[1];

    *scAttributesReturn = scattr;

    /* copy the search context attributes to the buffer */
    for (i=0; i<pxlMaxSCShift; i++)
    {
	f = (1L << i);
	if (valueMask & f)
	{
	    switch (f)
	    {
	    case PEXSCPosition:
		scattr->position = *((pxlCoord3D *)pv);
		pv = (unsigned long *)((char *)pv + sizeof(pxlCoord3D));
	  	break;
	    case PEXSCDistance:
		scattr->distance = *((FLOAT *)pv);
		pv = (unsigned long *)((char *)pv + sizeof(FLOAT));
		break;
	    case PEXSCCeiling:
		scattr->ceiling = *((CARD16 *)pv);
		pv++;
		break;
	    case PEXSCStartPath:
		tmp = *(int *)pv;
		pv++;
		scattr->startPath.pathLength = tmp;
		tmp *= sizeof(pxlElementRef);
		scattr->startPath.elementRefs = (pxlElementRef *)
				PEXAllocBuf ((unsigned)tmp);
		COPY_AREA ((char *)pv, (char *)(scattr->startPath.elementRefs),
			tmp);
		pv = (unsigned long *)((char *)pv + tmp);
		break;
	    case PEXSCNormalList:
		tmp = *(int *)pv;
		pv++;
		scattr->normalList.numPairs = tmp;
		tmp *= sizeof(pxlNameSetPair);
		scattr->normalList.pairs = (pxlNameSetPair *)
				PEXAllocBuf ((unsigned)tmp);
		COPY_AREA ((char *)pv, (char *)(scattr->normalList.pairs), tmp);
		pv = (unsigned long *)((char *)pv + tmp);
		break;
	    case PEXSCInvertedList:
		tmp = *(int *)pv;
		pv++;
		scattr->invertedList.numPairs = tmp;
		tmp *= sizeof(pxlNameSetPair);
		scattr->invertedList.pairs = (pxlNameSetPair *)
				PEXAllocBuf ((unsigned)tmp);
		COPY_AREA ((char *)pv, (char *)(scattr->invertedList.pairs),
			tmp);
		pv = (unsigned long *)((char *)pv + tmp);
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
 * 	PEXChangeSearchContext
 *
 * FORMAT:
 * 	`PEXChangeSearchContext`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	sc		The resource ID of the search context to be changed.
 *
 *	valueMask	A mask indicating the search context attributes
 *			to be changed.
 *
 *	values		A structure containing the new
 *			values for the specified search context attributes.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine modifies the attributes of the search context resource
 *	`sc'.  Each bit in the bitmask `valueMask' corresponds to a search
 *	context attribute.  If the bit is set, the value in the search context
 *	is changed to the value specified in `values'.  The `PEX.h' include
 *	file contains constants that can be used to construct `valueMask'
 *	(for example, `PEXSCPosition', `PEXSCCeiling',  and so on.)
 *
 * ERRORS:
 *	SearchContext		Specified search context resource ID is invalid.
 *
 *	Value			An item in the item list is out of range.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 * SEE ALSO:
 *	`PEXCreateSearchContext", `PEXCopySearchContext", `PEXFreeSearchContext",
 *	`PEXGetSearchContext", `PEXSearchNetwork"
 *
 */

void
PEXChangeSearchContext (display, sc, valueMask, values)

INPUT Display		*display;
INPUT pxlSearchContext	sc;
INPUT unsigned long	valueMask;
OUTPUT pxlSCAttributes	*values;

{
/* AUTHOR:  Sanjiv Maewall */
    pexChangeSearchContextReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (ChangeSearchContext, req);
    req->fpFormat = pexFpFormat;
    req->sc = sc;
    req->itemMask = valueMask;

    _PEXGenerateSCList (display, (pexReq *)req, valueMask, values);

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXSearchNetwork
 *
 * FORMAT:
 * 	`PEXSearchNetwork`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	sc		The resource ID of the search context used to
 *			perform the search.
 *
 *	pathReturn	Returns the structure network path of the first
 *			structure element found that meets the search criteria.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine performs a spatial search in world coordinates on a
 *	structure network.  The search operation parameters are found
 *	in the search context `sc'.  The search begins at the element
 *	following the one indicated by the start path attribute of
 *	`sc'.  The search is terminated when a successful match occurs,
 *	or when the end of the structure indicated by the
 *	search ceiling attribute has been reached.  The structure network
 *	path of the first element that
 *	meets the search criteria is returned in `pathReturn'.
 *
 *	PEXlib allocates storage for the structure network path
 *	pointed to by `pathReturn'.  `PEXFree"(`*pathReturn') deallocates
 *	the storage.
 *
 * ERRORS:
 *	SearchContext	Specified search context resource ID is invalid.
 *
 * SEE ALSO:
 * 	`PEXCreateSearchContext", `PEXCopySearchContext", `PEXChangeSearchContext",
 *	`PEXFreeSearchContext",  `PEXGetSearchContext"
 *
 */

Status
PEXSearchNetwork (display, sc, pathReturn)

INPUT Display		*display;
INPUT pxlSearchContext	sc;
OUTPUT pxlStructurePath	**pathReturn;

{
/* AUTHOR:  Sanjiv Maewall */
    pexSearchNetworkReply	rep;
    pexSearchNetworkReq		*req;
    pxlStructurePath		*scinfo;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (SearchNetwork, req);
    req->id = sc;

    if (_XReply (display, &rep, 0, xFalse) == 0)
    { /* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
        return (1);               /* return an error */
    }

    /* allocate a buffer for the path to pass back to the client */
    scinfo = (pxlStructurePath *)PEXAllocBuf
		((unsigned)(sizeof(PEXHeader) + sizeof(pxlStructurePath)));
    ((PEXHeader *)scinfo)->free = _PEXFreeSCInfo; /* routine to free it */
    scinfo = (pxlStructurePath *)&((PEXHeader *)scinfo)[1];
    *pathReturn = scinfo;

    scinfo->pathLength = rep.numItems;
    scinfo->elementRefs = (pxlElementRef *)PEXAllocBuf
		((unsigned)(rep.numItems * sizeof(pxlElementRef)));

    _XRead (display, (char *)(scinfo->elementRefs), (long)(rep.length << 2));

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}
