/* ********************************************************************** *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#include <stdio.h>
#include <clock.ih>
#include <menulist.ih>
#include <fontdesc.ih>
#include <cursor.ih>

class clockview[clockv]: view {
 classprocedures:
  InitializeClass() returns boolean;
  InitializeObject(struct clockview *self) returns boolean;
  FinalizeObject(struct clockview *self);
 overrides:	
  FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);
  Update();
  Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
  Print(FILE *file, char *processor, char *finalFormat, boolean topLevel);
  PostMenus(struct menulist *ml);
 data:
  struct clock_time lastclockface;
  int need_full_update;
  struct cursor *cursor;
  struct fontdesc *myfontdesc;
  struct menulist *ml;
  long last_options_timestamp;
};

