/* -*-Mode: C; -*- */

#ifndef _TTY_BSD_INCLUDE
#define _TTY_BSD_INCLUDE

/* $Header: /usr3/Src/emu/term/sysdep/RCS/tty_bsd.i,v 1.3 91/09/30 21:36:20 jkh Exp $ */

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
 * Tty service routines for BSD systems.
 *
 * Author: Jordan K. Hubbard
 * Date: March 20th, 1990.
 * Description: Various routines for dealing with ttys that are generic to
 *		BSD systems (in theory).
 *
 * Revision History:
 *
 * $Log:	tty_bsd.i,v $
 * Revision 1.3  91/09/30  21:36:20  jkh
 * Decwrl change set.
 * 
 * Revision 1.2  1991/08/17  10:16:06  jkh
 * General reorganization, changed declarations for TAGS.
 *
 * Revision 1.1  90/11/05  03:48:18  jkh
 * Initial revision
 * 
 * 
 */

/* BSD is a variant of unix */
#include "sysdep/tty_unix.i"

/* Sets no-blocking I/O on a file descriptor */
Local void
bsd_tty_set_nonblocking(w, fd)
TermWidget w;
int fd;
{
     int mode = 1;

     if (ioctl(fd, FIONBIO, &mode) == -1)
	  perror("Couldn't FIONBIO pty");
}

/* Set a new tty size */
Local void
bsd_tty_set_size(w, rows, cols, width, height)
TermWidget w;
int rows, cols, width, height;
{
     struct winsize wz;
     int pgrp;
     Import int ioctl(), kill();

     wz.ws_row = rows;
     wz.ws_col = cols;
     wz.ws_xpixel = width;
     wz.ws_ypixel = height;
     ioctl(w->term.slave, TIOCSWINSZ, &wz);
     if (ioctl(w->term.slave, TIOCGPGRP, &pgrp) != -1)
	  killpg(pgrp, SIGWINCH);
}

Local void
bsd_tty_add_utmp(w)
TermWidget w;
{
     /*
      * Can't do anything at present - don't know how to add utmp entries
      * under generic BSD.
      */
}

Local void
bsd_tty_remove_utmp(w)
TermWidget w;
{
     /* Same here for now */
}

/* If w is NULL, get tty values for invoking tty, not pty */
Local Generic
bsd_tty_get_values(w)
TermWidget w;
{
     /* nichts */
     return NULL;
}

/* Must return some "sane" set of hardwired values */
Local Generic
bsd_tty_get_sane(w)
TermWidget w;
{
     /* nada */
     return NULL;
}

/* Whap some saved values onto the tty */
Local void
bsd_tty_set_values(w, val)
TermWidget w;
Generic val;
{
     /* zilch */
}

#endif	/* _TTY_BSD_INCLUDE */
