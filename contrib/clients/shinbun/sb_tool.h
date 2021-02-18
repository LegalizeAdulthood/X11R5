#ifdef sccs
static char     sccsid[] = "@(#)sb_tool.h	1.1 91/04/24";
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


extern Server_image sb_image;
extern Server_image sb_mask_image;
extern Server_image sb_arrive_mask_image;
extern Server_image sb_arrive_image;
extern Server_image sb_non_arrive_mask_image;
extern Server_image sb_non_arrive_image;
extern Server_image sb_post_image;
extern Server_image sb_post_mask_image;

extern Xv_Cursor article_cursor;
extern Xv_Cursor articles_cursor;

extern Xv_server xserver;
extern Display *dpy;
extern int      screen_no;

extern void     window_check();
extern void     frame_msg();
extern void     frame_err();
extern void     one_notice();
extern int      two_notice();
extern int      three_notice();
extern void     sb_icon_create();

extern void     fcopy();
extern int      sb_center_items();
extern void     rm_return();
extern char    *rm_colon();
extern void     get_screen();
extern int      list_add_next();
extern int      list_add_prev();
extern void     list_delete();
extern void     list_insert();
extern void     list_all_delete();
extern void     pop_up_beside();
extern char    *is_to_low();
extern void     frame_destroy_proc();
extern void     free_key_data();
extern char    *get_text_seln();
