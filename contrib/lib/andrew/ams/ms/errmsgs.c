/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/ms/RCS/errmsgs.c,v 2.6 1991/09/12 15:50:34 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/ms/RCS/errmsgs.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/ms/RCS/errmsgs.c,v 2.6 1991/09/12 15:50:34 bobg Exp $ ";
#endif /* lint */

#include <stdio.h>
#include <errprntf.h>
#include <mailconf.h>
#include <mail.h>
#include <util.h>
#include <ms.h>

/* The functions NonfatalBizarreError and CriticalBizarreError are in separate files to make it easier for clients to override them  */

extern char home[];
extern FILE *fopen();

static char *ErrLevel(level)
int level;
{
    switch(level) {
	case ERR_CRITICAL:
	    return("CRITICAL");
	case ERR_WARNING:
	    return("warning");
	default:
	    return("odd level");
    }
}

BizarreError(text, level)
char *text;
int level;
{
    static char ProgName[12] = "";
    char Fname[1+MAXPATHLEN];
    static int IsPostman = -1;
    FILE *fp = NULL;

    if (!ProgName[0]) {
	if (IsPostman < 0) {
	    struct CellAuth *ca; char *PMName;
	    ca = NULL;
	    FindAMSHomeCell(&ca);
	    PMName = NULL;
	    if (ca != NULL) {FillInCell(ca); PMName = CheckAMSPMName(ca->CellName);}
	    if (ca != NULL && ca->UserName != NULL && PMName != NULL && strcmp(ca->UserName, PMName) == 0) IsPostman = 1;
	    else IsPostman = 0;
	}
	if (IsPostman > 0) {
	    sprintf(ProgName, "ms-%d", getpid());
	} else {
	    strcpy(ProgName, "ms");
	}
    }
    if (AMS_DevConsoleIsSacred) {
	strcpy(Fname, home);
	strcat(home, "/MS-Errors"); /* This string also appears in ams/messages/msgsbe.c */
	fp = fopen(Fname, "a");
    }
    if (fp) {
	long foo;
	foo =time(0);
	fprintf(fp, "Message server %s message reported on %s: %s", ErrLevel(level), ctime(&foo), text);
	fclose(fp);
    } else {
	errprintf(ProgName, level, 0, 0, "%s", text);
    }
}

