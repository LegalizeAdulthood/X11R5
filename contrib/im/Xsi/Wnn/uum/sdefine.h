/*
 * $Id: sdefine.h,v 1.3 1991/09/19 04:31:24 proj Exp $
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
/*************************
 * define of standard i/o
 *************************/

#define MAXCHG 80 /* 解析可能文字数 */
	/*漢字バッファ(次候補、単語検索など)の大きさ */
#define MAX_ICHIRAN_KOSU 36 /* 一覧表示字の表示個数 */
#define MAXWORDS	MAXJIKOUHO  /* 単語検索での検索語数 */
#define NBUN	2 /* n文節解析 */
#define JISHO_PRIO_DEFAULT 5 /*辞書プライオリティのデフォルト値 */

/* 評価関数の係数 */
#define	HINDOVAL 3 
#define	LENGTHVAL 100
#define	JIRITUGOVAL 1
#define FLAGVAL 200 /*今使ったよビットの係数*/
#define JISHOPRIOVAL 5

#define TBL_CNT 11		/* key_table no kazu */
/* Two tables are add. one is for selecting zenkouho's, and the other is jisho-ichiran. */
/* One more table is add, which is used in inspect */
#define TBL_SIZE 256

#define ESCAPE_CHAR(c)	(((c) < ' ') || ((c) == 127))
#define NORMAL_CHAR(c)	(((c) >= ' ') && ((c) < 127))
#define NOT_NORMAL_CHAR(c)	(((c) >= 128) && ((c) < 256))
#define ONEBYTE_CHAR(c)		(!((c) & ~0xff)) /* added by Nide */
/*
#define NISEMONO(c)		((c) & 0x80000000)
#define HONMONO(c)		(!NISEMONO(c))
#define KANJI_CHAR(c)		((c) >= 256 )
*/
#define KANJI_CHAR(c)		(((c) >= 256 ) && ((c) & 0xff00) != 0x8e00)

#define zenkaku(x)(KANJI_CHAR(x) || ESCAPE_CHAR(x))
#define hankaku(x) (!zenkaku(x))
#define ZENKAKU_HYOUJI(x)     zenkaku(x)
        /* CHANGE AFTERWARD TO BE ABLE TO TREAT hakaku katakana. */

#ifndef min
#define max(a , b) (((a) > (b))?(a) : (b))
#define min(a , b) (((a) <= (b))?(a) : (b))
#endif

#define numeric(x)	(((x >= S_NUM)&&(x <= E_NUM))? True : False)
#define ISKUTENCODE(x)	((x) == 0xa1a3)

struct jisho_ { /* 辞書管理用structure */
    char name[1024];
    char hname[1024]; /* 頻度ファイル名 */
    int dict_no; /* server が返してくる辞書ナンバー */
    int prio;
    int rdonly;
};

struct kansuu{			/* kansuu_hyo no entry */
    char *kansuumei;
    char *comment;
    int romkan_flag;			/* Clear Romkan or Not.  */
    int  (*func[TBL_CNT])();
};

#ifndef w_char
#define	w_char	unsigned short
#endif

#define printf        PRINTF
#define fprintf       FPRINTF
#define remove        REMOVE

#define throw_c(col)  throw_col((col) + disp_mode_length)

#define MAX_HISTORY 10


#define flush() fflush(stdout)
#define print_msg(X) {push_cursor();throw_c(0); clr_line();printf(X);flush();pop_cursor();}
#define print_msg_getc(X) {push_cursor();throw_c(0); clr_line();printf(X);flush();keyin();pop_cursor();}


#define UNDER_LINE_MODE (0x02 | 0x08 | 0x20)

#define OPT_CONVKEY		0x01
#define OPT_RKFILE		0x02
#define OPT_WNNKEY		0x04
#define OPT_FLOW_CTRL		0x08
#define OPT_WAKING_UP_MODE	0x10
#define OPT_VERBOSE		0x20
#define OPT_LANGDIR		0x40

#define convkey_defined_by_option	(defined_by_option & OPT_CONVKEY)
#define rkfile_defined_by_option	(defined_by_option & OPT_RKFILE)
#define uumkey_defined_by_option	(defined_by_option & OPT_WNNKEY)
#define verbose_option			(defined_by_option & OPT_VERBOSE)

/*
#define char_len(X)((hankaku(X))? 1 : 2)
*/

/*
  GETOPT string & ALL OPTIONS string for configuration.
  see each config.h for detail.
  NOTE: WHEN YOU MODIFY THESE, YOU ALSO MODIFY do_opt[] ARRAY AND
  ALL config.h!!!!
 */

#define GETOPTSTR   "hHujsUJSPxXk:c:r:l:D:n:vL:"
#define ALL_OPTIONS "hHujsUJSPxXkcrlDnvL"

/* for message file */
#include "msg.h"
#define	MSG_GET(no)	msg_get(cd, no, NULL, NULL)

#define	CWNN_PINYIN	       	0
#define	CWNN_ZHUYIN		1

#define print_out_func          (f_table->print_out_function)
#define input_func              (f_table->input_function)
#define call_t_redraw_move_func (f_table->call_t_redraw_move_function)
#define call_t_redraw_move_1_func (f_table->call_t_redraw_move_1_function)
#define call_t_redraw_move_2_func (f_table->call_t_redraw_move_2_function)
#define call_t_print_l_func     (f_table->call_t_print_l_function)
#define redraw_when_chmsig_func (f_table->redraw_when_chmsig_function)
#define char_len_func           (f_table->char_len_function)
#define char_q_len_func         (f_table->char_q_len_function)
#define t_redraw_move_func      (f_table->t_redraw_move_function)
#define t_print_l_func          (f_table->t_print_l_function)
#define c_top_func              (f_table->c_top_function)
#define c_end_func              (f_table->c_end_function)
#define c_end_nobi_func         (f_table->c_end_nobi_function)
#define call_redraw_line_func	(f_table->call_redraw_line_function)
#define hani_settei_func	(f_table->hani_settei_function)

#define env_normal               (cur_normal_env->env)
#define env_reverse              (cur_reverse_env->env)

#define envrcname               (cur_normal_env->envrc_name)
#define reverse_envrcname       (cur_reverse_env->envrc_name)

#define env_name_s               (cur_normal_env->env_name_str)
#define reverse_env_name_s       (cur_reverse_env->env_name_str)

#define servername              (cur_normal_env->host_name)
#define reverse_servername      (cur_reverse_env->host_name)

#define normal_sticky           (cur_normal_env->sticky)
#define reverse_sticky          (cur_reverse_env->sticky)
