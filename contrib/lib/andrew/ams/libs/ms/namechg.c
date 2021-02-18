/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *namechg_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/namechg.c,v 2.14 1991/09/12 15:46:12 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/namechg.c,v 2.14 1991/09/12 15:46:12 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/namechg.c,v $
*/
#include <stdio.h>
#include <mailconf.h>
#include <ms.h>
#include <andrewos.h>

extern FILE *fopen();
extern char home[], *index(), *fixDate();
extern int NumSubsInUse;
extern struct SubscriptionProfile **SubsInPathOrder, *SubsInUserOrder;

/* mustopen and mustfopen

These routines are used to get around a well-understood but hard-to-fix Vice problem. 
Rename operations on Vice are not really atomic as seen from the workstation.  Thus in
certain locking algorithms, a file can temporarily disappear.  Using this instead 
of open should greatly reduce the frequency of the bugs that result.

*/

mustopen(path, flags, mode)
char *path;
int flags, mode;
{
    int numtries = 5, code;

    while (1) {
	code = open(path, flags, mode);
        if (code >= 0 || errno != ENOENT || numtries-- < 0) return(code);
    }
}


FILE *
mustfopen(fname, ftype)
char *fname, *ftype;
{
    int numtries = 5;
    FILE *fp;

    while (1) {
	fp = fopen(fname, ftype);
        if (fp || errno != ENOENT || numtries-- < 0) return(fp);
    }
}

MS_NameChangedMapFile(MapFile, MailOnly, ListAll, NumChanged, NumUnavailable, NumMissingFolders, NumSlowpokes, NumFastFellas)
char *MapFile; /* passed out */
int MailOnly; /* passed in */
int ListAll;  /* Passed in */
int *NumChanged; /* Passed out */
int *NumUnavailable; /* Passed out */
int *NumMissingFolders; /* ditto */
int *NumSlowpokes; /* ditto */
int *NumFastFellas; /* ditto */
{
    int i, mailpathelt = 0, patheltinuse = -1, unavail = 0, code = 0;
    FILE *fp = NULL, *outfp;
    char MupFileName[1+MAXPATHLEN];
    char UpdLine[100+MAXPATHLEN], *auxstuff = NULL, *s;

    GenTempName(MapFile);
    if (MakeSubsListInPathOrder()) {
	unlink(MapFile);
	return(mserrcode);
    }
    *NumChanged = 0;
    *NumUnavailable = 0;
    *NumMissingFolders = 0;
    *NumSlowpokes = 0;
    *NumFastFellas = 0;
    UpdLine[0] = '\0';
    if (MailOnly) {
	int mplen;
	char MailPath[1+MAXPATHLEN];

	sprintf(MailPath, "%s%s", home, MAILSEARCHPATHTEMPLATE);
	mplen = strlen(MailPath);
	for (i=0; i<MS_NumDirsInSearchPath; ++i) {
	    if (!strncmp(MailPath, SearchPathElements[i].Path, mplen)) {
		mailpathelt = i;
		break;
	    }
	}
    }
    for (i=0; i<NumSubsInUse; ++i) {
	debug(2048, ("Processing subs entry %s status %d\n", SubsInPathOrder[i]->key, SubsInPathOrder[i]->status));
	SubsInPathOrder[i]->HasChanged = 0;
	if (SubsInPathOrder[i]->status == AMS_UNSUBSCRIBED) continue;
	if (MailOnly && SubsInPathOrder[i]->pathelt != mailpathelt) continue;
	if (patheltinuse != SubsInPathOrder[i]->pathelt) {

	    /* Open a new master update file */
	    unavail = 0;
	    patheltinuse = SubsInPathOrder[i]->pathelt;
	    if ((patheltinuse >= 0) && (patheltinuse < MS_NumDirsInSearchPath) && !SearchPathElements[patheltinuse].HasValidated) {
		ValidateSearchPath(patheltinuse); /* ignore errors here */
	    }
	    if (fp) {
		fclose(fp);
		ViceFlushPlusParent(MupFileName);
	    }
	    if ((patheltinuse >= 0) && (patheltinuse < MS_NumDirsInSearchPath)) {
		sprintf(MupFileName, "%s/%s", SearchPathElements[patheltinuse].Path, MS_MASTERUPDATE);
		debug(2048, ("Trying to open %s\n", MupFileName));
		fp = mustfopen(MupFileName, "r");
	    } else {
		fp = NULL;
		errno = ENOENT; /* Pretend we tried the open */
	    }
	    if (!fp) {
		if (errno == EACCES || vdown(errno)) {
		    unavail = 1;
		} else if (errno != ENOENT) {
		    unlink(MapFile);
		    AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_WRITECHANGEDSUBS);
		}
		UpdLine[0] = '\0';
	    } else {
		if (fgets(UpdLine, sizeof(UpdLine), fp)) {
		    auxstuff = index(UpdLine, ' ');
		    if (auxstuff) {*auxstuff++ = '\0'; fixDate(auxstuff);}
		} else {
		    fclose(fp);
		    ViceFlushPlusParent(MupFileName);
		    fp = NULL;
		}
	    }
	}
#ifdef OBSOLETE
	if (/* SubsInPathOrder[i]->status == AMS_ASKSUBSCRIBED
	|| */ SubsInPathOrder[i]->status == AMS_SHOWALLSUBSCRIBED) {
	    SubsInPathOrder[i]->HasChanged = 1;
	    ++*NumChanged;
	    ++*NumFastFellas;
	    continue;
	}
#endif /* OBSOLETE */
	/* find out about SubsInPathOrder[i]->*/
	debug(2048, ("Going into do loop\n"));
	if (fp) do {
	    code = PreorderSubscriptionStrcmp(SubsInPathOrder[i]->key, UpdLine);
	    debug(2048, ("compared %s to %s; code is %d\n", SubsInPathOrder[i]->key, UpdLine, code));
	    if (code>0) {
		if (fgets(UpdLine, sizeof(UpdLine), fp)) {
		    auxstuff = index(UpdLine, ' ');
		    if (auxstuff) {*auxstuff++ = '\0'; fixDate(auxstuff);}
		} else {
		    fclose(fp);
		    ViceFlushPlusParent(MupFileName);
		    fp = NULL;
		    break;
		}
	    }
	} while (code > 0);
	if (!fp || code < 0) {
	    if (unavail) {
		++*NumUnavailable;
	    } else {
		int new, tot;
		char Dbuf[AMS_DATESIZE];

		debug(2048, ("Slowpoke: %s\n", SubsInPathOrder[i]->key));
		++*NumSlowpokes;
		/* LOOK UP CHANGES THE SLOW WAY HERE */
		mserrcode = MS_GetNewMessageCount(SubsInPathOrder[i]->key, &new, &tot, Dbuf, (SubsInPathOrder[i]->status == AMS_SHOWALLSUBSCRIBED) ? 1 : 0);
		/* In the previous line, we have to get the tot variable right only for show-all subscribed things. */
		if (mserrcode) {
		    if (AMS_ERRNO == ENOENT) {
			++*NumMissingFolders;
			new = 1; /* Show it to them */
		    } else if (AMS_ERRNO == EACCES || vdown(AMS_ERRNO)|| (AMS_ERRNO == EMSBADDIRFORMAT)) {
			++*NumUnavailable;
			continue;
		    } else {
			if (fp) {
			    fclose(fp);
			    ViceFlushPlusParent(MupFileName);
			}
			unlink(MapFile);
			return(mserrcode);
		    }
		}
		if (fp && AMS_ERRNO != ENOENT) {
		    char ErrorText[MAXPATHLEN+100];

		    debug(2048, ("..even though there IS a master update file!\n"));
		    sprintf(ErrorText, "%s is not in the master update file.", ap_Shorten(SubsInPathOrder[i]->key));
		    NonfatalBizarreError(ErrorText);
		}
		if ((new > 0) || (tot > 0 && (SubsInPathOrder[i]->status == AMS_SHOWALLSUBSCRIBED))) {
		    debug(2048, ("It has CHANGED!\n"));
		    SubsInPathOrder[i]->HasChanged = 1;
		    ++*NumChanged;
		}
	    }
	} else {
	    debug(2048,  ("Bingo!  key is %s mupname %s auxstuff %s\n", SubsInPathOrder[i]->key, UpdLine, auxstuff));
	    ++*NumFastFellas;
	    s = index(auxstuff, ' ');
	    if (s) *s = '\0';
	    if (strcmp(auxstuff, "000000") && ((SubsInPathOrder[i]->status == AMS_SHOWALLSUBSCRIBED) || strcmp(auxstuff, SubsInPathOrder[i]->time64) > 0)) {
		debug(2048, ("It has CHANGED!  profile says %s\n", SubsInPathOrder[i]->time64));
		SubsInPathOrder[i]->HasChanged = 1;
		++*NumChanged;
	    }
	    if (fgets(UpdLine, sizeof(UpdLine), fp)) {
		auxstuff = index(UpdLine, ' ');
		if (auxstuff) {*auxstuff++ = '\0'; fixDate(auxstuff);}
	    } else {
		fclose(fp);
		ViceFlushPlusParent(MupFileName);
		fp = NULL;
	    }
	}
    }
    if (fp) {
	fclose(fp);
	ViceFlushPlusParent(MupFileName);
    }
    outfp = fopen(MapFile, "w");
    if (!outfp) {
	AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_WRITECHANGEDSUBS);
    }
    for (i=0; i<NumSubsInUse; ++i) {
	if (MailOnly && SubsInUserOrder[i].pathelt != mailpathelt) continue;
	if (SubsInUserOrder[i].status == AMS_UNSUBSCRIBED) continue;
	if (ListAll || SubsInUserOrder[i].HasChanged || (SubsInUserOrder[i].status == AMS_SHOWALLSUBSCRIBED)) {
	    fprintf(outfp, "%s:%s %d %d\n", SubsInUserOrder[i].sname, SubsInUserOrder[i].key, SubsInUserOrder[i].status, SubsInUserOrder[i].HasChanged ? 1 : 0);
	}
    }
    if (ferror(outfp) || feof(outfp)) {
	fclose(outfp);
	AMS_RETURN_ERRCODE(errno, EIN_FERROR, EVIA_WRITECHANGEDSUBS);
    }
    if (vfclose(outfp)) {
	AMS_RETURN_ERRCODE(errno, EIN_VFCLOSE, EVIA_WRITECHANGEDSUBS);
    }
    return(0);
}

