.ll +5
.PH ""
.ce
WLM's "Good-bye, World!"
.sp
.ta 0.25i 0.50i 0.75i 1.00i 1.25i 1.50i 1.75i 2.00i 2.25i 2.50i 2.75i 3.00i
The following two figures show how to implement a "Good-bye, world!" command
button, that causes the program to terminate.  Figure 1 is the Widget Layout
Description Language and Figure 2 is the client program.
.sp
.ce
FIGURE 1
.sp
.ps -1
XcuWlm {
.br
	XcuCommand {
.br
		name "exit"
.br
		label "Good-bye, world!"
.br
		}
.br
	}
.ps +1
.sp
.ce
FIGURE 2
.sp
.ps -1
.sp
#include <X11/Intrinsic.h>
.br
#include <X11/StringDefs.h>
.br
#include <X11/Shell.h>
.br
#include <Xcu/Wlm.h>
.sp
static void exit_routine () ;
.br
static Arg wlm_arg ;
.sp
main(argc, argv)
.br
    int argc ;
.br
    char **argv ;
.br
{
.br
Widget top, wlm_id ;
.br
top = XtInitialize ( NULL, "top", NULL, NULL, &argc, argv ) ;
.br
XtSetArg (wlm_arg, XtNfile, argv[1]) ;
.br
wlm_id = XtCreateManagedWidget ( "wlm", xcuWlmWidgetClass, top, &wlm_arg, 1 ) ;
.br
XcuWlmAddCallback ( wlm_id, NULL, "XcuCommand", "exit", "callback", exit_routine, NULL ) ;
.br
XtRealizeWidget (top) ;
.sp
for (;;) {
.br
	XcuWlmRequest ( wlm_id ) ;
.br
	}
.br
}
.sp
static void
.br
exit_routine ()
.br
{
.br
exit (0) ;
.br
}
.sp
#include <Xcu/WlmP.h>
.br
#include <Xcu/Wlm.h>
.br
#include <Xcu/Command.h>
.sp
void
.br
make_tag_class_list (ww)
.br
    XcuWlmWidget ww ;
.br
{
.br
TAG_CLASS_ENTRY(ww, "XcuWlm", xcuWlmWidgetClass) ;
.br
TAG_CLASS_ENTRY(ww, "XcuCommand", xcuCommandWidgetClass) ;
.br
return ;
.br
}
.ps +1
