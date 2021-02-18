/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/***************************************************************\
* 							        *
* 	BULL WINDOW MANAGER for X11 			        *
* 							        *
* 	general include file, contains GWM specific typedef.    *
* 							        *
\***************************************************************/

#ifndef INCLUDE_WOOL_H
#include "wool.h"
#endif
#ifdef DEBUG
#include <stdio.h>
#endif /* DEBUG */
#define GWM_h

/* some parameters */

#define WLPROFILE_USER_VARIABLE "GWMPROFILE"
#define WOOL_USER_PROFILE ".gwmrc"
#define WOOL_TEXT_EXTENSION ".gwm"
#define WLPATH_SHELL_VARIABLE "GWMPATH"

/* adaptation to the different X11 libraries */

#ifdef X11R4
#define SHAPE
#else					/* non-X11R4 */
#  ifdef X11R5
#  define SHAPE
#  else					/* R1, R2 & R3 */
#  define NOBASEDIMS
#  define NOXEHTYPE
#  endif
#endif /* X11R4 */

/* customization flags */

#ifndef TRUE
#define TRUE			1
#define FALSE			0
#endif
#ifndef Max
#define Max(x,y)		(((x)<(y))?(y):(x))
#define Min(x,y)		(((x)<(y))?(x):(y))
#define FlagOn(mask,flag) ((mask)&(flag))
#endif

#define HORIZONTAL		0
#define VERTICAL		1

typedef char   *Pointer;

typedef long    WoolAtom;
typedef char   *AtomName;
typedef int     Errno;
typedef int     (*PtrToFunc) ();
typedef void    (*PtrToProc) ();

/*
 * error numbers
 */

#define OK 				0
#define FatalError 		-1
EXT int      ErrorStatus;	/* setted at a string in XError */

/* X types */

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

typedef XEvent  EventData, *Event;

extern Card32 EventProperties[];
#define EPGrabRedirected (1<<0)
#define EPKeyOrButton    (1<<1)
#define EPCrossing       (1<<2)
#define EPFocus          (1<<3)
#define EPXY             (1<<4)
#define EPXYRoot         (1<<5)
#define EPTime           (1<<6)
#define EPRedirectable   (1<<7)

#define event_is_key_or_button(evt) \
	(EventProperties[(evt).type] & EPKeyOrButton)
#define event_is_grabbable(evt) \
	(EventProperties[(evt).type] & EPGrabRedirected)
#define event_is_redirectable(evt) \
	(EventProperties[(evt).type] & EPRedirectable)
#define event_is_crossing(evt) \
	(EventProperties[(evt).type] & EPCrossing)
#define event_is_focus(evt) \
	(EventProperties[(evt).type] & EPFocus)

#define LegalPointerEventMask(mask) ((mask) & (0xffff ^ 0x8003))

#ifndef WOB_h
#include "wob.h"
#endif

#define GWMUserEvent (LASTEvent+1)

/* The Context is a structure of all screen-dependent variables
 * Multi-screen operation will be handled by switching contextes
 */

EXT GWMScreenContext *GWMManagedScreens;/* the list of all screens on the
					   display, being NULL if not
					   managed. */
    
EXT GWMScreenContext  Context;		/* the current screen (pointer to) */
EXT GWMScreenContext *ContextPtr;	/* used by the following macro */
EXT GWMScreenContext GWM_OldContext;	/* for FOR_ALL_SCREENS macro */

#define FOR_ALL_SCREENS \
	{ GWMScreenContext OldContext = Context, *OldContextPtr = ContextPtr;\
	for (ContextPtr = GWMManagedScreens;\
	         ContextPtr < GWMManagedScreens + ScreenCount(dpy); \
	         ContextPtr++) \
	    if(Context = *ContextPtr)

#define END_OF_ALL_SCREENS Context = OldContext; ContextPtr = OldContextPtr;}

/*
 * ICCC structures
 */

#define WM_STATE_Withdrawn 0
#define WM_STATE_Normal 1
#define WM_STATE_Iconified 3

typedef struct _WM_STATE_PROP {
    Card32	state;		/* WM_STATE_Withdrawn, _Normal, _Iconified */
    Window	icon;		/* X window id of the icon */
} *WM_STATE_PROP;

#define WM_STATE_PROP_Length 2  /* in Card32 elements */

/* max length of some properties */

#define WM_COLORMAP_WINDOWS_PROP_Length 80 /* number of sub-wins colormaps */
#define WM_PROTOCOLS_PROP_Length        40 /* number of protocols */

/*
 * some useful macros
 */

#define check_window_size(wob) \
	if(!wob -> box.width && !wob -> box.height) \
		GWM_no_size_window_error(wob);

#define SAVE_EVENT_CONTEXT \
        Event old_TriggeringEvent = TriggeringEvent; \
	Wob   old_TargetWob = TargetWob; \
	ClientWindow old_TargetWindow = TargetWindow; \
	GWMScreenContext old_Context = Context

#define RESTORE_EVENT_CONTEXT \
	TriggeringEvent = old_TriggeringEvent; \
	TargetWob = old_TargetWob; \
	TargetWindow = old_TargetWindow; \
	Context = old_Context

#define TrapXErrors(statement) (\
  	XSync(dpy, 0), XSetErrorHandler(NoXError), ErrorStatus = 0,	\
	statement,  \
	XSync(dpy, 0), XSetErrorHandler(XError), ErrorStatus)

#define FlushXErrors() \
    	XSetErrorHandler(NoXError), XSync(dpy, 0), XSetErrorHandler(XError)

/*
 * tracing facilities:
 * trace1(level, string, parameter);
 * trace(level, string);
 */

#ifdef DEBUG
EXT int      GWM_Tracelevel INIT(0);

#define trace(l, s) ((l<=GWM_Tracelevel) ? fprintf(stderr,s) : 0);
#define trace1(l, s, p) ((l<=GWM_Tracelevel) ? fprintf(stderr,s,p) : 0);
#define trace2(l, s, p,q) ((l<=GWM_Tracelevel) ? fprintf(stderr,s,p,q) : 0);
#else /* DEBUG */
#define trace(l, s)
#define trace1(l, s, p)
#define trace2(l, s, p,q)
#endif /* DEBUG */

#ifdef SECURE
extern char    *eventtype[];

#endif /* SECURE */

#ifndef NULL
#define NULL	0
#endif

/*
 * GWM global variables, common to the display
 */

EXT Display    *dpy;		/* THE display */
EXT XContext 	wob_context;	/* to retrieve wobs via hooks */
EXT XContext 	client_context;	/* to retrieve wobs via clients */
EXT int         errno;
EXT Wob         TargetWob INIT(0);
EXT ClientWindow TargetWindow INIT(0);
EXT Event       TriggeringEvent INIT(0);
EXT char       *GWM_Display;
EXT int		GWM_ScreenCount;
EXT int		GWM_DefaultScreen;
EXT ClientWindow GWM_window_being_decorated INIT(0);
EXT int		GWM_ShapeExtension INIT(0); /* display has shape extension ? */
EXT int		GWM_ShapeEventBase INIT(0); /* first event # of shape ext. */
EXT int		GWM_ShapeErrorBase INIT(0); /* first error # of shape ext. */

/* switches */
EXT Wob         GWM_ServerGrabbed INIT(0);
EXT Wob         GWM_KeyboardGrabbed INIT(0);
EXT int         GWM_ProcessingExistingWindows INIT(0);
EXT int 	GWM_Mapall INIT(0);
EXT int 	GWM_Synchronize INIT(0);
EXT int		GWM_UserSynchronous INIT(1);
EXT int		GWM_Monoscreen INIT(0);
EXT int		GWM_No_set_focus INIT(0);
EXT char       *GWM_ScreensNotManaged INIT(0);
EXT int         GWM_Movegrid INIT(0);
EXT int         GWM_Resizegrid INIT(0);
EXT int 	GWM_Move_meter INIT(0);		/* is there a moving meter */
EXT int 	GWM_Resize_meter INIT(1);	/* is there a resize meter */
EXT int         GWM_InvertCursors INIT(1); /* for cursor-make */
EXT XEvent      GWM_LastEvent;		/* last key or button (user) event */
EXT Time        GWM_DoubleClickDelay INIT(500);	/* in millisecond */
EXT int		GWM_check_input_focus_flag INIT(1);/*  set focus check flag */
EXT int		GWM_backup_old_property INIT(0);
EXT int		GWM_GrabChildEvents INIT(0);
EXT int		GWM_Propagate_user_events INIT(1);
EXT ClientWindow GWM_Window_being_opened INIT(0);
EXT int		GWM_resize_style INIT(0);
EXT int		GWM_window_cutting_size INIT(0);
EXT int         GWM_Mwm_catch_corners INIT(0);
EXT int         GWM_grab_keyboard_also INIT(0);
EXT int         GWM_confine_grabs INIT(0);
EXT int         GWM_confine_windows INIT(0);
EXT int         GWM_quiet INIT(0);
EXT int 	GWM_reenter_on_opens INIT(1);
EXT int 	GWM_delete_read_properties INIT(False);

#ifdef DEBUG
EXT int         GWM_GrabServer INIT(0);
#else
EXT int         GWM_GrabServer INIT(1);
#endif /* DEBUG */

/* masks */
EXT unsigned int ClientMask 	     INIT(SubstructureRedirectMask);
EXT unsigned int ClientClientMask    INIT(PropertyChangeMask
					| StructureNotifyMask
					| ColormapChangeMask);
EXT unsigned int RootMask	     INIT(SubstructureRedirectMask
					| ColormapChangeMask
					| PropertyChangeMask);
EXT unsigned int WobMask 	     INIT(0);
EXT unsigned int MeterMask	     INIT(ExposureMask);

/* static data for remebering GWM state */

EXT int 	GWM_argc;			/* unix argc argv of GWM */
EXT char 	** GWM_argv;
EXT char 	** GWM_is_restarting INIT(0);
EXT int		GWM_is_ending INIT(0);
EXT int		GWM_time_of_last_bus_error INIT(0);
EXT union {			/* keep track of old prop */
    XWMHints	wm_hints;
} GWM_backup_of_old_property;

/* defaults */
EXT int         DefaultBorderWidth INIT(1);
EXT int         DefaultLabelHorizontalMargin INIT(4);
EXT int         DefaultLabelVerticalMargin INIT(2);
EXT int         DefaultPlugSeparator INIT(4);
EXT int         DefaultBarSeparator INIT(0);
EXT int         DefaultInnerBorderwidth INIT(ANY);
EXT int		DefaultMenuDirection INIT(1);
EXT int		DefaultBarMinWidth INIT(1);
EXT int		DefaultBarMaxWidth INIT(100);
EXT int		DefaultMenuMinWidth INIT(1);
EXT int		DefaultMenuMaxWidth INIT(10000);
EXT int         DefaultFont;

/* X Atoms */
EXT Atom	XA_WM_STATE;
EXT Atom	XA_WM_COLORMAP_WINDOWS;
EXT Atom	XA_WM_CHANGE_STATE;
EXT Atom	XA_WM_PROTOCOLS;
EXT Atom	XA_WM_TAKE_FOCUS;
EXT Atom	XA_WM_SAVE_YOURSELF;
EXT Atom	XA_WM_DELETE_WINDOW;
EXT Atom        XA_GWM_EXECUTE;
EXT Atom        XA_GWM_RUNNING;

 /* WOOL Atoms of the context */
#ifdef INCLUDE_WL_ATOM_H

EXT WOOL_Atom   WA_borderwidth;
EXT WOOL_Atom   WA_borderpixel;
EXT WOOL_Atom   WA_background;
EXT WOOL_Atom   WA_foreground;
EXT WOOL_Atom   WA_bordertile;
EXT WOOL_Atom   WA_fsm;
EXT WOOL_Atom   WA_menu;
EXT WOOL_Atom   WA_tile;
EXT WOOL_Atom   WA_cursor;
EXT WOOL_Atom   WA_property;
EXT WOOL_Atom   WA_describe_window;
EXT WOOL_Atom   WA_window_description;
EXT WOOL_Atom   WA_icon_description;
EXT WOOL_Atom   WA_describe_screen;
EXT WOOL_Atom   WA_window;
EXT WOOL_Atom   WA_bar;
EXT WOOL_Atom   WA_plug;
EXT WOOL_Atom   WA_root;
EXT WOOL_Atom   WA_icon;
EXT WOOL_Atom   WA_root_window;
EXT WOOL_Atom   WA_grabs;
EXT WOOL_Atom   WA_grab;
EXT WOOL_Atom   WA_ungrab;
EXT WOOL_Atom   WA_opening;
EXT WOOL_Atom   WA_closing;
EXT WOOL_Atom	WA_mapped;
EXT WOOL_Atom	WA_stacking_order;
EXT WOOL_Atom   WA_iconify_window;
EXT WOOL_Atom   WA_font;
EXT WOOL_Atom   WA_horizontal_margin;
EXT WOOL_Atom   WA_vertical_margin;
EXT WOOL_Atom   WA_color;
EXT WOOL_Atom   WA_gray;
EXT WOOL_Atom   WA_mono;
EXT WOOL_Atom   WA_ContextNamespace;
EXT WOOL_Atom   WA_cursorNW;
EXT WOOL_Atom   WA_cursorNE;
EXT WOOL_Atom   WA_cursorN;
EXT WOOL_Atom   WA_cursorSW;
EXT WOOL_Atom   WA_cursorSE;
EXT WOOL_Atom   WA_cursorS;
EXT WOOL_Atom   WA_cursorE;
EXT WOOL_Atom   WA_cursorW;
EXT WOOL_Atom	WA_here;
EXT WOOL_Atom	WA_class_name;
EXT WOOL_Atom	WA_client_name;
EXT WOOL_Atom	WA_icon_name;
EXT WOOL_Atom	WA_starts_iconic;
EXT WOOL_Atom   WA_x;
EXT WOOL_Atom   WA_y;
EXT WOOL_Atom   WA_gravity;

#endif /* INCLUDE_WL_ATOM_H */

#ifdef INCLUDE_WL_LIST_H
EXT WOOL_List   WL_describe_window_call;
EXT WOOL_List   WL_describe_screen_call;
EXT WOOL_List   WL_iconify_window_call;

#endif /* INCLUDE_WL_LIST_H */

#ifdef INCLUDE_WL_STRING_H

EXT WOOL_String DefaultClientClass;
EXT WOOL_String DefaultClientName;
EXT WOOL_String DefaultWindowName;
EXT WOOL_String DefaultMachineName;
EXT WOOL_String DefaultIconName;

#endif /* INCLUDE_WL_STRING_H */
