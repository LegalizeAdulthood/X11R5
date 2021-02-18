#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunFSUtil.c 1.3 91/08/09";
#endif
#endif

/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL NOTICE
 *      file for terms of the license.
 */

#include <stdio.h>
#include <ctype.h>
#include <X11/Xlib.h>
#if XlibSpecificationRelease != 5
#include "XFontSetImpl.h"
#else  /* XlibSpecificationRelease != 5 */
#include "XSunFSImpl.h"
#endif /* XlibSpecificationRelease != 5 */


/*
 * This function is not available in 4.0.3 C library, but is present 
 * in the 4.1 C library.
 */
char *
strstr(s1, s2)
        register char *s1, *s2;
{
        int s2len = strlen(s2); /* length of the second string */
        /*
         * If the length of the second string is 0,
         *  return the first argument.
         */
        if (s2len == 0)
                return (s1);
 
        while (strlen(s1) >= s2len) {
                if (strncmp(s1, s2, s2len) == 0)
                        return (s1);
                s1++;
        }
        return (0);
}

int
FindCodesetOfChar(fs_info, c, cs_info_return)
    XFontSetStruct  	*fs_info;
    wchar_t		c;
    CodesetInfo		**cs_info_return;
{
    register int	i;
    CodesetInfo		**codesets = fs_info->locale_info->codesets;
    PlaneInfo		**planes = fs_info->locale_info->planes;

    *cs_info_return = NULL;
    for (i = 0; i < MAXPLANES; i++)
	if (IS_CODESET_VALID(planes[i]) && 
		IS_CHAR_IN_CODESET(planes[i], c)) {
	    *cs_info_return = planes[i];
	    return(MAX_CODE_SET + i);
        }

    for (i = 0; i <= 3; i++)
	if (IS_CODESET_VALID(codesets[i]) && 
		IS_CHAR_IN_CODESET(codesets[i], c)) {
	    *cs_info_return = codesets[i];
	    return(i);
        }

    fprintf(stderr, "FindCodesetOfChar(): Unable to determine codeset\n");
    return(-1);
}


int
FindSubStringLength(cs_info, text, max_len)
    CodesetInfo	    *cs_info;
    wchar_t	    *text;
    int		    max_len;
{
    int	    	len = 0;

    if (!IS_CODESET_VALID(cs_info))
	return(0);

    while (len < max_len)
	if (!IS_CHAR_IN_CODESET(cs_info, *text))
	    break;
	else {
	    text++;
	    len++;
	}

    return(len);
}
	
char *
CharToFontEncoding(cs_info, text, len)
    CodesetInfo	    *cs_info;
    wchar_t	    *text;   
    int		    len;
{
    static char		*str = NULL;
    static short   	str_len = 0;
    register int   	i;
    register char  	*s;
    register unsigned short	temp_str;

    if (!IS_CODESET_VALID(cs_info))
	return(NULL);

    i = len * cs_info->width;
    if (str_len < i) {
	if (str != NULL)
	    free(str);
	str_len = i;
	str = (char *)malloc(i);
	if (str == NULL)
	    return(NULL);
    }

    s = str;
    for (i = 0; i < len; i++) {
	switch (cs_info->width) {
	case 1:
	    *s++ = (char)(*text++ & cs_info->mapping_mask);
	    break;

	case 2:
	    *((short *)s)++ = (short)(*text++ & cs_info->mapping_mask);
	    break;

	case 4:
	    temp_str = (unsigned short)(*text++ & cs_info->mapping_mask);
	    *((short *)s)++ = (((temp_str & 0x3f80) << 1) | (temp_str & 0x7f));
	    break;

	defaults:
	    break;
	}
    }

    return(str);
}

int
IsFontFixedWidth(name)
    char    *name;
{
    char   *n;
    int	    dashes = 0;

    /* 
     * We need a more robust check here. The X11/NeWS server uses 
     * similar logic to figure out fields in an XLFD. This implementation 
     * should suffice for now.
     */
    for (n = name; *n != '\0'; n++)
	if ((*n == '-') && (++dashes == 11)) {
	    n++;
	    if (((*n == 'm') || (*n == 'c') || (*n == 'M') || (*n == 'C')) &&
		(*++n == '-'))
		return(TRUE);
	    else 
		return (FALSE);
	}

    /* was probably a non-XLFD name */
    return(FALSE);
}

