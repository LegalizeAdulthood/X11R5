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
** Description:	This file contains a sample application which uses
**		Athena widgets.  The example functionally mimics the
**		xwidgets.c example provided in examples/Xaw on the
**		X11R4 tape from MIT.  The Widget Creation library
**		is used.
**
**              Several application specific callbacks are registered
**		with the Wc library, and bound to the interface by
**		specifications within the application resource file.
**
**		Note that this application has NO fallback resources.
**		Since the resource file actually defines the complete
**		widget tree, the resource file(s) MUST be found,
**		eliminating the raison d'etre of the fallbacks.
**
** Notes:	This program uses the Xrm (X resource management) database
**		for widget tree definition and management.
**
******************************************************************************/

/******************************************************************************
**   Include_files.
******************************************************************************/

/*  -- Minimum include files to use WidgetCreate Library */
#include <Wc/WcCreate.h>

/*  -- application specific include files */
#include <stdio.h>
#include <ctype.h>
#include <X11/Xaw/Dialog.h>
#include <X11/StringDefs.h>

/******************************************************************************
**  Private Functions
******************************************************************************/

extern void AriRegisterAthena ();

/*
    -- Tied to Dialog `Okay' button
******************************************************************************
    Adds an entry to the List widget whose name is passed as clientData.
*/

static void MyOkayCB( widget, widgetName, unused )
    Widget  widget;
    char*   widgetName;
    caddr_t unused;
{
    Widget list_widget = WcFullNameToWidget( widget, widgetName );
    static String * list = NULL;
    static int num_items = 0, allocated_items = 0;

    if (num_items == allocated_items) {
        allocated_items += 10;
        list = (String *) XtRealloc(list, sizeof(String) * allocated_items);
    }

    /*
     * The value returned by XawDialogGetValueString() does not remain good
     * forever so we must store is somewhere.  This creates a memory leak
     * since I don't remember these values, and free them later.  I know about
     * it, but it doesn't seem worth fixing in this example.
     */

    list[num_items++] = XtNewString(XawDialogGetValueString(XtParent(widget)));
    XawListChange(list_widget, list, num_items, 0, True);
}

/*
    -- MyQuitCB
******************************************************************************
    This function destroys the widget tree.
*/

static void MyQuitCB( widget, widgetName, ignored )
    Widget  widget;
    char*   widgetName;
    caddr_t ignored;
{
    Widget condemmed = WcFullNameToWidget( widget, widgetName );
    fprintf(stderr, "MyQuitCB(%s)\n", widgetName );
    if (condemmed == NULL)
        XtDestroyWidget( WcRootWidget( widget ) );
    else
	XtDestroyWidget( condemmed );
}

/*
    -- MyTimeSinceTouchedCB  Tied to the strip chart widget.
******************************************************************************
    This function returns the number of 10s of seconds since the user
    caused an event in this application.  
    Note that the StripChart requires a return value.
*/

static void MyTimeSinceTouchedCB( widget, ignored, value_ptr )
    Widget  widget;
    caddr_t ignored;
    double* value_ptr;
{
    double *value = (double *) value_ptr;
    static double old_value = 0.0;
    static Time old_stamp = 1;
    Time new_stamp;

    new_stamp = XtLastTimestampProcessed( XtDisplay( widget ) );

    if (old_stamp != new_stamp) 
    {
        old_stamp = new_stamp;
        old_value = 0.0;
    }
    else 
    {
        Arg args[1];
        int update;

        XtSetArg(args[0], XtNupdate, &update);
        XtGetValues(widget, args, 1);
        old_value += (double) update / 10.0;
    }

    *value = old_value;
}

/*
    -- MyCountCB  Tied to button, increments counter and displays.
******************************************************************************
   The argument is interpreted as a widget which must be a label.
   The count, after being incremented, is displayed in this label
   widget.  (Actually, it may be anything derived from Label).
*/

static void MyCountCB( widget, widgetName, ignored )
    Widget  widget;
    char*   widgetName;
    caddr_t ignored;
{
    Arg arg[1];
    char text[10];
    static int count = 0;

    sprintf( text, " %d ", ++count );
    XtSetArg( arg[0], XtNlabel, text );
    XtSetValues( WcFullNameToWidget( widget, widgetName ), arg, 1 );
}

/*
    -- MyThumbedCB  Tied to scrollbar's jumpProc
******************************************************************************
    Prints location of the thumb as a percentage of the height of the
    scrollbar into the labe widget named by the widgetName argument.
*/

static void MyThumbedCB( widget, widgetName, top_ptr )
    Widget  widget;
    char*   widgetName;	/* string from resource database (resource file) */
    float*  top_ptr;	/* scrollbar callback call data:  a pointer to a float
			** containing the location of the scrollbar's thumb.
			*/
{
    Widget label = WcFullNameToWidget( widget, widgetName );
    Arg args[1];
    char message[BUFSIZ];

    sprintf( message, " Thumbed to %d%% ", (int)((*top_ptr)*100) );
    XtSetArg( args[0], XtNlabel, message );
    XtSetValues( label, args, 1 );
}

/*
    -- MyScrolledCB  Tied to scrollbar's scrollProc
******************************************************************************
    Prints location of the pointer into the named label widget.
*/

static void MyScrolledCB( widget, widgetName, num_pixels )
    Widget  widget;
    char*   widgetName; /* string from resource database (resource file) */
    int*    num_pixels;	/* scrollbar callback call data:  the number of
			** pixels the bar has been scrolled.
			*/
{
    Widget label = WcFullNameToWidget( widget, widgetName );
    Arg args[1];
    char message[BUFSIZ];

    sprintf( message, " Scrolled at %d pixels", num_pixels);
    XtSetArg( args[0], XtNlabel, message );
    XtSetValues( label, args, 1 );
}

/*
    -- MyDestroyedCB  Destroy callback of application shell
******************************************************************************
    This is a Destroy callback that prints the name of the destroyed widget.
*/

static void MyDestroyedCB( widget, client, unused )
    Widget widget;
    char*  client;	/* as registered or from resource database */
    caddr_t unused;
{
    fprintf( stderr, "%s : Widget %s now destroyed.\n", 
	client, WcWidgetToFullName(widget));
}

/*
    -- RegisterApplicationCallbacks
******************************************************************************
    Register all the callbacks which define the application's behaviors.
*/

static void RegisterApplicationCallbacks ( app )
    XtAppContext app;
{
#define RCALL(name, func, client) WcRegisterCallback(app, name, func, client)

    RCALL( "MyOkayCB",			MyOkayCB,		NULL	  );
    RCALL( "MyQuitCB",			MyQuitCB,		NULL	  );
    RCALL( "MyTimeSinceTouchedCB",	MyTimeSinceTouchedCB,	NULL	  );
    RCALL( "MyCountCB",			MyCountCB,		NULL	  );
    RCALL( "MyThumbedCB",		MyThumbedCB,		NULL	  );
    RCALL( "MyScrolledCB",		MyScrolledCB,		"Scroll"  );
    RCALL( "MyDestroyedCB",		MyDestroyedCB,		"Destroy" );
}

/******************************************************************************
*   MAIN function
******************************************************************************/

main ( argc, argv )
    int argc;
    char* argv[];
{   
    char*        appClass;
    XtAppContext app;
    Widget       appShell;

    if ( argc < 2 ) {
      printf("usage: %s app-class-name [-dislay ...]\n", argv[0] );
      printf("where app-class-name specifies the app-defaults resource file\n");
      exit(1);
    }
    /* XtInitialize uses argv[0] as shell instance name */
    argv[0] = "App";
 
    /* With the Athena widgets, we MUST register the Wc
    ** converters first, so WcCvtStringToWidget is invoked
    ** instead of the far less useful XmuCvtStringToWidget.
    */
    WcAddConverters(NULL);

    /*  -- Intialize Toolkit creating the application shell */
    appShell = XtInitialize ( argv[0], argv[1], NULL, 0, &argc, argv );
    app = XtWidgetToApplicationContext(appShell);

    /*  -- Register all application specific callbacks and widget classes */
    RegisterApplicationCallbacks ( app );

    /*  -- Register all Athena widget classes */
    AriRegisterAthena ( app );

    /*  -- Create widget tree below toplevel shell using Xrm database */
    if ( WcWidgetCreation( appShell ) != 0 )
       return(1);

    /*  -- Realize the widget tree and enter the main application loop */
    XtRealizeWidget ( appShell );
    XtMainLoop ( );
}
