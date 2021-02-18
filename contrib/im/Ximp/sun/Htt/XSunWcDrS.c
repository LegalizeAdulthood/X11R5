#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunWcDrS.c 1.7 91/08/09";
#endif
#endif

/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL NOTICE
 *      file for terms of the license.
 */

#include <stdio.h>
#include <X11/Xlib.h>
#if XlibSpecificationRelease != 5
#include "XFontSetImpl.h"
#else /* XlibSpecificationRelease != 5 */
#include "XSunFSImpl.h"
#endif /* XlibSpecificationRelease != 5 */



extern void
#ifdef NO_PLUGGIN
XwcDrawString(dpy, d, font_set, gc, x, y, text, num_wchars)
#else /* NO_PLUGGIN */
_XSunwcDrawString(dpy, d, font_set, gc, x, y, text, num_wchars)
#endif /* NO_PLUGGIN */
    Display     *dpy;
    Drawable    d;
    XFontSet    font_set;
    GC          gc;
    int         x, y;
    wchar_t     *text;
    int         num_wchars;
{
    int	    pos;

    (void)_XwcDrawOrDrawImageString(dpy, d, font_set, gc, x, y, text, 
		num_wchars, XwcDrawString, &pos);
}


extern void
#ifdef NO_PLUGGIN
XwcDrawImageString(dpy, d, font_set, gc, x, y, text, num_wchars)
#else /* NO_PLUGGIN */
_XSunwcDrawImageString(dpy, d, font_set, gc, x, y, text, num_wchars)
#endif /* NO_PLUGGIN */
    Display     *dpy;
    Drawable    d;
    XFontSet    font_set;
    GC          gc;
    int         x, y;
    wchar_t     *text;
    int         num_wchars;
{
    int     pos; 

    (void)_XwcDrawOrDrawImageString(dpy, d, font_set, gc, x, y, text, 
		num_wchars, XwcDrawImageString, &pos);
}


int
_XwcDrawOrDrawImageString(dpy, d, font_set, gc, x, y, text, 
	num_wchars, proc, pos)
    Display 	*dpy;
    Drawable 	d;
    XFontSet 	font_set;
    GC 		gc;
    int 	x, y;
    wchar_t 	*text;
    int 	num_wchars;
    void	(*proc)();
    int		*pos;
{
    XFontSetStruct	*fs_info = (XFontSetStruct *)font_set;
    int			cs, sub_len, dummy;
    char		*mapped_str;
    XCharStruct		char_struct;
    int 		chars_left = num_wchars;
    CodesetInfo 	*cs_info = NULL;
    XFontStruct		*font_info;
    int			(*dims_proc)();

    *pos = 0;

    while (chars_left != 0) {
	if ((cs = FindCodesetOfChar(fs_info, *text, &cs_info)) == -1)
	    return(!Success);

	if ((sub_len = FindSubStringLength(cs_info, text, chars_left)) == 0)
	    return(!Success);

	if ((mapped_str = CharToFontEncoding(cs_info, text, sub_len)) == NULL)
	    return(!Success);
#ifdef NO_PLUGGIN
	font_info = fs_info->font_structs[cs];
#else /* NO_PLUGGIN */
	font_info = fs_info->core.font_struct_list[cs];
#endif /* NO_PLUGGIN */	
	XSetFont(dpy, gc, font_info->fid);

	if (cs_info->width == 1) {
	    if (proc == XwcDrawString)
	        XDrawString(dpy, d, gc, x, y, mapped_str, sub_len);
	    else
	        XDrawImageString(dpy, d, gc, x, y, mapped_str, sub_len);

	    dims_proc = XTextExtents;
	} else {
	    if (proc == XwcDrawString)
	        XDrawString16(dpy, d, gc, x, y, mapped_str, sub_len);
	    else
	        XDrawImageString16(dpy, d, gc, x, y, mapped_str, sub_len);

	    dims_proc = XTextExtents16;
	}

	/* figure out the total width of the rendered string */
	if (IS_FONT_FIXED_WIDTH(fs_info, cs)) {
	    x += sub_len * font_info->max_bounds.width;
	} else {
	    (*dims_proc)(fs_info->font_structs[cs], mapped_str, sub_len,
		    &dummy, &dummy, &dummy, &char_struct);
	    x += char_struct.width;
	}

	chars_left -= sub_len;
	text += sub_len;
    }

    *pos = x;

    return(Success);
}
	    

