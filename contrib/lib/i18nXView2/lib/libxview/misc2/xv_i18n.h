/*       @(#)xv_i18n.h 70.2 91/07/03 SMI        */

/*
 *	(c) Copyright 1990 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef xv_i18n_DEFINED
#define xv_i18n_DEFINED


#ifdef OW_I18N
/*
 ***********************************************************************
 *			Include Files
 ***********************************************************************
 */
 
#include <locale.h>
#include <widec.h>
#include <wctype.h>
#include <X11/Xlib.h>			/* We should remove this - Ako */

#define		ATOI		watoi
#define		CHAR		wchar_t
#define		EXPAND_PATH	expand_path_wc
#define		GETENV		getenv_wc
#define		INDEX		windex
#define 	MALLOC(_size)	((CHAR *)malloc((_size) * sizeof(CHAR)))
#define		RINDEX		windex
#define		SIZEOF(_obj)	(sizeof(_obj) / sizeof(CHAR))
#define		SPRINTF		wsprintf
#define		STRCAT		wscat
#define		STRCMP		wscmp
#define		STRCPY		wscpy
#define		STRLEN		wslen
#define		STRNCAT		wsncat
#define		STRNCMP		wsncmp
#define		STRNCPY		wsncpy
#define 	BCOPY(_from, _to, _len)					\
			(bcopy((_from), (_to), ((_len) * sizeof(CHAR))))
			
#define 	BZERO(_to, _len)					\
			(bzero((_to), ((_len) * sizeof(CHAR))))

#define 	REALLOC(_ptr, _len) 			\
			((CHAR *)realloc((_ptr), ((_len) * sizeof(CHAR))))

#define 	READ(_fd, _buf, _len)					\
			(read((_fd), (_buf), ((_len) * sizeof(CHAR))) / sizeof(CHAR))

#define 	WRITE(_fd, _buf, _len)					\
			(write((_fd), (_buf), ((_len) * sizeof(CHAR))) / sizeof(CHAR))

/*
 * For libxvin/misc/convdup.c
 */
extern wchar_t	*mbstowcsdup();
extern char	*wcstombsdup();
extern wchar_t	*ctstowcsdup();
extern char	*wcstoctsdup();

#else OW_I18N

#define		ATOI		atoi
#define		BCOPY		bcopy
#define		BZERO		bzero
#define		CHAR		char
#define		EXPAND_PATH	expand_path_wc
#define		GETENV		getenv
#define		INDEX		index
#define 	MALLOC		malloc
#define		RINDEX		index
#define		SIZEOF(_obj)	sizeof(_obj)
#define		REALLOC		realloc
#define		READ		read
#define		SPRINTF		sprintf
#define		STRCAT		strcat
#define		STRCMP		strcmp
#define		STRCPY		strcpy
#define		STRLEN		strlen
#define		STRNCAT		strncat
#define		STRNCMP		strncmp
#define		STRNCPY		strncpy
#define		WRITE		write

#endif OW_I18N

#endif xv_i18n_DEFINED
