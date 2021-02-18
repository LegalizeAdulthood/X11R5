/*
 * $Id: xim.h,v 1.3 1991/09/16 21:37:39 ohm Exp $
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
/*********

	xwnmo.h

*********/

#ifndef	XJUTIL
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include "Xi18nint.h"
#endif	/* !XJUTIL */
#include "client.h"
#ifndef	XJUTIL
#include "msg.h"

#define	MaskNeeded	KeyPressMask

typedef struct _XIMNestLangRec *XIMNestLangList;

typedef struct _XIMNestLangRec {
    char	*lc_name;
    char	*alias_name;
    char	*lang;
    struct _XIMNestLangRec	*next;
} XIMNestLangRec;
#endif	/* !XJUTIL */

typedef struct _BoxRec {
    Window	window;
    int		x, y;
    int		width, height;
    int		border_width;
    unsigned long	fg, bg;
    unsigned char	*string;
    GC		invertgc;
    int		(*cb)();
    int		*cb_data;
    int		(*redraw_cb)();
    int		*redraw_cb_data;
    int		do_ret;
    int		sel_ret;
    char	reverse;
    char	in;
    char	map;
    char	freeze;
    struct _BoxRec	*next;
} BoxRec;

typedef struct _YesOrNo {
    Window	w;
    BoxRec	*title;
    BoxRec	*button[2];
    char	map;
    char	exp;
    int		x, y;
    int		mode;
} YesOrNo;

#ifndef	XJUTIL
typedef	struct _XIMAttributes {
    XRectangle	area;
    XRectangle	area_needed;
    Colormap	colormap;
    Atom	std_colormap;
    unsigned long	fg;
    unsigned long	bg;
    Pixmap	bg_pixmap;
    char	*fontset;
    int		line_space;
    Cursor	cursor;
    GC		gc;
    GC		reversegc;
    GC		invertgc;
} XIMAttributes;

typedef	struct	_XIMLangRec {
    XIMNestLangList	lc_list;	/* List of locale */
    XIMNestLangRec	*cur_lc;
    char	*lc_name;		/*  Locale name */
    char	*lang;			/*  Language name */
    struct _LangUumrcRec *lang_uumrc;
    XFontSet	pe_fs;
    XFontSet	st_fs;
    XCharStruct	*pe_b_char;
    XCharStruct	*st_b_char;
    XLocale	xlc;
    Window	wp[3];			/*  Parent windows */
    Window    	w[3];			/*  child windows */
    Window	wn[3];			/*   */
    Window	ws;			/*  Status window */
    short	linefeed[3];		/*  */
    short	vst;			/*  */
    unsigned char	max_l1;
    unsigned char	max_l2;
    unsigned char	max_l3;
    wchar	*buf;			/* Buffer of Preedit */
    wchar	*buf0;			/* Buffer of Status */
    unsigned char	*att;		/* Attributes of buf */
    unsigned char	*att0;		/* Attributes of buf0 */
    short	currentcol;		/* Current cursor position of Preedit */
    short	currentcol0;		/* Current cursor position of Status */
    short	max_pos;		/* Max cursor position of Preedit */
    short	max_pos0;		/* Max cursor position of Status */
    short	note[2];		/*  */
    short	max_cur;
    short	del_x;
    unsigned char	cursor_flag;
    unsigned char	mark_flag;
    unsigned char	u_line_flag;
    unsigned char	r_flag;
    unsigned char	b_flag;
    unsigned char	visible;
    unsigned char	visible_line;
    unsigned char	m_cur_flag;
    WnnClientRec	*w_c;
    int		xjutil_fs_id;
} XIMLangRec;


typedef struct _XIMClientRec *XIMClientList;

typedef struct _XIMClientRec {
    char	*user_name;
    char	*using_language;
    int		lang_num;
    int		number;
    XIMLangRec	**xl;
    XIMLangRec	*cur_xl;
    XIMStyle	input_style;	/* Input style */
    Window	w;			/* Client window */
    unsigned long	mask;		/*  mask value of each element */
    XRectangle	client_area;		/* Area of client window */
    Window	focus_window;
    XRectangle	focus_area;		/* Area of focus window */
    XPoint	point;
    unsigned long	filter_events;	/* Event mask that IM need */
    XIMAttributes	pe;
    XIMAttributes	st;
    int		fd;			/* File descripter */
    short	columns;		/*  */
    short	max_columns;		/*  Max columns */
    short	c0;			/*  column number of first line */
    short	c1;			/*  column number of first line */
    short	c2;			/*  column number of third line */
    short	maxcolumns;
    YesOrNo	*yes_no;
#ifdef	CALLBACKS
    unsigned int	max_keycode;	/* Max keycode */
    unsigned char	cb_redraw_needed;
    int		cb_pe_start;
    int		cb_st_start;
#endif	/* CALLBACKS */
    int		have_ch_lc;
    int		ch_lc_flg;
    int		have_world;
    int		world_on;
#ifdef	XJPLIB
    int		xjp;
#endif	/* XJPLIB */
    struct _XIMClientRec *next;
} XIMClientRec;


typedef struct _Inspect {
    Window	w;
    Window	w1;
    BoxRec	*title;
    BoxRec	*button[MAX_BUTTON];
    int		max_button;
    unsigned char	*msg;
    char	map;
    char	exp;
    char	lock;
    int		max_len;
    unsigned short	save_x, save_y;
} Inspect;

typedef struct	_Ichiran {
    Window	w;
    Window	w0;
    Window	w1;
    BoxRec	*title;
    BoxRec	*subtitle;
    BoxRec	*button[MAX_BUTTON];
    int		max_button;
    GC		invertgc;
    Window	nyuu_w;
    int		kosuu;
    unsigned char	*buf[200];
    unsigned char	**save_buf;
    unsigned char	*nyuu;
    char	select_mode;
    char	map;
    char	exp;
    int		max_line;
    int		page_height;
    int		page;
    int		cur_page;
    int		kosuu_all;
    int		max_len;
    int		mode;
    int		max_lines;
    int		max_columns;
    int		hilited_item;
    int		hilited_ji_item;
    int		init;
    int		item_width;
    int		item_height;
    unsigned short	save_x, save_y;
} Ichiran;

typedef struct  _XInputManager {
    XIMClientRec	*ximclient;
    Ichiran	*ichi;
    Inspect	*inspect;
    XIMClientRec	*j_c;
    int		client_count;
    short	sel_ret;
    XIMStyle	supported_style[MAX_SUPPORT_STYLE];
    char	*supported_language;
    char	sel_button;
    char	exit_menu;
} XInputManager;

typedef struct	_XIMInputRec *XIMInputList;
	
typedef struct	_XIMInputRec {
    Window	w;				/*  Focus window */
    XIMClientRec 	*pclient;	/* Client window structure */
    struct _XIMInputRec *next;
} XIMInputRec;

typedef struct _LangUumrcRec *LangUumrcList;

typedef struct _LangUumrcRec{
    char	*server_name;
    char	*lang_name;
    char	*uumrc_name;
    char	*rkfile_name;
    RomkanTable *rk_table;
#ifdef	USING_XJUTIL
    int		xjutil_act;
    Window	xjutil_id;
    int		xjutil_pid;
    int		xjutil_fs_act;
#endif	/* USING_XJUTIL */
    struct _LangUumrcRec *next;
} LangUumrcRec;

typedef struct _ReadRkfileRec *ReadRkfileList;

typedef struct _ReadRkfileRec {
    char	*name;
    struct _ReadRkfileRec	*next;
} ReadRkfileRec;

extern Display	*dpy;
extern int	screen;
extern Window	root_window;
extern Atom	select_id;
extern XPoint	button;

#define	ClientWidth(xc)			(xc->client_area.width)
#define	ClientHeight(xc)		(xc->client_area.height)
#define	ClientX(xc)			(xc->client_area.x)
#define	ClientY(xc)			(xc->client_area.y)

#define	FocusX(xc)		(xc->focus_window ? \
				 xc->focus_area.x : ClientX(xc))
#define	FocusY(xc)		(xc->focus_window ? \
				 xc->focus_area.y : ClientY(xc))
#define	FocusWidth(xc)		(xc->focus_window ? \
				 xc->focus_area.width : ClientWidth(xc))
#define	FocusHeight(xc)		(xc->focus_window ? \
				 xc->focus_area.height : ClientHeight(xc))

#define	PreeditWidth(xc)	(xc->pe.area.width)
#define	PreeditHeight(xc)	(xc->pe.area.height)
#define	PreeditX(xc)		(xc->pe.area.x)
#define	PreeditY(xc)		(xc->pe.area.y)

#define	StatusWidth(xc)		(xc->st.area.width)
#define	StatusHeight(xc)	(xc->st.area.height)
#define	StatusX(xc)		(xc->st.area.x)
#define	StatusY(xc)		(xc->st.area.y)

#define	PreeditSpotX(xc)	(xc->point.x)
#define	PreeditSpotY(xc)	(xc->point.y)

#define FontWidth(xl)		(xl->pe_b_char->width)
#define	FontAscent(xl)		(xl->pe_b_char->ascent)
#define	FontDescent(xl)		(xl->pe_b_char->descent)
#define FontHeight(xl)		(FontAscent(xl) + FontDescent(xl))
#define StatusFontWidth(xl)	(xl->st_b_char->width)
#define StatusFontAscent(xl)	(xl->st_b_char->ascent)
#define StatusFontDescent(xl)	(xl->st_b_char->descent)
#define StatusFontHeight(xl)	(StatusFontAscent(xl) + StatusFontDescent(xl))

#define	c012(xc)	(xc->c0 + xc->c1 + xc->c2)
#define	c01(xc)		(xc->c0 + xc->c1)

typedef struct _XIMLangDataBase {
	char *lang;
	char *fontset_name;
	Status connect_serv;
	char *lc_name;
	FunctionTable f_table;
} XIMLangDataBase;

#endif	/* !XJUTIL */

#if defined(USING_XJUTIL) || defined(XJUTIL)
typedef struct _Xjutil_startRec {
	char uumkey_name[256];
	char rkfile_name[256];
	char cvtfun_name[256];
	char cvtmeta_name[256];
	char user_name[32];
	char lang[32];
	char lc_name[32];
	unsigned short fn_len;
	unsigned long fore_ground;
	unsigned long back_ground;
	unsigned short max_env;
} Xjutil_startRec;

typedef struct _Xjutil_envRec {
	int fs_id;
	unsigned short fn_len;
	unsigned long fore_ground;
	unsigned long back_ground;
	int cur_env_id;
	int cur_env_reverse_id;
	int env_id[16];
	int env_reverse_id[16];
} Xjutil_envRec;
#endif	/* defined(USING_XJUTIL) || defined(XJUTIL) */
