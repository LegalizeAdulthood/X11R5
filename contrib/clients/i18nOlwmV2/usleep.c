/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) usleep.c 50.1 90/12/12 Crucible";

/*
 * usleep() compatibility function
 *
 * It's not very robust, but it seems to work fine for what olwm
 * needs.
 */

#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>

void olwm_do_nothing();

int
olwm_usleep(usec)
	unsigned usec;
{
	struct itimerval	new, old;
	struct sigvec		new_vec, old_vec;

	new.it_interval.tv_sec = new.it_interval.tv_usec = 0;
	new.it_value.tv_sec = 0;
	new.it_value.tv_usec = usec;

	new_vec.sv_handler = olwm_do_nothing;
	new_vec.sv_mask= 0;
	new_vec.sv_flags = 0;

	sigvec(SIGALRM, &new_vec, &old_vec);
	setitimer(ITIMER_REAL, &new, &old);

	/* wait until we receive a signal */
	sigpause(0);

	/* restore previous values for SIGALRM handler and the real itimer */
	sigvec(SIGALRM, &old_vec, (struct sigvec *)0);
	setitimer(ITIMER_REAL, &old, (struct itimerval *)0);
	return(0);
}

void
olwm_do_nothing()
{
}
