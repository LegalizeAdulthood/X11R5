/*      @(#)windowimpl.h 70.3 91/08/02 SMI      */

/***********************************************************************/
/*	                      window_impl.h			       */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/***********************************************************************/

#ifndef window_impl_DEFINED
#define window_impl_DEFINED

#include <sys/types.h>
#include <sys/time.h>

#include <xview/font.h>
#include <xview/pkg.h>
#define	_NOTIFY_MIN_SYMBOLS
#include <xview/notify.h>
#undef	_NOTIFY_MIN_SYMBOLS
#include <xview/rect.h>
#include <xview/rectlist.h>

#ifndef	pixrect_hs_DEFINED
#define	pixrect_hs_DEFINED
/* <pixrect/pixrect_hs.h> without frame buffer variable include files */
#include <pixrect/pixrect.h>
#include <pixrect/pr_dblbuf.h>
#include <pixrect/pr_line.h>
#include <pixrect/pr_planegroups.h>
#include <pixrect/pr_util.h>
#include <pixrect/traprop.h>
#include <pixrect/memvar.h>
#include <pixrect/pixfont.h>
#include <rasterfile.h>
#include <pixrect/pr_io.h>
#endif	pixrect_hs_DEFINED

/* BUG: try to get rid of win_struct.h */
#include <xview/win_struct.h>
#include <xview/win_input.h>
#include <xview/window.h>
#include <xview/cursor.h>
#include <xview_private/draw_impl.h>
/* #include <xview_private/win_keymap.h> */   

#ifdef OW_I18N
#include <X11/XSunExt.h>
#endif

#define window_attr_next(attr) (Window_attribute *)attr_next((caddr_t *)attr)

/* 
   MAX_FUNC_KEY is maximum number of function keys + number of buttons in the
   mouse.
*/
#define MAX_KEYCODE		128
#define BITS_PER_BYTE		8
#define WINDOW_KEYMASK		(MAX_KEYCODE/((sizeof(char))* BITS_PER_BYTE))

#define DEFAULT_X_Y		0
#define DEFAULT_WIDTH_HEIGHT	64

/* For the rect of the window */
#define         EMPTY_VALUE                     0x7fff


#define WIN_DEFAULT_RECT(_rect) (((_rect)->r_left == DEFAULT_X_Y) && \
				 ((_rect)->r_top == DEFAULT_X_Y) && \
				 ((_rect)->r_width == DEFAULT_WIDTH_HEIGHT) && \
				 ((_rect)->r_height == DEFAULT_WIDTH_HEIGHT)) 

#define	WIN_PRIVATE(win)	XV_PRIVATE(Window_info, Xv_window_struct, win)
#define	WIN_PUBLIC(win)		XV_PUBLIC(win)

#define WIN_SET_DEAF(_win_info, flag) (_win_info->deaf = flag)
#define WIN_IS_DEAF(_win_info) (_win_info->deaf)

/* windows are in charge of their own borders */

/***********************************************************************/
/*	        	Structures 				       */
/***********************************************************************/
typedef struct window_client_msg {
    Xv_opaque		type;
    unsigned char	format;
    union {
    	char		b[WIN_MESSAGE_DATA_SIZE];
	short		s[WIN_MESSAGE_DATA_SIZE/sizeof(short)];
	int		l[WIN_MESSAGE_DATA_SIZE/sizeof(int)];
	} data;
} Window_client_msg;

typedef struct window_info {
    Xv_Window		 public_self;	/* back pointer to public struct */
    int			(*layout_proc)();
    void                (*event_proc)();
    struct window_info	*owner;
    Xv_Window		 parent;
    Xv_opaque		 menu;
    Pixfont		*font;
    Xv_Font		glyph_font; /*OPEN LOOK glyph font for window's scale*/
    int			 desired_width;
    int			 desired_height;
    Xv_Cursor		 cursor;
    Xv_Cursor		 normal_cursor; /* place to store normal cursor when switch */
					/* to busy cursor */
    Xv_opaque		 client_data; 
    Rect		 cache_rect;
    unsigned int	 xmask;   	/* X input mask */
    int	 		 scale;
    char		*cmdline;
    Pixmap		 background_pixmap;
    int			 visual_class;

    /* Client Message Info */
    Window_client_msg	 client_message;

    /* margin info */
    short		 top_margin;
    short		 bottom_margin;
    short		 left_margin;
    short		 right_margin;
    short		 row_height;
    short		 column_width;
    short		 row_gap;
    short		 column_gap;

    /* BUG Need to keep info about time */

    /* flags */
    unsigned		 has_kbd:1;
    unsigned		 map:1;	/* change to map when ready */
    unsigned		 rect_info:4;	/* x, y, width, or height set */
    unsigned		 top_level:1; /* indicates whether or */
    unsigned		 top_level_no_decor:1; /* does window have decors */
    unsigned		 created:1; 
    unsigned		 has_border:1;/* does window have border?*/
    unsigned		 being_rescaled:1;/* does window have border?*/
    unsigned 		 input_only:1; /*is the window input only ?*/
    unsigned 		 transparent:1; /* is background pixmap=None ?*/
    unsigned 		 in_fullscreen_mode:1; /*is the window in fullscreen mode ?*/
    unsigned 		 is_client_pane:1; /*is the window a client pane ?*/
    unsigned		 x_paint_window:1; /* is the window used for X graphics ? */
    unsigned		 win_front:1; /* is WIN_FRONT attr set before it is mapped? */
    unsigned		 inherit_colors:1;
    unsigned		 no_clipping:1; /* dont set clip rects on repaint */
    unsigned		 collapse_exposures:1;    /* colapse expose events  */
						  /* into a single event.   */
						  /* count = 0              */
    unsigned		 deaf:1;                  /* is window deaf ? */
#ifdef OW_I18N
    XIC                 xic;
    unsigned		 win_use_im:1; /* should this window create an IC using the default im? */
#endif
} Window_info;

typedef struct window_rescale_rect_obj {
    Rect        old_rect;
    Rect        new_rect;
    int         width_change, height_change,x_change,y_change;
    int         adjusted;
    Xv_Window   sw;
/* relationships */
} Window_rescale_rect_obj;

/* 
 * Package private
 */

#define	actual_row_height(win)		\
    (win->row_height ? win->row_height : xv_get(win->font, FONT_DEFAULT_CHAR_HEIGHT))

#define	actual_column_width(win)	\
    (win->column_width ? win->column_width : xv_get(win->font, FONT_DEFAULT_CHAR_WIDTH))

#define	actual_rescale_row_height(par,win)		\
    (win->row_height ? win->row_height : xv_get(par->font, FONT_DEFAULT_CHAR_HEIGHT))

#define	actual_rescale_column_width(par,win)	\
    (win->column_width ? win->column_width : xv_get(par->font, FONT_DEFAULT_CHAR_WIDTH))
/* window.c */
Pkg_private Notify_value	window_default_event_func();
Xv_private int			window_getrelrect();

/* window_set.c */
Pkg_private Xv_opaque		window_set_avlist();
Pkg_private int			win_appeal_to_owner();

/* window_get.c */
Pkg_private Xv_opaque		window_get_attr();

/* window_layout.c */
Pkg_private int			window_layout();

/* window_compat.c */
Xv_private void			window_scan_and_convert_to_pixels();

#endif ~window_impl_DEFINED
