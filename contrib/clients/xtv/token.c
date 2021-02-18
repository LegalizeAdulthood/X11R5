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
	token.c
	Routines for managing the token stacks for each tool

*/
#include <stdio.h>
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

#include "subscriber.h"
#include "memory.h"
#include "network.h"
#include "protocol.h"
#include "token.h"
#include "tool.h"

#define	DEBUGAddToTokenList	01&DEBUGON
#define	DEBUGGetNextToken	01&DEBUGON
#define DEBUGRemoveFromOneTokenQ 	01&DEBUGON
#define DEBUGRemoveFromAllTokenQs 	01&DEBUGON
#define DEBUGGiveTokenTo 	01&DEBUGON
#define   DEBUGSwitchToken    10&DEBUGON
#define   DEBUGGetToken      10&DEBUGON
#define   DEBUGSnatchToken      10&DEBUGON
#define   DEBUGDropToken      10&DEBUGON
#define DEBUGFloorTimeOut 	01&DEBUGON
#define DEBUGGiveTokenToCreator 01&DEBUGON
 

extern int POLICY;
extern int MinFloorTime;

extern Widget WToolList;

/*
 *	AddToTokenList()
 *
 *	Add a subscriber to the list of people who want the token.  If
 *	an entry for that subscriber already exists, toss it in the can.
 *
 *	RETURN VALUE:	void
 */
void AddToTokenList( WhatTool, Who )
Tool *WhatTool;
Subscriber *Who;
{
	TokenNode	*Where;
	TokenNode	*Last;
	TokenNode	*NewNode;

#	if DEBUGAddToTokenList
	fprintf( stderr, "AddToTokenList(): Adding %d\n", Who->SubscriberID );
#	endif
	for ( Where=WhatTool->TokenList; Where != NULL; Where=Where->Next ) {

		Last = Where;

		if ( Who->SubscriberID == Where->SubID ) {
#			if DEBUGAddToTokenList
			fprintf( stderr, "AddToTokenList(): ...Already there.\n" );
#			endif
			return;
		}
	}

	NewNode = (TokenNode *)GetMem(sizeof(TokenNode));
	NewNode->SubID = Who->SubscriberID;
	NewNode->Next = NULL;

	if ( WhatTool->TokenList == NULL ) {
		WhatTool->TokenList = NewNode;
	} else {
		Last->Next = NewNode;
	}
#	if DEBUGAddToTokenList
	fprintf( stderr, "AddToTokenList(): THE LIST CONTAINS:" );
	for ( Where=WhatTool->TokenList; Where != NULL; Where=Where->Next ) {
		fprintf( stderr, " %d,", Where->SubID );

	}
	fprintf( stderr, " END-OF-TOKEN-LIST\n");
#	endif
}

/* ----------------------------------------------------------------------------------------- */

/*
 *	GetNextToken()
 *
 *	Return the next person who wants the token.  If nobody
 *	has it, the host gets it.
 *   
 *      Last modified : AKR 05/08.
 *
 *	RETURN VALUE:	Pointer to new token holder
 *			NULL if the host gets it.
 */
Subscriber *GetNextToken( WhatTool )
Tool *WhatTool;
{
	Subscriber	*WhoGetsIt;
	TokenNode	*OldNode;

#	if DEBUGGetNextToken
		fprintf( stderr, "GetNextToken(): Starting\n" );
#	endif

	if ( WhatTool->TokenList == NULL ) {
#	if DEBUGGetNextToken
		fprintf( stderr, "GetNextToken(): No next token holder\n" );
#	endif
		return NULL;
	}
#	if DEBUGGetNextToken
		fprintf( stderr, "GetNextToken(): TokenList is not empty\n" );
#	endif

        WhoGetsIt = FindSubscriberByID( WhatTool->TokenList->SubID );
        OldNode = WhatTool->TokenList;
        WhatTool->TokenList = OldNode->Next;
        OldNode->Next = NULL;
#       if DEBUGGetNextToken
        fprintf( stderr, "GetNextToken(): %d %s gets the token\n",
            WhoGetsIt->SubscriberID, WhoGetsIt->LoginName );
#       endif


	FreeMem( OldNode );

	return WhoGetsIt;
}

/* ----------------------------------------------------------------------------------------- */

/*
 *	FindNextToken()
 *
 *	Return the next person who wants the token. 
 *      It does not remove him from queue like GetNextToken   
 *	RETURN VALUE:	Pointer to new token holder
 *			NULL if the host gets it.
 */
Subscriber *FindNextToken( WhatTool )
Tool *WhatTool;
{
	Subscriber	*WhoGetsIt;

#	if DEBUGFindNextToken
		fprintf( stderr, "FindNextToken(): Starting\n" );
#	endif

	if ( WhatTool->TokenList == NULL ) {
#	if DEBUGFindNextToken
		fprintf( stderr, "FindNextToken(): No next token holder\n" );
#	endif
		return NULL;
	} else {
#	if DEBUGFindNextToken
		fprintf( stderr, "FindNextToken(): TokenList is not empty\n" );
#	endif

        WhoGetsIt = FindSubscriberByID( WhatTool->TokenList->SubID );
        return (WhoGetsIt);
        }
}


/* ----------------------------------------------------------------------------------------- */
/* 
 *      RemoveFromOneTokenQ()
 *       
 *
 */ 
void RemoveFromOneTokenQ (Who,WhatTool)
Subscriber *Who;
Tool       *WhatTool;
{
  BYTE    remID;
  TokenNode *oldnode,*nextnode;

if (WhatTool->TokenList==NULL){
# if DEBUGRemoveFromOneTokenQ
        fprintf(stderr,"RemoveRemoveFromOneTokenQ(): Token list of tool %s is %s\n",
                WhatTool->ToolCommand, (WhatTool->TokenList==NULL) ? "EMPTY":"NOT EMPTY");
# endif
  return;
}
  remID = Who->SubscriberID;
  oldnode = NULL;
  nextnode = WhatTool->TokenList;
  while ((nextnode != NULL) && (nextnode->SubID != remID)){
    oldnode = nextnode;
    nextnode = nextnode->Next;
  }/* while */
  if (oldnode == NULL){
	 /* WhatTool->TokenList = nextnode ERROR by AKR */
	WhatTool->TokenList = ((nextnode == NULL)? NULL:nextnode->Next);
# if DEBUGRemoveFromOneTokenQ
        fprintf(stderr,"RemoveRemoveFromOneTokenQ(): Removed FIRST subscriber %s from tool %s\n",
                Who->LoginName, WhatTool->ToolCommand);
# endif

  }
  else{
	 oldnode->Next = ((nextnode == NULL)? NULL:nextnode->Next);
# if DEBUGRemoveFromOneTokenQ
	fprintf(stderr,"RemoveRemoveFromOneTokenQ(): Removed from MIDDLE subscriber %s from tool %s\n",
		Who->LoginName, WhatTool->ToolCommand);
# endif
  }
# if DEBUGRemoveFromOneTokenQ
        fprintf(stderr,"RemoveRemoveFromOneTokenQ(): Token list of tool %s is %s\n",
                WhatTool->ToolCommand, (WhatTool->TokenList==NULL) ? "EMPTY":"NOT EMPTY");
# endif


}

/* ----------------------------------------------------------------------------------------- */
/* 
 *      RemoveFromAllTokenQs()
 *       
 *      remove a subscriber who is quitting from all the tools waiting
 *      lists.
 * 
 */ 
void RemoveFromAllTokenQs(Who)
Subscriber *Who;
{

  Tool		*WhatTool;
  for ( WhatTool=FirstTool(); WhatTool != NULL; WhatTool=NextTool() ){
# if DEBUGRemoveFromAllTokenQs
	fprintf(stderr,"RemoveFromAllTokenQs(): Removing subscriber %s from tool %s\n",
		Who->LoginName, WhatTool->ToolCommand);
# endif
	RemoveFromOneTokenQ (Who,WhatTool);
  }
}

/* ----------------------------------------------------------------------------------------- */
/*
 *	ClearTokenList()
 *
 *	Clear token list.
 *      AKR 05/08
 */
void  ClearTokenList( WhatTool )
Tool *WhatTool;
{
	TokenNode	*TempNode, *NextNode;

	for( TempNode = WhatTool->TokenList; TempNode != NULL; TempNode = NextNode) {
	  NextNode = TempNode->Next;
  	  FreeMem( TempNode );
	}
	WhatTool->TokenList = NULL;
	return;
}/* ClearTokenList */


/* ----------------------------------------------------------------------------------------- */

/*
 *	GiveTokenTo()
 *
 *
 *	RETURN VALUE:	void
 */
void GiveTokenTo(Who, WhatTool)
Subscriber *Who;
Tool *WhatTool;
{
BYTE                NextID;
Subscriber  *WhoAt;

NextID = Who->SubscriberID;
        /* Tell everybody the good news */

UpdateMessage( "%s has the token\n for tool %s.",
                          Who->LoginName, WhatTool->ToolCommand);
for ( WhoAt=FirstSubscriber(); WhoAt != NULL; WhoAt=NextSubscriber()) {
            if (WhoAt->SubscriberID != 0) {
                if (SendByte( WhoAt->Connection, HS_TakeToken )==ERROR) CleanUpSubscriber(WhoAt);
                    if (SendByte( WhoAt->Connection, WhatTool->ToolID )==ERROR) CleanUpSubscriber(WhoAt);
                    if (SendByte( WhoAt->Connection, NextID )==ERROR) CleanUpSubscriber(WhoAt);
             }
     }

     WhatTool->TokenHolder = NextID;
}

/* ----------------------------------------------------------------------------------------- */


void SwitchToken(WhatTool)
Tool *WhatTool;
{
BYTE      NextID;
Subscriber   *Who;
Subscriber *NextVictim;

Bool HostInformed=FALSE;
BYTE InformedID=0;
BYTE OldHolder = WhatTool->TokenHolder;

	NextVictim = GetNextToken(WhatTool);
	if ( NextVictim == NULL ) {
		NextID = WhatTool->ToolCreator;
		GiveTokenToCreator(WhatTool,&HostInformed,&InformedID);
/*
 * Begin Murray Addition
 */

/*
 * NOTE: See how we never set NextVictim above! This should fix that.
 */
		NextVictim = FindSubscriberByID(WhatTool->ToolCreator);

/*
 * End Murray Addition
 */

	} 
	else NextID = NextVictim->SubscriberID;

	if (OldHolder == NextID) return;

	/* Tell everybody the good news */
	if (!HostInformed) 
	  {
	    UpdateMessage( "%s has the token\n for tool %s.", ( NextID == HOST_ID ? " The chairman (that's you)" : NextVictim->LoginName), WhatTool->ToolCommand);
	  }


	for ( Who=FirstSubscriber(); Who != NULL; Who=NextSubscriber() ) {
        	if ((Who->SubscriberID != 0) &&  (Who->SubscriberID != InformedID) ) {
			if (SendByte( Who->Connection, HS_TakeToken )==ERROR) CleanUpSubscriber(Who);
			if (SendByte( Who->Connection, WhatTool->ToolID )==ERROR)CleanUpSubscriber(Who);
			if (SendByte( Who->Connection, NextID )==ERROR)CleanUpSubscriber(Who);
		}
        }

	WhatTool->TokenHolder = NextID;
	WhatTool->AskedToGetToken = FALSE;

	if (POLICY == 1){
		XtRemoveTimeOut(WhatTool->floor_timer);
		WhatTool->floor_timer = XtAppAddTimeOut(GetAppCon(), MinFloorTime, FloorTimeOut, WhatTool);
	}
}


/* ----------------------------------------------------------------------------------------- */

/*
 *   GetToken()
 *
 *   Try and get the token for a selected tool
 *
 *   RETURN VALUE:   void
 */

XtCallbackProc GetToken( widget, clientData, callData )
Widget widget;
XtPointer clientData, callData;
{
	Tool         *WhatTool;
	XawListReturnStruct   *Selected;
	Subscriber      *Who;
	Subscriber      *WhoIndeed;
	Socket         Connection;

	Selected = XawListShowCurrent( WToolList );

	/* Bail out of nothing's highlighted */

	if ( Selected->list_index == XAW_LIST_NONE ) {
		UpdateMessage( "No tool is selected." );
		return;
	}

	if ( (WhatTool=FindTool(Selected->string)) == NULL ) {
		FatalError( "INTERNAL ERROR: List widget inconsistent with tool list" );
#       if DEBUGGetToken
		fprintf( stderr,"GetToken(): Can't find info about tool %s\n", Selected->string);
#       endif
	return;
	}

	if ( WhatTool->TokenHolder == GetLocalSubID() ) {
		UpdateMessage( "You already have the %s token.", WhatTool->ToolCommand );
		return;
	} else { 
		if ( WhatTool->AskedToGetToken == TRUE){
               		UpdateMessage( "You already asked for %s token,\n you will get it shortly.", WhatTool->ToolCommand );
			return;
	     	 } else {
			WhatTool->AskedToGetToken = TRUE;
			UpdateMessage( "You will get %s token shortly.", WhatTool->ToolCommand );
			}
         }

	if ( GetMode() ) {


	    if( WhatTool->ThisToolDead == 0 ){
		AddToTokenList( WhatTool, FindSubscriberByID(HOST_ID) );
	    }

            if ( (Who=FindSubscriberByID(WhatTool->TokenHolder)) == NULL ) {
                        FatalError( " GetToken(): Cant't find Token Holder");
#	if DEBUGGetToken
	    fprintf( stderr, "GetToken(): %s wants #%d: TokenHolder is %d\n", Who->LoginName, WhatTool->ToolID, WhatTool->TokenHolder); 
#	endif
		return;
            }

            if (SendByte( Who->Connection, HS_AskTokenRelease )==ERROR) CleanUpSubscriber(Who);
            if (SendByte( Who->Connection, HOST_ID)==ERROR)CleanUpSubscriber(Who);
            if (SendByte( Who->Connection, WhatTool->ToolID )==ERROR)CleanUpSubscriber(Who);
	} else {
		  if (SendByte( GetHostConnection(), SH_WantToken )==ERROR)EndSession();
		  if (SendByte( GetHostConnection(), WhatTool->ToolID )==ERROR)EndSession();
	}
}

/* ----------------------------------------------------------------------------------------- */

/*
 *   DropToken()
 *
 *   Drop the token if we have it.  If we're the host, grab the
 *   next in line for the token and tell 'im he's got it.
 *
 *   RETURN VALUE:   void
 */

XtCallbackProc DropToken( widget, clientData, callData )
Widget widget;
XtPointer clientData, callData;
{
	Subscriber      *NextVictim;
	Tool         *WhatTool;
	XawListReturnStruct   *Selected;

	Selected = XawListShowCurrent( WToolList );

	/* Bail out of nothing's highlighted */

	if ( Selected->list_index == XAW_LIST_NONE ) {
		UpdateMessage( "No tool is selected." );
		return;
	}

	if ( (WhatTool=FindTool(Selected->string)) == NULL ) {
		FatalError( "INTERNAL ERROR: List widget inconsistent with tool list" );
#       if DEBUGGetToken
		fprintf( stderr,"GetToken(): Can't find info about tool %s\n", Selected->string);
#       endif
	return;
	}

#   if DEBUGDropToken
	fprintf( stderr, "DropToken(): Tool is %s\n", WhatTool->ToolCommand );
#   endif



	if ( WhatTool->TokenHolder != GetLocalSubID() && WhatTool->AskedToGetToken == FALSE) {
			UpdateMessage( "You don't have the %s token\n and you did't ask forit", WhatTool->ToolCommand );
			return;
	}
	if ( GetMode() ) {
		if (WhatTool->AskedToGetToken == TRUE){
			WhatTool->AskedToGetToken = FALSE;
			RemoveFromOneTokenQ (FindSubscriberByID(HOST_ID),WhatTool);
         		if (WhatTool->TokenList == NULL)
                        	ContinueTokenHolder(WhatTool);
                	UpdateMessage("[%s] token request is canceled", WhatTool->ToolCommand );
			return;
		}

		if (WhatTool->pending_reply) {
			UpdateMessage( "Can't switch %s  token now, try later.", WhatTool->ToolCommand );
			return;
		}
		SwitchToken(WhatTool);
	} else {
                if (WhatTool->AskedToGetToken == TRUE) {
                WhatTool->AskedToGetToken = FALSE;
		/* Send the protocol stuff to deque the token */
                if (SendByte( GetHostConnection(), SH_DeQueToken )==ERROR)EndSession();
                if (SendByte( GetHostConnection(), WhatTool->ToolID )==ERROR)EndSession();
                UpdateMessage("[%s] token request is canceled", WhatTool->ToolCommand );
 
		} else{
		/* Send the protocol stuff to drop the token */

		if (SendByte( GetHostConnection(), SH_ReleasingToken )==ERROR)EndSession();
		if (SendByte( GetHostConnection(), WhatTool->ToolID )==ERROR)EndSession();
                UpdateMessage("[%s] token is released", WhatTool->ToolCommand );
		}
	}
}

/* ----------------------------------------------------------------------------------------- */

/*
 *
 *  AKRFloorTimeOut()
 * old AKR contention function
 *
 *  Called whenever timeout occurs;
 *  Calls itself to put another in queue 
 *  AKR 05/08
 */
XtTimerCallbackProc  AKRFloorTimeOut( WhatTool )
Tool *WhatTool;
{

  TokenNode     *OldNode;

  BYTE          NextID;
  Subscriber    *NextSub,*Who;

  if (WhatTool->pending_reply){

# if DEBUGFloorTimeOut
    fprintf(stderr, "FloorTimeOut(): Pending reply \n");
# endif

    WhatTool->floor_timer = XtAppAddTimeOut(GetAppCon(), MinFloorTime, FloorTimeOut, WhatTool); 
    return;
  }

  if (WhatTool->TokenList == NULL){

# if DEBUGFloorTimeOut
    fprintf(stderr,"FloorTimeOut(): Token List empty so holder can keep token \n");
# endif

    WhatTool->floor_timer = XtAppAddTimeOut(GetAppCon(), MinFloorTime, FloorTimeOut, WhatTool); 
    return;
  }
  /* So by now we do have somebody in the queue requesting the token */

  /* Snatch token from current */
  NextSub = GetNextToken(WhatTool);
  if (WhatTool->TokenHolder != HOST_ID){
    if (SendByte( Who->Connection, HS_ForceTokenRelease )==ERROR)CleanUpSubscriber(Who);
    if (SendByte( Who->Connection, WhatTool->ToolID )==ERROR)CleanUpSubscriber(Who);
  }
  WhatTool->TokenHolder = NextID;

		UpdateMessage( "%s has the token\n for tool %s.", ( NextID == HOST_ID ? " The chairman (that's you)" : NextSub->LoginName), WhatTool->ToolCommand);

  for ( Who=FirstSubscriber(); Who != NULL; Who=NextSubscriber() ) {
  if (Who->SubscriberID !=0){
    if (SendByte( Who->Connection, HS_TakeToken )==ERROR)CleanUpSubscriber(Who);
    if (SendByte( Who->Connection, WhatTool->ToolID )==ERROR)CleanUpSubscriber(Who);
    if (SendByte( Who->Connection, NextID )==ERROR)CleanUpSubscriber(Who);
  }
  }

  WhatTool->floor_timer = XtAppAddTimeOut(GetAppCon(), MinFloorTime, FloorTimeOut, WhatTool); 
  
  /* clear token requesters list */
/* WHY?
  ClearTokenList(WhatTool);
*/
  return;
}


/* ----------------------------------------------------------------------------------------- */

/*
 *
 *  FloorTimeOut()
 *
 *  Called whenever timeout occurs;
 */
XtTimerCallbackProc  FloorTimeOut( WhatTool )
Tool *WhatTool;
{
        static xError NULLMESG;
  	BYTE          NextID;
  	Subscriber    *WhoHasIt,*WhoWantsIt;
        NULLMESG.type=7;
/*
        NULLMESG.errorCode=17;
*/

	WhatTool->floor_timer = XtAppAddTimeOut(GetAppCon(), MinFloorTime, FloorTimeOut, WhatTool); 

/* test if the tool is still alive */
        if (SendReply( WhatTool->InConnection, (char *)&NULLMESG, WhatTool) == ERROR) {
                return;
        }

  	if (WhatTool->pending_reply){
# if DEBUGFloorTimeOut
    fprintf(stderr, "FloorTimeOut(): Pending reply \n");
# endif
    		return;
  	}

  	if (WhatTool->TokenList == NULL){
# if DEBUGFloorTimeOut
    fprintf(stderr,"FloorTimeOut(): Token List empty so holder can keep token \n");
# endif
    		return;
  }

  	/* So by now we do have somebody in the queue requesting the token */
	/* Send a friendly message to token holder to release it */

	WhoWantsIt = FindNextToken(WhatTool);
  	WhoHasIt = FindSubscriberByID(WhatTool->TokenHolder);
  	if (WhatTool->TokenHolder == HOST_ID){
		UpdateMessage( "%s wants the token\n for tool %s.\n Select %s tool and press [Drop Token]", WhoWantsIt->LoginName, WhatTool->ToolCommand,  WhatTool->ToolCommand );
  	}else {
    		if (SendByte( WhoHasIt->Connection, HS_AskTokenRelease )==ERROR)CleanUpSubscriber(WhoHasIt);
    		if (SendByte( WhoHasIt->Connection, WhoWantsIt->SubscriberID )==ERROR)CleanUpSubscriber(WhoHasIt);
    		if (SendByte( WhoHasIt->Connection, WhatTool->ToolID )==ERROR)CleanUpSubscriber(WhoHasIt);
	}
}


GiveTokenToCreator(WhatTool, HostInf, InfID)
Tool *WhatTool;
BOOL *HostInf;
BYTE *InfID;
{

BYTE      NextID;
Subscriber   *Who;
Subscriber *NextVictim;

	NextID = WhatTool->ToolCreator;
        NextVictim = FindSubscriberByID(NextID);
	*InfID = NextID;

# if DEBUGGiveTokenToCreator
        fprintf( stderr, "GiveTokenToCreator(): Giving token to %s",
		NextVictim->LoginName);
# endif
        if ( NextID == HOST_ID ) {
        	*HostInf = TRUE;
        	UpdateMessage( "No one wants %s token \nYou have it.\nsince you are the creator of this tool" , WhatTool->ToolCommand);
        }else{
                if (SendByte( NextVictim->Connection, HS_CreatorTakeToken )==ERROR)CleanUpSubscriber(NextVictim);
                if (SendByte( NextVictim->Connection, WhatTool->ToolID )==ERROR)CleanUpSubscriber(NextVictim);
                if (SendByte( NextVictim->Connection, NextID )==ERROR)CleanUpSubscriber(NextVictim);
	}
}


/* ----------------------------------------------------------------------------------------- */

/*
 *   SnatchToken()
 *
 *   Host Snatches Token from current Token Holder 
 *
 *   RETURN VALUE:   void
 */

XtCallbackProc SnatchToken( widget, clientData, callData )
Widget widget;
XtPointer clientData, callData;
{
	Tool         *WhatTool;
	XawListReturnStruct   *Selected;
	Subscriber      *Who;
	Subscriber      *WhoIndeed;
	Socket         Connection;

	Selected = XawListShowCurrent( WToolList );

	/* Bail out of nothing's highlighted */

	if ( Selected->list_index == XAW_LIST_NONE ) {
		UpdateMessage( "No tool is selected." );
		return;
	}

	if ( (WhatTool=FindTool(Selected->string)) == NULL ) {
		FatalError( "INTERNAL ERROR: List widget inconsistent with tool list" );
#       if DEBUGSnatchToken
		fprintf( stderr,"SnatchToken(): Can't find info about tool %s\n", Selected->string);
#       endif
	return;
	}

if (!GetMode()){
	UpdateMessage( "Participants can't Snatch Token!.");
	return;
}

		/* Snatch the token away from the poor sucker */

		if ( WhatTool->TokenHolder == HOST_ID ) {
			UpdateMessage( "You already have the %s token.", WhatTool->ToolCommand );
			UpdateMessage( "You already have the %s token.", WhatTool->ToolCommand );
			return;
		}

		if ( (Who=FindSubscriberByID(WhatTool->TokenHolder)) == NULL ) {
			FatalError( "SubID List out of whack" );
#       if DEBUGSnatchToken
		fprintf( stderr,"SnatchToken(): Can't find info about subscriber %d\n", WhatTool->TokenHolder);
#       endif
		return;
		}
		if (WhatTool->pending_reply) {
			UpdateMessage( "Can't switch %s  token now, try later.", WhatTool->ToolCommand );
			return;
		}

#       if DEBUGSnatchToken
		fprintf( stderr,"Sendig HS_ForceTokenRelease to %s\n", Who->LoginName);
#       endif


		if (SendByte( Who->Connection, HS_ForceTokenRelease )==ERROR)CleanUpSubscriber(Who);
		if (SendByte( Who->Connection, WhatTool->ToolID )==ERROR)CleanUpSubscriber(Who);

		/* Tell everybody the good news, except the dude we just grabbed */

		UpdateMessage( "You Snatched the token\n for tool %s.", WhatTool->ToolCommand );

		for ( WhoIndeed=FirstSubscriber(); WhoIndeed != NULL; WhoIndeed=NextSubscriber() ) {
                if (WhoIndeed->SubscriberID != 0) {

			if ( Who == WhoIndeed )
				continue;

			if (SendByte( WhoIndeed->Connection, HS_TakeToken )==ERROR)CleanUpSubscriber(WhoIndeed);
			if (SendByte( WhoIndeed->Connection, WhatTool->ToolID )==ERROR)CleanUpSubscriber(WhoIndeed);
			if (SendByte( WhoIndeed->Connection, HOST_ID )==ERROR)CleanUpSubscriber(WhoIndeed);
		}
         }

		WhatTool->TokenHolder = HOST_ID;
RemoveFromOneTokenQ (FindSubscriberByID(HOST_ID),WhatTool);

}
