/*
 * $Id: xheader.c,v 1.2 1991/09/16 21:35:31 ohm Exp $
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
#include "config.h"
#include "commonhd.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"

int	max_env = 0;
WnnEnv *normal_env = NULL;
WnnEnv *reverse_env = NULL;
WnnEnv *cur_normal_env = NULL;
WnnEnv *cur_reverse_env = NULL;

int save_env_id[32];
int save_env_reverse_id[32];

w_char *input_buffer;
w_char *return_buf;	/* return you mojiretsu buffer */

int	cur_bnst_  = 0;	/* current bunsetsu pointer */

int rubout_code = 127;

int (*main_table[TBL_CNT][TBL_SIZE])();
char romkan_clear_tbl[TBL_CNT][TBL_SIZE];
w_char jishopath[64];
w_char hindopath[64];
w_char fuzokugopath[64];

ClientBuf *c_b;

int excellent_delete = 1;

int *bunsetsu;
int *bunsetsuend;
int *touroku_bnst;
int touroku_bnst_cnt;
w_char *kill_buffer;
w_char *remember_buf;
int kill_buffer_offset = 0;

int touroku_comment = 0;

char *uumrc_name = NULL;
char *envrc_name = NULL;
char *uumkey_name = NULL;
char *cvt_fun_file = NULL;
char *cvt_meta_file = NULL;
char *rkfile_name = NULL;

char *jserver_name = NULL;
char *display_name = NULL;
char *username = NULL;

int maxbunsetsu;
int max_ichiran_kosu;
int maxchg;
int maxlength;

WNN_DIC_INFO *dicinfo;
int dic_list_size;

char *prgname;	/* argv[0] */

struct wnn_ret_buf	rb = {0, NULL};
struct wnn_ret_buf	dicrb = {0, NULL};
struct wnn_ret_buf	wordrb = {0, NULL};

Xjutil		*xjutil = NULL;
JutilTextRec	*cur_text = NULL;

Romkan		*cur_rk = NULL;
RomkanTable	*cur_rk_table = NULL;

BoxRec		*box_list = NULL;

XjutilFSList	font_set_list = NULL;
XjutilFSRec	*cur_fs = NULL;

struct msg_cat *cd;

FunctionTable *f_table = NULL;

XjutilFuncDataBase function_db[] = {
    { "ja_JP",
      {	NULL,	NULL,	call_t_redraw_move_normal,
	call_t_redraw_move_1_normal, call_t_redraw_move_2_normal,
	call_t_print_l_normal, NULL, char_len_normal, char_q_len_normal,
	t_redraw_move_normal, t_print_l_normal, c_top_normal, c_end_normal,
	c_end_nobi_normal
      }
    },
    { "zh_CN",
      {	print_out_yincod, input_yincod, call_t_redraw_move_yincod,
	call_t_redraw_move_1_yincod, call_t_redraw_move_2_yincod,
	call_t_print_l_yincod, redraw_when_chmsig_yincod, char_len_yincod,
	char_q_len_yincod, t_redraw_move_yincod,
	t_print_l_yincod, c_top_yincod, c_end_yincod, c_end_nobi_yincod
      }
    },
    { "ko_KR",
      {	NULL,	NULL,	call_t_redraw_move_normal,
	call_t_redraw_move_1_normal, call_t_redraw_move_2_normal,
	call_t_print_l_normal, NULL, char_len_normal, char_q_len_normal,
	t_redraw_move_normal, t_print_l_normal, c_top_normal, c_end_normal,
	c_end_nobi_normal
      }
    },
    { "lt_LN",
      {	NULL,	NULL,	call_t_redraw_move_normal,
	call_t_redraw_move_1_normal, call_t_redraw_move_2_normal,
	call_t_print_l_normal, NULL, char_len_normal, char_q_len_normal,
	t_redraw_move_normal, t_print_l_normal, c_top_normal, c_end_normal,
	c_end_nobi_normal
      }
    },
    { NULL,
      {	NULL,	NULL,	call_t_redraw_move_normal,
	call_t_redraw_move_1_normal, call_t_redraw_move_2_normal,
	call_t_print_l_normal, NULL, char_len_normal, char_q_len_normal,
	t_redraw_move_normal, t_print_l_normal, c_top_normal, c_end_normal,
	c_end_nobi_normal
      }
    }
};


FunctionTable default_func_table = {
	NULL,	NULL,	call_t_redraw_move_normal,
	call_t_redraw_move_1_normal, call_t_redraw_move_2_normal,
	call_t_print_l_normal, NULL, char_len_normal, char_q_len_normal,
	t_redraw_move_normal, t_print_l_normal, c_top_normal, c_end_normal,
	c_end_nobi_normal
      };

