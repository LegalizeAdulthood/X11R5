/*
 * $Id: prologue.c,v 1.3 1991/09/16 21:37:13 ohm Exp $
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
/* Standard Interface
 *    Initialize Routine
 */

#include <stdio.h>
#include "commonhd.h"
#include "config.h"
#include "sdefine.h"
#include <X11/Xos.h>
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"

extern char     *getenv();
extern FILE	*fopen();

/** Initialize of romkan */
int
open_romkan(lang)
char *lang;
{
    int rk_option_flg;
    LangUumrcRec *lu;

    rk_option_flg = RK_REDRAW | RK_CHMOUT | (excellent_delete? 0 : RK_SIMPLD);

    for (lu = languumrc_list; lu != NULL; lu = lu->next) {
	if (!strcmp(lang, lu->lang_name)) {
	    cur_rk_table = lu->rk_table;
	    break;
	}
    }
    c_c->rk = (Romkan *)romkan_init5(rubout_code,NULL,NULL,rk_option_flg);
    if (c_c->rk == NULL)
	return(-1);
    else {
	c_c->rk->rk_table = cur_rk_table;
	return(0);
    }
}



/** Open the wnnenvrc_R file */
static int
get_envrc_name_reverse(str, lang)
char *str, *lang;
{
    extern int access();

    if(c_c->cur_reverse_env == NULL || reverse_envrcname == NULL
       || reverse_envrcname[0] == '\0')
        return(0);
    strcpy(str, reverse_envrcname);
    if(access(str, R_OK) == -1){
	    return(-1);
    }
    return(1);
}

/** Open the wnnenvrc file */
static int
get_envrc_name(str, lang)
char *str, *lang;
{
    if(c_c->cur_normal_env == NULL || envrcname == NULL
       || envrcname[0] == '\0') {
	strcpy(str, LIBDIR);
	strcat(str, "/");
	strcat(str, lang);
	strcat(str, ENVRCFILE);
    }else{
	strcpy(str, envrcname);
    }
    return(1);
}

/* Allocate of Wnn Area */
int
allocate_areas()
{
    char *area_start;
    char *area_pter;

    int k;
    int total_size;
    int len1 = maxchg * sizeof(w_char);
    int return_buf_len = len1 * 7;

    total_size = return_buf_len + len1 * 3 + maxbunsetsu * sizeof(int) * 3 +
		 maxbunsetsu * sizeof(struct wnn_env *);

    if ((area_start = Malloc(total_size)) == NULL) {
	malloc_error("allocation of Wnn's area");
	return(-1);
    }

    area_pter = area_start;
    return_buf = (w_char *)area_pter; 
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
    area_pter += maxbunsetsu * sizeof(struct wnn_env *);
    touroku_bnst  = (int *)area_pter;
    area_pter += maxbunsetsu * sizeof(int);
    for (k = 0; k < maxbunsetsu; k++) {
	bunsetsu[k] = 0;
	bunsetsuend[k] = 0;
	touroku_bnst[k] = 0;
    }
    return(0);
}

void
free_areas()
{
    Free(return_buf);
}

static struct wnn_buf *
local_jl_open_lang()
{
    struct wnn_buf *buf;

    if ((buf = (struct wnn_buf *)Malloc(sizeof(struct wnn_buf))) == NULL) {
	return(NULL);
    }
    buf->bun_suu = 0;
    buf->zenkouho_suu = 0;
    buf->zenkouho_daip = 0;
    buf->c_zenkouho = -1;
    buf->zenkouho_bun = -1;
    buf->zenkouho_end_bun = -1;
    buf->free_heap = NULL;
    buf->heap = (char *)Malloc(sizeof(char));
    *(buf->heap) = '\0';
    buf->zenkouho_dai_suu = 0;
    buf->msize_bun = 0;
    buf->msize_zenkouho = 0;
    buf->zenkouho_dai_suu = 0;
    buf->bun = NULL;
    buf->zenkouho_dai = NULL;
    buf->zenkouho = NULL;
    buf->down_bnst = NULL;
    buf->zenkouho_dai = NULL;
    buf->env = NULL;
    return(buf);
}

/** Connecting to jserver */
int
connect_server(lang)
char *lang;
{
    char environment[PATHNAMELEN];
    char uumrc_name[PATHNAMELEN];
    XIMLangDataBase *db;
    register WnnEnv *p;

    for (db = language_db; db->lang != NULL; db++) {
	if (!strcmp(lang, db->lang)) {
	    break;
	}
    }
    if ((db->lang == NULL) || (db->connect_serv == False)) {
	if (c_c->normal_env == NULL) {
	    get_new_env(c_c, 0);
	}
	c_c->use_server = 0;
	c_c->cur_normal_env = c_c->normal_env;
	uumrc_name[0] = '\0';
	bun_data_ = local_jl_open_lang();
	return(0);
    } else {
	c_c->use_server = 1;
    }

    for (p = c_c->normal_env; p; p = p->next) {
	c_c->cur_normal_env = p;
	get_envrc_name(uumrc_name, lang); /* use username for env-name */
	environment[0] = '\0';
	strcpy(environment, username);
	strcat(environment, p->env_name_str);
	if (servername == NULL && def_servername && *def_servername) {
	    servername = alloc_and_copy(def_servername);
	}
	if (bun_data_ == NULL) {
	    bun_data_ = jl_open_lang(environment, servername, lang, uumrc_name,
				     yes_or_no, print_msg_wait, WNN_TIMEOUT);
	    if (bun_data_ != NULL && bun_data_->env != NULL) {
		env_normal = bun_data_->env;
	    }
	} else if (env_normal == 0 || jl_isconnect_e(env_normal) == 0) {
	    env_normal = jl_connect_lang(environment, servername,
					  lang, uumrc_name, yes_or_no,
					  print_msg_wait, WNN_TIMEOUT);
	}
	if(normal_sticky && env_normal)
	  jl_env_sticky(bun_data_);
    }
    if (c_c->normal_env == NULL) {
	get_new_env(c_c, 0);
	c_c->use_server = 0;
	c_c->cur_normal_env = c_c->normal_env;
	uumrc_name[0] = '\0';
	bun_data_ = local_jl_open_lang();
    }

    for (p = c_c->reverse_env; p; p = p->next) {
	c_c->cur_reverse_env = p;
	if (get_envrc_name_reverse(uumrc_name, lang) > 0) {
	    environment[0] = '\0';
	    strcpy(environment, username);
	    strcat(environment, p->env_name_str);
	    strcat(environment, "R");
	    if (reverse_servername == NULL && def_reverse_servername
		&& *def_reverse_servername) {
		reverse_servername = alloc_and_copy(def_reverse_servername);
	    }
	    env_reverse = jl_connect_lang(environment, reverse_servername,
					  lang, uumrc_name, yes_or_no,
					  print_msg_wait, WNN_TIMEOUT);
	    if(reverse_sticky && env_reverse)
	      jl_env_sticky_e(env_reverse);
	}
    }
#ifdef  USING_XJUTIL
    if (!isstart_xjutil(lang))
    	xjutil_start();
#endif  /* USING_XJUTIL */
  return(0);
}

int
init_xcvtkey()
{
    char tmp_file[MAXPATHLEN];

    if (cvt_fun_file) {
	if (cvt_fun_setup(cvt_fun_file) == -1) {
	    print_out1("I can't open cvt_fun file \"%s\", use default file.",
		      cvt_fun_file);
	    cvt_fun_file = NULL;
	}
    }
    if (cvt_fun_file == NULL) {
	strcat(strcpy(tmp_file, LIBDIR), CVTFUNFILE);
	cvt_fun_file = alloc_and_copy(tmp_file);
	if (cvt_fun_setup(cvt_fun_file) == -1) {
	    print_out1(
  "I can't open default cvt_fun file \"%s\".\n I don't convert function keys.",
		       cvt_fun_file);
	    Free(cvt_fun_file);
	    cvt_fun_file = NULL;
	}
    }
    if (cvt_meta_file) {
	if (cvt_meta_setup(cvt_meta_file) == -1) {
	    print_out1("I can't open cvt_meta file \"%s\", use default file.",
		      cvt_meta_file);
	    cvt_meta_file = NULL;
	}
    }
    if (cvt_meta_file == NULL) {
	strcat(strcpy(tmp_file, LIBDIR), CVTMETAFILE);
	cvt_meta_file = alloc_and_copy(tmp_file);
	if (cvt_meta_setup(cvt_meta_file) == -1) {
	    print_out1(
    "I can't open default cvt_meta file \"%s\".\n I don't convert meta keys.",
		       cvt_meta_file);
	    Free(cvt_meta_file);
	    cvt_meta_file = NULL;
	}
    }
    return(0);
}
