/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *papanote_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/papanote.c,v 2.8 1991/09/12 15:46:31 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/papanote.c,v 2.8 1991/09/12 15:46:31 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/papanote.c,v $
*/
#include <ms.h>
#include <hdrparse.h>
#include <andrewos.h> /* sys/file.h */
#include <stdio.h>

#ifndef _IBMR2
extern char *malloc();
#endif /* _IBMR2 */

extern char home[], *index();

AddParentalMessage(Msg, PDirName, bbname, bbpath)
struct MS_Message *Msg;
char *PDirName, *bbname, *bbpath;
{
    char TempFile[1+MAXPATHLEN];

    GenTempName(TempFile);
    if (WritePureFile(Msg, TempFile, TRUE, 0644)) {
	return(mserrcode);
    }
    mserrcode = AddParentalMessageFromFile(TempFile, PDirName, bbname, bbpath);
    unlink(TempFile);
    return(mserrcode);
}

AddParentalMessageFromFile(FileName, PDirName, bbname, bbpath)
char *FileName, *PDirName, *bbname, *bbpath;
{
    char NewFileName[MAXPATHLEN+1], ThisFormat[50], LineBuf[1000];
    struct MS_Message *Msg;
    int IsBE2;
    FILE *rfp, *wfp;
    Boolean InHead = TRUE, AtStart = TRUE;

    debug(1, ("Add parental message:  pdir %s bbname %s bbpath %s\n", PDirName, bbname, bbpath));
    if (FindTreeRoot(PDirName, NewFileName, FALSE)) AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_FINDTREEROOT);
    if (!strcmp(PDirName, NewFileName)) {
	debug(1, ("This is top-level, no parent note will be added\n"));
	return(0);
    }
    ReduceSlashes(bbpath);
    if ((Msg = (struct MS_Message *) malloc (sizeof (struct MS_Message))) == NULL) {
	AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_ADDPARENTAL);
    }
    bzero(Msg, sizeof(struct MS_Message));
    Msg->OpenFD = -1;
    if (ReadRawFile(FileName, Msg, FALSE)
    || ParseMessageFromRawBody(Msg)) {
	FreeMessage(Msg, TRUE);
	return(mserrcode);
    }
    GetFormatFromMessage(Msg, ThisFormat, sizeof(ThisFormat), &IsBE2) ;
    FreeMessage(Msg, TRUE);
    GenTempName(NewFileName);
    rfp = fopen(FileName, "r");
    if (!rfp) {
	AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_ADDPARENTAL);
    }
    wfp = fopen(NewFileName, "w");
    if (!wfp) {
	fclose(rfp);
	AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_ADDPARENTAL);
    }
    fprintf(wfp, "Subject: %s (new message group)\nX-Andrew-DirectoryCreation: %s\n", bbname, bbpath);
    while (fgets(LineBuf, sizeof(LineBuf), rfp)) {
	if (InHead) {
	    if (LineBuf[0] == '\n') InHead = FALSE;
	} else if (AtStart) {
	    if (!IsBE2 || ((LineBuf[0] != '\\') && (LineBuf[0] != '}'))) {
		AtStart = FALSE;
		fprintf(wfp, "\n\nA new message group, %s, has been created as a subdirectory of this message group.\n\nA copy of the first message in the new group is reproduced below.\n\nIf it interests you, you may want to subscribe to the new message group.\n\n============================================================================\n\n", bbname);
	    }
	}
	fputs(LineBuf, wfp);
    }	
    fclose(rfp);
    if (ferror(wfp) || feof(wfp)) {
	fclose(wfp);
	AMS_RETURN_ERRCODE(errno, EIN_FERROR, EVIA_ADDPARENTAL);
    }
    if (vfclose(wfp)) {
	unlink(NewFileName);
	AMS_RETURN_ERRCODE(errno, EIN_VFCLOSE, EVIA_ADDPARENTAL);
    }
    if (AppendFileToFolder(NewFileName, PDirName, TRUE)) {
	unlink(NewFileName);
	return(mserrcode);
    }
    unlink(NewFileName);
    return(0);

}

