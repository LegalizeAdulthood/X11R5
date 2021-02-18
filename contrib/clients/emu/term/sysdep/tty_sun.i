/* -*-Mode: C; -*- */

#ifndef _TTY_SUN_INCLUDE
#define _TTY_SUN_INCLUDE

/* $Header: /usr3/Src/emu/term/sysdep/RCS/tty_sun.i,v 1.3 91/09/30 21:36:36 jkh Exp Locker: jkh $ */

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
 * Tty allocation/deallocation routines for Sun systems.
 *
 * Author: Jordan K. Hubbard
 * Date: July 25th, 1990.
 * Description: Here are all the routines for getting and setting tty
 *		parameters on Sun 3 (and hopefully Sun 4) systems.
 *
 * Revision History:
 *
 * $Log:	tty_sun.i,v $
 * Revision 1.3  91/09/30  21:36:36  jkh
 * Decwrl change set.
 * 
 * Revision 1.2  1991/08/17  10:16:12  jkh
 * General reorganization, changed declarations for TAGS.
 *
 * Revision 1.4  90/11/20  17:45:34  jkh
 * Alpha.
 * 
 * Revision 1.3  90/10/11  17:31:30  jkh
 * Fudging with ioctl() calls.
 * 
 * Revision 1.2  90/10/08  23:30:48  jkh
 * Reshuffle.
 * 
 * Revision 1.1  90/10/08  18:19:16  jkh
 * Initial revision
 * 
 */

/* Suns are BSD */
#include "sysdep/tty_bsd.i"

/* And reasonably posix compliant */
#include "sysdep/tty_posix.i"

Export void
tty_set_nonblocking(w, fd)
TermWidget w;
int fd;
{
     /* Generic BSD does everything we need */
     bsd_tty_set_nonblocking(w, fd);
}

Export String
tty_find_pty(w)
TermWidget w;
{
     /* Generic unix code does what we want */
     return unix_tty_find_pty(w);
}

/* Set a new tty size */
Export void
tty_set_size(w, rows, cols, width, height)
TermWidget w;
int rows, cols, width, height;
{
     /* Generic BSD does what we want */
     bsd_tty_set_size(w, rows, cols, width, height);
}

/* Add a utmp/wtmp entry for a tty */
Export void
tty_add_utmp(w)
TermWidget w;
{
     /* Generic BSD does what we want */
     bsd_tty_add_utmp(w);
}

/* Remove a utmp/wtmp entry for a tty */
Export void
tty_remove_utmp(w)
TermWidget w;
{
     /* Generic BSD does what we want */
     bsd_tty_remove_utmp(w);
}

/* If w is NULL, get tty values for invoking tty, not pty */
Export Generic
tty_get_values(w)
TermWidget w;
{
     /* Suns are posix */
     return posix_tty_get_values(w);
}

/* Must return some "sane" set of hardwired values */
Export Generic
tty_get_sane(w)
TermWidget w;
{
     /* SUNOS has some extra thingies that POSIX doesn't */
     static struct termios sane;
     static char cchars[] = {
	  CINTR, CQUIT, CERASE, CKILL, CEOF, CEOL,
	  CEOL2, CSWTCH, CSTART, CSTOP, CSUSP, CDSUSP,
	  CRPRNT, CFLUSH, CWERASE, CLNEXT, CNUL,
     };

     /* load values individually for safety between SUNOS revisions */
     sane.c_iflag = IGNPAR|ICRNL|BRKINT|ISTRIP|IXON|IMAXBEL;
     sane.c_oflag = OPOST|ONLCR;
     sane.c_cflag = B9600|CS7|PARENB|CREAD;
     sane.c_lflag = ISIG|ICANON|ECHO|ECHOE|ECHOK|ECHOCTL|ECHOKE|IEXTEN;
     sane.c_line = 2;
     bcopy(cchars, sane.c_cc, sizeof(cchars));
     return (Generic)&sane;
}

/* Whap some saved values onto the tty */
Export void
tty_set_values(w, val)
TermWidget w;
Generic val;
{
     /* posix posix posix */
     posix_tty_set_values(w, val);
}

#endif	/* _TTY_SUN_INCLUDE */
