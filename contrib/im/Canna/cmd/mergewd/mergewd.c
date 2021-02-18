/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */

#ifndef lint
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/cmd/mergeword/RCS/mergeword.c,v 1.3 91/05/21 17:42:48 satoko Exp $";
#endif

#include	<stdio.h>
#include	"RKintern.h"

typedef unsigned char	uchar;

main(n, args)
int	n;
char	*args[];
{
    uchar	Yomi[256], Pair[1024], Kanji[2*1024];
    uchar	yomi[256], pair[1024], kanji[2*1024];
    uchar	S[256], *s, *d;
    uchar	fkanji[256];
    int		kouho = 0;
    int		wrec, krec = 0;
    int		rec = 0;

    int	first = 0;

    Yomi[0] = Pair[0] = Kanji[0] = 0;
    /*
    first = !strcmp(args[1], "-f");
    */

    while ( gets(s = S) ) {
	uchar	*p;

	if ( S[0] <= ' ' || S[0] == '#' )
	    continue;
	d = yomi;
	while ( (*d = *s++) > ' ' )	d++;
	*d = 0;
	while ( *s && *s <=  ' ' ) 	s++;
	d = pair;
	while ( (*d = *s++) > ' ' )	d++;
	*d = 0;
	while ( *s && *s <=  ' ' ) 	s++;
	d = kanji;
	while ( *d = *s++ )	d++;

	if ( !strcmp(Yomi, yomi) ) {
	    if ( !first ) {
		if ( strcmp(Pair, pair ) ) {
		     rec += strlen(kanji) + 4;
		     wrec = strlen(Yomi) + 1 + 1 + rec;
		     if( (kouho < NW_MAXCAND) && (wrec <= NW_MAXWREC) ) {
			  strcpy(Pair, pair);
			  strcat(Kanji, " ");
			  strcat(Kanji, Pair);

			  strcat(Kanji, " ");
			  strcat(Kanji, kanji);
			  strcpy(fkanji, kanji);
			  krec += strlen(kanji) + 4;
		     }
		     kouho++;
		}
		else if( strcmp(fkanji,kanji )){
		     rec += strlen(kanji) + 4;
		     wrec = strlen(Yomi) + 1 + 1 + rec;
		     if( (kouho < NW_MAXCAND) && (wrec <= NW_MAXWREC) ) {
			  strcat(Kanji, " ");
			  strcat(Kanji, kanji);
			  strcpy(fkanji, kanji);
			  krec += strlen(kanji) + 4;
		     }
		     kouho++;
		}
	    }
	}
	else {
	    if ( Yomi[0] ) {
		 wrec = strlen(Yomi) + 1 + 1 + rec;
		 printf("%s%s\n", Yomi, Kanji);
		 if( (kouho > NW_MAXCAND) && (wrec > NW_MAXWREC) )
		      fprintf(stderr, "%s: over word [%d %d]\n", Yomi, kouho, rec);
	    }
	    strcpy(Yomi, yomi);
	    strcpy(Pair, pair);
	    strcpy(fkanji, kanji);
			
	    strcpy(Kanji, " ");
	    strcat(Kanji, Pair);
	    strcat(Kanji, " ");
	    strcat(Kanji, kanji);
	    krec = strlen( kanji ) + 4;
	    rec = krec;
	    kouho = 1;
	};
    };
    if ( Yomi[0] ) {
	 wrec = strlen(Yomi) + 1 + 1 + rec;
	 printf("%s%s\n", Yomi, Kanji);
	 if( (kouho > NW_MAXCAND) && (wrec > NW_MAXWREC) )
	      fprintf(stderr, "%s: over word [%d %d]\n", Yomi, kouho, rec);
    }
    exit(0);
}





