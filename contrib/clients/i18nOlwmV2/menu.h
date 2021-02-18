/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

/* @(#) menu.h 50.4 90/12/12 Crucible */

/* a handy typedef for pointers to functions returning int */
typedef int (*FuncPtr)();

/* Button stacks (menus) are implemented as lists of buttons.
 * Each button in a stack may in turn be stacked, this is indicated by
 * the stacked flag of the button. If this flag is True then the buttonAction
 * is a pointer to a new stack. Otherwise it is the function to be called
 * after the menu has been dispatched.
 */
typedef struct {
	FuncPtr		callback;	/* if not stacked; call this */
	struct _menu	*submenu;	/* actually a Menu pointer, but */
					/* Menu hasn't been defined yet */
} ButtonAction;

/* possible button states */
typedef enum {Disabled, Enabled} ButtonState;

typedef struct _button {
#ifdef OW_I18N
	wchar_t		*label;		/* displayed text */
#else
	char		*label;		/* displayed text */
#endif
	Bool		stacked;	/* True if this is a button stack */
	ButtonState	state;		/* Enabled/Disabled */
	ButtonAction	action;

	/* following are filled in when menu is created */
	int		activeX, activeY, activeW, activeH;
} Button;

typedef struct _menu {
#ifdef OW_I18N
	wchar_t		*title;
#else
	char		*title;
#endif
	Button		*buttons;
	int		buttonCount;
	int		buttonDefault;
	Bool		hasPushPin;
	FuncPtr		pinAction;

	/* filled in when menu is created */
	Window		window;
	Window		shadow;
	int		titleHeight, titleWidth, titleX, titleY;
	int		pushPinX, pushPinY;
	int		x, y;
	int		width, height;

	Bool		currentlyDisplayed;  /* If this is true we gray out
					      * the pushpin when we display
					      * the menu. */
	struct _menu *originalMenu;	/* if this menu is pinned, this
					 * points to the original menu
					 * template */
#ifdef OW_I18N
	WinMenu		*PinWin;	/*
					 * If there are pinned menu
					 * for this menu, this will be
					 * a pinned menu.  This makes
					 * easy to access to the
					 * winInfo while destroying
					 * the menu.
					 */
#endif
} Menu;

/*
 * The MenuInfo structure contains information about currently active
 * menus.  There is one MenuInfo for each active pop-up menu (not pinned 
 * menu).  MenuInfo structures are destroyed when the pop-up menu goes down.
 */
typedef struct {
	Menu		*menu;
	struct _wingeneric *winInfo;
	Bool		childActive;
	Menu		*childMenu;
	Bool		pinIn;			/* current pinState */
	int		litButton;		/* highlighted button */
	Bool		ignoreNextExpose;
} MenuInfo;

/* external functions */
extern void InitMenus(/* dpy */);
extern void MenuCreate(/* dpy, menu */);
extern void MenuShow(/* dpy, WinGeneric, menu, event */);
extern void SetButton(/* dpy, menu, bindex, Bool */);
extern void ExecButtonAction(/* dpy, winInfo, menu, btn, Bool */);
extern void DrawMenu(/* dpy, menu */);
extern int  PointInRect(/* x, y, rx, ry, rw, rh */);
