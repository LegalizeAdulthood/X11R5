/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/lprruler.ch,v 2.8 1991/09/12 19:50:04 bobg Exp $ */
/* $ACIS:lprruler.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/lprruler.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidlprruler_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/lprruler.ch,v 2.8 1991/09/12 19:50:04 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
lprruler.H

	lprruler data object

	This module stores nothing and provides no methods.

	The file format for a lprruler has nothing between the lines for
	\begindata and \enddata.

*/

#define  lprruler_PROGRAMMERVERSION    1

/* value for NotifyObservers */
#define lprruler_DATACHANGED 2


class lprruler : dataobject[dataobj]{

overrides:

	Read( /* struct lprruler *self, */ FILE *file, long id ) returns long;
	Write( /* struct lprruler *self, */ FILE *file, long id, long level )
			returns long;

methods:

macromethods:

classprocedures:

	InitializeObject(/* struct classhdr *ClassID;*/ struct lprruler *self) returns boolean;
	FinalizeObject(/* struct classhdr *ClassID;*/ struct lprruler *self);

data:

};

