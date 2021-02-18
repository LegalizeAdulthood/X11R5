/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/rect.h,v 2.6 1991/09/12 19:23:25 bobg Exp $ */
/* $ACIS:rect.h 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/rect.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidrectangle = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/rect.h,v 2.6 1991/09/12 19:23:25 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */



#ifndef RECTANGLE_DEFINED

#define RECTANGLE_DEFINED

#include <point.h>

struct rectangle {
    long top,left,height,width;
};

struct bothrectangle {
    union  {
	struct rectangle rectformat;
	struct { struct point upperLeft, lowerRightOffset;} pointformat;
    } bothformats;
} ;


#define rectangle_SetRectSize(LHS,sleft,stop,swidth,sheight) \
(    ((LHS)->top = (stop)), \
    ((LHS)->left = (sleft)), \
    ((LHS)->height = (sheight)), \
    ((LHS)->width = (swidth)) )

#define rectangle_Left(Rect)  ((Rect)->left)
#define rectangle_Top(Rect)  ((Rect)->top)
#define rectangle_Right(Rect) ((Rect)->left + (Rect)->width)
#define rectangle_Bottom(Rect) ((Rect)->top + (Rect)->height)
#define rectangle_Width(Rect) ((Rect)->width)
#define rectangle_Height(Rect) ((Rect)->height)
#define rectangle_GetRectSize(r, x, y, w, h) \
		(*(x) = (r)->left, *(y) = (r)->top, \
		*(w) = (r)->width, *(h) = (r)->height)

void rectangle_IntersectRect(/*Result,LHS,RHS*/);
void rectangle_UnionRect(/*Result,LHS,RHS*/);
/*void rectangle_SetRectSize(LHS, left,top,width,height); */
void rectangle_SetRectSides(/*LHS, left,top,right, bottom*/);
void rectangle_SetRectCorners(/*LHS, OneCorner,OtherCorner*/);
void rectangle_InsetRect(/*LHS, DeltaX, DeltaY*/);
void rectangle_OffsetRect(/*LHS, DeltaX, DeltaY*/);
void rectangle_EmptyRect(/*Rect*/);
boolean rectangle_IsEmptyRect(/*TestedRectangle*/);
boolean rectangle_IsEqualRect(/*LHS, RHS*/);
boolean rectangle_IsEnclosedBy(/*InnerRect, OuterRect*/);
boolean rectangle_IsPtInRect(/*TestPoint,TestRect*/);
short rectangle_PtToAngle(/*SamplePoint,ReferenceRect*/);
/*long rectangle_Left(Rect);*/
/*long rectangle_Right(Rect);*/
/*long rectangle_Top(Rect);*/
/*long rectangle_Bottom(Rect);*/
/*long rectangle_Width(Rect);*/
/*long rectangle_Height(Rect);*/
void rectangle_SetLeft(/*Rect,Value*/);
void rectangle_SetRight(/*Rect,Value*/);
void rectangle_SetHeight(/*Rect,Value*/);
void rectangle_SetWidth(/*Rect,Value*/);
void rectangle_SetTop(/*Rect,Value*/);
void rectangle_SetBottom(/*Rect,Value*/);

struct rectangle *rectangle_CreateRectCorners(/*OneCorner,OtherCorner*/);
struct rectangle *rectangle_CreateRectSize(/*left,top,width,height*/);
struct rectangle *rectangle_CreateRectSides(/*left,top,right,bottom*/);
struct rectangle *rectangle_Duplicate(/*Rect*/);

#endif /* RECTANGLE_DEFINED */
