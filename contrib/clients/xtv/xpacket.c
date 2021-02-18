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
	xpacket.c
	The all-omnipotent(?) request packet translator

*/


#include <stdio.h>
#include <strings.h>
#define NEED_EVENTS
#define NEED_REPLIES
#include <X11/Xproto.h>
#include "tool.h"
#include "memory.h"
#include "misc.h"
#include "xpacket.h"
#include "xlspecial.h"

#define	DEBUGTranslateResourceID	0&DEBUGON
#define DEBUGTranslateColorPixel        0&DEBUGON
#define	DEBUGTranslateRequest		0&DEBUGON
#define DEBUGTranslateViewerEvent       0&DEBUGON
#define DEBUGTranslateReply	        0&DEBUGON

#define	DEBUGtoSTDOUT			0&DEBUGON
#if DEBUGtoSTDOUT
#define	TRACE_PLACE	stdout
#else
#define	TRACE_PLACE	stderr
#endif

extern XCOLOR;

int RequestType;        /* opcode of request. Useful when server replies to
                         * a request. */


/*
	A NOTE OF CAUTION ABOUT THE FOLLOWING #define:

	The X protocol states that the server's selection of resource ID
	basis is arbitrary and may not follow in any specific pattern.
	However, it seems that most of those distributed by MIT seem to
	keep this figure flush left in the ID.  Therefore, translation
	can be achieved more quickly by a couple of quick ANDs and ORs
	instead of the usual find-and-drop, which takes considerably
	more time.

	DEFINING FAST_XLATE TO 1 WILL MAKE FOR A MUCH FASTER PACKAGE BUT
	DOES NOT PROMISE 100% COMPATABILITY WITH THE X PROTOCOL.  IN 99
	OUT OF 100 CASES, THINGS SHOULD WORK WITHOUT DIFFICULTY, BUT IF
	YOU EXPERIENCE PROBLEMS, DISABLE FAST_XLATE BY DEFINING IT TO 0.
*/
#define	FAST_XLATE			01


#include "tables.h"
/* ----------------------------------------------------------------------------------------- */

/*
 *	SetStampAndSeq()
 *
 *	Extract the timestamp and sequence numbers from an
 *	event and update the "last" values in the tool
 *
 *	RETURN VALUE:	rv
 */

SetStampAndSeq( EventPacket, WhatTool )
xEvent *EventPacket;
Tool *WhatTool;
{
	CARD32	*TSAddress;

	if ( EventTable[EventPacket->u.u.type].HasSeqNumber ) {
		WhatTool->LastSeqNum = EventPacket->u.u.sequenceNumber;
	}

	if ( EventTable[EventPacket->u.u.type].TimeStampOffset ) {

		TSAddress = (CARD32 *)((BYTE *)EventPacket +
			(abs(EventTable[EventPacket->u.u.type].TimeStampOffset)));

		if (*TSAddress) {
			WhatTool->LastTimeStamp = *TSAddress;
		}
	}
WhatTool->LastSeqNum = EventPacket->u.u.sequenceNumber;

}


/*
 *	FixStampAndSeq()
 *
 *	Adjust the timestamp and sequence number if they exist in
 *	an event.
 *
 *	RETURN VALUE:	void
 */
void FixStampAndSeq( EventPacket, WhatTool )
BYTE *EventPacket;
Tool *WhatTool;
{
	CARD32	*TSAddress;

/*
	if ( EventTable[EventPacket->u.u.type].HasSeqNumber ) {
		EventPacket->u.u.sequenceNumber = ++(WhatTool->LastSeqNum);
	}

	if ( EventTable[EventPacket->u.u.type].TimeStampOffset ) {


		TSAddress = (CARD32 *)((BYTE *)EventPacket +
			(abs(EventTable[EventPacket->u.u.type].TimeStampOffset) * sizeof(XID)));

		if (*TSAddress) {
			*TSAddress = WhatTool->LastTimeStamp;
		}
	}
*/
/* just make it simple, attach sequence number with each packet sent to tool 
                ((xEvent *)EventPacket->u.u.sequenceNumber) = ++(WhatTool->LastSeqNum);

*/
}

/* ----------------------------------------------------------------------------------------- */


/*
 *	InitializePacketTranslator()
 *
 *	Initialize the packet translator by multiplying all of the
 *	offsets in the RequestTable and EventTable by sizeof(XID).
 *
 *	RETURN VALUE:	void
 */
void InitializePacketTranslator()
{
	int	Entry;
	int	Offset;

	for ( Entry=0; Entry < MAX_PACKET; Entry++ ) {
		for ( Offset=0; RequestTable[Entry].OffsetToID[Offset] != 0; Offset++ )
			RequestTable[Entry].OffsetToID[Offset] *= sizeof(XID);
	}

	for ( Entry=0; Entry < MAX_EVENT; Entry++ ) {
		for ( Offset=0; EventTable[Entry].OffsetToID[Offset] != 0; Offset++ )
			EventTable[Entry].OffsetToID[Offset] *= sizeof(XID);
		EventTable[Entry].TimeStampOffset *= 4;
	}

}


/*
 *	XID TranslateResourceID
 *
 *	Translate a resource ID from one server to another.
 *
 *	RETURN VALUE:	Translated XID
 */
XID TranslateResourceID( 	SourceServer,		TargetServer,
				SourceRoot,		TargetRoot,
				SourceVisualIDList,	TargetVisualIDList,
				SourceVisualIDCount,	TargetVisualIDCount,
                                SourceColorCellList,    TargetColorCellList,
                                SourceColorCellCount,   TargetColorCellCount,
				SourceMaskCount,	TargetMaskCount,
				SourceID )
xConnSetup *SourceServer;	/* Source server's resource ID base and mask info */
xConnSetup *TargetServer;	/* Target server's resource ID base and mask info */
xWindowRoot *SourceRoot;	/* Source server's root window ID */
xWindowRoot *TargetRoot;	/* Target server's root window ID */
XID *SourceVisualIDList;	/* Source server's visual ID list */
XID *TargetVisualIDList;	/* Target server's visual ID list */
CARD16 SourceVisualIDCount;	/* Source server's visual ID list count */
CARD16 TargetVisualIDCount;	/* Target server's visual ID list count */
CARD32 *SourceColorCellList;    /* Source server's color cell list */
CARD32 *TargetColorCellList;    /* Target server's color cell list */
CARD16 SourceColorCellCount;    /* Source server's color cell list count */
CARD16 TargetColorCellCount;    /* Target server's color cell list count */
BYTE SourceMaskCount;		/* Number of bits present in source resource ID mask */
BYTE TargetMaskCount;		/* Number of bits present in target resource ID mask */
XID SourceID;			/* Resource ID to be translated */
{
	XID	Target=0;	/* Working result */
	int	SourceBit=0;	/* Current bit number in source mask */
	int	TargetBit=0;	/* Current bit number in target mask */
	CARD16	WhichVisual;	/* Current visual number */
	CARD16  WhichColor;	/* Current colorcell number */
	int	WhichBit;	/* Current bit number */

	/* See if the bases match.  If they do, pull a brute-force translation and exit quick-like */

	if ( (SourceID & ~(SourceServer->ridMask)) == SourceServer->ridBase) {

#		if FAST_XLATE

#		if DEBUGTranslateResourceID 
			fprintf( TRACE_PLACE, "FAST TRANSLATION\n");
#               endif
		Target = (SourceID & SourceServer->ridMask) | TargetServer->ridBase;

#		else

		/* Stuff the target using whichever server has the smallest number of bits. */

		for ( WhichBit=((SourceMaskCount > TargetMaskCount) ? TargetMaskCount : SourceMaskCount);
			WhichBit > 0; WhichBit--, SourceBit++, TargetBit++ ) {

			/* Find the next "active" bit in each resource ID mask */

		   	for ( ; !(SourceServer->ridMask & (1 << SourceBit)) ; SourceBit++ );
		   	for ( ; !(TargetServer->ridMask & (1 << TargetBit)) ; TargetBit++ );

		   	/* Stuff the "new" bit if the source ID's bit is 1 */

			if (SourceID & (1 << SourceBit)) {
				Target |= (1 << TargetBit);
			}
		}

		/* Overlay the partially-built resource ID with the base of the target server */

		Target |= TargetServer->ridBase;

#		endif

#		if DEBUGTranslateResourceID 
			fprintf( TRACE_PLACE, "TransRID(): %08x --> %08x (BRUTE FORCE)\n",
				SourceID, Target );
#		endif

		return Target;
	}

	/* Is it the root window ID? */

	if ( SourceID == SourceRoot->windowId ) {
#		if DEBUGTranslateResourceID 
			fprintf( TRACE_PLACE, "TransRID(): %08x --> %08x (ROOT)\n",
				SourceID, TargetRoot->windowId );
#		endif
		return TargetRoot->windowId;
	}


	/* See if it's the default colormap */

	if ( SourceID == SourceRoot->defaultColormap ) {
#		if DEBUGTranslateResourceID 
			fprintf( TRACE_PLACE, "TransRID(): %08x --> %08x (DEF-CMAP)\n",
				SourceID, TargetRoot->defaultColormap );
#		endif
		return TargetRoot->defaultColormap;
	}


	/* See if it's the root window's visual */

	if ( SourceID == SourceRoot->rootVisualID ) {
#		if DEBUGTranslateResourceID
			fprintf( TRACE_PLACE, "TransRID(): %08x --> %08x (DEF_Visual)\n",
				SourceID, TargetRoot->rootVisualID );
#		endif
		return TargetRoot->rootVisualID;
	}

        if (XCOLOR) {
        /* Look for a color cell from the host */
            for ( WhichColor=0; WhichColor < SourceColorCellCount ; WhichColor++ ) {
#               if  DEBUGTranslateColorPixel
                        fprintf( TRACE_PLACE, "TransPixel: %08x <-> %08x (PIXELVALUE)\n",
                                SourceID, SourceColorCellList[WhichColor] );
#               endif
                if ( (SourceID) == SourceColorCellList[WhichColor] ) {
                        Target = TargetColorCellList[WhichColor];
#                       if DEBUGTranslateColorPixel
                                fprintf( TRACE_PLACE, "TransPixel:: %08x --> %08x (PIXELVALUE)\n",
                                        SourceID, Target );
#                       endif
                        return Target;
                }
            }
        }

	/* Look for a visual ID from the host */

	for ( WhichVisual=0 ; WhichVisual < SourceVisualIDCount ; WhichVisual++ ) {
#		if  DEBUGTranslateResourceID 
			fprintf( TRACE_PLACE, "TransRID(): %08x <-> %08x (VISUALID)\n",
				SourceID, SourceVisualIDList[WhichVisual] );
#		endif
		if ( (SourceID) == SourceVisualIDList[WhichVisual] ) {
			Target = TargetVisualIDList[
				(WhichVisual<TargetVisualIDCount ? WhichVisual : 0) ];
#			if DEBUGTranslateResourceID 
				fprintf( TRACE_PLACE, "TransRID(): %08x --> %08x (VISUALID)\n",
					SourceID, Target );
#			endif
			return Target;
		}
	}	 

	/* If we got here, things were totally mismatched.  LEAVE IT ALONE! */

	return SourceID;
}


/*
 *	TranslateForward()
 *
 *	Forward-translate a resource ID for a tool and return it.
 *
 *	RETURN VALUE:	Translated resource ID
 */
XID TranslateForward( WhatTool, SourceID )
Tool *WhatTool;
XID SourceID;
{
#               if DEBUGTranslateRequest
	fprintf( TRACE_PLACE, "TranslateForward():  Calling TranslateResourceID() %08x \n", SourceID );
#               endif
	return TranslateResourceID(
		&(WhatTool->HostConnectInfo),	&(WhatTool->LocalConnectInfo),
		&(WhatTool->HostRootInfo),	&(WhatTool->LocalRootInfo),
		WhatTool->VisualIDList,		WhatTool->LocalVisualIDList,
		WhatTool->NumVisualIDs,		WhatTool->LocalNumVisualIDs,
                WhatTool->ColorCellList,        WhatTool->LocalColorCellList,
                WhatTool->NumColorCells,        WhatTool->LocalNumColorCells,
		WhatTool->BitsInHostMask,	WhatTool->BitsInLocalMask,
		SourceID
	);
}


/*
 *	TranslateBackward()
 *
 *	Back-translate a resource ID for a tool and return it.
 *
 *	RETURN VALUE:	Translated resource ID
 */
XID TranslateBackward( WhatTool, SourceID )
Tool *WhatTool;
XID SourceID;
{
#               if DEBUGTranslateReply
	fprintf( TRACE_PLACE, "TranslateBackward():  Calling TranslateResourceID() %08x \n", SourceID );
#               endif
	return TranslateResourceID(
		&(WhatTool->LocalConnectInfo),	&(WhatTool->HostConnectInfo),
		&(WhatTool->LocalRootInfo),	&(WhatTool->HostRootInfo),
		WhatTool->LocalVisualIDList,	WhatTool->VisualIDList,
		WhatTool->NumVisualIDs,		WhatTool->NumVisualIDs,
                WhatTool->LocalColorCellList,   WhatTool->ColorCellList,
                WhatTool->LocalNumColorCells,   WhatTool->NumColorCells,
		WhatTool->BitsInLocalMask,	WhatTool->BitsInHostMask,
		SourceID
	);
}


/*
 *	TranslateRequest()
 *
 *	Translate a request in the forward direction and determine
 *	whether or not it needs to be sent to the server.
 *
 *	RETURN VALUE:	TRUE if the packet should be passed along
 *			FALSE if not
 */
Bool TranslateRequest( Packet, WhatTool )
BYTE *Packet;
Tool *WhatTool;
{
	xReq		*RequestHeader = (xReq *)Packet;
	int		Offset;

#	if DEBUGTranslateRequest
		fprintf( TRACE_PLACE, "TranslateRequest(): ---------------------- NEW PACKET\n" );
		fprintf( TRACE_PLACE, "TranslateRequest(): RequestHeader->reqType %d\n",
				RequestHeader->reqType );
#	endif



        RequestType = RequestHeader->reqType;

	/* First check to see if this is a DropDead */

	if ( (RequestTable[RequestHeader->reqType].ProcessFunc == ForceDropDead) ||
		(RequestHeader->reqType > MAX_PACKET) ) {
#		if DEBUGTranslateRequest
			if (RequestHeader->reqType > MAX_PACKET)
				fprintf( TRACE_PLACE, "TranslateRequest(): EXTENDED PACKET #%d\n",
					RequestHeader->reqType );
#		endif

		return FALSE;
	}

	/* Now pick through the offsets and translate the XIDs */

	for ( Offset=0 ; RequestTable[RequestHeader->reqType].OffsetToID[Offset] != 0 ; Offset++ ) {
		XID	*ResourceID;
		ResourceID = (XID *)(Packet+abs(RequestTable[RequestHeader->reqType].OffsetToID[Offset]));
                if ( (RequestTable[RequestHeader->reqType].OffsetToID[Offset] > 0) ||
                        (*ResourceID > 1) ) {
#               if DEBUGTranslateRequest
                fprintf( TRACE_PLACE, "TranslateRequest(): TranslateForward  offset [%d] of RequestTable[%d] - %s\n",
                             Offset, RequestHeader->reqType, RequestTable[RequestHeader->reqType].PacketName );
#               endif
			*ResourceID = TranslateForward( WhatTool, *ResourceID );


		}
	}
#		if DEBUGTranslateRequest
		fprintf( TRACE_PLACE, "TranslateRequest(): special function: RequestTable[%d] - %s\n",
                             RequestHeader->reqType, RequestTable[RequestHeader->reqType].PacketName );
#		endif

	/* If there's a special function, CALL IT! */

	if ( RequestTable[RequestHeader->reqType].ProcessFunc != NULL ) {
#		if DEBUGTranslateRequest
                fprintf( TRACE_PLACE, "TranslateRequest(): special function: RequestTable[%d] - %s\n",
                             RequestHeader->reqType, RequestTable[RequestHeader->reqType].PacketName );
#		endif
		RequestTable[RequestHeader->reqType].ProcessFunc( Packet, WhatTool );
	}

	return TRUE;
}


/*
 *	TranslateReply()
 *
 *	Translate a reply in the backwared direction.
 *
 */
Bool TranslateReply( Packet, WhatTool )
BYTE *Packet;
Tool *WhatTool;
{
        xGenericReply          *Reply = (xGenericReply *)Packet;
	int		Offset;

#       if DEBUGTranslateReply
                fprintf( TRACE_PLACE, "TranslateReply(): ---------------------- %s Reply\n",
                        ReplyTable[RequestType].PacketName );
#       endif

	/* Now pick through the offsets and translate the XIDs */

	for ( Offset=0 ; ReplyTable[RequestType].OffsetToID[Offset] != 0 ; Offset++ ) {
		XID	*ResourceID;
		ResourceID = (XID *)(Reply+abs(ReplyTable[RequestType].OffsetToID[Offset]));
		if ( (ReplyTable[RequestType].OffsetToID[Offset] > 0) || (*ResourceID > 1) ) {

#               if DEBUGTranslateReply
                fprintf( TRACE_PLACE, "TranslateReply(): Calling TranslateBackward  offset [%d] of ReplyTable[%d] - %s\n",
                             Offset, RequestType, ReplyTable[RequestType].PacketName );
#               endif
			*ResourceID = TranslateBackward( WhatTool, *ResourceID );
#               if DEBUGTranslateReply
                fprintf( TRACE_PLACE, "TranslateReply(): Called TranslateBackward  offset [%d] of ReplyTable[%d] - %s\n",
                             Offset, RequestType, ReplyTable[RequestType].PacketName );
#               endif

		}
	}

	/* If there's a special function, CALL IT! */

	if ( ReplyTable[RequestType].ProcessFunc != NULL ) {
#		if DEBUGTranslateReply
                fprintf( TRACE_PLACE, "TranslateReply(): special function: ReplyTable[%d] - %s\n",
                                 RequestType, ReplyTable[RequestType].PacketName );
#		endif
		ReplyTable[RequestType].ProcessFunc( Packet, WhatTool );
	}

	return TRUE;
}


/*
 *	TranslateViewerEvent()
 *
 *	Translate a Event in the reverse direction.  Check to see if
 *	the event is worth propogating and perform any necessary translations
 *	on it if needed.
 *
 *	RETURN VALUE:	TRUE if the packet should be passed along
 *			FALSE if not
 */
Bool TranslateViewerEvent( Packet, WhatTool )
BYTE *Packet;
Tool *WhatTool;
{
	xEvent		*Event = (xEvent *)Packet;
	int		Offset;

#	if DEBUGTranslateViewerEvent
		fprintf( TRACE_PLACE, "TranslateViewerEvent(): ---------------------- %s Event\n",
			EventTable[Event->u.u.type].EventName );
#	endif


	/* First check and see if we should deal with it */

	if ( (!EventTable[Event->u.u.type].Propogation) ||
		((EventTable[Event->u.u.type].Propogation == TOKEN) &&
			(WhatTool->TokenHolder != GetLocalSubID())) ) {
#		if DEBUGTranslateViewerEvent
			fprintf( TRACE_PLACE, "TranslateViewerEvent(): IGNORING EVENT %s\n",
				EventTable[Event->u.u.type].EventName );
#		endif
		return FALSE;
	}

	/* Now pick through the offsets and translate the XIDs */

	for ( Offset=0 ; EventTable[Event->u.u.type].OffsetToID[Offset] != 0 ; Offset++ ) {

		XID	*ResourceID;

		ResourceID = (XID *)(Packet+EventTable[Event->u.u.type].OffsetToID[Offset]);
                if ( (EventTable[Event->u.u.type].OffsetToID[Offset] > 0) ||
                        (*ResourceID > 1) ) {
		*ResourceID = TranslateBackward( WhatTool, *ResourceID );
#               if DEBUGTranslateViewerEvent
        fprintf( TRACE_PLACE, "TranslateViewerEvent(): TranslateBackward  offset [%d] of EventTable[%d] - %s\n",
                             Offset, Event->u.u.type, EventTable[Event->u.u.type].EventName );
#               endif

		}
	}

	/* If there's a special function, CALL IT! */

	if ( EventTable[Event->u.u.type].ViewerProcessFunc != NULL ) {
#		if DEBUGTranslateViewerEvent
			fprintf( TRACE_PLACE, "TranslateViewerEvent(): Calling special function\n" );
#		endif
		EventTable[Event->u.u.type].ViewerProcessFunc( Packet, WhatTool );
	}

	return TRUE;
}


/*
 *	DetermineHostRouting()
 *
 *	Check an event over to see if it should be kept or trashed by
 *	the host's event handler.  In other words, The answer is no if:
 * 	it's a token event and we don't have the token.
 *		
 *
 *	RETURN VALUE:	TRUE if the event should be sent
 *			FALSE otherwise.
 */
Bool DetermineHostRouting( WhatTool, EventPacket )
Tool *WhatTool;
xGenericReply *EventPacket;
{
	return  ( (EventTable[EventPacket->type].Propogation == TOKEN) &&
		(WhatTool->TokenHolder != HOST_ID) ? FALSE : TRUE );
}

/*
 *	DetermineViewerRouting()
 *
 *	Check an event over to see if it should be kept or trashed by
 *	the Viewer's event handler.  In other words, The answer is no if:
 * 	it's a token event and we don't have the token.
 *		
 *
 *	RETURN VALUE:	TRUE if the event should be sent
 *			FALSE otherwise.
 */
Bool DetermineViewerRouting( WhatTool, EventPacket, SubscriberID )
Tool *WhatTool;
xGenericReply *EventPacket;
BYTE SubscriberID;
{
	return(	(EventTable[EventPacket->type].Propogation == ALWAYS) ||
		(	(EventTable[EventPacket->type].Propogation == TOKEN) &&
			(WhatTool->TokenHolder == SubscriberID) ) ? TRUE : FALSE );

}
