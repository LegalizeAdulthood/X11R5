/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* gtime.c - inverse of localtime */
/*   value is unpredictable for illegal input values */

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/gtime.c,v 1.3 1991/09/12 17:26:47 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/gtime.c,v $ */

#ifndef lint
char *gtime_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/gtime.c,v 1.3 1991/09/12 17:26:47 bobg Exp $";
#endif /* lint */

#include <andrewos.h> /* sys/time.h sys/types.h */

static int dmsize[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#define FEBRUARY_P(m) ((m)==1)
/* Gregorian definition: */
#define LEAPYEAR_P(y) (  ((y)%4==0) && ((((y)%100) != 0) || (((y)%400) == 0)))
/* Used only in a relative sense */
#define LEAPDAYS_SINCE(y) ( (y)/4 - (y)/100 + (y)/400 )

/* return inverse of local time */

#if defined(_POSIX_SOURCE) || defined(_IBMR2)
time_t gtime(ct)
register struct tm *ct;
{
    return (mktime(ct));
}
#else /* _POSIX_SOURCE */
time_t gtime(ct)
register struct tm *ct;
{
    time_t copyt;
    register int day;
    register int month;
    register int year;
    static int init = 0;

    /* this part is the inverse of gmtime */

    year = ct->tm_year + 1900;
    month = ct->tm_mon;
    day = ct->tm_mday - 1;
    if (month >= 12)
	month = 11;
    while (month-- > 0)
      day += ((FEBRUARY_P(month) && LEAPYEAR_P(year)) ? 29 : dmsize[month]);
    /* The following calculation is broken for the year 2100: */
    day += (year - 1970) * 365 + (((year - 1901) / 4) - 17);
    /* here's the fix:
    day += ( ((year)*356 + LEAPDAYS_SINCE(year))
           - (1970*356 + LEAPDAYS_SINCE(1970)) ); */
    copyt = ct->tm_sec + 60 * (ct->tm_min + 60 * (ct->tm_hour + 24 * (long)day));

    /* this inverts the dst adjustment in localtime */

    if (init == 0) {
	osi_SetZone();
	init = 1;
    }
    if (osi_IsEverDaylight && ct->tm_isdst)
	copyt -= 60*60;
    return (copyt + (time_t)osi_SecondsWest);
}
#endif /* _POSIX_SOURCE */

#ifdef TESTINGONLYTESTING
#include <stdio.h>
main()
{
  struct tm *ct;
  unsigned long int the_time, parsed_time;

  the_time = osi_GetSecs();
  ct = localtime(&the_time);
  parsed_time = (long int) gtime(ct);
  printf("The time is %ld, parsed time is %ld.\n", the_time, parsed_time);
  if (the_time != parsed_time) printf("And they are DIFFERENT!  (by a factor of %ld)\n", the_time - parsed_time);
}
#endif /* TESTINGONLYTESTING */
