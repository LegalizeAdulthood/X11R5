/*
 * $Id: touroku.c,v 1.2 1991/09/16 21:35:28 ohm Exp $
 */
/*
 * Copyright 1989, 1990, 1991 by OMRON Corporation
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON and MIT not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON and MIT make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * OMRON, MIT AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL OMRON, MIT OR WNN CONSORTIUM BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	Author:	Seiji Kuwari	OMRON Corporation
 *				kuwa@omron.co.jp
 *				kuwa%omron.co.jp@uunet.uu.net
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
#include "jslib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"
#include "xext.h"

static int touroku_start,touroku_end;
/* these are used only for making yomi string for registered kanji */


int touroku_mode = 0;

static void
xw_exec_mode()
{
    unsigned int c1;
    int ret;

    for(;;) {
	c1 = keyin();
	if (xjutil->sel_ret == -2) {
	    touroku_mode = CANCEL;
	    xjutil->sel_ret = -1;
	    return;
	} else if(xjutil->sel_ret == 0) {
	    touroku_mode = TOUROKU_GO;
	    xjutil->sel_ret = -1;
	    return;
	} else if (xjutil->sel_ret == 1) {
	    if (xjutil->jutil->mode == MESSAGE1) {
		touroku_mode = KANJI_IN_START;
	    } else if (xjutil->jutil->mode == MESSAGE2) {
		touroku_mode = YOMI_IN;
	    } else if (xjutil->jutil->mode == MESSAGE3) {
		touroku_mode = HINSI_IN;
	    } else if (xjutil->jutil->mode == MESSAGE4) {
		touroku_mode = JISHO_IN;
	    }
	    xjutil->sel_ret = -1;
	    return;
	} else if ((c1 < 256) && (main_table[4][c1] != NULL)) {
	    if ((ret = (*main_table[4][c1])()) == 1) {
		touroku_mode = TOUROKU_GO;
		return;
	    } else if (ret == -1) {
		touroku_mode = CANCEL;
		return;
	    } else {
		ring_bell();
	    }
	} else {
	    ring_bell();
	}
    }
}

static int
xw_hani_settei(buffer , buflen)
w_char *buffer;
int buflen;
{
  ClientBuf *c_btmp , c_b1;

    c_btmp = c_b;
    c_b = &c_b1; 

    c_b->buffer = buffer;
    c_b->buflen = buflen;

    c_b->maxlen = Strlen(buffer);

    c_b->hanten = 0x04;
    c_b->t_c_p = touroku_bnst[cur_bnst_];
    hanten_jutil_mes_title(MESSAGE1,1);
    c_b->t_m_start = -1;

    c_b->key_table = main_table[5];
    c_b->rk_clear_tbl = romkan_clear_tbl[5];
    c_b->key_in_fun = NULL;
    c_b->redraw_fun = redraw_nisemono;
    c_b->ctrl_code_fun = NULL;
    init_screen();
    if(buffer_in() == -1) {
	if (touroku_mode == KANJI_IN_START || touroku_mode == KANJI_IN_END)
	    touroku_mode = CANCEL;
	/*
	    touroku_mode = KANJI_IN_START;
	*/
	c_b = c_btmp;
	return(-1);
    }
    if (xjutil->sel_ret == -2) {
	touroku_mode = CANCEL;
	xjutil->sel_ret = -1;
	c_b = c_btmp;
	return(-1);
    } else if (xjutil->sel_ret == 0) {
	touroku_mode = TOUROKU_GO;
	xjutil->sel_ret = -1;
	c_b = c_btmp;
	return(0);
    } else if (xjutil->sel_ret == 1) {
        if (xjutil->jutil->mode != MESSAGE1) {
	    if(xjutil->jutil->mode == MESSAGE2) {
		touroku_mode = YOMI_IN;
	    } else if(xjutil->jutil->mode == MESSAGE3) {
		touroku_mode = HINSI_IN;
	    } else if(xjutil->jutil->mode == MESSAGE4) {
		touroku_mode = JISHO_IN;
	    }
        } else {
	    touroku_mode = KANJI_IN_START;
	    c_b = c_btmp;
	    return(0);
	}
    } else {
	touroku_mode = YOMI_IN;
    }
    Strncpy(buffer, buffer + c_b->t_m_start, c_b->t_c_p - c_b->t_m_start);
    buffer[c_b->t_c_p - c_b->t_m_start] = 0;
    touroku_start = c_b->t_m_start;
    touroku_end = c_b->t_c_p;
    c_b = c_btmp;
    return(c_b1.t_c_p - c_b1.t_m_start);
}

void
touroku()
{
    int i , k;
    int ud_no, real_dic_no;
    w_char yomibuf[1024];
    char *message[4];
    char *message1[4];
    w_char hani_buffer[1024];
    w_char get_buffer[1024];
    char dic_name_heap[2048];
    char *hp = dic_name_heap;
    int hinsi = 0;
    int tmp;
    char *dict_name[JISHOKOSUU];
    int current_dic = -1;
    int current = -1;
    int size;
    
    cur_bnst_ = get_touroku_data(get_buffer);
    if(update_dic_list() == -1){
	errorkeyin();
	return;
    }
    for(k = 0 , size = 0 ; k < dic_list_size ; k++){
	if((dicinfo[k].type == WNN_UD_DICT || dicinfo[k].type == WNN_REV_DICT
#ifdef	CONVERT_with_SiSheng
	|| dicinfo[k].type == CWNN_REV_DICT
#endif	/* CONVERT_with_SiSheng */
	) && dicinfo[k].rw == 0){
	    size++;
	}
    }
    if(size == 0){
	print_msg_getc(msg_get(cd, 99, default_message[99], xjutil->lang),
		       NULL, NULL, NULL);
    }
    for(ud_no = 0, size = 0 ; ud_no < dic_list_size; ud_no++){
	if((dicinfo[ud_no].type == WNN_UD_DICT || dicinfo[ud_no].type == WNN_REV_DICT
#ifdef	CONVERT_with_SiSheng
	|| dicinfo[ud_no].type == CWNN_REV_DICT
#endif	/* CONVERT_with_SiSheng */
	   ) && dicinfo[ud_no].rw == 0){
	    if(*dicinfo[ud_no].comment){
		sStrcpy(hp,dicinfo[ud_no].comment);
		dict_name[size] = hp;
		hp += strlen(hp) + 1;
	    }else{
		dict_name[size] = dicinfo[ud_no].fname;
	    }
	    if(current_dic != -1 && dicinfo[ud_no].dic_no == current_dic)
		current = size;
	    size++;
	}
    }
/*
    if (size == 1)
*/
	    ud_no = 0;
    message[0] = msg_get(cd, 93, default_message[93], xjutil->lang);
    message[1] = msg_get(cd, 95, default_message[95], xjutil->lang);
    message[2] = msg_get(cd, 96, default_message[96], xjutil->lang);
    message[3] = msg_get(cd, 97, default_message[97], xjutil->lang);
    message1[0] = message1[1] = message1[2] = NULL;
    message1[3] = dict_name[((current == -1)? ud_no:current)];

    init_jutil(msg_get(cd, 92, default_message[92], xjutil->lang),
	       msg_get(cd, 92, default_message[92], xjutil->lang),
	       4,message, message1);
    touroku_mode = KANJI_IN_START;

repeat:
    for(;;) {
	switch(touroku_mode) {
    	    case KANJI_IN_START:
		change_cur_jutil(MESSAGE1);
		change_mes_title(MESSAGE1,
				 msg_get(cd, 93, default_message[93],
					 xjutil->lang),
				 1);
		kk_cursor_invisible();
		JWMline_clear(0);
		kk_cursor_normal();
		hani_buffer[0] = 0;
		Strcpy(hani_buffer, get_buffer);
		if((tmp = xw_hani_settei(hani_buffer , 1024)) < 0)
			break;
		if(Strlen(hani_buffer) >= 64) {
		    print_msg_getc(msg_get(cd, 100, default_message[100],
					   xjutil->lang),
				   NULL, NULL, NULL);
		    touroku_mode = KANJI_IN_START;
		}
		kk_cursor_invisible();
		break;
    	    case YOMI_IN:
		for(i = 0 ; i < LENGTHYOMI + 1 ; i++){
		    yomibuf[i] = 0;
		}
		yomibuf[LENGTHYOMI] = '0';
		change_cur_jutil(MESSAGE2);
		hanten_jutil_mes_title(MESSAGE2,1);
		JWMline_clear(0);
		kk_cursor_normal();
		xjutil->sel_ret = -1;
		kana_in_w_char_msg((0x08 | 0x20), yomibuf, LENGTHYOMI);
		if (xjutil->sel_ret == -2) {
		    touroku_mode = CANCEL;
		} else if (xjutil->sel_ret == 0) {
		    touroku_mode = TOUROKU_GO;
		} else if (xjutil->sel_ret == 1) {
      		    if (xjutil->jutil->mode == MESSAGE1) {
	  		    touroku_mode = KANJI_IN_START;
      		    } else if (xjutil->jutil->mode == MESSAGE2) {
	  		    touroku_mode = YOMI_IN;
      		    } else if (xjutil->jutil->mode == MESSAGE3) {
	  		    touroku_mode = HINSI_IN;
      		    } else if (xjutil->jutil->mode == MESSAGE4) {
	  		    touroku_mode = JISHO_IN;
      		    }
		} else {
		    touroku_mode = HINSI_IN;
		}
		hanten_jutil_mes_title(MESSAGE2,0);
		kk_cursor_invisible();
		break;
    	    case HINSI_IN:
		change_cur_jutil(MESSAGE3);
		hanten_jutil_mes_title(MESSAGE3,1);
		JWMline_clear(0);
		kk_cursor_invisible();
		hinsi = hinsi_in();
		hanten_jutil_mes_title(MESSAGE3,0);
		if (touroku_mode == HINSI_IN)
		    touroku_mode = JISHO_IN;
		break;
    	    case JISHO_IN:
		if(size == 1){
		    ud_no = 0;
		}else{
		    change_cur_jutil(MESSAGE4);
		    hanten_jutil_mes_title(MESSAGE4,1);
		    JWMline_clear(0);
		    kk_cursor_invisible();
		    ud_no = xw_select_one_element(dict_name,size,current,
						  msg_get(cd, 98,
							  default_message[98],
							  xjutil->lang),
						  0, NULL, NULL, NULL);
		    hanten_jutil_mes_title(MESSAGE4,0);
		    xw_jutil_write_msg(dict_name[ud_no]);
		    if (ud_no == -3) {
			touroku_mode = EXECUTE;
		    } else if (ud_no == -1) {
			touroku_mode = CANCEL;
		    }
		}
		if (touroku_mode == JISHO_IN)
		    touroku_mode = EXECUTE;
		break;
    	    case EXECUTE:
		xw_exec_mode();
		break;
    	    case TOUROKU_GO:
		goto do_touroku;
		break;
    	    case CANCEL:
		end_jutil();
 		return;
	}
    }

do_touroku:
    for (real_dic_no = 0; real_dic_no < dic_list_size; real_dic_no++) {
	if((dicinfo[real_dic_no].type == WNN_UD_DICT ||dicinfo[real_dic_no].type == WNN_REV_DICT
#ifdef	CONVERT_with_SiSheng
	|| dicinfo[real_dic_no].type == CWNN_REV_DICT
#endif	/* CONVERT_with_SiSheng */
	   ) && dicinfo[real_dic_no].rw == 0){
	    ud_no--;
	    if (ud_no < 0)
		break;
	}
    }
    current_dic = real_dic_no;

    if(js_word_add(cur_env, dicinfo[real_dic_no].dic_no, yomibuf, hani_buffer,
		   "", hinsi, 0) == -1){
	if_dead_disconnect(cur_env, -1);
        errorkeyin();
        touroku_mode = EXECUTE;
	goto repeat;
    }
    end_jutil();
    return;
}


static int
find_end_of_tango(c)
int c;
{
  int k;
  int jisyu; /* 0: katakana 1: hiragana 2:ascii 3:kanji */

  if(KATAP(c_b->buffer[c]))jisyu = 0;
  else if(HIRAP(c_b->buffer[c]))jisyu = 1;
  else if(ASCIIP(c_b->buffer[c])) jisyu = 2;
  else if(KANJIP(c_b->buffer[c])) jisyu = 3;
  else return(c + 1);

  for(k = c + 1; k < c_b->maxlen ; k++){
    if(jisyu == 0){
      if(!KATAP(c_b->buffer[k]))break;
    }else if(jisyu == 1){
      if(!HIRAP(c_b->buffer[k]))break;
    }else if(jisyu == 2){
      if(!ASCIIP(c_b->buffer[k]))break;
    }else if(jisyu == 3){
      if(!KANJIP(c_b->buffer[k]))break;
    }
  }
  return(k);
}

int
t_markset()
{
    if (xjutil->sel_ret == -2) {
	touroku_mode = CANCEL;
	hanten_jutil_mes_title(MESSAGE1,0);
	xjutil->sel_ret = -1;
        return(-1);
    }
  
    if(c_b->t_m_start == -2) {
	hanten_jutil_mes_title(MESSAGE1,0);
        return(0);
    }
    if(c_b->t_m_start == -1) {
	if(xjutil->jutil->mode != MESSAGE1) {
            touroku_mode = KANJI_IN_START;
	    change_mes_title(MESSAGE1,
			     msg_get(cd, 93, default_message[93], xjutil->lang),
			     1);
            return(0);
	}
        if(c_b->t_c_p == c_b->maxlen){
            touroku_mode = KANJI_IN_END;
	    change_mes_title(MESSAGE1,
			     msg_get(cd, 94, default_message[94], xjutil->lang),
			     1);
            return(0);
        }
        c_b->t_m_start = c_b->t_c_p;
        touroku_mode = KANJI_IN_END;
	change_mes_title(MESSAGE1,
			     msg_get(cd, 94, default_message[94], xjutil->lang),
			     1);
	call_t_redraw_move_1(find_end_of_tango(c_b->t_c_p), c_b->t_c_p,
			     find_end_of_tango(c_b->t_c_p), 0, 0, 0);
        kk_cursor_invisible();
        return(0);
    }else{
	if(c_b->t_m_start == c_b->t_c_p) {
            c_b->t_m_start = -1;
            touroku_mode = KANJI_IN_START;
	    change_mes_title(MESSAGE1,
			     msg_get(cd, 93, default_message[93], xjutil->lang),
			     1);
            kk_cursor_normal();
            return(-1);
	} else {
	    if (xjutil->jutil->mode == MESSAGE1)
		xjutil->jutil->mode = MESSAGE2;
            hanten_jutil_mes_title(MESSAGE1,0);
            return(1);
        }
    }
}


