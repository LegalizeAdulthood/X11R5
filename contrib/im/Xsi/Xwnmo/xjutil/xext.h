/*
 * $Id: xext.h,v 1.3 1991/09/16 21:35:30 ohm Exp $
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
 * basic_op.c
 */
extern int redraw_nisemono();
extern int buffer_in();
extern int kana_in();
extern int kana_in_w_char_msg();
extern int delete_char1();
extern int t_delete_char();
extern int t_rubout();
extern int t_kill();
#ifdef	CONVERT_by_STROKE
extern int errort_kill();
#endif	/*  CONVERT_by_STROKE */
extern int t_yank();
extern int t_ret();
extern int t_quit();
extern int c_top();
extern int c_end();
extern int c_end_nobi();
extern int t_jmp_backward();
extern int t_jmp_forward();
extern int forward();
extern int backward();
extern int insert_char();
extern int insert_string();
extern int quote();
extern void redraw_line();

/*
 * box.c
 */
extern BoxRec * create_box();
extern int remove_box();
extern void map_box();
extern void unmap_box();
extern void freeze_box();
extern void unfreeze_box();
extern void moveresize_box();
extern void changecolor_box();
extern void redraw_box();
extern void reverse_box();

/*
 * cursor.c
 */
extern void throw_col();
extern void h_r_on();
extern void h_r_off();
extern void u_s_on();
extern void u_s_off();
extern void kk_cursor_invisible();
extern void kk_cursor_normal();
extern void reset_cursor_status();
extern void set_cursor_status();
extern void clr_line_all();
extern void push_hrus();
extern void pop_hrus();
extern void set_hanten_ul();
extern void set_bold();
extern void reset_bold();

/*
 * display.c
 */
extern void JWmark_cursor();
extern void JW1Mputwc();
extern void JWMflushw_buf();
extern void jw_disp_mode();
extern void JWMline_clear();
extern void JWcursor_visible();
extern void JWcursor_invisible();
extern void JWcursor_move();
extern void redraw_text();
extern void redraw_note();
extern void check_scroll();

/*
 * functions.c
 */
extern int call_t_redraw_move_normal();
extern int call_t_redraw_move_yincod();
extern int call_t_redraw_move();
extern int call_t_redraw_move_1_normal();
extern int call_t_redraw_move_1_yincod();
extern int call_t_redraw_move_1();
extern int call_t_redraw_move_2_normal();
extern int call_t_redraw_move_2_yincod();
extern int call_t_redraw_move_2();
extern int call_t_print_l_normal();
extern int call_t_print_l_yincod();
extern int call_t_print_l();
extern int input_yincod();
extern int redraw_when_chmsig_yincod();
extern int c_top_normal();
extern int c_top_yincod();
extern int c_end_normal();
extern int c_end_yincod();
extern int c_end_nobi_normal();
extern int c_end_nobi_yincod();
extern int print_out_yincod();
extern int char_q_len_normal();
extern int char_q_len_yincod();
extern int char_len_normal();
extern int char_len_yincod();
extern int t_redraw_move_normal();
extern int t_redraw_move_yincod();
extern int t_print_l_normal();
extern int t_print_l_yincod();

/*
 * hinsi.c
 */
extern int hinsi_in();

/*
 * ichiran.c
 */
extern void decide_position();
extern void resize_text();
extern void xw_mousemove();
extern void init_ichiran();
extern void lock_ichiran();
extern void end_ichiran();
extern int init_input();
extern void end_input();
extern void init_keytable();
extern void end_keytable();
extern void init_yes_or_no();
extern void end_yes_or_no();
extern void JW3Mputc();
extern int next_ichi();
extern int back_ichi();
extern int find_hilite();
extern int xw_next_inspect();
extern int xw_back_inspect();
extern int xw_next_select();
extern int xw_previous_select();
extern void xw_expose();
extern Status xw_buttonpress();
extern int jutil_mode_set();
extern void xw_fillrec();
extern void xw_enterleave();

/*
 * init_w.c
 */
extern char env_state();
extern int set_cur_env();
extern void get_new_env();
extern void return_error();
extern void read_wm_id();
extern int create_text();
extern int create_jutil();
extern int create_ichi();
extern int create_yes_no();
extern void xw_end();
extern XjutilFSRec * add_fontset_list();
extern XjutilFSRec * get_fontset_list();
extern int create_xjutil();

/*
 * jhlp.c
 */
extern int keyin();
extern void terminate_handler();
extern void main();

/*
 * jutil.c
 */
extern void if_dead_disconnect();
extern int yes_or_no();
extern void  paramchg();
extern int  update_dic_list();
extern void  dic_nickname();
extern int  find_dic_by_no();
extern void dicinfoout();
extern void select_one_dict9();
extern void fuzoku_set();
extern void jishoadd();
extern int dic_delete_e();

/*
 * kensaku.c
 */
extern void kensaku();

/*
 * key_bind.c
 */
extern int init_key_table();

/*
 * keyin.c
 */
extern int get_env();
extern int get_touroku_data();
extern int xw_read();

/*
 * localalloc.c
 */
extern char * Malloc();
extern char * Realloc();
extern char * Calloc();
extern void Free();
extern char * alloc_and_copy();

/*
 * printf.c
 */
extern int char_q_len();
extern void put_char();
extern void flushw_buf();
extern int w_putchar();
extern void put_char1();
extern void putchar_norm();
extern void errorkeyin();
#ifdef	CONVERT_by_STROKE
void errorkeyin_q();
#endif	/* CONVERT_by_STROKE */
extern void malloc_error();
extern void print_out7();
extern void print_out3();
extern void print_out2();
extern void print_out1();
extern void print_out();
extern void print_msg_getc();
extern void print_msg_wait();

/*
 * prologue.c
 */
extern int open_romkan();
extern int init_wnn();
extern int connect_server();
extern int init_xcvtkey();

/*
 * screen.c
 */
extern void throw();
extern int char_len();
extern int cur_ichi();
extern void t_print_line();
extern void t_redraw_one_line();
extern void init_screen();
extern void t_redraw_move();
extern void t_move();
extern void t_print_l();
extern char *get_rk_modes();
extern void disp_mode();
extern void display_henkan_off_mode();
extern int char_len_normal();

/*
 * select_ele.c
 */
extern int xw_select_one_element();
extern int select_select();
extern int quit_select();
extern int kdicdel();
extern int kdicuse();
extern int kdiccom();
extern int kworddel();
extern int kworduse();
extern int kwordcom();

/*
 * termio.c
 */
extern void clr_end_screen();
extern void throw_cur_raw();
extern void h_r_on_raw();
extern void h_r_off_raw();
extern void u_s_on_raw();
extern void u_s_off_raw();
extern void b_s_on_raw();
extern void b_s_off_raw();
extern void ring_bell();
extern void cursor_invisible_raw();
extern void cursor_normal_raw();

/*
 * touroku.c
 */
extern void touroku();
extern int t_markset();

/*
 * uif.c
 */
extern int empty_modep();
extern int insert_modep();
extern int redraw_nisemono_c();
extern int isconect_jserver();
extern int kill_c();
#ifdef	CONVERT_by_STROKE
extern int errorkill_c();
#endif	/* CONVERT_by_STROKE */
extern int delete_c();
extern int rubout_c();
extern int backward_c();
extern int reconnect_server();
extern int disconnect_server();
extern int henkan_off();
extern int reset_c_b();

/*
 * uif1.c
 */
extern int jutil_c();
extern int touroku_c();
extern int reconnect_jserver_body();
extern int lang_c();
extern int push_unget_buf();
extern int * get_unget_buf();
extern int if_unget_buf();

/*
 * w_string.c
 */
extern void chartowchar();
extern int wchartochar();
extern void w_sttost();
extern void w_sttostn();
extern int eu_columlen();

/*
 * wnnrc_op.c
 */
extern int expand_expr();
extern void uumrc_get_entries();

/*
 * xcvtkey.c
 */
extern int comment_char();
extern int cvt_fun_setup();
extern int cvt_meta_setup();
extern int cvtfun();
extern int cvtmeta();

/*
 * xlc_util.c
 */
extern int XwcGetColumn();
extern int check_mb();
extern int w_char_to_char();
extern int skip_pending_wchar();
extern int char_to_wchar();
extern int w_char_to_wchar();
extern int w_char_to_ct();
extern void JWOutput();
extern XCharStruct * get_base_char();
extern XFontSet create_font_set();

/*
 * xw_touroku.c
 */
extern void xw_jutil_write_msg();
extern void init_jutil();
extern void end_jutil();
extern void change_cur_jutil();
extern void hanten_jutil_mes_title();
extern void change_mes_title();

/*
 * For etc
 */
#include "wnn_string.h"
/*
extern int sStrcpy();
extern int Sstrcpy();
extern char * sStrncpy();
extern w_char * Strcat();
extern w_char * Strncat();
extern int Strcmp();
extern int Strncmp();
extern w_char * Strcpy();
extern w_char * Strncpy();
extern int Strlen();
extern int Strlen();
*/

/*
 * Standard functions
 */
extern int fclose();
extern int fflush();
extern int atoi();
extern void bcopy();

/*
 * Wnn jslib functions
 */
extern int js_hinsi_list();
extern int js_hinsi_number();
extern int js_dic_list();
extern int js_fuzokugo_get();
extern int js_file_send();
extern int js_file_read();
extern int js_fuzokugo_set();
extern int js_access();
extern int js_mkdir();
extern int js_hindo_file_create_client();
extern int js_hindo_file_create();
extern int js_dic_file_create_client();
extern int js_dic_file_create();
extern int js_file_remove_client();
extern int js_file_remove();
extern int js_file_discard();
extern int js_dic_add();
extern int js_dic_info();
extern int js_dic_delete();
extern int js_word_search_by_env();
extern int js_hinsi_name();
extern int js_dic_use();
extern int js_file_info();
extern int js_file_comment_set();
extern int js_word_delete();
extern int js_word_info();
extern int js_hindo_set();
extern int js_word_comment_set();
extern int js_word_add();
extern int js_isconnect();
extern char * wnn_perror_lang();

/*
 * yincoding functions
 */
extern int cwnn_pzy_yincod();
extern int cwnn_yincod_pzy_str();
