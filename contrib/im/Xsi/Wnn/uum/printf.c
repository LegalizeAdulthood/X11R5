/*
 * $Id: printf.c,v 1.2 1991/09/16 21:33:48 ohm Exp $
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
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "jllib.h"

extern int cursor_colum;
extern int (*code_trans[])();

int
char_q_len(x)
w_char x;
{
    return((*char_q_len_func)(x));
}

void
fprintf(file , x2 , x3 , x4 , x5 , x6 ,x7 , x8 , x9 , x10 , x11 , x12 , x13)
FILE *file;
char *x2, *x3, *x4, *x5, *x6, *x7, *x8, *x9, *x10, *x11, *x12, *x13;
{
    char buf2[512];

    sprintf(buf2 , x2, x3 , x4 , x5 , x6 ,x7 , x8 , x9, x10 , x11 , x12 , x13);
    cursor_colum += eu_columlen(buf2);
    
    puteustring(buf2, file); 
}

void
puteustring(buf2, file)
     char *buf2;
     FILE *file;
{
  char *c;
  char buf[512];

  (*code_trans[(J_EUJIS<<2) | tty_c_flag])(buf , buf2 , strlen(buf2) + 1);
  for(c = buf ; *c ; c++){
    putc(*c , file);
  }
}

void
printf(format , x3 , x4 , x5 , x6 ,x7 , x8 , x9, x10 , x11 , x12 , x13)
char *format;
char *x3, *x4, *x5, *x6, *x7, *x8, *x9, *x10, *x11, *x12, *x13;
{
    fprintf(stdout ,format, x3 , x4 , x5 , x6 ,x7 , x8 , x9, x10 , x11 , x12 , x13);

}

#define W_BUFLEN 32
static w_char w_buf[W_BUFLEN];
static int w_maxbuf = 0;

int
w_putchar(wch)
w_char wch;
{
    w_char tmp_wch[10];
    int len, i, c_len = 0;
    int ret_col = 0;

    if(ESCAPE_CHAR(wch)){
	ret_col = char_q_len(wch);
	w_buf[w_maxbuf++] =(w_char)('^');
	if(wch == 0x7f)w_buf[w_maxbuf++] =(w_char)('?');
	else w_buf[w_maxbuf++] =(w_char)(wch + 'A' - 1);
    }else{
        if (print_out_func) {
            len = (*print_out_func)(tmp_wch, &wch, 1);
            for (i = 0; i < len ; i++) {
                w_buf[w_maxbuf++] = tmp_wch[i];
                c_len = char_q_len(tmp_wch[i]);
                ret_col += c_len;
            }
	} else {
	    ret_col = char_q_len(wch);
	    w_buf[w_maxbuf++] = wch;
	}
    }
    cursor_colum += ret_col;
    if(w_maxbuf >= W_BUFLEN - 2){
	flushw_buf();
    }
    return(ret_col);
}

void
putchar_norm(c)
int c;
{
    push_hrus();
    putchar1(c);
    pop_hrus();
}

void
putchar1(c)
int c;
{
    putchar(c);
    flush();
    cursor_colum += 1;
}

void
flushw_buf()
{
    register char *c;
    register int len;
    
    static char buf[W_BUFLEN * 8];
    len=(*code_trans[(J_IUJIS<<2)|tty_c_flag])(buf , w_buf , sizeof(w_char) * w_maxbuf);
    for(c = buf ; len > 0 ; len --,c++){
	putchar(*c);
    }
    w_maxbuf = 0;
    flush();
}

extern char *wnn_perror();
void
errorkeyin()
{
    push_cursor();
    throw_c(0);
    clr_line();
    printf(wnn_perror());
    printf(MSG_GET(8));
    /*
    printf(" (Ç¡²¿)");
    */
    flush();
    keyin();
    pop_cursor();
}

#ifdef nodef
#ifdef CONVERT_by_STROKE
void
errorkeyin_q()
{
    push_cursor();
    throw_c(0);
    clr_line();
    printf(wnn_perror());
    printf(" (Try again !!!)");
    flush();
    ring_bell();
    pop_cursor();
}
#endif
#endif
