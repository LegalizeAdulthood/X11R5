#ifndef lint
#ifdef sccs
static char     sccsid[] = "%Z%%M% %I% %E%";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Very active terminal emulator subwindow pty code.
 */

#include <errno.h>
#include <signal.h>
#ifdef SVR4
#include <sys/sigaction.h>
#include <sys/termios.h>
#endif SVR4
#include <stdio.h>
#include <sgtty.h>
#include <ctype.h>

#include <xview/win_struct.h>
#include <xview/win_input.h>
#include <xview/cursor.h>
#include <xview/pixwin.h>
#include <xview/icon.h>
#include <xview/ttysw.h>
#include <xview/notice.h>
#include <xview/frame.h>

#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <pixrect/pixfont.h>
#define _NOTIFY_MIN_SYMBOLS
#include <xview/notify.h>
#undef _NOTIFY_MIN_SYMBOLS
#include <xview_private/term_impl.h>
#include <xview/sel_svc.h>

#ifdef OW_I18N
#include <widec.h>
#include <wctype.h>
#include <xctype.h>
#include <stdlib.h>
#include <xview_private/xv_i18n_impl.h>
#include <xview_private/draw_impl.h>

#include <xview_private/charimage.h>
#include <X11/Xlib.h>
#include <X11/XSunExt.h>
#endif OW_I18N

#ifdef OW_I18N
/*
 * If there are committed chars and pre_edit chars returned by XIM,
 * we should display the echo'ed committed chars before display pre_edit chars.
 * Thus, tty callback will not display pre_edit chars until all committed chars
 * are echoed.
 */
int	committed_left = 0;
#endif
extern int      errno;
extern void     ttysw_lighten_cursor();
extern void     ttysetselection();
extern Textsw_index textsw_insert();

/*
 * jcb	-- remove continual cursor repaint in shelltool windows also known to
 * ttyansi.c
 */

extern int      do_cursor_draw;
extern int      tty_new_cursor_row, tty_new_cursor_col;


/* shorthand */
#define	iwbp	ttysw->ttysw_ibuf.cb_wbp
#define	irbp	ttysw->ttysw_ibuf.cb_rbp
#define	iebp	ttysw->ttysw_ibuf.cb_ebp
#define	ibuf	ttysw->ttysw_ibuf.cb_buf
#define	owbp	ttysw->ttysw_obuf.cb_wbp
#define	orbp	ttysw->ttysw_obuf.cb_rbp
#define	oebp	ttysw->ttysw_obuf.cb_ebp
#define	obuf	ttysw->ttysw_obuf.cb_buf

/* #ifdef TERMSW */
/*
 * The basic strategy for building a line-oriented command subwindow
 * (terminal emulator subwindow) on top of the text subwindow is as follows.
 * 
 * The idle state has no user input still to be processed, and no outstanding
 * active processes at the other end of the pty (except the shell).
 * 
 * When the user starts creating input events, they are passed through to the
 * textsw unless they fall in the class of "activating events" (which right
 * now consists of \n, \r, \033, ^C and ^Z).  In addition, the end of the
 * textsw's backing store is recorded when the first event is created.
 * 
 * When an activating event is entered, all of the characters in the textsw from
 * the recorded former end to the current end of the backing store are added
 * to the list of characters to be sent to the pty.  In addition, the current
 * end is set to be the insertion place for response from the pty.
 * 
 * If the user has started to enter a command, then in order to avoid messes on
 * the display, the first response from the pty will be suffixed with a \n
 * (unless it ends in a \n), and the pty will be marked as "owing" a \n.
 * 
 * In the meantime, if the user continues to create input events, they are
 * appended at the end of the textsw, after the response from the pty.  When
 * an activating event is entered, all of the markers, etc. are updated as
 * described above.
 * 
 * The most general situation is: Old stuff in the log file ^User editing here
 * More old stuff Completed commands enqueued for the pty Pty inserting here^
 * (Prompt)Partial user command
 */

/* #endif TERMSW */


/*
 * Main pty processing.
 */

/*
 * Write window input to pty
 */

#ifdef	OW_I18N
ttysw_pty_output(ttysw, pty)
    register Ttysw_folio ttysw;
    int             pty;
{
	register int    cc;
	char			* mb_irbp;	/* multi-byte copy of irbp */
	int			mb_nbytes;	/* number of bytes converted */
	CHAR			save_char;

    if (ttysw_getopt((caddr_t) ttysw, TTYOPT_TEXT)) {
	Termsw_folio    termsw = 
		TERMSW_FOLIO_FOR_VIEW(TERMSW_VIEW_PRIVATE_FROM_TTY_PRIVATE(ttysw));

	if (termsw->pty_eot > -1) {
	    CHAR           *eot_bp = ibuf + termsw->pty_eot;
	    CHAR            nullc = (CHAR)'\0';

	    /* write everything up to pty_eot */
	    if (eot_bp > irbp) {
		/* convert CHAR to char and calls write() */
		if (!(mb_irbp = malloc((eot_bp - irbp +1 )*MB_CUR_MAX))) {
		    perror(XV_I18N_MSG("xv_messages", "TTYSW:ttysw_pty_output: out of memory"));
	  	    return;
		}
		save_char = *eot_bp;
		*eot_bp   = nullc;
		mb_nbytes = wcstombs_with_null(mb_irbp, irbp, eot_bp - irbp);
		*eot_bp   = save_char;
		if( mb_nbytes == -1 ) {
		    perror(XV_I18N_MSG("xv_messages", "TTYSW:ttysw_pty_output: invalid wide character"));
		    free(mb_irbp);
		    return;
		}
		cc = ttysw_mbs_write(pty, mb_irbp, mb_nbytes);
		free(mb_irbp);
		if (cc > 0) {
		    irbp += cc;
		} else if (cc < 0) {
		    perror(XV_I18N_MSG("xv_messages", "TTYSW pty write failure"));
		}
	    }
	    /* write the eot */
	    if (irbp == eot_bp) {
		cc = write(pty, &nullc, 0);
		if (cc < 0) {
		    perror(XV_I18N_MSG("xv_messages", "TTYSW pty write failure"));
		} else {
		    termsw->pty_eot = -1;
		}
	    }
	}			/* termsw->pty_eot > -1 */
	/* only write the rest of the buffer if it doesn't have an eot in it */
	if (termsw->pty_eot > -1)
	    return;
    }
    if (iwbp > irbp) {
		/* convert CHAR to char and call write() */
		if (!(mb_irbp = malloc((iwbp - irbp + 1)*MB_CUR_MAX))) {
		    perror(XV_I18N_MSG("xv_messages", "TTYSW:ttysw_pty_output: out of memory"));
		    return;
		}
		save_char = *iwbp;
		*iwbp = '\0';
		mb_nbytes = wcstombs_with_null(mb_irbp, irbp, iwbp - irbp );
		*iwbp = save_char;
		if( mb_nbytes == -1 ) {
		    perror(XV_I18N_MSG("xv_messages", "TTYSW:ttysw_pty_output: invalid wide character"));
		    free(mb_irbp);
		    return;
		}
		cc = ttysw_mbs_write(pty, mb_irbp, mb_nbytes);
		free(mb_irbp);
		if (cc > 0) {
		    irbp += cc;
		    if (irbp == iwbp)
				irbp = iwbp = ibuf;
		} else if (cc < 0) {
		    perror(XV_I18N_MSG("xv_messages", "TTYSW pty write failure"));
		}
    }
}

/*
 *    write() sysstem call doesnot gurantee all data you have passed
 *    are acually written. This kind of paranoia occurs when you are
 *    writing to a channel to another process such as pipe.
 *    This function takes care that some data may remain unwritten and also,
 *    the boundary is within one multibyte character.
 */
ttysw_mbs_write(pty, mbs, nbytes)
    int		pty;
    char	*mbs;
    int		nbytes;
{
    int		nchars = 0;
    int		write_flag = 0;
    int		charlen,len;
    register char *tmp = mbs ;
    static int	nbytes_left;
    static char mbs_left[10];
    
    if( nbytes_left > 0 ) {
	len = write( pty, mbs_left , nbytes_left );
	if( len < 0 )
		return -1;
	else if( len == nbytes_left ) {
		nbytes_left = 0;
		write_flag = 1;
	}
	else { 
		nbytes_left -= len; 
		bcopy( &mbs_left[len] , mbs_left , nbytes_left );
		return 0;
	}
    }

    len = write( pty, mbs , nbytes );
    if( len < 0 )
	return  (write_flag == 1)? 0 : -1;

    while( len > 0 ) {
	charlen = euclen( tmp );
	tmp += charlen;
	len -= charlen;
	nchars++;
    }
    if( len < 0 ) {
	tmp += len;
	bcopy( tmp , mbs_left , -len );
	nbytes_left = -len;
    }

    return nchars;
}
#else 	/* OW_I18N */

ttysw_pty_output(ttysw, pty)
    register Ttysw_folio ttysw;
    int             pty;
{
    register int    cc;

    if (ttysw_getopt((caddr_t) ttysw, TTYOPT_TEXT)) {
	Termsw_folio    termsw = TERMSW_FOLIO_FOR_VIEW(TERMSW_VIEW_PRIVATE_FROM_TTY_PRIVATE(ttysw));

	if (termsw->pty_eot > -1) {
	    char           *eot_bp = ibuf + termsw->pty_eot;
	    char            nullc = '\0';

	    /* write everything up to pty_eot */
	    if (eot_bp > irbp) {
		cc = write(pty, irbp, eot_bp - irbp);
		if (cc > 0) {
		    irbp += cc;
		} else if (cc < 0) {
		    perror("TTYSW pty write failure");
		}
	    }
	    /* write the eot */
	    if (irbp == eot_bp) {
		cc = write(pty, &nullc, 0);
		if (cc < 0) {
		    perror("TTYSW pty write failure");
		} else {
		    termsw->pty_eot = -1;
		}
	    }
	}			/* termsw->pty_eot > -1 */
	/* only write the rest of the buffer if it doesn't have an eot in it */
	if (termsw->pty_eot > -1)
	    return;
    }
    if (iwbp > irbp) {
	cc = write(pty, irbp, iwbp - irbp);
	if (cc > 0) {
	    irbp += cc;
	    if (irbp == iwbp)
		irbp = iwbp = ibuf;
	} else if (cc < 0) {
	    perror("TTYSW pty write failure");
	}
    }
}
#endif  /* OW_I18N */

void
ttysw_process_STI(ttysw, cp, cc)
    register Ttysw_folio ttysw;
    register char  *cp;
    register int    cc;
{
    register short  post_id;
    register Textsw textsw;
    register Termsw_folio termsw;
    Textsw_index    pty_index;
    Textsw_index    cmd_start;

#ifdef DEBUG
    fprintf(stderr, "STI ");
    fwrite(cp, 1, cc, stderr);
    fprintf(stderr, "\n");
#endif				/* DEBUG */
    if (!TTY_IS_TERMSW(ttysw)) {
	return;
    }
    textsw = TEXTSW_FROM_TTY(ttysw);
    termsw = TERMSW_FOLIO_FOR_VIEW(TERMSW_VIEW_PRIVATE_FROM_TEXTSW(textsw));
    /* Assume app wants STI text echoed at cursor position */
    if (termsw->cooked_echo) {
	pty_index = textsw_find_mark(textsw, termsw->pty_mark);
	if (termsw->cmd_started) {
	    cmd_start = textsw_find_mark(textsw, termsw->user_mark);
	} else {
#ifdef OW_I18N	
	    cmd_start = (Textsw_index) xv_get(textsw, TEXTSW_LENGTH_WC);
#else
	    cmd_start = (Textsw_index) window_get(textsw, TEXTSW_LENGTH);
#endif	    
	}
	if (cmd_start > pty_index) {
	    if (termsw->append_only_log) {
		textsw_remove_mark(textsw, termsw->read_only_mark);
	    }
	    (void) textsw_delete(textsw, pty_index, cmd_start);
	    if (termsw->append_only_log) {
		termsw->read_only_mark =
		    textsw_add_mark(textsw, pty_index, TEXTSW_MARK_READ_ONLY);
	    }
	    termsw->pty_owes_newline = 0;
	}
    }
    /* Pretend STI text came in from textsw window fd */
    while (cc > 0) {
	post_id = (short) (*cp);
	win_post_id(textsw, post_id, NOTIFY_SAFE);
	cp++;
	cc--;
    }
    /* flush caches */
#ifdef OW_I18N	    
    (void) xv_get(textsw, TEXTSW_LENGTH_WC);
#else
    (void) window_get(textsw, TEXTSW_LENGTH);
#endif    
}

/*
 * Read pty's input (which is output from program)
 */
ttysw_pty_input(ttysw, pty)
    register Ttysw_folio ttysw;
    int             pty;
{
    static struct iovec iov[2];
    register int    cc;
    char            ucntl;
    register unsigned int_ucntl;
    register Termsw_folio termsw;
#ifdef OW_I18N
#define MB_BUF_SIZE	12
	register int	wc_nchar;	/* number of wide characters converted */
	unsigned char			* mb_buf_p;
	unsigned char			mb_buf[MB_BUF_SIZE]; 
	static unsigned char		rest_of_char[MB_BUF_SIZE];
	static int			rest_of_nchar = 0;

    /* readv to a buffer and convert it to CHAR */
    /* readv avoids need to shift packet header out of owbp. */
    iov[0].iov_base = &ucntl;
    iov[0].iov_len = 1;
    iov[1].iov_len = oebp - owbp;
#else
    iov[0].iov_base = &ucntl;
    iov[0].iov_len = 1;
    iov[1].iov_base = owbp;
    iov[1].iov_len = oebp - owbp;
#endif OW_I18N

#ifdef OW_I18N
	/*
	** This is terribly inefficient and must be replaced
	** before Beta
	** Allocate to worst-case: ascii
	*/
	if (!(mb_buf_p =
			(unsigned char *)malloc((iov[1].iov_len * sizeof(CHAR)) + sizeof(rest_of_char))))
		{
		perror(XV_I18N_MSG("xv_messages", "TTYSW:ttysw_pty_input: out of memory"));
		return;
		}
    if (rest_of_nchar != 0)
	bcopy(rest_of_char, mb_buf_p, rest_of_nchar);
    iov[1].iov_base = (char *)mb_buf_p + rest_of_nchar; /* yuck */
#endif OW_I18N
	cc = readv(pty, iov, 2);
#ifdef OW_I18N
	mb_buf_p[cc - 1 + rest_of_nchar] = 0;
	{
		register unsigned char	*p;
		register wchar_t	*w;
		register int		i, j;

		wc_nchar = 0;
		i = cc - 1 + rest_of_nchar;
		rest_of_nchar = 0;
		for (p = mb_buf_p, w = owbp; i > 0; )
		{
			if( *p == '\0' ) {
				*w = (wchar_t)'\0';
				j = 1;
			}
			else {
				if ((j = mbtowc(w, p, MB_CUR_MAX)) < 0) {
					p++;
					i--;
					continue;
				}
			}
			p += j;
			i -= j;
			w++;
			wc_nchar++;
		}
		if (i >= MB_CUR_MAX)
		{
			perror(XV_I18N_MSG("xv_messages", "TTYSW:ttysw_pty_input: invalid multi-byte character"));
			if (mb_buf_p != mb_buf)
				free(mb_buf_p);
			return;
		}
		*w = 0;
		if (i != 0)
		{
			bcopy(p, rest_of_char, i);
			rest_of_nchar = i;
		}
	}
	if (mb_buf_p != mb_buf) {
		free(mb_buf_p);
	}
#endif OW_I18N
    if (cc < 0 && errno == EWOULDBLOCK)
		cc = 0;
    else if (cc <= 0)
		cc = -1;
    if (cc > 0) {
	int_ucntl = (unsigned) ucntl;
	if (int_ucntl != 0 && ttysw_getopt((caddr_t) ttysw, TTYOPT_TEXT)) {
	    unsigned        tiocsti = TIOCSTI;
	    if (int_ucntl == (tiocsti & 0xff)) {
		ttysw_process_STI(ttysw, owbp, cc - 1);
	    }
	    termsw = TERMSW_FOLIO_FOR_VIEW(TERMSW_VIEW_PRIVATE_FROM_TTY_PRIVATE(ttysw));
	    (void) ioctl(ttysw->ttysw_tty, TIOCGETC, &termsw->tchars);
	    (void) ioctl(ttysw->ttysw_tty, TIOCGLTC, &termsw->ltchars);
	    (void) ttysw_getp(TTY_VIEW_HANDLE_FROM_TTY_FOLIO(ttysw));	/* jcb for nng */
	} else {
#ifdef OW_I18N
		owbp += wc_nchar;
#undef	MB_BUF_SIZE
#else
		owbp += cc - 1;
#endif
	}
    }
}

/*
 * Send program output to terminal emulator.
 */
ttysw_consume_output(ttysw_view)
    Ttysw_view_handle ttysw_view;

{
    register Ttysw_folio ttysw = TTY_FOLIO_FROM_TTY_VIEW_HANDLE(ttysw_view);
    short           is_not_text;
    int             cc;

    /* cache the cursor removal and re-render once in this set -- jcb */
    if (is_not_text = !ttysw_getopt((caddr_t) ttysw, TTYOPT_TEXT)) {
	(void) ttysw_removeCursor();
	do_cursor_draw = FALSE;
    }
    while (owbp > orbp && !(ttysw->ttysw_flags & TTYSW_FL_FROZEN)) {
	if (is_not_text) {
	    if (ttysw->ttysw_primary.sel_made) {
		ttysel_deselect(&ttysw->ttysw_primary, SELN_PRIMARY);
	    }
	    if (ttysw->ttysw_secondary.sel_made) {
		ttysel_deselect(&ttysw->ttysw_secondary, SELN_SECONDARY);
	    }
	}
	cc = ttysw_output_it(ttysw_view, orbp, (owbp - orbp));
#ifdef OW_I18N
	/*
	 * Display the pre_edit region after committed string is echoed.
	 * In case of non-echo mode, we don't guarantee the echoing of
	 * the preedit region either.
	 */
	if (committed_left > 0) {
	    committed_left -= cc;
	    if (committed_left <= 0) {
		committed_left = 0;
	    }
	}
#endif 
	orbp += cc;
	if (orbp == owbp)
	    orbp = owbp = obuf;
    }

    if (is_not_text) {
	(void) ttysw_drawCursor(tty_new_cursor_row, tty_new_cursor_col);
	do_cursor_draw = TRUE;
#ifdef	OW_I18N
    	if (ttysw->im_store) {
       	    ttysw->im_first_col = curscol;
       	    ttysw->im_first_row = cursrow;
	    if (STRLEN(ttysw->im_store) > 0 )  
	        tty_preedit_put_wcs( ttysw, ttysw->im_store , ttysw->im_attr,
                         ttysw->im_first_col ,ttysw->im_first_row,
			 &(ttysw->im_len));
	}
#endif
    }
}

/*
 * Add the string to the input queue.
 */
ttysw_input_it(ttysw, addr, len)
    register Ttysw_folio ttysw;
    char           *addr;
    register int    len;
{
#ifdef OW_I18N
	CHAR		*wcs_buf;
	register CHAR	*wc;
	register int	wc_nchar;
	static int	rest_of_nchar = 0;
	register	char	*mbc;
	int		mb_len;
	register int	i;
	char		*save_char;
#define	MB_BUF_SIZE	24
	static char	rest_of_char[MB_BUF_SIZE];
#endif
    Event           event;
    Textsw          textsw;
#ifdef OW_I18N
	/* convert char to CHAR: allocate to worst case (ascii) */
	if (!(wcs_buf = (CHAR *)malloc((len+2)*sizeof(CHAR))))
		{
		perror(XV_I18N_MSG("xv_messages", "TTYSW:ttysw_input_it: out of memory"));
		return;
		}

	if (!(save_char = (char *)malloc(len+2)))
		{
		perror(XV_I18N_MSG("xv_messages", "TTYSW:ttysw_input_it: out of memory"));
		return;
		}
	strncpy(save_char, addr, len);
	save_char[len] = '\0';
	if( rest_of_nchar != 0 ) {
		bcopy( save_char , &rest_of_char[rest_of_nchar] , 
			MIN( len , MB_BUF_SIZE-rest_of_nchar ) );
		if( rest_of_char[0] == '\0' ) {
			mb_len = 1;
			*wcs_buf = (wchar_t)'\0';
		}
		else {
			mb_len = mbtowc( wcs_buf, rest_of_char, MB_CUR_MAX );
		}
		if( mb_len < 0 ) {
			mbc = save_char + 1 - rest_of_nchar;
			len -= (1 - rest_of_nchar);
		} else {
			mbc = save_char + mb_len - rest_of_nchar;
			wc = wcs_buf + 1;
			wc_nchar = 1;
			len -= (mb_len - rest_of_nchar);
		}
	}
	else {
		mbc = save_char;
		wc = wcs_buf;
		wc_nchar = 0;
	}

	for( i = 0 ; i < len ; ) {
		if( *mbc == '\0' ) {
			mb_len = 1;
			*wc = (wchar_t)'\0';
		}
		else {
			mb_len = mbtowc( wc, mbc, MB_CUR_MAX );
			if( mb_len < 0 ) {
				mbc++;
				i++;
				continue;
			}
		}
		wc ++;
		mbc += mb_len;
		i += mb_len;
		wc_nchar++;
	}

	if( i < len ) {
		if( len -i > MB_CUR_MAX ) {
			perror(XV_I18N_MSG("xv_messages", 
					"TTYSW:ttysw_input_it: invalid multi-byte character"));
			free( wcs_buf );
			return;
		}
		bcopy( mbc , rest_of_char , len - i );
		rest_of_char[len - i] = '\0';
		rest_of_nchar = len - i;
	}
	else
		rest_of_nchar = 0;
	free(save_char);
#endif

    if (ttysw_getopt((caddr_t) ttysw, TTYOPT_TEXT)) {
	textsw = TEXTSW_FROM_TTY(ttysw);
#ifdef OW_I18N
	(void) textsw_insert_wcs(textsw, wcs_buf, (long int) wc_nchar);
#else
	(void) textsw_insert(textsw, addr, (long int) len);
#endif OW_I18N
    } else {
	Ttysw_view_handle ttysw_view = TTY_VIEW_HANDLE_FROM_TTY_FOLIO(ttysw);
	if (iwbp + len >= iebp) {
	    /* Input buffer would overflow, so tell user and discard chars.  */
	    (void) notice_prompt(
				 xv_get(TTY_PUBLIC(ttysw), WIN_FRAME),
				 (Event *) 0,
				 NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
			  XV_I18N_MSG("xv_messages", "\
Too many characters to add to the input buffer.\n\
Wait a few seconds after you click Continue,\n\
then retype the missing characters."),
#else
			  "Too many characters to add to the input buffer.",
			     "Wait a few seconds after you click Continue,",
				 "then retype the missing characters.",
#endif
				 0,
				 NOTICE_BUTTON_YES,
#ifdef OW_I18N
				 XV_I18N_MSG("xv_messages", "Continue"),
#else
				 "Continue",
#endif
				 NOTICE_TRIGGER, ACTION_STOP,
				 0);
#ifdef OW_I18N
	    free(wcs_buf);
#endif
	    return (0);
	}
#ifdef OW_I18N
	bcopy(wcs_buf, iwbp, wc_nchar * sizeof(CHAR));
	free(wcs_buf);
	iwbp += wc_nchar;
#else
	bcopy(addr, iwbp, len);
	iwbp += len;
#endif
	ttysw->ttysw_lpp = 0;	/* reset page mode counter */
	if (ttysw->ttysw_flags & TTYSW_FL_FROZEN)
	    (void) ttysw_freeze(ttysw_view, 0);
	if (!(ttysw->ttysw_flags & TTYSW_FL_IN_PRIORITIZER)) {
	    (void) ttysw_reset_conditions(ttysw_view);
	}
    }
#ifdef OW_I18N
	return(wc_nchar);
#undef	MB_BUF_SIZE
#else
    return (len);
#endif OW_I18N
}

#ifdef OW_I18N
/*
 * Add the wide character string to the input queue.
 */
ttysw_input_it_wcs(ttysw, addr, len)
    register Ttysw_folio	ttysw;
    CHAR			*addr;
    register int		len;
{

    Event           event;
    Textsw          textsw;
    if (ttysw_getopt((caddr_t) ttysw, TTYOPT_TEXT)) {
	textsw = TEXTSW_FROM_TTY(ttysw);
	(void) textsw_insert_wcs(textsw, addr, (long int) len);
    } else {
	Ttysw_view_handle ttysw_view = TTY_VIEW_HANDLE_FROM_TTY_FOLIO(ttysw);
	if (iwbp + len >= iebp) {
	    /* Input buffer would overflow, so tell user and discard chars.  */
	    (void) notice_prompt(
				 xv_get(TTY_PUBLIC(ttysw), WIN_FRAME),
				 (Event *) 0,
				 NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
			  XV_I18N_MSG("xv_messages", "\
Too many characters to add to the input buffer.\n\
Wait a few seconds after you click Continue,\n\
then retype the missing characters."),
#else
			  "Too many characters to add to the input buffer.",
			     "Wait a few seconds after you click Continue,",
				 "then retype the missing characters.",
#endif
				 0,
				 NOTICE_BUTTON_YES,
#ifdef OW_I18N
				 XV_I18N_MSG("xv_messages", "Continue"),
#else
				 "Continue",
#endif
				 NOTICE_TRIGGER, ACTION_STOP,
				 0);
	    return (0);
	}
	bcopy(addr, iwbp, len*sizeof(CHAR));
	iwbp += len;
	ttysw->ttysw_lpp = 0;	/* reset page mode counter */
	if (ttysw->ttysw_flags & TTYSW_FL_FROZEN)
	    (void) ttysw_freeze(ttysw_view, 0);
	if (!(ttysw->ttysw_flags & TTYSW_FL_IN_PRIORITIZER)) {
	    (void) ttysw_reset_conditions(ttysw_view);
	}
    }
    return (len);
}
#endif OW_I18N

/* #ifndef TERMSW */
ttysw_handle_itimer(ttysw)
    register Ttysw_folio ttysw;
{
    if (ttysw->ttysw_primary.sel_made) {
	ttysel_deselect(&ttysw->ttysw_primary, SELN_PRIMARY);
    }
    if (ttysw->ttysw_secondary.sel_made) {
	ttysel_deselect(&ttysw->ttysw_secondary, SELN_SECONDARY);
    }
    (void) ttysw_pdisplayscreen(0);
}

/*
 * handle standard events.
 */
Xv_public int
ttysw_eventstd(ttysw_view_public, ie)
/* This could be a public ttysw view or termsw view */
    register Tty_view ttysw_view_public;
    register struct inputevent *ie;
{
    Frame           frame_public;
    register Ttysw_folio ttysw = TTY_PRIVATE_FROM_ANY_VIEW(ttysw_view_public);
    register Tty    tty_public = TTY_PUBLIC(ttysw);

    switch (event_action(ie)) {
      case KBD_USE:
      case KBD_DONE:
	frame_public = (Frame) xv_get(tty_public, WIN_OWNER);
	switch (event_action(ie)) {
	  case KBD_USE:
#ifdef	OW_I18N
	  if (xv_get(tty_public, WIN_USE_IM)) {
	        Xv_Drawable_info *info;
        	
        	DRAWABLE_INFO_MACRO(ttysw_view_public, info);
	        XSetICValues(ttysw->ic, XNFocusWindow, xv_xid(info), NULL);	  
		XSetICFocus(ttysw->ic);
	  }
#endif
	    (void) ttysw_restore_cursor();
	    (void) frame_kbd_use(frame_public, tty_public);
	    return TTY_DONE;
	  case KBD_DONE:
	    ttysw_lighten_cursor();
	    (void) frame_kbd_done(frame_public, tty_public);
	    return TTY_DONE;
	}
      case WIN_REPAINT:
      case WIN_GRAPHICS_EXPOSE:
	if (TTY_IS_TERMSW(ttysw)) {
	    Termsw_view_handle termsw = TERMSW_VIEW_PRIVATE_FROM_TTY_PRIVATE(ttysw);

	    if (termsw->folio->cmd_started) {
		(void) ttysw_scan_for_completed_commands(
		    TTY_VIEW_PRIVATE_FROM_ANY_PUBLIC(ttysw_view_public), -1, 0);
	    }
	}
	(void) ttysw_display(ttysw);
#ifdef	OW_I18N
	if(ttysw->preedit_state  && (STRLEN(ttysw->im_store) > 0)) {
		tty_preedit_put_wcs( ttysw, ttysw->im_store , ttysw->im_attr,
			ttysw->im_first_col,ttysw->im_first_row, &(ttysw->im_len) );
	}
#endif
	return (TTY_DONE);
      case WIN_RESIZE:
	(void) ttysw_resize(TTY_VIEW_PRIVATE_FROM_ANY_PUBLIC(ttysw_view_public));
#ifdef	OW_I18N
	if( ttysw->preedit_state ) {
		ttysw_preedit_resize_proc(ttysw);
	}
#endif
	return (TTY_DONE);
      case ACTION_SELECT:
	return ttysw_process_point(ttysw, ie);
      case ACTION_ADJUST:
	return ttysw_process_adjust(ttysw, ie);

      case ACTION_MENU:{
            if (event_is_down(ie)) {
	        ttysw_show_walkmenu(ttysw_view_public, ie);
	        ttysw->ttysw_butdown = ACTION_MENU;
	    }

	    return (TTY_DONE);
	}

#ifdef notdef			/* BUG ALERT */
	/*
	 * 11 Sept 87: Alok found that if we do the exit processing, we turn
	 * off LOC_WINEXIT and thus defeat the auto-generation of KBD_DONE by
	 * xview_x_input_readevent.  Until we incorporate a fix in the lower input
	 * code we comment out this optimization.
	 */
      case LOC_WINEXIT:
	return ttysw_process_exit(ttysw, ie);
#endif
      case LOC_MOVEWHILEBUTDOWN:
	return ttysw_process_motion(ttysw, ie);
      default:
	return ttysw_process_keyboard(ttysw, ie);
    }
}

static int
ttysw_process_point(ttysw, ie)
    register Ttysw_folio ttysw;
    register struct inputevent *ie;
{
#ifdef OW_I18N      
    if (ttysw->ic) {
#ifdef POSTPUBLICREVIEW_BC    
        int		    *conv_on = NULL;
        if (!XGetICValues(ttysw->ic, XNExtXimp_Conversion, &conv_on, NULL)) {
            if (conv_on && *conv_on == XIMEnable) {
                XFree(conv_on);
	        return TTY_DONE;
	    }	        
	}
	if (conv_on)
	    XFree(conv_on);
#else /* POSTPUBLICREVIEW_BC */
        int		    conv_on;
        if (!XGetICValues(ttysw->ic, XNExtXimp_Conversion, &conv_on, NULL)) {
            if (conv_on == XIMEnable) {
	        return TTY_DONE;
	    }	        
	}
#endif	/* POSTPUBLICREVIEW_BC */    	                          
   }    
#endif

    if (win_inputposevent(ie)) {
	ttysw->ttysw_butdown = ACTION_SELECT;
	ttysel_make(ttysw, ie, 1);
    } else {
	if (ttysw->ttysw_butdown == ACTION_SELECT) {
	    (void) ttysel_adjust(ttysw, ie, FALSE, FALSE);
	    ttysetselection(ttysw);
	}
	/* ttysw->ttysw_butdown = 0; */
    }
    return TTY_DONE;
}

static int
ttysw_process_adjust(ttysw, ie)
    register Ttysw_folio ttysw;
    register struct inputevent *ie;
{
#ifdef OW_I18N      
    if (ttysw->ic) {
#ifdef POSTPUBLICREVIEW_BC    
        int		    *conv_on = NULL;
        if (!XGetICValues(ttysw->ic, XNExtXimp_Conversion, &conv_on, NULL)) {
            if (conv_on && *conv_on == XIMEnable) { 
                XFree(conv_on);
	        return TTY_DONE;
	    }	        
	}
	if (conv_on)
	    XFree(conv_on);
#else  /* POSTPUBLICREVIEW_BC */
        int		    conv_on;
        if (!XGetICValues(ttysw->ic, XNExtXimp_Conversion, &conv_on, NULL)) {
            if (conv_on == XIMEnable) { 
	        return TTY_DONE;
	    }	        
	}
#endif /* POSTPUBLICREVIEW_BC */	    	                          
   }    
#endif

    if (win_inputposevent(ie)) {
	(void) ttysel_adjust(ttysw, ie, TRUE,
			     (ttysw->ttysw_butdown == ACTION_ADJUST));
	/* Very important for this to be set after the call to ttysel_adjust */
	ttysw->ttysw_butdown = ACTION_ADJUST;
    } else {
	if (ttysw->ttysw_butdown == ACTION_ADJUST) {
	    (void) ttysel_adjust(ttysw, ie, FALSE, FALSE);
	    ttysetselection(ttysw);
	}
	/* ttysw->ttysw_butdown = 0; */
    }
    return TTY_DONE;
}

static int
ttysw_process_motion(ttysw, ie)
    register Ttysw_folio ttysw;
    register struct inputevent *ie;
{
#ifdef OW_I18N      
    if (ttysw->ic) {
#ifdef POSTPUBLICREVIEW_BC        
        int		    *conv_on = NULL;
        if (!XGetICValues(ttysw->ic, XNExtXimp_Conversion, &conv_on, NULL)) {
            if (conv_on && *conv_on == XIMEnable) {
                XFree(conv_on);
	        return TTY_DONE;
	    }	        
	}
	if (conv_on)
	    XFree(conv_on);
#else /* POSTPUBLICREVIEW_BC */
        int		    conv_on;
        if (!XGetICValues(ttysw->ic, XNExtXimp_Conversion, &conv_on, NULL)) {
            if (conv_on == XIMEnable) {
	        return TTY_DONE;
	    }	        
	}
#endif /* POSTPUBLICREVIEW_BC */	    	                          
   }    
#endif

    if ((ttysw->ttysw_butdown == ACTION_SELECT) ||
	(ttysw->ttysw_butdown == ACTION_ADJUST))
	(void) ttysel_adjust(ttysw, ie, FALSE, FALSE);
    return TTY_DONE;
}

/* ARGSUSED */
static int
ttysw_process_exit(ttysw, ie)
    Ttysw_folio     ttysw;
    struct inputevent *ie;
{
    register Xv_object window = (Xv_object) TTY_PUBLIC(ttysw);
    struct inputmask im;

    (void) win_getinputmask(window, &im, 0);
    win_unsetinputcodebit(&im, LOC_WINEXIT);
    (void) win_setinputmask(window, &im, 0, 0);
    return TTY_DONE;
}

static int
ttysw_process_keyboard(ttysw, ie)
    Ttysw_folio     ttysw;
    struct inputevent *ie;

{
    register int    id = event_id(ie);

    if (event_action(ie) == ACTION_HELP)
	return (ttysw_domap(ttysw, ie));

#ifdef	OW_I18N
    if( event_is_string(ie) && win_inputposevent(ie) ) {
	/*
	 *    This is a committed string or a string generated from a non-English
	 *    keyboard.  For example, a kana string from the Nihongo keyboard.
	 *    This string should only be displayed once on the down event.
	 */
	wchar_t	*ws_char;
	ws_char = mbstowcsdup(ie->ie_string);
	committed_left = wslen(ws_char);
        (void) ttysw_input_it_wcs(ttysw,ws_char,committed_left);
	free( ws_char );
	return TTY_DONE; 
    }
#endif

    if ((id >= ASCII_FIRST && id <= ISO_LAST) && (win_inputposevent(ie))) {
	char            c = (char) id;

	/*
	 * State machine for handling logical caps lock, ``F1'' key.
	 * Capitalize characters except when an ESC goes by.  Then go into a
	 * state where characters are passed uncapitalized until an
	 * alphabetic character is passed.  We presume that all ESC sequences
	 * end with an alphabetic character.
	 * 
	 * Used to solve the function key problem where the final `z' is is
	 * being capitalized. (Bug id: 1005033)
	 */
	if (ttysw->ttysw_capslocked & TTYSW_CAPSLOCKED) {
	    if (ttysw->ttysw_capslocked & TTYSW_CAPSSAWESC) {
		if (isalpha(c))
		    ttysw->ttysw_capslocked &= ~TTYSW_CAPSSAWESC;
	    } else {
		if (islower(c))
		    c = toupper(c);
		else if (c == '\033')
		    ttysw->ttysw_capslocked |= TTYSW_CAPSSAWESC;
	    }
	}
	(void) ttysw_input_it(ttysw, &c, 1);
#ifdef TTY_ACQUIRE_CARET
	if (!ttysw->ttysw_caret.sel_made) {
	    ttysel_acquire(ttysw, SELN_CARET);
	}
#endif
	return TTY_DONE;
    }
    if (id > ISO_LAST) {
	return ttysw_domap(ttysw, ie);
    }
    return TTY_OK;
}

/* #endif TERMSW */

/*
 * After the character array image changes size, this routine must be called
 * so that pty knows about the new size.
 */
xv_tty_new_size(ttysw, cols, lines)
    Ttysw_folio     ttysw;
    int             cols, lines;

{
#ifdef sun
    /*
     * The ttysize structure and TIOCSSIZE and TIOCGSIZE ioctls are available
     * only on Suns.
     */

    struct ttysize  ts;
#ifndef SVR4
    struct sigvec vec, ovec;
 
    vec.sv_handler = SIG_IGN;
    vec.sv_mask = vec.sv_onstack = 0;
    (void) sigvec(SIGTTOU, &vec, &ovec);
#endif SVR4
 
    ts.ts_lines = lines;
    ts.ts_cols = cols;
    if ((ioctl(ttysw->ttysw_tty, TIOCSSIZE, &ts)) == -1)
#ifdef OW_I18N
	perror(XV_I18N_MSG("xv_messages", "ttysw-TIOCSSIZE"));
#else
	perror("ttysw-TIOCSSIZE");
#endif

#ifndef SVR4
    (void) sigvec(SIGTTOU, &ovec, 0);
#endif SVR4
#else /* sun */
    /*
     * Otherwise, we use the winsize struct  and TIOCSWINSZ ioctl.
     */
    struct winsize  ws;
#ifndef SVR4
    struct sigvec vec, ovec;

    vec.sv_handler = SIG_IGN;
    vec.sv_mask = vec.sv_onstack = 0;
    (void) sigvec(SIGTTOU, &vec, &ovec);
#endif SVR4

    ws.ws_row = lines;
    ws.ws_col = cols;
    if ((ioctl(ttysw->ttysw_tty, TIOCSWINSZ, &ws)) == -1)
#ifdef OW_I18N
	perror(XV_I18N_MSG("xv_messages", "ttysw-TIOCSWINSZ"));
#else
	perror("ttysw-TIOCSWINSZ");
#endif

#ifndef SVR4
    (void) sigvec(SIGTTOU, &ovec, 0);
#endif SVR4
#endif /* sun */
}


/*
 * Freeze tty output.
 */
Pkg_private int
ttysw_freeze(ttysw_view, on)
    Ttysw_view_handle ttysw_view;
    int             on;
{
    register Ttysw_folio ttysw = ttysw_view->folio;
    register Tty_view ttysw_view_public = TTY_PUBLIC(ttysw_view);
    extern Xv_Cursor ttysw_cursor;
    extern Xv_Cursor ttysw_stop_cursor;

    if (!ttysw_cursor)
	ttysw_cursor = xv_get(ttysw_view_public, WIN_CURSOR);
    if (!(ttysw->ttysw_flags & TTYSW_FL_FROZEN) && on) {
	struct sgttyb   sgttyb;

	(void) ioctl(ttysw->ttysw_tty, TIOCGETP, (char *) &sgttyb);
#ifndef SVR4
	if ((sgttyb.sg_flags & (RAW | CBREAK)) == 0) {
#else SVR4
	if ((sgttyb.sg_flags & (RAW | O_CBREAK)) == 0) {
#endif SVR4
	    xv_set(ttysw_view_public,
		   WIN_CURSOR, ttysw_stop_cursor,
		   0);
	    ttysw->ttysw_flags |= TTYSW_FL_FROZEN;
	} else
	    ttysw->ttysw_lpp = 0;
    } else if ((ttysw->ttysw_flags & TTYSW_FL_FROZEN) && !on) {
	xv_set(ttysw_view_public,
	       WIN_CURSOR, ttysw_cursor,
	       0);
	ttysw->ttysw_flags &= ~TTYSW_FL_FROZEN;
	ttysw->ttysw_lpp = 0;
    }
    return ((ttysw->ttysw_flags & TTYSW_FL_FROZEN) != 0);
}

/*
 * Set (or reset) the specified option number.
 */
ttysw_setopt(ttysw_folio_or_view, opt, on)
    Xv_opaque       ttysw_folio_or_view;
    int             opt, on;
{
    Tty             folio_or_view_public = TTY_PUBLIC((Ttysw_folio) ttysw_folio_or_view);
    Ttysw_view_handle ttysw_view;
    Ttysw_folio     ttysw_folio;
    int             result = 0;

    if (IS_TTY_VIEW(folio_or_view_public) ||
	IS_TERMSW_VIEW(folio_or_view_public)) {
	ttysw_view = (Ttysw_view_handle) ttysw_folio_or_view;
	ttysw_folio = TTY_FOLIO_FROM_TTY_VIEW_HANDLE(ttysw_view);
    } else {
	ttysw_folio = (Ttysw_folio) ttysw_folio_or_view;
	ttysw_view = TTY_VIEW_HANDLE_FROM_TTY_FOLIO(ttysw_folio);
    }

    switch (opt) {
      case TTYOPT_TEXT:	/* termsw */
	if (on)
	    result = ttysw_be_termsw(ttysw_view);
	else
	    result = ttysw_be_ttysw(ttysw_view);
    }
    if (result != -1) {
	if (on)
	    ttysw_folio->ttysw_opt |= 1 << opt;
	else
	    ttysw_folio->ttysw_opt &= ~(1 << opt);
    }
}

ttysw_getopt(ttysw, opt)
    Ttysw_folio     ttysw;
    int             opt;
{
    return ((ttysw->ttysw_opt & (1 << opt)) != 0);
}

ttysw_flush_input(ttysw)
    Ttysw_folio     ttysw;
{
#ifndef SVR4
    struct sigvec   vec, ovec;	/* Sys V compatibility */
    int flushf = 0;

    vec.sv_handler = SIG_IGN;
    vec.sv_mask = vec.sv_onstack = 0;
    (void) sigvec(SIGTTOU, &vec, &ovec);
#else SVR4
    struct sigaction   vec, ovec;
    vec.sa_handler = SIG_IGN;
    sigemptyset(&vec.sa_mask);
    vec.sa_flags = 0;
    sigaction(SIGTTOU, &vec, &ovec);
#endif SVR4

    /* Flush tty input buffer */
#ifndef SVR4
    if (ioctl(ttysw->ttysw_tty, TIOCFLUSH, &flushf))
#else SVR4
    if (ioctl(ttysw->ttysw_tty, TIOCFLUSH, 0))
#endif SVR4
#ifdef OW_I18N
	perror(XV_I18N_MSG("xv_messages", "TIOCFLUSH"));
#else
	perror("TIOCFLUSH");
#endif

#ifndef SVR4
    (void) sigvec(SIGTTOU, &ovec, (struct sigvec *) 0);
#else SVR4
    sigaction(SIGTTOU, &ovec, (struct sigaction *) 0);
#endif SVR4

    /* Flush ttysw input pending buffer */
    irbp = iwbp = ibuf;
}

#ifdef	OW_I18N
/*
 *	This function converts a wide-character-string to a multibyte
 *	string without any regard to null-characters.
 */
wcstombs_with_null( mbs , wcs , nchar )
    char	*mbs;
    wchar_t	*wcs;
    register int	nchar;
{
    register char	*mbs_tmp = mbs;
    register wchar_t 	*wcs_tmp = wcs;
    register int	i,mb_len;

    for( i = 0 ; i < nchar ; i++ ) {
	if( *wcs_tmp == (wchar_t)'\0' ) {
		mb_len = 1;
		*mbs_tmp = '\0';
	}
	else {
		mb_len = wctomb( mbs_tmp, *wcs_tmp );
	}
	wcs_tmp++;
	mbs_tmp += mb_len;
    }

    return( (int)(mbs_tmp - mbs) );

}
#endif


