/*
 * Copyright (c) 1991 Old Dominion University.
 * Copyright (c) 1991 University of North Carolina at Chapel Hill.
 * All rights reserved.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  Old Dominion University and the University of North
 * Carolina at Chapel Hill make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

/*
        popups.c

	To create and mange dailogues and popup boxs
*/
#define DEBUGInitializePopUpBox 01&DEBUGON
#define DEBUGPopUpBox 01&DEBUGON

#include <stdio.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Shell.h>
#include <signal.h>

#include "subscriber.h"
#include "tool.h"
#include "network.h"
#include "protocol.h"
#include "token.h"
#include "popups.h"

extern XtCallbackProc ConfirmQuitProc(), CancelQuitProc();
extern XtCallbackProc ConfirmFryTool(), CancelFryTool();
extern XtCallbackProc ConfirmFrySubscriber(), CancelFrySubscriber();
extern XtCallbackProc DefaultHost();
extern XtCallbackProc DialogDoneHost();
extern XtCallbackProc DialogDoneRemote();
extern XtCallbackProc CancelDone();
extern XtCallbackProc DialogDoneJoinAll();
extern XtCallbackProc DialogDoneJoin();
extern XtCallbackProc JoinCancelDone();


extern Widget          Outer;
extern Widget          TopLevel;
extern Widget          WExit;


Widget TranShell[3];
Widget DialogBox[3];
Widget Confirm[3];
Widget Cancel[3];

String TranShellLabel[]={"TranShellQuit","TranShellTermTool","TranShellDropPart"};
String DialogBoxLabel[]={"QuitDialog","TermToolDialog","DropPartDialog"};
String ConfirmLabel[]={"ConfirmQuit","ConfirmTermTool","ConfirmDropPart"};
String CancelLabel[]={"CancelQuit","CancelTermTool","CancelDropPart"};

XtCallbackProc ConfirmCallbackProc[]={ConfirmQuitProc,ConfirmFryTool,ConfirmFrySubscriber};
XtCallbackProc CancelCallbackProc[]={CancelQuitProc,CancelFryTool,CancelFrySubscriber};

Widget      TranShellQuit, QuitDialog, ConfirmQuit, CancelQuit;
Widget      TranShellTermTool, TermToolDialog, ConfirmTermTool, CancelTermTool;
Widget      TranShellDropPart, DropPartDialog, ConfirmDropPart, CancelDropPart;

Widget      TranShellNewTool, NewToolDialog;

Widget	    TranShellJoinTool, WJoinParent, JoinToolLabel, JoinToolViewPort, JoinToolList;

		
/* -------------------------------------------------------------- ----------------------------- */

/*
 *   InitializePopUpBox()
 *
 *   Initialize all work related to PopUpBox button
 *
 */

void InitializePopUpBox(i)
BYTE i;
{

#   if DEBUGInitializePopUpBox
   fprintf( stderr, "InitializePopUpBox(): %s, %s, %s, %s\n",
	TranShellLabel[i], DialogBoxLabel[i], CancelLabel[i], ConfirmLabel[i] );
#   endif

   TranShell[i] = XtCreatePopupShell(
       TranShellLabel[i],
       transientShellWidgetClass,
       TopLevel,
       NULL,
       0
       );

   DialogBox[i] = XtCreateManagedWidget(
       DialogBoxLabel[i],
       boxWidgetClass,
       TranShell[i],
       NULL,
       0
       );

   Confirm[i]     = XtCreateManagedWidget(
       ConfirmLabel[i],
       commandWidgetClass,
       DialogBox[i],
       NULL,
       0
       );
XtAddCallback(Confirm[i], XtNcallback, ConfirmCallbackProc[i], TranShell[i]);

   Cancel[i] = XtCreateManagedWidget(
       CancelLabel[i],
       commandWidgetClass,
       DialogBox[i],
       NULL,
       0
       );

XtAddCallback(Cancel[i], XtNcallback, CancelCallbackProc[i], TranShell[i]);



#   if DEBUGInitializePopUpBox
   fprintf( stderr, "InitializePopUpBox(): Done.\n" );
#   endif

}

/* ---------------------------------------------------------------------------------- */

/*
 *   PopUpBox()
 *
 *   To PopUp a dialoge box in the center of the main panel
 *
 */

XtCallbackProc PopUpBox( widget, transhell, callData)
Widget widget, transhell;
XtPointer  callData;

{

   Dimension width, height;
   Arg arg[2];
   int i;
  int winx, winy;
  int rwinx, rwiny;
  unsigned int maskret;
  Window rootret, childret, root;
   Position x, y;



  /* Get the coordinates of the middle of TopLevel widget */


#       if DEBUGPopUpBox
        fprintf(stderr, "PopUpBox():QueryPointer\n" );
#       endif
  XQueryPointer(XtDisplay(Outer), XtWindow(Outer), &rootret, &childret,
	&rwinx, &rwiny, &winx, &winy, &maskret);



#       if DEBUGPopUpBox
        fprintf(stderr, "PopUpBox():Starting\n" );
#       endif

   i=0;
   XtSetArg(arg[i], XtNwidth, &width);
   i++;
   XtSetArg(arg[i], XtNheight, &height);
   i++;
/*
   XtGetValues(TopLevel, arg, i);
 */

   /*
 * translate coordinates in application top-level window
 * into coordinates from root window origin.
 */
/*
   XtTranslateCoords(TopLevel,
   (Position) width/2,
   (Position) height/2,
   &x, &y);
*/

   i=0;
   XtSetArg(arg[i], XtNx, rwinx);
   i++;
   XtSetArg(arg[i], XtNy, rwiny);
   i++;
   XtSetValues(transhell, arg, i);

#       if DEBUGPopUpBox
        fprintf(stderr, "PopUpBox():about to PopUp\n" );
#       endif
   XtPopup(transhell, XtGrabNonexclusive);
   XBell(XtDisplay(transhell), 10);

#       if DEBUGPopUpBox
        fprintf(stderr, "PopUpBox():finished PopUp\n" );
#       endif
}


/*------------------------------------------------------------------------------------------- */

/*
 *   InitializeNewTool()
 *
 *   Initialize all work related to NewTool button
 *
 */

void InitializeNewTool( AppCon, Parent, Mode )
XtAppContext AppCon;
Widget Parent;
Bool Mode;      /* TRUE if in 'host' mode */
{

   Widget Host, Remote, CancelNewTool; 

   static String   DefaultString = "";
   Arg     arg;
   static XtActionsRec PressHost[] = {
      {"defaultHost", DefaultHost}
   };




#   if DEBUGInitializeTool
   fprintf( stderr, "InitializeNewTool()\n" );
#   endif


   TranShellNewTool = XtCreatePopupShell(
       "TranShellNewTool",
       transientShellWidgetClass,
       TopLevel,
       NULL,
       0
       );

   NewToolDialog = XtCreateManagedWidget(
       "NewToolDialog",
       dialogWidgetClass,
       TranShellNewTool,
       NULL,
       0
       );

   Host     = XtCreateManagedWidget(
       "Host",
       commandWidgetClass,
       NewToolDialog,
       NULL,
       0
       );

   XtAddCallback(Host, XtNcallback, DialogDoneHost, NewToolDialog);

   Remote   =  XtCreateManagedWidget(
       "Remote",
       commandWidgetClass,
       NewToolDialog,
       NULL,
       0
       );

   XtAddCallback(Remote, XtNcallback, DialogDoneRemote, NewToolDialog);

   CancelNewTool = XtCreateManagedWidget(
       "CancelNewTool",
       commandWidgetClass,
       NewToolDialog,
       NULL,
       0
       );

   XtAddCallback(CancelNewTool, XtNcallback, CancelDone, 0);

   XtSetArg(arg, XtNvalue, DefaultString);
   XtSetValues(NewToolDialog, &arg, 1);
   XtAppAddActions(GetAppCon(), PressHost, XtNumber(PressHost));

#   if DEBUGInitializeTool
   fprintf( stderr, "InitializeNewTool(): Done.\n" );
#   endif
}

/*
 *   InitializeJoinTool()
 *
 *   Initialize all work related to JoinTool button
 *
 */
void InitializeJoinTool( AppCon, Parent )
XtAppContext AppCon;
Widget Parent;
{
        Widget WJoinAllTools, WJoinTool, WJoinCancel;

        TranShellJoinTool = XtCreatePopupShell(
                "TranShellJoinTool",
                transientShellWidgetClass,
                TopLevel,
                NULL,
                0
                );

        WJoinParent = XtCreateManagedWidget(
                "JoinParent",
                formWidgetClass,
                TranShellJoinTool,
                NULL,
                0
                );

        JoinToolLabel = XtCreateManagedWidget(
                "JoinToolLabel",
                labelWidgetClass,
                WJoinParent,
                NULL,
                0
                );

        JoinToolViewPort = XtCreateManagedWidget(
                "JoinToolViewPort",
                viewportWidgetClass,
                WJoinParent,
                NULL,
                0
                );

        JoinToolList = XtCreateManagedWidget(
                "JoinToolList",
                listWidgetClass,
                JoinToolViewPort,
                NULL,
                0
                );

        WJoinTool = XtCreateManagedWidget(
                "WJoinTool",
                commandWidgetClass,
                WJoinParent,
                NULL,
                0
                );

        XtAddCallback(WJoinTool, XtNcallback, DialogDoneJoin, TranShellJoinTool)
;

        WJoinCancel = XtCreateManagedWidget(
                "WJoinCancel",
                commandWidgetClass,
                WJoinParent,
                NULL,
                0
                );

        XtAddCallback(WJoinCancel, XtNcallback, JoinCancelDone, 0);

	WJoinAllTools = XtCreateManagedWidget(
		"WJoinAllTools",
		commandWidgetClass,
		WJoinParent,
		NULL,
		0
		);

	XtAddCallback(WJoinAllTools, XtNcallback, DialogDoneJoinAll, 0);
}

void InitializeAllPopUps()
{
InitializePopUpBox(Quit);

InitializePopUpBox(TermTool);

InitializePopUpBox(DropPart);
}
