/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/frame/RCS/framecmd.ch,v 2.10 1991/09/12 19:39:55 bobg Exp $ */
/* $ACIS:framecmd.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/frame/RCS/framecmd.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidframecmds_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/frame/RCS/framecmd.ch,v 2.10 1991/09/12 19:39:55 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* framecmd.H
 * A vestigial class to make static loading of framecmds optional.
 *
 */

package framecmds[framecmd] {
    classprocedures:
        InitKeymap(struct menulist **menuOut, struct menulist **defaultmenuOut) returns struct keymap *;
        InitializeClass() returns boolean;
};
