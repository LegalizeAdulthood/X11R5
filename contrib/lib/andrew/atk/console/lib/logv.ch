/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/logv.ch,v 2.7 1991/09/12 19:28:25 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/logv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidlogview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/logv.ch,v 2.7 1991/09/12 19:28:25 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class logview[logv] : textview[textv] {
overrides:
    LoseInputFocus();
    PostMenus(struct menulist *menulist);
    FrameDot(long pos);
classprocedures:
    InitializeClass() returns boolean;
    InitializeObject((struct logview *)self) returns boolean;
    FinalizeObject();
data:
    struct menulist *menu;
};
