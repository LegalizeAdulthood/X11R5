/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/wcursor.ch,v 1.6 1991/09/12 19:24:10 bobg Exp $ */
/* $ACIS:cursor.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/wcursor.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidwmcursor_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/wcursor.ch,v 1.6 1991/09/12 19:24:10 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* 
 *	wcursor.ch
 */

#include <rect.h>

/* The following should not be used because programs are not to know what window manager they use */
#define wmcursor_SelectWMCursor(CURSOR) 	\
	(((struct cursor *)(CURSOR))->fillFont == NULL)  \
		? wm_SetStandardCursor(((struct cursor *)(CURSOR))->fillChar)  \
		:  wm_SetCursor(fontdesc_GetRealFontDesc(  \
			((struct cursor *)(CURSOR))->fillFont,  \
			((struct cursor *)(CURSOR))->view),  \
		((struct cursor *)(CURSOR))->fillChar)

	
class wmcursor[wcursor] : cursor {
overrides:
	ChangeShape();
classprocedures:
	InitializeClass() returns boolean;
	InitializeObject(struct wmcursor *self) returns boolean;
	FinalizeObject(struct wmcursor *self);   /* could be used to get rid of active cursors */
data:
	struct rectangle rec;
	struct mrl *mrlist;
};

