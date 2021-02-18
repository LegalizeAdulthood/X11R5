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
 * Sj3P.h -
 *
 * @(#)1.1 91/09/28 17:13:26
 */

#ifndef _Sj3P_h
#define _Sj3P_h

/***********************************************************************
 *
 * XimpObject Private Data
 *
 ***********************************************************************/

#include <X11/ObjectP.h>
#include "Sj3.h"
#include <sj3lib.h>
#include "Ximp.h"

typedef struct _wc_douon {
    wchar_t ddata[256];
    int dlen;
    struct studyrec dcid;
} wc_douon;

typedef struct _wc_douon_list {
    int            current;
    unsigned char *sj_yomi;
    wchar_t       *yomi;
    int            len;
    wchar_t       *knj;
    int            knjlen;
    struct studyrec dcid;
    int            n;
    wc_douon      *dou;
} wc_douon_list;

typedef struct _henkan_buf {
    wchar_t *yomi;
    int      current;
    int      n;
    wc_douon_list *douon;
} henkan_buf;

/************************************************************
 *
 * New fields for the Sj3Object class record.
 *
 ************************************************************/

typedef struct _Sj3ObjectClassPart {
  void	(*Henkan)();
  void	(*Forward)();
  void	(*Backward)();
  void  (*Next)();
  void  (*Previous)();
  void	(*Cancel)();
  wchar_t	*(*Fix)();
} Sj3ObjectClassPart;

/* Full class record declaration */
typedef struct _Sj3ObjectClassRec {
    ObjectClassPart     object_class;
    Sj3ObjectClassPart	sj3_class;
} Sj3ObjectClassRec;

extern Sj3ObjectClassRec sj3ObjectClassRec;

/* New fields for the Sj3Object object record */


typedef struct {
    /* resources */
    String server_name;
    /* Private */
    wchar_t *display_char;
    XIMFeedback *feedback;
    henkan_buf *buf;
    
} Sj3ObjectPart;

/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _Sj3ObjectRec {
  ObjectPart     object;
  Sj3ObjectPart sj3;
} Sj3ObjectRec;

#endif /* _Sj3P_h */
