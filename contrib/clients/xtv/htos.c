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
	htos.c
	Subscriber-end rotuines for the host-to-subscriber protocol

*/

#include <stdio.h>

#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include "subscriber.h"
#include "tool.h"
#include "network.h"
#include "protocol.h"
#include "token.h"
#include "memory.h"
#include "misc.h"
#include "packet.h"
#include "htos.h"

/*
 * Begin Murray Addition
 */
#include "tokenInfo.h"
#include "particInfo.h"
/*
 * End Murray Addition
 */

#define	DEBUGHTOS_AddNewTool		01&DEBUGON
#define	DEBUGHTOS_DeleteTool		01&DEBUGON
#define	DEBUGHTOS_DroppedYou		01&DEBUGON
#define	DEBUGHTOS_X11Packet		01&DEBUGON
#define DEBUGHTOS_X11_PopUp_Packet	01&DEBUGON
#define	DEBUGHTOS_InternalAtom		01&DEBUGON
#define DEBUGHTOS_AllocColor            01&DEBUGON
#define	DEBUGHTOS_TakeToken		01&DEBUGON
#define	DEBUGHTOS_ForceTokenRelease	01&DEBUGON
#define	DEBUGHTOS_AskTokenRelease	01&DEBUGON
#define DEBUGHTOS_YouAre		01&DEBUGON
#define	DEBUGHTOS_CreatorTakeToken	01&DEBUGON

extern int has_reply[];
extern VERBOSE;
extern POPOFF;
int CatchUp = 0;
extern TimeOutFlag;
extern HostDisplayNumber;
#ifndef WINDOW
#define WINDOW  Window
#endif

#ifdef sun3
#define SUN
#endif

#ifdef sun4
#define SUN
#endif

#ifdef dec5000
#define DEC
#endif

#ifdef rs6000
#define IBM
#endif

/*
 *	HTOS_EndSession()
 *
 *	The chairman has ended the session.  Clean up NEATLY and
 *	exit.
 *
 *	Protocol:	HS_EndSession
 *
 *		HS_EndSession		Protocol ID Tag
 *
 *	RETURN VALUE:	void
 */
void HTOS_EndSession( Port, Who )
Socket Port;
Subscriber *Who;
{
	extern Widget	WSubscriber;
	extern Widget	WTool;
	Arg		args[5];
	int		n=0;

	XtSetArg( args[n],      XtNsensitive,           FALSE );
	n++;
	XtSetValues( WSubscriber, args, n );
	XtSetValues( WTool, args, n );

	CloseAllTools();

	UpdateMessage( "The Host has ended the session.\n Press [Exit XTV] to exit." );
}


/*
 *	HTOS_DroppedYou()
 *
 *	Somebody has booted you out.
 *
 *	Protocol:	HS_DroppedYou
 *
 *		HS_DroppedYou		Protocol ID Tag
 *
 *	RETURN VALUE:	void
 */
void HTOS_DroppedYou( Port, Who )
Socket Port;
Subscriber *Who;
{
	extern Widget	WSubscriber;
	extern Widget	WTool;
	Arg		args[5];
	int		n=0;

	BYTE            *TargetID;
	BYTE            *LeftOrEjected;
	Subscriber      *WhoItIs;

	TargetID = (BYTE *)ReadData( Port, sizeof(BYTE), BLOCK );

	if ( (WhoItIs=FindSubscriberByID(*TargetID)) != NULL ) {
#       if DEBUGHTOS_DroppedYou
		fprintf( stderr,"%s has been ejected  from the session\n", WhoItIs->LoginName);
#       endif
	} else {
#       if DEBUGHTOS_DroppedYou
		fprintf( stderr,"WARNING:  Subscriber ID %d unknown.", *TargetID);
#       endif
	}


	XtSetArg( args[n],      XtNsensitive,           FALSE );
	n++;
	XtSetValues( WSubscriber, args, n );
	XtSetValues( WTool, args, n );
	UpdateMessage( "You have been ejected from the session.\n Press [Exit XTV] to exit." );
	CloseAllTools();
}


/*
 *	HTOS_AddNewSubscriber()
 *
 *	Get a new subscriber structure from the host and add it to the list
 *
 *	Protocol:	HS_AddNewSubscriber | NewDude
 *
 *		HS_AddNewSubscriber		Protocol ID Tag
 *		NewDude			Subscriber Structure
 *
 *	RETURN VALUE:	void
 */
void HTOS_AddNewSubscriber( Port, Who )
Socket Port;
Subscriber *Who;
{
	Subscriber	*NewDude;

	NewDude = (Subscriber *)ReadData( Port, sizeof(Subscriber), BLOCK );

	AddSubscriber( NewDude );
	SetSubID( NewDude->SubscriberID, NewDude );

	UpdateMessage( "%s has joined the session.", NewDude->LoginName );
}


/*
 *	HTOS_DeleteSubscriber()
 *
 *	Cook a subscriber.
 *
 *	Protocol:	HS_DeleteSubscriber | TargetID | LeftOrEjected
 *
 *		HS_DeleteSubscriber	Protocol ID Tag
 *		TargetID		Subscriber ID of victim
 *		LeftOrEjected		00 - Left session
 *					01 - Ejected by chairman
 *
 *	RETURN VALUE:	void
 */
void HTOS_DeleteSubscriber( Port, Who )
Socket Port;
Subscriber *Who;
{
	BYTE		*TargetID;
	BYTE		*LeftOrEjected;
	Subscriber	*WhoItIs;

	TargetID = (BYTE *)ReadData( Port, sizeof(BYTE), BLOCK );
	LeftOrEjected = (BYTE *)ReadData( Port, sizeof(BYTE), BLOCK );

	if ( (WhoItIs=FindSubscriberByID(*TargetID)) != NULL ) {
		UpdateMessage( "%s has %s the session", WhoItIs->LoginName, ( *LeftOrEjected  ? "been ejected from" : "left" ) );
	} else {
		UpdateMessage( "WARNING:  Subscriber ID %d unknown.", *TargetID );
	}

	DeleteSubscriber( WhoItIs );
}



/*
 *	HTOS_AddNewTool()
 *
 *	Get a new tool structure from the host and add it to the list
 *
 *	Protocol:	HS_AddNewTool | NewTool | AuthData | VisualIDList
 *
 *		HS_AddNewTool		Protocol ID Tag
 *		NewTool			Tool Structure
 *		AuthData		Authorization data from connect request
 *		VisualIDList		List of Visual IDs
 *
 *	RETURN VALUE:	void
 */
void HTOS_AddNewTool( Port, Who )
     Socket Port;
     Subscriber *Who;
{
  Tool	*NewTool;
  BYTE	ByteOrder;
  
#if DEBUGHTOS_AddNewTool
  fprintf( stderr, "HTOS_AddNewTool(): Reading Tool Info\n" );
#endif

  NewTool = (Tool *)ReadData( Port, sizeof(Tool), BLOCK );
  
  /*
   * Begin Murray Addition
   */
  
  /*
   * We don't want to mess with the subscriber list that the host maintains.
   */
  
  NewTool->subscrList = 0L;
  
  /*
   * End Murray Addition
   */
  
  NewTool->CatchUpMode = CatchUp;
  CatchUp = 0;
  
  ByteOrder = NewTool->ConnectPrefix.byteOrder;
  
  NewTool->HowMuchAuthData = (CARD16)SwapInt( NewTool->HowMuchAuthData, ByteOrder );
  NewTool->NumVisualIDs    = (CARD16)SwapInt( NewTool->NumVisualIDs,    ByteOrder );
  
#if DEBUGHTOS_AddNewTool
  fprintf( stderr, "HTOS_AddNewTool(): ToolCommand Name [%s]\n", (NewTool->ToolCommand));
  fprintf( stderr, "HTOS_AddNewTool(): ToolPID  [%d]\n", (NewTool->ToolPID));
  fprintf( stderr, "HTOS_AddNewTool(): Authorization Data is [%04x]\n", (NewTool->HowMuchAuthData) );
  fprintf( stderr, "HTOS_AddNewTool(): Authorization Data has [%d bytes]\n", (int)(NewTool->HowMuchAuthData) );
  fprintf( stderr, "HTOS_AddNewTool(): Tool Visual ID List is [%04x]\n", (NewTool->NumVisualIDs) );
  fprintf( stderr, "HTOS_AddNewTool(): Tool Visual ID List has [%d elements]\n", (int)(NewTool->NumVisualIDs) );
#endif
  
#if DEBUGHTOS_AddNewTool
  fprintf( stderr, "HTOS_AddNewTool(): Reading Tool Authorization Data [%d bytes]\n", (int)NewTool->HowMuchAuthData );
#endif
  
  NewTool->AuthData = ReadData( Port, NewTool->HowMuchAuthData, BLOCK );

#if DEBUGHTOS_AddNewTool
  fprintf( stderr, "HTOS_AddNewTool(): Reading Tool Visual ID List [%d elements]\n", (int)NewTool->NumVisualIDs );
#endif

  NewTool->VisualIDList = (XID *)ReadData( Port, ((NewTool->NumVisualIDs) * sizeof(XID)), BLOCK );

#if DEBUGHTOS_AddNewTool
  fprintf( stderr, "HTOS_AddNewTool(): The VisualIDList Contains %d items\n", NewTool->NumVisualIDs);
  fprintf( stderr, "HTOS_AddNewTool(): The VisualIDList elements are\n");
  
  { int i;
    i = NewTool->NumVisualIDs;
    while (i)
      printf ("%08x\n",NewTool->VisualIDList[NewTool->NumVisualIDs - i--]);
  }
  
#endif
  
  
  if ( ProcessClientConnection(NewTool) < 0 ) {
    /* xyzzy - Free contents of tool structure */
    return;
  }

#if DEBUGHTOS_AddNewTool
  fprintf( stderr, "HTOS_AddNewTool(): The Local VisualIDList Contains %d items\n", NewTool->LocalNumVisualIDs);
  fprintf( stderr, "HTOS_AddNewTool(): The Local VisualIDList elements are\n");
  { int i;
    i = NewTool->LocalNumVisualIDs;
    while (i)
      printf ("%08x\n",NewTool->LocalVisualIDList[NewTool->LocalNumVisualIDs - i--]);
  }
  
#endif
  
  SetToolID( NewTool );
  AddTool( NewTool );
  
  UpdateMessage( "Tool %s has just been connected.", NewTool->ToolCommand );
  
  /*
   * Begin Murray Addition
   */
  
  /*
   * Confirm the acceptance of the new tool. This tells the host to add me
   * to the list of people interested in this tool. Note that this should go
   * to a dialog box in the future. However, there will be a problem with the
   * code that follows, especially if more than one tool request is coming
   * down the pipe. We will want to pass the tool id as a parameter to the
   * dialog popup, so that we can later get the info.
   */
  
  if (SendByte(GetHostConnection(), SH_ConfirmNewTool) == ERROR) 
    EndSession();
  if (SendByte(GetHostConnection(), NewTool->ToolID) == ERROR)
    EndSession();
  
  /*
   * End Murray Addition
   */
  
  if (NewTool->ToolCreator == GetLocalSubID()) {
    if (SendByte(GetHostConnection(), SH_GiveTokenToMe)==ERROR) 
      EndSession();
    if (SendByte(GetHostConnection(), NewTool->ToolID)==ERROR) 
      EndSession();
  }
  
  if( NewTool->CatchUpMode == 1 )
    if (SendByte(GetHostConnection(),SH_CatchUpAck)==ERROR)
      EndSession();
}


/*
 *	HTOS_DeleteTool()
 *
 *	Cook a Tool.
 *
 *	Protocol:	HS_DeleteTool | TargetID
 *
 *		HS_DeleteTool		Protocol ID Tag
 *		TargetID		Tool ID of victim
 *		LeftOrEjected		00 - Left session
 *					01 - Ejected by chairman
 *
 *	RETURN VALUE:	void
 */
void HTOS_DeleteTool( Port, Who )
Socket Port;
Subscriber *Who;
{
	BYTE		ToolID;
	Tool		*WhichTool;

	ToolID = GetByte( Port);
#	if DEBUGHTOS_DeleteTool
	fprintf( stderr, "HTOS_DeleteTool(): ToolId to be deleted is: %d\n", 
	    ToolID);
#	endif

	if ( (WhichTool=FindToolByID(ToolID)) == NULL ) {
#	if DEBUGHTOS_DeleteTool
		fprintf(stderr, "WARNING:  Tool ID %d unknown.", ToolID );
#	endif
		return;
	}
	if( WhichTool->CatchUpMode == 1 ) if (SendByte(GetHostConnection(),SH_CatchUpAck)==ERROR) EndSession();
	CleanUpTool( WhichTool );

#       if DEBUGHTOS_DeleteTool
        fprintf( stderr, "HTOS_DeleteTool(): Cleaned Up \n");
#       endif


}


/*
 *	HTOS_X11Packet()
 *
 *	Cook a Tool.
 *
 *	Protocol:	HS_X11Packet | Tool ID | Length | Packet
 *
 *		HS_X11Packet		Protocol ID Tag
 *		ToolID			Tool ID of sender
 *		Length			32-bit packet length in byte-order of tool
 *		Packet			X11 Packet
 *
 *	RETURN VALUE:	void
 */
void HTOS_X11Packet( Port, Who )
Socket Port;
Subscriber *Who;
{
	BYTE		*ToolID;
	Tool		*WhichTool;
	CARD32		*Length;
	BYTE		*Packet;
	xReq            *PHeader;
        INT16           temp;
        CARD32          BitMask;
        int             BitNumber;
        int             WhereAt = 0;
        XWindowAttributes  Win_Attr;
        int             abs_x,abs_y;
        Window          dummywin;
        Window          root;
        Window          parent;
        Window          children;
        unsigned        int     nchildren;
	int		returnVal;



	ToolID = (BYTE *)ReadData( Port, sizeof(BYTE), BLOCK );
	if ( (WhichTool=FindToolByID(*ToolID)) == NULL ) {
		/* xyzzy - for now trash this packet, it should't have
		arrived to us in the first place, the efficient fix
		is to let the host rcognize this and do't send it to us */
#       if DEBUGHTOS_X11Packet
		fprintf(stderr,  "X11Packet(): Tool ID %d unknown.", *ToolID );
#	endif
        FreeMem(ToolID);
	TrashIt(Port);
	return;
	}
	Length = (CARD32 *)ReadData( Port, sizeof(CARD32), BLOCK );
	*Length = SwapLong( *Length, WhichTool->ConnectPrefix.byteOrder );
	Packet = (BYTE *)ReadData( Port, *Length, BLOCK );

if ( VERBOSE ){
        if ( (++(WhichTool->InCount) % 20) == 0) printf("%s --> %d\n", WhichTool->
ToolCommand, WhichTool->InCount);
}

#       if DEBUGHTOS_X11Packet
        fprintf( stderr, "HTOS_X11Packet(): OpCode [%d], Length [%d], has_reply [%s]\n", *Packet, *Length+32,
			(has_reply[*Packet] ? "TRUE" : "FALSE"));
#       endif

	if ( TranslateRequest( Packet, WhichTool ) ) {

                if ((!POPOFF)&&( *Packet == 120)) {
#       if DEBUGHTOS_X11_PopUp_Packet
        fprintf( stderr, "HTOS_X11Packet(): PopUpInfo Packet received by token holder\n");
#       endif


                        XQueryTree(XtDisplay((Widget *)GetTopLevelWidget()),SwapLong(*((Window *)(Packet+4)),WhichTool->ConnectPrefix.byteOrder),
                                         &root, &parent, &children, &nchildren);
                        XGetWindowAttributes(XtDisplay((Widget *)GetTopLevelWidget()),SwapLong(*((Window *)(Packet+4)),WhichTool->ConnectPrefix.byteOrder),&Win_Attr);
                        if (root != parent) {
                        XTranslateCoordinates(XtDisplay((Widget *)GetTopLevelWidget()),
				SwapLong(*((WINDOW *)(Packet+4)),WhichTool->ConnectPrefix.byteOrder),
				SwapLong(WhichTool->LocalRootInfo.windowId,WhichTool->ConnectPrefix.byteOrder),
				Win_Attr.x, Win_Attr.y, &abs_x, &abs_y, &children);
                        Win_Attr.x = abs_x;
                        Win_Attr.y = abs_y;
                        }
                        *((INT16 *)(Packet+8)) = SwapInt(SwapInt(*((INT16 *)(Packet+8)),WhichTool->ConnectPrefix.byteOrder) - Win_Attr.x,FROMLOCAL(WhichTool->ConnectPrefix.byteOrder));
                        *((INT16 *)(Packet+10)) = SwapInt(SwapInt(*((INT16 *)(Packet+10)),WhichTool->ConnectPrefix.byteOrder) - Win_Attr.y,FROMLOCAL(WhichTool->ConnectPrefix.byteOrder));
                        if (SendData(Port, Packet, sizeof(PopUpInfo)) == ERROR) {
#       if DEBUGHTOS_X11_PopUp_Packet
        fprintf( stderr, "HTOS_X11Packet():ERROR in sending PopUpReply Packet back to host");
#       endif
CleanUpTool(WhichTool);
return;
}
                        FreeMem( ToolID );
                        FreeMem( Length );
                        FreeMem( Packet );
                        return;
                }
                if ((!POPOFF)&&( *Packet == 1)) {
                   BitMask = SwapLong(*((CARD32 *)(Packet+7*4)),WhichTool->ConnectPrefix.byteOrder);
                   if (BitMask & (1 << 9)) {
                        for ( BitNumber=0; BitNumber < 9; BitNumber++ ) 
                                if ( BitMask & (1 << BitNumber) ) 
                                        WhereAt++;

#ifdef SUN
                        if (*((BOOL *)(Packet+8*4+WhereAt*4+3)) == TRUE) {
#endif

#ifdef DEC
                        if (*((BOOL *)(Packet+8*4+WhereAt*4)) == TRUE) {
#endif
#ifdef IBM
                        if (*((BOOL *)(Packet+8*4+WhereAt*4+3)) == TRUE) {
#endif



#       if DEBUGHTOS_X11_PopUp_Packet
        fprintf( stderr, "HTOS_X11Packet():A PopUp Window REquest received:Win==%d\n",*(WINDOW *)(Packet+8));
#       endif

                            XQueryTree(XtDisplay((Widget *)GetTopLevelWidget()),SwapLong(*((Window *)(Packet+8)),WhichTool->ConnectPrefix.byteOrder),
                                          &root, &parent, &children, &nchildren);
                            XGetWindowAttributes(XtDisplay((Widget *)GetTopLevelWidget()),SwapLong(*((Window *)(Packet+8)),WhichTool->ConnectPrefix.byteOrder),&Win_Attr);
                            if (root != parent) {
                            XTranslateCoordinates(XtDisplay((Widget *)GetTopLevelWidget()),SwapLong(*((WINDOW *)(Packet+8)),WhichTool->ConnectPrefix.byteOrder),
				SwapLong(WhichTool->LocalRootInfo.windowId,WhichTool->ConnectPrefix.byteOrder), Win_Attr.x, Win_Attr.y, &abs_x, &abs_y, &children);
                            Win_Attr.x = abs_x;
                            Win_Attr.y = abs_y;
                            }

#       if DEBUGHTOS_X11_PopUp_Packet
        fprintf( stderr,"LocalToolAbsoluteWindow is ========%d\n",*((WINDOW *)(Packet+8)));
        fprintf( stderr,"LocalToolWindowPosition:X==%d,Y==%d\n",Win_Attr.x,Win_Attr.y);
#   endif
                            *((INT16 *)(Packet+12)) = SwapInt(SwapInt(*((INT16 *)(Packet+12)),WhichTool->ConnectPrefix.byteOrder) + Win_Attr.x,FROMLOCAL(WhichTool->ConnectPrefix.byteOrder));
                            *((INT16 *)(Packet+14)) = SwapInt(SwapInt(*((INT16 *)(Packet+14)),WhichTool->ConnectPrefix.byteOrder) + Win_Attr.y,FROMLOCAL(WhichTool->ConnectPrefix.byteOrder));
#       if DEBUGHTOS_X11_PopUp_Packet
        fprintf( stderr, "HTOS_X11Packet():The LocalRootWindowId :Win==%d\n",(WhichTool->LocalRootInfo).windowId);
#       endif

                            *((WINDOW *)(Packet+8)) = WhichTool->LocalRootInfo.windowId;
                        }
                   }

                }
		if((returnVal=TimedSendData( WhichTool->OutConnection, Packet, *Length ))==ERROR) CleanUpTool(WhichTool);
                else if (returnVal == TIMEOUT) {
			if ( SendByte(GetHostConnection(), SH_StopTool)==ERROR){
                                EndSession();
                                return;
                        }
                        if ( SendByte(GetHostConnection(), WhichTool->ToolID)==ERROR){
                                EndSession();
                                return;
                        }
                        TimeOutFlag = TRUE;
                        CleanUpTool(WhichTool);
                        TimeOutFlag = FALSE;
                }

	}

	FreeMem( ToolID );
	FreeMem( Length );
	FreeMem( Packet );
	if ( WhichTool->CatchUpMode == 1 ) if (SendByte(GetHostConnection(),SH_CatchUpAck)==ERROR) EndSession();
	

}


/*
 *	HTOS_InternalAtom()
 *
 *	Cook a Tool.
 *
 *	Protocol:	HS_InternalAtom | Tool ID | AtomValue | Length | AtomName
 *
 *		HS_InternalAtom		Protocol ID Tag
 *		ToolID			Tool ID of sender
 *		AtomValue		32-bit host atom value in tool's byte order
 *		Length			16-bit name length in tool's byte order
 *		AtomName		Name of atom, "Length" bytes.
 *
 *	RETURN VALUE:	void
 */
void HTOS_InternalAtom( Port, Who )
Socket Port;
Subscriber *Who;
{
	BYTE		*ToolID;
	Tool		*WhichTool;
	CARD32		*AtomValue;
	CARD16		*Length;
	BYTE		*AtomName;
	CARD32		LocalAtomValue;
	int		i;

#	if DEBUGHTOS_InternalAtom
	fprintf( stderr, "HTOS_InternalAtom()\n" );
#	endif

	ToolID = (BYTE *)ReadData( Port, sizeof(BYTE), BLOCK );
	if ( (WhichTool=FindToolByID(*ToolID)) == NULL ) {
		/* xyzzy - for now trash this packet, it should't have
		arrived to us in the first place, the efficient fix
		is to let the host rcognize this and do't send it to us */
#	if DEBUGHTOS_InternalAtom
		fprintf(stderr,  "HTOS_InternalAtom(): Tool ID %d unknown.", *ToolID );
#	endif
        FreeMem(ToolID);
	TrashIt(Port);
	return;
	}
	AtomValue = (CARD32 *)ReadData( Port, sizeof(CARD32), BLOCK );
	Length = (CARD16 *)ReadData( Port, sizeof(CARD16), BLOCK );
	*Length = SwapInt(*Length,WhichTool->ConnectPrefix.byteOrder);
#	if DEBUGHTOS_InternalAtom
	fprintf( stderr, "HTOS_InternalAtom(): Receiving string with %d bytes\n", *Length );
#	endif
	AtomName = (BYTE *)ReadData( Port, *Length, BLOCK );

#	if DEBUGHTOS_InternalAtom
	fprintf( stderr, "HTOS_InternalAtom(): Host's atom %08x = '%s'\n", (XID)(*AtomValue),
	    AtomName );
#	endif

	/* Grab the value of the atom from the local display server */

	LocalAtomValue = XInternAtom( XtDisplay((Widget *)GetTopLevelWidget()),
	    (char *)AtomName, TRUE );

	LocalAtomValue = SwapLong( LocalAtomValue, FROMLOCAL(WhichTool->ConnectPrefix.byteOrder) );

	/* Add it to the list, but put it at the HEAD of the list to make the
	 * resource ID translator run at its best.
	 */

	WhichTool->VisualIDList = (XID *)realloc( WhichTool->VisualIDList,
	    (++(WhichTool->NumVisualIDs) * sizeof(XID) ) );
	for ( i=(WhichTool->NumVisualIDs-1); i > 0; i-- ) {
		WhichTool->VisualIDList[i] = WhichTool->VisualIDList[i-1];
	}
	WhichTool->VisualIDList[0] = *AtomValue;

	WhichTool->LocalVisualIDList = (XID *)realloc( WhichTool->LocalVisualIDList,
	    (++(WhichTool->LocalNumVisualIDs) * sizeof(XID) ) );
	for ( i=(WhichTool->LocalNumVisualIDs-1); i > 0; i-- ) {
		WhichTool->LocalVisualIDList[i] = WhichTool->LocalVisualIDList[i-1];
	}
	WhichTool->LocalVisualIDList[0] = LocalAtomValue;

#	if DEBUGHTOS_InternalAtom
	fprintf( stderr, "HTOS_InternalAtom(): Host's atom %08x --> %08x\n", AtomValue,
	    LocalAtomValue );
#	endif

	/* Clean up and go home */

	FreeMem( ToolID );
	FreeMem( AtomValue );
	FreeMem( Length );
	FreeMem( AtomName );
	if( WhichTool->CatchUpMode == 1 ) if (SendByte(GetHostConnection(),SH_CatchUpAck)==ERROR) EndSession();

}


/*
 *      HTOS_AllocColor()
 *
 *      Cook a Tool.
 *
 *      Protocol:       HS_AllocColor | ToolID | Colormap | NameTag |
 *                      (Red | Green | Blue)or(Length | ColorName) | PixelValue
 *
 *              HS_AllocColor           Protocol ID Tag
 *              ToolID                  Tool ID of sender
 *              Colormap                Protocol Request Parameter(needs to be translated)
 *              NameTag                 Indicate whether specify color by name or by RGB value
 *              (Red | Green | Blue)/(Length | ColorName)
 *                                      Protocol Request Parameters
 *              PixelValue              ColorCell index in host's colormap
 *
 *      RETURN VALUE:   void
 */
void HTOS_AllocColor( Port, Who )
Socket Port;
Subscriber *Who;
{
        BYTE            *ToolID;
        Tool            *WhichTool;
        CARD32          *PixelValue;
        Colormap        *CMap;
        BYTE            *PrimaryColors;
        CARD32          *NameTag;
        CARD16          *Length;
        CARD16          *Pad;
        BYTE            *ColorName;
        XColor          ColorCellDef;
        XColor          RgbDbDef;
        CARD32          LocalPixelValue;

#       if DEBUGHTOS_AllocColor
        fprintf( stderr,  "HTOS_AllocColor()\n" );
#       endif

        ToolID = (BYTE *)ReadData( Port, sizeof(BYTE), BLOCK );
        if ( (WhichTool=FindToolByID(*ToolID)) == NULL ) {
                /* xyzzy - Tell the host we're bombing out */
                FatalError( "HTOS_AllocColor: Tool ID %d unknown.", *ToolID );
        }

        CMap = (Colormap *)ReadData( Port, sizeof(Colormap), BLOCK );
        *CMap = TranslateResourceID(
                &(WhichTool->HostConnectInfo),   &(WhichTool->LocalConnectInfo),
                &(WhichTool->HostRootInfo),      &(WhichTool->LocalRootInfo),
                WhichTool->VisualIDList,         WhichTool->LocalVisualIDList,
                WhichTool->NumVisualIDs,         WhichTool->LocalNumVisualIDs,
                WhichTool->ColorCellList,        WhichTool->LocalColorCellList,
                WhichTool->NumColorCells,        WhichTool->LocalNumColorCells,
                WhichTool->BitsInHostMask,       WhichTool->BitsInLocalMask,
                *CMap);
#       if DEBUGHTOS_AllocColor
        fprintf( stderr,  "HTOS_AllocColor(): CMap Translated.\n");
#       endif

	*CMap = SwapLong( *CMap, WhichTool->ConnectPrefix.byteOrder );
        NameTag = (CARD32 *)ReadData( Port, sizeof(CARD32), BLOCK );
	*NameTag = SwapLong(*NameTag, WhichTool->ConnectPrefix.byteOrder);
        if (*NameTag == 0) {
                PrimaryColors = (BYTE *)ReadData( Port, 4*sizeof(CARD16), BLOCK );
                ColorCellDef.red = SwapInt(*((CARD16 *)(PrimaryColors)),
					WhichTool->ConnectPrefix.byteOrder);
                ColorCellDef.green = SwapInt(*((CARD16 *)(PrimaryColors+2)),
					WhichTool->ConnectPrefix.byteOrder);
                ColorCellDef.blue = SwapInt(*((CARD16 *)(PrimaryColors+4)),
					WhichTool->ConnectPrefix.byteOrder);
                XAllocColor(XtDisplay((Widget *)GetTopLevelWidget()), *CMap, &ColorCellDef);
#       if DEBUGHTOS_AllocColor
        fprintf( stderr,  "HTOS_AllocColor():RGB Value red=%08x green=%08x blue=%08x\n",
                        ColorCellDef.red, ColorCellDef.green, ColorCellDef.blue);
#       endif
        }
        else {
#       if DEBUGHTOS_AllocColor
        fprintf( stderr,  "HTOS_AllocNamedColor()\n");
#       endif
                Length = (CARD16 *)ReadData( Port, sizeof(CARD16), BLOCK );
                *Length = SwapInt( *Length, WhichTool->ConnectPrefix.byteOrder );
                Pad = (CARD16 *)ReadData( Port, sizeof(CARD16), BLOCK );
                ColorName =(BYTE *)ReadData( Port, *Length, BLOCK );
#       if DEBUGHTOS_AllocColor
        fprintf( stderr,  "HTOS_AllocNamedColor():ColorName == %s\n",(char *)ColorName);
#       endif
                XAllocNamedColor(XtDisplay((Widget *)GetTopLevelWidget()), *CMap, (char *)ColorName,
                                &ColorCellDef, &RgbDbDef);
        }
        PixelValue = (CARD32 *)ReadData( Port, sizeof(CARD32), BLOCK );

        LocalPixelValue = SwapLong( ColorCellDef.pixel, FROMLOCAL(WhichTool->ConnectPrefix.byteOrder));

        WhichTool->ColorCellList = (CARD32 *)realloc( WhichTool->ColorCellList,
                (++(WhichTool->NumColorCells) * sizeof(CARD32) ) );
        WhichTool->LocalColorCellList = (CARD32 *)realloc(WhichTool->LocalColorCellList,
                (++(WhichTool->LocalNumColorCells) * sizeof(CARD32) ) );

        WhichTool->LocalColorCellList[(WhichTool->LocalNumColorCells)-1] = (CARD32)LocalPixelValue;
        WhichTool->ColorCellList[(WhichTool->NumColorCells)-1] = *((CARD32 *)PixelValue);

#       if DEBUGHTOS_AllocColor
        fprintf( stderr,  "HTOS_AllocColor(): Host's Pixel %08x --> %08x\n", WhichTool->ColorCellList[(WhichTool
->NumColorCells)-1],
                WhichTool->LocalColorCellList[(WhichTool->LocalNumColorCells)-1]);
#       endif

        /* Clean up and go home */

	if (*NameTag == 0)
		FreeMem( PrimaryColors);
	else {
		FreeMem( Length );
		FreeMem( Pad );
		FreeMem( ColorName );
	}
        FreeMem( ToolID );
        FreeMem( PixelValue );
        FreeMem( CMap );
        FreeMem( NameTag );
	if ( WhichTool->CatchUpMode == 1 ) if (SendByte(GetHostConnection(),SH_CatchUpAck)==ERROR) EndSession();
	
}


/*
 *	HTOS_YouAre()
 *
 *	Cook a Tool.
 *
 *      Protocol:       HS_YouAre | SubscriberID | HostDisplayNumber
 *
 *              HS_YouAre               Protocol ID Tag
 *              SubscriberID            Local subscriberID
 *              HostDisplayNumber       To start tool remotely
 *
 *	RETURN VALUE:	void
 */
void HTOS_YouAre( Port, Who )
Socket Port;
Subscriber *Who;
{
	BYTE		WhoAmI;

	WhoAmI = GetByte( Port );

	SetLocalSubID( WhoAmI );
        HostDisplayNumber = GetByte(Port);
#       if DEBUGHTOS_YouAre
        fprintf( stderr, "HTOS_YouAre(): WhoAmI: %d , HostDisplayNumber %d \n", 
		WhoAmI, HostDisplayNumber);
#       endif


}


/*
 *	HTOS_TakeToken()
 *
 *	Cook a Tool.
 *
 *	Protocol:	HS_TakeToken | ToolID | SubscriberID
 *
 *		HS_TakeToken		Protocol ID Tag
 *		ToolID			Tool ID of tool in question
 *		SubscriberID		Local subscriberID
 *
 *	RETURN VALUE:	void
 */
void HTOS_TakeToken( Port, Who )
Socket Port;
Subscriber *Who;
{
	BYTE		WhoHasIt;
	Subscriber	*WhoIsIt;
	BYTE		ToolID;
	Tool		*WhatTool;

	ToolID = GetByte( Port );
	WhoHasIt = GetByte( Port );

	if ( (WhatTool=FindToolByID(ToolID)) == NULL ) {
		/* xyzzy - for now trash this packet, it should't have
		arrived to us in the first place, the efficient fix
		is to let the host rcognize this and do't send it to us */
#       if DEBUGHTOS_TakeToken
		fprintf(stderr,  "HTOS_TakeToken(): Tool ID %d unknown.", ToolID );
#	endif
	return;
	}


	WhatTool->TokenHolder = WhoHasIt;

	WhoIsIt = FindSubscriberByID( WhoHasIt );
	if (GetLocalSubID() == WhoHasIt) {
                        WhatTool->AskedToGetToken = FALSE;
                	UpdateMessage( "Now you have the token for %s\n", WhatTool->ToolCommand);
	} else {

#	if DEBUGHTOS_TakeToken
	fprintf( stderr, "HTOS_TakeToken(): %s now has the token for %s\n",
	    (WhoHasIt == HOST_ID ? "THE HOST" : WhoIsIt->LoginName),
	    WhatTool->ToolCommand );
#	endif

	UpdateMessage( "%s has the token for %s\n", ( WhoHasIt == HOST_ID ? "The Chairman" : WhoIsIt->LoginName ), WhatTool->ToolCommand );
	}
}


/*
 *	HTOS_ForceTokenRelease()
 *
 *	Cook a Tool.
 *
 *	Protocol:	HS_ForceTokenRelease | ToolID
 *
 *		HS_ForceTokenRelease	Protocol ID Tag
 *		ToolID			Tool ID of tool in question
 *
 *
 *	RETURN VALUE:	void
 */
void HTOS_ForceTokenRelease( Port, Who )
Socket Port;
Subscriber *Who;
{
	BYTE		WhoHasIt;
	Subscriber	*WhoIsIt;
	BYTE		ToolID;
	Tool		*WhatTool;

	ToolID = GetByte( Port );

        if ( (WhatTool=FindToolByID(ToolID)) == NULL ) {
#       if DEBUGHTOS_ForceTokenRelease
                fprintf(stderr, "HTOS_ForceTokenRelease:  Tool ID %d unknown.", ToolID );
#       endif
                return;
        }


#	if DEBUGHTOS_ForceTokenRelease
	fprintf( stderr, "HTOS_ForceTokenRelease(): Host snatched %s token.\n",
	    WhatTool->ToolCommand );
#	endif

	UpdateMessage( "The chairman has snatched\n the token for tool %s", WhatTool->ToolCommand );

	WhatTool->TokenHolder = HOST_ID;
}

/*
 *	HTOS_AskTokenRelease()
 *
 *
 *	Protocol:	HS_AskTokenRelease | SubscriberID | ToolID
 *
 *		HS_AskTokenRelease	Protocol ID Tag
 *		SubscriberID		Subscriber ID that needs token
 *		ToolID			Tool ID of tool in question
 *
 *
 *	RETURN VALUE:	void
 */
void HTOS_AskTokenRelease( Port, Who )
Socket Port;
Subscriber *Who;
{
	Subscriber	*WhoIsIt;
	BYTE		ToolID;
	BYTE		SubscriberID;
	Tool		*WhatTool;

	SubscriberID = GetByte( Port );
	ToolID = GetByte( Port );

        if ( (WhoIsIt = FindSubscriberByID( SubscriberID )) == NULL){
#       if DEBUGHTOS_AskTokenRelease
               printf(stderr, "HTOS_AskTokenRelease(): Subscriber ID %d unknown.", SubscriberID );
#       endif
                return;
        }


        if ( (WhatTool=FindToolByID(ToolID)) == NULL ) {
#       if DEBUGHTOS_DeleteTool
                fprintf(stderr, "HTOS_AskTokenRelease:  Tool ID %d unknown.", ToolID );
#       endif
                return;
        }


#	if DEBUGHTOS_AskTokenRelease
	fprintf( stderr, "HTOS_AskTokenRelease(): Participant %s needs %s token.\n",
	    WhoIsIt->LoginName, WhatTool->ToolCommand );
#	endif

        if ( WhatTool->TokenHolder == GetLocalSubID() ) {
UpdateMessage( "%s wants the token\n for tool %s.\n Select %s tool and press [Drop Token]", WhoIsIt->LoginName, WhatTool->ToolCommand, WhatTool->ToolCommand);
}

}
TrashIt(Port)
Socket Port;
{
	CARD32		*Length;
	BYTE		*Packet;

	Length = (CARD32 *)ReadData( Port, sizeof(CARD32), BLOCK );
	Packet = (BYTE *)ReadData( Port, *Length, BLOCK );
        FreeMem( Length );
        FreeMem( Packet );
}
/*
 *      HTOS_CreatorTakeToken()
 *
 *      Cook a Tool.
 *
 *      Protocol:       HS_CreatorTakeToken | ToolID | SubscriberID
 *
 *              HS_CreatorTakeToken            Protocol ID Tag
 *              ToolID                  Tool ID of tool in question
 *              SubscriberID            Local subscriberID
 *
 *      RETURN VALUE:   void
 */
void HTOS_CreatorTakeToken( Port, Who )
Socket Port;
Subscriber *Who;
{
        BYTE            WhoHasIt;
        Subscriber      *WhoIsIt;
        BYTE            ToolID;
        Tool            *WhatTool;

        ToolID = GetByte( Port );
        WhoHasIt = GetByte( Port );

        if ( (WhatTool=FindToolByID(ToolID)) == NULL ) {
                /* xyzzy - for now trash this packet, it should't have
                arrived to us in the first place, the efficient fix
                is to let the host rcognize this and do't send it to us */
#       if DEBUGHTOS_CreatorTakeToken
                fprintf(stderr,  "HTOS_CreatorTakeToken(): Tool ID %d unknown.", ToolID );
#       endif
        return;
        }

/* if I am the owner and  did't ask for it, it means no one needs it and  I 
should keep it */

	UpdateMessage( "No one wants %s token \nYou have it.\nsince you are the creator of this tool" , WhatTool->ToolCommand);
	WhatTool->TokenHolder = WhoHasIt;
}



/*
 *      HTOS_CreatorTakeToken()
 *
 *      Cook a Tool.
 *
 *      Protocol:       HS_CreatorTakeToken | ToolID | SubscriberID
 *
 *              HS_CreatorTakeToken            Protocol ID Tag
 *              ToolID                  Tool ID of tool in question
 *              SubscriberID            Local subscriberID
 *
 *      RETURN VALUE:   void
 */
void HTOS_ContinueTakeToken( Port, Who )
Socket Port;
Subscriber *Who;
{
        BYTE            WhoHasIt;
        Subscriber      *WhoIsIt;
        BYTE            ToolID;
        Tool            *WhatTool;

        ToolID = GetByte( Port );
        WhoHasIt = GetByte( Port );

        if ( (WhatTool=FindToolByID(ToolID)) == NULL ) {
                /* xyzzy - for now trash this packet, it should't have
                arrived to us in the first place, the efficient fix
                is to let the host rcognize this and do't send it to us */
#       if DEBUGHTOS_ContinueTakeToken
                fprintf(stderr,  "HTOS_ContinueTakeToken(): Tool ID %d unknown.", ToolID );
#       endif
        return;
        }

/* if I am the token holder and  no one needs it and  I should keep it */

        UpdateMessage("No one waiting for [%s] token.\nYou may keep it", WhatTool->ToolCommand);

	WhatTool->TokenHolder = WhoHasIt;
}



/*
 *	EndSession()
 *
 *	The chairman has terminated unexpectedly. Clean up NEATLY and
 *	exit.
 */

void EndSession()
{
	extern Widget	WSubscriber;
	extern Widget	WTool;
	Arg		args[5];
	int		n=0;

	XtSetArg( args[n],      XtNsensitive,           FALSE );
	n++;
	XtSetValues( WSubscriber, args, n );
	XtSetValues( WTool, args, n );

	CloseAllTools();

	UpdateMessage( "The Host has unexpectedly terminated.\n Press [Exit XTV] to exit." );
}


/*
 * Begin Murray Addition
 */

/*
 * HTOS_QueuedList()
 *
 * Get the list of queued ids from the socket
 *
 * 2-VII-91 - Changed to now accept a list of tool holders at the same time.
 *
 * Protocol:
 *     HS_QueuedList - Protocol ID Tag
 *     destination   - Specifies the routine to call with the token information.
 *     toolID        - Number of the tool that this is the info for. For checking consistency.
 *     numQueued     - Number of queued entries in the list
 *     <queuedEntries> - Each entry in the list of people waiting for the token.
 *
 * Local vars:
 *     numQueued     - (BYTE) holds the number of queued participants. Since SubscriberIDs are
 *                     are this size. There can't be anymore of these than numQueued can represent.
 *     queuedEntries - (BYTE[512]) holds the actual queued entries as they are received.
 *     queueIndex    - (short) used to index into the queuedEntry array.
 *     tIToolID      - (BYTE) holds the id of the tool. This is passed to tokenInfoSubscr() as
 *                     part of a consistency check.
 *
 * Return value: void
 */

XtCallbackProc target[] = {(XtCallbackProc) tokenInfoBuild, (XtCallbackProc) particInfoBuild};

void HTOS_QueuedList(Port, Who)
Socket Port;
Subscriber *Who;
{
  BYTE    destination;
  BYTE    numQueued;
  BYTE    queuedEntries[512];
  short   queueIndex;
  BYTE    tIToolID;

  /*
   * Pull off the fixed part of the return message and set the appropriate local vars.
   */
  destination = GetByte(Port);
  tIToolID = GetByte(Port);
  numQueued = GetByte(Port);

  /*
   * Build the actual queuedEntries array by pulling ids off of the port given to us
   * as a parameter.
   */
  for (queueIndex=0; queueIndex < (short) numQueued; queueIndex++)
    queuedEntries[queueIndex] = GetByte(Port);

  (target[(int) destination])(tIToolID, (short) numQueued, queuedEntries);

  /*
   * tokenInfoBuild(tIToolID, (short) numQueued, queuedEntries);
   */
}

void HTOS_IncomingTools(Port, Who)
Socket Port;
Subscriber *Who;
{
  BYTE expectedNum, expectedLen;
  int  numTools, currentLen;
  char toolMsgBuffer[2048];
  char tmpName[128];

  expectedNum = GetByte(Port);
  if (!expectedNum)
    {
      (void) strcpy(toolMsgBuffer, "-- Expect zero tools from the host. --\n\r");
      UpdateMessage(toolMsgBuffer);
    }
  else
    {
      (void) strcpy(toolMsgBuffer, "Expect the following tools from the host:\n\r");
      for (numTools = 0; numTools < (int) expectedNum; numTools++)
	{
	  (void) strcat(toolMsgBuffer, "  ");
	  expectedLen = GetByte(Port);
	  for (currentLen = 0; currentLen < (int) expectedLen; currentLen++)
	    tmpName[currentLen] = (char) GetByte(Port);
	  tmpName[currentLen] = '\0';
	  (void) strcat(toolMsgBuffer, tmpName);
	  (void) strcat(toolMsgBuffer, "\n\r");
	}
      /*
       * NOTE: unlockMessages is sent when the subscriber confirms/(denies) the
       * subscription to the last tool.
       *
       * 10-VII-91
       */
      UpdateMessage(toolMsgBuffer);
      lockMessages();
    }
}

void HTOS_EndIncoming(Port, Who)
Socket Port;
Subscriber *Who;
{
  /*
   * Now that we are at the end of the tools, we
   * want the message box to stay with new messages.
   */

  unlockMessages();
}

/*
 * End Murray Addition
 */

void HTOS_ToolsToJoin(Port, Who)
Socket Port;
Subscriber *Who;
{
	HandleToolsToJoinList();
}

void HTOS_CatchUpStart(Port, Who)
Socket Port;
Subscriber *Who;
{

        CatchUp = 1;
}

void HTOS_CatchUpEnd(Port, Who)
Socket Port;
Subscriber *Who;
{
        BYTE *id;
        Tool *tool;

        id = (BYTE *)ReadData( Port, sizeof(BYTE), BLOCK );
        if( (tool = FindToolByID(*id)) != NULL )
        	tool->CatchUpMode = 0;
}
