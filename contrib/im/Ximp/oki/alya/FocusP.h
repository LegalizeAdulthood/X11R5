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
/*
 * FocusP.h - Private definitions for FocusWindowWidget
 *
 * @(#)1.2 91/09/28 17:12:03
 */

#ifndef _FocusP_h
#define _FocusP_h

/***********************************************************************
 *
 * FocusWindow Widget Private Data
 *
 ***********************************************************************/

#include "Focus.h"
#include <X11/CompositeP.h>
#include "Ximp.h"

/* New fields for the FocusWindow widget class record */
typedef struct {
    void (*focus_draw)();
    void (*focus_caret)();
} FocusWindowClassPart;

/* Full class record declaration */
typedef struct _FocusWindowClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    FocusWindowClassPart focusWindow_class;
} FocusWindowClassRec;

extern FocusWindowClassRec focusWindowClassRec;

/* New fields for the FocusWindow widget record */
typedef struct {
    /* resources */
    Window        window;
    Boolean       preedit_active;
    Boolean       input_active;
    Widget        conv_object;
    PreeditInfo   preedit;
    String        preedit_font;

    /* private state */
    Widget        preedit_widget1;
    Widget        preedit_widget2;
    int           wcs_len1;
    int           wcs_len2;
    unsigned long mask;
    XIMText       *preedit_text;
    int		  position;
    XIMCaretStyle style;
} FocusWindowPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _FocusWindowRec {
    CorePart	    core;
    CompositePart   composite;
    FocusWindowPart    focusWindow;
} FocusWindowRec;

typedef void (*_FocusWindowWidgetVoidFunc)();

#define XtInheritDraw  ((_FocusWindowWidgetVoidFunc)_XtInherit)
#define XtInheritCaret ((_FocusWindowWidgetVoidFunc)_XtInherit)

#endif /* _FocusP_h */
