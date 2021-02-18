/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) resources.c 50.9 91/08/26 Crucible";
#ifdef OW_I18N
#include "i18n.h"
#endif
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

#include <olgx/olgx.h>

#ifdef OW_I18N
#include "ollocale.h"
#endif
#include "mem.h"
#include "olwm.h"
#include "win.h"
#include "defaults.h"
#include "globals.h"
#include "resources.h"
#include "olcursor.h"

extern	char		*AppName;
extern	Bool 		ColorDisplay;
extern	List		*ActiveClientList;

extern 	Graphics_info	*olgx_gisbutton;
extern 	Graphics_info	*olgx_gistext;
extern 	Graphics_info	*olgx_gisnormal;
extern 	Graphics_info	*olgx_gisreverse;
extern 	Graphics_info	*olgx_gisrevpin;

extern	GC		DrawNormalGC, DrawReverseGC;
extern	GC		DrawLinesGC, DrawRevLinesGC;
extern	GC		DrawBackgroundGC, DrawSelectedGC;
extern	GC		DrawBusyGC;
extern	GC		IconUnselectedGC, IconBorderGC;
extern	GC		IconNormalGC, IconSelectedGC;

extern Pixmap Gray50;
extern int Gray50width;
extern int Gray50height;

XrmDatabase		OlwmDB = NULL;
GlobalResourceVariables	GRV;

Bool WorkspaceColorUsed = False;
unsigned long WorkspaceColorPixel;

static Bool windowColorsAllocated = False;

extern void FrameSetupGrabs();

/* forward function definitions */
static Bool setMinimalDecor();
static Bool setBeepResource();
static Bool updIconLocation();
static Bool updBorderColor();

Bool UpdInputFocusStyle();

/* NOTICE that the optionTable for command line entries is mostly 
 * independent of this table.  BOTH need to be modified in case of
 * changes or additions.
 */
/* quantity of/indices into the RMTable are #define'd in resources.h */
ResourceInfo	RMTable[] = {
	{ ".display",			".Display",		NULL,	
	    &(GRV.DisplayName),		(BFuncPtr)0,		(BFuncPtr)0 },
	{ ".name",			".Name",		"olwm", 
	    &(GRV.NameTag),		SetNameTag,		(BFuncPtr)0 },
	{ ".workspaceColor",		".WorkspaceColor",	NULL, 
	    &(GRV.WorkspaceColor),	setCharResource,	UpdWorkspace },
	{ ".use3D",			".Use3D",		"True", 
	    &(GRV.F3dUsed),		setDimension,		(BFuncPtr)0 },
	{ ".WindowColor", 		".Background",		NULL, 
	    &(GRV.Bg1Color),		setBg1Color,		UpdBg1Color },
	{ ".foreground",		".Foreground",		NULL, 
	    &(GRV.Fg1Color),		setFg1Color,		UpdFg1Color },
	{ ".borderColor",		".Foreground",		NULL, 
	    &(GRV.BorderColor),		setColor,		updBorderColor },
	{ ".cursorColor",		".Foreground",		NULL, 
	    &(GRV.CursorColor),		setColor,		UpdCursorColor },
/* NOTICE that font defaults are names, although variables are font infos */
#ifdef LATER	/* ideal name/class setup */
	{ ".title.font",		".Title.Font",	
	    "-b&h-lucida-bold-r-normal-sans-*-120-*-*-*-*-*-*", 
	    &(GRV.TitleFontInfo),	setFontResource,	UpdTitleFont },
	{ ".text.font",			".Text.Font",	
	    "-b&h-lucida-medium-r-normal-sans-*-120-*-*-*-*-*-*", 
	    &(GRV.TextFontInfo),	setFontResource, 	UpdTextFont },
	{ ".button.font",		".Button.Font",	
	     "-b&h-lucida-medium-r-normal-sans-*-120-*-*-*-*-*-*",
	    &(GRV.ButtonFontInfo),	setFontResource, 	UpdButtonFont },
	{ ".icon.font",			".Icon.Font",	
	     "-b&h-lucida-medium-r-normal-sans-*-120-*-*-*-*-*-*",
	    &(GRV.IconFontInfo),	setFontResource,	UpdIconFont },
#else
	{ ".titleFont",			".TitleFont",	
	    "-b&h-lucida-bold-r-normal-sans-*-120-*-*-*-*-*-*", 
#ifdef OW_I18N
	    &(GRV.TitleFontSetInfo),	setFontResource,	UpdTitleFont,
	    True },
#else
	    &(GRV.TitleFontInfo),	setFontResource,	UpdTitleFont },
#endif
	{ ".textFont",			".TextFont",	
	    "-b&h-lucida-medium-r-normal-sans-*-120-*-*-*-*-*-*", 
#ifdef OW_I18N
	    &(GRV.TextFontSetInfo),	setFontResource, 	UpdTextFont,
	    True },
#else
	    &(GRV.TextFontInfo),	setFontResource, 	UpdTextFont },
#endif
	{ ".buttonFont",		".MenuButtonFont",	
	     "-b&h-lucida-medium-r-normal-sans-*-120-*-*-*-*-*-*",
#ifdef OW_I18N
	    &(GRV.ButtonFontSetInfo),	setFontResource, 	UpdButtonFont,
	    True },
#else
	    &(GRV.ButtonFontInfo),	setFontResource, 	UpdButtonFont },
#endif
	{ ".iconFont",			".IconFont",	
	     "-b&h-lucida-medium-r-normal-sans-*-120-*-*-*-*-*-*",
#ifdef OW_I18N
	    &(GRV.IconFontSetInfo),	setFontResource,	UpdIconFont,
	    True },
#else
	    &(GRV.IconFontInfo),	setFontResource,	UpdIconFont },
#endif
#endif /* LATER */
	{ ".glyphFont",			".GlyphFont",	
	     "-sun-open look glyph-*-*-*-*-*-120-*-*-*-*-*-*",
#ifdef OW_I18N
	    &(GRV.GlyphFontInfo),	setGlyphFont,		UpdGlyphFont,
	    True },
#else
	    &(GRV.GlyphFontInfo),	setGlyphFont,		UpdGlyphFont },
#endif
	{ ".cursorFont",		".CursorFont",	
	     "-sun-open look cursor-*-*-*-*-*-120-*-*-*-*-*-*",
	    &(GRV.BasicPointer),	setCursors,		(BFuncPtr)0 },
	{ ".inputFocusStyle",		".SetInput",		"Select",
	    &(GRV.FocusFollowsMouse),	setInputFocusStyle,	UpdInputFocusStyle },
	{ ".reparentOK",		".ReparentOK",		"True", 
	    &(GRV.ReparentFlag),	setBoolResource,	(BFuncPtr)0 },
#ifdef OW_I18N
	{ ".defaultTitle",		".DefaultTitle",	"No Name", 
	    &(GRV.DefaultWinName),	setWCharResource,	 (BFuncPtr)0,
	    True },
#else
	{ ".defaultTitle",		".DefaultTitle",	"No Name", 
	    &(GRV.DefaultWinName),	setCharResource,	 (BFuncPtr)0 },
#endif
	{ ".flashFrequency",		".FlashFrequency",	"100000", 
	    &(GRV.FlashTime),		setIntResource,		(BFuncPtr)0 },
	{ ".menu.popup.outlineButton",	".Menu.Popup.OutlineButton", "False",
	    &(GRV.FShowMenuButtons),	setBoolResource,	 (BFuncPtr)0 },
	{ ".menu.pinned.outlineButton",	".Menu.Pinned.OutlineButton", "False",
	    &(GRV.FShowPinnedMenuButtons), setBoolResource,	(BFuncPtr)0 },
	{ ".iconLocation",		".IconLocation", 	"Top",
	    &(GRV.IconPlacement),	setIconLocation,	updIconLocation },
	{ ".focusLenience",		".FocusLenience", 	"False",
	    &(GRV.FocusLenience),	setBoolResource,	(BFuncPtr)0 },
	{ ".dragWindow",		".DragWindow", 		"False",
	    &(GRV.DragWindow),		setBoolResource,	(BFuncPtr)0 },
	{ ".autoRaise",			".AutoRaise",		"False",
	    &(GRV.AutoRaise),		setBoolResource,	(BFuncPtr)0 },
	{ ".dragRightDistance",		".DragRightDistance",	"100",
	    &(GRV.DragRightDistance),	setIntResource,		(BFuncPtr)0 },
	{ ".moveThreshold",		".MoveThreshold",	"5",
	    &(GRV.MoveThreshold),	setIntResource,		(BFuncPtr)0 },
	{ ".clickMoveThreshold",	".ClickMoveThreshold",	"5",
	    &(GRV.ClickMoveThreshold),	setIntResource,		(BFuncPtr)0 },
	{ ".mouseClickTimeout",		".MultiClickTimeout",	"500",
	    &(GRV.DoubleClickTime),	setClickTimeout,	(BFuncPtr)0 },
/* NOTICE that key defaults are string reps of syms, but vars are keycodes */
	{ ".frontKey",			".FrontKey",		"Any L5",
	    &(GRV.FrontKey),		setKey,			(BFuncPtr)0 },
	{ ".helpKey",			".HelpKey",		"Help",
	    &(GRV.HelpKey),		setKey,			(BFuncPtr)0 },
	{ ".openKey",			".OpenKey",		"Any L7",
	    &(GRV.OpenKey),		setKey,			(BFuncPtr)0 },
	{ ".confirmKey",		".ConfirmKey",		"Return",
	    &(GRV.ConfirmKey),		setKey,			(BFuncPtr)0 },
	{ ".print.orphans",		".Print.Debug",		"False", 
	    &(GRV.PrintOrphans),	setBoolResource,	(BFuncPtr)0 },
	{ ".print.all",			".Print.Debug",		"False", 
	    &(GRV.PrintAll),		setBoolResource,	(BFuncPtr)0 },
	{ ".synchronize",		".Debug",		"False", 
	    &(GRV.Synchronize),		setBoolResource,	UpdSync },
	{ ".snapToGrid",		".SnapToGrid",		"False",
	    &(GRV.FSnapToGrid),		setBoolResource,	(BFuncPtr)0 },
	{ ".saveWorkspaceTimeout",	".SaveWorkspaceTimeout", "10",
	    &(GRV.SaveWorkspaceTimeout), setIntResource,	(BFuncPtr)0 },
	{ ".popupJumpCursor",		".PopupJumpCursor",	"True",
	    &(GRV.PopupJumpCursor),	setBoolResource,	(BFuncPtr)0 },
	{ ".cancelKey",			".CancelKey",		"Escape",
	    &(GRV.CancelKey),		setKey,			(BFuncPtr)0 },
	{ ".colorLockKey",		".ColorLockKey",	"Control L2",
	    &(GRV.ColorLockKey),	setKey,			(BFuncPtr)0 },
	{ ".colorUnlockKey",		".ColorUnlockKey",	"Control L4",
	    &(GRV.ColorUnlockKey),	setKey,			(BFuncPtr)0 },
	{ ".colorFocusLocked",		".ColorFocusLocked",	"False",
	    &(GRV.ColorFocusLocked),	setBoolResource,	(BFuncPtr)0 },
	{ ".edgeMoveThreshold",		".EdgeMoveThreshold", 	"10",
	    &(GRV.EdgeThreshold),	setIntResource,		(BFuncPtr)0 },
	{ ".rubberBandThickness",	".RubberBandThickness",	"2",
	    &(GRV.RubberBandThickness),	setIntResource,		(BFuncPtr)0 },
	{ ".beep",			".Beep",		"always",
	    &(GRV.Beep),		setBeepResource,	(BFuncPtr)0 },
	{ ".pPositionCompat",		".PPositionCompat",	"false",
	    &(GRV.PPositionCompat),	setBoolResource,	(BFuncPtr)0 },
	{ ".minimalDecor",		".MinimalDecor",	"",
	    &(GRV.Minimals),		setMinimalDecor,	(BFuncPtr)0 },
	{ ".use3DFrames",		".Use3DFrames",		"False", 
	    &(GRV.F3dFrames),		setBoolResource,	(BFuncPtr)0 },
	{ ".refreshRecursively",	".RefreshRecursively",	"True",
	    &(GRV.RefreshRecursively),	setBoolResource,	(BFuncPtr)0 },
#ifdef OW_I18N
	{ ".basicLocale",		".BasicLocale",		NULL,
	    &(GRV.LC.BasicLocale),	setOLLC,		(BFuncPtr)0 },
	{ ".basicLocaleCL",		".BasicLocaleCL",	NULL,
	    &(GRV.LC.BasicLocale),	setOLLCCL,		(BFuncPtr)0 },

	{ ".displayLang",		".DisplayLang",		NULL,
	    &(GRV.LC.DisplayLang),	setOLLC,		(BFuncPtr)0 },
	{ ".displayLangCL",		".DisplayLangCL",	NULL,
	    &(GRV.LC.DisplayLang),	setOLLCCL,		(BFuncPtr)0 },

	{ ".inputLang",			".InputLang",		NULL,
	    &(GRV.LC.InputLang),	setOLLC,		(BFuncPtr)0 },
	{ ".inputLangCL",		".InputLangCL",		NULL,
	    &(GRV.LC.InputLang),	setOLLCCL,		(BFuncPtr)0 },

	{ ".numeric",			".Numeric",		NULL,
	    &(GRV.LC.Numeric),		setOLLC,		(BFuncPtr)0 },
	{ ".numericCL",			".NumericCL",		NULL,
	    &(GRV.LC.Numeric),		setOLLCCL,		(BFuncPtr)0 },

	{ ".timeFormat",		".TimeFormat",		NULL,
	    &(GRV.LC.TimeFormat),	setOLLC,		(BFuncPtr)0 },
	{ ".timeFormatCL",		".TimeFormatCL",	NULL,
	    &(GRV.LC.TimeFormat),	setOLLCCL,		(BFuncPtr)0 },

	{ ".boldFontEmulation",		".BoldFontEmulation",	"False",
	    &(GRV.BoldFontEmulation),	setBoolResource,	(BFuncPtr)0,
	    True },
#endif OW_I18N
};

#ifdef OW_I18N
Bool OLLCUpdated;
#endif


/* private data */

static Bool matchString();
static Bool matchBool();
static Bool matchFocusType();
static Bool matchIconPlace();
static Bool matchBeepKeyword();
static Bool checkClass();
static void setWorkspaceColor();


#define BSIZE 100

/*
 * Determine whether value matches pattern, irrespective of case.
 * This routine is necessary because not all systems have strcasecmp().
 */
static Bool
matchString( value, pattern )
char *value;
char *pattern;
{
    char buf[BSIZE];
    char *p;

    strncpy(buf,value,BSIZE);
    buf[BSIZE-1] = '\0'; /* force null termination */

    for ( p=buf; *p; ++p )
	if (isupper(*p))
	    *p = tolower(*p);

    return 0 == strcmp(buf,pattern);
}


/*
 * Match any of the following booleans: yes, no, 1, 0, on, off, t, nil, 
 * true, false.  Pass back the boolean matched in ret, and return True.  
 * Otherwise, return False.
 *
 * matchString is case independent.
 */
static Bool
matchBool( value, ret )
char *value;
Bool *ret;
{
    if (matchString(value,"yes") ||
	matchString(value,"on") ||
	matchString(value,"t") ||
	matchString(value,"true") ||
	(0 == strcmp(value,"1"))) {
	*ret = True;
	return True;
    }

    if (matchString(value,"no") ||
	matchString(value,"off") ||
	matchString(value,"nil") ||
	matchString(value,"false") ||
	(0 == strcmp(value,"0"))) {
	*ret = False;
	return True;
    }

    return False;
}

/*
 * Match any of the following input focus types: followmouse, follow, f, 
 * select, s, click, clicktotype, c.  Pass back True for focusfollows or 
 * False for clicktotype in ret (since FocusFollowsMouse is the global
 * corresponding to this resource), and return True.  
 * Otherwise, return False.
 *
 * matchString is case independent.
 */
static Bool
matchFocusType( value, ret )
char *value;
Bool *ret;
{
	if ( matchString(value, "followmouse")
	     || matchString(value, "follow") 
	     || matchString(value,"f") )
	{
		*ret = True;
		return True;
	}

	if ( matchString(value, "select")
	     || matchString(value, "click") 
	     || matchString(value,"clicktotype")
	     || matchString(value,"c")
	     || matchString(value,"s") )
	{
		*ret = False;
		return True;
	}

	return False;
}

/*
 * Match any of the three possible beep keywords:  always, never, or notices.
 * Pass back the BeepStatus value by reference, and return True, if
 * a match was found; otherwise return False and do not disturb the
 * passed value.
 */
static Bool
matchBeepKeyword(value,ret)
char *value;
BeepStatus *ret;
{
	if (matchString(value,"always"))
	{
	    *ret = BeepAlways;
	    return True;
	}
	if (matchString(value,"never"))
	{
	    *ret = BeepNever;
	    return True;
	}
	if (matchString(value,"notices"))
	{
	    *ret = BeepNotices;
	    return True;
	}
	return False;
}

/*
 * Match any of the following icon placement locations: top, bottom, right, 
 * left, s, click, clicktotype, c.  Pass back the IconPreference 
 * in ret, and return True.  
 * Otherwise, return False.
 *
 * matchString is case independent.
 */
static Bool
matchIconPlace( value, ret )
char		*value;
IconPreference	*ret;
{
	if (matchString(value, "top"))
	{
		*ret = AlongTop;
		return True;
	}
	if (matchString(value, "bottom"))
	{
		*ret = AlongBottom;
		return True;
	}
	if (matchString(value, "right"))
	{
		*ret = AlongRight;
		return True;
	}
	if (matchString(value, "left"))
	{
		*ret = AlongLeft;
		return True;
	}
	if (matchString(value, "top-lr"))
	{
		*ret = AlongTop;
		return True;
	}
	if (matchString(value, "top-rl"))
	{
		*ret = AlongTopRL;
		return True;
	}
	if (matchString(value, "bottom-lr"))
	{
		*ret = AlongBottom;
		return True;
	}
	if (matchString(value, "bottom-rl"))
	{
		*ret = AlongBottomRL;
		return True;
	}
	if (matchString(value, "right-tb"))
	{
		*ret = AlongRight;
		return True;
	}
	if (matchString(value, "right-bt"))
	{
		*ret = AlongRightBT;
		return True;
	}
	if (matchString(value, "left-tb"))
	{
		*ret = AlongLeft;
		return True;
	}
	if (matchString(value, "left-bt"))
	{
		*ret = AlongLeftBT;
		return True;
	}

	return False;
}

/*
 * checkClass - Check to see if the named class is the same as (or a subset of)
 * 	the compareClass specified.
 *
 *	Note that the fullClassname must be consistently bound!
 *	(If combine loose and tight bindings, need to convert to using 
 *	XrmStringToBindingQuarkList().)
 */
static Bool
checkClass( fullClassname, compareClass )
char	*fullClassname;
char	*compareClass;
{
#define	MAX_QUARKS	20
	XrmQuark	classQuarks[MAX_QUARKS];
	XrmQuark	compareQuark;
	int		ii;

	compareQuark = XrmStringToQuark( compareClass );
	XrmStringToQuarkList( fullClassname, classQuarks );
	for ( ii = 0 ;  (ii < MAX_QUARKS) && (classQuarks[ii] != NULL) ; ii++ )
		if ( compareQuark == classQuarks[ii] )
			return True;

	return False;
}


/*
 *	The Resource Set Functions
 */

/*
 * setBoolResource - set value of a bool resource
 *
 */
/*ARGSUSED*/	/* dpy is std arg for Set fns */
static Bool
setBoolResource( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	Bool	newBoolValue;
	Bool	updateNeeded = False;
#define RM_BOOLVAR     *((Bool *)(RMTable[rmIndex].varAddr))

	if ( (newValue == NULL) || !matchBool( newValue, &newBoolValue ) )
	{
		if ( varSet )
			return( updateNeeded );
		else	/* builtin default */
			matchBool( RMTable[rmIndex].defaultVal, &newBoolValue );
	}
	else if ( varSet )	/* a valid newValue and an old value */
	{
		if ( newBoolValue == RM_BOOLVAR )
			return( updateNeeded );
		else
			updateNeeded = True;
	}

	RM_BOOLVAR = newBoolValue;
	return( updateNeeded );
}

/*
 * setFontResource - set font info variable from db resource value 
 *
 *	Notice that since comparing XFontStructs would be rather messy,
 *	we cheat and simply compare the font names in the database.
 */
static Bool
setFontResource( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
#ifdef OW_I18N
	XFontSet		tmpFontSet;
#else
	XFontStruct		*tmpFontInfo;
#endif
	Bool			updateNeeded = False;
	char			name[MAX_NAME];
	char			class[MAX_CLASS];
	XrmRepresentation	rmType;
	XrmValue		rmValue;
#ifdef OW_I18N
	XFontSet		loadQueryFontSet();
	char			*locale;
#endif

#ifdef OW_I18N
#define RM_FONTVAR     ((XFontSetInfo *)(RMTable[rmIndex].varAddr))
#else
#define RM_FONTVAR     *((XFontStruct **)(RMTable[rmIndex].varAddr))
#endif

#ifdef OW_I18N
	locale = setlocale(LC_CTYPE, NULL);
	/*
	 * Doing XLoadQueryXXX is too expensive for Kanji and other
	 * Asian character sets.
	 */
	if ( newValue == NULL
	  || ( RM_FONTVAR->fsn != NULL
			&& (strcmp( RM_FONTVAR->fsn, newValue ) == 0) )
	  || (( tmpFontSet = loadQueryFontSet(dpy, newValue, locale)) == NULL ))
#else
	if ( ( newValue == NULL ) ||
	     ( (tmpFontInfo = loadQueryFont(dpy, newValue)) == NULL ) )
#endif
	{
		if ( varSet )
			return( updateNeeded );
#ifdef OW_I18N
		if (newValue != NULL)
			fprintf(stderr, gettext("\
olwm: Warning: could not open '%s' font set for '%s' locale,\n\
      use default '%s' as '%s' locale...\n"),
				newValue, locale,
				RMTable[rmIndex].defaultVal,
				DEFAULTFONT_LOCALE);
		if ( ( tmpFontSet = 
			    loadQueryFontSet(dpy,
			    		RMTable[rmIndex].defaultVal,
					DEFAULTFONT_LOCALE) )
			!= NULL )
			newValue = RMTable[rmIndex].defaultVal;
		else
		{
		fprintf(stderr, gettext("olwm: Warning: could not open default font set, use %s...\n"),
			DEFAULTFONT);
                	if ((tmpFontSet
				= loadQueryFontSet(dpy,
					DEFAULTFONT, DEFAULTFONT_LOCALE))
			     != NULL)
				ErrorGeneral(gettext("Could not open any font set"));
			
			newValue = DEFAULTFONT;
		}
#else
		else if ( ( tmpFontInfo = 
			    XLoadQueryFont(dpy, RMTable[rmIndex].defaultVal) )
			  == NULL )
                	tmpFontInfo = XLoadQueryFont(dpy, DEFAULTFONT);
#endif
	}
	else if ( varSet )
	{
		MakeRMName( name, rmIndex );
		MakeRMClass( class, rmIndex );
		/* compare newValue with old value - REMIND if default was used
		 * last time, and newValue was now set to it vi xrdb, then we 
		 * may do an update unnecessarily (since value will not be in 
		 * OlwmDB, so XrmGetResource will return False and updateNeeded
		 * will be set to True, since we can't strcmp the value)
		 */
		if ( XrmGetResource( OlwmDB, name, class, &rmType, &rmValue ) 
		     && !strcmp( newValue, rmValue.addr ) )
			return( updateNeeded );
		else
			updateNeeded = True;
	}

#ifdef OW_I18N
	if (RM_FONTVAR->fs != NULL)
	{
		freeFontSet(dpy, RM_FONTVAR->fs);
		free(RM_FONTVAR->fsn);
	}
	RM_FONTVAR->fs = tmpFontSet;
	RM_FONTVAR->fs_extents = XExtentsOfFontSet(tmpFontSet);
	RM_FONTVAR->fsn = MemNewString(newValue);
#else
	RM_FONTVAR = tmpFontInfo;
#endif
	return( updateNeeded );
#ifdef OW_I18N
#undef RM_FONTVAR
#endif
}

/*
 * setGlyphFont - set glyph font info variable from db resource value 
 *
 *	Notice that since comparing XFontStructs would be rather messy,
 *	we cheat and simply compare the font names in the database.
 *
 *	This is essentially the same as SetFontResource, except that
 *	the absence of a valid glyph font is a fatal error.
 */
static Bool
setGlyphFont( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	XFontStruct		*tmpFontInfo;
	Bool			updateNeeded = False;
	char			name[MAX_NAME];
	char			class[MAX_CLASS];
	XrmRepresentation	rmType;
	XrmValue		rmValue;

#ifndef RM_FONTVAR
#define RM_FONTVAR     *((XFontStruct **)(RMTable[rmIndex].varAddr))
#endif

	if ( ( newValue == NULL ) ||
	     ( (tmpFontInfo = XLoadQueryFont(dpy, newValue)) == NULL ) )
	{
		if ( varSet )
			return( updateNeeded );
		else if ( ( tmpFontInfo = 
			    XLoadQueryFont(dpy, RMTable[rmIndex].defaultVal) )
			  == NULL )
		{
			fprintf(stderr, 
#ifdef OW_I18N
				gettext("OLWM fatal error:  can't load glyph font \"%s\"; exiting\n"),
#else
				"OLWM fatal error:  can't load glyph font \"%s\"; exiting\n",
#endif
				RMTable[rmIndex].defaultVal);
			exit(1);
		}
	}
	else if ( varSet )
	{
		MakeRMName( name, rmIndex );
		MakeRMClass( class, rmIndex );
		/* compare newValue with old value - REMIND if default was used
		 * last time, and newValue was now set to it vi xrdb, then we 
		 * may do an update unnecessarily (since value will not be in 
		 * OlwmDB, so XrmGetResource will return False and updateNeeded
		 * will be set to True, since we can't strcmp the value)
		 */
		if ( XrmGetResource( OlwmDB, name, class, &rmType, &rmValue ) 
		     && !strcmp( newValue, rmValue.addr ) )
			return( updateNeeded );
		else
			updateNeeded = True;
	}

#ifdef OW_I18N_FIX
	if (RM_FONTVAR != NULL)
		XFree(RM_FONTVAR);
#endif
	RM_FONTVAR = tmpFontInfo;
	return( updateNeeded );
}

/*
 * setCursors - set up ALL cursors from cursor font specified
 *
 *	NOTE that CursorColor and Bg1Color must be set before the cursors!
 *
 *	Notice that six cursors are set up (and stored in six separate
 *	GRV elements) from this single resource.
 *
 *	Also, since we don't save the actual cursor font name,
 *	checking to see if the font has been changed must be done by
 *	comparing the font names in the database.
 *
 */
static Bool
setCursors( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	Font			cursorFont = 0; 	/* init for compare */
	Bool			updateNeeded = False;
	char			name[MAX_NAME];
	char			class[MAX_CLASS];
	XrmRepresentation	rmType;
	XrmValue		rmValue;
	int			ii;
	XColor			foreColor, backColor;

	if ( ( newValue == NULL )
	     || ( (cursorFont = XLoadFont( dpy, newValue )) == 0 ) )
	{
		if ( varSet )
			return( updateNeeded );
		else	/* use built-in default if possible */
			cursorFont = XLoadFont( dpy, 
						RMTable[rmIndex].defaultVal );
	}
	else if ( varSet )
	{
		/* check to see if cursor font in resource actually changed */
		MakeRMName( name, rmIndex );
		MakeRMClass( class, rmIndex );
		/* compare newValue with old value - REMIND doesn't detect
		 * when built-in default values are used 
		 */
		if ( XrmGetResource( OlwmDB, name, class, &rmType, &rmValue ) 
		     && !strcmp( newValue, rmValue.addr ) )
			return( updateNeeded );
		else
			updateNeeded = True;
	}
	
	/* set up cursor rgb values in color structure */
	foreColor.pixel = GRV.CursorColor;
	XQueryColor( dpy, CMAP, &foreColor );
	/* REMIND - should backColor be background (GRV.Bg1Color)? */
	backColor.pixel = WhitePixel( dpy, DefaultScreen(dpy) );
	XQueryColor( dpy, CMAP, &backColor );

	/* loop through and set all six pointers */
	for ( ii = 0 ; ii < NUM_CURSORS ; ii++ )
	{
	    Cursor		*tmpVariable;
	    unsigned int	tmpFontIndex;
	    unsigned int	defaultIndex;

	    switch( ii )
	    {
	    /* REMIND - we might need to change these absolute indices to
	     * #defines of some sort
	     */
	    case BASICPTR:	tmpVariable = &GRV.BasicPointer;
				tmpFontIndex = OLC_basic;
				defaultIndex = XC_left_ptr;
				break;
	    case MOVEPTR:	tmpVariable = &GRV.MovePointer;
				tmpFontIndex = OLC_basic;
				defaultIndex = XC_left_ptr;
				break;
	    case BUSYPTR:	tmpVariable = &GRV.BusyPointer;
				tmpFontIndex = OLC_busy;
				defaultIndex = XC_watch;
				break;
	    case ICONPTR:	tmpVariable = &GRV.IconPointer;
				tmpFontIndex = OLC_basic;
				defaultIndex = XC_left_ptr;
				break;
	    case RESIZEPTR:	tmpVariable = &GRV.ResizePointer;
				tmpFontIndex = OLC_beye;
				defaultIndex = XC_tcross;
				break;
	    case MENUPTR:	tmpVariable = &GRV.MenuPointer;
				tmpFontIndex = OLC_basic;
				defaultIndex = XC_sb_right_arrow;
				break;
	    case QUESTIONPTR:	tmpVariable = &GRV.QuestionPointer;
				tmpFontIndex = OLC_basic;
				defaultIndex = XC_question_arrow;
				break;
	    case TARGETPTR:	tmpVariable = &GRV.TargetPointer;
				tmpFontIndex = OLC_basic;
				defaultIndex = XC_circle;
				break;
	    case PANPTR:	tmpVariable = &GRV.PanPointer;
				tmpFontIndex = OLC_panning;
				defaultIndex = XC_sb_v_double_arrow;
				break;
	    }

	    if ( ( cursorFont == 0 )
	         || ( (*tmpVariable = XCreateGlyphCursor( dpy, cursorFont,
				cursorFont, tmpFontIndex, tmpFontIndex+1, 
				&foreColor, &backColor )) == 0 ) )
	    {
	    	/* use default */
	    	*tmpVariable = XCreateFontCursor( dpy, defaultIndex );
#ifdef LATER
		/* seems to be a bug in XRecolorCursor() right now */
		XRecolorCursor( dpy, tmpVariable, &foreColor, &backColor );
#endif
	    }
	}

	return( updateNeeded );
}

/*
 * setCharResource - set character resource value from db 
 */
/*ARGSUSED*/	/* dpy is std arg for Set fns */
static Bool
setCharResource( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	char	*setValue;
	Bool	updateNeeded = False;
#define RM_CHARVAR     (*((char **)(RMTable[rmIndex].varAddr)))

	if ( newValue == NULL )
	{
		if ( varSet )
			return( updateNeeded );
		else	/* builtin default - we're assuming varSet is a
			 * reliable indicator that the value was not set
			 * previously (else it would need to be MemFree'd
			 */
			setValue = RMTable[rmIndex].defaultVal;
	}
	else 
	{
		if (varSet && RM_CHARVAR != NULL &&
		    strcmp(RM_CHARVAR, newValue) == 0) {
			return( updateNeeded );
		} else {
			updateNeeded = True;
			/* REMIND: if not previously set, what will happen?
			 * Does our global array initialization guarantee a
			 * NULL ptr there?
			 */
			MemFree( RM_CHARVAR );
			setValue = newValue;
		}
	}

	/* REMIND: this is slightly inefficient, since the defaultVal may 
	 * copied, but it makes the MemFree unconditional when updating 
	 * the value
	 */
	if ( setValue != NULL )
		RM_CHARVAR = MemNewString( setValue );
	else
		RM_CHARVAR = NULL;

	return( updateNeeded );
}

#ifdef OW_I18N
/*
 * setWCharResource - set wide character resource value from db 
 */
/*ARGSUSED*/	/* dpy is std arg for Set fns */
static Bool
setWCharResource( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	wchar_t	*setValue;
	wchar_t	*newWValue;
	Bool	updateNeeded = False;
#define RM_WCHARVAR     (*((wchar_t **)(RMTable[rmIndex].varAddr)))

	newWValue = NULL;
	if ( newValue == NULL )
	{
		if ( varSet )
			return( updateNeeded );
		else	/* builtin default - we're assuming varSet is a
			 * reliable indicator that the value was not set
			 * previously (else it would need to be MemFree'd
			 */
			setValue = ctstowcsdup(RMTable[rmIndex].defaultVal);
	}
	else 
	{
		newWValue = ctstowcsdup(newValue);
		if (varSet && RM_WCHARVAR != NULL &&
		    wscmp(RM_WCHARVAR, newWValue) == 0) {
			return( updateNeeded );
		} else {
			updateNeeded = True;
			/* REMIND: if not previously set, what will happen?
			 * Does our global array initialization guarantee a
			 * NULL ptr there?
			 */
			MemFree( RM_WCHARVAR );
			setValue = newWValue;
		}
	}

	/* REMIND: this is slightly inefficient, since the defaultVal may 
	 * copied, but it makes the MemFree unconditional when updating 
	 * the value
	 */
	if ( setValue != NULL )
		RM_WCHARVAR = setValue;
	else
		RM_WCHARVAR = NULL;

	return( updateNeeded );
}
#endif OW_I18N

/*
 * setFloatResource - retrieve float resource from OlwmDB
 */
/*ARGSUSED*/	/* dpy is std arg for Set fns */
static Bool
setFloatResource( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	Bool	updateNeeded = False;
	float	newFloat;
#define RM_FLOATVAR     *((float *)(RMTable[rmIndex].varAddr))

	if ( newValue == NULL )
	{
		if ( varSet )
			return( updateNeeded );
		else	/* builtin default */
			sscanf( RMTable[rmIndex].defaultVal, "%f", &newFloat);
	}
	else 
	{
		sscanf( newValue, "%f", &newFloat);
		if ( varSet )
		{
			if ( newFloat == RM_FLOATVAR )
				return( updateNeeded );
			else
				updateNeeded = True;
		}
	}

	RM_FLOATVAR = newFloat;
	return( updateNeeded );
}


/*
 * setIntResource - retrieve integer value from resource db
 */
/*ARGSUSED*/	/* dpy is std arg for Set fns */
static Bool
setIntResource( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	Bool	updateNeeded = False;
	int	newInt;
#define RM_INTVAR     *((int *)(RMTable[rmIndex].varAddr))

	if ( newValue == NULL )
	{
		if ( varSet )
			return( updateNeeded );
		else	/* builtin default */
			newInt = atoi( RMTable[rmIndex].defaultVal );
	}
	else 
	{
		newInt = atoi( newValue );
		if ( varSet )
		{
			if ( newInt == RM_INTVAR )
				return( updateNeeded );
			else
				updateNeeded = True;
		}
	}

	RM_INTVAR = newInt;
	return( updateNeeded );
}

/*
 * setClickTimeout - retrieve value for clickTimeout resource
 *	(Not simply a SetIntResource since must be converted to millisec.)
 */
/*ARGSUSED*/	/* dpy is std arg for Set fns */
static Bool
setClickTimeout( dpy, rmIndex, newValue, varSet)
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	Bool	updateNeeded = False;
	int	newInt;
#ifndef RM_INTVAR
#define RM_INTVAR     *((int *)(RMTable[rmIndex].varAddr))
#endif

	if ( newValue == NULL )
	{
		if ( varSet )
			return( updateNeeded );
		else	/* builtin default REMIND - need convert to milli? */
			newInt = atoi( RMTable[rmIndex].defaultVal );
	}
	else 
	{
		newInt = atoi( newValue ) * 100;    /* convert to millisec*/
		if ( varSet )
		{
			if ( newInt == RM_INTVAR )
				return( updateNeeded );
			else
				updateNeeded = True;
		}
	}

	/*
	 * It's nearly impossible for the hardware to actually generate two
	 * clicks in less than 100ms.  We make a special case for this and
	 * make the minimum timeout value be 150ms.
	 */
	if (newInt < 150)
	    newInt = 150;

	RM_INTVAR = newInt;
	return( updateNeeded );
}


/*
 * setFg1Color - retrieve color name resource from OlwmDB 
 *	and convert to pixel value
 */
static Bool
setFg1Color( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	XColor		exact;
	unsigned long	newColor;
	Bool		updateNeeded = False;
#define RM_COLORVAR     *((unsigned long *)(RMTable[rmIndex].varAddr))

#ifndef COLOR2D
	if ( !GRV.F3dUsed )
		newColor = BlackPixel( dpy, DefaultScreen(dpy) );
	else
	{
#endif /* COLOR2D */
	if ( (newValue == NULL)
	      || (!XParseColor( dpy, CMAP, newValue, &exact ))
	      || (!XAllocColor( dpy, CMAP, &exact )) )
	{
		if ( varSet )
			return( updateNeeded );
		else 
		{
			exact.pixel = BlackPixel( dpy, DefaultScreen(dpy) );
			XQueryColor( dpy, CMAP, &exact );
			exact.red = (exact.red*8)/10;
			exact.green = (exact.green*8)/10;
			exact.blue = (exact.blue*8)/10;
			if ( XAllocColor( dpy, CMAP, &exact ) )
				newColor = exact.pixel;
			else	/* REMIND should OlwmDB value change? */
				newColor = BlackPixel(dpy, DefaultScreen(dpy));
		}
	}
	else 
	{
		newColor = exact.pixel;
		if ( varSet )
		{
			if ( newColor == RM_COLORVAR )
				return( updateNeeded );
			else
				updateNeeded = True;
		}
	}

#ifndef COLOR2D
	}	/* end GRV.F3dUsed */
#endif /* COLOR2D */

	RM_COLORVAR = newColor;
	return( updateNeeded );
}


/*
 * setBg1Color - retrieve color name resource from OlwmDB 
 *	and convert to pixel value
 *
 *	NOTE that Bg1Color must be set before any other color can be set!!!
 */
static Bool
setBg1Color( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	XColor		exact;
	XColor		fgc, bg2c, bg3c, whitec;
	unsigned long	newColor;
	unsigned long	oldcells[4];
	int		oldcellcount = 0;
#ifndef RM_COLORVAR
#define RM_COLORVAR     *((unsigned long *)(RMTable[rmIndex].varAddr))
#endif

	if (windowColorsAllocated) {
	    /*
	     * REMIND: this code helps prevent us from freeing the same cell 
	     * twice.  We should be able to do this according to the protocol, 
	     * but some servers have bugs where allocating a cell twice and 
	     * then freeing it once will actually free the cell.
	     */
	    if (GRV.Bg0Color != WorkspaceColorPixel)
		oldcells[oldcellcount++] = GRV.Bg0Color;
	    if (GRV.Bg1Color != WorkspaceColorPixel)
		oldcells[oldcellcount++] = GRV.Bg1Color;
	    if (GRV.Bg2Color != WorkspaceColorPixel)
		oldcells[oldcellcount++] = GRV.Bg2Color;
	    if (GRV.Bg3Color != WorkspaceColorPixel)
		oldcells[oldcellcount++] = GRV.Bg3Color;
	    XFreeColors(dpy, CMAP, oldcells, oldcellcount, 0);
	    windowColorsAllocated = False;
	}

	if ((newValue != NULL) &&
	    (XParseColor(dpy, CMAP, newValue, &exact) == 1) &&
	    (XAllocColor(dpy, CMAP, &exact) == 1))
	{
		newColor = exact.pixel;
		windowColorsAllocated = True;
	} else {
		/*
		 * No valid value was found in the resource, so we use 
		 * 20% gray as the default.
		 */
		exact.pixel = WhitePixel( dpy, DefaultScreen(dpy) );
		XQueryColor( dpy, CMAP, &exact );
		exact.red = (exact.red*8)/10;
		exact.green = (exact.green*8)/10;
		exact.blue = (exact.blue*8)/10;
		if (XAllocColor(dpy, CMAP, &exact) == 1) {
			newColor = exact.pixel;
			windowColorsAllocated = True;
		} else {
			/* REMIND should OlwmDB value change? */
			GRV.F3dUsed = False;
			newColor = WhitePixel(dpy, DefaultScreen(dpy));
			windowColorsAllocated = False;
		}
	}

	RM_COLORVAR = newColor;

	/* these colors aren't currently settable directly using resources */
#ifndef COLOR2D
	if ( GRV.F3dUsed )
	{
#endif /* COLOR2D */
	    fgc.pixel = GRV.Fg1Color;
	    XQueryColor(dpy,CMAP,&fgc);
	    olgx_calculate_3Dcolors(&fgc,&exact,&bg2c,&bg3c,&whitec);
	    XAllocColor(dpy,CMAP,&bg2c);
	    GRV.Bg2Color = bg2c.pixel;
	    XAllocColor(dpy,CMAP,&bg3c);
	    GRV.Bg3Color = bg3c.pixel;
	    XAllocColor(dpy,CMAP,&whitec);
	    GRV.Bg0Color = whitec.pixel;
#ifndef COLOR2D
	}
	else
	{
		/* in 2d these are not used by olgx, but olwm may */
		GRV.Bg0Color = WhitePixel( dpy, DefaultScreen(dpy) );
		GRV.Bg1Color = WhitePixel( dpy, DefaultScreen(dpy) );
		GRV.Bg2Color = BlackPixel( dpy, DefaultScreen(dpy) );
		GRV.Bg3Color = BlackPixel( dpy, DefaultScreen(dpy) );
	}
#endif /* COLOR2D */

	return True;
}

/*
 * setColor - retrieve color name resource from OlwmDB 
 *	and convert to pixel value
 *
 *	REMIND - the default (BlackPixel) is built-in to this function.
 */
static Bool
setColor( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	XColor		exact;
	unsigned long	newColor;
	Bool		updateNeeded = False;
#ifndef RM_COLORVAR
#define RM_COLORVAR     *((unsigned long *)(RMTable[rmIndex].varAddr))
#endif

	if ( (newValue == NULL)
	      || (!XParseColor( dpy, CMAP, newValue, &exact ))
	      || (!XAllocColor( dpy, CMAP, &exact )) )
	{
		if ( varSet )
			return( updateNeeded );
		else 
			newColor = BlackPixel(dpy, DefaultScreen(dpy));
	}
	else 
	{
		newColor = exact.pixel;
		if ( varSet )
		{
			if ( newColor == RM_COLORVAR )
				return( updateNeeded );
			else
				updateNeeded = True;
		}
	}

	RM_COLORVAR = newColor;
	return( updateNeeded );
}


/*
 * setInputFocusStyle - retrieve value of inputFocusStyle resource and
 *	return True is it's "followmouse" and False if it's "select" or
 *	"clicktotype"
 */
/*ARGSUSED*/	/* dpy is std arg for Set fns */
static Bool
setInputFocusStyle( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	Bool	focusFollows;
	Bool	updateNeeded = False;
#ifndef RM_BOOLVAR
#define RM_BOOLVAR     *((Bool *)(RMTable[rmIndex].varAddr))
#endif

	if ( (newValue == NULL) || !matchFocusType( newValue, &focusFollows ) )
	{
		if ( varSet )
			return( updateNeeded );
		else	/* builtin default */
			matchFocusType( RMTable[rmIndex].defaultVal, 
					&focusFollows );
	}
	else if ( varSet )
	{
		if ( focusFollows == RM_BOOLVAR )
			return( updateNeeded );
		else
			updateNeeded = True;
	}

	RM_BOOLVAR = focusFollows;
	return( updateNeeded );
}

/*
 * setBeepResource - retrieve value of Beep resource
 */
static Bool
setBeepResource( dpy, rmIndex, newValue, varSet )
Display *dpy;
int rmIndex;
char *newValue;
Bool varSet;
{
	BeepStatus newBeep;
	Bool updateNeeded = False;

#ifndef RM_BEEPVAR
#define RM_BEEPVAR *((BeepStatus *)(RMTable[rmIndex].varAddr))
#endif

	if ((newValue == NULL) || !matchBeepKeyword(newValue, &newBeep))
	{
	    if (varSet)
		return(updateNeeded);
	    else
		matchBeepKeyword(RMTable[rmIndex].defaultVal,&newBeep);
	}
	else if (varSet)
	{
	    if (newBeep == RM_BEEPVAR)
		return(updateNeeded);
	    else
		updateNeeded = True;
	}

	RM_BEEPVAR = newBeep;
	return(updateNeeded);
}


/*
 * setIconLocation - return value of iconLocation resource
 */
/*ARGSUSED*/	/* dpy is std arg for Set fns */
static Bool
setIconLocation( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	Bool		updateNeeded = False;
	IconPreference	newIconPlace;
#define RM_ICONVAR     *((IconPreference *)(RMTable[rmIndex].varAddr))

	if ( (newValue == NULL) || !matchIconPlace( newValue, &newIconPlace ) )
	{
		if ( varSet )
			return( updateNeeded );
		else	/* builtin default */
			matchIconPlace( RMTable[rmIndex].defaultVal, 
					&newIconPlace );
	}
	else if ( varSet )	/* a valid newValue and an old value */
	{
		if ( newIconPlace == RM_ICONVAR )
			return( updateNeeded );
		else
			updateNeeded = True;
	}

	RM_ICONVAR = newIconPlace;
	return( updateNeeded );
}


static Bool
parseKeySpec(dpy, str, modmask, keycode)
    Display *dpy;
    char *str;
    int *modmask;
    KeyCode *keycode;
{
    char line[100];
    char *word;
    extern unsigned int FindModiferMask();
    int kc, m;
    int mask = 0;
    int code = 0;
    KeySym ks;

    strcpy(line, str);
    word = strtok(line, " \t");
    if (word == NULL)
	return False;

    while (word != NULL) {
	ks = XStringToKeysym(word);
	if (ks == NoSymbol) {
	    if (strcmp(word, "Any") == 0) {
		mask = AnyModifier;
		word = strtok(NULL, " \t");
		continue;
	    } else if (strcmp(word, "Shift") == 0)
		ks = XK_Shift_L;
	    else if (strcmp(word, "Control") == 0)
		ks = XK_Control_L;
	    else if (strcmp(word, "Meta") == 0)
		ks = XK_Meta_L;
	    else if (strcmp(word, "Alt") == 0)
		ks = XK_Alt_L;
	    else if (strcmp(word, "Super") == 0)
		ks = XK_Super_L;
	    else if (strcmp(word, "Hyper") == 0)
		ks = XK_Hyper_L;
	    else
		return False;
	}
	    
	kc = XKeysymToKeycode(dpy, ks);
	if (kc == 0)
	    return False;

	m = FindModifierMask(kc);
	if (m == 0) {
	    code = kc;
	    break;
	}
	mask |= m;
	word = strtok(NULL, " \t");
    }

    if (code == 0)
	return False;

    *keycode = code;
    *modmask = mask;
    return True;
}


/*
 * setKey - retrieve value for named KeySym and convert it to a KeyCode
 *	Then grab the key so we'll deal with it as a special command.
 */
static Bool
setKey( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	unsigned int	modMask;
	KeyCode		newKeyCode = 0;
	Bool		updateNeeded = False;
	XKeyboardControl xkbc;
#define RM_KEYVAR     ((KeySpec *)(RMTable[rmIndex].varAddr))

	if ( newValue == NULL )
	{
		if ( varSet )
			return( updateNeeded );
		else	/* builtin default */
		{
		    (void) parseKeySpec(dpy, RMTable[rmIndex].defaultVal,
					&modMask, &newKeyCode);
		}
	}
	else 
	{
		(void) parseKeySpec(dpy, newValue, &modMask, &newKeyCode);
		if ( varSet )
		{
			if ((newKeyCode == RM_KEYVAR->keycode) &&
			    (modMask == RM_KEYVAR->modmask))
				return( updateNeeded );
			else
				updateNeeded = True;
		}
	}

	/* REMIND perhaps we should have separate functions to deal with
	 * keys that want grab and don't want grab
	 * Also, perhaps this activity should be in the updFunc
	 */
	if (rmIndex != RM_CONFIRMKEY && rmIndex != RM_CANCELKEY &&
	    newKeyCode != 0) {
		XGrabKey( dpy, newKeyCode, modMask, 
			  DefaultRootWindow(dpy), False, 
			  GrabModeAsync, GrabModeAsync );
#ifdef notdef
		xkbc.auto_repeat_mode = AutoRepeatModeOff;
		xkbc.key = newKeyCode;
		XChangeKeyboardControl(dpy, KBKey|KBAutoRepeatMode, &xkbc);
#endif /* notdef */
	}

	RM_KEYVAR->keycode = newKeyCode;
	RM_KEYVAR->modmask = modMask;
	return( updateNeeded );
}


/*
 * setDimension
 * 
 * Look at the default screen and determine whether to use 3D.
 */
static Bool
setDimension( dpy, rmIndex, newValue, varSet )
    Display	*dpy;
    int		rmIndex;
    char	*newValue;
    Bool	varSet;
{
	Bool	newDimension;
	Bool	updateNeeded = False;
	int	depth = DisplayPlanes(dpy,DefaultScreen(dpy));
	int	visclass = (DefaultVisual(dpy,DefaultScreen(dpy)))->class;

	if ((newValue == NULL) || !matchBool(newValue, &newDimension)) {
	    if (varSet)
		return(updateNeeded);
	    else
		matchBool(RMTable[rmIndex].defaultVal, &newDimension);
	} else if (varSet) {
	    if (newDimension == RM_BOOLVAR)
		return(updateNeeded);
	    else
		updateNeeded = True;
	}

	switch (visclass) {
	case StaticGray:
	case GrayScale:
	    if (depth < 2)
		newDimension = False;
	    break;
	case DirectColor:
	case PseudoColor:
	    if (depth < 4)
		newDimension = False;
	    break;
	case StaticColor:
	    if (depth < 8)
		newDimension = False;
	    break;
	case TrueColor:
	    if (depth < 6)
		newDimension = False;
	    break;
	}

	RM_BOOLVAR = newDimension;
	return(updateNeeded);
}


/*
 * SetNameTag - retrieve value of name resource.  If not set, use
 *	AppName.   Not likely to be valuable as a dynamically
 *	modified resource.
 *
 *	REMIND: currently not called in any resource parsing loop
 */
/*ARGSUSED*/	/* dpy, newValue, varSet are std args for Set fns */
Bool
SetNameTag( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	Bool			updateNeeded = False;
	char			name[MAX_NAME];
	char			class[MAX_CLASS];
	XrmRepresentation	repType;
	XrmValue		value;
#define RM_NAMEVAR     *((char **)(RMTable[rmIndex].varAddr))

	(void) strcpy( name, AppName );
	(void) strcat( name, RMTable[rmIndex].instance );
	MakeRMClass( class, rmIndex );

	/* REMIND NameTag will need to be MemFree'd, if 
	 * updating the NameTag is allowed and != AppName
	 */
	if ( XrmGetResource( OlwmDB, name, class, &repType, &value )
		== True && (value.addr != NULL) )
	{
		RM_NAMEVAR = MemNewString( value.addr );
		updateNeeded = True;
	}
	else
		RM_NAMEVAR = AppName;
	
	return( updateNeeded );
}

/* buildMinimalList -- build the list of classes/instances to be 
 * but minimally decorated; return True if the list was successfully parsed.
 */
static Bool 
buildMinimalList(str, pplist)
char *str;
List **pplist;
{
    char *swork, *swork2;
    List *l = NULL_LIST;

    if (str == NULL)
	return False;

    swork2 = swork = MemNewString(str);

    while ((swork2 = strtok(swork2, " \t")) != NULL)
    {
	l = ListCons(MemNewString(swork2),l);
	swork2 = NULL;
    }
    MemFree(swork);
    *pplist = l;
    return True;
}

static void *
freeMinimalStrings(str,junk)
char *str;
void *junk;
{
	MemFree(str);
	return NULL;
}

/*
 * setMinimalDecor - set list of windows to be minimally decorated
 *
 */
/*ARGSUSED*/	/* dpy is std arg for Set fns */
static Bool
setMinimalDecor( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	List	*newl = NULL_LIST;
#define RM_LISTVAR     *((List **)(RMTable[rmIndex].varAddr))

	if (newValue == NULL)
	{
	    if (!buildMinimalList( RMTable[rmIndex].defaultVal, &newl ))
	        return False;
	}
	else if (!buildMinimalList( newValue, &newl ) )
	    return False;

	if (RM_LISTVAR != NULL)
	{
	    ListApply(RM_LISTVAR,freeMinimalStrings,NULL);
	    ListDestroy(RM_LISTVAR);
	}
	RM_LISTVAR = newl;

	return( True );
}


#ifdef OW_I18N
/*
 * setCharResource - set character resource value from db 
 */
/*ARGSUSED*/	/* dpy is std arg for Set fns */
static Bool
setOLLC( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	return _setOLLC(rmIndex, newValue, varSet, OLLC_SRC_RESOURCE);
}


/*ARGSUSED*/	/* dpy is std arg for Set fns */
static Bool
setOLLCCL( dpy, rmIndex, newValue, varSet )
Display	*dpy;
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	return _setOLLC(rmIndex, newValue, varSet, OLLC_SRC_COMMAND_LINE);

}


/*ARGSUSED*/	/* dpy is std arg for Set fns */
static Bool
_setOLLC( rmIndex, newValue, varSet, priority )
int	rmIndex;
char	*newValue;
Bool	varSet;
{
	char	*setValue;
	Bool	updateNeeded = False;
#define RM_OLLCVAR     ((OLLCItem *)(RMTable[rmIndex].varAddr))


	if ( RM_OLLCVAR->priority > priority )
		return updateNeeded;

	if ( newValue == NULL )
	{
		if ( varSet )
			return( updateNeeded );
		else
		{
			/*
			 * We can not rely on the varSet to determine
			 * previously set or not, becuase we have an
			 * another infomation source for the locale
			 * setting, if no value in the resourceDB,
			 * then we should fall back to exising value,
			 * if any.
			 */
			if ( RM_OLLCVAR->locale != NULL )
			{
				setValue = RM_OLLCVAR->locale;
			}
			else
			{
				/*
				 * builtin default - we're assuming
				 * varSet is a reliable indicator that
				 * the value was not set pareviously
				 * (else it would need to be MemFree'd
				 */
				setValue = RMTable[rmIndex].defaultVal;
			}
		}
	}
	else 
	{
		if ( varSet && RM_OLLCVAR->locale != NULL &&
		    strcmp(RM_OLLCVAR->locale, newValue) == 0 ) {
			return( updateNeeded );
		} else {
			updateNeeded = True;
			/* REMIND: if not previously set, what will happen?
			 * Does our global array initialization guarantee a
			 * NULL ptr there?
			 */
			MemFree( RM_OLLCVAR->locale );
			setValue = newValue;
		}
	}

	/* REMIND: this is slightly inefficient, since the defaultVal may 
	 * copied, but it makes the MemFree unconditional when updating 
	 * the value
	 */
	if ( setValue != NULL )
	{
		if ( setValue != RM_OLLCVAR->locale )
		{
			RM_OLLCVAR->locale = MemNewString( setValue );
			RM_OLLCVAR->priority = priority;
		}
	}
	else
		RM_OLLCVAR->locale = NULL;

	if ( updateNeeded == True )
		OLLCUpdated = True;

	return( updateNeeded );
}
#endif OW_I18N


/*
 * 	Global routines
 */

/*
 *	The Resource Update Functions
 */

/*
 * UpdWorkspace - Update workspace color 
 *
 */
/*ARGSUSED*/	/* rmIndex not needed here */
Bool
UpdWorkspace( dpy, rmIndex, isUpdate )
Display	*dpy;
int	rmIndex;
Bool	isUpdate;
{
	XGCValues xgcv;

	if (isUpdate && WorkspaceColorUsed) {
	    /*
	     * REMIND: this code helps prevent us from freeing the same cell
	     * twice.  We should be able to do this according to the protocol,
	     * but some servers have bugs where allocating a cell twice and
	     * then freeing it once will actually free the cell.
	     */
	    if (WorkspaceColorPixel != GRV.Bg0Color &&
		WorkspaceColorPixel != GRV.Bg1Color &&
		WorkspaceColorPixel != GRV.Bg2Color &&
		WorkspaceColorPixel != GRV.Bg3Color)
	    {
		XFreeColors(dpy, CMAP, &WorkspaceColorPixel, 1, 0);
	    }
	    WorkspaceColorUsed = False;
	}

	setWorkspaceColor( dpy, GRV.WorkspaceColor, isUpdate );

	if (isUpdate)
	{
	    xgcv.foreground = WorkspaceColorPixel;
	    xgcv.background = WorkspaceColorPixel;
	    if (WorkspaceColorUsed)
	    {
	        XChangeGC(dpy, IconNormalGC, GCBackground, &xgcv);
	        XChangeGC(dpy, IconSelectedGC, GCBackground, &xgcv);
                XChangeGC(dpy, IconUnselectedGC, GCForeground, &xgcv);
	    }
            WinRedrawAllWindows();
	}
	return( True );
}

/*
 * setWorkspaceColor -- set the root window background.
 *	If isUpdate is true, then the background has already been set once
 *	(on startup), so XSetWindowBackground not done if default case.
 *
 * REMIND: this routine doesn't work properly for GrayScale visuals.
 *
 */
static void
setWorkspaceColor( dpy, colorname, isUpdate )
Display	*dpy;
char	*colorname;
Bool	isUpdate;
{
	XColor		color;
	GC		gc;
	XGCValues	gcv;
	Pixmap		bitmap = 0;
	Pixmap		pixmap;
	unsigned int	width, height;
	Bool		valid = False;
	Bool		docolor = False;

	if ( colorname != NULL )
	{
		/* Null string disables root setting */
		if (*colorname == '\0')
		    return;

		if ( *colorname == '/' )
		{
			unsigned int x, y;

			if ( BitmapSuccess == XReadBitmapFile( dpy,
							DefaultRootWindow(dpy),
							colorname,
							&width, &height,
							&bitmap, &x, &y ) )
			valid = True;
		}
		else if ( (*colorname == '#') && ColorDisplay )
		{
			XParseColor( dpy,
				     DefaultColormap(dpy,DefaultScreen(dpy)),
				     colorname, &color );

			if ( XAllocColor( dpy,
					DefaultColormap(dpy,DefaultScreen(dpy)),
					&color ) )
				valid = docolor = True;
		}
		else if ( ColorDisplay )
		{
			/* It is a color name */
			/*
			 * NOTE:  this could possibly fail on a full
			 * dynamic colormap. If it does, tough -- 
			 * we''ll just set it to whatever junk
			 * comes back for now. 
			 * Presumably this won''t happen since
			 * 'props' was able to display the color.
			 */
			if ( XAllocNamedColor(dpy,
					DefaultColormap(dpy,DefaultScreen(dpy)),
					colorname, &color, &color) )
			valid = docolor = True;
		}
	}

	/* the new color isn't usable, and we've set the background before */
	if ( !valid && isUpdate )
		return;

	if ( !valid && ColorDisplay ) 
	{
		XParseColor( dpy, DefaultColormap(dpy,DefaultScreen(dpy)),
			     DEFAULTCOLOR, &color );
		if ( XAllocColor( dpy, DefaultColormap(dpy,DefaultScreen(dpy)),
				  &color ) )
		valid = docolor = True;
	}

	if ( !valid )
	{
		bitmap = Gray50;
		width = Gray50width;
		height = Gray50height;
	}

	if ( docolor ) 
	{
		WorkspaceColorUsed = True;
		WorkspaceColorPixel = color.pixel;
		XSetWindowBackground(dpy, DefaultRootWindow(dpy), color.pixel);
	}
	else
	{
		WorkspaceColorUsed = False;
		gcv.foreground = BlackPixel(dpy,DefaultScreen(dpy));
		gcv.background = WhitePixel(dpy,DefaultScreen(dpy));
		gc = XCreateGC( dpy, DefaultRootWindow(dpy),
				GCForeground|GCBackground, &gcv );
		pixmap = XCreatePixmap(dpy, DefaultRootWindow(dpy),
				       width, height,
					(unsigned int)DefaultDepth(dpy,
							DefaultScreen(dpy)));
		XCopyPlane( dpy, bitmap, pixmap, gc, 0, 0, width, height,
			    0, 0, (unsigned long)1 );
		XSetWindowBackgroundPixmap(dpy, DefaultRootWindow(dpy), pixmap);
		XFreeGC( dpy, gc );
		XFreePixmap( dpy, bitmap );
		XFreePixmap( dpy, pixmap );
	}
	XClearWindow( dpy, DefaultRootWindow(dpy) );
}


/*
 * UpdSync - Update whether synchronization is being done, or not.
 *
 */
/*ARGSUSED*/	/* rmIndex not needed here */
Bool
UpdSync( dpy, rmIndex, isUpdate )
Display	*dpy;
int	rmIndex;
Bool	isUpdate;
{
	(void) XSynchronize( dpy, GRV.Synchronize );
	return( True );
}


/*
 * UpdTitleFont - Update title font being used
 *
 */
/*ARGSUSED*/	/* std arg rmIndex not used here */
Bool
UpdTitleFont( dpy, rmIndex, isUpdate )
Display	*dpy;
int	rmIndex;
Bool	isUpdate;
{
#ifndef OW_I18N
	XGCValues	values;

	values.font = GRV.TitleFontInfo->fid;
	XChangeGC( dpy, DrawNormalGC, GCFont, &values );
	XChangeGC( dpy, DrawReverseGC, GCFont, &values );
#ifdef REMIND_STILL_NEEDED?
	XChangeGC( dpy, DrawLinesGC, GCFont, &values );
	XChangeGC( dpy, DrawRevLinesGC, GCFont, &values );
#endif	/* REMIND_STILL_NEEDED? */
#endif !OW_I18N

#ifdef OW_I18N
	olgx_set_text_font(olgx_gisnormal, GRV.TitleFontSetInfo.fs,
			   OLGX_NORMAL);
	olgx_set_text_font(olgx_gisreverse, GRV.TitleFontSetInfo.fs,
			   OLGX_NORMAL);
	olgx_set_text_font(olgx_gisrevpin, GRV.TitleFontSetInfo.fs,
			   OLGX_NORMAL);
#else OW_I18N
	olgx_set_text_font(olgx_gisnormal, GRV.TitleFontInfo,
			   OLGX_NORMAL);
	olgx_set_text_font(olgx_gisreverse, GRV.TitleFontInfo,
			   OLGX_NORMAL);
	olgx_set_text_font(olgx_gisrevpin, GRV.TitleFontInfo,
			   OLGX_NORMAL);
#endif OW_I18N
	return( True );
}

/*
 * UpdTextFont - Update text font being used (e.g., notice box text)
 *
 */
/*ARGSUSED*/	/* std args dpy, rmIndex not used here */
Bool
UpdTextFont( dpy, rmIndex, isUpdate )
Display	*dpy;
int	rmIndex;
Bool	isUpdate;
{
#ifdef OW_I18N
	olgx_set_text_font(olgx_gistext, GRV.TextFontSetInfo.fs,
#else
	olgx_set_text_font(olgx_gistext, GRV.TextFontInfo,
#endif
			   OLGX_NORMAL);
	return( True );
}

/*
 * UpdButtonFont - Update button font being used
 *
 */
/*ARGSUSED*/	/* std args dpy, rmIndex not used here */
Bool
UpdButtonFont( dpy, rmIndex, isUpdate )
Display	*dpy;
int	rmIndex;
Bool	isUpdate;
{
#ifdef OW_I18N
	olgx_set_text_font(olgx_gisbutton, GRV.ButtonFontSetInfo.fs,
#else
	olgx_set_text_font(olgx_gisbutton, GRV.ButtonFontInfo,
#endif
			   OLGX_NORMAL);

	return( True );
}


/*
 * UpdIconFont - Update icon font being used
 *
 */
/*ARGSUSED*/	/* std arg rmIndex not used here */
Bool
UpdIconFont( dpy, rmIndex, isUpdate )
Display	*dpy;
int	rmIndex;
Bool	isUpdate;
{
#ifndef OW_I18N
	XGCValues	values;

	values.font = GRV.IconFontInfo->fid;
	XChangeGC( dpy, IconNormalGC, GCFont, &values );
	XChangeGC( dpy, IconSelectedGC, GCFont, &values );
#endif !OW_I18N

	return( True );
}


/*
 * UpdGlyphFont - Update glyph font being used
 *
 *	REMIND - this code should be used by InitGraphics.c
 *	instead of being duplicated.
 */
/*ARGSUSED*/	/* std args dpy, rmIndex not used here */
Bool
UpdGlyphFont( dpy, rmIndex, isUpdate )
Display	*dpy;
int	rmIndex;
Bool	isUpdate;
{
	olgx_set_glyph_font(olgx_gisnormal, GRV.GlyphFontInfo,
			    OLGX_NORMAL);
	olgx_set_glyph_font(olgx_gisreverse, GRV.GlyphFontInfo,
			    OLGX_NORMAL);
	olgx_set_glyph_font(olgx_gisrevpin, GRV.GlyphFontInfo,
			    OLGX_NORMAL);
	olgx_set_glyph_font(olgx_gisbutton, GRV.GlyphFontInfo,
			    OLGX_NORMAL);

	/*
	 * REMIND Even though it might not make sense, what about olgx_gistext?
	 */

	/* REMIND HACK - if the following routine is modified to
	 * use olgx macros, may only need to be from InitGraphics(), 
	 * or perhaps entire call should be moved here!
	 */
	setResizeSizes();

	return( True );
}


/*
 * UpdFg1Color - Update Fg1Color used 
 *
 */
/*ARGSUSED*/	/* std arg rmIndex not used here */
Bool
UpdFg1Color( dpy, rmIndex, isUpdate )
Display	*dpy;
int	rmIndex;
Bool	isUpdate;
{
	XGCValues xgcv;

	if (isUpdate)
	{
	    olgx_set_single_color(olgx_gisrevpin,OLGX_WHITE,GRV.Fg1Color,OLGX_SPECIAL);
	    olgx_set_single_color(olgx_gisreverse,OLGX_BG1,GRV.Fg1Color,OLGX_SPECIAL);
	    olgx_set_single_color(olgx_gisreverse,OLGX_BG3,GRV.Fg1Color,OLGX_SPECIAL);
	    olgx_set_single_color(olgx_gisnormal,OLGX_BLACK,GRV.Fg1Color,OLGX_SPECIAL);
	    olgx_set_single_color(olgx_gisbutton,OLGX_BLACK,GRV.Fg1Color,OLGX_SPECIAL);
	    olgx_set_single_color(olgx_gistext,OLGX_BLACK,GRV.Fg1Color,OLGX_SPECIAL);
	    olgx_set_single_color(olgx_gisreverse,OLGX_BLACK,GRV.Fg1Color,OLGX_SPECIAL);

	    xgcv.foreground = GRV.Fg1Color;
	    xgcv.background = GRV.Fg1Color;
	    XChangeGC(dpy, DrawNormalGC, GCBackground, &xgcv);
	    XChangeGC(dpy, DrawReverseGC, GCForeground, &xgcv);
	    XChangeGC(dpy, DrawBusyGC, GCForeground, &xgcv);
	    XChangeGC(dpy, IconNormalGC, GCBackground, &xgcv);
	    XChangeGC(dpy, IconSelectedGC, GCForeground, &xgcv);
#ifdef COLOR2D
	    XChangeGC(dpy, IconBorderGC, GCBackground, &xgcv);
#endif
            WinRedrawAllWindows();
	}
}


/*
 * UpdBg1Color - Update Bg1Color used 
 *
 */
/*ARGSUSED*/	/* std arg rmIndex not used here */
Bool
UpdBg1Color( dpy, rmIndex, isUpdate )
Display	*dpy;
int	rmIndex;
Bool	isUpdate;
{
	XGCValues xgcv;

	if (isUpdate)
	{
	    olgx_set_single_color(olgx_gisnormal,OLGX_BG1,GRV.Bg1Color,OLGX_SPECIAL);
	    olgx_set_single_color(olgx_gisrevpin,OLGX_BG1,GRV.Bg1Color,OLGX_SPECIAL);
	    olgx_set_single_color(olgx_gisbutton,OLGX_BG1,GRV.Bg1Color,OLGX_SPECIAL);
	    olgx_set_single_color(olgx_gistext,OLGX_BG1,GRV.Bg1Color,OLGX_SPECIAL);
	    olgx_set_single_color(olgx_gisreverse,OLGX_BLACK,GRV.Bg1Color,OLGX_SPECIAL);

	    xgcv.foreground = GRV.Bg1Color;
	    xgcv.background = GRV.Bg1Color;
	    XChangeGC(dpy, DrawBackgroundGC, GCForeground|GCBackground, &xgcv);
	    XChangeGC(dpy, DrawNormalGC, GCBackground, &xgcv);
	    XChangeGC(dpy, DrawReverseGC, GCForeground, &xgcv);
	    if (!WorkspaceColorUsed)
	    {
	        XChangeGC(dpy, IconNormalGC, GCBackground, &xgcv);
	        XChangeGC(dpy, IconSelectedGC, GCBackground, &xgcv);
	        XChangeGC(dpy, IconUnselectedGC, GCForeground, &xgcv);
	    }
	    XChangeGC(dpy, IconBorderGC, GCBackground, &xgcv);
	}

	UpdBg2Color(dpy,rmIndex,isUpdate);
	UpdBg3Color(dpy,rmIndex,isUpdate);
	UpdBg0Color(dpy,rmIndex,isUpdate);
	if (isUpdate)
	{
            WinRedrawAllWindows();
        }
}


/*
 * UpdBg2Color - Update Bg2Color used 
 *
 */
/*ARGSUSED*/	/* std arg rmIndex not used here */
Bool
UpdBg2Color( dpy, rmIndex, isUpdate )
Display	*dpy;
int	rmIndex;
Bool	isUpdate;
{
	XGCValues xgcv;

	if (isUpdate)
	{
	    olgx_set_single_color(olgx_gisreverse,OLGX_WHITE,GRV.Bg2Color,OLGX_SPECIAL);
	    olgx_set_single_color(olgx_gisnormal,OLGX_BG2,GRV.Bg2Color,OLGX_SPECIAL);
	    olgx_set_single_color(olgx_gisrevpin,OLGX_BG2,GRV.Bg2Color,OLGX_SPECIAL);
	    olgx_set_single_color(olgx_gisbutton,OLGX_BG2,GRV.Bg2Color,OLGX_SPECIAL);
	    olgx_set_single_color(olgx_gistext,OLGX_BG2,GRV.Bg2Color,OLGX_SPECIAL);

	    xgcv.foreground = GRV.Bg2Color;
	    xgcv.background = GRV.Bg2Color;
#ifdef COLOR2D
	    XChangeGC(dpy, DrawSelectedGC, GCForeground, &xgcv);
#else
	    if (GRV.F3dUsed)
	    {
	        XChangeGC(dpy, DrawSelectedGC, GCForeground, &xgcv);
	    }
#endif
	}
}


/*
 * UpdBg3Color - Update Bg3Color used 
 *
 */
/*ARGSUSED*/	/* std arg rmIndex not used here */
Bool
UpdBg3Color( dpy, rmIndex, isUpdate )
Display	*dpy;
int	rmIndex;
Bool	isUpdate;
{
    if (isUpdate)
    {
	olgx_set_single_color(olgx_gisnormal,OLGX_BG3,GRV.Bg3Color,OLGX_SPECIAL);
	olgx_set_single_color(olgx_gisrevpin,OLGX_BG3,GRV.Bg3Color,OLGX_SPECIAL);
	olgx_set_single_color(olgx_gisbutton,OLGX_BG3,GRV.Bg3Color,OLGX_SPECIAL);
	olgx_set_single_color(olgx_gistext,OLGX_BG3,GRV.Bg3Color,OLGX_SPECIAL);
    }
}


/*
 * UpdBg0Color - Update Bg0Color used for 3d color shading
 *
 */
/*ARGSUSED*/	/* std arg rmIndex not used here */
Bool
UpdBg0Color( dpy, rmIndex, isUpdate )
Display	*dpy;
int	rmIndex;
Bool	isUpdate;
{
    if (isUpdate)
    {
	olgx_set_single_color(olgx_gisrevpin,OLGX_BLACK,GRV.Bg0Color,OLGX_SPECIAL);
	olgx_set_single_color(olgx_gisreverse,OLGX_BG2,GRV.Bg0Color,OLGX_SPECIAL);
	olgx_set_single_color(olgx_gistext,OLGX_WHITE,GRV.Bg0Color,OLGX_SPECIAL);
	olgx_set_single_color(olgx_gisbutton,OLGX_WHITE,GRV.Bg0Color,OLGX_SPECIAL);
	olgx_set_single_color(olgx_gisnormal,OLGX_WHITE,GRV.Bg0Color,OLGX_SPECIAL);
    }
}

/*
 * updBorderColor - handle changes in the border colour 
 */
static Bool
updBorderColor( dpy, rmIndex, isUpdate)
Display	*dpy;
int	rmIndex;
Bool	isUpdate;
{
#ifndef COLOR2D
    XGCValues xgcv;

    if (isUpdate)
    {
	xgcv.foreground = GRV.BorderColor;
	xgcv.background = GRV.BorderColor;
	XChangeGC(dpy, IconBorderGC, GCForeground, &xgcv);
	if (!GRV.F3dUsed)
	    XChangeGC(dpy, DrawSelectedGC, GCForeground, &xgcv);
        WinRedrawAllWindows();
    }
#endif
}


/*
 * UpdCursorColor - Update CursorColor used 
 *
 *	REMIND - this doesn't work; a new XDefineCursor is probably needed.
 */
/*ARGSUSED*/	/* std arg rmIndex not used here */
Bool
UpdCursorColor( dpy, rmIndex, isUpdate )
Display	*dpy;
int	rmIndex;
Bool	isUpdate;
{
	int			ii;
	XColor			foreColor, backColor;
	
	/* set up new cursor rgb values in color structure */
	foreColor.pixel = GRV.CursorColor;
	XQueryColor( dpy, CMAP, &foreColor );
	/* REMIND - should backColor be background (GRV.Bg1Color)? 
	 * Is backColor even used when making these cursors??
	 */
	backColor.pixel = WhitePixel( dpy, DefaultScreen(dpy) );
	XQueryColor( dpy, CMAP, &backColor );

	/* loop through and update all six pointers */
	for ( ii = 0 ; ii < NUM_CURSORS ; ii++ )
	{
	    Cursor		*tmpVariable;

	    switch( ii )
	    {
	    case BASICPTR:	tmpVariable = &GRV.BasicPointer;
				break;
	    case MOVEPTR:	tmpVariable = &GRV.MovePointer;
				break;
	    case BUSYPTR:	tmpVariable = &GRV.BusyPointer;
				break;
	    case ICONPTR:	tmpVariable = &GRV.IconPointer;
				break;
	    case RESIZEPTR:	tmpVariable = &GRV.ResizePointer;
				break;
	    case MENUPTR:	tmpVariable = &GRV.MenuPointer;
				break;
	    case QUESTIONPTR:	tmpVariable = &GRV.QuestionPointer;
				break;
	    case TARGETPTR:	tmpVariable = &GRV.TargetPointer;
				break;
	    case PANPTR:	tmpVariable = &GRV.PanPointer;
				break;
	    }

#ifdef LATER
	    XRecolorCursor( dpy, *tmpVariable, &foreColor, &backColor );
#endif
	}

}


/* 
 * UpdIconLocation -- the icon placement policy has changed; rearrange the 
 * icons accordingly.
 */
static Bool
updIconLocation( dpy, rmIndex )
Display	*dpy;
int	rmIndex;
{
	SlotSetLocations(dpy);
}


/*
 * unconfigureFocus
 *
 * Tell a client to remove any grabs it may have set up according to the focus 
 * mode.  If this client is the focus, tell it to draw in its unfocused state.
 */
static void *
unconfigureFocus(cli)
    Client *cli;
{
    FrameSetupGrabs(cli, cli->framewin->core.self, False);
    if (cli->isFocus) {
	cli->isFocus = False;
	WinCallDraw(cli->framewin);
	cli->isFocus = True;
    }
    return NULL;
}


/*
 * reconfigureFocus
 *
 * Tell a client to restore any grabs it may need for the new focus mode.  If 
 * this client is the focus, tell it to draw using the proper highlighting for 
 * the new focus mode.
 */
static void *
reconfigureFocus(cli)
    Client *cli;
{
    FrameSetupGrabs(cli, cli->framewin->core.self, True);
    if (cli->isFocus) {
	WinCallDraw(cli->framewin);
    }
    return NULL;
}


/*
 * UpdInputFocusStyle -- change the focus style on the fly
 *
 * This is a trifle odd.  GRV.FocusFollowsMouse has already been updated with 
 * its new value.  Momentarily reset it back to its old value, and then call 
 * unconfigureFocus on every client.  This will clear grabs and highlighting 
 * and such as if the old focus mode were still in effect.  Reset GRV.
 * GRV.FocusFollowsMouse to the new value, and then call reconfigureFocus on 
 * every client to set up stuff for the new focus mode.
 */
Bool
UpdInputFocusStyle(dpy, rmIndex, isUpdate)
    Display *dpy;
    int rmIndex;
    Bool isUpdate;
{
    if (isUpdate) {
	GRV.FocusFollowsMouse = !GRV.FocusFollowsMouse;
	ListApply(ActiveClientList, unconfigureFocus, 0);
	GRV.FocusFollowsMouse = !GRV.FocusFollowsMouse;
	ListApply(ActiveClientList, reconfigureFocus, 0);
    }
}


/*
 *	The Resource Manipulation Functions
 */


/*
 * UpdateDBValues - check all our dynamically configurable current 
 *	resources against the ones in this new db.  If any of them
 *	differ, then set the new global variable, update the value in 
 *	the global db, and call the appropriate update routine.
 *
 *	NOTICE that this assumes that all global variables have been
 *	set to something by this point (since they will be used by
 *	the setFunc()s).
 *
 *	The setFunc() does the actual comparison between the new db
 *	resource and the value previously set (using the appropriate
 *	global variable), since it "knows" how to convert the string
 *	representation of the resource into the appropriate type.
 */
void
#ifdef OW_I18N
UpdateDBValues( dpy, newDB, locale_only )
Display		*dpy;
XrmDatabase	newDB;
Bool		locale_only;
#else
UpdateDBValues( dpy, newDB )
Display		*dpy;
XrmDatabase	newDB;
#endif
{
	char			name[MAX_NAME];
	char			class[MAX_CLASS];
	XrmRepresentation	newRepType;
	XrmValue		newValue;
	int			ii;
#ifdef OW_I18N
	char			name_locale[MAX_NAME];
	char			class_locale[MAX_CLASS];
#endif

	for ( ii = 0 ; ii < RM_ENTRYEND ; ii++ )
	{
		/* first, ignore all the non-dynamically settable resources */
		switch ( ii )
		{
		case RM_DISPLAY:
		case RM_PREFIX:
		case RM_USE3D:
		case RM_CURSORFONT:
				continue;
		}
#ifdef OW_I18N
		if (locale_only == True
		 && (RM_LC_BEGIN > ii || ii >= RM_LC_END))
			continue;
#endif

		/* check to see if this resource is set in the new db */
		MakeRMName( name, ii );
		MakeRMClass( class, ii );
#ifdef OW_I18N
		/*
		 * Check for the locale dependent (and/or specific)
		 * setting.
		 */
		if ( locale_only == False && GRV.LC.BasicLocale.locale != NULL
		  && RMTable[ii].localeDep == True )
		{
			sprintf( name_locale, "%s.%s",
				name, GRV.LC.BasicLocale.locale );
			sprintf( class_locale, "%s.%s",
				class, GRV.LC.BasicLocale.locale );
			if ( XrmGetResource( newDB, name_locale, class_locale,
					&newRepType, &newValue ) )
				goto set;
		}
#endif OW_I18N
		if ( !XrmGetResource( newDB, name, class, &newRepType, 
				      &newValue ) )
			continue;
		
#ifdef OW_I18N
set:
#endif
		/* let the setFunc convert the newValue, compare it against
		 * the old, and then set the global variable to that new
		 * value
		 */
		if ( RMTable[ii].setFunc( dpy, ii, newValue.addr, True ) )
		{
			/* if we really updated to using the new value,
			 * store it in the global db for future reference
			 */
			XrmPutStringResource( &OlwmDB, name, newValue.addr );
			if ( RMTable[ii].updFunc != (BFuncPtr)0 )
				RMTable[ii].updFunc( dpy, ii, True );
		}
	}
#ifdef OW_I18N
	if ( locale_only == True )
		EffectOLLC( NULL );
	else
		EffectOLLC( dpy );
#endif
}


#ifdef OW_I18N
/*
 * InitGRVLV: Here is the table for OPEN LOOK locale and POSIX/ANSI-C
 * locale categories.
 */
GRVLCInit()
{
	GRV.LC.BasicLocale.posix_category	= LC_CTYPE;
	GRV.LC.DisplayLang.posix_category	= LC_MESSAGES;
	GRV.LC.InputLang.posix_category		= -1;
	GRV.LC.Numeric.posix_category		= LC_NUMERIC;
	GRV.LC.TimeFormat.posix_category	= LC_TIME;

	OLLCUpdated = False;
}

	
setOLLCPosix()
{
	register OLLCItem	*ollci, *ollci_end;
	register char		*current;
	register int		category;


	ollci = &(GRV.LC.BasicLocale);
	ollci_end = &ollci[OLLC_LC_MAX];
	for (ollci; ollci < ollci_end; ollci++)
	{
		if (ollci->locale == NULL
		 || ollci->priority >= OLLC_SRC_POSIX)
		{
			category = ollci->posix_category < 0 ?
					LC_CTYPE : ollci->posix_category;
			ollci->locale = strdup(setlocale(category, NULL));
			OLLCUpdated = True;
		}
	}

}


EffectOLLC(dpy)
Display	*dpy;
{
	register OLLCItem	*ollci, *ollci_end, *ollci_basic;
	register char		*current;
	register int		require_all_update;
	static int		messages_updated = False;


	if (OLLCUpdated == False)
		goto msg;

	require_all_update = False;
	ollci_basic = &(GRV.LC.BasicLocale);
	ollci_end = &ollci_basic[OLLC_LC_MAX];

	/*
	 * This is silly restriction, but works well for Sundae1.0
	 * environment.
	 */
	current = setlocale(LC_CTYPE, NULL);
	if (strcmp(current, "C") == 0
	 && ollci_basic->locale != NULL
	 && strcmp(current, ollci_basic->locale) != 0)
	{
		/*
		 * I'm doing LC_ALL rather only LC_CTYPE, becuase
		 * there are some locale category which does not
		 * covered by current OLLC spec.
		 */
#ifdef DEBUG
fprintf(stderr, "Basic Locale -> %s\n", ollci_basic->locale);
#endif
		if (setlocale(LC_ALL, ollci_basic->locale) == NULL)
		{
			fprintf(stderr, gettext("\
olwm: Warning: '%s' is invalid locale, use 'C' locale.\n"),
				ollci_basic->locale);
			(void) setlocale(LC_ALL, "C");
		}
		require_all_update = True;
	}


	for (ollci = ollci_basic + 1; ollci < ollci_end; ollci++)
	{
		if (ollci->posix_category < 0)
			continue;
		current = setlocale(ollci->posix_category, NULL);
#ifdef DEBUG
fprintf(stderr, "locale#%d [%s] -> [%s]...",
	ollci->posix_category, current, ollci->locale);
#endif
		if (strcmp(current, ollci->locale) != 0
		 || require_all_update == True)
		{
			/*
			 * Again, we need following silly restriction
			 * in order to work well with EUC based
			 * environment.
			 */
			if (strcmp(current, "C") == 0
			 || strcmp(ollci->locale, "C") == 0
			 || strcmp(ollci->locale, ollci_basic->locale) == 0)
			{
				if (setlocale(ollci->posix_category,
					ollci->locale) == NULL)
				{
					fprintf(stderr, gettext("\
olwm: Warning: '%s' is invalid locale (category#%d), use 'C' locale.\n"),
						ollci->locale,
						ollci->posix_category);
					(void) setlocale(ollci->posix_category,
							 "C");
				}
				if (ollci->posix_category == LC_MESSAGES)
					messages_updated = True;
#ifdef DEBUG
fprintf(stderr, "Done!\n");
#endif
			}
#ifdef DEBUG
else fprintf(stderr, "No because restriction\n");
#endif
		}
#ifdef DEBUG
else fprintf(stderr, "No because it same\n");
#endif
	}
	OLLCUpdated = False;

msg:
	if (messages_updated == True && dpy != NULL)
	{
#ifdef DEBUG
fprintf(stderr, "Re-creating the Menus\n");
#endif
		WindowMenuDestroy(dpy);
		WindowMenuCreate(dpy);
		UserMenuDestroy(dpy);
		UserMenuCreate(dpy);
		messages_updated = False;
	}
}
#endif OW_I18N
