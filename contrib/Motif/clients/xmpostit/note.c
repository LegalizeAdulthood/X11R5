#ifndef lint
static char	*RCSid = "$Header: /usr/src/local/bin/X11/xmpostit/RCS/note.c,v 1.15 91/09/27 11:29:45 dm Exp $";
#endif

/*
 * note.c - routines for handling notes.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94086
 * davy@riacs.edu
 *
 *
 * Copyright 1989, 1990 Genentech, Incorporated
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Genentech not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Genentech makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * GENENTECH DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL GENENTECH BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 * Author:  David Mischel - Genentech, Inc.
 *
 */
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/file.h>
#include <stdio.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <Xm/Command.h>
#include <X11/Shell.h>
#include <Xm/MwmUtil.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/RowColumn.h>
#include <Xm/MainW.h>
#include <Xm/MessageB.h>
#include <Xm/PushBG.h>
#include <Xm/SelectioB.h>
#include <Xm/ScrolledW.h>

#include "xmpostit.h"

static PostItNote	*notes = NULL;

static PostItNote	*FindNote();
static PostItNote	*AllocNote();

static void		NoteNameCB();
static void		NoteNameCancelCB();
static void		SaveNote();
static void		EraseNote();
static Widget	createEraseW();
static void		EraseCB();
static void		DestroyNote();
static void		DestroyCB();
static Widget	createDestroyW();
static void		MakeNoteWidget();
static void		MakeStashButtons();
static void		StashCB();
static void		HardCopyCB();

/*
 * CreateNewNote - create a new note of the specified size.
 */
void
CreateNewNote(size)
int size;
{
	static int hpi = 0;
	static int wpi = 0;
	register PostItNote *pn;
	register int hmm, wmm, hpixel, wpixel;

	/*
	 * Find out the number of pixels per inch on the screen.  We
	 * can get the number of pixels, and the size in millimeters.
	 * Then we convert to pixels/inch using the formula
	 *
	 *       2.54 cm     10 mm     pixels     pixels
	 *	--------- x ------- x -------- = --------
	 *	  inch        cm         mm        inch
	 *
	 * The only problem with this is that some servers (like Xsun)
	 * lie about what these numbers really are.
	 */
	if ((hpi == 0) || (wpi == 0)) {
		hpixel = DisplayHeight(display, DefaultScreen(display));
		wpixel = DisplayWidth(display, DefaultScreen(display));

		hmm = DisplayHeightMM(display, DefaultScreen(display));
		wmm = DisplayWidthMM(display, DefaultScreen(display));

		hpi = (int) ((25.4 * hpixel) / (float) hmm + 0.5);
		wpi = (int) ((25.4 * wpixel) / (float) wmm + 0.5);
	}

	/*
	 * Calculate sizes for the note.
	 */
	switch (size) {
	case PostItNote_1p5x2:
		hpixel = 1.5 * hpi;
		wpixel = 2 * wpi;
		break;
	case PostItNote_3x3:
		hpixel = 3 * hpi;
		wpixel = 3 * wpi;
		break;
	case PostItNote_3x5:
		hpixel = 3 * hpi;
		wpixel = 5 * wpi;
		break;
	}

	/* Allocate a new note structure.  */
	pn = AllocNote(NewIndex);

	/* Set the text window size.  */
	pn->pn_textwidth = wpixel;
	pn->pn_textheight = hpixel;

	/* set the name */
	GetNoteName(pn);  
}
 

GetNoteName(pn)
	PostItNote *pn;
{
	Arg args[5];
	int nargs;
	Widget twidget;

	nargs = 0;
	SetArg(XmNdialogType, XmDIALOG_PROMPT);
	SetArg(XmNselectionLabelString,
		XmStringCreateLtoR("Name of PostIt Note (optional)",
		XmSTRING_DEFAULT_CHARSET));
	SetArg(XmNtextString, 
		XmStringCreateLtoR("PostIt Note", XmSTRING_DEFAULT_CHARSET));
	SetArg(XmNdialogStyle, XmDIALOG_APPLICATION_MODAL);
	SetArg(XmNtextColumns, MAXNAME);
	twidget = XmCreatePromptDialog(rcwidget, "notename", args, nargs);
	XtAddCallback(twidget, XmNokCallback, NoteNameCB, pn);
	XtAddCallback(twidget, XmNcancelCallback, NoteNameCancelCB, pn);

    XtUnmanageChild
        (XmSelectionBoxGetChild (twidget, XmDIALOG_HELP_BUTTON));
	
	/* turn off mwm borders */
	nargs = 0;
	SetArg(XmNmwmDecorations, MWM_DECOR_ALL);
	XtSetValues(XtParent(twidget), args, nargs);
	/* limit input text size */
	nargs = 0;
	SetArg(XmNmaxLength, MAXNAME);
	XtSetValues(XmSelectionBoxGetChild(twidget, XmDIALOG_TEXT), args, nargs);

	XtRealizeWidget(twidget);
	XtManageChild(twidget);
}

/* ARGSUSED */
  static void
NoteNameCB(w, client_data, call_data)
	Widget w;
	caddr_t client_data, call_data;
{
	char tname[MAXNAME +1];
	PostItNote *pn;
	Widget textW;

	if ((pn = (PostItNote *) client_data) == NULL) return;

	textW = XmSelectionBoxGetChild(w, XmDIALOG_TEXT);
	strcpy(tname, XmTextGetString(textW));
	pn->pn_name = SafeAlloc(strlen(tname) +1);
	strcpy(pn->pn_name, tname);
	XtDestroyWidget(XtParent(w));

	/*
	 * Make the widget for the note.
	 */
	MakeNoteWidget(pn);
}


/* ARGSUSED */
  static void
NoteNameCancelCB(w, client_data, call_data)
	Widget w;
	caddr_t client_data, call_data;
{
	destroyNote((PostItNote *) client_data);
}


/*
 * LoadSavedNotes - load in the notes the user has saved.
 */
void
LoadSavedNotes()
{
	DIR *dp;
	FILE *fp;
	char *realloc();
	register PostItNote *pn;
	register struct direct *d;
	char buf[BUFSIZ], fname[MAXPATHLEN];
	char tname[MAXNAME+1];
	int n, len, nlen;
	Position shellx, shelly;
	Dimension shellw, shellh, texth, textw;
	int numnotes = 0;

	/*
	 * Try to open the directory.
	 */
	if ((dp = opendir(app_res.note_dir)) == NULL)
		return;

	nlen = strlen(PostItNoteFname);

	/*
	 * For each entry...
	 */
	while ((d = readdir(dp)) != NULL) {
		/*
		 * Skip over anything which doesn't match our
		 * file naming scheme.
		 */
		if (strncmp(d->d_name, PostItNoteFname, nlen) != 0)
			continue;

		/*
		 * Make the full path name.
		 */
		sprintf(fname, "%s/%s", app_res.note_dir, d->d_name);

		if ((fp = fopen(fname, "r")) == NULL)
			continue;

		/*
		 * Look for the magic cookie identifying this as
		 * a Post-It note.
		 */
		if ((fscanf(fp, "%s", buf) == EOF) ||
		    (strcmp(buf, PostItNoteMagic) != 0)) {
			fclose(fp);
			continue;
		}

		/*
		 * Get the note position and size information.
		 */
		fgets(buf, sizeof(buf), fp);

		n = sscanf(buf, "%d %d %d %d %d %d %d %[ !-~]", &shellx, &shelly,
			&shellw, &shellh, &texth, &textw, &len, tname);

		if (n == 5) {	/* oops, a note made by an early xpostit */
			sscanf(buf, "%d %d %d %d %d", &shellx, &shelly, &texth,
				&textw, &len);
			shellw = shellh = 64;		/* arbitrarily set things,  */
			if (!shellx) shellx = 10;	/* just to make them        */
			if (!shelly) shelly = 10;	/* look a bit better        */
			strcpy(tname, "PostIt Note");
		}
		else if (n == 6) {	/* a note made by xpostit v3 */
			sscanf(buf, "%d %d %d %d %d", &shellx, &shelly, &texth,
				&textw, &len, tname);
			shellw = shellh = 64;		/* arbitrarily set things,  */
			if (!shellx) shellx = 10;	/* just to make them        */
			if (!shelly) shelly = 10;	/* look a bit better        */
			if (!*tname) strcpy(tname, "PostIt Note");
		}
		else if (n != 8) {	/* Bad format; skip it.  */
			fclose(fp);
			continue;
		}

		/*
		 * Get the index number of this note.
		 */
		n = atoi(&(d->d_name[nlen]));

		/*
		 * Get a note structure.
		 */
		pn = AllocNote(n);

		/*
		 * Set the information.
		 */
		pn->pn_shellx = shellx;
		pn->pn_shelly = shelly;
		pn->pn_shellw = shellw;
		pn->pn_shellh = shellh;
		pn->pn_textwidth = textw;
		pn->pn_textheight = texth;
		pn->pn_mapit = False;		/* start as button */
		pn->pn_name = SafeAlloc(strlen(tname) +1);
		strcpy(pn->pn_name, tname);

		/*
		 * Save the file name.
		 */
		pn->pn_file = SafeAlloc(strlen(fname) + 1);
		strcpy(pn->pn_file, fname);

		/*
		 * If we need a bigger buffer than the default,
		 * get one.
		 */
		if (len >= pn->pn_textsize) {
			n = (len + app_res.buf_size - 1) / app_res.buf_size;
			n = n * app_res.buf_size;

			if ((pn->pn_text = realloc(pn->pn_text, n)) == NULL) {
				fprintf(stderr, "xmpostit: out of memory.\n");
				ByeBye();
			}

			pn->pn_textsize = n;
		}

		/*
		 * Read in the text.
		 */
		fread(pn->pn_text, sizeof(char), len, fp);
		fclose(fp);

		/*
		 * Make a widget for this note.
		 */
		MakeNoteWidget(pn);
		numnotes++;
	}
	MakeStashButtons(numnotes);

	closedir(dp);
}

/* 
 *	MakeStashButtons - make stash buttons in the plaid widget
 */
  static void
MakeStashButtons(n)
	int n;	/* total notes that were loaded */
{
	PostItNote *pn;
	char notename[21];
	Widget *tmpWlist, *wp;	/* yuck; but faster */

	if (!n) return;
	if ((tmpWlist = (Widget *) malloc( sizeof(Widget) * n)) == NULL) {
		fprintf(stderr, "Malloc failed (stashbuttons)\n");
		ByeBye();
	}
	wp = tmpWlist;
	for (pn = notes; pn != NULL; pn = pn->pn_next) {
		if (strlen(pn->pn_name) > 20) strncpy(notename, pn->pn_name, 20);
		else strcpy(notename, pn->pn_name);
		notename[20] = '\0';
		pn->pn_stashbutton = XmCreatePushButtonGadget(rcwidget, notename,
			NULL, 0);
		XtAddCallback(pn->pn_stashbutton, XmNactivateCallback, StashCB, pn);
		*wp++ = pn->pn_stashbutton;
	}
	XtManageChildren(tmpWlist, n);
	free(tmpWlist);
}

/*
 * RaiseAllNotes - raise all the notes by raising their shell windows.
 */
void
RaiseAllNotes()
{
	register PostItNote *pn;

	for (pn = notes; pn != NULL; pn = pn->pn_next)
		XRaiseWindow(display, XtWindow(pn->pn_shellwidget));
}

/*
 * LowerAllNotes - lower all the notes by lowering their shell windows.
 */
void
LowerAllNotes()
{
	register PostItNote *pn;

	for (pn = notes; pn != NULL; pn = pn->pn_next)
		XLowerWindow(display, XtWindow(pn->pn_shellwidget));
}

/*
 * SaveAllNotes - save all the notes.
 */
void
SaveAllNotes()
{
	register PostItNote *pn;

	for (pn = notes; pn != NULL; pn = pn->pn_next)
		SaveNote(pn->pn_shellwidget, (caddr_t) pn, 0);
}

/*
 * MakeNoteWidget - make a widget for a Post-It note.
 */
static void
MakeNoteWidget(pn)
	PostItNote *pn;
{
	char notename[21];
	Arg args[8];
	Widget mainW, scrollW, textW, button_barW, hsb, vsb, widgets[5];
	int width;
	int nargs, nwidgets;

	/* main note widget */
	pn->pn_shellwidget = XtAppCreateShell(pn->pn_name, "PostItNote",
		topLevelShellWidgetClass, display, NULL, 0);
	
	nargs = 0;
	mainW = XmCreateMainWindow(pn->pn_shellwidget, "PostItNote", args, nargs);
	XtManageChild(mainW);

	if (pn->pn_mapit == True) {	/* this is a new note */
		/* make a stash button in the plaid widget */
		if (strlen(pn->pn_name) > 20) strncpy(notename, pn->pn_name, 20);
		else strcpy(notename, pn->pn_name);
		notename[21] = '\0';
		pn->pn_stashbutton = XmCreatePushButtonGadget(rcwidget, notename,
			NULL, 0);
		XtAddCallback(pn->pn_stashbutton, XmNactivateCallback, StashCB, pn);
		XtSetSensitive(pn->pn_stashbutton, False);
		XtManageChild(pn->pn_stashbutton);
	}

	/* command button bar widget */
	nargs = 0;
	SetArg(XmNorientation, XmHORIZONTAL);
	button_barW = XmCreateRowColumn(mainW, "buttonbar", args, nargs);
	XtManageChild(button_barW);

	nwidgets = 0;

	widgets[nwidgets] = XmCreatePushButtonGadget(button_barW, "Save", NULL, 0);
	XtAddCallback(widgets[nwidgets], XmNactivateCallback, SaveNote, pn);
	nwidgets++;

	widgets[nwidgets] = XmCreatePushButtonGadget(button_barW, "Erase", NULL, 0);
	XtAddCallback(widgets[nwidgets], XmNactivateCallback, EraseNote, pn);
	nwidgets++;

	widgets[nwidgets] = XmCreatePushButtonGadget(button_barW, "Destroy", NULL, 0);
	XtAddCallback(widgets[nwidgets], XmNactivateCallback, DestroyNote, pn);
	nwidgets++;

	/* hardcopy button */
	widgets[nwidgets] = XmCreatePushButtonGadget(button_barW, "Hardcopy", NULL, 0);
	XtAddCallback(widgets[nwidgets], XmNactivateCallback, HardCopyCB, pn);
	nwidgets++;

	/* "stash" button */
	widgets[nwidgets] = XmCreatePushButtonGadget(button_barW, "Stash", NULL, 0);
	XtAddCallback(widgets[nwidgets], XmNactivateCallback, StashCB, pn);
	nwidgets++;

	XtManageChildren(widgets, nwidgets);

	/* make scroll window */
    nargs = 0;
    scrollW = XmCreateScrolledWindow (mainW, "scrollW", args, nargs);
    XtManageChild (scrollW);


	/*
	 * Create the text window.
	 */
	nargs = 0;
	SetArg(XmNvalue, pn->pn_text);
	SetArg(XmNeditMode, XmMULTI_LINE_EDIT);
	SetArg(XmNscrollVertical, True);
	SetArg(XmNscrollHorizontal, True);
	SetArg(XmNwidth, pn->pn_textwidth);
	SetArg(XmNheight, pn->pn_textheight);
	textW = XmCreateText(scrollW, "text", args, nargs);
	XtManageChild(textW);
	pn->pn_textwidget = textW;


	XmMainWindowSetAreas(mainW, button_barW, NULL, NULL, NULL,
		scrollW);
    nargs = 0;
    SetArg (XmNhorizontalScrollBar, &hsb);
    SetArg (XmNverticalScrollBar, &vsb);
    XtGetValues (mainW, args, nargs);
    XmAddTabGroup (textW);
    if (hsb)
        XmAddTabGroup (hsb);
    if (vsb)
        XmAddTabGroup (vsb);

	/*
	 * Realize the note.  Pop it up if the user just created it.
	 */
	
	if (pn->pn_mapit == False) {	/* ie, pre-existing note */
		XtSetMappedWhenManaged(pn->pn_shellwidget, False);
	}
	XtRealizeWidget(pn->pn_shellwidget);
	XtManageChild(mainW);
}

/*
 * SaveNote - save a note to a file.
 */
/* ARGSUSED */
  static void
SaveNote(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	FILE *fp;
	char *MakeFname();
	Arg args[4];
	int nargs;
	register PostItNote *pn;
	int len; 
	Position shellx, shelly;
	Dimension shellw, shellh, texth, textw;

	if ((pn = (PostItNote *) client_data) == NULL)
		return;

	/*
	 * If it doesn't have a file name, make one.
	 */
	if (pn->pn_file == NULL)
		pn->pn_file = MakeFname(pn->pn_index);

	/*
	 * Create the file.
	 */
	if ((fp = fopen(pn->pn_file, "w")) == NULL) {
		fprintf(stderr, "xmpostit: ");
		perror(pn->pn_file);
		return;
	}

	/*
	 * Get the position of the shell window.
	 */
	nargs = 0;
	SetArg(XmNx, &shellx);
	SetArg(XmNy, &shelly);
	SetArg(XmNwidth, &shellw);
	SetArg(XmNheight, &shellh);
	XtGetValues(pn->pn_shellwidget, args, nargs);

	/*
	 * if this note was never unstashed, the xy is 0. so use the 
	 * old values if we have them.
	 */
	if (!shellx && pn->pn_shellx) shellx = pn->pn_shellx;
	if (!shelly && pn->pn_shelly) shelly = pn->pn_shelly;

	/*
	 * Get the size of the text window.
	 */
	nargs = 0;
	SetArg(XmNheight, &texth);
	SetArg(XmNwidth, &textw);
	XtGetValues(pn->pn_textwidget, args, nargs);

	/*
	 * Get the length of the text in the window.
	 */
	strcpy(pn->pn_text, XmTextGetString(pn->pn_textwidget));
	len = strlen(pn->pn_text);

	/*
	 * Print out the information needed to recreate the note.
	 */
	fprintf(fp, "%s %d %d %d %d %d %d %d %s\n", PostItNoteMagic, shellx, shelly,
			shellw, shellh, texth, textw, len, pn->pn_name);

	/*
	 * Write out the text of the note.
	 */
	if (len)
		fwrite(pn->pn_text, sizeof(char), len, fp);

	fclose(fp);
}

/*
 * EraseNote - erase all the text in a note.
 */
/* ARGSUSED */
  static void
EraseNote(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	PostItNote *pn;
	Widget confirmEraseW;
	static Boolean inited = False;

	if ((pn = (PostItNote *) client_data) == NULL)
		return;

	strcpy(pn->pn_text, XmTextGetString(pn->pn_textwidget));
	if (strlen(pn->pn_text) == 0)
		return;

	/*
	 * Get confirmation of what they want to do.
	 */
	if (!inited) {
		inited = True;
	}
	confirmEraseW = createEraseW(w, pn);
	XtManageChild(confirmEraseW);
}

/* ARGSUSED */
  static Widget
createEraseW(parent, pn)
	Widget parent;
	PostItNote *pn;
{
	Widget eraseW;
	Arg args[6];
	int nargs;

	nargs = 0;
	SetArg(XmNdialogType, XmDIALOG_QUESTION);
	SetArg(XmNmessageString,
		XmStringCreateLtoR("Ok to erase?", XmSTRING_DEFAULT_CHARSET));
	SetArg(XmNdialogStyle, XmDIALOG_APPLICATION_MODAL);
	SetArg(XmNokLabelString,
		XmStringCreateLtoR("Confirm", XmSTRING_DEFAULT_CHARSET));
	eraseW = XmCreateQuestionDialog(pn->pn_shellwidget, "Erase", args, nargs);
	XtUnmanageChild
		(XmMessageBoxGetChild (eraseW, XmDIALOG_HELP_BUTTON));
	nargs = 0;
	SetArg(XmNmwmDecorations, MWM_DECOR_ALL);
	XtSetValues(XtParent(eraseW), args, nargs); 

	XtAddCallback(eraseW, XmNokCallback, EraseCB, pn);
	return(eraseW);
}


/*
 * DestroyNote - destroy a note.
 */
/* ARGSUSED */
  static void
DestroyNote(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	PostItNote *pn;
	Widget confirmDestroyW;
	static Boolean inited = False;

	if ((pn = (PostItNote *) client_data) == NULL)
		return;

	if (!inited) {
		inited = True;
	}
	confirmDestroyW = createDestroyW(w, pn);
	XtManageChild(confirmDestroyW);
}

/* ARGSUSED */
  static Widget
createDestroyW(parent, pn)
	Widget parent;
	PostItNote *pn;
{
	Arg args[6];
	int nargs;
	Widget destroyW;

	nargs = 0;
	SetArg(XmNdialogType, XmDIALOG_QUESTION);
	SetArg(XmNmessageString,
		XmStringCreateLtoR("Ok to destroy note?", XmSTRING_DEFAULT_CHARSET));
	SetArg(XmNdialogStyle, XmDIALOG_APPLICATION_MODAL);
	SetArg(XmNmwmDecorations, MWM_DECOR_ALL);
	SetArg(XmNokLabelString,
		XmStringCreateLtoR("Confirm", XmSTRING_DEFAULT_CHARSET));
	destroyW = XmCreateQuestionDialog(pn->pn_shellwidget, "Destroy",
		args, nargs);
	XtUnmanageChild
		(XmMessageBoxGetChild (destroyW, XmDIALOG_HELP_BUTTON));
	nargs = 0;
	SetArg(XmNmwmDecorations, MWM_DECOR_ALL);
	XtSetValues(XtParent(destroyW), args, nargs); 
	XtAddCallback(destroyW, XmNokCallback, DestroyCB, pn);
	return(destroyW);
}


/*
 * EraseCB - callback for confirmed erase.
 */
/* ARGSUSED */
  static void
EraseCB(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	register PostItNote *pn;

	if ((pn = (PostItNote *) client_data) == NULL)
		return;

	bzero(pn->pn_text, pn->pn_textsize);
	XmTextSetString(pn->pn_textwidget, pn->pn_text);
}

/*
 * DestroyCB - callback for confirmed destroy.
 */
/* ARGSUSED */
  static void
DestroyCB(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	register PostItNote *pn;

	/*
	 * Get rid of the widgets for this note.
	 */
	pn = (PostItNote *) client_data;
	XtDestroyWidget(pn->pn_shellwidget);
	XtDestroyWidget(pn->pn_stashbutton);
	destroyNote(pn);
}

destroyNote(pn)
	PostItNote *pn;
{
	PostItNote *prev;
	int ret;
	/*
	 * Get rid of the note structure.
	 */
	if (pn != notes) {
		for (prev = notes; prev->pn_next; prev = prev->pn_next) {
			if (prev->pn_next == pn)
				break;
		}

		prev->pn_next = pn->pn_next;
	}
	else {
		notes = pn->pn_next;
	}

	/*
	 * Get rid of the file.
	 */
	if (pn->pn_file) {
		if ((ret = access(pn->pn_file, W_OK)) == 0)
			unlink(pn->pn_file);
		free(pn->pn_file);
	}

	/*
	 * Free the memory we used.
	 */
	free(pn->pn_text);
	free(pn);
}

/*
 * HardCopyCB - send note text to "printername"
 * prints the displayed text, not the saved text.  does not save.
 */
/* ARGSUSED */
  static void
HardCopyCB(w, client_data, call_data)
	Widget w;
	caddr_t client_data, call_data;
{
	PostItNote *pn = (PostItNote *) client_data;
	int len;
	FILE *fp;

	if (!*printername) {
		XBell(display, 50);
		return;
	}
	 
	len = strlen(XmTextGetString(pn->pn_textwidget));
	if (len) {
		char cmdbuf[128];

		sprintf(cmdbuf, "lpr -P%s", printername);
		if ((fp = popen(cmdbuf, "w")) == NULL) {
			XBell(display, 50);
			return;   /* probably a bad printer */
		}
		fwrite(XmTextGetString(pn->pn_textwidget),
			sizeof(char), len, fp);
		fclose(fp);
	}
}

/*
 * StashCB - manage/unmanage the button and note.
 */
/* ARGSUSED */
  static void
StashCB(w, client_data, call_data)
	Widget w;
	caddr_t client_data, call_data;
{
	PostItNote *pn = (PostItNote *) client_data;

	if (pn->pn_mapit) {	/* stash it */
		Arg args[4];
		int nargs;

		nargs = 0;
		SetArg(XmNx, &pn->pn_shellx);
		SetArg(XmNy, &pn->pn_shelly);
		SetArg(XmNwidth, &pn->pn_shellw);
		SetArg(XmNheight, &pn->pn_shellh);
		XtGetValues(pn->pn_shellwidget, args, nargs);

		pn->pn_mapit = False;
		XtUnmapWidget(pn->pn_shellwidget);
		XtSetSensitive(pn->pn_stashbutton, True);
	}
	else {	/* unstash it */
		int nargs;
		Arg args[4];
        XSizeHints hints;

		nargs = 0;
		SetArg(XmNx, pn->pn_shellx);
		SetArg(XmNy, pn->pn_shelly);
		XtSetValues(pn->pn_shellwidget, args, nargs);

        hints.x = pn->pn_shellx;
        hints.y = pn->pn_shelly;
        hints.height = pn->pn_shellh;
        hints.width = pn->pn_shellw;
        hints.flags = PSize | USPosition ;
        XSetNormalHints(display, XtWindow(pn->pn_shellwidget), &hints);

		pn->pn_mapit = True;
		XtSetMappedWhenManaged(pn->pn_shellwidget, True);
		XtSetSensitive(pn->pn_stashbutton, False);
		XtMapWidget(pn->pn_shellwidget);
	}
}


/*
 * AllocNote - allocate a new note structure and insert in into the
 *	       list of notes.
 */
static PostItNote *
AllocNote(index)
int index;
{
	register PostItNote *pn;

	/*
	 * Allocate a structure.
	 */
	if (notes == NULL) {
		notes = (PostItNote *) SafeAlloc(sizeof(PostItNote));
		pn = notes;
	}
	else {
		for (pn = notes; pn->pn_next != NULL; pn = pn->pn_next)
			;

		pn->pn_next = (PostItNote *) SafeAlloc(sizeof(PostItNote));
		pn = pn->pn_next;
	}

	/*
	 * Initialize the note.
	 */
	pn->pn_mapit = True;
	pn->pn_textsize = app_res.buf_size;
	pn->pn_text = SafeAlloc(pn->pn_textsize);

	/*
	 * If the index number was given, use it.  Otherwise,
	 * get a new index number.
	 */
	pn->pn_index = (index == NewIndex ? NoteIndex() : index);

	return(pn);
}

/*
 * FindNote - find the note structure with the given index number.
 */
static PostItNote *
FindNote(index)
register int index;
{
	register PostItNote *pn;

	for (pn = notes; pn != NULL; pn = pn->pn_next) {
		if (pn->pn_index == index)
			return(pn);
	}

	return(NULL);
}


/*
 * NoteIndex - find the lowest free index number.
 */

NoteIndex()
{
	register int index;

	/*
	 * This is O(n**2), but the list should be small.
	 */
	for (index = 1; ; index++) {
		if (FindNote(index) == NULL)
			return(index);
	}
}
