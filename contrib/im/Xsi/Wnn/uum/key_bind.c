/*
 * $Id: key_bind.c,v 1.2 1991/09/16 21:33:45 ohm Exp $
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
    0: 変換後、変換結果を修正しているモード
    1: 変換前の文字列を入力しているモード
    2: 変換後、文節の区切りを伸ばしたり縮めたりしているモード
    3: バッファに文字が何も入っていないモード
    4: カーソルを動かしながら、何か候補を選択する時のモード
    5: 単語登録時に、漢字の範囲を設定している時のモード
    6: 単語登録時の読み入力などの文字列を入力している時
    7: 単語削除、辞書削除などのモード
    8: 単語検索のモード
    9: 辞書検索のモード
    10:インスペクトのモード
*/    

#include <stdio.h>
#include "commonhd.h"
#include "config.h"
#include "sdefine.h"
#include "sheader.h"
#define MADA NULL

extern insert_it_as_yomi();
extern select_jikouho();
extern select_jikouho_dai();
extern jikouho_c();
extern zenkouho_c();
extern jikouho_dai_c();
extern zenkouho_dai_c();
extern int enlarge_bunsetsu();
extern int smallen_bunsetsu();
extern int top_bunsetsu();
extern int end_bunsetsu();
extern int ren_henkan();
extern int tan_henkan();
extern int tan_henkan_dai();
extern int nobi_henkan();
extern int nobi_henkan_dai();
#ifdef nodef
#ifdef	CONVERT_by_STROKE
extern int question_henkan();
extern int select_question();
#endif
#endif
extern int send_string();
extern int t_yank();
extern int yank_c();
extern int kakutei();
extern int remember_me();
extern int reset_line();
extern int touroku_c();
extern int forward();
extern int backward();
extern int c_top();
extern int c_end();
extern int c_end_nobi();
extern int t_quit();
extern int t_delete_char();
extern int delete_c();
extern int t_rubout();
extern int rubout_c();
extern int t_markset();
extern int kuten();
extern int t_kill();
extern int kill_c();
extern int kaijo();
extern int t_next();
extern int t_previous();
extern int t_jmp_forward();
extern int t_jmp_backward();
extern int t_ret();
extern int previous_history();
extern int next_history();
extern int forward_bunsetsu();
extern int backward_bunsetsu();
extern int jutil_c();
extern int jis();
extern int input_from_kk();
extern int tijime();
extern int jmptijime();
extern int henkan_forward();
extern int henkan_backward();
extern int backward_c();
extern int quote();
extern int send_ascii();
extern int not_send_ascii();
extern int toggle_send_ascii();
extern int send_ascii_e();
extern int not_send_ascii_e();
extern int toggle_send_ascii_e();
extern int pop_send_ascii();
extern int pop_send_ascii_e();
extern int quote_send_ascii_e();
extern int reconnect_jserver();

extern int select_select();
extern int quit_select();
extern int lineend_select();
extern int linestart_select();
extern int redraw_select();
extern int previous_select();
extern int next_select();
extern int backward_select();
extern int forward_select();

extern int select_select_line();
extern int quit_select_line();
extern int lineend_select_line();
extern int linestart_select_line();
extern int redraw_select_line();
extern int previous_select_line();
extern int next_select_line();
extern int backward_select_line();
extern int forward_select_line();
extern int inspect_kouho();
extern sakujo_kouho();

extern int henkan_off();

extern int kdicdel();
extern int kdicuse();
extern int kdiccom();
extern int kworddel();
extern int kworduse();
extern int kwordcom();

extern int inspectdel();
extern int inspectuse();
extern int previous_inspect();
extern int next_inspect();

extern int kankana_ren_henkan();

static struct kansuu kansuu_hyo[] = {
{ "send_string" , "send what is in the buffer to pty." ,0,send_string, send_string, send_string, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "forward_char" ,"cursor wo hitotu maeni susumeru." , 1, forward_bunsetsu ,forward ,henkan_forward , NULL , NULL,forward ,forward , NULL, NULL, NULL, NULL},
{ "backward_char" ,"cursor wo hitotu usironi susumeru.", 1,backward_bunsetsu ,backward_c ,henkan_backward ,NULL,NULL,backward,backward,NULL, NULL, NULL, NULL},
{ "goto_top_of_line" ,"",1  ,top_bunsetsu ,c_top ,jmptijime ,NULL ,NULL ,c_top, c_top,NULL, NULL, NULL, NULL},
{ "goto_end_of_line" ,"",1  ,end_bunsetsu, c_end ,c_end_nobi ,NULL ,NULL ,c_end, c_end,NULL, NULL, NULL, NULL},
{ "delete_char_at_cursor" ,"",1 , NULL , delete_c,NULL , NULL , NULL , t_delete_char, t_delete_char, NULL, NULL, NULL, NULL},
{ "kaijo" ,"",0 , kaijo , NULL , kaijo , NULL , NULL  , NULL, NULL, NULL, NULL, NULL, NULL} ,
{ "henkan" ,"" ,1,  NULL ,ren_henkan , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "tan_henkan" ,"" ,1,  NULL ,tan_henkan , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "tan_henkan_dai" ,"" ,1,  NULL ,tan_henkan_dai , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
#ifdef nodef
#ifdef	CONVERT_by_STROKE
{ "qhenkan" ,"" ,1,  NULL ,question_henkan , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
#endif
#endif
{ "jikouho" ,"" ,0,  jikouho_c,NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "zenkouho" ,"" ,0,  zenkouho_c,NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "jikouho_dai" ,"" ,0,  jikouho_dai_c,NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "zenkouho_dai" ,"" ,0,  zenkouho_dai_c,NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "select_jikouho" ,"" ,0,  select_jikouho, NULL , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
#ifdef nodef
#ifdef	CONVERT_by_STROKE
{ "select_jikouho_q" ,"" ,0,  select_question, NULL , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
#endif
#endif
{ "select_jikouho_dai" ,"" ,0,  select_jikouho_dai, NULL , NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "kill" ,"" ,1, NULL ,  kill_c ,NULL , NULL , NULL , t_kill, t_kill, NULL, NULL, NULL, NULL},
{ "yank" ,"" ,1, NULL, yank_c ,NULL ,NULL ,NULL ,t_yank, t_yank,NULL, NULL, NULL, NULL},
{ "yank_e" ,"" ,1, NULL, yank_c ,NULL ,yank_c ,NULL ,t_yank, t_yank,NULL, NULL, NULL, NULL},
{ "bunsetu_nobasi" ,"" ,0,  enlarge_bunsetsu, NULL , forward, NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "bunsetu_chijime" ,"" ,0,  smallen_bunsetsu, NULL , tijime, NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "jisho_utility" ,"" ,0,  jutil_c,jutil_c, jutil_c ,NULL, NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "jisho_utility_e" ,"" ,0,  jutil_c,jutil_c, jutil_c ,jutil_c, NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "touroku" , "",1, touroku_c,touroku_c,touroku_c,NULL,NULL, NULL, NULL,NULL, NULL, NULL, NULL},
{ "touroku_e" , "",1, touroku_c,touroku_c,touroku_c,touroku_c,NULL, NULL, NULL,NULL, NULL, NULL, NULL},
{ "kakutei" ,"" ,1,  kakutei, kakutei, kakutei , NULL , NULL ,NULL,NULL, NULL, NULL, NULL, NULL},
{ "sainyuuryoku" ,"" ,1,  NULL , remember_me, NULL , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "sainyuuryoku_e" ,"" ,1,  NULL , remember_me, NULL , remember_me , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "kuten" ,"" ,0,  NULL , kuten , NULL , NULL ,NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "kuten_e" ,"" ,0,  NULL , kuten , NULL , kuten ,NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "jis" ,"" ,0,  NULL , jis, NULL , NULL,NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "jis_e" ,"" ,0,  NULL , jis, NULL , jis,NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "redraw_line" ,"" ,1,  reset_line , reset_line , reset_line, NULL, redraw_select, reset_line, reset_line, redraw_select_line, redraw_select_line, redraw_select_line, redraw_select_line},
{ "redraw_line_e" ,"" ,1,  reset_line , reset_line , reset_line , reset_line , redraw_select , reset_line, reset_line, redraw_select_line, redraw_select_line, redraw_select_line, redraw_select_line},
{ "previous_history_e" ,"",1,  NULL, previous_history, NULL, previous_history, NULL,NULL,NULL, NULL, NULL, NULL, NULL},
{ "previous_history" ,"",1,  NULL, previous_history, NULL, NULL, NULL,NULL,NULL, NULL, NULL, NULL, NULL},
{ "next_history_e" ,"",1,  NULL, next_history, NULL, next_history, NULL,NULL,NULL, NULL, NULL, NULL, NULL},
{ "next_history" ,"",1,  NULL, next_history, NULL, NULL, NULL,NULL,NULL, NULL, NULL, NULL, NULL},
{ "touroku_mark_set" ,"" ,0,  NULL , NULL , NULL , NULL , NULL , t_markset,NULL, NULL, NULL, NULL, NULL},
{ "touroku_return" ,"" ,0,  NULL , NULL , NULL , NULL , NULL , NULL, t_ret, NULL, NULL, NULL, NULL},
{ "quit" ,"" ,0,  NULL , NULL , NULL , NULL , quit_select , t_quit, t_quit, quit_select_line, quit_select_line, quit_select_line, quit_select_line},
{ "touroku_jump_forward" ,"" ,0,  NULL , NULL , NULL , NULL , NULL , t_jmp_forward,NULL, NULL, NULL, NULL, NULL},
{ "touroku_jump_backward" ,"" ,0,  NULL , NULL , NULL , NULL , NULL , t_jmp_backward,NULL, NULL, NULL, NULL, NULL},
{"change_to_insert_mode", "",0, insert_it_as_yomi,NULL,NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "quote_e", "",0,  NULL, quote, NULL, quote, NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "quote", "",0,  NULL, quote, NULL, NULL, NULL,NULL,NULL, NULL, NULL, NULL, NULL},
{ "send_ascii_char","",0, send_ascii,send_ascii,send_ascii,send_ascii_e,send_ascii,send_ascii,send_ascii,send_ascii, send_ascii, send_ascii, send_ascii},
{ "toggle_send_ascii_char","",0, toggle_send_ascii,toggle_send_ascii,toggle_send_ascii,toggle_send_ascii_e,toggle_send_ascii,toggle_send_ascii,toggle_send_ascii,toggle_send_ascii, toggle_send_ascii, toggle_send_ascii, toggle_send_ascii},
{ "not_send_ascii_char","",0, not_send_ascii,not_send_ascii,not_send_ascii,not_send_ascii_e,not_send_ascii,not_send_ascii,not_send_ascii,not_send_ascii, not_send_ascii, not_send_ascii, not_send_ascii},
{ "pop_send_ascii_char","",0, pop_send_ascii,pop_send_ascii,pop_send_ascii,pop_send_ascii_e,pop_send_ascii,pop_send_ascii,pop_send_ascii,pop_send_ascii, pop_send_ascii, pop_send_ascii, pop_send_ascii},
{ "quote_send_ascii_char","",0, NULL,NULL,NULL,quote_send_ascii_e,NULL,NULL,NULL,NULL, NULL, NULL, NULL},
{ "select_select", "",0,  NULL, NULL, NULL, NULL, select_select, NULL, NULL, select_select_line, select_select_line, select_select_line, select_select_line},
{ "lineend_select", "",0,  NULL, NULL, NULL, NULL, lineend_select, NULL, NULL,lineend_select_line, lineend_select_line, lineend_select_line, lineend_select_line},
{ "linestart_select", "",0,  NULL, NULL, NULL, NULL, linestart_select, NULL, NULL, linestart_select_line, linestart_select_line, linestart_select_line, linestart_select_line},
{ "previous_select", "",0,  NULL, NULL, NULL, NULL, previous_select, NULL, NULL, previous_select_line, previous_select_line, previous_select_line, previous_inspect},
{ "next_select", "",0,  NULL, NULL, NULL, NULL, next_select, NULL, NULL, next_select_line, next_select_line, next_select_line, next_inspect},
{ "backward_select", "",0,  NULL, NULL, NULL, NULL, backward_select, NULL, NULL, backward_select_line, backward_select_line, backward_select_line, backward_select_line},
{ "forward_select", "",0,  NULL, NULL, NULL, NULL, forward_select, NULL, NULL, forward_select_line, forward_select_line, forward_select_line, forward_select_line},
{ "reconnect_jserver", "",0,  reconnect_jserver, reconnect_jserver, reconnect_jserver, NULL, reconnect_jserver,reconnect_jserver,reconnect_jserver, reconnect_jserver,  reconnect_jserver,  reconnect_jserver,  reconnect_jserver},
{ "henkan_on", "",0,  henkan_off, henkan_off, henkan_off, henkan_off, henkan_off,henkan_off,henkan_off, henkan_off, henkan_off, henkan_off, henkan_off},
{ "inspect","",0,  inspect_kouho, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "sakujo_kouho","",0,  sakujo_kouho, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
{ "nobi_henkan_dai" ,"" ,0,  NULL ,NULL , nobi_henkan_dai , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "nobi_henkan" ,"" ,0,  NULL ,NULL , nobi_henkan , NULL , NULL , NULL, NULL, NULL, NULL, NULL, NULL},
{ "del_entry","",0, NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL, kworddel, kdicdel, inspectdel},
{ "use_entry","",0,  NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL, kworduse, kdicuse, inspectuse},
{ "com_entry","",0,  NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL, kwordcom, kdiccom, NULL},
{ "kana_henkan" ,"" ,1,  NULL,kankana_ren_henkan, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
};

int kansuu_hyo_entry_suu = sizeof(kansuu_hyo) / sizeof(struct kansuu);

static char key_binded[sizeof(kansuu_hyo)/sizeof(struct kansuu) + 1];

int key_binding();

int
init_key_table()
{
 FILE *fp , *fopen(); char file_name[PATHNAMELEN];
 int j,k;

 for(k = 0 ; k < sizeof(kansuu_hyo)/sizeof(struct kansuu);k++){
   key_binded[k] = 0;
 }

   /* uumrcファイルから。*/
 if (uumkey_name_in_uumrc[0] != '\0') {
   strcpy(file_name, uumkey_name_in_uumrc);
 }else if(strcpy(file_name, LIBDIR),
	  strcat(file_name, "/"),
	  strcat(file_name, lang_dir),
	  strcat(file_name, CPFILE),
	  (fp = fopen(file_name , "r")) != NULL){
   /* default */
   fclose(fp);
 }else{
/*	fprintf(stderr, "Can't open default key bind file.%s\n" , CPFILE); */
/*   fprintf(stderr,"デフォルトのキーバインドが異常です。 %s\n",CPFILE); */
   fprintf(stderr, MSG_GET(29),CPFILE);
   return(-1);
 }

  for(j = 0 ; j < 3 ; j++){
/*
    for(k = 0; k < 32;k++){
      main_table[j][k] = null_fun;
    }
*/
    for(k = 0;k < TBL_SIZE;k++){
      main_table[j][k] = NULL;
    }
    main_table[j][NEWLINE] = (int (*)())0;
    main_table[j][CR] = (int (*)())0;
  }
  for(j = 3 ; j <TBL_CNT ; j++){
    for(k = 0;k < TBL_SIZE;k++){
      main_table[j][k] = NULL;
    }
  }
 if (key_binding(file_name) == -1)
   return(-1);
 return(0);
}


int
key_binding(file_name)
char file_name[64];
{                
  register int k,kk;
  FILE *fp , *fopen();
  char func_name[64];
  int com_cnt;
  int com_num ;
  char buf[256];
  char com_name[16][EXPAND_PATH_LENGTH];
  int lc = 0;
  int tbl;
  
  if(verbose_option) fprintf(stderr, "keybind: using keybind-file %s\r\n", file_name);
  if((fp = fopen(file_name , "r")) == NULL){
    printf("Key_binding file \"%s\" doesn't exist.\r\n",file_name);
    return(-1);
  }
  
  for(tbl = 0 ; tbl < TBL_CNT ; tbl++){
      romkan_clear_tbl[tbl][rubout_code] = 0; /* Not clear romkan when rubout */
  }
  main_table[1][rubout_code] = rubout_c;
  main_table[5][rubout_code] = t_rubout;
  main_table[6][rubout_code] = t_rubout;

  lc = 0;
  while(fgets(buf , 256 , fp)){
    lc += 1;
    if((*buf == ';') || (*buf == ':') || (*buf == '\n'))continue; /* for comment lines */
    if((com_cnt = sscanf(buf , "%s %s %s %s %s %s %s %s %s %s %s" , func_name , com_name[0],
	       com_name[1],com_name[2],com_name[3],com_name[4],com_name[5],com_name[6],com_name[7],
               com_name[8],com_name[9])) < 2){ /* able to get at most 9 arguments. */
      printf("uumkey:file %s. Line %d is illegal.\r\n" ,file_name, lc);
      continue;       /* every command needs at least 1 argument. */
    }

/* henkan_on is treated as like others. so REMOVE below if possible. 
    if (strcmp(func_name, "henkan_on")== 0){
      if ((com_num = expand_argument(com_name[0])) < 0) {
	fprintf(stderr,"uumkey: %s に対する %s は不適切なコードです。\r\n",
	func_name, com_name[0]);
	fflush(stderr);
      } else kk_on = com_num;
    } else 
*/
    if (strcmp(func_name, "include")== 0){
      if(0 != expand_expr(com_name[0])){
	/*
      	fprintf(stderr,"uumkey: %s を展開できません。\r\n", com_name[0]);
	*/
      	fprintf(stderr, MSG_GET(29), com_name[0]);
      }
      key_binding(com_name[0]);
    }else if (strcmp(func_name, "unset")== 0){
      if((k = find_entry(com_name[0])) != -1){
	key_binded[k] = 0;
	remove_key_bind(k);
      }else if(strcmp(com_name[0],"quote_keyin") == 0){
	quote_code = -1;
      }
    }else if (strcmp(func_name, "quote_keyin")== 0){
      if ((com_num = expand_argument(com_name[0])) < 0) {
	fprintf(stderr,"uumkey: %s is an inappropriate code for %s.\r\n",com_name[0],func_name);
	fflush(stderr);
      } else quote_code = com_num;
    } else {
      for(k = 0 ; k < kansuu_hyo_entry_suu ; k++){
	if (strcmp(func_name, kansuu_hyo[k].kansuumei)== 0){ /* found ! */
	  if(key_binded[k]){
	    remove_key_bind(k);
	  }
	  key_binded[k] = 1;
	  for(kk = 0 ; kk < com_cnt - 1 ; kk++) {
	    com_num = expand_argument(com_name[kk]);
	    if (com_num < 0) {
	      fprintf(stderr,"uumkey: %s is an inappropriate code for %s.\r\n",com_name[kk],func_name);
	      fflush(stderr);
	    } else {
	      for(tbl = 0 ; tbl < TBL_CNT ; tbl++){
		if ((main_table[tbl][com_num] != NULL) && 
		    (kansuu_hyo[k].func[tbl] != NULL)) {
		  /* fprintf(stderr, "uumkey: Duplicate Definition at key: %s.\n",com_name[kk]);*/
		  /*
		  fprintf(stderr, "uumkey: %s(0x%x)のキーが二重定義です。at line %d (%s)(table No. %d).\r\n",
		  */
		  fprintf(stderr, MSG_GET(30),
			  com_name[kk],com_num,lc,kansuu_hyo[k].kansuumei,tbl);
/*		  fprintf(stderr, "lc = %d,kansuu_hyo %d(%s) ,%d , \
                     %s(0x%x)のキーが二重定義です。\n",lc,k,kansuu_hyo[k].kansuumei,tbl ,com_name[kk],
			  com_num);
*/
		  main_table[tbl][com_num] = kansuu_hyo[k].func[tbl];
		  romkan_clear_tbl[tbl][com_num] = kansuu_hyo[k].romkan_flag;
		} else if (kansuu_hyo[k].func[tbl] != NULL){
		  main_table[tbl][com_num] = kansuu_hyo[k].func[tbl];
		  romkan_clear_tbl[tbl][com_num] = kansuu_hyo[k].romkan_flag;
		}
	      }
	    }
	  }
	  break;
	} /* end of routine when command is founded. */
       
	/* loop while command is founded */
      }
      if(k == kansuu_hyo_entry_suu){
	fprintf(stderr, "uumkey: unknown entry name %s.\r\n",func_name);
      }
    }
  }
  fclose(fp);
  return(0);
}

/** argument expansion. */
int
expand_argument(st)
char *st;
{
  int num;

  if(*st == '^'){
    num = (*(st + 1)) & 0x1f;
  } else if (*st == '0' || *st == '\\') {
    if (*(st + 1) == 'x' || *(st + 1) == 'X') {
      sscanf(st + 2,"%x",&num);
    } else {
      sscanf(st,"%o",&num);
    }
  } else if(!('0' <= *st && *st <= '9')){
    return(-1);
  } else {
    num = atoi(st);
  }
  if(num >= TBL_SIZE){
    printf("uumkey: You can only bind a function to code less than %d.\r\n",
    	   TBL_SIZE);
    return(-1);
  }
  return(num);
}

void
remove_key_bind(key)
int key;
{
  int tbl;
  int com;

  for(com = 0 ; com < TBL_SIZE; com++){
    for(tbl = 0 ; tbl < TBL_CNT ; tbl++){
      if (kansuu_hyo[key].func[tbl] != NULL) {
	if(main_table[tbl][com] == kansuu_hyo[key].func[tbl]){
	  main_table[tbl][com] = NULL;
	}
      }
    }
  }
}


int
find_entry(com_name)
char *com_name;
{
  int k;

  for(k = 0 ; k < kansuu_hyo_entry_suu ; k++){
    if (strcmp(com_name, kansuu_hyo[k].kansuumei)== 0){
      if(key_binded[k]){
	return(k);
      }
    }
  }
  return(-1);
}
