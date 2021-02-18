/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *getstats_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/stats/apollo68k/RCS/getstats.c,v 1.2 1991/09/12 16:08:04 bobg Exp $";

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/stats/apollo68k/RCS/getstats.c,v 1.2 1991/09/12 16:08:04 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/stats/apollo68k/RCS/getstats.c,v $ */

#ifndef lint
static char *getstats_c_id = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/stats/apollo68k/RCS/getstats.c,v 1.2 1991/09/12 16:08:04 bobg Exp $";
#endif /* lint */

/* **********************************************************************
*   ****************
*   STUB LIBRARY FILE---NOT YET IMPLEMENTED FOR THIS SYSTEM
*   THIS CODE SHOULD BE REPLACED BY AN IMPLEMENTATION OF
*   THE RELEVANT ROUTINES FOR YOUR MACHINE TYPE.
*   Please contribute your implementations to info-andrew-bugs@andrew.cmu.edu.
*   You may use any of the other ../<several>/getstats.c files as templates.
*   ****************
*
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

#include <system.h>

GetGVMStats(UsersID)
int UsersID;
{
}

InitGVMStats()
{
}

GetDiskStats(Init)
int Init;
{
}
