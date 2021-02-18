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
  packet.c
  Packet handlers
  
  */


#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#define  NEED_REPLIES      /* Tell the header to include the replies */
#define    NEED_EVENTS
#include <X11/Xproto.h>
#include <X11/Intrinsic.h>

#include "memory.h"
#include "network.h"
#include "tool.h"
#include "subscriber.h"
#include "connect.h"
#include "protocol.h"
#include "misc.h"
#include "packet.h"
/*
 * Begin Murray Addition
 */
#include "subscriber2.h"
#include "tool2.h"
/*
 * End Murray Addition
 */

#define   DEBUGHandleIncomingClientPacket   0&DEBUGON
#define   DEBUGHandleClientPopUpRequestPacket 0&DEBUGON
#define   DEBUGHandleIncomingAtom      0&DEBUGON
#define	  DEBUGHandleAllocColorPacket	0&DEBUGON
#define   DEBUGHandleIncomingServerPacket   0&DEBUGON
#define   DEBUGHandlePortException   0&DEBUGON
#define   DEBUGHandleIncomingViewerPacket   0&DEBUGON
#define   DEBUGGetReply			   0&DEBUGON
#define   DEBUGSendReply		   0&DEBUGON
#define   DEBUGReadError                  0&DEBUGON

extern int has_reply[];
extern VERBOSE;
extern JOIN;
extern POPOFF;
extern XCOLOR;

#ifndef WINDOW
#define WINDOW Window
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
 *   HandleIncomingClientPacket
 *
 *   Handle a packet inbound from the client process.  Send its
 *   contents (raw) to the X server and distribute it to whoever
 *   happens to be watching.
 *
 *   xyzzy - Need to see who's looking at the tool.  For now we
 *   xyzzy - just ship it wholesale to everybody.
 *
 *   RETURN VALUE:   void
 */
XtInputCallbackProc HandleIncomingClientPacket( WhatTool, Port, InputID )
     Tool *WhatTool;
     Socket *Port;
     XtInputId *InputID;
{
  xReq      Header;
  BYTE      RequestCode;
  BYTE      *Stuff;
  CARD32      RemainingBytes;
  CARD32      RBytes;
  CARD32      AtomValue;
  CARD32      BitMask;
  Subscriber   *Who;
  XWindowAttributes  Win_Attr;
  int        BitNumber;
  int        WhereAt = 0;
  int             abs_x,abs_y;
  Window          dummywin;
  PopUpInfo       PopUpXY;
  PopUpInfo       *PopUpReply;
  Window          root;
  Window          parent;
  Window          children;
  unsigned        int     nchildren;
  XColor          ColorCellDef;
  XColor          RgbDbDef;
  CARD32          PixelValue;
  CARD32          NameTag;
  CARD32		CMap;
  char            *ColorName;
  CARD16          NameLength;
  CARD16          Pad;
  CARD16          NamePacketLength;
  char *ArchPacketP;
  CARD16   HowMany;
  CARD16   HowManyS;
  BYTE	*Stuff2;
  
  
  /* Reset the toolkit before leaving */
  XtRemoveInput( *InputID );
  WhatTool->InID=XtAppAddInput( GetAppCon(), *Port, XtInputReadMask, HandleIncomingClientPacket, WhatTool );
  
  /* Check for a bogus request */
  if ( (RequestCode=CheckByte( *Port )) == NothingAvail) {
    return;
  }
  if ( VERBOSE ){
    if ( (++(WhatTool->InCount) % 20) == 0) printf("%s --> %d\n", WhatTool->ToolCommand, WhatTool->InCount);
  }
  
  
#   if DEBUGHandleIncomingClientPacket
  fprintf( stderr, "HandleIncomingClientPacket(): ----------- INCOMING PACKET [%d]\n",
	  RequestCode );
#   endif
  
  Header.reqType = RequestCode;
  WhatTool->pending_reply = has_reply[RequestCode];
  WhatTool->server_pending_reply = has_reply[RequestCode];
  
  /* Read the remainder of the request header and plug it into the header */
  
  if ((Stuff=ReadData( *Port, sizeof(xReq)-1, BLOCK ))==NULL) {
    ReadError( WhatTool);
    return (NULL);
  }
  
  memcpy( ((BYTE *)(&Header))+1, Stuff, sizeof(xReq)-1 );
  FreeMem( Stuff );
  
  RemainingBytes = (SwapInt( Header.length, WhatTool->ConnectPrefix.byteOrder )-1) * 4;
  
  if (RemainingBytes) {
    if ((Stuff = ReadData( *Port, RemainingBytes, BLOCK ))==NULL){
      ReadError( WhatTool);
      return (NULL);
    }
  }
  
#   if DEBUGHandleIncomingClientPacket
  fprintf( stderr, "HandleIncomingClientPacket(): Got Request [%d] Header + %d bytes, pending_reply [%d]\n",
	  Header.reqType, RemainingBytes,  has_reply[RequestCode] );
#   endif
  
  /* Check for a PopUpPacket and set the tag appropriately */
  PopUpPacket = FALSE;
  if ((!POPOFF)&&(Header.reqType == 1)) {
    BitMask = SwapLong(*((CARD32 *)(Stuff+6*4)),WhatTool->ConnectPrefix.byteOrder);
    if (BitMask & (1 << 9)) {
      for ( BitNumber=0; BitNumber < 9; BitNumber++ )
	if ( BitMask & (1 << BitNumber) )
	  WhereAt++;
#ifdef SUN
      if (*((BOOL *)(Stuff+7*4+WhereAt*4+3)) == TRUE)
#endif
	
	
#ifdef DEC
	if (*((BOOL *)(Stuff+7*4+WhereAt*4)) == TRUE)
#endif
	  
#ifdef IBM
	  if (*((BOOL *)(Stuff+7*4+WhereAt*4+3)) == TRUE)
#endif
	    
	    PopUpPacket = TRUE;
    }
    else {
      if ((WhatTool->FindAbsoluteWindow == FALSE) &&
	  (*((WINDOW *)(Stuff+4)) == (WINDOW)WhatTool->HostRootInfo.windowId)) {
	WhatTool->FindAbsoluteWindow = TRUE;
	WhatTool->AbsoluteWindow = *((WINDOW *)Stuff);
#   if DEBUGHandleClientPopUpRequestPacket
        fprintf( stderr,"AbsoluteWindow for the tool is FOUND@@@@@@@@@@@@%%%%%%%%\n");
        fprintf( stderr,"AbsoluteWindowId===%d Its Parent WIndowID===%d\n",*((WINDOW *)Stuff),*((WINDOW *)(Stuff+4)));
#   endif
      }
    }
  }
  if ((!POPOFF)&&(PopUpPacket == TRUE)&&(WhatTool->TokenHolder != HOST_ID)) {
    
#   if DEBUGHandleClientPopUpRequestPacket
    fprintf( stderr,"PopUpPacket == TRUE and the HOST do not has the token for WhatTool\n");
#   endif
    
    PopUpXY.opCode = 120;
    PopUpXY.AbsoluteWindow = WhatTool->AbsoluteWindow;
    PopUpXY.PopUpX = *((INT16 *)(Stuff+8));
    PopUpXY.PopUpY = *((INT16 *)(Stuff+10));
    
    Who = FindSubscriberByID(WhatTool->TokenHolder);
    /* Send the PopUpPacket to the subscriber who has the token for WhatTool*/
    
    if (SendByte( Who->Connection, HS_X11Packet )==ERROR) {
      CleanUpSubscriber(Who);
      return;
    }
    if (SendByte( Who->Connection, WhatTool->ToolID )==ERROR) {
      CleanUpSubscriber(Who);
      return;
    }
    
    RBytes = SwapLong(sizeof(PopUpInfo),
		      FROMLOCAL(WhatTool->ConnectPrefix.byteOrder) );
    if(SendData( Who->Connection, &RBytes, sizeof(CARD32) )==ERROR) {
#   if DEBUGHandleClientPopUpRequestPacket
      fprintf( stderr,"ERROR in sending PopUpInfo Packet to token holder:LENGTH#######\n");
#   endif
      CleanUpSubscriber(Who);
      return;
    }
    
    if(SendData( Who->Connection, &PopUpXY, sizeof(PopUpInfo) )==ERROR) {
#   if DEBUGHandleClientPopUpRequestPacket
      fprintf( stderr,"ERROR in sending PopUpInfo Packet to token holder:DATA%%%%%%%\n");
#   endif
      CleanUpSubscriber(Who);
      return;
    }
    if ((PopUpReply = (PopUpInfo *)ReadData(Who->Connection,sizeof(PopUpInfo),BLOCK))==NULL){
      ReadError( WhatTool);
      return (NULL);
    }
    
#   if DEBUGHandleClientPopUpRequestPacket
    fprintf( stderr,"Got PopUpReply from token holder\n");
#   endif
    
    
    XQueryTree(XtDisplay((Widget *)GetTopLevelWidget()),SwapLong(WhatTool->AbsoluteWindow,WhatTool->ConnectPrefix.byteOrder),
	       &root, &parent, &children, &nchildren);
    XGetWindowAttributes(XtDisplay((Widget *)GetTopLevelWidget()),SwapLong(WhatTool->AbsoluteWindow,WhatTool->ConnectPrefix.byteOrder),&Win_Attr);
    if (root != parent) {
      XTranslateCoordinates(XtDisplay((Widget *)GetTopLevelWidget()),SwapLong(WhatTool->AbsoluteWindow,WhatTool->ConnectPrefix.byteOrder),
			    SwapLong(WhatTool->HostRootInfo.windowId,WhatTool->ConnectPrefix.byteOrder), Win_Attr.x, Win_Attr.y, &abs_x, &abs_y, &children);
      Win_Attr.x = abs_x;
      Win_Attr.y = abs_y;
    }
#   if DEBUGHandleClientPopUpRequestPacket
    fprintf( stderr,"Absolute Parent WIndow X==%d, Y==%d\n", Win_Attr.x, Win_Attr.y);
#   endif
    *((INT16 *)(Stuff+8)) = SwapInt(Win_Attr.x + SwapInt(PopUpReply->PopUpX,WhatTool->ConnectPrefix.byteOrder),FROMLOCAL(WhatTool->ConnectPrefix.byteOrder));
    *((INT16 *)(Stuff+10)) = SwapInt(Win_Attr.y + SwapInt(PopUpReply->PopUpY,WhatTool->ConnectPrefix.byteOrder),FROMLOCAL(WhatTool->ConnectPrefix.byteOrder));
    
  }
  
  Stuff2 = (BYTE *) GetMem(RemainingBytes);
  memcpy(Stuff2,Stuff,RemainingBytes);
  
  /* Check for a pop-up window request and modify the packet if true      */
  if ((!POPOFF)&&(PopUpPacket == TRUE)) {
#   if DEBUGHandleClientPopUpRequestPacket
    fprintf( stderr, "HandleIncomingClientPacket():POP_UP Request is modified before sent to subscribers\n" );
#   endif
    
    XQueryTree(XtDisplay((Widget *)GetTopLevelWidget()),SwapLong(WhatTool->AbsoluteWindow,WhatTool->ConnectPrefix.byteOrder),
	       &root, &parent, &children, &nchildren);
    XGetWindowAttributes(XtDisplay((Widget *)GetTopLevelWidget()),SwapLong(WhatTool->AbsoluteWindow,WhatTool->ConnectPrefix.byteOrder),&Win_Attr);
    if (root != parent) {
      XTranslateCoordinates(XtDisplay((Widget *)GetTopLevelWidget()),SwapLong(WhatTool->AbsoluteWindow,WhatTool->ConnectPrefix.byteOrder),
			    SwapLong(WhatTool->HostRootInfo.windowId,WhatTool->ConnectPrefix.byteOrder), Win_Attr.x, Win_Attr.y, &abs_x, &abs_y, &children);
      Win_Attr.x = abs_x;
      Win_Attr.y = abs_y;
    }
    
#   if DEBUGHandleClientPopUpRequestPacket
    fprintf( stderr,"HostToolAbsoluteWindowPosition:X==%d,Y==%d\n",Win_Attr.x,Win_Attr.y);
#   endif
    *((INT16 *)(Stuff2+8)) = SwapInt(SwapInt(*((INT16 *)(Stuff+8)),WhatTool->ConnectPrefix.byteOrder) -  Win_Attr.x,FROMLOCAL(WhatTool->ConnectPrefix.byteOrder));
    *((INT16 *)(Stuff2+10)) = SwapInt(SwapInt(*((INT16 *)(Stuff+10)),WhatTool->ConnectPrefix.byteOrder) - Win_Attr.y,FROMLOCAL(WhatTool->ConnectPrefix.byteOrder));
    *((WINDOW *)(Stuff2+4)) = (WINDOW)WhatTool->AbsoluteWindow;
#   if DEBUGHandleClientPopUpRequestPacket
    fprintf( stderr,"HOSTROOTWINDOW hostRootWindow==%d\n",WhatTool->HostRootInfo.windowId);
#   endif
  }
  
  if( JOIN && Header.reqType < 128 ){
    /* Archive the packet */
    ArchPacketP = GetMem(sizeof(xReq)+RemainingBytes);
    memcpy(ArchPacketP,(char *) &Header,sizeof(xReq));
    memcpy(ArchPacketP+sizeof(xReq),Stuff2,RemainingBytes);
    ArchivePacket(WhatTool,ArchPacketP,sizeof(xReq)+RemainingBytes);
    FreeMem(ArchPacketP);
  }
  
  /* Send the whole mess off to the server */
  
  if (SendData( WhatTool->OutConnection, &Header, sizeof(xReq))==ERROR){
    
#   if DEBUGHandleIncomingClientPacket
    fprintf( stderr, "HandleIncomingClientPacket(): ERROR in sending Request sent to server\n" );
#   endif
    CleanUpTool(WhatTool);
    return;
  }
  if (RemainingBytes) {
    if(SendData( WhatTool->OutConnection, Stuff, RemainingBytes )==ERROR){
      
#   if DEBUGHandleIncomingClientPacket
      fprintf( stderr, "HandleIncomingClientPacket(): ERROR in sending Request sent to server\n" );
#   endif
      CleanUpTool(WhatTool);
      return;
    }
  }
#   if DEBUGHandleIncomingClientPacket
  fprintf( stderr, "HandleIncomingClientPacket(): Request sent to server\n" );
#   endif
  /* If we have an InternalAtom request, call the local display server and look it up */
  if ( Header.reqType == X_InternAtom ) {
    
    AtomValue = XInternAtom( XtDisplay((Widget *)GetTopLevelWidget()),
			    (char *)(Stuff+(sizeof(xInternAtomReq)-sizeof(xReq))), TRUE );
    
    AtomValue = SwapLong( AtomValue, FROMLOCAL(WhatTool->ConnectPrefix.byteOrder) );
    
#      if DEBUGHandleIncomingAtom
    fprintf( stderr, "HandleIncomingClientPacket(): New Atom: '%s' = %08x\n", 
	    (char *)(Stuff+(sizeof(xInternAtomReq)-sizeof(xReq))), AtomValue );
#      endif
  }
  /* If we have an AllocColor request, call the local display server and allocate a cell in cmap */
  if ( XCOLOR && ( Header.reqType == X_AllocColor ) &&
      (*((Colormap *)Stuff) == WhatTool->HostRootInfo.defaultColormap)) {
    
    ColorCellDef.red = SwapInt(*((CARD16 *)(Stuff+4)),WhatTool->ConnectPrefix.byteOrder);
    ColorCellDef.green = SwapInt(*((CARD16 *)(Stuff+6)),WhatTool->ConnectPrefix.byteOrder);
    ColorCellDef.blue = SwapInt(*((CARD16 *)(Stuff+8)),WhatTool->ConnectPrefix.byteOrder);
    CMap = SwapLong( *((Colormap *)Stuff), WhatTool->ConnectPrefix.byteOrder);
    XAllocColor( XtDisplay((Widget *)GetTopLevelWidget()),
		CMap, &ColorCellDef);
    PixelValue = SwapLong( ColorCellDef.pixel, FROMLOCAL(WhatTool->ConnectPrefix.byteOrder) );
#       if DEBUGHandleAllocColorPacket
    fprintf( stderr, "HandleAllocColorPacket():RGB Value red=%08x green=%08x blue=%08x\n",
	    ColorCellDef.red, ColorCellDef.green, ColorCellDef.blue);
    fprintf( stderr, "HandleAllocColorPacket():PixelValue=%08x\n", PixelValue);
#      endif
  }
  /* If we have an AllocNamedColor request, call the local display server and allocate a cell in
     cmap*/
  if ( XCOLOR &&( Header.reqType == X_AllocNamedColor ) &&
      (*((Colormap *)Stuff) == WhatTool->HostRootInfo.defaultColormap)) {
    
    NameLength = SwapInt(*((CARD16 *)(Stuff+4)),WhatTool->ConnectPrefix.byteOrder);
    NamePacketLength = ((NameLength+1) / 4 +1)*4;
    ColorName = (char *)malloc(NamePacketLength);
    strncpy(ColorName, (char *)(Stuff+8), NameLength);
    ColorName[NameLength] = '\0';
    CMap = SwapLong( *((Colormap *)Stuff), WhatTool->ConnectPrefix.byteOrder);
    XAllocNamedColor(XtDisplay((Widget *)GetTopLevelWidget()), CMap,
		     ColorName, &ColorCellDef, &RgbDbDef);
    PixelValue = SwapLong( ColorCellDef.pixel, FROMLOCAL(WhatTool->ConnectPrefix.byteOrder
							 ) );
#       if DEBUGHandleAllocColorPacket
    fprintf( stderr, "HandleAllocNamedColorPacket():PixelValue=%08x\n", PixelValue);
#      endif
  }
  
  if ( Header.reqType == X_AllocColorCells ) {
    printf("an AllocColorCells Packet found! \n");
  }
  
  
  /* Now ship everybody who's a subscriber a copy, in the usual protcol-istic fashion */
  
  /*
   * Begin Murray Addition
   */
  
  
  for ( Who=FirstSubscriber(); Who != NULL; Who=NextSubscriber() ) {
    /*
     *	for ( startSubscr(WhatTool); !offrightSubscr(WhatTool); forthSubscr(WhatTool))
     */
    /*
     *	    Who = getSubscr(WhatTool);
     */
    if (Who->SubscriberID != 0) {
      
      if( JOIN && ArchiveToBeSent(Who,WhatTool) )
	continue;
#      if DEBUGHandleIncomingClientPacket
      fprintf( stderr, "HandleIncomingClientPacket(): ...Sending to %s\n",
	      Who->LoginName );
#      endif
      
      /* Send the packet */
      
      if (SendByte( Who->Connection, HS_X11Packet )==ERROR) {
	CleanUpSubscriber(Who);
	continue;
      }
      if (SendByte( Who->Connection, WhatTool->ToolID )==ERROR) {
	CleanUpSubscriber(Who);
	continue;
      }
      RBytes = SwapLong( RemainingBytes+sizeof(xReq),
			FROMLOCAL(WhatTool->ConnectPrefix.byteOrder) );
      if(SendData( Who->Connection, &RBytes, sizeof(CARD32) )==ERROR){
	CleanUpSubscriber(Who);
	continue;
      }
      if(SendData( Who->Connection, &Header, sizeof(xReq) )==ERROR){
	CleanUpSubscriber(Who);
	continue;
      }
      if (RemainingBytes) {
	if(SendData( Who->Connection, Stuff2, RemainingBytes )==ERROR){
	  CleanUpSubscriber(Who);
	  continue;
	}
      }
      
      /* If we've got an InternalAtom request, ship that stuff too */
      
      if ( Header.reqType == X_InternAtom ) {
	
	HowMany = SwapInt( *((CARD16 *)Stuff),
			  WhatTool->ConnectPrefix.byteOrder );
	
	if (SendByte( Who->Connection, HS_InternalAtom )==ERROR) {
	  CleanUpSubscriber(Who);
	  continue;
	}
	if (SendByte( Who->Connection, WhatTool->ToolID )==ERROR) {
	  CleanUpSubscriber(Who);
	  continue;
	}
	if(SendData( Who->Connection, &AtomValue, sizeof(CARD32) )==ERROR){
	  CleanUpSubscriber(Who);
	  continue;
	}
	if(SendData( Who->Connection, Stuff, sizeof(CARD16) )==ERROR){
	  CleanUpSubscriber(Who);
	  continue;
	}
	if(SendData( Who->Connection, Stuff+(sizeof(xInternAtomReq)-sizeof(xReq)), HowMany )==ERROR){
	  CleanUpSubscriber(Who);
	  continue;
	}
      }
      
      /* if we got an AllocColor or AllocNamedColor request, ship that stuff too */
      
      if ( XCOLOR && ( Header.reqType == X_AllocColor || 
		      Header.reqType == X_AllocNamedColor ) &&
	  (*((Colormap *)Stuff) == WhatTool->HostRootInfo.defaultColormap)) {
	
	if (SendByte( Who->Connection, HS_AllocColor )==ERROR) CleanUpSubscriber(Who);
	if (SendByte( Who->Connection, WhatTool->ToolID )==ERROR) CleanUpSubscriber(Who);
	if (SendData( Who->Connection, Stuff, sizeof(CARD32) )==ERROR){  /* COLORMAP */
	  CleanUpSubscriber(Who);
	  continue;
	}
	if ( Header.reqType == X_AllocColor ) {
	  NameTag = SwapLong( 0, FROMLOCAL(WhatTool->ConnectPrefix.byteOrder) );
	  if (SendData( Who->Connection, &NameTag, sizeof(CARD32) )==ERROR) CleanUpSubscriber(Who);
	  if(SendData( Who->Connection, Stuff+4, 4*sizeof(CARD16) )==ERROR){ /* RED     */
	    CleanUpSubscriber(Who);
	    continue;
	  }
	  /*
	    if(SendData( Who->Connection, Stuff+6, sizeof(CARD16) )==ERROR){
	    CleanUpSubscriber(Who);
	    continue;
	    }
	    if(SendData( Who->Connection, Stuff+8, sizeof(CARD16) )==ERROR){
	    CleanUpSubscriber(Who);
	    continue;
	    }
	    */
	}
	else { /* AllocNamedColor request */
	  NameTag = SwapLong( 1, FROMLOCAL(WhatTool->ConnectPrefix.byteOrder) );
	  if (SendData( Who->Connection, &NameTag, sizeof(CARD32) )==ERROR) CleanUpSubscriber(Who)
	    ;
	  HowMany = NamePacketLength;
/*
 * chungg's bugfix.
 *	  NamePacketLength = SwapInt( NamePacketLength,
 *				     FROMLOCAL(WhatTool->ConnectPrefix.byteOrder) );
 *
 */
	  HowManyS = SwapInt( NamePacketLength,
			     FROMLOCAL(WhatTool->ConnectPrefix.byteOrder) );
	  
/*
 * chungg's bugfix.
 *	  if(SendData( Who->Connection, &NamePacketLength, sizeof(CARD16) )==ERROR){
 *	    CleanUpSubscriber(Who);
 *	    continue;
 *	  }
 *
 */
	  if(SendData( Who->Connection, &HowManyS, sizeof(CARD16) )==ERROR){
	    CleanUpSubscriber(Who);
	    continue;
	  }
	  if(SendData( Who->Connection, &Pad, sizeof(CARD16) )==ERROR){
	    CleanUpSubscriber(Who);
	    continue;
	  }
	  if(SendData( Who->Connection, ColorName, HowMany )==ERROR){
	    CleanUpSubscriber(Who);
	    continue;
	  }
	}
	if(SendData( Who->Connection, &PixelValue, sizeof(CARD32) )==ERROR){/*PIXELVAL */
	  CleanUpSubscriber(Who);
	  continue;
	}
      }
      
    }
    
  }
  if( JOIN && Header.reqType == X_InternAtom )
    {
      CARD32 TotalSize;
      
      HowMany = SwapInt( *((CARD16 *)Stuff),
			WhatTool->ConnectPrefix.byteOrder );
      TotalSize = 9+HowMany;
      ArchPacketP = GetMem(TotalSize);
      ArchPacketP[0] = Header.data; /* onlyIfExistsfield */
      ArchPacketP[1] = HS_InternalAtom;
      ArchPacketP[2] = WhatTool->ToolID;
      memcpy(ArchPacketP+3,(char *) &AtomValue,4);
      memcpy(ArchPacketP+7,Stuff,2);
      memcpy(ArchPacketP+9,Stuff+(sizeof(xInternAtomReq)-sizeof(xReq)),HowMany);
      if( (WhatTool->ToolInfo->InternalAtom =
	   realloc(WhatTool->ToolInfo->InternalAtom,
		   WhatTool->ToolInfo->InternalAtomSize+4+TotalSize))
	 == NULL){
	perror("Reallocating");
	exit(1);
      }
      memcpy(WhatTool->ToolInfo->InternalAtom+WhatTool->ToolInfo->InternalAtomSize
	     ,(char *) &TotalSize,4);
      memcpy(WhatTool->ToolInfo->InternalAtom+WhatTool->ToolInfo->InternalAtomSize
	     +4,ArchPacketP,TotalSize);
      WhatTool->ToolInfo->InternalAtomSize += ( 4+TotalSize );
      if( WhatTool->SubDirtyCount > 0 ){
	AppendRequestToArchiveBuf(WhatTool,ArchPacketP+1,TotalSize-1);
      }
      FreeMem(ArchPacketP);
    }
  if ( JOIN && XCOLOR &&
      ( Header.reqType == X_AllocColor || Header.reqType == X_AllocNamedColor ) &&
      (*((Colormap *)Stuff) == WhatTool->HostRootInfo.defaultColormap) ) {
    CARD32 TotalSize;
    ColormapObj *FindColormapByID();
    ColormapObj *cop;
    
    if( Header.reqType == X_AllocColor )
      TotalSize = 22;
    else
      TotalSize = 18+NamePacketLength;
    
    ArchPacketP = GetMem(TotalSize);
    
    ArchPacketP[0] = HS_AllocColor;
    ArchPacketP[1] = WhatTool->ToolID;
    memcpy(ArchPacketP+2,Stuff,sizeof(CARD32));
    if ( Header.reqType == X_AllocColor ) {
      NameTag = SwapLong( 0, FROMLOCAL(WhatTool->ConnectPrefix.byteOrder) );
      memcpy(ArchPacketP+6,(char *)(&NameTag),sizeof(CARD32));
      memcpy(ArchPacketP+10,Stuff+4,4*sizeof(CARD16));
      memcpy(ArchPacketP+18,(char *)(&PixelValue),sizeof(CARD32));
    }
    else { /* AllocNamedColor request */
      NameTag = SwapLong( 1, FROMLOCAL(WhatTool->ConnectPrefix.byteOrder) );
      memcpy(ArchPacketP+6,(char *) &NameTag,sizeof(CARD32));
      HowMany = NamePacketLength;
      HowManyS = SwapInt( NamePacketLength,
			 FROMLOCAL(WhatTool->ConnectPrefix.byteOrder) );
      memcpy(ArchPacketP+10,(char *) &HowManyS,sizeof(CARD16));
      memcpy(ArchPacketP+12,(char *) &Pad,sizeof(CARD16));
      memcpy(ArchPacketP+14,ColorName,HowMany);
      memcpy(ArchPacketP+14+HowMany,(char *) &PixelValue,sizeof(CARD32));
    }
    if( (cop = FindColormapByID(SwapLong(*((CARD32 *)Stuff),WhatTool->ConnectPrefix.byteOrder),WhatTool)) == NULL ){
      fprintf(stderr,"HandleIncomingClientPacket: Couldn't find the colormap\n");
      exit(1);
    }
    cop->AllocPackets = realloc(cop->AllocPackets,cop->AllocPacketsSize+4+TotalSize);
    memcpy(cop->AllocPackets+cop->AllocPacketsSize,(char *) &TotalSize,sizeof(CARD32));
    memcpy(cop->AllocPackets+cop->AllocPacketsSize+4,ArchPacketP,TotalSize);
    cop->AllocPacketsSize += (4+TotalSize);
    if( WhatTool->SubDirtyCount > 0 ){
      AppendRequestToArchiveBuf(WhatTool,ArchPacketP,TotalSize);
    }
    FreeMem(ArchPacketP);
  }
  if ( RemainingBytes ) {
    FreeMem( Stuff );
  }
  FreeMem( Stuff2 );
}




/*
 *   BYTE *GetReply
 *
 *   Check to see if the server has anything to send us.
 *
 *   RETURN VALUE:   Pointer to sent data if and is available
 *         NULL otherwise
 */
BYTE *GetReply( Source, WhatTool )
     Socket *Source;
     Tool *WhatTool;
{
  BYTE      *First32;
  BYTE      *Remainder;
  int      ReplyLength;
  xError     *PE;
  
  if ( (First32=ReadData( Source, sizeof(xGenericReply), BLOCK)) == NULL ) {
    ReadError( WhatTool);
    return (NULL);
  } else{
#   if DEBUGGetReply
    fprintf(stderr, "DEBUGGetReply(): Got First32 of REPLY/EVENT\n" );
printf("FRESH...Sequence No. is: %04d \n", (((xGenericReply *)First32)->sequenceNumber));
# endif
    if ( *First32 == 1 ) {
      ReplyLength = (SwapLong( ((xGenericReply *)First32)->length, WhatTool->ConnectPrefix.byteOrder)) * 4;
#   if DEBUGGetReply
      fprintf(stderr, "DEBUGGetReply(): Detected a REPLY, length is %d Bytes\n", 
	      32 + ReplyLength);
# endif
      if ( ReplyLength == 0 ) {
	return( First32 );
      }
#   if DEBUGGetReply
      fprintf( stderr,"DEBUGGetReply(): reading remainder of REPLY" );
#   endif
      
      if ((Remainder = ReadData( Source, ReplyLength, BLOCK ))==NULL){
	ReadError( WhatTool);
	return (NULL);
      }
      First32 = (BYTE *)realloc( First32, 32+ReplyLength );
      memcpy( First32+32, Remainder, ReplyLength );
      free( Remainder );
      return( First32 );
      
    }
    
    /* Anything else must be an event or an error... Always 32 bytes */
    
    PE = (xError *)First32;
    if (PE->type == 0){
#   if DEBUGGetReply
      
      fprintf( stderr,"DEBUGGetReply():Detected an ERROR, errCode (%d), sequenceNumber(%d) \n",
	      
	      PE-> errorCode, PE->sequenceNumber);
#   endif
    } else {
#   if DEBUGGetReply
      
      fprintf( stderr,"DEBUGGetReply():Detected an EVENT, OpCode (%d), sequenceNumber(%d)\n", 
	      PE->errorCode, PE->sequenceNumber);
#   endif
      
    }
    
    return( First32 );
  }
}


/*
 *   void SendReply
 *
 *   Send a reply, event, or error.  Detemine which it is by examining the
 *   first byte in the packet and act accordingly.
 *
 *   RETURN VALUE:   (void)
 */

SendReply( Dest, Packet, WhatTool )
     Socket Dest;
     BYTE *Packet;
     Tool *WhatTool;
     
{
  int   Length;
  int   ReplyLength;
  
#   if DEBUGSendReply
  fprintf( stderr,"DEBUGSendReply(): Sendig  a Reply \n");
#   endif
/*
    if ( GetMode() ) FixStampAndSeq( Packet, WhatTool );
*/
  if (GetMode() ){
    (((xGenericReply *)Packet)->sequenceNumber) = (WhatTool->LastSeqNum); 
    /* Tried to be Removed by GB */
#   if DEBUGSendReply
printf("Sequence No. is: %04d \n", (((xGenericReply *)Packet)->sequenceNumber));
    fprintf( stderr,"DEBUGSendReply(): Fixed Sequence Number, Last %04x \n", (WhatTool->LastSeqNum));
    fprintf( stderr,"DEBUGSendReply(): Sequence Number, Last %04x \n", ((((xGenericReply *)Packet)->sequenceNumber)));
#   endif
  }
  
  if ( *Packet == 1 ) {
    if ( GetMode() ) WhatTool->pending_reply = FALSE;
    ReplyLength = (SwapLong( ((xGenericReply *)Packet)->length, WhatTool->ConnectPrefix.byteOrder)) * 4;
    Length = 32 +  ReplyLength ;
#   if DEBUGSendReply
    fprintf( stderr,"DEBUGSendReply(): Reply Length = %d Bytes\n", Length );
#   endif
    
  } else {
    Length = 32;
#   if DEBUGSendReply
    fprintf( stderr,"DEBUGSendReply(): Event Length = %d Bytes\n", Length );
#   endif
    
  }
  if (SendData( Dest, Packet, Length ) == ERROR ) {
#   if DEBUGSendReply
    fprintf( stderr,"DEBUGSendReply(): Could't send data, Tool [%s-PID %d] is Terminated\n",
	    WhatTool->ToolCommand, WhatTool->ToolPID);
#   endif
    if (GetMode()) {
      CleanUpTool(WhatTool);
      return(ERROR);
    }
    else EndSession();
#   if DEBUGSendReply
    fprintf( stderr,"DEBUGSendReply(): Cleaned Up\n");
#   endif
  }
  
  
}



/*
 *   HandleIncomingServerPacket
 *
 *   Handle a packet outbound from the server and return it
 *   to the client.
 *
 *   RETURN VALUE:   void
 */
XtInputCallbackProc HandleIncomingServerPacket( WhatTool, Port, InputID )
     Tool *WhatTool;
     Socket *Port;
     XtInputId *InputID;
{
  BYTE      *Packet;
  
  /* Reset the toolkit before leaving */
  XtRemoveInput( *InputID );
  WhatTool->OutID=XtAppAddInput( GetAppCon(), *Port, XtInputReadMask, HandleIncomingServerPacket, WhatTool );
#   if DEBUGHandleIncomingServerPacket
  fprintf( stderr, "HandleIncomingServerPacket(): event/reply  detected\n" );
#   endif
  
  if ((Packet=GetReply(*Port, WhatTool)) != NULL) {
  if (GetMode() )
    (WhatTool->LastSeqNum)= ((xGenericReply *)Packet)->sequenceNumber;
    if ( VERBOSE ){
      if ( (++(WhatTool->OutCount) % 20) == 0) printf("%d --> %s\n", WhatTool->OutCount, WhatTool->ToolCommand);
    }
    if (*Packet == 12){
      /* Exposure event, send it out */
      SendReply( WhatTool->InConnection, Packet, WhatTool);
#   if DEBUGHandleIncomingServerPacket
      fprintf( stderr, "HandleIncomingServerPacket(): Exposure event sent to client\n" );
#   endif
    } else if ( WhatTool->TokenHolder == HOST_ID ) {
      SendReply( WhatTool->InConnection, Packet, WhatTool);

#   if DEBUGHandleIncomingServerPacket
      fprintf( stderr, "HandleIncomingServerPacket(): Reply/Event sent to client\n" );
#   endif
    }
    if ( *Packet == 1 )
      WhatTool->server_pending_reply = 0;
  }
  FreeMem( Packet );
}


/*
 *   HandlePortException
 *
 *   Handle any I/O exeptions from the input or output port(s) by
 *   closing 'em up and removing the tool from the list.
 *
 *   RETURN VALUE:   void
 */
XtInputCallbackProc HandlePortException( WhatTool, Port, InputID )
     Tool *WhatTool;
     Socket *Port;
     XtInputId *InputID;
{
  /* Tell Xt to forget about us and close up the ports */
  shutdown( WhatTool->InConnection );
  XtRemoveInput( WhatTool->InID );
  XtRemoveInput( WhatTool->OutID );
  XtRemoveInput( WhatTool->InExceptID );
  XtRemoveInput( WhatTool->OutExceptID );
  
  
#   if DEBUGHandlePortException
  fprintf( stderr, "HandlePortException(): Port exception on %dConnection\n",
	  ( *Port == WhatTool->InConnection ? "In" : "Out" ) );
#   endif
  
  UpdateMessage( "Tool %s has %s.\n", ( *Port == WhatTool->InConnection ? "terminated and disconnected" : "lost its connection to the server" ) );
  
  /* Cook the tool */
  
  DeleteTool( WhatTool );
  
}


/*
 *   HandleIncomingViewerPacket
 *
 *   Handle an inbound event from a viewer's server.  Do all
 *   translations, and ship it, along with the appropriate protocol
 *   stuff to the host if needed.
 *
 *   RETURN VALUE:   void
 */
XtInputCallbackProc HandleIncomingViewerPacket( WhatTool, Port, InputID )
     Tool *WhatTool;
     Socket *Port;
     XtInputId *InputID;
{
  BYTE      *Packet;
  
  /* Reset the toolkit before leaving */
  XtRemoveInput( *InputID );
  WhatTool->OutID=XtAppAddInput( GetAppCon(), *Port, XtInputReadMask, HandleIncomingViewerPacket, WhatTool );
#   if DEBUGHandleIncomingViewerPacket
  fprintf( stderr, "HandleIncomingViewerPacket(): REPLY/EVENT  detected\n" );
#   endif
  
  if ((Packet=GetReply(*Port, WhatTool)) != NULL) {
    if ( VERBOSE ){
      if ( (++(WhatTool->OutCount) % 20) == 0) printf("%d --> %s\n", WhatTool->OutCount, WhatTool->ToolCommand);
    }
    if (*Packet == 12){
      /* Exposure event, send it out */
      TranslateViewerEvent( Packet, WhatTool);
      if (SendByte( GetHostConnection(), SH_Event )== ERROR) EndSession();
      if (SendByte( GetHostConnection(), WhatTool->ToolID )== ERROR) EndSession;
      SendReply( GetHostConnection(), Packet, WhatTool);

#   if DEBUGHandleIncomingServerPacket
      fprintf( stderr, "HandleIncomingViewerPacket(): Exposure event sent to host\n" );
#   endif
    } else if ( (*Packet == 1) && (WhatTool->TokenHolder == GetLocalSubID()) ) {
#   if DEBUGHandleIncomingViewerPacket
      fprintf( stderr, "HandleIncomingViewerPacket(): Sending REPLY to Host\n" );
#   endif
      TranslateReply( Packet, WhatTool);
      if (SendByte( GetHostConnection(), SH_Reply )==ERROR) EndSession();
      if(SendByte( GetHostConnection(), WhatTool->ToolID )==ERROR) EndSession();
      SendReply( GetHostConnection(), Packet, WhatTool);

    } else if ( TRUE /* DetermineViewerRouting( WhatTool, Packet ) */) {
#         if DEBUGHandleIncomingViewerPacket
      fprintf( stderr, "HandleIncomingViewerPacket(): Sending EVENT to Host\n" );
#         endif
      TranslateViewerEvent( Packet, WhatTool ) ;
      if (SendByte( GetHostConnection(), SH_Event )==ERROR) EndSession();
      if (SendByte( GetHostConnection(), WhatTool->ToolID )==ERROR) EndSession();
      SendReply( GetHostConnection(), Packet, WhatTool);

    }
    
  }
  FreeMem( Packet );
  
  
}

ReadError(WhatTool)
     Tool *WhatTool;
{
#   if DEBUGReadError
  fprintf(stderr, "DEBUGReadError(): Got ERROR \n");
  fprintf(stderr, "DEBUGReadError(): terminating tool %d\n", WhatTool->ToolID);
# endif
  if (GetMode()){
    CleanUpTool(WhatTool);
    return (ERROR);
  }
  else {
    if ( SendByte(GetHostConnection(), SH_StopTool)==ERROR) EndSession();
    if ( SendByte(GetHostConnection(), WhatTool->ToolID)==ERROR) EndSession();
    CleanUpTool(WhatTool);
    return(ERROR);
  }
}
