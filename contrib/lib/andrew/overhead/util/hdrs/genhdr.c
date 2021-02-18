/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *genhdr_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/hdrs/RCS/genhdr.c,v 1.3 1991/09/12 17:22:46 bobg Exp $";


#include <stdio.h>

#define	OFILENAME   "andrdir.h"

/**
 **
 **  Create the file andrdir.h which gives the defined 
 **  QUOTED_DEFAULT_ANDREWDIR_ENV and possibly the 
 **  QUOTED_DEFAULT_LOCALDIR_ENV.
 **  Also, define QUOTED_DEFAULT_ANDREWDIR_ANDREWSETUP.
 **
 **/
int
main(argc, argv)

int argc;
char *argv[];

{
    /*
     * Check command line.
     */
    if ((argc != 2) && (argc != 3)) {
	(void) fprintf(stderr, "\ngenhdr:  genhdr ANDREWDIR [LOCALDIR] \n");
	exit(1);
    }


    /*
     * Write out the #defines.
     */
    (void) printf(" \n");
    (void) printf("#define QUOTED_DEFAULT_ANDREWDIR_ENV \"%s\"\n", argv[1]);
    if (argc == 3) {
	(void) printf("#define QUOTED_DEFAULT_LOCALDIR_ENV \"%s\"\n", argv[2]);
    }
    (void) printf("#define QUOTED_DEFAULT_ANDREWDIR_ANDREWSETUP \"%s/etc/AndrewSetup\"\n", argv[1]);
    (void) printf(" \n");


    /*
     * We must check flush because flushs can fail with remote filesystems.
     */
    if (0 != fflush(stdout)) {
	(void) fprintf(stderr, "\ngenhdr:  error fflush'ing stdout\n");
	exit(1);
    }

    /*
     * All done!
     */
    exit(0);

}
