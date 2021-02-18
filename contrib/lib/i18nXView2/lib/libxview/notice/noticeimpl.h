/*      @(#)noticeimpl.h 50.3 90/10/16 SMI      */

/* ------------------------------------------------------------------ */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/* ------------------------------------------------------------------ */

#include <locale.h>
#ifdef	OW_I18N
#include <xview/xv_i18n.h>
#endif
#include <xview/notice.h>
#include <xview/svrimage.h>

#define NOTICE_HELP		(NOTICE_TRIGGERED-1)
#define NOTICE_ACTION_DO_IT	'\015'

/* ------------------------------------------------------------------ */
/* -------------- opaque types and useful typedefs  ----------------- */
/* ------------------------------------------------------------------ */

struct notice {
    Xv_Window		client_window;
    Fullscreen		fullscreen;
    Xv_object		fullscreen_window;
    int			result;
    int			default_input_code;
    Event		*event;
    Xv_Font		notice_font;

    int			beeps;
    int			dont_beep;
    
    int			focus_x;
    int			focus_y;
    int			focus_specified;
    CHAR		**message_items;
    int			number_of_buttons;
    struct notice_buttons *button_info;
    int			yes_button_exists;
    int			no_button_exists;
    char 		*help_data;
    Event		help_event;
#ifdef	OW_I18N
    wchar_t		*text_string;
    wchar_t		**text_ptrs;
    int			text_nchars;
    int			text_nstrings;
#endif
};

typedef struct notice	*notice_handle;

struct notice_buttons {
    CHAR			*string;
    int				 value;
    int				 is_yes;
    int				 is_no;
    struct rect			 button_rect;
    struct notice_buttons	*next;
};

typedef struct notice_buttons	*notice_buttons_handle;
