/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/vmmon.c,v 2.11 1991/09/12 16:07:48 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/vmmon.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/vmmon.c,v 2.11 1991/09/12 16:07:48 bobg Exp $";
#endif /* lint */

#include <class.h>
#include <andrewos.h>
#include <im.ih>
#include <conclass.ih>
#include <console.h>

#include <fcntl.h>
#include <signal.h>


extern ComputeStatistics();
extern int children[]; /* Make getstats a bona-fide controlled child */
  /*
  * NOTE:  All references to the array of children are copied from code
  * in setup.c:SetupFromConsoleFile case FLAG_INITEXEC.
  * If the child management code changes, change this here too.
  * 12/29/88 wdc@athena.mit.edu
  */
 

/*
  * Call a small - secure setuid root/setgid kmem program to collect the stats
  * from /dev/kmem - and have them sent back through a pipe and
  * processed in ComputeStatistics - in vmmonf.c
  */

getkmemexit(pid, self)
int pid;
struct consoleClass *self;
{
    mydbg(("entering: getkmemexit\n"));
    /* no-op -- do I really need this?  Can I just use NULL for the function pointer
	in the im_AddZombieHandler call? */
}    

InitStats(self)
    struct consoleClass *self;
{
    /* pfds = Pipe File DescriptorS */
    int pfds[2];
    /* added the following counter variable for bounds checking the array --MKM */
    int i; 
    FILE *vmstats;
    int flags;
    char *myargv[5], userid[50], freq1[50], freq2[50];
    int mask, *newchild;

    mydbg(("entering: InitStats\n"));
    pipe(pfds);

    mask = sigblock(1 << (SIGCHLD - 1));
    /* loop up until the end of children, the last element is a -1
        added a bounds check here  --MKM */
    i = 0;
    for (newchild = children; *newchild > 0 && i < MAXCHILDPIDS; newchild++)
	i++;
/* why is the first element always set to a -1? This looks likea kludge to me.--MKM*/
/*    if (*newchild == -1){
	newchild[1] = -1;
    } */
    /* if we're forking getstats */
    if ((*newchild = osi_vfork()) == 0) {
	/* since the pid is 0, then all calls apply to this process group */
                theGetstatsPid = getpid();
	setpgrp(0, theGetstatsPid);
	/* 1 == stdout */
	if (dup2 (pfds[1], 1) == -1){ 
	    perror("console: dup2 failed while forking getstats\n");
	    _exit(-1);
	}
	if (close (pfds[0]) == -1){
	    perror("console:<child> close of pfds[0] failed\n");
	}
	if (close (pfds[1]) == -1){
	    perror(self, "console: close of pfds[1] failed\n");
	}
	myargv[0] = "getstats";
	sprintf(userid, "%d", getuid());
	myargv[1] = userid;
	sprintf(freq1, "%d", VMPollFreq);
	sprintf(freq2, "%d", DiskPollFreq);
	myargv[2] = freq1;
	myargv[3] = freq2;
	myargv[4] = NULL;
	execvp("getstats", myargv);
	/* it's an error if execvp returns */
	perror("execvp failed");
	_exit(-1);
    } else{
	/* set the child mask; assume that console stats are not forked */
	if (*newchild == -1){
	    *newchild = 0;
	    sigsetmask(mask);
	    ReportInternalError(self, "ERROR: console: fork failed - no Disk or GVMStats will be monitored");
	    return(0);
	}
    }
    sigsetmask(mask);
    /* im_AddZombieHandler(pid, getkmemexit, self); */

    if (close (pfds[1]) == -1){
	ReportInternalError(self, "ERROR: console:<parent> close of pfds[1] failed - no Disk or GVMStats will be monitored");
    }
    vmstats = fdopen(pfds[0], "r");
    if (!vmstats){
	ReportInternalError(self, "ERROR: console: fdopen of pfds[0] failed - no Disk or GVMStats will be monitored");
	return(0);
    }
    flags = fcntl(fileno(vmstats), F_GETFL, 0);
    fcntl(fileno(vmstats), F_SETFL, flags | FNDELAY);
    im_AddFileHandler(vmstats, ComputeStatistics, self, 1);
}

	    
	    

