/*
 * simple_panel.c -- an i18n XView program to display a panel button 
 * that says "Quit".   
 * All the strings are retrieved by dgettext().
 * Please refer to the man pages in docs for usage of dgettext and
 * messaging utilities xgettext and msgfmt.
 * Selecting the panel button exits the program.
 * The panel text item can accept ASCII and Japanese text input .
 */
#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/panel.h>

#define TEXTDOMAIN      "simple_panel"

main (argc, argv)
int argc;
char *argv[];
{
    Frame  frame;
    Panel  panel;
    void   quit();

    xv_init( XV_USE_LOCALE,     TRUE,
             XV_INIT_ARGC_PTR_ARGV, &argc, argv, 
             NULL);

    frame = (Frame)xv_create(NULL, FRAME,
        FRAME_LABEL,    argv[0],
        XV_WIDTH,       200,
        XV_HEIGHT,      100,
        NULL);

    panel = (Panel)xv_create(frame, PANEL, NULL);

    (void) xv_create(panel, PANEL_BUTTON,
        PANEL_LABEL_STRING,  dgettext(TEXTDOMAIN, "Quit"),
        PANEL_NOTIFY_PROC,   quit,
        NULL);
    (void) xv_create (panel, PANEL_TEXT,
                      PANEL_LABEL_STRING, dgettext(TEXTDOMAIN, "Text Item"),
                      NULL);
    xv_main_loop(frame);
}

void
quit()
{
    exit(0);
}
