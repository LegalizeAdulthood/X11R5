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

/* $Header: /andrew/X11/r3src/release/server/ddx/ibm/ppc/RCS/emulTile.c,v 30.0 89/01/23 19:28:31 paul Exp $ */
/* $Source: /andrew/X11/r3src/release/server/ddx/ibm/ppc/RCS/emulTile.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/release/server/ddx/ibm/ppc/RCS/emulTile.c,v 30.0 89/01/23 19:28:31 paul Exp $" ;
#endif

/* ppc Tile
 * P. Shupak 11/87
 * Modified From original ppc Tile
 * T. Paquin 9/87
 * Uses private imageFill a bunch of times
 */

#include "X.h"
#include "pixmapstr.h"
#include "pixmap.h"
#include "scrnintstr.h"
#include "gcstruct.h"
#include "gc.h"

#include "OScompiler.h"

#include "ppcGCstr.h"
#include "ppcScrn.h"

#include "ibmTrace.h"

extern void ErrorF() ;

static void
DrawFirstTile( pTile, x, y, fnp, w, h, alu, planes, xOffset, yOffset )
register PixmapPtr pTile ;
register int x ;
register int y ;
register void (* fnp)() ;
int w ;
int h ;
int alu ;
unsigned long int planes ;
int xOffset ;
int yOffset ;
{
register int htarget ;
register int vtarget ;

	if ( xOffset ) { /* Not X-Aligned */
		if ( yOffset ) { /* Nor Y-Aligned */
			htarget = MIN( pTile->drawable.width - xOffset, w ),
			vtarget = MIN( pTile->drawable.height - yOffset, h ),
			yOffset *= pTile->devKind ;
			(* fnp)( x, y,
				 htarget,
				 vtarget,
				 pTile->devPrivate.ptr + yOffset + xOffset,
				 pTile->devKind,
				 alu, planes ) ;
			if ( w > htarget ) {
				w = MIN( w, pTile->drawable.width ) ;
				if ( h > vtarget ) {
					h = MIN( h, pTile->drawable.height ) ;
					(* fnp)( x, y + vtarget,
						 htarget,
						 h - vtarget,
						 pTile->devPrivate.ptr + xOffset,
						 pTile->devKind,
						 alu, planes ) ;
					(* fnp)( x + htarget, y,
						 w - htarget,
						 vtarget,
						 pTile->devPrivate.ptr + yOffset,
						 pTile->devKind,
						 alu, planes ) ;
					(* fnp)( x + htarget,
						 y + vtarget,
						 w - htarget,
						 h - vtarget,
						 pTile->devPrivate.ptr,
						 pTile->devKind,
						 alu, planes ) ;
				}
				else { /* h <= vtarget */
					(* fnp)( x + htarget, y,
						 w - htarget,
						 vtarget,
						 pTile->devPrivate.ptr + yOffset,
						 pTile->devKind,
						 alu, planes ) ;
				}
			}
			else if ( h > vtarget ) {
				(* fnp)( x, y + vtarget,
					 htarget,
					 MIN( h, pTile->drawable.height ) - vtarget,
					 pTile->devPrivate.ptr + xOffset,
					 pTile->devKind,
					 alu, planes ) ;
				vtarget = pTile->drawable.height ;
			}
		}
		else { /* No Y Offset */
			(* fnp)( x, y,
				 htarget = MIN( pTile->drawable.width - xOffset, w ),
				 vtarget = MIN( pTile->drawable.height, h ),
				 pTile->devPrivate.ptr + xOffset,
				 pTile->devKind,
				 alu, planes ) ;
			if ( w > htarget ) {
				(* fnp)( x + htarget, y,
					 MIN( pTile->drawable.width, w ) - htarget,
					 vtarget,
					 pTile->devPrivate.ptr,
					 pTile->devKind,
					 alu, planes ) ;
			}
		}
	}
	else if ( yOffset ) {
		(* fnp)( x, y,
			 htarget = MIN( pTile->drawable.width, w ),
			 vtarget = MIN( pTile->drawable.height - yOffset, h ),
			 pTile->devPrivate.ptr + ( yOffset * pTile->devKind ),
			 pTile->devKind,
			 alu, planes ) ;
		if ( h > vtarget ) {
			(* fnp)( x, y + vtarget,
				 htarget,
				 MIN( pTile->drawable.height, h ) - vtarget,
				 pTile->devPrivate.ptr,
				 pTile->devKind,
				 alu, planes ) ;
		}
	}
	else { /* NO Offset */
		(* fnp)( x, y,
			 htarget = MIN( pTile->drawable.width, w ),
			 vtarget = MIN( pTile->drawable.height, h ),
			 pTile->devPrivate.ptr,
			 pTile->devKind,
			 alu, planes ) ;
	}

	return ;
}

void
ppcTileRect( pTile, alu, planes, x, y, w, h, xSrc, ySrc )
register PixmapPtr pTile ;
const int alu ;
const unsigned long int planes ;
register int x, y, w, h ;
int xSrc ;
int ySrc ;
{
register int htarget ;
register int vtarget ;
register void (*fnp)() ;
ScreenPtr pScreen ;
int xOffset ;
int yOffset ;

TRACE( ( "ppcTileRect(pTile=x%x,alu=x%x,planes=x%02x,x=%d,y=%d,w=%d,h=%d,xSrc=%d,ySrc=%d\n",
		pTile, alu, planes, x, y, w, h, xSrc, ySrc ) ) ;

     pScreen = pTile->drawable.pScreen ;
     switch ( alu ) {
	case GXclear:		/* 0x0 Zero 0 */
	case GXinvert:		/* 0xa NOT dst */
	case GXset:		/* 0xf 1 */
		(* ( ( (ppcScrnPriv *) (pScreen->devPrivate) )->solidFill ) )
			( 0xFF, alu, planes, x, y, w, h ) ;
	case GXnoop:		/* 0x5 dst */
		return ;
	default:
		break ;
}

    if ( ( xOffset = ( x - xSrc ) ) > 0 )
	xOffset %= pTile->drawable.width ;
    else
	xOffset = pTile->drawable.width - (( - xOffset ) % pTile->drawable.width ) ;

    if ( ( yOffset = ( y - ySrc ) ) > 0 )
	yOffset %= pTile->drawable.height ;
else
	yOffset = pTile->drawable.height - (( - yOffset ) % pTile->drawable.height ) ;

     fnp = ( (ppcScrnPriv *) (pScreen->devPrivate) )->imageFill ;

     switch ( alu ) {
	case GXcopyInverted:	/* 0xc NOT src */
	case GXcopy:		/* 0x3 src */
		/* Special Case Code */
		DrawFirstTile( pTile, x, y, fnp, w, h,
			       alu, planes, xOffset, yOffset ) ;
		/* Here We Double The Size Of The BLIT Each Iteration */
		(* ( (ppcScrnPriv *) ( pScreen->devPrivate ) )->replicateArea)(
			    x, y, planes, w, h,
			    MIN( w, pTile->drawable.width ), MIN( h, pTile->drawable.height ), pScreen ) ;
		break ;
	case GXnor:		/* 0x8 NOT src AND NOT dst */
	case GXandReverse:	/* 0x2 src AND NOT dst */
	case GXorReverse:	/* 0xb src OR NOT dst */
	case GXnand:		/* 0xe NOT src OR NOT dst */
	case GXandInverted:	/* 0x4 NOT src AND dst */
	case GXand:		/* 0x1 src AND dst */
	case GXequiv:		/* 0x9 NOT src XOR dst */
	case GXxor:		/* 0x6 src XOR dst */
	case GXorInverted:	/* 0xd NOT src OR dst */
	case GXor:		/* 0x7 src OR dst */
	default:
		{
		register unsigned char *data ;
		register int savex ;
		register int savey ;
		register int hcount, vcount ;
		int tmp ;
		int savehcount ;
		int savevcount ;
	
		htarget = x ;
		vtarget = y ;
		savex = MIN( w, pTile->drawable.width - xOffset ) ;
		savey = MIN( h, pTile->drawable.height - yOffset ) ;
		data = ( (unsigned char *) pTile->devPrivate.ptr )
		     + ( yOffset *= pTile->devKind )
		     + xOffset ;
		/* FIRST DO TOP EDGE PIECE */
		if ( yOffset ) {
			/* DO TOP LEFT CORNER PIECE */
			(* fnp)( htarget, vtarget,
				 savex,
				 savey,
				 data,
				 pTile->devKind,
				 alu, planes ) ;
			savevcount = ( h - savey ) / pTile->drawable.height ;
			if ( xOffset ) {
				/* DO REST OF LEFT EDGE */
				vtarget += savey ; /* Next Line */
				for ( vcount = savevcount ;
				      vcount-- ;
				      vtarget += pTile->drawable.height ) {
					(* fnp)( htarget, vtarget,
						 savex,
						 pTile->drawable.height,
						 data - yOffset,
						 pTile->devKind,
						 alu, planes ) ;
				}
				if ( tmp = y + h - vtarget )
					(* fnp)( htarget, vtarget,
						 savex,
						 tmp,
						 data - yOffset,
						 pTile->devKind,
						 alu, planes ) ;
				/* The ENTIRE left edge is done ! */
				x += savex ;
				w -= savex ;
				data -= xOffset ;
				htarget += savex ;
			}

			vtarget = y ;
			/* DO REST OF TOP EDGE */
			savehcount = ( x + w - htarget ) / pTile->drawable.height ;
			for ( hcount = savehcount ;
			      hcount-- ;
			      htarget += pTile->drawable.width ) {
				(* fnp)( htarget, vtarget,
					 pTile->drawable.width,
					 savey,
					 data,
					 pTile->devKind,
					 alu, planes ) ;
			}
			/* DO TOP RIGHT CORNER PIECE */
			if ( savex = x + w - htarget ) {
				(* fnp)( htarget, vtarget,
					 savex,
					 savey,
					 data,
					 pTile->devKind,
					 alu, planes ) ;
				/* DO REST OF RIGHT EDGE */
				data -= yOffset ;
				vtarget += savey ;
				for ( vcount = savevcount ;
				      vcount-- ;
				      vtarget += pTile->drawable.height ) {
					(* fnp)( htarget, vtarget,
						 savex,
						 pTile->drawable.height,
						 data,
						 pTile->devKind,
						 alu, planes ) ;
				}
				if ( tmp = y + h - vtarget )
					(* fnp)( htarget, vtarget,
						 savex,
						 tmp,
						 data,
						 pTile->devKind,
						 alu, planes ) ;
				/* The ENTIRE right edge is done ! */
				w -= savex ;
			}
			else
				data -= yOffset ;
			y += savey ;
			h -= savey ;
		}
		else if ( xOffset ) {
			/* DO TOP LEFT CORNER PIECE */
			(* fnp)( htarget, vtarget,
				 savex,
				 pTile->drawable.height,
				 data,
				 pTile->devKind,
				 alu, planes ) ;
			/* DO REST OF LEFT EDGE */
			vtarget += pTile->drawable.height ; /* Next Line */
			savevcount = ( h - pTile->drawable.height ) / pTile->drawable.height ;
			for ( vcount = savevcount ;
			      vcount-- ;
			      vtarget += pTile->drawable.height ) {
				(* fnp)( htarget, vtarget,
					 savex,
					 pTile->drawable.height,
					 data,
					 pTile->devKind,
					 alu, planes ) ;
			}
			if ( tmp = y + h - vtarget )
				(* fnp)( htarget, vtarget,
					 savex,
					 tmp,
					 data,
					 pTile->devKind,
					 alu, planes ) ;
			x += savex ;
			w -= savex ;
			data -= xOffset ;

			/* DO TOP RIGHT CORNER PIECE */
			if ( savex = w % pTile->drawable.width ) {
				(* fnp)( htarget = x + w - savex,
					 vtarget = y,
					 savex,
					 pTile->drawable.height,
					 data,
					 pTile->devKind,
					 alu, planes ) ;
				/* DO REST OF RIGHT EDGE */
				vtarget += pTile->drawable.height ;
				for ( vcount = savevcount ;
				      vcount-- ;
				      vtarget += pTile->drawable.height ) {
					(* fnp)( htarget, vtarget,
						 savex,
						 pTile->drawable.height,
						 data,
						 pTile->devKind,
						 alu, planes ) ;
				}
				if ( tmp )
					(* fnp)( htarget, vtarget,
						 savex,
						 tmp,
						 data,
						 pTile->devKind,
						 alu, planes ) ;
				/* The ENTIRE right edge is done ! */
				w -= savex ;
			}
			savehcount = w / pTile->drawable.height ;
		}
		else {
			savevcount = h / pTile->drawable.height ;
			savehcount = w / pTile->drawable.height ;
		}

		/* Fill The Center */
		for ( vtarget = y, vcount = savevcount ;
		      vcount-- ;
		      vtarget += pTile->drawable.height ) {
			for ( hcount = savehcount, htarget = x ;
			      hcount-- ;
			      htarget += pTile->drawable.width ) {
				(* fnp)( htarget, vtarget,
					 pTile->drawable.width,
					 pTile->drawable.height,
					 data,
					 pTile->devKind,
					 alu, planes ) ;
			}
		}
		/* Finally Fill The Bottom Row Center */
		if ( savey = y + h - vtarget  )
			for ( hcount = savehcount, htarget = x ;
			      hcount-- ;
			      htarget += pTile->drawable.width ) {
				(* fnp)( htarget, vtarget,
					 pTile->drawable.width,
					 savey,
					 data,
					 pTile->devKind,
					 alu, planes ) ;
			}
		}
	}
	

return ;
}
