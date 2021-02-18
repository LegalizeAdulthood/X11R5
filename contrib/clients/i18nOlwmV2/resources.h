/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

/* @(#) resources.h 50.4 90/12/12 Crucible */

/* useful macros */
#define	MakeRMName( buf, idx )	( strcpy( buf, GRV.NameTag ), \
				strcat( buf, RMTable[idx].instance ) )
#define	MakeRMClass( buf, idx )	( strcpy( buf, CLASSTAG ), \
				strcat( buf, RMTable[idx].class ) )
#define	RMFunc( d, idx, b )	( RMTable[idx].setFunction( d, idx, b ) )

/* maximum length of fully-specified instance/class name */
#define	MAX_NAME	100
#define	MAX_CLASS	100

#define	CLASSTAG	"OpenWindows"	/* value for initial class */

#define BASICPTR	0
#define MOVEPTR		1
#define BUSYPTR		2
#define ICONPTR		3
#define RESIZEPTR	4
#define MENUPTR		5
#define QUESTIONPTR	6
#define TARGETPTR	7
#define PANPTR		8
#define NUM_CURSORS	9		/* number of pointers supported */


typedef Bool (*BFuncPtr)();

typedef struct _resourceInfo {
	char	*instance;	/* without NameTag prefix */
	char	*class;		/* without CLASSTAG prefix */
	char	*defaultVal;	/* string representation */
	void	*varAddr;	/* address of global var in GRV struct */
	BFuncPtr setFunc;	/* takes dpy, RMTable index, bool as args */
	BFuncPtr updFunc;	/* takes dpy, RMTable index, bool as args */
#ifdef OW_I18N
	Bool	localeDep;	/* Locale dependent or independent */
#endif
} ResourceInfo;

extern	ResourceInfo	RMTable[];

/* resource names as indices into RMTable */
#define	RM_DISPLAY		0
#define	RM_PREFIX		1
#define	RM_WORKSPACECOLOR	2
#define	RM_USE3D		3
#define	RM_BACKGROUND		4
#define	RM_FOREGROUND		5
#define	RM_BORDERCOLOR		6
#define	RM_CURSORCOLOR		7
#define	RM_TITLE_FONT		8
#define	RM_TEXT_FONT		9
#define	RM_BUTTON_FONT		10
#define	RM_ICON_FONT		11
#define	RM_GLYPHFONT		12	/* not in Font class */
#define	RM_CURSORFONT		13	/* not in Font class */
#define	RM_INPUTFOCUS		14
#define	RM_REPARENTOK		15
#define	RM_DEFAULTTITLE		16
#define	RM_FLASHTIME		17
#define	RM_POPBOUTLINE		18	/* popup menu button outline */
#define	RM_PINBOUTLINE		19	/* pinned menu button outline */
#define	RM_ICONLOCATION		20
#define	RM_FOCUSLENIENCE	21
#define	RM_WINDOWDRAG		22
#define	RM_AUTORAISE		23
#define	RM_DRAGRIGHT		24
#define	RM_MMOVETHRESH		25	/* mouse move threshold */
#define	RM_CMOVETHRESH		26	/* multi-click move threshold */
#define	RM_CLICKTIME		27
#define	RM_FRONTKEY		28
#define	RM_HELPKEY		29
#define	RM_OPENKEY		30
#define	RM_CONFIRMKEY		31
#define	RM_ORPHANS		32
#define	RM_PRINTALL		33
#define	RM_SYNCH		34
#define	RM_SNAPTOGRID		35
#define RM_SAVEWORKSPACETIMEOUT 36
#define RM_POPUPJUMPCURSOR	37
#define RM_CANCELKEY		38
#define RM_COLORLOCKKEY		39
#define RM_COLORUNLOCKKEY	40
#define RM_COLORFOCUSLOCKED	41
#define RM_EDGEEFFECTDIST       42
#define RM_RUBBERBANDTHICKNESS	43
#define RM_BEEPSETTING          44
#define RM_PPOSITIONCOMPAT	45
#define RM_MINIMALDECORS	46
#define	RM_USE3DFRAMES		47
#define	RM_REFRESHRECURSIVELY	48
#ifdef OW_I18N
/*
 * In order to implement priority mechanism, I need separate entry for
 * command line and real Xrm.
 */
#define RM_LC_BEGIN		49

#define	RM_LC_BASIC_LOCALE	49	/* Really from Xrm */
#define	RM_LC_BASIC_LOCALE_CL	50	/* Actually came from command line */
#define	RM_LC_DISPLAY_LANG	51
#define	RM_LC_DISPLAY_LANG_CL	52
#define	RM_LC_INPUT_LANG	53
#define	RM_LC_INPUT_LANG_CL	54
#define	RM_LC_NUMERIC		55
#define	RM_LC_NUMERIC_CL	56
#define	RM_LC_TIME_FORMAT	57
#define	RM_LC_TIME_FORMAT_CL	58

#define RM_LC_END		59

#define	RM_BOLDFONTEMULATION	59
#define RM_ENTRYEND		60	/* number of entries in table */
#else
#define RM_ENTRYEND		49	/* number of entries in table */
#endif

/* resource set functions */
static Bool	setBoolResource();
static Bool	setFontResource();
static Bool	setGlyphFont();
static Bool	setCursors();
static Bool	setCharResource();
#ifdef OW_I18N
static Bool	setWCharResource();
#endif
static Bool	setFloatResource();
static Bool	setIntResource();
static Bool	setClickTimeout();
static Bool	setFg1Color();
static Bool	setBg1Color();
static Bool	setBg2Color();
static Bool	setBg3Color();
static Bool	setBg0Color();
static Bool	setBg0Percent();
static Bool	setColor();
static Bool	setInputFocusStyle();
static Bool	setIconLocation();
static Bool	setKey();
static Bool	setDimension();
#ifdef OW_I18N
static Bool	setOLLC();
static Bool	setOLLCCL();
#endif
Bool		SetNameTag();

/* resource update functions */
void	UpdateDBValues();
Bool	UpdWorkspace();
Bool	UpdSync();
Bool	UpdTitleFont();
Bool	UpdTextFont();
Bool	UpdButtonFont();
Bool	UpdIconFont();
Bool	UpdGlyphFont();
Bool	UpdFg1Color();
Bool	UpdBg0Percent();
Bool	UpdBg0Color();
Bool	UpdBg1Color();
Bool	UpdBg2Color();
Bool	UpdBg3Color();
Bool	UpdCursorColor();

#ifdef OW_I18N
Bool	EffectOLLC();		/* Take effect the setting of OLLC */
#endif
