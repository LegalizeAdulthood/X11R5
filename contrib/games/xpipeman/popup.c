/*
 * popup.c  - Xpipeman
 *
 * Send Constructive comments, bug reports, etc. to either
 *
 *          JANET: pavern@uk.ac.man.cs
 *
 *  or      INER : pavern%cs.man.ac.uk@nsfnet-relay.ac.uk
 *
 * All other comments > /dev/null !!
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
 */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#ifdef R3
#include <X11/Box.h>
#include <X11/Command.h>
#include <X11/Label.h>
#else
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Label.h>
#endif

#include "xpipeman.h"



Widget game_over_popup,level_over_popup,nomore_popup;
int game_over_visible,level_over_visible,nomore_visible;

void
show_level_over_popup()
{
  level_over_visible=1;
  XtPopup(level_over_popup, XtGrabNone);
}

void
level_over_popdown()
{
 if (level_over_visible)  XtPopdown(level_over_popup);
 level_over_visible=0;

}

void
show_game_over_popup()
{
  game_over_visible=1;
  XtPopup(game_over_popup, XtGrabNone);
}

void
game_over_popdown()
{
  if (game_over_visible ) XtPopdown(game_over_popup);
  game_over_visible=0;
}

void
show_nomore_popup()
{
  nomore_visible=1;
  XtPopup(nomore_popup, XtGrabNone);
}

void
nomore_popdown()
{
  if (nomore_visible) XtPopdown(nomore_popup);
  nomore_visible=0;
}

void
all_popdown()
{   /* remove all popups */
 nomore_popdown();
 game_over_popdown();
 level_over_popdown();
}

static Arg arglist_game_over_label[] = {
  {  XtNborderWidth, (XtArgVal) 0  },
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 320   },
  {  XtNheight, (XtArgVal) 50   },
  {  XtNlabel, (XtArgVal) "Game Over"  },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};

static Arg arglist_level_over_label[] = {
  {  XtNborderWidth, (XtArgVal) 0  },
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 320   },
  {  XtNheight, (XtArgVal) 50   },
  {  XtNlabel, (XtArgVal) "Level Over"  },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};

static Arg arglist_nomore_label[] = {
  {  XtNborderWidth, (XtArgVal) 0  },
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 320   },
  {  XtNheight, (XtArgVal) 50   },
  {  XtNlabel, (XtArgVal) "Sorry, No More Levels.."  },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};

static Arg arglist_continue_button[] = {
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 320   },
  {  XtNlabel, (XtArgVal) "Press to Continue"  },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};

/*ARGSUSED*/
static XtCallbackProc 
continue_callback(w, closure, call_data)
  Widget w;
  caddr_t closure;
  caddr_t call_data;
{
 level_over_popdown();
 start_new_level();
}

void
create_general_popups(parent)
  Widget parent;
{
  int i;
  Widget info_msg, game_over_box,level_over_box,continue_button,nomore_box;
  Arg wargs[1];
  char *message;
  
  game_over_popup = XtCreatePopupShell(
                                   "game_over",
                                   transientShellWidgetClass,
                                   parent, 0,0);

  game_over_box = XtCreateManagedWidget(
                                    "game_over_box",
                                    boxWidgetClass,
                                    game_over_popup,
                                    0,0);

   (void)XtCreateManagedWidget(
                                    "game_over_label",
                                    labelWidgetClass,
                                    game_over_box,
                                    arglist_game_over_label,
                                    XtNumber(arglist_game_over_label));


  
  nomore_popup = XtCreatePopupShell(
                                   "nomore",
                                   transientShellWidgetClass,
                                   parent, 0,0);

  nomore_box = XtCreateManagedWidget(
                                    "nomore_box",
                                    boxWidgetClass,
                                    nomore_popup,
                                    0,0);

   (void)XtCreateManagedWidget(
                                    "nomore_label",
                                    labelWidgetClass,
                                    nomore_box,
                                    arglist_nomore_label,
                                    XtNumber(arglist_nomore_label));


  level_over_popup = XtCreatePopupShell(
                                   "level_over",
                                   transientShellWidgetClass,
                                   parent, 0,0);

  level_over_box = XtCreateManagedWidget(
                                    "level_over_box",
                                    boxWidgetClass,
                                    level_over_popup,
                                    0,0);

   (void)XtCreateManagedWidget(
                                    "level_over_label",
                                    labelWidgetClass,
                                    level_over_box,
                                    arglist_level_over_label,
                                    XtNumber(arglist_level_over_label));

  continue_button = XtCreateManagedWidget(
                                    "Continue",
                                    commandWidgetClass,
                                    level_over_box,
                                    arglist_continue_button,
				    XtNumber(arglist_continue_button));

  XtAddCallback(continue_button,XtNcallback,continue_callback,0);

  nomore_visible=0;
  game_over_visible=0;
  level_over_visible=0;

}


