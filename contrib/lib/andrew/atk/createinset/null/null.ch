/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/createinset/null/RCS/null.ch,v 1.2 1991/09/12 19:30:59 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/createinset/null/RCS/null.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	char *null_ch_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/createinset/null/RCS/null.ch,v 1.2 1991/09/12 19:30:59 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
null.ch

	null data object

	This module stores the coordinates of a set of blobs and
	provides methods to add blobs and clear the list.

	The file format for a null begins and edns with the standard 
	\begindata \enddata.  There is one intervening line for each blob 
	in the image.  This line gives the coordinates of the upper left corner 
	of the blob as two ASCII integers separated by a space.  The origin 
	for the coordinates is the upper left corner of the rectangle.

*/
/*
 *    $Log: null.ch,v $
Revision 1.2  1991/09/12  19:30:59  bobg
Update copyright notice

Revision 1.1  1989/07/31  15:34:54  wjh
Initial revision

 *
 * Revision 1.0  88/05/14  15:40:32  wjh
 * Copied from /usr/andrew/lib/genericinset
 */




/* status values for NotifyObservers */
#define null_DATACHANGED 1

/* $$$ declare any structs that will be needed */
struct dotlist {
	long x, y;
	struct dotlist *next;
};


class null : dataobject [dataobj]
{

overrides:

	Read(/* struct null *self, */ FILE *file, long id)
			returns long;
	Write(/* struct null *self, */ FILE *file, long id, long level)
			returns long;

methods:

	/* $$$ unique to this data object */
	AddDot(/* struct null *self, */ x, y);
		/* add a pair to list of dots */
	ClearDots(/* struct null *self */);
		/* remove all dots from list */

macromethods:

	/* $$$ unique to this data object */
	GetFirstDot(/* struct null *self */) (self->dots)
	GetNextDot(/* struct null *self, */ pair) (pair->next)
	GetDotX(/* struct null *self, */ pair) (pair->x)
	GetDotY(/* struct null *self, */ pair) (pair->y)

classprocedures:

	InitializeClass() returns boolean;
	InitializeObject(struct null *self) returns boolean;
	FinalizeObject(struct null *self);

data:

	/* $$$ data values unique to this data object */
	struct dotlist *dots;

};

