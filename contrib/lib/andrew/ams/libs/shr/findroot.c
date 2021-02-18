/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *findroot_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/shr/RCS/findroot.c,v 2.5 1991/09/12 15:50:04 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/shr/RCS/findroot.c,v 2.5 1991/09/12 15:50:04 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/shr/RCS/findroot.c,v $
*/
#include <andrewos.h>
#include <ams.h>

extern char *index();

FindTreeRoot(DirName, RootName, ReallyWantParent)
char *DirName, *RootName;
short ReallyWantParent;
{
    char *s, *t;

    strcpy(RootName, DirName);
    for (s = RootName; *s; ++s) {
	if (*s == '/' && !strncmp(s+1, MS_TREEROOT, sizeof(MS_TREEROOT) -1)) {
	    break;
	}
    }
    if (!*s) {
	return(-1);
    }
    if (ReallyWantParent) {
	*s = '\0';
    } else {
	t = index(++s, '/');
	if (t) *t = '\0';
    }
    return(0);
}
