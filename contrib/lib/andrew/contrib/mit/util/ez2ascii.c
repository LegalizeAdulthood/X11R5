/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *ezascii_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/util/RCS/ez2ascii.c,v 1.3 1991/09/12 16:52:05 bobg Exp $";

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/util/RCS/ez2ascii.c,v 1.3 1991/09/12 16:52:05 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/util/RCS/ez2ascii.c,v $ */
/* $Author: bobg $ */
/*
 * Program to convert ATK datastream to plain ascii.
 *
 * Relies on overhead/mail/lib/unscribe code.
 * Uses same algorithm that sendmessages uses to strip
 * formatting and to break lines.
  *
  * Usage: ez2ascii [infile [outfile]]
  *
  * Outfile gets the stripped version of the file.
  * If outfile is not specified, output goes to stdout.
  * If no file is specified, stdin is read and output goes to stdout.
 */

#include <stdio.h>
#include <sys/file.h>

#include <andrewos.h>
#include <unscribe.h>

static int ez2a_doit (infile, outfile)
FILE *infile, *outfile;
{
    struct ScribeState *ussp;
    int version, err;
    char buf[1027];

    version = UnScribeInit(" 12", &ussp);

    while (fgets(buf,sizeof(buf)-1,infile)) {
	err = UnScribe(version, &ussp, buf, strlen(buf), outfile);
	if (err < 0) {
	    fprintf(stderr, "UnScribe error == %d.\n", err);
	    return -1;
	}
    }
    UnScribeFlush(version, &ussp, outfile);
    return 0;
}

main(argc, argv)
int argc;
char *argv[];
{
    FILE *inptr, *outptr;
    int status = -1;

    if (argc > 3) {
	fprintf(stderr, "Too many arguments. First arg is input file; second is output file.\n");
    } else if (argc == 1) {
	status = ez2a_doit (stdin, stdout);
    } else if(argc == 3) {
	if ((outptr = fopen(argv[2], "w")) == NULL) {
	    fprintf(stderr, "Could not open output file '%s'.\n", argv[2]);
	    exit(-1);
	}
	if ((inptr = fopen((argv[1]), "r")) == NULL) {
	    fprintf(stderr,	"Could not open	input file '%s'.\n", argv[1]);
	    exit (-1);
	}
	status = ez2a_doit (inptr, outptr);
    } else {
	if ((inptr = fopen((argv[1]), "r")) == NULL) {
	    fprintf(stderr,	"Could not open	input file '%s'.\n", argv[1]);
	    exit (-1);
	}
	status = ez2a_doit (inptr, stdout);
    }
    exit (status);
}
