/*
 * $Id: snd_rcv.c,v 1.2 1991/09/16 21:32:04 ohm Exp $
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

void xputc_cur();

static int fmode;
 /* JS_FILE_SEND (server receives) */
int
fopen_read_cur(fn)
char *fn;
{
	fmode = 'r';
	return 1;
}

/* JS_FILE_RECEIVE (server sends) */
int
fopen_write_cur(fn)
char *fn;
{
	fmode = 'w';
/*
	puts_cur(fn);putc_purge();
	if(get4_cur()== -1) return NULL;
	else return 1;
*/
	return 1;
}

/* JS_FILE_SEND (server recieves) */
int
fread_cur(p, size, nitems)
char *p;
register int size,nitems;
{
 register int i,j,xx;
 for(i=0;i<nitems;i++){
     for(j=0;j<size;j++){
	*p++ = xx =  xgetc_cur();
	if(xx== -1) return i;
    }
 }
 return nitems;
}

static int store = -1;

int
xgetc_cur()
{register int x;
 if(store != -1){ x = store; store = -1; return(x);}
 if((x= getc_cur()) != 0xFF) return x;
 if(getc_cur() == 0xFF) return -1; /* EOF */
 return 0xFF;
}

void
xungetc_cur(x)			/* H.T. */
int x;
{
    store = x;
}

/* JS_FILE_SRECEIVE (server sends) */
void
fwrite_cur(p, size, nitems)
unsigned char *p;
int size, nitems;
{register int i,x;
 x= size*nitems;
 for(i=0;i<x;i++){
	xputc_cur(*p++);
 }
}

void
xputc_cur(x)
unsigned char x;/* H.T. */
{
 putc_cur(x);
 if(x==0xFF){ putc_cur(0x00); }
}

void
fclose_cur()
{
 if(fmode!='w') return;
 putc_cur(0xFF);
 putc_cur(0xFF); /* EOF */
}
