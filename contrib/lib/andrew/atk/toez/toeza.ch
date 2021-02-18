/* ********************************************************************** *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/toez/RCS/toeza.ch,v 1.2 1991/09/12 20:04:15 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/toez/RCS/toeza.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
char *rcsid_rofftextapp_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/toez/RCS/toeza.ch,v 1.2 1991/09/12 20:04:15 bobg Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* toandrew application */

class toezapp[toeza] : application[app] {

overrides:
	ParseArgs(argc, argv) returns boolean;
	Start() returns boolean;
	Run() returns int;

classprocedures:
	InitializeObject(self) returns boolean;
	FinalizeObject(self);

data:

	char *inputfile;
	char *outputfile;
	char *inputtype;
	FILE *inf;
	FILE *outf;

	/*  vars for rofftext */
	char *macrofile;		/* /usr/lib/tmac/tmac.%s */
	boolean HelpMode;	/* ensures \n atop page */
	boolean BeCompletelyBogus;  /* only used in toandya */
};
