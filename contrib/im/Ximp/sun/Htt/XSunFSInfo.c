#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunFSInfo.c 1.3 91/08/09";
#endif
#endif

/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL NOTICE
 *      file for terms of the license.
 */


#ifdef NO_PLUGGIN 
#include <X11/Xlib.h>
#include "XFontSetImpl.h"

/* 
 * XFontsOfFontSet, XBaseFontNameListOfFontSet, XLocaleOfFontSet,
 * and XFontSetExtents are already defined in Xlib
 */
XFontSetExtents *
XExtentsOfFontSet(font_set)
    XFontSet    font_set;
{    
    XFontSetStruct    *fs_info = (XFontSetStruct *)font_set;

    return(&fs_info->font_set_extents);
}


char *
XLocaleOfFontSet(font_set) 
    XFontSet    font_set;
{    
    XFontSetStruct    *fs_info = (XFontSetStruct *)font_set;

    return(fs_info->locale_info->locale);
}

int
XFontsOfFontSet(font_set, font_struct_list, font_name_list)
    XFontSet    	font_set;
    XFontStruct		***font_struct_list;
    char		***font_name_list;
{
    XFontSetStruct    *fs_info = (XFontSetStruct *)font_set;
    register int      i, count = 0;

    *font_struct_list = fs_info->font_structs;
    *font_name_list = fs_info->font_names;

    for (i = 0; i <= 3; i++)
	if (fs_info->font_structs[i] != NULL)
	    ++count;
	
    return(count);
}

#endif /* NO_PLUGGIN */
