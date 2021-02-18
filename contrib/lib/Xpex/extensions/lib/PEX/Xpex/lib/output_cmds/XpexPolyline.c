/* $Header: XpexPolyline.c,v 2.4 91/09/11 16:06:44 sinyaw Exp $ */

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
XpexSetLineType(dpy, lineType)
	Display *dpy; 
	XpexEnumTypeIndex lineType;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexLineType *oc;
		XpexGetOutputCmd(LineType, dpy, info,oc);
		oc->lineType = lineType;
	}
	UnlockDisplay(dpy);
}

void
XpexSetLineWidth(dpy, width)
	Display *dpy;
	XpexFloat width;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexLineWidth *oc;
		XpexGetOutputCmd(LineWidth,dpy,info,oc);
		oc->width = width;
	}
	UnlockDisplay(dpy);
}

void
XpexSetLineColorIndex(dpy, color_index)
	Display *dpy;
	XpexTableIndex color_index;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexLineColourIndex *oc;
		XpexGetOutputCmd(LineColourIndex, dpy, info,oc);
		oc->index = color_index;
	}
	UnlockDisplay(dpy);
}

void
XpexSetLineColor(dpy, color)
	Display *dpy;
	XpexColor *color;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexLineColour *oc;
		XpexColorType type = XpexGetColorType(dpy);
		int extra = Xpex_sizeof_color(type);
		int length;
		XpexGetOutputCmdExtra(LineColour,dpy,info,extra,oc);
		oc->colourSpec.colourType = type;
		Xpex_pack_color(type,color,(char *)(oc + 1),&length);
	}
	UnlockDisplay(dpy);
}

void
XpexSetCurveApproximation(dpy, approx_method, tolerance)
	Display *dpy;
	XpexEnumTypeIndex approx_method;
	XpexFloat tolerance;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexCurveApproximation *oc;
		XpexGetOutputCmd(CurveApproximation,dpy,info,oc);
		oc->approx.approxMethod = approx_method;
		oc->approx.tolerance = tolerance;
	}
	UnlockDisplay(dpy);
}

void
XpexSetPolylineInterpMethod(dpy, method)
	Display *dpy;
	XpexEnumTypeIndex method;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexPolylineInterp *oc;
		XpexGetOutputCmd(PolylineInterp,dpy,info,oc);
		oc->polylineInterp = method;
	}
	UnlockDisplay(dpy);
}

void
XpexSetLineBundleIndex(dpy, index)
	Display *dpy;
	XpexTableIndex index;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexLineBundleIndex *oc;
		XpexGetOutputCmd(LineBundleIndex, dpy, info,oc);
		oc->index = (pexTableIndex) index;
	}
	UnlockDisplay(dpy);
}

void
XpexPolyline(dpy, points, num_points)
	Display *dpy;
	XpexCoord3D *points;
	int num_points;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexPolyline *oc;
		int extra = num_points * sizeof(pexCoord3D);
		XpexGetOutputCmdExtra(Polyline, dpy, info,extra, oc);
		bcopy((char *)points,(char *)(oc + 1),extra);
	}
	UnlockDisplay(dpy);
}

void
XpexPolyline2D(dpy, points, num_points)
	Display *dpy; 
	XpexCoord2D *points; 
	int num_points;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy, info);

	LockDisplay(dpy);
	{
		pexPolyline2D *oc;
		int extra = num_points * sizeof(pexCoord2D);
		XpexGetOutputCmdExtra(Polyline2D,dpy,info,extra,oc);
		bcopy((char *)points,(char *)(oc + 1),extra);
	}
	UnlockDisplay(dpy);
}

static int 
sizeof_polyline_data(attr_mask, type, lists, num_lists)
	XpexBitmaskShort attr_mask; 
	XpexColorType type;
	XpexListOfVertex *lists; 
	int num_lists;
{
	int size = 0;
	XpexBitmaskShort c = attr_mask & XpexGA_Color;
	int sc = 0; /* size of colour */

	if (c) {
		sc = Xpex_sizeof_color(type);
	}

	{
		int v = sizeof(pexCoord3D) + sc;
		{
			register long i;

			for (i = 0; i < num_lists; i++) {
				size += lists[i].numVertices;
			}
		}
	}

	return size;
}

void
XpexPolylineSet(dpy, vertexAttr, vertexLists, numLists)
	Display *dpy;
	XpexBitmaskShort vertexAttr;
	XpexListOfVertex *vertexLists;
	int numLists;
{
	extern void Xpex_pack_list_of_vertex();

	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexPolylineSet *oc;
		int length; /* bytes packed */
		XpexColorType type = XpexGetColorType(dpy);
		int extra = sizeof_polyline_data(vertexAttr, type,
		vertexLists, numLists);

		XpexGetOutputCmdExtra(PolylineSet,dpy,info,extra,oc);
		oc->vertexAttribs = (pexBitmaskShort) vertexAttr;
		oc->numLists = (CARD32) numLists;
		Xpex_pack_list_of_vertex(vertexAttr & XpexGA_Color, 
		type, vertexLists, numLists, (char *)(oc + 1), &length);
	}
	UnlockDisplay(dpy);
}

static int
sizeof_nurbcurve_data(coordType,numKnots,numPoints) 
	XpexCoordType coordType;
	int numKnots;
	int numPoints;
{
	int size = 
		(numKnots * sizeof(XpexFloat)) +
		(numPoints * Xpex_sizeof_coord(coordType));

	return size;
}

void
XpexNurbCurve(dpy, curveOrder, coordType, 
	knots, numKnots, points, numPoints, tMin, tMax)
	Display *dpy;
	int curveOrder;
	XpexCoordType coordType;
	XpexFloat *knots;
	long numKnots;
	XpexArrayOfCoord *points;
	long numPoints;
	XpexFloat tMin;
	XpexFloat tMax;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		char *pStream;
		pexNurbCurve *oc;
		int extra = sizeof_nurbcurve_data(coordType,numKnots,numPoints);

		XpexGetOutputCmdExtra(NurbCurve,dpy,info,extra,oc);
		oc->curveOrder = (CARD16) curveOrder;
		oc->coordType = coordType;
		oc->tmin = tMin;
		oc->tmax = tMax;
		oc->numKnots = (CARD32) numKnots;
		oc->numPoints = (CARD32) numPoints;
		pStream = (char *)(oc + 1);
		bcopy((char *)knots, (char *)(oc + 1), numKnots * sizeof(XpexFloat));
		pStream += numKnots * sizeof(XpexFloat);
		if (coordType == Xpex_Rational) {
			bcopy((char *) points->points_4d, pStream,
			numPoints * sizeof(pexCoord4D));
		} else {
			bcopy((char *) points->points_3d, pStream,
			numPoints * sizeof(pexCoord3D));
		}
	}
	UnlockDisplay(dpy);
}
