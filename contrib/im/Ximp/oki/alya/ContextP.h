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
 * ContextP.h - Private definitions for Input Context Object
 *
 * @(#)1.2 91/09/28 17:11:51
 */

#ifndef _ContextP_h
#define _ContextP_h

/***********************************************************************
 *
 * InputContext Object Private Data
 *
 ***********************************************************************/

#include "ConversioP.h"
#include "Context.h"

/************************************************************
 *
 * New fields for the InputContext object class record.
 *
 ************************************************************/

typedef struct _InputContextClassPart {
    int dummy;
} InputContextClassPart;

/* Full class record declaration */
typedef struct _InputContextClassRec {
    ObjectClassPart     object_class;
    ConversionObjectClassPart	conversion_class;
    InputContextClassPart	inputContext_class;
} InputContextClassRec;

extern InputContextClassRec inputContextClassRec;

/* New fields for the InputContext object record */

typedef struct {
    XIMStyle style;
} InputContextPart;

/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _InputContextRec {
    ObjectPart     object;
    ConversionObjectPart conversion;
    InputContextPart inputContext;
} InputContextRec;

/************************************************************
 *
 * Private declarations.
 *
 ************************************************************/

#endif /* _ContextP_h */
