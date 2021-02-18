/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
 *
 * "Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty."
 *
 */

#include "fig.h"
#include "mode.h"
#include "resources.h"

#define MAXERRORS 6
#define MAXERRMSGLEN 512

static int      error_cnt = 0;

/* VARARGS1 */
put_err(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
    char           *format, *arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7, *arg8;
{
    fprintf(stderr, format, arg1, arg2, arg3, arg4, arg5,
	    arg6, arg7, arg8);
    put_msg(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

error_handler(err_sig)
    int             err_sig;
{
    switch (err_sig) {
    case SIGHUP:
	fprintf(stderr, "xfig: SIGHUP signal trapped\n");
	break;
    case SIGFPE:
	fprintf(stderr, "xfig: SIGFPE signal trapped\n");
	break;
    case SIGBUS:
	fprintf(stderr, "xfig: SIGBUS signal trapped\n");
	break;
    case SIGSEGV:
	fprintf(stderr, "xfig: SIGSEGV signal trapped\n");
	break;
    }
    emergency_quit();
}

X_error_handler(d, err_ev)
    Display        *d;
    XErrorEvent    *err_ev;
{
    char            err_msg[MAXERRMSGLEN];

    XGetErrorText(tool_d, (int) (err_ev->error_code), err_msg, MAXERRMSGLEN - 1);
    (void) fprintf(stderr,
	   "xfig: X error trapped - error message follows:\n%s\n", err_msg);
    emergency_quit();
}

emergency_quit()
{
    if (++error_cnt > MAXERRORS) {
	fprintf(stderr, "xfig: too many errors - giving up.\n");
	exit(-1);
    }
    signal(SIGHUP, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGBUS, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);

    aborting = 1;
    if (figure_modified && !emptyfigure()) {
	fprintf(stderr, "xfig: attempting to save figure\n");
	if (emergency_save("xfig.SAVE") == -1)
	    if (emergency_save("/tmp/xfig.SAVE") == -1)
		fprintf(stderr, "xfig: unable to save figure\n");
    } else
	fprintf(stderr, "xfig: figure empty or not modified - exiting\n");
    quit();
}
