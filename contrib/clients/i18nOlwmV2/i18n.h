/*       @(#)i18n.h 1.6 91/08/26; SMI        */

/*
 *	(c) Copyright 1990 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef i18n_DEFINED
#define i18n_DEFINED


#ifdef OW_I18N
 
#include <widec.h>
#include <locale.h>
#include <wctype.h>

#include <X11/Xlib.h>

typedef struct {
	XFontSet	fs;
	XFontSetExtents	*fs_extents;
	char		*fsn;
} XFontSetInfo;

extern wchar_t	*mbstowcsdup();
extern wchar_t	*ctstowcsdup();
extern char	*ctstombsdup();
extern char	*wcstoctsdup();

#endif OW_I18N

#endif i18n_DEFINED


