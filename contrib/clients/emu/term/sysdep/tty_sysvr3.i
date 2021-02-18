/* -*-Mode: C; -*- */

#ifndef _TTY_SYSVR3_INCLUDE
#define _TTY_SYSVR3_INCLUDE

/* $Header: /usr3/Src/emu/term/sysdep/RCS/tty_sysvr3.i,v 1.3 91/09/30 21:36:41 jkh Exp $ */

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
 * Tty manipulation routines for System V revision 3 machines.
 *
 * Author: Jordan K. Hubbard
 * Date: March 20th, 1990.
 * Description: Various System Vr3 dependent routines for dealing with ttys
 *		that the Emu system expects us to declare.
 *
 * Revision History:
 *
 * $Log:	tty_sysvr3.i,v $
 * Revision 1.3  91/09/30  21:36:41  jkh
 * Decwrl change set.
 * 
 * Revision 1.2  1991/08/17  10:16:19  jkh
 * General reorganization, changed declarations for TAGS.
 *
 * Revision 1.1  90/11/05  03:46:24  jkh
 * Initial revision
 * 
 * 
 */

/* Include the generic sysv code. */
#include "sysdep/tty_sysv.i"

/*
 * Get the tty values for a revision 3 system, the TtyStuff and try
 * structures being the same for all revisions of sysv, and so declared
 * by the generic sysv code.
 *
 * As per convention, if w is NULL, get tty values for invoking tty, not pty.
 */
Local Generic
sysvr3_tty_get_values(w)
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

	  r = ioctl(try[i], TCGETA, (char *)&ret);
	  if (!r)	/* success? */
	       break;
     }

     if (try[i] == -1)
	  warn("sysvr3_tty_get_values: Can't get terminal values, errno = %d",
	       errno);
     return (Generic)&ret;
}

/* Whap some values onto the tty */
Local void
sysvr3_tty_set_values(w, val)
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
          r = ioctl(try[i], TCSETA, (char *)val);
	  if (!r)	/* success? */
	       break;
     }
     if (try[i] == -1)
	  warn("tty_set_values: Couldn't TCSETS, errno = %d.", errno);
}

Local Generic
sysvr3_tty_get_sane(w)
TermWidget w;
{
     Local TtyStuff tio;
     
     /* Grab current values */
     bcopy(sysvr3_tty_get_values(w), &tio, sizeof(tio));

     /* Mung the important ones into a known state */
     /* input: nl->nl, don't ignore cr, cr->nl */
     tio.c_iflag &= ~(INLCR|IGNCR);
     tio.c_iflag |= ICRNL;
     /* ouput: cr->cr, nl is not return, no delays, ln->cr/nl */
     tio.c_oflag &= ~(OCRNL|ONLRET|NLDLY|CRDLY|TABDLY|BSDLY|VTDLY|FFDLY);
     tio.c_oflag |= ONLCR;
#ifdef BAUD_0
     /* baud rate is 0 (don't care) */
     tio.c_cflag &= ~(CBAUD);
#else	/* !BAUD_0 */
     /* baud rate is 9600 (nice default) */
     tio.c_cflag &= ~(CBAUD);
     tio.c_cflag |= B9600;
#endif	/* !BAUD_0 */
     /* enable signals, canonical processing (erase, kill, etc), echo */
     tio.c_lflag |= ISIG|ICANON|ECHO;
     /* reset EOL to defalult value */
     tio.c_cc[VEOL] = '@' & 0x3f;		/* '^@'	*/
     /* certain shells (ksh & csh) change EOF as well */
     tio.c_cc[VEOF] = 'D' & 0x3f;		/* '^D'	*/
     return (Generic)&tio;
}

#endif	/* _TTY_SYSVR3_INCLUDE */
