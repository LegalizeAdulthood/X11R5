/*
 * score.h  - Xpipeman
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
 *
 *
 * Acknowledgements to Brian Warkentine (brian@sun.COM) for his xrobots
 * program (copyright  1989) which I cannibalized to write this program.
 * Infact this file is simply lifted staright from xrobots with few
 * if any changes.
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

#include <X11/Xos.h>	/* brings in <sys/file.h> */
#include <stdio.h>
#include "xpipeman.h"

/*----------------------------------------------------------------------*/

typedef struct {
  char 	score[6], 
	name[26],
        level[5];
} SCORE;

static SCORE scores[MAXSCORES];

void 	show_scores(), 
	new_high_score(), 
	load_scores(), 
	write_out_scores();

static FILE *scorefile = 0;
char *score_filename;

/*----------------------------------------------------------------------*/

void
check_score(current_score,level)
  int current_score,level;
{

  load_scores();

  if(current_score > atoi(scores[MAXSCORES-1].score)) {
    new_high_score(current_score,level);
    write_out_scores();
  }
  if(scorefile) {
#ifndef SYSV
    flock(scorefile->_file, LOCK_UN);
#endif
    fclose(scorefile);
    show_scores();
  }
}


static void
load_scores()
{
  int i = 0;

  if( !(scorefile = fopen(score_filename,"r+")) ) {
    scorefile = fopen(score_filename, "w");
    return;
  }
#ifndef SYSV
  flock(scorefile->_file, LOCK_EX);
#endif
  while( fgets(scores[i].score,6,scorefile) 	/* get score */
      && fgets(scores[i].name,26,scorefile) 	/* get name */
      && fgets(scores[i].level,5,scorefile)     /* get level */
      && fgetc(scorefile))			/* and newline */
  {
    i++;
    if( i > MAXSCORES ) break;
  }
}


static void
new_high_score(current_score,level)
  int current_score,level;
{
  int i;
  char textscore[6],
       name[26],
       levelno[5];

  sprintf(textscore,"%5d",current_score);
  sprintf(levelno,"%4d",level);
  strncpy(name,getenv("USER"),25);

  for(i=MAXSCORES-2;i>=0;i--)
    if( current_score < atoi(scores[i].score) ) {
         /* move new score into i+1 slot */
      strcpy(scores[i+1].score,textscore);
      strcpy(scores[i+1].name,name);
      strcpy(scores[i+1].level,levelno);
      return;
    } else {
      strcpy(scores[i+1].score,scores[i].score);
      strcpy(scores[i+1].name,scores[i].name);
      strcpy(scores[i+1].level,scores[i].level);
    }
  /* if it got here, there is a new number 1 score */
  strcpy(scores[0].score,textscore);
  strcpy(scores[0].name,name);
  strcpy(scores[0].level,levelno);

}


static void
write_out_scores()
{
  int i;

  if( !scorefile )
    return;
  rewind(scorefile);
  for(i=0;i<MAXSCORES;i++)
    fprintf(scorefile,"%5s%25s%4s\n",
	    scores[i].score,scores[i].name,scores[i].level);
}


/*----------------------------------------------------------------------*/


Widget score_popup;
Widget score_labels[MAXSCORES];

static Arg arglist_score_title[] = {
  {  XtNborderWidth, (XtArgVal) 0  },
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 300   },
  {  XtNheight, (XtArgVal) 30   },
  {  XtNlabel, (XtArgVal) "High Scores"  },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};

static Arg arglist_score_label[] = {
  {  XtNlabel, (XtArgVal) 0  },
  {  XtNborderWidth, (XtArgVal) 0  },
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 300   },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};

static Arg arglist_popdown[] = {
/*  {  XtNborderWidth, (XtArgVal) 2  },*/
  {  XtNresize, (XtArgVal) False  },
  {  XtNwidth, (XtArgVal) 300   },
  {  XtNlabel, (XtArgVal) "Pop Down"  },
  {  XtNjustify, (XtArgVal) XtJustifyCenter  },
};


/*ARGSUSED*/
static XtCallbackProc 
popdown_callback(w, closure, call_data)
  Widget w;
  caddr_t closure;
  caddr_t call_data;
{
  XtPopdown(score_popup);
}


void
create_high_score_popup(parent)
  Widget parent;
{
  int i;
  Widget score_box, popdown;

  score_popup = XtCreatePopupShell(
                                   "score_popup",
                                   transientShellWidgetClass,
                                   parent, 0,0);

  score_box = XtCreateManagedWidget(
                                    "score_box",
                                    boxWidgetClass,
                                    score_popup,
                                    0,0);

  (void)XtCreateManagedWidget(
                                    "score_title",
                                    labelWidgetClass,
                                    score_box,
                                    arglist_score_title,
                                    XtNumber(arglist_score_title));

  for(i=0;i<MAXSCORES;i++) {
    score_labels[i] = XtCreateManagedWidget(
                                    "alabel",
                                    labelWidgetClass,
                                    score_box,
                                    arglist_score_label,
                                    XtNumber(arglist_score_label));
  }

  popdown = XtCreateManagedWidget(
                                    "popdown",
                                    commandWidgetClass,
                                    score_box,
                                    arglist_popdown,
                                    XtNumber(arglist_popdown));
  XtAddCallback(popdown,XtNcallback,popdown_callback,0);
}



void
show_scores()
{
  int i;
  char tmp_str[38];
  Arg tmp_arg;

  for(i = 0;i<MAXSCORES;i++) {
    sprintf(tmp_str,"%5s  %25s  %4s", 
	    scores[i].score, scores[i].name,scores[i].level);
    XtSetArg(tmp_arg,XtNlabel,tmp_str);
    XtSetValues(score_labels[i],&tmp_arg,1);
  }
  XtPopup(score_popup, XtGrabNone);
}


/*----------------------------------------------------------------------*/


/*XtCallbackProc*/
void
show_scores_callback()
{
  load_scores();

  if(scorefile) {
#ifndef SYSV
    flock(scorefile->_file, LOCK_UN);
#endif
    fclose(scorefile);
    show_scores();
  }
}


