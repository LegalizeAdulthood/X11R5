/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/messages.ch,v 2.11 1991/09/12 20:11:07 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/messages.ch,v $ */

#ifndef lint
static char *rcsid_messages_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/messages.ch,v 2.11 1991/09/12 20:11:07 bobg Exp $ ";
#endif /* lint */

#define WHATIAM_UNDEFINED -1
#define WHATIAM_FOLDERS 0
#define WHATIAM_CAPTIONS 1
#define WHATIAM_BODIES 3

class messages: textview[textv] {
    classprocedures:
      InitializeObject(struct messwind *self) returns boolean;
      InitializeClass() returns boolean;
      FinalizeObject(struct messwind *self);
    overrides:
      PostKeyState(struct keystate *keystate);
      PostMenus(struct menulist *ml);
      ObservedChanged(struct observable *changed, long change);
    methods:
      SetWhatIAm(int WhatIAm);
      ResetFileIntoMenus();
      AppendOneMessageToFile(int cuid, char *fname) returns int;
    data:
      struct menulist *mymenulist, *fileintomenulist, *expandedmenuslist;
      struct keystate *mykeys;
      int WhatIAm;
      struct foldertreev *folderTree;
      struct frame *folderFrame;
};

