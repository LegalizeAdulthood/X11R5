#ifdef sccs
static char     sccsid[] = "@(#)sb_canvas.h	1.12 91/09/12";
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


/*
 * This include items used main canvas, that's defined in sb_canvas.c.
 */

#define DEFAULT_SPACE_X         30	/* Initial x */
#define DEFAULT_NEW_MARK_POSITION       2	/* Initial X of 'N' */
#define DEFAULT_CUR_MARK_POSITION       15	/* Initial X of '=>' */
#define DEFAULT_SPACE           3	/* Space between the msg of the
					 * header pane.  */

extern long     dclick_usec;
extern int      sb_font_height;
extern int      sb_font_width;
extern int      sb_line_height;
extern int      sb_line_width;	/* Need to re-calc if resize */

extern Scrollbar sb_scrollbar;
extern Canvas   sb_canvas;
extern Drawable sb_draw;
extern Display *sb_dpy;
extern GC       sb_gc;
extern GC       sb_cleargc;

extern Seln_rank sb_select_flag;
extern Seln_client sb_seln_client;

extern int      sb_num_of_article;
extern char    *sb_seln_file;
