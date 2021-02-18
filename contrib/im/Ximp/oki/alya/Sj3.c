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
#ifndef lint
static char sccsid[] = "@(#)Sj3.c	1.1";
#endif
/*
 * Sj3.c - 
 *
 * @(#)Sj3.c	1.1 91/09/28 17:13:13
 */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>
#include <pwd.h>
#include "Sj3P.h"

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffset(Sj3Object, sj3.field)

static XtResource resources[] = {
    { XtNserver, XtCServer, XtRString, sizeof(String),
      offset(server_name), XtRImmediate, (XtPointer)NULL },
};

static void Initialize();
static void Destroy();
static void Henkan();
static void Forward();
static void Backward();
static void Next();
static void Previous();
static void Cancel();
static wchar_t *Fix();

static initialized = False;

#define SuperClass		(&objectClassRec)
Sj3ObjectClassRec sj3ObjectClassRec = {
  {
/* core_class fields */	
    /* superclass	  	*/	(WidgetClass) SuperClass,
    /* class_name	  	*/	"Sj3Object",
    /* widget_size	  	*/	sizeof(Sj3ObjectRec),
    /* class_initialize   	*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize		  	*/	NULL,
    /* actions		  	*/	NULL,
    /* num_actions	  	*/	0,
    /* resources	  	*/	resources,
    /* num_resources	  	*/	XtNumber(resources),
    /* xrm_class	  	*/	NULLQUARK,
    /* compress_motion	  	*/	FALSE,
    /* compress_exposure  	*/	FALSE,
    /* compress_enterleave	*/	FALSE,
    /* visible_interest	  	*/	FALSE,
    /* destroy		  	*/	Destroy,
    /* resize		  	*/	NULL,
    /* expose		  	*/	NULL,
    /* set_values	  	*/	NULL,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	NULL,
    /* get_values_hook		*/	NULL,
    /* accept_focus	 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* tm_table		   	*/	NULL,
    /* query_geometry		*/	NULL,
    /* display_accelerator	*/	NULL,
    /* extension		*/	NULL
  },
/* sj3_class fields */
  {
    /* Henkan			*/      Henkan,
    /* Forward			*/      Forward,
    /* Backward			*/      Backward,
    /* Next			*/      Next,
    /* Previous			*/      Previous,
    /* Cancel			*/      Cancel,
    /* Fix			*/      Fix,
  }
};

WidgetClass sj3ObjectClass = (WidgetClass)&sj3ObjectClassRec;
    
/************************************************************
 *
 * Private
 *
 ************************************************************/

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
    Widget request;
    Widget new;
    ArgList args;
    Cardinal *num_args;
{
    Sj3Object imnew;
    Window win;
    int rc;
    struct passwd *pw;
#define MAX_DISP_BUF 200
    
    imnew = (Sj3Object)new;
    
    if (!initialized) {
	pw = getpwuid(getuid());
	if (pw) {
	    rc = sj3_open(imnew->sj3.server_name, pw->pw_name);
	    if (rc == 0) {
		initialized = TRUE;
	    }
	}
    }
    imnew->sj3.display_char = (wchar_t *)XtMalloc(sizeof(wchar_t)*MAX_DISP_BUF);
    imnew->sj3.feedback = (XIMFeedback *)XtMalloc(sizeof(XIMFeedback)*MAX_DISP_BUF);
    imnew->sj3.buf = NULL;
}

static void
Destroy(w)
    Widget w;
{
    Sj3Object imw = (Sj3Object)w;
    
/*
    sj3_close();
*/
    XtFree((char *)imw->sj3.display_char);
    XtFree((char *)imw->sj3.feedback);
}

/*
 * Private 
 */

#define KNJSIZ 1024

static int
wcslen(s)
    wchar_t *s;
{
    int l;
    
    l = 0;
    while (*s++) {
	l++;
    }
    return(l);
}

static int
sjtowc(p, q, l)
    unsigned char *p;
    wchar_t *q;
    int l;
{
    unsigned char c, cl;
    int i;
    
    i = 0;
    while ((c = *p++) && (!q || l > 0)) {
	if (c & 0x80) {
	    if (c < 0xa0 || c >= 0xe0) {
		cl = *p++;
		if (c >= 0xe0) {
		    c -= 0x70;
		}else {
		    c -= 0xb0;
		}
		c <<= 1;
		if (cl >= 0x9f) {
		    cl -= 0x7e;
		} else {
		    c--;
		    if (cl > 0x7f) {
			cl--;
		    }
		    cl -= 0x1f;
		}
		i++;
		if (q) {
		    *q++ = (c <<8) | cl | 0x8080;
		    l--;
		}
	    } else if (c > 0xa0 && c < 0xe0) {
		i++;
		if (q) {
	            *q++ = (wchar_t)c;
		    l--;
		}
	    }
	} else {
	    i++;
	    if (q) {
	        *q++=(wchar_t)(c & 0x7f);
	        l--;
	    }
	}
    }
    if (l > 0 && q) {
	*q = (wchar_t)'\0';
    }
    return (i);
}

static int
wctosj(p, q, l)
    wchar_t *p;
    unsigned char *q;
    int l;
{
    wchar_t c;
    unsigned char ch, cl;
    int i;
    
    i = 0;
    while ((c = *p++) && (!q || l > 0)) {
	switch (c & 0x8080) {
	  case 0:
	    i++;
	    if (q) {
	        *q++ = c & 0x7f;
	        l--;
	    }
	    break;
	  case 0x8080:
	    ch = (c >> 8) & 0x7f;
	    cl = c & 0x7f;
	    if (ch & 1) {
		cl += 0x1f;
		if (cl > 0x7e) {
		    cl++;
		}
	    } else {
		cl += 0x7e;
	    }
	    ch++;
	    ch >>= 1;
	    ch += 0x70;
	    if (ch > 0x9f) {
		ch += 0x40;
	    }
	    i += 2;
	    if (q) {
	        *q++ = ch;
	        l--;
	    }
	    if (l > 0 && q) {
		*q++ = cl;
	        l--;
	    }
	    break;
	  case 0x0080:
	    i++;
	    if (q) {
	        *q++ = c;
	        l--;
	    }
	    break;
	  case 0x8000:
	    break;
	  default:
	    break;
	}
    }
    if (l > 0 && q) {
	*q = '\0';
    }
    return (i);
}

static void
do_display(w)
    Sj3Object w;
{
    int i, n;
    
    wchar_t *p;
    XIMFeedback *f, fb;
    wc_douon_list *list;
    int dl, j;
    XIMText text;
    
    if (w->sj3.buf) {
	p = w->sj3.display_char;
	f = w->sj3.feedback;
	n = w->sj3.buf->n;
	list = w->sj3.buf->douon;
	
	for (i = 0; i < n; i++) {
	    if (!list->n) {
		dl = list->knjlen;
		bcopy((char *)list->knj, (char *)p, sizeof(wchar_t)*dl);
	    } else {
		dl = list->dou[list->current].dlen;
		bcopy((char *)list->dou[list->current].ddata, (char *)p,
		      sizeof(wchar_t)*dl);
	    }
	    p += dl;
	    if (i == w->sj3.buf->current) {
		fb = XIMReverse;
	    } else {
		fb = XIMUnderline;
	    }
	    for (j = 0; j < dl; j++) {
		*f++ = fb;
	    }
	    list++;
	}
	text.length = p - w->sj3.display_char;
	text.feedback = w->sj3.feedback;
	text.string.wide_char = w->sj3.display_char;
#ifdef DEBUG
	{
	    char buf[100];

	    wcstombs(buf, w->sj3.display_char, 100);
	    printf("Sj3: %s\n", buf);
	}
#endif
    } else {
	text.length = 0;
	text.feedback = w->sj3.feedback;
	text.string.wide_char = w->sj3.display_char;
    }
    text.encoding_is_wchar = TRUE;
    ConvDraw(XtParent((Widget)w), &text, 0);
}

static henkan_buf *
getkan(yomi)
    wchar_t *yomi;
{
    int i, l, k, n;
    unsigned char   *sj_yomi;
    unsigned char   *sj_knj;
    
    unsigned char   tmp[KNJSIZ];
    struct bunsetu  *sj_bun, *sj_bun_p;
    wchar_t         *yomi_p;
    wchar_t         *knj_p;
    henkan_buf *p;
    wc_douon_list *douon;
    
    /*  sj3_getkan をよぶためのバッファを割り当てる */
    
    l = wcslen(yomi);
    sj_yomi = (unsigned char *)XtMalloc(sizeof(wchar_t)*(l+1));
    sj_knj = (unsigned char *)XtMalloc(KNJSIZ);
    sj_bun = (struct bunsetu *)XtMalloc(sizeof(struct bunsetu)*l);
    
    wctosj(yomi, sj_yomi, sizeof(wchar_t)*(l+1));
    n = sj3_getkan(sj_yomi, sj_bun, sj_knj, KNJSIZ);
    
    sj_bun_p = sj_bun;
    yomi_p = yomi;
    
    p = (henkan_buf *)XtMalloc(sizeof(henkan_buf));
    douon = p->douon = (wc_douon_list *)XtMalloc(sizeof(wc_douon_list) * n);
    
    /* 文節単位の構造体に代入する */
    
    for (i = 0; i < n; i++) {
	douon->current = 0;
	
        strncpy(tmp, sj_bun_p->srcstr, sj_bun_p->srclen);
	tmp[sj_bun_p->srclen] ='\0';
	douon->sj_yomi = (unsigned char *)XtMalloc(sj_bun_p->srclen + 1);
	strcpy((char *)douon->sj_yomi, tmp);
	
	douon->yomi = yomi_p;
	douon->len = sjtowc(tmp, NULL, KNJSIZ);
        yomi_p += douon->len;
	
        strncpy(tmp, sj_bun_p->deststr, sj_bun_p->destlen);
	tmp[sj_bun_p->destlen] ='\0';
	douon->knjlen = sjtowc(tmp, NULL, KNJSIZ);
	douon->knj = (wchar_t *)XtMalloc(sizeof(wchar_t)*(douon->knjlen +1));
	sjtowc(tmp, douon->knj, douon->knjlen + 1);
	bcopy((char *)&sj_bun_p->dcid,
              (char *)&douon->dcid,
	      sizeof(struct studyrec));
	douon->n = 0;
	douon->dou = NULL;
	sj_bun_p++;
	douon++;
    }
    p->n = n;
    p->yomi = yomi;
    p->current = 0;
    XtFree((char *)sj_yomi);
    XtFree((char *)sj_knj);
    XtFree((char *)sj_bun);
    return(p);
}

static int
get_douon(p)
    wc_douon_list *p;
{
#define YOMI_MAX 128
    unsigned char tmp[YOMI_MAX];
    struct douon *sj_dou;
    wc_douon *wc_dou;
    int i, rc, n;
    
    strcpy(tmp, p->sj_yomi);
    n = sj3_douoncnt(tmp);
    if (n == 0) {
	return (1);
    }
    sj_dou = (struct douon *)XtMalloc(sizeof(struct douon) * n);
    rc = sj3_getdouon(tmp, sj_dou);
    if (rc == 0) {
        XtFree((char *)sj_dou);
	return (1);
    }
    wc_dou = (wc_douon *)XtMalloc(sizeof(wc_douon) * n);
    for (i = 0; i < n; i++) {
	sjtowc(sj_dou[i].ddata, wc_dou[i].ddata, 256);
	wc_dou[i].dlen = wcslen(wc_dou[i].ddata);
	bcopy((char *)&sj_dou[i].dcid,
	      (char *)&wc_dou[i].dcid,
	      sizeof(struct studyrec));
    }
    XtFree((char *)sj_dou);
    p->n = n;
    if (p->dou) {
	XtFree((char *)p->dou);
    }
    p->dou = wc_dou;
    return (0);
}

static void
douon_free(p)
    wc_douon_list *p;
{
    if (p->dou) {
        XtFree((char *)p->dou);
    }
    XtFree((char *)p->sj_yomi);
    XtFree((char *)p->knj);
    XtFree((char *)p);
}

static void
buf_free(p)
    henkan_buf *p;
{
    int i, n;
    
    n = p->n;
    for (i = 0; i < n; i++) {
	douon_free(p->douon[i]);
    }
}


/*
 * Class specific methods
 */

static void
Henkan(w, s)
    Sj3Object w;
    wchar_t *s;
{
    w->sj3.buf = getkan(s);
    do_display(w);
}

static void
Forward(w)
    Sj3Object w;
{
    henkan_buf *kekka;
    
    kekka = w->sj3.buf;
    if (kekka->current < kekka->n - 1) {
	kekka->current++;
    }
    do_display(w);
}

static void
Backward(w)
    Sj3Object w;
{
    henkan_buf *kekka;
    
    kekka = w->sj3.buf;
    if (kekka->current > 0) {
	kekka->current--;
    }
    do_display(w);
}

static void
Next(w)
    Sj3Object w;
{
    wc_douon_list *p;
    henkan_buf *kekka;
    
    kekka = w->sj3.buf;
    p = &kekka->douon[kekka->current];
    if (p->n == 0) {
	get_douon(p);
    }
    if (p->current < p->n - 1) {
	p->current++;
    }
    do_display(w);
}

static void
Previous(w)
    Sj3Object w;
{
    wc_douon_list *p;
    henkan_buf *kekka;
    
    kekka = w->sj3.buf;
    p = &kekka->douon[kekka->current];
    if (p->n == 0) {
	get_douon(p);
    }
    if (p->current > 0) {
	p->current--;
    }
    do_display(w);
}

static void
Cancel(w)
    Sj3Object w;
{
    buf_free(w->sj3.buf);
    w->sj3.buf = NULL;
    do_display(w);
}

static wchar_t *
Fix(w)
    Sj3Object w;
{
    int i, n, l;
    wchar_t *p, *s;
    wc_douon_list *list;
    int dl, j;
    
    if (!w->sj3.buf) {
	return((wchar_t *)NULL);
    }
    n = w->sj3.buf->n;
    list = w->sj3.buf->douon;
    
    l = 0;
    for (i = 0; i < n; i++) {
	if (!list->n) {
	    dl = list->knjlen;
	} else {
	    dl = list->dou[list->current].dlen;
	}
	l += dl;
	list++;
    }

    list = w->sj3.buf->douon;
    p = s = (wchar_t *)XtMalloc(sizeof(wchar_t)*(l+1));
    for (i = 0; i < n; i++) {
	if (!list->n) {
	    dl = list->knjlen;
	    bcopy((char *)list->knj, (char *)p, sizeof(wchar_t)*dl);
	} else {
	    dl = list->dou[list->current].dlen;
	    bcopy((char *)list->dou[list->current].ddata, (char *)p,
		  sizeof(wchar_t)*dl);
	}
	p += dl;
	list++;
    }
    *p = (wchar_t)'\0';
    buf_free(w->sj3.buf);
    w->sj3.buf = NULL;
    do_display(w);
    return(s);
}

void
KKHenkan(w, s)
    Sj3Object w;
    wchar_t *s;
{
    Sj3ObjectClass class = (Sj3ObjectClass)w->object.widget_class;

    (*class->sj3_class.Henkan)(w, s);
}

void
KKForward(w)
    Sj3Object w;
{
    Sj3ObjectClass class = (Sj3ObjectClass)w->object.widget_class;

    (*class->sj3_class.Forward)(w);
}

void
KKBackward(w)
    Sj3Object w;
{
    Sj3ObjectClass class = (Sj3ObjectClass)w->object.widget_class;

    (*class->sj3_class.Backward)(w);
}

void
KKNext(w)
    Sj3Object w;
{
    Sj3ObjectClass class = (Sj3ObjectClass)w->object.widget_class;

    (*class->sj3_class.Next)(w);
}

void
KKPrevious(w)
    Sj3Object w;
{
    Sj3ObjectClass class = (Sj3ObjectClass)w->object.widget_class;

    (*class->sj3_class.Previous)(w);
}

void
KKCancel(w)
    Sj3Object w;
{
    Sj3ObjectClass class = (Sj3ObjectClass)w->object.widget_class;

    (*class->sj3_class.Cancel)(w);
}

wchar_t *
KKFix(w)
    Sj3Object w;
{
    Sj3ObjectClass class = (Sj3ObjectClass)w->object.widget_class;

    return ((*class->sj3_class.Fix)(w));
}


