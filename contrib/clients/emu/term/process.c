#ifndef lint
static char *rcsid = "$Header: /usr/src/local/emu/term/RCS/process.c,v 1.15 91/09/30 18:07:34 me Exp Locker: jkh $";
#endif

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
 * Various process manipulation routines.
 *
 * Author: Jordan K. Hubbard
 * Date: March 20th, 1990.
 * Description: Here are all the public routines for frobbing the state of
 *		of the main process.
 *
 * Revision History:
 *
 * $Log:	process.c,v $
 * Revision 1.15  91/09/30  18:07:34  me
 * got rid of some compiler warnings in R5
 * 
 * Revision 1.14  91/08/17  10:16:44  jkh
 * Simplified the setup code somewhat.
 * 
 * Revision 1.13  91/07/11  17:31:32  tom
 * To quiet the broken MIPS cpp
 * 
 * Revision 1.12  91/07/05  13:56:45  jkh
 * Whatever I did.
 * 
 * Revision 1.11  90/11/20  17:45:23  jkh
 * Alpha.
 * 
 * Revision 1.10  90/10/12  13:44:37  jkh
 * Renaming and reorganization of system dependent stuff.
 * 
 * Revision 1.1  90/06/26  19:11:23  jkh
 * Initial revision
 * 
 *
 */

#include "TermP.h"
#include "TermCanvas.h"

/*
 * Since different systems vary widely in their handling of handling 
 * processes, each system type has its own file. If you can get by with
 * making only subtle tweaks to an existing file, then by all means
 * #ifdef them in, but DO NOT make major changes in this way; create
 * another file instead. I am trying to avoid the crazed proliferation
 * of #ifdefs that makes so much of the original xterm code unreadable.
 * Thanks. -jkh
 *
 * I use "#if defined(xxx)" instead of "#ifdef xxx" so that you can easily
 * append new conditionals for systems that share existing .i files. Sticking
 * to this convention is encouraged. I hope your pre-processor understands
 * #elif.
 *
 */

#if defined(PCS)
#     include "sysdep/process_pcs.i"
#else
#     if defined(mips)
#	   include "sysdep/process_mips.i"
#     else
#	   if defined(sun)
#		include "sysdep/process_sun.i"
#	   else
#		include "sysdep/unknown.i"
#	   endif
#     endif
#endif

/* System independent (hopefully) work procs */

/*
 * Child death handler. Should be largely (UNIX) system independent, but
 * those with really specialized needs are free to either #ifdef this or
 * move it into an appropriate process_<sys>.i file.
 */
Local void
process_reaper()
{
     int failcode, pid;
     TermWidget w;

     /* Ok, who croaked? */
     pid = process_wait(&failcode);

     /* Try and find a widget associated with this pid */
     if ((w = XpNfindWidgetByPid(pid)) != NULL) {
	  XpNremoveRememberedWidget(w);

	  /* Pre-process fail code for special cases */
	  switch (failcode >> 8) {
	  case PROC_EXEC_FAILED:
	       warn("Couldn't exec %s", w->term.command);
	       break;

	  case PROC_TTYOPEN_FAILED:
	       warn("Sub-process couldn't open /dev/tty");
	       break;

	  default:
	       if (failcode & 0xff == 0177)
		    warn("Process %s caught signal %d",
			 w->term.command, failcode >> 8);
	  }
	  /*
	   * We set the pid() to zero first so that any subsequent calls
	   * to Destroy() won't result in a gratuitous signal to a dead proc.
	   */
	  w->term.pid = 0;

	  /* Avoid spurious errors */
	  tty_close(w);

	  /* Call process cleanup code */
	  process_cleanup(w);

	  /* let the destroy handler do the rest */
	  process_destroy(w, failcode);
     }
     /*
      * If we couldn't find the process, just fall through. Note that this
      * is eminently possible since we use popen() in a few places which
      * will generate spurious calls to this routine.
      */
#ifdef SYSV
     /* Reset for SYSV braindeath */
     signal(SIGCHLD, process_reaper);
#endif
}

Import TermClassRec termClassRec;

/* Create the process */
Export void
process_create(w)
TermWidget w;
{
     /* Make sure the class signal handler gets invoked */
     signal(SIGCHLD, process_reaper);
     ++termClassRec.term_class.sig_cnt;

     /* Call the system dependent routine to actually get the work done */
     process_init(w);

     /*
      * It's really kludge to init the canvas from here, but it solves
      * number of problems with making the client do it. This is only
      * for the very first initialization, subsequent calls are done
      * automatically by parser_install().
      */
     XpTermDoRop(w, ROP_INIT_CANVAS);
}

/*
 * Nuke the process. "Fail" will be set to the process's return code
 * if this routine is being called from the class's SIGCHLD signal handler.
 */

Export void
process_destroy(w, fail)
TermWidget w;
int fail;
{
     /* Call any callbacks we have for child deletion */
     XtCallCallbackList((Widget)w, w->term.proc_death, (XtPointer)fail);

     /*
      * Let the widget Destroy handler do most of the memory cleanup work
      * so we don't have to dup the code.
      */
     XtDestroyWidget((Widget)w);

     /* If we don't need it anymore, remove it */
     if (!--termClassRec.term_class.sig_cnt)
	  signal(SIGCHLD, SIG_DFL);
}
     
Export void
process_signal(w, sig)
TermWidget w;
int sig;
{
     Import int kill();

     if (w->term.pid)
	  kill(w->term.pid, sig);
}
