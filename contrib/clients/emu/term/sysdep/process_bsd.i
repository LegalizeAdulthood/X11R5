/* -*-Mode: C; -*- */

#ifndef _PROCESS_BSD_INCLUDE
#define _PROCESS_BSD_INCLUDE

/* $Header: /usr3/Src/emu/term/sysdep/RCS/process_bsd.i,v 1.1 91/09/30 21:34:58 jkh Exp Locker: jkh $ */

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
 * Various process manipulation routines specific to BSD systems.
 *
 * Author: Jordan K. Hubbard
 * Date: March 20th, 1990.
 * Description: Here are some routines that generic BSD systems should be
 *	 	able to use.
 *
 *
 * $Log:	process_bsd.i,v $
 * Revision 1.1  91/09/30  21:34:58  jkh
 * Initial revision
 * 
 *
 */

/* BSD was a UNIX variant last time we checked */
#include "sysdep/process_unix.i"

Local void
bsd_set_environment(w)
TermWidget w;
{
     char envterm[255], *tmp;
     Import void putenv();

     /* TERM */
     sprintf(envterm, "TERM=%s", w->term.term_type);
     tmp = XtNewString(envterm);
     putenv(tmp);
}

/* Fire one process torpedo */
Local void
bsd_fork_process(w)
TermWidget w;
{
     int i;
     Import void exit();
     Import int ioctl(), setpgrp(), fork(), open(), close(), dup();
     Import int chown(), chmod();
     Import int execve();
     Import int sigblock();
     int omask;
     int fd;

     /* Make sure we don't get the signal just yet */
     omask = sigblock(sigmask(SIGCHLD));
     
     if (!(w->term.pid = fork())) {	/* now running in junior */
	  char *args[2], **argp;

	  if ((fd = open("/dev/tty", O_RDWR)) >= 0)
	       ioctl(fd, TIOCNOTTY, 0);
	  
	  /* close everything in sight */
	  for (i = 0; i < NFILES; i++)
	       close(i);

	  setpgrp(0, 0);

	  /*
	   * Open the slave side for stdin and dup twice for stdout and stderr.
	   * We need to open all fd's read/write since some weird applications
	   * (like "more") READ from stdout/stderr or WRITE to stdin!
	   * Ack, bletch, barf.
	   *
	   */
          fd = open(w->term.tname, O_RDWR);		/* 0 */
          dup(fd);					/* 1 */
          dup(fd);					/* 2 */

	  /*
	   * Make the pty be the controlling terminal of the process.
	   * I hope.
	   */
#ifdef TIOCSCTTY
	  ioctl(0, TIOCSCTTY, 0);
#endif

	  signal(SIGCHLD, SIG_DFL);
	  
	  /* Time to release the weasels */
	  sigsetmask(omask);
	  
	  /* It's MINE, I tell you! Mine mine mine! */
	  chown(w->term.tname, w->term.uid, w->term.gid);
	  chmod(w->term.tname, 0622);

	  /* We might as well add the utmp entry now */
	  tty_add_utmp(w);

	  /* since we may be setuid root (for utmp), be safe */
	  setuid(w->term.uid);
	  setgid(w->term.gid);
#ifdef DEBUG
          debug("child: opened %s for stdin stdout stderr", w->term.tname);
          debug("child: about to exec %s.", w->term.command);
#endif
	  args[1] = NULL;
	  argp = w->term.command_args ? w->term.command_args : args;
	  argp[0] = w->term.command;
	  execvp(w->term.command, argp);
	  /*
	   * This will show up in the client window (hopefully), so
	   * sleep for some time before exiting, to let the user read
	   * the message
	   */
	  pmessage("Couldn't exec %s", args[0]);
	  sleep(5);
	  exit(PROC_EXEC_FAILED);
     }
     /*
      * Now that we have stored the widget ID, we can enable SIGCHLD
      * and deal with a possible early failure. 
      */
     sigsetmask(omask);
}

Local void
bsd_process_init(w)
TermWidget w;
{
     bsd_set_environment(w);
     bsd_fork_process(w);
     XpNrememberWidget(w);
}

#endif	/* _PROCESS_BSD_INCLUDE */
