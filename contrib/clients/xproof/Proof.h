/*
 * Copyright 1991 University of Wisconsin-Madison
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of Wisconsin-Madison not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  The University of
 * Wisconsin-Madison makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF WISCONSIN-MADISON DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF WISCONSIN-MADISON BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:   Marvin Solomon
 *           Computer Sciences Department
 *           University of Wisconsin, Madison WI
 *           solomon@cs.wisc.edu or seismo!uwvax!solomon
 *
 * Current Maintainer:
 *
 *           Tim Theisen           Systems Programmer
 * Internet: tim@cs.wisc.edu       Department of Computer Sciences
 *     UUCP: uwvax!tim             University of Wisconsin-Madison
 *    Phone: (608)262-0438         1210 West Dayton Street
 *      FAX: (608)262-9777         Madison, WI   53706
 *
 * $Source: /src/X11/uw/xproof/RCS/Proof.h,v $
 * $Header: /src/X11/uw/xproof/RCS/Proof.h,v 3.7 1991/10/04 22:05:44 tim Exp $
 */

#ifndef _Proof_h
#define _Proof_h
/****************************************************************
 *
 * Proof widget
 *
 ****************************************************************/

/* Resources:

 Name                Class              RepType         Default Value
 ----                -----              -------         -------------
 width               Width              Dimension       0
 height              Height             Dimension       0
 background          Background         Pixel           XtDefaultBackground
 foreground          Foreground         Pixel           XtDefaultForeground
 reverse_video       ReverseVideo       Boolean         False
 use_pixmap          UsePixmap          Boolean         depends on -DUSEPIXMAP
 idle_cursor         Cursor             Cursor          mouse
 busy_cursor         Cursor             Cursor          watch
 file                File               String          NULL
 scale               Scale              Dimension       10

*/

/* resource names used by proof widget that aren't defined in StringDefs.h */

#include <X11/Xaw/Simple.h>  /* for XtNcursor and XtCCursor */
#define XtNscale	"scale"
#define XtCScale	"Scale"
#define XtNusePixmap	"usePixmap"
#define XtCUsePixmap	"UsePixmap"
#define XtNidleCursor	"idleCursor"
#define XtNbusyCursor	"busyCursor"

/* structures */

typedef struct _ProofRec *ProofWidget;            /* see ProofP.h */
typedef struct _ProofClassRec *ProofWidgetClass;  /* see ProofP.h */


extern WidgetClass proofWidgetClass;

/* Exported Procedures */

void XtProofSetFile(/* ProofWidget w, FILE *f */);
void XtProofShowPage(/* ProofWidget w, long offset */);
void XtProofSetScale(/* ProofWidget w, int val, int change */);
	/* if change==0, set scale factor to val/10;
	 * if change<0, divide scale factor by val/10;
	 * if change>0, multiply scale factor by val/10;
	 */

/* Callback (see comment in main.c) */
void SetPage(/* w, offset, relative */);

#endif _Proof_h
/* DON'T ADD STUFF AFTER THIS #endif */
