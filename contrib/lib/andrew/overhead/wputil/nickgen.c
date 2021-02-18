/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *nickgen_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/wputil/RCS/nickgen.c,v 5.5 1991/09/12 17:33:00 bobg Exp $";

#include <stdio.h>
#include <ctype.h>
#include <andrewos.h>
#include <wp.h>

#include <btwp.h>

static int Debugging = 0;
static int NickExceptions = 0;

main(argc, argv)
int argc; char *argv[];
{
    int arg;
    char *ifile = NULL;
    FILE *IF;
    static char Usage[] = " [-d(ebug)|-n(icknames)|-s(urnames)]";
    static char InBuf[1000];
    char *CPtr, *NextPtr, *Canon1, *Canon2;
    int DoSurns;

    DoSurns = 0;
    for (arg = 1; arg < argc; ++arg) {
	if (argv[arg][0] == '-') {
	    if (strcmp(argv[arg], "-d") == 0) {
		extern int bwDebugging;
		bwDebugging = Debugging = 1;
	    } else if (strcmp(argv[arg], "-s") == 0) {
		DoSurns = 1;
	    } else if (strcmp(argv[arg], "-n") == 0) {
		NickExceptions = 1;
	    } else {
		fprintf(stderr, "Bad switch %s\nusage: %s%s\n",
			argv[arg], argv[0], Usage);
		exit(1);
	    }
	} else {
	    if (ifile == NULL) {
		ifile = argv[arg];
	    } else {
		fprintf(stderr, "Extra argument %s\nusage: %s%s\n",
			argv[arg], argv[0], Usage);
		exit(1);
	    }
	}
    }
    if (ifile != NULL) {
	IF = fopen(ifile, "r");
	if (IF == NULL) {
	    fprintf(stderr, "File %s not readable\n", ifile);
	    exit(1);
	}
	if (NickExceptions) {	/* Producing dictionary of nickname mappings */
	    for (;;) {
		if (fgets(InBuf, sizeof(InBuf), IF) == NULL) exit(0);
		CPtr = index(InBuf, '\n');
		if (CPtr != NULL) *CPtr = '\0';
		else {
		    fprintf(stderr, "InBuf too small; need a larger buffer; change and recompile.\n");
		    exit(1);
		}
		Canon1 = Canon2 = NULL;
		for (CPtr = InBuf; CPtr != NULL; CPtr = NextPtr) {
		    NextPtr = index(CPtr, ' ');
		    if (NextPtr != NULL) *NextPtr++ = '\0';
		    if (Canon1 == NULL) {
			Canon1 = CanonGiven(CPtr);
			if (Canon1 == NULL) {
			    fprintf(stderr, "Out of memory on %s\n", CPtr);
			    exit(1);
			}
		    } else {
			Canon2 = CanonNick(CPtr);
			if (Canon2 == NULL) {
			    fprintf(stderr, "Out of memory on %s\n", CPtr);
			    exit(1);
			}
			if (strlen(Canon2) > strlen(Canon1)
			    || ULstlmatch(Canon1, Canon2) == 0) {
			    printf("%s\t%s\t%s\t%s\n",
				   Canon2, Canon1, CPtr, InBuf);
			}
			free(Canon2);
			Canon2 = NULL;
		    }
		}
		if (Canon1 != NULL) free(Canon1);
		if (Canon2 != NULL) free(Canon2);
	    }
	} else if (DoSurns) {
	    for (;;) {
		if (fgets(InBuf, sizeof(InBuf), IF) == NULL) exit(0);
		CPtr = index(InBuf, '\n');
		if (CPtr != NULL) *CPtr = '\0';
		Canon1 = Canon2 = NULL;
		for (CPtr = InBuf; CPtr != NULL; CPtr = NextPtr) {
		    NextPtr = index(CPtr, ' ');
		    if (NextPtr != NULL) *NextPtr++ = '\0';
		    Canon2 = CanonSurn(CPtr);
		    if (Canon2 == NULL) {
			printf("Out of memory on %s\n", CPtr);
			continue;
		    }
		    if (Canon1 != NULL) {
			if (strcmp(Canon1, Canon2) != 0) {
			    printf("%s/%s   %s/%s\n", InBuf, Canon1,
				   CPtr, Canon2);
			}
			free(Canon2);
			Canon2 = NULL;
		    } else {
			Canon1 = Canon2;
			Canon2 = NULL;
		    }
		}
		if (Canon1 != NULL) free(Canon1);
		if (Canon2 != NULL) free(Canon2);
	    }
	} else {	/* doing given/nicknames */
	    for (;;) {
		if (fgets(InBuf, sizeof(InBuf), IF) == NULL) exit(0);
		CPtr = index(InBuf, '\n');
		if (CPtr != NULL) *CPtr = '\0';
		Canon1 = Canon2 = NULL;
		for (CPtr = InBuf; CPtr != NULL; CPtr = NextPtr) {
		    NextPtr = index(CPtr, ' ');
		    if (NextPtr != NULL) *NextPtr++ = '\0';
		    if (Canon1 == NULL) {
			Canon1 = CanonGiven(CPtr);
			if (Canon1 == NULL) {
			    printf("Out of memory on %s\n", CPtr);
			    continue;
			}
		    } else {
			Canon2 = CanonNick(CPtr);
			if (Canon2 == NULL) {
			    printf("Out of memory on %s\n", CPtr);
			    continue;
			}
			if (ULstlmatch(Canon1, Canon2) == 0) {
			    printf("%s  %s	(%s/%s)\n",
				   Canon1, Canon2, InBuf, CPtr);
			}
			free(Canon2);
			Canon2 = NULL;
		    }
		}
		if (Canon1 != NULL) free(Canon1);
		if (Canon2 != NULL) free(Canon2);
	    }
	}
    }

    for (;;) {
	if (fgets(InBuf, sizeof(InBuf), stdin) == NULL) exit(0);
	CPtr = index(InBuf, '\n');
	if (CPtr != NULL) *CPtr = '\0';
	Canon1 = (DoSurns ? CanonSurn(InBuf) : CanonNick(InBuf));
	if (Canon1 == NULL) {
	    printf("out of memory\n");
	} else {
	    printf("%s    %s\n", Canon1, InBuf);
	    free(Canon1);
	}
    }
}
