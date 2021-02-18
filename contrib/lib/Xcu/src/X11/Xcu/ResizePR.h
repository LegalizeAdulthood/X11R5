/*
 * Copyright 1991 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          580 Theory Center, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@graphics.cornell.edu
 */

#ifndef _Xcu_ResizeParticipants_e
#define _Xcu_ResizeParticipants_e

#define XtNresizeParticipants	"resizeParticipants"
#define XtCResizeParticipants	"ResizeParticipants"
#define XtRResizeParticipants	"ResizeParticipants"

/*
 * Values for XtNresizeParticipants
 */

typedef enum
	{
	 XcuRESIZE_NONE =		0
	,XcuRESIZE_INTERS =	1
	,XcuRESIZE_INTERNALS =	2
	,XcuRESIZE_CHILDREN =	4 
	}
	XcuResizeParticipants ;

void XcuAddStringToResizeParticipantsConverter () ;

#endif /* _Xcu_ResizeParticipants_e */
