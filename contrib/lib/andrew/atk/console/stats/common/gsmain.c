/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *gsmain_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/stats/common/RCS/gsmain.c,v 1.2 1991/09/12 16:08:10 bobg Exp $";

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/stats/common/RCS/gsmain.c,v 1.2 1991/09/12 16:08:10 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/stats/common/RCS/gsmain.c,v $ */

#ifndef lint
static char *getstats_c_id = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/stats/common/RCS/gsmain.c,v 1.2 1991/09/12 16:08:10 bobg Exp $";
#endif /* lint */

/* **********************************************************************
*   This code is designed to read what might be priveledged (setuid) 
*   information regarding both Disk Statistics (% full) and a host of 
*   stats from /dev/kmem (including but not limited to, CPU, I/O, and VM)
*
*   When retriving the data - this program will print out to stdout
*   a string in the form of either "%d:%d\n" or "%d:%d:%s\n"
*   The latter case is for passing the name of where a disk is mounted
*   back to the parent program.
*
*   The parent program (Console, or any other program which wishes to get
*   at this information) is responsible for setting up a pipe, binding the
*   child's (this program) stdout to one end of a pipe, and parsing the
*   strings which are passed back.
*
*   The basic string format is an ID (int), a colon, a value (int), and
*   optionally another colon followed by a string.  The ID is coded from
*   the included file "getstats.h" - ID values 50 and over represent 
*   ERRORS as documented in the above mentioned inclued file.  When an 
*   ERROR or the optional string is passed, the value (second parameter)
*   can be safely ignored, and is usually set to 0.
*
*   The arguments to be passed to this program are the effective UID from
*   the parent program, a polling frequency (# of seconds) for checking
*   /dev/kmem (usually between 1 and 5, must be > 0), and a polling
*   frequency for checking how full the local disks are (generally higher
*   than the value for /dev/kmem, but could vary greatly).  Thus the call
*   is:
*
*   execvp("getstats", argv)
*
*   with argv as:
*
*   argv[0]="getstats";
*   argv[1]=~ ("%d", UID);
*   argv[2]=~ ("%d", kmempollfrequency);
*   argv[3]=~ ("%d", diskpollfrequency);
*   argv[4]=NULL;
*
********************************************************************** */
#include <sitevars.h>

/* The following include defines sys_vax_20 for Vax release 2.0 op sys, */
/* since that particular OS needs some special treatment.  We would have */
/* used -Dsys_vax_20 except we want to use the same Makefile for each cpu... */

#include <system.h>
#include <stdio.h>
#include <getstats.h>

#define sendval(text) {printf text ;fflush(stdout);}

extern GetGVMStats(); /* */
extern InitGVMStats(); /* */
extern GetDiskStats(); /* */

#define min(a, b) (((a) <= (b)) ? (a) : (b))


main(argc, argv)
int argc;
char **argv;
{
    int GVMPollFreq = 0, DiskPollFreq = 0, UsersID = 0;
    int time1 = 0, time2 = 0, sleepTime = 0;

    if (argc < 4){
	sendval(("%d:%d\n", PARSE_ERR_1, 0));
	exit(-1);
    }
    *argv++;
    UsersID = atoi(*argv++);
    if ((GVMPollFreq = atoi(*argv++)) < 1){
	sendval(("%d:%d\n", PARSE_ERR_2, 0));
	exit(-1);
    }
    if ((DiskPollFreq = atoi(*argv)) < 1){
	sendval(("%d:%d\n", PARSE_ERR_2, 0));
	exit(-1);
    }
    InitGVMStats();
    GetDiskStats(1);
    while (1){	/* for lack of a better loop for now? */
	if (time1 < 1){
	    GetGVMStats(UsersID);
	    time1 = GVMPollFreq;
	}
	if (time2 < 1){
	    GetDiskStats(0);
	    time2 = DiskPollFreq;
	}
	sleepTime = min(time1, time2);
	sleep(sleepTime);
	time1 -= sleepTime;
	time2 -= sleepTime;
    }
}

