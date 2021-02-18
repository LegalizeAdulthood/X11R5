/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/updlist.ch,v 2.8 1991/09/12 19:23:40 bobg Exp $ */
/* $ACIS:updlist.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/updlist.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidupdatelist_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/updlist.ch,v 2.8 1991/09/12 19:23:40 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


struct updateitem {
    struct view *view;
    struct updateitem *next;
};

class updatelist[updlist]  {
methods:
    AddTo(struct view *view); /* Adds an update request. */
    DeleteTree(struct view *tree); /* Removes update requests for views below the named view. */
    Clear(); /* Calls view_Update on all views that have been added and deletes them from the list. */
macromethods:
    UpdatesPending() (self->updates != NULL)
classprocedures:
    FinalizeObject(struct updatelist *self);
    InitializeObject(struct updatelist *self) returns boolean;
data:
    struct updateitem *updates;
};
