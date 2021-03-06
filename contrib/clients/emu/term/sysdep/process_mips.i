/* -*-Mode: C; -*- */

#ifndef _PROCESS_MIPS_INCLUDE
#define _PROCESS_MIPS_INCLUDE

/* $Header: /usr3/Src/emu/term/sysdep/RCS/process_mips.i,v 1.1 91/09/30 21:35:07 jkh Exp $ */

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
 * Various process manipulation routines specific to mips systems.
 *
 * Author: Jordan K. Hubbard
 * Date: August 22nd, 1991
 * Description: Here are all the MIPS / Ultrix 4.0 specific routines for
 *		frobbing the state of of the main process.
 *
 *
 * $Log:	process_mips.i,v $
Revision 1.1  91/09/30  21:35:07  jkh
Initial revision

 *
 */

/* MIPS and Ultrix PMAX machines are close enough to BSD */
#include "sysdep/process_bsd.i"

/*
 * These next two Exported routines, process_wait() and process_init(), are
 * expected of us. The rest is up to our organizational tastes.
 */

/* Mourn for the recently deceased */
Export int
process_wait(fc)
int *fc;
{
     /* generic unix routine does what we want */
     return unix_process_wait(fc);
}

Export void
process_cleanup(w)
TermWidget w;
{
     /* Just remove the utmp entry */
     tty_remove_utmp(w);
}

Export void
process_init(w)
TermWidget w;
{
     /* record uid and gid values for later use */
     w->term.uid = getuid();
     w->term.gid = getgid();

     /* generic BSD is good for us */
     bsd_process_init(w);
}

#endif	/* _PROCESS_MIPS_INCLUDE */
