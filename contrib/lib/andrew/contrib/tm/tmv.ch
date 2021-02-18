/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/tm/RCS/tmv.ch,v 2.6 1991/09/12 20:17:34 bobg Exp $ */
/* $ACIS:tmv.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/tm/RCS/tmv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtmview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/tm/RCS/tmv.ch,v 2.6 1991/09/12 20:17:34 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


class tmview[tmv]: textview[textv] {
    overrides:
	SetDataObject(struct termulator *do);
	PostKeyState(struct keystate *ks);
	PostMenus(struct menulist *ml);
	FullUpdate(enum view_UpdateType type,long left,top,width,height);
	Update();
	ReceiveInputFocus();
	GetClickPosition(pos,noc,action,startLeft,startRight,leftPosP,rightPosP);
    methods:
	SetFileMenus(boolean on);
	ReadShMenus(char *filename) returns boolean;
    macromethods:
	SetTitle(char *t) (self->title=(t))
        GetTitle() (self->title)
    classprocedures:
	InitializeClass() returns boolean;
        InitializeObject(struct tmview *tmv) returns boolean;
	FinalizeObject(struct tmview *tmv);
    data:
        int screen;
        struct keystate *keystate;
	struct menulist *menus, *shmenus;
	struct mark *curpos;
	char *cwd, *title;
	int height,width;
};
