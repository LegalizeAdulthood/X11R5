/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/notopf.c,v 2.7 1991/09/12 16:07:06 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/notopf.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/notopf.c,v 2.7 1991/09/12 16:07:06 bobg Exp $";
#endif /* lint */

/* 
 * notop.c -- Routines for instrument console that are
 * NOT shared by the operator's console.
 */

#include <class.h>
#include <conclass.ih>
#include <im.ih>
#include <event.ih>
#include <console.h>
#include <andrewos.h> /* sys/time.h */
#include <errno.h>
#include <signal.h>


WakeUp(self)
    struct consoleClass *self;
{
    mydbg(("entering: WakeUp\n"));
    if (!PauseEnqueuedEvents){
	if (!RingingAlarm){
	    if (DoVMStat && ++VMPollCt > VMPollFreq) {
		VMPollCt = 1;
/*		ComputeStatistics(self);*//*handled directly by im_AddFileHandler in vmmon.c:InitStatistics ? */
	    }
	    if (DoDiskFreeStat && ++DiskPollCt > DiskPollFreq) {
		DiskPollCt = 1;
/*		FindFreeDiskSpace(self);*//*handled directly by im_AddFileHandler in vmmon.c:InitStatistics ? */
	    }
	    if (DoVenusChecking && ++VenusPollCt > VenusPollFreq) {
		VenusPollCt = 1;
		CheckVenusQuota(self);
	    }
	    if (DoMailChecking && ++MailPollCt > MailPollFreq) {
		MailPollCt = 1;
		CheckMail(self, FALSE);
	    }
	    if (DoPrintChecking && ++PrintPollCt > PrintPollFreq) {
		PrintPollCt = 1;
		CheckPrint(self);	/* In mailmon.c, for no good reason */
	    }
	    if (DoDirChecking && ++DirPollCt > DirPollFreq) {
		DirPollCt = 1;
		CheckDirectories(self);	/* In mailmon.c */
	    }
	    if (DoCheckClock && ++ClockPollCt > ClockPollFreq) {
		ClockPollCt = 1;
		CheckClock(self);
	    }
	    if (++FPAPollCt > FPAPollFreq){
		FPAPollCt = 1;
		CheckFPA(self);
	    }
	}
	else
	    NewValue(self, &Numbers[CLOCKALL], Numbers[CLOCKALL].Value, NULL, TRUE);
	/* That last line is a hack so that the * alarm flashes as often as
	 possible */
	if (DoTroubleChecking) {
	    CheckTrouble(self);
	}
    }
    im_EnqueueEvent(WakeUp, self, event_SECtoTU(Period));
}

