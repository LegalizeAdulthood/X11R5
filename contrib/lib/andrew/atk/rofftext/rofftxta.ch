/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/rofftxta.ch,v 2.6 1991/09/12 19:48:13 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/rofftxta.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_rofftextapp_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/rofftxta.ch,v 2.6 1991/09/12 19:48:13 bobg Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* rofftext application */

class rofftextapp[rofftxta] : application[app] {
    overrides:
	ParseArgs(argc,argv) returns boolean;
        Start() returns boolean;
        Run() returns int;
    classprocedures:
        InitializeObject(self) returns boolean;
    data:
        char *macrofile;
        boolean RoffType;
        char *outputfile;
        char *inputfile;
        char **inputfiles;
        char **argv;
        int argc;
        boolean HelpMode;
        boolean BeCompletelyBogus;
        boolean PrintWarnings;
};
