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

#define NEED_REPLIES
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include "objects.h"
#include "tool.h"
#include "subscriber.h"
#include "misc.h"
#include "protocol.h"

extern SubscriberNode *SubscriberList;
extern XtInputCallbackProc HandleIncomingServerPacket();

/*
 *	CreateExposeList
 *
 *	Create the list of nodes for each window xtv is waiting for
 *	an exposure event for
 *
 *	RETURN VALUE: void
 */

CreateExposeList(tool,start)
Tool *tool;
ExposeWindow **start;
{
	ExposeWindow *temp;
	off_t curpos;
	off_t addend;
	Window Prev = 0;
	Window wid;

	/* Archive2Buf must have PutImage packets for backing store window
	   from the start of the buffer */
	for( curpos=0; curpos < tool->Archive2Size &&
		       *(tool->Archive2Buf+curpos+10) == X_PutImage; ){
		/* get the window ID out of the packet */
		memcpy((char *) &(wid),tool->Archive2Buf+curpos+14,4);
		/* there can be multiple PutImage packets for a window */
		if( wid != Prev ){
			Prev = wid;
			temp = (ExposeWindow *) GetMem(sizeof(ExposeWindow));
			temp->wid = wid;
			temp->ExposeCame = 0;
			temp->Next = *start;
			*start = temp;
		}
		memcpy((char *)&addend,tool->Archive2Buf+curpos,4);
		curpos += ( addend + 4 );
	}
}

/*
 *	CreatePutImagePackets
 *
 *	Create PutImage packets for a drawable given as a parameter
 *
 *	RETURN VALUE: pointer to the buffer containing the packets
 */

char * CreatePutImagePackets(tool,drawable,height,width,total)
Tool *tool;
Drawable drawable;
int height,width;
CARD32 *total;
{
	xGetImageReq gireq;
	xGetImageReply *Reply;
	CARD32 bytes_per_line;
	char *data;
	CARD32 LinesPerPackets;
	CARD32 i;
	char *buf;
	xPutImageReq *req;
	CARD32 pksize;
	int running_height;

	/* Create the GetImage request packet */
	gireq.reqType = X_GetImage;
	gireq.format = 2; /* ZPixmap */
	gireq.length = SwapInt(5,FROMLOCAL(tool->ConnectPrefix.byteOrder));
	gireq.drawable = SwapLong(drawable,FROMLOCAL(tool->ConnectPrefix.byteOrder));
	gireq.x = SwapInt(0,FROMLOCAL(tool->ConnectPrefix.byteOrder));
	gireq.y = SwapInt(0,FROMLOCAL(tool->ConnectPrefix.byteOrder));
	gireq.width = SwapInt(width,FROMLOCAL(tool->ConnectPrefix.byteOrder));
	gireq.height = SwapInt(height,FROMLOCAL(tool->ConnectPrefix.byteOrder));
	gireq.planeMask = SwapLong(0xffffffff,FROMLOCAL(tool->ConnectPrefix.byteOrder));

	/* Send the GetImage packet */
	if( SendData(tool->OutConnection, (char *) &gireq, sizeof(xGetImageReq))==ERROR ){
		CleanUpTool(tool);
		return(NULL);
	}

	/* Get the reply for the request */
	do{
		Reply = (xGetImageReply *) GetReply(tool->OutConnection,tool);
		if( Reply->type == 1 )
			if( tool->server_pending_reply ){
				tool->server_pending_reply = 0;
				if( tool->TokenHolder == HOST_ID )
					SendReply(tool->InConnection,(char *) Reply,tool);
			}
			else /* Now we've got the GetImage reply right */
				break;
		else if( Reply->type == 12 || tool->TokenHolder == HOST_ID )
			SendReply(tool->InConnection,(char *) Reply,tool);
	}while(1);

	Reply->length = SwapLong(Reply->length,tool->ConnectPrefix.byteOrder);
	bytes_per_line =
		pad( (((width*Reply->depth)%8)>0?1:0)+(width*Reply->depth)/8 );

	if( Reply->length != bytes_per_line*height/4 ){
		fprintf(stderr,"CreatePutImagePackets: Unexpected reply format\n");
		exit(1);
	}

	*total = 0;
	buf = GetMem(0);

	/* Get the lines per packet.65508 bytes is max a packet can contain */
	LinesPerPackets = 65508/bytes_per_line;
	data = ((char *) Reply) + sizeof(xGetImageReply);

	/* Divide the image */
	for( i=0; i<height; i+=LinesPerPackets ){

		running_height =
		       ((height>=i+LinesPerPackets)?
			LinesPerPackets:(height-i));

		pksize = sizeof(xPutImageReq)+
			 bytes_per_line*running_height;
		if( (buf = realloc(buf,*total+sizeof(pksize)+pksize)) == NULL ){
			*total = 0;
			FreeMem(buf);
			buf = NULL;
			break;
		}
		memcpy(buf+(*total),(char *) &pksize,sizeof(pksize));
		req = (xPutImageReq *)(buf+*total+sizeof(pksize));
		req->reqType = X_PutImage;
		req->format = 2;
		req->length = pksize/4;
		req->drawable = drawable;
		req->width = width;
		req->height = running_height;
		req->dstX = 0;
		req->dstY = i;
		req->leftPad = 0;
		req->depth = Reply->depth;
		memcpy(buf+(*total)+sizeof(pksize)+sizeof(xPutImageReq),
		      data+i*bytes_per_line,pksize-sizeof(xPutImageReq));
		*total += (sizeof(pksize)+pksize);
	}
	FreeMem(Reply);
	return(buf);
}

/*
 *	PreparePacketsToSend
 *
 *	Prepares packets to send to the new subscriber's server
 *
 *	RETURN VALUE: void
 */

PreparePacketsToSend(tool)
Tool *tool;
{
	CARD32 StartPacketSize;
	SubscriberNode *Current;
	CARD16	tmpHowMuchAuthData, tmpNumVisualIDs;

	GetPixmapAndWindowDrawings(tool);

	tmpHowMuchAuthData = tool->HowMuchAuthData;
	tmpNumVisualIDs = tool->NumVisualIDs;

	tool->HowMuchAuthData = SwapInt(tmpHowMuchAuthData,FROMLOCAL(tool->ConnectPrefix.byteOrder));
	tool->NumVisualIDs = SwapInt(tmpNumVisualIDs,FROMLOCAL(tool->ConnectPrefix.byteOrder));

	StartPacketSize = 1+sizeof(Tool)+tmpHowMuchAuthData+((tmpNumVisualIDs) * sizeof(XID));
	tool->ArchiveBuf = GetMem(StartPacketSize+4);
	tool->ArchiveSize = StartPacketSize+4;
	memcpy(tool->ArchiveBuf,(BYTE *) &StartPacketSize,sizeof(CARD32));
	(tool->ArchiveBuf)[4] = HS_AddNewTool;
	memcpy(tool->ArchiveBuf+5,(BYTE *) tool,sizeof(Tool));
	memcpy(tool->ArchiveBuf+5+sizeof(Tool),(BYTE *)(tool->AuthData),tmpHowMuchAuthData);
	memcpy(tool->ArchiveBuf+5+sizeof(Tool)+tmpHowMuchAuthData,(BYTE *)(tool->VisualIDList),((tmpNumVisualIDs) * sizeof(XID)));

	tool->HowMuchAuthData = tmpHowMuchAuthData;
	tool->NumVisualIDs = tmpNumVisualIDs;

	MakeACreateWindowPacket(tool,tool->ToolInfo->WindowList->Next);
	MakeDestroyedWindows(tool);
	MakeCreatePixmapPackets(tool);
	MakeColormapPackets(tool);
	MakeFontPackets(tool);
	MakeCreateGCPackets(tool);
	CreateGCsForXTV(tool);
	HandleCursorList(tool);
	HandlePixmapList(tool);
	MakeInternAtomPackets(tool);
	MakeCCChangeWindowAttribPackets(tool,tool->ToolInfo->WindowList->Next);
	MakeRemainderPackets(tool);
}

/*
 *	MakeDestroyedWindows
 *
 *	Create packets for CreateWindow for destroyed windows
 *
 *	RETURN VALUE: void
 */

MakeDestroyedWindows(tool)
Tool *tool;
{
	WindowObj *wop;

	for( wop = tool->ToolInfo->Destroyeds->Next; wop != NULL;
	     wop = wop->Next ){
		AppendX11RequestToArchiveBuf3(tool,
				(char *)(wop->Prefix),sizeof(xCreateWindowReq));
	}
}
				
/*
 *	GetPixmapAndWindowDrawings
 *
 *	Get the drawings of pixmaps and backing store windows if necessary
 *
 *	RETURN VALUE: void
 */

GetPixmapAndWindowDrawings(tool)
Tool *tool;
{
	PixmapObj *cpop;	/* Current PixmapObj pointer */
	WindowObj *wop;

	/* pixmap */
	for( cpop = tool->ToolInfo->PixmapList->Next; cpop != NULL;
	     cpop = cpop->Next ){
		/* pixmap drawing has not been gotten yet for dependency */
		if( cpop->PixmapPacketNodeList->Next != NULL &&
		    cpop->PixmapPacketNodeList->Next->PutImagePacket == NULL ){
			cpop->PixmapPacketNodeList->Next->PutImagePacket =
			CreatePutImagePackets(tool,cpop->pid,cpop->height,
			cpop->width,&(cpop->PixmapPacketNodeList->Next->PutImagePacketSize));
		}
		/* pixmap drawing has not been gotten since last drawn into */
		if( cpop->dirty == 1 ){
			MakeANewPixmapPacketNode(tool,cpop);
			cpop->PixmapPacketNodeList->Next->PutImagePacket =
			CreatePutImagePackets(tool,cpop->pid,cpop->height,
			cpop->width,&(cpop->PixmapPacketNodeList->Next->PutImagePacketSize));
        	}
	}

	/* window */
	if( (tool->HostRootInfo).backingStore > 0 ){
	     for( wop = tool->ToolInfo->WindowList->Next->Children->Next;
		wop != NULL; wop = wop->Next )
		    RecGetWindowDrawing(tool,wop);
	}
}

/*
 *	GetImageAble
 *
 *	Finds if it is possible to get the image of a backing store window
 *
 *	RETURN VALUE: 1 - true 0 - false
 */

GetImageAble(tool,wop,Attrib)
Tool *tool;
WindowObj *wop;
XWindowAttributes **Attrib;
{
	int rval = 1;
	int x,y;
	Window child;

	/* Get the window attribute of wop */
	(*Attrib) = (XWindowAttributes *) GetMem(sizeof(XWindowAttributes));
	XGetWindowAttributes(XtDisplay((Widget *)GetTopLevelWidget()),
			     wop->Prefix->wid,*Attrib);

	/* find if the window is viewable */
	if( (*Attrib)->map_state != IsViewable ){
		fprintf(stderr,"Window using backing store is not viewable\n");
		rval = 0;
	}

	/* find if the window is fully inside the screen */
	if( rval != 0 ){
	    XTranslateCoordinates(XtDisplay((Widget *)GetTopLevelWidget()),
				wop->Parent->Prefix->wid,
				tool->ToolInfo->WindowList->Next->Prefix->wid,
				(*Attrib)->x,(*Attrib)->y,
				&x,&y,&child);
	    if( x < 0 || y < 0 ||
	        ( x+(*Attrib)->width) > SwapInt((tool->HostRootInfo).pixWidth,tool->ConnectPrefix.byteOrder) ||
	        ( y+(*Attrib)->height) > SwapInt((tool->HostRootInfo).pixHeight,tool->ConnectPrefix.byteOrder) ){
		    fprintf(stderr,"Window using backing store is not fully inside the screen\n");
		    rval = 0;
	    }
	}

	if( rval == 0 )
		FreeMem(*Attrib);
	return(rval);
}

/*
 *	RecGetWindowDrawing
 *
 *	Traverse the window tree for backing store window and
 *	get the image of such window
 *
 *	RETURN VALUE: void
 */

RecGetWindowDrawing(tool,wop)
Tool *tool;
WindowObj *wop;
{
	WindowObj *current;
	XImage *image;
	xPutImageReq *pirp;
	XWindowAttributes *Attrib;

	if( ( ( (tool->HostRootInfo).backingStore == 1 && /* server: WhenMapped */
	      wop->Prefix->mask & 1<<6 &&
	      ( *(wop->Attrib+6) == 1 || *(wop->Attrib+6) == 2 ) && /* client: WhenMapped or Always */
	      wop->State == 1 ) /* window mapped */
		||
	    ( (tool->HostRootInfo).backingStore == 2 && /* server Always */
	      wop->Prefix->mask & 1<<6 &&
	      ( *(wop->Attrib+6) == 1 || *(wop->Attrib+6) == 2 ) ) ) /* client: WhenMapped or Always */
		&&
	    GetImageAble(tool,wop,&Attrib) ){
	    /* Get the image */
	    wop->PutImagePacket = CreatePutImagePackets(tool,wop->Prefix->wid,
						Attrib->height,Attrib->width,
						&(wop->PutImagePacketSize));
	    FreeMem(Attrib);
	}

	/* for each children */
	for( current = wop->Children->Next; current != NULL;
	     current = current->Next )
		RecGetWindowDrawing(tool,current);
}

/*
 *	MakeColormapPackets
 *
 *	Make colormap related packets
 *
 *	RETURN VALUE: void
 */

MakeColormapPackets(tool)
Tool *tool;
{
	ColormapObj *cop;
	ColormapHeadNode *chnp;
	CARD32	current;
	xResourceReq req; /* to be used for InstallColormap request */
	InstalledColormapNode *icnp;
	CARD32	temp;

	/* find the end of the colormap list */
	for( cop = tool->ToolInfo->ColormapList->Next; cop->Next != NULL;
	     cop = cop->Next );

	/* for each colormap, make all packets related to it */
	for( ; cop != tool->ToolInfo->ColormapList; cop = cop->Prev ){
		if( cop->PacketsSize != 0 ){
			current = 0;
			/* skip CopyColormapAndFree packet because it should
			   already have been created */
			if(*((CARD8*)(cop->Packets+4))==X_CopyColormapAndFree)
				current = *((CARD32 *)(cop->Packets))+4;
			if( cop->HeadColormap->Next != NULL ){
				/* find the end of the HeadColormap list */
				for( chnp = cop->HeadColormap->Next;
				     chnp->Next != NULL; chnp = chnp->Next );
				/* for each colormaps depending this colormap */
				for( ; chnp != cop->HeadColormap
				     ; chnp = chnp->Prev ){
					/* append packets until the point
					   where the depending colormap
					   executed CopyColormapAndFree */
					while(current<chnp->WhereAt){
					    AppendX11RequestToArchiveBuf2(tool,
					    cop->Packets+current);
					    current += (*((CARD32 *)
					    	(cop->Packets+current))+4);
					}
					/* Append the CopyColormapAndFree packet */
					AppendX11RequestToArchiveBuf2(tool,
					((ColormapObj *)(chnp->User))->Packets);
				}
			}
			/* Append the remaining packets */
			while(current<cop->PacketsSize){
				AppendX11RequestToArchiveBuf2(tool,
							cop->Packets+current);
				current+=(*((CARD32*)(cop->Packets+current))+4);
			}
		}
		if( cop->AllocPacketsSize != 0 ){
			current = 0;
			while(current<cop->AllocPacketsSize){
				memcpy((char *) &temp,cop->AllocPackets+current,4);
				AppendRequestToArchiveBuf(tool,cop->AllocPackets+current+4,temp);
				current += (temp+4);
			}
		}
	}

	/* Append packets for InstallColormap */
	if( tool->ToolInfo->ICList->Next != NULL ){
		/* install colormaps */
		/* find the end of the ICN list */
		for( icnp = tool->ToolInfo->ICList->Next; icnp->Next != NULL;
		     icnp = icnp->Next );
		for( ; icnp != tool->ToolInfo->ICList; icnp = icnp->Prev ){
			req.reqType = X_InstallColormap;
			req.length = 2;
			req.id = icnp->cid;
			AppendX11RequestToArchiveBuf3(tool,(char *) &req,8);
		}
	}
}

/*
 *	MakeACreateWindowPacket
 *
 *	Append CreateWindow packets traversing the window tree recursively
 *
 *	RETURN VALUE: void
 */

MakeACreateWindowPacket(tool,wop)
Tool *tool;
WindowObj *wop;
{
	char *packet;
	int i;
	CARD32 *attrib;
	WindowObj *current;
	int count;

	if( tool->ToolInfo->WindowList->Next != wop ){
	   packet = GetMem(sizeof(xCreateWindowReq)+15*sizeof(CARD32));
	   memcpy(packet,(char *)(wop->Prefix),sizeof(xCreateWindowReq));
	   /* Do not set background, border, colormap and cursor */
	   ((xCreateWindowReq *) packet)->mask=
		((xCreateWindowReq *) packet)->mask & 0xffff9ff0;
	   attrib = (CARD32 *) (packet+sizeof(xCreateWindowReq));
	   count = 0;
	   for( i=0; i<15; i++){
		if( ((xCreateWindowReq *) packet)->mask & 1 << i ){
			*attrib = *(wop->Attrib+i);
			attrib++;
			count++;
		}
	   }
	   ((xCreateWindowReq *) packet)->length =
			sizeof(xCreateWindowReq)/4 + count;
	   AppendX11RequestToArchiveBuf3(tool,packet,
					 sizeof(xCreateWindowReq)+4*count);
	   FreeMem(packet);
	}

	/* Do the same for the children */
	if( wop->Children->Next != NULL ){
		/* find the last entry in the children list */
		for( current = wop->Children->Next; current->Next != NULL;
			current = current->Next );
		for( ; current != wop->Children; current = current->Prev )
			MakeACreateWindowPacket(tool,current);
	}
}

/*
 *	MakeFontPackets
 *
 *	Creates font related packets
 *
 *	RETURN VALUE: void
 */

MakeFontPackets(tool)
Tool *tool;
{
	FontObj *fop;

	if( tool->ToolInfo->FontList->Next == NULL )
		return;
	/* find the end of the Font list */
	for( fop = tool->ToolInfo->FontList->Next; fop->Next != NULL;
	     fop = fop->Next );
	for( ; fop != tool->ToolInfo->FontList; fop = fop->Prev ){
		if( fop->SetPathPacket != NULL )
			AppendX11RequestToArchiveBuf2(tool,fop->SetPathPacket);
		if( fop->OpenPacket != NULL )
			AppendX11RequestToArchiveBuf2(tool,fop->OpenPacket);
		if( fop->OpenPacket == NULL && fop->SetPathPacket == NULL ){
			fprintf(stderr,"MakeFontPackets: font list screwed up\n");
			exit(1);
		}
	}
}

/*
 *	MakeCreateGCPackets
 *
 *	Appends CreateGC packets
 *
 *	RETURN VALUE: void
 */

MakeCreateGCPackets(tool)
Tool *tool;
{
	GCObj *gcop;
	char *packet;
	CARD32 *attrib;
	int count;
	int i;

	/* MakeFontPackets should have been executed before this one
	   because this function sets the font for the GC */
	for( gcop = tool->ToolInfo->GCList->Next; gcop != NULL;
	     gcop = gcop->Next ){
		packet = GetMem(sizeof(xCreateGCReq)+23*sizeof(CARD32));
		memcpy(packet,(char *) (gcop->Prefix),sizeof(xCreateGCReq));
		/* Do not set tile, stipple and clipmask */
		((xCreateGCReq *)packet)->mask = ((xCreateGCReq *)packet)->mask
						& 0xfff7f3ff;
		attrib = (CARD32 *) (packet+sizeof(xCreateGCReq));
		count = 0;
		for( i=0; i<23; i++)
			if( ((xCreateGCReq *) packet)->mask & 1<<i ){
				*attrib = *(gcop->Attrib+i);
				attrib++;
				count++;
			}
		((xCreateGCReq *)packet)->length = sizeof(xCreateGCReq)/4+count;
		AppendX11RequestToArchiveBuf3(tool,packet,
				sizeof(xCreateGCReq)+4*count);
		FreeMem(packet);
		if( gcop->SetClipRectanglesPacket != NULL )
			AppendX11RequestToArchiveBuf2(tool,gcop->SetClipRectanglesPacket);
		if( gcop->SetDashesPacket != NULL )
			AppendX11RequestToArchiveBuf2(tool,gcop->SetDashesPacket);
	}
}

/*
 *	CreateGCsForXTV
 *
 *	Creates packets for GCs used in PutImage packets for pixmap and window
 *
 *	RETURN VALUE: void
 */

CreateGCsForXTV(tool)
Tool *tool;
{
	char *buf;
	xCreateGCReq *req;
	CARD32 *attrib;
	CARD32 FindPixmapByDepth();
	CARD32 FindWindowByDepth();
	CARD32 idvalue;
	int i;

	/* select the ID so that new ID is the next highest number of the last
	   used one */
	idvalue = tool->ToolInfo->RecentResourceID + 1;
	for( i=1; i<65; i++ ){
		if( ((tool->ToolInfo->PAG)[i]).number > 0 ){
			buf = GetMem(sizeof(xCreateGCReq)+8);
			req = (xCreateGCReq *) buf;
			attrib = (CARD32 *)(buf+sizeof(xCreateGCReq));
			CleanUpStructure(buf,sizeof(xCreateGCReq)+8);
			req->reqType = X_CreateGC;
			req->length = 4;
			((tool->ToolInfo->PAG)[i]).gcid = ( req->gc= idvalue) ;
			idvalue++;
			if( (req->drawable = FindPixmapByDepth(tool,i)) == 0 ){
				if( (req->drawable = FindWindowByDepth(i,tool->ToolInfo->WindowList->Next))
				    == 0 ){
					fprintf(stderr,"CreateGCsForXTV: window or pixmap for depth %d not exist\n",i);
					exit(1);
				}
			}
			req->mask = 0x00000000;
			*(attrib+0) = 1;
			*(attrib+1) = 0;
			AppendX11RequestToArchiveBuf3(tool,buf,sizeof(xCreateGCReq)+0);
			FreeMem(buf);
		}
	}
}

/*
 *	MakeCreatePixmapPackets
 *
 *	Append CreatePixmap packets
 *
 *	RETURN VALUE: void
 */

MakeCreatePixmapPackets(tool)
Tool *tool;
{
	PixmapObj *pmop;
	xCreatePixmapReq req;

	if( tool->ToolInfo->PixmapList->Next == NULL ) return;

	/* find the last one */
	for( pmop = tool->ToolInfo->PixmapList->Next; pmop->Next != NULL;
	     pmop = pmop->Next );
	for( ; pmop != tool->ToolInfo->PixmapList; pmop = pmop->Prev ){
		req.reqType = X_CreatePixmap;
		req.depth = pmop->depth;
		req.length = 4;
		req.pid = pmop->pid;
		if( pmop->TailWindow != NULL )
			req.drawable = ((WindowObj *)(pmop->TailWindow))->Prefix->wid;
		else if( pmop->TailPixmap != NULL )
			req.drawable = pmop->TailPixmap->pid;
		else{
			fprintf(stderr,"MakeCreatePixmapPackets: drawable field in CreatePixmap empty\n");
			exit(1);
		}
		req.width = pmop->width;
		req.height = pmop->height;

		AppendX11RequestToArchiveBuf3(tool,(char *) &req,16);
	}
}

/*
 *	MakeChangeWindowAttribPackets
 *
 *	Append ChangeWindowAttributes packets for attributes using pixmap
 *	drawing
 *
 *	RETURN VALUE: void
 */

MakeChangeWindowAttribPackets(tool,ppnp)
Tool *tool;
PixmapPacketNode *ppnp;
{
	WindowObj *wop;
	PixmapPacketNodeHead *current;
	xChangeWindowAttributesReq *req;
	char *buf;
	CARD32 count;

	/* follow the singly linked list */
	for(current = ppnp->HeadWindow;current != NULL;current = current->Next){
		wop = (WindowObj *) (current->User);
		buf = GetMem(sizeof(xChangeWindowAttributesReq)+8);
		req = (xChangeWindowAttributesReq *) buf;
		req->reqType = X_ChangeWindowAttributes;
		req->window = wop->Prefix->wid;
		switch(current->AsWhat){
		    case 0: /* background */
			*((CARD32 *)(buf+sizeof(xChangeWindowAttributesReq)))
				= *(wop->Attrib+0);
			count = 1;
			req->valueMask = 0x00000001;
			if( wop->Prefix->mask & 1<<1 ){ /* pixel value also */
			 *((CARD32 *)(buf+sizeof(xChangeWindowAttributesReq)+4))
				= *(wop->Attrib+1);
			 count = 2;
			 req->valueMask = 0x00000003;
			}
			break;
		    case 1: /* border */
			*((CARD32 *)(buf+sizeof(xChangeWindowAttributesReq)))
				= *(wop->Attrib+2);
			count = 1;
			req->valueMask = 0x00000004;
			if( wop->Prefix->mask & 1<<3 ){ /* pixel value also */
			 *((CARD32 *)(buf+sizeof(xChangeWindowAttributesReq)+4))
				= *(wop->Attrib+3);
			 count = 2;
			 req->valueMask = 0x0000000c;
			}
			break;
		    default: /* error */
			fprintf(stderr,"MakeChangeWindowAttribPackets: Unexpected AsWhatin PixmapPacketNodeHead node\n");
			exit(1);
			break;
		}
		req->length = 3+count;
		AppendX11RequestToArchiveBuf3(tool,buf,
				sizeof(xChangeWindowAttributesReq)+4*count);
		FreeMem(buf);
	}
}

/*
 *	MakeChangeGCPackets
 *
 *	Append ChangeGC packets with attributes using pixmap drawings
 *
 *	RETURN VALUE: void
 */

MakeChangeGCPackets(tool,ppnp)
Tool *tool;
PixmapPacketNode *ppnp;
{
	GCObj *gcop;
	PixmapPacketNodeHead *current;
	xChangeGCReq *req;
	char *buf;

	for(current = ppnp->HeadGC; current != NULL; current = current->Next){
		gcop = (GCObj *) (current->User);
		buf = GetMem(sizeof(xChangeGCReq)+4);
		req = (xChangeGCReq *) buf;
		req->reqType = X_ChangeGC;
		req->length = 4;
		req->gc = gcop->gcid;
		switch(current->AsWhat){
		    case 0: /* tile */
			req->mask = 0x00000400;
			*((CARD32 *)(buf+sizeof(xChangeGCReq)))
				= *(gcop->Attrib+10);
			break;
		    case 1: /* stipple */
			req->mask = 0x00000800;
			*((CARD32 *)(buf+sizeof(xChangeGCReq)))
				= *(gcop->Attrib+11);
			break;
		    case 2: /* clip-mask */
			req->mask = 0x00080000;
			*((CARD32 *)(buf+sizeof(xChangeGCReq)))
				= *(gcop->Attrib+19);
			break;
		    default: /* error */
			printf("MakeChangeGCPackets: Unexpected AsWhat in PixmapPacketNodeHead node\n");
			exit(1);
			break;
		}
		AppendX11RequestToArchiveBuf3(tool,buf,sizeof(xChangeGCReq)+4);
		FreeMem(buf);
	}
}

/*
 *	GetGC
 *
 *	returns the correct GC ID for each different depth
 *
 *	RETURN VALUE: GC ID
 */

CARD32 GetGC(tool,depth)
Tool *tool;
int depth;
{
	return(((tool->ToolInfo->PAG)[depth]).gcid);
}

#define ProcessPixmapsForCursor(KIND)\
if( ppnhp->Next == NULL ){\
	/* If this is not at the end, it means you have to\
	go back further to execute some packets */\
	if( ppnp->Next != NULL ){\
		for( current = ppnp->Next; current->Next != NULL &&\
		     current->Next->HeadCursor == NULL;\
		     current = current->Next );\
		for( ; current != ppnp; current = current->Prev){\
			AppendX11PutImageRequestToArchiveBuf(tool,\
			  current->PutImagePacket,current->PutImagePacketSize,\
			  GetGC(tool,((PixmapObj *)(cop->KIND))->depth));\
			if( current->HeadWindow != NULL )\
				    MakeChangeWindowAttribPackets(tool,current);\
			if( current->HeadGC != NULL )\
				    MakeChangeGCPackets(tool,current);\
		}\
	}\
	AppendX11PutImageRequestToArchiveBuf(tool,ppnp->PutImagePacket,\
						ppnp->PutImagePacketSize,\
			GetGC(tool, ((PixmapObj *)(cop->KIND))->depth));\
	if( ppnp->HeadWindow != NULL )\
		MakeChangeWindowAttribPackets(tool,ppnp);\
	if( ppnp->HeadGC != NULL )\
		MakeChangeGCPackets(tool,ppnp);\
}

/*
 *	HandleCursorList
 *
 *	For each cursor created, append CreateCursor or CreateGlyphCursor
 *	packets in chronolgical way. Append ChangeWindowAttributes or
 *	ChangeGC packets for each different drawing along the way
 *
 *	RETURN VALUE: void
 */

HandleCursorList(tool)
Tool *tool;
{
	CursorObj *cop;
	PixmapPacketNodeHead *ppnhp;
	PixmapPacketNode *ppnp;
	PixmapPacketNode *current;

	if( tool->ToolInfo->CursorList->Next == NULL )
		return;
	/* Go to the last cursor */
	for(cop=tool->ToolInfo->CursorList->Next;cop->Next!=NULL;
	    cop=cop->Next);
	for(;cop != tool->ToolInfo->CursorList; cop = cop->Prev ){
		if( cop->pixmapcursor == 1 ){
			/* source */
			ppnp = (PixmapPacketNode *)(cop->Tailsource);
			for(ppnhp = ppnp->HeadCursor;ppnhp->User != (void *)cop;
			    ppnhp=ppnhp->Next);
			if( cop->Tailsource == cop->Tailmask )
				for(ppnhp=ppnhp->Next;ppnhp->User!=(void *)cop;
				    ppnhp=ppnhp->Next);
			ProcessPixmapsForCursor(TailsourcePixmap);
			/* mask */
			ppnp = (PixmapPacketNode *)(cop->Tailmask);
			for(ppnhp = ppnp->HeadCursor; ppnhp->User!=(void *)cop;
			    ppnhp=ppnhp->Next);
			ProcessPixmapsForCursor(TailmaskPixmap);
		}
		AppendX11RequestToArchiveBuf2(tool,cop->CreatePacket);
		if( cop->RecolorPacket != NULL )
			AppendX11RequestToArchiveBuf2(tool,cop->RecolorPacket);
	}
}

/*
 *	HandlePixmapList
 *
 *	Handles remaining pixmap drawings
 *
 *	RETURN VALUE: void
 */

HandlePixmapList(tool)
Tool *tool;
{
	PixmapObj *pmop;
	PixmapPacketNode *ppnp;

	for( pmop = tool->ToolInfo->PixmapList->Next; pmop != NULL;
	     pmop = pmop->Next ){
		if( pmop->PixmapPacketNodeList->Next != NULL &&
		    pmop->PixmapPacketNodeList->Next->HeadCursor == NULL ){
			/* find the last node such that it doesn't have
			   HeadCursor list */
			for( ppnp = pmop->PixmapPacketNodeList->Next;
			     ppnp->Next!=NULL && ppnp->Next->HeadCursor==NULL;
			     ppnp = ppnp->Next );
			for( ; ppnp != pmop->PixmapPacketNodeList;
			       ppnp = ppnp->Prev){
				AppendX11PutImageRequestToArchiveBuf(tool,
				 ppnp->PutImagePacket,ppnp->PutImagePacketSize,
				 GetGC(tool,pmop->depth));
				if( ppnp->HeadWindow != NULL )
				       MakeChangeWindowAttribPackets(tool,ppnp);
				if( ppnp->HeadGC != NULL )
					MakeChangeGCPackets(tool,ppnp);
			}
		}
	}
}

/*
 *	MakeCCChangeWindowAttribPackets
 *
 *	Append ChangeWindowAttributes packets now that all the pixmap drawings
 *	are in. While traversing the window tree, append other packets, too.
 *
 *	RETURN VALUE: void
 */

MakeCCChangeWindowAttribPackets(tool,wop)
Tool *tool;
WindowObj *wop;
{
	xChangeWindowAttributesReq *req;
	xResourceReq mapreq;
	xClearAreaReq careq;
	char *buf;
	CARD32 *attrib;
	WindowObj *current;
	int sofar;
	int count=0;

	buf = GetMem(sizeof(xChangeWindowAttributesReq)+28);
	req = (xChangeWindowAttributesReq *) buf;

	req->reqType = X_ChangeWindowAttributes;
	req->window = wop->Prefix->wid;
	req->valueMask = 0x00000000;

	attrib = (CARD32 *) (buf+sizeof(xChangeWindowAttributesReq));

	/* background pixmap */
	if( (wop->Prefix->mask & 1<<0) && *(wop->Attrib+0) < 2 ){
		req->valueMask |= 0x00000001;
		*attrib = *(wop->Attrib+0);
		attrib++;
		count++;
	}

	/* background pixel */
	if( ( wop->Prefix->mask & 1<<1 ) &&
	    ( !(wop->Prefix->mask & 1<<0) || 
	      ((wop->Prefix->mask & 1<<0) && *(wop->Attrib+0) < 2) ) ){
		req->valueMask |= 0x00000002;
		*attrib = *(wop->Attrib+1);
		attrib++;
		count++;
	}

	/* border pixmap */
	if( (wop->Prefix->mask & 1<<2) && *(wop->Attrib+2) < 1 ){
		req->valueMask |= 0x00000004;
		*attrib = *(wop->Attrib+2);
		attrib++;
		count++;
	}

	/* border pixel */
	if( ( wop->Prefix->mask & 1<<3 ) &&
	    ( !(wop->Prefix->mask & 1<<2) ||
	      ((wop->Prefix->mask & 1<<2) && *(wop->Attrib+2) < 1) ) ){
		req->valueMask |= 0x00000008;
		*attrib = *(wop->Attrib+3);
		attrib++;
		count++;
	}

	/* if backing store window image is available, xtv will be monitoring
	   exposure event. So if the exposure event is turned off by the client
	   turn it on temporarily so that xtv will get the exposure event */
	if( wop->PutImagePacket != NULL &&
	    ( !( wop->Prefix->mask & 1<<11) ||
	      ( (wop->Prefix->mask & 1<<11) && !(*(wop->Attrib+11) && 1<<15)))){
		if( !( wop->Prefix->mask & 1<<11) ){
			*(wop->Attrib+11) = 0;
		}
		req->valueMask |= 0x00000800;
		*attrib = *(wop->Attrib+11) | 1<<15;
		attrib++;
		count++;
	}

	/* colormap */
	if( (wop->Prefix->mask & 1<<13) && (*(wop->Attrib+13) > 0) ){
		req->valueMask |= 0x00002000;
		*attrib = *(wop->Attrib+13);
		attrib++;
		count++;
	}

	/* cursor */
	if( (wop->Prefix->mask & 1<<14) && (*(wop->Attrib+14) > 0) ){
		req->valueMask |= 0x00004000;
		*attrib = *(wop->Attrib+14);
		attrib++;
		count++;
	}

	/* Append the packet */
	req->length = sizeof(xChangeWindowAttributesReq)/4 + count;
	if( count > 0 ){
		AppendX11RequestToArchiveBuf3(tool,buf,
			sizeof(xChangeWindowAttributesReq)+4*count);
	}
	FreeMem(buf);

	/* Append the property packets */
	if( wop->PropertyPacketsSize > 0 ){
		sofar = 0;
		while( sofar < wop->PropertyPacketsSize ){
			AppendX11RequestToArchiveBuf2(tool,
						wop->PropertyPackets+sofar);
			sofar += (4+*((CARD32 *)(wop->PropertyPackets+sofar)));
		}
	}

	/* Append the grabbing request packets */
	if( wop->GrabRequestsSize > 0 ){
		sofar = 0;
		while( sofar < wop->GrabRequestsSize ){
			AppendX11RequestToArchiveBuf2(tool,
						wop->GrabRequests+sofar);
			sofar += (4+*((CARD32 *)(wop->GrabRequests+sofar)));
		}
	}

	/* map window */
	if( wop->State == 1 ){ /* mapped */
		mapreq.reqType = X_MapWindow;
		mapreq.length = 2;
		mapreq.id = wop->Prefix->wid;

		AppendX11RequestToArchiveBuf3(tool,(char *) &mapreq,8);
	}

	/* PutImage packet for backing store window */
	if( wop->PutImagePacket != NULL ){
		++(tool->SubDirtyCount); /* deceive for just a while */
		/* note that AppendX11PutImageRequestToArchiveBuf will append
		   the packet to Archive2Buf when anyone is tool->SubDirtyCount
		   is larger than 0 */
		AppendX11PutImageRequestToArchiveBuf(tool,
			wop->PutImagePacket,wop->PutImagePacketSize,
			GetGC(tool,wop->Prefix->depth));
		--(tool->SubDirtyCount);
	    	FreeMem(wop->PutImagePacket);
		wop->PutImagePacket = (void *) wop;
	}

	/* Call this function recursively for each child */
	if( wop->Children->Next != NULL ){
		for( current = wop->Children->Next; current->Next != NULL;
		     current = current->Next );
		for( ; current != wop->Children; current = current->Prev )
			MakeCCChangeWindowAttribPackets(tool,current);
	}

	/* Now that expose events may have been created, if the expose event
	   was disabled by the client, disable it */
	if( wop->PutImagePacket != NULL ){
	    wop->PutImagePacket = NULL;
	    if( !( wop->Prefix->mask & 1<<11) ||
	        ( (wop->Prefix->mask & 1<<11) && !(*(wop->Attrib+11) && 1<<15))){
		buf = GetMem(sizeof(xChangeWindowAttributesReq)+4);
		req = (xChangeWindowAttributesReq *) buf;
		req->reqType = X_ChangeWindowAttributes;
		req->length = 4;
		req->window = wop->Prefix->wid;
		req->valueMask = 0x00000800;
		*((CARD32 *) (buf+sizeof(xChangeWindowAttributesReq))) =
			*(wop->Attrib+11);
		++(tool->SubDirtyCount);
		AppendX11RequestToArchiveBuf3(tool,buf,16);
		--(tool->SubDirtyCount);
		FreeMem(buf);
	    }
	}
}

/*
 *	MakeInternAtomPackets
 *
 *	Append HS_InteranlAtom packets and InternAtom HS_X11Packet packets
 *
 *	RETURN VALUE: void
 */

MakeInternAtomPackets(tool)
Tool *tool;
{

	CARD16 HowMany; /* host's dataformat */
	CARD16 HowManyC; /* client's data format */
	CARD16 HowManyS; /* subscriber's data format */
	CARD32 TotalSize;
	CARD32 Current;
	char *buf;
	char *buf2;
	xInternAtomReq *req;
	

	Current = 0;
	while( Current < tool->ToolInfo->InternalAtomSize ){

	    /* HS_InternalAtom */
	    memcpy((char *) &TotalSize,tool->ToolInfo->InternalAtom+Current,4);
	    buf=GetMem(TotalSize);
	    memcpy(buf,tool->ToolInfo->InternalAtom+Current+4,TotalSize);
	    memcpy((char *) &HowMany,buf+7,2);
	    HowManyC = SwapInt(HowMany,tool->ConnectPrefix.byteOrder);
	    HowManyS=SwapInt(HowManyC,FROMLOCAL(tool->ConnectPrefix.byteOrder));
	    memcpy(buf+7,(char*)&HowManyS,2);

	    /* generating InternAtom packet */
	    buf2 = GetMem(sizeof(xInternAtomReq)+(int) pad(HowMany));
	    req = (xInternAtomReq *) buf2;
	    req->reqType = X_InternAtom;
	    req->onlyIfExists = buf[0];
	    req->length = (8+pad(HowMany))/4;
	    req->nbytes = HowManyC;
	    memcpy(buf2+sizeof(xInternAtomReq),buf+9,HowMany);

	    /* Append X11Packet InternAtom first */
	    AppendX11RequestToArchiveBuf3(tool,buf2,
					  sizeof(xInternAtomReq)+(int) pad(HowMany));
	    /* And then, InternalAtom xtv protocol packet */
	    AppendRequestToArchiveBuf(tool,buf+1,TotalSize-1);

	    FreeMem(buf);
	    FreeMem(buf2);
	    Current += ( TotalSize + 4 );
	}
}

/*
 *	MakeRemainderPackets
 *
 *	Append other remaining packets 
 *
 *	RETURN VALUE: void
 */

MakeRemainderPackets(tool)
Tool *tool;
{
	char buf[3];
	int i;
	xResourceReq FreeReq;
	CursorObj *cp;
	FontObj *fp;
	PixmapObj *pp;
	ColormapObj *cmp;
	WindowObj *wp;
	off_t sofar;

	/* SaveAll packets */
	if( tool->ToolInfo->SaveAllsSize > 0 ){
		for( sofar = 0; sofar<tool->ToolInfo->SaveAllsSize; ){
			AppendX11RequestToArchiveBuf2(tool,
				tool->ToolInfo->SaveAlls+sofar);
			sofar += (*((CARD32 *)(tool->ToolInfo->SaveAlls+sofar))+4);
		}
	}
	
	/* The following frees objects that have been explicitly freed by
	   the client, but have not been freed in xtv because there are
	   some objects depending on the freed objects */
	++(tool->SubDirtyCount); /* deceive for just a while */
	/* Free the GC packets for use within xtv */
	for( i=1; i<65; i++){
	    if( (FreeReq.id = GetGC(tool,i)) != 0 ){
		FreeReq.reqType = X_FreeGC;
		FreeReq.length = 2;
		AppendX11RequestToArchiveBuf3(tool,(char *) &FreeReq,8);
	    }
	}
	/* cursor */
	for( cp = tool->ToolInfo->CursorList->Next; cp != NULL; cp = cp->Next ){
		if( cp->State == 1 ){ /* freed */
			FreeReq.reqType = X_FreeCursor;
			FreeReq.length = 2;
			FreeReq.id = cp->cid;
			AppendX11RequestToArchiveBuf3(tool,(char *) &FreeReq,8);
		}
	}
	/* font */
	for( fp = tool->ToolInfo->FontList->Next; fp != NULL; fp = fp->Next ){
		if( fp->State == 1 ){ /* closed */
			FreeReq.reqType = X_CloseFont;
			FreeReq.length = 2;
			FreeReq.id = fp->fid;
			AppendX11RequestToArchiveBuf3(tool,(char *) &FreeReq,8);
		}
	}
	/* pixmap */
	for( pp = tool->ToolInfo->PixmapList->Next; pp != NULL; pp = pp->Next ){
		if( pp->State == 1 ){ /* freed */
			FreeReq.reqType = X_FreePixmap;
			FreeReq.length = 2;
			FreeReq.id = pp->pid;
			AppendX11RequestToArchiveBuf3(tool,(char *) &FreeReq,8);
		}
	}
	/* colormap */
	for( cmp = tool->ToolInfo->ColormapList->Next; cmp != NULL; cmp = cmp->Next ){
		if( cmp->State == 1 ){ /* freed */
			FreeReq.reqType = X_FreeColormap;
			FreeReq.length = 2;
			FreeReq.id = cmp->cid;
			AppendX11RequestToArchiveBuf3(tool,(char *) &FreeReq,8);
		}
	}
	/* window */
	for( wp = tool->ToolInfo->Destroyeds->Next; wp != NULL; wp = wp->Next ){
		FreeReq.reqType = X_DestroyWindow;
		FreeReq.length = 2;
		FreeReq.id = wp->Prefix->wid;
		AppendX11RequestToArchiveBuf3(tool,(char *) &FreeReq,8);
	}
	--(tool->SubDirtyCount);
}
