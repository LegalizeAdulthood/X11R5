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
#include <ctype.h>
#include <sys/errno.h>
#ifdef	BSD
#include <string.h>
#endif	BSD
#ifdef	SVR2
#include <curses.h>
#include <term.h>
#endif	SVR2

#include "extern.h"

char	*getenv();

#ifdef	putchar
#undef	putchar
#endif	putchar
extern int	putchar();

char		*Term_CleEndScreen;
int		Term_LineWidth;
int		Term_RowWidth;
char		*Term_StandOutStart;
char		*Term_StandOutEnd;
char		*Term_ThrowCursor;

#ifdef	TERMCAP

char		*sr_set();
char		*tgetstr();
char		*Term_RestoreCursor;
char		*Term_SaveCursor;
char		*Term_ScrollRegion;
char		*Term_SetScrollRegion;
static char	TermData[1024] = { '\0' };
char		TermDataArea[1024] = { '\0' };

int
openTermData()
{
  char	*pter;
  char	*j;
    
  if (tgetent(TermData, TermName) <= 0){
    fprintf(stderr, "can: Cannot get terminal information.\n");
    return(-1); 
  }
  pter = TermDataArea;

  if (!(	
	((Term_LineWidth = tgetnum(j = "co")) != -1) &&
	((Term_RowWidth = tgetnum(j = "li")) != -1) &&
	(Term_CleEndScreen = tgetstr(j = "cd", &pter)) &&
	(Term_ThrowCursor = tgetstr(j = "cm", &pter)) &&
	(Term_StandOutStart = tgetstr(j = "so", &pter)) &&
	(Term_StandOutEnd = tgetstr(j = "se", &pter)) &&
	(Term_SaveCursor = tgetstr(j = "sc", &pter)) &&
	(Term_RestoreCursor = tgetstr(j = "rc", &pter)) &&
	(Term_SetScrollRegion = tgetstr(j = "cs", &pter)) &&
	(Term_ScrollRegion = sr_set(&pter, 0, Term_RowWidth - 2)) )){
    fprintf(stderr, "can: Your terminal doesn't have %s entry in termcap.\n", j);
    fprintf(stderr, "can: Maybe, your terminal is not strong enough to use Unn!\n");
    return(-1);
  }

  maxwidth = Term_LineWidth;
  crow = Term_RowWidth - conv_lines;

  return(0);
}

char *
sr_set(st, start, end)
     int  start;
     int  end;
     char **st;
{
  char	*string = *st;
  char	*pt = Term_SetScrollRegion;
  char	*pt1;
  char	nextch;
  int	params[2];
  int	i = 0;
    
  *string = 0;
  params[0] = start ;
  params[1] =  end;
  for(; *pt != 0; pt++){
    if (i > 2) {
      fprintf(stderr, "can: Error in tparam few parameters.");
      return(NULL);
    }
    if (*pt == '%') {
      switch (*++pt) { 
      case 'd':
	sprintf(string + strlen(string), "%d", params[i++]);
	break;
      case '2':
	sprintf(string + strlen(string), "%2d", params[i++]);
	break;
      case '3':
	sprintf(string + strlen(string), "%3d", params[i++]);
	break;
      case '.':
	sprintf(string + strlen(string), "%c", params[i++]);
	break;
      case '+':
	if (!(nextch = *++pt)) {
	  fprintf(stderr, "can: Unexpected EOL in cs string.\n");
	  return(NULL);
	}
	sprintf(string + strlen(string), "%c", params[i++] + nextch);
	break;
      case '>':
	if (!(nextch = *++pt)) {
	  fprintf(stderr, "can: Unexpected EOL in cs string.\n");
	  return(NULL);
	}
	if (params[i] > nextch) {
	  if (!(nextch = *++pt)) {
	    fprintf(stderr, "can: Unexpected EOL in cs string.\n");
	    return(NULL);
	  }
	  params[i] += nextch;
	}
	break;
      case 'r':
	{
	  int temp;
	  temp = params[0];
	  params[0] = params[1];
	  params[1] = temp;
	}
	break;
      case 'i':
	params[0]++;
	params[1]++;
	break;
      case '%':
	strcat(string, "%");
	break;
      case 'n':
	params[0] ^= 0140;
	params[1] ^= 0140;
	break;
      case 'B':
	params[i] = ((params[i] / 10) << 4) + params[i] % 10;
	break;
      case 'D':
	params[i] = params[i] - 2 * (params[i] % 16);
	break;
      case '\0':
	fprintf(stderr, "can: Unexpected EOL in cs string.\n");
	return(NULL);
      }
    } else {
      pt1 = string + strlen(string);
      *pt1 = *pt;
      *++pt1 = 0;
    }
  }
  *st = string + strlen(string);
  return(string);
}

int
set_scroll_region(start, end)
     int start;
     int end;
{
  char *a;
  char TERM_SCROLLREGION[24];

  a = TERM_SCROLLREGION;
  sr_set(&a, start, end);
  fputs(TERM_SCROLLREGION, stdout);
}

int
clr_end_screen()
{
  tputs(Term_CleEndScreen, 1, putchar);
}

int
throw_cur_raw(col, row)
     int col;
     int row;
{
  tputs(tgoto(Term_ThrowCursor, col, row), 1, putchar);
}

int
reverse_on()
{
  tputs(Term_StandOutStart, 1, putchar);
}

int
reverse_off()
{
  tputs(Term_StandOutEnd, 1, putchar);
}

int
save_cursor_status()
{
  tputs(Term_SaveCursor, 1, putchar);
}

restore_cursor_status()
{
  tputs(Term_RestoreCursor, 1, putchar);    
}

#endif	TERMCAP

#ifdef TERMINFO

int
openTermData()
{
  char	*cp;
  char	lcode[10];
  int	k;
  int	status;
  char	termchar[20];

  setupterm(0, 1, &status);
  reset_shell_mode();
  if (status != 1){
    return(-1);
  }
  if (save_cursor == (char *) NULL || *save_cursor == NULL ||
      restore_cursor == (char *) NULL || *restore_cursor == NULL ||
      change_scroll_region == (char *) NULL || *change_scroll_region == NULL) {
    fprintf(stderr, "can: Your terminal is not strong enough. Goodbye !\n");
    return(-1);
  }
  sprintf(lcode,"%d", lines - conv_lines);
  setenv("LINES", lcode);
  Term_StandOutEnd = exit_standout_mode;
  Term_StandOutStart = enter_standout_mode;
  return(0);
}

int
closeTermData()
{
  resetterm();
  reset_shell_mode();
}

int
set_scroll_region(start, end)
     int start;
     int end;
{
  tputs(tparm(change_scroll_region, start, end), 1, putchar);
}

int
clr_end_screen()
{
  tputs(clr_eos, 1, putchar);
}

throw_cur_raw(col,row)
int col, row;
{
    tputs(tparm(cursor_address, row, col), 1, putchar);
}

reverse_on()
{
  tputs(enter_standout_mode, 1, putchar);
}

reverse_off()
{
  tputs(exit_standout_mode, 1, putchar);
}

save_cursor_status()
{
  tputs(save_cursor, 1, putchar);
}

restore_cursor_status()
{
  tputs(restore_cursor, 1, putchar);
}

cursor_normal_raw()
{
  tputs(cursor_normal, 1, putchar);
}

#endif	TERMINFO

