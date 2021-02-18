/* $Header: XpexReflAttr.c,v 2.3 91/09/11 16:06:46 sinyaw Exp $ */

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
_XpexSizeOfReflAttr(refl_attr)
	XpexReflectionAttr *refl_attr;
{
	extern int _XpexSizeOfColorValue();

	int color_type = refl_attr->specular_color.color_type;

	int size = sizeof(pexReflectionAttr) + 
		_XpexSizeOfColorValue(color_type);

	return size;
}

void
_XpexWriteReflAttr(pSrc, pStream, pLength)
	XpexReflectionAttr  *pSrc;
	char  *pStream;
	int  *pLength; /* RETURN */
{
	pexReflectionAttr *pDest = (pexReflectionAttr *) pStream;
	int color_type = pSrc->specular_color.color_type;

	pDest->ambient      = pSrc->ambient;
	pDest->diffuse      = pSrc->diffuse;
	pDest->specular     = pSrc->specular;
	pDest->specularConc = pSrc->specular_conc;
	pDest->transmission = pSrc->transmission;

	pDest->specularColour.colourType = (pexEnumTypeIndex) color_type;

	pStream += sizeof(pexReflectionAttr);
	{
		extern void Xpex_pack_color();

		int bytes_written;

		Xpex_pack_color(color_type, &(pSrc->specular_color.value), 
		pStream, &bytes_written);

		*pLength = sizeof(pexReflectionAttr) + bytes_written;
	}
}

void
_XpexReadReflAttr(pStream, refl_attr, pLength)
	char  *pStream;
	XpexReflectionAttr  *refl_attr; /* RETURN */
	int  *pLength; /* RETURN */
{
	pexReflectionAttr *pSrc = (pexReflectionAttr *) pStream;
	int color_type = (int) pSrc->specularColour.colourType;

	refl_attr->ambient = (XpexFloat) pSrc->ambient;
	refl_attr->diffuse = (XpexFloat) pSrc->diffuse;
	refl_attr->specular = (XpexFloat) pSrc->specular;
	refl_attr->specular_conc = (XpexFloat) pSrc->specularConc;
	refl_attr->transmission = (XpexFloat) pSrc->transmission;

	refl_attr->specular_color.color_type = color_type;

	pStream += sizeof(pexReflectionAttr);
	{
		extern void Xpex_unpack_color();

		int bytes_read;

		Xpex_unpack_color(color_type, (CARD32 *) pStream,
		&(refl_attr->specular_color.value), &bytes_read);

		*pLength = sizeof(pexReflectionAttr) + bytes_read;
	}
}
