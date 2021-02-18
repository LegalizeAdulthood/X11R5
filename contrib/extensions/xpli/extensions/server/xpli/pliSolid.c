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

/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliSolid.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliSolid.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */

#ifndef lint
static char *rcsid = "$Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliSolid.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $" ;
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

/* Declared in "plicurs.c" */
extern int pliCursorSemaphore ;
extern int pliCheckCursor() ;
extern void pliReplaceCursor() ;

static void fastFill( destination, bytewidth, height )
register volatile unsigned char *destination ;
register const unsigned int bytewidth ;	/* MUST BE > 0 !! */
register unsigned int height ;		/* MUST BE > 0 !! */
{
register volatile unsigned char *stop_address = destination + bytewidth ;
register int row_jump = BYTES_PER_ROW - bytewidth ;
#if !defined(OLDHC) && defined(BSDrt) && !defined(i386)
register const int notZero = ~0x0 ;
#else
#define notZero ( ~0 )
#endif

#define SINGLE_STORE \
    ( *( (PliMemoryPtr) ( destination++ ) ) = notZero )

/* TOP OF FIRST LOOP */
BranchPoint:

switch ( bytewidth & 0xF ) { /* Jump into loop at mod 16 remainder */
	LoopTop :
	case 0x0 : SINGLE_STORE ;
	case 0xF : SINGLE_STORE ;
	case 0xE : SINGLE_STORE ;
	case 0xD : SINGLE_STORE ;
	case 0xC : SINGLE_STORE ;
	case 0xB : SINGLE_STORE ;
	case 0xA : SINGLE_STORE ;
	case 0x9 : SINGLE_STORE ;
	case 0x8 : SINGLE_STORE ;
	case 0x7 : SINGLE_STORE ;
	case 0x6 : SINGLE_STORE ;
	case 0x5 : SINGLE_STORE ;
	case 0x4 : SINGLE_STORE ;
	case 0x3 : SINGLE_STORE ;
	case 0x2 : SINGLE_STORE ;
	case 0x1 : SINGLE_STORE ;
/* FIRST LOOP */
		if ( destination != stop_address )
			goto LoopTop ;
/* SECOND LOOP */
		if ( --height ) {
			destination += row_jump ;
			stop_address += BYTES_PER_ROW ;
			goto BranchPoint ;
		}
		else
			return ;
#undef SINGLE_STORE
}
/*NOTREACHED*/
}

/* For Read-Modify-Write Case */
static void fastFillRMW( destination, bytewidth, height )
register volatile unsigned char *destination ;
register const unsigned int bytewidth ;	/* MUST BE > 0 !! */
register unsigned int height ;		/* MUST BE > 0 !! */
{
register volatile unsigned char *stop_address = destination + bytewidth ;
register int row_jump = BYTES_PER_ROW - bytewidth ;
#if !defined(OLDHC) && defined(BSDrt) && !defined(i386)
register const int notZero = ~0x0 ;
#else
#define notZero ( ~0 )
#endif
register int tmp ;

#define SINGLE_STORE \
    tmp = *( (PliMemoryPtr) destination ) ; \
    ( *( (PliMemoryPtr) ( destination++ ) ) = notZero )

/* TOP OF FIRST LOOP */
BranchPoint:

switch ( bytewidth & 0xF ) { /* Jump into loop at mod 16 remainder */
	LoopTop :
	case 0x0 : SINGLE_STORE ;
	case 0xF : SINGLE_STORE ;
	case 0xE : SINGLE_STORE ;
	case 0xD : SINGLE_STORE ;
	case 0xC : SINGLE_STORE ;
	case 0xB : SINGLE_STORE ;
	case 0xA : SINGLE_STORE ;
	case 0x9 : SINGLE_STORE ;
	case 0x8 : SINGLE_STORE ;
	case 0x7 : SINGLE_STORE ;
	case 0x6 : SINGLE_STORE ;
	case 0x5 : SINGLE_STORE ;
	case 0x4 : SINGLE_STORE ;
	case 0x3 : SINGLE_STORE ;
	case 0x2 : SINGLE_STORE ;
	case 0x1 : SINGLE_STORE ;
/* FIRST LOOP */
		if ( destination != stop_address )
			goto LoopTop ;
/* SECOND LOOP */
		if ( --height ) {
			destination += row_jump ;
			stop_address += BYTES_PER_ROW ;
			goto BranchPoint ;
		}
		else
			return ;
}
#undef SINGLE_STORE
/*NOTREACHED*/
}

void pliFillSolid( color, alu, planes, x0, y0, lx, ly )
unsigned long int color ;
const int alu ;
unsigned long int planes ;
register int x0 ;
register const int y0 ;
register int lx ;
register const int ly ;		/* MUST BE > 0 !! */
{
register volatile unsigned char *dst ;
register tmp ;
register tmp2 ;
register tmp3 ;
unsigned int data_rotate_value = PLI_COPY_MODE ;
unsigned int read_write_modify = FALSE ;
unsigned int invert_existing_data = FALSE ;
int cursor_saved ;

if ( ( lx == 0 ) || ( ly == 0 ) )
	return;

switch ( alu ) {
	case GXclear:		/* 0x0 Zero 0 */
		color = 0 ;
		break ;
	case GXnor:		/* 0x8 NOT src AND NOT dst */
		invert_existing_data = TRUE ;
	case GXandInverted:	/* 0x4 NOT src AND dst */
		color = ~color ;
	case GXand:		/* 0x1 src AND dst */
		data_rotate_value = PLI_AND_MODE ;
		read_write_modify = TRUE ;
	case GXcopy:		/* 0x3 src */
		break ;
	case GXnoop:		/* 0x5 dst */
		return ;
	case GXequiv:		/* 0x9 NOT src XOR dst */
		color = ~color ;
	case GXxor:		/* 0x6 src XOR dst */
		data_rotate_value = PLI_XOR_MODE ;
		read_write_modify = TRUE ;
		planes &= color ;
		break ;
	case GXandReverse:	/* 0x2 src AND NOT dst */
		invert_existing_data = TRUE ;
		data_rotate_value = PLI_AND_MODE ;
		read_write_modify = TRUE ;
		break ;
	case GXorReverse:	/* 0xb src OR NOT dst */
		invert_existing_data = TRUE ;
		data_rotate_value = PLI_OR_MODE ;
		read_write_modify = TRUE ;
		break ;
	case GXnand:		/* 0xe NOT src OR NOT dst */
		invert_existing_data = TRUE ;
	case GXorInverted:	/* 0xd NOT src OR dst */
		color = ~color ;
	case GXor:		/* 0x7 src OR dst */
		data_rotate_value = PLI_OR_MODE ;
		read_write_modify = TRUE ;
		break ;
	case GXcopyInverted:	/* 0xc NOT src */
		color = ~color ;
		break ;
	case GXinvert:		/* 0xa NOT dst */
		data_rotate_value = PLI_XOR_MODE ;
		read_write_modify = TRUE ;
	case GXset:		/* 0xf 1 */
		color = PLI_ALLPLANES ;
	default:
		break ;
}

if ( !( planes &= PLI_ALLPLANES ) )
	return ;

/* Remove Cursor If In The Way */
cursor_saved = !pliCursorSemaphore && pliCheckCursor( x0, y0, lx, ly ) ;

/*
 * Set The Plane-Enable
 */
SetVideoSequencer( Mask_MapIndex, planes ) ;
SetVideoGraphics( Enb_Set_ResetIndex, planes ) ;
/*
 * Put Display Into SET/RESET Write Mode
 */
SetVideoGraphics( Graphics_ModeIndex, PLI_WRITE_MODE_3 ) ;
/*
 * Set The Color in The Set/Reset Register
 */
SetVideoGraphics( Set_ResetIndex, color & PLI_ALLPLANES ) ;
/*
 * Set The Function-Select In The Data Rotate Register
 */
SetVideoGraphics( Data_RotateIndex, data_rotate_value ) ;

/* Point At The Bit Mask Reg */
SetVideoGraphicsIndex( Bit_MaskIndex ) ;
/* Do Left Edge */
if ( tmp = x0 & 07 ) {
	tmp2 = SCRRIGHT8( ( (unsigned) 0xFF ), tmp ) ;
	/* Catch The Cases Where The Entire Region Is Within One Byte */
	if ( ( lx -= 8 - tmp ) < 0 ) {
		tmp2 &= SCRLEFT8( 0xFF, -lx ) ;
		lx = 0 ;
	}
	/* Set The Bit Mask Reg */
	outb( 0x3CF, tmp2 ) ;
	if ( invert_existing_data == TRUE ) {
		SetVideoGraphics( Data_RotateIndex, PLI_XOR_MODE ) ;
		for ( tmp = ly, dst = PLIBASE + BYTE_OFFSET( x0, y0 ) ;
		      tmp-- ;
		      dst += BYTES_PER_ROW ) {
			tmp3 = *( (PliMemoryPtr) dst ) ;
			*( (PliMemoryPtr) dst ) = tmp2 ;
		}
		SetVideoGraphicsData( data_rotate_value ) ; /* Un-Set XOR */
		SetVideoGraphicsIndex( Bit_MaskIndex ) ;
	}
	for ( tmp = ly, dst = SCREENADDRESS( x0, y0 ) ;
	      tmp-- ;
	      dst += BYTES_PER_ROW ) {
		tmp3 = *( (PliMemoryPtr) dst ) ;
		*( (PliMemoryPtr) dst ) = tmp2 ;
	}
	if ( !lx ) { /* All Handled In This Byte */
		if ( cursor_saved )
			pliReplaceCursor() ;
		return ;
	}
	x0 = ( x0 + 8 ) & ~07 ;
}

/* Fill The Center Of The Box */
if ( ROW_OFFSET( lx ) ) {
	SetVideoGraphicsData( 0xFF ) ;
	if ( invert_existing_data == TRUE ) {
		SetVideoGraphics( Data_RotateIndex, PLI_XOR_MODE ) ;
		fastFillRMW( PLIBASE + BYTE_OFFSET( x0, y0 ),
			     ROW_OFFSET( lx ), ly ) ;
		SetVideoGraphicsData( data_rotate_value ) ; /* Un-Set XOR */
		/* Point At The Bit Mask Reg */
		SetVideoGraphicsIndex( Bit_MaskIndex ) ;
	}
	(* ( ( read_write_modify == FALSE ) ? fastFill : fastFillRMW ) )
		( SCREENADDRESS( x0, y0 ), ROW_OFFSET( lx ), ly ) ;
}

/* Do Right Edge */
if ( tmp = BIT_OFFSET( lx ) ) { /* x0 Now Is Byte Aligned */
	/* Set The Bit Mask */
	SetVideoGraphicsData( tmp2 = SCRLEFT8( 0xFF, ( 8 - tmp ) ) ) ;
	if ( invert_existing_data == TRUE ) {
		SetVideoGraphics( Data_RotateIndex, PLI_XOR_MODE ) ;
		for ( tmp = ly, dst = PLIBASE + BYTE_OFFSET( ( x0 + lx ), y0 ) ;
		      tmp-- ;
		      dst += BYTES_PER_ROW ) {
			tmp3 = *( (PliMemoryPtr) dst ) ;
			*( (PliMemoryPtr) dst ) = tmp2 ;
		}
		SetVideoGraphicsData( data_rotate_value ) ; /* Un-Set XOR */
	}
	for ( tmp = ly, dst = SCREENADDRESS( ( x0 + lx ), y0 ) ;
	      tmp-- ;
	      dst += BYTES_PER_ROW ) {
		tmp3 = *( (PliMemoryPtr) dst ) ;
		*( (PliMemoryPtr) dst ) = tmp2 ;
	}
}
/* Disable Set/Reset Register */
SetVideoGraphics( Enb_Set_ResetIndex, 0 ) ;

if ( cursor_saved )
	pliReplaceCursor() ;

return ;
}
