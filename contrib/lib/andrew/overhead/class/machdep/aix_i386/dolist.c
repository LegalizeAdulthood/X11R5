/* ********************************************************************** *\
 *	   Copyright IBM Corporation 1988,1991 - All Rights Reserved	  *
 *	  For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *dolist_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/class/machdep/aix_i386/RCS/dolist.c,v 1.2 1991/09/12 17:00:04 bobg Exp $";

/*
	dolist.c - list contents of a.out or .o file

	Author:  John H Howard - April 9, 1987
 */


#include <stdio.h>
#include <a.out.h>
#include <setjmp.h>
#include <andrewos.h>
#include <doload.h>

/* read and list module */

void ListIt(inFD)		/* return pointer to data segment, */
				/* or NULL if error */
int inFD;			/* open fd for package file */
{
    struct doload_environment E;
    register struct doload_environment *e;
    RELOC *rp;
    int s;

    /* set up environment */

    doload_setup(e = &E, inFD, List);
    if (setjmp(e->errorJump)) {
	printf("\nerror exit taken\n"); fflush(stdout); fflush(stderr);
	doload_cleanup(e);
	return;
    }

    /* read module into memory */

    doload_read(e);

    /* list symbol table */

    doload_preset(e);

    /* list contents */

    doload_fixup(e);

    (void) puts("\n");
    doload_cleanup(e);
    return ;
}

/* main program for testing doload */

main(argc, argp)
int argc;
char **argp;
{
    int fd;
    int gotcha = 0;

    while (--argc > 0) {
	if (**++argp == '-') {
	    switch (*++*argp) {
	    case 'd':
		doload_trace += 1;
		break;
	    default:
		fprintf( stderr,
			 "dolist:  Unknown switch -%c ignored\n", **argp);
	    }
	}
	else {
	    gotcha += 1;
	    fd = open(*argp, O_RDONLY, 0);
	    if (fd < 0)
		fprintf(stderr, "dolist:  File %s not found\n", *argp);
	    else {
		(void) printf("Listing of program file %s\n", *argp);
		ListIt(fd);
		close(fd);
	    }
	}
    }
    if (gotcha == 0) {
	(void) puts("Listing of program file from standard input");
	ListIt(0);
    }
return 0 ;
}
