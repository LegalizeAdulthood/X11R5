/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/lsetv.ch,v 2.7 1991/09/12 19:19:21 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/lsetv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_lsetview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/lsetv.ch,v 2.7 1991/09/12 19:19:21 bobg Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#define lsetview_UnInitialized 0
#define lsetview_MakeHorz 1
#define lsetview_MakeVert 2
#define lsetview_INSERTVIEW 3
#define lsetview_HasView 4
#define lsetview_Initialized 5
#define lsetview_IsSplit 6
#define lsetview_UpdateView 7
#define lsetview_FirstUpdate 8
#define lsetview_NoUpdate 9

class lsetview[lsetv]:lpair {
overrides:
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
    Update();
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    ReceiveInputFocus();
    LoseInputFocus();
    SetDataObject(struct dataobject *ls);
    ObservedChanged (struct observable *changed, long value);
    LinkTree(struct view *parent);
    WantNewSize(struct view *requestor);
    Print(FILE *file, char *processor, char *finalFormat, boolean topLevel);
    InitChildren();
    CanView(char *TypeName) returns boolean;
methods:
    ReadFile(FILE *thisFile,char *iname);
    Unsplit(struct lsetview *who) returns boolean;
classprocedures:
    InitializeClass()returns boolean;
    Create(int level,struct lset *ls,struct view *parent) returns struct lsetview *;
    InitializeObject(struct lsetview *self) returns boolean;    
    FinalizeObject(struct lsetview *self);
data:
	int HasFocus;
	struct keystate *keystate;
	struct menulist *menulist;
	struct cursor *cursor;
	int mode;
	int level;
    struct view *child,*app;
    int promptforparameters;
    struct text *pdoc;
    int revision;
};

