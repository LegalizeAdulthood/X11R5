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

extern ColormapObj *MakeColormapDependecy();
extern ColormapObj *CreateNewColormap();

CleanUpStructure(buf,size)
char *buf;
int size;
{
	int i;

	for( i = 0; i<size; i++,buf++ )
		(*buf) = '\0';
}

/*
 *	InitializeObjects
 *
 *	Initializes the server state data structure for the tool
 *
 *	RETURN VALUE: void
 */
InitializeObjects(tool)
Tool	*tool;
{
	tool->ToolInfo = ( ToolState * ) GetMem(sizeof(ToolState));
	CleanUpStructure( (char *) (tool->ToolInfo),sizeof(ToolState) );

	tool->ToolInfo->CursorList =
			( CursorObj * ) GetMem(sizeof(CursorObj));
	CleanUpStructure( (char *) (tool->ToolInfo->CursorList),
			  sizeof(CursorObj) );

	tool->ToolInfo->FontList =
			( FontObj * ) GetMem(sizeof(FontObj));
	CleanUpStructure( (char *) (tool->ToolInfo->FontList),
			  sizeof(FontObj) );

	tool->ToolInfo->GCList = ( GCObj * ) GetMem(sizeof(GCObj));
	CleanUpStructure( (char *) (tool->ToolInfo->GCList),
			  sizeof(GCObj) );

	tool->ToolInfo->PixmapList =
			( PixmapObj * ) GetMem(sizeof(PixmapObj));
	CleanUpStructure( (char *) (tool->ToolInfo->PixmapList),
			  sizeof(PixmapObj) );

	tool->ToolInfo->ColormapList =
			( ColormapObj * ) GetMem(sizeof(ColormapObj));
	CleanUpStructure( (char *) (tool->ToolInfo->ColormapList),
			  sizeof(ColormapObj) );
	tool->ToolInfo->ICList =
			( InstalledColormapNode * ) GetMem(sizeof(InstalledColormapNode));
	CleanUpStructure( (char *) (tool->ToolInfo->ICList),
			  sizeof(InstalledColormapNode) );
	AddDefaultColormap(tool);

	tool->ToolInfo->WindowList =
			( WindowObj * ) GetMem(sizeof(WindowObj));
	CleanUpStructure( (char *) (tool->ToolInfo->WindowList),
			  sizeof(WindowObj) );
	AddRootWindowNode(tool);

	tool->ToolInfo->WindowList->Next->TailColormap = ( ColormapObj * )
	MakeColormapDependency(tool,tool->ToolInfo->ColormapList->Next->cid);

	tool->ToolInfo->Destroyeds =
			( WindowObj * ) GetMem(sizeof(WindowObj));
	CleanUpStructure( (char *) (tool->ToolInfo->Destroyeds),
			  sizeof(WindowObj) );

	tool->ToolInfo->InternalAtom = GetMem(0);
	tool->ToolInfo->InternalAtomSize = 0;
	tool->ToolInfo->SaveAlls = GetMem(0);
	tool->ToolInfo->SaveAllsSize = 0;
}

/*
 *	AddDefaultColormap
 *
 *	Adds the default colormap contained in the root info
 *
 *	RETURN VALUE: void
 */
AddDefaultColormap(tool)
Tool *tool;
{
	ColormapObj   *cop;
	xWindowRoot   *LocalRoot;

	LocalRoot = &(tool->HostRootInfo);
	cop = CreateNewColormap(tool,SwapLong(LocalRoot->defaultColormap,tool->ConnectPrefix.byteOrder));
	cop->Packets = GetMem(0);
	cop->AllocPackets = GetMem(0);
}

/*
 *	AddRootWindowNode
 *
 *	Attaches the node for the root window
 *
 *	RETURN VALUE: void
 */
AddRootWindowNode(tool)
Tool *tool;
{
	xCreateWindowReq *prefix;
	CARD32		 *attrib;
	xWindowRoot   LocalRoot;
	WindowObj     *wop;

	prefix = (xCreateWindowReq *) GetMem(sizeof(xCreateWindowReq));
	CleanUpStructure((char *) prefix,sizeof(xCreateWindowReq));
	LocalRoot = tool->HostRootInfo;

	prefix->depth = LocalRoot.rootDepth;
	prefix->wid = SwapLong(LocalRoot.windowId,tool->ConnectPrefix.byteOrder);
	prefix->class = 1; /* InputOutput */
	prefix->visual = SwapLong(LocalRoot.rootVisualID,tool->ConnectPrefix.byteOrder);

	attrib = ( CARD32 * ) GetMem( 15*sizeof(CARD32) );
	CleanUpStructure( (char *) attrib, 15*sizeof(CARD32) );
	*(attrib+0) = 1; /* ParentRelative */
	*(attrib+13) = SwapLong(LocalRoot.defaultColormap,tool->ConnectPrefix.byteOrder);

	wop = (WindowObj *) GetMem(sizeof(WindowObj));
	CleanUpStructure((char *) wop,sizeof(WindowObj));
	wop->Prefix = prefix;
	wop->Attrib = attrib;
	wop->PropertyPackets = GetMem(0);
	wop->PropertyPacketsSize = 0;

	InsertNewNode(tool->ToolInfo->WindowList,wop);

	wop->Children = (WindowObj *) GetMem(sizeof(WindowObj));
	CleanUpStructure( (char *) (wop->Children), sizeof(WindowObj) );
}

/*
 *	FreeToolInfo
 *
 *	Frees all the information on the tool when the tool gets deleted
 *
 *	RETURN VALUE: void
 */
FreeToolInfo( Info )
ToolState *Info;
{
	/* Cursor */
	{
		CursorObj *i;
		CursorObj *j;

		for( i = Info->CursorList, j = Info->CursorList->Next;
		     i != NULL; ){
			if( i->CreatePacket != NULL )
				FreeMem(i->CreatePacket);
			if( i->RecolorPacket != NULL )
				FreeMem(i->RecolorPacket);
			FreeMem(i);
			i = j;
			if( j != NULL )
				j = j->Next;
		}
	}

	/* Font */
	{
		FontObj *i;
		FontObj *j;

		for( i = Info->FontList, j = Info->FontList->Next;
		     i != NULL; ){
			if( i->OpenPacket != NULL )
				FreeMem(i->OpenPacket);
			if( i->SetPathPacket != NULL )
				FreeMem(i->SetPathPacket);
			FreeMem(i);
			i = j;
			if( j != NULL )
				j = j->Next;
		}
	}

	/* GC */
	{
		GCObj *i;
		GCObj *j;

		for( i = Info->GCList, j = Info->GCList->Next; i != NULL; ){
			if( i->Prefix != NULL )
				FreeMem(i->Prefix);
			if( i->Attrib != NULL )
				FreeMem(i->Attrib);
			if( i->SetClipRectanglesPacket != NULL )
				FreeMem(i->SetClipRectanglesPacket);
			if( i->SetDashesPacket != NULL )
				FreeMem(i->SetDashesPacket);
			FreeMem(i);
			i = j;
			if( j != NULL )
				j = j->Next;
		}
	}

	/* Pixmap */
	{
		PixmapObj *i;
		PixmapObj *j;

		for( i = Info->PixmapList, j = Info->PixmapList->Next;
		     i != NULL; ){
		     PixmapPacketNode *k, *l;
		     if( i->PixmapPacketNodeList != NULL ){
			for( k = i->PixmapPacketNodeList, l = i->PixmapPacketNodeList->Next;
			     k != NULL; ){
				if( k->HeadWindow != NULL ){
					PixmapPacketNodeHead *m, *n;

					for( m = k->HeadWindow, n = k->HeadWindow->Next;
					     m != NULL; ){
						FreeMem(m);
						m = n;
						if( n != NULL )
							n = n->Next;
					}
				}
				if( k->HeadGC != NULL ){
					PixmapPacketNodeHead *m, *n;

					for( m = k->HeadGC, n = k->HeadGC->Next;
					     m != NULL; ){
						FreeMem(m);
						m = n;
						if( n != NULL )
							n = n->Next;
					}
				}
				if( k->HeadCursor != NULL ){
					PixmapPacketNodeHead *m, *n;

					for( m = k->HeadCursor, n = k->HeadCursor->Next;
					     m != NULL; ){
						FreeMem(m);
						m = n;
						if( n != NULL )
							n = n->Next;
					}
				}
				if( k->PutImagePacket != NULL ){
					FreeMem(k->PutImagePacket);
				}
				FreeMem(k);
				k = l;
				if( l != NULL )
					l = l->Next;
			}
		    }
		    FreeMem(i);
		    i = j;
		    if( j != NULL )
			j = j->Next;
		}
	}

	/* Colormap */
	{
		ColormapObj *i, *j;
		for( i = Info->ColormapList, j = Info->ColormapList->Next;
		     i != NULL; ){
			ColormapHeadNode *k, *l;
		     if( i->HeadColormap != NULL ){
			for( k = i->HeadColormap, l = i->HeadColormap->Next;
			     k != NULL; ){
				FreeMem(k);
				k = l;
				if( l != NULL )
					l = l->Next;
			}
		     }
		     if( i->Packets != NULL )
			FreeMem(i->Packets);
		     if( i->AllocPackets != NULL )
			FreeMem(i->AllocPackets);
		     FreeMem(i);
		     i = j;
		     if( j != NULL )
			j = j->Next;
		}
	}

	/* ICList */
	{
		InstalledColormapNode *i, *j;
		for( i = Info->ICList, j = Info->ICList->Next;
		     i != NULL; ){
			FreeMem(i);
			i = j;
			if( j != NULL )
				j = j->Next;
		}
	}

	/* Window */
	{
		RecFreeMemWindow(Info->WindowList->Next);
		FreeMem(Info->WindowList);
	}

	/* Destroyeds */
	{
		WindowObj *i, *j;
		for( i = Info->Destroyeds, j = Info->Destroyeds->Next; i != NULL; ){
			RecFreeMemWindow(i);
			i = j;
			if( j != NULL )
				j = j->Next;
		}
	}

	/* InternalAtom */
	if( Info->InternalAtom != NULL )
		FreeMem(Info->InternalAtom);

	/* SaveAlls */
	if( Info->SaveAlls != NULL )
		FreeMem(Info->SaveAlls);

	/* Info itself */
	FreeMem(Info);
}

/*
 *	RecFreeMemWindow
 *
 *	Frees data structures for each window and do the same for its children
 *
 *	RETURN VALUE: void
 */
RecFreeMemWindow(wop)
WindowObj *wop;
{
	WindowObj *i, *j;

    if( wop->Children != NULL ){
	for( i = wop->Children, j = wop->Children->Next; i != NULL; ){
		RecFreeMemWindow(i);
		i = j;
		if( j != NULL )
			j = j->Next;
	}
    }
	if( wop->GrabRequests != NULL )
		FreeMem(wop->GrabRequests);
	if( wop->Prefix != NULL )
		FreeMem(wop->Prefix);
	if( wop->Attrib != NULL )
		FreeMem(wop->Attrib);
	if( wop->PropertyPackets != NULL )
		FreeMem(wop->PropertyPackets);
	FreeMem(wop);
}
