/*      @(#)base.h 50.1 90/12/12 SMI      */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef xview_base_DEFINED
#define xview_base_DEFINED

#include <strings.h>
#include <xview/xv_c_types.h>

#if defined(__cplusplus) || defined(__STDC__)
#include <stdlib.h>
#endif /* __cplusplus || __STDC__ */

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

#define XV_OK		0
#define	XV_ERROR	1

#ifndef TRUE
#define	TRUE		1
#endif

#ifndef FALSE
#define FALSE		0
#endif

#ifndef NULL
#define NULL		0
#endif

#ifndef MIN
#define MIN(x, y) 	( ((x) < (y)) ? (x) : (y) )
#endif

#ifndef MAX
#define MAX(x, y) 	( ((x) > (y)) ? (x) : (y) )
#endif

/*
 * 	These alloc macros should be functions someday with an error call out
 * 	to cleanup, if the underlying malloc fails.
 */
#ifdef MALLOC_DEBUG
#define xv_alloc(t)	((t *)calloc(1, (unsigned)sizeof(t)))
#else
#define xv_alloc(t)	((t *)xv_calloc((unsigned)1, (unsigned)sizeof(t)))
#endif MALLOC_DEBUG
#define xv_alloc_n(t, n) 	((t)calloc(1, (unsigned)n))	/* ??Keep?? */
#define xv_free(s)		((void) free((char *)s))
#define xv_strsave(s)		strcpy(xv_alloc_n(char *, strlen(s)+1), (s))

#define XV_NULL			((Xv_opaque)NULL)

/*
 ***********************************************************************
 *		Typedefs, Enumerations, and Structs
 ***********************************************************************
 */

typedef unsigned long	Xv_opaque;
typedef Xv_opaque	Xv_object;


/*
 ***********************************************************************
 *		Global Functions
 ***********************************************************************
 */

extern int defeat_event_security;

#endif xview_base_DEFINED
