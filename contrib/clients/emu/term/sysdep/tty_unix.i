/* -*-Mode: C; -*- */

#ifndef _TTY_UNIX_INCLUDE
#define _TTY_UNIX_INCLUDE

/* $Header: /usr3/Src/emu/term/sysdep/RCS/tty_unix.i,v 1.3 91/09/30 21:36:45 jkh Exp $ */

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
 * Tty manipulation routines for Generic UNIX machines.
 *
 * Author: Jordan K. Hubbard
 * Date: March 20th, 1990.
 * Description: Various UNIX dependent routines for dealing with ttys.
 *		These routines are typically used by more specific tty_<mx>.i
 *		files, where <mx> is some machine running a reasonably
 *		capable version of UNIX.
 *
 * Revision History:
 *
 * $Log:	tty_unix.i,v $
 * Revision 1.3  91/09/30  21:36:45  jkh
 * Decwrl change set.
 * 
 * Revision 1.2  1991/08/17  10:16:25  jkh
 * General reorganization, changed declarations for TAGS.
 *
 * Revision 1.1  91/08/16  06:42:09  jkh
 * Initial revision.
 * 
 */


/*
 * Search for an available pty, putting descriptor in passed "descr" and
 * returning name, if found.
 *
 * Provided that the #defines for PTY_ITERATION/PTYCHAR1/.. are done
 * properly for your system in os.h, this should be reasonably system
 * independent, meaning you may be able to just grab this intact for your
 * machine. It is UNIX dependent nonetheless, so it goes in this file
 * rather than tty.c.
 *
 * The process code is expected to take care of the hairy mode/process
 * group/controlling tty/etc. setup stuff later when the child is forked
 * off, so we don't do it now.
 *
 */

Export String
unix_tty_find_pty(w)
TermWidget w;
{
     char ttyname[80];
     struct stat st;
     register char *c1, *c2;
     Import int stat(), open(), access();
     Import void tty_set_nonblocking();
 
     /* Use user provided search loop or our own to grub through ptys */
#ifdef PTY_ITERATION
     PTY_ITERATION
#else
     for (c1 = PTYCHAR1; *c1; ++c1) {
	  for (c2 = PTYCHAR2; *c2; ++c2) {
#endif
	       /* Go for the master side */
	       sprintf(ttyname, PTYDEV, *c1, *c2);
#ifdef DEBUG
	       debug("unix_tty_find_pty: Trying to open %s", ttyname);
#endif
	       if (stat(ttyname, &st) < 0)
		    continue;
	       if ((w->term.master = open(ttyname, O_RDWR, 0)) >= 0) {
		    /*
		     * Now make sure that the slave side is available.
		     * This allows for a bug in some versions of rlogind
		     * and gives us a handle for ioctl() ops.
		     */
		    sprintf(ttyname, TTYDEV, *c1, *c2);
#ifdef DEBUG
		    debug("unix_tty_find_pty: Checking slave side %s",
			  ttyname);
#endif
		    if ((w->term.slave = open(ttyname, O_RDWR, 0)) >= 0) {
			 /* All is casual, dude. */
#ifdef DEBUG
			 debug("unix_tty_find_pty: Found pty %s", ttyname);
#endif
			 tty_set_nonblocking(w, w->term.slave);
			 return XtNewString(ttyname);
                    }
		    else
			 close(w->term.master);
	       }
	  }
     }
     pmessage("No free ptys.  Sorry mate.");
     return NULL;
}

#endif	/* _TTY_UNIX_INCLUDE */
