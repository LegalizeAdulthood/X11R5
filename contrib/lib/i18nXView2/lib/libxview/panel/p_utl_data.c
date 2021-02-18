#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_utl_data.c 50.1 90/12/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * p_utl_data.c: fix for shared libraries in SunOS4.0.  Code was isolated
 * from p_utl.c.
 */

#include <xview_private/panel_impl.h>

/* caret handling functions */
    void            (*panel_caret_invert_proc) () = (void (*) ()) panel_nullproc;
    void            (*panel_seln_hilite_proc) () = (void (*) ()) panel_nullproc;

/* selection service functions */
    void            (*panel_seln_inform_proc) () = (void (*) ()) panel_nullproc;
    void            (*panel_seln_destroy_proc) () = (void (*) ()) panel_nullproc;

