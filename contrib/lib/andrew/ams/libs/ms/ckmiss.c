/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *ckmiss_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/ckmiss.c,v 2.5 1991/09/12 15:42:13 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/ckmiss.c,v 2.5 1991/09/12 15:42:13 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/ckmiss.c,v $
*/
#include <andrewos.h>
#include <stdio.h>
#include <mailconf.h>
#include <ms.h>
#include <sys/stat.h>

extern char *index();
extern FILE *fopen();

/* This routine checks to see if a nonexistent folder has been replaced */

MS_CheckMissingFolder(OldName, NewName)
char *OldName; /* IN */
char *NewName; /* OUT */
{
    FILE *fp;
    struct stat statbuf;
    char LineBuf[500], *s, *t;
    int len;

    NewName[0] = '\0';
    len = strlen(OldName);
    if (len<=0) {
	AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_CHECKMISSINGFOLDER);
    }
    fp = fopen(GlobalChangeSubsFile, "r");
    if (!fp) {
	if (errno == ENOENT) return(0); /* NO problem... */
	AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_CHECKMISSINGFOLDER);
    }
    if (fstat(fileno(fp), &statbuf)) {
	fclose(fp);
	AMS_RETURN_ERRCODE(errno, EIN_STAT, EVIA_CHECKMISSINGFOLDER);
    }
    while (fgets(LineBuf,sizeof(LineBuf) -1, fp) != NULL) {
	if (LineBuf[len] == ' ' && !strncmp(LineBuf, OldName, len)) {
	    /* Found it!  Hoo-hah! */
	    s = LineBuf + len + 1;
	    t = index(s, ' ');
	    if (t) *t = '\0'; /* rest of line is comment */
	    t = index(s, '\t');
	    if (t) *t = '\0'; /* rest of line is comment */
	    t = index(s, '\n');
	    if (t) *t = '\0'; /* rest of line is comment */
	    strncpy(NewName, s, MAXPATHLEN);
	    fclose(fp);
	    return(0);
	}
    }
    fclose(fp);
    return(0);
}
