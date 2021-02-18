/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/type/RCS/chtest.c,v 1.6 1991/09/12 16:27:34 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/type/RCS/chtest.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
char *chtest_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/type/RCS/chtest.c,v 1.6 1991/09/12 16:27:34 bobg Exp $";
#endif

/*
 *	TEST PROGRAM FOR .CH FILE SYNTAX

	usage:  chtest  [-r]  [-d classname [-p itemname] ]  filename...

	Reads the named files and parses them as .ch files, checking for errors.

		(for both -d and -p there is a space between switch and name)
	-d classname  prints the declarations for the named class
	-p itemname  prints the type structure contents 
			for the named method, classproc, or variable
		(Note that -p can only be given if -d is also.)
	-r causes a reset after each file.  This is more accurate, but slower.
 */

/*
 *   $Log: chtest.c,v $
 * Revision 1.6  1991/09/12  16:27:34  bobg
 * Update copyright notice and rcsid
 *
 * Revision 1.5  1989/09/02  23:59:07  wjh
 * remove spurious declaration
 *
 * Revision 1.4  89/09/02  18:44:01  wjh
 * fixed argument scanning order
 * 
 * Revision 1.3  89/09/02  09:14:48  wjh
 * added flags -d -p -r
 * 
 *
 */

#include <type.ih>

struct type_ctypes *Ctypes;

	void
DoDumps(dumpclass, dumpitem)
	char *dumpclass, *dumpitem;
{
	struct type *classtype, *ttt, *elt;
	classtype = type_Lookup(Ctypes->basicobject, dumpclass);
	if (classtype != NULL) {
		type_Print(classtype);
		dumpclass = NULL;  /* do it only once */
		if (dumpitem != NULL) {
			ttt = type_Lookup(classtype, dumpitem);
			if (ttt != NULL) {
				type_Print(ttt);
				printf("\nargs in reverse order:\n\n");
				for (elt = type_GetPrev(ttt);
						elt != NULL;
						elt = type_GetPrev(elt))
					type_Print(elt);
			}
		}
	}
}

main(argc, argv)
	register int argc;
	register char **argv;
{
	char *result;

	boolean rswitch = FALSE;
	char *dumpitem = NULL;
	char *dumpclass = NULL;

	initClass();
	Ctypes = type_GetCtypes();

	argv++; argc--;	/* skip argv[0] */

	/* process switches */
	while (argc > 0 && **argv == '-') switch ((*argv)[1]) {
		case 'r':	rswitch = TRUE;  argv++; argc--;  break;
		case 'd':  argv++; argc--;  dumpclass = *argv; 
			argv++; argc--;  break;
		case 'p':  argv++; argc--;  dumpitem = *argv; 
			argv++; argc--;  
			if (dumpclass != NULL) 
				break;
			else {/* DROPTHRU for error */}
		default:	printf("usage: chtest  [-r]  [-d classname [-p itemname] ]  filename...\n");
			exit(1);
	}

	printf("testing %d .ch file%s\n\n", argc, (argc==1) ? "" : "s");

	while (argc-- > 0) {
		printf("compiling %s\n", *argv);
		result = type_DeclareFromFile(*argv++);
		if (*result == '\0') {
			printf("declaration successful\n\n");
			if (dumpclass != NULL) 
				DoDumps(dumpclass, dumpitem);
		}
		else 
			printf("declaration failed with the following message:\n%s\n\n", result);
		if (rswitch)
			type_Reset();
	}
}

initClass()
{
	char *classPath;
    
	classPath = (char *) AndrewDir("/dlib/atk");
	class_Init(classPath);
}
