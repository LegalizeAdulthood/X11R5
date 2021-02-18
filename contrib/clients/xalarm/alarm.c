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
 * The stuff needed for the alarm call widget to appear on the screen, 
 * and the stuff for snoozing it.
 */


#include "xalarm.h"
#include <X11/Xatom.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/cursorfont.h>


/* 
 * A pain, you may have to alter this.  Basically USER() should return the
 * user name, HOSTNAME() the hostname in the first arg.
 */

#define		HOSTNAME(str,l)	gethostname ((str), (l))
#define		TITLEFORMAT	"%s: %s%s%s%s%s%s"



void		CreateAlarmWidget(), WakeUp(), ShowClickToZero(), ShowSnoozeValue();
static void	SnoozeAmount(), Snooze();
extern void	AddTimeOuts(), Quit();
extern String	getenv();


extern AlarmData        xalarm;



/* 
 * The snoozetime button displays its message when the ptr is in the 
 * window, and the snooze time when it's not.
 */

static char snoozetimetranslations [] =
    "<EnterWindow>:	highlight() ShowClickToZero() \n\
     <LeaveWindow>:	ShowSnoozeValue() unhighlight()";



void CreateAlarmWidget (alarmmessage)
  String   alarmmessage;
{
    Widget   alarm, message, buttonbox,
	     quit, snooze, snooze1, snooze5, snooze15, snoozetime;
    Arg      warg[5];
	  
    alarm = XtCreateManagedWidget ("alarm", formWidgetClass, xalarm.toplevel,
				   (ArgList) NULL, 0);

    buttonbox = XtCreateManagedWidget ("buttonbox", formWidgetClass, alarm,
				       (ArgList) NULL, 0);

    CreateManagedButton (quit, "quit", buttonbox, Quit, NULL);
    CreateManagedButton (snooze, "snooze", buttonbox, Snooze, NULL);
    CreateManagedButton (snooze1, "snooze1", buttonbox, SnoozeAmount, 1);
    CreateManagedButton (snooze5, "snooze5", buttonbox, SnoozeAmount, 5);
    CreateManagedButton (snooze15, "snooze15" ,buttonbox, SnoozeAmount, 15);
    CreateManagedButton (snoozetime, "snoozetime", buttonbox, SnoozeAmount, 0);

    XtSetArg (warg[0], XtNcursor, XCreateFontCursor (XtDisplay (xalarm.toplevel),
						     XC_gumby));
    if (alarmmessage == NULL) {
	message = XtCreateManagedWidget ("message", labelWidgetClass, alarm, warg, 1);
	XtSetArg (warg[0], XtNlabel, &xalarm.messagestr);
	XtGetValues (message, warg, 1);
    }
    else {
	XtSetArg (warg[1], XtNlabel, xalarm.messagestr = alarmmessage);
	message = XtCreateManagedWidget ("message", labelWidgetClass, alarm, warg, 2);
    }

    XtOverrideTranslations (snoozetime, XtParseTranslationTable (snoozetimetranslations));

    xalarm.snoozetimewidget = snoozetime;
    xalarm.messagewidget = message;
}



/* 
 * This is for the snoozetime button which doubles as the label showing how 
 * much time the snooze is selected for.
 */

void ShowClickToZero ()
{
    Arg   warg[1];

    XtSetArg (warg[0], XtNlabel, "Zero time");
    XtSetValues (xalarm.snoozetimewidget, warg, 1);
}
  


void ShowSnoozeValue ()
{  
    Arg    warg[1];
    char   buf[TEXT];

    MAKETIMESTRING (buf, (int) (xalarm.timeout/(60*1000)));
    XtSetArg (warg[0], XtNlabel, buf);
    XtSetValues (xalarm.snoozetimewidget, warg, 1);
}



/* 
 * Add to the amount that is currently the amount to snooze by.
 */

static void SnoozeAmount (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    int   mins = (int) clientdata;

    if (mins == 0) {
	xalarm.timeout = 0;
	ShowClickToZero ();
    }
    else {
	xalarm.timeout += (mins*60*1000);
	ShowSnoozeValue ();
    }
}



/* 
 * Time to sleep.  Add the time outs & disappear.
 */

static void Snooze (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    if (not XtIsRealized (xalarm.toplevel))
	XtRealizeWidget (xalarm.toplevel);

    AddTimeOuts ();
    XtUnmapWidget (xalarm.toplevel);
}



/* 
 * Time to wake up.
 */

void WakeUp (clientdata, id)
  XtPointer 	 clientdata;
  XtIntervalId 	 id;
{
    Display 	   *display = XtDisplay (xalarm.toplevel);
    String 	    strs[1], name;
    XTextProperty   property;
    Atom 	    deletewindow;
#if not defined (NOGETHOSTNAME)
    char 	    hostname[TEXT];
#endif

    if (not XtIsRealized (xalarm.toplevel)) {
	XtRealizeWidget (xalarm.toplevel);
	/* 
	 * Set the name...
	 */
	name = getenv ("NAME");
#if not defined (NOGETHOSTNAME)
	HOSTNAME (hostname, TEXT-1);
#endif
	strs[0] = XtMalloc (TEXT);
	(void) sprintf (strs[0], TITLEFORMAT, xalarm.proggie,
			getenv ("USER"),
#if not defined (NOGETHOSTNAME)
			"@", hostname,
#else
			"", "",
#endif
			(name == NULL) ? "" : " (",
			(name == NULL) ? "" : name,
			(name == NULL) ? "" : ")");

	XStringListToTextProperty (strs, 1, &property);
	XSetWMName (display, XtWindow (xalarm.toplevel), &property);
	/* 
	 * Set for f.delete...
	 */
	deletewindow = XInternAtom (display, "WM_DELETE_WINDOW", False);
	(void) XSetWMProtocols (display, XtWindow (xalarm.toplevel), &deletewindow, 1);
    }
    XtMapWidget (xalarm.toplevel);
    xalarm.timeout = 0;
    SnoozeAmount (xalarm.toplevel, (XtPointer) xalarm.snooze, (XtPointer) NULL);
    if (xalarm.bell)
	XBell (XtDisplay (xalarm.toplevel), 0);
    XFlush (display);
}
