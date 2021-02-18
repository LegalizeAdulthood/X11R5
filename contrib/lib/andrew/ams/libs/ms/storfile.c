/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *storfile_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/storfile.c,v 2.5 1991/09/12 15:48:47 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/storfile.c,v 2.5 1991/09/12 15:48:47 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/storfile.c,v $
*/
#include <stdio.h>
#include <ms.h>
#include <andrewos.h> /* sys/file.h */

MS_StorePartialFile(FileName, startpos, len, mode, Truncate, WhatToStore)
char *FileName, *WhatToStore;
int startpos, len, mode, Truncate;
{
    int fd, errsave;

    debug(1, ("MS_StorePartialFile %s pos %d len %d mode %d\n", FileName, startpos, len, mode));
    fd = open(FileName, O_RDWR | O_CREAT, mode);
    if (fd<0) {
	AMS_RETURN_ERRCODE(errno, EIN_OPEN, EVIA_STOREPARTIALFILE);
    }
    if (startpos>0 && lseek(fd, startpos, L_SET) < 0) {
	errsave = errno;
	vclose(fd);
	if ((startpos == 0) && Truncate) unlink(FileName);
	AMS_RETURN_ERRCODE(errsave, EIN_LSEEK, EVIA_STOREPARTIALFILE);
    }
    if (writeall(fd, WhatToStore, len) < 0) {
	errsave = errno;
	vclose(fd); /* BOGUS -- should somehow ABORT the Vice store */
	if ((startpos == 0) && Truncate) unlink(FileName);
	AMS_RETURN_ERRCODE(errsave, EIN_WRITE, EVIA_STOREPARTIALFILE);
    }
    if (Truncate && ftruncate(fd, startpos + len)) {
	errsave = errno;
	vclose(fd); /* BOGUS -- should somehow ABORT the Vice store */
	if ((startpos == 0) && Truncate) unlink(FileName);
	AMS_RETURN_ERRCODE(errsave, EIN_FTRUNCATE, EVIA_STOREPARTIALFILE);
    }
    if (vclose(fd)) {
	errsave = errno;
	if ((startpos == 0) && Truncate) unlink(FileName);
	AMS_RETURN_ERRCODE(errsave, EIN_VCLOSE, EVIA_STOREPARTIALFILE);
    }
    return(0);
}
