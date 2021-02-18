/* ********************************************************************** *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nessruna.c,v 1.4 1991/09/12 16:26:33 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nessruna.c,v $ */

#ifndef lint
char *nessrunapp_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nessruna.c,v 1.4 1991/09/12 16:26:33 bobg Exp $ ";
#endif /* lint */

/*
 * app for ness
 *
 *	Program to execute a Ness file.  
 *
 *	nessrun [-d]  programfilename args
 *
 *	programfilename is the name of a file containing the ness program
 *		it must have a function main(), which will be called
 *		to initiate execution
 *	remaining args are concatenated and passed as a single marker parameter to main()
 *
 *	The -d switch causes the compiler to dump the generated code.
 */

/*
 * $Log: nessruna.c,v $
 * Revision 1.4  1991/09/12  16:26:33  bobg
 * Update copyright notice and rcsid
 *
 * Revision 1.3  1990/08/01  16:27:53  wjh
 * fixed a bug introduced last week where it mistakenly included the file name in the args as given to main()
 *
 * Revision 1.2  90/07/25  16:35:22  wjh
 * fix core dump on Sun3
 * 
 * Revision 1.1  90/07/15  15:24:35  wjh
 * Initial revision
 * 
* 11 May 1990 WJH converted from old standalone nessrun program.
 */
#include <stdio.h>
#include <andrewos.h>
#include <nessruna.eh>

#include <event.ih>
#include <mark.ih>
#include <nessmark.ih>
#include <ness.ih>
#include <xgetchar.h>
#include <filetype.ih>
#include <text.ih>
#include <textv.ih>
#include <error.h>

struct ness *program;


	boolean
nessrunapp__InitializeObject(classID,self)
	struct classheader *classID;
	struct nessrunapp *self;
{
	self->inputfile = NULL;
	self->dump = FALSE;
	self->theNess = ness_New();
	nessrunapp_SetMajorVersion(self, CURRENTSYNTAXLEVEL);
	nessrunapp_SetMinorVersion(self, CURRENTMODIFICATIONLEVEL);
	nessrunapp_SetFork(self, FALSE);
	return  TRUE;
}

	void
nessrunapp__FinalizeObject(classID,self)
	struct classheader *classID;
	struct nessrunapp *self;
{
	ness_Destroy(self->theNess);
	/* do not free self->inputfile because it is in argv */
}


/*
 * usage statement
 */
	static void
show_usage(self)
	struct nessrunapp *self;
{
	fprintf(stderr,
		"Usage: %s  [-d]  programfilename  arguments\n",
		nessrunapp_GetName(self));
	fprintf(stderr,
"\
	-d: display generated code\n\
	programfilename: execute program in this files\n\
	args: all further text is passed as the arg to main() in the program\n\
");
}

	void
dumpall()
{
	struct ness *n;
	unsigned char *name;
	for (n = ness_GetList(); n != NULL; n = ness_GetNext(n)) {
		name = ness_GetName(n);
		printf("\nObject code for %s\n", 
			(name != NULL) ? name : (unsigned char *)"unknown");
		ness_dumpattrs(n, stdout);
	}
}


	boolean 
nessrunapp__ParseArgs(self, argc, argv)
	struct nessrunapp *self;
	int argc;
	char **argv;
{
	struct nessmark *arg, *args, *blank;

	if(!super_ParseArgs(self, argc, argv))
		return FALSE;

	/* super_ParseArgs() passes across the "runapp" and its switches,
		leaving "nessrun" as the first arg.   */

	while(*++argv != NULL && **argv == '-') {
		switch((*argv)[1]){
			case 'd':		
				self->dump = TRUE;
				break;
			case 'f':		
				nessrunapp_SetFork(self, TRUE);
				break;
			default:
				fprintf(stderr,"%s: unrecognized switch: %s\n",
					nessrunapp_GetName(self), *argv);
				show_usage(self);
				return FALSE;
		}
	}

	if (*argv == NULL) {
		fprintf(stderr,"%s: no programfilename specified\n",
				nessrunapp_GetName(self));
		show_usage(self);
		return FALSE;
	}

	/* get the name of the ness program file */
	self->inputfile = *argv++;

	if (*argv != NULL) {
		/* concatenate args to pass to theNess */

		blank = nessmark_New();
		nessmark_MakeConst(blank, " ");
		arg = nessmark_New();
		args = nessmark_New();
		nessmark_SetText(args, simpletext_New());
		while (*argv != NULL) {
			nessmark_MakeConst(arg, *argv);
			nessmark_Next(args);
			nessmark_Replace(args, arg);
			nessmark_Next(args);
			nessmark_Replace(args, blank);
			argv++;
		}
		nessmark_Base(args);
		ness_SupplyMarkerArg(self->theNess, args);
	}
	return TRUE;
}

	boolean
nessrunapp__Start(self)
	struct nessrunapp *self;
{
	if (ness_ReadNamedFile(self->theNess, self->inputfile) 
				!= dataobject_NOREADERROR) {
		fprintf(stderr, "Input file is neither plain text not ATK format: %s\n",
				self->inputfile);
		return FALSE;
	}
	return TRUE;
}


	int 
nessrunapp__Run(self)
	struct nessrunapp *self;
{
	struct errornode *result;
	long t0, t1;
	struct text *text;
	struct textview *textview;
	boolean forkit;

	text = text_New();
	textview = textview_New();
	textview_SetDataObject(textview, text);
	ness_SetDefaultText(self->theNess, textview);

	ness_SetName(self->theNess, self->inputfile);
	ness_SetAccessLevel(self->theNess, ness_codeUV);

			t0 = event_TUtoMSEC(event_Now());
	result = ness_Compile(self->theNess);
			t1 = event_TUtoMSEC(event_Now());

	if (self->dump)
		dumpall();

	if (ness_PrintAllErrors("Compile") != 0)
		return(1);
	else printf ("Compile okay.  Elapsed time is %d msec.\n", t1-t0);

	forkit = nessrunapp_GetFork(self);
	if(!nessrunapp_Fork(self))
		return -1;

			t0 = event_TUtoMSEC(event_Now());
	result = ness_Execute(self->theNess, "main");
			t1 = event_TUtoMSEC(event_Now());

	if (result != NULL) 
		ness_PrintAllErrors("Execution");
	else if ( ! forkit)
		printf ("Execution okay.  Elapsed time is %d msec.\n", t1-t0);

	return ((result == NULL) ? 0 : 1);
}
