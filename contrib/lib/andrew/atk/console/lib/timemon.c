/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/timemon.c,v 2.8 1991/09/12 16:07:28 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/timemon.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/timemon.c,v 2.8 1991/09/12 16:07:28 bobg Exp $";
#endif /* lint */

#include <class.h>
#include <conclass.ih>
#include <console.h>
#include <environ.ih>
#include <envrment.ih>
#include <ctype.h>
#include <andrewos.h> /* sys/time.h sys/file.h */
#include <sitevars.h>

extern boolean WasUDPAction;

InitClock() {
    mydbg(("entering: InitClocks\n"));
    Numbers[CLOCKSECONDS].RawText = (char *)malloc(20);
    Numbers[CLOCKMINUTES].RawText = (char *)malloc(10);
    Numbers[CLOCKALL].RawText = (char *)malloc(40);
    Numbers[DATE].RawText = (char *)malloc(30);
}

CheckClock(self)
    struct consoleClass *self;
 {
    long    now;
    struct tm  *currentTime;
    boolean IsPM = FALSE;
    int     hours;
    static char *DayOfWeek[] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", 
	"Thursday", "Friday", "Saturday", "Sunday", 0};
    static int OldHours = 0,
	OldMinutes = 0,
	OldDate = 0,
    TFHC = -1;/* Twenty-Four Hour Clock */
    static long LastUDPAction = 0;

    mydbg(("entering: CheckClock\n"));
    now = time(0);
    if (Numbers[UDPIDLE].IsDisplaying) {
	if (WasUDPAction) {
	    LastUDPAction = now;
	    WasUDPAction = FALSE;
	}
	NewValue(self, &Numbers[UDPIDLE], now - LastUDPAction, NULL, FALSE);
    }
    if (TFHC == -1){
	TFHC = environ_GetProfileSwitch("console.tfhc", FALSE);
    }
    currentTime = localtime(&now);
    hours = currentTime->tm_hour;
    if (!TFHC && hours >= 12) {
        IsPM = TRUE;
	hours -= 12;
    }
    if (!TFHC && hours == 0) {
	hours = 12;
    }
    if (Numbers[CLOCKSECONDS].IsDisplaying) {
	OldMinutes = -1; /* Hack to avoid overwriting min hand */
	OldHours = -1; /* Hack to avoid overwriting hour hand */
	if (!TFHC){
	    sprintf(Numbers[CLOCKSECONDS].RawText, "%2d:%02d:%02d %cM",
	    hours, currentTime->tm_min, currentTime->tm_sec, IsPM ? 'P' : 'A');
	}
	else{
	    sprintf(Numbers[CLOCKSECONDS].RawText, "%02d:%02d:%02d",
	    hours, currentTime->tm_min, currentTime->tm_sec);
	}
	NewValue(self, &Numbers[CLOCKSECONDS], currentTime->tm_sec, NULL, TRUE);
    }
    if (Numbers[CLOCKMINUTES].IsDisplaying && OldMinutes != currentTime->tm_min) {
	OldMinutes = currentTime->tm_min;
	OldHours = -1; /* Hack to avoid overwriting hour hand */
	if (!TFHC){
	    sprintf(Numbers[CLOCKMINUTES].RawText, "%2d:%02d %cM",
	    hours, currentTime->tm_min, IsPM ? 'P' : 'A');
	}
	else{
	    sprintf(Numbers[CLOCKMINUTES].RawText, "%02d:%02d",
	    hours, currentTime->tm_min);
	}
	NewValue(self, &Numbers[CLOCKMINUTES], currentTime->tm_min, NULL, TRUE);
    }
    if (Numbers[CLOCKHOURS].IsDisplaying && OldHours != hours) {
	OldHours = hours;
	NewValue(self, &Numbers[CLOCKHOURS], hours, NULL, TRUE);
    }
    if (Numbers[CLOCKHOURFIFTHS].IsDisplaying) {
        NewValue(self, &Numbers[CLOCKHOURFIFTHS], (hours >= 12 ? hours -12 : hours) * 5+ currentTime->tm_min/12, NULL, TRUE);
    }
    if (!TFHC){
	sprintf(Numbers[CLOCKALL].RawText, "%2d:%02d:%02d %cM, %s, %d/%d/%d",
		hours, currentTime->tm_min, currentTime->tm_sec,
		IsPM ? 'P' : 'A',
		DayOfWeek[currentTime->tm_wday], currentTime->tm_mon + 1, 
		currentTime->tm_mday, currentTime->tm_year);
    }
    else{
	sprintf(Numbers[CLOCKALL].RawText, "%02d:%02d:%02d, %s, %d/%d/%d",
		hours, currentTime->tm_min, currentTime->tm_sec,
		DayOfWeek[currentTime->tm_wday], currentTime->tm_mon + 1, 
		currentTime->tm_mday, currentTime->tm_year);
    }
	NewValue(self, &Numbers[CLOCKALL], currentTime->tm_yday *86400 + currentTime->tm_min *60 + currentTime->tm_hour *3600 + currentTime->tm_sec, NULL, TRUE);
    if (Numbers[DATE].IsDisplaying && currentTime->tm_yday != OldDate) {
	OldDate = currentTime->tm_yday;
	sprintf(Numbers[DATE].RawText, "%s %d/%d/%d",
		DayOfWeek[currentTime->tm_wday], currentTime->tm_mon + 1, currentTime->tm_mday, currentTime->tm_year);
	NewValue(self, &Numbers[DATE], 0, NULL, TRUE);
    }
}

