/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/updateq.ch,v 2.7 1991/09/30 18:26:45 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/updateq.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_updateq_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/updateq.ch,v 2.7 1991/09/30 18:26:45 susan Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

struct fp_queue {
  procedure fp;
  struct fp_queue * next;
};

class updateq : dataobject[dataobj] {
methods:
 SetView( struct view * view );
 EnqueueUpdateFunction( procedure fp );
 ExecuteUpdateQueue();
 ClearUpdateQueue();
data:
 struct view * view;
 struct fp_queue * queue;
};

