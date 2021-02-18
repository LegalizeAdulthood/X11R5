#ifndef	lint
#ifdef sccs
static char     sccsid[] = "@(#)nint_copy.c 20.9 90/02/26 Copyr 1985 Sun Micro";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Nint_interpose.c - Implement the nint_interpose_func private interface.
 */

#include <xview_private/ntfy.h>
#include <xview_private/ndet.h>
#include <xview_private/nint.h>

pkg_private     Notify_error
nint_copy_callout(new_cond, old_cond)
    NTFY_CONDITION *new_cond;
    NTFY_CONDITION *old_cond;
{
    if (old_cond->func_count > 1) {
	if ((new_cond->callout.functions = ntfy_alloc_functions()) ==
	    NTFY_FUNC_PTR_NULL)
	    return (notify_errno);
	bcopy((caddr_t) old_cond->callout.functions,
	      (caddr_t) new_cond->callout.functions, sizeof(NTFY_NODE));
    } else
	new_cond->callout.function = old_cond->callout.function;
    return (NOTIFY_OK);
}
