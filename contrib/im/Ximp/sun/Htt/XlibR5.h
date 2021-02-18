/* Derived from $XConsortium: Xlib.h,v 11.205 91/05/13 20:50:19 rws Exp $ */
/* 
 * Copyright 1985, 1986, 1987, 1991 by the Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided 
 * that the above copyright notice appear in all copies and that both that 
 * copyright notice and this permission notice appear in supporting 
 * documentation, and that the name of M.I.T. not be used in advertising
 * or publicity pertaining to distribution of the software without specific, 
 * written prior permission. M.I.T. makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * X Window System is a Trademark of MIT.
 *
 */

#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XlibR5.h 1.5 91/08/10";
#endif
#endif

/******************************************************************

              Copyright 1990, 1991, by Sun Microsystems, Inc.

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sun Microsystems, Inc.
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.
Sun Microsystems, Inc. makes no representations about the suitability of
this software for any purpose.  It is provided "as is" without
express or implied warranty.

Sun Microsystems Inc. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL Sun Microsystems, Inc. BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

  Author: Hideki Hiura (hhiura@Sun.COM)
	  				     Sun Microsystems, Inc.
******************************************************************/
/*
 *	XlibR5.h - Additional header definition of XIM/MLTEXT features
 *      which will be newly introduced in X11R5 against X11R4 Xlib.h.
 *	
 *      To use of XIM/MLTEXT feature with X11R4, please include this header
 *      file right after including Xlib.h.
 */
#ifndef _XLIBR5_H_
#define _XLIBR5_H_

#ifdef USG
#ifndef __TYPES__
#include <sys/types.h>			/* forgot to protect it... */
#define __TYPES__
#endif /* __TYPES__ */
#else
#if defined(_POSIX_SOURCE) && defined(MOTOROLA)
#undef _POSIX_SOURCE
#include <sys/types.h>
#define _POSIX_SOURCE
#else
#include <sys/types.h>
#endif
#endif /* USG */


#ifndef X_WCHAR
#include <stddef.h>
#else
typedef unsigned int wchar_t;
#endif

#ifndef _Xconst
#if __STDC__ || defined(__cplusplus) || defined(c_plusplus) || (FUNCPROTO&4)
#define _Xconst const
#else
#define _Xconst
#endif
#endif /* _Xconst */

typedef char *XPointer;

typedef struct {
    XRectangle      max_ink_extent;
    XRectangle      max_logical_extent;
} XFontSetExtents;

typedef struct _XFontSet *XFontSet;

typedef struct {
    char           *chars;
    int             nchars;
    int             delta;
    XFontSet        font_set;
} XmbTextItem;

typedef struct {
    wchar_t        *chars;
    int             nchars;
    int             delta;
    XFontSet        font_set;
} XwcTextItem;

typedef void (*XIMProc)();

typedef struct _XIM *XIM;
typedef struct _XIC *XIC;

typedef unsigned long XIMStyle;

typedef struct {
    unsigned short count_styles;
    XIMStyle *supported_styles;
} XIMStyles;

#define XIMPreeditArea		0x0001L
#define XIMPreeditCallbacks	0x0002L
#define XIMPreeditPosition	0x0004L
#define XIMPreeditNothing	0x0008L
#define XIMPreeditNone		0x0010L
#define XIMStatusArea		0x0100L
#define XIMStatusCallbacks	0x0200L
#define XIMStatusNothing	0x0400L
#define XIMStatusNone		0x0800L

#define XNVaNestedList "XNVaNestedList"
#define XNQueryInputStyle "queryInputStyle"
#define XNClientWindow "clientWindow"
#define XNInputStyle "inputStyle"
#define XNFocusWindow "focusWindow"
#define XNResourceName "resourceName"
#define XNResourceClass "resourceClass"
#define XNGeometryCallback "geometryCallback"
#define XNFilterEvents "filterEvents"
#define XNPreeditStartCallback "preeditStartCallback"
#define XNPreeditDoneCallback "preeditDoneCallback"
#define XNPreeditDrawCallback "preeditDrawCallback"
#define XNPreeditCaretCallback "preeditCaretCallback"
#define XNPreeditAttributes "preeditAttributes"
#define XNStatusStartCallback "statusStartCallback"
#define XNStatusDoneCallback "statusDoneCallback"
#define XNStatusDrawCallback "statusDrawCallback"
#define XNStatusAttributes "statusAttributes"
#define XNArea "area"
#define XNAreaNeeded "areaNeeded"
#define XNSpotLocation "spotLocation"
#define XNColormap "colorMap"
#define XNStdColormap "stdColorMap"
#define XNForeground "foreground"
#define XNBackground "background"
#define XNBackgroundPixmap "backgroundPixmap"
#define XNFontSet "fontSet"
#define XNLineSpace "lineSpace"
#define XNCursor "cursor"

#define XBufferOverflow		-1
#define XLookupNone		1
#define XLookupChars		2
#define XLookupKeySym		3
#define XLookupBoth		4

#if __STDC__
typedef void *XVaNestedList;
#else
typedef XPointer XVaNestedList;
#endif

typedef struct {
    XPointer client_data;
    XIMProc callback;
} XIMCallback;

typedef unsigned long XIMFeedback;

#define XIMReverse	1
#define XIMUnderline	(1<<1) 
#define XIMHighlight	(1<<2)
#define XIMPrimary 	(1<<5)
#define XIMSecondary	(1<<6)
#define XIMTertiary 	(1<<7)

typedef struct _XIMText {
    unsigned short length;
    XIMFeedback *feedback;
    Bool encoding_is_wchar; 
    union {
	char *multi_byte;
	wchar_t *wide_char;
    } string; 
} XIMText;

typedef struct _XIMPreeditDrawCallbackStruct {
    int caret;		/* Cursor offset within pre-edit string */
    int chg_first;	/* Starting change position */
    int chg_length;	/* Length of the change in character count */
    XIMText *text;
} XIMPreeditDrawCallbackStruct;

typedef enum {
    XIMForwardChar, XIMBackwardChar,
    XIMForwardWord, XIMBackwardWord,
    XIMCaretUp, XIMCaretDown,
    XIMNextLine, XIMPreviousLine,
    XIMLineStart, XIMLineEnd, 
    XIMAbsolutePosition,
    XIMDontChange
} XIMCaretDirection;

typedef enum {
    XIMIsInvisible,	/* Disable caret feedback */ 
    XIMIsPrimary,	/* UI defined caret feedback */
    XIMIsSecondary	/* UI defined caret feedback */
} XIMCaretStyle;

typedef struct _XIMPreeditCaretCallbackStruct {
    int position;		 /* Caret offset within pre-edit string */
    XIMCaretDirection direction; /* Caret moves direction */
    XIMCaretStyle style;	 /* Feedback of the caret */
} XIMPreeditCaretCallbackStruct;

typedef enum {
    XIMTextType,
    XIMBitmapType
} XIMStatusDataType;
	
typedef struct _XIMStatusDrawCallbackStruct {
    XIMStatusDataType type;
    union {
	XIMText *text;
	Pixmap  bitmap;
    } data;
} XIMStatusDrawCallbackStruct;

/*
 * Function prototypes
 */

extern Bool XSupportsLocale(
#if NeedFunctionPrototypes
    void
#endif
);

extern char *XSetLocaleModifiers(
#if NeedFunctionPrototypes
    _Xconst char*	/* modifier_list */
#endif
);

extern XFontSet XCreateFontSet(
#if NeedFunctionPrototypes
    Display*		/* display */,
    _Xconst char*	/* base_font_name_list */,
    char***		/* missing_charset_list */,
    int*		/* missing_charset_count */,
    char**		/* def_string */
#endif
);

extern void XFreeFontSet(
#if NeedFunctionPrototypes
    Display*		/* display */,
    XFontSet		/* font_set */
#endif
);

extern void XFreeStringList(
#if NeedFunctionPrototypes
    char**		/* list */
#endif
);

extern int XFontsOfFontSet(
#if NeedFunctionPrototypes
    XFontSet		/* font_set */,
    XFontStruct***	/* font_struct_list */,
    char***		/* font_name_list */
#endif
);

extern char *XBaseFontNameListOfFontSet(
#if NeedFunctionPrototypes
    XFontSet		/* font_set */
#endif
);

extern char *XLocaleOfFontSet(
#if NeedFunctionPrototypes
    XFontSet		/* font_set */
#endif
);

extern XFontSetExtents *XExtentsOfFontSet(
#if NeedFunctionPrototypes
    XFontSet		/* font_set */
#endif
);

extern int XmbTextEscapement(
#if NeedFunctionPrototypes
    XFontSet		/* font_set */,
    _Xconst char*	/* text */,
    int			/* bytes_text */
#endif
);

extern int XwcTextEscapement(
#if NeedFunctionPrototypes
    XFontSet		/* font_set */,
    wchar_t*		/* text */,
    int			/* num_wchars */
#endif
);

extern int XmbTextExtents(
#if NeedFunctionPrototypes
    XFontSet		/* font_set */,
    _Xconst char*	/* text */,
    int			/* bytes_text */,
    XRectangle*		/* overall_ink_return */,
    XRectangle*		/* overall_logical_return */
#endif
);

extern int XwcTextExtents(
#if NeedFunctionPrototypes
    XFontSet		/* font_set */,
    wchar_t*		/* text */,
    int			/* num_wchars */,
    XRectangle*		/* overall_ink_return */,
    XRectangle*		/* overall_logical_return */
#endif
);

extern Status XmbTextPerCharExtents(
#if NeedFunctionPrototypes
    XFontSet		/* font_set */,
    _Xconst char*	/* text */,
    int			/* bytes_text */,
    XRectangle*		/* ink_extents_buffer */,
    XRectangle*		/* logical_extents_buffer */,
    int			/* buffer_size */,
    int*		/* num_chars */,
    XRectangle*		/* overall_ink_return */,
    XRectangle*		/* overall_logical_return */
#endif
);

extern Status XwcTextPerCharExtents(
#if NeedFunctionPrototypes
    XFontSet		/* font_set */,
    wchar_t*		/* text */,
    int			/* num_wchars */,
    XRectangle*		/* ink_extents_buffer */,
    XRectangle*		/* logical_extents_buffer */,
    int			/* buffer_size */,
    int*		/* num_chars */,
    XRectangle*		/* overall_ink_return */,
    XRectangle*		/* overall_logical_return */
#endif
);

extern void XmbDrawText(
#if NeedFunctionPrototypes
    Display*		/* display */,
    Drawable		/* d */,
    GC			/* gc */,
    int			/* x */,
    int			/* y */,
    XmbTextItem*	/* text_items */,
    int			/* nitems */
#endif
);

extern void XwcDrawText(
#if NeedFunctionPrototypes
    Display*		/* display */,
    Drawable		/* d */,
    GC			/* gc */,
    int			/* x */,
    int			/* y */,
    XwcTextItem*	/* text_items */,
    int			/* nitems */
#endif
);

extern void XmbDrawString(
#if NeedFunctionPrototypes
    Display*		/* display */,
    Drawable		/* d */,
    XFontSet		/* font_set */,
    GC			/* gc */,
    int			/* x */,
    int			/* y */,
    _Xconst char*	/* text */,
    int			/* bytes_text */
#endif
);

extern void XwcDrawString(
#if NeedFunctionPrototypes
    Display*		/* display */,
    Drawable		/* d */,
    XFontSet		/* font_set */,
    GC			/* gc */,
    int			/* x */,
    int			/* y */,
    wchar_t*		/* text */,
    int			/* num_wchars */
#endif
);

extern void XmbDrawImageString(
#if NeedFunctionPrototypes
    Display*		/* display */,
    Drawable		/* d */,
    XFontSet		/* font_set */,
    GC			/* gc */,
    int			/* x */,
    int			/* y */,
    _Xconst char*	/* text */,
    int			/* bytes_text */
#endif
);

extern void XwcDrawImageString(
#if NeedFunctionPrototypes
    Display*		/* display */,
    Drawable		/* d */,
    XFontSet		/* font_set */,
    GC			/* gc */,
    int			/* x */,
    int			/* y */,
    wchar_t*		/* text */,
    int			/* num_wchars */
#endif
);

extern XIM XOpenIM(
#if NeedFunctionPrototypes
    Display*			/* dpy */,
    struct _XrmHashBucketRec*	/* rdb */,
    char*			/* res_name */,
    char*			/* res_class */
#endif
);

extern Status XCloseIM(
#if NeedFunctionPrototypes
    XIM /* im */
#endif
);

extern char *XGetIMValues(
#if NeedVarargsPrototypes
    XIM /* im */, ...
#endif
);

extern Display *XDisplayOfIM(
#if NeedFunctionPrototypes
    XIM /* im */
#endif
);

extern char *XLocaleOfIM(
#if NeedFunctionPrototypes
    XIM /* im*/
#endif
);

extern XIC XCreateIC(
#if NeedVarargsPrototypes
    XIM /* im */, ...
#endif
);

extern void XDestroyIC(
#if NeedFunctionPrototypes
    XIC /* ic */
#endif
);

extern void XSetICFocus(
#if NeedFunctionPrototypes
    XIC /* ic */
#endif
);

extern void XUnsetICFocus(
#if NeedFunctionPrototypes
    XIC /* ic */
#endif
);

extern wchar_t *XwcResetIC(
#if NeedFunctionPrototypes
    XIC /* ic */
#endif
);

extern char *XmbResetIC(
#if NeedFunctionPrototypes
    XIC /* ic */
#endif
);

extern char *XSetICValues(
#if NeedVarargsPrototypes
    XIC /* ic */, ...
#endif
);

extern char *XGetICValues(
#if NeedVarargsPrototypes
    XIC /* ic */, ...
#endif
);

extern XIM XIMOfIC(
#if NeedFunctionPrototypes
    XIC /* ic */
#endif
);

extern void XRegisterFilter(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    Window		/* window */,
    unsigned long	/* event_mask */,
    Bool		/* nonmaskable */,
    Bool (*)(
#if NeedNestedPrototypes
	     Display*	/* display */,
	     Window	/* window */,
	     XEvent*	/* event */,
	     XPointer	/* client_data */
#endif
	     )		/* filter */,
    XPointer		/* client_data */
#endif
);

extern void XUnregisterFilter(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    Window		/* window */,
    Bool (*)(
#if NeedNestedPrototypes
	     Display*	/* display */,
	     Window	/* window */,
	     XEvent*	/* event */,
	     XPointer	/* client_data */
#endif
	     )		/* filter */,
    XPointer		/* client_data */
#endif
);

extern Bool XFilterEvent(
#if NeedFunctionPrototypes
    XEvent*	/* event */,
    Window	/* window */
#endif
);

extern int XmbLookupString(
#if NeedFunctionPrototypes
    XIC			/* ic */,
    XKeyPressedEvent*	/* event */,
    char*		/* buffer_return */,
    int			/* bytes_buffer */,
    KeySym*		/* keysym_return */,
    Status*		/* status_return */
#endif
);

extern int XwcLookupString(
#if NeedFunctionPrototypes
    XIC			/* ic */,
    XKeyPressedEvent*	/* event */,
    wchar_t*		/* buffer_return */,
    int			/* wchars_buffer */,
    KeySym*		/* keysym_return */,
    Status*		/* status_return */
#endif
);

extern XVaNestedList XVaCreateNestedList(
#if NeedVarargsPrototypes
    int /*unused*/, ...
#endif
);

#endif /* _XLIBR5_H_ */

/*
 * Temporary solution
 */
#ifndef xim_im_DEFINED
#define xim_im_DEFINED
#endif  ~xim_im_DEFINED
#ifndef immgr_cb_DEFINED
#define immgr_cb_DEFINED
#endif immgr_cb_DEFINED
#ifndef xim_ic_DEFINED
#define xim_ic_DEFINED
#endif  ~xim_ic_DEFINED


