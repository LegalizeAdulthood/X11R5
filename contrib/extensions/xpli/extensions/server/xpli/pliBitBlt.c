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

/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliBitBlt.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliBitBlt.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */

#ifndef lint
static char *rcsid = "$Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliBitBlt.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $" ;
#endif


/*
 * REGISTER USAGE
 * 0x3C4 -- GENERAL REGISTER INDEX
 * 0x3C5 -- MAP MASK
 * 0x3CE -- GRAPHICS REGISTER INDEX
 * 0x3CF -- DATA ROTATE
 * 0x3CF -- BIT MASK
 */

#include "newmacros.h"
#include "X.h"
#include "OScompiler.h"
#include "pliVideo.h"
/*
#include "ibmIOArch.h"
*/
#include "pliReg.h"

#undef TRUE
#undef FALSE
#define TRUE 1
#define FALSE 0

#ifdef LEFT_SHIFT
#undef LEFT_SHIFT
#endif

#ifdef NO_SHIFT
#undef NO_SHIFT
#endif

#define RIGHT_SHIFT

static /* fast_blt_Right() */
#include "pliAddr.c"

#undef RIGHT_SHIFT


#define LEFT_SHIFT
static /* fast_blt_Left() */
#include "pliAddr.c"
#undef LEFT_SHIFT


#define NO_SHIFT
#define MOVE_RIGHT
static /* fast_blt_Aligned_Right() */
#include "pliAddr.c"
#undef MOVE_RIGHT

#define MOVE_LEFT
static /* fast_blt_Aligned_Left() */
#include "pliAddr.c"
#undef MOVE_LEFT
#undef NO_SHIFT

static void
fix_video_byte( source, destination, byte_offset, alu )
register volatile unsigned char *source ;
register volatile unsigned char *destination ;
register const int byte_offset ;
register const int alu ;
{
register unsigned long int tmp1, tmp2 ;

	if ( byte_offset )

/* This Code MIGHT try to read the byte before the start of the screen */

	{
		if ( source != PLIBASE )
		tmp1 =
	  SCRRIGHT8( *( (PliMemoryPtr) ( source ) ), byte_offset )
	| SCRLEFT8( *( (PliMemoryPtr) ( source - 1 ) ), ( 8 - byte_offset ) ) ;
		else
		tmp1 =
	  SCRRIGHT8( *( (PliMemoryPtr) ( source ) ), byte_offset ) ;
	}
	else
		tmp1 = *( (PliMemoryPtr) source ) ;
	tmp2 = *( (PliMemoryPtr) destination ) ;
	switch ( alu ) {
	case GXnor:
		tmp1 = ~( tmp1 | tmp2 ) ;
		break ;
	case GXandInverted:
		tmp1 = ~tmp1 & tmp2 ;
		break ;
	case GXand:
		tmp1 &= tmp2 ;
		break ;
	case GXequiv:
		tmp1 = ~tmp1 ^ tmp2 ;
		break ;
	case GXxor:
		tmp1 ^= tmp2 ;
		break ;
	case GXandReverse:
		tmp1 &= ~tmp2 ;
		break ;
	case GXorReverse:
		tmp1 |= ~tmp2 ;
		break ;
	case GXnand:
		tmp1 = ~( tmp1 & tmp2 ) ;
		break ;
	case GXorInverted:
		tmp1 = ~tmp1 | tmp2 ;
		break ;
	case GXor:
		tmp1 |= tmp2 ;
		break ;
	case GXcopyInverted:
		tmp1 = ~tmp1 ;
	case GXcopy:
	default:
		break ;
	}
	*( (PliMemoryPtr) destination ) = tmp1 ;
return ;
}

static void
fix_video_byte_copy_edgeLoop( source, destination, byte_offset,
			      height, y_direction )
register volatile unsigned char *source ;
register volatile unsigned char *destination ;
register const int byte_offset ;
register unsigned int height ;
register int y_direction ;
{
register unsigned long int tmp1, tmp2 ;

	for ( ; height-- ; destination += y_direction, source += y_direction ) {
		if ( byte_offset )
	/* This Code MIGHT try to read the byte before the start of the screen */
		{
			if ( source != PLIBASE )
			tmp1 =
		  SCRRIGHT8( *( (PliMemoryPtr) ( source ) ), byte_offset )
		| SCRLEFT8( *( (PliMemoryPtr) ( source - 1 ) ), ( 8 - byte_offset ) ) ;
			else
			tmp1 =
		  SCRRIGHT8( *( (PliMemoryPtr) ( source ) ), byte_offset ) ;
		}
		else
			tmp1 = *( (PliMemoryPtr) source ) ;
		tmp2 = *( (PliMemoryPtr) destination ) ;
		*( (PliMemoryPtr) destination ) = tmp1 ;
	}
return ;
}

static void
edgeLoop( dest, src, height, y_direction )
register unsigned char *dest ;
register unsigned char *src ;
register unsigned int height ;
register int y_direction ;
{
register unsigned char tmp1, tmp2 ;

	for ( ; height-- ; dest += y_direction, src += y_direction ) {
		tmp1 = *( (PliMemoryPtr) src ) ;
		tmp2 = *( (PliMemoryPtr) dest ) ;
		*( (PliMemoryPtr) dest ) = tmp1 ;
	}
return ;
}

extern int pliFillSolid() ;
/* Declared in "plicurs.c" */
extern int pliCursorSemaphore ;
extern int pliCheckCursor() ;
extern void pliReplaceCursor() ;

void
pliBitBlt( alu, readplanes, writeplanes, x0, y0, x1, y1, w, h )
const int alu, readplanes, writeplanes ;
register int x0 ;
int y0 ;
register int x1 ;
int y1 ;
register int w, h ;
{
register volatile unsigned char *s1ptr ;
register unsigned int j ;
register volatile unsigned char *d1ptr ;
register center_width ;
register int x_interval ;
register int y_interval ;
register unsigned int i ;
register volatile unsigned char *src ;
register volatile unsigned char *dst ;
unsigned int currplane ;
int byte_offset ;
int left_ragged ;
int right_ragged ;
unsigned char first_edgeMask ;
unsigned char second_edgeMask ;
int cursor_saved ;

switch ( alu ) {
	case GXclear:		/* 0x0 Zero 0 */
	case GXinvert:		/* 0xa NOT dst */
	case GXset:		/* 0xf 1 */
		pliFillSolid( PLI_ALLPLANES, alu, writeplanes, x0, y0, w, h ) ;
	case GXnoop:		/* 0x5 dst */
		return ;
	default:
		break ;
}

left_ragged  = BIT_OFFSET( x1 ) ;
right_ragged = 7 - BIT_OFFSET( x1 + w - 1 ) ;
center_width = ROW_OFFSET( x1 + w ) - ROW_OFFSET( ( x1 + 0x7 ) & ~0x7 ) ;

src = (unsigned char *) PLIBASE + ( BYTES_PER_ROW * y0 ) ;
dst = (unsigned char *) PLIBASE + ( BYTES_PER_ROW * y1 ) ;
if ( y1 > y0 ) {
	y_interval = - BYTES_PER_ROW ;
	src += BYTES_PER_ROW * ( h - 1 ) ;
	dst += BYTES_PER_ROW * ( h - 1 ) ;
}
else {
	y_interval = BYTES_PER_ROW ;
}

if ( x1 < x0 ) {
	x_interval = 1 ;
	src += ROW_OFFSET( x0 ) ;
	dst += ROW_OFFSET( x1 ) ;
	byte_offset = left_ragged - BIT_OFFSET( x0 ) ;
	if ( center_width < 0 ) {
		first_edgeMask = SCRRIGHT8( 0xFF, left_ragged )
			       & SCRLEFT8( 0xFF, right_ragged ) ;
		second_edgeMask = 0 ;
	}
	else {
		first_edgeMask = left_ragged ?
				 SCRRIGHT8( 0xFF, left_ragged ) : 0 ;
		second_edgeMask = right_ragged ?
				 SCRLEFT8( 0xFF, right_ragged ) : 0 ;
	}
}
else {
	x_interval = -1 ;
	src += ROW_OFFSET( x0 + w - 1 ) ;
	dst += ROW_OFFSET( x1 + w - 1 ) ;
	byte_offset = 7 - BIT_OFFSET( x0 + w - 1 ) - right_ragged ;
	if ( center_width < 0 ) {
		first_edgeMask = SCRRIGHT8( 0xFF, left_ragged )
			       & SCRLEFT8( 0xFF, right_ragged ) ;
		second_edgeMask = 0 ;
	}
	else {
		first_edgeMask = right_ragged ?
				 SCRLEFT8( 0xFF, right_ragged ) : 0 ;
		second_edgeMask = left_ragged ?
				 SCRRIGHT8( 0xFF, left_ragged ) : 0 ;
	}
}
/*
 * byte_offset <==> Number Of Bits To Move To The Right
 * EXAMPLES:
 * if ( byte_offset == 2 )  THEN Shift "src" 2 Bits To To The Right
 *			    AND Shift In Byte From The Left
 * or
 * if ( byte_offset == -2 ) THEN Shift "src" 2 Bits To To The Left
 *			    AND Shift In Byte From The Right
 */
if ( byte_offset < 0 ) {
	src += 1 ;
	byte_offset += 8 ;
}
/* If Cursor Is In The Way Remove It */
cursor_saved = !pliCursorSemaphore
	    && ( pliCheckCursor( x0, y0, w, h )
	      || pliCheckCursor( x1, y1, w, h ) ) ;

/* Disable SET/RESET Function */
SetVideoGraphics( Enb_Set_ResetIndex, 0 ) ;
/* Set Write Mode To 0 -- Read Mode To 0 */
SetVideoGraphics( Graphics_ModeIndex, PLI_READ_MODE_0 | PLI_WRITE_MODE_0 ) ;

/* Test For Special Case -- Try To Do Fast Blt */
if ( alu == GXcopy ) {
	if ( !byte_offset ) { /* Test For Special Case -- VERY Fast Blt */
		/* Prepare To Set Bit Mask */
		SetVideoGraphicsIndex( Bit_MaskIndex ) ;
		if ( first_edgeMask ) {
			outb( 0x3CF, first_edgeMask ) ;
			/* Set Data Rotate Function To Direct Write */
			SetVideoGraphics( Data_RotateIndex, PLI_COPY_MODE ) ;
			for ( currplane = 0 ;
			      currplane <= PLI_MAXPLANES ;
			      currplane++  ) {
				if ( i = writeplanes & ( 1 << currplane ) ) {
					SetVideoSequencer( Mask_MapIndex, i ) ;
					/* Set Map Read Select */
					SetVideoGraphics( Read_Map_SelectIndex,
							  currplane ) ;
					/* Move First Edge */
					edgeLoop( dst, src, h, y_interval ) ;
				}
			}
			if ( center_width < 0 ) {
				if ( cursor_saved )
					pliReplaceCursor() ;
				return ;
			}
			src += x_interval ;
			dst += x_interval ;
		}
		/* Set Map Mask */
		SetVideoSequencer( Mask_MapIndex, writeplanes & PLI_ALLPLANES );
		/* Point At The Bit Mask Reg */
		SetVideoGraphicsIndex( Bit_MaskIndex ) ;
		/* Move Center Of Box */
		if ( center_width ) {
			/* Set Bit Mask -- ALL OFF */
			outb( 0x3CF, 0 ) ;
			(* ( ( x_interval > 0 )
			   ? fast_blt_Aligned_Right : fast_blt_Aligned_Left ) )
				( src, dst, center_width, h, y_interval ) ;
		}
		if ( !second_edgeMask ) {
			if ( cursor_saved )
				pliReplaceCursor() ;
			return ;
		}
		/* Move Second Edge */
		outb( 0x3CF, second_edgeMask ) ;
		/* Adjust Offsets */
		if ( x_interval > 0 ) {
			src += center_width ;
			dst += center_width ;
		}
		else { /* Move Left Edge */
			src -= center_width ;
			dst -= center_width ;
		}
		/* Set Data Rotate Function To Direct Write */
		SetVideoGraphics( Data_RotateIndex, PLI_COPY_MODE ) ;
		for ( currplane = 0 ;
		      currplane <= PLI_MAXPLANES ;
		      currplane++  ) {
			if ( i = writeplanes & ( 1 << currplane ) ) {
				SetVideoSequencer( Mask_MapIndex, i ) ;
				/* Set Map Read Select */
				SetVideoGraphics( Read_Map_SelectIndex,
						  currplane ) ;
				edgeLoop( dst, src, h, y_interval ) ;
			}
		}
	} /* End Of Very Fast BitBlt */
	else {	/* Slow GXcopy BitBlt Here -- Bits in Bytes NOT Aligned */
		/* Set Data Rotate Function To Direct Write */
		SetVideoGraphics( Data_RotateIndex, PLI_COPY_MODE ) ;
		for ( currplane = 0 ;
		      currplane <= PLI_MAXPLANES ;
		      currplane++  ) {
			if ( i = writeplanes & ( 1 << currplane ) ) {
				/* Logical Operation Depending On Src & Dst */
				SetVideoSequencer( Mask_MapIndex, i ) ;
				/* Set Map Read Select */
				SetVideoGraphics( Read_Map_SelectIndex,
						  currplane ) ;
				SetVideoGraphics( Bit_MaskIndex, 0xFF ) ;
				/* Move First Edge */
				s1ptr = src ;
				d1ptr = dst ;
				if ( first_edgeMask ) {
					outb( 0x3CF, first_edgeMask ) ;
					fix_video_byte_copy_edgeLoop(
					  src, dst, byte_offset,
					  h, y_interval ) ;
					if ( center_width < 0 ) /* All In One Byte */
						continue ; /* Next Plane */
					outb( 0x3CF, 0xFF ) ;
					s1ptr = src + x_interval ;
					d1ptr = dst + x_interval ;
				}
				/* Move Center Of Box */
				if ( center_width ) {
					(* ( ( x_interval >= 0 )
					 ? fast_blt_Right : fast_blt_Left ) )
					( ( ( x_interval >= 0 ) ? s1ptr : s1ptr - 1 ),
					 d1ptr, byte_offset,
					 center_width, h, y_interval ) ;

					if ( x_interval >= 0 ) {
						s1ptr += center_width ;
						d1ptr += center_width ;
					}
					else {
						s1ptr -= center_width ;
						d1ptr -= center_width ;
					}
				}
				/* Move Second Edge */
				if ( second_edgeMask ) {
					outb( 0x3CF, second_edgeMask ) ;
					fix_video_byte_copy_edgeLoop(
					  s1ptr, d1ptr, byte_offset,
					  h, y_interval ) ;
					outb( 0x3CF, 0xFF ) ;
				}
			}
		}
		/* Re-Enable All Planes In Map Mask */
		SetVideoSequencer( Mask_MapIndex, PLI_ALLPLANES ) ;
	}

	if ( cursor_saved )
		pliReplaceCursor() ;
	return ;
} /* End Of GXcopy BitBlt */

/* Slow BitBlt Here */
/* Set Data Rotate Function To Direct Write */
SetVideoGraphics( Data_RotateIndex, PLI_COPY_MODE ) ;
for ( currplane = 0 ; currplane <= PLI_MAXPLANES ; currplane++  ) {
	if ( i = writeplanes & ( 1 << currplane ) ) {
		register volatile unsigned char *s2ptr ;
		register volatile unsigned char *d2ptr ;
		/* Logical Operation Depending On Src & Dst */

		SetVideoSequencer( Mask_MapIndex, i ) ;
		/* Set Map Read Select */
		SetVideoGraphics( Read_Map_SelectIndex,
				  currplane ) ;
		SetVideoGraphics( Bit_MaskIndex, 0xFF ) ;
		/* Move First Edge */
		s1ptr = src ;
		d1ptr = dst ;
		if ( first_edgeMask ) {
			outb( 0x3CF, first_edgeMask ) ;
			for ( d1ptr = dst, s1ptr = src, j = h ;
		 	      j-- ;
			      d1ptr += y_interval, s1ptr += y_interval )
				fix_video_byte( s1ptr, d1ptr, byte_offset, alu ) ;
			if ( center_width < 0 ) /* All In One Byte */
				continue ; /* Next Plane */
			outb( 0x3CF, 0xFF ) ;
			s1ptr = src + x_interval ;
			d1ptr = dst + x_interval ;
		}
		/* Move Center Of Box */
		for ( i = h ;
		      i-- ;
		      s1ptr += y_interval, d1ptr += y_interval )
			for ( j = center_width, s2ptr = s1ptr,
			      d2ptr = d1ptr ;
			      j-- ;
			      s2ptr += x_interval, d2ptr += x_interval ) {
				register unsigned long int tmp1 ;

				if ( byte_offset ) {
					tmp1 =
				  SCRRIGHT8( *( (PliMemoryPtr) ( s2ptr ) ), byte_offset )
				| SCRLEFT8( *( (PliMemoryPtr) ( s2ptr - 1 ) ),
					   ( 8 - byte_offset ) ) ;
				}
				else
					tmp1 = *( (PliMemoryPtr) s2ptr ) ;
				switch ( alu ) {
				case GXnor:
					tmp1 = ~( tmp1
					 | *( (PliMemoryPtr) d2ptr ) ) ;
					break ;
				case GXandInverted:
					tmp1 =
				~tmp1 & *( (PliMemoryPtr) d2ptr ) ;
					break ;
				case GXand:
					tmp1 &= *( (PliMemoryPtr) d2ptr ) ;
					break ;
				case GXequiv:
					tmp1 = ~tmp1 ^ *( (PliMemoryPtr) d2ptr ) ;
					break ;
				case GXxor:
					tmp1 ^= *( (PliMemoryPtr) d2ptr ) ;
					break ;
				case GXandReverse:
					tmp1 &= ~ *( (PliMemoryPtr) d2ptr ) ;
					break ;
				case GXorReverse:
					tmp1 |= ~*( (PliMemoryPtr) d2ptr ) ;
					break ;
				case GXnand:
					tmp1 = ~( tmp1 & *( (PliMemoryPtr) d2ptr ) ) ;
					break ;
				case GXorInverted:
					tmp1 = ~tmp1 | *( (PliMemoryPtr) d2ptr ) ;
					break ;
				case GXor:
					tmp1 |= *( (PliMemoryPtr) d2ptr ) ;
					break ;
				case GXcopyInverted:
					tmp1 = ~*( (PliMemoryPtr) d2ptr ) ;
				default:
					break ;
				}
				*( (PliMemoryPtr) d2ptr ) = tmp1 ;
		}
		/* Adjust Offsets */
		j = ( h * y_interval ) - ( center_width * x_interval ) ;
		s1ptr -= j ;
		d1ptr -= j ;
		/* Move Second Edge */
		if ( second_edgeMask ) {
			outb( 0x3CF, second_edgeMask ) ;
			for ( j = h ;
		 	      j-- ;
			      d1ptr += y_interval, s1ptr += y_interval )
				fix_video_byte( s1ptr, d1ptr, byte_offset, alu ) ;
			outb( 0x3CF, 0xFF ) ;
		}
	}
}
/* Re-Enable All Planes In Map Mask */
SetVideoSequencer( Mask_MapIndex, PLI_ALLPLANES ) ;

if ( cursor_saved )
	pliReplaceCursor() ;
return ;
}
