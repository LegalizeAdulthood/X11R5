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
/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliImages.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliImages.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */

#ifndef lint
static char *rcsid = "$Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliImages.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $" ;
#endif

#include "newmacros.h"
#include "X.h"

#include "OScompiler.h"

/*
#include "ibmIOArch.h"
*/

#include "pliVideo.h"

#include "pliReg.h"

#undef TRUE
#undef FALSE
#define TRUE 1
#define FALSE 0

extern void pliFillSolid() ;
/* Declared in "plicurs.c" */
extern int pliCursorSemaphore ;
extern int pliCheckCursor() ;
extern void pliReplaceCursor() ;

void
pliDrawColorImage( x, y, w, h, data, RowIncrement, alu, planes )
int x, y ;
register int w, h ;
register const unsigned char *data ;
register int RowIncrement ;
const int alu ;
const unsigned long int planes ;
{
register unsigned long int tmp ;
register const unsigned char *src ;
register volatile unsigned char *dst ;
register int Pixel_Count ;
register unsigned int currMask ;
register unsigned int InitialMask ;
register volatile unsigned char *StartByte ;
unsigned int invert_source_data = FALSE ;
int cursor_saved ;

{
	unsigned int invert_existing_data = FALSE ;
	unsigned int data_rotate_value = PLI_COPY_MODE ;

	switch ( alu ) {
		case GXclear:		/* 0x0 Zero 0 */
		case GXinvert:		/* 0xa NOT dst */
		case GXset:		/* 0xf 1 */
			pliFillSolid( PLI_ALLPLANES, alu, planes, x, y, w, h ) ;
		case GXnoop:		/* 0x5 dst */
			return ;
		case GXnor:		/* 0x8 NOT src AND NOT dst */
			invert_existing_data = TRUE ;
		case GXandInverted:	/* 0x4 NOT src AND dst */
			invert_source_data = TRUE ;
		case GXand:		/* 0x1 src AND dst */
			data_rotate_value = PLI_AND_MODE ;
			break ;
		case GXequiv:		/* 0x9 NOT src XOR dst */
			invert_source_data = TRUE ;
		case GXxor:		/* 0x6 src XOR dst */
			data_rotate_value = PLI_XOR_MODE ;
			break ;
		case GXandReverse:	/* 0x2 src AND NOT dst */
			invert_existing_data = TRUE ;
			data_rotate_value = PLI_AND_MODE ;
			break ;
		case GXnand:		/* 0xe NOT src OR NOT dst */
			invert_source_data = TRUE ;
			invert_existing_data = TRUE ;
		case GXorReverse:	/* 0xb src OR NOT dst */
			data_rotate_value = PLI_OR_MODE ;
			break ;
		case GXorInverted:	/* 0xd NOT src OR dst */
			invert_source_data = TRUE ;
		case GXor:		/* 0x7 src OR dst */
			data_rotate_value = PLI_OR_MODE ;
			break ;
		case GXcopyInverted:	/* 0xc NOT src */
			invert_source_data = TRUE ;
		case GXcopy:		/* 0x3 src */
		default:
			break ;
	}
	cursor_saved = !pliCursorSemaphore && pliCheckCursor( x, y, w, h ) ;
	if ( invert_existing_data )
		pliFillSolid( PLI_ALLPLANES, GXinvert, planes, x, y, w, h ) ;
	/* Setup PLI Registers */
	SetVideoSequencer( Mask_MapIndex, planes & PLI_ALLPLANES ) ;
	/* Set Raster Op */
	SetVideoGraphics( Data_RotateIndex, data_rotate_value ) ;
	SetVideoGraphics( Graphics_ModeIndex, PLI_WRITE_MODE_2 ) ;
	SetVideoGraphicsIndex( Bit_MaskIndex ) ;
}

StartByte = (volatile unsigned char *)
	( PLIBASE + ( y * BYTES_PER_ROW ) + ROW_OFFSET( x ) ) ;
InitialMask = SCRRIGHT8( LeftmostBit, BIT_OFFSET( x ) ) ;
if ( invert_source_data )
	for ( ;
	      h-- ;
	      data += RowIncrement, StartByte += BYTES_PER_ROW ) {
		for ( dst = StartByte, src = data,
		      Pixel_Count = w, currMask = InitialMask ;
		      Pixel_Count-- ;
		      src++ ) {
			/* Set The Bit Mask Reg */
			SetVideoGraphicsData( currMask ) ;
			/* Read To Load pli Data Latches */
			tmp = *( (PliMemoryPtr) dst ) ;
			*( (PliMemoryPtr) dst ) = ~ *src ;
			if ( currMask & RightmostBit ) {
				currMask = LeftmostBit ;
				dst++ ;
			}
			else
				currMask = SCRRIGHT8( currMask, 1 ) ;
		}
	}
else /* invert_source_data == FALSE */
	for ( ;
	      h-- ;
	      data += RowIncrement, StartByte += BYTES_PER_ROW ) {
		for ( dst = StartByte, src = data,
		      Pixel_Count = w, currMask = InitialMask ;
		      Pixel_Count-- ;
		      src++ ) {
			/* Set The Bit Mask Reg */
			SetVideoGraphicsData( currMask ) ;
			/* Read To Load pli Data Latches */
			tmp = *( (PliMemoryPtr) dst ) ;
			*( (PliMemoryPtr) dst ) = *src ;
			if ( currMask & RightmostBit ) {
				currMask = LeftmostBit ;
				dst++ ;
			}
			else
				currMask = SCRRIGHT8( currMask, 1 ) ;
		}
	}
 
if ( cursor_saved )
	pliReplaceCursor() ;

return ;
}

static unsigned long int
read8Z( screen_ptr )
register volatile unsigned char *screen_ptr ;
{
register unsigned long int i ;
register unsigned long int j ;

/* Read One Byte At A Time to get
 *	i ==	[ Plane 3 ] [ Plane 2 ] [ Plane 1 ] [ Plane 0 ]
 * into a single register
 */

/*
 * Make that [ 0 ] [ Plane 2 ] [ Plane 1 ] [Plane 0 ]
 * Erb 5/10/90
 * (Read plane 3 anyway, in case the hardware needs us to)
 */
SetVideoGraphicsData( 3 ) ;
i = *( (PliMemoryPtr) screen_ptr ) << 8 ;
i = 0;	/* This is it! */

SetVideoGraphicsData( 2 ) ;
i |= *( (PliMemoryPtr) screen_ptr ) ;
i <<= 8 ;
SetVideoGraphicsData( 1 ) ;
i |= *( (PliMemoryPtr) screen_ptr ) ;
i <<= 8 ;
SetVideoGraphicsData( 0 ) ;
i |= *( (PliMemoryPtr) screen_ptr ) ;

/* Push Bits To Get
 * j ==	[Pixel 7][Pixel 6][Pixel 5][Pixel 4][Pixel 3][Pixel 2][Pixel 1][Pixel 0]
 * into one register
 */

j = ( i & 0x1 ) << 4 ;
j |= ( i >>= 1 ) & 0x1 ;
j <<= 4 ;
j |= ( i >>= 1 ) & 0x1 ;
j <<= 4 ;
j |= ( i >>= 1 ) & 0x1 ;
j <<= 4 ;
j |= ( i >>= 1 ) & 0x1 ;
j <<= 4 ;
j |= ( i >>= 1 ) & 0x1 ;
j <<= 4 ;
j |= ( i >>= 1 ) & 0x1 ;
j <<= 4 ;
j |= ( i >>= 1 ) & 0x1 ;

j |= ( i & 0x2 ) << 28 ;
j |= ( ( i >>= 1 ) & 0x2 ) << 24 ;
j |= ( ( i >>= 1 ) & 0x2 ) << 20 ;
j |= ( ( i >>= 1 ) & 0x2 ) << 16 ;
j |= ( ( i >>= 1 ) & 0x2 ) << 12 ;
j |= ( ( i >>= 1 ) & 0x2 ) <<  8 ;
j |= ( ( i >>= 1 ) & 0x2 ) <<  4 ;
j |= ( i >>= 1 ) & 0x2 ;

j |= ( i & 0x4 ) << 28 ;
j |= ( ( i >>= 1 ) & 0x4 ) << 24 ;
j |= ( ( i >>= 1 ) & 0x4 ) << 20 ;
j |= ( ( i >>= 1 ) & 0x4 ) << 16 ;
j |= ( ( i >>= 1 ) & 0x4 ) << 12 ;
j |= ( ( i >>= 1 ) & 0x4 ) <<  8 ;
j |= ( ( i >>= 1 ) & 0x4 ) <<  4 ;
j |= ( i >>= 1 ) & 0x4 ;

/*
 * Erb 5/10/90
 * Skip this as per above, though it would do nothing,
 * since we zeroed out plane 3
 */
/*
j |= ( i & 0x8 ) << 28 ;
j |= ( ( i >>= 1 ) & 0x8 ) << 24 ;
j |= ( ( i >>= 1 ) & 0x8 ) << 20 ;
j |= ( ( i >>= 1 ) & 0x8 ) << 16 ;
j |= ( ( i >>= 1 ) & 0x8 ) << 12 ;
j |= ( ( i >>= 1 ) & 0x8 ) <<  8 ;
j |= ( ( i >>= 1 ) & 0x8 ) <<  4 ;
j |= ( i >>= 1 ) & 0x8 ;
*/

return j ;
}

void
pliReadColorImage( x, y, lx, ly, data, RowIncrement )
int x, y ;
int lx, ly ;
register unsigned char *data ;
int RowIncrement ;
{
register unsigned long int tmp ;
register volatile unsigned char *src ;
volatile unsigned char *masterSrc ;
int savCenterWidth ;
int dx ;
int skip ;
int center_width ;
int ignore ;
int pad ;
int cursor_saved ;

if ( ( lx <= 0 ) || ( ly <= 0 ) )
	return ;

cursor_saved = !pliCursorSemaphore && pliCheckCursor( x, y, lx, ly ) ;

/* Setup PLI Registers */
SetVideoGraphicsIndex( Graphics_ModeIndex ) ;
SetVideoGraphicsData( inb( 0x3CF ) & ~0x8 ) ; /* Clear the bit */
SetVideoGraphicsIndex( Read_Map_SelectIndex ) ;

skip = BIT_OFFSET( x ) ;
pad = RowIncrement - lx ;
ignore = BIT_OFFSET( x + lx ) ;
masterSrc = (volatile unsigned char *) SCREENADDRESS( x, y ) ;
center_width = ROW_OFFSET( x + lx ) - ROW_OFFSET( ( x + 0x7 ) & ~0x7 ) ;

#define SINGLE_STEP 	*data++ = tmp & PLI_ALLPLANES ; tmp >>= 4

if ( center_width < 0 )
	for ( src = masterSrc ;
	      ly-- ;
	      src += BYTES_PER_ROW ) {
		tmp = read8Z( src ) >> ( skip << 2 ) ;
		for ( dx = lx + 1 ; --dx ; ) {
			SINGLE_STEP ;
		}
		data += pad ;
	}
else
	for ( savCenterWidth = center_width ;
	      ly-- ;
	      center_width = savCenterWidth,
	      masterSrc += BYTES_PER_ROW ) {
		src = masterSrc ;
		tmp = read8Z( src++ ) ;
		if ( dx = skip )
			tmp >>= ( dx << 2 ) ;
		else
			--center_width ;
	BranchPoint:
		switch ( dx ) {
		LoopTop:
		case 0x0: SINGLE_STEP ;
		case 0x1: SINGLE_STEP ;
		case 0x2: SINGLE_STEP ;
		case 0x3: SINGLE_STEP ;
		case 0x4: SINGLE_STEP ;
		case 0x5: SINGLE_STEP ;
		case 0x6: SINGLE_STEP ;
		case 0x7: *data++ = tmp & PLI_ALLPLANES ;

			/* Fall Through To End Of Inner Loop */
			if ( center_width > 0 ) {
				tmp = read8Z( src++ ) ;
				center_width-- ;
				goto LoopTop ;
			}
			else if ( ( center_width == 0 )
			       && ( dx = ( - ignore ) & 07 ) ) {
				tmp = read8Z( src++ ) ;
				center_width-- ;
				goto BranchPoint ; /* Do Mod 8 edge */
			}
			else /* End of this line */
				data += pad ;
		}
	}

if ( cursor_saved )
	pliReplaceCursor() ;

return ;
}
