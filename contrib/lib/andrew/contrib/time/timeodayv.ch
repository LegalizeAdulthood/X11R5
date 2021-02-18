/* ********************************************************************** *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#include <stdio.h>
#include <menulist.ih>
#include <cursor.ih>

class timeodayview[timeodayv]: view {
 classprocedures:
  InitializeClass() returns boolean;
  InitializeObject(struct timeodayview *self) returns boolean;
  FinalizeObject(struct timeodayview *self);
 overrides:	
  FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);
  Update();
  Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
  Print(FILE *file, char *processor, char *finalFormat, boolean topLevel);
  DesiredSize(long width, long height, enum view_DSpass pass, long * desired_width, long * desired_height) returns enum view_DSattributes;
  GetOrigin(long width, long height, long * originX, long * originY);
  PostMenus(struct menulist *ml);
 macromethods:
  GetMenuList() (self->ml)
  SetMenuList(ml) (self->ml = (ml))
 data:
  int need_full_update;
  struct cursor *cursor;
  struct menulist *ml;
  long last_width, last_height;
};

