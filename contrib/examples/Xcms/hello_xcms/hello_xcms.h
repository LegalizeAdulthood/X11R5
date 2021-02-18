/*
 *
 * Copyright 1991 Tektronix, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Tektronix, Inc. not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission. Tektronix makes no representations about
 * the suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Tektronix, Inc. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO 
 * EVENT SHALL Tektronix BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 *
 *	NAME
 *		 hello_xcms -- Another version of hello_color
 *
 *	DESCRIPTION
 *		This demonstrates some of the calls of Xcms and
 *		the InterClient Color Exchange Convention used
 *		in the TekColor Editor (xtici).
 *
 *	HISTORY
 *		
 *
 *	 HISTORY END
 *
 */

#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: hello_xcms.h,v 1.1 90/11/05 11:08:17 adamsc Exp $";
#endif /* RCS_ID */
#endif /* LINT */

#ifndef _HELLO_XCMS_h
#define _HELLO_XCMS_h

#define CMS_SelectionName	"_CMS_EDITED_COLORMAP"
#define CMS_MessageName		"_CMS_CHANGE_COLOR"
#define CMS_ExchangeName	"_CMS_COLOR"
#define CMAP			"COLORMAP"

typedef struct {
    long    when;	/* Time */
    long    min;	/* Minimum Pixel */
    long    max;	/* Maximum Pixel */
    long    x_root;	/* x location of pixel selected */
    long    y_root;	/* y location of pixel selected */
} CmsChangeStruct;

#endif /* _HELLO_XCMS_h */
