/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *setsubs_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/setsubs.c,v 2.5 1991/09/12 15:48:27 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/setsubs.c,v 2.5 1991/09/12 15:48:27 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/setsubs.c,v $
*/
#include <andrewos.h>
#include <ms.h>

extern char home[];

MS_SetSubscriptionEntry(FullName, NickName, status)
char *FullName; /* Value passed in to MS */
char *NickName; /* Value passed in to MS */
int status; /* Ditto */
{
    struct MS_Directory *Dir;

    if (status == AMS_UNSUBSCRIBED) {
	char DefaultMailDir[1+MAXPATHLEN],
	     MailRoot[1+MAXPATHLEN];

	sprintf(MailRoot, "%s/%s", home, MS_TREEROOT);
	if (FindDefaultDir(MailRoot, DefaultMailDir)) {
	    if (AMS_ERRNO != ENOENT) {
		return(mserrcode);
	    }
	    DefaultMailDir[0] = '\0';
	}
	if (!strcmp(DefaultMailDir, FullName)) {
	    AMS_RETURN_ERRCODE(EMSFASCISTSUBSCRIPTION, EIN_PARAMCHECK, EVIA_SETSUBSENTRY);
	}
    } else if (ReadOrFindMSDir(FullName, &Dir, MD_OK)) {
	return(mserrcode);
    }
    return(SetSubsEntry(FullName, NickName, status));
}
