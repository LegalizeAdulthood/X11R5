#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunMbTWth.c 1.6 91/08/09";
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


extern int
#ifdef NO_PLUGGIN
XmbTextEscapement(font_set, text, bytes_text)
#else /* NO_PLUGGIN */
_XSunmbTextEscapement(font_set, text, bytes_text)
#endif /* NO_PLUGGIN */
    XFontSet	font_set;		/* In: font set to measure with */
    char	*text;			/* In: multibyte text string */
    int		bytes_text;		/* In: length of text in bytes */
{
    wchar_t    	*wcs_buf = _XmbTowcBuffer(bytes_text);         
    size_t	num_wchars;

    if (text == NULL) 
        return(!Success); 

    num_wchars = mbstowcs(wcs_buf, text, bytes_text+1);

#ifdef NO_PLUGGIN
    return(XwcTextEscapement(font_set, wcs_buf, num_wchars));
#else /* NO_PLUGGIN */
    return(_XSunwcTextEscapement(font_set, wcs_buf, num_wchars));
#endif /* NO_PLUGGIN */
} 

wchar_t *
_XmbTowcBuffer(num_bytes)
    int	    num_bytes;
{
    static wchar_t      *wcs_buf = NULL;
    static short        wcs_buf_len = 0;
    size_t              num_wchars, wcs_bytes;

    wcs_bytes = (num_bytes+1) * sizeof(wchar_t);
    if (wcs_buf_len < wcs_bytes) {
	if (wcs_buf != NULL)
	    free(wcs_buf);
	wcs_buf_len = wcs_bytes;
	wcs_buf = (wchar_t *)malloc(wcs_buf_len);
    }
    return(wcs_buf);
}
