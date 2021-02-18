/*
 * $Id: sheader.h,v 1.2 1991/09/16 21:33:57 ohm Exp $
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
/**************************
 * header of standard i/o 
 **************************/

#include "jslib.h"

typedef struct _WnnEnv {
    char                *host_name;     /* server name */
    struct wnn_env      *env;           /* env */
    int                 sticky;         /* sticky bit */
    char                *envrc_name;    /* envrc name */
    char                env_name_str[32];       /* env name */
    struct _WnnEnv      *next;
} WnnEnv;

typedef struct _FunctionTable {
/* functions depends on lang */
    int         (*print_out_function)();
    int         (*input_function)();
    int         (*call_t_redraw_move_function)();
    int         (*call_t_redraw_move_1_function)();
    int         (*call_t_redraw_move_2_function)();
    int         (*call_t_print_l_function)();
    int         (*redraw_when_chmsig_function)();
    int         (*char_len_function)();
    int         (*char_q_len_function)();
    int         (*t_redraw_move_function)();
    int         (*t_print_l_function)();
    int         (*c_top_function)();
    int         (*c_end_function)();
    int         (*c_end_nobi_function)();
    int         (*call_redraw_line_function)();
    int         (*hani_settei_function)();
} FunctionTable;

typedef struct _FuncDadaBase {
        char *lang;
        FunctionTable f_table;
} FuncDataBase;


extern int 	not_redraw; /* c_b->bufferを用いていない時(エラーメッセージを
表示しているなど)に、リドローがかかってもリドローしないためのフラグ */
extern int 	maxchg;  /*一度に変換できる文字数 */

extern int	maxlength; /* 画面の横幅を表す */
extern w_char   *input_buffer; /* 画面制御に使うバッファ */

extern struct wnn_buf *bun_data_;

extern WnnEnv *normal_env;
extern WnnEnv *reverse_env;
extern WnnEnv *cur_normal_env;
extern WnnEnv *cur_reverse_env;

extern int 	cur_bnst_ ;		/* current bunsetsu pointer */

/*extern  int 	b_suu_;		*/	/* hold bunsetsu suu */
				/* Use jl_bun_suu(bun_data_) */

extern char romkan_clear_tbl[TBL_CNT][TBL_SIZE];
extern int (*main_table[TBL_CNT][TBL_SIZE])();	/* holding commands */

/*  extern w_char *p_holder; */	/* points the end of data in buffer. */

extern w_char *knj_buffer; /* 辞書ユーティリティー使用時の漢字バッファ */


extern int crow;
	/* holding row where i/f uses to display kanji line */

extern w_char *return_buf;
	/* 確定した漢字かな混じり文を返すためのバッファ */


extern char	Term_Name[];

extern int rubout_code; /* rubout に使われるコードを保持する */
extern int kk_on; /* 仮名漢字変換可能モードか否かを示すフラグ */
extern int quote_code; 
extern int quote_flag;

extern int max_history;
extern 	w_char jishopath[];
extern 	w_char hindopath[];
extern w_char fuzokugopath[];


extern short tty_c_flag;
extern short pty_c_flag;

extern int cursor_invisible_fun; /* flag that cursor_invisible is in the termcap entry */
extern int send_ascii_char; 	/* flag that if send ascii characters when the buffer is empty */
extern int excellent_delete;
extern int (*keyin_when_henkan_off)();

extern char lang_dir[];

extern char uumkey_name_in_uumrc[];
extern char convkey_name_in_uumrc[];
extern char rkfile_name_in_uumrc[];
extern short defined_by_option;

extern int conv_lines;
extern int flow_control ;
extern int henkan_off_flag;	/* 立ち上げ時に変換をon/offにした状態にします */
extern int henkan_on_kuten;	/* 句点入力で変換する/しない */

extern char def_servername[]; /* V3.0 */
extern char def_reverse_servername[];
extern char username[]; /* V3.0 */
extern char user_dic_dir[]; /* V3.0 */

extern int remove_cs_from_termcap;

extern int disp_mode_length; /* V3.0 Alternate for MHL */


extern int kanji_buf_size;
extern int maxbunsetsu;
extern int max_ichiran_kosu;

/*
#define MAXKUGIRI 32
extern w_char kugiri_str[];
*/

extern WNN_DIC_INFO *dicinfo;
extern int dic_list_size;

extern int touroku_comment;

extern struct msg_cat *cd;

extern FunctionTable *f_table;

extern FuncDataBase function_db[];

extern FunctionTable default_func_table;

/* extern function defines */
#include "rk_spclval.h"
#include "rk_fundecl.h"
#include "wnn_string.h"

extern void b_s_off_raw();
extern void b_s_on_raw();
extern int backward();
extern int buffer_in();
extern int change_ascii_to_int();
extern int change_mode();
extern void change_to_empty_mode();
extern void change_to_insert_mode();
extern void chartowchar();
extern void clr_end_screen();
extern void clr_line();
extern void clr_line_all();
extern int connect_jserver();
extern int convert_getterm();
extern int convert_key_setup();
extern int cur_ichi();
extern void cursor_invisible_raw();
extern void cursor_normal_raw();
extern int dai_end();
extern int dai_top();
extern int dic_nickname();
extern int disconnect_jserver();
extern int disp_mode();
extern void display_henkan_off_mode();
extern int dup();
extern int empty_modep();
extern int endttyent();
extern void epilogue();
extern void epilogue_no_close();
extern void err();
extern void errorkeyin();
extern int eu_columlen();
extern int eujis_to_iujis();
extern int expand_argument();
extern int expand_expr();
extern void fill();
extern int find_dic_by_no();
extern int find_end_of_tango();
extern int find_entry();
extern void flushw_buf();
extern int forward();
extern int getTermData();
extern void get_end_of_history();
extern int getdtablesize();
extern void getfname();
extern void h_r_off();
extern void h_r_off_raw();
extern void h_r_on();
extern void h_r_on_raw();
extern int henkan_gop();
extern void henkan_if_maru();
extern int henkan_off();
extern int hextodec();
extern int hinsi_in();
extern int init_history();
extern int init_key_table();
extern void init_screen();
extern int init_uum();
extern int initial_message_out();
extern void initialize_vars();
extern int input_a_char_from_function();
extern int insert_char();
extern int insert_char_and_change_to_insert_mode();
extern int insert_modep();
extern int j_term_init();
extern int jis_flush();
extern int jtosj();
extern int jutil();
extern int kakutei();
extern int kana_in();
extern int kana_in_w_char_msg();
extern int keyin1();
extern int kk();
extern void kk_cursor_invisible();
extern void kk_cursor_normal();
extern void kk_restore_cursor();
extern void kk_save_cursor();
extern int make_history();
extern int make_info_out();
extern int make_jikouho_retu();
extern void make_kanji_buffer();
extern int make_string_for_ke();
extern int next_history1();
extern int nobasi_tijimi_mode();
extern int nobi_conv();
extern void pop_cursor();
extern void pop_hrus();
extern int previous_history1();
extern void print_buf_msg();
extern void printf();
extern void reset_cursor();
extern void push_cursor();
extern void push_hrus();
extern void putchar1();
extern void putchar_norm();
extern void puteustring();
extern int reconnect_jserver_body();
extern int redraw_line();
extern int redraw_nisemono();
extern void remove_key_bind();
extern int ren_henkan0();
extern void reset_bold();
extern void reset_cursor_status();
extern int resetutmp();
extern void restore_cursor_raw();
extern void ring_bell();
extern void save_cursor_raw();
extern int saveutmp();
extern void scroll_up();
extern int select();
extern int select_jikouho1();
extern int select_line_element();
extern int select_one_dict1();
extern int select_one_element();
extern void set_TERMCAP();
extern void set_bold();
extern void set_cursor_status();
extern void set_escape_code();
extern void set_hanten_ul();
extern void set_lc_offset();
extern void set_screen_vars_default();
extern void set_scroll_region();
extern void setenv();
extern int setutmp();
extern int st_colum();
extern void t_cont_line_note_delete();
extern int t_delete_char();
extern int t_kill();
extern int t_move();
extern int t_print_l();
extern void t_print_line();
extern int t_redraw_move();
extern int t_rubout();
extern void t_throw();
extern int t_yank();
extern int tan_conv();
extern int tan_henkan1();
extern int tgoto();
extern int through();
extern void throw_col();
extern void throw_cur_raw();
extern void touroku();
extern void u_s_off();
extern void u_s_off_raw();
extern void u_s_on();
extern void u_s_on_raw();
extern int update_dic_list();
extern int uumrc_get_entries();
extern void w_printf();
extern int w_putchar();
extern void w_sttost();
extern int wchartochar();
extern int yes_or_no();
extern int yes_or_no_or_newline();
extern int zenkouho_dai_c();
extern void find_yomi_for_kanji();
extern int check_vst();
extern void t_redraw_one_line();
extern void throw();
extern int keyin();
extern int push_unget_buf();
extern unsigned int * get_unget_buf();
extern int if_unget_buf();

extern int set_cur_env();
extern char env_state();
extern void get_new_env();

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
extern int call_redraw_line_normal();
extern int call_redraw_line_yincod();
extern int call_redraw_line();
extern int hani_settei_normal();
extern int hani_settei_yincod();

extern int cwnn_pzy_yincod();
extern int cwnn_yincod_pzy_str();
