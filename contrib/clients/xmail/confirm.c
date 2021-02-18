/*
 * @(#)Confirm - a Yes/No confirmation window with optional prompt argument.
 *
 * From an idea contributed by Mitchell L. Model <mlm@odi.com> on 5-17-89
 *
 * Copyright 1990,1991 by National Semiconductor Corporation
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of National Semiconductor Corporation not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * NATIONAL SEMICONDUCTOR CORPORATION MAKES NO REPRESENTATIONS ABOUT THE
 * SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS"
 * WITHOUT EXPRESS OR IMPLIED WARRANTY.  NATIONAL SEMICONDUCTOR CORPORATION
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO
 * EVENT SHALL NATIONAL SEMICONDUCTOR CORPORATION BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Michael C. Wagnitz - National Semiconductor Corporation
 *
 */
#include "global.h"

#define	btnsepr	7

typedef struct _result {
			Bool	*popflg;
			Bool	*var;
			Widget	shell;
		       } *Result;
void
ProcessOneEvent(display)
Display *display;
{
 static XEvent event;

 XNextEvent(display, &event);
 XtDispatchEvent(&event);
} /* ProcessOneEvent */


static Result
MakeResult(shell, popflg, resultvar)
Bool *popflg, *resultvar;
Widget shell;
{
 Result rec = (Result) malloc(sizeof(struct _result));

 rec->popflg	= popflg;
 rec->var	= resultvar;
 rec->shell	= shell;

 return rec;
} /* MakeResult */


/* ARGSUSED */
/*
** ClearConfirm - get rid of the confirmation box.
*/
void
ClearConfirm(w, val, result)
Widget w;
Bool val;
Result result;
{
 XtPopdown(result->shell);
 XtDestroyWidget(result->shell);
 *result->popflg = FALSE;
 *result->var = val;
} /* ClearConfirm */


/* ARGSUSED */
static void
Yes(w, result, call_data)
Widget w;
Result result;
caddr_t call_data;			/* unused */
{
 ClearConfirm(w, TRUE, result);
}


/* ARGSUSED */
static void
No(w, result, call_data)
Widget w;				/* unused */
Result result;
caddr_t call_data;
{
 ClearConfirm(w, FALSE, result);
} /* No */


/*
 * Confirm - put up a window asking for confirmation.
 */
Bool
Confirm(prompt)
char*	prompt;
{
 Arg		args[4];
 Bool		popped_up = FALSE, result = FALSE;
 Result		resultrec;
 String		Translations = "<BtnDown>,<BtnUp>:\n";
 Widget		shell, form, label, yes, no;
 Window		root, child;
 int		root_x, root_y, child_x, child_y;
 int		nargs, buttons, labelwidth, yeswidth, btnborder, btnwidth;
 char		*query = NULL;

/*
** First, find out if expert flag is set.  If so, just return True.
*/
 if (XMail.expert == TRUE)
    return TRUE;
/*
** Find out where the mouse is, so we can put the confirmation
** box right there.
*/
 XQueryPointer(XtDisplay(toplevel), XtWindow(toplevel), &root, &child,
		  &root_x, &root_y, &child_x, &child_y, &buttons);
 /*
 ** Construct the confirmation box
 */
 shell=XtCreatePopupShell("Confirm",overrideShellWidgetClass,toplevel,args,0);

 XtSetArg(args[0], XtNtranslations, XtParseTranslationTable(Translations));
 XtSetArg(args[1], XtNborderWidth, (XtArgVal) 0);
 form = XtCreateManagedWidget("form", formWidgetClass, shell, args, 2);

 if (! prompt) label = NULL;
 else {
    XtFree(query);
    query = NULL;
    if (query = XtMalloc(strlen(prompt) + 2)) {
       (void) strcpy(query, prompt);
       (void) strcat(query, "?");
      }
    XtSetArg(args[0], XtNlabel, (XtArgVal) query);
    XtSetArg(args[1], XtNjustify, XtJustifyCenter);
    XtSetArg(args[2], XtNborderWidth, (XtArgVal) 0);
    label = XtCreateManagedWidget("prompt", labelWidgetClass, form, args, 3);
   }

 nargs = 0;
 XtSetArg(args[nargs], XtNhorizDistance, (XtArgVal) btnsepr);	nargs++;
 if (prompt) {
    XtSetArg(args[nargs], XtNfromVert, (XtArgVal) label);	nargs++;
    XtSetArg(args[nargs], XtNvertDistance, (XtArgVal) 12);	nargs++;
   }
 yes = XtCreateManagedWidget("yes", commandWidgetClass, form, args, nargs);

 nargs = 1;
 XtSetArg(args[nargs], XtNfromHoriz, (XtArgVal) yes);		nargs++;
 if (prompt) {
    XtSetArg(args[nargs], XtNfromVert, (XtArgVal) label);	nargs++;
    XtSetArg(args[nargs], XtNvertDistance, (XtArgVal) 12);	nargs++;
   }
 no = XtCreateManagedWidget("no", commandWidgetClass, form, args, nargs);

 if (! label)
    labelwidth = 0;
 else {
    XtSetArg(args[0], XtNwidth, (XtArgVal) NULL); 
    XtGetValues(label, args, ONE);
    labelwidth = args[0].value;
   }
    
 XtSetArg(args[0], XtNwidth, (XtArgVal) NULL); 
 XtSetArg(args[1], XtNborder, (XtArgVal) NULL); 
 XtGetValues(yes, args, TWO);

 yeswidth = args[0].value;
 btnborder = args[1].value;

 btnwidth = (labelwidth - btnsepr - (2 * btnborder)) / 2;
 if (btnwidth < yeswidth) btnwidth = yeswidth;

 XtSetArg(args[0], XtNwidth, (XtArgVal) btnwidth); 
 XtSetValues(yes, args, ONE);
 XtSetValues(no, args, ONE);

 XtRealizeWidget(shell);

 XtSetArg(args[0], XtNwidth, (XtArgVal) NULL); 
 XtSetArg(args[1], XtNheight, (XtArgVal) NULL); 
 XtGetValues(shell, args, TWO);

 root_x -= args[0].value / 2;
 root_y -= args[1].value / 2;
/*
** Keep confirm popup within root window borders (don't place it off-screen)
*/
 if (root_x + args[0].value > RootWidth)
     root_x = RootWidth - args[0].value;
 if (root_x < 0) root_x = 0;

 if (root_y + args[0].value > RootHeight)
     root_y = RootHeight - args[0].value;
 if (root_y < 0) root_y = 0;

 XtSetArg(args[0], XtNx, (XtArgVal) root_x); 
 XtSetArg(args[1], XtNy, (XtArgVal) root_y); 
 XtSetValues(shell, args, TWO);

 resultrec = MakeResult(shell, &popped_up, &result);
 XtAddCallback(yes, XtNcallback, Yes, resultrec);
 XtAddCallback(no, XtNcallback, No, resultrec);

 if (XMail.bellRing)			/* ring bell if not silenced by user */
    XBell (XtDisplay (toplevel), 33);

 XtPopup(shell, XtGrabExclusive);

 popped_up = TRUE;

 while (popped_up) ProcessOneEvent(XtDisplay(shell));

 return result;
} /* Confirm */
