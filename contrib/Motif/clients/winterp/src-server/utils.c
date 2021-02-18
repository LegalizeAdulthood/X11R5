/* -*-C-*-
********************************************************************************
*
* File:         utils.c
* RCS:          $Header: utils.c,v 1.2 91/03/14 03:13:20 mayer Exp $
* Description:  random, non-X utility functions needed by WINTERP
* Author:       Niels Mayer, HPLabs
* Created:      Thu Oct 25 22:37:08 1990
* Modified:     Thu Oct  3 20:01:34 1991 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r5 contrib tape release
*
* WINTERP Copyright 1989, 1990, 1991 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
*
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: utils.c,v 1.2 91/03/14 03:13:20 mayer Exp $";


/*
 * I don't have time to figure out how to make sleepms() portable. sleepms()
 * is used by w_utils.c:Wut_Prim_X_REFRESH_DISPLAY(), which is a hack
 * bug workaround for some Motif/Xt/X anomalies.
 *
 * If you want to try to get this working on your non-HPUX system, feel free
 * to show me how. A few hints: use <X11/Xos.h> rather than <time.h>
 * change sigvector() to sigvec(), etc. I tried that, but ran into a bunch
 * of weird compiler errors. So I punted....
 */
#ifdef hpux

#include <stdio.h>
#include <signal.h>
#include <time.h>

static void alarmhandler()
{
}

/******************************************************************************
 * sleepms -- this procedure sleeps for <msec> milliseconds. It is similar to
 * sleep(3C).
 *
 * This procedure was stolen from some code written by 
 * Nathan Meyers, HP Interface Technology Operation
******************************************************************************/
sleepms(msec)
     int msec;
{
  struct itimerval value, ovalue;
  struct sigvec vec, ovec;
  long savemask, sigblock(), sigpause(), sigsetmask();

  vec.sv_handler = alarmhandler;
  vec.sv_mask = 0x0;
  vec.sv_flags = 0;
  sigvector(SIGALRM, &vec, &ovec); /* Set up alarmhandler for SIGALRM */
  savemask = sigblock((long)(1L << (SIGALRM - 1)));

  value.it_interval.tv_sec = 0;
  value.it_interval.tv_usec = 0;
  value.it_value.tv_sec = msec / 1000;
  value.it_value.tv_usec = (msec % 1000) * 1000;
  setitimer(ITIMER_REAL, &value, &ovalue);

  (void)sigpause(~(long)(1L << (SIGALRM - 1)));
  (void)sigsetmask(savemask);

  sigvector(SIGALRM, &ovec, NULL);	/* Restore previous signal handler */
}

#endif				/* hpux */
