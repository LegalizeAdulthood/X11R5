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
  subscriber.c
  Routines to manage the subscriber list

*/

#define  DEBUGAddSubscriber    01&DEBUGON
#define  DEBUGDeleteSubscriber    01&DEBUGON
#define  DEBUGFindSubscriber    01&DEBUGON
#define  DEBUGFindSubscriberByID    01&DEBUGON
#define  DEBUGSetLocalSubID    01&DEBUGON
#define  DEBUGBuildSubscriberWidget  01&DEBUGON
#define  DEBUGRebuildSubscriberList  01&DEBUGON
#define  DEBUGFrySubscriber    01&DEBUGON
#define  DEBUGConfirmFrySubscriber    01&DEBUGON
#define  DEBUGInitializeSubscriber  01&DEBUGON
#define  DEBUGSubscriberCheck    01&DEBUGON
#define DEBUGTryToSubscribe    01&DEBUGON
#define DEBUGCleanUpSubscriber      01&DEBUGON

#define  MAXLEN 256

#include <stdio.h>
#include <pwd.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Cardinals.h>

#include "subscriber.h"
#include "memory.h"
#include "network.h"
#include "protocol.h"
#include "token.h"
#include "tool.h"

/*
 * Begin Murray Addition
 */
/*
 * These includes define the list manipulation functions for the toolList and
 * subscriber list.
 */
#include "subscriber2.h"
#include "tool2.h"
/*
 * End Murray Addition
 */
#include "popups.h"

/*
 * Begin Murray Addition
 */
#include "particInfo.h"
/*
 * End Murray Addition
 */

extern ToolNode *ToolList;
extern int JOIN;
extern int POLICY;
extern MinFloorTime;

extern DisplayNumber;
int HostDisplayNumber;

/* ------------------------------------------------------------------------------------------- */

static Subscriber  *SubscriberIDList[256];  /* List of subscribers by ID */
BYTE      LocalSubID = HOST_ID;
extern char INITIAL_MSG1[];


/*
 *  InitializeSubIDList()
 *
 *  Clear out the entire list of IDs and initialize the
 *  CurrentID value to 0.  Note that element 255 is reserved as
 *  a value for un-assignables.
 *
 *  RETURN VALUE:  void
 */
static void InitializeSubIDList()
{
	int  i;

	for ( i=0; i<256; i++ )
		SubscriberIDList[i] = NULL;
}


/*
 *  AssignSubID()
 *
 *  Hunt down an empty slot in the ID list and
 *  return its index.  This will serve as the subscriber's
 *  ID number throughout its life.
 *
 *  RETURN VALUE:  Element number (a byte)
 *      NoIDAvailable if the list is full.
 */
BYTE AssignSubID( Who )
Subscriber *Who;
{
	BYTE  i=0;

	for ( i=0; ((SubscriberIDList[i] != NULL) && (i <= NoIDAvailable )) ; i++ );

	if ( i < NoIDAvailable ) {
		SubscriberIDList[i] = Who;
	}
	return i;
}

void SetSubID( Which, Who )
BYTE Which;
Subscriber *Who;
{
	SubscriberIDList[Which] = Who;
}

/*
 *  ClearSubID()
 *
 *  Return a subscriber ID to the free pool
 *
 *  RETURN VALUE:  void
 */
void ClearSubID( Which )
BYTE Which;
{
	SubscriberIDList[Which] = NULL;
}


/*
 *  FindSubscriberByID()
 *
 *  Hunt down a subscriber by the SubscriberID field and
 *  return a pointer to the structure.
 *
 *  RETURN VALUE:  Pointer to subscriber structure
 *      NULL if not found
 */
Subscriber *FindSubscriberByID(Target)
BYTE Target;
{
	Subscriber *SubPointer;
	SubPointer = SubscriberIDList[Target];
#       if DEBUGFindSubscriberByID
	fprintf( stderr,"FindSubscriberByID(): Target is %d\n", Target);
	fprintf( stderr,"FindSubscriberByID(): LoginName is %s\n",
	    SubPointer->LoginName);
#       endif

	return (SubPointer);
}

/*
 *  SetLocalSubID()
 *
 *  Set the local subscriber ID
 *
 *  RETURN VALUE:  void
 */
void SetLocalSubID( WhoAmI )
BYTE WhoAmI;
{
#  if DEBUGSetLocalSubID
	fprintf( stderr, "SetLocalSubID(): Local ID is %d\n", WhoAmI );
#  endif
	LocalSubID = WhoAmI;
}


/*
 *  GetLocalSubID()
 *
 *  Get the local subscriber ID
 *
 *  RETURN VALUE:  Local subscriber ID
 */
BYTE GetLocalSubID( WhoAmI )
BYTE WhoAmI;
{
	return LocalSubID;
}


/* ------------------------------------------------------------------------------------------- */

SubscriberNode  *SubscriberList = NULL;
static SubscriberNode  *FindPointer = NULL;
static int    SubscriberCount = 0;

static char  *EmptyList[] = {
	"None"
};

Widget    WSubscriber;
Widget    Chairman;
Widget    WSubscriberLabel;
Widget    WSubscriberViewport;
Widget    WSubscriberList;
/*Widget    WSubscriberViewOnly; */
/*Widget    WSubscriberTalk; */
Widget    WSubscriberDrop;
Widget    WSubscriberSubscriberDrop;
/*
 * Begin Murray Addition
 */
Widget WSubscriberInfo;
/*
 * End Murray Addition
 */
static Arg  args[20];


/* ------------------------------------------------------------------------------------------- */

/*
 *  FirstSubscriber()
 *
 *  Reset FindPointer to the first subscriber in the list and return 
 *  a pointer to the structure.
 *
 *  RETURN VALUE:  Pointer to first subscriber
 *      NULL if list is empty.
 */
Subscriber *FirstSubscriber()
{
	FindPointer = SubscriberList;
	return ( FindPointer != NULL ? FindPointer->Subsc : NULL );
}


/*
 *  NextSubscriber()
 *
 *  Move FindPointer to the next subscriber in the list and return 
 *  a pointer to the structure.
 *
 *  RETURN VALUE:  Pointer to next subscriber
 *      NULL if at end of list
 */
Subscriber *NextSubscriber()
{
	if (FindPointer != NULL)
		FindPointer = FindPointer->Next;

	if ( FindPointer == NULL )
		return NULL;
	else
		return FindPointer->Subsc;
}

/*
 *
 *   UpdateChairLabel()
 *
 * Updates the Chairman label indicating who is the chairman
 * It displays the gecos as well as pw_name
 *
 * RETURN VALUE:   (void)
 *
 */
void UpdateChairLabel(Chair)
Subscriber *Chair;
{
	static char     *DefaultChairLabel = { "Chairman is : "};
	char    NewChairLabel[MAXLEN];
	Arg	arg;

	strcpy(NewChairLabel, DefaultChairLabel);
	strcat(NewChairLabel, Chair->FullName);
	strcat(NewChairLabel, " [");
        strcat(NewChairLabel, Chair->LoginName);
        strcat(NewChairLabel, "]");
        XtSetArg(arg, XtNlabel, NewChairLabel);
        XtSetValues(Chairman, &arg, 1);
	return;
}



/*
 *  AddSubscriber()
 *
 *  Add a new subscriber to the list.  Insert the new entry sorted by
 *  the LoginName field in the structure.
 *
 *  RETURN VALUE:  (void)
 */
void AddSubscriber(NewSubscriber)
Subscriber *NewSubscriber;
{
	char    *LastWord;
	SubscriberNode  *NewNode;
	SubscriberNode  *WhereAt;

#  if DEBUGAddSubscriber
	fprintf( stderr, "AddSubscriber(): Adding %s\n", NewSubscriber->LoginName );
#  endif

        if (NewSubscriber->SubscriberID == 0){
                UpdateChairLabel(NewSubscriber); /*Does not work so far */
        }

	NewNode=(SubscriberNode *)GetMem(sizeof(SubscriberNode));

	NewNode->Subsc = NewSubscriber;
	SubscriberCount++;

	/* Check to see if this is the first-ever item */

	if ( SubscriberList == NULL ) {
#    if DEBUGAddSubscriber
		fprintf( stderr, "AddSubscriber(): First entry in empty list.\n" );
#    endif
		NewNode->Next = NULL;
		SubscriberList = NewNode;
		RebuildSubscriberList();
		return;
	}

	/* Check for head-of-list */

	if ( strcmp(NewSubscriber->LoginName, SubscriberList->Subsc->LoginName ) < 0 ) {
#    if DEBUGAddSubscriber
		fprintf( stderr, "AddSubscriber(): Added at head, before %s\n",
		    SubscriberList->Subsc->LoginName );
#    endif
		NewNode->Next = SubscriberList;
		SubscriberList = NewNode;
		FindPointer = NULL;
		RebuildSubscriberList();
		return;
	}

	/* Otherwise, find the right place for it */

	for ( WhereAt=SubscriberList ; WhereAt->Next != NULL ; WhereAt=WhereAt->Next ) {
#    if DEBUGAddSubscriber
		fprintf( stderr, "AddSubscriber(): Checking %s\n", WhereAt->Next->Subsc->LoginName );
#    endif
		if ( strcmp(WhereAt->Next->Subsc->LoginName,NewSubscriber->LoginName) > 0 ) {
#    if DEBUGAddSubscriber
			fprintf( stderr, "AddSubscriber(): Stopped there.\n" );
#    endif
			break;
		}
	}

	NewNode->Next = WhereAt->Next;
	WhereAt->Next = NewNode;
	RebuildSubscriberList();
}

/*
 *  DeleteSubscriber()
 *
 *  Hunt down a subscriber and kill it.  If the entry does
 *  not exist in the list, 
 *
 *  RETURN VALUE:  void
 */
void DeleteSubscriber(Target)
Subscriber *Target;
{
	SubscriberNode  *WhereAt;
	SubscriberNode  *Temp;

#  if DEBUGDeleteSubscriber
	fprintf( stderr, "DeleteSubscriber(): Removing %s\n", Target->LoginName );
#  endif

/*
 * Begin Murray Addition
 */

/*
 * If on the host, ask all of the tools, to which Target currently subscribes,
 * to remove Target from their subscrLists.
 */
	if (GetMode())
	  {

	    Tool *tmpTool;

	    for (tmpTool=FirstTool(); tmpTool; tmpTool=NextTool())
	      {
		findSubscr(tmpTool, Target);
		if (!offrightSubscr(tmpTool))
		  deleteSubscr(tmpTool);
	      }
	  }

/*
 * End Murray Addition
 */

	RemoveFromAllTokenQs(Target);
	ClearSubID( Target->SubscriberID );

	if ( Target == SubscriberList->Subsc ) {
#    if DEBUGDeleteSubscriber
		fprintf( stderr, "DeleteSubscriber(): First in list\n" );
#    endif
		FreeMem(Target);
		Temp = SubscriberList->Next;
		FreeMem(SubscriberList);
		SubscriberList = Temp;
		SubscriberCount--;
		RebuildSubscriberList();
		return;
	}

	for ( WhereAt=SubscriberList ; WhereAt->Next != NULL ; WhereAt=WhereAt->Next ) {
#    if DEBUGDeleteSubscriber
		fprintf( stderr, "DeleteSubscriber(): Looking at %s\n",
		    WhereAt->Next->Subsc->LoginName );
#    endif
		if ( WhereAt->Next->Subsc == Target ) {
#      if DEBUGDeleteSubscriber
			fprintf( stderr, "DeleteSubscriber(): Found!\n" );
#      endif
			break;
		}
	}

	/* See if we didn't find it */

	if ( WhereAt->Next == NULL ) {
#    if DEBUGDeleteSubscriber
		fprintf( stderr, "DeleteSubscriber(): Not Found\n" );
#    endif
		return;
	}

	/* Fry the sucker! */

	Temp = WhereAt->Next->Next;
	FreeMem( WhereAt->Next->Subsc );
	FreeMem( WhereAt->Next );
	WhereAt->Next = Temp;
	SubscriberCount--;
	RebuildSubscriberList();
#    if DEBUGDeleteSubscriber
        fprintf( stderr, "DeleteSubscriber(): End of DeleteSubscriber\n" );
#    endif

}


/*
 *  FindSubscriber()
 *
 *  Hunt down a subscriber by the LoginName field and
 *  return a pointer to the structure.
 *
 *  RETURN VALUE:  Pointer to subscriber structure
 *      NULL if not found
 */
Subscriber *FindSubscriber(Target)
char *Target;
{
	SubscriberNode  *WhereAt;
	SubscriberNode  *Temp;

	for ( WhereAt=SubscriberList ; WhereAt != NULL ; WhereAt=WhereAt->Next ) {
#    if DEBUGFindSubscriber
		fprintf( stderr, "FindSubscriber(): Looking at %s\n",
		    WhereAt->Subsc->LoginName );
#    endif
		if ( !strcmp( WhereAt->Subsc->LoginName, Target ) ) {
#      if DEBUGFindSubscriber
			fprintf( stderr, "FindSubscriber(): Found!\n" );
#      endif
			break;
		}
	}

	return ( WhereAt ? WhereAt->Subsc : NULL );
}

/* ------------------------------------------------------------------------------------------- */

Socket      SubscriberSocket;
struct sockaddr_in  ListenerName;
PasswordQuery    PQuery;
Subscriber    *Selected = NULL;

/*
 *  GetHostConnection()
 *
 *  Return the connection to the host system
 *
 *  RETURN VALUE:  Socket value
 */
Socket GetHostConnection()
{
	return  SubscriberSocket;
}


/*
 *  ReuildSubscriberList()
 *
 *  Scoot through the subscriber list and build a list and build an array of pointers
 *  to the FullName field.
 *
 *  RETURN VALUE:  void
 */
void RebuildSubscriberList()
{
	static	char  **SubList = NULL;
	Subscriber  *WhereAt;
	int    n;
	int    Counter = 0;

	/* Handle an empty list by shutting everything off */

	if ( SubscriberList == NULL ) {
		if (SubList != NULL)
			FreeMem(SubList);
		n=0;
		XtSetArg( args[n],   XtNsensitive,    FALSE );
		n++;
		XtSetArg( args[n],   XtNlist,     EmptyList );
		n++;
		XtSetArg( args[n],   XtNnumberStrings,  1 );
		n++;
		XtSetValues( WSubscriberList, args, n );
/*		XtSetValues( WSubscriberTalk, args, 1 ); */
		if ( GetMode() ) {
/*			XtSetValues( WSubscriberViewOnly, args, 1 ); */
			XtSetValues( WSubscriberDrop, args, 1 );
			/*
			 * Begin Murray Addition
			 */
			XtSetValues( WSubscriberInfo, args, 1 );
			/*
			 * End Murray Addition
			 */
		}
		return;
	}

	/* Build the list */

#  if DEBUGRebuildSubscriberList
	fprintf( stderr, "RebuildSubscriberList(): %d items at %d bytes each\n",
	    SubscriberCount, sizeof(char *) );
#  endif

	SubList=(char **)GetMem( SubscriberCount * sizeof(char *) );

	for ( WhereAt=FirstSubscriber(); WhereAt != NULL; WhereAt=NextSubscriber() ) {
#    if DEBUGRebuildSubscriberList
		fprintf( stderr, "RebuildSubscriberList(): Found %s\n", WhereAt->LoginName );
#  endif

/*
 * Begin Murray Addition
 */
		{
		  char * newForm;

/*
 * Instead of just using the login name, use the login name and host name combined.
 */

		  newForm = (char *) GetMem ( strlen (WhereAt->LoginName) + strlen (WhereAt->HostName) + 2);
		  strcpy (newForm, WhereAt->LoginName);
		  strcat (newForm, "@");
		  strcat (newForm, WhereAt->HostName);
		  SubList[Counter++] = newForm;

/*
 *		  SubList[Counter++] = WhereAt->LoginName;
 */
		}
	}

	XawListChange( WSubscriberList, SubList, SubscriberCount, 0, FALSE );

/*
 * End Murray Addition
 */
	n=0;

	XtSetArg( args[n],   XtNsensitive,    TRUE );
	n++;
/*
 * Begin Murray Addition
 *
 * Fix the size of the viewport. This should technically be 2*internalMargin + rowSpacing + 2*heightFont
 *
 */
	XtSetValues( WSubscriberViewport, args, n );
/*
 * End Murray Addition
 */

/*	XtSetValues( WSubscriberViewOnly, args, n ); */
/*	XtSetValues( WSubscriberTalk, args, 1 ); */
	XtSetValues( WSubscriberDrop, args, n );
	/*
	 * Begin Murray Addition
	 */
	XtSetValues( WSubscriberInfo, args, n );
	/*
	 * End Murray Addition
	 */
	XtSetValues( WSubscriberList, args, n );
}

Subscriber *FriedSubscriber;

/* ------------------------------------------------------------------------------------------- */


/*
 *  FrySubscriber
 *
 *  Send a subscriber a HS_DroppedYou and close him out
 *
 *  RETURN VALUE:  void
 */
XtCallbackProc FrySubscriber( widget, clientData, callData )
Widget widget;
XtPointer clientData, callData;
{
	XawListReturnStruct  *Selected;
	Subscriber    *Who;
	BYTE      DeadID;
	extern 	BYTE 	WhoAmI;

	if (GetMode() == TRUE) {

		Selected = XawListShowCurrent( WSubscriberList );

		/* Bail out of nothing's highlighted */
		if ( Selected->list_index == XAW_LIST_NONE ) {
			UpdateMessage( "No participant is selected." );
#    if DEBUGFrySubscriber
	fprintf( stderr, "FrySubscriber(): Nobody selected.  Returning.\n" );
#    endif
			return;
		}

#  if DEBUGFrySubscriber
	fprintf( stderr, "FrySubscriber(): Cooking %s\n", Selected->string );
#  endif

		for (Who=FirstSubscriber();
		     Who && !CompareSubscrNameHost(Who, Selected->string);
		     Who=NextSubscriber())
		  ;
/*
 *		if ( (Who=FindSubscriber(Selected->string)) == NULL ) {
 */
		if (Who == NULL) {
			FatalError( "INTERNAL ERROR: List widget inconsistent with subscriber list" );

#  if DEBUGFrySubscriber
	fprintf( stderr, "FrySubscriber(): Can't find info about: %s\n", Selected->string );
#  endif
		return;
		}

/* just for testing, assume SubscriberID is 1 
		Who = FindSubscriberByID(1);
*/
		/* Cook him, but save a copy of his ID number */
		DeadID = Who->SubscriberID;
		if (Who->SubscriberID == HOST_ID) {
			UpdateMessage("Use (Exit XTV) to drop yourself !!\n");
			return;
		}

		FriedSubscriber = Who;

        	XtSetSensitive(WSubscriber, FALSE);
        	PopUpBox ( widget, TranShell[DropPart], callData );
	}
	else 
		UpdateMessage("Remote users can't drop participants !!\n");
}

XtCallbackProc CancelFrySubscriber (w, client_data, call_data)
Widget   w;
caddr_t client_data, call_data;

{
        XtPopdown(TranShell[DropPart]);
        XtSetSensitive(WSubscriber, TRUE);
}

XtCallbackProc ConfirmFrySubscriber (w, client_data, call_data)
Widget   w;
caddr_t client_data, call_data;

{

#       if DEBUGConfirmFrySubscriber
        fprintf( stderr, "ConfirmFrySubscriber(): Starting\n");
#       endif

        XtPopdown(TranShell[DropPart]);
        XtSetSensitive(WSubscriber, TRUE);

        if (SendByte( FriedSubscriber->Connection, HS_DroppedYou )==ERROR) CleanUpSubscriber(FriedSubscriber);
        if (SendByte( FriedSubscriber->Connection, FriedSubscriber->SubscriberID )==ERROR) CleanUpSubscriber(FriedSubscriber);
#       if DEBUGConfirmFrySubscriber
                fprintf(stderr,"ConfirmFrySubscriber(): sent byte HS_DroppedYou\n");
#       endif
        CleanUpSubscriber(FriedSubscriber, 1);

}



/* ------------------------------------------------------------------------------------------- */

/*
 * UpdateLabel()
 *
 *
 *
 *
 */

char UpdateLabel(w, SessionNumber)
Widget  w;
int     SessionNumber;
{

                char    *SubscriberLabel;
                char    NewLabel[MAXLEN];
                static char     *Session = {" "};
                Arg     arg;

/* Update Subscriber label widget with Session Number */

      Session[0] = SessionNumber + 48;
      XtSetArg(arg, XtNlabel, &SubscriberLabel);
      XtGetValues(w, &arg, 1);
      strcpy(NewLabel, SubscriberLabel);
      strcat(NewLabel, " ");
      strcat(NewLabel, Session);
      XtSetArg(arg, XtNlabel, NewLabel);
      XtSetValues(w, &arg, 1);
      return(Session[0]);

}


/* ------------------------------------------------------------------------------------------- */

/*
 *  InitializeSubscriber()
 *
 *  Initialize all work processes related to subscribers and
 *  build the widgets
 *
 *  RETURN VALUE:  void
 */
Widget InitializeSubscriber( AppCon, Parent, Mode, SessionFromArgv )
XtAppContext AppCon;
Widget Parent;
Bool Mode;
char *SessionFromArgv;
{
	int     n, SubSessionNumber;
	int     TryPort, SessionNumber;
	struct  passwd  *UserPasswd;
        char    HostName[HOST_NAME_SIZE];
        struct  hostent *hp, *gethostbyname();
        struct  sockaddr_in server;
	static char    Session[2] = {" "};
	Subscriber     *IAmSubscriber;
	int     dotIndex, nameLength;

#  if DEBUGInitializeSubscriber
	fprintf( stderr, "InitializeSubscriber()\n" );
#  endif

	InitializeSubIDList();

	Chairman = XtCreateManagedWidget( "Chairman", labelWidgetClass, Parent, NULL, 0 );
	WSubscriber = XtCreateManagedWidget( "PartPanel", formWidgetClass, Parent, NULL, 0 );
	WSubscriberLabel = XtCreateManagedWidget( "Label", labelWidgetClass, WSubscriber, NULL, 0 );
	WSubscriberViewport = XtCreateManagedWidget( "VPort", viewportWidgetClass, WSubscriber, NULL,0);
	WSubscriberList = XtCreateManagedWidget( "PartList", listWidgetClass, WSubscriberViewport, NULL, 0 );
	XawListChange( WSubscriberList, EmptyList, 1, 0, TRUE );

	/*WSubscriberViewOnly = XtCreateManagedWidget( "ViewOnly", commandWidgetClass, WSubscriber, NULL, 0);*/
	/*WSubscriberTalk = XtCreateManagedWidget( "Talk", commandWidgetClass, WSubscriber, NULL, 0);*/
	/*
	 * Begin Murray Addition
	 */
	WSubscriberInfo = XtCreateManagedWidget( "InfoPart", commandWidgetClass, WSubscriber, NULL, 0);
	XtAddCallback( WSubscriberInfo, XtNcallback, particInfo, NULL );
	/*
	 * End Murray Addition
	 */
	WSubscriberDrop = XtCreateManagedWidget( "DropPart", commandWidgetClass, WSubscriber, NULL, 0);
	XtAddCallback( WSubscriberDrop, XtNcallback, FrySubscriber,TranShell[DropPart] );

	if ( Mode ) {
#    if DEBUGInitializeSubscriber
		fprintf( stderr, "InitializeSubscriber(): In host mode.  Opening port.\n" );
#    endif
		TryPort= XTV_SUBSCRIBER_LOW_PORT;
		do {
			SubscriberSocket=OpenListener( htons(TryPort), 4, &ListenerName );
		}  while ( (SubscriberSocket < 0) && ( TryPort++ <= XTV_SUBSCRIBER_HIGH_PORT) );
		if (TryPort > XTV_SUBSCRIBER_HIGH_PORT) {
			FatalError("Unable to find an available port.");
#   if DEBUGInitializeSubscriber
		fprintf( stderr, "InitializeSubscriber(): Can't find Subscriber port (TryPort is %d)\n", TryPort );
#   endif
			exit(1);
		}
		XtAppAddInput( GetAppCon(), SubscriberSocket, XtInputReadMask,
		    SubscriberCheck, NULL );

#   if DEBUGInitializeSubscriber
		fprintf( stderr, "InitializeSubscriber(): Subscriber port open at %d\n", TryPort );
#   endif
                if ( gethostname( HostName, HOST_NAME_SIZE ) < 0 ) {
                        FatalError( "Unable to get host name" );
#   if DEBUGInitializeSubscriber
		fprintf( stderr, "InitializeSubscriber(): Can't find host name\n");
#   endif
		exit;
                }
/* special for the IBM rs6000 xtv4, since it belongs to concert.net domain
while the default is cs.unc.edu! */

if (strcmp(HostName, "xtv4:0.0")==0){
               putenv( "DISPLAY=xtv4.concert.net:0.0" );
}
   if  ( (hp = gethostbyname(HostName)) == NULL ) {
#   if DEBUGInitializeSubscriber
                fprintf( stderr, "InitializeSubscriber(): Can't find entry for host %s\n", HostName);
#   endif
                        FatalError( "Unable to get host entry\n" );
    			exit(1);
   }

/*	Now, try to add yourself to subscriber list        */

	IAmSubscriber = (Subscriber *) malloc(sizeof(Subscriber));
	UserPasswd = getpwuid(getuid());
	strcpy(IAmSubscriber->FullName, UserPasswd->pw_gecos);
	strcpy(IAmSubscriber->LoginName, UserPasswd->pw_name);
	if ((IAmSubscriber->SubscriberID = AssignSubID(IAmSubscriber)) == NoIDAvailable){
		FatalError("Cannot get a Subscriber ID");
		exit(1);
	}

/*
 * Begin Murray Addition
 */

	IAmSubscriber->toolList = ptrNew();
	nameLength = strlen(HostName);
	for (dotIndex = 0; (dotIndex < nameLength) && (HostName[dotIndex] != '.'); dotIndex++)
	  ;
        if (dotIndex < nameLength)
	  {
	    strncpy(IAmSubscriber->HostName, HostName, dotIndex);
	    IAmSubscriber->HostName[dotIndex] ='\0';
	  }
	else
	  strcpy(IAmSubscriber->HostName, HostName);

/*
 * End Murray Addition
 */

	AddSubscriber(IAmSubscriber);

/*	Also, update the Chairman label widget to indicate who is the chairman 	*/

	UpdateChairLabel(IAmSubscriber);

		SessionNumber = TryPort-XTV_SUBSCRIBER_LOW_PORT+1;

        Session[1] = UpdateLabel(WSubscriberLabel, SessionNumber);

        strcat(INITIAL_MSG1, "              *****");
	strcat(INITIAL_MSG1, "\nLet each Participant type: \n                xtv ");
	strcat(INITIAL_MSG1, HostName);
	strcat(INITIAL_MSG1, " ");
	strcat(INITIAL_MSG1, Session);
	strcat(INITIAL_MSG1, " \n             OR xtv ");
	bcopy ( hp->h_addr, &(server.sin_addr), hp->h_length);
	strncat(INITIAL_MSG1, inet_ntoa(server.sin_addr), 15);
	strcat(INITIAL_MSG1, Session);
	strcat(INITIAL_MSG1, "\n");

		fcntl( SubscriberSocket, F_SETFL, FNDELAY );

#    if DEBUGInitializeSubscriber
		fprintf( stderr, 
                         "InitializeSubscriber(): User is %s <%s>\n", 
                          UserPasswd->pw_gecos, UserPasswd->pw_name );
#    endif
	}
	else {
	  if (gethostname( HostName, HOST_NAME_SIZE ) < 0)
	    fprintf(stderr, "Unable to obtain host name.\n\r");
	  SubSessionNumber = atoi(SessionFromArgv);

#if DEBUGInitializeSubscriber
	  fprintf( stderr, 
		  "InitializeSubscriber(): SubSessionNumber %d\n",
		  SubSessionNumber);
#endif

	  Session[1] = UpdateLabel(WSubscriberLabel, SubSessionNumber);
	  
	  strcat(INITIAL_MSG1, "                 *****");
	  strcat(INITIAL_MSG1, "\nI am a Participant of host ");
	  strcat(INITIAL_MSG1, HostName);
	  strcat(INITIAL_MSG1, "\n");
	}

#  if DEBUGInitializeSubscriber
	fprintf( stderr, "InitializeSubscriber(): Done.\n" );
#  endif

	return WSubscriber;
}

/* ------------------------------------------------------------------------------------------- */

Subscriber    LocalInfo;

/*
 *  SubscriberCheck();
 *
 *  Check for a new subscriber and handle all of the formalities of connection
 *
 *  RETURN VALUE:  Always FALSE to keep Xt calling us
 */

XtInputCallbackProc SubscriberCheck(  XWho, Port, ID )
Subscriber *XWho;
Socket *Port;
XtInputId *ID;

{
	Subscriber    *NewSubscriber;
	Subscriber    *Who;
	struct sockaddr NewSubAddr;
	Socket      NewSocket;
	int      n = sizeof(struct sockaddr);
	unsigned long    *MagicNumber;
        BYTE            list[100];
        ToolNode        *tnode;
        int     i;

#    if DEBUGSubscriberCheck 
	fprintf( stderr, "SubscriberCheck()\n" );
#    endif

	XtRemoveInput( *ID );
	XtAppAddInput( GetAppCon(), *Port, XtInputReadMask, SubscriberCheck, XWho );

	if ((NewSocket=accept( SubscriberSocket, &NewSubAddr, &n )) < 0 ) {
#    if DEBUGSubscriberCheck 
		fprintf( stderr, "SubscriberCheck(): No Connection\n" );
#    endif

		return FALSE;
	}

#  if DEBUGSubscriberCheck
	fprintf( stderr, "SubscriberCheck(): Got connection\n" );
#  endif


	MagicNumber=(unsigned long *)ReadData( NewSocket, sizeof(unsigned long), TRUE );
	if ( *MagicNumber ==  XTV_CONNECT_MAGIC || *MagicNumber == R_XTV_CONNECT_MAGIC ) {
		SendByte( NewSocket, (BYTE)01 );
#        if DEBUGSubscriberCheck
		fprintf( stderr, "SubscriberCheck(): Waiting for data...\n" );
#        endif
		NewSubscriber=(Subscriber *)ReadData( NewSocket,sizeof(Subscriber), TRUE );

#        if DEBUGSubscriberCheck
		fprintf( stderr, "SubscriberCheck(): Adding new user...\n" );
#        endif

		NewSubscriber->Connection = NewSocket;
		if ( (NewSubscriber->SubscriberID=AssignSubID(NewSubscriber)) == NoIDAvailable ){


#        if DEBUGSubscriberCheck
		fprintf( stderr, "SubscriberCheck(): Adding new user...Too many subscribers are connected\n" );
#        endif
			FatalError( "Too many subscribers connected" );
			return;
		}

		/* Before we add the newcomer, tell everybody else */

#        if DEBUGSubscriberCheck
		fprintf( stderr, "SubscriberCheck(): ----- Telling everybody\n" );
#        endif
		for ( Who=FirstSubscriber(); Who != NULL; Who=NextSubscriber() ) {
#          if DEBUGSubscriberCheck
			fprintf( stderr, "SubscriberCheck(): Sending  HS_AddNewSubscriber [%d] to %s\n",
			    HS_AddNewSubscriber, Who->LoginName );
#          endif
        if (Who->SubscriberID != 0) {

			if (SendByte( Who->Connection, HS_AddNewSubscriber )==ERROR) CleanUpSubscriber(Who);
			if (SendData( Who->Connection, NewSubscriber, sizeof(Subscriber) )==ERROR) CleanUpSubscriber(Who);
        }
		}

		AddSubscriber( NewSubscriber );

/*
 * Begin Murray Addition
 */

		if (SendByte( NewSubscriber->Connection, HS_IncomingTools ) == ERROR )
		  CleanUpSubscriber(NewSubscriber);
		if (SendByte( NewSubscriber->Connection, (BYTE) items(FindSubscriberByID(HOST_ID)->toolList)) == ERROR ) 
		  CleanUpSubscriber(NewSubscriber);
		for (startTool(FindSubscriberByID(HOST_ID)); !offrightTool(FindSubscriberByID(HOST_ID)); forthTool(FindSubscriberByID(HOST_ID)))
		  {
		    if (SendByte( NewSubscriber->Connection, (BYTE) strlen(getTool(FindSubscriberByID(HOST_ID))->ToolCommand)) == ERROR )
		      CleanUpSubscriber(NewSubscriber);
		    if (SendData( NewSubscriber->Connection,
				 getTool(FindSubscriberByID(HOST_ID))->ToolCommand,
				 (BYTE) strlen(getTool(FindSubscriberByID(HOST_ID))->ToolCommand)) == ERROR)
		      CleanUpSubscriber(NewSubscriber);
		  }

/*
 * End Murray Addition
 */

#        if DEBUGSubscriberCheck
		fprintf( stderr, "SubscriberCheck(): ----- Sending to new guy\n" );
#        endif

		/* Tell the newcomer his subscriber ID */

		if (SendByte( NewSocket, HS_YouAre )==ERROR) CleanUpSubscriber(NewSubscriber);
		if (SendByte( NewSocket, NewSubscriber->SubscriberID )==ERROR) CleanUpSubscriber(NewSubscriber);
/* send Display Number of Chairman to subscriber to start remote tools */
                if (SendByte( NewSocket, DisplayNumber)==ERROR) CleanUpSubscriber(NewSubscriber);


		/* Send new subscriber a complete subscriber list, himself included */

		for ( Who=FirstSubscriber(); Who != NULL; Who=NextSubscriber() ) {
#          if DEBUGSubscriberCheck
			fprintf( stderr, "SubscriberCheck(): Sending [%d] %s\n",
			    HS_AddNewSubscriber, Who->LoginName );
#          endif
			if (SendByte( NewSocket, HS_AddNewSubscriber )==ERROR) CleanUpSubscriber(NewSubscriber);
			if (SendData( NewSocket, Who, sizeof(Subscriber) )==ERROR) CleanUpSubscriber(NewSubscriber);
		}
#        if DEBUGSubscriberCheck
		fprintf( stderr, "SubscriberCheck(): ----- Done\n" );
#        endif

                for( tnode = ToolList, i = 0; tnode != NULL;
                        tnode = tnode->Next, i++)
                        list[i] = tnode->ToolS->ToolID;

                if( i > 0 && JOIN )
                        InitiateCatchUp(NewSubscriber,NewSocket,i,list);

		/* Put in a callback for socket input */
		NewSubscriber->InputCallbackID = XtAppAddInput( GetAppCon(),
		    NewSocket, XtInputReadMask, HandleStoHProtocol, NewSubscriber );

		UpdateMessage( "%s has just joined the session.",NewSubscriber->LoginName);

/*
 * Begin Murray Addition
 */

		NewSubscriber->toolList = ptrNew();

/*
 * End Murray Addition
 */

	}

	FreeMem( MagicNumber );
	return FALSE;  /* Always!! */
}

InitiateCatchUp(NewSubscriber,NewSocket,num,list)
Subscriber *NewSubscriber;
Socket NewSocket;
int num;
BYTE list[];
{
        ToolNode *tnode;
        ToolNode *Prev;
        int i;

        Prev = NULL;
        for( i=num-1; i>=0; i-- ){
                tnode = (ToolNode *) GetMem(sizeof(ToolNode));
                if( (tnode->ToolS = FindToolByID(list[i])) == NULL ){
                        FreeMem(tnode);
                        continue;
                }
                tnode->Next = Prev;
                Prev = tnode;
        }
        if( NewSubscriber->CurTool != NULL ){
                /* find the end of the CatchUpList */
                for( tnode = NewSubscriber->CurTool; tnode->Next != NULL; tnode = tnode->Next );
                tnode->Next = Prev;
                return;
        }
        NewSubscriber->CatchUpList = Prev;
        NewSubscriber->CurTool = Prev;
        NewSubscriber->CurPos = 0;
        if( ((ToolNode *)(NewSubscriber->CurTool))->ToolS->SubDirtyCount > 0 ){
                if( ((ToolNode *)(NewSubscriber->CurTool))->ToolS->ArchiveSize == 0 ){
                        fprintf(stderr,"InitiateCatchUp: 0 length archive\n");
                        exit(1);
                }
        }
        else{
                PreparePacketsToSend(((ToolNode*)
                        (NewSubscriber->CurTool))->ToolS);
        }
        ++((ToolNode *)(NewSubscriber->CurTool))->ToolS->SubDirtyCount;
        NewSubscriber->ExposeWaitState = 0;
        NewSubscriber->ExposeList = NULL;
        CreateExposeList(((ToolNode *)(NewSubscriber->CurTool))->ToolS,&(NewSubscriber->ExposeList));
        if (SendByte( NewSocket, HS_CatchUpStart )==ERROR) CleanUpSubscriber(NewSubscriber);
        STOH_SendArchivePacket(NewSocket,NewSubscriber);
}

/*
 *  TryToSubscribe()
 *
 *  Connect to XTV at another host and negotiate the connection and
 *  password formalities.
 *
 *  RETURN VALUE:  File descriptor for connection
 *      -1 otherwise.
 */
Socket TryToSubscribe( HostName, SessionNumber )
char *HostName;
int SessionNumber;
{
	Socket    connection;
	struct passwd  *UserPasswd;
	unsigned long  Temp;
	BYTE    *TempByteP;
	int    i;
        static char Session[2] = {" "};

/*
 * Begin Murray Addition
 */
	char myHostName[HOST_SIZE];
/*
 * End Murray Addition
 */


#  if DEBUGTryToSubscribe
	fprintf( stderr, "TryToSubscribe()\n" );
#  endif

	UserPasswd = getpwuid(getuid());
#  if DEBUGTryToSubscribe
	fprintf( stderr, "TryToSubscribe(): User is %s <%s>\n", UserPasswd->pw_gecos,
	    UserPasswd->pw_name );
#  endif
	strcpy( LocalInfo.FullName,  UserPasswd->pw_gecos  );
	strcpy( LocalInfo.LoginName,  UserPasswd->pw_name  );

/*
 * Begin Murray Addition
 */

	if (gethostname(myHostName, HOST_NAME_SIZE) < 0)
	  FatalError( "Unable to get host name" );

	if (strchr(myHostName, '.'))
	  {
	    /*
	     * If there is a dot, then we need to copy up to, but not including the dot.
	     *
	     * Then append a NULL to the end of the name.
	     *
	     * This has to be done by hand, since rindex is not in the ANSI standard.
	     */

	    int dotIndex, nameLen = strlen(myHostName);
	    for (dotIndex = 0; (dotIndex < nameLen) && (myHostName[dotIndex] != '.'); dotIndex++)
	      ;
	    strncpy( LocalInfo.HostName, myHostName, dotIndex);
	    LocalInfo.HostName[dotIndex] = '\0';
	  }
	else
	  strcpy( LocalInfo.HostName, myHostName);

/*
 * End Murray Addition
 */

	if ((connection = ConnectTo( HostName, htons(XTV_SUBSCRIBER_LOW_PORT+SessionNumber ))) < 0 ) {
#  if DEBUGTryToSubscribe
		fprintf( stderr, "TryToSubscribe(): Fail to Connect to %s:%d\n", HostName, 
		    XTV_SUBSCRIBER_LOW_PORT+SessionNumber);
#  endif
		return -1;

	}
#  if DEBUGTryToSubscribe
	fprintf( stderr, "TryToSubscribe(): Connected to %s:%d\n", HostName, 
	    XTV_SUBSCRIBER_LOW_PORT+SessionNumber);
#  endif

	Temp = XTV_CONNECT_MAGIC;
	if (SendData( connection, &Temp, sizeof( Temp ) )==ERROR) EndSession();
#      if DEBUGTryToSubscribe
	fprintf( stderr, "TryToSubscribe(): Sent  XTV_CONNECT_MAGIC \n" );
	fprintf( stderr, "TryToSubscribe(): Waiting for reply\n" );
#      endif
	TempByteP=(BYTE *)ReadData( connection, sizeof(BYTE), TRUE );
	if (*TempByteP) {
		if (SendData( connection, &LocalInfo, sizeof( LocalInfo ) )==ERROR) EndSession();
		FreeMem( TempByteP );
#        if DEBUGTryToSubscribe
		fprintf( stderr, "TryToSubscribe(): Accepted.\n" );
#        endif
		XtAppAddInput( GetAppCon(), connection, XtInputReadMask,
		    HandleHtoSProtocol, NULL );
		SubscriberSocket = connection;
		return connection;
	}
	return -1;
}



/* ----------------------------------------------------------------------------------------- */
/* 
 *      CleanUpSubscriber()
 *       
 *
 */ 
void CleanUpSubscriber(Who, LeftOrEjected)
     Subscriber *Who;
     BYTE LeftOrEjected;
{
  Tool                *WhatTool;
  BYTE                NextID;
  Subscriber  *WhoAt;
  Subscriber  *NextVictim;
  BYTE OldTokenHolder;
  
  BYTE DeadID;
  DeadID = Who->SubscriberID;
  
#if DEBUGCleanUpSubscriber
  fprintf( stderr, "CleanUpSubscriber(): Deleting subscriber: %d\n", DeadID );
#endif
  
 UpdateMessage( "%s has %s the session", Who->LoginName, ( LeftOrEjected  ? "been ejected from" : "left" ) );

  
  
  XtRemoveInput(Who->InputCallbackID);
  close( Who->Connection );
  
  DeleteSubscriber( Who );
  
#if DEBUGCleanUpSubscriber
  fprintf( stderr, "CleanUpSubscriber(): Deleted subscriber: %d\n",
	  DeadID );
#endif
  
  /* Send everybody the bad news. */
  
  for ( Who=FirstSubscriber(); Who != NULL; Who=NextSubscriber() ){
    if (Who->SubscriberID != 0) {
      if (SendByte( Who->Connection, HS_DeleteSubscriber )==ERROR) CleanUpSubscriber(Who);
      if (SendByte( Who->Connection, DeadID )==ERROR) CleanUpSubscriber(Who);
      if (SendByte( Who->Connection, LeftOrEjected )==ERROR) CleanUpSubscriber(Who);

#if DEBUGCleanUpSubscriber
      fprintf( stderr, "CleanUpSubscriber(): Informed %d subscriber about the death of %d\n", Who->SubscriberID, DeadID);
#endif

    }
  }

#if DEBUGCleanUpSubscriber
  fprintf( stderr, "CleanUpSubscriber(): Informed all subscribers about the death of %d\n", DeadID);
#endif
  
  
  /* reclaim tokens and ownership of all tools that the 
     dead subscriber hold */
  
  for ( WhatTool=FirstTool(); WhatTool != NULL; WhatTool=NextTool() ) { 
    OldTokenHolder = WhatTool->TokenHolder;
    if ( WhatTool->TokenHolder == DeadID) {
      /*
       * Make sure that the next token holder is not the dead one.
       */

      if ( ((NextVictim=GetNextToken(WhatTool)) != NULL ) && (NextVictim->SubscriberID != DeadID))
	NextID = NextVictim->SubscriberID;
      else NextID = HOST_ID;
      WhatTool->TokenHolder = NextID;
    }
    NextID = WhatTool->TokenHolder;

#if DEBUGCleanUpSubscriber
    fprintf( stderr, "CleanUpSubscriber(): %d is the (new) token holder for tool %s\n", WhatTool->TokenHolder, WhatTool->ToolCommand);
#endif
    
    if ( WhatTool->ToolCreator == DeadID) 
      WhatTool->ToolCreator = HOST_ID; 

#if DEBUGCleanUpSubscriber
    fprintf( stderr, "CleanUpSubscriber(): %d is the (new) tool creator for tool %s\n", WhatTool->ToolCreator, WhatTool->ToolCommand);
#endif
    
    
    if (POLICY == 1){
      XtRemoveTimeOut(WhatTool->floor_timer);
      WhatTool->floor_timer = XtAppAddTimeOut(GetAppCon(), MinFloorTime, 
					      FloorTimeOut, WhatTool);
    }
    
    
    if (OldTokenHolder == NextID){
#if DEBUGCleanUpSubscriber
      fprintf( stderr, "CleanUpSubscriber(): No change in token holder\n");
#endif

/*
 * Begin Murray Addition
 *
 *    return;
 *
 * End Murray Addition
 */
    }
    
    /* Tell everybody the good news if there is change in token holder */
    
    if( (NextVictim = FindSubscriberByID(NextID))==NULL){

#if DEBUGCleanUpSubscriber
      fprintf( stderr, "CleanUpSubscriber(): Can't find subscriber %d\n ", NextID);
#endif

      return;
    }
    
    UpdateMessage( "%s has the token\n for tool %s.", ( NextID == HOST_ID ? "The chairman (that's you)" : NextVictim->LoginName), WhatTool->ToolCommand);
    
    for ( WhoAt=FirstSubscriber(); WhoAt != NULL; WhoAt=NextSubscriber()) {
      if (WhoAt->SubscriberID != 0) {
	if (SendByte( WhoAt->Connection, HS_TakeToken )==ERROR)CleanUpSubscriber(WhoAt);
	if (SendByte( WhoAt->Connection, WhatTool->ToolID )==ERROR)CleanUpSubscriber(WhoAt);
	if (SendByte( WhoAt->Connection, NextID )==ERROR)CleanUpSubscriber(WhoAt);
      }
    }

#if DEBUGCleanUpSubscriber
    fprintf( stderr, "CleanUpSubscriber(): Informed other subscribers about token status of tool%d\n", WhatTool->ToolCommand);
#endif

  }
}

/*
 * Begin Murray Addition
 */

Subscriber *
#ifdef __STDC__
FindSubscriberByNameHost(char *nameHost)
#else
FindSubscriberByNameHost(nameHost)
char *nameHost;
#endif
{
  Subscriber *thisSubscribe;
  int found = FALSE;
  int thisChar, logLen = strlen(thisSubscribe->LoginName), hostLen = strlen(thisSubscribe->HostName);
  int nameHostLen = strlen(nameHost);

  thisSubscribe = FirstSubscriber();
  while (thisSubscribe && !found)
    {
      for (thisChar = 0; (thisChar < logLen) && (thisSubscribe->LoginName[thisChar] == nameHost[thisChar]); thisChar++)
	;

      if (thisChar < logLen)
	found = TRUE;

      for (thisChar++ ; !found && (thisChar < nameHostLen) && (thisSubscribe->HostName[(thisChar - logLen - 1)] == nameHost[thisChar]); thisChar++)
	;

      if (thisChar < nameHostLen)
	found = TRUE;

      if (found)
	thisSubscribe = NextSubscriber();

      if (found)
	found = FALSE;
      else
	found = TRUE;
    }

  return thisSubscribe;
}


int
#ifdef __STDC__
CompareSubscrNameHost(Subscriber *thisSubscribe, char *nameHost)
#else
CompareSubscrNameHost(thisSubscribe, nameHost)
Subscriber *thisSubscribe;
char *nameHost;
#endif
{
  int found = FALSE;
  int thisChar, logLen = strlen(thisSubscribe->LoginName), hostLen = strlen(thisSubscribe->HostName);
  int nameHostLen = strlen(nameHost);

  for (thisChar = 0; (thisChar < logLen) && (thisSubscribe->LoginName[thisChar] == nameHost[thisChar]); thisChar++)
    ;

  if (thisChar < logLen)
    found = TRUE;

  for (thisChar++ ; !found && (thisChar < nameHostLen) && (thisSubscribe->HostName[(thisChar - logLen - 1)] == nameHost[thisChar]); thisChar++)
    ;

  if (thisChar < nameHostLen)
    found = TRUE;

  return !found;
}

void
#ifdef __STDC__
startTool(Subscriber *subscriber)
#else
startTool(subscriber)
Subscriber *subscriber;
#endif
{
  if (subscriber->SubscriberID != HOST_ID)
    return;

  start(subscriber->toolList);
}

void
#ifdef __STDC__
forthTool(Subscriber *subscriber)
#else
forthTool(subscriber)
Subscriber *subscriber;
#endif
{
  if (GetLocalSubID(HOST_ID) != HOST_ID)
    return;

  forth(subscriber->toolList);
}

int
#ifdef __STDC__
offrightTool(Subscriber *subscriber)
#else
offrightTool(subscriber)
Subscriber *subscriber;
#endif
{
  if (GetLocalSubID(HOST_ID) != HOST_ID)
    return 1;

  return offright(subscriber->toolList);
}

void
#ifdef __STDC__
addTool(Subscriber *subscriber, Tool *tool)
#else
addTool(subscriber, tool)
Subscriber *subscriber;
Tool *tool;
#endif
{
  if (GetLocalSubID(HOST_ID) != HOST_ID)
    return;

  addItem(subscriber->toolList, (void *) tool);
}

void
#ifdef __STDC__
insertTool(Subscriber *subscriber, Tool *tool)
#else
insertTool(subscriber, tool)
Subscriber *subscriber;
Tool *tool;
#endif
{
  saveCursor(subscriber->toolList);

  for (start(subscriber->toolList);
       !offright(subscriber->toolList) && 
       (strcmp(((Tool *) getItem(subscriber->toolList))->ToolCommand, tool->ToolCommand) < 0);
       forth(subscriber->toolList))
    ;
  if (!offright(subscriber->toolList) && (strcmp(((Tool *) getItem(subscriber->toolList))->ToolCommand, tool->ToolCommand) > 0))
    back(subscriber->toolList);
  addItem(subscriber->toolList, (void *) tool);
  restoreCursor(subscriber->toolList);
}

void
#ifdef __STDC__
deleteTool(Subscriber *subscriber)
#else
deleteTool(subscriber)
Subscriber *subscriber;
#endif
{
  if (GetLocalSubID(HOST_ID) != HOST_ID)
    return;

  deleteItem(subscriber->toolList);
}

Tool *
#ifdef __STDC__
getTool(Subscriber *subscriber)
#else
getTool(subscriber)
Subscriber *subscriber;
#endif
{
  if (GetLocalSubID(HOST_ID) != HOST_ID)
    return (Tool *) 0L;

  return ((Tool *) getItem(subscriber->toolList));
}

void
#ifdef __STDC__
findTool(Subscriber *subscriber, Tool *targetTool)
#else
findTool(subscriber, targetTool)
Subscriber *subscriber;
Tool *targetTool;
#endif
{
  if (GetLocalSubID(HOST_ID) != HOST_ID)
    return;

  for (start(subscriber->toolList);
       (!offright(subscriber->toolList) && (((Tool *) getItem(subscriber->toolList)) != targetTool));
       forth(subscriber->toolList))
    ;
}

void
#ifdef __STDC__
deleteAllTool(Subscriber *subscriber)
#else
deleteAllTool(subscriber)
Subscriber *subscriber;
#endif
{
  if (GetLocalSubID(HOST_ID) != HOST_ID)
    return;

  /*
   * This loop frees the nodes, BUT NOT the items they point to.
   */
  for (start(subscriber->toolList); !offright(subscriber->toolList); forth(subscriber->toolList))
    deleteItem(subscriber->toolList);

  free(subscriber->toolList);
}

Tool *
#ifdef __STDC__
lastTool(Subscriber *subscriber)
#else
lastTool(subscriber)
Subscriber *subscriber;
#endif
{
  return last(subscriber->toolList);
}

/*
 * End Murray Addition
 */
