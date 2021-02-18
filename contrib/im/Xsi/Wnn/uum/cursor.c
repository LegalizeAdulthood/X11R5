/*
 * $Id: cursor.c,v 1.2 1991/09/16 21:33:27 ohm Exp $
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

int cursor_colum = 0;
static int cursor_upscreen = 1;
static int cursor_reverse = 0;
static int cursor_underline = 0;
static int cursor_bold = 0;
int cursor_invisible = 0;

void
throw_col(col)
int col;
{
  if (cursor_upscreen){
    kk_save_cursor();
  }
  throw_cur_raw(col % maxlength, crow + (col / maxlength));
  cursor_colum = col;
}

void
h_r_on()
{
  if(!cursor_reverse){
    h_r_on_raw();
    cursor_reverse = 1;
  }
}

void
h_r_off()
{
  if(cursor_reverse){
    h_r_off_raw();
    cursor_reverse = 0;
  }
  if(cursor_bold){
    b_s_on_raw();
  }
}

void
u_s_on()
{
  if(!cursor_underline){
    u_s_on_raw();
    cursor_underline = 1;
  }
}

void
u_s_off()
{
  if(cursor_underline){
    u_s_off_raw();
    cursor_underline = 0;
  }
}

void
b_s_on()
{
  if(!cursor_bold){
    b_s_on_raw();
    cursor_bold = 1;
  }
  if(cursor_reverse){
    h_r_on_raw();
  }
}

void
b_s_off()
{
  if(cursor_bold){
    b_s_off_raw();
    cursor_bold = 0;
  }
}

void
kk_cursor_invisible()
{
    if(cursor_invisible_fun && (cursor_invisible == 0)){
      cursor_invisible_raw();
      flush();
    }
    cursor_invisible = 1;
}
 
void
kk_cursor_normal()
{
    if(cursor_invisible_fun && cursor_invisible){
      cursor_normal_raw();
      flush();
    }
    cursor_invisible = 0;
}

void
kk_save_cursor()
{
  if(cursor_upscreen){ 
    save_cursor_raw();
    cursor_upscreen = 0;
    set_cursor_status();
    flush();
  } 
}

void
kk_restore_cursor()
{
  if(!cursor_upscreen){
    reset_cursor_status();
    restore_cursor_raw();
    cursor_upscreen = 1;
    flush();
  }
}

void
reset_cursor_status()
{
  if(!cursor_upscreen){
      kk_cursor_normal();
      h_r_off();
      u_s_off();
    flush();
  }
}

void
set_cursor_status()
{
  if(!cursor_upscreen){
    if(cursor_invisible){
      if(cursor_invisible_fun){
	cursor_invisible_raw();
      }else{
	throw_col(maxlength - 1);
      }
    }else{
      if(cursor_invisible_fun){
	cursor_normal_raw();
      }
    }
    if(cursor_reverse){
      h_r_on_raw();
    }
    if(cursor_underline){
      u_s_on_raw();
    }
    throw_cur_raw(cursor_colum % maxlength, crow + (cursor_colum / maxlength));
    flush();
  }
}

void
scroll_up()
{
    int k;

    for(k = conv_lines ; k > 0 ; k--){
	putchar('\n');
    }
}


void clr_line_all()
{
    throw_c(0);
    clr_end_screen();
}	    

#define STACKLENGTH 20

struct cursor_state {
  int colum;
  int upscreen;
  int reverse;
  int underline;
  int invisible;
};
static struct cursor_state savedupscreen[STACKLENGTH];
static int top = 0;

void
reset_cursor()
{
  top = 0;
}

/* 現在のカーソルの位置を(上か下か)保存する*/
/* You can push the status of cursor. 
   buf terminal can remember only one cursor point!!.
*/
void
push_cursor()
{
  if(top >= STACKLENGTH ){
    printf("Error Cursor Stack");
  }
  savedupscreen[top].upscreen = cursor_upscreen;
  savedupscreen[top].colum = cursor_colum;
  savedupscreen[top].reverse = cursor_reverse;
  savedupscreen[top].underline = cursor_underline;
  savedupscreen[top].invisible = cursor_invisible;
  top++;
  reset_cursor_status();
}

/* push_cursorで保存された位置にカーソルを飛ばす。*/
void
pop_cursor()
{
  if(top <= 0){
    printf("Error Empty Stack");
  }
  top--;
  if(savedupscreen[top].upscreen){
    kk_restore_cursor();
  }else{
    kk_save_cursor();
  }
  cursor_colum = savedupscreen[top].colum;
  cursor_reverse = savedupscreen[top].reverse;
  cursor_underline = savedupscreen[top].underline;
  cursor_invisible = savedupscreen[top].invisible;
  set_cursor_status();
  flush();
}


static int saved_cursor_rev;
static int saved_cursor_und;
void
push_hrus()
{
saved_cursor_rev = cursor_reverse;
saved_cursor_und = cursor_underline;
h_r_off();
u_s_off();
}

void
pop_hrus()
{
if(saved_cursor_rev)
  h_r_on();
if(saved_cursor_und)
u_s_on();
}

void
set_hanten_ul(x,y)
int x,y;
{
  if(!x)h_r_off();
  if(!y)u_s_off();
  if(x)h_r_on();
  if(y)u_s_on();
  flush();
}

void
set_bold(x)
int x;
{
  if(x)b_s_on();
  flush();
}

void
reset_bold(x)
int x;
{
  if (x)b_s_off();
  flush();
}
