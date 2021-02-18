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
static char sccsid[] = "@(#)Context.c	1.2";
#endif
/*
 * XimpContex.c - XimpInputContextobject.
 *
 * @(#)Context.c	1.2 91/09/28 17:11:48
 */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/Xmu/Atoms.h>
#include "ContextP.h"
#include "Focus.h"
#include "Client.h"

extern root_preedit, root_status;

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffset(InputContextObject, inputContext.field)

static XtResource resources[] = {
    { XtNinputStyle, XtCInputStyle, XtRShort, sizeof(XIMStyle),
      offset(style), XtRImmediate, (XtPointer)0 },
};

static void Initialize();
static void Destroy();

static void Begin();
static void End();
static void SetFocus();
static void UnsetFocus();
static void XimpSetValues();
static void XimpGetValues();
static void Move();
static void Reset();
static void Key();
static void _Draw();
static void _Caret();
static void _StatusDraw();

#define SuperClass		(&conversionObjectClassRec)
InputContextClassRec inputContextClassRec = {
  {
/* core_class fields */	
    /* superclass	  	*/	(WidgetClass) SuperClass,
    /* class_name	  	*/	"InputContext",
    /* widget_size	  	*/	sizeof(InputContextRec),
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
/* ximpObjectclass fields */
  {
    /* Begin                    */      Begin,
    /* End                      */      End,
    /* SetFocus                 */      SetFocus,
    /* UnsetFocus               */      UnsetFocus,
    /* SetValue                 */      XimpSetValues,
    /* GetValue                 */      XimpGetValues,
    /* Move                     */      XtInheritMove,
    /* Reset                    */      Reset,
    /* Key                      */      XtInheritKey,
    /* Draw                     */      _Draw,
    /* Caret                    */      _Caret,
    /* StatusDraw               */      _StatusDraw,
  },
};

WidgetClass inputContextObjectClass = (WidgetClass)&inputContextClassRec;

/************************************************************
 *
 * Private
 *
 ************************************************************/

static void SetPreeditArg();
static void SetStatusArg();

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
    Widget request;
    Widget new;
    ArgList args;
    Cardinal *num_args;
{
    Widget focus;
    InputContextObject w;
    int lac;
    Arg largs[20];

    w = (InputContextObject)new;

    focus = w->conversion.focus_widget;
    switch (w->inputContext.style & 0xff) {
      case XIMPreeditPosition:
	lac = 0;
        SetPreeditArg(w, largs, &lac, (unsigned long)w->conversion.mask);
	XtSetValues(focus, largs, lac);
	break;
      case XIMPreeditNothing:
	break;
      default:
	;
    }
    switch (w->inputContext.style & 0xff00) {
      case XIMStatusArea:
        lac = 0;
        SetStatusArg(w, largs, &lac, w->conversion.mask);
        if (w->conversion.status_extension) {
	    XtSetValues(w->conversion.status_widget, largs, lac);
        } else {
	    XtSetValues(XtParent((Widget)w), largs, lac);
        }
	break;
      case XIMStatusNothing:
	break;
      default:
	;
    }
}

static void
Destroy(w)
    Widget w;
{
#ifdef DEBUG
    malloc_verify();
    printf("Context: destroy\n");
#endif
}

/**************************
 *
 * ximp 関係の処理
 *
 **************************/

static void
Begin(w, send)
    InputContextObject w;
    Boolean send;
{
    InputContextObjectClass sc, class;
    Widget client, focus, status;
    int ac;
    Arg args[10];

    class = (InputContextObjectClass)w->object.widget_class;
    sc = (InputContextObjectClass)class->object_class.superclass;
    (*sc->conversion_class.Begin)(w, TRUE);
    
    focus = w->conversion.focus_widget;
    switch (w->inputContext.style & 0xff) {
      case XIMPreeditPosition:
	ac = 0;
	XtSetArg(args[ac], XtNpreeditActive, TRUE); ac++;
	XtSetValues(focus, args, ac);
	break;
      case XIMPreeditCallbacks:
	ximp_preeditstart(XtDisplayOfObject(w),
			 w->conversion.focus_window,
			 w->conversion.icid);
	break;
      case XIMPreeditArea:
	break;
      case XIMPreeditNothing:
	break;
      default:
	break;
    }
    client = XtParent((Widget)w);
    status = w->conversion.status_widget;
    switch (w->inputContext.style & 0xff00) {
      case XIMStatusArea:
	ac = 0;
	XtSetArg(args[ac], XtNstatusActive, TRUE); ac++;
	if (w->conversion.status_extension) {
	    XtSetValues(status, args, ac);
	} else {
	    XtSetValues(client, args, ac);
	}
	break;
      case XIMStatusCallbacks:
	ximp_statusstart(XtDisplayOfObject(w),
			 w->conversion.focus_window,
			 w->conversion.icid);
	break;
      case XIMStatusNothing:
	break;
      default:
	break;
    }
    ConvStatusDraw(w);
}

static void
End(w, send)
    InputContextObject w;
    Boolean send;
{
    InputContextObjectClass sc;
    Widget client, focus, status;
    int ac;
    Arg args[10];

    client = XtParent((Widget)w);
    focus  = w->conversion.focus_widget;
    status = w->conversion.status_widget;
    switch (w->inputContext.style & 0xff) {
      case XIMPreeditPosition:
	ac = 0;
	XtSetArg(args[ac], XtNpreeditActive, FALSE); ac++;
	XtSetValues(focus, args, ac);
	break;
    case XIMPreeditCallbacks:
	ximp_preeditdone(XtDisplayOfObject(w),
			w->conversion.focus_window,
			w->conversion.icid);
	break;
      default:
	break;
    }
    switch (w->inputContext.style & 0xff00) {
      case XIMStatusArea:
	ac = 0;
	XtSetArg(args[ac], XtNstatusActive, FALSE); ac++;
	if (w->conversion.status_extension) {
	    XtSetValues(status, args, ac);
	} else {
	    XtSetValues(client, args, ac);
	}
	break;
      case XIMStatusCallbacks:
	ximp_statusdone(XtDisplayOfObject(w),
			w->conversion.focus_window,
			w->conversion.icid);
      case XIMStatusNothing:
      default:
	break;
    }
    sc = (InputContextObjectClass)w->object.widget_class->core_class.superclass;
    (*sc->conversion_class.End)(w, send);
}

static void
SetFocus(w)
    InputContextObject w;
{
    InputContextObjectClass sc, class;
    Widget client, focus, status;
    Arg args[10];
    int ac;

    class = (InputContextObjectClass)w->object.widget_class;
    sc = (InputContextObjectClass)class->object_class.superclass;
    (*sc->conversion_class.SetFocus)(w, TRUE);
    client = XtParent((Widget)w);
    focus = w->conversion.focus_widget;
    switch (w->inputContext.style & 0xff) {
      case XIMPreeditPosition:
	ac = 0;
	XtSetArg(args[ac], XtNpreeditActive, TRUE); ac++;
	XtSetValues(focus, args, ac);
	break;
      case XIMPreeditCallbacks:
	break;
      default:
	break;
    }
    status = w->conversion.status_widget;
    switch (w->inputContext.style & 0xff00) {
      case XIMStatusArea:
	if (w->conversion.in_conversion && w->conversion.focus) {
	    ac = 0;
	    XtSetArg(args[ac], XtNstatusActive, TRUE); ac++;
	    if (w->conversion.status_window) {
	        XtSetValues(status, args, ac);
	    } else {
	        XtSetValues(client, args, ac);
	    }
	}
	break;
      case XIMStatusCallbacks:
	break;
      case XIMStatusNothing:
	break;
      default:
	break;
    }
    ConvStatusDraw(w);
}

static void
UnsetFocus(w)
    InputContextObject w;
{
    InputContextObjectClass sc;
    Widget client, focus, status;
    Arg args[10];
    int ac;

    sc = (InputContextObjectClass)w->object.widget_class->core_class.superclass;
    (*sc->conversion_class.UnsetFocus)(w, TRUE);
    client = XtParent((Widget)w);
    focus = w->conversion.focus_widget;
    status = w->conversion.status_widget;
    switch (w->inputContext.style & 0xff) {
      case XIMPreeditPosition:
	ac = 0;
	XtSetArg(args[ac], XtNpreeditActive, FALSE); ac++;
	XtSetValues(focus, args, ac);
	break;
      case XIMPreeditCallbacks:
	break;
      default:
	break;
    }
    switch (w->inputContext.style & 0xff00) {
      case XIMStatusArea:
	if (!(w->conversion.in_conversion && w->conversion.focus)) {
	    ac = 0;
	    XtSetArg(args[ac], XtNstatusActive, FALSE); ac++;
	    if (w->conversion.status_window) {
	        XtSetValues(status, args, ac);
	    } else {
	        XtSetValues(client, args, ac);
	    }
	}
	break;
      case XIMStatusCallbacks:
	break;
      case XIMStatusNothing:
	break;
      default:
	break;
    }
}

static void
XimpSetValues(w, mask)
    InputContextObject w;
    unsigned long mask;
{
    InputContextObjectClass sc;
    Widget client, focus, status;
    int ac;
    Arg args[20];

    client = XtParent((Widget)w);
    status = w->conversion.status_widget;
    sc = (InputContextObjectClass)w->object.widget_class->core_class.superclass;
    (*sc->conversion_class.SetValue)(w, mask);
    focus  = w->conversion.focus_widget;
    switch (w->inputContext.style & 0xff) {
      case XIMPreeditPosition:
	if (mask & (XIMP_PROP_PREEDIT | XIMP_PROP_PREFONT)) {
	    ac = 0;
	    SetPreeditArg(w, args, &ac, mask);
	    XtSetValues(focus, args, ac);
        }
	break;
      case XIMPreeditCallbacks:
	break;
      default:
	;
    }
    switch (w->inputContext.style & 0xff00) {
      case XIMStatusArea:
    	if (mask & (XIMP_PROP_STATUS | XIMP_PROP_STSFONT)) {
	    ac = 0;
	    SetStatusArg(w, args, &ac, (unsigned long)w->conversion.mask);
	    if (w->conversion.status_extension) {
	        XtSetValues(status, args, ac);
	    } else {
	        XtSetValues(client, args, ac);
	    }
        }
	break;
      case XIMStatusCallbacks:
	break;
      case XIMStatusNothing:
	break;
      default:
	;
    }
}
static void
XimpGetValues(w, mask)
    InputContextObject w;
    unsigned long mask;
{
    InputContextObjectClass sc;
    Widget client, focus, status;
    Arg args[1];

    client = XtParent((Widget)w);
    focus  = w->conversion.focus_widget;
    status = w->conversion.status_widget;
    switch (w->inputContext.style & 0xff) {
      case XIMPreeditPosition:
        if (mask & (XIMP_PROP_PREEDIT | XIMP_PROP_PREFONT)) {
	    XtSetArg(args[0], XtNpreeditInfo, &w->conversion.preedit);
	    XtGetValues(focus, args, 1);
        }
	break;
      case XIMPreeditCallbacks:
	break;
      default:
	;
    }
    switch (w->inputContext.style & 0xff00) {
      case XIMStatusArea:
        if (mask & (XIMP_PROP_STATUS | XIMP_PROP_STSFONT)) {
	    XtSetArg(args[0], XtNstatusInfo, &w->conversion.status);
	    if (w->conversion.status_extension) {
	        XtGetValues(status, args, 1);
	    } else {
	        XtGetValues(client, args, 1);
	    }
	}
	break;
      case XIMStatusCallbacks:
	break;
      case XIMStatusNothing:
	break;
      default:
	;
    }
    sc = (InputContextObjectClass)w->object.widget_class->core_class.superclass;
    (*sc->conversion_class.GetValue)(w, mask);
}


static void
Reset(w)
    InputContextObject w;
{
    InputContextObjectClass sc;
    Widget focus, status;
    int ac;
    Arg args[10];

    focus = w->conversion.focus_widget;
    switch (w->inputContext.style & 0xff) {
      case XIMPreeditPosition:
	ac = 0;
	XtSetArg(args[ac], XtNpreeditActive, FALSE); ac++;
	XtSetValues(focus, args, ac);
	break;
      case XIMPreeditCallbacks:
	ximp_preeditdone(XtDisplayOfObject(w),
			w->conversion.focus_window,
			w->conversion.icid);
	break;
      default:
	break;
    }
    status = w->conversion.status_widget;
    switch (w->inputContext.style & 0xff00) {
      case XIMStatusArea:
	ac = 0;
	XtSetArg(args[ac], XtNstatusActive, FALSE); ac++;
	XtSetValues(status, args, ac);
	break;
      case XIMStatusCallbacks:
	ximp_statusdone(XtDisplayOfObject(w),
			w->conversion.focus_window,
			w->conversion.icid);
        break;
      case XIMStatusNothing:
        break;
      default:
        break;
    }
    sc = (InputContextObjectClass)w->object.widget_class->core_class.superclass;
    (*sc->conversion_class.Reset)(w);
}

static void
_Draw(w, text, cp)
    InputContextObject w;
    XIMText *text;
    int cp;
{
    ConversionObjectPart *xp;
    Widget focus;
    extern Atom AllocProp();
    ICID icid;
    Atom prop0, prop1, prop2;

    xp = &(w->conversion);
    focus = xp->focus_widget;
    icid = xp->icid;
    switch (w->inputContext.style & 0xff) {
      case XIMPreeditPosition:
	FocusPreeditDraw(focus, text, cp);
	break;
      case XIMPreeditCallbacks:
	{
	    int l;
	    unsigned char *ct;
	    unsigned long data[3];
	    Display *dpy;
	    Window server_window;
#ifdef DEBUG
	    int i;
#endif

	    dpy = XtDisplayOfObject(w);
	    server_window = w->conversion.server_window;

	    prop0 = AllocProp(icid);
	    prop1 = AllocProp(icid);
	    prop2 = AllocProp(icid);
	    l = 0;
	    Ximp_wcstoct(xp->ins_buf, xp->n_ins, NULL, &l, NULL);
	    ct = (unsigned char *)XtMalloc(l + 1);
	    l++;
	    Ximp_wcstoct(xp->ins_buf, xp->n_ins, ct, &l, NULL);
#ifdef DEBUG
	    printf("draw preedit l=%d\n", l);
	    for (i = 0; i < l; i++) {
		printf("%2x ", ct[i]);
	    }
	    printf("\n");
	    printf("caret = %d, chg_first = %d, chg_length = %d\n",
		   xp->caret, cp, xp->n_del);
#endif
	    data[0] = (long)xp->caret;
	    data[1] = (long)cp;
	    data[2] = (long)xp->n_del;
	    XChangeProperty(dpy, server_window, prop0, XIMP_PREEDIT_DRAW_DATA,
			    32, PropModeAppend, data, 3);
	    XChangeProperty(dpy, server_window, prop1, XA_COMPOUND_TEXT(dpy),
			    8, PropModeAppend, ct, l);
	    XChangeProperty(dpy, server_window, prop2, XIMP_FEEDBACKS,
			    8, PropModeAppend, &(xp->text.feedback[cp]), xp->n_ins);
	    ximp_preeditdraw(dpy, w->conversion.focus_window,
			    w->conversion.icid, prop0, prop1, prop2);
	    XtFree(ct);
	}
	break;
      case XIMPreeditNothing:
	break;
      default:
	break;
    }
}

/* ARGSUSED */
static void
_Caret(w, direction, style)
    InputContextObject w;
    XIMCaretDirection direction;
    XIMCaretStyle style;
{
    Widget focus;
    XIMPreeditCaretCallbackStruct cs;

    focus = w->conversion.focus_widget;
    switch (w->inputContext.style & 0xff) {
      case XIMPreeditPosition:
	switch (direction) {
	case XIMAbsolutePosition:
#ifdef DEBUG
	    printf("pos = %d\n", w->conversion.caret);
#endif
	    if (style == XIMIsInvisible) {
	        FocusPreeditCaret(focus, 0, style);
	    } else {
	        FocusPreeditCaret(focus, w->conversion.caret, style);
	    }
	default:
	;
	}
	break;
      case XIMPreeditCallbacks:
	break;
      default:
	break;
    }
    return;
}

static void
_StatusDraw(w)
    InputContextObject w;
{
    ConversionObjectClass sc;
    Widget client, status;
    XIMStatusDrawCallbackStruct *s;
    ICID icid;
    extern Atom AllocProp();
    Atom prop3, prop4;

    status = w->conversion.status_widget;
    s = &w->conversion.status_text;
    
    switch (w->inputContext.style & 0xff00) {
      case XIMStatusArea:
	client = XtParent((Widget)w);
	status = w->conversion.status_widget;
	if (w->conversion.status_extension) {
	    ClientStatusDraw(status, s);
	} else {
	    ClientStatusDraw(client, s);
	}
	break;
      case XIMStatusCallbacks:
	{
	    Display *dpy;
	    Window server_window;
	    int l;
	    unsigned char *ct;
#ifdef DEBUG
	    int i;
#endif

	    dpy = XtDisplayOfObject(w);
	    server_window = w->conversion.server_window;
	    icid  = w->conversion.icid;
	    prop3 = AllocProp(icid);
	    prop4 = AllocProp(icid);

	    l = 0;
	    Ximp_wcstoct(s->data.text->string.wide_char,
	    		 s->data.text->length, NULL, &l, NULL);
	    ct = (unsigned char *)XtMalloc(l + 1);
	    l++;
	    Ximp_wcstoct(s->data.text->string.wide_char,
	    		 s->data.text->length, ct, &l, NULL);
#ifdef DEBUG
	    printf("status draw l=%d\n", l);
	    for (i = 0; i < l; i++) {
		printf("%02x ", ct[i]);
	    }
	    printf("\n");
#endif
	    XChangeProperty(dpy, server_window, prop3, XA_COMPOUND_TEXT(dpy),
			    8, PropModeAppend, ct, l);
	    if (s->data.text->feedback) {
	        XChangeProperty(dpy, server_window, prop4, _XIMP_FEEDBACKS,
			        8, PropModeAppend, s->data.text->feedback,
			        s->data.text->length);
	    } else {
	        XChangeProperty(dpy, server_window, prop4, _XIMP_FEEDBACKS,
			        8, PropModeAppend, NULL, 0);
	    }
	    ximp_statusdraw(dpy, w->conversion.focus_window,
			   w->conversion.icid, XIMTextType, prop3, prop4);
	    XtFree(ct);
	}
	break;
      case XIMStatusNothing:
	break;
      default:
	break;
    }
}

static void
SetPreeditArg(w, args, acp, mask)
    ConversionObject w;
    Arg *args;
    int *acp;
    unsigned long mask;
{
    int ac;
    PreeditInfo *imcp = &w->conversion.preedit;

    ac = *acp;
    if (mask & XIMP_PRE_AREA_MASK) {
	XtSetArg(args[ac], XtNpreeditAreaX,        imcp->area.x);          ac++;
	XtSetArg(args[ac], XtNpreeditAreaY,        imcp->area.y);          ac++;
	XtSetArg(args[ac], XtNpreeditAreaWidth,    imcp->area.width);      ac++;
	XtSetArg(args[ac], XtNpreeditAreaHeight,   imcp->area.height);     ac++;
    }
    if (mask & XIMP_PRE_FG_MASK) {
	XtSetArg(args[ac], XtNpreeditForeground,   imcp->foreground );     ac++;
    }
    if (mask & XIMP_PRE_BG_MASK) {
	XtSetArg(args[ac], XtNpreeditBackground,   imcp->background);      ac++;
    }
    if (mask & XIMP_PRE_COLORMAP_MASK) {
	XtSetArg(args[ac], XtNpreeditColormap,     imcp->colormap);        ac++;
    }
    if (mask & XIMP_PRE_BGPIXMAP_MASK) {
	XtSetArg(args[ac], XtNpreeditBgPixmap,     imcp->background_pixmap); ac++;
    }
    if (mask & XIMP_PRE_LINESP_MASK) {
	XtSetArg(args[ac], XtNpreeditLineSpacing,  imcp->line_spacing);    ac++;
    }
    if (mask & XIMP_PRE_CURSOR_MASK) {
	XtSetArg(args[ac], XtNpreeditCursor,       imcp->cursor);          ac++;
    }
    if (mask & XIMP_PRE_AREANEED_MASK) {
	XtSetArg(args[ac], XtNpreeditNeededWidth,  imcp->areaneeded.width); ac++;
	XtSetArg(args[ac], XtNpreeditNeededHeight, imcp->areaneeded.height); ac++;
    }
    if (mask & XIMP_PRE_SPOTL_MASK) {
	XtSetArg(args[ac], XtNpreeditSpotX,        imcp->spot_location.x); ac++;
	XtSetArg(args[ac], XtNpreeditSpotY,        imcp->spot_location.y); ac++;
#ifdef DEBUG`
	printf("SetPreeditArg: x=%d,y=%d\n", imcp->spot_location.x, imcp->spot_location.y);
#endif
    }
    if (mask & XIMP_PRE_FONT_MASK) {
	XtSetArg(args[ac], XtNpreeditFont, w->conversion.preedit_font); ac++;
    }
    *acp = ac;
}

static void
SetStatusArg(w, args, acp, mask)
    ConversionObject w;
    Arg *args;
    int *acp;
    unsigned long mask;
{
    int ac;
    StatusInfo *imsp = &w->conversion.status;

    ac = *acp;
    if (mask & XIMP_STS_AREA_MASK) {
	XtSetArg(args[ac], XtNstatusAreaX,        imsp->area.x);          ac++;
	XtSetArg(args[ac], XtNstatusAreaY,        imsp->area.y);          ac++;
	XtSetArg(args[ac], XtNstatusAreaWidth,    imsp->area.width);      ac++;
	XtSetArg(args[ac], XtNstatusAreaHeight,   imsp->area.height);     ac++;
    }
    if (mask & XIMP_STS_FG_MASK) {
	XtSetArg(args[ac], XtNstatusForeground,   imsp->foreground );     ac++;
    }
    if (mask & XIMP_STS_BG_MASK) {
	XtSetArg(args[ac], XtNstatusBackground,   imsp->background);      ac++;
    }
    if (mask & XIMP_STS_COLORMAP_MASK) {
	XtSetArg(args[ac], XtNstatusColormap,     imsp->colormap);        ac++;
    }
    if (mask & XIMP_STS_BGPIXMAP_MASK) {
	XtSetArg(args[ac], XtNstatusBgPixmap,     imsp->background_pixmap); ac++;
    }
    if (mask & XIMP_STS_LINESP_MASK) {
	XtSetArg(args[ac], XtNstatusLineSpacing,  imsp->line_spacing);    ac++;
    }
    if (mask & XIMP_STS_CURSOR_MASK) {
	XtSetArg(args[ac], XtNstatusCursor,       imsp->cursor);          ac++;
    }
    if (mask & XIMP_STS_AREANEED_MASK) {
	XtSetArg(args[ac], XtNstatusNeededWidth,  imsp->areaneeded.width);  ac++;
	XtSetArg(args[ac], XtNstatusNeededHeight, imsp->areaneeded.height); ac++;
    }
    if (mask & XIMP_STS_FONT_MASK) {
	XtSetArg(args[ac], XtNstatusFont, w->conversion.status_font); ac++;
    }
    *acp = ac;
}

/************************************************************
 *
 * Class specific methods.
 *
 ************************************************************/

