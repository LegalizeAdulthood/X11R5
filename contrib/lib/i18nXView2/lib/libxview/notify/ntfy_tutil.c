#ifndef	lint
#ifdef sccs
static char     sccsid[] = "@(#)ntfy_tutil.c 50.4 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Ntfy_tutil.c - Utilities for ntfy_test.
 */
#include <signal.h>
#include <stdio.h>
#include <ctype.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

nt_sig(sigstr)
    register char  *sigstr;
{
    register        sig;

    if (strcmp(sigstr, "SIGHUP") == 0)
	sig = 1;
    else if (strcmp(sigstr, "SIGINT") == 0)
	sig = 2;
    else if (strcmp(sigstr, "SIGQUIT") == 0)
	sig = 3;
    else if (strcmp(sigstr, "SIGILL") == 0)
	sig = 4;
    else if (strcmp(sigstr, "SIGTRAP") == 0)
	sig = 5;
    else if (strcmp(sigstr, "SIGIOT") == 0)
	sig = 6;
    else if (strcmp(sigstr, "SIGEMT") == 0)
	sig = 7;
    else if (strcmp(sigstr, "SIGFPE") == 0)
	sig = 8;
    else if (strcmp(sigstr, "SIGKILL") == 0)
	sig = 9;
    else if (strcmp(sigstr, "SIGBUS") == 0)
	sig = 10;
    else if (strcmp(sigstr, "SIGSEGV") == 0)
	sig = 11;
    else if (strcmp(sigstr, "SIGSYS") == 0)
	sig = 12;
    else if (strcmp(sigstr, "SIGPIPE") == 0)
	sig = 13;
    else if (strcmp(sigstr, "SIGALRM") == 0)
	sig = 14;
    else if (strcmp(sigstr, "SIGTERM") == 0)
	sig = 15;
    else if (strcmp(sigstr, "SIGURG") == 0)
	sig = 16;
    else if (strcmp(sigstr, "SIGSTOP") == 0)
	sig = 17;
    else if (strcmp(sigstr, "SIGTSTP") == 0)
	sig = 18;
    else if (strcmp(sigstr, "SIGCONT") == 0)
	sig = 19;
    else if (strcmp(sigstr, "SIGCHLD") == 0)
	sig = 20;
    else if (strcmp(sigstr, "SIGTTIN") == 0)
	sig = 21;
    else if (strcmp(sigstr, "SIGTTOU") == 0)
	sig = 22;
    else if (strcmp(sigstr, "SIGIO") == 0)
	sig = 23;
    else if (strcmp(sigstr, "SIGXCPU") == 0)
	sig = 24;
    else if (strcmp(sigstr, "SIGXFSZ") == 0)
	sig = 25;
    else if (strcmp(sigstr, "SIGVTALRM") == 0)
	sig = 26;
    else if (strcmp(sigstr, "SIGPROF") == 0)
	sig = 27;
    else if (strcmp(sigstr, "SIGWINCH") == 0)
	sig = 28;
    else {
#ifdef	OW_I18N
	fprintf(stderr, XV_I18N_MSG("xv_messages","%s is an unknown signal\n"), sigstr);
#else
	fprintf(stderr, "%s is an unknown signal\n", sigstr);
#endif
	sig = atoi(sigstr);
    }
    return (sig);
}

nt_constructargs(args, programname, otherargs, maxargcount)
    char           *args[], *programname, *otherargs;
    int             maxargcount;
{
#define terminatearg() {*cpt = NULL;needargstart = 1;}
#define STRINGQUOTE     '"'
    int             argindex = 0, needargstart = 1, quotedstring = 0;
    register char  *cpt;

    args[argindex++] = programname;
    for (cpt = otherargs; (cpt != 0) && (*cpt != NULL); cpt++) {
	if (quotedstring) {
	    if (*cpt == STRINGQUOTE) {
		terminatearg();
		quotedstring = 0;
	    } else {		/* Accept char in arg */
	    }
	} else if (isspace(*cpt)) {
	    terminatearg();
	} else {
	    if (needargstart && (argindex < maxargcount)) {
		args[argindex++] = cpt;
		needargstart = 0;
	    }
	    if (*cpt == STRINGQUOTE) {
		/*
		 * Advance cpt in current arg
		 */
		args[argindex - 1] = cpt + 1;
		quotedstring = 1;
	    }
	}
    }
    args[argindex] = '\0';
    return (argindex);
}
