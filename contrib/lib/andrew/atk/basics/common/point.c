/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/point.c,v 2.4 1991/09/12 16:00:01 bobg Exp $ */
/* $ACIS:point.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/point.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/point.c,v 2.4 1991/09/12 16:00:01 bobg Exp $";
#endif /* lint */


#include <point.h>

/* These have been macrofied */

/*
long point_X(Pt)
struct point * Pt; {
    return Pt->x;
}

long point_Y(Pt)
struct point * Pt; {
    return Pt->y;
}
*/

void point_SetX(Pt,Value)
struct point * Pt;
long Value; {
    Pt->x=Value;
}

void point_SetY(Pt,Value)
struct point * Pt;
long Value; {
     Pt->y=Value;
}

void point_OffsetPoint(Pt,DeltaX, DeltaY)
struct point * Pt;
long DeltaY;
long DeltaX; {
    Pt->y += DeltaY;
    Pt->x += DeltaX;
}

void point_AddPt(LHS,RHS)
struct point * LHS;
struct point * RHS;{
    LHS->y += RHS->y;
    LHS->x += RHS->x;
}

void point_SubPt(LHS,RHS)
struct point * LHS;
struct point * RHS; {
    LHS->y -= RHS->y;
    LHS->x -= RHS->x;
}

/*
 More macrofied
*/
/*

void point_SetPt(Pt,NewX, NewY)
struct point * Pt;
long NewY;
long NewX;{
    Pt->y = NewY;
    Pt->x = NewX;
}

*/

boolean point_ArePtsEqual(LHS,RHS)
struct point * LHS;
struct point * RHS; {
    return (LHS->x == RHS->x) && (LHS->y == RHS->y);
}

struct point * point_CreatePoint(InitX, InitY)
long InitY;
long InitX; {
    struct point * RetValue;

    RetValue = (struct point *) malloc(sizeof(struct point));
    point_SetPt(RetValue,InitX,InitY);
    return RetValue;
}
