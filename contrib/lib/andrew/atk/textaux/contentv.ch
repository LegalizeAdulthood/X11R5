/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/textaux/RCS/contentv.ch,v 1.4 1991/09/12 20:03:53 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/textaux/RCS/contentv.ch,v $ */

#ifndef lint
static char *rcsidcontentv_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/textaux/RCS/contentv.ch,v 1.4 1991/09/12 20:03:53 bobg Exp $";
#endif /* lint */

class contentv : textview [textv] {
overrides:
     PostMenus(struct menulist *menulist);
     SetDotPosition(long newpos);
     GetClickPosition(long position, long numberOfClicks, enum view_MouseAction action, long startLeft, long startRight, long *leftPos, long *rightPos);
classprocedures:
    InitializeClass() returns boolean;
    MakeWindow(struct text *self);
macromethods:
SetSourceView(vw) self->srcview = vw
data:
    struct menulist *menus;
    struct textview *srcview;
};
