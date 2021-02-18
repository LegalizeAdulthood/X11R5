#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)xv_init.c 50.16 91/02/19";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifdef _XV_DEBUG
#include <xview_private/xv_debug.h>
#else
#include <stdio.h>
#endif
#include <xview/pkg.h>
#include <xview/xview_xvin.h>
#include <X11/Xlib.h>

#ifdef OW_I18N
#include <xview/server.h>
#include <xview/xv_i18n.h>
#endif OW_I18N

#define ERROR_MSG "Cannot open display on window server: "

/* FIXME: This should probably become integrated into the get/set paradigm */
int (*xv_error_proc) ();
int (*xv_x_error_proc)();
void (*xv_xlib_error_proc)();
extern void     xv_usage();

static int      xv_init_called;	/* = FALSE */


Xv_private void xv_init_x_pr();
Xv_private void xv_set_destroy_interposer();
Xv_private void xv_connection_error();
Xv_private int  xv_handle_xio_errors();
Xv_private void xv_x_error_handler();
Xv_private char *xv_base_name();

Xv_private_data char *xv_app_name;
#ifdef OW_I18N
char 			*getenv();
Xv_private_data char 	*xv_instance_app_name = NULL;
Xv_private_data wchar_t *xv_app_name_wcs;
Xv_private_data int	_xv_use_locale;
#endif OW_I18N

Xv_public_data Display *xv_default_display;
Xv_public_data Xv_Server xv_default_server;
Xv_public_data Xv_Screen xv_default_screen;


/*
 * Initialize XView.
 */
/*VARARGS*/
Xv_public	Xv_object
xv_init(va_alist)
va_dcl
{
    Attr_attribute     avlist[ATTR_STANDARD_SIZE];
    register Attr_avlist attrs;
    va_list         valist;
    void            (*help_proc) () = xv_usage;
    int             parse_result = 0,
                    argc = 0;
    char          **argv,
                   *server_name = (char *) NULL;
    Attr_avlist     attrs_start;
    Xv_object	    server;
#ifdef OW_I18N
    char	    *basiclocale = (char *) NULL;
    char	    *displaylang = (char *) NULL;
    char	    *inputlang = (char *) NULL;
    char	    *numeric = (char *) NULL;
    char 	    *timeformat = (char *) NULL;
    char 	    *localedir = (char *) NULL;
#endif OW_I18N

    /* can only be called once */
    if (xv_init_called)
	return((Xv_object)NULL);

    xv_init_called = TRUE;

    (void) xv_set_destroy_interposer();

    xv_error_proc = (int (*) ()) 0;
    xv_x_error_proc = (int (*) ()) 0;

    /* initialize the pixrect-to-x rop op table */
    xv_init_x_pr();
    
    /* silence the shut-down error messages, can turn on with option flag */
    (void) XSetIOErrorHandler(xv_handle_xio_errors);

/*
#ifdef OW_I18N
    localedir = getenv("XV_LOCALE_DIR");
#endif OW_I18N
*/

#ifdef ATTR_MAKE
    va_start(valist);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);
    attrs_start = (Attr_avlist) avlist;
#else
    attrs_start = (Attr_avlist) &va_alist;
    if (*attrs_start == (Attr_attribute) ATTR_LIST) {
	va_start(valist);
	(void) attr_make(avlist, ATTR_STANDARD_SIZE, valist);
	va_end(valist);
	attrs_start = (Attr_avlist) avlist;
    }
#endif				/* ATTR_MAKE */

    /*
     *  Get argv for command line parse below
     */

    for (attrs = attrs_start; *attrs; attrs = attr_next(attrs)) {
	switch ((Xv_attr) attrs[0]) {
	  case XV_INIT_ARGS:
	    argc = (int) attrs[1];
	    argv = (char **) attrs[2];
#ifdef OW_I18N
	    if (xv_app_name_wcs) {
		free(xv_app_name_wcs);
	    }
	    if (argv[0])
	    {
	      xv_app_name = xv_base_name(argv[0]);
	      xv_app_name_wcs = mbstowcsdup(xv_app_name);
	    }
#endif OW_I18N
	    break;

	  case XV_INIT_ARGC_PTR_ARGV:
	    argc = *(int *) attrs[1];
	    argv = (char **) attrs[2];
#ifdef OW_I18N
	    if (xv_app_name_wcs) {
		free(xv_app_name_wcs);
	    }
	    if (argv[0])
	    {
	      xv_app_name = xv_base_name(argv[0]);
	      xv_app_name_wcs = mbstowcsdup(xv_app_name);
	    }
#endif OW_I18N
	    break;

	  default:
	    break;
	}
    }
    /*
     *  Preparse
     *		"-display <name>" for server creation
     *  and #ifdef OW_I18N
     *		"-name <name>" for application
     *		"-lc_basiclocale", etc for locale announcement
     *		"-xv_locale_dir" for location of locale specific info
     */
    for (; argc--; argv++) {
        if (strcmp(*argv, "-display") == 0 ||
            strcmp(*argv, "-Wr") == 0) {
            server_name = *++argv;

	    /* Added --argc to fix v2 bug */ 
	    --argc;
	    continue;
        }
#ifdef OW_I18N
	else if (strcmp(*argv, "-lc_basiclocale") == 0) {
	    basiclocale = *++argv;
	    --argc;
	    continue;
        }
	else if (strcmp(*argv, "-lc_displaylang") == 0) {
	    displaylang = *++argv;
	    --argc;
	    continue;
        }
	else if (strcmp(*argv, "-lc_inputlang") == 0) {
	    inputlang = *++argv;
	    --argc;
	    continue;
        }
	else if (strcmp(*argv, "-lc_numeric") == 0) {
	    numeric = *++argv;
	    --argc;
	    continue;
        }
	else if (strcmp(*argv, "-lc_timeformat") == 0) {
	    timeformat = *++argv;
	    --argc;
	    continue;
        }
/*
	else if (strcmp(*argv, "-xv_locale_dir") == 0) {
	    localedir = *++argv;
	    --argc;
	    continue;
	}
*/
#endif OW_I18N
    }

    /*
     *  Override any command line arguments with xv_init parameters
     */

#ifdef OW_I18N

    for (attrs = attrs_start; *attrs; attrs = attr_next(attrs)) {
        switch ((Xv_attr) attrs[0]) {

	  case XV_USE_LOCALE:
	    _xv_use_locale = (int)attrs[1];
	    break;

          case XV_LC_BASIC_LOCALE:
	    basiclocale = (char *)attrs[1];
            break;

	  case XV_LC_DISPLAY_LANG:
	    displaylang = (char *) attrs[1];
	    break;

	  case XV_LC_INPUT_LANG:
	    inputlang = (char *) attrs[1];
	    break;
	  
	  case XV_LC_NUMERIC:
	    numeric = (char *) attrs[1];
	    break;

	  case XV_LC_TIME_FORMAT:
	    timeformat = (char *) attrs[1];
	    break;

	  case XV_LOCALE_DIR:
	    localedir = (char *) attrs[1];
	    break;
	    
          default:
            break;
        }   
    }
#endif OW_I18N

    /*
     * Check if any SERVER object has been created.  If not, then create one
     * to make sure that we read the defaults database from the correct
     * server before we parse cmd-line args. (xv_parse_cmdline stores the
     * parsed flags in the defaults database.)
     */

    if (!xv_has_been_initialized()) {
	if (server_name) {
	    server = xv_create(XV_NULL, SERVER,
                                XV_NAME, server_name,
#ifdef OW_I18N
				XV_LC_BASIC_LOCALE, basiclocale,
				XV_LC_DISPLAY_LANG, displaylang,
				XV_LC_INPUT_LANG, inputlang,
				XV_LC_NUMERIC, numeric,
				XV_LC_TIME_FORMAT, timeformat,
				XV_LOCALE_DIR, localedir,
#endif OW_I18N
                                0);
	}
	else {
            server = xv_create(XV_NULL, SERVER,
#ifdef OW_I18N
				XV_LC_BASIC_LOCALE, basiclocale,
				XV_LC_DISPLAY_LANG, displaylang,
				XV_LC_INPUT_LANG, inputlang,
				XV_LC_NUMERIC, numeric,
				XV_LC_TIME_FORMAT, timeformat,
				XV_LOCALE_DIR, localedir,
#endif OW_I18N
                                0);
	}
	if (!server)
	    (void) xv_connection_error(server_name);
    }

    /* Note: XSetErrorHandler must be called after the server connection
     * has been established.  XSetErrorHandler() returns the current
     * X Error handler, which is now the defualt Xlib X Error Handler address.
     * Note: The error handler is defined by Xlib to be an int function, but
     * no use is made of the return value.  So, XView's X Error Handler
     * has been declared to be a void function.
     */
    xv_xlib_error_proc = (void (*)())
	XSetErrorHandler((int (*)())xv_x_error_handler);

    for (attrs = attrs_start; *attrs; attrs = attr_next(attrs)) {
	switch ((int) attrs[0]) {
	  case XV_USAGE_PROC:
	    help_proc = (void (*) ()) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case XV_INIT_ARGS:
	    argc = (int) attrs[1];
	    argv = (char **) attrs[2];

/*************************
	    if (xv_app_name) {
		free(xv_app_name);
	    }
#ifdef OW_I18N
	    if (xv_app_name_wcs) {
		free(xv_app_name_wcs);
	    }
	    if (argv[0])
	    {
	      xv_app_name = xv_base_name(argv[0]);
	      xv_app_name_wcs = mbstowcsdup(xv_app_name);
	    }
#else OW_I18N
*************************/
#ifndef OW_I18N
	    if (xv_app_name) {
		free(xv_app_name);
	    }

	    if (argv[0])
	      xv_app_name = xv_base_name(argv[0]);
#endif ~OW_I18N
	    parse_result = xv_parse_cmdline(xv_app_name, &argc, argv, FALSE);
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case XV_INIT_ARGC_PTR_ARGV:
	    argv = (char **) attrs[2];

/***********************
	    if (xv_app_name) {
		(void) free(xv_app_name);
	    }
#ifdef OW_I18N
	    if (xv_app_name_wcs) {
		free(xv_app_name_wcs);
	    }
	    if (argv[0])
	    {
	      xv_app_name = xv_base_name(argv[0]);
	      xv_app_name_wcs = mbstowcsdup(xv_app_name);
	    }
#else OW_I18N
***********************/
#ifndef OW_I18N
	    if (xv_app_name) {
		(void) free(xv_app_name);
	    }
	    if (argv[0])
	      xv_app_name = xv_base_name(argv[0]);
#endif ~OW_I18N
	    parse_result = xv_parse_cmdline(xv_app_name, 
					    (int *) attrs[1], argv, TRUE);
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case XV_ERROR_PROC:
	    xv_error_proc = (int (*) ()) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case XV_X_ERROR_PROC:
	    xv_x_error_proc = (int (*)()) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  default:
	    break;
	}
    }

#ifdef OW_I18N
    xv_instance_app_name = xv_app_name;
#endif OW_I18N

    if (parse_result == -1) {
	/* Following routine often, but not always, calls exit(). */
	help_proc(xv_app_name);
    }

    return (server);
}

/*ARGSUSED*/
Xv_private int
xv_handle_xio_errors(display)
    Display *display;
{
    /* do nothing, be quiet */
    exit(0);
}

Xv_private void
xv_connection_error(server_name)
    char *server_name;
{
    char *error_string;
 
    server_name = (server_name) ? server_name :
                                (char *) defaults_get_string("server.name",
                                                             "Server.Name",
                                                             getenv("DISPLAY"));
 
    error_string =malloc(strlen(ERROR_MSG) + strlen(server_name) + 2);
    strcpy(error_string, ERROR_MSG);
    strcat(error_string, server_name);
    xv_error(NULL,
                 ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
                 ERROR_STRING, error_string,
                 ERROR_PKG, SERVER,
                 0);
    /* NOTREACHED */
    free(error_string);
}

/*
 * xv_base_name - return the base filename sans the path
 */
Xv_private char *
xv_base_name(fullname)
char *fullname;
{
	char *base_name;
	char *start;
	
	/* Find the beginning of the base name */
	start = fullname + strlen(fullname);
	while ((*start != '/') && (start != fullname))
	  start--;
	if (*start == '/') start++;
	base_name = malloc(strlen(start) + 1);
	(void) strcpy(base_name, start);
	return(base_name);
}

