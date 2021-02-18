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
/***********************************************************
		Copyright IBM Corporation 1987,1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliLine.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliLine.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */

#ifndef lint
static char *rcsid = "$Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliLine.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $" ;
#endif


/*
 *  Hardware interface routines for IBM VGA adapter for
 *  X.11 server(s) on IBM equipment.
 *
 */
#include "newmacros.h"
#include "X.h"

#include "OScompiler.h"

#include "pliVideo.h"

/*
#include "ibmIOArch.h"
*/

#include "pliReg.h"

/* Bit Ordering Macros */
/* The existing macros ( from the ".h"'s ) use casts ( to unsigned char )
 * and our compilers generate useless code ( for the casts ).
 */
#if defined(SCRLEFT8)
#undef SCRLEFT8
#endif
#if (BITMAP_BIT_ORDER == MSBFirst)	/* pc/rt */
#define SCRLEFT8(lw, n)	( ((lw) << (n)) )
#else
#if (BITMAP_BIT_ORDER == LSBFirst)	/* intel */
#define SCRLEFT8(lw, n)	( ((lw) >> (n)) )
#else
	******** ERROR ********
#endif
#endif
#if defined(SCRRIGHT8)
#undef SCRRIGHT8
#endif
#if BITMAP_BIT_ORDER == MSBFirst	/* pc/rt */
#define SCRRIGHT8(lw, n)	( ((lw) >> (n)) )
#else
#if BITMAP_BIT_ORDER == LSBFirst
#define SCRRIGHT8(lw, n)	( ((lw) << (n)) )
#else
	******** ERROR ********
#endif
#endif

extern void pliFillSolid( ) ;

void pliHorzLine( color, alu, mask, x, y, w )
unsigned long int color ;
int alu ;
unsigned long int mask ;
int x, y, w ;
{
pliFillSolid( color, alu, mask, x, y, w, 1 ) ;
return ;
}

void pliVertLine( color, alu, mask, x, y, h )
unsigned long int color ;
int alu ;
unsigned long int mask ;
int x, y, h ;
{
pliFillSolid( color, alu, mask, x, y, 1, h ) ;
return ;
}

#if defined(USE_PORTABLE_FAST_LINES)

/* X increasing */
static void fast_x_line_right( et, e1, e2, len, y_increment, addr, x_offset )
register int et ;
const int e1 ;
const int e2 ;
unsigned int len ;
const int y_increment ;
register unsigned volatile char *addr ;
const int x_offset ;
{
#if defined(ATRIO)
register unsigned long int tmp ;
register unsigned long int mask ;
register unsigned long int single_bit ;
#else
register unsigned char tmp ;
register unsigned char mask ;
register unsigned char single_bit ;
#endif

#define SINGLE_STEP \
if ( et < 0 ) { \
	mask |= single_bit ; \
	et += e1 ; \
} \
else { \
	tmp = *( (PliMemoryPtr) addr ) ; \
	et += e2 ; \
	*( (PliMemoryPtr) addr ) = mask ; \
	mask = single_bit ; \
	addr += y_increment ; \
}

/* Set-Up And Set Initial Mask Value */
mask = single_bit = SCRRIGHT8( LeftmostBit, tmp = x_offset ) ;
tmp = ( tmp + 1 ) & 7 ;
if ( !( single_bit = SCRRIGHT8( single_bit, 1 ) ) ) {
	tmp = *( (PliMemoryPtr) addr ) ;
	single_bit = LeftmostBit ;
	*( (PliMemoryPtr) addr++ ) = mask ;
	tmp = mask = 0 ;
}
if ( ( 8 - tmp ) > --len ) {
	tmp = len ;
	len = 8 ;
}
else if ( tmp ) {
	len += tmp ;	/* Round-Up */
	tmp = 8 - tmp ;
}

branchPoint:
switch ( tmp ) {
	Loop_Top:
	/* */
	/* First -- Leftmost -- Most-Significant Bit In Byte */
	case 0 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
	case 7 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
	case 6 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
	case 5 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
	case 4 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
	case 3 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
	case 2 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
	/* Last -- Rightmost -- Least-Significant Bit In Byte */
	case 1 : SINGLE_STEP ;
	/* Flush buffered mask */
	tmp = *( (PliMemoryPtr) addr ) ;
	len -= 8 ;
	*( (PliMemoryPtr) addr ) = mask ;
	if ( len > 7 ) {
		mask = 0 ;
		addr++ ;
		single_bit = LeftmostBit ;
		goto Loop_Top ;
	}
	else if ( len ) {
		tmp = len ;
		mask = 0 ;
		addr++ ;
		len = 8 ;
		single_bit = LeftmostBit ;
		goto branchPoint ;
	}
	else
		return ;
}
#undef SINGLE_STEP
/*NOTREACHED*/
}

/* X decreasing */
static void fast_x_line_left( et, e1, e2, len, y_increment, addr, x_offset )
register int et ;
const int e1 ;
const int e2 ;
unsigned int len ;
const int y_increment ;
register unsigned volatile char *addr ;
const int x_offset ;
{
#if defined(ATRIO)
register unsigned long int tmp ;
register unsigned long int mask ;
register unsigned long int single_bit ;
#else
register unsigned char tmp ;
register unsigned char mask ;
register unsigned char single_bit ;
#endif

#define SINGLE_STEP \
if ( et < 0 ) { \
	mask |= single_bit ; \
	et += e1 ; \
} \
else { \
	tmp = *( (PliMemoryPtr) addr ) ; \
	et += e2 ; \
	*( (PliMemoryPtr) addr ) = mask ; \
	mask = single_bit ; \
	addr += y_increment ; \
}

/* Set-Up And Set Initial Mask Value */
mask = single_bit = SCRLEFT8( RightmostBit, ( tmp = 7 - x_offset ) ) ;
tmp = ( tmp + 1 ) & 7 ;
if ( ( single_bit = SCRLEFT8( single_bit, 1 ) ) == SCRLEFT8( RightmostBit, 8 ) ) {
	tmp = *( (PliMemoryPtr) addr ) ;
	single_bit = RightmostBit ;
	*( (PliMemoryPtr) addr-- ) = mask ;
	tmp = mask = 0 ;
}
if ( ( 8 - tmp ) > --len ) {
	tmp = len ;
	len = 8 ;
}
else if ( tmp ) {
	len += tmp ;	/* Round-Up */
	tmp = 8 - tmp ;
}

branchPoint:
switch ( tmp ) {
	Loop_Top:
	/* */
	/* First -- Rightmost -- Least-Significant Bit In Byte */
	case 0 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
	case 7 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
	case 6 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
	case 5 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
	case 4 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
	case 3 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
	case 2 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
	/* Last -- Leftmost -- Most-Significant Bit In Byte */
	case 1 : SINGLE_STEP ;
	/* Flush buffered mask */
	tmp = *( (PliMemoryPtr) addr ) ;
	len -= 8 ;
	*( (PliMemoryPtr) addr ) = mask ;
	if ( len > 7 ) {
		mask = 0 ;
		addr-- ;
		single_bit = RightmostBit ;
		goto Loop_Top ;
	}
	else if ( len ) {
		tmp = len ;
		mask = 0 ;
		addr-- ;
		len = 8 ;
		single_bit = RightmostBit ;
		goto branchPoint ;
	}
	else
		return ;
}
#undef SINGLE_STEP
/*NOTREACHED*/
}

/* X increasing */
static void fast_y_line_right( et, e1, e2, len, y_increment, addr, x_offset )
register int et ;
const int e1 ;
const int e2 ;
unsigned int len ;
const int y_increment ;
register unsigned volatile char *addr ;
const int x_offset ;
{
#if defined(ATRIO)
register unsigned long int tmp ;
register unsigned long int single_bit ;
#else
register unsigned char tmp ;
register unsigned char single_bit ;
#endif

#define SINGLE_STEP \
addr += y_increment ; \
if ( et < 0 ) { \
	tmp = *( (PliMemoryPtr) addr ) ; \
	et += e1 ; \
} \
else { \
	et += e2 ; \
	if ( single_bit = SCRRIGHT8( single_bit, 1 ) ) { \
		tmp = *( (PliMemoryPtr) addr ) ; \
	} \
	else { \
		tmp = *( (PliMemoryPtr) ( ++addr ) ) ; \
		single_bit = LeftmostBit ; \
	} \
} \
*( (PliMemoryPtr) addr ) = single_bit
/* END MACRO */

/* Draw First Dot */
tmp = *( (PliMemoryPtr) addr ) ;
*( (PliMemoryPtr) addr ) =
	( single_bit = SCRRIGHT8( LeftmostBit, x_offset ) ) ;

if ( tmp = --len & 0xF ) /* Already Drew First Dot */
	len += 0xF ;
len >>= 0x4 ;

switch ( tmp ) {
	Loop_Top:
	/* */
	case 0x0 : SINGLE_STEP ;
	case 0xF : SINGLE_STEP ;
	case 0xE : SINGLE_STEP ;
	case 0xD : SINGLE_STEP ;
	case 0xC : SINGLE_STEP ;
	case 0xB : SINGLE_STEP ;
	case 0xA : SINGLE_STEP ;
	case 0x9 : SINGLE_STEP ;
	case 0x8 : SINGLE_STEP ;
	case 0x7 : SINGLE_STEP ;
	case 0x6 : SINGLE_STEP ;
	case 0x5 : SINGLE_STEP ;
	case 0x4 : SINGLE_STEP ;
	case 0x3 : SINGLE_STEP ;
	case 0x2 : SINGLE_STEP ;
	case 0x1 : SINGLE_STEP ;
	/* */
	if ( --len )
		goto Loop_Top ;
	else
		return ;
}
#undef SINGLE_STEP

/*NOTREACHED*/
}

/* X decreasing */
static void fast_y_line_left( et, e1, e2, len, y_increment, addr, x_offset )
register int et ;
const int e1 ;
const int e2 ;
unsigned int len ;
const int y_increment ;
register unsigned volatile char *addr ;
const int x_offset ;
{
#if defined(ATRIO)
#if ( !defined(OLDHC) && defined(BSDrt) ) && !defined(i386)
register const unsigned stop_bit = SCRLEFT8( RightmostBit, 8 ) ;
#else
#define stop_bit ( (unsigned) SCRLEFT8( RightmostBit, 8 ) )
#endif
register unsigned long int single_bit ;
register unsigned long int tmp ;
#else /* Not ATRIO */
#if ( !defined(OLDHC) && defined(BSDrt) ) && !defined(i386)
register const unsigned char stop_bit = SCRLEFT8( RightmostBit, 8 ) ;
#else
#define stop_bit ( (unsigned char) SCRLEFT8( RightmostBit, 8 ) )
#endif
register unsigned char single_bit ;
register unsigned char tmp ;
#endif /* ATRIO */

#define SINGLE_STEP \
addr += y_increment ; \
if ( et < 0 ) { \
	tmp = *( (PliMemoryPtr) addr ) ; \
	et += e1 ; \
} \
else { \
	et += e2 ; \
	if ( ( single_bit = SCRLEFT8( single_bit, 1 ) ) != stop_bit ) { \
		tmp = *( (PliMemoryPtr) addr ) ; \
	} \
	else { \
		tmp = *( (PliMemoryPtr) ( --addr ) ) ; \
		single_bit = RightmostBit ; \
	} \
} \
*( (PliMemoryPtr) addr ) = single_bit
/* END MACRO */

/* Draw First Dot */
tmp = *( (PliMemoryPtr) addr ) ;
*( (PliMemoryPtr) addr ) =
	( single_bit = SCRRIGHT8( LeftmostBit, x_offset ) ) ;

if ( tmp = --len & 0xF ) /* Already Drew First Dot */
	len += 0xF ;
len >>= 0x4 ;

switch ( tmp ) {
	Loop_Top:
	/* */
	case 0x0 : SINGLE_STEP ;
	case 0xF : SINGLE_STEP ;
	case 0xE : SINGLE_STEP ;
	case 0xD : SINGLE_STEP ;
	case 0xC : SINGLE_STEP ;
	case 0xB : SINGLE_STEP ;
	case 0xA : SINGLE_STEP ;
	case 0x9 : SINGLE_STEP ;
	case 0x8 : SINGLE_STEP ;
	case 0x7 : SINGLE_STEP ;
	case 0x6 : SINGLE_STEP ;
	case 0x5 : SINGLE_STEP ;
	case 0x4 : SINGLE_STEP ;
	case 0x3 : SINGLE_STEP ;
	case 0x2 : SINGLE_STEP ;
	case 0x1 : SINGLE_STEP ;
	/* */
	if ( --len )
		goto Loop_Top ;
	else
		return ;
}
#undef SINGLE_STEP

/*NOTREACHED*/
}
#else
extern void fast_x_line_right() ;
extern void fast_x_line_left() ;
extern void fast_y_line_right() ;
extern void fast_y_line_left() ;
#endif /* USE_PORTABLE_FAST_LINES */

#define X_AXIS 0
#define Y_AXIS 1

void pliBresLine( color, alu, planes, signdx, signdy,
		  axis, x, y, et, e1, e2, len )
unsigned long int color ;
int alu ;
unsigned long int planes ;
int signdx, signdy ;
int axis, x, y ;
int et, e1, e2 ;
unsigned long int len ;
{
unsigned int invert_existing_data = 0 ;
unsigned int data_rotate_value = PLI_COPY_MODE ;
int cursor_saved ;

extern int pliCursorSemaphore ;

if ( !( planes &= PLI_ALLPLANES ) )
	return ;
else if ( len == 1 ) {
	pliFillSolid( color, alu, planes, x, y, 1, 1 ) ;
	return ;
}

switch ( alu ) {
	case GXclear:		/* 0x0 Zero 0 */
		color = 0 ;
		break ;
	case GXnor:		/* 0x8 NOT src AND NOT dst */
		invert_existing_data = 1 ;
	case GXandInverted:	/* 0x4 NOT src AND dst */
		color = ~color ;
	case GXand:		/* 0x1 src AND dst */
		data_rotate_value = PLI_AND_MODE ;
	case GXcopy:		/* 0x3 src */
		break ;
	case GXnoop:		/* 0x5 dst */
		return ;
	case GXequiv:		/* 0x9 NOT src XOR dst */
		color = ~color ;
	case GXxor:		/* 0x6 src XOR dst */
		data_rotate_value = PLI_XOR_MODE ;
		break ;
	case GXandReverse:	/* 0x2 src AND NOT dst */
		invert_existing_data = 1 ;
		data_rotate_value = PLI_AND_MODE ;
		break ;
	case GXorReverse:	/* 0xb src OR NOT dst */
		invert_existing_data = 1 ;
		data_rotate_value = PLI_OR_MODE ;
		break ;
	case GXnand:		/* 0xe NOT src OR NOT dst */
		invert_existing_data = 1 ;
	case GXorInverted:	/* 0xd NOT src OR dst */
		color = ~color ;
	case GXor:		/* 0x7 src OR dst */
		data_rotate_value = PLI_OR_MODE ;
		break ;
	case GXcopyInverted:	/* 0xc NOT src */
		color = ~color ;
		break ;
	case GXinvert:		/* 0xa NOT dst */
		data_rotate_value = PLI_XOR_MODE ;
	case GXset:		/* 0xf 1 */
		color = PLI_ALLPLANES ;
	default:
		break ;

}
/* Remove Cursor If In The Way */
cursor_saved = !pliCursorSemaphore
	    && pliCheckCursor( ( ( signdx == -1 ) ? ( x - len ) : x ),
			       ( ( signdy == -1 ) ? ( y - len ) : y ),
			       len, len ) ;
++pliCursorSemaphore ;

if ( invert_existing_data ) /* Recurse */
	pliBresLine( PLI_ALLPLANES, GXxor, planes, signdx, signdy,
		     axis, x, y, et, e1, e2, len ) ;

/* Set-Up pli registers */
/*
 * Put Display Into SET-AND (i.e. Write Mode 3 )
 */
SetVideoGraphics( Graphics_ModeIndex, PLI_WRITE_MODE_3 ) ;
/*
 * Set The Color in The Set/Reset Register
 */
SetVideoGraphics( Set_ResetIndex, color & PLI_ALLPLANES ) ;
/*
 * Set The Plane-Enable
 */
SetVideoSequencer( Mask_MapIndex, planes ) ;
/*
 * Set The Pli's Alu Function
 */
SetVideoGraphics( Data_RotateIndex, data_rotate_value ) ;

/* Point At The Bit Mask Reg */
SetVideoGraphics( Bit_MaskIndex, 0xFF ) ;

/* Do It Here */
/* ********** */
(* ( ( signdx > 0 ) ? ( axis ? fast_y_line_right : fast_x_line_right )
		    : ( axis ? fast_y_line_left  : fast_x_line_left ) ) )
			( et, e1, e2, len,
			  ( signdy > 0 ? BYTES_PER_ROW : - BYTES_PER_ROW ),
			  SCREENADDRESS( x, y ), BIT_OFFSET( x ) ) ;

/* ********** */
if ( !--pliCursorSemaphore && cursor_saved )
	pliReplaceCursor() ;

return ;
}
