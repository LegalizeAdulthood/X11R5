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
/*
 * $Header: /home/orchestra/davy/progs/xpostit/RCS/xpostit.h,v 1.4 91/09/06 18:29:47 davy Exp $
 *
 * xpostit.h - declarations for xpostit.
 *
 * David A. Curry
 * Purdue University
 * Engineering Computer Network
 * West Lafayette, IN 47907
 * davy@ecn.purdue.edu
 *
 * $Log:	xpostit.h,v $
 * Revision 1.4  91/09/06  18:29:47  davy
 * Added copyright/permission notice for submission to MIT R5 contrib.
 * 
 * Revision 1.3  91/09/06  17:15:52  davy
 * Added new features.
 * 
 * Revision 1.2  90/06/14  11:21:33  davy
 * Ported to X11 Release 4.
 * 
 * Revision 1.1  90/06/13  09:48:52  davy
 * Initial revision
 * 
 */

/*
 * Default values.
 */
#define DefaultBufSize		1024
#define DefaultNoteDir		".postitnotes"
#define DefaultPlaidWidth	64
#define DefaultPlaidHeight	64

/*
 * Post-It Note sizes.
 */
#define PostItNote_1p5x2	0
#define PostItNote_2x3		1
#define PostItNote_3x3		2
#define PostItNote_3x4		3
#define PostItNote_3x5		4
#define PostItNote_4x6		5

/*
 * Post-It Note file information.
 */
#define PostItNoteMagic		"%!<postitnote>"
#define PostItNoteFname		"note"

/*
 * Application class.
 */
#define PostItNoteClass		"XPostit"

/*
 * Request for a new note index number.
 */
#define NewIndex		-1

/*
 * Just in case.
 */
#ifndef MAXPATHLEN
#define MAXPATHLEN	1024
#endif

/*
 * Useful macros.
 */
#define SetArg(which, val)	XtSetArg(args[nargs], (which),\
					 (XtArgVal) (val)); nargs++
#define SetCallback(which, val)	callbacks[0].callback = (which); \
				callbacks[0].closure = (caddr_t) (val)

/*
 * The Post-It Note record.  One of these is allocated
 * for each note created.
 */
typedef struct _PostItNote {
	Widget	pn_shellwidget;		/* shell widget holding it all	*/
	Widget	pn_textwidget;		/* text widget of the note	*/

	char	*pn_file;		/* file note will be saved in	*/
	char	*pn_text;		/* buffer holding text of note	*/
	char	*pn_title;		/* title of the note		*/

	int	pn_index;		/* index number of note		*/
	int	pn_shellx;		/* x coord of shell widget	*/
	int	pn_shelly;		/* y coord of shell widget	*/
	int	pn_textsize;		/* size of pn_text in chars	*/
	int	pn_textwidth;		/* width of text widget window	*/
	int	pn_textheight;		/* height of text widget window	*/

	Boolean	pn_hidden;		/* true if unmapped		*/
	Boolean	pn_positionit;		/* true if shellx/shelly valid	*/

	struct	_PostItNote *pn_next;	/* pointer to next note record	*/
} PostItNote;

/*
 * The resource record, for holding resources specific to xpostit.
 */
typedef struct {
	int	buf_size;		/* size of pn_text to be used	*/
	String	note_dir;		/* path to note directory	*/
	Boolean	scroll_ovf;		/* set scroll on overflow	*/
	Boolean	scroll_bar;		/* turn on scroll bars		*/
	Boolean save_notes;		/* save notes on exit		*/
} AppRes, *AppResPtr;

/*
 * External variable declarations.
 */
extern	AppRes app_res;

extern	Screen *screen;
extern	Display *display;

extern	Widget toplevel;
extern	Widget listwidget;
extern	Widget menuwidget;
extern	Widget plaidwidget;

/*
 * Function declarations.
 */
char	*SafeAlloc();

void	ByeBye();
void	ConfirmIt();
void	ClearConfirm();
void	SaveAllNotes();
void	HideAllNotes();
void	ShowAllNotes();
void	RaiseAllNotes();
void	LowerAllNotes();
void	CreateNewNote();
void	CreateMenuWidget();
void	CreatePlaidWidget();
