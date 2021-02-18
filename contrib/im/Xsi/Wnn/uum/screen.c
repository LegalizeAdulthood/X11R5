/*
 * $Id: screen.c,v 1.2 1991/09/16 21:33:51 ohm Exp $
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

#include "commonhd.h"
#include "config.h"
#ifdef SYSVR2
#include <string.h>
#endif
#ifdef BSD42
#include <strings.h>
#endif
#include "sdefine.h"
#include "sheader.h"
#include "buffer.h"

/*
 ���Υե�����ϡ�����������ʬ�ȤΥ��󥿡��ե�������Ԥ��ؿ���
������Ƥ��롣�����δؿ����Ѥ����ˡ����̤����椷�ƤϤʤ�ʤ���
���Υե����뤫��extern ����Ƥ���ؿ��򼨤���
�ʤ������̤�����ϡ�c_b->buffer ����ǤΥ���ǥå������Ѥ���
�Ԥ��롣����c_b->buffer ����Τɤ���ʬ�����̤˽Ф���Ƥ��뤫�ϰռ����ʤ�
�Ƥ褤��
t_move(x) : ��������� x �����Ф�����ɥ���ɬ��(ȿž��������Τ���)
����ʬ�⾡��˹ͤ��ƹԤä����롣
t_redraw_move(x , start , end,clr_l) :buffer ����ǡ�start ���� end �ޤǤ�
�Хåե������Ƥ��Ѥ���줿���ˡ����������x �����Ф��Τ��Ѥ��롣
print_buf_msg(msg) :��å�������ʬ��ɽ�����롣
disp_mode():romkan �Υ⡼�ɤ�ɽ�����롣
disp_mode_line():romkan �Υ⡼�ɤ�ɽ�����롣
t_print_l(): ���̤Υ�ɥ����롣

t_throw(): ������������Ф�
init_screen():���̴ط�(vst)�򥤥˥���饤����������ɽ����Ԥ���
*/

/* ʸ���β��̾�Ǥ��礭�����֤���*/

static int col = 0; /** startichi����β��̾�Υ���������� */
static int oldmaxcol = 0; /** redraw sita tokini doko made mae ni kaita data ga nokotteiruka wo simesu */

#define CHAR_BYTE 2

#define w_putchar_move(x) {col += w_putchar(x);}
#define putchar_move(x) {col += 1; putchar_norm(x);}
#define putchar_move1(x) {col += 1; putchar1(x);}

#define throw0(x)	throw_c((x) + c_b->start_col)
/*
#define throw(x) {int tmptmp = (x);if(cs == 0 || col != tmptmp){throw0((col = tmptmp) + 1);}}
*/

/* ���������col �����Ф��Τˤϡ�throw��
�Ѥ��롣 */
#define MARJIN 1 /* �������뤬��ü���餳������ν�ˤ���С����β��̤˹Ԥ�*/

static void t_cont_line_note();
static int find_character_on_that_col(), set_vst();

void
throw(x)
int x;
{
    throw0((col = x) + 1);
}

int
char_len(x)
w_char x;
{
    return((*char_len_func)(x));
}

/* vst �򥻥åȤ�ľ���ơ���ɥ�����*/
void t_redraw_one_line()
{
  if(c_b->vst){
    t_cont_line_note();
  }else{
    t_cont_line_note_delete();
  }
  t_print_line(c_b->vst, c_b->maxlen,1);
}

void
init_screen()
{
    int tmp;

    oldmaxcol = 0;  
    tmp = cur_ichi(c_b->t_c_p , 0);
    if((tmp >= c_b->vlen - c_b->duplicate) &&
       (tmp % (c_b->vlen - c_b->duplicate)) < c_b->duplicate - 1){
	c_b->vst = find_character_on_that_col(
	    (tmp / (c_b->vlen - c_b->duplicate) - 1)*(c_b->vlen - c_b->duplicate), 0);
    }else{
	c_b->vst = find_character_on_that_col(
	    (tmp / (c_b->vlen - c_b->duplicate))*(c_b->vlen - c_b->duplicate), 0);
    }
    if(c_b->maxlen || *c_b->buf_msg)
	t_print_l();
}

/* hituyouga areba vst wo settosinaosite, settosinaosita tokiniha 1 wo kaesu */
int
check_vst()
{
    int k = -1;

  if((c_b->t_c_p < c_b->vst) ||
     (cur_ichi(c_b->t_c_p , c_b->vst) >= (c_b->vlen - MARJIN )) ||
     (cur_ichi(c_b->t_m_start , c_b->vst) > (c_b->vlen - MARJIN ))){
      k = set_vst();
  }
  if(k == -1){
      return(0);
  }else{
      return(1);
  }
}

int
t_redraw_move(x , start , end,clr_l)
int x;
int start;
int end;
int clr_l;
{
    (*t_redraw_move_func)(x , start , end,clr_l);
    return(0);
}

int
t_move(x)
int x;
{
  int old_cp = c_b->t_c_p;

  if(((c_b->hanten >> 2) & 0x3) != ((c_b->hanten >> 4) & 0x3)){
    t_redraw_move(x , min(old_cp, x), max(old_cp, x),0);
    return(0);
  }
  if(x >= c_b->maxlen)x = c_b->maxlen;
  c_b->t_c_p = x;
  if(check_vst()){
      t_redraw_one_line();
  }
  throw(cur_ichi(c_b->t_c_p , c_b->vst));
  flush();
  return(0);
}

int
t_print_l()
{
  (*t_print_l_func)();
  return(0);
}

#define set_screen_reverse(X,Y){set_hanten_ul(c_b->hanten & X, c_b->hanten & Y);}
#define reset_screen_reverse(X,Y){flushw_buf(); if(c_b->hanten & X)h_r_off();if(c_b->hanten & Y)u_s_off(); flush();}

#define set_screen_bold(X){flushw_buf(); set_bold(c_b->hanten & X);}
#define reset_screen_bold(X){flushw_buf(); reset_bold(c_b->hanten & X);flush();}

/* st must be bigger than vst */


/* ���Υե�����ǰ��ֽ��פʴؿ���
c_b->buffer ����ǡ�st ���� end �ޤǤǲ��̤˸���Ƥ����(vst�����)��
ɽ����ľ�������������⤹�롣
��С����ξ����ϡ�c_b->hanten�ˤ�äƷ����롣
*/

void
t_print_line(st , end, clr_l) 
int st , end, clr_l;
{
  register int k;
  register int col1;
  register int end_of_line = maxlength - disp_mode_length - 1 - c_b->start_col ;
  int tmp;

  int mst = min(c_b->t_m_start , c_b->t_c_p);
  int mend = max(c_b->t_m_start , c_b->t_c_p);

  int bst = c_b->t_b_st;
  int bend = c_b->t_b_end;
  int bold = 0;
  
  if(end > c_b->maxlen)end = c_b->maxlen;
  col1 = cur_ichi(st , c_b->vst);
  if(col1 == -1){
    col1 = 0;
    k = st = c_b->vst;
  }else{
    k = st;
  }

  for(;end_of_line <= col1 ; end_of_line += maxlength);

  throw(col1);
  if(mst >= 0){
    if(st < mst){
      set_screen_reverse(0x01,0x02);
      for(k = st ; k <mst ; k++){

	if(bold == 0 && k >= bst && k < bend) {
	  set_screen_bold(0x40);
	  bold = 1;
	} else if (bold && (k < bst || k >= bend)) {
	  reset_screen_bold(0x40);
	  set_screen_reverse(0x01,0x02);
	  bold = 0;
	}
	if(k >= end){
	  reset_screen_reverse(0x01,0x02);
	  reset_screen_bold(0x40);
	  bold = 0;
	  goto normal_out;
	}
	tmp =  char_len(c_b->buffer[k]);
	if(tmp + col > c_b->vlen){
	  reset_screen_reverse(0x01,0x02);
	  reset_screen_bold(0x40);
	  bold = 0;
	  goto end_syori;
	}
	w_putchar_move(c_b->buffer[k]);
	if(col > end_of_line - CHAR_BYTE){
	    flushw_buf();
	    if(col  < end_of_line){
		reset_screen_reverse(0x01,0x02);
	  	reset_screen_bold(0x40);
	  	bold = 0;
		putchar_move(' ');
		set_screen_reverse(0x01,0x02);
		if(k >= bst && k < bend) {
	  	  set_screen_bold(0x40);
	  	  bold = 1;
		}
	    }
	    throw(end_of_line);
	    end_of_line += maxlength;
	}

      }
      reset_screen_reverse(0x01,0x02);
      reset_screen_bold(0x40);
      bold = 0;
    }
    if(k < mend){
      set_screen_reverse(0x04,0x08);
      for(; k < mend ; k++){

	if(bold == 0 && k >= bst && k < bend) {
	  set_screen_bold(0x40);
	  bold = 1;
	} else if (bold && (k < bst || k >= bend)) {
	  reset_screen_bold(0x40);
	  set_screen_reverse(0x04,0x08);
	  bold = 0;
	}

	if(k >= end){
	  reset_screen_reverse(0x04,0x08);
	  reset_screen_bold(0x40);
	  bold = 0;
	  goto normal_out;
	}
	tmp = char_len(c_b->buffer[k]);
	if(col + tmp > c_b->vlen ){
	  reset_screen_reverse(0x04,0x08);
	  reset_screen_bold(0x40);
	  bold = 0;
	  goto end_syori;
	}
	w_putchar_move(c_b->buffer[k]);
	if(col > end_of_line - CHAR_BYTE){
	    flushw_buf();
	    if(col < end_of_line){
		reset_screen_reverse(0x04,0x08);
	  	reset_screen_bold(0x40);
	  	bold = 0;
		putchar_move(' ');
		set_screen_reverse(0x04,0x08);
	  	if(k >= bst && k < bend) {
	    	  set_screen_bold(0x40);
	    	  bold = 1;
	  	}
	    }
	    throw(end_of_line);
	    end_of_line += maxlength;
	}
      }
      reset_screen_reverse(0x04,0x08);
      reset_screen_bold(0x40);
      bold = 0;
    }
  }
  if(k < c_b->maxlen && k < end){
      set_screen_reverse(0x10,0x20);
      for(; k <c_b->maxlen ; k++){

	  if(bold == 0 && k >= bst && k < bend) {
	    set_screen_bold(0x40);
	    bold = 1;
	  } else if (bold && (k < bst || k >= bend)) {
	    reset_screen_bold(0x40);
	    set_screen_reverse(0x10,0x20);
	    bold = 0;
	  }

	  if(k >= end){
	      reset_screen_reverse(0x10,0x20);
	      reset_screen_bold(0x40);
	      bold = 0;
	      goto normal_out;
	  }
	  tmp = char_len(c_b->buffer[k]);
	  if(col + tmp > c_b->vlen ){
	      reset_screen_reverse(0x10,0x20);
	      reset_screen_bold(0x40);
	      bold = 0;
	      goto end_syori;
	  }
	  w_putchar_move(c_b->buffer[k]);
	  if(col > end_of_line - CHAR_BYTE){
	      flushw_buf();
	      if(col < end_of_line ){
		  reset_screen_reverse(0x10,0x20);
	      	  reset_screen_bold(0x40);
	      	  bold = 0;
		  putchar_move(' ');
		  set_screen_reverse(0x10,0x20);
	  	  if(k >= bst && k < bend) {
	    	    set_screen_bold(0x40);
	    	    bold = 1;
	  	  }
	      }
	      throw(end_of_line);
	      end_of_line += maxlength;
	  }
      }
      reset_screen_reverse(0x10,0x20);
      reset_screen_bold(0x40);
      bold = 0;
  }

 normal_out:
  if(clr_l == 1){
    clr_line();
    oldmaxcol = col;
  }else if(clr_l == 2){
    clr_line();
    oldmaxcol = col;
/*      k = oldmaxcol - col;
      oldmaxcol = col;
      if(k > 0){
	  push_hrus();
	  for(; k > 0 ; k--){
	      putchar_move1(' ');		
	  }
	  pop_hrus();
      }
*/
  }else{
      oldmaxcol = max(oldmaxcol , col);
  }
  flush();
  return;

 end_syori:
    for(k = col ; k <= c_b->vlen ; k++){
	putchar_move('$');
  }
  oldmaxcol = col;
  flush();
  return;
}

/* �Ԥ���Ƭ���� */
static void
t_cont_line_note()
{
  throw0(0);
  putchar_norm('$');
}

/* �Ԥ���Ƭ���� */
void
t_cont_line_note_delete()
{
  throw0(0);
  putchar_norm(' ');
}

/*���̤Υ����0���Хåե�����start_point �λ��������c �ˤ���ʸ�����֤�*/
static int
find_character_on_that_col(c , start_point)
int c;     /* colum */
int start_point; /* in_buffer as vst */
{

  int k;
  register int end_of_line = maxlength - disp_mode_length - 1 - c_b->start_col ;
  int len = 0;
  for(k = start_point ;k <= c_b->maxlen ;k++ ){
    len += char_len(c_b->buffer[k]);
    if(len >= c) return(k);
    if(len  > end_of_line - CHAR_BYTE){
	len = end_of_line;
	end_of_line += maxlength;
    }
  }	
  /*error but default to 0 */
  return(0);
}

/*���̤Υ����0���Хåե�����start_point �λ���ʸ��cp�β��̾�ΰ��֤��֤���*/
/* static */
int
cur_ichi(cp , start_point)
register int cp;
int start_point;
{
    register int k;
    register int end_of_line = maxlength - disp_mode_length - 1 - c_b->start_col ;
    register int len = 0;
    if(cp < start_point)return(-1);

    for(k = start_point ;k < cp ;k++ ){
	len += char_len(c_b->buffer[k]);
	if(len  > end_of_line - CHAR_BYTE){
	    len = end_of_line;
	    end_of_line += maxlength;
	}
    }	
    return(len);
}


void
print_buf_msg(msg)
char *msg;
{
  push_cursor();
  throw_c(0);
  printf(msg);
  pop_cursor();
  flush();
}

/* vst�򥻥åȤ���*/
/* returns -1 if not changed
   else returns new start colum
*/
static int
set_vst()
{
  int tmp;
  int vst1;

  tmp = cur_ichi(c_b->t_c_p , 0);
  vst1 = find_character_on_that_col((tmp / (c_b->vlen - c_b->duplicate))*(c_b->vlen - c_b->duplicate), 0);
  if(cur_ichi(c_b->t_m_start , vst1) >= c_b->vlen){
      vst1 = c_b->t_c_p;
  }
  c_b->vst = vst1;
  return(vst1);
}


static char rk_modes[80];
extern char *romkan_dispmode(), *romkan_offmode();

char *
get_rk_modes()
{
    char	*p;
#ifndef SYSVR2
    extern char *index();
#else
    extern char *strchr();
#endif

    strcpy(rk_modes, (NULL == (p = romkan_dispmode()) ? "[   ]" : p));
#ifndef SYSVR2
    if ((p = (char*)index(rk_modes, ':')) != NULL) {
#else
    if ((p = (char*)strchr(rk_modes, ':')) != NULL) {
#endif
        set_cur_env(*(++p));
        *p = '\0';
    }
    return(rk_modes);
}

int
disp_mode()
{
  push_cursor();
  throw_col(0);
  printf("%s",get_rk_modes());
  pop_cursor();
  flush();
  return(0);
}

/* cursor status is saved before call it */
void
display_henkan_off_mode()
{
  char  *p;

  strcpy(rk_modes, (NULL == (p = romkan_offmode()) ? "[---]" : p));
  throw_col(0);
  printf("%s",rk_modes);
  kk_restore_cursor();
  flush();
}


/* ������������Ф�*/
void
t_throw()
{
    throw0(col + 1);
    flush();
}


void
clr_line()
{
    clr_end_screen();
}	    

