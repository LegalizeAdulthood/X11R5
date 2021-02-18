#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)attr_util.c 50.1 90/12/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <varargs.h>
#include <xview/attr.h>

/*
 * attr_create_list creates an avlist from the VARARGS passed on the stack.
 * The storage is always allocated.
 */
/*VARARGS*/
Attr_avlist
attr_create_list(va_alist)
va_dcl
{
    va_list         valist;
    Attr_avlist     avlist;

    va_start(valist);
    avlist = attr_make((Attr_avlist) 0, 0, valist);
    va_end(valist);
    return avlist;
}

/*
 * attr_find searches and avlist for the first occurrence of a specified
 * attribute.
 */
Attr_avlist
attr_find(attrs, attr)
    register Attr_avlist attrs;
    register Attr_attribute attr;
{
    for (; *attrs; attrs = attr_next(attrs)) {
	if (*attrs == (Xv_opaque) attr)
	    break;
    }
    return (attrs);
}
