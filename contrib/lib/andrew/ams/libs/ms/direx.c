/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *direx_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/direx.c,v 2.15 1991/09/12 15:42:51 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/direx.c,v 2.15 1991/09/12 15:42:51 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/direx.c,v $
*/
#include <andrewos.h>
#include <ms.h>
#include <mailconf.h>
#ifdef AFS_ENV
#include <afs/param.h>
#include <sys/ioctl.h>
#include <afs/venus.h>
#include <afs/prs_fs.h>
#endif /* AFS_ENV */

extern char home[], Me[], *GetPersonalMailbox();

static void AddWelcomeMail(mdir)
char *mdir;
{
    char ErrorText[256];

    if (!WelcomeMailFile || !*WelcomeMailFile) return;
    if (AppendFileToFolder(WelcomeMailFile, mdir, FALSE)) {
	if (errno == ENOENT) {
	    strcpy(ErrorText, "The 'Welcome to Andrew' mail file is missing, so you'll get no 'welcome' mail.");
	} else {
	    sprintf(ErrorText, "Could not give you the 'Welcome to Andrew' mail (%d, %d, %d)", AMS_ERRNO, AMS_ERRCAUSE, AMS_ERRVIA);
	}
	NonfatalBizarreError(ErrorText);
    }
    return;
}

int EnsureMailDirExists() {
    char MailDirPath[1+MAXPATHLEN], MailDir[1+MAXPATHLEN], FlamesFile[1+MAXPATHLEN], *Mailbox;
    struct MS_Directory *Dir;
    Boolean Created = FALSE;

    strcpy(MailDirPath, home);
    strcat(MailDirPath, "/.MESSAGES");
    if (access(MailDirPath, F_OK)) {
	if (vdown(errno)) {
	    NonfatalBizarreError("AFS/network down; cannot check the existence of your mail directory; hoping for the best...");
	    AMS_RETURN_ERRCODE(ETIMEDOUT, EIN_ACCESS, EVIA_CONVERTOLD);
	} else if (errno ==  EACCES) {
	    NonfatalBizarreError("Protection error checking your own mail directory; are you authenticated?");
	    AMS_RETURN_ERRCODE(EACCES, EIN_ACCESS, EVIA_CONVERTOLD);
	} else {
	    if (mkdir(MailDirPath, 0700)) {
		NonfatalBizarreError("Cannot create your mail directory!");
		AMS_RETURN_ERRCODE(errno, EIN_MKDIR, EVIA_CONVERTOLD);
	    }
	    NonfatalBizarreError("Creating your mail directories; please wait...");
	    Created = TRUE;
	    if (EnsurePrivacy(MailDirPath, FALSE)) {
		return(mserrcode);
	    }
	}
    }
    strcpy(FlamesFile, home);
    strcat(FlamesFile, "/.AMS.flames");
    if (Created || access(FlamesFile, F_OK)) {
	if (!Created && vdown(errno)) {
	    NonfatalBizarreError("AFS/network down; cannot check the existence of your mail directory; hoping for the best...");
	    AMS_RETURN_ERRCODE(ETIMEDOUT, EIN_ACCESS, EVIA_CONVERTOLD);
	} else if (!Created && (errno ==  EACCES)) {
	    NonfatalBizarreError("Protection error checking for your mail directory; are you authenticated?");
	    AMS_RETURN_ERRCODE(EACCES, EIN_ACCESS, EVIA_CONVERTOLD);
	} else {
	    /* Need to create this guy a default mail directory */
	    char Nick[1+MAXPATHLEN]; int substat;

	    if (FindDefaultDir(MailDirPath, MailDir)) {
		if (AMS_ERRNO != ENOENT) return(mserrcode);
		sprintf(MailDir, "%s/%s", MailDirPath, AMS_DEFAULTMAILDIR);
		if (ReadOrFindMSDir(MailDirPath, &Dir, MD_OK)) {
		    if (!Created) {
			NonfatalBizarreError("Creating your mail directories; please wait...");
		    }
		    Created = TRUE;
		    if (MS_CreateNewMessageDirectory(MailDir, 0, MailDir)) {
			return(mserrcode);
		    }
		    BuildNickName(MailDir, Nick);
		    AddWelcomeMail(MailDir);
		    if (MS_SetSubscriptionEntry(MailDir, Nick, AMS_ALWAYSSUBSCRIBED)
			|| MS_FastUpdateState()) {
			return(mserrcode);
		    }
		}
	    } else {
		if (GetSubsEntry(MailDir, Nick, &substat)) return(mserrcode);
		if (substat == AMS_UNSUBSCRIBED) {
		    if (MS_SetSubscriptionEntry(MailDir, Nick, AMS_ALWAYSSUBSCRIBED)) return(mserrcode);
		}
	    }
	}
    }
    Mailbox = GetPersonalMailbox();
    if (access(Mailbox, R_OK)) {
	if (vdown(errno)) {
	    NonfatalBizarreError("AFS/network down; cannot check the existence of your Mailbox; hoping for the best...");
	    AMS_RETURN_ERRCODE(ETIMEDOUT, EIN_ACCESS, EVIA_CONVERTOLD);
	} else if (errno ==  EACCES) {
	    NonfatalBizarreError("Protection error in checking your own Mailbox; are you authenticated?");
	    AMS_RETURN_ERRCODE(EACCES, EIN_ACCESS, EVIA_CONVERTOLD);
	} else {
	    if (mkdir(Mailbox, 0700)) {
		NonfatalBizarreError("Cannot create your mail directory!");
		AMS_RETURN_ERRCODE(errno, EIN_MKDIR, EVIA_CONVERTOLD);
	    }
	    if (!Created) {
		NonfatalBizarreError("Creating your mail directories; please wait...");
	    }
	    Created = TRUE;
	    if (EnsurePrivacy(Mailbox, TRUE)) {
		return(mserrcode);
	    }
	}
    }
    if (Created) {
	NonfatalBizarreError("Mail directories created.  Welcome to Andrew!");
    }
    return(0);
}

EnsurePrivacy(DirName, SemiPrivate)
char *DirName;
Boolean SemiPrivate;
{
#ifdef AFS_ENV
    struct ViceIoctl blob;
    char space[2000], myprotstuff[1000];
#endif /* AFS_ENV */

    if (chmod(DirName, 0700)) {
	AMS_RETURN_ERRCODE(errno, EIN_CHMOD, EVIA_CONVERTOLD);
    }
#ifdef AFS_ENV
    if (AMS_ViceIsRunning) {
        if (SemiPrivate) {
	    sprintf(myprotstuff, "2\n0\n%s %d\nSystem:AnyUser %d\n", Me, PRSFS_READ | PRSFS_LOOKUP | PRSFS_INSERT | PRSFS_DELETE | PRSFS_WRITE | PRSFS_LOCK | PRSFS_ADMINISTER, PRSFS_LOOKUP | PRSFS_INSERT | PRSFS_LOCK);
	} else {
	    sprintf(myprotstuff, "1\n0\n%s %d\n", Me, PRSFS_READ | PRSFS_LOOKUP | PRSFS_INSERT | PRSFS_DELETE | PRSFS_WRITE | PRSFS_LOCK | PRSFS_ADMINISTER);
	}
	blob.in = myprotstuff;
	blob.in_size = 1+strlen(myprotstuff);
	blob.out = space;
	blob.out_size = sizeof space;
	if (pioctl(DirName, _VICEIOCTL(1), &blob, 1)) {
	    AMS_RETURN_ERRCODE(errno, EIN_PIOCTL, EVIA_CONVERTOLD);
	}
    }
#endif /* AFS_ENV */
    return(0);
}
