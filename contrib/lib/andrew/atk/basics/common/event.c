/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/event.c,v 2.6 1991/09/12 15:58:17 bobg Exp $ */
/* $ACIS:event.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/event.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/event.c,v 2.6 1991/09/12 15:58:17 bobg Exp $";
#endif /* lint */

#include <class.h>
#include <event.eh>

#include <andrewos.h> /* sys/time.h */

static struct event *timerQueue = NULL;
static long currSec;			/* seconds since last StartTimer call */
static long tuBase;
static boolean timeInited = FALSE;
static long MSEC10 = event_MSECtoTU(10);

struct event *freeList;

boolean event__InitializeObject(classID, self)
    struct classheader *classID;
    struct event *self;
{
    self->t = event_ENDOFTIME;
    self->proc = NULL;
    self->procdata = NULL;
    self->next = NULL;
    return TRUE;
}

struct event *event__Allocate(classID)
    struct classheader *classID;
{
    register struct event *e;

    if (freeList == NULL)
	e = (struct event *) malloc(sizeof(struct event));
    else  {
	e = freeList;
	freeList = freeList->next;
    }
    return e;
}

void event__Deallocate(classID, self)
    struct classheader *classID;
    struct event *self;
{
    self->next = freeList;
    freeList = self;
}

void event__Cancel(self)
    struct event *self;
{
    register struct event *prev = NULL;
    register struct event *x;

    for (x = timerQueue; x != NULL && self != x; x=x->next) 
	prev=x;
    if (x == NULL) return;
    if (prev)
	prev->next = self->next;
    else
	timerQueue = self->next;
    event_Destroy(self);
}

struct event *event__Enqueue(classID, time, proc, procdata)
    struct classheader *classID;
    long time;
    int (*proc)();
    char *procdata;
{
    register struct event *e;

    e = event_New();
    e->t = time;
    e->proc = proc;
    e->procdata = procdata;
    
    /* enqueue in sequence by time of happening */

    if (timerQueue == NULL) {
	timerQueue = e;
    }
    else {
	register struct event *prev = NULL,
			*x = timerQueue;

	for (; x != NULL && time > x->t; prev=x, x=x->next) ;
	if (prev) {
	    e->next = prev->next;
	    prev->next=e;
	}
	else {
	    e->next = timerQueue;
	    timerQueue = e;
	}
    }
    return e;
}

void event__ForceNext(classID)
    struct classheader *classID;
{
    /* set time so next event will occur
    this routine changes the value that will be
    returned by the next call on now()  */

    if (timerQueue == NULL) return;
    tuBase = timerQueue->t;
}

long event__FirstTime(classID, currentTime)
    struct classheader *classID;
    long currentTime;
{
    /* returns the time remaining to first event on queue */

    return ((timerQueue == NULL) ? event_ENDOFTIME : timerQueue->t - currentTime);
}

void event__StartTimer(classID)
    struct classheader *classID;
{
    /* start timer for elapsed time
    units are   microseconds >>6  (max of 64000 sec) */

    struct osi_Times tp;
    register struct event *e = timerQueue;

    osi_GetTimes(&tp);
    if (timerQueue) 
	if (timeInited) {
	    /* reduce every time by 'now' */

	    register long oldNow, deltaSec;

	    deltaSec = tp.Secs - currSec;
	    currSec += deltaSec;
	    tuBase += event_SECtoTU(deltaSec);
	    oldNow = tuBase + event_USECtoTU(tp.USecs);
	    for (; e; e=e->next)
		e->t -= oldNow;
	} else {
	    /* queue is bogus, clear all times */

	    for ( ; e; e = e->next)
		e->t = 0;
	}
    currSec = tp.Secs;
    tuBase = - event_USECtoTU(tp.USecs);
    timeInited = TRUE;
}

long event__HandleTimer(classID, currentTime)
    struct classheader *classID;
    long currentTime;
{
    /* there are elements on timer queue.  process first if it is
    time (or if it will be time within 10 msec).
    return time to wait before next event  */

    register long twait;
    register struct event *e = timerQueue;

    if (timerQueue == NULL)
	return event_ENDOFTIME;

    twait = e->t - currentTime;		/* time to wait for next event */
    if (twait > MSEC10)
	return (twait);

    /* handle first event on queue */

    timerQueue = e->next;
    (*e->proc)(e->procdata, currentTime);
    event_Destroy(e);

    if (timerQueue == NULL) return event_ENDOFTIME;
    twait = timerQueue->t - currentTime - MSEC10;
    return (twait>0 ? twait : 0);
}

long event__Now(classID)
    struct classheader *classID;
{
    /* returns time relative to last StartTimer 
    units are   microseconds >>6  (max of 64000 sec) 
    This routine returns timeintervals good for about a day;
    for longer execution times (as in a server) there
    ought to be an ocassional call on StartTimer().
    If times are only used for EnqueueEvent, StartTimer
    can be called at any time, the queue will be updated. */

    struct osi_Times tp;
    register long deltaSec;

    if (! timeInited)
	event_StartTimer(); 
    osi_GetTimes(&tp);
    if ((deltaSec=tp.Secs-currSec) )  {
	/* second hand has skipped to next second
	   record new second hand position and update 
	   the current time value. */

	currSec += deltaSec;
	if (deltaSec>1) 
	    tuBase += event_SECtoTU(deltaSec);
	else tuBase += (1000000>>6);
    }
    return (tuBase + event_USECtoTU(tp.USecs));

}

