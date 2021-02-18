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
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/cmd/crrdic/RCS/crrdic.c,v 1.1 91/05/21 17:26:17 satoko Exp $";
#endif

#include	<stdio.h>
#include	<ctype.h>
#include	"RK.h"
#include	"RKintern.h"

#define		MAXKEY	1024

static char	fileName[256];
static int	lineNum;
static int	errCount;

/*VARARGS*/
void
alert(fmt, arg)
char	*fmt;
char	*arg;
{
    char	msg[256];
    (void)sprintf(msg, fmt, arg);
    (void)fprintf(stderr, "#line %d %s: %s\n", lineNum, fileName, msg);
    ++errCount;
}
void
fatal(fmt, arg)
char	*fmt;
char	*arg;
{
    char	msg[256];
    (void)sprintf(msg, fmt, arg);
    (void)fprintf(stderr, "#line %d %s: (FATAL) %s\n", lineNum, fileName, msg);
    exit(1);
}

unsigned char	*
getWORD(s, word, maxword)
unsigned char	*s;
unsigned char	*word;
int		maxword;
{
    unsigned 	c;
    int	 	i;

    i = 0;
    while ( *s && *s <= ' ' )
	s++;
    while ( (c = *s) > ' ' ) {
	s++;
	if ( c == '\\' ) {
	    switch(*s) {
	    case 0:
		break;
	    case '0':
		if ( s[1] == 'x' && isxdigit(s[2]) && isxdigit(s[3]) ) {
		    unsigned char   xx[3];

		    s += 2;
		    xx[0] = *s++; xx[1] = *s++; xx[2] = 0;
		    sscanf(xx, "%x", &c);
		    break;
		}
		else {
		    c = 0;
		    while ( isdigit(*s) ) 
			c = 8*c + (*s++ - '0');
		    break;
		};
	    default:
		c = *s++;
		break;
	    };
	};
	if ( i < maxword - 1 )
	    word[i++] = c;
    };
    word[i++] = 0;
    return s;
}

unsigned char
*allocStr(s)
unsigned char	*s;
{
    unsigned char	*d;

    if ( d = (unsigned char *)malloc(strlen(s) + 1) )
	 strcpy(d, s);
    return d;
}

struct roman { 
    unsigned char	*roma;
    unsigned char	*kana;
};

int
compar(p, q)
struct roman	*p, *q;
{	
    unsigned char	*s = p->roma;
    unsigned char	*t = q->roma;

    while ( *s == *t )
	if ( *s )
	    s++, t++;
	else
	    return 0;
    return ((int)*s) - ((int)*t);
}

main()
{
    struct roman	roman[MAXKEY];
    unsigned char	rule[256], *r;
    int			nKey, size;
    int			i;
    unsigned char	l4[4];

    nKey = 0;
    size  = 0;
    while ( gets(r = rule) ) {
	unsigned char	roma[256], kana[256];

	lineNum++;
	if ( *r == '#' )
	    continue;
	r = getWORD(r, roma, sizeof(roma));
	if ( roma[0] ) {
	    r = getWORD(r, kana, sizeof(kana));
	    if ( kana[0] ) {
		for ( i = 0; i < nKey; i++ ) 
		    if ( !strcmp(roman[i].roma, roma) ) 
			break;
		if ( i < nKey ) {
		    alert("multiply defined key <%s>", roma);
		    continue;
		};
		if ( nKey < MAXKEY ) {
		    roman[nKey].roma = allocStr(roma);
		    roman[nKey].kana = allocStr(kana);
		    size += strlen(roma) + 1 + strlen(kana) + 1;
		    nKey++;
		}
		else
		    fatal("more than %d romaji rules are given.", MAXKEY);
	    }
	    else 
		alert("syntax error", 0);
	};
    };

    if ( errCount )
	fatal("romaji dictionary is not produced.", 0);
    qsort((char *)roman, nKey, sizeof(struct roman), compar);
    putchar('R'); putchar('D');
    LTOL4((size<<16) | nKey, l4);
    putchar(l4[0]); putchar(l4[1]); putchar(l4[2]); putchar(l4[3]);

    for ( i = 0; i < nKey; i++ ) {
	r = roman[i].roma; do { putchar(*r); } while (*r++);
	r = roman[i].kana; do { putchar(*r); } while (*r++);
    };
    fprintf(stderr, "SIZE %d KEYS %d\n", size, nKey);
    exit(0);
}
