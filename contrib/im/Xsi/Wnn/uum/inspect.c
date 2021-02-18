/*
 * $Id: inspect.c,v 1.2 1991/09/16 21:33:37 ohm Exp $
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
#include "jllib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "config.h"
#include "buffer.h"

int
inspect(bun_no)
int bun_no;
{
    char buf[1024];
    char *c = buf;

  if(make_string_for_ke(bun_no,buf,sizeof(buf)) == -1){
    print_msg_getc("Error in inspect");
    return(0);
  }
  select_line_element(&c,1,0,"",0,0,main_table[10]);
  return(1);
}

static int next_flag;
#define I_NEXT 1
#define I_PREV 2
#define I_CUR 3

int
inspect_kouho()
{
  push_cursor();
  next_flag = I_CUR;
  while(next_flag){
      if(next_flag == I_NEXT || next_flag == I_PREV){
	  if (jl_zenkouho_bun(bun_data_) != cur_bnst_ ||
	      jl_zenkouho_daip(bun_data_) != WNN_SHO){
	      jl_zenkouho(bun_data_, cur_bnst_, WNN_USE_ZENGO, WNN_UNIQ);
	  }
	  if(next_flag == I_NEXT){
	      jl_next(bun_data_);
	  }else{
	      jl_previous(bun_data_);
	  }
	  make_kanji_buffer(cur_bnst_);
	  c_b->t_m_start = bunsetsuend[cur_bnst_ ];
	  c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
	  c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
      }
      next_flag = 0;
      inspect(cur_bnst_);
  }
  pop_cursor();
  t_print_l();
  return(0);
}

/*
 *  Very Dirty, but we permit only this inspect function to manipulate
 *  directly the contents of bun_data_.
 */

static int Bun_no;

int
make_string_for_ke(bun_no,buf,buf_size)
int bun_no;
char *buf;
int buf_size;
{
    struct wnn_jl_bun *bun;
    w_char buf1[1024];
    w_char save_c;
    char tmp[WNN_HINSI_NAME_LEN * 2];
    
    Bun_no = bun_no;
    bun = bun_data_->bun[bun_no];
    jl_get_kanji(bun_data_, bun_no, bun_no + 1, buf1);
    if (print_out_func) (*print_out_func)(buf1, buf1, Strlen(buf1));
    sStrcpy(buf,buf1);
    strcat(buf," ");
    jl_get_yomi(bun_data_, bun_no, bun_no + 1, buf1);
    if (print_out_func) (*print_out_func)(buf1, buf1, Strlen(buf1));
    save_c = *(buf1 + bun->jirilen);
    *(buf1 + bun->jirilen) = 0;
    sStrcpy(buf + strlen(buf), buf1);
    *(buf1 + bun->jirilen) = save_c;
    strcat(buf, "-");
    sStrcpy(buf + strlen(buf), buf1 + bun->jirilen);

    set_escape_code(buf);

    strcat(buf,"   (");
    if(bun->dic_no >= 0){
	update_dic_list(bun_data_);
	dic_nickname(bun->dic_no, buf + strlen(buf));
	sStrcpy(tmp, jl_hinsi_name(bun_data_,bun->hinsi));
	sprintf(buf + strlen(buf), ":%d %s",
		bun->entry,tmp);
	if(bun->hindo != 0x7f){
	    sprintf(buf + strlen(buf), MSG_GET(16),
		    bun->ima ? '*' : ' ', bun->hindo);
	}else{
	    sprintf(buf + strlen(buf), " ---------");
	}			/* Means This entry is not used.
				   This must not appear in ordinary use!*/

    }else{
	sStrcpy(tmp, jl_hinsi_name(bun_data_,bun->hinsi));
	sprintf(buf + strlen(buf),"GIJI BUNSETU:%s", tmp);
    }
    strcat(buf,")   ");
    sprintf(buf + strlen(buf), MSG_GET(17),
	    bun->hyoka,bun->kangovect);

    if(bun->dai_top){
	if(bun->daihyoka != -1){
	    sprintf(buf + strlen(buf),"(%s:%d) ", MSG_GET(18),bun->daihyoka);
	}else {
	    sprintf(buf + strlen(buf),"(%s:---) ", MSG_GET(18));
	}
    }

    return(0);
}

static void sakujo_kouho1();

int
sakujo_kouho()
{
  push_cursor();
  sakujo_kouho1(cur_bnst_);
  pop_cursor();
  t_print_l();
  return(0);
}

static void
sakujo_kouho1(bun_no)
int bun_no;
{
  w_char w_buf[512];
  char buf[512];
  int kanji_len = jl_kanji_len(bun_data_, bun_no, bun_no+1) - 
      jl_fuzoku_len(bun_data_, bun_no);

  jl_get_kanji(bun_data_, bun_no, bun_no + 1, w_buf);
  w_buf[kanji_len] = 0;
  w_sttost(w_buf, buf);
  strcat(buf, MSG_GET(19));
  /*
  strcat(buf,"  読み:");
  */
  jl_get_yomi(bun_data_, bun_no, bun_no + 1, w_buf);
  w_buf[jl_jiri_len(bun_data_, bun_no)] = 0;
  w_sttost(w_buf,buf + strlen(buf));
  set_escape_code(buf);
  strcat(buf," ");
  sprintf(buf + strlen(buf), MSG_GET(20));
  /*
  sprintf(buf + strlen(buf), "を削除しますか?(Y/N)");
  */
  if(yes_or_no(buf) == 1){
    if(jl_word_delete(bun_data_, bun_data_->bun[bun_no]->dic_no, 
		      bun_data_->bun[bun_no]->entry) == -1){
      errorkeyin();
      return;
    }
  }
}

int
inspectdel()
{
    struct wnn_jl_bun *bun = bun_data_->bun[Bun_no];
    int type;
    w_char buf1[1024];
    char buf[512];
    int kanji_len = jl_kanji_len(bun_data_, Bun_no, Bun_no+1) - 
	jl_fuzoku_len(bun_data_, Bun_no);

    type = dicinfo[find_dic_by_no(bun->dic_no)].type;
    if(type  != WNN_UD_DICT && type != WNN_REV_DICT
		&& type != BWNN_REV_DICT
		&& type != CWNN_REV_DICT
       ){
	print_msg_getc(MSG_GET(21));
	/*
	print_msg_getc("固定形式の辞書のエントリは削除出来ません。(如何?)");
	*/
	return(0);
    }
    if(dicinfo[find_dic_by_no(bun->dic_no)].rw == WNN_DIC_RDONLY){
	print_msg_getc(MSG_GET(22));
	/*
	print_msg_getc("リードオンリーの辞書のエントリは削除出来ません。(如何?)");
	*/
	return(0);
    }
	
    jl_get_kanji(bun_data_, Bun_no, Bun_no + 1, buf1);
    buf1[kanji_len] = 0;

    strcpy(buf, MSG_GET(23));
    /*
    strcpy(buf, "「");
    */
    w_sttost(buf1, buf + strlen(buf));
    sprintf(buf + strlen(buf), MSG_GET(24));
    /*
    sprintf(buf + strlen(buf), "」を削除します。(Y/N)");
    */
    set_escape_code(buf);
    if(yes_or_no(buf) == 1){
	if(jl_word_delete(bun_data_, bun->dic_no, 
			  bun->entry) == -1){
	    errorkeyin();
	    return(0);
	}
	return(1);
    }
    return(0);
}

int
inspectuse()
{
    struct wnn_jl_bun *bun = bun_data_->bun[Bun_no];
    w_char buf1[1024];
    char buf[512];
    int kanji_len = jl_kanji_len(bun_data_, Bun_no, Bun_no+1) - 
	jl_fuzoku_len(bun_data_, Bun_no);

    if(dicinfo[find_dic_by_no(bun->dic_no)].hindo_rw == WNN_DIC_RDONLY){
	print_msg_getc(MSG_GET(25));
	/*
	print_msg_getc("リードオンリーの頻度は変更出来ません。(如何?)");
	*/
	return(0);
    }
    strcpy(buf, MSG_GET(23));
    /*
    strcpy(buf, "「");
    */
    jl_get_kanji(bun_data_, Bun_no, Bun_no + 1, buf1);
    buf1[kanji_len] = 0;
    w_sttost(buf1, buf + strlen(buf));
    set_escape_code(buf);
    sprintf(buf + strlen(buf), MSG_GET(26));
    /*
    sprintf(buf + strlen(buf), "」の使用を中止します。(Y/N)");
    */
    if(yes_or_no(buf) == 1){
	if(jl_word_use(bun_data_, bun->dic_no, bun->entry) == -1){
	    errorkeyin();
	    return(0);
	}
	return(1);
    }
    return(0);
}

int
next_inspect()
{
    next_flag = I_NEXT;
    return(1);
}

int
previous_inspect()
{
    next_flag = I_PREV;
    return(1);
}
