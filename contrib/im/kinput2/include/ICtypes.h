/* $Id: ICtypes.h,v 1.2 1990/12/07 15:29:26 ishisone Rel $ */
/*
 * Copyright (c) 1990  Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 */

#ifndef _ICtypes_h
#define _ICtypes_h

/*
 * $B%"%H%j%S%e!<%H$D$-J8;zNs7?(J -- ICString
 *	nbytes $B$O!"(JICString $B$N(J data $B$N%G!<%?7?$rCN$i$J$/$F$b(J
 *	$B%3%T!<$G$-$k$h$&$K$D$1$F$"$k(J
 */

typedef struct {
    unsigned short nchars;	/* data $B$NJ8;z?t(J */
    unsigned short nbytes;	/* data $B$N%P%$%H?t(J */
    char *data;			/* $BJ8;zNs(J ($B%(%s%3!<%G%#%s%0$O%/%i%90MB8(J) */
    int attr;			/* $BJ8;zNs$NB0@-(J */
} ICString;


/*
 * ICString.attr $B$NCM(J
 */

/* 1. $BJ8;zNs$,JQ49ESCf$N%F%-%9%H%;%0%a%s%H$rI=$7$F$$$k;~(J
 * ($B2<5-$NCM$N(J bitwise-or)
 */
#define ICAttrNotConverted	0	/* $B$^$@JQ49$5$l$F$$$J$$%;%0%a%s%H(J */
#define ICAttrConverted		1	/* $BJQ49:Q$_%;%0%a%s%H(J */
#define ICAttrCurrentSegment	2	/* $B8=:_CmL\$7$F$$$k%;%0%a%s%H(J */
#define ICAttrCurrentSubSegment 4	/* $B8=:_CmL\$7$F$$$k%5%V%;%0%a%s%H(J */

/* 2. $B$=$l0J30!"Nc$($P8=:_$NF~NO%b!<%I$J$I$rI=$7$F$$$k;~(J ($B>o$K$3$NCM(J) */
#define ICAttrNormalString	(-1)


/* $BA*Br$N0\F0J}8~(J */
#define ICMoveLeft	1
#define ICMoveRight	2
#define ICMoveUp	3
#define ICMoveDown	4
#define ICMoveLeftMost	5
#define ICMoveRightMost	6
#define ICMoveNextPage	7
#define ICMovePrevPage	8
#define ICMoveFirst	9
#define ICMoveLast	10

#endif /* _ICtypes_h */
