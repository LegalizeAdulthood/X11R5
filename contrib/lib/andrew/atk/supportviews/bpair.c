/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/bpair.c,v 2.10 1991/09/12 16:32:22 bobg Exp $ */
/* $ACIS:bpair.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/bpair.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/bpair.c,v 2.10 1991/09/12 16:32:22 bobg Exp $";
#endif /* lint */

/* bpair.c	

	An lpair without the line between the parts
	and without movable or cursors 

*/


#include <class.h>
#include <graphic.ih>
#include <view.ih>
#include <cursor.ih>

#include <im.ih>
#include <rect.h>

#include <lpair.ih>
#include <bpair.eh>


#define BARWIDTH 0
#define GRAVITY 1

/* Forward Declarations */
static void lpair_ComputeSizes ();
static void lpair_ResetDimensions();

/* the following two routines are needed for FullUpdate 
	They are copied from lpair.c, with deletion of the line-between code,
	code for movable borders, and the calls on DesiredSize   */

static void lpair_ComputeSizes (l)
register struct lpair *l;
{

    int totalsize, i = 0;

/* Find out how much space the two must share. */
    if (l->typex == lpair_VERTICAL)
        totalsize = bpair_GetLogicalWidth(l);
    else
        totalsize = bpair_GetLogicalHeight(l);

/* See if we don't have any space -- actually we should be testing 
	to see if children can fit at all, but for, now a simple test for zero */
    if (totalsize == 0) {
	l->objcvt[0] = l->objcvt[1] = 0;
	return;
    }

    --totalsize;	/* Make room for the bar in the middle. */

    switch(l->sizeform) { /* Allocate space for the 'hard' allocated view. */
        case lpair_PERCENTAGE:
            i = 1;
            l->objcvt[i] = (l->objsize[i] * totalsize) / 100;
            break;
        case lpair_BOTTOMFIXED:
            i = 1;
            l->objcvt[i] = l->objsize[i];
            break;
        case lpair_TOPFIXED:
            i = 0;
            l->objcvt[i] = l->objsize[i];
            break;
    }
/* Give the rest to the other view. */
    l->objcvt[1-i] = totalsize - l->objcvt[i];
}

static void lpair_ResetDimensions(self)
register struct lpair *self;
{

    register int i, x, y;
    register struct  view *child;
    struct rectangle enclosingRect;

    x = 0; y = 0;
    for (i = 0; i < 2; i++) { /* Loop over the two halves of the lpair. */
	child = (struct view *)self->obj[i];
	if (self->typex == lpair_VERTICAL) {
	      rectangle_SetRectSize(&enclosingRect,
			x, y, self->objcvt[i],  lpair_GetLogicalHeight(self));
	      view_InsertView(child, self, &enclosingRect);
	      x += self->objcvt[i] + 2 * BARWIDTH + 1;
	}
	else {
	      rectangle_SetRectSize(&enclosingRect,
			x, y,  lpair_GetLogicalWidth(self), self->objcvt[i]);
	      view_InsertView(child, self, &enclosingRect);
	      y += self->objcvt[i] + 2 * BARWIDTH + 1;
	}
    }
}


	boolean
bpair__InitializeObject(ClassID, self)
	struct classheader *ClassID;
	register struct bpair  *self;
{
/* BOGUS, cannot access lpair_InitializeObject */
/* copy the code instead */
 /*   Assume that initialization is really done via lpair_Create */
    self->header.lpair.obj[0] = NULL; 
    self->header.lpair.obj[1] = NULL;
    self->header.lpair.typex = lpair_HORIZONTAL;
    self->header.lpair.ismoving = FALSE;
    self->header.lpair.movable = FALSE;
    self->header.lpair.cursor = cursor_Create(self);
    cursor_SetStandard(self->header.lpair.cursor, Cursor_HorizontalBars);
    self->header.lpair.needsfull = 0;
    return TRUE;
}
	void
bpair__FinalizeObject(ClassID, self)
	struct classheader *ClassID;
	register struct bpair  *self;
{
}


void 
bpair__FullUpdate(self, type, left, top, width, height)
	register struct bpair  *self;
	register enum view_UpdateType  type;
	register long  left, top, width, height;
{
/* ( the following, including derogatory comment, is copied from lpair.c) */

			/*  All this code needs changed */
    register struct view * leftTopObject = self->header.lpair.obj[0];
    register struct view * rightBottomObject = self->header.lpair.obj[1];
    struct rectangle r;

    self->header.lpair.movable = FALSE;

    self->header.lpair.needsfull = 0;

    lpair_ComputeSizes((struct lpair *)self);
    lpair_ResetDimensions((struct lpair *)self);	/* reset the child lpair sizes */

    view_GetLogicalBounds(leftTopObject, &r);
    view_FullUpdate(leftTopObject, type, r.left, r.top, r.width, r.height);
    view_GetLogicalBounds(rightBottomObject, &r);
    view_FullUpdate(rightBottomObject, type, r.left, r.top, r.width, r.height);

	/* deleted the line and cursor code */
}
