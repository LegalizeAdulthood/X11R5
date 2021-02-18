/* -*-Mode: C; -*- */

#ifndef _TTY_MIPS_INCLUDE
#define _TTY_MIPS_INCLUDE

/* $Header: /usr3/Src/emu/term/sysdep/RCS/tty_mips.i,v 1.1 91/09/30 21:35:22 jkh Exp $ */

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
 * Tty allocation/deallocation routines for MIPS systems.
 *
 * Author: Jordan K. Hubbard
 * Date: August 22nd, 1991
 * Description: Here are all the routines for getting and setting tty
 *		parameters on MIPS and PMAX systems.
 *
 * $Log:	tty_mips.i,v $
Revision 1.1  91/09/30  21:35:22  jkh
Initial revision

 * 
 */

/* Some is BSD, some is POSIX */
#include "sysdep/tty_bsd.i"
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
     /* Use the posix tty handling */
     return posix_tty_get_values(w);
}

/* Must return some "sane" set of hardwired values */
Export Generic
tty_get_sane(w)
TermWidget w;
{
     /* Use the posix sane values */
     return posix_tty_get_sane(w);
}

/* Whap some saved values onto the tty */
Export void
tty_set_values(w, val)
TermWidget w;
Generic val;
{
     /* Use the posix tty handling */
     posix_tty_set_values(w, val);
}

#endif	/* _TTY_MIPS_INCLUDE */
