/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

/* @(#) globals.h 50.5 90/12/12 Crucible */

#include "list.h"

typedef struct {
	unsigned int	modmask;
	KeyCode		keycode;
} KeySpec;

typedef enum { BeepAlways, BeepNever, BeepNotices } BeepStatus;

typedef struct _globalResourceVariables {
	char		*DisplayName;
	char		*NameTag;
	char		*WorkspaceColor;
	Bool		F3dUsed;
	Bool		F3dFrames;
	unsigned long	Fg1Color;
	unsigned long	Bg1Color;
	unsigned long	Bg2Color;
	unsigned long	Bg3Color;
	unsigned long	Bg0Color;
	unsigned long	BorderColor;
	unsigned long	CursorColor;
#ifdef OW_I18N
	XFontSetInfo 	TitleFontSetInfo;
	XFontSetInfo	TextFontSetInfo;
	XFontSetInfo 	ButtonFontSetInfo;
	XFontSetInfo	IconFontSetInfo;
#else
	XFontStruct    	*TitleFontInfo;
	XFontStruct	*TextFontInfo;
	XFontStruct    	*ButtonFontInfo;
	XFontStruct	*IconFontInfo;
#endif
	XFontStruct	*GlyphFontInfo;
	Cursor		BasicPointer;
	Cursor		MovePointer;
	Cursor		BusyPointer;
	Cursor		IconPointer;
	Cursor		ResizePointer;
	Cursor		MenuPointer;
	Cursor		QuestionPointer;
	Cursor		TargetPointer;
	Cursor		PanPointer;
	Bool		FocusFollowsMouse;
	Bool		ReparentFlag;
#ifdef OW_I18N
	wchar_t		*DefaultWinName;	/* DEFAULTWINNAME */
#else
	char		*DefaultWinName;	/* DEFAULTWINNAME */
#endif
	int		SaveWorkspaceTimeout;
	int		FlashTime;
	Bool		FShowMenuButtons;
	Bool		FShowPinnedMenuButtons;
	IconPreference	IconPlacement;
	Bool		FSnapToGrid;
	Bool		FocusLenience;
	Bool		DragWindow;
	Bool		AutoRaise;
	Bool		PopupJumpCursor;
	Bool		ColorFocusLocked;
	Bool		PPositionCompat;
	Bool		RefreshRecursively;
	BeepStatus	Beep;
	int		EdgeThreshold;
	int		DragRightDistance;
	int		MoveThreshold;
	int		ClickMoveThreshold;
	int		DoubleClickTime;
	int		RubberBandThickness;
	KeySpec		FrontKey;
	KeySpec		HelpKey;
	KeySpec		OpenKey;
	KeySpec		ConfirmKey;
	KeySpec		CancelKey;
	KeySpec		ColorLockKey;
	KeySpec		ColorUnlockKey;
	List		*Minimals;
#ifdef OW_I18N
	OLLC		LC;
	Bool		BoldFontEmulation;
#endif OW_I18N
/* Following are three entries are strictly for debugging purposes and 
 * are not mentioned in the usage message or doc.
 * Orphaned events are events that are associated with a window or frame 
 * has no entry in the frame hash table, or events that are not handled by the
 * various event handlers.
 * 'PrintAll' is useful for when verification of an events existance is needed.
 */
	Bool		PrintOrphans;
	Bool		PrintAll;
	Bool		Synchronize;
} GlobalResourceVariables;

extern GlobalResourceVariables	GRV;

