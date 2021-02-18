/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *scavenge_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/scavenge.c,v 1.6 1991/09/12 15:48:16 bobg Exp $";



#include <andrewos.h>
#include <ms.h>
#ifdef M_UNIX
#include <dirent.h>
#define direct dirent
#else
#include <sys/dir.h>
#endif
#include <sys/stat.h>
#include <ctype.h>

#ifndef EVIA_SCAVENGE
#define EVIA_SCAVENGE EVIA_UNKNOWN /* temp hack until first wash */
#endif

#ifndef _IBMR2
extern char *malloc();
#endif /* _IBMR2 */

long MS_ScavengeDirectory(DirName, Recurse, numgood, numbad, quiet, Purge)
char *DirName;
int Recurse, *numgood, *numbad, quiet, Purge;
{
    *numgood = *numbad = 0;
    if (!Recurse) {
	ScavengeOneDirectory(DirName, numgood, numbad, quiet, Purge);
	return(0);
    } else {
	return(ScavengeDown(DirName, numgood, numbad, quiet, Purge));
    }
}

ScavengeDown(DirName, numgood, numbad, quiet, Purge)
char *DirName;
int *numgood, *numbad, quiet, Purge;
{
    DIR *dirp;
    struct direct *dirent;
    struct stat stbuf;
    char Prefix[MAXPATHLEN+1];
    char **Children;
    int PathOffset, ChildrenCt=0, ChildrenAllocated = 200, i;

    Children = (char **) malloc(ChildrenAllocated * sizeof(char *));
    if (!Children) {
	AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_SCAVENGE);
    }
    if ((dirp = opendir(DirName)) == NULL) {
	free(Children);
	AMS_RETURN_ERRCODE(errno, EIN_OPENDIR, EVIA_SCAVENGE);
    }

    sprintf(Prefix, "%s/", DirName);
    PathOffset = strlen(Prefix);
    for (dirent = readdir(dirp); dirent != NULL; dirent = readdir(dirp)) {
	if (*dirent->d_name == '+' || *dirent->d_name == '.') {
	    continue;
	}
	Children[ChildrenCt] = malloc(PathOffset + strlen(dirent->d_name)+2);
	if (Children[ChildrenCt] == NULL) {
	    closedir(dirp);
	    while (--ChildrenCt >= 0) free (Children[ChildrenCt]);
	    free(Children);
	    AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_SCAVENGE);
	}
	sprintf(Children[ChildrenCt], "%s%s", Prefix, dirent->d_name);
	stat(Children[ChildrenCt], &stbuf);
	if ((stbuf.st_mode & S_IFMT) != S_IFDIR) {
	    debug(4, ("Skipping non-directory file %s\n", Children[ChildrenCt]));
	    continue;
	}
	if (++ChildrenCt >= ChildrenAllocated) {
	    ChildrenAllocated += 200;
	    Children = (char **) realloc(Children, ChildrenAllocated * sizeof(char *));
	    if (!Children) {
		closedir(dirp);
		AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_SCAVENGE);
	    }
	}
    }
    closedir(dirp);
    for (i=0; i<ChildrenCt; ++i) {
	mserrcode = ScavengeDown(Children[i], numgood, numbad, quiet, Purge);
	if (mserrcode) {
	    while (ChildrenCt-- > 0) free (Children[ChildrenCt]);
	    free(Children);
	    return(mserrcode);
	}
    }
    while (ChildrenCt-- > 0) free (Children[ChildrenCt]);
    free(Children);
    ScavengeOneDirectory(DirName, numgood, numbad, quiet, Purge);
    return(0);
}

ScavengeOneDirectory(DirName, numgood, numbad, quiet, Purge)
char *DirName;
int *numgood, *numbad, quiet, Purge;
{
    struct MS_Directory *Dir;
    char ErrorText[100+MAXPATHLEN];

    if (ReadOrFindMSDir(DirName, &Dir, MD_APPEND)
	 || HandleMarksInProgress(Dir, quiet)
	 || CloseMSDir(Dir, MD_APPEND)
	 || DropHint(DirName)) {

	sprintf(ErrorText, "Scavenge failed for %s (%d, %d, %d)\n", DirName, AMS_ERRNO, AMS_ERRCAUSE, AMS_ERRVIA);
	NonfatalBizarreError(ErrorText);
	++*numbad;
    } else {
	++*numgood;
	if (Purge && MS_PurgeDeletedMessages(DirName)) {
	    sprintf(ErrorText, "Could not purge deletions in %s (%d, %d, %d)\n", DirName, AMS_ERRNO, AMS_ERRCAUSE, AMS_ERRVIA);
	    NonfatalBizarreError(ErrorText);
	}
    }
    return(0);
}
