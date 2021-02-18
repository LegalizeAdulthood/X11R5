/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
 * Return status values for StartNewDir
 */

#define diredview_Done         0
#define diredview_NoSuchDir   -1
#define diredview_NotADir     -2

class diredview[diredv]: textview[textv] {
classprocedures:
    InitializeClass() returns boolean;
    InitializeObject(struct diredview *self) returns boolean;
    FinalizeObject(struct diredview *self);
overrides:
    SetDataObject(struct basicobject *object);
    Hit(enum view_MouseAction action, long x, long y,
      long numberOfClicks) returns struct view *;
    PostKeyState(struct keystate *keystate);
    PostMenus(struct menulist *menus);
    ObservedChanged(struct observable *changed, long value);
methods:
data:
    struct keystate *keystate;
    struct menulist *menulist;
};
