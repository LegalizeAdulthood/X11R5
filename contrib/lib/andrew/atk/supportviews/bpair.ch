/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/bpair.ch,v 2.7 1991/09/12 19:49:46 bobg Exp $ */
/* $ACIS:bpair.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/bpair.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidbpair_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/bpair.ch,v 2.7 1991/09/12 19:49:46 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
bpair.H

	Class definitions for bpair

*/

#define  bpair_PROGRAMMERVERSION    1

class bpair: lpair
{
overrides:

	FullUpdate(/* struct bpair *self, */ enum view_UpdateType type, 
			long left, long top, long width, long height);

classprocedures:

	InitializeObject(/* struct classhdr *ClassID;*/ struct bpair *self) returns boolean;
	FinalizeObject(/* struct classhdr *ClassID;*/ struct bpair *self);

};
