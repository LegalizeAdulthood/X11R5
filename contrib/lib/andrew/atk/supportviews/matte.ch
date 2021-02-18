/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/matte.ch,v 2.8 1991/09/12 19:50:12 bobg Exp $ */
/* $ACIS:matte.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/matte.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidmatte_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/matte.ch,v 2.8 1991/09/12 19:50:12 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class matte : view {
overrides:
    WantNewSize(struct view *requestor);
    PostMenus(struct menulist *menulist);
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
    Update();
    Print(FILE *file, char *processor, char *finalFormat, boolean topLevel);
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    DesiredSize(long width, long height, enum view_DSpass pass, long *dWidth, long *dheight) returns enum view_DSattributes;
    GetOrigin(long width, long height, long *originX, long *originY);
    ReceiveInputFocus();
    LoseInputFocus();
    PostMenus(struct menulist *menulist);
    SetDataObject(struct dataobject *dataobject);
    ObservedChanged (struct observable *changed, long value);
    LinkTree(struct view *parent);
    InitChildren();
methods:
    SetResizing(long key);
classprocedures:
    Create(struct viewref *vr,struct view *parent) returns struct matte *;
    InitializeClass() returns boolean;
    InitializeObject(struct matte *self) returns boolean;
    FinalizeObject(struct matte *self);
data:
    int desw,desh;
    struct view *child;
    struct viewref *ref;
    struct cursor *widthcursor, *heightcursor;
    int Moving,resizing,WasMoving, WasResizing;
    struct menulist *menus;
    int drawing, OldMode,sizepending;
};
