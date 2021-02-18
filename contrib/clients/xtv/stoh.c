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
	stoh.c
	Host-end routines for subscriber-to-host protocol

*/

#include <stdio.h>

#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#define	NEED_EVENTS
#define	NEED_REPLIES
#include <X11/Xproto.h>

#include "subscriber.h"
#include "tool.h"
/*
 * Begin Murray Addition
 */
#include "subscriber2.h"
#include "tool2.h"
/*
 * End Murray Addition
 */
#include "network.h"
#include "protocol.h"
#include "memory.h"
#include "token.h"
#include "misc.h"
#include "packet.h"

#define	DEBUGSTOH_Event			0&DEBUGON
#define	DEBUGSTOH_Reply			0&DEBUGON
#define	DEBUGSTOH_WantToken		10&DEBUGON
#define	DEBUGSTOH_ReleasingToken	10&DEBUGON
#define DEBUGSTOH_SendArchivePacket	0&DEBUGON

#define DEBUGSTOH_NewToolLocal		01&DEBUGON
#define DEBUGSTOH_NewToolRemote		01&DEBUGON
#define DEBUGSTOH_TerminateTool         01&DEBUGON
#define DEBUGSTOH_StopTool	         01&DEBUGON
#define DEBUGSTOH_GiveTokenToMe		01&DEBUGON
#define DEBUGSTOH_LeavingSession	01&DEBUGON
#define DEBUGSTOH_ContinueTokenHolder   01&DEBUGON
#define DEBUGSTOH_DeQueToken		01&DEBUGON

extern void StartLocalTool();
extern void StartRemoteTool();

extern int JOIN;
extern int POLICY;
extern int MinFloorTime;
/*
 *	STOH_Event()
 *
 *
 *	Protocol:	SH_Event | ToolID | Event
 *
 *		SH_Event			Protocol ID Tag
 *		ToolID				Tool ID Number
 *		Event				32-byte event
 *
 *	RETURN VALUE:	void
 */
void STOH_Event( Port, Who )
Socket Port;
Subscriber *Who;
{
	BYTE	ToolID;
	BYTE	*EventPacket;
	Tool	*WhatTool;
	void	STOH_SendArchivePacket();


	ToolID = GetByte( Port );

/* chungg_bugfix
	if ( (WhatTool=FindToolByID( ToolID )) == NULL ) {
		FatalError( "Tool list is screwed up" );
	}
*/
	if ( (WhatTool=FindToolByID( ToolID )) == NULL ) {
		fprintf(stderr,"Event Packet for invalid tool received\n");
		/* trash it */
        	EventPacket=ReadData( Port, sizeof(xGenericReply), BLOCK);
		FreeMem( EventPacket );
		return;
	}
	EventPacket = GetReply( Port, WhatTool);

#	if DEBUGSTOH_Event
	fprintf( stderr," tool waiting for reply [%d]\n", WhatTool->pending_reply);
	fprintf( stderr, "STOH_Event(): got an event (%d) from %s to %s\n", 
	    *EventPacket, Who->LoginName, WhatTool->ToolCommand );
#	endif
/* Make sure Expose event is passed through no matter what */
	if (*EventPacket == 12) {
		if( JOIN && Who->CurTool != NULL &&
		    WhatTool == ((ToolNode *)(Who->CurTool))->ToolS ){
			ExposeWindow *current;
			for( current = Who->ExposeList; current != NULL &&
			     current->wid != *((CARD32 *) (EventPacket+4));
			     current = current->Next );
			if( current != NULL ){
				current->ExposeCame = 1;
			}
			if( Who->ExposeWaitState == 1 ){
				for( current = Who->ExposeList;
				     current != NULL &&
				     current->ExposeCame == 1;
				     current = current->Next );
				if( current == NULL ){
					/* Send PutImage packet for window */
					Who->ExposeWaitState = 0;
					STOH_SendArchivePacket(Port,Who);
				}
			}
		}
		if ( WhatTool->ThisToolDead == 0 )
			SendReply( WhatTool->InConnection, EventPacket, WhatTool);
	}

	/* Make double SURE that this event should be forward to the tool */
	if (  WhatTool->ThisToolDead == 0 &&
	     DetermineViewerRouting( WhatTool, EventPacket, Who->SubscriberID) )
	    SendReply( WhatTool->InConnection, EventPacket, WhatTool);

	FreeMem( EventPacket );
}

/*
 *
 *    STOH_REply()
 *
 */

void STOH_Reply( Port, Who )
Socket Port;
Subscriber *Who;
{
	    BYTE	ToolID;
	    BYTE	*ReplyPacket;
	    Tool	*WhatTool;


#       if DEBUGSTOH_Reply
	    fprintf( stderr, "STOH_Reply(): got a Reply\n");
#       endif

	    ToolID = GetByte( Port );

/* chungg_bugfix
	if ( (WhatTool=FindToolByID( ToolID )) == NULL ) {
		FatalError( "Tool list is screwed up" );
	}
*/

	if ( (WhatTool=FindToolByID( ToolID )) == NULL ) {
		fprintf(stderr,"Reply packet for invalid tool received\n");
        	FreeMem(ToolID);
        	TrashIt(Port);
		return;
	}
	ReplyPacket = GetReply (Port, WhatTool);

    if ( WhatTool->ThisToolDead == 0 )
    {
#	if DEBUGSTOH_Reply
	fprintf( stderr, "STOH_Reply(): got a Reply from  %s to %s\n", Who->LoginName,
	    WhatTool->ToolCommand );
#	endif


	    /* Make double SURE that this Reply should be forward to the tool */

	if (  WhatTool->TokenHolder == Who->SubscriberID )
	    SendReply( WhatTool->InConnection, ReplyPacket, WhatTool);
     }

	    FreeMem(ReplyPacket);
}

/*
 *	STOH_LeavingSession()
 *
 *	The subscriber has left the session.  Cook him.
 *
 *	Protocol:	SH_LeavingSession
 *
 *		SH_LeavingSession		Protocol ID Tag
 *
 *	RETURN VALUE:	void
 */
void STOH_LeavingSession( Port, Who )
Socket Port;
Subscriber *Who;
{
	Subscriber	*WhoIndeed;

#       if DEBUGSTOH_LeavingSession
            fprintf( stderr, "STOH_LeavingSession(): subscriber:%d (%s)\n", 
                     Who->SubscriberID, Who->LoginName);
#       endif

	    CleanUpSubscriber( Who , 0);
}

/*
 *	STOH_SendArchivePacket
 *
 *	Send a archive packet one by one
 *
 *	RETURN VALUE: void
 */

void STOH_SendArchivePacket(Port,starter)
Socket Port;
Subscriber *starter;
{
	char *memp;
	int pksize;
	ToolNode *CurNode;
        ToolNode *Next;

	CurNode = (ToolNode *) starter->CurTool;
	/* find if we have sent all archive packets */
	if( starter->CurPos == 
	    (CurNode->ToolS->ArchiveSize + CurNode->ToolS->Archive2Size) )
	{
		Tool	*TmpTool;

		/* Free ExposeList if it at all exists */
		if( starter->ExposeList != NULL ){
			ExposeWindow *Current;
			ExposeWindow *NextWindow;
			for( NextWindow = Current = starter->ExposeList;
			     NextWindow != NULL; Current = NextWindow ){
				NextWindow = Current->Next;
				FreeMem(Current);
			}
		}
		starter->ExposeList = NULL;
		starter->ExposeWaitState = 0;

		starter->CurTool = (void *) (CurNode->Next);
		starter->CurPos = 0;

		/* Send the CatchUpEnd packet */
		if( CurNode->ToolS->ThisToolDead == 0 )
		{
#if DEBUGSTOH_SendArchivePacket
			ReportToolStat(CurNode->ToolS);
#endif
			if (SendByte(starter->Connection,HS_CatchUpEnd)==ERROR) CleanUpSubscriber(starter);
			if (SendByte(starter->Connection,CurNode->ToolS->ToolID)==ERROR) CleanUpSubscriber(starter);
		}

		/* if this is the last new subscriber at this time,
		   free archive buffers because they consume much space */
		if(--CurNode->ToolS->SubDirtyCount == 0)
		{
			CurNode->ToolS->ArchiveSize = 0;
			FreeMem(CurNode->ToolS->ArchiveBuf);
			CurNode->ToolS->ArchiveBuf = NULL;
			if( CurNode->ToolS->Archive2Size > 0 ){
				CurNode->ToolS->Archive2Size = 0;
				FreeMem(CurNode->ToolS->Archive2Buf);
				CurNode->ToolS->Archive2Buf = GetMem(0);
			}
			/* if this tool was dead while catching up
			   delete the tool structure */
			if( CurNode->ToolS->ThisToolDead )
			{
				TmpTool = CurNode->ToolS;
				DeleteTool(TmpTool);
			}
		}
		/* See if there's more tool to catch up on */
		if( starter->CurTool != NULL )
		{
			if( ((ToolNode *)(starter->CurTool))->ToolS->SubDirtyCount > 0 )
			{
			    if( ((ToolNode *)(starter->CurTool))->ToolS->ArchiveSize == 0 ){
				fprintf(stderr,"HTOS_SendArchivePacket: 0 length archive\n");
				exit(1);
			    }
			}
			else{
			    PreparePacketsToSend(((ToolNode *)(starter->CurTool))->ToolS);
			}
			++((ToolNode *)(starter->CurTool))->ToolS->SubDirtyCount;

			starter->ExposeWaitState = 0;
			starter->ExposeList = NULL;
			CreateExposeList(((ToolNode *)(starter->CurTool))->ToolS,
					 &(starter->ExposeList));
			if (SendByte(starter->Connection,HS_CatchUpStart )==ERROR) CleanUpSubscriber(starter);
		}
		CurNode = (ToolNode *) starter->CurTool;
		if( CurNode == NULL ){
                        for( CurNode = (ToolNode *)(starter->CatchUpList);
                             CurNode != NULL; CurNode = Next ){
                                Next = CurNode->Next;
                                FreeMem(CurNode);
                        }
                        starter->CatchUpList = NULL;
			return;
		}
	}
	if( starter->CurPos >
	    (CurNode->ToolS->ArchiveSize +CurNode->ToolS->Archive2Size) )
	{
		perror("Went over the archive while sending archive packets");
		exit(1);
	}
	if( starter->CurPos >= CurNode->ToolS->ArchiveSize ){
#if DEBUGSTOH_SendArchivePacket
fprintf(stderr,"Sending from Archive2\n");
#endif
		memp = CurNode->ToolS->Archive2Buf
			+starter->CurPos-CurNode->ToolS->ArchiveSize;
	}
	else{
#if DEBUGSTOH_SendArchivePacket
fprintf(stderr,"Sending from Archive\n");
#endif
		memp = CurNode->ToolS->ArchiveBuf+starter->CurPos;
	}

	/* See if we have to send backing store window image */
	if( starter->CurPos == CurNode->ToolS->ArchiveSize ){
		ExposeWindow *current;

		for( current = starter->ExposeList; current != NULL && current->ExposeCame == 1; current = current->Next );
		if( current != NULL ){
#if DEBUGSTOH_SendArchivePacket
fprintf(stderr,"I'm waiting for expose events to come\n");
#endif
			starter->ExposeWaitState = 1;
			return;
		}
	}
	memcpy((char *) &pksize,memp,sizeof(pksize));
	SendData(starter->Connection,memp+sizeof(pksize),pksize);
	starter->CurPos += ( pksize + sizeof(pksize) );
}


/*
 *
 *    STOH_NewToolLocal()
 *
 */
void STOH_NewToolLocal( Port, Who)
Socket Port;
Subscriber *Who;
{
 char *command;

 command = (char *)ReadData(Who->Connection, TOOL_CMD_SIZE, TRUE);
#	if DEBUGSTOH_NewToolLocal
        fprintf( stderr, "STOH_NewToolLocal(): command: %s, SubscriberID: %d\n",
		command,Who->SubscriberID);
#	endif
 StartLocalTool(command,Who->SubscriberID);
}

/*
 *
 *    STOH_NewToolRemote()
 *
 */

void STOH_NewToolRemote( Port, Who)
Socket Port;
Subscriber *Who;
{
 char *command;

 command = (char *)ReadData(Who->Connection, TOOL_CMD_SIZE, TRUE);
#	if DEBUGSTOH_NewToolRemote
      fprintf( stderr, "STOH_NewToolRemote(): command: %s, SubscriberID: %d\n",
		command,Who->SubscriberID);
#	endif
 StartRemoteTool(command,Who->SubscriberID);
}

/*
 *
 *    STOH_TerminateTool()
 *
 */

void STOH_TerminateTool( Port, Who)
Socket Port;
Subscriber *Who;
{
 BYTE TID;
 Tool *What;

 TID = GetByte(Who->Connection);
 What = FindToolByID(TID);
 if (What == NULL) return;
#	if DEBUGSTOH_TerminateTool
      fprintf( stderr, "STOH_TerminateTool(): ToolID: %s, SubscriberID: %d\n",
		TID, Who->SubscriberID);
#	endif
 CleanUpTool(What);
}


BOOL RemoteCreatorFlag = FALSE;
/*
 *	STOH_WantToken()
 *
 *	The subscriber wants the token for a tool.
 *
 *	Protocol:	SH_WantToken | ToolID
 *
 *		SH_LeavingSession		Protocol ID Tag
 *		ToolID				Tool ID Number
 *
 *	RETURN VALUE:	void
 */
void STOH_WantToken( Port, Who )
Socket Port;
Subscriber *Who;
{
	Subscriber *WhoHasIt;
	BYTE	ToolID;
	    Tool	*WhatTool;

	    ToolID = GetByte( Port );

#	if DEBUGSTOH_WantToken
	    fprintf( stderr, "STOH_WantToken(): %s wants #%d\n", Who->LoginName, ToolID );
#	endif

	if ( (WhatTool=FindToolByID( ToolID )) != NULL ) {
	    if( WhatTool->ThisToolDead == 0 ){
		AddToTokenList( WhatTool, Who );
	    }
	}
        if ( WhatTool->ThisToolDead == 0 &&
	     WhatTool->TokenHolder == GetLocalSubID() ) {
		if (RemoteCreatorFlag){
			UpdateMessage( "%s created tool %s.\nWait until the tool windows appear then:\nSelect %s tool and press [Drop Token]", Who->LoginName, WhatTool->ToolCommand,  WhatTool->ToolCommand );
			RemoteCreatorFlag = FALSE;
		} else
			UpdateMessage( "%s wants the token\n for tool %s.\n Select %s tool and press [Drop Token]", Who->LoginName, WhatTool->ToolCommand,  WhatTool->ToolCommand );
        }
        else {
                if ( (WhoHasIt=FindSubscriberByID(WhatTool->TokenHolder)) == NULL ) {
                        FatalError( " STOH_WantToken(): Cant't find Token Holder");
#	if DEBUGSTOH_WantToken
	    fprintf( stderr, "STOH_WantToken(): %s wants #%d: TokenHolder is %d\n", Who->LoginName, ToolID, WhatTool->TokenHolder);
#	endif
		return;
                }
                if (SendByte( WhoHasIt->Connection, HS_AskTokenRelease )==ERROR) CleanUpSubscriber(WhoHasIt);
                if (SendByte( WhoHasIt->Connection, Who->SubscriberID)==ERROR)CleanUpSubscriber(WhoHasIt);
                if (SendByte( WhoHasIt->Connection, WhatTool->ToolID )==ERROR)CleanUpSubscriber(WhoHasIt);
	}
}

/*
 *      STOH_DeQueToken()
 *
 *      The subscriber wants the token for a tool.
 *
 *      Protocol:       SH_DeQueToken | ToolID
 *
 *      RETURN VALUE:   void
 */
void STOH_DeQueToken( Port, Who )
Socket Port;
Subscriber *Who;
{
        BYTE            ToolID;
            Tool                *WhatTool;
            BYTE                NextID;
            Subscriber  *WhoAt;
            Subscriber  *NextVictim;

            ToolID = GetByte( Port );

        if ( (WhatTool=FindToolByID( ToolID )) == NULL ) {
#       if DEBUGSTOH_DeQueToken
        	fprintf( stderr, "STOH_DeQueToken(): %s dequeued ToolID %d: Can't find Tool inf ormation\n", Who->LoginName, ToolID );
#       endif

                FatalError( "STOH_DeQueToken(): Can't find tool info" );
                return;
        }

        if ( WhatTool->ThisToolDead == 1 ) return;

         RemoveFromOneTokenQ (Who,WhatTool);
         /* if token list is emty, let the token holder continue */
         if (WhatTool->TokenList == NULL)
                        ContinueTokenHolder(WhatTool);
#       if DEBUGSTOH_DeQueToken
        fprintf( stderr, "STOH_DeQueToken(): %s dequeued from %s queue\n", 
Who->LoginName, WhatTool->ToolCommand );
#       endif
}


/*
 *	STOH_ReleasingToken()
 *
 *	The subscriber wants the token for a tool.
 *
 *	Protocol:	SH_ReleasingToken | ToolID
 *
 *	RETURN VALUE:	void
 */
void STOH_ReleasingToken( Port, Who )
Socket Port;
Subscriber *Who;
{
	BYTE		ToolID;
	    Tool		*WhatTool;
	    BYTE		NextID;
	    Subscriber	*WhoAt;
	    Subscriber	*NextVictim;

	    ToolID = GetByte( Port );

	if ( (WhatTool=FindToolByID( ToolID )) == NULL ) {
#	if DEBUGSTOH_ReleasingToken
	fprintf( stderr, "STOH_ReleasingToken(): %s dropped ToolID %d: Can't find Tool information\n", Who->LoginName, ToolID );
#	endif
		FatalError( "STOH_ReleasingToken(): Can't find tool info" );
		return;
	}

	if ( WhatTool->ThisToolDead == 1 ) return;

#	if DEBUGSTOH_ReleasingToken
	fprintf( stderr, "STOH_ReleasingToken(): %s dropped %s\n", Who->LoginName, WhatTool->ToolCommand );
#	endif
	SwitchToken(WhatTool);

}

/*
 *	STOH_StopTool()
 *
 *	The subscriber stoped subscribing to this tool tool.
 *      either by destroing its winodws or by using a stop button.
 *
 *	Protocol:	SH_StopTool | ToolID
 *
 *	RETURN VALUE:	void
 */
void STOH_StopTool( Port, Who )
     Socket Port;
     Subscriber *Who;
{
  BYTE		ToolID;
  Tool		*WhatTool;
  BYTE		NextID;
  Subscriber	*WhoAt;
  Subscriber	*NextVictim;
  
  ToolID = GetByte( Port );
  
  if ( (WhatTool=FindToolByID( ToolID )) == NULL ) {
#if DEBUGSTOH_StopTool
    fprintf( stderr, "STOH_StopTool(): %s stopped ToolID %d: Can't find Tool information\n", Who->LoginName, ToolID );
#endif
    FatalError( "STOH_StopTool(): Can't find tool info" );
    return;
  }

  /* If the subscriber was catching up on this tool,
     let him catch up on the next tool */
  if ( Who->CurTool != NULL && WhatTool == ((ToolNode *) (Who->CurTool))->ToolS ){
    Who->CurPos = ((ToolNode *) (Who->CurTool))->ToolS->ArchiveSize
			+ ((ToolNode *) (Who->CurTool))->ToolS->Archive2Size;
    STOH_SendArchivePacket(Port, Who);
  }
  
  if ( WhatTool->ThisToolDead == 1 ) return;
  
  /* switch ownership of tool if the owner stoped subscribing to it */
  if ( WhatTool->ToolCreator == Who->SubscriberID)
    WhatTool->ToolCreator = HOST_ID;

#if DEBUGSTOH_StopTool
  fprintf( stderr, "STOH_StopTool(): %s stoped %s\n", Who->LoginName, WhatTool->ToolCommand );
#endif

  /* deque him from the tool if he is waitingto get token */
  RemoveFromOneTokenQ (Who,WhatTool);
  /* if token list is emty, let the token holder continue */
  if (WhatTool->TokenList == NULL)
    ContinueTokenHolder(WhatTool);
  /* reassign token if he holds it */
  
 if (Who->SubscriberID == WhatTool->TokenHolder) 
  SwitchToken(WhatTool);

  /*
   * Begin Murray Addition
   */
  
  /*
   * Remove the subscriber Who from the subscrList of WhatTool.
   * This means find the subscriber, and if present, perform a delete item.
   */
  findSubscr(WhatTool, Who);
  if (offrightSubscr(WhatTool))
    FatalError("STOH_StopTool(): Unable to find indicated subscriber.\n");
  else
    {
      deleteSubscr(WhatTool);
    }

  findTool(Who, WhatTool);
  if (offrightTool(Who))
    FatalError("STOH_StopTool: Unable to find tool in subscriber.\n\r");
  else
    {
      deleteTool(Who);
    }

  /*
   * End Murray Addition
   */
}

extern ToolNode *ToolList;

void STOH_WantJoinTools( Port, Who )
     Socket Port;
     Subscriber *Who;
{
        ToolNode *Current;
        Tool *WhatTool;

    SendByte(Port,HS_ToolsToJoin);
    if( JOIN ){
        for( Current = ToolList; Current != NULL; Current = Current->Next ){
                WhatTool = Current->ToolS;
                findSubscr(WhatTool, Who);
                if( offrightSubscr(WhatTool) && !ArchiveToBeSent(Who,WhatTool) )
                {
                        SendByte(Port,WhatTool->ToolID);
                        SendData(Port,WhatTool->ToolCommand,100);
                }
        }
    }
    SendByte(Port,0xff);
}

void STOH_JoinTools( Port, Who )
     Socket Port;
     Subscriber *Who;
{
        BYTE list[100];
        int i;

        for( i=0; (list[i] = GetByte(Port)) != 0xff; i++);

        if( i > 0 && JOIN )
                InitiateCatchUp(Who,Who->Connection,i,list);
}

/*
 *
 *    STOH_GiveTokenToMe()
 *
 */
Tool *WhatToolL;

void STOH_GiveTokenToMe( Port, Who)
Socket Port;
Subscriber *Who;
{
 BYTE TID;
 Tool *WhatTool;
int GiveTokenToRemoteCreator();
long sleeptime;

/*
 TID = GetByte(Who->Connection);
 WhatTool = FindToolByID(TID);
#       if DEBUGSTOH_GiveTokenToMe
      fprintf( stderr, "STOH_GiveTokenToMe(): ToolName: %s, SubscriberID: %d\n",
                WhatTool->ToolCommand ,Who->SubscriberID);
#       endif

sleeptime = 10*1000 ; 
WhatToolL=WhatTool;
XtAppAddTimeOut(GetAppCon(), sleeptime, GiveTokenToRemoteCreator, Who);
*/
/* Alternative to time-out, ask the chairman to release the token */
 RemoteCreatorFlag = TRUE;
 STOH_WantToken( Port, Who );
 RemoteCreatorFlag = FALSE;

}

GiveTokenToRemoteCreator(Who, id)
Subscriber *Who;
XtIntervalId id;
{
GiveTokenTo(Who, WhatToolL);
}

ContinueTokenHolder(WhatTool)
Tool *WhatTool;
{
Subscriber *Who;
	if (WhatTool->TokenHolder == HOST_ID)
		UpdateMessage("No one waiting for [%s] token.\nYou may keep it", WhatTool->ToolCommand);
        else {
        if ( (Who=FindSubscriberByID(WhatTool->TokenHolder)) == NULL ) {
                FatalError( " STOH_WantToken(): Cant't find Token Holder");
#if  DEBUGSTOH_ContinueTokenHolder
            fprintf( stderr, "STOH_ContinueTokenHolder(): Cant get information about TokenHolder %d\n", WhatTool->TokenHolder);
#       endif
        return;
        }
        if (SendByte( Who->Connection, HS_ContinueTakeToken )==ERROR)CleanUpSubscriber(Who);
        if (SendByte( Who->Connection, WhatTool->ToolID )==ERROR)CleanUpSubscriber(Who);
        if (SendByte( Who->Connection, Who->SubscriberID)==ERROR)CleanUpSubscriber(Who);
        }
}

/*
 * Begin Murray Addition
 */

/*
 * STOH_QueuedList()
 *
 * Ask for the list of queued ids from the host.
 *
 * 2-VII-91 - Now adds a list of all the holders of the tool.
 *
 * Protocol:
 *    SH_QueuedList - Protocol ID Tag.
 *    destination   - Where the return call should be routed by HTOS_QueuedList.
 *    toolID        - Number of the tool that we are interested in.
 *
 * Local vars:
 *    toolID        - (BYTE) holds the id of the tool. This is passed to tokenInfoSubscr() as
 *                    part of a consistency check.
 *    tITool        - (Tool *) this is the tool that we are getting the info for.
 *    numQueued     - (int) How many people are queued. This is sent before the queued entries.
 *    tokenCursor   - (TokenNode *) Used to walk the token list of the tool.
 *
 * Return value: void.
 */

#ifdef __STDC__
void STOH_QueuedList(Socket Port, Subscriber *Who)
#else 
void STOH_QueuedList(Port, Who)
Socket Port;
Subscriber *Who;
#endif
{
  BYTE destination;
  BYTE toolID;
  Tool *tITool;
  int numQueued;
  TokenNode *tokenCursor;

  destination = GetByte(Port);
  toolID = GetByte(Port);
  if (!(tITool=FindToolByID(toolID)))
    {
      fprintf(stderr, "ACK! No tool #%d exists in STOH_QueuedList().\n", (unsigned int) toolID);
      return;
    }

  numQueued = 0;
     /* tokenStart() */              /* !tokenOffright() */      /* tokenForth() */
  for (tokenCursor = tITool->TokenList; tokenCursor; tokenCursor = tokenCursor->Next)
    numQueued++;

/*
 * Increment numQueued for the separator token in the stream.
 */

  numQueued++;

/*
 * Increment numQueued for the host token, since the host always a tool.
 */

  numQueued++;


  for (startSubscr(tITool); !offrightSubscr(tITool); forthSubscr(tITool))
    numQueued++;

  if (SendByte( Who->Connection, HS_QueuedList )==ERROR) CleanUpSubscriber(Who);
  if (SendByte( Who->Connection, (BYTE) destination )==ERROR) CleanUpSubscriber(Who);
  if (SendByte( Who->Connection, (BYTE) toolID ) == ERROR) CleanUpSubscriber(Who);
  if (SendByte( Who->Connection, (BYTE) numQueued) == ERROR) CleanUpSubscriber(Who);

/*
 * Send the ids of the token holder and all those queued for the token.
 */

  for (tokenCursor = tITool->TokenList; tokenCursor; tokenCursor = tokenCursor->Next)
    if (SendByte( Who->Connection, tokenCursor->SubID ) == ERROR) CleanUpSubscriber(Who);

/*
 * Send the id of the token holder as a separator.
 */

  if (SendByte( Who->Connection, tITool->TokenHolder ) == ERROR) CleanUpSubscriber(Who);

/*
 * Send the id of the host, since the host is not included in the list of subscribers to tool.
 */

  if (SendByte( Who->Connection, HOST_ID ) == ERROR) CleanUpSubscriber(Who);

/*
 * Send the ids of everyone who subscribes to the tool.
 */

  for (startSubscr(tITool); !offrightSubscr(tITool); forthSubscr(tITool))
    if (SendByte( Who->Connection, getSubscr(tITool)->SubscriberID ) == ERROR) CleanUpSubscriber(Who);
}

/*
 * STOH_ConfirmNewTool
 *
 * Confirm the acceptance of a new tool.
 * This is sent by a subscriber, when that subscriber accepts a new tool
 * at his or her workstation.
 *
 * Protocol:
 *    SH_ConfirmNewTool - Gets us here.
 *    BYTE toolID       - Tells the host what tool was accepted.
 *
 * Temp Vars:
 *    BYTE toolID       - Holds the ID of the tool confirmed.
 *    Tool *thisTool    - This is the actual tool.
 *
 */

#ifdef __STDC__
void STOH_ConfirmNewTool(Socket Port, Subscriber *Who)
#else
void STOH_ConfirmNewTool(Port, Who)
Socket Port;
Subscriber *Who;
#endif
{
  BYTE toolID;
  Tool *thisTool;

  toolID = GetByte(Port);
  if( !(thisTool = FindToolByID(toolID)) )
    {
      FatalError(stderr, "STOH_ConfirmNewTool(): Uh Oh! Can't find the tool that is being confirmed.\n\r");
      exit(1);
    }

  /*
   * Make sure that the tool knows about the subscriber and that the subscriber
   * knows about the tool.
   */

  insertTool(Who, thisTool);
  addSubscr(thisTool, Who);  

  if (lastTool(FindSubscriberByID(HOST_ID))->ToolID == thisTool->ToolID)
    {
      if (SendByte(Who->Connection, HS_EndIncoming) == ERROR)
	CleanUpSubscriber(Who);
    }
}

/*
 * End Murray Addition
 */

