/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xcursor.ch,v 1.6 1991/09/12 19:24:37 bobg Exp $ */
/* $ACIS:cursor.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xcursor.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidxcursor_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xcursor.ch,v 1.6 1991/09/12 19:24:37 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* 
 *	cursor.ch
 */

#include <rect.h>

class xcursor : cursor {
overrides:
	SetStandard(short ch);
	ChangeShape();
methods:
	Make(struct xim *im);
classprocedures:
	InitializeObject(struct xcursor *self) returns boolean;
	FinalizeObject(struct xcursor *self); /*  used to get rid of active cursors */
	InitializeClass() returns boolean;
data:
	Cursor  Xc;
	Window Xw;
	Display * Xd;
	struct xim *im;
	boolean valid;
};
