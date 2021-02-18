#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)term_ntfy.c 70.1 91/08/22";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Notifier related routines for the termsw.
 */
#include <sys/types.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sgtty.h>
#include <signal.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <xview/notice.h>
#include <xview/notify.h>
#include <xview/rect.h>
#include <xview/rectlist.h>
#include <xview/win_input.h>
#include <xview/win_notify.h>
#include <xview/ttysw.h>
#include <xview/termsw.h>
#include <xview/window.h>
#include <xview_private/tty_impl.h>
#include <xview_private/term_impl.h>
#include <xview_private/ultrix_cpt.h>

#ifdef OW_I18N 
#include <xview_private/draw_impl.h>
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

#define PTY_OFFSET	(int) &(((Ttysw_folio)0)->ttysw_pty)

extern void     ttysw_display_capslock();
extern Textsw   textsw_first();
extern Textsw   textsw_next();
extern int      textsw_default_notify();
extern char    *textsw_checkpoint_undo();
extern Textsw_index textsw_insert();
extern Xv_opaque xv_set();
extern Textsw_index textsw_erase();
extern void     textsw_display();

#ifdef DEBUG
#define ERROR_RETURN(val)	abort();	/* val */
#else
#define ERROR_RETURN(val)	return(val);
#endif				/* DEBUG */

/* performance: global cache of getdtablesize() */
extern int      dtablesize_cache;
#define GETDTABLESIZE() \
        (dtablesize_cache?dtablesize_cache:(dtablesize_cache=getdtablesize()))

Notify_value    ttysw_text_destroy();	/* Destroy func for termsw */
Notify_value    ttysw_text_event();	/* Event func for termsw */

extern void     ttysw_sendsig();
extern void     ttysw_sigwinch();

/* shorthand - Duplicate of what's in ttysw_main.c */

#define	iwbp	ttysw->ttysw_ibuf.cb_wbp
#define	iebp	ttysw->ttysw_ibuf.cb_ebp
#define	ibuf	ttysw->ttysw_ibuf.cb_buf


#ifdef SUNVIEW1
/* BUG ALERT: MAS, 13 AUG 87 -- I bet this routine is wrong! */

Pkg_private     Notify_value
ttysw_text_destroy(textsw, status)
    register Textsw textsw;
    Destroy_status  status;
{
    Notify_value    nv = NOTIFY_IGNORED;
    int             last_view;
    Termsw_view_handle view = TERMSW_VIEW_PRIVATE_FROM_TEXTSW(textsw);
    Termsw_folio    termsw = TERMSW_FOLIO_FOR_VIEW(view);
    Ttysw_folio     ttysw = TTY_FROM_TERMSW(termsw->public_self);
    /* WARNING: ttysw may be NULL */

    /*
     * WARNING: call on notify_next_destroy_func invalidates textsw, and thus
     * the information about the textsw must be extracted first.
     */
    last_view = (textsw_next(textsw_first(textsw)) == (Textsw) 0);

    if (status == DESTROY_CHECKING) {
	return (NOTIFY_IGNORED);
    } else {
	nv = notify_next_destroy_func((Notify_client) (textsw), status);
    }

    if (last_view && ttysw) {
	nv = (Notify_value) ttysw_destroy(ttysw, status);
    }
    return nv;
}

#endif

/* #ifdef TERMSW */
static          Textsw_index
find_and_remove_mark(textsw, mark)
    Textsw          textsw;
    Textsw_mark     mark;
{
    Textsw_index    result;

    result = textsw_find_mark(textsw, mark);
    if (result != TEXTSW_INFINITY)
	textsw_remove_mark(textsw, mark);
    return (result);
}

Pkg_private void
ttysw_move_mark(textsw, mark, to, flags)
    Textsw          textsw;
    Textsw_mark    *mark;
    Textsw_index    to;
    int             flags;
{
    textsw_remove_mark(textsw, *mark);
    *mark = textsw_add_mark(textsw, to, (unsigned) flags);
}

Pkg_private     Notify_value
ttysw_text_event(textsw, event, arg, type)
    register Textsw_view textsw;/* This is really a public termsw view */
    Event          *event;
    Notify_arg      arg;
    Notify_event_type type;
{
    int             insert = TEXTSW_INFINITY;
    int             length = TEXTSW_INFINITY;
    int             cmd_start;
    int             did_map = 0;
    Notify_value    nv = NOTIFY_IGNORED;
    register        Termsw_folio
                    termsw = TERMSW_FOLIO_FROM_TERMSW_VIEW(textsw);
    register        Ttysw_folio
                    ttysw = TTY_FROM_TERMSW(termsw->public_self);
    register        Ttysw_view_handle
                    ttysw_view = TTY_VIEW_PRIVATE_FROM_ANY_VIEW(textsw);
    register int    action = (termsw->cooked_echo)
    ? event_action(event) : event_id(event);
    register int    down_event = event_is_down(event);

    if (!down_event && (action >= ASCII_FIRST) && (action <= ASCII_LAST))
	return (NOTIFY_DONE);

    if ((action == ACTION_MENU) && down_event) {

	/*
	 * The view that should be affected by the menu is the one that the
	 * menu appears over.  This must be reflected in the value of the
	 * MENU_CLIENT_DATA.  Relying on the view that the ttysw last
	 * remembered when it got a KBD_USE is wrong in click-to-type.
	 */
	ttysw_show_walkmenu(textsw, event);
	return (NOTIFY_DONE);

    }
    if (!ttysw_getopt((caddr_t) ttysw, TTYOPT_TEXT)) {
	if (textsw == TTY_VIEW_PUBLIC(ttysw_view)) {
	    extern Notify_value ttysw_event();
	    nv = ttysw_event(TTY_VIEW_PUBLIC(ttysw_view), event, arg, type);
	} else {
	    nv = notify_next_event_func((Notify_client) (textsw),
					(Notify_event) event, arg, type);
	}
	return (nv);
    }
    if (termsw->cooked_echo && down_event) {
	if (termsw->cmd_started) {
	    if (termsw->literal_next && action <= ASCII_LAST &&
		(insert = (int) xv_get(textsw, TEXTSW_INSERTION_POINT)) ==
#ifdef OW_I18N		
		(length = (int) xv_get(textsw, TEXTSW_LENGTH_WC))) {
#else
		(length = (int) xv_get(textsw, TEXTSW_LENGTH))) {
#endif		
		char            input_char = (char) action;

		/* Following call changes termsw state by side effect. */
		textsw_replace_bytes(textsw, length - 1, length, &input_char, 1);
		termsw->literal_next = FALSE;
		return NOTIFY_DONE;
	    }
	} else if ((insert = (int) xv_get(textsw, TEXTSW_INSERTION_POINT)) ==
#ifdef OW_I18N			
		   (length = (int) xv_get(textsw, TEXTSW_LENGTH_WC))) {
#else
		   (length = (int) xv_get(textsw, TEXTSW_LENGTH))) {
#endif		   
	    (void) textsw_checkpoint_again(textsw);
	}
    }
    /* ^U after prompt, before newline should only erase back to prompt. */
    if (termsw->cooked_echo
	&& event_id(event) == (short) termsw->erase_line
	&& down_event
	&& !event_shift_is_down(event)
	&& termsw->cmd_started != 0
	&& ((insert = (int) xv_get(textsw, TEXTSW_INSERTION_POINT))) >
	(cmd_start = (int) textsw_find_mark(textsw, termsw->user_mark))) {
	int             pattern_start = cmd_start;
	int             pattern_end = cmd_start;
	char            newline = '\n';

	if (textsw_find_bytes(
		     textsw, (long *) &pattern_start, (long *) &pattern_end,
			      &newline, 1, 0) == -1
	 || (pattern_start <= cmd_start || pattern_start >= (insert - 1))) {
	    (void) textsw_erase(textsw,
			   (Textsw_index) cmd_start, (Textsw_index) insert);
	    return NOTIFY_DONE;
	}
    }
    if (!termsw->cooked_echo
	&& (action == '\r' || action == '\n')) {
	/* Implement "go to end of file" ourselves. */
	/* First let textsw do it to get at normalize_internal. */
	nv = notify_next_event_func((Notify_client) (textsw),
				    (Notify_client) (event), arg, type);
	/*
	 * Now fix it up. Only necessary when !append_only_log because
	 * otherwise the read-only mark at INFINITY-1 gets text to implement
	 * this function for us.
	 */
	if (!termsw->append_only_log)
	    (void) xv_set(textsw, TEXTSW_INSERTION_POINT,
			  textsw_find_mark(textsw, termsw->pty_mark), 0);
    } else if (!termsw->cooked_echo
	       && action <= ASCII_LAST
	       && (iscntrl((char) action) || (char) action == '\177')
	       && (insert = (int) xv_get(textsw, TEXTSW_INSERTION_POINT))
	       == textsw_find_mark(textsw, termsw->pty_mark)) {
	/* In !cooked_echo, ensure textsw doesn't gobble up control chars */
	char            input_char = (char) action;
	(void) textsw_insert(textsw, &input_char, (long) 1);
	nv = NOTIFY_DONE;
    } else if (termsw->cooked_echo && down_event) {
	if (action == (short) termsw->tchars.t_stopc) {
	    /* implement flow control characters as page mode */
	    (void) ttysw_freeze(ttysw_view, 1);
	} else if (action == (short) termsw->tchars.t_startc) {
	    (void) ttysw_freeze(ttysw_view, 0);
	    (void) ttysw_reset_conditions(ttysw_view);
	} else if (action != (short) termsw->tchars.t_eofc) {
	    /* Nice normal event */
	    nv = notify_next_event_func((Notify_client) (textsw),
					(Notify_event) (event), arg, type);
	}
    } else {
	/* Nice normal event */
	nv = notify_next_event_func((Notify_client) (textsw),
				    (Notify_event) (event), arg, type);
    }
       /* jcb 5/29/90 -- changed back to original state */
    if ( /* down_event ||*/ nv == NOTIFY_IGNORED) { /* jcb 5/8/90 */
	if ((action > ISO_LAST) &&
	    (action < LOC_MOVE || action > WIN_UNUSED_11)) {
	    did_map = (ttysw_domap(ttysw, event) == TTY_DONE);
	    nv = did_map ? NOTIFY_DONE : NOTIFY_IGNORED;
	}
    }
    /* the following switch probably belongs in a state transition table */
    switch (event_id(event)) {
      case WIN_REPAINT:
      case WIN_GRAPHICS_EXPOSE:
	ttysw_sigwinch(ttysw);
	nv = NOTIFY_DONE;
	break;
      case WIN_RESIZE:
	(void) ttysw_resize(ttysw_view);
	nv = NOTIFY_DONE;
	break;
      case KBD_USE:
#ifdef  OW_I18N
	if (!ttysw_getopt(ttysw, TTYOPT_TEXT) &&
	    xv_get(TTY_PUBLIC(ttysw), WIN_USE_IM)) {	    
	        Xv_Drawable_info *info;
        	
        	DRAWABLE_INFO_MACRO(textsw, info);
	        XSetICValues(ttysw->ic, XNFocusWindow, xv_xid(info), NULL);

		XSetICFocus(ttysw->ic);
	}
#endif
	if (TTY_IS_TERMSW(ttysw)) {
	    /*
	     * Update public_self as it tracks the text view that is
	     * transformed into a "pure" ttysw.
	     */
	    ttysw->current_view_public = (Tty_view) textsw;
	}
	/*
	 * Update pty's idea of tty size in case a descendant process looks
	 * at the TERMCAP.  (ttysw_be_ttysw() is too late, because vi, etc.
	 * will have already read the size of the terminal.)
	 */
	(void) xv_tty_new_size(ttysw, textsw_screen_column_count(TERMSW_PUBLIC(termsw)),
			   textsw_screen_line_count(TERMSW_PUBLIC(termsw)));
	break;
      case KBD_DONE:
      case LOC_MOVE:
      case LOC_WINENTER:
      case LOC_WINEXIT:
	break;
      default:
#ifdef DEBUG
	if (action <= ASCII_LAST) {
	    int             ctrl_state = event->ie_shiftmask & CTRLMASK;
	    int             shift_state = event->ie_shiftmask & SHIFTMASK;
	    char            ie_code = action;
	}
#endif				/* DEBUG */
	if (!termsw->cooked_echo)
	    break;
	/* Only look at the down event for control key */
	if (event_is_up(event))
	    break;
	/* Only send interrupts when characters are actually typed. */
	if (action == termsw->tchars.t_intrc) {
	    ttysw_sendsig(ttysw, textsw, SIGINT);
	} else if (action == termsw->tchars.t_quitc) {
	    ttysw_sendsig(ttysw, textsw, SIGQUIT);
	} else if (action == termsw->ltchars.t_suspc
		   || action == termsw->ltchars.t_dsuspc) {
	    ttysw_sendsig(ttysw, textsw, SIGTSTP);
	} else if (action == termsw->tchars.t_eofc) {
	    if (insert == TEXTSW_INFINITY)
		insert = (int) xv_get(textsw, TEXTSW_INSERTION_POINT);
	    if (length == TEXTSW_INFINITY)
#ifdef OW_I18N	    
		length = (int) xv_get(textsw, TEXTSW_LENGTH_WC);
#else
		length = (int) xv_get(textsw, TEXTSW_LENGTH);
#endif		
	    if (length == insert) {
		/* handle like newline or carriage return */
		if (termsw->cmd_started
		  && length > textsw_find_mark(textsw, termsw->user_mark)) {
		    if (ttysw_scan_for_completed_commands(ttysw_view, -1, 0))
			nv = NOTIFY_IGNORED;
		} else {
		    /* but remember to send eot. */
		    termsw->pty_eot = iwbp - ibuf;
		    termsw->cmd_started = 0;
		    (void) ttysw_reset_conditions(ttysw_view);
		}
	    } else {		/* length != insert */
		nv = notify_next_event_func((Notify_client) (textsw),
					(Notify_client) (event), arg, type);
	    }
	}
    }				/* switch */
    return (nv);
}

static          Notify_value
ttysw_cr(tty_public, tty)
    Tty             tty_public;
    int             tty;
{
    int             nfds = 0;
    fd_set          wfds;
    static struct timeval timeout = {0, 0};
    int             maxfds = GETDTABLESIZE();
    Ttysw_folio     ttysw_folio = TTY_PRIVATE_FROM_ANY_PUBLIC(tty_public);

    /*
     * GROSS HACK:
     * 
     * There is a race condition such that between the notifier's select() call
     * and our write, the app may write to the tty, causing our write to
     * block.  The tty cannot be flushed because we don't get to read the pty
     * because our write is blocked. This GROSS HACK doesn't eliminate the
     * race condition; it merely narrows the window, making it less likely to
     * occur. We don't do an fcntl(tty, FN_NODELAY) because that affects the
     * file, not merely the file descriptor, and we don't want to change what
     * the application thinks it sees.
     * 
     * The right solution is either to invent an ioctl that will allow us to set
     * the tty driver's notion of the cursor position, or to avoid using the
     * tty driver altogether.
     */
    FD_ZERO(&wfds);
    FD_SET(tty, &wfds);
    if ((nfds = select(maxfds, NULL, &wfds, NULL, &timeout)) < 0) {
	perror("ttysw_cr: select");
	return (NOTIFY_IGNORED);
    }
    if (nfds == 0 || !FD_ISSET(tty, &wfds)) {
	return (NOTIFY_IGNORED);
    }
    if (write(tty, "\r", 1) < 0) {
	fprintf(stderr, "for tty%x, tty fd %d, ",
		tty_public, ttysw_folio->ttysw_tty);
#ifdef OW_I18N
	perror(XV_I18N_MSG("xv_messages", "TTYSW tty write failure"));
#else
	perror("TTYSW tty write failure");
#endif
    }
    (void) notify_set_output_func((Notify_client) (tty_public),
				  NOTIFY_FUNC_NULL, tty);
    return (NOTIFY_DONE);
}

static void
ttysw_reset_column(ttysw)
    Ttysw_folio     ttysw;
{
    Tty             tty_public = TTY_PUBLIC(ttysw);
    Termsw_folio    termsw = TERMSW_PRIVATE_FROM_TTY_PRIVATE(ttysw);

    /* BUG ALERT accessing field of termsw */
    if ((termsw->sgttyb.sg_flags & XTABS)
	&& notify_get_output_func((Notify_client) (tty_public),
				  ttysw->ttysw_tty) != ttysw_cr) {
	if (notify_set_output_func((Notify_client) (tty_public),
			  ttysw_cr, ttysw->ttysw_tty) == NOTIFY_FUNC_NULL) {
#ifdef OW_I18N
	    fprintf(stderr,
		    XV_I18N_MSG("xv_messages", "cannot set output func on ttysw %x, tty fd %d\n"),
		    ttysw, ttysw->ttysw_tty);
#else
	    fprintf(stderr,
		    "cannot set output func on ttysw %x, tty fd %d\n",
		    ttysw, ttysw->ttysw_tty);
#endif
	}
    }
}

Pkg_private void
ttysw_post_error(public_folio_or_view, msg1, msg2)
    Xv_opaque       public_folio_or_view;	/* Public handle */
    char           *msg1, *msg2;
{
    char            buf[1000];
    int             size_to_use = sizeof(buf);

    buf[0] = '\0';
    (void) strncat(buf, msg1, size_to_use);
    if (msg2) {
	int             len = strlen(buf);
	if (len < size_to_use) {
	    (void) strncat(buf, msg2, size_to_use - len);
	}
    }
    (void) notice_prompt(
			 xv_get(public_folio_or_view, WIN_FRAME),
			 (Event *) 0,
			 NOTICE_BUTTON_YES, "Continue",
			 NOTICE_TRIGGER, ACTION_STOP,
			 NOTICE_MESSAGE_STRINGS,
			 buf,
			 0,
			 0);
}


int
ttysw_scan_for_completed_commands(ttysw_view, start_from, maybe_partial)
    Ttysw_view_handle ttysw_view;
    int             start_from;
    int             maybe_partial;
{
    Ttysw_folio     ttysw = TTY_FOLIO_FROM_TTY_VIEW_HANDLE(ttysw_view);
    register Textsw textsw = TEXTSW_FROM_TTY(ttysw);
    register Termsw_folio termsw =
    TERMSW_FOLIO_FOR_VIEW(TERMSW_VIEW_PRIVATE_FROM_TEXTSW(textsw));
    register CHAR  *cp;
#ifdef OW_I18N    
    int             length = (int) xv_get(textsw, TEXTSW_LENGTH_WC);
#else
    int             length = (int) xv_get(textsw, TEXTSW_LENGTH);
#endif    
    int             use_mark = (start_from == -1);
    int             cmd_length;

    if (use_mark) {
	if (TEXTSW_INFINITY == (
		  start_from = textsw_find_mark(textsw, termsw->user_mark)))
	    ERROR_RETURN(1);
	if (start_from == length)
	    return (0);
    }
    cmd_length = length - start_from;
    /* Copy these commands into the buffer for pty */
    if ((iwbp + cmd_length) < iebp) {
#ifdef	OW_I18N
	(void) xv_get(textsw,
		      TEXTSW_CONTENTS_WCS, start_from, iwbp, cmd_length);
#else
	(void) xv_get(textsw,
		      TEXTSW_CONTENTS, start_from, iwbp, cmd_length);
#endif
	if (maybe_partial) {
	    /* Discard partial commands. */
	    for (cp = iwbp + cmd_length - 1; cp >= iwbp; --cp) {
		switch (*cp) {
		  case '\n':
		  case '\r':
		    goto Done;
		  default:
		    if (*cp == termsw->tchars.t_brkc)
			goto Done;
		    cmd_length--;
		    break;
		}
	    }
	}
Done:
	if (cmd_length > 0) {
	    iwbp += cmd_length;
	    cp = iwbp - 1;
	    (void) ttysw_reset_conditions(ttysw_view);
#ifdef	OW_I18N
	    if (*cp == (wchar_t)'\n'
		|| *cp == (wchar_t)'\r') {
#else
	    if (*cp == '\n'
		|| *cp == '\r') {
#endif
		ttysw_reset_column(ttysw);
	    }
	    ttysw_move_mark(textsw, &termsw->pty_mark,
			    (Textsw_index) (start_from + cmd_length),
			    TEXTSW_MARK_DEFAULTS);
	    if (termsw->cmd_started) {
		if (start_from + cmd_length < length) {
		    ttysw_move_mark(textsw, &termsw->user_mark,
				    (Textsw_index) (start_from + cmd_length),
				    TEXTSW_MARK_DEFAULTS);
		} else {
		    termsw->cmd_started = 0;
		}
		if (termsw->append_only_log) {
		    ttysw_move_mark(textsw, &termsw->read_only_mark,
				    (Textsw_index) (start_from + cmd_length),
				    TEXTSW_MARK_READ_ONLY);
		}
	    }
	    termsw->pty_owes_newline = 0;
	}
	return (0);
    } else {
#ifdef OW_I18N
	ttysw_post_error(textsw,
		  XV_I18N_MSG("xv_messages", "Pty cmd buffer overflow: last cmd ignored."), (char *) 0);
#else
	ttysw_post_error(textsw,
		  "Pty cmd buffer overflow: last cmd ignored.", (char *) 0);
#endif
	return (0);
    }
}

void
ttysw_doing_pty_insert(textsw, commandsw, toggle)
    register Textsw textsw;
    Termsw_folio    commandsw;
    int        	    toggle;
{
    unsigned        notify_level = (unsigned) window_get(textsw,
						       TEXTSW_NOTIFY_LEVEL);
    commandsw->doing_pty_insert = toggle;
    if (toggle) {
	window_set(textsw,
		   TEXTSW_NOTIFY_LEVEL, notify_level & (~TEXTSW_NOTIFY_EDIT),
		   0);
    } else {
	window_set(textsw,
		   TEXTSW_NOTIFY_LEVEL, notify_level | TEXTSW_NOTIFY_EDIT,
		   0);
    }
}

Xv_private int
ttysw_cooked_echo_cmd(ttysw_view, buf, buflen)
    Ttysw_view_handle ttysw_view;
    char           *buf;
    int             buflen;
{
    Ttysw_folio     ttysw = TTY_FOLIO_FROM_TTY_VIEW_HANDLE(ttysw_view);
    register Textsw textsw = TEXTSW_FROM_TTY(ttysw);
    register Termsw_folio termsw =
    TERMSW_FOLIO_FOR_VIEW(TERMSW_VIEW_PRIVATE_FROM_TEXTSW(textsw));
    Textsw_index    insert = (Textsw_index) xv_get(textsw,
						   TEXTSW_INSERTION_POINT);
#ifdef OW_I18N						   
    int             length = (Textsw_index) xv_get(textsw, TEXTSW_LENGTH_WC);
#else
    int             length = (Textsw_index) xv_get(textsw, TEXTSW_LENGTH);
#endif        
    Textsw_index    insert_at;
    Textsw_mark     insert_mark;

    if (termsw->append_only_log) {
	textsw_remove_mark(textsw, termsw->read_only_mark);
    }
    if (termsw->cmd_started) {
	insert_at = find_and_remove_mark(textsw, termsw->user_mark);
	if (insert_at == TEXTSW_INFINITY)
	    ERROR_RETURN(-1);
	if (insert == insert_at) {
	    insert_mark = TEXTSW_NULL_MARK;
	} else {
	    insert_mark =
		textsw_add_mark(textsw, insert, TEXTSW_MARK_DEFAULTS);
	}
    } else {
	if (insert == length)
	    (void) textsw_checkpoint_again(textsw);
	termsw->next_undo_point = textsw_checkpoint_undo(textsw,
						 (caddr_t) TEXTSW_INFINITY);
	insert_at = length;
    }
    if (insert != insert_at) {
	(void) xv_set(textsw, TEXTSW_INSERTION_POINT, insert_at, 0);
    }
    (void) textsw_checkpoint_undo(textsw, termsw->next_undo_point);
    /* Stop this insertion from triggering the cmd scanner! */
    ttysw_doing_pty_insert(textsw, termsw, TRUE);
    (void) textsw_insert(textsw, buf, (long) buflen);
    ttysw_doing_pty_insert(textsw, termsw, FALSE);
    (void) ttysw_scan_for_completed_commands(ttysw_view, (int) insert_at, TRUE);
    if (termsw->cmd_started) {
	insert_at = (Textsw_index) xv_get(textsw, TEXTSW_INSERTION_POINT);
	if (insert_at == TEXTSW_INFINITY)
	    ERROR_RETURN(-1);
	termsw->user_mark =
	    textsw_add_mark(textsw, (Textsw_index) insert_at, TEXTSW_MARK_DEFAULTS);
	if (termsw->append_only_log) {
	    termsw->read_only_mark =
		textsw_add_mark(textsw,
		      termsw->cooked_echo ? insert_at : TEXTSW_INFINITY - 1,
				TEXTSW_MARK_READ_ONLY);
	}
	if (insert_mark != TEXTSW_NULL_MARK) {
	    insert = find_and_remove_mark(textsw, insert_mark);
	    if (insert == TEXTSW_INFINITY)
		ERROR_RETURN(-1);
	    (void) xv_set(textsw, TEXTSW_INSERTION_POINT, insert, 0);
	}
    } else {
	if (insert < length)
	    (void) xv_set(textsw, TEXTSW_INSERTION_POINT, insert, 0);
	if (termsw->append_only_log) {
#ifdef OW_I18N	
	    length = (int) xv_get(textsw, TEXTSW_LENGTH_WC);
#else
	    length = (int) xv_get(textsw, TEXTSW_LENGTH);
#endif	    
	    termsw->read_only_mark =
		textsw_add_mark(textsw,
				(Textsw_index) (termsw->cooked_echo ? length : TEXTSW_INFINITY - 1),
				TEXTSW_MARK_READ_ONLY);
	}
    }
    return (0);
}

#ifdef	OW_I18N
/* ARGSUSED */
static void
ttysw_textsw_changed_handler(textsw, insert_before, length_before,
			     replaced_from, replaced_to, count_inserted)
    Textsw          textsw;
    int             insert_before;
    int             length_before;
    int             replaced_from;
    int             replaced_to;
    int             count_inserted;
{
    CHAR            last_inserted;
    char	    last_inserted_mbs[4];
    int		    mbs_len;
    Termsw_view_handle view = TERMSW_VIEW_PRIVATE_FROM_TEXTSW(textsw);
    Termsw_folio    termsw = TERMSW_FOLIO_FOR_VIEW(view);
    Ttysw_folio     ttysw = TTY_FROM_TERMSW(termsw->public_self);
    Ttysw_view_handle ttysw_view = TTY_VIEW_PRIVATE_FROM_TERMSW_VIEW(TERMSW_VIEW_PUBLIC(view));

    if (insert_before != length_before)
	return;
    if (termsw->cmd_started == 0) {
	if (termsw->cmd_started = (count_inserted > 0)) {
	    (void) textsw_checkpoint_undo(textsw, termsw->next_undo_point);
	    ttysw_move_mark(textsw, &termsw->user_mark,
			    (Textsw_index) length_before,
			    TEXTSW_MARK_DEFAULTS);
	}
    }
    if (!termsw->cmd_started)
	termsw->next_undo_point =
	    (caddr_t) textsw_checkpoint_undo(textsw,
					     (caddr_t) TEXTSW_INFINITY);
    if (count_inserted >= 1) {
	/* Get the last inserted character. */
	(void) xv_get(textsw, TEXTSW_CONTENTS_WCS,
		      replaced_from + count_inserted - 1,
		      &last_inserted, 1);
	mbs_len = wctomb( last_inserted_mbs , last_inserted );
	if ( mbs_len == 1  && 
		last_inserted_mbs[0] == termsw->ltchars.t_rprntc ) {

#ifndef	BUFSIZE
#define	BUFSIZE 1024
#endif				/* BUFSIZE */
	    CHAR            buf[BUFSIZE + 1];
	    CHAR            cr_nl[3];
	    int             buflen = 0;
	    Textsw_index    start_from;
	    Textsw_index    length =
	    (int) xv_get(textsw, TEXTSW_LENGTH_WC);

	    cr_nl[0] = (wchar_t)'\r';
	    cr_nl[1] = (wchar_t)'\n';
	    cr_nl[2] = (wchar_t)'\0';
	    start_from = textsw_find_mark(textsw, termsw->user_mark);
	    if (start_from == (length - 1)) {
		*buf = (wchar_t)'\0';
	    } else {
		(void) xv_get(textsw, TEXTSW_CONTENTS_WCS,
			      start_from, buf,
			  (buflen = MIN(BUFSIZE, length - 1 - start_from)));
	    }
	    termsw->pty_owes_newline = 0;
	    termsw->cmd_started = 0;
	    ttysw_move_mark(textsw, &termsw->pty_mark, length,
			    TEXTSW_MARK_DEFAULTS);
	    if (termsw->append_only_log) {
		ttysw_move_mark(textsw, &termsw->read_only_mark, length,
				TEXTSW_MARK_READ_ONLY);
	    }
	    ttysw_output_it(ttysw_view, cr_nl, 2);   /* This is only wchar */
	    if (buflen > 0)
		ttysw_input_it_wcs(ttysw, buf, buflen);
	} else if ( mbs_len == 1 &&
		last_inserted_mbs[0] == termsw->ltchars.t_lnextc) {
	    termsw->literal_next = TRUE;
	} else if ( mbs_len == 1 &&
		   ( last_inserted_mbs[0] == termsw->tchars.t_brkc
		   || last_inserted_mbs[0] == (wchar_t)'\n'
		   || last_inserted_mbs[0] == (wchar_t)'\r' )) {
	    (void) ttysw_scan_for_completed_commands(ttysw_view, -1, 0);
	}
    }
}

#else	/* OW_I18N */
/* ARGSUSED */
static void
ttysw_textsw_changed_handler(textsw, insert_before, length_before,
			     replaced_from, replaced_to, count_inserted)
    Textsw          textsw;
    int             insert_before;
    int             length_before;
    int             replaced_from;
    int             replaced_to;
    int             count_inserted;
{
    CHAR            last_inserted;
    Termsw_view_handle view = TERMSW_VIEW_PRIVATE_FROM_TEXTSW(textsw);
    Termsw_folio    termsw = TERMSW_FOLIO_FOR_VIEW(view);
    Ttysw_folio     ttysw = TTY_FROM_TERMSW(termsw->public_self);
    Ttysw_view_handle ttysw_view = TTY_VIEW_PRIVATE_FROM_TERMSW_VIEW(TERMSW_VIEW_PUBLIC(view));

    if (insert_before != length_before)
	return;
    if (termsw->cmd_started == 0) {
	if (termsw->cmd_started = (count_inserted > 0)) {
	    (void) textsw_checkpoint_undo(textsw, termsw->next_undo_point);
	    ttysw_move_mark(textsw, &termsw->user_mark,
			    (Textsw_index) length_before,
			    TEXTSW_MARK_DEFAULTS);
	}
    }
    if (!termsw->cmd_started)
	termsw->next_undo_point =
	    (caddr_t) textsw_checkpoint_undo(textsw,
					     (caddr_t) TEXTSW_INFINITY);
    if (count_inserted >= 1) {
	/* Get the last inserted character. */
	(void) xv_get(textsw, TEXTSW_CONTENTS,
		      replaced_from + count_inserted - 1,
		      &last_inserted, 1);
	if (last_inserted == termsw->ltchars.t_rprntc) {

#ifndef	BUFSIZE
#define	BUFSIZE 1024
#endif				/* BUFSIZE */
	    CHAR            buf[BUFSIZE + 1];
	    CHAR            cr_nl[3];
	    int             buflen = 0;
	    Textsw_index    start_from;
	    Textsw_index    length =
	    (int) xv_get(textsw, TEXTSW_LENGTH);

	    cr_nl[0] = '\r';
	    cr_nl[1] = '\n';
	    cr_nl[2] = '\0';
	    start_from = textsw_find_mark(textsw, termsw->user_mark);
	    if (start_from == (length - 1)) {
		*buf = '\0';
	    } else {
		(void) xv_get(textsw, TEXTSW_CONTENTS,
			      start_from, buf,
			  (buflen = MIN(BUFSIZE, length - 1 - start_from)));
	    }
	    termsw->pty_owes_newline = 0;
	    termsw->cmd_started = 0;
	    ttysw_move_mark(textsw, &termsw->pty_mark, length,
			    TEXTSW_MARK_DEFAULTS);
	    if (termsw->append_only_log) {
		ttysw_move_mark(textsw, &termsw->read_only_mark, length,
				TEXTSW_MARK_READ_ONLY);
	    }
	    ttysw_output_it(ttysw_view, cr_nl, 2);
	    if (buflen > 0)
		ttysw_input_it(ttysw, buf, buflen);
	} else if (last_inserted == termsw->ltchars.t_lnextc) {
	    termsw->literal_next = TRUE;
	} else if (last_inserted == termsw->tchars.t_brkc
		   || last_inserted == '\n'
		   || last_inserted == '\r') {
	    (void) ttysw_scan_for_completed_commands(ttysw_view, -1, 0);
	}
    }
}
#endif  /* OW_I18N */

extern int
ttysw_textsw_changed(textsw, attributes)
    Textsw          textsw;
    Attr_avlist     attributes;
{
    register Attr_avlist attrs;
    int             do_default = 0;
    register Termsw_view_handle view = TERMSW_VIEW_PRIVATE_FROM_TEXTSW(textsw);
    register Termsw_folio termsw = TERMSW_FOLIO_FOR_VIEW(view);
    register Ttysw_folio ttysw = TTY_FROM_TERMSW(termsw->public_self);

    for (attrs = attributes; *attrs; attrs = attr_next(attrs)) {
	switch ((Textsw_action) (*attrs)) {
	  case TEXTSW_ACTION_CAPS_LOCK:
	    ttysw->ttysw_capslocked = (attrs[1]) ? TTYSW_CAPSLOCKED : 0;
	    (void) ttysw_display_capslock(ttysw);
	    break;
	  case TEXTSW_ACTION_REPLACED:
	    if (!termsw->doing_pty_insert)
		ttysw_textsw_changed_handler(textsw,
			     (int) attrs[1], (int) attrs[2], (int) attrs[3],
					     (int) attrs[4], (int) attrs[5]);
	    break;
	  case TEXTSW_ACTION_LOADED_FILE:{
		Textsw_index    insert;
		Textsw_index    length;

		insert =
		    (Textsw_index) xv_get(textsw, TEXTSW_INSERTION_POINT);
#ifdef OW_I18N		    
		length = (Textsw_index) xv_get(textsw, TEXTSW_LENGTH_WC);
#else
		length = (Textsw_index) xv_get(textsw, TEXTSW_LENGTH);		
#endif		
		if (length == insert + 1) {
		    (void) xv_set(textsw, TEXTSW_INSERTION_POINT, length, 0);
		    ttysw_reset_column(ttysw);
		} else if (length == 0) {
		    ttysw_reset_column(ttysw);
		}
		if (length < textsw_find_mark(textsw, termsw->pty_mark)) {
		    ttysw_move_mark(textsw, &termsw->pty_mark, length,
				    TEXTSW_MARK_DEFAULTS);
		}
		if (termsw->append_only_log) {
		    ttysw_move_mark(textsw, &termsw->read_only_mark,
				    length, TEXTSW_MARK_READ_ONLY);
		}
		termsw->cmd_started = FALSE;
		termsw->pty_owes_newline = 0;
	    }
	  default:
	    do_default = TRUE;
	    break;
	}
    }
    if (do_default) {
	(void) textsw_default_notify(textsw, attributes);
    }
}
