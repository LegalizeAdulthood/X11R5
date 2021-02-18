/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

/* @(#) win.h 50.4 90/12/12 Crucible */

#include "list.h"

/***************************************************************************
* Client state structures
***************************************************************************/

/*
 * The following structure is defined in Xutil.h, but it differs between X11R3
 * and X11R4 to reflect the acceptance of the ICCCM (version 1.0) as a
 * Consortium standard.  This replacement is necessary so that olwm can be
 * assured of getting the right structure, independent of the include files
 * present on the system it's compiled on.  Defining this this way allows olwm
 * to accomodate clients that use the R4-based structure as well as older
 * clients.
 */
typedef struct {
	long	flags;
	struct {
	    int x;
	    int y;
	    int width;
	    int height;
	} pad;			/* x, y, w, h replaced by pad[4] for X11R4 */
	int	min_width;
	int	min_height;
	int	max_width;
	int	max_height;
	int	width_inc;
	int	height_inc;
	struct	{
	    int x;
	    int y;
	} min_aspect, max_aspect;
	int	base_width;	/* added in X11R4 */
	int	base_height;	/* added in X11R4 */
	int	win_gravity;	/* added in X11R4 */
} NewXSizeHints;

/*
 * The following definitions are new in ICCCM version 1.0.  Olwm requires 
 * them, but they may not be present if olwm is being compiled against an old 
 * version of Xutil.h (e.g. from X11R3).  We define them here if necessary.
 */

#ifndef PBaseSize
#define PBaseSize	(1L << 8) /* program specified base for incrementing */
#endif

#ifndef PWinGravity
#define PWinGravity	(1L << 9) /* program specified window gravity */
#endif

#ifndef WithdrawnState
#define WithdrawnState 0        /* for windows that are not mapped */
#endif

/* a type for the four possible focus modes */
typedef enum { Passive, NoInput, LocallyActive, GloballyActive } FocusMode;

typedef struct {
        long flags;
        enum {
                MENU_FULL = 0,          /* Close, Zoom, Props, Scale, Back,
                                         * Refresh, Quit
                                         */
                MENU_LIMITED = 1,       /* Dismiss, Scale, Refresh */
                MENU_NONE = 2,  	/* No Menu */
        } menu_type;
        int pushpin_initial_state;
	/* numbers assigned as per OLXCI 3/20/89 */
} WMDecorations;

/* pin states numbers, as per OLXCI 3/20/89 */
#define PIN_OUT 0
#define PIN_IN 1

/* value for flags */
#define WMDecorationHeader      (1L<<0)
#define WMDecorationFooter      (1L<<1)
#define WMDecorationPushPin     (1L<<2)
#define WMDecorationCloseButton (1L<<3)
#define WMDecorationHeaderDeco (WMDecorationPushPin | WMDecorationCloseButton)
#define WMDecorationOKButton    (1L<<4)
#define WMDecorationResizeable  (1L<<5)
#ifdef OW_I18N
#define	WMDecorationIMStatus	(1L<<6)
#endif

/* window kinds */
typedef enum { OLBASEWINDOW, OLCMDWINDOW, OLPROPWINDOW, OLHELPWINDOW,
	OLNOTICEWINDOW, OLOTHERWINDOW, OLTRANSIENTWINDOW, OLSPECIAL,
	OLMINIMALDECOR } OLType;

typedef int WMState;	/* use state codes in Xutil.h */

typedef struct {	/* pointer warp state data */
	int		warpToX, warpToY;	/* where to pointer goes on warp */
	int		dflButtonX, dflButtonY;	/* location of default button */
	int		dflButtonW, dflButtonH;	/* size of default button */
	Window		warpBackWin;		/* return warp window */
	int		warpBackX, warpBackY;	/* where to put it back */
} WarpInfo;

/***************************************************************************
* Client structures
***************************************************************************/

/* a top-level client window */

typedef struct _client
{
	OLType		*olType;
	WMDecorations	*wmDecors;
	WMState		wmState;
	NewXSizeHints	*normHints;	/* WM_NORMAL_HINTS */
	XWMHints	*wmHints;	/* WM_HINTS */
	char		*wmInstance;	/* WM_CLASS instance name */
	char		*wmClass;	/* WM_CLASS class name */
	Bool		hasMessages;	/* WM_HINTS has obsolete fields */
	int 		protocols;
	FocusMode	focusMode;
	Bool		isFocus;
	Bool		isSelected;
	Bool		isBusy;
	Display		*dpy;
	int		screen;
	struct _winpaneframe *framewin;	
	struct _winiconframe *iconwin;
	WarpInfo	warpInfo;
	Window		groupid;	/* Actually GroupID */
	unsigned int	groupmask;	/* role that client is playing in group */
	long		busyState;
	List		*colormapWins;
} Client;


/***************************************************************************
* Window class structures
***************************************************************************/

/* classes of window which we can deal with */
typedef enum { WIN_FRAME, WIN_ICON, WIN_RESIZE, WIN_PUSHPIN, WIN_WINBUTTON,
	WIN_PANE, WIN_ICONPANE, WIN_COLORMAP, WIN_MENU, WIN_NOFOCUS, 
	WIN_ROOT, WIN_BUSY } WinKind;

typedef int (*EvFunc)();
typedef int (*IntFunc)();
typedef struct _genericclasscore {
	WinKind kind;
	EvFunc xevents[LASTEvent];	/* function for each X event */
	EvFunc focusfunc;	/* focus state has changed */
	EvFunc drawfunc;	/* draw window */
	EvFunc destroyfunc;	/* destroy window and structures */
	EvFunc selectfunc;	/* selecte state has changed */
	EvFunc newconfigfunc;  	/* compute configuration */
	EvFunc newposfunc;     	/* set position */
	EvFunc setconfigfunc;  	/* set configuration */
	EvFunc createcallback; 	/* used in menu creation */
	IntFunc heightfunc;	/* compute correct height */
	IntFunc widthfunc;	/* compute correct width */
} GenericClassCore;

typedef struct _classgeneric {
	GenericClassCore core;
} ClassGeneric;

typedef struct _genericframeclasscore {
	IntFunc heighttop, heightbottom, widthleft, widthright;
	IntFunc menuPress;
	IntFunc adjustPress, adjustClick;
	IntFunc selectPress, selectClick, selectDoubleClick, selectDrag;
} GenericFrameClassCore;

typedef struct _classgenericframe {
	GenericClassCore core;
	GenericFrameClassCore fcore;
} ClassGenericFrame;

typedef struct _genericpaneclasscore {
	EvFunc setsizefunc;
} GenericPaneClassCore;

typedef struct _classpane {
	GenericClassCore core;
	GenericPaneClassCore pcore;
} ClassGenericPane;

typedef ClassGenericFrame ClassPaneFrame;
typedef ClassGenericFrame ClassIconFrame;
typedef ClassGeneric ClassPushPin;
typedef ClassGenericPane ClassPane;
typedef ClassGenericPane ClassMenu;
typedef ClassGeneric ClassResize;
typedef ClassGeneric ClassButton;
typedef ClassGeneric ClassBusy;
typedef ClassGenericPane ClassIconPane;
typedef ClassGeneric ClassRoot;
typedef ClassGeneric ClassNoFocus;
typedef ClassGeneric ClassColormap;

/* Core functions:
 *
 * destroyfunc - int (*destroyfunc)(Display *dpy, WinGeneric *win)
 *   Called when the window is being destroyed; should destroy any private
 *   resources associated with the window (including possibly destroying
 *   the X window) and destroy the window information structure.
 *
 * selectfunc - int (*selectfunc)(Display *dpy, WinGeneric *win, Bool selected)
 *   Called whenever the window is selected/deselected.  The window should
 *   update its private state -- most particularly, its screen appearance --
 *   to match the new state.  This function is only called when the selection
 *   state has changed.
 *
 * focusfunc - int (*focusfunc)(Display *dpy, WinGeneric *win, Bool focus)
 *   Called whenever the window gains/loses focus.  The window should update
 *   its private state -- most particularly, its screen appearance --
 *   to match the new state.  The window does _not_ need to take focus if
 *   it is active; this has already been taken care of.  This function is
 *   only called when the focus has changed.
 *
 * newconfigfunc - int (*newconfigfunc)(WinGeneric *win, 
 *			XConfigureRequestEvent *xcre)
 *   Called when the configuration of a window should be recomputed.  It
 *   can be assumed that all child windows will have already computed their
 *   sizes.  This function should compute the size of this window, and call
 *   the newposfunc of each child that needs to be moved.  If the configuration
 *   of the window is changed by this function, the dirtyconfig flag should be
 *   set.  The xcre parameter will be be set to a configure request event
 *   structure only if a configure request was received for this
 *   particular window; otherwise the parameter will be null.
 *   Returns True if any dirtyconfig flag has been set.
 *
 * newposfunc - int (*newposfunc)(WinGeneric *win, int x, y)
 *   Instructs a child window to move to a position (x,y).  If this position
 *   is different from the old position, then the dirtyconfig flag
 *   should be set.  Returns True if any dirtyconfig flag has been set.
 *
 * setconfigfunc - int (*setconfigfunc)(Display *dpy, WinGeneric *win)
 *   The window's configuration may have changed; if so, call XConfigureWindow
 *   to make the changes.  Clear the dirtyconfig bits in the process.
 *   The pane window will need to send a synthetic configure notify for
 *   any configuration call.
 *
 * createcallback - int (*createcallback)(WinGeneric *self, Client *cli, 
 *			WinGeneric *parent)
 *   For internally-created panes, the client and parent are filled out
 *   when the StateNew process would normally create a pane.
 *
 * heightfunc - int (*heightfunc)(WinGeneric *self, XConfigureRequestEvent *pxcre)
 *   should return the window's correct height, given a configure request
 *   event and the current environment.  Used to see if a window needs to 
 *   be resized.
 * 
 * widthfunc - same as heightfunc, except that it returns correct width
 */

/***************************************************************************
* Window instance structures
***************************************************************************/

/*
 * The wincore structure contains common information about each window.
 *
 * stack_mode, stack_sib, and dirtyconfig contain information pending window 
 * configuration changes that have not yet been sent to the server.
 *
 * exposures is a list of exposed rectangles that have not yet been repainted.
 *
 * colormapClients is a list of clients that have this window in their 
 * colormapWins list.  tag is used only while processing changes to a client's 
 * colormapWins list.
 */
typedef struct _wincore {
	Window			self;
	WinKind			kind;
	struct _wingeneric 	*parent;
	List			*children;
	Client			*client;
	int			x, y;
	unsigned int		width, height;
	int			stack_mode;
	Window			stack_sib;
	unsigned int		dirtyconfig;
	Colormap		colormap;
	List			*exposures;
	List			*colormapClients;
	Bool			tag;
} WinCore;
/* REMIND maybe add: cursor */

/* macros associated with a window core */
#define WinIsKind(w,k) ((w)->core.kind == (k))
#define WinClass(w) ((w)->class)
#define WinFunc(w,f) ((w)->class->f)

/* FrameCore defines fields common to all types of frame */
typedef struct _winframecore {
	struct _wingenericpane *panewin;	/* pane inside frame */
#ifdef OW_I18N
	wchar_t *name;		/* name to be displayed on frame */
#else
	char *name;		/* name to be displayed on frame */
#endif
	void *menu;		/* actually Menu * */
} WinFrameCore;

/* PaneCore defines fields common to all types of panes */
typedef struct _winpanecore {
	int		oldBorderWidth;
	int		oldSaveUnder;
	int		pendingUnmaps;
} WinPaneCore;

/* Specific window types */

typedef struct _wingeneric {
	ClassGeneric	*class;
	WinCore		core;
} WinGeneric;

typedef struct _wingenericframe {
	ClassGenericFrame *class;
	WinCore 	core;
	WinFrameCore	fcore;
} WinGenericFrame;

/* macros for generic frames */
#define FrameHeightTop(w) (WinFunc((w),fcore.heighttop))((w))
#define FrameHeightBottom(w) (WinFunc((w),fcore.heightbottom))((w))
#define FrameWidthLeft(w) (WinFunc((w),fcore.widthleft))((w))
#define FrameWidthRight(w) (WinFunc((w),fcore.widthright))((w))


typedef struct _wingenericpane {
	ClassGenericPane *class;
	WinCore core;
	WinPaneCore pcore;
} WinGenericPane;

typedef struct _winpaneframe {
	ClassPaneFrame 		*class;
	WinCore			core;
	WinFrameCore		fcore;

	/* footer fields */
#ifdef OW_I18N
	wchar_t			*leftfooter;
#else
	char			*leftfooter;
#endif
	int			leftfooterLength, leftfooterWidth;
#ifdef OW_I18N
	wchar_t			*rightfooter;
#else
	char			*rightfooter;
#endif
	int			rightfooterLength, rightfooterWidth;
#ifdef OW_I18N
	wchar_t			*leftIMstatus;
	int			leftIMstatusLength, leftIMstatusWidth;
	wchar_t			*rightIMstatus;
	int			rightIMstatusLength, rightIMstatusWidth;
#endif

	/* title fields */
	int			titleOff;	/* x offset of title area in pix */
	int			nameLength;	/* length of name in chars */
	int			nameWidth;	/* width of name in pix */
	int			titlex, titley;	/* title position */

	/* pointer warp status */
	Bool			pointerIsWarped;

	/* sizing functions */
	void (*normfullsizefunc)();	/* normal/full size function */
	Bool restoreSet;		/* True => restore values valid */
	int			restoreY, restoreWidth, restoreHeight;
					/* context for full/norm size 
					 * note that y is frame; w/h are pane
					 */

	/* other decorations */
	WinGeneric *winDeco;	/* window button or pushpin */
	struct _winbusy *winBusy; /* special window when busy */
} WinPaneFrame;

typedef struct _winiconframe {
	ClassIconFrame *class;
	WinCore		core;
	WinFrameCore	fcore;
	struct _iconSlot *iconslot;
	Bool		fManuallyPositioned;
	int		nameX, nameY;		/* name of icon */
	int		nameWidth;
	int		nameLength;
} WinIconFrame;

typedef struct _winiconpane {
	ClassIconPane 	*class;
	WinCore 	core;
	WinPaneCore 	pcore;
	Bool		iconClientWindow;	/* true iff client owns pane */
	Pixmap		iconPixmap;		/* None if icon has client window */
#ifdef OBSOLETE
	int		iconX, iconY;		/* geometry of iconDrawable */
	unsigned int	iconWidth, iconHeight;
#endif
} WinIconPane;

typedef struct _winpushpin {
	ClassPushPin *class;
	WinCore		core;
	Bool		pushpinin;
} WinPushPin;

typedef struct _winpane {
	ClassPane *class;
	WinCore 	core;
	WinPaneCore	pcore;
} WinPane;

typedef struct _winmenu {		/* menus are a subclass of panes */
	ClassMenu *class;
	WinCore		core;
	WinPaneCore	pcore;
	Bool		pushpinin;
	void		*menu;	/* actually Menu * */
} WinMenu;

typedef enum {upleft, upright, lowleft, lowright} WhichResize;

typedef struct _winresize {
	ClassResize *class;
	WinCore		core;
	WhichResize	which;
	Bool		depressed;
} WinResize;

typedef struct _winbutton {
	ClassButton *class;
	WinCore		core;
} WinButton;

typedef struct _winbusy {
	ClassBusy *class;
	WinCore core;
	Bool isFocus;
} WinBusy;

typedef WinGeneric WinRoot;
typedef WinGeneric WinNoFocus;

/* colormap windows aren't linked into the ordinary window tree since they
 * are presumed under client control.  We create info structures for them 
 * only so appropriate events can be dispatched on them.  Colormap windows
 * are in a list on the client structure.
 */
typedef WinGeneric WinColormap;

/* functions mapping windows to infos */

extern void WIInstallInfo();
extern Bool WIUninstallInfo();
extern WinGeneric *WIGetInfo();
extern void WIApply();

/* window functions */
extern void WinAddChild();
extern void WinRemoveChild();

/* Useful client macros */
#define PANEOFCLIENT(cli) ((cli)->framewin->fcore.panewin)
#define PANEWINOFCLIENT(cli) (PANEOFCLIENT(cli)->core.self)
