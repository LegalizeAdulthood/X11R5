#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunFS.c 1.16 91/08/22";
#endif
#endif

/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL NOTICE
 *      file for terms of the license.
 */

#include <locale.h>
#include <string.h>
#ifdef SVR4
#include <widec.h>
#endif /* SVR4 */
#ifndef SVR4
#include <strings.h>
#endif /* SVR4 */
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#if XlibSpecificationRelease != 5	
#include "XFontSetImpl.h"
#else /* XlibSpecificationRelease != 5 */
#include "Xlcint.h"
#include "XSunFSImpl.h"
#endif /* XlibSpecificationRelease != 5 */

LocaleInfo    *locale_head = NULL;

static char * 	MakePtSizeZero();
static char ** 	ParseBaseFontNameList();
static short          FindFontForCharset();
static XFontStruct            *LoadFontForCharset();
static int            WildCardInFontName();
static char **	SetMissingList();

void XFreeStringList ();
#ifndef NO_PLUGGIN	
extern int	XFreeFontNames();
extern char**   XListFonts();
extern void    	_XSunFreeFontSet();
extern int 	_XSunmbTextEscapement();
extern int    	_XSunmbTextExtents();
extern int    	_XSunmbTextPerCharExtents();
extern void    	_XSunmbDrawString();
extern void    	_XSunmbDrawImageString();
extern int    	_XSunwcTextEscapement();
extern int    	_XSunwcTextExtents();
extern int   	_XSunwcTextPerCharExtents();
extern void    	_XSunwcDrawString();
extern void    	_XSunwcDrawImageString();

static XFontSetMethodsRec fs_methods = {
    _XSunFreeFontSet,
    _XSunmbTextEscapement,
    _XSunmbTextExtents,
    _XSunmbTextPerCharExtents,
    _XSunmbDrawString,
    _XSunmbDrawImageString,
    _XSunwcTextEscapement,
    _XSunwcTextExtents,
    _XSunwcTextPerCharExtents,
    _XSunwcDrawString,
    _XSunwcDrawImageString
};
#endif /* NO_PLUGGIN */ 


/* 
 *	XCreateFontSet()
 * XCreateFontSet() currently assumes that the font name list passed 
 * contains full XLFD conformant names. It does not understand wild cards.
 * In its current form, it is an incomplete implementation of the mltalk 
 * spec.
 */
#ifdef NO_PLUGGIN 
XFontSet
XCreateFontSet(dpy, base_font_name_list, missing_charset_list,
           missing_charset_count, def_string)
    Display	   *dpy;
    char           *base_font_name_list;
    char          **missing_charset_list;
    int            *missing_charset_count;
    char	   **def_string;
{
    char	    **charsets;
    int	    	    code_set, plane;
    XFontSetStruct   *fs_info = NULL;
    XFontStruct	    *font_info = NULL;
    char            *locale = setlocale(LC_CTYPE, (char *)NULL);
    LocaleInfo      *locale_info, *FindLocaleInfo();
    char	    *missing_list[MAX_CODE_SET + MAXPLANES];
    int		    count = 0;
    int		    fixed_width;
    short	    font_found = FALSE;
    char	    *font_name;
    char	    **font_list;

    *missing_charset_list = (char *)NULL;
    *missing_charset_count = 0;
    *def_string = "";
    
    missing_list[0] = (char *)NULL;
    font_list = (char **)ParseBaseFontNameList(base_font_name_list);
    

    if ((locale_info = FindLocaleInfo(locale)) == NULL) {
	fprintf(stderr, 
	    "XCreateFontSet(): locale-specific information not found\n");
	return(NULL);
    }

    for (code_set = 0; code_set <= 3; code_set++) {
	/* check if this codeset is not used in this locale */
	if (locale_info->codesets[code_set] == NULL) 
	    continue;

	/* For Codeset 2 of tchinese locale, we need to work on the planes */
	if ((strcmp(locale, "tchinese") == 0) && (code_set == 2)) {
	   for (plane = 0; plane < MAXPLANES; plane++) {
		/* find the list of valid charsets for this plane */
		charsets = locale_info->planes[plane]->charsets;
		font_info = NULL;
		fixed_width = FALSE;
		for ( ; *charsets && !font_info; charsets++) 
		    font_found = FindFontForCharset(dpy, *charsets, font_list, 
				&font_info, &font_name, &fixed_width);
	
		    if (font_found) {
		        if (!fs_info) {
			    fs_info = (XFontSetStruct *)calloc(1, sizeof(XFontSetStruct));
			    fs_info->locale_info = locale_info;
		        }
		        fs_info->font_structs[MAX_CODE_SET + plane] = font_info;
		        fs_info->font_names[MAX_CODE_SET + plane] = strdup(font_name);
		        if (fixed_width)
			    SET_FONT_FIXED_WIDTH(fs_info, code_set);
		    }  else {
		        /*
		         * returning only the primary charset of this codeset in the 
		         * missing list
		         */
		        missing_list[count++] = *locale_info->planes[plane]->charsets;
		    }
	   }
	} else {
	    /* find the list of valid charsets for this code set */
	    charsets = locale_info->codesets[code_set]->charsets;
	    font_info = NULL;
	    fixed_width = FALSE;
	
	    for ( ; *charsets && !font_info; charsets++) 
	        font_found = FindFontForCharset(dpy, *charsets, font_list, 
				&font_info, &font_name, &fixed_width);

	    if (font_found) {
	        if (!fs_info) {
		    fs_info = (XFontSetStruct *)calloc(1, sizeof(XFontSetStruct));
		    fs_info->locale_info = locale_info;
	        }
	        fs_info->font_structs[code_set] = font_info;
	        fs_info->font_names[code_set] = strdup(font_name);
	        if (fixed_width == TRUE)
		    SET_FONT_FIXED_WIDTH(fs_info, code_set);
	    }  else {
	        /*
	         * returning only the primary charset of this codeset in the 
	         * missing list
	         */
	        missing_list[count++] = *locale_info->codesets[code_set]->charsets;
	    }
	
	}
    }

    if (missing_list[0] != NULL) {
	*missing_charset_list = (char **)SetMissingList(missing_list, count);
	(*missing_charset_list)[count] = NULL;
	*missing_charset_count = count;
    }

    if (fs_info != NULL) 
	SetFontsetInfo(fs_info);
	
    XFreeStringList (font_list);
    
    return((XFontSet)fs_info);
}
#else /* NO_PLUGGIN */
extern XFontSet
_XSunCreateFontSet(lcd, dpy, base_font_name_list, font_list, font_name_count,
           missing_charset_list, missing_charset_count)
    XLCd 	   lcd;
    Display	   *dpy;
    char           *base_font_name_list;
    char	    **font_list;
    int		    font_name_count;
    char          ***missing_charset_list;
    int            *missing_charset_count;
{
    char	    **charsets;
    int	    	    code_set, plane;
    XFontSetStruct   *fs_info = NULL;
    XFontStruct	    *font_info = NULL;
    char            *locale = setlocale(LC_CTYPE, (char *)NULL);
    LocaleInfo      *locale_info, *FindLocaleInfo();
    char	    *missing_list[MAXPLANES + MAX_CODE_SET];
    int		    count = 0;
    int		    fixed_width;
    short	    font_found = FALSE;
    char	    *font_name;

    *missing_charset_list = NULL;
    *missing_charset_count = 0;
    
    missing_list[0] = (char *)NULL;
    

    if ((locale_info = FindLocaleInfo(locale)) == NULL) {
	fprintf(stderr, 
	    "XCreateFontSet(): locale-specific information not found\n");
	return(NULL);
    }

    for (code_set = 0; code_set <= 3; code_set++) {
	/* check if this codeset is not used in this locale */
	if (locale_info->codesets[code_set] == NULL) 
	    continue;

	/* For Codeset 2 of tchinese locale, we need to work on the planes */
	if ((strcmp(locale, "tchinese") == 0) && (code_set == 2)) {
	   for (plane = 0; plane < MAXPLANES; plane++) {
		/* find the list of valid charsets for this plane */
		charsets = locale_info->planes[plane]->charsets;
		font_info = NULL;
		fixed_width = FALSE;
		for ( ; *charsets && !font_info; charsets++) 
		    font_found = FindFontForCharset(dpy, *charsets, font_list, 
				&font_info, &font_name, &fixed_width);
	
		    if (font_found) {
		        if (!fs_info) {
			    fs_info = (XFontSetStruct *)calloc(1, sizeof(XFontSetStruct));
			    fs_info->locale_info = locale_info;
		        }
		        fs_info->font_structs[MAX_CODE_SET + plane] = font_info;
		        fs_info->font_names[MAX_CODE_SET + plane] = strdup(font_name);
		        if (fixed_width)
			    SET_FONT_FIXED_WIDTH(fs_info, code_set);
		    }  else {
		        /*
		         * returning only the primary charset of this codeset in the 
		         * missing list
		         */
		        missing_list[count++] = *locale_info->planes[plane]->charsets;
		    }
	   }
	} else {
	    /* find the list of valid charsets for this code set */
	    charsets = locale_info->codesets[code_set]->charsets;
	    font_info = NULL;
	    fixed_width = FALSE;
	
	    for ( ; *charsets && !font_info; charsets++) 
	        font_found = FindFontForCharset(dpy, *charsets, font_list, 
				&font_info, &font_name, &fixed_width);

	    if (font_found) {
	        if (!fs_info) {
		    fs_info = (XFontSetStruct *)calloc(1, sizeof(XFontSetStruct));
		    fs_info->locale_info = locale_info;
	        }
	        fs_info->font_structs[code_set] = font_info;
	        fs_info->font_names[code_set] = strdup(font_name);
	        if (fixed_width == TRUE)
		    SET_FONT_FIXED_WIDTH(fs_info, code_set);
	    }  else {
	        /*
	         * returning only the primary charset of this codeset in the 
	         * missing list
	         */
	        missing_list[count++] = *locale_info->codesets[code_set]->charsets;
	    }
	
	}
    }
    
/* PLUGGIN begin */
    if (fs_info) {
    	int		i;
    	
        fs_info->methods = (XFontSetMethods) &fs_methods;
	fs_info->core.lcd = lcd;
	fs_info->core.base_name_list = base_font_name_list;
    	fs_info->core.num_of_fonts = MAX_CODE_SET + MAXPLANES;  
    	/* 
    	 * We should rewrite this code in the future, since
    	 * we should be using fs_info->core.font_name_list and
    	 * fs_info->core.font_struct_list instead of fs_info->font_names and
    	 * fs_info->font_structs
    	 */
    	fs_info->core.font_name_list = (char **)Xmalloc((fs_info->core.num_of_fonts) * sizeof(char *));
    	for (i = 0; i < fs_info->core.num_of_fonts; i++) {
    	    fs_info->core.font_name_list[i] = (fs_info->font_names[i]) ? 
    	    	                               strdup(fs_info->font_names[i]) : NULL;
    	    
    	}
    	
    	fs_info->core.font_struct_list = (XFontStruct **)Xmalloc((fs_info->core.num_of_fonts) * sizeof(XFontStruct *));
    	for (i = 0; i < fs_info->core.num_of_fonts; i++) {
    	    if (fs_info->font_structs[i]) {
    	        fs_info->core.font_struct_list[i] = (XFontStruct *)Xmalloc(sizeof(XFontStruct));
#ifdef SVR4
    	        memmove(fs_info->core.font_struct_list[i], fs_info->font_structs[i], sizeof(XFontStruct));
#else
    	        bcopy(fs_info->font_structs[i], fs_info->core.font_struct_list[i], sizeof(XFontStruct));
#endif /* SVR4 */
    	    } else {
		fs_info->core.font_struct_list[i] = NULL;
	    }
    	}
    	/* default_string should be set also */
    	fs_info->core.context_dependent = FALSE;
    }  
/* PLUGGIN end */
    
    if (missing_list[0] != NULL) {
	*missing_charset_list = (char **)SetMissingList(missing_list, count);
	(*missing_charset_list)[count] = NULL;
	*missing_charset_count = count;
    }

    if (fs_info != NULL) {
	SetFontsetInfo(fs_info);
    }
	    
    return((XFontSet)fs_info);
}
#endif /* NO_PLUGGIN */

#ifdef NO_PLUGGIN
static char **
ParseBaseFontNameList(base_font_name_list)
    char	* base_font_name_list;
{
    char	*temp_ptr_1, *temp_ptr_2;
    char	**return_ptr; 
    int		i = 0, count = 0; 
    
    /* Count the number of filename */
    temp_ptr_1 = base_font_name_list;
    if (temp_ptr_1)
        count = 1;	/* Should be at least one name */
    while (temp_ptr_1) {
#ifdef SVR4
        temp_ptr_1 = strchr(temp_ptr_1, ',');
#else
        temp_ptr_1 = index(temp_ptr_1, ',');
#endif /* SVR4 */
        if (temp_ptr_1) {
            count++;
            temp_ptr_1++;
        } else
            break; 
    }
    
    if (count == 0)
        return((char **)NULL);
        
    return_ptr =  (char **)Xmalloc(sizeof(char *) * (count + 1));
    
    temp_ptr_1 = base_font_name_list;
    for (i = 0; i < count; i++) {
        char	*temp;
        int	char_count;
        
        if (i < (count-1)) {
#ifdef SVR4
            temp_ptr_2 = strchr(temp_ptr_1, ',');            
#else
            temp_ptr_2 = index(temp_ptr_1, ',');            
#endif /* SVR4 */
            temp = temp_ptr_2 + 1;  /* Point to the next name */
        } else {
#ifdef SVR4
            temp_ptr_2 = strchr(temp_ptr_1,NULL);
#else
            temp_ptr_2 = index(temp_ptr_1,NULL);
#endif /* SVR4 */
            char_count =  (int)(temp_ptr_2  - temp_ptr_1);
        }            
        
        char_count =  (int)(temp_ptr_2  - temp_ptr_1) + 1;
         
         /* eliminate white space */
         while (isspace(*(temp_ptr_2 - 1))) {
                temp_ptr_2--;
                char_count--;
         }
            
         return_ptr[i] = (char *)Xmalloc(char_count);
         strncpy(return_ptr[i], temp_ptr_1, char_count - 1);
         return_ptr[i][char_count - 1] = NULL;            
         temp_ptr_1 = temp;
    }
    return_ptr[i] = (char *) NULL;
    return(return_ptr);
}

#endif /* NO_PLUGGIN */

void
SetCodeSetInfoC(info, codeset, localeInfo)
    CodesetInfo    *info;
    int		   codeset;
    LocaleInfo     *localeInfo;   /* Not used */
{
    switch(codeset) {
	case 0:
	    info->charsets = (char **)malloc(5 * sizeof(char *));
	    info->charsets[0] = "iso8859-1";
	    info->charsets[1] = "jisx0201.1976-0";
	    info->charsets[2] = "sunolcursor-1";
	    info->charsets[3] = "sunolglyph-1";
	    info->charsets[4] = NULL;
	    info->mask = 0x8080;
	    info->value = 0x0000;
	    info->mapping_mask = 0x007f;
	    info->width = 1;
	    break;

	case 1: 
	    info->charsets = (char **)malloc(4 * sizeof(char *));
	    info->charsets[0] = "iso8859-1";
	    info->charsets[1] = "sunolcursor-1";
	    info->charsets[2] = "sunolglyph-1";
	    info->charsets[3] = NULL;
            info->mask = 0x8080;
   	    info->value = 0x8080;
            info->mapping_mask = 0x00ff;
	    info->width = 1;
            break;

	default:
 	    break;
    }
}

void
SetCodeSetInfoJapanese(info, codeset, locale_info)
    CodesetInfo    *info;
    int		   codeset;
    LocaleInfo	   *locale_info;	/* Not used */
{
    switch(codeset) {
	case 0:
	    info->charsets = (char **)malloc(5 * sizeof(char *));
	    info->charsets[0] = "iso8859-1";
	    info->charsets[1] = "jisx0201.1976-0";
	    info->charsets[2] = "sunolcursor-1";
	    info->charsets[3] = "sunolglyph-1";
	    info->charsets[4] = NULL;
	    info->mask = 0x8080;
	    info->value = 0x0000;
	    info->mapping_mask = 0x007f;
	    info->width = 1;
	    break;

	case 1: 
	    info->charsets = (char **)malloc(2 * sizeof(char *));
	    info->charsets[0] = "jisx0208.1983-0";
	    info->charsets[1] = NULL;
            info->mask = 0x8080;
   	    info->value = 0x8080;
            info->mapping_mask = 0x7f7f;
	    info->width = 2;
            break;

    	case 2:
	    info->charsets = (char **)malloc(2 * sizeof(char *));
	    info->charsets[0] = "jisx0201.1976-0";
	    info->charsets[1] = NULL;
            info->mask = 0x8080;
            info->value = 0x0080;
            info->mapping_mask = 0x00ff;
	    info->width = 2;
            break;

	default:
	    break;
    }
}

void
SetCodeSetInfoKorean(info, codeset, locale_info)
    CodesetInfo    *info;
    int		   codeset;
    LocaleInfo	   *locale_info;	/* Not used */

{
    switch(codeset) {
	case 0:
	    info->charsets = (char **)malloc(5 * sizeof(char *));
	    info->charsets[0] = "iso8859-1";
	    info->charsets[1] = "ksc5636-0";
	    info->charsets[2] = "sunolcursor-1";
	    info->charsets[3] = "sunolglyph-1";
	    info->charsets[4] = NULL;
	    info->mask = 0x8080;
	    info->value = 0x0000;
	    info->mapping_mask = 0x007f;
	    info->width = 1;
	    break;

	case 1: 
	    info->charsets = (char **)malloc(2 * sizeof(char *));
	    info->charsets[0] = "ksc5601.1987-0";
	    info->charsets[1] = NULL;
            info->mask = 0x8080;
   	    info->value = 0x8080;
            info->mapping_mask = 0x7f7f;
	    info->width = 2;
            break;

	default:
	    break;
    }
}

void
SetCodeSetInfoChinese(info, codeset, locale_info)
    CodesetInfo    *info;
    int		   codeset;
    LocaleInfo	   *locale_info;	/* Not used */
    
{
    switch(codeset) {
	case 0:
	    info->charsets = (char **)malloc(5 * sizeof(char *));
	    info->charsets[0] = "iso8859-1";
	    info->charsets[1] = "gb2312.1980-0";
	    info->charsets[2] = "sunolcursor-1";
	    info->charsets[3] = "sunolglyph-1";
	    info->charsets[4] = NULL;
	    info->mask = 0x8080;
	    info->value = 0x0000;
	    info->mapping_mask = 0x007f;
	    info->width = 1;
	    break;

	case 1: 
	    info->charsets = (char **)malloc(2 * sizeof(char *));
	    info->charsets[0] = "gb2312.1980-1";
	    info->charsets[1] = NULL;
            info->mask = 0x8080;
   	    info->value = 0x8080;
            info->mapping_mask = 0x7f7f;
	    info->width = 2;
            break;

	default:
	    break;
    }
}

void
SetCodeSetInfoTchinese(info, codeset, locale_info)
    CodesetInfo    *info;
    int		   codeset;
    LocaleInfo	   *locale_info;	
{

    PlaneInfo	*p_info;

    /* Note: tchinese use 4 byte wchar_t */
    switch(codeset) {
	case 0:
	    info->charsets = (char **)malloc(5 * sizeof(char *));
	    info->charsets[0] = "iso8859-1";
	    info->charsets[1] = "cns11643-0";
	    info->charsets[2] = "sunolcursor-1";
	    info->charsets[3] = "sunolglyph-1";
	    info->charsets[4] = NULL;
	    info->mask = 0xff000000;
	    info->value = 0x00000000;
	    info->mapping_mask = 0x007f;
	    info->width = 1;
	    break;

	case 1: 
	    info->charsets = (char **)malloc(2 * sizeof(char *));
	    info->charsets[0] = "cns11643-1";
	    info->charsets[1] = NULL;
            info->mask = 0x60000000;
   	    info->value = 0x60000000;
            info->mapping_mask = 0x3fff;
	    info->width = 4;
            break;

    	case 2:
	    info->charsets = (char **)malloc(1 * sizeof(char *));
	    info->charsets[0] = NULL;
            info->mask = 0;
            info->value = 0;
            info->mapping_mask = 0;
	    info->width = 4;

	    /* Plane A2 */
	    p_info = locale_info->planes[PLANE_A2];
	    p_info->charsets = (char **)malloc(2 * sizeof(char *));
	    p_info->charsets[0] = "cns11643-2";
	    p_info->charsets[1] = NULL;
            p_info->mask = 0x601fc000;
            p_info->value = 0x20088000;
            p_info->mapping_mask = 0x3fff;
	    p_info->width = 4;

	    /* Plane AE  */
	    p_info = locale_info->planes[PLANE_AE];
	    p_info->charsets = (char **)malloc(2 * sizeof(char *));
	    p_info->charsets[0] = "cns11643-14";
	    p_info->charsets[1] = NULL;
            p_info->mask = 0x601fc000;
            p_info->value = 0x200b8000;
            p_info->mapping_mask = 0x3fff;
	    p_info->width = 4;

	    /* Plane AF  */
	    p_info = locale_info->planes[PLANE_AF];
	    p_info->charsets = (char **)malloc(2 * sizeof(char *));
	    p_info->charsets[0] = "cns11643-15";
	    p_info->charsets[1] = NULL;
            p_info->mask = 0x601fc000;
            p_info->value = 0x200bc000;
            p_info->mapping_mask = 0x3fff;
	    p_info->width = 4;

	    /* Plane B0  */
	    p_info = locale_info->planes[PLANE_B0];
	    p_info->charsets = (char **)malloc(2 * sizeof(char *));
	    p_info->charsets[0] = "cns11643-16";
	    p_info->charsets[1] = NULL;
            p_info->mask = 0x601fc000;
            p_info->value = 0x200c0000;
            p_info->mapping_mask = 0x3fff;
	    p_info->width = 4;
            break;

	default:
	    break;
    }
}    


LocaleInfo *
FindLocaleInfo(locale)
    char    *locale;
{
    LocaleInfo      *locale_info = NULL;
    int             cs_num;
    void            (*cs_info_proc)();
    register int    i;

    for (locale_info = locale_head; locale_info != NULL; 
	    locale_info = locale_info->next)
	if (strcmp(locale_info->locale, locale) == 0)
	    break;

    if (locale_info == NULL) {
	locale_info = (LocaleInfo *)calloc(1, sizeof(LocaleInfo));
	locale_info->next = locale_head;
	locale_head = locale_info;

	if (strcmp(locale, "C") == 0) {
            cs_num = 2;
            cs_info_proc = SetCodeSetInfoC;
        } else if (strcmp(locale, "japanese") == 0) {
            cs_num = 3;
            cs_info_proc = SetCodeSetInfoJapanese;
        } else if (strncmp(locale, "korean", 6) == 0) {
            cs_num = 2;
            cs_info_proc = SetCodeSetInfoKorean;
        } else if (strcmp(locale, "chinese") == 0) {
            cs_num = 2;
            cs_info_proc = SetCodeSetInfoChinese;
        } else if (strcmp(locale, "tchinese") == 0) {
            cs_num = 3;
            cs_info_proc = SetCodeSetInfoTchinese;
        } else {
            fprintf(stderr, "No locale info for locale %s\n", locale);
            free(locale_info);
            return(NULL);
        }
 
        for (i = 0; i < cs_num; i++) {
            locale_info->codesets[i] =
                (CodesetInfo *)malloc(sizeof(CodesetInfo));
            (*cs_info_proc)(locale_info->codesets[i], i, locale_info);
        }
 
        locale_info->locale = strdup(locale);
    }
             
    return(locale_info);
}

SetFontsetInfo(fs_info)
    XFontSetStruct	*fs_info;
{
    SetFontSetBounds(fs_info);
    SetFontSetDirection(fs_info);
}

SetFontSetDirection(fs_info)
    XFontSetStruct      *fs_info;
{
    register int    i, FLTR = 0, FRTL = 0;
#ifdef NO_PLUGGIN    
    XFontStruct     **font_structs = fs_info->font_structs;
#else /* NO_PLUGGIN */
    XFontStruct     **font_structs = fs_info->core.font_struct_list;
#endif /* NO_PLUGGIN */    


    for (i = 0 ; i < (4 + MAXPLANES); i++) {
	if (font_structs[i] == NULL)
	    continue;
	if (font_structs[i]->direction == FontLeftToRight)
	    FLTR++;
	if (font_structs[i]->direction == FontRightToLeft)             
	    FRTL++; 
    }

    fs_info->direction = (FLTR > FRTL) ? FontLeftToRight : FontRightToLeft;
}


static 
SetFontSetBounds(fs_info)
    XFontSetStruct	*fs_info;
{
    register int    i;
    XCharStruct	    min_bounds, max_bounds;
#ifdef NO_PLUGGIN    
    XFontStruct     **font_structs = fs_info->font_structs;
#else /* NO_PLUGGIN */
    XFontStruct     **font_structs = fs_info->core.font_struct_list;
#endif /* NO_PLUGGIN */    
    short	    ascent, descent;

    for (i = 0 ; i <= (3 + MAXPLANES); i++) 
	if (font_structs[i] != NULL) {
            min_bounds = font_structs[i]->min_bounds;
            max_bounds = font_structs[i]->max_bounds;
            ascent = font_structs[i]->ascent;
            descent = font_structs[i]->descent;
	    break;
	}

    for (++i ; i <= (3 + MAXPLANES); i++) {
	if (font_structs[i] == NULL)
	    continue;

	min_bounds.lbearing = 
	    MIN(min_bounds.lbearing, font_structs[i]->min_bounds.lbearing);
	min_bounds.rbearing = 
	    MIN(min_bounds.rbearing, font_structs[i]->min_bounds.rbearing);
	min_bounds.width = 
	    MIN(min_bounds.width, font_structs[i]->min_bounds.width);    
	min_bounds.ascent = 
	    MIN(min_bounds.ascent, font_structs[i]->min_bounds.ascent);
	min_bounds.descent = 
	    MIN(min_bounds.descent, font_structs[i]->min_bounds.descent);

	max_bounds.lbearing = 
	    MAX(max_bounds.lbearing, font_structs[i]->max_bounds.lbearing);
	max_bounds.rbearing = 
	    MAX(max_bounds.rbearing, font_structs[i]->max_bounds.rbearing);
	max_bounds.width = 
	    MAX(max_bounds.width, font_structs[i]->min_bounds.width);    
	max_bounds.ascent = 
	    MAX(max_bounds.ascent, font_structs[i]->max_bounds.ascent);
	max_bounds.descent = 
	    MAX(max_bounds.descent, font_structs[i]->max_bounds.descent);

	ascent = MAX(ascent, font_structs[i]->ascent);
	descent = MAX(descent, font_structs[i]->descent);
    }
#ifdef NO_PLUGGIN      
    /*
     * set the ink bounding box of font_set.
     */
    fs_info->font_set_extents.max_ink_extent.x = min_bounds.lbearing;
    fs_info->font_set_extents.max_ink_extent.y = - max_bounds.ascent;
    fs_info->font_set_extents.max_ink_extent.width =
            max_bounds.rbearing - min_bounds.lbearing;
    fs_info->font_set_extents.max_ink_extent.height =
            max_bounds.ascent + max_bounds.descent;
    /*
     * set the logical bounding box of font_set.
     * suppose the lbearing >= 0;
     */
    fs_info->font_set_extents.max_logical_extent.x = 0;
    fs_info->font_set_extents.max_logical_extent.y =
            - ascent;
    fs_info->font_set_extents.max_logical_extent.width =
            max_bounds.width;
    fs_info->font_set_extents.max_logical_extent.height =
            ascent + descent;
#else /* NO_PLUGGIN */
    fs_info->core.font_set_extents.max_ink_extent.x =
    fs_info->font_set_extents.max_ink_extent.x = min_bounds.lbearing;
    fs_info->core.font_set_extents.max_ink_extent.y =
    fs_info->font_set_extents.max_ink_extent.y = - max_bounds.ascent;
    fs_info->core.font_set_extents.max_ink_extent.width =
    fs_info->font_set_extents.max_ink_extent.width =
            max_bounds.rbearing - min_bounds.lbearing;
    fs_info->core.font_set_extents.max_ink_extent.height =       
    fs_info->font_set_extents.max_ink_extent.height =
            max_bounds.ascent + max_bounds.descent;
    /*
     * set the logical bounding box of font_set.
     * suppose the lbearing >= 0;
     */
    fs_info->core.font_set_extents.max_logical_extent.x = 
    fs_info->font_set_extents.max_logical_extent.x = 0;
    fs_info->core.font_set_extents.max_logical_extent.y =
    fs_info->font_set_extents.max_logical_extent.y =
            - ascent;
    fs_info->core.font_set_extents.max_logical_extent.width =       
    fs_info->font_set_extents.max_logical_extent.width =
            max_bounds.width;
    fs_info->core.font_set_extents.max_logical_extent.height =       
    fs_info->font_set_extents.max_logical_extent.height =
            ascent + descent;
#endif  /* NO_PLUGGIN */
}

void
XFreeMissingCharsets(list)
    char **list;
{
    if (list == NULL)
        return;

    /* 
     * The actual charset strings are allocated statically and 
     * must not be freed.
     */
    free(list);
}

extern void
#ifdef NO_PLUGGIN  
XFreeFontSet(dpy, font_set)
#else /* NO_PLUGGIN */
_XSunFreeFontSet(dpy, font_set)
#endif  /* NO_PLUGGIN */
    Display     *dpy;
    XFontSet    font_set;
{
#ifdef NO_PLUGGIN
/* The pluggable layer will free the font */
    XFontSetStruct    *fs_info = (XFontSetStruct *)font_set;
    register int      i;

    if (font_set == NULL)
        return;

    /*
     * Dont free the locale information of the font set since
     * other font sets may be using it.
     */
    for (i = 0; i <= (3 + MAXPLANES); i++)
        if (fs_info->font_structs[i] != NULL)
            XFreeFont(dpy, fs_info->font_structs[i]);
    free(fs_info);
#endif /* NO_PLUGGIN */    
}

static short
FindFontForCharset(dpy, charset, base_font_list, font_info, font_name, 
	fixed_width)
    Display	*dpy;
    char	*charset;
    char	**base_font_list;
    XFontStruct **font_info;
    char	**font_name;
    int		*fixed_width;
{
    char	    **tmp_list;
    char	    *str;
    char	    tmp_name[MAXFONTLEN];
    register int    i = 0;
    
    *font_info = (XFontStruct *)NULL;
    *font_name = (char *)NULL;

    /* First pass: Examine all XLFD conforming names only.*/
    for(tmp_list = base_font_list; *tmp_list ; tmp_list++) {
	if (IsXLFDName(*tmp_list) == FALSE) 
	    continue;

	/* 
	 * check to see if charset registry and encoding are explicitly 
	 * specified. If not, construct a font name using the passed in 
	 * font name and the desired charset.
	 */
	str = *tmp_list;
	for (i = 0; i <= 12; str++)
	    if (*str == '-')
		++i;

#ifdef SVR4
	if (strchr(str, '*') || strchr(str, '?') || strstr(str,"--")) {
#else
	if (index(str, '*') || index(str, '?') || strstr(str,"--")) {
#endif /* SVR4 */
	    strncpy(tmp_name, *tmp_list, MAXFONTLEN - 1);
	    str = tmp_name; 
	    for (i = 0; i <= 12; str++)
		if (*str == '-') 
		    ++i;
	    *str = NULL;
	    strcat(tmp_name, charset);
	    *font_info = LoadFontForCharset(dpy, charset, tmp_name);
 	} else
	    *font_info = LoadFontForCharset(dpy, charset, *tmp_list);

	if (*font_info != NULL) {
	    *font_name = *tmp_list;
	    *fixed_width = IsFontFixedWidth(*tmp_list);

#define X11NEWS_BUG_WORKAROUND
#ifdef X11NEWS_BUG_WORKAROUND
	    /* 
	     * Workaround for bug on X11/NeWS (id 1063406) wherein it returns 
	     * zeros for all fields in the min_mounds of a jisx0208.1983-0
	     * font.
	     */
	    if (((*font_info)->per_char == NULL) && 
			((*font_info)->min_bounds.width <= 0))
		memcpy(&(*font_info)->min_bounds, 
		    &(*font_info)->max_bounds, sizeof(XCharStruct)); 
#endif /*X11NEWS_BUG_WORKAROUND*/

	    return(TRUE);
	}
    }

    return(FALSE);
}


static short
IsXLFDName(font_name)
    char    *font_name;
{
    register int i = 0;

    if (font_name == NULL)
        return(FALSE);

    while(isspace(*font_name))
        ++font_name;

    if (*font_name != '-')
        return(FALSE);

    while(*font_name) {
        if (*font_name == '-')
            ++i;
        ++font_name;
    }

    if (i != 14)
        return(FALSE);
    else
        return(TRUE);
}
    
static XFontStruct *
LoadFontForCharset(dpy, charset, font_name)
    Display     *dpy; 
    char        *charset;
    char	*font_name;
{
    XFontStruct	*font_info = NULL;
    int		count;

    if (strstr(font_name, charset) != NULL) {
	/* 
	 * If a wildcard is present in the font name, there is no 
	 * guarantee that XListFontsWithInfo() with the font name returns 
	 * the font info for the same font as the one returned by 
	 * XLoadFont() subsequently. Ofcourse, if the font is not fixed 
	 * width, XLoadQueryFont() must be called to get per char info.
	 */
	if (WildCardInFontName(font_name) 
		|| (IsFontFixedWidth(font_name) == FALSE)) {
	    font_info = XLoadQueryFont(dpy, font_name);
	} else {
	    XListFontsWithInfo(dpy, font_name, 1, &count, &font_info);
	    if (count > 0)
		font_info->fid = XLoadFont(dpy, font_name);
	}
    }

    return(font_info);
}

static int
WildCardInFontName(name)
    char    *name;
{
#ifdef SVR4
    if (strchr(name, '*') || strchr(name, '?'))
#else
    if (index(name, '*') || index(name, '?'))
#endif /* SVR4 */
	return(TRUE);
    else 
	return(FALSE);
}

static char **
SetMissingList(name_list, count)
	char	*name_list[];
	int	count;	
{
	char	**missing_list = NULL;
	char    *missing_names;
	int	i, total_len = 0;
	
	for (i = 0; i < count; i++) {
	    if (name_list[i])
	       total_len += (strlen(name_list[i]) + 1);
	}
	
	if (total_len > 0) {
	    missing_list = (char **)Xmalloc(sizeof(char *) * count);
	    missing_names = (char *)Xmalloc(sizeof(char) * total_len);	
	    
	    for (i = 0; i < count; i++) {
	        missing_list[i] = missing_names;
	        strcpy(missing_list[i], name_list[i]); 
	        missing_names += (strlen(name_list[i]) + 1);
	    }    
	}
	return(missing_list);
	
}
