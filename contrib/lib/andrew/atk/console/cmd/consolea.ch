/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/cmd/RCS/consolea.ch,v 2.7 1991/09/12 19:27:35 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/cmd/RCS/consolea.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidconsoleapp_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/cmd/RCS/consolea.ch,v 2.7 1991/09/12 19:27:35 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


class consoleapp[consolea] : application[app] {
    overrides:
	ParseArgs(int argc,char **argv) returns boolean;
	Start() returns boolean;
    classprocedures:
      InitializeClass() returns boolean;
      InitializeObject((struct consoleapp *) self) returns boolean;
      FinalizeObject(self);
    data:
        char *consoleName;
};
