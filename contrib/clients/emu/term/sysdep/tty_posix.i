/* -*-Mode: C; -*- */

#ifndef _TTY_POSIX_INCLUDE
#define _TTY_POSIX_INCLUDE

/* $Header: /usr3/Src/emu/term/sysdep/RCS/tty_posix.i,v 1.1 91/09/30 21:35:26 jkh Exp Locker: jkh $ */

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
 * Tty service routines for generic POSIX.1 compliant systems.
 *
 * Author: Jordan K. Hubbard
 * Date: August 26th, 1991
 * Description: Various routines for dealing with ttys that are generic to
 *		POSIX compliant systems (in theory).
 *
 * Revision History:
 *
 * $Log:	tty_posix.i,v $
 * Revision 1.1  91/09/30  21:35:26  jkh
 * Initial revision
 * 
 * 
 * 
 */

/*
 * These next three routines complement eachother by getting and setting
 * some number of tty values using the TtyStuff structure.
 */

#include <termios.h>

typedef struct termios TtyStuff;

/* File descriptors to try */
Local int Fd_try[] = { 0, 1, 2, -1, 0, -1 };

/* If w is NULL, get tty values for invoking tty, not pty */
Local Generic
posix_tty_get_values(w)
TermWidget w;
{
     register int i, *try;
     static TtyStuff ret;
     Import int errno;

     if (w == NULL)
	  try = Fd_try;
     else {
	  try = Fd_try + 4;
	  try[0] = w->term.slave;
     }
     for (i = 0; try[i] >= 0; i++) {
	  int r;

	  r = tcgetattr(try[i], &ret);
	  if (!r)	/* success? */
	       break;
     }

     if (try[i] == -1)
	  warn("posix_tty_get_values: Couldn't tcgetattr, errno = %d", errno);
     return (Generic)&ret;
}

/* Must return some "sane" set of hardwired values */
Local Generic
posix_tty_get_sane(w)
TermWidget w;
{
     static struct termios sane;
     static char cchars[] = {
	  CINTR, CQUIT, CERASE, CKILL, CEOF, CEOL,
#ifdef CEOL2
	  CEOL2,
#else
	  CNUL,
#endif
#ifdef CSWTCH
	  CSWTCH,
#else
	  CNUL,
#endif
	  CSTART, CSTOP,
#ifdef CSUSP
	  CSUSP,
#else
	  CNUL,
#endif
     };

     /*
      * load values individually rather than in static struct since
      * the order of the members *doesn't* seem to be something that
      * all posix systems agree on.
      */
     sane.c_iflag = IGNPAR|ICRNL|BRKINT|ISTRIP|IXON;
     sane.c_oflag = OPOST|ONLCR;
     sane.c_cflag = B9600|CS7|PARENB|CREAD;
     sane.c_lflag = ISIG|ICANON|ECHO|ECHOE|ECHOK|ECHONL;
     sane.c_line = 2;
     bcopy(cchars, sane.c_cc, sizeof(cchars));
     return (Generic)&sane;
}

/* Whap some saved values onto the tty */
Local void
posix_tty_set_values(w, val)
TermWidget w;
Generic val;
{

     register int i, *try;
     Import int errno;

     if (w == NULL)
	  try = Fd_try;
     else {
	  try = Fd_try + 4;
	  try[0] = w->term.slave;
     }
     for (i = 0; try[i] >= 0; i++) {
	  int r;
	  r = tcsetattr(try[i], TCSADRAIN,(char *)val);
	  if (!r)	/* success? */
	       break;
     }
     if (try[i] == -1)
	  warn("posix_tty_set_values: Couldn't tcsetattr, errno = %d.", errno);
}

#endif	/* _TTY_POSIX_INCLUDE */
