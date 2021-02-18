/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 *  Author : Osamu Hata, NEC Corporation  (hata@d1.bs2.mt.nec.co.jp)
 *
 */

static	char	rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/cmd/icustom/RCS/icustom.c,v 3.4 91/09/06 13:04:45 hata Exp $";

#include <curses.h>
#include <signal.h>
#include "icustom.h"

#ifdef SVR4
#include <locale.h>
#endif

#define L_MARGIN 10
#define KANKAKU  2

#define _KEY_SPACE   0x20
#define _KEY_ENTER   0x0d
#define _KEY_A       0x61
#define _KEY_D       0x64
#define _KEY_I       0x69
#define _KEY_N       0x6e
#define _KEY_K       0x6b
#define _KEY_U       0x75
#define _KEY_Q       0x71
#define _KEY_ESC     0x1b
#define _KEY_C_N     0x0e
#define _KEY_C_P     0x10
#define _KEY_C_F     0x06
#define _KEY_C_B     0x02

#define NEXTPAGE   2
#define PREVPAGE   1
#define DONOT      0

#define UP         2
#define DOWN      -2
#define RIGHT      1
#define LEFT      -1

#define KEYS       1
#define ACTS       2
#define YAME      -1

#define INSERT    -1
#define ADD        1

#define NULL_UNDO  1
#define STR_UNDO   0
#define NON_UNDO  -1

SCREEN *trm;
SCREEN *set_term();

WINDOW *base_win,  *err_win,   *load_win,
       *save_win,  *dic_win,   *ddic_win,
       *kctm_win,  *disp_win,  *kutl_win,
       *mode1_win, *mode2_win, *dddic_win,
       *etc_win,   *_etc_win,  *end_win;

disp_block(win, y, x, block, start, end)
WINDOW *win;
int y, x, start, end;
char **block;
{
  int i, j;

  if (start > 0)
    i = start;
  else
    i = 0;
  if (end > 0) {
    for(j =0; block[i]; i++,j++) {
      if (i <= end) {
	if (block[i]) {
	  mvwaddstr(win, y+j, x, block[i]);
	  wclrtoeol(win);
	}
	else {
	  y--;
	}
      }
    }
  }
  else {
    for(j = 0; block[i]; i++,j++) {
      if (block[i]) {
	mvwaddstr(win, y+j, x, block[i]);
	wclrtoeol(win);
      }
      else {
	y--;
      }
    }
  }
  wrefresh(win);
}

err_word(format, string)
char *format, *string;
{
  wattron(err_win,A_BLINK);
  wprintw(err_win, format, string);
  wattroff(err_win,A_BLINK);
  wrefresh(err_win);
}

current_word(win, format, string)
char *format, *string;
{
  wattron(win, A_REVERSE);
  wprintw(win, format, string);
  wattroff(win, A_REVERSE);
  wrefresh(win);
}

clr_cul_to_end(win, y, x)
WINDOW *win;
int y, x;
{
  wmove(win, y, x);
  wclrtobot(win);
  wrefresh(win);
}

void
current_print(win,y,x,string)
WINDOW *win;
int    y, x;
char   *string;
{
  wattron(win, A_REVERSE);
  mvwaddstr(win, y, x, string);
  wattroff(win, A_REVERSE);
  touchwin(win);
  wrefresh(win);
}

current_print2(win, y, x, string, copy)
WINDOW *win;
int y, x;
char *string, *copy;
{
  int i = 0;

  for (; string[i] != ' ' && string[i] != '\0'; i++)
    copy[i] = string[i];
  copy[i] = '\0';
  current_print(win, y, x, copy);
  return (x + i/2 -1);
}

inc_rev_print(win, y, x, str1, rev, str2)
WINDOW *win;
int y, x;
char *str1, *rev, *str2;
{
  mvwaddstr(win, y, x, str1);
  current_word(win, "%s", rev);
  wprintw(win, "%s", str2);
  wclrtoeol(win);
}

loadFile()
{
  FILE *f_load;
  char f_name[1024];
  int  y, x;

  mvwaddstr(load_win, 1, 10, "☆カスタマイズファイルの読み込み☆");
  mvwaddstr(load_win, 5, 10, "ファイル名 :");
  touchwin(load_win);
  wrefresh(load_win);
  getyx(load_win, y, x);
  for(;;) {
    echo();
    wgetstr(load_win, f_name);
    noecho();
    werase(err_win);
    wrefresh(err_win);
    if (*f_name != '\0') {
      *err_mess = (char)NULL;
      tilda(f_name);
      if (initFileSpecified)
	free(initFileSpecified);
      initFileSpecified = (char *)malloc(strlen(f_name)+1);
      if (initFileSpecified)
	strcpy(initFileSpecified, f_name);
      parse();
      initctm();
      if (*err_mess) {
	err_word("ファイル:%sはオープンできません。",err_mess);
	beep();
	clr_cul_to_end(load_win, y, x);
	if (initFileSpecified) {
	  free(initFileSpecified);
	  initFileSpecified = (char *)NULL;
	  *err_mess = '\0';
	}
      }
      else {
	clr_cul_to_end(load_win, y, x);
	return;
      }
    }
    else {
      return;
    }
  }
}

saveFile()
{
  FILE *f_save;
  char f_name[1024];
  int  y, x;

  mvwaddstr(save_win, 1, 10, "☆カスタマイズファイルへの保存☆");
  mvwaddstr(save_win, 5, 10, "ファイル名 :");
  touchwin(save_win);
  wrefresh(save_win);
  getyx(save_win, y, x);
  for(;;) {
    echo();
    wgetstr(save_win, f_name);
    noecho();
    werase(err_win);
    wrefresh(err_win);
    tilda(f_name);
    if (*f_name != '\0') {

      f_save = fopen(f_name, "w");
      if (!f_save) {
	err_word("ファイル:%sはオープンできません。",f_name);
	beep();
	clr_cul_to_end(save_win, y, x);
      }
      else {
	write_iroha(f_save);
	fclose(f_save);
	clr_cul_to_end(save_win, y, x);
	return;
      }
    }
    else { 
      return;
    }
  }
}

ask_dic(loc)
int loc;
{
  switch(loc) {
  case 0 : /* ローマ字かな変換テーブル */
    return 0;
  case 1 : /* システム辞書 */
    return nkanjidics;
  case 2 : /* 部首変換辞書 */
    return nbushudics;
  case 3 : /* ユーザ辞書 */
    return nuserdics;
  case 4 : /* 連語変換 */
    return 0;
  }
}

print_dic_list(loc, start, end, current)
int loc, start, end, current;
{
  int y, x, i, kazu;
  char **dic;
  
  switch(loc) {
  case 0 : /* ローマ字かな変換テーブル */
    werase(ddic_win);
    if (RomkanaTable)
      mvwaddstr(ddic_win, 0, L_MARGIN, RomkanaTable);
    wrefresh(ddic_win);
    return;
  case 1 : /* システム辞書 */
    dic = kanjidicname;
    kazu = nkanjidics;
    break;
  case 2 : /* 部首変換辞書 */
    dic = bushudicname;
    kazu = nbushudics;
    break;
  case 3 : /* ユーザ辞書 */
    dic = userdicname;
    kazu = nuserdics;
    break;
  case 4 : /* 連語変換 */
    werase(ddic_win);
    if (RengoGakushu)
      mvwaddstr(ddic_win, 0, L_MARGIN, RengoGakushu);
    wrefresh(ddic_win);
    return;
  }
  werase(ddic_win);
  if (!kazu) {
    wrefresh(ddic_win);
    return;
  }
  for (i = start; i <= end && i < kazu; i++) {
    if (i < (7+1))
      mvwaddstr(ddic_win, i, L_MARGIN, dic[i]);
    else
      mvwaddstr(ddic_win, i -7-1, L_MARGIN, dic[i]);
  }
  if (current != -1) {
    if (current < (7+1)) {
      if (current == kazu) 
	current--;
      current_print(ddic_win, current, L_MARGIN, dic[current]);
    }
    else {
      if (current == kazu)
	current--;
      if (current == 7) {
	for (i = 0; i <= end && i < 7; i++)
	  mvwaddstr(ddic_win, i, L_MARGIN, dic[i]);
	current_print(ddic_win, current, L_MARGIN, dic[current]);	
      }
      current_print(ddic_win, current -7-1,L_MARGIN,dic[current]);
    }
  }
  wrefresh(ddic_win);
  return current;
}

confDic()
{
  char d_name[512], *d_name2, *dic[16];
  int  y, x, location, ndic,
       c_location = 0, d_location = 0;
  chtype c, d;

/*  echo();*/
  for(;;) {
    nocbreak();
    clear();
    disp_block(dddic_win, 0, L_MARGIN, d_mess, -1, -1);
    wrefresh(dddic_win);
    touchwin(ddic_win);
    werase(ddic_win);
    wrefresh(ddic_win);
    werase(dic_win);
    mvwaddstr(dic_win, 1, 10, "☆使用する辞書の選択☆");
    mvwaddstr(dic_win, 1, 40, "辞書の種類選択(辞書登録)");
    for (location = 0; dic_menu[location]; location++)
      mvwaddstr(dic_win, 3 + location, L_MARGIN, dic_menu[location]);
    if (RomkanaTable) {
      mvwaddstr(ddic_win, 0, L_MARGIN, RomkanaTable);
      wrefresh(ddic_win);
    }
    current_print(dic_win, 3 + c_location, 10, dic_menu[c_location]);
    cbreak();
    while((c=wgetch( dic_win )) !=  _KEY_ESC && c != _KEY_Q) {/* SVR4 curses */
      switch(c) {
      case KEY_DOWN :
      case _KEY_C_N :
	if (c_location == location -1)
	  break;
	mvwaddstr(dic_win, 3 + c_location, L_MARGIN, dic_menu[c_location]);
	c_location++;
	current_print(dic_win, 3 + c_location, L_MARGIN, dic_menu[c_location]);
	getyx(dic_win, y, x);
	print_dic_list(c_location, 0, 7, -1);
	break;
      case KEY_UP :
      case _KEY_C_P :
	if (!c_location)
	  break;
	mvwaddstr(dic_win, 3 + c_location, L_MARGIN, dic_menu[c_location]);
	c_location--;
	current_print(dic_win, 3 + c_location, L_MARGIN, dic_menu[c_location]);
	getyx(dic_win, y, x);
	print_dic_list(c_location, 0, 7, -1);
	break;
      case '\n' :
	if (ask_dic(c_location) < 15) {
	  getyx(dic_win, y, x);
	  mvwaddstr(dic_win, y, x +1,":");
	  echo();
	  wgetstr(dic_win, d_name);
	  noecho();
	  if (strlen(d_name))
	    append_dic(c_location, d_name);
	  print_dic_list(c_location, 0, 7, -1);
	  clr_cul_to_end(dic_win, 3 + c_location, 0);
	  for (location = c_location; dic_menu[location]; location++)
	    mvwaddstr(dic_win, 3 + location, L_MARGIN, dic_menu[location]);
	  current_print(dic_win, 3 + c_location, L_MARGIN,
			dic_menu[c_location]);
	  wrefresh(dic_win);
	}
	break;
      case _KEY_D :
	if (!c_location) {
	  RomkanaTable = NULL;
	  clr_cul_to_end(ddic_win, 0, L_MARGIN);
	 } 
	break;
      case _KEY_SPACE :
	if (!c_location)
	  break;
	mvwaddstr(dic_win, 1, 40, "登録内容表示");
	wclrtoeol(dic_win);
	wrefresh(dic_win);
	print_dic_list(c_location, 0, 7, 0);
	while((d=wgetch( dic_win )) != _KEY_SPACE && d != _KEY_ESC && d != _KEY_Q) { /* SVR4 curses */
	  switch(d) {
	  case KEY_DOWN :
	  case _KEY_C_N :
	    if (d_location == 15 || d_location >= ask_dic(c_location) -1)
	      break;
	    d_location++;
	    if (d_location < (7+1))
	      print_dic_list(c_location, 0, 7, d_location);
	    else
	      print_dic_list(c_location, 7+1, 15, d_location);
	    break;
	  case KEY_UP :
	  case _KEY_C_P :
	    if (!d_location) {
	      print_dic_list(c_location, 0, 7, d_location);
	      break;
	    }
	    d_location--;
	    if (d_location < (7+1))
	      print_dic_list(c_location, 0, 7, d_location);
	    else
	      print_dic_list(c_location, 7+1, 15, d_location);
	    break;
	  case _KEY_D :
	    delete_dic(c_location, d_location);
	    if (d_location < (7+1))
	      d_location = print_dic_list(c_location, 0, 7, d_location);
	    else
	      d_location = print_dic_list(c_location, 7+1, 15, d_location);
	    break;
	  }
	}
	d_location = 0;
	mvwaddstr(dic_win, 1, 40, "辞書の種類選択(辞書登録)");
	wclrtoeol(dic_win);
	wmove(dic_win, y, x);
	wrefresh(dic_win);
	break;
      }
    }
    werase(dddic_win);
    wrefresh(dddic_win);
    return;
  }  
}

#define char_width(c) (((c) & 0x80) ? (((c) == 0x8e) ? 1 : 2) : 1)

howManyLines(s1, s2, s3, cols)
unsigned char *s1, *s2, *s3;
int cols;
{
  unsigned char *p;
  int l = 1, co = L_MARGIN, cwidth;

  for (p = s1 ; *p ; p++) {
    cwidth = char_width(*p);
    if (*p & 0x80) {
      p++;
    }
    if (co + cwidth > cols) {
      l++;
      co = cwidth;
    }
    else {
      co += cwidth;
    }
  }
  for (p = s2 ; *p ; p++) {
    cwidth = char_width(*p);
    if (*p & 0x80) {
      p++;
    }
    if (co + cwidth > cols) {
      l++;
      co = cwidth;
    }
    else {
      co += cwidth;
    }
  }
  for (p = s3 ; *p ; p++) {
    cwidth = char_width(*p);
    if (*p & 0x80) {
      p++;
    }
    if (co + cwidth > cols) {
      l++;
      co = cwidth;
    }
    else {
      co += cwidth;
    }
  }
  return l;
}

m_set(ph_mode)
int ph_mode;
{
  switch(ph_mode) {
  case 0:
    current_acts = allFunc;
    current_keys = allKey;
    return NallKeyFunc;
  case 1:
    current_acts = alphaFunc;
    current_keys = alphaKey;
    return NalphaKeyFunc;
  case 2:
    current_acts = yomiganaiFunc;
    current_keys = yomiganaiKey;
    return NyomiganaiKeyFunc;
  case 3:
    current_acts = yomiFunc;
    current_keys = yomiKey;
    return NyomiKeyFunc;
  case 4:
    current_acts = jishuFunc;
    current_keys = jishuKey;
    return NjishuKeyFunc;
  case 5:
    current_acts = tankouhoFunc;
    current_keys = tankouhoKey;
    return NtankouhoKeyFunc;
  case 6:
    current_acts = ichiranFunc;
    current_keys = ichiranKey;
    return NichiranKeyFunc;
  case 7:
    current_acts = zenHiraFunc;
    current_keys = zenHiraKey;
    return NzenHiraKeyFunc;
  case 8:
    current_acts = zenKataFunc;
    current_keys = zenKataKey;
    return NzenKataKeyFunc;
  case 9:
    current_acts = zenAlphaFunc;
    current_keys = zenAlphaKey;
    return NzenAlphaKeyFunc;
  case 10:
    current_acts = hanKataFunc;
    current_keys = hanKataKey;
    return NhanKataKeyFunc;
  case 11:
    current_acts = hanAlphaFunc;
    current_keys = hanAlphaKey;
    return NhanAlphaKeyFunc;
  case -1:
    current_acts = cc_acts;
    current_keys = cc_keys;
    return 1;
  }
}

create_key_buff(mode, num, kora, ban, str1, rev, str2)
int mode, num, kora, ban;
char *str1, *rev, *str2;
{
  int i, nseq, max_key, max_act ,sum, len = 0, length;
  char buff[512], *p, *r, *key, *acts;

  strcpy(str1, "");
  strcpy(rev, "");
  strcpy(str2, "");
  strcpy(buff, "");
  if (mode == -1) {
    if (!cc_keys_len) {
      if (kora == KEYS) {
	strcpy(rev, "新規キー");
	len +=8;
      }
      else {
	strcpy(str1, "新規キー》");
	len +=10;
      }
    }
    if (!cc_acts_len) {
      if (kora == ACTS) {
	strcpy(rev, "新規機能");
	len +=8;
      }
      else if (kora == KEYS) {
	strcpy(str2, "》新規機能");
	len +=10;
      }
      else {
	strcpy(str2, "新規機能");
	len +=8;
      }
    }
/*
    if (!cc_acts_len && !cc_keys_len && kora == DONOT) {
      strcpy(str1, "新規キー》新規機能");
      len +=18;
    }
*/
    if (cc_keys_len) {
      if (kora == KEYS) {
        if (showChar((unsigned char)*ccc_keys)) {
	  strcpy(rev, returnKey);
	  length = strlen(returnKey);
	  len = len + length;
	}
      }
      else {
	if (showChar((unsigned char)*ccc_keys)) {
	  strcpy(str1, returnKey);
	  strcat(str1, "》");
	  length = strlen(returnKey);
	  len = len + length +2;
	}
      }
    }
    if (cc_acts_len) {
      if (kora == ACTS) {
	if (funcList[*ccc_acts]) {
	  strcpy(rev, funcList[*ccc_acts]);
	  length = strlen(funcList[*ccc_acts]);
	  len = len + length;
	}
      }
      else if (kora == KEYS) {
	if (funcList[*ccc_acts]) {
	  strcpy(str2, "》");
	  strcat(str2, funcList[*ccc_acts]);
	  length = strlen(funcList[*ccc_acts]);
	  len = len + length +2;
	}
      }
      else {
	if (funcList[*ccc_acts]) {
	  strcat(str1, funcList[*ccc_acts]);
	  length = strlen(funcList[*ccc_acts]);
	  len = len + length;
	}
      }
    }
    return len;
  }

  nseq = m_set(mode);
  if (num > nseq -1)
    return -1;
  p = current_keys[num];
  max_key = specialen(p);
  if (kora == KEYS) {
    if (ban > max_key-1)
      ban = max_key-1;
    for (i = 0; i < ban; i++) {
      len = strlen(showChar((unsigned char)*(p+i))) +len +2;
      strcat(buff, returnKey);
      strcat(buff, "→");
    }
    if (str1)
      strcpy(str1, buff);
    len = strlen(showChar((unsigned char)*(p+ban))) +len +2; 
    if (!(mode == -1 && !cc_keys_len) && *p == -1 && ban == -1) {
      if (rev)
	strcpy(rev, "undefine");
    }
    else {
      if (rev)
	strcpy(rev, returnKey);
    }
    strcpy(buff, "→");
    for (i = ban +1; i < max_key; i++) {
      len = strlen(showChar((unsigned char)*(p+i))) +len +2;
      strcat(buff, returnKey);
      strcat(buff, "→");
    }
    buff[strlen(buff) -2] = '\0';
    strcat(buff, "》");
  }
  else {
    if (!(mode == -1 && !cc_keys_len) && *p == -1)
	strcpy(buff, "undefine  ");
    for (;*p != -1; p++) {
      len = strlen(showChar((unsigned char)*p)) +len +2;
      strcat(buff, returnKey);
      strcat(buff, "→");
    }
    buff[strlen(buff) -2] = '\0';
    strcat(buff, "》");
  }
  p = current_acts[num];
  max_act = strlen(p);
  if (kora == ACTS) {
    if (ban > max_act -1)
      ban = max_act -1;
    for (i = 0; i < ban; i++) {
      len = strlen(funcList[(unsigned char)*(p+i)]) +len +2;
      strcat(buff, funcList[(unsigned char)*(p+i)]);
      strcat(buff, "→");
    }
    if (str1)
      strcpy(str1, buff);
    len = strlen(funcList[(unsigned char)*(p+ban)]) +len +2;
    if (rev)
      strcpy(rev, funcList[(unsigned char)*(p+ban)]);
    strcpy(buff, "→");
    for (i = ban +1; i < max_act; i++) {
      len = strlen(funcList[(unsigned char)*(p+i)]) +len +2;
      strcat(buff, funcList[(unsigned char)*(p+i)]);
      strcat(buff, "→");
    }
    buff[strlen(buff) -2] = '\0';
    len -=2;
    if (str2)
      strcpy(str2, buff);
  }
  else {
    for (;*p; p++) {
      len = strlen(funcList[(unsigned char)*p]) +len +2;
      strcat(buff, funcList[(unsigned char)*p]);
      strcat(buff, "→");
    }
    buff[strlen(buff) -2] = '\0';
    len -=2;
    if ((kora == DONOT) && str1)
      strcpy(str1, buff);
    if ((kora == KEYS) && str2)
      strcpy(str2, buff);
  }
  return len;
}

sp_disp(mode, top, line, kora, ban)
int mode, top, line, kora, ban;
{
  int length, tate = 0, pate = 0, i, j, kazu, end_stat = 0, etent;
  char str1[256], rev[256], str2[256];

  clr_cul_to_end(disp_win, 5, 0);

  kazu = m_set(mode);

  if (touroku_start >= 0) {
    if (line >= kazu)
      line = kazu -1;
    if (top > line)
      top = line;
    for (i = kazu -1, j = 0;  i == touroku_start; i--,j++) {
      if (j >= top) {
	tate = tate + dk_blocks[i].gyousu;
	if (line == j)
	      WDisp.current = i;
	if ((top == -1 && tate > 5) || (top != -1 && tate > 6)) {
	  end_stat = j;
	  if (end_stat > line) {
	  }
	  break;
	}
      }
    }
    if ((top == -1 && tate <= 5) || (top != -1 && tate <= 6)) {
      for (i = 0; i < touroku_start; i++, j++) {
	if (j >= top) {
	  tate = tate + dk_blocks[i].gyousu;
	  if (line == j)
	    WDisp.current = i;
	  if ((top == -1 && tate > 5) || (top != -1 && tate > 6)) {
	    break;
	  }
	}
      }
      end_stat = j;
      if (line >= end_stat) {
	sp_disp(mode, ++top, line, kora, ban);
	return;
      }
    }
  }
  else {
    if (line >= kazu)
      line = kazu-1;
    if (top > line)
      top = line;
    if (top < 0) {
      i = 0;
    }
    else {
      i = top;
    }
    WDisp.current = line;
    for (; i < kazu; i++) {
      tate = tate + dk_blocks[i].gyousu;
      if ((top < 0  && tate > 5) || (top > -1 && tate > 6)) {
	break;
      }
    }
    end_stat = i;
    if (line >=  end_stat) {
      sp_disp(mode, ++top, line, kora, ban);
      return;
    }
  }
  if (top < 0) {
    create_key_buff(-1, 0, DONOT, 0, str1, rev, str2);
    inc_rev_print(disp_win, 5, L_MARGIN, str1, rev, str2);
  }
  if (line < 0) {
    create_key_buff(-1, 0, kora, 0, str1, rev, str2);
    WDisp.current = line;
  }
  else {
    if (kora == KEYS && ban >=  dk_blocks[WDisp.current].keysu) {
      ban = dk_blocks[WDisp.current].keysu -1;
    }
    else if (kora == ACTS && ban >= dk_blocks[WDisp.current].actsu) {
      ban = dk_blocks[WDisp.current].actsu -1;
    }
    create_key_buff(mode, WDisp.current, kora, ban, str1, rev, str2);
  }
  if (touroku_start >= 0) {
    if (line == -1)
      inc_rev_print(disp_win, 5, L_MARGIN, str1, rev, str2);
    for (i = kazu -1, j = 0;  i == touroku_start; i--,j++) {
      if (j >= top && j < end_stat) {
	if (top == -1) {
	  if (j == line) {
	    inc_rev_print(disp_win, 6+pate, L_MARGIN, str1, rev, str2);
	  }
	  else {
	    mvwaddstr(disp_win, 6+pate , L_MARGIN, dk_blocks[i].str);
	  }
	  wclrtoeol(disp_win);
	  pate = pate + dk_blocks[i].gyousu;
	}
	else {
	  if (j == line) {
	    inc_rev_print(disp_win, 5 +pate, L_MARGIN, str1, rev, str2);
	  }
	  else {
	    mvwaddstr(disp_win, 5 +pate , L_MARGIN, dk_blocks[i].str);
	  }
	  wclrtoeol(disp_win);
	  pate = pate + dk_blocks[i].gyousu;
	}
      }
    }
    for (i = 0; i < touroku_start; i++, j++) {
      if (j >= top && j < end_stat) {
	if (top == -1) {
	  if (j == line) {
	    inc_rev_print(disp_win, 6+pate, L_MARGIN, str1, rev, str2);
	  }
	  else {
	    mvwaddstr(disp_win, 6+pate , L_MARGIN, dk_blocks[i].str);
	  }
	  wclrtoeol(disp_win);
	  pate = pate + dk_blocks[i].gyousu;
	}
	else {
	  if (j == line) {
	    inc_rev_print(disp_win, 5 +pate, L_MARGIN, str1, rev, str2);
	  }
	  else {
	    mvwaddstr(disp_win, 5 +pate , L_MARGIN, dk_blocks[i].str);
	  }
	  wclrtoeol(disp_win);
	  pate = pate + dk_blocks[i].gyousu;
	}
      }
    }
  }
  else {
    if (top < 0) {
      for (i =0, pate = 0; i < end_stat; i++) {
	if (i == line) {
	  inc_rev_print(disp_win, 6+pate, L_MARGIN, str1, rev, str2);
	}
	else {
	  mvwaddstr(disp_win, 6+pate, L_MARGIN, dk_blocks[i].str);
	}
	wclrtoeol(disp_win);
	pate = pate + dk_blocks[i].gyousu;
      }
      if (line == -1)
	inc_rev_print(disp_win, 5, L_MARGIN, str1, rev, str2);
    }
    else {
      for (i = top, pate = 0; i < end_stat; i++) {
	if (i == line) {
	  inc_rev_print(disp_win, 5+pate, L_MARGIN, str1, rev, str2);
	}
	else {
	  mvwaddstr(disp_win, 5+pate, L_MARGIN, dk_blocks[i].str);
	}
	wclrtoeol(disp_win);
	pate = pate + dk_blocks[i].gyousu;
      }
    }
  }

  WDisp.top = top;
  WDisp.line = line;
  WDisp.kora = kora;
  WDisp.ban = ban;
}

set_dk_blocks(mode, n)
int mode, n;
{
  char str1[256], rev[256], str2[256];

  dk_blocks[n].len = create_key_buff(mode, n, DONOT, 0, str1, rev, str2);
  dk_blocks[n].str = (char *)malloc(strlen(str1) +1);
  strcpy(dk_blocks[n].str, str1);
  dk_blocks[n].keysu = specialen(current_keys[n]);
  if (!dk_blocks[n].keysu)
    dk_blocks[n].keysu = 1;
  dk_blocks[n].actsu = strlen(current_acts[n]);
  dk_blocks[n].gyousu = howManyLines(str1, rev, str2, COLS);
}

init_dk_blocks(mode)
int mode;
{
  int i;

  for (i = 0; i < m_set(mode); i++)
    set_dk_blocks(mode, i);
  dk_blocks[i].str = (char *)NULL;
}

copy_dk_blocks(a, b)
int a, b;
{
  dk_blocks[a].len = dk_blocks[b].len;
  dk_blocks[a].str = dk_blocks[b].str;
  dk_blocks[a].keysu = dk_blocks[b].keysu;
  dk_blocks[a].actsu = dk_blocks[b].actsu;
  dk_blocks[a].gyousu = dk_blocks[b].gyousu;
}

fin_dk_blocks()
{
  int i;
  for (i = 0; dk_blocks[i].str; i++)
      free(dk_blocks[i].str);
}

tourokuN(ph_mode, N)
int ph_mode, N;
{
  switch(ph_mode) {
  case 0:
    NallKeyFunc = N;
    break;
  case 1:
    NalphaKeyFunc = N;
    break;
  case 2:
    NyomiganaiKeyFunc = N;
    break;
  case 3:
    NyomiKeyFunc = N;
    break;
  case 4:
    NjishuKeyFunc = N;
    break;
  case 5:
    NtankouhoKeyFunc = N;
    break;
  case 6:
    NichiranKeyFunc = N;
    break;
  case 7:
    NzenHiraKeyFunc = N;
    break;
  case 8:
    NzenKataKeyFunc = N;
    break;
  case 9:
    NzenAlphaKeyFunc = N;
    break;
  case 10:
    NhanKataKeyFunc = N;
    break;
  case 11:
    NhanAlphaKeyFunc = N;
    break;
  }
}

for_UNDO(status)
char status;     
{
  char *p, *q, *s;

  undo_status = status;
  undo_ts = touroku_start;
  strcpy(undo_acts, current_acts[WDisp.current]);
  for (s = p = current_keys[WDisp.current], q = undo_keys ; *p != -1; p++,q++)
    *q = *p;
  *q = -1;
  if (p == s)
    *++q = -1;

  strcpy(U_dk_blocks.str, dk_blocks[WDisp.current].str);
  U_dk_blocks.len = dk_blocks[WDisp.current].len;
  U_dk_blocks.gyousu = dk_blocks[WDisp.current].gyousu;
  U_dk_blocks.keysu = dk_blocks[WDisp.current].keysu;
  U_dk_blocks.actsu = dk_blocks[WDisp.current].actsu;

  U_WDisp.top = WDisp.top;
  U_WDisp.line = WDisp.line;
  U_WDisp.kora = WDisp.kora;
  U_WDisp.ban = WDisp.ban;
  U_WDisp.current = WDisp.current;
}

from_UNDO()
{
  char *p, *q, *s;
int i;
  touroku_start = undo_ts;
  current_acts[U_WDisp.current] = (char *)malloc(strlen(undo_acts)+1);
  strcpy(current_acts[U_WDisp.current], undo_acts);
  current_keys[U_WDisp.current] = (char *)malloc(specialen(undo_keys)+1);
  for (s = p = current_keys[U_WDisp.current], q = undo_keys ; *q != -1;
       p++,q++)
    *p = *q;
  *p = -1;
  if (p == s);
  *++q = -1;

  dk_blocks[U_WDisp.current].str = (char *)malloc(strlen(U_dk_blocks.str) +1);
  strcpy(dk_blocks[U_WDisp.current].str, U_dk_blocks.str);
  dk_blocks[U_WDisp.current].len = U_dk_blocks.len;
  dk_blocks[U_WDisp.current].gyousu = U_dk_blocks.gyousu;
  dk_blocks[U_WDisp.current].keysu = U_dk_blocks.keysu;
  dk_blocks[U_WDisp.current].actsu = U_dk_blocks.actsu;

  WDisp.top = U_WDisp.top;
  WDisp.line = U_WDisp.line;
  WDisp.kora = U_WDisp.kora;
  WDisp.ban = U_WDisp.ban;
  WDisp.current = U_WDisp.current;

  undo_status = -1;
}

shinki(mode, kora, num)
int mode, kora, num;
{
  int N;
  char *p;

  undo_status = -1;

  N = m_set(mode);
  if (kora == KEYS) {
    ccc_keys[0] = num;
    ccc_keys[1] = -1;
    if (!cc_acts_len) {
      cc_keys_len = 1;
      return;
    }
  }
  else if (kora == ACTS) {
    ccc_acts[0] = num;
    ccc_acts[1] = '\0';
    if (!cc_keys_len) {
      cc_acts_len = 1;
      return;
    }
  }
  if ((kora == KEYS && cc_acts_len) || (kora == ACTS && cc_keys_len)){
    cc_keys_len = cc_acts_len = 0;
    p = current_keys[N] = (char *)malloc(2);
    *p = ccc_keys[0];
    *++p = ccc_keys[1];
    current_keys[N+1] = (char *)NULL;
    current_acts[N] = (char *)malloc(2);
    strncpy(current_acts[N], ccc_acts, 2);
    current_acts[N+1] = (char *)NULL;
    tourokuN(mode, N+1);
    touroku_start = N;
    set_dk_blocks(mode, N);
    dk_blocks[N+1].str = (char *)NULL;
    sp_disp(mode, -1, 0, kora, 0);
  }
}

to_multi(mode, num, kora, ban, which)
int mode, num, kora, ban, which;
{
  int atai, hosei, kazu, i;
  char *buff,f;

  m_set(mode);
  if (which == ADD) {
    for_UNDO(_KEY_A);
    hosei = 1;
  }
  else if (which == INSERT) {
    for_UNDO(_KEY_I);
    hosei = 0;
  }
  
  if (WDisp.kora == KEYS) {
    if (current_keys[num][ban] == 255 || current_keys[num][ban] == -1)
      goto red;
    atai = keyIn(mode, which);
    if (atai == 255 || atai == -1)
      goto red;
    kazu = dk_blocks[num].keysu;
    buff = (char *)malloc(kazu +2);
    specpy(buff, current_keys[num]);
  }
  else if (WDisp.kora == ACTS) {
    if (current_keys[num][0] == 255 || current_keys[num][0] == -1)
      goto red;
    atai = actIn(mode, which);
    if (atai == -1)
      goto red;
    kazu = dk_blocks[num].actsu;
    buff = (char *)malloc(kazu +2);
    strcpy(buff, current_acts[num]);
  }

  for (i = kazu; i >=  ban + hosei; i--)
    buff[i+1] = buff[i];
  buff[ban + hosei] = atai;
  if (kora == KEYS) {
    if (current_keys[num])
      free(current_keys[num]);
    current_keys[num] = buff;
  }
  else if (kora == ACTS) {
    if (current_acts[num])
      free(current_acts[num]);
    current_acts[num] = buff;
  }
  if (dk_blocks[num].str)
    free(dk_blocks[num].str);
  set_dk_blocks(mode, num);

  red :
  werase(kutl_win);
  disp_block(kutl_win, 0, L_MARGIN, kh_mess, -1, -1);
  wrefresh(kutl_win);
  sp_disp(mode, WDisp.top, WDisp.line, WDisp.kora, WDisp.ban);
}

okikae(mode, num, kora, ban, obj)
int mode, num, kora, ban, obj;
{
  if (kora == KEYS && (dk_blocks[num].keysu > 1 || dk_blocks[num].actsu > 1)
      && obj == 255)
   return;
  for_UNDO('\n');

  m_set(mode);
  if (kora == KEYS)
    current_keys[num][ban] = obj; 
  else if (kora == ACTS)
    current_acts[num][ban] = obj;
  if (dk_blocks[num].str)
    free(dk_blocks[num].str);
  set_dk_blocks(mode, num);
  sp_disp(mode, WDisp.top, WDisp.line, WDisp.kora, WDisp.ban);
}

delete_obj(mode, num, kora, ban)
int mode, num, kora, ban;
{
  int i;
  char *p;

  for_UNDO(_KEY_D);
  
  m_set(mode);
  if (kora == KEYS) {
    p = current_keys[num];
    for (i = ban; current_keys[num][i] != -1; i++)
      current_keys[num][i] = current_keys[num][i+1];
    current_keys[num] = (char *)realloc(current_keys[num], i);
    specpy(current_keys[num], p);
  }
  else if (kora == ACTS) {
    p = current_acts[num];
    for (i = ban; current_acts[num][i] != 0; i++)
      current_acts[num][i] = current_acts[num][i+1];
    current_acts[num] = (char *)realloc(current_acts[num], i);
    strcpy(current_acts[num], p);
  }
  if (dk_blocks[num].str)
    free(dk_blocks[num].str);
  set_dk_blocks(mode, num);
}
    

kill_G(mode, num)
int mode, num;
{
  int kazu, i;

  for_UNDO(_KEY_K);

  kazu = m_set(mode);
  if (dk_blocks[num].str)
    free(dk_blocks[num].str);
  for (i = num;i < kazu; i++) {
    current_keys[i] = current_keys[i+1];
    current_acts[i] = current_acts[i+1];
    copy_dk_blocks(i, i+1);
  }
  tourokuN(mode, kazu-1);
  if (touroku_start > 0 && num != kazu -1)
    touroku_start--;
}

kill_UNDO(mode)
int mode;
{
  int kazu, i, num;

  num = U_WDisp.current;
  kazu = m_set(mode);
  for (i = kazu;i >= num; i--) {
    current_keys[i+1] = current_keys[i];
    current_acts[i+1] = current_acts[i];
    copy_dk_blocks(i+1, i);
  }
  tourokuN(mode, kazu+1);
  from_UNDO();
}

dispIn(cus_mode)
int cus_mode;
{
  char *p;
  char str1[256], rev[256], str2[256];
  int  y, x, i, length;
  int  p_point = L_MARGIN, location, c_location = 0;
  chtype c;

  ccc_keys[0] = -1;
  strcpy(ccc_acts, "");
  cc_keys[0] = ccc_keys;
  cc_acts[0] = ccc_acts;
    
/*  echo();*/
  nocbreak();
  clear();
  touchwin(disp_win);
  werase(disp_win);
  wrefresh(disp_win);
  WDisp.top = WDisp.line = -1;
  WDisp.ban = 0;
  mvwaddstr(disp_win, 1, L_MARGIN, "☆キーのカスタマイズ☆");
  mvwaddstr(disp_win, 3, L_MARGIN, key_menu[cus_mode]); 
  werase(kutl_win);
  disp_block(kutl_win, 0, L_MARGIN, kh_mess, -1, -1);
  init_dk_blocks(cus_mode);
  sp_disp(cus_mode, -1, -1, KEYS, 0);
  wrefresh(disp_win);

  cbreak();
  while((c=wgetch( disp_win )) != _KEY_ESC && c != _KEY_Q ) { /* SVR4 curses */
    switch(c) {
    case KEY_LEFT :
    case _KEY_C_B :
      if (WDisp.kora == KEYS && !WDisp.ban) {
	break;
      }
      else if (WDisp.line == -1 && WDisp.kora == KEYS) {
	break;
      }
      else if (WDisp.line == -1 && WDisp.kora == ACTS) {
	sp_disp(cus_mode,WDisp.top, WDisp.line, KEYS, WDisp.ban);
	break;
      }
      else if (WDisp.kora == ACTS && !WDisp.ban) {
	sp_disp(cus_mode, WDisp.top, WDisp.line, KEYS,
		dk_blocks[WDisp.current].keysu-1);
	break;
      }
      else {
	sp_disp(cus_mode, WDisp.top, WDisp.line, WDisp.kora, WDisp.ban-1);
	break;
      }
    case KEY_RIGHT :
    case _KEY_C_F :
      if (WDisp.line == -1 && WDisp.kora == ACTS) {
	break;
      }
      else if (WDisp.line == -1 && WDisp.kora == KEYS) {
	sp_disp(cus_mode,WDisp.top, WDisp.line, ACTS, WDisp.ban);
	break;
      }
      if (WDisp.kora == KEYS &&
	  WDisp.ban == dk_blocks[WDisp.current].keysu-1) {
	sp_disp(cus_mode, WDisp.top, WDisp.line, ACTS, 0);
	break;
      }
      else {
	sp_disp(cus_mode, WDisp.top, WDisp.line, WDisp.kora, WDisp.ban+1);
	break;
      }
    case KEY_UP :
    case _KEY_C_P :
      if (WDisp.line >= 0)
	  sp_disp(cus_mode, WDisp.top, WDisp.line -1, WDisp.kora, WDisp.ban);
      break;
    case KEY_DOWN :
    case _KEY_C_N :
      sp_disp(cus_mode, WDisp.top, WDisp.line +1, WDisp.kora, WDisp.ban);
      break;
    case _KEY_A :
      if (WDisp.line != -1) {
	to_multi(cus_mode, WDisp.current, WDisp.kora, WDisp.ban, ADD);
      }
      break;
    case _KEY_I :
      if (WDisp.line != -1) {
	to_multi(cus_mode, WDisp.current, WDisp.kora, WDisp.ban, INSERT);
      }
      break;
    case _KEY_K :
      if (WDisp.line != -1) {
	kill_G(cus_mode, WDisp.current);
	sp_disp(cus_mode, WDisp.top, WDisp.line, WDisp.kora, WDisp.ban);
      }
      break;
    case _KEY_D :
      if (WDisp.line != -1 && (
	  ((WDisp.kora == KEYS) && dk_blocks[WDisp.current].keysu > 1) ||
	    ((WDisp.kora == ACTS) && dk_blocks[WDisp.current].actsu > 1))) {
		delete_obj(cus_mode, WDisp.current, WDisp.kora, WDisp.ban);
	sp_disp(cus_mode, WDisp.top, WDisp.line, WDisp.kora, WDisp.ban);
      }
      break;
    case _KEY_U :
      if (undo_status == _KEY_K) {
	kill_UNDO(cus_mode);
	sp_disp(cus_mode, WDisp.top, WDisp.line, WDisp.kora, WDisp.ban);
      }
      else if (undo_status == '\n'|| undo_status == _KEY_D ||
	       undo_status == _KEY_A || undo_status == _KEY_I) {
	from_UNDO();
	sp_disp(cus_mode, WDisp.top, WDisp.line, WDisp.kora, WDisp.ban);
      }
      break;
    case '\n' :
      if (WDisp.kora == KEYS) {
	keyIn(cus_mode, 0);
      }
      else if (WDisp.kora == ACTS) {
	actIn(cus_mode, 0);
      }
      werase(kutl_win);
      disp_block(kutl_win, 0, L_MARGIN, kh_mess, -1, -1);
      wrefresh(kutl_win);
      sp_disp(cus_mode, WDisp.top, WDisp.line, WDisp.kora, WDisp.ban);
      break;
    }
    wrefresh(disp_win);
  }
  fin_dk_blocks();
}

keyIn(cus_mode, status)
int cus_mode, status;
{
  char prev_char[20], *p;
  int  y, x, i;
  int  p_point = L_MARGIN, c_location = 0, t_location = 0;
  chtype c;

/*  echo();*/
  for(;;) {
    nocbreak();
    clear();
    touchwin(kutl_win);
    werase(kutl_win);
    wrefresh(kutl_win);
    mvwaddstr(kutl_win, 0, 10, "キーを選択してリターンキーを押してください");
    disp_block(kutl_win, 2, L_MARGIN, keyLine, 0, 8);
    current_print2(kutl_win, 2 + c_location, p_point,
		  keyLine[c_location], prev_char);
    cbreak();
    while((c=wgetch( kutl_win )) != '\n') { /* SVR4 curses */
      switch(c) {
      case KEY_LEFT :
      case _KEY_C_B :
	mvwaddstr(kutl_win, c_location +2, p_point, prev_char);
	if (p_point == L_MARGIN) {
	  if (!c_location) {
	    if (!t_location)
	      break;
	    t_location--;
	    disp_block(kutl_win, 2, L_MARGIN, keyLine, t_location -c_location,
		       t_location -c_location +8);
	    for (p = keyLine[t_location], i= 0; p[i]; i++);
	    for (;p[i] != ' '; i--);
	    p_point = ++i + L_MARGIN;
	    current_print2(kutl_win, c_location +2, p_point, p + i, prev_char);
	    break;
	  }
	  t_location--;
	  c_location--;
	  for (p = keyLine[t_location], i = 0; p[i]; i++);
	  for (; p[i] != ' '; i--);
	  p_point = ++i + L_MARGIN;
	  current_print2(kutl_win, c_location +2, p_point, p + i, prev_char);
	  break;
	}
	else {
	  for (p = keyLine[t_location], i = p_point - L_MARGIN - KANKAKU -1;
	       (p[i] != ' ') && (i >= 0); i--);
	  p_point = ++i + L_MARGIN;
	  current_print2(kutl_win, c_location +2, p_point, p + i, prev_char);
	  break;
	}
      case KEY_RIGHT :
      case _KEY_C_F :
	mvwaddstr(kutl_win, c_location + 2, p_point, prev_char);
	getyx(kutl_win,y ,x);
	p = keyLine[t_location];
	if (!*(p + x - L_MARGIN)) {
	  if (!keyLine[t_location +1]) {
	    current_print2(kutl_win, c_location +2, p_point,
			   prev_char, prev_char);
	    break;
	  }
	  t_location++;
	  p_point = L_MARGIN;
	  if (c_location == 8) {
	    disp_block(kutl_win, 2, L_MARGIN, keyLine, t_location -c_location,
		       t_location -c_location +8);
	    current_print2(kutl_win, c_location +2, p_point,
			   keyLine[t_location], prev_char);
	  }
	  else {
	    c_location++;
	    current_print2(kutl_win, c_location +2, p_point,
			   keyLine[t_location], prev_char);
	  }
	  break;
	}
	else {
	  p_point = x + KANKAKU;
	  current_print2(kutl_win, c_location +2, p_point,
			 p + p_point - L_MARGIN, prev_char);
	  break;
	}
      case KEY_UP :
      case _KEY_C_P :
	if (!t_location)
	  break;
	t_location--;
	if (!c_location) {
	  disp_block(kutl_win, 2, L_MARGIN, keyLine, t_location -c_location,
		     t_location -c_location +8);
	  for (p = keyLine[t_location], i = 0; p[i]; i++);
	  for (;p[i] != ' '; i--);
	  if (p_point >= ++i + L_MARGIN) {
	    p_point = i + L_MARGIN;
	    current_print2(kutl_win, c_location +2, p_point, p + i, prev_char);
	    break;
	  }
	  if (p[p_point - L_MARGIN] == ' ') {
	    for (i = p_point - L_MARGIN; p[i] == ' '; i++);
	    p_point = i + L_MARGIN;
	    current_print2(kutl_win, c_location +2, p_point,p + i, prev_char);
	    break;
	  }
	  else {
	    for(i = p_point - L_MARGIN; p[i] != ' ' && i >= 0;i--);
	    p_point = ++i + L_MARGIN;
	    current_print2(kutl_win, c_location +2, p_point,p + i, prev_char);
	    break;
	  }
	}
	mvwaddstr(kutl_win, c_location +2, p_point, prev_char);
	c_location--;
	for (p = keyLine[t_location], i = 0; p[i]; i++);
	for (;p[i] != ' '; i--);
	if(p_point >= ++i + L_MARGIN) {
	  p_point = i + L_MARGIN;
	  current_print2(kutl_win, c_location +2, p_point,p + i, prev_char);
	  break;
	}
	if (p[p_point - L_MARGIN] == ' ') {
	  for (i = p_point - L_MARGIN; p[i] == ' '; i++);
	  p_point = i + L_MARGIN;
	  current_print2(kutl_win, c_location +2, p_point,p + i, prev_char);
	  break;
	}
        else {
	  for(i = p_point - L_MARGIN; p[i] != ' ' && i >= 0;i--);
	  p_point = ++i + L_MARGIN;
	  current_print2(kutl_win, c_location +2, p_point,p + i, prev_char);
	  break;
	}
      case KEY_DOWN :
      case _KEY_C_N :
	if (!keyLine[t_location +1])
	  break;
	t_location++;
	if (c_location == 8) {
	  disp_block(kutl_win, 2, L_MARGIN, keyLine, t_location -c_location, 
		     t_location -c_location +8);
	  for (p = keyLine[t_location], i = 0; p[i]; i++);
	  for (;p[i] != ' '; i--);
	  if(p_point >= ++i + L_MARGIN) {
	    p_point = i + L_MARGIN;
	    current_print2(kutl_win, c_location +2, p_point,p + i, prev_char);
	    break;
	  }
	  if (p[p_point - L_MARGIN] == ' ') {
	    for (i = p_point - L_MARGIN; p[i] == ' '; i++);
	    p_point = i + L_MARGIN;
	    current_print2(kutl_win, c_location +2, p_point,p + i, prev_char);
	    break;
	  }
	  else {
	    for(i = p_point - L_MARGIN; p[i] != ' ' && i >= 0;i--);
	    p_point = ++i + L_MARGIN;
	    current_print2(kutl_win, c_location +2, p_point,p + i, prev_char);
	    break;
	  }
	}
	mvwaddstr(kutl_win, c_location +2, p_point, prev_char);
	c_location++;
	for (p = keyLine[t_location], i = 0; p[i]; i++);
	for (;p[i] != ' '; i--);
	if(p_point >= ++i + L_MARGIN) {
	  p_point = i + L_MARGIN;
	  current_print2(kutl_win, c_location +2, p_point,p + i, prev_char);
	  break;
	}
	if (p[p_point - L_MARGIN] == ' ') {
	  for (i = p_point - L_MARGIN; p[i] == ' '; i++);
	  p_point = i + L_MARGIN;
	  current_print2(kutl_win, c_location +2, p_point,p + i, prev_char);
	  break;
	}
        else {
	  for(i = p_point - L_MARGIN; p[i] != ' ' && i >= 0;i--);
	  p_point = ++i + L_MARGIN;
	  current_print2(kutl_win, c_location +2, p_point,p + i, prev_char);
	  break;
	}
      case _KEY_ESC :
      case _KEY_Q :
	return -1;
      }
    }
    if (c == '\n') {
      if (WDisp.line == -1) {
	shinki(cus_mode, KEYS, scc(prev_char));
      }
      else if (!status){
	okikae(cus_mode, WDisp.current, KEYS, WDisp.ban, scc(prev_char));
      }
    }
    return  scc(prev_char);
  }
}

actIn(cus_mode, status)
int cus_mode, status;
{
  int  y, x; 
  int  current_point = 0, location,
       c_location = 0, t_location = 0;
  chtype c;

/*  echo();*/
  for(;;) {
    nocbreak();
    clear();
    touchwin(kutl_win);
    werase(kutl_win);
    wrefresh(kutl_win);
    mvwaddstr(kutl_win, 0, 10, "機能を選択してリターンキーを押してください。");
    disp_block(kutl_win, 2, L_MARGIN, actLine, 0, 8);
    current_print(kutl_win, 2 + c_location, L_MARGIN, actLine[c_location]);
    cbreak();
    while((c=wgetch( kutl_win )) != '\n') { /* SVR4 curses */
      switch(c) {
      case KEY_DOWN :
      case _KEY_C_N :
	if (!actLine[t_location +1]) {
	  break;
	}
	if (c_location == 8) {
	  t_location++;
	  disp_block(kutl_win, 2, L_MARGIN, actLine, t_location -8,
		     t_location -1);
	  current_print(kutl_win, 2 +c_location, L_MARGIN,
			actLine[t_location]);
	  wclrtoeol(kutl_win);
	  break;
	}
	else {
	  c_location++;
	  t_location++;
	  mvwaddstr(kutl_win, 1 +c_location, L_MARGIN,
		    actLine[t_location -1]);
	  current_print(kutl_win, 2 +c_location, L_MARGIN,
			actLine[t_location]);
	  wclrtoeol(kutl_win);
	  break;
	}
      case KEY_UP :
      case _KEY_C_P :
	if (!(t_location) && !c_location) {
	  break;
	}
	if (!c_location) {
	  t_location--;
	  disp_block(kutl_win, 3, L_MARGIN, actLine, t_location +1,
		     t_location +8);
	  current_print(kutl_win, 2 +c_location, L_MARGIN,
			actLine[t_location]);
	  wclrtoeol(kutl_win);
	  break;
	}
	else {
	  c_location--;
	  t_location--;
	  mvwaddstr(kutl_win, 3 +c_location, L_MARGIN,
		    actLine[t_location +1]);
	  current_print(kutl_win, 2 +c_location, L_MARGIN,
			actLine[t_location]);
	  wclrtoeol(kutl_win);
	  break;
	}
      case _KEY_ESC :
      case _KEY_Q :
	return -1;
      }
      wrefresh(kutl_win);
    }
    if (c == '\n') {
      if (t_location == 40 || t_location == 41) {
	t_location +=2; /* NextKouhoとPrevKouhoとなるようにする。 */
      }
      if (WDisp.line == -1) {
	shinki(cus_mode, ACTS, t_location +2);
      }
      else if (!status) {
	okikae(cus_mode, WDisp.current, ACTS, WDisp.ban, t_location +2);
      }
    }
    return (t_location +2);
  }
}

keyCustom()
{
  int  y, x, location, c_location = 0;
  chtype c;

/*  echo();*/
  for(;;) {
    nocbreak();
    
    touchwin(kctm_win);
    werase(kctm_win);
    wrefresh(kctm_win);
    mvwaddstr(kctm_win, 1, 10, "☆キーのカスタマイズ☆");
    for (location = 0; key_menu[location]; location++)
      mvwaddstr(kctm_win, 3 + location, 10, key_menu[location]);
    current_print(kctm_win, 3 + c_location, 10, key_menu[c_location]);
    cbreak();
    while((c=wgetch( kctm_win )) != '\n') {
      switch(c) {
      case KEY_DOWN :
      case _KEY_C_N :
	if (c_location == location -1)
	  break;
	mvwaddstr(kctm_win, 3 + c_location, 10, key_menu[c_location]);
	c_location++;
	current_print(kctm_win, 3 + c_location, 10, key_menu[c_location]);
	break;
      case KEY_UP :
      case _KEY_C_P :
	if (c_location == 0)
	  break;
	mvwaddstr(kctm_win, 3 + c_location, 10, key_menu[c_location]);
	c_location--;
	current_print(kctm_win, 3 + c_location, 10, key_menu[c_location]);
	break;
      case _KEY_SPACE :
      case _KEY_ESC :
      case _KEY_Q :
	return;
      }
    }
    dispIn(c_location);
    touroku_start = -1;
    cc_keys_len = cc_acts_len = 0;
  }
}

char *hozon_mode_mei;

modeNameDisplay(page)
int    page;
{
  WINDOW *win;
  char   **menu, print_mode[512], undo_mode[512], undo_sts;
  int    location, c_location = 0, u_location = -1;
  chtype c;

  if (page == 1) {
    win = mode1_win;
    menu = mode1_menu;
  }
  else {
    win = mode2_win;
    menu = mode2_menu;
  }

  mvwaddstr(win, 1, 10, "☆モードの表示文字列のカスタマイズ☆");
  wprintw(win,"      %d頁\n",page);
  for (location = 0; menu[location]; location++)
    mvwaddstr(win, 3 + location, 10, menu[location]);
  current_print(win, 3 + c_location, 10, menu[c_location]);
  wmove(err_win, 0, 0);
  if (page == 1) {
    if (mode_mei[0])
      strncpy(print_mode, mode_mei[0], 255);
    else {
      if (null_mode[0])
	strncpy(print_mode,"変わらない状態",255);
      else
	strncpy(print_mode,"カスタマイズしていない状態",255);
    }
  }
  else if (page == 2) {
    if (mode_mei[mm2_set[0]])
      strncpy(print_mode, mode_mei[mm2_set[0]], 255);
    else {
      if (null_mode[mm2_set[0]])
	strncpy(print_mode,"変わらない状態",255);
      else
	strncpy(print_mode,"カスタマイズしていない状態",255);
    }
  }
  werase(err_win);
  wprintw(err_win,"現在、このモードの文字列は%sになっています。", print_mode);
  mvwaddstr(err_win, 2, 0,"リターンキーを押すと文字列を\
カスタマイズできます。\nスペースキーでモードの表示文字列のカ\
スタマイズを終了します。");
  wrefresh(err_win);
  cbreak();
  for (;c=wgetch( err_win );) {
    switch(c) {
    case KEY_DOWN :
    case _KEY_C_N :
      if (c_location == location -1 ) {
	if (page == 1) {
	  return NEXTPAGE;
	}
	if (page == 2) {
	  break;
	}
      }
      mvwaddstr(win, 3 + c_location, 10, menu[c_location]);
      c_location++;
      current_print(win, 3 + c_location, 10, menu[c_location]);
      if (page == 1) {
	if (mode_mei[mm1_set[c_location]])	  
	  strncpy(print_mode, mode_mei[mm1_set[c_location]], 255);
	else {
	  if (null_mode[mm1_set[c_location]])
	    strncpy(print_mode,"変わらない状態",255);	    
	  else
	    strncpy(print_mode,"カスタマイズしていない状態",255);
	}
      }
      else if (page == 2) {
	if (mode_mei[mm2_set[c_location]])
	  strncpy(print_mode, mode_mei[mm2_set[c_location]], 255);
	else {
	  if (null_mode[mm2_set[c_location]])
	    strncpy(print_mode,"変わらない状態", 255);
	  else
	    strncpy(print_mode,"カスタマイズしていない状態", 255);
	}
      }
      wmove(err_win, 1, 0);
      wclrtoeol(err_win);
      wmove(err_win, 0, 0);
      wprintw(err_win,"現在、このモードの文字列は%sになっています。",
	      print_mode);
      wclrtoeol(err_win);
      wrefresh(err_win);
      break;
    case KEY_UP :
    case _KEY_C_P :
      if (c_location == 0) {
	if (page == 2)
	  return PREVPAGE;
	break;
      }
      mvwaddstr(win, 3 + c_location, 10, menu[c_location]);
      c_location--;
      current_print(win, 3 + c_location, 10, menu[c_location]);
      if (page == 1) {
	if (mode_mei[mm1_set[c_location]])	  
	  strncpy(print_mode, mode_mei[mm1_set[c_location]], 255);
	else {
	  if (null_mode[mm1_set[c_location]])
	    strncpy(print_mode,"変わらない状態", 255);
	  else
	    strncpy(print_mode,"カスタマイズしていない状態", 255);
	}
      }
      else if (page == 2) {
	if (mode_mei[mm2_set[c_location]])
	  strncpy(print_mode, mode_mei[mm2_set[c_location]], 255);
	else {
	  if (null_mode[mm2_set[c_location]])
	    strncpy(print_mode,"変わらない状態",31);
	  else
	    strncpy(print_mode,"カスタマイズしていない状態", 255);
	}
      }
      wmove(err_win, 1, 0);
      wclrtoeol(err_win);
      wmove(err_win, 0, 0);
      wprintw(err_win,"現在、このモードの文字列は%sになっています。",
	      print_mode);
      wclrtoeol(err_win);
      wrefresh(err_win);
      break;
    case _KEY_D :
      wmove(err_win, 1, 0);
      wclrtoeol(err_win);
      wmove(err_win, 0, 0);
      if (page == 1) {
	if (mode_mei[mm1_set[c_location]]) {
	  u_location = c_location;
	  strcpy(undo_mode, print_mode);
	  undo_sts = STR_UNDO;
	  mode_mei[mm1_set[c_location]] = NULL;
	  null_mode[mm1_set[c_location]] = 0;
	  wclrtoeol(err_win);
	  wprintw(err_win,"このモードの文字列、%sを消去しました。", print_mode);
	}
	else if (null_mode[mm1_set[c_location]]) {
	  u_location = c_location;
          undo_sts = NULL_UNDO;
	  null_mode[mm1_set[c_location]] = 0;
	  wclrtoeol(err_win);
	  wprintw(err_win,"このモードの文字列を初期値に戻しました。");
	}	  
      }
      else if (page == 2) {
	if (mode_mei[mm2_set[c_location]]) {
	  u_location = c_location;
	  strcpy(undo_mode, print_mode);
	  undo_sts = STR_UNDO;
	  mode_mei[mm2_set[c_location]] = NULL;
	  null_mode[mm2_set[c_location]] = 0;
	  wclrtoeol(err_win);
	  wprintw(err_win,"このモードの文字列、%sを消去しました。", print_mode);
	}
	else if (null_mode[mm2_set[c_location]]) {
	  u_location = c_location;
          undo_sts = NULL_UNDO;
	  null_mode[mm2_set[c_location]] = 0;
	  wprintw(err_win,"このモードの文字列を初期値に戻しました。");
	  wclrtoeol(err_win);
	}
      }
      wrefresh(err_win);
      break;
    case _KEY_U :
      if (u_location == c_location) {
	wmove(err_win, 1, 0);
	wclrtoeol(err_win);
	wmove(err_win, 0, 0);
	if (page == 1) {
	  if (!mode_mei[mm1_set[c_location]]) {
	    if (!undo_sts) {
	      mode_mei[mm1_set[c_location]] = 
		(char *)malloc(strlen(undo_mode) +1);
	      strcpy(mode_mei[mm1_set[c_location]], undo_mode);
	      wprintw(err_win,"文字列、%sをこのモードに復帰させました。",
		      undo_mode);
	      wclrtoeol(err_win);
	    }
	    else if (undo_sts == NULL_UNDO) {
	      wprintw(err_win,"文字列が変わらない状態に戻しました。",
		      undo_mode);
	      wclrtoeol(err_win);
	      null_mode[mm1_set[c_location]] = 1;
	    }
	    else if (undo_sts == NON_UNDO) {
	      wprintw(err_win,"カスタマイズしていない状態に戻しました。",
		      undo_mode);
	      wclrtoeol(err_win);
	      null_mode[mm1_set[c_location]] = 0;
	    }
	  }
	}
	else if (page == 2) {
	  if (!mode_mei[mm2_set[c_location]]) {
	    if (!undo_sts) {
	      mode_mei[mm2_set[c_location]] = 
		(char *)malloc(strlen(undo_mode) +1);
	      strcpy(mode_mei[mm2_set[c_location]], undo_mode);
	      wprintw(err_win,"文字列、%sをこのモードに復帰させました。",
		      undo_mode);
	      wclrtoeol(err_win);
	    }
	    else if (undo_sts == NULL_UNDO) {
	      wprintw(err_win,"文字列が変わらない状態に戻しました。",
		      undo_mode);
	      wclrtoeol(err_win);
	      null_mode[mm2_set[c_location]] = 1;
	    }
	    else if (undo_sts == NON_UNDO) {
	      wprintw(err_win,"カスタマイズしていない状態に戻しました。",
		      undo_mode);
	      wclrtoeol(err_win);
	      null_mode[mm2_set[c_location]] = 0;
	    }
	  }
	}
	wrefresh(err_win);
      }	
      break;
    case '\n' :
      clr_cul_to_end(err_win, 3, 0);
      mvwaddstr(err_win, 2, 0,"カスタマイズしたい文字列を入力してください :");
      wclrtoeol(err_win);
      wrefresh(err_win);
      u_location = c_location;
      strcpy(undo_mode, print_mode);
      echo();
      wgetstr(err_win, print_mode);
      noecho();
      werase(err_win);
      wmove(err_win, 0, 0);
      if (strlen(print_mode)) {
	hozon_mode_mei = (char *)malloc(strlen(print_mode) +1);
	strcpy(hozon_mode_mei, print_mode);
	if (page == 1)
	  mode_mei[mm1_set[c_location]] = hozon_mode_mei;
	else if (page == 2)
	  mode_mei[mm2_set[c_location]] = hozon_mode_mei;
      }
      else {
	if (page == 1) {
	  if (mode_mei[mm1_set[c_location]]) {
	    strcpy(print_mode, mode_mei[mm1_set[c_location]]);
	  }
	  else {
	    if (null_mode[mm1_set[c_location]])
	      strncpy(print_mode,"変わらない状態",31);	      
	    else
	      strncpy(print_mode,"カスタマイズしていない状態",31);
	  }
	}
	else if (page == 2) {
	  if (mode_mei[mm2_set[c_location]]) {
	    strcpy(print_mode, mode_mei[mm2_set[c_location]]);
	  }
	  else {
	    if (null_mode[mm1_set[c_location]])
	      strncpy(print_mode,"変わらない状態",31);
	    else
	      strncpy(print_mode,"カスタマイズしていない状態",31);
	  }
	}
      }
      if (!strlen(print_mode))
	strncpy(print_mode,"カスタマイズしていない状態",31);
      wprintw(err_win,"現在、このモードの文字列は%sになっています。",
	      print_mode);
      wclrtoeol(err_win);
      mvwaddstr(err_win, 2, 0,"リターンキーを押すと文字列を\
カスタマイズできます。\nスペースキーでモードの表示文字列のカ\
スタマイズを終了します。");
      wclrtobot(err_win);
      wrefresh(err_win);
      break;
    case _KEY_N :
      u_location = c_location;
      strcpy(undo_mode, print_mode);
      undo_sts = STR_UNDO;
      if (page == 1) {
	if (!mode_mei[mm1_set[c_location]]) {
	  if (null_mode[mm1_set[c_location]]) {
	    break;
	  }
	  else {
	    undo_sts = NON_UNDO;
	  }
	}
	mode_mei[mm1_set[c_location]] = NULL;
	null_mode[mm1_set[c_location]] = 1;
      }
      else if (page ==2) {
	if (!mode_mei[mm2_set[c_location]]) {
	  if (null_mode[mm2_set[c_location]]) {
	    break;
	  }
	  else {
	    undo_sts = NON_UNDO;
	  }
	}
	mode_mei[mm2_set[c_location]] = NULL;
	null_mode[mm2_set[c_location]] = 1;
      }
      wmove(err_win, 1, 0);
      wclrtoeol(err_win);
      wmove(err_win, 0, 0);
      wprintw(err_win,"このモードの文字列を変わらなくしました。");
      wclrtoeol(err_win);
      wrefresh(err_win);
      break;
    case _KEY_SPACE :
    case _KEY_ESC :
    case _KEY_Q :
      return DONOT;
    }
  }
  return DONOT;
}

modeName()
{
  int  y, x, page = 1; 
  int  location, c_location = 0;
  chtype c;


/*  echo();*/
  for(;;) {
    nocbreak();
    
    for(;page = modeNameDisplay(page););
    werase(err_win);
    wrefresh(err_win);
    return;
  }
}

on_off(win,ctm)
WINDOW  *win;
char ctm;
{
  int y, x;

  getyx(win, y, x);
  if (ctm == ON) {
    wattron(win, A_REVERSE);
    wprintw(win,"%s","O N");
    wattroff(win, A_REVERSE);
    wprintw(win,"%s","    OFF");
    wmove(win, y, x+3); 
  }
  else {
    wprintw(win,"%s","O N    ");
    wattron(win, A_REVERSE);
    wprintw(win,"%s","OFF");
    wattroff(win, A_REVERSE);
  }
  wrefresh(win);
}

etcCustom()
{
  int  y, x, 
       location, c_location = 0;
  chtype c;

/*  echo();*/
  for(;;) {
    nocbreak();
    
    touchwin(etc_win);
    werase(etc_win);
    wrefresh(etc_win);
    mvwaddstr(etc_win, 1, 10, "☆その他のカスタマイズ☆");
    for (location = 0; etc_menu[location]; location++) {
      wmove(etc_win, 3 +location, 34);
      on_off(etc_win, etc_ctm[location]);
      mvwaddstr(etc_win, 3 + location, 10, etc_menu[location]);
    }
    mvwaddstr(etc_win, 3 + c_location, 34, mode_ichiran[InitialMode]);
    wmove(etc_win, 3 + location -1, 34);
    wprintw(etc_win, "%d回        ", kouho_threshold);
    current_print(etc_win, 3 + c_location, 10, etc_menu[c_location]);
    werase(_etc_win);
    mvwaddstr(_etc_win, 0, 0, etc_manual[c_location]);
    wrefresh(_etc_win);
    cbreak();
    getyx(etc_win, y, x);
    while((c=wgetch( etc_win )) != '\n' && c != _KEY_SPACE && c != _KEY_ESC &&
	  c != _KEY_Q) {
      switch(c) {
      case KEY_DOWN :
      case _KEY_C_N :
	if (c_location == location -1)
	  break;
	mvwaddstr(etc_win, 3 + c_location, 10, etc_menu[c_location]);
	c_location++;
	current_print(etc_win, 3 + c_location, 10, etc_menu[c_location]);
	werase(_etc_win);
	mvwaddstr(_etc_win, 0, 0, etc_manual[c_location]);
	wrefresh(_etc_win);
	break;
      case KEY_UP :
      case _KEY_C_P :
	if (c_location == 0)
	  break;
	mvwaddstr(etc_win, 3 + c_location, 10, etc_menu[c_location]);
	c_location--;
	current_print(etc_win, 3 + c_location, 10, etc_menu[c_location]);
	werase(_etc_win);
	mvwaddstr(_etc_win, 0, 0, etc_manual[c_location]);
	wrefresh(_etc_win);
	break;
      case KEY_LEFT :
      case _KEY_C_B :
	if ((!c_location && !junban) ||
	    (c_location == location -1 && !kouho_threshold))
	  break;
	else if (etc_ctm[c_location] == ON)
	  break;
	else if (c_location > 0 && c_location < location -1) {
	  etc_ctm[c_location] = ON;
	  etc_action(c_location, ON);
	  wmove(etc_win, 3 +c_location, 34);
	  on_off(etc_win, etc_ctm[c_location]);
	}
	else if (!c_location) {
	  wmove(etc_win, 3, 34);
	  wclrtoeol(etc_win);
	  InitialMode = mode_set[--junban];
	  mvwaddstr(etc_win, 3, 34, mode_ichiran[InitialMode]);
	  wrefresh(etc_win);
	}
	else {
	  kouho_threshold--;
	  wmove(etc_win, 3 + location -1, 34);
	  wclrtoeol(etc_win);
	  wprintw(etc_win, "%d回        ", kouho_threshold);
	  wrefresh(etc_win);
	}
	break;
      case KEY_RIGHT :
      case _KEY_C_F :
	if ((!c_location && junban == 11) 
	    || (c_location == location -1 && kouho_threshold == NHENKAN_MAX)) 
	  break;
	else if(etc_ctm[c_location] == OFF)
	  break;
	else if (c_location > 0 && c_location < location -1) {
	  etc_ctm[c_location] = OFF;
	  etc_action(c_location, OFF);
	  wmove(etc_win, 3 +c_location, 34);
	  on_off(etc_win, etc_ctm[c_location]);
	}
	else if (!c_location) {
	  wmove(etc_win, 3, 34);
	  wclrtoeol(etc_win);
	  InitialMode = mode_set[++junban];
	  mvwaddstr(etc_win, 3, 34, mode_ichiran[InitialMode]);
	  wrefresh(etc_win);
	}
	else {
	  kouho_threshold++;
	  wmove(etc_win, 3 + location -1, 34);
	  wclrtoeol(etc_win);
	  wprintw(etc_win, "%d回        ", kouho_threshold);
	  wrefresh(etc_win);
	}
	break;
      }
    }
    werase(_etc_win);
    wrefresh(_etc_win);
    return;
  }
}

char i_file[128];

get_save_file()
{
  char *p, *getenv();
  FILE *f;
  int home_iroha_exist = 0;
  if (initFileSpecified) {
    strncpy(i_file, initFileSpecified,127);
    return;
  }
  if (p = getenv("IROHAFILE")) {
    strncpy(i_file, p, 127);
    return;
  }
  if (p = getenv("HOME")) {
    strncpy(i_file, p, 120);
    strcat(i_file, "/.iroha");
    return;
  }
}

santaku(win, y, x, which, a, b, c)
WINDOW *win;
int y, x, which;
char *a, *b, *c;
{
  wmove(win, y, x);
  wprintw(win, "%s  %s  %s\n", a, b, c);
  switch(which) {
  case 1:
    current_print(win, y, x, a);
    wprintw(win, "  %s  %s\n", b, c);
    return;
  case 2:
    wmove(win, y, x);
    wprintw(win, "%s  ", a);
    current_print(win, y, x + strlen(a) + 2, b);
    wprintw(win, "  %s", c);
    return;
  case 3:
    wmove(win, y, x);
    wprintw(win, "%s  %s  ", a, b);
    current_print(win, y, x + strlen(a) + strlen(b) +4, c);
    return;
  }
}

endCustom()
{
  FILE *f_save;
  int sentaku = 2;
  chtype c;

  get_save_file();
  mvwaddstr(end_win, 5, L_MARGIN, "カスタマイズの結果を保存しますか？");
  santaku(end_win, 7, L_MARGIN, sentaku, "保存後終了", "保存せず終了", "取消");
  cbreak();
  for(;;) {
    c = wgetch( end_win );
    switch(c) {
    case KEY_LEFT :
    case _KEY_C_B :
      if (sentaku == 1)
	break;
      else {
	sentaku--;
	santaku(end_win, 7, L_MARGIN, sentaku,
		"保存後終了", "保存せず終了", "取消");
      }
      break;
    case KEY_RIGHT :
    case _KEY_C_F :
      if (sentaku == 3)
	break;
      else {
	sentaku++;
	santaku(end_win, 7, L_MARGIN, sentaku,
		"保存後終了", "保存せず終了", "取消");
      }
      break;
    case _KEY_U :
      return;
    case '\n' :
      switch(sentaku) {
      case 1 :
	f_save = fopen(i_file, "w");
	if (f_save) {
	  write_iroha(f_save);
	  fclose(f_save);
	}
	else {
	  wmove(err_win, 0, 0);
	  err_word(
	  "%sに保存できませんでした。\n別のファイルに保存してください。",i_file);
	  beep();
	  break;
	}
	endwin();
	exit(0);
      case 2 :
	endwin();
	exit(0);
      case 3 :
	return;
      }
    }
  }
}

initctm()
{
  etc_ctm[0]  =  MID; /* initialMode */
  etc_ctm[1] =  CursorWrap;
  etc_ctm[2] =  HexkeySelect;  /* numericalKeySelect */
  etc_ctm[3] =  SelectDirect;
  etc_ctm[4] =  BunsetsuKugiri;
  etc_ctm[5] =  IchitaroMove; /* characterBaseMove*/
  etc_ctm[6] =  ReverseOnlyCursor; /* reverseWidely */
  etc_ctm[7] =  QuitIchiranIfEnd; /* QuitIfEndOfIchiran */
  etc_ctm[8] =  BreakIntoRoman;
  etc_ctm[9] =  Gakushu;
  etc_ctm[10] = stayAfterValidate;
  etc_ctm[11] = kakuteiIfEndOfBunsetsu;
  etc_ctm[12] = gramaticalQuestion;
  etc_ctm[13] = MID; /* nHenkanForIchiran */
  etc_ctm[14] = (char)NULL;

  if (kouho_threshold > 9999)
    kouho_threshold = 9999;
}

static int (*func[])() = {
/* カスタマイズファイルの読み込み   */  loadFile,
/* カスタマイズファイルへの保存     */  saveFile,
/* 使用する辞書の設定               */  confDic,
/* キーのカスタマイズ               */  keyCustom,
/*モードの表示文字列のカスタマイズ" */  modeName,
/* その他のカスタマイズ             */  etcCustom,
/* 終了                             */  endCustom,
};

main(argc, argv)
int argc;
char *argv[];
{
  void root_ctm(), int_exit();
  int proc_delete_key();
  char *term;

#ifdef SVR4
  setlocale(LC_ALL,"");
#endif

  term = getenv("TERM");
  if (!term) {
    fprintf(stderr, "環境変数:TERMが設定されていませんでした。\n");
    exit(1);
  }
  trm = newterm(term , stdout, stdin);
  if (!trm) {
    fprintf(stderr, "TERM\"%s\"は認識できませんでした。\n", term);
    exit(1);
  }
  set_term(trm);
  signal(SIGINT, proc_delete_key);
  signal(SIGQUIT, int_exit);
  keypad(stdscr, TRUE);

  scrollok(stdscr,TRUE);
  cbreak();
  noecho();
  
  if (LINES < 23 || COLS < 80) {
    endwin();
    fprintf(stderr,"    画面を縦２３行以上、横８０文字以上に設定\n");
    fprintf(stderr," して 、もう１度やり直してくだいさい。\n");
    exit(0);
  }

  init_mode_mei();
  if (argc > 1) {
    initFileSpecified = (char *)malloc(strlen(argv[1])+1);
    if (initFileSpecified)
      strcpy(initFileSpecified, argv[1]);
  }  
  parse();
  initctm();
  base_win = newwin(18, COLS, 0, 0);
  err_win = newwin(4, COLS -10, 18, 10);
  load_win = newwin(18, COLS, 0, 0);
  save_win = newwin(18, COLS, 0, 0);
  dic_win = newwin(10, COLS, 0, 0);
  ddic_win = newwin(8, COLS, 10, 0);
  dddic_win = newwin(5, COLS, 18, 0); 
  kctm_win = newwin(23, COLS, 0, 0);
  disp_win = newwin(12, COLS, 0, 0);
  kutl_win = newwin(11, COLS, 12, 0);
  mode1_win = newwin(18, COLS, 0, 0);
  mode2_win = newwin(18, COLS, 0, 0);
  etc_win = newwin(18, COLS, 0, 0);
  _etc_win = newwin(4, COLS -20, 18, 10);
  end_win = newwin(18, COLS, 0, 0);

  keypad(base_win, TRUE);
  keypad(err_win, TRUE);
  keypad(load_win, TRUE);
  keypad(save_win, TRUE);
  keypad(dic_win, TRUE);
  keypad(ddic_win, TRUE);
  keypad(dddic_win, TRUE);
  keypad(kctm_win, TRUE);
  keypad(disp_win, TRUE);
  keypad(kutl_win, TRUE);
  keypad(mode1_win, TRUE);
  keypad(mode2_win, TRUE);
  keypad(etc_win, TRUE);
  keypad(_etc_win, TRUE);
  keypad(end_win, TRUE);

  root_ctm();
  
  refresh();
  endwin();
  exit(0);
}

void
root_ctm()
{
  int    x, y, location, c_location = 0;
  chtype c;

/*  echo();*/
  for(;;) {
    nocbreak();
    
    mvwaddstr(base_win, 1, 10, "☆☆☆カスタマイズツール☆☆☆");
    for (location = 0; base_menu[location]; location++)
      mvwaddstr(base_win, (3 + location * 2), 10, base_menu[location]);
    current_print(base_win, (3 +c_location *2), 10,base_menu[c_location]);
    
    if (*err_mess) {
      err_word("ファイル:%sはオープンできませんでした。",err_mess);
      beep();
      *err_mess = '\0';
    }
    cbreak();
    while((c=wgetch( base_win )) != '\n') {
      switch(c) {
      case KEY_DOWN :
      case _KEY_C_N :
	if (c_location == location -1)
	  break;
	mvwaddstr(base_win, (3 + c_location * 2), 10, base_menu[c_location]);
	c_location++;
	current_print(base_win, (3 +c_location *2), 10,base_menu[c_location]);
	break;
      case KEY_UP :
      case _KEY_C_P :	
	if (c_location == 0)
	  break;
	mvwaddstr(base_win, (3 + c_location * 2), 10, base_menu[c_location]);
	c_location--;
	current_print(base_win, (3 +c_location *2), 10,base_menu[c_location]);
	break;
      }
    }
    werase(err_win);
    wrefresh(err_win);
    (*func[c_location])();
  }
}

void
int_exit()
{
  endwin();
  exit(0);
}

proc_delete_key(sig)
int sig;
/* ARGSUSED */
{
  static unsigned char counter = 0;
  static long prev_time = 0;
  long cur_time;

  time(&cur_time);

  if (cur_time - prev_time < 10) {
    counter++;
  }
  else {
    counter = 1;
  }
  prev_time = cur_time;
  if (counter > 2) {
    int_exit();
  }
  else {
    signal(SIGINT, proc_delete_key);
    return 0;
  }
}
