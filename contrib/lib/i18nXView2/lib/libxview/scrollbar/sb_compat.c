#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)sb_compat.c 1.17 90/04/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	sb_compat.c
 * 
 * 
 * Include files:
 */

#include <xview_private/sb_impl.h>

/*
 * Declaration of Functions Defined in This File (in order):
 */

Sv1_public Scrollbar scrollbar_create();
Sv1_public int  scrollbar_set();
Sv1_public Xv_opaque scrollbar_get();
Sv1_public int  scrollbar_destroy();
Sv1_public void scrollbar_scroll_to();


/* VARARGS */
Sv1_public      Scrollbar
scrollbar_create(va_alist)
va_dcl
{
    va_list         args;
    Attr_avlist     avlist[ATTR_STANDARD_SIZE];

    va_start(args);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, args);
    va_end(args);

    return (Scrollbar) xv_create(0, SCROLLBAR,
				 ATTR_LIST, avlist,
				 0);
}

/* VARARGS1 */
Sv1_public int
scrollbar_set(sb_public, va_alist)
    Scrollbar       sb_public;
va_dcl
{
    va_list         args;
    Attr_avlist     avlist[ATTR_STANDARD_SIZE];

    va_start(args);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, args);
    va_end(args);
    return (int) xv_set_avlist(sb_public, avlist);
}


Sv1_public      Xv_opaque
scrollbar_get(sb_public, attr)
    Scrollbar       sb_public;
    Attr_attribute  attr;
{
    return xv_get(sb_public, attr);
}


Sv1_public int
scrollbar_destroy(sb_public)
    Scrollbar       sb_public;
{
    xv_destroy(sb_public);
    return 0;
}

Sv1_public void
scrollbar_scroll_to(sb_public, new_start)
    Scrollbar       sb_public;
    unsigned long   new_start;
{
    (void) xv_set(sb_public, SCROLLBAR_VIEW_START, new_start, 0);
}
