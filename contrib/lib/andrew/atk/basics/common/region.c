/*LIBS: -lX11
*/
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/region.c,v 2.8 1991/09/12 16:00:19 bobg Exp $ */
/* $ACIS:region.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/region.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/region.c,v 2.8 1991/09/12 16:00:19 bobg Exp $";
#endif /* lint */

#include <class.h>
#include <point.h>
#include <rect.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <region.eh>



/************** methods *********************/


void region__XorRegion(self, RHS, Result)
struct region * self;
struct region * RHS;
struct region * Result;{
    XXorRegion(self->regionData,RHS->regionData,Result->regionData);
}

void region__UnionRegion(self, RHS, Result)
struct region * self;
struct region * RHS; 
struct region * Result; {
    XUnionRegion(self->regionData,RHS->regionData,Result->regionData);
}

void region__IntersectRegion(self, RHS, Result)
struct region * self;
struct region * RHS;
struct region * Result; {
    XIntersectRegion(self->regionData,RHS->regionData,Result->regionData);
}

void region__SubtractRegion(self, RHS, Result)
struct region * self;
struct region * RHS;
struct region * Result; {
    XSubtractRegion(self->regionData,RHS->regionData,Result->regionData);
}

void region__ClearRegion(self)
struct region * self; {
    if(self->regionData) {
	XDestroyRegion(self->regionData);
    }
    self->regionData = XCreateRegion();
}

void region__RectRegion(self, NewRegnRect)
struct region * self;
struct rectangle * NewRegnRect;{
    XPoint rectPoint[5];
    rectPoint[0].x = rectPoint[4].x = rectangle_Left(NewRegnRect);
    rectPoint[0].y = rectPoint[4].y = rectangle_Top(NewRegnRect);
    rectPoint[1].x = rectangle_Right(NewRegnRect);
    rectPoint[1].y = rectangle_Top(NewRegnRect);
    rectPoint[2].x = rectangle_Right(NewRegnRect);
    rectPoint[2].y = rectangle_Bottom(NewRegnRect);
    rectPoint[3].x = rectangle_Left(NewRegnRect);
    rectPoint[3].y = rectangle_Bottom(NewRegnRect);
    if (self->regionData) {
	XDestroyRegion(self->regionData);
    }
    self->regionData = XPolygonRegion(rectPoint, 5,  EvenOddRule);
}

struct region * region__DuplicateRegion(self)
struct region * self;{
    struct region * retValue;

    retValue = region_CreateEmptyRegion();
    region_UnionRegion(self,retValue,retValue);
    return retValue;

}

void region__OffsetRegion(self, DeltaX,DeltaY)
struct region * self;
long DeltaY;
long DeltaX;{
    XOffsetRegion(self->regionData,DeltaX,DeltaY);
}

void region__InsetRegion(self, DeltaX, DeltaY)
struct region * self;
long DeltaY;
long DeltaX;{
    XShrinkRegion(self->regionData,DeltaX,DeltaY);
}

boolean region__IsRegionEmpty(self)
struct region * self;{
    return XEmptyRegion(self->regionData);
}

boolean region__IsPointInRegion(self,TestPt)
struct region * self;
struct point * TestPt;{
    return XPointInRegion(self->regionData,point_X(TestPt), point_Y(TestPt));
}

boolean region__IsRectInRegion(self, TestRect)
struct region * self;
struct rectangle * TestRect;{
    return XRectInRegion(self->regionData, rectangle_Left(TestRect), rectangle_Top(TestRect), rectangle_Width(TestRect), rectangle_Height(TestRect));
}

boolean region__AreRegionsEqual(self,TestRegion)
struct region * self;
struct region * TestRegion;{
    return XEqualRegion(self->regionData, TestRegion->regionData);
}

struct rectangle *region__GetBoundingBox(self, retRect)
struct region *self;
struct rectangle *retRect;
{
    XRectangle rect;

    if (retRect != NULL)  {
	XClipBox(self->regionData, &rect);
	
	rectangle_SetRectSize(retRect, rect.x, rect.y,
			      rect.width, rect.height);
    }
    return retRect;
}


/****************** classprocedures *****************/

struct region * region__CreateEmptyRegion(classID)
struct classheader *classID;
{
    struct region * retValue;
    retValue = region_New();
    return retValue;
}

struct region * region__CreateRectRegion(classID,RegionShape)
struct classheader *classID;
struct rectangle * RegionShape;
{
    struct region * self;

    self = region_New();
    region_RectRegion(self,RegionShape);
    return self;
}

struct region * region__CreateOvalRegion(classID,RegionShape)
struct classheader *classID;
struct rectangle * RegionShape;
{
    return region_CreateRectRegion(RegionShape);
}

struct region * region__CreatePolyRegion(classID,PointArray,PointCount)
struct classheader *classID;
struct point * PointArray;
short PointCount;
{
    XPoint * polyPts;
    int i;
    struct region * retValue;

    retValue = region_New();
    if(!retValue) {
	return NULL;
    }
    
    polyPts = (XPoint *) malloc(sizeof(XPoint) * (PointCount + 1));

    if(!polyPts) return NULL;
    
    for (i=0;i<PointCount;i++) {
	polyPts[i].x = point_X(&PointArray[i]);
        polyPts[i].y = point_Y(&PointArray[i]);
    }
    polyPts[PointCount] = polyPts[0];

    XDestroyRegion(retValue->regionData);
    retValue->regionData = XPolygonRegion(polyPts, PointCount+1, EvenOddRule);
    free(polyPts);
    return retValue;
}

void region__CopyRegion(classID,Destination,Source)
struct classheader *classID;
struct region * Destination;
struct region * Source;
{
    if (!Destination) return;
    if (Destination->regionData) {
	XDestroyRegion(Destination->regionData);
    }
    Destination->regionData = XCreateRegion();
    XUnionRegion(Destination->regionData, Source->regionData, Destination->regionData);
}

/************* Predefines *************/

boolean region__InitializeObject(classID,self)
struct classheader *classID;
struct region * self;
{
    self->regionData = XCreateRegion();
    if(self->regionData) return TRUE;
    else return FALSE;
}

void region__FinalizeObject(classID,self)
struct classheader *classID;
struct region * self;
{
    if (self->regionData) {
	XDestroyRegion(self->regionData);
    }
}
