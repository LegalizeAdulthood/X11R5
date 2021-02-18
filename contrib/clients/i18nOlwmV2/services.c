/*
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) services.c 50.6 90/12/12 Crucible";

#ifdef OW_I18N
#include "i18n.h"
#endif
#include <stdio.h>
#include <strings.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#ifdef OW_I18N
#include "ollocale.h"
#endif
#include "events.h"
#include "olwm.h"
#include "win.h"
#include "menu.h"
#include "notice.h"
#include "globals.h"
#include "group.h"
#include "mem.h"
#include "resources.h"

char	*getenv();

/*
 * Externals
 */
extern Atom AtomProtocols;
extern Atom AtomSaveYourself;
extern Atom AtomShowProperties;
#ifdef OW_I18N
extern Atom AtomCompoundText;
#endif
extern Window	NoFocusWin;
extern char	**Envp;
extern Menu FrameFullMenu;

extern Bool UpdInputFocusStyle();

/*
 * Execute a command by handing it to /bin/sh.
 */
static void
execCommand(cmd)
    char *cmd;
{
    char *args[4];
    int pid;

    args[0] = "/bin/sh";
    args[1] = "-c";
    args[2] = cmd;
    args[3] = NULL;

    pid = fork();
    if (pid == -1) {
	perror("olwm: fork");
    } else if (pid == 0) {
	/* child */
	setpgrp(0, getpid());
	execve(args[0], args, Envp);
	perror("olwm: exec");
	exit(1);
    }
}


/***************************************************************************
* Exit from WM
****************************************************************************/

/*ARGSUSED*/	/* winInfo, menu, and idx args will be used 
		 * when more sophisticated ExitFunc is written */
int 
ExitFunc(dpy, winInfo, menu, idx)
Display *dpy;
WinGeneric *winInfo;
Menu *menu;
int idx;
{
	int		screen;
	NoticeBox	noticeBox;
#ifdef OW_I18N
static	wchar_t		*buttons[] = { NULL, NULL };
static	wchar_t		*strings;
#else OW_I18N
static	char		*buttons[] = { "Exit", "Cancel" };
static	char		*strings[] = 
				{ "Please confirm exit from window system" };
#endif OW_I18N

#ifdef OW_I18N
	if (buttons[0] == NULL)
	{
		/* STRING_EXTRACTION - The next two text are button
		   name for notice window which has message as third
		   text.
		 */
		buttons[0] = mbstowcsdup(gettext("Exit"));
		buttons[1] = mbstowcsdup(gettext("Cancel"));
		strings = mbstowcsdup(
			gettext("Please confirm exit from window system"));
	}
#endif OW_I18N

	/* multiple screens not yet supported */
	screen = DefaultScreen( dpy );

	/* set up noticeBox information */
	noticeBox.numButtons = 2;
	noticeBox.defaultButton = 1;
	noticeBox.buttonText = buttons;
#ifndef OW_I18N
	noticeBox.numStrings = 1;
#endif
	noticeBox.stringText = strings;
	noticeBox.boxX = -1;
	noticeBox.boxY = -1;

	/* If Exit button is selected, will return 0 */
	if ( UseNoticeBox( dpy, screen, &noticeBox ) == 0 )
		Exit(dpy);

}

/***************************************************************************
* Command execution
****************************************************************************/

/*
 * AppMenuFunc -- called when a command is listed as the item selected on
 *      the olwm menu
 */
/*ARGSUSED*/    /* dpy, winInfo args will be used later */
int
AppMenuFunc(dpy, winInfo, menu, idx)
Display *dpy;
WinGeneric *winInfo;
Menu    *menu;
int     idx;
{
	execCommand((char *)menu->buttons[idx].action.submenu);
}

/*
 * PshFunc -- called when the "POSTSCRIPT" keyword is present for the
 *      item selected in the olwm menu
 *
 */
/*ARGSUSED*/    /* dpy, winInfo args will be used later */
int
PshFunc(dpy, winInfo, menu, idx)
Display 	*dpy;
WinGeneric	*winInfo;
Menu    	*menu;
int     	idx;
{
	char    *commArgv[2];
	int	pshPipe[2];
	int     pid;
	char	*dir;
	char	pshPath[100];

	if ( (dir = getenv( "OPENWINHOME" )) == NULL )
		commArgv[0] = "/usr/bin/psh";
	else
	{
		strcpy( pshPath, dir ); 
		strcat( pshPath, "/bin/psh" );
		commArgv[0] = pshPath;
	}

	commArgv[1] = NULL;

	if ( pipe( pshPipe ) == -1 )
	{
		perror( "olwm: pipe" );
		return( -1 );
	}

	pid = fork();
	if ( pid == -1 )
	{
		perror("olwm: fork");
		return( -1 );
	}
	else if ( pid == 0 )
	{
		/* child reads from pipe and writes to stdout/err */
		close( 0 );		/* close stdin */
		dup( pshPipe[0] );	/* make stdin the read end */
		close( pshPipe[0] ); 	/* don't need orig pipe fds */
		close( pshPipe[1] );
		close( 1 );		/* close stdout */
		dup( 2 );		/* make olwm stderr = psh stdout */
		setpgrp(0, getpid());
		execve( commArgv[0], commArgv, Envp );
#ifdef OW_I18N
		/* STRING_EXTRACTION - "psh" is command name (do not
		   translate).
		 */
		fprintf( stderr, gettext("olwm: psh error: %d\n"), errno );
#else
		fprintf( stderr, "olwm: psh error: %d\n", errno );
#endif
	}
	else
	{
		/* parent writes user menu postscript code down pipe */
		close( pshPipe[0] );	/* don't need to read pipe */
		write( pshPipe[1], 
		       (char *)(menu->buttons[idx].action.submenu), 
		       strlen((char *)(menu->buttons[idx].action.submenu)) );
		close( pshPipe[1] );
	}
}

/***************************************************************************
* Window controls
****************************************************************************/

/*
 * WindowCtlFunc - Window Controls
 */
/*ARGSUSED*/    /* dpy, winInfo, menu, idx args used when implemented */
int
WindowCtlFunc(dpy, winInfo, menu, idx)
Display *dpy;
WinGeneric *winInfo;
Menu    *menu;
int     idx;
{
#ifdef OW_I18N
        fprintf(stderr, gettext("Window controls not implemented\n"));
#else
        fprintf(stderr,"Window controls not implemented\n");
#endif
}

/***************************************************************************
* Flip Drag
****************************************************************************/

/*ARGSUSED*/
int
FlipDragFunc(dpy, winInfo, menu, idx)
Display *dpy;
WinGeneric *winInfo;
Menu    *menu;
int     idx;
{
    GRV.DragWindow = !GRV.DragWindow;
    return 0;
}


/***************************************************************************
* Flip Focus
****************************************************************************/

int
FlipFocusFunc(dpy, winInfo, menu, idx)
    Display *dpy;
    WinGeneric *winInfo;
    Menu    *menu;
    int     idx;
{
    GRV.FocusFollowsMouse = !GRV.FocusFollowsMouse;
    (void) UpdInputFocusStyle(dpy, RM_INPUTFOCUS, True);
    return 0;
}

/***************************************************************************
* No-Operation
****************************************************************************/

/*
 * NopFunc - a no-operation function, used as a placeholder for
 *      the NOP service
 */
/*ARGSUSED*/    /* dpy, winInfo, menu, idx args included for consistency */
int
NopFunc(dpy, winInfo, menu, idx)
Display *dpy;
WinGeneric *winInfo;
Menu    *menu;
int     idx;
{
}

/***************************************************************************
* Clipboard
****************************************************************************/

/*ARGSUSED*/	/* dpy, winInfo, menu, and idx args will be used 
		 * when Clipboard is implemented */
int 
ClipboardFunc(dpy, winInfo, menu, idx)
Display *dpy;
WinGeneric *winInfo;
Menu *menu;
int idx;
{
	NoticeBox	noticeBox;
#ifdef OW_I18N
static	wchar_t		*buttons[] = { NULL };
static	wchar_t		*strings;
#else
static	char		*buttons[] = { "Ok" };
static	char		*strings[] = 
			    { "Sorry, the clipboard is not yet implemented." };
#endif

#ifdef OW_I18N
	if (buttons[0] == NULL)
	{
		/* STRING_EXTRACTION - The next text is button name
		   for notice window which has message as second text.
		 */
		buttons[0] = mbstowcsdup(gettext("Ok"));
		strings = mbstowcsdup(
		    gettext("Sorry, the clipboard is not yet implemented."));
	}
#endif OW_I18N
	/* set up noticeBox information */
	noticeBox.numButtons = 1;
	noticeBox.defaultButton = 0;
	noticeBox.buttonText = buttons;
#ifndef OW_I18N
	noticeBox.numStrings = 1;
#endif
	noticeBox.stringText = strings;
	noticeBox.boxX = -1;
	noticeBox.boxY = -1;

	(void) UseNoticeBox(dpy, DefaultScreen(dpy), &noticeBox);
}

/***************************************************************************
* Print Screen
****************************************************************************/

/*ARGSUSED*/	/* dpy, winInfo, menu, and idx args will be used 
		 * when Print Screen is implemented */
int 
PrintScreenFunc(dpy, winInfo, menu, idx)
Display *dpy;
WinGeneric *winInfo;
Menu *menu;
int idx;
{
	NoticeBox	noticeBox;
#ifdef OW_I18N
static	wchar_t		*buttons[] = { NULL };
static	wchar_t		*strings;
#else
static	char		*buttons[] = { "Ok" };
static	char		*strings[] = 
			    { "Sorry, Print Screen is not yet implemented." };
#endif

#ifdef OW_I18N
	if (buttons[0] == NULL)
	{
		/* STRING_EXTRACTION - The next text is button name
		   for notice window which has message as second text.
		 */
		buttons[0] = mbstowcsdup(gettext("Ok"));
		strings = mbstowcsdup(
			gettext("Sorry, Print Screen is not yet implemented."));
	}
#endif OW_I18N

	/* set up noticeBox information */
	noticeBox.numButtons = 1;
	noticeBox.defaultButton = 0;
	noticeBox.buttonText = buttons;
#ifndef OW_I18N
	noticeBox.numStrings = 1;
#endif
	noticeBox.stringText = strings;
	noticeBox.boxX = -1;
	noticeBox.boxY = -1;

	(void) UseNoticeBox(dpy, DefaultScreen(dpy), &noticeBox);
}


/***************************************************************************
* Refresh screen
****************************************************************************/

/*
 * RecursiveRefresh
 * 
 * Recursively refresh an entire window tree, by walking the hierarchy and 
 * sending Expose events to each window (via XClearWindow).  Note that 
 * XClearArea will generate a BadMatch error if called on InputOnly windows; 
 * this error is suppressed in Error.c.
 */
void
RecursiveRefresh(dpy, win)
    Display *dpy;
    Window win;
{
    int i, nchildren;
    Status s;
    Window root, parent;
    Window *childlist;

    XClearArea(dpy, win, 0, 0, 0, 0, True);
    s = XQueryTree(dpy, win, &root, &parent, &childlist, &nchildren);
    if (s == 0)
	return;
    for (i=0; i<nchildren; ++i) {
	RecursiveRefresh(dpy, childlist[i]);
    }
    if (nchildren > 0)
	XFree(childlist);
}


/*
 * RefreshFunc -- called when the "Refresh Screen" item has been selected on
 *	the olwm menu
 */
/*ARGSUSED*/	/* winInfo, menu, and idx args will be used later */
int 
RefreshFunc(dpy, winInfo, menu, idx)
Display	*dpy;
WinGeneric	*winInfo;
Menu	*menu;
int	idx;
{
    if (GRV.RefreshRecursively) {
	RecursiveRefresh(dpy, DefaultRootWindow(dpy));
    } else {
	Window	w;
	XSetWindowAttributes xswa;

	/* We create a window over the whole screen, map it,
	 * then destroy it.
	 */
	xswa.override_redirect = True;
	w = XCreateWindow(dpy, 
			  RootWindow(dpy, DefaultScreen(dpy)),
			  0, 0,
			  DisplayWidth(dpy,DefaultScreen(dpy)),
			  DisplayHeight(dpy,DefaultScreen(dpy)),
			  0,
			  DefaultDepth(dpy,DefaultScreen(dpy)), 
			  InputOutput, 
			  DefaultVisual(dpy,DefaultScreen(dpy)),
			  CWOverrideRedirect, 
			  &xswa);

	XMapRaised(dpy, w);
	XDestroyWindow(dpy, w);
    }
}

/***************************************************************************
* Properties
****************************************************************************/

#define WORKSPACEPROPS "props"

/*
 * PropertiesFunc -- called when the "Properties ..." item has been selected 
 * on the root menu.  REMIND: this and AppMenuFunc should be merged.
 */
/*ARGSUSED*/	/* dpy, winInfo, menu, and idx args will be used later */
int
PropertiesFunc(dpy,winInfo,menu,idx)
Display	*dpy;
WinGeneric	*winInfo;
Menu	*menu;
int	idx;
{
	execCommand(WORKSPACEPROPS);
}

/***************************************************************************
* Save Workspace
****************************************************************************/

static Window SaveWaiters[500];    /* REMIND: should dynamically allocate */
static int SaveCount = 0;
static FILE *SaveFile;

static char *backfilename = NULL;
static char *basefilename = NULL;
static char *tempfilename = NULL;

#define INITFILENAME ".openwin-init"

/*
 * Construct the names: "$HOME/.openwin-init" and "$HOME/.openwin-init.BAK",
 * and "$HOME/.openwin-init.TEMP" and set up pointers to them.
 */
static void
sw_makenames()
{
    char *home;
    char buffer[1024];

    if (basefilename != NULL)
	return;

    home = getenv("HOME");
    if (home == NULL)
	home = "";  /* REMIND - what to do here? */

    strcpy(buffer, home);
    strcat(buffer, "/");
    strcat(buffer, INITFILENAME);

    basefilename = MemNewString(buffer);

    strcat(buffer, ".BAK");
    backfilename = MemNewString(buffer);

    strcpy(buffer, basefilename);
    strcat(buffer, ".TEMP");
    tempfilename = MemNewString(buffer);
}


static void
sw_timeout(dpy)
Display *dpy;
{
    NoticeBox		    noticeBox;
#ifdef OW_I18N
static    wchar_t	    *buttons[] = { NULL };
static    wchar_t	    *strings;
#else OW_I18N
static    char		    *buttons[] = { "Cancel" };
static    char		    *strings[] = {
    "Save Workspace could not be completed, because",
    "some applications did not respond." 
    };
#endif OW_I18N

#ifdef DEBUG
    fprintf(stderr,"Save Workspace timeout!\n");
#endif /* DEBUG */
#ifdef OW_I18N
    if (buttons[0] == NULL)
    {
	/* STRING_EXTRACTION - The next text is button name
	   for notice window which has message as second text.
	 */
	buttons[0] = mbstowcsdup(gettext("Cancel"));
	strings = mbstowcsdup(gettext(

"Save Workspace could not be completed, because\n\
some applications did not respond."));

    }
#endif OW_I18N
    XBell(dpy, 100);
    /* set up noticeBox information */
    noticeBox.numButtons = 1;
    noticeBox.defaultButton = 0;
    noticeBox.buttonText = buttons;
#ifndef OW_I18N
    noticeBox.numStrings = 2;
#endif
    noticeBox.stringText = strings;
    noticeBox.boxX = -1;
    noticeBox.boxY = -1;

    (void) UseNoticeBox(dpy, DefaultScreen(dpy), &noticeBox );
    sw_interposer(dpy, NULL, NULL, NULL);
}

/*
 * sw_interposer - the interposition function for getting PropertyNotify
 * events on WM_COMMAND on the windows that were sent WM_SAVE_YOURSELF.
 */
static int
sw_interposer( dpy, event, wi, closure )
    Display *dpy;
    XEvent *event;
    WinGeneric *wi;
    void *closure;
{
    int i;
    Window temp;
    Bool result;
    Atom acttype;
    int actfmt;
    unsigned long nitems, remainder;
    char *data;
    NoticeBox	noticeBox;
#ifdef OW_I18N
    char *p;
static    wchar_t	*buttons[] = { NULL };
static    wchar_t	*strings;
#else
static    char	*buttons[] = { "Ok" };
static    char	*strings[] = { "Save Workspace complete." };
#endif


#ifdef OW_I18N
    if (buttons[0] == NULL)
    {
	/* STRING_EXTRACTION - The next text is button name
	   for notice window which has message as second text.
	 */
	buttons[0] = mbstowcsdup(gettext("Ok"));
	strings = mbstowcsdup(gettext("Save Workspace complete."));
    }
#endif
    if ( event == NULL ) {
	XUngrabKeyboard( dpy, CurrentTime );
	XUngrabPointer( dpy, CurrentTime );
	UninstallInterposer();
	TimeoutCancel();
	fclose(SaveFile);
	(void) unlink(tempfilename);
	return DISPOSE_DISPATCH;
    }

    if ( event->type == PropertyNotify &&
	 event->xproperty.atom == XA_WM_COMMAND &&
	 event->xproperty.state == PropertyNewValue )
    {
	for ( i=0; i<SaveCount; ++i ) {
	    if ( SaveWaiters[i] == event->xproperty.window )
		break;
	}
	if ( i == SaveCount )
	    return DISPOSE_DISPATCH;

	--SaveCount;
	temp = SaveWaiters[i];
	SaveWaiters[i] = SaveWaiters[SaveCount];
	SaveWaiters[SaveCount] = temp;

#ifdef OW_I18N
	result = XGetWindowProperty( dpy, event->xproperty.window,
		    XA_WM_COMMAND, 0L, 1000000L, False, AnyPropertyType,
		    &acttype, &actfmt, &nitems, &remainder, &data );
	if ( result != Success
	 || (acttype != XA_STRING && actfmt != 8)
	 || (acttype != AtomCompoundText && actfmt != 8))
	    return DISPOSE_DISPATCH;

	/* ignore zero-length properties */
	if ( nitems > 0 ) {
	    for ( p = data;  p < &data[nitems]; p++ )
		if ( *p == '\0' )
		    *p = ' ';
	    if (acttype == AtomCompoundText) {
		p = ctstombsdup(data);
		fputs(p, SaveFile);
		free(p);
	    }
	    else
		fputs(data, SaveFile);
	    fputs( " &\n", SaveFile );
	}
#else OW_I18N
	data = GetWindowProperty(dpy, event->xproperty.window, XA_WM_COMMAND,
		0L, 1000000L, XA_STRING, 8, &nitems, &remainder);
	if (data == NULL)
	    return DISPOSE_DISPATCH;

	/* ignore zero-length properties */
	if ( nitems > 0 ) {
	    for ( i=0; i<nitems; ++i )
		if ( data[i] == '\0' )
		    data[i] = ' ';
	    fputs( data, SaveFile );
	    fputs( " &\n", SaveFile );
	}
#endif OW_I18N
	XFree(data);

	if ( SaveCount == 0 ) {
	    XUngrabKeyboard( dpy, CurrentTime );
	    XUngrabPointer( dpy, CurrentTime );
	    UninstallInterposer();
	    TimeoutCancel();
	    fclose(SaveFile);

	    /* set up noticeBox information */
	    noticeBox.numButtons = 1;
	    noticeBox.defaultButton = 0;
	    noticeBox.buttonText = buttons;
#ifndef OW_I18N
	    noticeBox.numStrings = 1;
#endif
	    noticeBox.stringText = strings;
	    noticeBox.boxX = -1;
	    noticeBox.boxY = -1;

	    if (1 == UseNoticeBox(dpy, DefaultScreen(dpy), &noticeBox)) {
		/* they hit Cancel */
		/* actually, there's no longer a Cancel button ... */
		(void) unlink(tempfilename);
	    } else {
		/* they hit OK */
		(void) unlink(backfilename);
		(void) rename(basefilename, backfilename);
		(void) rename(tempfilename, basefilename);
	    }
	}

	return DISPOSE_USED;
    } else {
	return DISPOSE_DISPATCH;
    }
}

void *
sw_send(cli,dpy)
Client *cli;
Display	*dpy;
{
    if ( cli->protocols & SAVE_YOURSELF ) 
    {
	ClientSendProtocol(cli,AtomSaveYourself,LastEventTime);
	SaveWaiters[SaveCount++] = ClientPane(cli);
    }
    return NULL;
}

/*
 * SaveWorkspaceFunc - called when "Save Workspace" is selected
 * from the root menu.
 */
int
SaveWorkspaceFunc(dpy,winInfo,menu,idx)
    Display	*dpy;
    WinGeneric	*winInfo;
    Menu	*menu;
    int		idx;
{
    char		    *home;
    struct stat		    statbuf;
    NoticeBox		    noticeBox;
#ifdef OW_I18N
static    wchar_t	    *buttons[] = { NULL };
static    wchar_t	    *strings;
static	  wchar_t	    *strings2;
#else OW_I18N
static    char		    *buttons[] = { "Cancel" };
static    char		    *strings[] = {
    "Save Workspace cannot be performed, because no",
    "running applications understand the Save Workspace."
};
static	  char		    *strings2[] = {
    "Save Workspace cannot be performed, because",
    "your home directory is unwritable."
};
#endif OW_I18N

#ifdef OW_I18N
    if (buttons[0] == NULL)
    {
	/* STRING_EXTRACTION - The next text is button name for notice
	   window which has message as second (or third) text.
	 */
	buttons[0] = mbstowcsdup(gettext("Cancel"));
	strings = mbstowcsdup(gettext(

"Save Workspace cannot be performed, because no\n\
running applications understand the Save Workspace."));

	strings2 = mbstowcsdup(gettext(

"Save Workspace cannot be performed, because\n\
your home directory is unwritable."));

    }
#endif OW_I18N

    sw_makenames();

    SaveFile = fopen(tempfilename, "w");
    if ( SaveFile == NULL ) {
	noticeBox.numButtons = 1;
	noticeBox.defaultButton = 0;
	noticeBox.buttonText = buttons;
#ifndef OW_I18N
	noticeBox.numStrings = 2;
#endif
	noticeBox.stringText = strings2;
	noticeBox.boxX = -1;
	noticeBox.boxY = -1;

	(void) UseNoticeBox(dpy, DefaultScreen(dpy), &noticeBox );
	return 0;
    }

    /* tell all clients to save themselves */
    SaveCount = 0;
    ListApply(ActiveClientList, sw_send, dpy);

    if ( SaveCount > 0 ) {
	(void) fstat(fileno(SaveFile), &statbuf);
	(void) fchmod(fileno(SaveFile),
		      statbuf.st_mode | S_IXUSR | S_IXGRP | S_IXOTH );

	fputs( "#! /bin/sh\n", SaveFile );
	fprintf( SaveFile, "# %s - OpenWindows initialization script.\n",
		 INITFILENAME );
	fputs("# WARNING: This file is automatically generated.\n", SaveFile );
	fputs("#          Any changes you make here will be lost!\n",
	      SaveFile );

	XGrabKeyboard( dpy, NoFocusWin, False, GrabModeAsync, GrabModeAsync,
		       CurrentTime );
	XGrabPointer( dpy, NoFocusWin, False, ButtonPressMask,
		      GrabModeAsync, GrabModeAsync, None,
		      GRV.BusyPointer, CurrentTime );

	InstallInterposer( sw_interposer, (void *)0 );
	TimeoutRequest(GRV.SaveWorkspaceTimeout,sw_timeout,dpy);
    } else {
	/* set up noticeBox information */
	noticeBox.numButtons = 1;
	noticeBox.defaultButton = 0;
	noticeBox.buttonText = buttons;
#ifndef OW_I18N
	noticeBox.numStrings = 2;
#endif
	noticeBox.stringText = strings;
	noticeBox.boxX = -1;
	noticeBox.boxY = -1;

	(void) UseNoticeBox(dpy, DefaultScreen(dpy), &noticeBox );
	(void) unlink(tempfilename);
    }

    return 0;
}

/***************************************************************************
* Icon menu services
****************************************************************************/

/*
 * DoIconFullAction -- routine called for each window when a menu action is
 *      selected from the full frame menu.
 */
/*ARGSUSED*/    /* menu arg will be used later */
void
DoIconFullAction(dpy, winInfo, menu, idx)
Display *dpy;
WinGeneric *winInfo;
Menu    *menu;
int     idx;
{
	Client *cli = winInfo->core.client;
        WinPaneFrame *frameInfo = cli->framewin;
	WinPane *paneInfo = (WinPane *)frameInfo->fcore.panewin;
        XEvent clientEvent;
        Window w;
        int width, height;

#define FFA_OPEN                0
#define FFA_FULLSIZE            1
#define FFA_PROPERTIES          2
#define FFA_BACK                3
#define FFA_REFRESH             4
#define FFA_QUIT                5


        switch(idx)

        {
        case FFA_OPEN:
                StateIconNorm(cli);
                break;

        case FFA_FULLSIZE:
                StateIconNorm(cli);
                FrameFullSize(frameInfo);
                break;

        case FFA_PROPERTIES:
                /* send a ClientMessage of type WM_SHOW_PROPERTIES */
                clientEvent.xclient.type = ClientMessage;
                clientEvent.xclient.message_type = AtomShowProperties;
                clientEvent.xclient.format = 32;
                clientEvent.xclient.display = dpy;
                clientEvent.xclient.window = paneInfo->core.self;

                /* send the event */
                XSendEvent(dpy, clientEvent.xclient.window,
                           False, NoEventMask, &clientEvent);
                break;

        case FFA_BACK:
                /* lower the window */
                XLowerWindow(dpy, winInfo->core.self);
                break;

        case FFA_REFRESH:
                /* refresh the window.  We do this by creating a window on top
                 * of the window to refresh and then immediately destroy it
                 */
                w = XCreateSimpleWindow(dpy, paneInfo->core.self, 0, 0,
                                   paneInfo->core.width, paneInfo->core.height, 0,
                                   (Pixmap) 0, (Pixmap) 0);
                XMapRaised(dpy, w);
                XDestroyWindow(dpy, w);
                XFlush(dpy);
                break;

        case FFA_QUIT:
		ClientKill(cli,True);
                break;
        }
}

/* IconFullAction -- handle actions from the Icon menus */
int
IconFullAction(dpy, winInfo, menu, idx, pinnedMenu)
Display *dpy;
WinGeneric *winInfo;
Menu    *menu;
int     idx;
Bool    pinnedMenu;
{
	Client *cli = NULL;

        if (pinnedMenu || IsSelected(winInfo->core.client))
                while(cli = EnumSelections(cli))
		{
			if (cli->wmState == NormalState)
				FrameFullAction(dpy, cli->framewin, menu, idx);
			else
	                        DoIconFullAction(dpy, cli->iconwin, menu, idx);
		}
        else
        {
                DoIconFullAction(dpy, winInfo, menu, idx);
        }
}

#ifdef NOTDEF

/*
 * ListFullAction is obsolete.  It was used when menu items operated on the
 * selection.  It may someday be useful for Window Controls.
 */

/*
 * ListFullAction --  routine called when a menu action is selected
 *      from the full frame menu. We step through all the selected windows
 *      and perform the required action.
 */
ListFullAction(dpy, winInfo, menu, idx, pinnedMenu)
Display *dpy;
WinInfo *winInfo;
Menu    *menu;
int     idx;
Bool    pinnedMenu;
{
        WinInfo *tempInfo = (WinInfo *)0;

        if (pinnedMenu || IsSelected(winInfo))
                while(tempInfo = EnumSelections(tempInfo))
                        FrameFullAction(dpy, tempInfo, menu, idx, pinnedMenu);
        else
        {
                FrameFullAction(dpy, winInfo, menu, idx, pinnedMenu);
        }
}
#endif /* NOTDEF */


/*
 * FrameFullAction -- routine called for each window when a menu action is
 *      selected from the full frame menu.
 */
FrameFullAction(dpy, winInfo, menu, idx, pinnedMenu)
Display *dpy;
WinPaneFrame *winInfo;
Menu    *menu;
int     idx;
Bool    pinnedMenu;
{
#define FFA_CLOSE               0
#define FFA_FULLSIZE            1
#define FFA_PROPERTIES          2
#define FFA_BACK                3
#define FFA_REFRESH             4
#define FFA_QUIT                5

	Client *cli = winInfo->core.client;

        switch(idx)

        {
        case FFA_CLOSE:
		StateNormIcon(cli);
                break;

        case FFA_FULLSIZE:
		(winInfo->normfullsizefunc)(winInfo);
                break;

        case FFA_PROPERTIES:
		ClientShowProps(cli);
                break;

        case FFA_BACK:
                XLowerWindow(dpy, winInfo->core.self);
                break;

        case FFA_REFRESH:
		ClientRefresh(cli);
                break;

        case FFA_QUIT:
		ClientKill(cli, True);
                break;
        }
}


#ifdef NOTDEF
/*
 * ListLimAction is obsolete.  It was used when menu items operated on the
 * selection.  It may someday be useful for Window Controls.
 */

/*
 * ListLimAction --  routine called when a menu action is selected
 *      from the limited frame menu. We step through all the selected windows
 *      and perform the required action.
 */
ListLimAction(dpy, winInfo, menu, idx, pinnedMenu)
Display *dpy;
WinInfo *winInfo;
Menu    *menu;
int     idx;
Bool    pinnedMenu;
{
        WinInfo *tempInfo = (WinInfo *)0;

        if (pinnedMenu || IsSelected(winInfo))
                while(tempInfo = EnumSelections(tempInfo))
                        FrameLimAction(dpy, tempInfo, menu, idx, pinnedMenu);
        else
        {
                ClearSelections(dpy);
		/* REMIND the timestamp should maybe be more meaningful? */
                AddSelection(winInfo->core.client, TimeFresh(winInfo));
                FrameLimAction(dpy, winInfo, menu, idx, pinnedMenu);
        }
}
#endif /* NOTDEF */


/*
 * FrameLimAction -- routine called when a menu action is selected
 *      from the limited frame menu.
 */
/*ARGSUSED*/    /* menu arg will be used later */
FrameLimAction(dpy, winInfo, menu, idx, pinnedMenu)
Display *dpy;
WinPaneFrame *winInfo;
Menu    *menu;
int     idx;
Bool    pinnedMenu;
{
#define FLM_DISMISS             0
#define FLM_BACK                1
#define FLM_REFRESH             2
#define FLM_OWNER               3
	Client *cli = winInfo->core.client;
	Client *cliLead;

        switch(idx)
        {
        case FLM_DISMISS:
                /*
                 * REMIND
                 * This is probably redundant with DismissAction
                 */
                /* send a ClientMessage of type WM_DELETE_WINDOW */
                if (cli->groupmask == GROUP_DEPENDENT) /* make sure is pop-up */
                {
			ClientKill(cli,False);
                }
                break;

        case FLM_BACK:
                /* lower the window */
                XLowerWindow(dpy, winInfo->core.self);
                break;

        case FLM_REFRESH:
                /* refresh the window.  We do this by creating a window on top
                 * of the window to refresh and then immediately destroy it
                 */
		ClientRefresh(cli);
                break;

        case FLM_OWNER:
                /* find the owner of this pop-up */
		cliLead = GroupLeader(cli->groupid);
		if ((cliLead != NULL) && ((winInfo = cliLead->framewin) != NULL))
		{
                    XRaiseWindow(cli->dpy, winInfo->core.self);
                    XFlush(dpy);
                    FrameFlashTitleBar(winInfo);
		}
                break;
        }
}


#ifdef NOTDEF
/*
 * ListDismissAction is obsolete.  It was used when menu items operated on the
 * selection.  It may someday be useful for Window Controls.
 */

/*
 * ListDismissAction --  routine called when a menu action is selected
 *      from the dismiss menu. We step through all the selected windows
 *      and perform the required action.
 */
/*ARGSUSED*/    /* pinnedMenu arg will be used later */
ListDismissAction(dpy, winInfo, menu, idx, pinnedMenu)
Display *dpy;
WinInfo *winInfo;
Menu    *menu;
int     idx;
Bool    pinnedMenu;
{
        WinInfo *tempInfo = (WinInfo *)0;

        if (IsSelected(winInfo))
                while(tempInfo = EnumSelections(tempInfo))
                        DismissAction(dpy, tempInfo, menu, idx, pinnedMenu);
        else
        {
                ClearSelections(dpy);
		/* REMIND the timestamp should be more meaningful ? */
                AddSelection(winInfo->cli, TimeFresh(winInfo));
                DismissAction(dpy, winInfo, menu, idx, pinnedMenu);
        }
}
#endif /* NOTDEF */

/*
 * DismissAction -- called when Dismiss submenu is entered and an item
 *      selected.
 */
/*ARGSUSED*/    /* menu arg will be used later */
DismissAction(dpy, winInfo, menu, idx, pinnedMenu)
Display *dpy;
WinPaneFrame *winInfo;
Menu    *menu;
int     idx;
Bool    pinnedMenu;
{
	Client *cli = winInfo->core.client;
	Client *cliLeader;
	List *l;

#define DA_THIS         0
#define DA_POPUPS       1

        switch(idx)
        {
        case DA_THIS:
                if (cli->groupmask == GROUP_DEPENDENT) /* ensure this is a popup */
                {
			ClientKill(cli,False);
                }
                break;

        case DA_POPUPS:
                /*
                 * Find all pop-ups associated with this window, or with
                 * this window's leader, and send them a WM_DELETE_WINDOW
                 * message.
                 */
		GroupApply(cli->groupid, ClientKill, (void *)False, GROUP_DEPENDENT);
                break;
        }
}
