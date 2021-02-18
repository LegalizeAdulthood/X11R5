/*
 * $Id: header.c,v 1.3 1991/09/16 21:36:49 ohm Exp $
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
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"

int rubout_code = 127;
int kk_on = 0;
int quote_code = -1;

char *ximrc_file = NULL;
char *cvt_fun_file = NULL;
char *cvt_meta_file = NULL;
int defined_by_option = 0;

WNN_DIC_INFO *dicinfo;
int dic_list_size;

char *prgname;				/* program name (argv[0])	*/

char *xim_title = NULL;		/* title string when henkan-off	*/

char *display_name = NULL;	/* display string		*/
#ifdef	USING_XJUTIL
char *xjutil_name = NULL;	/* xjutil string		*/
#endif	/* USING_XJUTIL */

/*
 * Default env
 */
char *def_lang;

char *cur_lang = NULL;

WnnClientRec	*c_c = NULL;
XIMClientRec	*cur_p = NULL;
XIMClientRec	*cur_x = NULL;
XIMInputRec	*cur_input = NULL;
XInputManager	*xim = NULL;
XIMInputList	input_list = NULL;
WnnClientList	wnnclient_list = NULL;
XIMClientList	ximclient_list = NULL;
LangUumrcList	languumrc_list = NULL;
ReadRkfileList	read_rkfile_list = NULL;
BoxRec		*box_list = NULL;

Romkan		*cur_rk = NULL; 
RomkanTable	*cur_rk_table = NULL;

char read_rk_file[MAX_LANGS][62];

struct msg_cat *cd;

char		*root_def_servername = NULL;
char		*root_def_reverse_servername = NULL;
char		*root_username = NULL;
Bool		root_henkan_off_def = NULL;
char		*root_uumkeyname = NULL;
char		*root_rkfilename = NULL;

XIMLangDataBase language_db[] = {
    { "ja_JP",
      "-*--14-*-iso8859-1,-*--14-*-jisx0208.1983-0,-*--14-*-jisx0201.1976-0",
      True, "ja_JP.ujis",
      {	NULL,	NULL,	call_t_redraw_move_normal,
	call_t_redraw_move_1_normal, call_t_redraw_move_2_normal,
	call_t_print_l_normal, NULL, char_len_normal, char_q_len_normal,
	t_redraw_move_normal, t_print_l_normal, c_top_normal, c_end_normal,
	c_end_nobi_normal
      }
    },
    { "zh_CN",
      "-*--18-*-iso8859-1,-*--18-*-gb2312.1980-0,-*--18-*-pinyin.1989-0",
      True, "zh_CN.ugb",
      {	print_out_yincod, input_yincod, call_t_redraw_move_yincod,
	call_t_redraw_move_1_yincod, call_t_redraw_move_2_yincod,
	call_t_print_l_yincod, redraw_when_chmsig_yincod, char_len_yincod,
	char_q_len_yincod, t_redraw_move_yincod,
	t_print_l_yincod, c_top_yincod, c_end_yincod, c_end_nobi_yincod
      }
    },
    { "ko_KR",
      "-*--16-*-iso8859-1,-*--16-*-ksc5601.1987-0",
      False, "ko_KR.euc",
      {	NULL,	NULL,	call_t_redraw_move_normal,
	call_t_redraw_move_1_normal, call_t_redraw_move_2_normal,
	call_t_print_l_normal, NULL, char_len_normal, char_q_len_normal,
	t_redraw_move_normal, t_print_l_normal, c_top_normal, c_end_normal,
	c_end_nobi_normal
      }
    },
    { "lt_LN",
      "-*--14-*-iso8859-1",
      False, "lt_LN.bit8",
      {	NULL,	NULL,	call_t_redraw_move_normal,
	call_t_redraw_move_1_normal, call_t_redraw_move_2_normal,
	call_t_print_l_normal, NULL, char_len_normal, char_q_len_normal,
	t_redraw_move_normal, t_print_l_normal, c_top_normal, c_end_normal,
	c_end_nobi_normal
      }
    },
    { "wr_WR",
      "-*--18-*",
      False, "wr_WR.ct",
      { NULL,	NULL,	call_t_redraw_move_normal,
	call_t_redraw_move_1_normal, call_t_redraw_move_2_normal,
	call_t_print_l_normal, NULL, char_len_normal, char_q_len_normal,
	t_redraw_move_normal, t_print_l_normal, c_top_normal, c_end_normal,
	c_end_nobi_normal
      }
    },
    { NULL, NULL, False, NULL,
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

