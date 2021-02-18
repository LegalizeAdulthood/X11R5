/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *amsgenid_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/mail/cmd/RCS/amsgenid.c,v 1.6 1991/09/12 17:13:52 bobg Exp $";

/* 
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/mail/cmd/RCS/amsgenid.c,v 1.6 1991/09/12 17:13:52 bobg Exp $

ams_genid -- generate a world-unique identifier on stdout for usage in scripts.

*/

#include <stdio.h>
#include <mailconf.h>
#include <mail.h>

main(argc, argv)
int argc;
char **argv;
{
    int i;
    int isFile = 0;
    
    for (i = 1; i < argc; ++i) {
	if (strcmp(argv[i], "-f") == 0) isFile = 1;
	else {
		fprintf(stderr, "usage: %s [-f]\n", argv[0]);
		exit(2);
	}
    }
    puts(ams_genid(isFile));
    exit(0);
}
