/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

class t822view[text822v]: messages {
    overrides:
      PostKeyState(struct keystate *keystate);
      PostMenus(struct menulist *ml);
      DeleteApplicationLayer(struct view *scrollbar);
      SetDataObject(struct dataobject *do);
      ObservedChanged(struct text *changed, long value);
    methods:
      ShowHelp(char *text);
      SetCaptions(struct captions *cap);
      NewCaptionsInNewWindow() returns struct captions *;
      GetCaptions() returns struct captions *;
    classprocedures:
      InitializeClass() returns boolean;
      InitializeObject(struct textview *self) returns boolean;
      FinalizeObject(struct captions *self);
    data:
      struct captions *mycaps;
      struct keystate *myks;
      struct menulist *myml;
      boolean PriorReadOnliness;	/* for ESC-~ */
};
