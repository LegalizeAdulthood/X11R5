#ifndef	lint
#ifdef sccs
static char     sccsid[] = "@(#)ndisdsched.c 20.9 90/02/26 Copyr 1985 Sun Micro";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Ndis_d_sched.c - Default scheduler for the dispatcher.
 */

#include <xview_private/ntfy.h>
#include <xview_private/ndis.h>

pkg_private     Notify_value
ndis_default_scheduler(n, nclients)
    int             n;
    register Notify_client *nclients;
{
    register Notify_client nclient;
    register int    i;

    for (i = 0; i < n; i++) {
	nclient = *(nclients + i);
	/* Notify client if haven't been done yet */
	if (nclient != NOTIFY_CLIENT_NULL) {
	    /* notify_client detects errors from nclients */
	    if (notify_client(nclient) != NOTIFY_OK)
		return (NOTIFY_UNEXPECTED);
	    /*
	     * Null out client entry prevents it from being notified again.
	     */
	    *(nclients + i) = NOTIFY_CLIENT_NULL;
	}
    }
    return (NOTIFY_DONE);
}
