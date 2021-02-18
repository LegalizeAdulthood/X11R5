#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunWcTWth.c 1.7 91/08/09";
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



int
#ifdef NO_PLUGGIN 
XwcTextEscapement(font_set, text, num_wchars)
#else /* NO_PLUGGIN */
_XSunwcTextEscapement(font_set, text, num_wchars)
#endif /* NO_PLUGGIN */
    XFontSet	font_set;		/* In: font set to measure with */
    wchar_t	*text;			/* In: wide char text string */
    int		num_wchars;		/* In: length of text in wchars */
{
    XFontSetStruct	*fs_info = (XFontSetStruct *)font_set;
    int			cs, sub_len;	
    char		*mapped_str;
    int			width = 0;
    int 		chars_left = num_wchars;
    CodesetInfo		*cs_info;

    while (chars_left != NULL) {
	if ((cs = FindCodesetOfChar(fs_info, *text, &cs_info)) == -1)
            return(0);
 
        if ((sub_len = FindSubStringLength(cs_info, text, chars_left)) == 0)
            return(!0);
 
        if ((mapped_str = CharToFontEncoding(cs_info, text, sub_len)) == NULL)
            return(0);

	if (cs_info->width == 1) 
	    width += XTextWidth(
#ifdef NO_PLUGGIN
				fs_info->font_structs[cs],
#else /* NO_PLUGGIN */
				fs_info->core.font_struct_list[cs],
#endif /* NO_PLUGGIN */	
	    
	    			mapped_str, sub_len);
	else 
	    width += XTextWidth16(
#ifdef NO_PLUGGIN
				fs_info->font_structs[cs],
#else /* NO_PLUGGIN */
				fs_info->core.font_struct_list[cs],
#endif /* NO_PLUGGIN */	
	    
	    			mapped_str, sub_len);

	chars_left -= sub_len;
	text += sub_len;
    }

    return(width);
}
	    

