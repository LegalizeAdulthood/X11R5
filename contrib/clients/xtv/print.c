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
#include <stdio.h>

print_window(wop,indent)
WindowObj *wop;
int indent;
{
	blanks(indent);
	printf("Window: ID = %d WindowP = %d parentID = %d parentP = %d\n",
		wop->Prefix->wid,wop,wop->Prefix->parent,wop->Parent);
	blanks(indent+2);
	printf("State = %d\n",wop->State);
	blanks(indent+2);
	printf("HeadColormap: %d, HeadGC: %d, HeadPixmap: %d\n",
		wop->HeadColormap,wop->HeadGC,wop->HeadPixmap);

	if( wop->Prefix->mask & 1 << 6 ){
		blanks(indent+2);
		printf("Backing store: value = %d\n",*(wop->Attrib+6));
	}

	if( wop->Prefix->mask & 1 << 11 ){
		blanks(indent+2);
		printf("event-mask: %x\n",*(wop->Attrib+11));
	}

	if( wop->Prefix->mask & 1 << 12 ){
		blanks(indent+2);
		printf("do-not-pro: %x\n",*(wop->Attrib+12));
	}

	if( wop->Prefix->mask & 1 << 0 ){
		blanks(indent+2);
		printf("background pixmap: ID = %d PixP = %d PicP = %d\n",
			*(wop->Attrib+0),wop->TailBackgroundPixmapObj,
			wop->TailBackgroundPixmap);
	}

	if( wop->Prefix->mask & 1 << 1 ){
		blanks(indent+2);
		printf("background pixel: value = %d\n",*(wop->Attrib+1));
	}

	if( wop->Prefix->mask & 1 << 2){
		blanks(indent+2);
		printf("border pixmap: ID = %d PixP = %d PicP = %d\n",
			*(wop->Attrib+2),wop->TailBorderPixmapObj
			,wop->TailBorderPixmap);
	}

	if( wop->Prefix->mask & 1 << 3){
		blanks(indent+2);
		printf("border pixel: value = %d\n",*(wop->Attrib+3));
	}

	if( wop->Prefix->mask &  1 << 13){
		blanks(indent+2);
		printf("colormap: ID = %d CmapP = %d\n",
			*(wop->Attrib+13),wop->TailColormap);
	}

	if( wop->Prefix->mask &  1 << 14){
		blanks(indent+2);
		printf("cursor: ID = %d CursP = %d\n",
			*(wop->Attrib+14),wop->TailCursor);
	}

	blanks(indent+2);
	printf("PropertyPacketsSize = %d The Pointer = %d\n",
		wop->PropertyPacketsSize,wop->PropertyPackets);
}

rec_print_window(wop,indent)
WindowObj *wop;
int indent;
{
	WindowObj *Current;

	print_window(wop,indent);
	for( Current=wop->Children->Next;Current!=NULL;Current=Current->Next)
		rec_print_window(Current,indent+5);
}

print_pixmap(pmop)
PixmapObj *pmop;
{
	PixmapPacketNode *i;
	PixmapPacketNodeHead *j;

	printf("Pixmap: ID = %d PixmapP = %d\n",pmop->pid,pmop);
	printf("  State = %d dirtybit = %d depth = %d\n",
		pmop->State,pmop->dirty,pmop->depth);
	printf("  HeadPixmap: %d, HeadGC: %d\n",pmop->HeadPixmap,pmop->HeadGC);
	printf("  TailWindow: %d, TailPixmap: %d\n",
		pmop->TailWindow,pmop->TailPixmap);
	for(i=pmop->PixmapPacketNodeList->Next;i!=NULL;i=i->Next){
		printf("     PutImagePacketP = %d MyAddr = %d\n",
			i->PutImagePacket,i);
		printf("       HeadWindows:\n");
		for(j=i->HeadWindow;j!=NULL;j=j->Next)
			printf("        WindowP = %d AsWhat = %d MyAddr = %d\n",
				j->User,j->AsWhat,j);
		printf("       HeadGC:\n");
		for(j=i->HeadGC;j!=NULL;j=j->Next)
			printf("        GCP = %d AsWhat = %d MyAddr = %d\n",
				j->User,j->AsWhat,j);
		printf("       HeadCursor:\n");
		for(j=i->HeadCursor;j!=NULL;j=j->Next)
                        printf("        CursorP = %d AsWhat = %d MyAddr = %d\n",
				j->User,j->AsWhat,j);
	}
}

print_colormap(cmop)
ColormapObj *cmop;
{
	ColormapHeadNode *i;

	printf("Colormap: ID = %d ColormapP = %d\n",cmop->cid,cmop);
	printf("  State = %d\n",cmop->State);
	printf("  HeadColormap:\n");
	for(i=cmop->HeadColormap->Next;i!=NULL;i=i->Next)
		printf("     User = %d WhereAt = %d MyAddr = %d\n",
			i->User,i->WhereAt,i);
	printf("  HeadWindow: Number of = %d\n",cmop->HeadWindow);
	printf("  TailColormap: PtoHeadNode = %d PtoObj = %d\n",
		cmop->TailColormap,cmop->TailColormapObj);
	printf("  TailWindow: %d\n",cmop->TailWindow);
	printf("  PacketsSize = %d\n  ICNP = %d\n",
		cmop->PacketsSize,cmop->ICNentry);
}

print_ICN(tool)
Tool *tool;
{
	InstalledColormapNode *i;

	printf("Installed colormap id's:");
	for(i=tool->ToolInfo->ICList->Next;i!=NULL;i=i->Next)
		printf(" %d",i->cid);
	printf("\n");
}

print_GC(gcop)
GCObj *gcop;
{
	int i;

	printf("GC: ID = %d GCP = %d\n",gcop->gcid,gcop);
	printf("  State = %d\n",gcop->State);
	printf("  TailWindow: %d, TailPixmap: %d\n",
		gcop->TailWindow,gcop->TailPixmap);

	if( gcop->Prefix->mask & 1 << 10 )
		printf("  TailTile: ID = %d TailTileP = %d TailTileObjP = %d\n",
			*(gcop->Attrib+10),gcop->TailTile,gcop->TailTileObj);
	if(gcop->Prefix->mask & 1 << 11 )
		printf("  TailStipple: ID = %d TailStippleP = %d TailStippleObjP = %d\n",
			*(gcop->Attrib+11),gcop->TailStipple,gcop->TailStippleObj);
	if(gcop->Prefix->mask & 1 << 14 )
		printf("  TailFont: ID = %d TailFontP = %d\n",
			*(gcop->Attrib+14),gcop->TailFont);
	if(gcop->Prefix->mask & 1 << 19 )
		printf("  TailClippingMask: ID = %d TailClippingMaskP = %d TailClippingMaskObjP = %d\n",
			*(gcop->Attrib+19),gcop->TailClippingMask,gcop->TailClippingMaskObj);
	printf("  SetClipRectanglesPacketP = %d\n  SetDashesPacket = %d\n",
		gcop->SetClipRectanglesPacket,gcop->SetDashesPacket);

	for( i=0; i<23; i++ ){
		if( gcop->Prefix->mask & 1 << i ){
			printf("%d %d\n",i+1,*(gcop->Attrib+i));
		}
	}
}

print_font(fop)
FontObj *fop;
{
	printf("Font: ID = %d FontP = %d\n",fop->fid,fop);
	printf("  State = %d\n",fop->State);

	printf("  HeadCursor: Number of = %d\n",fop->HeadCursor);
	printf("  HeadGC: Number of = %d\n",fop->HeadGC);
	printf("  OpenPacketP = %d SetPathPacketP = %d\n",
		fop->OpenPacket,fop->SetPathPacket);
}

print_cursor(cop)
CursorObj *cop;
{
	printf("Cursor: ID = %d CursorP = %d\n",cop->cid,cop);
	printf("  State = %d\n",cop->State);
	printf("  HeadWindow: Number of = %d\n",cop->HeadWindow);
	printf("  PixmapCursor? = %d\n",cop->pixmapcursor);
	printf("  TailsourceP = %d TailsourcePixmapP = %d\n",
		cop->Tailsource,cop->TailsourcePixmap);
	printf("  TailmaskP = %d TailmaskPixmapP = %d\n",
		cop->Tailmask,cop->TailmaskPixmap);
	printf("  CreatePacketP = %d RecolorPacketP = %d\n",
		cop->CreatePacket,cop->RecolorPacket);
}

blanks(n)
int n;
{
	int i;

	for( i=0; i<n; i++)
		printf(" ");
}

ReportToolStat(tool)
Tool *tool;
{
	CursorObj *cursori;
	FontObj *fonti;
	GCObj *gci;
	PixmapObj *pixmapi;
	ColormapObj *colormapi;
	int i;

	rec_print_window(tool->ToolInfo->WindowList->Next,0);
	printf("\n");
	for( gci = tool->ToolInfo->GCList->Next; gci != NULL; gci = gci->Next )
		print_GC(gci);
	printf("\n");
	for( colormapi = tool->ToolInfo->ColormapList->Next; colormapi != NULL;
		colormapi = colormapi->Next )
		print_colormap(colormapi);
	printf("\n");
	print_ICN(tool);
	printf("\n");
	for( cursori = tool->ToolInfo->CursorList->Next; cursori != NULL;
		cursori = cursori->Next )
		print_cursor(cursori);
	printf("\n");
	for( pixmapi = tool->ToolInfo->PixmapList->Next; pixmapi != NULL;
		pixmapi = pixmapi->Next)
		print_pixmap(pixmapi);
	printf("\n");
	for( fonti = tool->ToolInfo->FontList->Next; fonti != NULL;
		fonti = fonti->Next)
		print_font(fonti);
	printf("\n");
	printf("PixmapDepths: ");
	for(i=0; i<65; i++)
		printf("%d ",(tool->ToolInfo->PAG)[i].number);
	printf("\n\n");
	printf("InternAtomSize = %d SaveAllsSize = %d ThisToolDead = %d\n",
	tool->ToolInfo->InternalAtomSize,tool->ToolInfo->SaveAllsSize,
	tool->ThisToolDead);
	fflush(stdout);
}
