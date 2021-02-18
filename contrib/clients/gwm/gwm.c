/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/******************************\
* 			       *
* BULL WINDOW MANAGER for X11  *
* 			       *
* 	main body	       *
* 			       *
\******************************/

/* include */

#include	<stdio.h>
#include	"EXTERN.h"
#include	"wool.h"
#include	"wl_atom.h"
#include	"wl_string.h"
#include	"wl_list.h"
#include	"wl_client.h"
#include 	"wl_func.h"
#include	"yacc.h"
#include	"INTERN.h"
#include 	"gwm.h"
#include 	<signal.h> 
#include 	<ctype.h> 

/* external */

#ifdef NOXEHTYPE
typedef int (*XErrorHandler) ();
extern XErrorHandler XSetErrorHandler ();
#endif

extern XErrorHandler XError;
extern int      InitWool();
extern Wob      LookUpWob();
extern          InitScreens();
extern Wob      NewClientWindow();
extern void     ScreenClose();
extern void	DecorateWindows();
extern char    *wool_fix_path();
extern 		NoXError();
extern GWMScreenContext RegisterScreen();
extern void GWM_BusErrorSignalHandler();
extern ClientWindow LookUpClient();
DECLARE_strchr;

/* local */

static char     err[] = "\007GWM:";
static int GWM_Decorate_one_window=0;
static int GWM_Decorate_all_windows=0;
static int GWM_is_initialized = 0;
static int GWM_RetryStart = 0;

/* routines */

int 		GWM_Initialize();

/*
 * Main body of GWM:
 * 	- parse arguments
 * 	- initialize X
 * 	- initialize Wool
 * 	- read user profile
 * 	- decorate all already present windows
 * 	- enter main loop: GWM_ProcessEvents
 */

int
main(argc, argv)
int             argc;
char           *argv[];
{
    malloc_init();			/* FIRST THING TO DO!!! */
#ifdef MONITOR
    moncontrol(0);			/* do not trace profile read */
#endif /* MONITOR */
    GWM_argc = argc;
    GWM_argv = argv;
    GWM_getopts(argc, argv);		/* options & open display */
    RegisterScreens();

    if (wool_error_occurred(GWM_Initialize))	/* Init wool and display */
	GWM_Abort("cannot initialize");

    if (wool_error_in_profile && !wool_continue_reading_on_error)
	GWM_Abort("error while reading profile");

    if (wool_error_occurred(InitScreens))	/* screen and syswd */
	GWM_Abort("cannot set up");

    if (wool_error_occurred(DecorateWindows) &&
	!wool_continue_reading_on_error)/* existing windows */
	GWM_Abort("cannot decorate windows");

    FOR_ALL_SCREENS {			/* call opening on all screens */
	SetTarget(Context -> rootWob);
	WOOL_send(WOOL_eval, Context -> rootWob -> opening,
		  (Context -> rootWob -> opening));
	decrease_reference(Context -> rootWob -> opening);
	Context -> rootWob -> opening = NULL;
    } END_OF_ALL_SCREENS;
    GWM_is_initialized = 1;
    if (set_wool_error_resume_point()) { /* wool_error will jump here */
	if (GWM_is_restarting || GWM_is_ending)
	    GWM_BusErrorSignalHandler(0); /* skip wool when error in ending */
	zrt_gc(0);
	if (GWM_window_being_decorated) {
	    Window w = GWM_window_being_decorated -> client;
	    ClientWindowClose(GWM_window_being_decorated);
	    GWM_window_being_decorated = 0;
	    XMapWindow(dpy, w);
	}
    }

#ifdef MONITOR
    moncontrol(1);			/* only trace main loop */
#endif /* MONITOR */
    GWM_ProcessEvents(1);		/* MAIN LOOP (never returns!) */
    return 0;
}

/* fatal init error handler */

GWM_Abort(message)
char * message;
{
    fprintf(stderr, "%s -- %s, aborting\n", err, message);
    exit(-1);
}


/* The main loop of GWM:
 * 	- wait for an event
 * 	- look if it is for the hook (X window) of a wob
 * 	  (or the client of a window), then dispatch it to the concerned
 * 	  wob
 * 	- if it is a new window being mapped, decorate it
 * 	- the server is grabbed, redirect events to grabbing wob
 * 
 * If blocking is 1, the loop never ends, whereas if it is 0, it returns as
 * soons as the queue is empty (via the XPending call)
 */

GWM_ProcessEvents(blocking)
int	blocking;
{
    EventData       evt;
    Wob             wob;

#ifdef DEBUG
    wob = 0;			/* entry point for dbx */
#endif /* DEBUG */
    while (blocking || XPending(dpy)) {
	XNextEvent(dpy, &evt);
	TraceDo('e',GWM_prettyprint_event("XEV: ", &evt, "\n"));

	if (wob = LookUpWob(evt.xany.window)) {
	    if (GWM_ServerGrabbed) {
		if(event_is_grabbable(evt)) {
		    if (GWM_GrabChildEvents
			|| !IsAnAncestor(GWM_ServerGrabbed, wob))
			wob = GWM_ServerGrabbed;
		    WOOL_send(WOOL_process_event, wob, (wob, &evt));
		} else if (event_is_redirectable(evt)) {
		    if (wob ==  GWM_ServerGrabbed
			|| (!GWM_GrabChildEvents
			    && IsAnAncestor(GWM_ServerGrabbed, wob))
			|| IsNotGrabDiscarded(&evt)) {
			WOOL_send(WOOL_process_event, wob, (wob, &evt));
		    }
		} else {
		    WOOL_send(WOOL_process_event, wob, (wob, &evt));
		}
	    } else {
		WOOL_send(WOOL_process_event, wob, (wob, &evt));
	    }
	} else if (wob = (Wob) LookUpClient(evt.xany.window)) {
 		WOOL_send(WOOL_process_event, wob, (wob, &evt));
	} else if (evt.xany.type == MappingNotify) {
	    XRefreshKeyboardMapping(&evt.xmapping);
	} else if (evt.xany.type == ColormapNotify) {
	    Update_colormap_in_colormap_windows_list(&evt);
	} else if (MeterEventHandler(&evt)) {
	    ;
#ifdef DEBUG
	} else {
	    wool_printf("Received event %s", eventtype[evt.type]);
	    wool_printf(" for unknown window 0x%x\n", evt.xany.window);
#endif /* DEBUG */
	}
	if (event_is_key_or_button(evt))/* store last button */
	    bcopy(&evt, &GWM_LastEvent, sizeof(XEvent));
	if (blocking)
	    dft_gc();
    }
}


/*
 * Initialize GCs used in wool
 */

InitGC()
{
    XGCValues       gc_values;

    FOR_ALL_SCREENS {
	gc_values.graphics_exposures = False;
	gc_values.function = GXinvert;
	Context -> gc.Invert = XCreateGC(dpy, Context -> root,
			      GCGraphicsExposures | GCFunction, &gc_values);
	gc_values.foreground = Context -> pixel.Fore;
	gc_values.background = Context -> pixel.Back;
	gc_values.function = GXcopy;
	Context -> gc.Work = XCreateGC(dpy, Context -> root,
	     GCGraphicsExposures | GCForeground | GCBackground | GCFunction,
				       &gc_values);
	gc_values.ts_x_origin = 0;
	gc_values.ts_y_origin = 0;
	gc_values.function = GXcopy;
	gc_values.fill_style = FillStippled;
	Context -> gc.Set = XCreateGC(dpy, Context -> root,
			GCFunction | GCTileStipXOrigin | GCGraphicsExposures
			     | GCTileStipYOrigin | GCFillStyle, &gc_values);
	gc_values.function = GXinvert;
	Context -> gc.Back = XCreateGC(dpy, Context -> root,
			      GCGraphicsExposures | GCFunction, &gc_values);
	gc_values.line_width = 0;
	gc_values.foreground = 0xfd;
	gc_values.function = GXxor;
	gc_values.subwindow_mode = IncludeInferiors;
	Context -> gc.Draw = XCreateGC(dpy, Context -> root,
	       GCGraphicsExposures | GCLineWidth | GCForeground | GCFunction
				       | GCSubwindowMode, &gc_values);
	MakeDefaultBitmap();
	gc_values.function = GXinvert;
	Context -> gc.Bitmap = XCreateGC(dpy, Context -> DefaultBitmap,
			      GCGraphicsExposures | GCFunction, &gc_values);
    }END_OF_ALL_SCREENS;
}

/*
 * GWM_Initialize
 * initialize:
 * 	X
 * 	Wool
 * 	internal variables
 */

int
GWM_Initialize()
{
    XSetWindowAttributes attributes;

    GWM_SignalsInit();
    XrmInitialize();
    SetUpDefaults();
    InitGC();
    if (InitWool() != OK)
	return FatalError;

    /* Manage Windows */
    attributes.event_mask = KeyPressMask | ButtonPressMask | ButtonReleaseMask
	| EnterWindowMask | LeaveWindowMask | FocusChangeMask
	| SubstructureRedirectMask;
    XSetErrorHandler(NoXError);
    XSync(dpy, False);
    FOR_ALL_SCREENS {
	ErrorStatus = 0;
	XChangeWindowAttributes(dpy, Context -> root,
				CWEventMask, &attributes);
	XSync(dpy, False);
	if (ErrorStatus && (!GWM_Decorate_all_windows)) {
	    fprintf(stderr,
		    "GWM: cannot get control of windows on screen #%d!\n%s\n",
		    Context -> screen,
		 "Maybe another window manager is running on the display?");
	    if (GWM_RetryStart) {
		fprintf(stderr, "Keeping retrying...\n");
		do {
		    ErrorStatus = 0;
		    XChangeWindowAttributes(dpy, Context -> root,
					    CWEventMask, &attributes);
		    XSync(dpy, False);
		} while(ErrorStatus);
		fprintf(stderr, "OK for screen %d\n", Context -> screen);
	    } else {
		exit(1);
	    }
	}
    }END_OF_ALL_SCREENS;
    GWM_re_init_PointerRoot();
    XSetErrorHandler(XError);
    return OK;
}

/* re-init pointerroot */

GWM_re_init_PointerRoot()
{
    XSelectInput(dpy, Context -> root, 0);
    XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
}

/* CloseEverything
 * n = 	0 normal (end)
 * 	1 error (we can still print messages)
 * 	-1 fatal error (connection closed)
 */

GWM_end(n)
int n;
{
#ifdef MONITOR
    moncontrol(0);			/* do not trace ending code */
#endif /* MONITOR */
    if (n >= 0 && GWM_is_initialized) {
	XSetErrorHandler(NoXError);
	GWM_re_init_PointerRoot();
	XSync(dpy, True);
	if (!GWM_is_ending && Context -> rootWob) {
	    GWM_is_ending = 1;
	    wool_do_print_errors = 0;
	    FOR_ALL_SCREENS {
		SetTarget(Context -> rootWob);
		ScreenClose(Context -> rootWob);
	    }END_OF_ALL_SCREENS;
	}
	XCloseDisplay(dpy);
    }
    exit(n);
}

/* wool_end is calling GWM_end */
wool_end(n)
int n;
{
    GWM_end(n);
}

/* signals management
 */

static int GWM_in_fatal_error;

/* We try to handle bus errors gracefully, i.e. by trying to trigger a
 * wool_error and going on, but aborting if we get one more
 */

void
GWM_FatalSignalHandler(sig)
int	sig;
{
    if (!GWM_in_fatal_error) {
	GWM_in_fatal_error = 1;
	XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
	XSync(dpy, True);
	if (wool_is_reading_file)
	    fprintf(stderr, "\n\"%s\": line %d:\n", wool_is_reading_file,
	    yylineno);
	wool_stack_dump(1);
	fprintf(stderr,
	    "\007\nGWM: Fatal error due to UNIX signal # %d -- Aborting\n",
	    sig);
    }
    exit(sig);
}

/* Try to handle bus errors as normal errors, execpt that:
 * 	- if 2 bus errors come within 1 second, abort.
 * 	- if we were ending, just end, (in case the end code is faulty)
 * 	- ditto for restarting
 */

void
GWM_BusErrorSignalHandler(sig)
int	sig;
{
    int             current_time;
    static int      aborting;

    if (GWM_is_restarting) {
	XCloseDisplay(dpy);
	execvp(GWM_is_restarting[0], GWM_is_restarting);
    }
    if (GWM_is_ending)
	GWM_end(-1);
    current_time = time(0);
    if (current_time == GWM_time_of_last_bus_error) {
	if (aborting) {
	    exit(-1);
	} else {
	    aborting = 1;
	    GWM_is_ending = 1;
	    fprintf(stderr,
		    "\007\nGWM: Internal error: bus error -- restarting\n");
	    XCloseDisplay(dpy);
	    execvp(GWM_argv[0], GWM_argv);
	}
    }
    else {
	GWM_time_of_last_bus_error = current_time;
#ifdef SYSV
	signal(sig, GWM_BusErrorSignalHandler);
#endif /* SYSV */
	wool_error(INTERNAL_ERROR, "bus error");
    }
}

GWM_SignalsInit()
{
    signal(SIGHUP, SIG_IGN);
    /* keep signals for dbx debugging */
#ifndef DEBUG
    signal(SIGINT, GWM_end);
    signal(SIGQUIT, GWM_end);
    signal(SIGTERM, GWM_end);
    signal(SIGFPE, GWM_FatalSignalHandler);
    signal(SIGILL, GWM_BusErrorSignalHandler);
    signal(SIGBUS, GWM_BusErrorSignalHandler);
    signal(SIGSEGV, GWM_BusErrorSignalHandler);
    signal(SIGSYS, GWM_FatalSignalHandler);
#endif /* DEBUG     */
}

/*
 * functions to process events in the queue
 */

WOOL_OBJECT
wool_process_masked_events(mask)
int	mask;		/* such as ExposureMask, SubstructureNotifyMask */
{
    EventData       evt;
    Wob             wob;

    SAVE_EVENT_CONTEXT;
    XSync(dpy, 0);
    while (XCheckMaskEvent(dpy, mask, &evt)) {
	if ((wob = LookUpWob(evt.xany.window))
	    || (wob = (Wob) LookUpClient(evt.xany.window))) {
	    WOOL_send(WOOL_process_event, wob, (wob, &evt));
	}
    }
    RESTORE_EVENT_CONTEXT;
    return NIL;
}

/* to remove unmapped events
   problem: argument w does not seem to be correctly passed, so use global
*/

static Window WindowUnmapped;

GWM_remove_unmap_event(d, evt, w)
Display *d;
XEvent *evt;
Window w;
{
    if (evt->xany.type == UnmapNotify
	&& evt->xunmap.window == WindowUnmapped)
	return 1;
    else
	return 0;
}

wool_process_unmap_events(w)
Window w;
{
    XEvent       evt;

    XSync(dpy, 0);
    WindowUnmapped = w;
    while (XCheckIfEvent(dpy, &evt, GWM_remove_unmap_event, (char *)w))
	;
}

WOOL_OBJECT
wool_process_exposes()
{
    return wool_process_masked_events(ExposureMask);
}

/*
 *  initialize X and Context
 */

GWM_OpenDisplay(display)
char           *display;
{
    GWM_Display = (char *) XDisplayName(display);
    if ((dpy = XOpenDisplay(display)) == NULL) {
	fprintf(stderr, "%s Cannot XOpenDisplay on %s\n",
		err, XDisplayName(display));
	exit(1);
    }
#ifdef SHAPE
    GWM_ShapeExtension = XShapeQueryExtension(dpy, &GWM_ShapeEventBase,
					      &GWM_ShapeErrorBase);
#endif /* SHAPE */
    if (GWM_Synchronize)
	XSynchronize(dpy, 1);
    GWM_ScreenCount = ScreenCount(dpy);
    wob_context = XUniqueContext();
    client_context = XUniqueContext();
    XSetErrorHandler(XError);
}

/* options handling
 */

char *
path_append(old_path, new_path)
char *old_path;
char *new_path;
{
    char *path;
    static int path_was_malloced;

    switch(*new_path){
    case '+': case '-':		/* append & prepend */
	if(path_was_malloced)
	    path = Realloc(old_path, strlen(old_path) + strlen(new_path) + 1);
	else
	    path = Malloc(strlen(old_path) + strlen(new_path) + 1);
	path_was_malloced = 1;
	strcpy(path, (*new_path == '+' ? old_path : new_path+1));
	strcat(path, ":");
	strcat(path, (*new_path == '+' ? new_path+1 : old_path));
	break;
    default:			/* replace */
	if(path_was_malloced)
            Free(old_path);
	path = new_path;
	path_was_malloced = 0;
    }
    return path;
}

usage()
{
    fprintf(stderr, "USAGE:   gwm OPTIONS [host:display]\n");
    fprintf(stderr, "OPTIONS:\n");	
    fprintf(stderr, "    -f file      use file instead of .gwmrc\n");
    fprintf(stderr, "    -p path      force GWMPATH\n");	
    fprintf(stderr, "    -p +path     appends to GWMPATH\n");	
    fprintf(stderr, "    -p -path     prepends to GWMPATH\n");	
    fprintf(stderr, "    -d display   manage display\n");
    fprintf(stderr, "    -x screens   do not manage screens e.g. -x3,2,5\n");
    fprintf(stderr, "    -1    manages only 1 screen, the default one\n");
    fprintf(stderr, "    -r    retries till it can manage screens\n");
    fprintf(stderr, "    -i    input focus remains PointerRoot\n");
    fprintf(stderr, "    -F    no freeze: never freeze server\n");
    fprintf(stderr, "    -t    to turn tracing on (as (trace 1))\n");	
    fprintf(stderr, "    -m    to map all windows on startup\n");	
    fprintf(stderr, "    -s    synchronize X calls\n");	
    fprintf(stderr, "    -q    quiet: do not print startup banner\n");
    fprintf(stderr, "    -a    asynchronously obeys button clicks\n");
    fprintf(stderr, "    -D    debug: does not abort on error in profile\n");
    fprintf(stderr, "    -W    decorate over another Window Manager\n");
    fprintf(stderr, "    -w window_id decorate ONLY one window\n");
#ifdef DEBUG
    fprintf(stderr, "    -T flags to trace some items:\n");
    fprintf(stderr, "          f  for fsm events\n");
    fprintf(stderr, "          e  for all X events\n");
    fprintf(stderr, "          l  for all file loads\n");
#endif
    fprintf(stderr, "\nGWMPATH is %s\n", wool_path);
    fprintf(stderr, "built-in path was  %s\n", DEFAULT_WLPATH);
}

GWM_getopts(argc, argv)
int             argc;
char           *argv[];

{
    extern int      optind;
    extern char    *optarg;
    char 	   *display_name = 0;
    char            *s;
    int             c, errflag = 0;

    wool_user_profile_name = WOOL_USER_PROFILE;
    wool_text_extension = WOOL_TEXT_EXTENSION;

    /* initialize paths (put .:$HOME before built-ins) */
    wool_path = wool_fix_path(DEFAULT_WLPATH);
    if ((s = (char *) getenv(WLPROFILE_USER_VARIABLE)) && (s[0] != '\0'))
	wool_user_profile_name = s;	     
    if ((s = (char *) getenv(WLPATH_SHELL_VARIABLE)) && (s[0] != '\0'))
	wool_path = s;
    if ((s = (char *) getenv("GWM_MONOSCREEN")) && (s[0] != '\0'))
	GWM_Monoscreen = 1;

    while ((c = getopt(argc, argv, "mtsb:p:f:DFT:w:Wd:iax:1qr")) != EOF) {
	switch ((char) c) {
#ifdef USER_DEBUG
	case 't':		/* set trace level */
	    wool_tracing_on = 1;
	    break;
#endif /* USER_DEBUG */
#ifdef DEBUG
	case 'T':
	    GWM_Tracelevel = atoi(optarg);
	    WlTraceFlags = optarg;
/* Current traceflags:
 * l load files
 * e All X events received 
 * f fsm MATCH/REJECTS
 */
	    break;
#endif /* DEBUG */
	case 'i':
	    GWM_No_set_focus = 1;
	    break;
	case 'm':		/* map all windows found on startup */
	    GWM_Mapall = 1;
	    break;
	case 's':		/* no synchro in DEBUG mode */
	    GWM_Synchronize = 1;
	    break;
	case 'r':		/* retry on StructureRedirect */
	    GWM_RetryStart = 1;
	    break;
	case 'f':
	    wool_user_profile_name = optarg;
	    break;
	case 'p':
	    wool_path = path_append(wool_path, optarg);
	    break;
	case 'D':
	    wool_continue_reading_on_error = 1;
	    break;
	case 'F':
	    GWM_GrabServer = 0;
	    break;
	case 'w':
	    GWM_Decorate_one_window = atoi(optarg);
	    break;
	case 'W':
	    GWM_Decorate_all_windows = 1;
	    break;
	case 'd':
	    display_name = optarg;
	    break;
	case 'x':
	    GWM_ScreensNotManaged = optarg;
	    break;
	case 'a':
	    GWM_UserSynchronous = 0;
	    break;
	case '1':
	    GWM_Monoscreen = 1;
	    break;
	case 'q':
	    GWM_quiet = 1;
	    break;
	case '?':		/* usage */
	default:
	    errflag++;
	    break;
	}
    }
    if(!GWM_quiet || errflag)
	GWM_print_banner();

    if (errflag) {
	usage();
	exit(1);
    }
    GWM_OpenDisplay(optind < argc ? argv[optind] : display_name);
}

/*
 * executes a function. Returns 1 on first wool_error, and aborts function.
 */

int
wool_error_occurred(function)
int	(*function)();
{
    if (set_wool_error_resume_point()) {
	return 1;
    }
    (*function)();
    return 0;
}

void
DecorateWindows()
/*
 * called to make wobs other all pre-existing windows
 */
{
    Window          dummywin, parent;	/* dummy parent */
    Window         *children;		/* list of root sons */
    unsigned int    nchildren;		/* number of children */
    ClientWindow    wob;
    int             i;

    FOR_ALL_SCREENS {
	if (XQueryTree(dpy, Context -> root, &dummywin, &parent,
		       &children, &nchildren)) {
	    SetTarget(Context -> rootWob);
	    GWM_ProcessingExistingWindows = 1;
	    for (i = 0; i < nchildren; i++) {
		if (wob = LookUpClient(children[i]))
		    ClientWindowInitialMap(wob);
		else if ((Mapped(children[i]) || GWM_Mapall)
		    && ((!GWM_Decorate_one_window)
			|| (GWM_Decorate_one_window == children[i]))) {
		    if (wob = (ClientWindow)
			DecorateWindow(children[i],
				       Context -> rootWob, 1, 1)) {
			ClientWindowInitialMap(wob);
		    }
		}
	    }
	    GWM_ProcessingExistingWindows = 0;
	    if (children)
		Free((char *) children);
	}
    }END_OF_ALL_SCREENS;
}

/* initialise managed screens */

InitScreens()
{
    FOR_ALL_SCREENS {
	SetTarget(Context -> rootWob);
	SetUpScreen(Context -> rootWob,
		    WOOL_send(WOOL_eval, WL_describe_screen_call,
			      (WL_describe_screen_call)));
	ScreenOpen(Context -> rootWob);
    }END_OF_ALL_SCREENS;
}

/* set up the list of managed screens: GWMManagedScreens */

RegisterScreens()   
{
    int             screen_num;
    int             number_of_managed_screens = 0;

    GWMManagedScreens = (GWMScreenContext *)
	Calloc(sizeof(GWMScreenContext), ScreenCount(dpy));

    /* parse list of non-managed screens and pokes them */
    mark_numbers_in_list(ScreenCount(dpy), GWM_ScreensNotManaged,
			 GWMManagedScreens);

    /* poke 1 in used screens, 0 in unused  and count them
     * manage only DefaultScreen if we are in Monoscreen mode
     */
    for (screen_num = 0; screen_num < ScreenCount(dpy); screen_num++) {
	if (GWMManagedScreens[screen_num]) {
	    GWMManagedScreens[screen_num] = NULL;
	} else if ((!GWM_Monoscreen) || screen_num == DefaultScreen(dpy)) {
	    GWMManagedScreens[screen_num] = (GWMScreenContext) 1;
	    number_of_managed_screens++;
	}
    }

    /* (abort if none left) */
    if (!number_of_managed_screens)
	GWM_Abort("no screen left to manage");

    /* allocates and registers the managed screens */
    for (screen_num = 0; screen_num < ScreenCount(dpy); screen_num++) {
	if (GWMManagedScreens[screen_num]) {
	    GWMManagedScreens[screen_num] = RegisterScreen(screen_num);
	}
    }
}


/* parses a comma-separated list of numbers (string) and pokes 1 in the list
 * if comprised beetween 0 and max_n
 */

mark_numbers_in_list(max_n, string, list)
int	max_n;
char   *string;
WOOL_OBJECT   *list;
{
    int             n;

    while (string && *string) {
	n = atoi(string);
	if (*string >= '0' && *string <= '9'
	    && n >= 0 && n < ScreenCount(dpy))
	    list[n] = (WOOL_OBJECT) 1;
	if (string = strchr(string, ','))
	    string++;
	else
	    return;
    }
}

