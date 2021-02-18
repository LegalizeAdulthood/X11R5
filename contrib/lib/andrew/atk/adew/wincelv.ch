/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/wincelv.ch,v 1.6 1991/09/12 19:19:34 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/wincelv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_celview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/wincelv.ch,v 1.6 1991/09/12 19:19:34 bobg Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class wincelview[wincelv]: view {
overrides:
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    ObservedChanged (struct observable *changed, long value);
methods:
    pushchild();
    popchild();
data:
    struct celview *celview;
    struct im *window;
};

