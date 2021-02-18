/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/**************************************\
*   				       *
* 	BULL WINDOW MANAGER for X11    *
* 				       *
* 		error handling.	       *
* 				       *
\**************************************/

/*  include  */
#include 	<stdio.h>
#include	<errno.h>
#include 	"EXTERN.h"
#include 	"gwm.h"
#include	"wl_atom.h"
#include	"yacc.h"

/*  external  */

extern void     UserMessage();

/*  global  */

/* request error codes */

#define MAX_X_EVENT 64

#ifdef SECURE
char *eventtype[MAX_X_EVENT] = {
	"zero",
	"one",
	"KeyPress",
	"KeyRelease",
	"ButtonPress",
	"ButtonRelease",
	"MotionNotify",
	"EnterNotify",
	"LeaveNotify",
	"FocusIn",
	"FocusOut",
	"KeymapNotify",
	"Expose",
	"GraphicsExpose",
	"NoExpose",
	"VisibilityNotify",
	"CreateNotify",
	"DestroyNotify",
	"UnmapNotify",
	"MapNotify",
	"MapRequest",
	"ReparentNotify",
	"ConfigureNotify",
	"ConfigureRequest",
	"GravityNotify",
	"ResizeRequest",
	"CirculateNotify",
	"CirculateRequest",
	"PropertyNotify",
	"SelectionClear",
	"SelectionRequest",
	"SelectionNotify",
	"ColormapNotify",
	"ClientMessage",
	"MappingNotify",
	"LASTEvent",
	"GWMUserEvent"
};

#endif /* SECURE */

#ifdef DO_BUS_ERROR
#define DoABusError() *((int *)0) = 1
#else /* DO_BUS_ERROR */
#define DoABusError()
#endif /* DO_BUS_ERROR */

/*  routines  */

/* purposeful silent error handler */

NoXError(display, error)
Display        *display;
XErrorEvent    *error;
{
    ErrorStatus = error -> error_code;
}

/*
 * Normal verbose error handler 
 */

XError(display, error)
Display        *display;
XErrorEvent    *error;
{
    char            msg[MAX_TEMP_STRING_SIZE], buffer[MAX_TEMP_STRING_SIZE];
    char            number[32];

    /* aborts if anything occurs while decorating */

    if (GWM_window_being_decorated
	&& error -> error_code == BadWindow
	&& error -> resourceid == GWM_window_being_decorated -> client) {
	ClientWindowClose(GWM_window_being_decorated);
	GWM_window_being_decorated = 0;
	/* flush errors, there are surely more coming along! */
	FlushXErrors();
	longjmp(wool_goes_here_on_error, 1);
    } else if (wool_do_print_errors
#ifndef DEBUG
	       && error -> error_code != BadWindow
	       && error -> error_code != BadDrawable
#endif
	       ) {
	if (GWM_Synchronize) {		/* only meaningful when sync */
	    if (wool_is_reading_file) {
		wool_printf("\"%s\"", wool_is_reading_file);
		wool_printf(": line %d, ", yylineno);
	    }
	    wool_stack_dump(0);
	}
	wool_puts("Gwm X error: ");
	sprintf(number, "%d", error -> request_code);
	XGetErrorDatabaseText(dpy, "XRequest", number, "", buffer,
			      MAX_TEMP_STRING_SIZE);
	sprintf(msg, "%s(0x%x): ", buffer, error -> resourceid);
	wool_puts(msg);
	XGetErrorText(display, error -> error_code, msg,
		      MAX_TEMP_STRING_SIZE);
	wool_puts(msg);
	if (error -> minor_code)
	    sprintf(msg, " (minor = %d)", error -> minor_code);
	wool_newline();
	yyoutflush();
    }
    ErrorStatus = error -> error_code;
    stop_if_in_dbx();
    DoABusError();
}

Error(s1, s2)
char           *s1, *s2;
{
    UserMessage(s1, s2);
}

void
UserMessage(s1, s2)
char           *s1, *s2;
{
    wool_puts(s1);
    wool_puts(s2);
    wool_newline();
}

/* what to do when a wool-error occurs?
 */

wool_error_handler()
{
    if (GWM_ServerGrabbed)
	remove_grab(0);
    if (GWM_is_restarting || GWM_is_ending)
	GWM_BusErrorSignalHandler(0);	/* skip wool when error in ending */
    if (GWM_Window_being_opened)
    	XMapWindow(dpy, GWM_Window_being_opened -> hook);
}

/* some silent X primitives */

int
GwmSilentXGetWindowProperty(display,w,property,long_offset,long_length,
			    delete,req_type,actual_type_return,
			    actual_format_return,nitems_return,
			    bytes_after_return,prop_return)
    Display*		 display;
    Window		 w;
    Atom		 property;
    long		 long_offset;
    long		 long_length;
    Bool		 delete;
    Atom		 req_type;
    Atom*		 actual_type_return;
    int*		 actual_format_return;
    unsigned long*	 nitems_return;
    unsigned long*	 bytes_after_return;
    unsigned char**	 prop_return;
{
    int result;
    XSetErrorHandler(NoXError);
    result = XGetWindowProperty(display,w,property,long_offset,long_length,
			    delete,req_type,actual_type_return,
			    actual_format_return,nitems_return,
			    bytes_after_return,prop_return);
    XSync(dpy, 0);
    XSetErrorHandler(XError);
    return result;
}
