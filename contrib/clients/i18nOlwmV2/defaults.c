/*
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) defaults.c 50.7 90/12/12 Crucible";
#ifdef OW_I18N
#include "i18n.h"
#endif
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <strings.h>
#include <sys/file.h>
#ifdef OW_I18N
#include <sys/param.h>
#endif
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>

char	*getenv();

#ifdef OW_I18N
#include "ollocale.h"
#endif
#include "olwm.h"
#include "defaults.h"
#include "globals.h"
#include "resources.h"

#ifdef OW_I18N_FIX
extern	char	*AppName;
#else
extern	char	*ProgramName;
#endif

extern	XrmDatabase	OlwmDB;

#ifdef OW_I18N
XrmDatabase	applicationLocaleDB = NULL;
#endif

/* private data */
static void parseResourceDefaults();
static void parseApplicationDefaults();

/* 
 * GetDefaults - Set up global OlwmDB with built-in default values.
 *		Then, initialize all the appropriate olwm variables
 *		using the information in the OlwmDB
 */
void
GetDefaults(dpy, commandlineDB)
Display		*dpy;
XrmDatabase	commandlineDB;
{
#ifdef OW_I18N
	char	commandlineDBfile[MAXPATHLEN + 1];


	/*
	 * There are two step we need to do here, first step is only
	 * for the locale annoucement resources, and set the locale
	 * value to the system (setlocale).  Now we know the which
	 * locale we should worry to.  Then second step is whole
	 * resource processing include the locale dependend resource
	 * file.
	 */
#endif OW_I18N

	/* put resource settings from default files into global OlwmDB */
	parseApplicationDefaults();
	parseResourceDefaults( dpy );

	/* merge command line options last into global OlwmDB
	 * notice that commandlineDB is destroyed by this call!
	 */
	if ( commandlineDB != NULL )
#ifdef OW_I18N
	{
		/*
		 * There are no way (well, no simple way) to re-create
		 * the commandlineDB, so, I will just keep it in file
		 * for now.
		 */
		(void) strcpy(commandlineDBfile, "/tmp/olwmDBXXXXXX");
		(void) mktemp(commandlineDBfile);
		XrmPutFileDatabase(commandlineDB, commandlineDBfile);
		(void) XrmMergeDatabases( commandlineDB, &OlwmDB );
	}
#else
		(void) XrmMergeDatabases( commandlineDB, &OlwmDB );
#endif OW_I18N

#ifdef OW_I18N
	SetAllDBValues( dpy, 1 /* locale only */ );


	/*
	 * Second step.
	 */
	parseApplicationLocaleDefaults();

	if (applicationLocaleDB != NULL)
	{
		/*
		 * Now we have locale specific application default
		 * database.  We already construct the Xrm database
		 * from other files/resources, but in order to
		 * straight the right precedence order, we need to do
		 * the merge process again.
		 */
		parseApplicationDefaults();

		(void) XrmMergeDatabases(applicationLocaleDB, &OlwmDB);

		parseResourceDefaults( dpy );

		if (commandlineDB != NULL)
		{
			commandlineDB = XrmGetFileDatabase(commandlineDBfile);
			if (commandlineDB != NULL)
				(void) XrmMergeDatabases(commandlineDB, &OlwmDB);
		}
	}
	
	if (commandlineDB != NULL)
		(void) unlink(commandlineDBfile);

	SetAllDBValues( dpy, 0 /* all of them! */ );
#else
	SetAllDBValues( dpy );
#endif
}


/* 
 * parseApplicationDefaults - get resource settings from application 
 *	default file
 */
static void
parseApplicationDefaults()
{
	XrmDatabase	applicationDB = NULL;
	char		filename[1024];
        char            *openWinPath;
	Bool		notFound = False;

        /* look for application default file */
	if ( (openWinPath = getenv( "OPENWINHOME" )) != NULL )
	{
		(void) strcpy( filename, openWinPath );
		(void) strcat( filename, "/lib/app-defaults/" );
#ifdef OW_I18N_FIX
		(void) strcat( filename, AppName );
#else
		(void) strcat( filename, ProgramName );
#endif
		if ( access( filename, R_OK ) != 0 )
			notFound = True;
	}
        else
		notFound = True;

	if ( notFound )
	{
        	(void) strcpy( filename, "/usr/lib/X11/app-defaults/" );
#ifdef OW_I18N_FIX
		(void) strcat( filename, AppName );
#else
		(void) strcat( filename, ProgramName );
#endif
	}

        applicationDB = XrmGetFileDatabase( filename );
	if ( applicationDB != NULL )
        	(void) XrmMergeDatabases( applicationDB, &OlwmDB );
}

#ifdef OW_I18N
/*
 * parseApplicationLocaleDefaults - get locale specific resource
 * settings from application default file
 */
void
parseApplicationLocaleDefaults()
{
	char		filename[1024];
        char            *openWinPath;
	Bool		notFound = False;
	char		*locale;

	if ((locale = setlocale(LC_CTYPE, NULL)) == NULL)
		return;

        /* look for application default file */
	if ( (openWinPath = getenv( "OPENWINHOME" )) != NULL )
	{
		(void) sprintf(filename, "%s/lib/locale/%s/app-defaults/%s",
				openWinPath, locale, AppName);
		if ( access( filename, R_OK ) != 0 )
			notFound = True;
	}
        else
		notFound = True;

	if ( notFound )
	{
		(void) sprintf(filename, "/usr/lib/X11/app-defaults/%s/%s",
				locale, AppName);
	}

	if ((applicationLocaleDB = XrmGetFileDatabase(filename)) == NULL
	  && strcmp(locale, "C") != 0)
		fprintf(stderr, "\
olwm: Warning: Could not find the locale specific resource DB file\n\
      for '%s' locale.\n",
			locale);
}
#endif OW_I18N

/* 
 * parseResourceDefaults - get resource settings from default user locations.
 *	Look first for server property - if it's there, use it alone.
 *	If it's not there, look for XENVIRONMENT file and use it.
 *	If still no luck, use $HOME/.Xdefaults.
 */
static void
parseResourceDefaults( dpy )
Display	*dpy;
{
	XrmDatabase	serverDB = NULL, homeDB = NULL;
	char		filename[1024];
	Atom		actualType;
	int		actualFormat;
	unsigned long	nitems, remain;
	char		*resourceString;
	char		*environmentFile;

	/*  look for xrdb/server specified defaults */
        resourceString = GetWindowProperty(dpy, RootWindow(dpy, DefaultScreen(dpy)),
                XA_RESOURCE_MANAGER, 0L, 100000000L,
                /* REMIND what should the length really be ? */
                XA_STRING, 0, &nitems, &remain);
        if ((resourceString != NULL) && (nitems != 0))
	{
		serverDB = XrmGetStringDatabase( resourceString );
		(void) XrmMergeDatabases( serverDB, &OlwmDB );
		XFree( resourceString );
		return;
	}

	/* if unsuccessful, look for user defaults: 
	 * first try XENVIRONMENT, next in $HOME/.Xdefaults 
	 */
	if ( ( (environmentFile = getenv( "XENVIRONMENT" )) == NULL )
	     || ( access( environmentFile, R_OK ) != 0 ) )
	{
		if ( (environmentFile = getenv( "HOME" )) != NULL )
		{
			(void) strcpy( filename, environmentFile );
			(void) strcat( filename, "/" );
			(void) strcat( filename, ".Xdefaults" );
			environmentFile = filename;
		}
	}
	if ( environmentFile != NULL )
	{
		homeDB = XrmGetFileDatabase( environmentFile );
		(void) XrmMergeDatabases( homeDB, &OlwmDB );
	}
	return;
}


/*
 * SetAllDBValues - read all the resources from the OlwmDB and
 *	set the appropriate olwm variables
 */
void
#ifdef OW_I18N
SetAllDBValues(dpy, locale_only)
Display *dpy;
Bool	locale_only;
#else
SetAllDBValues(dpy)
Display	*dpy;
#endif
{
	char			name[MAX_NAME];
	char			class[MAX_CLASS];
	XrmRepresentation	repType;
	XrmValue        	value;
	int             	ii;
#ifdef OW_I18N
	char			*locale;
	char			name_locale[MAX_NAME];
	char			class_locale[MAX_CLASS];
#endif

	/* find out if ProgramName is the name tag, or if the user
	 * has specified something else
	 *
	 * NOTICE that this was already done once in main() (for commandline
	 * arg only), is it possible to change in a resource specification?
	 */
	(void) SetNameTag( dpy, RM_PREFIX, NULL, False );

	/*  loop through all resources, except RM_DISPLAY and RM_PREFIX */
	for ( ii = 0 ; ii < RM_ENTRYEND ; ii++ )
	{
#ifdef OW_I18N
		if (locale_only == True
		 && (RM_LC_BEGIN > ii || ii >= RM_LC_END))
			continue;
#endif
	if ( ii == RM_DISPLAY || ii == RM_PREFIX )
			continue;

		MakeRMName( name, ii );
		MakeRMClass( class, ii );
#ifdef OW_I18N
		/*
		 * Check for the locale dependent (and/or specific)
		 * setting.
		 */
		locale = setlocale(LC_CTYPE, NULL);
		if ( locale_only == False && locale != NULL
		  && RMTable[ii].localeDep == True )
		{
			sprintf( name_locale, "%s.%s", name, locale );
			sprintf( class_locale, "%s.%s", class, locale );
			if ( XrmGetResource( OlwmDB, name_locale, class_locale,
					&repType, &value ) )
			{
				RMTable[ii].setFunc( dpy, ii,
							value.addr, False );
				continue;
			}
		}
#endif OW_I18N
		if ( XrmGetResource( OlwmDB, name, class, &repType, &value ) )
			RMTable[ii].setFunc( dpy, ii, value.addr, False );
		else
			RMTable[ii].setFunc( dpy, ii, NULL, False );
	}
#ifdef OW_I18N
	if (locale_only)
	{
		setOLLCPosix();
		/*
		 * We have been set the locale using POSIX rule to
		 * process the command line argument, but now we need
		 * to change back to the "C" locale (in order to
		 * EffectOLLC() work right).
		 */
		setlocale (LC_ALL, "C");
		/*
		 * We will create the menu window later on, and we are
		 * not ready to create menu window (require the
		 * InitGraphics).  So, I'm passing the NULL pointer
		 * for now.
		 */
		EffectOLLC( NULL );
	}
#endif	
}
