/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/controlv.ch,v 1.1 1991/09/30 18:16:13 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/controlv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_controlV_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/controlv.ch,v 1.1 1991/09/30 18:16:13 susan Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


class controlV[controlv] : buttonV[buttonv]{
    overrides:
      DoHit(enum view_MouseAction type, long x,y,hits) returns struct controlV *;
      LookupParameters();
      DrawFromScratch(long x,y,width,height);
    classprocedures:
      InitializeClass() returns boolean;
      InhibitAutoInit();
      SetAutoInit(boolean val);
    data:
      char *cclass;
      char *function;
      boolean autoinit,needsinit;
};

