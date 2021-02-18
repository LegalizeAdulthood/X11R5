/*
 * $Id: client.h,v 1.3 1991/09/16 21:36:33 ohm Exp $
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
/*	Version 4.0
 */
/*
	client.h
*/

#ifndef	JLLIB
#include "jllib.h"
#endif	/* JLLIB */

#include "rk_header.h"
#include "rk_multi.h"

/* 
 *	 when mark is set
 *        	hanten bit0,bit1:      --- mark
 *		bit2,bit3:        mark --- cursor
 *		bit4,bit5:      cursor --- 
 *	  when mark is not set(-1)
 *		bit4,bit5:      
 *    first one....hanten
 *    second one...under_line
 */

typedef struct _ClientBuf {
    w_char	*buffer;		/* pointer for buffer		*/
    short	t_c_p;			/* position of cursor on buffer	*/
    short	hanten;			/* see over			*/
    short	maxlen;			/* length of string on buffer	*/
    short	buflen;			/* max length of the buffer	*/
    short	t_m_start;		/* -1      : mark is NOT set
					    > 0    : position of mark	*/
    short	t_b_st;			/* start position of bolding	*/
    short	t_b_end;		/* end position of bolding	*/

    int		(*key_in_fun)();	/* */
    int		(*redraw_fun)();	/* */
    int		(*ctrl_code_fun)();	/* */

    int		(**key_table)();	/* table of keybinding		*/
    char	*rk_clear_tbl;		/* flag of romakn_clear 	*/
} ClientBuf;

#ifndef	XJUTIL
typedef	struct _MainTable {
    int		(*m_table[11][256])();
    char	rk_cl_tbl[11][256];
    int		re_count;
} MainTable;

typedef struct _History {
	w_char *hbuffer;
	struct _History *previous;
	struct _History *next;
	int num;
} History;
#endif /* !XJUTIL */

typedef struct _WnnEnv {
    char		*host_name;	/* server name */
    struct wnn_env	*env;		/* env */
    int			sticky;		/* sticky bit */
    char		*envrc_name;	/* envrc name */
    char		env_name_str[32];	/* env name */
    struct _WnnEnv	*next;
} WnnEnv;

typedef	struct _FunctionTable {
/* functions depends on lang */
    int		(*print_out_function)();
    int		(*input_function)();
    int		(*call_t_redraw_move_function)();
    int		(*call_t_redraw_move_1_function)();
    int		(*call_t_redraw_move_2_function)();
    int		(*call_t_print_l_function)();
    int		(*redraw_when_chmsig_function)();
    int		(*char_len_function)();
    int		(*char_q_len_function)();
    int		(*t_redraw_move_function)();
    int		(*t_print_l_function)();
    int		(*c_top_function)();
    int		(*c_end_function)();
    int		(*c_end_nobi_function)();
} FunctionTable;

#ifndef	XJUTIL
/*
 * WnnClient structure: Client's data for Wnn env.
 */

typedef struct _WnnClientRec *WnnClientList;

typedef struct _WnnClientRec {
    int		use_server;
    char	*user_name;		/* env_name		 	*/
    char	*host_name;
    char	*rev_host_name;
    WnnEnv	*cur_normal_env; 
    WnnEnv	*cur_reverse_env;
    WnnEnv	*normal_env; 
    WnnEnv	*reverse_env;

    int		two_byte_first;		/*  */
    w_char	wc;			/*  */

/* buffers */
    struct wnn_buf	*b_data_;	/* this means bun_data_		*/
    ClientBuf	*c_buf;			/* this means c_b		*/

    char	*uumkey_name;		/* uumkey file name		*/
    char	*rkfile_name;		/* romkan file name		*/

    int		*b_setsu;		/* pointer for bunsetsu start	*/
    int		*b_setsuend;		/* pointer for bunsetsu end	*/

    struct wnn_env	**b_setsuenv;	/* pointer for bunsetsu env	*/
    int		*t_bnst;		/* pointer for regist start	*/
    int		t_bnst_cnt;		/* count of regist bunsetsu	*/

    w_char	*k_buffer;
    int		k_buffer_offset;
    w_char	*rem_buf;
    w_char	*i_buffer;	/*  */
    w_char	*ret_buf;	/*  */

    int		t_comment;	/* Comment for Words in Word_add */

/* key_table */
    MainTable	*m_table;

/* History */
    History	*bgn_hist;
    History	*end_hist;
    History	*add_hist;
    History	*cur_hist;
    History	*save_hist;
    short	w_holding;
    short	hist_cunt;

    short	c_bnst_;	/* current bunsetsu */

/* maximum values */
    short	m_length;	/* */
    short	m_chg;		/* */
    short	m_history;
    short	m_bunsetsu;

/* cursor attribute */
    short	c_colum;		/* current column of cursor	*/
    Bool	c_reverse;		/* flag for reverse		*/
    Bool	c_underline;		/* flag for under list		*/
    Bool	c_bold;			/* flag for bolding		*/
    Bool	c_invisible;		/* flag for invisible		*/

/* internal flags */
    short	h_mode;			/* mode of henkan		*/
    Bool	q_flag;			/* flag for quote		*/ 

    int		(*command_func_stack[MAX_FUNC_STACK])();
    int		func_stack_count;
    int		buffer_ignored;

    w_char	s_yomi[512];
    int		c_bnst_sv;

/* external flags */
    Bool	s_ascii_char;		 /* flag that send ascii characters
					    when the buffer is empty	*/
    Bool	s_ascii_char_def;
    Bool	s_ascii_stack;
    Bool	s_ascii_char_quote;
    Bool	e_delete;
    Bool	h_flag;			/* */
    Bool	h_on_kuten;		/* flag that convert when kuten	*/
    Bool	h_off_def;

    FunctionTable	f_table;
/* romkan */
    Romkan	*rk;
    struct _WnnClientRec	*next;
} WnnClientRec;

#define	BUFFER_IN_CONT	-2

/*  buffers */
#define username		(c_c->user_name)
#define def_servername		(c_c->host_name)
#define def_reverse_servername	(c_c->rev_host_name)

#define	bun_data_		(c_c->b_data_)
#define	c_b			(c_c->c_buf)

#define	servername		(c_c->cur_normal_env->host_name)
#define	reverse_servername	(c_c->cur_reverse_env->host_name)

#define	env_normal		(c_c->cur_normal_env->env)
#define	env_reverse		(c_c->cur_reverse_env->env)

#define	normal_sticky		(c_c->cur_normal_env->sticky)
#define	reverse_sticky		(c_c->cur_reverse_env->sticky)

#define envrcname		(c_c->cur_normal_env->envrc_name)
#define reverse_envrcname	(c_c->cur_reverse_env->envrc_name)

#define env_name		(c_c->cur_normal_env->env_name_str)
#define reverse_env_name	(c_c->cur_reverse_env->env_name_str)

#define uumkeyname		(c_c->uumkey_name)
#define rkfilename		(c_c->rkfile_name)

#define bunsetsu		(c_c->b_setsu)
#define bunsetsuend		(c_c->b_setsuend)
#define bunsetsu_env		(c_c->b_setsuenv)
#define touroku_bnst		(c_c->t_bnst)
#define touroku_bnst_cnt	(c_c->t_bnst_cnt)

#define kill_buffer		(c_c->k_buffer)
#define kill_buffer_offset	(c_c->k_buffer_offset)
#define remember_buf		(c_c->rem_buf)
#define	input_buffer		(c_c->i_buffer)
#define	return_buf		(c_c->ret_buf)

#define	touroku_comment		(c_c->t_comment)

#define	main_table		(c_c->m_table->m_table)

#define	romkan_clear_tbl	(c_c->m_table->rk_cl_tbl)

#define	beginning_of_history	(c_c->bgn_hist)
#define	end_of_history		(c_c->end_hist)
#define	add_history		(c_c->add_hist)
#define	current_history		(c_c->cur_hist)
#define	wchar_holding		(c_c->w_holding)
#define	history_cunt		(c_c->hist_cunt)

#define	cur_bnst_		(c_c->c_bnst_)

#define	maxlength		(c_c->m_length)
#define	maxchg			(c_c->m_chg)
#define max_history		(c_c->m_history)
#define maxbunsetsu		(c_c->m_bunsetsu)

#define	cursor_colum		(c_c->c_colum)
#define	cursor_reverse		(c_c->c_reverse)
#define	cursor_underline	(c_c->c_underline)
#define	cursor_bold		(c_c->c_bold)
#define	cursor_invisible	(c_c->c_invisible)

#define	henkan_mode		(c_c->h_mode)
#define quote_flag		(c_c->q_flag)

#define ignored			(c_c->buffer_ignored)

#define	save_yomi		(c_c->s_yomi)
#define	cur_bnst_sv		(c_c->c_bnst_sv)

#define	send_ascii_char		(c_c->s_ascii_char)
#define	send_ascii_char_def	(c_c->s_ascii_char_def)
#define	send_ascii_stack	(c_c->s_ascii_stack)
#define	send_ascii_char_quote	(c_c->s_ascii_char_quote)
#define	excellent_delete	(c_c->e_delete)
#define	henkan_off_flag		(c_c->h_flag)
#define	henkan_on_kuten		(c_c->h_on_kuten)
#define	henkan_off_def		(c_c->h_off_def)

#define	print_out_func		(c_c->f_table.print_out_function)
#define	input_func		(c_c->f_table.input_function)
#define	call_t_redraw_move_func	(c_c->f_table.call_t_redraw_move_function)
#define	call_t_redraw_move_1_func \
    (c_c->f_table.call_t_redraw_move_1_function)
#define	call_t_redraw_move_2_func \
    (c_c->f_table.call_t_redraw_move_2_function)
#define	call_t_print_l_func	(c_c->f_table.call_t_print_l_function)
#define	redraw_when_chmsig_func	(c_c->f_table.redraw_when_chmsig_function)
#define	char_len_func		(c_c->f_table.char_len_function)
#define	char_q_len_func		(c_c->f_table.char_q_len_function)
#define	t_redraw_move_func	(c_c->f_table.t_redraw_move_function)
#define	t_print_l_func		(c_c->f_table.t_print_l_function)
#define	c_top_func		(c_c->f_table.c_top_function)
#define	c_end_func		(c_c->f_table.c_end_function)
#define	c_end_nobi_func		(c_c->f_table.c_end_nobi_function)

extern char	*alloc_and_copy();
extern char	*Malloc();
extern char	*Calloc();
extern char	*Realloc();
extern void	Free();
#endif /* !XJUTIL */
