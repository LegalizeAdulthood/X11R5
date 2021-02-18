#ifdef sccs
static char     sccsid[] = "@(#)sb_def.h	1.8 91/09/12";
#endif
/*
****************************************************************************

              Copyright 1991, by Sun Microsystems, Inc.

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Sun Microsystems, Inc.
not be used in advertising or publicity pertaining to distribution of
the software without specific, written prior permission.  
Sun Microsystems, Inc. makes no representations about the suitability
of this software for any purpose.  It is provided "as is" without 
express or implied warranty.

SUN MICROSYSTEMS, INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL  SUN MICROSYSTEMS, INC. BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Auther: Hiroshi Watanabe	(nabe@japan.sun.com) Sun Microsystems, Inc.
	Tomonori Shioda		(shioda@japan.sun.com) Sun Microsystems, Inc.

****************************************************************************
*/


#define KIND_WIN 	10
#define COPOSE_TYPE 	11
#define NEXT_PROC_TYPE 	12
#define FIND_TYPE 	13
#define SUBJECT_NEXT_TYPE 14
#define SUPER_NEXT 	15
#define NEXT_NG 	16
#define INCLUDE_TYPE 17

typedef enum {
	NEW_POST,
	REPLY_POST,
	INDENT_POST,
	BRACKETED_POST,
	SELECTED_POST,
	SELECTED_REFER_POST
}               Sb_post_window_type;

typedef enum {
	NEWS,
	MAIL
}               Sb_post_type;

typedef enum {
	NEXT,
	PREV
}               Sb_next_type;

typedef enum {
	FNEXT,
	FPREV,
	FALL
}               Sb_find_type;

typedef enum {
	VSELECT,
	VADJUST
}               Sb_view_seln_type;

typedef enum {
	Sb_pane_height = 0,
	Sb_pane_width = 2,
	Sb_post_height = 4,
	Sb_view_height = 6,
	Sb_max_msg = 8,
	Sb_print_script = 10,
	Sb_article_display = 11,
	Sb_default_post_header = 12,
	Sb_default_header_type = 13,
	Sb_indent_prefix = 14,
	Sb_biff_timer = 15,
	Sb_biff_beep = 17,
	Sb_biff_move = 19,
	Sb_include_sender = 21
} Sb_props_type;
