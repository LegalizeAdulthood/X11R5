#ifndef lint
static char rcsid[] = "$Header: /usr/people/sam/tiff/tools/RCS/mk2dtab.c,v 1.6 91/07/16 16:31:41 sam Exp $";
#endif

/*
 * Copyright (c) 1990, 1991 Sam Leffler
 * Copyright (c) 1991 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

#include "tiffcompat.h"

/*
 * Program to construct packed state tables
 * used by 2d decompression algorithms (G3+G4).
 * The tables are indexed by PACK(code,len) and
 * the result, if nonzero, indicates a code match
 * and the new decoding state.
 */
static char* prolog = "\
#define	PACK(code,len)	(((len)<<4)+(code))\n\
\n\
#define	PASS		1\n\
#define	HORIZONTAL	2\n\
#define	VERTICAL	3\n\
#define	EXTENSION	4\n\
#define	    UNCOMPRESSED	7\n\
\n\
#define	PACKINFO(mode,v)	(((v)<<3)+mode)\n\
#define	UNPACKMODE(v)		((v)&07)\n\
#define	UNPACKINFO(v)		((v)>>3)\n\
\n";

#define	PACK(code,len)	(((len)<<4)+(code))

#define	PASS		1
#define	HORIZONTAL	2
#define	VERTICAL	3
#define	EXTENSION	4
#define	    UNCOMPRESSED	7

#define	PACKINFO(mode,v)	(((v)<<3)+mode)
#define	UNPACKMODE(v)		((v)&07)
#define	UNPACKINFO(v)		((v)>>3)

main()
{
#define	NTABENTS	(PACK(0xf,10)+1)
	short tab[NTABENTS];
	char* sep;
	int i;

	bzero(tab, sizeof (tab));
	tab[PACK(0x1,4)] = PACKINFO(PASS, 0);
	tab[PACK(0x1,3)] = PACKINFO(HORIZONTAL, 0);
	tab[PACK(0x1,1)] = PACKINFO(VERTICAL, 0);
	tab[PACK(0x3,3)] = PACKINFO(VERTICAL, 1);
	tab[PACK(0x3,6)] = PACKINFO(VERTICAL, 2);
	tab[PACK(0x3,7)] = PACKINFO(VERTICAL, 3);
	tab[PACK(0x2,3)] = PACKINFO(VERTICAL, -1);
	tab[PACK(0x2,6)] = PACKINFO(VERTICAL, -2);
	tab[PACK(0x2,7)] = PACKINFO(VERTICAL, -3);
	tab[PACK(0x8,10)] = PACKINFO(EXTENSION, 0);
	tab[PACK(0x9,10)] = PACKINFO(EXTENSION, 1);
	tab[PACK(0xA,10)] = PACKINFO(EXTENSION, 2);
	tab[PACK(0xB,10)] = PACKINFO(EXTENSION, 3);
	tab[PACK(0xC,10)] = PACKINFO(EXTENSION, 4);
	tab[PACK(0xD,10)] = PACKINFO(EXTENSION, 5);
	tab[PACK(0xE,10)] = PACKINFO(EXTENSION, 6);
	tab[PACK(0xF,10)] = PACKINFO(EXTENSION, UNCOMPRESSED);
	printf("%s", prolog);
	printf("static short g32dtab[%d] = {\n", NTABENTS);
	sep = "    ";
	for (i = 0; i < NTABENTS; i++) {
		printf("%s%4d", sep, tab[i]);
		if (((i + 1) % 8) == 0) {
			printf(",	/* 0x%02x - 0x%02x */\n", i-7, i);
			sep = "    ";
		} else
			sep = ", ";
	}
	if (i % NTABENTS)
		putchar('\n');
	printf("};\n");
	printf("#define	NG32D	(sizeof (g32dtab) / sizeof (g32dtab[0]))\n");
	exit(0);
}
