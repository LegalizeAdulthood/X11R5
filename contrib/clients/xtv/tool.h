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
	tool.h
	Declarations for tool management module

*/

#ifndef WINDOW
#define WINDOW          Window
#endif


#ifndef TOOL__H
#define TOOL__H

#include <X11/Xmd.h>
#include <X11/Xproto.h>
#include <X11/Intrinsic.h>
#include "network.h"
#include "objects.h"
/*
 * Begin Murray Addition
 */
#include "ptrlists.h"
#ifndef SUBSCRIBER__H
#include "subscriber.h"
#endif
/*
 * End Murray Addition
 */
#include "token.h"

#define	TOOL_CMD_SIZE	200
#define	HOST_NAME_SIZE	200

typedef struct _Tool {

	/* Stuff supplied by the host.  All of this is in the host's native
		 * data format (as specified by HostByteOrder) and will need to be
		 * converted by the local machine before use.  Note that any XIDs
		 * stored will not need conversions as the bits will already be
		 * appropriately screwed up.
		 */
	char			ToolCommand[100];	/* Command used to start tool */
	BYTE			ToolID;			/* ID number assigned to tool */
	BYTE			pad1;
	CARD16			pad2;
	int			ToolPID;		/* Process ID of tool */
	xConnClientPrefix	ConnectPrefix;		/* Connection prefix */
	CARD16			HowMuchAuthData;	/* Number of bytes of AuthData */
	CARD16			pad3;
	BYTE			*AuthData;		/* Pointer to authorization data */

	xConnSetup		HostConnectInfo;	/* First Part of connect info */
	xWindowRoot		HostRootInfo;		/* Host root window information */
        xPixmapFormat           *HostPixmapFormats;     /* Host pixmap formats infomation */
	CARD16			NumVisualIDs;		/* How many visual IDs in the list */
	CARD16			pad4;
	XID			*VisualIDList;		/* Array of visual IDs */
        CARD16                  NumColorCells;          /* How many color cells in the list */
        CARD16                  pad44;
        CARD32                  *ColorCellList;         /* Array of color cells */
	BYTE			BitsInHostMask;		/* Number of bits in Resource ID mask */
	BYTE			pad5;
	CARD16			pad6;

	/* These fields are used only at the host's end but are passed along
		 * anyway and replaced by the subscriber.
		 */
	Socket			InConnection;		/* Incoming connection from client */
	Socket			OutConnection;		/* Outgoing connection to server */
	TokenNode		*TokenList;		/* Token waiting list */
	BYTE			TokenHolder;		/* Subscriber ID of token holder */
	BYTE			pad7;
	CARD16			pad8;

	/* Stuff defined locally during the connection process
		 */
	xConnSetup		LocalConnectInfo;	/* Local Connect Info */
	xWindowRoot		LocalRootInfo;		/* Local Root info */
        xPixmapFormat           *LocalPixmapFormats;    /* Local Pixmap Formats Info  */
	CARD16			LocalNumVisualIDs;	/* Number of local visual IDs */
	CARD16			pad9;
	XID			*LocalVisualIDList;	/* List of Local Visual IDs */
        CARD16                  LocalNumColorCells;     /* Number of local color cells */
        CARD16                  pad99;
        CARD32                  *LocalColorCellList;    /* List of local Color cells */
	BYTE			BitsInLocalMask;	/* Number of bits in local RID mask */
	BYTE			pad10;
	CARD16			pad11;

	/* Stuff used for determining input availability and exceptions.
		 */
	XtInputId		InID;			/* ID for available client input */
	XtInputId		OutID;			/* ID for available server input */
	XtInputId		InExceptID;		/* ID for client exceptions */
	XtInputId		OutExceptID;		/* ID for server exceptions */

	/* Stuff used by the host in translating events
		 */

	CARD16			LastSeqNum;		/* Last known sequence number */
	CARD16			pad12;
	CARD32			LastTimeStamp;		/* Last known time stamp */
	int			pending_reply;		/* Do't switch token if TRUE */
	int			server_pending_reply;
	int			InCount;		/* number of Incoming packets */
	int			OutCount;		/* number of Outgoing packets */

        /* Stuff used to calculate the correct position of Pop-up menu
                */

        BOOL                    FindAbsoluteWindow;
	BYTE			pad13;
	CARD16			pad14;
        Window                  AbsoluteWindow;

	/* Stuff used to archive packets */

	ToolState		*ToolInfo;
	char			*ArchiveBuf;		/* pointer to packet buffer */
	off_t			ArchiveSize;		/* size of the archive */
	char			*Archive2Buf;		/* pointer to packet buffer to be sent after an expose event occurs */
	off_t			Archive2Size;		/* size of archive2 */
	int			SubDirtyCount;		/* number of new subscribers catching up on this tool */
	int			ThisToolDead;		/* flag to indicate this tool is dead */

	/* Stuff needed to get archives */

	int			CatchUpMode;		/* flag to indicate that this subscriber is in CatchUpMode for this tool */

	/* AKR 05/08 Stuff to do floor policies */

	XtIntervalId		floor_timer;		/* handle for timer routine for this tool */

	BYTE                    ToolCreator;            /* Subscriber ID of creator */
	BOOL                    AskedToGetToken;            /* sent asking for token */
        CARD16                   pad15;
	/*
	 * Begin Murray Addition
	 */
	PTRLIST                *subscrList;
	/*
	 * A list of subscribers who use this tool. 
	 * This list is valid only on the host.
	 */

	/*
	 * End Murray Addition
	 */
} Tool;


typedef struct _ToolNode {
	Tool			*ToolS;
	struct _ToolNode	*Next;
} ToolNode;

Tool *FirstTool();
Tool *NextTool();
void AddTool();
void DeleteTool();
Tool *FindTool();
void RebuildToolList();
void InitializeTool();
XtInputCallbackProc ToolCheck();
XtInputCallbackProc ConfirmFryTool();
void CloseAllTools();
BYTE AssignToolID();
void SetToolID();
void ClearToolID();
Tool *FindToolByID();
void StartLocalTool();
void StartRemoteTool();
void CleanUpTool();
void CleanUpAllTools();

#ifndef NoIDAvailable
#define	NoIDAvailable		255
#endif

#endif /* TOOL__H */

