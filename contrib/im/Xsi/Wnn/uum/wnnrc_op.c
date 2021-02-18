/*
 * $Id: wnnrc_op.c,v 1.2 1991/09/16 21:34:07 ohm Exp $
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
/* uumrc operations */

#include "stdio.h"
#include <ctype.h>
#include <sys/errno.h>
#include <pwd.h>
#include "jllib.h"
#include "commonhd.h"
#include "config.h"
#include "buffer.h"
#include "rk_spclval.h"
#include "sdefine.h"
#include "sheader.h"

#ifdef BSD42
#include <strings.h>
#else
#include <string.h>
#endif

extern char     *getenv();
extern FILE	*fopen();
extern int conv_keyin();
extern int keyin0();


void
getfname(file,path)
char *path , *file;
{
    char *c;

    c = path + strlen(path);
    for(;*c != '/' && c >= path ; c--);
    strcpy(file , c + 1);
}



 /**	s1とs2がマッチしたら非0を返す。s1にはワイルドカード '*' を
	含んでよい（s2にはだめ）。*/
int
strmatch(s1, s2)
char	*s1, *s2;
{
	while(1){
		if(*s1 == '\0') return(*s2 == '\0');
		if(*s1 != '*'){
			if(*s1 != *s2) return(0);
			s1++; s2++;
			continue;
		}

		s1++;
		do{
			if(strmatch(s1, s2)) return(1);
		} while(*s2++ != '\0');
		return(0);
	}
}

 /**	KEYBOARDという環境変数が設定されていればそれが、又、いなければTERMが
	返る */
char	*get_kbd_env()
{
	char	*term;

	return(NULL != (term = getenv(WNN_KEYBOARD_ENV)) ? term :
							   getenv("TERM"));
}	

 /**	KEYBOARDという環境変数が設定されていればそれが、又、いなければTERMが、
	引き数とマッチするかどうかの検査（マッチしたら非0が返る）。convert_key
	の初期設定に使う。*/
int
lookWnnterm(s)
char	*s;
{
	char	*term;

	return(NULL == (term = get_kbd_env()) ? 0 : strmatch(s, term));
}

int
expand_expr(s)
 /**	~user、@HOME、@LIBDIRの展開(但し、文字列の先頭のみ)。できない時は-1が
	返り、その場合sの中身は着々とそのまんま。sの長さ＜256と仮定してる。*/
 /**	@USR (env名、logname), @LANG の展開 */
char	*s;
{
	char	tmp[EXPAND_PATH_LENGTH];
	register char	*p, *s1;
	int	noerr, expandsuc;
	struct	passwd	*u;
	extern struct passwd *getpwnam();

    if(*s != '~' && *s != '@') {
	strcpy(tmp, s);
	*s = '\0';
	noerr = 1;
    }else{
	if((int)strlen(s) >= EXPAND_PATH_LENGTH) return(-1);

	s1 = s;
#ifdef BSD42	
	if(NULL != (p = index(++s1, '/'))){
#else
	if(NULL != (p = strchr(++s1, '/'))){
#endif
		strcpy(tmp, p);
		*p = '\0';
	} else *tmp = '\0';
 /* ここまでは準備。s…先頭、s1…２文字目、p…最初の'/'のあったところ
    （ここで一旦切る）、tmp…それ以後のコピー。*/

	if(*s == '~'){
		if(*s1){
			noerr = expandsuc = 
			(NULL != (u = getpwnam(s1)) &&
			 (int)(strlen(p = u -> pw_dir) + strlen(tmp)) <
			 EXPAND_PATH_LENGTH );

		} else {
			noerr = expandsuc =
			(NULL != (p = getenv("HOME")) &&
			 (int)(strlen(p) + strlen(tmp)) < EXPAND_PATH_LENGTH);
		}

	} else { /* then, *s must be '@' */
		if(!strcmp(s1, "HOME")){
			noerr = expandsuc = 
			(NULL != (p = getenv("HOME")) &&
			 (int)(strlen(p) + strlen(tmp)) < EXPAND_PATH_LENGTH);
		} else if(!strcmp(s1, "LIBDIR")){
			noerr = expandsuc = 
			((int)(strlen(p= LIBDIR)+ strlen(tmp)) < EXPAND_PATH_LENGTH);
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
	for(; *p; p++){
	    if (!strncmp(p, "@USR", len)) {
		if ((int)(strlen(username) + strlen(p) + strlen(s) - len)
		     < EXPAND_PATH_LENGTH) {
		    strcat(s, username);
		    p += len - 1;
		} else {
		    return (-1);
		}
	    }else if (!strncmp(p, "@LANG", len1)) {
		if ((int)(strlen(lang_dir) + strlen(p) + strlen(s) - len1)
		     < EXPAND_PATH_LENGTH) {
		    strcat(s, lang_dir);
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



/** uumrc を見てのパラメータの設定 */
int
uumrc_set_entry(data,dn )
char *data;
int *dn;
{
    int num,d1;
    char code[256];
    char s[7][EXPAND_PATH_LENGTH];
    char tmp[1024];
    FILE *fp;
    char  *s0or1;

    num = sscanf(data,"%s %s %s %s %s %s %s %s",code,
    		 s[0],s[1],s[2],s[3],s[4],s[5],s[6]) ;
    if (num <= 0) { return(-1); }

    if(code[0] == ';'){return(0);
    }else
    if (strcmp(code, "include") == 0){
	if(expand_expr(s[0]) != 0){
	    fprintf(stderr , "Can't expand %s.\r\n", s[0]);
	}
	if((fp = fopen(s[0], "r")) != NULL){
	    while(fgets(tmp,1024,fp  ) != NULL){
		uumrc_set_entry(tmp,dn);
	    }   
	    fclose(fp);
	}else{
	    fprintf(stderr , "Can't open %s.\r\n", s[0]);
	}
    }else if(strcmp(code , "setuumkey") == 0){
	/* setuumkey --- this must be a file.*/
	if(!uumkey_defined_by_option && (num > 1)) {
	    if(expand_expr(s[0]) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s[0]);
	    }
	    strncpy(uumkey_name_in_uumrc, s[0], PATHNAMELEN);
	}
    }else if(strcmp(code , "setconvenv") == 0){
	if (num > 1) {
	    get_new_env(0);
	    /* setconvenv*/
	    if(num > 2) {
		if(!(strcmp(s[num - 1], "sticky"))) {
		    normal_sticky = 1;
		    num--;
		}
	    }
	    if (num == 2) s0or1 = s[0];
	    else {
		s0or1 = s[1];
		if (def_servername && *def_servername) {
		    servername = (char *)malloc(strlen(def_servername));
		    strcpy(servername, def_servername);
		} else {
		    servername = (char *)malloc(strlen(s[0]));
		    strcpy(servername, s[0]);
		}
	    }
	    if(expand_expr(s0or1) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s0or1);
	    }
	    envrcname= (char *)malloc(strlen(s0or1));
	    strcpy(envrcname, s0or1);
	}
    } else if(strcmp(code , "setkankanaenv") == 0){
	if (num > 1) {
	    get_new_env(1);
	    /* setkankanaenv */
	    if(num > 2) {
		if(!(strcmp(s[num - 1], "sticky"))){
		    reverse_sticky = 1;
		    num--;
		}
	    }
	    if (num == 2) s0or1 = s[0];
	    else {
		s0or1 = s[1];
		if (def_reverse_servername && *def_reverse_servername) {
		    reverse_servername =
		      (char *)malloc(strlen(def_reverse_servername));
		    strcpy(reverse_servername, def_reverse_servername);
		} else {
		    reverse_servername = (char *)malloc(strlen(s[0]));
		    strcpy(reverse_servername, s[0]);
		}
	    }
	    if(expand_expr(s0or1) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s0or1);
	    }
	    reverse_envrcname= (char *)malloc(strlen(s0or1));
	    strcpy(reverse_envrcname, s0or1);
	}
    } else if(strcmp(code , "setenv") == 0) {
            if(num > 2) {
                get_new_env(0);
		if(!(strcmp(s[num - 3], "sticky"))) {
		    normal_sticky = 1;
		    num--;
		}
                if (num == 3) s0or1 = s[1];
                else {
                    s0or1 = s[2];
		    if (def_servername && *def_servername) {
			servername = (char *)malloc(strlen(def_servername));
			strcpy(servername, def_servername);
		    } else {
			servername = (char *)malloc(strlen(s[1]));
			strcpy(servername, s[1]);
		    }
                }
                if(expand_expr(s0or1) != 0) {
		    fprintf(stderr , "Can't expand %s.\r\n", s0or1);
                }
		envrcname = (char *)malloc(strlen(s0or1));
		strcpy(envrcname, s0or1);
		strcpy(env_name_s, s[0]);
            }
        } else if(strcmp(code , "setenv_R") == 0) {
            if(num > 2) {
                get_new_env(1);
		if(!(strcmp(s[num - 1], "sticky"))) {
		    reverse_sticky = 1;
		    num--;
		}
                if (num == 3) s0or1 = s[1];
                else {
                    s0or1 = s[2];
		    if (def_reverse_servername && *def_reverse_servername) {
			reverse_servername =
			  (char *)malloc(strlen(def_reverse_servername));
			strcpy(reverse_servername, def_reverse_servername);
		    } else {
			reverse_servername = (char *)malloc(strlen(s[1]));
			strcpy(reverse_servername, s[1]);
		    }
                }
                if(expand_expr(s0or1) != 0) {
		    fprintf(stderr , "Can't expand %s.\r\n", s0or1);
                }
		reverse_envrcname = (char *)malloc(strlen(s0or1));
		strcpy(reverse_envrcname, s0or1);
		strcpy(reverse_env_name_s, s[0]);
            }
    } else if(strcmp(code , "setrkfile") == 0){
	/* setrkfile */
	if(!rkfile_defined_by_option && (num > 1)){
	    if(expand_expr(s[0]) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s[0]);
	    }
	    strncpy(rkfile_name_in_uumrc, s[0], PATHNAMELEN);
	}
    } else if(strcmp(code , "setconvkey") == 0) {
	/* setconvkey --- pair of TERM name and used table name */
	if(!convkey_defined_by_option){
	    if (num == 2) s0or1 = s[0];
	     else if ((num > 2) && lookWnnterm(s[0])) s0or1 = s[1];
	     else goto NOMATCH;

	    if(expand_expr(s0or1) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s0or1);
	    }
	    strncpy(convkey_name_in_uumrc, s0or1, PATHNAMELEN);
NOMATCH:    ;
	}
    }else if(strcmp(code , "flow_control_on") == 0){
	/* setflowctrl */
	if(!(defined_by_option & OPT_FLOW_CTRL)) {
	    flow_control = 1;
	}
    }else if(strcmp(code , "flow_control_off") == 0){
	/* setflowctrl */
	if(!(defined_by_option & OPT_FLOW_CTRL)) {
	    flow_control = 0;
	}
    }else if(strcmp(code , "waking_up_in_henkan_mode") == 0 ||
	     strcmp(code , "waking_up_in_convert_mode") == 0){
	/* */
	if(!(defined_by_option & OPT_WAKING_UP_MODE)) {
	    henkan_off_flag = 0;
	}
    }else if(strcmp(code , "waking_up_no_henkan_mode") == 0 ||
	     strcmp(code , "waking_up_no_convert_mode") == 0) {
	/* */
	if(!(defined_by_option & OPT_WAKING_UP_MODE)) {
	    henkan_off_flag = 1;
	}
	/* 暫定? */
    }else if(strcmp(code , "convkey_always_on") == 0){
	keyin_when_henkan_off = conv_keyin;
    }else if(strcmp(code , "convkey_not_always_on") == 0){
	keyin_when_henkan_off = keyin0;
	/* end of "暫定?" */
    }else if(strcmp(code,"simple_delete")==0){
    	excellent_delete = 0;
    }else if(strcmp(code,"excellent_delete")==0){
    	excellent_delete = 1;
    } else if(strcmp(code , "send_ascii_char") == 0){
	/* send ascii char or not */
	send_ascii_char = 1;
    }else if(strcmp(code , "not_send_ascii_char") == 0){
	send_ascii_char = 0;
    }else if (strcmp(code, "setmaxchg") == 0){
	/* setmaxchg ----- henkan kanou saidai mojisuu */
	if (change_ascii_to_int(s[0],&d1) != -1){
	    maxchg = (d1 <= 0) ? maxchg:d1;
	}
    } else if (strcmp(code, "setmaxbunsetsu") == 0){
      /* setmaxbunsetsu ---- kaiseki kanou saidai bunsetsu kosuu */
	if(num >= 2){
	    if (change_ascii_to_int(s[0],&d1) != -1){
		maxbunsetsu = (d1 <= 0) ? maxbunsetsu:d1;
	    }
	}
    } else if (strcmp(code, "setmaxichirankosu") == 0){
      /* setmaxichirankosuu --- jikouho ichiran hyouji kosuu */
	if(num >= 2){
	    if (change_ascii_to_int(s[0],&d1) != -1){
		max_ichiran_kosu = (d1 <= 0) ? max_ichiran_kosu:d1;
	    }
	}
    } else if (strcmp(code, "setmaxhistory") == 0){
      /* setmaxhistory --- set depth of history */
	if(num >= 2){
	    if (change_ascii_to_int(s[0],&d1) != -1){
		max_history = (d1 <= 0) ? max_history:d1+1;	
	    }
	}
    }else if(strcmp(code, "remove_cs") == 0){
      remove_cs_from_termcap = 1;
    }else if(strcmp(code, "not_remove_cs") == 0){
      remove_cs_from_termcap = 0;
    } else if (strcmp(code, "setjishopath") == 0 ||
	       strcmp(code , "setdicpath") == 0){
      /* setjishopath --- 辞書のあるディレクトリ */
	if(num >= 2){
	    if(expand_expr(s[0]) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s[0]);
	    }
	    chartowchar(s[0], jishopath);
	}
    } else if (strcmp(code, "sethindopath") == 0 ||
	       strcmp(code , "setfreqpath") == 0){
      /* sethindopath --- 頻度のあるディレクトリ */
	if(num >= 2){
	    if(expand_expr(s[0]) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s[0]);
	    }
	    chartowchar(s[0], hindopath);
	}
    } else if ((strcmp(code, "setfuzokugopath") == 0) ||
	       (strcmp(code, "setgrammarpath") == 0)){
	if(num >= 2){
	    if(expand_expr(s[0]) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s[0]);
	    }
	    chartowchar(s[0], fuzokugopath);
	}
/*
    } else if (strcmp(code, "dai_kugiri_str") == 0){
	if(num >= 2){
	    char tmp[MAXKUGIRI * 2];
	    read_kanji_str(tmp, s[0]);
	    chartowchar(tmp, kugiri_str);
	}else{
	    kugiri_str[0] = 0;
	}
*/
    }else if (strcmp(code, "touroku_comment") == 0){
	touroku_comment = 1;
    }else if (strcmp(code, "touroku_no_comment") == 0){
	touroku_comment = 0;
    }else{
/*      printf("Undefined entry \"%s\" in uumrc.\r\n",code); */
    }
    return(0);
}

int
change_ascii_to_int(st,dp)
char *st;
int *dp;
{
    register int total,flag;

    total = 0;
    flag = 0;
    while(*st != NULL){
	if (isdigit(*st)){
	    total = total * 10 + (*st - '0');
	} else if (*st == '+') {
	    if (flag != 0) { return(-1); }
	    flag = 1;
	} else if (*st == '-') {
	    if (flag != 0) { return(-1); }
	    flag = -1;
	} else { return(-1); }
	st++;
    }
    if (flag == 0){
	flag = 1;
    }
    *dp = total * flag;
    return(1);
}


/* uumrc ファイルからデータを読みその設定を行う。*/
int
uumrc_get_entries()
{
 extern FILE *open_uumrc();
 FILE *fp;
 int dn; char data[1024];

 /* default setting */

 maxchg = MAXCHG;
 maxbunsetsu = MAXBUNSETSU;
 max_ichiran_kosu = MAX_ICHIRAN_KOSU;

 max_history = MAX_HISTORY + 1;

 keyin_when_henkan_off = keyin0;

 if((fp= open_uumrc())==NULL){	return -1; }
 dn = 0;
 while(fgets(data,1024,fp  ) != NULL){
   uumrc_set_entry(data,&dn);
 }
 fclose(fp);
 return(0);
}


/** uumrc ファイルのオープン */
FILE *
open_uumrc()
{
    FILE *fp;
    char *n, buf[1024];

    if(n = getenv(WNN_UUM_ENV)){
	if(fp = fopen(n, "r")){
	    if(verbose_option) fprintf(stderr, "uumrc: using uumrc %s\r\n", n);
	    return fp;
	}else{
	    goto error;
	}
    }
    if(n = getenv("HOME")){
	strcat(strcpy(buf,n),USR_UUMRC);
	if(fp = fopen(buf, "r")){
	    if(verbose_option) fprintf(stderr, "uumrc: using uumrc %s\r\n", buf);
	    return fp;
	}
    }
    strcpy(buf, LIBDIR);
    strcat(buf, "/");
    strcat(buf, lang_dir);
    strcat(buf, RCFILE);
    if((fp = fopen(buf, "r")) != NULL){
	if(verbose_option) fprintf(stderr, "uumrc: using uumrc %s\r\n", buf);
	return fp;
    }
error:
    fprintf(stderr , MSG_GET(14));
/*
    fprintf(stderr , "uumrc ファイルがありません。");
*/
    fflush(stderr);
    return(0);
}


#ifdef	nodef
read_kanji_str(c , o)
register char *c , *o;
{
    for(;*o;c++){
	if(*o == '\\'){
	    if(*++o == '0'){
		o += 1;
		if(*o >= '0' && *o <= '7'){
		    *c = (*o++ - '0');
		}else continue;
		if(*o >= '0' && *o <= '7'){
		    *c *= 8;
		    *c |= (*o++ - '0');
		}else continue;
	    }else{
		*c = *o++;
	    }
	}else{
	    *c = *o++;
	}
    }
    *c = 0;
}
#endif
