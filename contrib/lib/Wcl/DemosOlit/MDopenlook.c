/*
** Copyright (c) 1990 David E. Smyth
**
** Redistribution and use in source and binary forms are permitted
** provided that the above copyright notice and this paragraph are
** duplicated in all such forms and that any documentation, advertising
** materials, and other materials related to such distribution and use
** acknowledge that the software was developed by David E. Smyth.  The
** name of David E. Smyth may not be used to endorse or promote products
** derived from this software without specific prior written permission.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/

/******************************************************************************
**
** SCCS_data: @(#)MDopenlook.c 1.0 ( 17 Mar 91 )
**
** Description:	This file contains main() for an example demonstrating
**		how to support multiple displays.  This file is derived
**		from Ari.c, and which allows prototype interfaces to be 
**		built from resource files.  The Widget Creation library 
**		is used.
**
**		Note that a real application would probably want to keep
**		the multiple displays syncronized.  The only way to do
**		that is to specifically cause each widget to update all
**		the other widgets.  This takes PROGRAMMING!!!
**
*******************************************************************************
**   Include_files.
******************************************************************************/

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <ctype.h>
#include <Xol/OpenLook.h>
#include <Wc/WcCreate.h>
#include <Wc/WcCreateP.h>	/* for callback convenience funcs	*/

/******************************************************************************
**  Global Application Data
******************************************************************************/

int          gargc;
char**       gargv;

char*        appName;
char*        appClass;
XtAppContext app;

static char msg[MAX_ERRMSG];


/******************************************************************************
**  Private Functions
******************************************************************************/

extern void OriRegisterOpenLook ();

/*
    -- Open a new display, build another widget tree on that display.
******************************************************************************
*/

static void NewWidgetTreeOnDisplay( this, name, ignored )
    Widget  this;
    char*   name;	/* display name from res file */
    caddr_t ignored;
{
    static char clean[128];	/* display name w/o leading & trailing junk */
    int		i;
    int		largc;
    char**	largv;
    Display*    dpy;
    Widget      newShell;
    Arg		args[3];
    XtAppContext app      = XtWidgetToApplicationContext(this);

    /* -- get clean display name by stripping leading & trailing whitespace */
    (void)WcCleanName( name, clean );

    /* -- make local copy of global argc and argv */
    largc = gargc;
    largv = (char**)XtMalloc( (gargc+1) * sizeof(char*) );
    for (i = 0 ; i < gargc ; i++)
	largv[i] = gargv[i];
    largv[i] = NULL;

    /* -- open display, build Xrdb, add to app context */
    dpy = XtOpenDisplay( 
	app, clean, appName, appClass, NULL, 0, &largc, largv );
    if (dpy == NULL)
    {
	sprintf( msg, "NewWidgetTreeOnDisplay( %s ) failed:\
		Could not open display %s", clean, clean);
	XtWarning( msg );
	return;
    }

    /* -- create new shell on new display */
    /* these args are exactly what XtAppInitialize sets. */
    i = 0;
 /* XtSetArg(args[i], XtNscreen, DefaultScreenOfDisplay(dpy));	i++; */
    XtSetArg(args[i], XtNargc, largc);				i++;
    XtSetArg(args[i], XtNargv, largv);				i++;

    newShell = XtAppCreateShell( appName, appClass,
                                 applicationShellWidgetClass, dpy, args, i );
    if (newShell == NULL)
    {
	sprintf( msg, "NewWidgetTreeOnDisplay( %s ) failed:\
		Could not create new application shell.", clean);
        XtWarning( msg );
        return;
    }

    /* -- create widget tree under this new application shell */
    if ( WcWidgetCreation ( newShell ) != 0 )
       return;

    /*  -- Realize the widget tree and return to the main application loop */
    XtRealizeWidget ( newShell );
    return;
}

/*
    -- Kill a widget tree on a display, close that display.
******************************************************************************
*/

static void KillWidgetTreeOnDisplay( this, notused, ignored )
    Widget  this;
    caddr_t notused;
    caddr_t ignored;
{
    Display* dpy = XtDisplay(this);
    XtDestroyWidget( WcRootWidget(this) );
    XtCloseDisplay ( dpy );
}

/*
    -- Register application specific callbacks.
******************************************************************************
*/

static void RegisterApplication ( app )
    XtAppContext app;
{
#define RCB( name, func  ) WcRegisterCallback  ( app, name, func, NULL );

    /* -- register callbacks to open a new display */
    RCB( "NewWidgetTreeOnDisplay",	NewWidgetTreeOnDisplay		);
    RCB( "KillWidgetTreeOnDisplay",	KillWidgetTreeOnDisplay		);
}

/******************************************************************************
*   MAIN function
******************************************************************************/

main ( argc, argv )
    int argc;
    char* argv[];
{   
    int		i;
    Widget	appShell;
    XtAppContext app;

    if ( argc < 2 ) {
      printf("usage: %s app-class-name [-dislay ...]\n", argv[0] );
      printf("where app-class-name specifies the app-defaults resource file\n");
      exit(1);
    }

    /* make copies of argc and argv for later use in NewWidgetTreeOnDisplay */
    gargc = argc;
    gargv = (char**)XtMalloc( (argc+1) * sizeof(char*) );
    for (i = 0 ; i < argc ; i++)
	gargv[i] = argv[i];
    gargv[i] = NULL;

    /* XtInitialize uses argv[0] as shell instance name */
    argv[0]  = "MDopenlook";   /* Use the old name for compatibility reasons */
    appName  = argv[0];
    appClass = argv[1];
 
    /*  -- Intialize Toolkit creating the application shell */
    appShell = OlInitialize ( appName, appClass, NULL, 0, &argc, argv );
    app      = XtWidgetToApplicationContext(appShell);

    /*  -- Register all application specific callbacks and widget classes */
    RegisterApplication ( app );

    /*  -- Register all classes and constructors for desired widget set */
    OriRegisterOpenLook ( app );

    /*  -- Create widget tree below toplevel shell using Xrm database */
    if ( WcWidgetCreation ( appShell ) != 0)
       return(1);

    /*  -- Realize the widget tree and enter the main application loop */
    XtRealizeWidget ( appShell );
    XtMainLoop ( );
}
