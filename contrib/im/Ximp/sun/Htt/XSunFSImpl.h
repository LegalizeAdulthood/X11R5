#ifndef XFontSet_impl_h_DEFINED
#define XFontSet_impl_h_DEFINED

#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunFSImpl.h 1.8 91/08/10";
#endif
#endif

/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL NOTICE
 *      file for terms of the license.
 */


#include <stdio.h>
#include <widec.h>
#include <X11/Xlib.h>
#if XlibSpecificationRelease != 5
#include <X11/XlibR5.h>
#include <X11/XSunExt.h>
#else /* XlibSpecificationRelease != 5 */
#include "Xlcint.h"
#include "XSunExt.h"
#endif /* XlibSpecificationRelease != 5 */

/*
 ***************************************************************** 
 *		Private defines, typedefs, enumerations
 *****************************************************************
 */

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif MAX

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif MIN

#ifndef TRUE
#define TRUE 1
#endif TRUE

#ifndef FALSE
#define FALSE 0
#endif FALSE

/* max length of font name is 255, add 1 for null terminator */
#define MAXFONTLEN	256

/* max font names to be returned for wild card matches */
#define MAX_FONT_NAMES	100

/* corresponding values for the different planes in the LocaleInfo
   structure for tchinese locale.
 */
#define	PLANE_A2	0
#define PLANE_AE	1
#define PLANE_AF	2	
#define PLANE_B0	3

/* max number of planes support for tchinese locale */
#define MAXPLANES	4

/* max number of codeset */
#define MAX_CODE_SET	4

/* code set info macros */
#define IS_CODESET_VALID(cs_info) 	((cs_info) != NULL)
#define IS_CHAR_IN_CODESET(cs_info, c) 	\
		(((cs_info)->mask & c) == (cs_info)->value)

/* font set info macros */
#define IS_FONT_FIXED_WIDTH(fs_info, font) \
	    ((fs_info)->status & (1 << (font)))
#define SET_FONT_FIXED_WIDTH(fs_info, font) \
	    (fs_info)->status |= (1 << (font))



typedef struct CodesetInfo {
    char    **charsets;
    int	    mask;
    int	    value;
    int     mapping_mask;
    int	    width;
} CodesetInfo, PlaneInfo;

typedef struct LocaleInfo {
    struct LocaleInfo   *next;
    char                *locale;
    CodesetInfo		*codesets[MAX_CODE_SET];
    PlaneInfo		*planes[MAXPLANES];
} LocaleInfo;


typedef struct {
#ifndef NO_PLUGGIN
    XFontSetMethods     methods;        /* methods of this font set */
    XFontSetCoreRec     core;           /* core data of this font set */
#endif /* NO_PLUGGIN */
    LocaleInfo	    *locale_info;
    XFontStruct	    *font_structs[MAX_CODE_SET + MAXPLANES]; 
    char	    *font_names[MAX_CODE_SET + MAXPLANES];
    XFontSetExtents  font_set_extents;
    unsigned	    direction;
    unsigned	    status;
} XFontSetStruct;


typedef enum {
    FontNameRegistry,
    FontNameVersion,
    FontNameSuffix,
    FontNameEnd,
    FontNameError
} XLFD_STATE;

extern char 		*strstr(); /* not available in 4.0.3 C library */
extern char		*CharToFontEncoding();
extern int		FindCodeset();
extern int		FindSubStringLength();
extern wchar_t		*_XmbTowcBuffer();
extern int		XwcDrawOrDrawImageString();

extern int		XwcTextEscapement();
extern int		XFreeFontNames();
extern void		XwcDrawString();
extern void		XwcDrawImageString();
extern short		IsXLFDName();
extern int		XwcTextExtents();
extern char**		XListFonts();

#ifndef NO_PLUGGIN
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
extern XFontSet      _XSunCreateFontSet();
#endif /* NO_PLUGGIN */

#endif XFontSet_impl_h_DEFINED
