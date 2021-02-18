/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *snpfkscb_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/snap2/loadserv/RCS/snpfkscb.c,v 2.4 1991/09/12 17:19:55 bobg Exp $";

/*
   a fake version of the ibmpc scb module
*/
#include <scb.h>

/*
  On a pc set this to false right after calling SNAP_ClientInit()
  to inhibid any SNAP_BeginConv from making sessions that login can see.
  If SNAP_login is set to false, do not call SNAP_ClientTerm as it will
  close all (read) cids for all the sessions login has.

  Clearly, this needs to be reorganized some day.
*/
int SNAP_login;

/*
  On a pc this maps a fake cid made up by snapstubs
  (which is really a scb id) into a real snap cid.
  Call this routine on a (fake)cid before calling
  SCB_ClearSession
*/
int SCB_GetCID(acid)
{
    return acid;
}

/*
  on a pc ending a connection still leaves a cid around
  for login to see.  SCB_ClearSession is called after
  SNAP_EndConv() and flushes the session.
  DANGER: The cid passed here is a *real* cid, not the
 fake one retruned by the SNAP_BeginConv glue.
*/
void SCB_ClearSession (cid)
int cid;
{
    return;
}

SNAP_Nop()
{
    return 0;
}

SNAP_GetAnchor(a)
int a;
{
    return 1;
}

SCB_GetLastCid(aname)
{
    return NULLCID;
}
