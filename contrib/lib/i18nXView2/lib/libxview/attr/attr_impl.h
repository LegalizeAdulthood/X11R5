/*      @(#)attr_impl.h 50.1 90/12/12 SMI      */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef _attr_impl_h_already_included
#define	_attr_impl_h_already_included

/*  SunOS 4.0 does not have a guard for including /usr/include/varargs.h 
 *  multiple times, hence adding our own guard.  The #define must be
 *  placed after the include because SunOS 4.1 does have the guard defined.
 */
#ifndef _sys_varargs_h
#include <varargs.h>
#define _sys_varargs_h
#endif

#include <xview/attr.h>

/* NON_PORTABLE means that the var-args list is treated
 * as an avlist.  This is known to work for Sun1/2/3/rise.
 * If the implementation of varargs.h does not have va_arg()
 * equivalent to an array access (e.g. *avlist++), then
 * NON_PORTABLE should NOT be defined.
 */
#define	NON_PORTABLE

/* size of an attribute */
#define	ATTR_SIZE	(sizeof(Xv_opaque))

/* package private routines */
extern Attr_avlist	attr_copy_avlist();
extern int		attr_count_avlist();
extern Attr_avlist	attr_copy_valist();

#endif _attr_impl_h_already_included
