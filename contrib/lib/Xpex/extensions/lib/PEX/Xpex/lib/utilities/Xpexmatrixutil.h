/* $Header: Xpexmatrixutil.h,v 2.2 91/09/11 15:51:11 sinyaw Exp $ */
/* 
 *	Xpexmatrixutil.h 
 */

/*****************************************************************
Copyright 1991 by Sony Microsystems Company, San Jose, California

					All Rights Reserved

Permission to use, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sony not be used
in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

SONY DISCLAIMS ANY AND ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL EXPRESS WARRANTIES AND ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, FOR A PARTICULAR PURPOSE. IN NO EVENT
SHALL SONY BE LIABLE FOR ANY DAMAGES OF ANY KIND, INCLUDING BUT NOT
LIMITED TO SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING FROM
LOSS OF USE, DATA OR LOSS OF ANY PAST, PRESENT, OR PROSPECTIVE PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.
*****************************************************************/

#ifndef XPEX_MATRIX_UTIL_H_INCLUDED
#define XPEX_MATRIX_UTIL_H_INCLUDED

#define XPEX_NPC_XMIN 0.0
#define XPEX_NPC_YMIN 0.0 
#define XPEX_NPC_ZMIN 0.0
#define XPEX_NPC_XMAX 1.0
#define XPEX_NPC_YMAX 1.0
#define XPEX_NPC_ZMAX 1.0

#ifndef _ABS
#define _ABS(x) ((x) < 0 ? -(x) : (x))
#endif /* _ABS */

#define XPEX_IN_RANGE( low, high, val) \
    ((val) >= (low) && (val) <= (high))

#define XPEX_MAG_V3( v)          \
    (sqrt( (v)->x * (v)->x + (v)->y * (v)->y + (v)->z * (v)->z ))

#define XPEX_MAG_V2( v) \
    (sqrt( (v)->x * (v)->x + (v)->y * (v)->y ))

#define XPEX_ZERO_TOLERANCE              1.0e-30

#define XPEX_NEAR_ZERO( s) \
    (_ABS(s) < XPEX_ZERO_TOLERANCE)

#define XPEX_ZERO_MAG( s) \
    ((s) < XPEX_ZERO_TOLERANCE)

#endif /* XPEX_MATRIX_UTIL_H_INCLUDED */
