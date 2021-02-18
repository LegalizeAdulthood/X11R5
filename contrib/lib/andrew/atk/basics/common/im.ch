/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/im.ch,v 2.30 1991/09/12 19:22:04 bobg Exp $ */
/* $ACIS:im.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/im.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidim_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/im.ch,v 2.30 1991/09/12 19:22:04 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */



/* im.H
 * Class header file for the interface manager.
 *
 *
 */

#include <stdio.h>

#define im_PROGRAMMMERVERSION 2

#define im_AllUp 0L
#define im_LeftDown 1L
#define im_RightDown 2L

/* these bits are used in the default configuration function for transients and override windows the top 16 bits in the rock provided to that function are reserved for future use.  the lower bits may be used in any way a particular im might desire, particular applications should use the custom rock to hold any data they want to give to their config function. */
#define im_InMiddle (1<<31)
#define im_Centered (1<<30)
#define im_AtTop (1<<29)
#define im_AtBottom (1<<28)

enum im_EventType  {
    im_NoEvent,
    im_MenuEvent,
    im_MouseEvent,
    im_KeyboardEvent,
    im_MacroEvent
};

struct im_ArgState {
    long argument;	/* the current argument */
    long tmpArgument;
    boolean argProvided;	/* true if argument is valid */
    boolean argNext;	/* if argProvided should be set on next command */
    boolean argDigit;	/* true if a digit has been seen on this argument */
    boolean argPending; /* true if we are currently trying to process a ^U as a command */
    boolean processCmd;
    long cmdpos;
    char argcmd[10];
};

struct im_InteractionEvent {
    struct im_InteractionEvent *next;
    struct im *im;
    struct event *event;
    procedure function;
    long data;
};

class im : view  {
overrides:
    WantUpdate(struct view *requestor);
    WantInputFocus(struct view *requestor);
    WantNewSize(struct view *requestor);
    WantHandler(char *handlerName) returns struct basicobject *;
    WantInformation(char *key) returns char *;
    PostKeyState(struct keystate *keystate);
    PostMenus(struct menulist *menulist);
    PostCursor(struct rectangle *rec,struct cursor *cursor) ;
    RetractCursor(struct cursor *cursor) ;
    RetractViewCursors(struct view *requestor);
    PostDefaultHandler(char *handlerName, struct basicobject *handler);
    GetResource( struct atomlist * name, struct atomlist * className,
		struct atom * type, long * data ) returns short;
    PostResource( struct atomlist * path, struct atom * type,
		 long data );
    GetManyParameters( struct resourceList * resources,
		      struct atomlist * name,
		      struct atomlist * className );		      
    UnlinkNotification(struct view *unlinkedTree);
    Hit (enum view_MouseAction action, long x, long y, long clicks) returns struct view *;
  
methods:
    SetView(struct view *toplevel);
    WhichWS() returns unsigned char *;  /* "X" or "wm" */
    GetCharacter() returns int;
    WasMeta() returns boolean;	/* Was the last ESC returned by
				 * GetCharacter a real escape,
				 * or part of a meta sequence? */
    DoKey(long key) returns struct im *; /* Simulates typing of key on window. */
    DoKeySequence(unsigned char *keys);  /* simulates typing string */
    HandleMenu(struct proctable_Entry *procTableEntry, struct basicobject *object, long rock) returns struct im *;
    HandleMouse(enum view_MouseAction action, long x, long y, long newButtonState) returns struct im *;
    DoMacro();
    DoMenu(unsigned char *itemname);

    /* These routines are used to handle argument passing with keystrokes */
    GetArgState() returns struct im_ArgState *;
    ClearArg();
    ArgProvided() returns boolean;
    Argument() returns long;
    ProvideArg(long arg);
    /* These routine manipulate late the global last command value. */
    GetLastCmd() returns long;	/* get it */
    SetLastCmd(long cmd);	/* set it */
    BumpArg(long digit) returns long; /* shift one place and add in new digit */
    DisplayArg();	/* display it */
    SetWindowCursor(struct cursor *cursor) ;
    ClearCursors(struct cursor * C);
    ClearCursorList();
    UpdateCursors();
    SetTitle(char *titleString);
    GetTitle() returns char *;
    FromCutBuffer() returns FILE *;
    ToCutBuffer() returns FILE *;
    CloseFromCutBuffer(FILE *readFile);
    CloseToCutBuffer(FILE *writeFile);
    RotateCutBuffers(long count);
    AppendToCutBuffer(FILE *writeFile);
    SetInteractionEvent(procedure function, long functionData, long time) returns struct event *;
    CancelInteractionEvent(struct event *event);
    DispatchPendingInteractionEvents(); /* This is an internal only method. */
    /* maybe this should be done by HandleRedraw */
    RedrawWindow();

    /* These are window manager interface calls */
    SetWMFocus();
    ExposeWindow();
    HideWindow();	/* iconified */
    VanishWindow();	/* totally invisible */
    HandleRedraw();
    CreateWindow(char *host) returns boolean;
    CreateTransientWindow(struct im *other) returns boolean;
    CreateOverrideWindow(struct im *other) returns boolean;
    SupportsTransient() returns boolean;
    SupportsOverride() returns boolean;
    SupportsOffscreen() returns boolean;
    CreateOffscreenWindow(struct im *other, long width, long height) returns boolean;
    SetBorderWidth(long n);
    GetLoc(struct view *v, struct rectangle *r) returns struct rectangle *;
    NormalConfiguration(long rock, long customrock, struct im *parent, long *x, long *y, long *w, long *h);

macros:
    GetInputFocus(self) (((struct im *)self)->inputFocus)
/* these provide a way to modify the configure function's operation (the configure function controls size and placement) for a transient or override */
    ConfigureFunction(self) (((struct im *)self)->configfunc)
    ConfigureRock(self) (((struct im *)self)->configrock)
    ConfigureCustomRock(self) (((struct im *)self)->configcustomrock)

classprocedures:
    AllocLastCmd() returns long;	/* allocate a value for it */
    InitializeClass() returns boolean;
    InitializeObject(struct im *self) returns boolean;
    FinalizeObject(struct im *self);
    Create(char *host) returns struct im *;
    CreateTransient(struct im *other) returns struct im *;
    CreateOverride(struct im *other) returns struct im *;
    AddFileHandler(FILE *file, procedure proc, char *procdata, long priority) returns boolean;
    RemoveFileHandler(FILE *file);
    AddCanOutHandler(FILE *file, procedure proc, char *procdata, long priority) returns boolean;
    RemoveCanOutHandler(FILE *file);
    Interact(boolean mayBlock) returns boolean;
    SetCleanUpZombies(boolean value);
    AddZombieHandler(int pid, procedure function, long functionData);
    RemoveZombieHandler(int pid);
    SignalHandler(long signalNumber, procedure proc, char *procdata);
    ChangeDirectory(char *dirName) returns long;
    GetDirectory(char *outputString) returns char *;
    KeyboardExit();
    KeyboardLevel() returns long;
    KeyboardProcessor();
    EnqueueEvent(procedure proc, char *procdata, long timeIncrement) returns struct event *;
    IsPlaying() returns boolean;
    CheckForInterrupt() returns boolean;  /* scans ahead for control-G */
    ForceUpdate();
    SetProcessCursor(struct cursor *cursor) ;
    GetProcessCursor() returns struct cursor *;
    GetWriteID() returns long;
    SetProgramName(char *nameString);
    GetProgramName() returns char *;
    SetGlobalInit(struct init *init);
    GetGlobalInit() returns struct init *;
    SetPreferedDimensions(long top, long left, long width, long height);
    GetPreferedDimensions(long *top, long *left, long *width, long *height);
    SetLastUsed(struct im *used);
    GetLastUsed() returns struct im *;

    /* Private declarations for use by other interaction managers */
    GetWindowSystem() returns struct windowsystem *;
    DeliverSignals();
    GetCursor() returns struct cursor *;
    GetFontdesc() returns struct fontdesc *;
    GetGraphic() returns struct graphic *;
    GetGlobalData() returns struct im_GlobalDataType *;
    IOMGRCancel(char * imPid);
    IOMGRSelect(long maxnum,long *rmask,long *wmask,long *emask,struct timeval *retTime) returns boolean;
    IOMGRSoftSig(procedure aproc, char *arock);
    LWPCurrentProcess(char * curProcessID) returns long;
    vfileopen(char * mode, struct expandstring *buffer) returns FILE *;
    vfileclose(FILE *f, struct expandstring *buffer);
    vfilecleanup();
    plumber(FILE * reportFile);

    RedrawChangedWindows();
    SetDefaultServerHost(char *name);
    SetGeometrySpec(char *value);
    CancelMacro();
    DefaultConfigureRock(long pos) returns long;
    DefaultConfigureFunction(procedure func) returns procedure;
    DefaultConfigureCustomRock(long rock) returns long;
    CreateOffscreen(struct im *other, long width, long height) returns struct im *;
   
data:
    boolean WasMeta;		/* TRUE if GetCharacter returned
				 * an "invented" escape */
    struct atom * programNameAtom;
    struct im *next;			/* Next im allocated object */
    boolean doRedraw;			/* TRUE if redraw must be done */
    boolean inRedraw;			/* TRUE if in redraw routine */
    struct view *topLevel;		/* The top level view connected to this object */
    struct view *inputFocus;		/* Current view that is highlighted */
    struct view *mouseFocus;		/* View that is receieving mouse events */
    long buttonState;		/* button state at output of queue */
    struct keystate *imKeystate;	/* Key state for im object */
    struct keystate *keystate;		/* Key state that is being used for input */
    enum im_EventType lastEvent;
    long lastX;
    long lastY;
    enum view_MouseAction lastMouseDown;
    long clickCount;
    struct im_ArgState argState;	/* structure for maintaining argument info */
    long lastCommand;
    struct menulist *menus;
    struct im_InteractionEvent *interactionEvents;
    struct im_InteractionEvent *pendingInteractionEvents;
    struct cursor *cursorlist;
    struct cursor *WindowCursor;
    int CursorsAreActive;
    struct handler *handlers;
    char *title;
    char *programName;
    struct init *init;
    long cursorPostsPending;
    FILE *LogFile;

    /* these are for echoing keys in the message line */
    char keyEcho[40];
    enum {im_KeyEchoOff, im_KeyEchoPending, im_KeyEchoDisplayed}
		 keyEchoState;
    struct event *keyEchoEvent;
    procedure configfunc;
    long configrock, configcustomrock;
    struct im *moreRecentlyUsed, *lessRecentlyUsed; /* Links for GetLastUsed
						     * queue */
};

/* Private declarations for use only by interaction managers -- to get access to these data, interaction managers must turn on the flag "INTERACTION_MANAGER" */

#ifdef INTERACTION_MANAGER

/* Global data for im and its children. This structure is used to pass global data back and forth among winodw manager dependent modules (too bad we don't have real dynamic loading) */

#define NUMFILE 16

struct FILEHandlers {
    FILE *file;
    int (*proc)();
    char *procdata;
    int priority;
};

struct expandstring  {
    char *string;
    long pos;
    long length;
    long size;
};


/* Here is the actual state to be shared among everyone */

struct im_GlobalDataType {
    char *g_initialProgramName;
    boolean g_setDimensions;
    char *g_geometrySpec;
    long g_preferedTop, g_preferedLeft, g_preferedWidth, g_preferedHeight;

    struct timeval g_PollTime;/* static to allow FlagRedraw to change it */
    struct im *g_imList;
    boolean g_doRedraw;
    struct updatelist *g_globalUpdateList;
    struct cursor *g_ProcessCursor;
    char *g_imPid;		/* lwp pid of im process currently in a select call. */
    struct init *g_globalInit;
    boolean g_anyDelivered;
    struct FILEHandlers g_FILEHandlers[NUMFILE];   /* Input Handlers */
    long g_NFILEHandlers;
    struct FILEHandlers g_CanOutHandlers[NUMFILE];  /* Output Handlers */
    long g_NCanOutHandlers;
    struct expandstring g_readCutBuffer;
    struct expandstring g_writeCutBuffer;
    char *g_defaultServerHost;
    void (*g_enQuserKey)();
    void (*g_enQuserMouse)();
    void (*g_enQuserMenu)();
};


/* Mongo macros to promote ease of transition from old to new */

#define initialProgramName (gData->g_initialProgramName)
#define setDimensions (gData->g_setDimensions)
#define geometrySpec (gData->g_geometrySpec)
#define preferedTop (gData->g_preferedTop)
#define preferedLeft (gData->g_preferedLeft)
#define preferedWidth (gData->g_preferedWidth)
#define preferedHeight (gData->g_preferedHeight)

#define PollTime (gData->g_PollTime)
#define imList (gData->g_imList)
#define globalDoRedraw (gData->g_doRedraw)
#define globalUpdateList (gData->g_globalUpdateList)
#define ProcessCursor (gData->g_ProcessCursor)
#define imPid (gData->g_imPid)
#define globalInit (gData->g_globalInit)
#define anyDelivered (gData->g_anyDelivered)
#define globalFILEHandlers (gData->g_FILEHandlers)
#define NFILEHandlers (gData->g_NFILEHandlers)
#define CanOutHandlers (gData->g_CanOutHandlers)
#define NCanOutHandlers (gData->g_NCanOutHandlers)
#define readCutBuffer (gData->g_readCutBuffer)
#define writeCutBuffer (gData->g_writeCutBuffer)
#define defaultServerHost (gData->g_defaultServerHost)
#define enQuserKey (gData->g_enQuserKey)
#define enQuserMouse (gData->g_enQuserMouse)
#define enQuserMenu (gData->g_enQuserMenu)

#define IM_METAESC (-22)	/* This value is used to
				 * tell the event queue that a
				 * meta-foo keypress has been
				 * converted to esc, then foo */

#endif /* INTERACTION_MANAGER */
