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

/* $Header: /andrew/X11/R3src/tape/server/ddx/ibm/ppc/RCS/ppcRslvC.c,v 9.1 88/10/24 04:01:18 paul Exp $ */
/* $Source: /andrew/X11/R3src/tape/server/ddx/ibm/ppc/RCS/ppcRslvC.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/R3src/tape/server/ddx/ibm/ppc/RCS/ppcRslvC.c,v 9.1 88/10/24 04:01:18 paul Exp $";
#endif

/* Generic Color Resolution Scheme
 * P. Shupak 12/31/87
 */

#include "X.h"
#include "screenint.h"
#include "scrnintstr.h"
#include "OScompiler.h"

void
ppcResolveColor( pRed, pGreen, pBlue, pVisual )
register unsigned short * const pRed ;
register unsigned short * const pGreen ;
register unsigned short * const pBlue ;
register VisualPtr const pVisual ;
{ 
register unsigned long int tmp ;
register unsigned long int round ;
register unsigned short int mask ;

tmp = pVisual->bitsPerRGBValue ;
round = 0x8000 >> tmp ;
mask = ( 0xFFFF << ( 16 - tmp ) ) & 0xFFFF ;

switch ( pVisual->class ) {
	case PseudoColor:
	*pRed   = ( ( tmp =   *pRed + round ) > mask ) ? mask : ( tmp & mask ) ;
	*pGreen = ( ( tmp = *pGreen + round ) > mask ) ? mask : ( tmp & mask ) ;
	*pBlue  = ( ( tmp =  *pBlue + round ) > mask ) ? mask : ( tmp & mask ) ;
	break ;

	case GrayScale:
	/* Convert To Resonable Approx. Of Gray */
	tmp = ( ( 30 * *pRed ) + ( 59 * *pGreen ) + ( 11 * *pBlue ) ) / 100 ;
	if ( ( tmp += round ) > mask )
		tmp = mask ;
	else
		tmp &= mask ;
	*pRed   =
	*pGreen =
	*pBlue = tmp ;
	break ;

	default:
	Error( "can't resolve color for this visual" ) ;
	break ;
}

return ;
}
