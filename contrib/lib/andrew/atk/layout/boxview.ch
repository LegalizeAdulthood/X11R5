/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/layout/RCS/boxview.ch,v 1.2 1991/09/12 19:41:58 bobg Exp $ */
/* $ACIS$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/layout/RCS/boxview.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidspread_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/layout/RCS/boxview.ch,v 1.2 1991/09/12 19:41:58 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* boxview.ch - box view definition and interface */
#define boxview_VERSION 1

/* internal interfaces */

#define getView(self) ((self) -> header.view)
#define getBox(self) ((struct box *)boxview_GetDataObject(self))


/* Interface definition */

class boxview: view {

overrides:
  FullUpdate(enum view_UpdateType how, long left, long top, long width, long height);
  Update();
  Hit(enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
  DesiredSize(long width, long height, enum view_DSpass pass, long *dWidth, long *dheight) returns enum view_DSattributes;
  WantNewSize(struct view *requestor);
  LinkTree(struct view *parent);
  UnlinkNotification(struct view *unlinkedview);
  ObservedChanged(struct observable *changed, long status);
  Print(FILE *f, char *processor, char *finalFormat, boolean toplevel);

methods:
  ToggleDebug();
  Paste();
  DrawBox();
  BoxWidth() returns int;

classprocedures:
  InitializeClass() returns boolean;
  InitializeObject(struct boxview *self) returns boolean;
  FinalizeObject(struct	boxview *self);

data:
	boolean	updateNeeded;		    /* the box itself needs to be updated */
	long lastUpdate;		    /* modification timestamp of last update */
	struct view *child;		    /* view of contents */
};

/* end of boxview.ch */

