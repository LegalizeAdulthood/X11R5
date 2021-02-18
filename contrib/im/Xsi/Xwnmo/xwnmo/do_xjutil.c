/*
 * $Id: do_xjutil.c,v 1.5 1991/09/16 21:36:44 ohm Exp $
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
#ifdef	USING_XJUTIL

#include <stdio.h>
#include <signal.h>
#include <X11/Xos.h>
#include "config.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "proto.h"
#include "ext.h"

static LangUumrcRec *
set_xjutil(prop_name)
char *prop_name;
{
    register int i, nbytes;
    register unsigned char *p;
    XFontStruct **font_struct_list;
    char **font_name_list;
    int num;
    Atom id;
    register XIMLangRec *xl = cur_p->cur_xl;
    Xjutil_startRec xjutil_start;
    LangUumrcRec *lu;
    WnnEnv *we;
    unsigned char tmp_buf[256];

    for (lu = languumrc_list; lu != NULL; lu = lu->next) {
	if (!strcmp(lu->lang_name, xl->lang)) {
	    if (lu->xjutil_pid) return(NULL);
	    if (uumkeyname && *uumkeyname)
		strcpy(xjutil_start.uumkey_name, uumkeyname);
	    else
		xjutil_start.uumkey_name[0] = '\0';
	    if (lu->rkfile_name && *lu->rkfile_name)
		strcpy(xjutil_start.rkfile_name, lu->rkfile_name);
	    else
		xjutil_start.rkfile_name[0] = '\0';
	    if (cvt_fun_file && *cvt_fun_file)
		strcpy(xjutil_start.cvtfun_name, cvt_fun_file);
	    else
		xjutil_start.cvtfun_name[0] = '\0';
	    if (cvt_meta_file && *cvt_meta_file)
		strcpy(xjutil_start.cvtmeta_name, cvt_meta_file);
	    else
		xjutil_start.cvtmeta_name[0] = '\0';
	    if (username && *username)
		strcpy(xjutil_start.user_name, username);
	    else
		xjutil_start.user_name[0] = '\0';
	    if (xl->lang && *xl->lang)
		strcpy(xjutil_start.lang, xl->lang);
	    else
		xjutil_start.lang[0] = '\0';
	    if (xl->lc_name && *xl->lc_name)
		strcpy(xjutil_start.lc_name, xl->lc_name);
	    else
		xjutil_start.lc_name[0] = '\0';
	    nbytes = 0;
	    num = XFontsOfFontSet(xl->pe_fs, &font_struct_list,
				  &font_name_list);
	    for (i = 0; i < num; i++) {
		nbytes += (strlen(font_name_list[i]) + 1);
	    }
	    p = (unsigned char *)Malloc((unsigned)(nbytes + 1));
	    p[0] = '\0';
	    for (i = 0; i < num; i++) {
		strcat((char*)p, font_name_list[i]);
		strcat((char*)p, ",");
	    }
	    p[nbytes - 1] = '\0';
	    xjutil_start.fn_len = nbytes - 1;
	    xjutil_start.fore_ground = cur_p->pe.fg;
	    xjutil_start.back_ground = cur_p->pe.bg;

	    for (i = 0, we = c_c->normal_env; we; we = we->next, i++);
	    xjutil_start.max_env = i;

	    strcpy(prop_name, XJUTIL_START_PRO);
	    strcat(prop_name, xl->lang);
	    id = XInternAtom(dpy, prop_name, False);
	    XChangeProperty(dpy, root_window, id, XA_STRING, 8, PropModeReplace,
			    (unsigned char *)&xjutil_start,
			    sizeof(Xjutil_startRec));
	    
	    for (i = 0, we = c_c->normal_env; we; we = we->next, i++) {
		tmp_buf[0] = '\0';
		if (we->host_name) strcpy((char*)tmp_buf, we->host_name);
		XChangeProperty(dpy, root_window, id, XA_STRING, 8,
				PropModeAppend, tmp_buf, 256);
		tmp_buf[0] = '\0';
		if (we->envrc_name) strcpy((char*)tmp_buf, we->envrc_name);
		XChangeProperty(dpy, root_window, id, XA_STRING, 8,
				PropModeAppend, tmp_buf, 256);
		tmp_buf[0] = '\0';
		strcpy((char*)tmp_buf, we->env_name_str);
		XChangeProperty(dpy, root_window, id, XA_STRING, 8,
				PropModeAppend, tmp_buf, 32);
	    }
	    for (i = 0, we = c_c->reverse_env; we; we = we->next, i++) {
		tmp_buf[0] = '\0';
		if (we->host_name) strcpy((char*)tmp_buf, we->host_name);
		XChangeProperty(dpy, root_window, id, XA_STRING, 8,
				PropModeAppend, tmp_buf, 256);
		tmp_buf[0] = '\0';
		if (we->envrc_name) strcpy((char*)tmp_buf, we->envrc_name);
		XChangeProperty(dpy, root_window, id, XA_STRING, 8,
				PropModeAppend, tmp_buf, 256);
		tmp_buf[0] = '\0';
		strcpy((char*)tmp_buf, we->env_name_str);
		XChangeProperty(dpy, root_window, id, XA_STRING, 8,
				PropModeAppend, tmp_buf, 32);
	    }
	    XChangeProperty(dpy, root_window, id, XA_STRING, 8, PropModeAppend,
			    p, strlen((char*)p));
	    XSetSelectionOwner(dpy, id, xim->ximclient->w,  0L);
	    XFlush(dpy);
	    lu->xjutil_fs_act = 1;
	    xl->xjutil_fs_id = 0;
	    XFree(p);
	    return(lu);
	}
    }
    return(NULL);
}

static void
exec_xjutil(prop_name, counter)
char *prop_name;
int counter;
{
    char tmp[2];
    unsigned char *p, tmp_buf[256];

    tmp[0] = (char)counter;
    tmp[1] = '\0';
    if (xjutil_name == NULL) {
	strcpy((char*)tmp_buf, XJUTILBINDIR);
	strcat((char*)tmp_buf, "/");
	strcat((char*)tmp_buf, XJUTIL_NAME);
	p  = tmp_buf;
    } else {
	p = (unsigned char *)xjutil_name;
    }
    execlp((char*)p, (char*)p, DisplayString(dpy), prop_name, tmp, 0);
}

void
xjutil_start()
{
    char prop_name[32];
    static int counter = 0;
    LangUumrcRec *lu = NULL;

    lu = set_xjutil(prop_name);
    signal(SIGCHLD, SIG_IGN);
#ifndef	NOTFORK_XJUTIL
    if (lu != NULL) {
	counter++;
	if ((lu->xjutil_pid = fork()) == 0) {
	    close(dpy->fd);
	    exec_xjutil(prop_name, counter);
	    exit(0);
	}
	lu->xjutil_act = counter;
    }
#endif	/* NOTFORK_XJUTIL */
}

void
kill_xjutil(lu)
LangUumrcRec *lu;
{
    extern int kill();

    if (lu->xjutil_pid > 0) {
	kill(lu->xjutil_pid, SIGTERM);
	lu->xjutil_act = 0;
	lu->xjutil_id = 0;
	lu->xjutil_pid = 0;
	lu->xjutil_fs_act = 0;
    }
}

void
kill_lang_xjutil(lang)
char *lang;
{
    LangUumrcRec *lu;

    for (lu = languumrc_list; lu != NULL; lu = lu->next) {
	if (!strcmp(lang, lu->lang_name)) {
	    kill_xjutil(lu);
	    return;
	}
    }
    return;
}

void
kill_all_xjutil()
{
    LangUumrcRec *lu;

    for (lu = languumrc_list; lu != NULL; lu = lu->next) {
	if (lu->xjutil_pid > 0) {
	    kill_xjutil(lu);
	}
    }
    return;
}

static void
send_xjutil_event(id)
int	id;
{
    XEvent event;

    event.type = ClientMessage;
    event.xclient.format = 32;
    event.xclient.window = xim->ximclient->w;
    event.xclient.data.l[0] = id;
    XSendEvent(dpy, cur_p->cur_xl->lang_uumrc->xjutil_id,
		False, NoEventMask, &event);
    XFlush(dpy);
    return;
}

void
set_xjutil_id(w, act, error)
Window w;
int act;
char error;
{
    LangUumrcRec *lu;

    for (lu = languumrc_list; lu != NULL; lu = lu->next) {
	if (lu->xjutil_act == act) {
	    if (error) {
		lu->xjutil_id = 0;
		lu->xjutil_pid = 0;
		lu->xjutil_fs_act = 0;
	    } else {
		lu->xjutil_id = w;
	    }
	    return;
	}
    }
    return;
}

void
reset_xjutil_fs_id(id, act)
int id;
int act;
{
    register int i;
    LangUumrcRec *lu;
    XIMClientList xc = ximclient_list;
    XIMLangRec **xl;

    for (lu = languumrc_list; lu != NULL; lu = lu->next) {
	if (lu->xjutil_act == act) break;
    }
    if (lu == NULL) return;
    for (; xc != NULL; xc = xc->next) {
	for (i = 0, xl = xc->xl; i < xc->lang_num; i++) {
	    if ((xl[i]->lang_uumrc == lu) && (xl[i]->xjutil_fs_id == id)) {
		xl[i]->xjutil_fs_id = -1;
		return;
	    }
	}
    }
    return;
}

int
isstart_xjutil(lang)
char *lang;
{
    Atom ret;
    char tmp[64];

    strcpy(tmp, XJUTIL_START_PRO);
    strcat(tmp, lang);
    strcat(tmp, "_OK");
    if (ret = XInternAtom(dpy, tmp, True)) {
	if (XGetSelectionOwner(dpy, ret)) {
	    return(1);
	}
    }
    return(0);
}

static Atom
set_uum_env()
{
    Atom id;
    Xjutil_envRec xjutil_env;
    register int i, nbytes = 0;
    register unsigned char *p = NULL;
    XFontStruct **font_struct_list;
    char **font_name_list;
    int num;
    WnnEnv *we;

    if((id = XInternAtom(dpy, XJUTIL_ENV_PRO, True)) == 0) {
	id = XInternAtom(dpy, XJUTIL_ENV_PRO, False);
    }
    if (cur_p->cur_xl->xjutil_fs_id == -1) {
	nbytes = 0;
	num = XFontsOfFontSet(cur_p->cur_xl->pe_fs, &font_struct_list,
			      &font_name_list);
	for (i = 0; i < num; i++) {
	    nbytes += (strlen(font_name_list[i]) + 1);
	}
	p = (unsigned char *)Malloc((unsigned)(nbytes + 1));
	p[0] = '\0';
	for (i = 0; i < num; i++) {
	    strcat((char*)p, font_name_list[i]);
	    strcat((char*)p, ",");
	}
	p[nbytes - 1] = '\0';
	xjutil_env.fs_id = -1;
	xjutil_env.fn_len = nbytes - 1;
    } else {
	xjutil_env.fs_id = cur_p->cur_xl->xjutil_fs_id;
	xjutil_env.fn_len = 0;
    }
    xjutil_env.fore_ground = cur_p->pe.fg;
    xjutil_env.back_ground = cur_p->pe.bg;
    xjutil_env.cur_env_id = c_c->cur_normal_env->env->env_id;
    xjutil_env.cur_env_reverse_id = c_c->cur_reverse_env->env->env_id;
    for (i = 0, we = c_c->normal_env; we; we = we->next, i++) {
	xjutil_env.env_id[i] = we->env->env_id;
    }
    for (i = 0, we = c_c->reverse_env; we; we = we->next, i++) {
	xjutil_env.env_reverse_id[i] = we->env->env_id;
    }
    XChangeProperty(dpy, root_window, id, XA_STRING, 8,
		    PropModeReplace, (unsigned char *)&xjutil_env,
		    sizeof(Xjutil_envRec));
    if (nbytes > 0) {
	XChangeProperty(dpy, root_window, id, XA_STRING, 8,
			PropModeAppend, p, strlen((char*)p));
	Free(p);
    }
    XFlush(dpy);
    return(id);
}

static void
set_uum_touroku(start)
int start;
{
    Atom id;

    id = set_uum_env();
    XChangeProperty(dpy, root_window, id, XA_STRING, 8,
		    PropModeAppend, (unsigned char*)&start, sizeof(int));
    XChangeProperty(dpy, root_window, id, XA_STRING, 8,
		    PropModeAppend, (unsigned char*)c_b->buffer,
		     Strlen(c_b->buffer)*2);
    XFlush(dpy);
}

void
xw_jishoadd()
{
    (void)set_uum_env();
    send_xjutil_event(DIC_ADD);
}

void
xw_kensaku()
{
    (void)set_uum_env();
    send_xjutil_event(DIC_KENSAKU);
}

void
xw_select_one_dict9()
{
    (void)set_uum_env();
    send_xjutil_event(DIC_ICHIRAN);
}

void
xw_paramchg()
{
    (void)set_uum_env();
    send_xjutil_event(DIC_PARAM);
}

void
xw_dicinfoout()
{
    (void)set_uum_env();
    send_xjutil_event(DIC_INFO);
}

void
xw_fuzoku_set()
{
    (void)set_uum_env();
    send_xjutil_event(DIC_FZK);
}

void
xw_touroku(s)
int s;
{
    set_uum_touroku(s);
    send_xjutil_event(DIC_TOUROKU);
}

#endif	/* USING_XJUTIL */
