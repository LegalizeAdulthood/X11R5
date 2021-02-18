/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/********************************************\
* 					     *
* 	The GWM Window Object Abstraction    *
* 					     *
\********************************************/

#ifndef WOB_h
#ifndef INCLUDE_WOOL_H
#include "wool.h"
#endif /* INCLUDE_WOOL_H */
#define WOB_h

typedef PtrToFunc WOB_METHOD;
typedef WOB_METHOD *Methods;

/*
 * A wob is an object maintained in a list of all wobs.
 * It is able to open (be created) or destroyed, but its XWindow, the hook
 * will govern its visibility and attributes
 * Its behaviour is given via a finite state machine, fsm.
 */

#define WobMacro  Methods type; \
                  int valid; \
		  int status;   \
		  Window hook;  \
		  struct _Wob *parent; \
		  struct _Box	box; \
		  int curstate; \
    		  Card32 input_mask; \
		  WOOL_OBJECT bordertile; \
		  WOOL_OBJECT menu; \
		  WOOL_OBJECT fsm; \
		  WOOL_OBJECT cursor; \
		  WOOL_OBJECT tile; \
		  WOOL_OBJECT property;

#define WobMAGIC 456789
#define WobIsValid(w) (((Wob)(w))->valid == WobMAGIC)

/*
 * WobMacro define fields common to all objects
 */

#define NOFLAGS			0
#define ClientWindowStatus	(1L << 1)
#define IconStatus		(1L << 2)
#define RootStatus		(1L << 3)
#define MenuStatus		(1L << 4)
#define TopLevelXWindowStatus	(1L << 5)
#define TargetWindowStatus      ClientWindowStatus|IconStatus \
				|RootStatus

typedef struct _Box {		/* the box is there for ANY wob */
    int             x, y;
    unsigned int    width, height;
    unsigned int    borderwidth;
    long            borderpixel;
    long            background;
}               *Box;

typedef struct _Wob {	/* any wob matches this one */
    WobMacro
}               *Wob;

typedef struct _Plug {
    WobMacro
    unsigned int graphic_mask;
    WOOL_OBJECT	graphic;	/* what to draw on the background */
}               *Plug;

typedef struct _Bar {
    WobMacro
    short		    min_width, max_width;
    short		    plug_separator;
    short 	    direction;
    short             nplugs;
    Wob             plugs[1];
}               *Bar;

typedef struct _Meter {
    WobMacro
    long	foreground;
    long         font;
    short		horizontal_margin;
    short		vertical_margin;
}               *Meter;

typedef struct _CWCachedProperties { 	/* common to window & icon */
    WOOL_OBJECT	clientclass;		
    WOOL_OBJECT clientname;
    WOOL_OBJECT machinename;
    WOOL_OBJECT windowname;
    WOOL_OBJECT iconname;
    XWMHints	wm_hints;
    XSizeHints  normal_hints;
    WOOL_OBJECT	transient_for;		/* raw X11 hint */
    WOOL_OBJECT client_group;		/* the logical GWM group */
    int		wm_state;		/* last updated state */
    struct _ClientWindow *user_icon;	/* user-managed icon */
    int		colormap_windows_size;	/* list of sub-windows that must */
    Window     *colormap_windows;	/* have colormap installed */
    int		colormap_windows_index;	/* current selected one */
					/* standard ICCC protocols */
    unsigned    wm_take_focus : 1;
    unsigned    wm_save_yourself : 1;
    unsigned    wm_delete_window : 1;
    unsigned    new_normal_hints : 1;
} 	        *CWCachedProperties;    

typedef struct _ClientWindow {
    WobMacro
    struct _GWMScreenContext *screen;	/* on which screen is it? */
    Window 	client;		  	/* the inner window */
    Wob		client_wob;		/* if client is made by GWM */
    struct _ClientWindow *window;	/* the client in normal state */
    struct _ClientWindow *icon;		/* the client when iconified */
    struct _ClientWindow *previous;	/* doubly linked list of windows */
    struct _ClientWindow *next;
    short		inner_x, inner_y;	/* dims */
    unsigned short inner_width, inner_height;
    short	       	inner_borderwidth;
    short	       	old_inner_borderwidth;
    Bar		titlebar;		/* the 4 bars */
    Bar		leftbar;
    Bar		rightbar;
    Bar		basebar;
    WOOL_OBJECT opening;		/* NULL after first map */
    WOOL_OBJECT closing;		/* evaluated on destruction */
    WOOL_OBJECT grabs;			/* on whole frame */
    WOOL_OBJECT icon_plug;		/* for icons only */
    WOOL_OBJECT icon_description;	/* wool code to build the icon */
    CWCachedProperties cached_props;	/* X properties cached in common */
    Colormap	colormap;		/* colormap for main window */
    unsigned	mapped : 1;		/* flag if window is mapped */
    unsigned    client_shaped : 1; 	/* is client non-rectangular? */

}               *ClientWindow;

typedef struct _Menu {
    WobMacro
    struct _GWMScreenContext *screen;	/* on which screen is it? */
    short		min_width, max_width;
    short		bar_separator;
    short		direction;
    short		nbars;
    Bar		bars[1];
}               *Menu;

#define MENU_unmapped	0
#define MENU_placed	1
#define MENU_popped	2

/* screen description */

typedef struct _GWMScreenContext {	/* All screen-dependent data */
    int		    screen;		/* screen number */
    Screen	   *x_screen;		/* X structure */
    Window          root;		/* root window */
    ClientWindow    rootWob;		/* and its associated wob */
    int		    WindowCount;	/* number of managed windows */
    Window          GwmWindow;	        /* window used to set properties on */
    struct {
			long Back;	/* Background */
			long Fore;	/* Foreground */
			long Border;	/* BorderPixel of windows */
			long GridColor;	/* Context -> pixel.GridColor */
			long InvertColor; /* for wob-invert */
    }		    pixel;		/* screen-dependent colors */
    struct {
    			GC Work;
    			GC Set;
    			GC Draw;
    			GC Invert;
    			GC Back;
    			GC Bitmap;
    } 		    gc;			/* the GC for the screen */
    int		    width, height, depth;	/* dims in pixels */
    int		    widthMM, heightMM;		/* dims in millimeters */
    ClientWindow    InstalledColormapCW;	/* window of inst. colormap */
    Pixmap	    DefaultBitmap;
    struct {
			Window window;		/* meter X window */
			short  x,y;
			unsigned short width, height;
			unsigned short borderwidth;
			long borderpixel;
			short anchor_x, anchor_y; /* gravity and anchor */
			short gravity;
			GC  gc;
			int  font;		/* meter appearance */
			char *string;
			short string_length;
			short string_x, string_y;
			long background;	
			long foreground;
			short  horizontal_margin;
			short  vertical_margin;
    }		    meter;
}               *GWMScreenContext;

/*
 * methods generic to wobs
 */

extern int WobEval(), WobPrint(), WobRelease(), WobExecute(), WobSet(),
	   WobGetCValue(), WobOpen(), WobClose(), WobEventHandler(), 
	   WobActionRoutine(), WobGetDimensions();

extern WOB_METHOD       ClientWindowClass[],
			MenuClass[],
			BarClass[],
			PlugClass[],
			ScreenClass[];
     
#endif /* WOB_h */
