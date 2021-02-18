/* $Header: XpexText.c,v 2.5 91/09/11 16:06:57 sinyaw Exp $ */
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

#include "Xpexlibint.h"
#include "Xpexlib.h"
#include <X11/extensions/Xext.h>
#include "extutil.h"
#include "Xpextutil.h"

static int encode_text();

void
XpexSetTextFontIndex(dpy, text_font_index)
	Display *dpy;
	XpexTableIndex text_font_index;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexTextFontIndex *oc;

		XpexGetOutputCmd(TextFontIndex,dpy,info,oc);

		oc->index = text_font_index;
	}
	UnlockDisplay(dpy);
}


void
XpexSetTextPrecision(dpy, precision)
	Display *dpy;
	XpexTextPrecision precision;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexTextPrecision *oc;

		XpexGetOutputCmd(TextPrecision,dpy,info,oc);

		oc->precision = (CARD16) precision;
	}
	UnlockDisplay(dpy);
}

void
XpexSetCharExpansion(dpy, expansion)
	Display *dpy;
	XpexFloat expansion;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexCharExpansion *oc;

		XpexGetOutputCmd(CharExpansion,dpy,info,oc);

		oc->expansion = expansion;
	}
	UnlockDisplay(dpy);
}

void
XpexSetCharSpacing(dpy, spacing)
	Display *dpy;
	XpexFloat spacing;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexCharSpacing *oc;

		XpexGetOutputCmd(CharSpacing,dpy,info,oc);

		oc->spacing = spacing;
	}
	UnlockDisplay(dpy);
}

void
XpexSetTextColorIndex(dpy, text_color_index)
	Display *dpy;
	XpexTableIndex text_color_index;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexTextColourIndex *oc;

		XpexGetOutputCmd(TextColourIndex,dpy,info,oc);

		oc->index = (pexTableIndex) text_color_index;
	}
	UnlockDisplay(dpy);
}

void 
XpexSetTextColor(dpy, text_color)
	Display *dpy;
	XpexColor *text_color;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexTextColour *oc;

		XpexColorType colorType = XpexGetColorType(dpy);

		int length;

		int extra = Xpex_sizeof_color(colorType);

		XpexGetOutputCmdExtra(TextColour,dpy,info,extra,oc);

		oc->colourSpec.colourType = (pexColourType) colorType;

		Xpex_pack_color(colorType, text_color, 
		(char *)(oc + 1), &length);
	}
	UnlockDisplay(dpy);
}

void
XpexSetCharHeight(dpy, height)
	Display *dpy;
	XpexFloat height;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexCharHeight *oc;

		XpexGetOutputCmd(CharHeight,dpy,info,oc);

		oc->height = height;
	}
	UnlockDisplay(dpy);
}

void
XpexSetCharUpVector(dpy, up)
	Display *dpy;
	XpexVector3D *up;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexCharUpVector *oc;

		XpexGetOutputCmd(CharUpVector,dpy,info,oc);

		oc->up.x = up->x;
		oc->up.y = up->y;
	}
	UnlockDisplay(dpy);
}

void
XpexSetTextPath(dpy, path)
	Display *dpy;
	XpexTextPath path;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexTextPath *oc;

		XpexGetOutputCmd(TextPath,dpy,info,oc);

		oc->path = path;
	}
	UnlockDisplay(dpy);
}

void
XpexSetTextAlignment(dpy, vertical, horizontal)
	Display *dpy;
	XpexTextValignment vertical;
	XpexTextHalignment horizontal;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexTextAlignment *oc;

		XpexGetOutputCmd(TextAlignment,dpy,info,oc);

		oc->alignment.vertical = vertical;
		oc->alignment.horizontal = horizontal;
	}
	UnlockDisplay(dpy);
}

void
XpexSetAtextHeight(dpy, height)
	Display *dpy;
	XpexFloat height;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexAtextHeight *oc;

		XpexGetOutputCmd(AtextHeight,dpy,info,oc);

		oc->height = height;
	}
	UnlockDisplay(dpy);
}

void
XpexSetAtextUpVector(dpy, up)
	Display *dpy;
	XpexVector3D *up;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexAtextUpVector *oc;

		XpexGetOutputCmd(AtextUpVector,dpy,info,oc);

		oc->up.x = up->x;
		oc->up.x = up->y;
	}
	UnlockDisplay(dpy);
}

void
XpexSetAtextPath(dpy, path)
	Display *dpy;
	XpexTextPath path;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexAtextPath *oc;

		XpexGetOutputCmd(AtextPath,dpy,info,oc);

		oc->path = path;
	}
	UnlockDisplay(dpy);
}

void
XpexSetAtextAlignment(dpy, vertical, horizontal)
	Display *dpy;
	XpexTextValignment vertical;
	XpexTextHalignment horizontal;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexAtextAlignment *oc;

		XpexGetOutputCmd(AtextAlignment,dpy,info,oc);

		oc->alignment.vertical = vertical;
		oc->alignment.horizontal = horizontal;
	}
	UnlockDisplay(dpy);
}

void
XpexSetAtextStyle(dpy, style)
	Display *dpy;
	XpexEnumTypeIndex style;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexAtextStyle *oc;

		XpexGetOutputCmd(AtextStyle,dpy,info,oc);

		oc->style = (pexEnumTypeIndex) style;
	}
	UnlockDisplay(dpy);
}

void
XpexSetTextBundleIndex(dpy, index)
	Display *dpy;
	XpexTableIndex index;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexTextBundleIndex *oc;

		XpexGetOutputCmd(TextBundleIndex,dpy,info,oc);

		oc->index = (pexTableIndex) index;
	}
	UnlockDisplay(dpy);
}

void
XpexText(dpy, origin, vector1, vector2, string, length)
	Display *dpy;
	XpexCoord3D *origin;
	XpexVector3D *vector1;
	XpexVector3D *vector2;
	XpexString string;
	int length;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexText *oc;
		pexMonoEncoding *mono;
		int extra = sizeof(pexMonoEncoding) + length + PADDING(length);

		XpexGetOutputCmdExtra(Text,dpy,info,extra,oc);

		oc->origin.x = origin->x;
		oc->origin.y = origin->y;
		oc->origin.z = origin->z;
		oc->vector1.x = vector1->x;
		oc->vector1.y = vector1->y;
		oc->vector1.z = vector1->z;
		oc->vector2.x = vector2->x;
		oc->vector2.y = vector2->y;
		oc->vector2.z = vector2->z;
		oc->numEncodings = (CARD16) encode_text(length, string,
		(pexMonoEncoding *)(oc + 1)); 
	}
	UnlockDisplay(dpy);
}

void
XpexText2D(dpy, origin, string, length)
	Display *dpy;
	XpexCoord2D *origin;
	XpexString string;
	int length;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexText2D *oc;
		pexMonoEncoding *mono;
		int extra = sizeof(pexMonoEncoding) + length + PADDING(length);

		XpexGetOutputCmdExtra(Text2D,dpy,info,extra,oc);

		oc->origin.x = origin->x;
		oc->origin.y = origin->y;
		oc->numEncodings = (CARD16) encode_text(length, string,
		(pexMonoEncoding *)(oc + 1)); 
	}
	UnlockDisplay(dpy);
}

void
XpexAtext(dpy, origin, offset, string, length)
	Display *dpy;
	XpexCoord3D *origin;
	XpexCoord3D *offset;
	XpexString string;
	int length;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexAnnotationText *oc;
		pexMonoEncoding *mono;
		int extra = sizeof(pexMonoEncoding) + length + PADDING(length);

		XpexGetOutputCmdExtra(AnnotationText,dpy,info,extra,oc);

		oc->origin.x = origin->x;
		oc->origin.y = origin->y;
		oc->origin.z = origin->z;
		oc->offset.x = offset->x;
		oc->offset.y = offset->y;
		oc->offset.z = offset->z;
		oc->numEncodings = (CARD16) encode_text(length, string,
		(pexMonoEncoding *)(oc + 1)); 
	}
	UnlockDisplay(dpy);
}

void
XpexAtext2D(dpy, origin, offset, string, length)
	Display *dpy;
	XpexCoord2D *origin;
	XpexCoord2D *offset;
	XpexString string;
	int length;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexAnnotationText2D *oc;
		pexMonoEncoding *mono;
		int extra = sizeof(pexMonoEncoding) + length + PADDING(length);

		XpexGetOutputCmdExtra(AnnotationText2D,dpy,info,extra,oc);

		oc->origin.x = origin->x;
		oc->origin.y = origin->y;
		oc->offset.x = offset->x;
		oc->offset.y = offset->y;
		oc->numEncodings = (CARD16) encode_text(length, string,
		(pexMonoEncoding *)(oc + 1)); 
	}
	UnlockDisplay(dpy);
}

static int
encode_text(length, string, encoding)
	int  length;
	XpexString string;
	pexMonoEncoding *encoding;
{
	int num_encodings;
	/* TODO: More than one encoding. */
	num_encodings = 1;

	encoding->characterSet = 0;
	encoding->characterSetWidth = PEXCSByte;
	encoding->encodingState = 0;
	encoding->numChars = length;
	if ((int)(encoding->numChars) > 0) {
		bcopy((char *) string, (char *)(encoding + num_encodings), 
		(int)(encoding->numChars));
	}

	return num_encodings;
}
