#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)ei_attr.c 20.14 90/02/26";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Attribute support for entity interpreters.
 */

#include <sys/types.h>
#include <xview/attrol.h>
#include <xview/pkg.h>
#include <xview_private/primal.h>

#include <xview_private/ei.h>

/* VARARGS1 */
Pkg_private int
ei_set(eih, va_alist)
    register Ei_handle eih;
va_dcl
{
    return (eih->ops->set(eih, &va_alist));
}
