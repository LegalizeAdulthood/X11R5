/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *instinfo_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/instinfo.c,v 2.8 1991/09/12 15:45:04 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/instinfo.c,v 2.8 1991/09/12 15:45:04 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/instinfo.c,v $
*/
#include <ms.h>
#include <andrewos.h> /* sys/file.h */
#include <mailconf.h>
#include <hdrparse.h>
#include <stdio.h>

#ifndef _IBMR2
extern char *malloc();
#endif /* _IBMR2 */

extern char Me[], *MyPrettyAddress, home[];

MS_InstallWelcomeMessage(ParentName, InitDir, InitFile, ShortName)
char *ParentName, *InitDir, *InitFile, *ShortName;
{
    char DirName[MAXPATHLEN+1], SourceFileName[1+MAXPATHLEN], LineBuf[1000];
    FILE *rfp, *wfp;
    Boolean AtStart = TRUE;

    debug(1, ("MS_InstallWelcome Message %s %s %s\n", ParentName, InitFile, ShortName));
    sprintf(DirName, "%s/%s", ParentName, ShortName);
    if (InitDir && *InitDir) {
	sprintf(SourceFileName, "%s/+%s", InitDir, InitFile);
	if (AppendFileToFolder(SourceFileName, DirName, FALSE)) {
	    return(mserrcode);
	}
    } else {
	GenTempName(SourceFileName);
	rfp = fopen(InitFile, "r");
	if (!rfp) {
	    AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_ADDPARENTAL);
	}
	wfp = fopen(SourceFileName, "w");
	if (!wfp) {
	    fclose(rfp);
	    AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_ADDPARENTAL);
	}
	fprintf(wfp, "Date: %sFrom:%s\nSubject: Welcome to %s\n", arpadate(), MyPrettyAddress, ShortName);
	while (fgets(LineBuf, sizeof(LineBuf), rfp)) {
	    if (AtStart) {
		if (LineBuf[0] == '\\') {
		    /* Bogus hardwiring of format 12 */
		    fprintf(wfp, "Content-type: X-BE2; 12\n\n");
		} else {
		    fprintf(wfp, "\n");
		}
		AtStart = FALSE;
	    }
	    fputs(LineBuf, wfp);
	}
	fclose(rfp);
	if (ferror(rfp) || feof(rfp)) {
	    fclose(rfp);
	    AMS_RETURN_ERRCODE(errno, EIN_FERROR, EVIA_ADDPARENTAL);
	}
	if (vfclose(wfp)) {
	    unlink(SourceFileName);
	    AMS_RETURN_ERRCODE(errno, EIN_VFCLOSE, EVIA_ADDPARENTAL);
	}
	if (AppendFileToFolder(SourceFileName, DirName, FALSE)) {
	    unlink(SourceFileName);
	    return(mserrcode);
	}
    }
    if (AddParentalMessageFromFile(SourceFileName, ParentName, ShortName, DirName)) {
	if (!InitDir || !*InitDir) unlink(SourceFileName);
	AMS_RETURN_ERRCODE(EMSNOPARENT, AMS_ERRCAUSE, AMS_ERRVIA);
    }
    if (!InitDir || !*InitDir) {
	unlink(SourceFileName);
    }
    return(0);
}
