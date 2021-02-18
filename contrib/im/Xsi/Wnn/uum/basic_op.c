/*
 * $Id: basic_op.c,v 1.2 1991/09/16 21:33:24 ohm Exp $
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
buffer�λȤ����Ȥ�buffer�����Ƥ��ɤΤ褦�˲��̤�ɽ������뤫�Ȥ�
�ξܤ��������ϡ�buffer.h�����ϡ�screen.c�򻲾Ȥ��Ʋ�������
���Υե�����ϡ�baffer �����椹�뤿��δ��ܴؿ��ǡ�
�桼�������󥿡��ե���������Ͽ��������¾�����Ϥ˶��̤���褦�ʴؿ���
������Ƥ��ޤ���
*/
#include <stdio.h>
#include "jllib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "rk_spclval.h"
#include "buffer.h"

/** buffer��Ȥäơ���̾ʸ��������Ϥ��ޤ���
buflen�ϥХåե�����Ĺ����mode��ȿž�������ξ���
buffer����˽��ʸ�����������֤��Ʋ�������
msg�ϡ���̾���ϹԤ���Ƭ��ɽ����å�����ʸ(w_char�Ǥ��뤳�Ȥ����)
���˥������뤹����ˤϡ����Υ�å���������˥������뤷�ޤ�
����ϡ�buffer��Ȥ����٥�Τ������ϥ롼����Ǥ���
���˹��٤ʻ���򤷤����ͤϡ�buffer_in���Ѥ��Ʋ�������
*/

extern int cursor_invisible;

int
redraw_nisemono()
{
    call_t_redraw_move(c_b->t_c_p, c_b->t_c_p, c_b->maxlen, 2);
    return(0);
}


int kana_in_w_char_msg(msg , mode , buffer , buflen)
w_char *msg;
int mode;
w_char *buffer;
int buflen;
{
  struct buf c_b1, *c_btmp;
  int msg_len = Strlen(msg);
  c_btmp = c_b;
  c_b = &c_b1; 
  if((c_b->buffer =(w_char *)malloc((buflen  + msg_len)* 2)) == NULL){
      print_msg_getc(MSG_GET(2));
      return(-1);
  }
  Strcpy(c_b->buffer , msg);
  c_b->t_m_start = Strlen(c_b->buffer);
  Strcpy(c_b->buffer + msg_len , buffer);
  c_b->buf_msg = "";

  c_b->hanten = mode;
  c_b->start_col = 0;
  c_b->maxlen = Strlen(c_b->buffer);
  c_b->t_c_p = c_b->maxlen;
  c_b->buflen = buflen + msg_len;
  set_screen_vars_default();
  c_b->redraw_fun = redraw_nisemono;
  c_b->key_table = main_table[6];
  c_b->rk_clear_tbl = romkan_clear_tbl[6];
  c_b->key_in_fun = NULL;
  c_b->ctrl_code_fun = (int (*)())NULL;
  kill_buffer_offset = 0;

  init_screen();
  if(buffer_in() == -1){
      free(c_b->buffer);
      c_b = c_btmp;
      return(-1);
  }
  free(c_b->buffer);
  Strncpy(buffer , c_b->buffer + msg_len , c_b->maxlen - msg_len);
  buffer[c_b->maxlen - msg_len] = 0;
  c_b = c_btmp;
  return(0);
}

/** buffer��Ȥäơ���̾ʸ��������Ϥ��ޤ���
kana_in_w_char_msg�Ȥΰ㤤�ϡ�
msg����char�Ǥ��뤳�Ȥȡ����˥������뤹����ˤϡ����Υ�å������������
�������뤹�뤳�ȡ�
*/
int  kana_in(msg, mode, buffer, buflen)
char *msg;
int mode;
w_char *buffer;
int buflen;
{
  struct buf c_b1, *c_btmp;

  c_btmp = c_b;
  c_b = &c_b1; 

  c_b->buffer = buffer;
  c_b->buf_msg = msg;

  c_b->hanten = mode;
  c_b->start_col = strlen(msg);
  c_b->maxlen = Strlen(buffer);
  c_b->t_c_p = c_b->maxlen;
  c_b->buflen = buflen;
  c_b->t_m_start = -2;
  set_screen_vars_default();
  c_b->key_table = main_table[6];
  c_b->rk_clear_tbl = romkan_clear_tbl[6];
  c_b->key_in_fun = NULL;
  c_b->redraw_fun = redraw_nisemono;
  c_b->ctrl_code_fun = (int (*)())NULL;
  kill_buffer_offset = 0;

  init_screen();
  if(buffer_in() == -1){
      c_b = c_btmp;
      return(-1);
  }
  c_b->buffer[c_b->maxlen] = 0;
  c_b = c_btmp;
  return(0);
}

/** �������Ϥ�Ԥ����������椹�뤿��δ��ܥ롼����
�����Х��ѿ�c_b�λؤ���(struct buf)�ˡ����ʽ������򤷤Ƥ���
�����Ƥ�ǲ�������
ʸ����ϡ�c_b->buffer�����äƽ�λ���ޤ���
c_b->maxlen��ʸ�����Ĺ��������ޤ���
*/
int buffer_in()
{
  int c;
  int ret;
  int romkan;
  int ignored = 0;
  int not_redrawtmp = not_redraw;
  int tmp_send = 0;
  int in, conv_len;
  unsigned int *output;

  extern int kakutei();
  extern int send_string();
  extern int return_it();
  
  not_redraw = 0;
  while(1){

    if(cursor_invisible && !cursor_invisible_fun){
	throw_col(maxlength - 1);
	flush();
    }
    if (if_unget_buf()) {
	output = get_unget_buf();
    } else {
	in = keyin();
	output = romkan_henkan(in);
    }
    for ( ; *output != EOLTTR ; output++) {
    c = ((romkan = *output) & 0x0000ffff);
    if(cursor_invisible && !cursor_invisible_fun){
	t_throw();
    }

    if(isSPCL(romkan)){
	if(romkan == REDRAW){
		(*c_b->redraw_fun)();
		continue;
	} else if(romkan == CHMSIG) { /* mode changed */
	        if (redraw_when_chmsig_func) (*redraw_when_chmsig_func)();
	        disp_mode();
		continue;
	} else if(romkan == NISEBP) { /* error */
		ring_bell();
		continue;
	} else continue; /* if (romkan == EOLTTR) */
    }
    if (input_func) {
	if ((conv_len = (*input_func)(output, output)) > 0) {
	    c = ((romkan = *output) & 0x0000ffff);
	    output += (conv_len - 1);
	}
    }
    if (ignored){
      if (isNISE(romkan)) {
	if (c == rubout_code) {
	  --ignored;
	} else {
	  ++ignored;
	}
      } else {
	ignored = 0;
      }
      continue;
    }
	/* ��ʪ�Υ���ȥ��륳���ɤϲ���ɽ�����ʤ��ΤǸ�����rubout
	�Υ����ɤ�̵�뤹��ɬ�פ����롣ignored��̵�뤹�٤�
	rubout�θĿ����ݻ����롣*/
    if (isNISE(romkan) && (ESCAPE_CHAR(c) ||  NOT_NORMAL_CHAR(c)) &&
	c != rubout_code ){
      ++ignored;
      continue;
    }

    if(c < TBL_SIZE &&  c_b->key_table[c]){
	if (c_b->rk_clear_tbl[c])
	    romkan_clear();
	ret = (*c_b->key_table[c])(c,romkan);
    }else if(c >= 0x80 && c < 0xff){
	if(tmp_send){
		ret = return_it(c, romkan);
		tmp_send = 0;
	} else {
		ret = 0;
		ring_bell();
	}
    }else if(!(ESCAPE_CHAR(c)) || c == 0x09){	/* TAB is not ESCAPE char */
      if(c_b->maxlen < c_b->buflen){
	if(tmp_send){
		ret = return_it(c);
		tmp_send = 0;
	} else if(c_b->key_in_fun){
		ret = (*c_b->key_in_fun)(c , romkan);
	} else {
		ret = insert_char(c);
		call_t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,
				   0, 1);
		if (henkan_on_kuten && c_b->t_c_p == c_b->maxlen)
		    henkan_if_maru(c);
	}
      }else{
	romkan_clear();
	ret = 0;
      }

    }else{
      if(c_b->ctrl_code_fun){
	ret = (*c_b->ctrl_code_fun)(c,romkan);
      }else{
	ret = 0;
	ring_bell();
      }
    }
    if(ret == 1){
      not_redraw = not_redrawtmp;
      return(0);
    }else if(ret == -1){
	not_redraw = not_redrawtmp;
	return(-1);
    }
  }
  }
}

int
t_rubout(c, romkan)
int c, romkan;
{

  if(c_b->t_c_p != 0){
      backward();
      t_delete_char();
  }
  return(0);
}


void
delete_char1()
{
  if(c_b->t_c_p < c_b->maxlen){	
    Strncpy(c_b->buffer + c_b->t_c_p , c_b->buffer + c_b->t_c_p + 1,
	    c_b->maxlen - c_b->t_c_p - 1);
    c_b->maxlen -= 1;
  }
}

int
t_delete_char()
{
    delete_char1();
    call_t_redraw_move(c_b->t_c_p, c_b->t_c_p, c_b->maxlen, 2, 1);
    return(0);
}

extern int in_kuten();
extern int in_jis();

int
kuten()
{
    input_a_char_from_function(in_kuten);
    return(0);
}

int
jis()
{
    input_a_char_from_function(in_jis);
    return(0);
}

int
input_a_char_from_function(fun)
int (*fun)();
{
  int c;
  int ret = 0;
  
  if((c = (*fun)()) != -1){
      if(c_b->key_in_fun){
	  ret = (*c_b->key_in_fun)(c , c);
      }else{
	  ret = insert_char(c);
	  t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,0);
      }
  }
  redraw_line();
  return(ret);
}

int t_kill()
{
  kill_buffer_offset = min(c_b->maxlen - c_b->t_c_p , maxchg);
  Strncpy(kill_buffer , c_b->buffer + c_b->t_c_p , kill_buffer_offset);
  c_b->maxlen = c_b->t_c_p;
  t_redraw_move(c_b->t_c_p ,c_b->t_c_p, c_b->maxlen,1);
  return(0);
}

#ifdef nodef
#ifdef	CONVERT_by_STROKE
int errort_kill()
{
/* I think that should not save to kill-buffer KURI
  kill_buffer_offset = min(c_b->maxlen - c_b->t_c_p , maxchg);
  Strncpy(kill_buffer , c_b->buffer + c_b->t_c_p , kill_buffer_offset);
*/
  c_b->maxlen = c_b->t_c_p;
  return(0);
}
#endif
#endif

int t_yank()
{
  if(kill_buffer_offset <= c_b->buflen - c_b->t_c_p){
    Strncpy(c_b->buffer + c_b->t_c_p + kill_buffer_offset,
	      c_b->buffer + c_b->t_c_p, c_b->maxlen - c_b->t_c_p);
    Strncpy(c_b->buffer + c_b->t_c_p,
	      kill_buffer, kill_buffer_offset);
    c_b->maxlen += kill_buffer_offset;
    call_t_redraw_move(c_b->t_c_p + kill_buffer_offset, c_b->t_c_p,
		       c_b->maxlen,0,1);
  }
  return(0);
}  

int t_ret()
{
  return(1);
}

int
t_quit()
{
  return(-1);
}


int 
c_top()
{
    (*c_top_func)();
    return(0);
}

int 
c_end()
{
    (*c_end_func)();
    return(0);
}

int
c_end_nobi()
{
    (*c_end_nobi_func)();
    return(0);
}

int t_jmp_backward()
{
  int k;
  
  for(k = 1 ; k < touroku_bnst_cnt; k++){
    if(touroku_bnst[k] >= c_b->t_c_p){
      if(touroku_bnst[k - 1] >= c_b->t_m_start){
	  call_t_redraw_move_1(touroku_bnst[k - 1], touroku_bnst[k - 1],
			       c_b->maxlen, 0, 1, 0);
      }else{
	  c_top();
      }
      return(0);
    }
  }
  backward();
  return(0);
}

int t_jmp_forward()
{
  int k;
  
  for(k = 1 ; k < touroku_bnst_cnt; k++){
    if(touroku_bnst[k] > c_b->t_c_p){
	call_t_redraw_move_1(touroku_bnst[k ], c_b->t_c_p,
			     c_b->maxlen, 0, 1, 0);
	return(0);
    }
  }
  forward();
  return(0);
}


int
forward()
{
    if(c_b->t_c_p < c_b->maxlen){
	call_t_redraw_move_2(c_b->t_c_p + 1, c_b->t_c_p,
                             c_b->t_m_start, c_b->t_c_p + 2,
                             c_b->t_c_p + 1, 0 ,1);
    }
    return(0);
}

int
backward()
{
    if(c_b->t_c_p >= 1 && c_b->t_c_p > c_b->t_m_start){
        call_t_redraw_move_2(c_b->t_c_p - 1, c_b->t_c_p - 1,
                             c_b->t_m_start, c_b->t_c_p + 1,
                             c_b->t_c_p, 0 ,1);
    }
    return(0);
}

int
insert_char(c)
w_char c;
{
    if(c_b->maxlen < c_b->buflen){
	Strncpy(c_b->buffer + c_b->t_c_p + 1, c_b->buffer + c_b->t_c_p, c_b->maxlen - c_b->t_c_p);
	c_b->maxlen += 1;
	c_b->buffer[c_b->t_c_p] = c;
    }
    return(0);
}

#ifdef	nodef
int
insert_string(string, len)
w_char *string;
int len;
{
  if(c_b->maxlen + len <= c_b->buflen){
    Strncpy(c_b->buffer + c_b->t_c_p + len, c_b->buffer + c_b->t_c_p , c_b->maxlen - c_b->t_c_p);
    Strncpy(c_b->buffer + c_b->t_c_p, string , len);
    c_b->maxlen += len;
  }
}
#endif

/** vlen��duplicate��Ŭ���Ȼפ�����֤����ꤷ�ޤ���*/
void
set_screen_vars_default()
{
  c_b->vlen = maxlength * conv_lines - c_b->start_col - 3 - disp_mode_length ;
  c_b->vst = 0;
  c_b->duplicate = max((c_b->vlen + 3) >> 2 , 2);
}

int
quote()
{
    int c;
    c = keyin();
    if (empty_modep() && c_b->key_in_fun)
	return ((*c_b->key_in_fun)(c, c));
    insert_char(c);
    t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,0);
    return(0);
}

