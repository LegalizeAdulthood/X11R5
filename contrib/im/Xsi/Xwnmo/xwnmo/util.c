/*
 * $Id: util.c,v 1.5 1991/09/16 21:37:28 ohm Exp $
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
#include "sheader.h"
#include "proto.h"
#include "ext.h"

static int c_data_len = 0, use_langs_len = 0, cur_l_len = 0;
static int pre_font_len = 0, st_font_len = 0;
static char *c_data = NULL, *use_langs = NULL, *cur_l = NULL;
static char *pre_font = NULL, *st_font = NULL;

void
GetIM()
{
    ximGetIMReq req;
    ximGetIMReply reply;
    unsigned long mask;
    int i;
    XIMStyle tmp;

    if(EACH_REQUEST(&req, sz_ximGetIMReq) == -1) return;
    if (need_byteswap() == True) {
	mask = byteswap_l(req.mask);
    } else {
	mask = req.mask;
    }
    reply.state = 0;
    if (mask & (1 << IMQueryInputStyle)) {
	reply.num_styles = MAX_SUPPORT_STYLE;
    } else {
	reply.num_styles = 0;
    }
    if (mask & (1 << IMQueryLanguage)) {
	reply.nbytes = strlen(xim->supported_language);
    } else {
	reply.nbytes = 0;
    }
    if (need_byteswap() == True) {
	byteswap_GetIMReply(&reply);
    }
    if (_WriteToClient(&reply, sz_ximGetIMReply) == -1) return;
    if (reply.num_styles > 0) {
	for (i = 0; i < MAX_SUPPORT_STYLE; i++) {
	    if (need_byteswap() == True) {
		tmp = byteswap_l(xim->supported_style[i]);
	    } else {
		tmp = xim->supported_style[i];
	    }
	    if (_WriteToClient(&tmp, sizeof(XIMStyle) * MAX_SUPPORT_STYLE)
		== -1) {
		return;
	    }
	}
    }
    if (reply.nbytes > 0) {
	if (_WriteToClient(xim->supported_language, reply.nbytes) == -1) {
	    return;
	}
    }
    if (_Send_Flush() == -1) return;
    return;
}

Status
have_world(lc_list)
XIMNestLangList lc_list;
{
    XIMNestLangList p = lc_list;

    for (; p != NULL; p = p->next) {
	if (!strcmp(p->lc_name, "wr_WR.ct")) return(True);
    }
    return(False);
}

void
CreateIC()
{
    ximICValuesReq ic_req;
    ximICAttributesReq pre_req, st_req;
    ximCreateICReply reply;
#ifdef	CALLBACKS
    ximEventReply reply1;
#endif	/* CALLBACKS */
    int c_len;
    XIMNestLangList p;
    XIMNestLangList lc_list = NULL, cur_lc = NULL;
    XIMClientRec *xc = NULL;
    int error = 0;
    int i;
    short detail;

    c_len = get_rest_len();
    if (read_strings(&c_data, &c_data_len, c_len) == -1) return;

    if (_ReadFromClient(&ic_req, sz_ximICValuesReq) == -1) return;
    if (need_byteswap() == True) {
	byteswap_ICValuesReq(&ic_req);
    }
    if ((read_strings(&use_langs, &use_langs_len, ic_req.nbytes) == -1)
	|| (read_strings(&cur_l, &cur_l_len, ic_req.nbytes2) == -1))
	return;

    if (_ReadFromClient(&pre_req, sz_ximICAttributesReq) == -1) return;
    if (need_byteswap() == True) {
	byteswap_ICAttributesReq(&pre_req);
    }
    if (read_strings(&pre_font, &pre_font_len, pre_req.nbytes) == -1)
	return;

    if	(_ReadFromClient(&st_req, sz_ximICAttributesReq) == -1) return;
    if (need_byteswap() == True) {
	byteswap_ICAttributesReq(&st_req);
    }
    if (read_strings(&st_font, &st_font_len, st_req.nbytes) == -1)
	return;

    /*
     * Check resources.
     */
    error = -1;
    if ((ic_req.mask & (1 << ICInputStyle))) {
	for (i = 0; i < MAX_SUPPORT_STYLE; i++) {
	    if (xim->supported_style[i] == ic_req.input_style) error = 0;
	}
    }
    if (error != 0) {
	detail = BadStyle;
    } else {
	if (ic_req.input_style & (XIMPreeditArea | XIMPreeditPosition)) {
	    if (!(ic_req.mask & (1 << ICFontSet)) || !pre_font || !*pre_font) {
		error = -1;
		detail = BadFontSet;
	    }
	}
	if (ic_req.input_style & XIMStatusArea) {
	    if (!(ic_req.mask & (1 << (ICFontSet + StatusOffset)))
		|| !st_font || !*st_font) {
		error = -1;
		detail = BadFontSet;
	    }
	}
	if (ic_req.input_style & XIMPreeditPosition) {
	    if (!(ic_req.mask & (1 << ICSpotLocation))) {
		error = -1;
		detail = BadSpotLocation;
	    }
	}
    }

    if (error == 0) {
	if (!(ic_req.input_style & XIMPreeditNothing)) {
	    lc_list = get_langlist(use_langs);
	    if (lc_list == NULL || have_world(lc_list) == True) {
		if (use_langs_len <=
		    (int)(strlen(use_langs) + strlen(LANG_WR) + 2)) {
		    use_langs = (char *)Realloc(use_langs,
			(use_langs_len = (strlen(use_langs) +
					  strlen(LANG_WR) + 2)));
		}
		if (lc_list == NULL) {
		    strcpy(use_langs, LANG_WR);
		} else {
		    strcat(use_langs, ";");
		    strcat(use_langs, LANG_WR);
		}
		if (lc_list) free_langlist(lc_list);
		lc_list = get_langlist(use_langs);
	    }
	    if (cur_l && *cur_l) {
		for (p = lc_list; p != NULL; p = p->next) {
		    if (!strcmp(p->lc_name, cur_l) ||
			(p->alias_name && !strcmp(p->alias_name, cur_l))) break;
		}
		if (p == NULL) {
		    if (cur_lc = get_langlist(cur_l)) {
			cur_lc->next = lc_list;
			if (use_langs_len <=
			    (int)(strlen(use_langs) + strlen(cur_l) + 2)) {
			    use_langs = (char *)Realloc(use_langs,
				(use_langs_len = (strlen(use_langs) +
						  strlen(cur_l) + 2)));
			}
			if (lc_list == NULL) {
			    strcpy(use_langs, cur_l);
			} else {
			    strcat(use_langs, ";");
			    strcat(use_langs, cur_l);
			}
		    } else {
			cur_lc = lc_list;
		    }
		} else {
		    cur_lc = p;
		}
	    } else {
		cur_lc = lc_list;
	    }
	}
	xc = create_client(&ic_req, &pre_req, &st_req, lc_list, cur_lc,
			   use_langs, pre_font, st_font, c_data, &detail);
	free_langlist(lc_list);
    }

    if (error != 0 || xc == NULL) {
	reply.state = -1;
	reply.detail = detail;
	reply.xic = 0;
    } else {
	reply.state = 0;
	reply.xic = xim->client_count++;
	xc->number = reply.xic;
    }
#ifdef	CALLBACKS
    reply1.state = 0;
    reply1.detail = XIM_IC;
    if (need_byteswap() == True) {
	byteswap_EventReply(&reply1);
	byteswap_CreateICReply(&reply);
    }
    if ((_WriteToClient(&reply1, sz_ximEventReply) == -1) ||
        (_WriteToClient(&reply, sz_ximCreateICReply) == -1) ||
#else	/* CALLBACKS */
    if (need_byteswap() == True) {
	byteswap_CreateICReply(&reply);
    }
    if ((_WriteToClient(&reply, sz_ximCreateICReply) == -1) ||
#endif	/* CALLBACKS */
	(_Send_Flush() == -1)) {
	for (i = 0; i < xc->lang_num; i++) {
	    Free((char *)xc->xl[i]);
	}
	Free((char *)xc);
	return;
    }
    return;
}

void
GetIC()
{
    ximGetICReq req;
    ximGetICReply reply;
    ximICValuesReq reply1;
    ximICAttributesReq reply2;
    register XIMClientRec *xc;
    unsigned long mask = 0L;
    XIMAttributes *pe, *st;

    if(EACH_REQUEST(&req, sz_ximGetICReq) == -1) return;
    if (need_byteswap() == True) {
	req.mask = byteswap_l(req.mask);
	req.xic = byteswap_l(req.xic);
    }
    mask = req.mask;

    for (xc = ximclient_list; xc != NULL; xc = xc->next) {
	if (xc->number == req.xic) {
	    break;
	}
    }
    if (xc == NULL) {
	reply.state = -1;
	reply.detail = BadIC;
	if (need_byteswap() == True) {
	    byteswap_GetICReply(&reply);
	}
	if (_WriteToClient(&reply, sz_ximGetICReply) == -1) return;
	return;
    } else {
	reply.state = 0;
    }
    if (need_byteswap() == True) {
	byteswap_GetICReply(&reply);
    }
    if (_WriteToClient(&reply, sz_ximGetICReply) == -1) return;
#ifdef	CALLBACKS
/*
    if (IsPreeditNothing(xc)) {
	xc = xim->ximclient;
    }
*/
#endif	/* CALLBACKS */

    reply1.mask = mask;

    if (mask & (1 << ICInputStyle)) {
	reply1.input_style = xc->input_style;
    }
    if (mask & (1 << ICClientWindow)) {
	reply1.c_window = xc->w;
    }
    if (mask & (1 << ICFocusWindow)) {
	reply1.focus_window = xc->focus_window;
    }
    if (mask & (1 << ICFilterEvents)) {
	reply1.filter_events = xc->filter_events;
    }
    if (mask & (1 << ICUsingLanguage)) {
	if (IsPreeditNothing(xc)) {
	    reply1.nbytes = strlen(xim->ximclient->using_language);
	} else if (xc->using_language) {
	    reply1.nbytes = strlen(xc->using_language);
	} else {
	    reply1.nbytes = 0;
	}
    } else {
	reply1.nbytes = 0;
    }
    if (mask & (1 << ICCurrentLanguage)) {
	if (IsPreeditNothing(xc)) {
	    if (xim->ximclient->world_on) {
		reply1.nbytes2 = strlen("wr_WR.ct");
	    } else {
		reply1.nbytes2 =
			 strlen(xim->ximclient->cur_xl->cur_lc->lc_name);
	    }
	} else if (xc->world_on) {
	    reply1.nbytes2 = strlen("wr_WR.ct");
	} else if (xc->cur_xl->cur_lc->lc_name) {
	    reply1.nbytes2 = strlen(xc->cur_xl->cur_lc->lc_name);
	} else {
	    reply1.nbytes2 = 0;
	}
/*
    if ((mask & (1 << ICCurrentLanguage)) && !IsPreeditNothing(xc)
	&& xc->cur_xl->cur_lc->lc_name) {
	reply1.nbytes2 = strlen(xc->cur_xl->cur_lc->lc_name);
*/
    } else {
	reply1.nbytes2 = 0;
    }
    if (need_byteswap() == True) {
	byteswap_ICValuesReq(&reply1);
    }
    if (_WriteToClient(&reply1, sz_ximICValuesReq) == -1) return;
    if (reply1.nbytes > 0) {
	if (IsPreeditNothing(xc)) {
	    if (_WriteToClient(xim->ximclient->using_language, reply1.nbytes)
		== -1) return;
	} else {
	    if (_WriteToClient(xc->using_language, reply1.nbytes) == -1) return;
	}
    }
    if (reply1.nbytes2 > 0) {
	if (IsPreeditNothing(xc)) {
	    if (xim->ximclient->world_on) {
		if (_WriteToClient("wr_WR.ct", reply1.nbytes2) == -1)
		    return;
	    } else {
		if (_WriteToClient(xim->ximclient->cur_xl->cur_lc->lc_name,
				   reply1.nbytes2) == -1) return;
	    }
	} else {
	    if (xc->world_on) {
		if (_WriteToClient("wr_WR.ct", reply1.nbytes2) == -1)
		    return;
	    } else {
		if (_WriteToClient(xc->cur_xl->cur_lc->lc_name, reply1.nbytes2)
		    == -1)
		    return;
	    }
	}
    }
    
    pe = &(xc->pe);
    if (mask & (1 << ICArea)) {
	reply2.area_x = pe->area.x;
	reply2.area_y = pe->area.y;
	reply2.area_width = pe->area.width;
	reply2.area_height = pe->area.height;
    }
    if (mask & (1 << ICAreaNeeded)) {
	reply2.areaneeded_width = pe->area_needed.width;
	reply2.areaneeded_height = pe->area_needed.height;
    }
    if (IsPreeditPosition(xc)) {
	if (mask & (1 << ICSpotLocation)) {
	    reply2.spot_x = xc->point.x;
	    reply2.spot_y = xc->point.y + FontAscent(xc->cur_xl);
	}
    }
    if (mask & (1 << ICColormap)) {
	reply2.colormap = pe->colormap;
    }
    if (mask & (1 << ICStdColormap)) {
	reply2.std_colormap = pe->std_colormap;
    }
    if (mask & (1 << ICForeground)) {
	reply2.foreground = pe->fg;
    }
    if (mask & (1 << ICBackground)) {
	reply2.background = pe->bg;
    }
    if (mask & (1 << ICBackgroundPixmap)) {
	reply2.pixmap = pe->bg_pixmap;
    }
    if (mask & (1 << ICLineSpace)) {
	reply2.line_space = pe->line_space;
    }
    if (mask & (1 << ICCursor)) {
	reply2.cursor = pe->cursor;
    }
    reply2.nfonts = 0;
    reply2.nbytes = 0;
    if (need_byteswap() == True) {
	byteswap_ICAttributesReq(&reply2);
    }
    if (_WriteToClient(&reply2, sz_ximICAttributesReq) == -1) return;

    st = &(xc->st);
    if (mask & (1 << (ICArea + StatusOffset))) {
	reply2.area_x = st->area.x;
	reply2.area_y = st->area.y;
	reply2.area_width = st->area.width;
	reply2.area_height = st->area.height;
    }
    if (mask & (1 << (ICAreaNeeded + StatusOffset))) {
	reply2.areaneeded_width = st->area_needed.width;
	reply2.areaneeded_height = st->area_needed.height;
    }
    if (mask & (1 << (ICColormap + StatusOffset))) {
	reply2.colormap = st->colormap;
    }
    if (mask & (1 << (ICStdColormap + StatusOffset))) {
	reply2.std_colormap = st->std_colormap;
    }
    if (mask & (1 << (ICForeground + StatusOffset))) {
	reply2.foreground = st->fg;
    }
    if (mask & (1 << (ICBackground + StatusOffset))) {
	reply2.background = st->bg;
    }
    if (mask & (1 << (ICBackgroundPixmap + StatusOffset))) {
	reply2.pixmap = st->bg_pixmap;
    }
    if (mask & (1 << (ICLineSpace + StatusOffset))) {
	reply2.line_space = st->line_space;
    }
    if (mask & (1 << (ICCursor + StatusOffset))) {
	reply2.cursor = st->cursor;
    }
    reply2.nfonts = 0;
    reply2.nbytes = 0;
    if (need_byteswap() == True) {
	byteswap_ICAttributesReq(&reply2);
    }
    if (_WriteToClient(&reply2, sz_ximICAttributesReq) == -1) return;
    if (_Send_Flush() == -1) return;
}

void
SetICFocus()
{
    ximICFocusReq req;
    ximEventReply reply;
    register XIMClientRec *xc;
    register XIMInputRec *xi;
    XIMClientRec *save_cur_p, *save_cur_x;
    WnnClientRec *save_c_c;

    if(EACH_REQUEST(&req, sz_ximICFocusReq) == -1) return;
    if (need_byteswap() == True) {
	req.xic = byteswap_l(req.xic);
    }

    for (xc = ximclient_list; xc != NULL; xc = xc->next) {
	if (xc->number == req.xic) break;
    }
    if (xc == NULL) {
	reply.state = -1;
	reply.detail = BadIC;
    } else {
	for (xi = input_list; xi != NULL; xi = xi->next){
	    if(xi->w == xc->focus_window) break;
	}
	if (xi == NULL) {
	    xi = (XIMInputRec *)Malloc(sizeof(XIMInputRec));
	    xi->w = xc->focus_window;
	    xi->pclient = xc;
	    xi->next = NULL;
	    add_inputlist(xi);
	}
	save_cur_p = cur_p;
	save_cur_x = cur_x;
	save_c_c = c_c;
	cur_x = xc;
	if (IsPreeditNothing(xc)) {
	    cur_p = xim->ximclient;
	} else {
	    cur_p = cur_x;
	}
	c_c = cur_p->cur_xl->w_c;
	cur_rk = c_c->rk;
	cur_rk_table = cur_rk->rk_table;
#ifdef	CALLBACKS
	if (IsPreeditPosition(xc) || IsPreeditArea(xc) ||
	    IsStatusCallbacks(xc)) {
#else	/* CALLBACKS */
	if (IsPreeditPosition(xc) || IsPreeditArea(xc)) {
#endif	/* CALLBACKS */
	    reset_preedit(xc);
	}
#ifdef	CALLBACKS
	if (IsStatusArea(xc) || IsStatusCallbacks(xc)) {
#else	/* CALLBACKS */
	if (IsStatusArea(xc)) {
#endif	/* CALLBACKS */
	    visual_status();
	}
	cur_p = save_cur_p;
	cur_x = save_cur_x;
	c_c = save_c_c;
	if (c_c) {
	    cur_rk = c_c->rk;
	    cur_rk_table = cur_rk->rk_table;
	}
	reply.state = 0;
	reply.detail = 0;
    }
    if (need_byteswap() == True) {
	byteswap_EventReply(&reply);
    }
    if ((_WriteToClient(&reply, sz_ximEventReply) == -1)
	|| (_Send_Flush() == -1)) return;
    return;
}

void
UnsetICFocus()
{
    ximICFocusReq req;
    ximEventReply reply;
    register XIMClientRec *xc;
    register XIMInputRec *xi;
    XIMClientRec *save_cur_p, *save_cur_x;
    WnnClientRec *save_c_c;

    if(EACH_REQUEST(&req, sz_ximICFocusReq) == -1) return;
    if (need_byteswap() == True) {
	req.xic = byteswap_l(req.xic);
    }

    for (xc = ximclient_list ; xc != NULL; xc = xc->next) {
        if (xc->number == req.xic) break;
    }
    if (xc == NULL) {
	reply.state = -1;
	reply.detail = BadIC;
    } else {
	for (xi = input_list; xi != NULL; xi = xi->next){
	    if(xi->w == xc->focus_window) break;
	}
	if (xi == NULL) {
	    reply.state = -1;
	    reply.detail = BadFocusWindow;
	} else {
	    remove_inputlist(xi);
	    if (cur_input == xi) cur_input = 0;
	    Free((char *)xi);
	    save_cur_p = cur_p;
	    save_cur_x = cur_x;
	    save_c_c = c_c;
	    cur_x = xc;
	    if (IsPreeditNothing(xc)) {
		cur_p = xim->ximclient;
	    } else {
		cur_p = cur_x;
	    }
	    c_c = cur_p->cur_xl->w_c;
	    cur_rk = c_c->rk;
	    cur_rk_table = cur_rk->rk_table;
#ifdef	CALLBACKS
	    if (IsPreeditPosition(xc) || IsPreeditArea(xc) ||
		IsPreeditCallbacks(xc)) {
#else	/* CALLBACKS */
	    if (IsPreeditPosition(xc) || IsPreeditArea(xc)) {
#endif	/* CALLBACKS */
		invisual_window();
	    }
#ifdef	CALLBACKS
	if (IsStatusArea(xc) || IsStatusCallbacks(xc)) {
#else	/* CALLBACKS */
	if (IsStatusArea(xc)) {
#endif	/* CALLBACKS */
		invisual_status();
	    }
	    cur_p = save_cur_p;
	    cur_x = save_cur_x;
	    c_c = save_c_c;
	    if (c_c) {
		cur_rk = c_c->rk;
		cur_rk_table = cur_rk->rk_table;
	    }
	    reply.state = 0;
	    reply.detail = 0;
	}
    }
    if (need_byteswap() == True) {
	byteswap_EventReply(&reply);
    }
    if ((_WriteToClient(&reply, sz_ximEventReply) == -1)
	|| (_Send_Flush() == -1)) return;
    return;
}

void
ChangeIC()
{
    register XIMClientRec *xc;
    XIMNestLangRec *p = NULL;
    short detail;
    ximChangeICReq req;
    ximICValuesReq ic_req;
    ximICAttributesReq pre_req, st_req;
    ximEventReply reply;

    if(EACH_REQUEST(&req, sz_ximChangeICReq) == -1) return;
    if (need_byteswap() == True) {
	req.xic = byteswap_l(req.xic);
    }

    if (_ReadFromClient(&ic_req, sz_ximICValuesReq) == -1)
    if (need_byteswap() == True) {
	byteswap_ICValuesReq(&ic_req);
    }
    if ((read_strings(&use_langs, &use_langs_len, ic_req.nbytes) == -1)
       || (read_strings(&cur_l, &cur_l_len, ic_req.nbytes2) == -1))
	return;

    if (_ReadFromClient(&pre_req, sz_ximICAttributesReq) == -1)
    if (need_byteswap() == True) {
	byteswap_ICAttributesReq(&pre_req);
    }
    if (read_strings(&pre_font, &pre_font_len, pre_req.nbytes) == -1)
	return;

    if (_ReadFromClient(&st_req, sz_ximICAttributesReq) == -1)
    if (need_byteswap() == True) {
	byteswap_ICAttributesReq(&st_req);
    }
    if (read_strings(&st_font, &st_font_len, st_req.nbytes) == -1)
	return;

    for (xc = ximclient_list; xc != NULL; xc = xc->next) {
	if(xc->number == req.xic) break;
    }
    if (xc == NULL) {
	reply.state = -1;
	reply.detail = BadIC;
    } else {
	if (cur_l && *cur_l) {
	    p = get_langlist(cur_l);
	}
	if (change_client(xc, &ic_req, &pre_req, &st_req, p,
			  pre_font, st_font, &detail) == -1) {
	    reply.state = -1;
	    reply.detail = detail;
	} else {
	    reply.state = 0;
	    reply.detail = 0;
	}
    }
    if (need_byteswap() == True) {
	byteswap_EventReply(&reply);
    }
    if ((_WriteToClient(&reply, sz_ximEventReply)  == -1)
	|| (_Send_Flush() == -1)) return;
    return;
}

void
DestroyIC()
{
    ximDestroyICReq req;
    ximEventReply reply;
    register XIMClientRec *xc;

    if(EACH_REQUEST(&req, sz_ximDestroyICReq) == -1) return;
    if (need_byteswap() == True) {
	req.xic = byteswap_l(req.xic);
    }

    for (xc = ximclient_list; xc != NULL; xc = xc->next) {
	if(xc->number == req.xic) break;
    }
    if (xc == NULL) {
	reply.state = -1;
	reply.detail = BadIC;
    } else {
	reply.state = 0;
	reply.detail = 0;
    }
    if (need_byteswap() == True) {
	byteswap_EventReply(&reply);
    }
    if ((_WriteToClient(&reply, sz_ximEventReply) == -1)
	|| (_Send_Flush() == -1)) return;
    if (xc) {
	(void)destroy_client(xc);
    }
    return;
}

void
destroy_for_sock(fd)
int fd;
{
    register XIMClientRec *xc;

    for (xc = ximclient_list; xc != NULL; xc = xc->next) {
	if (xc->fd == fd) {
	    (void)destroy_client(xc);
	    break;
	}
    }
}

void
destroy_client(xc)
register XIMClientRec *xc;
{
    register int i;
    register XIMInputRec *xi;
    register XIMLangRec *xl;

#ifdef	CALLBACKS
    if ((IsPreeditPosition(xc) || IsPreeditArea(xc) || IsPreeditCallbacks(xc))
	&& (cur_x == xc)) {
#else	/* CALLBACKS */
    if ((IsPreeditPosition(xc) || IsPreeditArea(xc)) && (cur_x == xc)) {
#endif	/* CALLBACKS */
	invisual_window();
	cur_input = NULL;
	cur_x = NULL;
	cur_p = NULL;
	c_c = NULL;
    }
    for(xi = input_list; xi != NULL; xi = xi->next) {
	if (xi->pclient == xc) {
	    remove_inputlist(xi);
	    if (cur_input == xi) cur_input = NULL;
	}
    }
    for(i = 0; i < xc->lang_num; i++) {
	xl = xc->xl[i];
	remove_locale_to_xl(xl, NULL);
	if (xl->lang) Free((char *)xl->lang);
	if (xl->lc_name) Free((char *)xl->lc_name);
	if (xl->xlc) Free(xl->xlc);
	if (!(IsPreeditNothing(xc))) {
	    if (IsPreeditArea(xc)) {
		XDestroyWindow(dpy, xl->wp[0]);
	    } else if (IsPreeditPosition(xc)) {
		XDestroyWindow(dpy, xl->wp[0]);
		XDestroyWindow(dpy, xl->wp[1]);
		XDestroyWindow(dpy, xl->wp[2]);
		XDestroyWindow(dpy, xl->wn[0]);
		XDestroyWindow(dpy, xl->wn[1]);
		XDestroyWindow(dpy, xl->wn[2]);
	    }
	    if (xl->pe_fs) XFreeFontSet(dpy, xl->pe_fs);
	    if (xl->pe_b_char) Free((char *)xl->pe_b_char);
	    del_client(xl->w_c);
	    Free((char *)xl->buf);
	    if (xc->pe.gc) Free((char *)xc->pe.gc);
	    if (xc->pe.reversegc) Free((char *)xc->pe.reversegc);
	}
	if (!(IsStatusNothing(xc))) {
	    if (IsStatusArea(xc)){
		XDestroyWindow(dpy, xl->ws);
	    }
	    if (xl->st_fs) XFreeFontSet(dpy, xl->st_fs);
	    if (xl->st_b_char) Free((char *)xl->st_b_char);
	    Free((char *)xl->buf0);
	}
	Free((char *)xl);
    }
    remove_ximclientlist(xc);
    if (xc->yes_no) {
	remove_box(xc->yes_no->title);
	remove_box(xc->yes_no->button[0]);
	remove_box(xc->yes_no->button[1]);
	Free((char *)xc->yes_no);		
    }
    if (xc->using_language) Free((char *)xc->using_language);
    if (xc->user_name) Free((char *)xc->user_name);
    if (xc->pe.fontset) Free((char *)xc->pe.fontset);
    if (xc->st.fontset) Free((char *)xc->st.fontset);
    if (xc->xl) Free((char *)xc->xl);
    Free((char *)xc);		
    return;
}

void
ResetIC()
{
    ximResetICReq req;
    ximEventReply reply;
    register XIMClientRec *xc;
    WnnClientRec *save_c_c;
    XIMClientRec *save_cur_p, *save_cur_x;
    int ret;

    if(EACH_REQUEST(&req, sz_ximResetICReq) == -1) return;
    if (need_byteswap() == True) {
	req.xic = byteswap_l(req.xic);
    }

    for (xc = ximclient_list; xc != NULL; xc = xc->next) {
	if (xc->number == req.xic) break;
    }
    if (xc == NULL) {
	reply.state = -1;
	reply.detail = BadIC;
    } else {
	save_c_c = c_c;
	save_cur_p = cur_p;
	save_cur_x = cur_x;
	cur_x = xc;
	if (IsPreeditNothing(xc)) {
	    cur_p = xim->ximclient;
	} else {
	    cur_p = cur_x;
	}
	c_c = cur_p->cur_xl->w_c;
	ret = reset_c_b();
	JWMline_clear(0);
	c_c = save_c_c;
	cur_p = save_cur_p;
	cur_x = save_cur_x;
	if (ret == -1) {
	    reply.state = -1;
	    reply.detail = BadIC;
	} else {
	    reply.state = 0;
	    reply.detail = 0;
	}
    }
    if (need_byteswap() == True) {
	byteswap_EventReply(&reply);
    }
    if ((_WriteToClient(&reply, sz_ximEventReply) == -1)
	|| (_Send_Flush() == -1)) return;
    return;
}

void
free_langlist(p)
XIMNestLangList p;
{
    for (; p != NULL; p = p->next) {
	if (p->lc_name) Free((char *)p->lc_name);
	if (p->alias_name) Free((char *)p->alias_name);
	if (p->lang) Free((char *)p->lang);
	Free((char *)p);
    }
}

XIMNestLangList
get_langlist(lang)
char *lang;
{
    register char *d, *save_d, *p, *s;
    register XIMNestLangList pp = NULL, prev = NULL, ls;
    register int end = 0, alias;
    register int i;

    if (lang == NULL || *lang == '\0') return(NULL);
    save_d = d = alloc_and_copy(lang);
    for (end = 0; end == 0 && *d ;) {
	alias = 0;
	ls = (XIMNestLangList)Calloc(1, sizeof(XIMNestLangRec));
	if (prev  == NULL) {
	    pp = ls;
	} else {
	    prev->next = ls;
	}
	prev = ls;
        ls->next = NULL;
	for (p = d; ; p++) {
	    if (*p == '\0') {
		end = 1;
		break;
	    } else if (*p == ',') {
		alias = 1;
		break;
	    } else if (*p == ';') {
		break;
	    }
	}
	if (*p != '\0') *p = '\0';
	ls->lc_name = alloc_and_copy(d);
	if (alias == 1) {
	    d = ++p;
	    for (p = d; ; p++) {
		if (*p == '\0') {
		    end = 1;
		    break;
		} else if (*p == ';') {
		    break;
		}
	    }
	    if (*p != '\0') *p = '\0';
	    ls->alias_name = alloc_and_copy(d);
	} else {
	    ls->alias_name = NULL;
	}
	ls->lang = (char *)Malloc(strlen(ls->lc_name) + 1);
	for (i = 0, s = ls->lc_name; (*s != '@') && (*s != '.') && *s;
	    i++, s++) {
	    ls->lang[i] = *s;
	}
	ls->lang[i] = '\0';
	d = ++p;
    }
    Free(save_d);
    return((XIMNestLangList)pp);
}

#ifdef SPOT
void
ChangeSpot()
{
    register XIMClientRec *xc;
    ximChangeSpotReq req;
    ximEventReply reply;

    if(EACH_REQUEST(&req, sz_ximChangeSpotReq) == -1) return;
    if (need_byteswap() == True) {
	req.xic = byteswap_l(req.xic);
	req.spot_x = byteswap_s(req.spot_x);
	req.spot_y = byteswap_s(req.spot_y);
    }

    for (xc = ximclient_list; xc != NULL; xc = xc->next) {
	if(xc->number == req.xic) break;
    }
    if (xc == NULL) {
	reply.state = -1;
	reply.detail = BadIC;
    } else {
	if (change_spotlocation(xc, req.spot_x, req.spot_y) == -1) {
	    reply.state = -1;
	    reply.detail = 0;
	} else {
	    reply.state = 0;
	    reply.detail = 0;
	}
    }
    if (need_byteswap() == True) {
	byteswap_EventReply(&reply);
    }
    if ((_WriteToClient(&reply, sz_ximEventReply)  == -1)
	|| (_Send_Flush() == -1)) return;
    return;
}
#endif	/* SPOT */
