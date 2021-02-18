/*
 * $Id: printf.c,v 1.3 1991/09/30 02:04:33 proj Exp $
 */
/*
 * Copyright 1989, 1990, 1991 by OMRON Corporation
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON and MIT not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON and MIT make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * OMRON, MIT AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL OMRON, MIT OR WNN CONSORTIUM BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	Author:	Seiji Kuwari	OMRON Corporation
 *				kuwa@omron.co.jp
 *				kuwa%omron.co.jp@uunet.uu.net
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
	xwnmo
 */
#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#ifdef	XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#else	/* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#endif	/* XJUTIL */
#include "jllib.h"

#ifdef	XJUTIL
extern int cursor_colum;
static int t_byte_first = 0;
w_char work_wc;
#else	/* XJUTIL */
#define t_byte_first	(c_c->two_byte_first)
#define work_wc		(c_c->wc)
#endif	/* XJUTIL */

#define W_BUFLEN 32
static w_char w_buf[W_BUFLEN];
static int w_maxbuf = 0;
static char buf[256];
#ifdef	XJUTIL
static char tmp_buf[256];
#endif	/* XJUTIL */

int
char_q_len(x)
w_char x;
{
    return((*char_q_len_func)(x));
}

void
put_char(x)
unsigned char	x;
{
    if((x & 0x80) != 0) {
	if(t_byte_first == 1) {
	    work_wc += x;
	    JWMflushw_buf(&work_wc, 1);
	    t_byte_first = 0;
	} else {
	    t_byte_first = 1;
	    work_wc = x * 0x100;
	}
    } else {
	work_wc = x;
	t_byte_first = 0;
	JWMflushw_buf(&work_wc, 1);
    }
}

void
flushw_buf()
{
    JWMflushw_buf(w_buf, w_maxbuf);
    w_maxbuf = 0;
}

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
put_char1(c)
int c;
{
    put_char((unsigned char)c);
    cursor_colum += 1;
}

void
putchar_norm(c)
char c;
{
    push_hrus();
    put_char1(c);
    pop_hrus();
}

void
print_msg_getc(format, arg1, arg2, arg3)
register char	*format;
char *arg1, *arg2, *arg3;
{
    int ret;

    sprintf(buf, format, arg1, arg2, arg3);
    init_yes_or_no((unsigned char *)buf, YesMessage);
    for (;;) {
#ifdef	XJUTIL
	ret = xw_read(tmp_buf);
	if (ret != -1 && xjutil->yes_no->map) break;
#else	/* XJUTIL */
	ret = get_xevent();
	if (ret != -1 && cur_p->yes_no->map) break;
#endif	/* XJUTIL */
    }
    end_yes_or_no();
    return;
}

#ifdef	XJUTIL
#define cur_lang	xjutil->lang
#endif
extern char *wnn_perror_lang();
void
errorkeyin()
{
    print_msg_getc("%s", wnn_perror_lang(cur_lang), NULL, NULL);
}

#ifdef	CONVERT_by_STROKE
void
errorkeyin_q()
{
    print_msg_getc("%s", wnn_perror_lang(cur_lang), NULL, NULL);
}
#endif	/* CONVERT_by_STROKE */

#ifdef	XJUTIL
#undef cur_lang
#endif

void
print_out7(format, x1, x2, x3, x4, x5, x6, x7)
char *format;
char *x1, *x2, *x3, *x4, *x5, *x6, *x7;
{
    extern char *prgname;
    extern int fflush();

    fprintf(stderr, "%s : ", prgname);
    fprintf(stderr, format, x1, x2, x3, x4, x5, x6, x7);
    fprintf(stderr, "\r\n");
    fflush(stderr);
}
void
print_out3(format, x1, x2, x3)
char *format;
char *x1, *x2, *x3;
{
    extern char *prgname;
    extern int fflush();

    fprintf(stderr, "%s : ", prgname);
    fprintf(stderr, format, x1, x2, x3);
    fprintf(stderr, "\r\n");
    fflush(stderr);
}
void
print_out2(format, x1, x2)
char *format;
char *x1, *x2;
{
    extern char *prgname;
    extern int fflush();

    fprintf(stderr, "%s : ", prgname);
    fprintf(stderr, format, x1, x2);
    fprintf(stderr, "\r\n");
    fflush(stderr);
}
void
print_out1(format, x1)
char *format;
char *x1;
{
    extern char *prgname;
    extern int fflush();

    fprintf(stderr, "%s : ", prgname);
    fprintf(stderr, format, x1);
    fprintf(stderr, "\r\n");
    fflush(stderr);
}
void
print_out(format)
char *format;
{
    extern char *prgname;
    extern int fflush();

    fprintf(stderr, "%s : ", prgname);
    fprintf(stderr, format);
    fprintf(stderr, "\r\n");
    fflush(stderr);
}

void
malloc_error(x)
char *x;
{
    print_out1("Malloc failed in %s.", x);
}

void
print_msg_wait(format)
register char   *format;
{
    init_yes_or_no((unsigned char *)format, MessageOnly);
    for (;;) {
#ifdef	XJUTIL
	xw_read(tmp_buf);
	if (xjutil->yes_no->map && xjutil->yes_no->exp) break;
#else	/* XJUTIL */
	get_xevent();
	if (cur_p->yes_no->map && cur_p->yes_no->exp) break;
#endif	/* XJUTIL */
    }
    XFlush(dpy);
    sleep(2);
    end_yes_or_no();
}
