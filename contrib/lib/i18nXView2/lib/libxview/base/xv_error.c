#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)xv_error.c 50.6 90/12/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <stdio.h>
#include <xview/pkg_public.h>
#include <X11/Xlib.h>

/* Unix system error variables */
extern int      sys_nerr;
extern char    *sys_errlist[];
extern int      errno;

#ifdef OW_I18N
#include <xview_private/xv_i18n_impl.h>
char		*dgettext();
#endif

Xv_private_data char *xv_app_name;
Xv_private int (*xv_error_proc) ();

/*
 * xv_error_format - Process the avlist and generate a formatted error message
 * of up to ERROR_MAX_STRING_SIZE characters, which has one NEW LINE character
 * at the end.
 */
Xv_public char *
xv_error_format(object, avlist)
    Xv_object       object;
    Attr_avlist	    avlist;
{
    static char	    msg[ERROR_MAX_STRING_SIZE]; /* formatted error message */

    Attr_avlist     attrs;
#define BUFSIZE 128
    char            buf[BUFSIZE];	/* to hold reason for error */
    Display	   *dpy;
    char	   *error_string = NULL;
    Error_layer	    layer = ERROR_TOOLKIT;
#define LMSGSIZE 128
    char	    layer_msg[LMSGSIZE]; /* to hold layer message */
    char	   *layer_name;
    int		    length;
    char           *pkg_name = NULL;
    char	   *severity = "warning";
    XErrorEvent    *xerror = NULL;

    buf[0] = 0;
    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch ((int) attrs[0]) {

	  case ERROR_BAD_ATTR:
#ifdef OW_I18N
	    sprintf(buf, XV_I18N_MSG("xv_messages", "bad attribute, %s"),
		 attr_name(attrs[1]));
#else
	    sprintf(buf, "bad attribute, %s", attr_name(attrs[1]));
#endif
	    break;

	  case ERROR_BAD_VALUE:
#ifdef OW_I18N
	    sprintf(buf, XV_I18N_MSG("xv_messages",
		"bad value (0x%x) for attribute %s"), attrs[1], attr_name(attrs[2]));
#else
	    sprintf(buf, "bad value (0x%x) for attribute %s",
		 attrs[1], attr_name(attrs[2]));
#endif OW_I18N
	    break;

	  case ERROR_CANNOT_GET:
#ifdef OW_I18N
	    sprintf(buf, XV_I18N_MSG("xv_messages", "cannot get %s"),
		 attr_name(attrs[1]));
#else
	    sprintf(buf, "cannot get %s", attr_name(attrs[1]));
#endif
	    break;

	  case ERROR_CANNOT_SET:
#ifdef OW_I18N
	    sprintf(buf, XV_I18N_MSG("xv_messages", "cannot set %s"),
		 attr_name(attrs[1]));
#else
	    sprintf(buf, "cannot set %s", attr_name(attrs[1]));
#endif OW_I18N
	    break;

	  case ERROR_CREATE_ONLY:
#ifdef OW_I18N
	    sprintf(buf, XV_I18N_MSG("xv_messages", "%s only valid in xv_create"),
		    attr_name(attrs[1]));
#else
	    sprintf(buf, "%s only valid in xv_create", attr_name(attrs[1]));
#endif OW_I18N
	    break;

	  case ERROR_INVALID_OBJECT:
#ifdef OW_I18N
	    sprintf(buf, XV_I18N_MSG("xv_messages", "invalid object (%s)"),
		 (char *) attrs[1]);
#else
	    sprintf(buf, "invalid object (%s)", (char *) attrs[1]);
#endif OW_I18N
	    break;

	  case ERROR_LAYER:
	    if ((unsigned int) attrs[1] <= (unsigned int) ERROR_PROGRAM)
	        layer = (Error_layer) attrs[1];
	    break;
	  case ERROR_PKG:
	    pkg_name = ((Xv_pkg *) attrs[1])->name;
	    break;
	  case ERROR_SERVER_ERROR:
	    xerror = (XErrorEvent *) attrs[1];
	    break;
	  case ERROR_SEVERITY:
	    if ((Error_severity) attrs[1] == ERROR_NON_RECOVERABLE)
		severity = "error";
	    break;
	  case ERROR_STRING:
	    error_string = (char *) attrs[1];
	    break;
	}
    }

    switch (layer) {

      case ERROR_SYSTEM:
#ifdef OW_I18N
	layer_name = XV_I18N_MSG("xv_messages", "System");
#else
	layer_name = "System";
#endif OW_I18N
	if ((int) errno < sys_nerr)
	    sprintf(layer_msg, "%s", sys_errlist[(int) errno]);
	else
#ifdef OW_I18N
	    sprintf(layer_msg, XV_I18N_MSG("xv_messages", "unix error %d"),
		 (int) errno);
#else
	    sprintf(layer_msg, "unix error %d", (int) errno);
#endif OW_I18N
	break;

      case ERROR_SERVER:
#ifdef OW_I18N
	layer_name = XV_I18N_MSG("xv_messages", "Server");
#else
	layer_name = "Server";
#endif OW_I18N
	if (xerror) {
	    dpy = xerror->display;
	    XGetErrorText(dpy, (int) xerror->error_code, layer_msg, LMSGSIZE);
	} else
#ifdef OW_I18N
	    strcpy(layer_msg, XV_I18N_MSG("xv_messages", "error unknown"));
#else
	    strcpy(layer_msg, "error unknown");
#endif OW_I18N
	break;

      case ERROR_TOOLKIT:
#ifdef OW_I18N
	layer_name = XV_I18N_MSG("xv_messages", "XView");
#else
	layer_name = "XView";
#endif OW_I18N
	layer_msg[0] = 0;
	break;

      case ERROR_PROGRAM:
	layer_name = xv_app_name;
	layer_msg[0] = 0;
	break;
    }
    if (!object)
	sprintf(msg, "%s %s:", layer_name, severity);
    else
#ifdef OW_I18N
	sprintf(msg, XV_I18N_MSG("xv_messages", "%s %s: Object 0x%x,"),
	    layer_name, severity, object);
#else
	sprintf(msg, "%s %s: Object 0x%x,", layer_name, severity, object);
#endif OW_I18N
    if (layer_msg[0]) {
	strcat(msg, " ");
	strcat(msg, layer_msg);
    }
    if (buf[0]) {
	strcat(msg, " ");
	strcat(msg, buf);
    }
    if (error_string) {
	/* Append ERROR_STRING, stripping off trailing New Lines. */
	length = strlen(error_string);
	while (length && error_string[length-1] == '\n')
	    length--;
	if (length) {
	    if (layer_msg[0] || buf[0])
		strcat(msg, ", ");
	    else
		strcat(msg, " ");
	    strncat(msg, error_string, length);
	}
    }
    if (pkg_name)
#ifdef OW_I18N
	sprintf(msg, XV_I18N_MSG("xv_messages", "%s (%s package)\n"),
		 msg, pkg_name);
#else
	sprintf(msg, "%s (%s package)\n", msg, pkg_name);
#endif OW_I18N
    else
	strcat(msg, "\n");
    return msg;
}


/*
 * xv_error_default - default error routine called by xv_error() (below).
 * Print the formatted error message generated by xv_error_format to stderr.
 * If ERROR_SEVERITY is ERROR_RECOVERABLE, xv_error_default returns to the
 * caller with XV_OK.  Otherwise, the program is aborted with an exit(1).
 */
Xv_public int
xv_error_default(object, avlist)
    Xv_object       object;
    Attr_avlist	    avlist;
{
    Attr_attribute *attrs;
    Error_severity severity = ERROR_RECOVERABLE;

    fprintf(stderr, "%s", xv_error_format(object, avlist));

    for (attrs = avlist; *attrs; attrs = attr_next(attrs))
	switch (attrs[0]) {
	  case ERROR_SEVERITY:
	    severity = (Error_severity) attrs[1];
	    break;
	}

    if (severity != ERROR_RECOVERABLE)
	exit(1);
    return XV_OK;
}


/*VARARGS*/
Xv_public int
xv_error(object, va_alist)
    Xv_object       object;
va_dcl
{
    va_list         valist;
    Attr_attribute  avarray[ATTR_STANDARD_SIZE];
    Attr_avlist	    avlist;

#ifdef ATTR_MAKE
    avlist = avarray;
    va_start(valist);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);
#else
    avlist = (Attr_avlist) &va_alist;
    if (*avlist == (Attr_attribute) ATTR_LIST) {
	avlist = avarray;
	va_start(valist);
	(void) attr_make(avlist, ATTR_STANDARD_SIZE, valist);
	va_end(valist);
    }
#endif				/* ATTR_MAKE */

    if (xv_error_proc)
	return ((*xv_error_proc) (object, avlist));
    else
	return (xv_error_default(object, avlist));
}
