static char *hwapp_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/examples/atkbook/hw/RCS/hwapp.c,v 1.3 1991/09/12 16:15:25 bobg Exp $";

/* **************************************************** *\
Copyright 1989 Nathaniel S. Borenstein
Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear in
supporting documentation, and that the name of 
Nathaniel S. Borenstein not be used in
advertising or publicity pertaining to distribution of the software
without specific, written prior permission. 

Nathaniel S. Borenstein DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
Nathaniel S. Borenstein BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
\* ***************************************************** */
#include <hwapp.eh> /* Definition of this object (hwapp) */
#include <im.ih> /* Import interaction manager definition */
#include <text.ih> /* Import text object definition */
#include <textv.ih> /* Import textview object definition */

boolean hwapp__Start(self)
struct hwapp *self;
{
    struct im *im = im_Create(NULL);
    struct text *t = text_New();
    struct textview *tv = textview_New();

    if (im == NULL || t == NULL || tv == NULL) {
	/* Object creation failed, return error code */
	return(FALSE);
    }
    textview_SetDataObject(tv, t);
    text_InsertCharacters(t, 0, "Hello, world!", 13); 
    im_SetView(im, tv);
    textview_WantInputFocus(tv, tv);
    return(TRUE); /* Success */
}
