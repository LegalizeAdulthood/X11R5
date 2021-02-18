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

/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliCurs.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliCurs.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */

#ifndef lint
static char *rcsid = "$Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliCurs.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $" ;
#endif

#include "newmacros.h"
#include "X.h"
#include "Xmd.h"

#include "miscstruct.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "cursorstr.h"
#include "resource.h"

#include "OScompiler.h"

#include "pliScreen.h"
#include "ibmColor.h"

/*
#include "ibmIOArch.h"
*/

#include "ppc.h"
#include "ppcBitMacs.h"

#include "pliVideo.h"
#include "pliReg.h"
#include "pliProcs.h"

#include "ibmTrace.h"

#undef TRUE
#undef FALSE
#define TRUE 1
#define FALSE 0

#define ByteToRight( c_ptr ) ( ( (unsigned char *) c_ptr ) + 1 )

/* CONSTANTS FOR NOW */
#define MAX_CURSOR_WIDTH 32
#define MAX_CURSOR_HEIGHT 32
#define CURSOR_SAVE_ROW ( MAX_OFFSCREEN_ROW - MAX_CURSOR_HEIGHT )
#define CURSOR_SAVE_COL 0

/* Global Semaphore For Low Level ( e.g. Screen Drawing ) Routines */
int pliCursorSemaphore = 0 ;

/* Static Cursor Variables */
static int cursor_save_x_loc ;
static int cursor_save_y_loc ;
static int cursor_save_width ;
static int cursor_save_height ;

static short c_x ;
static short c_y ;
static unsigned long int c_fg = 1 ;
static unsigned long int c_bg = 0 ;
static unsigned short curs_fore_r, curs_fore_g, curs_fore_b;
static unsigned short curs_back_r, curs_back_g, curs_back_b;
static int active = FALSE ;
static int cursor_not_displayed = FALSE ;
static int screen_index ;

static unsigned long int cursorFG[ MAX_CURSOR_HEIGHT ] ;
static int foreground_width  = MAX_CURSOR_WIDTH ;	/* width */
static int foreground_height = MAX_CURSOR_HEIGHT ;	/* height */

static unsigned long int cursorBG[ MAX_CURSOR_HEIGHT ] ;
static int background_width  = MAX_CURSOR_WIDTH ;	/* width */
static int background_height = MAX_CURSOR_HEIGHT ;	/* height */

/* Bytewidth == 1 */
static void
fast_CursorBlt1( source, destination, height )
register volatile unsigned char *source ;
register volatile unsigned char *destination ;
register unsigned int height ;
{

#define row_jump ( BYTES_PER_ROW - 1 )

for ( ; height-- ; source += row_jump, destination += row_jump ) {
	*( (PliMemoryPtr) ( destination ++ ) ) =
		*( (PliMemoryPtr) ( source ++ ) ) ;
}
return ;
}
#undef row_jump
/* Bytewidth == 2 */
static void
fast_CursorBlt2( source, destination, height )
register volatile unsigned char *source ;
register volatile unsigned char *destination ;
register unsigned int height ;
{

#define row_jump ( BYTES_PER_ROW - 2 )

for ( ; height-- ; source += row_jump, destination += row_jump ) {
	*( (PliMemoryPtr) ( destination ++ ) ) =
		*( (PliMemoryPtr) ( source ++ ) ) ;
	*( (PliMemoryPtr) ( destination ++ ) ) =
		*( (PliMemoryPtr) ( source ++ ) ) ;
}
return ;
}
#undef row_jump
/* Bytewidth == 3 */
static void
fast_CursorBlt3( source, destination, height )
register volatile unsigned char *source ;
register volatile unsigned char *destination ;
register unsigned int height ;
{

#define row_jump ( BYTES_PER_ROW - 3 )

for ( ; height-- ; source += row_jump, destination += row_jump ) {
	*( (PliMemoryPtr) ( destination ++ ) ) =
		*( (PliMemoryPtr) ( source ++ ) ) ;
	*( (PliMemoryPtr) ( destination ++ ) ) =
		*( (PliMemoryPtr) ( source ++ ) ) ;
	*( (PliMemoryPtr) ( destination ++ ) ) =
		*( (PliMemoryPtr) ( source ++ ) ) ;
}
return ;
}
#undef row_jump
/* Bytewidth == 4 */
static void
fast_CursorBlt4( source, destination, height )
register volatile unsigned char *source ;
register volatile unsigned char *destination ;
register unsigned int height ;
{

#define row_jump ( BYTES_PER_ROW - 4 )

for ( ; height-- ; source += row_jump, destination += row_jump ) {
	*( (PliMemoryPtr) ( destination ++ ) ) =
		*( (PliMemoryPtr) ( source ++ ) ) ;
	*( (PliMemoryPtr) ( destination ++ ) ) =
		*( (PliMemoryPtr) ( source ++ ) ) ;
	*( (PliMemoryPtr) ( destination ++ ) ) =
		*( (PliMemoryPtr) ( source ++ ) ) ;
	*( (PliMemoryPtr) ( destination ++ ) ) =
		*( (PliMemoryPtr) ( source ++ ) ) ;
}
return ;
}
#undef row_jump
/* Bytewidth == 5 */
static void
fast_CursorBlt5( source, destination, height )
register volatile unsigned char *source ;
register volatile unsigned char *destination ;
register unsigned int height ;
{

#define row_jump ( BYTES_PER_ROW - 5 )

for ( ; height-- ; source += row_jump, destination += row_jump ) {
	*( (PliMemoryPtr) ( destination ++ ) ) =
		*( (PliMemoryPtr) ( source ++ ) ) ;
	*( (PliMemoryPtr) ( destination ++ ) ) =
		*( (PliMemoryPtr) ( source ++ ) ) ;
	*( (PliMemoryPtr) ( destination ++ ) ) =
		*( (PliMemoryPtr) ( source ++ ) ) ;
	*( (PliMemoryPtr) ( destination ++ ) ) =
		*( (PliMemoryPtr) ( source ++ ) ) ;
	*( (PliMemoryPtr) ( destination ++ ) ) =
		*( (PliMemoryPtr) ( source ++ ) ) ;
}
return ;
}
#undef row_jump

static void (*UnwoundBlitProcs[5])() = {
	fast_CursorBlt1,
	fast_CursorBlt2,
	fast_CursorBlt3,
	fast_CursorBlt4,
	fast_CursorBlt5
} ;

/* Only Does Special Case -- VERY Fast Blt
 * ( Width % 8 ) == 0 AND ( x0 % 8 ) == 0 AND ( x1 % 8 ) == 0
 */
static void
pliCursorBitBlt( x0, y0, x1, y1, w, h )
register int x0 ;
register int y0 ;
register int x1 ;
register int y1 ;
register int w, h ;
{

/* ******************** Vga Register Setup ******************** */
/* Disable SET/RESET Function */
SetVideoGraphics( Enb_Set_ResetIndex, 0 ) ;
/* Set Write Mode To 0 -- Read Mode To 0 */
SetVideoGraphics( Graphics_ModeIndex, 0 ) ;
/* Set Bit Mask -- ALL OFF */
SetVideoGraphics( Bit_MaskIndex, 0 ) ;
/* Ignore Data Rotate Function */
/* Set Map Mask */
SetVideoSequencer( Mask_MapIndex, PLI_ALLPLANES ) ;

/* Really Do It Here! */
(*UnwoundBlitProcs[ROW_OFFSET( w ) - 1])(
	(unsigned char *) PLIBASE + ( BYTES_PER_ROW * y0 ) + ROW_OFFSET( x0 ),
	(unsigned char *) PLIBASE + ( BYTES_PER_ROW * y1 ) + ROW_OFFSET( x1 ),
	h ) ;

return ;
}

static void
pliScreenDrawCursor( data, w, h, cursorHeight, x, y, xSrc, ySrc )
register const unsigned char *data ;
register int w ;
int const h ;
int const cursorHeight ;
int x ;
int const y ;
int const xSrc ;
int const ySrc ;
{
register const unsigned char *cptr ;
register int shift ;
register unsigned SourceRow ;
register int tmp1 ;
register int tmp2 ;
register volatile unsigned char *dst ;
register const unsigned char *lptr ;
register int NeedValX = 0 ;
register int SavNeedX ;
int xshift ;

#define paddedByteWidth ( MAX_CURSOR_WIDTH >> 3 ) /* Should Be 4 Bytes */

/* Figure Bit Offsets & Source Address */
if ( ( xshift = ( x - xSrc ) ) < 0 )
	xshift += MAX_CURSOR_WIDTH ;
else if ( xshift >= MAX_CURSOR_WIDTH )
	xshift -= MAX_CURSOR_WIDTH ;

if ( ( tmp1 = ( y - ySrc ) ) < 0 ) /* Now Use Tmp1 as Y-Shift */
	tmp1 += cursorHeight ;
else if ( tmp1 >= cursorHeight ) /* Now Use Tmp1 as Y-Shift */
	tmp1 -= cursorHeight ;

if ( tmp1 )
	data += ( tmp1 * paddedByteWidth ) ;

/* Point At The Bit Mask Reg */
SET_BYTE_REGISTER( GraphicsIndexRegister, Bit_MaskIndex ) ;

/* Do Left Edge */
if ( tmp1 = x & 07 ) {
	tmp2 = SCRRIGHT8( ( (unsigned) 0xFF ), tmp1 ) ;
	/* Catch The Cases Where The Entire Region Is Within One Byte */
	if ( ( w -= 8 - tmp1 ) < 0 ) {
		tmp2 &= SCRLEFT8( 0xFF, -w ) ;
		w = 0 ;
	}
	SET_BYTE_REGISTER( GraphicsDataRegister, tmp2 ) ;
	/*
	 * For Each Line In The Cursor Data
	 */
	SourceRow = h ;
	dst = PLIBASE + BYTE_OFFSET( x, y ) ;
	if ( ( NeedValX = xshift - tmp1 ) < 0 ) {
		NeedValX += MAX_CURSOR_WIDTH ;
		cptr = data + ( NeedValX >> 3 ) - paddedByteWidth ;
	}
	else
		cptr = data + ( NeedValX >> 3 ) ;
	if ( shift = NeedValX & 7 )
		for ( ;
		      SourceRow-- ;
		      dst += BYTES_PER_ROW, cptr += paddedByteWidth ) {
			/* Read To Save */
			tmp2 = *( (PliMemoryPtr) dst ) ;
			/* Write Pattern */
			*( (PliMemoryPtr) dst ) =
				SCRLEFT8( *cptr, shift )
			      | SCRRIGHT8( *ByteToRight( cptr ), ( 8 - shift ) );
		}
	else /* shift == 0 */
		for ( ;
		      SourceRow-- ;
		      dst += BYTES_PER_ROW, cptr += paddedByteWidth ) {
			/* Read To Save */
			tmp2 = *( (PliMemoryPtr) dst ) ;
			/* Write Pattern */
			*( (PliMemoryPtr) dst ) = *cptr ;
		}
	x = ( x + 7 ) & ~07 ;
	if ( ( SavNeedX = NeedValX + 8 ) >= MAX_CURSOR_WIDTH )
		SavNeedX -= MAX_CURSOR_WIDTH ;
}
else {
	SavNeedX = xshift ;
}
shift = SavNeedX & 0x7 ;

/* Fill The Center Of The Box */
SET_BYTE_REGISTER( GraphicsDataRegister, 0xFF ) ; /* Set The Bit Mask Reg */

/*
 * For Each Line In The Source Pixmap
 */
for ( SourceRow = h, dst = PLIBASE + BYTE_OFFSET( x, y ), lptr = data ;
      SourceRow-- ;
      dst += BYTES_PER_ROW - ROW_OFFSET( w ),
      lptr += paddedByteWidth ) {
	/*
	 * For Each Byte Across The Pattern In The X Dimension
	 */
	for ( tmp2 = ROW_OFFSET( w ), NeedValX = SavNeedX ;
	      tmp2-- ;
	      dst++ ) {
		/* Read To Save */
		tmp1 = *( (PliMemoryPtr) dst ) ;
		/* Write Pattern */
		cptr = lptr + ( NeedValX >> 3 ) ;
		*( (PliMemoryPtr) dst ) = ( shift )
			? ( SCRLEFT8( *cptr, shift )
			  | SCRRIGHT8( *ByteToRight( cptr ), ( 8 - shift ) ) )
			: *cptr ;
		if ( ( NeedValX += 8 ) >= MAX_CURSOR_WIDTH )
			NeedValX -= MAX_CURSOR_WIDTH ;
	}
}

/* Do Right Edge */
if ( tmp1 = BIT_OFFSET( w ) ) { /* x Now Is Byte Aligned */
	SET_BYTE_REGISTER( GraphicsDataRegister,  /* Set The Bit Mask */
			   SCRLEFT8( 0xFF, ( 8 - tmp1 ) ) ) ;
	/*
	 * For Each Line In The Source Pixmap
	 */
	cptr = data + ( NeedValX >> 3 ) ;
	SourceRow = h ;
	dst = PLIBASE + BYTE_OFFSET( ( x + w ), y ) ;
	if ( shift = NeedValX & 7 )
		for ( ;
		      SourceRow-- ;
		      dst += BYTES_PER_ROW, cptr += paddedByteWidth ) {
			/* Read To Save */
			tmp1 = *( (PliMemoryPtr) dst ) ;
			/* Write Pattern */
			*( (PliMemoryPtr) dst ) =
				SCRLEFT8( *cptr, shift )
			      | SCRRIGHT8( *ByteToRight( cptr ), ( 8 - shift ) );
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

return ;
#undef paddedByteWidth
}

void
pliRemoveCursor()
{
    pliCursorBitBlt( CURSOR_SAVE_COL, CURSOR_SAVE_ROW,
		     cursor_save_x_loc, cursor_save_y_loc,
		     cursor_save_width, cursor_save_height ) ;
    active = FALSE ;
return ;
}

void
pliShowCursor( x_loc, y_loc )
register const int x_loc, y_loc ;
{
register int x ;
register int y ;
register int fgwidth ;
register int bgwidth ;
register int bg_height ;
register int fg_height ;
register int hotx, hoty ;

if ( active )
	pliRemoveCursor() ;

bg_height = background_height ;
fg_height = foreground_height ;
fgwidth = foreground_width ;
bgwidth = background_width ;

x = ( hotx = ( ( c_x = x_loc ) - ibmCursorHotX( screen_index ) ) ) ;
y = ( hoty = ( ( c_y = y_loc ) - ibmCursorHotY( screen_index ) ) ) ;

/* Validate The Arguments */
if ( x < 0 ) {
	fgwidth += x ;
	bgwidth += x ;
	x = 0 ;
}
if ( x + fgwidth > ( MAX_COLUMN + 1 ) )
	fgwidth = ( MAX_COLUMN + 1 ) - x ;
if ( x + bgwidth > ( MAX_COLUMN + 1 ) )
	bgwidth = ( MAX_COLUMN + 1 ) - x ;

if ( y < 0 ) {
	bg_height += y ;
	fg_height += y ;
	y = 0 ;
}
if ( y + bg_height > ( MAX_ROW + 1 ) )
	bg_height = ( MAX_ROW + 1 ) - y ;
if ( y + fg_height > ( MAX_ROW + 1 ) )
	fg_height = ( MAX_ROW + 1 ) - y ;

/* Check If The Cursor Is On-Screen */
if ( ( ( fgwidth > 0 ) || ( bgwidth > 0 ) )
  && ( ( bg_height > 0 ) || ( fg_height > 0 ) )
  && ( x <= MAX_COLUMN )
  && ( y <= MAX_ROW ) ) {
	/* Round Edges To Use FAST Blt */
	pliCursorBitBlt( 
		   ( cursor_save_x_loc = x & ~0x7 ),
		   ( cursor_save_y_loc = y ),
		   CURSOR_SAVE_COL,
		   CURSOR_SAVE_ROW,
( cursor_save_width = ( ( MAX( bgwidth, fgwidth )
			+ ( x & 0x7 ) ) + 0x7 ) & ~0x7 ),
		   ( cursor_save_height = MAX( bg_height, fg_height ) ) ) ;
	/*
	 * Set The Plane-Enable
	 */
	SetVideoSequencer( Mask_MapIndex, PLI_ALLPLANES ) ;
	/*
	 * Set The Pli's Alu Function
	 */
	SetVideoGraphics( Data_RotateIndex, PLI_COPY_MODE ) ;
	/*
	 * Put Display Into SET/RESET Write Mode
	 */
	SetVideoGraphics( Graphics_ModeIndex, PLI_WRITE_MODE_3 ) ;

	if ( ( fg_height > 0 ) && ( fgwidth > 0 ) ) {
		/*
		 * Set The Color in The Set/Reset Register
		 */
		SetVideoGraphics( Set_ResetIndex, c_fg ) ;
		pliScreenDrawCursor( (unsigned char *) cursorFG,
				     fgwidth,
				     fg_height,
				     foreground_height,
				     x, y,
				     hotx, hoty ) ;
	}
	if ( ( bg_height > 0 ) && ( bgwidth > 0 ) ) {
		/*
		 * Set The Color in The Set/Reset Register
		 */
		SetVideoGraphics( Set_ResetIndex, c_bg ) ;
		pliScreenDrawCursor( (unsigned char *) cursorBG,
				     bgwidth,
				     bg_height,
				     background_height,
				     x, y,
				     hotx, hoty ) ;
	}
	active = TRUE ;
}

return ;
}

void
pliCursorInit( index )
register int index ;
{

    screen_index = index ;
    c_x = 0 ;
    c_y = 0 ;
    ibmCursorShow( index )=	pliShowCursor ;
    ibmCurrentCursor( index )=	NULL ;
    active = FALSE ;
    pliCursorSemaphore = 0 ;

return ;
}

void
pliColorCursor( fg, bg )
register unsigned long int fg, bg ;
{

if ( ( fg > 0xF ) || ( bg > 0xF ) ) {
	ErrorF( "pliColorCursor: bad color value(s)(fg/bg)=(%d,%d)", fg, bg ) ;
	return ;
}
c_fg = fg ;
c_bg = bg ;

return ;
}

void
pliRecolorCursor( cmap )
register ColormapPtr cmap;
{
c_fg = ibmFindColor( cmap, curs_fore_r, curs_fore_g, curs_fore_b ) ;
c_bg = ibmFindColor( cmap, curs_back_r, curs_back_g, curs_back_b ) ;
return ;
}

/***============================================================***/
/*
static void
pliStripCursor( cursorBitArray )
register unsigned long int cursorBitArray[32] ;
{
register unsigned int i ;
register unsigned long int bitMask ;
register unsigned long int *sptr ;

*/
    /* Strip The Cursor Down To Size */
    /* Remove Zero's At The Beginning */
/*
    for ( i = MAX_CURSOR_HEIGHT, sptr = curdef ;
	  i && !*sptr ;
	  i--, sptr-- ) ;
    foregroundOffsetY = MAX_CURSOR_WIDTH - i ;
*/
    /* Remove Zero's At The End */
/*
    for ( sptr = &( cursorBitArray[31] ) ;
	  !*sptr ;
	  i--, sptr-- ) ;
    foreground_height = i ;
*/
    /* Find All Used Bit Positions */
/*
    for ( bitMask = 0 ;
	  i-- ;
	  bitMask |= *sptr-- ) ;
    bitMask = ~bitMask ;
*/
    /* Leading & Trailing 0nes are Unused Bit Positions */
    /* Remove Zero's At The Beginning */
/*
    for ( i = MAX_CURSOR_WIDTH ;
	  bitMask & 0x80000000 && i-- ;
	  bitMask <<= 1 ) ;
*/
    /* Remove Zero's At The End */
/*
    for ( bitMask >>= ( foregroundHorizOffset = MAX_CURSOR_WIDTH - i ) ;
	  bitMask & 0x1 && i-- ;
	  bitMask >>= 1 ) ;
    foreground_width = i ;

*/
    /* Build Copy(s) */
/*
    for ( dptr = &cursorFG[i -1] ;
	  i-- ;
	  *dptr-- = *sptr-- ) ;

    return ;
}
*/

/***============================================================***/

int
pliDisplayCursor( pScr, pCurs )
register ScreenPtr const pScr ;
register CursorPtr const pCurs ;
{
    register const unsigned long int *curdef ;
    register const unsigned long int *sptr ;
    register unsigned long int *dptr ;
    register unsigned long int bitmask ;
    register int i ;

    TRACE( ("pliDisplayCursor( pScr = 0x%x, pCurs = 0x%x )\n", pScr, pCurs ) ) ;

    if ( active )
	pliRemoveCursor() ;
    curdef = (const unsigned long int *) pCurs->devPriv[ pScr->myNum ] ;

    ibmCursorHotX( pScr->myNum ) = pCurs->bits->xhot ;
    ibmCursorHotY( pScr->myNum ) = pCurs->bits->yhot ;
    ibmCurrentCursor( pScr->myNum ) = pCurs ;

    pliColorCursor(
       ibmFindColor( ((ppcScrnPrivPtr)pScr->devPrivate)->InstalledColormap,
		     curs_fore_r = pCurs->foreRed,
		     curs_fore_g = pCurs->foreGreen,
		     curs_fore_b = pCurs->foreBlue ),
       ibmFindColor( ((ppcScrnPrivPtr) (pScr->devPrivate))->InstalledColormap,
		     curs_back_r = pCurs->backRed,
		     curs_back_g = pCurs->backGreen,
		     curs_back_b = pCurs->backBlue ) ) ;

    /* Strip The Cursor Down To Size */
    for ( i = MAX_CURSOR_HEIGHT, sptr = &( curdef[63] ) ;
	  i && !*sptr ;
	  i--, sptr-- ) ;
    background_height = i ;
    for ( bitmask = 0, dptr = &cursorBG[i -1] ;
	  i-- ;
	  bitmask |= ( *dptr-- = *sptr-- ) ) ;
    for ( bitmask = ~bitmask, i = MAX_CURSOR_WIDTH ;
	  bitmask & SCRRIGHT( -1, 31 ) ;
	  bitmask = SCRRIGHT( bitmask, 1 ), i-- ) ;
    background_width = i ;
    for ( i = MAX_CURSOR_HEIGHT, sptr = &( curdef[31] ) ;
	  i && !*sptr ;
	  i--, sptr-- ) ;
    foreground_height = i ;
    for ( dptr = &cursorFG[i -1], bitmask = 0 ;
	  i-- ;
	  bitmask |= ( *dptr-- = *sptr-- ) ) ;
    for ( bitmask = ~bitmask, i = MAX_CURSOR_WIDTH ;
	  bitmask & SCRRIGHT( -1, 31 ) ;
	  bitmask = SCRRIGHT( bitmask, 1 ), i-- ) ;
    foreground_width = i ;

    pliShowCursor( c_x, c_y ) ;

    return TRUE ;
}

int
pliCheckCursor( x, y, lx, ly )
register const int x, y, lx, ly ;
{
/* check if the cursor is in this rectangle.  if so, remove and return TRUE
    else return FALSE */

if ( !pliCursorSemaphore && active
  && !( ( x >= ( cursor_save_x_loc + cursor_save_width ) )
     || ( y >= ( cursor_save_y_loc + cursor_save_height ) )
     || ( ( x + lx ) <= cursor_save_x_loc )
     || ( ( y + ly ) <= cursor_save_y_loc ) ) ) {
	pliRemoveCursor() ;
	return cursor_not_displayed = TRUE ;
}
else
	return FALSE ;
/*NOTREACHED*/
}

void
pliReplaceCursor()
{
if ( cursor_not_displayed && !pliCursorSemaphore ) {
	pliShowCursor( c_x, c_y ) ;
	cursor_not_displayed = FALSE ;
}

return ;
}
