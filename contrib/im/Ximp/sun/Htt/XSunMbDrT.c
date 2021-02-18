#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunMbDrT.c 1.6 91/08/09";
#endif
#endif

/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL NOTICE
 *      file for terms of the license.
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#if XlibSpecificationRelease != 5
#include "XFontSetImpl.h"
#else /* XlibSpecificationRelease != 5 */
#include "XSunFSImpl.h"
#endif /* XlibSpecificationRelease != 5 */

#ifdef NO_PLUGGIN   /* This is not a pluggin routine in R5 */
void
XmbDrawText(display, d, gc, x, y, items, nitems)
    Display *display;
    Drawable d;                         /* In: what to draw in */
    GC gc;                              /* In: gc to use */
    int x, y;                           /* In: where to draw */
    XmbTextItem *items;                	/* In: array of text items to draw */
    int nitems;                         /* In: number of text items */

{
    register int        i; 
    wchar_t		*wcs_buf;
    size_t      	num_wchars;
    int                 pos = x, status = Success; 
    XmbTextItem         item; 
     
    for (i = 0; i < nitems; i++) { 
        item = items[i];
	wcs_buf = _XmbTowcBuffer(item.nchars);
	num_wchars = mbstowcs(wcs_buf, item.chars, item.nchars + 1);
        x = pos + item.delta;
        status = _XwcDrawOrDrawImageString(display, d, item.font_set, gc, x, y,
                        wcs_buf, num_wchars, XwcDrawString, &pos);  
        if (status != Success) {                         
            fprintf(stderr, "_XmbDrawText() failed\n"); 
            break; 
        }             
    }         
}
#endif /* NO_PLUGGIN */

