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

/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliVideo.h,v 6.3 90/03/12 15:12:05 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliVideo.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidpli_video = "$Header: /usr/pli/x11r4.ps2/mit/extensions/xpli/server/pliVideo.h,v 6.3 90/03/12 15:12:05 jrhyne Exp $";
#endif

#ifdef lint
#if defined(volatile)
#undef volatile
#endif
#define volatile /**/
#if defined(const)
#undef const
#endif
#define const /**/
#if defined(signed)
#undef signed
#endif
#define signed /**/
#endif

/*
 * References to all pc ( i.e. '286 ) memory in the
 * regions used by the [ev]ga server ( the 128K windows )
 * MUST be long-word ( i.e. 32-bit ) reads or writes.
 * This definition will change for other memory architectures
 * ( e.g. AIX-Rt )
 */
#if defined(ATRIO)
typedef unsigned long int VideoAdapterObject ;
#else
#if defined(RTIO) || defined(PCIO)
typedef unsigned char VideoAdapterObject ;
#else
	******** ERROR ********
#endif
#endif
typedef volatile VideoAdapterObject *VideoMemoryPtr ;
#if !defined(BUILD_EGA)
typedef volatile VideoAdapterObject *PliMemoryPtr ;
#else
typedef volatile VideoAdapterObject *EgaMemoryPtr ;
#endif

#if defined(BUILD_EGA)
#define EGA_DEV_FILE "/dev/ega"
#else
#define PLI_DEV_FILE "/dev/pli"
#endif

#if !defined(BITMAP_BIT_ORDER)
#if defined(ibm032) || defined(i386) || defined(iAPX286) || defined(DOS) || defined (_IBMR2)
#define BITMAP_BIT_ORDER MSBFirst
#else
	******** ERROR ********
#endif
#endif

#if !defined(IMAGE_BYTE_ORDER)
#if defined(ibm032)
#define IMAGE_BYTE_ORDER MSBFirst
#else
#if defined(i386) || defined(iAPX286) || defined(DOS) || defined (_IBMR2)
#define IMAGE_BYTE_ORDER LSBFirst
#else
	******** ERROR ********
#endif
#endif
#endif

#if defined(BUILD_EGA)
#if defined(BSDrt) && defined(ATRIO)
#define SCREEN_ADDR	0x000b8000
#else
#define SCREEN_ADDR	0x000a0000
#endif
#else /* Must be for the pli */
#if ( defined(i386) && defined(AIX386) ) || defined (_IBMR2)
#define SCREEN_ADDR	0x020400000 /* "very" MAGIC NUMBER */
#else
#define SCREEN_ADDR	0x000a0000
#endif
#endif

#if defined(i386) || defined(iAPX286) || defined(DOS) || defined (_IBMR2)
#define VIDEO_MEMORY_BASE ( (volatile unsigned char *) ( SCREEN_ADDR ) )
#else /* BSD43 */
#if defined(ATRIO)
#define VIDEO_MEMORY_BASE ( (volatile unsigned char *) \
	( 0xd00c0000 | ( SCREEN_ADDR & 0x0001ffff ) ) )
#else
#if defined(RTIO)
#define VIDEO_MEMORY_BASE ( (volatile unsigned char *) \
	( 0xd00c0000 | ( SCREEN_ADDR & 0x0001ffff ) ) )
#endif
#endif
#endif

#if defined(BUILD_EGA)
#define EGABASE VIDEO_MEMORY_BASE
#else
#define PLIBASE VIDEO_MEMORY_BASE
#endif

/* Bit Ordering Macros */
#if !defined(SCRLEFT8)
#if (BITMAP_BIT_ORDER == MSBFirst)	/* pc/rt */
#define SCRLEFT8(lw, n)	( (unsigned char) (((unsigned char) lw) << (n)) )
#else
#if (BITMAP_BIT_ORDER == LSBFirst)	/* intel */
#define SCRLEFT8(lw, n)	( (unsigned char) (((unsigned char) lw) >> (n)) )
#else
	******** ERROR ********
#endif
#endif
#endif
#if !defined(SCRRIGHT8)
#if BITMAP_BIT_ORDER == MSBFirst	/* pc/rt */
#define SCRRIGHT8(lw, n)	( (unsigned char) (((unsigned char)lw) >> (n)) )
#else
#if BITMAP_BIT_ORDER == LSBFirst
#define SCRRIGHT8(lw, n)	( (unsigned char) (((unsigned char)lw) << (n)) )
#else
	******** ERROR ********
#endif
#endif
#endif
/* These work ONLY on 8-bit wide Quantities !! */
#define LeftmostBit ( SCRLEFT8( 0xFF, 7 ) & 0xFF )
#define RightmostBit ( SCRRIGHT8( 0xFF, 7 ) & 0xFF )

/*
 * [ev]ga video screen defines & macros
 */
#define PIX_PER_BYTE 8

#define xGA_BLACK_PIXEL 0
#define xGA_WHITE_PIXEL 1

#define xGA_MAXPLANES 3
#define xGA_ALLPLANES 0x7

#if !defined(BUILD_EGA)
#define NO_TUBE 0
#define COLOR_TUBE 1
#define MONO_TUBE 2

#define PLI_BLACK_PIXEL xGA_BLACK_PIXEL
#define PLI_WHITE_PIXEL xGA_WHITE_PIXEL

#define PLI_MAXPLANES xGA_MAXPLANES
#define PLI_ALLPLANES xGA_ALLPLANES

#define VIDEO_MEM_BYTES_PER_PLANE ( 64 * 1024 )

#ifndef PLI720
#define BYTES_PER_ROW 80
#define MAX_ROW	479
#else
#define BYTES_PER_ROW 90
#define MAX_ROW	539
#endif
#else /* Must be for ega */
#define EGA_BLACK_PIXEL xGA_BLACK_PIXEL
#define EGA_WHITE_PIXEL xGA_WHITE_PIXEL

#define EGA_MAXPLANES xGA_MAXPLANES
#define EGA_ALLPLANES xGA_ALLPLANES

#define VIDEO_MEM_BYTES_PER_PLANE ( 32 * 1024 )

#define BYTES_PER_ROW 80
#define MAX_ROW	349
#endif

#define MAX_COLUMN ( ( BYTES_PER_ROW * PIX_PER_BYTE ) - 1 )
#define MAX_OFFSCREEN_ROW ( ( VIDEO_MEM_BYTES_PER_PLANE / BYTES_PER_ROW ) - 1 )
#define ROW_OFFSET( PIXEL_X_VALUE ) ( ( PIXEL_X_VALUE ) >> 3 )
#define BIT_OFFSET( PIXEL_X_VALUE ) ( ( PIXEL_X_VALUE ) & 0x7 )
#define BYTE_OFFSET( PIXEL_X_VALUE, PIXEL_Y_VALUE ) \
	( ( ( PIXEL_Y_VALUE ) * BYTES_PER_ROW ) + ( ( PIXEL_X_VALUE ) >> 3 ) )
#define SCREENADDRESS( PIXEL_X_VALUE, PIXEL_Y_VALUE ) \
	( VIDEO_MEMORY_BASE + BYTE_OFFSET( PIXEL_X_VALUE, PIXEL_Y_VALUE ) )
