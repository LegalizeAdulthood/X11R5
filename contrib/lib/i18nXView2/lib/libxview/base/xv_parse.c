#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)xv_parse.c 50.8 91/02/19";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Do standard parse on argv to defaults.
 */

#ifdef _XV_DEBUG
#include <xview_private/xv_debug.h>
#else
#include <stdio.h>
#endif
#include <xview/defaults.h>
#include <xview/pkg.h>
#include <xview/xv_error.h>
#include <xview/fullscreen.h>
#include <X11/Xlib.h>

#ifdef OW_I18N
#include <xview_private/xv_i18n_impl.h>
#endif

Xv_private int  list_fonts;

#ifdef _XV_DEBUG
Xv_private int  server_gather_stats;
int             sview_gprof_start;
#endif

typedef struct {
    char           *short_name, *long_name;
    char           *def_name[2];/* defaults name(s) */
    char            num_args;
}               Cmd_line_flag;

/*
 * WARNING: The enumeration Flag_name, the array cmd_line_flags, and the
 * procedure xv_usage must all be updated together whenever a new option is
 * added, and old option is deleted, or the order of the existing options is
 * permuted.
 */
typedef enum {
    FLAG_SCALE,
    FLAG_FONT,
    FLAG_X_FONT,
    FLAG_WIDTH,
    FLAG_HEIGHT,
    FLAG_SIZE,
    FLAG_POSITION,
    FLAG_GEOMETRY,
    FLAG_ICON_POSITION,
    FLAG_LABEL,
    FLAG_TITLE,
    FLAG_ENABLE_ICONIC,
    FLAG_DISABLE_ICONIC,
    FLAG_FOREGROUND_COLOR,
    FLAG_X_FOREGROUND_COLOR,
    FLAG_BACKGROUND_COLOR,
    FLAG_X_BACKGROUND_COLOR,
    FLAG_SET_DEFAULT_COLOR,
#ifdef REVERSE_VIDEO
    FLAG_ENABLE_REVERSE,
    FLAG_DISABLE_REVERSE,
#endif /* REVERSE_VIDEO */
    FLAG_ICON_IMAGE,
    FLAG_ICON_LABEL,
    FLAG_ICON_FONT,
    FLAG_DEFAULTS_ENTRY,
    FLAG_XRM_ENTRY,
    FLAG_HELP,
    FLAG_ENABLE_SYNC,
    FLAG_DISABLE_SYNC,
    FLAG_SERVER,
    FLAG_DISABLE_RETAINED,
    FLAG_DISABLE_XIO_ERROR_HANDLER,
    FLAG_FULLSCREEN_DEBUG,
    FLAG_FULLSCREEN_DEBUG_SERVER,
    FLAG_FULLSCREEN_DEBUG_PTR,
    FLAG_FULLSCREEN_DEBUG_KBD,
    FLAG_NO_SECURITY,
#ifdef _XV_DEBUG
    FLAG_LIST_FONTS,
    FLAG_DEBUG,
    FLAG_STATS,
    FLAG_GPROF_START,
#endif
#ifdef OW_I18N
    FLAG_LC_BASIC_LOCALE,
    FLAG_LC_DISPLAY_LANG,
    FLAG_LC_INPUT_LANG,
    FLAG_LC_NUMERIC,
    FLAG_LC_TIME_FORMAT,
#endif OW_I18N
}               Flag_name;

static Cmd_line_flag *find_cmd_flag();

static Cmd_line_flag cmd_line_flags[] = {
    "-Wx", "-scale", "window.scale", 0, 1,
    "-Wt", "-font", "font.name", 0, 1,
    "-fn", "", "font.name", 0, 1,
    "-Ww", "-width", "window.columns", 0, 1,
    "-Wh", "-height", "window.rows", 0, 1,
    "-Ws", "-size", "window.width", "window.height", 2,
    "-Wp", "-position", "window.x", "window.y", 2,
    "-WG", "-geometry", "window.geometry", 0, 1,
    "-WP", "-icon_position", "icon.x", "icon.y", 2,
    "-Wl", "-label", "window.header", 0, 1,
    "", "-title",    "window.header", 0, 1,
    "-Wi", "-iconic", "window.iconic", 0, 0,
    "+Wi", "+iconic", "window.iconic", 0, 0,
    "-Wf", "-foreground_color", "window.color.foreground", 0, 3,
    "-fg", "-foreground", "window.color.foreground", 0, 1,
    "-Wb", "-background_color", "window.color.background", 0, 3,
    "-bg", "-background", "window.color.background", 0, 1,
    "-Wg", "-set_default_color", "window.inheritcolor", 0, 0,
#ifdef REVERSE_VIDEO
    "-rv", "-reverse", "window.reverseVideo", 0, 0,
    "+rv", "+reverse", "window.reverseVideo", 0, 0,
#endif /* REVERSE_VIDEO */
    "-WI", "-icon_image", "icon.pixmap", 0, 1,
    "-WL", "-icon_label", "icon.footer", 0, 1,
    "-WT", "-icon_font", "icon.font.name", 0, 1,
    "-Wd", "-default", 0, 0, 2,
    "",    "-xrm", 0, 0, 1,
    "-WH", "-help", 0, 0, 0,
    "-sync", "-synchronous", "window.synchronous", 0, 0,
    "+sync", "+synchronous", "window.synchronous", 0, 0,
    "-Wr", "-display", "server.name", 0, 1,
    "-Wdr", "-disable_retained", "window.mono.disableRetained", 0, 0,
    "-Wdxio", "-disable_xio_error_handler", 0, 0, 0,
    "-Wfsdb", "-fullscreendebug", 0, 0, 0,
    "-Wfsdbs", "-fullscreendebugserver", 0, 0, 0,
    "-Wfsdbp", "-fullscreendebugptr", 0, 0, 0,
    "-Wfsdbk", "-fullscreendebugkbd", 0, 0, 0,
    "-WS", "-defeateventsecurity", 0, 0, 0,
#ifdef _XV_DEBUG
    "", "-list_fonts", 0, 0, 0,
    "", "-Xv_debug", 0, 0, 1,
    "", "-stats", 0, 0, 0,
    "", "-gprof_start", 0, 0, 0,
#endif
#ifdef OW_I18N
    "", "-lc_basiclocale", "lc.basiclocale", 0, 1,
    "", "-lc_displaylang", "lc.displaylang", 0, 1,
    "", "-lc_inputlang", "lc.inputlang", 0, 1,
    "", "-lc_numeric", "lc.numeric", 0, 1,
    "", "-lc_timeformat", "lc.timeformat", 0, 1,
    0, 0, 0, 0, 0
#endif OW_I18N
};

static Defaults_pairs known_scales[] = {
	"small", WIN_SCALE_SMALL,
	"Small", WIN_SCALE_SMALL,
	"medium", WIN_SCALE_MEDIUM,
	"Medium", WIN_SCALE_MEDIUM,
	"large", WIN_SCALE_LARGE,
	"Large", WIN_SCALE_LARGE,
	"extra_large", WIN_SCALE_EXTRALARGE,
	"Extra_Large", WIN_SCALE_EXTRALARGE,
	NULL, -1,
};

Xv_private void
xv_cmdline_scrunch(argc_ptr, argv, remove, n)
    int            *argc_ptr;
    char          **argv, **remove;
    int             n;
/*
 * Takes remove to remove+n-1 out of argv, which is assumed to be NULL
 * terminated, although no use is made of that assumption. The original argv
 * is required from the caller to avoid having to scan the list looking for
 * its end.
 */
{
#ifdef _XV_DEBUG
    if (*argc_ptr < n) {
#ifdef OW_I18N
	(void) fprintf(stderr,
	   XV_I18N_MSG("xv_messages",
			"xv_cmdline_scrunch: argc (%d) < count (%d)\n"), *argc_ptr, n);
#else
	(void) fprintf(stderr, "xv_cmdline_scrunch: argc (%d) < count (%d)\n",
			 *argc_ptr, n);
#endif OW_I18N
	return;
    }
    if (argv[*argc_ptr]) {
#ifdef OW_I18N
	(void) fprintf(stderr,
	   XV_I18N_MSG("xv_messages",
			"xv_cmdline_scrunch: argv[argc(%d)] (%d:%s) not NULL\n"),
		       *argc_ptr, argv[*argc_ptr], argv[*argc_ptr]);
#else
	(void) fprintf(stderr,
		"xv_cmdline_scrunch: argv[argc(%d)] (%d:%s) not NULL\n",
		       *argc_ptr, argv[*argc_ptr], argv[*argc_ptr]);
#endif OW_I18N
	return;
    }
#endif _XV_DEBUG
    *argc_ptr = *argc_ptr - n;
    bcopy((char *) (remove + n), (char *) (remove),
	  sizeof(*remove) * (*argc_ptr - (remove - argv) + 1));
}

Xv_public int
xv_parse_cmdline(app_name, argc_ptr, argv_base, scrunch)
    char           *app_name;
    int            *argc_ptr;
    char          **argv_base;
    int             scrunch;
/*
 * Parse the command line, looking for sv flags.  Abort if a partial flag is
 * encountered, but just ignore unrecognized flags. If scrunch, remove
 * recognized flags (and their arguments) from the command line (argv) and
 * adjust the command count (argc_ptr).
 */
{
    register char **argv = argv_base;
    register int    argc = *argc_ptr;
    int             n;

    while (argc > 0) {
	switch ((n = xv_parse_one(app_name, argc, argv))) {
	  case 0:		/* Unrecognized flag: ignore it */
	    argc--;
	    argv++;
	    break;
	  case -1:
#ifdef _XV_DEBUG
	    /* Always print debugging flags, when #define'd. */
	    xv_generic_debug_help(stderr);
#endif
	    return (-1);
	  default:
	    if (scrunch) {
		xv_cmdline_scrunch(argc_ptr, argv_base, argv, n);
	    } else
		argv += n;
	    argc -= n;
	}
    }
    return (0);
}

int
xv_parse_one(app_name, argc, argv)
    char           *app_name;
    register int    argc;
    register char **argv;
{
    int             plus;
    int             bad_arg = 0;
    register Cmd_line_flag *slot;
    Flag_name       flag_name;

    if (argc < 1 || ((**argv != '-') && (**argv != '+')))
	return (0);

    slot = find_cmd_flag(argv[0]);

    if (!slot)
	return 0;

    if (argc <= slot->num_args) {
	char            dummy[128];

#ifdef OW_I18N
	(void) sprintf(dummy,
			XV_I18N_MSG("xv_messages","%s: missing argument after %s"),
			   app_name, argv[0]);
#else
	(void) sprintf(dummy, "%s: missing argument after %s",
			   app_name, argv[0]);
#endif OW_I18N
	xv_error(NULL,
		 ERROR_STRING, dummy,
		 0);

	return (-1);
    }
    flag_name = (Flag_name) (slot - cmd_line_flags);
    switch (flag_name) {

      case FLAG_LC_BASIC_LOCALE:
      case FLAG_LC_DISPLAY_LANG:
      case FLAG_LC_INPUT_LANG:
      case FLAG_LC_NUMERIC:
      case FLAG_LC_TIME_FORMAT:
	break;

      case FLAG_WIDTH:
      case FLAG_HEIGHT:
	if ((plus = atoi(argv[1])) < 0) {
	    bad_arg = 1;
	    goto NegArg;
	}
	defaults_set_integer(slot->def_name[0], plus);
	break;

      case FLAG_SIZE:
	if ((plus = atoi(argv[1])) < 0) {
	    bad_arg = 1;
	    goto NegArg;
	}
	defaults_set_integer(slot->def_name[0], plus);
	if ((plus = atoi(argv[2])) < 0) {
	    bad_arg = 2;
	    goto NegArg;
	}
	defaults_set_integer(slot->def_name[1], plus);
	break;

      case FLAG_POSITION:
      case FLAG_ICON_POSITION:
	defaults_set_integer(slot->def_name[0], atoi(argv[1]));
	defaults_set_integer(slot->def_name[1], atoi(argv[2]));
	break;

      case FLAG_GEOMETRY:
	defaults_set_string(slot->def_name[0], argv[1]);
	break;
	
      case FLAG_LABEL:
	defaults_set_string(slot->def_name[0], argv[1]);
	break;

      case FLAG_TITLE:
	defaults_set_string(slot->def_name[0], argv[1]);
	break;
	
      case FLAG_ICON_LABEL:
	defaults_set_string(slot->def_name[0], argv[1]);
	break;

      case FLAG_ICON_IMAGE:
	defaults_set_string(slot->def_name[0], argv[1]);
	break;

      case FLAG_ICON_FONT:
	defaults_set_string(slot->def_name[0], argv[1]);
	break;

      case FLAG_FONT:
	/* this is a hack to allow for Xt -fn default */
      case FLAG_X_FONT:
	defaults_set_string(slot->def_name[0], argv[1]);
	break;
	
      case FLAG_SCALE:
	if (defaults_lookup(argv[1], known_scales) == -1) {
		char dummy[1024];
		
#ifdef OW_I18N
		(void) sprintf(dummy,
			XV_I18N_MSG("xv_messages",
				 "%s: unknown scale \"%s\" used with %s option"),
			       app_name, argv[1], argv[0]);
#else
		(void) sprintf(dummy,
			"%s: unknown scale \"%s\" used with %s option",
			       app_name, argv[1], argv[0]);
#endif OW_I18N
		xv_error(XV_NULL,
		     ERROR_STRING, dummy,
			 0);
		return(-1);
	} else
	  defaults_set_string(slot->def_name[0], argv[1]);
	break;

      case FLAG_ENABLE_ICONIC:
	defaults_set_boolean(slot->def_name[0], TRUE);
	break;

      case FLAG_DISABLE_ICONIC:
	defaults_set_boolean(slot->def_name[0], FALSE);
	break;
	
      case FLAG_SET_DEFAULT_COLOR:
	/* this is really just a no op, but we need to consume it
	 * so that old applications won't see it.
	 */
	/* boolean value -- if specified then TRUE */
	defaults_set_boolean(slot->def_name[0], TRUE);
	break;

#ifdef REVERSE_VIDEO
      case FLAG_ENABLE_REVERSE:
	defaults_set_boolean(slot->def_name[0], TRUE);
	break;

      case FLAG_DISABLE_REVERSE:
	defaults_set_boolean(slot->def_name[0], FALSE);
	break;
#endif /* REVERSE_VIDEO */

      case FLAG_DISABLE_RETAINED:
	/* boolean value -- if specified then TRUE */
	defaults_set_integer(slot->def_name[0], TRUE);
	break;

      case FLAG_DISABLE_XIO_ERROR_HANDLER:
	/* boolean value -- if specified then  */
	(void) XSetIOErrorHandler((int (*) ()) NULL);
	break;

      case FLAG_FULLSCREEN_DEBUG:
	fullscreendebug = 1;
	break;

      case FLAG_FULLSCREEN_DEBUG_SERVER:
	fullscreendebugserver = 1;
	break;

      case FLAG_FULLSCREEN_DEBUG_PTR:
	fullscreendebugptr = 1;
	break;

      case FLAG_FULLSCREEN_DEBUG_KBD:
	fullscreendebugkbd = 1;
	break;

      case FLAG_FOREGROUND_COLOR:
      case FLAG_BACKGROUND_COLOR:{
	  int             i, rgb[3];
	  char            chars[100];
	  
	  /* convert three ints into one string with three RGB values */
	  for (i = 0; i <= 2; i++) {
	      /* if bad number or neg. then use 0 */
	      if ((sscanf(argv[i + 1], "%d", &(rgb[i])) != 1) ||
		  (rgb[i] < 0))
		rgb[i] = 0;
	  }
	  (void) sprintf(chars, "%d %d %d", rgb[0], rgb[1], rgb[2]);
	  defaults_set_string(slot->def_name[0], chars);
	  break;
      }
	
      case FLAG_X_FOREGROUND_COLOR:
      case FLAG_X_BACKGROUND_COLOR:
	defaults_set_string(slot->def_name[0], argv[1]);
	break;
	
      case FLAG_DEFAULTS_ENTRY:
	defaults_set_string(argv[1], argv[2]);
	break;
	
      case FLAG_XRM_ENTRY: {
	      char resource[1000], value[1000];
	      int i = 0;

	      /* split the argv in the form of "resource:value"
	       * into two different strings to pass into defaults_set_string
	       */
	      while (argv[1][i] != ':' && argv[1][i] != '\0') {
		      resource[i] = argv[1][i];
		      i++;
	      }
	      resource[i] = '\0';
	      if (argv[1][i] == ':') {
		      while (argv[1][i] != '\0') {
			      i++;
			      value[i] = argv[1][i];
		      }
		      defaults_set_string(resource, value);
	      }
      }
	break;

      case FLAG_HELP:
	return (-1);

      case FLAG_ENABLE_SYNC:
	defaults_set_boolean(slot->def_name[0], TRUE);
	break;

      case FLAG_DISABLE_SYNC:
	defaults_set_boolean(slot->def_name[0], FALSE);
	break;
	
      case FLAG_SERVER:
	defaults_set_string(slot->def_name[0], argv[1]);
	break;

      case FLAG_NO_SECURITY:
	defeat_event_security = 1;
	break;

#ifdef _XV_DEBUG
      case FLAG_LIST_FONTS:
	list_fonts = TRUE;
	break;

      case FLAG_DEBUG:
	plus = atoi(argv[1]);
SetDebug:
	if (xv_set_debug_flag(plus, TRUE)) {
	    char            dummy[128];

#ifdef OW_I18N
	    (void) sprintf(dummy,
		XV_I18N_MSG("xv_messages",
			"xv_set_debug_flag; '%d' is out of bounds"), plus);
#else
	    (void) sprintf(dummy, "xv_set_debug_flag; '%d' is out of bounds",
		plus);
#endif OW_I18N
	    xv_error(NULL,
		     ERROR_STRING, dummy,
		     0);
	}
	break;
      case FLAG_STATS:
	server_gather_stats = TRUE;
	break;
      case FLAG_GPROF_START:
	sview_gprof_start = TRUE;
	break;
#endif

      default:
	return (0);

    }

    return (slot->num_args + 1);

/* BadFont:
    {
	char            dummy[128];

#ifdef OW_I18N
	(void) sprintf(dummy, XV_I18N_MSG("xv_messages","%s: bad font file (%s)"),
			app_name, argv[1]);
#else
	(void) sprintf(dummy, "%s: bad font file (%s)",
			app_name, argv[1]);
#endif OW_I18N
	xv_error(NULL,
		 ERROR_STRING, dummy,
		 0);
	return (-1);
    } */

NegArg:
    {
	char            dummy[128];

#ifdef OW_I18N
	(void) sprintf(dummy,
		XV_I18N_MSG("xv_messages",
			 "%s: can't have negative argument %s after %s"), app_name, argv[bad_arg], argv[0]);
#else
	(void) sprintf(dummy, "%s: can't have negative argument %s after %s",
			 app_name, argv[bad_arg], argv[0]);
#endif OW_I18N
	xv_error(XV_NULL,
		 ERROR_STRING, dummy,
		 0);
	return (-1);
    }
	    
/* NoMsgError:
    return (-1); */
} 

static Cmd_line_flag *
find_cmd_flag(key)
    register char  *key;
{
    register Cmd_line_flag *slot = cmd_line_flags;

    for (slot = cmd_line_flags; slot->short_name; slot++)
	if ((strcmp(key, slot->short_name) == 0) ||
	    (strcmp(key, slot->long_name) == 0))
	    return slot;
    return 0;
}

#ifdef _XV_DEBUG
static int
xv_generic_debug_help(fd)
    int             fd;
{
#ifdef OW_I18N
    (void) fprintf(fd,
	XV_I18N_MSG("xv_messages", "Generic debugging flags are:\n\
	(-sync)			bool	(run synchronous with server)\n\
	(-list_fonts)\n\
	(-stats)\n\
	(-Xv_debug)		unsigned\n"));
#else 
    (void) fprintf(fd, "Generic debugging flags are:\n\
	(-sync)			bool	(run synchronous with server)\n\
	(-list_fonts)\n\
	(-stats)\n\
	(-Xv_debug)		unsigned\n");
#endif OW_I18N
}

#endif
