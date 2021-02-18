/*
 * $Id: termio.c,v 1.2 1991/09/16 21:34:01 ohm Exp $
 */
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/*	Version 4.0
 */
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"

#ifdef TERMINFO
#include <stdio.h>
#include <curses.h>
#include <term.h>
#ifdef uniosu
#undef putchar
extern int putchar();
#endif

extern char Term_Name[];
extern char    *Term_UnderScoreStart;
extern char    *Term_UnderScoreEnd;
extern char	*Term_ClrScreen;
extern char 	*Term_ClrEofLine;
extern char 	*Term_ThrowCursor;
extern char 	*Term_StandOutStart;
extern char 	*Term_StandOutEnd;
extern char 	*Term_BoldOutStart;
extern char 	*Term_BoldOutEnd;
static int	bold_mode_fun = 0;

int
openTermData()
{
    char *cp, *getenv(), *get_kbd_env();
    int status;
    int k;
    char lcode[10];
    char termchar[20];

	/* for convert_key --- added by Nide 10/3 */
    if (NULL == (cp = get_kbd_env()) ||
        0 != convert_getterm(cp, (0 != verbose_option))){
	fprintf(stderr, "Cannot get keyboard information.\n");
	return(-1); 
    }

    if ((cp = getenv("TERM")) == NULL){
        fprintf(stderr, "Cannot get terminal name.");
        return(-1);
    }
    strcpy(Term_Name, cp);

    if ((strlen(Term_Name) > 2) && (strcmp(Term_Name + (strlen(Term_Name) - 2),"-j") == 0)) {
	fprintf(stderr, MSG_GET(4));
	/*
	fprintf(stderr,"Uum:ｕｕｍからｕｕｍはおこせません。\n");
	*/
	return(-1);
    }
    setupterm(0,1,&status);
    reset_shell_mode();
    if (status != 1){
        return(-1);
    }
#if defined(uniosu)
    if(jterm < 2) {	/* kanji terminal */
        fprintf(stderr, "Not kanji terminal. Goodbye !\n");
        return(-1);
    }
#endif /* defined(uniosu) */
    if(save_cursor == (char *)NULL || *save_cursor == NULL ||
       restore_cursor == (char *)NULL || *restore_cursor == NULL ||
       change_scroll_region == (char *)NULL || *change_scroll_region == NULL) {
         fprintf(stderr, "Your terminal is not strong enough. Goodbye !\n");
         return(-1);
    }
    termchar[0] = 0;
    strcat(termchar,cp);
    strcat(termchar,"-j");
    setenv("TERM", termchar);

    sprintf(lcode,"%d", lines - conv_lines);
    setenv("LINES", lcode);

    if(cursor_normal && cursor_invisible){
	cursor_invisible_fun = 1;
    }else{
	cursor_invisible_fun = 0;
    }
    Term_UnderScoreEnd = exit_underline_mode;
    Term_UnderScoreStart = enter_underline_mode;
    Term_StandOutEnd = exit_standout_mode;
    Term_StandOutStart = enter_standout_mode;
    if (enter_bold_mode && exit_attribute_mode)
	bold_mode_fun = 1;
    else
	bold_mode_fun = 0;

    Term_BoldOutStart = enter_bold_mode;
    Term_BoldOutEnd = exit_attribute_mode;
    return(0);
}


void
closeTermData()
{
    resetterm();
    reset_shell_mode();
}

void
set_scroll_region(start, end)
int start, end;
{
    tputs(tparm(change_scroll_region, start, end), 1 , putchar);
}

void
clr_end_screen()
{
    tputs(clr_eos ,1,putchar);
}


void
clr_screen()
{
    tputs(clear_screen, lines, putchar);
    Term_ClrScreen = clear_screen;
}

void
clr_line1()
{
    tputs(clr_eol, 1, putchar);
    Term_ClrEofLine = clr_eol;
}

void
throw_cur_raw(col,row)
int col, row;
{
    tputs(tparm(cursor_address, row, col), 1, putchar);
}

void
h_r_on_raw()
{
    tputs(enter_standout_mode, 1, putchar);
}

void
h_r_off_raw()
{
    tputs(exit_standout_mode, 1, putchar);
}

void
u_s_on_raw()
{
    tputs(enter_underline_mode, 1, putchar);
}

void
u_s_off_raw()
{
    tputs(exit_underline_mode, 1, putchar);
}

void
b_s_on_raw()
{
    if (bold_mode_fun)
	tputs(enter_bold_mode, 1, putchar);
    else
	tputs(enter_underline_mode, 1, putchar);
}

void
b_s_off_raw()
{
    if (bold_mode_fun)
	tputs(exit_attribute_mode, 1, putchar);
    else
	tputs(exit_underline_mode, 1, putchar);
}

void
ring_bell()
{
    tputs(bell, 1, putchar);
    flush();
}

void
save_cursor_raw()
{
  tputs(save_cursor, 1, putchar);
}

void
restore_cursor_raw()
{
    tputs(restore_cursor, 1, putchar);
}

void
cursor_invisible_raw()
{
  tputs(cursor_invisible , 1 ,putchar);
}
 
void
cursor_normal_raw()
{
  tputs(cursor_normal , 1 , putchar);
}

#endif                          /* TERMINFO */
