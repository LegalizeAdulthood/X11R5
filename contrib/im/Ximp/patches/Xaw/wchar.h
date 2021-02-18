/*
 * Copyright 1991 Oki Technosystems Laboratory, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Oki Technosystems Laboratory not
 * be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * Oki Technosystems Laboratory makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Oki TECHNOSYSTEMS LABORATORY DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL OKI TECHNOSYSTEMS LABORATORY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Yasuhiro Kawai,	Oki Technosystems Laboratory, Inc.
 *				kawai@otsl.oki.co.jp
 */
#ifndef _X11_WCHAR_H
#define _X11_WCHAR_H

#define iswcntrl(c) (((c) & 0x7f)?0:iscntrl(c))
#define iswprint(c) (((c) & 0x7f)?1:isprint(c))
#define iswspace(c) (((c) & 0x7f)?0:isspace(c))

/* typedef unsigned short wchar_t; */
/* typedef long  wchar_t */

typedef wchar_t wchar;
#define WNULL NULL
#define atowc(wc) ((wchar_t)wc)
#define wgetgroup(wc) (wc)
extern int wcslen();
extern wchar_t *wcscpy();
extern wchar_t *wcsncpy();
#endif /* _X11_WCHAR_H */
