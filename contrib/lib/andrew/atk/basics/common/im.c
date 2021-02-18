/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/im.c,v 2.56 1991/09/12 15:58:41 bobg Exp $ */
/* $ACIS:im.c 1.5$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/im.c,v $ */

#ifndef lint
char *im_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/im.c,v 2.56 1991/09/12 15:58:41 bobg Exp $";
#endif /* lint */

/* Put in error messages for handling of keystrokes.
Figure out how to handle handlers and information requests.
Figure out some way to handle levels of user.  Macros should probably not be an novice level facility. */

#include <andrewos.h> /* sys/time.h sys/file.h sys/types.h */
#include <class.h>
#include <signal.h>
#ifndef MAXPATHLEN
#include <sys/param.h>
#endif

#include <andyenv.h>
#define INTERACTION_MANAGER
#include <im.eh>
#undef INTERACTION_MANAGER
#include <atomlist.ih>
#include <rm.ih>
#include <atom.ih>
#include <graphic.ih>
#include <view.ih>
#include <event.ih>
#include <keymap.ih>
#include <keystate.ih>
#include <environ.ih>
#include <proctbl.ih>
#include <menulist.ih>
#include <updlist.ih>
#include <cursor.ih>
#include <physical.h>
#include <message.ih>
#include <init.ih>
#include <bind.ih>
#include <winsys.ih>
#include <profile.ih>

#include <sys/wait.h>	/* for pruning zombies */ 
#include <netinet/in.h>	/* for byte ordering in logs */

#define HITPIXELS 1

/* For use with PostDefaultHandler and WantHandler */
struct handler {
    char *name;
    struct basicobject *handler;
    struct handler *next;
};

static struct windowsystem *currentWS = NULL; /* Used to distiguish window systems. */

static struct atom * A_application;
static struct atom * ProgramNameAtom;

/* Handlers for im_AddZombieHandler function. */
struct zombiehandler {
    struct zombiehandler *next;
    int pid;
    procedure function;
    long functionData;
};

static boolean childDied;
static boolean cleanUpZombies;
static long eventCount;
static boolean doRecord;
static boolean playingRecord;
static long thisCmd;		/* Used for deciding the last command that was executed */
static boolean keyboardExitFlag;
static long keyboardLevel;
static struct zombiehandler * allZombieHandlers; /* Handlers for im_AddZombieHandler function. */
static int (*(sigProcs[NSIG]))();
static char *sigData[NSIG];
static boolean sigDelivered[NSIG];
static struct keymap *globalKeymap;
static struct menulist *imMenus;
static long destroycount;
static boolean allowCtrlUCmds;

static void userKey(), userMouse(), userMenu();
static char *getMenuEntryName();
static boolean getMenuEntry();
static SetArgProvided();
static void FreeInteractionEvents();
/* Everyone uniformly references this data through a pointer, 
		declared below and statically allocated in im */

static struct im_GlobalDataType im_GlobalData;
static struct im_GlobalDataType * gData = &im_GlobalData;

static void InitGlobalStructure() {

    initialProgramName = NULL;
    setDimensions = FALSE;
    geometrySpec = NULL;
    preferedTop = 0;
    preferedLeft = 0;
    preferedWidth = 500;
    preferedHeight = 500;

    imList = NULL;
    globalDoRedraw = FALSE;
    globalUpdateList = NULL;
    ProcessCursor = NULL;
    imPid = NULL;
    globalInit = NULL;
    anyDelivered = FALSE;
    NFILEHandlers = 0;  
    NCanOutHandlers = 0;  
    allZombieHandlers = NULL;
    readCutBuffer.string = NULL;
    readCutBuffer.pos = readCutBuffer.length = readCutBuffer.size = 0;
    writeCutBuffer.string = NULL;
    writeCutBuffer.pos = writeCutBuffer.length = writeCutBuffer.size = 0;
    defaultServerHost = NULL;
    enQuserKey = userKey;
    enQuserMouse = userMouse;
    enQuserMenu = userMenu;
}

/* Everyone gets the global data pointer through the following im class procedure */

struct im_GlobalDataType * im__GetGlobalData(classID)
struct classheader * classID; {
    return gData;
}



static char *charToPrintable(c)
long c;
{
    static char s[8];

    if (c==9)
	strcpy(s,"Tab");
    else if(c == 27)
        strcpy(s, "Esc");
    else if (c < 32)
        s[0] = '^', s[1] = c + '@', s[2] = '\0';
    else if(c==32)
	strcpy(s,"Spc");
    else if (c < 127)
        s[0] = c, s[1] = '\0';
    else if (c == 127)
        strcpy(s, "Del ");
    else {
        s[0] = '\\';
        s[1] = '0' + (c > 127);
        s[2] = '0' + ((c & 56) >> 3);
        s[3] = '0' + (c & 3);
        s[4] = '\0';
    }

    return s;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *   log file - set environment variable  LOGDIRECTORY  to the name of a directory
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* Write an entry to the log file.  This is for use in studying the
   use of the editor.  Log entries are ASCII
	time	seconds since start of program
	code
	value
	newline

   The codes and their related values are:
	space	a single key that was typed
	@	host name
	:	title line (usually the file name)
	-	nothing - a menu selection was made
	! 	mouse down,  followed by coordinates
	^	mouse up, followed by coordinates
   To save space, mouse coordinates are divided by 16 and the remainders discarded
 */
#define log_KEY		' '
#define log_HOST  	'@'
#define log_TITLE  	':'
#define log_MENU  	'-'
#define log_MOUSEDOWN	'!'
#define log_MOUSEUP	'^'

static long LogStart;	/* time log started */

	void
WriteLogEntry (self, code, str)
	struct im *self;
	unsigned char code;
	char *str;
{
	long now = time(0);
	if (now - LogStart > 600) {
		LogStart = now;
		fprintf(self->LogFile, "%s", ctime(&now));
	}
	if (self->LogFile == NULL) return;
	if(str == NULL) str="NULL host";
	else {
	    if (str[1] == '\0') 
		str = charToPrintable(*str);
	}
	fprintf(self->LogFile, "%d%c%s\n", now - LogStart, code, str);
}

	void
WriteLogXY (self, code, x, y)
	struct im *self;
	unsigned char code;
	long x, y;
{
	if (self->LogFile == NULL) return;
	fprintf(self->LogFile, "%d%c%d,%d\n", time(0) - LogStart, code, x>>4, y>>4);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *	action package - input event queue routines and definitions
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

struct action {
	enum im_EventType type;
	struct action *next;
	struct im *im;
	union {
		long key;
		struct {
 			struct proctable_Entry *procTableEntry;
			struct basicobject *object;
   			long rock;
		} menu;
		struct {
			enum view_MouseAction action;
			long x;
			long y;
			long newButtonState;
		} mouse;
		struct {
			struct action *macro;
			struct action *nextaction;
			long remainingrepetitions;
		} macro;
	} v;
};


/* NOTE - at present there is ONE input queue for the entire process 
	It is in global data as g_InQ  and accessible via macro InQ
*/
/* If there is  to be one queue per window, then enqAction and stackAction 
	need to have 'self' as a first argument */

static struct action *InQ = NULL;
static struct action *Record = NULL;
static struct action *FreeQ = NULL;
static struct action *PendingRelease = NULL;

/* newAction()
	allocate an empty action block 
*/
	struct action *
newAction()
{
	register struct action *a;
	if (FreeQ == NULL) 
		return (struct action *)malloc(sizeof(struct action));
	a = FreeQ;
	FreeQ = a->next;
	return a;
}

/* cloneAction(a)
	allocate an empty action block 
*/
	struct action *
cloneAction(a)
	register struct action *a;
{
	register struct action *new = newAction();
	if (a == NULL) return NULL;
	*new = *a;
	new->next = NULL;
	return new;
}

/* stackAction(Q, a)
	put an action at the front of a queue
*/
	void
stackAction(Q, a)
	register struct action **Q, *a;
{
	if (a == NULL) return;	/* the malloc failed */
	a->next = *Q;
	*Q = a;
}

/* enqAction(Q, a)
	put an action at the rear of a queue
*/
	void
enqAction(Q, a)
	register struct action **Q, *a;
{
	if (a == NULL) return;	/* the malloc failed */
	a->next = NULL;
	for ( ; *Q != NULL; Q = &((*Q)->next)) {}
	*Q = a;
}

/* freeQelt(Q)
	return action element to FreeQ
*/
	void
freeQelt(Q)
	struct action *Q;
{
	Q->next = FreeQ;
	FreeQ = Q;
}

/* freeQlist(Q)
	return list of action elements to FreeQ
*/
	void
freeQlist (Q)
	struct action *Q;
{
	struct action *a;
	if (Q == NULL) return;
	for (a = Q; a->next != NULL; a = a->next) {}
	a->next = FreeQ;
	FreeQ = Q;
}

/* pruneActions(im)
	remove all interactions for the given im from the queue InQ
	(no need to prune PendingRelease because its im field is not used)
*/
	void
pruneActions(im)
	struct im *im;
{
	register struct action *a, *p, *n;
	for (p = NULL, a = InQ; a != NULL; a = n) {
		n = a->next;
		if (a->im == im) {
			/* dequeue this element */
			if (p == NULL) InQ = n;
			else p->next = n;
			freeQelt(a);
		}
		else {
		    p = a;
		}
	}
}

/* keyAction(im, k)
	allocate an action block for a keystroke
*/
	struct action *
keyAction(im, k)
	struct im *im;
	register long k;
{
	register struct action *a = newAction();
	if (a == NULL) return NULL;
	a->type = im_KeyboardEvent;
	a->im = im;
	a->v.key = k;
	return a;
}

/* mouseAction(im, act, x, y, newButtonState)
	allocate an action block for a mouse hit
*/
	struct action *
mouseAction(im, act, x, y, newButtonState)
	struct im *im;
	enum view_MouseAction act;
	long x;
	long y;
	long newButtonState;
{
	register struct action *a = newAction();
	if (a == NULL) return NULL;
	a->type = im_MouseEvent;
	a->im = im;
	a->v.mouse.action = act;
	a->v.mouse.x = x;
	a->v.mouse.y = y;
	a->v.mouse.newButtonState = newButtonState;
	return a;
}

/* menuAction(im, procTableEntry, object, rock)
	allocate an action block for a menu selection
*/
	struct action *
menuAction(im, procTableEntry, object, rock)
	struct im *im;
 	struct proctable_Entry *procTableEntry;
	struct basicobject *object;
   	long rock;
{
	register struct action *a = newAction();
	if (a == NULL) return NULL;
	a->type = im_MenuEvent;
	a->im = im;
	a->v.menu.procTableEntry = procTableEntry;
	a->v.menu.object = object;
	a->v.menu.rock = rock;
	return a;
}

/* macroAction(im, macro, nextaction, remainingrepetitions)
	allocate an action block for a macro playback action
*/
	struct action *
macroAction(im, macro, nextaction, remainingrepetitions)
	struct im *im;
	struct action *macro;
	struct action *nextaction;
	long remainingrepetitions;
{
	register struct action *a = newAction();
	if (a == NULL) return NULL;
	a->type = im_MacroEvent;
	a->im = im;
	a->v.macro.macro = macro;
	a->v.macro.nextaction = nextaction;
	a->v.macro.remainingrepetitions = remainingrepetitions;
	return a;
}

	static void
userKey(self, key)
	register struct im *self;
	long key;
{
	struct action *a;
	if (self->LogFile != NULL) {
		char buf[2];
		buf[0] = ((key == IM_METAESC) ? '\033' : key);
		buf[1] = '\0';
		WriteLogEntry(self, log_KEY, buf);
	}

	a = keyAction(self, key);
	enqAction(&InQ, a);
	if (doRecord)   enqAction(&Record, cloneAction(a));
}

	static void
userMouse(self, act, x, y, newButtonState)
	register struct im *self;
	enum view_MouseAction act;
	long x;
	long y;
	long newButtonState;
{
	struct action *a;
	if (self->LogFile != NULL) switch (act) {
		case view_RightDown:
		case view_LeftDown:
			WriteLogXY(self, log_MOUSEDOWN, x, y);    break;
		case view_RightUp:
		case view_LeftUp:
			WriteLogXY(self, log_MOUSEUP, x, y);    break;
	}

	a = mouseAction(self, act, x, y, newButtonState);
	enqAction(&InQ, a);
	if (doRecord)   enqAction(&Record, cloneAction(a));
}

	static void
userMenu(self, procTableEntry, object, rock)
	register struct im *self;
 	struct proctable_Entry *procTableEntry;
	struct basicobject *object;
   	long rock;
{
	struct action *a;
	if (self->LogFile != NULL) 
		WriteLogEntry(self, log_MENU,
			 getMenuEntryName(self->menus, procTableEntry,
				object, rock));

	a = menuAction(self, procTableEntry, object, rock);
	enqAction(&InQ, a);
	if (doRecord)   enqAction(&Record, cloneAction(a));
}


/* GetNextInputEvent()
	get the next event off the input queue
	This routine knows about finding macros on the queue.
	Returns NULL if there are no events.
	Returns a struct action if there is an event.

	The event remains the property of the event queue
	and is only guaranteed to exist until the next call to this routine.

	The caller of this routine must use the event IMMEDIATELY.

*/
	struct action *
GetNextInputEvent()
{
	if (PendingRelease != NULL) {
		if (PendingRelease->type == im_MacroEvent) {
			struct im *im;
			playingRecord = FALSE;

			/* cursors have been disabled,  update them all */
			for (im = imList; im != NULL; im = im->next)
				im_UpdateCursors(im);
		}
		freeQelt(PendingRelease);
		PendingRelease = NULL;
	}
	if (InQ == NULL) return NULL;
	if (InQ->type == im_MacroEvent) {
		/* peel off one element of the macro */
		struct action *a;
		a = InQ->v.macro.nextaction;
		if (a->next == NULL) {
			/* reduce repetitions or remove macro elt */
			if (--InQ->v.macro.remainingrepetitions <= 0) {
				PendingRelease = InQ;
				InQ = InQ->next;
			}
			else {
				/* do another iteration.  First check for ^G */
				if (im_CheckForInterrupt()) {
					/* (the macro has been
					   flushed from the Q 
					   by im_CheckForInterrupt()) */
					return NULL;
				}
				InQ->v.macro.nextaction = InQ->v.macro.macro;
			}
		}
		else
			InQ->v.macro.nextaction = a->next;
		return a;
	}
	else {
		PendingRelease = InQ;
		InQ = InQ->next;
		return PendingRelease;
	}
}

/* PeekInputEvent()
	get a reference to the next event which will come off the input queue
	This routine knows about finding macros on the queue.
	Returns NULL if there are no events.
	Returns a struct action if there is an event.

	The event remains the property of the event queue
	and is only guaranteed to exist until 
		the second subsequent call to GetNextInputEvent()

	The caller of this routine must use the event IMMEDIATELY.

*/
	struct action *
PeekInputEvent()
{
	if (InQ == NULL) return NULL;
	if (InQ->type == im_MacroEvent)
		return InQ->v.macro.nextaction;
	else 
		return InQ;
}

/* im__DoKeySequence(self, keys)
	queues the sequence of keys to be executed as if a macro
*/
	void
im__DoKeySequence(self, keys)
	struct im *self;
	unsigned char *keys;
{
	struct action *old;
	old = InQ;
	for ( ; *keys; keys++)
		enqAction(&InQ, keyAction(self, *keys));
	while (old != InQ)
		im_Interact(FALSE);
}



/* Special stubs so that everyone can use the one shared version of the LWP and vfile package */

void im__IOMGRCancel(classID,localImPid)
struct classheader * classID;
char * localImPid; /* actually of type PROCESS which is a point to a lwp_pcb struct*/
{
#ifdef LWP
    IOMGR_Cancel(localImPid);
#endif /* LWP */
}

void im__IOMGRSoftSig(classID,aproc,arock)
struct classheader * classID;
procedure aproc;
char * arock;
{
#ifdef LWP
    IOMGR_SoftSig(aproc,arock);
#endif /* LWP */
}

boolean im__IOMGRSelect(classID,maxnum,rmask,wmask,emask,timeOut)
struct classheader * classID;
long maxnum;
long *rmask;
long *wmask;
long *emask;
struct timeval * timeOut;
{
    long ret = 0;
#ifdef LWP
    ret = IOMGR_Select(maxnum,rmask,wmask,emask,timeOut);
#endif /* LWP */
    return ret;
}

long im__LWPCurrentProcess(classID,curProcessID)
struct classheader * classID;
char * curProcessID; /* really type *PROCESS */ {
    long retValue = 0;
#ifdef LWP
    retValue = LWP_CurrentProcess(curProcessID);
#endif /* LWP */
    return retValue;
}

#define NUMBEROFVFILES 4
#define INITIALSIZE 200

struct vfile  {
    FILE *file;
    char name[30];
    long size;
    char mode;
    char used;
};

static struct vfile vFiles[NUMBEROFVFILES];
static struct vfile *lastVFile;

static struct vfile *GetUnUsedVfile()
{
    register int i;

    for (i = 0; i < NUMBEROFVFILES; i++)  {
	if (! vFiles[i].used)  {
	    if (vFiles[i].file == NULL)  {
		register FILE *f;

		sprintf(vFiles[i].name, "/tmp/cb%d-XXXXXX", i);
		mktemp(vFiles[i].name);
		
		f = fopen(vFiles[i].name, "w+");
		if (f == NULL)
		    continue;

		vFiles[i].file = f;
#ifdef M_UNIX
		chmod(vFiles[i].name, 0600);
#else
		fchmod(fileno(f), 0600);
#endif
                unlink(vFiles[i].name); /* Get rid of the name in the filesystem. */
		vFiles[i].size = 0;
		vFiles[i].mode = ' ';
	    }
	    vFiles[i].used = 1;
	    lastVFile = &(vFiles[i]);
	    return lastVFile;
	}
    }
    return NULL;
}

static struct vfile *GetCorrespondingVFile(f)
FILE *f;
{
    register int i;

    for (i = 0; i < NUMBEROFVFILES; i++)  {
	if (vFiles[i].used && vFiles[i].file == f)
	    return (&(vFiles[i]));
    }

    return NULL;
}


FILE *im__vfileopen(classID, mode, buffer)
struct classheader * classID;
char *mode;
struct expandstring *buffer;
{
    FILE *f;
    struct vfile *vf;

    vf = GetUnUsedVfile();
    if (vf == NULL)
        return NULL;
    f = vf->file;

    if (*mode == 'r')  {
	rewind(f);
	fwrite(buffer->string, sizeof (char), buffer->length, f);
	rewind(f);
	if (buffer->length < vf->size)  {
	    ftruncate(fileno(f), buffer->length);
	}
	vf->size = buffer->length;
    }
    else if (*mode == 'w')  {
	rewind(f);
    }
    else  {
	vf->used = 0;
        return NULL;
    }

    vf->mode = *mode;

    return f;
}

void im__vfileclose(classID, f, buffer)
struct classheader *classID;
FILE *f;
struct expandstring *buffer;
{
    struct vfile *vf;

    vf = (f == lastVFile->file) ? lastVFile : GetCorrespondingVFile(f);

    if (vf == NULL)
        return;

    if (vf->mode == 'r')  {
	rewind(f);
    }
    else {
	if (buffer != NULL)  {
	    buffer->length = ftell(f);
	    if (buffer->size < buffer->length + 1)  {
		if (buffer->string == NULL)  {
		    buffer->size = (INITIALSIZE > buffer->length + 1) ? INITIALSIZE : (buffer->length + 1);
		    buffer->string = (char *) malloc(buffer->size);
		}
		else {
		    buffer->size = buffer->length + 1;
		    buffer->string = (char *) realloc(buffer->string, buffer->size);
		}
	    }
	    if (buffer->length > vf->size)
	        vf->size = buffer->length;
	    rewind(f);
	    fread(buffer->string, sizeof (char), buffer->length, f);
	    buffer->string[buffer->length] = '\0';
	}
    }
    vf->used = 0;
}


void im__vfilecleanup(classID)
struct classheader *classID;
{
    register int i;

    for (i = 0; i < NUMBEROFVFILES; i++)  {
	if (vFiles[i].file != NULL)  {
	    fclose(vFiles[i].file);
	    unlink(vFiles[i].name);
	}
    }
}


void im__plumber(classID, reportFile)
struct classheader * classID;
FILE * reportFile; {
#if defined(ANDREW_MALLOC_ENV) && defined(DEBUG_MALLOC_ENV)
    plumber(reportFile);
#endif /* #if defined(ANDREW_MALLOC_ENV) && defined(DEBUG_MALLOC_ENV) */
}

void im__SetDefaultServerHost(classID, value)
struct classheader *classID;
char *value;  {
    char *buffer;

    buffer = malloc(strlen(value) + 1);
    strcpy(buffer, value);
    if (defaultServerHost != NULL)  {
	free(defaultServerHost);
    }
    defaultServerHost = buffer;
}

#ifdef LWP
/* called on iomgr lwp's stack at a safe time*/
static int WakeUpIM(dummy)
    char *dummy;
{
    if (imPid != NULL)
        IOMGR_Cancel(imPid);
}
#endif /* LWP */

static DeathInTheFamily() {
    childDied = TRUE;
}


static void startKeyEchoing(self,time)
struct im *self;
long time;
{
    if(self->keyEchoState==im_KeyEchoPending){
	self->keyEchoState=im_KeyEchoDisplayed;
	message_DisplayString(self,0,self->keyEcho);
    }

    self->keyEchoEvent=NULL;
}

#define KEYECHODELAY 750 /* msec */

static void echoKey(self,key,pending)
struct im *self;
long key;
int pending;
{
    if(self->keyEchoState==im_KeyEchoOff){
	if(self->argState.argProvided) {
	    sprintf(self->keyEcho,"%d ",self->argState.argument);
	}
	else if (self->argState.argPending) {
	    sprintf(self->keyEcho,"%d ",self->argState.tmpArgument);
	}
	else {
	    self->keyEcho[0]='\0';
	}
	self->keyEchoState=im_KeyEchoPending;
	self->keyEchoEvent=NULL;
    }

    if(self->keyEchoState!=im_KeyEchoDisplayed){
	if(self->keyEchoEvent!=NULL){
	    event_Cancel(self->keyEchoEvent);
	    self->keyEchoEvent=NULL;
	}

	if(pending)
	    self->keyEchoEvent=
	      event_Enqueue(event_Now()+
				event_MSECtoTU(KEYECHODELAY),
			    (procedure) startKeyEchoing,
			    (char *) self);
    }

    strcat(self->keyEcho,charToPrintable(key));
    if(pending)
	strcat(self->keyEcho,"-");

    if(self->keyEchoState==im_KeyEchoDisplayed)
	message_DisplayString(self,0,self->keyEcho);
}	

static void resetKeyEcho(self)
struct im *self;
{
    if(self->keyEchoState!=im_KeyEchoOff){
	if(self->keyEchoEvent!=NULL){
	    event_Cancel(self->keyEchoEvent);
	    self->keyEchoEvent=NULL;
	}
	self->keyEchoState=im_KeyEchoOff;
    }
}
static boolean stillexists(self)
	struct im *self;
{
    register struct im *im = imList;
    while (im != NULL)  {
	if(im == self) return TRUE;
	im = im->next;
    }
    return FALSE;
}

static HandleArgumentProcessing(self, key)
struct im *self;
long key;
{
    long newArg;
    char buf[30];

    if  (key == 025 && self->argState.cmdpos == 0) {
	self->argState.argDigit = FALSE;
	newArg = 4;
	if (self->argState.argPending) {
	    newArg = self->argState.tmpArgument * 4;
	    if (newArg > 65536)
		newArg = 65536;
	}
	self->argState.tmpArgument = newArg;
	self->argState.argPending = TRUE;
	self->argState.processCmd = allowCtrlUCmds;

	sprintf(buf, "Arg: %d", newArg);
	message_DisplayString(self, 0, buf);
    }
    else if (self->argState.argPending && self->argState.cmdpos == 0 && key >= '0' && key <= '9') {
	if (! self->argState.argDigit) {
	    self->argState.argDigit = TRUE;
	    self->argState.tmpArgument = 0;
	}
	newArg = self->argState.tmpArgument * 10 + (key - '0');
	if (newArg > 65536) {
	    newArg = 65536;
	}
	self->argState.tmpArgument = newArg;

	sprintf(buf, "Arg: %d", newArg);
	message_DisplayString(self, 0, buf);
    }
    else if (self->argState.argPending) {
	if (self->argState.processCmd) {
	    self->argState.argcmd[self->argState.cmdpos++] = key;
	}
	else {
	    im_ProvideArg(self, self->argState.tmpArgument);
	    SetArgProvided(self, TRUE);
	}
    }
}


	struct im * 
im__DoKey(self, key)
	struct im *self;
	long key;
{
	struct proctable_Entry *procTableEntry;
	struct basicobject *object;
	long rock = 0L;
	long dest = destroycount;

	if (self->keystate == NULL) return self;

	HandleArgumentProcessing(self, key);

	if ((! self->argState.argPending) || self->argState.processCmd) {
	    switch (keystate_ApplyKey(self->keystate, key, &procTableEntry, &rock, &object))  {
		case keystate_ProcFound:
		    if(self->keyEchoState==im_KeyEchoDisplayed){
			echoKey(self, key, FALSE);
			im_ForceUpdate();
		    }
		    message_DisplayString(self, 0, "");
		    resetKeyEcho(self);
		    self->argState.argNext = FALSE;
		    thisCmd = 0;
		    switch (keystate_DoProc(self->keystate, procTableEntry, rock, object))  {
			case keystate_NoProc:
			    message_DisplayString(self, 0, "Could not load procedure");
			    break;
			case keystate_TypeMismatch:
			    message_DisplayString(self, 0, "Bad command");
			    break;
		    }
		    if(dest == destroycount || stillexists(self))/* make sure self still exists */
		    {
			SetArgProvided(self, self->argState.argNext);
			self->lastCommand = thisCmd;
		    }
		    else {
			SetArgProvided(self, FALSE);
			return NULL;
		    }
		    self->argState.argPending = FALSE;
		    break;
		case keystate_NoBinding:
		    if (key == 'G' - 64) {
			message_DisplayString(self,0,"Cancelled.");
			SetArgProvided(self, FALSE);
		    }
		    else {
			if (self->argState.argPending) {
			    if (self->argState.cmdpos != 0) {
				im_ProvideArg(self, self->argState.tmpArgument);
				self->argState.argcmd[self->argState.cmdpos] = '\0';

				SetArgProvided(self, TRUE);
				im__DoKeySequence(self, self->argState.argcmd);
			    }
			    else {
				self->argState.processCmd = FALSE;
			    }
			}
			else {
			    echoKey(self,key,FALSE);
			    strcat(self->keyEcho, ": Unknown command");
			    message_DisplayString(self,0,self->keyEcho);

			    SetArgProvided(self, FALSE);
			    self->lastCommand = 0;
			}
		    }
		    resetKeyEcho(self);
		    break;
		case keystate_Pending:
		    echoKey(self, key, TRUE);
		    break;
	    }
	}
	return self;
}

/* used for DoMenu */
	static boolean
getMenuEntry(ml, name, pPE, pObj, pRock)
	struct menulist *ml;
	char *name;
 	struct proctable_Entry **pPE;
	struct basicobject **pObj;
   	long *pRock;
{
	struct menulist *tml;
	struct proctable_Entry *tpe;
	long trock;
	char *entryname;

	menulist_RewindBeforeMC(ml);
	while ((tml=menulist_NextBeforeMC(ml)) != NULL)
		if (getMenuEntry(tml,  name, pPE, pObj, pRock))
			return TRUE;

	menulist_RewindML(ml);
	while (menulist_NextME(ml, &entryname, &trock, &tpe)) {
		char *itemnm, *nmend;
		itemnm = index(entryname, ',');
		if (itemnm == NULL) itemnm = entryname;
		else itemnm++;
		nmend = index(itemnm, '~');
		if (nmend == NULL) nmend = itemnm + strlen(itemnm);
		if (strncmp(itemnm, name, nmend-itemnm) == 0
				|| strcmp(entryname, name) == 0) {
			*pPE = tpe;
			*pObj = ml->object;
			*pRock = trock;
			return TRUE;
		}
	}

	menulist_RewindAfterMC(ml);
	while ((tml=menulist_NextAfterMC(ml)) != NULL)
		if (getMenuEntry(tml,  name, pPE, pObj, pRock))
			return TRUE;

	return FALSE;
}


/* used for logging menu hits */
	static char *
getMenuEntryName(ml, procTableEntry, object, rock)
	struct menulist *ml;
 	struct proctable_Entry *procTableEntry;
	struct basicobject *object;
   	long rock;
{
	struct menulist *tml;
	char *entryname;
	struct proctable_Entry *tpe;
	long trock;

	entryname = NULL;

	menulist_RewindBeforeMC(ml);
	while (entryname == NULL  
			&&  (tml=menulist_NextBeforeMC(ml)) != NULL)
		entryname = getMenuEntryName(tml, procTableEntry, object, rock);

	menulist_RewindML(ml);
	if (ml->object == object)
		while (entryname == NULL 
			&& menulist_NextME(ml, &entryname, 
				&trock, &tpe)) 
			if (trock != rock  ||  tpe != procTableEntry)
				entryname = NULL;

	menulist_RewindAfterMC(ml);
	while (entryname == NULL  
			&&  (tml=menulist_NextAfterMC(ml)) != NULL)
		entryname = getMenuEntryName(tml, procTableEntry, object, rock);

	return ((entryname == NULL) ? "" : entryname);
}


struct im * 
im__HandleMenu(self, procTableEntry, object, rock)
    struct im *self;
    struct proctable_Entry *procTableEntry;
    struct basicobject *object;
    long rock;
{
    register long dest = destroycount;
    thisCmd = 0;
    self->argState.argProvided = FALSE;
    self->argState.argNext = FALSE;
    
    switch (keystate_DoProc(self->keystate, procTableEntry,rock, object))  {
	case keystate_NoProc:
	    message_DisplayString(self, 0, "Could not load procedure");
	    break;
	case keystate_TypeMismatch:
	    message_DisplayString(self, 0, "Bad command");
	    break;
    }
    if(dest == destroycount || stillexists(self)) /* make sure self still exists */
    {   
	SetArgProvided(self, self->argState.argNext);
	self->lastCommand = thisCmd;
	return self;
    }
    SetArgProvided(self, FALSE);
    return NULL;
}

/* We have a hit method here so that it can be subclassed, if necessary
  for such things as override windows */

struct view *
im__Hit (self, action, x, y, clicks)
struct im *self;
enum view_MouseAction action;
long x, y, clicks;
{
    return view_Hit(self->topLevel, action, x, y, clicks);
}

struct im *
im__HandleMouse(self, action, x, y, newButtonState)
	struct im *self;
	enum view_MouseAction action;
	long x;
	long y;
	long newButtonState;
{
	register long dest = destroycount;

	if (self->keystate) {
	    keystate_Reset(self->keystate);
	}

	if (self->topLevel != NULL && action != view_NoMouseEvent) {
		if ((self->buttonState == im_AllUp && newButtonState != im_AllUp)
				|| self->mouseFocus == NULL)  {
			register boolean closeHit;

			closeHit = (self->lastEvent == im_MouseEvent) 
					&& (self->lastMouseDown == action) 
					&& (((self->lastX - HITPIXELS) <= x) 
					&& ((self->lastX + HITPIXELS) >= x) 
					&& ((self->lastY - HITPIXELS) <= y) 
					&& ((self->lastY + HITPIXELS) >= y));
			if (closeHit)
				self->clickCount += 1;
			else if (action == view_LeftDown || action == view_RightDown)  {
				self->lastMouseDown = action;
				self->clickCount = 1;
				self->lastX = x;
				self->lastY = y;
			}
			self->buttonState = newButtonState;
			self->mouseFocus = im_Hit(self, action, x, y, self->clickCount);
		}
		else {
			struct view *hitee = self->mouseFocus;

			if (newButtonState == im_AllUp)
				self->mouseFocus = NULL;

			self->buttonState = newButtonState;
			view_Hit(hitee, action,
			 	physical_GlobalXToLogicalX(view_GetDrawable(hitee),x),
				physical_GlobalYToLogicalY(view_GetDrawable(hitee),y),
				self->clickCount);
		}
	}
	SetArgProvided(self, FALSE);
	if(dest == destroycount || stillexists(self)) /* make sure self still exists */
	{   
	    self->lastCommand = 0;
	    return self;
	}
	return NULL;
}

void im__NormalConfiguration(self, rock, customrock, parent, x, y, w, h)
struct im *self;
long rock, customrock;
struct im *parent;
long *x, *y;
unsigned long *w, *h;
{
    if(rock&im_AtTop) {
	long py=im_GetVisualTop(parent);
	long ph=im_GetVisualHeight(parent);
	*y=py+ph/10;
    } else if(rock&im_InMiddle) {
	long ph=im_GetVisualHeight(parent);
	*y=((long)ph/2)-((long)*h/2);
    } else if(rock&im_AtBottom) {
	long py=im_GetVisualBottom(parent);
	long ph=im_GetVisualHeight(parent);
	*y=py-ph/10-*h;
    }
    if(rock&im_Centered) {
	long pw=im_GetVisualWidth(parent);
	*x=((long)pw/2)-((long)*w/2);
    }
}

/* so that im's can easily override the "normal" configuration function */
static void GenericConfig(self, rock, customrock, parent, x, y, w, h)
struct im *self;
long rock, customrock;
struct im *parent;
long *x, *y;
unsigned long *w, *h;
{
    im_NormalConfiguration(self, rock, customrock, parent, x, y, w, h);
}

static procedure configfunc=(procedure)GenericConfig;

procedure im__DefaultConfigureFunction(classID, func)
struct classheader *classID;
procedure func;
{
    procedure result=configfunc;
    configfunc=func;
    return result;
}

static long configrock=0;
long im__DefaultConfigureRock(classID, rock)
struct classheader *classID;
long rock;
{
    long result=configrock;
    configrock=rock;
    return result;
}

static long configcustomrock=0;
long im__DefaultConfigureCustomRock(classID, rock)
struct classheader *classID;
long rock;
{
    long result=configcustomrock=0;
    configcustomrock=rock;
    return result;
}

boolean im__InitializeObject(classID, self)
    struct classheader *classID;
    struct im *self;
{

    struct atom * atom;
    
    self->configfunc=configfunc;
    self->configrock=configrock;
    self->configcustomrock=configcustomrock;
    
    self->topLevel = NULL;
    self->inputFocus = NULL;
    self->mouseFocus = NULL;
    self->buttonState = im_AllUp;
    self->imKeystate = keystate_Create(self, globalKeymap);
    self->keystate = self->imKeystate;
    self->menus = menulist_DuplicateML(imMenus, self);
    self->lastEvent = im_NoEvent;
    self->lastX = -1;
    self->lastY = -1;
    self->lastMouseDown = view_NoMouseEvent;
    self->clickCount = 1;
    self->next = imList;
    imList = self;
    self->header.view.imPtr = self;
    self->argState.argument = 1;
    self->argState.argProvided = FALSE;
    self->argState.argNext = FALSE;
    self->argState.argDigit = FALSE;
    self->argState.argPending = FALSE;
    self->argState.cmdpos = 0;
    self->cursorlist = NULL;
    self->WindowCursor = NULL;
    self->CursorsAreActive = 0;
    self->handlers = NULL;
    self->programName = NULL;
    self->title = NULL;
    if (globalInit != NULL)
        self->init = init_Duplicate(globalInit);
    else
	self->init = NULL;
    self->interactionEvents = NULL;
    self->pendingInteractionEvents = NULL;
    self->cursorPostsPending = FALSE;
    self->LogFile = NULL;

    atom = atom_InternRock((long) self);
    self->header.view.name = atomlist_New(); /* can't use setname here because of a class `feature' */
    atomlist_Prepend(self->header.view.name,atom);
    self->header.view.className = atomlist_StringToAtomlist("im");

    self->keyEchoState=im_KeyEchoOff;

    self->moreRecentlyUsed = self->lessRecentlyUsed = (struct im *) 0;

    return TRUE;
}

static struct im *lastUsed = NULL;

struct im *im__GetLastUsed(classID)
    struct classheader *classID;
{
    return lastUsed;
}

void im__SetLastUsed(classID, used)
    struct classheader *classID;
    struct im *used;
{
    if (lastUsed == used)
	return;
    if (used->moreRecentlyUsed)
	(used->moreRecentlyUsed)->lessRecentlyUsed =
	    used->lessRecentlyUsed;
    if (used->lessRecentlyUsed)
	(used->lessRecentlyUsed)->moreRecentlyUsed =
	    used->moreRecentlyUsed;
    used->lessRecentlyUsed = lastUsed;
    if (lastUsed)
	lastUsed->moreRecentlyUsed = used;
    used->moreRecentlyUsed = (struct im *) 0;
    lastUsed = used;
}

void im__FinalizeObject(classID, self)
    struct classheader *classID;
    struct im *self;
{
    register struct im *im = imList;
    register struct im *prevIM = NULL;
    void FreeInteractionEvents();
    
    if (self->topLevel)
        view_UnlinkTree(self->topLevel);

    while (im != NULL && im != self)  {
	prevIM = im;
	im = im->next;
    }
    if (im != NULL)  {
	if (prevIM != NULL)
	    prevIM->next = im->next;
	else
	    imList = im->next;
    }
    keystate_Destroy(self->imKeystate);
    menulist_Destroy(self->menus);
    if (self->init != NULL)
        init_Destroy(self->init);
    pruneActions(self);

    FreeInteractionEvents(self);

    if (self->moreRecentlyUsed || self->lessRecentlyUsed) {
	if (self->moreRecentlyUsed)
	    (self->moreRecentlyUsed)->lessRecentlyUsed =
		self->lessRecentlyUsed;
	if (self->lessRecentlyUsed)
	    (self->lessRecentlyUsed)->moreRecentlyUsed =
		self->moreRecentlyUsed;
    }
    if (lastUsed == self) {
	lastUsed = self->lessRecentlyUsed;
    }

    if (self->LogFile) 
	fclose(self->LogFile);
    destroycount++;

    while(self->handlers) {
	if(self->handlers->name) free(self->handlers->name);
	free(self->handlers);
	self->handlers=self->handlers->next;
    }
}

void im__WantUpdate(self, requestor)
    struct im *self;
    struct view *requestor;
{
    updatelist_AddTo(globalUpdateList, requestor);
}

void im__WantInputFocus(self, requestor)
    struct im *self;
    struct view *requestor;
{
    if (self->inputFocus != NULL)  {
	view_LoseInputFocus(self->inputFocus);
    }
    self->inputFocus = requestor;
    SetArgProvided(self, FALSE);
    self->lastCommand = 0;
    if (self->inputFocus != NULL)  {
	view_ReceiveInputFocus(self->inputFocus);
    }
}

void im__WantNewSize(self, requestor)
    struct im *self;
    struct view *requestor;
{
}

struct basicobject *im__WantHandler(self, handlerName)
    struct im *self;
    char *handlerName;
{
    struct handler *ptr;

    for (ptr = self->handlers; ptr != NULL; ptr = ptr->next)
        if (strcmp(ptr->name, handlerName) == 0)
            return ptr->handler;

    return NULL;
}

char *im__WantInformation(self, key)
    struct im *self;
    char *key;
{
    return NULL;
}

void im__PostKeyState(self, keystate)
    struct im *self;
    struct keystate *keystate;
{
    if (self->keystate != NULL)  {
	keystate_Reset(self->keystate);
    }

    self->keystate = keystate_AddAfter(self->imKeystate, keystate);

    if (self->init != NULL)
        self->keystate = init_ModifyKeystate(self->init, self->keystate);
    if (self->keystate != NULL)  {
	keystate_Reset(self->keystate);
    }

    SetArgProvided(self, FALSE);
    self->lastCommand = 0;
}

/* Menu stuff... */


void im__PostMenus(self, menulist)
    struct im *self;
    struct menulist *menulist;
{
    printf("im_PostMenus: missing method\n");
}

void im__PostDefaultHandler(self, handlerName, handler)
    struct im *self;
    char *handlerName;
    struct basicobject *handler;
{
    struct handler **ptr;

    for (ptr = &(self->handlers); *ptr != NULL; ptr = &((*ptr)->next))
        if (strcmp((*ptr)->name, handlerName) == 0)
            break;

    if (handler == NULL) {
        if (*ptr != NULL) {
            free((*ptr)->name);
            free(*ptr);
            *ptr = (*ptr)->next;
        }
    }
    else {
        struct handler *this = *ptr;

        if (this == NULL) {
            this = (struct handler *)malloc(sizeof(struct handler));
            this->next = self->handlers;
            self->handlers = this;
        }
        else
            free(this->name);

        this->name = (char *)malloc(strlen(handlerName)+1);
        strcpy(this->name, handlerName);
        this->handler = handler;
    }
}
    
void im__SetView(self, topLevel)
    struct im *self;
    struct view *topLevel;
{
    if (self->topLevel)
        view_UnlinkTree(self->topLevel);

/* In theory, all of these lines of code will be handled by the UnlinkNotification procedure,
 * Maybe later, I will remove them.
 */
    self->inputFocus = NULL;
    self->mouseFocus = NULL;
    self->buttonState = im_AllUp;
    self->keystate = self->imKeystate;
    self->lastX = -1;
    self->lastY = -1;
    self->lastMouseDown = view_NoMouseEvent;
    self->clickCount = 1;

    self->topLevel = topLevel;
    self->doRedraw = TRUE;
    self->argState.argument = 1;
    SetArgProvided(self, FALSE);
    self->argState.argNext = FALSE;
    self->argState.argDigit = FALSE;
    self->lastCommand = 0;
    if(topLevel != NULL){
	view_LinkTree(topLevel, self); /* Sets up parent and imPtr fields. */
	view_InsertView(topLevel, self, &self->header.view.drawable->localBounds);
    }
    globalDoRedraw = TRUE;
}

	boolean
im__CreateWindow(self, host)
    struct im *self;
    char *host;
{
    printf("im_CreateWindow: missing method\n");
    return FALSE;
}

void im__SetBorderWidth(self,n)
struct im *self;
long n;
{
    printf("im_SetBorderWidth: missing method\n");
}

/* these should be overridden by any ims which support transients or overrides, they need to be methods so that if you have a generic im pointer you can discover whether the specific kind of im underlying it supports transients or overrides, as opposed to the previous macros which would only actually tell you if the class used in the *im_SupportsTransient call supports transients */
boolean im__SupportsTransient(self)
struct im *self;
{
    return FALSE;
}

boolean im__SupportsOverride(self)
struct im *self;
{
    return FALSE;
}

/* It is expected that CreateTransientWindow
 will be overridden by our window-server specific 
 subclass.
 If our window server does not support Transient
 Windows we will create a top level one instead. */

	boolean
im__CreateTransientWindow(self, other)
    struct im *self, *other;
{
    return im_CreateWindow(self, NULL);
}

/* It is expected that CreateOverrideWindow
 will be overridden by our window-server specific 
 subclass.
 If our window server does not support Override
 Windows we will create a top level one instead. */

	boolean
im__CreateOverrideWindow(self, other)
    struct im *self, *other;
{
    return im_CreateWindow(self, NULL);
}

#define NORMAL_IM_CREATE 0
#define TRANSIENT_IM_CREATE 1
#define OVERRIDE_IM_CREATE 2
#define OFFSCREEN_IM_CREATE 3

static struct im *
DoCreate(classID, host, other, flag, width, height)
struct classheader *classID;
char *host;
struct im *other;
int flag;
long width, height;
{
    struct im *newIM;
    unsigned char *logdir;

    newIM = windowsystem_CreateIM(currentWS);

    switch (flag) {
	case NORMAL_IM_CREATE:
	    if (newIM == NULL  ||  ! im_CreateWindow(newIM, host))
		return NULL;
	    break;
	case TRANSIENT_IM_CREATE:
	    if (newIM == NULL  || ! im_CreateTransientWindow(newIM, other))
		return NULL;
	    break; 
	case OVERRIDE_IM_CREATE:
	    if (newIM == NULL  || ! im_CreateOverrideWindow(newIM, other))
		return NULL;
	    break;
	case OFFSCREEN_IM_CREATE:
	    if (newIM == NULL  || ! im_CreateOffscreenWindow(newIM, other, width, height))
		return NULL;
	    break;
	default: return NULL;
    }

    logdir = (unsigned char *)environ_Get("LOGDIRECTORY");
    if (logdir) {
	/* initialize logging of user actions */
	char  name[200];
	register struct tm *tm;

	LogStart = time (0) - 10000;
	tm = localtime(&LogStart);
	sprintf(name, "%s/Log.%d.%02d.%02d.%02d.%02d", logdir, getpid(),
		tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);
	newIM->LogFile = fopen(name, "w");
	WriteLogEntry(newIM, log_HOST, host);
    }
    return newIM;
}

struct im *im__Create(classID, host)
struct classheader *classID;
char *host;
{
	return (DoCreate(classID, host, NULL, NORMAL_IM_CREATE,0, 0));
}

/* Just the same as im__Create except we call im_CreateTransientWindow. */

struct im *im__CreateTransient(classID, other)
    struct classheader *classID;
    struct im *other;
{
	return (DoCreate(classID, NULL, other, TRANSIENT_IM_CREATE, 0, 0));
}

struct im *im__CreateOverride(classID, other)
    struct classheader *classID;
    struct im *other;
{
	return (DoCreate(classID, NULL, other, OVERRIDE_IM_CREATE, 0, 0));
}

struct im *im__CreateOffscreen(classID, other, width, height)
struct classheader *classID;
struct im *other;
long width, height;
{
    return (DoCreate(classID, NULL, other, OFFSCREEN_IM_CREATE, width, height));
}

/* im__WhichWS()
	returns a string for the current window system:  "X" or "wm"
	(Overriden in the subclasses)
*/
	unsigned char *
im__WhichWS(self)
	struct im *self;
{
	return (unsigned char *)"none";
}


void im__ForceUpdate(classID)
    struct classheader *classID;
{
    im_RedrawChangedWindows();
    updatelist_Clear(globalUpdateList);
    windowsystem_FlushAllWindows(currentWS);
}

void im__RedrawChangedWindows(classID)
struct classheader *classID;
{
    struct im *im;

    globalDoRedraw=FALSE;

    for(im=imList; im!=NULL; im=im->next)
	if(im->doRedraw){
	    updatelist_DeleteTree(globalUpdateList,im);

	    im->doRedraw=FALSE;
	    im_RedrawWindow(im);
	}
}

void im__RedrawWindow(self)
struct im *self;
{
}

static char workingDirectory[MAXPATHLEN];
static boolean initializeWorkingDirectory = TRUE;
#if !defined(hpux) && !defined(M_UNIX)
	extern char *getwd();
#endif /* hpux */


/* the next two routines are complements of CCH */
/*  Change to use PWD if defined instead of getcwd();
 *  This shows what the user expects to see, instead of the
 *  physical path, which maybe confusing.
 */
	static void 
set_logical_wd(dir,	newdir)
	char *dir, *newdir;
{
	if( *newdir != '/' ) {
		strcat(dir, newdir);
		/* This really needs further work to take care of "../" etc */
		/* Fortunately this leg does not seem to get used at all! */
		/* im__ChangeDirectory seems to always get full paths to change to */
		/* but I will leave this in here just in case */
	} 
	else {
		char *p;
		/* get rid of the last '/' */
		strcpy(dir, newdir);
		p = rindex(dir, '/');
		if (p != NULL && *(p+1) == '\0') *p = '\0';
	}
}

static char *get_logical_wd(dir)
	 char *dir;
{
#ifdef LOGICAL_WD_ENV
/* this code is ifdef'ed out because Zalman feels it can give incorrect results */
	char *wdir = environ_Get("PWD");
	if( wdir != NULL && *wdir != '\0') {
		strcpy(dir, wdir);
		return(dir);
	} 
	else 
#endif /* LOGICAL_WD_ENV */
#ifdef M_UNIX
		/* this may not be needed on all systems -
		 * it's to work around a problem with popen()
		 */
		(void)getwd(dir);
#endif /* M_UNIX */
		return(getwd(dir));
}


	long 
im__ChangeDirectory(classID, dirName)
	struct classheader *classID;
	char *dirName;
{
	register long code;

	if ((code = chdir(dirName)) >= 0) {
		set_logical_wd(workingDirectory, dirName);
	 	initializeWorkingDirectory = FALSE;
	}
	return code;
}

	char *
im__GetDirectory(classID, outputString)
	struct classheader *classID;
	char *outputString;
{
	boolean returnFail = FALSE;

	if (initializeWorkingDirectory)
		returnFail = (get_logical_wd(workingDirectory) == NULL);
	strcpy(outputString, workingDirectory);
	if (returnFail)
		return NULL;
	initializeWorkingDirectory = FALSE;
	return outputString;
}


void im__DeliverSignals(classID)
    struct classheader *classID;
{
    register int i;
    anyDelivered = 0;
    for(i=1;i<NSIG;i++) {
	if(sigDelivered[i]) {
	    sigDelivered[i] = 0;
	    (*sigProcs[i])(i, sigData[i]);
	}
    }
}

static int InternalSignal (asigno)
    int asigno;
{
    anyDelivered = 1;
    sigDelivered[asigno] = 1;
    PollTime.tv_sec = 0;
    PollTime.tv_usec = 0;
    return 0;
}

void im__SignalHandler(classID, signalNumber, proc, procdata)
    struct classheader *classID;
    long signalNumber;
    int (*proc)();
    char *procdata;
{
    sigProcs[signalNumber] = proc;
    sigData[signalNumber] = procdata;
#if POSIX_ENV
    {
        struct sigaction sa;
	sigset_t empty;

	sigemptyset(&empty);
	sa.sa_handler = InternalSignal;
	sa.sa_mask = empty;
	sa.sa_flags = 0;
	sigaction(signalNumber, &sa, NULL);
    }
#else
    signal(signalNumber, InternalSignal);
#endif
}


boolean im__AddFileHandler (classID, file, proc, procdata, priority)
    struct classheader *classID;
    FILE *file;
    procedure proc;
    char *procdata;
    long priority;
{
    register long i;
    register struct FILEHandlers  *p = globalFILEHandlers;

    for (i = 0; i < NFILEHandlers; i++, p++)  {
	if (p->file == file) {
	    if (p->priority != priority)  {
		im_RemoveFileHandler(file);
		break;
	    }
	    p->proc = proc;
	    p->procdata= procdata;
	    return TRUE;
        }
    }

    if (i >= NUMFILE)
	return FALSE;

    for (p = globalFILEHandlers+NFILEHandlers-1; p>=globalFILEHandlers && p->priority > priority; p--)  {
	*(p+1) = *p;
    }

    NFILEHandlers++;
    p++;
    p->file = file;
    p->proc = proc;
    p->procdata = procdata;
    p->priority = priority;
    return TRUE;
}

void im__RemoveFileHandler (classID, file)
    struct classheader *classID;
    FILE *file;
{
    register struct FILEHandlers *p = &globalFILEHandlers[NFILEHandlers];

    while (--p >= globalFILEHandlers)  {
	if (p->file == file) {
	    for (NFILEHandlers--; p < &globalFILEHandlers[NFILEHandlers]; p++)
		*p = *(p+1);
	    return;
	}
    }
}

boolean im__AddCanOutHandler (classID, file, proc, procdata, priority)
    struct classheader *classID;
    FILE *file;
    procedure proc;
    char *procdata;
    long priority;
{
    register long i;
    register struct FILEHandlers  *p = CanOutHandlers;

    for (i = 0; i < NCanOutHandlers; i++, p++)  {
	if (p->file == file) {
	    if (p->priority != priority)  {
		im_RemoveCanOutHandler(file);
		break;
	    }
	    p->proc = proc;
	    p->procdata= procdata;
	    return TRUE;
        }
    }

    if (i >= NUMFILE)
	return FALSE;

    for (p = CanOutHandlers+NCanOutHandlers-1; p>=CanOutHandlers && p->priority > priority; p--)  {
	*(p+1) = *p;
    }

    NCanOutHandlers++;
    p++;
    p->file = file;
    p->proc = proc;
    p->procdata = procdata;
    p->priority = priority;
    return TRUE;
}

void im__RemoveCanOutHandler (classID, file)
    struct classheader *classID;
    FILE *file;
{
    register struct FILEHandlers *p = &CanOutHandlers[NCanOutHandlers];

    while (--p >= CanOutHandlers)  {
	if (p->file == file) {
	    for (NCanOutHandlers--; p < &CanOutHandlers[NCanOutHandlers]; p++)
		*p = *(p+1);
	    return;
	}
    }
}

	static void
ProcessInputQueue() 
{
	struct action *a;
	struct im *self;
	
	a = GetNextInputEvent();
	if (a == NULL) return;
	self = a->im;
	im_SetLastUsed(self);
	
	switch (a->type) {
	case im_KeyboardEvent:
	    if (a->v.key == IM_METAESC)
		self = im_DoKey(a->im, '\033');
	    else
		self = im_DoKey(a->im, a->v.key);
	    break;
	case im_MenuEvent:
		self = im_HandleMenu(a->im, a->v.menu.procTableEntry, 
				a->v.menu.object, a->v.menu.rock);
		    self= NULL;
		break;
	case im_MouseEvent:
		self = im_HandleMouse(a->im, a->v.mouse.action, 
				a->v.mouse.x, a->v.mouse.y, a->v.mouse.newButtonState);
		break;
	}
	if(self)
	    self->lastEvent = a->type;
}



static void im__HandleRedraw (im)
    register struct im *im;
{
    printf("im_HandleRedraw: missing method\n");
}


#if defined(hp9000s300) && HP_OS < 70
xim_sigAlrm()
{ }
#endif /* hp9000s300 */
  
	boolean 
im__Interact(classID, mayBlock)
	struct classheader *classID;
	boolean mayBlock;
{
	if (InQ != NULL) 
		ProcessInputQueue();
	else if (globalDoRedraw) 
		im_RedrawChangedWindows();
	else if (windowsystem_HandleFiles(currentWS, 0, TRUE))  {
		/* if HandleFiles called a handler, don't do the updatelist clear,
	  	  or anything else, for that matter. */
	}
	else { /* check timer and then other possibilities */
		long when, WaitTime;

		if (eventCount > 0) {
			/* do not rely on the timeout in fselect;  recompute the time 
			to see if first event should now occur */
	   
			when = event_Now();
			WaitTime = event_FirstTime(when);
			if (WaitTime == event_ENDOFTIME)
				eventCount = 0;
		}
		else WaitTime = event_ENDOFTIME;

		if (WaitTime < event_MSECtoTU(10))  {
			event_HandleTimer(when);
			eventCount -= 1;
		}
		else if (updatelist_UpdatesPending(globalUpdateList)) 
			updatelist_Clear(globalUpdateList);
		else if (childDied) {
 			childDied = FALSE;
			if (cleanUpZombies) {
#if defined(hpux) && HP_OS < 70
				struct sigvec vecAlrm;
				struct itimerval timer;
				int pid, status;
  
				/** enable an interval timer to abort the wait(); **/
				vecAlrm.sv_handler = xim_sigAlrm;
				vecAlrm.sv_mask = 0;
				vecAlrm.sv_flags = 0;
				sigvector(SIGALRM, &vecAlrm, 0);
				timer.it_value.tv_sec = 0;
				timer.it_value.tv_usec = 100000;
				timer.it_interval.tv_sec = 0;
				timer.it_interval.tv_usec = 100000;
				setitimer(ITIMER_REAL, &timer, 0);

				/** God only knows if the status this wait returns looks
				    anything like what wait3 will return;  I hope it's good
				    enough to pass to the zombie handler; 
				 **/
				while ((pid = wait(&status)) > 0) {
					struct zombiehandler *thisHandler;
					for (	thisHandler = allZombieHandlers;
						thisHandler != NULL 
						   && thisHandler->pid != pid;
						thisHandler = thisHandler->next)
						{}
					if (thisHandler != NULL) {
						(*thisHandler->function)
							(pid,
							thisHandler->functionData, 
							&status);
						im_RemoveZombieHandler(pid);
					}
				}

				/** disable the timer **/
				timer.it_value.tv_sec = 0;
				timer.it_value.tv_usec = 0;
				setitimer(ITIMER_REAL, &timer, 0);
#else /* any system except HP_OS < 70 */
				int pid;
#ifdef hpux
#define WNOHANG 1
			    int *status;
			    while ((pid = (int) wait3 (status, WNOHANG, 0)) > 0) 
#else /* any system other than hpux */
				union wait status;
				while ((pid = wait3 (&status, WNOHANG, 0)) > 0)
#endif /* hpux */
				    {
					struct zombiehandler *thisHandler;
					for (	thisHandler = allZombieHandlers;
						thisHandler != NULL 
						    && thisHandler->pid != pid; 
						thisHandler = thisHandler->next)
						{}
					if (thisHandler != NULL) {
						(*thisHandler->function)
							(pid, 
							thisHandler->functionData,
							&status);
						im_RemoveZombieHandler(pid);
					}
				}
#endif /* defined(hpux) && HP_OS < 70 */
			} /* cleanUpZombies */
#ifdef M_UNIX
			signal(SIGCHLD, DeathInTheFamily);
#endif
		} /* childDied */

		else {
			if (mayBlock) 
				windowsystem_HandleFiles(currentWS, WaitTime, FALSE);
			else 
				/* interact with mayBlock FALSE 
				    returns whether we should call it again */
				return windowsystem_HandleFiles(currentWS, 0, FALSE);
		}

	} /* check timer and then other possibilities */

	return TRUE;
}




/* This function has largely been replaced by im_AddZombieHandler.
 * It is still here because it is very convenient for the messages system
 * model of the world and possibly other programs which were not originally
 * native to BE 2. Don't remove it without contacting the current messages
 * maintainer.
 */
void im__SetCleanUpZombies(classID, value)
    struct classheader *classID;
    boolean value;
{
#if POSIX_ENV
    {
        struct sigaction sa;
	sigset_t empty;

	sigemptyset(&empty);
        if ((cleanUpZombies = value)) /* Insure proper state of signal handler. */
            sa.sa_handler = DeathInTheFamily;
        else
	    sa.sa_handler = SIG_DFL;
	sa.sa_mask = empty;
	sa.sa_flags = 0;
	sigaction(SIGCHLD, &sa, NULL);
    }
#else
    if ((cleanUpZombies = value)) /* Insure proper state of signal handler. */
        signal(SIGCHLD, DeathInTheFamily);
    else
        signal(SIGCHLD, SIG_DFL);
#endif
}

void im__AddZombieHandler(classID, pid, function, functionData)
    struct classheader *classID;
    int pid;
    procedure function;
    long functionData; /* Actually any 32 bit crufty. */
{

    struct zombiehandler *thisHandler;

    for (thisHandler = allZombieHandlers; thisHandler != NULL && thisHandler->pid != pid; thisHandler = thisHandler->next)
        ;

    if (thisHandler != NULL) {
        thisHandler->pid = pid;
        thisHandler->function = function;
        thisHandler->functionData = functionData;
    }
    else {
        thisHandler = (struct zombiehandler *) malloc(sizeof(struct zombiehandler));
        thisHandler->pid = pid;
        thisHandler->function = function;
        thisHandler->functionData = functionData;
        thisHandler->next = allZombieHandlers;
        allZombieHandlers = thisHandler;
    }
}

void im__RemoveZombieHandler(classID, pid)
    struct classheader *classID;
    int pid;
{

    struct zombiehandler *thisHandler, **previous = &allZombieHandlers;

    for (thisHandler = allZombieHandlers; thisHandler != NULL && thisHandler->pid != pid; thisHandler = thisHandler->next)
        previous = &thisHandler->next;

    if (thisHandler != NULL) {
        *previous = thisHandler->next;
        free(thisHandler);
    }
}


void im__KeyboardExit(classID)
    struct classheader *classID;
{
    keyboardExitFlag = TRUE;
}

long im__KeyboardLevel(classID)
    struct classheader *classID;
{
    return keyboardLevel;
}

void im__KeyboardProcessor(classID)
    struct classheader *classID;
{
    keyboardLevel += 1;
    while (! keyboardExitFlag)
	im_Interact(TRUE);
    keyboardLevel -= 1;
    keyboardExitFlag = FALSE;
}


struct event *im__EnqueueEvent(classID, proc, procdata, timeIncrement)
    struct classheader *classID;
    int (*proc) ();
    char *procdata;
    long timeIncrement;
{
    struct event *event;

    event = event_Enqueue(event_Now() + timeIncrement, proc, procdata);
    eventCount += 1;
    return event;
}

	boolean
im__IsPlaying(ClassID)
	struct classheader *ClassID;
{
	return playingRecord;
}


static void InteractionEventWork(interactionEvent)
    struct im_InteractionEvent *interactionEvent;
{

    struct im *im = interactionEvent->im;
    struct im_InteractionEvent *event;
    struct im_InteractionEvent **previous = &im->interactionEvents;

    for (event = im->interactionEvents; event != NULL && event != interactionEvent; event = event->next)
        previous = &event->next;

    if (event == NULL)
        return; /* If this happens, some invariant has been violated. */
    
    *previous = event->next;
    event->next = im->pendingInteractionEvents;
    im->pendingInteractionEvents = event;
}

struct event *im__SetInteractionEvent(self, interactionFunction, interactionData, timeIncrement)
    struct im *self;
    procedure interactionFunction;
    long interactionData;
    long timeIncrement;
{

    struct im_InteractionEvent *newEvent;

    newEvent = (struct im_InteractionEvent *) malloc(sizeof(struct im_InteractionEvent));
    if (newEvent == NULL)
        return NULL;
    newEvent->function = interactionFunction;
    newEvent->data = interactionData;
    newEvent->im = self;
    newEvent->event = im_EnqueueEvent((int (*)()) InteractionEventWork, (char *) newEvent, timeIncrement);
    newEvent->next = self->interactionEvents;
    self->interactionEvents = newEvent;
    return newEvent->event;
}

void im__CancelInteractionEvent(self, event)
    struct im *self;
    struct event *event;
{

    struct im_InteractionEvent **previous = &self->interactionEvents;
    struct im_InteractionEvent *interactionEvent;

    for (interactionEvent = self->interactionEvents; interactionEvent != NULL && interactionEvent->event != event; interactionEvent = interactionEvent->next)
        previous = &interactionEvent->next;

    if (interactionEvent == NULL) {
        previous = &self->pendingInteractionEvents;
        for (interactionEvent = self->pendingInteractionEvents; interactionEvent != NULL && interactionEvent->event != event; interactionEvent = interactionEvent->next)
            previous = &interactionEvent->next;
    }

    if (interactionEvent != NULL) {
        *previous = interactionEvent->next;
        event_Cancel(event);
        free(interactionEvent);
    }
}

static void FreeInteractionEvents(self)
    struct im *self;
{

    struct im_InteractionEvent *interactionEvent;
    struct im_InteractionEvent *next;

    for (interactionEvent = self->interactionEvents;
         interactionEvent != NULL;
         interactionEvent = next) {
        event_Cancel(interactionEvent->event);
        next = interactionEvent->next;
        free(interactionEvent);
    }
    self->interactionEvents = NULL;

    for (interactionEvent = self->pendingInteractionEvents;
         interactionEvent != NULL;
         interactionEvent = next) {
        next = interactionEvent->next;
        free(interactionEvent);
    }
    self->pendingInteractionEvents = NULL;
}

void im__DispatchPendingInteractionEvents(self)
    struct im *self;
{

    struct im_InteractionEvent *interactionEvent;
    struct im_InteractionEvent *nextInteractionEvent;

    for (interactionEvent = self->pendingInteractionEvents;
         interactionEvent != NULL; interactionEvent = nextInteractionEvent) {
        (*interactionEvent->function)(interactionEvent->data);
        nextInteractionEvent = interactionEvent->next;
        free(interactionEvent);
    }
    self->pendingInteractionEvents = NULL;
}

static void RedrawWindow(self, key)
    struct im *self;
    long key;
{
    self->doRedraw = TRUE;
    globalDoRedraw = TRUE;
}

static void StartKeyboardMacro(self, key)
    struct im *self;
    long key;
{
    if (playingRecord) 
	return;
    if (doRecord)  {
	message_DisplayString(self, 0, "Already recording events");
	return;
    }
    doRecord = TRUE;
    freeQlist(Record);
    Record = NULL;
    message_DisplayString(self, 0, "Recording on");
}


static void StopKeyboardMacro(self, key)
    struct im *self;
    long key;
{
    if (playingRecord) 
	return;
    if (doRecord)  {
	doRecord = FALSE;
	message_DisplayString(self, 0, "Recording off");
    }
    else
	message_DisplayString(self, 0, "You weren't recording events");
}

static void PlayKeyboardMacro(self, key)
    struct im *self;
    long key;
{
    register long count;
    
    count = im_Argument(self);
    im_ClearArg(self);
    if (doRecord)  {
	message_DisplayString(self, 0, "Can not execute event macro while recording events");
    }
    else if (playingRecord)  {
	message_DisplayString(self, 0, "Recursive call to playing event macro");
    }
    else if (Record == NULL)  {
	message_DisplayString(self, 0, "You haven't defined a keyboard macro yet");
    }
    else  {
	playingRecord = TRUE;
	enqAction(&InQ, macroAction(self, Record, Record, count));
	if (count > 0)
		message_DisplayString(self, 0, "Playing macro.  Control-G to quit early.");
    }
}

	void
im__CancelMacro(classID)
	struct classheader *classID;
{
	/* if PendingRelease is not NULL, the macro has completed */
	if ( ! playingRecord || PendingRelease != NULL) return;

	/* remove macro from front of InQ */
	PendingRelease = InQ;
	InQ = InQ->next;

	playingRecord = FALSE;
}

/* Code for handling key events for im object (Used to be in keystate).
This section deals with the global command argument, usually set by the ^U command.
 */

static SetArgProvided(self, value)
struct im *self;
boolean value;
{
    if (self->argState.argProvided != value) {
	keystate_Reset(self->keystate);
	self->argState.argProvided = value;
    }
    self->argState.argPending = FALSE;
    self->argState.cmdpos = 0;
}

struct im_ArgState *im__GetArgState(self)
    struct im *self;
{
    return &(self->argState);
}

void im__ClearArg(self)
    struct im *self;
{
    self->argState.argument = 1;
}

boolean im__ArgProvided(self)
    struct im *self;
{
    return self->argState.argProvided;
}

long im__Argument(self)
    struct im *self;
{
    if (self->argState.argProvided)
	return self->argState.argument;
    else
	return 1;
}

void im__ProvideArg(self, arg)
    struct im *self;
    long arg;
{
    self->argState.argNext = TRUE;
    self->argState.argument = arg;
}

void im__DisplayArg(self)
struct im *self;
{
    char buf[30];

    struct im_ArgState *as = im_GetArgState(self);

    sprintf(buf, "Arg: %d", as->argument);
    message_DisplayString(self, 0, buf);
}

long im__BumpArg(self, val)
struct im *self;
long val;
{
    struct im_ArgState *as = im_GetArgState(self);
    long newArg;

    if (! as->argDigit) {
	as->argDigit = TRUE;
	as->argument = 0;
    }
    newArg = as->argument * 10 + val;
    if (newArg > 65536)
	newArg = 65536;
    im_ProvideArg(self, newArg);
    return newArg;
}
    
/* These routines deal with the last command variable. */

static long nextCmdValue = 1;	/* next value to alloc */

long im__AllocLastCmd(classID)
    struct classheader *classID;
{
    return nextCmdValue++;
}

long im__GetLastCmd(self)
    struct im *self;
{
    return self->lastCommand;
}

void im__SetLastCmd(self, cmd)
    struct im *self;
    long cmd;
{
    thisCmd = cmd;
}

	void 
im__DoMacro(self)
	struct im *self;
{
	if (doRecord)  {
		message_DisplayString(self, 0, 
			"Cannot execute event macro while recording events");
	}
	else if (playingRecord)  {
		message_DisplayString(self, 0, "Recursive call to playing event macro");
	}
	else  {
		playingRecord = TRUE;
		enqAction(&InQ, macroAction(self, Record, Record, 1));
		while (playingRecord) 
			im_Interact(FALSE);
	}
}

/* im__DoMenu(self, itemname)
	executes the function associated with the menuitem
	the itemname may be just the element name or the name preceded
		by the cardname and a comma.
	comparisons are case insensitive
*/
	void
im__DoMenu(self, itemname)
	struct im *self;
	unsigned char *itemname;
{
 	struct proctable_Entry *pe;
	struct basicobject *obj;
   	long rock; 
	if (getMenuEntry(self->menus, itemname, &pe, &obj, &rock))
		im_HandleMenu(self, pe, obj, rock);
}

/* im__CheckForInterrupt()
	scans ahead in user input looking for control-G and queuing the inputs
	if finds control-G, discards the input queue and returns TRUE
	otherwise returns FALSE
*/
	boolean
im__CheckForInterrupt(classID)
	struct classheader *classID;
{
	struct action *tQ, *tx;
	struct im *im;

	/* go get pending events from the windowsystem 
		actually do the select to get new input */
	windowsystem_HandleFiles(currentWS, 0, FALSE);

	/* Check queue for control-G */
	for (tQ = InQ;  tQ != NULL;  tQ = tQ->next) 
		if (tQ->type == im_KeyboardEvent && tQ->v.key == '\007') {
			/* BINGO! discard everything 
					up to and including the control-G */
			tx = InQ;
			InQ = tQ->next;	/* retain stuff after the ^G */
			tQ->next = NULL;

			/* XXX need to traverse the discarded stuff to update mouse state */

			freeQlist(tx);
			playingRecord = FALSE;  /* if it was TRUE, it isn't now */
			for (im = imList; im != NULL; im = im->next)
				/* discard any partial keystates */
				if (im->keystate != NULL)  
					keystate_Reset(im->keystate);
			return TRUE;
		}
	return FALSE;
}
	
boolean im__WasMeta(self)
struct im *self;
{
    return (self->WasMeta);
}

	int 
im__GetCharacter(self)
	struct im *self;
{
	struct action * a;
	struct view *curview = self->topLevel;
	struct view *curfocus = self->inputFocus;
	while (TRUE) {
		if (InQ != NULL) {
			a = PeekInputEvent();
			if (a->type == im_KeyboardEvent && a->im == self) {
				a = GetNextInputEvent();
				if (a->v.key == IM_METAESC) {
				    self->WasMeta = TRUE;
				    return '\033';
				}
				else {
				    self->WasMeta = FALSE;
				    return a->v.key;
				}
			}
			else 
				return EOF;
		}

		/* in the following call, InQ is NULL, so im_Interact will NOT
		 *	call ProcessInputQueue()
		*/
		im_Interact(TRUE);

		if (keyboardExitFlag  ||  curview != self->topLevel  
				|| curfocus != self->inputFocus)
			return EOF;
	}
}


static long WriteID = 1;

long im__GetWriteID(classID)
    struct classheader *classID;
{
    return WriteID++;
}

void static PrintMallocStats(self, c)
    struct im *self;
    int c;
{
#if defined(ANDREW_MALLOC_ENV) && defined(DEBUG_MALLOC_ENV)
    FILE *outFile;
    char filename[500];
    char outstring[1000];
    
    sprintf(filename, "/tmp/mallocstats.%d", getpid());
    if ((outFile = fopen(filename, "a")) != NULL)  {
	MallocStats("be2 stats", outFile);
	fclose(outFile);
	sprintf(outstring, "Malloc statistics appended to %s", filename);
	message_DisplayString(self, 0, outstring);
    }
    else 
#endif /* #if defined(ANDREW_MALLOC_ENV) && defined(DEBUG_MALLOC_ENV) */
	message_DisplayString(self, 0, "could not write out malloc statistics");
}

void ResetMallocStats(self, c)
    struct im *self;
    long c;
{
#if defined(ANDREW_MALLOC_ENV) && defined(DEBUG_MALLOC_ENV)
    resetmstats();
    message_DisplayString(self, 0, "Reset malloc statistics");
#endif /* #if defined(ANDREW_MALLOC_ENV) && defined(DEBUG_MALLOC_ENV) */
}

void PrintMallocTable(self, c)
    struct im *self;
    long c;
{
#if defined(ANDREW_MALLOC_ENV) && defined(DEBUG_MALLOC_ENV)
    FILE *outFile;
    char filename[500];
    char outstring[1000];
    static long count = 0;
    
    sprintf(filename, "/tmp/malloctable.%d.%d", getpid(), ++count);
    if ((outFile = fopen(filename, "w")) != NULL)  {
	plumber(outFile);
	fclose(outFile);
	sprintf(outstring, "Malloc table appended to %s", filename);
	message_DisplayString(self, 0, outstring);
    }
    else 
#endif /* #if defined(ANDREW_MALLOC_ENV) && defined(DEBUG_MALLOC_ENV) */
	message_DisplayString(self, 0, "Could not write out malloc table");
}

static void StartProfiling(self,c)
struct im *self;
long c;
{
    int success;

    if(profile_Active()){
	message_DisplayString(self,0,"Already profiling.");
	return;
   }

    if(im_ArgProvided(self)){
	char class[200];
	message_AskForString(self,0,"Class to profile: ",NULL,class,sizeof(class));
	success=profile_StartClass(class,"mon.out");
    }else
	success=profile_Start(NULL,0,"mon.out");

    if(success)
	message_DisplayString(self, 0,"Started profiling...");
    else
	message_DisplayString(self,0,"Couldn't start profiling!");
}

static void StopProfiling(self, c)
    struct im *self;
    long c;
{
    if(!profile_Active()){
	message_DisplayString(self,0,"Not profiling.");
	return;
    }

    if(profile_Stop())
	message_DisplayString(self,0,"Stopped; profile in \"mon.out\".");
    else
	message_DisplayString(self,0,"Error stopping profiling!");
}

static struct bind_Description imBindings[]={
	{"im-exit-program", "\030\003", 0, "Quit~99", 0, 0, im__KeyboardExit, 
			"Exit program."},
	{"im-start-keyboard-macro", "\030(", 0, NULL, 0, 0, StartKeyboardMacro , 
			"Start recording input events."},
	{"im-stop-keyboard-macro", "\030)", 0, NULL, 0, 0, StopKeyboardMacro, 
			"Stop recording input events."},
	{"im-play-keyboard-macro", "\030e", 0, NULL, 0, 0, PlayKeyboardMacro, 
			"Play current keyboard macro."},
	{"im-redraw-window", "\014", 0, NULL, 0, 0, RedrawWindow, 
			"Redraw the window."},

	{"exit", 0, 0, 0, 0, 0, im__KeyboardExit, "Obsolete"},
	{"start-keyboard-macro", 0, 0, 0, 0, 0, StartKeyboardMacro, "Obsolete"},
	{"stop-keyboard-macro", 0, 0, 0, 0, 0, StopKeyboardMacro, "Obsolete"},
	{"play-keyboard-macro", 0, 0, 0, 0, 0, PlayKeyboardMacro, "Obsolete"},
	{"redraw-window", 0, 0, 0, 0, 0, RedrawWindow, "Obsolete"},

	{"im-print-malloc-statistics", "\030\200m", 0, NULL, 0, 0, PrintMallocStats,
			"Print malloc statisticsp to a file."},
	{"im-reset-malloc-statistics", "\030\200r", 0, NULL, 0, 0, ResetMallocStats,
			"Reset malloc statistics."},
	{"im-print-malloc-table", "\030\200t", 0, NULL, 0, 0, PrintMallocTable,
			"Print malloc table to a file."},
	{"im-start-profile", "\030\200p", 0, NULL, 0, 0, StartProfiling,
			"Start profiling program."},
	{"im-stop-profile", "\030\200q", 0, NULL, 0, 0, StopProfiling,
			"Stop profiling program."},
	NULL
};

/* This array is magic in that its first entry is the default. */
static struct wsinfo {
    char *keyName;
    char *windowSystemName;
} knownWindowSystems[] = {
#ifdef WM_ENV
    {"andrewwm", "wmws"},
#endif /* WM_ENV */
    {"x11", "xws"}
};

boolean im__InitializeClass(classID)
    struct classheader *classID;
{

    char *envString;
    boolean NotFound = TRUE;
    struct wsinfo *windowsys;
    char *wsName;

    InitGlobalStructure();
    childDied = FALSE;
    cleanUpZombies = TRUE;
    doRecord = FALSE;
    playingRecord = FALSE;
    eventCount = 0;
    thisCmd = 0;
    keyboardExitFlag = FALSE;
    keyboardLevel = 0;
    destroycount = 0;
    allZombieHandlers = NULL;
    allowCtrlUCmds = environ_GetProfileSwitch("CtrlUCmds", TRUE); 

#if POSIX_ENV
    {
	struct sigaction sa;
	sigset_t empty;

	sigemptyset(&empty);
	sa.sa_handler = DeathInTheFamily;
	sa.sa_mask = empty;
	sa.sa_flags = 0;
	sigaction(SIGCHLD, &sa, NULL);
    }
#else
    signal(SIGCHLD, DeathInTheFamily);
#endif

    if ((envString = environ_Get("BE2WM")) != NULL) {
	for (windowsys = knownWindowSystems; (windowsys < knownWindowSystems + (sizeof(knownWindowSystems)/sizeof(knownWindowSystems[0]))) && (NotFound = (strcmp(envString, windowsys->keyName) != 0)); windowsys++);
	if (NotFound) {
	    windowsys = knownWindowSystems;
	    if (envString != NULL)
		printf("im_InitializeClass: Unknown window manager %s; assuming %s\n", envString, windowsys->keyName);
	}
	wsName = windowsys->windowSystemName;
    }
#ifdef WM_ENV
    else if ((envString = environ_Get("WMHOST")) != NULL) 
	wsName = "wmws";
#endif /* WM_ENV */
#ifdef X11_ENV
    else if ((envString = environ_Get("DISPLAY")) != NULL) 
	wsName = "xws";
#endif /* X11_ENV */
    else 
        wsName = knownWindowSystems[0].windowSystemName;

    currentWS = (struct windowsystem *) class_NewObject(wsName);


    A_application = atom_Intern("application");
    ProgramNameAtom = A_application;

    globalUpdateList = updatelist_New();

    globalKeymap = keymap_New();

    imMenus = menulist_New();
    
    bind_BindList(imBindings, globalKeymap, imMenus, &im_classinfo);

    return TRUE;
}

/*     The following supply cursor support.   */
/*
 NOTE The ClearCursors() Macro does different things in WM and X

In WM it delete the subrectangles associated with a cursor, but they will be rebuilt in im_UpdateCursor if the item remains in the im's cursorlist. Routines that remove cursors have to call this macro on any cursor that may be affected by the removal of a cursor that had been placed over it.

Under X it unmaps the inputonly window associated the cursor, since X handles these windows properly, only cursors that are really being removed are cleared.

*/

void im__ClearCursors(self,C)
struct im * self;
struct cursor * C; {
    printf("im_ClearCursors: missing method\n");
}

void im__PostCursor(self,rec,cursor)
struct im *self;
struct rectangle *rec;
struct cursor *cursor;
{
    printf("im_PostCursor: missing method\n");

}

void im__RetractCursor(self,cursor)
struct im *self;
register struct cursor *cursor;
{
    register struct cursor *cp,*lastcp;
    if(cursor == NULL || cursor->posted == NULL) return;
    for(cp= self->cursorlist,lastcp = NULL; cp != NULL;cp = cp->next){
	if(cp == cursor){
	    cp->posted = NULL;
	    im_ClearCursors(self, cp);
	    if(lastcp)
		lastcp->next = cp->next;
	    else self->cursorlist= cp->next;
	}
	else lastcp = cp;
    }
    self->cursorPostsPending = TRUE;
    if(ProcessCursor == NULL && self->WindowCursor == NULL && self->inRedraw == FALSE)
	im_UpdateCursors(self);
}

void im__RetractViewCursors(self,requestor)
struct im *self;
struct view *requestor;
{
    /* clears cursors belonging to a view */
    register struct cursor *cp,*lastcp;
    register int found = 0;
    for(cp= self->cursorlist,lastcp = NULL; cp != NULL;cp = cp->next){
	if(cp->view == requestor) {
	    found++;
	    cp->posted = NULL;
	    im_ClearCursors(self, cp);
	    if(lastcp)
		lastcp->next = cp->next;
	    else self->cursorlist= cp->next;
	}
	else  {
	    lastcp = cp;
	}
    }
    self->cursorPostsPending = TRUE;
    if(found && ProcessCursor == NULL && self->WindowCursor == NULL && self->inRedraw == FALSE)
	im_UpdateCursors(self);
}


void im__SetProcessCursor(classID, cursor) /* set cursor to NULL to deactivate */
    struct classheader *classID;
struct cursor *cursor;
{
    register struct im *im;
    if(ProcessCursor == cursor){
	if(cursor == NULL || !cursor->changed)  return;
	cursor->changed = FALSE;
    }
    else{
	if(ProcessCursor)ProcessCursor->processC = FALSE;
	if(cursor) cursor->processC = TRUE;
        ProcessCursor = cursor;
    }
    for (im = imList; im != NULL ; im = im->next)
        im_UpdateCursors(im);
}

struct cursor *im__GetProcessCursor(classID)
    struct classheader *classID;
{
	return ProcessCursor;
}

void im__SetWindowCursor(self,cursor) /* set cursor to NULL to deactivate */
struct im *self;
struct cursor *cursor;
{
    if(self->WindowCursor == cursor){
	if(cursor == NULL || !cursor->changed)  return;
	cursor->changed = FALSE;
    }
    else{
	if(self->WindowCursor)self->WindowCursor->windowim = NULL;
	if(cursor) cursor->windowim = self;
        self->WindowCursor = cursor;
    }
    im_UpdateCursors(self);
}

void im__ClearCursorList(self)
struct im *self;
    {
    register struct cursor *cp;

    if (im_IsPlaying()) return;

    for(cp= self->cursorlist; cp != NULL;cp = cp->next){
        cp->posted = NULL;
	im_ClearCursors(self, cp);
        }
    self->cursorlist = NULL;
    self->cursorPostsPending = TRUE;
    }

void im__UpdateCursors(self)
struct im *self;
{
    printf("im_UpdateCursors: missing method\n");
}




void im__SetTitle(self, title)
    struct im *self;
    char *title;
{
    if (self->title != NULL)  {
	free(self->title);
	self->title = NULL;
    }
    if (title != NULL)  {
	self->title = (char *) malloc(strlen(title) + 1);
	strcpy(self->title, title);
    }
    if (self->LogFile != NULL) WriteLogEntry(self, log_TITLE, title);
}

char *im__GetTitle(self)
    struct im *self;
{
    return self->title;
}

void im__SetProgramName(classID, name)
    struct classheader *classID;
    char *name;
{
    unsigned char *shudder;
    if (initialProgramName != NULL)  {
	free(initialProgramName);
	initialProgramName = NULL;
    }
    if (name != NULL)  {
	initialProgramName = (char *) malloc(strlen(name) + 1);
	strcpy(initialProgramName, name);
	/* Used for reading the preferences file */
	environ_SetProgramName(name);

	/* the following 3 lines are gross and awful because app.c also checks this
		preference option and because this may override a SetGeometrySpec
		deliberatly set by the program.  */
	shudder = (unsigned char *)environ_GetProfile("Geometry");
	if (shudder != NULL)
	    im_SetGeometrySpec(shudder);
    }

    ProgramNameAtom = atom_Intern(initialProgramName);
}

char *im__GetProgramName(classID)
    struct classheader *classID;
{
    return initialProgramName;
}

void im__SetGlobalInit(classID, init)
    struct classheader *classID;
    struct init *init;
{

    globalInit = init;
}

struct init *im__GetGlobalInit(classID)
    struct classheader *classID;
{

    return globalInit;
}

/* We really ought to tell the window manager about this change in preferences.
    This should be a method instead of a class procedure. */
void im__SetPreferedDimensions(classID, top, left, width, height)
    struct classheader *classID;
    long top, left, width, height;
{

    preferedTop = top;
    preferedLeft = left;
    preferedWidth = width;
    preferedHeight = height;
    setDimensions = TRUE;
}

void im__GetPreferedDimensions(classID, top, left, width, height)
    struct classheader *classID;
    long *top, *left, *width, *height;
{

    *top = preferedTop;
    *left = preferedLeft;
    *width = preferedWidth;
    *height = preferedHeight;
}

/* We really ought to tell the window manager about this change in preferences.
    This should be a method instead of a class procedure. */
	void 
im__SetGeometrySpec(classID, value)
	struct classheader *classID;
	char *value;
{
	char *buffer;
	buffer = malloc(strlen(value) + 2);
	if (*value != '=') {
		*buffer = '=';
		strcpy(buffer+1, value);
	}
	else
		strcpy(buffer, value);
	if (geometrySpec != NULL)  
		free(geometrySpec);
	geometrySpec = buffer;
}


FILE *im__FromCutBuffer(self)
    struct im *self;
{
    printf("im_FromCutBuffer: missing method\n");
    return NULL;
}

FILE *im__ToCutBuffer(self)
    struct im *self;
{
    FILE *cutFile;

    writeCutBuffer.pos = 0;
    writeCutBuffer.length = 0;
    cutFile = (FILE *) im_vfileopen("w",0);
    return cutFile;
}

void im__CloseFromCutBuffer(self, readFile)
    struct im *self;
    FILE *readFile;
{
    im_vfileclose(readFile, 0);
}

void im__CloseToCutBuffer(self, writeFile)
    struct im *self;
    FILE *writeFile;
{
    printf("im_CloseToCutBuffer: missing method\n");

}

void im__RotateCutBuffers(self, count)
    struct im *self;
    long count;
{
    printf("im_RotateCutBuffers: missing method\n");

}

void im__AppendToCutBuffer(self, writeFile)
    struct im *self;
    FILE *writeFile;
{
    printf("im_AppendToCutBuffer: missing method\n");

}

void im__SetWMFocus(self)
    struct im *self;
{
    printf("im_SetWMFocus: missing method\n");

}

void im__ExposeWindow(self)
    struct im *self;
{
    printf("im_ExposeWindow: missing method\n");
}

	void
im__HideWindow(self)
	struct im *self;
{
	printf("im_HideWindow: missing method\n");
}

	void
im__VanishWindow(self)
	struct im *self;
{
	printf("im_VanishWindow: missing method\n"); 
}

struct windowsystem *im__GetWindowSystem(classID)
    struct classheader *classID;
{

    return currentWS;
}

struct cursor * im__GetCursor(classID)
    struct classheader *classID;
{
    return windowsystem_CreateCursor(im_GetWindowSystem());
}

struct fontdesc * im__GetFontdesc(classID)
    struct classheader *classID;
{
    return windowsystem_CreateFontdesc(im_GetWindowSystem());
}

struct graphic * im__GetGraphic(classID)
    struct classheader *classID;
{
    return windowsystem_CreateGraphic(im_GetWindowSystem());
}

short im__GetResource( self, name, class, type, data )
     struct im * self;
     struct atomlist * name;
     struct atomlist * class;
     struct atom * type;
     long * data;
{
  struct atoms * nameMark = atomlist_Mark(name);
  struct atoms * classMark = atomlist_Mark(name);
  short found;

  atomlist_JoinToBeginning( name, self->header.view.name );
  atomlist_JoinToBeginning( class, self->header.view.className );
  atomlist_Prepend( name, ProgramNameAtom );
  atomlist_Prepend( class, A_application );
  found = rm_GetResource( name, class, type, data );
  atomlist_Cut( name, nameMark );
  atomlist_Cut( class, classMark );
  return found;
}


void im__PostResource( self, path, type, data )
     struct im * self;
     struct atomlist * path;
     struct atom * type;
     long data;
{
  struct atoms * pathMark = atomlist_Mark(path);

  atomlist_JoinToBeginning( path, self->header.view.name );
  atomlist_Prepend( path, ProgramNameAtom );
  rm_PostResource( path, data, type );
  atomlist_Cut( path, pathMark );
}


void im__GetManyParameters(self, resources, name, class)
     struct im * self;
     struct resourceList * resources;
     struct atomlist * name;
     struct atomlist * class;
{
  struct atoms * nameMark = NULL;
  struct atoms * classMark = NULL;
  struct atomlist * passname;
  struct atomlist * passclass;

  if (name == NULL)
    passname = self->header.view.name;
  else
    {
      nameMark = atomlist_Mark(name);
      atomlist_JoinToBeginning(name,self->header.view.name);
      passname = name;
    }

  if (class == NULL)
    passclass = self->header.view.className;
  else
    {
      classMark = atomlist_Mark(class);
      atomlist_JoinToBeginning(class,self->header.view.className);
      passclass = class;
    }

  atomlist_Prepend( passname, ProgramNameAtom );
  atomlist_Prepend( passclass, A_application );
  rm_GetManyResources( resources, passname, passclass );
  atomlist_DropFirst(passname);
  atomlist_DropFirst(passclass);
  
  if (name != NULL)
    atomlist_Cut(name,nameMark);
  if (class != NULL)
    atomlist_Cut(class,classMark);
}

void im__UnlinkNotification(self, unlinkedTree)
    struct im *self;
    struct view *unlinkedTree;
{

    struct cursor *thisCursor;

/* Input focus. */
    if (self->inputFocus != NULL && view_IsAncestor(self->inputFocus, unlinkedTree)) {
        view_LoseInputFocus(self->inputFocus);
        self->inputFocus = NULL;
        im_PostKeyState(self, NULL);
        im_PostMenus(self, NULL);
    }

/* Mouse focus */
    if (self->mouseFocus != NULL && view_IsAncestor(self->mouseFocus, unlinkedTree)) {
        self->mouseFocus = NULL;
        self->buttonState = im_AllUp;
        self->lastX = -1;
        self->lastY = -1;
        self->lastMouseDown = view_NoMouseEvent;
        self->clickCount = 1;
    }

 /* Cursors */
    for (thisCursor = self->cursorlist; thisCursor != NULL; thisCursor = thisCursor->next)
        if (view_IsAncestor(thisCursor->view, unlinkedTree))
            im_RetractCursor(self, thisCursor);

/* Pending updates. */
    updatelist_DeleteTree(globalUpdateList, unlinkedTree);

    super_UnlinkNotification(self, unlinkedTree);
}

     boolean
im__CreateOffscreenWindow(self, other, width, height)
struct im *self, *other;
long width, height;
{
    printf("im_CreateOffscreenWindow: missing method\n");
    return FALSE;
}

boolean im__SupportsOffscreen(self)
struct im *self;
{
    return FALSE;
}

struct rectangle *im__GetLoc(self, view, rect)
struct im *self;
struct view *view;
struct rectangle *rect;
{
    printf("im_GetLoc: missing method\n");
    return NULL;
}
