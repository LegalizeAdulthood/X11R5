/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/gob/RCS/gob.ch,v 1.6 1991/09/12 19:40:32 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/gob/RCS/gob.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/gob/RCS/gob.ch,v 1.6 1991/09/12 19:40:32 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
 * P_R_P_Q_# (C) COPYRIGHT IBM CORPORATION 1987, 1988
 * LICENSED MATERIALS - PROPERTY OF IBM
 * REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
 */
/*
gob.H

	gob data object

	This module stores the coordinates of a set of blobs and
	provides methods to add blobs and clear the list.

	The file format for a gob begins and edns with the standard 
	\begindata \enddata.  There is one intervening line for each blob 
	in the image.  This line gives the coordinates of the upper left corner 
	of the blob as two ASCII integers separated by a space.  The origin 
	for the coordinates is the upper left corner of the rect.

*/
/*
 *    $Log: gob.ch,v $
Revision 1.6  1991/09/12  19:40:32  bobg
Update copyright notice

Revision 1.5  1989/02/20  12:50:44  ghoti
class syntax changes

Revision 1.4  89/02/17  16:16:36  ghoti
ifdef/endif,etc. label fixing - courtesy of Ness

Revision 1.3  89/02/08  16:20:25  ghoti
change copyright notice

Revision 1.2  89/02/07  10:55:27  ghoti
first pass porting changes: filenames and references to them

Revision 1.1  88/12/23  16:02:58  tpn
Initial revision
 
 * Revision 1.0  88/12/15  10:04:24  tpn
 * Copied from /usr/andrew/lib/nullinset
 */

#include <point.h>
#ifndef timercmp
#include <sys/time.h>
#endif /* timercmp */

/* status values for NotifyObservers */
#define gob_DATACHANGED 1

#define gob_OPEN 1
#define gob_CLOSED 2
#define gob_FILLED 4

#define gob_PATH 8
#define gob_OVAL 16
#define gob_ARC 32
#define gob_LABEL 64

#define gob_OBMASK (8|16|32|64)
#define gob_TYPEMASK (1|2|4)

#define gob_WHITE 0
#define gob_BLACK 16
#define gob_GRAY(X) X
#define gettime(X)  gettimeofday(X,0) 
/* $$$ declare any structs that will be needed */
struct oblist {
    long type;
    struct timeval modtime;
    struct point *ptlist;
    struct oblist *next;
    short gray;
    short size;
    short mode;
    char *label;
    struct fontdesc *fd;
};


class gob : dataobject[dataobj]
{

overrides:

	Read(/* struct gob *self, */ FILE *file, long id)
			returns long;
	Write(/* struct gob *self, */ FILE *file, long id, long level)
			returns long;

methods:
        SetLogicalRect(x,y,width,heigth);
	ClearAllObs(/* struct gob *self */);
		/* remove all obs from list */

	ClearOb(/* struct gob *self, */ struct oblist *ob);
               /* remove one ob from list */

	/* In the following colorwid may beas follows.
	 For Filled objects 
	       gob_BLACK, gob_WHITE,  or gob_GRAY(X)  where X is 0 to 16  
	 For drawn objects
	     a width. Negitive widths indicate drawing in white;
	     */

	AddPathOb(struct point *pointlist,short size,short type,short colorwid,short mode ) returns struct oblist *; 
             /* type == gob_OPEN, gob_CLOSED, or gob_FILLED */

        AddOvalOb(long x, long y, long width,long height, short type,short colorwid,short mode )returns struct oblist *;
	             /* type == gob_OPEN, or gob_FILLED */

        AddArcOb(long x, long y, long width,long height,short StartAngle, short OffsetAngle, short type,short colorwid,short mode )returns struct oblist *;
	             /* type == gob_OPEN, or gob_FILLED */

	AddLabelOb(long	x,long y,char *label,struct fontdesc *fd,short op,short	mode )returns struct oblist *;	
macromethods:

	/* $$$ unique to this data object */
	GetFirstOb(/* struct gob *self */) (self->obs)
	GetNextOb(/* struct gob *self, */ pair) (pair->next)

classprocedures:
	InitializeClass() returns boolean;
	InitializeObject(struct gob *self) returns boolean;
	FinalizeObject(struct gob *self);

data:

	/* $$$ data values unique to this data object */
	struct oblist *obs;
        long xmin,xmax,ymin,ymax;
	struct timeval modtime;
};

#if 0
	AddOb(/* struct gob *self, */ struct oblist *ob);
		/* add a pair to list of obs */

	FreeOb(struct olist *ob);

	FreeObs();
		/* remove and free all obs from list */
#endif /* 0 */




