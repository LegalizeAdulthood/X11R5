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
#include "objects.h"
#include "tool.h"

extern int ReportToolStat();
extern WindowObj *MakeWindowDependency();

/*
 *	FindPixmapByDepth
 *
 *	Finds a pixmap with a depth given as a parameter
 *
 *	RETURN VALUE: pixmap ID
 */

CARD32 FindPixmapByDepth(tool,depth)
Tool *tool;
CARD32 depth;
{
	PixmapObj *Current;

	for(Current = tool->ToolInfo->PixmapList->Next;
	    Current != NULL; Current = Current->Next){
		if( Current->depth == depth )
			return(Current->pid);
	}
	return(0);
}

/*
 *	FindPixmapByID
 *
 *	Finds a pixmap by ID
 *
 *	RETURN VALUE: pointer to the found pixmap
 */

PixmapObj *FindPixmapByID(pid,tool)
Pixmap pid;
Tool *tool;
{
	PixmapObj *Current;

	Current = tool->ToolInfo->PixmapList->Next;
        while( Current != NULL ){
                if( Current->pid == pid )
			return(Current);
		Current = Current->Next;
        }
        return(NULL);
}

/*
 *	MakePixmapDependency2
 *
 *	Makes dependency on pixmap of another pixmap or a GC
 *
 *	RETURN VALUE: pointer to the pixmap object
 */

PixmapObj *MakePixmapDependency2(tool,pid,whose)
Tool *tool;
CARD32 pid;
int whose;	/* 0 - pixmap, 1 - GC */
{
	PixmapObj *pmop;

	if( (pmop = FindPixmapByID(pid,tool)) == NULL ){
		fprintf(stderr,"MakePixmapDependency2: can't find pixmap %d\n",pid);
		exit(1);
	}
	if( pmop->State == 1 ){
		fprintf(stderr,"Client referring to a freed pixmap\n");
		exit(1);
	}
	switch(whose){
		case 0: ++pmop->HeadPixmap;
			break;
		case 1: ++pmop->HeadGC;
			break;
		default: fprintf(stderr,"MakePixmapDependency2: unexpected argument value\n");
			exit(1);
			break;
	}
	return(pmop);
}

/*
 *	RemoveDependencyOnPixmap2
 *
 *	Removes dependency created by MakePixmapDependency2
 *
 *	RETURN VALUE: void
 */

RemoveDependencyOnPixmap2(tool,pmop,whose)
Tool *tool;
PixmapObj *pmop;
int whose;
{
	switch(whose){
		case 0: --pmop->HeadPixmap;
			break;
		case 1: --pmop->HeadGC;
			break;
		default: fprintf(stderr,"RemoveDependencyOnPixmap2: unexpected argument value\n");
			exit(1);
			break;
	}
	RecFreePixmap(tool,pmop);
}

/*
 *	MakePixmapDependency
 *
 *	Makes dependency on the pixmap drawing of window, GC or cursor
 *
 *	RETURN VALUE: pointer to the pixmap object
 */

PixmapObj *MakePixmapDependency(tool,pid,op,object,aswhat,ppnpp)
Tool *tool;
CARD32 pid;
void *op;
int object;	/* 0 - window, 1 - GC, 2 - cursor */
int aswhat;	/* window: 0 - background, 1 - border */
		/* GC:	   0 - tile, 1 - stipple, 2 - clip-mask */
 		/* cursor: 0 - source, 1 - mask */
PixmapPacketNode **ppnpp; /* return */
{
	PixmapObj *pmop;
	PixmapPacketNodeHead *ppnhp;

	if( (pmop = FindPixmapByID(pid,tool)) == NULL ){
		fprintf(stderr,"MakePixmapDependency: can't find pixmap %d\n",pid);
		exit(1);
	}
	if( pmop->State == 1 ){
		fprintf(stderr,"Client referring to a freed pixmap\n");
		exit(1);
	}

	if( pmop->dirty || /* The pixmap has been written to since last visit */
	    pmop->PixmapPacketNodeList->Next == NULL )
		MakeANewPixmapPacketNode(tool,pmop);
	/* Get the last node */
	*ppnpp = pmop->PixmapPacketNodeList->Next;

	ppnhp = (PixmapPacketNodeHead *) GetMem(sizeof(PixmapPacketNodeHead));
	CleanUpStructure((char *) ppnhp,sizeof(PixmapPacketNodeHead));

	ppnhp->User = op;
	ppnhp->AsWhat = aswhat;

	/* put the new node to the front of the singly linked list */
	switch(object){
		case 0: ppnhp->Next = (*ppnpp)->HeadWindow;
			(*ppnpp)->HeadWindow = ppnhp;
			break;
		case 1: ppnhp->Next = (*ppnpp)->HeadGC;
			(*ppnpp)->HeadGC = ppnhp;
			break;
		case 2: ppnhp->Next = (*ppnpp)->HeadCursor;
			(*ppnpp)->HeadCursor = ppnhp;
			break;
		default: fprintf(stderr,"MakePixmapDependency: wrong object number\n");
			exit(1);
	}
	return(pmop);
}

/*
 *	MakeANewPixmapPacketNode
 *
 *	Insert a new PixmapPacketNode to the front of the list
 *
 *	RETURN VALUE: void
 */

MakeANewPixmapPacketNode(tool,pmop)
Tool *tool;
PixmapObj *pmop;
{
	PixmapPacketNode *ppnp;

	pmop->dirty = 0;
	/* Get a New PixmapPacketNode */
	ppnp = ( PixmapPacketNode * ) GetMem(sizeof(PixmapPacketNode));
	CleanUpStructure((char *) ppnp,sizeof(PixmapPacketNode));
	InsertNewNode(pmop->PixmapPacketNodeList,ppnp);
}

/*
 *	RecFreePixmap
 *
 *	Free the structure for the pixmap and other objects recursively if
 *	possible
 *
 *	RETURN VALUE: void
 */

RecFreePixmap(tool,pmop)
Tool *tool;
PixmapObj *pmop;
{
	if( pmop->State == 1 && /* freed */
	    /* no pixmap, GC depending on this pixmap */
	    pmop->HeadPixmap == 0 && pmop->HeadGC == 0 &&
	    /* no object depending on a drawing of the pixmap */
	    pmop->PixmapPacketNodeList->Next == NULL ){
		if( pmop->TailWindow != NULL )
		    RemoveDependencyOnWindow((WindowObj *)(pmop->TailWindow),2);
		else
		    RemoveDependencyOnPixmap2(tool,pmop->TailPixmap,0);
		/* decrease the number of pixmaps of the depth */
		--(((tool->ToolInfo->PAG)[pmop->depth]).number);
		FreeMem(pmop->PixmapPacketNodeList);
		RemoveANode(pmop);
		FreeMem(pmop);
	}
}

#define subREIPPN(substitute)\
	for(Previous = (Current = ppnp->substitute);\
	    Found == 0 && Current != NULL;){\
		if(p==Current->User){\
			Found = 1;\
			break;\
		}\
		Previous = Current;\
		Current = Current->Next;\
	}\
	if( Found == 1 ){\
		if( Previous == Current )\
			ppnp->substitute = Current->Next;\
		else\
			Previous->Next = Current->Next;\
		FreeMem(Current);\
		break;\
	}

/*
 *	RemoveDependencyOnPixmapDrawing
 *
 *	removes dependency on a pixmap drawing
 *
 *	RETURN VALUE: void
 */

RemoveDependencyOnPixmapDrawing(tool,ppnp,p,pmop)
Tool *tool;
PixmapPacketNode *ppnp;
void *p;
PixmapObj *pmop;
{
	char Found = 0;
	PixmapPacketNodeHead *Current;
	PixmapPacketNodeHead *Previous;

	do{
		/* find the match in the three singly linked lists and free
		   the node found */
		subREIPPN(HeadWindow)
		subREIPPN(HeadGC)
		subREIPPN(HeadCursor)
		fprintf("RemoveDependencyOnPixmapDrawing: Something wrong\n");
		exit(1);
	}while( 0 );
	if( (pmop->dirty == 1 || /* the drawing is not up-to-date */
	     ppnp->Prev != pmop->PixmapPacketNodeList || /* the drawing is not
							    the last one */
	     ppnp->PutImagePacket == NULL) && /* no drawing gotten yet */
	    ppnp->HeadWindow == NULL && /* no object depends on the drawing */
	    ppnp->HeadGC == NULL &&
	    ppnp->HeadCursor == NULL ){
		/* then free the node */
		if( ppnp->PutImagePacket != NULL )
			FreeMem(ppnp->PutImagePacket);
		RemoveANode(ppnp);
		FreeMem(ppnp);
		if( pmop->PixmapPacketNodeList->Next == NULL )
			pmop->dirty = 1;
		RecFreePixmap(tool,pmop);
	}
}

/*
 *	CreatePixmapFunc
 *
 *	Processes CreatePixmap request
 *
 *	RETURN VALUE: void
 */

void CreatePixmapFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	PixmapObj *pmop;
	xCreatePixmapReq *req;

	req = (xCreatePixmapReq *) packet;
	if( (pmop = FindPixmapByID(req->pid,tool)) != NULL ){
		fprintf(stderr,"CreatePixmapFunc: duplicate pixmap\n");
		exit(1);
	}

	/* register the current wid */
	tool->ToolInfo->RecentResourceID = req->pid;

	pmop = (PixmapObj *) GetMem(sizeof(PixmapObj));
	CleanUpStructure((char *) pmop,sizeof(PixmapObj));
	pmop->pid = req->pid;
	pmop->State = 0;

	/* make dependency for the drawable this pixmap depends on */
	if( (pmop->TailWindow =
		(void *) MakeWindowDependency(tool,req->drawable,2))
		== NULL ){
		pmop->TailPixmap = MakePixmapDependency2(tool,req->drawable,0);
	}
	else{
		pmop->TailPixmap = NULL;
	}

	pmop->PixmapPacketNodeList
		= (PixmapPacketNode *) GetMem(sizeof(PixmapPacketNode));
	CleanUpStructure((char *) (pmop->PixmapPacketNodeList),
			 sizeof(PixmapPacketNode));
	pmop->depth = req->depth;
	pmop->width = req->width;
	pmop->height = req->height;
	pmop->dirty = 0;

	++(((tool->ToolInfo->PAG)[pmop->depth]).number);

	InsertNewNode(tool->ToolInfo->PixmapList,pmop);
}

/*
 *	FreePixmapFunc
 *
 *	Frees the structure for the pixmap and any recursive objects
 *
 *	RETURN VALUE: void
 */

void FreePixmapFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	PixmapObj *pmop;
	PixmapPacketNode *ppnp;
	extern char *CreatePutImagePackets();

	if( ( pmop = FindPixmapByID( ((xResourceReq *) packet)->id,tool) )
		== NULL ){
		fprintf(stderr,"FreePixmapFunc: Can't find the pixmap\n");
		exit(1);
	}
	if( pmop->State == 1 ){
		fprintf(stderr,"Client referring to a freed pixmap\n");
		exit(1);
	}
	/* See if there's any dependencies on this pixmap drawings */
	if( pmop->PixmapPacketNodeList->Next != NULL ){
	    /* See if the drawing has not been gotten yet */
	    if( pmop->PixmapPacketNodeList->Next->PutImagePacket == NULL ){
		if( pmop->PixmapPacketNodeList->Next->HeadWindow != NULL ||
		    pmop->PixmapPacketNodeList->Next->HeadGC != NULL ||
		    pmop->PixmapPacketNodeList->Next->HeadCursor != NULL ){
		    pmop->PixmapPacketNodeList->Next->PutImagePacket =
		  CreatePutImagePackets(tool,pmop->pid,pmop->height,pmop->width,
		  &(pmop->PixmapPacketNodeList->Next->PutImagePacketSize));
		}else{
		    fprintf(stderr,"FreePixmapFunc: Strange state\n");
		    exit(1);
		}
	    }
	    /* if drawing remains just in case for a new dependency,delete it */
	    else{ /* pmop->PixmapPacketNodeList->Next->PutImagePacket != NULL */
		if( pmop->PixmapPacketNodeList->Next->HeadWindow == NULL &&
		    pmop->PixmapPacketNodeList->Next->HeadGC == NULL &&
		    pmop->PixmapPacketNodeList->Next->HeadCursor == NULL ){
		    ppnp = pmop->PixmapPacketNodeList->Next;
		    FreeMem(ppnp->PutImagePacket);
		    RemoveANode(ppnp);
		    FreeMem(ppnp);
		    pmop->PixmapPacketNodeList->Next = NULL;
		}
	    }
	}
	/* check if the pixmap was drawn on even when it is not used for drawing */
	if( pmop->dirty == 1 ){
		MakeANewPixmapPacketNode(tool,pmop);
		pmop->PixmapPacketNodeList->Next->PutImagePacket =
		CreatePutImagePackets(tool,pmop->pid,pmop->height,
		pmop->width,&(pmop->PixmapPacketNodeList->Next->PutImagePacketSize));
	}

	pmop->State = 1;
	RecFreePixmap(tool,pmop);
}
