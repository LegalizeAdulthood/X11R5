/*
 * $Id: multi_lang.c,v 1.4 1991/09/16 21:37:10 ohm Exp $
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
#include "sdefine.h"
#include "xim.h"
#include "Xlocaleint.h"
#include "sheader.h"
#include "proto.h"
#include "ext.h"


XSetWindowAttributes	attributes;
XGCValues	xgcv;

char *
get_default_font_name(lang)
register char *lang;
{
    register XIMLangDataBase *db = NULL;

    for (db = language_db; db->lang != NULL; db++) {
	if (!strcmp(db->lang, lang)) {
	    return(db->fontset_name);
	}
    }
    return(NULL);
}

int
load_font_set(xl, p, s)
register XIMLangRec *xl;
register char *p, *s;
{
    register XIMLangDataBase *db = NULL;
    char **miss;
    int miss_cnt;
    char *def;
    XFontSet fs;
    XLocale xlc;

    for (db = language_db; db->lang != NULL; db++) {
	if (!strcmp(db->lang, xl->lang)) break;
    }
    if (db->lang == NULL) return(-1);
    xl->lc_name = alloc_and_copy(db->lc_name);
    setlocale(LC_ALL, xl->lc_name);
    xlc = _XFallBackConvert();
    xl->xlc = _XlcDupLocale(xlc);

    if (((fs = XCreateFontSet(dpy, p, &miss, &miss_cnt, &def)) == NULL)/* ||
	(miss_cnt > 0)*/) {
	return(-1);
    }
    xl->pe_fs = fs;
    if ((xl->pe_b_char = get_base_char(xl->pe_fs)) == NULL) return(-1);
    if (((fs = XCreateFontSet(dpy, s, &miss, &miss_cnt, &def)) == NULL)/* ||
	(miss_cnt > 0)*/) {
	return(-1);
    }
    xl->st_fs = fs;
    if ((xl->st_b_char = get_base_char(xl->st_fs)) == NULL) return(-1);
    return(0);
}

void
add_locale_to_xl(xl, lang)
XIMLangRec *xl;
XIMNestLangRec *lang;
{
    register XIMNestLangRec *lr;

    lr = (XIMNestLangRec *)Calloc(1, sizeof(XIMNestLangRec));
    lr->next = xl->lc_list;
    xl->lc_list = lr;

    lr->lc_name = alloc_and_copy(lang->lc_name);
    if (lang->alias_name) {
	lr->alias_name = alloc_and_copy(lang->alias_name);
    } else {
	lr->alias_name = NULL;
    }
    lr->lang = NULL;
}

void
remove_locale_to_xl(xl, lang)
XIMLangRec *xl;
XIMNestLangRec *lang;
{
    register XIMNestLangRec *p, **prev;

    for (prev = &xl->lc_list; p = *prev; prev = &p->next) {
	if (lang == NULL || lang == *prev) {
	    if (p->lc_name) Free((char *)p->lc_name);
	    if (p->alias_name) Free((char *)p->alias_name);
	    *prev = p->next;
	    Free((char *)p);
	    if (lang) break;
	}
    }
}

static int
create_lang_env(xc, num, lang, pre_font, st_font, detail)
register XIMClientRec *xc;
int num;
XIMNestLangList lang;
char *pre_font, *st_font;
short *detail;
{
    register XIMLangRec *xl;

#ifdef	CALLBACKS
    if (IsPreeditCallbacks(xc)) {
	if (pre_font == NULL || *pre_font == '\0') {
	    if ((pre_font = get_default_font_name(lang->lang)) == NULL) {
		*detail = BadFontSet;
		return(0);
	    }
	}
    }
    if (IsStatusCallbacks(xc)) {
	if (st_font == NULL || *st_font == '\0') {
	    if ((st_font = get_default_font_name(lang->lang)) == NULL) {
		*detail = BadFontSet;
		return(0);
	    }
	}
    }
#endif	/* CALLBACKS */
    if((xl = xc->xl[num]
	   = (XIMLangRec *)Calloc(1, sizeof(XIMLangRec))) == NULL) {
	*detail = AllocError;
	return(-1);
    }
    xl->lang = alloc_and_copy(lang->lang);
    xl->lc_list = NULL;
    add_locale_to_xl(xl, lang);
    xl->w_c = NULL;
#ifdef	USING_XJUTIL
    xl->xjutil_fs_id = -1;
#endif	/* USING_XJUTIL */
    if (IsPreeditArea(xc) || IsPreeditPosition(xc) || IsPreeditCallbacks(xc)) {
	/*
	 * Loading multipule fonts
	 */
	if (load_font_set(xl, pre_font, st_font) == -1) {
	    Free((char *)xl->lang);
	    free_langlist(xl->lc_list);
	    if (xl->lc_name) Free(xl->lc_name);
	    if (xl->pe_fs) XFreeFontSet(dpy, xl->pe_fs);
	    if (xl->pe_b_char) Free(xl->pe_b_char);
	    if (xl->st_fs) XFreeFontSet(dpy, xl->st_fs);
	    if (xl->st_b_char) Free(xl->st_b_char);
	    Free((char *)xl);
	    *detail = BadFontSet;
	    return(0);
	}
	/*
	 * Creating GC
	 */
	if (num == 0) {
	    xgcv.foreground = xc->pe.fg;
	    xgcv.background = xc->pe.bg;
	    xc->pe.gc = XCreateGC(dpy, DefaultRootWindow(dpy),
				  (GCForeground|GCBackground), &xgcv);
	    xgcv.foreground = xc->pe.bg;
	    xgcv.background = xc->pe.fg;
	    xc->pe.reversegc = XCreateGC(dpy, DefaultRootWindow(dpy),
					 (GCForeground|GCBackground),
					 &xgcv);
	    xgcv.foreground = xc->pe.bg;
	    xgcv.function = GXinvert;
	    xgcv.plane_mask = XOR(xc->pe.fg, xc->pe.bg);
	    xc->pe.invertgc = XCreateGC(dpy, DefaultRootWindow(dpy),
					(GCForeground| GCFunction |
					 GCPlaneMask), &xgcv);
	    xgcv.foreground = xc->st.fg;
	    xgcv.background = xc->st.bg;
	    xc->st.gc = XCreateGC(dpy, DefaultRootWindow(dpy),
				  (GCForeground|GCBackground), &xgcv);
	    xgcv.foreground = xc->st.bg;
	    xgcv.background = xc->st.fg;
	    xc->st.reversegc = XCreateGC(dpy, DefaultRootWindow(dpy),
					 (GCForeground|GCBackground),
					 &xgcv);
	    if (IsPreeditArea(xc)) {
		xc->max_columns = ((int)PreeditWidth(xc)/(int)FontWidth(xl));
		xc->columns = xc->max_columns;
	    } else if (IsPreeditPosition(xc)) {
		xc->max_columns = ((int)xc->client_area.width/(int)FontWidth(xl));
		xc->columns = xc->max_columns;
	    }
	    xc->yes_no = (YesOrNo *)Malloc(sizeof(YesOrNo));
	    create_yes_no(xc);
	}
    }
    if (new_client(xc, xl, False) == -1) {
	Free((char *)xl->lang);
	free_langlist(xl->lc_list);
	if (xl->lc_name) Free(xl->lc_name);
	if (xl->pe_fs) XFreeFontSet(dpy, xl->pe_fs);
	if (xl->pe_b_char) Free(xl->pe_b_char);
	if (xl->st_fs) XFreeFontSet(dpy, xl->st_fs);
	if (xl->st_b_char) Free(xl->st_b_char);
	return(0);
    }
    return(1);
}

int
add_lang_env(xc, lang, new)
register XIMClientRec *xc;
XIMNestLangRec *lang;
int *new;
{
    register int i;
    Status match = False;
    int ret;
    short detail;

    *new = 0;
    for (i = 0; i < xc->lang_num; i++) {
	if (!strcmp(lang->lang, xc->xl[i]->lang)) {
	    match = True;
	    break;
	}
    }
    if (match == True) {
	add_locale_to_xl(xc->xl[i], lang);
	return(i);
    }
    if ((xc->xl = (XIMLangRec**)Realloc(xc->xl,
					(sizeof(XIMLangRec*)
					 * (xc->lang_num + 1)))) == NULL) {
	return(-1);
    }
    if ((ret = create_lang_env(xc, xc->lang_num, lang, xc->pe.fontset,
			       xc->st.fontset, &detail)) <= 0) {
	return(-1);
    } 
    *new = 1;
    return(xc->lang_num);
}

void
default_xc_set(xc)
register XIMClientRec *xc;
{
    XWindowAttributes attr;

    if (xc->w) {
	XGetWindowAttributes(dpy, xc->w, &attr);
    } else {
	XGetWindowAttributes(dpy, root_window, &attr);
    }
    xc->filter_events = MaskNeeded;
    xc->client_area.x = attr.x;
    xc->client_area.y = attr.y;
    xc->client_area.width = attr.width;
    xc->client_area.height = attr.height;
    xc->pe.colormap = attr.colormap;
    xc->pe.std_colormap = 0;
    xc->pe.fg = xim->ximclient->pe.fg;
    xc->pe.bg = xim->ximclient->pe.bg;
    xc->pe.bg_pixmap = NULL;
    xc->pe.line_space = 0;
    xc->pe.cursor = NULL;
    xc->st.colormap = attr.colormap;
    xc->st.std_colormap = 0;
    xc->st.fg = xim->ximclient->st.fg;
    xc->st.bg = xim->ximclient->st.bg;
    xc->st.bg_pixmap = NULL;
    xc->st.line_space = 0;
    xc->st.cursor = NULL;
#ifdef	CALLBACKS
    xc->cb_pe_start = xc->cb_st_start = 0;
#endif	/* CALLBACKS */
}

XIMClientRec *
create_client(ic_req, pre_req, st_req, lc_list, init_lc, languages,
	      pre_font, st_font, c_data, detail)
ximICValuesReq *ic_req;
ximICAttributesReq *pre_req, *st_req;
XIMNestLangList lc_list, init_lc;
char *languages;
char *pre_font, *st_font;
char *c_data;
short *detail;
{
    register XIMClientRec *xc;
    register i, num, num_xl = 0;
    register WnnClientRec *save_c_c;
    register XIMClientRec *save_cur_p, *save_cur_x;
    register XIMNestLangList p, xp;
    register char *cur_lang_sv;
    XIMLangRec *cur_xl = NULL;
    XIMNestLangRec *cur_lc = NULL;
    int ret;
    int count;
    Status match = False, same = False;
#ifdef	X11R3
    XStandardColormap map_info;
#else	/* X11R3 */
    XStandardColormap *map_info;
#endif	/* !X11R3 */
    XWindowAttributes attr;

    if ((xc = (XIMClientRec *)Calloc(1, sizeof(XIMClientRec))) == NULL) {
	*detail = AllocError;
	return(NULL);
    }
    xc->cur_xl = NULL;
    xc->next = NULL;
    xc->using_language = alloc_and_copy(languages);
    xc->using_language[0] = '\0';
    xc->user_name = alloc_and_copy(c_data);
    if (!xc->user_name)
	xc->user_name = alloc_and_copy(xim->ximclient->user_name);
    if (!(xc->mask & (1 << ICClientWindow)))
	xc->w = DefaultRootWindow(dpy);
    else
	xc->w = ic_req->c_window;
    xc->mask = ic_req->mask;
    xc->fd = get_cur_sock();
    xc->focus_window = xc->w;
#ifdef	CALLBACKS
    xc->max_keycode = ic_req->max_keycode;
#endif	/* CALLBACKS */
    default_xc_set(xc);
    xc->have_world = 0;
    xc->world_on = 0;

    if (xc->w) XSelectInput(dpy, xc->w, StructureNotifyMask);

    update_ic(xc, ic_req, pre_req, st_req, pre_font, st_font);

    if (xc->focus_window != xc->w) {
	XSelectInput(dpy, xc->focus_window, StructureNotifyMask);
	XGetWindowAttributes(dpy, xc->focus_window, &attr);
	xc->focus_area.x = attr.x;
	xc->focus_area.y = attr.y;
	xc->focus_area.width = attr.width;
	xc->focus_area.height = attr.height;
    } else {
	xc->focus_area.x = 0;
	xc->focus_area.y = 0;
	xc->focus_area.x = xc->focus_area.y = 0;
	xc->focus_area.width = xc->focus_area.height = 0;
    }

    if (xc->pe.std_colormap) {
#ifdef	X11R3
	if (XGetStandardColormap(dpy, root_window, &map_info,
				 xc->pe.std_colormap)) {
	    xc->pe.colormap = map_info.colormap;
#else	/* X11R3 */
	if (XGetRGBColormaps(dpy, root_window, &map_info, &count,
			     xc->pe.std_colormap)) {
	    xc->pe.colormap = map_info->colormap;
#endif	/* X11R3 */
	}
    }
    if (xc->st.std_colormap) {
#ifdef	X11R3
	if (XGetStandardColormap(dpy, root_window, &map_info,
				 xc->st.std_colormap)) {
	    xc->st.colormap = map_info.colormap;
#else	/* X11R3 */
	if (XGetRGBColormaps(dpy, root_window, &map_info, &count,
			     xc->st.std_colormap)) {
	    xc->pe.colormap = map_info->colormap;
#endif	/* X11R3 */
	}
    }

    if (!(IsPreeditNothing(xc))) {
	if (ClientWidth(xc) <= 0 || ClientHeight(xc) <= 0) {
	    Free((char *)xc);
	    return(NULL);
	}
	if (pre_font == NULL) {
	    if (IsPreeditArea(xc) || IsPreeditPosition(xc)) {
		Free((char *)xc);
		*detail = BadFontSet;
		return(NULL);
	    }
	}
	if (st_font == NULL) {
	    if (IsStatusArea(xc)) {
		st_font = pre_font;
	    }
	}
	for (num = 0, p = lc_list; p != NULL; p = p->next) num++;
	if ((xc->xl = (XIMLangRec**)Malloc(sizeof(XIMLangRec*) * num)) == NULL) {
	    Free((char *)xc);
	    *detail = BadFontSet;
	    return(NULL);
	}
	/*
	 * Create lang env.
	 */
	save_c_c = c_c;
	save_cur_p = cur_p;
	save_cur_x = cur_x;
	cur_lang_sv = cur_lang;
	cur_p = cur_x = xc;
	for (i = 0, p = lc_list; p != NULL; p = p->next) {
	    if (!strcmp(p->lc_name, "wr_WR.ct")) {
		if (init_lc && !strcmp(p->lc_name, init_lc->lc_name)) {
		    xc->world_on = 1;
		}
		xc->have_world = 1;
		continue;
	    }
	    match = False;
	    same = False;
	    for (i = 0; i < num_xl; i++) {
		if (!strcmp(p->lang, xc->xl[i]->lang)) {
		    match = True;
		    break;
		}
	    }
	    if (match == True) {
		for (xp = xc->xl[i]->lc_list; xp != NULL; xp = xp->next) {
		    if (!strcmp(p->lc_name, xp->lc_name)) {
			same = True;
			break;
		    }
		}
		if (same == True) continue;
		add_locale_to_xl(xc->xl[i], p);
		if (init_lc && !strcmp(p->lc_name, init_lc->lc_name)) {
		    cur_xl = xc->xl[i];
		    cur_lc = cur_xl->lc_list;
		}
	    } else {
		if ((ret = create_lang_env(xc, num_xl, p, pre_font, st_font,
					   detail)) == -1) {
		    Free((char *)xc->xl);
		    Free((char *)xc);
		    return(NULL);
		} else if (ret == 0) {
		    continue;
		}
		if (init_lc && !strcmp(p->lc_name, init_lc->lc_name)) {
		    cur_xl = xc->xl[i];
		    cur_lc = cur_xl->lc_list;
		}
		num_xl++;
	    }
	    strcat(xc->using_language, p->lc_name);
	    strcat(xc->using_language, ";");
	}
	c_c = save_c_c;
	cur_p = save_cur_p;
	cur_x = save_cur_x;
	cur_lang = cur_lang_sv;
	if (c_c) {
	    cur_rk = c_c->rk;
	    cur_rk_table = cur_rk->rk_table;
	}
	if (num_xl == 0) {
	    Free((char *)xc->xl);
	    Free((char *)xc);
	    *detail = BadLanguage;
	    return(NULL);
	}
	xc->using_language[strlen(xc->using_language) - 1] = '\0';
	if (cur_xl == NULL) {
	    cur_xl = xc->xl[0];
	    cur_lc = cur_xl->lc_list;
	}
	xc->lang_num = num_xl;
    } else {
	xc->cur_xl = NULL;
	xc->lang_num = 0;
    }
    add_ximclientlist(xc);
    if (IsPreeditNothing(xc) || xc->lang_num == 0) {
	xc->yes_no = NULL;
	return(xc);
    }
    save_c_c = c_c;
    save_cur_p = cur_p;
    save_cur_x = cur_x;
    cur_lang_sv = cur_lang;
    if (IsPreeditPosition(xc)) {
	if (!(xc->mask & (1 << ICArea))) {
	    xc->pe.area.x = xc->focus_area.x;
	    xc->pe.area.y = xc->focus_area.y;
	    xc->pe.area.width = xc->focus_area.width;
	    xc->pe.area.height = xc->focus_area.height;
	}
	update_spotlocation(xc, ic_req, pre_req);
    }
    cur_p = cur_x = xc;
    cur_p->cur_xl = cur_xl;
    cur_xl->cur_lc = cur_lc;
    cur_lang = cur_p->cur_xl->lang;
    c_c = cur_p->cur_xl->w_c;
    cur_rk = c_c->rk;
    cur_rk_table = cur_rk->rk_table;
    if (IsStatusArea(xc)
#ifdef	CALLBACKS
	|| IsStatusCallbacks(xc)
#endif	/* CALLBACKS */
	) {
#ifdef	nodef
	visual_status();
#endif	/* nodef */
	if (henkan_off_flag == 0) {
	    disp_mode();
	} else {
	    display_henkan_off_mode();
	}
    }

    XSelectInput(dpy, xc->w, StructureNotifyMask);
    if (c_c->use_server && !jl_isconnect(bun_data_)) {
	if (servername && *servername){
	    print_msg_getc(" I can not connect server(at %s)", servername,
			   NULL, NULL);
	} else {
	    print_msg_getc(" I can not connect server", NULL, NULL, NULL);
	}
    }
    c_c = save_c_c;
    cur_p = save_cur_p;
    cur_x = save_cur_x;
    cur_lang = cur_lang_sv;
    if (c_c) {
	cur_rk = c_c->rk;
	cur_rk_table = cur_rk->rk_table;
    }
    return(xc);
}

static char	*lang_title = "LANGUAGE";
int
lang_set(in, p_xl, p_lang)
int in;
XIMLangRec **p_xl;
XIMNestLangRec **p_lang;
{
    static int init;
    static int c, cnt;
    int i;
    static char *buf[MAX_LANGS];
    static WnnClientRec *c_c_sv = 0;
    static lang_step = 0;
    XIMNestLangRec *p;

/*
    if (henkan_off_flag == 1) {
	return(-1);
    }
*/
    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return (-1);
    }
    if (lang_step == 0) {
	if ((cur_p->lang_num == 0) || (cur_p->have_ch_lc == 0)) return(-1);
	c_c_sv = c_c;
	cnt = 0;
	for(i = 0 ; i < cur_p->lang_num ;i++){
	    for(p = cur_p->xl[i]->lc_list; p; p = p->next) {
		if (p->alias_name) 
		    buf[cnt] = p->alias_name;
		else
		    buf[cnt] = p->lc_name;
		if (cur_p->cur_xl == cur_p->xl[i] && cur_p->xl[i]->cur_lc == p)
		    init = cnt;
		cnt++;
	    }
	}
	if (cur_p->have_world) {
	    buf[cnt] = "wr_WR.ct";
	    if (cur_p->world_on) {
		init = cnt;
	    }
	    cnt++;
	}
	lang_step++;
    }
    if (lang_step == 1) {
	c = xw_select_one_element(buf, cnt, init, -1, lang_title, SENTAKU,
				  main_table[4], in);
	if(c == BUFFER_IN_CONT) {
	    return(BUFFER_IN_CONT);
	}
	if(c == -1 || c == -3) {
	    c_c_sv = 0;
	    lang_step = 0;
	    return(-1);
	}
	lang_step++;
    }
    if (lang_step == 2) {
	c_c_sv = 0;
	lang_step = 0;
	if (cur_p->have_world) {
	    if (!strcmp(cur_p->xl[0]->lc_list->lc_name, buf[c])) {
		*p_xl = cur_p->xl[0];
		*p_lang = cur_p->xl[0]->lc_list;
		cur_p->ch_lc_flg = 1;
		cur_p->world_on = 1;
		return(c);
	    } else {
		cur_p->world_on = 0;
	    }
	}
	for(i = 0 ; i < cur_p->lang_num ;i++){
	    for(p = cur_p->xl[i]->lc_list; p; p = p->next) {
		if ((p->alias_name && !strcmp(p->alias_name, buf[c])) ||
		    !strcmp(p->lc_name, buf[c])) {
		    *p_xl = cur_p->xl[i];
		    *p_lang = p;
		    cur_p->ch_lc_flg = 1;
		    return(c);
		}
	    }
	}
	return(-1);
    }
    c_c_sv = 0;
    lang_step = 0;
    return(-1);
}

static char	*lang_title_ct = "LANGUAGE_CT";
int
lang_set_ct(in, p_xl, p_lang)
int in;
XIMLangRec **p_xl;
XIMNestLangRec **p_lang;
{
    static int init;
    static int c;
    int i;
    static char *buf[MAX_LANGS];
    static WnnClientRec *c_c_sv = 0;
    static lang_step = 0;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return (-1);
    }
    if (lang_step == 0) {
	if ((cur_p->lang_num == 0) || (cur_p->world_on == 0)) return(-1);
	c_c_sv = c_c;
	for(i = 0 ; i < cur_p->lang_num ;i++){
	    buf[i] = cur_p->xl[i]->lang;
	    if (cur_p->cur_xl == cur_p->xl[i])
	      init = i;
	}
	lang_step++;
    }
    if (lang_step == 1) {
	c = xw_select_one_element(buf, cur_p->lang_num, init, -1, lang_title_ct,
				  SENTAKU,  main_table[4], in);
	if(c == BUFFER_IN_CONT) {
	    return(BUFFER_IN_CONT);
	}
	if(c == -1 || c == -3) {
	    c_c_sv = 0;
	    lang_step = 0;
	    return(-1);
	}
	lang_step++;
    }
    if (lang_step == 2) {
	c_c_sv = 0;
	lang_step = 0;
	for(i = 0 ; i < cur_p->lang_num ;i++){
	    if (!strcmp(cur_p->xl[i]->lang, buf[c])) {
		*p_xl = cur_p->xl[i];
		*p_lang = cur_p->xl[i]->lc_list;
		return(c);
	    }
	}
	return(-1);
    }
    c_c_sv = 0;
    lang_step = 0;
    return(-1);
}

void
change_lang(xl, p)
XIMLangRec *xl;
XIMNestLangRec *p;
{
    if (IsPreeditNothing(cur_p)) return;
    if (cur_p->cur_xl == xl) {
	if (cur_p->cur_xl->cur_lc == p) return;
	cur_p->cur_xl->cur_lc = p;
    } else {
	invisual_window1();
	invisual_status();
	cur_p->cur_xl = xl;
	cur_p->cur_xl->cur_lc = p;
	c_c = cur_p->cur_xl->w_c;
	cur_lang = cur_p->cur_xl->lang;
	cur_rk = c_c->rk;
	cur_rk_table = cur_rk->rk_table;
	visual_status();
	if (henkan_off_flag == 0) {
	disp_mode();
	} else {
	    display_henkan_off_mode();
	}
	if (!ifempty()) {
	    visual_window();
	}
	cur_input = NULL;
    }
}
