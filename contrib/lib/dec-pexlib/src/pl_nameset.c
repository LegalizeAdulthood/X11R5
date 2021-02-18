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
**	pl_nameset.c
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
**	This file contains the routines associated with PEX namesets.
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
**	18-Aug-88 
**
**--
*/

/*
 *   Table of Contents (routine names in order of appearance)
 */

/*
 * E   PEXCreateNameSet
 * E   PEXFreeNameSet
 * E   PEXCopyNameSet
 * E   PEXGetNameSet
 * E   PEXChangeNameSet
 */

/*
 *   Include Files
 */

#include "pex_features.h"
#include "pex_macros.h"

#include "PEXlib.h"
#include "PEXlibint.h"

#include "pl_ref_data.h"

/*+
 * NAME:
 * 	PEXCreateNameSet
 *
 * FORMAT:
 * 	`PEXCreateNameSet`
 *
 * ARGUMENTS:
 *	display		 A pointer to a display structure
 *			 returned by a successful `XOpenDisplay" call.
 *
 * RETURNS:
 *	The resource identifier of the newly created name set.
 *
 * DESCRIPTION:
 *  	This routine creates a name set resource and returns the resource
 *	identifier of the created name set.  The returned identifier can be
 *	used to refer to the created name set.
 *
 * ERRORS:
 *	IDChoice	Identifier already in use or not in range assigned to
 *                      user.
 *
 *	Alloc		Server failed to allocate the resource.
 *
 * SEE ALSO:
 *	`PEXFreeNameSet", `PEXCopyNameSet", `PEXGetNameSet",
 *	`PEXChangeNameSet", `PEXAddToNameSet", `PEXRemoveFromNameSet"
 *
 */

pxlNameSet
PEXCreateNameSet (display)

INPUT Display		*display;

{
/* AUTHOR: */
    pexCreateNameSetReq	*req;
    pxlNameSet		ns;

    /* get a nameset resource id from X */
    ns = XAllocID (display);

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (CreateNameSet, req);
    req->id = ns;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (ns);
}

/*+
 * NAME:
 * 	PEXFreeNameSet
 *
 * FORMAT:
 * 	`PEXFreeNameSet`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	ns		The resource identifier of the name set to be freed.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *  	This routine deletes the association between the name set resource
 *	identifier `ns' and the name set.  The actual name set storage in
 *	the server is freed when no other resource references it.
 *
 * ERRORS:
 *	NameSet		Specified name set resource identifier is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateNameSet", `PEXCopyNameSet", `PEXGetNameSet",
 *	`PEXChangeNameSet", `PEXAddToNameSet", `PEXRemoveFromNameSet"
 *
 */

void
PEXFreeNameSet (display, ns)

INPUT Display		*display;
INPUT pxlNameSet	ns;

{
/* AUTHOR: */
    pexFreeNameSetReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (FreeNameSet, req);
    req->id = ns;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXCopyNameSet
 *
 * FORMAT:
 * 	`PEXCopyNameSet`
 *
 * ARGUMENTS:
 *	display		 A pointer to a display structure
 *			 returned by a successful `XOpenDisplay" call.
 *
 *	srcNs		 The resource identifier of the source name set.
 *
 *	destNs		 The resource identifier of the destination name set.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine copies the contents of the source name set `srcNs' to the
 *	destination name set `destNs'.  Both must already exist as valid
 *	name set resources.  Values already in `destNs' are overwritten.
 *
 * ERRORS:
 *	NameSet		Specified name set resource identifier is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateNameSet", `PEXFreeNameSet", `PEXGetNameSet",
 *	`PEXChangeNameSet", `PEXAddToNameSet", `PEXRemoveFromNameSet"
 *
 */

void
PEXCopyNameSet (display, srcNs, destNs)

INPUT Display		*display;
INPUT pxlNameSet	srcNs;
INPUT pxlNameSet	destNs;

{
/* AUTHOR: */
    pexCopyNameSetReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (CopyNameSet, req);
    req->src = srcNs;
    req->dst = destNs;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXGetNameSet
 *
 * FORMAT:
 * 	`PEXGetNameSet`
 *
 * ARGUMENTS:
 *	display	 	A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	ns		The resource identifier of the name set to be queried.
 *
 *	namesReturn	Returns a pointer to the list of names
 *			stored in `ns'.
 *
 *	numNamesReturn 	Returns the number of entries in the returned list
 *			of names.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *  	This routine returns the names in the name set specified by `ns'.
 *	The list of names is returned in an array, and a pointer to the list is
 *	returned in `namesReturn'.  The number of returned names is
 *	returned in `numNamesReturn'.
 *
 *	PEXlib allocates storage for the list of returned names.
 *	`PEXFree"(`*namesReturn') frees the storage.
 *
 * ERRORS:
 *	NameSet		Specified name set resource identifier is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateNameSet", `PEXFreeNameSet", `PEXCopyNameSet",
 *	`PEXChangeNameSet", `PEXAddToNameSet", `PEXRemoveFromNameSet"
 *
 */

Status
PEXGetNameSet (display, ns, namesReturn, numNamesReturn)

INPUT Display		*display;
INPUT pxlNameSet	ns;
OUTPUT pxlName 		**namesReturn;
OUTPUT int		*numNamesReturn;

{
/* AUTHOR: */
    pexGetNameSetReq	*req;
    pexGetNameSetReply	rep;
    char		*pn;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (GetNameSet, req);
    req->id = ns;

    /* get a reply and read the fixed-length part of it */
    if (_XReply (display,  &rep, 0, xFalse) == 0)
    {/* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
	return (1); /* return an error */
    }

    /* allocate a buffer for the replies to pass back to the user */
    pn = (char *)PEXAllocBuf ((unsigned)(sizeof(PEXHeader) + (rep.length << 2)));
    ((PEXHeader *)pn)->free = _PEXFreeList;     /* routine to call to free it */
    pn = (char *)&(((PEXHeader *)pn)[1]);

    /* read the variable length data (list of names) into the buffer */
    if (rep.numNames)
        _XRead (display, (char *)pn, (long)(rep.length << 2));
    *namesReturn = (pxlName *)pn;
    *numNamesReturn = rep.numNames;

   /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	PEXChangeNameSet
 *
 * FORMAT:
 * 	`PEXChangeNameSet`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	ns		The resource identifier of the name set to be changed.
 *
 *	action		An integer specifying the type of change to
 *			be made to the name set.
 *
 *	values		A list of names used to modify
 *			the name set.
 *
 *	numValues	The number of names in the modification list.
 *
 * RETURNS:
 * 	None
 *
 * DESCRIPTION:
 *  	This routine modifies the name set resource `ns'.  The list of names
 *	pointed to by `values' is added to the name set if `action' is `PEXNSAdd',
 *	removed from the name set if `action' is `PEXNSRemove', or used to
 *	replace the names currently in the name set if `action' is `{PEXReplace OR PEXNSReplace}'.
 *	The constants defining the possible actions are defined in the `PEX.h'
 *	include file.  `NumValues' specifies the number of names in the
 *	`values' list.
 *
 * ERRORS:
 *	NameSet		Specified name set resource identifier is invalid.
 *
 *	Value		`Action' is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateNameSet", `PEXFreeNameSet", `PEXCopyNameSet",
 *	`PEXGetNameSet", `PEXAddToNameSet", `PEXRemoveFromNameSet"
 *
 */

void
PEXChangeNameSet (display, ns, action, values, numValues)

INPUT Display		*display;
INPUT pxlNameSet	ns;
INPUT int		action;
INPUT pxlName		*values;
INPUT int		numValues;

{
/* AUTHOR: */
    pexChangeNameSetReq	*req;
    int			size;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    size = numValues * sizeof(pxlName);
    PEXGetReqExtra (ChangeNameSet, size, req);
    req->ns = ns;
    req->action = action;
    COPY_AREA ((char *)values, ((char *)&(req[1])), size);

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}
