/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/fad/RCS/fadv.ch,v 2.8 1991/09/12 19:39:35 bobg Exp $ */
/* $ACIS:fadv.ch 1.5$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/fad/RCS/fadv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidfadview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/fad/RCS/fadv.ch,v 2.8 1991/09/12 19:39:35 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#include <fad.ih>
class fadview[fadv] : view {
overrides:
	DesiredSize(long width, long height, enum view_DSpass pass, long *dWidth, long *dheight) returns enum view_DSattributes;
	Update();
    	FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
   	ReceiveInputFocus();
    	LoseInputFocus();
    	Hit (enum view_MouseAction action, long mousex, long mousey, long numberOfClicks) returns struct fadview *;
	Print(FILE *file, char *processor, char *finalFormat, boolean topLevel);
	ObservedChanged( struct observable *changed, long value);
	SetDataObject(struct dataobject *dataObject);
methods:
	aniframe(int framecount,int startat,int gofor,int mtm);
	nextframe(struct fad *cp);
	lastframe(struct fad *cp);
	showfad(int i,struct fad *cp);
	geticons(char *s)returns int;	
	fileread(char *fnm);

classprocedures:
InitializeObject(struct fadview *self) returns boolean;
FinalizeObject(struct fadview *self);
InitializeClass() returns boolean;
data:
	int HasFocus;
	struct keystate *keystate;
	struct menulist *menulist;
	int framecount;
	int startat;
	int gofor;
	int mtm;
	int DoAnimation;
	struct cursor *cursor;
	int Moving;
    int removed;
    int anbufsize;
    boolean Redraw,FrameChanged,animationPending,needUpdate,FocusChanged;
    struct fad_frame *f;
    int mode;
struct aniinfo *anobj;
struct event *nextevent;
boolean updatedue;
};
