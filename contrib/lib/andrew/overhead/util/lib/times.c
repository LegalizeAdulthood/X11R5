/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: */
/* $Source: */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

#include <errno.h>
#include <andrewos.h>
#if (!defined(_IBMR2) && !SY_AIXx && !defined(HP300_ENV) && !defined(HP800_ENV) && !defined(M_UNIX) && (!defined(SGI_4D_ENV)) && !defined(SUN_ENV))
extern char *timezone();
#endif /* (!defined(_IBMR2) && !SY_AIXx && !defined(HP300_ENV) && !defined(HP800_ENV)) */

extern int errno;

/* OS independence for times. */
#if SY_AIXx
#define HAS_GETTIMEOFDAY 1
#endif /* #if SY_AIXx */

#ifdef HP300_ENV
#define HAS_GETTIMEOFDAY 1
#endif /* #ifdef HP300_ENV */

#ifdef HP800_ENV
#define HAS_GETTIMEOFDAY 1
#endif /* #ifdef HP800_ENV */

#if SY_B4x
#define HAS_GETTIMEOFDAY 1
#endif /* #if SY_B4x */

int osi_GetTimes(blk)
struct osi_Times *blk;
{
#ifdef HAS_GETTIMEOFDAY
    struct timeval TV;
    int Ret;

    Ret = gettimeofday(&TV, 0);
    blk->Secs = (unsigned long int) TV.tv_sec;
    blk->USecs = (unsigned long int) TV.tv_usec;
    return (Ret);
#else /* #ifdef HAS_GETTIMEOFDAY */
    blk->Secs = (unsigned long int) time(0);
    blk->USecs = 0;
    return (0);
#endif /* #ifdef HAS_GETTIMEOFDAY */
}

#if (SY_AIXx == 0 && SY_U5x == 0)
static int ZonesInitialized = 0;
char *osi_ZoneNames[2];
long int osi_SecondsWest;
int osi_IsEverDaylight;

void osi_SetZone()
{
    struct timeval TV;
    struct timezone TZ;
    static char TZ0[20], TZ1[20];
    if (ZonesInitialized == 0) {
	gettimeofday(&TV, &TZ);
	osi_SecondsWest = (long) TZ.tz_minuteswest * 60;
	osi_IsEverDaylight = TZ.tz_dsttime;
	strncpy(TZ0, timezone(TZ.tz_minuteswest, 0), sizeof(TZ0) - 1);
	TZ0[sizeof(TZ0) - 1] = '\0';
	osi_ZoneNames[0] = TZ0;
	strncpy(TZ1, timezone(TZ.tz_minuteswest, 1), sizeof(TZ1) - 1);
	TZ1[sizeof(TZ1) - 1] = '\0';
	osi_ZoneNames[1] = TZ1;
	ZonesInitialized = 1;
    }
}
#endif /* #if (SY_AIXx == 0 && SY_U5x == 0) */

#ifdef TESTINGONLYTESTING
#include <stdio.h>
main() {
    struct osi_Times TMs;
    printf("It's now %ld o'clock; do you know where your children are?\n", osi_GetSecs());
    osi_SetZone();
    printf("Not only that, but this system is %ld seconds (%ld minutes, %ld hours) west of GMT, and %suses Daylight time.\n", osi_SecondsWest, osi_SecondsWest / 60, osi_SecondsWest / (60*60), (osi_IsEverDaylight ? "" : "never "));
    printf("The standard zone name is ``%s''; the daylight zone name is ``%s''.\n", osi_ZoneNames[0], osi_ZoneNames[1]);
    if (osi_GetTimes(&TMs)) {
	printf("osi_GetTimes failed; errno %d\n", errno);
    } else {
	printf("osi_GetTimes returned {%ld, %ld}.\n", TMs.Secs, TMs.USecs);
    }
    exit(0);
}
#endif /* #ifdef TESTINGONLYTESTING */
