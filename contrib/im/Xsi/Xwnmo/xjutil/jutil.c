/*
 * $Id: jutil.c,v 1.3 1991/09/16 21:35:19 ohm Exp $
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
/* jisho utility routine for otasuke (user interface) process */

#include <stdio.h>
#include <X11/Xos.h>
#include "jslib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "config.h"
#include "xext.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"

int (*jutil_table[2][3])() = {
  {kworddel, kworduse, kwordcom},
  {kdicdel, kdicuse, kdiccom}
};

void
if_dead_disconnect(env, ret)
register struct wnn_env *env;
int ret;
{
    if (wnn_errorno == WNN_JSERVER_DEAD) {
	js_close(env);
    }
}

int
yes_or_no(string)
char *string;
{
    unsigned int c1;
    init_yes_or_no(string,YesNoMessage);
    for(;;) {
	c1 = keyin() ;
	if (xjutil->sel_ret == 1) {
	    end_yes_or_no();
	    xjutil->sel_ret = -1;
	    return(1);
	} else if (xjutil->sel_ret == 0) {
	    end_yes_or_no();
	    xjutil->sel_ret = -1;
	    return(0);
	} else {
	    ring_bell();
	    xjutil->sel_ret = -1;
	}
    }
}

static int
yes_or_no_or_newline(string)
char *string;
{
    return(yes_or_no(string));
}

void
paramchg()
{
    int c,k;
    int newval;
    char *ch;
    char st[80];
    char message1[80];
    char message2[80];
    w_char kana_buf[1024];

    static char *ccc[] = {"NULL" , "NULL", "NULL" , "NULL" , "NULL" , "NULL" , "NULL", "NULL", "NULL", "NULL",
"NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"};
    struct wnn_param para;
    int *paraary;
    char *cccmsg[(sizeof(ccc) / sizeof(char *))];
    char buf[1024];

    if (get_env() == -1) return;

    paraary = (int *)&para;
    if(js_param_get(cur_env, &para) == -1) {
	if_dead_disconnect(cur_env, -1);
	errorkeyin();
	return;
    }
    for(k = 0,ch = buf ; k < (sizeof(ccc) / sizeof(char *)) ; k++){
	cccmsg[k] = ch;
	sprintf(ch,"%s[%d]",
		msg_get(cd, 10 + k, default_message[10 + k], xjutil->lang),
		paraary[k]);
 	ch += strlen(ch) + 1;
    }
    c = xw_select_one_element(cccmsg,(sizeof(ccc) / sizeof(char *)), 0,
			      msg_get(cd, 9, default_message[9], xjutil->lang),
			      0, NULL, NULL, NULL, NULL);
    if (c == -1 || c == -3) {
        return;
    }
    if(c == 0){
	sprintf(message1 , "%s %d %s" ,
		msg_get(cd, 27, default_message[27], xjutil->lang),
		paraary[0], msg_get(cd, 28, default_message[28], xjutil->lang));
	sprintf(message2 , "%s",
		msg_get(cd, 29, default_message[29], xjutil->lang));
    }else{
	sprintf(message1 , "%s %s %s %d %s",
		msg_get(cd, 30, default_message[30], xjutil->lang),
		cccmsg[c] ,msg_get(cd, 31, default_message[31], xjutil->lang),
		paraary[c], msg_get(cd, 32, default_message[32], xjutil->lang));
	sprintf(message2 , "%s",
		msg_get(cd, 33, default_message[33], xjutil->lang));
    }
    init_input(message1, message2,
	       msg_get(cd, 8, default_message[8], xjutil->lang));
Retry:
    cur_text->max_pos = 127;
    clr_line_all();
    throw_col(0);
    kk_cursor_normal();
    kana_buf[0] = 0;
    if(kana_in(UNDER_LINE_MODE , kana_buf , 1024) == -1) {
		end_input();
		return;
    }
    if(wchartochar(kana_buf , st) || sscanf(st , "%d" , &newval) <= 0 ){
	print_msg_getc("%s", msg_get(cd, 34, default_message[34], xjutil->lang),
		       NULL, NULL);
	goto Retry;
    } else if(c == 0 && newval <= 0){
	print_msg_getc("%s", msg_get(cd, 35, default_message[35], xjutil->lang),
		       NULL, NULL);
	goto Retry;
    }
    paraary[c] = newval;
    if(js_param_set(cur_env, &para) == -1) {
	if_dead_disconnect(cur_env, -1);
	errorkeyin();
    }
    end_input();
    return;
}

int
update_dic_list(buf)
struct wnn_buf *buf;
{
    if((dic_list_size = js_dic_list(cur_env, &dicrb)) < -1) {
	if_dead_disconnect(cur_env, -1);
	return(-1);
    }
    dicinfo = (WNN_DIC_INFO *)(dicrb.buf);
    return(0);
}

void
dic_nickname(dic_no, buf)
int dic_no;
char *buf;
{
    int j;

    if((j = find_dic_by_no(dic_no)) == -1)return;
    if(*dicinfo[j].comment) sStrcpy(buf, dicinfo[j].comment);
    else strcpy(buf, dicinfo[j].fname);
}

int
find_dic_by_no(dic_no)
int dic_no;
{
    int j;

    for(j = 0 ; j < dic_list_size; j++){
	if(dicinfo[j].dic_no == dic_no)
	    return(j);
    }
    return(-1);
}

static void
fill(c,x)
char *c;
int x;
{
  for(;*c;c++,x--);
  for(;x;x--,c++){
    *c = ' ';
  }
}

static int
make_info_out(buf,size,infobuf,cnt)
char buf[];
WNN_DIC_INFO infobuf[];
int cnt;
int size;
{
  int max1 = 0;
  int max0 = 0;
  int i,k,j;
  
  for(k = 0 ; k < cnt ; k++){
    for(j = 0 ; j < size ; j++){
      buf[k * size + j] = 0;
    }
  }

  for(i = 0 ; i < 13;i++){
    if(i == 12){
      sprintf(buf + max0, "%3d/%-3d", 0, cnt - 1);
    } else {
      sprintf(buf + max0,"%s",
	      msg_get(cd, 54 + i, default_message[54 + i], xjutil->lang));
    }
    max1 = MAX_VAL(max1, strlen(buf));
    if(max1 >= size)return(-1);
    for(k = 1 ; k < cnt;k++){
      switch(i){
      case 0:
	sprintf(buf + size * k + max0,"%3d ",infobuf[k - 1].dic_no);
	break;
      case 1:
	{
	char *tmp;
#ifdef	CONVERT_by_STROKE
	if((0xff00&infobuf[k - 1].type) == (BWNN_REV_DICT & 0xff00))
	    tmp = bwnn_dic_types[0xff&infobuf[k - 1].type];
	else
#endif	/* CONVERT_by_STROKE */
#ifdef	CONVERT_with_SiSheng
	if((0xff00&infobuf[k - 1].type) == (CWNN_REV_DICT & 0xff00))
	    tmp = cwnn_dic_types[0xff&infobuf[k - 1].type];
	else
#endif	/* CONVERT_with_SiSheng */
	    tmp = wnn_dic_types[infobuf[k - 1].type];
	sprintf(buf + size * k + max0,"%8s", tmp);
	}
	break;
      case 2:
	sStrcpy(buf + size * k + max0,infobuf[k - 1].comment);
	break;
      case 3:
	strcpy(buf + size * k + max0,infobuf[k - 1].fname);
	break;
      case 4:
	strcpy(buf + size * k + max0,infobuf[k - 1].hfname);
	break;
      case 5:
	sprintf(buf + size * k + max0,"%6d",infobuf[k - 1].gosuu);
	break;
      case 6:
	sprintf(buf + size * k + max0,"%4d",infobuf[k - 1].nice);
	break;
      case 7:
	strcpy(buf + size * k + max0,(!infobuf[k - 1].rw)? "Yes":"No");
	break;
      case 8:
	strcpy(buf + size * k + max0,(!infobuf[k - 1].hindo_rw)? "Yes":"No");
	break;
      case 9:
	strcpy(buf + size * k + max0,
	       ((infobuf[k - 1].enablef)?
		msg_get(cd, 43, default_message[43], xjutil->lang):
		msg_get(cd, 44, default_message[44], xjutil->lang)));
	break;
      case 10:
	strcpy(buf + size * k + max0,infobuf[k - 1].passwd);
	break;
      case 11:
	strcpy(buf + size * k + max0,infobuf[k - 1].hpasswd);
	break;
      case 12:
	sprintf(buf + size * k + max0,"%3d/%-3d",k,cnt - 1);
	break;
      }
      max1 = MAX_VAL(max1, strlen(buf + size * k));
      if(max1 >= size)return(-1);
    }
    max0 = max1 + 1;
    for(k = 0 ; k < cnt;k++){
	fill(buf + size * k,max0);
    }
  }
  return(1);
}

void
dicinfoout()
{
    int k;
    int cnt;
    char buf[4096];
    char *info[JISHOKOSUU];
    char *comment;
    int size;
    char *table_t[3];

    if (get_env() == -1) return;

    if(update_dic_list() == -1){
	errorkeyin();
	return;
    }
    cnt = dic_list_size;
    cnt += 1; /* for the comment line */
    size = 4096 / cnt;
    if(make_info_out(buf, size, dicinfo, cnt) == -1){
	print_msg_getc("%s", msg_get(cd, 40, default_message[40], xjutil->lang),
		       NULL, NULL);
    }
    comment = buf;
    for(k = 1 ; k < cnt ; k++){
	info[k-1] = buf + k * size;
    }
    table_t[0] = msg_get(cd, 37, default_message[37], xjutil->lang);
    table_t[1] = msg_get(cd, 38, default_message[38], xjutil->lang);
    table_t[2] = msg_get(cd, 39, default_message[39], xjutil->lang);
    xw_select_one_element(info, cnt - 1, 0,
			  msg_get(cd, 36, default_message[36], xjutil->lang),
			  3, table_t, jutil_table[1], main_table[9], comment);
    return;
}

void
select_one_dict9()
{
    int l;
    char *c;
    char buf[4096];
    char *buf_ptr[JISHOKOSUU];
    char baka[1024];

    if (get_env() == -1) return;

    if(update_dic_list() == -1){
	errorkeyin();
	return;
    }
    if(dic_list_size == 0){
	print_msg_getc("%s", msg_get(cd, 42, default_message[42], xjutil->lang),
		       NULL, NULL);
	return;
    }

    for(c = buf,l = 0 ; l < dic_list_size ; l++){
	buf_ptr[l] = c;
	dic_nickname(dicinfo[l].dic_no, baka);
	sprintf(c,"%s(%s) %s %d/%d",
		baka, dicinfo[l].hfname,
		(dicinfo[l].enablef? 
		 msg_get(cd, 43, default_message[43], xjutil->lang):
		 msg_get(cd, 44, default_message[44], xjutil->lang)),
		 (l + 1), dic_list_size);
	c += strlen(c) + 1;
    }
    xw_select_one_element(buf_ptr, dic_list_size, 0,
			  msg_get(cd, 41, default_message[41], xjutil->lang),
			  0, NULL, NULL, NULL, NULL);
    return;
}

void
fuzoku_set()
{
    int ret;
    char *message1 = msg_get(cd, 45, default_message[45], xjutil->lang);
    char *message2 = msg_get(cd, 46, default_message[46], xjutil->lang);
    char fname[128];
    w_char kana_buf[1024];
    register int fid, orgfid;

    if (get_env() == -1) return;

    if (init_input(message1, message2,
		   msg_get(cd, 8, default_message[8], xjutil->lang)) == -1) {
	    ring_bell();
	    return;
    }
    cur_text->max_pos = 127;
    clr_line_all();
    throw_col(0);
    kk_cursor_normal();
    Strcpy(kana_buf , fuzokugopath);
    JWMflushw_buf(kana_buf, Strlen(kana_buf));
    if((ret = kana_in(UNDER_LINE_MODE , kana_buf , 1024)) == -1) {
	end_input();
	return;
    }
    wchartochar(kana_buf , fname);
    if(strlen(fname) == 0) {
	end_input();
	return;
    }
    Strcpy(fuzokugopath , kana_buf);
    orgfid = js_fuzokugo_get(cur_env);
    if(fname[0] == C_LOCAL){
	fid = js_file_send(cur_env, fname + 1);
    } else {
	fid = js_file_read(cur_env, fname);
    }
    if (fid == -1) {
	if_dead_disconnect(cur_env, -1);
	errorkeyin();
    }else if ((ret = js_fuzokugo_set(cur_env,fid)) < 0) {
	if_dead_disconnect(cur_env, -1);
	errorkeyin();
    } else if(fid != orgfid && orgfid != -1){
	if_dead_disconnect(cur_env, -1);
	errorkeyin();
    }
    end_input();
    return;
}

static int
call_error_handler(error_handler, c)
int (*error_handler)();
char *c;
{
    register int x;
    x = (*error_handler)(c);
    return(x);
}

static void
message_out(message_handler, format, s1, s2, s3)
int (*message_handler)();
char *format;
char *s1, *s2, *s3;
{
    if(message_handler){
	(*message_handler)(format, s1, s2, s3);
    }
}

static int
get_pwd(pwd_dic, pwd)
register char *pwd_dic, *pwd;
{
    FILE *fp;
	    
    if(pwd_dic && *pwd_dic){
	if((fp = fopen(pwd_dic, "r")) == NULL){
	    wnn_errorno = WNN_CANT_OPEN_PASSWD_FILE;
	    return(-1);
	}
	fgets(pwd, WNN_PASSWD_LEN, fp);
	fclose(fp);
    }else {
	pwd[0] = 0;
    }
    return(0);
}

static int
file_exist(env, n)
struct wnn_env *env;
char *n;
{
    extern int access();

    if(n[0] == C_LOCAL){
 	wnn_errorno = 0;
 	return(access(n + 1, 4));
     }else{
	 return(js_access(env,n,4));
    }
}    

static int
make_dir1(env, dirname, error_handler, message_handler)
register struct wnn_env *env;
register char *dirname;
int  (*error_handler)(), (*message_handler)();
{
    char gomi[128];
    extern int mkdir(), chmod(), chown();

    if(dirname[0] == C_LOCAL){
	if(*(dirname + 1) == 0) return(0);
	if(access(dirname + 1 , 0) == 0){ /* check for existence */
	    return(0); /* dir already exists */ 
	}
    }else{
	if(*dirname == 0) return(0);
	if(js_access(env, dirname , 0) == 0){ /* check for existence */
	    return(0); /* dir already exists */ 
	}
    }
    if((int)error_handler != -1){
	sprintf(gomi, "%s \"%s\" %s%s",
		msg_get(cd, 110, default_message[110], xjutil->lang),
		dirname,
		msg_get(cd, 103, default_message[103], xjutil->lang),
		msg_get(cd, 104, default_message[104], xjutil->lang));
	if(call_error_handler(error_handler,gomi) == 0){
	    wnn_errorno = WNN_MKDIR_FAIL;
	    return(-1);
	}
    }
    if(dirname[0] == C_LOCAL){  /* Create Directory */
#define	MODE (0000000 | 0000777)
#if defined(BSD42) || defined(SX8870)
	if(mkdir(dirname + 1 , MODE ) != 0 ){
	    wnn_errorno=WNN_MKDIR_FAIL;
	    return(-1);
	}
#endif
#if defined(SYSVR2) && !defined(SX8870)
	char buf[256];
	strcpy(buf , "/bin/mkdir ");
	strcat(buf ,dirname + 1 );
	if(system(buf) != 0){
	    /*
	    wnn_errorno=WNN_MKDIR_FAIL;
	    return(-1);
	    */
	}
#endif
	chmod(dirname + 1,MODE);
	chown(dirname + 1, getuid(), -1);
    }else{
	if(js_mkdir(env, dirname)){
	    if_dead_disconnect(env, -1);
	    return(-1);
	}
    }
    return(0);
}

static int
make_dir_rec1(env, path, error_handler, message_handler)
struct wnn_env *env;
register char *path;
int  (*error_handler)(), (*message_handler)();
{
    char gomi[128];
    register char *c;
    for(c = path;*c;c++){
	if(*c == '/'){
	    strncpy(gomi,path,c - path);
	    gomi[c - path] = 0;
	    if(make_dir1(env, gomi, error_handler, message_handler) == -1){
		return(-1);
	    }
	}
    }
    return(0);
}

static int
create_pwd_file(env, pwd_file, error_handler, message_handler)
register struct wnn_env *env;
char *pwd_file;
int  (*error_handler)(), (*message_handler)();
{
    FILE *fp;
    char *gomi[256];
#ifdef SYSVR2
    extern int srand();
    extern long rand();
#else
    extern int srandom();
    extern long random();
#endif
    extern long time();

    if(pwd_file == NULL || *pwd_file == 0) return(0);
    if(access(pwd_file, F_OK) != -1) return(0);
    sprintf(gomi, "%s \"%s\" %s%s",
	    msg_get(cd, 108, default_message[108], xjutil->lang),
	    pwd_file,
	    msg_get(cd, 103, default_message[103], xjutil->lang),
	    msg_get(cd, 104, default_message[104], xjutil->lang));
    if(call_error_handler(error_handler,gomi) == 0){
	wnn_errorno = WNN_NO_EXIST;
	return(-1);
    }
    if((fp = fopen(pwd_file, "w")) == NULL){
	wnn_errorno = WNN_CANT_OPEN_PASSWD_FILE;
	message_out(message_handler, wnn_perror_lang(xjutil->lang),
		    NULL, NULL, NULL);
	return(-1);
    }
#ifdef SYSVR2
    srand(time(0)+getuid());
    fprintf(fp,"%d\n",rand());
#else
    srandom(time(0)+getuid());
    fprintf(fp,"%d\n",random());
#endif
    fclose(fp);
#define	MODE_PWD (0000000 | 0000400)
    chmod(pwd_file,MODE_PWD);
    return(0);
}

#define JISHO 1
#define HINDO 2

static int
create_file(env,n, d, fid, pwd_dic, pwd_hindo, error_handler, message_handler)
register struct wnn_env *env;
char *n;
int d;
int fid;
char *pwd_dic, *pwd_hindo;
int  (*error_handler)(), (*message_handler)();
{
    char pwd[WNN_PASSWD_LEN], hpwd[WNN_PASSWD_LEN];
    int rev_dict_type;

    if(make_dir_rec1(env, n, error_handler, message_handler) == -1){
	wnn_errorno = WNN_MKDIR_FAIL;
	return(-1);
    }
    if(d == HINDO){
	if(create_pwd_file(env, pwd_hindo, error_handler, message_handler) == -1)return(-1);
	if(get_pwd(pwd_hindo, hpwd) == -1)return(-1);
	if(n[0] == C_LOCAL){
	    if(js_hindo_file_create_client(env, fid, n + 1, NULL, hpwd) == -1){
		message_out(message_handler, wnn_perror_lang(xjutil->lang),
			    NULL, NULL, NULL);
		if_dead_disconnect(env, -1);
	    }else{
		message_out(message_handler, "%s \"%s\" %s",
			   msg_get(cd, 105, default_message[105], xjutil->lang),
			   n,
			   msg_get(cd, 109, default_message[109], xjutil->lang));
		chown(n + 1, getuid(), -1); /* H.T. */
		return(0);
	    }
	}else{
	    if(js_hindo_file_create(env, fid, n, NULL, hpwd) == -1){
		message_out(message_handler, wnn_perror_lang(xjutil->lang));
		if_dead_disconnect(env, -1);
		return(-1);
	    }else{
		message_out(message_handler, "%s \"%s\" %s",
			   msg_get(cd, 105, default_message[105], xjutil->lang),
			   n,
			   msg_get(cd, 109, default_message[109], xjutil->lang));
		return(0);
	    }
	}
    }else {
	if(create_pwd_file(env, pwd_hindo, error_handler, message_handler) == -1)return(-1);
	if(get_pwd(pwd_hindo, hpwd) == -1)return(-1);
	if(create_pwd_file(env, pwd_dic, error_handler, message_handler) == -1)return(-1);
	if(get_pwd(pwd_dic, pwd) == -1)return(-1);

#ifdef	CONVERT_with_SiSheng
#ifdef	LANG_CN
	if(strncmp(js_get_lang(env), LANG_CN, 5) == 0)
	    rev_dict_type = CWNN_REV_DICT;
	else
#endif	/* LANG_CN */
#ifdef	LANG_TW
	if(strncmp(js_get_lang(env), LANG_TW, 5) == 0)
	    rev_dict_type = CWNN_REV_DICT;
	else
#endif	/* LANG_TW */
#endif	/* CONVERT_with_SiSheng */
	    rev_dict_type = WNN_REV_DICT;

	if(n[0] == C_LOCAL){
	    if(js_dic_file_create_client(env, n + 1, rev_dict_type,
						NULL, pwd, hpwd) == -1){
		message_out(message_handler, wnn_perror_lang(xjutil->lang),
			    NULL, NULL, NULL);
		if_dead_disconnect(env, -1);
		return(-1);
	    }else{
		message_out(message_handler, "%s \"%s\" %s",
			  msg_get(cd, 102, default_message[102], xjutil->lang),
			  n,
			  msg_get(cd, 109, default_message[109], xjutil->lang));
		chown(n + 1, getuid(), -1);
		return(0);
	    }
	}else{
	    if(js_dic_file_create(env, n, rev_dict_type, NULL, pwd, hpwd)== -1){
		message_out(message_handler, wnn_perror_lang(xjutil->lang),
			    NULL, NULL, NULL);
		if_dead_disconnect(env, -1);
		return(-1);
	    }else{
		message_out(message_handler, "%s \"%s\" %s",
			  msg_get(cd, 102, default_message[102], xjutil->lang),
			  n,
			  msg_get(cd, 109, default_message[109], xjutil->lang));
		return(0);
	    }
	}
    }
    return(-1);
}

static int
file_read(env, fname)
struct wnn_env *env;
char *fname;
{
    register int fid;
    if(fname[0] == C_LOCAL){
	fid = js_file_send(env, fname + 1);
    }else{
	fid = js_file_read(env, fname);
    }
    return(fid);
}

static int
file_remove(server, fname, pwd)
register WNN_JSERVER_ID *server;
char *fname;
char *pwd;
{
    if(fname[0] == C_LOCAL){
	return(js_file_remove_client(server, fname + 1, pwd));
    }else{
	return(js_file_remove(server, fname, pwd));
    }
}

static int
file_discard(env, fid)
register struct wnn_env *env;
register int fid;
{
    return(js_file_discard(env, fid));
}

static int
dic_add_e(env,dic_name,hindo_name,rev, prio,rw, hrw, pwd_dic, pwd_hindo, error_handler, message_handler)
register struct wnn_env *env;
char *dic_name;
char *hindo_name;
int prio;
int rw, hrw, rev;
char *pwd_dic, *pwd_hindo;
int  (*error_handler)(), (*message_handler)();
{
    char tmp[256];
    char pwd[WNN_PASSWD_LEN], hpwd[WNN_PASSWD_LEN];
    int fid, hfid = -1;
    register int ret;


    if(file_exist(env, dic_name) == -1) {
	if (wnn_errorno == WNN_JSERVER_DEAD) {
	    js_close(env->js_id);return(-1);
	}
	if((int)error_handler == 0 || 
	   (rw == WNN_DIC_RDONLY)){
	    message_out(message_handler, "%s \"%s\" %s",
		    msg_get(cd, 102, default_message[102], xjutil->lang),
		    dic_name,
		    msg_get(cd, 103, default_message[103], xjutil->lang));
	    wnn_errorno = WNN_NO_EXIST;
	    return(-1);
	}
	sprintf(tmp, "%s \"%s\" %s%s",
		msg_get(cd, 102, default_message[102], xjutil->lang),
		dic_name,
		msg_get(cd, 103, default_message[103], xjutil->lang),
		msg_get(cd, 104, default_message[104], xjutil->lang));
	if((int)error_handler == -1 || 
	   call_error_handler(error_handler, tmp)){
	    if(create_file(env, dic_name,JISHO, -1, /* -1 is dummy */
			   pwd_dic, 
			   (hindo_name && *hindo_name)? "": pwd_hindo,
			   error_handler, message_handler) == -1) {
		return(-1);
	    }
	}else{
	    wnn_errorno = WNN_NO_EXIST;
	    return(-1);
	}
    }
    if((fid = file_read(env, dic_name)) == -1) {
	    if_dead_disconnect(env, -1);
	    return(-1);
    }
    if(hindo_name && *hindo_name){
	if(file_exist(env, hindo_name) == -1) {
	    if(wnn_errorno == WNN_JSERVER_DEAD)	{
		js_close(env->js_id);;return(-1);
	    }
	    if((int)error_handler == 0 || (hrw == WNN_DIC_RDONLY)){
		message_out(message_handler, "%s \"%s\" %s",
			msg_get(cd, 105, default_message[105], xjutil->lang),
			hindo_name,
			msg_get(cd, 103, default_message[103], xjutil->lang));
		wnn_errorno = WNN_NO_EXIST;
		return(-1);
	    }
	    sprintf(tmp, "%s \"%s\" %s%s",
		    msg_get(cd, 105, default_message[105], xjutil->lang),
		    hindo_name,
		    msg_get(cd, 103, default_message[103], xjutil->lang),
		    msg_get(cd, 104, default_message[104], xjutil->lang));
	    if((int)error_handler == -1 || 
	       call_error_handler(error_handler, tmp)){
		if(create_file(env, hindo_name, HINDO, fid, 
			       "", pwd_hindo, error_handler,
			       message_handler) == -1) return(-1);
	    }else{
		wnn_errorno = WNN_NO_EXIST;
		return(-1);
	    }
	}
	if((hfid = file_read(env, hindo_name)) == -1){
	    if_dead_disconnect(env, -1);
	    return(-1);
	}
    }
    if(get_pwd(pwd_dic, pwd) == -1)return(-1);
    if(get_pwd(pwd_hindo, hpwd) == -1)return(-1);
    if((ret = js_dic_add(env, fid, hfid, rev, prio, rw, hrw, pwd, hpwd)) < 0){
	if(wnn_errorno == WNN_JSERVER_DEAD){
	    js_close(env->js_id);
	    return(-1);
	}else if(wnn_errorno == WNN_HINDO_NO_MATCH){
	    if((int)error_handler == 0){
		return(-1);
	    }
	    sprintf(tmp, "%s \"%s\" %s",
		    msg_get(cd, 106, default_message[106], xjutil->lang),
		    hindo_name,
		    msg_get(cd, 107, default_message[107], xjutil->lang));
	    if(!((int)error_handler == -1 ||
		 call_error_handler(error_handler, tmp))){
		return(-1);
	    }
	    if(file_discard(env, hfid) == -1) {
		if_dead_disconnect(env, -1);
		return(-1);
	    }
	    if(file_remove(env->js_id, hindo_name, hpwd) == -1) {
		if_dead_disconnect(env, -1);
		return(-1);
	    }
	    if(create_file(env,hindo_name, HINDO, fid, NULL, pwd_hindo, -1, message_handler) == -1) return(-1);
	    if((hfid = file_read(env, hindo_name)) == -1) {
		if_dead_disconnect(env, -1);
		return(-1);
	    }
	    if((ret = js_dic_add(env, fid, hfid, rev, prio, rw, hrw, pwd, hpwd))< 0) {
		if_dead_disconnect(env, -1);
		return(-1);
	    }
	}
    }
    return(ret);
}

static void
getfname(file,path)
char *path , *file;
{
    char *c;

    c = path + strlen(path);
    for(;*c != '/' && c >= path ; c--);
    strcpy(file , c + 1);
}

void
jishoadd()
{
    int ret;
    char *message1 = msg_get(cd, 47, default_message[47], xjutil->lang);
    char *message2 = msg_get(cd, 48, default_message[48], xjutil->lang);
    char *message = msg_get(cd, 49, default_message[49], xjutil->lang);
    char *hmessage = msg_get(cd, 50, default_message[50], xjutil->lang);
    char *priomessage = msg_get(cd, 51, default_message[51], xjutil->lang);
    char fname[128];
    char tmp[128];
    char hfname[128];
    int prio;
    int rdonly;
    int hrdonly;
    int id, l;
    w_char kana_buf[1024];

    if (get_env() == -1) return;

    if (init_input(message1, message, message2) == -1) {
	    ring_bell();
	    return;
    }
    JWMline_clear(0);
    throw_col(0);
    kk_cursor_normal();
    Strcpy(kana_buf , jishopath);
    JWMflushw_buf(kana_buf, Strlen(kana_buf));
    if((ret = kana_in(UNDER_LINE_MODE , kana_buf , 1024)) == -1) {
	    end_input();
	    return;
    }
    end_input();
    wchartochar(kana_buf , fname);
    if(strlen(fname) == 0) {
	return;
    }
    Strcpy(jishopath , kana_buf);
    Strcpy(kana_buf,hindopath);
    getfname(hfname,fname);
    if(strcmp(hfname + strlen(hfname) - 4, ".usr") == 0 ||
       strcmp(hfname + strlen(hfname) - 4, ".sys") == 0 ||
       strcmp(hfname + strlen(hfname) - 4, ".dic") == 0 ||
       strcmp(hfname + strlen(hfname) - 4, ".rev") == 0) {
	hfname[strlen(hfname) - 4] = 0;
	strcat(hfname , ".h");
    } else if(strcmp(hfname + strlen(hfname) - 5, ".usrR") == 0 ||
       strcmp(hfname + strlen(hfname) - 5, ".sysR") == 0) {
	hfname[strlen(hfname) - 5] = 0;
	strcat(hfname , ".hR");
    } else {
	strcat(hfname , ".h");
    }
    Sstrcpy(kana_buf + Strlen(kana_buf), hfname);
    if (init_input(message1, hmessage, message2) == -1) {
	ring_bell();
	return;
    }
    clr_line_all();
    throw_col(0);
    kk_cursor_normal();
    JWMflushw_buf(kana_buf, Strlen(kana_buf));
    if((ret = kana_in(UNDER_LINE_MODE , kana_buf , 1024)) == -1) {
	end_input();
	return;
    }
    end_input();
    wchartochar(kana_buf , hfname);
    sprintf(tmp,"%d",JISHO_PRIO_DEFAULT);
    Sstrcpy(kana_buf, tmp);
    if (init_input(message1, priomessage, message2) == -1) {
	ring_bell();
	return;
    }
    clr_line_all();
    throw_col(0);
    kk_cursor_normal();
    JWMflushw_buf(kana_buf, Strlen(kana_buf));
    if((ret = kana_in(UNDER_LINE_MODE , kana_buf , 1024)) == -1) {
	end_input();
	return;
    }
    end_input();
    wchartochar(kana_buf , tmp);
    prio = atoi(tmp);
    if (yes_or_no_or_newline(msg_get(cd, 52, default_message[52], xjutil->lang))
	== 1){
	rdonly = 0;
    }else{
	rdonly = 1;
    }
    if (yes_or_no_or_newline(msg_get(cd, 53, default_message[53], xjutil->lang))
	== 1){
	hrdonly = 0;
    }else{
	hrdonly = 1;
    }

    if (((id = dic_add_e(cur_env,fname,hfname,0,prio,rdonly, hrdonly, NULL, NULL,
			 yes_or_no_or_newline, print_msg_getc)) == -1) &&
	(wnn_errorno != -1)){
	errorkeyin();  
    }
    update_dic_list();
    for(l = 0 ; l < dic_list_size ; l++){
	 if(dicinfo[l].dic_no == id){
	     break;
	}
    }
    if(l != dic_list_size && cur_reverse_env->env &&
       (dicinfo[l].type == WNN_REV_DICT
#ifdef	CONVERT_with_SiSheng
       || dicinfo[l].type == CWNN_REV_DICT
#endif	/* CONVERT_with_SiSheng */
       )){
	if ((dic_add_e(cur_reverse_env->env, fname,hfname, 1,prio,rdonly,
		       hrdonly, NULL, NULL,
		       yes_or_no_or_newline, print_msg_getc) == -1) &&
	    (wnn_errorno != -1)) {
	    errorkeyin();
	}
    }
    /*
    clr_line_all();
    */
    end_yes_or_no();
}

int
dic_delete_e(env, dic_no)
register struct wnn_env *env;
register int dic_no;
{
    WNN_DIC_INFO dic;


    if(js_dic_info(env,dic_no,&dic) < 0){
	if_dead_disconnect(env, -1);
	return(-1);
    }
    if(js_dic_delete(env, dic_no) < 0) {
	if_dead_disconnect(env, -1);
	return(-1);
    }
    /*	dic Body	*/
    if (file_discard(env,dic.body) < 0) {
	if_dead_disconnect(env, -1);
	return(-1);
    }
    /*	dic hindo	*/
    if(dic.hindo != -1){
	if (file_discard(env,dic.hindo) < 0){
	    if_dead_disconnect(env, -1);
	    return(-1);
	}
    }
    return(0);
}



