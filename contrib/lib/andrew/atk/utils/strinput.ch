/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* Changed June 1990 njw */
/* A simple string input view, with a fenced prompt */

class strinput : view {
classprocedures:
  InitializeObject(struct thisobject *self) returns boolean;
  FinalizeObject(struct thisobject *self);
  InitializeClass() returns boolean;
overrides:
  FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);
  Hit(enum view_mouseAction action, long x, long y, long clicks) returns struct view *;
  ReceiveInputFocus();
  LinkTree(struct view *parent);
methods:
  GetInput(int length) returns char *;
  SetInput(char *string);
  ClearInput();
  SetPrompt(char *string);
  HaveYouGotTheFocus() returns boolean;
macromethods:
  IsLinked() ((self)->header.view.parent && (self)->header.view.imPtr)
data:
  struct text *textobj;
  struct textview *textv;
  char *prompt;
};
