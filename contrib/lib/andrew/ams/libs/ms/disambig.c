/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *disambig_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/disambig.c,v 2.15 1991/09/12 15:42:55 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/disambig.c,v 2.15 1991/09/12 15:42:55 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/disambig.c,v $
*/
#include <util.h>
#include <ms.h>
#include <sys/stat.h>
#include <andrewos.h> /* sys/file.h */
#include <mail.h>

extern char *rindex();

char *SearchPath;
struct SearchPathElement SearchPathElements[MAXPATHELTS];
Boolean DidInit = FALSE;
int     MS_NumDirsInSearchPath;

/* This routine should be cleaned up to set error codes properly, and then
	the routines that call it should pass on its error codes */

MS_DisambiguateFile(source, target, AccessCode)
char *source, *target;
short AccessCode;
{
    int     i, RC;
    char   *tempname, *SlashPtr = NULL, possiblename[MAXPATHLEN + 1];
#ifdef AFS_ENV
    char *AtPtr;
#endif /* AFS_ENV */
    char *obracket, *cbracket;
    char CopyName[MAXPATHLEN+1], AtName[MAXPATHLEN+1];
    struct stat statbuf;
    struct cell_msPath *MSP;

    debug(1,("Disambiguating %s...\n", source));
    if (!DidInit) {		/* Do first-time-only initialization? */
	if ((i = InitializeSearchPaths()) != 0)
	    return(i);
    }
    strncpy(CopyName, source, sizeof(CopyName));
    AtName[0] = '\0';
#ifdef AFS_ENV
    AtPtr = rindex(CopyName, '@');
    if (AtPtr != NULL) {
	*AtPtr++ = '\0';    /* terminate the first part */
	for (tempname = AtPtr; *tempname != '\0'; ++tempname)
	  if (*tempname == '/') *tempname = '.';  /* restore dots in domain name */
	strcpy(possiblename, "/afs/");
	LCappend(possiblename, AtPtr);
	i = readlink(possiblename, AtName, sizeof(AtName));
	if (i >= 0) {
	    AtName[i] = '\0';
	    AtPtr = rindex(AtName, '/');
	    if (AtPtr != NULL) {
		strncpy(possiblename, ++AtPtr, sizeof(possiblename));
		strncpy(AtName, possiblename, sizeof(AtName));
	    }
	} else {
	    AtName[0] = '\0';
	    LCappend(AtName, AtPtr);
	}
	sprintf(possiblename, "/afs/%s", AtName);
	if (access(possiblename, F_OK) != 0) {
	    AMS_RETURN_ERRCODE(errno, EIN_ACCESS, EVIA_DISAMB);
	}
    }
#endif /* AFS_ENV */
    obracket = index(CopyName, '[');
    if (obracket) {
	cbracket = rindex(++obracket, ']');
	if (cbracket) {
	    *cbracket++ = '\0';
	    for (i=0; i< MS_NumDirsInSearchPath; ++i) {
		if (SearchPathElements[i].label && !ULstrcmp(SearchPathElements[i].label, obracket)) break;
	    }
	    if (i < MS_NumDirsInSearchPath && AtName[0] != '\0') {
		if (GetCellFromFileName(SearchPathElements[i].Path, possiblename, sizeof(possiblename)) < 0) possiblename[0] = '\0';
	    }
	    if (i < MS_NumDirsInSearchPath && (AtName[0] == '\0' || possiblename[0] == '\0' || ULstrcmp(AtName, possiblename) == 0)) {
		strncpy(possiblename, SearchPathElements[i].Path, sizeof(possiblename)-1);
		strcat(possiblename, "/");
		strncat(possiblename, cbracket, sizeof(possiblename) - strlen(possiblename));
		strncpy(CopyName, possiblename, sizeof(CopyName));
	    } else {
		if (AtName[0] != '\0') {
		    RC = CheckAMSDfMSPath(AtName, &MSP);
		    i = RC;
		    if (RC > 0) for (i = 0; i < RC; ++i) {
			if (MSP[i].Abbrev != NULL && ULstrcmp(MSP[i].Abbrev, obracket) == 0) break;
		    }
		    if (i < RC) {
			strncpy(possiblename, MSP[i].RootDir, sizeof(possiblename)-1);
			strcat(possiblename, "/");
			strncat(possiblename, cbracket, sizeof(possiblename) - strlen(possiblename));
			strncpy(CopyName, possiblename, sizeof(CopyName));
		    } else {
			--cbracket;
			*cbracket = ']';
		    }
		} else {
		    --cbracket;
		    *cbracket = ']';
		}
	    }
	}
    }
    if (CopyName[0] == '~') {
	mserrcode = ResolveTildes(CopyName, &tempname, AtName);
	if (mserrcode)
	    return(mserrcode);
	if (AccessCode == AMS_DISAMB_FILEMAYBECREATED) {
	    SlashPtr = rindex(tempname, '/');
	    if (SlashPtr) *SlashPtr = '\0';
	}
	debug(16,("Trying resolved tilde %s\n", tempname));
	errno = 0;		/* Just make sure */
	if (access(tempname, F_OK) != 0) {
	    free(tempname);
	    AMS_RETURN_ERRCODE(errno, EIN_ACCESS, EVIA_DISAMB);
	}
	if (AccessCode == AMS_DISAMB_ISAFILE || AccessCode == AMS_DISAMB_ISADIR) {
	    if (stat(tempname, &statbuf) != 0) {
		free(tempname);
		AMS_RETURN_ERRCODE(errno, EIN_ACCESS, EVIA_DISAMB);
	    }
	    if (AccessCode == AMS_DISAMB_ISAFILE) {
		if ((statbuf.st_mode & S_IFMT) != S_IFREG) {
		    free(tempname);
		    AMS_RETURN_ERRCODE(EISDIR, EIN_ACCESS, EVIA_DISAMB);
		}
	    }
	    if (AccessCode == AMS_DISAMB_ISADIR) {
		if ((statbuf.st_mode & S_IFMT) != S_IFDIR) {
		    free(tempname);
		    AMS_RETURN_ERRCODE(ENOTDIR, EIN_ACCESS, EVIA_DISAMB);
		}
	    }
	}
	if (AccessCode == AMS_DISAMB_DIREXISTS) {
	    char kidname[1+MAXPATHLEN];

	    sprintf(kidname, "%s/%s", tempname, MS_DIRNAME);
	    if (access(kidname, F_OK)) {
		free(tempname);
		AMS_RETURN_ERRCODE(errno == ENOENT ? ENOTDIR : errno, EIN_ACCESS, EVIA_DISAMB);
	    }
	}
	if (SlashPtr) *SlashPtr = '/';
	abspath(tempname, target);
				/* Depends on stable working directory */
	free(tempname);
	debug(16,("returning tilde resolved %s\n", target));
	return(0);
    }
    if (CopyName[0] == '/') {
	if (AccessCode == AMS_DISAMB_FILEMAYBECREATED) {
	    SlashPtr = rindex(CopyName, '/');
	    if (SlashPtr) *SlashPtr = '\0';
	}
	if (access(CopyName, F_OK)) {
	    AMS_RETURN_ERRCODE(errno, EIN_ACCESS, EVIA_DISAMB);
	}
	if (AccessCode == AMS_DISAMB_ISAFILE || AccessCode == AMS_DISAMB_ISADIR) {
	    if (stat(CopyName, &statbuf) != 0) {
		AMS_RETURN_ERRCODE(errno, EIN_ACCESS, EVIA_DISAMB);
	    }
	    if (AccessCode == AMS_DISAMB_ISAFILE) {
		if ((statbuf.st_mode & S_IFMT) != S_IFREG) {
		    AMS_RETURN_ERRCODE(EISDIR, EIN_ACCESS, EVIA_DISAMB);
		}
	    }
	    if (AccessCode == AMS_DISAMB_ISADIR) {
		if ((statbuf.st_mode & S_IFMT) != S_IFDIR) {
		    AMS_RETURN_ERRCODE(ENOTDIR, EIN_ACCESS, EVIA_DISAMB);
		}
	    }
	}
	if (AccessCode == AMS_DISAMB_DIREXISTS) {
	    char kidname[1+MAXPATHLEN];

	    sprintf(kidname, "%s/%s", CopyName, MS_DIRNAME);
	    if (access(kidname, F_OK)) {
		AMS_RETURN_ERRCODE(errno == ENOENT ? ENOTDIR : errno, EIN_ACCESS, EVIA_DISAMB);
	    }
	}
	if (SlashPtr) *SlashPtr = '/';
	strcpy(possiblename, CopyName);
    }
    else {
	if (AccessCode == AMS_DISAMB_FILEEXISTS) {
	    AMS_RETURN_ERRCODE(ENOENT, EIN_ACCESS, EVIA_DISAMB);
	}
	if (AccessCode == AMS_DISAMB_ISAFILE || AccessCode == AMS_DISAMB_ISADIR) {
	    if (stat(CopyName, &statbuf) != 0) {
		AMS_RETURN_ERRCODE(errno, EIN_ACCESS, EVIA_DISAMB);
	    }
	    if (AccessCode == AMS_DISAMB_ISAFILE) {
		if ((statbuf.st_mode & S_IFMT) != S_IFREG) {
		    AMS_RETURN_ERRCODE(EISDIR, EIN_ACCESS, EVIA_DISAMB);
		}
	    }
	    if (AccessCode == AMS_DISAMB_ISADIR) {
		if ((statbuf.st_mode & S_IFMT) != S_IFDIR) {
		    AMS_RETURN_ERRCODE(ENOTDIR, EIN_ACCESS, EVIA_DISAMB);
		}
	    }
	    strcpy(possiblename, CopyName);
	} else {
	    Boolean triedvalidating;
	    char *PathName, *outCell;
	    int LatestViceError = 0, LatestPermError = 0, GenIx, SawAny;

	    GenIx = 0; SawAny = FALSE;
	    while (GenMSPathElts(&GenIx, AtName, &PathName, &outCell, &i)) {
		if (AtName[0] != '\0' && outCell != NULL && outCell != ((char *) -1)) {    /* check cell match */
		    if (ULstrcmp(AtName, outCell) != 0) continue;
		}
		if (i >= 0 && !SearchPathElements[i].HasValidated) {
		    if (ValidateSearchPath(i)) return(mserrcode);
		    triedvalidating = TRUE;
		} else {
		    triedvalidating = FALSE;
		}
		if (PathName) {
		    sprintf(possiblename, "%s/%s", PathName, CopyName);
		}
		else {
		    strcpy(possiblename, CopyName);
		}
		debug(16,("Trying %s\n", possiblename));
		errno = 0;
		if (access(possiblename, F_OK) == 0) {
			if (AccessCode != AMS_DISAMB_DIREXISTS) {
			    SawAny = TRUE;
			    break;
			} else {
			    char kidname[1+MAXPATHLEN];

			    sprintf(kidname, "%s/%s", possiblename, MS_DIRNAME);
			    if (!access(kidname, F_OK)) {
				SawAny = TRUE;
				break;
			    }
			}
		}
		if (errno) {
		    if (vdown(errno)) {
			/* if it was a vice error and the validation routine did not
			    complain, we should complain here */
			LatestViceError = errno;
			if (!triedvalidating) {
			    auto char ErrorText[1000];

			    sprintf(ErrorText, "AFS/network error; can't check existence of %s", ap_Shorten(possiblename));
			    NonfatalBizarreError(ErrorText);
			}
		    } else if (errno == EACCES) {
			LatestPermError = errno;
			if (!triedvalidating) {
			    auto char ErrorText[1000];

			    sprintf(ErrorText, "Permission denied; can't check existence of %s", ap_Shorten(possiblename));
			    NonfatalBizarreError(ErrorText);
			}
		    } else if (errno != ENOENT)  {
			AMS_RETURN_ERRCODE(errno, EIN_ACCESS, EVIA_DISAMB);
		    }
		}
	    }
	    if (!SawAny) {
		AMS_RETURN_ERRCODE(LatestViceError ? LatestViceError : (LatestPermError ? LatestPermError : ENOENT), EIN_PATHSEARCH, EVIA_DISAMB);
	    }
	}
    }
    abspath(possiblename, target);
				/* Depends on stable working directory */
    debug(16,("returning %s\n", target));
    return(0);
}

