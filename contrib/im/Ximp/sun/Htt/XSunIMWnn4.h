/*
 *     (C) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *     pending in the U.S. and foreign countries. See LEGAL NOTICE
 *     file for terms of the license.
 * 
 *     @(#)iml_private.h 1.5 90/12/12  
 */
#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunIMWnn4.h 1.3 91/08/14";
#endif
#endif

/******************************************************************

              Copyright 1990, 1991, by Sun Microsystems, Inc.

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sun Microsystems, Inc.
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.
Sun Microsystems, Inc. makes no representations about the suitability of
this software for any purpose.  It is provided "as is" without
express or implied warranty.

Sun Microsystems Inc. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL Sun Microsystems, Inc. BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

  Author: Hideki Hiura (hhiura@Sun.COM)
	  				     Sun Microsystems, Inc.
******************************************************************/


#ifndef xim_imlogic_wnn403_DEFIND
#define xim_imlogic_wnn403_DEFIND

typedef struct _imlogic_session {
/* ---- Inherit Base class [imlogic_session] ----- */

#define xim_iml_inheritance_DEFIND
#include "XSunIMCore.h"
#undef  xim_iml_inheritance_DEFIND

/* ----- Derived class [wnn403_session] declaration ---- */

/*
 * jclib, cconv related member
 */
    WNN_JSERVER_ID *jserver_id ;	
    WNN_ENV *wnn_env ;
    jcConvBuf *jcconvbuf ;
    ccRule ccrule ;
    ccBuf ccbuf ;
/*
 * private member
 */
    int state ;
/*
 * Region specific member
 *
 * Preedit region
 */
    wchar_t     *pets;      /* allocate once, will be recycled */
    int          petssz;
    XIMFeedback *petf;      /* allocate once, will be recycled */
    int          petfsz;

/*
 * Status region
 */

/*
 * Lookup region
 */

/*
 * Aux region
 */

    
} iml_session_t;

#define xim_imlogic_session_DEFIND

/*
 * cconv functbl
 */
typedef struct _functblt_t {
    char 	*fname ;
    void	(*func_normal)();
    void	(*func_selection)();
    void	(*func_symbol)();
}    functbl_t ;


/*
 * Wnn403 public member functions
 */



/*
 * Wnn403 private functions
 */
Private int wnn403_open();
Private void wnn403_close();
Private void wnn403_reset();
/* 
Private int wnn403_get_key();
Private int wnn403_get_key_map();
Private int wnn403_set_key();
Private int wnn403_set_key_map();
Private int wnn403_set_keyboard_state();
*/
Private int wnn403_send_event();
Private void wnn403_conversion_on();
Private void wnn403_send_results();
Private void wnn403_send_commit();

Private void make_conversion_on();

Private void	convert();
Private void	convert_sp();
Private void	convert_small();
Private void	unconvert();
Private void	select_next();
Private void	select_next_small();
Private void	select_prev();
Private void	select_prev_small();
Private void	move_forward();
Private void	move_backward();
Private void	move_top();
Private void	move_bottom();
Private void	clear_buffer();
Private void	symbol_top();
Private void	symbol_bottom();
Private void	symbol_forward();
Private void	symbol_backward();
Private void	symbol_next();
Private void	symbol_prev();
Private void	symbol_select();
Private void	symbol_abort();
Private void	selection_top();
Private void	selection_bottom();
Private void	selection_forward();
Private void	selection_backward();
Private void	selection_next();
Private void	selection_prev();
Private void	selection_select();
Private void	expand_clause();
Private void	expand_clause_small();
Private void	shrink_clause();
Private void	shrink_clause_small();
Private void	expand_clause2();
Private void	expand_clause2_small();
Private void	shrink_clause2();
Private void	shrink_clause2_small();
Private void	commit();
Private void	commit_or_cr();
Private void	hankaku();
Private void	zenkaku();
Private void	hiragana();
Private void	katakana();
Private void	backspace();
Private void	delete();
Private void	fixwrite();
Private void	beep();
Private void	cr();
Private void	jiscode_begin();
Private void	jiscode_end();
Private void	symbol_input();
Private void	inspect();
Private void	convend();
Private void     insert_char();
Private void     delete_char();
Private void     function_dispatch();
Private void     mode_notify();

#define MODE_NORMAL		0
#define MODE_SELECTION_S	1
#define MODE_SELECTION_L	2
#define MODE_SYMBOL		3

#endif  xim_imlogic_private_DEFINED
