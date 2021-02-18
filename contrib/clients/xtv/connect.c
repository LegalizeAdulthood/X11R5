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
  connect.c
  Routines to process a connection from a client to the server

*/



#define  DEBUGBitCount				01&DEBUGON
#define  DEBUGRootSize				01&DEBUGON
#define  DEBUGProcessHostClientConnection	01&DEBUGON
#define  DEBUGProcessClientConnection		01&DEBUGON
#define  DEBUGConnectToDisplay			01&DEBUGON

#define  USE_DISPLAY_1				0	/* For xscope-ing things at the viewer's end */

#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xproto.h>

#include "memory.h"
#include "network.h"
#include "packet.h"
#include "tool.h"
#include "connect.h"
#include "misc.h"

extern int VERBOSE;
extern int XCOLOR;
char HostName[HOST_NAME_SIZE];


/*
 *  BitCount()
 *
 *  Count the number of 1s in an XID.  Used for
 *  calculating how many sig figs we're going to 
 *  need in translating XIDs.
 *
 *  RETURN VALUE:	Number of ones.
 */
BYTE BitCount( Value )
XID Value;
{
	int	WhatBit;
	BYTE	TotalOnes = 0;

#	if DEBUGBitCount
	fprintf( stderr, "BitCount( %08x )\n", Value );
#	endif

	for ( WhatBit=0; WhatBit < 32; WhatBit++ ) {
		if ( (Value & ( 1 << WhatBit)) ) {
			TotalOnes++;
		}
	}

#	if DEBUGBitCount
	fprintf( stderr, "BitCount(): Returning %d\n", TotalOnes );
#	endif

	return TotalOnes;
}


/*
 *  RootSize()
 *
 *  Calculate how many bytes a root screen entry takes up
 *
 *  RETURN VALUE:	Byte count
 */
CARD16 RootSize( RootStart, ByteOrder )
BYTE *RootStart;
BYTE ByteOrder;
{
	BYTE		*WhereAt;
	xWindowRoot	*RootInfo;
	CARD16		Depths;
	CARD16		Visuals	= 0;

	Depths = 0;
	RootInfo = (xWindowRoot *)RootStart;
	WhereAt  = RootStart + sizeof(xWindowRoot);

#	if DEBUGRootSize
	fprintf( stderr, "RootSize(): Examining Root Window %08x\n", RootInfo->windowId );
	fprintf( stderr, "RootSize(): %d Depths\n", RootInfo->nDepths );
#	endif
	for ( Depths=0; Depths < RootInfo->nDepths; Depths++ ) {

		int WhichVT;

		Visuals += (WhichVT=SwapInt( ((xDepth *)WhereAt)->nVisuals, ByteOrder ));
#		if DEBUGRootSize
		fprintf( stderr, "RootSize(): Depth %d has %d visuals\n", Depths, WhichVT );
#		endif
		WhereAt += sizeof(xDepth);
		WhereAt += (WhichVT * sizeof(xVisualType));
	}

#	if DEBUGRootSize
	fprintf( stderr, "RootSize(): Returning %d\n",
	    ( sizeof(xWindowRoot) + (Depths*sizeof(xDepth)) + (Visuals*sizeof(xVisualType)) ) );
#	endif
	return ( sizeof(xWindowRoot) + (Depths*sizeof(xDepth)) + (Visuals*sizeof(xVisualType)) );
}

/*
 *  ProcessHostClientConnection();
 *
 *  Process a connection from a client waiting at the socket
 *  described in NewTool->InConnection, establish a connection
 *  to the X server, and attempt to hook the client up.
 *
 *  RETURN VALUE:	0 if everything went okay
 *  		-1 if the server refused the connection.
 */

static  BYTE			ByteOrder;
static  xConnClientPrefix	*WorkingPrefix;
static  xConnSetupPrefix	*ReplyFixed;
static  xConnSetup		*ReplySuccess;
static  char			*Vendor;
static  xPixmapFormat		*PixmapFormats;
static  CARD16			RootAreaSize;
static  BYTE			*RootScreens;
static  BYTE			*LocalRoot;
static  CARD16			LocalRootSize;
static  XID			*VisualIDList;
static  char			*WhyFailed;
static  int                     DisplayScreen;


int ProcessHostClientConnection( NewTool )
Tool *NewTool;
{
#	if DEBUGProcessHostClientConnection
	fprintf( stderr, "ProcessHostClientConnection()\n" );
#	endif

	if (VERBOSE) {
		fprintf( stderr, "Connecting to Tool: %s\n",
		    NewTool->ToolCommand);
	}
	/* Start by reading the connection request from the client */

	WorkingPrefix = (xConnClientPrefix *)ReadData( NewTool->InConnection,
	    sizeof(xConnClientPrefix), BLOCK );
	memcpy( &(NewTool->ConnectPrefix), WorkingPrefix, sizeof(xConnClientPrefix) );


	ByteOrder = WorkingPrefix->byteOrder;
#	if DEBUGProcessHostClientConnection
	fprintf( stderr, "ProcessHostClientConnection(): Byte order is [%02x] %cSB first\n",
	    *((BYTE *)(WorkingPrefix)),
	    ( ByteOrder == MSB_First ? 'M' : 'L' ) );
#	endif

	/* Do some translations on the working copy */

	WorkingPrefix->majorVersion	= SwapInt( WorkingPrefix->majorVersion,		ByteOrder );
	WorkingPrefix->minorVersion	= SwapInt( WorkingPrefix->minorVersion,		ByteOrder );
	WorkingPrefix->nbytesAuthProto	=  pad(SwapInt(WorkingPrefix->nbytesAuthProto,	ByteOrder ));
	WorkingPrefix->nbytesAuthString	=  pad(SwapInt(WorkingPrefix->nbytesAuthString,ByteOrder ));

	/* Grab the remaining section of the connect request */

	NewTool->HowMuchAuthData = WorkingPrefix->nbytesAuthProto + WorkingPrefix->nbytesAuthString;
	if ( NewTool->HowMuchAuthData > 0 ) {
#		if DEBUGProcessHostClientConnection
		fprintf( stderr, "ProcessHostClientConnection(): Receiving %d+%d extra bytes\n",
		    NewTool->HowMuchAuthData );
#		endif
		NewTool->AuthData = (BYTE *)ReadData( NewTool->InConnection, NewTool->HowMuchAuthData,
		    BLOCK );
	} else {
#		if DEBUGProcessHostClientConnection
		fprintf( stderr, "ProcessHostClientConnection(): Receiving NO extra bytes\n" );
#		endif
		NewTool->AuthData = NULL;
	}

#	if DEBUGProcessHostClientConnection
	fprintf( stderr, "ProcessHostClientConnection(): Client expects X%dR%d\n",
	    WorkingPrefix->majorVersion, WorkingPrefix->minorVersion );
	fprintf( stderr, "ProcessHostClientConnection(): Authorization protocol is [%3d]: %s\n",
	    WorkingPrefix->nbytesAuthProto, (char *)NewTool->AuthData );
	fprintf( stderr, "ProcessHostClientConnection(): Authorization string   is [%3d]: %s\n",
	    WorkingPrefix->nbytesAuthString,
	    (char *)(NewTool->AuthData+WorkingPrefix->nbytesAuthProto) );
#	endif
	if (ConnectToDisplay(NewTool,&(NewTool->NumVisualIDs),&(NewTool->VisualIDList)) == -1) return (-1);
	/* Build a full tool structure for the victim(s) */

	memcpy( &(NewTool->HostConnectInfo),	ReplySuccess,	sizeof(xConnSetup) );
	memcpy( &(NewTool->HostRootInfo),	LocalRoot,	sizeof(xWindowRoot) );
        NewTool->HostPixmapFormats =  (xPixmapFormat *)malloc(ReplySuccess->numFormats *
								  sizeof(xPixmapFormat));
	memcpy( NewTool->HostPixmapFormats, PixmapFormats, ReplySuccess->numFormats * sizeof(xPixmapFormat));

	/* Adjust the size of the reply from the server and ship it back to the client */

	{
		CARD16		NewLength;
		CARD16		Offset = 0;
		CARD16		VendorSize;
		CARD16		PixmapFormatSize;
		BYTE		*BigBlock;

		VendorSize		= pad(SwapInt(ReplySuccess->nbytesVendor, ByteOrder));
		PixmapFormatSize	= (ReplySuccess->numFormats * sizeof(xPixmapFormat));

		NewLength = (	sizeof(xConnSetupPrefix) +	/* Connect Prefix */
		sizeof(xConnSetup) +		/* Connect Info */
		VendorSize +
		    PixmapFormatSize +
		    LocalRootSize );

#		if DEBUGProcessHostClientConnection
		fprintf( stderr, "ProcessHostClientConnection(): New Length is %d\n", NewLength);
#		endif

		ReplyFixed->length = SwapInt( ( (NewLength-sizeof(xConnSetupPrefix))/4),
		    FROMLOCAL(ByteOrder) );
		ReplySuccess->numRoots = 1;	/* Always! */

		BigBlock = (BYTE *)GetMem( NewLength );

		memcpy( BigBlock, ReplyFixed,		sizeof(xConnSetupPrefix));
		Offset+=sizeof(xConnSetupPrefix);
		memcpy( BigBlock+Offset, ReplySuccess,	sizeof(xConnSetup)	);
		Offset+=sizeof(xConnSetup);
		memcpy( BigBlock+Offset, Vendor,	VendorSize		);
		Offset+=VendorSize;
		memcpy( BigBlock+Offset, PixmapFormats,	PixmapFormatSize	);
		Offset+=PixmapFormatSize;
		memcpy( BigBlock+Offset, LocalRoot,	LocalRootSize		);

		if (SendData( NewTool->InConnection, BigBlock, NewLength )==ERROR) {
			CleanUpTool(NewTool);
			return (ERROR);
		}
#		if DEBUGProcessHostClientConnection
		fprintf( stderr, "ProcessHostClientConnection(): Full reply sent\n" );
#		endif
		FreeMem( BigBlock );
	}

	NewTool->pending_reply = FALSE;
	NewTool->LastSeqNum = 1;
	/* Install callbacks for the tool's connection */

	NewTool->InID = XtAppAddInput( GetAppCon(), (NewTool->InConnection), XtInputReadMask,
	    HandleIncomingClientPacket, NewTool );
	NewTool->OutID = XtAppAddInput( GetAppCon(), (NewTool->OutConnection), XtInputReadMask,
	    HandleIncomingServerPacket, NewTool );
	NewTool->InExceptID = XtAppAddInput( GetAppCon(), NewTool->InConnection, XtInputExceptMask,
	    HandlePortException, NewTool );
	NewTool->OutExceptID = XtAppAddInput( GetAppCon(), NewTool->OutConnection, XtInputExceptMask,
	    HandlePortException, NewTool );

	/* Clean up */
	/* xyzzy - Make sure we've freed EVERYTHING */
	FreeMem( ReplySuccess );
	FreeMem( Vendor );
	FreeMem( PixmapFormats );
	FreeMem( RootScreens );

#       if DEBUGProcessHostClientConnection
	fprintf( stderr, "ProcessClientConnection(): Tool Connection Complete\n");
#       endif

	if (VERBOSE) {
		fprintf( stderr, "Connected to Tool: %s\n",
		    NewTool->ToolCommand);
	}
	return 0;
}


/*
 *  ProcessClientConnection();
 *
 *  Taking a tool structure received from the host, try and
 *  establish a connection with the local X server.  Also
 *  invoke callbacks for the server and client sockets.
 *
 *  RETURN VALUE:	0 if everything went okay
 *  		-1 if the server refused the connection.
 */
int ProcessClientConnection( NewTool )
Tool *NewTool;
{
#	if DEBUGProcessClientConnection
	fprintf( stderr, "ProcessClientConnection()\n" );
#	endif

	/* Set the Byte Order */
	if (VERBOSE) {
		fprintf( stderr, "Connecting to Tool: %s\n",
		    NewTool->ToolCommand);
	}

	ByteOrder = NewTool->ConnectPrefix.byteOrder;

#       if DEBUGProcessClientConnection 
	fprintf( stderr, "ProcessClientConnection(): Byte order is [%02x] %cSB first\n",
		 ByteOrder, ( ByteOrder == MSB_First ? 'M' : 'L' ) );
#       endif

	if (ConnectToDisplay(NewTool,&(NewTool->LocalNumVisualIDs),&(NewTool->LocalVisualIDList)) == -1) return (-1);

	/* Build a full tool structure for the victim(s) */

	memcpy( &(NewTool->LocalConnectInfo),	ReplySuccess,	sizeof(xConnSetup) );
	memcpy( &(NewTool->LocalRootInfo),	LocalRoot,	sizeof(xWindowRoot) );
        NewTool->LocalPixmapFormats = (xPixmapFormat *)malloc(ReplySuccess->numFormats *
								  sizeof(xPixmapFormat));
        memcpy( NewTool->LocalPixmapFormats, PixmapFormats, ReplySuccess->numFormats * sizeof(xPixmapFormat));
        if (XCOLOR) {
        NewTool->NumColorCells =2;
        NewTool->ColorCellList = (CARD32 *)GetMem( 2 * sizeof(CARD32) );
        NewTool->ColorCellList[0] = NewTool->HostRootInfo.whitePixel;
        NewTool->ColorCellList[1] = NewTool->HostRootInfo.blackPixel;
        NewTool->LocalNumColorCells = 2;
        NewTool->LocalColorCellList = (CARD32 *)GetMem( 2 * sizeof(CARD32) );
        NewTool->LocalColorCellList[0] = NewTool->LocalRootInfo.whitePixel;
        NewTool->LocalColorCellList[1] = NewTool->LocalRootInfo.blackPixel;

        }

	/* Start an event input callback */

	NewTool->OutID = XtAppAddInput( GetAppCon(), NewTool->OutConnection, XtInputReadMask,
	    HandleIncomingViewerPacket, NewTool );

	NewTool->OutExceptID = XtAppAddInput( GetAppCon(), NewTool->OutConnection, XtInputExceptMask,
	    HandlePortException, NewTool );
	/* Clean up */
	/* xyzzy - Free the right stuff here */
	FreeMem( ReplySuccess );
	FreeMem( Vendor );
	FreeMem( PixmapFormats );
	FreeMem( RootScreens );
#       if DEBUGProcessClientConnection
	fprintf( stderr, "ProcessClientConnection(): Tool Connection Complete\n");
#       endif



	if (VERBOSE) {
		fprintf( stderr, "Connected to Tool: %s\n",
		    NewTool->ToolCommand);
	}

	return 0;
}

ConnectToDisplay(NewTool, LNumVisualIDs, LVisualIDList)
Tool *NewTool;
CARD16  *LNumVisualIDs;
XID  **LVisualIDList;
{


static char			*DisplayVar;
static int			DisplayNumber;
char			*TempD;
char			*TempS;
static BYTE FIRST = TRUE;
	/* Dissect the DISPLAY environment variable to see where XTV is running */
if (FIRST) {
	FIRST = FALSE;
	if ( (DisplayVar=getenv("DISPLAY")) == NULL ){
#       if DEBUGConnectToDisplay
                fprintf( stderr, "DISPLAY variable is NULL\n" );
#       endif
                        FatalError("DISPLAY variable is NULL\n" );
                        return(ERROR);
        }

#       if DEBUGConnectToDisplay
	fprintf( stderr, "ConnectToDisplay(): DISPLAY=%s\n", DisplayVar );
#       endif
        if ( (TempD = index(DisplayVar,':')) == NULL){
#       if DEBUGConnectToDisplay
                fprintf( stderr, "ConnectToDisplay(): Can't find ':' in DISPLAY variable %s\n", DisplayVar);
#       endif
                FatalError(" Can't find ':' in DISPLAY variable %s\n", DisplayVar);
                return(ERROR);
	}

	*(TempD++) = '\0';
        if ((TempS = rindex(TempD,'.')) == NULL){
		DisplayNumber = atoi(TempD);
	} else {
                *(TempS++) = '\0';
                DisplayNumber = atoi(TempD);
	}
	if ( strstr(DisplayVar, "unix") ) {
#       if DEBUGConnectToDisplay
		fprintf( stderr, "value of DISPLAY variable is unix\n" );
#       endif
		if (gethostname(HostName, HOST_NAME_SIZE) < 0){
#       if DEBUGConnectToDisplay
                fprintf( stderr, "ConnectToDisplay(): Unable to get host name\n");
#       endif
                        FatalError("Unable to get host name");
                        return(ERROR);
                }
		DisplayVar = HostName;
	}

#	if DEBUGConnectToDisplay
	fprintf( stderr, "ConnectToDisplay(): Connecting to %s:%d\n",
	    DisplayVar, DisplayNumber);
#	endif
}
	if ((NewTool->OutConnection = ConnectTo( DisplayVar, ntohs(X_TCP_PORT+DisplayNumber))) < 0) {
#  if DEBUGConnectToDisplay
		fprintf( stderr, "ConnectToDisplay(): Fail to Connect to %s:%d\n",  DisplayVar,
		    ntohs(X_TCP_PORT+DisplayNumber));
#  endif
		return -1;

	}

	/* Send the connect request */

#       if DEBUGConnectToDisplay
	fprintf( stderr, "ConnectToDisplay(): Sending ConnectPrefix request to server [%d] bytes\n",
	    sizeof(xConnClientPrefix) );
#       endif

	if (SendData( NewTool->OutConnection, &(NewTool->ConnectPrefix), sizeof(xConnClientPrefix) )==ERROR) {
			CleanUpTool(NewTool);
			return (ERROR);
		}

#       if DEBUGConnectToDisplay
	fprintf( stderr, "ConnectToDisplay(): Sent ConnectPrefix request to server [%d] bytes\n",
	    sizeof(xConnClientPrefix) );
#       endif

	if ( NewTool->AuthData != NULL ) {
/* chungg_bugfix
		if (SendData( NewTool->OutConnection, &(NewTool->AuthData), NewTool->HowMuchAuthData )==ERROR) {
			CleanUpTool(NewTool);
			return (ERROR);
		}
*/
		if (SendData( NewTool->OutConnection, NewTool->AuthData, NewTool->HowMuchAuthData )==ERROR) {
			CleanUpTool(NewTool);
			return (ERROR);
		}
	}
#	if DEBUGConnectToDisplay
	fprintf( stderr, "ConnectToDisplay(): Sent connection request to server\n" );
#	endif

	/* Now wait for the server to reply */

	ReplyFixed = (xConnSetupPrefix *)ReadData( NewTool->OutConnection,
	    sizeof(xConnSetupPrefix), BLOCK );

	if ( !ReplyFixed->success ) {

		WhyFailed = (char *)ReadData( NewTool->OutConnection, pad(ReplyFixed->lengthReason), BLOCK );
#		if DEBUGConnectToDisplay
		fprintf( stderr, "ConnectToDisplay(): Connection Refused [%d]: %s\n",
		    ReplyFixed->lengthReason, WhyFailed );
#		endif
		UpdateMessage( "Connection failed for tool %s:\n%s", NewTool->ToolCommand, WhyFailed );
		/* xyzzy - Why doesn't this get sent right???? */
		if (SendData( NewTool->InConnection, ReplyFixed, sizeof(xConnSetupPrefix) )==ERROR) {
			CleanUpTool(NewTool);
			return (ERROR);
		}
		if (SendData( NewTool->InConnection, WhyFailed,  ReplyFixed->lengthReason )==ERROR) {
			CleanUpTool(NewTool);
			return (ERROR);
		}

		FreeMem( NewTool->AuthData );
		FreeMem( ReplyFixed );
		FreeMem( WhyFailed );
		close( NewTool->InConnection );
		close( NewTool->OutConnection );
		return -1;
	}

	/* Read the rest */

#	if DEBUGConnectToDisplay
	fprintf( stderr, "ConnectToDisplay(): Connection accepted.  %d bytes follow\n",
	    SwapInt(ReplyFixed->length,ByteOrder) * 4 );
#	endif

	ReplySuccess	= (xConnSetup *)ReadData( NewTool->OutConnection, sizeof(xConnSetup), BLOCK );
	NewTool->BitsInHostMask  = BitCount( ReplySuccess->ridMask );
#	if DEBUGConnectToDisplay
	fprintf( stderr, "ConnectToDisplay(): Server is R%d\n",
	    SwapLong(ReplySuccess->release,ByteOrder) );
	fprintf( stderr, "ConnectToDisplay(): Base=%08x  Mask=%08x\n",
	    ReplySuccess->ridBase, ReplySuccess->ridMask );
	fprintf( stderr, "ConnectToDisplay(): %d Roots\n", ReplySuccess->numRoots );
#	endif

	Vendor 		= (char *)ReadData( NewTool->OutConnection,
	    pad(SwapInt(ReplySuccess->nbytesVendor,ByteOrder)), BLOCK );
#	if DEBUGConnectToDisplay
	fprintf( stderr, "ConnectToDisplay(): Vendor=\"%s\" [%d bytes]\n",
	    Vendor, pad(SwapInt(ReplySuccess->nbytesVendor,ByteOrder)) );
#	endif

	PixmapFormats	= (xPixmapFormat *)ReadData( NewTool->OutConnection,
	    ReplySuccess->numFormats * sizeof(xPixmapFormat), BLOCK );
#	if DEBUGConnectToDisplay
	fprintf( stderr, "ConnectToDisplay(): %d Formats [%d bytes each]\n",
	    ReplySuccess->numFormats, sizeof(xPixmapFormat) );
#	endif


	RootAreaSize	= 	(SwapInt(ReplyFixed->length,ByteOrder) * 4) -		/* Whole Mess */
	sizeof(xConnSetup) -					/* ConnSetup */
	pad(SwapInt(ReplySuccess->nbytesVendor,ByteOrder)) -		/* Vendor len. */
	(ReplySuccess->numFormats * sizeof(xPixmapFormat));	/* Pixmap fmts. */
#	if DEBUGConnectToDisplay
	fprintf( stderr, "ConnectToDisplay(): Root screens take up %d bytes\n",
	    RootAreaSize );
#	endif
	RootScreens	= ReadData( NewTool->OutConnection, RootAreaSize, BLOCK );


	/* Hunt down the root screen entry for the screen to which XTV is connected */

	LocalRoot = RootScreens;
	{
		int	n;

		for ( n=DisplayScreen ; n > 1; n-- ){
			printf("DisplayScreen %d\n", DisplayScreen);
			printf("n %d\n", n);
			LocalRoot += RootSize( LocalRoot, ByteOrder );
		}
	}
	LocalRootSize = RootSize( LocalRoot, ByteOrder );

	/* Build the Visual ID list from the root window */

	{
		BYTE		*WhereAt;
		xWindowRoot	*RootInfo;
		CARD16		Depths;
		CARD16		Visuals	= 0;
		int		WhatVisual;
		int		TopDepths = 25;		/* Set to a good number of depths */

		*LNumVisualIDs = 0;
		*LVisualIDList = (XID *)GetMem( TopDepths * sizeof(XID) );

		Depths = 0;
		RootInfo = (xWindowRoot *)LocalRoot;
		WhereAt  = LocalRoot + sizeof(xWindowRoot);

#		if DEBUGConnectToDisplay
		fprintf( stderr, "ConnectToDisplay(): Building VID list for Root Window %08x\n",
		    RootInfo->windowId );
		fprintf( stderr, "ConnectToDisplay(): %d Depths\n", RootInfo->nDepths );
#		endif

		for ( Depths=0; Depths < RootInfo->nDepths; Depths++ ) {

			int WhichVT;

			Visuals += (WhichVT=SwapInt( ((xDepth *)WhereAt)->nVisuals, ByteOrder ));
#			if DEBUGConnectToDisplay
			fprintf( stderr, "ConnectToDisplay(): %d visuals in Depth %d\n",
			    WhichVT, Depths );
#			endif
			WhereAt += sizeof(xDepth);
			for ( WhatVisual=0; WhatVisual < WhichVT; WhatVisual++ ) {
#				if DEBUGConnectToDisplay
				fprintf( stderr, "ConnectToDisplay(): VisualID %08x\n",
				    ((xVisualType *)WhereAt)->visualID );
#				endif
				if ( *LNumVisualIDs == TopDepths ) {
					TopDepths += 10;
					*LVisualIDList = (XID *)realloc( *LVisualIDList,
					    TopDepths * sizeof(XID) );
				}
				(*LVisualIDList)[(*LNumVisualIDs)++] =
				    ((xVisualType *)WhereAt)->visualID;
				WhereAt += sizeof(xVisualType);
			}
		}
#	if DEBUGConnectToDisplay
		fprintf( stderr, "ConnectToDisplay(): %d VisualIDs\n", *LNumVisualIDs );
#	endif

	}

}
