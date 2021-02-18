/*
 * main.c  - Xpipeman
 *
 * Send Constructive comments, bug reports, etc. to either
 *
 *          JANET: pavern@uk.ac.man.cs
 *
 *  or      INER : pavern%cs.man.ac.uk@nsfnet-relay.ac.uk
 *
 * All other comments > /dev/null !!
 *
 * 
 * Copyright 1991 Nigel Paver
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the author's name not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The author makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 * 
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE 
 * AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY 
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF 
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 *
 *
 * Acknowledgements to Brian Warkentine (brian@sun.COM) for his xrobots
 * program (copyright  1989) which I cannibalized to write this program
 */

#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#include <X11/Core.h>
#include <X11/Shell.h>

#ifdef R3
#include <X11/Label.h>
#include <X11/Command.h>
#include <X11/Box.h>
#else
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Box.h>
#endif


#include "xpipeman.h"

/*----------------------------------------------------------------------*/

Widget top_shell, top_widget, playfield_widget, score_command,
       bonus_command, remain_command, level_command,info_command;


Display *display;
Window playfield;

GC gc;
GC cleargc;

/*----------------------------------------------------------------------*/

static Arg arglisttop_shell[] = {
  {  XtNinput,  (XtArgVal)True },
};

static Arg arglistplayfield[] = {
  {  XtNheight, (XtArgVal) pos_to_coord(MAXY)  },
  {  XtNwidth,  (XtArgVal) pos_to_coord(MAXX)  },
  {  XtNborderWidth, (XtArgVal) 4 },
};

static Arg arglistquit_command[] = {
  {  XtNlabel, (XtArgVal) "Quit"  }, 
};

static Arg arglistnew_game[] = {
  {  XtNlabel, (XtArgVal) "New Game"  }, 
};

static Arg arglistscore_command[] = {
  {  XtNlabel, (XtArgVal) "Score:    0"  }, 
};

static Arg arglistbonus_command[] = {
  {  XtNlabel, (XtArgVal) "Bonus:    0"  }, 
};

static Arg arglistremain_command[] = {
  {  XtNlabel, (XtArgVal) "Remain:    0"  }, 
};

static Arg arglistlevel_command[] = {
  {  XtNlabel, (XtArgVal) "Level:    0"  }, 
};

static Arg arglistinfo_command[] = {
  {  XtNlabel, (XtArgVal) "Info"  }, 
};

/*----------------------------------------------------------------------*/

static char translations_str[] = 
  "<Btn1Down>:	do_nothing()		\n\
   <Btn2Down>:	do_nothing()		\n\
   <Btn3Down>:	do_nothing()		\n\
   <Btn1Up>:    move_here()		\n\
   <Btn2Up>:    place()    		\n\
   <Btn3Up>:    do_nothing()   		\n\
   :<Key>x:     move(right)		\n\
   :<Key>z:     move(left)		\n\
   :<Key>':     move(up)		\n\
   :<Key>/:     move(down)		\n\
   <Key>\\ :    place()        		\n\
   :<Key>p:     place()  		\n\
   <Key>Tab:    fastflow() 		\n\
   <Key>n:      new_game()		\n\
   Ctrl<Key>c:  quit() ";

static void quit_game();

static XtActionsRec actions[] = {
  {"quit",      (XtActionProc)quit_game},
  {"move",      (XtActionProc)move_action},
  {"move_here", (XtActionProc)move_here_action},
  {"fastflow",   (XtActionProc)fast_flow_action},
  {"place",     (XtActionProc)place_action},
  {"new_game",  (XtActionProc)new_game},
  {"do_nothing",(XtActionProc)do_nothing_action},
};

Pixel fg, bg;

XtTranslations translations;

static XrmOptionDescRec options[] = {
	{"-scorefile","scorefile",XrmoptionSepArg, NULL },
};

static XtResource application_resources[] = {
  {"foreground", "Foreground", XtRPixel, sizeof(Pixel),
                (Cardinal)&fg, XtRString, (caddr_t) "Black"},
  {"background", "Background", XtRPixel, sizeof(Pixel),
                (Cardinal)&bg, XtRString, (caddr_t) "White"},
  {"translations","Translations", XtRTranslationTable, sizeof(XtTranslations),
                (Cardinal)&translations, XtRString, (caddr_t)translations_str},
  {"scorefile","Scorefile", XtRString, sizeof(String),
                (Cardinal)&score_filename, XtRString, (caddr_t)SCORE_FILE},
};

/*----------------------------------------------------------------------*/

main(argc, argv)
  unsigned int argc;
  char **argv;
{
  Arg args[1];
  XGCValues gcv;
  Widget quit_command,
	 new_game_command;

  srandom(getpid());
  current_block = 0;
  current_callback = NULL;

  top_shell = XtInitialize(argv[0], "xpipeman", options, XtNumber(options), &argc, argv);
  XtSetValues(top_shell, arglisttop_shell, XtNumber(arglisttop_shell));

  XtAddActions(actions,XtNumber(actions));

  XtGetApplicationResources(top_shell, 0, application_resources, 
			XtNumber(application_resources), NULL, 0 );

  top_widget = XtCreateManagedWidget(
                                    "top_widget",
                                    boxWidgetClass,
                                    top_shell,
                                    0,0);

  playfield_widget = XtCreateManagedWidget(
                                    "playfield",
                                    widgetClass,
                                    top_widget,
                                    arglistplayfield,
                                    XtNumber(arglistplayfield));

  XtAugmentTranslations(playfield_widget,translations);

  quit_command = XtCreateManagedWidget(
                                    "quit_button",
                                    commandWidgetClass,
                                    top_widget,
                                    arglistquit_command,
                                    XtNumber(arglistquit_command));

  XtAddCallback(quit_command,     XtNcallback, (XtCallbackProc)quit_game, 0);

  new_game_command = XtCreateManagedWidget(
                                    "new_game_button",
                                    commandWidgetClass,
                                    top_widget,
                                    arglistnew_game,
                                    XtNumber(arglistnew_game));

  XtAddCallback(new_game_command, XtNcallback, (XtCallbackProc)new_game, 0);

  score_command = XtCreateManagedWidget(
                                    "score_button",
                                    commandWidgetClass,
                                    top_widget,
                                    arglistscore_command,
                                    XtNumber(arglistscore_command));

  XtAddCallback(score_command, XtNcallback, (XtCallbackProc)show_scores_callback, 0);
  /* buttons used so that they all look the same    */

  bonus_command = XtCreateManagedWidget(
                                    "bonus_button",
                                    commandWidgetClass,
                                    top_widget,
                                    arglistbonus_command,
                                    XtNumber(arglistbonus_command));

  remain_command = XtCreateManagedWidget(
                                    "remain_button",
                                    commandWidgetClass,
                                    top_widget,
                                    arglistremain_command,
                                    XtNumber(arglistremain_command));


  level_command = XtCreateManagedWidget(
                                    "level_button",
                                    commandWidgetClass,
                                    top_widget,
                                    arglistlevel_command,
                                    XtNumber(arglistlevel_command));

  info_command = XtCreateManagedWidget(
                                    "info_button",
                                    commandWidgetClass,
                                    top_widget,
                                    arglistinfo_command,
                                    XtNumber(arglistinfo_command));

  XtAddCallback(info_command, XtNcallback, (XtCallbackProc)show_info, 0);

  create_high_score_popup(top_widget);
  create_info_popup(top_widget);
  create_general_popups(top_widget);

  XtRealizeWidget(top_shell);


  display   = XtDisplay(playfield_widget);
  playfield = XtWindow(playfield_widget);
  gcv.foreground = fg;
  gcv.background = bg;
  gcv.function = GXcopy;
  gc = XCreateGC(display, playfield, 
 		GCForeground | GCBackground | GCFunction, &gcv);
  gcv.foreground = bg;
  cleargc = XCreateGC(display, playfield,
 		 GCForeground | GCBackground | GCFunction, &gcv);

  XtAddEventHandler(playfield_widget, ExposureMask, 0, redisplay_level, 0);
  XtAddEventHandler(playfield_widget, PointerMotionMask, 0, pointer_moved, 0);

  init_pixmaps(top_shell);

  XtMainLoop();

}


static void
quit_game()
{
  free_pixmaps();
  XtDestroyWidget(top_shell);
  XFreeGC(display,gc);
  XFreeGC(display,cleargc);

  exit(0);
}


void
update_score(score)
  int score;
{
  char text[13];
  (void)sprintf(text,"Score: %4d",score);
  XtSetArg(arglistscore_command[0],XtNlabel,text);
  XtSetValues(score_command,arglistscore_command,1);
}

void
update_bonus(score)
  int score;
{
  char text[13];
  (void)sprintf(text,"Bonus: %4d",score);
  XtSetArg(arglistbonus_command[0],XtNlabel,text);
  XtSetValues(bonus_command,arglistbonus_command,1);
}

void
update_remain(score)
  int score;
{
  char text[13];
  (void)sprintf(text,"Remaining: %4d",score);
  XtSetArg(arglistremain_command[0],XtNlabel,text);
  XtSetValues(remain_command,arglistremain_command,1);
}

void
update_level(level)
  int level;
{
  char text[13];
  (void)sprintf(text,"Level: %4d",level);
  XtSetArg(arglistlevel_command[0],XtNlabel,text);
  XtSetValues(level_command,arglistlevel_command,1);
}



