/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *purge_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/purge.c,v 2.17 1991/09/12 15:46:53 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/purge.c,v 2.17 1991/09/12 15:46:53 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/purge.c,v $
*/
#include <ms.h>
#include <andrewos.h> /* sys/file.h */

#ifndef _IBMR2
extern char *malloc();
#endif /* _IBMR2 */

MS_PurgeDeletedMessages(dirname)
char *dirname;
{
    char SnapshotDum[AMS_SNAPSHOTSIZE], FileNameBuf[1+MAXPATHLEN],
	TmpFileName[1+MAXPATHLEN], HeadDum[AMS_DIRHEADSIZE];
    struct MS_Directory *Dir;
    int msgnum, errsave, wfd, numdeleted = 0, savefd;

    debug(1, ("MS_PurgeDeletedMessages %s\n", dirname));
    CloseDirsThatNeedIt();
    MarkQuietlyInProgress(dirname);
    if (ReadOrFindMSDir(dirname, &Dir, MD_APPEND) != 0) {
	return(mserrcode);
    }

    /* Make a first pass unlinking the messages to be deleted
     * hopefully this will make enough space to rewrite the
     * message directory.
     */
    for (msgnum = 0; msgnum < Dir->MessageCount; ++msgnum) {
	if (GetSnapshotByNumber(Dir, msgnum, SnapshotDum)) {
	  CloseMSDir(Dir, MD_APPEND);
	return(mserrcode);
	}
	if (!(AMS_GET_ATTRIBUTE(SnapshotDum, AMS_ATT_DELETED)))
	  continue;
	GetBodyFileName(Dir->UNIXDir, AMS_ID(SnapshotDum), FileNameBuf);
	unlink(FileNameBuf);
	debug(4, ("Unlinked: %s\n", FileNameBuf));
    }

    sprintf(TmpFileName, "%s/%s.p", dirname, MS_DIRNAME);
    wfd = open(TmpFileName, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (wfd < 0) {
	CloseMSDir(Dir, MD_APPEND);
	AMS_RETURN_ERRCODE(errno, EIN_OPEN, EVIA_PURGEDELETEDMESSAGES);
    }
    debug(4, ("Purging %s: reading FD is %d, writing FD is %d.\n", dirname, Dir->fd, wfd));
    if (osi_ExclusiveLockNoBlock(wfd)) { /* Hand-lock the thing */
	errsave = errno;
	CloseMSDir(Dir, MD_APPEND);
	unlink(TmpFileName);
	close(wfd);
	debug(128, ("Lock failed -- Closed dir %s\n", dirname));
	AMS_RETURN_ERRCODE(errsave, EIN_FLOCK, EVIA_PURGEDELETEDMESSAGES);
    }
    if (lseek(Dir->fd, 0, L_SET) < 0) {
	errsave = errno;
	CloseMSDir(Dir, MD_APPEND);
	unlink(TmpFileName);
	close(wfd);
	AMS_RETURN_ERRCODE(errsave, EIN_LSEEK, EVIA_PURGEDELETEDMESSAGES);
    }
    if (read(Dir->fd, HeadDum, AMS_DIRHEADSIZE) != AMS_DIRHEADSIZE) {
	errsave = errno;
	CloseMSDir(Dir, MD_APPEND);
	unlink(TmpFileName);
	close(wfd);
	AMS_RETURN_ERRCODE(errsave, EIN_LSEEK, EVIA_PURGEDELETEDMESSAGES);
    }
    if (writeall(wfd, HeadDum, AMS_DIRHEADSIZE) != AMS_DIRHEADSIZE) {
	errsave = errno;
	CloseMSDir(Dir, MD_APPEND);
	unlink(TmpFileName);
	close(wfd);
	AMS_RETURN_ERRCODE(errsave, EIN_WRITE, EVIA_PURGEDELETEDMESSAGES);
    }
    for (msgnum = 0; msgnum < Dir->MessageCount; ++msgnum) {
	if (GetSnapshotByNumber(Dir, msgnum, SnapshotDum)) {
	    unlink(TmpFileName);
	    close(wfd);
	    CloseMSDir(Dir, MD_APPEND);
	    ++numdeleted;
	    return(mserrcode);
	}
	/* This one is still good, copy it over */
	if (!(AMS_GET_ATTRIBUTE(SnapshotDum, AMS_ATT_DELETED)))
	  if (writeall(wfd, SnapshotDum, AMS_SNAPSHOTSIZE) != AMS_SNAPSHOTSIZE) {
	    errsave = errno;
	    CloseMSDir(Dir, MD_APPEND);
	    unlink(TmpFileName);
	    close(wfd);
	    AMS_RETURN_ERRCODE(errsave, EIN_WRITE, EVIA_PURGEDELETEDMESSAGES);
	  }
    }

    Dir->MessageCount -= numdeleted;
    /* Start to make the new file be the one pointed to by the Dir structure. */
    savefd = Dir->fd;
    Dir->fd = wfd;
    if (DestructivelyWriteDirectoryHead(Dir)) {
	errsave = mserrcode;
	Dir->fd = savefd;
	CloseMSDir(Dir, MD_APPEND);
	unlink(TmpFileName);
	close(wfd);
	return(errsave);
    }
    if (errsave = CloseMSDir(Dir, MD_APPEND)) {
	unlink(TmpFileName);
	(void) close(savefd);
	return (errsave);
    }
    sprintf(FileNameBuf, "%s/%s", dirname, MS_DIRNAME);
    if (RenameEvenInVice(TmpFileName, FileNameBuf)) {
	errsave = errno;
	unlink(TmpFileName);
	(void) close(savefd);
	AMS_RETURN_ERRCODE(errsave, EIN_RENAME, EVIA_PURGEDELETEDMESSAGES);
    }
    (void) close(savefd);
    return (0);
}
