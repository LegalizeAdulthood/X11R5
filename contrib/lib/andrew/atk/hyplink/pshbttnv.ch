/* ********************************************************************** *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/*
  Trigger:  "buttonpushed"
     called when the user lets up on the pushbutton over the pushbutton.

*/

class pushbuttonview[pshbttnv]: view {
  classprocedures:
    InitializeClass() returns boolean;
    InitializeObject(struct pushbuttonview *self) returns boolean;
    FinalizeObject(struct pushbuttonview *self);
  overrides:	
    ObservedChanged (struct observable *changed, long value);
    DesiredSize(long width, long height, enum view_DSpass pass, long * desired_width, long * desired_height) returns enum view_DSattributes;
    GetOrigin(long width, long height, long * originX, long * originY);
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);
    Update();
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    LinkTree(struct view *parent);
    PostMenus(struct menulist *ml);
    WantUpdate(struct view *requestor);
  data:
    short lit;
    struct cursor *cursor;
    double foreground_color[3], background_color[3];
    char *cached_label;
    int cached_style;
    struct fontdesc *cached_fontdesc;
    struct menulist *ml;
    int awaitingUpdate;
};

