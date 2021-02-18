/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/*
 * ppcFont.c - ppc font related routines
 */

#include "X.h"
#include "Xmd.h"
#include "Xproto.h"
#include "misc.h"

#include "fontstruct.h"
#include "dixfontstr.h"
#include "scrnintstr.h"

#include "ppcCache.h"
#include "bitmap.h"

Bool
ppcRealizeFont( pScreen, pFont )
	ScreenPtr pScreen;
	FontPtr pFont;
	{
	int		index = pScreen->myNum;
	FontInfoPtr	pfi = &(pFont->info);
	xCharInfo	* minb = & pfi->minbounds;
	ppcBMInfoPtr	*bmtable;
	int		numGlyphs;
    BitmapFontPtr  bitmapFont = (BitmapFontPtr) pFont->fontPrivate;
	ppcBMInfoPtr   * ptemp;

	ppcCacheInit(pScreen);
#ifdef PPC_NO_CACHE
	return(mfbRealizeFont( pScreen, pFont ));
#else

	if ( ( minb->descent + minb->ascent ) > 32 )
		{	/* this font has no cacheable characters */
		return(mfbRealizeFont( pScreen, pFont ));
		}

	numGlyphs = ( pfi->lastCol - pfi->firstCol + 1 ) *
			( pfi->lastRow - pfi->firstRow + 1 );

	
	if ( ( bmtable = (ppcBMInfoPtr *)Xalloc(sizeof(ppcBMInfoPtr)*numGlyphs ) ) == NULL )
		{
		return(FALSE);
		}

	/* initialize to null */
	bzero(bmtable, sizeof(ppcBMInfoPtr)*numGlyphs);

/*
	pFont->devPriv[index] = (pointer)bmtable;
 */
	if ( (pointer) ( bitmapFont->bitmapExtra) == (pointer) NULL)
	{
		/* Allocate memory. */
		if ( ( bitmapFont->bitmapExtra = 
		   (BitmapExtraPtr) Xalloc(sizeof(ppcBMInfoPtr) * MAXSCREENS )) 
		        == (BitmapExtraPtr ) NULL)
		{
			
			return ( FALSE) ;
		}
	}
	ptemp = (ppcBMInfoPtr * ) (bitmapFont->bitmapExtra) + index;
	  
   	*ptemp =(ppcBMInfoPtr ) bmtable;
	return(TRUE);
#endif /* PPC_NO_CACHE */
	}

Bool
ppcUnrealizeFont( pScreen, pFont )
	ScreenPtr pScreen;
	FontPtr pFont;
	{
	int		index = pScreen->myNum;
	ppcBMInfoPtr * ptemp;
   	BitmapFontPtr  bitmapFont = (BitmapFontPtr) pFont->fontPrivate;

/*
	if ( (int)pFont->devPriv[index] > 20 )
		Xfree(pFont->devPriv[index]);
*/
#ifndef PPC_NO_CACHE
	ptemp = (ppcBMInfoPtr * ) (bitmapFont->bitmapExtra) + index;
	if ( (int) *(ptemp) > 20)
	{
		Xfree(*ptemp);
	}
#endif /* PPC_NO_CACHE */
	return(TRUE);
	}

