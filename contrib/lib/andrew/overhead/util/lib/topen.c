/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: */
/* $ACIS: */
/* $Source: */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

#include <system.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <andrewos.h>		/* sys/time.h */
#include <setjmp.h>
extern int errno;
#define	tst(a,b)	(*mode == 'r'? (b) : (a))
#define	RDR	0
#define	WTR	1
static	int	*popen_pid = NULL;
static	int	dtablesize = -1;

#ifdef VMUNIX
#define	mask(s)	(1<<((s)-1))
#endif
#ifndef	SIGRETRO
#define	sigchild()
#endif /* SIGRETRO */

static int (*oldfunc)();

FILE *topen(name, argv, mode, pgrp)
char *name, *argv[], *mode;
int *pgrp;
{
    int p[2];
    register myside, hisside;

    if (popen_pid == NULL) {
	dtablesize = getdtablesize();
	if (dtablesize > 0) {
	    popen_pid = (int *) malloc(dtablesize * sizeof(int));
	}
    }
    if (popen_pid == NULL) {errno = ENOMEM; return NULL;}

    /* Set up to ignore broken pipe signals */
    oldfunc = (int (*)()) signal(SIGPIPE, SIG_IGN);

    if(pipe(p) < 0) return NULL;
    myside = tst(p[WTR], p[RDR]);
    hisside = tst(p[RDR], p[WTR]);
    if((*pgrp = osi_vfork()) == 0) {
	int pid, fd;
	/* myside and hisside reverse roles in child */
	sigchild();
	pid = getpid();
#if defined(hpux) || defined(SGI_4D_ENV)
	setpgrp();
#else /* hpux */
	setpgrp(pid, pid);
#endif /* hpux */
	close(myside);
	dup2(hisside, tst(0, 1));
	close(hisside);
	for (fd = getdtablesize(); fd > 2; --fd) close(fd);
	execv(name, argv);
	_exit(1);
    }
    if (*pgrp == -1) return NULL;
    /* I am parent */
    popen_pid[myside] = *pgrp;
    close(hisside);
    return fdopen(myside, mode);
}

static jmp_buf env;

static lclalarm()
{
    longjmp(env, 1);
}

int tclose(ptr, seconds, timedout)
FILE *ptr;
int seconds, *timedout;
{
    register f, r;
    int status, omask;
    extern int errno;
    struct itimerval val;
    int (*old)();

    f = fileno(ptr);
    fclose(ptr);
#ifdef VMUNIX
    omask = sigblock(mask(SIGINT)|mask(SIGQUIT)|mask(SIGHUP)|mask(SIGCHLD));
#endif

    if (timedout != 0) {
	/* Enable signal handler */
	old = (int (*)()) signal(SIGALRM, lclalarm);

	/* Set timer */
	val.it_interval.tv_sec = 0;
	val.it_interval.tv_usec = 0;
	val.it_value.tv_sec = seconds;
	val.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &val, 0);

	if (setjmp(env) == 0) {
	    while((r = wait(&status)) != popen_pid[f] && r != -1) ;

	    /* Turn off timer */
	    val.it_value.tv_sec = 0;
	    val.it_value.tv_usec = 0;
	    setitimer(ITIMER_REAL, &val, 0);
	    *timedout = 0;
	} else {
	    /* Timer expired */
	    status = -1;
	    *timedout = 1;
	}
	(void) signal(SIGALRM, old);	/* Restore old value */
    } else {
	while ((r=wait(&status)) != popen_pid[f] && r != -1) ;
    }

#ifdef VMUNIX
    sigsetmask(omask);
#endif

    /* Reenable old SIGPIPE action */
    (void) signal(SIGPIPE, oldfunc);

    return status;
}

FILE *qopen(name, argv, mode)
char *name, *argv[], *mode;
{
    int dummy;

    return topen(name, argv, mode, &dummy);
}

int qclose(ptr)
FILE *ptr;
{
    return tclose(ptr, 0, 0);
}

int getpidfromfp(ptr)
FILE *ptr;
{
    return(popen_pid[fileno(ptr)]);
}
