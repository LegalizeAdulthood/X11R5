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
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include "objects.h"
#include "tool.h"

extern char *CombinePacketAndSize();
extern PixmapObj *FindPixmapByID();
extern WindowObj *FindWindowByID();
extern CursorObj *FindCursorByID();
extern XtInputCallbackProc HandleIncomingServerPacket();

/*
 *	SaveAllFunc
 *
 *	This function saves all packets that are not related to the objects
 *	or not very significant in dependency relationship
 *
 *	RETURN VALUE: void
 */

void SaveAllFunc(tool,packet,size)
Tool *tool;
char *packet;
int size;
{
	tool->ToolInfo->SaveAlls = realloc(tool->ToolInfo->SaveAlls,
					   tool->ToolInfo->SaveAllsSize+
					   sizeof(size)+size);
	memcpy(tool->ToolInfo->SaveAlls+tool->ToolInfo->SaveAllsSize,
		(char *) &size, sizeof(size));
	memcpy(tool->ToolInfo->SaveAlls+tool->ToolInfo->SaveAllsSize+sizeof(size),
		packet,size);
	tool->ToolInfo->SaveAllsSize += (sizeof(size)+size);
}

/*
 *	PropertyFunc
 *
 *	Saves all property related packets in the proper window object
 *	structure in chronological order
 *
 *	RETURN VALUE: void
 */

void PropertyFunc(tool,packet,size)
Tool *tool;
char *packet;
int size;
{
	WindowObj *wop;

	if( (wop = FindWindowByID( ((xResourceReq *) packet)->id,
				    tool->ToolInfo->WindowList->Next ))
		== NULL ){
		fprintf(stderr,"PropertyFunc: Can't find the window\n");
		return;
	}

	wop->PropertyPackets = realloc(wop->PropertyPackets,
			wop->PropertyPacketsSize+sizeof(size)+size);
	memcpy(wop->PropertyPackets+wop->PropertyPacketsSize,
		(char *) &size,sizeof(size));
	memcpy(wop->PropertyPackets+wop->PropertyPacketsSize+
		sizeof(size),packet,size);
	wop->PropertyPacketsSize += (sizeof(size)+size);
}

/*
 *	DrawingFunc
 *
 *	Check if the drawing is to a pixmap and if so, mark the pixmap as dirty
 *	and get the image of the pixmap if necessary
 *
 *	RETURN VALUE: void
 */

void DrawingFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	xResourceReq *req;
	Pixmap	     pid;
	PixmapObj    *pmop;
	PixmapPacketNode *temp;
	extern char *CreatePutImagePackets();

	req = ( xResourceReq * ) packet;
	pid = req->id;
	if( req->reqType == X_CopyArea || req->reqType == X_CopyPlane )
		pid = *((CARD32 *) (packet+sizeof(xResourceReq)));

	/* Only pixmap applies */
	if( (pmop = FindPixmapByID(pid,tool)) != NULL ){ 
		if( pmop->State == 1 ){
			fprintf(stderr,"Client trying to refer to freed pixmap\n");
			exit(1);
		}
		if( pmop->PixmapPacketNodeList->Next == NULL ){
			/* No object uses this pixmap's drawing */
			pmop->dirty = 1;
		}
		else if( pmop->PixmapPacketNodeList->Next->PutImagePacket
			 != NULL ){
			/* Previously gotten image remains, but not freed
			   just in case, but now it has to be freed if
			   no object uses the drawing */
			pmop->dirty = 1;
			if( pmop->PixmapPacketNodeList->Next->HeadWindow == NULL
			    && pmop->PixmapPacketNodeList->Next->HeadGC == NULL
			    && pmop->PixmapPacketNodeList->Next->HeadCursor ==
			    NULL ){
				FreeMem(pmop->PixmapPacketNodeList->Next->PutImagePacket);
				temp = pmop->PixmapPacketNodeList->Next;
				RemoveANode(temp);
				FreeMem(temp);
			}
		}
		else{
			/* Some object uses the drawing of the pixmap
			   so better get the drawing before it's too late */
			pmop->PixmapPacketNodeList->Next->PutImagePacket =
			CreatePutImagePackets(tool,pmop->pid,pmop->height,pmop->width,
			&(pmop->PixmapPacketNodeList->Next->PutImagePacketSize));
		}
	}
}

/*
 *	GrabFunc
 *
 *	Handle GrabButton and GrabKey requests and also Ungrab of the two
 *	For now, those requests are all saved in chronological order in the
 *	proper window object
 *
 *	RETURN VALUE: void
 */

void GrabFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	xResourceReq *req;
	xGrabButtonReq *gbr;
	CursorObj *cop;
	WindowObj *wop;

	req = (xResourceReq *) packet;
	if( (wop = FindWindowByID(req->id,tool->ToolInfo->WindowList->Next)) == NULL ){
		fprintf(stderr,"GrabFunc: Can't find the window\n");
		exit(1);
	}
	if( req->reqType == X_GrabButton ){
		gbr = (xGrabButtonReq *) packet;
		if( gbr->cursor != 0 ){
			if( (cop = FindCursorByID(gbr->cursor,tool)) == NULL ){
				fprintf(stderr,"GrabFunc: Can't find the cursor for GrabButton request\n");
				exit(1);
			}
			++(cop->HeadWindow);
		}
	}
	wop->GrabRequests = realloc(wop->GrabRequests,
				    wop->GrabRequestsSize+4+size);
	memcpy(wop->GrabRequests+wop->GrabRequestsSize,(char *) &size,4);
	memcpy(wop->GrabRequests+wop->GrabRequestsSize+4,packet,size);
	wop->GrabRequestsSize += ( 4 + size );
}
