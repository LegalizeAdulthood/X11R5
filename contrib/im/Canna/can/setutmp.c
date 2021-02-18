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

#include  "define.h"
#include <stdio.h>
#include <sys/types.h>
#include <utmp.h>
#include <pwd.h>
#ifdef	SVR2
#include <memory.h>
#endif	SVR2
#ifdef	BSD
#include <fcntl.h>
#endif	BSD

char *ttyname();

#ifdef BSD

static struct utmp saveut;
static int savslotnum = 0;
static char savttynm[8];

static int suf = 0;
static struct utmp nullut;

int
saveutmp()
{
  register int	utmpFd;
  register char	*p;

  if (suf > 0)
      return(0);
  suf = -1;
  if ((p = ttyname(0)) == NULL)
    return(-1);
  bzero(&nullut, sizeof nullut);
  strncpy(savttynm, rindex(p, '/') + 1, 8);
  if (!(savslotnum = ttyslot()))
    return(-1);
  if ((utmpFd = open("/etc/utmp", O_RDONLY, 0)) < 0)
    return(-1);
  lseek(utmpFd, savslotnum * (sizeof saveut), 0);
  read(utmpFd, &saveut, sizeof saveut);
  close(utmpFd);
  strncpy(nullut.ut_line, saveut.ut_line, 8);
  strncpy(nullut.ut_host, saveut.ut_host, 16);
  nullut.ut_time = saveut.ut_time;
  suf = 1;
  return(0);
}

int
setutmp(ttyFd)
     int ttyFd;
{
  int utmpFd;
  struct utmp ut;
  char *p;
  int i;

  if ((p = ttyname(ttyFd)) == NULL)
    return(-1);
  if (suf <= 0)
    return(-1);
  bzero(&ut, sizeof ut);
  strncpy(ut.ut_line, rindex(p, '/') + 1, 8);
  strncpy(ut.ut_name, getpwuid(getuid())->pw_name, 8);
  ut.ut_time = time(0);
  strncpy(ut.ut_host, savttynm, 8);
  if (!(i = ttyfdslot(ttyFd)))
    return(-1);
  if ((utmpFd = open("/etc/utmp", O_RDWR, 0)) < 0)
    return(-1);
  lseek(utmpFd, savslotnum * (sizeof nullut), 0);
  write(utmpFd, &nullut, sizeof nullut);
  lseek(utmpFd, i * (sizeof ut), 0);
  write(utmpFd, &ut, sizeof ut);
  close(utmpFd);
  return(0);
}

int
resetutmp(ttyFd)
     int ttyFd;
{
  struct utmp	ut;
  int		utmpFd;
  char		*p;
  int		i;
  
  if ((p = ttyname(ttyFd)) == NULL)
    return(-1);
  bzero(&ut, sizeof ut);
  strncpy(ut.ut_line, rindex(p, '/') + 1, 8);
  ut.ut_time = time(0);
  if (!(i = ttyfdslot(ttyFd)))
    return(-1);
  if ((utmpFd = open("/etc/utmp", O_RDWR, 0)) < 0)
    return( -1);
  lseek(utmpFd, savslotnum * (sizeof saveut), 0);
  write(utmpFd, &saveut, sizeof saveut);
  lseek(utmpFd, i * (sizeof ut), 0);
  write(utmpFd, &ut, sizeof ut);
  close(utmpFd);
  return(0);
}
#endif	BSD

#ifdef	SVR2
int
setutmp(ttyFd)
     int ttyFd;
{
  int set_utmp();

  return(set_utmp(ttyFd, USER_PROCESS));
}

int
resetutmp(ttyFd)
     int ttyFd;
{
  int set_utmp();

  return(set_utmp(ttyFd, DEAD_PROCESS));
}

static int
set_utmp(ttyFd, Type)
     int ttyFd;
     int Type;
{
  struct utmp	ut;
  char		*p;
  struct passwd	*getpwuid();

  if ((p = ttyname(ttyFd)) == NULL)
    return(-1);
  memset(&ut, 0, sizeof ut);
  strncpy(ut.ut_line, strrchr(p, '/') + 1, 12);
  strncpy(ut.ut_user, getpwuid(getuid())->pw_name, 8);
  ut.ut_time = time(0);
  strncpy(ut.ut_id, &ut.ut_line[2], 4);
  ut.ut_id[0] = 't';
  ut.ut_pid = getpid();
  ut.ut_type = (short) Type;
  setutent(); 
  getutid(&ut); 
  pututline(&ut);
  endutent();
  return(0);
}
#endif	SVR2
