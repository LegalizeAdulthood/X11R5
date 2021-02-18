/* $Header: XpexColorSpec.c,v 2.2 91/09/11 16:06:35 sinyaw Exp $ */

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
_XpexSizeOfColorSpec(color_type)
	int color_type;
{
	extern int _XpexSizeOfColorValue();

	int size = sizeof(pexColourSpecifier) + 
		_XpexSizeOfColorValue(color_type);

	return size;
}

void
_XpexWriteColorSpec(pSrc, pStream, pLength)
	XpexColorSpecifier *pSrc;
	char  *pStream; /* RETURN */
	int  *pLength; /* RETURN */
{
	pexColourSpecifier *pDest = (pexColourSpecifier *) pStream;

	pDest->colourType = (pexEnumTypeIndex) pSrc->color_type;

	pStream += sizeof(pexColourSpecifier);
	{
		extern void Xpex_pack_color();

		int bytes_written;

		Xpex_pack_color(pSrc->color_type, 
		&(pSrc->value), pStream, &bytes_written);

		*pLength = sizeof(pexColourSpecifier) + bytes_written;
	}
}

void
_XpexReadColorSpec(pStream, pColourSpec, pLength)
	char  *pStream;
	XpexColorSpecifier *pColourSpec;	/* RETURN */
	int  *pLength; 		/* RETURN */
{
	extern void Xpex_unpack_color();

	pexColourSpecifier *cs = (pexColourSpecifier *) pStream;

	pColourSpec->color_type = (int) cs->colourType;

	pStream += sizeof(pexColourSpecifier);

	Xpex_unpack_color(cs->colourType, (CARD32 *)pStream, 
	&(pColourSpec->value), pLength);

	*pLength += sizeof(pexColourSpecifier);
}
