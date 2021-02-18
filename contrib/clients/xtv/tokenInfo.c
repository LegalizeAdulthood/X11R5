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
 * tokenInfo.c
 *
 * Murray Anderegg -- 30-V-91
 *
 * tokenInfo.c contains the token information code.
 *
 * token information posts information in the xtv/xdesk message box about
 * the selected tool.
 *
 * There are supposed to be only two external entry points:
 *    tokenInfo() - The basic entry point when the button gets pushed.
 *    tokenInfoSubscr(short numQueued, BYTE* queuedEntries, BYTE toolID)
 *        - The entry point for the callback event from the host.
 *        - The callback entry is HTOS_QueuedList.
 *        - The callback number is entered as HS_QueuedList.
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


extern Widget WToolList;

#include "tokenInfo.h"
#include "htos.h"

/*
 * Give all variables the prefix tI for tokenInfo.
 * They are all static, which should make them all local to the module.
 * If it turns out that they can be used elsewhere, then the C compiler is
 * truly terrible and we should use CFront to clean stuff up.
 */

/*
 * tIHold holds the login name of the subscriber.
 * tIQueue holds the names of those people who have requested the token.
 * tIMessage is a static 2K buffer for holding the assembled message.
 * TINUMIDSLINE is a constant that states the number of ids to print on
 *     one line in the queued participants list.
 */

static BYTE queuedEntries[256];
static char tIMessage[2048];

#define TINUMIDSLINE 4

/*
 * End declarations
 */

/*
 * tokenInfoConcat is a utility routine which concatenates a string onto the message.
 * tokenInfoAddHolder(ToolID, numBlanks) adds a string about the token holder to the message.
 *    toolID is a BYTE that identifies the tool whose holder should be added to the message.
 *    numBlanks is a short integer specifying the number of blank lines to prepend.
 * tokenInfoAddQueued adds a string with the login of the current subscriber to the message.
 */

static void tokenInfoConcat(nextPart)
char *nextPart;
{
  (void) strcat(tIMessage, nextPart);
}

static void tokenInfoAddHolder(subscriber, numBlanks)
Subscriber *subscriber;
short numBlanks;
{
  short blankTally;

  /*
   * Insert the requested number of blank lines.
   */
  for (blankTally=0; blankTally < numBlanks; blankTally++)
    tokenInfoConcat("\n");

  /*
   * Put out the Token Holder header and the name of
   * the subscriber listed as the token holder.
   */
  tokenInfoConcat("Floor Holder is:\n  ");
  tokenInfoConcat(subscriber->LoginName);
}

static void tokenInfoAddQueued(subscriber, tIBuildOffset)
Subscriber *subscriber;
short tIBuildOffset;
{
  /*
   * Anytime the offset is at zero, go to the next line.
   */
  if (!tIBuildOffset)
    tokenInfoConcat("\n");

  /*
   * Add a queued subscriber name to the list.
   */
  tokenInfoConcat("  ");
  tokenInfoConcat(subscriber->LoginName);
}

/*
 * tokenInfoInitialize and tokenInfoWrapup are static (local) functions for this module.
 */
static void tokenInfoInitialize(tITool)
Tool *tITool;
{
  /*
   * Either say that no tool was selected, or clear tIMessage and
   * add the header for the tool.
   */
  if (!tITool)
    (void) strcpy(tIMessage, " -- no tool selected -- ");
  else
    {
      (void) strcpy(tIMessage, "Floor information for tool ");
      tokenInfoConcat(tITool->ToolCommand);
    }
}

static void tokenInfoWrapup()
{
  tokenInfoConcat("\n");
  UpdateMessage(tIMessage);
}

/*
 * tokenInfoBuild takes care of assembling all of the pieces of the tokenInfo message.
 *
 * First the header is built.
 *
 * Then each of the queued participants is added.
 */
void tokenInfoBuild(toolID, numQueued, queuedEntries)
BYTE     toolID;
short    numQueued;
BYTE     queuedEntries[256];
{
  /*
   * Build the part of the message for the token holder.
   *
   * This involves walking the list of subscribers until a match is made.
   */

  /*
   * tIBuildOffset is used with tokenInfoAddQueued to insure
   * no more than three names per line.
   * It is initialized with -1 due to the preincrement in the
   * tokenInfoAddQueued call.
   */
  int tIBuildOffset = -1;
  Tool *tITool;
  int queuedIndex;

  tITool = FindToolByID(toolID);

  tokenInfoInitialize(tITool);

  tokenInfoAddHolder(FindSubscriberByID(tITool->TokenHolder), 2);

  /*
   * If there is nobody queued for the token, then say so, otherwise start
   * spewing them into the message.
   */
  if ((!numQueued) || (queuedEntries[0] == tITool->TokenHolder))
    tokenInfoConcat("\n\nNo participants are queued for this tool.");
  else
    {
      tokenInfoConcat("\nQueued participants:");
      for (queuedIndex = 0; (queuedIndex < numQueued) && (queuedEntries[queuedIndex] != tITool->TokenHolder); queuedIndex++)
	{
	  /*
	   * Find the subscriber for this id, and then add that person's
	   * name to the message at the given offset.
	   *
	   * tIBuildOffset is used for formatting purposes by tokenInfoAddQueued.
	   */

	  /*
	   * TINUMIDSLINE is a #define from the declarations section. This sets
	   * the number of IDs on a line, after which the message goes to the next
	   * line.
	   */
	  tokenInfoAddQueued(FindSubscriberByID(queuedEntries[queuedIndex]), tIBuildOffset = ++tIBuildOffset % TINUMIDSLINE);
	}
    }
  tokenInfoWrapup();
}

/*
 * tokenInfo() is the entry point to the token information code.
 */
XtCallbackProc tokenInfo(widget, clientData, callData)
Widget widget;
XtPointer clientData, callData;
{
  XawListReturnStruct *selected;
  short numQueued;
  BYTE queuedEntries[256];
  TokenNode *tokenCursor;
  Tool *tITool;

  /*
   * Check to see that we indeed have a tool.
   */

  selected = XawListShowCurrent(WToolList);
  if (selected->list_index == XAW_LIST_NONE)
    tITool = NULL;
  else if ((tITool=FindTool(selected->string)) == NULL)
    FatalError("INTERNAL ERROR: List widget inconsistent with tool list.");

  /*
   * Set up the message buffer.
   * If we don't have a tool, insert a message to that effect and post the message.
   * If we do have a tool, then build the information about it.
   */

  if (tITool == NULL)
    {
      tokenInfoInitialize(tITool);
      tokenInfoWrapup();
    }
  else
  if (GetLocalSubID(tITool->TokenHolder) != HOST_ID)
    {
      /*
       * If we are a subscriber, then ask the host for the list of queued
       * subscribers. When the request is filled, the response will come
       * via HTOS_QueuedList ( htos.c ), which calls tokenInfoSubscr.
       */
      SendByte(GetHostConnection(), SH_QueuedList);
      SendByte(GetHostConnection(), tokInfo);
      SendByte(GetHostConnection(), tITool->ToolID);
    }
  else
    {
      tokenInfoInitialize(tITool);
/*
 *    for (startSubscr(tITool); !offrightSubscr(tITool); forthSubscr(tITool))
 *	{
 *	  tokenInfoConcat("\n");
 *	  tokenInfoConcat(getSubscr(tITool)->LoginName);
 *	}
 *    tokenInfoWrapup();
 */
      for (numQueued = 0, tokenCursor = tITool->TokenList; tokenCursor; tokenCursor = tokenCursor->Next, numQueued++)
	queuedEntries[numQueued] = tokenCursor->SubID;
      tokenInfoBuild(tITool->ToolID, numQueued, queuedEntries);
    }
}
