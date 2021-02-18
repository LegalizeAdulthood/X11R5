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

#ifndef _Xcu_Wlm_h
#define _Xcu_Wlm_h

/*
 * Superclass Public Include File
 */

#include <X11/Composite.h>

/*
 * New Resource Names
 */

/***
#define XtNfile			"file"
***/
#define XtNexposeCallback	"exposeCallback"

/*
 * Widget Type Definition
 */

typedef struct _XcuWlmClassRec	*XcuWlmWidgetClass;
typedef struct _XcuWlmRec	*XcuWlmWidget;

/*
 * Class Record Pointer
 */

extern WidgetClass xcuWlmWidgetClass;
 
/*
 * Public Definitions
 */

/* XcuWlmAddCallback() argument */
typedef enum
	{ XcuWLM_EVENTS=0, XcuWLM_NO_SAMPLING=0, XcuWLM_BYPASS=1, XcuWLM_SAMPLING=2 }
	XcuWlmConnectionType ;

/*
 * Public Procedures
 */

void	XcuWlmSetInt () ;
void	XcuWlmSetFloat () ;
void	XcuWlmSetString () ;
void	XcuWlmSetBoolean () ;
void	XcuWlmGetInt () ;
void	XcuWlmGetFloat () ;
void	XcuWlmGetString () ;
void	XcuWlmGetBoolean () ;
void	XcuWlmGetScan () ;
void	XcuWlmAddCallback () ;
void	XcuWlmAddInput () ;

void	XcuWlmKillRequest () ;
void	XcuWlmAppRequest () ;
Boolean	XcuWlmAppEvent () ;
void	XcuWlmRequest () ;
void	XcuWlmSample () ;
Boolean	XcuWlmEvent () ;

Widget	XcuWlmInquireWidget () ;
void	XcuWlmSetValue () ;
void	XcuWlmGetValue () ;

#endif /* _Xcu_Wlm_h */
