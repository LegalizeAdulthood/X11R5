/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/msgsa.c,v 2.15 1991/09/12 16:48:51 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/msgsa.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/msgsa.c,v 2.15 1991/09/12 16:48:51 bobg Exp $ ";
#endif /* lint */

#include <andrewos.h>
#include <sys/param.h>
#include <stdio.h>
#include <errprntf.h>
#include <class.h>
#include <im.ih>
#include <frame.ih>

#include <cui.h>
#include <fdphack.h>
#include <sendmsg.ih>
#include <messwind.ih>
#include <captions.ih>
#include <folders.ih>
#include <init.ih>
#include <environ.ih>
#include <ams.ih>
#include <amsutil.ih>
#include <msgsa.eh>

#include <msgsvers.h>

boolean messagesapp__InitializeObject(cid, m)
struct classheader *cid;
struct messagesapp *m;
{
    m->MailOnly = FALSE;
    m->SendOnly = FALSE;
    m->AppendBugInfo = FALSE;
    m->ConsiderChanged = 0;
    m->UsingSnap = 0;
    m->RetainedArgc = 0;
    m->RetainedArg[0] = NULL;
    m->AddHeaderList[0] = NULL;
    m->MailSourceFile[0] = NULL;
    m->SeparateWindows = FALSE;
    messagesapp_SetMajorVersion(m, MESSAGES_MAJOR_VERSION);
    messagesapp_SetMinorVersion(m, MESSAGES_MINOR_VERSION);
    return(TRUE);
}

boolean messagesapp__Start(m)
struct messagesapp *m;
{
    struct folders *fold = NULL;
    struct sendmessage *sm = NULL;
    struct view *v;
    int i;
    char VerString[25], DeliveryVersion[25];
    boolean Die = FALSE;

    super_Start(m);
    sprintf(VerString, "Version %d.%d-%c",
             MESSAGES_MAJOR_VERSION,MESSAGES_MINOR_VERSION , m->UsingSnap ? 'S' : 'N');
    sprintf(DeliveryVersion, "Messages %d.%d-%c",
             MESSAGES_MAJOR_VERSION,MESSAGES_MINOR_VERSION , m->UsingSnap ? 'S' : 'N');
    if (m->SendOnly) {
	sm = sendmessage_New();
	sm->myframe = ams_InstallInNewWindow(sm, "sendmessage", VerString, environ_GetProfileInt("sendmessage.width", -1), environ_GetProfileInt("sendmessage.height", -1), sm);
	if (!sm->myframe) Die = TRUE;
	v = (struct view *) sm;
    } else {
	if (m->SeparateWindows) {
	    fold = folders_New();
	    v = (struct view *) folders_GetApplicationLayer(fold);
	    fold->myframe = ams_InstallInNewWindow(v, "messages-folders", VerString, environ_GetProfileInt("folders.width", 600), environ_GetProfileInt("folders.height", 120), fold);
	} else {
	    struct messwind *mess = messwind_New();
	    fold = mess->folders;
	    v = (struct view *) mess;
	    fold->myframe = ams_InstallInNewWindow(mess, "messages", VerString, environ_GetProfileInt("messages.width", -1), environ_GetProfileInt("messages.height", -1), fold);
	}
	if (!fold->myframe) Die = TRUE;
    }
    if (Die) {
	fprintf(stderr, "Could not create new im object--sorry!");
	exit(-1);
    }
    ams_SetCUIRock(v);
    ams_SetWantSnap(m->UsingSnap);
    messagesapp_Fork(m);
    message_DisplayString(NULL, 10, "Processing arguments...");
    im_ForceUpdate();
    ams_WaitCursor(TRUE);
    ams_CUI_SetClientVersion(ams_GetAMS(), DeliveryVersion);

    if (m->SendOnly) {
	char TmpToHeader[1000];

	TmpToHeader[0] = '\0';
	for (i=0; i<m->RetainedArgc; ++i) {
	    char *s = amsutil_StripWhiteEnds(m->RetainedArg[i]);
	    strcat(TmpToHeader, s);
	    if (*(s+strlen(s)-1) != ',') {
		strcat(TmpToHeader, ", ");
	    } else {
		strcat(TmpToHeader, " ");
	    }
	}
	if (m->MailSourceFile[0]) {
	    sendmessage_ReadFromFile(sm, m->MailSourceFile, FALSE);
	} else {
	    sendmessage_Reset(sm);
	}
	for(i=0; m->AddHeaderList[i]; ++i) {
	    sendmessage_AddHeaderLine(sm, m->AddHeaderList[i]);
	    free(m->AddHeaderList[i]);
	}
	if (TmpToHeader[0]) {
	    TmpToHeader[strlen(TmpToHeader) - 2] = '\0';
	    sendmessage_AddToToHeader(sm, TmpToHeader);
	}
	if (m->ConsiderChanged) {
	    --sm->HeadModified;
	}
	sendmessage_ResetSendingDot(sm);
	if (m->AppendBugInfo) {
	    sendmessage_AppendBugInfoToBody(sm, FALSE);
	}
    } else {
	folders_UpdateMsgs(fold, m->MailOnly, (m->RetainedArgc > 0) ? m->RetainedArg : NULL, FALSE);
	if (m->MailOnly && amsutil_GetOptBit(EXP_SHOWCLASSES)) {
	    folders_Reconfigure(fold, LIST_MAIL_FOLDERS);
	}
    }
    ams_WaitCursor(FALSE);
    return(TRUE);
}
 
boolean messagesapp__ParseArgs(m, argc, argv)  
struct messagesapp *m;
int argc;
char **argv;
{
    int HeadersToAdd = 0;
    super_ParseArgs(m, argc, argv);
    ++argv;
    while (argv[0]) {
	if (argv[0][0] == '-') {
	    switch (argv[0][1]) {
		case 'b':
		    m->AppendBugInfo = TRUE;
		    break;
		case 'f':
		    ++argv;
		    if (argv[0]) {
			strcpy(m->MailSourceFile, argv[0]);
		    } else {
			--argv;
		    }
		    break;
		case 'h':
		    ++argv;
		    if (argv[0]) {
			m->AddHeaderList[HeadersToAdd] = malloc(1+strlen(argv[0]));
			strcpy(m->AddHeaderList[HeadersToAdd++], argv[0]);
			if (HeadersToAdd >= MAXADDHEADS) {
			    fprintf(stderr, "Too many added headers (compiled-in maximum is %d)\n", MAXADDHEADS);
			    exit(-1);
			}
		    } else {
			--argv;
		    }
		    break;
		case 'm':
		    m->MailOnly = TRUE;
		    break;
		case 's':
		    m->SendOnly = TRUE;
		    break;
		case 'w':
		    m->SeparateWindows = TRUE;
		    break;
		case 'z':
		    m->ConsiderChanged = 1;
		    break;
		case 'N':
		    m->UsingSnap = 0;
		    break;
		case 'S':
		    m->UsingSnap = 1;
		    break;

		default:
		    fprintf(stderr, "Illegal option: '%s'--ignored\n", argv[0]);
		    break;
	    }
	} else {
	    m->RetainedArg[m->RetainedArgc++] = argv[0];
	    if (m->RetainedArgc >= MAXARGS) {
		fprintf(stderr, "Too many arguments (compiled-in maximum is %d)\n", MAXARGS);
		exit(-1);
	    }
	}
	++argv;
    }
    m->RetainedArg[m->RetainedArgc] = NULL;
    m->AddHeaderList[HeadersToAdd] = NULL;
    environ_SetProgramName(m->SendOnly ? "sendmessage" : "messages");
    messagesapp_SetName(m, m->SendOnly ? "sendmessage" : "messages");
    return(TRUE);
}
