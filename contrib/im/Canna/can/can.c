/*
	Copyright
		Kyoto University Research Institute for Mathematical Sciences
		1987, 1988, 1989
	Copyright OMRON TATEISI ELECTRONICS CO. 1987, 1988, 1989
	Copyright ASTEC, Inc. 1987, 1988, 1989


    Permission to use, copy, modify, and distribute this software
    and its documentation for any purpose and without any fee is
    hereby granted, subject to the following restrictions:

    The above copyright notice and this permission notice must appear
    in all versions of this software;

    The name of "Wnn" may not be changed; 

    All documentations of software based on "Wnn" must contain the wording
    "This software is based on the original version of Wnn developed by
    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
    OMRON TATEISI ELECTRONICS CO. and
    ASTEC, Inc.", followed by the above copyright notice;

    The name of KURIMS, OMRON and ASTEC may not be used
    for any purpose related to the marketing or advertising
    of any product based on this software.

    KURIMS, OMRON and ASTEC make no representations about
    the suitability of this software for any purpose.
    It is provided "as is" without express or implied warranty.

    KURIMS, OMRON and ASTEC DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
    SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
    IN NO EVENT SHALL KURIMS, OMRON AND ASTEC BE LIABLE FOR ANY SPECIAL,
    INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
    LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
    OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.

    Author: Hideki Tsuiki	Kyoto University
				tsuiki%kaba.or.jp@uunet.uu.net

	    Hiroshi Kuribayashi	OMRON TATEISI ELECTRONICS CO.
				kuri@frf.omron.co.jp
				uunet!nff.ncl.omron.co.jp!kuri

	    Naouki Nide		Kyoto University
				nide%kaba.or.jp@uunet.uu.net

	    Shozo Takeoka	ASTEC, Inc.
				take%astec.co.jp@uunet.uu.net

	    Takashi Suzuki	Advanced Software Technology & Mechatronics
				Research Institute of KYOTO
				suzuki%astem.or.jp@uunet.uu.net

*/
/* Copyright 1992 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of NEC
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  NEC Corporation makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */
/*
 * This sorftware is based on a Wnn's command "uum", that is, this is made
 * by modifying source code of "uum".
 */


#include "define.h"
#include <stdio.h>
#include <pwd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <iroha/jrkanji.h>

#ifdef	SVR2
#include <curses.h>
#include <term.h>
#include <sys/param.h>
#ifndef	SVR4
#define	SIGCHLD	SIGCLD
#endif	SVR4
#endif	SVR2

#ifdef	BSD
#include <sgtty.h>
#endif	BSD

char	*getenv();

extern	int	Term_RowWidth;
extern	int	Term_LineWidth;

#ifdef	SVR2
# define	getdtablesize()	(NOFILE)
# undef	SIGWINCH
struct termio savetmio;
struct termio term = {
    ICRNL | IXON,
    ONLCR | OPOST,
    B9600 | CS8 | CSTOPB | CREAD | CLOCAL,
    ISIG | ICANON | ECHO | ECHOE | ECHOK,
    0,
    { 0x3, 0x1c, 0x8, 0x15, 0x4, 0, 0, 0 }
  };
#endif	SVR2

#ifdef	BSD
 struct sgttyb	savetmio;
 struct tchars	tcharsv;
 struct ltchars	ltcharsv;
 struct sgttyb	ttyb;
 int		local_mode;
 int		local_mode_sv;
#endif	BSD

#ifdef	TIOCSSIZE
 struct ttysize pty_rowcol;
#endif	TIOCSSIZE

int	conv_lines	=	1;
int	is_opened	= 	0;
int	maxwidth	=	50;
int	crow;
int	ptyfd;
int	pty_bit;
int	sel_ptn;
int	ttyfd;
int	tty_bit;
short	pty_c_flag;
short	tty_c_flag;
char	TermName[32];
int	child_id;
char	*cmdnm = DEFAULT_SHELL;
int	flow_control = FLOW_CONTROL;
char	*ptynm = "/dev/pty";
int	ptyno;
int	ttypfd;
char	*ttypnm = "/dev/tty";


int
main(argc, argv)
     int	argc;
     char	**argv;
{
  char	nmbuf[20];
  char		*p;

  if (!isatty(0)) {
    fprintf(stderr,"can: Your terminal must be a tty.\n");
    exit(1);
  }
  if ((p = getenv(CAN_CHECK_ENV)) == NULL) {
    setenv(CAN_CHECK_ENV,"0");
  } else {
    fprintf(stderr, "can: 'can' already runnig. \n");
    sleep(2);
  } 

  if ((p = getenv("TERM")) == NULL) {
    fprintf(stderr, "can: Can't get your terminal name.\n");
    exit(1);
  } else {
    strcpy(TermName, p);
  }

#ifdef	BSD
  if (saveutmp() < 0)
    puts("Can't save utmp\n");
#endif	BSD

  save_signals();

  if (openTermData() == -1) {
    fprintf(stderr, "can: Something is wrong with terminfo, maybe.\n");
    exit(21);
  }
#ifdef	SVR2
  maxwidth = columns;
  crow = lines - conv_lines;
#endif	SVR2

#ifdef	BSD
  setsize();
#endif	BSD

  open_pty();
  open_ttyp();

  exec_cmd(argc, argv);

  ttyfd = 0;

  fcntl(ttyfd, F_SETFL, O_NDELAY);

  if (j_term_init() == ERROR) {
    fprintf(stderr, "can: term initialize fault.\n");
    exit(1);
  }

  pty_bit = 1 << ptyfd;
  tty_bit = 1 << ttyfd;
  sel_ptn = pty_bit | tty_bit;

  throw_cur_raw(0, 0);
  clr_end_screen();
  set_scroll_region(0, crow - 1);
  flush();

  romkan();

  close_terminal();
}

close_terminal()
{
  char	nmbuf[20];
  static int	do_end_flg = 0;

  if (do_end_flg == 1)
    return;
  else
    do_end_flg = 1;

  if (is_opened) 
    jrKanjiControl(0, KC_FINALIZE, NULL);
  save_cursor_status();
/*  throw_cur_raw(0, crow); */
  throw_cur_raw(0, 500);
  clr_end_screen();
  set_scroll_region(0, crow + conv_lines - 1);
  restore_cursor_status();
  flush();
#ifdef	SVR2
    closeTermData();
#endif	SVR2

  signal(SIGCHLD, SIG_DFL);
  fcntl(ttyfd, F_SETFL, 0);

#ifdef	BSD
  ioctl(ttyfd, TIOCSETP, &savetmio);
  ioctl(ttyfd, TIOCLSET, &local_mode_sv);
#endif	BSD
#ifdef	SVR2
  if (ioctl(ttyfd, TCSETA, &savetmio) < 0) {
    fprintf(stderr, "cab: error in ioctl TCSETA.\n");
    exit(1);
  }
#endif	SVR2

  ptyname(nmbuf, ptynm, ptyno);
  if (chown(nmbuf, 0, 0) == ERROR) {
    perror(nmbuf);
  }
  if (chmod(nmbuf, 0666) == ERROR) {
    perror(nmbuf);
  }
  ptyname(nmbuf, ttypnm, ptyno);
  if (chown(nmbuf, 0, 0) == ERROR) {
    perror(nmbuf);
  }
  if (chmod(nmbuf, 0666) == ERROR) {
    perror(nmbuf);
  }
  if (resetutmp(ttypfd) == ERROR) {
    printf("Can't reset utmp.");
  }

#ifdef	TIOCSSIZE
  pty_rowcol.ts_lines = 0;
  pty_rowcol.ts_cols = 0;
  ioctl(ttypfd, TIOCSSIZE, &pty_rowcol);
#endif	TIOCSSIZE

  close(ttyfd);
  close(ptyfd);

  chdir("/tmp");

#ifdef	SVR2
  kill(-child_id, SIGHUP);
#endif	SVR2
#ifdef	BSD
  killpg(child_id, SIGHUP);
#endif	BSD

  exit(0);
}

#ifdef BSD

setsize()
{
  register int	i;
  struct winsize	win;
  
  if (ioctl(ttyfd, TIOCGWINSZ, &win) < 0) {
    return;
  } else {
    if ((i = win.ws_row) != 0) {
      crow = Term_RowWidth = i - conv_lines;
    }
    if ((i = win.ws_col) != 0) {
      maxwidth = Term_LineWidth = i;
    }
  }
}

#endif BSD

open_pty()
{
  char		nmbuf[20];

  for (ptyno = 0; ptyno < 32; ptyno++) {
    ptyname(nmbuf, ptynm, ptyno);
    if ((ptyfd = open(nmbuf, O_RDWR, 0622)) != ERROR) {
      chown(nmbuf, getuid(), getgid());
      return;
    }
  }
  err("Can't get pty.");
}

open_ttyp()
{
  char		errbuf[256];
  char		nmbuf[20];
  int		word;
  int		pid;
#ifdef	SVR2
  struct termio		buf1;
#endif	SVR2

  ptyname(nmbuf, ttypnm, ptyno);
  if ((ttypfd = open(nmbuf, O_RDWR, 0622)) == ERROR) {
    sprintf(errbuf, "Can't open %s.", nmbuf);
    err(errbuf);
  }
  chown(nmbuf, getuid(), getgid());

#ifdef BSD
  ioctl(ttyfd, TIOCGETC, &tcharsv);
  ioctl(ttyfd, TIOCGLTC, &ltcharsv);
  ioctl(ttyfd, TIOCGETP, &ttyb);
  savetmio = ttyb;
  ioctl(ttyfd, TIOCLGET, &local_mode_sv);
  local_mode = local_mode_sv | LPASS8;
  ioctl(ttyfd, TIOCLSET, &local_mode);
  ioctl(ttypfd, TIOCSETC, &tcharsv);
  ioctl(ttypfd, TIOCSLTC, &ltcharsv);
  ioctl(ttypfd, TIOCSETP, &ttyb);
  ioctl(ttypfd, TIOCLSET, &local_mode);
#ifdef	TIOCSSIZE
  pty_rowcol.ts_lines = crow;
  pty_rowcol.ts_cols = maxwidth;
  ioctl(ttypfd, TIOCSSIZE, &pty_rowcol);
#endif	TIOCSSIZE
  pid = getpid();
  ioctl(ttypfd, TIOCSPGRP, &pid);
  setpgrp(0, 0);
  close(open(ttyname(ttypfd), O_WRONLY, 0));
  setpgrp(0, pid);
#endif	BSD

#ifdef SVR2   
  if (ioctl(ttyfd, TCGETA, &buf1) == -1) {
    buf1 = term;
  }
  savetmio = buf1;
  if (ioctl(ttypfd, TCSETA, &buf1) < 0) {
    fprintf(stderr, "can: error in ioctl TCSETA.\n");
    exit(1);
  }
#endif	SVR2
}

ptyname(name, dev, no)
     char *name;
     char *dev;
     int no;
{
  sprintf(name, "%s%1c%1x", dev, 'p' + (no >> 4), no & 0x0f);
}

err(s)
     char *s;
{
  puts(s);
  fclose(stdout);
  fclose(stderr);
  fclose(stdin);
  close_terminal();
}

#ifdef	SVR2

#ifndef	SVR4
int	(*sigchld)();
int	(*sighup)();
int	(*sigint)();
int	(*sigpipe)();
int	(*sigquit)();
int	(*sigterm)();
#ifdef	SIGTSTP
int	(*sigtstp)();
int	(*sigttin)();
int	(*sigttou)();
#endif	SIGTSTP
#ifdef	SIGWINCH
 int	(*sigwinch)();
#endif	SIGWINCH

#else	SVR4

void	(*sigchld)();
void	(*sighup)();
void	(*sigint)();
void	(*sigpipe)();
void	(*sigquit)();
void	(*sigterm)();
#ifdef	SIGTSTP
void	(*sigtstp)();
void	(*sigttin)();
void	(*sigttou)();
#endif	SIGTSTP
#ifdef	SIGWINCH
void	(*sigwinch)();
#endif	SIGWINCH

#endif	SVR4
#endif	SVR2

#ifdef	BSD
void	(*sigchld)();
void	(*sighup)();
void	(*sigint)();
void	(*sigpipe)();
void	(*sigquit)();
void	(*sigterm)();
#ifdef	SIGTSTP
void	(*sigtstp)();
void	(*sigttin)();
void	(*sigttou)();
#endif	SIGTSTP
#ifdef	SIGWINCH
void	(*sigwinch)();
#endif	SIGWINCH
#endif	BSD

int	child_handler();
int	terminate_handler();
int	resize_handler();

save_signals()
{
  sigchld = signal(SIGCHLD, child_handler);
  sighup  = signal(SIGHUP,  terminate_handler);
  sigint  = signal(SIGINT,  SIG_IGN);
  sigpipe = signal(SIGPIPE, SIG_IGN);
  sigquit = signal(SIGQUIT, SIG_IGN);
  sigterm = signal(SIGTERM, terminate_handler);
#ifdef	SIGTSTP
  sigtstp = signal(SIGTSTP, SIG_IGN);
  sigttin = signal(SIGTTIN, SIG_IGN);
  sigttou = signal(SIGTTOU, SIG_IGN);
#endif	SIGTSTP
#ifdef	SIGWINCH
  sigwinch = signal(SIGWINCH, resize_handler);
#endif	SIGWINCH
}

restore_signals()
{
  signal(SIGCHLD, sigchld);
  signal(SIGHUP,  sighup);
  signal(SIGINT,  sigint);
  signal(SIGPIPE, sigpipe);
  signal(SIGQUIT, sigquit);
  signal(SIGTERM, sigterm);
#ifdef	SIGTSTP
  signal(SIGTSTP, sigtstp);
  signal(SIGTTIN, sigttin);
  signal(SIGTTOU, sigttou);
#endif	SIGTSTP

#ifdef SIGWINCH
  signal(SIGWINCH, sigwinch);
#endif	SIGWINCH
}

#ifdef	SVR2
#define	re_signal(x, y)	signal((x), (y))
#else	SVR2
#define re_signal(x, y)
#endif	SVR2

child_handler()
{
  if (wait(0) == child_id) {
    signal(SIGCHLD, SIG_IGN);
    printf("\r\ncan: finished\r\n");
    close_terminal();
  }
  re_signal(SIGCHLD, child_handler);
}

#ifdef	SVR2
#undef	re_signal
#endif	SVR2

terminate_handler()
{
  signal(SIGCHLD, SIG_IGN);
  close_terminal();
}

#ifdef	SIGWINCH

resize_handler()
{
  register int	i;
  struct winsize win;
  
  if (ioctl(ttyfd, TIOCGWINSZ, &win) < 0) {
    return;
  } else {
    int grp;
    int flg;

    flg = (((i = win.ws_row) - conv_lines < crow) ? 0 : 1);
    if (flg) {
      save_cursor_status();
      flush();
/*       throw_cur_raw(0, crow); */
      throw_cur_raw(0, 500);
      clr_end_screen();
      flush();
      restore_cursor_status();
      flush();
    }
    if (i != 0) {
      crow = Term_RowWidth = i - conv_lines;
    }
    if ((i = win.ws_col) != 0) {
      maxwidth = Term_LineWidth = i;
    }
    pty_rowcol.ts_lines = crow;
    pty_rowcol.ts_cols = maxwidth; 
    ioctl(ttypfd, TIOCSSIZE, &pty_rowcol);
    ioctl(ptyfd, TIOCGPGRP, &grp);
    killpg(grp, SIGWINCH);
    set_scroll_region(0, crow - 1);
    flush();
    jrKanjiControl(0, KC_SETWIDTH, (char *) (maxwidth - 20));
    if (flg) {
      save_cursor_status();
      flush();
      redraw();
      restore_cursor_status();
      flush();
    } else {
      throw_cur_raw(0, 0);
      clr_end_screen();
      flush();
      redraw();
      throw_cur_raw(0, 0);
      flush();
    }
  }
}

#endif	SIGWINCH

int
exec_cmd(argc, argv)
     int argc;
     char *argv[];
{
  char *name;
  int	i;

  if (argc > 1) {
    cmdnm = *++argv;
  } else {
    if ((name = getenv("CSHELL")) != NULL || (name = getenv("SHELL")) != NULL) {
      cmdnm = name;
    }
    argv[0] = cmdnm;
    argv[1] = NULL;
  }

  if ((child_id = fork()) == 0) {
#ifdef	BSD
    int	pid;
    pid = getpid();
    ioctl(ttypfd, TIOCSPGRP, &pid);
    setpgrp(0, 0);
    close(open(ttyname(ttypfd), O_WRONLY, 0));
    setpgrp(0, pid);
#endif	BSD

#ifdef	SVR2
    setpgrp();
    close(open(ttyname(ttypfd), O_WRONLY, 0));
#endif	SVR2

    if (setutmp(ttypfd) == ERROR) {
      puts("Can't set utmp.");
    } 
    
    setgid(getgid());
    setuid(getuid());
    close(0);
    close(1);
    close(2);
    if (dup(ttypfd) != 0 || dup(ttypfd) != 1 || dup(ttypfd) != 2) {
      err("redirection fault.");
    }
    close(ttypfd);
    for (i = getdtablesize() - 1; i > 2; i--) {
      close(i);
    }
    restore_signals();
    execvp(cmdnm, argv);
    err("exec fault.");
  }
}

#ifdef	SVR2
#undef	getdtablesize
#endif	SVR2

j_term_init()
{
#ifdef	BSD
  struct sgttyb buf;
#endif	BSD
#ifdef	SVR2
  struct termio buf1;
#endif	SVR2

#ifdef	BSD
  buf = savetmio;
  buf.sg_flags |= RAW;
  buf.sg_flags &= ~ECHO;
#endif	BSD
#ifdef	SVR2
  buf1 = savetmio;
  buf1.c_iflag &= ~(ISTRIP | INLCR | IGNCR | ICRNL | IUCLC | IXON); 
  if (flow_control) {
    buf1.c_iflag |= IXON;
  }
  buf1.c_lflag &= ~(ECHONL | ECHOK | ECHOE | ECHO | XCASE | ICANON | ISIG);
  buf1.c_oflag = OPOST; 
  buf1.c_cc[VINTR] = CDEL;
  buf1.c_cc[VQUIT] = CDEL;
  buf1.c_cc[VERASE] =CDEL;
  buf1.c_cc[VKILL] = CDEL;
  buf1.c_cc[VEOF] = 1;	/* cf. ICANON */
  buf1.c_cc[VEOL] = 0;
#endif	SVR2

#ifdef	BSD
  ioctl(ttyfd, TIOCSETP, &buf);
#endif	BSD

#ifdef	SVR2
  if (ioctl(ttyfd, TCSETA, &buf1) == -1) {
    fprintf(stderr, "can: error in ioctl TCSETA.\n");
    exit(1);
  }
#endif SVR2

  return(0);
}
