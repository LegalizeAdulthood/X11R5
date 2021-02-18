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
 * Ximp.h -
 *
 * @(#)1.2 91/09/28 17:12:10
 */
#ifndef _Ximp_h
#define _Ximp_h

#ifdef X11R4
#include "XIMlib.h"
#endif
#include "XIMProto.h"

/*
 *	property
 */
extern Atom XIMP_PROTOCOL;

extern Atom XIMP_VERSION;
extern Atom XIMP_STYLE;
extern Atom XIMP_KEYS;
extern Atom XIMP_SERVERNAME;
extern Atom XIMP_SERVERVERSION;
extern Atom XIMP_EXTENSIONS;
extern Atom XIMP_PREEDIT;
extern Atom XIMP_STATUS;
extern Atom XIMP_FOCUS;
extern Atom XIMP_PREEDITFONT;
extern Atom XIMP_STATUSFONT;
extern Atom XIMP_PREEDIT_DRAW_DATA;
extern Atom XIMP_FEEDBACKS;
extern Atom XIMP_PREEDITDRAWLENGTH;
extern Atom XIMP_PREEDITDRAWSTRING;
extern Atom XIMP_PREEDITDRAWFEEDBACK;
extern Atom XIMP_FEEDBACKS;
extern Atom XIMP_EXT_UI_STATUSWINDOW;
extern Atom XIMP_EXT_UI_BACK_FRONT;

/* サーバ名 */

#define SERVERNAME "OKI"

/* サーババージョン */

#define SERVERVERSION "0.1"

/* ClientMessage で送ることのできる最大のバイト数 */

#define MESSAGE_MAX_SIZE 15

#define MAX_PROP 32

typedef struct {
    XRectangle    area;
    unsigned long foreground;
    unsigned long background;
    Colormap      colormap;
    Pixmap        background_pixmap;
    int           line_spacing;
    Cursor        cursor;
    XRectangle    areaneeded;
    Window        status_window;
} StatusInfo;

typedef struct {
    XRectangle    area;
    unsigned long foreground;
    unsigned long background;
    Colormap      colormap;
    Pixmap        background_pixmap;
    int           line_spacing;
    Cursor        cursor;
    XRectangle    areaneeded;
    XRectangle    spot_location;
} PreeditInfo;

#define XIMP_PROP_FOCUS		( XIMP_FOCUS_WIN_MASK )
#define XIMP_PROP_PREEDIT	( XIMP_PRE_AREA_MASK \
				| XIMP_PRE_FG_MASK \
				| XIMP_PRE_BG_MASK \
				| XIMP_PRE_COLORMAP_MASK \
				| XIMP_PRE_BGPIXMAP_MASK \
				| XIMP_PRE_LINESP_MASK \
				| XIMP_PRE_CURSOR_MASK \
				| XIMP_PRE_AREANEED_MASK \
				| XIMP_PRE_SPOTL_MASK )
#define XIMP_PROP_STATUS	( XIMP_STS_AREA_MASK \
				| XIMP_STS_FG_MASK \
				| XIMP_STS_BG_MASK \
				| XIMP_STS_COLORMAP_MASK \
				| XIMP_STS_BGPIXMAP_MASK \
				| XIMP_STS_LINESP_MASK \
				| XIMP_STS_CURSOR_MASK \
				| XIMP_STS_AREANEED_MASK \
				| XIMP_STS_WINDOW_MASK )
#define XIMP_PROP_PREFONT       ( XIMP_PRE_FONT_MASK )
#define XIMP_PROP_STSFONT       ( XIMP_STS_FONT_MASK )

#define DEFAULTFONT "-*-*-*-R-Normal--*-130-75-75-*-*"

#endif /* _Ximp_h */
