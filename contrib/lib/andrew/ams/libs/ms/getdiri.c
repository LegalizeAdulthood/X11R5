/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *getdiri_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getdiri.c,v 2.8 1991/09/12 15:43:49 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getdiri.c,v 2.8 1991/09/12 15:43:49 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getdiri.c,v $
*/
#include <ms.h>
#include <andrewos.h> /* sys/file.h */
#include <mailconf.h>

extern char home[], MyMailDomain[];

long    MS_GetDirInfo (DirName, ProtCode, MsgCount)
char   *DirName;
int    *ProtCode,
       *MsgCount;
{
    struct MS_Directory *Dir;
    char BoxName[1+MAXPATHLEN], RootName[1+MAXPATHLEN];

    debug(1, ("Entering MS_GetDirInfo %s\n", DirName));
    *ProtCode = AMS_DIRPROT_AWFUL;
    *MsgCount = -42;
    /* We open in MD_READ mode to make sure the count is up to date */
    if (ReadOrFindMSDir(DirName, &Dir, MD_READ)) {
	return(mserrcode);
    }
    CloseMSDir(Dir, MD_READ);
    if (!Dir->Writable) {
	if (!strncmp(DirName, LOCALSEARCHPATHTEMPLATE, strlen(LOCALSEARCHPATHTEMPLATE))) {
	    *ProtCode = AMS_DIRPROT_LOCALBB;
	} else if (!strncmp(DirName, EXTERNALSEARCHPATHTEMPLATE, strlen(EXTERNALSEARCHPATHTEMPLATE))) {
	    *ProtCode = AMS_DIRPROT_EXTBB;
	} else if (!strncmp(DirName, OFFICIALSEARCHPATHTEMPLATE, strlen(OFFICIALSEARCHPATHTEMPLATE))) {
	    *ProtCode = AMS_DIRPROT_OFFBB;
	} else {
	    *ProtCode = AMS_DIRPROT_READ;
	}
    } else {
	if (FindTreeRoot(DirName, RootName, FALSE)) AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_FINDTREEROOT);
	strncpy(BoxName, RootName, MAXPATHLEN);
	if (TransformPathRootToMailbox(BoxName)) {
	    strncpy(BoxName, RootName, MAXPATHLEN);
	    if (GetAssocMailbox(BoxName)) return (mserrcode);
	}
	debug(4, ("Checking mailbox %s\n", BoxName));
	if (access(BoxName, R_OK)) {
	    if (errno != EACCES && errno != ENOENT) {
		AMS_RETURN_ERRCODE(errno, EIN_ACCESS, EVIA_GETDIRINFO);
	    }
	    *ProtCode = AMS_DIRPROT_MODIFY;
	} else {
	    char Tempname[1+MAXPATHLEN];

	    sprintf(Tempname, "%s/.MESSAGES/", home);
	    if (strncmp(Tempname, DirName, strlen(Tempname))) {
		*ProtCode = AMS_DIRPROT_MBOX;
	    } else {
		*ProtCode = AMS_DIRPROT_FULLMAIL;
	    }
	}
    }
    *MsgCount = Dir->MessageCount;
    return(0);
}
