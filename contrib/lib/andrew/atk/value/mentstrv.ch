/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/mentstrv.ch,v 2.8 1991/09/30 18:26:45 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/mentstrv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_mentertextstrV_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/mentstrv.ch,v 2.8 1991/09/30 18:26:45 susan Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */
class menterstrV[mentstrv] : buttonV[buttonv] {
overrides:
   DoHit(enum view_MouseAction type, long x,y,hits) returns struct buttonV *;
   ObservedChanged( struct observable * observed, long status );
   GetApplicationLayer() returns struct view *;
   WantInputFocus(struct view *requestor);
classprocedures:
   InitializeObject(struct menterstrV *self) returns boolean;
   FinalizeObject(struct menterstrV *self) returns boolean;
data:
    struct mentertext *etext;
    struct metextview *etextview;
};

