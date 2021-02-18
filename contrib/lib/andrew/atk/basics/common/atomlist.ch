/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/atomlist.ch,v 2.6 1991/09/12 19:21:19 bobg Exp $ */
/* $ACIS:atomlist.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/atomlist.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidatomlist_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/atomlist.ch,v 2.6 1991/09/12 19:21:19 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


#include <atom.ih>

struct atoms 
{
    struct atom *atom;
    struct atoms *next;
};

class atomlist {
methods:
  Last() returns struct atom *;
    Memberp(struct atom *list) returns short;
  DropFirst();
    Prepend(struct atom *newitem);
    Append(struct atom *newitem);
    JoinToEnd(struct atomlist *otherlist);
    JoinToBeginning(struct atomlist *otherlist);
    Cut(struct atoms *mark);
macromethods:
    TraversalStart() (self->atoms)
    TraversalNext(struct atoms *loc) \
            ((loc) == NULL ? NULL : (loc)->next)
    TraversalAtom(struct atoms *loc) \
            ((loc) == NULL ? NULL : (loc)->atom)
    First() (self->atoms != NULL ? self->atoms->atom : NULL)
    Mark() (self->atoms)
classprocedures:
    InitializeObject(struct atomlist *self) returns boolean;
    FinalizeObject(struct atomlist *trashed);
    Copy(struct atomlist *oldlist) returns struct atomlist *;
    StringToAtomlist(char *string) returns struct atomlist *;
    /* (... assumes it can transiently smudge its input string) */
data:
    struct atoms *atoms;
};
