/*
 * $Id: init.c,v 1.2 1991/09/16 21:36:53 ohm Exp $
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

#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"

static void
default_flag_set()
{
    cursor_colum = 0;
    cursor_reverse = 0;
    cursor_underline = 0;
    cursor_invisible = 1;
    cursor_bold = 0;
    quote_flag = 0;
    send_ascii_char = 0;
    send_ascii_char_def = 0;
    send_ascii_stack = 0;
    send_ascii_char_quote = 0;
    excellent_delete = 1;
    c_c->command_func_stack[0] = NULL;
    c_c->func_stack_count = -1;
    c_c->buffer_ignored = 0;
}

static void
copy_env(wc)
register WnnClientRec *wc;
{
    WnnEnv *p;

    for (p = wc->normal_env; p; p = p->next) {
	get_new_env(c_c, 0);
	envrcname = alloc_and_copy(p->envrc_name);
	servername = alloc_and_copy(p->host_name);
	strcpy(env_name, p->env_name_str);
    }
    for (p = wc->reverse_env; p; p = p->next) {
	get_new_env(c_c, 1);
	reverse_envrcname = alloc_and_copy(p->envrc_name);
	reverse_servername = alloc_and_copy(p->host_name);
	strcpy(reverse_env_name, p->env_name_str);
    }
    uumkeyname = alloc_and_copy(wc->uumkey_name);
    rkfilename = alloc_and_copy(wc->rkfile_name);

    maxchg = wc->m_chg;
    max_history = wc->m_history;
    maxbunsetsu = wc->m_bunsetsu;
    send_ascii_char = send_ascii_char_def = wc->s_ascii_char_def;
    excellent_delete = wc->e_delete;
    henkan_on_kuten = wc->h_on_kuten;
    henkan_off_def = wc->h_off_def;

    c_c->m_table = wc->m_table;
    c_c->m_table->re_count++;
}

static void
add_wnnclientlist(cl)
register WnnClientRec *cl;
{
    cl->next = wnnclient_list;
    wnnclient_list = cl;
}

static void
remove_wnnclientlist(cl)
register WnnClientRec *cl;
{
    register WnnClientList p, *prev;
    for (prev = &wnnclient_list; p = *prev; prev = &p->next) {
	if (p == cl) {
	    *prev = p->next;
	    break;
	}
    }
}

void
add_ximclientlist(cl)
register XIMClientRec *cl;
{
    cl->next = ximclient_list;
    ximclient_list = cl;
}

void
remove_ximclientlist(cl)
register XIMClientRec *cl;
{
    register XIMClientList p, *prev;
    for (prev = &ximclient_list; p = *prev; prev = &p->next) {
	if (p == cl) {
	    *prev = p->next;
	    break;
	}
    }
}

void
add_inputlist(xi)
register XIMInputRec *xi;
{
    xi->next = input_list;
    input_list = xi;
}

void
remove_inputlist(xi)
register XIMInputRec *xi;
{
    register XIMInputList p, *prev;
    for (prev = &input_list; p = *prev; prev = &p->next) {
	if (p == xi) {
	    *prev = p->next;
	    break;
	}
    }
}

static WnnClientRec *
find_same_lang(lang)
char *lang;
{
    register XIMClientRec *p;
    register int i;
    for (p = ximclient_list; p != NULL; p = p->next) {
	for (i = 0; i < p->lang_num; i++) {
	  if (p->xl[i]->w_c && p->xl[i]->w_c->m_table) {
	    if(!strcmp(p->xl[i]->lang, lang) && p->xl[i]->w_c != NULL) {
		return((WnnClientRec *)p->xl[i]->w_c);
	    }
	  }
	}
    }
    return((WnnClientRec *)NULL);
}

int
allocate_wnn()
{
    extern void bzero();

    if ((c_c = (WnnClientList)Malloc(sizeof(WnnClientRec))) == NULL) {
	return(-1);
    }
    bzero((char *)c_c, sizeof(WnnClientRec));
    if ((c_b = (ClientBuf *)Malloc(sizeof(ClientBuf))) == NULL) {
	Free((char *)c_c);
	return(-1);
    }
    bzero((char *)c_b, sizeof(ClientBuf));
    add_wnnclientlist(c_c);
    default_flag_set();
    return(0);
}

void
free_wnn()
{
    remove_wnnclientlist(c_c);
    Free((char *)c_b);
    Free((char *)c_c);
}

int
initialize_wnn(lang)
char *lang;
{
    WnnClientRec *p;

    if ((p = find_same_lang(lang)) != NULL) {
	copy_env(p);
    } else {
	if (uumrc_get_entries(lang, 0) == -1) {
	    return(-1);
	}
	if (init_key_table(lang) == -1) {
	    return(-1);
	}
    }
    henkan_off_flag = henkan_off_def;
    maxlength = maxchg * 2;
    if (allocate_areas() == -1) {
	return(-1);
    }
    if (init_history() == -1) {
	return(-1);
    }
    if(open_romkan(lang) == -1) {
	return(-1);
    }
    return(0);
}

