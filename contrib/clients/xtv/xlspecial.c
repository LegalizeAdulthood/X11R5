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
	xlspecial.c
	Special routines for trsnlating special packets

*/

#define	DEBUGSpecials	0


#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include "tool.h"
#include "xpacket.h"
#include "xlspecial.h"

#define	TRUE	1
#define	FALSE	0
extern  int     XCOLOR;

typedef struct _VListEntry {
	char   FieldName[30];	/* Name of field for debugging */
	BOOL   Translate;		/* Whether or not it gets translated */
	BOOL   HandleZero;	/* Whether or not we handle the 0/1 case */
} VListEntry;

/* Definition of tables for translating LISTofVALUES entries in Window  Requests */

VListEntry   WinVTable[32] = {
	{ "BackgroundPixmap",	TRUE,	TRUE	},
	{ "BackgroundPixel",	TRUE,	FALSE	},
	{ "BorderPixmap",	TRUE,	TRUE	},
	{ "BorderPixel",	TRUE,	FALSE	},
	{ "BitGravity",		FALSE,	FALSE	},
	{ "WinGravity",		FALSE,	FALSE	},
	{ "BackingStore",	FALSE,	FALSE	},
	{ "BackingPlanes",	FALSE,	FALSE	},
	{ "BackingPixel",	FALSE,	FALSE	},
	{ "OverrideRedirect",	FALSE,	FALSE	},
	{ "SaveUnder",		FALSE,	FALSE	},
	{ "EventMask",		FALSE,	FALSE	},
	{ "DNPMask",		FALSE,	FALSE	},
	{ "Colormap",		TRUE,	TRUE	},
	{ "Cursor",		TRUE,	TRUE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
};

/* Definition of tables for translating LISTofVALUES entries in GC Requests */

VListEntry   GCVTable[32] = {
	{ "Function",		FALSE,	FALSE	},
	{ "PlaneMask",		FALSE,	FALSE	},
	{ "Foreground",		TRUE,	FALSE	},
	{ "Background",		TRUE,	FALSE	},
	{ "LineWidth",		FALSE,	FALSE	},
	{ "LineStyle",		FALSE,	FALSE	},
	{ "CapStyle",		FALSE,	FALSE	},
	{ "JoinStyle",		FALSE,	FALSE	},
	{ "FillStyle",		FALSE,	FALSE	},
	{ "FillRule",		FALSE,	FALSE	},
	{ "Tile",		TRUE,	FALSE	},
	{ "Stipple",		TRUE,	FALSE	},
	{ "TileStippleXOrigin",	FALSE,	FALSE	},
	{ "TileStippleYOrigin",	FALSE,	FALSE	},
	{ "Font",		TRUE,	FALSE	},
	{ "SubWindowMode",	FALSE,	FALSE	},
	{ "GraphicsExposures",	FALSE,	FALSE	},
	{ "ClipXOrigin",	FALSE,	FALSE	},
	{ "ClipYOrigin",	FALSE,	FALSE	},
	{ "ClippingMask",	TRUE,	TRUE	},
	{ "DashOffset",		FALSE,	FALSE	},
	{ "Dashes",		FALSE,	FALSE	},
	{ "ArcMode",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,	FALSE	},
	{ "UNDEFINED",		FALSE,  FALSE	}
};




/*
 *   void FixValue
 *
 *   Translate the contents of a LISTofVALUE entry in a CreateGC
 *   or ChangeGC request based on the value of BitMask.
 *
 *   RETURN VALUE:   (void)
 */
void FixValue( BitMask, ValueList, Table, WhatTool )
CARD32 BitMask;
XID *ValueList;
VListEntry *Table;
Tool *WhatTool;
{
	int	BitNumber;
	int	WhereAt = 0;

/* chungg_bugfix */

	BitMask = SwapLong(BitMask,WhatTool->ConnectPrefix.byteOrder);

	for ( BitNumber=0; BitNumber < 32; BitNumber++ ) {
		if ( BitMask & (1 << BitNumber) ) {
#			if DEBUGSpecials
			fprintf( stderr, "FixValue(): Field %s present",
			    Table[BitNumber].FieldName );
#			endif

			if ( (Table[BitNumber].Translate) && 
			    (!(Table[BitNumber].HandleZero) ||
/* chungg_bugfix */
			     !(SwapLong(ValueList[WhereAt],WhatTool->ConnectPrefix.byteOrder) <= 1)) ) {
				ValueList[WhereAt] = TranslateForward( WhatTool, ValueList[WhereAt] );
#				if DEBUGSpecials
				fprintf( stderr, "  -- TRANSLATED\n" );
#				endif
			} else {
#				if DEBUGSpecials
				fprintf( stderr, "\n" );
#				endif
			}
			WhereAt++;
		}
	}
}


/*
 *   void XlateCreateGC
 *
 *   Complex translation for CreateGC (55)
 *
 *   RETURN VALUE:   (void)
 */
void XlateCreateGC( Packet, WhatTool )
BYTE *Packet;
Tool *WhatTool;
{
#	if DEBUGSpecials
	fprintf( stderr, "XlateCreateGC()\n" );
#	endif
	FixValue(	((xCreateGCReq *)Packet)->mask,		/* Bitmask */
	(XID *)(Packet+sizeof(xCreateGCReq)),	/* Pointer to ValueList */
	GCVTable,				/* Pointer to xlate table */
	WhatTool );				/* Pointer to tool structure */
}



/*
 *   void XlateChangeGC
 *
 *   Complex translation for ChangeGC (56)
 *
 *   RETURN VALUE:   (void)
 */
void XlateChangeGC( Packet, WhatTool )
BYTE *Packet;
Tool *WhatTool;
{
#	if DEBUGSpecials
	fprintf( stderr, " XlateChangeGC()\n" );
#	endif
	FixValue(	((xChangeGCReq *)Packet)->mask,		/* Bitmask */
	(XID *)(Packet+sizeof(xChangeGCReq)),	/* Pointer to ValueList */
	GCVTable,				/* Pointer to xlate table */
	WhatTool );				/* Pointer to tool structure */
}



/*
 *   void XlateCreateWindow
 *
 *   Complex translation for CreateWindow (01)
 *
 *   RETURN VALUE:   (void)
 */
void XlateCreateWindow( Packet, WhatTool )
BYTE *Packet;
Tool *WhatTool;
{

#	if DEBUGSpecials
	fprintf( stderr, "XlateCreateWindow()\n" );
#	endif
	FixValue(	((xCreateWindowReq *)Packet)->mask,		/* Bitmask */
	(XID *)(Packet+sizeof(xCreateWindowReq)),	/* Pointer to ValueList */
	WinVTable,					/* Pointer to xlate table */
	WhatTool );					/* Pointer to tool structure */

	/* Check if the depth of window to be created is supported by the server */

	if ( ((xCreateWindowReq *)Packet)->depth > WhatTool->LocalRootInfo.nDepths ) {
		((xCreateWindowReq *)Packet)->depth = WhatTool->LocalRootInfo.rootDepth;
	}
}

void XlateCreatePixmap( Packet, WhatTool )
BYTE *Packet;
Tool *WhatTool;
{
#	if DEBUGSpecials
	fprintf( stderr, "XlateCreatePixmap()\n" );
#	endif
	/* Check if the depth of the pixmap to be created is supported by the server */

	if ( ((xCreateWindowReq *)Packet)->depth > WhatTool->LocalRootInfo.nDepths ) {
		((xCreateWindowReq *)Packet)->depth = WhatTool->LocalRootInfo.rootDepth;
	}

}


/*
 *   void XlateChangeWindowAttr
 *
 *   Complex translation for ChangeWindowAttributes (02)
 *
 *   RETURN VALUE:   (void)
 */
void XlateChangeWindowAttr( Packet, WhatTool )
BYTE *Packet;
Tool *WhatTool;
{
#	if DEBUGSpecials
	fprintf( stderr, "XlateChangeWindowAttr()\n" );
#	endif
	FixValue(	((xChangeWindowAttributesReq *)Packet)->valueMask,	/* Bitmask */
	(XID *)(Packet+sizeof(xChangeWindowAttributesReq)),	/* Ptr to ValueList */
	WinVTable,						/* Ptr to xlate table */
	WhatTool );						/* Ptr to tool structure */

}


/*
 *   void XlateConfigWindow
 *
 *   Complex translation for ConfigureWindow (12)
 *
 *   RETURN VALUE:   (void)
 */
void XlateConfigWindow( Packet, WhatTool )
BYTE *Packet;
Tool *WhatTool;
{
	xConfigureWindowReq	*Request = (xConfigureWindowReq *)Packet;
	XID			*AttrTable = (XID *)(Packet+sizeof(xConfigureWindowReq));
	int			i;
	CARD16			tmp_mask;

#	if DEBUGSpecials
	fprintf( stderr, "XlateConfigWindow(()\n" );
#	endif

/* chungg_bugfix */
	tmp_mask = SwapInt(Request->mask,WhatTool->ConnectPrefix.byteOrder);
	for ( i=0; i < 16; i++ ) {
		if ( (tmp_mask & (1 << i)) ) {
			AttrTable[i] = TranslateForward( WhatTool, AttrTable[i] );
		}
	}
}


/*
 *   void XlateRotateProps
 *
 *   Complex translation for RotateProperties (114)
 *
 *   RETURN VALUE:   (void)
 */
void XlateRotateProps( Packet, WhatTool )
BYTE *Packet;
Tool *WhatTool;
{
	xRotatePropertiesReq	*Request = (xRotatePropertiesReq *)Packet;
	XID			*AtomList = (XID *)(Packet + sizeof(xRotatePropertiesReq));
	int			i;
	CARD16			nAtoms;


#	if DEBUGSpecials
	fprintf( stderr, "XlateRotateProps()\n" );
#	endif

/* chungg_bugfix */
	nAtoms = SwapInt(Request->nAtoms,WhatTool->ConnectPrefix.byteOrder);
	for ( i=0; i < nAtoms ; i++ ) {
		AtomList[i] = TranslateForward( WhatTool, AtomList[i] );
	}
}



#define ROUNDUP(nbytes, pad)  ((((nbytes) + ((pad) - 1)) / (pad)) * (pad))

/*
 *   void XlatePutImage
 *
 *   Complex translation for PutImage (72)
 *
 *   RETURN VALUE:      (void)
 *
 */
void XlatePutImage( Packet, WhatTool )
BYTE *Packet;
Tool *WhatTool;
{
    if (XCOLOR) {
        xPutImageReq    *Request = (xPutImageReq *)Packet;
        BYTE            *Data = (BYTE *)(Packet + sizeof(xPutImageReq));
        XImage          *image = (XImage *)malloc( sizeof(XImage) );
        CARD32          pixelValue;
        register int    j;
        register int    x, y;


        image->width = SwapInt(Request->width, WhatTool->ConnectPrefix.byteOrder);
        image->height = SwapInt(Request->height, WhatTool->ConnectPrefix.byteOrder);
        image->format = Request->format;
        image->byte_order = WhatTool->HostConnectInfo.imageByteOrder;
        image->bitmap_unit =  WhatTool->HostConnectInfo.bitmapScanlineUnit;
        image->bitmap_bit_order = WhatTool->HostConnectInfo.bitmapBitOrder;
        image->red_mask = image->green_mask = image->blue_mask = 0;
        if (image->format == ZPixmap) {
                register xPixmapFormat *fmt = WhatTool->LocalPixmapFormats;
                register int i;

                for (i = WhatTool->HostConnectInfo.numFormats + 1; --i; ++fmt)
                        if (fmt->depth == Request->depth) {
                                image->bits_per_pixel = fmt->bitsPerPixel;
                                break;  /* actually break out of for-loop */
                        }
        }
        image->xoffset = Request->leftPad;
        image->bitmap_pad = WhatTool->HostConnectInfo.bitmapScanlinePad;
        image->depth = Request->depth;
        image->data = (char *)Data;
        if (image->format == ZPixmap)
                image->bytes_per_line =
                        ROUNDUP((image->bits_per_pixel * image->width),
                                                image->bitmap_pad) >> 3;
        else
                image->bytes_per_line =
                         ROUNDUP((image->width + image->xoffset),
                                                image->bitmap_pad) >> 3;
        image->obdata = NULL;
        _XInitImageFuncPtrs (image);

        for (x=0; x < image->width; x++)
                for (y =0; y < image->height; y++)
        {
			pixelValue = XGetPixel(image, x, y);
                        for (j = 0; j < WhatTool->NumColorCells; j++)
                                if (pixelValue == WhatTool->ColorCellList[j]) {
                                        pixelValue = WhatTool->LocalColorCellList[j];
                                        break;
                                }
                        XPutPixel(image, x, y, pixelValue);
        }
    }
}



/*
 *   void XlateStoreColors
 *
 *   Complex translation for StoreColors (89)
 *
 *   RETURN VALUE:      (void)
 *
 */
void XlateStoreColors( Packet, WhatTool )
BYTE *Packet;
Tool *WhatTool;
{
        xStoreColorsReq         *Request = (xStoreColorsReq *)Packet;
        CARD32                  *ColorItemList = (CARD32 *)(Packet + sizeof(xStoreColorsReq));
        int                     i;
        CARD16                  nColorItems;

#       if DEBUGSpecials
        fprintf( stderr, "XlateStoreColors()\n" );
#       endif

        if (Request->cmap == WhatTool->HostRootInfo.defaultColormap) {
           nColorItems = (SwapInt(Request->length, WhatTool->ConnectPrefix.byteOrder)-2) / 3;
           for ( i=0; i < nColorItems ; i++ ) {
                   ColorItemList[3*i] = TranslateForward( WhatTool, ColorItemList[3*i] );
           }
	}
}


/*
 *   void XlateQueryColors
 *
 *   Complex translation for QueryColors (91)
 *
 *   RETURN VALUE:      (void)
 *
 */
void XlateQueryColors( Packet, WhatTool )
BYTE *Packet;
Tool *WhatTool;
{
        xQueryColorsReq         *Request = (xQueryColorsReq *)Packet;
        CARD32                  *PixelList = (CARD32 *)(Packet + sizeof(xQueryColorsReq));
        int                     i;
        CARD16                  nPixels;

#       if DEBUGSpecials
        fprintf( stderr, "XlateQueryColors()\n" );
#       endif

        if (Request->cmap == WhatTool->HostRootInfo.defaultColormap) {
           nPixels = SwapInt(Request->length, WhatTool->ConnectPrefix.byteOrder)-2;
           for ( i=0; i < nPixels ; i++ ) {
                   PixelList[i] = TranslateForward( WhatTool, PixelList[i] );
           }
	}
}


void ForceDropDead()
{
#	if DEBUGSpecials
	fprintf( stderr, "ForceDropDead()\n" );
#	endif
}


/*----------------------------------------------------------------------*
 *									*
 * Following are the routines that perform complex replacement of XIDs  *
 * on reply from X server.						*
 * 									*
 *----------------------------------------------------------------------*/

void XlateQueryTree (buf,  WhatTool)
BYTE *buf;
Tool *WhatTool;

{
	short n; /* number of windows in children */
	XID *XID_ptr;
	int i;

/* chungg_bugfix */
	n = SwapInt( (short) *((short *)(buf + 16)), WhatTool->ConnectPrefix.byteOrder);
	XID_ptr = (XID *) (buf + 32);

	for (i = 0; i < n; i++) {
		*XID_ptr = TranslateBackward (WhatTool, *XID_ptr);
		XID_ptr++;
	}
}

void XlateListProp (buf,WhatTool)
BYTE *buf;
Tool *WhatTool;

{
	short n; /* number of windows in children */
	XID *XID_ptr;
	int i;

/* chungg_bugfix */
	n = SwapInt( (short) *((short *)(buf + 8)), WhatTool->ConnectPrefix.byteOrder);
	XID_ptr = (XID *) (buf + 32);

	for (i = 0; i < n; i++) {
		*XID_ptr = TranslateBackward (WhatTool, *XID_ptr);
		XID_ptr++;
	}
}

void XlateListFonts (buf, WhatTool)
BYTE *buf;
Tool *WhatTool;

{
	short m; /* number of FONTPROPs in properties */
	XID *XID_ptr;
	int i;

/* chungg_bugfix */
	m = SwapInt( (short) *((short *)(buf + 46)), WhatTool->ConnectPrefix.byteOrder);
	XID_ptr = (XID *) (buf + 64);

	for (i = 0; i < m; i++) {
		*XID_ptr = TranslateBackward (WhatTool, *XID_ptr);
		XID_ptr++;
		XID_ptr++;
	}
}

void XlateColormaps (buf,WhatTool)
BYTE *buf;
Tool *WhatTool;

{
	short n; /* number of COLORMAPs in cmaps */
	XID *XID_ptr;
	int i;

	n = SwapInt( (short) *((short *)(buf + 8)), WhatTool->ConnectPrefix.byteOrder);
	XID_ptr = (XID *) (buf + 32);

	for (i = 0; i < n; i++) {
		*XID_ptr = TranslateBackward (WhatTool, *XID_ptr);
		XID_ptr++;
	}
}
