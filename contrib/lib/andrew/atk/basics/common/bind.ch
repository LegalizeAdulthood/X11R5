/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/bind.ch,v 2.7 1991/09/12 19:21:27 bobg Exp $ */
/* $ACIS:bind.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/bind.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidbind_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/bind.ch,v 2.7 1991/09/12 19:21:27 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

struct bind_Description {
    char    *procName;
    char    *keyVector;
    long    keyRock;
    char    *menuEntry;
    long    menuRock;
    long    menuMask;
    void    (*proc)();
    char    *doc;
    char    *module;
};

package bind {
classprocedures:
    BindList(struct bind_Description *bl, struct keymap *km, struct menulist *ml, struct classinfo *type);
};
