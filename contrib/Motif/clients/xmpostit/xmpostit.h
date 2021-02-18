/*
 * $Header: /usr/src/local/bin/X11/xmpostit/RCS/xmpostit.h,v 1.7 91/09/27 11:25:20 dm Exp $
 *
 * xpostit.h - declarations for xpostit.
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
#define PostItNote_3x3		1
#define PostItNote_3x5		2

/*
 * Post-It Note file information.
 */
#define PostItNoteMagic		"%!<postitnote>"
#define PostItNoteFname		"note"

/*
 * Application class.
 */
#define PostItNoteClass		"XmPostit"

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
#define MAXNAME	64

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
	Widget	pn_stashbutton;		/* button widget in postit note */

	char 	*pn_name;		/* name for the note */
	char	*pn_file;		/* file note will be saved in	*/
	char	*pn_text;		/* buffer holding text of note	*/

	int	pn_index;		/* index number of note		*/
	Position	pn_shellx;		/* x coord of shell widget	*/
	Position	pn_shelly;		/* y coord of shell widget	*/
	Dimension pn_shellh;		/* height of widget */
	Dimension pn_shellw;		/* width of widget */
	int	pn_textsize;		/* size of pn_text in chars	*/
	Dimension	pn_textwidth;		/* width of text widget window	*/
	Dimension	pn_textheight;		/* height of text widget window	*/

	Boolean	pn_mapit;		/* true if shellx/shelly valid	*/

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
	Boolean show_plaid;		/* use plaid background 	*/
} AppRes, *AppResPtr;

/*
 * External variable declarations.
 */
extern	AppRes app_res;

extern	Screen *screen;
extern	Display *display;

extern	Widget toplevel;
extern	Widget rcwidget;
extern	Widget menuwidget;
extern	Widget plaidwidget;

extern	char printername[];

/*
 * Function declarations.
 */
char	*SafeAlloc();

void	ByeBye();
void	SaveAllNotes();
void	RaiseAllNotes();
void	LowerAllNotes();
void	ChoosePrinter();
void	CreateNewNote();
void	CreateMenuWidget();
void	CreatePlaidWidget();
