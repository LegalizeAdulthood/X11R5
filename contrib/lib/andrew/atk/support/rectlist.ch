/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/rectlist.ch,v 2.7 1991/09/12 19:49:08 bobg Exp $ */
/* $ACIS:rectlist.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/rectlist.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidrectlist_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/rectlist.ch,v 2.7 1991/09/12 19:49:08 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#define rectlist_VERSION 1

package rectlist {
classprocedures:
    ResetList();
    AddOldRectangle(long bottom, long top, long left, long right);
    AddNewRectangle(long bottom, long top, long left, long right, long startScan);
    InvertRectangles(struct view *vPtr);
};
