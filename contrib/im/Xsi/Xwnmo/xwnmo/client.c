/*
 * $Id: client.c,v 1.5 1991/09/16 21:36:32 ohm Exp $
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
#include <stdio.h>
#include "commonhd.h"
#include "config.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"

/*
 *	env. control routines
 */

char
env_state()
{
    char *p;
    char ret = '\0';
#ifndef SYSVR2
    extern char *index();
#else
    extern char *strchr();
#endif

    if ((p = romkan_dispmode()) == NULL) return(ret);
#ifndef SYSVR2
    if ((p = (char*)index(p, ':')) == NULL) return(ret);
#else
    if ((p = (char*)strchr(p, ':')) == NULL) return(ret);
#endif
    return(*(p + 1));
}

int
set_cur_env(s)
char s;
{
    register WnnEnv *p;
    register int i;

    for (p = c_c->normal_env; p; p = p->next) {
	for (i = 0; p->env_name_str[i]; i++) {
	    if (s == p->env_name_str[i]) {
		c_c->cur_normal_env = p;
		break;
	    }
	}
    }
    for (p = c_c->reverse_env; p; p = p->next) {
	for (i = 0; p->env_name_str[i]; i++) {
	    if (s == p->env_name_str[i]) {
		c_c->cur_reverse_env = p;
		break;
	    }
	}
    }
    return(0);
}

void
get_new_env(wc, rev)
WnnClientRec *wc;
int rev;
{
    register WnnEnv *p;

    if (wc == NULL) return;
    p = (WnnEnv *)Malloc(sizeof(WnnEnv));
    p->host_name = NULL;
    p->env = NULL;
    p->sticky = 0;
    p->envrc_name = NULL;
    p->env_name_str[0] = '\0';
    if (rev) {
	p->next = wc->reverse_env;
	wc->reverse_env = wc->cur_reverse_env = p;
    } else {
	p->next = wc->normal_env;
	wc->normal_env = wc->cur_normal_env = p;
    }
}

static void
free_env(wnnenv)
WnnEnv *wnnenv;
{
    WnnEnv *p, *next;

    for (p = wnnenv; p; p = next) {
	if (p->host_name) Free((char *)p->host_name);
	if (p->envrc_name) Free((char *)p->envrc_name);
	next = p->next;
	Free((char *)p);
    }
}

static void
set_server_name()
{
    if ((def_servername == NULL || *def_servername == '\0') &&
	root_def_servername != NULL && (*(root_def_servername)!= '\0'))
	def_servername = alloc_and_copy(root_def_servername);

    if ((def_reverse_servername == NULL || *def_reverse_servername == '\0') &&
	(root_def_reverse_servername != NULL) &&
	(*(root_def_reverse_servername) != '\0'))
	def_reverse_servername = alloc_and_copy(root_def_reverse_servername);
}

void
set_function_table(wc, lang)
register WnnClientRec *wc;
char *lang;
{
    register XIMLangDataBase *db = NULL;

    for (db = language_db; db->lang != NULL; db++) {
	if (!strcmp(db->lang, lang)) {
	    bcopy(&(db->f_table), &(wc->f_table), sizeof(FunctionTable));
	    return;
	}
    }
    bcopy(&(default_func_table), &(wc->f_table), sizeof(FunctionTable));
    return;
}

static int
create_c_c(xc, xl, root)
register XIMClientRec *xc;
register XIMLangRec *xl;
Bool root;
{
    register LangUumrcRec *lu;

    xc->cur_xl = xl;
    cur_lang = xl->lang;
    if (allocate_wnn() == -1) {
	return(-1);
    }
    xl->w_c = c_c;
    if (root == True) {
	def_servername = root_def_servername;
	def_reverse_servername = root_def_reverse_servername;
	henkan_off_def = root_henkan_off_def;
	uumkeyname = root_uumkeyname;
	rkfilename = root_rkfilename;
    }
    username = alloc_and_copy(xc->user_name);
    set_function_table(c_c, xl->lang);
    set_server_name();
    if (initialize_wnn(xl->lang) == -1) {
	free_wnn();
	return(-1);
    }
    connect_server(xl->lang);
    for (lu = languumrc_list; lu != NULL; lu = lu->next) {
	if (!strcmp(lu->lang_name, xl->lang)) {
	    xl->lang_uumrc = lu;
	    break;
	}
    }
    return(0);
}

int
new_client(xc, xl, root)
XIMClientRec *xc;
XIMLangRec *xl;
Bool root;
{
    if (IsPreeditArea(xc) || IsPreeditPosition(xc)
#ifdef	CALLBACKS
	|| IsPreeditCallbacks(xc)
#endif	/* CALLBACKS */
	) {
	if (create_c_c(xc, xl, root) == -1) {
	    return(-1);
	}
	if (create_preedit(xc, xl) < 0) {
	    free_wnn();
	    return(-1);
	}
	clear_c_b();
    } else if (IsPreeditNothing(xc)) {
	if (create_preedit(xc, NULL) < 0) {
	    return(-1);
	}
	xc->yes_no = NULL;
    }
    if (IsStatusArea(xc)
#ifdef	CALLBACKS
	|| IsStatusCallbacks(xc)
#endif	/* CALLBACKS */
	) {
	xc->cur_xl = xl;
	if (create_status(xc, xl) < 0) {
	    free_wnn();
	    return(-1);
	}
	if (xc->cur_xl->w_c->h_flag) {
	    push_func(xc->cur_xl->w_c, henkan_off);
	}
    } else if (IsStatusNothing(xc)) {
	if (create_status(xc, NULL) < 0) {
	    return(-1);
	}
    }
    return(0);
}

void
del_client(wc)
register WnnClientRec *wc;
{
    register WnnClientRec *c_c_sv;
    register WnnEnv *p;

    c_c_sv = c_c;
    c_c = wc;
    free_areas();
    if (c_c->use_server) {
	for (p = c_c->normal_env; p; p = p->next) {
	    if (jl_isconnect_e(p->env)) {
		jl_dic_save_all_e(p->env);
		jl_disconnect(p->env);
	    }
	}
	free_env(c_c->normal_env);
	for (p = c_c->reverse_env; p; p = p->next) {
	    if (jl_isconnect_e(p->env)) {
		jl_dic_save_all_e(p->env);
		jl_disconnect(p->env);
	    }
	}
	free_env(c_c->reverse_env);
    }
    jl_env_set(bun_data_, NULL);
    jl_close(bun_data_);
    rk_close(c_c->rk);
    if (c_c->m_table->re_count == 1) {
	Free((char *)c_c->m_table);
    } else {
	c_c->m_table->re_count--;
    }

    if (def_servername) Free((char *)def_servername);
    if (def_reverse_servername) Free((char *)def_servername);
    if (uumkeyname) Free((char *)uumkeyname);
    if (rkfilename) Free((char *)rkfilename);
    if (username) Free((char *)username);
    destroy_history();
    free_wnn();
    c_c = c_c_sv;
}

void
epilogue()
{
    register WnnClientRec *wc;

    for (wc = wnnclient_list; wc != NULL; wc = wc->next) {
	c_c = wc;
	disconnect_server();
    }
}

