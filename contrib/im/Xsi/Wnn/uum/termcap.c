/*
 * $Id: termcap.c,v 1.3 1991/09/16 21:33:58 ohm Exp $
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
#include <stdio.h>
#include <ctype.h>
#include <sys/errno.h>
#include "commonhd.h"

#include "sdefine.h"
#include "sheader.h"

#undef putchar
extern int putchar();

#undef DEFAULT_KKCAP
/* declaration about terminal */

int kkcap_mode;

int 	Term_LineWidth;
int 	Term_RowWidth;
/*
char 	*Term_Bell;
*/
char	*Term_ClrScreen;
char 	*Term_ClrEofLine;
char    *Term_CleEndScreen;
char 	*Term_ThrowCursor;
char 	*Term_StandOutStart;	/* These variables has value even if terminfo is used. see termio.c. */
char 	*Term_StandOutEnd;
static int bold_mode_fun;
char 	*Term_BoldOutStart;
char 	*Term_BoldOutEnd;
/*
int 	Term_StandOutBlankNum;
char 	*Term_DelChar;
*/
char    *Term_UnderScoreStart;
char    *Term_UnderScoreEnd;
#ifdef TERMCAP
char    *Term_CursorNormal;
char    *Term_CursorInvisible;


char 	*Term_SaveCursor;
char	*Term_RestoreCursor;
char	*Term_SetScrollRegion;
char	*Term_ScrollRegion;
char    *Term_ResetScreen;
char 	TermDataArea[1024];
static char 	TermData[1024];

int cursor_state;

extern char		*tgetstr();
extern FILE		*fopen();

#ifdef DCUREOR 
FILE *debugc , *fopen();
#endif


int
getTermData()
{
    char *name;
    char 	*pter;
    char *sr_set();
    char *j;
    extern char *getenv(), *get_kbd_env();
    

#ifdef DCUREOR 
    if((debugc = fopen(DP , "w"))== NULL){
	fprintf(stderr , "OPEN ERR IN %d. \n" , DP);
    }
#endif

	/* for convert_key --- added by Nide 10/3 */
    if (NULL == (name = get_kbd_env()) ||
    	0 != convert_getterm(name, (0 != verbose_option))){
	fprintf(stderr, "Cannot get keyboard information.\n");
	return(-1); 
    }

    if (((name = getenv("TERM")) == NULL) || (tgetent(TermData, name) <= 0)){
	fprintf(stderr, "Cannot get terminal information.\n");
	return(-1); 
    }
    strcpy(Term_Name,name);
    pter = TermDataArea;

    Term_ResetScreen = tgetstr("rs" , &pter);
    Term_UnderScoreEnd = tgetstr("ue", &pter);
    if(!(	
	 (Term_UnderScoreEnd = tgetstr(j = "ue", &pter)) &&
	 ((Term_LineWidth = tgetnum(j = "co")) != -1) &&
	 ((Term_RowWidth = tgetnum(j = "li")) != -1) && /* line */
	 /*	     (Term_Bell = tgetstr(j = "bl", &pter)) &&  */
	 (Term_ClrScreen = tgetstr(j = "cl", &pter)) &&
	 (Term_ClrEofLine = tgetstr(j = "ce" , &pter)) &&
	 (Term_CleEndScreen = tgetstr(j = "cd" , &pter)) &&
	 (Term_ThrowCursor = tgetstr(j = "cm", &pter)) &&
	     /* Check padding */
	 (Term_StandOutStart = tgetstr(j = "so", &pter)) &&
	 (Term_StandOutEnd = tgetstr(j = "se", &pter)) &&
	 /*
	   ((Term_StandOutBlankNum = tgetnum(j = "sg")) != -1) &&   
	   (Term_DelChar = tgetstr(j = "dc", &pter)) &&
	   */
	 (Term_UnderScoreStart = tgetstr(j = "us", &pter)) &&
	 (Term_SaveCursor = tgetstr(j = "sc", &pter)) &&
	 (Term_RestoreCursor = tgetstr(j = "rc", &pter)) &&
	 (Term_SetScrollRegion = tgetstr(j = "cs" , &pter)) &&
	 (Term_ScrollRegion = sr_set(&pter, 0, Term_RowWidth - 2)) )){
	     fprintf(stderr , "Your terminal doesn't have %s entry in termcap.\n" , j);
	     fprintf(stderr , "Maybe, your terminal is not strong enough to use Uum!\n");
	     return(-1);
	 }

        if(Term_BoldOutStart && Term_BoldOutEnd){
	    bold_mode_fun = 1;
	}else{
	    bold_mode_fun = 0;
	}
	 Term_BoldOutStart = tgetstr("md" , &pter);
	 Term_BoldOutEnd = tgetstr("me" , &pter);

        Term_CursorInvisible = tgetstr("vi", &pter);
        Term_CursorNormal= tgetstr("ve", &pter);
        if( Term_CursorInvisible && Term_CursorNormal){
	    cursor_invisible_fun = 1;
	}else{
	    cursor_invisible_fun = 0;
	}
/*  and needs more precise check of Terminal status. */

    maxlength = Term_LineWidth;
    crow = Term_RowWidth - conv_lines;

/*    decfline(TermData); decfline is moved after reading uumrc.*/

    return(0);
}

char *
strsch(str1 , str2)
char *str1 , *str2;
{
    char *c = NULL;
    int flag = 0;
    for(;*str1++;){
	if(!flag){
	    if(*str1 == *str2){
		flag = 1;
		c = str2 + 1;
	    }
	}else{
	    if(*c == 0)return(str1);
	    if(*c++ != *str1)flag = 0;
	}
    }
    return(NULL);
}

/* Remove an entry from TERMCAP string   by T.S. */

static char *remove(p,ob)
char *p,*ob;
{
    char *r;

    if((r = strsch(p , ob)) != NULL){
      p = r - strlen(ob);
      for(;*r != ':' ; r++);
      strcpy(p , r);
    };
    return(r);
}

static void strascii(), decfline();

void
set_TERMCAP()
{
    decfline(TermData);
}


/** termcap no entry no naka de li: to cs: wo kakikaemasu */
static void
decfline(name)
char *name;
{
    char name1[1024];
    char *c;

    if((c = strsch(name , "li#")) != NULL){
      strncpy(name1 , name , c - name);
      name1[c - name] = 0;
      for(;*c >= '0' && *c <= '9' ; c++);
      sprintf(name1 + strlen(name1) , "%d" , crow);
      strcat(name1 , c);
    }
/* add rs entry to termcap */
    remove(name1 , ":rs");
    strcat(name1 , "rs=");
    if(Term_ResetScreen)
      strascii(name1 + strlen(name1) , Term_ResetScreen);
    strascii(name1 + strlen(name1) , Term_ScrollRegion);
    strcat(name1 , ":");

    if(remove_cs_from_termcap){
	remove(name1,":cs");
    }
    remove(name1,":sc");
    remove(name1,":rc");
    setenv("TERMCAP" , name1);
}

/* functions using Terminal Information. */

char *
sr_set(st, start, end)
int start, end;
char **st;
{
    char *string = *st;
    char *pt = Term_SetScrollRegion;
    char *pt1;
    char nextch;
    int params[2];
    int i = 0;			/* points tp params */
    
    *string = 0;
    params[0] = start ;
    params[1] =  end;
    for(;*pt != 0 ; pt++){
	if(i > 2){
	    fprintf(stderr , "ERROR in tparam few parameters.");
	     /* Message ga wakaridurai? */
	    return(NULL);
	}
	if(*pt == '%'){
	    switch(*++pt){
	    case 'd':
		sprintf(string + strlen(string), "%d" , params[i++]);
		break;
	    case '2':
		sprintf(string + strlen(string), "%2d" , params[i++]);
		break;
	    case '3':
		sprintf(string + strlen(string), "%3d" , params[i++]);
		break;
	    case '.':
		sprintf(string + strlen(string), "%c" , params[i++]);
		break;
	    case '+':
		if (!(nextch = *++pt)) {
			fprintf(stderr, "Unexpected EOL in cs string.\n");
			return NULL;
		}
		sprintf(string + strlen(string), "%c" , params[i++] + nextch);
		break;
	    case '>':
		if (!(nextch = *++pt)) {
			fprintf(stderr, "Unexpected EOL in cs string.\n");
			return NULL;
		}
		if (params[i] > nextch) {
			if (!(nextch = *++pt)) {
				fprintf(stderr, "Unexpected EOL in cs string.\n");
				return NULL;
			}
			params[i] += nextch;
		}
		break;
	    case 'r':
		{	int temp;
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
		strcat(string , "%");
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
		fprintf(stderr, "Unexpected EOL in cs string.\n");
		return NULL;
	    }
	}else{
	    pt1 = string + strlen(string);
	    *pt1 = *pt;
	    *++pt1 = 0;
	}
    }
    *st = string + strlen(string);
    return(string);
}

void
set_scroll_region(start, end)
int start, end;
{
    char *a;
    char TERM_SCROLLREGION[24];
    a = TERM_SCROLLREGION;

    sr_set(&a , start , end);	/* changed in June 9 */
    fputs(TERM_SCROLLREGION , stdout);
}

void
clr_end_screen()
{
    tputs(Term_CleEndScreen, 1, putchar);
}

void
clr_screen()
{
    tputs(Term_ClrScreen, Term_RowWidth ,putchar);
}

void
clr_line1()
{
    tputs(Term_ClrEofLine, 1, putchar);
}

void
throw_cur_raw(col,row)
int col, row;
{
    tputs(tgoto(Term_ThrowCursor, col, row), 1, putchar);
}

void
h_r_on_raw()
{
    tputs(Term_StandOutStart, 1, putchar);
}

void
h_r_off_raw()
{
    tputs(Term_StandOutEnd, 1, putchar);
}

void
u_s_on_raw()
{
    tputs(Term_UnderScoreStart, 1, putchar);
}

void
u_s_off_raw()
{
    tputs(Term_UnderScoreEnd, 1, putchar);
    flush();
}

void
b_s_on_raw()
{
    if (bold_mode_fun)
	tputs(Term_BoldOutStart, 1, putchar);
    else
	tputs(Term_UnderScoreStart, 1, putchar);
    flush();
}

void
b_s_off_raw()
{
    if (bold_mode_fun)
	tputs(Term_BoldOutEnd, 1, putchar);
    else
	tputs(Term_UnderScoreEnd, 1, putchar);
    flush();
}

void
ring_bell()
{
    putchar(Ctrl('G'));flush();
/*
    tputs(Term_Bell, 1, putchar);
*/
}

void
save_cursor_raw()
{
  tputs(Term_SaveCursor, 1, putchar);
}

void
restore_cursor_raw()
{
  tputs(Term_RestoreCursor, 1, putchar);    
}

void
cursor_invisible_raw()
{
  tputs(Term_CursorInvisible, 1, putchar);
}
 
void
cursor_normal_raw()
{
  fputs(Term_CursorNormal , stdout);
}

static void
strascii(dest , str)
unsigned char *dest , *str;
{
  for(;*str;str++){
    if(*str >= ' '){
      *dest++ = *str;
    }else if(*str == '\033'){
      *dest++ = '\\';
      *dest++ = 'E';
    }else{
      *dest++ = '^';
      *dest++ = *str + '@';
    }
  }
  *dest = '\0';
}

#endif /* TERMCAP */
