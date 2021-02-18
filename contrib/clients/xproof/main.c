/*
 * Copyright 1991 University of Wisconsin-Madison
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of Wisconsin-Madison not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  The University of
 * Wisconsin-Madison makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF WISCONSIN-MADISON DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF WISCONSIN-MADISON BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:   Marvin Solomon
 *           Computer Sciences Department
 *           University of Wisconsin, Madison WI
 *           solomon@cs.wisc.edu or seismo!uwvax!solomon
 *
 * Current Maintainer:
 *
 *           Tim Theisen           Systems Programmer
 * Internet: tim@cs.wisc.edu       Department of Computer Sciences
 *     UUCP: uwvax!tim             University of Wisconsin-Madison
 *    Phone: (608)262-0438         1210 West Dayton Street
 *      FAX: (608)262-9777         Madison, WI   53706
 */
#include <stdio.h>
#include <ctype.h>
#include "xproof.h"
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/Sme.h>
#include <X11/Xaw/SmeBSB.h>
#include <assert.h>
#include <sys/stat.h>

/* for lint */
#ifdef ultrix
void exit();
#endif

#ifndef lint
static char *rcsid="$Header: /src/X11/uw/xproof/RCS/main.c,v 3.12 1991/10/04 22:05:44 tim Exp $";
#endif

/* The following are defined in version.c */
extern int MajorVersion, MinorVersion, PatchLevel;

static char *copyright = "Copyright 1988, 1989 by Marvin Solomon";

int printf();
extern WidgetClass
	proofWidgetClass,
	viewportWidgetClass,
	boxWidgetClass,
	labelWidgetClass,
	commandWidgetClass,
	panedWidgetClass;

static XrmOptionDescRec options[] = {
{"-file",        "*proof.file",       XrmoptionSepArg, NULL},
{"-idle_cursor", "*proof.idleCursor", XrmoptionSepArg, NULL},
{"-busy_cursor", "*proof.busyCursor", XrmoptionSepArg, NULL},
{"-scale",       "*proof.scale",      XrmoptionSepArg, NULL},
{"-usepixmap",   "*proof.usePixmap",  XrmoptionNoArg,  "true"},
{"+usepixmap",   "*proof.usePixmap",  XrmoptionNoArg,  "false"},
};

FILE *proofFile;
char *fileName;
int npages;       /* 0 means the input is coming from stdin */
int current_page; /* 0..npages-1 */

static  struct stat fstatbuf;           /* mechanism to see if file was */
time_t  proof_time;                     /* modified since last usage */

/* the window where the text is displayed */
ProofWidget proof_widget;

/* buttons for the menu bar */
Widget forward_button, back_button, quit_button, refresh_button, 
       larger_button, smaller_button;

/* resource for pulldown menu */
String fallback_resources[] = {
	"*menuButton.label:	Go To Page...",
	"*menu.label:		PAGE",
	"*menuLabel.vertSpace:	100",
	"*blank.height:		20",
	NULL,
};

usage(prog)
char *prog;
{
	(void)fprintf(stderr,"usage: %s [options] [file]\n",prog);
	exit(1);
}

Arg viewportArgs[] = {
	{ XtNallowHoriz, (XtArgVal)True },
	{ XtNallowVert, (XtArgVal)True },
};

Arg menuArgs[] = {
	{ XtNskipAdjust, (XtArgVal)True },
};

Arg menuLabelArgs[] = {
	{ XtNborderWidth, (XtArgVal)0 },
};

/* Callback procedures */
static void forward_page(), refresh_page(), go_to_page(), giveup(), change_scale();

main(argc,argv)
char **argv;
{
	Widget
		toplevel,           /* the shell that contains everything */
			paned,         /* two panes as follows: */
				menu_bar,   /* the menu bar at the top (a box widget) */
					/* forward_button, back_button, quit_button,
					 * popup_button, larger_button, smaller_button
					 */
				viewport    /* viewport for scrolling around in ...*/
					/*proof_widget */;  /* the proof window itself */
	XtAppContext app_con;
	Dimension width, height;
	Dimension horiz_margin, vert_margin;
	Dimension screen_width, screen_height;
	Arg args[2];
	XSizeHints hints;
	long supplied;
	int n;

	setbuf(stdout, (char *)0); /* for debugging */

	toplevel = XtAppInitialize(&app_con, "Proof", options, XtNumber(options),
			     &argc, argv, fallback_resources, NULL, ZERO);
	if (argc > 1 && strcmp(argv[1],"-V")==0) {
		(void) fprintf(stderr, "Xproof  %s Version %d.%d, Patch level %d\n",
			copyright, MajorVersion, MinorVersion, PatchLevel);
		exit(0);
	}

	/* Open the input file */
	if (argc > 2) usage(argv[0]);

	if (argc == 2) {
		fileName = argv[1];
		proofFile = fopen(fileName, "r");
		if (proofFile == NULL) {
			perror(fileName);
			exit(1);
		}
		(void) fstat(fileno(proofFile), &fstatbuf);
		proof_time = fstatbuf.st_mtime;
		npages = makeIndex(proofFile);
	}
	else {
		fileName = "Standard Input";
		proofFile = stdin;
		npages = 0;
	}
	/* Read stuff up to start of the first page to "prime the pump". 
	 * We do this here to learn about things like device resolution, etc.
	 * before deciding on a default window size.  However, since we're
	 * doing this before the rest of initialization, there better
	 * not be any commands that actully display anything!
	 */
	if (n = PrimePump((ProofWidget)toplevel, proofFile)) {
		(void) fprintf(stderr, "%s: %s\n", fileName,
			n == -1 ? "file is empty"
			: n == -2 ? "file not in troff output format"
			: "unrecoverable I/O error");
		exit(1);
	}

	paned = XtCreateManagedWidget("paned", panedWidgetClass,
		toplevel,
		(ArgList)0, ZERO);

	menu_bar = XtCreateManagedWidget("menu bar", boxWidgetClass,
		paned,
		menuArgs, XtNumber(menuArgs));

	viewport = XtCreateManagedWidget("viewport", viewportWidgetClass,
		paned,
		viewportArgs, XtNumber(viewportArgs));

	proof_widget = (ProofWidget)XtCreateManagedWidget("proof", proofWidgetClass,
		viewport,
		(ArgList)0, ZERO);

	/* label for the menu */
	(void) XtCreateManagedWidget(fileName, labelWidgetClass,
		menu_bar,
		menuLabelArgs, XtNumber(menuLabelArgs));

	/* buttons for the menu (add these after the proof widget is created) */

	back_button = XtCreateManagedWidget("back", commandWidgetClass,
		menu_bar, (ArgList)0, ZERO);
	XtAddCallback(back_button, XtNcallback, go_to_page, (caddr_t)-1);
	make_sensitive(back_button, FALSE);

	forward_button = XtCreateManagedWidget("forward", commandWidgetClass,
		menu_bar, (ArgList)0, ZERO);
	XtAddCallback(forward_button, XtNcallback, forward_page, (caddr_t)0);

	quit_button = XtCreateManagedWidget("quit", commandWidgetClass,
		menu_bar, (ArgList)0, ZERO);
	XtAddCallback(quit_button, XtNcallback, giveup, (caddr_t)0);

	if (npages) {
		int i;
		char buf[100];
		Widget entry;
		Widget popupbutton =
			XtCreateManagedWidget( "menuButton", menuButtonWidgetClass,
				menu_bar, NULL, ZERO );
		Widget menu = XtCreatePopupShell("menu", simpleMenuWidgetClass,
			popupbutton, NULL, ZERO);

		for (i=0; i<npages; i++) {
			(void)sprintf(buf,"Page %d",pageNumber(i));
			entry = XtCreateManagedWidget(buf, smeBSBObjectClass,
				menu, NULL, ZERO);
			XtAddCallback(entry, XtNcallback, go_to_page, (caddr_t)i);
		}
		refresh_button = XtCreateManagedWidget("refresh",
			commandWidgetClass, menu_bar, NULL, ZERO);
		XtAddCallback(refresh_button, XtNcallback, refresh_page, (caddr_t)0);
	}

	larger_button = XtCreateManagedWidget("magnify", commandWidgetClass,
		menu_bar, (ArgList)0, ZERO);
	XtAddCallback(larger_button, XtNcallback, change_scale, (caddr_t)1);

	smaller_button = XtCreateManagedWidget("shrink", commandWidgetClass,
		menu_bar, (ArgList)0, ZERO);
	XtAddCallback(smaller_button, XtNcallback, change_scale, (caddr_t)-1);

	XtProofSetFile(proof_widget, proofFile);

	current_page = 0;
	/* There should be a command-line option to choose which page to start on.
	 */

	/* Double check the size of the window.
	 * Reduce it if it is larger than the screen.
	 */
	XtSetMappedWhenManaged(toplevel, False);
	XtRealizeWidget(toplevel);

	XGetWMNormalHints(XtDisplay(toplevel), XtWindow(toplevel), &hints,
			  &supplied);
	if (!(hints.flags & USSize)) {
	    XtSetArg(args[0], XtNheight, &height);
	    XtSetArg(args[1], XtNwidth, &width);
	    XtGetValues(toplevel, args, TWO);

	    horiz_margin = 20;
	    vert_margin = 40;
	    screen_width = WidthOfScreen(XtScreen(toplevel)) - horiz_margin;
	    screen_height = HeightOfScreen(XtScreen(toplevel)) - vert_margin;

	    if (width > screen_width) {
		XtSetArg(args[0], XtNwidth, screen_width);
		XtSetValues(toplevel, args, ONE);
	    }
	    if (height > screen_height) {
		XtSetArg(args[0], XtNheight, screen_height);
		XtSetValues(toplevel, args, ONE);
	    }
	}
	XtMapWidget(toplevel);

	XtAppMainLoop(app_con);
	exit(0);
}

/* ARGSUSED */
static void refresh_page(button, dummy1, dummy2)
Widget button;
int dummy1, dummy2;
{
	go_to_page(button, current_page, dummy2);
}

/* ARGSUSED */
static void forward_page(button, dummy1, dummy2)
Widget button;
int dummy1, dummy2;
{
	if (npages) {
		if (current_page >= npages-1) {/* can't happen! */
			(void)printf("current page %d, npages %d !!\n",
				current_page, npages);
			(void)fflush(stdout);
		}
		current_page++;
		if (fstat(fileno(proofFile), &fstatbuf) != 0 ||
		    fstatbuf.st_mtime != proof_time) {
			fclose(proofFile);
			proofFile = fopen(fileName, "r");
			if (proofFile == NULL) {
				perror(fileName);
				exit(1);
			}
			(void) fstat(fileno(proofFile), &fstatbuf);
			proof_time = fstatbuf.st_mtime;
			XtProofSetFile(proof_widget, proofFile);
			npages = makeIndex(proofFile);
			if (current_page > npages-1) current_page = npages-1;
			XtProofShowPage(proof_widget, seekPointer(current_page));
		} else {
			XtProofShowPage(proof_widget, -1L);
		}
		make_sensitive(back_button, TRUE);
		if (current_page == npages-1)
			make_sensitive(forward_button, FALSE);
	}
	else
		XtProofShowPage(proof_widget, -1L);
}

/* go to page n (0..npages-1).  If n==-1, go back one page */
/* ARGSUSED */
static void go_to_page(button, page, dummy)
Widget button;
int page, dummy;
{
	if (npages) {
		if (page < 0) current_page--;
		else current_page = page;
		if (fstat(fileno(proofFile), &fstatbuf) != 0 ||
		    fstatbuf.st_mtime != proof_time) {
			fclose(proofFile);
			proofFile = fopen(fileName, "r");
			if (proofFile == NULL) {
				perror(fileName);
				exit(1);
			}
			(void) fstat(fileno(proofFile), &fstatbuf);
			proof_time = fstatbuf.st_mtime;
			XtProofSetFile(proof_widget, proofFile);
			npages = makeIndex(proofFile);
		}
		if (current_page > npages-1) current_page = npages-1;
		XtProofShowPage(proof_widget, seekPointer(current_page));
		make_sensitive(forward_button, current_page < npages-1);
		make_sensitive(back_button, current_page > 0);
	}
	else {
		(void)printf("go_to_page %d with no page table?\n",page);
		(void)fflush(stdout);
	}
}

/* Change the scale.  Eventually, we'll fix this to go to the next
 * "reasonable" scale based on available fonts, but for now we just go up
 * or down by a factor of 1.2 and let fonts take care of themselves!
 */
/* ARGSUSED */
static void change_scale(button, delta, dummy)
Widget button;
int delta, dummy;
{
	XtProofSetScale(proof_widget, 12, delta);
}

make_sensitive(button, option)
Widget button;
Boolean option;
{
	Arg arg;

	XtSetArg(arg, XtNsensitive, option);
	XtSetValues(button, &arg, ONE);
}

/* ARGSUSED */
static void giveup(button, dummy1, dummy2)
Widget button;
int dummy1, dummy2;
{
	exit(0);
}

/* The proof widget catches button presses, but doesn't have the wherewithal
 * to do the right thing.  Thus, we export the following function which is
 * called back from the proof widget.  This really should either be a resource
 * or a callback, but I'm too lazy to fix it right now.
 */

/* Change the current page according to 'offset' and 'relative'.
 * If 'relative' is True, move forward 'offset' pages (back if offset<0).
 * (INFINITY => move to last page, -INFINITY => move to first page).
 *
 * If 'relative' is False, try to find a nearby page numbered 'offset',
 * searching forward circularly from the current page.  In this case, page
 * numbers are always positive; 0 means the first page, and -1 means the last
 * page.
 */
void SetPage(w, offset, relative)
Widget w;
int offset;
Boolean relative;
{
	int newpage;
#	define BAD { BEEP(w); return; }

	if (relative) {
		newpage = current_page + offset;
		if (offset < 0) {
			/* backward relative move */
			if (npages == 0) BAD /* can't back up */
			if (newpage < 0) newpage = 0;
			go_to_page(w, newpage, 0);
		}
		else {
			/* forward relative move */
			if (npages == 0) {
				if (offset != 1) BAD /* only one page forward at a time */
				forward_page(w, 0, 0);
			}
			else {
				if (newpage >= npages) newpage = npages-1;
				go_to_page(w, newpage, 0);
			}
		}
	}
	else {
		/* absolute goto */
		if (npages == 0) BAD /* no abs goto from stdin */
		newpage =
			offset == 0 ? 0
			: offset == -1 ? npages - 1
			: findPage(current_page, offset);
		if (newpage < 0) BAD
		go_to_page(w, newpage, 0);
	}
}
