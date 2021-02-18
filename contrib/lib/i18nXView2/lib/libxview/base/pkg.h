/*      @(#)pkg.h 50.3 90/12/12 SMI      */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/* 
 * 	Of interest to package implementors only.
 */

#ifndef xview_pkg_DEFINED
#define xview_pkg_DEFINED

/*
 ***********************************************************************
 *			Include Files
 ***********************************************************************
 */

/* Don't include notify.h - it is not actually used by very many modules, and
 * generates too many symbols in the libraries.
 */

/* SunOS 4.0 does not have a guard against including /usr/include/varags.h
 * multiple times, hence adding our own guard.  The #define must be placed
 * after the include since SunOS 4.1 does have the guard defined.
 */
#ifndef _sys_varargs_h
#include <varargs.h>
#define _sys_varargs_h
#endif

#include <xview/attr.h>
#include <xview/notify.h>

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

/*
 * PRIVATE #defines (for package implementors only)
 */

#ifdef OW_I18N
#ifndef MAX_NESTED_PKGS
#define MAX_NESTED_PKGS 20
#endif
#endif

/*
 * Conversion macros for package implementors (public <=> private)	
 */

/*
 * FIXME: put debugging checks here when implemented 
 */
#define XV_PRIVATE(private_type, public_type, obj) \
	((private_type *)((public_type *) (obj))->private_data)
#define XV_PUBLIC(obj) \
	((obj)->public_self)

/* set procs can return XV_SET_DONE if xv_super_set_avlist()
 * has been called.  This will end the set.  Note that
 * other possible set proc return values are XV_OK or an attribute.
 */
#define	XV_SET_DONE	((Xv_opaque) 2)

#define Sv1_public	extern /* SunView1 compatibility only; part of the 
				* client interface 
				*/
#define Xv_public	extern	/* Part of the client interface */
#define Xv_public_data		/* Part of the client interface */
#define Xv_private	extern	/* Should only be used by Sv toolkit */
#define Xv_private_data		/* Should only be used by Sv toolkit */
#define Pkg_private	extern	/* Should only be used by same pkg */

typedef struct _xview_pkg {
    char                *name;
    Attr_attribute       attr_id;
    unsigned             size_of_object;
    struct _xview_pkg   *parent_pkg;
    int                  (*init)(DOTDOTDOT);
    Xv_opaque            (*set)(DOTDOTDOT);
    Xv_opaque            (*get)(DOTDOTDOT);
    int                  (*destroy)(DOTDOTDOT);
    Xv_object            (*find)(DOTDOTDOT);
} Xv_pkg;

/*
 ***********************************************************************
 *				Globals
 ***********************************************************************
 */

/*
 * PRIVATE functions for package implementors only
 */

EXTERN_FUNCTION (Xv_object xv_create_avlist, (Xv_opaque parent, Xv_pkg *pkg, Attr_attribute *avlist));
EXTERN_FUNCTION (Xv_opaque xv_set_avlist, (Xv_opaque passed_object, Attr_avlist avlist));
EXTERN_FUNCTION (Xv_opaque xv_super_set_avlist, (Xv_opaque object, Xv_pkg *pkg, Attr_avlist avlist));
EXTERN_FUNCTION (int xv_destroy_status, (Xv_object passed_object, Destroy_status status));
EXTERN_FUNCTION (int xv_check_bad_attr, (Xv_pkg *pkg, Attr_attribute attr));
EXTERN_FUNCTION (char *	xv_calloc, (unsigned n, unsigned size));


#endif ~xview_pkg_DEFINED
