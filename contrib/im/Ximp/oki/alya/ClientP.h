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
 *     Copyright (c) 1991 Oki Techosystems Laboratory, Inc.
 *     Copyright (c) 1991 Oki Electric Industry Co., Ltd.
 */
/*
 * ClientP.h - Private definitions for ClientWindowWidget
 *
 * @(#)1.2 91/09/28 17:11:46
 */

#ifndef _ClientP_h
#define _ClientP_h

/***********************************************************************
 *
 * Client Window Widget Private Data
 *
 ***********************************************************************/

#include "Client.h"
#include "FocusP.h"
#include "Ximp.h"

/* New fields for the client window widget class record */
typedef struct {
    void (*status_draw)();
} ClientWindowClassPart;

/* Full class record declaration */
typedef struct _ClientWindowClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    FocusWindowClassPart	focusWindow_class;
    ClientWindowClassPart	clientWindow_class;
} ClientWindowClassRec;

extern ClientWindowClassRec clientWindowClassRec;

/* New fields for the client window widget record */
typedef struct {
    /* resources */
    Boolean       status_active;
    Widget        status_widget;
    StatusInfo    status;
    String        status_font;

    /* private state */
    String        app_class;
    unsigned int  refcnt;
    XIMText       *status_text;
} ClientWindowPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _ClientWindowRec {
    CorePart		core;
    CompositePart	composite;
    FocusWindowPart	focusWindow;
    ClientWindowPart	clientWindow;
} ClientWindowRec;

#endif /* _ClientP_h */
