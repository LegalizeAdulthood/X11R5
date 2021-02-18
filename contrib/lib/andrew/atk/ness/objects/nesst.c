/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nesst.c,v 1.2 1991/09/12 16:26:41 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nesst.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	char *nesst_c_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nesst.c,v 1.2 1991/09/12 16:26:41 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/* nesst.c

	test the ness object
*/


#include <stdio.h>

#include <ness.ih>


/* include .ih files to be loaded with the binary of the file but none of whose
		methods are called in this file itself */
#define class_StaticEntriesOnly

/* required classes */
#include <environ.ih>

/* classes that are anyway dynamically loaded by the
	required classes */
#include <dataobj.ih>
#include <event.ih>
#include <filetype.ih>
#include <proctbl.ih>
#include <observe.ih>

#undef class_StaticEntriesOnly



main(argc, argv)
	register int argc;
	register char **argv;
{
	register struct ness *dobj;
	boolean debug = TRUE;

	FILE *f;
	char *fnm;
	long i, id;
    
	printf("Start\n"); fflush(stdout);
  	class_Init(AndrewDir("/dlib/atk"));

	printf("Init done\n"); fflush(stdout);

/*	Cause static linking and loading of most ATK routines:
 */
	environ_StaticEntry;
	dataobject_StaticEntry;
	event_StaticEntry;
	filetype_StaticEntry;
	proctable_StaticEntry;
	observable_StaticEntry;

/* dynamically load ness so nesst need not be relinked when ness is recompiled */

	fnm = NULL;
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-')
			switch (argv[i][1]) {
				case 'f': /* Debugging. */
					debug = FALSE;
					break;
				default: /* Unknown switch. Treat it as a file... */
					fnm = argv[i]+1;
					break;
			}
		else { /* Its a file right? */
			fnm = argv[i];
		}
	}

	printf("About to New\n"); fflush(stdout);

	dobj = ness_New();
	printdata(dobj);

	/* $$$ establish some initial contents for the data object */
	if (fnm && (f=fopen(fnm, "r")))
		ness_Read(dobj, f, 0);
	else {
		static unsigned char *small = "function main(arg) printline(arg); end function";
		ness_InsertCharacters(dobj, 0, small, strlen(small));
	}

	printdata(dobj);
	printf("writing t1\n");  fflush(stdout);
	ness_Write(dobj, f=fopen("/tmp/t1", "w"), 12, 0);
	fclose(f);
	ness_Destroy(dobj);
	dobj = ness_New();
	printf("reading t1\n");  fflush(stdout);
	f = fopen("/tmp/t1", "r");
	fscanf(f, "\\begindata{ness,%d} ", &id);
	printf("read: %d\n",ness_Read(dobj, f, id));
	fclose(f);
	printdata(dobj);
	printf("writing t2\n");  fflush(stdout);
	ness_Write(dobj, f=fopen("/tmp/t2", "w"), 12, 0);
}

printdata(dobj)
	register struct ness *dobj;
{
	printf("Origin: %s\n", dobj->Origin);
	printf("OriginalModValue: %ld\n", dobj->OriginalModValue);
	printf("ScriptLoc: %ld\n", dobj->ScriptLoc);
	printf("AfterScriptLoc: %ld\n", dobj->AfterScriptLoc);
	printf("DeauthButtonLoc: %ld\n", dobj->DeauthButtonLoc);
	printf("ScanButtonLoc: %ld\n", dobj->ScanButtonLoc);
	printf("CompileButtonLoc: %ld\n", dobj->CompileButtonLoc);
	printf("accesslevel: %ld\n", dobj->accesslevel);
	printf("DisplayDialogBox: %ld\n", dobj->DisplayDialogBox);
	printf("hasWarningText: %ld\n", dobj->hasWarningText);
	fflush(stdout);
}

