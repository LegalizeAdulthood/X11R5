/*
 * $Id: ext.h,v 1.5 1991/09/16 21:36:47 ohm Exp $
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
extern int t_rubout();
extern int delete_char1();
extern int t_delete_char();
extern int kuten();
extern int jis();
extern int input_a_char_from_function();
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
extern int reset_line();
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
 * change.c
 */
extern void reset_preedit();
extern void reset_status();
extern unsigned long update_ic();
extern unsigned long update_spotlocation();
extern int change_current_language();
extern void change_client_area();
extern void change_focus_area();
extern int change_client();
#ifdef	SPOT
extern int change_spotlocation();
#endif	/* SPOT */

/*
 * client.c
 */
extern void set_function_table();
extern int new_client();
extern void del_client();
extern void epilogue();
extern char env_state();
extern int set_cur_env();
extern void get_new_env();

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
extern void redraw_window3();
extern void redraw_lines();
extern void redraw_note();
extern void redraw_window0();
extern void redraw_xj_all();
extern void visual_window1();
extern void visual_window();
extern void invisual_window1();
extern void invisual_window();
extern void JWMflushw_buf();
extern void jw_disp_mode();
extern void JWMline_clear();
extern void JWMline_clear1();
extern void visual_status();
extern void invisual_status();
extern void JWcursor_visible();
extern void JWcursor_invisible();
extern void JWcursor_move();
extern void check_scroll();

/*
 * do_socket.c
 */
extern int _Send_Flush();
extern int _WriteToClient();
extern int _ReadFromClient();
extern short init_net();
extern void close_net();
extern int wait_for_socket();
extern void close_socket();
extern int get_cur_sock();
extern int get_rest_len();
extern int read_requestheader();
extern int read_strings();

/*
 * do_xjutil.c
 */
#ifdef	USING_XJUTIL
extern void xjutil_start();
extern void kill_xjutil();
extern void kill_lang_xjutil();
extern void kill_all_xjutil();
extern void set_xjutil_id();
extern void reset_xjutil_fs_id();
extern int isstart_xjutil();
extern void xw_jishoadd();
extern void xw_kensaku();
extern void xw_select_one_dict9();
extern void xw_paramchg();
extern void xw_dicinfoout();
extern void xw_fuzoku_set();
extern void xw_touroku();
#endif	/* USING_XJUTIL */

/*
 * ev_dispatch.c
 */
extern void X_flush();
extern void XEventDispatch();
extern int get_xevent();

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
 * history.c
 */
extern int init_history();
extern int make_history();
extern void get_end_of_history();
extern int previous_history1();
extern int next_history1();
extern void destroy_history();

/*
 * ichiran.c
 */
extern void JW3Mputc();
extern int init_ichiran();
extern void end_ichiran();
extern void check_map();
extern void lock_inspect();
extern void unlock_inspect();
extern int init_inspect();
extern void end_inspect();
extern int xw_next_inspect();
extern int xw_back_inspect();
extern void xw_move_hilite();
extern void xw_jikouho_move_hilite();
extern void alloc_for_save_buf();
extern void redraw_ichi_w();
extern int next_ichi();
extern int back_ichi();
extern int find_hilite();
extern int find_ji_hilite();
extern void xw_forward_select();
extern void xw_backward_select();
extern void xw_next_select();
extern void xw_previous_select();
extern void xw_linestart_select();
extern void xw_lineend_select();
extern int init_yes_or_no();
extern void end_yes_or_no();
extern int set_j_c();
extern int cur_cl_change3();
extern int cur_cl_change4();
extern void xw_select_button();
extern void xw_select_jikouho_button();
extern void xw_mouseleave();
extern int xw_mouse_select();
extern void draw_nyuu_w();
extern void clear_nyuu_w();
extern void nyuu_w_cursor();
extern int yes_or_no();
extern int yes_or_no_or_newline();

/*
 * init.c
 */
extern void add_ximclientlist();
extern void remove_ximclientlist();
extern void add_inputlist();
extern void remove_inputlist();
extern int allocate_wnn();
extern void free_wnn();
extern int initialize_wnn();

/*
 * init_w.c
 */
extern int get_application_resources();
extern void create_yes_no();
extern void read_wm_id();
extern int create_xim();
extern void xw_end();
extern int create_preedit();
extern int create_status();

/*
 * inspect.c
 */
extern int inspect();
extern int inspect_kouho();
extern int sakujo_kouho();
extern int inspectdel();
extern int inspectuse();
extern unsigned char * next_inspect();
extern unsigned char * previous_inspect();

/*
 * jhlp.c
 */
extern void do_end();
extern void in_put();
extern void main();

/*
 * jikouho.c
 */
extern int dai_top();
extern int dai_end();
extern void set_escape_code();
extern int jikouho_c();
extern int zenkouho_c();
extern int jikouho_dai_c();
extern int zenkouho_dai_c();
extern int select_jikouho();
extern int select_jikouho_dai();
#ifdef	CONVERT_by_STROKE
extern int select_question();
#endif	/* CONVERT_by_STROKE */

/*
 * jis_in.c
 */
extern int in_jis();

/*
 * jutil.c
 */
extern int jutil();
extern int dicsv();
#ifdef  USING_XJUTIL
extern int  paramchg();
extern int dicinfoout();
extern int select_one_dict9();
extern int fuzoku_set();
extern int jishoadd();
extern int kensaku();
#endif  /* USING_XJUTIL */

/*
 * key_bind.c
 */
extern int init_key_table();

/*
 * keyin.c
 */
extern int key_input();
int ifempty();
extern int RequestDispatch();
/*
 * kuten.c
 */
extern int in_kuten();

/*
 * localalloc.c
 */
extern char * Malloc();
extern char * Realloc();
extern char * Calloc();
extern void Free();
extern char * alloc_and_copy();

/*
 * multi_lang.c
 */
extern char *get_default_font_name();
extern int load_font_set();
extern void add_locale_to_xl();
extern void remove_locale_to_xl();
extern int add_lang_env();
extern void default_xc_set();
extern XIMClientRec * create_client();
extern int lang_set();
extern int lang_set_ct();
extern void change_lang();

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
extern int allocate_areas();
extern void free_areas();
extern int connect_server();
extern int init_xcvtkey();

/*
 * readximrc.c
 */
int read_ximrc();

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

/*
 * select_ele.c
 */
extern int xw_select_one_element();
extern int xw_select_one_jikouho();
extern int forward_select();
extern int backward_select();
extern int lineend_select();
extern int linestart_select();
extern int select_select();
extern int quit_select();
extern int previous_select();
extern int next_select();
extern int redraw_select();

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
extern int touroku();

/*
 * uif.c
 */
extern void push_func();
extern void pop_func();
extern int insert_char_and_change_to_insert_mode();
extern int kakutei();
extern int return_it();
extern int return_it_if_ascii();
extern int redraw_nisemono_c();
extern int kk();
extern int insert_modep();
extern int empty_modep();
extern void clear_c_b();
extern void make_kanji_buffer();
extern int isconect_jserver();
extern int ren_henkan();
extern int kankana_ren_henkan();
#ifdef	CONVERT_by_STROKE
extern int errorkaijo();
#endif	/* CONVERT_by_STROKE */
extern int tan_henkan();
extern int tan_henkan_dai();
extern int nobi_henkan();
extern int nobi_henkan_dai();
extern void henkan_if_maru();
#ifdef	CONVERT_by_STROKE
extern int question_henkan();
#endif	/* CONVERT_by_STROKE */
extern int yank_c();
extern int remember_me();
extern int kill_c();
#ifdef	CONVERT_by_STROKE
extern int errorkill_c();
#endif	/* CONVERT_by_STROKE */
extern int delete_c();
extern int rubout_c();
extern int end_bunsetsu();
extern int top_bunsetsu();
extern int forward_bunsetsu();
extern int backward_bunsetsu();
extern int kaijo();
extern int enlarge_bunsetsu();
extern int smallen_bunsetsu();
extern int send_string();
extern int tijime();
extern int jmptijime();
extern int henkan_forward();
extern int henkan_backward();
extern int backward_c();
extern int insert_it_as_yomi();
extern int previous_history();
extern int next_history();
extern int send_ascii();
extern int not_send_ascii();
extern int toggle_send_ascii();
extern int pop_send_ascii();
extern int send_ascii_e();
extern int not_send_ascii_e();
extern int toggle_send_ascii_e();
extern int pop_send_ascii_e();
extern int quote_send_ascii_e();
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
extern unsigned int * get_unget_buf();
extern int if_unget_buf();

/*
 * util.c
 */
extern void GetIM();
extern void CreateIC();
extern void GetIC();
extern void SetICFocus();
extern void UnsetICFocus();
extern void ChangeIC();
extern void DestroyIC();
extern void destroy_for_sock();
extern void destroy_client();
extern void ResetIC();
extern void free_langlist();
extern XIMNestLangList get_langlist();
#ifdef	SPOT
extern void ChangeSpot();
#endif	/* SPOT */

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
extern int uumrc_get_entries();
extern int read_default_rk();

/*
 * write.c
 */
extern int return_eventreply();
extern int send_nofilter();
extern int send_end();
extern int return_cl_it();
extern void xw_write();

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
extern int get_columns_wchar();
extern int XwcGetColumn();
extern int check_mb();
extern int w_char_to_char();
extern int skip_pending_wchar();
extern int put_pending_wchar_and_flg();
extern int char_to_wchar();
extern int w_char_to_wchar();
extern int w_char_to_ct();
extern void JWOutput();
extern XCharStruct * get_base_char();
extern XFontSet create_font_set();
#ifdef	CALLBACKS
extern int XwcGetChars();

/*
 * For callback.c
 */
extern void CBStatusDraw();
extern void CBStatusStart();
extern void CBStatusDone();
extern void CBPreeditDraw();
extern void CBPreeditStart();
extern void CBPreeditDone();
extern void CBPreeditRedraw();
extern void CBPreeditClear();
extern void CBCursorMove();
extern void CBCursorMark();
extern void SendCBRedraw();
#endif	/* CALLBACKS */

/*
 * For etc
 */
#include "wnn_string.h"
/*
extern int sStrcpy();
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
 * Wnn jllib functions
 */
extern int jl_isconnect_e();
extern int jl_dic_save_all_e();
extern void jl_close();
extern int jl_dic_list_e();
extern int wnn_get_area();
extern int jl_kanji_len();
extern int jl_yomi_len();
extern int jl_word_delete_e();
extern int jl_word_use_e();
extern int jl_zenkouho();
extern int jl_set_jikouho();
extern void jl_env_set();
extern int jl_zenkouho_dai();
extern int jl_set_jikouho_dai();
extern void jl_get_zenkouho_kanji();
extern void jl_get_zenkouho_yomi();
extern int jl_zenkouho_q();
extern int js_env_sticky();
extern int jl_kill();
extern int jl_update_hindo();
extern int jl_ren_conv();
extern int jl_tan_conv();
extern int jl_nobi_conv();
extern int jl_nobi_conv_e2();
extern int jl_q_conv();
extern void jl_disconnect();

/*
 * yincoding functions
  */
extern int cwnn_pzy_yincod();
extern int cwnn_yincod_pzy_str();

#ifdef	XJPLIB
extern XIMClientRec *XJp_cur_cl_set();
#endif	/* XJPLIB */
