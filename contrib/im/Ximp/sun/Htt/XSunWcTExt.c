#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunWcTExt.c 1.8 91/08/09";
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


extern int
#ifdef NO_PLUGGIN
XwcTextExtents(font_set, text, num_wchars, 
	       overall_ink_extents, overall_logical_extents)
#else /* NO_PLUGGIN */
_XSunwcTextExtents(font_set, text, num_wchars, 
	       overall_ink_extents, overall_logical_extents)
#endif /* NO_PLUGGIN */	
    XFontSet	font_set;		/* In: font set to measure with */
    wchar_t	*text;			/* In: wide char text string */
    int		num_wchars;		/* In: length of text in wchars */
    XRectangle	*overall_ink_extents;	/* Out: overall ink dimensions */
    XRectangle	*overall_logical_extents; /* Out: overall logical dimensions */
{
    XFontSetStruct	*fs_info = (XFontSetStruct *)font_set;
    int			cs, sub_len;	
    int			direction, ascent, descent, max_ascent, max_descent;
    XCharStruct		dims;
    char		*mapped_str;
    int 		chars_left = num_wchars;
    CodesetInfo         *cs_info = NULL;
    XCharStruct         overall;

    overall.ascent= 0;
    overall.descent = 0;    
    overall.lbearing = 0;
    overall.rbearing = 0;
    overall.width = 0;
    max_ascent = max_descent = 0;
    
    while (chars_left !=0) {
	if ((cs = FindCodesetOfChar(fs_info, *text, &cs_info)) == -1)
            return(!Success);

        if ((sub_len = FindSubStringLength(cs_info, text, chars_left)) == 0)
            return(!Success);

        if ((mapped_str = CharToFontEncoding(cs_info, text, sub_len)) == NULL)
            return(!Success);

	if (cs_info->width == 1) {
	    XTextExtents(
#ifdef NO_PLUGGIN
			 fs_info->font_structs[cs],
#else /* NO_PLUGGIN */
			 fs_info->core.font_struct_list[cs],
#endif /* NO_PLUGGIN */	
	    	    	 mapped_str, sub_len, &direction, &ascent, &descent, 
	    	    	 &dims);
	} else {
	    XTextExtents16(
#ifdef NO_PLUGGIN
			 fs_info->font_structs[cs],
#else /* NO_PLUGGIN */
			 fs_info->core.font_struct_list[cs],
#endif /* NO_PLUGGIN */	
	    	    	 mapped_str, sub_len, &direction, &ascent, &descent, 
	    	    	 &dims);
	}
	
        max_ascent = MAX(max_ascent, ascent);
        max_descent = MAX(max_descent, descent);
	overall.ascent = MAX(overall.ascent, dims.ascent); 
	overall.descent = MAX(overall.descent, dims.descent); 
	/* BIG BUG:  should check on the left and right bearing */
	overall.lbearing = MIN(overall.lbearing, dims.lbearing);
	overall.rbearing = MAX(overall.rbearing, dims.rbearing);
	overall.width += dims.width;
	
	chars_left -= sub_len;
	text += sub_len;
    }
    overall_ink_extents->x = overall.lbearing;
    overall_ink_extents->y = - overall.ascent;
    overall_ink_extents->width = overall.rbearing - overall.lbearing;
    overall_ink_extents->height = overall.ascent + overall.descent;
 
    overall_logical_extents->x = 0;
    overall_logical_extents->y = - max_ascent;
    overall_logical_extents->width = overall.width;
    overall_logical_extents->height = max_ascent + max_descent;
    return(Success);
}

int    
#ifdef NO_PLUGGIN 
XwcTextPerCharExtents(font_set, text, num_wchars,
                      ink_extents_buffer, logical_extents_buffer,
                      buffer_size, num_chars,
                      overall_ink_extents, overall_logical_extents)
#else /* NO_PLUGGIN */
_XSunwcTextPerCharExtents(font_set, text, num_wchars,
                      ink_extents_buffer, logical_extents_buffer,
                      buffer_size, num_chars,
                      overall_ink_extents, overall_logical_extents)
#endif /* NO_PLUGGIN */                      
    XFontSet        font_set;		/* In: font set to measure with */
    wchar_t        *text;		/* In: wide char text string */
    int             num_wchars;		/* In: length of text in wchars */
    XRectangle     *ink_extents_buffer;	/* Out: pre-char ink dimensions */
    XRectangle     *logical_extents_buffer; /* Out: pre-char logical dim */
    int             buffer_size;	/* In: size of both buffers */
    int            *num_chars;		/* Out: number of XRectangles set */
    XRectangle     *overall_ink_extents;	/* Out: overall ink dim */
    XRectangle     *overall_logical_extents;	/* Out: overall logical dim */
{
    int			i;
    XRectangle		overall_ink, overall_logical;
    
    *num_chars = num_wchars;
    
    if (buffer_size < num_wchars) {        
        return(0);
    }

   for (i = 0; i < num_wchars; i++) {
       XwcTextExtents(font_set, text, 1, 
	       &ink_extents_buffer[i], &logical_extents_buffer[i]);
       text++;       
       if (i == 0) {
           overall_ink.x = ink_extents_buffer[i].x;
           overall_ink.y = ink_extents_buffer[i].y;
           overall_ink.width = ink_extents_buffer[i].width;
           overall_ink.height = ink_extents_buffer[i].height;
           
           overall_logical.x = logical_extents_buffer[i].x;
           overall_logical.y = logical_extents_buffer[i].y;
           overall_logical.width = logical_extents_buffer[i].width;
           overall_logical.height = logical_extents_buffer[i].height;

       } else {
            ink_extents_buffer[i].x += ink_extents_buffer[i-1].x;
            
            overall_ink.y = MIN(overall_ink.y, ink_extents_buffer[i].y);
            overall_ink.width += ink_extents_buffer[i].x;
            overall_ink.height = MAX(overall_ink.height, ink_extents_buffer[i].height);
            
            logical_extents_buffer[i].x += logical_extents_buffer[i-1].x;
            
            overall_logical.y = MIN(overall_logical.y, logical_extents_buffer[i].y);
            overall_logical.width += logical_extents_buffer[i].x;
            overall_logical.height = MAX(overall_logical.height, logical_extents_buffer[i].height);

       }     

   }
   if (overall_ink_extents) {
       overall_ink_extents->x = overall_ink.x;
       overall_ink_extents->y = overall_ink.y;
       overall_ink_extents->width = overall_ink.width;
       overall_ink_extents->height = overall_ink.height;
   }

   if (overall_logical_extents) {
       overall_logical_extents->x = overall_logical.x;
       overall_logical_extents->y = overall_logical.y;
       overall_logical_extents->width = overall_logical.width;
       overall_logical_extents->height = overall_logical.height;
   }
   
   return(Success);
}
