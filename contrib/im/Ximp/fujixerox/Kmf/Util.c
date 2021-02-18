/*

Copyright 1991 by Fuji Xerox Co., Ltd., Tokyo, Japan.

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of M.I.T. not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission.  Fuji Xerox Co., Ltd. makes no representations
about the suitability of this software for any purpose.  It is provided "as is"
without express or implied warranty.

FUJI XEROX CO.,LTD. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJI XEROX CO.,LTD. BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

*/

#include "XIMServer.h"

void *
XAllocFreeArea(size)
int size;
{
    void *p = (void *)Xmalloc(size);
    if (p) bzero(p, size);
    return(p);
}

FreeXIMText(text)
XIMText *text;
{
    if (text) {
	if (text->feedback) Xfree(text->feedback);
	if ((text->encoding_is_wchar) && (text->string.wide_char))
	    Xfree(text->string.wide_char);
	if ((!text->encoding_is_wchar) && (text->string.multi_byte))
	    Xfree(text->string.multi_byte);
	Xfree(text);
    }
}

Bool
CheckDestroyNotify(d, ev, w)
Display *d;
XDestroyWindowEvent *ev;
Window w;
{
    if ((ev->type == DestroyNotify) &&
	(ev->display == d) &&
	(ev->window == w)) {
	return(True); 
    }
    return(False);
}
