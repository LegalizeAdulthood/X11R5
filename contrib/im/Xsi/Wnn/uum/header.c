/*
 * $Id: header.c,v 1.2 1991/09/16 21:33:33 ohm Exp $
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
#include "config.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "buffer.h"

int not_redraw;
w_char *input_buffer;
w_char *return_buf;	/* return you mojiretsu buffer */
struct wnn_buf *bun_data_;

WnnEnv *normal_env = NULL;
WnnEnv *reverse_env = NULL;
WnnEnv *cur_normal_env = NULL;
WnnEnv *cur_reverse_env = NULL;

int 	cur_bnst_  = 0;	/* current bunsetsu pointer */

int crow;
int disp_mode_length = WNN_DISP_MODE_LEN;  /* モード表示に必要なcolumn数 */
char	Term_Name[128];

int rubout_code = 127;
int kk_on = 0;
int quote_code = -1;
int quote_flag = 0;

int max_history;

int (*main_table[TBL_CNT][TBL_SIZE])();
char romkan_clear_tbl[TBL_CNT][TBL_SIZE];
w_char jishopath[64];
w_char hindopath[64];
w_char fuzokugopath[64];

struct buf *c_b;
short pty_c_flag;
short tty_c_flag;

int cursor_invisible_fun = 0;
int send_ascii_char = 0;
int excellent_delete = 1;
int (*keyin_when_henkan_off)();

int *bunsetsu;
int *bunsetsuend;
struct wnn_env **bunsetsu_env;
int *touroku_bnst;
int touroku_bnst_cnt;
/* int bunsetsucnt = 0; */
w_char *kill_buffer;
w_char *remember_buf;
int kill_buffer_offset = 0;

int touroku_comment = 0;	/* Comment for Words in Word_add */

char lang_dir[32] = {'\0'};

char uumkey_name_in_uumrc[PATHNAMELEN] = {'\0'};
char convkey_name_in_uumrc[PATHNAMELEN] = {'\0'};
char rkfile_name_in_uumrc[PATHNAMELEN] = {'\0'};
short defined_by_option = 0;

int conv_lines = 1;
int flow_control = 0;
int henkan_off_flag = 0;	/* 立ち上げ時に変換をoffにした状態にします */
int henkan_on_kuten = 0; /* 句点入力で変換しない */

char def_servername[PATHNAMELEN] = { '\0' };  /* V3.0 */
char def_reverse_servername[PATHNAMELEN] = { '\0' };
char username[PATHNAMELEN] = { '\0' };  /* V3.0 */

int remove_cs_from_termcap = 1;

int maxbunsetsu;
int max_ichiran_kosu;
int maxchg;
int maxlength;

/*
w_char kugiri_str[MAXKUGIRI] = {' ',0};
*/

WNN_DIC_INFO *dicinfo;
int dic_list_size;

struct msg_cat *cd;

FunctionTable *f_table = NULL;

FuncDataBase function_db[] = {
    { "ja_JP",
	{ NULL,   NULL,   call_t_redraw_move_normal,
        call_t_redraw_move_1_normal, call_t_redraw_move_2_normal,
        call_t_print_l_normal, NULL, char_len_normal, char_q_len_normal,
        t_redraw_move_normal, t_print_l_normal, c_top_normal, c_end_normal,
        c_end_nobi_normal, call_redraw_line_normal, hani_settei_normal
      }
  },
    { "zh_CN",
	{ print_out_yincod, input_yincod, call_t_redraw_move_yincod,
        call_t_redraw_move_1_yincod, call_t_redraw_move_2_yincod,
        call_t_print_l_yincod, redraw_when_chmsig_yincod, char_len_yincod,
        char_q_len_yincod, t_redraw_move_yincod,
        t_print_l_yincod, c_top_yincod, c_end_yincod, c_end_nobi_yincod,
	call_redraw_line_yincod, hani_settei_yincod
      }
  },
    { NULL,
	{ NULL,   NULL,   call_t_redraw_move_normal,
        call_t_redraw_move_1_normal, call_t_redraw_move_2_normal,
        call_t_print_l_normal, NULL, char_len_normal, char_q_len_normal,
        t_redraw_move_normal, t_print_l_normal, c_top_normal, c_end_normal,
        c_end_nobi_normal, call_redraw_line_normal, hani_settei_normal
      }
  }
};

FunctionTable default_func_table = {
        NULL,   NULL,   call_t_redraw_move_normal,
        call_t_redraw_move_1_normal, call_t_redraw_move_2_normal,
        call_t_print_l_normal, NULL, char_len_normal, char_q_len_normal,
        t_redraw_move_normal, t_print_l_normal, c_top_normal, c_end_normal,
        c_end_nobi_normal, call_redraw_line_normal, hani_settei_normal
      };
