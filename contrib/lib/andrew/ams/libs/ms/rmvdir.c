/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *rmvdir_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/rmvdir.c,v 2.9 1991/09/12 15:48:01 bobg Exp $";


/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/rmvdir.c,v 2.9 1991/09/12 15:48:01 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/rmvdir.c,v $
*/
#include <ms.h>
#include <andrewos.h>
#ifdef M_UNIX
#include <dirent.h>
#define direct dirent
#else
#include <sys/dir.h>
#endif
#include <sys/stat.h>

extern char *getprofile(), *StripWhiteEnds(), *index();

long    MS_RemoveDirectory (DirName, MaxRemovals)
char   *DirName;
int MaxRemovals;
{
    struct MS_Directory *Dir;
    DIR *dirp;
    struct direct *dirent;
    struct stat stbuf;
    char FileName[1+MAXPATHLEN];

    debug(1, ("Entering MS_RemoveDirectory %s\n", DirName));
    if (ReadOrFindMSDir(DirName, &Dir, MD_OK)) {
	return(mserrcode);
    }
    if (Dir->MessageCount > MaxRemovals) {
	AMS_RETURN_ERRCODE(ENOTEMPTY, EIN_PARAMCHECK, EVIA_REMOVEDIR);
    }
    if ((dirp = opendir(DirName)) == NULL) {
	debug(4, ("Can't open source directory %s\n", DirName));
	AMS_RETURN_ERRCODE(errno, EIN_OPENDIR, EVIA_REMOVEDIR);
    }
    while ((dirent = readdir(dirp)) != NULL) {
	if (!strcmp(dirent->d_name, ".")) continue;
	if (!strcmp(dirent->d_name, "..")) continue;
	sprintf(FileName, "%s/%s", DirName, dirent->d_name);
	if (stat(FileName, &stbuf)) {
	    closedir(dirp);
	    AMS_RETURN_ERRCODE(errno, EIN_STAT, EVIA_REMOVEDIR);
	}
	if ((stbuf.st_mode & S_IFMT) == S_IFDIR) {
	    closedir(dirp);
	    AMS_RETURN_ERRCODE(EMSDIRHASKIDS, EIN_PARAMCHECK, EVIA_REMOVEDIR);
	}
    }
    rewinddir(dirp);
    while ((dirent = readdir(dirp)) != NULL) {
	if (!strcmp(dirent->d_name, ".")) continue;
	if (!strcmp(dirent->d_name, "..")) continue;
	sprintf(FileName, "%s/%s", DirName, dirent->d_name);
	if (unlink(FileName) && errno != ENOENT) { /* ENOENT is spurious */
	    closedir(dirp);
	    AMS_RETURN_ERRCODE(errno, EIN_UNLINK, EVIA_REMOVEDIR);
	}
    }
    closedir(dirp);
    if(rmdir(DirName) && errno != ENOENT) { /* BOGUS -- spurious vice ENOENT */
	AMS_RETURN_ERRCODE(errno, EIN_RMDIR, EVIA_REMOVEDIR);
    }
    RemoveSubsEntry(DirName);
    SetSubsEntry(DirName, "", AMS_UNSUBSCRIBED);
    DropHint(DirName);
    RemoveFromCrucialClassesPreference(DirName, NULL);
    return(EnsureNotInSubscriptionMap(DirName));
}

RemoveFromCrucialClassesPreference(DirName, NewName)
char *DirName, *NewName;
{
    char *s, *t, *t2, NickName[1+MAXPATHLEN], NewPref[2500];
    Boolean FoundIt = FALSE, IsMagic;

    NewPref[0] = '\0';
    BuildNickName(DirName, NickName);
    s = getprofile("messages.CrucialClasses");
    if (!s) return(0);
    while (s && *s) {
	t = index(s, ',');
	if (t) *t++='\0';
	t2 = index(s, ':');
	if (t2) {
	    *t2++ = '\0';
	    t2 = StripWhiteEnds(t2);
	}
	s = StripWhiteEnds(s);
	if (*s == '*') {
	    ++s;
	    IsMagic = TRUE;
	} else {
	    IsMagic = FALSE;
	}
	if (!strcmp(s, NickName) || !strcmp(s, DirName) || (t2 && (!strcmp(t2, NickName) || !strcmp(t2, DirName)))) {
	    FoundIt = TRUE;
	    if (NewName && *NewName) {
		char NewNick[1+MAXPATHLEN];
		if (NewPref[0]) strcat(NewPref, ",");
		if (IsMagic) strcat(NewPref, "*");
		BuildNickName(NewName, NewNick);
		strcat(NewPref, NewNick);
	    }
	} else {
	    if (NewPref[0]) strcat(NewPref, ",");
	    if (IsMagic) strcat(NewPref, "*");
	    strcat(NewPref, s);
	    if (t2) {
		strcat(NewPref, ':');
		strcat(NewPref, t2);
	    }
	}
	s = t;
    }
    if (FoundIt) {
	if (setprofilestring("messages", "crucialclasses", NewPref)) {
	    NonfatalBizarreError("Could not rewrite your CrucialClasses preference; sorry!");
	}
    }
    return(0);
}
