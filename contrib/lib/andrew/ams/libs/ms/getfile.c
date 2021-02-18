/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *getfile_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getfile.c,v 2.6 1991/09/12 15:43:54 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getfile.c,v 2.6 1991/09/12 15:43:54 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getfile.c,v $
*/

#include <andrewos.h> /* sys/file.h */
#include <ms.h>
#include <sys/stat.h>

MS_GetPartialFile(FileName, Buf, BufLim, offset, remaining, ct)
char *FileName, *Buf;
int BufLim, offset, *remaining, *ct;
{
    int fd;
    struct stat statbuf;

    debug(1, ("MS_GetPartialFile %s\n", FileName));
    *remaining = 0;
    *ct = 0;
    if ((fd = open(FileName, O_RDONLY, 0)) < 0) {
	AMS_RETURN_ERRCODE(errno, EIN_OPEN, EVIA_GETPARTIALFILE);
    }
    if (fstat(fd, &statbuf) != 0) {
	close(fd);
	AMS_RETURN_ERRCODE(errno, EIN_FSTAT, EVIA_GETPARTIALFILE);
    }
    if (offset > 0 && lseek(fd, offset, L_SET) < 0) {
	close(fd);
	AMS_RETURN_ERRCODE(errno, EIN_LSEEK, EVIA_GETPARTIALFILE);
    }
    if ((*ct = read(fd, Buf, BufLim)) < 0) {
	close(fd);
	AMS_RETURN_ERRCODE(errno, EIN_READ, EVIA_GETPARTIALFILE);
    }
    *remaining = statbuf.st_size -offset - *ct;
    if (*ct < BufLim) Buf[*ct] = '\0';  /* Be nice to clients */
    close(fd);
    return(0);
}
