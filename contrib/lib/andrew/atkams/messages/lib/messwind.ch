/***********************************************************
		Copyright IBM Corporation 1991

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/messwind.ch,v 1.4 1991/09/12 20:11:13 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/messwind.ch,v $ */

#ifndef lint
static char *rcsid_messwind_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/messwind.ch,v 1.4 1991/09/12 20:11:13 bobg Exp $ ";
#endif /* lint */

class messwind: lpair {
    classprocedures:
      InitializeObject(struct messwind *self) returns boolean;
      InitializeClass() returns boolean;
      FinalizeObject(struct messwind *self);
    overrides:
      PostKeyState(struct keystate *keystate);
      PostMenus(struct menulist *ml);
    methods:
      ToggleSideBySide();
    data:
      struct captions *captions;
      struct folders *folders;
      struct view *foldapplayer;
      struct t822view *bview;
      struct lpair *capbodylp;
      int SideBySide;
      struct menulist *mymenulist;
      struct keystate *mykeys;

};

