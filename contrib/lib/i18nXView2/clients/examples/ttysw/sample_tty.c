/*
 * sample_tty.c -- create a base frame with a i18n tty subwindow.
 * This subwindow runs a UNIX command specified in an argument
 * vector as shown below.  
 * The command will be run in the same locale as the
 * OpenWindows environment.  The example does a "man cat".
 */
#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/tty.h>

char *my_argv[] = { "man", "cat",  NULL };

main(argc, argv)
char *argv[];
{
    Tty tty;
    Frame frame;
    char   locale_env[30];

    xv_init( XV_USE_LOCALE,	TRUE,
	     XV_INIT_ARGC_PTR_ARGV, &argc, argv, 
	     NULL);
	     
    frame = (Frame)xv_create(NULL, FRAME, NULL);
    
    /*
     *   Make sure the shell has the same basic locale as the OpenWindows
     *   environment.
     */
    sprintf(locale_env, "LANG = %s", xv_get(frame, XV_LC_BASIC_LOCALE));
    putenv(locale_env);
    
    tty = (Tty)xv_create(frame, TTY,
        WIN_ROWS,       24,
        WIN_COLUMNS,    80,
        TTY_ARGV,       my_argv,
        NULL);

    window_fit(frame);
    xv_main_loop(frame);
}
