/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/wcursor.c,v 1.7 1991/09/12 16:00:53 bobg Exp $ */
/* $ACIS:cursor.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/wcursor.c,v $ */

#ifndef lint
char *wcursor_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/wcursor.c,v 1.7 1991/09/12 16:00:53 bobg Exp $";
#endif /* lint */

#include <class.h>
#include <andyenv.h>
#include <fontdesc.ih>
#include <wgraphic.ih>
#include <view.ih>
#include <cursor.ih>
#include <im.ih>
#include <wmclient.h>
#include <mrl.ih>
#include <wcursor.eh>


	void 
wmcursor__ChangeShape(self)
	struct wmcursor *self;
{     
	struct  cursor *castedSelf  = (struct cursor *) self;
	struct mrl *mp;

	if(cursor_IsPosted(castedSelf) && castedSelf->posted->CursorsAreActive){
		wm_SelectWindow( 
			((struct wmgraphic *)im_GetDrawable(castedSelf->posted))
			->window);
		/* we expand wmcursor_SelectWMCursor() inline to
			avoid duplicate computations */
		if (castedSelf->fillFont == NULL) {
			for(mp = self->mrlist; mp != NULL; mp = mp->next)
			    if(mp->wmregion > 0){
				wm_SelectRegion(mp->wmregion);
				wm_SetStandardCursor(castedSelf->fillChar);
			    }
		}
		else {
			struct font *fd;
			fd = fontdesc_GetRealFontDesc( 
				castedSelf->fillFont, view_GetDrawable(castedSelf->view));
			for(mp = self->mrlist; mp != NULL; mp = mp->next)
			    if(mp->wmregion > 0){
				wm_SelectRegion(mp->wmregion);
				wm_SetCursor(fd, castedSelf->fillChar);
			    }
		}
	}

	if (cursor_IsWindowCursor(castedSelf)) {
		castedSelf->changed = TRUE;
		im_SetWindowCursor(castedSelf->windowim,  self);
	}
	if (cursor_IsProcessCursor(castedSelf)) {
		castedSelf->changed = TRUE;
		im_SetProcessCursor(self);
	}
}



	boolean 
wmcursor__InitializeObject(classID, self)
	struct classheader *classID;
	struct wmcursor *self;
{
	self->mrlist = NULL;
	return TRUE;
}

	void
wmcursor__FinalizeObject(classID, self)
	struct classheader *classID;
	struct wmcursor *self;
{
}

	boolean 
wmcursor__InitializeClass(classID)
	struct classheader *classID; 
{
	return TRUE;
}
