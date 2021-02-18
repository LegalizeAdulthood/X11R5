/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/typescript/RCS/tscripta.ch,v 2.8 1991/09/12 20:04:41 bobg Exp $ */
/* $ACIS:tscripta.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/typescript/RCS/tscripta.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtscriptapp_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/typescript/RCS/tscripta.ch,v 2.8 1991/09/12 20:04:41 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class typescriptapp[tscripta] : application[app] {
classprocedures:
    InitializeObject(struct typescriptapp *self) returns boolean;
    FinalizeObject(struct typescriptapp *self);
overrides:
    ParseArgs(int argc, char **argv) returns boolean;
    Start() returns boolean;
    Stop();
data:
    struct typescript *ss;
    struct frame *frame;
    int ShellMenu;
    int filemenu;
    char *Menu;
    char *titleLine ;
    char *font ;
    char **argv;
 };

