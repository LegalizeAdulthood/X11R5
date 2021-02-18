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
**	pl_lut.c
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
**	This file contains the routines associated with PEX lookup tables.
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
 * E   PEXCreateLookupTable
 * E   PEXFreeLookupTable
 * E   PEXCopyLookupTable
 * E   PEXGetTableInfo
 * I   _PEXRepackLUTEntries
 * E   PEXGetPredefinedEntries
 * E   PEXGetDefinedIndices
 * E   PEXGetTableEntry
 * E   PEXGetTableEntries
 * E   PEXSetTableEntries
 * E   PEXDeleteTableEntries
 */

/*   Include Files
 */

#include "pex_features.h"
#include "pex_macros.h"

#include "PEXlib.h"
#include "PEXlibint.h"

#ifdef MPEX
#include "MPEX.h"
#include "MPEXprotostr.h"
#include "MPEXlib.h"
#endif

#include "pl_ref_data.h"

    extern char		*_XAllocScratch();

/*
 * Copy Colour Specifier Data. Note that pxlColour is a union.  This macro
 * only copies the valid colour data (ie: it uses the true colour length)
 */ 
#define PackColourSpecifier(srcBuf, dstBuf, sizeColour) \
{ \
    ((pxlColourSpecifier *)(dstBuf))->colourType = \
        ((pxlColourSpecifier *)(srcBuf))->colourType; \
    sizeColour = \
	ColourSizeFromType ( ((pxlColourSpecifier *)(srcBuf))->colourType ); \
    COPY_SMALL_AREA( &(((pxlColourSpecifier *)(srcBuf))->colour), \
	&(((pxlColourSpecifier *)(dstBuf))->colour), \
	sizeColour); \
}

#define GetLUTEntryBuffer(lutType, n, entryType, buf) \
{ \
    (buf) = (PEXHeader *) \
	PEXAllocBuf( sizeof(PEXHeader) + ((n) * (sizeof(entryType))) ); \
    (buf)->size = (n); \
    (buf)->type = (lutType); \
    (buf)->free = _PEXFreeTableEntries; \
    (buf)++; \
}


/*+
 * NAME:
 * 	PEXCreateLookupTable
 *
 * FORMAT:
 *	`PEXCreateLookupTable`
 *
 * ARGUMENTS:
 * 	display		A pointer to a display structure returned by a
 *			successful `XOpenDisplay" call.
 *
 * 	d		The resource identifier of an X window or pixmap.
 *
 * 	type		The type of lookup table to be created.
 *
 * RETURNS:
 *	The resource identifier of the newly created lookup table.
 *
 *
 * DESCRIPTION:
 * 	This routine creates a lookup table resource and returns the resource
 *	identifier of the
 *	new lookup table.  The returned identifier can be used to
 *	refer to the created lookup table.  `Type' specifies the type of
 *	lookup table to be created and can be one of the following:
 *
 *	>
 *	`PEXLineBundleLUT'
 *	`PEXMarkerBundleLUT'
 *	`PEXTextBundleLUT'
 *	`PEXInteriorBundleLUT'
 *	`PEXEdgeBundleLUT'
 *	`PEXPatternLUT'
 *	`PEXTextFontLUT'
 *	`PEXColourLUT'
 *	`PEXViewLUT'
 *	`PEXLightLUT'
 *	`PEXDepthCueLUT'
 *	`PEXColourApproxLUT'
 *	<
 *
 *	The `PEX.h' include file contains definitions of these types.
 *
 *	In addition, if the PEX server extension supports the Digital extensions
 *	to the renderer resource, `type' can be one of the following, defined
 *	in the `MPEX.h' include file.
 *
 *	>
 *	`MPEXHighlightLUT'
 *	`MPEXEchoLUT'
 *	`MPEXPixmapLUT'
 *	<
 *
 *	The newly-created lookup table can
 *	only be used with drawables having the same depth and root as
 *	the drawable `d'.
 *
 * ERRORS:
 *	IDChoice	Identifier already in use or not in range assigned to
 *			client.
 *
 *	Drawable	Specified drawable resource identifier is invalid.
 *
 *	Value		Table type is invalid.
 *
 *	Alloc		Server failed to allocate the resource.
 *
 * SEE ALSO:
 *     	`PEXFreeLookupTable", `PEXCopyLookupTable", `PEXGetTableInfo",
 *	`PEXGetPredefinedEntries", `PEXGetDefinedIndices", `PEXGetTableEntry",
 *	`PEXGetTableEntries", `PEXSetTableEntries", `PEXDeleteTableEntries",
 *	`PEXCreateRenderer", `PEXSetLineBundleIndex", `PEXSetMarkerBundleIndex",
 *  	`PEXSetTextBundleIndex", `PEXSetInteriorBundleIndex", `PEXSetEdgeBundleIndex",
 *	`PEXSetInteriorStyleIndex", `PEXSetBFInteriorStyleIndex", `PEXSetTextFontIndex",
 *	`PEXSetViewIndex", `PEXSetDepthCueIndex", `PEXSetMarkerColourIndex",
 *	`PEXSetMarkerColour", `PEXSetTextColourIndex", `PEXSetTextColour",
 *	`PEXSetLineColourIndex", `PEXSetLineColour", `PEXSetSurfaceColourIndex",
 *	`PEXSetSurfaceColour", `PEXSetBFSurfaceColourIndex", `PEXSetBFSurfaceColour",
 *      `PEXSetEdgeColourIndex", `PEXSetEdgeColour", `PEXSetReflectionAttributes",
 *  	`PEXSetBFReflectionAttributes", `MPEXSetHighlightIndex", `MPEXSetEchoIndex",
 *	`MPEXAnnotationPixmap"
 *
 */

pxlLookupTable
PEXCreateLookupTable (display, d, type)

INPUT Display	*display;
INPUT Drawable	d;
INPUT int	type;

{
/* AUTHOR: */
    pxlLookupTable		id;
    pexCreateLookupTableReq	*req;

    /* get a lookup table resource id from X */
    id = XAllocID (display);

    /* lock around critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (CreateLookupTable, req);
    req->drawableExample = d;
    req->lut = id;
    req->tableType = type;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (id);
}

/*+
 * NAME:
 * 	PEXFreeLookupTable
 *
 * FORMAT:
 *	`PEXFreeLookupTable`
 *
 * ARGUMENTS:
 * 	display		A pointer to a display structure returned
 *			by a successful `XOpenDisplay" call.
 *
 * 	lut		The resource identifier of the lookup table to be
 *			freed.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *  	This routine deletes the association between the lookup table
 *	resource identifier `lut'
 * 	and the lookup table.  The lookup table storage is freed
 *	when no other resource references it.
 *
 * ERRORS:
 *	LookupTable	Lookup table resource identifier is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateLookupTable", `PEXCopyLookupTable", `PEXGetTableInfo",
 *	`PEXGetPredefinedEntries", `PEXGetDefinedIndices", `PEXGetTableEntry",
 *	`PEXGetTableEntries", `PEXSetTableEntries", `PEXDeleteTableEntries"
 *
 */

void
PEXFreeLookupTable (display, lut)

INPUT Display		*display;
INPUT pxlLookupTable	lut;

{
/* AUTHOR: */
    pexFreeLookupTableReq     *req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (FreeLookupTable, req);
    req->id = lut;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXCopyLookupTable
 *
 * FORMAT:
 *	`PEXCopyLookupTable`
 *
 * ARGUMENTS:
 * 	display		A pointer to a display structure returned
 *			by a successful `XOpenDisplay" call.
 *
 * 	srcLut		The resource identifier of the lookup table from which
 *			to copy entries.
 *
 * 	destLut		The resource identifier of the lookup table to which
 *			entries are to be copied.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine copies entries from lookup table `srcLut' to
 *      lookup table `destLut'.  Both tables must already exist as valid
 *	lookup table resources.  Both must have been created for
 *	the same class of drawables, and both must be the same
 *	table type.  Previous `destLut' values are overwritten.
 *
 * ERRORS:
 *	LookupTable	Lookup table resource identifier is invalid.
 *
 *	Match		Lookup tables not for use with same class of drawables
 *			or not the same type.
 *
 * SEE ALSO:
 *	`PEXCreateLookupTable", `PEXFreeLookupTable", `PEXGetTableInfo",
 *	`PEXGetPredefinedEntries", `PEXGetDefinedIndices", `PEXGetTableEntry",
 *	`PEXGetTableEntries", `PEXSetTableEntries", `PEXDeleteTableEntries"
 *
 */

void
PEXCopyLookupTable (display, srcLut, destLut)

INPUT Display		*display;
INPUT pxlLookupTable	srcLut;
INPUT pxlLookupTable	destLut;

{
/* AUTHOR: */
    pexCopyLookupTableReq     *req;

    /* lock around critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (CopyLookupTable, req);
    req->src = srcLut;
    req->dst = destLut;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXGetTableInfo
 *
 * FORMAT:
 *	`PEXGetTableInfo`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 * 	d		The resource identifier of an X window or pixmap.
 *
 *  	type		The type of lookup table to be queried.
 *
 *	tableInfoReturn
 *			Returns a pointer to information about the specified
 *			lookup table type.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine returns information about the specified type of
 *	lookup table.
 *	`Type' specifies the type of lookup table and can be one of the
 *	following:
 *
 *	>
 *	`PEXLineBundleLUT'
 *	`PEXMarkerBundleLUT'
 *	`PEXTextBundleLUT'
 *	`PEXInteriorBundleLUT'
 *	`PEXEdgeBundleLUT'
 *	`PEXPatternLUT'
 *	`PEXTextFontLUT'
 *	`PEXColourLUT'
 *	`PEXViewLUT'
 *	`PEXLightLUT'
 *	`PEXDepthCueLUT'
 *	`PEXColourApproxLUT'
 *	<
 *
 *	The `PEX.h' include file contains definitions of these types.
 *
 *	In addition, if the PEX server extension supports the Digital extensions
 *	to the renderer resource, `type' can be one of the following, defined
 *	in the `MPEX.h' include file.
 *
 *	>
 *	`MPEXHighlightLUT'
 *	`MPEXEchoLUT'
 *	`MPEXPixmapLUT'
 *	<
 *
 *	The returned information is based on the assumption that
 *	the lookup table would be used on drawables with the same root
 *	and depth as drawable `d'.  A pointer to the returned information
 *	is returned in `tableInfoReturn'.  The returned information includes the
 *	number of predefined table entries, the number of definable table
 *	entries, and the indices of the predefined minimum and maximum entries.
 *
 *	PEXlib allocates the storage for `tableInfoReturn'.
 *	`PEXFree"(`*tableInfoReturn') deallocates the memory.
 *
 * ERRORS:
 *	Drawable	Specified drawable resource identifier is invalid.
 *
 *	Value		Table type is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateLookupTable", `PEXFreeLookupTable", `PEXCopyLookupTable",
 *      `PEXGetPredefinedEntries", `PEXGetDefinedIndices", `PEXGetTableEntry",
 *	`PEXGetTableEntries", `PEXSetTableEntries", `PEXDeleteTableEntries"
 *
 */

Status
PEXGetTableInfo (display, d, type, tableInfoReturn)

INPUT Display		*display;
INPUT Drawable		d;
INPUT int		type;
OUTPUT pxlTableInfo	**tableInfoReturn;

{
/* AUTHOR: */
    pexGetTableInfoReq     *req;
    pexGetTableInfoReply   rep;
    char		   *pt;

    /* lock around critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (GetTableInfo, req);
    req->drawableExample = d;
    req->tableType = type;

    /* get a reply */
    if (_XReply (display, &rep, 0, xTrue) == 0)
    {/* there was an error */
	UnlockDisplay (display);
	PEXSyncHandle ();
	return (1);            /* return an error */
    }

    /* allocate a buffer for the replies to pass back to the user */
    pt = (char *)PEXAllocBuf
	((unsigned)(sizeof(PEXHeader) + sizeof(pxlTableInfo)));
    ((PEXHeader *)pt)->free = _PEXFreeList;  /* routine to call to free it */
    pt += sizeof(PEXHeader);

    /* fill the output buffer */
    ((pxlTableInfo *)pt)->definableEntries = rep.definableEntries;
    ((pxlTableInfo *)pt)->numPredefined = rep.numPredefined;
    ((pxlTableInfo *)pt)->predefinedMin = rep.predefinedMin;
    ((pxlTableInfo *)pt)->predefinedMax = rep.predefinedMax;
    *tableInfoReturn = (pxlTableInfo *)pt;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/* This routine repacks the lut entries returned by PEXGetTableEntry,
PEXGetTableEntries, and PEXGetPredefinedEntries.  This is mostly to change the
colours, which may be of different sizes, to a fixed size to make it
easier for application programs.  This isn't the fastest way to process
replies that have no colours, but it's not expected to be used a whole lot. */

char *
_PEXRepackLUTEntries (pt, numEntries, type)

INPUT  char		*pt;
INPUT  int		numEntries;
INPUT  int		type;

{
    int			size;
    int			sizeColour;
    int			colourType;
    PEXHeader		*buf;
    int			i;
    int			j;
    int			numx;
    int			numy;


    switch (type)
    {
    case PEXLineBundleLUT:
     {
        pxlLineBundleEntry *dst;
	pexLineBundleEntry *src = (pexLineBundleEntry *) pt;

	GetLUTEntryBuffer(type, numEntries, pxlLineBundleEntry, buf);
	dst = (pxlLineBundleEntry *) buf;

	for (i=0; i<numEntries; i++, dst++)
	{
	    dst->lineType = src->lineType;
	    dst->polylineInterp = src->polylineInterp;
	    dst->curveApprox = src->curveApprox;
	    dst->lineWidth = src->lineWidth;

            PackColourSpecifier( &(src->lineColour), &(dst->lineColour),
                sizeColour);

            src = (pexLineBundleEntry *) ((char *)src +
                sizeof(pexLineBundleEntry) + sizeColour);
	}
	break;
     }
    case PEXMarkerBundleLUT:
     {
        pxlMarkerBundleEntry *dst;
        pexMarkerBundleEntry *src = (pexMarkerBundleEntry *) pt;

	GetLUTEntryBuffer(type, numEntries, pxlMarkerBundleEntry, buf);
	dst = (pxlMarkerBundleEntry *) buf;

	for (i=0; i<numEntries; i++, dst++)
	{
	    dst->markerType = src->markerType;
	    dst->markerScale = src->markerScale;

            PackColourSpecifier( &(src->markerColour), &(dst->markerColour),
                sizeColour);

            src = (pexMarkerBundleEntry *) ((char *)src +
                sizeof(pexMarkerBundleEntry) + sizeColour);

	}
	break;
     }

    case PEXTextBundleLUT:
     {
        pxlTextBundleEntry *dst;
        pexTextBundleEntry *src = (pexTextBundleEntry *)pt;

	GetLUTEntryBuffer(type, numEntries, pxlTextBundleEntry, buf);
	dst = (pxlTextBundleEntry *) buf;

	for (i=0; i<numEntries; i++, dst++)
	{
	    dst->textFontIndex = src->textFontIndex;
	    dst->textPrecision = src->textPrecision;
	    dst->charExpansion = src->charExpansion;
	    dst->charSpacing = src->charSpacing;

            PackColourSpecifier( &(src->textColour), &(dst->textColour),
                sizeColour);

            src = (pexTextBundleEntry *) ((char *)src +
                sizeof(pexTextBundleEntry) + sizeColour);

	}
	break;
     }

    case PEXInteriorBundleLUT:
     {
        pxlInteriorBundleEntry *dst;
        char *src = (char *) pt;

	GetLUTEntryBuffer(type, numEntries, pxlInteriorBundleEntry, buf);
	dst = (pxlInteriorBundleEntry *) buf;

	for (i=0; i<numEntries; i++, dst++)
	{
	    dst->interiorStyle =
		((pexInteriorBundleEntry *)src)->interiorStyle;
	    dst->interiorStyleIndex =
		((pexInteriorBundleEntry *)src)->interiorStyleIndex;
	    dst->reflectionModel =
		((pexInteriorBundleEntry *)src)->reflectionModel;
	    dst->surfaceInterp =
		((pexInteriorBundleEntry *)src)->surfaceInterp;
	    dst->bfInteriorStyle =
		((pexInteriorBundleEntry *)src)->bfInteriorStyle;
	    dst->bfInteriorStyleIndex =
		((pexInteriorBundleEntry *)src)->bfInteriorStyleIndex;
	    dst->bfReflectionModel =
		((pexInteriorBundleEntry *)src)->bfReflectionModel;
	    dst->bfSurfaceInterp =
		((pexInteriorBundleEntry *)src)->bfSurfaceInterp;
	    dst->surfaceApprox =
		((pexInteriorBundleEntry *)src)->surfaceApprox;
	    src += sizeof(pexInteriorBundleEntry);

            /* copy surfaceColour */
            PackColourSpecifier( src, &(dst->surfaceColour), sizeColour);
            src += sizeof(pexColourSpecifier) + sizeColour;

            /* copy reflectionAttr */
            dst->reflectionAttr.ambient = 
            	((pexReflectionAttr *)src)->ambient;
            dst->reflectionAttr.diffuse =
            	((pexReflectionAttr *)src)->diffuse;
            dst->reflectionAttr.specular =
            	((pexReflectionAttr *)src)->specular;
	    dst->reflectionAttr.specularConc =
                ((pexReflectionAttr *)src)->specularConc;
	    dst->reflectionAttr.transmission =
                ((pexReflectionAttr *)src)->transmission;
            PackColourSpecifier(
                &(((pexReflectionAttr *)src)->specularColour),
                &(dst->reflectionAttr.specularColour),
                sizeColour);
            src += sizeof(pexReflectionAttr) + sizeColour;

            /* copy bfSurfaceColour */
            PackColourSpecifier( src, &(dst->bfSurfaceColour), sizeColour);
            src += sizeof(pexColourSpecifier) + sizeColour;

   	    /* copy bfReflectionAttr */
	    dst->bfReflectionAttr.ambient =
                ((pexReflectionAttr *)src)->ambient;
	    dst->bfReflectionAttr.diffuse =
                ((pexReflectionAttr *)src)->diffuse;
	    dst->bfReflectionAttr.specular =
                ((pexReflectionAttr *)src)->specular;
	    dst->bfReflectionAttr.specularConc =
                ((pexReflectionAttr *)src)->specularConc;
	    dst->bfReflectionAttr.transmission =
                ((pexReflectionAttr *)src)->transmission;
            PackColourSpecifier(
                &(((pexReflectionAttr *)src)->specularColour),
                &(dst->bfReflectionAttr.specularColour),
                sizeColour);
            src += sizeof(pexReflectionAttr) + sizeColour;
        }
	break;
     }

    case PEXEdgeBundleLUT:
     {
	pxlEdgeBundleEntry *dst;
        pexEdgeBundleEntry *src = (pexEdgeBundleEntry *) pt;
         
	GetLUTEntryBuffer(type, numEntries, pxlEdgeBundleEntry, buf);
	dst = (pxlEdgeBundleEntry *)buf;

	for (i=0; i<numEntries; i++, dst++)
	{
	    dst->edges = src->edges;
	    dst->edgeType = src->edgeType;
	    dst->edgeWidth = src->edgeWidth;

            PackColourSpecifier( &(src->edgeColour), &(dst->edgeColour),
                sizeColour);

            src = (pexEdgeBundleEntry *)((char *)src +
                sizeof(pexEdgeBundleEntry) + sizeColour);
    
	}
	break;
     }
    case PEXPatternLUT:
     {
    	pxlPatternEntry *dst;
        pexPatternEntry *src = (pexPatternEntry *)pt;
    
	GetLUTEntryBuffer(type, numEntries, pxlPatternEntry, buf);
	dst = (pxlPatternEntry *)buf;

	for (i=0; i<numEntries; i++, dst++)
	{
	    dst->colourType = src->colourType;
	    dst->numx = src->numx;
	    dst->numy = src->numy;

            sizeColour = ColourSizeFromType (src->colourType);
            sizeColour *= (src->numx * src->numy);
	    dst->colours = (char *) PEXAllocBuf ((unsigned)sizeColour);
            COPY_AREA (&(src[1]), dst->colours, sizeColour);

            src = (pexPatternEntry *) ((char *)src +
                sizeof(pexPatternEntry) + sizeColour);
	}
	break;
     }

    case PEXTextFontLUT:
     {
    	pxlTextFontEntry *dst;
        pexTextFontEntry *src = (pexTextFontEntry *)pt;

	GetLUTEntryBuffer(type, numEntries, pxlTextFontEntry, buf);
	dst = (pxlTextFontEntry *)buf;

	for (i=0; i<numEntries; i++, dst++)
        {
	    dst->numFonts = src->numFonts;
	    dst->fonts = (pexFont *) PEXAllocBuf(src->numFonts*sizeof(pexFont));
            COPY_AREA (&(src[1]), dst->fonts, src->numFonts*sizeof(pexFont));

	    src = (pexTextFontEntry *) ((char *)src + 
		sizeof(pexTextFontEntry) + (src->numFonts * sizeof(pexFont)) );
	}

	break;
      }

    case PEXColourLUT:
     {
        pxlColourEntry *dst;
        pexColourSpecifier *src = (pexColourSpecifier *) pt;
    
	GetLUTEntryBuffer(type, numEntries, pxlColourEntry, buf);
	dst = (pxlColourEntry *) buf;

	for (i=0; i<numEntries; i++, dst++)
	{
            PackColourSpecifier( src, &(dst->colour), sizeColour);

            src = (pexColourSpecifier *)((char *)src +
                sizeof(pexColourSpecifier) + sizeColour);
 	}
    
	break;
     }

    case PEXViewLUT:
	GetLUTEntryBuffer(type, numEntries, pxlViewEntry, buf);

	COPY_AREA ((char *)pt, (char *)buf, numEntries*sizeof(pexViewEntry));
	break;

    case PEXLightLUT:
     {
	pxlLightEntry *dst;
        pexLightEntry *src = (pexLightEntry *) pt;
    
	GetLUTEntryBuffer(type, numEntries, pxlLightEntry, buf);
	dst = (pxlLightEntry *) buf;

	for (i=0; i<numEntries; i++, dst++)
	{
	    dst->lightType = src->lightType;
	    dst->direction = src->direction;
	    dst->point = src->point;
	    dst->concentration = src->concentration;
	    dst->spreadAngle = src->spreadAngle;
	    dst->attenuation1 = src->attenuation1;
	    dst->attenuation2 = src->attenuation2;

            PackColourSpecifier( &(src->lightColour), &(dst->lightColour),
                sizeColour);

            src = (pexLightEntry *)((char *)src +
                sizeof(pexLightEntry) + sizeColour);
    
	}
	break;
     }

    case PEXDepthCueLUT:
     {
    	pxlDepthCueEntry *dst;
        pexDepthCueEntry *src = (pexDepthCueEntry *) pt;
    
	GetLUTEntryBuffer(type, numEntries, pxlDepthCueEntry, buf);
	dst = (pxlDepthCueEntry *) buf;

	for (i=0; i<numEntries; i++, dst++)
	{
	    dst->mode = src->mode;
	    dst->frontPlane = src->frontPlane;
	    dst->backPlane = src->backPlane;
	    dst->frontScaling = src->frontScaling;
	    dst->backScaling = src->backScaling;

            PackColourSpecifier( &(src->depthCueColour), &(dst->depthCueColour),
                sizeColour);

            src = (pexDepthCueEntry *)((char *)src +
                sizeof(pexDepthCueEntry) + sizeColour);
    
	}
	break;
     }

    case PEXColourApproxLUT:
	GetLUTEntryBuffer(type, numEntries, pxlColourApproxEntry, buf);

	COPY_AREA ((char *)pt, (char *)buf,
		numEntries*sizeof(pexColourApproxEntry));
	break;

#ifdef MPEX
    case MPEXHighlightLUT:
     {
	mpxlHighlightEntry *dst;
        mpexHighlightEntry *src = (mpexHighlightEntry *) pt;
    
	GetLUTEntryBuffer(type, numEntries, mpxlHighlightEntry, buf);
	dst = (mpxlHighlightEntry *) buf;

	for (i=0; i<numEntries; i++, dst++)
	{
	    dst->method = src->method;

            PackColourSpecifier( &(src->colour), &(dst->colour), sizeColour);

            src = (mpexHighlightEntry *)((char *)src +
                sizeof(mpexHighlightEntry) + sizeColour);
    
	}
	break;
     }

    case MPEXEchoLUT:
     {
	mpxlEchoEntry *dst;
        mpexEchoEntry *src = (mpexEchoEntry *) pt;
       
	GetLUTEntryBuffer(type, numEntries, mpxlEchoEntry, buf);
	dst = (mpxlEchoEntry *) buf;

	for (i=0; i<numEntries; i++, dst++)
	{
	    dst->method = src->method;

            PackColourSpecifier( &(src->colour), &(dst->colour), sizeColour);

            src = (mpexEchoEntry *)((char *)src +
                sizeof(mpexEchoEntry) + sizeColour);
    
	}
	break;
     }

    case MPEXPixmapLUT:
	GetLUTEntryBuffer(type, numEntries, mpxlPixmapEntry, buf);

	COPY_AREA ((char *)pt, (char *)buf, numEntries*sizeof(mpxlPixmapEntry));
	break;
#endif
    }

    return ((char *) buf);
}

/*+
 * NAME:
 * 	PEXGetPredefinedEntries
 *
 * FORMAT:
 *	`PEXGetPredefinedEntries`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 * 	d		The resource identifier of an X window or pixmap.
 *
 *	type            The type of lookup being queried.
 *
 *	start		The index of the first predefined entry
 *  			to be returned.
 *
 *	count		The number of predefined entries to be
 *			returned.
 *
 *	entriesReturn   Returns a pointer to an array of predefined entries.
 *
 *	numEntriesReturn
 *		        The number of table entries returned
 *			in `entriesReturn'.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine returns the predefined entries for the specified lookup
 *	table `type'.
 *	`Type' can be one of the following:
 *
 *	>
 *	`PEXLineBundleLUT'
 *	`PEXMarkerBundleLUT'
 *	`PEXTextBundleLUT'
 *	`PEXInteriorBundleLUT'
 *	`PEXEdgeBundleLUT'
 *	`PEXPatternLUT'
 *	`PEXTextFontLUT'
 *	`PEXColourLUT'
 *	`PEXViewLUT'
 *	`PEXLightLUT'
 *	`PEXDepthCueLUT'
 *	`PEXColourApproxLUT'
 *	<
 *
 *	The `PEX.h' include file contains definitions of these types.
 *
 *	In addition, if the PEX server extension supports the Digital extensions
 *	to the renderer resource, `type' can be one of the following, defined
 *	in the `MPEX.h' include file.
 *
 *	>
 *	`MPEXHighlightLUT'
 *	`MPEXEchoLUT'
 *	`MPEXPixmapLUT'
 *	<
 *
 *	Predefined entries are those automatically filled with valid data
 *	when a lookup table is created.
 *	The query is conducted based on the assumption that the
 *	lookup table would be used on drawables with the same root and depth as
 *	drawable `d'.  The routine returns at most `count' entries,
 *	starting with the entry whose index is `start'.
 *	A pointer to the list of predefined entries
 *	obtained from the PEX server extension is returned in
 *	array `entriesReturn'.  The
 *	number of entries contained in this array is returned in
 *	`numEntriesReturn'.
 *
 *	`EntriesReturn' is an array of structures.
 *	The type of structure depends on `type'.
 *	See `PEXSetTableEntries" for the structure types.
 *
 *	`PEXlib' allocates storage for the returned entries.
 *	`PEXFree"(`*entriesReturn') deallocates the memory.
 *
 * ERRORS:
 *	Drawable		Specified drawable resource identifier is
 *				invalid.
 *
 *	Value			Table type is invalid, start is less than the
 *				minimum predefined entry, or the sum of start
 *				and count is greater than the maximum predefined
 *				entry.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 * SEE ALSO:
 *	`PEXCreateLookupTable", `PEXFreeLookupTable", `PEXCopyLookupTable",
 *	`PEXGetTableInfo", `PEXGetDefinedIndices", `PEXGetTableEntry",
 *	`PEXGetTableEntries", `PEXSetTableEntries", `PEXDeleteTableEntries"
 *
 */

Status
PEXGetPredefinedEntries (display, d, type, start, count, entriesReturn,
			numEntriesReturn)

INPUT Display		*display;
INPUT Drawable		d;
INPUT int		type;
INPUT int		start;
INPUT int		count;
OUTPUT char		**entriesReturn;
OUTPUT int		*numEntriesReturn;

{
/* AUTHOR: */
    pexGetPredefinedEntriesReq		*req;
    pexGetPredefinedEntriesReply	rep;
    char				*pt;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (GetPredefinedEntries, req);
    req->fpFormat = pexFpFormat;
    req->drawableExample = d;
    req->tableType = type;
    req->start = start;
    req->count = count;

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
	UnlockDisplay (display);
	PEXSyncHandle ();
	return (1);          /* return an error */
    }

    /* allocate a buffer for the replies to pass back to the user */
    pt = (char *)_XAllocScratch (display, (unsigned long)(rep.length << 2));

    /* copy the table entries to the buffer */
    _XRead (display, (char *)pt, (long)(rep.length << 2));

    *entriesReturn = _PEXRepackLUTEntries ((char *)pt, (int)rep.numEntries,
	type);

    *numEntriesReturn = rep.numEntries;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	PEXGetDefinedIndices
 *
 * FORMAT:
 *	`PEXGetDefinedIndices`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	lut		The resource identifier of the lookup table to be
 *			queried.
 *
 *	indicesReturn	Returns a pointer to an array containing index
 *			values of defined table entries.
 *
 *	numIndicesReturn
 *			Returns the number of values in `indicesReturn'.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine returns the defined indices for the lookup table `lut'.
 *	The index
 *	of each defined table entry is returned in a list.  A pointer to
 *	this list is returned in `indicesReturn'.  The number of index values
 *	in this list is returned in `numIndicesReturn'.
 *
 *	`PEXlib' allocates storage for the returned list of indices.
 *	`PEXFree"(`*indicesReturn') deallocates the memory.
 *
 * ERRORS:
 *	LookupTable	Lookup table resource identifier is invalid.
 *
 * SEE ALSO:
 *	`PEXCreateLookupTable", `PEXFreeLookupTable", `PEXCopyLookupTable",
 *	`PEXGetTableInfo", `PEXGetPredefinedEntries", `PEXGetTableEntry",
 *	`PEXGetTableEntries", `PEXSetTableEntries", `PEXDeleteTableEntries"
 *
 */

Status
PEXGetDefinedIndices (display, lut, indicesReturn, numIndicesReturn)

INPUT Display		*display;
INPUT pxlLookupTable	lut;
OUTPUT unsigned short	**indicesReturn;
OUTPUT int		*numIndicesReturn;

{
/* AUTHOR: */
    pexGetDefinedIndicesReq	*req;
    pexGetDefinedIndicesReply	rep;
    char			*pt;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (GetDefinedIndices, req);
    req->id = lut;

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
	UnlockDisplay (display);
	PEXSyncHandle ();
	return (1);              /* return an error */
    }

    /* allocate a buffer for the replies to pass back to the user */
    pt = (char *)PEXAllocBuf
	((unsigned)(sizeof(PEXHeader) + (rep.length << 2)));
    ((PEXHeader *)pt)->free = _PEXFreeList;    /* routine to call to free it */
    pt = (char *)&(((PEXHeader *)pt)[1]);

    /* copy the indices to the buffer */
    _XRead (display, (char *)pt, (long)(rep.length << 2));
    *indicesReturn = (pxlTableIndex *)pt;
    *numIndicesReturn = rep.numIndices;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	PEXGetTableEntry
 *
 * FORMAT:
 * 	`PEXGetTableEntry`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	lut		The resource identifier of the lookup table from which
 *			to obtain the table entry.
 *
 *	index		The index of the entry to be returned from the
 *			lookup table.
 *
 *	valueType	Set or Realized
 *
 *	statusReturn	The constant `PEXDefinedEntry' if the specified lookup table
 *			entry is defined, and `PEXDefaultEntry' if it is undefined.
 *
 *	entryReturn	Returns a pointer to the returned lookup table entry.
 *
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine returns a single entry from the lookup table `lut'.
 *	`Index' specifies the table entry to be returned.
 *	If the specified lookup table entry is defined, the defined
 *	entry is returned.  If the specified lookup table entry is not
 *	defined, the default entry for that type
 *	of table is returned.  A pointer to the returned entry is returned in
 *	`entryReturn'.
 *
 *	The type of structure returned in `entryReturn' depends on `type'.
 *	See `PEXSetTableEntries" for the structure types.
 *
 *	PEXlib allocates storage for the returned table entry.
 *	`PEXFree"(`*entryReturn') deallocates the memory.
 *
 * ERRORS:
 *	LookupTable		Lookup table resource identifier is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 * SEE ALSO:
 *	`PEXCreateLookupTable", `PEXFreeLookupTable", `PEXCopyLookupTable",
 *	`PEXGetTableInfo", `PEXGetPredefinedEntries", `PEXGetDefinedIndices",
 *	`PEXGetTableEntries", `PEXSetTableEntries", `PEXDeleteTableEntries"
 *
 */

Status
PEXGetTableEntry (display, lut, index, valueType, statusReturn, entryReturn)

INPUT Display		*display;
INPUT pxlLookupTable	lut;
INPUT int		index;
INPUT int		valueType;
OUTPUT int		*statusReturn;
OUTPUT char		**entryReturn;

{
/* AUTHOR: */
    pexGetTableEntryReq		*req;
    pexGetTableEntryReply	rep;
    char			*pt;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (GetTableEntry, req);
    req->fpFormat = pexFpFormat;
    req->valueType = valueType;
    req->lut = lut;
    req->index = index;

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
	UnlockDisplay (display);
	PEXSyncHandle ();
	return (1);               /* return an error */
    }

    *statusReturn = rep.status;

    /* allocate a buffer for the replies to pass back to the user */
    pt = (char *)_XAllocScratch (display, (unsigned long)(rep.length << 2));

    /* copy the table entries to the buffer */
    _XRead (display, (char *)pt, (long)(rep.length << 2));

    *entryReturn = _PEXRepackLUTEntries ((char *)pt, 1, (int)rep.tableType);

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	PEXGetTableEntries
 *
 * FORMAT:
 * 	`PEXGetTableEntries`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	lut		The resource identifier of the lookup table from which
 *			the table entries are to be obtained.
 *
 *	start		The index of the first lookup table entry to
 *			be returned.
 *
 *	count		The number of entries to be returned.
 *
 *	valueType	Set or Realized
 *
 *	entriesReturn	Returns a pointer to the returned array of entries.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine returns `count' entries from the lookup table specified
 *	by `lut',
 *	starting at the entry whose index is specified by `start'.  A
 *	pointer to the array of returned entries is returned in
 *	`entriesReturn'.  If a table entry in the requested range is
 *	not defined, the default entry for a table of this type is returned.
 *
 *	`EntriesReturn' is an array of structures.
 *	The type of structure depends on `type'.
 *	See `PEXSetTableEntries" for the structure types.
 *
 *	PEXlib allocate storage for this array.  `PEXFree"(`*entriesReturn')
 *	deallocates the memory.
 *
 * ERRORS:
 *	LookupTable		Lookup table resource identifier is invalid.
 *
 *	Value			The sum of start and count is greater than
 *				2**16.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 * SEE ALSO:
 *	`PEXCreateLookupTable", `PEXFreeLookupTable", `PEXCopyLookupTable",
 *	`PEXGetTableInfo", `PEXGetPredefinedEntries", `PEXGetDefinedIndices",
 *	`PEXGetTableEntry", `PEXSetTableEntries", `PEXDeleteTableEntries"
 *
 */

Status
PEXGetTableEntries (display, lut, start, count, valueType, entriesReturn)

INPUT Display		*display;
INPUT pxlLookupTable	lut;
INPUT int		start;
INPUT int		count;
INPUT int		valueType;
OUTPUT char		**entriesReturn;

{
/* AUTHOR: */
    pexGetTableEntriesReq	*req;
    pexGetTableEntriesReply	rep;
    char			*pt;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (GetTableEntries, req);
    req->fpFormat = pexFpFormat;
    req->valueType = valueType;
    req->lut = lut;
    req->start = start;
    req->count = count;

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
	UnlockDisplay (display);
	PEXSyncHandle ();
	return (1);         /* return an error */
    }

    /* allocate a buffer for the replies to pass back to the user */
    pt = (char *)_XAllocScratch (display, (unsigned long)(rep.length << 2));

    /* copy the table entries to the buffer */
    _XRead (display, (char *)pt, (long)(rep.length << 2));

    *entriesReturn = _PEXRepackLUTEntries ((char *)pt, (int)rep.numEntries,
	(int)rep.tableType);

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	PEXSetTableEntries
 *
 * FORMAT:
 *	`PEXSetTableEntries`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	lut		The resource identifier of the lookup table whose
 *			entries are to be set.
 *
 *	type		The type of lookup table whose entries to be set.
 *
 *	start 		The index of the first table entry to be set.
 *
 *	count		The number of table entries to be set.
 *
 *	entries		A pointer to the array containing the
 *			table entries to be set.
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine sets `count' lookup table entries in the lookup table
 *	specified by
 *	`lut', starting at the entry specified by `start'.  `Type' defines
 *	the type of lookup table.  The list of table entries
 *	set is pointed to by `entries'.
 *
 *	`Entries' must point to an array of structures having one of the
 *	following types:
 *
 *	(Types defined in the `PEXlib.h' include file)
 *	>
 *	`pxlTextFontEntry' if `type' is `PEXTextFontLUT'
 *	`pxlViewEntry' if `type' is `PEXViewLUT'
 *	`pxlColourApproxEntry' if `type' is `PEXColourApproxLUT'
 *	`pxlLineBundleEntry' if `type' is `PEXLineBundleLUT'
 *	`pxlMarkerBundleEntry' if `type' is `PEXMarkerBundleLUT'
 *	`pxlTextBundleEntry' if `type' is `PEXTextBundleLUT'
 *	`pxlInteriorBundleEntry' if `type' is `PEXInteriorBundleLUT'
 *	`pxlEdgeBundleEntry' if `type' is `PEXEdgeBundleLUT'
 *	`pxlLightEntry' if `type' is `PEXLightLUT'
 *	`pxlDepthCueEntry' if `type' is `PEXDepthCueLUT'
 *	`pxlColourEntry' if `type' is `PEXColourLUT'
 *	`pxlPatternEntryData' if `type' is `PEXPatternLUT'
 *	<
 *
 *	(In addition, if the PEX server extension supports the Digital extensions
 *	to the renderer resource, these types are defined in the
 *	`MPEXlib.h' include file)
 *
 *	>
 *	`mpxlHighlightEntry' if `type' is `MPEXHighlightLUT'
 *	`mpxlEchoEntry' if `type' is `MPEXEchoLULUTT'
 *	`mpxlPixmapEntry' if `type' is `MPEXPixmapLUT'
 *	<
 *
 *	The type of structure returned in the array depends on the table type
 *	of `lut'.
 *
 *
 * ERRORS:
 *	LookupTable		Lookup table resouce identifier is invalid.
 *
 *	Value			The sum of start plus count is greater than
 *				2**16, or a table entry field contains an
 *				illegal value, or there are no more
 *				definable entries.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	ColourType		Device does not support the specified colour
 *				type.
 *
 * SEE ALSO:
 *	`PEXCreateLookupTable", `PEXFreeLookupTable", `PEXCopyLookupTable",
 *	`PEXGetTableInfo", `PEXGetPredefinedEntries", `PEXGetDefinedIndices",
 *	`PEXGetTableEntry", `PEXGetTableEntries", `PEXDeleteTableEntries"
 *
 */

void
PEXSetTableEntries (display, lut, type, start, count, entries)

INPUT Display		*display;
INPUT pxlLookupTable	lut;
INPUT int		type;
INPUT int		start;
INPUT int		count;
INPUT char		*entries;

{
/* AUTHOR: */
    pexSetTableEntriesReq	*req;
    int		 		size;
    int				sizeColour;
    unsigned long		*buf;
    int				i;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (SetTableEntries, req);
    req->fpFormat = pexFpFormat;
    req->lut = lut;
    req->start = start;
    req->count = count;

    switch (type)
    {
    case PEXLineBundleLUT:
      {
        pexLineBundleEntry *dst, *dstStart;
        pxlLineBundleEntry *src = (pxlLineBundleEntry *) entries;

	dstStart = dst = (pexLineBundleEntry *)
	    _XAllocScratch (display, count * sizeof(pxlLineBundleEntry));

	/* Copy the data, converting the colours.  Note that a pxlColour */
	/* may be larger than the actual colour data			*/
	for (i=0; i<count; i++, src++)
	{
	    dst->lineType = src->lineType;
	    dst->polylineInterp = src->polylineInterp;
	    dst->curveApprox = src->curveApprox;
	    dst->lineWidth = src->lineWidth;

	    PackColourSpecifier( &(src->lineColour), &(dst->lineColour),
		sizeColour);

	    dst = (pexLineBundleEntry *) ((char *)dst + 
		sizeof(pexLineBundleEntry) + sizeColour);
	}

	/* update the request length */
	size = (char *)dst - (char *)dstStart;
	req->length += (size + 3) >> 2;

	/* add the table entry data to the end of the X request */
	Data (display, (char *)dstStart, size);

	break;
      }

    case PEXMarkerBundleLUT:
      {
        pexMarkerBundleEntry *dst, *dstStart;
        pxlMarkerBundleEntry *src = (pxlMarkerBundleEntry *) entries;

	dstStart = dst = (pexMarkerBundleEntry *)
	    _XAllocScratch (display, count * sizeof(pxlMarkerBundleEntry));

	/* Copy the data, converting the colours.  Note that a pxlColour */
	/* may be larger than the actual colour data			*/
	for (i=0; i<count; i++, src++)
	{
	    dst->markerType = src->markerType;
	    dst->markerScale = src->markerScale;

	    PackColourSpecifier( &(src->markerColour), &(dst->markerColour),
		sizeColour);

	    dst = (pexMarkerBundleEntry *) ((char *)dst + 
		sizeof(pexMarkerBundleEntry) + sizeColour);
	}

	/* update the request length */
	size = (char *)dst - (char *)dstStart;
	req->length += (size + 3) >> 2;

	/* add the table entry data to the end of the X request */
	Data (display, (char *)dstStart, size);
	break;
      }

    case PEXTextBundleLUT:
      {
        pexTextBundleEntry *dst, *dstStart;
        pxlTextBundleEntry *src = (pxlTextBundleEntry *) entries;

	dstStart = dst = (pexTextBundleEntry *)
	    _XAllocScratch (display, count * sizeof(pxlTextBundleEntry));

	/* Copy the data, converting the colours.  Note that a pxlColour */
	/* may be larger than the actual colour data			*/
	for (i=0; i<count; i++, src++)
	{
	    dst->textFontIndex = src->textFontIndex;
	    dst->textPrecision = src->textPrecision;
	    dst->charExpansion = src->charExpansion;
	    dst->charSpacing = src->charSpacing;

	    PackColourSpecifier( &(src->textColour), &(dst->textColour),
		sizeColour);

	    dst = (pexTextBundleEntry *) ((char *)dst + 
		sizeof(pexTextBundleEntry) + sizeColour);
	}

	/* update the request length */
	size = (char *)dst - (char *)dstStart;
	req->length += (size + 3) >> 2;

	/* add the table entry data to the end of the X request */
	Data (display, (char *)dstStart, size);
	break;
      }

    case PEXInteriorBundleLUT:
      {
        char *dst, *dstStart;
        pxlInteriorBundleEntry *src = (pxlInteriorBundleEntry *) entries;

	dstStart = dst = (char *)
	    _XAllocScratch (display, count * sizeof(pxlInteriorBundleEntry));

	/* Copy the data, converting the colours.  Note that a pxlColour */
	/* may be larger than the actual colour data			*/
	for (i=0; i<count; i++, src++)
	{
	    ((pexInteriorBundleEntry *)dst)->interiorStyle =
		src->interiorStyle;
	    ((pexInteriorBundleEntry *)dst)->interiorStyleIndex =
		src->interiorStyleIndex;
	    ((pexInteriorBundleEntry *)dst)->reflectionModel =
		src->reflectionModel;
	    ((pexInteriorBundleEntry *)dst)->surfaceInterp =
		src->surfaceInterp;
	    ((pexInteriorBundleEntry *)dst)->bfInteriorStyle =
		src->bfInteriorStyle;
	    ((pexInteriorBundleEntry *)dst)->bfInteriorStyleIndex =
		src->bfInteriorStyleIndex;
	    ((pexInteriorBundleEntry *)dst)->bfReflectionModel =
		src->bfReflectionModel;
	    ((pexInteriorBundleEntry *)dst)->bfSurfaceInterp =
		src->bfSurfaceInterp;
	    ((pexInteriorBundleEntry *)dst)->surfaceApprox =
		src->surfaceApprox;
	    dst += sizeof(pexInteriorBundleEntry);

	    /* copy surfaceColour */
	    PackColourSpecifier( &(src->surfaceColour), dst, sizeColour);
	    dst += sizeof(pexColourSpecifier) + sizeColour;

	    /* copy reflectionAttr */
	    ((pexReflectionAttr *)dst)->ambient = src->reflectionAttr.ambient;
	    ((pexReflectionAttr *)dst)->diffuse = src->reflectionAttr.diffuse;
	    ((pexReflectionAttr *)dst)->specular = src->reflectionAttr.specular;
	    ((pexReflectionAttr *)dst)->specularConc = 
		src->reflectionAttr.specularConc;
	    ((pexReflectionAttr *)dst)->transmission = 
		src->reflectionAttr.transmission;
	    PackColourSpecifier( 
		&(src->reflectionAttr.specularColour),
		&(((pexReflectionAttr *)dst)->specularColour), 
		sizeColour);
	    dst += sizeof(pexReflectionAttr) + sizeColour;

	    /* copy bfSurfaceColour */
	    PackColourSpecifier( &(src->bfSurfaceColour), dst, sizeColour);
	    dst += sizeof(pexColourSpecifier) + sizeColour;

	    /* copy bfReflectionAttr */
	    ((pexReflectionAttr *)dst)->ambient = src->bfReflectionAttr.ambient;
	    ((pexReflectionAttr *)dst)->diffuse = src->bfReflectionAttr.diffuse;
	    ((pexReflectionAttr *)dst)->specular = 
		src->bfReflectionAttr.specular;
	    ((pexReflectionAttr *)dst)->specularConc = 
		src->bfReflectionAttr.specularConc;
	    ((pexReflectionAttr *)dst)->transmission = 
		src->bfReflectionAttr.transmission;
	    PackColourSpecifier( 
		&(src->bfReflectionAttr.specularColour),
		&(((pexReflectionAttr *)dst)->specularColour), 
		sizeColour);
	    dst += sizeof(pexReflectionAttr) + sizeColour;
	}

	/* update the request length */
	size = (char *)dst - (char *)dstStart;
	req->length += (size + 3) >> 2;

	/* add the table entry data to the end of the X request */
	Data (display, (char *)dstStart, size);
	break;
      }

    case PEXEdgeBundleLUT:
      {
        pexEdgeBundleEntry *dst, *dstStart;
        pxlEdgeBundleEntry *src = (pxlEdgeBundleEntry *) entries;

	dstStart = dst = (pexEdgeBundleEntry *)
	    _XAllocScratch (display, count * sizeof(pxlEdgeBundleEntry));

	/* Copy the data, converting the colours.  Note that a pxlColour */
	/* may be larger than the actual colour data			*/
	for (i=0; i<count; i++, src++)
	{
	    dst->edges = src->edges;
	    dst->edgeType = src->edgeType;
	    dst->edgeWidth = src->edgeWidth;

	    PackColourSpecifier( &(src->edgeColour), &(dst->edgeColour), 
		sizeColour);

	    dst = (pexEdgeBundleEntry *)((char *)dst + 
		sizeof(pexEdgeBundleEntry) + sizeColour);
	}

	/* update the request length */
	size = (char *)dst - (char *)dstStart;
	req->length += (size + 3) >> 2;

	/* add the table entry data to the end of the X request */
	Data (display, (char *)dstStart, size);
	break;
      }

    case PEXPatternLUT:
      {
        pexPatternEntry *dst, *dstStart;
        pxlPatternEntry *src = (pxlPatternEntry *)entries;

	/* allocate some scratch memory to hold the table entries (max size) */
	size = count * sizeof(pexPatternEntry);
	for (i=0; i<count; i++)
	{
	    size += src[i].numx * src[i].numy * sizeof(pxlColour);
	}
	dstStart = dst = (pexPatternEntry *)
	    _XAllocScratch (display, (unsigned long)size);

	/* copy the data to the scratch memory, converting the colours */
	for (i=0; i<count; i++, src++)
	{
	    dst->colourType = src->colourType;
	    dst->numx = src->numx;
	    dst->numy = src->numy;

	    sizeColour = ColourSizeFromType (src->colourType);
	    sizeColour *= (src->numx * src->numy);
	    COPY_AREA (src->colours, &(dst[1]), sizeColour);

	    dst = (pexPatternEntry *) ((char *)dst + 
		sizeof(pexPatternEntry) + sizeColour);
	}

	/* update the request length */
	size = (char *)dst - (char *)dstStart;
	req->length += (size + 3) >> 2;

	/* add the table entry data to the end of the X request */
	Data (display, (char *)dstStart, size);
	break;
      }

    case PEXTextFontLUT:
      {
        pexTextFontEntry *dst, *dstStart;
        pxlTextFontEntry *src = (pxlTextFontEntry *)entries;

	/* allocate some scratch memory to hold the table entries (max size) */
	size = count * sizeof(pexTextFontEntry);
	for (i=0; i<count; i++)
	{
	    size += src[i].numFonts * sizeof( pexFont);
	}
	dstStart = dst = (pexTextFontEntry *)
	    _XAllocScratch (display, (unsigned long)size);

	/* copy the data to the scratch memory */
	for (i=0; i<count; i++, src++)
	{
	    dst->numFonts = src->numFonts;

	    COPY_AREA (src->fonts, &(dst[1]), src->numFonts * sizeof(pexFont));

	    dst = (pexTextFontEntry *) ((char *)dst + 
		sizeof(pexTextFontEntry) + (src->numFonts * sizeof(pexFont)) );
	}

        /* update the request length */
        size = (char *)dst - (char *)dstStart;
        req->length += (size + 3) >> 2;

        /* add the table entry data to the end of the X request */
        Data (display, (char *)dstStart, size);

	break;
      }

    case PEXColourLUT:
      {
        pexColourSpecifier *dst, *dstStart;
        pxlColourEntry *src = (pxlColourEntry *) entries;

	dstStart = dst = (pexColourSpecifier *)
	    _XAllocScratch (display, count * sizeof(pxlColourEntry));

	/* Copy the data, converting the colours.  Note that a pxlColour */
	/* may be larger than the actual colour data			*/
	for (i=0; i<count; i++, src++)
	{
	    PackColourSpecifier( &(src->colour), dst, sizeColour);

	    dst = (pexColourSpecifier *)((char *)dst + 
		sizeof(pexColourSpecifier) + sizeColour);
	}

	/* update the request length */
	size = (char *)dst - (char *)dstStart;
	req->length += (size + 3) >> 2;

	/* add the table entry data to the end of the X request */
	Data (display, (char *)dstStart, size);
	break;
      }

    case PEXViewLUT:
	size = count * sizeof(pxlViewEntry);

	/* update the request length */
	req->length += (size + 3) >> 2;

	/* add the table entry data to the end of the X request */
	Data (display, (char *)entries, size);
	break;

    case PEXLightLUT:
      {
        pexLightEntry *dst, *dstStart;
        pxlLightEntry *src = (pxlLightEntry *) entries;

	dstStart = dst = (pexLightEntry *)
	    _XAllocScratch (display, count * sizeof(pxlLightEntry));

	/* Copy the data, converting the colours.  Note that a pxlColour */
	/* may be larger than the actual colour data			*/
	for (i=0; i<count; i++, src++)
	{
	    dst->lightType = src->lightType;
	    dst->direction = src->direction;
	    dst->point = src->point;
	    dst->concentration = src->concentration;
	    dst->spreadAngle = src->spreadAngle;
	    dst->attenuation1 = src->attenuation1;
	    dst->attenuation2 = src->attenuation2;

	    PackColourSpecifier( &(src->lightColour), &(dst->lightColour), 
		sizeColour);

	    dst = (pexLightEntry *)((char *)dst + 
		sizeof(pexLightEntry) + sizeColour);
	}

	/* update the request length */
	size = (char *)dst - (char *)dstStart;
	req->length += (size + 3) >> 2;

	/* add the table entry data to the end of the X request */
	Data (display, (char *)dstStart, size);
	break;
      }

    case PEXDepthCueLUT:
      {
        pexDepthCueEntry *dst, *dstStart;
        pxlDepthCueEntry *src = (pxlDepthCueEntry *) entries;

	dstStart = dst = (pexDepthCueEntry *)
	    _XAllocScratch (display, count * sizeof(pxlDepthCueEntry));

	/* Copy the data, converting the colours.  Note that a pxlColour */
	/* may be larger than the actual colour data			*/
	for (i=0; i<count; i++, src++)
	{
	    dst->mode = src->mode;
	    dst->frontPlane = src->frontPlane;
	    dst->backPlane = src->backPlane;
	    dst->frontScaling = src->frontScaling;
	    dst->backScaling = src->backScaling;

	    PackColourSpecifier( &(src->depthCueColour), &(dst->depthCueColour), 
		sizeColour);

	    dst = (pexDepthCueEntry *)((char *)dst + 
		sizeof(pexDepthCueEntry) + sizeColour);
	}

	/* update the request length */
	size = (char *)dst - (char *)dstStart;
	req->length += (size + 3) >> 2;

	/* add the table entry data to the end of the X request */
	Data (display, (char *)dstStart, size);
	break;
      }

    case PEXColourApproxLUT:
	size = count * sizeof(pxlColourApproxEntry);

	/* update the request length */
	req->length += (size + 3) >> 2;

	/* add the table entry data to the end of the X request */
	Data (display, (char *)entries, size);
	break;

#ifdef MPEX
    case MPEXHighlightLUT:
      {
        mpexHighlightEntry *dst, *dstStart;
        mpxlHighlightEntry *src = (mpxlHighlightEntry *) entries;

	dstStart = dst = (mpexHighlightEntry *)
	    _XAllocScratch (display, count * sizeof(mpxlHighlightEntry));

	/* Copy the data, converting the colours.  Note that a pxlColour */
	/* may be larger than the actual colour data			*/
	for (i=0; i<count; i++, src++)
	{
	    dst->method = src->method;

	    PackColourSpecifier( &(src->colour), &(dst->colour), sizeColour);

	    dst = (mpexHighlightEntry *)((char *)dst + 
		sizeof(mpexHighlightEntry) + sizeColour);
	}

	/* update the request length */
	size = (char *)dst - (char *)dstStart;
	req->length += (size + 3) >> 2;

	/* add the table entry data to the end of the X request */
	Data (display, (char *)dstStart, size);
	break;
      }

    case MPEXEchoLUT:
      {
        mpexEchoEntry *dst, *dstStart;
        mpxlEchoEntry *src = (mpxlEchoEntry *) entries;

	dstStart = dst = (mpexEchoEntry *)
	    _XAllocScratch (display, count * sizeof(mpxlEchoEntry));

	/* Copy the data, converting the colours.  Note that a pxlColour */
	/* may be larger than the actual colour data			*/
	for (i=0; i<count; i++, src++)
	{
	    dst->method = src->method;

	    PackColourSpecifier( &(src->colour), &(dst->colour), sizeColour);

	    dst = (mpexEchoEntry *)((char *)dst + 
		sizeof(mpexEchoEntry) + sizeColour);
	}

	/* update the request length */
	size = (char *)dst - (char *)dstStart;
	req->length += (size + 3) >> 2;

	/* add the table entry data to the end of the X request */
	Data (display, (char *)dstStart, size);
	break;
      }

    case MPEXPixmapLUT:
	size = count * sizeof(mpxlPixmapEntry);

	/* update the request length */
	req->length += (size + 3) >> 2;

	/* add the table entry data to the end of the X request */
	Data (display, (char *)entries, size);
	break;
#endif
    }

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 *	PEXDeleteTableEntries
 *
 * FORMAT:
 *	`PEXDeleteTableEntries`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure returned
 * 			by a successful `XOpenDisplay" call.
 *
 *	lut		The resource identifier of the lookup table from which
 *			entries are to be deleted.
 *
 *	start		The initial lookup table entry to be deleted.
 *
 *	count		The number of entries to be deleted from the
 *			lookup table `lut'.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine deletes lookup table entries from lookup table
 *	`lut', starting at the entry indicated by `start'.
 *	Entries with index values between `start' and
 *	`start'+`count'-1 are deleted.  Attempts to delete
 *	undefined entries are ignored.
 *
 * ERRORS:
 *	LookupTable	Lookup table resouce identifier is invalid.
 *
 *	Value		The sum of start and count is greater than 2**16.
 *
 * SEE ALSO:
 *	`PEXCreateLookupTable", `PEXFreeLookupTable", `PEXCopyLookupTable",
 *	`PEXGetTableInfo", `PEXGetPredefinedEntries", `PEXGetDefinedIndices",
 *	`PEXGetTableEntry", `PEXGetTableEntries", `PEXSetTableEntries"
 *
 */

void
PEXDeleteTableEntries (display, lut, start, count)

INPUT Display		*display;
INPUT pxlLookupTable	lut;
INPUT int		start;
INPUT int		count;

{
/* AUTHOR: */
    pexDeleteTableEntriesReq	*req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (DeleteTableEntries, req);
    req->lut = lut;
    req->start = start;
    req->count = count;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}
