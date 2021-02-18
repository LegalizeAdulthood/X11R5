#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)curs_cmpat.c 20.14 90/02/26";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * curs_cmpat.c: Compatibility Routines for creating & modifying a cursor.
 * 
 */

#include <xview_private/curs_impl.h>
#include <xview/notify.h>
#include <xview/window.h>
#include <xview/screen.h>

/*VARARGS0*/
Xv_Cursor
cursor_create(va_alist)
va_dcl
{
    Xv_opaque       avlist[ATTR_STANDARD_SIZE];
    char           *calloc();
    va_list         valist;

    va_start(valist);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist)
	return (xv_create_avlist(XV_NULL, CURSOR, avlist));
}

void
cursor_destroy(cursor_public)
    Xv_Cursor       cursor_public;
{
    (void) xv_destroy(cursor_public);
}

/* cursor_get returns the current value of which_attr. */
Xv_opaque
cursor_get(cursor_public, which_attr)
    Xv_Cursor       cursor_public;
    Cursor_attribute which_attr;
{
    return (xv_get(cursor_public, (Attr_attribute)which_attr));
}

/*VARARGS1*/
int
cursor_set(cursor_public, va_alist)
    Xv_Cursor       cursor_public;
va_dcl
{
    Xv_opaque       avlist[ATTR_STANDARD_SIZE];
    va_list         valist;

    va_start(valist);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);

    return (int) xv_set_avlist(cursor_public, avlist);
}


Xv_Cursor
cursor_copy(cursor_public)
    register Xv_Cursor cursor_public;
{
    return (Xv_Cursor) xv_create(cursor_public, XV_COPY, 0);
}
