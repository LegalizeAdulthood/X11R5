/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *fixsub_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/fixsub.c,v 2.5 1991/09/12 15:43:15 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/fixsub.c,v 2.5 1991/09/12 15:43:15 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/fixsub.c,v $
*/
#include <ms.h>

#ifndef _IBMR2
extern char *malloc();
#endif /* _IBMR2 */

struct {
    char *left, *right;
} SubjectProductions[] = {
    "re: re:", "Re:",
    "fwd: fwd:", "Fwd:",
    "re: fwd:", "Re:",
    "fwd: re:", "Fwd:",
    0, 0};

char *
RewriteSubject(oldsub)
char *oldsub;
{
    char *newsub, *tempsub;
    Boolean DidSomething = TRUE;
    int i;

    newsub = malloc(1+strlen(oldsub));
    if (!newsub) return(NULL);
    strcpy(newsub, StripWhiteEnds(oldsub));
    ReduceWhiteSpace(newsub);
    while (DidSomething) {
	DidSomething = FALSE;
	for (i=0; SubjectProductions[i].left; ++i) {
	    if (!lc2strncmp(SubjectProductions[i].left, newsub, strlen(SubjectProductions[i].left))) {
		DidSomething = TRUE;
		tempsub = malloc(1+strlen(newsub));
		if (!tempsub) {
		    free(newsub);
		    return(NULL);
		}
		sprintf(tempsub, "%s%s", SubjectProductions[i].right, newsub+strlen(SubjectProductions[i].left));
		free(newsub);
		newsub = tempsub;
		break;
	    }
	}
    }
    return(newsub);
}
