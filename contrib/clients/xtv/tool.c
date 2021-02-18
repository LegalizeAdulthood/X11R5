/*
 * Copyright (c) 1991 Old Dominion University.
 * Copyright (c) 1991 University of North Carolina at Chapel Hill.
 * All rights reserved.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  Old Dominion University and the University of North
 * Carolina at Chapel Hill make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

/*
   tool.c
   Routines to manage the tool list

*/

#define   DEBUGAddTool      01&DEBUGON
#define   DEBUGDeleteTool      01&DEBUGON
#define   DEBUGFindTool      01&DEBUGON
#define   DEBUGBuildToolWidget   01&DEBUGON
#define   DEBUGRebuildToolList   01&DEBUGON
#define   DEBUGInitializeTool   01&DEBUGON
#define   DEBUGToolCheck      01&DEBUGON
#define   DEBUGStartNewTool      01&DEBUGON
#define   DEBUGFryTool    01&DEBUGON
#define   DEBUGCleanUpTool    01&DEBUGON
#define   DEBUGStartLocalTool    01&DEBUGON
#define   DEBUGStartRemoteTool    01&DEBUGON
#define   DEBUGsig_child    01&DEBUGON
#define   DEBUGJoinTool         01&DEBUGON


#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <X11/Xproto.h>         /* Needed for X_TCP_PORT */
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Cardinals.h>
#include <signal.h>
#include <sys/wait.h>

#include "memory.h"
#include "protocol.h"
#include "connect.h"
#include "subscriber.h"
#include "token.h"
#include "tool.h"
#include "popups.h"
#include "network.h"

/*
 * Begin Murray Addition
 */

/*
 * arpa/inet.h has the definition of inet_ntoa that was producing warnings.
 * I was getting sick of the warnings.
 */
#include <arpa/inet.h>
#include "subscriber2.h"
#include "tool2.h"
#include "tokenInfo.h"

/*
 * End Murray Addition
 */

extern HostDisplayNumber;
extern char HHostName[];

extern int JOIN;
extern int MinFloorTime;
extern TimeOutFlag;
extern int POLICY;
int sig_child();

BYTE CleanUpToolFlag = FALSE;

/* ------------------------------------------------------------------------------------------- */

static Tool   *ToolIDList[256];   /* List of Tools by ID */

#ifndef NoIDAvailable
#define   NoIDAvailable      255
#endif

/*
 *   InitializeToolIDList()
 *
 *   Clear out the entire list of IDs and initialize the
 *   CurrentID value to 0.  Note that element 255 is reserved as
 *   a value for un-assignables.
 *
 *   RETURN VALUE:   void
 */
static void InitializeToolIDList()
{
	int   i;

	for ( i=0; i<256; i++ )
		ToolIDList[i] = NULL;
}


/*
 *   AssignToolID()
 *
 *   Hunt down an empty slot in the ID list and
 *   return its index.  This will serve as the tool's
 *   ID number throughout its life.
 *
 *   RETURN VALUE:   Element number (a byte)
 *         NoIDAvailable if the list is full.
 */
BYTE AssignToolID( Who )
Tool *Who;
{
	BYTE   i=0;

	for ( i=0; ((ToolIDList[i] != NULL) && (i <= NoIDAvailable )) ; i++ );

	if ( i < NoIDAvailable ) {
		ToolIDList[i] = Who;
	}
	return i;
}

/*
 *   SetToolID()
 *
 *   Set a tool ID based on the byte in the tool structure passed in.
 *
 *   RETURN VALUE:   void
 */
void SetToolID( Who )
Tool *Who;
{
	ToolIDList[Who->ToolID] = Who;
}


/*
 *   ClearToolID()
 *
 *   Return a Tool ID to the free pool
 *
 *   RETURN VALUE:   void
 */
void ClearToolID( Which )
BYTE Which;
{
	ToolIDList[Which] = NULL;
}

/*
 *   FindToolByID()
 *
 *   Hunt down a tool by the ToolID field and
 *   return a pointer to the structure.
 *
 *   RETURN VALUE:   Pointer to tool structure
 *         NULL if not found
 */
Tool *FindToolByID(Target)
BYTE Target;
{
	return ToolIDList[Target];
}


/* ------------------------------------------------------------------------------------------- */

ToolNode   *ToolList = NULL;
static   ToolNode   *FindPointer = NULL;
static   int      ToolCount = 0;

static char   *EmptyList[] = {
	"None"
};


Widget      WTool;
Widget      WToolLabel;
Widget      WToolViewport;
Widget      WToolList;
Widget      WToolNew;
Widget      WToolGToken;
Widget      WToolDToken;
Widget      WToolSToken;
/*
 * Begin Murray Addition
 */
Widget      WToolIToken;
/*
 * End Murray Addition
 */
Widget      WToolJoin;
Widget      WToolTerminate;


static Arg  args[20];

extern Widget GetTopLevelWidget();   /* Added 01 */

/* ------------------------------------------------------------------------------------------- */

/*
 *   FirstTool()
 *
 *   Reset FindPointer to the first tool in the list and return 
 *   a pointer to the structure.
 *
 *   RETURN VALUE:   Pointer to first tool
 *         NULL if list is empty.
 */
Tool *FirstTool()
{
	FindPointer = ToolList;
	return (FindPointer != NULL ? FindPointer->ToolS : NULL);
}


/*
 *   NextTool()
 *
 *   Move FindPointer to the next tool in the list and return 
 *   a pointer to the structure.
 *
 *   RETURN VALUE:   Pointer to nextt tool
 *         NULL if at end of list
 */
Tool *NextTool()
{
	if (FindPointer != NULL)
		FindPointer = FindPointer->Next;

	if ( FindPointer == NULL )
		return NULL;
	else
		return FindPointer->ToolS;
}


/*
 *   AddTool()
 *
 *   Add a new tool to the list.  Insert the new entry sorted by
 *   the ToolCommand field in the structure.  Also blank out the
 *   token list field.
 *
 *   RETURN VALUE:   (void)
 */
void AddTool(NewTool)
Tool *NewTool;
{
	char      *LastWord;
	ToolNode   *NewNode;
	ToolNode   *WhereAt;

#   if DEBUGAddTool
	fprintf( stderr, "AddTool(): Adding %s\n", NewTool->ToolCommand );
#   endif

	NewNode=(ToolNode *)GetMem(sizeof(ToolNode));

	NewNode->ToolS = NewTool;
	ToolCount++;

	/* Check to see if this is the first-ever item */

	if ( ToolList == NULL ) {
#      if DEBUGAddTool
		fprintf( stderr, "AddTool(): First entry in empty list.\n" );
#      endif
		NewNode->Next = NULL;
		ToolList = NewNode;
		RebuildToolList();
		return;
	}

	/* Check for head-of-list */

	if ( strcmp(NewTool->ToolCommand, ToolList->ToolS->ToolCommand ) < 0 ) {
#      if DEBUGAddTool
		fprintf( stderr, "AddTool(): Added at head, before %s\n",
		    ToolList->ToolS->ToolCommand );
#      endif
		NewNode->Next = ToolList;
		ToolList = NewNode;
		FindPointer = NULL;
		RebuildToolList();
		return;
	}

	/* Otherwise, find the right place for it */

	for ( WhereAt=ToolList ; WhereAt->Next != NULL ; WhereAt=WhereAt->Next ) {
#      if DEBUGAddTool
		fprintf( stderr, "AddTool(): Checking %s\n", WhereAt->Next->ToolS->ToolCommand );
#      endif
		if ( strcmp(WhereAt->Next->ToolS->ToolCommand,NewTool->ToolCommand) > 0 ) {
#      if DEBUGAddTool
			fprintf( stderr, "AddTool(): Stopped there.\n" );
#      endif
			break;
		}
	}

	NewNode->Next = WhereAt->Next;
	WhereAt->Next = NewNode;
	RebuildToolList();
}


/*
 *   DeleteTool()
 *
 *   Hunt down a Tool and kill it.  If the entry does
 *   not exist in the list, scrub it.
 *
 *   RETURN VALUE:   void
 */
void DeleteTool(Target)
Tool *Target;
{
	ToolNode   *WhereAt;
	ToolNode   *Temp;

#   if DEBUGDeleteTool
	fprintf( stderr, "DeleteTool(): Removing %s\n", Target->ToolCommand );
#   endif

	ClearToolID( Target->ToolID );


#   if DEBUGDeleteTool
        fprintf( stderr, "DeleteTool(): Looking at First Node \n");
        fprintf( stderr, "Contains %s\n", ToolList->ToolS->ToolCommand);
#   endif

	if ( Target == ToolList->ToolS ) {
#      if DEBUGDeleteTool
		fprintf( stderr, "DeleteTool(): First in list\n" );
#      endif
		Temp = ToolList;
		ToolList = ToolList->Next;
		Temp->Next=NULL;
		if( GetMode() == TRUE && JOIN ){
			FreeMem( Target->ArchiveBuf );
			FreeMem( Target->Archive2Buf );
			FreeToolInfo( Target->ToolInfo );
		}
		FreeMem(Target);
		FreeMem(Temp);

		ToolCount--;
		RebuildToolList();
		return;
	}
#   if DEBUGDeleteTool
        fprintf( stderr, "DeleteTool(): Looking after First Node\n");
#   endif


	for ( Temp = ToolList, WhereAt=ToolList->Next ; WhereAt!= NULL ; Temp=WhereAt, WhereAt=WhereAt->Next ) {
#      if DEBUGDeleteTool
		fprintf( stderr, "DeleteTool(): Looking at %s\n",
		    WhereAt->ToolS->ToolCommand );
#      endif
		if ( WhereAt->ToolS == Target ) {
#         if DEBUGDeleteTool
			fprintf( stderr, "DeleteTool(): Found!\n" );
#         endif
			break;
		}
	}

	/* See if we didn't find it */

	if ( WhereAt == NULL ) {
#      if DEBUGDeleteTool
		fprintf( stderr, "DeleteTool(): Not Found\n" );
#      endif
		return;
	}

	/* Fry the sucker! */

	Temp->Next = WhereAt->Next;
	WhereAt->Next=NULL;
	if( GetMode() == TRUE && JOIN ){
		FreeMem( WhereAt->ToolS->ArchiveBuf );
		FreeMem( WhereAt->ToolS->Archive2Buf );
		FreeToolInfo( WhereAt->ToolS->ToolInfo );
	}
	FreeMem( WhereAt->ToolS );
	FreeMem( WhereAt);
	ToolCount--;
	RebuildToolList();
}


/*
 *   FindTool()
 *
 *   Hunt down a tool by the ToolCommand field and
 *   return a pointer to the structure.
 *
 *   RETURN VALUE:   Pointer to tool structure
 *         NULL if not found
 */
Tool *FindTool(Target)
char *Target;
{
	ToolNode   *WhereAt;
	ToolNode   *Temp;

	for ( WhereAt=ToolList ; WhereAt != NULL ; WhereAt=WhereAt->Next ) {
#      if DEBUGFindTool
		fprintf( stderr, "FindTool(): Looking at %s\n",
		    WhereAt->ToolS->ToolCommand );
#      endif
		if ( !strcmp( WhereAt->ToolS->ToolCommand, Target ) ) {
#         if DEBUGFindTool
			fprintf( stderr, "FindTool(): Found!\n" );
#         endif
			break;
		}
	}

	return ( WhereAt ? WhereAt->ToolS : NULL );
}


/* ------------------------------------------------------------------------------------------- */

Socket         ToolSocket;
int         DisplayNumber;
struct sockaddr_in   ListenerName;

/*
 *   RebuildToolList()
 *
 *   Scoot through the tool list and build a list and build an array of pointers
 *   to the FullName field.
 *
 *   RETURN VALUE:   void
 */
void RebuildToolList()
{
	static char   **TList = NULL;
	Tool      *WhereAt;
	int      n;
	int      Counter = 0;

	/* Handle an empty list by shutting everything off */

	if ( ToolList == NULL ) {
		if (TList != NULL)
			FreeMem(TList);
		n=0;
		XtSetArg( args[n],    XtNsensitive,      FALSE );
		n++;
		XtSetArg( args[n],    XtNlist,       EmptyList );
		n++;
		XtSetArg( args[n],    XtNnumberStrings,   1 );
		n++;
		XtSetValues( WToolList, args, n );

		n=1;
		XtSetArg( args[n], XtNwidth, 300);
		n++;
#ifdef rs6000
		XtSetArg( args[n], XtNheight, 40);
#else
		XtSetArg( args[n], XtNheight, 36);
#endif
		n++;
		XtSetValues( WToolViewport, args, n);
		n--; n--;

		XtSetValues( WToolGToken, args, 1 );
		XtSetValues( WToolDToken, args, 1 );
		XtSetValues( WToolSToken, args, 1 );
/*
 * Begin Murray Addition
 */
		XtSetValues( WToolIToken, args, 1 );
/*
 * End Murray Addition
 */
		XtSetValues( WToolTerminate, args, 1 );
		return;
	}

	/* Build the list */

#   if DEBUGRebuildToolList
	fprintf( stderr, "RebuildToolList(): %d items at %d bytes each\n",
	    ToolCount, sizeof(char *) );
#   endif

	TList=(char **)GetMem( ToolCount * sizeof(char *) );

	for ( WhereAt=FirstTool(); WhereAt != NULL; WhereAt=NextTool() ) {
#      if DEBUGRebuildToolList
		fprintf( stderr, "RebuildToolList(): Found %s\n", WhereAt->ToolCommand );
#   endif
		/* xyzzy - Extract actual command, not the whole mess */
		TList[Counter++] = WhereAt->ToolCommand;
	}

/*
 * For some reason, the resizing was messing up and clipping the viewport
 * So leave it alone and never resize.
 */

	XawListChange( WToolList, TList, ToolCount, 0, FALSE );


	n=0;
	XtSetArg( args[n],    XtNsensitive,      TRUE );
	n++;
/*
 * Begin Murray Addition
 */
	XtSetValues( WToolViewport, args, n);
/*
 * End Murray Addition
 */
	XtSetValues( WToolGToken, args, n );
	XtSetValues( WToolDToken, args, n );
	XtSetValues( WToolSToken, args, n );
/*
 * Begin Murray Addition
 */
	XtSetValues( WToolIToken, args, n );
/*
 * End Murray Addition
 */
	XtSetValues( WToolTerminate, args, n );
	XtSetValues( WToolList, args, n );

        n=1;
        XtSetArg( args[n], XtNwidth, 300);
        n++;
#ifdef rs6000
        XtSetArg( args[n], XtNheight, 40);
#else
        XtSetArg( args[n], XtNheight, 36);
#endif
        n++;
        XtSetValues( WToolViewport, args, n);

}

/* ------------------------------------------------------------------------------------------- */

/* Start Added 01 */
XtCallbackProc CancelDone(w, client_data, call_data)
Widget   w;
caddr_t client_data, call_data;
{
   XtPopdown(TranShellNewTool);
   XtSetSensitive(WTool, TRUE);
}

/*-------------------------------------------------------------------*/

Bool	ToolExpected = FALSE;	/* True if we're expecting a connection from a tool */
Tool	*TempTool;		/* Pointer to temporary tool structure */


/*
 *
 * DialogDoneHost()
 *
 * A call back when Host Button is pressed.
 *
 */
XtCallbackProc DialogDoneHost(w, dialog, call_data)
Widget   w;
Widget   dialog;   /* client_data */
caddr_t call_data;
{
   String      DCommand;
   static String   DefaultString = "";
   char      Command[TOOL_CMD_SIZE];

   Arg     arg;

   XtPopdown(TranShellNewTool);
   XtSetSensitive(WTool, TRUE);
   DCommand = XawDialogGetValueString(dialog);
        Command[strlen(DCommand)-1]=NULL;
        strcpy(Command, DCommand);

   XtSetArg(arg, XtNvalue, DefaultString);
   XtSetValues(dialog, &arg, 1);

   if (strlen(Command) <1) {
      UpdateMessage("\nNull Entry for tool !!\n\n");
      return;
   }


   if (GetMode()) {
      StartLocalTool(Command, GetLocalSubID());
   } else { /*Send the tool create message to the chairman */
      if ( SendByte(GetHostConnection(), SH_NewToolLocal )==ERROR){
		 EndSession();
		return;
	}
      if ( SendData(GetHostConnection(), (BYTE *)Command, TOOL_CMD_SIZE)==ERROR) {
		EndSession();
		return;
	}
   }
}


/* ------------------------------------------------------------------------------------------- */
/*
 *   StartLocalTool()
 *
 *   Start up a new tool localy.
 *
 */
void StartLocalTool(command,creator)
char *command;
BYTE creator;
{
   char      Command[TOOL_CMD_SIZE];
   static char     NewDisplay[HOST_NAME_SIZE];
   char            HostName[HOST_NAME_SIZE];
   char		*HostInet;
   char            *CommandArgs[10];
   char            CommandName[TOOL_CMD_SIZE];
   struct hostent *hp, *gethostbyname();
   struct sockaddr_in server;
   Socket          creconnection;

#   if DEBUGStartLocalTool
   fprintf( stderr, "StartLocalTool(): starting %s\n",
       command);
#   endif

   strcpy(Command, command);
   if (strlen(Command) <1) {
      UpdateMessage("\nNull Entry for tool !!\n\n");
      return;
   }
   TempTool=(Tool *)GetMem(sizeof(Tool));

/*
 * Begin Murray Addition
 */

   TempTool->subscrList = ptrNew();

/*
 * End Murray Addition
 */

   LGetArgs(CommandArgs,Command);
   if ( gethostname( HostName, HOST_NAME_SIZE ) < 0 ) {
#   if DEBUGStartNewTool
      fprintf( stderr, "StartLocalTool(): Can't find host Name\n" );
#   endif
      FatalError( "StartLocalTool(): Unable to get host name" );
	return;

   }

   if  ( (hp = gethostbyname(HostName)) == NULL ) {
#   if DEBUGStartNewTool
      fprintf( stderr, "StartLocalTool(): Can't find host %s\n", HostName);
#   endif
      FatalError("StartLocalTool(): Can't find entry for host %s\n", HostName);
      return;
   }

   bcopy ( hp->h_addr, &(server.sin_addr), hp->h_length);
   HostInet = inet_ntoa(server.sin_addr);
   sprintf( NewDisplay, "DISPLAY=%s:%d.0", HostInet, DisplayNumber );
   strcpy(CommandName, CommandArgs[0]);
   strcpy(TempTool->ToolCommand, CommandName);

   ToolExpected = TRUE;
   TempTool->InCount = 0;
   TempTool->OutCount = 0;
   TempTool->ToolPID = 0;
   TempTool->ToolCreator = creator;
   TempTool->AskedToGetToken = FALSE;
#   if DEBUGStartLocalTool
   fprintf( stderr, "StartLocalTool(): CreatorID %d\n", TempTool->ToolCreator);
   fprintf( stderr, "StartLocalTool(): Forknig %s\n", TempTool->ToolCommand);
   fprintf( stderr, "StartLocalTool(): NewDisplay %s\n", NewDisplay);
#   endif
	        siginterrupt(SIGCHLD, 1);
	signal(SIGCHLD, sig_child);
   if ( (TempTool->ToolPID = fork()) == 0 ){
/*
    	int fd;
    	if ((fd = open ("/dev/tty", O_RDWR)) >= 0){
		ioctl(fd, TIOCNOTTY, (char *) NULL);
		close(fd);
    	}
    	for (fd =0; fd < NOFILE; fd++) close (fd);
      	setpgrp(0,getpid());
*/
      	putenv( NewDisplay );

      execvp (CommandName,&CommandArgs[0]);
      fprintf(stderr, "\nStartLocalTool(): Can't execute %s %s\n", CommandName,TempTool->ToolCommand);
      UpdateMessage( "\nCan't execute Tool\n");
      /*
                        UpdateMessage( "\nStartLocalTool(): Can't execute %s %s\n", CommandName,TempTool->ToolCommand);
*/
      exit(0);
   }
#   if DEBUGStartLocalTool
   fprintf( stderr, "StartLocalTool(): PID for %s : %d \n", CommandName,
       TempTool->ToolPID );
#   endif

}

/* ------------------------------------------------------------------------------------------- */
/*
 *   StartRemoteTool()
 *
 *   Start up a new tool remotely.
 *
 */
void StartRemoteTool(tool, creator)
char *tool;
BYTE creator;
{
   char      Command[TOOL_CMD_SIZE];
   static char     NewDisplay[HOST_NAME_SIZE];
   char            HostName[HOST_NAME_SIZE];
   char            *CommandArgs[10];
   char            CommandName[TOOL_CMD_SIZE];

   strcpy(Command, tool);
#   if DEBUGStartRemoteTool
   fprintf( stderr, "StartRemoteTool(): starting %s\n",
       tool);
#   endif

   TempTool=(Tool *)GetMem(sizeof(Tool));
   LGetArgs(CommandArgs,Command);
   strcpy(CommandName, CommandArgs[0]);
   strcpy(TempTool->ToolCommand, CommandName);

/*
 * Begin Murray Addition
 */

   TempTool->subscrList = ptrNew();

/*
 * End Murray Addition
 */

   ToolExpected = TRUE;
   TempTool->InCount = 0;
   TempTool->OutCount = 0;
   TempTool->ToolPID = 0;
   TempTool->ToolCreator = creator;
   TempTool->AskedToGetToken = FALSE;

#   if DEBUGStartRemoteTool
   fprintf( stderr, "StartRemoteTool(): CreatorID %d\n", TempTool->ToolCreator);
   fprintf( stderr, "StartRemoteTool(): started %s\n",
       TempTool->ToolCommand);
#   endif
}


/*
 *
 * DefaultHost()
 *
 * An action proc called when Return is pressed
 *
 */
XtCallbackProc DefaultHost(w, event)
Widget w;
XButtonEvent *event;
{
   /* What an imitation !, violation of OOP !! */
   DialogDoneHost(w, NewToolDialog, NULL);
}

/* ------------------------------------------------------------------------------------- */

/*
 *
 *
 */

XtCallbackProc DialogDoneRemote(w, dialog, call_data)
Widget  w;
Widget dialog;
caddr_t call_data;
{
   String      DCommand;
   char      Command[TOOL_CMD_SIZE];

   static String   DefaultString = "";
   Arg     arg;
   static char     NewDisplay[HOST_NAME_SIZE];
   char            HostName[HOST_NAME_SIZE];
   char         *HostInet;
   char            *CommandArgs[10];
   char            CommandName[TOOL_CMD_SIZE];
   struct hostent *hp, *gethostbyname();
   struct sockaddr_in server;



   XtPopdown(TranShellNewTool);
   XtSetSensitive(WTool, TRUE);
   DCommand = XawDialogGetValueString(dialog);
        Command[strlen(DCommand)-1]=NULL;
        strcpy(Command, DCommand);
   XtSetArg(arg, XtNvalue, DefaultString);
   XtSetValues(dialog, &arg, 1);

   if (strlen(Command) <1) {
      UpdateMessage("\nNull Entry for tool !!\n\n");
      return;
   }

   if (GetMode() == TRUE) {
      if ( gethostname( HostName, HOST_NAME_SIZE ) < 0 ) {
#   if DEBUGStartNewTool
      fprintf( stderr, "StartLocalTool(): Can't find host Name\n" );
#   endif
      FatalError( "StartLocalTool(): Unable to get host name" );
	return;
	}
      StartRemoteTool(Command, GetLocalSubID());
   }
   else {
#   if DEBUGStartNewTool
      printf(stderr,"DialogDoneRemote():HHostName %s\n", HHostName);
#   endif
      strcpy(HostName, HHostName);
      DisplayNumber = HostDisplayNumber;
      if ( SendByte(GetHostConnection(), SH_NewToolRemote )==ERROR){
		 EndSession();
		return;
	}
      if ( SendData(GetHostConnection(), (BYTE *)Command, TOOL_CMD_SIZE)==ERROR) {
		EndSession();
		return;
	}
#   if DEBUGDialogDoneRemote
      fprintf( stderr,
          "DialogDoneRemote(): Sent  SH_NewToolRemote %s to start %s\n",
          HostName, Command);
#   endif

   }

   if  ( (hp = gethostbyname(HostName)) == NULL ) {
#   if DEBUGStartNewTool
      fprintf( stderr, "StartNewTool(): Can't find host %s\n", HostName);
#   endif
      FatalError("StartNewTool(): Can't find entry for host %s\n", HostName);
      return;
   }

   bcopy ( hp->h_addr, &(server.sin_addr), hp->h_length);
   HostInet = inet_ntoa(server.sin_addr);

   UpdateMessage("Login to remote host and then Type:     \n setenv DISPLAY  %s:%d.0\n OR setenv DISPLAY  %s:%d.0     \n then run X client application:     \n (%s)\n ", HostName, DisplayNumber, HostInet, DisplayNumber, Command);
   return;
}


/* ------------------------------------------------------------------------------------------- */
/*
 *   StartNewTool()
 *
 *   Start up a new tool.
 *
 *   RETURN VALUE:   rv
 */
XtCallbackProc StartNewTool( widget, TopLevel, callData )
Widget widget;
Widget TopLevel;
XtPointer callData;
{
        XtSetSensitive(WTool, FALSE);
        PopUpBox ( widget, TranShellNewTool, callData );
}

static BYTE id[100];
static BYTE *name[100];
static int JoinToolNum;
static Widget TmpWidget;
static XtPointer TmpCallData;

/*
 *   JoinTool()
 *
 *   Join a tool.
 *
 *   RETURN VALUE: rv
 */
XtCallbackProc JoinTool( widget, TopLevel, callData )
Widget widget;
Widget TopLevel;
XtPointer callData;
{
        TmpWidget = widget;
        TmpCallData = callData;
        if( SendByte(GetHostConnection(), SH_WantJoinTools)==ERROR)
                EndSession();
}

HandleToolsToJoinList()
{
        int n;
        int i;

        XtSetSensitive(WTool, FALSE);

        i = 0;
        while(1){
                id[i] = GetByte(GetHostConnection());
                if( id[i] == 0xff ) break;
                name[i] = ReadData(GetHostConnection(),100,BLOCK);
#       if DEBUGJoinTool
                printf("%d %s\n",id[i],name[i]);
#       endif
                i++;
        }
        JoinToolNum = i;
        if( i == 0 ){
                XtSetSensitive(WTool, TRUE);
                UpdateMessage("No tool available to join");
                return;
        }

        XawListChange( JoinToolList, name, i, 0, FALSE );

        n=0;
        XtSetArg( args[n], XtNsensitive, TRUE );
        n++;
        XtSetValues( JoinToolList, args, n);

        XtSetArg( args[n], XtNwidth, 150);
        n++;
        XtSetArg( args[n], XtNheight, 50);
        n++;
        XtSetValues( JoinToolViewPort, args, n);

        PopUpBox ( TmpWidget, TranShellJoinTool, TmpCallData );
}

/*
 * DialogDoneJoinAll()
 *
 * A call back when Join_All Button is pressed
 *
 */
XtCallbackProc DialogDoneJoinAll(w, dialog, call_data)
Widget   w;
Widget   dialog;   /* client_data */
caddr_t call_data;
{
	int i;

	XtPopdown(TranShellJoinTool);
	XtSetSensitive(WTool, TRUE);

	SendByte(GetHostConnection(), SH_JoinTools);
	for( i=0; i<JoinToolNum; i++)
		SendByte(GetHostConnection(), id[i]);
	SendByte(GetHostConnection(),0xff);
}

/*
 *
 * DialogDoneJoin()
 *
 * A call back when Join Button is pressed
 *
 */
XtCallbackProc DialogDoneJoin(w, dialog, call_data)
Widget   w;
Widget   dialog;   /* client_data */
caddr_t call_data;
{
        XawListReturnStruct     *Selected;
        int i;

        XtPopdown(TranShellJoinTool);
        XtSetSensitive(WTool, TRUE);

        Selected = XawListShowCurrent( JoinToolList );

        /* Bail out of nothing's highlighted */
        if( Selected->list_index == XAW_LIST_NONE ){
                UpdateMessage( "No tool selected!");
                return;
        }

        for( i=0; i<JoinToolNum && strcmp(name[i],Selected->string); i++);

        if( JoinToolNum == i ){
                fprintf(stderr, "Tool list for joining screwed up!\n");
                exit(1);
        }

        SendByte(GetHostConnection(), SH_JoinTools);
        SendByte(GetHostConnection(), id[i]);
        SendByte(GetHostConnection(), 0xff);
}

XtCallbackProc JoinCancelDone(w, client_data, call_data)
Widget   w;
caddr_t client_data, call_data;
{
        XtPopdown(TranShellJoinTool);
        XtSetSensitive(WTool, TRUE);
}

Tool *FriedTool;

/* ------------------------------------------------------------------------------------------- */

/*
 *      FryTool
 *
 *      Send a subscriber a HS_DeleteTool and close it out
 *
 *      RETURN VALUE:   void
 */
XtCallbackProc FryTool( widget, clientData, callData )
Widget widget, clientData;
XtPointer callData;
{
   XawListReturnStruct     *Selected;
   Tool              *What;



      Selected = XawListShowCurrent( WToolList );

      /* Bail out of nothing's highlighted */
      if ( Selected->list_index == XAW_LIST_NONE ) {
         UpdateMessage( "No Tool is selected." );
#               if DEBUGFryTool
         fprintf( stderr, "FryTool(): NoTool selected.  Returning.\n" );
#               endif
         return;
      }

#       if DEBUGFryTool
        fprintf( stderr, "FryTool(): Cooking %s\n", Selected->string );
#       endif

      if ( (What=FindTool(Selected->string)) == NULL ) {
         FatalError( "FryTool(): ERROR: List widget inconsistent with tool list" );
#       if DEBUGFryTool
      	fprintf( stderr, "FryTool(): Can't find info about %s\n", Selected->string );
#       endif
	return;
      }
/* allow a participnat to stop a tool */
/*
        if (!GetMode() && What->ToolCreator != GetLocalSubID()) {
                UpdateMessage("Not the owner for this tool\n");
		return;
	}
*/

#       if DEBUGFryTool
           fprintf( stderr,
          "FryTool(): Killing %s whose PID is: %d and whose ToolID is: %d\n",
          What->ToolCommand, What->ToolPID, What->ToolID);
#        endif

	FriedTool = What;

        XtSetSensitive(WTool, FALSE);
	PopUpBox ( widget, TranShell[TermTool], callData );
}

XtCallbackProc CancelFryTool (w, client_data, call_data)
Widget   w;
caddr_t client_data, call_data;

{
        XtPopdown(TranShell[TermTool]);
        XtSetSensitive(WTool, TRUE);

}

XtCallbackProc ConfirmFryTool (w, client_data, call_data)
Widget   w;
caddr_t client_data, call_data;

{
        XtPopdown(TranShell[TermTool]);
        XtSetSensitive(WTool, TRUE);


    if (GetMode() == TRUE) 
    	CleanUpTool(FriedTool);
    else {
#       if DEBUGConfirmFryTool
      	fprintf( stderr, "ConfirmFryTool(): ToolCreator: %d, GetLocalSubID():%d\n ", 
        FriedTool->ToolCreator, GetLocalSubID());
#       endif
/* Allow participnats to stop tools */
/*
      	if (FriedTool->ToolCreator == GetLocalSubID()) {
*/
       	  /* tool creator can kill tool */
       	  	if ( SendByte(GetHostConnection(), SH_StopTool)==ERROR) {
			EndSession();
			return;
		}
       	  	if ( SendByte(GetHostConnection(), FriedTool->ToolID)==ERROR){
			 EndSession();
			return;
		}
		CleanUpTool(FriedTool);
/*
      	} else 
         	UpdateMessage("Not the owner for this tool\n");
*/
   }
}

/* ------------------------------------------------------------------------------------------- */

void CleanUpTool(What)
Tool *What;
{

	BYTE                    DeadID;
	Subscriber              *Who;
	/* Cook it, but save a copy of its ID number */
        CleanUpToolFlag = TRUE;
	DeadID = What->ToolID;
#       if DEBUGCleanUpTool
	fprintf( stderr,
	    "CleanUpTool(): Killing %s whose PID is: %d and whose ToolID is: %d\n",
	    What->ToolCommand, What->ToolPID, DeadID);
#       endif


	if (GetMode() == TRUE) 
	  {
	    if (POLICY == 1) XtRemoveTimeOut(What->floor_timer);
	    XtRemoveInput(What->InID);
	    XtRemoveInput(What->InExceptID);
	    shutdown( What->InConnection, 2 );
#       if DEBUGCleanUpTool
	    fprintf( stderr, "CleanUpTool(): shutdown Tool socket\n");
#       endif
	  }

	shutdown( What->OutConnection,2 );
	XtRemoveInput(What->OutID);
	XtRemoveInput(What->OutExceptID);
#       if DEBUGCleanUpTool
	fprintf( stderr, "CleanUpTool(): shutdown Server socket\n");
#       endif


/*
	if( GetMode() != TRUE || JOIN == 0 || What->SubDirtyCount == 0 )
*/

	DeleteTool( What );

	/* Send everybody the bad news. */

	if (TimeOutFlag == FALSE)
		UpdateMessage( "Tool %s has been terminated.", What->ToolCommand );
	else
		UpdateMessage( "You have been time out the Tool %s.",What->ToolCommand );

	if (GetMode() == TRUE) {
	  for ( Who=FirstSubscriber(); Who != NULL; Who=NextSubscriber() ) 
	    {
	      if( JOIN && ArchiveToBeSent(Who,What) )
		continue;
	      if (Who->SubscriberID != 0)
		{
		  if (SendByte( Who->Connection, HS_DeleteTool )==ERROR)
		    {
		      CleanUpSubscriber(Who);
		      continue;
		    }
		  if (SendByte( Who->Connection, DeadID )==ERROR)
		    {
		      CleanUpSubscriber(Who);
		      continue;
		    }
		}
	    }
	  {
	    char buf[2];
	    
	    What->ThisToolDead = 1;
	    if( JOIN && What->SubDirtyCount > 0 ){
	      buf[0] = HS_DeleteTool;
	      buf[1] = DeadID;
	      AppendRequestToArchiveBuf(What,buf,2);
	    }
	  }
#       if DEBUGCleanUpTool
		fprintf( stderr, "CleanUpTool(): XtRemoveInput \n");
#       endif
		if (What->ToolPID != 0) {
			kill(What->ToolPID,SIGKILL);
#       if DEBUGCleanUpTool
			fprintf( stderr, 
			    "CleanUpTool(): Killed %s whose PID is: %d and whose ToolID is: %d\n", 
			    What->ToolCommand, What->ToolPID, DeadID);
#       endif
		}

#ifdef MFAdebug
	  fprintf(stderr, "About to remove the tool from each of the subscribers.\n\r");
#endif

	  findTool(FindSubscriberByID(HOST_ID), What);
	  if (!offrightTool(FindSubscriberByID(HOST_ID)))
	    deleteTool(FindSubscriberByID(HOST_ID));
	  else
	    fprintf(stderr, "Unable to remove tool %s from HOST.\n\r", What->ToolCommand);

	  for (startSubscr(What); !offrightSubscr(What); forthSubscr(What))
	    {
	      findTool(getSubscr(What), What);
	      if (!offrightTool(getSubscr(What)))
		deleteTool(getSubscr(What));
	    }

#ifdef MFAdebug
	  fprintf(stderr, "About to attempt the deletion of tool %s.\n\r", What->ToolCommand);
#endif

	  deleteAllSubscr(What);

#ifdef MFAdebug
	  fprintf(stderr, "Done deleting %s.\n\r", What->ToolCommand);
#endif

	}
	CleanUpToolFlag = FALSE;
}


/*
 *   ConnectAndRefuse()
 *
 *   Fake a connection with an X client and refuse to accept it
 *
 *   RETURN VALUE:   void
 */
void ConnectAndRefuse( Port )
Socket Port;
{
	xConnClientPrefix   *Prefix;
	BYTE         *Leftovers;
	xConnSetupPrefix   SPrefix;

	/* xyzzy - Do nothing for now... Fake a refused connection later */
}

/* ------------------------------------------------------------------------------------------- */

/*
 *   InitializeTool()
 *
 *   Initialize all work processes related to tools and
 *   build the widgets
 *
 *   RETURN VALUE:   void
 */
void InitializeTool( AppCon, Parent, Mode )
XtAppContext AppCon;
Widget Parent;
Bool Mode;      /* TRUE if in 'host' mode */
{
   int      n;

InitializeNewTool( AppCon, Parent, Mode );
InitializeJoinTool( AppCon, Parent );

   /* Hunt down an unused port above 6010 */
if ( GetMode() ) {

   DisplayNumber = XTV_LOW_DISPLAY;
   do {
      ToolSocket = OpenListener( htons(X_TCP_PORT+DisplayNumber), 4, &ListenerName );
   } while ( (ToolSocket < 0) && ( DisplayNumber++ <= XTV_HIGH_DISPLAY ) );

   if (DisplayNumber > XTV_HIGH_DISPLAY) {
      FatalError("Unable to find an available tool port.");
#   if DEBUGInitializeTool
   fprintf( stderr, "InitializeTool(): Unable to allocate a port, current valus is: display port %d\n", DisplayNumber );
#   endif
   return;
   }

   XtAppAddInput( GetAppCon(), ToolSocket, XtInputReadMask, ToolCheck, NULL );

#   if DEBUGInitializeTool
   fprintf( stderr, "InitializeTool(): Tool port open at display port %d\n", DisplayNumber );
#   endif

   fcntl( ToolSocket, F_SETFL, FNDELAY );
}

   WTool = XtCreateManagedWidget( "ToolPanel", formWidgetClass, Parent, NULL, 0 );
   WToolLabel = XtCreateManagedWidget( "Label", labelWidgetClass, WTool, NULL, 0);
   WToolViewport = XtCreateManagedWidget( "VPort", viewportWidgetClass, WTool, NULL, 0 );

   WToolList = XtCreateManagedWidget( "ToolList", listWidgetClass, WToolViewport, NULL, 0 );
   XawListChange( WToolList, EmptyList, 1, 0, TRUE );

   WToolGToken = XtCreateManagedWidget( "GToken", commandWidgetClass, WTool, NULL, 0 );
   XtAddCallback( WToolGToken, XtNcallback, GetToken, NULL );

   WToolDToken = XtCreateManagedWidget( "DToken", commandWidgetClass, WTool, NULL, 0 );
   XtAddCallback( WToolDToken, XtNcallback, DropToken, NULL );

   WToolSToken = XtCreateManagedWidget( "SToken", commandWidgetClass, WTool, NULL, 0 );
   XtAddCallback( WToolSToken, XtNcallback, SnatchToken, NULL );

   WToolNew = XtCreateManagedWidget( "NewTool", commandWidgetClass, WTool, NULL, 0 );
   XtAddCallback( WToolNew, XtNcallback, StartNewTool, TranShellNewTool); /* Added 01 */

/*
 * Begin Murray Addition
 */
   WToolIToken = XtCreateManagedWidget( "IToken", commandWidgetClass, WTool, NULL, 0 );
   XtAddCallback( WToolIToken, XtNcallback, tokenInfo, NULL );
/*
 * End Murray Addition
 */

   WToolJoin = XtCreateManagedWidget( "Join", commandWidgetClass, WTool, NULL, 0 );
   XtAddCallback( WToolJoin, XtNcallback, JoinTool, TranShellJoinTool);

   WToolTerminate = XtCreateManagedWidget( "TermTool", commandWidgetClass, WTool, NULL, 0 );
   XtAddCallback( WToolTerminate, XtNcallback, FryTool, TranShell[TermTool]);

#   if DEBUGInitializeTool
   fprintf( stderr, "InitializeTool(): Done.\n" );
#   endif
}

/*
 *   ToolCheck();
 *
 *   Check for a new Tool and handle all of the formalities of connection
 *
 *   RETURN VALUE:   rv
 */

XtInputCallbackProc ToolCheck(   XWho, Port, ID )
Subscriber *XWho;
Socket *Port;
XtInputId *ID;

{
	Tool         *NewTool;
	struct sockaddr_in   NewToolAddr;
	Socket         NewSocket;
	int         n;
	Subscriber      *Who;
	CARD16          tmpHowMuchAuthData, tmpNumVisualIDs;

#               if DEBUGToolCheck
	fprintf( stderr, "ToolCheck()\n" );
#               endif

	XtRemoveInput( *ID );
	XtAppAddInput( GetAppCon(), *Port, XtInputReadMask, ToolCheck, XWho );


	n = sizeof(struct sockaddr_in);
	if ((NewSocket=accept( ToolSocket, &NewToolAddr, &n )) < 0 ) {
#      if DEBUGToolCheck > 1
		fprintf( stderr, "ToolCheck(): No connection\n" );
#      endif
		return FALSE;
	}

#   if DEBUGToolCheck
	fprintf( stderr, "ToolCheck(): Got connection\n" );
#   endif

	if ( !ToolExpected ) {
#      if DEBUGToolCheck
		fprintf( stderr, "ToolCheck(): Not waiting for a tool.  Refusing.\n" );
#      endif
		ConnectAndRefuse( NewSocket );
		close( NewSocket );
		return FALSE;
	}

#   if DEBUGToolCheck
	fprintf( stderr, "ToolCheck():  Tool expected.  Connecting.\n" );
#   endif
	/* xyzzy - Handle connection to tool */
#   if DEBUGToolCheck
	fprintf( stderr, "ToolCheck(): Adding new tool...\n" );
#   endif

	TempTool->InConnection = NewSocket;
	TempTool->TokenList = NULL;
	TempTool->TokenHolder = HOST_ID;
        TempTool->FindAbsoluteWindow = FALSE;
	TempTool->ArchiveBuf = NULL;
	TempTool->ArchiveSize = 0;
	TempTool->Archive2Buf = GetMem(0);
	TempTool->Archive2Size = 0;
	TempTool->SubDirtyCount = 0;
	TempTool->ThisToolDead = 0;

	if ( POLICY ) 
		TempTool->floor_timer = XtAppAddTimeOut(GetAppCon(), MinFloorTime, FloorTimeOut, TempTool); 	

	if ( ProcessHostClientConnection(TempTool) >= 0 ) {
		AddTool( TempTool );
		UpdateMessage( "Tool %s has just been connected.", TempTool->ToolCommand );
	} else {
		ToolExpected = FALSE;
		return FALSE;
	}

    if( JOIN )
	InitializeObjects(TempTool);

	if ( (TempTool->ToolID=AssignToolID(TempTool)) == NoIDAvailable ){
		FatalError( "Too many tools connected" );
#      if DEBUGToolCheck
	fprintf( stderr, "ToolCheck(): Too many tools connected\n");
#	endif
	return;
	}
	/* Inform everybody else about the new tool */

#      if DEBUGToolCheck
	fprintf( stderr, "ToolCheck(): The VisualIDList Contains %d items\n",
	    TempTool->NumVisualIDs);
	fprintf( stderr, "ToolCheck(): The VisualIDList elements are\n");
	{
		int i;
		i = TempTool->NumVisualIDs;
		while (i) printf ("%08x\n",TempTool->VisualIDList[TempTool->NumVisualIDs - i--]);
	}
#      endif

        tmpHowMuchAuthData = TempTool->HowMuchAuthData;
        tmpNumVisualIDs = TempTool->NumVisualIDs;

        TempTool->HowMuchAuthData = SwapInt(TempTool->HowMuchAuthData,FROMLOCAL(TempTool->ConnectPrefix.byteOrder));
        TempTool->NumVisualIDs = SwapInt(TempTool->NumVisualIDs,FROMLOCAL(TempTool->ConnectPrefix.byteOrder));

	insertTool(FindSubscriberByID(HOST_ID), TempTool);

	for ( Who=FirstSubscriber(); Who != NULL; Who=NextSubscriber() ) {
      if (Who->SubscriberID != 0) {

		if( JOIN && ArchiveToBeSent(Who,TempTool) )
			continue;
#      if DEBUGToolCheck
		fprintf( stderr, "ToolCheck(): Informing %s\n", Who->LoginName );
#      endif
		if(SendByte( Who->Connection, HS_AddNewTool )==ERROR){
			CleanUpSubscriber(Who);
			continue;
		}
		if(SendData( Who->Connection,  (BYTE *)(TempTool), sizeof(Tool) )==ERROR) {
			CleanUpSubscriber(Who);
                        continue;
                }

#      if DEBUGToolCheck
		fprintf( stderr, "ToolCheck():  [%d] bytes to %d\n", sizeof(Tool), Who->Connection);
#      endif

		if(SendData( Who->Connection, (BYTE *)(TempTool->AuthData), tmpHowMuchAuthData )==ERROR) {
                        CleanUpSubscriber(Who);
                        continue;
                }
#      if DEBUGToolCheck
		fprintf( stderr, "ToolCheck():  [%d] bytes to %d\n", tmpHowMuchAuthData,  Who->Connection);
#    endif

		if(SendData( Who->Connection, (BYTE *)(TempTool->VisualIDList), ((tmpNumVisualIDs) * sizeof(XID)) )==ERROR){
                        CleanUpSubscriber(Who);
                        continue;
                }

#      if DEBUGToolCheck
		fprintf( stderr, "ToolCheck():  [%d] bytes to %d\n", ((tmpNumVisualIDs) * sizeof(XID) ),
		    Who->Connection);
		fprintf( stderr, "ToolCheck(): The First VisualIDList element is: %08x\n",TempTool->VisualIDList[0]);
#      endif

#      if DEBUGToolCheck
		fprintf( stderr, "ToolCheck(): Informd %s\n", Who->LoginName );
#      endif

        }
	}

	TempTool->HowMuchAuthData = tmpHowMuchAuthData;
	TempTool->NumVisualIDs = tmpNumVisualIDs;

	ToolExpected = FALSE;

	return FALSE;   /* Always!! */
}



/* handling zombies */
sig_child()
{
int pid;
union wait	status;

Tool *WhatTool;


while ( (pid=wait3(&status, WNOHANG, (struct rusage *) 0)) > 0){
#        if DEBUGsig_child
		fprintf( stderr, "sig_child(): process(pid) %dhave terminated\n", pid);
#        endif
	for ( WhatTool=FirstTool(); WhatTool != NULL; WhatTool=NextTool() ) {
		if (WhatTool->ToolPID == pid){
#        if DEBUGsig_child
                fprintf( stderr, "sig_child(): CleaningUp Tool %s\n whose pid is %d\n", WhatTool->ToolCommand, WhatTool->ToolPID);
#        endif
		if (!CleanUpToolFlag)
			 CleanUpTool(WhatTool);

		}
	}
}
}

/*
 * Begin Murray Addition
 */

void
#ifdef __STDC__
startSubscr(Tool *tool)
#else
startSubscr(tool)
Tool *tool;
#endif
{
  if (GetLocalSubID(HOST_ID) != HOST_ID)
    return;

  start(tool->subscrList);
}

void
#ifdef __STDC__
forthSubscr(Tool *tool)
#else
forthSubscr(tool)
Tool *tool;
#endif
{
  if (GetLocalSubID(HOST_ID) != HOST_ID)
    return;

  forth(tool->subscrList);
}

int
#ifdef __STDC__
offrightSubscr(Tool *tool)
#else
offrightSubscr(tool)
Tool *tool;
#endif
{
  if (GetLocalSubID(HOST_ID) != HOST_ID)
    return 1;

  return offright(tool->subscrList);
}

void
#ifdef __STDC__
addSubscr(Tool *tool, Subscriber *newSubscr);
#else
addSubscr(tool, newSubscr)
Tool *tool;
Subscriber *newSubscr;
#endif
{
  if (GetLocalSubID(HOST_ID) != HOST_ID)
    return;

  addItem(tool->subscrList, newSubscr);
}

void
#ifdef __STDC__
deleteSubscr(Tool *tool)
#else
deleteSubscr(tool)
Tool *tool;
#endif
{
  if (GetLocalSubID(HOST_ID) != HOST_ID)
    return;

  deleteItem(tool->subscrList);
}

Subscriber *
#ifdef __STDC__
getSubscr(Tool *tool)
#else
getSubscr(tool)
Tool *tool;
#endif
{
  if (GetLocalSubID(HOST_ID) != HOST_ID)
    return (Subscriber *) 0L;

  return ((Subscriber *) getItem(tool->subscrList));
}

void
#ifdef __STDC__
findSubscr(Tool *tool, Subscriber *targetSubscr)
#else
findSubscr(tool, targetSubscr)
Tool *tool;
Subscriber *targetSubscr;
#endif
{
  if (GetLocalSubID(HOST_ID) != HOST_ID)
    return;

  for (start(tool->subscrList);
       (!offright(tool->subscrList) && (((Subscriber *) getItem(tool->subscrList)) != targetSubscr));
       forth(tool->subscrList))
    ;
}

/*
 * This is a LOCAL function that removes the PTRLIST * of
 * subscribers for the given tool. This should 
 */
void
#ifdef __STDC__
deleteAllSubscr(Tool *tool)
#else
deleteAllSubscr(tool)
Tool *tool;
#endif
{
  if (GetLocalSubID(HOST_ID) != HOST_ID)
    return;

  /*
   * This loop frees the nodes, BUT NOT the items they point to.
   */

  for (start(tool->subscrList); !offright(tool->subscrList); forth(tool->subscrList))
    deleteItem(tool->subscrList);

  /*
   * This gets rid of the list header.
   */
  free(tool->subscrList);
}

/*
 * End Murray Addition
 */

/*
 *   CloseAllTools()
 *
 *   Go through the entire list of tools and shut 'em off
 *   one at a time.
 *
 *   Last Modified: AKR 05/09 changed incrementer in the for loop to NextTool
 *                  from FirstTool.
 *   RETURN VALUE:   void
 */
void CloseAllTools()
{
        Tool   *WhichTool;


        /* Yes, we want to re-use FirstTool since we're going to change
    * the contents of the subscriber list inside the loop.
    */

        for ( WhichTool=FirstTool(); WhichTool != NULL; WhichTool=NextTool() ) {
                if ( GetMode() ) {
                        shutdown( WhichTool->InConnection );
                        XtRemoveInput( WhichTool->InID );
                        XtRemoveInput( WhichTool->InExceptID );

                }
                DeleteTool( WhichTool );
        }
}

/*
 *   CleanUpAllTools()
 *
 *   Go through the entire list of tools and shut 'em off
 *   one at a time.
 *
 *   RETURN VALUE:   void
 */
void CleanUpAllTools()
{
	Tool   *WhichTool;

	/* Yes, we want to re-use FirstTool since we're going to change
    * the contents of the subscriber list inside the loop.
    */

	for ( WhichTool=FirstTool(); WhichTool != NULL; WhichTool=NextTool() ) {
		if ( GetMode() ) {

		CleanUpTool( WhichTool );
		}
	}
}
