/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/init.ch,v 2.7 1991/09/12 19:22:11 bobg Exp $ */
/* $ACIS:init.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/init.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidinit_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/init.ch,v 2.7 1991/09/12 19:22:11 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* init.H
 * Class header file for initialization file reader.
 *
 */

class init {
    methods:
        ModifyMenulist(struct menulist *ml) returns struct menulist *;
        ModifyKeystate(struct keystate *ks) returns struct keystate *;
        Duplicate() returns struct init *;
        Load(char *filename, procedure errorProc, long errorRock, boolean executeImmediately) returns int;
        AddKeyBinding(char *class, boolean inherit, struct keymap *keymap);
        DeleteKeyBinding(char *class, boolean inherit, struct keymap *keymap);
        AddMenuBinding(char *class, boolean inherit, struct menulist *menulist);
        DeleteMenuBinding(char *class, boolean inherit, struct menulist *menulist);

    data:
        struct keys *keys;
        struct menus *menus;
        struct keystateList *usedKeystates;
        struct mlList *usedMenus;
};
