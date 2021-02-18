/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/ptext/RCS/ptextv.ch,v 1.2 1991/09/12 20:16:32 bobg Exp $ */
/* $ACIS:ptextview.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/ptext/RCS/ptextv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidptextview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/ptext/RCS/ptextv.ch,v 1.2 1991/09/12 20:16:32 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class ptextview[ptextv]: textview[textv] {
  overrides:
    PostKeyState(struct keystate *keystate);
    PostMenus(struct menulist *menulist);
  classprocedures:
    InitializeClass() returns boolean;
    SetBounceTime(long time) returns long;
  data:
    struct keystate *c_state;
    struct menulist *c_menus;    
};
