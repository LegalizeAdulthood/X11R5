/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/typescript/RCS/pscripta.ch,v 2.8 1991/09/12 20:04:27 bobg Exp $ */
/* $ACIS:pscripta.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/typescript/RCS/pscripta.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidpscriptapp_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/typescript/RCS/pscripta.ch,v 2.8 1991/09/12 20:04:27 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class pipescriptapp[pscripta] :application[app] {
overrides:
    ParseArgs(int argc, char **argv) returns boolean;
    Start() returns boolean;
data:
    FILE *df;
    struct typescript *ss;
    struct frame *frame;
    int ShellMenu;
    int filemenu;
    char *Menu;
    char *titleLine ;
    char *font ;
    char **argv;
};

