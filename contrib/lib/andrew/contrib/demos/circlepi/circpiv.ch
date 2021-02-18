/* ********************************************************************** *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

class circlepiview[circpiv]: view {
  classprocedures:
    InitializeClass() returns boolean;
    InitializeObject(struct circlepiview *self) returns boolean;
    FinalizeObject(struct circlepiview *self);
  overrides:	
    ObservedChanged (struct observable *changed, long value);
    LinkTree(struct view *parent);
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);
    Update();
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    PostMenus(struct menulist *ml);
  data:
    int need_full_update;
    struct cursor *cursor;
    int color;
    struct menulist *ml;
};

