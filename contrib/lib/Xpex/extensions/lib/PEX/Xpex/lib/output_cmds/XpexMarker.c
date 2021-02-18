/* $Header: XpexMarker.c,v 2.3 91/09/11 16:06:41 sinyaw Exp $ */
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

#include "Xpexlibint.h"
#include "Xpexlib.h"
#include <X11/extensions/Xext.h>
#include "extutil.h"
#include "Xpextutil.h"

void
XpexMarkers(dpy, points, num_points)
	Display *dpy;
	XpexCoord3D *points;
	int num_points;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexMarker *oc;
		int extra = num_points * sizeof(pexCoord3D);
		XpexGetOutputCmdExtra(Marker,dpy,info,extra,oc);
		bcopy((char *)points,(char *)(oc + 1), extra);
	}
	UnlockDisplay(dpy);
}

void
XpexMarkers2D(dpy, points, num_points)
	Display *dpy;
	XpexCoord2D *points;
	int num_points;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexMarker2D *oc;
		int extra = num_points * sizeof(pexCoord2D);
		XpexGetOutputCmdExtra(Marker2D,dpy,info,extra,oc);
		bcopy((char *)points,(char *)(oc + 1), extra);
	}
	UnlockDisplay(dpy);
}

void
XpexSetMarkerType(dpy, type)
	Display *dpy;
	XpexEnumTypeIndex type;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexMarkerType *oc;
		XpexGetOutputCmd(MarkerType,dpy,info,oc);
		oc->markerType = type;
	}
	UnlockDisplay(dpy);
}

void
XpexSetMarkerScale(dpy, scale)
	Display *dpy;
	XpexFloat scale;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexMarkerScale *oc;
		XpexGetOutputCmd(MarkerScale,dpy,info,oc);
		oc->scale = scale;
	}
	UnlockDisplay(dpy);
}

void
XpexSetMarkerColorIndex(dpy, color_index)
	Display *dpy;
	XpexTableIndex color_index;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexMarkerColourIndex *oc;
		XpexGetOutputCmd(MarkerColourIndex,dpy,info,oc);
		oc->index = color_index;
	}
	UnlockDisplay(dpy);
}

void
XpexSetMarkerColor(dpy, color)
	Display *dpy;
	XpexColor *color;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexMarkerColour *oc;
		XpexColorType type = XpexGetColorType(dpy);
		int extra = Xpex_sizeof_color(type);
		int length;
		XpexGetOutputCmdExtra(MarkerColour,dpy,info,extra,oc);
		oc->colourSpec.colourType = type;
		Xpex_pack_color(type,color,(char *)(oc + 1),length);
	}
	UnlockDisplay(dpy);
}

void
XpexSetMarkerBundleIndex(dpy, index)
	Display *dpy;
	XpexTableIndex index;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexMarkerBundleIndex *oc;
		XpexGetOutputCmd(MarkerBundleIndex,dpy,info,oc);
		oc->index = index;
	}
	UnlockDisplay(dpy);
}
