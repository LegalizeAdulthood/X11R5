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
   main.c
   Main program module

*/

#define DEBUGmain      10&DEBUGON

#include <stdio.h>

#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Cardinals.h>
#include <signal.h>


#include "subscriber.h"
#include "tool.h"
#include "network.h"
#include "protocol.h"
#include "token.h"  
#include "resources.h"
#include "popups.h"

int catch_PIPE_sig();

int VERBOSE=0; /* 0 means no reports about in/out packets */
int JOIN=1; /* 0 means late joiners will not get old tools */
int POPOFF=0; /* 0 means to intervene in popoups and fix them */
int POLICY = 1; /* 0  means to reminder message policy */
int XCOLOR = 1; /* 0 means not to translate color */
int MinFloorTime = MINFLOORTIME;
BOOL TimeOutFlag = FALSE;



char HHostName[100];
char    INITIAL_MSG1[1024] = {"Welcome to XTV -- X TerminalView --\nA Framework for Sharing X Window Clients\n in Remote Synchronous Collaboration\n"};


Widget      Outer;
Widget      TopLevel;
Widget   WExit;
extern Widget    WSubscriberDrop;
extern Widget    WToolSToken;
extern Widget    WToolJoin;
extern void CloseAllTools();
extern void CleanUpAllTools();

/* ---------------------------------------------------------------------------------- */

Bool      Mode = FALSE;      /* TRUE if in Host mode */

Bool GetMode()
{
   return Mode;
}


XtAppContext   AppCon;

XtAppContext GetAppCon()
{
   return AppCon;
}

/* ---------------------------------------------------------------------------------- */


Widget GetTopLevelWidget()
{
   return TopLevel;
}

/* ---------------------------------------------------------------------------------- */

void InitializeQuit();

void main(argc, argv)
int argc;
char **argv;
{
   Arg     arg;
   static String   Chair_title = "XTV Control Panel : Chairman";
   static String   Parts_title  = "XTV Control Panel : Participant";
   int             pid;/* AKR 05/08 */

   Arg      *ArgL;

        siginterrupt(SIGPIPE, 1);
        siginterrupt(SIGALRM, 1);
   signal (SIGPIPE, catch_PIPE_sig);
#   if DEBUGmain
   fprintf( stderr, "main()\n" );
#   endif

   TopLevel = XtAppInitialize(   &AppCon,      /* Application Context */
   "xtv",          /* Class */
   NULL, ZERO,      /* Args? */
   &argc, argv,      /* Command-line arguments */
   FallbackResources,   /* Default Resources */
   NULL, ZERO);

   /* Build all widgets */


   /* Interpret options */
   while (--argc > 0 && (*++argv)[0] == '-') {

      char   *s;

      for (s = argv[0] + 1; *s; s++)
         switch (*s) {

         case 'h':
            fprintf( stderr, "Chairman Usage: %s  [-c -j -p -v -h -r n ]\n\n", argv[-1] );
            fprintf( stderr, "Participant Usage: %s  [-v] host_name session_number\n\n", argv[-1] );
fprintf (stderr, "Options are:\n");
fprintf (stderr, "-c (To Turn OFF Color Translations)\n");
fprintf (stderr, "-j (To Turn OFF Dynamic Joining feature)\n");
fprintf (stderr, "-p (To Turn OFF PopUp Menues feature )\n");
fprintf (stderr, "-v (To Turn ON  Verbose Mode)\n");
fprintf (stderr, "-h (To Display this help message)\n");
fprintf (stderr, "-r n (To Change Token Reminder Message to be every n seconds\n");
fprintf (stderr, "       Current default value of n is %d seconds)\n", MINFLOORTIME/1000);
            exit( 1 );
            break;
              case 'v':
            VERBOSE = 1;
            break;
              case 'j':
            JOIN = 0;
            break;
              case 'r':
            POLICY = 1;
            if (!*++argv || sscanf(*argv,"%d",&MinFloorTime)==0)
                                        exit(2);
            MinFloorTime *= 1000;
            if (MinFloorTime < MINFLOORTIME){
              MinFloorTime = MINFLOORTIME;
              fprintf(stderr,"Minimum Floor Time cannot be less than %d seconds\n",
                MINFLOORTIME/1000);
                 }
#   if DEBUGmain
   fprintf( stderr, "main(): Minimum floor time is %d seconds\n", MinFloorTime/1000);
#   endif
            argc--;
            break;
         case 'p':
            POPOFF = 1;
            break;
         case 'c':
            XCOLOR = 0;
            break;

         default:
            fprintf (stderr, "Unknown option: -%c\n\n", *s);
fprintf (stderr, "Options are:\n-j (Dynamic Joinis OFF)\n-p (PopUp Menu Mode is OFF)\n");
fprintf (stderr, "-v (Verbose Mode is ON)\n-h for help message\n");
fprintf (stderr, "-r n (Token Reminder Message every n seconds)\n");
fprintf (stderr, "-c (Turn OFF Color Translations)\n");
            argc = 0;
            break;
         }
         }
   if (argc > 3) {
      fprintf( stderr, "Usage: %s host_name session_number\n", argv[0] );
      exit( 1 );
   }

   if ( argc == 2 ) {
#   if DEBUGmain
   fprintf( stderr, "main(): Subscriber: Try to contact seession %d at host %s\n",
      atoi(argv[argc -1])-1 , argv[argc -2] );
#   endif
      strcpy(HHostName,argv[argc -2]);
      if ( TryToSubscribe(argv[argc -2], atoi(argv[argc -1])-1) < 0 )
         exit(1);
   } else {
#   if DEBUGmain
   fprintf( stderr, "main(): Chairman Mode\n");
#   endif
      Mode = TRUE;
   }


   Outer = XtCreateManagedWidget( "MainBox", formWidgetClass, TopLevel, NULL, 0 );
   /* Added 01 */
   if (Mode) {
      XtSetArg(arg,XtNtitle, Chair_title);
      XtSetValues(TopLevel, &arg, 1);
   }
   else {
      XtSetArg(arg, XtNtitle, Parts_title);
      XtSetValues(TopLevel, &arg, 1);
   }
   InitializeSubscriber( AppCon, Outer, Mode, argv[argc -1] );
   InitializeTool( AppCon, Outer, Mode );
        InitializeMisc( AppCon, Outer);
   InitializePacketTranslator();
   InitializeQuit( AppCon, Outer, Mode); 
        InitializeAllPopUps();

#   if DEBUGmain
   fprintf( stderr, "main(): Initialized\n" );
#   endif


   /* Crank it up! */
   XtRealizeWidget(TopLevel);
        if (!Mode) {
      XtUnmapWidget(WSubscriberDrop);
      XtUnmapWidget(WToolSToken);
        }
	else
      XtUnmapWidget(WToolJoin);

/*
 * Begin Murray Addition
 *
 * For those shells that don't automatically append a return, here it is.
 *
 */
   printf("\r");
/*
 * End Murray Addition
 */

   XtAppMainLoop(AppCon);
}

catch_PIPE_sig()
{
#   if DEBUGmain
        fprintf( stderr, "catch_PIPE_sig(): Tool Terminated \n");
#   endif
}




XtCallbackProc QuitSession (w, client_data, call_data)
Widget   w;
caddr_t client_data, call_data;
{
      XtSetSensitive(WExit, FALSE);
      PopUpBox(w, TranShell[Quit], call_data);
}

XtCallbackProc CancelQuitProc (w, client_data, call_data)
Widget   w;
caddr_t client_data, call_data;
{
      XtPopdown(TranShell[Quit]);
      XtSetSensitive(WExit, TRUE);
}

XtCallbackProc ConfirmQuitProc (w, client_data, call_data)
Widget   w;
caddr_t client_data, call_data;
{
   Subscriber      *Who;


   XtPopdown(TranShell[Quit]);
      XtSetSensitive(WExit, TRUE);

   if ( GetMode() ) {

      /* If in host mode, send everybody a HS_EndSession */

   for ( Who=FirstSubscriber(); Who != NULL; Who = NextSubscriber()) {
      if (Who->SubscriberID != 0) {
         fprintf( stderr, "Quit(): Terminating %s\n", Who->LoginName );
         if (SendByte( Who->Connection, HS_EndSession )==ERROR) CleanUpSubscriber(Who);
      }
   }
   CleanUpAllTools();

   } else {

      /* If a subscriber, politely bow out */

      extern Socket SubscriberSocket;   /* Our connection to the host */
      if (SendByte( SubscriberSocket, SH_LeavingSession )==ERROR) EndSession(); 
      CloseAllTools();
   }

/*
 * Begin Murray Addition
 */
/*
 * This is purely for debugging purposes.
 * Now that the message buffer works, it's not needed.
 *
 *      dumpMessageBuffer();
 */

/*
 * This may not be necessary, but it's the right thing to do
 */
   freeMessageBuffer();
/*
 * End Murray Addition
 */

   XtDestroyWidget((Widget)client_data);
   exit(0);
}

void InitializeQuit( AppCon, Outer, Mode)
XtAppContext AppCon;
Widget Outer;
Bool Mode;      /* TRUE if in 'host' mode */
{
WExit = XtCreateManagedWidget( "QuitButton", commandWidgetClass, Outer, NULL, 0 );
XtAddCallback( WExit, XtNcallback, QuitSession, TranShell[Quit]);
}
