#ifdef sccs
static char     sccsid[] = "@(#)sb_item.h	1.1 91/04/24";
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
 * This include items used main panel, that's defined in sb_pane.c.
 */

/*
 * Used for main frame
 */
extern Frame    sb_frame;	/* main frame */
extern Panel    sb_panel;	/* main panel */
extern Icon     sb_frame_icon;	/* frame icon */
extern Panel_item sb_group_txt;	/* panel text in main frame */
extern Menu     sb_group_menu;	/* menu in main frame */

/*
 * Used for find frame
 */
extern Frame    sb_find_frame;	/* find frame */
extern Panel_item sb_find_sender_txt;	/* panel text in find frame */
extern Panel_item sb_find_subject_txt;	/* panel text in find frame */

/*
 * Used for cancel frame
 */
extern Frame    sb_cancel_frame;/* cancel frame */
extern Panel_item sb_cancel_group_txt;	/* panel text used for group name */
extern Panel_item sb_cancel_newsno_txt;	/* panel text used for newsno name */

/*
 * Used for biff
 */
extern Frame    sb_newsbiff_frame;
extern Panel_item sb_newsbiff_panel;
