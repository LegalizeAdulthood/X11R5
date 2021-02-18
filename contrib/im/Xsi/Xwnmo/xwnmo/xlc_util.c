/*
 * $Id: xlc_util.c,v 1.6 1991/09/30 02:03:33 proj Exp $
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
/*	Version 4.0
 */
#include <stdio.h>
#include <string.h>
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

char c_buf[512];

int get_columns_wchar(from)
register wchar *from;
{
    register wchar *wc = from;

    while(*wc == PENDING_WCHAR) wc++;
    if (*wc) wc++;
    return(wc - from);
}

int XwcGetColumn(wc)
wchar wc;
{
    if (wc == PENDING_WCHAR) return(0);
    return(_XcwGetLength(wc));
}

int
check_mb(buf, x)
register wchar *buf;
register int x;
{
    if (*(buf + x) == PENDING_WCHAR) return(1);
    if (XwcGetColumn(*(buf + x)) > 1) return(2);
    return(0);
}

int w_char_to_char(w, c, n)
register w_char *w;
register char *c;
register int n;
{
    register char *c1;

    for (c1 = c;n > 0;n--,w++) {
	if(*w & 0xff00)
	    *c1++ = (int)(*w & 0xff00) >> 8;
	/*
	else if (*w & 0x80)
	    *c1++ = SS2;
	*/
	*c1++ = *w & 0x00ff;
    }
    *c1 = 0;
    return (c1 - c);
}

int skip_pending_wchar(to, from)
register wchar *to, *from;
{
    register wchar *wc = to;
    for (; *from;) {
	if (*from == PENDING_WCHAR) from++;
	*wc++ = *from++;
    }
    *wc = 0;
    return(wc - to);
}

int put_pending_wchar_and_flg(wc, buf, att, flg)
register wchar wc;
wchar *buf;
unsigned char *att, flg;
{
    register wchar *p = buf;
    register unsigned char *f = att;
    register int mb_len, i;

    mb_len = XwcGetColumn(wc);
    for (i = 1; i < mb_len; i++) {
	*p++ = PENDING_WCHAR;
	*f++ = flg;
    }
    *p++ = wc;;
    *f++ = flg;
    return(mb_len);
}

#ifdef	CALLBACKS
int
XwcGetChars(wc, end, left)
register wchar *wc;
int end;
int *left;
{
    register int i, cnt;

    *left = 0;
    for (i = 0, cnt = 0; i < end && *wc; i++, cnt++, wc++) {
	if (*wc == PENDING_WCHAR) {
	    wc++;
	    i++;
	    if (!(i < end)) {
		for (*left = 1;*wc == PENDING_WCHAR; wc++) *left += 1;
		cnt++;
		return(cnt);
	    }
	}
    }
    return(cnt);
}
#endif	/* CALLBACKS */

int
char_to_wchar(xlc, c, wc, len, wc_len)
XLocale xlc;
unsigned char *c;
wchar *wc;
int len;
int wc_len;
{
    int wc_str_len = wc_len;
    int scanned_byte = 0;
    int ret;

    ret = _XConvertMBToWC(xlc, c, len, wc, &wc_str_len, &scanned_byte, NULL);
    if (ret != Success) return(0);
    return(wc_str_len);
}

int
w_char_to_wchar(xlc, w, wc, len, wc_len)
XLocale xlc;
w_char *w;
wchar *wc;
int len;
int wc_len;
{
    int ret;

    ret = w_char_to_char(w, c_buf, len);
    if (ret <= 0) return(0);

    return(char_to_wchar(xlc, (unsigned char *)c_buf, wc, ret, wc_len));
}

int w_char_to_ct(xlc, w, ct, len, ct_len)
XLocale xlc;
w_char *w;
unsigned char *ct;
int len;
int ct_len;
{
    int ret;
    int ct_str_len = ct_len;
    int scanned_byte = 0;

    ret = w_char_to_char(w, c_buf, len);
    if (ret <= 0) return(0);

    if (_XConvertMBToCT(xlc, c_buf, ret, ct, &ct_str_len, &scanned_byte, NULL)
	!= Success) return(0);
    return(ct_str_len);
}

int wchar_to_ct(xlc, w, ct, len, ct_len)
XLocale xlc;
wchar *w;
unsigned char *ct;
int len;
int ct_len;
{
    int scanned_byte;
    int ct_str_len = ct_len;

    if (_XConvertWCToCT(xlc, w, len, ct, &ct_str_len, &scanned_byte, NULL)
	!= Success) return(0);
    return(ct_str_len);
}

void
JWOutput(w, fs, gc, start_col, col, flg, offset_x, offset_y, cs, text, text_len)
Window w;
XFontSet fs;
GC gc;
short start_col, col;
unsigned char flg;
short offset_x, offset_y;
XCharStruct *cs;
wchar *text;
int text_len;
{
    register int start_x, start_y;
    wchar_t *w_text;
    int w_text_len;
#ifndef	X_WCHAR
    wchar_t w_buf[512];
    int mb_buf_len;
    int scanned_byte;
    int ret;
#endif	/* !X_WCHAR */

#ifdef	X_WCHAR
    w_text = (wchar_t *)text;
    w_text_len = text_len;
#else	/* X_WCHAR */
    mb_buf_len = 512;
#ifdef	XJUTIL
    ret = _XConvertWCToMB(NULL, text, text_len, c_buf,
#else
    ret = _XConvertWCToMB(cur_p->cur_xl->xlc, text, text_len, c_buf,
#endif
			  &mb_buf_len, &scanned_byte, NULL);
    if (ret != Success) return;
    w_text = w_buf;
    w_text_len = mbstowcs(w_buf, (char *)c_buf, 512);
#endif	/* X_WCHAR */

    start_x = (cs->width * start_col) + offset_x;
    start_y = cs->ascent + offset_y;
    if (flg & REV_FLAG) {
	XwcDrawImageString(dpy, w, fs, gc, start_x, start_y,
			   w_text, w_text_len);
    } else {
	XClearArea(dpy, w, start_x, offset_y,
		   (cs->width * col), (cs->ascent + cs->descent), False);
	XwcDrawString(dpy, w, fs, gc, start_x, start_y, w_text, w_text_len);
    }
    if (flg & BOLD_FLAG)
	XwcDrawString(dpy, w, fs, gc, (start_x + 1), start_y,
		       w_text, w_text_len);
    if (flg & UNDER_FLAG)
	XDrawLine(dpy, w, gc, start_x, (start_y + 1),
		  (start_x + cs->width * col), start_y + 1);
    return;
}

XCharStruct *
get_base_char(fs)
XFontSet fs;
{
    XCharStruct *cs;
    XFontStruct **fs_list;
    char **fn_list;

    if (XFontsOfFontSet(fs, &fs_list, &fn_list) < 1) {
	print_out("I could not get base char struct.");
	return(NULL);
    }
    cs = (XCharStruct *)Malloc(sizeof(XCharStruct));
    cs->width = fs_list[0]->max_bounds.width;
    cs->ascent = fs_list[0]->ascent;
    cs->descent = fs_list[0]->descent;
    return((XCharStruct *)cs);
}

XFontSet
create_font_set(s)
char *s;
{
    XFontSet xfontset;
    char **missing_charset_list;
    int missing_charset_count;
    char *def_string;

    xfontset = XCreateFontSet(dpy, s, &missing_charset_list,
			      &missing_charset_count, &def_string);
    if ((xfontset == NULL)/* || (missing_charset_count > 0) */) {
	print_out("Can not create FontSet\n");
	return((XFontSet)NULL);
    }
    return((XFontSet)xfontset);
}
