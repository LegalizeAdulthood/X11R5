static char *hgghapp_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/examples/atkbook/hginset/RCS/hgghapp.c,v 1.3 1991/09/12 16:15:17 bobg Exp $";

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
#include <hgghapp.eh>
#include <im.ih>
#include <hggh.ih>
#include <hgghview.ih>

boolean hgghapp__InitializeObject(c, self)
struct classheader *c;
struct hgghapp *self;
{
    hgghapp_SetMajorVersion(self, 1);
    hgghapp_SetMinorVersion(self, 0);
    return(TRUE);
}

void hgghapp__FinalizeObject(c, self)
struct classheader *c;
struct hgghapp *self;
{
}

boolean hgghapp__Start(self)
struct hgghapp *self;
{
    struct im *im = im_Create(NULL);
    struct hggh *h = hggh_New();
    struct hgghview *hv = hgghview_New();

    hgghview_SetDataObject(hv, h);
    im_SetView(im, hv);
    hgghview_WantInputFocus(hv, hv);
    return(TRUE);
}
