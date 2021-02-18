/*
 * $Id: key_bind.c,v 1.3 1991/09/16 21:37:03 ohm Exp $
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
#include <pwd.h>
#include "commonhd.h"
#include "config.h"
#include "sdefine.h"
#ifdef	XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#else	/* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#endif	/* XJUTIL */

extern char *getenv();
#ifdef	XJUTIL
static struct kansuu kansuu_hyo[] = {
{ "send_string",	"send what is in the buffer to pty.",	 0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL },
{ "forward_char",	"cursor wo hitotu maeni susumeru.",	1,
  NULL,		forward,	NULL,		NULL,		NULL,
  forward,	forward, 	NULL, 		NULL, 		NULL,
  NULL},
{ "backward_char",	"cursor wo hitotu usironi susumeru.",	1,
  NULL,		backward_c,	NULL,		NULL,		NULL,
  backward,	backward,	NULL,		NULL,		NULL,
  NULL},
{ "goto_top_of_line",	"",					1,
  NULL,		c_top,		NULL,		NULL,		NULL,
  c_top,	c_top,		NULL,		NULL,		NULL,
  NULL},
{ "goto_end_of_line",	"",					1,
  NULL,		c_end,		c_end_nobi,	NULL,		NULL,
  c_end,	c_end,		NULL,		NULL,		NULL,
  NULL},
{ "delete_char_at_cursor" ,"",					1,
  NULL,		delete_c,	NULL,		NULL,		NULL,
  t_delete_char,t_delete_char,	NULL,		NULL,		NULL,
  NULL },
{ "kaijo",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
 NULL} ,
{ "henkan",		"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "tan_henkan",		"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "tan_henkan_dai",	"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
#ifdef	nodef
#ifdef	CONVERT_by_STROKE
{ "qhenkan",		"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
#endif	/* CONVERT_by_STROKE */
#endif
{ "jikouho",			"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "zenkouho",			"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "jikouho_dai",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "zenkouho_dai",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "select_jikouho",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
#ifdef	nodef
#ifdef	CONVERT_by_STROKE
{ "select_jikouho_q",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
#endif	/* CONVERT_by_STROKE */
#endif
{ "select_jikouho_dai",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "kill",			"",					1,
  NULL,		kill_c,		NULL,		NULL,		NULL,
  t_kill,	t_kill,		NULL,		NULL,		NULL,
  NULL},
{ "yank",			"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "yank_e",			"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "bunsetu_nobasi",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "bunsetu_chijime",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "jisho_utility",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "jisho_utility_e",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "touroku",			"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "touroku_e",			"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "kakutei",			"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "sainyuuryoku",		"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "sainyuuryoku_e",		"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "kuten",			"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "kuten_e",			"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "jis",			"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "jis_e",			"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "redraw_line",		"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "redraw_line_e",		"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "previous_history_e",		"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "previous_history",		"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "next_history_e",		"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "next_history",		"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "touroku_mark_set",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  t_markset,	NULL,		NULL,		NULL,		NULL,
  NULL},
{ "touroku_return",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		t_ret,		NULL,		NULL,		NULL,
  NULL},
{ "quit",			"",					0,
  NULL,		NULL,		NULL,		NULL,		quit_select,
  t_quit,	t_quit,		NULL,		NULL,		NULL,
  quit_select},
{ "touroku_jump_forward",	"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		t_jmp_forward,	NULL,		NULL,		NULL,
  NULL},
{ "touroku_jump_backward",	"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		t_jmp_backward,	NULL,		NULL,		NULL,
  NULL},
{ "change_to_insert_mode",	"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "quote_e",			"",					0,
  NULL,		quote,		NULL,		quote,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "quote",			"",					0,
  NULL,		quote,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "send_ascii_char",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "toggle_send_ascii_char",	"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "not_send_ascii_char",	"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "pop_send_ascii_char",	"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "quote_send_ascii_char",	"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "select_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,		select_select,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "lineend_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "linestart_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
/*{ "previous_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,		previous_select,
  NULL,		NULL,		NULL,		NULL,		NULL,
  previous_select},*/
{ "previous_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,	xw_previous_select,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
/*{ "next_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,		next_select,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},*/
{ "next_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,		xw_next_select,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "backward_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "forward_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "reconnect_jserver",		"",					0,
  reconnect_server,reconnect_server,reconnect_server,reconnect_server,
  reconnect_server,
  reconnect_server,reconnect_server,reconnect_server,reconnect_server,
  reconnect_server,
  reconnect_server},
{ "henkan_on",			"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "inspect",			"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "sakujo_kouho",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "nobi_henkan_dai",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "nobi_henkan",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "del_entry",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		kworddel,	kdicdel,
  NULL},
{ "use_entry",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		kworduse,	kdicuse,
  NULL},
{ "com_entry",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		kwordcom,	kdiccom,
  NULL},
{ "kana_henkan",		"",					1,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "change_locale",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "change_locale_ct",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL}
};
#else	/* XJUTIL */
extern int lang_c();
extern int lang_ct();
static struct kansuu kansuu_hyo[] = {
{ "send_string",	"send what is in the buffer to pty.",	 0,
  send_string,	send_string,	send_string,	NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL },
{ "forward_char",	"cursor wo hitotu maeni susumeru.",	1,
  forward_bunsetsu,forward,	henkan_forward,	NULL,		NULL,
  forward,	forward,	NULL,		NULL,		NULL,
  NULL},
{ "backward_char",	"cursor wo hitotu usironi susumeru.",	1,
  backward_bunsetsu,backward_c,	henkan_backward,NULL,		NULL,
  backward,	backward,	NULL,		NULL,		NULL,
  NULL},
{ "goto_top_of_line",	"",					1,
  top_bunsetsu,	c_top,		jmptijime,	NULL,		NULL,
  c_top,	c_top,		NULL,		NULL,		NULL,
  NULL},
{ "goto_end_of_line",	"",					1,
  end_bunsetsu,	c_end,		c_end_nobi,	NULL,		NULL,
  c_end,	c_end,		NULL,		NULL,		NULL,
  NULL},
{ "delete_char_at_cursor" ,"",					1,
  NULL,		delete_c,	NULL,		NULL,		NULL,
  t_delete_char,t_delete_char,	NULL,		NULL,		NULL,
  NULL },
{ "kaijo",		"",					0,
  kaijo,	NULL,		kaijo,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
 NULL} ,
{ "henkan",		"",					1,
  NULL,		ren_henkan,	NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "tan_henkan",		"",					1,
  NULL,		tan_henkan,	NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "tan_henkan_dai",	"",					1,
  NULL,		tan_henkan_dai,	NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
#ifdef	nodef
#ifdef	CONVERT_by_STROKE
{ "qhenkan",		"",					1,
  NULL,		question_henkan,NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
#endif	/* CONVERT_by_STROKE */
#endif
{ "jikouho",			"",					0,
  jikouho_c,	NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "zenkouho",			"",					0,
  zenkouho_c,	NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "jikouho_dai",		"",					0,
  jikouho_dai_c,NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "zenkouho_dai",		"",					0,
  zenkouho_dai_c,NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "select_jikouho",		"",					0,
  select_jikouho,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
#ifdef	nodef
#ifdef	CONVERT_by_STROKE
{ "select_jikouho_q",		"",					0,
  select_question,NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
#endif	/* CONVERT_by_STROKE */
#endif
{ "select_jikouho_dai",		"",					0,
  select_jikouho_dai,NULL,	NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "kill",			"",					1,
  NULL,		kill_c,		NULL,		NULL,		NULL,
  t_kill,	t_kill,		NULL,		NULL,		NULL,
  NULL},
{ "yank",			"",					1,
  NULL,		yank_c,		NULL,		NULL,		NULL,
  t_yank,	t_yank,		NULL,		NULL,		NULL,
  NULL},
{ "yank_e",			"",					1,
  NULL,		yank_c,		NULL,		yank_c,		NULL,
  t_yank,	t_yank,		NULL,		NULL,		NULL,
  NULL},
{ "bunsetu_nobasi",		"",					0,
  enlarge_bunsetsu,NULL,	forward,	NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "bunsetu_chijime",		"",					0,
  smallen_bunsetsu,NULL,	tijime,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "jisho_utility",		"",					0,
  jutil_c,	jutil_c,	jutil_c,	NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "jisho_utility_e",		"",					0,
  jutil_c,	jutil_c,	jutil_c,	jutil_c,	NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "touroku",			"",					1,
  touroku_c,	touroku_c,	touroku_c,	NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "touroku_e",			"",					1,
  touroku_c,	touroku_c,	touroku_c,	touroku_c,	NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "kakutei",			"",					1,
  kakutei,	kakutei,	kakutei,	NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "sainyuuryoku",		"",					1,
  NULL,		remember_me,	NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "sainyuuryoku_e",		"",					1,
  NULL,		remember_me,	NULL,		remember_me,	NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "kuten",			"",					0,
  NULL,		kuten,		NULL,		NULL,		NULL,
  kuten,	kuten,		NULL,		NULL,		NULL,
  NULL},
{ "kuten_e",			"",					0,
  NULL,		kuten,		NULL,		kuten,		NULL,
  kuten,	kuten,		NULL,		NULL,		NULL,
  NULL},
{ "jis",			"",					0,
  NULL,		jis,		NULL,		NULL,		NULL,
  jis,		jis,		NULL,		NULL,		NULL,
  NULL},
{ "jis_e",			"",					0,
  NULL,		jis,		NULL,		jis,		NULL,
  jis,		jis,		NULL,		NULL,		NULL,
  NULL},
{ "redraw_line",		"",					1,
  reset_line,	reset_line,	reset_line,	NULL,		reset_line,
  reset_line,	reset_line,	NULL,		NULL,		NULL,
  NULL},
{ "redraw_line_e",		"",					1,
  reset_line,	reset_line,	reset_line,	reset_line,	reset_line,
  reset_line,	reset_line,	NULL,		NULL,		NULL,
  NULL},
{ "previous_history_e",		"",					1,
  NULL,		previous_history,NULL,		previous_history,NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "previous_history",		"",					1,
  NULL,		previous_history,NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "next_history_e",		"",					1,
  NULL,		next_history,	NULL,		next_history,	NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "next_history",		"",					1,
  NULL,		next_history,	NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
/*{ "touroku_mark_set",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  t_markset,	NULL,		NULL,		NULL,		NULL,
  NULL}, */
{ "touroku_mark_set",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "touroku_return",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		t_ret,		NULL,		NULL,		NULL,
  NULL},
{ "quit",			"",					0,
  NULL,		NULL,		NULL,		NULL,		quit_select,
  t_quit,	t_quit,		NULL,		NULL,		NULL,
  quit_select},
{ "touroku_jump_forward",	"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		t_jmp_forward,	NULL,		NULL,		NULL,
  NULL},
{ "touroku_jump_backward",	"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		t_jmp_backward,	NULL,		NULL,		NULL,
  NULL},
{ "change_to_insert_mode",	"",					0,
  insert_it_as_yomi,NULL,	NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "quote_e",			"",					0,
  NULL,		quote,		NULL,		quote,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "quote",			"",					0,
  NULL,		quote,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "send_ascii_char",		"",					0,
  send_ascii,	send_ascii,	send_ascii,	send_ascii,	send_ascii,
  send_ascii,	send_ascii,	send_ascii,	send_ascii,	send_ascii,
  send_ascii},
{ "toggle_send_ascii_char",	"",					0,
  toggle_send_ascii,toggle_send_ascii,toggle_send_ascii,toggle_send_ascii,
  toggle_send_ascii,
  toggle_send_ascii,toggle_send_ascii,toggle_send_ascii,toggle_send_ascii,
  toggle_send_ascii,
  toggle_send_ascii},
{ "not_send_ascii_char",	"",					0,
  not_send_ascii,not_send_ascii,not_send_ascii,not_send_ascii,not_send_ascii,
  not_send_ascii,not_send_ascii,not_send_ascii,not_send_ascii,not_send_ascii,
  not_send_ascii},
{ "pop_send_ascii_char",	"",					0,
  pop_send_ascii,pop_send_ascii,pop_send_ascii,pop_send_ascii,pop_send_ascii,
  pop_send_ascii,pop_send_ascii,pop_send_ascii,pop_send_ascii,pop_send_ascii,
  pop_send_ascii},
{ "quote_send_ascii_char",	"",					0,
  NULL,		NULL,		NULL,		quote_send_ascii_e,NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "select_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,		select_select,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "lineend_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,		lineend_select,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "linestart_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,	linestart_select,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
/*{ "previous_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,		previous_select,
  NULL,		NULL,		NULL,		NULL,		NULL,
  previous_select},*/
{ "previous_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,		previous_select,
  NULL,		NULL,		NULL,		NULL,		NULL,
  previous_select},
/*{ "next_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,		next_select,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},*/
{ "next_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,		next_select,
  NULL,		NULL,		NULL,		NULL,		NULL,
  next_select},
{ "backward_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,		backward_select,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "forward_select",		"",					0,
  NULL,		NULL,		NULL,		NULL,		forward_select,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "reconnect_jserver",		"",					0,
  reconnect_server,reconnect_server,reconnect_server,reconnect_server,
  reconnect_server,
  reconnect_server,reconnect_server,reconnect_server,reconnect_server,
  reconnect_server,
  reconnect_server},
{ "henkan_on",			"",					0,
  henkan_off,	henkan_off,	henkan_off,	henkan_off,	henkan_off,
  henkan_off,	henkan_off,	henkan_off,	henkan_off,	henkan_off,
  henkan_off},
{ "inspect",			"",					0,
  inspect_kouho,NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "sakujo_kouho",		"",					0,
  sakujo_kouho,	NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "nobi_henkan_dai",		"",					0,
  NULL,		NULL,		nobi_henkan_dai,NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "nobi_henkan",		"",					0,
  NULL,		NULL,		nobi_henkan,	NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
/*{ "del_entry",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		kworddel,	kdicdel,
  inspectdel}, */
{ "del_entry",			"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  inspectdel},
/*{ "use_entry",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		kworduse,	kdicuse,
  inspectuse}, */
{ "use_entry",			"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  inspectuse},
/*{ "com_entry",		"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		kwordcom,	kdiccom,
  NULL}, */
{ "com_entry",			"",					0,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "kana_henkan",		"",					1,
  NULL,		kankana_ren_henkan,NULL,	NULL,		NULL,
  NULL,		NULL,		NULL,		NULL,		NULL,
  NULL},
{ "change_locale",		"",					0,
  lang_c,	lang_c,		lang_c,		lang_c,		lang_c,
  lang_c,	lang_c,		lang_c,		lang_c,		lang_c,
  lang_c},
{ "change_locale_ct",		"",					0,
  lang_ct,	lang_ct,	lang_ct,	lang_ct,	lang_ct,
  lang_ct,	lang_ct,	lang_ct,	lang_ct,	lang_ct,
  lang_ct}
};
#endif	/* XJUTIL */

int kansuu_hyo_entry_suu = sizeof(kansuu_hyo) / sizeof(struct kansuu);

static char key_binded[sizeof(kansuu_hyo)/sizeof(struct kansuu) + 1];

/** argument expansion. */
static int
expand_argument(st)
char *st;
{
    int num;

    if (*st == '^') {
	num = (*(st + 1)) & 0x1f;
    } else if (*st == '0' || *st == '\\') {
	if (*(st + 1) == 'x' || *(st + 1) == 'X') {
	    sscanf(st + 2,"%x",&num);
	} else {
	    sscanf(st,"%o",&num);
	}
    } else if (!('0' <= *st && *st <= '9')) {
	return(-1);
    } else {
	num = atoi(st);
    }
    if (num >= TBL_SIZE) {
	print_out1(
    "In uumkey file, you can only bind a function to code less than %d.",
    (char *)TBL_SIZE);
	return(-1);
    }
    return(num);
}

static void
remove_key_bind(key)
int key;
{
    register int tbl;
    register int com;

    for (com = 0 ; com < TBL_SIZE; com++) {
	for (tbl = 0 ; tbl < TBL_CNT ; tbl++) {
	    if (kansuu_hyo[key].func[tbl] != NULL) {
		if (main_table[tbl][com] == kansuu_hyo[key].func[tbl]) {
		    main_table[tbl][com] = (int (*)())0;
		}
	    }
	}
    }
}

static int
find_entry(com_name)
char *com_name;
{
    register int k;

    for (k = 0 ; k < kansuu_hyo_entry_suu ; k++) {
	if (strcmp(com_name, kansuu_hyo[k].kansuumei)== 0) {
	    if (key_binded[k]) {
		return(k);
	    }
	}
    }
    return(-1);
}

int
expand_expr(s, lang)
char	*s, *lang;
{
    char tmp[EXPAND_PATH_LENGTH];
    register char	*p, *s1;
    int	noerr, expandsuc;
    struct passwd	*u;
    extern struct passwd *getpwnam();

    if(*s != '~' && *s != '@'){
	strcpy(tmp, s);
	*s = '\0';
	noerr = 1;
    }else{
	if((int)strlen(s) >= EXPAND_PATH_LENGTH) return(-1);

	s1 = s;
#ifdef BSD42	
	if(NULL != (p = index(++s1, '/'))) {
#else
	if(NULL != (p = strchr(++s1, '/'))) {
#endif
	    strcpy(tmp, p);
	    *p = '\0';
	} else *tmp = '\0';
	if(*s == '~') {
	    if(*s1) {
		noerr = expandsuc = (NULL != (u = getpwnam(s1)) &&
		    (int)(strlen(p = u -> pw_dir) + strlen(tmp)) <
		     EXPAND_PATH_LENGTH );
	    } else {
		noerr = expandsuc = (NULL != (p = getenv("HOME")) &&
		    (int)(strlen(p) + strlen(tmp)) < EXPAND_PATH_LENGTH);
	    }
	} else { /* then, *s must be '@' */
	    if(!strcmp(s1, "HOME")) {
		noerr = expandsuc = (NULL != (p = getenv("HOME")) &&
		    (int)(strlen(p) + strlen(tmp)) < EXPAND_PATH_LENGTH);
	    } else if(!strcmp(s1, "LIBDIR")) {
		noerr = expandsuc = ((int)(strlen(p= LIBDIR)+ strlen(tmp)) <
		    EXPAND_PATH_LENGTH);
	    } else if(!strcmp(s1, "LANG")) {
		noerr = expandsuc = ((int)(strlen(p= lang)+ strlen(tmp)) <
		    EXPAND_PATH_LENGTH);
	    } else { /* @HOME, @LIBDIR igai ha kaenai */
		noerr = 1; expandsuc = 0;
	    }
	}
	if(expandsuc) strcpy(s, p);
    }

    if (noerr) {
	int len = strlen("@USR");
	int len1 = strlen("@LANG");
	p = tmp;
	for(; *p; p++) {
	    if (!strncmp(p, "@USR", len)) {
		if ((int)(strlen(username) + strlen(p) + strlen(s) - len) <
		     EXPAND_PATH_LENGTH) {
		    strcat(s, username);
		    p += len - 1;
		} else {
		    return (-1);
		}
	    } else if (!strncmp(p, "@LANG", len1)) {
		if ((int)(strlen(lang) + strlen(p) + strlen(s) - len) <
		     EXPAND_PATH_LENGTH) {
		    strcat(s, lang);
		    p += len1 - 1;
		} else {
		    return (-1);
		}
	    }else{
		strncat(s, p, 1);
	    }
	}
    }
    return(noerr ? 0 : -1);
}

static int
key_binding(file_name, lang)
char *file_name;
char *lang;
{
    register int k, kk;
    FILE *fp , *fopen();
    char func_name[64];
    int com_cnt;
    int com_num ;
    char buf[256];
    char com_name[16][EXPAND_PATH_LENGTH];
    int lc = 0;
    int tbl;

    if ((fp = fopen(file_name , "r")) == NULL) {
	print_out1("Key_binding file \"%s\" doesn't exist.",file_name);
	return(-1);
    }
  
    for (tbl = 0 ; tbl < TBL_CNT ; tbl++) {
	romkan_clear_tbl[tbl][rubout_code] = 0;
    }
    main_table[1][rubout_code] = rubout_c;
    main_table[5][rubout_code] = t_rubout;
    main_table[6][rubout_code] = t_rubout;

    lc = 0;
    while(fgets(buf , 256 , fp)) {
	lc += 1;
	if ((*buf == ';') || (*buf == ':') || (*buf == '\n'))
	    continue;
	if ((com_cnt = sscanf(buf , "%s %s %s %s %s %s %s %s %s %s %s",
			     func_name,com_name[0],com_name[1],com_name[2],
			     com_name[3],com_name[4],com_name[5],com_name[6],
			     com_name[7], com_name[8],com_name[9])) < 2) {
	    print_out2("In uumkey file \"%s\", Line %d is illegal.",
		      file_name, (char *)lc);
	    continue;       /* every command needs at least 1 argument. */
	}

	if (strcmp(func_name, "include")== 0) {
	    if (0 != expand_expr(com_name[0], lang)) {
		print_out3("In uumkey file \"%s\", %s %s.",
			  file_name, com_name[0],
			  msg_get(cd, 22, default_message[22], lang));
	    }
	    key_binding(com_name[0], lang);
	} else if (strcmp(func_name, "unset")== 0) {
	    if ((k = find_entry(com_name[0])) != -1) {
		key_binded[k] = 0;
		remove_key_bind(k);
	    } else if (strcmp(com_name[0],"quote_keyin") == 0) {
#ifndef	XJUTIL
		quote_code = -1;
#endif	/* !XJUTIL */
	    }
	} else if (strcmp(func_name, "quote_keyin")== 0) {
#ifndef	XJUTIL
	    if ((com_num = expand_argument(com_name[0])) < 0) {
		print_out3(
		"In uumkey file \"%s\",%s is an inappropriate code for %s.",
			  file_name, com_name[0], func_name);
		fflush(stderr);
	    } else quote_code = com_num;
#endif	/* !XJUTIL */
	} else {
	    for (k = 0 ; k < kansuu_hyo_entry_suu ; k++) {
		if (strcmp(func_name, kansuu_hyo[k].kansuumei)== 0) { /*found!*/
		    if (key_binded[k]) {
			remove_key_bind(k);
		    }
		    key_binded[k] = 1;
		    for (kk = 0 ; kk < com_cnt - 1 ; kk++) {
			com_num = expand_argument(com_name[kk]);
			if (com_num < 0) {
			    print_out3(
		"In uumkey file \"%s\",%s is an inappropriate code for %s.",
				      file_name, com_name[kk], func_name);
			    fflush(stderr);
			} else {
			    for (tbl = 0 ; tbl < TBL_CNT ; tbl++) {
				if ((main_table[tbl][com_num] != NULL) && 
				    (kansuu_hyo[k].func[tbl] != NULL)) {
				    print_out7(
	"In uumkey file \"%s\", %s(0x%x)%s at line %d (%s)(table No. %d).",
		file_name, (char *)com_name[kk], (char *)com_num,
		(char *)msg_get(cd, 23, default_message[23], lang),
		(char *)lc, kansuu_hyo[k].kansuumei, (char *)tbl);
				    main_table[tbl][com_num]
					= kansuu_hyo[k].func[tbl];
				    romkan_clear_tbl[tbl][com_num]
					= kansuu_hyo[k].romkan_flag;
				} else if (kansuu_hyo[k].func[tbl] != NULL) {
				    main_table[tbl][com_num]
					= kansuu_hyo[k].func[tbl];
				    romkan_clear_tbl[tbl][com_num]
					= kansuu_hyo[k].romkan_flag;
				}
			    }
			}
		    }
	  break;
	} /* end of routine when command is founded. */
       
	/* loop while command is founded */
	    }
	    if (k == kansuu_hyo_entry_suu) {
		print_out2("In uumkey file \"%s\", unknown entry name %s.",
			  file_name, func_name);
	    }
	}
    }
    fclose(fp);
    return(0);
}

int
init_key_table(lang)
char *lang;
{
    FILE *fp , *fopen(); char file_name[PATHNAMELEN];
    int j,k;
    extern int fclose();

    for (k = 0 ; k < sizeof(kansuu_hyo)/sizeof(struct kansuu);k++) {
	key_binded[k] = 0;
    }

#ifdef	XJUTIL
    if (uumkey_name && *uumkey_name)
	strcpy(file_name, uumkey_name);
    if ((fp = fopen(file_name, "r")) != NULL){
	fclose(fp);
    } else {
	print_out2("%s %s",msg_get(cd, 72, default_message[72], lang),CPFILE);
	return(-1);
    }
#else	/* XJUTIL */
    /* from uumrc */
    if (uumkeyname == NULL || uumkeyname[0] != '\0') {
	strcpy(file_name, uumkeyname);
    } else if (strcpy(file_name , LIBDIR),
	      strcat(file_name, "/"),
	      strcat(file_name, lang),
	      strcat(file_name, CPFILE),
	      (fp = fopen(file_name , "r")) != NULL) {
	/* default */
	uumkeyname = alloc_and_copy(file_name);
	fclose(fp);
    } else {
	print_out2("%s %s",msg_get(cd, 21, default_message[21], lang),CPFILE);
	return(-1);
    }

    c_c->m_table = (MainTable *)Malloc(sizeof(MainTable));
    c_c->m_table->re_count = 1;
#endif	/* XJUTIL */
    for (j = 0 ; j < 3 ; j++) {
	for (k = 0;k < TBL_SIZE;k++) {
	    main_table[j][k] = (int (*)())0;
	}
	main_table[j][NEWLINE] = (int (*)())0;
	main_table[j][CR] = (int (*)())0;
    }
    for (j = 3 ; j <TBL_CNT ; j++) {
	for (k = 0;k < TBL_SIZE;k++) {
	    main_table[j][k] = (int (*)())0;
	}
    }
    if (key_binding(file_name, lang) == -1)
	return(-1);
    return(0);
}

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/
