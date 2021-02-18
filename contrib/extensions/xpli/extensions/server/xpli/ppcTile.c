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

#include "X.h"
#include "pixmapstr.h"
#include "pixmap.h"
#include "scrnintstr.h"
#include "colormap.h"

#include "ppc.h"
#include "ppcProcs.h"
#include "ibmTrace.h"
#include "OScompiler.h"

extern void ErrorF( ) ;
extern void mfbDestroyPixmap( ) ;

void ppcTileRect( pTile, alu, planes, x, y, w, h, xSrc, ySrc )
register PixmapPtr pTile ;
const int alu ;
const unsigned long int planes ;
register int x, y, w, h ;
{
register int		htarget, vtarget ;
register int 		tlx, tly ;
register void		(*fnp)() ;
register PixmapPtr 	pTmpTile ;
int			depth ;
ScreenPtr		pScreen ;
int			TileCopied ;

TRACE( ( "ppcTileRect(pTile = x%x, alu= x%x, planes=x%02x, x=%d, y=%d, w=%d, h=%d, xSrc=%d, ySrc=%d\n",
		pTile, alu, planes, x, y, w, h, xSrc, ySrc ) ) ;

tlx = pTile->drawable.width ;
tly = pTile->drawable.height ;
pScreen = pTile->drawable.pScreen ;
depth   = pTile->drawable.depth ;

switch ( alu ) {
	case GXclear:		/* 0x0 Zero 0 */
	case GXinvert:		/* 0xa NOT dst */
	case GXset:		/* 0xf 1 */
	     (* ( ( (ppcScrnPriv *) (pScreen->devPrivate) )->solidFill) )
		     (0xFF, alu, planes, x, y, w, h ) ;
	case GXnoop:		/* 0x5 dst */
		return ;
	default:
		break ;
}


if ( ( x - xSrc ) % tlx ) {
	if ( !( pTmpTile = ppcCreatePixmap( pScreen, tlx, tly, depth ) ) ) {
		ErrorF( "Failed To Create Temporary Pixmap in ppcTileRect\n" ) ;
		return ;
	}
	if (depth==1)
	   	ppcRotBitmapLeft( pTile, pTmpTile, ( ( x - xSrc ) % tlx ) ) ;
	   else ppcRotZ8mapLeft( pTile, pTmpTile, ( ( x - xSrc ) % tlx ) ) ;

	if ( ( y - ySrc ) % tly ) {
		if ( !( pTile = ppcCreatePixmap( pScreen, tlx, tly, depth ) ) ) {
			ErrorF( "Failed To Create Pixmap \"pTile\" in ppcTileRect\n" ) ;
			return ;
		}
		if (depth==1)
			ppcRotBitmapUp( pTmpTile, pTile, ( (y-ySrc) % tly ) );
		   else ppcRotZ8mapUp( pTmpTile, pTile, ( (y-ySrc) % tly ) );
		mfbDestroyPixmap( pTmpTile ) ;
	}
	else
		pTile = pTmpTile ;
	TileCopied = 1 ;
}
else if ( ( y - ySrc ) % tly ) {
	if ( !( pTmpTile = ppcCreatePixmap( pScreen, tlx, tly, depth ) ) ) {
		ErrorF( "Failed To Create Pixmap \"pTmpTile\" in ppcTileRect\n" ) ;
		return ;
	}
	if (depth==1)
		ppcRotBitmapUp( pTile, pTmpTile, ( ( y - ySrc ) % tly ) ) ;
	   else ppcRotZ8mapUp( pTile, pTmpTile, ( ( y - ySrc ) % tly ) ) ;
	pTile = pTmpTile ;
	TileCopied = 1 ;
}
else {
	TileCopied = 0 ;
}

fnp = ( (ppcScrnPriv *) (pScreen->devPrivate) )->imageFill ;

switch ( alu ) {
	case GXcopyInverted:	/* 0xc NOT src */
	case GXcopy:		/* 0x3 src */
		/* Special Case Code */
		vtarget = ( h > tly ) ? tly : h ;
		if ( w >= tlx ) {
			/* Note: Drawing with Height Less Than
			   The Actual Height Of The Image Will
			   Work Correctly */
			(*fnp)( x, y,
				htarget = tlx,
				vtarget,
				pTile->devPrivate.ptr,pTile->devKind,
				alu, planes ) ;
		}
		else {
			if ( !( pTmpTile = ppcCreatePixmap( pScreen,
							    htarget = w,
					   		    vtarget,
							    depth ) ) ) {
				ErrorF( "Failed To Create Short Pixmap in ppcTileRect\n" ) ;
				return ;
			}
			if (depth==1)
				ppcClipZ1Pixmap( pTile, pTmpTile ) ;
			   else ppcClipZ8Pixmap( pTile, pTmpTile ) ;

			(*fnp)( x, y,
				htarget,
				vtarget,
				pTmpTile->devPrivate.ptr,pTmpTile->devKind,
				alu, planes ) ;
			mfbDestroyPixmap( pTmpTile ) ;
		}
		/* Here We Double The Size Of The BLIT Each Iteration */
		ppcReplicateArea( x, y, planes, w, h,
				  htarget, vtarget, pScreen ) ;
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
		register int	hcount, vcount ;
		register int	savevcount ;
		register int	savehcount ;
		char 		*data ;
		int		Rtlx, Btly ;
	
		savevcount = h / tly ;
		savehcount = w / tlx ;
	
		/* FIRST DO COMPLETELY VISIBLE PORTIONS */
		for ( data = (char *) pTile->devPrivate.ptr,
		      vcount = savevcount,
		      vtarget = y ;
		      vcount-- ;
		      vtarget += tly ) {
			for ( hcount = savehcount,
			      htarget = x ;
			      hcount-- ;
			      htarget += tlx ) {
				(*fnp)( htarget, vtarget,
					tlx, tly, data,pTile->devKind,
					alu, planes ) ;
			}
		}
		
		/* NOW DO RIGHT HAND SIDE */
		if ( Rtlx = w % tlx ) {
			if ( !( pTmpTile = ppcCreatePixmap( pScreen, Rtlx, tly, depth ) ) ) {
				ErrorF( "Failed To Create Right Pixmap in ppcTileRect\n" ) ;
				return ;
			}
			if (depth==1)
				ppcClipZ1Pixmap( pTile, pTmpTile ) ;
			   else ppcClipZ8Pixmap( pTile, pTmpTile ) ;
		
			for ( data = (char *) pTmpTile->devPrivate.ptr,
			      vcount = savevcount,
			      htarget = x + savehcount * tlx,
			      vtarget = y ;
			      vcount-- ;
			      vtarget += tly ) {
				(*fnp)( htarget, vtarget,
					Rtlx, tly, data, pTmpTile->devKind,
					alu, planes ) ;
			}
			mfbDestroyPixmap( pTmpTile ) ;
		}
		
		/* NOW DO BOTTOM */
		if ( Btly = h % tly ) {
			for ( data = (char *) pTile->devPrivate.ptr,
			      hcount = savehcount,
			      htarget = x,
			      vtarget = y + savevcount * tly ;
			      hcount-- ;
			      htarget += tlx ) {
				(*fnp)( htarget, vtarget,
					tlx, Btly, data,pTile->devKind,
					alu, planes ) ;
			}
			if ( Rtlx ) {
			/* NOW DO BOTTOM RIGHT CORNER */
				if ( !( pTmpTile = ppcCreatePixmap( pScreen, Rtlx, Btly, depth ) ) ) {
					ErrorF( "Failed To Create Corner Pixmap in ppcTileRect\n" ) ;
					return ;
				}
				if (depth==1)
					ppcClipZ1Pixmap( pTile, pTmpTile ) ;
				   else ppcClipZ8Pixmap( pTile, pTmpTile ) ;
				(*fnp)( ( x + savehcount * tlx ),
					( y + savevcount * tly ),
					Rtlx, Btly, pTmpTile->devPrivate.ptr,
					pTmpTile->devKind, alu, planes ) ;
				mfbDestroyPixmap( pTmpTile ) ;
			}
		}
	}
	break ;
}

if ( TileCopied )
	mfbDestroyPixmap( pTile ) ;

return ;
}
