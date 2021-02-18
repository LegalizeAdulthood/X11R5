/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/mrl.ch,v 2.7 1991/09/12 19:24:06 bobg Exp $ */
/* $ACIS:mrl.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/mrl.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidmrl_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/mrl.ch,v 2.7 1991/09/12 19:24:06 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


class mrl {
methods:
	Disect(struct rectangle *r) returns struct mrl *;
	Reset(struct rectangle *r)returns struct mrl *;
classprocedures:
	InitializeObject(struct mrl *self) returns boolean;
	FinalizeObject(struct mrl *self);
	Create(long region, struct mrl *next,struct rectangle *r) returns struct mrl *;
data:
	long wmregion;
	long top;
	long bottom;
	long left;
	long right;
	struct mrl *next;
};

