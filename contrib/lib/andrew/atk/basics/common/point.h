/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/point.h,v 2.4 1991/09/12 19:23:14 bobg Exp $ */
/* $ACIS:point.h 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/point.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidpoint = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/point.h,v 2.4 1991/09/12 19:23:14 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */



#ifndef POINT_DEFINED

#define POINT_DEFINED

#include <class.h>

struct point {
    long y,x;
};

#define point_X(thePoint) ((thePoint)->x)
#define point_Y(thePoint) ((thePoint)->y)
/* original unmacroified versions */
/*
long point_X();
long point_Y();
*/
void point_SetX(/*Pt,Value*/);
void point_SetY(/*Pt,Value*/);

void point_OffsetPoint(/*Pt,DeltaX, DeltaY*/);
void point_AddPt(/*LHS,RHS*/);
void point_SubPt(/*LHS,RHS*/);
/*void point_SetPt(LHS, NewX, NewY); */
#define point_SetPt(PtToSet,NewX, NewY) \
(    ((PtToSet)->y = (NewY)), \
     ((PtToSet)->x = (NewX)) )

boolean point_ArePtsEqual(/*LHS,RHS*/);
struct point * point_CreatePoint(/*InitX,InitY*/);
#endif /* POINT_DEFINED */
