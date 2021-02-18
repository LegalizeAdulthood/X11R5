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
**	pl_font.c
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
**	This file contains the routines associated with PEX fonts.
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
**  Modification history:
**
**	19-Aug-88  
**--
*/

/*
 *   Table of Contents (routine names in order of appearance)
 */

/*
 * E   PEXOpenFont
 * E   PEXCloseFont
 * E   PEXQueryFont
 * E   PEXListFonts
 * E   PEXListFontsWithInfo
 * E   PEXQueryTextExtents
 * E   PEXMonoEncodedQueryTextExtents
 */

/*
 *   Include Files
 */

/*
 * XXX A pxlFontInfo should have the list of pexFontProp defined explicitly
 * for ease of use. ie:
 *      typedef struct pxlFontInfo
 *	{
 *	    CARD32      firstGlyph;
 *	    CARD32      lastGlyph;
 *	    CARD32      defaultGlyph;
 *	    pexSwitch   allExist;
 *	    pexSwitch   strokeFont;
 *	    CARD16      unused;
 *	    CARD32      numProps;
 *	    pxlFontProp *fontProps;
 *	}
 * This affects PEXQueryFont and PEXListFontsWithInfo.  Also _PEXFreeFontInfo.
 */

#include "pex_features.h"
#include "pex_macros.h"

#include "PEXlib.h"
#include "PEXlibint.h"

#include "pl_ref_data.h"

extern	char	*_XAllocScratch();


/*+
 * NAME:
 * 	PEXOpenFont
 *
 * FORMAT:
 * 	`PEXOpenFont`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	fontname	A string containing the name of the font to be opened.
 *
 * RETURNS:
 *	The resource ID of the loaded PEX font.
 *
 * DESCRIPTION:
 *	This routine loads the specified PEX font, if necessary.  The font 
 *	name should use the ISO Latin-1 encoding and upper/lower case does 
 *	not matter.  PEX fonts are not associated with a particular screen, 
 *    	and can be used with any renderer or PHIGS workstation resource.  
 *	An error will be generated if the specified font is not "PEX usable", 
 *   	that is, it is not capable of supporting the full range of PEX text 
 *	attributes.	
 *
 * ERRORS:
 *	IDChoice	ID already in use or not in range assigned to client.
 *
 *	PEXFont		String does not name a useable PEX font.
 *
 *	Alloc		Server failed to allocate the resource.
 *
 * SEE ALSO:
 *
 */

pxlFont
PEXOpenFont (display, fontname)

INPUT Display	*display;
INPUT char	*fontname;

{
/* AUTHOR:  Sanjiv Maewall */
    pexOpenFontReq	*req;
    pexFont		id;

    /* lock around critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (OpenFont, req);
    req->numBytes = strlen(fontname);
    req->font = id = XAllocID (display);
    req->length += (req->numBytes + 3) >> 2;

    /* put the data in the X buffer */
    Data (display, (char *)fontname, req->numBytes);

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (id);
}

/*+
 * NAME:
 * 	PEXCloseFont
 *
 * FORMAT:
 * 	`PEXCloseFont`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *  	font		The resource ID of the PEX font to be closed.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine deletes the association between the resource ID and the 
 *	PEX font.  The PEX font itself will be freed when no other resource 
 *	references it.
 *
 * ERRORS:
 *	PEXFont		Font resource ID does not name a useable PEX font.
 *
 * SEE ALSO:
 *
 */

void
PEXCloseFont (display, font)

INPUT Display	*display;
INPUT pxlFont	font;

{
/* AUTHOR:  Sanjiv Maewall */
    pexResourceReq *req;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReq (CloseFont, req);
    req->id = font;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 * 	PEXQueryFont
 *
 * FORMAT:
 * 	`PEXQueryFont`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	font		The resource ID of the font to be queried.
 *
 *	fontInfoReturn	Returns a pointer to a structure containing
 *			information about the font.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine returns information about the specified PEX font. 
 *
 *	PEXlib allocates the storage for `fontInfoReturn',
 *	and the application should free this data when
 *	it is no longer needed by calling `PEXFree"(`*fontInfoReturn').
 *
 * ERRORS:
 *	PEXFont			Font resource ID does not name a useable PEX
 *				font.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 * SEE ALSO:
 *
 */

Status
PEXQueryFont (display, font, fontInfoReturn)

INPUT Display		*display;
INPUT pxlFont		font;
OUTPUT pxlFontInfo	**fontInfoReturn;

{
/* AUTHOR:  Sanjiv Maewall */
    pexQueryFontReply 	rep;
    pexQueryFontReq	*req;
    char		*buf;
    int			i;
    int 		font_info_size;

    /* Lock around critical section, for multithreading */
    LockDisplay (display);

    /* put request in the X request buffer */
    PEXGetReq (QueryFont, req);
    req->font = font;

    if (_XReply (display, &rep, 0, xFalse) == 0)
    { /* there was an error */
	UnlockDisplay (display);
	PEXSyncHandle ();
	return (1);            /* return an error */
    }

    /* Read the reply and check that the length is correct */
    buf = (char *)_XAllocScratch (display, (unsigned long)(rep.length << 2));
    _XRead (display, (char *)buf, (long)(rep.length << 2));
    font_info_size = sizeof(pxlFontInfo) + 
		((pexFontInfo *)(buf))->numProps * sizeof(pxlFontProp);
    if ( rep.length<<2 < font_info_size )
    {
	UnlockDisplay (display);
	PEXSyncHandle ();
	return (1);            /* return an error */
    }

    /* allocate space for the font info */
    *fontInfoReturn = (pxlFontInfo *)
	PEXAllocBuf ( (unsigned)(sizeof(PEXHeader) + font_info_size) );
    ((PEXHeader *)(*fontInfoReturn))->size = 1;
    ((PEXHeader *)(*fontInfoReturn))->free = _PEXFreeFontInfo;
    *fontInfoReturn = (pxlFontInfo *)&((PEXHeader *)(*fontInfoReturn))[1];

    COPY_AREA ((char *)buf, (char *)*fontInfoReturn, font_info_size);

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	PEXListFonts
 *
 * FORMAT:
 * 	`PEXListFonts`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	pattern		Specifies the string (null terminated) associated
 *			with the font names to be returned.
 *			The `?' character (octal value 77) matches any single 
 *			character, and the character `*' (octal value 52) 
 *			matches any number of characters.
 *
 *	maxNames	Maximum number of names that are in the
 *			returned list.
 *
 *	countReturn	Returns the actual number of font names.
 *
 *	namesReturn	A list of font names that match the string
 *			passed in the `pattern' argument.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	Like X11 `ListFonts' except that this routine only returns information 
 *	about fonts that can support the full range of PEX text attributes (ie: 
 *	those fonts that are "PEX usable").  This list may or may not contain 
 *	some of the same fonts returned by the X11 `ListFonts' routine.  This 
 *	routine returns a list of at most `maxNames' entries, each of which 
 *      contains the name of the font matching the `pattern'.  `pattern' is a 
 *	string that uses the ISO Latin-1 encoding and upper/lower case does 
 *	not matter. The returned names are in lower case and are also ISO 
 *	Latin-1 encoded strings.
 *
 *	The number of names actually returned is stored in the `countReturn'.  
 *	Pointers to the font name strings are stored in the array `namesReturn'.
 *	PEXlib allocates the storage for `namesReturn' and the application 
 *	should free this data when
 *	it is no longer needed by calling `PEXFree"(`*namesReturn').
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *
 */

Status
PEXListFonts (display, pattern, maxNames, countReturn, namesReturn)

INPUT Display		*display;
INPUT char		*pattern;
INPUT int		maxNames;
OUTPUT int		*countReturn;
OUTPUT pxlStringData	**namesReturn;

{
/* AUTHOR:  Sanjiv Maewall */
    pexListFontsReply 	rep;
    pexListFontsReq   	*req;
    long		numChars;
    pexString 		*repStrings;
    int			i;
    pxlStringData	*nextName;

    /* Lock around critical section, for multithreading */
    LockDisplay (display);

    /* put request in the X request buffer */
    PEXGetReq (ListFonts, req);
    req->maxNames = maxNames;
    numChars = req->numChars = strlen(pattern);
    req->length += (numChars + 3) >> 2;

    /* put the data in the X buffer */
    Data (display, (char *)pattern, numChars);

    if (_XReply (display, &rep, 0, xFalse) == 0)
    { /* there was an error */
	UnlockDisplay (display);
	PEXSyncHandle ();
	return (1);            /* return an error */
    }

    *countReturn = rep.numStrings;

    /* Allocate space for repStrings.  After the XRead 'repStrings' points
     * at the list of font names that matched the pattern string
     * passed in from the user */
    repStrings = (pexString *)_XAllocScratch (display, 
		(unsigned long)(rep.length << 2));
    _XRead (display, (char *)repStrings, (long)(rep.length << 2));

    /* Allocate space for a list of font names */
    *namesReturn = (pxlStringData *)PEXAllocBuf( sizeof(PEXHeader) + 
		(rep.numStrings * sizeof(pxlStringData)));
    ((PEXHeader *)(*namesReturn))->size = rep.numStrings;
    ((PEXHeader *)(*namesReturn))->free = _PEXFreeFontName;
    *namesReturn = (pxlStringData *)&((PEXHeader *)(*namesReturn))[1];

    /* Now stuff strings from reply into *namesReturn */
    for(i=0, nextName = *namesReturn; i<rep.numStrings; i++, nextName++)
    {
	nextName->numChars = (int) repStrings->length;

	nextName->chars = PEXAllocBuf ((unsigned) nextName->numChars);
	COPY_AREA ((char *)&(repStrings[1]), (char *)nextName->chars,
		(unsigned)nextName->numChars);

	repStrings = (pexString *) ((char *)repStrings + sizeof(pexString) + 
	         nextName->numChars + Pad(nextName->numChars));
    }

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	PEXListFontsWithInfo
 *
 * FORMAT:
 * 	`PEXListFontsWithInfo`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	pattern		Specifies the string (null terminated) associated
 *			with the font names to be returned.
 *			The `?' character (octal value 77) matches any single 
 *			character, and the character `*' (octal value 52) 
 *			matches any number of characters.
 *
 *	maxNames	Maximum number of names that are in the
 *			returned list.
 *
 *	numStringsReturn	
 *			Returns the number of strings in namesReturn
 *
 *	numFontInfoReturn	
 *			Returns the number of pxlFontInfo structure in 
 *		        fontInfoReturn
 *
 *	namesReturn	A list of font names that match the string
 *			passed in the `pattern' argument.
 *
 *	fontInfoReturn	Returns a pointer to the array of pxlFontInfo
 *			structures.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	Like X11 `ListFontsWithInfo' except that this routine only returns 
 *	about fonts that can support the full range of PEX text attributes (ie: 
 *	those fonts that are "PEX usable").  This list may or may not contain 
 *	some of the same fonts returned the X11 `ListFontsWithInfo' routine.  
 *	This routine returns a list of at most `maxNames' entries, each of 
 *	which contains information about a font matching the `pattern'.  
 *	`pattern is a string that uses the ISO Latin-1 encoding and upper/lower 
 *	case does not matter. The returned names are in lower case and are 
 *	also ISO Latin-1 encoded strings.
 *
 *	The information returned for each font is identical to what 
 *	`pexQueryFont' would return.  The number of names actually returned 
 *	is stored in the `numStringsReturn'.  Pointers to the font name
 *	strings are stored in the array `namesReturn'.  The number of
 *	pxlFontInfo structures actually returned is stored in 
 *	`numFontInfoReturn'.  A pointer to the array of pxlFontInfo structures 
 *	is returned in `fontInfoReturn'.
 *
 *	The application should allocate the storage for `numSringsReturn' and
 *	`numFontInfoReturn'.
 *	PEXlib allocates the storage for the array of font names pointed
 *	to by `namesReturn' and for the array of pxlFontInfo records
 *	pointed to by `fontInfoReturn'. The application should free
 *	this data when it is no longer needed by calling
 *	`PEXFree"(`*fontInfoReturn') and PEXFree(`*namesReturn').  
 *
 * ERRORS:
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 * SEE ALSO:
 *
 */

Status
PEXListFontsWithInfo (display, pattern, maxNames, numStringsReturn, 
	numFontInfoReturn, namesReturn, fontInfoReturn)

INPUT Display		*display;
INPUT char		*pattern;
INPUT int		maxNames;
OUTPUT int		*numStringsReturn;
OUTPUT int		*numFontInfoReturn;
OUTPUT pxlStringData	**namesReturn;
OUTPUT pxlFontInfo	**fontInfoReturn;

{
/* AUTHOR:  Sanjiv Maewall */
    pexListFontsWithInfoReq	*req;
    pexListFontsWithInfoReply	rep;
    long			numChars;
    int				i;
    int				font_info_size;
    pxlStringData		*nextName;
    char			*buf, *startRepData;
    pexString			*repStrings;

    /* Lock around critical section, for multithreading */
    LockDisplay (display);

    /* put request in the X request buffer */
    PEXGetReq (ListFontsWithInfo, req);
    req->maxNames = maxNames;
    numChars = req->numChars = strlen(pattern);
    req->length += (numChars + 3) >> 2;

    /* put the data in the X buffer */
    Data (display, (char *)pattern, numChars);

    if (_XReply (display, &rep, 0, xFalse) == 0)
    { /* there was an error */
	UnlockDisplay (display);
        PEXSyncHandle ();
        return (1);                /* return an error */
    }

    *numStringsReturn = rep.numStrings;

    /* Allocate space for 'buf'.  After the XRead 'buf' points at the
     * LISTofSTRINGS, followed by number of PEX_FONTINFO, followed
     * by LISTofPEX_FONTINFO.
     */
    startRepData = buf = (char *)
	_XAllocScratch (display, (unsigned long)(rep.length << 2));
    _XRead (display, (char *)buf, (long)(rep.length << 2));

    /* Allocate space for a list of font names */
    *namesReturn = (pxlStringData *)PEXAllocBuf( sizeof(PEXHeader) + 
		(rep.numStrings * sizeof(pxlStringData)));
    ((PEXHeader *)(*namesReturn))->size = rep.numStrings;
    ((PEXHeader *)(*namesReturn))->free = _PEXFreeFontName;
    *namesReturn = (pxlStringData *)&((PEXHeader *)(*namesReturn))[1];

    /* Now stuff font names from reply into *namesReturn */
    repStrings = (pexString *)buf;
    for (i=0, nextName = *namesReturn; i<rep.numStrings; i++, nextName++)
    {
        nextName->numChars = (int) repStrings->length;

        nextName->chars = PEXAllocBuf ((unsigned) nextName->numChars);
        COPY_AREA ((char *)&(repStrings[1]), (char *)nextName->chars,
                (unsigned)nextName->numChars);

        repStrings = (pexString *) ((char *)repStrings + sizeof(pexString) +
                 nextName->numChars + Pad(nextName->numChars));
    }

    /* repString now points at the number of PEX_FONTINFO.  This is
     * followed by a LISTofPEX_FONTINFO.
     */
    buf = (char *)repStrings;
    *numFontInfoReturn = (int) *((CARD32 *)buf); 
    buf += sizeof(CARD32);

    /* Allocate space for a list of font info */
    font_info_size = (rep.length << 2) - (buf - startRepData);
    *fontInfoReturn = (pxlFontInfo *) PEXAllocBuf ( sizeof(PEXHeader) + 
		font_info_size );
    ((PEXHeader *)(*fontInfoReturn))->size = *numFontInfoReturn;
    ((PEXHeader *)(*fontInfoReturn))->free = _PEXFreeFontInfo;
    *fontInfoReturn = (pxlFontInfo *)&((PEXHeader *)(*fontInfoReturn))[1];

    COPY_AREA ((char *)buf, (char *)*fontInfoReturn, font_info_size);

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	PEXQueryTextExtents
 *
 * FORMAT:
 * 	`PEXQueryTextExtents`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	id	 	Resource id to use.  It is either a renderer,
 *			a workstation or a text font table (if it is a 
 *			renderer or workstation then the textfont table 
 *			associated with the renderer or workstation is used)
 *
 *	fontGroup	index of text font to use
 *
 *      path            The text path to use 
 *
 *      expansion       The character expansion factor to use 
 *
 *      spacing         A floating point value specifying the
 *                      character spacing factor to use 
 *
 *      height          The character height value to use 
 *	
 *      halignment      An integer specifying the value of the
 *                      horizontal text alignment attribute.
 *
 *      valignment      An integer specifying the value of the
 *                      vertical text alignment attribute.
 *
 *	string		Specifies the string to be queried.
 *
 *	count		The number of bytes in `string'.
 *
 *	lowerLeftReturn The lower left corner of the text extents
 *
 *	upperRightReturn The upper right corner of the text extents
 *
 *	concatPtReturn	Returns the point where next glyph should go if
 *			the string is to be extended.  This is a modeling
 *			coordinate.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine returns the text extents of the specified text string 
 *	in the local 2D text coordinate system.   If  `id' is a renderer
 *	or PHIGS workstation resource, the text font table used to perform 
 *	the extents computation will be the text font table associated with 
 *	the renderer or PHIGS workstation.  If `id' is a lookup table 
 *	resource of type `PEXTextFontLUT' it is used directly.  `fontGroup' 
 *	provides the index of the entry that is to be used to obtain the 
 *	font group.
 *
 *	Stroke precision is assumed.  The text position is (0,0) in the local 
 *	2D text coordinate system.  `concatPtReturn' returns the point where 
 *	the next glyph should go if the string is to be extended.  This 
 *	position is given in the 2D text coordinate system  (A suitable 
 *	modelling transformation to account for the character up vector will 
 *	still need to be applied by the client.)
 *
 *	If a specified font has no defined default glyph then undefined glyphs 
 *	in `string' are taken to have all zero metrics. 
 *
 * ERRORS:
 *	PEXFont			Font resource ID does not name a useable PEX
 *				font.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	PipelineContext		Pipeline context ID is invalid.
 *
 * SEE ALSO:
 *
 */

Status
PEXQueryTextExtents (display, id, fontGroup, path, expansion, spacing, height, 
 	halign, valign, string, count, lowerLeftReturn, upperRightReturn,
	concatPtReturn)

INPUT Display			*display;
INPUT XID			id;
INPUT int			fontGroup;
INPUT int			path;
INPUT float			expansion;
INPUT float			spacing;
INPUT float			height;
INPUT int			halign;
INPUT int			valign;
INPUT char			*string;
INPUT int			count;
OUTPUT pxlCoord2D		*lowerLeftReturn;
OUTPUT pxlCoord2D		*upperRightReturn;
OUTPUT pxlCoord2D		*concatPtReturn;

{
/* AUTHOR:  Sanjiv Maewall */
    pexQueryTextExtentsReq	*req;
    pexQueryTextExtentsReply 	rep;
    char			*ch;
    pexMonoEncoding 		pMonoEncoding;
    int				encode_count;


    /* Lock around critical section, for multithreading */
    LockDisplay (display);

    /* put request in the X request buffer */
    PEXGetReq (QueryTextExtents, req);
    req->fpFormat = pexFpFormat;
    req->textPath = path;
    req->fontGroupIndex = fontGroup;
    req->id = id;
    req->fontGroupIndex = (pexTableIndex) fontGroup;
    req->charExpansion = expansion;
    req->charSpacing = spacing;
    req->charHeight = height;
    req->textAlignment.vertical = valign;
    req->textAlignment.horizontal = halign;
    req->numStrings = 1;

    pMonoEncoding.characterSet = (INT16) 1;
    pMonoEncoding.characterSetWidth = (CARD8) PEXCSByte;
    pMonoEncoding.encodingState = 0;  
    pMonoEncoding.numChars = (CARD16) (count);

    req->length += sizeof(CARD32) >> 2;
    req->length += sizeof(pexMonoEncoding) >> 2;
    req->length += (count + 3) >> 2;

    encode_count = 1;
    Data (display, (char *)&encode_count, sizeof(CARD32));
    Data (display, (char *)&pMonoEncoding, sizeof(pexMonoEncoding));
    Data (display, (char *)string, count);

    if (_XReply (display, &rep, 0, xFalse) == 0)
    { /* there was an error */
        UnlockDisplay(display);
        PEXSyncHandle();
        return (1);            /* return an error */
    }

    ch = (char *)_XAllocScratch (display, (unsigned long)(rep.length << 2));
    _XRead (display, (char *)ch, (long)(rep.length << 2));

    *lowerLeftReturn = *(pxlCoord2D *)ch;
    ch += sizeof(pxlCoord2D);
    *upperRightReturn = *(pxlCoord2D *)ch;
    ch += sizeof(pxlCoord2D);
    *concatPtReturn = *(pxlCoord2D *)ch;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}


/*+
 * NAME:
 * 	PEXMonoEncodedQueryTextExtents
 *
 * FORMAT:
 * 	`PEXMonoEncodedQueryTextExtents`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	id	 	Resource id to use.  It is either a renderer,
 *			a workstation or a text font table (if it is a 
 *			renderer or workstation then the textfont table 
 *			associated with the renderer or workstation is used)
 *
 *	fontGroup	index of text font to use
 *
 *      path            The text path to use 
 *
 *      expansion       The character expansion factor to use 
 *
 *      spacing         A floating point value specifying the
 *                      character spacing factor to use 
 *
 *      height          The character height value to use 
 *	
 *      halignment      An integer specifying the value of the
 *                      horizontal text alignment attribute.
 *
 *      valignment      An integer specifying the value of the
 *                      vertical text alignment attribute.
 *
 *      encodedTextList List of mono encoded text strings
 *
 *      numEncodings    The number of mono encoded text strings in `meTextList'
 *
 *	lowerLeftReturn The lower left corner of the text extents
 *
 *	upperRightReturn The upper right corner of the text extents
 *
 *	concatPtReturn	Returns the point where next glyph should go if
 *			the string is to be extended.  This is a modeling
 *			coordinate.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *      This routine is the same as `PEXQueryTextExtents", except
 *      that multiple encoded text strings can be specified.  Each text
 *      string in `encodedTextList' has a character set, a character
 *      set width, an encoding state, and a list of characters. (see
 *      `pxlMonoEncodedTextData' defined in `PEXlib.h').  The character
 *      set is an index into the current font group.  Font groups have
 *      individual fonts which are numbered starting at one; a value of
 *      three would select the third font in the font group currently being
 *      used. 
 *
 * ERRORS:
 *	PEXFont			Font resource ID does not name a useable PEX
 *				font.
 *
 *	FloatingPointFormat	Device does not support the specified floating point format.
 *
 *	PipelineContext		Pipeline context ID is invalid.
 *
 * SEE ALSO:
 *
 */

Status
PEXMonoEncodedQueryTextExtents (display, id, fontGroup, path, expansion, spacing, height, 
 	halign, valign, strings, count, lowerLeftReturn, upperRightReturn,
	concatPtReturn)

INPUT Display			*display;
INPUT XID			id;
INPUT int			fontGroup;
INPUT int			path;
INPUT float			expansion;
INPUT float			spacing;
INPUT float			height;
INPUT int			halign;
INPUT int			valign;
INPUT pxlMonoEncodedTextData    *strings;
INPUT int			count;
OUTPUT pxlCoord2D		*lowerLeftReturn;
OUTPUT pxlCoord2D		*upperRightReturn;
OUTPUT pxlCoord2D		*concatPtReturn;

{
/* AUTHOR:  Sanjiv Maewall */
    pexQueryTextExtentsReq	*req;
    pexQueryTextExtentsReply 	rep;
    pxlMonoEncodedTextData      *nextString;
    char			*ch;
    pexMonoEncoding 		pMonoEncoding;
    int				encode_count, i;


    /* Lock around critical section, for multithreading */
    LockDisplay (display);

    /* put request in the X request buffer */
    PEXGetReq (QueryTextExtents, req);
    req->fpFormat = pexFpFormat;
    req->textPath = path;
    req->fontGroupIndex = fontGroup;
    req->id = id;
    req->fontGroupIndex = (pexTableIndex) fontGroup;
    req->charExpansion = expansion;
    req->charSpacing = spacing;
    req->charHeight = height;
    req->textAlignment.vertical = valign;
    req->textAlignment.horizontal = halign;
    req->numStrings = 1;

    req->length += sizeof(CARD32) >> 2;

    for (i=0, nextString=strings; i < count; i++, nextString++) 
    {
	req->length += sizeof(pexMonoEncoding) >> 2; 

        if ( nextString->characterSetWidth == PEXCSLong) 
          req->length += nextString->numChars;
        else if ( nextString->characterSetWidth == PEXCSShort) 
          req->length += (nextString->numChars+1) >> 1;
        else /* nextString->characterSetWidth == PEXCSByte) */ 
          req->length += (nextString->numChars+3) >> 2;
    } 

    encode_count = count;
    Data (display, (char *)&encode_count, sizeof(CARD32));


    for (i=0, nextString=strings; i < count; i++, nextString++) 
    { 
        Data (display, nextString, sizeof(pexMonoEncoding)); 
 
        if ( nextString->characterSetWidth == PEXCSLong) 
	{
            Data (display, nextString->chars,
                  nextString->numChars * sizeof(long));
	}
        else if ( nextString->characterSetWidth == PEXCSShort) 
	{
            Data (display, nextString->chars,
		  nextString->numChars * sizeof(short));
	}
        else /* nextString->characterSetWidth == PEXCSByte) */ 
	{
            Data (display, nextString->chars,
		  nextString->numChars);
	}
    } 

    if (_XReply (display, &rep, 0, xFalse) == 0)
    { /* there was an error */
        UnlockDisplay(display);
        PEXSyncHandle();
        return (1);            /* return an error */
    }

    ch = (char *)_XAllocScratch (display, (unsigned long)(rep.length << 2));
    _XRead (display, (char *)ch, (long)(rep.length << 2));

    *lowerLeftReturn = *(pxlCoord2D *)ch;
    ch += sizeof(pxlCoord2D);
    *upperRightReturn = *(pxlCoord2D *)ch;
    ch += sizeof(pxlCoord2D);
    *concatPtReturn = *(pxlCoord2D *)ch;

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}
