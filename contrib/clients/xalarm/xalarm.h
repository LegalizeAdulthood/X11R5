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

#ifndef xalarm_h
#define xalarm_h

/* 
 * Bits & bobs #defines, needed by most/all bits & bobs files.
 */

#include "patchlevel.h"

#include <stdio.h>
#include <ctype.h>
#include <signal.h>

#include <X11/Xos.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>


/* 
 * Change these if you want a different notation/language.  Keep it to 
 * 3 chars each, lower case only.
 */

#define		WEEKDAYS	"sun", "mon", "tue", "wed", "thu", "fri", "sat"
#define		MONTHS		"jan", "feb", "mar", "apr", "may", "jun", \
				"jul", "aug", "sep", "oct", "nov", "dec"

/* 
 * Application data - from the parsing of the command line args.
 */

typedef struct {
    String    snooze, proggie, geometry, time, warnings;
    Boolean   bell, confirm, list, version, help;
    int       reset;
} ApplData, *ApplDataPtr;


/* 
 * Global xalarm data, stuffed under one structure.
 */

typedef struct {
    XtAppContext   appcon;
    Widget 	   toplevel, messagewidget, snoozetimewidget,
		   gettimewidget, getwtimewidget, confirmwidget;
    Boolean 	   bell, confirm;
    String 	   proggie, geometry, messagestr, timestr, warningsstr;
    int 	   snooze, numwarnings;
    long 	   timeout, warnings[100], settime;
    XtIntervalId   timeouts[100];
} AlarmData, *AlarmDataPtr;

/* 
 * macro names...
 */

#define		XtNtime		"time"
#define		XtCTime		"Time"
#define		XtNwarnings	"warnings"
#define		XtCWarnings	"Warnings"
#define		XtNsnooze	"snooze"
#define		XtCSnooze	"Snooze"
#define		XtNconfirm	"confirm"
#define		XtCConfirm	"Confirm"
#define		XtNreset	"reset"
#define		XtCReset	"Reset"
#define		XtNbell		"bell"
#define		XtCBell		"Bell"
#define		XtNlist		"list"
#define		XtCList		"List"
#define		XtNversion	"version"
#define		XtCVersion	"Version"
#define		XtNhelp		"help"
#define		XtCHelp		"Help"

/* 
 * A R4 problem, I think, not apparent in R5.
 */

#ifndef XtNname
#define		XtNname		"name"
#endif
#ifndef XtCName
#define		XtCName		"Name"
#endif

/* 
 * And the rest...
 */

#define		or		||
#define		and		&&
#define		not		!
#define		INVALID		(-1)
#define		TEXT		1024

#if defined(SIGNALRETURNSINT)
#define		SIGRET		int
#else
#define		SIGRET		void
#endif


#define		CreateManagedButton(widget,name,parent,callback,clientdata) \
    widget = XtCreateManagedWidget (name, commandWidgetClass, parent, \
				    (ArgList) NULL, 0); \
    XtAddCallback (widget, XtNcallback, callback, (XtPointer) clientdata)


#define		ISINVALID(x)	((x) == (INVALID))

#define		PLURAL(x)	((((int) (x)) == 1) ? "" : "s")

#define		MAKETIMESTRING(buf,mins) \
    				if ((mins) >= 60) \
    (void) sprintf (buf, "%d:%02d hr%s", (mins)/60, (mins)%60, PLURAL ((mins)/60)); \
				else \
    (void) sprintf (buf, "%d min%s", (mins), PLURAL (mins));

#define		MAKETIME(buf,mins) \
    				if ((mins) >= 60) \
    (void) sprintf (buf, "%d:%02d", (mins)/60, (mins)%60); \
				else \
    (void) sprintf (buf, "%d", (mins));

	     
/*
 * DON'T PUT ANYTHING AFTER THIS!!!
 */
#endif
