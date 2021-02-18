/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/mtchfold.c,v 1.10 1991/09/12 15:45:55 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/mtchfold.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/mtchfold.c,v 1.10 1991/09/12 15:45:55 bobg Exp $ ";
#endif /* lint */

#include <stdio.h>
#include <ms.h>
#include <andrewos.h>
#include <mailconf.h>
#ifdef M_UNIX
#include <dirent.h>
#define direct dirent
#else
#include <sys/dir.h>
#endif

extern char *index(), *rindex();
extern FILE *fopen();

long MS_MatchFolderName(pat, filename)
char *pat, *filename;
{
    int i, patlen;
    long errsave;
    FILE *wfp, *rfp;
    char SubsMapFile[1+MAXPATHLEN], LineBuf[3*MAXPATHLEN], *s;

    if (!pat) pat = "";
    GenTempName(filename);
    wfp = fopen(filename, "w");
    if (!wfp) {
	AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_MATCHFOLDERNAME);
    }
    if (*pat == '/') { /* Searching in absolute path  -- might not even be on mspath. */
	char dirname[1+MAXPATHLEN];
	DIR *dirp;
	struct direct *dirent;
	int len;

	strcpy(dirname, pat);
	s = rindex(dirname, '/');
	if (s) {
	    *s++ = NULL;
	} else {
	    strcpy(dirname, "/");
	    s = pat;
	}
	if ((dirp = opendir(dirname)) == NULL) {
	    errsave = errno;
	    fclose(wfp);
	    unlink(filename);
	    AMS_RETURN_ERRCODE(errsave, EIN_OPENDIR, EVIA_MATCHFOLDERNAME);
	}
	len = strlen(s);
	while ((dirent = readdir(dirp)) != NULL) {
	    if (!strncmp(dirent->d_name, s, len)) {
		fprintf(wfp, "%s/%s\n", dirname, dirent->d_name);
	    }
	}
	closedir(dirp);
    } else { /* Searching through mspath preference */
	for (s = pat; *s; ++s) {
	    if (*s == '/') *s = '.';
	}
	patlen = strlen(pat);
	for (i=0; i<MS_NumDirsInSearchPath; ++i) {
	    sprintf(SubsMapFile, "%s/%s", SearchPathElements[i].Path, AMS_SUBSCRIPTIONMAPFILE);
	    if (!(rfp = fopen(SubsMapFile, "r"))) {
		NonfatalBizarreError("Unreadable directory on mspath, ignoring!");
		continue;
	    }
	    while(fgets(LineBuf, sizeof(LineBuf), rfp) != NULL) {
		s = index(LineBuf, ':');
		if (s) *s = '\0';
		if ((strlen(LineBuf) >= patlen) && !strncmp(LineBuf, pat, patlen)) {
		    fputs(LineBuf, wfp);
		    fputs("\n", wfp);
		}
	    }
	    fclose(rfp);
	}
    }
    if (vfclose(wfp)) {
	errsave = errno;
	unlink(filename);
	AMS_RETURN_ERRCODE(errsave, EIN_VFCLOSE, EVIA_MATCHFOLDERNAME);
    }
    return(0);
}
