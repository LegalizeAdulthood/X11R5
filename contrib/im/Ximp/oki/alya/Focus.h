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
 * Focus.h -
 *
 * @(#)1.2 91/09/28 17:12:01
 */
#ifndef _Focus_h
#define _Focus_h

/***********************************************************************
 *
 * Focus Widget (subclass of CompositeClass)
 *
 ***********************************************************************/

#define XtNpreeditWidget        "preeditWidget"
#define XtNconvObject           "convObject"
#define XtNpreeditActive        "preeditActive"
#define XtNinputActive	        "inputActive"

#define XtNpreeditAreaX         "preeditAreaX"
#define XtNpreeditAreaY         "preeditAreaY"
#define XtNpreeditAreaWidth     "preeditArewWidth"
#define XtNpreeditAreaHeight    "preeditAreaHeight"
#define XtNpreeditNeededWidth   "preeditNeededWidth"
#define XtNpreeditNeededHeight  "preeditNeededHeight"
#define XtNpreeditSpotX         "preeditSpotX"
#define XtNpreeditSpotY         "preeditSpotY"
#define XtNpreeditColormap      "preeditColormap"
#define XtNpreeditForeground    "preeditForeground"
#define XtNpreeditBackground    "preeditBackground"
#define XtNpreeditBgPixmap      "preeditBgPixmap"
#define XtNpreeditFont          "preeditFont"
#define XtNpreeditLineSpacing   "preeditLineSpacing"
#define XtNpreeditCursor        "preeditCursor"

#define XtNpreeditInfo          "preeditInfo"

#define XtCPreeditWidget        "PreeditWidget"
#define XtCConvObject           "ConvObject"
#define XtCPreeditActive        "PreeditActive"
#define XtCInputActive	        "InputActive"

/* Class record constants */

extern WidgetClass focusWindowWidgetClass;

typedef struct _FocusWindowClassRec *FocusWindowWidgetClass;
typedef struct _FocusWindowRec      *FocusWindowWidget;

extern void FocusPreeditDraw();
extern void FocusPreeditCaret();
#endif /* _Focus_h */
