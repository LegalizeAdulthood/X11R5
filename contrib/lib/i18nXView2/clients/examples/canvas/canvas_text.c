/*
 * canvas_input.c --
 * Display a canvas that accept keyboard input.
 * When there is committed Japanese text input, it
 * will draw into the canvas window.
 */


#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/canvas.h>
#include <locale.h>
#include <xview/font.h>
#include <widec.h>



Frame 	frame;
Display		*dpy;
GC		gc;
XFontSet        font_set_id;
XFontSetExtents *font_set_extents;
int 		canvas_width;



static void canvas_event_proc(), my_resize_proc();

main (argc, argv)
int argc;
char *argv[];

{
	Font	font;
	Canvas	canvas;
	char	*mb_str		= "ÆüËÜ¸ì Window";
	XGCValues   gcvalues;
	
	wchar_t	wcs_str[100];

    /*
     * Initialize, create base frame (with footers) and create canvas.
     */
	xv_init( XV_USE_LOCALE,	TRUE,
		 XV_INIT_ARGC_PTR_ARGV, &argc, argv, 
		 NULL);
	/*
	 * Convert the EUC string mb_str to  the  Process
         * Code  string wcs_str.
         */
	mbstowcs(wcs_str, mb_str, 100);

	frame = (Frame) xv_create (NULL, FRAME_BASE,
				FRAME_LABEL_WCS, wcs_str,
				XV_WIDTH,	200,
				XV_HEIGHT,	100,
				NULL);

	canvas = xv_create(frame, CANVAS,
			XV_WIDTH,	600,
			XV_HEIGHT,	400,
			CANVAS_RESIZE_PROC,     my_resize_proc,
			CANVAS_PAINTWINDOW_ATTRS,
				WIN_EVENT_PROC, canvas_event_proc,
				WIN_CONSUME_EVENT, WIN_ASCII_EVENTS,
			    		   KBD_USE, KBD_DONE,
			    		   NULL,			    	
			NULL);
			
	dpy = (Display *)xv_get(canvas, XV_DISPLAY);
	font = xv_get(canvas, XV_FONT);
	font_set_id = (XFontSet)xv_get(font, FONT_SET_ID);
	font_set_extents = XExtentsOfFontSet(font_set_id);        


        gcvalues.foreground = BlackPixel(dpy, DefaultScreen(dpy));
        gcvalues.background = WhitePixel(dpy, DefaultScreen(dpy));
        gcvalues.graphics_exposures = False;
        gc = XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)),
             GCForeground | GCBackground | GCGraphicsExposures,
             &gcvalues);
			
	window_fit(frame);
	xv_main_loop(frame);
	exit(0);
}

	


static void
canvas_event_proc(window, event)
Xv_window       window;
Event           *event;
{
    static int x = 10, y = 10;
    Window xwin = (Window)xv_get(window, XV_XID);
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
        win_set_kbd_focus(window, xwin);
}

/*
 * check resizing so we know how wide to allow the user to type.
 */
static void
my_resize_proc(canvas, width, height)
Canvas canvas;
int width, height;
{

    canvas_width = width;
}
