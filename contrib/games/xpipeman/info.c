/*
 * help.c  - Xpipeman
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



Widget info_popup;

/*ARGSUSED*/
static XtCallbackProc 
info_popdown_callback(w, closure, call_data)
  Widget w;
  caddr_t closure;
  caddr_t call_data;
{
  XtPopdown(info_popup);
}

/*XtCallbackProc*/
void
show_info()
{
  XtPopup(info_popup, XtGrabNone);
}

static Arg arglist_info_title[] = {
  {  XtNborderWidth, (XtArgVal) 0  },
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 320   },
  {  XtNheight, (XtArgVal) 50   },
  {  XtNlabel, (XtArgVal) "Xpipeman 1.5"  },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};

static Arg arglist_copyright_label[] = {
  {  XtNborderWidth, (XtArgVal) 0  },
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 320   },
  {  XtNlabel, (XtArgVal) "Written by Nigel Paver 1991"  },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};

static Arg arglist_author_label1[] = {
  {  XtNborderWidth, (XtArgVal) 0  },
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 320   },
  {  XtNlabel, (XtArgVal) "Current Address"  },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};

static Arg arglist_author_label2[] = {
  {  XtNborderWidth, (XtArgVal) 0  },
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 320   },
  {  XtNlabel, (XtArgVal) "Dept. Computer Science"  },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};

static Arg arglist_author_label3[] = {
  {  XtNborderWidth, (XtArgVal) 0  },
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 320   },
  {  XtNlabel, (XtArgVal) "Manchester University"  },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};

static Arg arglist_author_label4[] = {
  {  XtNborderWidth, (XtArgVal) 0  },
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 320   },
  {  XtNlabel, (XtArgVal) "ENGLAND M13 9PL"  },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};

static Arg arglist_author_label5[] = {
  {  XtNborderWidth, (XtArgVal) 0  },
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 320   },
  {  XtNlabel, (XtArgVal) "Janet: pavern@uk.ac.man.cs"  },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};

static Arg arglist_author_label6[] = {
  {  XtNborderWidth, (XtArgVal) 0  },
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 320   },
  {  XtNlabel, (XtArgVal) "Inet: pavern%uk.ac.man.cs@nsfnet-relay.ac.uk"  },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};

static Arg arglist_blank_label[] = {
  {  XtNborderWidth, (XtArgVal) 0  },
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 320   },
  {  XtNlabel, (XtArgVal) "  "  },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};

static Arg arglist_info_popdown[] = {
  {  XtNborderWidth, (XtArgVal) 2  },
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 320   },
  {  XtNlabel, (XtArgVal) "Cancel"  },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};


void
create_info_popup(parent)
  Widget parent;
{
  int i;
  Widget info_msg, info_popdown,info_box;
  Arg wargs[1];
  char *message;
  
  info_popup = XtCreatePopupShell(
                                   "info_popup",
                                   transientShellWidgetClass,
                                   parent, 0,0);

  info_box = XtCreateManagedWidget(
                                    "info_box",
                                    boxWidgetClass,
                                    info_popup,
                                    0,0);

   (void)XtCreateManagedWidget(
                                    "info_title",
                                    labelWidgetClass,
                                    info_box,
                                    arglist_info_title,
                                    XtNumber(arglist_info_title));

   (void)XtCreateManagedWidget(
                                    "copyright_dec",
                                    labelWidgetClass,
                                    info_box,
                                    arglist_copyright_label,
                                    XtNumber(arglist_copyright_label));

   (void)XtCreateManagedWidget(
                                    "blank",
                                    labelWidgetClass,
                                    info_box,
                                    arglist_blank_label,
                                    XtNumber(arglist_blank_label));

   (void)XtCreateManagedWidget(
                                    "author1",
                                    labelWidgetClass,
                                    info_box,
                                    arglist_author_label1,
                                    XtNumber(arglist_author_label1));

   (void)XtCreateManagedWidget(
                                    "author2",
                                    labelWidgetClass,
                                    info_box,
                                    arglist_author_label2,
                                    XtNumber(arglist_author_label2));
   (void)XtCreateManagedWidget(
                                    "author3",
                                    labelWidgetClass,
                                    info_box,
                                    arglist_author_label3,
                                    XtNumber(arglist_author_label3));
   (void)XtCreateManagedWidget(
                                    "author4",
                                    labelWidgetClass,
                                    info_box,
                                    arglist_author_label4,
                                    XtNumber(arglist_author_label4));
   (void)XtCreateManagedWidget(
                                    "author5",
                                    labelWidgetClass,
                                    info_box,
                                    arglist_author_label5,
                                    XtNumber(arglist_author_label5));
   (void)XtCreateManagedWidget(
                                    "author6",
                                    labelWidgetClass,
                                    info_box,
                                    arglist_author_label6,
                                    XtNumber(arglist_author_label6));


   (void)XtCreateManagedWidget(
                                    "blank",
                                    labelWidgetClass,
                                    info_box,
                                    arglist_blank_label,
                                    XtNumber(arglist_blank_label));

 info_popdown = XtCreateManagedWidget(
                                    "info_popdown",
                                    commandWidgetClass,
                                    info_box,
                                    arglist_info_popdown,
                                    XtNumber(arglist_info_popdown));


  XtAddCallback(info_popdown,XtNcallback,info_popdown_callback,0);
 
}

