/*
 * $Id: sstrings.c,v 1.4 1991/09/16 21:29:25 ohm Exp $
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
#include "commonhd.h"
#include <stdio.h>
#include "wnn_string.h"

int 
sStrcpy(c , w)
register char *c;
register w_char *w;
{
    char *c0 = c;
    for(;*w != 0 ; w++){
	if(*w & 0xff00)
	  *c++ = (char)((*w & 0xff00) >> 8);
	else if (*w & 0x80)
	  *c++ = 0x8e;
	*c++ = (char)(*w & 0x00ff);
    }
    *c++ = '\0';
    return (c - c0);
}


int
Sstrcpy(w , c )
w_char *w;
unsigned char *c;
{
    w_char *w0 = w;

    for(;*c;){
	if(*c & 0x80){
	    if (*c == 0x8e)
		*c = 0;
	    *w++ = (unsigned short)(*c << 8) | *(c + 1);
	    c += 2;
	}else{
	    *w++ = (unsigned short)*c;
	    c += 1;
	}
    }
    *w = 0;
    return(w - w0);
}

#ifdef CHINESE
int
pan_Sstrcpy(w , c)
w_char *w;
unsigned char *c;
{
    w_char *w0 = w;

    for(;*c;){
	if(*c & 0x80){
	    *w++ = (unsigned short)(*c << 8) | *(c + 1);
	    c += 2;
	}else{
	    *w++ = (unsigned short)*c;
	    c += 1;
	}
    }
    *w = 0;
    return(w - w0);
}
#endif


void
Sreverse(d, s)
w_char *d, *s;
{
  w_char *s0;

  s0 = s;
  for(;*s++;);
  s--;
  for(;--s >= s0;){
    *d++ = *s;
  }
  *d = 0;
}

char *
Stos(c)
w_char *c;
{
    char *c1 = (char *)c;
    for(;*c;c++){
	if(ASCIIP(*c)){
	    *c1++ = *c;
	}else{
	    *c1++ = (*c << 8);
	    *c1++ = *c;
	}	
    }
    return((char *)c);
}

char *sStrncpy(s1,s2,n)
register char *s1;
register w_char *s2;
register int n;
{
	register char *d;

	for (d = s1;n > 0;n--,s2++) {
	    if(*s2 & 0xff00)
		*d++ = (char)((*s2 & 0xff00) >> 8);
	    *d++ = *s2 & 0x00ff;
	}
	return s1;
}

#ifdef CHINESE
void
CStrcat(w , c )
w_char *w;
w_char c;
{
    for ( ; *w; w++);

    *w++ = c;
    *w = 0;
}

int
Sstrcat(w , c )
w_char *w;
unsigned char *c;
{
    w_char *w0 = w;

    for ( ; *w; w++);

    for(; *c; ){
	if(*c & 0x80){
	    *w++ = (unsigned short)(*c << 8) | *(c + 1);
	    c += 2;
	}else{
	    *w++ = *c;
	    c++;
	}
    }
    *w = 0;
    return(w - w0);
}
#endif
