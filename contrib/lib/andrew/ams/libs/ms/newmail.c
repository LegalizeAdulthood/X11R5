/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/newmail.c,v 2.16 1991/09/12 15:46:20 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/newmail.c,v $
*/
/*
        newmail.c -- Routines for accessing incoming mail files in the raw.
*/

char            ams_newmail_rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/newmail.c,v 2.16 1991/09/12 15:46:20 bobg Exp $";

#include <andyenv.h>
#include <ms.h>
#include <hdrparse.h>
#include <andrewos.h>

#ifndef _IBMR2
extern char    *malloc();
#endif /* _IBMR2 */

extern char    *rindex(), home[];

extern struct MS_Directory *MyOpenDir;

/* As of 23-May, "Code" is only ever "PROCESSNEW_MBOX" */
/* Note: All of the following code is highly bogus.
  * It supports SPEC, which no longer exists, and the
  * old-style recon, which no longer exists.
  * Code, NumDirInsertions and IsReconstruction are all superfluous.
  */

ProcessNewMail(ThisFileName, ParseSpec, Code, UnlinkFailures, EliErrBuf, EliErrBufLim)
char           *ThisFileName, *ParseSpec, *EliErrBuf;
int             Code, EliErrBufLim;
int            *UnlinkFailures;
{
#if 0
    char            DirList[MAXDIRINSERTIONS][MAXPATHLEN+1], NewFileName[MAXPATHLEN+1];
    struct MS_CaptionTemplate CaptionTemplateList[MAXDIRINSERTIONS];
    struct MS_Directory *MSDirList[MAXDIRINSERTIONS];
    int             dcode, errsave, i;
#endif /* 0 */
    char *s;
    Boolean         IsReconstruction;
    int             NumDirInsertions = 0;
    struct MS_Message *Msg;

    if (Code == PROCESSNEW_MBOX) {
	IsReconstruction = FALSE;
    }
    else {
	IsReconstruction = TRUE;
    }
    Msg = (struct MS_Message *) malloc(sizeof(struct MS_Message));
    if (Msg == NULL) {
	AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_PROCNEWMSGS);
    }
    bzero(Msg, sizeof(struct MS_Message));
    Msg->OpenFD = -1;
#if 0
    switch (Code) {
	case PROCESSNEW_FASTRECON:
	    dcode = DATETYPE_FROMFILE;
	    break;
	case PROCESSNEW_SLOWRECON:
	    dcode = DATETYPE_FROMHEADER;
	    break;
	case PROCESSNEW_MBOX:
	default:
	    dcode = DATETYPE_CURRENT;
	    break;
    }
#endif                                 /* 0 */
    if (ReadRawFile(ThisFileName, Msg, TRUE)
	 || ParseMessageFromRawBody(Msg)
	 || CheckAuthUid(Msg)
	 || (InventID(Msg)
	     || Flames_HandleNewMessage(Msg, ParseSpec,
					&NumDirInsertions,
					(strncmp(ThisFileName, home, strlen(home))) ? FALSE : TRUE,
					ThisFileName,
					EliErrBuf,
					EliErrBufLim))) {        /* Whew! -- bobg */
	FreeMessage(Msg, TRUE);
	return (mserrcode);
    }
    if (!IsReconstruction) {
	AMS_SET_ATTRIBUTE(Msg->Snapshot, AMS_ATT_UNSEEN);
    }
    if (IsReconstruction) {
	s = rindex(ThisFileName, '/');
	if (s && *++s == '+' && strlen(++s) < AMS_IDSIZE) {
	    strcpy(AMS_ID(Msg->Snapshot), s);
	}
    }

#if 0                       /* NOTE: Spec does *not* live */

    /*
      * Next we check and see if we can open all these suckers before we
	  * actually use any of them -- this is to try to avoid multiple passes
	  * succeeding on, say, the first bb and failing (to retry again) on the
	    * second
	    */

    for (i = 0; i < NumDirInsertions; ++i) {
	if (ReadOrFindMSDir(DirList[i], &MSDirList[i], MD_APPEND)) {
	    errsave = mserrcode;
	    FreeMessage(Msg, TRUE);
	    while (--i >= 0) {
		if (MSDirList[i] != MyOpenDir)
		    CloseMSDir(MSDirList[i], MD_APPEND);
	    }
	    return (errsave);
	}
    }

    /*
      * This next line is sub-optimal; we could make do with a rename the first
	    * time.  However, it is just too much work, and too rarely useful
	    */
    if (NumDirInsertions > 1)
	Msg->WeFiddled = TRUE;
    debug(4, ("Read and parsed message from file %s\n", ThisFileName));
    for (i = 0; i < NumDirInsertions; ++i) {
	if (MSDirList[i]->OpenMode < MD_APPEND) {
	    if (CheckOpenMSDirectory(MSDirList[i], MD_APPEND)) {
		errsave = mserrcode;
		if (MSDirList[i] != MyOpenDir)
		    CloseMSDir(MSDirList[i], MD_APPEND);
		FreeMessage(Msg, TRUE);
		while (++i < NumDirInsertions) {
		    if (MSDirList[i] != MyOpenDir)
			CloseMSDir(MSDirList[i], MD_APPEND);
		}
		return (errsave);
	    }
	}
	debug(4, ("Trying to add message to directory %s\n", DirList[i]));

	CaptionTemplateList[i].basictype = BASICTEMPLATE_NORMAL;
	if (Code == PROCESSNEW_FASTRECON) {
	    CaptionTemplateList[i].datetype = DATETYPE_FROMFILE;
	} else {
	    CaptionTemplateList[i].datetype = DATETYPE_FROMHEADER;
	}

	if (BuildCaption(Msg, &(CaptionTemplateList[i]),
			 (strncmp(MSDirList[i]->UNIXDir, home, strlen(home))) ? FALSE : TRUE)) {
	    errsave = mserrcode;
	    if (MSDirList[i] != MyOpenDir)
		CloseMSDir(MSDirList[i], MD_APPEND);
	    FreeMessage(Msg, TRUE);
	    while (++i < NumDirInsertions) {
		if (MSDirList[i] != MyOpenDir)
		    CloseMSDir(MSDirList[i], MD_APPEND);
	    }
	    return (errsave);
	}
	debug(4, ("Got caption template and built caption\n"));
	sprintf(NewFileName, "%s/+%s", MSDirList[i]->UNIXDir, AMS_ID(Msg->Snapshot));
	if (IsReconstruction || !(Msg->WeFiddled)) {
	    debug(4, ("No need to write the file %s\n", NewFileName));
	    if (NeedsNewID) {
		char            ErrorText[(2 * MAXPATHLEN) + 100];

		debug(4, ("Renaming %s to %s\n", ThisFileName, NewFileName));
		if (RenameEvenInVice(ThisFileName, NewFileName)) {
		    if (IsReconstruction) {
			errsave = errno;
			if (MSDirList[i] != MyOpenDir)
			    CloseMSDir(MSDirList[i], MD_APPEND);
			FreeMessage(Msg, TRUE);
			while (++i < NumDirInsertions) {
			    if (MSDirList[i] != MyOpenDir)
				CloseMSDir(MSDirList[i], MD_APPEND);
			}
			AMS_RETURN_ERRCODE(errsave, EIN_RENAME, EVIA_PROCNEWMSGS);
		    }
		    else {
			switch (errno) {
			    case EXDEV:
				/* A volume problem (cross-device link) */
				/* We drop through to next major if clause */
				Msg->WeFiddled = TRUE;
				break;
			    default:
				/* A real error */
				errsave = errno;
				sprintf(ErrorText, "Rename failed: error %d (%s) renaming %s to ", errno, UnixError(errno), ap_Shorten(ThisFileName));
				strcat(ErrorText, ap_Shorten(NewFileName));
				NonfatalBizarreError(ErrorText);
				if (MSDirList[i] != MyOpenDir)
				    CloseMSDir(MSDirList[i], MD_APPEND);
				FreeMessage(Msg, TRUE);
				while (++i < NumDirInsertions) {
				    if (MSDirList[i] != MyOpenDir)
					CloseMSDir(MSDirList[i], MD_APPEND);
				}
				AMS_RETURN_ERRCODE(errsave, EIN_RENAME, EVIA_PROCNEWMSGS);
			}
		    }
		}
	    }
	}

	/*
	 * The next is NOT an else clause because of the side effects on
	       * WeFiddled
	       */
	if (!IsReconstruction && Msg->WeFiddled) {
	    debug(4, ("Writing pure new file %s\n", NewFileName));
	    /* In the following, 644 should not be hardwired */
	    if (WritePureFile(Msg, NewFileName, FALSE, 0644)) {
		errsave = mserrcode;
		if (MSDirList[i] != MyOpenDir)
		    CloseMSDir(MSDirList[i], MD_APPEND);
		FreeMessage(Msg, TRUE);
		while (++i < NumDirInsertions) {
		    if (MSDirList[i] != MyOpenDir)
			CloseMSDir(MSDirList[i], MD_APPEND);
		}
		return (errsave);
	    }
	}
	/* Here we treat EMSALREADYTHERE as a success code because it is one. */

	/*
	 * Also, if it was already there, we set WeFiddled to TRUE and then
	     * skip this block -- bobg
	     */

	/*
	 * NOTE: This section of code was not modified to reflect changes to
	 * AppendMessageToMSDir, since all of this stuff is obsolete!  (bobg,
	 * 23-May-89)  [The changes in question involve the removal of
	 * "IsMessageAlreadyThere" from "AppendMessageToMSDir"]
	 */

	if (AppendMessageToMSDir(Msg, MSDirList[i]) && ((AMS_ERRNO != EMSALREADYTHERE) || (Msg->WeFiddled = TRUE, FALSE))) {
	    errsave = mserrcode;
	    if (!IsReconstruction) {
		if (Msg->WeFiddled) {
		    unlink(NewFileName);

		    /*
		     * If this does not work, an orphan file will remain until
		     * the dir is purged. This is a reasonably likely
		     * occurrence if Vice crashes at just the right moment
			 */
		    debug(4, ("Unlinked %s\n", NewFileName));
		}
		else {
		    RenameEvenInVice(NewFileName, ThisFileName);
		    debug(4, ("Renamed %s to %s\n", NewFileName, ThisFileName));
		}
	    }
	    if (MSDirList[i] != MyOpenDir)
		CloseMSDir(MSDirList[i], MD_APPEND);
	    FreeMessage(Msg, TRUE);
	    while (++i < NumDirInsertions) {
		if (MSDirList[i] != MyOpenDir)
		    CloseMSDir(MSDirList[i], MD_APPEND);
	    }
	    return (errsave);
	}
	if (!IsReconstruction) {
	    if (PlanToCloseDir(MSDirList[i], Msg->OpenFD, ThisFileName, NewFileName, Msg->WeFiddled, UnlinkFailures, (i == (NumDirInsertions - 1)))) {
		errsave = mserrcode;
		FreeMessage(Msg, TRUE);
		while (++i < NumDirInsertions) {
		    if (MSDirList[i] != MyOpenDir)
			CloseMSDir(MSDirList[i], MD_APPEND);
		}
		return (mserrcode);
	    }
	    if (i == (NumDirInsertions - 1)) {
		Msg->OpenFD = -1;      /* Prevent FreeMessage from closing
					* this */
	    }
	}
    }

#endif /* 0 */

    FreeMessage(Msg, TRUE);
    if (NumDirInsertions == 0) {
	debug(4, ("FLAMES seems to have satisfactorily handled this file, I'll unlink %s\n", ThisFileName));
	unlink(ThisFileName);          /* ignore errors -- might have been
					* renamed out of the way */
    }
    return (0);
}
