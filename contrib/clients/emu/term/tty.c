/* $Header: /usr3/Src/emu/term/RCS/tty.c,v 1.10 91/07/11 17:32:50 tom Exp Locker: jkh $ */

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
 * Tty allocation/deallocation routines.
 *
 * Author: Jordan K. Hubbard
 * Date: March 20th, 1990.
 * Description: This file conditionally includes various machine specific
 *		chunks of code for dealing with the tty for the tty
 *
 * Revision History:
 *
 * $Log:	tty.c,v $
 * Revision 1.10  91/07/11  17:32:50  tom
 * To quiet the broken MIPS cpp
 * 
 * Revision 1.9  91/07/05  13:07:40  me
 * Whatever Mr. Hubbard did
 * 
 * Revision 1.8  90/11/20  17:45:27  jkh
 * Alpha.
 * 
 * Revision 1.7  90/11/13  14:54:22  jkh
 * double fd version.
 * 
 * Revision 1.1  90/06/26  19:11:32  jkh
 * Initial revision
 *
 */

#include "TermP.h"

/*
 * Since different systems vary widely in their handling of getting and
 * setting tty attributes, each system type has its own file. If you
 * can get by with making only subtle tweaks to an existing file, then
 * by all means #ifdef them in, but DO NOT make major changes in this way -
 * create another file instead. I am trying to avoid the crazed proliferation
 * of #ifdefs that makes so much of the original xterm code unreadable.
 * Thanks. -jkh
 *
 * I use "#if defined(xxx)" instead of "#ifdef xxx" so that you can easily
 * append new conditionals for systems that share existing .i files. Sticking
 * to this convention is encouraged.
 *
 */

/* Define these if you support the newer POSIX tc{get,set}attr() calls */
#ifdef _POSIX_SOURCE
#define TCGETATTR
#define TCSETATTR
#endif

#if defined(PCS)
#     include "sysdep/tty_pcs.i"
#else
#     if defined(mips)
#	   include "sysdep/tty_mips.i"
#     else
#          if defined(sun)
#	        include "sysdep/tty_sun.i"
#          else
#	        include "sysdep/unknown.i"
#          endif
#     endif
#endif

/*
 * What follows should be reasonably system independent. If there's something
 * here that needs radical munging for your system, move it into a .i file
 * and tell me why you needed to do it; I'll then see whether or not there's
 * a more general solution for more than just your machine. -jkh
 */

/* Open a PTY and initialize it */
Export Boolean
tty_open(w)
TermWidget w;
{
     if (!(w->term.tname = tty_find_pty(w)))
	  return FALSE;
     else {
	  /* We know by now that there is at least one input callback */
	  w->term.xi = XtAddInput((int)w->term.master,
				  (XtPointer)XtInputReadMask,
				  (XtInputCallbackProc)iparse,
				  (XtPointer)w);
	  /* Output callbacks are always optional */
	  if (w->term.out_parse)
	       w->term.xo = XtAddInput((int)w->term.master,
				       (XtPointer)XtInputWriteMask,
				       (XtInputCallbackProc)oparse,
				       (XtPointer)w);
     }
     return TRUE;
}

Export void
tty_close(w)
TermWidget w;
{
     if (w->term.xi)
	  XtRemoveInput(w->term.xi);
     if (w->term.xo)
	  XtRemoveInput(w->term.xo);

     if (w->term.master >= 0)
	  close(w->term.master);

     if (w->term.slave >= 0)
	  close(w->term.slave);

     w->term.xi = w->term.xo = (XtInputId)NULL;
     w->term.master = w->term.slave = -1;
}
