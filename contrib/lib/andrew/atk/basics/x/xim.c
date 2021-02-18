/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/usr20/rr2b/work/atk/basics/x/RCS/xim.c,v 1.67 1991/06/30 01:
  35:08 rr2b Exp $ */
/* $ACIS:im.c 1.6$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xim.c,v $ */

#ifndef lint
	char *xim_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xim.c,v 1.90 1991/09/30 18:48:30 rr2b Exp $";
#endif /* lint */

/* Put in error messages for handling of keystrokes.
Figure out how to handle handlers and information requests.
Figure out some way to handle levels of user.  Macros should probably not be an novice level facility. */

#include <andrewos.h> /* sys/time.h sys/types.h sys/file.h */
#include <signal.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#if defined(PRE_X11R4_ENV)
typedef int (*XErrorHandler) ();
#endif /* defined(PRE_X11R4_ENV) */

#include <cmenu.h>
#include "menubar.h"


#include <point.h>
#include <rect.h>
#define INTERACTION_MANAGER
#include <im.ih>
#undef INTERACTION_MANAGER
#include <xim.eh>
#include <graphic.ih>
#include <xgraphic.ih>
#include <xfontd.ih>
#include <observe.ih>
#include <view.ih>
#include <event.ih>
#include <keymap.ih>
#include <keystate.ih>
#include <proctbl.ih>
#include <menulist.ih>
#include <updlist.ih>
#include <environ.ih>
#include <cursor.ih>
#include <xcursor.ih>
#include <physical.h>
#include <message.ih>
#include <init.ih>
#include <bind.ih>
#include <filetype.ih>

#include <errprntf.h>

/* Note this constant also occurs in the cmenus package and in menubar.c */
#define MAXPANEPRIORITY 100

#define STANDARDEVENTMASK \
	ExposureMask 		\
	| KeyPressMask		\
	| ButtonPressMask		\
	| ButtonReleaseMask		\
	| ButtonMotionMask		\
	| StructureNotifyMask	    \
	| PropertyChangeMask /* \
        | SubstructureNotifyMask */
/* (sigh.  really don't want PropertyChangeMask most of the time.
	But I don't think it hurts much.) */
/* Substructure notify and PropertyChangeNotify are needed to cache location also */

static struct im_GlobalDataType * gData = NULL;

static Time lastEventTime = CurrentTime;	/* set as each input event is received */



static boolean SelectionAndCB = TRUE;  /* T iff should grab PRIMARY and CLIPBOARD
				when a cut/copy occurs */
static boolean IOwnSelection = FALSE;
static Time SelectionStartTime = CurrentTime;

static updateGlobalCursors();

/* the four states below indicate variety of elements on the PropList.
	For each there is a window of interest and some atom value
	which will be checked against toe see if the event is desired. */
enum PropState {
	prop_SelectTarget,
		/* We are awaiting a reply to an XConvertSelection asking for TARGETS
			the atom in the proplistelt is PRIMARY or CLIPBOARD,
			the window is self */
	prop_Incoming,
		/* we are awaiting the first or only portion of the selection
			the window is self,
			the atom is PRIMARY or CLIPBOARD */
	prop_InIncr,
		/* The selection value is arriving in increments.  The window is self.
			The atom is the property under which the value will arrive.
			The string field is an expandstring to which the 
			value is to be appended. */
	prop_OutIncr
		/* The selection is being sent elsewhere via the INCR protocol.
			the window is the desination and
			the atom is the property where it is being stored
			The string field is a cutbuffer whose 'pos' field
			says what is to be sent next.  */
};

static struct proplistelt {
	Window window;
	Atom atom;
	enum PropState state;
	Time when;
	struct expandstring *string;
	boolean *xfree;
	Atom type;
	struct proplistelt *next;
} *PropList = NULL;

static struct proplistelt *FreeProps = NULL;

#define MENUVIEWHASH(x) ((((long)(x))>>3)%MENUVIEWCACHESIZE)

/* A copy of the value in CUT_BUFFER0 is cached here and used instead of monkeying
	around with vfiles. */
static struct expandstring CachedCutBuffer;
static boolean CBCacheValid = FALSE;

static void HandleSelectionNotify();
static void HandleProperty();
static void RespondToSelectionRequest();
static void Unscribe();
static void retrieveFromCutBuffer0();
static void LocateWindow();


/* the point of the MaskList is to keep track of whether we want PropertyNotify events 
	from other windows.  We remove an element from the list and
	reset its mask when the count becomes zero.
  Subtle problem: if ever one of the windows for this current process gets on the list, 
	when we take it off we will terminate receiving events from that window.
	I believe that no window from this process will get on the list.
*/
static struct masklist {
	Window window;
	long count;
	struct masklist *next;	
} *MaskList = NULL;


struct seldata {
	struct proctable_Entry *proc;
	struct basicobject *object;
	long data;
	int refs;
};

int cursordebug =  0;
static int regionDebug = 0;
static boolean useBackingStore = TRUE;

static int iconMaxLength = -1;
static int titleMaxLength = -1;
static boolean useProgramNameInTitle = TRUE;

/* there is one mouseStatus struct per display
	it keeps track of the state of the button,
	the X and Y for the timer and the event struct, if any
*/
struct mouseStatus {
	struct mouseStatus *next;
	Display *display;
	Time lasttime;
	enum mouseState {msAllUp, msLeftDownPending, msRightDownPending,
			msLeftDown, msRightDown}
		state;
	long xPending, yPending;
	struct event *event;
	struct xim *xim;
};

struct mouseStatus *Mice = NULL;
static long MouseHysteresis = 50;	/* (milliseconds) delay in sending a down button */

static boolean mousedebug = FALSE;
static FILE *mf= NULL;	/* file to dump mouse info for debugging */

static Atom wmchangestate = NULL;

#define HITPIXELS 1
#define INITIALCUTBUFFERSIZE 200

/* atom defined by ATK */
static Atom xim_ATK = NULL;
static Atom xim_INCOMING;

/* atoms defined by ICCCM, but not in Xatom.h */
static Atom xim_TARGETS;
static Atom xim_TIMESTAMP;
static Atom xim_MULTIPLE;
static Atom xim_TEXT;
static Atom xim_INCR;
static Atom xim_CLIPBOARD;
static Atom xim_LENGTH;

static long CurrLeftIncr, CurrTopIncr;
static long *LeftIncrList;
static long *TopIncrList;
static long leftIncrCount, topIncrCount;
static long maxLeftCount, maxTopCount;

struct xwindowfiles  {
    int Xfileno;
    int count;
    char * host;
    Display *display;
};

static struct xwindowfiles *windowList;
static long numberOfWindowFiles = 0;
static long xWindowCtr = 0;
#define LEFTBUTTON 1
#define RIGHTBUTTON 3
#define MENUBUTTON 2

#define MENUBARHEIGHT(anXIMPtr) (anXIMPtr->menubaron?anXIMPtr->mbi->prefs->menubarheight:0)

#ifdef LWP
/* called on iomgr lwp's stack at a safe time*/
static int WakeUpIM(dummy)
    char *dummy;
{
    if (imPid != NULL)
        im_IOMGRCancel(imPid);
}
#endif /* LWP */

static int mystrcmp(x,y)
char **x,**y;
{
    return strcmp(*x,*y);
}

/* Grrrr, this used to be small...
  Specification:
  Items must be quoted, they can contain ','s, '"'s or
      '\'s.  '"'s and '\' must be escaped with a '\'.  Escaping any other character has no effect.
      Items are comma separated.
      */    
static char **SetupMenuChoices(menulistname,count)
char *menulistname;
int *count;
{
    int maxchoices=8;
    char **list;
    char *pref=environ_GetProfile(menulistname), *p;
    char buf[1024];
    boolean lookingforquote=FALSE;
    boolean havebackslash=FALSE;
    int c=0;
    *count=0;
    if(!pref) return NULL;
    else {
	if(*pref=='\0') return NULL;
	if(strlen(pref)>sizeof(buf)-1) { /* punt prefs too large */
	    fprintf(stderr,"MenuChoices preference %s is too large.\n",menulistname);
	    return NULL;
	}
    }
    
    list=(char **)malloc(sizeof(char *)*maxchoices);
    if(!list) return NULL;
    
    p=buf;
    while(pref && *pref && p-buf<sizeof(buf)-1) {
	boolean newbackslash;
	newbackslash=FALSE;
	*p=(*pref);
	switch(*pref) {
	    case '\"':
		if(!havebackslash) { /* this isn't escaped */
		    if(!lookingforquote) { /* we're not in a quoted item yet, now we will be */
			p=buf;
			lookingforquote=TRUE;
		    } else { /* we found an un-escaped quote within a quoted item, enter the item in the table. */
			*p='\0';
			list[c]=malloc(strlen(buf)+1);
			if(!list[c]) {
			    while(--c>=0 && list[c]) free(list[c]);
			    free(list);
			    return NULL;
			}
			strcpy(list[c],buf);
			c++;
			if(c>=maxchoices) { /* too many choices? then make more room */
			    maxchoices*=2;
			    list=(char **)realloc(list, sizeof(char *)*maxchoices);
			    if(!list) {
				/* If this happens some malloced data will be left hanging but I have no way of freeing it now... */
				return NULL;
			    }
			}
			p=buf;
			pref=index(pref,','); /* skip to the comma ending this item */
			lookingforquote=FALSE;
		    }
		} else p++;
		break;
	    case '\\':
		if(!havebackslash) newbackslash=TRUE; /* if we didn't have a backslash before now we will */
		else p++; /* just keep the escaped backslash */
		break;
	    default:
		p++; /* just keep any other characters */
	}
	if(pref && *pref) pref++; /* if we're not at the end of the preference string skip the character we just processed */
	havebackslash=newbackslash;
    }
    *count=c;

    /* no longer sort the list sort the list for "quick" lookup */
/*    qsort(list, c, sizeof(char *), mystrcmp);  */
    return list;
}

static void FreeMenuChoices(list,count)
char **list;
int count;
{
    while(--count>=0) free(list[count]);
    free(list);
}

static int CheckMenuChoice(list,count,choice)
char **list;
int count;
char *choice;
{
    int i;
    
    if(count==0) return -1;
    for(i=0;i<count;i++) if(strcmp(list[i], choice)==0) return i;
    return -1;    
}

struct cardorder {
    char **list;
    int count;
    int priorities[1];
};

static struct cardorder *SetupCardOrder(prefname)
char *prefname;
{
    int count,i;
    int curprio=10;
    int priostep=environ_GetProfileInt("PriorityStep",5);
    char **list=SetupMenuChoices(prefname, &count);
    struct cardorder *result=(struct cardorder *)malloc(sizeof(struct cardorder)+(count-1)*sizeof(int));
    if(!result) {
	if(list) FreeMenuChoices(list, count);
	return NULL;
    }
    if(!list) {
	free(result);
	return NULL;
    }
    result->count=count;
    result->list=list;
    for(i=0;i<count;i++) {
	char *p;
	p=index(list[i],'~');
	if(p) {
	    *p='\0';
	    curprio=atoi(p+1);
	} else curprio+=priostep;
	result->priorities[i]=curprio;
    }
    return result;
}

static int GetCardPriority(co, card, def)
struct cardorder *co;
char *card;
int def;
{
    int i=CheckMenuChoice(co->list, co->count, card);
    if(i<0) return def;
    else return co->priorities[i];
}

static void FreeCardOrder(co)
struct cardorder *co;
{
    FreeMenuChoices(co->list, co->count);
    free(co);
}

boolean xim__SupportsTransient(self)
struct xim *self;
{
    return TRUE;
}

boolean xim__SupportsOverride(self)
struct xim *self;
{
    return TRUE;
}

boolean xim__SupportsOffscreen(self)
struct xim *self;
{
    return TRUE;
}


/* Place andrew fonts at the front of the font list */
void xim__AddAndrewFontPath(classID,DisplayPtr)
    struct classheader *classID;
    Display * DisplayPtr; /* display for font path */
{
    char fontPath[256];	    /* Place where new font path will be built */
    char *andrewDir;	    /* Returned value of ANDREWDIR */
    char **fontPathList;   /* Return list of directories */
    int numPaths;	    /* Number of returned directories */
    int	i;		    /* Index into font path list */

    /* First get what is suposed to be the "correct" andrew font path, which is
       * defined as $ANDREWDIR/X11fonts, where ANDREWDIR should map to
       * "/usr/andrew" if undefined.
       */

    andrewDir = environ_AndrewDir("/X11fonts/");
    strcpy(fontPath, andrewDir);

    filetype_CanonicalizeFilename(fontPath, fontPath, sizeof(fontPath));

    /* Get the font paths from the server and see if it is already in front */
    fontPathList = XGetFontPath(DisplayPtr, &numPaths);

    for (i = 0; i < numPaths && (strcmp(fontPathList[i], fontPath) != 0); i++)
	;

    /* If necessary, insert the Andrew font directory in the fontpath. */
    if (i >= numPaths) {

	char **newPathList = (char **) malloc((numPaths + 1) * (sizeof(char *)));

	if (newPathList != NULL) {
	    for (i = 0; i < numPaths; ++i)
		newPathList[i] = fontPathList[i];

	    newPathList[i] = fontPath;

	    XSetFontPath(DisplayPtr, newPathList, numPaths + 1);
	    free(newPathList);
	}
    }

    /* Give back storage for returned font paths */
    if (fontPathList)
	XFreeFontPath(fontPathList);
  
}

static void
SetWMProperties(self, nameChanged)
struct xim *self;
boolean nameChanged;
{
    Display *display = xim2display(self);
    Window window = xim2window(self);
    char *WMName, *iconName, host[500];
    int lengthName = 0;
    int lengthTitle = 0;
    int lengthWMName;
    char *name = xim2programname(self);
    char *title = xim_GetTitle(self);
    int clen = 0;
    char *c = NULL;
    XWMHints *wmhints, *hints, myhints;

    if(self->IsOffscreenWindow) return;
    if(nameChanged) {
	XClassHint programClass;

	programClass.res_name = name;
	programClass.res_class = "atk";
	XSetClassHint(display, window, &programClass);
    }

    if(useProgramNameInTitle && name)
	lengthName = strlen(name);

    if(c = title) {
	clen = lengthTitle = strlen(c);
	if(titleMaxLength > 0 && lengthTitle > titleMaxLength) {
	    c = title + lengthTitle - titleMaxLength;
	    clen = titleMaxLength;
	    *c = '.'; *(c+1) = '.';
	}
    }

    if((WMName = (char*)malloc(lengthName + clen + 2)) == NULL)
	return;

    bcopy(name, WMName, lengthName);
    WMName[lengthName] = ' ';
    if(c)
	bcopy(c, WMName + lengthName + 1, clen);
    WMName[lengthWMName = lengthName + 1 + clen] = (char) 0;

    XStoreName(display, window, useProgramNameInTitle ? WMName : WMName + lengthName + 1);

    if(iconMaxLength > 0 && clen > iconMaxLength) {
	c = WMName + lengthWMName - iconMaxLength;
	clen = strlen(c);
	*c = '.'; *(c+1) = '.';
	bcopy(c, WMName + lengthName + 1, clen);
	WMName[lengthName + 1 + clen] = (char) 0;
	iconName = WMName;
    }
    else iconName = WMName;
    XSetIconName(display, window, iconName);
    free(WMName);

    if((wmhints = XGetWMHints (display, window)) == NULL) {
	hints = &myhints;
	hints->flags = 0;
    }
    else hints = wmhints;
    hints->flags |= StateHint;		/* try to make expose work -wjh */
    hints->initial_state = NormalState;
    hints->flags |= InputHint;
    hints->input = TRUE;
    XSetWMHints(display, window, hints);
    if(wmhints)
	XFree((char*) wmhints);

    gethostname(host, sizeof(host));
    XChangeProperty(display, window, XA_WM_CLIENT_MACHINE, XA_STRING,
                  8, PropModeReplace, (unsigned char*)host, strlen(host));
}

static int FirstTimeThrough = TRUE;

static char ErrMsg[120];
static long Nerrors = 0;

/* Define USEGETSERV if you want wmclient to look up service names
   in /etc/services.  We wire them in here for speed. */
#ifndef USEGETSERV
#define CONSPORT	2018		/* to avoid getservbyname calls */
#endif /* USEGETSERV */

int XErrorsToConsole(DisplayPtr, ErrorBlock)
Display * DisplayPtr;
XErrorEvent * ErrorBlock; {
    XGetErrorText(DisplayPtr,ErrorBlock->error_code,ErrMsg,sizeof(ErrMsg));
    errprintf(im_GetProgramName(),ERR_WARNING,0,0,"X error %s (%d) major code %d minor code %d ignored", ErrMsg, ErrorBlock->error_code, ErrorBlock->request_code, ErrorBlock->minor_code);
    Nerrors++;
}

void xim_EstablishConsole(xhost)
char * xhost; {
    int i;
    char tmpHostName[255];
    unsigned long hostaddr;
    register int fd;
    struct sockaddr_in sin;
    struct hostent *hostent;
#ifdef USEGETSERV
    struct servent *servent;
#endif /* USEGETSERV */
    static boolean established = FALSE;

    if (! established)  {
	/* Create a name with screen number */
	strcpy(tmpHostName, xhost);
	for (i=0;i<255 &&(tmpHostName[i] != 0)&& (tmpHostName[i] != ':');i++);
	if (i<255) tmpHostName[i] =	0;  /* Truncate at the : */

	/* get address of host */
	if ((i==0) || (strcmp("unix",tmpHostName)==0)) {
	    long getaddr();
	    hostaddr = getaddr();
	}
	else {
	    if (isdigit(*tmpHostName)) 
		hostent = gethostbyaddr(tmpHostName, strlen(tmpHostName), AF_INET);
	    else 
		hostent = gethostbyname(tmpHostName);
	    if (hostent == 0) {
		fprintf (stderr, "Could not find host %s in %s(xim). Error messages may not go to console.\n", tmpHostName, im_GetProgramName());
		fflush (stderr);
		return;
	    }
	    bcopy(hostent->h_addr, &hostaddr, sizeof(hostaddr));
	}

	/* Have the address, try to open the special connection */
	/* establish a connection from stderr to console error log */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
#ifdef USEGETSERV
	servent = getservbyname("console", 0);
	if (servent != NULL && fd >= 0)
#else /* USEGETSERV */
        if (fd >= 0)
#endif /* USEGETSERV */
	{
	    bzero((char *) &sin, sizeof(sin));
	    sin.sin_family = AF_INET;
#ifdef USEGETSERV
	    sin.sin_port = servent->s_port;
#else /* USEGETSERV */
	    sin.sin_port = htons(CONSPORT);
#endif /* USEGETSERV */
	    sin.sin_addr.s_addr = hostaddr;
	    if (connect(fd, &sin, sizeof(sin)) >= 0) {
		/*
		 Ignore SIGPIPE since writes on a connected udp
		 socket can generate SIGPIPE if there is no
		     receiver on the other side.
		     */

		signal(SIGPIPE, SIG_IGN);

		fclose(stderr);
		dup2(fd, 2);
#ifdef hpux
	    setvbuf(fdopen(2, "w"), NULL, _IOLBF, BUFSIZ);
#else /* hpux */
		setlinebuf(fdopen(2, "w"));
#endif /* hpux */
	    }
	}
	if (fd >= 0)
	    close(fd);
	established = TRUE;
    }
}

void HandleExposeFromMenubar();

static struct keybinding {
    KeySym key;
    char *binding;
} keybindings[] = {
    XK_Home, "\033H", /* esc H  */
    XK_Left, "\033D", /* esc D */
    XK_Up, "\033A", /* esc A */
    XK_Right, "\033C", /* esc C */
    XK_Down, "\033B", /* esc B */
    XK_Prior, "\033G", /* esc G */
    XK_Next, "\033E", /* esc E */
    XK_End, "\033F", /* esc F */
    XK_Begin, "\001", /* control A */
#if 0
    XK_Insert, "\033", /* esc */
    XK_Delete, "\004", /* control D */
#endif /* 0 */

    /* give unique mappings to every key  - CCH */
    /* mind you, these are damn non-standard  
      they should be ESC-digit-digit-digit-letter   -wjh */
    XK_F1, "\033S",   	/*  esc S */
    XK_F2, "\033T",  	/* esc T */
    XK_F3, "\033U",  	/* esc U */
    XK_F4, "\033V",  	/*  esc V */
    XK_F5, "\033W",  	/* esc W */
    XK_F6, "\033P",  	/* esc P */
    XK_F7, "\033Q",  	/* esc Q */
    XK_F8, "\033R",  	/* esc R */
    XK_F9, "\033Y",  	/* esc Y */
    XK_F10, "\033<",  	/* esc < */
    XK_F11, "\033[",  	/* esc [ */
    XK_F12, "\033,",  	/* esc , */

    XK_L1, "\033E",   	/* esc E */
    XK_L2, "\033F",  	/* esc F */
    XK_L3, "\033G",  	/* esc G */
    XK_L4, "\033H",  	/* esc H */
    XK_L5, "\033I",    	/* esc I */
    XK_L6, "\033J",   	/* esc J */
    XK_L7, "\033K",  	/* esc K */
    XK_L8, "\033L",  	/* esc L */
    XK_L9, "\033M",  	/* esc M */
    XK_L10, "\033N",  	/* esc N */

    XK_R1, "\033O",  	/* esc O */
    XK_R2, "\033X",  	/* esc X */
    XK_R3, "\033Z",  	/* esc Z */
    XK_R4, "\033-",   	/* esc - */
    XK_R5, "\033=",   	/* esc = */
    XK_R6, "\033+",  	/* esc + */
    XK_R7, "\033/",    	/* esc / */
    XK_R8, "\033A",  	/* esc A */
    XK_R9, "\033*",   	/* esc * */
    XK_R10, "\033D",  	/* esc D */
    XK_R11, "\033&",  	/* esc & */
    XK_R12, "\033C",  	/* esc C */
    XK_R13, "\033:",  	/* esc : */
    XK_R14, "\033B",  	/* esc B */
    XK_R15, "\033;",  	/* esc ; */

#ifdef notdef
    /* the old stuff mapped multiple keys to same code */
    XK_F1, "\033S", /*  esc S */
    XK_L1, "\033S", /* esc S */
    XK_R1, "\033S", /* esc S */
    XK_F2, "\033T", /* esc T */
    XK_L2, "\033T", /* esc T */
    XK_R2, "\033T", /* esc T */
    XK_F3, "\033U", /* esc U */
    XK_L3, "\033U", /* esc U */
    XK_R3, "\033U", /* esc U */
    XK_F4, "\033V", /*  esc V */
    XK_L4, "\033V", /* esc V */
    XK_R4, "\033V", /* esc V */
    XK_F5, "\033W", /* esc W */
    XK_L5, "\033W", /* esc W */
    XK_R5, "\033W", /* esc W */
    XK_F6, "\033P", /* esc P */
    XK_L6, "\033P", /* esc P */
    XK_R6, "\033P", /* esc P */
    XK_F7, "\033Q", /* esc Q */
    XK_L7, "\033Q", /* esc Q */
    XK_R7, "\033Q", /* esc Q */
    XK_F8, "\033R", /* esc R */
    XK_L8, "\033R", /* esc R */
    XK_R8, "\033R", /* esc R */
    XK_F9, "\033Y", /* esc Y */
    XK_L9, "\033Y", /* esc Y */
    XK_R9, "\033Y", /* esc Y */
    XK_F10, "\033<", /* esc < */
    XK_L10, "\033<", /* esc < */
    XK_R10, "\033<", /* esc < */
    XK_F11, "\033[", /* esc [ */
    XK_R11, "\033[", /* esc [ */
    XK_F12, "\033,", /* esc , */
    XK_R12, "\033,", /* esc , */
#endif /* notdef */

};

/* SetupDisplay: sets up  state ATK needs on a new X server returns NULL if the display cannot be opened. */
static Display *SetupDisplay(self, host)
struct xim *self;
char *host;
{
    int i, Xfileno;
    struct mouseStatus *tmouse;
    char *xhost;
    struct keybinding *thisBinding;
    Display *xDisplay;
    static int doSynch=0;
    
    /* ensure that 'host' points to a host name */
    if (host == NULL || *host == '\0') 
	/* no host supplied, simluate what XOpenDisplay will do 
		so that we can cache the host name for later reuse */
	host = (defaultServerHost != NULL) 
		? defaultServerHost
		: environ_Get("DISPLAY");
    if (host == NULL)  
	host = "unix";

    /* copy host to be xhost.  
	Append ":0 if needed */
    xhost = malloc(strlen(host)+3);
    strcpy(xhost, host);
    if (index(xhost, ':') == 0) 
	strcat(xhost, ":0");
    for (host = xhost; *host; host++)
	if (isupper(*host)) *host = tolower(*host);

    /* XXX probably ought to canonicalize the hostname to the "official" name  */

    /* See if we already opened a display for this host */
    xDisplay = NULL;
    for (i = 0; i < xWindowCtr; i++)  {
	if (strcmp(windowList[i].host, xhost) == 0)  {
	    struct mouseStatus *tmouse;
	    /* (This really should be a fake "insertgraphic" into its own graphic 
		to guarantee that all graphic state exists alike for all views.) */

	    windowList[i].count++;
	    xDisplay=windowList[i].display;
	    xim2display(self) = xDisplay;
	    xim2screen(self) = DefaultScreen(xDisplay);
	    xim2valid(self) = TRUE;
	    /* find the proper entry in Mice for this display and set self->MouseFacts */
	    for (tmouse = Mice; 
		 tmouse->display != xim2display(self); 
		 tmouse = tmouse->next);
	    self->MouseFacts = tmouse;
	    free(xhost); /* we've already seen this hosts name it's in our table from before, no need for a new copy */
	    return xDisplay;
	}
    }

    if ((xDisplay = XOpenDisplay(xhost)) == NULL)  {
        printf("Could not open the display; this program will not run without a window system.\n");
        return NULL;
    }
    
    if (wmchangestate == NULL) 
	wmchangestate = XInternAtom(xDisplay, "WM_CHANGE_STATE", TRUE);

    xim_EstablishConsole(xhost);

    (void) XSynchronize(xDisplay,doSynch);
    
    /* (This really should be a fake "insertgraphic" into its own graphic 
	 to guarantee that all graphic state exists alike for all views.) */

    xim2display(self) = xDisplay;
    xim2screen(self) = DefaultScreen(xDisplay);
    xim2valid(self) = TRUE;
      
    Xfileno = ConnectionNumber(xim2display(self));
    /* New display (might) require a new font path */
    xim_AddAndrewFontPath(xim2display(self));

    for (thisBinding = keybindings; 
	  thisBinding < keybindings + sizeof(keybindings) / sizeof(struct keybinding);
	  thisBinding++)
	XRebindKeysym(xim2display(self), thisBinding->key, NULL, 0, (unsigned char *)thisBinding->binding, 2);

    /* New display, so see if someone has bothered to define the cut buffers yet! */
    if (xim_ATK == NULL) {
	xim_ATK = XInternAtom(xim2display(self), "ATK", FALSE);
	xim_INCOMING = XInternAtom(xim2display(self), "INCOMING", FALSE);
	xim_TARGETS = XInternAtom(xim2display(self), "TARGETS", FALSE);
	xim_TIMESTAMP = XInternAtom(xim2display(self), "TIMESTAMP", FALSE);
	xim_MULTIPLE = XInternAtom(xim2display(self), "MULTIPLE", FALSE);
	xim_TEXT = XInternAtom(xim2display(self), "TEXT", FALSE);
	xim_INCR = XInternAtom(xim2display(self), "INCR", FALSE);
	xim_CLIPBOARD = XInternAtom(xim2display(self), "CLIPBOARD", FALSE);
	xim_LENGTH = XInternAtom(xim2display(self), "LENGTH", FALSE);
    }
    for (i=0;i<8;i++) {
	Atom RetAtom;
	int RetFormat;
	unsigned long RetNumItems;
	unsigned long RetBytesAfter;
	unsigned char * RetData = NULL;
	static Atom n_to_atom[8] = { 
	    XA_CUT_BUFFER0,
	    XA_CUT_BUFFER1,
	    XA_CUT_BUFFER2,
	    XA_CUT_BUFFER3,
	    XA_CUT_BUFFER4,
	    XA_CUT_BUFFER5,
	    XA_CUT_BUFFER6,
	    XA_CUT_BUFFER7};

	    (void) XGetWindowProperty(xim2display(self), RootWindow(xim2display(self), DefaultScreen(xim2display(self))),  
				      n_to_atom[i], 0L, 0L, False,
				      /* I'll take any kind of cut buffer!*/ AnyPropertyType,
				      &RetAtom, &RetFormat, &RetNumItems, &RetBytesAfter,  &RetData);
	    if (RetAtom == None) 
		XStoreBuffer(xim2display(self), (char *)&RetNumItems, 0, i);
	    if (RetData) XFree(RetData);
    }

    /* set up the mouseStatus structure for the new display */
    tmouse = (struct mouseStatus *)malloc(sizeof(struct mouseStatus));
    tmouse->next = Mice;
    tmouse->display = xim2display(self);
    tmouse->lasttime = CurrentTime;
    tmouse->state = msAllUp;
    Mice = tmouse;
    self->MouseFacts = tmouse;

    /* See if have a new file descriptor to select on */
    for (i = 0; i < xWindowCtr; i++)  {
	if (windowList[i].Xfileno == Xfileno)  {
	    windowList[i].count += 1;
	    break;
	}
    }
    if (i == xWindowCtr)  {
	if (numberOfWindowFiles == 0)  {
	    numberOfWindowFiles = 2;
	    windowList = (struct xwindowfiles *) malloc(2 * sizeof(struct xwindowfiles));
	}
	else if (xWindowCtr == numberOfWindowFiles)  {
	    numberOfWindowFiles *= 2;
	    windowList = (struct xwindowfiles *)realloc(windowList, numberOfWindowFiles * sizeof(struct xwindowfiles));
	}
	windowList[i].Xfileno = Xfileno;
	windowList[i].count = 1;
	windowList[i].display = xim2display(self);
	windowList[i].host = xhost;
	xWindowCtr += 1;
	self->Xfileno = Xfileno;
    }

    return xDisplay;
}

static void DoGeometry(self, left, top,sizehintsp, zoomhintsp)
struct xim *self;
int *top, *left;
XSizeHints **sizehintsp, **zoomhintsp;
{   
    static char *PriorSpec = NULL;
    boolean GeometrySize, GeometryPosition;
    static XSizeHints sizehints, zoomhints;

    GeometrySize = FALSE;
    GeometryPosition = FALSE;
    *sizehintsp=NULL;
    *zoomhintsp=NULL;
    if (geometrySpec != NULL)  {
	int mask, x, y;unsigned int width, height;
	mask = XParseGeometry(geometrySpec, &x, &y, &width, &height);
	if (mask & (XValue | YValue))  {
	    GeometryPosition = TRUE;
	    if (mask & XValue)
		preferedLeft = (mask & XNegative) ? x - 1 : x;
	    if (mask & YValue) 
		preferedTop = (mask & YNegative) ? y - 1 : y;
	}
	if (mask & (WidthValue | HeightValue))  {
	    GeometrySize = TRUE;
	    if (mask & WidthValue) 
		preferedWidth = width;
	    if (mask & HeightValue)  
		preferedHeight = height;
	}
    }

    if (preferedWidth<=0) {
	errprintf(xim2programname(self),ERR_WARNING,0,0,
		  "window of width %d requested for %X; using 91 instead", 
		  preferedWidth, self);
	preferedWidth = 91;
    }
    if (preferedHeight<=0) {
	errprintf(xim2programname(self),ERR_WARNING,0,0,
		  "window of height %d requested for %X; using 20 instead",
		  preferedHeight, self);
	preferedHeight = 20;
    }
    if (preferedLeft < 0)
	*left = DisplayWidth(xim2display(self), DefaultScreen(xim2display(self))) + preferedLeft - preferedWidth;
    else
	*left = preferedLeft;
    if (preferedTop < 0)
	*top = DisplayHeight(xim2display(self), DefaultScreen(xim2display(self))) + preferedTop - preferedHeight;
    else
	*top = preferedTop;

    if (PriorSpec != NULL && PriorSpec == geometrySpec) {   /* (compare addresses) */
	/* another window for same specification.  
	    Use increments, if any.  
		Otherwise use PPosition to try to get rubberbanding.
		*/

	CurrTopIncr += TopIncrList[topIncrCount];
	CurrLeftIncr += LeftIncrList[leftIncrCount];

	if (TopIncrList[topIncrCount] == 0  &&  LeftIncrList[leftIncrCount] == 0) {
	    /* if the window would appear at the same place,
		we try to get rubberbanding of position */
	    GeometryPosition = FALSE;
	    *top = 0;  *left = 0;		/* these cause twm (R3) to rubberband */
	}
	else {
	    *left += CurrLeftIncr;
	    *top += CurrTopIncr;
    }

	if (topIncrCount + 1 < maxTopCount) {
	    topIncrCount++;
	}
	if (leftIncrCount + 1 < maxLeftCount) {
	    leftIncrCount++;
	}
    }
    else {
	/* new specification.  use it. */
	CurrTopIncr = 0;
	CurrLeftIncr = 0;
	PriorSpec = geometrySpec;
    }

    /* set size hints
      ASSUMPTION: if dimensions were set with im_SetDimensions
	  it was done by the program
	  but if set with im_SetGeometrySpec it was by the user
	      The latest ICCCM says that the x,y,width,height values in the hints are ignored,
	      but twm still uses them as of June, 1989.
	      */	
    if (setDimensions || GeometrySize || GeometryPosition) {
	sizehints.flags = PMinSize | PMaxSize
	  | ((GeometrySize) ? USSize : (setDimensions) ? PSize : 0)
	  | ((GeometryPosition) ? USPosition : (setDimensions) ? PPosition : 0);
	sizehints.x = *left;
	sizehints.y = *top;
	sizehints.width = preferedWidth;
	sizehints.height = preferedHeight;
	sizehints.min_width = (preferedWidth < 91) ? preferedWidth : 91;
	sizehints.min_height = (preferedHeight < 20) ? preferedHeight : 20;
	sizehints.max_width = DisplayWidth(xim2display(self), DefaultScreen(xim2display(self)));
	sizehints.max_height = DisplayHeight(xim2display(self), DefaultScreen(xim2display(self)));
	*sizehintsp=(&sizehints);
    }
    else {}	/* no size hints if none set */

    zoomhints.flags = (PSize | PPosition | PMinSize | PMaxSize);
    zoomhints.x = 6;
    zoomhints.y = 6;
    zoomhints.width = DisplayWidth(xim2display(self), DefaultScreen(xim2display(self))) - 12;
    zoomhints.height = DisplayHeight(xim2display(self), DefaultScreen(xim2display(self))) - 12;
    zoomhints.min_width = preferedWidth;
    zoomhints.min_height = preferedHeight;
    zoomhints.max_width = DisplayWidth(xim2display(self), DefaultScreen(xim2display(self)));
    zoomhints.max_height = DisplayHeight(xim2display(self), DefaultScreen(xim2display(self)));
    *zoomhintsp=(&zoomhints);
}

static void DoTransientGeometry(self, override, other, left, top, width, height, sizehintsp, zoomhintsp)
struct xim *self, *other;
boolean override;
int *top, *left, *width, *height;
XSizeHints **sizehintsp, **zoomhintsp;
{
    Display *xDisplay=xim2display(self);
    Window oWindow=xim2window(other);
    Window discardWindow;
    int ox, oy;
    unsigned int owidth, oheight, obwidth, odepth;
    static XSizeHints sizehints, zoomhints;
    unsigned int overridepos=0;
    *sizehintsp=NULL;
    *zoomhintsp=NULL;

    XGetGeometry (xDisplay, oWindow, &discardWindow, &ox, &oy, &owidth, &oheight, &obwidth, &odepth);
  
#ifdef TRANSIENTUSEGEOMETRYSPEC
    if (geometrySpec != NULL)  {
        int mask, x, y;unsigned int gwidth, gheight;
        mask = XParseGeometry(geometrySpec, &x, &y, &gwidth, &gheight);
        if (mask & (XValue | YValue))  {
                  GeometryPosition = TRUE;
	    if (mask & XValue)
		preferedLeft = (mask & XNegative) ? x - 1 : x;
	    if (mask & YValue) 
		preferedTop = (mask & YNegative) ? y - 1 : y;
        }
        if (mask & (WidthValue | HeightValue))  {
                  GeometrySize = TRUE;
	    if (mask & WidthValue) 
		preferedWidth = gwidth;
	    if (mask & HeightValue)  
		preferedHeight = gheight;
        }
    }
#endif /* TRANSIENTUSEGEOMETRYSPEC */

    /* Tricky part: the other window may be a child of a window manager
      decorated window.  So we ignore x and y from XGetGeometry
      and use XTranslateCoordinates to get it's location. */

    if (override) { /* overrides are relative to parent */
	ox = 0;
	oy = MENUBARHEIGHT(other);
	oheight -= oy;
    } else
	XTranslateCoordinates(xDisplay, oWindow, RootWindow(xDisplay, DefaultScreen(xDisplay)), 0, 0, &ox, &oy, &discardWindow);

    if (preferedWidth <= 0) {
	*width = owidth / 2;
	if(preferedWidth<0) errprintf(xim2programname(self),ERR_WARNING,0,0,
		"window of width %d requested for %X; using %d instead", 
		preferedWidth, self, *width);
	
    } else *width = preferedWidth;

    if (preferedHeight <= 0) {
	*height = oheight / 4;
	if(preferedHeight<0) errprintf(xim2programname(self),ERR_WARNING,0,0,
		"window of height %d requested for %X; using %d instead",
		preferedHeight, self, *height);
    } else *height = preferedHeight;
   
   if (preferedLeft == 0) {
       if(override) overridepos|=im_Centered;
       else *left = ((owidth / 2) - (*width / 2));
   } else *left =  preferedLeft;

    if (preferedTop == 0) {
	if(override) overridepos|=im_InMiddle;
	else *top = ((oheight / 2) - (*height / 2));
    } else *top = preferedTop;

    if(override) {
	xim_ConfigureRock(self)|=overridepos;
	if(xim_ConfigureFunction(self)) xim_ConfigureFunction(self)(self, xim_ConfigureRock(self), xim_ConfigureCustomRock(self), other, left, top, width, height);
	*left+=ox;
	*top+=oy;
	if (*left < 0) *left = 0;
	if (*top < 0) *top = 0;
    } else {
	xim_ConfigureRock(self)|=overridepos;
	if(xim_ConfigureFunction(self)) xim_ConfigureFunction(self)(self, xim_ConfigureRock(self), xim_ConfigureCustomRock(self), other, left, top, width, height);
	*left+=ox;
	*top+=oy;
	if(self->menubaron && height) *height+=MENUBARHEIGHT(self); 
	if (*left < 0)
	    *left = DisplayWidth(xDisplay, DefaultScreen(xDisplay)) + *left - *width;
	if (*top < 0)
	    *top = DisplayHeight(xDisplay, DefaultScreen(xDisplay)) + *top - *height;
    }
   /* set size hints
	ASSUMPTION: if dimensions were set with im_SetDimensions
			it was done by the program
		but if set with im_SetGeometrySpec it was by the user
	The latest ICCCM says that the x,y,width,height values in the hints are ignored,
	but twm still uses them as of June, 1989.
    */	
    sizehints.flags = PMinSize | PMaxSize	| USSize | PSize | USPosition | PPosition;
    sizehints.x = *left;
    sizehints.y = *top;
    sizehints.width = *width;
    sizehints.height = *height;
    sizehints.min_width = *width;
    sizehints.min_height = *height;
    sizehints.max_width = DisplayWidth(xDisplay, DefaultScreen(xDisplay));
    sizehints.max_height = DisplayHeight(xDisplay, DefaultScreen(xDisplay));
    *sizehintsp=(&sizehints);
    
    zoomhints.flags = (PSize | PPosition | PMinSize | PMaxSize);
    zoomhints.x = 6;
    zoomhints.y = 6;
    zoomhints.width = DisplayWidth(xDisplay, DefaultScreen(xDisplay)) - 12;
    zoomhints.height = DisplayHeight(xDisplay, DefaultScreen(xDisplay)) - 12;
    zoomhints.min_width = preferedWidth;
    zoomhints.min_height = preferedHeight;
    zoomhints.max_width = DisplayWidth(xDisplay, DefaultScreen(xDisplay));
    zoomhints.max_height = DisplayHeight(xDisplay, DefaultScreen(xDisplay));
    *zoomhintsp=(&zoomhints);
}

static void SetForegroundBackground(self, foregroundColor, backgroundColor, foreground, background)
struct xim *self;
char **foregroundColor, **backgroundColor;
XColor *foreground, *background;
{
    long status;
    graphic_GetDefaultColors(foregroundColor, backgroundColor);
    if (foregroundColor != NULL)  {
	status = XParseColor(xim2display(self), DefaultColormap(xim2display(self), DefaultScreen(xim2display(self))), *foregroundColor, foreground);
	if (status)
	    status = XAllocColor(xim2display(self), DefaultColormap(xim2display(self), DefaultScreen(xim2display(self))), foreground);
	if (!status) {
	    foreground->pixel=BlackPixel(xim2display(self), DefaultScreen(xim2display(self)));
	    foreground->red=foreground->green=foreground->blue=0;
	}
    }

    self->backgroundPixel = WhitePixel(xim2display(self), DefaultScreen(xim2display(self)));
    if (backgroundColor != NULL)  {
	status = XParseColor(xim2display(self), DefaultColormap(xim2display(self), DefaultScreen(xim2display(self))), *backgroundColor, background);
	if (status)
	    status = XAllocColor(xim2display(self), DefaultColormap(xim2display(self), DefaultScreen(xim2display(self))), background);
	if (!status) {
	    background->pixel=WhitePixel(xim2display(self), DefaultScreen(xim2display(self)));
	    background->red=background->green=background->blue=65535;
	}
	self->backgroundPixel = background->pixel;
    }
}


static struct menubar *MakeStartupMenu(mbi, progname)
struct mbinit *mbi;
char *progname;
{
    char buf[256];
    static char *Quit="Quit";
    struct menubar *mb;
    strncpy(buf, progname, sizeof(buf)-1);
    strncat(buf, " initializing...", sizeof(buf)-1-strlen(buf));
    mb=mb_Create(mbi, buf, "" , NULL, NULL);
    if(!mb) return NULL;
    mb_AddSelection(mb, NULL, 1, Quit, 1, FALSE, NULL);
    return mb;
}

static void FreeSelectionData(seldata)
struct seldata *seldata;
{
    if(!--(seldata->refs)) free(seldata);
}
   
boolean xim__CreateWindow(self, host)
struct xim *self;
char *host;
{
    Display *xDisplay;
    Window newWindow;
    XSetWindowAttributes windowAttributes;

    XSizeHints *sizehints, *zoomhints;
    int left, top;
    
    char *foregroundColor;
    char *backgroundColor;
    XColor foreground, background;

    /* see if we have installed the error handler yet  and performed special mappings */
    if (FirstTimeThrough) {
	FirstTimeThrough = FALSE;

	/* Setup error handler */
	XSetErrorHandler((XErrorHandler)XErrorsToConsole);
    }
    self->IsOffscreenWindow = FALSE; /* redundant, but useful for debugging offscreen windows */
    if ((xDisplay=SetupDisplay(self, host)) == NULL) return NULL;
    
    DoGeometry(self, &left, &top, &sizehints, &zoomhints);

    SetForegroundBackground(self, &foregroundColor, &backgroundColor, &foreground, &background);

    if(self->menubaron) {
	self->mbi=mb_Init(xDisplay, &foreground, &background, HandleExposeFromMenubar, self, FreeSelectionData);

	if(self->mbi==NULL) {
	    self->menubaron=FALSE;
	    self->cmenuson=TRUE;
	} else {
	    newWindow = XCreateSimpleWindow(xDisplay, RootWindow(xDisplay, DefaultScreen(xDisplay)), left, top, preferedWidth, preferedHeight, 2, BlackPixel(xDisplay, DefaultScreen(xDisplay)), WhitePixel(xDisplay, DefaultScreen(xDisplay)));
	    mb_InitWindows(self->mbi, newWindow);
	    self->menu=self->startupmenu=MakeStartupMenu(self->mbi, xim2programname(self));
	}
    } else {
	newWindow = XCreateSimpleWindow(xDisplay, RootWindow(xDisplay, DefaultScreen(xDisplay)), left, top, preferedWidth, preferedHeight, 2, BlackPixel(xDisplay, DefaultScreen(xDisplay)), WhitePixel(xDisplay, DefaultScreen(xDisplay)));
   }

    xim2window(self)= newWindow;
    
    if (useBackingStore)  {
	windowAttributes.backing_store = WhenMapped;
	XChangeWindowAttributes(xDisplay, newWindow, CWBackingStore, &windowAttributes);
    }

   /* Note: we assume default values of ForgetGravity to guarantee
    that any window reconfiguration will generate an exposure event,
    thereby eliminating the need to capture or select for configure
    events (comment obsolete, configure events may be used by the menubar
    and at some point in the future perhaps by transient window
    code) */
    /* gravity notify events are needed to update the location cache */

    /* Register our newly created window with whatever window manager is running (if any) */
    
    XSelectInput(xDisplay, newWindow, STANDARDEVENTMASK);
   
  
    /* Include all window header information */
    SetWMProperties(self,  TRUE);
    
    if(sizehints) XSetNormalHints(xDisplay, xim2window(self), sizehints);
    if(zoomhints) XSetZoomHints(xDisplay, xim2window(self), zoomhints);
    
    XSetWindowBorder(xDisplay, xim2window(self), foreground.pixel);
    XSetWindowBackground(xDisplay, xim2window(self), background.pixel);
    
    point_SetPt(&xim_GetDrawable(self)->physicalOrigin, 0, MENUBARHEIGHT(self));

    /* Bring the process cursor situation up to date, 
	i.e., if a process cursor is outstanding, make sure this new window picks it up */
    updateGlobalCursors(self);

    im_SetLastUsed(self);
    LocateWindow(self);
    return TRUE;
}

/* Similar to CreateWindow, except that we are a 
 transient for another pre-existing window.  
 We assure ourselves that the other window is
 valid, and we skip all the once-only stuff
 associated with making a window. (It was done
 for the other.) 

 The desired position is relative
 to the position of the other window.  This tailors
 CreateTransientWindow for use in making dialogue
 boxes. 

 If you set desired position to 0,0 and desired height and 
 width to the size of the dialogue box, it will appear centered
 on the other window.

 If the override flag is TRUE, this is an override-redirect
 window being created.  It will get key and mouse events
 INSTEAD of the 'other'window.

 If the override flag is FALSE this is just a transient window.
 It will get key and mouse events IN ADDITION TO the other
 window. */

static boolean
DoCreateTransientWindow(self, other, override)
    struct xim *self, *other;
    int override;
{
    Display *xDisplay;
    Window oWindow, newWindow;
    int i;
    int Xfileno;
    static int doSynch = 0;
    XSetWindowAttributes windowAttributes;
    XSizeHints *sizehints, *zoomhints;
    int left, top;
    char *foregroundColor;
    char *backgroundColor;
    XColor foreground, background;
    unsigned int width, height;

    if(override) {
	self->menubaron=FALSE;
	self->cmenuson=FALSE;
    }
    
    if (!(xim2window(other) && xim2display(other) && xim2valid(other))) {
	fprintf(stderr,"Other window for this transient invalid. Creating top level window.\n");
	return xim_CreateWindow(self, NULL);
    }

    xDisplay = xim2display(other);
    oWindow = xim2window(other);

    (void) XSynchronize(xDisplay,doSynch);
  
    xim2display(self) = xDisplay;
    xim2screen(self) = DefaultScreen(xDisplay);
    xim2valid(self) = TRUE;

    /* Note: the call to DoTransientGeometry has been moved down to just before the relevant XCreateSimpleWindow calls since DoTransientGeometry now needs to have the mbi structure if the menubar is on. */
    
    SetForegroundBackground(self, &foregroundColor, &backgroundColor, &foreground, &background);

    if(self->menubaron) {
	
	self->mbi=mb_Init(xDisplay,&foreground, &background, HandleExposeFromMenubar, self, FreeSelectionData);
	if(self->mbi==NULL) {
	    self->menubaron=FALSE;
	    self->cmenuson=TRUE;
	} else {
	    /* note this cannot be an override because overrides don't have either menu system enabled */
	    DoTransientGeometry(self, override, other, &left, &top, &width, &height, &sizehints, &zoomhints);
	    newWindow = XCreateSimpleWindow(xDisplay, RootWindow(xDisplay, DefaultScreen(xDisplay)), left, top, width, height, 2, BlackPixel(xDisplay, DefaultScreen(xDisplay)), WhitePixel(xDisplay, DefaultScreen(xDisplay)));
	    mb_InitWindows(self->mbi, newWindow);
	    self->menu=self->startupmenu=MakeStartupMenu(self->mbi, xim2programname(self));
	}
    } else {
	DoTransientGeometry(self, override, other, &left, &top, &width, &height, &sizehints, &zoomhints);
	newWindow = XCreateSimpleWindow(xDisplay, override?oWindow:RootWindow(xDisplay, DefaultScreen(xDisplay)), left, top, width, height, 2, BlackPixel(xDisplay, DefaultScreen(xDisplay)), WhitePixel(xDisplay, DefaultScreen(xDisplay)));
    } 
    /* (This really should be a fake "insertgraphic" into its own graphic 
	to guarantee that all graphic state exists alike for all views.) */
    xim2window(self)= newWindow;
    
    if (useBackingStore)  {
	windowAttributes.backing_store = WhenMapped;
	XChangeWindowAttributes(xDisplay, newWindow, CWBackingStore, &windowAttributes);
    }

    if(sizehints) XSetNormalHints(xDisplay, xim2window(self), sizehints);
    if(zoomhints) XSetZoomHints(xDisplay, xim2window(self), zoomhints);

    
    XSetWindowBorder(xDisplay, xim2window(self), foreground.pixel);
    XSetWindowBackground(xDisplay, xim2window(self), background.pixel);
   
    /* Register our newly created window with whatever window manager is running (if any) */

    XSelectInput(xDisplay, newWindow, STANDARDEVENTMASK);
    

    /* MouseFacts, xDisplay, and Xfileno should be the same for 
	both self and other */ 

    Xfileno = ConnectionNumber(xDisplay);
    self->MouseFacts = other->MouseFacts;
    self->Xfileno = Xfileno;

    /* Increment count of windows on this file descriptor. */
    for (i = 0; i < xWindowCtr; i++)  {
	if (windowList[i].Xfileno == Xfileno)  {
	    windowList[i].count += 1;
	    break;
	}
    }
    /* Include all window header information */
    SetWMProperties(self,  TRUE);

    /* the most important property: TransientFor */
    XSetTransientForHint(xDisplay, newWindow, oWindow);
    
    point_SetPt(&xim_GetDrawable(self)->physicalOrigin, 0, MENUBARHEIGHT(self));

    /* Bring the process cursor situation up to date, 
	i.e., if a process cursor is outstanding, make sure this new window picks it up */
    updateGlobalCursors(self);

    im_SetLastUsed(self);

    /* for overrides, set self->popup_parent to 'other' */
    /* Set other's popup_active to self */
    /* If it is already non-null, follow popup_active chain
      until we find one that is null */

    if (override) {
	struct xim *parent = other;
	XWindowChanges changes;
	XWMHints hints;
	int mask = 0;

	hints.window_group = oWindow;
	hints.flags = WindowGroupHint;
	XSetWMHints(xDisplay, newWindow, &hints);

	self->popup_parent = other;
	/* this will set parent to the last window and choose the right sibling window if any */
	while (parent->popup_active) {
	    parent = parent->popup_active;
	    changes.sibling = xim2window(parent);
	    changes.stack_mode = Above;
	    mask = CWSibling | CWStackMode;
	}
	if (mask) XConfigureWindow(xim2display(self), xim2window(self), mask, &changes);
	parent->popup_active = self;
    }

    return TRUE;
}

boolean
xim__CreateTransientWindow(self, other)
    struct xim *self, *other;
{
    return (DoCreateTransientWindow(self, other, FALSE));
}

boolean
xim__CreateOverrideWindow(self, other)
    struct xim *self, *other;
{
    return (DoCreateTransientWindow(self, other, TRUE));
}

void xim__FlushAllWindows(classID)
    struct classheader *classID;
{

    int i;

    for (i = 0; i < xWindowCtr; i++)
        XFlush(windowList[i].display);
}


/* Menu stuff... */

	void 
ExplodeMenuString(str, paneStr, paneStrLen, panePriority, 
		selectionStr, selectionStrLen, selectionPriority)
	char *str;
	char *paneStr;
	long paneStrLen;
	long *panePriority;
	char *selectionStr;
	long selectionStrLen;
	long *selectionPriority;
{
	char *p;
	char *pLimit;
	boolean inSelection = FALSE;
	boolean inPriority = FALSE;
	long priority = 0;
	
	p = paneStr;
	pLimit = paneStr + paneStrLen;
	for ( ; ; str++) switch (*str)  {
		case '\0':
		case ',':
			*p = '\0';
			if (inSelection)  {
				if (selectionPriority != NULL)
					*selectionPriority = priority;
				return;
			}
			else if (*str == '\0')  {
				strncpy(selectionStr, paneStr, selectionStrLen);
				selectionStr[selectionStrLen - 1] = '\0';
				paneStr[0] = '\0';
				if (selectionPriority != NULL)
					*selectionPriority = priority;

				/* Pane priority of -1 is special to cmenu_AddPane. 
				   It essentially means put this pane at the end of the list... 
				   In the InstallMenus code we special case the empty 
				   title pane to give it a priority of 0. Life sucks... 
				*/
				if (panePriority != NULL)
					*panePriority = -1;
				return;
			}
			if (panePriority != NULL) 
				*panePriority = (priority == 0) ? -1 : priority;
			p = selectionStr;
			pLimit = selectionStr + selectionStrLen;
			inSelection = TRUE;
			inPriority = FALSE;
			priority = 0;
			break;
		case '~':
			inPriority = TRUE;
			break;
		default:
			if (inPriority)
				priority = priority * 10 + *str - '0';
			else
				if (p < pLimit)  *p++ = *str;
			break;
	}
}

static struct menuviews **menuviewp(self, obj)
struct xim *self;
struct view *obj;
{
    struct menuviews **mv;
    for(mv=(&(self->menuviewslist[MENUVIEWHASH(obj)])); *mv; mv=(&(*mv)->next)) {
	if((*mv)->view==(struct view *)obj) {
	    return mv;
	}
    }
    return NULL;
}

/* Recursively installs a menu list and all its chainees. */
static void 
InstallMenus(self, menulist)
struct xim *self;
struct menulist *menulist;
{
    register struct itemlist *item;
    struct headerlist *header;
    if (menulist == NULL) /* Should never happen, but... */
	return;
    menulist->curIM = (struct im *) self;
   
    if(self->IsOffscreenWindow) return;
   if(!menuviewp(self, (struct view *)menulist->object)) {
	int viewhash=MENUVIEWHASH(menulist->object);
	struct menuviews *new=(struct menuviews *)malloc(sizeof(struct menuviews));
	 observable_AddObserver((struct observable *)menulist->object, (struct observable *)self); 
	if(new) {
	    new->next=self->menuviewslist[viewhash];
	    self->menuviewslist[viewhash]=new;
	    new->view=(struct view *)menulist->object;
	}
    }
    
    for (header = menulist->menuChainAfter; header != NULL; header = header->next)
	InstallMenus(self, header->menulist);

    for (item = menulist->menus; item != NULL; item = item->next){
	char paneString[500];
	long panePriority;
	char selectionString[500];
	long selectionPriority;
	char *paneTitle;
	struct seldata *selectionData;

	ExplodeMenuString(item->string, 
			  paneString, sizeof(paneString), &panePriority,
			  selectionString, sizeof(selectionString), &selectionPriority);
	
	if (*paneString == '\0') {
	    paneTitle = NULL;
	    panePriority = 0;
	}
	else
	    paneTitle = paneString;

	if (item->proc != NULL) { /* Is an addition. */
	    int cmenuspanePriority,menubarpanePriority;


	    menubarpanePriority = cmenuspanePriority = self->CardOrder?GetCardPriority(self->CardOrder, paneString, panePriority):panePriority;

	    menubarpanePriority = self->MenubarCardOrder?GetCardPriority(self->MenubarCardOrder, paneString, menubarpanePriority):menubarpanePriority;

	    cmenuspanePriority = self->PopupCardOrder?GetCardPriority(self->PopupCardOrder, paneString, cmenuspanePriority):cmenuspanePriority;

	    if(menubarpanePriority>MAXPANEPRIORITY) menubarpanePriority=MAXPANEPRIORITY;
	    if(cmenuspanePriority>MAXPANEPRIORITY) cmenuspanePriority=MAXPANEPRIORITY;

	    if(menubarpanePriority==0 && paneTitle) menubarpanePriority=1;
	    if(cmenuspanePriority==0 && paneTitle) cmenuspanePriority=1;
	    selectionData = (struct seldata *)malloc(sizeof(struct seldata));
	    if (selectionData == NULL)
		return;
	    selectionData->proc = item->proc;
	    selectionData->object = menulist->object;
	    selectionData->data = item->functionData;
	    selectionData->refs=0;
	    
	    if(self->menubaron && self->menu) {
		selectionData->refs++;
		mb_AddSelection(self->menu, paneTitle, menubarpanePriority, selectionString, selectionPriority, FALSE, (char *)selectionData);
	    }

	    if(menulist_ItemIsEnabled(menulist,item) && self->cmenuson && self->cmenu) {
		if(!self->menubaron || !self->PopupMenuList || CheckMenuChoice(self->PopupMenuList, self->PopupMenuListCount, paneString)>=0) {
		    selectionData->refs++;
		    cmenu_AddSelection(self->cmenu, paneTitle, cmenuspanePriority, selectionString, selectionPriority, selectionData, cmenu_CreatePane | cmenu_DisallowDuplicates);
		}
	    }
	} else {
	    if(self->menubaron && self->menu) {
		mb_DeleteSelection( self->menu, paneTitle, selectionString);
	    }
	    /* delete unwanted menu item. */
	    if(self->cmenuson && self->cmenu) cmenu_DeleteSelection(self->cmenu, paneTitle, panePriority, selectionString, selectionPriority, cmenu_DeleteEmptyPanes);
	    continue; 
	}
	if(self->menubaron && self->menu) mb_SetItemStatus(self->menu, paneTitle, selectionString, menulist_ItemIsEnabled(menulist, item));
    }
    for (header = menulist->menuChainBefore; header != NULL; header = header->next)
	InstallMenus(self, header->menulist);
}

#define CACHEDREGIONS 10

struct cacheregion {
    struct menubar *menus;
    struct cmenu *cmenus;
    struct mlcacheNode *who;
    struct cacheregion *next, **selfP;
};

	static struct cacheregion *
lastCacheRegion(list)
	struct cacheregion *list;
{
	while(list->next!=NULL)
		list=list->next;
	return list;
}

	static void
linkCacheRegion(region, listP)
	struct cacheregion *region, **listP;
{
	if (region->selfP != NULL)
		*region->selfP = region->next;
	if (region->next!=NULL)
		region->next->selfP = region->selfP;

	region->next = *listP;
	region->selfP = listP;

	if (*listP != NULL)
		(*listP)->selfP = &region->next;
	*listP = region;
}

	static struct cacheregion *
unlinkCacheRegion(region)
	struct cacheregion *region;
{
	if (region->selfP != NULL)
		*region->selfP = region->next;
	if (region->next != NULL)
		region->next->selfP = region->selfP;

	region->selfP = NULL;
	region->next = NULL;

	return region;
}

	static void
freeCacheRegions(region)
	struct cacheregion *region;
{
	while(region != NULL) {
		struct cacheregion *next = region->next;
		free((char *)region);
		region = next;
	}
}

struct mlcacheNode {
	struct menulist *ml;
	long mask;
	int version;
	struct cacheregion *region;
	struct mlcacheNode *next, *prev, *others;
};

	static struct mlcacheNode *
newCacheNode(prev)
	struct mlcacheNode *prev;
	{
	struct mlcacheNode *cache =
	   (struct mlcacheNode *)malloc(sizeof(struct mlcacheNode)); 
	cache->next = NULL;
	cache->prev = prev;
	cache->others = NULL;
	cache->ml = NULL;
	cache->region = NULL;
	cache->mask = 0;
	cache->version =  -1;
	
	return cache;
}

static void freeMLCache(self,cache)
struct xim *self;
struct mlcacheNode *cache;
{
/* printf("fmlc: freeing a cache\n"); */
	if (cache->next != NULL)
		freeMLCache(self,cache->next);
	if (cache->others != NULL)
		freeMLCache(self, cache->others);
	if(cache->region) {
	    if(cache->region->menus) {
	    mb_Destroy(cache->region->menus);
	    cache->region->menus=NULL;
	    }
	    if(cache->region->cmenus) {
		cmenu_Destroy(cache->region->cmenus);
		cache->region->cmenus=NULL;
	    }	
	    linkCacheRegion(cache->region, &self->freeRegions);
	}
	free((char *)cache);
}

	static void 
discardCachedML(cache, rootP)
	struct mlcacheNode *cache, **rootP;
{
	struct mlcacheNode **backP;

	cache->ml = NULL;

	while (cache->prev != NULL && /* not root */
		   cache->others == NULL && cache->prev->next == cache && /* only child */
		   cache->next == NULL && cache->ml == NULL) {
		struct mlcacheNode *trash = cache;
		cache = cache->prev;
		cache->next = NULL;
		/* printf("dcml: blowing away a ml...going up.\n"); */
		free((char *)trash);
	}

	if (cache->ml != NULL || cache->next != NULL)
		return;

	if (cache->prev == NULL)
		backP = rootP;
	else
		backP = &cache->prev->next;

	while (*backP != cache)
		backP = &(*backP)->others;

	if (backP != rootP || cache->others != NULL) { 
		/* don't NULL out root */
		/* printf("dcml: blowing away an OR node%s.\n", 
			(backP == rootP) ? " (Replacing root)" : ""); */
		*backP = cache->others;
		free((char *)cache);
	}
	/* else printf("dcml: Avoiding blowing away the root.\n"); */
}

	static struct mlcacheNode *
findCachedML(self, ml, cache, newVersionP)
	struct xim *self;
	struct menulist *ml;
	struct mlcacheNode *cache;
	boolean *newVersionP;
{
	struct headerlist *h;
	struct mlcacheNode *unused = NULL;

	for (h = ml->menuChainBefore; h != NULL; h = h->next) {
		cache = findCachedML(self, h->menulist, cache, newVersionP);
		if (cache->next == NULL)
			cache->next = newCacheNode(cache);
		cache = cache->next;
	}

	while(cache->ml != ml || cache->mask != ml->selectMask) {
		if (cache->ml == NULL)
			unused = cache;
		if (cache->others != NULL)
			cache = cache->others;
		else{
			*newVersionP = TRUE;
			if (unused == NULL)
				cache = cache->others = newCacheNode(cache->prev);
			else
				cache = unused;
			cache->ml = ml;
			cache->mask = ml->selectMask;
			cache->version = ml->menuVersion;
		}
	}

	if (cache->version != ml->menuVersion ||  (struct im *) self != cache->ml->curIM)
			/* added 'cache->' above  to try 
				to get menus on the correct host -- wjh */
		*newVersionP = TRUE;

	for (h = ml->menuChainAfter; h != NULL; h = h->next) {
		if (cache->next == NULL)
			cache->next = newCacheNode(cache);
		cache = findCachedML(self, h->menulist, cache->next, newVersionP);
	}

	return cache;
}
	
void QMenuChoice(mb,idata,mdata)
struct menubar *mb;
char *idata;
char *mdata;
{
    struct xim *im=(struct xim *)mdata;
    struct seldata *sel=(struct seldata *)idata;
    enQuserMenu(im, sel->proc, sel->object, sel->data);
}

static void 
updateMenus(self, ml)
struct xim *self;
struct menulist *ml;
{
    boolean newVersion = FALSE;
    struct cacheregion *destroy = NULL;
    struct mlcacheNode *cache;
    boolean redrawflag=FALSE;

    if (ml == NULL) {
	/* It may be in the cache(??), so we don't free it. */
	self->menu = NULL;
	self->cmenu = NULL;
	return;
    }

    cache = findCachedML(self, ml, self->mlcache, &newVersion);

    if (cache->region == NULL || newVersion) {
	if (newVersion) {
	    menulist_IncrementMLVersion();
	    if (cache->region != NULL)  {
		/* printf("um: destroying menu region %d.\n",
			  cache->region->id); */
		destroy = unlinkCacheRegion(cache->region);
	    } 
	}			
	if (self->freeRegions != NULL)
	    cache->region=self->freeRegions;
	else if (self->activeRegions != NULL) {
	    cache->region = lastCacheRegion(self->activeRegions);
	    if (cache->region->who != NULL && cache->region->who != cache)
		discardCachedML(cache->region->who, &self->mlcache);
	    unlinkCacheRegion(cache->region);
	    /* this can't be most recently used region if the number
		* of regions used is >2
		*/
	    if(self->menubaron && cache->region->menus) {
		mb_Destroy(cache->region->menus);
		cache->region->menus=NULL;
	    }
	    if(self->cmenuson && cache->region->cmenus) {
		cmenu_Destroy(cache->region->cmenus);
		cache->region->cmenus=NULL;
	    }
	}
	else {
	    fprintf(stderr, "xmim(updateMenus): out of menu regions!\n");
	    return;
	}

	cache->region->who = cache;
	if(self->menubaron) self->menu = cache->region->menus 
	      = mb_Create(self->mbi, xim2programname(self), "More...", (char *)self , QMenuChoice);
	if(self->cmenuson) self->cmenu = cache->region->cmenus = cmenu_Create(xim2display(self), xim2window(self), xim2programname(self), FreeSelectionData);

	if ((self->menubaron && self->menu == NULL) || (self->cmenuson && self->cmenu == NULL))
	    fprintf(stderr, "xim: Couldn't allocate menus\n");
	InstallMenus(self, ml);
	redrawflag=TRUE;
    }
    /*
      else if (cache->region->id != self->menuRegion)
	  printf("um: reusing menu region %d.\n", cache->region->id);
      */		
    linkCacheRegion(cache->region, &self->activeRegions);

    if (destroy != NULL) {
	if(self->menubaron && destroy->menus) {
	    mb_Destroy(destroy->menus);
	    destroy->menus=NULL;
	}
	if(self->cmenuson && destroy->cmenus) {
	    cmenu_Destroy(destroy->cmenus);
	    destroy->cmenus=NULL;
	}
	linkCacheRegion(destroy, &self->freeRegions);
    }
    self->cmenu = cache->region->cmenus;

    /* don't bother redrawing this if it was the last posted menubar, note that redrawflag catches the case where this is a new menubar since in this case self->menu==cache->region->menus but this really isn't the last posted menubar */
    if(self->menu!=cache->region->menus || redrawflag) {
	self->menu = cache->region->menus;
	if(self->menubaron && self->menu) {
	    mb_RefitMenubar(self->menu);
	    mb_RedrawMenubar(self->menu, mb_Update);
	}
    }
}


void xim__PostMenus(self, menulist)
	struct xim *self;
	struct menulist *menulist;
{
	struct im *imself = (struct im *)self;

	if (xim2window(self) == NULL)
		return;
	
	/* the value of NULL for menulist is special it just says to the im re-post all your own menus */
	if(menulist) {
	    menulist_ClearChain(imself->menus);
	    menulist_ChainBeforeML(imself->menus, menulist, menulist);
	}
	
	if (imself->init != NULL)
		menulist = init_ModifyMenulist(imself->init, imself->menus);
	else
		menulist = imself->menus;

	/* if we have a popup active it's responsible for posting menus
	    and we aren't allowed to. */
	if(self->popup_active) return;
	while(self->popup_parent) self=self->popup_parent;
	    
	updateMenus(self, menulist);
}

/* xim__WhichWS()
	returns a string for the current window system:  "X" 
*/
	unsigned char *
xim__WhichWS(self)
	struct xim *self;
{
	return (unsigned char *)"X";
}

	boolean 
xim__InitializeObject(classID, self)
	struct classheader *classID;
	struct xim *self;
{
    int i;
    char *name=im_GetProgramName();

    self->MouseFacts=NULL;

    self->cmenu=NULL;
    
    self->mbi=NULL;
    self->menu = self->startupmenu = NULL;
    self->IsOffscreenWindow = FALSE;

    bzero(self->menuviewslist, sizeof(self->menuviewslist));
    
    self->mlcache=newCacheNode(NULL);
    self->freeRegions=NULL;
    for (i=0;i<CACHEDREGIONS;i++) {
	linkCacheRegion((struct cacheregion *)
			calloc(1, sizeof(struct cacheregion)),
			&self->freeRegions);  /* CORELEAK? -rr2b */
	self->freeRegions->menus=NULL;
	self->freeRegions->cmenus=NULL;
    }
    self->activeRegions=NULL;
    self->globalCursor = NULL;	/* will get on first use */
    self->globalCursorWindow = 0;  /* will get set during createwindow as needed*/
    self->StandardCursor = NULL;  /* Will be  during update */
    self->EverMapped = FALSE;	/* never been mapped */
    self->CurrentlyMapped = FALSE;
    self->popup_active = NULL;	/* No active popup initially */
    self->popup_parent = NULL;	/* We are not a popup */

    
    self->CardOrder = SetupCardOrder("CardOrder");
    self->PopupCardOrder = SetupCardOrder("PopupCardOrder");
    self->MenubarCardOrder = SetupCardOrder("MenubarCardOrder");
    
    self->PopupMenuList = SetupMenuChoices("PopupMenuList", &self->PopupMenuListCount);
    
    self->menubaron = environ_GetProfileSwitch("Menubar", TRUE);
    self->cmenuson = environ_GetProfileSwitch("PopUpMenus", TRUE);
    
    if((!self->menubaron) && (!self->cmenuson)) self->cmenuson = TRUE;

    if(self->menubaron) self->MenubarRedrawType = mb_FullRedraw;

    self->programname = (char *)malloc(strlen(name)+1);
    if(self->programname) strcpy(self->programname, name);
    
    return TRUE;
}

void xim__FinalizeObject(classID, self)
struct classheader *classID;
struct xim *self;
{
    register int i;
    struct xim *im;

    if(self->programname) free(self->programname);
  
    /* Throw away anyone who is connected in -- we're about to go away */
    if (self->header.im.topLevel)
        view_UnlinkTree(self->header.im.topLevel);
    self->header.im.topLevel = NULL;

    /* this was before the unlinktree but apparently somebody was posting menus while being unlinked! */
    if(!self->popup_parent) {
	for(i=0;i<MENUVIEWCACHESIZE;i++) {
	    if(self->menuviewslist[i]) {
		struct menuviews *mv=self->menuviewslist[i];
		while(mv) {
		    struct menuviews *next=mv->next;
		    observable_RemoveObserver((struct observable *)mv->view, self);
		    free(mv);
		    mv=next;
		}
	    }
	}
    }
    /* This should not be necessary since destroying a window
       destroys all of its subswindows as well:
    XDestroySubwindows(xim2display(self), xim2window(self));
    */
    if (self->globalCursorWindow) XDestroyWindow(xim2display(self), self->globalCursorWindow);

    xgraphic_FinalizeWindow(xim2display(self), xim2window(self));
    if (self->IsOffscreenWindow)
	XFreePixmap(xim2display(self), xim2window(self));
    else
	XDestroyWindow(xim2display(self), xim2window(self));

    if(self->startupmenu && self->startupmenu!=self->menu) mb_Destroy(self->startupmenu);
    
    if(self->mlcache) {
	freeMLCache(self, self->mlcache);
	self->mlcache=NULL;
    }
    if(self->freeRegions) freeCacheRegions(self->freeRegions);
    if(self->activeRegions) freeCacheRegions(self->activeRegions);

    if(self->mbi) mb_Finalize(self->mbi);
   
    if(self->PopupMenuList) FreeMenuChoices(self->PopupMenuList, self->PopupMenuListCount);
    if(self->CardOrder) FreeCardOrder(self->CardOrder);
    if(self->MenubarCardOrder) FreeCardOrder(self->MenubarCardOrder);
    if(self->PopupCardOrder) FreeCardOrder(self->PopupCardOrder);
    
    /* 
    Remove windows effect on windowList
    */

    for (i = 0; i < xWindowCtr; i++)  {
	if (windowList[i].Xfileno == self->Xfileno)  {
	    windowList[i].count -= 1;
	    /* 
	    If count is now zero, remove from windowList and shuffle elements down.
	    */
	    if (windowList[i].count == 0)  {
		/* THIS IS COMPLETELY WRONG!!! */
		/* im_RemoveFileHandler takes a FILE * 
		im_RemoveFileHandler(self->Xfileno);
		 */
		/* Note: it is nearly impossible to shut down the connection once open 
			since random clients and even xgraphic itself, will continue to 
			make calls to that connect to release freed storage. 
			Too bad there is no way to easily find out if a connection is still open */
		/* XCloseDisplay(windowList[i].display); */
		if (windowList[i].host) free(windowList[i].host);
		for (xWindowCtr -= 1; i < xWindowCtr; i++)
		    windowList[i] = windowList[i + 1];
	    }
	    break;
	}
    }

    /* If we are an active popup, we should remove ourself from
      the popup chain */
    if (self->popup_parent) {
	struct xim *cur, *prev;

	cur = prev = self->popup_parent;
	while ((cur = cur->popup_active) != NULL) {
	    if (cur == self) {
		break;
	    }
	    prev = cur;
	}
	if (cur == self) {
	    prev->popup_active = self->popup_active;
	    /* Tell next elt in chain to stack above prev, if prev isn't the popup_parent. */
	    if (prev != self->popup_parent) {
		XWindowChanges changes;

		changes.sibling = xim2window(prev);
		changes.stack_mode = Above;
		XConfigureWindow(xim2display(self->popup_active), xim2window(self->popup_active), CWSibling | CWStackMode, &changes);
	    }
	}

	/* find the next im up from us and tell him to post his own menus */
	xim_PostMenus(self->popup_parent, NULL);  /* the value of NULL is special it just says to the im "re-post all your own menus" */

    }

    /* If anyone has us as a parent, they must be unmapped, and
      they must not point to us as a parent */
    for	(        im = (struct xim *) imList;
		 im != NULL;
		 im = (struct xim *) im->header.im.next) {
	if (im->popup_parent == self) {
	    if (im->CurrentlyMapped) {
		/* should we call xim__Vanish? */
		XUnmapWindow(xim2display(im),xim2window(im));
		im->CurrentlyMapped = FALSE;
	    }
	    im->popup_parent = NULL;
	}
    }
}


static Region curUpdateRgn = NULL;

	void
xim__HandleRedraw (im)
	register struct xim *im;
{
	long width;
	long height;
	XRectangle RedrawBox;

	if (im->IsOffscreenWindow) {
	    view_FullUpdate(im->header.im.topLevel, view_FullRedraw, 0, 0, xim_GetLogicalWidth(im), xim_GetLogicalHeight(im));
	    return;
	}
	if(im->menubaron && im->menu) {
	    mb_RedrawMenubar(im->menu, im->MenubarRedrawType);
	    im->MenubarRedrawType=mb_FullRedraw;
	}
	im->header.im.doRedraw = FALSE;
	im->header.im.inRedraw = TRUE;
/* 
Get the size of the window. Note: this size should be good since HandleRedraw is called only from the event handler which uses XGetGeometry to set the width and height before we get to this point.
 */
	width = xim_GetLogicalWidth(im);
	height = xim_GetLogicalHeight(im);


	if (width > 0 && height > 0)  {
		/* It is actually on the screen, so display the top level view  */

		im->header.view.drawable->visualBounds 
			= im->header.view.drawable->localBounds;
		point_SetPt(&im->header.view.drawable->enclosedOrigin, 0, 0);

		/* Clear out the view specific cursors */
		/* if (im->header.im.cursorlist) xim_ClearCursorList(im); */

		/* clear out any pending window or process cursors
		   (X dependencies that is, but keep the logical structure 
		   -- it will be recaptured when the cursors are recalculated */
		if (im->globalCursorWindow) {
			/* Unmap so no one will watch us! */
			XUnmapWindow(xim2display(im), im->globalCursorWindow);
			/* Remove connections between window and cursor */
			XUndefineCursor(xim2display(im), im->globalCursorWindow);
			/* reposition window for later use */
			XResizeWindow(xim2display(im), 
				im->globalCursorWindow, width, height);
			/* toss the cursor itself */
			if (im->globalCursor) {
				im->globalCursor = NULL;
			}
		}

		if (im->header.im.topLevel != NULL)  {
			view_InsertView(im->header.im.topLevel, im, 				&im->header.view.drawable->localBounds);
			/* See if we have only a little bit to do, and if so, only update that part */
			if (curUpdateRgn) {
				XClipBox(curUpdateRgn,&RedrawBox);
				if (		(RedrawBox.x == 0) 
						&& (RedrawBox.y == 0) 
						&& (RedrawBox.width == width) 
						&& (RedrawBox.height == height))
					view_FullUpdate(im->header.im.topLevel,
						view_FullRedraw, 0, 0, width, height);
				else 
					view_FullUpdate(im->header.im.topLevel,
						view_LastPartialRedraw,
						RedrawBox.x, RedrawBox.y,
						RedrawBox.width, RedrawBox.height);
			}
			/* Nope, not drawing in response to the hardware exposing 
				a little piece, so go for a full update */
			else
				view_FullUpdate(im->header.im.topLevel, 
					view_FullRedraw, 0, 0, width, height);

		}

		/* Now make the pointer to be the cursor seen if nothing else is available. 
		   This is done by just popping the arrow cursor onto the im window */
		if ( ! im->StandardCursor) {
			struct xcursor  * tempC;
			tempC = (struct xcursor *) cursor_Create(im);
			xcursor_SetStandard(tempC,Cursor_Arrow);
			xcursor_Make(tempC, im);
			XDefineCursor(xim2display(im),xim2window(im),tempC->Xc);
			im->StandardCursor = tempC;
		}
		xim_UpdateCursors(im);
		im->header.im.inRedraw = FALSE;
	}

	if (mousedebug  &&  mf == NULL) 
		mf = fopen("/tmp/mouseout", "w");
}

	static void 
DumpPolyRect(tempRect)
	XPoint * tempRect; 
{
	printf("DumpPolyRect: using rectangle (%d,%d), (%d,%d), (%d,%d), (%d,%d)\n",
		tempRect[0].x, tempRect[0].y,tempRect[1].x, tempRect[1].y,tempRect[2].x,
		tempRect[2].y,tempRect[3].x, tempRect[3].y);
}

	static void 
GetValuesFromEvent(event,tempRect,retWidth,retHeight)
	XEvent * event;
	XPoint * tempRect;
	long *retWidth, *retHeight; 
{
	XExposeEvent * exposeEvent = (XExposeEvent *) event;
	XGraphicsExposeEvent * graphicsEvent = (XGraphicsExposeEvent *) event;

	if (regionDebug) 
		printf("GetValues: event x %d, y %d, width %d, height %d\n", 
				exposeEvent->x, exposeEvent->y, 
				exposeEvent->width, exposeEvent->height);
	if (event->type == Expose) {
		*retWidth = exposeEvent->width;
		*retHeight = exposeEvent->height;
		tempRect[0].x = tempRect[3].x = exposeEvent->x;
		tempRect[0].y = tempRect[1].y = exposeEvent->y;
		tempRect[1].x = tempRect[2].x = tempRect[0].x + exposeEvent->width;
		tempRect[2].y = tempRect[3].y = tempRect[0].y + exposeEvent->height;
	}
	else if (event->type == GraphicsExpose) {
		*retWidth = graphicsEvent->width;
		*retHeight = graphicsEvent->height;
		tempRect[0].x = tempRect[3].x = exposeEvent->x;
		tempRect[0].y = tempRect[1].y = exposeEvent->y;
		tempRect[1].x = tempRect[2].x = tempRect[0].x + exposeEvent->width;
		tempRect[2].y = tempRect[3].y = tempRect[0].y + exposeEvent->height;
	}
	else if (event->type == NoExpose) {
		if (regionDebug) printf("Getvaluesfromevent: saw no event\n");
		tempRect[0].x = tempRect[0].y 
		  = tempRect[1].x = tempRect[1].y 
		  = tempRect[2].x = tempRect[2].y 
		  = tempRect[3].x = tempRect[3].y 
		  = 0;
	}
}

static void TakeDownPopups(im)
struct xim *im;
{
    struct xim *p=im->popup_active;
    while(p) {
	XUnmapWindow(xim2display(p), xim2window(p));
	p=p->popup_active;
    }
}

static void PutUpPopups(im)
struct xim *im;
{
    struct xim *p=im->popup_active;
    while(p) {
	XMapWindow(xim2display(p), xim2window(p));
	p=p->popup_active;
	LocateWindow(p);
    }
}


static void ReConfigurePopups(im)
struct xim *im;
{
    struct xim *p=im->popup_active;
    while(p) {
	if(xim_ConfigureFunction(p)) {
	    int x,y;
	    unsigned int w,h, bw, dr;
	    Window dummy;
	    XGetGeometry(xim2display(p), xim2window(p), &dummy, &x, &y, &w, &h, &bw, &dr);
	    y-=MENUBARHEIGHT(im);
	    xim_ConfigureFunction(p)(p, xim_ConfigureRock(p), xim_ConfigureCustomRock(p), im, &x, &y, &w, &h);
	    y+=MENUBARHEIGHT(im);
	    XMoveResizeWindow(xim2display(p), xim2window(p), x, y, w, h);
	    LocateWindow(p);
	}
	p=p->popup_active;
    }
}
	void
HandleExposure(display, im, event)
	Display *display;
	struct xim *im;
	XEvent *event;
{
	XPoint tempRect[4];	/* used to hold exposure rectangle */
	XEvent tempEvent;
	Window exposedWindow;
	long lastSeenWidth, lastSeenHeight;
	Region newUpdateRgn, windowRgn;
	int x, y; unsigned int  width, height, border, depth;
	Drawable root;
	boolean sizechanged=FALSE;

	/* initialize all variables */
	exposedWindow = event->xany.window;
	lastSeenWidth = lastSeenHeight = 0;
	GetValuesFromEvent(event,tempRect, &lastSeenWidth,&lastSeenHeight);

	
	XGetGeometry(display, event->xany.window, &root, 
		     &x, &y, &width, &height, &border, &depth);
	LocateWindow(im);
	width-=point_X(&xim_GetDrawable(im)->physicalOrigin);
	height-=point_Y(&xim_GetDrawable(im)->physicalOrigin);
	if(width!=xim_GetLogicalWidth(im) || height!=xim_GetLogicalHeight(im)) {
	    rectangle_SetRectSize( &im->header.view.drawable->localBounds, 0, 0, width,  height);
	    sizechanged=TRUE;
	    if(im->popup_active) TakeDownPopups(im);
	}

	/* And start accumulating all of the areas into a region */
	if (regionDebug) {
		printf("\nxim:creating region for original event\n"); 
		DumpPolyRect(tempRect);
	}
	curUpdateRgn = XPolygonRegion(tempRect,4,EvenOddRule);
	if (regionDebug) printf("xim:got original region\n");

	/* Let's get all exposure events pending for this window. 
	   This is a bit inefficient since we really ought to accumulate 
	   the areas to be redrawn and then selectively do the redrawing, 
	   but 
		since everyone redraws everything anyway, 
		{{an invalid assumption  --wjh}}
	   we might as well go for it */
	XSync(display,  /* don't throw away:*/ 0);
	while(XCheckWindowEvent(display, exposedWindow, 
			ExposureMask, &tempEvent)) {
		/* Note: we could get graphics exposure events, 
			noevents and exposure events */
		event = &tempEvent;
		GetValuesFromEvent(event, tempRect,
				 &lastSeenWidth, &lastSeenHeight);
		if (event->type != NoExpose) {
			if (regionDebug) 
				printf("xim:creating region for coalesed event\n");
			newUpdateRgn = XPolygonRegion(tempRect, 4,
					EvenOddRule);
			if (regionDebug) 
				printf("xim:unioning two regions into curUpdate\n");
			XUnionRegion(curUpdateRgn,newUpdateRgn,curUpdateRgn);
			if (regionDebug) printf("xim:destroying new updatergn\n");
			XDestroyRegion(newUpdateRgn);
		}
	}
	/* We have all of the events and we've accumulated the update regions in the region. Get the region and see if we are drawing whole window.
	    Yes=> punt the region clipping and go for it, 
	    No => save the special region and let graphic know about it */
	tempRect[0].x = tempRect[0].y = tempRect[1].y = tempRect[3].x = 0;
	tempRect[1].x = tempRect[2].x = width;
	tempRect[2].y = tempRect[3].y = height;
	if (regionDebug) printf("xim:about to create window region\n");
	windowRgn = XPolygonRegion(tempRect,4,EvenOddRule);

	if (regionDebug) {
		XRectangle fakeRect;
		printf("xim:about to test for region equality\n");
		fakeRect.x = fakeRect.y = 0;
		fakeRect.width = fakeRect.height = 32000;
		XClipBox(curUpdateRgn,&fakeRect);
		printf("xim: clipbox of update region is %d, %d, %d, %d\n", 
			fakeRect.x, fakeRect.y, fakeRect.width, fakeRect.height);
	}
	if (XEqualRegion(windowRgn, curUpdateRgn) 
			||  (updatelist_UpdatesPending(globalUpdateList))) {
		/* The cumulative update is the same as the window, 
		   so call it a fullupdate rather than a partial, 
		   or there are partial update requests generated 
		   from within the application for which we do not know 
		   the enclosing rectangle, and hence cannot include it 
		   in the clipping region */
		if (regionDebug)  
			printf("xim: destroy curupdate region equal to window\n");
		XDestroyRegion(curUpdateRgn);
		curUpdateRgn = NULL;
	}
	if(windowRgn) XDestroyRegion(windowRgn);
	if (curUpdateRgn) {
		if (regionDebug) printf("xim: installing update region\n");
		xgraphic_SetUpdateRegion(curUpdateRgn, display, event->xany.window);
	}
	if(im->menubaron) im->MenubarRedrawType=mb_Exposed;
	xim_HandleRedraw(im);
	if (curUpdateRgn) { 
		if (regionDebug) printf("xim: reseting update region to 0\n");
		xgraphic_SetUpdateRegion(0L,display, event->xany.window);
		if (regionDebug) 
			printf("xim:destroy update rgn after Handleredraw\n");
	
		XDestroyRegion(curUpdateRgn);
		curUpdateRgn = NULL;
	}
	if(sizechanged) {
	    ReConfigurePopups(im);
	    PutUpPopups(im);
	}
}



/* timer queuing for mouse buttons */

	static void
ButtonTimerFire(mfacts, now)
	struct mouseStatus *mfacts;
	long now;
{
	enum mouseState state = mfacts->state;

	if (now < 0) {
		/* this routine has been called while the event is pending.  cancel it */
		if (mfacts->event != NULL) 
			event_Cancel(mfacts->event);
	}
	if (mf) {
		fprintf(mf, "TFire   @(%d,%d)   (%d)  now %d \n", 
			mfacts->xPending, mfacts->yPending, 
			(long)mfacts->state, now);
		fflush(mf);
	}
	mfacts->event = NULL;


	/* send the button */
	switch (state) {
	case msLeftDownPending:
		mfacts->state = msLeftDown;
		enQuserMouse(mfacts->xim, view_LeftDown, 
			mfacts->xPending, mfacts->yPending, 
			im_LeftDown);
		break;
	case msRightDownPending:
		mfacts->state = msRightDown;
		enQuserMouse(mfacts->xim, view_RightDown, 
			mfacts->xPending, mfacts->yPending, 
			im_RightDown);
		break;
	}
}

/* start timer to fire in (MouseHysteresis) milliseconds 
*/
	static void
StartButtonTimeout(xim, button, x, y)
	struct xim *xim;
	unsigned int button;
	long x, y;
{
	struct mouseStatus *mfacts = xim->MouseFacts;
	mfacts->xPending = x;
	mfacts->yPending = y;
	mfacts->xim = xim;
	mfacts->state = (button == LEFTBUTTON) 
			? msLeftDownPending 
			: msRightDownPending;
	mfacts->event = im_EnqueueEvent(ButtonTimerFire, mfacts,
			event_MSECtoTU(MouseHysteresis));
}

	static void
CancelButtonTimeout(mfacts)
	struct mouseStatus *mfacts;
{
	if (mfacts->event != NULL) 
		event_Cancel(mfacts->event);
	mfacts->event = NULL;
	mfacts->state = msAllUp;
}


/* requirement: state must be msLeftDown or msRightDown 
*/
	static void
SendButtonUp(mfacts, x, y)
	struct mouseStatus *mfacts;
	long x, y;
{
	switch (mfacts->state) {
	case msLeftDown:
		enQuserMouse(mfacts->xim, view_LeftUp, x, y, im_AllUp);
		break;
	case msRightDown:
		enQuserMouse(mfacts->xim, view_RightUp, x, y, im_AllUp);
		break;
	}
	mfacts->state = msAllUp;
}

struct view *
xim__Hit (self, action, x, y, clicks)
struct xim *self;
enum view_MouseAction action;
long x, y, clicks;
{
    if (self->popup_active)
	return (struct view *)self;
    else
	return view_Hit(self->header.im.topLevel, action, x, y, clicks);
}
	
static void HandleExposeFromMenubar(ee,im)
XEvent *ee;
struct xim *im;
{

    im->CurrentlyMapped = TRUE;
    HandleExposure(xim2display(im), im, ee);
}


static void
HandleWindowEvent(display)
Display *display;
{
#define KEYEXPANSIONSIZE 80
	XEvent tempEvent, event;
	XButtonEvent *buttonEvent;
	XMotionEvent *motionEvent;
	XMappingEvent * mappingEvent;
	XKeyEvent *keyEvent;
	struct xim *im;
	struct mouseStatus *mfacts;
	long mouseX = 0, mouseY = 0;
	char str[KEYEXPANSIONSIZE];
	char * strp = str;
	long length;
        struct masklist *ml;

do {	/* while (XPending(display)) */

	/* get the next event*/
	XNextEvent(display, &event);
	lastEventTime = CurrentTime;   /* in case there's no time in this particular event */
	event.type &= 0177; 	/* Mask out high order bit, which distinguishes 
				between server generated events
				 and client generated (simulated) events 
				-- they should look the same to us */

	/* If it's a mapping event, it doesn't matter if it's our window or not, 
		we still need to respond */
	if (event.type == MappingNotify) {
		mappingEvent = (XMappingEvent *) &event;
		if ((mappingEvent->request == MappingModifier) 
			|| (mappingEvent->request == MappingKeyboard) )
		XRefreshKeyboardMapping(mappingEvent);
	}

	/* find the im for the window */
	im = (struct xim *) imList;

	while(im) {
	    if(((xim2window(im) == event.xany.window) && xim2display(im) == display )) break;
	    im=(struct xim *) im->header.im.next;
	}
	if (im == NULL) {
		/* check to see if it is a property notify for 
			one of the windows on the MaskList. */
		if (event.type != PropertyNotify) continue;
		for (ml = MaskList; ml != NULL;  ml = ml->next)
			if (ml->window == event.xany.window) {
				lastEventTime = ((XPropertyEvent *)&event)->time;
				HandleProperty(&event);
				break;
			}
		continue;
        }
	mfacts = im->MouseFacts;


	/* if there is a function to perform before processing the next interaction, do it */
	if (		event.type != GraphicsExpose 
			&& event.type != Expose 
			&& im->header.im.pendingInteractionEvents != NULL)
	    xim_DispatchPendingInteractionEvents(im);

	switch (event.type)  {
		XConfigureEvent *confEvent;
	    case ReparentNotify:
	    case GravityNotify:
	    case MapNotify:
		LocateWindow(im);
		break;
	    case ConfigureNotify:
		confEvent = (XConfigureEvent *) &event;
		if(im->menubaron) {
		    mb_HandleConfigure(im->mbi, im->menu, confEvent->width, confEvent->height);
		}
		LocateWindow(im);
		break;
	/* StructureNotifyMask had to be reinstated to get UnmapNotifyEvents so we 
		can tell when a window is not visible.  Thus im_ExposeWindow
		can only do something when it has to.  
		We ignore all StructureNotify events other than UnmapNotify.  -wjh */

	case UnmapNotify:
		im->CurrentlyMapped = FALSE;
		break;

	case Expose:
	case GraphicsExpose:
		im->CurrentlyMapped = TRUE;	
		HandleExposure(display, im, &event);
		break;

	case SelectionClear:
		/* this process/window no longer has the selection. */
		lastEventTime = ((XSelectionClearEvent *)&event)->time;
		if (IOwnSelection && ((XSelectionClearEvent *)&event)->selection == XA_PRIMARY) {
			IOwnSelection = FALSE;
			CBCacheValid = FALSE;
		}
		break;
	case SelectionRequest:
		/* somebody wants CUTBUFFER0.  We'll do what we can. */
		lastEventTime = ((XSelectionRequestEvent *)&event)->time;
		RespondToSelectionRequest(&event);
		break;
	case SelectionNotify:
		/* our request for a selection has been granted */
		lastEventTime = ((XSelectionEvent *)&event)->time;
		HandleSelectionNotify(&event);
		break;
	case PropertyNotify:
		/* a property somewhere of interest has changed */
		lastEventTime = ((XPropertyEvent *)&event)->time;
		HandleProperty(&event);
		LocateWindow(im);
		break;

	case KeyPress:
		/* If we have a popup, IT gets the key event */
		while (im->popup_active) 
		    im = im->popup_active;

	        keyEvent = (XKeyEvent *) &event;
		lastEventTime = keyEvent->time;
		strp = str;
		length = XLookupString(keyEvent, str, KEYEXPANSIONSIZE, 
				       /* keysym*/ NULL, /* status = XComposeStatus */ NULL);
		if (length == 1 && keyEvent->state & Mod1Mask)
		    enQuserKey(im, IM_METAESC); /* changed from '\033'--bobg */
		while (length-- > 0)
		    enQuserKey(im, *strp++);
	        break;

	case ButtonPress:
		buttonEvent = (XButtonEvent *) &event;
		lastEventTime = buttonEvent->time;
		if(im->menubaron && buttonEvent->y<MENUBARHEIGHT(im)) {
		    mb_Activate(im->menu, buttonEvent->x, buttonEvent->y);
		    continue; /* goto while(XPending) */
		}
		mouseX = buttonEvent->x - point_X(&xim_GetDrawable(im)->physicalOrigin);
		mouseY = buttonEvent->y - point_Y(&xim_GetDrawable(im)->physicalOrigin);

		if (mf) {
			fprintf(mf, "Press  %d @(%d,%d)   [%d]   %d  (%d)\n", 
				buttonEvent->button,
				buttonEvent->x, buttonEvent->y, 
				buttonEvent->time,
				buttonEvent->state,  (long)mfacts->state);
			fflush(mf);
		}

		/* The X server appears to botch it for the "middle" button,
		we may get first a left or right down and then a middle down event.

		To deal with this, we start a timer at the down and only send 
		the button to the client after the timer fires.  
		The timer, if one is active, is pointed at by mfacts->event.
		the mfacts structure keeps track of the mouse state for one display.

		The action for a single button down depends on the former state:
			none down - enqueue timer event
			button pending - cancel the timer 
				and continue as for case of none down
			some button already down - invent an up for it 
					and start timer for the new button
		The action for the middle button down is to cancel the timer, if any,
		and call cmenu.  If the button state shows a button was down, 
		ignore the menu operation.

		XXX    HYPERKLUDGE     XXX
		We seem to occasionally get multiple mouse clicks from the X server
		for a single press.  Experience shows that they have the same timestamp,
		so we discard the second event with the same time.

		*/

		if (mfacts->lasttime == buttonEvent->time) {
			/* ignore this new button press.  It repeats the old.  ????  XXX  */
			continue;		/* goto while(XPending()) */
		}
		else mfacts->lasttime = buttonEvent->time;

		switch (mfacts->state) {
		case msLeftDownPending:
		case msRightDownPending:
			CancelButtonTimeout(mfacts);
			break;
		case msAllUp:
			break;
		default:
			/* we have started some mouse button.  
				Invent its up. */
			SendButtonUp(mfacts, mouseX, mouseY);
			if (buttonEvent->button == MENUBUTTON) {
				/* we've already told the client a button is
					down, so we ignore the menu events.
				   This prevents the problem that the mouse up
				   would cause the selection to change just
				   as the selection is obscured by the menu */
				continue;		/* goto while(XPending()) */
			}
			break;
		}
		switch (buttonEvent->button) {
		case MENUBUTTON:
			/* New clever hack: if we are an active poup ask our uppermost parent to activate our menus */
		    while(im->popup_parent) im=im->popup_parent;
		    
		    if (im->cmenuson) {
			if(im->cmenu) {
			    int retVal;
			    struct seldata *sel;
			    retVal = cmenu_Activate(im->cmenu, buttonEvent, &sel, cmenu_BackgroundPixel, im->backgroundPixel);
			    if (retVal == cm_SUCCESS) enQuserMenu(im, sel->proc, sel->object, sel->data);

			} else errprintf(xim2programname(im), ERR_WARNING, 0, 0, "no menus defined. %s", "Maybe some fonts are missing.");
		    }
		    break;
		case LEFTBUTTON:
		case RIGHTBUTTON:
			StartButtonTimeout(im, buttonEvent->button, 
				mouseX, mouseY);
			break;
		}
		break;
		
	/* alledgedly the buttonstate is not reported accurately  for motion events.  
		We just use whatever state we started with */

	case MotionNotify:
		motionEvent = (XMotionEvent *) &event;
		lastEventTime = motionEvent->time;

		if (mf) {
			fprintf(mf, "Move  @(%d,%d)   [%d]   %d \n", 
				motionEvent->x, motionEvent->y, 
				motionEvent->time,
				motionEvent->state);
			fflush(mf);
		}
		if (mfacts->state == msLeftDownPending
				||  mfacts->state == msRightDownPending) {
			/* probably movement for the pending down.
			   fire the PendingButtonEvent */
			ButtonTimerFire(mfacts, -1);
		}

		/* Keep pulling off all motion notify events 
		    until something else appears in the queue or until nothing else is present 
		    Don't discard events, just collect as many events as are outstanding
		    (we need some way to slow it down. sigh, what a notion) */
		XSync(xim2display(im), 0); 
		while(XPending(xim2display(im))) {
			XNextEvent(xim2display(im), &tempEvent);
			if (tempEvent.type == MotionNotify) {

				if (mf) {
					fprintf(mf, "Scan  @(%d,%d)  [%d]    %d   \n", 
						motionEvent->x, motionEvent->y, 
						motionEvent->time,
						motionEvent->state);
					fflush(mf);
				}

				/* Use the later motion event 
				   -- one can argue if we should check for same  
				   button state as before as well but since we are  
				   not getting button state back from server, 
				   it is fruitless now to argue about it 
				   and impossible to check */
				event = tempEvent;
			}
			else {
				/* Not another motion event, put it back and use last event */
				XPutBackEvent(xim2display(im), &tempEvent);
				break;
			}
		}
		mouseX = motionEvent->x;
		mouseY = motionEvent->y;
		switch (mfacts->state)  {
		case msLeftDown:
			enQuserMouse(mfacts->xim, view_LeftMovement, 
				mouseX, mouseY, im_LeftDown);
			break;
		case msRightDown:
			enQuserMouse(mfacts->xim, view_RightMovement, 
				mouseX, mouseY, im_RightDown);
			break;
		}
		break;

	/* the button release code ensures the invariant that every down 
	is followed by an up.  If a button is down, an up operation for any
	button is sent to the client as an up for the button which is down.

	If no button is down, an up event is ignored.
	If a button-down is pending, it is forced to occur. 
	*/

	case ButtonRelease:
		buttonEvent = (XButtonEvent *)&event;
		lastEventTime = buttonEvent->time;
		mouseX = buttonEvent->x;
		mouseY = buttonEvent->y;

		if (mf) {
			fprintf(mf, "LetUp  %d @(%d,%d)   [%d]   %d  (%d)\n", 
				buttonEvent->button,
				buttonEvent->x, buttonEvent->y, 
				buttonEvent->time,
				buttonEvent->state, (long)mfacts->state);
			fflush(mf);
		}

		switch (mfacts->state) {
		case msLeftDownPending:
		case msRightDownPending:
			/* probably the up for the pending down.
			   fire the PendingButtonEvent */
			ButtonTimerFire(im->MouseFacts, -1);
			/* DROP THRU */
		case msLeftDown:
		case msRightDown:
			SendButtonUp(mfacts, mouseX, mouseY);
			break;
		case msAllUp:
			/* up without down.  Maybe leftover from menu.  Ignore */
			break;
		}
		break;
	}
} while (XPending(display));
}


/* when HandleFiles is called with beCheap true, it shouldn't do a select, but only
    check to see if any data is already in this process (via stdio's previous read).
    This should cut down on the number of selects per read done by this thing.
    
    Function returns true if it invoked any handlers, false otherwise.
*/
	static boolean 
xim__HandleFiles (ClassID, twait, beCheap)
	struct classheader *ClassID;
	long twait; 
	boolean beCheap;
{
    int nfs;
    long rmask = 0;
    long wmask = 0;
    boolean ret = FALSE;
    FILE *f;
    int i;


    for (i = 0; i < xWindowCtr; i++)  {
	if (XPending(windowList[i].display))  {
	    ret = TRUE;
	    HandleWindowEvent(windowList[i].display);
	}
	else
	    rmask |= 1 << windowList[i].Xfileno;
    }

    nfs = NFILEHandlers;
    while (--nfs>=0)  {
	f = globalFILEHandlers[nfs].file;
	if (f != NULL)  {
	    if (f->_cnt > 0)  {
		ret = TRUE;
		(globalFILEHandlers[nfs].proc)(f, globalFILEHandlers[nfs].procdata);
	    }
	    else
		rmask |= 1 << fileno(f);
	}
    }
    if (ret)
	return TRUE;
    else if (beCheap)
	return FALSE;

    if (NCanOutHandlers) {
	nfs = NCanOutHandlers;
	while (--nfs>=0)  {
	    f = CanOutHandlers[nfs].file;
	    if (f != NULL)
		wmask |= 1 << fileno(f);
	}
    }

    /* do the select to wait */

    if (twait < event_ENDOFTIME) {
	if ((PollTime.tv_sec = event_TUtoSEC(twait)))
		twait -= event_SECtoTU(PollTime.tv_sec);
	PollTime.tv_usec = event_TUtoUSEC(twait);
    }
    else {
	/* hack from Nichols: don't use 0 here, we have to be able to zap it from signal proc */

	PollTime.tv_sec = 99999999;
	PollTime.tv_usec = 0;
    }

    if (anyDelivered)
        im_DeliverSignals();		/* avoid non-race case */

#ifdef LWP
    im_LWPCurrentProcess(&imPid);
    ret = im_IOMGRSelect(32, &rmask, &wmask, 0, &PollTime);
#else /* LWP */
    ret = select(32, &rmask, &wmask, 0, &PollTime);
#endif /* LWP */
    /* deliver the signals */
    if (anyDelivered) im_DeliverSignals();

    if (ret > 0)  {
	if (rmask)  {


	    for (i = 0; i < xWindowCtr; i++)  {
		if ((rmask & (1 << windowList[i].Xfileno)) != 0)  {
		    HandleWindowEvent(windowList[i].display);
		}
	    }


	    nfs = NFILEHandlers;
	    while (--nfs>=0)  {
		f = globalFILEHandlers[nfs].file;
		if (f != NULL && (rmask & (1 << fileno(f))) != 0)
		    (globalFILEHandlers[nfs].proc)(f, globalFILEHandlers[nfs].procdata);
	    }
	}
	if (wmask)  {
	    nfs = NCanOutHandlers;
	    while (--nfs>=0)  {
		f = CanOutHandlers[nfs].file;
		if (f != NULL && (wmask & (1 << fileno(f))) != 0)
		    (CanOutHandlers[nfs].proc)(f, CanOutHandlers[nfs].procdata);
	    }
	}
	return TRUE;
    }
    else
	return FALSE;
}

	void
xim__RedrawWindow(self)
	struct xim *self;
{
	XWindowAttributes attr;

	if(self->IsOffscreenWindow) {
 	    xim_HandleRedraw(self);
	    return;
	}
	XGetWindowAttributes(xim2display(self), xim2window(self), &attr);
	if (attr.map_state == IsUnmapped)  {
		/*  If the window is not mapped, we have to map it 
		    and then wait for the exposure event.
		   The HandleExposure routine will call xim_HandleRedraw() */
		XEvent Event;
		XEvent exposeEvent;
		boolean seenstructure = FALSE;
		boolean seenexpose=FALSE;

		XMapWindow(xim2display(self), xim2window(self));
		LocateWindow(self);
		if ( !self->EverMapped && !self->popup_parent) {
		    self->EverMapped = TRUE;
		    /* the very first time we map the window we handle
		     exposure events immediately.  This lets `messages`
		     display the message of the day while it is initializing it data base. 
			 */
		    /* however, to keep the location cache correct, we must have both
		     a ConfigureNotify and an Expose event before continuing */
		    while(!(seenexpose&&seenstructure)) {
			XWindowEvent(xim2display(self), xim2window(self), 
				     ExposureMask|StructureNotifyMask, &Event);
			LocateWindow(self);
			if (Event.type == ConfigureNotify) {
			    seenstructure = TRUE;
			}
			if (Event.type == Expose) {
			    exposeEvent = Event;
			    seenexpose=TRUE;
			}
		    }
		    if(self->menubaron && self->menu) mb_RefitMenubar(self->menu);
		    HandleExposure(xim2display(self), self, &exposeEvent);
		    self->CurrentlyMapped = TRUE;
		} else {
		    /* catch the case of a override window being mapped the
			first time */
		    self->EverMapped = TRUE;
			/* exposure events will eventually arrive and be handled in
				Interact / HandleFiles / HandleWindowEvent  
			 */
		}
	}
	else  {
		/* We are already mapped, so let's draw */
		XClearWindow(xim2display(self), xim2window(self));
		xim_HandleRedraw(self);
	}

}

static void xim_ActivateMenubar(xim, rock)
struct xim *xim;
long rock;
{
    if(xim->menubaron) mb_KeyboardActivate(xim->menu);
}

static char *mygetdefaults(dpy, pname)
Display *dpy;
char *pname;
{
    return environ_GetProfile(pname);
}

static long *CalculateIncrementList(str, finalIncr, listCount)
char *str;
long finalIncr;
long *listCount;
{
    long incrList[1000];
    long count = 0;
    long neg = 1;
    long val = 0;
    boolean inVal = FALSE;
    char *ptr;
    long *list;

    if (str) {
	do {
	    if (*str >= '0' && *str <= '9') {
		val = val * 10 + (*str - '0');
		inVal = TRUE;
	    }
	    else {
		if (inVal) {
		    inVal = FALSE;
		    incrList[count++] = neg * val;
		    neg = 1;
		    val = 0;
		}
		if (*str == '-'){
		    neg = -neg;
		}
	    }
	} while (*str++ != '\0');

    }

    incrList[count++] = finalIncr;
    *listCount = count;
    list = (long *) malloc(sizeof(long) * count);
    bcopy(incrList, list, sizeof(long) * count);
    return list;
}

    boolean 
xim__InitializeClass(classID)
    struct classheader *classID;
{
    char *str;
    long LeftIncrement;
    long TopIncrement;

    gData = im_GetGlobalData(); /* load up the pointer to shared, global state */
    useBackingStore = environ_GetProfileSwitch("UseBackingStore", TRUE);
    MouseHysteresis = environ_GetProfileInt("MouseHysteresis", 50);	/* milliseconds */
    if (MouseHysteresis < 1) MouseHysteresis = 1;
    if (MouseHysteresis > 200) MouseHysteresis = 200;
    LeftIncrement = environ_GetProfileInt("LeftIncrement", 10);	/* pixels */
    TopIncrement = environ_GetProfileInt("TopIncrement", 10);	/* pixels */
    str = environ_GetProfile("LeftIncrementList");
    LeftIncrList = CalculateIncrementList(str, LeftIncrement, &maxLeftCount);
    str = environ_GetProfile("TopIncrementList");
    TopIncrList = CalculateIncrementList(str, TopIncrement, &maxTopCount);
    leftIncrCount = 0;
    topIncrCount = 0;
    CurrLeftIncr = CurrTopIncr = 0;
    SelectionAndCB = environ_GetProfileSwitch("SelectionAndCB", TRUE);
    CachedCutBuffer.string = malloc(CachedCutBuffer.size = 200);
    CachedCutBuffer.length = CachedCutBuffer.pos = 0;

    iconMaxLength = environ_GetProfileInt ("MaxIconLabelLength", iconMaxLength);
    titleMaxLength = environ_GetProfileInt ("MaxTitleLength", titleMaxLength);
    useProgramNameInTitle = environ_GetProfileSwitch("UseProgramNameInTitle", useProgramNameInTitle);

    (void)mb_SetGetDefault(mygetdefaults);

    (void)proctable_DefineProc("xim-activate-menubar", xim_ActivateMenubar, &xim_classinfo, "xim", "Activates the menubar in preparation for key traversal with the arrow keys.");

    return TRUE;
}


/*     The following supplies cursor support.   */


void xim__ClearCursors(self, C)
	struct xim *self;
	struct xcursor *C; 
{
    if (im_IsPlaying()) return;

    if (cursordebug) 
	printf("xim_ClearCursors: clearing cursor %X using window %X and X cursor %X\n", 
			C, C->Xw, C->Xc);
    if (C->Xw)
	XDestroyWindow(C->Xd,C->Xw);
    C->Xw = 0;
}

void xim__PostCursor(self,rec,reqCursor)
struct xim *self;
struct rectangle *rec;
struct xcursor *reqCursor;
{
    struct rectangle vrec;
    struct cursor * plainCur = (struct cursor *) reqCursor;
    struct im * imself = (struct im *) self;
    long initialWidth, initialHeight, initialX, initialY;
    struct rectangle initRect;
    int doMap = TRUE; /* nonzero window should be mapped */
    XWindowChanges changes;

    if (self->IsOffscreenWindow) return;
    if (im_IsPlaying()) return;

    /* Why aren't these flagged as errors instead of just returning? */
    if (plainCur == NULL || plainCur->view == NULL  ) return;
    if (plainCur->posted != imself && plainCur->posted != NULL)
	 /* Cursor already posted to another im  */
	    return;

    /* We have an unused cursor, so let's post it */
    if (cursordebug) 
	printf("xim_PostCursor: about to add new cursor on top %X (%c) for view %X\n",
			reqCursor, plainCur->fillChar, plainCur->view);

    /* First. make sure we have a window for the cursor to appear in */
    if ( reqCursor->Xw == 0) {
	view_GetVisualBounds(plainCur->view,&initRect);
	physical_LogicalToGlobalRect(view_GetDrawable(plainCur->view), &initRect);
	initialWidth = rectangle_Width(&initRect);
	initialHeight = rectangle_Height(&initRect);
	initialX = rectangle_Left(&initRect);
	initialY = rectangle_Top(&initRect);
	/* Let's make sure we still have a legal (non-0 window) */
	if (initialWidth <= 0 || initialHeight <= 0) {
	    /* Oh well, view is gone for some reason, so make the window but don't map it */
	    initialWidth = initialHeight = 1; /* will have to be resized anyway */
	    doMap = FALSE;
	}
	reqCursor->Xw = XCreateWindow( xim2display(self),xim2window(self), /*x*/initialX,/*y*/initialY,/*width*/initialWidth,/*height*/initialHeight, /*border width*/0,/*depth*/0, /*class*/InputOnly, /*visual*/ DefaultVisual(xim2display(self),xim2screen(self)), /* valuemask*/ 0,NULL);
	reqCursor->Xd = xim2display(self);
	if (cursordebug) printf("xim_postcursor: creating window %X for cursor %X at left %d, top %d, width %d, height %d\n", reqCursor->Xw, reqCursor, initialX, initialY, initialWidth, initialHeight);
    }

    /* Remove any cursors using the current input-only window */
    XUndefineCursor(xim2display(self),reqCursor->Xw);

    /* Convert the requested cursor into a X specific cursor */
    xcursor_Make(reqCursor, self);

    /* Associate the cursor with the window */
    XDefineCursor(reqCursor->Xd,reqCursor->Xw,reqCursor->Xc);

    /* Resize the input only window as appropriate */
    if (rec) {
	rectangle_IntersectRect(&vrec, rec, &(plainCur->view->drawable->visualBounds));
	physical_LogicalToGlobalRect(view_GetDrawable(plainCur->view), &vrec);
	/* This is a hack because of no 0-sized windows -- something better should be done */
	if (vrec.height>0 && vrec.width>0) {
	    if (cursordebug) printf("xim_PostCursor: moving window %X for cursor %X to left %d, top %d, width %d, height %d\n", reqCursor->Xw, reqCursor, vrec.left, vrec.top, vrec.width, vrec.height);
	    XMoveResizeWindow(xim2display(self), reqCursor->Xw, vrec.left, vrec.top, vrec.width, vrec.height);
	}
	else doMap = FALSE; /* we have an empty cursor area, so don't map the window */
    }
    /* Now place cursor in proper stacking order -- if never seen before, then it is already in proper position (first if test); if it is a new cursor, then it goes on top of the stack (second set of statements); however, global (window/process) cursors have highest priority, so if just raised the view cursor, then we must reraise the global cursor (last test) */

    /* If we have a popup, we stack the cursor windows below it. */
    if (plainCur->posted != imself) {
	if (cursordebug) printf("xim_PostCursor: raising cursor window %X for cursor %X\n", reqCursor->Xw, reqCursor);
	if (self->popup_active) {
	    changes.sibling = xim2window(self->popup_active);
	    changes.stack_mode = Below;
	    XConfigureWindow(xim2display(self), reqCursor->Xw, (CWSibling | CWStackMode), &changes);
	} else 
	    XRaiseWindow(xim2display(self), reqCursor->Xw);
         if (ProcessCursor  || imself->WindowCursor) {
	     if (self->popup_active) {
		 changes.sibling = xim2window(self->popup_active);
		 changes.stack_mode = Below;
		 XConfigureWindow(xim2display(self), self->globalCursorWindow, (CWSibling | CWStackMode), &changes);
	     } else 
		 XRaiseWindow(xim2display(self), self->globalCursorWindow);
	     if (cursordebug) printf("xim_PostCursor: global cursors being used, so raising global cursors over %X(window %X)\n", reqCursor, reqCursor->Xw);
	 }
    }
    else if (cursordebug) printf("xim_PostCursor: cursor window %X for cursor %X already in order, not raising\n", reqCursor->Xw, reqCursor);

    /* If everything is OK, map the window, but if the window region disappeared, remove the window and let other cursors show through */
    if (doMap) {
	if (cursordebug) printf("xim_PostCursor: mapping cursor window %X for cursor %X\n",reqCursor->Xw, reqCursor);
	XMapWindow(xim2display(self), reqCursor->Xw);
    }
    else {
	if (cursordebug) printf("xim_PostCursor: unmapping cursor window %X for cursor %X\n",reqCursor->Xw, reqCursor);
	XUnmapWindow(xim2display(self), reqCursor->Xw);
    }

    /* All done, link in the cursor for later use */
    if (plainCur->posted != imself) {
        plainCur->next = imself->cursorlist;
        imself->cursorlist = plainCur;
    }
    plainCur->posted = imself;
    plainCur->changed = FALSE;

    /* Make sure the server sees all of these requests *now* */
    XFlush(xim2display(self));
}

void xim__UpdateCursors(self)
struct xim *self;
{
    if (im_IsPlaying()) return;

    updateGlobalCursors(self);

    XFlush(xim2display(self));
}

static updateGlobalCursors(self)
struct xim *self;
{
    struct xcursor * XProcessCursor = (struct xcursor *) ProcessCursor;
    struct xcursor * XWindowCursor = (struct xcursor *) self->header.im.WindowCursor;
    struct xcursor * tmp = 0;
    Display *DummyDisplay = xim2display(self); /* used to hold useless return value */

    if (self->IsOffscreenWindow) return NULL;
    /* If we had a global cursor, toss it */
    if (self->globalCursor) {
	XUndefineCursor(xim2display(self),self->globalCursorWindow);
	self->globalCursor = NULL;
    }

    /* If we have a global cursor, place it in an appropriate window */
    if (XProcessCursor || XWindowCursor) {
	/* First, make sure we have a global window */
	if (!self->globalCursorWindow) {
	    register long curGWindowWidth;
	    register long curGWindowHeight;
	    if (cursordebug) printf("(xim)updateGlobalCursors: make global window for global cursor\n");
	    curGWindowWidth = xim_GetLogicalWidth(self);
	    curGWindowHeight = xim_GetLogicalHeight(self);
	    /* Make the window */

	    if (curGWindowWidth <= 0) {
		curGWindowWidth = 1;
	    }
	    if (curGWindowHeight <= 0) {
		curGWindowHeight= 1;
	    }
	    self->globalCursorWindow = XCreateWindow( xim2display(self),xim2window(self), /*x*/0,/*y*/0,/*width*/curGWindowWidth,/*height*/curGWindowHeight, /*border width*/0,/*depth*/0, /*class*/InputOnly, /*visual*/ DefaultVisual(xim2display(self),xim2screen(self)), /* valuemask*/ 0,NULL);
	}

	/* Convert BE2 cursor into X cursor */
	if (XProcessCursor) tmp=XProcessCursor;
	   else tmp=XWindowCursor;
	xcursor_Make(tmp, self);
	self->globalCursor = tmp;

	/* Define, raise, map, save */
        /*self->globalXCursor = tmp->Xc;*/
	/* Was this person trying to be clever and use this cursor as both a view cursors and a global cursor. If so, eliminate its ability to be a view cursor. Too bad the cursor functionality was never precisely defined. Let's leave a warning in there for now so that many people will complain and we can get a consensus on what to do */
	if (cursordebug)
	    if (tmp->header.cursor.posted) printf("updateGlobalCursors(xim): posting an active view cursor as a global cursor\n");
/* Note: we used to execute this code to remove any possibility of reusing a view cursor as a global cursor. Since we now only copy that information out of the cursor structure, we will leave the rest of the cursor information unchanged. (the things we do to keep old code working) */
/*
	if (tmp->Xw) {
	    if (tmp->Xd) XDestroyWindow(tmp->Xd,tmp->Xw);
	    else printf("updateGlobalCursors(xim): internal error - cursor window but no display\n");
	    tmp->Xw = 0;
	}
        tmp->Xc = 0;
        tmp->Xd = 0;
*/
        XDefineCursor(xim2display(self), self->globalCursorWindow, self->globalCursor->Xc);
        if (cursordebug) printf("(xim)updateGlobalCursor: raising global window %X for global cursor %X\n", self->globalCursorWindow, self->globalCursor->Xc);
	/* If we have a popup, we stack the global cursor window below it. */
	if (self->popup_active) {
	    XWindowChanges changes;

	    changes.sibling = xim2window(self->popup_active);
	    changes.stack_mode = Below;
	    XConfigureWindow(xim2display(self), self->globalCursorWindow, (CWSibling | CWStackMode), &changes);
	} else 
	    XRaiseWindow(xim2display(self), self->globalCursorWindow);
        XMapWindow(xim2display(self), self->globalCursorWindow);
	LocateWindow(self);
    }

    /* See if all cursors punted but window remains. If yes, then get rid of overlaying global cursor window (which will inherit cursors from parent and possibly screw up everything */
    if ((self->globalCursor == NULL || self->globalCursor->Xc == 0) && self->globalCursorWindow) {
        if (cursordebug) printf("(xim)updateGlobalCursors: unmapping global cursor window %X\n", self->globalCursorWindow);
	XUnmapWindow(xim2display(self),self->globalCursorWindow);
    }

    self->header.im.CursorsAreActive = 1;

}

void xim__SetTitle(self, title)
struct xim *self;
char *title;
{
    char *oldtitle=xim_GetTitle(self);
    if(oldtitle && title && !strcmp(oldtitle, title)) return;
    
    super_SetTitle(self, title);

    SetWMProperties(self, FALSE);
}


/* = * = * = * = * = * = * = * = * = * = * = * = *
	Cut Buffer

	ought to revise to have timeout
	or at least message_DisplayString that ^G will terminate 


for each elt:
	allocate
	SetPNMask
	free
	RemovePNMask
	handle and transition to the next

	delete properties
	XFree propLists

	be sure the property name in the proplistelt 
		is the one that will be delivered

 * = * = * = * = * = * = * = * = * = * = * = * = */

#define MAXXFER 3500

/* SetPNMask(display, window)
    RemovePNMask(display, window)
	These routines turn on and off interest in PropertyChange events 
	for remote windows.  A count is maintained so the interest will not be 
	turned off as a result of one operation while another is in progress.
*/
	static void
SetPNMask(display, window)
	Display *display;
	Window window;
{
	struct masklist *ml;
	for (ml = MaskList; ml != NULL;  ml = ml->next)
		if (ml->window == window) {
			ml->count++;
			return;
		}
	ml = (struct masklist *)malloc(sizeof(struct masklist));
	ml->count = 1;
	ml->window = window;
	ml->next = MaskList;
	MaskList = ml;
	XSelectInput(display, window, PropertyChangeMask);
}
	static void
RemovePNMask(display, window)
	Display *display;
	Window window;
{
	struct masklist *ml, *prev = NULL;
	for (ml = MaskList;  ml != NULL;  prev = ml, ml = ml->next)
		if (ml->window == window) 
			break;
	if (ml == NULL) return;

	ml->count--;
	if (ml->count <= 0) {
		if (prev==NULL) MaskList = ml->next;
		else prev->next = ml->next;
		free(ml);
		XSelectInput(display, window, NoEventMask);
	}
}

appendtostring(string, buffer, length)
	struct expandstring *string;
	char *buffer;
	long length;
{
	if (string->string == NULL) {
		string->string = malloc(string->size = length+1);
		string->pos = string->length = 0;
	}
	else if (string->size < string->length + length + 1)
		string->string = realloc(string->string, 
				string->size = string->length + length + 1);
	strncpy(string->string + string->length, buffer, length);
	string->length += length;
	string->string[string->length] = '\0';
}


/* xim_Now(self)
	generate the current time by a zero-length append to
	XA_CUT_BUFFER0 on the root window

	This sure is a stupid way to find out what time it is! 
*/
	static Time
xim_Now(self)
	struct xim *self;
{
	Display *display = xim2display(self);
	Window window =  RootWindow(display, 0);
	XEvent event;

	static char *junk = "";
	
	if (lastEventTime == CurrentTime) {
		SetPNMask(display, window);
		(void) XChangeProperty(display, window,
			XA_CUT_BUFFER0,  XA_STRING, 
			 8, PropModeAppend, junk, 0);
		XWindowEvent(display, window,
			PropertyChangeMask, &event);

		if (event.xany.type == PropertyNotify)
			lastEventTime = event.xproperty.time;	/* BINGO */
		/* else we didn't get a time!  Leave it as CurrentTime */

		RemovePNMask(display, window);
	}
	return lastEventTime;
}

/* PostPropList(window, prop, state, time)
	Post to PropList the information to await property 'prop' on window 'window'
	Returns the newly enqueued element so the caller can add values.
*/
	static struct proplistelt *
PostPropList(window, prop, state, time)
	Window window;
	Atom prop;
	enum PropState state;
	Time time;
{
	struct proplistelt *elt;
	if (FreeProps != NULL) {
		elt = FreeProps;
		FreeProps = elt->next;
	}
	else
		elt = (struct proplistelt *)malloc(sizeof(struct proplistelt));
	elt->window = window;
	elt->atom = prop;
	elt->state = state;
	elt->when = time;
	elt->string = NULL;
	elt->xfree = NULL;
	elt->next = PropList;
	PropList = elt;

	return elt;
}

/* DequeueProp(window, atom)
	find the PropList element for the given window and property
	remove it from PropList and return it
	return NULL for failure to find it
	(Note: if there are multiple waits on same window-atom pair, 
		all are dequeued, but only the last (the earliest) is returned)
*/
	static struct proplistelt *
DequeueProp(window, atom)
	Window window;
	Atom atom;
{
	struct proplistelt *prev, *elt, *bingo = NULL;
	for (prev = NULL, elt = PropList; elt != NULL; prev = elt, elt = elt->next)
		if (elt->window == window  &&  elt->atom == atom) {
			bingo = elt;
			if (prev == NULL)  PropList = elt->next;
			else prev->next = elt->next;
		}
	return bingo;
}

/* RequeueProp(elt)
	put the elt back on the queue
*/
	static void
RequeueProp(elt)
	struct proplistelt *elt;
{
	elt->next = PropList;
	PropList = elt;
}

	static void
CancelProp(elt)
	struct proplistelt *elt;
{
	elt->next = FreeProps;
	FreeProps = elt;
}

/* PropLoop()
	The loop in this routine stays active until all events have been 
	handled for sending to a property or receiving it.
	Global data used:
		PropList - a list of pending property operations

	xxx should timeout
	xxx should at least display message that control-G is available
*/
	static void
PropLoop()
{
	struct proplistelt *elt, *next;
	while (PropList != NULL) {
		xim_HandleFiles(event_ENDOFTIME, FALSE);
		if (im_CheckForInterrupt()) {
			/* for each element on PropList, 
				call its cancel routine */
			for (elt = PropList; elt != NULL; elt = next) {
				next = elt->next;
				CancelProp(elt);
			}
			PropList = NULL;
			return;
		}
	}
}


/* retrieveProperty(dpy, wndw, prop, del, cb, pXfree, pActualtype)
	fetches the data under property 'prop' on window 'wndw' 
	and stores it in 'cb'
	If 'del' is true, delete the existing property value.
	If cb->string exists, append to it.
	otherwise, if one fetch was enough, use the string returned from x
		and set *pXfree to TRUE
	return TRUE for success, FALSE for failure or zero-length retrieval
*/
	static boolean
retrieveProperty(dpy, wndw, prop, del, cb, pXfree, pActualtype)
	Display *dpy;
	Window wndw;
	Atom prop;
	struct expandstring *cb;
	boolean *pXfree;
	Atom *pActualtype;
{
	int actualformat;
	unsigned long nitems, remainingbytes;
	unsigned char *data;
	Atom junktype;
	long offset;	/* start XGet loc in 32-bit quantities */
	long maxlen = MAXXFER / 4;  /* # of 32-bit quantities to get */

	offset = 0;
	if (Success != XGetWindowProperty(dpy, wndw, prop, offset, maxlen,
				del,  AnyPropertyType, 
				pActualtype, &actualformat,
				&nitems, &remainingbytes, &data)
			|| nitems == 0) 
		return FALSE;

	if (remainingbytes == 0 && cb->string == NULL) {
		/* okay, the XGetWindowProperty above was enough */
		cb->string = (char *)data;
		cb->size = cb->length = nitems * (actualformat >> 3);
		cb->pos = 0;
		if (pXfree != NULL)
			*pXfree = TRUE;	
                return TRUE;
	}
	else  while (TRUE) {
		/* okay, we've got to loop to get the rest (sigh) */
		appendtostring(cb, (char *)data, nitems * (actualformat >> 3));
		if (remainingbytes == 0) 
			return TRUE;
		offset += maxlen;
		if (Success != XGetWindowProperty(dpy, wndw, prop, offset, maxlen,
				del,  AnyPropertyType, 
				&junktype, &actualformat,
				&nitems, &remainingbytes, &data)) 
			return FALSE;
	}
}


/* HandleSelectionNotify(event)
	We are being told by a selection owner that the selection value has
	been stored on our window.  Find it and retrieve property value as cut value.
*/
	static void
HandleSelectionNotify(event)
	XSelectionEvent *event;
{
	struct proplistelt *elt, *nelt;
	long success;
	Atom actualtype;
	int actualformat;
	unsigned long nitems, remainingbytes;
	unsigned char *propList;
	static Atom DesirableTargets[4] = {None};
	long i, j;
	Atom target;

	if (DesirableTargets[0] == None) {
		DesirableTargets[0] = xim_ATK;
		DesirableTargets[1] = XA_STRING;
		DesirableTargets[2] = xim_TEXT;
		DesirableTargets[3] = None;
	}

	elt = DequeueProp(event->requestor, event->selection);
	if (elt == NULL) return;

	switch (elt->state) {

		/* a 'break' without a PostPropList will effectively cancel the operation */

	case prop_SelectTarget:
		if (event->target != xim_TARGETS)  break;
		/* decide what target to ask for */
		success = XGetWindowProperty(event->display, event->requestor, 
			event->property, 0, MAXXFER/4,
			TRUE /* delete */,  AnyPropertyType, 
			&actualtype, &actualformat,
			&nitems, &remainingbytes, &propList);
		target = None;
		if (success == Success) {
		    /*
		     * XXX - work around XView bug, wherein it gives the type
		     * TARGETS rather than ATOM to the result.
		     */
		    if (actualtype == xim_TARGETS)
			actualtype = XA_ATOM;
		    if (actualtype == XA_ATOM) {
			for (i = 0; DesirableTargets[i] != None && target == None; i++)
			    for (j = 0; j < nitems && target == None; j++)
				if (DesirableTargets[i] == ((Atom *)propList)[j]) 
					target = DesirableTargets[i];
		    } else
			fprintf(stderr,"TARGETS targets isn't of type ATOM\n");
		}
		XFree((caddr_t)propList);
		if (target == None) {
		    /*
		     * They don't speak our language.
		     */
		    fprintf(stderr,"Owner of selection handles neither ATK nor STRING nor TEXT\n");
		    break;
		}

		/* okay, we'll ask for the selection in type 'target' */

		nelt = PostPropList(elt->window, elt->atom, prop_Incoming, elt->when);
		nelt->string = elt->string;
		nelt->xfree = elt->xfree;
		XConvertSelection(event->display, elt->atom,  target,
			 xim_INCOMING, elt->window, elt->when);
		break;

	case prop_Incoming:
		/* we got selection to a property.  Now put it in elt->string for
			transfer to the vfile */
		if (event->property == None) break;

		if ( ! retrieveProperty(event->display, event->requestor, 
				event->property, TRUE, 
				elt->string, elt->xfree, &actualtype))
			break;

		/* is is an INCR ? */
		if (actualtype == xim_INCR) {
			/* nuts, it is INCR.  start multiple retrievals */
			remainingbytes = *(long *)elt->string->string;

			if (*elt->xfree)  XFree(elt->string->string);
			else   free(elt->string->string);
			*elt->xfree = FALSE;

			nelt = PostPropList(event->requestor, event->property,
				prop_InIncr, elt->when);
			nelt->string = elt->string;
			nelt->string->size=remainingbytes+1;
			nelt->string->string = malloc(nelt->string->size);
			nelt->string->length = 0;
			nelt->xfree = elt->xfree;
			break;
		}

		break;
	}
	CancelProp(elt);
}



/* HandleProperty(event)
*/
	static void
HandleProperty(event)
	XPropertyEvent *event;
{
	struct proplistelt *elt, *nelt;
	Atom actualtype;
	unsigned long nitems, remainingbytes;

	elt = DequeueProp(event->window, event->atom);
	if (elt == NULL) return;

	switch (elt->state) {

		/* a 'break' without a PostPropList will effectively cancel the operation */

	case prop_InIncr:
		/* we will get a spurious event as the property is deleted,
			ignore it.  */
		if (event->state != PropertyNewValue) {
			RequeueProp(elt);
			return;
		}
		/* we got selection to a property.  Now put it in elt->string for
			transfer to the vfile  */
		if ( ! retrieveProperty(event->display, event->window, 
				event->atom,  TRUE, 
				elt->string, NULL, &actualtype))
			break;

		nelt = PostPropList(event->window, event->atom,
				prop_InIncr, elt->when);
		nelt->string = elt->string;
		nelt->xfree = elt->xfree;
		break;
	
	case prop_OutIncr:
		/* we will get a spurious event as the property is created,
			ignore it.  */
		if (event->state != PropertyDelete)  {
			RequeueProp(elt);
			return;
		}
		/* send some of the string.  quit only after send a 0 length string */
		remainingbytes = elt->string->length - elt->string->pos;
		nitems = (remainingbytes > MAXXFER) ? MAXXFER : remainingbytes;
		if (remainingbytes > 0) {
			nelt = PostPropList(event->window, event->atom, 
				prop_OutIncr, event->time);
			nelt->string = elt->string;
			nelt->type = elt->type;
		}
		else {
			/* we are done,  discard the cutBuffer struct */
			if (elt->xfree) XFree(elt->string->string);
			else   free(elt->string->string);
			free(elt->string);
			RemovePNMask(event->display, elt->window);
		}
		XChangeProperty(event->display, event->window,
			event->atom, elt->type, 8, PropModeReplace, 
			elt->string->string + elt->string->pos, nitems);
		elt->string->pos += nitems;
		break;
	}
	CancelProp(elt);
}




/* RequestSelection(self, cutBuff)
	 get the selection from the owner into cutBuffer 
		request TARGETS, look for ATK, STRING, or TEXT.
		send selection request
		wait for selection notify
		if INCR, go into loop getting pieces
		delete properties after value is retrieved
*/
	static void
RequestSelection(self, cutBuff, xfree)
	struct xim* self;
	struct expandstring *cutBuff;
	boolean *xfree;
{
	Display *display = xim2display(self);
	Window window = xim2window(self);
	Window selOwner;
	Atom selection;
	Time now = xim_Now(self);
	struct proplistelt *elt;

	/*  determine whether to look for PRIMARY or CLIPBOARD
		(We should check timestamps.  XXX) */
	selection = None;
	selOwner = XGetSelectionOwner(display, XA_PRIMARY);
	if (selOwner != None) 
		selection = XA_PRIMARY;
	else {
		/* no PRIMARY,  let's try the CLIPBOARD */
		selOwner = XGetSelectionOwner(display, xim_CLIPBOARD);
		if (selOwner != None)
			selection = xim_CLIPBOARD;
	} 
	if (selection == None) 
		return;

	/* initiate the transfer, asking first for targets */
	elt = PostPropList(window, selection, prop_SelectTarget, now);
	elt->string = cutBuff;
	elt->xfree = xfree;
	XConvertSelection(display, selection,  xim_TARGETS,
		 xim_INCOMING, window, now);
	if (PropList->next == NULL)
		PropLoop();
	/* the loop will exit when there are no more property waits in progress 
		at that time, *cutBuffer and *xfree will have values */
}

/* ProcessRequest(req)
	Process a single property request:
		TARGETS, TIMESTAMP, ATK, STRING, or TEXT

	For ATK, STRING, TEXT
		Check time vs. time of the Selection ownership, fail if not proper.

		Get the value from the cut buffer and transfer it.
		Put the response on the window req->requestor under
			property req->property
		If req->property is NONE, use req->target as the property. 
		If greater than MAXXFER bytes, use INCR scheme.
*/
	static boolean
ProcessRequest(req)
	XSelectionRequestEvent *req;
{
	Atom targets[7];
	struct expandstring cutBuffer;
	boolean xfree;
	struct proplistelt *elt;
	Atom actualtype;
	long OriginalNerrors = Nerrors;

	if (req->property == None) 
		req->property = req->target;

	if (req->target == xim_TARGETS) {
		targets[0] = xim_ATK;
		targets[1] = XA_STRING;
		targets[2] = xim_TIMESTAMP;
		targets[3] = xim_TEXT;
		targets[4] = xim_TARGETS;
		targets[5] = xim_MULTIPLE;
		targets[6] = xim_LENGTH;
		XChangeProperty(req->display, req->requestor, req->property,
			XA_ATOM, 32, PropModeReplace, (char *)targets, 7);
	}
	else if (req->target == xim_TIMESTAMP) {
		XChangeProperty(req->display, req->requestor, req->property,
			XA_INTEGER, 32, PropModeReplace, 
			(char *)&SelectionStartTime, 1);
	}
	else if (req->target == xim_ATK  ||  req->target == XA_STRING  
			||  req->target == xim_TEXT
			||  req->target == xim_LENGTH) {
		if ( ! IOwnSelection ||
				(req->time != CurrentTime  
				&&  0 > (long)(req->time - SelectionStartTime))) {
			/* invalid request */
			req->property = None;
			return FALSE;
		}
		cutBuffer.string = NULL;
		cutBuffer.length = cutBuffer.size = cutBuffer.pos = 0;
		xfree = FALSE;
		retrieveProperty(req->display, RootWindow(req->display, 0), 
			XA_CUT_BUFFER0, FALSE, 
			&cutBuffer, &xfree, &actualtype);

		if (cutBuffer.string == NULL) {
			req->property = None;
			return FALSE;
		}
		if (req->target == xim_LENGTH) {
			/*
			 * Keep XView happy by telling it how big the
			 * selection is.  The SunView System Programmer's
			 * Guide says that SELN_REQ_BYTESIZE is "the number
			 * of bytes in the selection's ascii(sic) contents",
			 * and that's what XView turns into a request for
			 * LENGTH, so we delete objects and remove styles,
			 * and give them the resulting size.
			 */
			Unscribe(&cutBuffer);
			/* send it with one XChangeProperty */
			XChangeProperty(req->display, req->requestor,
				req->property, req->target, 32,
				PropModeReplace,
				(unsigned char *)&cutBuffer.length, 1);
			if (xfree)  XFree(cutBuffer.string);
			else   free(cutBuffer.string);
		} else {
			if (req->target == XA_STRING  ||  req->target == xim_TEXT) {
				/* delete objects and remove styles */
				Unscribe(&cutBuffer);
				if (req->target == xim_TEXT)
					req->target = XA_STRING;
			}

			/* send the data to the requestor */
			if (cutBuffer.length < MAXXFER) {
				/* send it with one XChangeProperty */
				XChangeProperty(req->display, req->requestor,
					req->property, req->target, 8,
					PropModeReplace, cutBuffer.string,
					cutBuffer.length);
				if (xfree)  XFree(cutBuffer.string);
				else   free(cutBuffer.string);
			}
			else {
				/* send it via INCR scheme */
				SetPNMask(req->display, req->requestor);
				elt = PostPropList(req->requestor,
				req->property, prop_OutIncr, req->time);
				elt->string = (struct expandstring *)
					malloc(sizeof(struct expandstring));
				*(elt->string) = cutBuffer;
				elt->xfree = (xfree) ? &xfree : NULL;
					/* the value of elt->xfree is tested
					for non-NULL, but *elt->xfree is not used */
				elt->type = req->target;
				XChangeProperty(req->display, req->requestor,
					req->property,xim_INCR, 32,
					PropModeReplace,
					(unsigned char *)&elt->string->length,
					1);
				/* the main Interact loop will handle
				 PropertyEvents we have  to exit to do the
				 SelectionNotify */
			}
		}
		if (OriginalNerrors != Nerrors) {
			/* OOPS.  There was an error.  Reject the request. */
			XDeleteProperty(req->display, req->requestor, req->property);
			req->property = None;
			return FALSE;
		}
	}
	else {
		/* reject the request */
		req->property = None;
		return FALSE;
	}
	return TRUE;
}

/* ProcessMultiple(req)
	respond to a request for MULTIPLE targets.
	Get the request list from the requestor window.
	Call ProcessRequest for each target.
	Write the request list back if there have been any failures.
*/
	static void
ProcessMultiple(req)
	XSelectionRequestEvent *req;
{
	long success;
	Atom actualtype;
	int actualformat;
	unsigned long nitems, remainingbytes;
	unsigned char *propList;
	Atom *pairlist;
	long i;
	Atom originalProperty;
	boolean OK = TRUE;
	
	originalProperty = req->property;

	success = XGetWindowProperty(req->display, req->requestor, 
			req->property, 0, MAXXFER/4,
			TRUE /* delete */,  AnyPropertyType, 
			&actualtype, &actualformat,
			&nitems, &remainingbytes, &propList);
	if (success != Success) {
		req->property = None;
		return;
	}
	pairlist = (Atom *)propList;

	/* go thru the elements of the pair list and respond */
	for (i = 0;  i < nitems/2; i +=2) {
		req->target = pairlist[i];
		req->property = pairlist[i+1];
		if ( ! ProcessRequest(req)) {
			pairlist[i+1] = None;
			OK = FALSE;
		}
	}
	if ( ! OK) {
		/* at least one failure: rewrite the originalProperty with modified values */
		XChangeProperty(req->display, req->requestor, originalProperty,
			XA_ATOM, 32, PropModeReplace, 
			propList, nitems);
	}
	XFree(propList);
}


/* RespondToSelectionRequest(req)
	Respond to the XSelectionRequestEvent 'req'.
	When done, send SelectionNotify event.  
		If failed, its property field says None.
*/
	static void
RespondToSelectionRequest(req)
	XSelectionRequestEvent *req;
{
	XSelectionEvent NotificationEvent;

	if (req->target == xim_MULTIPLE) {
		if (req->property != None) 
			ProcessMultiple(req);
	}
	else  
		ProcessRequest(req);

	/* now notify the requestor */
	NotificationEvent.type = SelectionNotify;
	NotificationEvent.send_event = TRUE;
	NotificationEvent.display = req->display;
	NotificationEvent.requestor = req->requestor;
	NotificationEvent.selection = req->selection;
	NotificationEvent.target = req->target;
	NotificationEvent.property = req->property;
	NotificationEvent.time = req->time;

	XSendEvent(req->display, req->requestor, 
		FALSE, NoEventMask, (XEvent *)&NotificationEvent);
}


	FILE *
xim__FromCutBuffer(self)
	struct xim *self;
{
	FILE *pasteFile;
	struct expandstring cutBuffer;
	boolean xfree;
	Atom actualtype;

	cutBuffer.length = cutBuffer.size = cutBuffer.pos = 0;
	cutBuffer.string = NULL;
	xfree = FALSE;

	if (CBCacheValid) {}
	else if (SelectionAndCB  &&  ! IOwnSelection)
		RequestSelection(self, &cutBuffer, &xfree);
	else 
		retrieveProperty(xim2display(self), 
			RootWindow(xim2display(self), 0), 
			XA_CUT_BUFFER0, FALSE, 
			&cutBuffer, &xfree, &actualtype);

	/* Give the data to the vfile */
	if (CBCacheValid)
		pasteFile = (FILE *) im_vfileopen("r", &CachedCutBuffer);
	else {
		pasteFile = (FILE *) im_vfileopen("r", &cutBuffer);

		/* free the X data returned from server */
		if (cutBuffer.string != NULL)  {
			if (xfree)  XFree(cutBuffer.string);
			else   free(cutBuffer.string);
		}
	}

	return pasteFile;
}

	static void
sendToCutBuffer(self, initialmode, cb)
	struct xim *self;
	int initialmode;
	struct expandstring *cb;
{
	Display *xDisplay = xim2display(self);
	Window window =  RootWindow(xDisplay, 0);
	long l;
	char *x;
	int mode = initialmode;

/*   The following fails:
 *	XStoreBytes(xDisplay, cb->string, cb->length);
 *   (The library implementation of XChangeProperty ignores max_request_size.)
 *   So we use XChangeProperty.
 *
 *   Arguably the type should be xim_ATK because it is not pure STRING data.
 *   But XFetchBytes demands that the type be XA_STRING.  We leave it
 *   as XA_STRING because other applications may use XFetchBytes.
 *
 */

	l = cb->length;	/* length to send */
	x = cb->string;	/* source location */
	while (l > 0) {
		/* send a chunk of bytes */
		XChangeProperty(xDisplay, window, XA_CUT_BUFFER0,
			XA_STRING, 8, mode, (unsigned char *) x, 
			(l > MAXXFER) ? MAXXFER : l);
		l -= MAXXFER;
		x += MAXXFER;
		mode = PropModeAppend;  /* mode for subsequent transfers */
	}
}

	void 
xim__CloseToCutBuffer(self, writeFile)
	struct xim *self;
	FILE *writeFile;
{
	Display *display = xim2display(self);
	Window window = xim2window(self);
	Window selOwner;
	Time now = xim_Now(self);

	im_vfileclose(writeFile, &CachedCutBuffer);
/*
		the cut buffer cache fails in the case where another
		process puts something into CUT_BUFFER0 without
		grabbing PRIMARY, which would tell this process
		to drop the cache
	CBCacheValid = TRUE;
*/
	XRotateBuffers(display, 1);
	sendToCutBuffer(self, PropModeReplace, &CachedCutBuffer);

	if (SelectionAndCB  &&  ! IOwnSelection) {
		/* grab the PRIMARY selection */

		XSetSelectionOwner(display, XA_PRIMARY, window, now);
		selOwner = XGetSelectionOwner(display, XA_PRIMARY);
		if (selOwner == window) {
			IOwnSelection = TRUE;
			SelectionStartTime = now;

			/* What the heck.  Let's grab the clipboard, too. */
			XSetSelectionOwner(display, xim_CLIPBOARD, window, now);
		}
	}
}

	void 
xim__RotateCutBuffers(self, count)
	struct xim *self;
	long count;
{
	XRotateBuffers(xim2display(self), -count);
}

	void 
xim__AppendToCutBuffer(self, writeFile)
	struct xim *self;
	FILE *writeFile;
{
	im_vfileclose(writeFile, &writeCutBuffer);
	sendToCutBuffer(self, PropModeAppend, &writeCutBuffer);
	if (CBCacheValid)
		appendtostring(&CachedCutBuffer, writeCutBuffer.string, 
				writeCutBuffer.length);
}



/* = * = * = * = * = * = * = * = * = * = * = * = *
	Focus, Hide, Expose
 * = * = * = * = * = * = * = * = * = * = * = * = */


void xim__SetWMFocus(self)
    struct xim *self;
{
    /* First make sure window is around! */
    if(self->IsOffscreenWindow) return;
    if ( ! self->CurrentlyMapped)
	return;
    /* And make sure if we move the pointer there that the pointer lands on it! */
    XRaiseWindow(xim2display(self), xim2window(self));
    /* And move the mouse there */
    XWarpPointer(xim2display(self), NULL, xim2window(self), 0,0,0,0,0,0);
    /* be sure it gets the input focus */
    XSetInputFocus(xim2display(self), xim2window(self), RevertToPointerRoot, lastEventTime);
    
    /* Be sure to admit we don't need the input focus anymore */
    XSetInputFocus(xim2display(self), RootWindow(xim2display(self),xim2screen(self)),RevertToPointerRoot, lastEventTime);
    /* do it now! */
    XFlush(xim2display(self));
}

	void 
xim__ExposeWindow(self)
	struct xim *self;
{
	Display *D = xim2display(self);
	if(!self->EverMapped) {
	    xim_RedrawWindow(self);
	}
	if ( ! self->CurrentlyMapped) {
		/* we currently don't think the window is mapped.  Let's make sure
			This code is really here to try to make the X11R3 twm
			expose the window if iconified   -wjh */
		XUnmapWindow(D, xim2window(self));  /* does this help? -wjh */
	}
	if (!self->IsOffscreenWindow) {
	    XMapRaised(D, xim2window(self));
	    LocateWindow(self);
	}
	XFlush(D);

	/* If we're a popup, put us into the parent's chain
	 of active popups at the end.  */
	/* If we're already in the chain, just return */
	if (self->popup_parent) {
	    XWindowChanges changes;
	    int mask = 0;
	    struct xim *parent = self->popup_parent;

	    while (parent->popup_active) {
		if (parent->popup_active == self) return;
		parent = parent->popup_active;
		changes.sibling = xim2window(parent);
		changes.stack_mode = Above;
		mask = CWSibling | CWStackMode;
	    }
	    if (mask) XConfigureWindow(xim2display(self), xim2window(self), mask, &changes);
	    parent->popup_active = self;
	}

}


/* xim__HideWindow(self)
	Iconify the window.
*/
	void
xim__HideWindow(self)
	struct xim *self;
{
	XClientMessageEvent cmsg;

	/* If we're a popup, just return without doing anything */ 
	if (self->popup_parent) return;
	/* same with offscreen window */
	if(self->IsOffscreenWindow) return;

	cmsg.display = xim2display(self);
	cmsg.type = ClientMessage;
	cmsg.message_type = wmchangestate;
	cmsg.window = xim2window(self);
	cmsg.send_event = TRUE;
	cmsg.format = 32;
	cmsg.data.l[0] = IconicState;

	XSendEvent(xim2display(self), RootWindow(xim2display(self), xim2screen(self)), 
		FALSE, (SubstructureRedirectMask | SubstructureNotifyMask), (XEvent *)&cmsg);

	XFlush(xim2display(self));
	self->CurrentlyMapped = FALSE;
}

/* xim__VanishWindow(self)
	cause a window to be completely hidden.  Not even an icon.
*/
	void 
xim__VanishWindow(self)
	struct xim *self;
{
	    /* not necessary for offscreen win */
	    if(self->IsOffscreenWindow) return;
	XUnmapWindow(xim2display(self),xim2window(self));

	/* If we're a popup, remove ourself from the popup
	 chain, and don't bother sending our unmap notify event */
	if (self->popup_parent) {
	    struct xim *cur, *prev;

	    cur = prev = self->popup_parent;
	    while ((cur = cur->popup_active) != NULL) {
		if (cur == self) {
		    break;
		}
		prev = cur;
	    }
	    if (cur == self) prev->popup_active = self->popup_active;
	} else {
	    XUnmapEvent unmapnotify;

	    unmapnotify.display = xim2display(self);
	    unmapnotify.type = UnmapNotify;
	    unmapnotify.window = xim2window(self);
	    unmapnotify.event = RootWindow(xim2display(self), xim2screen(self));
	    unmapnotify.send_event = TRUE;
	    unmapnotify.from_configure = FALSE;
	    XSendEvent(xim2display(self), unmapnotify.event, FALSE, 
		       (SubstructureRedirectMask | SubstructureNotifyMask), (XEvent *)&unmapnotify);
	}

	XFlush(xim2display(self));
	self->CurrentlyMapped = FALSE;
}


/* deletefromstring(b, start, len)
	delete 'len' characters beginning at 'bx' from expandstring 'b'
	'bx' must be a pointer into b->string
*/
	static void
 deletefromstring(b, bx, len)
	struct expandstring *b;
	char *bx;
	long len;
{
	strncpy(bx, bx+len, 1 + (b->length + b->string) - (bx + len));
	b->length -= len;
}

/* 	op 1 - delete from backslash to and including char close
	op 2 - delete up to and including matching \enddata{...}\n
*/
static struct copstype {char *name; long op; char close;}
	controlops[] = {
		{"view", 1, '}'},
		{"begindata", 2, ','},
		{"define", 1, '}'},
		{"enddata", 1, '\n'},
		{"template", 1, '}'},
		{"textdsversion", 1, '}'},
		{"origin", 1, '}'},
		{NULL, 1, '{'}
	};

/* Unscribe(cb)
	remove ATKisms from the data stream

    keep only the contents of text object(s):
	o remove the first of each set of consecutive newlines
	o remove backslash-newline
	o remove extra backslashes before }, {, and \
	o remove \begindata{otherthantext,99999} ...  \enddata{same}\n
	o remove \begindata{text,99999}\n   and   \enddata{same}\n
	o remove entirety of:  \template, \origin, \view,  \textdsversion, \define
	o remove \style{  and  corresponding }
*/
	static void
Unscribe(cb)
	struct expandstring *cb;
{
	register char *cx, *tx;
	char *brace;
	boolean istext;
	long wordlen, idnum;
	struct copstype *cop;
	static char *atkflag = "\\begindata{";
	long flaglen = strlen(atkflag);

	if (strncmp(cb->string, atkflag, flaglen) != 0) {
		/* not ATK data stream:  no change */
		return;
	}
	/* make sure the text is followed by an end of string ( for index() ) */
	if (cb->string[cb->length] != '\0') {
		appendtostring(cb, "", 1);
		cb->length--;
	}
	cx = index(cb->string+flaglen, ',');
	if (cx == NULL) {
		/* invalid ATK datastream: sigh.  Send as is.  */
		return;
	}
	*cx = '\0';
	istext = class_IsTypeByName(cb->string+flaglen, "simpletext");
	*cx = ',';
	if (! istext) {
		/* ATK data stream, but not text:  forget it  */
		cb->length = 0;
		return;
	}

	/* It is an ATK text datastream.  Strip ATKisms. */

	/* delete the \begindata line */
	brace = index(cx, '\n');
	if (brace == NULL) return;
	deletefromstring(cb, cb->string, brace+1 - cb->string);

	/* iterate over text looking for backslashes, braces, and newlines */
	cx = cb->string;
	while (*cx != '\0') {
		if (*cx == '}') {
			/* close brace for a style: delete it */
			deletefromstring(cb, cx, 1);
			continue;
		}
		if (*cx == '\n') {
			/* delete the first of the group of newlines */
			deletefromstring(cb, cx, 1);
			do {cx++;} while (*cx == '\n');
			continue;	
		}		
		if (*cx != '\\') {
			/* uninteresting character: ignore it */
			cx++;
			continue;
		}

		/* hereafter:  *cx == '\\' */

		if (*(cx+1) == '\n') {
			/* backslash newline: delete  both */
			deletefromstring(cb, cx, 2);
			continue;
		}
		if (*(cx+1) == '}' || *(cx+1) == '{' || *(cx+1) == '\\') {
			/* backslash followed by quoted char:
				delete the leading backslash */
			deletefromstring(cb, cx, 1);
			cx++;
			continue;
		}
		brace = index(cx, '{');
		if (brace == NULL  ||  brace - cx > 100) {
			/* silly backslash.  keep it */
			cx++;
			continue;
		}

		/* process backslash followed by word and brace  */
		wordlen = brace - cx -1;
		for (cop = controlops; cop->name != NULL
				&& strncmp(cx+1, cop->name, wordlen) != 0;
			cop++) {}
		brace = index(brace, cop->close);
		if (brace == NULL) {cx++; continue;}
		switch (cop->op) {
		case 1:	/* delete from backslash to and including char close */
			deletefromstring(cb, cx, brace+1-cx);
			continue;
		case 2:	/* begindata:  delete up to and including matching \enddata{...}\n 
				unfortunately, some applications put spaces after commas
				we cheat and look for a \enddata with same id number
				Because of cop->close, we have *brace==','.
			*/
			idnum = atol(brace+1);
			brace = index(brace, '\n');
			if (brace == NULL) {cx++; continue;}

			for (tx = brace+1; *tx != NULL; tx++) {
				if (*tx != '\\' ) continue;
				if (strncmp(tx+1, "enddata", 7) != 0) continue;
				tx = index(tx+8, ',');
				if (tx == NULL) break;
				if (idnum == atol(tx+1)) break;
			}
			if (tx == NULL || *tx == NULL)  {cx++; continue;}
			tx = index(tx+1, '\n');
			if (tx == NULL)   {cx++; continue;}
			deletefromstring(cb, cx, tx - cx + 1);
			continue;
		}
	}
}
	

static void freeViewsMLCache(self,cache, view)
struct xim *self;
struct mlcacheNode *cache;
struct basicobject *view;
{
    if (cache->next != NULL)
	freeViewsMLCache(self,cache->next, view);
    if (cache->others != NULL)
	freeViewsMLCache(self, cache->others, view);
    if(cache->ml && cache->ml->object==view) {
	if(cache->region) {
	    if(cache->region->menus) {
		mb_Destroy(cache->region->menus);
		cache->region->menus=NULL;
	    }
	    if(cache->region->cmenus) {
		cmenu_Destroy(cache->region->cmenus);
		cache->region->cmenus=NULL;
	    }
	    unlinkCacheRegion(cache->region);
	    linkCacheRegion(cache->region, &self->freeRegions);
	    cache->region=NULL; 
	}
	discardCachedML(cache, &self->mlcache);
    }
}

void xim__ObservedChanged(self, changed, value)
struct xim *self;
struct observable *changed;
long value;
{
    struct menuviews **mv, *mv2;
    if(value!=observable_OBJECTDESTROYED) return;
    mv=menuviewp(self, (struct view *)changed);
    if(mv) {
	mv2=(*mv)->next;
	free(*mv);
	*mv=mv2;
	if(self->mlcache) freeViewsMLCache(self, self->mlcache, (struct basicobject *)changed);
    }
}

struct rectangle *xim__GetLoc(self, view, rect)
struct xim *self;
struct view *view;
struct rectangle *rect;
{
    long x,y;

    if(self->IsOffscreenWindow) return NULL;
    rect->width = view_GetEnclosedWidth(view);
    rect->height = view_GetEnclosedHeight(view);
    for(x = y = 0; view != (struct view *)self; view = view->parent){
	x += view_GetEnclosedLeft(view);
	y += view_GetEnclosedTop(view);
    }
/*
    LocateWindow(self);
*/
    x+=self->xloc;
    y+=self->yloc;
    rect->left = x;
    rect->top  = y;
    return rect;
}

static void LocateWindow(self)
struct xim *self;
{
    int x,y;
    Window child;

    if(!self) return;
    if(self->IsOffscreenWindow) return;
    XTranslateCoordinates(xim2display(self), xim2window(self), RootWindow(xim2display(self), xim2screen(self)), 0, 0, &x, &y, &child);
    self->xloc = x;
    self->yloc = y;
    return;
}

boolean xim__ResizeWindow(self, width, height)
struct xim *self;
int width, height;
{
    XSizeHints hints;
    Pixmap newMap;
 
    if(self->IsOffscreenWindow) {
/* because of the way X works, must create */
/* a new pixmap of the appropriate new size, */
/* copy the data from the old to the new, */
/* then delete the old */
	newMap = XCreatePixmap(xim2display(self), RootWindow(xim2display(self), DefaultScreen(xim2display(self))), preferedWidth, preferedHeight, DefaultDepth(xim2display(self), DefaultScreen(xim2display(self))));
	/* clear new pixmap */
	XFillRectangle(xim2display(self), newMap, DefaultGC(xim2display(self),DefaultScreen(xim2display(self))), 0, 0, (unsigned int)width, (unsigned int)height);
	rectangle_SetRectSize( &self->header.view.drawable->localBounds, 0, 0, width,  height);
	XCopyArea(xim2display(self), xim2window(self), newMap, DefaultGC(xim2display(self), DefaultScreen(xim2display(self))), 0, 0, width, height, 0, 0);
	XFreePixmap(xim2display(self), xim2window(self));
	xim2window(self) = newMap;
	rectangle_SetRectSize( &self->header.view.drawable->localBounds, 0, 0, width, height);
	self->header.view.drawable->visualBounds = self->header.view.drawable->localBounds;
    }
    else {
	hints.width = width;
	hints.height = height;
	hints.flags = PSize;
/* is this necessary? */
/*
#ifdef PRE_X11R4_ENV
	XSetNormalHints(xim2display(self), xim2window(self), &hints);
#else
	XSetWMNormalHints(xim2display(self), xim2window(self), &hints);
#endif
*/
	XResizeWindow(xim2display(self), xim2window(self), width, height);
    }
    XFlush(xim2display(self));
    return TRUE;
}

boolean xim__MoveWindow(self, x, y)
struct xim *self;
long x,y;
{
    XSizeHints hints;

    if(self->IsOffscreenWindow) return FALSE;
    hints.x = x;
    hints.y = y;
    hints.flags = USPosition;
/* is this necessary? */
/*
#ifdef PRE_X11R4_ENV
    XSetNormalHints(xim2display(self), xim2window(self), &hints);
#else
    XSetWMNormalHints(xim2display(self), xim2window(self), &hints);
#endif
*/
    XMoveWindow(xim2display(self), xim2window(self), x, y);
    self->xloc = x;
    self->yloc = y;
    XFlush(xim2display(self));
    return TRUE;
}

void xim__SetBorderWidth(self, n)
struct xim *self;
long n;
{
    if(self->IsOffscreenWindow) return;
    XSetWindowBorderWidth(xim2display(self), xim2window(self), n);
}

boolean xim__CreateOffscreenWindow(self, other, width, height)
struct xim *self, *other;
long width, height;
{
    Display *xDisplay;
    Pixmap newPixmap;

    char *foregroundColor;
    char *backgroundColor;
    XColor foreground, background;

    self->IsOffscreenWindow = TRUE;
    self->EverMapped = FALSE;
    self->CurrentlyMapped = FALSE;
    self->menubaron = FALSE;
    self->xloc=0;
    self->yloc=0;

    xDisplay=xim2display(other);
    xim2display(self)=xim2display(other);
    SetForegroundBackground(self, &foregroundColor, &backgroundColor, &foreground, &background);
    newPixmap = XCreatePixmap(xDisplay, xim2window(other), (unsigned int)width, (unsigned int)height, DefaultDepth(xDisplay, xim2screen(other)));
    /* clear new pixmap */
    XFillRectangle(xDisplay, newPixmap, DefaultGC(xDisplay,DefaultScreen(xDisplay)), 0, 0, (unsigned int)width, (unsigned int)height);
    point_SetPt(&xim_GetDrawable(self)->physicalOrigin, 0, 0);
    xgraphic_InsertGraphicSize(xim2graphic(self), xim2graphic(other), 0, 0, width, height);
    point_SetPt(&xim_GetDrawable(self)->physicalOrigin, 0, 0);
    ((struct view *)self)->parent = NULL;
    xim2graphic(self)->localWindow = newPixmap;
    xim2window(self)= newPixmap;
    rectangle_SetRectSize( &self->header.view.drawable->localBounds, 0, 0, width,	 height);
    self->header.view.drawable->visualBounds = self->header.view.drawable->localBounds;
    point_SetPt(&self->header.view.drawable->enclosedOrigin, 0, 0);
    xgraphic_SetTransferMode((struct xgraphic *)xim2graphic(self), graphic_COPY);
    return TRUE;
}
