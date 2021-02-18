/*
 * xmail - X window system interface to the mail program
 *
 * Copyright 1989 The University of Texas at Austin
 *
 * Author:	Po Cheung
 * Date:	March 10, 1989
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.  The University of Texas at Austin makes no 
 * representations about the suitability of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
 *
 * Copyright 1990,1991 by National Semiconductor Corporation
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of National Semiconductor Corporation not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * NATIONAL SEMICONDUCTOR CORPORATION MAKES NO REPRESENTATIONS ABOUT THE
 * SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS"
 * WITHOUT EXPRESS OR IMPLIED WARRANTY.  NATIONAL SEMICONDUCTOR CORPORATION
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO
 * EVENT SHALL NATIONAL SEMICONDUCTOR CORPORATION BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Michael C. Wagnitz - National Semiconductor Corporation
 *
**  Xmail talks to mail through a pseudo terminal which is a pair of master
**  and slave devices: /dev/pty?? and /dev/tty??, where ?? goes from p0 to
**  zf (system dependent).  The terminal is opened for both read and write.
**
*/

#include	"global.h"
#include	<sys/stat.h>
#include	<signal.h>

#if	!defined(SYSV) || defined(clipper)
#include	<sgtty.h>
#else
#include	<sys/termio.h>
#if	defined(att)
#include	<sys/sysmacros.h>
#include	<fcntl.h>
#include	<sys/stropts.h>
#endif
#endif

#ifndef TTYDEV
#ifdef		hpux
#define TTYDEV		"/dev/pty/ptyxx"
#else	/* !hpux */
#define	TTYDEV		"/dev/ptyxx"
#endif	/* !hpux */
#endif	/* !TTYDEV */

#ifndef PTYCHAR1
#ifdef		hpux
#define PTYCHAR1	"zyxwvutsrqp"
#else	/* !hpux */
#define	PTYCHAR1	"pqrstuvwxyz"
#endif	/* !hpux */
#endif	/* !PTYCHAR1 */

#ifndef PTYCHAR2
#ifdef		hpux
#define	PTYCHAR2	"fedcba9876543210"
#else	/* !hpux */
#define	PTYCHAR2	"0123456789abcdef"
#endif	/* !hpux */
#endif	/* !PTYCHAR2 */

int    	    	mail_fd;		/* mail process master tty id */
int    	    	mailpid;		/* mail process id */
int    	    	mailInputId;		/* mail input id */
char		pseudo_tty[20];


/*
** @(#) openMaster - searches for and opens a pty master.  If it finds one,
**		     it returns the value of the file descriptor.  If not,
**		     it reports an error and terminates.  Portions of this
**		     routine were stolen from X11R4 xterm get_pty() sources.
*/
int
openMaster()
{
 static int	devindex = 0;
 static int	letter = 0;
 struct	stat	st_buf;
 int		master;

#ifdef att
 (void) strcpy(pseudo_tty, "/dev/ptmx");
 if ((master = open(pseudo_tty, O_RDWR)) >= 0)
    return(master);
#else /* !att, need lots of code */
#if defined(umips) && defined (SYSV)
 int		c;

 (void) strcpy(pseudo_tty, "/dev/ptc");
 master = open(pseudo_tty, O_RDWR);
 if (master >= 0 && (fstat(master, &st_buf)) == 0) {
    (void) sprintf(pseudo_tty, "/dev/ttyq%d", minor(st_buf.st_rdev));
    if ((c = open(pseudo_tty, O_RDWR)) < 0) {
       (void) close (master);
      } else {				/* got one! */
       (void) close (c);
       return(master);
      }
   }
#else /* not (umips && SYSV) */
 (void) strcpy(pseudo_tty, TTYDEV);
 while (PTYCHAR1[letter]) {
       pseudo_tty[strlen(pseudo_tty) - 2] = PTYCHAR1[letter];
       pseudo_tty[strlen(pseudo_tty) - 1] = PTYCHAR2[0];

       if (stat(pseudo_tty, &st_buf) == 0)
          while (PTYCHAR2[devindex]) {
                pseudo_tty[strlen(pseudo_tty) - 1] = PTYCHAR2[devindex];
                if ((master = open(pseudo_tty, O_RDWR)) >= 0)
                   return(master);

                devindex++;
               }
       devindex = 0;
       (void) letter++;
      }
#endif /* umips && SYSV */
#endif /* att */
/*
** We were unable to allocate a pty master!  Report the error and terminate.
*/
 XtError("xmail cannot open master/slave pipe connection");
 /* NOTREACHED */
} /* end - openMaster */


/*
** @(#)openSlave() - open slave side of pipe
*/
/* ARGSUSED */
int
openSlave(master)
int	master;
{
 int		slave;

#ifdef att
 char		*slaveName;
 int		grantpt(), unlockpt();
 char		*ptsname();

 if (grantpt(master) >= 0 &&
    unlockpt(master) >= 0 &&
    (slaveName = ptsname(master)) != NULL &&
    (slave = open(slaveName, O_RDWR)) >= 0 &&
    (void) ioctl(slave, I_PUSH, "ptem") >= 0 &&
    (void) ioctl(slave, I_PUSH, "ldterm") >= 0)
    return(slave);
#else /* !att */
#if defined(umips) && defined (SYSV)
 struct	stat	st_buf;

 fstat(master, &st_buf);
 (void) sprintf(pseudo_tty, "/dev/ttyq%d", minor(st_buf.st_rdev));
 if ((slave = open(pseudo_tty, O_RDWR)) >= 0)
    return(slave);				/* got one! */
#else /* not (umips && SYSV) */
 pseudo_tty[strlen(pseudo_tty) - 5] = 't';
 if ((slave = open(pseudo_tty, O_RDWR)) >= 0)
    return(slave);
#endif
#endif
 return(-1);				/* look for more master/slave pairs */
} /* openSlave */


/*
** @(#)callMail() - fork child to execute mail and attach to xmail input
*/
/* ARGSUSED */
void
callMail(argv)
char *argv[];
{
#if defined(SYSV)  && !defined(clipper)
 struct termio	tio;
#else	
 struct sgttyb	Sgtty;
#endif
 int		slave;			/* file descriptor to slave pty */
 char		*Mailpgm;		/* name of executable Mailpgm */


 if (! (Mailpgm = (char *)getenv("XMAILER")))	/* first looks up env var */
    Mailpgm = DEFAULT_MAILER;
  
 for (;;) {				/* until we find a pair, or master fails */
     mail_fd = openMaster();
     if ((slave = openSlave(mail_fd)) != -1)
        break;
    }
/*
** Set minimal requirements for slave connection (no echo, no NL->CR, keep TABS)
*/
#if defined(SYSV) && !defined(clipper)
 (void) ioctl(slave, TCGETA, &tio);
 tio.c_oflag &= ~(OCRNL|ONLRET|TABDLY);
 tio.c_oflag &= ICRNL;
 tio.c_lflag &= ~(ISIG|ECHO|IXOFF);
 (void) ioctl(slave, TCSETA, &tio);
#else	
 (void) ioctl(slave, TIOCGETP, &Sgtty);
 Sgtty.sg_flags &= ~(ECHO|CRMOD|XTABS);
 (void) ioctl(slave, TIOCSETP, &Sgtty);
#endif

 mailpid = fork();
 if (mailpid == -1) {
    XtError("callMail could not fork the child process");
   } else if (mailpid) { 
             /* 
              * Parent : close the slave side of pty
              *          set the mail file descriptor to append mode
              *          register mail input with X
              */
             (void) close(slave);
             (void) fcntl(mail_fd, F_SETFL, FAPPEND);
             mailInputId = XtAddInput(mail_fd, XtInputReadMask, readMail, NULL);
            } else { 
             /* 
              * Child : close X connection and mail_fd side of pty
              *         redirect stdin, stdout, stderr of mail to pty
              *         unbuffer output data from mail
              *         exec mail with arguments
              *
              *         Use a select() call to delay starting the mail process
              *         until our parent can close its slave side of the pipe.
              *         Eliminates the parent hanging (for approximately 15
              *         seconds) on the close because the child terminated for
              *         lack of mail before the parent could issue its close.
              */
             int		readfds, width;
             struct timeval	timeout;

             (void) close(ConnectionNumber(XtDisplay(toplevel)));

             timeout.tv_sec = 0; timeout.tv_usec = 250000; /* 0.25 seconds */
             readfds = 1 << slave; width = slave + 1;

             (void) select(width, (fd_set *) &readfds, (fd_set *) NULL, (fd_set *) NULL, &timeout);

             (void) close(mail_fd);
             (void) dup2(slave, 0);
             (void) dup2(slave, 1);
             (void) dup2(slave, 2);
             if (slave > 2)
                 (void) close(slave);
             (void) fcntl(1, F_SETFL, FAPPEND);
             setbuf(stdout, (char *) NULL);
             argv[0] = Mailpgm;
             (void) execvp(Mailpgm, argv);
             /*
              * If we fail to make contact, we must re-establish access to
              * the terminal screen that started us for our error message,
              * because we don't want to send it up the xmail pipe.
              * Also terminate our parent.
              */
             if ((slave = open("/dev/tty", O_RDWR)) != -1) {
                (void) dup2(slave, 1);
                (void) dup2(slave, 2);
                perror(Mailpgm);
               }
             (void) kill(getppid(), SIGKILL);		/* kill our parent */
             (void) exit(1);
             /* NOTREACHED */
            }
} /* callMail */
