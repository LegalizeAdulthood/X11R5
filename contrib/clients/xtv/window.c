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

extern PixmapObj *MakePixmapDependency();
extern ColormapObj  *MakeColormapDependency();
extern CursorObj *MakeCursorDependency();
extern int ReportToolStat();

/*
 *	FindWindowByDepth
 *
 *	Find a window with a given depth
 *
 *	RETURN VALUE: window ID
 */

CARD32 FindWindowByDepth(depth,start)
CARD32 depth;
WindowObj *start;
{
	WindowObj *Current;
	CARD32	Found;

	if( start->Prefix->depth == depth )
		return(start->Prefix->wid);
	for( Current = start->Children->Next;
	     Found == 0 && Current != NULL;
	     Current = Current->Next)
		Found = FindWindowByDepth(depth,Current);
	return(Found);
}

/*
 *	FindWindowByID
 *
 *	Find the window given the ID of it
 *
 *	RETURN VALUE: pointer to the window object
 */

WindowObj *FindWindowByID(wid,start)
Window	wid;
WindowObj *start;
{
	WindowObj *Current;
	WindowObj *Found=NULL;

	if( start->Prefix->wid == wid )
		return(start);
	for( Current = start->Children->Next;
	     Found == NULL && Current != NULL;
	     Current = Current->Next)
		Found=FindWindowByID(wid,Current);
	return(Found);
}

/*
 *	MakeWindowDependency
 *
 *	Make dependency on window of other objects
 *
 *	RETURN VALUE: pointer to the window
 */

WindowObj *MakeWindowDependency(tool,wid,whose)
Tool *tool;
CARD32 wid;
int whose;	/* 0 - Colormap; 1 - GC; 2 - Pixmap */
{
	WindowObj *wop;

	if( (wop = FindWindowByID(wid,tool->ToolInfo->WindowList->Next))
	    == NULL ){
		if( whose == 2 || whose == 1 ) return(NULL);
		fprintf(stderr,"MakeWindowDependency: Can't find the window\n");
		exit(1);
	}
	if( wop->State == 2 ){
		fprintf(stderr,"Client referring to a destroyed pixmap\n");
		exit(1);
	}
	/* increment the number of the objects depending on the window */
	switch(whose){
		case 0: ++wop->HeadColormap;
			break;
		case 1: ++wop->HeadGC;
			break;
		case 2: ++wop->HeadPixmap;
			break;
		default: fprintf(stderr,"MakeWindowDependency: unexpected argument value\n");
			exit(1);
			break;
	}
	return(wop);
}

/*
 *	RemoveDependencyOnWindow
 *
 *	Remove dependency on the window
 *
 *	RETURN VALUE: void
 */

RemoveDependencyOnWindow(wop,whose)
WindowObj *wop;
int whose;	/* 0 - Colormap; 1 - GC; 2 - Pixmap */
{
	/* decrement the number of objects depending on the window */
	switch(whose){
		case 0: --wop->HeadColormap;
			break;
		case 1: --wop->HeadGC;
			break;
		case 2: --wop->HeadPixmap;
			break;
		default: fprintf(stderr,"RemoveDependencyOnWindow: unexpected argument value\n");
			exit(1);
			break;
	}
	if( wop->State != 2 || /* destroyed */
	    wop->HeadColormap > 0 ||
	    wop->HeadGC > 0 ||
	    wop->HeadPixmap > 0 )
		return;

	if( wop->PropertyPacketsSize > 0 )
		FreeMem(wop->PropertyPackets);
	FreeMem(wop->Prefix);
	FreeMem(wop->Attrib);
	RemoveANode(wop);
	FreeMem(wop);
}

/*
 *	RecDestroyWindow
 *
 *	Remove the data structure for a window if possible and
 *	free other objects the window depends on if necessary
 *
 *	RETURN VALUE: void
 */

RecDestroyWindow(wop,tool)
WindowObj *wop;
Tool *tool;
{
        WindowObj	*Current;
	WindowObj	*Future;
	CARD32	       CurMask;
	CARD32	       *Attrib;

	wop->State = 3; /* yellow node */

	CurMask = wop->Prefix->mask;
	Attrib = wop->Attrib;

	if( wop->Prefix->mask & 1<<6 )
		--(((tool->ToolInfo->PAG)[wop->Prefix->depth]).number);

	/* release all resources ( if I can ) this window refers to */
	/* background pixmap */
	if( (CurMask & 1<<0) && ( *Attrib > 1 ) )
                RemoveDependencyOnPixmapDrawing(tool,wop->TailBackgroundPixmap,
                                wop,wop->TailBackgroundPixmapObj);
        /* border pixmap */
        if( (CurMask & 1<<2) && ( *(Attrib+2) > 0 ) )
                RemoveDependencyOnPixmapDrawing(tool,wop->TailBorderPixmap,
                                wop,wop->TailBorderPixmapObj);
        /* colormap */
        if( (CurMask & 1<<13) && ( *(Attrib+13) > 0 ) )
		RemoveDependencyOnColormap(wop->TailColormap);
        /* cursor */
        if( (CurMask & 1<<14) && ( *(Attrib+14) > 0 ) )
		RemoveDependencyOnCursor(tool,wop->TailCursor);

	wop->Prefix->mask = 0;

	/* for each subwindows: call this function recursively */
	for( Current = wop->Children->Next; Current != NULL;
	     Current = wop->Children->Next)
		RecDestroyWindow(Current,tool);

	/* free property packets */
	if( wop->PropertyPacketsSize > 0 ){
		FreeMem(wop->PropertyPackets);
		wop->PropertyPacketsSize = 0;
	}
	/* free grab request packets */
	if( wop->GrabRequestsSize > 0 ){
		FreeMem(wop->GrabRequests);
		wop->GrabRequests = NULL;
	}

	/* if any object still depends on this window, put it in the Destroyed
	   window list */
	if( wop->HeadColormap > 0 ||
	    wop->HeadGC > 0 ||
	    wop->HeadPixmap > 0 ){
		RemoveANode(wop);
		/* leave only the essential packets, because this window
		   will be freed on the new subscriber's server immediately */
		FreeMem(wop->Children);
		wop->Children = NULL;
		if( wop->GrabRequests != NULL ){
			FreeMem(wop->GrabRequests);
			wop->GrabRequests = NULL;
		}
		if( wop->PropertyPackets != NULL ){
			FreeMem(wop->PropertyPackets);
			wop->PropertyPackets = NULL;
		}
		wop->State = 2; /* destroyed */
		wop->Prefix->mask = 0;
		wop->Prefix->parent = (tool->HostRootInfo).windowId;
		InsertNewNode(tool->ToolInfo->Destroyeds,wop);
	}
	else{
		FreeMem(wop->Prefix);
		FreeMem(wop->Attrib);
		FreeMem(wop->Children);
		RemoveANode(wop);
		FreeMem(wop);
	}
}

/*
 *	CreateWindowFunc
 *
 *	Handles CreateWindow request
 *
 *	RETURN VALUE: void
 */

void CreateWindowFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	xCreateWindowReq *cwp;
	WindowObj	*root;
	WindowObj	*parent;
	WindowObj      *wop;
	CARD32	       *ap;
	int		i;

	root = tool->ToolInfo->WindowList->Next;

	cwp = (xCreateWindowReq *) (packet);
	if( (wop = FindWindowByID(cwp->wid,tool->ToolInfo->WindowList->Next))
	    != NULL ){
		fprintf(stderr,"CreateWindowFunc: Duplicate Window ID\n");
		exit(1);
	}
	if( (parent = FindWindowByID(cwp->parent,
	    tool->ToolInfo->WindowList->Next)) == NULL ){
		fprintf(stderr,"CreateWindowFunc: Specified parent not found\n");
		exit(1);
	}

	/* register the current wid */
	tool->ToolInfo->RecentResourceID = cwp->wid;

	/* constructing the tree node structure */
	wop = (WindowObj *) GetMem(sizeof(WindowObj));
	CleanUpStructure( (char *) wop,sizeof(WindowObj) );
	wop->PropertyPackets = GetMem(0);
	wop->GrabRequests = GetMem(0);
	wop->Parent = parent;
	wop->Children = ( WindowObj *) GetMem(sizeof(WindowObj));
	CleanUpStructure((char *) (wop->Children),sizeof(WindowObj));
	/* appending to the tree */
	InsertNewNode(parent->Children,wop);

	/* copying prefix of the request */
	wop->Prefix = (xCreateWindowReq *) GetMem(sizeof(xCreateWindowReq));
	memcpy((char *)(wop->Prefix),packet,sizeof(xCreateWindowReq));

	/* unpacking the optional attributes */
	wop->Attrib = (CARD32 *) GetMem(15*sizeof(CARD32));
	CleanUpStructure((char *)(wop->Attrib),15*sizeof(CARD32));
	ap = ( CARD32 * ) ( packet+sizeof(xCreateWindowReq) );
	for(i=0;i<15;i++)
		if( wop->Prefix->mask & 1 << i ){
			*(wop->Attrib+i) = *ap;
			ap++;
		}

	/* processing prefix part of the request */
	/* depth */
	if( wop->Prefix->depth == 0 &&
	    ( wop->Prefix->class == 0 || /* CopyFromParent */
	      wop->Prefix->class == 1 ) && /* InputOutput */
	    wop != root )
		wop->Prefix->depth = parent->Prefix->depth; /* copy it */
	/* class */
	if( wop->Prefix->class == 0 && /* CopyFromParent */
	    wop != root )
		wop->Prefix->class = parent->Prefix->class;
	/* visual */
	if( wop->Prefix->visual == 0 && /* CopyFromParent */
	    wop != root )
		wop->Prefix->visual = parent->Prefix->visual;

	/* processing attribute part */
	/* border pixmap */
	if( (wop->Prefix->mask & 1 << 2) && (*(wop->Attrib+2) == 0) &&
	    (wop != root) )
		/* CopyFromParent */
		*(wop->Attrib+2) = *(parent->Attrib+2);
	/* colormap */
	if( (wop->Prefix->mask & 1 << 13) && (*(wop->Attrib+13) == 0) &&
	    (wop != root) )
		/* CopyFromParent */
		*(wop->Attrib+13) = *(parent->Attrib+13);

	/* Making dependencies */
	/* background pixmap */
	if( (wop->Prefix->mask & 1 << 0) &&
	    (*(wop->Attrib+0) > 1) ){ /* It's a pixmap */
		wop->TailBackgroundPixmapObj =
			MakePixmapDependency(tool,*(wop->Attrib+0),wop,0,0,
				&(wop->TailBackgroundPixmap));
	}
	/* border pixmap */
	if( (wop->Prefix->mask & 1 << 2) &&
	    *(wop->Attrib+2) > 0 ){ /* It's a pixmap */
		wop->TailBorderPixmapObj =
			MakePixmapDependency(tool,*(wop->Attrib+2),wop,0,1,
			&(wop->TailBorderPixmap));
	}
	/* colormap */
	if( (wop->Prefix->mask & 1 << 13) &&
	    *(wop->Attrib+13) > 0 ) /* It's a colormap */
		wop->TailColormap =
			MakeColormapDependency(tool,*(wop->Attrib+13));
	/* cursor */
	if( (wop->Prefix->mask & 1 << 14) &&
	    *(wop->Attrib+14) > 0 ) /* It's a cursor */
		wop->TailCursor =
			MakeCursorDependency(tool,*(wop->Attrib+14));

        /* You must not do anything to modify the Pixmap structures down here */
	/* register the depth of the window if this window has backing store
	   option turned on */
	if( wop->Prefix->mask & 1<<6 )
		++(((tool->ToolInfo->PAG)[wop->Prefix->depth]).number);
}

/*
 *	ChangeWindowAttributesFunc
 *
 *	Handles ChangeWindowAttirbutes request
 *
 *	RETURN VALUE: void
 */

void ChangeWindowAttributesFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	CARD32 AndedMask;
	CARD32 NewMask;
	Window	wid;
	WindowObj *wop;
	WindowObj *parent;
	xChangeWindowAttributesReq *req;
	CARD32 *OldAttrib;
	CARD32 *NewAttrib;
	int i;

	req = ( xChangeWindowAttributesReq * ) (packet);
	/* Find the window */
	wid = req->window;
	if( (wop = FindWindowByID(wid,tool->ToolInfo->WindowList->Next))
	    == NULL ){
		fprintf(stderr,"ChangeWindowAttributesFunc: can't find window %d\n",wid);
		return;
	}

	/* Changing Attributes */
	AndedMask = wop->Prefix->mask & req->valueMask;
	OldAttrib = wop->Attrib;

	if( (req->valueMask & 1<<6) && !(wop->Prefix->mask & 1<<6) )
		++(((tool->ToolInfo->PAG)[wop->Prefix->depth]).number);

	/* Check if we have to delete dependency for some resources */
	/* background pixmap */
	if( (req->valueMask & 1<<1) || (req->valueMask & 1<<0) ){
	    if( (wop->Prefix->mask & 1<<0) && (*OldAttrib > 1) ){
		RemoveDependencyOnPixmapDrawing(tool,wop->TailBackgroundPixmap,
				wop,wop->TailBackgroundPixmapObj);
	    }
	    wop->Prefix->mask &= 0xfffffffc;
	}
	/* border pixmap */
	if( (req->valueMask & 1<<3) || (req->valueMask & 1<<2) ){
	    if( (wop->Prefix->mask & 1<<2) && (*(OldAttrib+2) > 0) ){
		RemoveDependencyOnPixmapDrawing(tool,wop->TailBorderPixmap,
				wop,wop->TailBorderPixmapObj);
	    }
	    wop->Prefix->mask &= 0xfffffff3;
	}
	/* colormap */
	if( (AndedMask & 1<<13) && ( *(OldAttrib+13) > 0 ) )
		RemoveDependencyOnColormap(wop->TailColormap);
	/* cursor */
	if( (AndedMask & 1<<14) && ( *(OldAttrib+14) > 0 ) )
		RemoveDependencyOnCursor(tool,wop->TailCursor);

	NewAttrib = (CARD32 *) ( packet+sizeof(xChangeWindowAttributesReq) );
	parent = wop->Parent;
	/* reflecting New Attributes */
	for( i=0; i<15; i++){
		if( req->valueMask & 1<<i ){
			*(OldAttrib+i) = *NewAttrib;
			NewAttrib++;
			switch( i ){
				case 0: 
if( *(OldAttrib+0) > 1 ){
	wop->TailBackgroundPixmapObj =
	MakePixmapDependency(tool,*(OldAttrib+0),wop,0,0,&(wop->TailBackgroundPixmap));
}
break;
				case 2:
if( *(OldAttrib+2) == 0 && parent != NULL )
	*(OldAttrib+2) = *(parent->Attrib+2);
if( *(OldAttrib+2) > 0 )
	wop->TailBorderPixmapObj =
	MakePixmapDependency(tool,*(OldAttrib+2),wop,0,1,&(wop->TailBorderPixmap));
break;
				case 13:
if( *(OldAttrib+13) == 0 && parent != NULL )
	*(OldAttrib+13) =  *(parent->Attrib+13);
if( *(OldAttrib+13) > 0 )
	wop->TailColormap = MakeColormapDependency(tool,*(OldAttrib+13));
break;
				case 14:
if( *(OldAttrib+14) == 0 ) break;
wop->TailCursor = MakeCursorDependency(tool,*(OldAttrib+14));
			}
		}
	}

	/* Combining the masks */
	wop->Prefix->mask |= req->valueMask;
}

/*
 *	MapWindowFunc
 *
 *	Find the window and Mark the status as mapped
 *
 *	RETURN VALUE: void
 */

void MapWindowFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	WindowObj	*wop;
	Window	       wid;

	wid = ((xResourceReq *) (packet))->id;
	if( (wop = FindWindowByID(wid,tool->ToolInfo->WindowList->Next))
	    == NULL ){
		fprintf(stderr,"MapWindowFunc: can't find window\n");
		exit(1);
	}

	wop->State = 1; /* mapped */
}

/*
 *	UnmapWindowFunc
 *
 *	Find the window and Mark the status as unmapped
 *
 *	RETURN VALUE: void
 */

void UnmapWindowFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
        WindowObj	*wop;
        Window         wid;

        wid = ((xResourceReq *) (packet))->id;
        if( (wop = FindWindowByID(wid,tool->ToolInfo->WindowList->Next))
            == NULL ){
                fprintf(stderr,"UnmapWindowFunc: can't find window\n");
                exit(1);
        }

        wop->State = 0; /* unmapped */
}

/*
 *	MapSubwindowsFunc
 *
 *	Find the window and Mark the status of its children to mapped
 *
 *	RETURN VALUE: void
 */

void MapSubwindowsFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
        WindowObj	*wop;
        Window         wid;
	WindowObj	*Current;

        wid = ((xResourceReq *) (packet))->id;
        if( (wop = FindWindowByID(wid,tool->ToolInfo->WindowList->Next))
            == NULL ){
                fprintf(stderr,"MapSubwindowFunc: can't find window\n");
                exit(1);
        }

	for( Current = wop->Children->Next; Current != NULL;
	     Current = Current->Next)
		Current->State = 1;
}

/*
 *	UnmapSubwindowsFunc
 *
 *	Find the window and Mark the status of its children to unmapped
 *
 *	RETURN VALUE: void
 */

void UnmapSubwindowsFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
        WindowObj	*wop;
        Window         wid;
        WindowObj	*Current;

        wid = ((xResourceReq *) (packet))->id;
        if( (wop = FindWindowByID(wid,tool->ToolInfo->WindowList->Next))
            == NULL ){
         	fprintf(stderr,"UnmapSubwindowFunc: can't find window\n");
                exit(1);
        }

        for( Current = wop->Children->Next; Current != NULL;
             Current = Current->Next)
                Current->State = 0;
}

/*
 *	DestroyWindowFunc
 *
 *	Find the window and call RecDestroyWindow
 *
 *	RETURN VALUE: void
 */

void DestroyWindowFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	WindowObj	*wop;
        Window         wid;

        wid = ((xResourceReq *) (packet))->id;
        if( (wop = FindWindowByID(wid,tool->ToolInfo->WindowList->Next))
            == NULL ){
                fprintf(stderr,"DestroyWidnowFunc: can't find window\n");
                exit(1);
        }
	if( wop == tool->ToolInfo->WindowList->Next ){
		fprintf(stderr,"Trying to destroy the root window\n");
		return;
	}

	RecDestroyWindow(wop,tool);
}

/*
 *	DestroySubwindowsFunc
 *
 *	Find the window and call RecDestroyWindow for each child
 *
 *	RETURN VALUE: void
 */

void DestroySubwindowsFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
        WindowObj	*wop;
        Window         wid;
	WindowObj	*Current;

        wid = ((xResourceReq *) (packet))->id;
        if( (wop = FindWindowByID(wid,tool->ToolInfo->WindowList->Next))
            == NULL ){
                fprintf(stderr,"DestroySubwindowFunc: can't find window\n");
                exit(1);
        }

	/* for each subwindows: call RecDestroyWindow */
	for( Current = wop->Children->Next; Current != NULL;
             Current = wop->Children->Next)
                RecDestroyWindow(Current,tool);
}

/*
 *	Occludes
 *
 *	Find if window A occludes B
 *
 *	RETURN VALUE: 0 - false 1 - true
 */

Occludes(A,B)
WindowObj *A;
WindowObj *B;
{
	WindowObj *Current;
	int leftA,rightA,aboveA,belowA;
	int coordB[4][2];
	int i;

	if( A->State == 0 || B->State == 0 ) /* both have to be mapped */
		return(0);

	/* A should be above B */
	for( Current = A->Next; Current != B && Current != NULL;
	     Current = Current->Next );
	if( Current == NULL )
		return(0);

	leftA = A->Prefix->x - A->Prefix->borderWidth;
	rightA = A->Prefix->x + A->Prefix->width + A->Prefix->borderWidth;
	aboveA = A->Prefix->y - A->Prefix->borderWidth;
	belowA = A->Prefix->y + A->Prefix->height + A->Prefix->borderWidth;

	coordB[0][0] = B->Prefix->x - B->Prefix->borderWidth;
	coordB[0][1] = B->Prefix->y - B->Prefix->borderWidth;

	coordB[1][0] = B->Prefix->x + B->Prefix->width + B->Prefix->borderWidth;
	coordB[1][1] = B->Prefix->y - B->Prefix->borderWidth;

	coordB[2][0] = B->Prefix->x - B->Prefix->borderWidth;
	coordB[2][1] = B->Prefix->y + B->Prefix->height + B->Prefix->borderWidth;

	coordB[3][0] = B->Prefix->x + B->Prefix->width + B->Prefix->borderWidth;
	coordB[3][1] = B->Prefix->y + B->Prefix->height + B->Prefix->borderWidth;

	/* A and B should be overlapping */
	for( i=0; i<4; i++ )
		if( coordB[i][0] >= leftA && coordB[i][0] <= rightA &&
		    coordB[i][1] >= aboveA && coordB[i][1] <= belowA )
			return(1);

	return(0);
}

/*
 *	ConfigureWindowFunc
 *
 *	Processes ConfigureWindow request
 *
 *	RETURN VALUE: void
 */

void ConfigureWindowFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	WindowObj *wop;
	WindowObj *sibling;
	WindowObj *Current;
	xConfigureWindowReq *req;
	CARD32 *Attrib;
	int i,flag;

	req = (xConfigureWindowReq *) packet;

	if( (wop = FindWindowByID(req->window,tool->ToolInfo->WindowList->Next))
	    == NULL ){
		fprintf(stderr,"ConfigureWindowFunc: Trying to configure a non-registered window\n");
		exit(1);
	}

	Attrib = (CARD32 *)(packet+sizeof(xConfigureWindowReq));
	for( i=0; i<5; i++ )
		if( req->mask & 1<<i ){
			switch(i){
				case 0: wop->Prefix->x = *Attrib;
					break;
				case 1: wop->Prefix->y = *Attrib;
					break;
				case 2: wop->Prefix->width = *Attrib;
					break;
				case 3: wop->Prefix->height = *Attrib;
					break;
				case 4: wop->Prefix->borderWidth = *Attrib;
					break;
			}
			Attrib++;
		}

	if( req->mask & 1<<5 ){ /* sibling specified? */
		if( *Attrib == req->window ){ /* strange request */
			fprintf(stderr,"ConfigureWindowFunc: Sibling == window\n");
			return;
		}
		/* Find the sibling */
		for( sibling = wop->Parent->Children->Next; sibling != NULL;
		     sibling = sibling->Next )
			if( sibling->Prefix->wid == *Attrib ) break;
		if( sibling == NULL ){
			fprintf(stderr,"ConfigureWindowFunc: Sibling not found\n");
			return;
		}
		if( req->mask & 1<<6 ){
			fprintf(stderr,"ConfigureWindowFunc: stack mode not specified for sibling related operation\n");
			return;
		}
		Attrib++;
		switch(*Attrib){ /* stack-mode */
			case 0: /* Above */
				RemoveANode(wop);
				InsertNewNode(sibling->Prev,wop);
				break;
			case 1: /* Below */
				RemoveANode(wop);
				InsertNewNode(sibling,wop);
				break;
			case 2: /* TopIf */
			case 3: /* BottomIf */
			case 4: /* Opposite */
				if( (*Attrib == 2 || *Attrib == 4) &&
				    Occludes(sibling,wop) ){
					RemoveANode(wop);
					InsertNewNode(wop->Parent->Children,wop);
					break;
				}
				if( (*Attrib == 3 || *Attrib == 4) &&
				    Occludes(wop,sibling) ){
					RemoveANode(wop);
					/* Find the bottom */
					for( Current = sibling;
					     Current->Next != NULL;
					     Current = Current->Next );
					InsertNewNode(Current,wop);
				}
				break;
		}
	}
	else if( req->mask & 1<<6 ){
		switch(*Attrib){ /* stack-mode */
			case 0: /* Above */
				RemoveANode(wop);
				InsertNewNode(wop->Parent->Children,wop);
				break;
			case 1: /* Below */
				RemoveANode(wop);
				/* Find the bottom */
				for( Current = wop->Parent->Children;
				     Current->Next != NULL;
				     Current = Current->Next);
				InsertNewNode(Current,wop);
				break;
			case 2: /* TopIf */
			case 3: /* BottomIf */
			case 4: /* Opposite */
			    if( *Attrib == 2 || *Attrib == 4 ){
				flag = 0;
				for( Current = wop->Prev;
				     Current != wop->Parent->Children;
				     Current = Current->Prev)
					if( (flag = Occludes(Current,wop)) )
						break; /* from for loop */
				if( flag ){
					RemoveANode(wop);
					InsertNewNode(wop->Parent->Children,wop);
					break;
				}
			    }
			    if( *Attrib == 3 || *Attrib == 4){
				flag = 0;
				for( Current = wop->Next;
				     Current != NULL;
				     Current = Current->Next)
					if( (flag = Occludes(wop,Current)) )
						break; /* from for loop */
				if( flag ){
					/* Find the bottom */
					for( Current = wop->Next;
					     Current->Next != NULL;
					     Current = Current->Next );
					RemoveANode(wop);
					InsertNewNode(Current,wop);
				}
				break;
			    }
		}
	}
}

/*
 *	ReparentWindowFunc
 *
 *	Implement the ReparentWindow request
 *
 *	RETURN VALUE: void
 */

void ReparentWindowFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	xReparentWindowReq *req;
	WindowObj *wop;
	WindowObj *parent;

	req = (xReparentWindowReq *) (packet);
	if( (wop = FindWindowByID(req->window,tool->ToolInfo->WindowList->Next))
	    == NULL ){
		fprintf(stderr,"ReparentWindowFunc: Can't find the child window\n");
		exit(1);
	}
	if( (parent = FindWindowByID(req->parent,tool->ToolInfo->WindowList->Next))
	    == NULL ){
		fprintf(stderr,"ReparentWindowFunc: Can't find the parent window\n");
		exit(1);
	}

	RemoveANode(wop);
	wop->Prefix->x = req->x;
	wop->Prefix->y = req->y;

	InsertNewNode(parent->Children,wop);
	wop->Parent = parent;
	wop->Prefix->parent = req->parent;
}

/*
 *	CirculateWindowFunc
 *
 *	Handles CirculateWindow request
 *
 *	RETURN VALUE: void
 */

void CirculateWindowFunc(tool,packet,size)
Tool *tool;
char *packet;
CARD32 size;
{
	WindowObj *parent;
	WindowObj *child;
	WindowObj *sibling;
	xCirculateWindowReq *req;

	req = (xCirculateWindowReq *) packet;
	if( (parent = FindWindowByID(req->window,tool->ToolInfo->WindowList->Next))
	     == NULL ){
		fprintf(stderr,"CirculateWindowFunc: Can't find the parent window\n");
		exit(1);
	}

	if( parent->Children->Next == NULL ){ /* the parent have no child */
		return;
	}

	if( req->direction == 0 ){ /* RaiseLowest */
		/* find the lowest */
		for( child = parent->Children->Next; child->Next != NULL;
		     child = child->Next );
		/* for each child, find if it is occluded by a big sibling */
		for( ; child != parent->Children->Next; child = child->Prev ){
			for( sibling = child->Prev; sibling != parent->Children;
			     sibling = sibling->Prev ){
				if( Occludes(sibling,child) ){
					RemoveANode(child);
					InsertNewNode(parent->Children,child);
					break;
				}
			}
			if( sibling != parent->Children ) break;
		}
	}
	else{ /* LowerHighest */
		/* for each child, find if it occludes any younger sibling */
		for( child = parent->Children->Next; child->Next != NULL;
		     child = child->Next ){
			for( sibling = child->Next; sibling != NULL;
			     sibling = sibling->Next ){
				if( Occludes(child,sibling) ){
					RemoveANode(child);
					/* find the bottom */
					for( ; sibling->Next != NULL;
					       sibling = sibling->Next );
					InsertNewNode(sibling,child);
					break;
				}
			}
			if( sibling != NULL ) break;
		}
	}
}
