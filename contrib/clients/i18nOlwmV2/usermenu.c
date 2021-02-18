/*
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) usermenu.c 50.10 90/12/12 Crucible";
/*
 * This file contains all of the functions for manipulating the user menu
 *
 * Global Functions:
 * InitUserMenu -- load the user menu and initialise
 *
 * Global data:
 * RootMenu -- workspace menu
 */

/*
 * Syntax of the user menu file should be identical to that used by
 *	buildmenu (SunView style rootmenu files).
 *
 *	NOTICE that SunView compatibility has resulted in old-style 
 *	olwm menus no longer being supported.
 *
 *	There are two new reserved keywords: 
 *
 *		DEFAULT tags a default button
 *		TITLE tags a title string for a menu (for titlebar)
 *
 *	One syntax in sunview menus is not supported:
 *		<icon_file> can not be used as a menu item 
 *
 *	Here are the common reserved keywords:
 *		MENU and END are used to delimit a submenu
 *		PIN (appearing after END) indicates the menu is pinnable
 *		EXIT (built-in - olwm service)
 *		REFRESH (built-in - olwm service)
 *		POSTSCRIPT will invoke psh on the named command
 *
 * 	The file is line-oriented, however commands to be executed can
 *	extend to the next line if the newline is escaped (\).
 *
 *	Each line consists of up to three fields:  a label (a string 
 *	corresponding to either the menu label or menu option label),
 *	up to two tags (keywords), and a command to be executed
 *	(or a file from which to read a submenu).  Two tags are allowed
 *	if one of them is "DEFAULT" or "END".  
 *
 *	The tag is used to indicate the start and end of menu definitions,
 *	pinnability, built-in functions, and default options.  
 *	The label indicates the text which appears on the user's menu,
 *	and the command describes what should be done when each item
 *	is selected.
 *
 *	Labels must be enclosed in double quotes if they contain 
 *	whitespace.  Commands may be enclosed in double quotes (but
 *	do not have to be).
 *
 *	Comments can be embedded in a file by starting a line with a
 *	pound sign (#).  Comments may not be preserved as the file is 
 *	used.
 *
 *	There are several functions which aren't invoked as programs;
 *	rather, they are built in to window manager.  These built-in
 *	services are each denoted by a single keyword.  The keywords are
 *	listed in the svctokenlookup[] array initialization.
 *
 *	example (will always have label: "Workspace Menu"):
 *
 *	"Workspace Menu"	TITLE
 *	Programs	MENU
 *		"Helpful Programs"	TITLE
 *		"Command Tool"	cmdtool
 *		"Blue Xterm"	DEFAULT xterm -fg white \
 *				-bg blue
 *	Programs	END	PIN
 *	Utilities	MENU
 *		"Refresh Screen" DEFAULT REFRESH
 *		"Clipboard"	 CLIPBOARD
 *	Utilities	END
 */

#ifdef OW_I18N
#include "i18n.h"
#endif
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include <sys/file.h>
#include <sys/param.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

extern char *strtok();		/* not defined in strings.h */

#ifdef OW_I18N
#include "ollocale.h"
#endif
#include "olwm.h"
#include "mem.h"
#ifdef OW_I18N
#include "win.h"
#endif
#include "menu.h"
#ifdef OW_I18N
#include "globals.h"
#endif

#define	MENUFILE	"openwin-menu"
#define TOKLEN		300

/* parseMenu return values */
#define MENU_FATAL     -1
#define MENU_NOTFOUND	0
#define MENU_OK		1
#define MENU_PINNABLE	2

typedef enum { UsrToken, MenuToken, EndToken, DefaultToken, PinToken, 
	TitleToken, ServiceToken, PshToken } TokenType;

/* locally useful macro */
#define	APPEND_STRING(buf, str)	( strncat( buf, str, \
					( sizeof(buf) - strlen(buf) - 1 ) ) )

/* Externals */
extern int RefreshFunc();
extern int ClipboardFunc();
extern int PrintScreenFunc();
extern int ExitFunc();
extern int PropertiesFunc();
extern int SaveWorkspaceFunc();
extern int FlipDragFunc();
extern int AppMenuFunc();
extern int PshFunc();
extern int NopFunc();
extern int WindowCtlFunc();
extern int RestartOLWM();
extern int FlipFocusFunc();

/* global data */
Menu *RootMenu;

/* local forward declarations */
static Bool	checkFile();
static int	menuFromFile();
static int	parseMenu();
static void	fillMenuStruct();
static TokenType lookupToken();
static Menu	*buildFromSpec();
static void	initMenu();
static void	initButton();
static void	freeButtonData();
static void	freeMenuData();
#ifdef OW_I18N
static void     freeMenu();
#endif OW_I18N

/* local data */
typedef struct _buttondata {
	struct _buttondata *next;
	char *name;
	Bool isDefault;
	Bool isLast;
	FuncPtr func;
	char *exec;		/* string to be executed, like "xterm" */
	void *submenu;
	} buttondata;


typedef struct {
	char *title;
	char *label;
	int idefault;		/* index of default button */
	int nbuttons;
	Bool pinnable;
	buttondata *bfirst;
} menudata;

#ifdef OW_I18N
menudata userroot = { NULL, NULL, -1, 0, True, NULL };
#else
menudata userroot = { "Workspace Menu", NULL, -1, 0, True, NULL };
#endif

/* default Root menu should be quite minimal */
#ifdef OW_I18N
/*
 * default will not be i18n, since system supposed to setup the i18n
 * user menu file.
 */
static wchar_t	wcs_Xterm[]	= {'X', 't', 'e', 'r', 'm', 0};
static wchar_t	wcs_Refresh[]	= {'R', 'e', 'f', 'r', 'e', 's', 'h', 0};
static wchar_t	wcs_Exit_WM[]	= {'E', 'x', 'i', 't', ' ', 'W', 'M', 0};
static wchar_t	wcs_Exit[]	= {'E', 'x', 'i', 't', 0};
static wchar_t	wcs_Workspace[]	= {'W', 'o', 'r', 'k', 's', 'p', 'a', 'c',
					'e', 0};
Button RootButtons[] = {
	{ wcs_Xterm, False, Enabled, {AppMenuFunc, (void *)"xterm"} },
        { wcs_Refresh, False, Enabled, {RefreshFunc, NULL} },
	{ wcs_Exit_WM, False, Enabled, {ExitOLWM, NULL} },
        { wcs_Exit, False, Enabled, {ExitFunc, NULL} },
        };
Menu DefaultRootMenu = { wcs_Workspace, RootButtons, 4, -1,
			True, (FuncPtr)MakeMenu};

#else OW_I18N

Button RootButtons[] = {
	{ "Xterm", False, Enabled, {AppMenuFunc, (void *)"xterm"} },
        { "Refresh", False, Enabled, {RefreshFunc, NULL} },
	{ "Exit WM", False, Enabled, {ExitOLWM, NULL} },
        { "Exit", False, Enabled, {ExitFunc, NULL} },
        };
Menu DefaultRootMenu = { "Workspace", RootButtons, 4, -1, True, (FuncPtr)MakeMenu};

#endif OW_I18N

/*
 * Global routines
 */

/*	
 * InitUserMenu	-- load the user menu from a file using menuFromFile()
 *	and then create the actual RootMenu using buildFromSpec().
 *
 *	The file to be read is either in the directory specified by 
 *	OLWMPATH or HOME, or OPENWINHOME/lib, and should be called 
 *	MENUFILE.  If none of those three files exist,
 *	use the default menu.
 *
 */
void
InitUserMenu(dpy)
Display		*dpy;
{
    char *getenv();
    char temp[MAXPATHLEN];
    char *path;
#ifdef OW_I18N
    extern char *strdup();
#endif

    RootMenu = (Menu *) 0;

#ifdef OW_I18N
    /* STRING_EXTRACTION - The next text is menu title for entire
       workspace.
     */
    userroot.title = strdup(gettext("Workspace Menu"));
#endif

    /*
     * SLEAZE
     * This isn't a loop.  It's a construct that makes "break" mean "jump to 
     * the statement just outside the brace."
     */
    while (1) {
	/* try reading $OLWMMENU */
	path = getenv("OLWMMENU");
	if (path != NULL && menuFromFile(path, &userroot, False) >= MENU_OK)
	    break;

	/* try reading $HOME/.openwin-menu */
	path = getenv("HOME");
	strcpy(temp, path ? path : "");
	strcat(temp, "/.");
	strcat(temp, MENUFILE);
	if (menuFromFile(temp, &userroot, False) >= MENU_OK)
	    break;

	/* try reading $OPENWINHOME/lib/openwin-menu */
	path = getenv("OPENWINHOME");
	strcpy(temp, path ? path : "");
	strcat(temp, "/lib/");
	strcat(temp, MENUFILE);
	if (menuFromFile(temp, &userroot, False) >= MENU_OK)
	    break;

	/* use default root menu */
	MenuCreate(dpy, &DefaultRootMenu);
	RootMenu = &DefaultRootMenu;
	return;
    }

    /* we read a menu from a file; now build it */
    RootMenu = buildFromSpec( dpy, &userroot, userroot.title );
}

#ifdef OW_I18N

static int	UserMenuCreated = False;

UserMenuCreate(dpy)
Display	*dpy;
{
	if (UserMenuCreated == True)
		return;
	InitUserMenu(dpy);
	UserMenuCreated = True;
}


UserMenuDestroy(dpy)
Display	*dpy;
{
	if (UserMenuCreated == False)
		return;
#ifdef OW_I18N_DEBUG
fprintf(stderr, "UserMenuDestroy ...\n");
#endif
	MenuDestroy(dpy, RootMenu);
	freeMenu(RootMenu);
#ifdef OW_I18N_DEBUG
fprintf(stderr, "UserMenuDestroy ... done\n");
#endif
	UserMenuCreated = False;
}
#endif OW_I18N

/*
 * Local routines
 */

/*
 * checkFile - check to see that a file (composed of named file and dir)
 *	is readable
 */
static Bool
checkFile( location, file, path )
char *location, *file, *path;
{
	char	*getenv();
	char	*dir;

	if ( (dir = getenv( location )) == NULL )
		return False;
	strcpy( path, dir );
	strcat( path, file );
	return ( access( path, R_OK ) == 0 );
}

/*
 * menuFromFile - read a menu description from a file
 *
 *	Return values: same as parseMenu, with the addition of
 *		MENU_NOTFOUND = couldn't read submenu file
 */
static int
menuFromFile(file, menu, messages)
char		*file;
menudata	*menu;
Bool		messages;
{
	FILE       *stream;
	int         lineno = 1;	/* Needed for recursion */
	int         rval;
#ifdef OW_I18N
	char	    file_locale[MAXPATHLEN];
#endif

#ifdef OW_I18N
	strcpy(file_locale, file);
	strcat(file_locale, ".");
	strcat(file_locale, setlocale(LC_MESSAGES, NULL));
#ifdef OW_I18N_DEBUG
fprintf(stderr, "menuFromFile: Try [%s] and [%s]\n", file_locale, file);
#endif
	if ((stream = fopen(file_locale, "r")) == NULL
	 && (stream = fopen(file, "r")) == NULL) {
#else OW_I18N
	stream = fopen(file, "r");
	if (stream == NULL) {
#endif OW_I18N
	    if (messages)
		fprintf(stderr, "olwm: can't open menu file %s\n", file);
	    return(MENU_NOTFOUND);
	}

	rval = parseMenu(file, stream, menu, &lineno);
	fclose(stream);
	if (rval >= MENU_OK)
	    fillMenuStruct(menu);
	else
	    freeMenuData(menu);

	return(rval);
}


/*
 * parseMenu -- read the user menu from the given stream and
 *	parse the stream into the menu structures defined locally.
 *	These structures (which are local to this module) are later
 *	used to build real menu structures.
 *
 *	Note that fillMenuStruct() needs to be called after parseMenu()
 *	is called (to finish filling out the menudata structure).
 *	If parseMenu() returns < 0, then freeMenuData() needs to be 
 *	called instead, to free up unused memory.
 *
 *	Return values:
 *		MENU_OK		= an unpinnable menu was read successfully
 *		MENU_PINNABLE	= a pinnable menu was read successfully
 *		MENU_FATAL	= a fatal error was encountered
 *
 *	This is based heavily on buildmenu's getmenu() parsing routine.
 *
 */
static int
parseMenu(filename, stream, rootmenu, lineno)
char	 *filename;
FILE	 *stream;
menudata *rootmenu;
int      *lineno;
{
	menudata	*currentMenu, *saveMenu;
	buttondata	*currentButton;
	char		line[TOKLEN];
	char		label[TOKLEN];
	char		prog[TOKLEN];
	char		args[TOKLEN];
static	char		localBuf[1024];
	char		*nqformat = 
				"%[^ \t\n]%*[ \t]%[^ \t\n]%*[ \t]%[^\n]\n";
	char		*qformat = 
				"\"%[^\"]\"%*[ \t]%[^ \t\n]%*[ \t]%[^\n]\n";
	char		*format;
	register char	*p;
	char		*tmpPtr;
	int		continuation;
	Bool		done;

	currentMenu = rootmenu;
	initButton( (buttondata **)&(currentMenu->bfirst) );
	currentButton = currentMenu->bfirst;
	continuation = 0;

	for ( ; fgets(line, sizeof(line), stream) ; (*lineno)++ )
	{
	    if (line[0] == '#') 
		    continue;

	    for ( p = line ; isspace(*p) ; p++ )
		    ;

	    if ( *p == '\0' )
		    continue;

	    /*
	     * if we're already on a continuation line (the previous 
	     * line ended in '\') then just copy the input through
	     * to the output until we get a line that doesn't end in 
	     * '\' (nuke the vi backslash).
	     */
	    if (continuation) 
	    {
		    /* fgets includes the newline in the string read */
		    while ( line[strlen(line) - 2] == '\\' )
		    {
			    /* get rid of backslash */
			    line[strlen(line) - 2] = '\0'; 
			    APPEND_STRING( localBuf, " " );
			    APPEND_STRING(localBuf, p);
			    if ( !fgets(line, sizeof(line), stream) )
				    break;
			    (*lineno)++;
			    for ( p = line ; isspace(*p) ; p++ )
				    ;
		    }
		    /* last line of continuation - replace \n with \0 */
		    line[strlen(line) - 1] = '\0';
		    APPEND_STRING( localBuf, " " );
		    APPEND_STRING(localBuf, p);
		    /* save it permanently in the buttondata structure */
		    currentButton->exec = MemNewString( localBuf );
		    localBuf[0] = '\0';
		    continuation = 0;
		    initButton( (buttondata **)&(currentButton->next) );
		    currentButton = currentButton->next;
		    continue;
	    }

	    /* if the line ends in '\' remember that continuation 
	     * has started.
	     */
	    if ( line[strlen(line) - 2] == '\\' )
	    {
		    continuation = 1;
		    line[strlen(line) - 2] = '\0';
	    }
	
	    args[0] = '\0';
	    format = ( *p == '"' ) ? qformat : nqformat;

	    if ( sscanf( p, format, label, prog, args ) < 2 )
	    {
		    fprintf(stderr,
#ifdef OW_I18N
		      gettext("olwm: syntax error in menu file %s, line %d\n"),
#else
			"olwm: syntax error in menu file %s, line %d\n", 
#endif
			    filename, *lineno);
		    return(MENU_FATAL);
	    }

	    if ( strcmp(prog, "END") == 0 ) 
	    {
		    /* currently allocated button is last for this menu */
		    currentButton->isLast = True;
		    if (currentMenu->label != NULL &&
			strcmp(label, currentMenu->label) != 0) {
			fprintf(stderr,
#ifdef OW_I18N
			    gettext("olwm: menu label mismatch in file %s, line %d\n"),
#else
			    "olwm: menu label mismatch in file %s, line %d\n",
#endif
			    filename, *lineno);
			return(MENU_FATAL);
		    }
		    if ( strcmp(args, "PIN") == 0 )
			    return(MENU_PINNABLE);
		    else
			    return(MENU_OK);
	    }

	    if ( strcmp(prog, "TITLE") == 0 ) 
	    {
		    currentMenu->title = MemNewString( label );
		    /* we don't need to set up the next button, since
		     * the TITLE line didn't use up a button
		     */
		    continue;
	    }

	    currentButton->name = MemNewString( label );

	    if ( strcmp(prog, "DEFAULT") == 0) {
		char *t;
		char *u;

		currentButton->isDefault = True;

		/*
		 * Pull the first token from args into prog.
		 */
		t = strtok(args, " \t");
		if ( t == NULL ) {
		    fprintf(stderr,
#ifdef OW_I18N
			    gettext("olwm: error in menu file %s, line %d\n"),
#else
			    "olwm: error in menu file %s, line %d\n",
#endif
			    filename, *lineno);
#ifdef OW_I18N
		    /* STRING_EXTRACTION - Since DEFAULT is keyword,
		       do not translate.
		     */
		    fputs(gettext("missing item after DEFAULT keyword.\n"), stderr);
#else
		    fputs("missing item after DEFAULT keyword.\n", stderr);
#endif
		    return(MENU_FATAL);
		}
		strcpy(prog, t);
		t = strtok(NULL, ""); /* get remainder of args */
		if (t == NULL)
		    args[0] = '\0';
		else {
		    u = args;
		    /* can't use strcpy because they overlap */
		    while ( *u++ = *t++ )
			;
		}
	    }

	    if ( strcmp(prog, "MENU") == 0 ) 
	    {
		    int         rval;

		    initMenu( (menudata **)&(currentButton->submenu) );
		    saveMenu = currentMenu;
		    currentMenu = (menudata *)currentButton->submenu;
		    currentMenu->label = MemNewString(label);

		    if (args[0] == '\0') 
		    {
			    /* we haven't incremented lineno for this
			     * read loop yet, so we need to do it now.
			     * when END is read, parseMenu returns without
			     * incrementing lineno, so the count will be
			     * ok when this loop increments it before
			     * reading the next line of the file.
			     */
			    (*lineno)++;
			    if ( (rval = parseMenu(filename, stream, 
						   currentMenu, lineno)) < 0 )
			    {
				freeMenuData( currentMenu );
				return(MENU_FATAL);
			    }
			    else
				fillMenuStruct( currentMenu );
		    }
		    else {
			rval = menuFromFile(args, currentMenu, True);
			if (rval <= MENU_NOTFOUND)
			    return(MENU_FATAL);
		    }
		    if ( rval == MENU_PINNABLE ) 
			    currentMenu->pinnable = True;

		    currentMenu = saveMenu;
		    /* if submenu not found, reuse button */
		    if ( rval != MENU_NOTFOUND )
		    {
		        initButton( (buttondata **)&(currentButton->next) );
		        currentButton = currentButton->next;
		    }
		    continue;			
	    }

	    done = False;
	    while ( !done )
	    {
		switch ( lookupToken( prog, &(currentButton->func) ) )
		{
		case UsrToken:
		    /* if UsrToken, that means that "prog" was just
		     * the first word of the command to be executed,
		     */
		    strcpy( localBuf, prog );
		    APPEND_STRING( localBuf, " " );
		    APPEND_STRING( localBuf, args );
 		    /* copy current contents of localBuf back into
		     * args array so that PshToken code can be used
		     */
		    strcpy( args, localBuf );
		    localBuf[0] = '\0';
		    /* fall through */
		case PshToken:
		    if (continuation) 
		    	strcpy( localBuf, args );
		    else
		    	currentButton->exec = MemNewString( args );
		    done = True;
		    break;
		case PinToken:
		    fprintf( stderr, 
#ifdef OW_I18N
			gettext("olwm: format error in menu file %s, line %d\n"),
#else
			"olwm: format error in menu file %s, line %d\n", 
#endif
			filename, *lineno );
#ifdef OW_I18N
		    /* STRING_EXTRACTION - "END" and "PIN" is keyword.
		     */
		    fputs(gettext("menu title and END required before PIN keyword.\n"),
#else
		    fputs("menu title and END required before PIN keyword.\n",
#endif
			  stderr);
		    return(MENU_FATAL);
		    break;
		default:
		    /* some other valid token found and returned */
		    done = True;
		    break;
		}
	    }
		    
	    if ( !continuation )
	    {
	        initButton( (buttondata **)&(currentButton->next) );
	        currentButton = currentButton->next;
	    }
	}
	/* never used the last button created */
	currentButton->isLast = True;

	return(MENU_OK);
}


/* 
 * fillMenuStruct - Once the menu structures have been filled out using 
 * 	information in the menu description file (via parseMenu()), the 
 * 	nbuttons and idefault elements need to be set.
 */
static void
fillMenuStruct( mptr )
menudata	*mptr;
{
	buttondata	*bptr;
	int		buttonIndex = 0;

	bptr = mptr->bfirst;
	if ( bptr->isLast == True )
	{
		MemFree( bptr );
		bptr = mptr->bfirst = NULL;
	}
	for ( ; bptr != NULL && bptr->isLast == False ; bptr = bptr->next )
	{
		if ( bptr->isDefault == True )
			mptr->idefault = buttonIndex;

		if ( (bptr->next)->isLast == True )
		{
			MemFree( bptr->next);
			bptr->next = NULL;
		}

		buttonIndex++;
	}
	/* buttonIndex is one past end, but started at 0, so = number buttons */
	mptr->nbuttons = buttonIndex;
}


/* 
 * Allowed menu keywords ("Token") 
 */

struct _svctoken {
	char *token;
	FuncPtr func;
	TokenType toktype;
} svctokenlookup[] = {
	{ "REFRESH", RefreshFunc, ServiceToken },
	{ "CLIPBOARD", ClipboardFunc, ServiceToken },
	{ "PRINT_SCREEN", PrintScreenFunc, ServiceToken },
	{ "EXIT", ExitFunc, ServiceToken },
	{ "WMEXIT", ExitOLWM, ServiceToken },
	{ "PROPERTIES", PropertiesFunc, ServiceToken },
	{ "NOP", NopFunc, ServiceToken },
	{ "DEFAULT", NULL, DefaultToken },
	{ "MENU", NULL, MenuToken },
	{ "END", NULL, EndToken },
	{ "PIN", NULL, PinToken },
	{ "TITLE", NULL, TitleToken },
	{ "WINDOW_CONTROLS", WindowCtlFunc, ServiceToken },
	{ "FLIPDRAG", FlipDragFunc, ServiceToken },
	{ "SAVE_WORKSPACE", SaveWorkspaceFunc, ServiceToken },
	{ "POSTSCRIPT", PshFunc, PshToken },
	{ "RESTART", RestartOLWM, ServiceToken },
	{ "FLIPFOCUS", FlipFocusFunc, ServiceToken },
};

#define NSERVICES (sizeof(svctokenlookup)/sizeof(struct _svctoken))

/* lookupToken -- look up a token in the list of tokens
 *	given a supposed keyword or service name.  If the name doesn't
 *	match any existing token, return the user-defined token.  
 */
static TokenType
lookupToken(nm,ppf)
char *nm;
FuncPtr *ppf;
{
	int ii;

	for (ii=0; ii<NSERVICES; ii++)
	{
		if (!strcmp(nm,svctokenlookup[ii].token))
		{
			if (ppf != (FuncPtr *)0) 
				*ppf = svctokenlookup[ii].func;
			return svctokenlookup[ii].toktype;
		}
	}
	if (ppf != (FuncPtr *)0)
		*ppf = AppMenuFunc;
	return UsrToken;
}


/* buildFromSpec -- build the real menu structures, and create the
 * 	associated menus, from the specifications parsed from
 *	the menu layout.  Free up the specifications as we go
 *	along.
 */
static Menu *
buildFromSpec(dpy,menu,deftitle)
Display *dpy;
menudata *menu;
char *deftitle;
{
	Menu *m;
	Button *b;
	int ii;
	buttondata *bdata, *bsave;

	m = MemNew(Menu);
	if (menu->pinnable)
	{
		m->hasPushPin = True;
		m->pinAction = (FuncPtr)MakeMenu;
		if (menu->title == NULL)
#ifdef OW_I18N
		{
			m->title = mbstowcsdup(deftitle);
		}
		else
		{
			m->title = mbstowcsdup(menu->title);
			MemFree(menu->title);
		}
#else OW_I18N
			m->title = deftitle;
		else
			m->title = menu->title;
#endif OW_I18N
	}
	else
	{
		m->hasPushPin = False;
		m->pinAction = NULL;
		/* non-pinnable menus only get titles if they ask for them */
		/* m->title must be NULL if menu->title is NULL */
#ifdef OW_I18N
		m->title = mbstowcsdup(menu->title);
		MemFree(menu->title);
#else
		m->title = menu->title;
#endif
	}

	m->buttonCount = menu->nbuttons;
	m->buttonDefault = menu->idefault;

	b = (Button *)MemAlloc((unsigned)(menu->nbuttons * sizeof(Button)));
	m->buttons = b;
	for ( ii=0, bdata=menu->bfirst ; ii<menu->nbuttons ; ii++ )
	{
#ifdef OW_I18N
		b[ii].label = mbstowcsdup(bdata->name);
#else
		b[ii].label = bdata->name;
#endif
		b[ii].stacked = bdata->submenu != NULL;
		b[ii].state = Enabled;
		b[ii].action.callback = bdata->func;
		if ( b[ii].stacked )
			b[ii].action.submenu = 
				(void *)buildFromSpec(dpy, 
					      (menudata *)(bdata->submenu),
					      bdata->name);
		else
			b[ii].action.submenu = (void *)bdata->exec;

#ifdef OW_I18N
		MemFree(bdata->name);
#endif
		bsave = bdata;
		bdata = bdata->next;
		MemFree(bsave);

	}

	MenuCreate(dpy,m);
#ifndef OW_I18N
	MemFree(menu->label);
	MemFree(menu);
#endif
	return(m);
}


/*
 * initMenu - 
 */
static void
initMenu( newmenu )
menudata	**newmenu;
{
	*newmenu = MemNew(menudata);
	(*newmenu)->title = NULL;
	(*newmenu)->label = NULL;
	(*newmenu)->idefault = -1;
	(*newmenu)->nbuttons = 0;
	(*newmenu)->pinnable = False;
	(*newmenu)->bfirst = (buttondata *)0;
}

/*
 * initButton - 
 */
static void
initButton( newButton )
buttondata	**newButton;
{
	*newButton = MemNew(buttondata);
	(*newButton)->next = NULL;
	(*newButton)->name = NULL;
	(*newButton)->isDefault = False;
	(*newButton)->isLast = False;
	(*newButton)->func = (FuncPtr)0;
	(*newButton)->exec = NULL;
	(*newButton)->submenu = NULL;
}

/*
 * freeMenuData - free any possibly allocated memory for this menudata 
 *	structure (and its buttons), since it's not going to be used
 */
static void
freeMenuData( unusedMenu )
menudata	*unusedMenu;
{
	buttondata	*unusedButton;
	buttondata	*saveButton;

	/* isLast probably isn't set, since this menu had an error */
	if ( ( unusedButton = unusedMenu->bfirst ) != (buttondata *)0 )
		freeButtonData( unusedButton );

	MemFree( unusedMenu->title );
	MemFree( unusedMenu->label );
	MemFree( unusedMenu );
	unusedMenu = NULL;
}

/*
 * freeButtonData - free any possibly allocated memory for this buttondata 
 *	structure, since it's not going to be used
 */
static void
freeButtonData( unusedButton )
buttondata	*unusedButton;
{

	if ( unusedButton->next != NULL )
		freeButtonData( unusedButton->next );

	MemFree( unusedButton->name );
	MemFree( unusedButton->exec );
	if ( unusedButton->submenu != NULL )
		freeMenuData( unusedButton->submenu );
	MemFree( unusedButton );
	unusedButton = NULL;
}


#ifdef OW_I18N
/*
 * freeMenu     - free's a dynamically allocated menu and its buttons
 *      This assumes that all components of the menu structure are
 *      unique and dynamically allocated.
 */
static void
freeMenu(menu)
Menu    *menu;
{
        int     i;

        if (!menu)
            return;

        MemFree(menu->title);
        for (i=0; i < menu->buttonCount; i++) {
                if (menu->buttons[i].stacked) {
                        freeMenu(menu->buttons[i].action.submenu);
                }
                else
                        MemFree(menu->buttons[i].action.submenu);
                MemFree(menu->buttons[i].label);
        }
        MemFree(menu->buttons);
        MemFree(menu);
}

#endif OW_I18N
