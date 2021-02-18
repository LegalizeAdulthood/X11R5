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
**	pl_wks.c
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
**	This file contains the routines associated with PEX PHIGS Workstations.
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
 *   Table of Contents (Routine names in order of appearance)
 */

/*
 * E   PEXCreatePhigsWks
 * E   PEXFreePhigsWks
 * E   PEXGetWksInfo
 * E   PEXGetWksDynamics
 * E   PEXGetWksViewRep
 * E   PEXGetWksPostings
 * E   PEXSetWksViewPriority
 * E   PEXSetWksDisplayUpdateMode
 * E   PEXSetWksBufferMode
 * E   PEXSetWksViewRep
 * E   PEXSetWksWindow
 * E   PEXSetWksViewport
 * E   PEXSetWksHlhsrMode
 * E   PEXRedrawAllStructures
 * E   PEXUpdatePhigsWks
 * E   PEXExecuteDeferredActions
 * E   PEXMapDCtoWC
 * E   PEXMapWCtoDC
 * E   PEXPostStructure
 * E   PEXUnpostStructure
 * E   PEXUnpostAllStructures
 * E   PEXRedrawClipRegion
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
 * 	PEXCreatePhigsWks
 *
 * FORMAT:
 * 	`PEXCreatePhigsWks`
 *
 * ARGUMENTS:
 *  	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	d		Drawable
 *
 *	lineBundle	line bundle table
 *
 *	markerBundle	marker bundle table
 *
 *	textBundle	text bundle table
 *
 *	interiorBundle	interior bundle table
 *
 *	edgeBundle	edge bundle table
 *
 *	colourTable	colour table
 *
 *	patternTable	pattern table
 *
 *	textFontTable	text font table
 *
 *	depthCueTable	depth cue table
 *
 *	lightTable	light table
 *
 *	colourApproxTable	colour Approximation table
 *
 *	highlightIncl	highlight inclusion nameset
 *
 * 	highlightExcl	highlight exclusion nameset
 *
 *	invisibilityIncl	invisibility inclusion nameset
 *
 *	invisibilityExcl	invisibility exclusion nameset
 *
 *	bufferMode		single/double buffering mode
 *
 * RETURNS:
 *	The resource ID of the newly-created PHIGS workstation.
 *
 * DESCRIPTION:
 * 	[  To be documented.  ]
 *
 * ERRORS:
 *	IDChoice	ID already in use or not in range assigned to client.
 *
 *	Drawable	Specified drawable resource ID is invalid.
 *
 *	Match		Specified lookup table resource was not created for
 *			drawables of the same root and depth as the specified
 *			drawable.
 *
 *	LookupTable	A specified lookup table resource ID is invalid.
 *
 *	NameSet		A specified name set resource ID is invalid.
 *
 *	Alloc		Server failed to allocate the resource.
 *
 * SEE ALSO:
 *
 */

pxlPhigsWks
PEXCreatePhigsWks (display, d, lineBundle, markerBundle,
	textBundle, interiorBundle, edgeBundle, colourTable, patternTable,
	textFontTable, depthCueTable, lightTable, colourApproxTable,
	highlightIncl, 	highlightExcl, invisibilityIncl, invisibilityExcl,
	bufferMode)

INPUT Display		*display;
INPUT Drawable		d;
INPUT pxlLookupTable	lineBundle;
INPUT pxlLookupTable	markerBundle;
INPUT pxlLookupTable	textBundle;
INPUT pxlLookupTable	interiorBundle;
INPUT pxlLookupTable	edgeBundle;
INPUT pxlLookupTable	colourTable;
INPUT pxlLookupTable	patternTable;
INPUT pxlLookupTable	textFontTable;
INPUT pxlLookupTable	depthCueTable;
INPUT pxlLookupTable	lightTable;
INPUT pxlLookupTable	colourApproxTable;
INPUT pxlNameSet	highlightIncl;
INPUT pxlNameSet	highlightExcl;
INPUT pxlNameSet	invisibilityIncl;
INPUT pxlNameSet	invisibilityExcl;
INPUT int		bufferMode;

{
/* AUTHOR: */
    pxlPhigsWks		id;

    return (id);
}

/*+
 * NAME:
 * 	PEXFreePhigsWks
 *
 * FORMAT:
 * 	`PEXFreePhigsWks`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks		The resource ID of the PHIGS workstation resource
 *			that is to be freed.
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine deletes the PHIGS workstation resource `wks' and frees
 *	the storage associated with it.
 *
 * ERRORS:
 *	PhigsWKS	Specified PHIGS workstation resource ID is invalid.
 *
 * SEE ALSO:
 *
 */

void
PEXFreePhigsWks (display, wks)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;

{
/* AUTHOR: */
    return;
}

/*+
 * NAME:
 * 	PEXGetWksInfo
 *
 * FORMAT:
 * 	`PEXGetWksInfo`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks		workstation object
 *
 *	valueMask	bitmask of values to return
 *
 *	wksAttributesReturn	attributes
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	[  To be documented.  ]
 *
 * ERRORS:
 *	PhigsWKS		Specified PHIGS workstation resource ID is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 * SEE ALSO:
 *
 */
/*	Returns the requested components of the PHIGS workstation
 *	specified by `wks'.  Each bit in the bitmask `valueMask'
 *	corresponds to a "wks" attribute. If the bit is a 1, the value
 *	is returned in a structure pointed to by `wksAttributesReturn'.
 *
 *	The storage for the pxlPhigsWksInfo and the list of defined views is
 *	allocated by PEXlib, and can be deallocated by calling
 *	PEXFree(*wksAttributesReturn).
 */

Status
PEXGetWksInfo (display, wks, valueMask, wksAttributesReturn)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;
INPUT unsigned long	valueMask;
OUTPUT pxlPhigsWksInfo	**wksAttributesReturn;

{
/* AUTHOR: */
    return (Success);
}

/*+
 * NAME:
 * 	PEXGetWksDynamics
 *
 * FORMAT:
 * 	`PEXGetWksDynamics`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	d		drawable
 *
 *	wksDynamicsReturn	dynamics list
 *
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *
 *	Returns the requested dynamics of the PHIGS workstation
 *	specified by `wks'.  Each bit in the bitmask `valueMask'
 *	corresponds to a "wks" dynamics attribute. If the bit is a 1,
 *	the value is returned in structure pointed to by
 *	`wksDynamicsReturn'.
 *
 *	The storage for the PhigsWksDynamics is allocated by PEXlib,
 *	and can be deallocated by calling PEXFree(*wksDynamicsReturn).
 *
 *
 * ERRORS:
 *	Drawable	Specified drawable resource ID is invalid.
 *
 * SEE ALSO:
 *
 */

Status
PEXGetWksDynamics (display, d, wksDynamicsReturn)

INPUT Display			*display;
INPUT Drawable			d;
OUTPUT pxlPhigsWksDynamics	**wksDynamicsReturn;

{
/* AUTHOR: */
    return (Success);
}

/*+
 * NAME:
 * 	PEXGetWksViewRep
 *
 * FORMAT:
 * 	`PEXGetWksViewRep`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks		workstation ID
 *
 *	index		view table index
 *
 *	viewUpdateReturn 
 *
 *	reqViewReturn              
 *
 *	curViewReturn
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	[  To be documented.  ]
 *
 *	The two returned view reps are allocated by PEXlib and can be freed
 *	by calling PEXFree with a pointer to the structure (*reqViewReturn and
 *	*curViewReturn).  The viewUpdateReturn is NOT allocated by PEXlib.
 *
 *
 * ERRORS:
 *	PhigsWKS		Specified PHIGS workstation resource ID is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	Value			Specified view table entry is not defined.
 *
 * SEE ALSO:
 *
 */

Status
PEXGetWksViewRep (display, wks, index, viewUpdateReturn, reqViewReturn,
		curViewReturn)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;
INPUT unsigned int	index;
OUTPUT int		*viewUpdateReturn;
OUTPUT pxlViewEntry	**reqViewReturn;
OUTPUT pxlViewEntry	**curViewReturn;

{
/* AUTHOR: */
    return (Success);
}

/*+
 * NAME:
 * 	PEXGetWksPostings
 *
 * FORMAT:
 * 	`PEXGetWksPostings`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	structure	The resource ID of the structure whose postings
 *			are to be returned.
 *
 *	wksReturn	Returns a pointer to a list of PHIGS workstation
 *			resource IDs.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine returns in `wksReturn' the list of PHIGS workstation
 *	resources to which `structure' has been posted.
 *
 *	PEXlib allocates the storage for the list of PHIGS workstation
 *	resources.  PEXFree(*wksReturn) frees this storage.
 *
 * ERRORS:
 *	Structure	Specified structure resource ID is invalid.
 *
 * SEE ALSO:
 *
 */

Status
PEXGetWksPostings (display, structure, wksReturn)

INPUT Display		*display;
INPUT pxlStructure	structure;
OUTPUT pxlPhigsWks	**wksReturn;

{
/* AUTHOR: */
    return (Success);
}

/*+
 * NAME:
 * 	PEXSetWksViewPriority
 *
 * FORMAT:
 * 	`PEXSetWksViewPriority`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks		Workstation ID
 *
 *	index1		view table entry index 1
 *
 *	index2		view table entry index 2
 *
 *	priority	relative priority of index 1 with respect to index 2
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	[  To be documented.  ]
 *
 * ERRORS:
 *	PhigsWKS	Specified PHIGS workstation resource ID is invalid.
 *
 *	Value		"Priority" is invalid.
 *
 * SEE ALSO:
 *
 */

void
PEXSetWksViewPriority (display, wks, index1, index2, priority)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;
INPUT unsigned int	index1;
INPUT unsigned int	index2;
INPUT int		priority;

{
/* AUTHOR: */
    return;
}

/*+
 * NAME:
 * 	PEXSetWksDisplayUpdateMode
 *
 * FORMAT:
 * 	`PEXSetWksDisplayUpdateMode`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks		Workstation resource ID
 *
 *	displayUpdate
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *
 *	Sets the drawable update attribute of `wks' to `displayUpdate'.  (See
 *	protocol doc PEXSetDisplayUpdateMode.)
 *
 *
 * ERRORS:
 *	PhigsWKS	Specified PHIGS workstation resource ID is invalid.
 *
 *	Value		"Display_update" is invalid.
 *
 * SEE ALSO:
 *
 */

void
PEXSetWksDisplayUpdateMode (display, wks, displayUpdate)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;
INPUT int		displayUpdate;

{
/* AUTHOR: */
    return;
}


/*+
 * NAME:
 * 	PEXSetWksBufferMode
 *
 * FORMAT:
 * 	`PEXSetWksBufferMode`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks		Workstation resource ID
 *
 *	bufferMode	buffering mode
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *
 *	Sets the buffer mode of `wks'.  (See
 *	protocol doc PEXSetBufferMode.)
 *
 *
 * ERRORS:
 *	PhigsWKS	Specified PHIGS workstation resource ID is invalid.
 *
 *	Value		"bufferMode" is invalid.
 *
 * SEE ALSO:
 *
 */

void
PEXSetWksBufferMode (display, wks, bufferMode)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;
INPUT int		bufferMode;

{
/* AUTHOR: */
    return;
}



/*+
 * NAME:
 * 	PEXSetWksViewRep
 *
 * FORMAT:
 * 	`PEXSetWksViewRep`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks
 *
 *	viewIndex
 *
 *	viewRep
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	[  To be documented.  ]
 *
 * ERRORS:
 *	PhigsWKS		Specified PHIGS workstation resource ID is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 * SEE ALSO:
 *
 */

void
PEXSetWksViewRep (display, wks, viewIndex, viewRep)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;
INPUT unsigned int	viewIndex;
INPUT pxlViewEntry	*viewRep;

{
/* AUTHOR: */
    return;
}

/*+
 * NAME:
 * 	PEXSetWksWindow
 *
 * FORMAT:
 * 	`PEXSetWksWindow`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks
 *
 *	npcSubvolume
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	[  To be documented.  ]
 *
 * ERRORS:
 *	PhigsWKS		Specified PHIGS workstation resource ID is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 * SEE ALSO:
 *
 */

void
PEXSetWksWindow (display, wks, npcSubvolume)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;
INPUT pxlNpcSubvolume	*npcSubvolume;

{
/* AUTHOR: */
    return;
}

/*+
 * NAME:
 * 	PEXSetWksViewport
 *
 * FORMAT:
 * 	`PEXSetWksViewport`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks
 *
 *	viewport
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	[  To be documented.  ]
 *
 * ERRORS:
 *	PhigsWKS		Specified PHIGS workstation resource ID is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	Value			"Use_drawable" is invalid.
 *
 * SEE ALSO:
 *
 */

void
PEXSetWksViewport (display, wks, viewport)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;
INPUT pxlViewport	*viewport;

{
/* AUTHOR: */
    return;
}

/*+
 * NAME:
 * 	PEXSetWksHlhsrMode
 *
 * FORMAT:
 * 	`PEXSetWksHlhsrMode`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks
 *
 *	hlhsrMode
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	[  To be documented.  ]
 *
 * ERRORS:
 *	PhigsWKS	Specified PHIGS workstation resource ID is invalid.
 *
 *	Value		"Mode" is invalid.
 *
 * SEE ALSO:
 *
 */

void
PEXSetWksHlhsrMode (display, wks, hlhsrMode)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;
INPUT unsigned int	hlhsrMode;

{
/* AUTHOR: */
    return;
}

/*+
 * NAME:
 * 	PEXRedrawAllStructures
 *
 * FORMAT:
 * 	`PEXRedrawAllStructures`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *
 *	Redraw all of the posted structures for the PHIGS workstation "wks".
 *	Apply pending requests and set visual state to PEXCorrect.
 *
 *
 * ERRORS:
 *	PhigsWKS	Specified PHIGS workstation resource ID is invalid.
 *
 * SEE ALSO:
 *
 */

void
PEXRedrawAllStructures (display, wks)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;

{
/* AUTHOR: */
    return;
}

/*+
 * NAME:
 * 	PEXUpdatePhigsWks
 *
 * FORMAT:
 * 	`PEXUpdatePhigsWks`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	[  To be documented.  ]
 *
 * ERRORS:
 *	PhigsWKS	Specified PHIGS workstation resource ID is invalid.
 *
 * SEE ALSO:
 *
 */

void
PEXUpdatePhigsWks (display, wks)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;

{
/* AUTHOR: */
    return;
}

/*+
 * NAME:
 * 	PEXExecuteDeferredActions
 *
 * FORMAT:
 * 	`PEXExecuteDeferredActions`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	[  To be documented.  ]
 *
 * ERRORS:
 *	PhigsWKS	Specified PHIGS workstation resource ID is invalid.
 *
 * SEE ALSO:
 *
 */

void
PEXExecuteDeferredActions (display, wks)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;

{
/* AUTHOR: */
    return;
}

/*+
 * NAME:
 * 	PEXMapDCtoWC
 *
 * FORMAT:
 * 	`PEXMapDCtoWC`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks
 *
 *	dcPoints
 *
 *	numPoints
 *
 *	wcPointsReturn
 *
 * 	viewIndexReturn
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	[  To be documented.  ]
 *
 * ERRORS:
 *	PhigsWKS		Specified PHIGS workstation resource ID is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 * SEE ALSO:
 *
 */

void
PEXMapDCtoWC (display, wks, dcPoints, numPoints, wcPointsReturn,
		viewIndexReturn)

INPUT Display			*display;
INPUT pxlPhigsWks		wks;
INPUT pxlDeviceCoord		*dcPoints;
INPUT int			numPoints;
OUTPUT pxlCoord3D		*wcPointsReturn;
OUTPUT unsigned int		*viewIndexReturn;

{
/* AUTHOR: */
    return;
}

/*+
 * NAME:
 * 	PEXMapWCtoDC
 *
 * FORMAT:
 * 	`PEXMapWCtoDC`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks
 *
 *	wcPoints
 *
 *	numPoints
 *
 *	viewIndex
 *
 *	dcPointsReturn
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	[  To be documented.  ]
 *
 * ERRORS:
 *	PhigsWKS		Specified PHIGS workstation resource ID is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 * SEE ALSO:
 *
 */

void
PEXMapWCtoDC (display, wks, wcPoints, numPoints, viewIndex, dcPointsReturn)

INPUT Display			*display;
INPUT pxlPhigsWks		wks;
INPUT pxlCoord3D		*wcPoints;
INPUT int			numPoints;
INPUT unsigned int		viewIndex;
OUTPUT pxlDeviceCoord		*dcPointsReturn;

{
/* AUTHOR: */
    return;
}

/*+
 * NAME:
 * 	PEXPostStructure
 *
 * FORMAT:
 * 	`PEXPostStructure`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks
 *
 *	structure
 *
 *	priority
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	[  To be documented.  ]
 *
 * ERRORS:
 *	PhigsWKS	Specified PHIGS workstation resource ID is invalid.
 *
 *	Structure	Specified structure resource ID is invalid.
 *
 * SEE ALSO:
 *
 */

void
PEXPostStructure (display, wks, structure, priority)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;
INPUT pxlStructure	structure;
INPUT unsigned long	priority;

{
/* AUTHOR: */
    return;
}

/*+
 * NAME:
 * 	PEXUnpostStructure
 *
 * FORMAT:
 * 	`PEXUnpostStructure`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks
 *
 *	structure
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	[  To be documented.  ]
 *
 * ERRORS:
 *	PhigsWKS	Specified PHIGS workstation resource ID is invalid.
 *
 * 	Structure	Specified structure resource ID is invalid.
 *
 * SEE ALSO:
 *
 */

void
PEXUnpostStructure (display, wks, structure)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;
INPUT pxlStructure	structure;

{
/* AUTHOR: */
    return;
}


/*+
 * NAME:
 * 	PEXUnpostAllStructures
 *
 * FORMAT:
 * 	`PEXUnpostAllStructures`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	[  To be documented.  ]
 *
 * ERRORS:
 *	PhigsWKS	Specified PHIGS workstation resource ID is invalid.
 *
 * SEE ALSO:
 *
 */

void
PEXUnpostAllStructures (display, wks)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;

{
/* AUTHOR: */
    return;
}


/*+
 * NAME:
 * 	PEXRedrawClipRegion
 *
 * FORMAT:
 * 	`PEXRedrawClipRegion`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	wks		The resource ID of the PHIGS workstation resource
 *			that is to be.
 *
 *	deviceRectangles
 *
 *	numRectangles
 *
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	[  To be documented.  ]
 *
 * ERRORS:
 *	PhigsWKS	Specified PHIGS workstation resource ID is invalid.
 *
 * SEE ALSO:
 *
 */

void
PEXRedrawClipRegion (display, wks, deviceRectangles, numRectangles)

INPUT Display		*display;
INPUT pxlPhigsWks	wks;
INPUT pxlDeviceRect	*deviceRectangles;
INPUT int		numRectangles;

{
/* AUTHOR: */
    return;
}

