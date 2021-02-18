/*
 * fonts.c -- provide an interface for the user to pick and choose
 * between font families and styles known to i18n XView.  The program
 * provides several panel buttons to choose between font types, and
 * a canvas window in which the user can type.  The characters typed
 * are printed in the current font.  Using the SELECT
 * button on the canvas window positions the current typing location
 * at the x,y coordinates of the button-down event.
 * All the strings are retrieved by dgettext().
 * Please refer to the man pages in docs for usage of dgettext and
 * messaging utilities xgettext and msgfmt.
 *
 */
#include <ctype.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/font.h>

Display 	*dpy;
GC      	gc;
XFontSetExtents *font_set_extents;
XFontSet        font_set_id;
Panel_item 	family_item, style_item, scale_item, name_item;
int 		canvas_width;
char 		buf[128];

#define TEXTDOMAIN      "fonts"

main(argc, argv)
int     argc;
char    *argv[];
{
    Frame       frame;
    Panel       panel;
    Canvas      canvas;
    XGCValues   gcvalues;
    Xv_Font     font;
    void        change_font();
    void        my_event_proc(), my_resize_proc();
    int         change_font_by_name();
    extern void exit();

    xv_init(XV_USE_LOCALE,          TRUE,
	    XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL);


    frame = (Frame)xv_create(XV_NULL, FRAME,
        FRAME_LABEL,            argv[0],
        FRAME_SHOW_FOOTER,      TRUE,
        NULL);

    panel = (Panel)xv_create(frame, PANEL,
        PANEL_LAYOUT,           PANEL_VERTICAL,
        NULL);
    (void) xv_create(panel, PANEL_BUTTON,
        PANEL_LABEL_STRING,     dgettext(TEXTDOMAIN, "Quit"),
        PANEL_NOTIFY_PROC,      exit,
        NULL);
    family_item = (Panel_item)xv_create(panel, PANEL_CHOICE,
        PANEL_LABEL_STRING,     dgettext(TEXTDOMAIN, "Family"),
        PANEL_LAYOUT,           PANEL_HORIZONTAL,
        PANEL_DISPLAY_LEVEL,    PANEL_CURRENT,
        PANEL_CHOICE_STRINGS,
            FONT_FAMILY_DEFAULT, FONT_FAMILY_DEFAULT_FIXEDWIDTH,
            FONT_FAMILY_LUCIDA, FONT_FAMILY_LUCIDA_FIXEDWIDTH,
            FONT_FAMILY_ROMAN, FONT_FAMILY_SERIF, 
            FONT_FAMILY_SANS_SERIF, FONT_FAMILY_COUR,
            FONT_FAMILY_CMR, FONT_FAMILY_GALLENT,
            FONT_FAMILY_OLGLYPH, FONT_FAMILY_OLCURSOR, NULL,
        PANEL_NOTIFY_PROC,      change_font,
        NULL);
    style_item = (Panel_item)xv_create(panel, PANEL_CHOICE,
        PANEL_LABEL_STRING,     dgettext(TEXTDOMAIN, "Style"),
        PANEL_LAYOUT,           PANEL_HORIZONTAL,
        PANEL_DISPLAY_LEVEL,    PANEL_CURRENT,
        PANEL_CHOICE_STRINGS,
            FONT_STYLE_DEFAULT, FONT_STYLE_NORMAL, FONT_STYLE_BOLD,
            FONT_STYLE_ITALIC, FONT_STYLE_BOLD_ITALIC, NULL,
        PANEL_NOTIFY_PROC,      change_font,
        NULL);
    scale_item = (Panel_item)xv_create(panel, PANEL_CHOICE,
        PANEL_LABEL_STRING,     dgettext(TEXTDOMAIN, "Scale"),
        PANEL_LAYOUT,           PANEL_HORIZONTAL,
        PANEL_DISPLAY_LEVEL,    PANEL_CURRENT,
        PANEL_CHOICE_STRINGS,
            dgettext(TEXTDOMAIN, "Small"), dgettext(TEXTDOMAIN, "Medium"), dgettext(TEXTDOMAIN, "Large"), dgettext(TEXTDOMAIN, "X-Large"), NULL,
        PANEL_NOTIFY_PROC,      change_font,
        NULL);
    window_fit(panel);

    canvas = (Canvas)xv_create(frame, CANVAS,
        XV_WIDTH,               400,
        XV_HEIGHT,              200,
        CANVAS_X_PAINT_WINDOW,  TRUE,
        CANVAS_RESIZE_PROC,     my_resize_proc,
        NULL);
    xv_set(canvas_paint_window(canvas),
        WIN_EVENT_PROC,         my_event_proc,
        WIN_CONSUME_EVENT,      LOC_WINENTER,
        NULL);

    window_fit(frame);

    dpy = (Display *)xv_get(frame, XV_DISPLAY);
    font = (Xv_Font)xv_get(frame, XV_FONT);    
    font_set_id = (XFontSet)xv_get(font, FONT_SET_ID);
    font_set_extents = XExtentsOfFontSet(font_set_id);        
  
    sprintf(buf, dgettext(TEXTDOMAIN, "Current font: %s-%s-%2d"), (char *)xv_get(font, FONT_FAMILY), (char *)xv_get(font, FONT_STYLE), (int)xv_get(font, FONT_SCALE));
    xv_set(frame, FRAME_LEFT_FOOTER, buf, FRAME_RIGHT_FOOTER, NULL, NULL);
    gcvalues.foreground = BlackPixel(dpy, DefaultScreen(dpy));
    gcvalues.background = WhitePixel(dpy, DefaultScreen(dpy));
    gcvalues.graphics_exposures = False;
    gc = XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)),
        GCForeground | GCBackground | GCGraphicsExposures,
        &gcvalues);
    xv_main_loop(frame);
}

void
my_event_proc(win, event)
Xv_Window win;
Event *event;
{
    static int x = 10, y = 10;
    Window xwin = (Window)xv_get(win, XV_XID);
    char c;

    if (event_is_up(event))
        return;
        
    if (event_is_string(event)) {
        wchar_t		text[100];
        
        mbstowcs(text, event_string(event), 100);
        XwcDrawString(dpy, xwin, font_set_id, gc, x, y, text, wslen(text));
        if ((x += XwcTextEscapement(font_set_id, text, wslen(text))) >= canvas_width) {
            y += font_set_extents->max_ink_extent.height;
            x = 10;
        }

    } else if (event_is_ascii(event)) {
        c = (char)event_action(event);
        XmbDrawString(dpy, xwin, font_set_id, gc, x, y, &c, 1);
        /* advance x to next position.  If over edge, linewrap */
        if ((x += XmbTextEscapement(font_set_id, &c, 1)) >= canvas_width) {
            y += font_set_extents->max_ink_extent.height;
            x = 10;
        }
    } else if (event_action(event) == ACTION_SELECT) {
        x = event_x(event);
        y = event_y(event);
    } else if (event_action(event) == LOC_WINENTER)
        win_set_kbd_focus(win, xwin);
}

/*
 * check resizing so we know how wide to allow the user to type.
 */
void
my_resize_proc(canvas, width, height)
Canvas canvas;
int width, height;
{

    canvas_width = width;
}

void
change_font(item, value, event)
Panel_item   item;
Event       *event;
{
    static int  family, style, scale;
    char        buf[128];
    Frame       frame;
    char        *family_name;
    char        *style_name;
    int         scale_value;
    Xv_Font     font;

    frame = (Frame)xv_get(xv_get(item, PANEL_PARENT_PANEL), XV_OWNER);
    family_name = (char *)xv_get(family_item, PANEL_CHOICE_STRING,
				xv_get(family_item, PANEL_VALUE));
    style_name = (char *)xv_get(style_item, PANEL_CHOICE_STRING,
				xv_get(style_item, PANEL_VALUE));
    scale_value = (int) xv_get(scale_item, PANEL_VALUE);
    xv_set(frame, FRAME_BUSY, TRUE, NULL);
    font = (Xv_Font)xv_find(frame, FONT,
        FONT_FAMILY,    family_name,
        FONT_STYLE,     style_name,
        /* scale_value happens to coincide with Window_rescale_state */
        FONT_SCALE,     scale_value,
	/* 
	 * If run on a server that cannot rescale fonts, only font
	 * sizes that exist should be passed
	 */
        FONT_SIZES_FOR_SCALE, 12, 14, 16, 22,
        NULL);
    xv_set(frame, FRAME_BUSY, FALSE, NULL);

    if (!font) {
        if (item == family_item) {
            sprintf(buf, dgettext(TEXTDOMAIN, "cannot load '%s'"), family_name);
            xv_set(family_item, PANEL_VALUE, family, NULL);
        } else if (item == style_item) {
            sprintf(buf, dgettext(TEXTDOMAIN, "cannot load '%s'"), style_name);
            xv_set(style_item, PANEL_VALUE, style, NULL);
        } else {
            sprintf(buf, dgettext(TEXTDOMAIN, "Not available in %s scale."),
                xv_get(scale_item, PANEL_CHOICE_STRING, scale));
            xv_set(scale_item, PANEL_VALUE, scale, NULL);
        }
        xv_set(frame, FRAME_LEFT_FOOTER, NULL, FRAME_RIGHT_FOOTER, buf, NULL);
        return;
    }
    if (item == family_item)
        family = value;
    else if (item == style_item)
        style = value;
    else
        scale = value;
    font_set_id = (XFontSet)xv_get(font, FONT_SET_ID);
    font_set_extents = XExtentsOfFontSet(font_set_id);
    sprintf(buf, dgettext(TEXTDOMAIN, "Current font: %s-%s-%2d"), family_name, style_name, scale_value);
    xv_set(frame, FRAME_LEFT_FOOTER, buf, FRAME_RIGHT_FOOTER, NULL, NULL);
}

