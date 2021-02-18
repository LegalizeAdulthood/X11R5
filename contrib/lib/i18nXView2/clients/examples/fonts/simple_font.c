/*
 * simple_font.c -- very simple program showing how to render text
 * using a font gotten from xv_find().  Hello World and
 * 日本語 Hello World are printed in
 * the top-left corner of a canvas window.
 */
#include <stdio.h>
#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/canvas.h>
#include <locale.h>
#include <xview/font.h>
#include <widec.h>
#include <xview/xv_xrect.h>

#define GC_KEY  10 /* any arbitrary number -- used for XV_KEY_DATA */

main(argc, argv)
int     argc;
char    *argv[];
{
    Frame       frame;
    Canvas      canvas;
    XGCValues   gcvalues;
    Xv_Font     font;
    void        my_repaint_proc();
    Display     *dpy;
    GC          gc;

    /*
     * Initialize, create base frame (with footers) and create canvas.
     */
    xv_init( XV_USE_LOCALE,	TRUE,
	     XV_INIT_ARGC_PTR_ARGV, &argc, argv, 
	     NULL);

    frame = (Frame)xv_create(XV_NULL, FRAME, NULL);

    canvas = (Canvas)xv_create(frame, CANVAS,
        XV_WIDTH,               400,
        XV_HEIGHT,              200,
        WIN_USE_IM,     	FALSE,
        CANVAS_X_PAINT_WINDOW,  TRUE,
        CANVAS_REPAINT_PROC,    my_repaint_proc,
        NULL);
    window_fit(frame);

    dpy = (Display *)xv_get(frame, XV_DISPLAY);
    
    font = (Xv_Font)xv_find(frame, FONT, 
    				   FONT_FAMILY, FONT_FAMILY_SANS_SERIF, 
				   FONT_STYLE, FONT_STYLE_NORMAL, NULL);
    
    if (!font) {
        fprintf(stderr, "%s: cannot use font: %s-%s.\n", argv[0], FONT_FAMILY_SANS_SERIF, FONT_STYLE_NORMAL);
        font = (Xv_Font)xv_get(frame, XV_FONT);
    }

    /* Create a GC to use with Xlib graphics -- set the fg/bg colors
     * and set the Font, which is the XV_XID of the XView font object.
     */
    gcvalues.font = (Font)xv_get(font, XV_XID);
    gcvalues.foreground = BlackPixel(dpy, DefaultScreen(dpy));
    gcvalues.background = WhitePixel(dpy, DefaultScreen(dpy));
    gcvalues.graphics_exposures = False;
    gc = XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)),
        GCForeground | GCBackground | GCFont | GCGraphicsExposures,
        &gcvalues);

    /* Assign the gc to the canvas object so we can use the same gc
     * each time we draw into the canvas.  Also avoids a global
     * variable to store the GC.
     */
    xv_set(canvas, XV_KEY_DATA, GC_KEY, gc, NULL);
    xv_main_loop(frame);
}

/*
 * Called every time the window needs repainting.
 */
void
my_repaint_proc(canvas, pw, dpy, xwin, xrects)
Canvas          canvas;
Xv_Window       pw;
Display         *dpy;
Window          xwin;
Xv_xrectlist    *xrects;
{
    GC 		gc = (GC)xv_get(canvas, XV_KEY_DATA, GC_KEY);
    Font	font = xv_get(canvas, XV_FONT);
    XFontSet	font_set_id = (XFontSet)xv_get(font, FONT_SET_ID);


    XDrawString(dpy, xwin, gc, 10, 20,
        "Hello World", 11); /* 11 = strlen("Hello World") */
        
    XmbDrawString(dpy, xwin, font_set_id, gc, 10, 50,
        "日本語 Hello World", strlen( "日本語 Hello World"));    
}
