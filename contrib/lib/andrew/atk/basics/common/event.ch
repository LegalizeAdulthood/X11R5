/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/event.ch,v 2.6 1991/09/12 19:21:44 bobg Exp $ */
/* $ACIS:event.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/event.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidevent_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/event.ch,v 2.6 1991/09/12 19:21:44 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#define event_VERSION 1

class event  {
methods:
    Cancel();
classprocedures:
    Enqueue(long time, procedure proc, char *procdata) returns struct event *;
    ForceNext();
    FirstTime(long currentTime) returns long;
    StartTimer();
    HandleTimer(long currentTime) returns long;
    Now() returns long;
    Allocate() returns struct event *;
    Deallocate(struct event *self);
data:
    long t;		/* time of evenet in microsec >> 6 */
    int (*proc)();	/* procedure to call for event */
    char *procdata;	/* data to be passed to proc */
    struct event *next;
};

#define event_TUtoSEC(x)  ((x)/(1000000>>6))
#define event_TUtoUSEC(x) ((x)<<6)
#define event_TUtoMSEC(x) (((x)<<3)/(1000>>3))
#define event_SECtoTU(x)  ((x)*(1000000>>6))
#define event_USECtoTU(x) ((x)>>6)
#define event_MSECtoTU(x) (((x)*(1000>>3))>>3)
#define event_ENDOFTIME 2000000000

