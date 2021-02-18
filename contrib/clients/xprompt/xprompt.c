static char *SCCS_Id = "@(#)xprompt.c\tver 1.4 (91/09/28 14:23:33) brachman@cs.ubc.ca";

/* vi: set tabstop=4 : */

/*
 * xprompt - prompt the user for one or more replies
 *
 *
 * 28-Sep-91  Convert to X11R5, retain X11R4, lose X11R3
 *            Casey Leedom <casey@gauss.llnl.gov>
 *
 * 28-Jan-90  Convert to X11R4, retain X11R3 compatibility
 *
 * 11-Mar-89  Fixed problem with XSetInputFocus being done before windows
 *            were viewable.
 *
 * 24-Jan-89  Written for X11R3.  Released for distribution.
 *
 *
 * Copyright 1989, 1990, 1991 by
 * Barry Brachman and the University of British Columbia
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of U.B.C. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  U.B.C. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * -----
 * Barry Brachman           | {alberta,uw-beaver,uunet}!ubc-cs!brachman
 * Dept. of Computer Science| brachman@cs.ubc.ca (cs.ubc.ca=137.82.8.5)
 * Univ. of British Columbia| brachman@ubc.csnet
 * Vancouver, B.C. V6T 1Z2  | Office: (604) 822-3123, FAX: (604) 822-5485
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/AsciiText.h>

#include "trexp.h"

#define maximum(x, y)		((x) > (y) ? (x) : (y))

#define GEOM_WIDTH_PAD		25		/* Extra padding for window width */
#define GEOM_HEIGHT_PAD		15		/* Same for height */

static XrmOptionDescRec table[] = {
    {"-grab",   "Grab",                XrmoptionNoArg,  (caddr_t) "on"},
    {"-ibw",    "insideborderWidth",   XrmoptionSepArg, NULL},
    {"-nograb", "Grab",                XrmoptionNoArg,  (caddr_t) "off"},
    {"-nowarp", "Warp",                XrmoptionNoArg,  (caddr_t) "off"},
	{"-nre",    "returnExit",          XrmoptionNoArg,  (caddr_t) "off"},
	{"-p",      "",                    XrmoptionSkipLine, NULL},
    {"-pfn",    "promptFont",          XrmoptionSepArg, NULL},
	{"-re",     "returnExit",          XrmoptionNoArg,  (caddr_t) "on"},
    {"-rfn",    "replyFont",           XrmoptionSepArg, NULL},
    {"-rlen",   "Rlen",                XrmoptionSepArg, NULL},
    {"-tf",     "textTranslationFile", XrmoptionSepArg, NULL},
    {"-w",      "wordChars",           XrmoptionSepArg, NULL},
    {"-warp",   "Warp",                XrmoptionNoArg, (caddr_t) "on"},
};

typedef struct {
    int rlen;				/* maximum reply length */
    Boolean grab;			/* If TRUE, grab the keyboard */
	Boolean warp;			/* If TRUE, warp to reply window */
	Boolean return_exit;	/* If TRUE, exit on <return> if single prompt */
	char *texttranslations;
	char *texttranslationfile;
	char *wordchars;
    char *geometry;
    int borderwidth;
    int insideborderwidth;
    XFontStruct *font;
    XFontStruct *pfont;
    XFontStruct *rfont;
} app_resourceRec, *app_res;

static app_resourceRec app_resources;

static XtResource resources[] = {
{"rlen",  "Rlen", XtRInt, sizeof(int),
   XtOffset(app_res, rlen), XtRImmediate, (caddr_t) 80},
{"grab",  "Grab", XtRBoolean, sizeof(Boolean),
   XtOffset(app_res, grab), XtRImmediate, (caddr_t) TRUE },
{"insideborderwidth", "insideborderWidth", XtRInt, sizeof(int),
   XtOffset(app_res, insideborderwidth), XtRImmediate, (caddr_t) 1},
{"replyfont", "replyFont", XtRFontStruct, sizeof(XFontStruct *),
   XtOffset(app_res, rfont), XtRString, NULL},
{"promptfont", "promptFont", XtRFontStruct, sizeof(XFontStruct *),
   XtOffset(app_res, pfont), XtRString, NULL},
{"returnexit", "returnExit", XtRBoolean, sizeof(Boolean),
   XtOffset(app_res, return_exit), XtRImmediate, (caddr_t) FALSE},
{"texttranslationfile", "textTranslationFile", XtRString, sizeof(caddr_t),
   XtOffset(app_res, texttranslationfile), XtRString, NULL},
{"texttranslations", "textTranslations", XtRString, sizeof(caddr_t),
   XtOffset(app_res, texttranslations), XtRString, NULL},
{"wordchars", "wordChars", XtRString, sizeof(caddr_t),
   XtOffset(app_res, wordchars), XtRString, "a-zA-Z0-9"},
{"warp",  "Warp", XtRBoolean, sizeof(Boolean),
   XtOffset(app_res, warp), XtRImmediate, (caddr_t) FALSE },

{XtNgeometry,  "Geometry", XtRString, sizeof(caddr_t),
   XtOffset(app_res, geometry), XtRString, (caddr_t) "+500+400"},
{XtNborderWidth, "borderWidth", XtRInt, sizeof(int),
   XtOffset(app_res, borderwidth), XtRImmediate, (caddr_t) 1},
{XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
   XtOffset(app_res, font), XtRString, (caddr_t) XtDefaultFont},
};

static char xprompt_TextTranslations[] =
"\
Ctrl<Key>C:		    abort() \n\
Ctrl<Key>D:		    finish-prompt() \n\
Ctrl<Key>J:         next-prompt() \n\
Ctrl<Key>M:         next-prompt() \n\
<Key>Down:		    next-prompt() \n\
<Key>Up:		    previous-prompt() \n\
<Key>Linefeed:	    next-prompt() \n\
<Key>Return:	    next-prompt-or-finish() \n\
Ctrl<Key>U:         erase-line() \n\
<Btn1Up>:           finish-prompt() \n\
<Btn2Up>:           finish-prompt() \n\
<Btn3Up>:           finish-prompt() \n\
<Btn1Down>:         ignore() \n\
<Btn2Down>:         ignore() \n\
<Btn3Down>:         ignore() \n\
<Btn1Motion>:       ignore() \n\
<Btn2Motion>:       ignore() \n\
<Btn3Motion>:       ignore() \n\
<VisibilityNotify>: visibility-event() \n\
";

static struct promptargs {
	char *prompt;
	char *reply;
    Boolean seenprompt;
	struct promptargs *next;
	struct promptargs *prev;
} *promptargs, *npa, *npa_prev;
static int cpromptarg, npromptargs, nprompts_seen;

static char *promptbuf, *replybuf;
static Widget toplevel, box, popup, reply_w, prompt_w;
static Widget complete_top_w, complete_w;
static Window orig_win;
static int orig_x, orig_y;
static int reply_x, reply_y;
static int prompt_bb_width, prompt_bb_height;
static int reply_bb_width, reply_bb_height;
static int visible;

static trexp *word_tr;
static void changeprompt(), null_cursor(), string_changed();
static void unparsegeometry(), unwarp();
static void Syntax();
static int get_user_text_translations();

char *malloc();

/*ARGSUSED*/
static void
EraseLine(ctx, event, args, nargs)
TextWidget ctx;
XEvent event;
String *args;
Cardinal *nargs;
{

	replybuf[0] = '\0';
	string_changed(reply_w, replybuf, 0);
	XawTextSetInsertionPoint(reply_w, 0);
	XawTextDisplay(reply_w);
}

/*ARGSUSED*/
static void
EraseWord(ctx, event, args, nargs)
TextWidget ctx;
XEvent event;
String *args;
Cardinal *nargs;
{
	XawTextPosition endpos, pos, startpos;

	startpos = pos = XawTextGetInsertionPoint(reply_w);

	/* Skip any leading "non-word" characters */
	while (replybuf[pos] != '\0' && !trexec(word_tr, replybuf[pos]))
		pos++;
	/* Skip any "word" characters */
	while (trexec(word_tr, replybuf[pos]))
		pos++;

	endpos = pos;
	pos = startpos;
	while (replybuf[endpos] != '\0')
		replybuf[pos++] = replybuf[endpos++];
	replybuf[pos] = '\0';

	string_changed(reply_w, replybuf, pos);
	XawTextSetInsertionPoint(reply_w, startpos);
	XawTextDisplay(reply_w);
}

/*ARGSUSED*/
static void
FinishPrompt(w, event, args, nargs)
Widget w;
XEvent event;
String *args;
Cardinal *nargs;
{
	int i;

	strcpy(npa->reply, replybuf);
	for (i = 0, npa = promptargs; i < npromptargs; npa = npa->next, i++)
		printf("%s\n", npa->reply);
	unwarp();
	XtDestroyWidget(toplevel);
	exit(0);
}

/*ARGSUSED*/
static void
PreviousPrompt(w, event, args, nargs)
Widget w;
XEvent event;
String *args;
Cardinal *nargs;
{

	npa_prev = npa;
	npa = npa->prev;
	if (--cpromptarg == 0)
		cpromptarg = npromptargs;
	changeprompt();
}

/*ARGSUSED*/
static void
NextPrompt(w, event, args, nargs)
Widget w;
XEvent event;
String *args;
Cardinal *nargs;
{

	if (npromptargs > 1) {
		npa_prev = npa;
		npa = npa->next;
		if (++cpromptarg > npromptargs)
			cpromptarg = 1;
		changeprompt();
	}
}

/*ARGSUSED*/
static void
NextPromptOrFinish(w, event, args, nargs)
Widget w;
XEvent event;
String *args;
Cardinal *nargs;
{

	if (nprompts_seen == npromptargs && app_resources.return_exit) {
		FinishPrompt(w, event, args, nargs);
		/*NOTREACHED*/
	}
	NextPrompt(w, event, args, nargs);
}

/*ARGSUSED*/
static void
changeprompt()
{
	int replylen, promptlen;

	if (npromptargs > 1)
		sprintf(promptbuf, "%s[%d/%d]:", npa->prompt, cpromptarg, npromptargs);
	else
		sprintf(promptbuf, "%s:", npa->prompt);
	strcpy(npa_prev->reply, replybuf);
	strcpy(replybuf, npa->reply);
	if (npa->seenprompt == FALSE) {
		npa->seenprompt = TRUE;
		nprompts_seen++;
	}

	replylen = strlen(replybuf);
	promptlen = strlen(promptbuf);
	string_changed(reply_w, replybuf, replylen);
	string_changed(prompt_w, promptbuf, promptlen);

	XawTextSetInsertionPoint(reply_w, replylen);
	XawTextSetInsertionPoint(prompt_w, promptlen);

	XawTextDisplay(reply_w);
	XawTextDisplay(prompt_w);
}

/*ARGSUSED*/
static void
Abort(w, event, args, nargs)
Widget w;
XEvent event;
String *args;
Cardinal *nargs;
{

	unwarp();
    XtDestroyWidget(toplevel);
    exit(1);
}

/*ARGSUSED*/
static void
Ignore(w, event, args, nargs)
Widget w;
XEvent event;
String *args;
Cardinal *nargs;
{

}

/*ARGSUSED*/
static void
VisibilityEvent(w, event, args, nargs)
Widget w;
XEvent event;
String *args;
Cardinal *nargs;
{

	visible = 1;
}

static XtActionsRec xprompt_actions[] = {
  {"erase-line",		    EraseLine },
  {"erase-word",            EraseWord },
  {"next-prompt",		    NextPrompt },
  {"previous-prompt",	    PreviousPrompt },
  {"finish-prompt",		    FinishPrompt },
  {"next-prompt-or-finish", NextPromptOrFinish },
  {"abort",				    Abort },
  {"ignore",                Ignore },
  {"visibility-event",      VisibilityEvent },
};

static void
Syntax(call)
char *call;
{

	fprintf(stderr, "%s\n", SCCS_Id);
    fprintf(stderr, "Usage: %s [flags] [xtoolkitargs] -p prompt [-r reply] \
[-p prompt [-r reply]] ...\n", call);
    fprintf(stderr, "where <flags> is one or more of:\n");
fprintf(stderr, "-rlen #     (Maximum length of user's reply: default 80)\n");
fprintf(stderr, "-ibw #      (Border width of inside window: default 1)\n");
fprintf(stderr, "-grab       (Grab keyboard: default)\n");
fprintf(stderr, "-nograb     (Don't grab keyboard)\n");
fprintf(stderr, "-re         (Return key will exit if there's one prompt)\n");
fprintf(stderr, "-nre        (Return key won't exit: default)\n");
fprintf(stderr, "-pfn <font> (Prompt font)\n");
fprintf(stderr, "-rfn <font> (Reply font)\n");
fprintf(stderr, "-tf <file>  (Translation file to override defaults)\n");
fprintf(stderr, "-w <str>    (Characters making up a word\n");
fprintf(stderr, "-warp       (Warp cursor to reply window)\n");
fprintf(stderr, "-nowarp     (Don't warp cursor)\n");
    exit(1);
}

void unparsegeometry();

main(argc, argv)
unsigned int argc;
char **argv;
{
    register int i, j;
	int len, maxpromptlen;
    Arg arg[11];
    int geom_mask, geom_x, geom_y, geom_width, geom_height;
    char geom_str[100];
	XtTranslations t;
    XFontStruct *prompt_font, *reply_font;

    toplevel = XtInitialize(argv[0], "XPrompt", table, XtNumber(table),
							&argc, argv);

    XtGetApplicationResources(toplevel, &app_resources, resources,
							  XtNumber(resources), NULL, (Cardinal) 0);

    j = 0;
	promptargs = NULL;
	maxpromptlen = 0;
    for (i = 1; i < argc; i++) {
		if (i == argc - 1 || strcmp(argv[i], "-p"))
			Syntax(argv[0]);
		if (promptargs) {
			npa->next =
				(struct promptargs *) malloc(sizeof(struct promptargs));
			npa->next->prev = npa;
			npa = npa->next;
			npa->next = NULL;
		}
		else {
			promptargs = npa =
				(struct promptargs *) malloc(sizeof(struct promptargs));
			npa->prev = npa->next = NULL;
		}
		npa->prompt = argv[++i];
		if ((len = strlen(npa->prompt)) > maxpromptlen)
			maxpromptlen = len;
		if ((npa->reply = (char *) malloc(app_resources.rlen + 1)) == NULL) {
			fprintf(stderr, "Can't alloc reply buffer\n");
			exit(1);
		}
		npa->reply[0] = '\0';
		if (argv[i+1] && !strcmp(argv[i+1], "-r")) {
			if (++i == argc - 1)
				Syntax(argv[0]);
			if (strlen(argv[++i]) > app_resources.rlen) {
				fprintf(stderr, "xprompt: default reply is too long\n");
				exit(1);
			}
			strcpy(npa->reply, argv[i]);
		}
		npa->seenprompt = FALSE;
		j++;
	}
	if ((npromptargs = j) == 0)
		Syntax(argv[0]);
	promptargs->prev = npa;
	npa->next = promptargs;
	npa = promptargs;
	cpromptarg = 1;
	replybuf = (char *) malloc(app_resources.rlen + 1);
	if (npromptargs > 1) {
		maxpromptlen += 5;
		if (npromptargs < 10)
			maxpromptlen += 2;
		else if (npromptargs < 100)
			maxpromptlen += 4;
		else
			maxpromptlen += 10;		/* yuk */
	}
	else
		maxpromptlen += 2;
	promptbuf = (char *) malloc(maxpromptlen);

    if (app_resources.pfont == NULL)
		app_resources.pfont = app_resources.font;
    if (app_resources.rfont == NULL)
		app_resources.rfont = app_resources.font;

    XtAddActions(xprompt_actions, XtNumber(xprompt_actions));

	if ((word_tr = trcomp(app_resources.wordchars)) == NULL) {
		fprintf(stderr, "xprompt: Parse of word chars failed.\n");
		exit(1);
	}
	
	/*
	 * Determine the window geometry
	 */
    prompt_font = app_resources.pfont;
    prompt_bb_width = prompt_font->max_bounds.rbearing
		- prompt_font->min_bounds.lbearing;
    prompt_bb_height = prompt_font->max_bounds.ascent
		+ prompt_font->max_bounds.descent;
    reply_font = app_resources.rfont;
    reply_bb_width = reply_font->max_bounds.rbearing
		- reply_font->min_bounds.lbearing;
    reply_bb_height = reply_font->max_bounds.ascent
		+ reply_font->max_bounds.descent;

    geom_x = 500;				/* an arbitrary default location */
    geom_y = 400;
    geom_width = maxpromptlen * prompt_bb_width
		+ app_resources.rlen * reply_bb_width + GEOM_WIDTH_PAD;
    geom_height = maximum(prompt_bb_height, reply_bb_height) + GEOM_HEIGHT_PAD;
    geom_mask =
		XParseGeometry(app_resources.geometry, &geom_x, &geom_y, &geom_width,
			&geom_height);
	if (geom_mask & WidthValue)
		geom_width *= reply_bb_width;
	if (geom_mask & HeightValue)
		geom_height *= reply_bb_height;
    unparsegeometry(geom_str, geom_mask,
					geom_width, geom_height,
					geom_x, geom_y);

#ifdef DEBUG
	fprintf(stderr, "prompt_bb_width=%d prompt_bb_height=%d\n",
			prompt_bb_width, prompt_bb_height);
	fprintf(stderr, "reply_bb_width=%d reply_bb_height=%d\n",
			reply_bb_width, reply_bb_height);
	fprintf(stderr, "geom_width=%d geom_height=%d\n",
			maxpromptlen * prompt_bb_width
			+ app_resources.rlen * reply_bb_width + GEOM_WIDTH_PAD,
			maximum(prompt_bb_height, reply_bb_height) + GEOM_HEIGHT_PAD);
	fprintf(stderr, "geom_mask=%d geom_width=%d geom_height=%d\n",
			geom_mask, geom_width, geom_height);
	fprintf(stderr, "%s\n", geom_str);
#endif DEBUG

    XtSetArg(arg[0], XtNgeometry, geom_str);
    XtSetArg(arg[1], XtNborderWidth, app_resources.borderwidth);
	/*
	 * If you prefer a popup window instead of a managed window, you
	 * can use the overrideShellWidgetClass instead of the
	 * topLevelShellWidgetClass
	 *    popup = XtCreatePopupShell("popup", overrideShellWidgetClass,
	 *			toplevel, arg, (Cardinal) 2);
	 */
	popup = XtCreatePopupShell("xprompt", topLevelShellWidgetClass,
							   toplevel, arg, (Cardinal) 2);

	box =
		XtCreateManagedWidget("box", boxWidgetClass,
							  popup, NULL, (Cardinal) 0);

	if (npromptargs > 1)
		sprintf(promptbuf, "%s[1/%d]:", promptargs->prompt, npromptargs);
	else
		sprintf(promptbuf, "%s:", promptargs->prompt);
	promptargs->seenprompt = TRUE;
	nprompts_seen = 1;
    XtSetArg(arg[0], XtNstring, promptbuf);
	XtSetArg(arg[1], XtNlength, maxpromptlen);
    XtSetArg(arg[2], XtNborderWidth, 0);
    XtSetArg(arg[3], XtNfont, app_resources.pfont);
	XtSetArg(arg[4], XtNeditType, XawtextRead);
	XtSetArg(arg[5], XtNwidth, prompt_bb_width * maxpromptlen);
	XtSetArg(arg[6], XtNsensitive, False);
	XtSetArg(arg[7], XtNuseStringInPlace, True);
	XtSetArg(arg[8], XtNdisplayCaret, False);
	XtSetArg(arg[9], XtNtype, XawAsciiString);
	prompt_w = XtCreateManagedWidget("text", asciiTextWidgetClass,
									 box, arg, (Cardinal) 10);

	strcpy(replybuf, promptargs->reply);
	XtSetArg(arg[0], XtNscrollVertical, XawtextScrollWhenNeeded);
    XtSetArg(arg[1], XtNheight, reply_bb_height + 6);
    XtSetArg(arg[2], XtNstring, replybuf);
    XtSetArg(arg[3], XtNlength, app_resources.rlen);
    XtSetArg(arg[4], XtNeditType, XawtextEdit);
    XtSetArg(arg[5], XtNborderWidth, app_resources.insideborderwidth);
    XtSetArg(arg[6], XtNwidth, app_resources.rlen * reply_bb_width);
    XtSetArg(arg[7], XtNinsertPosition, strlen(promptargs->reply));
    XtSetArg(arg[8], XtNfont, app_resources.rfont);
	XtSetArg(arg[9], XtNuseStringInPlace, True);
	XtSetArg(arg[10], XtNtype, XawAsciiString);
	reply_w = XtCreateManagedWidget("reply", asciiTextWidgetClass,
									box, arg, (Cardinal) 11);

	t = XtParseTranslationTable(xprompt_TextTranslations);
	XtOverrideTranslations(reply_w, t);
	if (app_resources.texttranslations != NULL) {
		t = XtParseTranslationTable(app_resources.texttranslations);
		if (t == NULL) {
			fprintf(stderr, "xprompt: error parsing translation table.\n");
			exit(1);
		}
		XtOverrideTranslations(reply_w, t);
	}
	if (app_resources.texttranslationfile != NULL
		&& get_user_text_translations(app_resources.texttranslationfile) < 0)
		exit(1);

	visible = 0;
    XtPopup(popup, XtGrabNonexclusive);

	/*
	 * Handle the grab and/or warp, if necessary
	 */
	if (app_resources.grab || app_resources.warp) {
		XWindowAttributes reply_w_attr;

		if (XGetWindowAttributes(XtDisplay(reply_w), XtWindow(reply_w),
								 &reply_w_attr) == 0)
			app_resources.warp = app_resources.grab = FALSE;
		else if (!visible) {
			XEvent event;
			long em;

			/*
			 * The following weirdness waits for the windows to become visible
			 * before warping the cursor or setting the input focus.
			 * This is necessary to avoid the cases where a window manager
			 * repositions things *after* the warp has occurred and where the
			 * focus window is not viewable at the time of the XSetInputFocus.
			 * Suggestions on the Right Thing welcome.
			 */
			em = VisibilityChangeMask;
			XSelectInput(XtDisplay(popup), XtWindow(popup), em);
			while (1) {
				bzero(&event, sizeof(event));
				XNextEvent(XtDisplay(popup), &event);
				if (event.type == VisibilityNotify)
					break;
			}
			em = reply_w_attr.all_event_masks;
			XSelectInput(XtDisplay(popup), XtWindow(popup), em);
		}

		if (app_resources.grab) {
			XGrabKeyboard(XtDisplay(reply_w), XtWindow(reply_w), False,
						  GrabModeAsync, GrabModeAsync, CurrentTime);
			XSetInputFocus(XtDisplay(reply_w), XtWindow(reply_w),
						   RevertToPointerRoot, CurrentTime);
		}

		if (app_resources.warp) {
			Window root_return, child_return;
			int root_x_return, root_y_return;
			int win_x_return, win_y_return;
			unsigned int mask_return;

			orig_x = -1;
			orig_y = -1;
			/*
			 * Find out where the cursor is so that it can be put back
			 * before the program exits.
			 */
			if (XQueryPointer(XtDisplay(popup), XtWindow(popup),
							  &root_return, &child_return,
							  &root_x_return, &root_y_return,
							  &win_x_return, &win_y_return,
							  &mask_return) != 0) {
				orig_win = root_return;
				orig_x = root_x_return;
				orig_y = root_y_return;
			}
			reply_x = reply_w_attr.width - reply_w_attr.width / 10;
			reply_y = reply_w_attr.height / 2;

			XWarpPointer(XtDisplay(reply_w), None, XtWindow(reply_w),
						 0, 0, 0, 0, reply_x, reply_y);
		}
	}

    XtMainLoop();
}

static int
get_user_text_translations(file)
char *file;
{
	char *p;
	FILE *fp;
	struct stat statb;
	XtTranslations t;

	if ((fp = fopen(file, "r")) == NULL) {
		fprintf(stderr, "xprompt: Can't open translation file '%s'.\n", file); 
		return(-1);
	}
	if (fstat(fileno(fp), &statb) < 0) {
		fprintf(stderr, "xprompt: Can't stat translation file '%s'.\n", file);
		fclose(fp);
		return(-1);
	}
	if (statb.st_size == 0) {
		fclose(fp);
		return(0);
	}
	if ((p = (char *) malloc((unsigned) statb.st_size)) == NULL) {
		fprintf(stderr, "xprompt: Can't malloc translation table.\n");
		fclose(fp);
		return(-1);
	}
	if (fread(p, statb.st_size, 1, fp) == 0) {
		fprintf(stderr, "xprompt: error reading translation table.\n");
		free(p);
		fclose(fp);
		return(-1);
	}
	fclose(fp);
	if ((t = XtParseTranslationTable(p)) == NULL) {
		fprintf(stderr, "xprompt: error parsing translation table.\n");
		free(p);
		return(-1);
	}
	XtOverrideTranslations(reply_w, t);
	free(p);
	return(0);
}

static void
unwarp()
{

	if (app_resources.warp && orig_x != -1 && orig_y != -1) {
		XWarpPointer(XtDisplay(reply_w), None, orig_win,
					 0, 0, 0, 0, orig_x, orig_y);
		XFlush(XtDisplay(reply_w));
	}
}

static void
unparsegeometry(buf, mask, w, h, x, y)
char *buf;
int mask, w, h, x, y;
{

	sprintf(buf, "%dx%d%c%d%c%d",
			w, h,
			mask & XNegative ? '-' : '+', x,
			mask & YNegative ? '-' : '+', y);
}

static void
string_changed(w, buf, pos)
Widget w;
char *buf;
XawTextPosition pos;
{
	Arg args[20];

	XtSetArg(args[0], XtNstring, buf);
	XtSetValues(w, args, (Cardinal) 1);
}
