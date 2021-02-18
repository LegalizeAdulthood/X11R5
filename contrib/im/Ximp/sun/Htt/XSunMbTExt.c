#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunMbTExt.c 1.6 91/08/09";
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
XmbTextExtents(font_set, text, bytes_text,
               overall_ink_extents, overall_logical_extents)
#else /* NO_PLUGGIN */
_XSunmbTextExtents(font_set, text, bytes_text, 
	       overall_ink_extents, overall_logical_extents)
#endif /* NO_PLUGGIN */	       
    XFontSet	font_set;		/* In: font set to measure with */
    char	*text;			/* In: multibyte text string */
    int		bytes_text;		/* In: length of text in bytes */
    XRectangle	*overall_ink_extents;	/* Out: overall ink dimensions */
    XRectangle	*overall_logical_extents; /* Out: overall logical dimensions */
{
    wchar_t     *wcs_buf = _XmbTowcBuffer(bytes_text);
    size_t      num_wchars;

    if (text == NULL) 
	return(!Success);

    num_wchars = mbstowcs(wcs_buf, text, bytes_text+1);

    return(XwcTextExtents(font_set, wcs_buf, num_wchars, overall_ink_extents, overall_logical_extents));
}


/* Not tested yet */
extern int
#ifdef NO_PLUGGIN
XmbTextPerCharExtents(font_set, text, bytes_text,
                      ink_extents_buffer, logical_extents_buffer,
                      buffer_size, num_chars,
                      overall_ink_extents, overall_logical_extents)
#else /* NO_PLUGGIN */
_XSunmbTextPerCharExtents(font_set, text, bytes_text,
                      ink_extents_buffer, logical_extents_buffer,
                      buffer_size, num_chars,
                      overall_ink_extents, overall_logical_extents)
#endif /* NO_PLUGGIN */                      
    XFontSet        font_set;
    char           *text;
    int             bytes_text;
    XRectangle     *ink_extents_buffer;
    XRectangle     *logical_extents_buffer;
    int             buffer_size;
    int            *num_chars;
    XRectangle     *overall_ink_extents;
    XRectangle     *overall_logical_extents;
{
    wchar_t     *wcs_buf = _XmbTowcBuffer(bytes_text);
    size_t      num_wchars;

    if (text == NULL) 
	return(!Success);

    num_wchars = mbstowcs(wcs_buf, text, bytes_text+1);
    
#ifdef NO_PLUGGIN
    return(XwcTextPerCharExtents(font_set, wcs_buf, num_wchars,
                      ink_extents_buffer, logical_extents_buffer,
		      buffer_size, num_chars, overall_ink_extents,
		      overall_logical_extents));

#else /* NO_PLUGGIN */    
    return(_XSunwcTextPerCharExtents(font_set, wcs_buf, num_wchars,
                      ink_extents_buffer, logical_extents_buffer,
		      buffer_size, num_chars, overall_ink_extents,
		      overall_logical_extents));
		      
#endif /* NO_PLUGGIN */    
}
