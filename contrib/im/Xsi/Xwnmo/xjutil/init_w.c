/*
 * $Id: init_w.c,v 1.6 1991/09/16 21:35:17 ohm Exp $
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
#include "xjutil.h"
#include "sxheader.h"
#include "config.h"
#include "xext.h"
#include "rk_fundecl.h"

#define	XOR(a,b)	((a&(~b)) | ((~a)&b))

#define MaskOfButton (ButtonPressMask | ButtonReleaseMask | EnterWindowMask | \
		      LeaveWindowMask | ExposureMask)

Display	*dpy = 0;
int screen;
Window root_window;
Xjutil	*xjutil;

char	xjutil_name[32] = {0};
char	xjutil_name_ok[32] = {0};
int	counter = 0;
Cursor	cursor1, cursor2, cursor3;

Window window_xim;
Atom atom_xim;

Atom	wm_id = 0;
Atom	wm_id1 = 0;

XGCValues	xgcv;
XSizeHints	hints;
XClassHint	class_hints;
XWMHints	wm_hints;
#ifndef	X11R3
XTextProperty	winName, iconName;
#else	/* X11R3 */
char	*winName, *iconName;
#endif	/* X11R3 */
Atom actual_type;
int actual_format;
unsigned long nitems, byte_after, leftover;

wchar_t dol_wchar_t;

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

    for (p = normal_env; p; p = p->next) {
	for (i = 0; p->env_name_str[i]; i++) {
	    if (s == p->env_name_str[i]) {
		cur_normal_env = p;
		return(0);
	    }
	}
    }
    return(0);
}

void
get_new_env(rev)
int rev;
{
    WnnEnv *new, *p, *prev = NULL;

    new = (WnnEnv *)Malloc(sizeof(WnnEnv));
    new->host_name = NULL;
    new->env = NULL;
    new->sticky = 0;
    new->envrc_name = NULL;
    new->env_name_str[0] = '\0';
    new->next = NULL;
    if (rev) {
	cur_reverse_env = new;
	if (reverse_env == NULL) {
	    reverse_env = new;
	    return;
	}
	p = reverse_env;
    } else {
	cur_normal_env = new;
	if (normal_env == NULL) {
	    normal_env = new;
	    return;
	}
	p = normal_env;
    }
    for (; p; prev = p, p = p->next);
    if (prev) prev->next = new;
}

static void
send_ok()
{
    XEvent event;
    Atom select_id;

    if (!(select_id = XInternAtom(dpy, xjutil_name_ok, True))) {
	select_id = XInternAtom(dpy, xjutil_name_ok, False);
    }
    XSetSelectionOwner(dpy, select_id, xjutil->jutil->w, 0L);
    event.type = ClientMessage;
    event.xclient.format = 32;
    event.xclient.window = xjutil->jutil->w;
    event.xclient.data.l[0] = DIC_START;
    event.xclient.data.l[1] = xjutil->jutil->w;
    event.xclient.data.l[2] = counter;
    XSendEvent(dpy, window_xim, False, NoEventMask, &event);
    XFlush(dpy);
}

void
return_error()
{
    XEvent event;
    Atom select_id;

    if (dpy == NULL) return;
    if (!(select_id = XInternAtom(dpy, xjutil_name_ok, True))) {
	select_id = XInternAtom(dpy, xjutil_name_ok, False);
    }
    XSetSelectionOwner(dpy, select_id, xjutil->jutil->w, 0L);
    event.type = ClientMessage;
    event.xclient.format = 32;
    event.xclient.window = xjutil->jutil->w;
    event.xclient.data.l[0] = DIC_START_ER;
    event.xclient.data.l[1] = xjutil->jutil->w;
    event.xclient.data.l[2] = counter;
    XSendEvent(dpy, window_xim, False, NoEventMask, &event);
    XFlush(dpy);
}

void
read_wm_id()
{
    if ((wm_id = XInternAtom(dpy, "WM_PROTOCOLS", True))) {
	wm_id1 = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
	XChangeProperty(dpy, xjutil->jutil->w, wm_id, XA_INTEGER, 32,
			PropModeReplace, (unsigned char *)&wm_id1,
			sizeof(Atom));
	XChangeProperty(dpy, xjutil->ichi->w, wm_id, XA_INTEGER, 32,
			PropModeReplace, (unsigned char *)&wm_id1,
			sizeof(Atom));
	XChangeProperty(dpy, xjutil->yes_no->w, wm_id, XA_INTEGER, 32,
			PropModeReplace, (unsigned char *)&wm_id1,
			sizeof(Atom));
    } else {
	wm_id = wm_id1 = 0;
    }
}

static char *
get_property()
{
    Xjutil_startRec *xjutil_start;
    unsigned char *data, *s_data;
    char *p, *s_p;
    char nlspath[64];
    int all_size = 0, uumkey_name_size,
	rkfile_name_size, cvtfun_name_size, cvtmeta_name_size,
	user_name_size, lang_size, lc_name_size, fn_size = 0;
    int i;
    unsigned char tmp_buf[256];

    if ((atom_xim = XInternAtom (dpy, xjutil_name, True)) == (Atom)NULL) {
	return(NULL);
    }
    window_xim = XGetSelectionOwner (dpy, atom_xim);
    XSelectInput(dpy, window_xim, StructureNotifyMask);
    xjutil_start = (Xjutil_startRec *)Malloc(sizeof (Xjutil_startRec));
    XGetWindowProperty(dpy, root_window, atom_xim, 0L, 1000000L,
    			False,XA_STRING, &actual_type, &actual_format,
			&nitems, &leftover, &data);

    bcopy(data, xjutil_start, sizeof(Xjutil_startRec));
    if (xjutil_start->fn_len > 0) {
	all_size = fn_size = xjutil_start->fn_len + 1;
    }
    all_size += uumkey_name_size = (strlen(xjutil_start->uumkey_name) + 1);
    all_size += rkfile_name_size = (strlen(xjutil_start->rkfile_name) + 1);
    all_size += cvtfun_name_size = (strlen(xjutil_start->cvtfun_name) + 1);
    all_size += cvtmeta_name_size = (strlen(xjutil_start->cvtmeta_name) + 1);
    all_size += user_name_size = (strlen(xjutil_start->user_name) + 1);
    all_size += lang_size = (strlen(xjutil_start->lang) + 1);
    all_size += lc_name_size = (strlen(xjutil_start->lc_name) + 1);
    if ((p = (char *)Malloc(all_size)) == NULL) {
	malloc_error("allocation of initial area");
	Free(xjutil_start);
	Free(data);
	return(NULL);
    }
    s_data = data;
    data += sizeof(Xjutil_startRec);
    for (i = 0; (int)i < (int)xjutil_start->max_env; i++) {
	get_new_env(0);
	tmp_buf[0] = '\0';
	strcpy((char *)tmp_buf, (char *)data);
	cur_normal_env->host_name = alloc_and_copy(tmp_buf);
	data += 256;
	tmp_buf[0] = '\0';
	strcpy((char *)tmp_buf, (char *)data);
	cur_normal_env->envrc_name = alloc_and_copy(tmp_buf);
	data += 256;
	tmp_buf[0] = '\0';
	strcpy((char *)tmp_buf, (char *)data);
	strcpy((char *)cur_normal_env->env_name_str, (char *)tmp_buf);
	data += 32;
    }
    for (i = 0; (int)i < (int)xjutil_start->max_env; i++) {
	get_new_env(1);
	tmp_buf[0] = '\0';
	strcpy((char *)tmp_buf, (char *)data);
	cur_reverse_env->host_name = alloc_and_copy(tmp_buf);
	data += 256;
	tmp_buf[0] = '\0';
	strcpy((char *)tmp_buf, (char *)data);
	cur_reverse_env->envrc_name = alloc_and_copy(tmp_buf);
	data += 256;
	tmp_buf[0] = '\0';
	strcpy((char *)tmp_buf, (char *)data);
	strcpy((char *)cur_reverse_env->env_name_str, (char *)tmp_buf);
	data += 32;
    }
    strncpy(p, (char*)data, fn_size);
    p[fn_size] = '\0';
    Free(s_data);
    s_p = p;

    uumkey_name = p += fn_size;
    strcpy(p, xjutil_start->uumkey_name);
    rkfile_name = p += uumkey_name_size;
    strcpy(p, xjutil_start->rkfile_name);
    cvt_fun_file = p += rkfile_name_size;
    strcpy(p, xjutil_start->cvtfun_name);
    cvt_meta_file = p += cvtfun_name_size;
    strcpy(p, xjutil_start->cvtmeta_name);
    username = p += cvtmeta_name_size;
    strcpy(p, xjutil_start->user_name);
    xjutil->lang = p += user_name_size;
    strcpy(p, xjutil_start->lang);
    xjutil->lc_name = p += lang_size;
    strcpy(p, xjutil_start->lc_name);

    xjutil->fg = xjutil_start->fore_ground;
    xjutil->bg = xjutil_start->back_ground;

    strcpy(nlspath, LIBDIR);
    strcat(nlspath, "/%L/%N");
    cd = msg_open("xjutil.msg", nlspath, xjutil->lang);
    Free(xjutil_start);
    return(s_p);
}

static void
clear_text(t)
JutilTextRec *t;
{
	t->vst = 0;
	t->note[0] = t->note[1] = 0;
	t->currentcol = 0;
	t->max_pos = 0;
	t->cursor_flag = 0;
	t->mark_flag = 0;
	t->u_line_flag = t->r_flag = t->b_flag = 0;
}

int
create_text(text)
register JutilTextRec	*text;
{
    int	border_width;
    register int i;

    if ((text->buf = (wchar *)Malloc(sizeof(wchar) * (maxchg * 2 + 1)))
	== NULL) {
	malloc_error("allocation of client's area");
	return(-1);
    }
    if ((text->att = (unsigned char *)
		Malloc(sizeof(unsigned char) * (maxchg * 2 + 1))) == NULL) {
	malloc_error("allocation of client's area");
	return(-1);
    }
    for (i = (maxchg * 2); i >= 0; i--) {
	*(text->buf + i) = 0;
	*(text->att + i) = '\0';
    }
    clear_text(text);

    border_width = 0;
    text->maxcolumns = maxchg * 2;
		
    text->wn[0] = XCreateSimpleWindow(dpy, text->wp, 0, 0,
				      FontWidth, FontHeight,
				      border_width, xjutil->fg, xjutil->bg);

    text->wn[1] = XCreateSimpleWindow(dpy, text->wp,
				      text->width - FontWidth, 0,
				      FontWidth, FontHeight,
				      border_width, xjutil->fg, xjutil->bg);

    text->wn[2] = XCreateSimpleWindow(dpy, text->wp,
				      text->width - FontWidth, 0,
				      FontWidth, FontHeight,
				      border_width, xjutil->fg, xjutil->bg);

    text->w = XCreateSimpleWindow(dpy, text->wp, 0, 0,
				  FontWidth * (text->maxcolumns + 1),
				  FontHeight,
				  border_width, xjutil->fg, xjutil->bg);

    text->vst = 0;
    text->note[0] = 0;
    text->note[1] = 0;

    XSelectInput(dpy, text->w, ExposureMask);
    XSelectInput(dpy, text->wn[0], ExposureMask);
    XSelectInput(dpy, text->wn[1], ExposureMask);
    return(0);
}

int
create_jutil()
{
    JutilRec *jutil = xjutil->jutil;
    unsigned int width, height, bwidth;
    int i;
    extern int jutil_mode_set();

    bwidth = 2;
    width = FontWidth;
    height = FontHeight;

    jutil->w = XCreateSimpleWindow(dpy, root_window, 0, 0,
				   FontWidth, FontHeight,
				   bwidth, xjutil->fg, xjutil->bg);
    XSelectInput(dpy, jutil->w, KeyPressMask | StructureNotifyMask);
    XDefineCursor(dpy, jutil->w, cursor3);

    jutil->title = create_box(jutil->w, 0, 0, FontWidth, FontHeight, 1,
			      xjutil->fg, xjutil->bg, NULL, NULL);
    jutil->rk_mode = create_box(jutil->w, 0, 0, FontWidth, FontHeight, 1,
				xjutil->fg, xjutil->bg, NULL, NULL);
    map_box(jutil->title);
    map_box(jutil->rk_mode);

    jutil->max_button = MAX_JU_BUTTON;
    for ( i = 0; i < MAX_JU_BUTTON; i++) {
	jutil->button[i] = create_box(jutil->w, 0, 0, FontWidth, FontHeight,
				      2, xjutil->fg, xjutil->bg, cursor2, 1);
	map_box(jutil->button[i]);
    }
    jutil->button[CANCEL_W]->string
	= (unsigned char *)msg_get(cd, 2, default_message[2], xjutil->lang);
    jutil->button[CANCEL_W]->sel_ret = -2;
    jutil->button[EXEC_W]->sel_ret = 0;
    jutil->button[CANCEL_W]->do_ret = jutil->button[EXEC_W]->do_ret = True;

    for (i = 0; i < JCLIENTS; i++) {
	if ((jutil->mes_text[i] = (JutilTextRec *)Malloc(sizeof(JutilTextRec)))
	    == NULL){
	    malloc_error("allocation of client's area");
	    return(-1);
	}
	jutil->mes_button[i] = create_box(jutil->w, 0, 0,
					  FontWidth, FontHeight, 0,
					  xjutil->fg, xjutil->bg, cursor2, 0);
	map_box(jutil->mes_button[i]);
	jutil->mes_mode_return[i] = i;
	jutil->mes_button[i]->sel_ret = 1;
	jutil->mes_button[i]->do_ret = True;
	jutil->mes_button[i]->cb = jutil_mode_set;
	jutil->mes_button[i]->cb_data = (int *)i;

	jutil->mes_text[i]->x = 0;
	jutil->mes_text[i]->y = 0;
	jutil->mes_text[i]->width = FontWidth * TEXT_WIDTH;
	jutil->mes_text[i]->height = FontHeight;
	jutil->mes_text[i]->max_columns = jutil->mes_text[i]->width/FontWidth;
	jutil->mes_text[i]->bp = 0;
	jutil->mes_text[i]->wp = XCreateSimpleWindow(dpy, jutil->w, 0, 0,
						     FontWidth * TEXT_WIDTH,
						     FontHeight, 0,
						     xjutil->fg, xjutil->bg);
	if (create_text(jutil->mes_text[i])) {
		return(-1);
	}
	XFlush(dpy);
    }
#ifndef	X11R3
    winName.encoding = XA_STRING;
    winName.format = 8;
    winName.value = (unsigned char *)"Jutil";
    winName.nitems = strlen((char*)winName.value);
    iconName.encoding = XA_STRING;
    iconName.format = 8;
    iconName.value = (unsigned char *)"Jutil";
    iconName.nitems = strlen((char*)winName.value);
#else	/* X11R3 */
    winName = "Jutil";
    iconName = "Jutil";
#endif	/* X11R3 */

    hints.flags = (USSize | USPosition | PMinSize | PMaxSize | PResizeInc
#ifndef	X11R3
		   | PBaseSize | PWinGravity
#endif	/* X11R3 */
		  );

    hints.x = 0;
    hints.y = 0;
    hints.width = hints.min_width = hints.max_width = width;
    hints.height = hints.min_height = hints.max_height = height;
    hints.width_inc = hints.height_inc = 0;
#ifndef	X11R3
    hints.base_width = width;
    hints.base_height = height;
    hints.win_gravity = CenterGravity;
#endif	/* X11R3 */
    class_hints.res_name = "jutil";
    class_hints.res_class = "Jutil";
    wm_hints.input = True;
    wm_hints.flags = InputHint;

#ifndef	X11R3
    XSetWMProperties(dpy, jutil->w, &winName, &iconName, NULL, 0,
		     &hints, &wm_hints, &class_hints);
#else	/* X11R3 */
    XSetStandardProperties(dpy, jutil->w, winName, iconName, None, NULL, 0,
			   &hints);
    XSetWMHints(dpy, jutil->w, &wm_hints);
    XSetClassHint(dpy, jutil->w, &class_hints);
#endif	/* X11R3 */
    return(0);
}

int
create_ichi() 
{
    JutilTextRec *text;
    Ichiran	*ichi;
    Keytable	*kt;
    unsigned int width, height, bwidth;
    int	i;
    extern int next_ichi(), back_ichi();
    XSetWindowAttributes attr;

    ichi = xjutil->ichi;


    if((ichi->text = (JutilTextRec *)Malloc(sizeof(JutilTextRec))) == NULL){
	malloc_error("allocation of the initial area");
	return(-1);
    }
    if((ichi->keytable = (Keytable *)Malloc(sizeof(Keytable))) == NULL){
	malloc_error("allocation of the initial area");
	return(-1);
    }
    text = ichi->text;
    kt = ichi->keytable;

    width = 1;
    height = 1;
    bwidth = 2;

    ichi->w = XCreateSimpleWindow(dpy, root_window, 0, 0, width, height,
				  bwidth, xjutil->fg, xjutil->bg);
    ichi->w1 = XCreateSimpleWindow(dpy, ichi->w, 0, FontWidth * 4,
				   width, height, 1, xjutil->fg, xjutil->bg);
    ichi->comment = create_box(ichi->w, 0, FontHeight * 4, FontWidth,
			       FontHeight, 1, xjutil->fg, xjutil->bg,
			       NULL, NULL);
    ichi->rk_mode = create_box(ichi->w, FontWidth,
			       (FontHeight * 4 + FontHeight/2 - IN_BORDER),
			       (FontWidth * MHL0 + IN_BORDER * 2),
			       (FontHeight + IN_BORDER * 2), 1,
			       xjutil->fg, xjutil->bg, NULL, NULL);
    ichi->title = create_box(ichi->w, 0, 0, FontWidth, FontHeight, 1,
			     xjutil->fg, xjutil->bg, NULL, NULL);
    ichi->subtitle = create_box(ichi->w, 0, 0, FontWidth, FontHeight, 0,
			     xjutil->fg, xjutil->bg, NULL, NULL);
    for (i = 0; i < MAX_ICHI_BUTTON; i++) {
	ichi->button[i] = create_box(ichi->w, 0, 0, FontWidth, FontHeight, 2,
				     xjutil->fg, xjutil->bg, cursor2, 1);
					
    }
    ichi->max_button = MAX_ICHI_BUTTON;
    ichi->button[CANCEL_W]->string
	= (unsigned char *)msg_get(cd, 2, default_message[2], xjutil->lang);
    ichi->button[NEXT_W]->string
	= (unsigned char *)msg_get(cd, 3, default_message[3], xjutil->lang);
    ichi->button[BACK_W]->string
	= (unsigned char *)msg_get(cd, 4, default_message[4], xjutil->lang);

    ichi->button[CANCEL_W]->do_ret = ichi->button[EXEC_W]->do_ret = True;
    ichi->button[CANCEL_W]->sel_ret = -2;
    ichi->button[EXEC_W]->sel_ret = 1;
    ichi->button[NEXT_W]->cb = next_ichi;
    ichi->button[BACK_W]->cb = back_ichi;

    map_box(ichi->button[CANCEL_W]);
    map_box(ichi->title);

    kt->w = XCreateSimpleWindow(dpy, root_window, 0, 0, FontWidth,
					    FontHeight, bwidth,
					    xjutil->fg, xjutil->bg);
    XDefineCursor(dpy, kt->w, cursor3);

    kt->max_button = MAX_KEY_BUTTON;
    for (i = 0; i < MAX_KEY_BUTTON; i++) {
	kt->button[i] = create_box(kt->w, 0, 0, FontWidth, FontHeight, 2,
				   xjutil->fg, xjutil->bg, cursor2, 1);
	kt->button[i]->sel_ret = i;
	kt->button[i]->do_ret = True;
    }

    XDefineCursor(dpy, ichi->w, cursor3);

    text->x = FontWidth * (MHL0 + 2);
    text->y = FontHeight * 4 + FontHeight/2;
    text->width = FontWidth * TEXT_WIDTH;
    text->height = FontHeight;
    text->max_columns = text->width/FontWidth;
    text->bp = 0;
    text->wp = XCreateSimpleWindow(dpy, ichi->w, text->x, text->y,
				   text->width, text->height, text->bp,
				   xjutil->fg, xjutil->bg);
    if (create_text(text)) {
	return(-1);
    }
    XSelectInput(dpy, ichi->w, KeyPressMask);
    attr.override_redirect = True;
    XChangeWindowAttributes(dpy, kt->w, CWOverrideRedirect, &attr);
    XFlush(dpy);

#ifndef	X11R3
    winName.encoding = XA_STRING;
    winName.format = 8;
    winName.value = (unsigned char *)"Ichiran";
    winName.nitems = strlen((char*)winName.value);
    iconName.encoding = XA_STRING;
    iconName.format = 8;
    iconName.value = (unsigned char *)"Ichiran";
    iconName.nitems = strlen((char*)winName.value);
#else	/* X11R3 */
    winName = "Ichiran";
    iconName = "Ichiran";
#endif	/* X11R3 */

    hints.flags = (USSize | USPosition | PMinSize | PMaxSize | PResizeInc
#ifndef	X11R3
		   | PBaseSize | PWinGravity
#endif	/* X11R3 */
		  );

    hints.x = 0;
    hints.y = 0;
    hints.width = hints.min_width = hints.max_width = width;
    hints.height = hints.min_height = hints.max_height = height;
    hints.width_inc = hints.height_inc = 0;
#ifndef	X11R3
    hints.base_width = width;
    hints.base_height = height;
    hints.win_gravity = CenterGravity;
#endif	/* X11R3 */
    class_hints.res_name = "ichiran";
    class_hints.res_class = "Ichiran";
    wm_hints.input = True;
    wm_hints.flags = InputHint;

#ifndef	X11R3
    XSetWMProperties(dpy, ichi->w, &winName, &iconName, NULL, 0,
		     &hints, &wm_hints, &class_hints);
#else	/* X11R3 */
    XSetStandardProperties(dpy, ichi->w, winName, iconName, None, NULL, 0,
			   &hints);
    XSetWMHints(dpy, ichi->w, &wm_hints);
    XSetClassHint(dpy, ichi->w, &class_hints);
#endif	/* X11R3 */
    return(0);
}

int
create_yes_no()
{
    YesOrNo *yes_no = xjutil->yes_no;
    unsigned int width, height, bwidth;

    width = 1;
    height = 1;
    bwidth = 2;

    yes_no->w = XCreateSimpleWindow(dpy, root_window, 0, 0, width, height,
				    bwidth, xjutil->fg, xjutil->bg);
    yes_no->title = create_box(yes_no->w, 0, 0, width, height, 0,
			       xjutil->fg, xjutil->bg, NULL, NULL);
    yes_no->button[0] = create_box(yes_no->w, 0, 0, width, height, 2,
			       xjutil->fg, xjutil->bg, cursor2, 1);
    yes_no->button[1] = create_box(yes_no->w, 0, 0, width, height, 2,
			       xjutil->fg, xjutil->bg, cursor2, 1);
    yes_no->button[0]->sel_ret = 1;
    yes_no->button[1]->sel_ret = 0;
    yes_no->button[0]->do_ret = yes_no->button[1]->do_ret = True;

    XDefineCursor(dpy, yes_no->w, cursor3);
    XSelectInput(dpy, yes_no->w, (StructureNotifyMask | KeyPressMask));
    map_box(yes_no->title);
    XFlush(dpy);
#ifndef	X11R3
    winName.encoding = XA_STRING;
    winName.format = 8;
    winName.value = (unsigned char *)"YesNo";
    winName.nitems = strlen((char*)winName.value);
    iconName.encoding = XA_STRING;
    iconName.format = 8;
    iconName.value = (unsigned char *)"YesNo";
    iconName.nitems = strlen((char*)winName.value);
#else	/* X11R3 */
    winName = "YesNo";
    iconName = "YesNo";
#endif	/* X11R3 */

    hints.flags = (USSize | USPosition | PMinSize | PMaxSize | PResizeInc
#ifndef	X11R3
		   | PBaseSize | PWinGravity
#endif	/* X11R3 */
		  );

    hints.x = 0;
    hints.y = 0;
    hints.width = hints.min_width = hints.max_width = width;
    hints.height = hints.min_height = hints.max_height = height;
    hints.width_inc = hints.height_inc = 0;
#ifndef	X11R3
    hints.base_width = width;
    hints.base_height = height;
    hints.win_gravity = CenterGravity;
#endif	/* X11R3 */
    class_hints.res_name = "yesNo";
    class_hints.res_class = "YesNo";
    wm_hints.input = True;
    wm_hints.flags = InputHint;

#ifndef	X11R3
    XSetWMProperties(dpy, yes_no->w, &winName, &iconName, NULL, 0,
		     &hints, &wm_hints, &class_hints);
#else	/* X11R3 */
    XSetStandardProperties(dpy, yes_no->w, winName, iconName, None, NULL, 0,
			   &hints);
    XSetWMHints(dpy, yes_no->w, &wm_hints);
    XSetClassHint(dpy, yes_no->w, &class_hints);
#endif	/* X11R3 */
    return(0);
}

extern int sleep(), shutdown(), close();

void
xw_end()
{
    if(dpy) {
	sleep(1);
	shutdown(dpy->fd, 2);
	close(dpy->fd);
    }
}

static int
xerror(d, ev)
Display	*d;
register XErrorEvent *ev;
{
    return(0);
}

static int
xioerror(d)
Display	d;
{
    extern int exit();

    perror("xjutil");
    disconnect_server();
    shutdown(dpy->fd, 2);
    close(dpy->fd);
    exit(2);
    return(0);
}


XjutilFSRec *
add_fontset_list(fs, act)
XFontSet fs;
int act;
{
    register XjutilFSRec *rec;
    XCharStruct *cs;
    extern XCharStruct *get_base_char();

    if ((rec = (XjutilFSRec *)Malloc(sizeof(XjutilFSRec))) == NULL) {
	return(NULL);
    }
    if ((cs = get_base_char(fs)) == NULL) {
	return(NULL);
    }
    rec->fs = fs;
    rec->act = act;
    rec->b_char = cs;
    rec->next = font_set_list;
    font_set_list = rec;
    return(rec);
}

XjutilFSRec *
get_fontset_list(act)
int act;
{
    register XjutilFSRec *p;

    for (p = font_set_list; p; p = p->next) {
	if (p->act == act) return(p);
    }
    return(NULL);
}

int
create_xjutil()
{
    register Ichiran *ichi;
    register YesOrNo *yes_no;
    register JutilRec *jutil;
    extern int xerror(), xioerror();
    extern char	*getenv();
    int ret;
    char *p;
    int size_of_xjutil;
    int size_of_jutil;
    int size_of_ichiran;
    int size_of_yes_no;
    int all_size;
    XLocale xlc;
    char **miss, *def;
    int miss_cnt;
    XFontSet fs;
    XjutilFuncDataBase *db;
    extern XjutilFSRec *add_fontset_list();
    extern char *get_property();

#ifdef	X_WCHAR
    dol_wchar_t = 0x20202024;
#else	/* X_WCHAR */
    mbtowc(&dol_wchar_t, "$", 1);
#endif	/* X_WCHAR */
    if((dpy = XOpenDisplay(display_name)) == NULL) {
	print_out1("I could not open Display : %s.", display_name);
	return(-1);
    }
    screen = DefaultScreen(dpy);
    root_window = RootWindow(dpy, screen);
    XSetErrorHandler(xerror);
    XSetIOErrorHandler(xioerror);

    if ((atom_xim = XInternAtom (dpy, xjutil_name, True)) == (Atom)NULL) {
	print_out("I could not find InputManager.");
	return(-1);
    }
    if ((window_xim = XGetSelectionOwner (dpy, atom_xim)) == 0) {
	print_out("I could not find InputManager.");
	return(-1);
    }
    XSelectInput(dpy, window_xim, StructureNotifyMask);
    strcpy(xjutil_name_ok, xjutil_name);
    strcat(xjutil_name_ok, "_OK");
    if (ret = XInternAtom(dpy, xjutil_name_ok, True)) {
	if (XGetSelectionOwner(dpy, ret)) {
	    print_out("I am already running.");
	    return(-1);
	}
    }
    size_of_xjutil = sizeof(Xjutil);
    size_of_jutil = sizeof(JutilRec);
    size_of_ichiran = sizeof(Ichiran);
    size_of_yes_no = sizeof(YesOrNo);
    all_size = size_of_xjutil + size_of_jutil + size_of_ichiran
	     + size_of_yes_no;
    if ((p = (char *)Malloc(all_size)) == NULL) {
	malloc_error("allocation of initial area");
	return(-1);
    }
    xjutil = (Xjutil *)p;
    p += size_of_xjutil;
    jutil = (JutilRec *)p;
    p += size_of_jutil;
    ichi = (Ichiran *)p;
    p += size_of_ichiran;
    yes_no = (YesOrNo *)p;

    xjutil->jutil = jutil;
    xjutil->ichi = ichi;
    xjutil->yes_no = yes_no;
	
    cursor1 = XCreateFontCursor(dpy, 52);
    cursor2 = XCreateFontCursor(dpy, 60);
    cursor3 = XCreateFontCursor(dpy, 24);

    p = get_property();

    setlocale(LC_ALL, xjutil->lc_name);
    xlc = _XFallBackConvert();
    xjutil->xlc = _XlcDupLocale(xlc);

    if (((fs = XCreateFontSet(dpy, p, &miss, &miss_cnt, &def)) == NULL)/* ||
	(miss_cnt > 0)*/) {
	return(-1);
    }
    cur_fs = add_fontset_list(fs, 0);

    xgcv.foreground = xjutil->fg;
    xgcv.background = xjutil->bg;
    xjutil->gc = XCreateGC(dpy, root_window,
				(GCForeground | GCBackground ),&xgcv);
    xgcv.foreground = xjutil->bg;
    xgcv.background = xjutil->fg;
    xjutil->reversegc = XCreateGC(dpy, root_window,
				(GCForeground | GCBackground),&xgcv);
    xgcv.foreground = xjutil->fg;
    xgcv.function = GXinvert;
    xgcv.plane_mask = XOR(xjutil->fg, xjutil->bg);
    xjutil->invertgc = XCreateGC(dpy, root_window,
			         (GCForeground | GCFunction | GCPlaneMask),
			         &xgcv);

    if (create_ichi() == -1) return(-1);
    if (create_jutil() == -1) return(-1);
    if (create_yes_no() == -1) return(-1);
    send_ok();
    read_wm_id();
    XFlush(dpy);
    for (db = function_db; db->lang != NULL; db++) {
	if (!strcmp(db->lang, xjutil->lang)) {
	    f_table = &(db->f_table);
	}
    }
    if (f_table == NULL) f_table = &default_func_table;
    return(0);
}
