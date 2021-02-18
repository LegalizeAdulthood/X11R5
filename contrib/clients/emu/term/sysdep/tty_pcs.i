/* -*-Mode: C; -*- */

#ifndef _TTY_PCS_INCLUDE
#define _TTY_PCS_INCLUDE

/* $Header: /usr3/Src/emu/term/sysdep/RCS/tty_pcs.i,v 1.3 91/09/30 21:36:31 jkh Exp $ */

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
 * Tty manipulation routines for PCS machines.
 *
 * Author: Jordan K. Hubbard
 * Date: March 20th, 1990.
 * Description: Various routines for tweaking ttys on PCS/Cadmus systems
 *		that the Emu system expects us to declare.
 *
 * Revision History:
 *
 * $Log:	tty_pcs.i,v $
 * Revision 1.3  91/09/30  21:36:31  jkh
 * Decwrl change set.
 * 
 * Revision 1.2  1991/08/17  10:16:09  jkh
 * General reorganization, changed declarations for TAGS.
 *
 * Revision 1.4  90/11/20  17:45:31  jkh
 * Alpha.
 * 
 * Revision 1.3  90/11/13  14:59:31  jkh
 * Double descriptor version.
 * 
 * Revision 1.1  90/10/08  18:19:14  jkh
 * Initial revision
 * 
 */

/* Include the sysv revision 3 stuff */
#include "sysdep/tty_sysvr3.i"

Local void
tty_set_nonblocking(w, fd)
TermWidget w;
int fd;
{
     /* Generic SYSV does what we want - just inherit it */
     sysv_tty_set_nonblocking(w, fd);
}

/*
 * Search for an available pty, putting descriptor in passed "descr" and
 * returning name, if found. Since we're only called once (better be), it's
 * OK to stick this name in static storage.
 *
 * Provided that the #defines for PTY_ITERATION/PTYCHAR1/.. are done
 * properly for your system in os.h, this should be reasonably system
 * independent, meaning you may be able to just grab this intact for your
 * system. It is system dependent nonetheless, so it goes in this file
 * rather than tty.c.
 *
 * The process code is expected to take care of the hairy mode/process
 * group/controlling tty/etc. setup stuff later when the child is forked
 * off, so we don't do it now.
 *
 */
Export String
tty_find_pty(w)
TermWidget w;
{
     /* Generic unix pty finding code works for us */
     return unix_tty_find_pty(w);
}

/* Set a new tty size */
Export void
tty_set_size(w, rows, cols, width, height)
TermWidget w;
int rows, cols, width, height;
{
     /* Generic sysv does what we want */
     sysv_tty_set_size(w, rows, cols, width, height);
}

/* Add a utmp/wtmp entry for a tty */
Export void tty_add_utmp(w)
TermWidget w;
{
     /* Generic SYSV does what we want */
     sysv_tty_add_utmp(w);
}

/* Remove a utmp/wtmp entry for a tty */
Export void tty_remove_utmp(w)
TermWidget w;
{
     /* Generic SYSV does what we want */
     sysv_tty_remove_utmp(w);
}

/*
 * These next three routines complement eachother by getting and setting
 * some number of tty values using a private (and probably machine dependent)
 * format. It is up to the implementor of these routines to devise some
 * suitable data structure for getting, setting and properly storing said
 * values for their particular operating system. The term widget will
 * encode these values as "generic" and leave it to the implementor to keep
 * them straight.
 *
 */

/* Get the current settings for tty */
Export Generic
tty_get_values(w)
TermWidget w;
{
     /* sysvr3 code does what we want */
     return sysvr3_tty_get_values(w);
}

/* Must return some "sane" set of hardwired values */
Export Generic
tty_get_sane(w)
TermWidget w;
{
     /* sysvr3 code does what we want */
     return sysvr3_tty_get_sane(w);
}

/* Whap some settings onto the tty */
Export void
tty_set_values(w, val)
TermWidget w;
Generic val;
{
     /* sysvr3 code does what we want */
     sysvr3_tty_set_values(w, val);
}

#endif	/* _TTY_PCS_INCLUDE */
