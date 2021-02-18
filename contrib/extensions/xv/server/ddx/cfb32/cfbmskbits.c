/************************************************************
Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the names of Sun or MIT not be used in
advertising or publicity pertaining to distribution  of  the
software  without specific prior written permission. Sun and
M.I.T. make no representations about the suitability of this
software for any purpose. It is provided "as is" without any
express or implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

********************************************************/

/* $XConsortium: cfbmskbits.c,v 4.6 91/07/05 10:52:59 rws Exp $ */

/*
 * ==========================================================================
 * Converted to Color Frame Buffer by smarks@sun, April-May 1987.  The "bit 
 * numbering" in the doc below really means "byte numbering" now.
 * ==========================================================================
 */

/*
   these tables are used by several macros in the cfb code.

   the vax numbers everything left to right, so bit indices on the
screen match bit indices in longwords.  the pc-rt and Sun number
bits on the screen the way they would be written on paper,
(i.e. msb to the left), and so a bit index n on the screen is
bit index 32-n in a longword

   see also cfbmskbits.h
*/
#include	<X.h>
#include	<Xmd.h>
#include	<servermd.h>
#include	"cfb.h"

#if	(BITMAP_BIT_ORDER == MSBFirst)
#define cfbBits(v)	(v)
#else
#define cfbFlip2(a)	((((a) & 0x1) << 1) | (((a) & 0x2) >> 1))
#define cfbFlip4(a)	((cfbFlip2(a) << 2) | cfbFlip2(a >> 2))
#define cfbFlip8(a)	((cfbFlip4(a) << 4) | cfbFlip4(a >> 4))
#define cfbFlip16(a)	((cfbFlip8(a) << 8) | cfbFlip8(a >> 8))
#define cfbFlip32(a)	((cfbFlip16(a) << 16) | cfbFlip8(a >> 16))
#define cfbBits(a)	cfbFlip32(a)
#endif

/* NOTE:
the first element in starttab could be 0xffffffff.  making it 0
lets us deal with a full first word in the middle loop, rather
than having to do the multiple reads and masks that we'd
have to do if we thought it was partial.
*/
#if PPW == 4
unsigned int cfbstarttab[] =
    {
	cfbBits(0x00000000),
	cfbBits(0x00FFFFFF),
	cfbBits(0x0000FFFF),
	cfbBits(0x000000FF)
    };
unsigned int cfbendtab[] =
    {
	cfbBits(0x00000000),
	cfbBits(0xFF000000),
	cfbBits(0xFFFF0000),
	cfbBits(0xFFFFFF00)
    };
#endif
#if PPW == 2
unsigned int cfbstarttab[] =
    {
	cfbBits(0x00000000),
	cfbBits(0x0000FFFF),
    };
unsigned int cfbendtab[] =
    {
	cfbBits(0x00000000),
	cfbBits(0xFFFF0000),
    };
#endif
#if PPW == 1
unsigned int cfbstarttab[] =
    {
	cfbBits(0x00000000),
    };
unsigned int cfbendtab[] = 
    {
	cfbBits(0x00000000),
    };
#endif

/* a hack, for now, since the entries for 0 need to be all
   1 bits, not all zeros.
   this means the code DOES NOT WORK for segments of length
   0 (which is only a problem in the horizontal line code.)
*/
#if PPW == 4
unsigned int cfbstartpartial[] =
    {
	cfbBits(0xFFFFFFFF),
	cfbBits(0x00FFFFFF),
	cfbBits(0x0000FFFF),
	cfbBits(0x000000FF)
    };

unsigned int cfbendpartial[] =
    {
	cfbBits(0xFFFFFFFF),
	cfbBits(0xFF000000),
	cfbBits(0xFFFF0000),
	cfbBits(0xFFFFFF00)
    };
#endif
#if PPW == 2
unsigned int cfbstartpartial[] =
    {
	cfbBits(0xFFFFFFFF),
	cfbBits(0x0000FFFF),
    };

unsigned int cfbendpartial[] =
    {
	cfbBits(0xFFFFFFFF),
	cfbBits(0xFFFF0000),
    };
#endif
#if PPW ==  1
unsigned int cfbstartpartial[] =
    {
	cfbBits(0xFFFFFFFF),
    };

unsigned int cfbendpartial[] =
    {
	cfbBits(0xFFFFFFFF),
    };
#endif

/* used for masking bits in bresenham lines
   mask[n] is used to mask out all but bit n in a longword (n is a
screen position).
   rmask[n] is used to mask out the single bit at position n (n
is a screen posiotion.)
*/

#if PPW == 4
unsigned int cfbmask[] =
    {
	cfbBits(0xFF000000),
 	cfbBits(0x00FF0000),
 	cfbBits(0x0000FF00),
 	cfbBits(0x000000FF)
    }; 
unsigned int cfbrmask[] = 
    {
	cfbBits(0x00FFFFFF),
 	cfbBits(0xFF00FFFF),
 	cfbBits(0xFFFF00FF),
 	cfbBits(0xFFFFFF00)
    };
#endif
#if PPW == 2
unsigned int cfbmask[] =
    {
	cfbBits(0xFFFF0000),
 	cfbBits(0x0000FFFF),
    }; 
unsigned int cfbrmask[] = 
    {
	cfbBits(0x0000FFFF),
 	cfbBits(0xFFFF0000),
    };
#endif
#if PPW == 1
unsigned int cfbmask[] =
    {
	cfbBits(0xFFFFFFFF),
    }; 
unsigned int cfbrmask[] = 
    {
	cfbBits(0xFFFFFFFF),
    };
#endif

/*
 * QuartetBitsTable contains four masks whose binary values are masks in the
 * low order quartet that contain the number of bits specified in the
 * index.  This table is used by getstipplepixels.
 */
unsigned int QuartetBitsTable[5] = {
    cfbBits(0x00000000),                         /* 0 - 0000 */
    cfbBits(0x00000008),                         /* 1 - 1000 */
    cfbBits(0x0000000C),                         /* 2 - 1100 */
    cfbBits(0x0000000E),                         /* 3 - 1110 */
    cfbBits(0x0000000F)                          /* 4 - 1111 */
};

/*
 * QuartetPixelMaskTable is used by getstipplepixels to get a pixel mask
 * corresponding to a quartet of bits.
 */
#if PPW == 4
unsigned int QuartetPixelMaskTable[16] = {
    cfbBits(0x00000000),
    cfbBits(0x000000FF),
    cfbBits(0x0000FF00),
    cfbBits(0x0000FFFF),
    cfbBits(0x00FF0000),
    cfbBits(0x00FF00FF),
    cfbBits(0x00FFFF00),
    cfbBits(0x00FFFFFF),
    cfbBits(0xFF000000),
    cfbBits(0xFF0000FF),
    cfbBits(0xFF00FF00),
    cfbBits(0xFF00FFFF),
    cfbBits(0xFFFF0000),
    cfbBits(0xFFFF00FF),
    cfbBits(0xFFFFFF00),
    cfbBits(0xFFFFFFFF)
};
#endif
#if PPW == 2
unsigned int QuartetPixelMaskTable[16] = {
    cfbBits(0x00000000),
    cfbBits(0x00000000),
    cfbBits(0x00000000),
    cfbBits(0x00000000),
    cfbBits(0x0000FFFF),
    cfbBits(0x0000FFFF),
    cfbBits(0x0000FFFF),
    cfbBits(0x0000FFFF),
    cfbBits(0xFFFF0000),
    cfbBits(0xFFFF0000),
    cfbBits(0xFFFF0000),
    cfbBits(0xFFFF0000),
    cfbBits(0xFFFFFFFF),
    cfbBits(0xFFFFFFFF),
    cfbBits(0xFFFFFFFF),
    cfbBits(0xFFFFFFFF),
};
#endif
#if PPW == 1
unsigned int QuartetPixelMaskTable[16] = {
    cfbBits(0x00000000),
    cfbBits(0x00000000),
    cfbBits(0x00000000),
    cfbBits(0x00000000),
    cfbBits(0x00000000),
    cfbBits(0x00000000),
    cfbBits(0x00000000),
    cfbBits(0x00000000),
    cfbBits(0xFFFFFFFF),
    cfbBits(0xFFFFFFFF),
    cfbBits(0xFFFFFFFF),
    cfbBits(0xFFFFFFFF),
    cfbBits(0xFFFFFFFF),
    cfbBits(0xFFFFFFFF),
    cfbBits(0xFFFFFFFF),
    cfbBits(0xFFFFFFFF),
};
#endif

#ifdef	vax
#undef	VAXBYTEORDER
#endif
