/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xpipeinput.c
 */

#include <stdio.h>
/*
 * Include files required for all Toolkit programs
 */
#include <X11/Intrinsic.h>	/* Intrinsics Definitions */
#include <X11/StringDefs.h>	/* Standard Name-String definitions */

/*
 * Public include file for widgets we actually use in this file.
 */
#include <X11/Xaw/Command.h>		/* Athena Label Widget */

/*
 * Quit button callback function
 */
/* ARGSUSED */
void Quit(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{ 
	fprintf(stderr, "It was nice knowing you.\n");
	exit(0); 
}

/* ARGSUSED */
get_file_input(client_data, fid, id)
XtPointer client_data;	/* unused */
int *fid;
XtInputId *id;
{
	char buf[BUFSIZ];
	int nbytes;
	int i;


	if ((nbytes = read(*fid, buf, BUFSIZ)) == -1)
		perror("get_file_input");

	if (nbytes)
		for (i = 0; i < nbytes; i++)
			putchar(buf[i]);
}

main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_context;
	Widget topLevel, goodbye;
	FILE *fid;
	String filename;

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
		"XPipeInput",	/* Application class */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

	if (argv[1] == NULL) {
		fprintf(stderr, "xpipeinput: no command specified\n");
		exit(1);
	}

	filename = argv[1];

	goodbye = XtCreateManagedWidget(
		"goodbye",		/* arbitrary widget name */
		commandWidgetClass,	/* widget class from Label.h */
		topLevel,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	XtAddCallback(goodbye, XtNcallback, Quit, 0);

	/* open file, pipe, or socket */
	/* for pipe, fid is fileno(stdin) */
	if ((fid = popen(filename, "r")) == NULL)
		fprintf(stderr, "xpipeinput: couldn't open pipe");

	/* register function to handle that input */
	XtAppAddInput(app_context, fileno(fid), XtInputReadMask, get_file_input, NULL);

	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	/*
	 *  Loop for events.
	 */
	XtAppMainLoop(app_context);
}
