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
/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliGlyph.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliGlyph.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */

#ifndef lint
static char *rcsid = "$Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliGlyph.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $" ;
#endif


#include "newmacros.h"
#include "X.h"
#include "misc.h"	/* Need typedef of "Bool" */

#include "OScompiler.h"

/*
#include "ibmIOArch.h"
*/

#include "pliVideo.h"

#include "pliReg.h"

#define CURSOR_ONLY
#include "pliProcs.h"

#undef TRUE
#undef FALSE
#define TRUE 1
#define FALSE 0

void 
pliDrawGlyph( data, x, y, w, h, fg, alu, planes )
register const unsigned char *data ;
register int x ;
int y ;
register int w ;
int h ;
unsigned long int fg ;
const int alu ;
const unsigned long int planes ;
{
register const unsigned char *cptr ;
register int shift ;
register unsigned SourceRow ;
register int tmp1 ;
register int tmp2 ;
register volatile unsigned char *dst ;
register const unsigned char *lptr ;
register unsigned int paddedByteWidth ;
int cursor_saved ;
int data_rotate_value = PLI_COPY_MODE ;

/* Test The Raster-Op */
{
	unsigned int invert_existing_data = FALSE ;

	switch ( alu ) {
		case GXclear:		/* 0x0 Zero 0 */
			fg = 0 ;
			break ;
		case GXinvert:		/* 0xa NOT dst */
			data_rotate_value = PLI_XOR_MODE ;
		case GXset:		/* 0xf 1 */
			fg = PLI_ALLPLANES ;
			break ;
		case GXnoop:		/* 0x5 dst */
			return ;
		case GXnor:		/* 0x8 NOT src AND NOT dst */
			invert_existing_data = TRUE ;
		case GXandInverted:	/* 0x4 NOT src AND dst */
			fg = ~fg ;
		case GXand:		/* 0x1 src AND dst */
			data_rotate_value = PLI_AND_MODE ;
		case GXcopy:		/* 0x3 src */
			break ;
		case GXequiv:		/* 0x9 NOT src XOR dst */
			fg = ~fg ;
		case GXxor:		/* 0x6 src XOR dst */
			data_rotate_value = PLI_XOR_MODE ;
			break ;
		case GXandReverse:	/* 0x2 src AND NOT dst */
			invert_existing_data = TRUE ;
			data_rotate_value = PLI_AND_MODE ;
			break ;
		case GXorReverse:	/* 0xb src OR NOT dst */
			invert_existing_data = TRUE ;
			data_rotate_value = PLI_OR_MODE ;
			break ;
		case GXnand:		/* 0xe NOT src OR NOT dst */
			invert_existing_data = TRUE ;
		case GXorInverted:	/* 0xd NOT src OR dst */
			fg = ~fg ;
		case GXor:		/* 0x7 src OR dst */
			data_rotate_value = PLI_OR_MODE ;
			break ;
		case GXcopyInverted:	/* 0xc NOT src */
			fg = ~fg ;
		default:
			break ;
	}
	if ( invert_existing_data )
		pliDrawGlyph( data, x, y, w, h,
			      PLI_ALLPLANES, GXinvert, planes ) ;
}

/* Check If Cursor Is In The Way */
cursor_saved = !pliCursorSemaphore && pliCheckCursor( x, y, w, h ) ;

/* Figure Bit Offsets & Source Address */
/* Pad To Byte Boundary */
paddedByteWidth = ( ( w + 07 ) & ~07 ) >> 3 ;

/* Setup PLI Registers */
/*
 * Set The Plane-Enable
 */
SetVideoSequencer( Mask_MapIndex, planes & PLI_ALLPLANES ) ;
/*
 * Set The Color in The Set/Reset Register
 */
SetVideoGraphics( Set_ResetIndex, fg & PLI_ALLPLANES ) ;
/*
 * Set The Pli's Alu Function
 */
SetVideoGraphics( Data_RotateIndex, data_rotate_value ) ;
/*
 * Put Display Into SET/RESET Write Mode
 */
SetVideoGraphics( Graphics_ModeIndex, PLI_WRITE_MODE_3 ) ;
/*
 * Point At The Bit Mask Reg
 */
SetVideoGraphicsIndex( Bit_MaskIndex ) ;

/* Do Left Edge */
if ( shift = x & 07 ) { /* shift is amount to shift right */
	/*
	 * For Each Line In The Cursor Data
	 */
	dst = SCREENADDRESS( x, y ) ;

	/* Set-Up The Bit Mask */
	tmp2 = SCRRIGHT8( 0xFF, shift ) ;
	/* Catch The Cases Where The Entire Region Is Within One Byte */
	if ( ( w -= 8 - shift ) < 0 ) {
		tmp2 &= SCRLEFT8( 0xFF, -w ) ;
		w = 0 ;
	}
	SetVideoGraphicsData( tmp2 ) ; /* Set The Bit Mask Reg */
	for ( SourceRow = h,
	      cptr = data ;
	      SourceRow-- ;
	      dst += BYTES_PER_ROW, cptr += paddedByteWidth ) {
		/* Read To Save */
		tmp2 = *( (PliMemoryPtr) dst ) ;
		/* Write Pattern */
		*( (PliMemoryPtr) dst ) = SCRRIGHT8( *cptr, shift ) ;
	}
	if ( w )
		x = ( x + 7 ) & ~07 ; /* Byte Align "x" */
	else {
		if ( cursor_saved )
			pliReplaceCursor() ;
		return ;
	}
}

/* Fill The Center Of The Box */
/*
 * For Each Line In The Source Pixmap
 */
if ( ROW_OFFSET( w ) ) {
	SetVideoGraphicsData( 0xFF ) ; /* Set The Bit Mask Reg */
	for ( SourceRow = h,
	      dst = SCREENADDRESS( x, y ),
	      lptr = data ;
	      SourceRow-- ;
	      dst += BYTES_PER_ROW - ROW_OFFSET( w ),
	      lptr += paddedByteWidth ) {
		/*
		 * For Each Byte Across The Pattern In The X Dimension
		 */
		tmp2 = ROW_OFFSET( w ) ;
		cptr = lptr ;
		if ( shift ) {
			register unsigned long int savebits ;

			for ( savebits = SCRLEFT8( *cptr++, ( 8 - shift ) ) ;
			      tmp2-- ;
			      cptr++, dst++ ) {
				/* Read To Save */
				tmp1 = *( (PliMemoryPtr) dst ) ;
				tmp1 = *cptr ;
				/* Write Pattern */
				*( (PliMemoryPtr) dst ) =
					savebits | SCRRIGHT8( tmp1, shift ) ;
				savebits = SCRLEFT8( tmp1, ( 8 - shift ) ) ;
			}
		}
		else
			for ( ;
			      tmp2-- ;
			      cptr++, dst++ ) {
				/* Read To Save */
				tmp1 = *( (PliMemoryPtr) dst ) ;
				/* Write Pattern */
				*( (PliMemoryPtr) dst ) = *cptr ;
			}
	}
}

/* Do Right Edge */
if ( tmp1 = BIT_OFFSET( w ) ) { /* x Now Is Byte Aligned */
	/* Set The Bit Mask */
	SetVideoGraphicsData( SCRLEFT8( 0xFF, ( 8 - tmp1 ) ) ) ;
	/*
	 * For Each Line In The Source Pixmap
	 */
	cptr = data + ROW_OFFSET( w ) ;
	SourceRow = h ;
	dst = SCREENADDRESS( ( x + w ), y ) ;
	if ( shift )
		for ( ;
		      SourceRow-- ;
		      dst += BYTES_PER_ROW, cptr += paddedByteWidth ) {
			/* Read To Save */
			tmp1 = *( (PliMemoryPtr) dst ) ;
			/* Write Pattern */
			*( (PliMemoryPtr) dst ) =
			  SCRLEFT8( *cptr, ( 8 - shift ) )
			| SCRRIGHT8( cptr[1], shift ) ;
		}
	else /* shift == 0 */
		for ( ;
		      SourceRow-- ;
		      dst += BYTES_PER_ROW, cptr += paddedByteWidth ) {
			/* Read To Save */
			tmp1 = *( (PliMemoryPtr) dst ) ;
			/* Write Pattern */
			*( (PliMemoryPtr) dst ) = *cptr ;
		}
}

if ( cursor_saved )
	pliReplaceCursor() ;

return ;
}
