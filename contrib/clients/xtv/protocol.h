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
	protocol.h
	Protocol-related definitions.  CONTAINS NO FUNCTION PROTOTYPES.

	INFORMATION RELATING TO THE ACTUAL OPERATION OF THE PROTOCOL CAN BE
	FOUND BOTH IN THE CODE AND IN THE FILE "PROTOCOL."

*/


/*
	The Connection Protocol
*/

#define XTV_SUBSCRIBER_LOW_PORT	 	10000  /* Lowest Port where new subscribers connect */
#define XTV_SUBSCRIBER_HIGH_PORT	10200  /* Highest Port where new subscribers connect */

#define	XTV_QUERY_MAGIC		0xfeedbeef	/* Magic number for connection query */
#define	XTV_CONNECT_MAGIC	0xdeadface	/* Magic number for connection */
#define R_XTV_CONNECT_MAGIC	0xcefaadde	/* reverse of XTV_CONNECT_MAGIC */


/*
	The General Control/Communications Protocol

	Everything here is in GROUPS, which are as follows:

	00x - Basic control mechanisms
	01x - Client and tool list management
	02x - Token passing mechanisms
	25x - Error and data availability condition markers used
	      internally.
*/

#define	MAX_PROTO_REQUEST 30

/* Host-To-Subscriber Protocol */

#define	HS_EndSession		0		/* Host has ended session */
#define	HS_DroppedYou		1		/* Host has removed subscriber */
#define	HS_X11Packet		2		/* X11 Packet follows */
#define	HS_InternalAtom		3		/* The internal atom thingy */
#define	HS_YouAre		4		/* Host has assigned a newcomer an ID */
#define HS_AllocColor           5               /* The color cell allocation stuff */
#define HS_AllocColorCells      6
#define HS_AllocColorPlanes     7

#define	HS_AddNewSubscriber	10		/* Initiate new client */
#define	HS_DeleteSubscriber	11		/* Delete existing client */
#define	HS_AddNewTool		12		/* Initiate a new tool */

/*
 * Begin Murray Addition
 */

#define HS_IncomingTools	13		/* List of tool names to follow. */

/*
 * End Murray Addition
 */

#define	HS_DeleteTool		14		/* Delete existing tool */

/*
 * Begin Murray Addition
 */

/*
 * #define	HS_StopTool		15		// Delete existing tool
 *
 * MFA - 10-VII-91 - I undefined this, since it is NEVER used.
 * It has been replaced by HS_EndIncoming.
 */

#define HS_EndIncoming		15		/* Done sending tools,
						   so unlock the message box */
#define HS_ToolsToJoin          16              /* Tools a subscriber can join */
#define HS_CatchUpStart         17		/* Start of catchup */
#define HS_CatchUpEnd           18		/* End of catchup */


/*
 * End Murray Addition
 */

#define	HS_TakeToken		20		/* Token has been passed */
#define	HS_ForceTokenRelease	21		/* Force token drop */
#define	HS_AskTokenRelease	22		/* Ask token drop */
#define HS_CreatorTakeToken	23		/* creator takes it, no one needs it */
#define HS_ContinueTakeToken	24		/* holder continue, no one needs it */

/*
 * Begin Murray Addition
 */
#define HS_QueuedList           25              /* Here's the next queued id. */
/*
 * End Murray Addition
 */


/* Viewer-To-Host Protocol */

#define	SH_LeavingSession	0		/* Subscriber is leaving session */
#define	SH_Event		1		/* Subscriber has an event to report */
#define	SH_Reply		2		/* Subscriber has an Reply to report */

/*
 * Begin Murray Addition
 */
#define SH_ConfirmNewTool	10	/* Subscriber is accepting the new tool./*
/*
 * End Murray Addition
 */

#define	SH_WantToken		20
#define SH_GiveTokenToMe        21
#define	SH_ReleasingToken	22
#define	SH_DeQueToken		23

/*
 * Begin Murray's Addition
 */
#define SH_QueuedList           25              /* Send the list of ids queued for the tool. */
/*
 * End Murray's Addition
 */

#define SH_NewToolLocal         12
#define SH_NewToolRemote        13
#define SH_TerminateTool        14
#define SH_StopTool        	15
#define SH_WantJoinTools        16
#define SH_JoinTools            17


#define SH_CatchUpAck		30


/*
	Things related to tool connections
*/

#define	XTV_LOW_DISPLAY		2		/* Lowest display number to use */
#define	XTV_HIGH_DISPLAY	100		/* Highest display number to uae */



/*
typedef	void	ProtocolHandler;
*/

/*
	Routines available in this module
*/

XtInputCallbackProc HandleStoHProtocol();
XtInputCallbackProc HandleHtoSProtocol();
