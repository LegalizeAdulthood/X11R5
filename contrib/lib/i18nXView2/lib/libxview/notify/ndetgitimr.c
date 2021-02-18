#ifndef	lint
#ifdef sccs
static char     sccsid[] = "@(#)ndetgitimr.c 20.9 90/02/26 Copyr 1985 Sun Micro";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Ndet_g_itimer.c - Implement the notify_get_itimer_func interface.
 */

#include <xview_private/ntfy.h>
#include <xview_private/ndet.h>

extern          Notify_func
notify_get_itimer_func(nclient, which)
    Notify_client   nclient;
    int             which;
{
    NTFY_TYPE       type;

    /* Check arguments */
    if (ndet_check_which(which, &type))
	return (NOTIFY_FUNC_NULL);
    return (ndet_get_func(nclient, type, NTFY_DATA_NULL, NTFY_IGNORE_DATA));
}
