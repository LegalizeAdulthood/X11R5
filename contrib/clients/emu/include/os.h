#ifndef _OS_H_INCLUDE
#define _OS_H_INCLUDE

/* $Header: /usr3/emu/include/RCS/os.h,v 1.8 90/11/20 17:35:58 jkh Exp Locker: jkh $ */

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
 * Operating system specific interface.
 *
 * Author: Jordan K. Hubbard and Digital Equipment Corporation
 * Date: March 20th, 1990.
 * Description: This file contains (or includes) all OS specific portions
 *		of emu. Make any OS specific changes to (and only to)
 *		this file if you absolutely have to, but you probably won't
 *		even want to look at it. I just LOVE all the different Unix's
 *		we have out there these days. Wish we had a few more.
 *
 * Revision History:
 *
 * $Log:	os.h,v $
 * Revision 1.8  90/11/20  17:35:58  jkh
 * Alpha
 * 
 *
 */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xos.h>

#ifdef SYSTYPE_SYSV
#define SYSV
#endif

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>

#ifdef	sun
#include <sys/ttydev.h>
#include <sys/termio.h>
#include <sys/filio.h>
#include <sys/sockio.h>
#else
#include <sys/ioctl.h>
#endif	/* sun */

#ifdef PCS
#include <sys/time.h>	/* for timeval */

/* This doesn't get defined on PCS systems */
typedef int		pid_t;
#endif

/* #include <mtrace.h> */

/* Our generic type */
typedef caddr_t Generic;

/*
 * If you have an alloca, define this. If you have GCC, this will be
 * defined automatically.
 */

#if defined(PCS) && !defined(PCSFBOX)
#define HAVE_ALLOCA
#endif /* !defined(PCSFBOX) */

#ifdef __GNUC__
#define alloca __builtin_alloca
#define Inline __inline__
#ifndef HAVE_ALLOCA	/* If it isn't already defined */
#define HAVE_ALLOCA
#endif
#else
#ifdef HAVE_ALLOCA
#include <alloca.h>
#endif
#define Inline
#endif

/*
 * Constants used by bitstream.h.
 * Adjust to fit your machine accordingly.
 */

/* how many bits in the unit returned by sizeof ? */
#define	_bit_charsize	8

/* what type will the bitstring be an array of ? */
#define	_bit_type	unsigned int

/* how many bits in an int ? */
#define	_bit_intsiz	(sizeof(_bit_type) * _bit_charsize)

/* an int of all '0' bits */
#define	_bit_0s		((_bit_type)0)

/* an int of all '1' bits */
#define	_bit_1s		((_bit_type)~0)

#define Fresh(type) ((type *)malloc_and_clear(sizeof(type)))

/* Where debugging output goes */
#define DEBUGFILE	"/tmp/emu.log"

/* predefined subprocess exit codes */
#define PROC_EXEC_FAILED	-4
#define PROC_TTYOPEN_FAILED	-5

/*
 * We deliberately avoid the use of the simpler #ifdef and #ifndef forms
 * here simply because it is easier to come by later and add additional
 * clauses when needed.
 */

#if !defined(PCSFBOX) && !defined(mips)
#ifndef tolower
extern char tolower();
#endif

#ifndef toupper
extern char toupper();
#endif
#endif /* !PCSFBOX && !mips */

#if !defined(NFILES)
#define NFILES	64	/* take a wild guess */
#endif

#if defined(SYSV)
#if defined(PCS)
#define killpg(x, sig)	kill(0, sig)
#else
#define	killpg(x,sig)	kill(-x,sig)
#endif /* PCS */
#if !defined(CRAY)
#define	dup2(fd1,fd2)	((fd1 == fd2) ? fd1 : \
			 (close(fd2), fcntl(fd1, F_DUPFD, fd2)))
#endif
#endif	/* SYSV */

#if defined(SYSV)
#include <memory.h>
#define bzero(s, n) memset(s, 0, n)
#ifndef __GNUC__
#define bcopy(f, t, n) memcpy(t, f, n)
#endif
#else
Import char *bcopy(), *memcopy();
#endif /* SYSV */

#if (defined(SYSV) || defined(USG)) && !defined(HAVE_VFORK)
#define vfork()	fork()
#endif

#if !defined (SIGCHLD) && defined (SIGCLD)
#define SIGCHLD SIGCLD
#endif /* SIGCLD */


#if !defined(UTMP_FILENAME)
#define UTMP_FILENAME "/etc/utmp"
#endif
#if !defined(WTMP_FILENAME)
#if defined(SYSV)
#define WTMP_FILENAME "/etc/wtmp"
#else
#define WTMP_FILENAME "/usr/adm/wtmp"
#endif /* SYSV */
#endif /* !WTMP_FILENAME */

#if !defined(PTYDEV)
#if defined(hpux)
#define	PTYDEV		"/dev/ptym/pty%c%c"
#else	/* !hpux */
#define	PTYDEV		"/dev/pty%c%c"
#endif	/* !hpux */
#endif	/* !PTYDEV */

#if !defined(TTYDEV)
#if defined(hpux)
#define TTYDEV		"/dev/pty/tty%c%c"
#else	/* !hpux */
#define	TTYDEV		"/dev/tty%c%c"
#endif	/* !hpux */
#endif	/* !TTYDEV */

#if !defined(PTYCHAR1)
#if defined(hpux)
#define PTYCHAR1	"zyxwvutsrqp"
#else	/* !hpux */
#if defined(PCS)
#define PTYCHAR1        "qrstuvw"
#else   /* !hpux && !PCS */
#define	PTYCHAR1	"pqrstuvwxyz"
#endif  /* !PCS */
#endif	/* !hpux */
#endif	/* !PTYCHAR1 */

#if !defined(PTYCHAR2)
#if defined(hpux)
#define	PTYCHAR2	"fedcba9876543210"
#else	/* !hpux */
#define	PTYCHAR2	"0123456789abcdef"
#endif	/* !hpux */
#endif	/* !PTYCHAR2 */

#endif	/* _OS_H_INCLUDE */
