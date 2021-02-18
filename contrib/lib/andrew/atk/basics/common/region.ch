/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/region.ch,v 2.8 1991/09/12 19:23:28 bobg Exp $ */
/* $ACIS:region.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/region.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidregion_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/region.ch,v 2.8 1991/09/12 19:23:28 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */



#define region_PROGRAMMERVERSION 2

class region {
methods:

    XorRegion(struct region * RHS, struct region * Result);
    UnionRegion(struct region * RHS, struct region * Result);
    IntersectRegion(struct region * RHS, struct region * Result);
    SubtractRegion(struct region * RHS, struct region * Result);
    ClearRegion();
    RectRegion(struct rectangle * NewRegnRect);
    DuplicateRegion() returns struct region *;
    OffsetRegion(long DeltaX, long DeltaY);
    InsetRegion(long DeltaX, long DeltaY);

    IsRegionEmpty() returns boolean;
    IsPointInRegion(struct point * TestPt) returns boolean;
    IsRectInRegion(struct rectangle * TestRect) returns boolean;
    AreRegionsEqual(struct region * TestRegion) returns boolean;

    GetBoundingBox(struct rectangle *rect) returns struct rectangle *;

macromethods:
    GetRegionData() ((self)->regionData)    /* Only used in the X code.  probably should subclass region to do this */

classprocedures:
    CreateEmptyRegion() returns struct region *;
    CreateRectRegion(struct rectangle * RegionShape) returns struct region *;
    CreateOvalRegion(struct rectangle * RegionShape) returns struct region *;
    CreatePolyRegion(struct point * PointArray, short PointCount) returns struct region *;
    CopyRegion(struct region * Destination, struct region * Source);
    FinalizeObject(struct region * self);
    InitializeObject(struct region *self) returns boolean;
data:
    struct _XRegion * regionData; /* really XRegion, but I don't want to include all of X just to get the typedef */
};

