#ifndef	lint
#ifdef sccs
static char     sccsid[] = "@(#)ntfyperror.c 50.4 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Ntfy_perror.c - Notify_perror implementation.
 */

#include <stdio.h>
#include <xview/xv_error.h>
#include <xview_private/ntfy.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

extern void
notify_perror(str)
    char           *str;
{
    register char  *msg;
    char            dummy[128];

    switch (notify_errno) {
#ifdef	OW_I18N
      case NOTIFY_OK:
	msg = (char *)XV_I18N_MSG("xv_messages","Success");
	break;
      case NOTIFY_UNKNOWN_CLIENT:
	msg = (char *)XV_I18N_MSG("xv_messages","Unknown client");
	break;
      case NOTIFY_NO_CONDITION:
	msg = (char *)XV_I18N_MSG("xv_messages","No condition for client");
	break;
      case NOTIFY_BAD_ITIMER:
	msg = (char *)XV_I18N_MSG("xv_messages","Unknown interval timer type");
	break;
      case NOTIFY_BAD_SIGNAL:
	msg = (char *)XV_I18N_MSG("xv_messages","Bad signal number");
	break;
      case NOTIFY_NOT_STARTED:
	msg = (char *)XV_I18N_MSG("xv_messages","Notifier not started");
	break;
      case NOTIFY_DESTROY_VETOED:
	msg = (char *)XV_I18N_MSG("xv_messages","Destroy vetoed");
	break;
      case NOTIFY_INTERNAL_ERROR:
	msg = (char *)XV_I18N_MSG("xv_messages","Notifier internal error");
	break;
      case NOTIFY_SRCH:
	msg = (char *)XV_I18N_MSG("xv_messages","No such process");
	break;
      case NOTIFY_BADF:
	msg = (char *)XV_I18N_MSG("xv_messages","Bad file number");
	break;
      case NOTIFY_NOMEM:
	msg = (char *)XV_I18N_MSG("xv_messages","Not enough memory");
	break;
      case NOTIFY_INVAL:
	msg = (char *)XV_I18N_MSG("xv_messages","Invalid argument");
	break;
      case NOTIFY_FUNC_LIMIT:
	msg = (char *)XV_I18N_MSG("xv_messages","Too many interposition functions");
	break;
      default:
	msg = (char *)XV_I18N_MSG("xv_messages","Unknown notifier error");
#else
      case NOTIFY_OK:
	msg = "Success";
	break;
      case NOTIFY_UNKNOWN_CLIENT:
	msg = "Unknown client";
	break;
      case NOTIFY_NO_CONDITION:
	msg = "No condition for client";
	break;
      case NOTIFY_BAD_ITIMER:
	msg = "Unknown interval timer type";
	break;
      case NOTIFY_BAD_SIGNAL:
	msg = "Bad signal number";
	break;
      case NOTIFY_NOT_STARTED:
	msg = "Notifier not started";
	break;
      case NOTIFY_DESTROY_VETOED:
	msg = "Destroy vetoed";
	break;
      case NOTIFY_INTERNAL_ERROR:
	msg = "Notifier internal error";
	break;
      case NOTIFY_SRCH:
	msg = "No such process";
	break;
      case NOTIFY_BADF:
	msg = "Bad file number";
	break;
      case NOTIFY_NOMEM:
	msg = "Not enough memory";
	break;
      case NOTIFY_INVAL:
	msg = "Invalid argument";
	break;
      case NOTIFY_FUNC_LIMIT:
	msg = "Too many interposition functions";
	break;
      default:
	msg = "Unknown notifier error";
#endif
    }
    (void) sprintf(dummy, "%s: %s", str, msg);
    xv_error(NULL,
	     ERROR_STRING, dummy,
	     0);
}
