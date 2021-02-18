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

#include <stdio.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <iroha/jrkanji.h>
#include "define.h"
#include "extern.h"
#ifdef	SVR2
#include <string.h>
#endif	SVR2

#define	CHECK	40

extern	int	errno;
char	*getenv();
struct	CODE_TABLE {
  char	*from;
  int	to;
};

struct CODE_TABLE code_table[] = {
  {"\033[28~", IROHA_KEY_Help},
  {"\033[209z", IROHA_KEY_Nfer},
  {"\033[210z", IROHA_KEY_Xfer},
  {"\033O2~", IROHA_KEY_Insert},
  {"\033O5~", IROHA_KEY_Rollup},
  {"\033O6~", IROHA_KEY_Rolldown},
  {"\033[A", IROHA_KEY_Up},
  {"\033[D", IROHA_KEY_Left},
  {"\033[C", IROHA_KEY_Right},
  {"\033OB", IROHA_KEY_Down},
  {"\033OA", IROHA_KEY_Up},
  {"\033OD", IROHA_KEY_Left},
  {"\033OC", IROHA_KEY_Right},
  {"\033[B", IROHA_KEY_Down},
  {"\033[11~", IROHA_KEY_F1},
  {"\033[12~", IROHA_KEY_F2},
  {"\033[13~", IROHA_KEY_F3},
  {"\033[14~", IROHA_KEY_F4},
  {"\033[15~", IROHA_KEY_F5},
  {"\033[17~", IROHA_KEY_F6},
  {"\033[18~", IROHA_KEY_F7},
  {"\033[19~", IROHA_KEY_F8},
  {"\033[20~", IROHA_KEY_F9},
  {"\033[21~", IROHA_KEY_F10}
};

int	code_table_size = sizeof(code_table) / sizeof(struct CODE_TABLE);
int	flg;

jrKanjiStatus	ks;

unsigned char	mode_string[MAXSIZE]	= { '\0' };
int		mode_string_length	= 0;

struct	linebuf {
  unsigned char	line[MAXSIZE];
  int		length;
  int		revPos;
  int		revLen;

};

struct linebuf	line = {{'\0' }, 0, 0, 0};
struct linebuf	gline= {{'\0' }, 0, 0, 0};

void
romkan()
{
  jrKanjiStatusWithValue	ksv;

  unsigned char	workbuf[MAXSIZE];
  unsigned char	buffer[64];
  unsigned char	buf[BUFSIZ];
  unsigned char	inbuf[BUFSIZ];
  unsigned char	*bufend = inbuf;
  unsigned char	*bufstart = inbuf;
  unsigned char	*t;

  int	id, nbytes, sel_ret, rfds, i, j, n, flag = CHECK;
  char	*s;
  char	**msg;

#ifdef	BSD
  struct timeval to;
#else	BSD
  int	to;
#endif	BSD

  jrKanjiControl(0, KC_INITIALIZE, &msg);
  if (msg) {
    for (; *msg; msg++)
      fprintf(stderr, "%s\n", *msg);
  }
  is_opened = 1;

  jrKanjiControl(0, KC_SETWIDTH, (char *) (maxwidth - 20));

  ksv.ks = &ks;
  ksv.buffer = buffer;
  ksv.bytes_buffer = 32;
  ksv.val = IROHA_MODE_AlphaMode;
  jrKanjiControl(0, KC_CHANGEMODE, &ksv);

  id = 0;
  j = BUFSIZ;
  for (;;) {
    if (bufend == bufstart)
      bufstart = bufend = inbuf;
    if ((n = read(ttyfd, buf, inbuf - bufend + BUFSIZ)) > 0) {
      strncpy(bufend, buf, n);
      bufend += n;
    }
    if (bufend > bufstart) {
      for (; id < code_table_size; id++) {
	for(s = code_table[id].from, t = bufstart;
	    *s != '\0' &&  t < bufend && *s == *t ; s++, t++)
	  ;
	if (*s == '\0') {
	  if ((nbytes = jrKanjiString(0, code_table[id].to, workbuf, MAXSIZE, &ks)) == 1
	      && code_table[id].to == *workbuf)
	    write(ptyfd, (unsigned char *) code_table[id].from, strlen(code_table[id].from));
	  else if (nbytes)
	    write(ptyfd, workbuf, nbytes);
	  if (check_redraw()) {
	    save_cursor_status();
	    flush();
	    redraw();
	    restore_cursor_status();
	    flush();
	  }
	  bufstart = bufend = inbuf;
	  id = 0;
	  flag = CHECK;
	  j = BUFSIZ;
	  break;
	} else if (t == bufend) {
	  if (j) { 
	    j--;
	    break;
	  }
	}
      }
      if (id == code_table_size) {
	if ((nbytes = jrKanjiString(0, (int) *bufstart++, workbuf, MAXSIZE, &ks)) > 0) {
	  write(ptyfd, workbuf, nbytes);
	}
	if (check_redraw()) {
	  save_cursor_status();
	  flush();
	  redraw();
	  restore_cursor_status();
	  flush();
	}
	id = 0;
	j = BUFSIZ;
	flag = CHECK;
      } else
	flag--;
    } else {
#ifdef	BSD
      to.tv_sec = 2;
      to.tv_usec = 0;
#else	BSD
      to = 2;
#endif	BSD
      for(rfds = sel_ptn;
	  (sel_ret = select(20, &rfds, 0, 0, &to)) < 0 && errno == EINTR;
	  rfds = sel_ptn)
	;
      if (rfds & pty_bit) {
	if ((n = read(ptyfd, buf, BUFSIZ)) > 0)
	  write(ttyfd, buf, n);
      }
    }
  }
}

int
check_redraw()
{
  int	result = 0;

  if (ks.info & KanjiModeInfo && strcmp(ks.mode, mode_string) != 0) {
    strcpy(mode_string, ks.mode);
    mode_string_length = strlen(mode_string);
    mode_string[mode_string_length++] = ' ';
    mode_string[mode_string_length++] = ' ';
    mode_string[mode_string_length] = '\0';
    result = 1;
  }

  if (ks.info & KanjiGLineInfo) {
    strncpy(gline.line, ks.gline.line, ks.gline.length);
    gline.length = ks.gline.length;
    gline.revPos = ks.gline.revPos;
    gline.revLen = ks.gline.revLen;
  }

  if (line.length > 0 || gline.length > 0 || ks.length > 0)
    result = 1;

  if (ks.length >= 0) {
    strncpy(line.line, ks.echoStr, ks.length);
    line.length = ks.length;
    line.revPos = ks.revPos;
    line.revLen = ks.revLen;
  }
  return(result);
}

int
redraw()
{
/*  throw_cur_raw(0, crow); */
  throw_cur_raw(0, 500);
  clr_end_screen();
  flush();
  write(ttyfd, mode_string, mode_string_length);
  if (gline.length > 0) {
    echo_string(&gline);
  } else if (line.length > 0) {
    echo_string(&line);
  }
}

int
echo_string(buf)
     struct linebuf	*buf;
{
  unsigned char	*line	= buf->line;
  int		length	= buf->length;
  int		revLen	= buf->revLen;
  int		revPos	= buf->revPos;
  unsigned char	left[2];
  unsigned char	right[2];
  int		l;
  
  l = mode_string_length + length + 2;
  if (l < maxwidth) {
    strcpy(left, " ");
    strcpy(right,  " ");
  } else if (revLen == 0) {
    strcpy(left, SUB);
    strcpy(right, " ");
    l -= maxwidth;
    line += ((which(line, l) == 2) ? ++l : l);
    length -= l;
  } else if (l - revPos < maxwidth) {
    strcpy(right, " ");
    strcpy(left, SUB);
    l -= maxwidth;
    line += ((which(line, l) == 2) ? ++l : l);
    length -= l;
    revPos -= l;
  } else {
    strcpy(left, (revPos == 0 ? " " : SUB));
    strcpy(right, SUB);
    line += revPos;
    revPos = 0;
    length = maxwidth - mode_string_length -  2;
    if (which(line, length - 1) == 1)
      length--;
  }
  write(ttyfd, left, 1);
  write(ttyfd, line, revPos);
  if (revLen) {
    reverse_on();
    flush();
    write(ttyfd, line+revPos, revLen);
    reverse_off();
    flush();
  }
  write(ttyfd, line+revPos+revLen, length-revPos-revLen);
  if (!revLen) {
    reverse_on();
    flush();
  }
  write(ttyfd, right, 1);
  if (!revLen) {
    reverse_off();
    flush();
  }
}

int
which(s, len)
     unsigned char	*s;
     int		len;
{
  unsigned char	*p = s + len;
  int	l = 0;

  if (p < s)
    return(ERROR);
  for (; s <= p; s++) {
    if (*s & 0xff == 0x8e) {
      if (l == 0 || l == 2) {
	l = 1;
      } else {
	return(ERROR);
      }
    } else if (*s & 0x80) {
      if (l == 0 || l == 2) {
	l = 1;
      } else {
	l = 2;
      }
    } else if (l == 1) {
      return(ERROR);
    } else
      l = 0;
  }
  return(l);
}

