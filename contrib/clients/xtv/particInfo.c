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
 * particInfo.c
 *
 * This is the participant information module.
 *
 * Murray Anderegg 12-VI-91
 * token information posts information in the xtv/xdesk message box about
 * the selected tool.
 *
 * There are supposed to be only two external entry points:
 *    particInfo() - The basic entry point when the button gets pushed.
 *    particInfoSubscr(short numQueued, BYTE* queuedEntries, BYTE toolID)
 *        - The entry point for the callback event from the host.
 *        - The callback entry is HTOS_ParicipantInfo
 *        - The callback number is entered as HS_ParticipantInfo.
 */

/*
 * Declarations
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
#include "subscriber2.h"
#include "tool2.h"
#include "particInfo.h"
#include "htos.h"

#include "ptrlists.h"

extern Widget WSubscriberList;
extern Widget WToolList;

/*
 * Give all variables the prefix pI for particInfo.
 * They are all static, which should make them all local to the module.
 * If it turns out that they can be used elsewhere, then the C compiler is
 * truly terrible and we should use CFront to clean stuff up.
 */

/*
 * pIMessQueue and pISubsQueue are two queues for participant information that
 * shadow each other.
 *  pIMessQueue is a PTRLIST * of char *.
 *  pISubsQueue is a PTRLIST * of Subscriber *
 */

static PTRLIST *pIMessQueue = 0;
static PTRLIST *pISubsQueue = 0;

/*
 * pILastTool is reset everytime through particInfo.
 * It is used in particInfoBuild to wrapup the message and
 * send it to the message buffer.
 */
static Tool *pILastTool;

int CHAR2048 = sizeof(char) * 2048;

/*
 * particInfoStartConcat and particInfoFinishConcat
 *
 * These two functions concatenate the next part of
 * the message onto the end of either the item at the
 * head or tail of the list.
 */
static void 
#if defined(__STDC__) || defined(__STRICT_ANSI__)
particInfoStartConcat(char *nextPart)
#else
particInfoStartConcat(nextPart)
char *nextPart;
#endif
{
  char *thisMessage;

  saveCursor(pIMessQueue);
  start(pIMessQueue);
  if (offright(pIMessQueue))
    {
      FatalError("Attempt to build message on empty list.\n");
    }
  thisMessage = (char *) getItem(pIMessQueue);
  (void) strcat(thisMessage, nextPart);
  restoreCursor(pIMessQueue);
}

static void 
#if defined(__STDC__) || defined(__STRICT_ANSI__)
particInfoFinishConcat(char *nextPart)
#else
particInfoFinishConcat(nextPart)
char *nextPart;
#endif
{
  char *thisMessage;

  saveCursor(pIMessQueue);
  finish(pIMessQueue);
  if (offright(pIMessQueue))
    {
      FatalError("Attempt to build message in empty list.\n");
    }
  thisMessage = (char *) getItem(pIMessQueue);
  (void) strcat(thisMessage, nextPart);
  restoreCursor(pIMessQueue);
}

static void 
#if defined(__STDC__) || defined(__STRICT_ANSI__)
particInfoInitialize(Subscriber *subscriber)
#else
particInfoInitialize(subscriber)
Subscriber *subscriber;
#endif
{
  if (!pIMessQueue)
    {
      pIMessQueue = ptrNew();
      pISubsQueue = ptrNew();
    }
  addTail(pIMessQueue, (void *) malloc(CHAR2048));
  addTail(pISubsQueue, (void *) subscriber);

  (void) strcpy((char *) getItem(pIMessQueue), "");
  particInfoFinishConcat("Participant Info: ");
  particInfoFinishConcat(subscriber->LoginName);
  particInfoFinishConcat("@");
  particInfoFinishConcat(subscriber->HostName);
  particInfoFinishConcat("\n  Full name: ");
  particInfoFinishConcat(subscriber->FullName);
  particInfoFinishConcat("\n\n");
}

static void
particInfoWrapup()
{
  saveCursor(pIMessQueue);
  start(pIMessQueue);
  start(pISubsQueue);
  if (offright(pIMessQueue))
    {
      FatalError("Attempted to display message from empty list.\n");
      return;
    }
  (void) UpdateMessage((char *) getItem(pIMessQueue));
  free((char *) getItem(pIMessQueue));
  deleteItem(pIMessQueue);
  deleteItem(pISubsQueue);
  restoreCursor(pIMessQueue);
}

/*
 * particInfoBuild
 *
 * This is the reentry point into the module. This allows us to take the current
 * list of entries as either the host or callback routine supplies them and
 * build a message based upon them. When we are done, we need to post by means
 * of the particInfoWrapup() routine.
 */
void
#if defined(__STDC__) || defined(__STRICT_ANSI__)
particInfoBuild(BYTE toolID, short numQueued, BYTE queuedEntries[512])
#else
particInfoBuild(toolID, numQueued, queuedEntries)
BYTE toolID;
short numQueued;
BYTE queuedEntries[512];
#endif
{
  Tool *thisTool;
  short thisQueued;
  char tmpBuffer[128];
  Subscriber *pISubscriber;
  int thisHolder, checkHolder;

  saveCursor(pISubsQueue);
  start(pISubsQueue);
  pISubscriber = (Subscriber *) getItem(pISubsQueue);
  
  thisTool = FindToolByID(toolID);

  if (thisTool->TokenHolder == pISubscriber->SubscriberID)
    {
      particInfoStartConcat("  ");
      particInfoStartConcat(pISubscriber->LoginName);
      particInfoStartConcat(" has floor for: ");
      particInfoStartConcat(thisTool->ToolCommand);
      particInfoStartConcat("\n");
    }

  /*
   * Run through the returned list of queued entries and check the login names.
   */
  for (thisQueued=0; (queuedEntries[thisQueued] != thisTool->TokenHolder) ; thisQueued++)
    if (queuedEntries[thisQueued] == pISubscriber->SubscriberID)
      {
	particInfoStartConcat("  ");
	particInfoStartConcat(pISubscriber->LoginName);
/*
 *	particInfoStartConcat("@");
 *	particInfoStartConcat(pISubscriber->HostName);
 */
	particInfoStartConcat(" queued #");
	sprintf(tmpBuffer,"%d ", thisQueued+1);
	particInfoStartConcat(tmpBuffer);
	particInfoStartConcat("for: ");
	particInfoStartConcat(thisTool->ToolCommand);
	particInfoStartConcat("\n");
      }
  
  for (thisHolder=thisQueued+1; thisHolder < numQueued; thisHolder++)
    if ((queuedEntries[thisHolder] != thisTool->TokenHolder) && (queuedEntries[thisHolder] == pISubscriber->SubscriberID))
      {
	for (checkHolder=0; (checkHolder < thisQueued) && (queuedEntries[checkHolder] != queuedEntries[thisHolder]); checkHolder++)
	  ;
	if (checkHolder >= thisQueued)
	  {
	    particInfoStartConcat("  ");
	    particInfoStartConcat(pISubscriber->LoginName);
/*
 *	    particInfoStartConcat("@");
 *	    particInfoStartConcat(pISubscriber->HostName);
 */
	    particInfoStartConcat(" has ");
	    particInfoStartConcat(thisTool->ToolCommand);
	    particInfoStartConcat("\n");
	  }
      }
  
  /*
   * Always try and leave things as we found them.
   */
  restoreCursor(pISubsQueue);

  if (pILastTool && (pILastTool->ToolID == toolID))
    particInfoWrapup();
}


/*
 * particInfo() is the entry point to the token information code.
 */
XtCallbackProc particInfo(widget, clientData, callData)
Widget widget;
XtPointer clientData, callData;
{
  Subscriber *thisSubscriber;
  XawListReturnStruct *selected;
  Tool *thisTool, *nextTool;
  BYTE queuedEntries[512];
  TokenNode *tokenCursor;
  short numQueued, thisQueued;

  selected = XawListShowCurrent( WSubscriberList );
  
  /*
   * Bail out if nothing's highlighted
   */
  if ( selected->list_index == XAW_LIST_NONE )
    {
      UpdateMessage( "-- No participant is selected. --\n" );
      return;
    }

  /*
   * Find the last tool, so that we can use it as a flag for posting info messages.
   */

  for (thisTool=NULL, nextTool=FirstTool(); nextTool; thisTool=nextTool, nextTool=NextTool())
    ;

  pILastTool = thisTool;

  /*
   * This is similar to FindSubscriber(), but can allow one to continue past
   * the first match.
   *
   * This allows one to match for than one subscriber of the same name.
   *
   * This may be important for subscribers with names like: smith, jones, etc.
   */

  for (thisSubscriber=FirstSubscriber();
       thisSubscriber;
       thisSubscriber=NextSubscriber())
    if (CompareSubscrNameHost(thisSubscriber, selected->string))
/*
 *  if (thisSubscriber && !strcmp(thisSubscriber->LoginName,selected->string))
 */
      {
	particInfoInitialize(thisSubscriber);
	
	if (GetLocalSubID((BYTE) 0) != HOST_ID)
	  /*
	   * For each of the tools that we know about, ask the host who is in the queue
	   * for them.
	   */
	  for (thisTool=FirstTool(); thisTool != NULL; thisTool=NextTool(thisTool))
	    {
	      SendByte(GetHostConnection(), SH_QueuedList);
	      SendByte(GetHostConnection(), partInfo);
	      SendByte(GetHostConnection(), thisTool->ToolID);
	    }
	else
	  /*
	   * Manually build up the token entries for each tool.
	   */
	  for (thisTool=FirstTool(); thisTool != NULL; thisTool=NextTool())
	    {
	      /*
	       * First add all of the queued subscribers.
	       */
	      for (numQueued = 0, tokenCursor = thisTool->TokenList; tokenCursor; tokenCursor = tokenCursor->Next, numQueued++)
		queuedEntries[numQueued] = tokenCursor->SubID;
	      /*
	       * Then add the token holder as a separator,
	       * since one can't both hold the token and be queued for it.
	       */
	      queuedEntries[numQueued] = thisTool->TokenHolder;
	      numQueued++;
	      /*
	       * Next we add everyone who has the tool.
	       * Since the host always has the tool and is not in the subscriber
	       * lists of tools, we need to manually insert it into the list.
	       * Then add all of the tool holders.
	       */
	      queuedEntries[numQueued] = HOST_ID;
	      numQueued++;
	      for (startSubscr(thisTool); !offrightSubscr(thisTool); forthSubscr(thisTool), numQueued++)
		queuedEntries[numQueued] = getSubscr(thisTool)->SubscriberID;
	      /*
	       * Finally, build the participant information message.
	       */
	      particInfoBuild(thisTool->ToolID, numQueued, queuedEntries);
	    }
	if (pILastTool == NULL)
	  {
	    particInfoStartConcat("-- You have no tools open --\n");
	    particInfoWrapup();
	  }
      }
}


