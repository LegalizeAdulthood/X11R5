#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)pw_traprop.c 50.5 90/11/04 Copyr 1985 Sun Micro";
#endif
#endif
/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * pw_traprop.c traprop scan routine for pixwins built on top of pr_traprop
 * see pr_traprop.c for algorithmic details
 */

#include <xview_private/pw_impl.h>
#include <pixrect/traprop.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

pw_traprop(pw, dx, dy, trap, op, spr, sx, sy)
    register Xv_opaque pw;
    register int    dx, dy;
    struct pr_trap  trap;
    int             op;
    struct pixrect *spr;
    int             sx, sy;
{
    xv_error(NULL,
	     ERROR_STRING, 
#ifdef OW_I18N
	     XV_I18N_MSG("xv_messages", "pw_traprop: not implemented"),
#else
	     "pw_traprop: not implemented",
#endif
	     0);
}
