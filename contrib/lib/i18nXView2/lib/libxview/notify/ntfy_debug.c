#ifndef	lint
#ifdef sccs
static char     sccsid[] = "@(#)ntfy_debug.c 50.4 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Ntfy_debug.c - Debugging routines enabled by NTFY_DEBUG in ntfy.h
 */

#include <stdio.h>
#include <xview/xv_error.h>
#include <xview_private/ntfy.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

pkg_private_data int ntfy_errno_no_print;
pkg_private_data int ntfy_warning_print;

pkg_private_data int ntfy_errno_abort;
pkg_private_data int ntfy_errno_abort_init;

pkg_private void
ntfy_set_errno_debug(error)
    Notify_error    error;
{
    notify_errno = error;
    if ((!ntfy_errno_no_print) && error != NOTIFY_OK)
	notify_perror("Notifier error");
    if (!ntfy_errno_abort_init) {
	extern char    *getenv();
	char           *str = getenv("Notify_error_ABORT");

	if (str && (str[0] == 'y' || str[0] == 'Y'))
	    ntfy_errno_abort = 1;
	else
	    ntfy_errno_abort = 0;
    }
    if (ntfy_errno_abort == 1 && error != NOTIFY_OK)
	abort();
}

pkg_private void
ntfy_set_warning_debug(error)
    Notify_error    error;
{
    notify_errno = error;
    if (ntfy_warning_print && error != NOTIFY_OK)
	notify_perror("Notifier warning");
}

pkg_private void
ntfy_assert_debug(msg)
    char           *msg;
{
    char	   *error_string;

    error_string = malloc(strlen(msg) + strlen("Notifier assertion botched: ")
			  + 2);
    strcpy(error_string, "Notifier assertion botched: ");
    strcat(error_string, msg);
    xv_error(NULL,
	     ERROR_STRING, error_string,
	     0);
    free(error_string);
}

pkg_private void
ntfy_fatal_error(msg)
    char           *msg;
{
    char	   *error_string;

#ifdef	OW_I18N
    error_string = malloc(strlen(msg) + strlen(XV_I18N_MSG("xv_messages","Notifier fatal error: "))
			  + 2);
    strcpy(error_string, XV_I18N_MSG("xv_messages","Notifier fatal error: "));
#else
    error_string = malloc(strlen(msg) + strlen("Notifier fatal error: ")
			  + 2);
    strcpy(error_string, "Notifier fatal error: ");
#endif
    strcat(error_string, msg);
    xv_error(NULL,
	     ERROR_STRING, error_string,
	     0);
    free(error_string);
}
