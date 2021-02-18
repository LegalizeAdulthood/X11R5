/*
 * Copyright 1991 by David A. Curry
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation.  The
 * author makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied warranty.
 */
#ifndef lint
static char	*RCSid = "$Header: /home/orchestra/davy/progs/xpostit/RCS/note.c,v 1.11 91/09/06 18:29:21 davy Exp $";
#endif

/*
 * note.c - routines for handling notes.
 *
 * David A. Curry
 * Purdue University
 * Engineering Computer Network
 * West Lafayette, IN 47907
 * davy@ecn.purdue.edu
 *
 * $Log:	note.c,v $
 * Revision 1.11  91/09/06  18:29:21  davy
 * Added copyright/permission notice for submission to MIT R5 contrib.
 * 
 * Revision 1.10  91/09/06  18:06:15  davy
 * Fixed title on new notes.
 * 
 * Revision 1.9  91/09/06  17:13:22  davy
 * Added code for hide and show all notes, and to allow changing the titles
 * of note windows and icons.
 * 
 * Revision 1.8  91/07/11  09:08:25  davy
 * Attempt to fix note positioning on save.
 * 
 * Revision 1.7  91/07/11  09:04:46  davy
 * Fixed to not truncate notes when killed.  From George Ross,
 * gdmr@cs.ed.ac.uk.
 * 
 * Revision 1.6  91/07/11  09:03:27  davy
 * Added patch from Stuart Marks, smarks@eng.sun.com.
 * 
 * Revision 1.5  91/07/11  09:01:51  davy
 * Added patch for SVr3 from Tim Roper (tim@labtam.labtam.oz.au).
 * 
 * Revision 1.4  90/06/14  11:20:09  davy
 * Ported to X11 Release 4.  Changed to get the note save position in an
 * ICCCM compliant (although kludgy) way.
 * 
 * Revision 1.3  89/01/10  09:57:24  davy
 * Changed XGetNormalHints to XGetWindowAttributes for getting the size of
 * the text window when saving.  XGetNormalHints comes up with the wrong
 * size, for some reason.
 * 
 * Revision 1.2  89/01/10  09:13:13  davy
 * Added XtChain... arguments to buttons and text window to prevent the
 * buttons from getting screwed up on a resize.
 * 
 * Revision 1.1  89/01/10  09:00:21  davy
 * Initial revision
 * 
 */
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Shell.h>
#include <sys/param.h>
#ifdef USG
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
#include <stdio.h>

#include "xpostit.h"
#include "vroot.h"

static PostItNote	*notes = NULL;

static PostItNote	*FindNote();
static PostItNote	*AllocNote();

static void		SaveNote();
static void		EraseNote();
static void		TitleNote();
static void		OkayTitle();
static void		CancelTitle();
static void		DestroyNote();
static void		CancelErase();
static void		ConfirmErase();
static void		CancelDestroy();
static void		ConfirmDestroy();
static void		MakeNoteWidget();

static int		NoteIndex();

extern Widget		dialog;

/*
 * CreateNewNote - create a new note of the specified size.
 */
void
CreateNewNote(size)
int size;
{
	char buf[64];
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
	case PostItNote_2x3:
		hpixel = 2 * hpi;
		wpixel = 3 * wpi;
		break;
	case PostItNote_3x3:
		hpixel = 3 * hpi;
		wpixel = 3 * wpi;
		break;
	case PostItNote_3x4:
		hpixel = 3 * hpi;
		wpixel = 4 * wpi;
		break;
	case PostItNote_3x5:
		hpixel = 3 * hpi;
		wpixel = 5 * wpi;
		break;
	case PostItNote_4x6:
		hpixel = 4 * hpi;
		wpixel = 6 * wpi;
		break;
	}

	/*
	 * Allocate a new note structure.
	 */
	pn = AllocNote(NewIndex);

	/*
	 * Set the text window size.
	 */
	pn->pn_textwidth = wpixel;
	pn->pn_textheight = hpixel;

	sprintf(buf, "PostItNote #%d", pn->pn_index);
	pn->pn_title = SafeAlloc(strlen(buf) + 1);
	strcpy(pn->pn_title, buf);

	/*
	 * Make the widget for the note.
	 */
	MakeNoteWidget(pn);
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
#ifdef USG
	register struct dirent *d;
#else
	register struct direct *d;
#endif
	int n, len, nlen, shellx, shelly, texth, textw;
	char buf[BUFSIZ], title[BUFSIZ], fname[MAXPATHLEN];

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

		/*
		 * Open the file.
		 */
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
		title[0] = '\0';

		n = sscanf(buf, "%d %d %d %d %d %[^\n]", &shellx, &shelly,
			   &texth, &textw, &len, title);

		/*
		 * Bad format; skip it.  Allow 5 or 6 fields
		 * for backward compatibility (no titles).
		 */
		if ((n != 5) && (n != 6)) {
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

		if (strlen(title) == 0)
			sprintf(title, "PostItNote #%d", n);

		/*
		 * Set the information.
		 */
		pn->pn_shellx = shellx;
		pn->pn_shelly = shelly;
		pn->pn_textwidth = textw;
		pn->pn_textheight = texth;
		pn->pn_positionit = True;

		/*
		 * Save the file name.
		 */
		pn->pn_file = SafeAlloc(strlen(fname) + 1);
		strcpy(pn->pn_file, fname);

		/*
		 * Save the title.
		 */
		pn->pn_title = SafeAlloc(strlen(title) + 1);
		strcpy(pn->pn_title, title);

		/*
		 * If we need a bigger buffer than the default,
		 * get one.
		 */
		if (len >= pn->pn_textsize) {
			n = (len + app_res.buf_size - 1) / app_res.buf_size;
			n = n * app_res.buf_size;

			if ((pn->pn_text = realloc(pn->pn_text, n)) == NULL) {
				fprintf(stderr, "xpostit: out of memory.\n");
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
	}

	closedir(dp);
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
 * HideAllNotes - hide all the notes by unmapping them.
 */
void
HideAllNotes()
{
	register PostItNote *pn;

	for (pn = notes; pn != NULL; pn = pn->pn_next) {
		XtUnmapWidget(pn->pn_shellwidget);
		pn->pn_hidden = True;
	}
}

/*
 * ShowAllNotes - show all the notes by mapping them.
 */
void
ShowAllNotes()
{
	register PostItNote *pn;

	for (pn = notes; pn != NULL; pn = pn->pn_next) {
		if (pn->pn_hidden) {
			XtMapWidget(pn->pn_shellwidget);
			XRaiseWindow(display, XtWindow(pn->pn_shellwidget));
			pn->pn_hidden = False;
		}
	}
}

/*
 * SaveAllNotes - save all the notes.
 */
void
SaveAllNotes()
{
	register PostItNote *pn;

	for (pn = notes; pn != NULL; pn = pn->pn_next)
		SaveNote(pn->pn_shellwidget, (caddr_t) pn->pn_index, 0);
}

/*
 * MakeNoteWidget - make a widget for a Post-It note.
 */
static void
MakeNoteWidget(pn)
PostItNote *pn;
{
	Arg args[16];
	char geometry[64];
	Widget form, widgets[5];
	XtCallbackRec callbacks[2];
	register int nargs, nwidgets;

	bzero(callbacks, sizeof(callbacks));
	nargs = 0;

	/*
	 * If the shell window coordinates are valid, use them.
	 */
	if (pn->pn_positionit) {
		sprintf(geometry, "+%d+%d", pn->pn_shellx, pn->pn_shelly);
		SetArg(XtNgeometry, geometry);
		SetArg(XtNx, pn->pn_shellx);
		SetArg(XtNy, pn->pn_shelly);
	}
	else {
		SetArg(XtNx, pn->pn_shellx);
		SetArg(XtNy, pn->pn_shelly);
	}

	/*
	 * Make the shell widget for this note.  We just use
	 * a pop-up shell for this.
	 */
	SetArg(XtNinput, True);
	SetArg(XtNtitle, pn->pn_title);
	SetArg(XtNiconName, pn->pn_title);
	pn->pn_shellwidget = XtCreatePopupShell("PostItNote",
						wmShellWidgetClass,
						toplevel, args, nargs);

	/*
	 * Make a form widget.
	 */
	form = XtCreateWidget("Note", formWidgetClass, pn->pn_shellwidget,
			      NULL, 0);

	nwidgets = -1;

	/*
	 * Put each button into the shell widget.  The second and third
	 * buttons will position themselves relative to the first one;
	 * the first one will position itself relative to the edge of
	 * the form.
	 */
	SetCallback(SaveNote, pn->pn_index);

	nargs = 0;
	SetArg(XtNtop, XtChainTop);
	SetArg(XtNleft, XtChainLeft);
	SetArg(XtNright, XtChainLeft);
	SetArg(XtNbottom, XtChainTop);
	SetArg(XtNcallback, callbacks);
	widgets[++nwidgets] = XtCreateWidget("Save", commandWidgetClass,
			form, args, nargs);

	SetCallback(EraseNote, pn->pn_index);

	nargs = 0;
	SetArg(XtNtop, XtChainTop);
	SetArg(XtNleft, XtChainLeft);
	SetArg(XtNright, XtChainLeft);
	SetArg(XtNbottom, XtChainTop);
	SetArg(XtNcallback, callbacks);
	SetArg(XtNfromHoriz, widgets[nwidgets]);
	widgets[++nwidgets] = XtCreateWidget("Erase", commandWidgetClass,
			form, args, nargs);

	SetCallback(DestroyNote, pn->pn_index);

	nargs = 0;
	SetArg(XtNtop, XtChainTop);
	SetArg(XtNleft, XtChainLeft);
	SetArg(XtNright, XtChainLeft);
	SetArg(XtNbottom, XtChainTop);
	SetArg(XtNcallback, callbacks);
	SetArg(XtNfromHoriz, widgets[nwidgets]);
	widgets[++nwidgets] = XtCreateWidget("Destroy", commandWidgetClass,
			form, args, nargs);

	SetCallback(TitleNote, pn->pn_index);

	nargs = 0;
	SetArg(XtNtop, XtChainTop);
	SetArg(XtNleft, XtChainLeft);
	SetArg(XtNright, XtChainLeft);
	SetArg(XtNbottom, XtChainTop);
	SetArg(XtNcallback, callbacks);
	SetArg(XtNfromHoriz, widgets[nwidgets]);
	widgets[++nwidgets] = XtCreateWidget("Title", commandWidgetClass,
			form, args, nargs);

	/*
	 * Create the text window.
	 */
	nargs = 0;
	SetArg(XtNtop, XtChainTop);
	SetArg(XtNleft, XtChainLeft);
	SetArg(XtNright, XtChainRight);
	SetArg(XtNstring, pn->pn_text);
	SetArg(XtNeditType, XawtextEdit);
	SetArg(XtNbottom, XtChainBottom);
	SetArg(XtNuseStringInPlace, True);
	SetArg(XtNlength, pn->pn_textsize);
	SetArg(XtNwidth, pn->pn_textwidth);
	SetArg(XtNheight, pn->pn_textheight);
	SetArg(XtNresize, XawtextResizeNever);
	SetArg(XtNfromVert, widgets[nwidgets]);

	if (app_res.scroll_bar) {
		SetArg(XtNscrollVertical, XawtextScrollAlways);
	}
	else {
		SetArg(XtNscrollVertical, XawtextScrollWhenNeeded);
	}

	widgets[++nwidgets] = XtCreateWidget("NoteText", asciiTextWidgetClass,
					     form, args, nargs);

	/*
	 * Save the text widget.
	 */
	pn->pn_textwidget = widgets[nwidgets];

	/*
	 * Let the top level shell know all these guys are here.
	 */
	XtManageChildren(widgets, XtNumber(widgets));
	XtManageChild(form);

	/*
	 * Realize the note and pop it up.
	 */
	XtRealizeWidget(pn->pn_shellwidget);
	XtPopup(pn->pn_shellwidget, XtGrabNone);
}

/*
 * SaveNote - save a note to a file.
 */
static void
SaveNote(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	FILE *fp;
	Window junk;
	char *MakeFname();
	register PostItNote *pn;
	XWindowAttributes win_attributes;
	int len, shellx, shelly, texth, textw;

	/*
	 * Find the note we're saving.
	 */
	if ((pn = FindNote((int) client_data)) == NULL)
		return;

	/*
	 * If it doesn't have a file name, make one.
	 */
	if (pn->pn_file == NULL)
		pn->pn_file = MakeFname(pn->pn_index);

	/*
	 * Get the position of the shell window.
	 */
	GetNotePosition(pn->pn_shellwidget, &shellx, &shelly);

	/*
	 * Get the size of the text window.
	 */
	XGetWindowAttributes(display, XtWindow(pn->pn_textwidget),
			     &win_attributes);
	texth = win_attributes.height;
	textw = win_attributes.width;

	/*
	 * Get the length of the text in the window.
	 */
	len = strlen(pn->pn_text);

	/*
	 * Create the file.
	 */
	if ((fp = fopen(pn->pn_file, "w")) == NULL) {
		fprintf(stderr, "xpostit: ");
		perror(pn->pn_file);
		return;
	}

	/*
	 * Print out the information needed to recreate the note.
	 */
	fprintf(fp, "%s %d %d %d %d %d %s\n", PostItNoteMagic, shellx, shelly,
		texth, textw, len, pn->pn_title);

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
static void
EraseNote(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	PostItNote *pn;
	XtCallbackRec cancel[2], confirm[2];

	bzero(confirm, sizeof(confirm));
	bzero(cancel, sizeof(cancel));

	/*
	 * Find the note we're erasing.
	 */
	if ((pn = FindNote((int) client_data)) == NULL)
		return;

	/*
	 * If there's nothing in the window, then there's
	 * no need to erase it.
	 */
	if (strlen(pn->pn_text) == 0)
		return;

	confirm[0].callback = ConfirmErase;
	confirm[0].closure = (caddr_t) pn->pn_index;
	cancel[0].callback = CancelErase;
	cancel[0].closure = (caddr_t) pn->pn_index;

	/*
	 * Get confirmation of what they want to do.
	 */
	ConfirmIt(confirm, cancel);
}

/*
 * DestroyNote - destroy a note.
 */
static void
DestroyNote(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	PostItNote *pn;
	XtCallbackRec cancel[2], confirm[2];

	bzero(confirm, sizeof(confirm));
	bzero(cancel, sizeof(cancel));

	/*
	 * Find the note we're destroying.
	 */
	if ((pn = FindNote((int) client_data)) == NULL)
		return;

	confirm[0].callback = ConfirmDestroy;
	confirm[0].closure = (caddr_t) pn->pn_index;
	cancel[0].callback = CancelDestroy;
	cancel[0].closure = (caddr_t) pn->pn_index;

	/*
	 * Get confirmation of what they want to do.
	 */
	ConfirmIt(confirm, cancel);
}

/*
 * TitleNote - change note title.
 */
static void
TitleNote(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	int nargs;
	Arg args[4];
	char *title;
	PostItNote *pn;

	/*
	 * Find the note we're titling.
	 */
	if ((pn = FindNote((int) client_data)) == NULL)
		return;

	/*
	 * Get a new title.
	 */
	GetNoteTitle(pn, OkayTitle, CancelTitle);
}

/*
 * ConfirmErase - callback for erase confirmation.
 */
static void
ConfirmErase(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	Arg args[4];
	register int nargs;
	register PostItNote *pn;
	XawTextPosition pos = 0;

	/*
	 * Get rid of the confirmation box.
	 */
	ClearConfirm();

	/*
	 * Find the note we're erasing.
	 */
	if ((pn = FindNote((int) client_data)) == NULL)
		return;

	/*
	 * This is a kludge.  They should have provided
	 * an XtTextErase function.
	 */
	bzero(pn->pn_text, pn->pn_textsize);

	nargs = 0;
	SetArg(XtNstring, pn->pn_text);
	XtSetValues(pn->pn_textwidget, args, nargs);
}

/*
 * CancelErase - callback for erase cancellation.
 */
static void
CancelErase(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	/*
	 * Get rid of the confirmation box.
	 */
	ClearConfirm();
}

/*
 * ConfirmDestroy - callback for destroy confirmation.
 */
static void
ConfirmDestroy(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	register PostItNote *pn, *prev;

	/*
	 * Get rid of the confirmation box.
	 */
	ClearConfirm();

	/*
	 * Find the note we're destroying.
	 */
	if ((pn = FindNote((int) client_data)) == NULL)
		return;

	/*
	 * Get rid of the widgets for this note.
	 */
	XtPopdown(pn->pn_shellwidget);
	XtDestroyWidget(pn->pn_shellwidget);

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
 * CancelDestroy - callback for destroy cancellation.
 */
static void
CancelDestroy(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	/*
	 * Get rid of the confirmation box.
	 */
	ClearConfirm();
}

/*
 * OkayTitle - callback for title okay button.
 */
static void
OkayTitle(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	int nargs;
	Arg args[4];
	String title;
	register PostItNote *pn;

	/*
	 * Get rid of the dialog box.
	 */
	ClearTitle();

	/*
	 * Find the note.
	 */
	if ((pn = FindNote((int) client_data)) == NULL)
		return;

	/*
	 * Get the new title from the dialog.
	 */
	title = (String) XawDialogGetValueString(dialog);

	/*
	 * Save the new title.
	 */
	free(pn->pn_title);
	pn->pn_title = SafeAlloc(strlen(title) + 1);
	strcpy(pn->pn_title, title);

	/*
	 * Change the title.  There's a bug in R4 twm; it won't
	 * change the icon name until you unmap and map the
	 * window (ick).
	 */
	nargs = 0;
	SetArg(XtNtitle, pn->pn_title);
	SetArg(XtNiconName, pn->pn_title);
	XtSetValues(pn->pn_shellwidget, args, nargs);
}

/*
 * CancelTitle - callback for title cancel.
 */
static void
CancelTitle(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	/*
	 * Get rid of the dialog box.
	 */
	ClearTitle();
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
	pn->pn_hidden = False;
	pn->pn_positionit = False;
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
static int
NoteIndex()
{
	register int index;
	register PostItNote *pn;

	/*
	 * This is O(n**2), but the list should be small.
	 */
	for (index = 1; ; index++) {
		if ((pn = FindNote(index)) == NULL)
			return(index);
	}
}

/*
 * GetNotePosition - find the position of the widget window, taking into
 *		     account any borders stuck on by reparenting window
 *		     managers.
 *
 *		     This is a KLUDGE.  The ICCCM does not specify a way
 *		     for a client to find out what kind of border stuff
 *		     the window manager has added.
 *
 *		     Thanks to Stewart Levin for the original code.
 */
GetNotePosition(w, x, y)
int *x, *y;
Widget w;
{
	Window *children;
	unsigned int status, nchildren;
	Window here, parent, root, vroot;
	XWindowAttributes win_attributes;

	parent = XtWindow(w);
	vroot = VirtualRootWindow(display, DefaultScreen(display));

	/*
	 * Walk up the tree looking for a parent of this window whose
	 * parent is the root.  That'll either be this window (if there
	 * is no window manager window) or the window added by the
	 * window manager.
	 */
	do {
		here = parent;

		status = XQueryTree(display, here, &root, &parent, &children,
				    &nchildren);

		if (!status)
			break;

		if (children)
			XFree((XrmString) children);
	} while ((parent != vroot) && (parent != root));

	/*
	 * Get the attributes of this window we just found.
	 */
	XGetWindowAttributes(display, here, &win_attributes);

#ifdef notdef
	/*
	 * Now deduct the border from the position of the window.
	 * We know the coordinates don't need translating, since
	 * this window's parent is the root.
	 */
	*x = win_attributes.x - win_attributes.border_width;
	*y = win_attributes.y - win_attributes.border_width;
#else
	*x = win_attributes.x;
	*y = win_attributes.y;
#endif
}
