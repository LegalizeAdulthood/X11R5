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
**	pl_struct.c
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
**	This file contains the routines associated with PEX structures.
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
**
**--
*/

/*
 *   Table of Contents (routine names in order of appearance)
 */

/*
 * E   PEXCreateStructure
 * E   PEXDestroyStructures
 * E   PEXCopyStructure
 * E   PEXGetStructureInfo
 * E   PEXGetElementInfo
 * E   PEXGetStructuresInNetwork
 * E   PEXGetAncestors
 * E   PEXGetDescendants
 * E   PEXFetchElList
 * E   PEXSetEditingMode
 * E   PEXSetElementPtr
 * E   PEXSetElementPtrAtLabel
 * E   PEXElementSearch
 * E   PEXDeleteElements
 * E   PEXDeleteToLabel
 * E   PEXDeleteBetweenLabels
 * E   PEXCopyElements
 * E   PEXChangeStructureRefs
 */

/*
 *   Include Files
 */

#include "pex_features.h"
#include "pex_macros.h"

#include "PEXlib.h"
#include "PEXlibint.h"

#include "pl_ref_data.h"
#include "pl_oc_buff.h"

extern	char		*_XAllocScratch();

/*+
 * NAME:
 * 	PEXCreateStructure
 *
 * FORMAT:
 *	`PEXCreateStructure`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *  			returned by a successful `XOpenDisplay" call.
 *
 * RETURNS:
 *	The resource ID of the newly-created structure resource.
 *
 * DESCRIPTION:
 * 	This routine creates a structure resource and returns the resource ID
 *	of the created structure.  The returned ID can be used to
 *	refer to the created structure resource.
 *
 * ERRORS:
 *	IDChoice	ID already in use or not in range assigned to client.
 *
 *	Alloc		Server failed to allocate the resource.
 *
 * SEE ALSO:
 *	`PEXDestroyStructures",  `PEXCopyStructure",
 *	`PEXGetStructureInfo", `PEXGetStructuresInNetwork",  `PEXGetAncestors",
 *	`PEXGetDescendants", `PEXChangeStructureRefs",  `PEXExecuteStructure",
 *	`PEXBeginStructure", `PEXEndStructure"
 *
 */

pxlStructure
PEXCreateStructure (display)

INPUT Display		*display;

{
/* AUTHOR: */
    pexCreateStructureReq	*req;
    pexStructure		s;

    /* get a structure resource id from X */
    s = XAllocID (display);

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (CreateStructure, req);
    req->id = s;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (s);
}

/*+
 * NAME:
 * 	PEXDestroyStructures
 *
 * FORMAT:
 * 	`PEXDestroyStructures`
 *
 * ARGUMENTS:
 *	display		A pointer to a structure returned by
 *			a successful `XOpenDisplay" call.
 *
 *	structures	A list of resource IDs of the structures to
 *			be destroyed.
 *
 *	numStructures	The number of resource IDs in `structures'.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine destroys each structure in the list of structure IDs
 *	pointed to by `structures' and removes all references to it in the
 *	PEX server extension.  `NumStructures' specifies the number of
 *	resource IDs listed in `structures'.
 *
 *	This routine also removes any "execute structure" structure elements
 *	that reference a structure in the list and unposts any structure in
 *	the list from a PHIGS workstation resource to which it is posted.
 *	If a structure has any structure elements removed from it as a result
 *	of this call, its element pointer will continue to point at the same
 *	structure element.  However, if the structure element being pointed
 *	at is removed, the element pointer will be positioned at the previous
 *	structure element.
 *
 *	If a path in a search context or pick measure resource which contains
 *	a destroyed structure resource ID is later used in a `PEXSearchNetwork"
 *	or `PEXUpdatePickMeasure" request, an error is generated.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateStructure", `PEXCopyStructure", `PEXGetStructureInfo",
 *	`PEXGetStructuresInNetwork",  `PEXGetAncestors", `PEXGetDescendants",
 *	`PEXChangeStructureRefs",  `PEXExecuteStructure", `PEXBeginStructure",
 *	`PEXEndStructure"
 *
 */

void
PEXDestroyStructures (display, structures, numStructures)

INPUT Display		*display;
INPUT pxlStructure	*structures;
INPUT int		numStructures;

{
/* AUTHOR: */
    pexDestroyStructuresReq	*req;
    int size;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    size = numStructures * sizeof(pexStructure);

    PEXGetReqExtra (DestroyStructures, size, req);
    req->numStructures = numStructures;

    COPY_AREA ((char *)structures, ((char *)&(req[1])), size);

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXCopyStructure
 *
 * FORMAT:
 * 	`PEXCopyStructure`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	srcStructure	The resource ID of the source structure.
 *
 *	destStructure	The resource ID of the destination structure.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *  	This routine copies elements in structure resource `srcStructure' to
 *	the destination structure `destStructure'.  Both structures must already
 *	exist as valid structure resources.  Elements already in
 *	`destStructure' are overwritten.
 *	The element pointer and editing mode attributes of `srcStructure'
 *	are copied to `destStructure' as well.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateStructure", `PEXDestroyStructures",
 *	`PEXGetStructureInfo", `PEXGetStructuresInNetwork",  `PEXGetAncestors",
 *	`PEXGetDescendants", `PEXChangeStructureRefs",  `PEXExecuteStructure",
 *	`PEXBeginStructure", `PEXEndStructure"
 *
 */

void
PEXCopyStructure (display, srcStructure, destStructure)

INPUT Display		*display;
INPUT pxlStructure	srcStructure;
INPUT pxlStructure	destStructure;

{
/* AUTHOR: */
    pexCopyStructureReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (CopyStructure, req);
    req->src = srcStructure;
    req->dst = destStructure;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXGetStructureInfo
 *
 * FORMAT:
 * 	`PEXGetStructureInfo`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *  	structure	The resource ID of the queried structure.
 *
 *	structureInfoReturn Returns a pointer to a data structure that
 *			contains information about the structure resource.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 * 	This routine returns information about the specified `structure'
 *	resource.  A pointer to the information is returned in
 *	`structureInfoReturn'.  Argument `structureInfoReturn' is of type
 *	`pxlStructureInformation', defined in the `PEXlib.h' include file.
 *
 *	PEXlib allocates storage for `structureInfoReturn'.
 *	`PEXFree"(`*structureInfoReturn') deallocates the storage.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateStructure", `PEXDestroyStructures",
 *	`PEXCopyStructure", `PEXGetStructuresInNetwork",  `PEXGetAncestors",
 *	`PEXGetDescendants", `PEXChangeStructureRefs",  `PEXExecuteStructure",
 *	`PEXBeginStructure", `PEXEndStructure"
 *
 */

Status
PEXGetStructureInfo (display, structure, structureInfoReturn)

INPUT Display			*display;
INPUT pxlStructure		structure;
OUTPUT pxlStructureInformation	**structureInfoReturn;

{
/* AUTHOR: */
    pexGetStructureInfoReq	*req;
    pexGetStructureInfoReply	rep;
    pxlStructureInformation	*psi;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (GetStructureInfo, req);
    req->fpFormat = pexFpFormat;
    req->sid = structure;
    req->itemMask = PEXElementPtr | PEXNumElements | PEXLengthStructure
	    | PEXHasRefs | PEXEditMode;

    /* get a reply */
    if (_XReply (display, &rep, 0, xTrue) == 0)
    {/* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
	return (1);            /* return an error */
    }

    /* allocate a buffer for the replies to pass back to the client */
    psi = (pxlStructureInformation *) PEXAllocBuf
	((unsigned)(sizeof(PEXHeader) + sizeof(pxlStructureInformation)));
    ((PEXHeader *)psi)->free = _PEXFreeList;   /* routine to call to free it */
    psi = (pxlStructureInformation *)&(((PEXHeader *)psi)[1]);

    /* fill the output buffer */
    psi->editMode = rep.editMode;
    psi->elementPtr = rep.elementPtr;
    psi->numElements = rep.numElements;
    psi->length = rep.lengthStructure;
    psi->numRefs = rep.hasRefs;
    *structureInfoReturn = psi;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	PEXGetElementInfo
 *
 * FORMAT:
 * 	`PEXGetElementInfo`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	structure	The resource ID of the queried structure.
 *
 *	whence1		An integer specifying, with `offset1', the
 *			beginning of the range of queried elements.
 *
 *	offset1		An offset from `whence1' denoting the beginning
 *			of the range of queried elements.
 *
 *	whence2		An integer specifying, with `offset2', the
 *			end of the range of elements to be queried.
 *
 *	offset2		An offset from `whence2' denoting the end
 *			of the range of elements to be queried.
 *
 *	elementInfoReturn Returns a pointer to a list of records that
 *			describe the elements in the specified range.
 *
 *	numElementInfoReturn Returns an integer containing the number
 *			of data records in the list returned in
 *			`elementInfoReturn'.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 * 	This routine returns information about a range of elements from the
 *	structure specified by `structure'.
 *	The beginning of the range is the sum of `whence1'
 *	plus `offset1'.  The end of the range is the sum
 *	of `whence2' and `offset2'.  Possible values for `whence1' and
 *	`whence2' are `PEXBeginning', `PEXCurrent', and `PEXEnd' (defined in the `PEX.h'
 *	include file).
 *	`Offset1' and `offset2' are integer values which can be negative.
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
 *	If a computed offset is less than zero it is set to
 *	zero before obtaining the element information.  If a
 *	computed offset is greater than the number of elements in the
 *	structure, it is set to the offset of the last structure element
 *	in the structure.  The element pointer attribute of `structure'
 *	is not affected by this command.
 *
 *	Information about the list of inquired elements is returned in
 *	a list of records pointed to by `elementInfoReturn'.  Each record is
 *	of type `pxlElementInfo', defined in the `PEXprotost.h' include file.
 *	Each record includes the type of each element and its size.
 *	The number of records in this list is returned in `numElementsReturn'.
 *
 *	PEXlib allocates memory for `elementInfoReturn'.
 *	`PEXFree"(`*elementInfoReturn') releases the memory.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 *	Value		"Whence" is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateStructure"
 *
 */

Status
PEXGetElementInfo (display, structure, whence1, offset1, whence2, offset2,
		  elementInfoReturn, numElementInfoReturn)

INPUT Display		*display;
INPUT pxlStructure	structure;
INPUT int		whence1;
INPUT long		offset1;
INPUT int		whence2;
INPUT long		offset2;
OUTPUT pxlElementInfo	**elementInfoReturn;
OUTPUT int		*numElementInfoReturn;

{
/* AUTHOR: */
    pexGetElementInfoReq	*req;
    pexGetElementInfoReply	rep;
    pexElementInfo		*pei;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (GetElementInfo, req);
    req->fpFormat = pexFpFormat;
    req->sid = structure;
    req->range.position1.whence = whence1;
    req->range.position1.offset = offset1;
    req->range.position2.whence = whence2;
    req->range.position2.offset = offset2;

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
	return (1);        /* return an error */
    }

    /* allocate a buffer for the replies to pass back to the client */
    pei = (pexElementInfo *)PEXAllocBuf
	((unsigned)(sizeof(PEXHeader) + (rep.length << 2)));
    ((PEXHeader *)pei)->free = _PEXFreeList;    /* routine to call to free it */
    pei = (pexElementInfo *)&(((PEXHeader *)pei)[1]);

    /* fill the buffer */
    _XRead (display, (char *)pei, (long)(rep.length << 2));

    *elementInfoReturn = pei;
    *numElementInfoReturn = rep.numInfo;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	PEXGetStructuresInNetwork
 *
 * FORMAT:
 * 	`PEXGetStructuresInNetwork`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	structure	The resource ID of the root structure in the
 *			structure network.
 *
 *	which		A value indicating which structure resource IDs
 *			are returned.
 *
 *	structuresReturn Returns a pointer to an array of returned structure
 *			resource IDs.
 *
 *	numStructuresReturn Returns the number of structure resource IDs
 *			in `structuresReturn'.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 * 	This routine returns a list of unique structure resource IDs that are
 *	referenced in the structure network rooted at `structure'.
 *	Possible values for `which' are `PEXAll' and `PEXOrphans' (defined
 *	in the `PEX.h' include file).  If `which' is `PEXAll', the IDs of all
 *	structure resources referenced in the structure network rooted
 *	at `structure' are returned in the list `structuresReturn'.
 *	If `which' is `PEXOrphans', the IDs returned are those
 *	not referenced by any structures outside of those in the
 *	specified structure network.
 *	The number of structure IDs returned is placed in a location
 *	pointed to by `numStructuresReturn'.
 *
 * 	PEXlib allocates storage for information contained in
 *	`structuresReturn'.  `PEXFree"(`*structuresReturn') deallocates
 *	 the storage.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 *	Value		"Which" is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateStructure", `PEXDestroyStructures",
 *	`PEXCopyStructure", `PEXGetStructureInfo",  `PEXGetAncestors",
 *	`PEXGetDescendants", `PEXChangeStructureRefs",  `PEXExecuteStructure",
 *	`PEXBeginStructure", `PEXEndStructure"
 *
 */

Status
PEXGetStructuresInNetwork (display, structure, which, structuresReturn,
	numStructuresReturn)

INPUT Display		*display;
INPUT pxlStructure	structure;
INPUT int		which;
OUTPUT pxlStructure	**structuresReturn;
OUTPUT int		*numStructuresReturn;

{
/* AUTHOR: */
    pexGetStructuresInNetworkReq	*req;
    pexGetStructuresInNetworkReply	rep;
    pexStructure			*ps;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (GetStructuresInNetwork, req);
    req->sid = structure;
    req->which = which;

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
	return (1);             /* return an error */
    }

    /* allocate a buffer for the replies to pass back to the client */
    ps = (pexStructure *)PEXAllocBuf
	((unsigned)(sizeof(PEXHeader) + (rep.length << 2)));
    ((PEXHeader *)ps)->free = _PEXFreeList;     /* routine to call to free it */
    ps = (pexStructure *)&(((PEXHeader *)ps)[1]);

    /* fill the buffer */
    _XRead (display, (char *)ps, (long)(rep.length << 2));

    *structuresReturn = ps;
    *numStructuresReturn = rep.numStructures;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	PEXGetAncestors
 *
 * FORMAT:
 * 	`PEXGetAncestors`
 *
 * ARGUMENTS:
 * 	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	structure	The resource ID of the structure whose ancestors
 *			are returned.
 *
 *	pathPart	The part of the path to `structure' which is
 *			returned.
 *
 *	pathDepth	The maximum number of structure network path levels
 *			to be returned in each path found.
 *
 *	pathsReturn	Returns a pointer to a list of paths defining
 *			the ancestors of the specified structure.
 *
 *	numPathsReturn	Returns the number of paths returned in
 *			`pathsReturn'.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *  	This routine returns a list of structure network paths which
 *	reference the
 *	specified `structure'.  `PathsReturn' is a pointer to an array of
 *	structures in which the returned path information is placed.
 *	The elements of the array are of type `pxlStructurePath', which is
 *	defined in the `PEXlib.h' include file.  Paths are
 *	returned as lists of element references, each of which is represented
 *	as a structure resource ID and an offset that gives the element's
 *	position in the structure.  The number of paths found is returned
 *	in `numPathsReturn'.
 *
 *	`pathPart' can be either `PEXTopPart', which requests that the top of
 *	the structure paths be returned or `PEXBottomPart', which
 *	requests that the bottom of the structure paths be returned.
 *	(Both of these constants are defined in the `PEX.h' include file.)
 *
 *	`PathDepth' specifies the maximum number of element references
 *	to be returned in each path.  If `pathDepth' is 0, the entire
 *	path is returned.  If `pathDepth' is 1, only one element
 *	reference is returned for each path.  `pathDepth' of 2 returns two
 *	elements, and so on.
 *
 *	Specifying `pathDepth'=0 and `pathPart'=`PEXTopPart'
 *	returns the unique top parts of all paths to `structure'.
 *	Specifying `pathDepth'=1 and
 *	`pathPart'=`PEXTopPart' returns the root structure of all
 *	structure networks which contain `structure'.
 *	`PathDepth'=2 and `pathPart'=`PEXBottomPart' returns all of
 *	the structure's immediate ancestors.  Determine the number
 *	of references to `structure' by setting
 *	`pathDepth'=1 and `pathPart'=`PEXBottomPart'.
 *
 *	PEXlib allocates storage for the ancestor information returned in
 *	`pathsReturn'.  `PEXFree"(`*pathsReturn') deallocates the storage.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 *	Value		"Path_part" is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateStructure", `PEXDestroyStructures",
 *	`PEXCopyStructure", `PEXGetStructureInfo", `PEXGetStructuresInNetwork",
 *	`PEXGetDescendants", `PEXChangeStructureRefs",  `PEXExecuteStructure",
 *	`PEXBeginStructure", `PEXEndStructure"
 *
 */

Status
PEXGetAncestors (display, structure, pathPart, pathDepth, pathsReturn,
	numPathsReturn)

INPUT Display		*display;
INPUT pxlStructure	structure;
INPUT int		pathPart;
INPUT int		pathDepth;
OUTPUT pxlStructurePath	**pathsReturn;
OUTPUT int		*numPathsReturn;

{
/* AUTHOR: */
    pexGetAncestorsReq		*req;
    pexGetAncestorsReply	rep;
    pxlStructurePath		*psp;
    char			*prep;
    pexElementRef		*per;
    int				i;
    int				numElements;
    int				size;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (GetAncestors, req);
    req->sid = structure;
    req->pathOrder = pathPart;
    req->pathDepth = pathDepth;

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
	return (1);           /* return an error */
    }

    /* allocate a buffer for the replies to pass back to the client */
    psp = (pxlStructurePath *)PEXAllocBuf
	((unsigned)(sizeof(PEXHeader) + rep.numPaths*sizeof(pxlStructurePath)));
    ((PEXHeader *)psp)->free = _PEXFreeStructurePath;/* routine to free it */
    ((PEXHeader *)psp)->size = rep.numPaths;         /* size of buffer */
    psp = (pxlStructurePath *)&(((PEXHeader *)psp)[1]);

    *pathsReturn = psp;
    *numPathsReturn = rep.numPaths;

    /* fill the buffer */
    prep = _XAllocScratch (display, (unsigned long)(rep.length << 2));
    _XRead (display, (char *)prep, (long)(rep.length << 2));

    for (i=0; i<rep.numPaths; i++)
    {
	numElements = *((CARD32 *)prep);
	prep += sizeof(CARD32);
	size = numElements * sizeof(pexElementRef);
	per = (pexElementRef *)PEXAllocBuf ((unsigned)size);
	COPY_AREA ((char *)prep, (char *)per, size);
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
 * 	PEXGetDescendants
 *
 * FORMAT:
 * 	`PEXGetDescendants`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	structure	The resource ID of the structure whose descendants
 *			are returned.
 *
 *	pathPart	The part of the path to `structure' which is
 *			returned.
 *
 *	pathDepth	The maximum number of structure network path levels
 *			to be returned in each path found.
 *
 *	pathsReturn	Returns a pointer to a list of paths defining
 *			the descendants of the specified structure.
 *
 *	numPathsReturn	Returns the number of paths returned in
 *			`pathsReturn'.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 * 	This routine returns a list of structure network paths referenced by the
 *	specified `structure'.  `PathsReturn' is a pointer to an array of
 *	structures in which the returned path information is placed.
 *	The elements of the array are of type `pxlStructurePath', which is
 *  	defined in the `PEXlib.h' include file.  Paths are
 *	returned as lists of element references, each of which is represented
 *	as a structure resource ID and an offset that gives the element's
 *	position in the structure.  The number of paths found is returned
 *	in `numPathsReturn'.
 *
 *	`pathPart' can be either `PEXTopPart', which requests that the top of
 *	the structure paths be returned or `PEXBottomPart', which
 *	requests that the bottom of the structure paths be returned.
 *	(Both of these constants are defined in the `PEX.h' include file.)
 *
 *	`PathDepth' specifies the maximum number of element references
 * 	to be returned in each path.  If `pathDepth' is 0, the entire
 *	path is returned.  If `pathDepth' is 1, only one element
 *	reference is returned for each path, and so on.
 *
 *	*** Redo this description ***
 *	For instance, specifying `pathDepth'=0 and `pathOrder'=`PEXTopFirst'
 * 	returns all paths from `structure' to leaf nodes in the structure
 *	network tree in the order they would be traversed.  Specifying
 *	`pathDepth'=2 and `pathOrder'=`PEXTopFirst' returns all children of
 *	`structure'.  Specify `pathDepth'=1 and `pathOrder'=`PEXBottomFirst'
 *  	to determine the bottom-most structures of the structure network
 *	rooted at `structure'.
 *
 *	PEXlib allocates storage for the descendant information returned in
 *	`pathsReturn'.  `PEXFree"(`*pathsReturn') deallocates this storage.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 *	Value		"Path_part" is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateStructure", `PEXDestroyStructures",
 *	`PEXCopyStructure", `PEXGetStructureInfo", `PEXGetStructuresInNetwork",
 *	`PEXGetAncestors", `PEXChangeStructureRefs",  `PEXExecuteStructure",
 *	`PEXBeginStructure", `PEXEndStructure"
 *
 */

Status
PEXGetDescendants (display, structure, pathPart, pathDepth, pathsReturn,
	numPathsReturn)

INPUT Display		*display;
INPUT pxlStructure	structure;
INPUT int		pathPart;
INPUT int		pathDepth;
OUTPUT pxlStructurePath	**pathsReturn;
OUTPUT int		*numPathsReturn;

{
/* AUTHOR: */
    pexGetDescendantsReq	*req;
    pexGetDescendantsReply	rep;
    pxlStructurePath		*psp;
    char			*prep;
    pexElementRef		*per;
    int				i;
    int				numElements;
    int				size;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (GetDescendants, req);
    req->sid = structure;
    req->pathOrder = pathPart;
    req->pathDepth = pathDepth;

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
	return (1);          /* return an error */
    }

    /* allocate a buffer to pass the replies back to the client */
    psp = (pxlStructurePath *)PEXAllocBuf
	((unsigned)(sizeof(PEXHeader) + rep.numPaths*sizeof(pxlStructurePath)));
    ((PEXHeader *)psp)->free = _PEXFreeStructurePath;/* routine to call to free it */
    ((PEXHeader *)psp)->size = rep.numPaths;         /* size of buffer */
    psp = (pxlStructurePath *)&(((PEXHeader *)psp)[1]);

    *pathsReturn = psp;
    *numPathsReturn = rep.numPaths;

    /* fill the buffer */
    prep = _XAllocScratch (display, (unsigned long)(rep.length << 2));
    _XRead (display, (char *)prep, (long)(rep.length << 2));

    for (i=0; i<rep.numPaths; i++)
    {
	numElements = *((CARD32 *)prep);
	prep += sizeof(CARD32);
	size = numElements * sizeof(pexElementRef);
	per = (pexElementRef *)PEXAllocBuf ((unsigned)size);
	COPY_AREA ((char *)prep, (char *)per, size);
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
 * 	PEXFetchElList
 *
 * FORMAT:
 * 	`PEXFetchElList`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure returned
 *			by a successful `XOpenDisplay" call.
 *
 *	structure	The resource ID of the structure from which elements
 *			are fetched.
 *
 *	whence1		An integer specifying, with `offset1', the
 *			beginning of the range of elements to be fetched.
 *
 *	offset1		An offset from `whence1' denoting the beginning
 *			of the range of elements to be fetched.
 *
 *	whence2		An integer specifying, with `offset2', the
 *			end of the range of elements to be fetched.
 *
 *	offset2		An offset from `whence2' denoting the end
 *			of the range of elements to be fetched.
 *
 *	ocReturn 	Returns pointer to list of output commands 
 *
 *      numElementsReturn Number of elements in the list
 *
 *      sizeReturn	Size of the list (in bytes)
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 * 	This routine fetches a range of structure elements from the specified
 *	`structure'.
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
 *	zero before fetching the structure elements.  If either
 *	computed offset is greater than the number of elements in the
 *	structure, it is set to the offset of the last structure element
 *	in the structure.  The element pointer attribute of `structure'
 *	is not affected by this command.
 *
 *	A pointer to the list of fetched structure elements is returned in
 *	`ocReturn'. `PEXStoreOCList" may be called to add the list of output
 *	commands to an oc buffer.
 *
 *	The application should deallocate the memory allocated for storing
 *	the list of output commands when it is done with it. This is 
 *	accomplished by calling `PEXFree".
 *
 * ERRORS:
 *	Structure		Specified structure resource ID is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	Value			"Whence" is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateStructure", `PEXFree", `PEXStoreOCList" 
 *
 */

Status
PEXFetchElList(display, structure, whence1, offset1, whence2, offset2,
	ocReturn, numElementsReturn, sizeReturn)

INPUT Display		*display;
INPUT pxlStructure	structure;
INPUT int		whence1;
INPUT long		offset1;
INPUT int		whence2;
INPUT long		offset2;
OUTPUT pxlElementInfo 	**ocReturn;
OUTPUT long		*numElementsReturn;
OUTPUT long		*sizeReturn;

{
/* AUTHOR:  Sally C. Barry */
    pexFetchElementsReq		*req;
    pexFetchElementsReply	rep;
    PEXHeader			*prep;
    long			repSize;

    /* Note that this routine may be completely wrong and need re-coding.  It
	will need a lot of testing, once the server handles it. -- scb */

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (FetchElements, req);
    req->fpFormat = pexFpFormat;
    req->sid = structure;
    req->range.position1.whence = whence1;
    req->range.position1.offset = offset1;
    req->range.position2.whence = whence2;
    req->range.position2.offset = offset2;

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
        return (1);            /* return an error */
    }

    /* the information is returned in an OC buffer because this is intended
       to be used for archival */

    /* read the reply information */
    repSize = rep.length<<2;
    prep = (PEXHeader *) PEXAllocBuf( sizeof( PEXHeader) + repSize);
    if (prep != NULL)
    {
        prep->free = _PEXFreeList; /* routine to call to free it */
        *ocReturn = (pexElementInfo *) &prep[1];
        _XRead (display, (char *)*ocReturn, (long)repSize);
    }
    else
	*ocReturn = NULL;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    *sizeReturn = rep.length;
    *numElementsReturn = rep.numElements;
    return ( (*ocReturn == NULL) ? (!Success) : (Success) );
}

/*+
 * NAME:
 * 	PEXSetEditingMode
 *
 * FORMAT:
 * 	`PEXSetEditingMode`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	structure	An unsigned longword specifying the resource
 *			ID of the structure network.
 *
 *	mode		An integer specifying the editing mode to be
 *			set.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *  	This routine sets the editing mode attribute `mode' for the structure
 *	specified
 *	as `structure'.	  The editing mode specifies how editing operations
 *	affect the structure.  Possible values for `mode' are
 *	`PEXStructureInsert' and `PEXStructureReplace'.
 *	(These constants are defined in the `PEX.h' include file.)
 *	If the editing mode is
 *	`PEXStructureInsert', subsequent requests to create structure
 *	elements cause elements to be inserted into the structure.
 *	The element pointer is then incremented by the number of elements
 *	inserted.  If the editing mode is `PEXStructureReplace',
 *	output requests that create structure elements cause
 *	structure elements to replace elements, starting at the location
 *	specified by the element pointer.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 *	Value		"Mode" is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateStructure"
 *
 */

void
PEXSetEditingMode (display, structure, mode)

INPUT Display		*display;
INPUT pxlStructure	structure;
INPUT int		mode;

{
/* AUTHOR: */
    pexSetEditingModeReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (SetEditingMode, req);
    req->sid = structure;
    req->mode = mode;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXSetElementPtr
 *
 * FORMAT:
 * 	`PEXSetElementPtr`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	structure	An unsigned longword specifying the resource
 *			ID of the editing context resource whose element
 *			pointer is to be set.
 *
 *	whence		An integer specifying with `offset', the
 *			position of the element pointer to be set.
 *
 *	offset		A long integer specifying the offset from
 *			`whence' which specifies the location of the
 *			element pointer to be set.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine sets the element pointer for the structure specified as
 *	`structure' to the position specified by `whence' and `offset'.
 *  	The element pointer position is the sum of `whence'
 *	plus `offset'.  Possible values for `whence' are `PEXBeginning',
 *	`PEXCurrent', and `PEXEnd'.
 *	(These constants are defined in the `PEX.h' include file.)
 *	`offset' can be any long integer value and may be negative.
 *
 *	For example, if `whence' is `PEXBeginning', the beginning
 *	element pointer position is the value of `offset'.  If
 *	`whence' is `PEXCurrent', the pointer position is
 *	the value of the current element pointer position plus the value
 *	of `offset', and so on.
 *
 *	If either computed offset is less than zero, it is set to
 *	zero before obtaining the element information.  If either
 *	computed offset is greater than the number of elements in the
 *	structure, it is set to the offset of the last structure
 *	element in the structure.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 *	Value		"Whence" is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateStructure"
 *
 */

void
PEXSetElementPtr (display, structure, whence, offset)

INPUT Display		*display;
INPUT pxlStructure	structure;
INPUT int		whence;
INPUT long		offset;

{
/* AUTHOR: */
    pexSetElementPointerReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (SetElementPointer, req);
    req->sid = structure;
    req->position.whence = whence;
    req->position.offset = offset;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXSetElementPtrAtLabel
 *
 * FORMAT:
 * 	`PEXSetElementPtrAtLabel`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	structure	An unsigned longword giving the resource ID
 *			of the structure whose element pointer position
 *			is to be set.
 *
 *	label		A longword specifying the name of
 *			the label.
 *
 *	offset		A long integer specifying the offset from
 *			the pointer position of `label'.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *  	This routine sets the element pointer for the structure specified as
 *	`structure'
 *	at a position denoted by `label'.  A search is conducted for
 *	the next occurrence of a "label" structure element containing
 *	`label', starting at the current element pointer position plus
 *	one and proceeding in the forward direction.  If `label' is
 *	found, the element pointer for the structure is set to the
 *	offset of the located label plus the the value of the offset
 *	`offset'.  If `label' is not found, the structure's element
 *	pointer is left unchanged.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 *	Label		No occurrences of specified label in structure.
 *
 * SEE ALSO:
 *	`PEXCreateStructure",  `PEXLabel"
 *
 */

void
PEXSetElementPtrAtLabel (display, structure, label, offset)

INPUT Display		*display;
INPUT pxlStructure	structure;
INPUT long		label;
INPUT long		offset;

{
/* AUTHOR: */
    pexSetElementPointerAtLabelReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (SetElementPointerAtLabel, req);
    req->sid = structure;
    req->label = label;
    req->offset = offset;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXElementSearch
 *
 * FORMAT:
 * 	`PEXElementSearch`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	structure	An unsigned longword containing the resource
 *			ID of the structure to be searched.
 *
 *	whence		An integer specifying, with `offset', the
 *			offset at which the search is to begin.
 *
 *	offset		A long integer specifying the offset from
 *			`whence' at which the search is to begin.
 *
 *	direction	An integer specifying the direction of the
 *			search, whether forward or backward.
 *
 *	inclList	A pointer to a list of short integers that
 *			specify the structure elements to be searched
 *			for in `structure'.
 *
 *	numIncl		An integer specifying the number of values
 *			in `inclList'.
 *
 *	exclList	A pointer to a list of short integers that
 *			specify the structure elements not to be
 *			searched for in `structure'.
 *
 *	numExcl		An integer specifying the number of values
 *			in `exclList'.
 *
 *	statusReturn	The reply status from the PEX server extension,
 *			`PEXFound' or `PEXNotFound'.
 *
 *	offsetReturn	A pointer to an unsigned longword
 *			that specify the structure elements located
 *			by the search.
 *
 * RETURNS:
 *	Zero if the request was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *  	This routine searches for the first occurrence of the specified
 *	element type
 *	in the structure specified as `structure'.  The offset at which
 *	the search begins is `whence' plus `offset'.  Possible
 *	values for `whence' are `PEXBeginning', `PEXCurrent', and `PEXEnd'.  These
 *	values refer to the beginning of the structure, the position of the
 *	current element pointer, and the end of the structure,
 *	respectively.  The `PEX.h' include file contains definitions of these
 *	constants.
 *
 *	For example, if `whence' is `PEXBeginning', the element search begins
 *	at the beginning of the structure plus an offset equal to the
 *	value of `offset'.  If `whence' is `PEXCurrent', the search begins
 *	at the current element pointer position plus the offset `offset',
 *	and so on.
 *
 *	If the computed offset is less than zero, it is set to zero before
 *	the search is performed.  If the computed offset is greater than
 *	the number of elements in the structure, it is set to the offset
 *	of the last structure element in the structure.
 *
 *	An element is selected if its element type is contained in
 *	`inclList' and is not contained in `exclList'.  The constants that
 *	describe the element types are defined in the `PEX.h' include file
 *	 (for example,
 *	`OCTextFontIndex', `PEXOCLineType', and so on.)  An element type of
 *	`PEXOCAll' causes all element types to match.  If a structure element
 *	type is in both `inclList' and `exclList', it is excluded.
 *
 *	The search terminates if a match is found or if the limits of the
 *	structure are reached.  The search progresses from the start point
 *	and proceeds either `PEXForward' or `PEXBackward', depending upon the value
 *	of `direction'.
 *	(These constants are defined in the `PEX.h' include file.)
 *	This is a non-descending search, that is, the
 *	search does not include any structures referenced by "execute
 *	structure" elements.  If the search finds a match, a status of
 *	`PEXFound' is returned in `statusReturn', and the offset of the matching
 *	element is
 *	returned in `offsetReturn'.  If the search is unsuccessful, a
 *	status of `PEXNotFound' is returned in `statusReturn'.
 *	(These constants are defined in the `PEX.h' include file.)
 *
 *	The element pointer position of `structure' is not changed.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 *	Value		"Whence" or "direction" is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateStructure"
 *
 */

Status
PEXElementSearch (display, structure, whence, offset, direction, inclList,
	numIncl, exclList, numExcl, statusReturn, offsetReturn)

INPUT Display		*display;
INPUT pxlStructure	structure;
INPUT int		whence;
INPUT long		offset;
INPUT int		direction;
INPUT short		*inclList;
INPUT int		numIncl;
INPUT short		*exclList;
INPUT int		numExcl;
OUTPUT int		*statusReturn;
OUTPUT unsigned long	*offsetReturn;

{
/* AUTHOR: */
    pexElementSearchReq		*req;
    pexElementSearchReply	rep;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    /* get enough room for the include list plus pad and the exclude list plus
       pad */
    PEXGetReqExtra (ElementSearch,
	(numIncl + (numIncl & 1) + numExcl + (numExcl & 1)) * sizeof(CARD16),
	req);
    req->sid = structure;
    req->position.whence = whence;
    req->position.offset = offset;
    req->direction = direction;
    req->numIncls = numIncl;
    req->numExcls = numExcl;

    COPY_AREA ((char *)inclList, ((char *)&req[1]), numIncl * sizeof(CARD16));
    COPY_AREA ((char *)exclList,
	((char *)&req[1]) + (numIncl + (numIncl & 1)) * sizeof(CARD16),
	numExcl * sizeof(CARD16));

    /* get a reply */
    if (_XReply (display, &rep, 0, xTrue) == 0)
    {/* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
	return (1);               /* return an error */
    }

    *statusReturn = rep.status;
    *offsetReturn = rep.foundOffset;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	PEXDeleteElements
 *
 * FORMAT:
 * 	`PEXDeleteElements`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	structure	An unsigned longword specifying the resource
 *			ID of the structure.
 *
 *	whence1		An integer specifying, with `offset1', the
 *			beginning position of the range of elements
 *			to be deleted.
 *
 *	offset1		A long integer specifying the offset from
 *			`whence1' which denotes the beginning of the
 *			range of elements to be deleted.
 *
 *	whence2		An integer specifying, with `offset2',
 *			the end of the range of elements to be deleted.
 *
 *	offset2		A long integer specifying the offset from
 *			`whence2' denoting the end of the range
 *			of elements to be deleted.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine deletes a range of elements from the structure specified by
 *	`structure'.  The beginning and end of the range are computed
 *	from an initial element pointer position plus an offset.
 *  	The beginning of the range is the sum of `whence1'
 *	plus `offset1'.  The end of the range is the sum
 *	of `whence2' and `offset2'.  Possible values for `whence1' and
 *	`whence2' are `PEXBeginning', `PEXCurrent', and `PEXEnd'.
 *	(These constants are defined in the `PEX.h' include library.)
 *	`offset1' and
 *	`offset2' can be any long integer value, and can be negative.
 *
 *	For example, if `whence1' is `PEXBeginning', the beginning
 *	element pointer position is the value of `offset1'.  If
 *	`whence2' is `PEXCurrent', the end pointer position is
 *	the value of the current element pointer position plus the value
 *	of `offset2'.
 *
 *	If a computed offset is less than zero, it is set to
 *	zero before obtaining the element information.  If a
 *	computed offset is greater than the number of elements in the
 *	structure, the offset is set to the offset of the last structure element
 *	in the structure.  The first offset need not be
 *	less than the second.  Deleting the zero element is effectively
 *	a no-op.  After the deletion operation, the structure element
 *	pointer is set to the element immediately preceding the range
 *	of deleted elements.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 *	Value		"Whence" is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateStructure"
 *
 */

void
PEXDeleteElements (display, structure, whence1, offset1, whence2, offset2)

INPUT Display		*display;
INPUT pxlStructure	structure;
INPUT int		whence1;
INPUT long		offset1;
INPUT int		whence2;
INPUT long		offset2;

{
/* AUTHOR: */
    pexDeleteElementsReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (DeleteElements, req);
    req->sid =  structure;
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
 * 	PEXDeleteToLabel
 *
 * FORMAT:
 * 	`PEXDeleteToLabel`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	structure	An unsigned longword specifying the resource
 *			ID of the structure.
 *
 *	whence		An integer specifying, with `offset', the
 *			beginning of the range of elements to be deleted.
 *
 *	offset		A long integer specifying the offset from
 *			`whence' denoting the beginning of the
 *			range of elements to be deleted.
 *
 *	label		A longword specifying the name of
 *			the label.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine deletes a range of elements between a computed offset and a
 *	specified `label' in the structure specified as `structure'.
 *  	Arguments `whence' and `offset' are used to compute the beginning of the
 *	deletion range. Argument `label' specifies the end of the deletion
 *	range.  Elements are deleted from the element immediately
 *	after the computed offset up to the next occurrence of the label.
 *	The label is not deleted.  If `label' is not found, no
 *	elements are deleted.  Multiple labels can occur in the given
 *	structure.
 *
 *	Possible values for `whence' are `PEXBeginning', `PEXCurrent', and
 *	`PEXEnd'.  These values refer to the beginning of the structure, the
 *	position of the current element pointer position, and the end
 *	of the structure, respectively.  These constants are defined in the
 *	`PEX.h' include file.
 *
 *	The offset can have a negative or zero value. For example,
 *	if `whence' is `PEXCurrent', the first structure element to be deleted
 *	is located at the current position of the structure element pointer
 *	plus the value of `offset'.
 *
 *	If the computed offset is less than zero, it is set to zero
 *	before the deletion occurs.  If the computed offset is greater
 *	than the number of elements in the structure, the offset is set to the
 *	offset of the last structure element.  Deleting the zero
 *	element is effectively a no-op.  After the deletion operation,
 *	the structure element pointer is set to the element immediately
 *	preceding the range of deleted elements.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 *	Value		"Whence" is invalid.
 *
 *	Label		No occurrences of specified label in structure.
 *
 * SEE ALSO:
 *	`PEXCreateStructure", `PEXLabel"
 *
 */

void
PEXDeleteToLabel (display, structure, whence, offset, label)

INPUT Display		*display;
INPUT pxlStructure	structure;
INPUT int		whence;
INPUT long		offset;
INPUT long		label;

{
/* AUTHOR: */
    pexDeleteElementsToLabelReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (DeleteElementsToLabel, req);
    req->sid =  structure;
    req->position.whence = whence;
    req->position.offset = offset;
    req->label = label;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXDeleteBetweenLabels
 *
 * FORMAT:
 * 	`PEXDeleteBetweenLabels`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	structure	An unsigned longword specifying the resource
 *			ID of the structure from which elements are
 *			deleted.
 *
 *	label1		A longword denoting the name of the
 *			first label.
 *
 *	label2		A longword denoting the name of the
 *			second label.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine deletes a range of elements from the structure specified as
 *	`structure'.  Elements between `label1' and `label2' are deleted.
 *  	The label elements are not deleted.  A search for
 *	`label1' is performed starting at the current offset.
 *	A search for `label2' is performed starting at the element following
 *	`label1'.  After the deletion operation, the structure element
 *	pointer is set to the pointer position at `label1'.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 *	Label		No occurrences of specified label in structure.
 *
 * SEE ALSO:
 *	`PEXCreateStructure", `PEXLabel"
 *
 */

void
PEXDeleteBetweenLabels (display, structure, label1, label2)

INPUT Display		*display;
INPUT pxlStructure	structure;
INPUT long		label1;
INPUT long		label2;

{
/* AUTHOR: */
    pexDeleteBetweenLabelsReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (DeleteBetweenLabels, req);
    req->sid =  structure;
    req->label1 = label1;
    req->label2 = label2;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXCopyElements
 *
 * FORMAT:
 * 	`PEXCopyElements`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	srcStructure	An unsigned longword specifying the
 *			resource ID of the source structure.
 *
 *	srcWhence1	An integer specifying, with `srcOffset1',
 *			the beginning of the range of structure elements
 *			to be copied.
 *
 *	srcOffset1	A long integer specifying the offset from
 *			`srcWhence1' denoting the beginning of the
 *			range of structure elements to be copied.
 *
 *	srcWhence2	An integer specifying, with `srcOffset2',
 *			the end of the range of structure elements to
 *			be copied.
 *
 *	srcOffset2	A long integer specifying the offset from
 *			`srcWhence2' denoting the end of the range
 *			of structure elements to be copied.
 *
 *	destStructure	An unsigned longword specifying the
 *			resource ID of the destination structure.
 *
 *	destWhence	An integer specifying, with `destOffset',
 *		        the beginning position at which the elements
 *			copied from `srcStructure' are inserted
 *			into the destination structure `destStructure'.
 *
 *	destOffset	A long integer specifying the offset from
 *			`destWhence' denoting the beginning position
 *			for the copied elements in the destination
 *			structure `destStructure'.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine copies all of the elements from the structure specified as
 *	`srcStructure' to the structure specified as `destStructure'.
 *	The beginning and end of the range of elements to be copied from
 *	the source structure is computed from an initial element pointer
 *	position plus an offset.  The beginning of the range is computed
 *  	as the sum of `srcWhence1' and `srcOffset1'.  The end of the
 *	range is computed as the sum of `srcWhence2' plus `srcOffset2'.
 *	Possible values for `srcWhence1' and `srcWhence2' are `PEXBeginning',
 *	`PEXCurrent', and `PEXEnd'.  These constants are defined in the `PEX.h'
 *	include library.
 *	Arguments `srcOffset1' and `srcOffset2' can be any long integer value
 *	and can be negative.
 *
 *	For example, if `srcWhence1' is `PEXBeginning', the beginning of the
 *	range of elements to be copied has an offset equal to the
 *	beginning of the source structure plus the value of `srcOffset1'.
 *	If `srcWhence2' is `PEXCurrent', the end of the range of elements to be
 *	copied is equal to the value of the current element pointer position
 *	plus the value of the offset `srcOffset2'.
 *
 *	If a computed offset is less than zero, it is set to
 *	zero before obtaining the element information.  If either
 *	computed offset is greater than the number of elements in the
 *	structure, the offset is set to the offset of the last element in the
 *	structure.
 *
 *	The copied structure elements are inserted into `destStructure'
 *	at the element pointer position equal to an initial position
 *	`destWhence' plus an offset `destOffset'.   Possible values for
 *	`destWhence' are `PEXBeginning', `PEXCurrent', and `PEXEnd'.  If
 *	`destWhence' is `PEXBeginning', the destination structure element
 *	pointer position is the sum of the beginning of the
 *	destination structure and the offset `destOffset'.
 *	If `destWhence' is `PEXCurrent', the element pointer position in the
 *	destination is the sum of the current pointer position in
 *	`destStructure' plus the value of the offset `destOffset'.
 *
 *	Arguments `srcStructure' and `destStructure' can
 *	be the same.  In this case, the copy operation proceeds
 *	as though the indicated range were copied to a temporary location
 *	and then inserted relative to the destination position.
 *
 *	After the copy operation, the element pointer of the destination
 *	structure is updated to point at the last element copied
 *	into the destination structure.  The editing mode attribute of
 *	`destStructure' is ignored during this request.  The copied
 *	elements are always inserted into the destination structure
 *	and are never used to replace existing structure elements.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 *	Value		"Whence" is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateStructure"
 *
 */

void
PEXCopyElements (display, srcStructure, srcWhence1, srcOffset1, srcWhence2,
		srcOffset2, destStructure, destWhence, destOffset)

INPUT Display		*display;
INPUT pxlStructure	srcStructure;
INPUT int		srcWhence1;
INPUT long		srcOffset1;
INPUT int		srcWhence2;
INPUT long		srcOffset2;
INPUT pxlStructure	destStructure;
INPUT int		destWhence;
INPUT long		destOffset;

{
/* AUTHOR: */
    pexCopyElementsReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (CopyElements, req);
    req->src = srcStructure;
    req->srcRange.position1.whence = srcWhence1;
    req->srcRange.position1.offset = srcOffset1;
    req->srcRange.position2.whence = srcWhence2;
    req->srcRange.position2.offset = srcOffset2;
    req->dst = destStructure;
    req->dstPosition.whence = destWhence;
    req->dstPosition.offset = destOffset;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXChangeStructureRefs
 *
 * FORMAT:
 * 	`PEXChangeStructureRefs`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	oldStructure	An unsigned longword specifying the resource
 *			ID of the structure no longer to be
 *			referenced.
 *
 *	newStructure	An unsigned longword specifying the resource
 *			ID of the structure now referenced.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine changes "execute structure" elements in the server that
 *	reference the structure specified by `oldStructure' into
 *	"execute structure" elements which reference the structure
 *	specified as `newStructure'.  Both structures must already
 *	exist as valid structure resources.
 *
 *	Any references to `newStructure' that existed before this
 *	request are not affected.  If references to `oldStructure'
 *	exist and `newStructure' does not exist, an error is returned
 *	and no action is taken.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateStructure", `PEXDestroyStructures",
 *	`PEXCopyStructure", `PEXGetStructureInfo", `PEXGetStructuresInNetwork",
 *	`PEXGetAncestors", `PEXGetDescendants"
 *
 */

void
PEXChangeStructureRefs (display, oldStructure, newStructure)

INPUT Display		*display;
INPUT pxlStructure	oldStructure;
INPUT pxlStructure	newStructure;

{
/* AUTHOR: */
    pexChangeStructureRefsReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (ChangeStructureRefs, req);
    req->old_id = oldStructure;
    req->new_id = newStructure;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}
