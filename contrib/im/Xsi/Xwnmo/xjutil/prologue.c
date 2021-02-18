/*
 * $Id: prologue.c,v 1.2 1991/09/16 21:35:24 ohm Exp $
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
#include "jslib.h"
#include "jd_sock.h"
#include "commonhd.h"
#include "config.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"
#include "xext.h"

extern char     *getenv();
extern FILE	*fopen();

int
open_romkan()
{
    int rk_option_flg;
    extern int keyin();

    if (rkfile_name == NULL) return(-1);
    cur_rk_table = (RomkanTable *)romkan_table_init(NULL, rkfile_name, NULL, 0);
    rk_option_flg = RK_REDRAW | RK_CHMOUT | (excellent_delete? 0 : RK_SIMPLD);

    cur_rk = (Romkan *)romkan_init5(rubout_code,NULL,NULL,rk_option_flg);
    if (cur_rk == NULL) {
	return(-1);
    } else {
	return(0);
    }
}

static int
allocate_areas()
{
    char *area_start;
    char *area_pter;

    int k;
    int total_size;
    int len1 = maxchg * sizeof(w_char);
    int return_buf_len = len1 * 7;

    total_size = return_buf_len + len1 * 3 + maxbunsetsu * sizeof(int) * 3;

    if ((area_start = (char *)Malloc(total_size)) == NULL){
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

int
init_wnn()   
{
    if (init_key_table(xjutil->lang) == -1) {
	return(-1);
    }

    if(open_romkan() == -1){
	return(-1);
    }

    if (allocate_areas() == -1) {
	return(-1);
    }
    return(0);
}

int
connect_server()
{
    register WNN_JSERVER_ID *js = NULL;
    struct hostent *hent;
    char *machine_n;
    WnnEnv *p;
    int i;
    char environment[PATHNAMELEN];
    extern char *_wnn_get_machine_of_serv_defs();
	
    for (i = 0, p = normal_env; p ; p = p->next, i++) {
	cur_normal_env = p;
	js = NULL;
	if (!p->host_name || !*p->host_name) {
	    if (machine_n = _wnn_get_machine_of_serv_defs(xjutil->lang)) {
		if (hent = gethostbyname(machine_n)) {
		    p->host_name = alloc_and_copy(hent->h_name);
		    if ((js = js_open_lang(p->host_name, xjutil->lang,
					   WNN_TIMEOUT)) == NULL) {
			p->host_name = NULL;
		    }
		}
	    }
	}
	if (js == NULL) {
	    if ((js = js_open_lang(p->host_name, xjutil->lang, WNN_TIMEOUT))
		== NULL) {
		return(-1);
	    }
	}
	environment[0] = '\0';
	strcpy(environment, username);
	strcat(environment, p->env_name_str);
	if((p->env = js_connect_lang(js, environment, xjutil->lang)) == NULL) {
	    return(-1);
	}
	save_env_id[i] = p->env->env_id;
    }
    return(0);
}

int
init_xcvtkey()
{
    if (cvt_fun_setup(cvt_fun_file) == -1) {
	fprintf(stderr, "File \"%s\" can't open.", cvt_fun_file);
	return(-1);
    }
    if(cvt_meta_setup(cvt_meta_file) == -1) {
	fprintf(stderr, "File \"%s\" can't open.", cvt_meta_file);
	return(-1);
    }
    return(0);
}
