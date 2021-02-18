#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunWcDrT.c 1.6 91/08/09";
#endif
#endif

/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL NOTICE
 *      file for terms of the license.
 */

#include <X11/Xlib.h>
#ifdef SVR4
#include <widec.h>
#endif /* SVR4 */
#if XlibSpecificationRelease != 5
#include "XFontSetImpl.h"
#else /* XlibSpecificationRelease != 5 */
#include "XSunFSImpl.h"
#endif /* XlibSpecificationRelease != 5 */

#ifdef NO_PLUGGIN   /* This is not a pluggin routine in R5 */
void
XwcDrawText(display, d, gc, x, y, items, nitems)
    Display *display;
    Drawable d;                         /* In: what to draw in */
    GC gc;                              /* In: gc to use */
    int x, y;                           /* In: where to draw */
    XwcTextItem *items;                	/* In: array of text items to draw */
    int nitems;                         /* In: number of text items */

{
    register int	i;
    int			pos = x, status = Success;
    XwcTextItem		item;

    for (i = 0; i < nitems; i++) {
	item = items[i];
	x = pos + item.delta;
	status = _XwcDrawOrDrawImageString(display, d, item.font_set, gc, x, y,
			item.chars, item.nchars, XwcDrawString, &pos); 
	if (status != Success) {
	    fprintf(stderr, "XwcDrawText() failed\n");
	    break;
	}
    }
}
#endif /* NO_PLUGGIN */


