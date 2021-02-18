/*
 * $Id: uif.c,v 1.2 1991/09/16 21:34:05 ohm Exp $
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
 このファイルは、標準ユーザーインターフェースの関数を定義します。
buffer_inを用いているので、そちらの方も参照して下さい。
それぞれの関数の動作は、key_bind.cの中でどこにバインドされているか
みる事により、分かると思います。
モードが存在します(henkan_mode)が、あまり利用しないように務めています。
*/

#include <stdio.h>
#include "commonhd.h"
#include "config.h"
#include "jllib.h"
#ifdef SYSVR2
#include <string.h>
#endif
#ifdef BSD42
#include <strings.h>
#endif
#include "sdefine.h"
#include "sheader.h"
#include "rk_spclval.h" /* defines of CHMSIG, NISEBP */
#include "buffer.h"
#include "rk_fundecl.h"

static int henkan_mode;
/*  0: select
    1: yomi_nyuuryoku
    2: bunsetu_nobasi
    3: kara
*/

int
redraw_nisemono_c()
{
    redraw_nisemono();
    if(c_b->maxlen == 0){
	change_to_empty_mode();
    }
    return(0);
}

/** 標準インターフェースのトップ関数です。
この関数は、mojiretsuの中に変換結果を入れて、文字列の長さを返します。
*/
int
kk()
{
  static struct buf c_b1;
  int len;

  initialize_vars();
  if(bun_data_) jl_kill(bun_data_, 0, -1);
  c_b = &c_b1;
  c_b->buffer = input_buffer;
  c_b->buf_msg = "";
  c_b->start_col = 0;
  c_b->t_c_p = 0;
  c_b->t_b_st = 0;
  c_b->t_b_end = 0;
  c_b->maxlen = 0;
  c_b->buflen = maxchg;
  c_b->redraw_fun = redraw_nisemono_c;
  c_b->t_m_start = 0;
  set_screen_vars_default();
  change_to_empty_mode();
  init_screen();
  if(henkan_off_flag){
    henkan_off();
  }
/* end of addition */
  buffer_in();
  if (print_out_func) {
      len = (*print_out_func)(return_buf , c_b->buffer , c_b->maxlen);
  } else {
      Strncpy(return_buf , c_b->buffer , c_b->maxlen);
      len = c_b->maxlen;
  }
  return(len);
}

/** ここからの関数は、 c_b->key_in_fun, c_b->ctrl_code_fun に
モードに応じて設定するものです。設定の仕方は、この下のモード変更の関数群を
みて下さい。
*/

int
return_it(c)
int c;
{
  c_b->maxlen = 1;
  c_b->buffer[0] = c;
  return(1);
}

int
return_it_if_ascii(c , nisemono)
int c;
int nisemono;  /* it means that what romkan returned is a nisemono character. */
{
    if((NORMAL_CHAR(c) || c == 0x09) && is_HON(nisemono)){ /* TAB is not ESCAPE char */
	c_b->maxlen = 1;
	c_b->buffer[0] = c;
	return(1);
    }else{
	insert_char_and_change_to_insert_mode(c);
	return(0);
    }
}

int
push_char_return(c,romkan)
int c,romkan;
{
  push_unget_buf(romkan);
  kakutei(c);
  return(1);
}

int
ignore_it_and_clear_romkan()
{
  romkan_clear();
  return(0);
}


int
insert_char_and_change_to_insert_mode(c)
int c;
{
    extern send_ascii_char_quote;
    insert_char(c);
    t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,0);
    change_to_insert_mode();
    if (send_ascii_char_quote == 1 && send_ascii_char == 0){
	send_ascii_char = 1;
        send_ascii_char_quote = 0;
    }
    return(0);
}

/** ここからの関数は、モード変更の時に呼ばれるものです。
   設定するのは、key_table,key_in_fun,ctrl_code_fun,hantenです。	
*/

void
change_to_insert_mode()
{
  c_b->key_table = main_table[1];
  c_b->rk_clear_tbl =  romkan_clear_tbl[1];
  c_b->key_in_fun = 0;
  c_b->ctrl_code_fun = (int (*)())0;
  c_b->hanten = 0x08 | 0x20;
  henkan_mode = 1;
  kk_cursor_normal();
}

void
change_to_nobasi_tijimi_mode()
{
  c_b->key_table = main_table[2];
  c_b->rk_clear_tbl =  romkan_clear_tbl[2];
  c_b->key_in_fun = ignore_it_and_clear_romkan;
  c_b->ctrl_code_fun = (int (*)())0;
  c_b->hanten = 0x04;
  henkan_mode = 2;
  kk_cursor_invisible();
}


void
change_to_empty_mode()
{
  c_b->key_table = main_table[3];
  c_b->rk_clear_tbl =  romkan_clear_tbl[3];
  if(send_ascii_char){
      c_b->key_in_fun = return_it_if_ascii;
  }else{
      c_b->key_in_fun = insert_char_and_change_to_insert_mode;
  }
  c_b->ctrl_code_fun = return_it;
  c_b->hanten =  0x08 | 0x20;
  henkan_mode = 3;
  kk_restore_cursor();
  kk_cursor_normal();
  flush();
}

void
change_to_henkango_mode()
{
  c_b->key_table = main_table[0];
  c_b->rk_clear_tbl =  romkan_clear_tbl[0];
  c_b->key_in_fun = push_char_return;
  c_b->ctrl_code_fun = (int (*)())0;
  c_b->hanten = 0x04 | 0x40;
  henkan_mode = 0;
  kk_cursor_invisible();
}


/** 変換後の文字列のbnst番目の文節から先を、c_b->bufferに入れます。
同時に、bunsetsuも設定します。
*/
void
make_kanji_buffer(bnst)
int bnst;
{
  int k , l;
  w_char *bp;
  w_char	*buffer_end; /** 仮名入力用のバッファーの最後 */

  buffer_end = c_b->buffer + c_b->buflen - 1;
  if(bnst) bp = c_b->buffer + bunsetsuend[bnst - 1];
  else bp = c_b->buffer;
  for(k = bnst ; k < jl_bun_suu(bun_data_) ; k++){
      if (k < maxbunsetsu) bunsetsu[k] = bp - c_b->buffer;
      l = jl_get_kanji(bun_data_, k, k+1, bp);
      bp += l;
      if (k < maxbunsetsu) bunsetsuend[k] = bp - c_b->buffer;
      if (bp > buffer_end) {
	  c_b->maxlen = bp - c_b->buffer;
      }
  }
  if(k < maxbunsetsu){
    bunsetsu[k++] = bp - c_b->buffer;
    bunsetsuend[k++] = bp - c_b->buffer;
  }else{
    bunsetsu[maxbunsetsu - 1] = bp - c_b->buffer;
    bunsetsuend[maxbunsetsu - 1] = bp - c_b->buffer;
  }
  c_b->maxlen = bp - c_b->buffer;
}

/** 連文節変換 */
int
ren_henkan()
{
    int ret;

    jl_env_set(bun_data_, env_normal);
    ret = ren_henkan0();
    return (ret);
}

/** 連文節漢字かな変換 */
int
kankana_ren_henkan()
{
    int ret;

    if (*reverse_envrcname != '\0') {
	jl_env_set(bun_data_, env_reverse);
	ret = ren_henkan0();
	return (ret);
    } else {
	ring_bell();
	return (0);
    }
}


int
isconect_jserver()
{
    if (!jl_isconnect(bun_data_)) {
	connect_jserver(1);
    }
    if (!jl_isconnect(bun_data_)) {
	push_cursor();
	print_msg_getc(MSG_GET(9));
/*
	print_msg_getc(" jserverとの接続が切れています。(如何?)");
*/
	pop_cursor();
	t_print_l();
	return (0);
    }
    return(1);
}


int
ren_henkan0()
{
  w_char yomi[512];
  int moji_suu;
  register int i;

  if (!isconect_jserver()) {
    return(0);
  }

  c_b->buflen = maxchg;
  moji_suu =  c_b->maxlen - c_b->t_m_start;
  if(moji_suu > 0){
    int ret;
      Strncpy(yomi, c_b->buffer + c_b->t_m_start, moji_suu);
      yomi[moji_suu] = 0;
      ret = jl_ren_conv(bun_data_, yomi, cur_bnst_, -1, WNN_USE_MAE );
      if(ret == -1){
	  errorkeyin();
	  t_print_l();
	  return(0);
      }
      for (i = cur_bnst_; i < jl_bun_suu(bun_data_); i++)
	  bunsetsu_env[i] = jl_env_get(bun_data_);
      make_kanji_buffer(0);
  change_to_henkango_mode();
  c_b->t_m_start = bunsetsuend[cur_bnst_ ];
  c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
  c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
  t_redraw_move(bunsetsu[cur_bnst_ ] 
		,bunsetsu[dai_top(bun_data_, cur_bnst_)],c_b->maxlen,1);
  }
  return(0);
}

int
tan_henkan()
{
    int tmp;

    jl_env_set(bun_data_, env_normal);
    tmp = tan_conv(WNN_SHO);
    return (tmp);
}

int
tan_henkan_dai()
{
    int tmp;

    jl_env_set(bun_data_, env_normal);
    tmp = tan_conv(WNN_DAI);
    return (tmp);
}

int
tan_conv(daip)
int daip;
{
    w_char yomi[512];
    int moji_suu;
    register int i;

    if (!isconect_jserver()) {
	return(0);
    }

    c_b->buflen = maxchg;
    moji_suu =  c_b->maxlen - c_b->t_m_start;
    if(moji_suu > 0){
	int ret;
	Strncpy(yomi, c_b->buffer + c_b->t_m_start, moji_suu);
	yomi[moji_suu] = 0;
	ret = jl_tan_conv(bun_data_, yomi, cur_bnst_, -1, WNN_USE_MAE, daip);
	if(ret == -1){
/*
#ifdef CONVERT_by_STROKE

	    errorkeyin_q();
	    errorkaijo();
	    errorkill_c();
#else
*/
	    errorkeyin();
/*
#endif
*/
	    t_print_l();
	    return(0);
	}
    	for (i = cur_bnst_; i < dai_end(bun_data_, cur_bnst_); i++)
	    bunsetsu_env[i] = jl_env_get(bun_data_);
	make_kanji_buffer(0);
    change_to_henkango_mode();
    c_b->t_m_start = bunsetsuend[cur_bnst_ ];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    t_redraw_move(bunsetsu[cur_bnst_ ] 
		  ,bunsetsu[dai_top(bun_data_, cur_bnst_)],c_b->maxlen,1);
    }
    return(0);
}


/** 単文節変換*/
int
nobi_henkan()
{
    int tmp;

    tmp = nobi_conv(WNN_SHO, NULL);
    return (tmp);
}

int
nobi_henkan_dai()
{
    int tmp;

    tmp = nobi_conv(WNN_DAI, NULL);
    return (tmp);
}


int
nobi_conv(daip, env)
struct wnn_env *env;
int daip;
{
    int tmp;
    if(!isconect_jserver()) {
	return(0);
    }
    tmp = c_b->t_b_st;
    if(tan_henkan1(daip, env) == 0){
	c_b->t_m_start = bunsetsuend[cur_bnst_ ];
	t_redraw_move(bunsetsu[cur_bnst_ ] ,min(tmp, c_b->t_b_st), c_b->maxlen,1);
    }
    return(0);
}

/*単文節変換するが、画面に表示しない*/
int
tan_henkan1(daip, env)
struct wnn_env *env;
int daip;
{
  int ret;
  register int i;
  int moji_suu = c_b->t_c_p - c_b->t_m_start;

  if(!isconect_jserver()) {
    return(-1);
  }

  if(moji_suu == 0){
    return(-1);
  }
  bunsetsu_env[cur_bnst_] = jl_env_get(bun_data_);
  if(env == NULL) {
      ret = jl_nobi_conv(bun_data_, cur_bnst_, moji_suu, -1, WNN_USE_MAE, daip);
  } else {
      ret = jl_nobi_conv_e2(bun_data_, env, cur_bnst_, moji_suu, -1, WNN_USE_MAE, daip);
  }

  if (ret == -1) {
    errorkeyin();
    t_print_l();
    return(-1);
  }
  make_kanji_buffer(0);

  for (i = cur_bnst_+1; i < jl_bun_suu(bun_data_); i++)
    bunsetsu_env[i] = jl_env_get(bun_data_);
  /**/
  change_to_henkango_mode();
  c_b->t_c_p = bunsetsu[cur_bnst_ ];
  c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
  c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
  return(0);
}


/* c_b->bufferのマークの位置から、kana_bufのcur_bnst_以降に
文字列をコピーする */
/* make_yomi() : DELETED */

/* 区点入力で変換する */
void
henkan_if_maru(c)
w_char c;
{
	if (ISKUTENCODE(c) && c_b->key_table == main_table[1])
		ren_henkan();
}


/** Convert by stroke : BWNN */
#ifdef nodef
#ifdef CONVERT_by_STROKE
int
question_henkan()
{
    int tmp;

    jl_env_set(bun_data_, env_normal);
    tmp = q_conv(WNN_SHO);
    return (tmp);
}

/** return Question Mark char */
w_char
romkan_state()
{
     char *dspmod;
     dspmod = romkan_dispmode();
     return(strlen(dspmod) > WNN_DISP_MODE_LEN ?
		 *(dspmod + WNN_DISP_MODE_LEN) : NULL);
}

int
q_conv(daip)
int daip;
{
    w_char yomi[512];
    int moji_suu;
    register int i;

    if (!isconect_jserver()) {
	return(0);
    }

    c_b->buflen = maxchg;
    moji_suu =  c_b->maxlen - c_b->t_m_start;
    if(moji_suu > 0){
	int ret;
	w_char rk_state = (w_char)romkan_state();
	Strncpy(yomi, c_b->buffer + c_b->t_m_start, moji_suu);
	if(rk_state)
	    for(i = 0; i < moji_suu; i++)
		if(yomi[i] == rk_state) yomi[i] = Q_MARK;
	yomi[moji_suu] = 0;
	ret = jl_q_conv(bun_data_, yomi, cur_bnst_, -1, WNN_USE_MAE, daip);
	if(ret == -1){
	    errorkeyin_q();
	    errorkaijo();
	    errorkill_c();
	    t_print_l();
	    return(0);
        }
    	for (i = cur_bnst_; i < dai_end(bun_data_, cur_bnst_); i++)
	    bunsetsu_env[i] = jl_env_get(bun_data_);
        make_kanji_buffer(0);
    change_to_henkango_mode();
    c_b->t_m_start = bunsetsuend[cur_bnst_ ];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    t_redraw_move(bunsetsu[cur_bnst_ ] 
		  ,bunsetsu[dai_top(bun_data_, cur_bnst_)],c_b->maxlen,1);
    }
    return(0);
}
#endif
#endif

/* 確定 */
int kakutei()
{
    int moji_suu = 0;
    w_char *w = (w_char *)0;
    w_char yomi[512];
     /* conversion nashi de kakutei surubaai wo kouryo seyo. */

   /* 確定時に読みを覚えておく  remember_me() のため */
    remember_buf[0] = 0;
    if (jl_bun_suu(bun_data_) != 0) {
	jl_get_yomi(bun_data_, 0, -1, remember_buf);
    }
    /* 解除した文字があれば覚えておく */
    if(insert_modep()){
	moji_suu =  c_b->maxlen - c_b->t_m_start;
	if(moji_suu > 0){
	    w = remember_buf + Strlen(remember_buf);
	    Strncat(w,c_b->buffer + c_b->t_m_start, moji_suu);
	}
    }else if(nobasi_tijimi_mode()){
    /* のばし、ちじみモードだったら、ひらがなに戻すために解除する */
    /* 解除 si te iino ?? */
	moji_suu = jl_get_yomi(bun_data_, cur_bnst_, -1, yomi);
	Strcpy(c_b->buffer + c_b->t_c_p ,yomi);
	c_b->maxlen = Strlen(c_b->buffer);
	jl_kill(bun_data_, cur_bnst_, -1);
    }
    c_b->buffer[0] = 0;
    if (jl_bun_suu(bun_data_) != 0) {
	jl_get_kanji(bun_data_, 0, -1, c_b->buffer);
	if(jl_update_hindo(bun_data_, 0, -1) == -1){
	    errorkeyin();
	    redraw_line();
	}
    } 
    if (insert_modep() && moji_suu > 0) { /* 解除した文字がある */
	Strcat(c_b->buffer,w);
    }else if(nobasi_tijimi_mode() && moji_suu > 0){ /* のばし、ちじみモード */
	Strcat(c_b->buffer,yomi);
    }
    c_b->maxlen = Strlen(c_b->buffer);

    throw_c(0);
    clr_line_all();
    flush();
    return(1);
}

/*yank*/
int yank_c()
{

  if(empty_modep()){
	change_to_insert_mode();
  }
  t_yank();
  return(0);
}

/** 読みの再入力*/
int remember_me()
{
    if(jl_bun_suu(bun_data_) == 0){
	if(c_b->key_in_fun){
	    (*c_b->key_in_fun)(-1);
	    c_b->key_in_fun = NULL;
	}
	Strcpy(c_b->buffer ,remember_buf);
	c_b->maxlen = Strlen(remember_buf);
	call_t_redraw_move(0, 0, c_b->maxlen,1,1);
    }
    return(0);
}


void
initialize_vars()
{
  cur_bnst_ = 0;
}

void kill_c()
{
    t_kill();
    if(c_b->maxlen == 0){
	change_to_empty_mode();
    }
}

#ifdef nodef
#ifdef CONVERT_by_STROKE
void errorkill_c()
{
    errort_kill();
    if(c_b->maxlen == 0){
	change_to_empty_mode();
    }
}
#endif
#endif

void delete_c(c,romkan)
int c,romkan;
{
    t_delete_char(c , romkan);
    if((c_b->maxlen == 0) && is_HON(romkan)){
	change_to_empty_mode();
    }
}

void rubout_c(c , romkan)
w_char c;
int romkan;
{
  t_rubout(c, romkan);
  if((c_b->maxlen == 0) && is_HON(romkan)){
      change_to_empty_mode();
  }
}  

void
end_bunsetsu()
{
  int tmp = cur_bnst_;
  cur_bnst_ = jl_bun_suu(bun_data_) - 1;
  c_b->t_m_start = bunsetsuend[cur_bnst_ ];
  c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
  c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
  t_redraw_move(bunsetsu[cur_bnst_],
		bunsetsu[dai_top(bun_data_, tmp)],
		bunsetsuend[cur_bnst_],0);
}

void
top_bunsetsu()
{
  int tmp = cur_bnst_;
  cur_bnst_ = 0;
  c_b->t_m_start = bunsetsuend[0];
  c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
  c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
  t_redraw_move(bunsetsu[0],0,bunsetsuend[dai_end(bun_data_, tmp) -1],0);
}

int
forward_bunsetsu()
{
  if(cur_bnst_ < jl_bun_suu(bun_data_) - 1){
    cur_bnst_ += 1;
    c_b->t_m_start = bunsetsuend[cur_bnst_ ];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    t_redraw_move(bunsetsu[cur_bnst_ ] 
		  ,bunsetsu[dai_top(bun_data_, cur_bnst_ - 1)]
		  ,bunsetsuend[dai_end(bun_data_, cur_bnst_) -1],0);
  }
  return(0);
}

int
backward_bunsetsu()
{
  if(cur_bnst_ > 0){
    cur_bnst_ -= 1;
    c_b->t_m_start = bunsetsuend[cur_bnst_];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    t_redraw_move(bunsetsu[cur_bnst_ ] 
		  ,bunsetsu[dai_top(bun_data_, cur_bnst_)],
		  bunsetsuend[dai_end(bun_data_, cur_bnst_ + 1) - 1],0);
  }
  return(0);
}

int kaijo()
{
  w_char yomi[512];

  c_b->t_b_end = c_b->t_b_st;
  t_redraw_move(bunsetsu[cur_bnst_],
		bunsetsu[dai_top(bun_data_, cur_bnst_)],
		bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1],0);
  change_to_insert_mode();
  c_b->t_c_p = bunsetsu[cur_bnst_];
  c_b->t_m_start = c_b->t_c_p;
  if (jl_env(bun_data_) == env_normal)
      jl_get_yomi(bun_data_, cur_bnst_, -1, yomi);
  else if (jl_env(bun_data_) == env_reverse)
      jl_get_yomi(bun_data_, cur_bnst_, -1, yomi); /* kankana_ren_henkan() */
  Strcpy(c_b->buffer + c_b->t_c_p ,yomi);

  c_b->buflen = maxchg;
  c_b->maxlen = Strlen(c_b->buffer);
  jl_kill(bun_data_, cur_bnst_, -1);
  call_t_redraw_move(c_b->t_c_p, c_b->t_c_p, c_b->maxlen, 1, 1);
  return(0);
}

#ifdef nodef
#ifdef CONVERT_by_STROKE
int errorkaijo()
{
  w_char yomi[512];

  c_b->t_b_end = c_b->t_b_st;
  change_to_insert_mode();
  c_b->t_c_p = bunsetsu[cur_bnst_];
  c_b->t_m_start = c_b->t_c_p;
  if (jl_env(bun_data_) == env_normal)
      jl_get_yomi(bun_data_, cur_bnst_, -1, yomi);
  else if (jl_env(bun_data_) == env_reverse)
      jl_get_yomi(bun_data_, cur_bnst_, -1, yomi); /* kankana_ren_henkan() */
  Strcpy(c_b->buffer + c_b->t_c_p ,yomi);

  c_b->buflen = maxchg;
  c_b->maxlen = Strlen(c_b->buffer);
  jl_kill(bun_data_, cur_bnst_, -1);
  return(0);
}
#endif
#endif

int
jutil_c()
{

    if(!isconect_jserver()){
	return (0);
    }
    push_cursor();
    jutil();
    pop_cursor();
    t_print_l();
    if (empty_modep()) {
	kk_restore_cursor();
    }
    return(0);
}

int
touroku_c()
{
    if(!isconect_jserver()){
	return(0);
    }
    push_cursor();
    touroku();
    pop_cursor();
    t_print_l();
    return(0);
}

int
enlarge_bunsetsu()
{
    w_char yomi[512];
    int area_len, len1;

    area_len = jl_yomi_len(bun_data_, cur_bnst_, dai_end(bun_data_, cur_bnst_));
    len1 = jl_get_yomi(bun_data_, cur_bnst_, -1, yomi);

    if(area_len < len1)
	area_len ++;

  change_to_nobasi_tijimi_mode();
  c_b->t_m_start = c_b->t_c_p;
  c_b->t_b_end = c_b->t_c_p;
  Strcpy(c_b->buffer + c_b->t_m_start ,yomi);
  c_b->maxlen = Strlen(c_b->buffer);
  t_redraw_move(c_b->t_m_start + area_len, c_b->t_m_start, c_b->maxlen,1);
  return(0);
}

int
smallen_bunsetsu()
{
    w_char yomi[512];
    int area_len;

    area_len = jl_yomi_len(bun_data_, cur_bnst_, dai_end(bun_data_, cur_bnst_));
    jl_get_yomi(bun_data_, cur_bnst_, -1, yomi);

    if (area_len > 1) area_len--;

  change_to_nobasi_tijimi_mode();
  c_b->t_m_start = c_b->t_c_p;
  c_b->t_b_end = c_b->t_c_p;
  Strcpy(c_b->buffer + c_b->t_m_start ,yomi);
  c_b->maxlen = Strlen(c_b->buffer);
  t_redraw_move(c_b->t_m_start + area_len, c_b->t_m_start , c_b->maxlen,1);
  return(0);
}


int
send_string(c)
int c;
{
  kakutei();
  c_b->buffer[c_b->maxlen] = c;
  c_b->maxlen += 1;
  c_b->t_c_p = c_b->maxlen;
  return(1);
}

int
tijime()
{
    if(c_b->t_c_p > c_b->t_m_start + 1){
	backward();
    }
    return(0);
}

int
jmptijime()
{
    call_t_redraw_move_1(c_b->t_m_start, c_b->t_m_start, c_b->maxlen, 1, 1, 1);
    backward();
    return(0);
}


int
henkan_forward()
{
    if(!isconect_jserver()) {
	return(0);
    }

    if(tan_henkan1(WNN_DAI, NULL) == -1){
	return(0);
    }
    zenkouho_dai_c();
    forward_bunsetsu();
    return(0);
}

int
henkan_backward()
{
    if(!isconect_jserver()) {
	return(0);
    }

    if (cur_bnst_ <= 0) {	/* 左端なら変換のみ */
	nobi_henkan_dai();
    } else {
    	if(tan_henkan1(WNN_DAI, NULL) == -1){
		return(0);
    	}
	zenkouho_dai_c();
    	backward_bunsetsu();
    }
    return(0);
}

int
backward_c()
{
    if(!isconect_jserver()) {
	return(0);
    }
    if(c_b->t_c_p == c_b->t_m_start){
	if(c_b->t_c_p != 0) {
	    ren_henkan0();
	    change_to_henkango_mode();
	    backward_bunsetsu();
	}
    }else{
	backward();
    }
    return(0);
}


void
insert_it_as_yomi()
{
    kakutei();
    change_to_insert_mode();
    c_b->t_m_start = 0;
    cur_bnst_ = 0;
    call_t_print_l();
    if(bun_data_)
	jl_kill(bun_data_, 0, -1);
}

/****history *******/
int
previous_history()
{
    int k;
  if (jl_bun_suu(bun_data_) == 0) {
    k = previous_history1(c_b->buffer);
    if(k > 0){
	change_to_insert_mode();
	c_b->vst = 0;
	c_b->maxlen = k;
	c_b->t_c_p = 0;
	call_t_print_l();
    }
  }
    return(0);
}

int
next_history()
{
    int k;
  if (jl_bun_suu(bun_data_) == 0) {
    k = next_history1(c_b->buffer);
    if(k > 0){
	change_to_insert_mode();
	c_b->vst = 0;
	c_b->maxlen = k;
	c_b->t_c_p = 0;
	call_t_print_l();
    }
  }
    return(0);
}

int
empty_modep()
{
    return(henkan_mode == 3);
}

int
nobasi_tijimi_mode()
{
    return(henkan_mode == 2);
}

int
insert_modep()
{
    return(henkan_mode == 1);
}

int
henkan_gop()
{
    return(henkan_mode == 0 || henkan_mode == 2);
}


static int send_ascii_stack = 0;

int
send_ascii()
{
  send_ascii_stack = send_ascii_char;
  send_ascii_char = 1;
  return(0);
}

int
not_send_ascii()
{
  send_ascii_stack = send_ascii_char;
  send_ascii_char = 0;
  return(0);
}

int
toggle_send_ascii()
{
  send_ascii_stack = send_ascii_char;
  if(send_ascii_char == 0){
    send_ascii_char = 1;
  }else{
    send_ascii_char = 0;
  }
  return(0);
}

int
pop_send_ascii()
{
  send_ascii_char = send_ascii_stack;
  return(0);
}

int send_ascii_char_quote = 0;

void
quote_send_ascii()
{
  if(send_ascii_char == 1){
    send_ascii_char = 0;
    send_ascii_char_quote = 1;
  }
}

void
check_empty_mode_keyin_fun()
{
  if(send_ascii_char){
    c_b->key_in_fun = return_it_if_ascii;
  }else{
    c_b->key_in_fun = insert_char_and_change_to_insert_mode;
  }
}

int
send_ascii_e()
{
  send_ascii();
  check_empty_mode_keyin_fun();
  return(0);
}

int
not_send_ascii_e()
{
  not_send_ascii();
  check_empty_mode_keyin_fun();
  return(0);
}

int
toggle_send_ascii_e()
{
  toggle_send_ascii();
  check_empty_mode_keyin_fun();
  return(0);
}

int
pop_send_ascii_e()
{
  pop_send_ascii();
  check_empty_mode_keyin_fun();
  return(0);
}

int
quote_send_ascii_e()
{
  quote_send_ascii();
  check_empty_mode_keyin_fun();
  return(0);
}

/** jserverとのコネクションを再確立する。*/
int
reconnect_jserver()
{
    if(!jl_isconnect_e(env_normal) ||
       env_reverse != 0 && !jl_isconnect_e(env_reverse)) {
	push_cursor();
	reconnect_jserver_body();
	pop_cursor();
	t_print_l();
    }
    return(0);
}

int
reconnect_jserver_body()
{
  /*
  char *message = " 接続ホスト名 > ";
  */
  w_char *kana_buf[1024];
  char hostname[128];

  chartowchar(servername,kana_buf);
  if (kana_in(MSG_GET(11),
/*
  if (kana_in(" 接続ホスト名 > ",
*/
	UNDER_LINE_MODE, kana_buf, 1024) == -1) return(0);
  wchartochar(kana_buf, hostname);
  if(strlen(hostname) == 0)return(0);
  strcpy(servername, hostname);

  print_msg(MSG_GET(10));
  /*
  print_msg(" 接続中・・・");
  */
  if(connect_jserver(1) < 0){
    print_msg_getc(MSG_GET(12));
    /*
    print_msg_getc(" 接続がうまくいきません。(如何?)");
    */
  } else if(!jl_isconnect(bun_data_)) {
    print_msg_getc(MSG_GET(13));
    /*
    print_msg_getc(" 接続に失敗しました。(如何?)");
    */
  }
  return(0);
}


/** jserver との connection を切る。*/
int
disconnect_jserver()
{
    WnnEnv *p;

    for (p = normal_env; p; p = p->next) {
	cur_normal_env = p;
	if (jl_isconnect_e(env_normal)) {
	    jl_dic_save_all_e(env_normal);
	    jl_disconnect(env_normal);
	    env_normal =0;
	}
    }
    for (p = reverse_env; p; p = p->next) {
	cur_reverse_env = p;
	if (jl_isconnect_e(env_reverse)) {
	    jl_dic_save_all_e(env_reverse);
	    jl_disconnect(env_reverse);
	    env_reverse =0;
	}
    }
    if (bun_data_) {
	jl_env_set(bun_data_, 0);
	jl_close(bun_data_);
    }
    return(1);
}

/* MOVED from jhlp.c */

/** 変換オフ時のループ */

int
henkan_off()
{
    unsigned char c;
    int i;
    extern int (*keyin_when_henkan_off)();
    extern int ptyfd;
    static char outbuf[16];
    extern int (*code_trans[])();
    extern int henkan_off_flag;

    push_cursor();
    display_henkan_off_mode();
    flush();

    henkan_off_flag = 1;
    
    for(;;) {
	if ((i = (*keyin_when_henkan_off)()) == -1) /* convert_key ha tooru. */
	    continue;
	c = i;
	if ((!quote_flag) && (c == quote_code)) {
	    quote_flag = 1;
	    continue;
	}
	if ((quote_flag == 0) && (c_b->key_table[c] == henkan_off)){
	    break;
	}
	/* コード変換 */
	if (pty_c_flag == tty_c_flag)
	    i=through(outbuf, &c, 1);
	else
	    i=(*code_trans[(J_EUJIS<<2)|pty_c_flag])(outbuf, &c, 1);
	if (i <= 0) continue;
	write(ptyfd, outbuf , i);
	quote_flag = 0;
    }
    henkan_off_flag = 0;
    pop_cursor();
    call_redraw_line(c_b->t_c_p, 1);
    flush();
    return(0);
}

char
env_state()
{
    char *p;
    char ret = '\0';
#ifndef SYSVR2
    extern char *index();
#else
    extern char *strchr();
#endif

    if ((p = romkan_dispmode()) == NULL) return(ret);
#ifndef SYSVR2
    if ((p = (char *)index(p, ':')) == NULL) return(ret);
#else
    if ((p = (char *)strchr(p, ':')) == NULL) return(ret);
#endif
    return(*(p + 1));
}

int
set_cur_env(s)
char s;
{
    register WnnEnv *p;
    register int i;

    for (p = normal_env; p; p = p->next) {
        for (i = 0; p->env_name_str[i]; i++) {
            if (s == p->env_name_str[i]) {
                cur_normal_env = p;
		jl_env_set(bun_data_, env_normal);
                break;
            }
        }
    }
    for (p = reverse_env; p; p = p->next) {
        for (i = 0; p->env_name_str[i]; i++) {
            if (s == p->env_name_str[i]) {
                cur_reverse_env = p;
                break;
            }
        }
    }
    return(0);
}

void
get_new_env(rev)
int rev;
{
    register WnnEnv *p;

    p = (WnnEnv *)malloc(sizeof(WnnEnv));
    p->host_name = NULL;
    p->env = NULL;
    p->sticky = 0;
    p->envrc_name = NULL;
    p->env_name_str[0] = '\0';
    if (rev) {
        p->next = reverse_env;
        reverse_env = cur_reverse_env = p;
    } else {
        p->next = normal_env;
        normal_env = cur_normal_env = p;
    }
}

#define UNGETBUFSIZE    32
static unsigned int unget_buf[UNGETBUFSIZE];
static int count = 0;

int
push_unget_buf(c)
int c;
{
    if ((count + 1) >= UNGETBUFSIZE) return(-1);
    unget_buf[count++] = c;
    unget_buf[count] = EOLTTR;
    return(0);
}

unsigned int *
get_unget_buf()
{
    if (count <= 0) unget_buf[0] = EOLTTR;
    count = 0;
    return(unget_buf);
}

int
if_unget_buf()
{
    if (count > 0) return(1);
    return(0);
}
