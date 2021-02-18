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
static char sccsid[] = "@(#)Conversion.c	1.2";
#endif
/*
 * Conversion.c - Conversion object.
 *
 * @(#)Conversion.c	1.2 91/09/28 17:11:54
 */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/Xmu/Atoms.h>
#include "ConversioP.h"
#include "Focus.h"
#include "Client.h"
#include "Sj3.h"

#define INS_BUF_SIZE 10
#define PREEDIT_SIZE 160

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffset(ConversionObject, conversion.field)

static XtResource resources[] = {
    { XtNicid, XtCIcid, XtRInt, sizeof(int),
      offset(icid), XtRImmediate, (XtPointer)0 },
    { XtNwindow, XtCWindow, XtRWindow, sizeof(Window),
      offset(client_window), XtRImmediate, (XtPointer)NULL },
    { XtNserver, XtCWindow, XtRWindow, sizeof(Window),
      offset(server_window), XtRImmediate, (XtPointer)NULL },
    { XtNmask, XtCMask, XtRInt, sizeof(int),
      offset(mask), XtRImmediate, (XtPointer)0 },
};

static void ClassPartInitialize();
static void Initialize();
static void Destroy();

static void Begin();
static void End();
static void SetFocus();
static void UnsetFocus();
static void XimpSetValue();
static void XimpGetValue();
static void Move();
static void Reset();
static void Key();
static void _Draw();
static void _Caret();
static void _StatusDraw();

#define SuperClass		(&objectClassRec)
ConversionObjectClassRec conversionObjectClassRec = {
  {
/* core_class fields */	
    /* superclass	  	*/	(WidgetClass) SuperClass,
    /* class_name	  	*/	"ConversionObject",
    /* widget_size	  	*/	sizeof(ConversionObjectRec),
    /* class_initialize   	*/	NULL,
    /* class_part_initialize	*/	ClassPartInitialize,
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
/* conversion_class fields */
  {
    /* Begin                    */      Begin,
    /* End                      */      End,
    /* SetFocus                 */      SetFocus,
    /* UnsetFocus               */      UnsetFocus,
    /* SetValue                 */      XimpSetValue,
    /* GetValue                 */      XimpGetValue,
    /* Move                     */      Move,
    /* Reset                    */      Reset,
    /* Key                      */      Key,
    /* Draw                     */      _Draw,
    /* Caret                    */      _Caret,
    /* StatusDraw               */      _StatusDraw,
  }
};

WidgetClass conversionObjectClass = (WidgetClass)&conversionObjectClassRec;

static char *func_table[] = {
    "convert",				/* 変換 */
    "forward",                          /* 前に */
    "backward",				/* 後ろに */
    "next",
    "previous",
    "fix",				/* 確定 */
    "backspace",
    "delete",
    "carriage-return",                  /*  */
    "beep",				/* エラー */
    "end-conversion",			/* 変換終了 */
};

static void default_action();
static void insert_char();
static void delete_char();
static void func_dispatch();

static void convert();
static void forward();
static void backward();
static void next();
static void previous();
static void fix();
static void _fix();
static void backspace();
static void delete();
static void carriage();
static void beep();
static void endconv();

static void (*func_1[])() = {
    convert,
    forward,
    backward,
    next,
    previous,
    fix,
    backspace,
    delete,
    carriage,
    beep,
    endconv,
};

#define TABLE_SIZE	(sizeof(func_table) / sizeof(char *))

/************************************************************
 *
 * Private
 *
 ************************************************************/

static void GetPreeditProp();
static void GetFocusProp();
static void GetPreeditFontProp();
static void GetStatusProp();
static void GetStatusFontProp();

static void
ClassPartInitialize(wc)
WidgetClass wc;
{
    ConversionObjectClass conversionwc, sc;

    conversionwc = (ConversionObjectClass) wc;
    sc = (ConversionObjectClass) conversionwc->object_class.superclass;

    if (conversionwc->conversion_class.Begin == XtInheritBegin) {
	conversionwc->conversion_class.Begin = sc->conversion_class.Begin;
    }
    if (conversionwc->conversion_class.End == XtInheritEnd) {
	conversionwc->conversion_class.End = sc->conversion_class.End;
    }
    if (conversionwc->conversion_class.SetFocus== XtInheritSetFocus) {
	conversionwc->conversion_class.SetFocus = sc->conversion_class.SetFocus;
    }
    if (conversionwc->conversion_class.UnsetFocus == XtInheritUnsetFocus) {
	conversionwc->conversion_class.UnsetFocus = sc->conversion_class.UnsetFocus;
    }
    if (conversionwc->conversion_class.SetValue == XtInheritXimpSetValue) {
	conversionwc->conversion_class.SetValue = sc->conversion_class.SetValue;
    }
    if (conversionwc->conversion_class.GetValue == XtInheritXimpGetValue) {
	conversionwc->conversion_class.GetValue = sc->conversion_class.GetValue;
    }
    if (conversionwc->conversion_class.Move == XtInheritMove) {
	conversionwc->conversion_class.Move = sc->conversion_class.Move;
    }
    if (conversionwc->conversion_class.Reset == XtInheritReset) {
	conversionwc->conversion_class.Reset = sc->conversion_class.Reset;
    }
    if (conversionwc->conversion_class.Key == XtInheritKey) {
	conversionwc->conversion_class.Key = sc->conversion_class.Key;
    }
    if (conversionwc->conversion_class.Draw == XtInheritDraw) {
	conversionwc->conversion_class.Draw = sc->conversion_class.Draw;
    }
    if (conversionwc->conversion_class.Caret == XtInheritCaret) {
	conversionwc->conversion_class.Caret = sc->conversion_class.Caret;
    }
    if (conversionwc->conversion_class.StatusDraw == XtInheritStatusDraw) {
	conversionwc->conversion_class.StatusDraw = sc->conversion_class.StatusDraw;
    }
}

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
    Widget request;
    Widget new;
    ArgList args;
    Cardinal *num_args;
{
    ConversionObject imnew;
    Window win;
    ConversionObjectPart *ipart;
    unsigned long mask;
    extern char *getenv();
    Arg largs[20];
    int lac;

    imnew = (ConversionObject)new;
    ipart = &(imnew->conversion);
/*
 *  とりあえずは front-end モードだけ
 */
    ipart->input_mode = XIMP_FRONTEND;

    win = ipart->client_window;
    ipart->focus_window = win;
    ipart->status_extension = FALSE;
    ipart->status_window = (Window) NULL;

    ipart->focus_widget = (Widget)NULL;
    ipart->status_widget = (Widget)NULL;

    ipart->preedit_font = (char *)NULL;
    ipart->status_font = (char *)NULL;
    ipart->in_conversion = FALSE;
    ipart->focus = FALSE;
/*
 * ここでかな漢字変換関係の初期化を行う。
 */
    ipart->ccrule = NULL;
    ipart->ccdeffile = NULL;

    if (ipart->ccrule == NULL) {
	if (ipart->ccdeffile == NULL) {
	    ipart->ccdeffile = getenv("CC_DEF");
	    if (ipart->ccdeffile == NULL) {
		ipart->ccdeffile = DEF_CCDEF_FILE;
	    }
	}
	ipart->ccrule = ccParseRule(ipart->ccdeffile, XtWarning);
    }
    
    ipart->ccbuf = ccCreateBuf(ipart->ccrule, 10,
			       func_table, TABLE_SIZE,
			       default_action, insert_char, delete_char,
			       func_dispatch, NULL, (caddr_t)imnew);

/*
 *
 */
    ipart->state = 0;
    ipart->mode = ccGetMode(ipart->ccbuf);
    ipart->caret = 0;
    ipart->ins_buf = (wchar_t *)XtMalloc(sizeof(wchar_t)*INS_BUF_SIZE);
    ipart->text.length = 0;
    ipart->text.feedback = (XIMFeedback *)XtMalloc(sizeof(XIMFeedback)*(PREEDIT_SIZE + 1));
    bzero((char *)ipart->text.feedback, sizeof(XIMFeedback)*(PREEDIT_SIZE + 1));
    ipart->text.encoding_is_wchar = TRUE;
    ipart->text.string.wide_char = (wchar_t *)XtMalloc(sizeof(wchar_t)*(PREEDIT_SIZE) + 1);

/*
 *
 */
    ipart->status_text.type = XIMTextType;
    ipart->status_text.data.text = (XIMText *)XtMalloc(sizeof(XIMText));
    ipart->status_text.data.text->encoding_is_wchar = TRUE;
    ipart->status_text.data.text->length            = 0;
    ipart->status_text.data.text->string.wide_char  = (wchar_t *)XtMalloc(sizeof(wchar_t)*PREEDIT_SIZE);
    ipart->status_text.data.text->feedback          = NULL;

/*
 *
 */

    lac = 0;
    ipart->cv = XtCreateWidget("sj3", sj3ObjectClass, new, largs, lac);

/*
 *
 */

    mask = (unsigned long)ipart->mask;
    if (mask & XIMP_PROP_PREEDIT) {
        GetPreeditProp(imnew, win, mask);
    }
    if (mask & XIMP_PROP_FOCUS) {
        GetPreeditFontProp(imnew, win);
    }
    if (mask & XIMP_PROP_STATUS) {
        GetStatusProp(imnew, win, mask);
    }
    if (mask & XIMP_PROP_STSFONT) {
        GetStatusFontProp(imnew, win);
    }
    if (mask & XIMP_PROP_FOCUS) {
        GetFocusProp(imnew, win);
    }
    ximp_create_return(XtDisplayOfObject(imnew), ipart->focus_window, ipart->icid);
}

static void
GetFocusProp(imw,win)
    ConversionObject imw;
    Window win;
{
    Atom type;
    long format, nitems, byte_after;
    long *prop;

    if (XGetWindowProperty(XtDisplayOfObject(imw), win, XIMP_FOCUS,
			   0L, 1L, True, XA_WINDOW, 
			   &type, &format, &nitems, 
			   &byte_after, (char **)&prop) == Success) {
	if ((Window)*prop != (Window)(NULL)) {
	    imw->conversion.focus_window = (Window)*prop;
	    if (XtWindowToWidget(XtDisplayOfObject(imw),
			         imw->conversion.focus_window) == NULL) {
		Arg largs[20];
		int ac;

		ac = 0;
		XtSetArg(largs[ac], XtNwindow, imw->conversion.focus_window); ac++;
		imw->conversion.focus_widget = XtCreateWidget("focus", focusWindowWidgetClass,
					            XtParent((Widget)imw), largs, ac);
		XtRealizeWidget(imw->conversion.focus_widget);
	    }
	}
	Xfree(prop);
    }
}

static void
GetPreeditProp(imw, win, mask)
    ConversionObject imw;
    Window win;
    unsigned long mask;
{
    Atom type;
    long format, nitems, byte_after;
    long *prop;
    PreeditInfo *imcp = &(imw->conversion.preedit);

    if (XGetWindowProperty(XtDisplayOfObject(imw), win, XIMP_PREEDIT,
			   0L, 14L, True, XIMP_PREEDIT, 
			   &type, &format, &nitems, 
			   &byte_after, (char **)&prop) == Success) {
	if (mask & XIMP_PRE_AREA_MASK) {
	    imcp->area.x            = (short)prop[0];
	    imcp->area.y            = (short)prop[1];
	    imcp->area.width        = (unsigned short)prop[2];
	    imcp->area.height       = (unsigned short)prop[3];
#ifdef DEBUG
	    printf("preedit area = + %d + %d  %d X %d\n", prop[0], prop[1], prop[2], prop[3]);
#endif
	}
	if (mask & XIMP_PRE_FG_MASK) {
	    imcp->foreground        = (unsigned long)prop[4];
	}
	if (mask & XIMP_PRE_BG_MASK) {
	    imcp->background        = (unsigned long)prop[5];
	}
	if (mask & XIMP_PRE_COLORMAP_MASK) {
	    imcp->colormap          = (Colormap)prop[6];
	}
	if (mask & XIMP_PRE_BGPIXMAP_MASK) {
	    imcp->background_pixmap = (unsigned long)prop[7];
	}
	if (mask & XIMP_PRE_BG_MASK) {
	    imcp->line_spacing      = (int)prop[8];
	}
	if (mask & XIMP_PRE_LINESP_MASK) {
	    imcp->cursor            = (Cursor)prop[9];
	}
	if (mask & XIMP_PRE_AREANEED_MASK) {
	    imcp->areaneeded.width  = (unsigned short)prop[10];
	    imcp->areaneeded.height = (unsigned short)prop[11];
#ifdef DEBUG
	    printf("preedit areaneeded =%d X %d\n", prop[10], prop[11]);
#endif
	}
	if (mask & XIMP_PRE_SPOTL_MASK) {
	    imcp->spot_location.x   = (short)prop[12];
	    imcp->spot_location.y   = (short)prop[13];
#ifdef DEBUG
	    printf("preedit spot location =%d %d\n", prop[12], prop[13]);
#endif
	}
	Xfree((char *)prop);
    }
}

static void
GetPreeditFontProp(imw,win)
    ConversionObject imw;
    Window win;
{
    Atom type;
    long format, nitems, byte_after;
    char *prop;

    if (XGetWindowProperty(XtDisplayOfObject(imw), win, XIMP_PREEDITFONT,
			   0L, 1000L, True, XA_STRING, 
			   &type, &format, &nitems, 
			   &byte_after, &prop) == Success) {
	if (imw->conversion.preedit_font != (char *)NULL) {
	    Xfree(imw->conversion.preedit_font);
	}
	imw->conversion.preedit_font = prop;
#ifdef DEBUG
	printf("preedit font = %s\n", prop);
#endif
    }
}

static void
GetStatusProp(imw, win, mask)
    ConversionObject imw;
    Window win;
    unsigned long mask;
{
    Atom type;
    long format, nitems, byte_after;
    long *prop;
    StatusInfo *imcp = &(imw->conversion.status);

    if (XGetWindowProperty(XtDisplayOfObject(imw), win, XIMP_STATUS,
			   0L, 13L, True, XIMP_STATUS, 
			   &type, &format, &nitems, 
			   &byte_after, (char **)&prop) == Success) {
	if (mask & XIMP_STS_AREA_MASK) {
	    imcp->area.x            = (short)prop[0];
	    imcp->area.y            = (short)prop[1];
	    imcp->area.width        = (unsigned short)prop[2];
	    imcp->area.height       = (unsigned short)prop[3];
#ifdef DEBUG
	    printf("status area = + %d + %d  %d X %d\n", prop[0], prop[1], prop[2], prop[3]);
#endif
	}
	if (mask & XIMP_STS_FG_MASK) {
	    imcp->foreground        = (unsigned long)prop[4];
#ifdef DEBUG
	    printf("status fg = %d\n", prop[4]);
#endif
	}
	if (mask & XIMP_STS_BG_MASK) {
	    imcp->background        = (unsigned long)prop[5];
#ifdef DEBUG
	    printf("status bg = %d\n", prop[5]);
#endif
	}
	if (mask & XIMP_STS_COLORMAP_MASK) {
	    imcp->colormap          = (Colormap)prop[6];
	}
	if (mask & XIMP_STS_BGPIXMAP_MASK) {
	    imcp->background_pixmap = (unsigned long)prop[7];
	}
	if (mask & XIMP_STS_LINESP_MASK) {
	    imcp->line_spacing      = (int)prop[8];
	}
	if (mask & XIMP_STS_CURSOR_MASK) {
	    imcp->cursor            = (Cursor)prop[9];
	}
	if (mask & XIMP_STS_AREANEED_MASK) {
	    imcp->areaneeded.width  = (unsigned short)prop[10];
	    imcp->areaneeded.height = (unsigned short)prop[11];
#ifdef DEBUG
	    printf("status areaneeded =%d X %d\n", prop[10], prop[11]);
#endif
	}
	if (mask & XIMP_STS_WINDOW_MASK) {
	    imcp->status_window     = (Window)prop[12];
	}
	Xfree((char *)prop);
    }
}

static void
GetStatusFontProp(imw, win)
    ConversionObject imw;
    Window win;
{
    Atom type;
    long format, nitems, byte_after;
    char *prop;

    if (XGetWindowProperty(XtDisplayOfObject(imw), win, XIMP_STATUSFONT,
			   0L, 1000L, True, XA_STRING, 
			   &type, &format, &nitems,
			   &byte_after, &prop) == Success) {
	if (imw->conversion.status_font != (char *)NULL) {
	    Xfree(imw->conversion.status_font);
	}
	imw->conversion.status_font = prop;
#ifdef DEBUG
	printf("status font = %s\n", prop);
#endif
    }
}

/* set */

static void
SetFocusProp(imw, win)
    ConversionObject imw;
    Window win;
{
    long prop[1];

    prop[0] = imw->conversion.focus_window;
    XChangeProperty(XtDisplayOfObject(imw), win, XIMP_FOCUS,
		    XA_WINDOW, 32, PropModeReplace,
		    (unsigned char *)prop, XtNumber(prop));
}

static void
SetPreeditProp(imw, win)
    ConversionObject imw;
    Window win;
{
    long prop[14];
    PreeditInfo *imcp = &(imw->conversion.preedit);

    prop[0] = (long)imcp->area.x;
    prop[1] = (long)imcp->area.y;
    prop[2] = (long)imcp->area.width;
    prop[3] = (long)imcp->area.height;
    prop[4] = (long)imcp->foreground;
    prop[5] = (long)imcp->background;
    prop[6] = (long)imcp->colormap;
    prop[7] = (long)imcp->background_pixmap;
    prop[8] = (long)imcp->line_spacing;
    prop[9] = (long)imcp->cursor;
    prop[10] = (long)imcp->areaneeded.width;
    prop[11] = (long)imcp->areaneeded.height;
    prop[12] = (long)imcp->spot_location.x;
    prop[13] = (long)imcp->spot_location.y;
    XChangeProperty(XtDisplayOfObject(imw), win, XIMP_PREEDIT,
		    XIMP_PREEDIT, 32, PropModeReplace, 
		    (unsigned char *)prop, XtNumber(prop));
}

static void
SetPreeditFontProp(imw, win)
    ConversionObject imw;
    Window win;
{
    XChangeProperty(XtDisplayOfObject(imw), win,
		    XIMP_PREEDITFONT, XA_STRING, 8, PropModeReplace,
		    (unsigned char *)imw->conversion.preedit_font,
		    strlen(imw->conversion.preedit_font));
}

static void
SetStatusProp(imw, win)
    ConversionObject imw;
    Window win;
{
    long prop[13];
    StatusInfo *imcp = &(imw->conversion.status);

    prop[0] = (long)imcp->area.x;
    prop[1] = (long)imcp->area.y;
    prop[2] = (long)imcp->area.width;
    prop[3] = (long)imcp->area.height;
    prop[4] = (long)imcp->foreground;
    prop[5] = (long)imcp->background;
    prop[6] = (long)imcp->colormap;
    prop[7] = (long)imcp->background_pixmap;
    prop[8] = (long)imcp->line_spacing;
    prop[9] = (long)imcp->cursor;
    prop[10] = (long)imcp->areaneeded.width;
    prop[11] = (long)imcp->areaneeded.height;
    prop[12] = (long)imcp->status_window;
    XChangeProperty(XtDisplayOfObject(imw), win, XIMP_STATUS,
		    XIMP_STATUS, 32, PropModeReplace,
		    (unsigned char *)prop, XtNumber(prop));
}

static void
SetStatusFontProp(imw, win)
    ConversionObject imw;
    Window win;
{
    XChangeProperty(XtDisplayOfObject(imw), win,
		    XIMP_STATUSFONT, XA_STRING, 8, PropModeReplace,
		    (unsigned char *)imw->conversion.preedit_font,
		    strlen(imw->conversion.preedit_font));
}

static void
Destroy(w)
    Widget w;
{
    ConversionObject imw = (ConversionObject)w;
#ifdef DEBUG
    printf("Conversion: Destroy\n");
#endif
/*
 * かな漢字変換の後処理
 */

    ccDestroyBuf(imw->conversion.ccbuf);
    if (imw->conversion.ccrule) {
	ccFreeRule(imw->conversion.ccrule);
    }
    XtFree((char *)imw->conversion.status_text.data.text->string.wide_char);
    XtFree((char *)imw->conversion.status_text.data.text);
    XtFree((char *)imw->conversion.ins_buf);
    XtFree((char *)imw->conversion.text.feedback);
    XtFree((char *)imw->conversion.text.string.wide_char);
#ifdef DEBUG
    printf("Conversion: Destroy end\n");
#endif
}

/**************************
 *
 * conversion actions
 *
 **************************/

static void startAction();
static void endAction();

static int wcslen();

static int
wcslen(s)
    wchar_t *s;
{
    int n;

    n = 0;
    while (*s++) {
	n++;
    }
    return(n);
}

static void
startAction(w, event)
    ConversionObject w;
    XEvent *event;
{
    ConversionObjectPart *conversionpart = &(w->conversion);

    conversionpart->n_ins = 0;
    conversionpart->n_del = 0;
    conversionpart->old_caret = conversionpart->caret;
    conversionpart->event = event;
}

static void
endAction(w)
    ConversionObject w;
{
    ConversionObjectPart *conversionpart = &(w->conversion);
    ConversionObjectClass class;
    int mode;

    class = (ConversionObjectClass)w->object.widget_class;

    if ( conversionpart->n_ins != 0 || conversionpart->n_del != 0) {
	int cp = conversionpart->caret;
	int nd = conversionpart->n_del;
	int ni = conversionpart->n_ins;
        int delta, len;
	int i, j;
        wchar_t *buf;
	XIMFeedback *feedback;

        delta = ni - nd;
        buf = conversionpart->text.string.wide_char;
        feedback = conversionpart->text.feedback;

	conversionpart->ins_buf[ni] = (wchar_t)0;
	len = conversionpart->text.length - cp;
        if (cp + delta + len > PREEDIT_SIZE) {
	    len = PREEDIT_SIZE - delta - cp;
        }
	if (len > 0) {
	    bcopy((char *)&buf[cp + nd],
		  (char *)&buf[cp + ni],
		  sizeof(wchar_t)*len);
	    bcopy((char *)&feedback[cp + nd],
		  (char *)&feedback[cp + ni],
		  sizeof(XIMFeedback)*len);
	}
	bcopy((char *)(conversionpart->ins_buf),
	      (char *)&buf[cp],
	      sizeof(wchar_t)*ni);
	for (i = cp, j=0; j < ni; i++, j++) {
	      feedback[i] = XIMUnderline;
	} 
	conversionpart->caret += ni;
	conversionpart->text.length += delta;
        if (XtIsSubclass(w, conversionObjectClass)) {
#ifdef DEBUG
	    int i;

	    printf("draw cp=%d nd=%d ni=%d delta= %d\n", cp, nd, ni, delta);
	    printf("length =%d ", conversionpart->text.length);
	    for (i=0; i<conversionpart->text.length; i++) {
		printf(" %04x", conversionpart->text.string.wide_char[i]);
	    }
	    printf(":\n");
#endif
	    if (conversionpart->state == 0) {
		ConvCaret(w, XIMAbsolutePosition, XIMIsPrimary);
	        ConvDraw(w, &conversionpart->text, cp);
	    } else {
	    }
	}
    } else if (conversionpart->old_caret != conversionpart->caret) {
	if (conversionpart->state == 0) {
	    ConvCaret(w, XIMAbsolutePosition, XIMIsPrimary);
	    ConvDraw(w, &conversionpart->text, conversionpart->caret);
	} else {
	}
    }
    mode = ccGetMode(conversionpart->ccbuf);
    if (mode != conversionpart->mode) {
	wchar_t *s;
	XIMText *t;
	conversionpart->mode = mode;

	t = conversionpart->status_text.data.text;
	s = ccGetModePrompt(conversionpart->ccbuf);
	t->length = wcslen(s);
	bcopy((char *)s, (char *)t->string.wide_char, sizeof(wchar_t)*t->length);
	ConvStatusDraw(w);
    }
}

static void
default_action(s, n, client_data)
    unsigned char *s;
    int n;
    caddr_t client_data;
{
    ConversionObject w = (ConversionObject)client_data;

    ximp_keypress(XtDisplayOfObject(w), w->conversion.focus_window, 
		  w->conversion.icid, w->conversion.event);
}

static void
beep(w)
    ConversionObject w;
{
    XBell(XtDisplayOfObject(w), 20);
}

static void
insert_char(c, client_data)
    wchar_t c;
    caddr_t client_data;
{
    ConversionObject w = (ConversionObject)client_data;
    ConversionObjectPart *conversionpart = &(w->conversion);

    if (conversionpart->state != 0) {
	fix(client_data);
	conversionpart->state = 0;
    }	
    if (conversionpart->n_ins < INS_BUF_SIZE ) {
	conversionpart->ins_buf[conversionpart->n_ins] = c;
	(conversionpart->n_ins)++;
    } else {
	beep(w);
    }
}

static void
delete_char(client_data)
    caddr_t client_data;
{
    ConversionObject w = (ConversionObject)client_data;
    ConversionObjectPart *conversionpart = &(w->conversion);

    if (conversionpart->state != 0) {
	fix(client_data);
	conversionpart->state = 0;
    }	
    if (conversionpart->n_ins != 0) {
	(conversionpart->n_ins)--;
    } else if (conversionpart->n_del < conversionpart->text.length) {
	(conversionpart->n_del)++;
	if (conversionpart->caret > 0) {
	    (conversionpart->caret)--;
	}
    }
    ccContextDelete(conversionpart->ccbuf);
}

static void
convert(client_data)
    caddr_t client_data;
{
    ConversionObject w = (ConversionObject)client_data;
    int l;
    wchar_t *wcs;
    XIMText text;

    if (w->conversion.state == 0) {
	w->conversion.state = 1;
	l = w->conversion.text.length;
	wcs = w->conversion.text.string.wide_char;
	wcs[l] = (wchar_t)'\0';
	text.length = 0;
	text.feedback = (XIMFeedback *)NULL;
	text.encoding_is_wchar = TRUE;
	text.string.wide_char = (wchar_t *)NULL;
	ConvCaret(w, XIMAbsolutePosition, XIMIsInvisible);
	ConvDraw(w, &text, 0);
	KKHenkan(w->conversion.cv, wcs);
    }else {
	KKNext(w->conversion.cv);
    }
}

static void
forward(client_data)
    caddr_t client_data;
{
    ConversionObject w = (ConversionObject)client_data;
    ConversionObjectPart *conversionpart = &(w->conversion);

    if (w->conversion.state == 0) {
    ccContextClear(w->conversion.ccbuf);
    if (conversionpart->caret < conversionpart->text.length) {
	(conversionpart->caret)++;
    }
    } else {
	KKForward(w->conversion.cv);
    }
}

static void
backward(client_data)
    caddr_t client_data;
{
    ConversionObject w = (ConversionObject)client_data;
    ConversionObjectPart *conversionpart = &(w->conversion);

    if (w->conversion.state == 0) {
    ccContextClear(w->conversion.ccbuf);
    if (conversionpart->caret > 0) {
	(conversionpart->caret)--;
    }
    } else {
	KKBackward(w->conversion.cv);
    }
}

static void
next(client_data)
    caddr_t client_data;
{
    ConversionObject w = (ConversionObject)client_data;

    if (w->conversion.state == 0) {
    } else {
	KKNext(w->conversion.cv);
    }
}

static void
previous(client_data)
    caddr_t client_data;
{
    ConversionObject w = (ConversionObject)client_data;

    if (w->conversion.state == 0) {
    } else {
	KKPrevious(w->conversion.cv);
    }
}

static void
fix(client_data)
    caddr_t client_data;
{
    ConversionObject w = (ConversionObject)client_data;
    ConversionObjectClass class;
    int l;
    wchar_t *wcs;

    if (w->conversion.state == 0) {
	w->conversion.caret = 0;
	l = w->conversion.text.length;
	wcs = w->conversion.text.string.wide_char;
	wcs[l] = (wchar_t)NULL;
	_fix(w, wcs);
	ccContextClear(w->conversion.ccbuf);
	w->conversion.text.length = 0;
    } else {
	w->conversion.caret = 0;
	w->conversion.text.length = 0;
	w->conversion.state = 0;
        wcs = KKFix(w->conversion.cv);
        _fix(w, wcs);
	XtFree((char *)wcs);
    }
    ConvCaret(w, XIMAbsolutePosition, XIMIsPrimary);
    ConvDraw(w, &w->conversion.text, 0);
}

static void
_fix(w, wcs)
    ConversionObject w;
    wchar_t *wcs;
{
    ConversionObjectClass class;
    int l;
    unsigned char buf[1000];
    int n;
    Atom prop;
#ifdef DEBUG
    int i;
#endif

    class = (ConversionObjectClass)w->object.widget_class;
    n = 1000;
    l = wcslen(wcs);
    Ximp_wcstoct(wcs, l, buf, &n, NULL);
#ifdef DEBUG
    printf("fix buf =");
    for (i=0; i<n; i++) {
	printf(" %02x", buf[i]);
    }
    printf(":\n");
#endif
    if (n > MESSAGE_MAX_SIZE) {
	prop = AllocProp(w->conversion.icid);
	ximp_readprop(XtDisplayOfObject(w),
		      w->conversion.focus_window,
		      w->conversion.icid,
		      prop,
		      w->conversion.server_window,
		      buf,
		      n);
    } else {
	ximp_read(XtDisplayOfObject(w),
		  w->conversion.focus_window,
		  w->conversion.icid, buf, n);
    }
}

static void
backspace(client_data)
    caddr_t client_data;
{
    ConversionObject w = (ConversionObject)client_data;
    ConversionObjectPart *conversionpart = &(w->conversion);

    if (conversionpart->n_del < conversionpart->text.length) {
	(conversionpart->n_del)++;
	(conversionpart->caret)--;
    }
    ccContextDelete(conversionpart->ccbuf);
}

static void
delete(client_data)
    caddr_t client_data;
{
    ConversionObject w = (ConversionObject)client_data;
    ConversionObjectPart *conversionpart = &(w->conversion);

    if ((conversionpart->caret + conversionpart->n_del) < conversionpart->text.length) {
	(conversionpart->n_del)++;
    }
    ccContextDelete(conversionpart->ccbuf);
}

static void
carriage(client_data)
    caddr_t client_data;
{
    ConversionObject w = (ConversionObject)client_data;

    if (w->conversion.text.length == 0) {
	ximp_keypress(XtDisplayOfObject(w), w->conversion.focus_window, 
		      w->conversion.icid, w->conversion.event);
    } else {
	fix((caddr_t)w);
    }
}

static void
endconv(client_data)
    caddr_t client_data;
{
    ConversionObject w = (ConversionObject)client_data;
    ConversionObjectClass class;

    class = (ConversionObjectClass)w->object.widget_class;
    (*class->conversion_class.End)(w, TRUE);
}


/* ARGSUSED */
static void
func_dispatch(func, str, nbytes, w)
    int func;
    unsigned char *str;
    int nbytes;
    ConversionObject w;
{
    (*func_1[func])(w);
}

/**************************
 *
 * Ximp protocol の処理
 *
 **************************/

static void
Begin(w, send)
    ConversionObject w;
    Boolean send;
{
    ConversionObjectClass class;
    Widget focus, client;
    XIMText text;
    int ac;
    Arg args[20];
    wchar_t *s;

    class = (ConversionObjectClass)w->object.widget_class;
    focus = XtWindowToWidget(XtDisplayOfObject(w), w->conversion.focus_window);
    client = XtParent((Widget)w);
    ac = 0;
    if (w->conversion.input_mode == XIMP_FRONTEND) {
	XtSetArg(args[ac], XtNinputActive, TRUE); ac++;
    }

    XtSetArg(args[ac], XtNconvObject, w); ac++;
    XtSetValues(focus, args, ac);

/*
    if (send) {
	ximp_process_begin(XtDisplayOfObject(w), w->conversion.focus_window,
			     w->conversion.icid);
    }
*/
    w->conversion.focus_widget = focus;
    w->conversion.in_conversion = TRUE;
    w->conversion.focus = TRUE;
    {
	XIMText *t;

	t = w->conversion.status_text.data.text;
	w->conversion.status_text.type = XIMTextType;
	s = ccGetModePrompt(w->conversion.ccbuf);
	t->length = wcslen(s);
	bcopy((char *)s, (char *)t->string.wide_char, sizeof(wchar_t)*t->length);
	ConvStatusDraw(w);
    }
    ConvCaret(w, XIMAbsolutePosition, XIMIsPrimary);
    ConvDraw(w, &w->conversion.text, 0);
}

static void
End(w, send)
    ConversionObject w;
    Boolean send;
{
    Widget focus;
    int ac;
    Arg args[10];

    focus = w->conversion.focus_widget;
    ac = 0;
    if (w->conversion.input_mode == XIMP_FRONTEND) {
	XtSetArg(args[ac], XtNinputActive, FALSE) ; ac++;
    }
    XtSetArg(args[ac], XtNconvObject, NULL) ; ac++;
    XtSetValues(focus, args, ac);
    if (send) {
	ximp_process_end(XtDisplayOfObject(w), w->conversion.focus_window,
		         w->conversion.icid);
    }
    w->conversion.in_conversion = FALSE;
    w->conversion.focus = FALSE;
}

static void
SetFocus(w)
    ConversionObject w;
{
    w->conversion.focus = TRUE;
    return;
}

static void
UnsetFocus(w)
    ConversionObject w;
{
    w->conversion.focus = FALSE;
    return;
}

static void
XimpSetValue(w, mask)
    ConversionObject w;
    unsigned long mask;
{
    ConversionObjectClass class;
    ConversionObject imw = (ConversionObject)w;
    Window win = XtWindowOfObject(imw);
    unsigned long m;

    m = (unsigned long)imw->conversion.mask | mask;
    imw->conversion.mask = (int)m;
    if (mask & XIMP_PROP_FOCUS) {
	if (w->conversion.in_conversion) {
	    class = (ConversionObjectClass)imw->object.widget_class;
	    (*class->conversion_class.End)(imw, TRUE);
	    GetFocusProp(imw, win);
	    (*class->conversion_class.Begin)(imw, TRUE);
	} else {
	    GetFocusProp(imw, win);
	}
    } 
    if (mask & XIMP_PROP_PREEDIT) {
	GetPreeditProp(imw, win, mask);
    }
    if (mask & XIMP_PROP_PREFONT) {
	GetPreeditFontProp(imw, win);
    }
    if (mask & XIMP_PROP_STATUS) {
        GetStatusProp(imw, win, mask);
    }
    if (mask & XIMP_PROP_STSFONT) {
	GetStatusFontProp(imw, win);
    }
}

static void
XimpGetValue(w, mask)
    ConversionObject w;
    unsigned long mask;
{
    if (mask & XIMP_PROP_FOCUS) {
	SetFocusProp(w, w->conversion.client_window);
    }
    if (mask & XIMP_PROP_PREEDIT) {
#ifdef DEBUG
	PreeditInfo *p = &(w->conversion.preedit);

	if (mask & XIMP_PRE_AREA_MASK) {
	    printf("preedit area = + %d + %d %d X %d\n",
		   p->area.x, p->area.y,
		   p->area.width, p->area.height);
	}
	if (mask & XIMP_PRE_AREANEED_MASK) {
	    printf("preedit areaneeded = %d X %d\n",
		    p->areaneeded.width, p->areaneeded.height);
	}
#endif
	SetPreeditProp(w, w->conversion.client_window);
    }
    if (mask & XIMP_PROP_PREFONT) {
	SetPreeditFontProp(w, w->conversion.client_window);
    }
    if (mask & XIMP_PROP_STATUS) {
#ifdef DEBUG
	StatusInfo *p = &(w->conversion.status);

	if (mask & XIMP_STS_AREA_MASK) {
	    printf("status area = + %d + %d %d X %d\n",
		   p->area.x, p->area.y,
		   p->area.width, p->area.height);
	}
	if (mask & XIMP_STS_AREANEED_MASK) {
	    printf("status areaneeded = %d X %d\n",
		    p->areaneeded.width, p->areaneeded.height);
	}
#endif
	SetStatusProp(w, w->conversion.client_window);
    }
    if (mask & XIMP_PROP_STSFONT) {
	SetStatusFontProp(w, w->conversion.client_window);
    }
    ximp_getvalue_return(XtDisplayOfObject(w), XtWindowOfObject(w),
			 w->conversion.icid);
}

static void
Move(w, x, y)
    ConversionObject w;
    Position x, y;
{
    Widget focus;
    int ac;
    Arg args[10];

    focus = w->conversion.focus_widget;
    w->conversion.preedit.spot_location.x = x;
    w->conversion.preedit.spot_location.y = y;
    ac = 0;
    XtSetArg(args[ac], XtNpreeditSpotX, x); ac++;
    XtSetArg(args[ac], XtNpreeditSpotY, y); ac++;
    XtSetValues(focus, args, ac);
    return;
}

static void
Reset(w)
    ConversionObject w;
{
    Widget focus;
    int ac;
    Arg args[10];

    focus = w->conversion.focus_widget;
    if (w->conversion.input_mode == XIMP_FRONTEND) {
	ac = 0;
	XtSetArg(args[ac], XtNinputActive, FALSE) ; ac++;
	XtSetValues(focus, args, ac);
    }
    ximp_process_end(XtDisplayOfObject(w), w->conversion.focus_window,
		       w->conversion.icid);
    w->conversion.focus_widget = (Widget)NULL;
    w->conversion.in_conversion = FALSE;
}

static void
Key(w, event)
    ConversionObject w;
    XEvent *event;
{
    startAction(w, event);
    (void)ccConvchar(w->conversion.ccbuf, event);
    endAction(w);
}

static void
_Draw(w, text, cp)
    ConversionObject w;
    XIMText *text;
    int cp;
{
}

static void
_Caret(w, direction, style)
    ConversionObject w;
    XIMCaretDirection direction;
    XIMCaretStyle style;
{
}

static void
_StatusDraw(w)
    ConversionObject w;
{
}

/************************************************************
 *
 * 
 *
 ************************************************************/

/*
 * XimpKey -
 *
 *    Key event の処理
 */

void
XimpKey(w, event)
    ConversionObject w;
    XEvent *event;
{
    ConversionObjectClass class = (ConversionObjectClass) w->object.widget_class;

    if (XtIsSubclass(w, conversionObjectClass)) {
	(*class->conversion_class.Key)(w, event);
    }
}

/*
 * XimpMessage -
 *
 *    Client Message event の振り分け処理
 */

void
XimpMessage(imw, request, p1, p2, event)
    ConversionObject imw;
    int request;
    long  p1, p2;
    XEvent *event;
{
    ConversionObjectClass class = (ConversionObjectClass)imw->object.widget_class;
    ICID icid;
    
    if (!XtIsSubclass(imw, conversionObjectClass)) {
	return;
    }
    switch (request) {
      case XIMP_DESTROY:
#ifdef DEBUG
	printf("<Destroy>\n");
#endif
	XtDestroyWidget(imw);
	break;
      case XIMP_BEGIN:
#ifdef DEBUG
	printf("<Begin>\n");
#endif
	(*class->conversion_class.Begin)(imw, TRUE);
	break;
      case XIMP_END:
#ifdef DEBUG
	printf("<End>\n");
#endif
	(*class->conversion_class.End)(imw, TRUE);
	break;
      case XIMP_SETFOCUS:
#ifdef DEBUG
	printf("<SetFocus>\n");
#endif
	(*class->conversion_class.SetFocus)(imw);
	break;
      case XIMP_UNSETFOCUS:
#ifdef DEBUG
	printf("<UnsetFocus>\n");
#endif
	(*class->conversion_class.UnsetFocus)(imw);
	break;
      case XIMP_SETVALUE:
#ifdef DEBUG
	printf("<SetValue>\n");
#endif
	(*class->conversion_class.SetValue)(imw, (unsigned long)p1);
	break;
      case XIMP_GETVALUE:
#ifdef DEBUG
	printf("<GetValue>\n");
#endif
	(*class->conversion_class.GetValue)(imw, (unsigned long)p1);
	break;
      case XIMP_MOVE:
#ifdef DEBUG
	printf("<Move>\n");
#endif
	(*class->conversion_class.Move)(imw, (Position)p1, (Position)p2);
	break;
      case XIMP_RESET:
#ifdef DEBUG
	printf("<Reset>\n");
#endif
	(*class->conversion_class.Reset)(imw);
	break;
      case XIMP_KEYPRESS:
#ifdef DEBUG
	printf("<KeyPress>\n");
#endif
	{
	    XEvent key;

	    key.xkey.type        = KeyPress;
	    key.xkey.serial      = event->xclient.serial;
	    key.xkey.send_event  = FALSE;
	    key.xkey.display     = event->xclient.display;
	    key.xkey.window      = event->xclient.window;
	    key.xkey.root        = DefaultRootWindow(key.xkey.display);
	    key.xkey.subwindow   = (Window)NULL;
	    key.xkey.time        = CurrentTime;
	    key.xkey.x           = 0;
	    key.xkey.y           = 0;
	    key.xkey.x_root      = 0;
	    key.xkey.y_root      = 0;
	    key.xkey.state       = p1;
	    key.xkey.keycode     = p2;
	    key.xkey.same_screen = TRUE;
	    XimpKey(imw, &key);
	}
        break;
      case XIMP_EXTENSION:
	break;
      default:
	break;
    }
}

void
ConvDraw(w, text, cp)
    ConversionObject w;
    XIMText *text;
    int cp;
{
    ConversionObjectClass class = (ConversionObjectClass) w->object.widget_class;

    if (XtIsSubclass(w, conversionObjectClass)) {
	(*class->conversion_class.Draw)(w, text, cp);
    }
}

void
ConvCaret(w, direction, style)
    ConversionObject w;
    XIMCaretDirection direction;
    XIMCaretStyle style;
{
    ConversionObjectClass class = (ConversionObjectClass) w->object.widget_class;

    if (XtIsSubclass(w, conversionObjectClass)) {
	(*class->conversion_class.Caret)(w, direction, style);
    }
}

void
ConvStatusDraw(w)
    ConversionObject w;
{
    ConversionObjectClass class = (ConversionObjectClass) w->object.widget_class;

    if (XtIsSubclass(w, conversionObjectClass)) {
	(*class->conversion_class.StatusDraw)(w);
    }
}
