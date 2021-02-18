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
 * ConversioP.h - Private definitions for ConversionObject
 *
 * @(#)1.2 91/09/28 17:11:53
 */

#ifndef _ConversioP_h
#define _ConversioP_h

/***********************************************************************
 *
 * XimpObject Private Data
 *
 ***********************************************************************/

#include <X11/ObjectP.h>
#include "Conversion.h"
#include "Ximp.h"
#include "cconv.h"

/************************************************************
 *
 * New fields for the ConversionObject class record.
 *
 ************************************************************/

typedef struct _ConversionObjectClassPart {
  void	(*Begin)();
  void	(*End)();
  void	(*SetFocus)();
  void  (*UnsetFocus)();
  void  (*SetValue)();
  void  (*GetValue)();
  void	(*Move)();
  void	(*Reset)();
  void	(*Key)();
  void	(*Draw)();
  void	(*Caret)();
  void	(*StatusDraw)();
} ConversionObjectClassPart;

/* Full class record declaration */
typedef struct _ConversionObjectClassRec {
    ObjectClassPart     object_class;
    ConversionObjectClassPart	conversion_class;
} ConversionObjectClassRec;

extern ConversionObjectClassRec conversionObjectClassRec;

/* New fields for the ConversionObject object record */


typedef struct {
    /* resources */
    int           icid;
    Window        client_window;
    Window        server_window;
    int           input_mode;		/* front-end:0 / back-end:1 */
    int		  mask;
    /* private state */

    PreeditInfo   preedit;
    String	  preedit_font;
    Window        focus_window;
    Widget        focus_widget;

    StatusInfo    status;
    String        status_font;
    Boolean       status_extension;
    Window        status_window;
    Widget        status_widget;

    Boolean       in_conversion;
    Boolean       focus;

    ccRule        ccrule;
    ccBuf         ccbuf;
    String        ccdeffile;
    int           mode;			/* input mode (kana,eisuu etc )*/
    int           state;		/* */
    int           n_ins;
    int           n_del;
    wchar_t       *ins_buf;
    int           caret;                /* caret offset within preedit str */
    int           old_caret;
    XEvent        *event;

    Widget        cv;

    XIMText	  text;
    XIMStatusDrawCallbackStruct status_text;
} ConversionObjectPart;

/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _ConversionObjectRec {
  ObjectPart     object;
  ConversionObjectPart conversion;
} ConversionObjectRec;

/************************************************************
 *
 * Private declarations.
 *
 ************************************************************/

typedef void (*_ConversionObjectVoidFunc)();

#define XtInheritBegin      ((_ConversionObjectVoidFunc)_XtInherit)
#define XtInheritEnd        ((_ConversionObjectVoidFunc)_XtInherit)
#define XtInheritSetFocus   ((_ConversionObjectVoidFunc)_XtInherit)
#define XtInheritUnsetFocus ((_ConversionObjectVoidFunc)_XtInherit)
#define XtInheritXimpSetValue ((_ConversionObjectVoidFunc)_XtInherit)
#define XtInheritXimpGetValue ((_ConversionObjectVoidFunc)_XtInherit)
#define XtInheritMove       ((_ConversionObjectVoidFunc)_XtInherit)
#define XtInheritReset      ((_ConversionObjectVoidFunc)_XtInherit)
#define XtInheritKey        ((_ConversionObjectVoidFunc)_XtInherit)
#define XtInheritDraw       ((_ConversionObjectVoidFunc)_XtInherit)
#define XtInheritCaret      ((_ConversionObjectVoidFunc)_XtInherit)
#define XtInheritStatusDraw ((_ConversionObjectVoidFunc)_XtInherit)
#define XtInheritStatusEnd  ((_ConversionObjectVoidFunc)_XtInherit)

#define DEF_CCDEF_FILE "ccdef.kinput"

#endif /* _ConversioP_h */
