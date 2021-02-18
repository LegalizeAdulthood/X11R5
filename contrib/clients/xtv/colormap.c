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

#include "tool.h"
#include "objects.h"

extern char *CombinePacketAndSize();

/*
 *	RemoveDependencyOnColormap
 *
 *	Decreases the number of windows depending on the colormap and
 *	call routine to recursively free structures for colormaps if necessary
 *
 *	RETURN VALUE: void
 */
 
RemoveDependencyOnColormap(cop)
ColormapObj *cop;
{
	--(cop->HeadWindow);
	RecFreeColormap(cop);
}

/*
 *	RecFreeColormap
 *
 *	Free the data structure for the colormap if there's no object depending
 * 	on it.
 *
 *	RETURN VALUE: void
 */

RecFreeColormap(cop)
ColormapObj *cop;
{
	WindowObj *wop;

	if( cop->State == 0 || /* Not freed */
	    cop->HeadColormap->Next != NULL || /* no cmap depending on this */
	    cop->HeadWindow > 0 ) /* No window using this colormap */
		return;
	if( cop->TailColormap != NULL ){
	/* This colormap was created using CopyColormapAndFree */
		RemoveANode(cop->TailColormap);
		FreeMem(cop->TailColormap);
		if( cop->TailColormapObj == NULL ){
			fprintf(stderr,"TailColormapObj not existent\n");
			exit(1);
		}
		RecFreeColormap(cop->TailColormapObj);
	}
	else{
	/* This colormap was created using CreateColormap */
		RemoveDependencyOnWindow((WindowObj *)(cop->TailWindow));
	}
	FreeMem(cop->HeadColormap);
	if( cop->Packets != NULL )
		FreeMem(cop->Packets);
	if( cop->AllocPackets != NULL )
		FreeMem(cop->AllocPackets);
	RemoveANode(cop);
	FreeMem(cop);
}

/*
 *	MakeColormapDependency
 *
 *	Increases the number of windows depending on this colormap and return
 *
 *	RETURN VALUE: pointer to the colormap object
 */

ColormapObj *MakeColormapDependency(tool,cid)
Tool *tool;
CARD32 cid;
{
        ColormapObj *cop, *FindColormapByID();

        if( (cop = FindColormapByID(cid,tool))
	    == NULL ){
		fprintf(stderr,"MakeColormapDependency: Can't find the colormap\n");
		exit(1);
	}
	if( cop->State == 1 ){
		fprintf(stderr,"Client referring to freed colormap\n");
		exit(1);
	}
	++(cop->HeadWindow);
	return(cop);
}

/*
 *	FindColormapByID
 *
 *	Finds a colormap using the colormap ID given
 *
 *	RETURN VALUE: pointer to the colormap object found
 */

ColormapObj *FindColormapByID(cid,tool)
Colormap cid;
Tool *tool;
{
	ColormapObj *Current;

        Current = tool->ToolInfo->ColormapList->Next;
        while( Current != NULL ){
                if( Current->cid == cid )
			return(Current);
		Current = Current->Next;
        }
        return(NULL);
}

/*
 *	SimpleColormapFunc
 *
 *	Simply appends the request packet to the colormap object structure
 *
 *	RETURN VALUE: void
 */

void SimpleColormapFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	ColormapObj *cop, *FindColormapByID();
	Colormap     cid;
	char	     *p;

	cid = (( xResourceReq * ) ( packet ))->id;
	if( (cop = FindColormapByID(cid,tool)) == NULL ){
		fprintf(stderr,"A Unknown colormap specified\n");
		exit(1);
	}
	if( cop->State == 1 ){
		fprintf(stderr,"Client referring to freed colormap\n");
		exit(1);
	}
	cop->Packets = realloc(cop->Packets,(cop->PacketsSize+size+sizeof(size)));
	p = CombinePacketAndSize(packet,size);
	memcpy(cop->Packets+cop->PacketsSize,p,size+sizeof(size));
	FreeMem(p);
	cop->PacketsSize += (size+sizeof(size));
}

/*
 *	CreateNewColormap
 *
 *	Creates a structure for a colormap object
 *
 *	RETURN VALUE: pointer to the created colormap object
 */

ColormapObj *CreateNewColormap(tool,cid)
Tool *tool;
Colormap cid;
{
	ColormapObj *cop;

	cop = ( ColormapObj * ) GetMem(sizeof(ColormapObj));
	CleanUpStructure((char *) cop,sizeof(ColormapObj));

	cop->cid = cid;

	cop->HeadColormap = (ColormapHeadNode *) GetMem(sizeof(ColormapHeadNode));
	CleanUpStructure((char *) (cop->HeadColormap),sizeof(ColormapHeadNode));

	InsertNewNode(tool->ToolInfo->ColormapList,cop);

	return(cop);
}

/*
 *	CopyColormapAndFreeFunc
 *
 *	Process CopyColormapAndFree Request
 *
 *	RETURN VALUE: void
 */

void CopyColormapAndFreeFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
        ColormapObj *copmid, *copsrc;
	ColormapObj *FindColormapByID(), *CreateNewColormap();
        Colormap     cidmid;
	Colormap     cidsrc;
	ColormapHeadNode *newcmapheadnode;

	/* target */
        cidmid = (( xResourceReq * ) ( packet ))->id;
        if( (copmid = FindColormapByID(cidmid,tool)) != NULL ){
		fprintf(stderr,"dest colormap already exists: CopyColormapFree\n");
		exit(1);
	}

	/* source */
	cidsrc = (( xCopyColormapAndFreeReq *) (packet))->srcCmap;
	if( (copsrc = FindColormapByID(cidsrc,tool)) == NULL ){
		fprintf(stderr,"Source cmap not found in CopyColormapFree\n");
		exit(1);
	}
	if( copsrc->State == 1 ){
		fprintf(stderr,"Client referring to freed colormap\n");
		exit(1);
	}

	/* register the id */
	tool->ToolInfo->RecentResourceID = cidmid;

	copmid = CreateNewColormap(tool,cidmid);

	/* CopyColormapAndFree packet */
	copmid->Packets = CombinePacketAndSize(packet,size);
	copmid->PacketsSize = size+sizeof(size);

	copmid->AllocPackets = GetMem(0);
	
	/* Create the dependency on the source colormap */
	newcmapheadnode = (ColormapHeadNode *) GetMem(sizeof(ColormapHeadNode));
	CleanUpStructure((char *) newcmapheadnode,sizeof(ColormapHeadNode));
	newcmapheadnode->User = ( void * ) copmid;
	newcmapheadnode->WhereAt = copsrc->PacketsSize;
	InsertNewNode(copsrc->HeadColormap,newcmapheadnode);
	copmid->TailColormap = newcmapheadnode;
	copmid->TailColormapObj = copsrc;
}

/*
 *	CreateColormapFunc
 *
 *	Processes CreateColormap request
 *
 *	RETURN VALUE: void
 */

void CreateColormapFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	ColormapObj *cop, *FindColormapByID(), *CreateNewColormap();
	WindowObj *MakeWindowDependency();
	Colormap     cid;
	CARD32	     wid;

	cid = (( xResourceReq * ) ( packet ))->id;
	if( (cop = FindColormapByID(cid,tool)) != NULL ){
		fprintf(stderr,"colormap already exists: CreateColormap\n");
		fprintf(stderr,"The colormap is %s\n",cop->State?"Freed":"Not Freed");
		exit(1);
	}

	/* register the id */
	tool->ToolInfo->RecentResourceID = cid;

	cop = CreateNewColormap(tool,cid);

	/* Make the dependency on the window */
	cop->TailWindow = (void *)
	    MakeWindowDependency(tool,((xCreateColormapReq *)packet)->window,0);

	/* CreateColormap packet */
	cop->Packets = CombinePacketAndSize(packet,size);
	cop->PacketsSize = size + sizeof(size);
	cop->AllocPackets = GetMem(0);
}

/*
 *	CreateICN
 *
 *	Create a InstalledColormapNode and return
 *
 *	RETURN VALUE: pointer to the created ICN
 */

InstalledColormapNode *CreateICN(cid)
Colormap cid;
{
	InstalledColormapNode *p;

	p = ( InstalledColormapNode * ) GetMem(sizeof(InstalledColormapNode));
	CleanUpStructure((char *) p,sizeof(InstalledColormapNode));
	p->cid = cid;
	return(p);
}

/*
 *	InstallColormapFunc
 *
 *	Processes InstallColormap request
 *
 *	RETURN VALUE: void
 */

void InstallColormapFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	ColormapObj *cop, *FindColormapByID();
        Colormap     cid;
	InstalledColormapNode *icn;

	cid = (( xResourceReq * ) ( packet ))->id;
        if( (cop = FindColormapByID(cid,tool)) == NULL ){
		fprintf(stderr,"InstallColormap: ID not found in registered colormap\n");
		exit(1);
	}
	if( cop->State == 1 ){
		fprintf(stderr,"Client trying to refer to freed colormap\n");
		exit(1);
	}

	/* find if the colormap was already installed */
	if( cop->ICNentry != NULL ){
		/* if already installed, move it to the front of the list */
		RemoveANode(cop->ICNentry);
		InsertNewNode(tool->ToolInfo->ICList,cop->ICNentry);
		return;
	}

	/* if not installed, install it */
	cop->ICNentry = CreateICN(cid);
	InsertNewNode(tool->ToolInfo->ICList,cop->ICNentry);
}

/*
 *	UninstallColormapFunc
 *
 *	Processes UninstallColormap request
 *
 *	RETURN VALUE: void
 */

void UninstallColormapFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
        ColormapObj *cop, *FindColormapByID();
        Colormap     cid;
        InstalledColormapNode *icn;

        cid = (( xResourceReq * ) ( packet ))->id;
	if( (cop = FindColormapByID(cid,tool)) == NULL ){
                fprintf(stderr,"UninstallColormap: ID not found in registered colormap\n");
		exit(1);
	}
	if( cop->State == 1 ){
		fprintf(stderr,"Client trying to refer to freed colormap\n");
		exit(1);
	}
	if( cop->ICNentry != NULL ){
		/* remove the entry if installed */
		RemoveANode(cop->ICNentry);
		FreeMem(cop->ICNentry);
		cop->ICNentry = NULL;
	}
}

/*
 *	FreeColormapFunc
 *
 *	Handles FreeColormap request
 *
 *	RETURN VALUE: void
 */

void FreeColormapFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
        ColormapObj *cop, *FindColormapByID();
        Colormap     cid;

	cid = (( xResourceReq * ) ( packet ))->id;
	if( (cop = FindColormapByID(cid,tool))
		== NULL ){
		fprintf(stderr,"Client has requested to free unregistered colormap\n");
		return;
	}
	if( cop->State == 1 ){
		fprintf(stderr,"Client trying to refer to freed colormap\n");
		exit(1);
	}
	cop->State = 1; /* Freed */
	if( cop->Next == NULL ) /* Don't want to free Default colormap */
		return;
	if( cop->ICNentry != NULL ){ /* if ICNentry exists, free it also */
		RemoveANode(cop->ICNentry);
		FreeMem(cop->ICNentry);
	}
	RecFreeColormap(cop);
}
