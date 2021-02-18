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
**	pl_free.c
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
**	This file contains the routines that free storage that has been
**	allocated by PEXlib.
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
 *   Table of Contents (Routine names in order of appearance)
 */

/*
 * E   PEXFree
 * I   _PEXFreeInfo
 * I   _PEXFreeEnumTypeDesc
 * I   _PEXFreeTableEntries
 * I   _PEXFreeList
 * I   _PEXFreeStructurePath
 * I   _PEXFreeRdrAttr
 * I   _PEXFreePCAttr
 * I   _PEXFreePtr
 * I   _PEXFreeFontInfo
 * I   _PEXFreeFontName

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
 * 	PEXFree
 *
 * FORMAT:
 * 	`PEXFree`
 *
 * ARGUMENTS:
 *	pch		A pointer to the memory to be deallocated.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	Frees memory PEXlib has allocated.  Argument `pch' passes a pointer to
 *	the memory PEXFree frees.
 *
 * 	WARNING:  Application programs cannot REALLOC() the structures
 *	PEXlib returns.
 *	When PEXlib allocates memory for the user program, it puts some
 *	data before the location it passes back.  PEXlib needs this data
 *	to free the storage.  Attempts to free storage not allocated
 *	by PEXlib or storage the user program tampered with will have
 *	unexpected results.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *
 */

void
PEXFree (pch)

INPUT char	*pch;

{
 /* AUTHOR: */
    /* back up past the PEX header and call the appropriate routine to free
       the memory */
    (*(((PEXHeader *)pch)[-1]).free) (pch);

    return;
}

/* Routine to free pxlInfo structure, which has one string hanging off of
 * vendorName */

void
_PEXFreeInfo (pInfo)

INPUT pxlInfo	*pInfo;

{
    char	*p;

    /* free the vendor name, which has no PEX header */
    Xfree ((char *)(pInfo->vendorName));

    /* then back up past the PEX header and free the pxlInfo structure */
    Xfree ((char *)&(((PEXHeader *)pInfo)[-1]));

    return;
}

/* Routine to free array of pxlEnumTypeDescList, as returned by Get Enumerated
Type Descriptions */

void
_PEXFreeEnumTypeDesc (pDescs)

INPUT pxlEnumTypeDescList	*pDescs;

{
    int		i, numDescs;

    numDescs = (((PEXHeader *)pDescs)[-1]).size;

    /* for each descriptor in the list */
    for (i=0; i<numDescs; i++)
    {
	/* free the descriptor string, if any (no header) */
	if (pDescs[i].string != NULL)
	{
	    Xfree ((char *)(pDescs[i].string));
	}
    }

    /* back up past the header and free the descriptor data structure */
    Xfree ((char *)&(((PEXHeader *)pDescs)[-1]));

    return;
}

/* Routine to free an array of table entries, as returned by Get Predefined
Entries, Get Table Entry and Get Table Entries */

/* Since all the memory is contiguous, no matter how many entries are passed
in, a single Xfree call is all that's necessary, unless there are colours in
the entries */

void
_PEXFreeTableEntries (pEntry)

INPUT char	*pEntry;

{
    int			numEntries;
    int			i;
    char		*nextEntry;

    if ( (((PEXHeader *)pEntry)[-1]).type == PEXPatternLUT )
    {
	numEntries = (((PEXHeader *)pEntry)[-1]).size;
        nextEntry  = pEntry;

	for (i=0; i<numEntries; i++)
	{
	    Xfree ( (char *)((pxlPatternEntry *)nextEntry)->colours);
	    nextEntry += sizeof(pxlPatternEntry);
	}
    }
    else if ( (((PEXHeader *)pEntry)[-1]).type == PEXTextFontLUT )
    {
	numEntries = (((PEXHeader *)pEntry)[-1]).size;
        nextEntry  = pEntry;

	for (i=0; i<numEntries; i++)
	{
	    Xfree ( (char *)((pxlTextFontEntry *)nextEntry)->fonts);
	    nextEntry += sizeof(pxlTextFontEntry);
	}
    }

    /* back up past the header and free the table entry */
    Xfree ( (char *)&(((PEXHeader *)pEntry)[-1]) );

    return;
}

/* Routine to free a piece of memory that has no pointers to other places */

void
_PEXFreeList (pList)

INPUT unsigned short	*pList;

{
    /* back up past the header and free it */
    Xfree ((char *)&(((PEXHeader *)pList)[-1]));

    return;
}

/* Routine to free a structure path, as returned by Get Ancestors and Get
Descendants */

void
_PEXFreeStructurePath (psp)

INPUT pxlStructurePath	*psp;

{
    int		i;
    int		numPaths;

    numPaths = (((PEXHeader *)psp)[-1]).size;

    /* free all the element references */
    for (i=0; i<numPaths; i++)
    {
	Xfree ((char *)(psp[i].elementRefs));
    }

    /* back up past the header and free the main structure */
    Xfree ((char *)&(((PEXHeader *)psp)[-1]));

    return;
}

/* Routine to free a renderer attributes structure, as returned by Get
Renderer */

void
_PEXFreeRdrAttr (pra)

INPUT pxlRendererAttributes	*pra;

{
    /* free the current path storage, if any */
    if (pra->currentPath.elementRefs != NULL)
    {
	Xfree ((char *)(pra->currentPath.elementRefs));
    }

    /* free the clip list storage, if any */
    if (pra->clipList.deviceRect != NULL)
    {
	Xfree ((char *)(pra->currentPath.elementRefs));
    }

    /* back up past the header and free the main structure */
    Xfree ((char *)&(((PEXHeader *)pra)[-1]));

    return;
}

/* Routine to free a pipeline context structure, as returned by Get Pipeline
Context */

void
_PEXFreePCAttr (ppca)

INPUT pxlPCAttributes	*ppca;

{
    /* free the half spaces */
    if (ppca->modelClipVolume.halfSpace != NULL)
    {
        Xfree ((char *)(ppca->modelClipVolume.halfSpace));
    }

    /* free the lights */
    if (ppca->lightState.lightIndex != NULL)
    {
        Xfree ((char *)(ppca->lightState.lightIndex));
    }

    /* free the PSC data */
    if (ppca->psc.pscData != NULL)
    {
        Xfree ((char *)(ppca->psc.pscData));
    }

    /* back up past the PEX header and free the main structure */
    Xfree ((char *)&(((PEXHeader *)ppca)[-1]));
}

/* Routine to free memory with one pointer (stored in .type) */

void
_PEXFreePtr (buf)

INPUT char	*buf;

{
    /* free the sub-structure, pointed to by .type */
    if ((((PEXHeader *)buf)[-1]).type != NULL)
    {
	Xfree ((char *)(((PEXHeader *)buf)[-1]).type);
    }

    /* back up past the header and free the main structure */
    Xfree ((char *)&(((PEXHeader *)buf)[-1]));
}

/* Routine to free memory of font (fontInfoReturn) */

/* (Might someday be removed in favor of FreeList) */

void
_PEXFreeFontInfo (buf)

INPUT pxlFontInfo	*buf;

{
/*
 *XXX a pxlFontInfo should really contain a pointer to a list of properties.
 *XXX this routine should go through a list of font info and free the 
 *  list of properties.. see pl_font.c
 */

    /* free the header information */
    Xfree ((char *)&(((PEXHeader *)buf)[-1]));
}

/* Routine to free font (namesReturn) */

void
_PEXFreeFontName (buf)

INPUT pxlStringData *buf;

{
    int		i;
    int		numStrings;

    numStrings = (((PEXHeader *)buf)[-1]).size;

    /* free the char strings */
    for (i=0; i<numStrings; i++)
    {
	Xfree (buf[i].chars);
    }

    /* free the header information */
    Xfree ((char *)&(((PEXHeader *)buf)[-1]));
}
