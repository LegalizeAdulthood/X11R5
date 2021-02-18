#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xfuncs.h>
#include <X11/Xcu/Wlm.h>

static void exit_routine () ;

main(argc, argv)
    int argc ;
    char **argv ;
{
unsigned int saved_argc ;
char **saved_argv ;
Arg wlm_arg ;
Widget top, wlm_id ;
XtAppContext app ;
Display *dpy ;
int n ;
Arg args[2] ;

XtToolkitInitialize() ;
app = XtCreateApplicationContext () ;

saved_argc = argc ;
saved_argv = (char **)XtMalloc (argc * sizeof(char *)) ;
bcopy (argv, saved_argv, argc * sizeof(char *)) ;

dpy = XtOpenDisplay (app, NULL, NULL, "top", NULL, 0, &argc, argv ) ;
n=0;
XtSetArg(args[n], XtNargc, saved_argc) ;
n++ ;
XtSetArg(args[n], XtNargv, saved_argv) ;
n++ ;
top = XtAppCreateShell (NULL, "hello", applicationShellWidgetClass, dpy, args, n) ;
XtSetArg (wlm_arg, XtNfile, argv[1]) ;
wlm_id = XtCreateManagedWidget ( "wlm", xcuWlmWidgetClass, top, &wlm_arg, 1 ) ;

XcuWlmAddCallback ( wlm_id, NULL,
		   "XcuCommand", "exit", "callback",
		   exit_routine, NULL ) ;
XtRealizeWidget (top) ;

for (;;) {
	XcuWlmAppRequest ( app, wlm_id ) ;
	}
}

static void
exit_routine ()
{
exit (0) ;
}

#include <X11/Xcu/WlmP.h>
#include <X11/Xcu/Command.h>

void
make_tag_class_list (ww)
    XcuWlmWidget ww ;
{
TAG_CLASS_ENTRY(ww, "XcuWlm", xcuWlmWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuCommand", xcuCommandWidgetClass) ;
return ;
}

/*
 * Copyright 1991 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          580 Theory Center, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@graphics.cornell.edu
 */

