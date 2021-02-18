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
 * $Source: /src/X11/uw/xproof/RCS/ProofP.h,v $
 * $Header: /src/X11/uw/xproof/RCS/ProofP.h,v 3.5 1991/10/04 22:05:44 tim Exp $
 */

#ifndef _ProofP_h
#define _ProofP_h

#include "Proof.h"
#include <X11/CoreP.h>
#include <stdio.h>

typedef struct {                    /* new fields for proof class */
	int dummy;                      /* stupid C compiler */
} ProofClassPart;

typedef struct _ProofClassRec { /* full class record declaration */
	CoreClassPart core_class;
	ProofClassPart proof_class;
} ProofClassRec;

typedef struct {                /* new fields for proof widget */
	/* resources */
	Pixel foreground_pixel;         /* color index of normal state fg */
	Boolean reverse_video;          /* do reverse video? */
	Boolean use_pixmap;             /* use off screen pixmap? */
	FILE *file;                     /* file to display */
	Cursor idle_cursor;             /* cursor to use while idle */
	Cursor busy_cursor;             /* cursor to use while busy */
	Dimension scale;                /* scale factor (*10) for locations and
									 * point sizes. */

	/* private state */
	GC DrawGc;                      /* GC for displaying text and lines */
	GC FillGc;                      /* GC for filling polygons */
	GC ClearGc;                     /* GC for clearing canvas */
	GC CopyGc;                      /* GC for copying canvas to the window */
	Pixmap canvas;                  /* place where the drawing is done */
	long offset;                    /* seek pointer for current page (-1 if
									 * no seeking is possible in file) */
	int shown;                      /* has the current page been interpreted
									 * yet? */
	/* for graphics commands */
	int line_style;                 /* LineSolid or LineOnOffDash */
	int thickness;                  /* line width */
} ProofPart;

typedef struct _ProofRec {          /* full instance record */
	CorePart core;
	ProofPart proof;
} ProofRec;


extern ProofClassRec proofClassRec;/* class pointer */

#endif _ProofP_h
