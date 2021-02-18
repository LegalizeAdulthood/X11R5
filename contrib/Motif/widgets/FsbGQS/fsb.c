/*** This only a example of use for the convenience function
     XmcFsbGetQualifyString provided in FsbGQS.c.
***/

#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include <Xm/Text.h>
#include "FsbGQS.h"

static void ok_callback(widget,tag, callb)
Widget widget;
XtPointer tag;
XtPointer callb;
{
    printf("Unqualified selection: %s\n",
	   XmTextGetString(XmFileSelectionBoxGetChild(widget, 
						      XmDIALOG_TEXT)));
    printf("Qualified selection: %s\n",
	   XmcFsbGetQualifyString(widget));
}


main(argc, argv) unsigned int argc; char **argv;
{
    Widget      toplevel, fsb ;
    XtAppContext app_context;

    toplevel = XtAppInitialize(&app_context, "Test", NULL, 0,
			       &argc, argv, NULL, NULL, 0);
    fsb = XmCreateFileSelectionBox(toplevel, "fsb", NULL, 0);
    XtAddCallback (fsb, XmNokCallback, ok_callback, NULL);
    XtManageChild(fsb);
    
    XtRealizeWidget(toplevel);
    XtAppMainLoop(app_context);
}

