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

#include <X11/wchar.h>
int
wcslen(s)
    wchar_t *s;
{
    int l;

    l = 0;
    while (*s) {
	s++;
	l++;
    }
    return(l);
}

wchar_t *
wcscpy(p, q)
    wchar_t *p, *q;
{
    wchar_t *s;

    *s = *p;
    while (*p++ = *q++)
	;
    *p = (wchar_t)'\0';
    return(s);
}

wchar_t *
wcsncpy(p, q, n)
    wchar_t *p, *q;
    int n;
{
    wchar_t *s;

    *s = *p;
    while (n && (*p++ = *q++)) {
	n--;
    }
    if (n > 0) {
	*p = (wchar_t)'\0';
    }
    return(s);
}
    
