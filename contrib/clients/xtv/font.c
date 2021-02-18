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

/*
 *	MakeFontDependency
 *
 *	Makes dependency on font
 *
 *	RETURN VALUE: pointer to the font object
 */

FontObj *MakeFontDependency(tool,fid,csr)
Tool *tool;
CARD32 fid;
int csr;	/* 0 - GC, 1 - cursor */
{
	FontObj *fop, *FindFontByID();

	if( (fop = FindFontByID(fid,tool))
		== NULL ){
		fprintf(stderr,"MakeFontDependency: Can't find the font\n");
		return(NULL);
	}
	if( fop->State == 1 ){
		fprintf(stderr,"Client referring to a freed font\n");
		exit(1);
	}
	if( csr )
		++(fop->HeadCursor);
	else
		++(fop->HeadGC);
	return(fop);
}

/*
 *	RemoveDependencyOnFont
 *
 *	Removes dependency on font
 *
 *	RETURN VALUE: void
 */

RemoveDependencyOnFont(fop,crs)
FontObj *fop;
int	crs;
{
	if( fop == NULL ) return;
	if( crs )
		--(fop->HeadCursor);
	else
		--(fop->HeadGC);
	RecCloseFont(fop);
}

/*
 *	RecCloseFont
 *
 *	Frees the structure for the font if possible
 *
 *	RETURN VALUE: void
 */

RecCloseFont(fop)
FontObj *fop;
{
	if( fop->State == 0 || /* opened */
	    fop->HeadCursor > 0 || /* no cursor using this font */
	    fop->HeadGC > 0 ) /* no GC using this font */
		return;
	if( fop->SetPathPacket != NULL ){
		if( fop->Prev->Prev == NULL ){/* This is the last font opened */
			FreeMem(fop->OpenPacket);
			fop->OpenPacket = NULL; /* just free the packet */
			fop->fid = 0;
			fop->State = 0;
			return;	/* leave the SetPathPacket */
		}
		else if( fop->Prev->SetPathPacket != NULL )
			FreeMem(fop->SetPathPacket);
			/* Next font uses different path */
		else
			fop->Prev->SetPathPacket = fop->SetPathPacket;
			/* moving the packet to the next FontObj node */
	}
	FreeMem(fop->OpenPacket);
	RemoveANode(fop);
	FreeMem(fop);
}

/*
 *	FindFontByID
 *
 *	Finds the font by ID
 *
 *	RETURN VALUE: pointer to the found font
 */

FontObj *FindFontByID(fid,tool)
Font fid;
Tool *tool;
{
	FontObj *Current;

	Current = tool->ToolInfo->FontList->Next;
        while( Current != NULL ){
                if( Current->fid == fid )
			return(Current);
		Current = Current->Next;
	}
	return(NULL);
}

/*
 *	OpenFontFunc
 *
 *	Processes OpenFont request
 *
 *	RETURN VALUE: void
 */

void OpenFontFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	FontObj *fop, *FindFontByID();
	xOpenFontReq *req;

	/* check for duplicity */
	req = (xOpenFontReq *) packet;
	if( (fop = FindFontByID(req->fid,tool)) != NULL ){
		fprintf(stderr,"OpenFontFunc:Duplicate font\n");
		exit(1);
	}

	/* check if the first entry doesn't have OpenFont request */
	if( tool->ToolInfo->FontList->Next != NULL &&
	    tool->ToolInfo->FontList->Next->OpenPacket == NULL ){
		fop = tool->ToolInfo->FontList->Next;
		if( fop->SetPathPacket == NULL ){
			fprintf(stderr,"OpenFontFunc: List screwed up\n");
			exit(1);
		}
	}
	else{	/* Create a new node */
		fop = (FontObj *) GetMem(sizeof(FontObj));
		CleanUpStructure((char *) fop,sizeof(FontObj));
		fop->SetPathPacket = NULL;
		InsertNewNode(tool->ToolInfo->FontList,fop);
	}

	/* register the current id */
	tool->ToolInfo->RecentResourceID = req->fid;

	fop->fid = req->fid;
	fop->State = 0;
	fop->HeadCursor = 0;
	fop->HeadGC = 0;
	fop->OpenPacket = CombinePacketAndSize(packet,size);
}

/*
 *	CloseFontFunc
 *
 *	Processes CloseFont request
 *
 *	RETURN VALUE: void
 */

void CloseFontFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	FontObj *fop, *FindFontByID();

	if( (fop = FindFontByID(((xResourceReq *) packet)->id,tool)) == NULL ){
		fprintf(stderr,"CloseFontFunc: Can't find the font\n");
		exit(1);
	}
	if( fop->State == 1 ){
		fprintf(stderr,"Client referring to a freed font\n");
		exit(1);
	}

	fop->State = 1;

	RecCloseFont(fop);
}

/*
 *	SetFontPathFunc
 *
 *	Processes SetFontPath request
 *
 *	RETURN VALUE: void
 */

void SetFontPathFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	FontObj *fop;

	/* find if the last node contains only SetPathPacket */
	if( tool->ToolInfo->FontList->Next != NULL &&
	    tool->ToolInfo->FontList->Next->OpenPacket == NULL ){
		fop = tool->ToolInfo->FontList->Next;
		if( fop->SetPathPacket == NULL ){
			fprintf(stderr,"SetFontPathFunc: List screwed up\n");
			exit(1);
		}
		/* free it */
		FreeMem(fop->SetPathPacket);
	}
	else{ /* last node contains OpenPacket, create a new one */
		fop = (FontObj *) GetMem(sizeof(FontObj));
		CleanUpStructure((char *) fop,sizeof(FontObj));
		fop->fid = 0;
		fop->State = 0;
		fop->HeadCursor = 0;
		fop->HeadGC = 0;
		fop->OpenPacket = NULL;
		InsertNewNode(tool->ToolInfo->FontList,fop);
	}

	fop->SetPathPacket = CombinePacketAndSize(packet,size);
}
