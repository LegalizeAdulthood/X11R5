#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)window_get.c 50.8 91/01/08";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <stdio.h>
#include <xview_private/windowimpl.h>
#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <xview/notify.h>
#include <xview/win_screen.h>
#include <xview/rectlist.h>
#include <xview/xv_xrect.h>
#include <xview/cms.h>

/*
 * Private
 */

static int      get_mask_bit();
static Xv_opaque  window_empty_event_proc();
static struct timeval alarmdata;


/* BUG: should be in hdr file */
extern void		win_xmask_to_im(); /* defined in win_input.c */
extern unsigned int	win_im_to_xmask(); /* defined in win_input.c */


/* VARARGS2 */
Xv_public       Xv_opaque
window_get(win_public, attr, d1, d2, d3, d4, d5)
    Xv_Window       win_public;
    Window_attribute attr;
{
    return xv_get(win_public, attr, d1, d2, d3, d4, d5);
}


Pkg_private     Xv_opaque
window_get_attr(win_public, status, attr, valist)
    Xv_Window       win_public;
    int            *status;
    Window_attribute attr;
    va_list         valist;
{
    register Window_info *win = WIN_PRIVATE(win_public);
    register Xv_Drawable_info *info;
    register Xv_opaque v = 0;
    int             vp;

    DRAWABLE_INFO_MACRO(win_public, info);
    switch (attr) {
      case WIN_OUTER_RECT:
	{
	    static Rect     rec;
	    (void) win_appeal_to_owner(FALSE, win, (Xv_opaque) WIN_GET_RECT, (Xv_opaque) & rec);
	    if (win->has_border)
		rect_borderadjust(&rec, WIN_DEFAULT_BORDER_WIDTH);
	    v = (Xv_opaque) & rec;
	}
	break;

      case WIN_X_PAINT_WINDOW:
	v = (Xv_opaque) win->x_paint_window;
	break;

      case WIN_NO_CLIPPING:
	v = (Xv_opaque) win->no_clipping;
	break;

      case WIN_X_CLIP_RECTS:{
	    static Xv_xrectlist xrects;
	    Rectlist       *rl;
	    extern Rectlist *win_get_damage();

	    if ((rl = win_get_damage(win_public)) != RECTLIST_NULL) {
		xrects.count = win_convert_to_x_rectlist(rl, xrects.rect_array,
							 XV_MAX_XRECTS);
		v = (Xv_opaque) & xrects;
	    } else {
		v = (Xv_opaque) NULL;
	    }
	    break;
	}

      case WIN_BIT_GRAVITY:
	{
	    XWindowAttributes win_attrs;
	    
	    (void) XGetWindowAttributes(xv_display(info), xv_xid(info), &win_attrs);
	    v = (Xv_opaque) win_attrs.bit_gravity;
        }
	break;
	
      case WIN_BORDER:
	v = (Xv_opaque) win->has_border;
	break;

      case WIN_CMD_LINE:
	v = (Xv_opaque) win->cmdline;
	break;

      case WIN_INPUT_ONLY:
	v = (Xv_opaque) win->input_only;
	break;

      case WIN_IS_CLIENT_PANE:
	v = (Xv_opaque) win->is_client_pane;
	break;

      case WIN_IS_IN_FULLSCREEN_MODE:
	v = (Xv_opaque) win->in_fullscreen_mode;
	break;

      case WIN_IS_ROOT:
	if (win->owner == NULL)
	    v = (Xv_opaque) TRUE;
	break;

      case WIN_FINDINTERSECT:{
	    /* return win number of child underlying cursor */
	    short           x, y;

	    x = va_arg(valist, int);
	    y = va_arg(valist, int);
	    v = (Xv_opaque) win_findintersect(win_public, x, y);
	}
	break;

      case WIN_ALARM_DATA:
	/* return pointer to static that has alarm info from environment */
	/* (someday this will be more than just time values) */
	alarmdata.tv_sec = 0;
	alarmdata.tv_usec = 100000;

	v = (Xv_opaque) & alarmdata;
	break;

      case WIN_TOP_LEVEL:
	v = (Xv_opaque) win->top_level;
	break;

      case WIN_TOP_LEVEL_NO_DECOR:
	v = (Xv_opaque) win->top_level_no_decor;
	break;

      case WIN_CLIENT_DATA:
	v = (Xv_opaque) win->client_data;
	break;

      case WIN_KBD_FOCUS:
	v = (Xv_opaque) win->has_kbd;
	break;

      case WIN_DESIRED_WIDTH:
	v = (Xv_opaque) win->desired_width;
	break;

      case WIN_DESIRED_HEIGHT:
	v = (Xv_opaque) win->desired_height;
	break;

      case WIN_COLUMNS:
	(void) win_appeal_to_owner(FALSE, win, (Xv_opaque) WIN_GET_WIDTH,
				   (Xv_opaque) & vp);
	vp -= win->left_margin + win->right_margin;
	v = (Xv_opaque) (vp / (actual_column_width(win) + win->column_gap));
	break;

      case WIN_CURSOR:
	v = win->cursor;
	break;

      case WIN_EVENT_PROC:
	v = (Xv_opaque) (win->event_proc);
	if (!v)
	    v = (Xv_opaque) window_empty_event_proc;
	break;

      case WIN_FONT:
	v = (Xv_opaque) win->font;
	break;

      case WIN_GLYPH_FONT:
	if (!win->glyph_font)
#ifdef OW_I18N
	{
	    int  size;

	    /*
	     * This new code nothing do with I18N issue, but very
	     * generic fix.  Bcause 24 dot font is very essential to
	     * the some Asian, and 24 dot is larger than "extra_large"
	     * scale.  This fix will try to find the matching glyph
	     * font regardless scaling issue.  The new code borrowed
	     * from the idea from panel code (use size intead of
	     * scale).
	     */
	    if ((size = xv_get(win->font, FONT_SIZE)) != FONT_NO_SIZE)
		win->glyph_font = xv_find(win_public, FONT,
				  FONT_FAMILY, FONT_FAMILY_OLGLYPH,
				  FONT_SIZE, size,
				  FONT_TYPE, FONT_TYPE_GLYPH,
				  NULL);
	    if (win->glyph_font == NULL)
		win->glyph_font = xv_find(win_public, FONT,
				  FONT_FAMILY, FONT_FAMILY_OLGLYPH,
				  FONT_SCALE, xv_get(win->font, FONT_SCALE),
				  FONT_TYPE, FONT_TYPE_GLYPH,
				  0);
	}
#else
	    win->glyph_font = xv_find(win_public, FONT,
				  FONT_FAMILY, FONT_FAMILY_OLGLYPH,
				  FONT_SCALE, xv_get(win->font, FONT_SCALE),
				  FONT_TYPE, FONT_TYPE_GLYPH,
				  0);
#endif
	v = (Xv_opaque) win->glyph_font;
	break;

      case WIN_HEIGHT:
	(void) win_appeal_to_owner(FALSE, win, (Xv_opaque) WIN_GET_HEIGHT,
				   (Xv_opaque) & vp);
	v = (Xv_opaque) vp;	/* Move to register */
	break;

      case WIN_LAYOUT_PROC:
	v = (Xv_opaque) (win->layout_proc);
	break;

      case WIN_MENU:
	v = win->menu;
	break;

      case WIN_NOTIFY_SAFE_EVENT_PROC:
	v = (Xv_opaque) (notify_get_event_func(win_public, NOTIFY_SAFE));
	break;

      case WIN_NOTIFY_IMMEDIATE_EVENT_PROC:
	v = (Xv_opaque) (notify_get_event_func(win_public, NOTIFY_IMMEDIATE));
	break;

      case WIN_OWNER:
	if (win->owner)
	    v = WIN_PUBLIC(win->owner);
	break;

      case WIN_PARENT:
	v = win->parent;
	break;

      case WIN_FRAME:
	for (v = win_public; v; v = (Xv_opaque) WIN_PRIVATE(v)->parent) {
	    if ((Bool) xv_get(v, WIN_TOP_LEVEL))
		break;
	}
	break;

      case WIN_PERCENT_HEIGHT:
	/* % height needs owner */
	if (!win->owner)
	    break;

	v = (Xv_opaque)
	    ((xv_get(win_public, WIN_GET_HEIGHT) + win->owner->column_gap)*100 /
	     (xv_get(WIN_PUBLIC(win->owner), WIN_HEIGHT)
	      - win->owner->top_margin - win->owner->bottom_margin));
	break;

      case WIN_PERCENT_WIDTH:
	/* % width needs owner */
	if (!win->owner)
	    break;

	v = (Xv_opaque)
	    ((xv_get(win_public, WIN_GET_WIDTH) + win->owner->row_gap) * 100 /
	     (xv_get(WIN_PUBLIC(win->owner), WIN_WIDTH)
	      - win->owner->left_margin - win->owner->right_margin));
	break;

      case WIN_RECT:
	{
	    static Rect     rec;
	    (void) win_appeal_to_owner(FALSE, win, (Xv_opaque) WIN_GET_RECT, (Xv_opaque) & rec);
	    v = (Xv_opaque) & rec;
	}
	break;

      case WIN_ROWS:
	(void) win_appeal_to_owner(FALSE, win, (Xv_opaque) WIN_GET_HEIGHT, (Xv_opaque) & vp);
	vp -= win->top_margin + win->bottom_margin;
	v = (Xv_opaque) (vp / (actual_row_height(win) + win->row_gap));
	break;

      case WIN_SCREEN_RECT:
	{
	    static Rect     rect;

	    rect = *(Rect *) xv_get(xv_root(info), WIN_RECT);

	    v = (Xv_opaque) & rect;
	}
	break;

      case WIN_DEPTH:
	v = (Xv_opaque) xv_depth(info);
	break;

      case XV_SHOW:
      case WIN_MAP:
	v = (Xv_opaque) win->map;
	break;

      case WIN_RETAINED:
	v = (Xv_opaque) win_get_retained(win_public);
	break;

      case WIN_TYPE:
	/* SunView1.X compatibility */
	v = (Xv_opaque) WINDOW_TYPE;
	break;

      case WIN_WIDTH:
	(void) win_appeal_to_owner(FALSE, win, (Xv_opaque) WIN_GET_WIDTH, (Xv_opaque) & vp);
	v = (Xv_opaque) vp;	/* Move to register */
	break;

      case WIN_WINDOW_GRAVITY:
	{
	    XWindowAttributes win_attrs;
	    
	    (void) XGetWindowAttributes(xv_display(info), xv_xid(info), &win_attrs);
	    v = (Xv_opaque) win_attrs.win_gravity;
        }
	break;
	
      case WIN_X:
	(void) win_appeal_to_owner(FALSE, win, (Xv_opaque) WIN_GET_X, (Xv_opaque) & vp);
	v = (Xv_opaque) vp;	/* Move to register */
	break;

      case WIN_Y:
	(void) win_appeal_to_owner(FALSE, win, (Xv_opaque) WIN_GET_Y, (Xv_opaque) & vp);
	v = (Xv_opaque) vp;	/* Move to register */
	break;

      case WIN_INPUT_MASK:{
	    static Inputmask im;

	    (void) win_xmask_to_im(win->xmask, &im);
	    v = (Xv_opaque) & im;
	    break;
	}
      case WIN_TOP_MARGIN:
	v = (Xv_opaque) win->top_margin;
	break;

      case WIN_BOTTOM_MARGIN:
	v = (Xv_opaque) win->bottom_margin;
	break;

      case WIN_LEFT_MARGIN:
	v = (Xv_opaque) win->left_margin;
	break;

      case WIN_RIGHT_MARGIN:
	v = (Xv_opaque) win->right_margin;
	break;

      case WIN_ROW_HEIGHT:
	v = (Xv_opaque) actual_row_height(win);
	break;

      case WIN_COLUMN_WIDTH:
	v = (Xv_opaque) actual_column_width(win);
	break;

      case WIN_ROW_GAP:
	v = (Xv_opaque) win->row_gap;
	break;

      case WIN_COLUMN_GAP:
	v = (Xv_opaque) win->column_gap;
	break;

      case WIN_CONSUME_EVENT:{
	    Inputmask       im;

	    (void) win_xmask_to_im(win->xmask, &im);
	    v = (Xv_opaque) get_mask_bit(&im, va_arg(valist, Window_input_event), win_public);
	    break;
	}
      case WIN_CONSUME_X_EVENT_MASK:
      case WIN_X_EVENT_MASK:
	v = (Xv_opaque) win->xmask;
	break;

      case WIN_RECT_INFO:
	v = (Xv_opaque) win->rect_info;
	break;

      case WIN_MOUSE_XY:{
	    static Rect     rect;
	    /* Rect struct is used to get mouse x and y */
	    win_getmouseposition(win_public, &rect.r_left, &rect.r_top);
	    v = (Xv_opaque) & rect;
	    break;
	}

      case WIN_MESSAGE_TYPE:
	return ((Xv_opaque) win->client_message.type);

      case WIN_MESSAGE_FORMAT:
	return ((Xv_opaque) win->client_message.format);

      case WIN_MESSAGE_DATA:
	return ((Xv_opaque) win->client_message.data.b);

      case WIN_FIT_HEIGHT:
      case WIN_FIT_WIDTH:
      case WIN_VERTICAL_SCROLLBAR:
      case WIN_HORIZONTAL_SCROLLBAR:	/* package will handle these */
	*status = XV_ERROR;
	break;

      case XV_ROOT:
	v = (Xv_opaque) xv_root(info);
	break;

      case XV_SCREEN:
	v = xv_screen(info);
	break;

      case WIN_INHERIT_COLORS:
	v = (Xv_opaque) win->inherit_colors;
	break;

      case XV_VISUAL_CLASS:
	v = (Xv_opaque) win->visual_class;
	break;

      case WIN_CMS_NAME:
	v = (Xv_opaque) xv_get(info->cms, CMS_NAME);
	break;

      case WIN_FOREGROUND_COLOR:
	v = (Xv_opaque) xv_cms_fg(info);
	break;

      case WIN_BACKGROUND_COLOR:
	v = (Xv_opaque) xv_cms_bg(info);
	break;

      case WIN_BACKGROUND_PIXMAP:
	v = (Xv_opaque) win->background_pixmap;
	break;

      case WIN_COLLAPSE_EXPOSURES:
	v = (Xv_opaque) win->collapse_exposures;
	break;

      case WIN_CMS:
	v = (Xv_opaque) info->cms;
	break;

      case WIN_COLOR_INFO: {
	static Xv_Color_info	xv_color_info;

	xv_color_info.cms    = xv_cms(info);
	xv_color_info.cms_fg = xv_cms_fg(info);
	xv_color_info.cms_bg = xv_cms_bg(info);
	v = (Xv_opaque) &xv_color_info;
	break;
      }

      case WIN_X_COLOR_INDICES: {
	  v = (Xv_opaque)xv_get(xv_cms(info), CMS_INDEX_TABLE);
	break;
      }

      case WIN_CMS_DATA: {
	  static Xv_cmsdata	*xv_cms_data = NULL;
	  static short		 xv_rgb_size = 0;
	  XColor		 xcolors[256];
	  register int           i;
	  unsigned long		 cms_size;

	  if (!xv_cms_data) {
	      xv_cms_data = xv_alloc(Xv_cmsdata);
	  }

	  cms_size = (unsigned long)xv_get(xv_cms(info), CMS_SIZE);
	  if (xv_rgb_size < cms_size) {
	      if (xv_rgb_size) {
		  free(xv_cms_data->red);
		  free(xv_cms_data->green);
		  free(xv_cms_data->blue);
	      }

	      xv_cms_data->red = xv_alloc_n(unsigned char *, 
				     cms_size * sizeof(unsigned char));
	      xv_cms_data->green = xv_alloc_n(unsigned char *, 
				     cms_size * sizeof(unsigned char));
	      xv_cms_data->blue = xv_alloc_n(unsigned char *, 
				     cms_size * sizeof(unsigned char));
	      xv_rgb_size = cms_size;
	  }
	
	  xv_cms_data = (Xv_cmsdata *)
			  xv_get(xv_cms(info), CMS_CMS_DATA, xv_cms_data);
	  v = (Xv_opaque)xv_cms_data;
	  break;
     }	
	      
#ifdef OW_I18N
      case WIN_IC: {
	  v = (Xv_opaque)win->xic;
	  break;
      }	

      case WIN_USE_IM: {
	  v = (Xv_opaque)win->win_use_im;
	  break;
      }	

      case XV_LC_BASIC_LOCALE:
	return (Xv_opaque) xv_get(XV_SERVER_FROM_WINDOW(win_public),
						XV_LC_BASIC_LOCALE);

      case XV_LC_DISPLAY_LANG:
	return (Xv_opaque) xv_get(XV_SERVER_FROM_WINDOW(win_public),
						XV_LC_DISPLAY_LANG);

      case XV_LC_INPUT_LANG:
	return (Xv_opaque) xv_get(XV_SERVER_FROM_WINDOW(win_public),
						XV_LC_INPUT_LANG);

      case XV_LC_NUMERIC:
	return (Xv_opaque) xv_get(XV_SERVER_FROM_WINDOW(win_public),
						XV_LC_NUMERIC);

      case XV_LC_TIME_FORMAT:
	return (Xv_opaque) xv_get(XV_SERVER_FROM_WINDOW(win_public),
						XV_LC_TIME_FORMAT);
#endif

      default:
	if (xv_check_bad_attr(&xv_window_pkg, attr) == XV_ERROR) {
	    *status = XV_ERROR;
	}
	break;

    }
    return v;
}


static int
get_mask_bit(mask, code, win_public)
    Inputmask      *mask;
    Window_input_event code;
    Xv_Window       win_public;
{
    int             v, i;

    switch (code) {
      case WIN_NO_EVENTS:
	v = (mask == 0);
	break;

      case WIN_ASCII_EVENTS:
	v = mask->im_flags & (IM_ASCII | IM_META);
	break;

      case WIN_UP_ASCII_EVENTS:
	v = mask->im_flags & (IM_NEGASCII | IM_NEGMETA);
	break;

      case WIN_UP_EVENTS:
	v = mask->im_flags & IM_NEGEVENT;
	break;

      case WIN_MOUSE_BUTTONS:
	v = (win_getinputcodebit(mask, MS_LEFT) &&
	     win_getinputcodebit(mask, MS_MIDDLE) &&
	     win_getinputcodebit(mask, MS_RIGHT)
	    );
	break;

      case WIN_LEFT_KEYS:
	v = 1;
	for (i = 1; i <= 15; i++)
	    v &= win_getinputcodebit(mask, KEY_LEFT(i));
	break;

      case WIN_RIGHT_KEYS:
	v = 1;
	for (i = 1; i <= 15; i++)
	    v &= win_getinputcodebit(mask, KEY_RIGHT(i));
	break;

      case WIN_META_EVENTS:
	v = mask->im_flags & IM_META;
	break;

      case WIN_UP_META_EVENTS:
	v = mask->im_flags & IM_NEGMETA;
	break;

      case WIN_TOP_KEYS:
	v = 1;
	for (i = 1; i <= 15; i++)
	    v &= win_getinputcodebit(mask, KEY_TOP(i));
	break;

      case WIN_IN_TRANSIT_EVENTS:
	v = mask->im_flags & IM_INTRANSIT;
	break;

      default:
	if (isworkstationdevid((int) code))
	    v = win_getinputcodebit(mask, (int) code);
	else {
	    /* see if key press events are enabled. */
	    unsigned int    xmask = win_im_to_xmask(win_public, mask);
	    Inputmask       im;

	    /*
	     * translating between im to xmask sets KeyPressMask if any key
	     * press events are enabled.  Translating xmask back to im will
	     * enable all key press events.
	     */
	    win_xmask_to_im(xmask, &im);
	    win_getinputcodebit(&im, KEY_LEFT(1));
	}
	break;

    }
    return (v != 0);
}

static Xv_opaque
window_empty_event_proc()
{
	short dummy = 0;
	return((Xv_opaque) dummy);
}
