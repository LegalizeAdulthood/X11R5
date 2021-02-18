/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/update.c,v 2.12 1991/09/12 15:49:37 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/update.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/update.c,v 2.12 1991/09/12 15:49:37 bobg Exp $ ";
#endif /* lint */

#include <andyenv.h>
#include <ms.h>
#include <mailconf.h>
#if defined(hpux) && HP_OS < 70
#include <sys/time.h>
#include <signal.h>
#else /* hpux */
#include <sys/wait.h>
#endif /* hpux */

#ifdef WHITEPAGES_ENV
#define CLOSE_WP_DELTA (10*60)	/* close WP every 10 minutes */
#endif /* WHITEPAGES_ENV */

extern int SubscriptionsAreDirty;
int NeedToTimeOut = 0;
static int MS_ShouldCleanZombies = 1;

/* This is a FAKE routine -- only used in non-snap version, and faked in cuisnap.c */

MS_SetCleanupZombies(value)
int value;
{
    MS_ShouldCleanZombies = value;
}

MS_Die() {
    MS_UpdateState();
    safeexit(0);
}

MS_FastUpdateState() {
    return(UpdateState(FALSE));
}

MS_UpdateState() {
    return(UpdateState(TRUE));
}

VitalCheckpoints(DoEverything, errct)
Boolean DoEverything;
int *errct;
{
    long retcode = 0;

    if (CloseNeedyDirs(DoEverything)) {
	retcode = mserrcode;
	++*errct;
    }
    if (SubscriptionsAreDirty && WriteSubs()) {
	retcode = mserrcode;
	++*errct;
    }
    return(retcode);
}
    
#if defined(hpux) && HP_OS < 70
setup_sigAlrm()
{ }
#endif /* hpux */


UpdateState(DoEverything)
Boolean DoEverything;
{
#ifndef hpux
    union wait status;
#else	/* hpux */
#if HP_OS >= 70
#define WNOHANG 1
    int *status;
#endif /* HP_OS >= 70 */
#endif /*  not hpux */

    int pid, retcode = 0, errct = 0;
#ifdef WHITEPAGES_ENV
    static long time_when_wp_last_closed = 0;
    long time_now;
#endif /* WHITEPAGES_ENV */

#ifdef WHITEPAGES_ENV
    time_now = time(0);
    if (time_when_wp_last_closed == 0) time_when_wp_last_closed = time_now;
#endif /* WHITEPAGES_ENV */
    retcode = VitalCheckpoints(DoEverything, &errct);
    if (PrintPendingRequests(DoEverything)) {
	retcode = mserrcode;
	++errct;
    }

    if (DoEverything) {
#ifdef WHITEPAGES_ENV
	time_when_wp_last_closed = time_now;
	CloseWhitePages();
#endif /* WHITEPAGES_ENV */
	getla_ShutDown();
	dropoff_Reset();
    } else {			/* if not DoEverything */
#ifdef WHITEPAGES_ENV
				/* on fast updates, close WP every
				  CLOSE_WP_DELTA seconds */
      if ((time_now - time_when_wp_last_closed) > CLOSE_WP_DELTA) {
	time_when_wp_last_closed = time_now;
	CloseWhitePages();
      }
#endif /* WHITEPAGES_ENV */
    }

    if (MS_ShouldCleanZombies) {
#if defined(hpux) && HP_OS < 70
	int status;
	struct sigvec vecAlrm;
	struct itimerval timer;

	/** enable an interval timer so we can escape from wait(); **/
	vecAlrm.sv_handler = setup_sigAlrm;
	vecAlrm.sv_mask = 0;
	vecAlrm.sv_flags = 0;
	sigvector(SIGALRM, &vecAlrm, 0);
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 100000;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 100000;
	setitimer(ITIMER_REAL, &timer, 0);

	while ((pid = wait(&status)) > 0) ;

	/** disable the timer **/
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &timer, 0);
#else /* any system except HP_OS < 70 */
	do { /* Have you checked the children? */
#ifdef hpux /* HP_OS >= 70 */
	    pid = (int) wait3(status, WNOHANG, NULL);
#else	/* non hpux system */
	    pid = wait3(&status, WNOHANG, NULL);
#endif /* hpux */
	} while (pid > 0);
#endif /* hp_os < 70 */
    }
    NeedToTimeOut = 0;
    if (errct > 1) {
	char ErrorText[256];
	sprintf(ErrorText, "There were %d different errors in updating messagserver state!", errct);
	NonfatalBizarreError(ErrorText);
    }

    return(retcode);
}
