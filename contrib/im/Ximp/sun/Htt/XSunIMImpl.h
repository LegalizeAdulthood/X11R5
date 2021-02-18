#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunIMImpl.h 1.7 91/08/22";
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
#ifndef xim_impl_DEFINED
#define xim_impl_DEFINED

/*  SunOS 4.0 does not have a guard for including /usr/include/varargs.h
 *  multiple times, hence adding our own guard.  
 */
#ifndef _sys_varargs_h
#include <varargs.h>
#define _sys_varargs_h
#endif
#if XlibSpecificationRelease == 5
#include "Xlcint.h"
#endif /* XlibSpecificationRelease */


/*
 * XIM value (structure).
 */
typedef struct {
#ifndef NO_PLUGGIN
    XIMMethodsRec	*methods;
    XIMCoreRec		core;
#endif    
    Display		*display;
    char		locale[256];	/* language str */
    caddr_t		priv_data;	/* Unique to each IM */
} xim;

typedef	xim	*Xim;

/*
 * translate_kana_keysyms ret_status
 */
typedef enum {
          ModeOn	= 1,
          ModeOff	= 2,
} translate_kana_status;

typedef enum {
    XICOpCreate = 1,
    XICOpSet    = 2,
    XICOpGet    = 3,
} XICOp_t ;

typedef struct _icop {
    char *name ;
    int (*func)();
} icop_t ;

typedef struct {
#ifndef NO_PLUGGIN
    XICMethods		*methods;
    XICCoreRec		core;
#endif    
    char                language[256];  /* language str */
    Window		client_window;
    Window		focus_window;
    unsigned short	input_style;
    XIMCallback		*text_start;
    XIMCallback		*text_draw;
    XIMCallback		*text_done;
    XIMCallback		*text_caret;
    XIMCallback		*status_start;
    XIMCallback		*status_draw;
    XIMCallback		*status_done;
    XIMCallback		*lookup_start;
    XIMCallback		*lookup_draw;
    XIMCallback		*lookup_process;
    XIMCallback		*lookup_done;
    XIMCallback		*aux_start;
    XIMCallback		*aux_draw;
    XIMCallback		*aux_process;
    XIMCallback		*aux_done;
    Xim			im;		/* points to its IM */
/*
 * imlogic specific members
 */
    int			id;		/* IMLogic id */
    iml_status_t	iml_status;	/* see XSunIMCore.h */
    keymaps_t          *keymaps ;       /* function key maps */
    Bool                Backfront ;     /* frontend/backend switch */
} xic;

typedef	xic	*Xic;

/*
 * See also XSunIMPriv.h
 */
#define DEFAULTPreEditTextBufferSize 512

#ifndef NO_PLUGGIN
Public XIM _XSunOpenIM(
#ifdef notdef_arglist
	XLCd, Display*, XrmDatabase, char*, char*
#endif
);

extern Status _XSunCloseIM(
#ifdef notdef_arglist
	XIM 
#endif
);


extern char *_XSunGetIMValues(
#ifdef notdef_arglist
	XIM, XIMArg*
#endif
);

extern XIC _XSunCreateIC(
#ifdef notdef_arglist
	XIM, XIMArg*
#endif
);

extern void _XSunDestroyIC(
#ifdef notdef_arglist
	XIC
#endif
);

extern void _XSunSetICFocus(
#ifdef notdef_arglist
	XIC
#endif
);

void _XSunUnsetICFocus(
#ifdef notdef_arglist
	XIC
#endif
);

extern char* _XSunSetICValues(
#ifdef notdef_arglist
	XIC, XIMArg*
#endif
);

extern char* _XSunGetICValues(
#ifdef notdef_arglist
	XIC, XIMArg*
#endif
);

extern char* _XSunmbResetIC(
#ifdef notdef_arglist
	XIC
#endif
);

extern wchar_t* _XSunwcResetIC(
#ifdef notdef_arglist
	XIC
#endif
);

extern int _XSunmbLookupString(
#ifdef notdef_arglist
	XIC, XKeyEvent*, char*, int, KeySym*, Status*
#endif
);

extern int _XSunwcLookupString(
#ifdef notdef_arglist
	XIC, XKeyEvent*, wchar_t*, int, KeySym*, Status*
#endif
);

#endif 

#endif  ~xim_impl_DEFINED

