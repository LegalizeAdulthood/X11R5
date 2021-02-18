/*
 * $Id: MALLOC.c,v 1.2 1991/09/16 21:31:06 ohm Exp $
 */
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/*	Version 4.0
 */
#include <stdio.h>
#include "wnn_malloc.h"
#undef	malloc
#undef	realloc
#undef	free

#ifdef	HONMONO
extern char *malloc();
extern void free();
extern char *realloc();
extern char *calloc();
#else
extern char *MALLOC();
extern void free();
extern char *REALLOC();
extern char *CALLOC();
#define	malloc(x) MALLOC(x)
#define	calloc(x,y) CALLOC(x,y)
#define	realloc(x,y) REALLOC(x,y)
#endif

char *
malloc0(size)
int size;
{
    test2();
    if(size == NULL){
	size = 1;
    }
    return(calloc(size,1));
}

void
free0(pter)
char *pter;
{
    if(pter == NULL) return;
    free(pter);
}

char *
realloc0(pter,size)
char *pter;
int size;
{
    if(size == NULL){
	size = 1;
    }
    if(pter == NULL){
	return(malloc(size));
    }else{
	return(realloc(pter,size));
    }
}
/********************************/
#undef	realloc
#undef	malloc
#undef	calloc

extern char *malloc();
extern char *calloc();
static char *malloc_p;

test2()
{
 test(malloc(5000));
}
test1()
{
 test(malloc_p);
}
test(p)
int *p;
{
	static int tst = 1;
	static int tst1 = 0;
	int *p0, *p1;
	malloc_p = (char *)p;
	if (tst) {
		p1 = p - 1;
		p0 = p1;
		while(1) {
		    if (tst1) {
			    printf("%x: %x\n", p1, *p1);
		    }
		    p1 = *p1 & 0xfffffffe;
		    if (p1 == p || p1 == p0){
			printf("OK!\n");
			break;
		    }
		}
	}
}


char *
MALLOC(cnt)
int cnt;
{
	char *p;
	printf("malloc size = %d\n", cnt);
	p = malloc(cnt);
	test(p);
	return(p);
}

char *
CALLOC(cnt,size)
int cnt;
int size;
{
static int tmp = 0;
	char *p;
	printf("%d : calloc size = %d\n",tmp++, cnt);
	p = calloc(cnt,size);
	test(p);
	return(p);
}


char *
REALLOC(p, cnt)
int *p;
int cnt;
{
	extern char * realloc();
	printf("realloc size = %d\n", cnt);
	p = realloc(p, cnt);
	test(p);
	return(p);
}

