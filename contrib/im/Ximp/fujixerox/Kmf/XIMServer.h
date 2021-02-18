/*

Copyright 1991 by Fuji Xerox Co., Ltd., Tokyo, Japan.

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of M.I.T. not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission.  Fuji Xerox Co., Ltd. makes no representations
about the suitability of this software for any purpose.  It is provided "as is"
without express or implied warranty.

FUJI XEROX CO.,LTD. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJI XEROX CO.,LTD. BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

*/

#define NEED_EVENTS
#include "Xlibint.h"
#include "Xlcint.h"
#include <X11/Xutil.h>
#include "XIMProto.h"
#include <string.h>

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

typedef struct {
    Atom atom;
    Bool busy;
    struct _InputContextRec *ic;
} PropRec, *PropList;

typedef struct _PMRec {
    struct _PMRec *next;
    Window window;
    PropList list;
    unsigned long list_size;
    unsigned long link_count;
} PropManagerRec, *PropManager;

extern PropManager AllocPropManager(
#if NeedFunctionPrototypes
    Window	/* window */
#endif
);

extern void FreePropManager(
#if NeedFunctionPrototypes
    PropManager	/* pm */
#endif
);

extern Atom GetPropAtom(
#if NeedFunctionPrototypes
    InputContext	/* ic */
#endif
);

extern void ProcPropertyNotify(
#if NeedFunctionPrototypes
    Display *	/* dpy */,
    XPropertyEvent *	/* ev */
#endif
);

extern void FreePropAtom(
#if NeedFunctionPrototypes
    InputContext	/* ic */,
    Atom	/* atom */
#endif
);

extern void FreeAllPropAtom(
#if NeedFunctionPrototypes
    InputContext	/* ic */,
    Bool	/* delete */
#endif
);


typedef struct _InputMethodRec InputMethodRec, *InputMethod;
typedef struct _InputContextRec InputContextRec, *InputContext;
typedef struct _ICAttributeRec ICAttributeRec, *ICAttribute;
typedef struct _MethodPrivateRec MethodPrivateRec, *MethodPrivate;

struct _InputMethodRec {
    InputMethod next;
    char *locale_name;		/* get from setlocale() */
    char *server_name;		/* argument of InitializeProtocol() */
    char *protocol_version;	/* argument of InitializeProtocol() */
    char *server_version;	/* argument of InitializeProtocol() */
    Display *display;		/* argument of InitializeProtocol() */
    Window owner;		/* argument of InitializeProtocol() */
    Window preedit;
    Window status;
    Atom sel_detail;		/* get from XInternAtom() */
    Atom sel_simple;		/* get from XInternAtom() */
    XIMStyles styles;		/* argument of InitializeProtocol() */
    XFontSet font;
    XFontSetExtents *f_ext;
    unsigned long fore;
    unsigned long back;
    GC gc;
    GC invgc;
    InputContext ic;
    InputContext focused_ic;
    /* Internal Atoms */
    Atom _version;
    Atom _style;
    Atom _keys;
    Atom _servername;
    Atom _serverversion;
    Atom _protocol;
    Atom _extensions;
    Atom _ext_ui_back_front;
    Atom _focus;
    Atom _preedit;
    Atom _status;
    Atom _preedit_font;
    Atom _status_font;
    Atom _preeditdraw_data;
    Atom _feedbacks;
    /* List of Property Atom */
    PropManager prop;
};


typedef struct {
    /* for Request */
    Bool (*create)();
    Bool (*destroy)();
    Bool (*begin)();
    Bool (*end)();
    Bool (*getvalue)();
    Bool (*setvalue)();
    Bool (*set_focus)();
    Bool (*unset_focus)();
    Bool (*reset)();
    Bool (*key_press)();
    Bool (*move)();
    /* for Reply */
    Bool (*send_key)();
    Bool (*send)();
    Bool (*reset_reply)();
    Bool (*proc_begin)();
    Bool (*proc_end)();
} ICMethodsRec, *ICMethods;

struct _InputContextRec {
    InputContext next;
    ICMethods methods;
    InputMethod im;
    unsigned int icid;
    XIMStyle style;
    Window client;
    Window focus;
    Bool intercepting;
    Bool is_bep;
    ICAttribute preedit;
    ICAttribute status;
    MethodPrivate private;
};


typedef struct {
    Bool (*create)();
    Bool (*destroy)();
    Bool (*draw)();
    Bool (*caret)();
    Bool (*setvalue)();
    Bool (*getvalue)();
} ICAttrMethodsRec, *ICAttrMethods;

struct _ICAttributeRec {
    ICAttrMethods methods;
    XIMStyle style;
    XIMText *text;
    XRectangle *hilight;
    int num_hilight;
    void *data;
};

#define XIMPreeditStyles (XIMPreeditCallbacks | XIMPreeditPosition | XIMPreeditArea | XIMPreeditNothing)
#define XIMStatusStyles (XIMStatusCallbacks | XIMStatusArea | XIMStatusNothing)

typedef struct _PreeditProps {
    struct {
	unsigned long x, y, width, height;
    } area;
    unsigned long foreground;
    unsigned long background;
    Colormap colormap;
    Pixmap background_pixmap;
    unsigned long line_spacing;
    Cursor cursor;
    struct {
	unsigned long width, height;
    } area_needed;
    struct {
	unsigned long x, y;
    } spot_location;
} PreeditPropsRec, *PreeditProps;

typedef struct _StatusProps {
    struct {
	unsigned long x, y, width, height;
    } area;
    unsigned long foreground;
    unsigned long background;
    Colormap colormap;
    Pixmap background_pixmap;
    unsigned long line_spacing;
    Cursor cursor;
    struct {
	unsigned long width, height;
    } area_needed;
    Window status_window;
} StatusPropsRec, *StatusProps;


typedef struct {
    Bool (*create)();
    Bool (*destroy)();
    Bool (*begin)();
    Bool (*end)();
    Bool (*reset)();
    Bool (*keyin)();
} PrivateMethodsRec, *PrivateMethods;

struct _MethodPrivateRec {
    PrivateMethods methods;
    void *private;
};


extern InputMethod IMOfSelectionOwner();
extern InputMethod AllocIM();
extern InputContext ICOfICID();
extern InputContext GetMatchedActiveIC();

extern void *XAllocFreeArea();
extern Bool CheckDestroyNotify();

#define XAlloc(type) (type *)XAllocFreeArea(sizeof(type))
