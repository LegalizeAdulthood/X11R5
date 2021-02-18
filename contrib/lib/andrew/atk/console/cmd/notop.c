/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/cmd/RCS/notop.c,v 2.11 1991/09/12 16:05:53 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/cmd/RCS/notop.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/cmd/RCS/notop.c,v 2.11 1991/09/12 16:05:53 bobg Exp $";
#endif /* lint */

/* 
 ****************************************************************
 * notop.c -- Routines for instrument console that are NOT
 * shared by the operator's console.
 ****************************************************************
 */

#include <class.h>
#include <conclass.ih>
#include <console.h>
#include <andrewos.h> /* sys/time.h */
#include <errno.h>
#include <signal.h>

/* the following char* values get used in:
  1.) ../lib/input.c
  2.) ../lib/setup.c
*/
char *RealProgramName = "Console";
char EXTENSION[] = "con";
char EXTENSION1[] = "console";
char EXTENSION2[] = "Console";



ChooseColumns(numcol)
    int numcol;
{
    mydbg(("entering: ChooseColumns\n"));
}

ChooseMachines(self, machinelist)
struct consoleClass *self;
    char *machinelist;
{
    mydbg(("entering: ChooseMachines\n"));
}

ConfigureMachines(self, Rows, Columns, Machines, Initialize)
struct consoleClass *self;
    int *Rows, *Columns, *Machines;
    boolean Initialize;
{
    mydbg(("entering: ConfigureMachines\n"));
    *Rows = *Columns = *Machines = 1;
}

struct datum *BuildDatum(keyword, machine)
    char *keyword;
    int machine;
{
    mydbg(("entering: BuildDatum\n"));
    return(&Numbers[ALWAYS]);
}

OneTimeRemoteInit(self)
    struct consoleClass *self;
{
    mydbg(("entering: OneTimeRemoteInit\n"));
    /* Does nothing if not operator console */
}

InitializeGetStats(self)
    struct consoleClass *self;
{
    static boolean DidInitDisks = FALSE;
    if ((DoVMStat && ! DidInitGVM) || (DoDiskFreeStat && !DidInitDisks)){
	InitStats(self);
	DidInitGVM = TRUE;
	DidInitDisks=TRUE;
    }

}

InitializeInstruments(self)
    struct consoleClass *self;
{
    static boolean DidInitMail = FALSE, DidInitDirs = FALSE, DidInitPrint = FALSE;

    mydbg(("entering: InitializeInstruments\n"));
    if (DoVenusMonitoring && ! DidInitVenus) {
	InitializeMariner(self);
	DidInitVenus = TRUE;
    }
    if (! DidInitVenus) {
/*	VenusIn = winin; */
	VenusIn = NULL; /* ??? */
    }
    if (DoMailChecking && !DidInitMail) {
	InitMail(self);
	DidInitMail = TRUE;
    }
    if (DoPrintChecking && !DidInitPrint) {
	InitPrint(self);
	DidInitPrint = TRUE;
    }
    if (DoDirChecking && !DidInitDirs) {
	InitDirectories(); /* In mailmon.c */
	DidInitDirs = TRUE;
    }
}

struct RegionLog *WhichErrorLog(i)
    int i;
{
    mydbg(("entering: WhichErrorLog\n"));
    return &RegionLogs[ERRORREGIONLOG];
}

ReInitializeRemoteInstruments() {}
