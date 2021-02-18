/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *altsnap_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/altsnap.c,v 2.5 1991/09/12 15:41:16 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/altsnap.c,v 2.5 1991/09/12 15:41:16 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/altsnap.c,v $
*/
#include <stdio.h>
#include <andrewos.h>
#include <ms.h>

MS_AlterSnapshot(dirname, id, NewSnapshot, Code) 
char *dirname, *id, *NewSnapshot;
int Code;
{
    struct MS_Directory *Dir;
    int msgnum, errsave, i;
    char SnapshotDum[AMS_SNAPSHOTSIZE], *s, *t, DateBuf[AMS_DATESIZE];

    if (MSDebugging & 1) { /* Debugging SHOULD go to stdout -- nsb 5/16/86 */
	printf("MS_AlterSnapshot %s %s code %d\n", dirname, id, Code);
	fputs("Snapshot: 0x", stdout);
	for (i=0; i<AMS_SNAPSHOTSIZE; i++) fprintf(stdout, "%02x", (unsigned char) NewSnapshot[i]);
	fputs(")\n", stdout);
    }

    if (ReadOrFindMSDir(dirname, &Dir, MD_WRITE) != 0) {
	return(mserrcode);
    }
    if (GetSnapshotByID(Dir, id, &msgnum, SnapshotDum)) {
	errsave = mserrcode; 
	CloseMSDir(Dir, MD_WRITE);
	return(errsave);
    }
    switch(Code) {
	case ASS_REPLACE_ALL:
	    strcpy(DateBuf, AMS_DATE(SnapshotDum));
	    bcopy(NewSnapshot, SnapshotDum, AMS_SNAPSHOTSIZE);
	    strcpy(AMS_DATE(SnapshotDum), DateBuf);
	    break;
	case ASS_REPLACE_ATT_CAPT:
	    bcopy(AMS_CAPTION(NewSnapshot), AMS_CAPTION(SnapshotDum), AMS_CAPTIONSIZE);
	    /* DROP THROUGH TO NEXT CLAUSE */
	case ASS_REPLACE_ATTRIBUTES:
	    bcopy(AMS_ATTRIBUTES(NewSnapshot), AMS_ATTRIBUTES(SnapshotDum), AMS_ATTRIBUTESIZE);
	    break;
	case ASS_OR_ATTRIBUTES:
	    for (i=0, s=AMS_ATTRIBUTES(NewSnapshot), t = AMS_ATTRIBUTES(SnapshotDum); i<AMS_ATTRIBUTESIZE; ++i, ++s, ++t) {
		*t |= *s;
	    }
	    break;
	case ASS_AND_ATTRIBUTES:
	    for (i=0, s=AMS_ATTRIBUTES(NewSnapshot), t = AMS_ATTRIBUTES(SnapshotDum); i<AMS_ATTRIBUTESIZE; ++i, ++s, ++t) {
		*t &= *s;
	    }
	    break;
    }
    debug(4, ("Altering snapshot of  message %d %s\n", msgnum, AMS_CAPTION(SnapshotDum)));
    if (RewriteSnapshotInDirectory(Dir, msgnum, SnapshotDum)) {
	errsave = mserrcode;
	CloseMSDir(Dir, MD_WRITE);
	return(errsave);
    }
    if (CacheDirectoryForClosing(Dir, MD_WRITE)) {
	return(mserrcode);
    }
    return(0);
}
