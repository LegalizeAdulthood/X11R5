#ifndef lint
#ifdef sccs
static char     sccsid[] = "%Z%%M% %I% %E%";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/param.h>
#include <stdio.h>
#include <strings.h>
#include <xview/win_input.h>
#include <xview/win_struct.h>
#include <xview_private/ntfy.h>
#include <xview_private/ndet.h>
#include <xview/notify.h>
#include <xview/win_notify.h>
#include <xview/defaults.h>
#include <X11/Xlib.h>
#include <xview_private/svr_impl.h>
#include <xview_private/win_kmdata.h>
#include <xview_private/draw_impl.h>
#include <xview_private/xv_color.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#ifdef OW_I18N
#include <xview_private/xv_i18n_impl.h>
#include <locale.h>
#endif OW_I18N

#ifdef OW_I18N
#define	LIB_LOCALE	"/lib/locale/"
#endif

extern char	*getenv();

Xv_private_data char *shell_prompt;


#ifdef _XV_DEBUG
Xv_private_data int server_gather_stats;
#endif

static void     server_init_atoms();
static Notify_value scheduler();
static Notify_func default_scheduler;	/* = 0 */

Xv_private Notify_value xv_input_pending();
Xv_private void xv_do_enqueued_input();

#ifdef OW_I18N
char		    *setlocale();
char		    *strdup();
XrmDatabase 	    XrmGetFileDatabase();
Xv_private	    void server_setlocale(), server_setlocale_defaults();
Xv_private int 	    xv_has_been_initialized();
extern XrmDatabase  defaults_rdb;
extern char	    *xv_app_name;
#endif OW_I18N

/* global default server parameters */
Xv_private_data Xv_Server xv_default_server;
Xv_private_data Xv_Screen xv_default_screen;
Xv_private_data Display *xv_default_display;

Pkg_private void server_init_modifier();

#ifdef OW_I18N
Xv_private_data char *app_name;
#endif OW_I18N

static unsigned int string_to_modmask();
static Server_atom_type save_atom();


static void
server_build_keymap_table(server)
    Server_info    *server;
{
    /* BUG: This should be improved to allow the user to bind in their own
     *      keymap, and semantic mapping table.  Or at least allow the user
     *      to rebind individual keys.			
     */
    server->xv_map = win_keymap;
    server->sem_map = win_keymap_sem;
    server->ascii_map = win_ascii_sem;
}


static int
svr_parse_display(display_name)
char *display_name;
{
 
	 /*
	 * The following code stolen form XConnectDisplay to parse the
	 * string and return the default screen number or 0.
	 */
	 char displaybuf[256];       /* Display string buffer */
	 register char *display_ptr; /* Display string buffer pointer */
	 register char *numbuf_ptr;  /* Server number buffer pointer */
	 char *screen_ptr;       /* Pointer for locating screen num */
	 char numberbuf[16];
	 char *dot_ptr = NULL;       /* Pointer to . before screen num */
	 char *index();
	 /*
	 * Find the ':' seperator and extract the hostname and the
	 * display number.
	 * NOTE - if DECnet is to be used, the display name is formatted
	 * as "host::number"
	 */
	 (void) strncpy(displaybuf, display_name, sizeof(displaybuf));
	 if ((display_ptr = index(displaybuf,':')) == NULL) return (-1);
	 *(display_ptr++) = '\0';
 
	 /* displaybuf now contains only a null-terminated host name, and
	 * display_ptr points to the display number.
	 * If the display number is missing there is an error. */
		  
	if (*display_ptr == '\0') return(-1);

	/*
	* Build a string of the form <display-number>.<screen-number> in
	* numberbuf, using ".0" as the default.
	*/
	screen_ptr = display_ptr;       /* points to #.#.propname */
	numbuf_ptr = numberbuf;         /* beginning of buffer */
	while (*screen_ptr != '\0') {
		if (*screen_ptr == '.') {       /* either screen or prop */
			if (dot_ptr) {          /* then found prop_name */
				screen_ptr++;
				break;
			}
			dot_ptr = numbuf_ptr;       /* found screen_num */
			*(screen_ptr++) = '\0';
			*(numbuf_ptr++) = '.';
		} else {
			*(numbuf_ptr++) = *(screen_ptr++);
		}
	}
	
	/*
	* If the spec doesn't include a screen number, add ".0" (or "0" if
    * only "." is present.)
	*/
	if (dot_ptr == NULL) {          /* no screen num or prop */
		dot_ptr = numbuf_ptr;
		*(numbuf_ptr++) = '.';
		*(numbuf_ptr++) = '0';
	} else {
		if (*(numbuf_ptr - 1) == '.')
			*(numbuf_ptr++) = '0';
	}
	*numbuf_ptr = '\0';

	/*
	* Return the screen number
	*/
	return(atoi(dot_ptr + 1));
}

/*ARGSUSED*/
Pkg_private int
server_init(parent, server_public, avlist)
    Xv_opaque       parent;
    Xv_Screen       server_public;
    Attr_avlist     avlist;
{
    register Server_info	*server = (Server_info *) NULL;
    Xv_server_struct 		*server_object;
    Attr_avlist    		attrs;

#ifdef OW_I18N
    char           		*home,	        /* pathname to home directory */
    				*server_name = NULL,
    				filename[MAXPATHLEN], *xv_message_dir;
#else
    char           		*home,	        /* pathname to home directory */
    				*server_name = NULL,
    				filename[128];	/* assume home filename
						   length < 118 chars */
#endif OW_I18N

    unsigned char		pmap[256];      /* pointer mapping list */
    int        default_screen_num;

#ifdef OW_I18N
    XrmQuarkList                quarks;
    XrmDatabase			new_db;
    /*
    XrmValue			xrm_result;
    char 			*strtype;
    char 			*lc_result;
    */
    extern int			_xv_use_locale;
#endif OW_I18N

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch (attrs[0]) {
	  case XV_NAME:
	    server_name = (char *) attrs[1];
	    *attrs = ATTR_NOP(*attrs);
	    break;
	  default:
	    break;
	}
    }
    if (!server_name)
    	server_name = (char *) defaults_get_string("server.name",
					       	   "Server.Name",
					           getenv("DISPLAY"));

    /* Allocate private data and set up forward/backward links. */
    server = (Server_info *) xv_alloc(Server_info);
    server->public_self = server_public;
    server_object = (Xv_server_struct *) server_public;
    server_object->private_data = (Xv_opaque) server;

    server->display_name = xv_strsave( server_name ? server_name : ":0" );

    if (!(server->xdisplay = server_init_x(server->display_name))) {
	goto Error_Return;
    }
    
    /* Create keycode maps */
    (void) server_build_keymap_table(server);

    if (notify_set_input_func(server->xdisplay, xv_input_pending,
			      win_server_fd(server->xdisplay))
	== NOTIFY_FUNC_NULL) {
	notify_perror("server_init");
	goto Error_Return;
    }

    /* Screen creation requires default server to be set. */
    if (!xv_default_server) {
	xv_default_server = server_public;
	xv_default_display = (Display *)server->xdisplay;
    }
    /*
     * Now that a server connection has been established, initialize the
     * defaults database. Note - This assumes that server_init will be called
     * only once per X11 server.
     */
    defaults_init_db();		/* init Resource Manager */
    if (home = getenv("HOME"))
	(void) strcpy(filename, home);
    (void) strcat(filename, "/.Xdefaults");

#ifndef OW_I18N
    defaults_load_db(filename);	/* load user's database */
    defaults_load_db((char *) NULL);	/* load server database */
#else
    /*
     *  Create a database from .Xdefaults and from the server property
     *  and stash its XID in the server. This used to be done in the
     *  Xv_database object, which has been removed pending design review
     */
    server->db = XrmGetFileDatabase(filename);
    if (((Display *)server->xdisplay)->xdefaults) {
	new_db = XrmGetStringDatabase(((Display *)server->xdisplay)->xdefaults);
        XrmMergeDatabases(new_db, &server->db);
    }
    defaults_rdb = server->db;

    /*
     *  Retrieve the locale announcers from the environment
     */

    server->basiclocale = NULL;
    server->displaylang = NULL;
    server->inputlang = NULL;
    server->numeric = NULL;
    server->timeformat = NULL;
    server->localedir = NULL;

    if (_xv_use_locale) {
	server_setlocale(server);

	/*
	 *  Now that we have the locale announcers from the environment
	 *  override w/ the locale information from the defaults 
	 */

	server_setlocale_defaults(server);
	
	/*
	 *  Override locale from env and Xrm w/ function parameters
	 */

	for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch (attrs[0]) {
	      case XV_LC_BASIC_LOCALE:
		if (attrs[1]) {
		    if (server->basiclocale) {
			free(server->basiclocale);
		    }
		    server->basiclocale = strdup(attrs[1]);
		}
		break;

	      case XV_LC_DISPLAY_LANG:
		if (attrs[1]) {
		    if (server->displaylang) {
			free(server->displaylang);
		    }
		    server->displaylang = strdup(attrs[1]);
		}
		break;

	      case XV_LC_INPUT_LANG:
		if (attrs[1]) {
		    if (server->inputlang) {
			free(server->inputlang);
		    }
		    server->inputlang = strdup(attrs[1]);
		}
		break;

	      case XV_LC_NUMERIC:
		if (attrs[1]) {
		    if (server->numeric) {
			free(server->numeric);
		    }
		    server->numeric = strdup(attrs[1]);
		}
		break;

	      case XV_LC_TIME_FORMAT:
		if (attrs[1]) {
		    if (server->timeformat) {
			free(server->timeformat);
		    }
		    server->timeformat = strdup(attrs[1]);
		}
		break;

	      case XV_LOCALE_DIR:
		if (attrs[1]) {
		    if (server->localedir) {
			free(server->localedir);
		    }
		    server->localedir = strdup(attrs[1]);
		}
		break;

	      default:
		break;
	    }
	}

	if (!setlocale(LC_ALL, server->basiclocale)) {
	    if (server->basiclocale) {
		free(server->basiclocale);
	    }
	    server->basiclocale = strdup("C");
	    setlocale(LC_ALL, "C");
	    setlocale(LC_CTYPE, "C");
	    xv_error(NULL,
		 ERROR_SEVERITY, ERROR_RECOVERABLE,
		 ERROR_STRING, "XV_LC_BASIC_LOCALE invalid",
		 ERROR_PKG, SERVER,
		 NULL);
	} else {
	    setlocale(LC_CTYPE, server->basiclocale);
	}

	if (!setlocale(LC_MESSAGES, server->displaylang)) {
	    if (server->displaylang) {
		free(server->displaylang);
	    }
	    server->displaylang = strdup("C");
	    setlocale(LC_MESSAGES, "C");
	    xv_error(NULL,
		 ERROR_SEVERITY, ERROR_RECOVERABLE,
		 ERROR_STRING, "XV_LC_DISPLAY_LANG invalid",
		 ERROR_PKG, SERVER,
		 NULL);
	}

	if (!setlocale(LC_NUMERIC, server->numeric)) {
	    if (server->numeric) {
		free(server->numeric);
	    }
	    server->numeric = strdup("C");
	    setlocale(LC_NUMERIC, "C");
	    xv_error(NULL,
		 ERROR_SEVERITY, ERROR_RECOVERABLE,
		 ERROR_STRING, "XV_LC_NUMERIC invalid",
		 ERROR_PKG, SERVER,
		 NULL);
	}

	if (!setlocale(LC_TIME, server->timeformat)) {
	    if (server->timeformat) {
		free(server->timeformat);
	    }
	    server->timeformat = strdup("C");
	    setlocale(LC_TIME, "C");
	    xv_error(NULL,
		 ERROR_SEVERITY, ERROR_RECOVERABLE,
		 ERROR_STRING, "XV_LC_TIME_FORMAT invalid",
		 ERROR_PKG, SERVER,
		 NULL);
	}

	if (server->inputlang == NULL)
	    server->inputlang = strdup(server->basiclocale);

    /*
     * Now that we know the locale, get the local specific resource files,
     * merge with server->db
     */
	
	
	if (home = getenv("OPENWINHOME")) {
	    xv_message_dir =
		malloc(strlen(home)+strlen(LIB_LOCALE)+1);
	    strcpy(xv_message_dir, home);
	    strcat(xv_message_dir, LIB_LOCALE);
	    bindtextdomain("xv_messages", xv_message_dir);
	    if (!server->localedir) {
		server->localedir = xv_message_dir;
	    } else {
		free(xv_message_dir);
	    }
	}

#ifdef notdef  /* This should be removed */
	if (server->localedir) {
	    bindtextdomain("", server->localedir);
	    if (xv_app_name) {
		strcpy(filename, server->localedir);
		strcat(filename, "/");
		strcat(filename, server->basiclocale);
		strcat(filename, "/app-defaults/");
		strcat(filename, xv_app_name);
		strcat(filename, ".db");

		if (new_db = XrmGetFileDatabase(filename)) {
		    XrmMergeDatabases(server->db, &new_db);
		    server->db = new_db;
		    defaults_rdb = server->db;
		}
	    }
	}
#endif

    } else {	/* if (_xv_use_locale) */
	server->basiclocale = "C";
	server->displaylang = "C";
	server->inputlang = "C";
	server->numeric = "C";
	server->timeformat = "C";
    }
#endif OW_I18N

    default_screen_num = svr_parse_display(server->display_name);
    server->screens[default_screen_num] = xv_create(server_public, SCREEN,
			SCREEN_NUMBER, default_screen_num,
			0);

    if (!server->screens[default_screen_num]) {
		goto Error_Return;
    }

    if (xv_default_server != server_public) {
	(void) XV_SL_ADD_AFTER(SERVER_PRIVATE(xv_default_server),
			SERVER_PRIVATE(xv_default_server), server);
    } else {
	XV_SL_INIT(server);
	xv_default_screen = server->screens[default_screen_num];
	(void) xv_set_scheduler();
    }

    /* Used in atom hash table */
    server->atom_context = (XID) XAllocID((Display *)server->xdisplay);
    server_init_atoms(server_public);
    selection_init_agent(server_public, server->screens[default_screen_num]);
    server_init_modifier(server);

    /* Be prepared to handle a mouse with only one or two physical buttons */
    server->nbuttons = XGetPointerMapping(server->xdisplay, pmap, 256);
    if (server->nbuttons < 3)
	server->but_two_mod =
	      string_to_modmask(defaults_get_string("mouse.modifier.button2",
						    "Mouse.Modifier.Button2",
						    "Shift"));
    if (server->nbuttons < 2) 
	server->but_three_mod =
	      string_to_modmask(defaults_get_string("mouse.modifier.button3",
						    "Mouse.Modifier.Button3",
						    "Ctrl"));
#ifdef OW_I18N
    if (strcmp(server->inputlang, "C") != 0) {
	/* first release only one input method supported, hence this
  	   check; when we support multiple locale, each with an input
	   method, we should save the LC_CTYPE locale, set it what is
	   specified by inputlang, then re-set the saved LC_CTYPE locale */
	if (strcmp(server->inputlang,setlocale(LC_CTYPE,NULL))==0)
            (XIM) server->xim =
#ifdef notdef
		(XIM) XOpenIM(server->xdisplay, server->inputlang);
#else
		(XIM) XOpenIM(server->xdisplay, server->db, "openwindows", "OpenWindows");
#endif
	else
	    fprintf(stderr, "inputlang is different from LC_CTYPE!\n");
    }     
#endif OW_I18N
    return XV_OK;

Error_Return:
    if (server) {
	if (xv_default_server == server_public) {
	    xv_default_server = (Xv_Server) NULL;
	}
	free((char *) server);
    }
    return XV_ERROR;
}

#ifdef OW_I18N
void
server_setlocale (server)
    Server_info  *server;
{
    char	*lc_result;

    /*
     *  Set any locale categories that exist in the 
     *  environment in a POSIX compliant fashion.
     */

    (void) setlocale(LC_ALL, "");

    /*
     *  Copy any existing locale announcers into the server struct
     */
    if (lc_result = setlocale(LC_CTYPE, "")) {
	if (server->basiclocale) {
		free(server->basiclocale);
	}
	server->basiclocale = strdup(lc_result);
    }

    if (lc_result = setlocale(LC_MESSAGES, "")) {
	if (server->displaylang) {
		free(server->displaylang);
	}
	server->displaylang = strdup(lc_result);
    }

#ifdef notdef
    server->displaylang = strdup("C");
#endif

    if (lc_result = setlocale(LC_NUMERIC, "")) {
	if (server->numeric) {
		free(server->numeric);
	}
	server->numeric = strdup(lc_result);
    }
    if (lc_result = setlocale(LC_TIME, "")) {
	if (server->timeformat) {
		free(server->timeformat);
	}
	server->timeformat = strdup(lc_result);
    }
}

void
server_setlocale_defaults (server)
    Server_info  *server;
{
    char	*strtype;
    XrmValue	xrm_result;

    xrm_result.size = 0;
    xrm_result.addr = NULL;

    if (XrmGetResource(server->db, "basicLocale", "basicLocale",
       &strtype, &xrm_result)) {
	if (server->basiclocale) {
		free(server->basiclocale);
	}
	server->basiclocale = strdup((char *) xrm_result.addr);
    }

    xrm_result.size = 0;
    xrm_result.addr = NULL;
    if (XrmGetResource(server->db, "displayLang", "displayLang",
       &strtype, &xrm_result)) {
	if (server->displaylang) {
		free(server->displaylang);
	}
	server->displaylang = strdup((char *) xrm_result.addr);
    }

    xrm_result.size = 0;
    xrm_result.addr = NULL;
    if (XrmGetResource(server->db, "inputLang", "inputLang",
       &strtype, &xrm_result)) {
	if (server->inputlang) {
		free(server->inputlang);
	}
	server->inputlang = strdup((char *) xrm_result.addr);
    }
    
    xrm_result.size = 0;
    xrm_result.addr = NULL;
    if (XrmGetResource(server->db, "numeric", "numeric",
       &strtype, &xrm_result)) {
	if (server->numeric) {
		free(server->numeric);
	}
	server->numeric = strdup((char *) xrm_result.addr);
    }

    xrm_result.size = 0;
    xrm_result.addr = NULL;
    if (XrmGetResource(server->db, "timeFormat", "timeFormat",
       &strtype, &xrm_result)) {
	if (server->timeformat) {
		free(server->timeformat);
	}
	server->timeformat = strdup((char *) xrm_result.addr);
    }
}
#endif OW_I18N

Pkg_private int
server_destroy(server_public, status)
    Xv_Server       server_public;
    Destroy_status  status;
{
    /*
     * The Notifier knows about both screen and server objects.  When the
     * entire process is dying, the Notifier calls the destroy routines for
     * the objects in an arbitrary order.  We attempt to change the ordering
     * so that the screen(s) are destroyed before the server(s), so that the
     * screen(s) can always assume that the server(s) are valid. In addition,
     * destruction of a server causes destruction of every object attached to
     * that server.  [BUG ALERT!  Not currently implemented.]
     */
    Server_info    *server = SERVER_PRIVATE(server_public);
    int             i;

    /* Give screens a chance to clean up. */
    for (i = 0; i < MAX_SCREENS; i++)
	if (server->screens[i])
	    if (xv_destroy(server->screens[i]) == XV_ERROR)
		return XV_ERROR;

    switch (status) {
      case DESTROY_PROCESS_DEATH:
      case DESTROY_CLEANUP:
	XCloseDisplay(server->xdisplay);
	/* Remove the client from the notifier. */
	(void) notify_remove(server->xdisplay);
	server->xdisplay = 0;
	if (xv_default_server == server_public) {
	    /* Remove our scheduler else will deref null server */
	    (void) notify_set_scheduler_func(default_scheduler);
	    xv_default_server = (Xv_Server) NULL;
	    xv_default_display = (Display *) NULL;
	    xv_default_screen = (Xv_Screen) NULL;
	}
	if (status == DESTROY_CLEANUP) {
	    xv_free(XV_SL_REMOVE(SERVER_PRIVATE(server_public), server));
	    xv_free(server->display_name);
	}			/* else waste of time fixing up heap or list
				 * links. */
#ifdef OW_I18N
        XCloseIM(server->xim);
        server->xim = NULL;
#endif OW_I18N
	break;
      default:
	break;
    }

    return XV_OK;
}

Xv_private void
xv_scheduler_internal()
{
    Server_info    *server;

    /* flush any output now */
    XV_SL_TYPED_FOR_ALL(SERVER_PRIVATE(xv_default_server), server,
			Server_info *) {
	xv_do_enqueued_input(server->xdisplay, FALSE);
	XFlush(server->xdisplay);	/* flush the output buffer */
    }
}

/*
 * invoke the default scheduler, then flush all servers.
 */
static          Notify_value
scheduler(n, clients)
    int             n;
    Notify_client   clients[];
{
    Notify_value    status = (default_scheduler) (n, clients);

    /* If xv_default_server is NULL we return because, xv_scheduler_internal()
     * dereferences it.  The problem is that default_scheduler will
     * process the xv_destroy(server) (nulling xv_default_server). 
     * The second problem here is that xv_scheduler_internal assumes that
     * there will always be an xv_default_server.  This is not true.  In
     * a multi server env, the xv_default_server could be destroyed but
     * other server will continue to be around to process events.
     */
    if (!xv_default_server)
	return status;

    /*
     * WARNING: we only want to process events from servers when the notifier
     * is ready to run, not whenever the notifier gets called (e.g. as a
     * result of xv_destroy() calling notify_post_destroy()). The notifier is
     * ready to run either after xv_main_loop() calls notify_start(), or
     * after the client calls notify_do_dispatch() or notify_dispatch().
     */
    if ((status == NOTIFY_DONE) &&
	(ndet_flags & (NDET_STARTED | NDET_DISPATCH)))
	xv_scheduler_internal();

    return status;
}

static int
xv_set_scheduler()
{
    /*
     * register a scheduler and an input handler with the notifier for this
     * process.
     */
    default_scheduler = notify_set_scheduler_func(scheduler);
    if (default_scheduler == NOTIFY_FUNC_NULL) {
	notify_perror("xv_set_scheduler");
	return XV_ERROR;
    }
    return XV_OK;
}

static void
server_init_atoms(server_public)
    Xv_Server       server_public;
{
    Server_info    *server = SERVER_PRIVATE(server_public);
    Atom            atom;

    /*
     * do not create the SERVER_JOURNAL_ATOM atom if it does not already
     * exists
     */
    atom = XInternAtom(server->xdisplay, "JOURNAL_SYNC", TRUE);
    if (atom == BadValue || atom == BadAlloc) {
	xv_error(NULL,
		 ERROR_SEVERITY, ERROR_NON_RECOVERABLE, 
		 ERROR_STRING,
#ifdef OW_I18N
		 XV_I18N_MSG("xv_messages", "Can't create SERVER_JOURNAL_ATOM atom"),
#else
		 "Can't create SERVER_JOURNAL_ATOM atom",
#endif OW_I18N
		 ERROR_PKG, SERVER,
		 0);
    }
    if (atom == None) {		/* not in journalling mode */
	server->journalling = FALSE;
    } else {			/* in journalling mode */
	int             status, actual_format;
	long            nitems, bytes;
	Atom            actual_type;
	char           *data;	/* default prompt */
	char           *shell_ptr;
	shell_prompt = (char *) calloc(40, sizeof(char));

	/* check to see if this property hangs of the root window */

	status = XGetWindowProperty(server->xdisplay,
			    DefaultRootWindow((Display *) server->xdisplay),
				atom, 0, 2, False, XA_INTEGER, &actual_type,
				    &actual_format, &nitems, &bytes, &data);

	if (status != Success || actual_type == None) {
	    server->journalling = FALSE;
	    XFree(data);
	} else {
	    server->journalling = TRUE;
	    if ((shell_ptr = getenv("PROMPT")) == NULL) {
		shell_prompt[0] = '%';
	    } else {
		(void) strcpy(shell_prompt, shell_ptr);
	    }
	    (void) xv_set(server_public, SERVER_JOURNAL_SYNC_ATOM, atom, NULL);
	}
    }
}

Xv_private      Server_atom_type
server_get_atom_type(server_public, atom)
    Xv_Server       server_public;
    Atom            atom;
{
    Server_atom_type    type;
    Server_info        *server = SERVER_PRIVATE(server_public);

    if (XFindContext(server->xdisplay, server->atom_context, 
		     (XContext) atom, &type) != XCNOENT)
	return ((Server_atom_type) type);
    else {
	char *atomName;

	if ((int) atom <= XA_LAST_PREDEFINED)      /* Cache predefined atoms */
		return (save_atom(server, atom, SERVER_WM_UNKNOWN_TYPE));

	atomName = XGetAtomName(server->xdisplay, atom);

	if (!strcmp(atomName, "_OL_WIN_ATTR"))
		type = save_atom(server, atom, SERVER_WM_WIN_ATTR_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_ADD"))
		type = save_atom(server, atom, SERVER_WM_ADD_DECOR_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_DEL"))
		type = save_atom(server, atom, SERVER_WM_DELETE_DECOR_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_CLOSE"))
		type = save_atom(server, atom, SERVER_WM_DECOR_CLOSE_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_FOOTER"))
		type = save_atom(server, atom, SERVER_WM_DECOR_FOOTER_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_RESIZE"))
		type = save_atom(server, atom, SERVER_WM_DECOR_RESIZE_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_HEADER"))
		type = save_atom(server, atom, SERVER_WM_DECOR_HEADER_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_OK"))
		type = save_atom(server, atom, SERVER_WM_DECOR_OK_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_PIN"))
		type = save_atom(server, atom, SERVER_WM_DECOR_PIN_TYPE);
	else if (!strcmp(atomName, "_OL_SCALE_SMALL"))
		type = save_atom(server, atom, SERVER_WM_SCALE_SMALL_TYPE);
	else if (!strcmp(atomName, "_OL_SCALE_MEDIUM"))
		type = save_atom(server, atom, SERVER_WM_SCALE_MEDIUM_TYPE);
	else if (!strcmp(atomName, "_OL_SCALE_LARGE"))
		type = save_atom(server, atom, SERVER_WM_SCALE_LARGE_TYPE);
	else if (!strcmp(atomName, "_OL_SCALE_XLARGE"))
		type = save_atom(server, atom, SERVER_WM_SCALE_XLARGE_TYPE);
	else if (!strcmp(atomName, "_OL_PIN_STATE"))
		type = save_atom(server, atom, SERVER_WM_PIN_STATE_TYPE);
	else if (!strcmp(atomName, "_OL_WIN_BUSY"))
		type = save_atom(server, atom, SERVER_WM_WIN_BUSY_TYPE);
	else if (!strcmp(atomName, "_OL_WINMSG_STATE"))
		type = save_atom(server, atom, SERVER_WM_WINMSG_STATE_TYPE);
	else if (!strcmp(atomName, "_OL_WINMSG_ERROR"))
		type = save_atom(server, atom, SERVER_WM_WINMSG_ERROR_TYPE);
	else if (!strcmp(atomName, "_OL_WT_BASE"))
		type = save_atom(server, atom, SERVER_WM_WT_BASE_TYPE);
	else if (!strcmp(atomName, "_OL_WT_CMD"))
		type = save_atom(server, atom, SERVER_WM_WT_CMD_TYPE);
	else if (!strcmp(atomName, "_OL_WT_PROP"))
		type = save_atom(server, atom, SERVER_WM_WT_PROP_TYPE);
	else if (!strcmp(atomName, "_OL_WT_HELP"))
		type = save_atom(server, atom, SERVER_WM_WT_HELP_TYPE);
	else if (!strcmp(atomName, "_OL_WT_NOTICE"))
		type = save_atom(server, atom, SERVER_WM_WT_NOTICE_TYPE);
	else if (!strcmp(atomName, "_OL_WT_OTHER"))
		type = save_atom(server, atom, SERVER_WM_WT_OTHER_TYPE);
	else if (!strcmp(atomName, "_OL_MENU_FULL"))
		type = save_atom(server, atom, SERVER_WM_MENU_FULL_TYPE);
	else if (!strcmp(atomName, "_OL_MENU_LIMITED"))
		type = save_atom(server, atom, SERVER_WM_MENU_LIMITED_TYPE);
	else if (!strcmp(atomName, "_OL_NONE"))
		type = save_atom(server, atom, SERVER_WM_NONE_TYPE);
	else if (!strcmp(atomName, "_OL_PIN_IN"))
		type = save_atom(server, atom, SERVER_WM_PIN_IN_TYPE);
	else if (!strcmp(atomName, "_OL_PIN_OUT"))
		type = save_atom(server, atom, SERVER_WM_PIN_OUT_TYPE);
	else if (!strcmp(atomName, "WM_TAKE_FOCUS"))
		type = save_atom(server, atom, SERVER_WM_TAKE_FOCUS_TYPE);
	else if (!strcmp(atomName, "XV_DO_DRAG_MOVE"))
		type = save_atom(server, atom, SERVER_DO_DRAG_MOVE_TYPE);
	else if (!strcmp(atomName, "XV_DO_DRAG_COPY"))
		type = save_atom(server, atom, SERVER_DO_DRAG_COPY_TYPE);
	else if (!strcmp(atomName, "XV_DO_DRAG_LOAD"))
		type = save_atom(server, atom, SERVER_DO_DRAG_LOAD_TYPE);
	else if (!strcmp(atomName, "_OL_WIN_DISMISS"))
		type = save_atom(server, atom, SERVER_WM_DISMISS_TYPE);
	else if (!strcmp(atomName, "WM_SAVE_YOURSELF"))
		type = save_atom(server, atom, SERVER_WM_SAVE_YOURSELF_TYPE);
	else if (!strcmp(atomName, "WM_PROTOCOLS"))
		type = save_atom(server, atom, SERVER_WM_PROTOCOLS_TYPE);
	else if (!strcmp(atomName, "WM_DELETE_WINDOW"))
		type = save_atom(server, atom, SERVER_WM_DELETE_WINDOW_TYPE);
	else if (!strcmp(atomName, "WM_COMMAND"))
		type = save_atom(server, atom, SERVER_WM_COMMAND_TYPE);
	else if (!strcmp(atomName, "WM_CHANGE_STATE"))
		type = save_atom(server, atom, SERVER_WM_CHANGE_STATE_TYPE);
	else if (!strcmp(atomName, "_OL_DFLT_BIN"))
		type = save_atom(server, atom, SERVER_WM_DEFAULT_BUTTON_TYPE);
#ifdef OW_I18N
	else if (!strcmp(atomName, "COMPOUND_TEXT"))
		type = save_atom(server, atom, SERVER_COMPOUND_TEXT_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_IMSTATUS"))
		type = save_atom(server, atom, SERVER_WM_DECOR_IMSTATUS_TYPE);
	else if (!strcmp(atomName, "_OL_WINMSG_IMSTATUS"))
		type = save_atom(server, atom, SERVER_WM_WINMSG_IMSTATUS_TYPE);
	else if (!strcmp(atomName, "_OL_WINMSG_IMPREEDIT"))
		type = save_atom(server, atom, SERVER_WM_WINMSG_IMPREEDIT_TYPE);
#endif OW_I18N
	else 
		type = save_atom(server, atom, SERVER_WM_UNKNOWN_TYPE);

	XFree(atomName);
	return ((Server_atom_type) type);
    }
}

static Server_atom_type
save_atom(server, atom, type)
Server_info	*server;
Atom		 atom;
Server_atom_type type;
{
	(void) XSaveContext(server->xdisplay, server->atom_context,
		     (XContext) atom, (caddr_t) type);
	server->atom[(int) type] = (Xv_opaque) atom;
	return (type); 
}




/*
 * BUG:  use_default_mapping should be set by comparing the default keycode
 * to keysym table.
 */
Xv_private void
server_journal_sync_event(server_public, type)
    Xv_Server       server_public;
    int             type;

{
    Server_info    *server = SERVER_PRIVATE(server_public);
    Atom            sync_atom = (Atom) xv_get(server_public, 
				     (Attr_attribute)SERVER_JOURNAL_SYNC_ATOM);
    XEvent          send_event;
    XClientMessageEvent *cme = (XClientMessageEvent *) & send_event;
    unsigned int    mask;
    Display        *dpy = (Display *) server->xdisplay;
    /*
     * Xv_Drawable_info       *info;
     */

    cme->type = ClientMessage;
    cme->serial = ClientMessage;/* should prob leave this alone */
    cme->send_event = 1;
    cme->display = dpy;
    /* get the xid of the root window -- not 100% correct */
    /*
     * DRAWABLE_INFO(xv_get(xv_get(server_public,SERVER_NTH_SCREEN,0),XV_ROOT),
     * info); cme->window = xv_xid(info); */
    cme->window = DefaultRootWindow((Display *) server->xdisplay),
    cme->message_type = sync_atom;
    cme->format = 32;
    cme->data.l[0] = type;
    mask = 0;
    XSync(dpy, 0);		/* make sure journal process has been
				 * scheduled and is waiting for the sync
				 * event */
    (void) XSendEvent(dpy, cme->window, 0, mask, cme);
    XSync(dpy, 0);		/* make sure journal event has occurred */
}

xv_string_to_rgb(buffer, red, green, blue)
    char            *buffer;
    unsigned char   *red;
    unsigned char   *green;
    unsigned char   *blue;

{
        int     hex_buffer;
        unsigned char   *conv_ptr;
        (void) sscanf(buffer, "#%6x", &hex_buffer);

        conv_ptr = (unsigned char *) &hex_buffer;
        *red = conv_ptr[1];
        *green = conv_ptr[2];
        *blue = conv_ptr[3];
}

static unsigned int
string_to_modmask(str)
char *str;
{
	if (strcmp(str, "Shift") == 0) 
		return ShiftMask;
	else if (strcmp(str, "Ctrl") == 0) 
		return ControlMask;
	else if (strcmp(str, "Meta") == 0) 
		return Mod1Mask;
	else  { /* Punt for now, just return Mod1Mask */
		/* What really needs to be done here is look up the 
		   modifier mapping from the server and add the new modifier
		   keys we are now interested in.   			     */
		xv_error(NULL,
			 ERROR_STRING,
#ifdef OW_I18N
		  XV_I18N_MSG("xv_messages", "Only support Shift, Ctrl and Meta as mouse button modifiers"),
#else
		  "Only support Shift, Ctrl and Meta as mouse button modifiers",
#endif OW_I18N
			 ERROR_PKG, SERVER,
			 0);
		return(Mod1Mask);
	}
}

Pkg_private Xv_opaque
server_intern_atom(server, type, atom_name)
register Server_info *server;
Server_atom_type      type;
char 		     *atom_name;
{
	if (server->atom[(int) type] == NULL) {
	    server->atom[(int) type] = (Xv_opaque) XInternAtom(server->xdisplay,
							       atom_name, FALSE);
		if ((Atom) server->atom[(int) type] == BadValue ||
		    (Atom) server->atom[(int) type] == BadAlloc) {
			xv_error(NULL,
				 ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
				 ERROR_STRING,
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Can't create/get window manager atom"),
#else
				"Can't create/get window manager atom",
#endif OW_I18N
				 ERROR_PKG, SERVER,
				 0);
		}
		(void) XSaveContext(server->xdisplay, server->atom_context,
			     (XContext) server->atom[(int) type],
			     (caddr_t) type); 
	}
	return((Xv_opaque) server->atom[(int) type]);
}
