#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)fm_display.c 50.5 90/11/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Handle frame displaying and size changes.
 */

#include <X11/Xlib.h>
#include <xview_private/fm_impl.h>
#include <xview_private/draw_impl.h>
#include <xview/server.h>
#include <xview/screen.h>
#include <xview/font.h>
#include <xview/cms.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#define LEFT_FOOTER 1 
#define RIGHT_FOOTER 0

static void     frame_display_icon();
static void     flushline();
#ifdef OW_I18N
static void	XwcChangeStringProperty();
#endif

#ifndef SVR4
static short    _frame_dtgrayimage[16] = {
#else SVR4
static unsigned short    _frame_dtgrayimage[16] = {
#endif SVR4
    0x8888,
    0x8888,
    0x2222,
    0x2222,
    0x8888,
    0x8888,
    0x2222,
    0x2222,
    0x8888,
    0x8888,
    0x2222,
    0x2222,
    0x8888,
    0x8888,
    0x2222,
0x2222};

mpr_static(_frame_pixrectdtgray, 16, 16, 1, _frame_dtgrayimage);
struct pixrect *frame_bkgrd = &_frame_pixrectdtgray;



Pkg_private void
frame_display_label(frame)
    register Frame_class_info *frame;
{
#ifdef OW_I18N
    XwcChangeStringProperty(frame, XA_WM_NAME,
			    PropModeReplace, frame->label_wcs);
#else
    Xv_Drawable_info *info;

    DRAWABLE_INFO_MACRO(FRAME_PUBLIC(frame), info);
    XStoreName(xv_display(info), xv_xid(info), frame->label);
#endif
}

Pkg_private void
frame_display_footer(frame, paint_side)
    register Frame_class_info *frame;
    register int paint_side;
{
    Frame           frame_public = FRAME_PUBLIC(frame);
    Xv_Drawable_info *info;
    Xv_object       screen, server;

    DRAWABLE_INFO_MACRO(frame_public, info);
    screen = xv_get(frame_public, XV_SCREEN);
    server = xv_get(screen, SCREEN_SERVER);

#ifdef OW_I18N
    if (frame->leftfooter_wcs && paint_side == LEFT_FOOTER) {
	XwcChangeStringProperty(frame,
				xv_get(server, SERVER_WM_WINMSG_ERROR),
				PropModeReplace, frame->leftfooter_wcs);
    }
    if (frame->rightfooter_wcs && paint_side == RIGHT_FOOTER) {
	XwcChangeStringProperty(frame,
				xv_get(server, SERVER_WM_WINMSG_STATE),
				PropModeReplace, frame->rightfooter_wcs);
    }
    if (frame->leftfooter_wcs || frame->rightfooter_wcs)
#else OW_I18N
    if (frame->leftfooter && paint_side == LEFT_FOOTER) {
	XChangeProperty(xv_display(info), xv_xid(info),
			xv_get(server, SERVER_WM_WINMSG_ERROR), XA_STRING,
			8, PropModeReplace, frame->leftfooter,
			strlen(frame->leftfooter));
    }
    if (frame->rightfooter && paint_side == RIGHT_FOOTER) {
	XChangeProperty(xv_display(info), xv_xid(info),
			xv_get(server, SERVER_WM_WINMSG_STATE), XA_STRING,
			8, PropModeReplace, frame->rightfooter,
			strlen(frame->rightfooter));
    }
    if (frame->leftfooter || frame->rightfooter)
#endif OW_I18N
        XFlush(xv_display(info));
}

#ifdef OW_I18N
Pkg_private void
frame_display_IMstatus(frame, paint_side)
    register Frame_class_info *frame;
    register int paint_side;
{
    Frame           frame_public = FRAME_PUBLIC(frame);
    Xv_Drawable_info *info;
    Xv_object       screen, server;

    DRAWABLE_INFO_MACRO(frame_public, info);
    screen = xv_get(frame_public, XV_SCREEN);
    server = xv_get(screen, SCREEN_SERVER);

    if (frame->leftIMstatus_wcs && paint_side == LEFT_IMSTATUS) {
	XwcChangeStringProperty(frame,
				xv_get(server, SERVER_WM_WINMSG_IMSTATUS),
				PropModeReplace, frame->leftIMstatus_wcs);
    }
    if (frame->rightIMstatus_wcs && paint_side == RIGHT_IMSTATUS) {
	XwcChangeStringProperty(frame,
				xv_get(server, SERVER_WM_WINMSG_IMPREEDIT),
				PropModeReplace, frame->rightIMstatus_wcs);
    }
    if (frame->leftIMstatus_wcs || frame->rightIMstatus_wcs)
        XFlush(xv_display(info));
}
#endif OW_I18N

Pkg_private void
frame_display_busy(frame, status)
    register Frame_class_info *frame;
    int             status;

{
    Frame           frame_public = FRAME_PUBLIC(frame);
    Xv_Drawable_info *info;
    Xv_object       screen, server;


    DRAWABLE_INFO_MACRO(frame_public, info);
    screen = xv_get(frame_public, XV_SCREEN);
    server = xv_get(screen, SCREEN_SERVER);

    XChangeProperty(xv_display(info), xv_xid(info),
		    xv_get(server, SERVER_WM_WIN_BUSY), XA_INTEGER,
		    32, PropModeReplace, &status,
		    1);
    XFlush(xv_display(info));
}

static void
frame_display_icon(frame)
    register Frame_class_info *frame;
{
    icon_display(frame->icon, FRAME_PUBLIC(frame), 0, 0);
}

/*
 * highlight subwindow border for sw.
 */
Xv_private void
frame_kbd_use(frame_public, sw)
    Frame           frame_public;
    Xv_Window       sw;
{
    Frame_class_info *frame = FRAME_CLASS_PRIVATE(frame_public);

    if (frame->focus_subwindow != sw) {
	/* Remove caret from current frame focus subwindow */
	if (frame->focus_subwindow)
	    xv_set(frame->focus_subwindow, WIN_REMOVE_CARET, 0);
	/* Set new frame focus subwindow */
	frame->focus_subwindow = sw;
    }
    /* Show caret in subwindow with input focus */
    xv_set(sw, WIN_KBD_FOCUS, TRUE, 0);
}

/*
 * unhighlight subwindow border for client.
 */
Xv_private void
frame_kbd_done(frame_public, sw)
    Frame           frame_public;
    Xv_Window       sw;
{
    xv_set(sw, WIN_KBD_FOCUS, FALSE, 0);
}

/*
 * Display utilities.  Note: Belongs somewhere else.
 */
#define	newline(x, y, w, h, chrht, rect) \
	{ *x = rect->r_left; \
	  *y += chrht; \
	  *h -= chrht; \
	  *w = rect->r_width; \
	}

Xv_private void
frame_format_string_to_rect(pixwin, s, font, rect)
    Xv_Window       pixwin;
    char           *s;
    Pixfont        *font;
    register Rect  *rect;
{
    register char  *charptr, *lineptr, *breakptr, c;
    XFontStruct		*x_font_info;
    short           x, y, w, h, chrht, chrwth;
#ifdef lint
    short           dummy;
#endif

    if (s == 0 || font == 0)
	return;

    x = rect->r_left;
#ifdef lint
    dummy = x;
    x = dummy;
#endif
    y = rect->r_top;
    w = rect->r_width;
    h = rect->r_height;
    chrht = xv_get(font, FONT_DEFAULT_CHAR_HEIGHT);
    x_font_info = (XFontStruct *)xv_get(font, FONT_INFO);
    breakptr = lineptr = s;
    for (charptr = s; *charptr != '\0'; charptr++) {
	c = (*charptr) & 127;
	/*
	 * Setup to wrap on blanks Note: Need better break test.
	 */
	if (c == ' ') {
	    breakptr = charptr;
	}
	if (x_font_info->per_char)  {
	    chrwth = x_font_info->per_char[c - x_font_info->min_char_or_byte2].width;
	}
	else  {
	    chrwth = x_font_info->min_bounds.width;
	}
	/*
	 * Wrap when not enough room for next char
	 */
	if (w < chrwth) {
	    if (breakptr != lineptr) {
		flushline(pixwin, rect->r_left, y, lineptr, breakptr, font);
		charptr = breakptr;
		lineptr = ++breakptr;
		continue;
	    } else {
		flushline(pixwin, rect->r_left, y, lineptr, charptr, font);
		breakptr = lineptr = charptr;
	    }
	    newline(&x, &y, &w, &h, chrht, rect);
	}
	/*
	 * Punt when run out of vertical space
	 */
	if (h < chrht)
	    break;
	w -= chrwth;
	x += chrwth;
    }
    flushline(pixwin, rect->r_left, y, lineptr, charptr, font);
}

#define	STRBUF_LEN	1000

static void
flushline(pixwin, x, y, lineptr, charptr, font)
    Xv_Window       pixwin;
    int             x, y;
    char           *lineptr, *charptr;
    Pixfont        *font;
{
    char            strbuf[STRBUF_LEN];
    int             len = charptr - lineptr;
    XFontStruct		*x_font_info;

    if (charptr == 0 || lineptr == 0 || len > STRBUF_LEN)
	return;

    x_font_info = (XFontStruct *)xv_get(font, FONT_INFO);
    (void) strncpy(strbuf, lineptr, len);
    *(strbuf + len) = '\0';
    if (x_font_info->per_char)  {
        (void) xv_text(pixwin, 
	    x - x_font_info->per_char[*strbuf - x_font_info->min_char_or_byte2].lbearing,
	    y + x_font_info->ascent, PIX_SRC, font, strbuf);
    }
    else  {
        (void) xv_text(pixwin, 
	    x - x_font_info->min_bounds.lbearing,
	    y + x_font_info->ascent, PIX_SRC, font, strbuf);
    }
}


/* ARGSUSED */
Pkg_private
frame_set_color(frame, fg, bg)
    Frame_class_info *frame;
    register Xv_singlecolor *fg;
    register Xv_singlecolor *bg;
{
    Frame		frame_public = FRAME_PUBLIC(frame);
    Xv_Drawable_info	*info;
    Cms			cms;	
    Xv_singlecolor	colors[2];
    short		index, rgb_count = 0;
    int			static_visual;

    DRAWABLE_INFO_MACRO(frame_public, info);
    if ((xv_depth(info) < 2) || (!fg && !bg)) {
	return;
    }

    colors[0].red = bg->red;
    colors[0].green = bg->green;
    colors[0].blue = bg->blue;

    colors[1].red = fg->red;
    colors[1].green = fg->green;
    colors[1].blue = fg->blue;

    cms = (Cms)xv_create(xv_screen(info), CMS,
			 CMS_SIZE, 2,
			 CMS_COLORS, colors,
			 CMS_FRAME_CMS, TRUE,
			 CMS_TYPE, XV_STATIC_CMS,
			 XV_VISUAL_CLASS, xv_get(frame_public, XV_VISUAL_CLASS, 0),
			 NULL);
    if (cms != (Cms)NULL)
      xv_set(frame_public, WIN_CMS, cms, NULL);
}


#ifdef OW_I18N
static void
XwcChangeStringProperty(frame, props, mode, w)
Frame_class_info	*frame;
Atom			props;
int			mode;
register wchar_t	*w;
{
    register Xv_Drawable_info *info;
    register wchar_t	*wp;
    register char	*p;

    DRAWABLE_INFO_MACRO(FRAME_PUBLIC(frame), info);
    for (wp = w; *wp; wp++)
    {
	if (! iswascii(*wp))
	{
		wchar_t		*wcs;
	    /*
	     * There are none ASCII character exist, so, we have to
	     * send it as Compound Text Atom.
	     */
	    p = wcstoctsdup(w);
wcs = ctstowcsdup(p);    
	    XChangeProperty(xv_display(info), xv_xid(info), props,
			    xv_get(xv_server(info), SERVER_COMPOUND_TEXT), 8,
			    mode, (unsigned char *)p, strlen(p));
	    free(p);
	    return;
	}
    }

    /*
     * There are only ASCII characters, we can send it as STRING atom.
     */
    p = wcstombsdup(w);
    XChangeProperty(xv_display(info), xv_xid(info), props,
		    XA_STRING, 8, mode, (unsigned char *)p, strlen(p));
    free(p);
}
#endif OW_I18N
