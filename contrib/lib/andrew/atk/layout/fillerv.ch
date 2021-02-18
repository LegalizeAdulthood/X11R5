/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/layout/RCS/fillerv.ch,v 1.2 1991/09/12 19:42:05 bobg Exp $ */
/* $ACIS$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/layout/RCS/fillerv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidspread_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/layout/RCS/fillerv.ch,v 1.2 1991/09/12 19:42:05 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* fillerview.ch - filler view definition and interface */
#define fillerview_VERSION 1

/* reference to the associated data object */

#define getView(V) ((V) -> header.view)
#define getIM(V) view_GetIM(&getView(V))
#define getDrawable(V) view_GetDrawable(&getView(V))
#define MyFiller(V) ((struct filler *)(getView(V).dataobject))

/* Interface definition */

class fillerview[fillerv]: celview[celv] {

overrides:
  FullUpdate(enum view_UpdateType how, long left, long top, long width, long height);
  Update();
  Hit(enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
  ReceiveInputFocus();
  LoseInputFocus();

methods:
  SetDataObjectByName(char *dataname);	/* replace myself */

classprocedures:
  InitializeClass() returns boolean;	/* initialize class */
  InitializeObject(struct fillerview *V) returns boolean;	/* initialize instance */
  FinalizeObject(struct	fillerview *V);	/* clean up instance */

data:
	boolean hasInputFocus;
        struct menulist *menulist;
	int hitindex;
};

/* end of fillerview.ch */

