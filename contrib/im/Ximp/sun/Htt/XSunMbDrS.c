#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunMbDrS.c 1.5 91/08/09";
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

extern void
#ifdef NO_PLUGGIN
XmbDrawString(dpy, d, font_set, gc, x, y, mbs, num_bytes)
#else /* NO_PLUGGIN */
_XSunmbDrawString(dpy, d, font_set, gc, x, y, mbs, num_bytes)
#endif /* NO_PLUGGIN */
    Display     *dpy;
    Drawable    d;
    XFontSet    font_set;
    GC          gc;
    int         x, y;
    char        *mbs;
    int         num_bytes;
{
    wchar_t     *wcs_buf = _XmbTowcBuffer(num_bytes);
    size_t      num_wchars;
    int		pos;

    num_wchars = mbstowcs(wcs_buf, mbs, num_bytes+1);

    (void)_XwcDrawOrDrawImageString(dpy, d, font_set, gc, x, y, wcs_buf, 
		num_wchars, XwcDrawString, &pos);
}


extern void
#ifdef NO_PLUGGIN
XmbDrawImageString(dpy, d, font_set, gc, x, y, mbs, num_bytes)
#else /* NO_PLUGGIN */
_XSunmbDrawImageString(dpy, d, font_set, gc, x, y, mbs, num_bytes)
#endif /* NO_PLUGGIN */
    Display     *dpy;
    Drawable    d;
    XFontSet    font_set;
    GC          gc;
    int         x, y;
    char        *mbs;
    int         num_bytes;
{
    wchar_t     *wcs_buf = _XmbTowcBuffer(num_bytes);
    size_t      num_wchars;
    int		pos;

    num_wchars = mbstowcs(wcs_buf, mbs, num_bytes+1);
    (void)_XwcDrawOrDrawImageString(dpy, d, font_set, gc, x, y, wcs_buf, 
		num_wchars, XwcDrawImageString, &pos);
}


