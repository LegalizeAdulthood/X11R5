/*
 * xv_termsw.c
 * Demonstrate incorporation of an i18n Term subwindow in an application;
 * keyboard input to the termsw can come either directly to the
 * termsw or from an adjoining panel text item.
 */
#include <stdio.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/termsw.h>
#include <widec.h>


Termsw          term;
Panel_item      text_item;

main(argc,argv)
int     argc;
char    *argv[];
{
    Frame       frame;
    Panel       panel;
    int         notify_proc();

   xv_init( XV_USE_LOCALE,	TRUE,
	    XV_INIT_ARGC_PTR_ARGV, &argc, argv, 
	    NULL);

    frame = (Frame)xv_create(NULL, FRAME, NULL);
    panel = (Panel)xv_create(frame, PANEL, NULL);
    text_item = (Panel_item)xv_create(panel, PANEL_TEXT,
        PANEL_LABEL_STRING,         "Command:",
        PANEL_NOTIFY_PROC,          notify_proc,
        PANEL_VALUE_DISPLAY_LENGTH, 20,
        NULL);
    (void) xv_create(panel, PANEL_BUTTON,
        PANEL_LABEL_STRING,     "Apply",
        PANEL_NOTIFY_PROC,      notify_proc,
        NULL);
    window_fit_height(panel);

    term = (Termsw)xv_create(frame, TERMSW, NULL);

    window_fit(frame);
    xv_main_loop(frame);
}

/*
 * This procedure is called when the user this return on the
 * panel text item or clicking on the <apply> button.
 * Use ttysw_input_wcs() to feed the string to the termal window.
 */
int
notify_proc(item,event)
Panel_item      item;
Event   *event;
{
    wchar_t        str[81];
    
    wsprintf(str, "%.81ws\n", (wchar_t *)xv_get(text_item, PANEL_VALUE_WCS));
    ttysw_input_wcs(term, str, wslen(str));
    
    str[0] = NULL;
    xv_set(text_item, PANEL_VALUE_WCS, str, NULL);
    return XV_OK;
}
