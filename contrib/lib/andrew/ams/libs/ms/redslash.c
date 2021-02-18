/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *redslash_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/redslash.c,v 2.4 1991/09/12 15:47:42 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/redslash.c,v 2.4 1991/09/12 15:47:42 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/redslash.c,v $
*/
ReduceSlashes(filename)
char *filename;
{
    char *old = filename, *new = filename;
    int AtSlash = 0;

    while (*old) {
	switch (*old) {
	    case '/':
		if (AtSlash) {
		    break;
		}
		AtSlash = 1;
		*new++ = *old;
		break;
	    default:
		AtSlash = 0;
		*new++ = *old;
		break;
	}
	++old;
    }
    *new = '\0';
}    

