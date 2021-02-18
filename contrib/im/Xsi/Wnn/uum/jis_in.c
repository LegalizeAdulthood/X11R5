/*
 * $Id: jis_in.c,v 1.2 1991/09/16 21:33:41 ohm Exp $
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
/*
 *	JIS 入力のためのプログラム
 *	programs for JIS NYUURYOKU
 */

#include <stdio.h>
#include <ctype.h>
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"

#define FROM_JIS 	0x21
#define TO_JIS		0x7E

static int get_jis_ichiran();

int 
in_jis()		/*  returns code for a moji  */
{
	char buffer[5];
	int code;
	int c_p = 0; /* 何文字はいったかを示す */
	int c , k;
	extern int	t_quit();
	extern int henkan_off();
	int not_redrawtmp = not_redraw;
	not_redraw = 1;	/*リドローしない */
	
	for(k=0;k < 5;k++){
	    buffer[k] = ' ';
	}

    completely_start:
	throw_c(0);	/* モード表示の後に出す */
	printf(MSG_GET(6));
	/*
	printf("JISコード: ");
	*/
	clr_line();
	flush();
    start:
	while(1){

	    throw_c(10);
	    h_r_on();
	    for(k = 0 ; k < 4 ; k++){
		putchar(buffer[k]);
	    }
	    h_r_off();
	    clr_line();
	    flush();

	    c = keyin();
	    if((c == ESC) || (t_quit == main_table[5][c])){
		throw_c(0);
		clr_line();
		flush();
		not_redraw = not_redrawtmp;
		return(-1);
	      } else if (henkan_off == main_table[5][c]){ /*きたない!*/
		henkan_off();
	    }else if(c_p < 4 && 
		     ((c <= '9' && c >= '0') || 
		      (c <= 'F' && c >= 'A') || 
		      (c <= 'f' && c >= 'a'))) {
		if ((c_p == 0 || c_p == 2) ?
			(c <= 'F' && c >= 'A') || /* For EUC */
			(c <= 'f' && c >= 'a') || /* For EUC */
			(c <= '7' && c >= '2') :
			!((buffer[c_p-1]=='2' || 
			   buffer[c_p-1]=='A' || buffer[c_p-1]=='a')
			  && c == '0' ||
			  (buffer[c_p-1]=='7' ||
			   buffer[c_p-1]=='F' || buffer[c_p-1]=='f')
			  && (c == 'F' || c == 'f'))){
			buffer[c_p++] = (char)c;
		    } else {
			ring_bell();
		    }
	    }else if(c == rubout_code && c_p){
		buffer[--c_p] = ' ';
	    }else if(c == rubout_code && c_p == 0){
		throw_c(0);
		clr_line();
		flush();
		not_redraw = not_redrawtmp;
		return(-1);
	    }else if( c == NEWLINE || c == CR ){
		if (c_p <= 1){
		    goto start;
		} else if (c_p == 4){
		    code = 0;
		    for(k = 0; k < 4; k++){
			code = code * 16 + hextodec(buffer[k]);
		    }
		    code |= 0x8080; /* ujis にして返す */
		    break;
		}
		if((code = get_jis_ichiran(buffer, c_p)) == -1){
		    goto completely_start;
		}
		break;
	    } else {
		ring_bell();
	    }
	}
	throw_c(0);
	clr_line();
	not_redraw = not_redrawtmp;
	return(code);
}

int
hextodec(c)
char c;
{
    if (c >= '0' && c <= '9'){
	return(c - '0');
    } else if (c >= 'A' && c <= 'F'){
	return(c - 'A' + 10);
    } else if (c >= 'a' && c <= 'f'){
	return(c - 'a' + 10);
    } else
	return(-1);
}


static int
get_jis_ichiran(st, num)
char *st;
int num;
{
  int from;
  int i;
  char *c;
  int maxh;
  int code;
  char buf[512];
  char *buf_ptr[96]; 

  if (num == 2){
    from = hextodec(st[0]) << (8 + 4) | hextodec(st[1]) << 8 | 0x21 ;
    from |= 0x8080;
    maxh = 94;
  }else{
    from = hextodec(st[0]) << (8 + 4) | hextodec(st[1]) << 8 |
    	   hextodec(st[2]) << 4;
    from |= 0x8080;
    maxh = 16;
    if ((from & 0xf0) == 0xf0) {
	maxh--;
    } else if ((from & 0xf0) == 0xa0) {
	maxh--;
	from++;
    }
  }

  for(i = 0,c = buf ; i < maxh; i++){
    code = from + i;
    buf_ptr[i] = c;
    *c++ = (code & 0xff00 ) >> 8;
    *c++ = (code & 0x00ff );
    *c++ = 0;
  }
  if((i = select_one_element(buf_ptr,maxh,0,"",0,0,main_table[4])) == -1){
    return(-1);
  }
  return(from + i);
}
