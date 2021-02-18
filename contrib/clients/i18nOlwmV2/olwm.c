/*
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) olwm.c 50.12 91/08/26 Crucible";

#ifdef OW_I18N
#include "i18n.h"
#endif
#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <strings.h>

extern char *strtok();		/* not defined in strings.h */

#include <sys/time.h>
#include <sys/types.h>

#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
#ifdef OW_I18N
#include <sys/param.h>		/* for MAXPATHLEN */
#endif

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>

#ifdef OW_I18N
#include "ollocale.h"
#endif
#include "events.h"
#include "mem.h"
#include "olwm.h"
#include "win.h"
#include "menu.h"
#include "defaults.h"
#include "resources.h"
#include "globals.h"
#include "group.h"
#include "slots.h"

/*
 * Externals
 */
extern void	EventLoop();
extern void	UpdateModifierMap();

extern Atom	AtomChangeState;
extern Atom	AtomClass;
extern Atom	AtomColorMapWindows;
extern Atom	AtomDecorAdd;
extern Atom	AtomDecorClose;
extern Atom	AtomDecorDelete;
extern Atom	AtomDecorFooter;
extern Atom	AtomDecorHeader;
extern Atom	AtomDecorPin;
extern Atom	AtomDecorResize;
extern Atom	AtomDeleteWindow;
extern Atom	AtomLeftFooter;
extern Atom	AtomLength;
extern Atom	AtomListLength;
extern Atom	AtomMenuFull;
extern Atom	AtomMenuLimited;
extern Atom	AtomMultiple;
extern Atom	AtomNone;
extern Atom	AtomOlwmTimestamp;
extern Atom	AtomPinIn;
extern Atom	AtomPinOut;
extern Atom	AtomProtocols;
extern Atom	AtomPushpinState;
extern Atom	AtomRightFooter;
extern Atom	AtomSaveYourself;
extern Atom	AtomShowProperties;
extern Atom	AtomTakeFocus;
extern Atom	AtomTargets;
extern Atom	AtomTimestamp;
extern Atom	AtomWMApplyProperties;
extern Atom	AtomWMResetProperties;
extern Atom	AtomWMState;
extern Atom	AtomWTBase;
extern Atom	AtomWTCmd;
extern Atom	AtomWTHelp;
extern Atom	AtomWTNotice;
extern Atom	AtomWTOther;
extern Atom	AtomWTProp;
extern Atom	AtomWinAttr;
extern Atom	AtomWindowBusy;
extern Atom	AtomDfltBtn;
#ifdef OW_I18N
extern Atom	AtomCompoundText;
extern Atom	AtomDecorIMStatus;
extern Atom	AtomLeftIMStatus;
extern Atom	AtomRightIMStatus;
#endif OW_I18N

/*
 * Globals
 */
char	*ProgramName;
char	*AppName;		/* strictly last component of ProgramName */
int	FrameFullAction(), Exit(), FrameLimAction(), DismissAction();
int 	IconFullAction();
extern Window	NoFocusWin;
#ifdef OW_I18N
/*  cm_open, cm_put and cm_close are just place holder.
 *  olwm never need to reference it.
 *  The reason we need this is sun's R5 version of Xlib will
 *  reference them.
 */
int
cm_open(cm_id, dummy1, dummy2)
	int	cm_id;
	int	*dummy1, *dummy2;
{
	return(0);
}
cm_close(lang_eng, session_id)
	int	lang_eng;
	int	session_id;
{
	return(0);
}
int
cm_put(lang_eng, session_id, dummy)
	int	lang_eng;
	int	session_id; 
	int	*dummy;
{
	return(0);
}
#endif /* OW_I18N */

/* The Properties button is currently disabled in all of these menus */
#ifdef OW_I18N
/*
 * OW_I18N: Take out all of the menu string from following
 * intialization value to allow runtime(startup time) switch.
 */
Button IconFullButtons[] = {
        { NULL, False, Enabled, {IconFullAction, NULL} },
        { NULL, False, Enabled, {IconFullAction, NULL} },
        { NULL, False, Disabled, {IconFullAction, NULL} },
        { NULL, False, Enabled, {IconFullAction, NULL} },
        { NULL, False, Enabled, {IconFullAction, NULL} },
        { NULL, False, Enabled, {IconFullAction, NULL} },
        };
Menu IconFullMenu = { NULL, IconFullButtons, 6, 0, False, (FuncPtr)MakeMenu};

Button FrameNormButtons[] = {
        { NULL, False, Enabled, {FrameFullAction, NULL} },
        { NULL, False, Enabled, {FrameFullAction, NULL} },
        { NULL, False, Disabled, {FrameFullAction, NULL} },
        { NULL, False, Enabled, {FrameFullAction, NULL} },
        { NULL, False, Enabled, {FrameFullAction, NULL} },
        { NULL, False, Enabled, {FrameFullAction, NULL} },
        };
Menu FrameNormMenu = { NULL, FrameNormButtons, 6, 0, False, (FuncPtr)MakeMenu};

Button FrameFullButtons[] = {
        { NULL, False, Enabled, {FrameFullAction, NULL} },
        { NULL, False, Enabled, {FrameFullAction, NULL} },
        { NULL, False, Disabled, {FrameFullAction, NULL} },
        { NULL, False, Enabled, {FrameFullAction, NULL} },
        { NULL, False, Enabled, {FrameFullAction, NULL} },
        { NULL, False, Enabled, {FrameFullAction, NULL} },
        };
Menu FrameFullMenu = { NULL, FrameFullButtons, 6, 0, False, (FuncPtr)MakeMenu};

Button DismissButtons[] = {
        { NULL, False, Enabled, {DismissAction, NULL} },
        { NULL, False, Enabled, {DismissAction, NULL} },
	};
Menu DismissMenu = { NULL, DismissButtons, 2, 0, False, NULL};
	
Button FrameLimButtons[] = {
	{ NULL, True, Enabled, {(FuncPtr) 0, (void *) &DismissMenu} },
        { NULL, False, Enabled, {FrameLimAction, (void *)0} },
        { NULL, False, Enabled, {FrameLimAction, (void *)0} },
        { NULL, False, Enabled, {FrameLimAction, (void *)0} },
	};
Menu FrameLimMenu = { NULL, FrameLimButtons, 4, 0, False, (FuncPtr)MakeMenu};

#else OW_I18N

Button IconFullButtons[] = {
        { "Open", False, Enabled, {IconFullAction, NULL} },
        { "Full Size", False, Enabled, {IconFullAction, NULL} },
        { "Properties", False, Disabled, {IconFullAction, NULL} },
        { "Back", False, Enabled, {IconFullAction, NULL} },
        { "Refresh", False, Enabled, {IconFullAction, NULL} },
        { "Quit", False, Enabled, {IconFullAction, NULL} },
        };
Menu IconFullMenu = { "Window", IconFullButtons, 6, 0, False, (FuncPtr)MakeMenu};

Button FrameNormButtons[] = {
        { "Close", False, Enabled, {FrameFullAction, NULL} },
        { "Normal Size", False, Enabled, {FrameFullAction, NULL} },
        { "Properties", False, Disabled, {FrameFullAction, NULL} },
        { "Back", False, Enabled, {FrameFullAction, NULL} },
        { "Refresh", False, Enabled, {FrameFullAction, NULL} },
        { "Quit", False, Enabled, {FrameFullAction, NULL} },
        };
Menu FrameNormMenu = { "Window", FrameNormButtons, 6, 0, False, (FuncPtr)MakeMenu};

Button FrameFullButtons[] = {
        { "Close", False, Enabled, {FrameFullAction, NULL} },
        { "Full Size", False, Enabled, {FrameFullAction, NULL} },
        { "Properties", False, Disabled, {FrameFullAction, NULL} },
        { "Back", False, Enabled, {FrameFullAction, NULL} },
        { "Refresh", False, Enabled, {FrameFullAction, NULL} },
        { "Quit", False, Enabled, {FrameFullAction, NULL} },
        };
Menu FrameFullMenu = { "Window", FrameFullButtons, 6, 0, False, (FuncPtr)MakeMenu};

Button DismissButtons[] = {
        { "This Window", False, Enabled, {DismissAction, NULL} },
        { "All Pop-ups", False, Enabled, {DismissAction, NULL} },
	};
Menu DismissMenu = { NULL, DismissButtons, 2, 0, False, NULL};
	
Button FrameLimButtons[] = {
	{ "Dismiss", True, Enabled, {(FuncPtr) 0, (void *) &DismissMenu} },
        { "Back", False, Enabled, {FrameLimAction, (void *)0} },
        { "Refresh", False, Enabled, {FrameLimAction, (void *)0} },
        { "Owner?", False, Enabled, {FrameLimAction, (void *)0} },
	};
Menu FrameLimMenu = { "Window", FrameLimButtons, 4, 0, False, (FuncPtr)MakeMenu};
#endif OW_I18N

/* Current Display/Screen */
Display	*DefDpy;
int	DefScreen;

static void	parseCommandline();
/* Note that changes to RMTable instance names must also be made here !! */
static	XrmOptionDescRec	optionTable[] = {
	{ "-display",		".display",	
		XrmoptionSepArg, (caddr_t) NULL },
	{ "-f",			".inputFocusStyle", 
		XrmoptionNoArg, (caddr_t) "follow" },
	{ "-follow",		".inputFocusStyle", 
		XrmoptionNoArg, (caddr_t) "follow" },
	{ "-c", 		".inputFocusStyle",	
		XrmoptionNoArg, (caddr_t) "select" },
	{ "-click", 		".inputFocusStyle",	
		XrmoptionNoArg, (caddr_t) "select" },
	{ "-2d", 		".use3D",	
		XrmoptionNoArg, (caddr_t) "False" },
	{ "-3d", 		".use3D",	
		XrmoptionNoArg, (caddr_t) "True" },
	{ "-parent", 		".reparentOK",	
		XrmoptionNoArg, (caddr_t) "False" },
	{ "-orphans", 		".debug.print.orphans", 
		XrmoptionNoArg, (caddr_t) "True" },
	{ "-all", 		".debug.print.all", 
		XrmoptionNoArg, (caddr_t) "True" },
	{ "-synchronize",	".debug.synchronize",
		XrmoptionNoArg, (caddr_t) "True" },
	{ "-debug",	"*Debug",	XrmoptionNoArg,	(caddr_t) "True" },
	{ "-name",		".name",	
		XrmoptionSepArg, (caddr_t) NULL },
	{ "-bg",	"*Background",	XrmoptionSepArg, (caddr_t) NULL },
	{ "-background", "*Background",	XrmoptionSepArg, (caddr_t) NULL },
	{ "-fg",	"*Foreground",	XrmoptionSepArg, (caddr_t) NULL },
	{ "-foreground","*Foreground",	XrmoptionSepArg, (caddr_t) NULL },
	{ "-bd",	"*Background",	
		XrmoptionSepArg, (caddr_t) NULL },
	{ "-bordercolor",	"*Background",	
		XrmoptionSepArg, (caddr_t) NULL },
	{ "-fn",	"*TitleFont",	XrmoptionSepArg, (caddr_t) NULL },
	{ "-font",	"*TitleFont",	XrmoptionSepArg, (caddr_t) NULL },
	{ "-xrm",	NULL,		XrmoptionResArg, (caddr_t) NULL },
#ifdef OW_I18N
	{ "-basiclocale",	"*basicLocaleCL",
		XrmoptionSepArg, (caddr_t) NULL },
	{ "-displaylang",	"*displayLangCL",
		XrmoptionSepArg, (caddr_t) NULL },
	{ "-inputlang",		"*inputLangCL",
		XrmoptionSepArg, (caddr_t) NULL },
	{ "-numeric",		"*numericCL",
		XrmoptionSepArg, (caddr_t) NULL },
	{ "-dateformat",	"*dateFormatCL",
		XrmoptionSepArg, (caddr_t) NULL },
#endif
};
#define OPTION_TABLE_ENTRIES (sizeof(optionTable)/sizeof(XrmOptionDescRec))

static void	parseResourceDefaults();
static void	initWinClasses();

/* Child Process Handling */
void HandleChildSignal();
void ReapChildren();
int DeadChildren = 0;

static char	**argVec;
char		**Envp;

/*
 * main	-- parse arguments, perform initialization, call event-loop
 *
 * The OpenLook window manager was written before the OpenLook
 * spec. was completed.  Because of this, some of it works in
 * an awkward way.  Specifically, the way a window's focus and
 * select state is shown.
 *
 * For example, when a window is focused in click-to-focus mode
 * the header gets highlighted, (black background, white text).
 * As it was written, the titlebar window sits just inside of the 
 * resize corners.  Because the OpenLook spec. requires the header to 
 * be highlighted in line with the resize corners, blacking in the
 * the background of the titlebar window is one pixel short in each
 * direction of being a big enough highlighted area.  We can't make
 * the titlebar bigger because it would then overlap the resize corners.
 * Now that the spec. is complete, OLWM should be restructured.
 *
 * Overview:
 *
 *	Upon startup OLWM reparents all the client windows unless
 *	the user has told it otherwise (-p option).
 * 	OLWM creates a frame which will contain the client window (the pane)
 *  	and the decoration windows (resize corners, titlebar).  The
 *	titlebar window contains the pushpin, if any.
 *
 *	After creating the decorations for the windows OLWM sits
 * 	in a forever loop getting the next event and calling the
 * 	corresponding window's event handling routine.
 *
 *	Each window has associated with it an event handling 
 * 	routine (EventFrame, EventPushPin) which the main EventLoop
 *	will call when an event happens in that window.  The event
 *	handling routine will call the proper routines to move a 
 *	window, create a menu, etc.
 */
main( argc, argv, envp )
int	argc;
char	**argv;
char	**envp;
{
	int			ii;
	int			ErrorHandler();
	XSetWindowAttributes	attributes;
	int			ExitOLWM();
	Bool			changeEnvp = False;
	char			**updateEnvp();
	XrmDatabase		commandlineDB = NULL;
	XrmValue		rmValue;
	XrmRepresentation	rmType;
	char			name[MAX_NAME];
	char			class[MAX_NAME];
	WinGeneric		*winroot;
#ifdef OW_I18N
	char			*OpenWinHome;
	char			locale_dir[MAXPATHLEN+1];
	extern char		*getenv();
#endif

#ifdef MALLOCDEBUG
	malloc_debug(MALLOCDEBUG);
#endif /* MALLOCDEBUG */

#ifdef OW_I18N
	/*
	 * Even in the OW_I18N1.0 (first release) we might need the
	 * dynamic locale change for window manager, since window
	 * manager is usually never re-start again in one sesstion.
	 * But leave static for now.
	 */
	/*
	 * We are setting the locale (issuing the setlocale) by
	 * EffectOLLC() function, but we need to call setlocale here
	 * to handle command line argument with certain locale.
	 * FIX_ME! But may not work well, because we did not touch the
	 * Xlib function XrmParseCommand().
	 */
	if (setlocale(LC_ALL, "") == NULL)
	{
		/*
		 * FIX_ME: Following message may be misleading,
		 * because setlocale will retrieve more than just a
		 * $LANG environment variable (such as $LC_CTYPE).
		 * Also, later if the resource DB has a locale
		 * infomation, we will use it. Hence, this setting is
		 * may be only for the command line processing.
		 */
		fprintf(stderr, gettext("\
olwm: Warning: '%s' is invalid locale, use 'C' locale.\n"),
			getenv("LANG"));
		(void) setlocale(LC_ALL, "C");
	}
	if ((OpenWinHome = getenv("OPENWINHOME")) != 0)
		(void) strcpy(locale_dir, OpenWinHome);
	else
		(void) strcpy(locale_dir, "/usr");
	(void) strcat(locale_dir, "/lib/locale");
	bindtextdomain("olwm_messages", locale_dir);
	textdomain("olwm_messages");
	GRVLCInit();
#endif

	ProgramName = argv[0];
	argVec = argv;

	/* Set up a signal handler so that if we are killed we put
	 * the windows back into a usable state.  Specifically we
	 * need to reset the focus so they will be able to get input.
	 */
	signal(SIGHUP, ExitOLWM);
	signal(SIGINT, ExitOLWM);
	signal(SIGTERM, ExitOLWM);
	signal(SIGCHLD, HandleChildSignal);

	/* initialize the resource manager */
	XrmInitialize();

	/* parse the command line arguments into local tmp DB */
	parseCommandline( &argc, argv, &commandlineDB );

	SetNameTag(NULL, RM_PREFIX, NULL, False);

	/* check to see if display was specified on command line */
	MakeRMName( name, RM_DISPLAY );
	MakeRMClass( class, RM_DISPLAY );
	
	if ( XrmGetResource( commandlineDB, name, class, &rmType, &rmValue ) )
	{
		GRV.DisplayName = MemNewString( rmValue.addr );
		/* need to change display in Envp for commands invoked */
		Envp = updateEnvp( envp, "DISPLAY", GRV.DisplayName );
	}
	else
	{
		GRV.DisplayName = NULL;
		Envp = envp;
	}

	/* Open the display connection. */
	if ((DefDpy = XOpenDisplay(GRV.DisplayName)) == NULL)
	{
#ifdef OW_I18N
		fprintf( stderr, gettext("%s: cannot connect to %s\n"),
#else
		fprintf( stderr, "%s: cannot connect to %s\n",
#endif
			ProgramName, GRV.DisplayName );
		exit( -1 );
	}

	DefScreen = DefaultScreen( DefDpy );

	PreInitGraphics( DefDpy );

	/* Get the modifier mapping from the server. */
	UpdateModifierMap(DefDpy);

	/* put all resources into global OlwmDB and set olwm variables */
	GetDefaults(DefDpy, commandlineDB);

	/* Set up the error handling function. */
	XSetErrorHandler(ErrorHandler);

	/* this can be useful for debuging strange client behaivior */
	if (GRV.Synchronize)
		XSynchronize(DefDpy, True);

	/* Tell server we need to get mapping requests.
	 * ErrorSensitive is used to check for the possibility
	 * of another window manager running, normally XSelectInput
	 * generates warnings.
	 */
#ifdef OW_I18N
	ErrorSensitive(
		  gettext("Perhaps there is another window manager running?"));
#else
	ErrorSensitive("Perhaps there is another window manager running?");
#endif
	XSelectInput( DefDpy, RootWindow(DefDpy, DefScreen),
			KeyPressMask | SubstructureRedirectMask |
			ButtonPressMask | ButtonReleaseMask |
			Button1MotionMask | Button2MotionMask |
			EnterWindowMask | PropertyChangeMask );
	XSync(DefDpy, False);
	ErrorInsensitive(DefDpy);

	/* set workspace color (not an update - first time) */
	UpdWorkspace( DefDpy, RM_WORKSPACECOLOR, False );

	/* Init graphics stuff. */
	InitGraphics( DefDpy );

	/* Initialize the atoms we will need to communicate with
	 * Open Look clients.
	 */
	InitAtoms(DefDpy);

	/* Initialize the database. */
	WIInit( DefDpy );

	/* Initialise all types of window classes */
	initWinClasses(DefDpy);

	/* initialise the group module */
	GroupInit();

	/* initialise the icon-slot module */
	SlotInit(DefDpy);

	/*
	 * Force child processes to disinherit the TCP file descriptor.
	 * This helps shell commands forked and exec'ed from menus
	 * to work properly.
	 */
	if (fcntl(ConnectionNumber(DefDpy), F_SETFD, 1) == -1)
	{
#ifdef OW_I18N
		perror( gettext("olwm: child cannot disinherit TCP fd") );
#else
		perror( "olwm: child cannot disinherit TCP fd" );
#endif
		exit( -1 );
	}

	/* Add the root window to the window database. */
	winroot = MakeRoot(DefDpy, NULL);

	/* initialize colormap focus stuff */
	ColorFocusInit(DefDpy, winroot);

	/*
	 * Make an input only window that is invisible.
	 * This window will have the focus when no client
	 * window has the focus.  This is done so olwm
	 * can beep at the user when they hit a key while
	 * no window has the input focus.
	 */
	MakeNoFocus(DefDpy, winroot);

	/* Set the focus to the NoFocusWin.
	 * 	(RevertToNone is cast from long to 
	 *	int - lint will warn) 
	 */
	XSetInputFocus(DefDpy, NoFocusWin, RevertToParent, CurrentTime);

	if (GRV.ReparentFlag)
		ReparentTree(DefDpy);

	/*
	 * The following code is temporary, being used to generate the
	 * menus on the frame.
	 */
	InitMenus(DefDpy);
#ifdef OW_I18N
	EffectOLLC(NULL);
	/*
	 * Most likely above EffectOLLC() call the WindowMenuCreate()
	 * and UserMenuCreate(), but not guaranteed!
	 */
	WindowMenuCreate(DefDpy);
	UserMenuCreate(DefDpy);
#else OW_I18N
	MenuCreate(DefDpy, &IconFullMenu);
	MenuCreate(DefDpy, &FrameNormMenu);
	MenuCreate(DefDpy, &FrameFullMenu);
	MenuCreate(DefDpy, &FrameLimMenu);
	MenuCreate(DefDpy, &DismissMenu);
	InitUserMenu(DefDpy);
#endif OW_I18N

	if (GRV.Beep != BeepNever)
	    XBell(DefDpy, 100);

	EventLoop( DefDpy );

	/*NOTREACHED*/
}


/* 
 * parseCommandline - parse the command line arguments into a resource
 *	database
 */
static void
parseCommandline( argc, argv, tmpDB )
int		*argc;
char		*argv[];
XrmDatabase	*tmpDB;
{
	char	*p, *fullName, *saveName;

	/* resource instance name (aka AppName) should be simply the name of 
	 * the program, not the full pathname
	 */
	fullName = MemNewString( ProgramName );
	saveName = strtok( fullName, "/" );
	for( p = strtok(NULL, "/"); p != NULL; p = strtok(NULL, "/") )
			saveName = p;
	if ( !strcmp( saveName, ProgramName ) )
		AppName = ProgramName;
	else
		AppName = MemNewString( saveName );
	MemFree( fullName );

	/* REMIND - if AppName is used when parsing command line options,
	 * how will -name arg override it for other command line options given?
	 */
	XrmParseCommand( tmpDB, optionTable, OPTION_TABLE_ENTRIES,
			 AppName, argc, argv );

	/* check to see if there are any arguments left unparsed */
	if ( *argc != 1 )
	{
		/* check to see if it's -help */
		if ( argv[1][0] == '-' && argv[1][1] == 'h' )
#ifdef OW_I18N
			/* STRING_EXTRACTION - The next two text will
			   concatenate to make part of error message
			   (usage).
			 */
			usage(gettext("Command line arguments accepted"),
			      gettext("are:"));
#else
			usage("Command line arguments accepted", "are:");
#endif
		else
#ifdef OW_I18N
			/* STRING_EXTRACTION - The next two text will
			   concatenate to make part of error message
			   (usage).
			 */
			usage(gettext("Unknown argument(s)"),
			      gettext("encountered"));
#else
			usage("Unknown argument(s)", "encountered");
#endif
	}
}


/*
 * InitAtoms -- initialize the atoms needed to communicate with Open
 *	Look clients
 */
static
InitAtoms(dpy)
Display	*dpy;
{
	/* ICCCM specific flags */
	AtomColorMapWindows = XInternAtom(dpy, "WM_COLORMAP_WINDOWS", False);
	AtomWMState = XInternAtom(dpy, "WM_STATE", False);
	AtomChangeState = XInternAtom(dpy, "WM_CHANGE_STATE" , False);
	AtomProtocols = XInternAtom(dpy, "WM_PROTOCOLS" , False);
	AtomTakeFocus = XInternAtom(dpy, "WM_TAKE_FOCUS" , False);
	AtomSaveYourself = XInternAtom(dpy, "WM_SAVE_YOURSELF" , False);
	AtomDeleteWindow = XInternAtom(dpy, "WM_DELETE_WINDOW" , False);
	AtomClass = XInternAtom(dpy, "WM_CLASS", False);
#ifdef OW_I18N
	AtomCompoundText = XInternAtom(dpy, "COMPOUND_TEXT" , False);
#endif

	/* OpenLook specific flags */
	AtomWinAttr = XInternAtom(dpy, "_OL_WIN_ATTR" , False);
	AtomPushpinState = XInternAtom(dpy, "_OL_PIN_STATE" , False);
	AtomWindowBusy = XInternAtom(dpy, "_OL_WIN_BUSY" , False);
	AtomLeftFooter = XInternAtom(dpy, "_OL_WINMSG_ERROR" , False);
	AtomRightFooter = XInternAtom(dpy, "_OL_WINMSG_STATE" , False);
	AtomShowProperties = XInternAtom(dpy, "_OL_SHOW_PROPS" , False);
	AtomWMApplyProperties = XInternAtom(dpy, "_OL_PROPS_APPLY" , False);
	AtomWMResetProperties = XInternAtom(dpy, "_OL_PROPS_RESET" , False);
	AtomPinOut = XInternAtom(dpy, "_OL_PIN_OUT" , False);
	AtomDecorResize = XInternAtom(dpy, "_OL_DECOR_RESIZE" , False);
	AtomWTBase = XInternAtom(dpy, "_OL_WT_BASE" , False);
	AtomDecorFooter = XInternAtom(dpy, "_OL_DECOR_FOOTER" , False);
	AtomDecorAdd = XInternAtom(dpy, "_OL_DECOR_ADD" , False);
	AtomDecorDelete = XInternAtom(dpy, "_OL_DECOR_DEL" , False);
	AtomDecorPin = XInternAtom(dpy, "_OL_DECOR_PIN" , False);
	AtomWTProp = XInternAtom(dpy, "_OL_WT_PROP" , False);
	AtomWTCmd = XInternAtom(dpy, "_OL_WT_CMD" , False);
	AtomPinIn = XInternAtom(dpy, "_OL_PIN_IN" , False);
	AtomNone = XInternAtom(dpy, "_OL_NONE" , False);
	AtomWTNotice = XInternAtom(dpy, "_OL_WT_NOTICE" , False);
	AtomMenuFull = XInternAtom(dpy, "_OL_MENU_FULL" , False);
	AtomDecorHeader = XInternAtom(dpy, "_OL_DECOR_HEADER" , False);
	AtomWTHelp = XInternAtom(dpy, "_OL_WT_HELP" , False);
	AtomMenuLimited = XInternAtom(dpy, "_OL_MENU_LIMITED" , False);
	AtomDecorClose = XInternAtom(dpy, "_OL_DECOR_CLOSE" , False);
	AtomWTOther = XInternAtom(dpy, "_OL_WT_OTHER" , False);
	AtomOlwmTimestamp = XInternAtom(dpy,"_OLWM_TIMESTAMP",False);
	AtomDfltBtn = XInternAtom(dpy, "_OL_DFLT_BTN", False);
#ifdef OW_I18N
	AtomDecorIMStatus = XInternAtom(dpy, "_OL_DECOR_IMSTATUS", False);
	AtomLeftIMStatus = XInternAtom(dpy, "_OL_WINMSG_IMSTATUS", False);
	AtomRightIMStatus = XInternAtom(dpy, "_OL_WINMSG_IMPREEDIT", False);
#endif OW_I18N

	/* ICCCM selection atoms */
	AtomTargets = XInternAtom(dpy,"TARGETS",False);
	AtomMultiple = XInternAtom(dpy,"MULTIPLE",False);
	AtomTimestamp = XInternAtom(dpy,"TIMESTAMP",False);
	AtomListLength = XInternAtom(dpy,"LIST_LENGTH",False);
	AtomLength = XInternAtom(dpy,"LENGTH",False);
}

static void
initWinClasses(dpy)
Display *dpy;
{
#ifdef DEBUG
	printf("initialising classes\n");
#endif
	FrameInit(dpy);
	IconInit(dpy);
	ResizeInit(dpy);
	ColormapInit(dpy);
	ButtonInit(dpy);
	BusyInit(dpy);
	MenuInit(dpy);
	RootInit(dpy);
	NoFocusInit(dpy);
	PushPinInit(dpy);
	PaneInit(dpy);
	IconPaneInit(dpy);
}


/*
 * Exit -- exit routine called by menus.
 *	Kill all running applications.
 */
Exit(dpy)
Display	*dpy;
{
	extern void *ClientKill();
	ListApply(ActiveClientList, ClientKill, True);
	XSync(dpy, True);
	exit(0);
	/*NOTREACHED*/
}


static void
cleanup()
{
	extern void *UnparentClient();

	/* Reset the input focus so it will be
	 * RevertToRoot instead of RevertToNone.
	 * If we don't do this, when OLWM exits
	 * the user won't be able to get any client
	 * window to get the focus.
	 * 	(RevertToPointerRoot is cast from long to 
	 *	int - lint will warn)
	 */
	XSetInputFocus(DefDpy, PointerRoot, RevertToPointerRoot,CurrentTime);

	/*
	 * Go through the list of windows.  Unmap all icons that are on the
	 * screen.  Reparent all windows back to the root, suitably offset
	 * according to their window-gravities.  Also remap all non-withdrawn
	 * windows, and remove all Withdrawn windows from the save-set (so
	 * they don't get remapped.  REMIND: We have to do this because
	 * Withdrawn windows are still left reparented inside the frame; this
	 * shouldn't be the case.
	 */
	ListApply(ActiveClientList,UnparentClient,NULL);

	/* Re-install the default colormap. */
	InstallColormap(DefDpy, WIGetInfo(DefaultRootWindow(DefDpy)));

	/* If we set the root's background, restore it. */
	if (GRV.WorkspaceColor == NULL || *(GRV.WorkspaceColor) != '\0') {
	    XSetWindowBackgroundPixmap(DefDpy, DefaultRootWindow(DefDpy),
				       None);
	    XClearWindow(DefDpy, DefaultRootWindow(DefDpy));
	}

	XSync(DefDpy, True);
}


/* Clean up and then re-exec argv. */
int
RestartOLWM()
{
    cleanup();
    execvp(argVec[0], argVec);
    ErrorGeneral("cannot restart");
    /*NOTREACHED*/
}


/* Clean up and then exit. */
int
ExitOLWM()
{
    cleanup();
    exit(0);
}


/*
 * HandleChildSignal - keep track of children that have died
 */
static void
HandleChildSignal()
{
	++DeadChildren;
}

/*
 * ReapChildren - wait() for all dead child processes
 */
void
ReapChildren()
{
	int oldmask;
	int pid;
	union wait status;

	oldmask = sigblock(sigmask(SIGCHLD));
	while (DeadChildren>0)
	{
		pid = wait3(&status, WNOHANG, (struct rusage *)0);
		if (pid == 0)
		{
#ifdef OW_I18N
			fputs(
			  gettext("olwm: got SIGCHLD but found no children\n"),
			  stderr);
#else
			fputs("olwm: got SIGCHLD but found no children\n", stderr);
#endif
			DeadChildren = 0;
			break;
		}
		if (WIFSTOPPED(status))
		{
			kill(pid, SIGKILL);
		}
		--DeadChildren;
	}
	(void) sigsetmask(oldmask);
}

/*
 * usage(s1, s2)	-- print informative message regarding usage
 */
static
usage(s1, s2)
char	*s1, *s2;
{
    fprintf(stderr, "%s %s\n", s1, s2);
    fprintf(stderr,
	"usage: %s [-2d] [-3d] [-click] [-follow] [-parent]\n", ProgramName);
    fputs(
#ifdef OW_I18N
	/* STRING_EXTRACTION - do not translate "-display" ,"-name"
	   and "-xrm", becuase those are namne of command line option.
	 */
	gettext("\t[-display <display>] [-name <classname>] [-xrm <rsrcstring>]\n"),
#else
	"\t[-display <display>] [-name <classname>] [-xrm <rsrcstring>]\n",
#endif
	stderr);
    fputs("\t[-all] [-debug] [-orphans] [-synchronize]\n", stderr);
    exit(1);
}

/*
 * updateEnvp - modify environment vector
 *
 * Purpose - Duplicate environment vector and set the named environment 
 *	     variable to the value passed.  Notice that space is created 
 *	     inside this routine for the new environment vector, but not freed.
 *
 * Input:
 *	envp	 - current environment vector
 *	
 *	variable - environment variable to be changed
 *
 *	newValue - new value for the variable
 *
 * Returns:
 *	New environment vector.
 */
static char **
updateEnvp( envp, variable, newValue )
char	**envp;
char	*variable;
char	*newValue;
{
	int	count;		/* counter for number of env variables */
	int	numVariables;
	char	**newEnvp;
	char	*newSetting;

	/* create new string to be put into environment */
	/* need space for null at end, and "=" in the middle */
	newSetting = (char *)MemAlloc( (unsigned)(strlen( variable ) 
					+ strlen( newValue ) + 2 )
				      );
	sprintf( newSetting, "%s=%s", variable, newValue );

	/* count existing env vector entries and make pointers in newEnvp */
	for ( count = 0 ; envp[count] != NULL ; count++ )
		;

	/* number of variables in env vector */
	numVariables = count;

	/* make space for numVariables + 2 pointers in newEnvp 
	 * (+ 1 for the NULL at end and + 1 in case the variable being 
	 * changed isn't already in the environment)
	 */ 
	newEnvp = (char **)MemAlloc( (unsigned)(
					sizeof(char *) * (numVariables + 2)
				      ));

	/* copy existing env vector */
	for ( count = 0 ; count < numVariables ; count++ )
		newEnvp[count] = MemNewString( envp[count] ); 
	
	/* search through, checking for variable in question */
	for ( count = 0 ; count < numVariables ; count++ )
		if ( !strcmp( newEnvp[count], variable ) )
			break;
	
	newEnvp[count] = newSetting;

	if ( count == numVariables )	/* finished loop without match */
		numVariables++;		/* added 1 more var to the env */
	else
		MemFree( newEnvp[numVariables+1] );	/* don't need */

	/* make sure the last entry in the vector is NULL */
	newEnvp[numVariables] = NULL;

	return( newEnvp );
}


#ifdef OW_I18N

#define	WCS_WINDOW	0
#define	WCS_OPEN	1
#define	WCS_CLOSE	2
#define	WCS_FULL_SIZE	3
#define	WCS_NORMAL_SIZE	4
#define	WCS_PROPERTIES	5
#define	WCS_BACK	6
#define	WCS_REFRESH	7
#define	WCS_QUIT	8
#define	WCS_THIS_WINDOW	9
#define	WCS_ALL_POP_UPS	10
#define	WCS_DISMISS	11
#define	WCS_OWNER	12
#define	WCS_MAX		13

static wchar_t	*wcs_menu[WCS_MAX];

InitWindowMenusLabel()
{
	/*
	 * We should share the string space as much as possible,
	 * becuase after retrieve from locale database, this is fixed
	 * string.
	 */
	/* STRING_EXTRACTION - The next 13 text are part of window
	   menu.
	 */
	wcs_menu[WCS_WINDOW]		= mbstowcsdup(gettext("Window"));
	wcs_menu[WCS_OPEN]		= mbstowcsdup(gettext("Open"));
	wcs_menu[WCS_CLOSE]		= mbstowcsdup(gettext("Close"));
	wcs_menu[WCS_FULL_SIZE]		= mbstowcsdup(gettext("Full Size"));
	wcs_menu[WCS_NORMAL_SIZE]	= mbstowcsdup(gettext("Normal Size"));
	wcs_menu[WCS_PROPERTIES]	= mbstowcsdup(gettext("Properties"));
	wcs_menu[WCS_BACK]		= mbstowcsdup(gettext("Back"));
	wcs_menu[WCS_REFRESH]		= mbstowcsdup(gettext("Refresh"));
	wcs_menu[WCS_QUIT]		= mbstowcsdup(gettext("Quit"));
	wcs_menu[WCS_THIS_WINDOW]	= mbstowcsdup(gettext("This Window"));
	wcs_menu[WCS_ALL_POP_UPS]	= mbstowcsdup(gettext("All Pop-ups"));
	wcs_menu[WCS_DISMISS]		= mbstowcsdup(gettext("Dismiss"));
	wcs_menu[WCS_OWNER]		= mbstowcsdup(gettext("Owner?"));

	IconFullButtons[0].label = wcs_menu[WCS_OPEN];
	IconFullButtons[1].label = wcs_menu[WCS_FULL_SIZE];
	IconFullButtons[2].label = wcs_menu[WCS_PROPERTIES];
	IconFullButtons[3].label = wcs_menu[WCS_BACK];
	IconFullButtons[4].label = wcs_menu[WCS_REFRESH];
	IconFullButtons[5].label = wcs_menu[WCS_QUIT];

	IconFullMenu.title = wcs_menu[WCS_WINDOW];


	FrameNormButtons[0].label = wcs_menu[WCS_CLOSE];
 	FrameNormButtons[1].label = wcs_menu[WCS_NORMAL_SIZE];
 	FrameNormButtons[2].label = wcs_menu[WCS_PROPERTIES];
 	FrameNormButtons[3].label = wcs_menu[WCS_BACK];
 	FrameNormButtons[4].label = wcs_menu[WCS_REFRESH];
 	FrameNormButtons[5].label = wcs_menu[WCS_QUIT];

	FrameNormMenu.title = wcs_menu[WCS_WINDOW];


	FrameFullButtons[0].label = wcs_menu[WCS_CLOSE];
	FrameFullButtons[1].label = wcs_menu[WCS_FULL_SIZE];
	FrameFullButtons[2].label = wcs_menu[WCS_PROPERTIES];
	FrameFullButtons[3].label = wcs_menu[WCS_BACK];
	FrameFullButtons[4].label = wcs_menu[WCS_REFRESH];
	FrameFullButtons[5].label = wcs_menu[WCS_QUIT];

	FrameFullMenu.title = wcs_menu[WCS_WINDOW];


	DismissButtons[0].label = wcs_menu[WCS_THIS_WINDOW];
	DismissButtons[1].label = wcs_menu[WCS_ALL_POP_UPS];


	FrameLimButtons[0].label = wcs_menu[WCS_DISMISS];
	FrameLimButtons[1].label = wcs_menu[WCS_BACK];
	FrameLimButtons[2].label = wcs_menu[WCS_REFRESH];
	FrameLimButtons[3].label = wcs_menu[WCS_OWNER];

	FrameLimMenu.title = wcs_menu[WCS_WINDOW];
}


FreeWindowMenuLabel()
{
	register wchar_t	**w;

	for (w = wcs_menu; w < &wcs_menu[WCS_MAX]; w++)
		MemFree(*w);
}


static int	WindowMenuCreated = False;

WindowMenuCreate(dpy)
register Display	*dpy;
{
	if (WindowMenuCreated == True)
		return;
	InitWindowMenusLabel();
	MenuCreate(dpy, &IconFullMenu);
	MenuCreate(dpy, &FrameNormMenu);
	MenuCreate(dpy, &FrameFullMenu);
	MenuCreate(dpy, &FrameLimMenu);
	MenuCreate(dpy, &DismissMenu);
	WindowMenuCreated = True;
}


WindowMenuDestroy(dpy)
register Display	*dpy;
{
	if (WindowMenuCreated == False)
		return;
	MenuDestroy(dpy, &IconFullMenu);
	MenuDestroy(dpy, &FrameNormMenu);
	MenuDestroy(dpy, &FrameFullMenu);
	MenuDestroy(dpy, &FrameLimMenu);
/*	MenuDestroy(dpy, &DismissMenu);	*/
	FreeWindowMenuLabel();
	WindowMenuCreated = False;
}
#endif OW_I18N
