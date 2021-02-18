/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *enssubs_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/enssubs.c,v 2.7 1991/09/12 15:42:59 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/enssubs.c,v 2.7 1991/09/12 15:42:59 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/enssubs.c,v $
*/
#include <andrewos.h>
#include <stdio.h>
#include <ms.h>

extern char *index();
extern FILE *fopen();

EnsureInSubscriptionMap(DirName)
char *DirName;
{
    return(EnsureSubMapCorrect(DirName, TRUE));
}

EnsureNotInSubscriptionMap(DirName)
char *DirName;
{
    return(EnsureSubMapCorrect(DirName, FALSE));
}

EnsureSubMapCorrect(DirName, DoesExist)
char *DirName;
Boolean DoesExist;
{

    FILE * rfp, *wfp;
    Boolean WroteIt;
    int     code;
    char    MapFileName[1 + MAXPATHLEN],
            TempMapFileName[1 + MAXPATHLEN],
	    MyLine[2 * MAXPATHLEN],
           *s, *t,
            NickName[1 + MAXPATHLEN],
            LineBuf[2 * MAXPATHLEN];

    debug(1,("EnsureSubMapCorrect %s %d\n", DirName, DoesExist));
    strcpy(MapFileName, DirName);
    if (FindTreeRoot(DirName, MapFileName, FALSE)) AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_FINDTREEROOT);
    strcat(MapFileName, "/");
    strcat(MapFileName, AMS_SUBSCRIPTIONMAPFILE);
    BuildNickName(DirName, NickName);
    debug(16,("Map file name is %s, nickname %s\n", MapFileName, NickName));
    if (!CheckSubMapCorrect(DirName, MapFileName, NickName, DoesExist)) return(0);
    strcpy(TempMapFileName, MapFileName);
    strcat(TempMapFileName, ".NEW");
    wfp = fopen(TempMapFileName, "w");
    if (!wfp) {
	AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_ENSUREINSUBS);
    }
    rfp = fopen(MapFileName, "r");
    sprintf(MyLine, "%s:%s\n", NickName, DirName);
    if (!rfp) {
	if (errno != ENOENT) {
	    fclose(wfp);
	    AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_ENSUREINSUBS);
	}
	debug(16, ("First entry in subscription map file.\n"));
	fputs(MyLine, wfp);
	if (ferror(wfp) || feof(wfp)) {
	    fclose(wfp);
	    AMS_RETURN_ERRCODE(errno, EIN_FERROR, EVIA_ENSUREINSUBS);
	}
    }
    else {
	WroteIt = !DoesExist;
	while (fgets(LineBuf, sizeof(LineBuf), rfp) != NULL) {
	    if (BadSubMapLine(LineBuf)) continue;
	    if (!WroteIt) {
		s = index(MyLine, ':');
		t = index(LineBuf, ':');
		if (!s || !t) {
		    fclose(rfp);
		    fclose(wfp);
		    AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_ENSUREINSUBS);
		}
		*s = *t = '\0';
		code = PreorderSubscriptionStrcmp(LineBuf, MyLine);
		*s = *t = ':';
		if (code >= 0) {
		    if (code > 0) fputs(MyLine, wfp);
		    WroteIt = TRUE;
		}
	    }
	    if (DoesExist || strcmp(LineBuf, MyLine)) {
		fputs(LineBuf, wfp);
	    }
	}
	if (!WroteIt) {
	    fputs(MyLine, wfp);
	}
	if (ferror(rfp) || ferror(wfp) || feof(wfp)) {
	    fclose(rfp);
	    fclose(wfp);
	    AMS_RETURN_ERRCODE(errno, EIN_FERROR, EVIA_ENSUREINSUBS);
	}
	fclose(rfp);
    }
    if (vfclose(wfp)) {
	AMS_RETURN_ERRCODE(errno, EIN_VFCLOSE, EVIA_ENSUREINSUBS);
    }
    if (RenameEvenInVice(TempMapFileName, MapFileName)) {
	AMS_RETURN_ERRCODE(errno, EIN_RENAME, EVIA_ENSUREINSUBS);
    }
    return(0);
}

CheckSubMapCorrect(DirName, MapFileName, NickName, DoesExist)
char *DirName, *MapFileName, *NickName;
Boolean DoesExist;
{
    FILE * rfp;
    Boolean Answer = DoesExist ? FALSE : TRUE;
    char    MyLine[2 * MAXPATHLEN],
            LineBuf[2 * MAXPATHLEN];

    debug(1,("CheckSubMapCorrect %s %d\n", DirName, DoesExist));
    sprintf(MyLine, "%s:%s\n", NickName, DirName);
    rfp = fopen(MapFileName, "r");
    if (rfp) {
	while (fgets(LineBuf, sizeof(LineBuf), rfp) != NULL) {
	    if (!strcmp(MyLine, LineBuf)) {
		Answer = DoesExist ? TRUE : FALSE;
		break;
	    }
	}
	fclose(rfp);
    }
    return(Answer ? 0 : 1);
}
