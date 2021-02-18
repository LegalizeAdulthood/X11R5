/*
 * $id$
 */

/*
 * Copyright 1991 by OMRON Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of OMRON not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  OMRON makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * OMRON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * OMRON BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE. 
 *
 *	Author:	Seiji Kuwari	OMRON Corporation
 *				kuwa@omron.co.jp
 *				kuwa%omron.co.jp@uunet.uu.net
 */				

#if defined(IM)

#ifdef __STDC__
# include <stdarg.h>
# define Va_start(a,b) va_start(a,b)
#else
# include <varargs.h>
# define Va_start(a,b) va_start(a)
#endif
#include <X11/Xlocale.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/ShellP.h>
#include <X11/Vendor.h>
#include <X11/Xaw/MultiText.h>
#include <X11/Xaw/MultiSrc.h>
#include <X11/Xaw/MultiSink.h>
#include <X11/Xaw/TextP.h>
#include <X11/Xaw/XawImP.h>
#include <X11/Xaw/VendorEP.h>

#define maxAscentOfFontSet(fontset)     \
        ( - (XExtentsOfFontSet((fontset)))->max_logical_extent.y)

#define maxHeightOfFontSet(fontset) \
        ((XExtentsOfFontSet((fontset)))->max_logical_extent.height)

#define maxDescentOfFontSet(fontset) \
        (maxHeightOfFontSet(fontset) - maxAscentOfFontSet(fontset))

#define Offset(field) (XtOffsetOf(XawIcTablePart, field))

static XtResource resources[] =
{
    {
	XtNfontSet, XtCFontSet, XtRFontSet, sizeof(XFontSet),
	Offset (font_set), XtRString, XtDefaultFontSet
    },
    {
	XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	Offset (foreground), XtRString, (XtPointer)"XtDefaultForeground"
    },
    {
	XtNbackground, XtCBackground, XtRPixel, sizeof(Pixel),
	Offset (background), XtRString, (XtPointer)"XtDefaultBackground"
    },
    {
	XtNbackgroundPixmap, XtCPixmap, XtRPixmap, sizeof(Pixmap),
	Offset (bg_pixmap), XtRImmediate, (XtPointer) XtUnspecifiedPixmap
    },
    {
	XtNinsertPosition, XtCTextPosition, XtRInt, sizeof (XawTextPosition),
	Offset (cursor_position), XtRImmediate, (XtPointer) 0
    }
};
#undef Offset

static void _ImSetVaArg(arg, value)
XPointer *arg, value;
{
    *arg = value;
}

static VendorShellWidget _SearchVendorShell(w)
    Widget w;
{
    while(w && !XtIsShell(w)) w = XtParent(w);
    if (w && XtIsVendorShell(w)) return((VendorShellWidget)w);
    return(NULL);
}

static XContext extContext = NULL;

static XawVendorShellExtPart *_ImSetExtPart(w, vew)
    Widget	w;
    XawVendorShellExtWidget vew;
{
    contextDataRec *contextData;

    if (extContext == NULL)extContext = XUniqueContext();

    contextData = XtNew(contextDataRec);
    contextData->parent = w;
    contextData->ve = (Widget)vew;
    if (XSaveContext(XtDisplay(w), (Window)w, extContext, (char *)contextData)) {
	return(NULL);
    }
    return(&(vew->vendor_ext));
}

static XawVendorShellExtPart *_ImGetExtPart(w)
    Widget	w;
{
    contextDataRec *contextData;
    XawVendorShellExtWidget vew;

    if (XFindContext(XtDisplay(w), (Window)w, extContext,
		      (caddr_t*)&contextData)) {
	return(NULL);
    }
    vew = (XawVendorShellExtWidget)contextData->ve;
    return(&(vew->vendor_ext));
}

static XawIcTableList _ImGetIcTable(w, ve)
    Widget	w;
    XawVendorShellExtPart	*ve;
{
    XawIcTableList	p;

    for (p = ve->ic.ic_table; p; p = p->next) {
	if (p->widget == w)
	    return(p);
    }
    return(NULL);
}

static XIMStyle _ImGetImStyle(ve)
    XawVendorShellExtPart	*ve;
{
    if (!ve || !ve->im.preedit_type)
	 return((XIMPreeditNothing | XIMStatusNothing));
    if (!strcmp(ve->im.preedit_type, "OverTheSpot")) {
	return((XIMPreeditPosition | XIMStatusArea));
    } else if (!strcmp(ve->im.preedit_type, "OffTheSpot")) {
	return((XIMPreeditArea | XIMStatusArea));
    } else if (!strcmp(ve->im.preedit_type, "Root")) {
	return((XIMPreeditNothing | XIMStatusNothing));
    } else {
	return((XIMPreeditNothing | XIMStatusNothing));
    }
}

static void _ImConfigureCB(w, closure, event)
    Widget	w;
    Opaque	closure;
    XEvent	*event;
{
    XawIcTableList		p;
    XawVendorShellExtPart	*ve;
    VendorShellWidget		vw;
    XVaNestedList		pe_attr;
    XRectangle			pe_area;
    XawTextMargin		*margin;

    if (event->type != ConfigureNotify) return;

    if ((vw = _SearchVendorShell(w)) == NULL) return;

    if (ve = _ImGetExtPart(vw)) {
	if ((ve->im.xim == NULL) || ((p = _ImGetIcTable(w, ve)) == NULL) ||
	    (p->xic == NULL) || !(p->input_style & XIMPreeditPosition)) return;
	pe_area.x = 0;
        pe_area.y = 0;
        pe_area.width = w->core.width;
        pe_area.height = w->core.height;
	margin = &(((TextWidget)w)->text.margin);
	pe_area.x += margin->left;
	pe_area.y += margin->top;
	pe_area.width -= (margin->left + margin->right - 1);
	pe_area.height -= (margin->top + margin->bottom - 1);

	pe_attr = XVaCreateNestedList(0, XNArea, &pe_area, NULL);
	XSetICValues(p->xic, XNPreeditAttributes, pe_attr, NULL);
	XtFree(pe_attr);
    }
}

static XContext errContext = NULL;

static Widget _ImSetErrCnxt(w, xim)
    Widget	w;
    XIM xim;
{
    contextErrDataRec *contextErrData;

    if (errContext == NULL)errContext = XUniqueContext();

    contextErrData = XtNew(contextErrDataRec);
    contextErrData->widget = w;
    contextErrData->xim = xim;
    if (XSaveContext(XtDisplay(w), (Window)xim, errContext,
	(char *)contextErrData)) {
	return(NULL);
    }
    return(contextErrData->widget);
}

static Widget _ImGetErrCnxt(error_im)
    XIM	error_im;
{
    contextErrDataRec *contextErrData;

    if (XFindContext(XDisplayOfIM(error_im), (Window)error_im, errContext,
		      (caddr_t*)&contextErrData)) {
	return(NULL);
    }
    return(contextErrData->widget);
}

static void _ImCloseIM(ve)
    XawVendorShellExtPart	*ve;
{
    if (ve->im.xim)
	XCloseIM(ve->im.xim);
}

static int _ImIOErrorHandler(error_im)
XIM error_im;
{
    VendorShellWidget	vw;
    XawIcTableList	p;
    XawVendorShellExtPart	*ve;
    extern void XawVendorShellExtResize(), _ImDestroyIC();
    extern Dimension _ImSetVendorShellHeight();

    if ((vw = (VendorShellWidget)_ImGetErrCnxt(error_im)) == NULL
	|| (ve = _ImGetExtPart(vw)) == NULL) return(0);

    /*
     * Destory all ICs
     */
    for (p = ve->ic.ic_table; p; p = p->next) {
	if (p->xic == NULL) continue;
	_ImDestroyIC(p->widget, ve);
	p->xic = NULL;
	p->ic_focused = FALSE;
    }
    /*
     * Close Input Method
     */
    _ImCloseIM(ve);
    ve->im.xim = NULL;

    /*
     * resize vendor shell to core size
     */
    _ImSetVendorShellHeight(ve, 0);
    /*
    XawVendorShellExtResize(vw);
    */
    return(0);
}

/*
 * Attempt to open an input method
 */
static void _ImOpenIM(ve)
    XawVendorShellExtPart	*ve;
{
    char	*p, modifiers[32];

    if (!strcmp(setlocale(LC_ALL, NULL), "C")) return;
    if ((p = XSetLocaleModifiers(NULL)) == NULL || *p == '\0') {
	if (ve->im.input_method == NULL) return;
	strcpy(modifiers, "@im=");
	strcat(modifiers, ve->im.input_method);
	XSetLocaleModifiers(modifiers);
    }
    if ((ve->im.xim = XOpenIM(XtDisplay(ve->parent), NULL,
				     NULL, NULL)) == NULL) {
	XtWarning("we can not open any input method");
    } else {
	_ImSetErrCnxt(ve->parent, ve->im.xim);
	_XipSetIOErrorHandler(_ImIOErrorHandler);
    }
}

void _ImResizeVendorShell(vw, ve)
    VendorShellWidget		vw;
    XawVendorShellExtPart	*ve;
{
    XawIcTableList		p;
    XVaNestedList		pe_attr, st_attr;
    XRectangle			pe_area, st_area;

    for (p = ve->ic.ic_table; p; p = p->next) {
	if (p->xic == NULL) continue;
	st_area.width = 0;
	if (p->input_style & XIMStatusArea) {
	    st_attr = XVaCreateNestedList(0, XNArea, &st_area, NULL);
	    XGetICValues(p->xic, XNStatusAttributes, st_attr, NULL);
	    if (p->xic == NULL) {
		XtFree(st_attr);
		return;
	    }
	    st_area.y = vw->core.height - ve->im.area_height;
	    XSetICValues(p->xic, XNStatusAttributes, st_attr, NULL);
	    XtFree(st_attr);
	}
	if (p->xic == NULL) {
	    XtFree(st_attr);
	    return;
	}
	if (p->input_style & XIMPreeditArea) {
	    pe_attr = XVaCreateNestedList(0, XNArea, &pe_area, NULL);
	    XGetICValues(p->xic, XNPreeditAttributes, pe_attr, NULL);
	    if (p->xic == NULL) {
		XtFree(pe_attr);
		return;
	    }
	    pe_area.x = st_area.width;
	    pe_area.y = vw->core.height - ve->im.area_height;
	    pe_area.width = vw->core.width;
	    if (p->input_style & XIMStatusArea) {
		pe_area.width -= st_area.width;
	    }
	    XSetICValues(p->xic, XNPreeditAttributes, pe_attr, NULL);
	    XtFree(pe_attr);
	}
    }
}

void _XawImResizeVendorShell(vw)
    VendorShellWidget		vw;
{
    XawVendorShellExtPart	*ve;

    if ((ve = _ImGetExtPart(vw)) && ve->im.xim) {
	_ImResizeVendorShell(vw, ve);
    }
}

static void _ImRegisterToVendorShell(w, ve)
    Widget	w;
    XawVendorShellExtPart	*ve;
{
    XawIcTableList	table;

    table = (XawIcTableList) XtMalloc(sizeof(XawIcTablePart));
    table->widget = w;
    table->xic = NULL;
    table->input_style = _ImGetImStyle(ve);
    table->flg = table->setted_flg = 0;
    table->font_set = NULL;
    table->foreground = table->background = 0xffffffff;
    table->bg_pixmap = 0;
    table->cursor_position = 0xffff;
    table->line_spacing = 0;
    table->next = ve->ic.ic_table;
    ve->ic.ic_table = table;
}

static void _ImUnregisterFromVendorShell(w, ve)
    Widget	w;
    XawVendorShellExtPart	*ve;
{
    XawIcTableList	*prev, p;

    for (prev = &ve->ic.ic_table; p = *prev; prev = &p->next) {
	if (p->widget == w) {
	    *prev = p->next;
	    XtFree((char *)p);
	    break;
	}
    }
    return;
}

static Dimension _ImSetVendorShellHeight(ve, height)
    XawVendorShellExtPart	*ve;
    Dimension	height;
{
    Arg			args[2];
    Cardinal		i = 0;

   if (ve->im.area_height < height || height == 0) {
       XtSetArg(args[i], XtNheight,
		(ve->parent->core.height + height - ve->im.area_height));
       XtSetValues(ve->parent, args, 1);
       ve->im.area_height = height;
   }
   return(ve->im.area_height);
}

static Boolean _ImIsCreatedIC(w, ve)
    Widget	w;
    XawVendorShellExtPart	*ve;
{
    XawIcTableList	p;

    if (ve->im.xim == NULL) return(FALSE);
    if ((p = _ImGetIcTable(w, ve)) == NULL) return(FALSE);
    if (p->xic == NULL) return(FALSE);
    return(TRUE);
}
    
static void _ImCreateIC(w, ve)
    Widget	w;
    XawVendorShellExtPart	*ve;
{
    XawIcTableList	p;
    XPoint		position;
    XRectangle		pe_area, st_area, pe_area_needed, st_area_needed;
    XVaNestedList	pe_attr = NULL, st_attr = NULL;
    XPointer		ic_a[20], pe_a[20], st_a[20];
    Dimension		height = 0;
    int			ic_cnt = 0, pe_cnt = 0, st_cnt = 0;
    XawTextMargin	*margin;

    if (!XtIsRealized(w)) return;
    if ((p = _ImGetIcTable(w, ve)) == NULL) return;

    XFlush(XtDisplay(w));
    if (p->flg & CIFontSet) {
	_ImSetVaArg(&pe_a[pe_cnt], XNFontSet); pe_cnt++;
	_ImSetVaArg(&pe_a[pe_cnt], p->font_set); pe_cnt++;
	_ImSetVaArg(&st_a[st_cnt], XNFontSet); st_cnt++;
	_ImSetVaArg(&st_a[st_cnt], p->font_set); st_cnt++;
	height = maxAscentOfFontSet(p->font_set)
	  + maxDescentOfFontSet(p->font_set);
	height = _ImSetVendorShellHeight(ve, height);
    }
    if (p->flg & CIFg) {
	_ImSetVaArg(&pe_a[pe_cnt], XNForeground); pe_cnt++;
	_ImSetVaArg(&pe_a[pe_cnt], p->foreground); pe_cnt++;
	_ImSetVaArg(&st_a[st_cnt], XNForeground); st_cnt++;
	_ImSetVaArg(&st_a[st_cnt], p->foreground); st_cnt++;
    }
    if (p->flg & CIBg) {
	_ImSetVaArg(&pe_a[pe_cnt], XNBackground); pe_cnt++;
	_ImSetVaArg(&pe_a[pe_cnt], p->background); pe_cnt++;
	_ImSetVaArg(&st_a[st_cnt], XNBackground); st_cnt++;
	_ImSetVaArg(&st_a[st_cnt], p->background); st_cnt++;
    }
    if (p->flg & CIBgPixmap) {
	_ImSetVaArg(&pe_a[pe_cnt], XNBackgroundPixmap); pe_cnt++;
	_ImSetVaArg(&pe_a[pe_cnt], p->bg_pixmap); pe_cnt++;
	_ImSetVaArg(&st_a[st_cnt], XNBackgroundPixmap); st_cnt++;
	_ImSetVaArg(&st_a[st_cnt], p->bg_pixmap); st_cnt++;
    }
    if (p->flg & CILineS) {
	_ImSetVaArg(&pe_a[pe_cnt], XNLineSpace); pe_cnt++;
	_ImSetVaArg(&pe_a[pe_cnt], p->line_spacing); pe_cnt++;
	_ImSetVaArg(&st_a[st_cnt], XNLineSpace); st_cnt++;
	_ImSetVaArg(&st_a[st_cnt], p->line_spacing); st_cnt++;
    }
    if (p->input_style & XIMPreeditArea) {
	pe_area.x = 0;
	pe_area.y = ve->parent->core.height - height;
	pe_area.width = ve->parent->core.width;
	pe_area.height = height;
	_ImSetVaArg(&pe_a[pe_cnt], XNArea); pe_cnt++;
	_ImSetVaArg(&pe_a[pe_cnt], &pe_area); pe_cnt++;
    }
    if (p->input_style & XIMPreeditPosition) {
	pe_area.x = 0;
	pe_area.y = 0;
	pe_area.width = w->core.width;
	pe_area.height = w->core.height;
	margin = &(((TextWidget)w)->text.margin);
	pe_area.x += margin->left;
	pe_area.y += margin->top;
	pe_area.width -= (margin->left + margin->right - 1);
	pe_area.height -= (margin->top + margin->bottom - 1);
	_ImSetVaArg(&pe_a[pe_cnt], XNArea); pe_cnt++;
	_ImSetVaArg(&pe_a[pe_cnt], &pe_area); pe_cnt++;
	if (p->flg & CICursorP) {
	    _XawPosToXY(w, p->cursor_position, &position.x, &position.y);
	    _ImSetVaArg(&pe_a[pe_cnt], XNSpotLocation); pe_cnt++;
	    _ImSetVaArg(&pe_a[pe_cnt], &position); pe_cnt++;
	}
    }
    if (p->input_style & XIMStatusArea) {
	st_area.x = 0;
	st_area.y = ve->parent->core.height - height;
	st_area.width = ve->parent->core.width;
	st_area.height = height;
	_ImSetVaArg(&st_a[st_cnt], XNArea); st_cnt++;
	_ImSetVaArg(&st_a[st_cnt], &st_area); st_cnt++;
    }

    _ImSetVaArg(&ic_a[ic_cnt], XNInputStyle); ic_cnt++;
    _ImSetVaArg(&ic_a[ic_cnt], p->input_style); ic_cnt++;
    _ImSetVaArg(&ic_a[ic_cnt], XNClientWindow); ic_cnt++;
    _ImSetVaArg(&ic_a[ic_cnt], XtWindow(ve->parent)); ic_cnt++;
    _ImSetVaArg(&ic_a[ic_cnt], XNFocusWindow); ic_cnt++;
    _ImSetVaArg(&ic_a[ic_cnt], XtWindow(w)); ic_cnt++;

    if (pe_cnt > 0) {
	_ImSetVaArg(&pe_a[pe_cnt], NULL);
	pe_attr = XVaCreateNestedList(0, pe_a[0], pe_a[1], pe_a[2], pe_a[3],
				   pe_a[4], pe_a[5], pe_a[6], pe_a[7], pe_a[8],
				   pe_a[9], pe_a[10], pe_a[11], pe_a[12],
				   pe_a[13], pe_a[14], pe_a[15], pe_a[16],
				   pe_a[17], pe_a[18],  pe_a[19]);
	_ImSetVaArg(&ic_a[ic_cnt], XNPreeditAttributes); ic_cnt++;
	_ImSetVaArg(&ic_a[ic_cnt], pe_attr); ic_cnt++;
    }

    if (st_cnt > 0) {
	_ImSetVaArg(&st_a[st_cnt], NULL);
	st_attr = XVaCreateNestedList(0, st_a[0], st_a[1], st_a[2], st_a[3],
				   st_a[4], st_a[5], st_a[6], st_a[7], st_a[8],
				   st_a[9], st_a[10], st_a[11], st_a[12],
				   st_a[13], st_a[14], st_a[15], st_a[16],
				   st_a[17], st_a[18],  st_a[19]);
	_ImSetVaArg(&ic_a[ic_cnt], XNStatusAttributes); ic_cnt++;
	_ImSetVaArg(&ic_a[ic_cnt], st_attr); ic_cnt++;
    }
    _ImSetVaArg(&ic_a[ic_cnt], NULL);

    p->xic = XCreateIC(ve->im.xim, ic_a[0], ic_a[1], ic_a[2], ic_a[3],
		       ic_a[4], ic_a[5], ic_a[6], ic_a[7], ic_a[8], ic_a[9],
		       ic_a[10], ic_a[11], ic_a[12], ic_a[13], ic_a[14],
		       ic_a[15], ic_a[16], ic_a[17], ic_a[18], ic_a[19]);
    if (pe_attr) XtFree(pe_attr);
    if (st_attr) XtFree(st_attr);

    if (p->xic == NULL) return;

    pe_attr = st_attr = NULL;
    ic_cnt = pe_cnt = st_cnt = 0;

    if (p->input_style & XIMPreeditArea) {
	pe_attr = XVaCreateNestedList(0, XNAreaNeeded, &pe_area_needed, NULL);
	_ImSetVaArg(&ic_a[ic_cnt], XNPreeditAttributes); ic_cnt++;
	_ImSetVaArg(&ic_a[ic_cnt], pe_attr); ic_cnt++;
    }
    if (p->input_style & XIMStatusArea) {
	st_attr = XVaCreateNestedList(0, XNAreaNeeded, &st_area_needed, NULL);
	_ImSetVaArg(&ic_a[ic_cnt], XNStatusAttributes); ic_cnt++;
	_ImSetVaArg(&ic_a[ic_cnt], st_attr); ic_cnt++;
    }
    _ImSetVaArg(&ic_a[ic_cnt], NULL);

    if (ic_cnt > 0) {
	XGetICValues(p->xic, ic_a[0], ic_a[1], ic_a[2], ic_a[3], ic_a[4]);
	if (pe_attr) XtFree(pe_attr);
	if (st_attr) XtFree(st_attr);
	if (p->xic == NULL) {
	    return;
	}
	pe_attr = st_attr = NULL;
	ic_cnt = pe_cnt = st_cnt = 0;
	if (p->input_style & XIMStatusArea) {
	    st_area.height = st_area_needed.height;
	    if (p->input_style & XIMPreeditArea) {
		st_area.width = st_area_needed.width;
	    }
	    st_attr = XVaCreateNestedList(0, XNArea, &st_area, NULL);
	    _ImSetVaArg(&ic_a[ic_cnt], XNStatusAttributes); ic_cnt++;
	    _ImSetVaArg(&ic_a[ic_cnt], st_attr); ic_cnt++;
	}
	if (p->input_style & XIMPreeditArea) {
	    if (p->input_style & XIMStatusArea) {
		pe_area.x = st_area.width;
		pe_area.width -= st_area.width;
	    }
	    pe_area.height = pe_area_needed.height;
	    pe_attr = XVaCreateNestedList(0, XNArea, &pe_area, NULL);
	    _ImSetVaArg(&ic_a[ic_cnt], XNPreeditAttributes); ic_cnt++;
	    _ImSetVaArg(&ic_a[ic_cnt], pe_attr); ic_cnt++;
	}
	_ImSetVaArg(&ic_a[ic_cnt], NULL);
	XSetICValues(p->xic, ic_a[0], ic_a[1], ic_a[2], ic_a[3], ic_a[4]);
	if (pe_attr) XtFree(pe_attr);
	if (st_attr) XtFree(st_attr);
	if (p->xic == NULL) {
	    return;
	}
    }
    p->flg &= ~(CIFontSet | CIFg | CIBg | CIBgPixmap | CICursorP | CILineS);

    if (p->input_style & XIMPreeditPosition) {
	XtAddEventHandler(w, (EventMask)StructureNotifyMask, FALSE,
			  (XtEventHandler)_ImConfigureCB, (Opaque)NULL);
    }
}

static void _ImSetICValues(w, ve)
    Widget	w;
    XawVendorShellExtPart	*ve;
{
    XawIcTableList	p;
    XPoint		position;
    XVaNestedList	pe_attr = NULL, st_attr = NULL;
    XPointer		ic_a[20], pe_a[20], st_a[20];
    int			ic_cnt = 0, pe_cnt = 0, st_cnt = 0;
    int			height;

    if ((p = _ImGetIcTable(w, ve)) == NULL) return;

    XFlush(XtDisplay(w));
    if (!(p->flg & (CIFontSet | CIFg | CIBg |
		   CIBgPixmap | CICursorP | CILineS))) return;
#ifdef SPOT
    if ((p->input_style & XIMPreeditPosition)
	&& ((p->flg & ~CIICFocus) == CICursorP)) {
	_XawPosToXY(w, p->cursor_position, &position.x, &position.y);
	_XipChangeSpot(p->xic, position.x, position.y);
	p->flg &= ~CICursorP;
	return;
    }
#endif

    if (p->flg & CIFontSet) {
	_ImSetVaArg(&pe_a[pe_cnt], XNFontSet); pe_cnt++;
	_ImSetVaArg(&pe_a[pe_cnt], p->font_set); pe_cnt++;
	_ImSetVaArg(&st_a[st_cnt], XNFontSet); st_cnt++;
	_ImSetVaArg(&st_a[st_cnt], p->font_set); st_cnt++;
	height = maxAscentOfFontSet(p->font_set)
	  + maxDescentOfFontSet(p->font_set);
	height = _ImSetVendorShellHeight(ve, height);
    }
    if (p->flg & CIFg) {
	_ImSetVaArg(&pe_a[pe_cnt], XNForeground); pe_cnt++;
	_ImSetVaArg(&pe_a[pe_cnt], p->foreground); pe_cnt++;
	_ImSetVaArg(&st_a[st_cnt], XNForeground); st_cnt++;
	_ImSetVaArg(&st_a[st_cnt], p->foreground); st_cnt++;
    }
    if (p->flg & CIBg) {
	_ImSetVaArg(&pe_a[pe_cnt], XNBackground); pe_cnt++;
	_ImSetVaArg(&pe_a[pe_cnt], p->background); pe_cnt++;
	_ImSetVaArg(&st_a[st_cnt], XNBackground); st_cnt++;
	_ImSetVaArg(&st_a[st_cnt], p->background); st_cnt++;
    }
    if (p->flg & CIBgPixmap) {
	_ImSetVaArg(&pe_a[pe_cnt], XNBackgroundPixmap); pe_cnt++;
	_ImSetVaArg(&pe_a[pe_cnt], p->bg_pixmap); pe_cnt++;
	_ImSetVaArg(&st_a[st_cnt], XNBackgroundPixmap); st_cnt++;
	_ImSetVaArg(&st_a[st_cnt], p->bg_pixmap); st_cnt++;
    }
    if (p->flg & CILineS) {
	_ImSetVaArg(&pe_a[pe_cnt], XNLineSpace); pe_cnt++;
	_ImSetVaArg(&pe_a[pe_cnt], p->line_spacing); pe_cnt++;
	_ImSetVaArg(&st_a[st_cnt], XNLineSpace); st_cnt++;
	_ImSetVaArg(&st_a[st_cnt], p->line_spacing); st_cnt++;
    }
    if (p->input_style & XIMPreeditPosition) {
	if (p->flg & CICursorP) {
	    _XawPosToXY(w, p->cursor_position, &position.x, &position.y);
	    _ImSetVaArg(&pe_a[pe_cnt], XNSpotLocation); pe_cnt++;
	    _ImSetVaArg(&pe_a[pe_cnt], &position); pe_cnt++;
	}
    }

    if (pe_cnt > 0) {
	_ImSetVaArg(&pe_a[pe_cnt], NULL);
	pe_attr = XVaCreateNestedList(0, pe_a[0], pe_a[1], pe_a[2], pe_a[3],
				      pe_a[4], pe_a[5], pe_a[6], pe_a[7],
				      pe_a[8], pe_a[9], pe_a[10], pe_a[11],
				      pe_a[12], pe_a[13], pe_a[14], pe_a[15],
				      pe_a[16], pe_a[17], pe_a[18],  pe_a[19]);
	_ImSetVaArg(&ic_a[ic_cnt], XNPreeditAttributes); ic_cnt++;
	_ImSetVaArg(&ic_a[ic_cnt], pe_attr); ic_cnt++;
    }
    if (st_cnt > 0) {
	_ImSetVaArg(&st_a[st_cnt], NULL);
	st_attr = XVaCreateNestedList(0, st_a[0], st_a[1], st_a[2], st_a[3],
				      st_a[4], st_a[5], st_a[6], st_a[7],
				      st_a[8], st_a[9], st_a[10], st_a[11],
				      st_a[12], st_a[13], st_a[14], st_a[15],
				      st_a[16], st_a[17], st_a[18],  st_a[19]);
	_ImSetVaArg(&ic_a[ic_cnt], XNStatusAttributes); ic_cnt++;
	_ImSetVaArg(&ic_a[ic_cnt], st_attr); ic_cnt++;
    }
    if (ic_cnt > 0) {
	_ImSetVaArg(&ic_a[ic_cnt], NULL);
	XSetICValues(p->xic, ic_a[0], ic_a[1], ic_a[2], ic_a[3], ic_a[4],
		     ic_a[5], ic_a[6], ic_a[7], ic_a[8], ic_a[9], ic_a[10],
		     ic_a[11], ic_a[12], ic_a[13], ic_a[14], ic_a[15],
		     ic_a[16], ic_a[17], ic_a[18], ic_a[19]);
	if (pe_attr) XtFree(pe_attr);
	if (st_attr) XtFree(st_attr);
    }
    p->flg &= ~(CIFontSet | CIFg | CIBg | CIBgPixmap | CICursorP | CILineS);
}

static void _ImSetICFocus(w, ve)
    Widget		w;
    XawVendorShellExtPart	*ve;
{
    XawIcTableList	p;

    if ((p = _ImGetIcTable(w, ve)) == NULL) return;

    if (p->flg & CIICFocus && p->ic_focused == FALSE) {
	p->ic_focused = TRUE;
	XSetICFocus(p->xic);
    }
    p->flg &= ~CIICFocus;
}

static void _ImUnsetICFocus(w, ve)
    Widget		w;
    XawVendorShellExtPart	*ve;
{
    XawIcTableList	p;

    if ((p = _ImGetIcTable(w, ve)) == NULL) return;

    if (p->ic_focused == TRUE) {
	XUnsetICFocus(p->xic);
	p->ic_focused = FALSE;
    }
}

static void _ImSetValues(w, ve, args, num_args)
    Widget		w;
    XawVendorShellExtPart	*ve;
    ArgList		args;
    register Cardinal	num_args;
{
    register ArgList	arg;

    register XrmName	argName;
    register XrmResourceList	xrmres;
    register int	i;
    XawIcTablePart	*p, save_tbl;

    if ((p = _ImGetIcTable(w, ve)) == NULL) return;

    bcopy(p, &save_tbl, sizeof(XawIcTablePart));

    for (arg = args ; num_args != 0; num_args--, arg++) {
	argName = XrmStringToName(arg->name);
	for (xrmres = (XrmResourceList)ve->im.resources, i = 0;
	     i < ve->im.num_resources; i++, xrmres++) {
            if (argName == xrmres->xrm_name) {
                _XtCopyFromArg(arg->value,
			       (char *)p - xrmres->xrm_offset - 1,
			       xrmres->xrm_size);
                break;
            }
        }
    }
    if (p->input_style & (XIMPreeditPosition | XIMPreeditArea |
				     XIMStatusArea)) {
	if (p->font_set != save_tbl.font_set) {
	    p->flg |= CIFontSet;
	}
	if (p->foreground != save_tbl.foreground) {
	    p->flg |= CIFg;
	}
	if (p->background !=save_tbl.background) {
	    p->flg |= CIBg;
	}
	if (p->bg_pixmap != save_tbl.bg_pixmap) {
	    p->flg |= CIBgPixmap;
	}
	if (p->input_style & XIMPreeditPosition) {
	    if (p->cursor_position != save_tbl.cursor_position) {
		p->flg |= CICursorP;
	    }
	}
	if (p->line_spacing != save_tbl.line_spacing) {
	    p->flg |= CILineS;
	}
	p->setted_flg |= p->flg;
    }
}

static void _ImSetFocus(w, ve)
    Widget	w;
    XawVendorShellExtPart	*ve;
{
    XawIcTableList	p;
    if ((p = _ImGetIcTable(w, ve)) == NULL) return;

    if ( p->ic_focused == FALSE) {
	p->flg |= CIICFocus;
    }
    p->setted_flg |= p->flg;
}

static void _ImDestroyIC(w, ve)
    Widget	w;
    XawVendorShellExtPart	*ve;
{
    XawIcTableList	p;

    if ((ve->im.xim == NULL) || (p = _ImGetIcTable(w, ve)) == NULL) return;
    XDestroyIC(p->xic);
    if (p->input_style & XIMPreeditPosition) {
	XtRemoveEventHandler(w, (EventMask)StructureNotifyMask, FALSE,
			  (XtEventHandler)_ImConfigureCB, (Opaque)NULL);
    }
}

static void _ImSetFocusValues(inwidg, args, num_args, focus)
    Widget	inwidg;
    ArgList	args;
    Cardinal	num_args;
    Boolean	focus;
{
    XawVendorShellExtPart	*ve;
    VendorShellWidget		vw;

    if ((vw = _SearchVendorShell(inwidg)) == NULL) return;
    if (ve = _ImGetExtPart(vw)) {
	_ImSetValues(inwidg, ve, args, num_args);
	if (focus) _ImSetFocus(inwidg, ve);
	if (XtIsRealized((Widget)vw) && ve->im.xim) {
	    if (_ImIsCreatedIC(inwidg, ve)) {
		_ImSetICValues(inwidg, ve);
		if (focus) _ImSetICFocus(inwidg, ve);
	    } else {
		_ImCreateIC(inwidg, ve);
	    	_ImSetICFocus(inwidg, ve);
	    }
	}
    }
}

static void _ImUnsetFocus(inwidg)
    Widget	inwidg;
{
    XawVendorShellExtPart	*ve;
    VendorShellWidget		vw;
    XawIcTableList		p;

    if ((vw = _SearchVendorShell(inwidg)) == NULL) return;
    if (ve = _ImGetExtPart(vw)) {
	if ((p = _ImGetIcTable(inwidg, ve)) == NULL) return;
	if (p->flg & CIICFocus) {
	    p->flg &= ~CIICFocus;
	}
	p->setted_flg &= ~CIICFocus;
	if (ve->im.xim && XtIsRealized((Widget)vw) && p->xic) {
	    _ImUnsetICFocus(inwidg, ve);
	}
    }
}

static Boolean _ImIsRegistered(w, ve)
    Widget	w;
    XawVendorShellExtPart	*ve;
{
    XawIcTableList	p;

    for (p = ve->ic.ic_table; p; p = p->next)
	{
	    if (p->widget == w) return(TRUE);
	}
    return(FALSE);
}

static void _ImRegister(inwidg, ve)
    Widget	inwidg;
    XawVendorShellExtPart	*ve;
{
    if (ve->im.xim == NULL)
	{
	    _ImOpenIM(ve);
	}

    if (_ImIsRegistered(inwidg, ve)) return;

    _ImRegisterToVendorShell(inwidg, ve);

    if (ve->im.xim == NULL) return;

    if (XtIsRealized(ve->parent))
	{
	    _ImCreateIC(inwidg, ve);
	    _ImSetICFocus(inwidg, ve);
	}
}

static Boolean _ImNoRegistered(ve)
     XawVendorShellExtPart	*ve;
{
    if (ve->ic.ic_table == NULL) return(TRUE);
    return(FALSE);
}

static void _ImUnregister(inwidg, ve)
    Widget	inwidg;
    XawVendorShellExtPart	*ve;
{
    if (!_ImIsRegistered(inwidg, ve)) return;

    _ImDestroyIC(inwidg, ve);

    _ImUnregisterFromVendorShell(inwidg, ve);

    if (_ImNoRegistered(ve))
	{
	    _ImCloseIM(ve);
	}
}

static void _ImAllCreateIC(ve)
    XawVendorShellExtPart	*ve;
{
    register XawIcTableList	p;

    if (ve->im.xim == NULL) return;
    for (p = ve->ic.ic_table; p; p = p->next) {
	_ImCreateIC(p->widget, ve);
    }
    for (p = ve->ic.ic_table; p; p = p->next) {
	_ImSetICFocus(p->widget, ve);
    }
}

static void _ImReconnect(ve)
    XawVendorShellExtPart	*ve;
{
    register XawIcTableList	p;

    if (ve->im.xim == NULL) {
	_ImOpenIM(ve);
    } else {
	return;
    }
    if (ve->im.xim == NULL) return;

    for (p = ve->ic.ic_table; p; p = p->next) {
	p->flg = p->setted_flg;
    }
    _ImAllCreateIC(ve);
}

static void
_ImCompileResourceList(res, num_res)
    XtResourceList res;
    unsigned int	num_res;
{
    register unsigned int count;

#define xrmres	((XrmResourceList) res)
    for (count = 0; count < num_res; res++, count++) {
	xrmres->xrm_name         = XrmPermStringToQuark(res->resource_name);
	xrmres->xrm_class        = XrmPermStringToQuark(res->resource_class);
	xrmres->xrm_type         = XrmPermStringToQuark(res->resource_type);
	xrmres->xrm_offset	= -res->resource_offset - 1;
	xrmres->xrm_default_type = XrmPermStringToQuark(res->default_type);
    }
#undef xrmres
}

void _ImInitialize(vw, ve)
    VendorShellWidget		vw;
    XawVendorShellExtPart	*ve;
{
    if (!XtIsVendorShell((Widget)vw)) return;
    ve->parent = (Widget)vw;
    ve->im.xim = NULL;
    ve->im.area_height = 0;
    ve->im.resources = (XrmResourceList)XtMalloc(sizeof(resources));
    bcopy((char *)resources, (char *)ve->im.resources,
	  sizeof(resources));
    ve->im.num_resources = XtNumber(resources);
    _ImCompileResourceList(ve->im.resources,
			   ve->im.num_resources);
    ve->ic.ic_table = NULL;
}

Dimension _XawGetImShellHeight(vw)
    VendorShellWidget	vw;
{
    XawVendorShellExtPart	*ve;

    if (!XtIsVendorShell((Widget)vw)) return(vw->core.height);
    if (ve = _ImGetExtPart(vw)) {
	return(vw->core.height - ve->im.area_height);
    }
    return(vw->core.height);
}

void _XawImRealize(vw)
    VendorShellWidget	vw;
{
    XawVendorShellExtPart	*ve;
    extern void XawVendorShellExtResize();

    if (!XtIsRealized((Widget)vw) || !XtIsVendorShell((Widget)vw)) return;
    if (ve = _ImGetExtPart(vw)) {
	XtAddEventHandler((Widget)vw, (EventMask)StructureNotifyMask, FALSE,
			  XawVendorShellExtResize, (Opaque)NULL);
	_ImAllCreateIC(ve);
    }
}

void _XawImInitialize(vw, ext)
    VendorShellWidget	vw;
    XawVendorShellExtWidget ext;
{
    XawVendorShellExtPart	*ve;

    if (!XtIsVendorShell((Widget)vw)) return;
    if (ve = _ImSetExtPart(vw, ext)) {
	_ImInitialize(vw, ve);
    }
}

void _XawImReconnect(inwidg)
    Widget	inwidg;
{
    XawVendorShellExtPart	*ve;
    VendorShellWidget		vw;

    if ((vw = _SearchVendorShell(inwidg)) == NULL) return;
    if (ve = _ImGetExtPart(vw)) {
	_ImReconnect(ve);
    }
}

#if NeedFunctionPrototypes
void XawImRegister(Widget inwidg)
#else
void XawImRegister(inwidg)
    Widget	inwidg;
#endif
{
    XawVendorShellExtPart	*ve;
    VendorShellWidget		vw;

    if ((vw = _SearchVendorShell(inwidg)) == NULL) return;
    if (ve = _ImGetExtPart(vw)) {
	_ImRegister(inwidg, ve);
    }
}

#if NeedFunctionPrototypes
void XawImUnregister(Widget inwidg)
#else
void XawImUnregister(inwidg)
    Widget	inwidg;
#endif
{
    XawVendorShellExtPart	*ve;
    VendorShellWidget		vw;

    if ((vw = _SearchVendorShell(inwidg)) == NULL) return;
    if (ve = _ImGetExtPart(vw)) {
	_ImUnregister(inwidg, ve);
    }
}

#if NeedFunctionPrototypes
void XawImSetValues(Widget inwidg, ArgList args, Cardinal num_args)
#else
void XawImSetValues(inwidg, args, num_args)
    Widget	inwidg;
    ArgList	args;
    Cardinal	num_args;
#endif
{
    _ImSetFocusValues(inwidg, args, num_args, FALSE);
}

#if NeedFunctionPrototypes
void XawImVASetValues(Widget inwidg, ...)
#else
void XawImVASetValues(inwidg, va_alist)
    Widget	inwidg;
    va_dcl
#endif
{
    va_list		var;
    ArgList		args = NULL;
    Cardinal		num_args;
    int			total_count, typed_count;

    Va_start(var, inwidg);
    _XtCountVaList(var, &total_count, &typed_count);
    va_end(var);

    Va_start(var,inwidg);

    _XtVaToArgList(inwidg, var, total_count, &args, &num_args);
    XawImSetValues(inwidg, args, num_args);
    if (args != NULL) {
	XtFree((XtPointer)args);
    }
    va_end(var);
}

#if NeedFunctionPrototypes
void XawImSetFocusValues(Widget inwidg, ArgList args, Cardinal num_args)
#else
void XawImSetFocusValues(inwidg, args, num_args)
    Widget	inwidg;
    ArgList	args;
    Cardinal	num_args;
#endif
{
    _ImSetFocusValues(inwidg, args, num_args, TRUE);
}

#if NeedFunctionPrototypes
void XawImVASetFocusValues(Widget inwidg, ...)
#else
void XawImVASetFocusValues(inwidg, va_alist)
    Widget	inwidg;
    va_dcl
#endif
{
    va_list		var;
    ArgList		args = NULL;
    Cardinal		num_args;
    int			total_count, typed_count;

    Va_start(var, inwidg);
    _XtCountVaList(var, &total_count, &typed_count);
    va_end(var);

    Va_start(var,inwidg);

    _XtVaToArgList(inwidg, var, total_count, &args, &num_args);
    XawImSetFocusValues(inwidg, args, num_args);
    if (args != NULL) {
	XtFree((XtPointer)args);
    }
    va_end(var);
}

#if NeedFunctionPrototypes
void XawImUnsetFocus(Widget inwidg)
#else
void XawImUnsetFocus(inwidg)
    Widget	inwidg;
#endif
{
    _ImUnsetFocus(inwidg);
}

#if NeedFunctionPrototypes
int XawImWcLookupString(Widget inwidg, XKeyPressedEvent *event,
		        wchar *buffer_return, int bytes_buffer,
		        KeySym *keysym_return, Status *status_return)
#else
int XawImWcLookupString(inwidg, event, buffer_return, bytes_buffer,
		       keysym_return, status_return)
    Widget	inwidg;
    XKeyPressedEvent	*event;
    wchar	*buffer_return;
    int		bytes_buffer;
    KeySym	*keysym_return;
    Status	*status_return;
#endif
{
    XawVendorShellExtPart	*ve;
    VendorShellWidget		vw;
    XawIcTableList		p;
    int				i, ret;
    char			tmp_buf[64], *tmp_p;
    wchar			*buf_p;

    if ((vw = _SearchVendorShell(inwidg)) && (ve = _ImGetExtPart(vw)) &&
	ve->im.xim && (p = _ImGetIcTable(inwidg, ve)) && p->xic) {
	  return(XwcLookupString(p->xic, event, buffer_return, bytes_buffer,
				 keysym_return, status_return));
    }
    ret = XLookupString(event, tmp_buf, 64, keysym_return, (XComposeStatus *)status_return);
    for (i = 0, tmp_p = tmp_buf, buf_p = buffer_return; i < ret; i++) {
	*buf_p++ = atowc(*tmp_p++);
    }
    return(ret);
}

int _XawImGetVendorShellHeight(w)
    Widget w;
{
    XawVendorShellExtPart	*ve;
    VendorShellWidget		vw;

    if ((vw = _SearchVendorShell(w)) && (ve = _ImGetExtPart(vw))) {
	return(ve->im.area_height);
    }
    return(0);
}

void _XawImCallVendorShellExtResize(w)
    Widget w;
{
    XawVendorShellExtPart	*ve;
    VendorShellWidget		vw;
    extern void XawVendorShellExtResize();

    if ((vw = _SearchVendorShell(w)) && (ve = _ImGetExtPart(vw))) {
	XawVendorShellExtResize(vw);
    }
}
#endif	/* defined(IM) */
