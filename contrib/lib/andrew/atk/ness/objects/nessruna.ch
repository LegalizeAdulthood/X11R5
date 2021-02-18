/* ********************************************************************** *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nessruna.ch,v 1.2 1991/09/12 19:44:18 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nessruna.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
char *rcsid_rofftextapp_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nessruna.ch,v 1.2 1991/09/12 19:44:18 bobg Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* toandrew application */

class nessrunapp[nessruna] : application[app] {

overrides:
	ParseArgs(argc, argv) returns boolean;
	Start() returns boolean;
	Run() returns int;

classprocedures:
	InitializeObject(self) returns boolean;
	FinalizeObject(self);

data:
	char *inputfile;
	struct ness *theNess;
	boolean dump;
};
