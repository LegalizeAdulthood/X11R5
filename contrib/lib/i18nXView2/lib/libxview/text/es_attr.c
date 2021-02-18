#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)es_attr.c 20.15 90/04/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Attribute support for entity streams.
 */

#include <sys/types.h>
#include <xview/attrol.h>
#include <xview/pkg.h>
#include <xview_private/primal.h>

#include <xview_private/es.h>

/* VARARGS1 */
Pkg_private int
es_set(esh, va_alist)
    register Es_handle esh;
va_dcl
{

    return (esh->ops->set(esh, &va_alist));
}
