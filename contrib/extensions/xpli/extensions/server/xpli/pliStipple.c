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

/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliStipple.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliStipple.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */

#ifndef lint
static char *rcsid = "$Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliStipple.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $" ;
#endif


#include "newmacros.h"
#include "X.h"
#include "pixmapstr.h"

#include "OScompiler.h"

/*
#include "ibmIOArch.h"
*/

#include "pliVideo.h"

#include "pliReg.h"

static unsigned char
getbits( x, patternWidth, lineptr )
register const int x ;
register const unsigned int patternWidth ;
register const unsigned char * const lineptr ;
{
register unsigned char bits ;
register const unsigned char *cptr ;
register shift ;
register wrap ;

cptr = lineptr + ( x >> 3 ) ;
bits = *cptr ;
if ( shift = x & 7 )
	bits = SCRLEFT8( bits, shift ) | SCRRIGHT8( cptr[1], ( 8 - shift ) ) ;
if ( ( wrap = x + 8 - patternWidth ) > 0 ) {
	bits &= SCRLEFT8( 0xFF, wrap ) ;
	bits |= SCRRIGHT8( *lineptr, ( 8 - wrap ) ) ;
}

return bits ;
}

extern void pliFillSolid() ;
/* Declared in "plicurs.c" */
extern int pliCursorSemaphore ;
extern int pliCheckCursor() ;
extern void pliReplaceCursor() ;

static void
DoMonoSingle( w, x, y, mastersrc, h, width, paddedByteWidth, height,
	      xshift, yshift )
int w, x, y ;
register const unsigned char *mastersrc ;
int h ;
register unsigned int width ;
register unsigned int paddedByteWidth ;
unsigned int height ;
int xshift ;
int yshift ;
{
register volatile unsigned char *xDst ;
register VideoAdapterObject tmp2 ;
register int NeedValX ;
register int counter ;
register int tmp1 ;
unsigned int rowCounter ;
int byte_cnt ;

/* Do Left Edge */
if ( tmp1 = x & 07 ) {
	tmp2 = SCRRIGHT8( ( (unsigned) 0xFF ), tmp1 ) ;
	/* Catch The Cases Where The Entire Region Is Within One Byte */
	if ( ( w -= 8 - tmp1 ) < 0 ) {
		tmp2 &= SCRLEFT8( 0xFF, -w ) ;
		w = 0 ;
	}
	SetVideoGraphicsData( tmp2 ) ; /* Set The Bit Mask */
	if ( ( NeedValX = xshift - tmp1 ) < 0 )
		NeedValX += width ;
	/*
	 * For Each Line In The Source Pixmap
	 */
	for ( tmp1 = yshift, rowCounter = h, xDst = SCREENADDRESS( x, y ) ;
	      rowCounter-- ;
	      tmp1++, xDst += BYTES_PER_ROW ) {

		if ( tmp1 >= height )
			tmp1 -= height ;
		/* Read To Save */
		tmp2 = *( (PliMemoryPtr) xDst) ;
		/* Write Pattern */
		*( (PliMemoryPtr) xDst ) =
			getbits( NeedValX, width,
				 mastersrc
				 + ( tmp1 * paddedByteWidth ) ) ;
	}
	x = ( x + 7 ) & ~07 ;
	if ( ( NeedValX += 8 ) >= width )
		NeedValX -= width ;
}
else {
	NeedValX = xshift ;
}

if ( byte_cnt = ROW_OFFSET( w ) ) { /* Fill The Center Of The Box */
	int SavNeedX = NeedValX ;

	SetVideoGraphicsData( 0xFF ) ; /* Set The Bit Mask */
	/*
	 * For Each Line In The Source Pixmap
	 */
	for ( tmp1 = yshift, rowCounter = h, xDst = SCREENADDRESS( x, y ) ;
	      rowCounter-- ;
	      tmp1++, xDst += BYTES_PER_ROW - byte_cnt ) {
		register const unsigned char *l_ptr ;
		if ( tmp1 >= height )
			tmp1 -= height ;
		l_ptr = mastersrc + ( tmp1 * paddedByteWidth ) ;
		/*
		 * For Each Byte Across The Pattern In X
		 */
		for ( counter = byte_cnt, NeedValX = SavNeedX ;
		      counter-- ; xDst++ ) {
			/* Read To Save */
			tmp2 = *( (PliMemoryPtr) xDst) ;
			/* Write Pattern */
			*( (PliMemoryPtr) xDst ) =
				getbits( NeedValX, width, l_ptr ) ;
			if ( ( NeedValX += 8 ) >= width )
				NeedValX -= width ;
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
	for ( tmp1 = yshift, rowCounter = h,
	      xDst = SCREENADDRESS( ( x + w ), y ) ;
	      rowCounter-- ;
	      tmp1++, xDst += BYTES_PER_ROW ) {
		if ( tmp1 >= height )
			tmp1 -= height ;
		/* Read To Save */
		tmp2 = *( (PliMemoryPtr) xDst) ;
		/* Write Pattern */
		*( (PliMemoryPtr) xDst ) =
			getbits( NeedValX, width,
				 mastersrc
				 + ( tmp1 * paddedByteWidth ) ) ;
	}
}

return ;
}

static void
DoMonoMany( w, x, y, mastersrc, h, width, paddedByteWidth, height,
	    xshift, yshift )
int w, x, y ;
register const unsigned char *mastersrc ;
int h ;
register unsigned int width ;
register unsigned int paddedByteWidth ;
unsigned int height ;
int xshift ;
int yshift ;
{
register volatile unsigned char *xDst ;
register VideoAdapterObject tmp2 ;
register int NeedValX ;
register int byte_cnt ;
register int tmp1 ;
unsigned DestinationRow ;
unsigned int SourceRow ;
volatile unsigned char *dst ;
int scr_incr = ( height * BYTES_PER_ROW ) ;

/* Do Left Edge */
if ( tmp1 = x & 07 ) {
	tmp2 = SCRRIGHT8( ( (unsigned) 0xFF ), tmp1 ) ;
	/* Catch The Cases Where The Entire Region Is Within One Byte */
	if ( ( w -= 8 - tmp1 ) < 0 ) {
		tmp2 &= SCRLEFT8( 0xFF, -w ) ;
		w = 0 ;
	}
	SetVideoGraphicsData( tmp2 ) ; /* Set The Bit Mask */
	if ( ( NeedValX = xshift - tmp1 ) < 0 )
		NeedValX += width ;
	/*
	 * For Each Line In The Source Pixmap
	 */
	for ( tmp1 = yshift, SourceRow = 0, dst = SCREENADDRESS( x, y ) ;
	      SourceRow < height ;
	      tmp1++, SourceRow++, dst += BYTES_PER_ROW ) {
		register unsigned bitPattern ;

		if ( tmp1 >= height )
			tmp1 -= height ;
		/*
		 * For Each Time Pattern Repeats In The Y Dimension
		 */
		for ( xDst = dst, DestinationRow = SourceRow,
		      bitPattern = getbits( NeedValX, width,
					    mastersrc
					+ ( tmp1 * paddedByteWidth ) ) ;
		      DestinationRow < h ;
		      xDst += scr_incr,
		      DestinationRow += height ) {
			/* Read To Save */
			tmp2 = *( (PliMemoryPtr) xDst ) ;
			/* Write Pattern */
			*( (PliMemoryPtr) xDst ) = bitPattern ;
		}
	}
	x = ( x + 7 ) & ~07 ;
	if ( ( NeedValX += 8 ) >= width )
		NeedValX -= width ;
}
else {
	NeedValX = xshift ;
}

if ( byte_cnt = ROW_OFFSET( w ) ) { /* Fill The Center Of The Box */
	int SavNeedX = NeedValX ;

	SetVideoGraphicsData( 0xFF ) ; /* Set The Bit Mask */
	/*
	 * For Each Line In The Source Pixmap
	 */
	for ( tmp1 = yshift, SourceRow = 0, dst = SCREENADDRESS( x, y ) ;
	      SourceRow < height ;
	      tmp1++, SourceRow++, dst += BYTES_PER_ROW - byte_cnt ) {
		register const unsigned char *l_ptr ;
		if ( tmp1 >= height )
			tmp1 -= height ;
		l_ptr = mastersrc + ( tmp1 * paddedByteWidth ) ;
		/*
		 * For Each Byte Across The Pattern In X
		 */
		for ( tmp2 = byte_cnt, NeedValX = SavNeedX ;
		      tmp2-- ;
		      dst++ ) {
			register unsigned bitPattern ;
			register VideoAdapterObject tmp3 ;
			/*
			 * For Each Time Pattern Repeats In Y
			 */
			for ( xDst = dst,
			      DestinationRow = SourceRow,
			      bitPattern = getbits( NeedValX, width, l_ptr ) ;
			      DestinationRow < h ;
			      xDst += scr_incr,
			      DestinationRow += height ) {
				/* Read To Save */
				tmp3 = *( (PliMemoryPtr) xDst) ;
				/* Write Pattern */
				*( (PliMemoryPtr) xDst ) = bitPattern ;
			}
			if ( ( NeedValX += 8 ) >= width )
				NeedValX -= width ;
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
	for ( tmp1 = yshift, SourceRow = 0,
	      dst = SCREENADDRESS( ( x + w ), y ) ;
	      SourceRow < height ;
	      tmp1++, SourceRow++, dst += BYTES_PER_ROW ) {
		register unsigned bitPattern ;
		if ( tmp1 >= height )
			tmp1 -= height ;
		/*
		 * For Each Time Pattern Repeats In The Y Dimension
		 */
		for ( xDst = dst, DestinationRow = SourceRow,
		      bitPattern = getbits( NeedValX, width,
					    mastersrc
					+ ( tmp1 * paddedByteWidth ) ) ;
		      DestinationRow < h ;
		      xDst += scr_incr, DestinationRow += height ) {
			/* Read To Save */
			tmp2 = *( (PliMemoryPtr) xDst) ;
			/* Write Pattern */
			*( (PliMemoryPtr) xDst ) = bitPattern ;
		}
	}
}

return ;
}

static void
pliSetMonoRegisters( plane_mask, desiredState )
register unsigned long int plane_mask ;
register unsigned long int desiredState ;
{
/* Setup VGA Registers */
/*
 * Set The Plane-Enable
 */
SetVideoSequencer( Mask_MapIndex, plane_mask ) ;
SetVideoGraphics( Enb_Set_ResetIndex, plane_mask ) ;
/*
 * Put Display Into SET-AND (i.e. Write Mode 3 )
 */
SetVideoGraphics( Graphics_ModeIndex, PLI_WRITE_MODE_3 ) ;
/*
 * Set The Color in The Set/Reset Register
 */
SetVideoGraphics( Set_ResetIndex, desiredState & PLI_ALLPLANES ) ;
/*
 * Set The Vga's Alu Function
 */
SetVideoGraphics( Data_RotateIndex, desiredState >> 8 ) ;

/* Point At The Bit Mask Reg For Later */
SetVideoGraphicsIndex( Bit_MaskIndex ) ;

return ;
}

static int
pliCalcMonoMode( rasterOp, color )
int rasterOp ;
register unsigned long int color ;
{
#define DO_RECURSE 0x10000
register unsigned int data_rotate_value = PLI_COPY_MODE << 8 ;
register unsigned int invert_existing_data = 0 ;

/* Test The Raster-Op */
switch ( rasterOp ) {
	case GXclear:		/* 0x0 Zero 0 */
		color = 0 ;
		break ;
	case GXinvert:		/* 0xa NOT dst */
		data_rotate_value = PLI_XOR_MODE << 8 ;
	case GXset:		/* 0xf 1 */
		color = PLI_ALLPLANES ;
		break ;
	case GXnor:		/* 0x8 NOT src AND NOT dst */
		invert_existing_data = DO_RECURSE ;
	case GXandInverted:	/* 0x4 NOT src AND dst */
		color = ~color ;
	case GXand:		/* 0x1 src AND dst */
		data_rotate_value = PLI_AND_MODE << 8 ;
	case GXcopy:		/* 0x3 src */
		break ;
	case GXequiv:		/* 0x9 NOT src XOR dst */
		color = ~color ;
	case GXxor:		/* 0x6 src XOR dst */
		data_rotate_value = PLI_XOR_MODE << 8 ;
		break ;
	case GXandReverse:	/* 0x2 src AND NOT dst */
		invert_existing_data = DO_RECURSE ;
		data_rotate_value = PLI_AND_MODE << 8 ;
		break ;
	case GXorReverse:	/* 0xb src OR NOT dst */
		invert_existing_data = DO_RECURSE ;
		data_rotate_value = PLI_OR_MODE << 8 ;
		break ;
	case GXnand:		/* 0xe NOT src OR NOT dst */
		invert_existing_data = DO_RECURSE ;
	case GXorInverted:	/* 0xd NOT src OR dst */
		color = ~color ;
	case GXor:		/* 0x7 src OR dst */
		data_rotate_value = PLI_OR_MODE << 8 ;
		break ;
	case GXcopyInverted:	/* 0xc NOT src */
		color = ~color ;
		break ;
	case GXnoop:		/* 0x5 dst */
		; /* Shouldn't Get Here !! */
}

return ( color & PLI_ALLPLANES ) | data_rotate_value | invert_existing_data ;
}

void
pliDrawMonoImage( data, x, y, w, h, fg, alu, planes )
unsigned char *data;
int x, y, w, h ;
unsigned long int fg ;
int alu ;
unsigned long int planes;
{
int cursor_saved ;
int regState ;

if ( ( alu == GXnoop ) || !( planes &= PLI_ALLPLANES ) )
	return ;

if ( ( regState = pliCalcMonoMode( alu, (char) fg ) ) & DO_RECURSE ) {
	pliDrawMonoImage( data, x, y, w, h,
			  PLI_ALLPLANES, GXinvert, planes ) ;
	regState &= ~DO_RECURSE ;
}

/* Check If Cursor Is In The Way */
cursor_saved = !pliCursorSemaphore && pliCheckCursor( x, y, w, h ) ;

pliSetMonoRegisters( planes, regState ) ;

DoMonoSingle( w, x, y, (const unsigned char *) data, h,
	      w, ( ( w + 31 ) & ~31 ) >> 3, h, 0, 0 ) ;

/* Replace Cursor If Nessesary */
if ( cursor_saved )
	pliReplaceCursor() ;

return ;
}

void
pliFillStipple( pStipple, fg, alu, planes, x, y, w, h, xSrc, ySrc )
register PixmapPtr const pStipple ;
unsigned long int fg ;
const int alu ;
unsigned long int planes ;
int x, y, w, h ;
const int xSrc, ySrc ;
{
unsigned int width ;
unsigned int height ;
int xshift ;
int yshift ;
int regState ;
int cursor_saved ;

if ( ( alu == GXnoop ) || !( planes &= PLI_ALLPLANES ) )
	return ;

if ( ( regState = pliCalcMonoMode( alu, (char) fg ) ) & DO_RECURSE ) {
	pliFillStipple( pStipple, PLI_ALLPLANES, GXinvert, planes,
			x, y, w, h, xSrc, ySrc ) ;
	regState &= ~DO_RECURSE ;
}

/* Check If Cursor Is In The Way */
cursor_saved = !pliCursorSemaphore && pliCheckCursor( x, y, w, h ) ;

pliSetMonoRegisters( planes, regState ) ;

/* Figure Bit Offsets & Source Address */
width = pStipple->drawable.width ;
if ( ( xshift = ( x - xSrc ) ) < 0 )
	xshift = width - ( ( - xshift ) % width ) ;
else
	xshift %= width ;

height = pStipple->drawable.height ;
if ( ( yshift = ( y - ySrc ) ) < 0 )
	yshift = height - ( ( - yshift ) % height ) ;
else
	yshift %= height ;

	(* ( (h > height) ? DoMonoMany : DoMonoSingle ) ) (
			w, x, y,
			(const unsigned char *) pStipple->devPrivate.ptr,
			h,
			width,
			( ( width + 31 ) & ~31 ) >> 3,
			height,
			xshift, yshift
			) ;

/* Replace Cursor If Nessesary */
if ( cursor_saved )
	pliReplaceCursor() ;

return ;
}
