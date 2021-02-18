/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *ckmail_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/ckmail.c,v 2.6 1991/09/12 15:42:09 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/ckmail.c,v 2.6 1991/09/12 15:42:09 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/ckmail.c,v $
*/
#include <ms.h>
#ifdef M_UNIX
#include <dirent.h>
#define direct dirent
#else
#include <sys/dir.h>
#endif

extern char home[], *GetPersonalMailbox();

MS_DoIHaveMail(count)
int *count;
{
    DIR *dirp;
    struct direct *dirent;
    char *Mailbox;

    *count = 0;
    Mailbox = GetPersonalMailbox();
    if ((dirp = opendir(Mailbox)) == NULL) {
	AMS_RETURN_ERRCODE(errno, EIN_OPENDIR, EVIA_DOIHAVEMAIL);
    }
    while ((dirent = readdir(dirp)) != NULL) {
	if (dirent->d_name[0] != '.') {
	    ++*count;
	}
    }
    closedir(dirp);
    return(0);
}
