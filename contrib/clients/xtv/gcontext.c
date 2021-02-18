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

extern WindowObj *FindWindowByID();
extern WindowObj *MakeWindowDependency();
extern PixmapObj *FindPixmapByID();
extern PixmapObj *MakePixmapDependency();
extern PixmapObj *MakePixmapDependency2();
extern FontObj	 *MakeFontDependency();
extern char	 *CombinePacketAndSize();

/*
 *	FindGCByID
 *
 *	Finds GC object by ID
 *
 *	RETURN VALUE: pointer to the found GC object
 */

GCObj *FindGCByID(gcid,tool)
GContext gcid;
Tool *tool;
{
	GCObj *Current;

	Current = tool->ToolInfo->GCList->Next;
	while( Current != NULL ){
		if( Current->gcid == gcid )
			return(Current);
		Current = Current->Next;
	}
	return(NULL);
}

/*
 *	CreateGCFunc
 *
 *	Processes CreateGC request
 *
 *	RETURN VALUE: void
 */

void CreateGCFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	xCreateGCReq *req;
	GContext     gcid;
	GCObj	     *gcop, *FindGCByID();
	CARD32	     *attrib;
	int	     i;
	WindowObj	*wop;
	PixmapObj      *pmop;
	PixmapPacketNode *tmp;

	req = (xCreateGCReq *) (packet);
	gcid = req->gc;

	if( (gcop = FindGCByID(gcid,tool)) != NULL ){
		fprintf(stderr,"CreateGCFunc: duplicate gcontext\n");
		exit(1);
	}

	/* register the current id */
	tool->ToolInfo->RecentResourceID = gcid;

	/* get structures */
	gcop = (GCObj *) GetMem(sizeof(GCObj));
	CleanUpStructure((char *) gcop,sizeof(GCObj));
	gcop->gcid = gcid;
	gcop->Prefix = (xCreateGCReq *) GetMem(sizeof(xCreateGCReq));
	CleanUpStructure((char *) (gcop->Prefix),sizeof(xCreateGCReq));
	gcop->Attrib = (CARD32 *) GetMem(23*sizeof(CARD32));
	CleanUpStructure((char *) (gcop->Attrib),23*sizeof(CARD32));

	/* copy Prefix */
	memcpy((char *) gcop->Prefix,packet,sizeof(xCreateGCReq));
	/* unpack Attrib */
	attrib = (CARD32 *) (packet+sizeof(xCreateGCReq));
	for( i=0; i<23; i++ )
		if( gcop->Prefix->mask & 1 << i ){
			*(gcop->Attrib+i) = *attrib;
			switch(i){
				case 10: gcop->TailTileObj =
					MakePixmapDependency(tool,*attrib,gcop,
					1,0,&tmp);
					gcop->TailTile = (void *) tmp;
					break;
				case 11: gcop->TailStippleObj =
					MakePixmapDependency(tool,*attrib,gcop,
					1,1,&tmp);
					gcop->TailStipple = (void *) tmp;
					break;
				case 14: gcop->TailFont =
					MakeFontDependency(tool,*attrib,0);
					break;
				case 19: if( *attrib == 0 ) break; /* None */
					gcop->TailClippingMaskObj =
					MakePixmapDependency(tool,*attrib,gcop,
					1,2,&tmp);
					gcop->TailClippingMask = (void *) tmp;
					break;
				default: break;
			}
			attrib++;
		}

	/* Make dependency on window or pixmap */
	if( (gcop->TailWindow =
		(void *) MakeWindowDependency(tool,req->drawable,1))
	    == NULL ){
		gcop->TailPixmap = MakePixmapDependency2(tool,req->drawable,1);
	}
	else
		gcop->TailPixmap = NULL;

	/* Put the node into the GCList */
	InsertNewNode(tool->ToolInfo->GCList,gcop);
}

/*
 *	ChangeGCFunc
 *
 *	Processes ChangeGC request
 *
 *	RETURN VALUE: void
 */

void ChangeGCFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	GCObj	*gcop, *FindGCByID();
	GContext gcid;
	CARD32	 AndedMask;
	CARD32	 NewMask;
	CARD32	*OldAttrib;
	CARD32	*NewAttrib;
	int	 i;
	PixmapPacketNode *tmp;

	gcid = ((xChangeGCReq *) (packet))->gc;
	if( ( gcop = FindGCByID(gcid,tool) ) == NULL ){
		fprintf(stderr,"ChangeGCFunc: Can't find the gc\n");
		exit(1);
	}
	
	NewMask = ((xChangeGCReq *) (packet))->mask;
	AndedMask = gcop->Prefix->mask & NewMask; /* attributes to be changed */

	/* tile */
	if( AndedMask & 1<<10 )
		RemoveDependencyOnPixmapDrawing(tool,gcop->TailTile,gcop,gcop->TailTileObj);
	/* stipple */
	if( AndedMask & 1<<11 )
		RemoveDependencyOnPixmapDrawing(tool,gcop->TailStipple,gcop,gcop->TailStippleObj);
	/* font */
	if( AndedMask & 1<<14 )
		RemoveDependencyOnFont(gcop->TailFont,0);
	/* clip-mask */
	if( (AndedMask & 1<<19) && *(gcop->Attrib+19) >0 )
		RemoveDependencyOnPixmapDrawing(tool,gcop->TailClippingMask,gcop,gcop->TailClippingMaskObj);

	OldAttrib = gcop->Attrib;
	NewAttrib = ( CARD32 * ) (packet+sizeof(xChangeGCReq));
	for( i=0; i<23; i++ )
		if( NewMask & 1<<i ){
			*(OldAttrib+i) = *NewAttrib;
			NewAttrib++;
			switch(i){
				case 10: gcop->TailTileObj =
					MakePixmapDependency(tool,*(OldAttrib+i)
					,gcop,1,0,&tmp);
					gcop->TailTile = (void *) tmp;
					break;
				case 11: gcop->TailStippleObj =
                                        MakePixmapDependency(tool,*(OldAttrib+i)
					,gcop,1,1,&tmp);
					gcop->TailStipple = (void *) tmp;
					break;
				case 14: gcop->TailFont =
					MakeFontDependency(tool,*(OldAttrib+i)
					,0);
					break;
				case 19: if( *(OldAttrib+i) == 0 ) break;
					/* None */
                                        gcop->TailClippingMaskObj =
                                        MakePixmapDependency(tool,*(OldAttrib+i)
					,gcop,1,2,&tmp);
					gcop->TailClippingMask=(void *) tmp;
					break;
				default: break;
                        }
		}

	gcop->Prefix->mask = gcop->Prefix->mask | NewMask;
}

/*
 *	CopyGCFunc
 *
 *	Process CopyGC request
 *	To remove the dependency among GC's, a new CreateGC will be created
 *	having the same attributes as the source GC
 *
 *	RETURN VALUE: void
 */

void CopyGCFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	GCObj *srcp, *FindGCByID();
	GCObj *dstp;
	xCopyGCReq *req;
	CARD32   AndedMask;
	CARD32   NewMask;
	CARD32	*OldAttrib;
	CARD32	*NewAttrib;
	int	i;
	PixmapPacketNode *tmp;

	req = (xCopyGCReq *) (packet);
	if( (srcp = FindGCByID(req->srcGC,tool)) == NULL ){
		fprintf(stderr,"CopyGCFunc: Can't find the source\n");
		exit(1);
	}
	if( (dstp = FindGCByID(req->dstGC,tool)) == NULL ){
		fprintf(stderr,"CopyGCFunc: Can't find the destination\n");
		exit(1);
	}

	NewMask = req->mask;
	AndedMask = dstp->Prefix->mask & NewMask; /* attributes to be changed */

	/* tile */
	if( AndedMask & 1<<10 )
		RemoveDependencyOnPixmapDrawing(tool,dstp->TailTile,dstp,dstp->TailTileObj);
	/* stipple */
	if( AndedMask & 1<<11 )
		RemoveDependencyOnPixmapDrawing(tool,dstp->TailStipple,dstp,dstp->TailStippleObj);
	/* font */
	if( AndedMask & 1<<14 )
		 RemoveDependencyOnFont(dstp->TailFont,0);
	/* clip-mask */
	if( (AndedMask & 1<<19) && *(dstp->Attrib+19) >0 )
		RemoveDependencyOnPixmapDrawing(tool,dstp->TailClippingMask,dstp,dstp->TailClippingMaskObj);

	OldAttrib = dstp->Attrib;
	NewAttrib = srcp->Attrib;
	for( i=0; i<23; i++ )
		if( NewMask & 1<<i ){
			*(OldAttrib+i) = *(NewAttrib+i);
			switch(i){
				case 10: dstp->TailTileObj =
					MakePixmapDependency(tool,*(OldAttrib+i)
					,dstp,1,0,&tmp);
					dstp->TailTile = (void *) tmp;
					break;
				case 11: dstp->TailStippleObj =
					MakePixmapDependency(tool,*(OldAttrib+i)
					,dstp,1,1,&tmp);
					dstp->TailStipple = (void *) tmp;
					break;
				case 14: dstp->TailFont =
					MakeFontDependency(tool,*(OldAttrib+i)
					,0);
					break;
				case 19: if( *(OldAttrib+i) == 0 ) break;
					/* None */
					dstp->TailClippingMaskObj =
					MakePixmapDependency(tool,*(OldAttrib+i)
					,dstp,1,2,&tmp);
					dstp->TailClippingMask=(void *) tmp;
					break;
				default: break;
			}
		}
	dstp->Prefix->mask = dstp->Prefix->mask | NewMask;
}

/*
 *	SetDashesFunc
 *
 *	Processes SetDashes packet
 *
 *	RETURN VALUE: void
 */

void SetDashesFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	GCObj *gcop, *FindGCByID();

	if( (gcop = FindGCByID(((xSetDashesReq *)(packet))->gc,tool)) == NULL ){
		fprintf(stderr,"SetDashesFunc: Can't find the gc\n");
		exit(1);
	}

	if( gcop->SetDashesPacket != NULL )
		FreeMem(gcop->SetDashesPacket);

	gcop->SetDashesPacket = CombinePacketAndSize(packet,size);
}

/*
 *	SetClipRectanglesFunc
 *
 *	Process SetClipRectangles request
 *
 *	RETURN VALUE: void
 */

void SetClipRectanglesFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
        GCObj *gcop, *FindGCByID();

        if( (gcop = FindGCByID(((xSetClipRectanglesReq *)(packet))->gc,
		tool)) == NULL ){
                fprintf(stderr,"SetClipRectanglesFunc: Can't find the gc\n");
                exit(1);
        }

        if( gcop->SetClipRectanglesPacket != NULL )
                FreeMem(gcop->SetClipRectanglesPacket);

        gcop->SetClipRectanglesPacket = CombinePacketAndSize(packet,size);
}

/*
 *	FreeGCFunc
 *
 *	Processes FreeGC request and free all object structures this GC depends
 *	on if possible
 *
 *	RETURN VALUE: void
 */

void FreeGCFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	GCObj *gcop, *FindGCByID();

	if( (gcop = FindGCByID( ((xResourceReq *)(packet))->id,tool)) == NULL ){
		fprintf(stderr,"FreeGCFunc: Can't find gc\n");
		exit(1);
	}

	/* tile */
	if( gcop->Prefix->mask & 1<<10 )
		RemoveDependencyOnPixmapDrawing(tool,gcop->TailTile,gcop,gcop->TailTileObj);
	/* stipple */
        if( gcop->Prefix->mask & 1<<11 )
                RemoveDependencyOnPixmapDrawing(tool,gcop->TailStipple,gcop,gcop->TailStippleObj);
        /* font */
        if( gcop->Prefix->mask & 1<<14 )
                RemoveDependencyOnFont(gcop->TailFont,0);
        /* clip-mask */
        if( (gcop->Prefix->mask & 1<<19) && *(gcop->Attrib+19) >0 )
                RemoveDependencyOnPixmapDrawing(tool,gcop->TailClippingMask,gcop,gcop->TailClippingMaskObj);

	if( gcop->TailWindow != NULL )
		RemoveDependencyOnWindow((WindowObj *)(gcop->TailWindow),1);
	else
		RemoveDependencyOnPixmap2(tool,(PixmapObj *)(gcop->TailPixmap),1);

	FreeMem(gcop->Prefix);
	FreeMem(gcop->Attrib);
	if( gcop->SetClipRectanglesPacket != NULL )
		FreeMem(gcop->SetClipRectanglesPacket);
	if( gcop->SetDashesPacket != NULL )
		FreeMem(gcop->SetDashesPacket);
	RemoveANode(gcop);
	FreeMem(gcop);
}
