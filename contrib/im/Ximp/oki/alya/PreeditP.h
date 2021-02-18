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
 * PreeditP.h -
 *
 * @(#)1.2 91/09/28 17:12:08
 */
#ifndef _PreeditP_h
#define _PreeditP_h

#ifdef X11R4
#include "XIMlib.h"
#endif

#include "Preedit.h"

typedef struct {
    void (*Draw)();
    int (*QueryWidth)();
    void (*Caret)();
} PreeditClassPart;

typedef struct _PreeditClassRec {
    CoreClassPart	core_class;
    PreeditClassPart	preedit_class;
} PreeditClassRec;

extern PreeditClassRec preeditClassRec;

typedef struct {
    /* resources */
    Position    offset_x;
    Position    offset_y;
    Pixel       foreground;
    XFontSet    font_set;
    Cursor      cursor;
    /* private state */
    unsigned long specified;
    GC          normal_GC;
    GC          reverse_GC;
    Position    spot_y_offset;
    Dimension   preedit_width;
    Dimension   preedit_height;
    wchar_t     *wcs;
    XIMFeedback *feedback;
    int         wcs_len;
    int         caret_position;
    XIMCaretStyle style;
    Dimension   caret_width, caret_height;
} PreeditPart;

typedef struct _PreeditRec {
    CorePart	core;
    PreeditPart	preedit;
} PreeditRec;

#endif /* _PreeditP_h */
