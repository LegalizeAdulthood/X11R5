/*
 * $Id: xjutil.h,v 1.2 1991/09/16 21:35:36 ohm Exp $
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

	dicserver.h

*********/

#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include "Xi18nint.h"
#include "msg.h"
#include "xim.h"
/*
#include "rk_header.h"
#include "rk_multi.h"
*/

typedef struct _XjutilFSRec {
    XFontSet	fs;
    XCharStruct	*b_char;
    int		act;
    struct _XjutilFSRec	*next;
} XjutilFSRec;

typedef XjutilFSRec	*XjutilFSList;

#define JCLIENTS	4

typedef	struct	_JutilTextRec {
	Window		wp;
	Window		w;
	Window		wn[3];
	long		x;
	long		y;
	long		width;
	long		height;
	long		bp;
	short		max_columns;
	short		vst;
	wchar		*buf;
	unsigned char	*att;
	short		max_pos;
	short		currentcol;
	short		maxcolumns;
	short		note[2];
	unsigned char	cursor_flag;
	unsigned char	mark_flag;
	unsigned char	u_line_flag;
	unsigned char	r_flag;
	unsigned char	b_flag;
} JutilTextRec;

typedef struct _Keytable {
	int		cnt;
	Window		w;
	BoxRec		*button[MAX_KEY_BUTTON];
	int		max_button;
	XPoint		save_p;
	char		map;
	char		exp;
} Keytable;

typedef struct	_Ichiran {
	Window		w;
	Window		w1;
	BoxRec		*rk_mode;
	BoxRec		*title;
	BoxRec		*comment;
	BoxRec		*subtitle;
	BoxRec		*button[MAX_ICHI_BUTTON];
	int		max_button;
	GC		invertgc;
	JutilTextRec	*text;
	int		kosuu;
	unsigned char	*buf[200];
	Keytable	*keytable;
	char		map;
	char		exp;
	char		lock;
	int		mode;
	int		max_line;
	int		start_line;
	int		end_line;
	int		page_height;
	int		page;
	int		cur_page;
	int		kosuu_all;
	int		max_len;
	int		hilited_item;
	int		item_width;
	int		item_height;
	XPoint		save_p;
} Ichiran;


/* For Restory*/
typedef struct	_JutilRec {
	Window		w;
	BoxRec		*rk_mode;
	BoxRec		*title;
	BoxRec		*button[MAX_JU_BUTTON];
	int		max_button;
	BoxRec		*mes_button[JCLIENTS];
	int		mes_mode_return[JCLIENTS];
	int		max_mes;
	JutilTextRec	*mes_text[JCLIENTS];
	int		mode;
	XPoint		save_p;
} JutilRec;


typedef struct  _Xjutil {
	char		*lang;
	char		*lc_name;
	XLocale		xlc;
	Ichiran		*ichi;
	JutilRec	*jutil;
	YesOrNo		*yes_no;
	int		mode;	/* ICHIRAN or JUTIL */
	unsigned long	fg;
	unsigned long	bg;
	GC		gc;
	GC		reversegc;
	GC		invertgc;
	short		sel_ret;
	char		sel_button;
	unsigned char	work;
} Xjutil;

typedef struct _XjutilFuncDadaBase {
	char *lang;
	FunctionTable f_table;
} XjutilFuncDataBase;

extern Display	*dpy;
extern int	screen;
extern Window	root_window;
extern Atom	select_id;

#define FontWidth		(cur_fs->b_char->width)
#define FontAscent		(cur_fs->b_char->ascent)
#define FontDescent		(cur_fs->b_char->descent)
#define FontHeight		(FontAscent + FontDescent)

#define	print_out_func		(f_table->print_out_function)
#define	input_func		(f_table->input_function)
#define	call_t_redraw_move_func	(f_table->call_t_redraw_move_function)
#define	call_t_redraw_move_1_func (f_table->call_t_redraw_move_1_function)
#define	call_t_redraw_move_2_func (f_table->call_t_redraw_move_2_function)
#define	call_t_print_l_func	(f_table->call_t_print_l_function)
#define	redraw_when_chmsig_func	(f_table->redraw_when_chmsig_function)
#define	char_len_func		(f_table->char_len_function)
#define	char_q_len_func		(f_table->char_q_len_function)
#define	t_redraw_move_func	(f_table->t_redraw_move_function)
#define	t_print_l_func		(f_table->t_print_l_function)
#define	c_top_func		(f_table->c_top_function)
#define	c_end_func		(f_table->c_end_function)
#define	c_end_nobi_func		(f_table->c_end_nobi_function)

#define cur_env			(cur_normal_env->env)
