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

/*
	objects.h

		Header file to define data structures to maintain the server
		state for each tool used

*/
#include <X11/Xproto.h>
#include <malloc.h>
#include <stdio.h>

#ifndef OBJECTS__H
#define OBJECTS__H

#include "memory.h"
#include "misc.h"

typedef struct _CursorObj {

	Cursor	cid;		/* Cursor's ID */
	CARD32	State;		/* 1 - Freed, 0 - Not freed */

	CARD32	HeadWindow;	/* Number of Windows using this cursor */

	/* source and mask of the cursor */
	CARD8	pixmapcursor;	/* 1 - pixmapcursor, 0 - glyphcursor */
	void	*Tailsource;	/* cast to PixmapPacketNode */
	void	*Tailmask;	/* or FontObj */
	void	*TailsourcePixmap; /* cast to PixmapObj */
	void	*TailmaskPixmap;

	char	*CreatePacket;	/* Actual packet to create the cursor */
	char	*RecolorPacket; /* The last one of these packets */

	struct	_CursorObj	*Prev;
	struct	_CursorObj	*Next;
} CursorObj;
	
typedef struct _FontObj {
	Font	fid;		/* Font's ID */
	CARD32	State;		/* 0 - opened, 1 - closed */

	CARD32	HeadCursor;	/* Cursors depending on this font */
	CARD32	HeadGC;		/* GC depending on this font */

	char	*OpenPacket;	/* packet containing OpenFont request */
	char	*SetPathPacket;

	struct	_FontObj	*Prev;
	struct	_FontObj	*Next;
} FontObj;

typedef struct _GCObj {
	GContext	gcid;	/* GC ID */
	CARD32		State;	/* 1 - Freed, 0 - Not freed */

	xCreateGCReq	*Prefix;/* packet containing CreateGC request */
	CARD32		*Attrib;/* attributes of the GC */

	CARD32		Depth;

	/* objects this GC depends on */

	/* For drawable field of the packet */
	void		*TailWindow;		/* cast to WindowObj */
	void		*TailPixmap;		/* cast to PixmapObj */
	/* Tile */
	void		*TailTile;		/* cast to PixmapPacketNode */
	void		*TailTileObj;		/* cast to PixmapObj */
	/* Stipple */
	void		*TailStipple;		/* cast to PixmapPacketNode */
	void		*TailStippleObj;	/* cast to PixmapObj */
	/* Clip Mask */
	void		*TailClippingMask;	/* cast to PixmapPacketNode */
	void		*TailClippingMaskObj;	/* cast to PixmapObj */
	/* Font */
	FontObj		*TailFont;

	char		*SetClipRectanglesPacket; /* The last one of these */
	char		*SetDashesPacket;	  /* The last one of these */

	struct	_GCObj	*Prev;
	struct	_GCObj	*Next;
} GCObj;

typedef struct _PixmapPacketNodeHead {
	void	*User;	/* To be cast to WindowObj 0, GCObj 1, CursorObj 2*/
	CARD32	AsWhat; /* 0(Window)- 0 == background, 1 == border */
			/* 1(GC)    - 0 == tile, 1 == stipple, 2 == clip-mask */
			/* 2(Cursor)- 0 == source, 1 == mask */
	struct	_PixmapPacketNodeHead	*Next;
} PixmapPacketNodeHead;

typedef struct _PixmapPacketNode {
	char			*PutImagePacket;/* PutImage Packets that will draw the pixmap */
	CARD32			PutImagePacketSize;

	/* Objects using this drawing of the pixmap */
	PixmapPacketNodeHead	*HeadWindow; /* Windows using current picture */
	PixmapPacketNodeHead	*HeadGC;     /* GC's using current picture */
	PixmapPacketNodeHead	*HeadCursor; /* Cursors using current picture */

	struct	_PixmapPacketNode	*Prev;
	struct	_PixmapPacketNode	*Next;
} PixmapPacketNode;

typedef struct _PixmapObj {
	Pixmap			pid;		/* pixmap id */
	CARD32			State;		/* 1 - Freed, 0 - Not freed */

	/* Objects depending on this pixmap */
	CARD32			HeadPixmap;	/* Number of pixmaps */
	CARD32			HeadGC;		/* Number of GC's */

	/* Objects this pixmap depends on */
	void			*TailWindow;	/* cast to WindowObj */
	struct	_PixmapObj	*TailPixmap;

	PixmapPacketNode	*PixmapPacketNodeList;

	CARD8			depth;
	CARD16			width;
	CARD16			height;

	CARD8			dirty;		/* 0 - clean; 1 - dirty */

	struct	_PixmapObj	*Prev;
	struct	_PixmapObj	*Next;
} PixmapObj;

typedef struct _InstalledColormapNode {
	Colormap			cid;	/* Installed colormap ID */

	struct	_InstalledColormapNode	*Prev;
	struct	_InstalledColormapNode	*Next;
} InstalledColormapNode;

typedef struct _ColormapHeadNode {
	void				*User;	/* to be cast to ColormapObj */
	CARD32				WhereAt;/* Position in the archive */
	struct	_ColormapHeadNode	*Prev;
	struct	_ColormapHeadNode	*Next;
} ColormapHeadNode;

typedef struct _ColormapObj {
	Colormap		cid;		/* colormap id */
	CARD32			State;		/* 1 - Freed, 0 - Not Freed */

	/* Objects depending on this colormap */
	ColormapHeadNode	*HeadColormap;	/* Colormap depending on this colormap */
	CARD32			HeadWindow;	/* Window using this colormap */

	/* Objects this colormap depends on */
	ColormapHeadNode	*TailColormap;	/* Colormap this cmap depends on */
	struct	_ColormapObj	*TailColormapObj;
	void			*TailWindow;	/* Cast to WindowObj */

	char			*Packets;	/* series of simple colormap packets */
	CARD32			PacketsSize;
	char			*AllocPackets;	/* XTV protocol AllocColor packets */
	CARD32			AllocPacketsSize;
	InstalledColormapNode	*ICNentry;	/* pointer to the ICNode if this colormap is installed */

	struct	_ColormapObj	*Prev;
	struct	_ColormapObj	*Next;
} ColormapObj;

typedef struct _WindowObj {
	xCreateWindowReq *Prefix;		/* Create Window packet */
	CARD32		 *Attrib;		/* Window Attributes */
	CARD32		 State;			/* 0 - unmapped */
						/* 1 - mapped */
						/* 2 - destroyed */
						/* 3 - yellow node */

	/* Objects depending on this Window */
	CARD32		 HeadColormap;
	CARD32		 HeadGC;
	CARD32		 HeadPixmap;
	
	/* Objects this window depends on */
	PixmapPacketNode *TailBackgroundPixmap; /* Background */
	PixmapObj	 *TailBackgroundPixmapObj;
	PixmapPacketNode *TailBorderPixmap;	/* Border */
	PixmapObj	 *TailBorderPixmapObj;
	ColormapObj	 *TailColormap;		/* colormap */
	CursorObj	 *TailCursor;		/* Cursor */

	/* Other packets related to this window */
	char		 *PropertyPackets;
	int		 PropertyPacketsSize;
	char		 *PutImagePacket;
	CARD32		 PutImagePacketSize;
	char		 *GrabRequests;
	CARD32		 GrabRequestsSize;

	struct	_WindowObj	 *Parent;
	struct	_WindowObj	 *Prev;
	struct	_WindowObj	 *Next;
	struct	_WindowObj	 *Children;
} WindowObj;

struct PixmapAndGC{
	CARD32 number;	/* number of pixmap or window of the depth */
	CARD32 gcid;	/* GC usable for the depth */
};

typedef struct _ToolState {
	CursorObj	*CursorList;	/* Cursors used */
	FontObj		*FontList;	/* Fonts used */
	GCObj		*GCList;	/* GC's */
	PixmapObj	*PixmapList;	/* Pixmaps */
	ColormapObj	*ColormapList;	/* Colormaps */
	InstalledColormapNode *ICList;	/* Installed Colormaps */
	WindowObj	*WindowList;	/* Windows */
	WindowObj	*Destroyeds;	/* Destoryed windows with 'alive' objects depending on them */
	char		*InternalAtom;	 /* InternAtom packets */
	CARD32		InternalAtomSize;
	char		*SaveAlls;	 /* other misc. packets to be sent */
	CARD32		SaveAllsSize;
	struct PixmapAndGC PAG[65];	 /* list of number of pixmaps or windows for each index depth */
	CARD32		RecentResourceID;/* The most recently used ID */
} ToolState;

/* Macro used to insert a new node to a place in a doubly linked list */
#define InsertNewNode(Pred,New)\
        New->Next = Pred->Next;\
        New->Prev = Pred;\
        if( New->Next != NULL )\
                New->Next->Prev = New;\
        Pred->Next = New;

/* Macro used to remove a node from a doubly linked list */
#define RemoveANode(node)\
if( node->Next != NULL )\
        node->Next->Prev = node->Prev;\
node->Prev->Next = node->Next;

#endif /* OBJECTS__H */
