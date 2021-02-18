/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/entrstrv.ch,v 2.8 1991/09/30 18:26:45 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/entrstrv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_enterstrV_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/entrstrv.ch,v 2.8 1991/09/30 18:26:45 susan Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */
class enterstrV[entrstrv] : buttonV[buttonv] {
overrides:
   DoHit(enum view_MouseAction type, long x,y,hits) returns struct buttonV *;
   ObservedChanged( struct observable * observed, long status );
   LinkTree(struct view *parent);
  DrawButtonText(char *text,long len,struct rectangle *rect,struct rectangle*rect2,pushd);
Hit(enum view_MouseAction type, long x, long y, long numberOfClicks)
    returns struct view *;

classprocedures:
   InitializeObject(struct enterstrV *self) returns boolean;
   FinalizeObject(struct enterstrV *self) returns boolean;
data:
    struct entertext *etext;
    struct etextview *etextview;
};

