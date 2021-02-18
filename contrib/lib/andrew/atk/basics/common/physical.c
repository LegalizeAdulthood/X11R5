/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/physical.c,v 2.4 1991/09/12 15:59:46 bobg Exp $ */
/* $ACIS:physical.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/physical.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/physical.c,v 2.4 1991/09/12 15:59:46 bobg Exp $";
#endif /* lint */

/* This is a utility package only to be used by im and graphic (and
 perhaps other machine dependent modules that I do not yet know about,
 such as the cursor system. This package provides a way to convert
 between the coordinate spaces of a graphic and wm "hardware" */

#include <class.h>
#include <graphic.ih>


#define SINGLEWINDOW 1

long physical_LogicalPtToGlobalX(CoordinateSystem,LogicalPoint)
struct graphic * CoordinateSystem;
struct point * LogicalPoint; {
#if SINGLEWINDOW
    return point_X(LogicalPoint)+
	   point_X(&CoordinateSystem->physicalOrigin)-
	   rectangle_Left(&CoordinateSystem->localBounds);
#else /* SINGLEWINDOW */
    return point_X(LogicalPoint)-
	   rectangle_Left(&CoordinateSystem->localBounds);
#endif /* SINGLEWINDOW */
}   

long physical_LogicalPtToGlobalY(CoordinateSystem,LogicalPoint)
struct graphic * CoordinateSystem;
struct point * LogicalPoint; {
#if SINGLEWINDOW
    return point_Y(LogicalPoint)+
	   point_Y(&CoordinateSystem->physicalOrigin)-
	   rectangle_Top(&CoordinateSystem->localBounds);
#else /* SINGLEWINDOW */
    return point_Y(LogicalPoint)-
	   rectangle_Top(&CoordinateSystem->localBounds);
#endif /* SINGLEWINDOW */
}   

void physical_LogicalPtToGlobalPt(CoordinateSystem,tempPoint)
struct graphic * CoordinateSystem;
struct point * tempPoint; {
#if SINGLEWINDOW
    point_OffsetPoint(tempPoint,
	   point_X(&CoordinateSystem->physicalOrigin)-
	   rectangle_Left(&CoordinateSystem->localBounds),
	   point_Y(&CoordinateSystem->physicalOrigin)-
	   rectangle_Top(&CoordinateSystem->localBounds));
#else /* SINGLEWINDOW */
    point_OffsetPoint(tempPoint,
	   -rectangle_Left(&CoordinateSystem->localBounds),
	   -rectangle_Top(&CoordinateSystem->localBounds));
#endif /* SINGLEWINDOW */
}   

void physical_GlobalPtToLogicalPt(CoordinateSystem,tempPoint)
struct graphic * CoordinateSystem;
struct point * tempPoint; {
#if SINGLEWINDOW
    point_OffsetPoint(tempPoint,
	   -(point_X(&CoordinateSystem->physicalOrigin)-
	   rectangle_Left(&CoordinateSystem->localBounds)),
	   -(point_Y(&CoordinateSystem->physicalOrigin)-
	   rectangle_Top(&CoordinateSystem->localBounds)));
#else /* SINGLEWINDOW */
    point_OffsetPoint(tempPoint,
	   rectangle_Left(&CoordinateSystem->localBounds),
	   rectangle_Top(&CoordinateSystem->localBounds));
#endif /* SINGLEWINDOW */
}   



long physical_LogicalXToGlobalX(CoordinateSystem,LogicalX)
struct graphic * CoordinateSystem;
long LogicalX; {
#if SINGLEWINDOW
    return LogicalX +
	   point_X(&CoordinateSystem->physicalOrigin)-
	   rectangle_Left(&CoordinateSystem->localBounds);
#else /* SINGLEWINDOW */
    return LogicalX -
	   rectangle_Left(&CoordinateSystem->localBounds);
#endif /* SINGLEWINDOW */
}   

long physical_LogicalYToGlobalY(CoordinateSystem,LogicalY)
struct graphic * CoordinateSystem;
long LogicalY; {
#if SINGLEWINDOW
    return LogicalY +
	   point_Y(&CoordinateSystem->physicalOrigin)-
	   rectangle_Top(&CoordinateSystem->localBounds);
#else /* SINGLEWINDOW */
    return LogicalY -
	   rectangle_Top(&CoordinateSystem->localBounds);
#endif /* SINGLEWINDOW */
}   

long physical_GlobalXToLogicalX(CoordinateSystem,PhysicalX)
struct graphic * CoordinateSystem;
long PhysicalX; {
#if SINGLEWINDOW
    return PhysicalX - (
	   point_X(&CoordinateSystem->physicalOrigin)-
	   rectangle_Left(&CoordinateSystem->localBounds));
#else /* SINGLEWINDOW */
    return PhysicalX +
	   rectangle_Left(&CoordinateSystem->localBounds);
#endif /* SINGLEWINDOW */
}

long physical_GlobalYToLogicalY(CoordinateSystem,PhysicalY)
struct graphic * CoordinateSystem;
long PhysicalY; {
#if SINGLEWINDOW
    return PhysicalY - (
	   point_Y(&CoordinateSystem->physicalOrigin)-
	   rectangle_Top(&CoordinateSystem->localBounds));
#else /* SINGLEWINDOW */
    return PhysicalY +
	   rectangle_Top(&CoordinateSystem->localBounds);
#endif /* SINGLEWINDOW */
}

void physical_LogicalToGlobalRect(CoordinateSystem,TempRect)
struct graphic * CoordinateSystem;
struct rectangle * TempRect; {
#if SINGLEWINDOW
    rectangle_OffsetRect(TempRect,
	   point_X(&CoordinateSystem->physicalOrigin)-
	   rectangle_Left(&CoordinateSystem->localBounds),
	   point_Y(&CoordinateSystem->physicalOrigin)-
	   rectangle_Top(&CoordinateSystem->localBounds));
#else /* SINGLEWINDOW */
    rectangle_OffsetRect(TempRect,
	   -rectangle_Left(&CoordinateSystem->localBounds),
	   -rectangle_Top(&CoordinateSystem->localBounds));
#endif /* SINGLEWINDOW */
}

void physical_GlobalToLogicalRect(CoordinateSystem,TempRect)
struct graphic * CoordinateSystem;
struct rectangle * TempRect; {
#if SINGLEWINDOW
    rectangle_OffsetRect(TempRect,
	   -(point_X(&CoordinateSystem->physicalOrigin)-
	   rectangle_Left(&CoordinateSystem->localBounds)),
	   -(point_Y(&CoordinateSystem->physicalOrigin)-
	   rectangle_Top(&CoordinateSystem->localBounds)));
#else /* SINGLEWINDOW */
    rectangle_OffsetRect(TempRect,
	   rectangle_Left(&CoordinateSystem->localBounds),
	   rectangle_Top(&CoordinateSystem->localBounds));
#endif /* SINGLEWINDOW */
}

