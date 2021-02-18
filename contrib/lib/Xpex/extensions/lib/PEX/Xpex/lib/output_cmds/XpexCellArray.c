/* $Header: XpexCellArray.c,v 2.4 91/09/11 16:06:33 sinyaw Exp $ */

/*****************************************************************
Copyright 1991 by Sony Microsystems Company, San Jose, California

					All Rights Reserved

Permission to use, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sony not be used
in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

SONY DISCLAIMS ANY AND ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL EXPRESS WARRANTIES AND ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, FOR A PARTICULAR PURPOSE. IN NO EVENT
SHALL SONY BE LIABLE FOR ANY DAMAGES OF ANY KIND, INCLUDING BUT NOT
LIMITED TO SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING FROM
LOSS OF USE, DATA OR LOSS OF ANY PAST, PRESENT, OR PROSPECTIVE PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.
*****************************************************************/

#include "Xpexlib.h"
#include "Xpexlibint.h"
#include <X11/extensions/Xext.h>
#include "extutil.h"
#include "Xpextutil.h"

static void pack_colors();

void
XpexCellArray(dpy, pt1, pt2, pt3, dx, dy, color_indices)
	Display *dpy;
	XpexCoord3D *pt1;
	XpexCoord3D *pt2;
	XpexCoord3D *pt3;
	int dx;
	int dy;
	XpexTableIndex *color_indices;
{
	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);
	XpexCheckLastReq(dpy,i);

	LockDisplay(dpy);
	{
		pexCellArray *oc;
		int count = dx * dy;
		int extra = count * sizeof(pexTableIndex);
		extra += PADDING(extra);
		XpexGetOutputCmdExtra(CellArray, dpy, i,extra, oc);
		oc->point1.x = pt1->x;
		oc->point1.y = pt1->y;
		oc->point1.z = pt1->z;
		oc->point2.x = pt2->x;
		oc->point2.y = pt2->y;
		oc->point2.z = pt2->z;
		oc->point3.x = pt3->x;
		oc->point3.y = pt3->y;
		oc->point3.z = pt3->z;
		oc->dx = (CARD32) dx;
		oc->dy = (CARD32) dy;
		bcopy((char *)color_indices,(char *)(oc + 1),
		count * sizeof(pexTableIndex));
	}
	UnlockDisplay(dpy);
}

void
XpexCellArray2D(dpy, pt1, pt2, dx, dy, color_indices)
	Display *dpy;
	XpexCoord2D *pt1;
	XpexCoord2D *pt2;
	int dx;
	int dy;
	XpexTableIndex *color_indices;
{
	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);
	XpexCheckLastReq(dpy,i);

	LockDisplay(dpy);
	{
		pexCellArray2D *oc;
		int count = dx * dy;
		int extra = count * sizeof(pexTableIndex);
		extra += PADDING(extra);
		XpexGetOutputCmdExtra(CellArray2D, dpy, i, extra, oc);
		oc->point1.x = pt1->x;
		oc->point1.y = pt1->y;
		oc->point2.x = pt2->x;
		oc->point2.y = pt2->y;
		oc->dx = (CARD32) dx;
		oc->dy = (CARD32) dy;
		bcopy((char *)color_indices,(char *)(oc + 1),
		count * sizeof(pexTableIndex));
	}
	UnlockDisplay(dpy);
}

void
XpexExtCellArray(dpy, pt1, pt2, pt3, dx, dy, colors)
	Display *dpy;
	XpexCoord3D *pt1;
	XpexCoord3D *pt2;
	XpexCoord3D *pt3;
	int dx;
	int dy;
	XpexArrayOfColor *colors;
{
	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);
	XpexCheckLastReq(dpy,i);

	LockDisplay(dpy);
	{
		char *pStream;
		pexExtCellArray *oc;
		XpexColorType type = XpexGetColorType(dpy);
		int bytes_packed;
		int count = dx * dy;
		int extra = Xpex_sizeof_color(type) * count;
		XpexGetOutputCmdExtra(ExtCellArray, dpy, i, extra, oc);
		oc->colourType = type;
		oc->point1.x = pt1->x;
		oc->point1.y = pt1->y;
		oc->point1.z = pt1->z;
		oc->point2.x = pt2->x;
		oc->point2.y = pt2->y;
		oc->point2.z = pt2->z;
		oc->point3.x = pt3->x;
		oc->point3.y = pt3->y;
		oc->point3.z = pt3->z;
		oc->dx = (CARD32) dx;
		oc->dy = (CARD32) dy;
		pStream = (char *)(oc + 1);

		pack_colors(type, colors, count, 
		(char *)(oc + 1), &bytes_packed);
	}
	UnlockDisplay(dpy);
}

static void
pack_colors(type, colors, count, stream, bytesPacked)
	XpexColorType type;
	XpexArrayOfColor *colors;
	int count;
	char *stream; /* RETURN */
	int *bytesPacked; /* RETURN */
{
	switch (type) {
	case PEXIndexedColour:
		*bytesPacked = sizeof(pexIndexedColour) * count;
		bcopy((char *)(colors->index), stream, *bytesPacked);
		break;
	case PEXRgb8Colour:
		*bytesPacked = sizeof(pexRgb8Colour) * count;
		bcopy((char *)(colors->rgb8), stream, *bytesPacked);
		break;
	case PEXRgb16Colour:
		*bytesPacked = sizeof(pexRgb16Colour) * count;
		bcopy((char *)(colors->rgb16), stream, *bytesPacked);
		break;
	case PEXRgbFloatColour:
	case PEXCieFloatColour:
	case PEXHlsFloatColour:
	case PEXHsvFloatColour:
	default: 
		*bytesPacked = sizeof(pexFloatColour) * count;
		bcopy((char *)(colors->color_float), stream, *bytesPacked);
		break;
	}
}
