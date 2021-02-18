/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *findmbox_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/findmbox.c,v 2.14 1991/09/12 15:43:11 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/findmbox.c,v 2.14 1991/09/12 15:43:11 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/findmbox.c,v $
*/
#include <ms.h>
#include <andrewos.h> /* sys/file.h */
#include <mailconf.h>

extern char home[], *rindex(), *getprofile(), MyMailDomain[];

char *GetPersonalMailbox() {
    static char Mailbox[1+MAXPATHLEN] = "";
    char *newstr, *CheckAMSMBName();

    if (Mailbox[0] == '\0') {
	if ((newstr = getprofile("mailboxdir")) != NULL) {
	    char NewBuf[1+MAXPATHLEN], *NewPtr;
	    strcpy(NewBuf, newstr);
	    NewPtr = NULL;
	    ResolveTildes(NewBuf, &NewPtr, MyMailDomain);
	    if (NewPtr != NULL) {
		strncpy(Mailbox, NewPtr, sizeof(Mailbox));
		free(NewPtr);
	    }
	} else {
	    newstr = CheckAMSMBName(MyMailDomain);
	    if (!newstr) newstr = MailboxName;
	    sprintf(Mailbox, "%s/%s", home, newstr);
	}
    }
    return(Mailbox);
}

GetAssocMailbox(buf)
char *buf;
{/* Overwrite the given name with the Mailbox directory that should be associated with it according to cellular conventions. */
    char FileCell[200], *s, *mn, *CheckAMSMBName();

    s = rindex(buf, '/');
    if (!s) AMS_RETURN_ERRCODE(EMSNOPARENT, EIN_INDEX, EVIA_CHECKMAILBOXES);
    *s = '\0';
    FileCell[0] = '\0';
    if (GetCellFromFileName(buf, FileCell, sizeof(FileCell)) != 0) FileCell[0] = '\0';
    mn = CheckAMSMBName(FileCell[0] != '\0' ? FileCell : MyMailDomain);
    if (!mn) mn = "Mailbox";
    *s++ = '/';
    strcpy(s, mn);
    return(0);
}

TransformPathRootToMailbox(Buf)
char *Buf;
{
    char Scratch[1+MAXPATHLEN];

    strcpy(Scratch, home);
    strcat(Scratch, "/");
    strcat(Scratch, MS_TREEROOT);
    if (!strcmp(Scratch, Buf)) {
	/* It is my home .MESSAGES directory */
	strcpy(Buf, GetPersonalMailbox());
	return(0);
    }
    if (access(Buf, W_OK)) {
	AMS_RETURN_ERRCODE(errno, EIN_ACCESS, EVIA_CHECKMAILBOXES);
    }
    if (GetAssocMailbox(Buf)) return(mserrcode);
    if (access(Buf, R_OK)) {
	AMS_RETURN_ERRCODE(errno, EIN_ACCESS, EVIA_CHECKMAILBOXES);
    }
    return(0);
}

MS_FindMailbox(pathelt, Buf)
int pathelt;
char *Buf;
{
    if (MS_GetSearchPathEntry(pathelt, Buf, MAXPATHLEN)) return(mserrcode);
    if (strncmp(Buf, home, strlen(home)) && !SearchPathElements[pathelt].HasMailbox) {
	AMS_RETURN_ERRCODE(ENOENT, EIN_PARAMCHECK, EVIA_CHECKMAILBOXES);
    }
    return(TransformPathRootToMailbox(Buf));
}
