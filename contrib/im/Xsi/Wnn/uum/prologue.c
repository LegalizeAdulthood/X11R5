/*
 * $Id: prologue.c,v 1.5 1991/09/19 10:06:31 proj Exp $
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
/* Standard Interface
 *    Initialize Routine
 */

#include <stdio.h>
#include <ctype.h>
#include <sys/errno.h>
#include <pwd.h>
#include "jllib.h"
#include "commonhd.h"
#include "config.h"
#include "buffer.h"
#include "rk_spclval.h"
#ifdef SYSVR2
#   include <fcntl.h>
#   include <string.h>
#endif
#include <sys/types.h>
#include <sys/file.h>
#ifdef BSD42
#   include <strings.h>
#endif
#ifdef TERMINFO
#   include <curses.h>
#   include <term.h>
#endif
#include "sdefine.h"
#include "sheader.h"
#ifdef UX386
#include <unistd.h>
#endif

#undef putchar
extern int putchar();
extern struct passwd *getpwnam();

extern char     *getenv();
extern FILE	*fopen();




/** romkan のイニシャライズ */
int
open_romkan()
{
    char name[PATHNAMELEN];
    int rk_option_flg;

    romkan_set_lang(lang_dir);
    if (*rkfile_name_in_uumrc != '\0') {
	strcpy(name, rkfile_name_in_uumrc);
    }else {
	strcpy(name , LIBDIR);
	strcat(name , RKFILE);
    }
    rk_option_flg = RK_REDRAW | RK_CHMOUT | (excellent_delete? 0 : RK_SIMPLD)
		    | (verbose_option? RK_VERBOS : 0);

    return(romkan_init5(name,rubout_code,NULL,NULL,rk_option_flg) == 0 ?
    	   0 : -1);
				/* romaji-kana henkan junbi */
}


/** 漢字かな変換用 envrc ファイルのオープン */
int
get_envrc_name_reverse(str)
char *str;
{
    if(cur_reverse_env == NULL || *reverse_envrcname == 0)
	return(0);
    strcpy(str, reverse_envrcname);
    if(access(str, R_OK) == -1){
	return(-1);
    }
    return(1);
}


/** envrc ファイルのオープン */
int
get_envrc_name(str)
char *str;
{
    if(cur_normal_env == NULL || envrcname[0] == '\0') {
	strcpy(str, LIBDIR);
	strcat(str, "/");
	strcat(str, lang_dir);
	strcat(str, ENVRCFILE);
    }else{
	strcpy(str, envrcname);
    }
    return(1);
}

/* uum で必要なエリアをアロケートする。*/
int
allocate_areas()
{
    char *malloc();
    char *area_start;
    char *area_pter;

    int k;
    int total_size;
    int len1 = maxchg * sizeof(w_char);
    int return_buf_len = len1 * 7;

    total_size = return_buf_len + len1 * 3 + maxbunsetsu * sizeof(int) * 3 +
		 maxbunsetsu * sizeof(struct wnn_env *);

    if ((area_start = malloc(total_size)) == NULL){
	/*
	fprintf(stderr,"初期化でmalloc不能。\n");
	*/
	fprintf(stderr, MSG_GET(2));
	return(-1);
    }

    area_pter = area_start;
    return_buf = (w_char *)area_pter;  /* mojiretsu -> return_buf */
    area_pter += return_buf_len;
    input_buffer = (w_char *)area_pter;
    area_pter += len1;
    kill_buffer = (w_char *)area_pter;
    area_pter += len1;
    remember_buf = (w_char *)area_pter;
    *remember_buf = 0;
    area_pter += len1;
    bunsetsu  = (int *)area_pter;
    area_pter += maxbunsetsu * sizeof(int);
    bunsetsuend  = (int *)area_pter;
    area_pter += maxbunsetsu * sizeof(int);
    bunsetsu_env  = (struct wnn_env **)area_pter;
    area_pter += maxbunsetsu * sizeof(int);
    touroku_bnst  = (int *)area_pter;
    area_pter += maxbunsetsu * sizeof(int);
    for (k = 0; k < maxbunsetsu; k++) {
	bunsetsu[k] = 0;
	bunsetsuend[k] = 0;
	touroku_bnst[k] = 0;
    }

    return(0);
}


/* henkan initialize */
/** uum のイニシャライズ */
int
init_uum()   
{
    char	*p;
    FuncDataBase *f;

    if(*lang_dir == '\0') {
	if ((p = getenv("LANG")) != NULL) {
	    strncpy(lang_dir, getenv("LANG"), 5);
	    lang_dir[5] = '\0';
	} else {
	    lang_dir[0] = '\0';
	}
    }
    if(*lang_dir == '\0') {
	strcpy(lang_dir, WNN_DEFAULT_LANG);
    }

    for (f = function_db; f && f->lang; f++) {
	if (!strcmp(f->lang, lang_dir)) {
	    f_table = &(f->f_table);
	    break;
	}
    }
    if (f_table == NULL) {
	f_table = &default_func_table;
    }

    uumrc_get_entries();

    if(*(p = convkey_name_in_uumrc) == '\0') p = NULL;
    if(convert_key_setup(p, (verbose_option != 0)) == -1) return(-1);
    
    if (init_key_table() == -1) {
	return(-1);
    }

    if(open_romkan() == -1){
	return(-1);
    }

    if (allocate_areas() == -1) {
	return(-1);
    }

    if (init_history() == -1) {
	return(-1);
    }

    connect_jserver(0);
    
    throw_cur_raw(0 ,crow + conv_lines);
    scroll_up();
    set_scroll_region(0 , crow - 1);
    throw_cur_raw(0 ,crow  - 1);
    flush();

    return(0);
}

static void
puts1(s)
char *s;
{
    printf("%s\n",s);
    flush();
}

static void
puts2(s)
char *s;
{
    throw_c(0);
    clr_line();
    printf("%s",s);
    flush();
}

static int
yes_or_no_init(s)
char *s;
{
    for(;;){char x[256];
	printf(s); flush(); if(gets(x) == NULL){return(0);}
	if(*x=='n' || *x=='N') return(0);
	if(*x == 'y' || *x == 'Y')return(1);
    }
}

/** Connecting to jserver */
int
connect_jserver(first)
int first;
{
    char uumrc_name[PATHNAMELEN];
    char environment[PATHNAMELEN];
    WnnEnv *p;
    extern int yes_or_no(), put2();
    extern char *malloc();

    if (first == 0) {
	bun_data_ = NULL;
    }
    get_envrc_name(uumrc_name);	/* use username for env-name */
	
    for (p = normal_env; p; p = p->next) {
	cur_normal_env = p;
	get_envrc_name(uumrc_name);
	strcpy(environment, username);
	strcat(environment, p->env_name_str);
	if (servername == NULL && def_servername && *def_servername) {
            servername = malloc(strlen(def_servername)+1);
	    strcpy(servername, def_servername);
        }
	if (bun_data_ == NULL) {
            bun_data_ = jl_open_lang(environment, servername, lang_dir,
				     uumrc_name, yes_or_no_init,
				     puts1, WNN_TIMEOUT);
            if (bun_data_ != NULL && bun_data_->env != NULL) {
                env_normal = bun_data_->env;
            }
	} else if (env_normal == 0 || jl_isconnect_e(env_normal) == 0) {
	    env_normal = jl_connect_lang(environment, servername,
					 lang_dir, uumrc_name,
					 yes_or_no, puts2, WNN_TIMEOUT);
	}
	if (normal_sticky && env_normal)
	  jl_env_sticky(bun_data_);
    }
    if (normal_env == NULL) {
        get_new_env(0);
        cur_normal_env = normal_env;
        uumrc_name[0] = '\0';
        bun_data_ = jl_open_lang(username, servername, lang_dir, uumrc_name,
                                 yes_or_no, puts2, WNN_TIMEOUT);
    }

    for (p = reverse_env; p; p = p->next) {
        cur_reverse_env = p;
        if (get_envrc_name_reverse(uumrc_name) > 0) {
            strcpy(environment, username);
            strcat(environment, p->env_name_str);
            strcat(environment, "R");
	    if (reverse_servername == NULL && def_reverse_servername
		&& *def_reverse_servername) {
		reverse_servername =
		  malloc(strlen(def_reverse_servername)+1);
		strcpy(reverse_servername, def_reverse_servername);
	    }
            env_reverse = jl_connect_lang(environment, reverse_servername,
                                          lang_dir, uumrc_name, yes_or_no,
                                          puts2, WNN_TIMEOUT);
            if(reverse_sticky && env_reverse)
              jl_env_sticky_e(env_reverse);
        }
    }
    return(0);
}


/** 立ち上がり時にメッセージファイルが存在すればそれを表示する。*/
int initial_message_out() 
{
    FILE *fp;
    char data[1024];
    char fname[PATHNAMELEN];

    strcpy(fname, LIBDIR);
    strcat(fname, MESSAGEFILE);

    if((fp = fopen(fname, "r")) != NULL) {
	while(fgets(data,1024,fp) != NULL){
	    fprintf(stderr,"\r%s",data);
	}
	fclose(fp);
	fprintf(stderr,"\r\n");
    }
    return(1);
}
