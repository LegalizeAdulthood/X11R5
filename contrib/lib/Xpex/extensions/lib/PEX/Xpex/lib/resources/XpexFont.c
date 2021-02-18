/* $Header: XpexFont.c,v 2.3 91/09/11 16:07:30 sinyaw Exp $ */
/*****************************************************************
Copyright 1991 by Sony Microsystems Company, San Jose, California

					All Rights Reserved

Permission to use, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sony not be used
in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

SONY DISCLAIMS ANY AND ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL EXPRESS WARRANTIES AND ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, FOR A PARTICULAR PURPOSE. IN NO EVENT
SHALL SONY BE LIABLE FOR ANY DAMAGES OF ANY KIND, INCLUDING BUT NOT
LIMITED TO SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING FROM
LOSS OF USE, DATA OR LOSS OF ANY PAST, PRESENT, OR PROSPECTIVE PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.
*****************************************************************/

#include <stdio.h>
#include <X11/extensions/Xext.h>
#include "Xpexlibint.h"
#include "Xpexlib.h"
#include "extutil.h"
#include "Xpextutil.h"

static int _XpexQueryFont();
static int _XpexListFonts();
static int _XpexListFontsWithInfo();
static int _XpexQueryTextExtents();

XpexFont 
XpexOpenFont(dpy, name)
	register Display *dpy;
	char *name;
{
	XExtDisplayInfo *i;
	pexFont		fid;
	register	pexOpenFontReq *req;

	int			opcode;
	CARD32		nameLength = (CARD32) strlen(name);
	int			extra = nameLength + PADDING(nameLength);

	i = XpexFindDisplay(dpy);

	XpexCheckExtension(dpy,i,0);

	LockDisplay(dpy);
	XpexGetReqExtra(OpenFont, dpy, i, extra, req);
	req->font = fid = XAllocID(dpy);
	req->numBytes = nameLength;
	bcopy((char *)name, (char *)(req + 1), nameLength);
	UnlockDisplay(dpy);
	SyncHandle();
	return (fid);
}

void
XpexCloseFont(dpy, font_id)
	register Display *dpy;
	XpexFont font_id;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);

	LockDisplay(dpy);
	{
		register pexCloseFontReq *req;
		XpexGetResReq(CloseFont, dpy, i, font_id, req);
		UnlockDisplay(dpy);
	}
	SyncHandle();
}

XpexFontInfo *
XpexQueryFont(dpy, fid)
	register Display *dpy;
	XpexFont fid;
{
	XpexFontInfo	*dest;
	pexFontInfo		*src;
	pexFontProp		*pFontProp;

	char	*pStream;
	int		numProps;
	int		i;

	if (!_XpexQueryFont(dpy, fid, &pStream) > 0) {
		return (XpexFontInfo *) NULL;
	}

	if (!(dest = (XpexFontInfo *)Xmalloc(sizeof(XpexFontInfo)))) {
		return (XpexFontInfo *) NULL;
	}

	src = (pexFontInfo *) pStream;
	dest->first_glyph = src->firstGlyph;
	dest->last_glyph = src->lastGlyph;
	dest->default_glyph = src->defaultGlyph;
	dest->all_exist = src->allExist;
	dest->stroke_font = src->strokeFont;
	dest->n_properties = numProps = src->numProps;

	if (!(dest->properties = (XpexFontProp *) 
	Xmalloc(numProps * sizeof(pexFontProp))))  {
		return (XpexFontInfo *) NULL;
	}

	pStream += sizeof(pexFontInfo);
	pFontProp = (pexFontProp *) pStream;

	for (i = 0; i < numProps; i++) {
		dest->properties[i].name = pFontProp[i].name;
		dest->properties[i].value = pFontProp[i].value;
	}
	return dest;
}

char **
XpexListFonts(dpy, pattern, maxnames, actual_count)
    register Display  *dpy;
    char  *pattern;
    int  maxnames;
    int  *actual_count; /* RETURN */
{
    CARD32  num_strings;
    char *pStream;
	char **flist;
	char *ch;
	CARD16 length;
	int i;

    if(_XpexListFonts(dpy, (CARD16) maxnames, pattern, 
	&num_strings, &pStream)) {
		return (char **) NULL;
	}

	if (num_strings) {
		flist = (char **)
		malloc((unsigned) num_strings * sizeof(char *));

		if (!flist) {
			return (char **) NULL;
		}
		for (i = 0; i < num_strings; i++) {
			length = (int) *((CARD16 *) pStream);
			pStream += sizeof(CARD16);
			flist[i] = (char *) malloc((unsigned)(length+1));
			ch = flist[i];
			bcopy( pStream, flist[i], length + 1);
			/* the assumption here is that the string
			being read from the string is NULL terminated */

			/* ch[length] = '\0'; */
			pStream += (length + 1);
		}
	} else
		flist = (char **) NULL;

	*actual_count = (int) num_strings;

	return flist;
}


char **
XpexListFontsWithInfo(dpy, pattern, maxnames, count, info)
	register Display *dpy;
	char *pattern;
	int maxnames;
	int *count;	/* RETURN */
	XpexFontInfo	**info;	/* RETURN */
{
    CARD32  num_strings;
    char *pStream;
	char **flist;

    if (!_XpexListFontsWithInfo(dpy, (CARD16) maxnames, pattern,
	&num_strings, &pStream)) {
		return (char **) NULL;
	}
	*count = (int) num_strings;

	if (num_strings) {
	}

} 


XpexExtentInfo *
XpexQueryTextExtents(dpy, resource_id, font_group,
	text_path, char_expansion, char_spacing, char_height, 
	h_alignment, v_alignment, strings, num_strings, num_info)
	register Display *dpy;
	XID resource_id;
	XpexTableIndex	font_group;
	short		text_path;
	XpexFloat	char_expansion;
	XpexFloat	char_spacing;
	XpexFloat	char_height;
	short 		h_alignment;
	short 		v_alignment;
	char		*strings[];
	int			num_strings;
{
	register int	i;
	CARD32			length_of_istring_list;
	pexExtentInfo	*extent_info;

	if (!_XpexQueryTextExtents(dpy, (XID) resource_id,
	(pexTableIndex) font_group, (CARD16) text_path, 
	(XpexFloat) char_expansion, (XpexFloat) char_spacing, (XpexFloat) char_height, 
	(CARD16) h_alignment, (CARD16) v_alignment, 
	(CARD32) num_strings, strings, length_of_istring_list,
	&extent_info)) {
		return (XpexExtentInfo *) NULL;
	}
	return (XpexExtentInfo *) extent_info;
}

static Status 
_XpexQueryFont(dpy, fid, lengthFontInfo, fontInfo)
	register Display *dpy;
	XpexFont  	 fid;
	CARD32		*lengthFontInfo; /* RETURN */
	char		**fontInfo; /* RETURN */
{
	register	XExtDisplayInfo *i;
	register	pexQueryFontReq *req;
				pexQueryFontReply reply;

	Status		status = 0;
	unsigned	size;

	i = XpexFindDisplay(dpy);
	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReq(QueryFont, dpy, i, req);
	req->font = fid;

	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*lengthFontInfo = reply.lengthFontInfo;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*fontInfo = Xmalloc(size)) {
			_XRead(dpy, (char *)*fontInfo, (long)size);
		} else {
			status = 0;
			_XpexClearReply(dpy, reply.length);
		}
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return status;
}

static int
_XpexListFonts(dpy, maxNames, patternString, numStrings, strings)
	register Display *dpy;
	CARD16 maxNames;
	char *patternString;
	CARD32 *numStrings;
	char **strings;
{
	register	XExtDisplayInfo *i;
	register 	pexListFontsReq *req;
				pexListFontsReply reply;

	int			status = 0;
	int			numChars = strlen(patternString);
	int 		opcode;
	int			extra = numChars + PADDING(numChars);
	unsigned	size;

	i = XpexFindDisplay(dpy);
	XpexCheckExtension(dpy,i,0);

	LockDisplay(dpy);
	XpexGetReqExtra(ListFonts, dpy, i, extra, req);
	req->maxNames = maxNames;
	req->numChars = numChars;
	bcopy((char *)patternString, (char *)(req + 1), numChars);
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*numStrings = reply.numStrings;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*strings = Xmalloc(size)) {
			_XRead(dpy, (char *)*strings, (long)size);
		} else {
			status = 0;
			_XpexClearReply(dpy, reply.length);
		}
	}
	UnlockDisplay(dpy);
	SyncHandle();
	return status;
}

static int
_XpexListFontsWithInfo(dpy, maxNames, patternString,
	numStrings, stringsAndInfo)
	register Display	*dpy;
	CARD16	maxNames;
	char	*patternString;
	CARD32	*numStrings; /* RETURN */
	char	**stringsAndInfo; /* RETURN */
{
	register	XExtDisplayInfo *i;
	register	pexListFontsWithInfoReq		*req;
				pexListFontsWithInfoReply	reply;

	int			status = 0;
	int			numChars = strlen(patternString);
	unsigned	size;
	int 		extra = numChars + PADDING(numChars);

	i = XpexFindDisplay(dpy);
	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReqExtra(ListFontsWithInfo, dpy, i, extra, req);
	req->maxNames = (CARD16) maxNames;
	req->numChars = (CARD16) numChars;
	bcopy((char *)patternString, (char *)(req + 1), numChars);
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*numStrings = (int) reply.numStrings;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*stringsAndInfo = (char *)Xmalloc(size)) {
			_XRead(dpy, (char *)*stringsAndInfo, (long)size);
		} else {
			status = 0;
			_XpexClearReply(dpy, reply.length);
		}
	}
	UnlockDisplay(dpy);
	SyncHandle();
	return status;
}

static Status
_XpexQueryTextExtents(dpy, rid, font_group, text_path, expansion, 
	spacing, height, horizontal, vertical, strings, num_istrings,
	length_of_istring_list, istrings, extent_info)
	register Display *dpy;
	XID	 rid;
	pexTableIndex font_group;
	CARD16	text_path;
	XpexFloat	expansion;
	XpexFloat	spacing;
	XpexFloat	height;
	pexTextHAlignment	horizontal; 
	pexTextVAlignment	vertical; 
	CARD32		num_istrings;
	unsigned	length_of_istring_list; /* in bytes */
	pexExtentInfo *extent_info[]; /* RETURN */
{
	register	XExtDisplayInfo *i;
	register	pexQueryTextExtentsReq *req;
				pexQueryTextExtentsReply reply;

	int			status = 0;
	unsigned	size;
	int			opcode;
	int 		extra = length_of_istring_list;

	i = XpexFindDisplay(dpy);
	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReqExtra(QueryTextExtents, dpy, i, extra, req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->textPath = text_path;
	req->id = rid;
	req->fontGroupIndex = font_group;
	req->charExpansion = expansion;
	req->charSpacing = spacing;
	req->textAlignment.horizontal = horizontal;
	req->textAlignment.vertical = vertical;
	req->numStrings = num_istrings;

	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*extent_info = (pexExtentInfo *)Xmalloc(size)) {
			_XRead(dpy, (char *)*extent_info, (long)size);
		} else {
			status = 0;
			_XpexClearReply(dpy, reply.length);
		}
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return status;
}
