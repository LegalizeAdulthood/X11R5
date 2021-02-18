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


#include <X11/Xlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "network.h"
#include "subscriber.h"
#include "protocol.h"
#include "tool.h"
#include "objects.h"
#include "memory.h"
#include "misc.h"
#include "archive.h"

#define	DEBUGAppendRequestToArchiveBuf		0&DEBUGON
#define	DEBUGArchivePacket			0&DEBUGON

extern BYTE LocalByteOrder();

static int BeingSwappedToLocal;

/*
 *	AppendRequestToArchiveBuf
 *
 *	Appends a new xtv protocol request packet to either Archive or Archive2
 *
 *	packet format:	HS_X11Packet or HS_InternalAtom ( 1 byte )
 *			ToolID				( 1 byte )
 *			Length of the X11 packet	( 4 bytes )
 *			X11 packet			( n bytes )
 *
 *	RETURN VALUE:	void
 */

AppendRequestToArchiveBuf(tool,packet,pksize)
Tool    *tool;
char *packet;	/* format as explained above */
int pksize;	/* 6+n */
{
	char **buffer;
	off_t *size;

	/* Check if we have to swap some byte orders */
	if( *packet != HS_InternalAtom &&
	    LocalByteOrder() != tool->ConnectPrefix.byteOrder ){
		BeingSwappedToLocal = 0;
		SwapPacket(tool,(packet+6));
	}

	if( tool->SubDirtyCount > 0 ){
#if DEBUGAppendRequestToArchiveBuf
		fprintf(stderr,"AppendRequestToArchiveBuf():Saving the packet to Archive2\n");
#endif
		buffer = &(tool->Archive2Buf);
		size = &(tool->Archive2Size);
	}
	else{
#if DEBUGAppendRequestToArchiveBuf
		fprintf(stderr,"AppendRequestToArchiveBuf():Saving the packet to Archive\n");
#endif
		buffer = &(tool->ArchiveBuf);
		size = &(tool->ArchiveSize);
	}

	if((*buffer = realloc(*buffer,*size+sizeof(pksize)+pksize))==NULL)
        {
                perror("Reallocating");
                exit(1);
        }
/*
 * Archive format
 *
 * 	length of the packet	( 4 bytes )
 *	packet as given by the formal parameter ( 6+n bytes )
 */
        memcpy(*buffer+*size,(char *) &pksize,sizeof(pksize));
        memcpy(*buffer+*size+sizeof(pksize),packet,pksize);
        *size += ( pksize + sizeof(pksize) );

#if DEBUGAppendRequestToArchiveBuf
	if( *packet == HS_InternalAtom ){
		fprintf(stderr,"InternalAtom\n");
	}
	else if( *packet == HS_X11Packet ){
		fprintf(stderr,"%d %s\n",*(packet+6),ArchiveTable[*(packet+6)].Request);
	}
	else if( *packet == HS_AddNewTool ){
		fprintf(stderr,"AddNewTool\n");
	}
#endif
}

/*
 *	AppendX11RequestToArchiveBuf2
 *
 *	Gets a X11 packet prepended by the length of it and turns it into
 *	xtv protocol format
 *
 *	buf format:	lenth of the X11 packet		( 4 bytes )
 *			X11 packet			( n bytes )
 *
 *	RETURN VALUE: void
 */
			
AppendX11RequestToArchiveBuf2(tool,buf)
Tool *tool;
char *buf;
{
        CARD32 size;
        CARD32 RBytes;
        char *mybuf; /* to contain HS_X11Packet, tool->ToolID, length in
                        subscriber's format and the packet */

        size = *((CARD32 *) buf);
        mybuf = GetMem(size+6);
        mybuf[0] = HS_X11Packet;
        mybuf[1] = tool->ToolID;
        RBytes = SwapLong( size, FROMLOCAL(tool->ConnectPrefix.byteOrder) );
        memcpy(mybuf+2,(char *) &RBytes,4);
        memcpy(mybuf+6,buf+4,size);
        AppendRequestToArchiveBuf(tool,mybuf,size+6);
        FreeMem(mybuf);
}

/*
 *	AppendX11RequestToArchiveBuf3
 *
 *	Gets a X11 packet from buf and the size of it from size and turns it
 *	into xtv protocol format
 *
 *	RETURN VALUE: void
 */

AppendX11RequestToArchiveBuf3(tool,buf,size)
Tool *tool;
char *buf;
CARD32 size;
{
        CARD32 RBytes;
        char *mybuf;

        mybuf = GetMem(size+6);
        mybuf[0] = HS_X11Packet;
        mybuf[1] = tool->ToolID;
        RBytes = SwapLong( size, FROMLOCAL(tool->ConnectPrefix.byteOrder) );
        memcpy(mybuf+2,(char *) &RBytes,4);
        memcpy(mybuf+6,buf,size);
        AppendRequestToArchiveBuf(tool,mybuf,size+6);
        FreeMem(mybuf);
}

/*
 *	AppendX11PutImageRequestToArchiveBuf
 *
 *	Appends a series of PutImage packets contained in buf and turns each
 *	into xtv protocol format
 *
 *	buf format:	x repetitions of following ( x > 0 )
 *			length of the following PutImage X11 packet ( 4 bytes )
 *			X11 PutImage packet			    ( n bytes )
 *
 *	RETURN VALUE: void
 */

AppendX11PutImageRequestToArchiveBuf(tool,buf,size,gcid)
Tool *tool;
char *buf;
CARD32 size;	/* total size of buf  (4+n)x */
CARD32 gcid;	/* graphics context ID to be used to 'put' the images */
{
	CARD32 CurPos;

	for( CurPos = 0; CurPos < size;
	     CurPos += (*((CARD32 *)(buf+CurPos))+sizeof(CARD32)) ){
		((xPutImageReq *)(buf+CurPos+4))->gc = gcid;
		AppendX11RequestToArchiveBuf2(tool,buf+CurPos);
	}
}

/*
 *	ArchivePacket
 *
 *	Make changes in the server state for the tool for each arriving packet
 * 	from the client
 *
 *	RETURN VALUE: void
 */

ArchivePacket(tool,packet,pksize)
Tool	*tool;
char *packet;	/* X11 packet */
int pksize;	/* size of the packet */
{
	char *mypacket;
	xReq *request;

	mypacket = GetMem(pksize);
	memcpy(mypacket,packet,pksize);

	request = ( xReq * ) ( mypacket );

#if DEBUGArchivePacket
	fprintf(stderr,"%d %d %s\n",request->reqType,request->data,ArchiveTable[request->reqType].Request);
#endif

	/* execute the given packet */
	if( (ArchiveTable[request->reqType]).Func != NULL ){
		/* Swap byteOrder if necessary */
		if( LocalByteOrder() != tool->ConnectPrefix.byteOrder ){
			BeingSwappedToLocal = 1;
			SwapPacket(tool,mypacket);
		}
		(ArchiveTable[request->reqType].Func)(tool,mypacket,pksize);
	}

	/* if anyone is catching up on this tool, append the packet
	   to the archive so that he can get this packet, too */
	if( tool->SubDirtyCount > 0 )
		AppendX11RequestToArchiveBuf3(tool,mypacket,pksize);
	FreeMem(mypacket);
}

/*
 *	ArchiveToBeSent
 *
 *	Decides if a subscriber is going to receive packets for the tool
 *
 *	RETURN VALUE: void
 */

ArchiveToBeSent(Who,NewTool)
Subscriber	*Who;
Tool		*NewTool;
{
	ToolNode	*tmp;

	/* if we got past the tool, Who is not going to get the packet */
	for( tmp = (ToolNode *)(Who->CurTool);
	     tmp != NULL && tmp->ToolS != NewTool;
	     tmp = tmp->Next );
	return((tmp==NULL)?0:1);
}

/*
 *	CombinePacketAndSize
 *
 *	Convenience routine to combine a packet and its size into one buffer
 *
 *	RETURN VALUE: pointer to the buffer
 */

char *CombinePacketAndSize(packet,size)
char *packet;
CARD32 size;
{
        char *p;

        p = GetMem(size+sizeof(size));
        memcpy(p,(char *) &size,sizeof(size));
        memcpy(p+sizeof(size),packet,size);
        return(p);
}

/*
 *	SwapPacket
 *
 *	Swaps some fields of a packet
 *
 *	RETURN VALUE: void
 */

SwapPacket(tool,packet)
Tool *tool;
char *packet;
{
	int i;
	CARD16 tmp16;
	CARD32 tmp32;

	for( i = 0; ((ArchiveTable[*packet]).OffsetList[i]).Offset != 0; i++)
		if( ((ArchiveTable[*packet]).OffsetList[i]).size == 2 ){
			memcpy((char *) &tmp16,packet+((ArchiveTable[*packet]).OffsetList[i]).Offset,2);
			tmp16 = SwapInt(tmp16,tool->ConnectPrefix.byteOrder);
			memcpy(packet+((ArchiveTable[*packet]).OffsetList[i]).Offset,(char *) &tmp16,2);
		}
		else{
			memcpy((char *) &tmp32,packet+((ArchiveTable[*packet]).OffsetList[i]).Offset,4);
			tmp32 = SwapLong(tmp32,tool->ConnectPrefix.byteOrder);
			memcpy(packet+((ArchiveTable[*packet]).OffsetList[i]).Offset,(char *) &tmp32,4);
		}

	if( (ArchiveTable[*packet]).SwapFunc != NULL )
		((ArchiveTable[*packet]).SwapFunc)(tool,packet);
}

/*
 *	SwapCARD32Buffer
 *
 *	Swaps a buffer full of CARD32 numbers
 *
 *	RETURN VALUE: void
 */

void SwapCARD32Buffer(tool,buf,limit,mask)
Tool *tool;
char *buf;
int limit;
CARD32 mask;
{
	int i;
	int num = 0;
	CARD32	tmp32;

	/* See if the mask has been swapped to client byteOrder */
	if( BeingSwappedToLocal == 0 )
		mask = SwapLong(mask,tool->ConnectPrefix.byteOrder);

	for( i = 0; i < limit; i++ ){
		if( mask & 1 << i ){
			memcpy((char *) &tmp32,buf+num*4,4);
			tmp32 = SwapLong( tmp32, tool->ConnectPrefix.byteOrder );
			memcpy(buf+num*4,(char *) &tmp32,4);
			num++;
		}
	}
}

/*
 *	SwapCARD32Buffer2
 *
 *	Swaps a buffer full of CARD32 numbers. mask is of 2 bytes this time
 *
 *	RETURN VALUE: void
 */

void SwapCARD32Buffer2(tool,buf,limit,mask)
Tool *tool;
char *buf;
int limit;
CARD16 mask;
{
	int i;
	int num = 0;
	CARD32	tmp32;

	/* See if the mask has been swapped to client byteOrder */
	if( BeingSwappedToLocal == 0 )
		mask = SwapInt(mask,tool->ConnectPrefix.byteOrder);

	for( i = 0; i < limit; i++ ){
		if( mask & 1 << i ){
			memcpy((char *) &tmp32,buf+num*4,4);
			tmp32 = SwapLong( tmp32, tool->ConnectPrefix.byteOrder );
			memcpy(buf+num*4,(char *) &tmp32,4);
			num++;
		}
	}
}

/*
 *	SwapCreateWindow
 *
 *	Swaps attributes fields of CreateWindow packets
 *
 *	RETURN VALUE: void
 */

void SwapCreateWindow(tool,packet)
Tool *tool;
char *packet;
{
	CARD32 mask;

	memcpy((char *) &mask,packet+sizeof(xCreateWindowReq)-4,4);
	SwapCARD32Buffer(tool,packet+sizeof(xCreateWindowReq),15,mask);
}

/*
 *	SwapChangeWindowAttributes
 *
 *	Swaps attributes fields of ChangeWindowAttributes packets
 *
 *	RETURN VALUE: void
 */

void SwapChangeWindowAttributes(tool,packet)
Tool *tool;
char *packet;
{
	CARD32 mask;

	memcpy((char *) &mask,packet+sizeof(xChangeWindowAttributesReq)-4,4);
	SwapCARD32Buffer(tool,packet+sizeof(xChangeWindowAttributesReq),15,mask);
}

/*
 *	SwapConfigureWindow
 *
 *	Swaps attributes fields of ConfigureWindow packets
 *
 *	RETURN VALUE: void
 */

void SwapConfigureWindow(tool,packet)
Tool *tool;
char *packet;
{
	CARD16 mask;

	memcpy((char *) &mask,packet+sizeof(xConfigureWindowReq)-4,2);
	SwapCARD32Buffer2(tool,packet+sizeof(xConfigureWindowReq),7,mask);
}

/*
 *	SwapCreateGC
 *
 *	Swaps attributes fields of CreateGC packets
 *
 *	RETURN VALUE: void
 */

void SwapCreateGC(tool,packet)
Tool *tool;
char *packet;
{
	CARD32 mask;

	memcpy((char *) &mask,packet+sizeof(xCreateGCReq)-4,4);
	SwapCARD32Buffer(tool,packet+sizeof(xCreateGCReq),23,mask);
}

/*
 *	SwapChangeGC
 *
 *	Swaps attributes fields of ChangeGC packets
 *
 *	RETURN VALUE: void
 */

void SwapChangeGC(tool,packet)
Tool *tool;
char *packet;
{
	CARD32 mask;

	memcpy((char *) &mask,packet+sizeof(xChangeGCReq)-4,4);
	SwapCARD32Buffer(tool,packet+sizeof(xChangeGCReq),23,mask);
}
