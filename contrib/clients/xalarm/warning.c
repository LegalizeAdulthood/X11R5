/*
	 Copyright (c) 1991 by Simon Marshall, University of Hull, UK

		   If you still end up late, don't blame me!
				       
  Permission to use, copy, modify, distribute, and sell this software and its
       documentation for any purpose and without fee is hereby granted,
    provided that the above copyright notice appear in all copies and that
	both that copyright notice and this permission notice appear in
			   supporting documentation.
				       
  This software is provided AS IS with no warranties of any kind.  The author
    shall have no liability with respect to the infringement of copyrights,
     trade secrets or any patents by this file or any part thereof.  In no
      event will the author be liable for any lost revenue or profits or
	      other special, indirect and consequential damages.
*/

/* 
 * The stuff for dealing with warnings; processing to get the times & 
 * the fns for popping up the warnings themselves.
 */



#include "xalarm.h"

#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>


#define		WARNING		"Warning: %s due in "


void		SetWarningTimes(), Warning();
static void	QuitWarning();
extern long	TimeToMilliSeconds();
extern void	Quit();


extern AlarmData	xalarm;



/* 
 * Process the string to extract warning times.  Currently just gives 
 * a message if a warning time is invalid.
 */

void SetWarningTimes (warnings)
  String   warnings;
{
    String   warnstr = warnings;
    char     warning[TEXT];
    int      i;
	            
    xalarm.numwarnings = 0;
    while ((*warnstr != '\0') and (not ISINVALID (xalarm.numwarnings))) {
	warning[i=0] = '+';
	while ((*warnstr != ',') and (*warnstr != '\0'))
	    warning[++i] = *warnstr++;
	warning[++i] = '\0';

	switch (xalarm.warnings[xalarm.numwarnings] = TimeToMilliSeconds (warning)) {
	 case 0:
	    break;
	 case INVALID:
	    xalarm.numwarnings = INVALID;
	    break;
	 default:
	    xalarm.numwarnings++;
	    break;
	}

	while (*warnstr == ',')
	    warnstr++;
    }
    if ((warnstr != warnings) and (*(warnstr-1) == ','))
	xalarm.numwarnings = INVALID;
}



/* 
 * pop up a warning.  includes a dismiss & quit button.
 */

void Warning (clientdata, id)
  XtPointer 	 clientdata;
  XtIntervalId 	 id;
{
    Widget    popup, warningbox, dismiss, quit;
    Display  *display = XtDisplay (xalarm.toplevel);
    Arg       warg[2];
    char      buf[TEXT];

    popup = XtCreatePopupShell ("Warning!", topLevelShellWidgetClass, xalarm.toplevel,
				(ArgList) NULL, 0);

    warningbox = XtCreateManagedWidget ("warning", formWidgetClass, popup,
					(ArgList) NULL, 0);

    (void) sprintf (buf, WARNING, xalarm.proggie+1);
    MAKETIMESTRING (buf + strlen (buf), (int) clientdata);
    XtSetArg (warg[0], XtNlabel, buf);

    CreateManagedButton (dismiss, "dismiss", warningbox, QuitWarning, popup);
    XtCreateManagedWidget ("message", labelWidgetClass, warningbox, warg, 1);
    CreateManagedButton (quit, "quit", warningbox, Quit, NULL);

    if (xalarm.geometry != NULL) {
	XtSetArg (warg[0], XtNgeometry, xalarm.geometry);
	XtSetValues (popup, warg, 1);
    }
    XtPopup (popup, XtGrabNone);
    if (xalarm.bell)
	XBell (XtDisplay (xalarm.toplevel), 0);
    XFlush (display);
}
  
	     

static void QuitWarning (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    XtPopdown ((Widget) clientdata);
}
