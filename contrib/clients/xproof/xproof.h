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
 */
#include <stdio.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include "Proof.h"

/*
 * $Header: /src/X11/uw/xproof/RCS/xproof.h,v 3.7 1991/10/04 22:05:44 tim Exp $
 */

#ifndef DESCDIR
#define DESCDIR "/usr/lib/font" /* Where device descriptions are kept */
#endif
#ifndef DEVICE
#define DEVICE "psc"
#endif
#define ALLOC(type,number) ((type *)XtMalloc((unsigned)(number)*sizeof(type)))

#define MAXFACES 60     /* Max number of distinct type faces supported */
#define RESOLUTION 576  /* default value for resolution (see below) */
#define SCALE(x) ((scaled_res * (x))/resolution)
/* NB:
 * position = x     / resolution   * screen_res  * proof.scale/10 
 *          [ units / (units/inch) * pixels/inch * pixels/pixels  = pixels ]
 *          = (proof.scale/10) * screen_res * x / resolution
 *          = scaled_res * x / resolution
 * Do multiplication before division to preserve precision.  Really should
 * round rather than truncate, but that's too slow and the difference is
 * not visible.
 */
#define BEEP(w) XBell(XtDisplay((Widget)w), 33)
#define INFINITY 0x7FFF /* assume at least 16 bits/int! */

/* Information about (troff) fonts.
 * See also info about X fonts in xfontinfo.c
 */
struct faceInfo {
	char name[3];
	int *pos;       /* maps character number to position (-1 == not present) */
	Font *xfont;    /* maps size to X Font resource */
}
	*face[MAXFACES],    /* all faces that have been "opened" */
	*mounted[MAXFACES], /* which face is "mounted" where */
	*curface;           /* currently mounted face */

int curmount;       /* mounted[curmount] == curface */
int nfaces;         /* number of elements of face[] in use */
int maxmount;       /* mounted faces are 1..maxmount */

int nsizes;         /* number of point sizes available */
int *size;			/* list of available sizes */
int cursize;        /* current point size (index into size[] array) */

int nspecial;       /* number of "fancy" characters ( \(xx ). */
char **fancy_char;  /* names of "fancy" characters */

int hpos, vpos;     /* current position on page */
int resolution;     /* device resolution on target device (units/inch) */
int screen_res;     /* approximate resolution of screen (either 75 or 100) */
int scaled_res;     /* proof.scale * screen_res / 10 */

#define MAXPAGES	400

/* stuff for lint */

char *strcpy();
#ifdef ultrix
void exit(), perror();
#endif

/* routines defined in draw.c and called from showpage.c */
void drawline(/* w, deltax, deltay */);
void drawcirc(/* w, diam */);
void drawellipse(/* w, ah, av */);
void drawarc(/* w, cdh, cdv, pdh, pdv */);
void drawpolygon(/* w, f, fill, border */);
void drawcurve(/* w, f, style */);
void drawthick(/* w, s */);
void drawstyle(/* w, mask */);

/* routines defined in Proof.c and called from draw.c */
void drawXarc(/* w, x, y, width, height, angle1, angle2 */);
void drawXline(/* w, x0, y0, x1, y1 */);
void drawXpoint(/* w, x0, y0 */);
void drawXpoly(/* w, fill, border, points, npoints */);
void drawXthick(/* w, thickness */);
void drawXlinestyle(/* w, dashes, ndashes */);

/* routines defined in index.c and called from main.c */
int makeIndex(/* f */);
long seekPointer(/* page */);
int pageNumber(/* page */);
int findPage(/* int current, new */);

/* routines defined in xfontinfo.c and called from Proof.c */
Font LoadXfont(/* widget, troffname, pointsize */);
void UnloadXFonts(/* widget */);
