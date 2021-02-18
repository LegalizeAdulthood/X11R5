#ifndef _TERMP_H
#define _TERMP_H

/* $Header: /usr3/Src/emu/term/RCS/TermP.h,v 1.9 91/09/30 21:41:14 me Exp $ */

/*
 * This file is part of the PCS emu system.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * Term widget private include file.
 *
 * Author: Jordan K. Hubbard
 * Date: June 6th, 1990.
 * Description: Class specific and private data for the term widget.
 *
 * Revision History:
 *
 * $Log:	TermP.h,v $
 * Revision 1.9  91/09/30  21:41:14  me
 * Whatever the heck I did - I don't remember.
 * 
 * Revision 1.8  91/08/26  12:09:06  jkh
 * *** empty log message ***
 * 
 * Revision 1.7  91/08/05  15:54:42  jkh
 * Whatever Mikey Baby did.
 * 
 * Revision 1.6  90/11/13  14:57:06  jkh
 * Double fd version.
 * 
 * Revision 1.1  90/07/19  15:47:03  jkh
 * Initial revision
 * 
 *
 */

#include <X11/IntrinsicP.h>
#include <X11/CompositeP.h>

#include "common.h"
#include "Term.h"
#include "trie.h"

/* For convenience */
#define CANVAS_W(w)		((w)->term.canvas)
#define RESIZE_LOCKED(w)	((w)->term.resizing == TRUE)
#define RESIZE_UNLOCKED(w)	((w)->term.resizing == FALSE)
#define LOCK_RESIZE(w)		((w)->term.resizing = TRUE)
#define UNLOCK_RESIZE(w)	((w)->term.resizing = FALSE)

/*
 * So our class can keep track of processes. Too bad the toolkit has no
 * way of dealing with signals; it wouldn't be necessary to do things this
 * way.
 */
typedef struct _ptable {
     TermWidget w;
     struct _ptable *next;
} PTable;

typedef struct {
     int sig_cnt;
     PTable *procs;
} TermClassPart;

typedef struct _TermClassRec {
     CoreClassPart	core_class;
     CompositeClassPart composite_class;
     TermClassPart	term_class;
} TermClassRec;

typedef struct {
     /* Resources */
     String command;			/* command name			*/
     String *command_args;		/* command arguments		*/
     String term_type;			/* terminal type		*/
     String rops;			/* initial rops			*/
     Dimension w_inc, h_inc;		/* width and height incs	*/
     Widget canvas;			/* which widget's the canvas	*/
     XtCallbackList in_parse;		/* input parser(s)		*/
     XtCallbackList out_parse;		/* output parser(s)		*/
     XtCallbackList proc_death;		/* process death callback	*/
     XtCallbackList iop_req;		/* for iop requests to parent	*/
     GFuncPtr layout_kids;		/* for external layout purposes	*/
     int read_size;			/* number of chars to read in	*/
     Boolean utmp_inhibit;		/* don't add utmp entries	*/
     Boolean login_shell;		/* invoke shell as login shell	*/

     /* Private state */
     ComBlockPtr cb;			/* current communications block	*/
     ComBlockPtr save;			/* saved communications block	*/
     String tname;			/* name of allocated PTY	*/
     pid_t pid;				/* process ID of child process	*/
     uid_t uid;				/* uid of invoking user		*/
     gid_t gid;				/* gid of invoking user		*/
     int master;			/* pty master file descriptor	*/
     int slave;				/* pty slave file descriptor	*/
     XtInputId xi, xo;			/* toolkit input/output ID's	*/
     State *zero_state;			/* root of possible trie parser	*/
     Possible_state *current_states;	/* current trie possibilities	*/
     int chars_to_parse;		/* now many chars buffered up	*/
     int char_parse_mark;		/* point of interest in buffer	*/
     unsigned char buf[BUF_SZ];		/* buffer for unparsed chars	*/
     unsigned char immediate;		/* do next request right away	*/
     Rparse *rparse_list;		/* reverse parser operations	*/
     Generic init_tty;			/* parent's saved tty settings	*/
     Boolean resizing;			/* in the middle of resizing?	*/
     /**/
     int save_pos;
     char save_buf[BUF_SZ];
} TermPart;

typedef struct _TermRec {
     CorePart           core;
     CompositePart	composite;
     TermPart		term;
} TermRec;

/* Routines shared across multiple parts of the widget */

/* Misc tools */
Import int strcomp();

/* From the tty section */
Import Boolean tty_open();
Import String tty_find_pty();
Import Generic tty_get_values();
Import Generic tty_get_sane();
Import void tty_close();
Import void tty_set_size();
Import void tty_add_utmp();
Import void tty_remove_utmp();
Import void tty_set_values();

/* From the process section */
Import void process_init();
Import void process_create();
Import void process_destroy();
Import void process_signal();
Import void process_reaper();

/* From the parser section */
Import Boolean parser_install();
Import void iparse(), oparse(), rparse();
Import void dispatch();

/* From widget */
Import void XpNrememberWidget();
Import void XpNremoveRememberedWidget();
Import TermWidget XpNfindWidgetByPid();

#endif /* _TERMP_H */
