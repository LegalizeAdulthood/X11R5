/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/typescript/RCS/tscript.ch,v 2.12 1991/09/12 20:04:33 bobg Exp $ */
/* $ACIS:tscript.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/typescript/RCS/tscript.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtypescript_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/typescript/RCS/tscript.ch,v 2.12 1991/09/12 20:04:33 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


#define READONLYMAX 65

/* Typescript menu masks. */
#define typescript_SelectionMenus 1   /* Menus to be posted when there is a selection region. */
#define typescript_NoSelectionMenus 2 /* Menus to be posted only when there is no
                                     * selection. Handles the bogus case of the
                                     * paste item which should show up all the
                                     * time.
                                     */
#define typescript_AfterFenceMenus 4 /* Menus to be posted for writable textviews. */

class typescript[tscript] : textview[textv] {
overrides:
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
    Update();
    ReceiveInputFocus();
    ObservedChanged (struct thisobject *changed, long value);
    PostMenus(struct menulist *menulist);
    GetClickPosition(long position, long numberOfClicks, enum view_MouseAction action, long startLeft, long startRight, long *leftPos, long *rightPos);
    SetDataObject(struct dataobject *dataObject);

methods:
    SetTitle(char *title);
    GetTitle() returns char *;
classprocedures:
    InitializeObject(struct typescript *self) returns boolean;
    Create(char **arglist,FILE *diskf,boolean filemenu) returns struct typescript *;
    CreatePipescript(FILE *indiskf, FILE *outdiskf, boolean filemenu) returns struct typescript *;
    InitializeClass() returns boolean;
    FinalizeObject(struct typescript *ap);
data:
    struct mark *cmdStart;	/* last place a new command was started */
    long lastPosition;		/* used for deciding to frame the dot when reading from the process */
    FILE *SCFile;               /* Sub Channel file * for BE 2file handler. */
    short SubChannel;           /* File descriptor of sub process i/o channel. */
    int SlaveChannel;          /* File descriptor of slave side of the pty. */
    short readOnlyLen;		/* Number of charaters in read only buffer. -1 if not in read only mode. */
    char readOnlyBuf[READONLYMAX]; /* Place to gather characters when in read only (no echo) mode. */
    char OutputWait;
    char ChopLines;
    struct keystate *keystate;
    struct menulist *menulist;
    char *progname;
    boolean pipescript;
    int	pgrpid;                /* process group id for signaling in no job-control systems */
    char *ptyname;             /* Name of the slave side pty device. */
    char *title;
    struct text *cmdText;
    long lastCmdPos;
};
