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

#include "objects.h"
#include "tool.h"

extern char *CombinePacketAndSize();
extern PixmapObj *MakePixmapDependency();
extern FontObj *MakeFontDependency();

/*
 *	MakeCursorDependency
 *
 *	Makes dependency on cursor
 *
 *	RETURN VALUE: pointer to the cursor object
 */

CursorObj *MakeCursorDependency(tool,cid)
Tool *tool;
CARD32 cid;
{
	CursorObj *cop, *FindCursorByID();

	if( (cop = FindCursorByID(cid,tool))
	    == NULL ){
		fprintf(stderr,"MakeCursorDependency: Can't find the cursor\n");
		exit(1);
	}
	if( cop->State == 1 ){
		fprintf(stderr,"Client referring to freed cursor\n");
		exit(1);
	}

	/* increase the number of windows depending on this cursor */
	++(cop->HeadWindow);
	return(cop);
}

/*
 *	RecFreeCursor
 *
 *	Frees the cursor and recursively frees other resources if necessary
 *
 *	RETURN VALUE: void
 */

RecFreeCursor(tool,cop)
Tool *tool;
CursorObj *cop;
{
	if( cop->State == 0 || /* Not freed */
	    cop->HeadWindow > 0 ) /* no window depends on this cursor */
		return;

	/* find whether this cursor was made out of pixmaps or fonts */
	if( cop->pixmapcursor == 1 ){ /* pixmap */
		RemoveDependencyOnPixmapDrawing(tool,(PixmapPacketNode *)
					      (cop->Tailsource),cop,
					      (PixmapObj *)
					      (cop->TailsourcePixmap));
		/* if mask exists, the dependency must also be deleted */
		if( ((xCreateCursorReq *) (cop->CreatePacket+4))->mask != 0 )
			RemoveDependencyOnPixmapDrawing(tool,(PixmapPacketNode*)
							(cop->Tailmask),cop,
							(PixmapObj *)
							(cop->TailmaskPixmap));
	}
	else{ /* font cursor */
		RemoveDependencyOnFont((FontObj *) (cop->Tailsource),1);
		/* check for the mask */
		if(((xCreateGlyphCursorReq *) (cop->CreatePacket+4))->mask != 0)
			RemoveDependencyOnFont((FontObj *) (cop->Tailmask),1);
	}
	FreeMem(cop->CreatePacket);
	if(cop->RecolorPacket != NULL)
		FreeMem(cop->RecolorPacket);
	RemoveANode(cop);
	FreeMem(cop);
}

/*
 *	RemoveDependencyOnCursor
 *
 *	Decreases the number of windows using this cursor and
 *	delete any other objects if necessary
 *
 *	RETURN VALUE: void
 */

RemoveDependencyOnCursor(tool,cop)
Tool *tool;
CursorObj *cop;
{
	--(cop->HeadWindow);
	RecFreeCursor(tool,cop);
}

/*
 *	FindCursorByID
 *
 *	Finds the cursor by its ID
 *
 *	RETURN VALUE: pointer to the found cursor
 */

CursorObj *FindCursorByID(cid,tool)
Cursor cid;
Tool *tool;
{
	CursorObj *Current;

	Current = tool->ToolInfo->CursorList->Next;
	while( Current != NULL ){
		if( Current->cid == cid )
			return(Current);
		Current = Current->Next;
	}
	return(NULL);
}

/*
 *	CreateCursorFunc
 *
 *	Processes CreateCursor request
 *
 *	RETURN VALUE: void
 */

void CreateCursorFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	CursorObj *cop, *FindCursorByID();
	xCreateCursorReq *req;

	/* check duplicity */
	req = (xCreateCursorReq *) packet;
	if( (cop = FindCursorByID(req->cid,tool)) != NULL ){
		fprintf(stderr,"CreateCursorFunc: duplicate cursor\n");
		exit(1);
	}

	/* register the id */
	tool->ToolInfo->RecentResourceID = req->cid;

	/* fill up some fields */
	cop = (CursorObj *) GetMem(sizeof(CursorObj));
	CleanUpStructure((char *) cop,sizeof(CursorObj));
	cop->cid = req->cid;
	cop->State = 0; /* Not freed */
	cop->HeadWindow = 0;
	cop->pixmapcursor = 1;
	cop->CreatePacket = CombinePacketAndSize(packet,size);
	cop->RecolorPacket = NULL;

	/* Making Pixmap dependency */
	cop->TailsourcePixmap =
	MakePixmapDependency(tool,req->source,cop,2,0,&(cop->Tailsource));
	if( req->mask != 0 )
		cop->TailmaskPixmap =
		MakePixmapDependency(tool,req->mask,cop,2,1,&(cop->Tailmask));
	else{
		cop->TailmaskPixmap = NULL;
		cop->Tailmask = NULL;
	}

	InsertNewNode(tool->ToolInfo->CursorList,cop);
}

/*
 *	CreateGlyphCursorFunc
 *
 *	Processes CreateGlyphCursor request
 *
 *	RETURN VALUE: void
 */

void CreateGlyphCursorFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	CursorObj *cop, *FindCursorByID();
	xCreateGlyphCursorReq *req;

	/* check for duplicity */
	req = (xCreateGlyphCursorReq *) packet;
	if( (cop = FindCursorByID(req->cid,tool)) != NULL ){
                fprintf(stderr,"CreateGlyphCursorFunc: duplicate cursor\n");
                exit(1);
        }

	/* register the id */
	tool->ToolInfo->RecentResourceID = req->cid;

	cop = (CursorObj *) GetMem(sizeof(CursorObj));
	CleanUpStructure((char *) cop,sizeof(CursorObj));
	cop->cid = req->cid;
        cop->State = 0; /* Not freed */
        cop->HeadWindow = 0;
        cop->pixmapcursor = 0;
	cop->CreatePacket = CombinePacketAndSize(packet,size);
        cop->RecolorPacket = NULL;

	/* Making font dependency */
	cop->Tailsource =
	MakeFontDependency(tool,req->source,1);
	if( req->mask != 0 )
		cop->Tailmask =
		MakeFontDependency(tool,req->mask,1);
	else
		cop->Tailmask = NULL;

	InsertNewNode(tool->ToolInfo->CursorList,cop);
}

/*
 *	RecolorCursorFunc
 *
 *	Handles RecolorCursor request
 *
 *	RETURN VALUE: void
 */

void RecolorCursorFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	CursorObj *cop, *FindCursorByID();
	xRecolorCursorReq *req;

	req = (xRecolorCursorReq *) (packet);
	if( (cop = FindCursorByID(req->cursor,tool)) == NULL ){
                fprintf(stderr,"RecolorCursorFunc: Can't find the cursor\n");
		exit(1);
	}
	if( cop->State == 1 ){
		fprintf(stderr,"Client referring to freed cursor\n");
		exit(1);
	}

	/* find if there already is a packet */
	if ( cop->RecolorPacket == NULL )
		cop->RecolorPacket = GetMem(sizeof(size)+size);
	/* copy */
	memcpy(cop->RecolorPacket,(char *) &size,sizeof(size));
	memcpy(cop->RecolorPacket+sizeof(size),packet,size);
}

/*
 *	FreeCursorFunc
 *
 *	Handles FreeCursor request
 *
 *	RETURN VALUE: void
 */
void FreeCursorFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	CursorObj *cop, *FindCursorByID();
	xResourceReq *req;

	req = (xResourceReq *) packet;
	if( (cop = FindCursorByID(req->id,tool)) == NULL ){
		fprintf(stderr,"FreeCursorFunc: Can't find the cursor\n");
		exit(1);
	}
	if( cop->State == 1 ){
		fprintf(stderr,"Client referring to freed cursor\n");
		exit(1);
	}

	cop->State = 1;

	RecFreeCursor(tool,cop);
}
