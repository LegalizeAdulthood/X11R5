/* $Header: XpexColor.c,v 2.3 91/09/11 16:07:07 sinyaw Exp $ */

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

int 
_XpexSizeOfColorValue( type)
	int type;
{
	int fp = sizeof(XpexFloat);
	int i = ((type == Xpex_IndexedColor) ? 1 : 0);
	int f = ((type == Xpex_Rgb8Color) ? 1 :
		((type == Xpex_Rgb16Color) ? 2 : 3));

	int size = ((i || f == 1) ? 4 : ((f == 2) ? 8 : 3 * fp));

	return size;
}

void
Xpex_pack_color(color_type, color, pStream, pLength)
	XpexColorType  color_type;
	XpexColor *color;
	char  *pStream; /* RETURN */
	int  *pLength; /* RETURN */
{
	switch (color_type) {
	case PEXIndexedColour:
	{
		pexIndexedColour *d = (pexIndexedColour *) pStream;
		d->index = color->index;
		*pLength = sizeof(pexIndexedColour);
		break;
	}
	case PEXRgb8Colour:
	{
		pexRgb8Colour *d = (pexRgb8Colour *) pStream;
		d->red   = color->rgb8.red;
		d->green = color->rgb8.green;
		d->blue  = color->rgb8.blue;
		*pLength = sizeof(pexRgb8Colour);
		break;
	}
	case PEXRgb16Colour:
	{
		pexRgb16Colour *d = (pexRgb16Colour *) pStream;
		d->red   = color->rgb16.red;
		d->green = color->rgb16.green;
		d->blue  = color->rgb16.blue;
		*pLength = sizeof(pexRgb16Colour);
		break;
	}
	case PEXRgbFloatColour:
	case PEXCieFloatColour:
	case PEXHsvFloatColour:
	case PEXHlsFloatColour:
	default:
	{
		pexFloatColour *d = (pexFloatColour *) pStream;
		d->first  = color->color_float.first;
		d->second = color->color_float.second;
		d->third  = color->color_float.third;
		*pLength = sizeof(pexFloatColour);
		break;
	}

	} /* end-switch */

} /* end-func */

void
Xpex_unpack_color(color_type, list, color, bytes_unpacked)
	XpexColorType  color_type;
	CARD32  *list; 
	XpexColor *color; /* RETURN */
	int  *bytes_unpacked; /* RETURN */
{
	switch (color_type) {
	case PEXIndexedColour:
	{
		pexIndexedColour *s = (pexIndexedColour *) list;
		color->index = (XpexTableIndex) s->index;
		*bytes_unpacked = sizeof(pexIndexedColour);
		break;
	}
	case PEXRgb8Colour:
	{
		pexRgb8Colour *s = (pexRgb8Colour *) list;
		color->rgb8.red = s->red;
		color->rgb8.green = s->green;
		color->rgb8.blue = s->blue;
		*bytes_unpacked = sizeof(pexRgb8Colour);
		break;
	}
	case PEXRgb16Colour:
	{
		pexRgb16Colour *s = (pexRgb16Colour *) list;
		color->rgb16.red = s->red;
		color->rgb16.green = s->green;
		color->rgb16.blue = s->blue;
		*bytes_unpacked = sizeof(pexRgb16Colour);
		break;
	}
	case PEXRgbFloatColour:
	case PEXCieFloatColour:
	case PEXHsvFloatColour:
	case PEXHlsFloatColour:
	default:
	{
		pexFloatColour *s = (pexFloatColour *) list;
		color->color_float.first = s->first;
		color->color_float.second = s->second;
		color->color_float.third = s->third;
		*bytes_unpacked = sizeof(pexFloatColour);
		break;
	}
	} /* end-switch */

} /* end-func */
